/**
 * @file NetworkManager.cpp
 * @brief Implementação do gerenciador de rede seguro para WYDBR 2.0
 * 
 * Este arquivo implementa o gerenciamento de conexões de rede com segurança
 * aprimorada, detecção de exploits e prevenção contra ataques.
 */

#include "../../include/Network/NetworkManager.h"
#include "../../include/Network/PacketHandler.h"
#include "../../include/Network/CryptoManager.h"
#include "../../include/Security/SecurityManager.h"
#include <cstring>
#include <chrono>
#include <thread>
#include <random>
#include <iostream>

namespace WYDBR {
namespace Network {

NetworkManager::NetworkManager() 
    : is_initialized_(false), 
      server_socket_(-1), 
      max_connections_(500),
      port_(8281), 
      backlog_(10),
      running_(false) {
    // Inicializar tabela de conexões
    connections_.resize(max_connections_);
    for (int i = 0; i < max_connections_; i++) {
        connections_[i].socket = -1;
        connections_[i].state = ConnectionState::Disconnected;
    }
}

NetworkManager::~NetworkManager() {
    Shutdown();
}

NetworkManager& NetworkManager::GetInstance() {
    static NetworkManager instance;
    return instance;
}

bool NetworkManager::Initialize(uint16_t port, int max_connections) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (is_initialized_) {
        return true;
    }
    
    port_ = port;
    max_connections_ = max_connections;
    connections_.resize(max_connections_);
    
    // Inicializar o subsistema de criptografia
    if (!CryptoManager::GetInstance().Initialize()) {
        std::cerr << "Falha ao inicializar CryptoManager" << std::endl;
        return false;
    }
    
    // Inicializar o gerenciador de segurança
    if (!Security::SecurityManager::GetInstance().Initialize("security_config.json")) {
        std::cerr << "Falha ao inicializar SecurityManager" << std::endl;
        return false;
    }
    
    // Inicializar subsistema de socket (cross-platform)
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Falha ao inicializar Winsock" << std::endl;
        return false;
    }
#endif
    
    // Criar socket do servidor
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        std::cerr << "Falha ao criar socket do servidor" << std::endl;
        return false;
    }
    
    // Configurar socket reutilizável
    int opt = 1;
#ifdef _WIN32
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
#else
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
#endif
        std::cerr << "Falha ao configurar socket reutilizável" << std::endl;
        CloseSocket(server_socket_);
        return false;
    }
    
    // Configurar timeout (recv/send)
    struct timeval timeout;
    timeout.tv_sec = 30;  // 30 segundos de timeout
    timeout.tv_usec = 0;
    
#ifdef _WIN32
    if (setsockopt(server_socket_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        std::cerr << "Falha ao configurar timeout de recepção" << std::endl;
        CloseSocket(server_socket_);
        return false;
    }
    
    if (setsockopt(server_socket_, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        std::cerr << "Falha ao configurar timeout de envio" << std::endl;
        CloseSocket(server_socket_);
        return false;
    }
#else
    if (setsockopt(server_socket_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Falha ao configurar timeout de recepção" << std::endl;
        CloseSocket(server_socket_);
        return false;
    }
    
    if (setsockopt(server_socket_, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Falha ao configurar timeout de envio" << std::endl;
        CloseSocket(server_socket_);
        return false;
    }
#endif
    
    // Configurar endereço
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    
    // Vincular socket
    if (bind(server_socket_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Falha ao vincular socket: " << port_ << std::endl;
        CloseSocket(server_socket_);
        return false;
    }
    
    // Configurar para escuta
    if (listen(server_socket_, backlog_) < 0) {
        std::cerr << "Falha ao configurar socket para escuta" << std::endl;
        CloseSocket(server_socket_);
        return false;
    }
    
    is_initialized_ = true;
    return true;
}

void NetworkManager::Start() {
    if (!is_initialized_) {
        std::cerr << "NetworkManager não está inicializado" << std::endl;
        return;
    }
    
    running_ = true;
    
    // Iniciar thread principal de aceitação
    accept_thread_ = std::thread(&NetworkManager::AcceptConnectionsThread, this);
    
    // Iniciar thread de processamento
    process_thread_ = std::thread(&NetworkManager::ProcessConnectionsThread, this);
    
    std::cout << "NetworkManager iniciado na porta " << port_ << std::endl;
}

void NetworkManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!is_initialized_) {
        return;
    }
    
    running_ = false;
    
    // Aguardar threads terminarem
    if (accept_thread_.joinable()) {
        accept_thread_.join();
    }
    
    if (process_thread_.joinable()) {
        process_thread_.join();
    }
    
    // Fechar todas as conexões
    for (auto& conn : connections_) {
        if (conn.state != ConnectionState::Disconnected) {
            CloseConnection(conn);
        }
    }
    
    // Fechar socket do servidor
    if (server_socket_ >= 0) {
        CloseSocket(server_socket_);
        server_socket_ = -1;
    }
    
#ifdef _WIN32
    WSACleanup();
#endif
    
    is_initialized_ = false;
    std::cout << "NetworkManager desligado" << std::endl;
}

void NetworkManager::AcceptConnectionsThread() {
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        // Aceitar nova conexão
        int client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            // Verificar se o servidor está sendo desligado
            if (!running_) {
                break;
            }
            
            // Erro temporário, continuar
#ifdef _WIN32
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
#endif
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            
            std::cerr << "Erro ao aceitar conexão" << std::endl;
            continue;
        }
        
        // Obter endereço IP do cliente
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        
        // Verificar se o IP está banido
        if (Security::SecurityManager::GetInstance().IsIPBanned(client_ip)) {
            std::cout << "Conexão rejeitada de IP banido: " << client_ip << std::endl;
            CloseSocket(client_socket);
            continue;
        }
        
        // Armazenar a conexão
        std::lock_guard<std::mutex> lock(mutex_);
        int conn_index = -1;
        
        // Encontrar slot disponível
        for (int i = 0; i < max_connections_; i++) {
            if (connections_[i].state == ConnectionState::Disconnected) {
                conn_index = i;
                break;
            }
        }
        
        if (conn_index == -1) {
            // Sem slots disponíveis
            std::cerr << "Limite de conexões atingido, rejeitando: " << client_ip << std::endl;
            CloseSocket(client_socket);
            continue;
        }
        
        // Configurar a nova conexão
        ConnectionInfo& conn = connections_[conn_index];
        conn.socket = client_socket;
        conn.state = ConnectionState::Connected;
        conn.last_activity = std::chrono::steady_clock::now();
        strncpy(conn.ip_address, client_ip, sizeof(conn.ip_address) - 1);
        conn.ip_address[sizeof(conn.ip_address) - 1] = '\0';
        
        // Inicializar buffer de recebimento
        conn.recv_buffer_size = 0;
        conn.packet_size = 0;
        
        // Gerar chave de criptografia para esta conexão
        conn.encryption_key = CryptoManager::GetInstance().GenerateSessionKey();
        
        std::cout << "Nova conexão aceita [" << conn_index << "]: " << client_ip << std::endl;
    }
}

void NetworkManager::ProcessConnectionsThread() {
    while (running_) {
        // Processar todas as conexões ativas
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            for (int i = 0; i < max_connections_; i++) {
                ConnectionInfo& conn = connections_[i];
                
                if (conn.state == ConnectionState::Disconnected) {
                    continue;
                }
                
                // Verificar timeout de inatividade (2 minutos)
                auto now = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - conn.last_activity).count();
                
                if (duration > 120) {
                    std::cout << "Conexão expirada [" << i << "]: " << conn.ip_address << std::endl;
                    CloseConnection(conn);
                    continue;
                }
                
                // Verificar se há dados para receber
                if (conn.state == ConnectionState::Connected) {
                    ProcessIncomingData(i);
                }
            }
        }
        
        // Aguardar um pouco para não sobrecarregar a CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void NetworkManager::ProcessIncomingData(int conn_index) {
    ConnectionInfo& conn = connections_[conn_index];
    
    // Calcular espaço disponível no buffer
    int available_space = sizeof(conn.recv_buffer) - conn.recv_buffer_size;
    
    if (available_space <= 0) {
        // Buffer cheio, possível ataque - desconectar
        std::cerr << "Buffer de recepção cheio, possível ataque de: " << conn.ip_address << std::endl;
        Security::SecurityManager::GetInstance().RegisterAttackAttempt(
            "BUFFER_OVERFLOW", 3, conn_index, "Receive buffer full"
        );
        
        CloseConnection(conn);
        return;
    }
    
    // Receber dados
    int bytes_received = recv(conn.socket, 
                             conn.recv_buffer + conn.recv_buffer_size, 
                             available_space, 
                             0);
    
    if (bytes_received <= 0) {
        // Erro ou conexão fechada
#ifdef _WIN32
        if (bytes_received == 0 || WSAGetLastError() != WSAEWOULDBLOCK) {
#else
        if (bytes_received == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
#endif
            // Conexão fechada ou erro
            if (bytes_received == 0) {
                std::cout << "Conexão fechada pelo cliente [" << conn_index << "]: " << conn.ip_address << std::endl;
            } else {
                std::cerr << "Erro ao receber dados [" << conn_index << "]: " << conn.ip_address << std::endl;
            }
            
            CloseConnection(conn);
        }
        return;
    }
    
    // Atualizar timestamp de atividade
    conn.last_activity = std::chrono::steady_clock::now();
    
    // Atualizar tamanho do buffer
    conn.recv_buffer_size += bytes_received;
    
    // Processar pacotes completos
    while (conn.recv_buffer_size >= sizeof(PacketHeader)) {
        // Se ainda não lemos o tamanho do pacote, ler agora
        if (conn.packet_size == 0) {
            // Ler cabeçalho
            PacketHeader* header = reinterpret_cast<PacketHeader*>(conn.recv_buffer);
            
            // Verificar tamanho de pacote válido
            if (header->size < sizeof(PacketHeader) || header->size > MAX_PACKET_SIZE) {
                // Tamanho inválido, possível ataque
                std::cerr << "Tamanho de pacote inválido de: " << conn.ip_address << std::endl;
                Security::SecurityManager::GetInstance().RegisterAttackAttempt(
                    "INVALID_PACKET_SIZE", 4, conn_index, 
                    "Invalid packet size: " + std::to_string(header->size)
                );
                
                CloseConnection(conn);
                return;
            }
            
            conn.packet_size = header->size;
        }
        
        // Verificar se temos um pacote completo
        if (conn.recv_buffer_size < conn.packet_size) {
            // Pacote incompleto, aguardar mais dados
            break;
        }
        
        // Processar o pacote
        ProcessPacket(conn_index, conn.recv_buffer, conn.packet_size);
        
        // Remover o pacote processado do buffer
        if (conn.recv_buffer_size > conn.packet_size) {
            // Mover os dados restantes para o início do buffer
            memmove(conn.recv_buffer, 
                    conn.recv_buffer + conn.packet_size, 
                    conn.recv_buffer_size - conn.packet_size);
        }
        
        conn.recv_buffer_size -= conn.packet_size;
        conn.packet_size = 0;
    }
}

void NetworkManager::ProcessPacket(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    ConnectionInfo& conn = connections_[conn_index];
    
    // Verificar tamanho mínimo
    if (packet_size < sizeof(PacketHeader)) {
        std::cerr << "Pacote muito pequeno de: " << conn.ip_address << std::endl;
        return;
    }
    
    // Decodificar o pacote (se necessário)
    uint8_t decoded_packet[MAX_PACKET_SIZE];
    memcpy(decoded_packet, packet_data, packet_size);
    
    if (conn.encryption_enabled) {
        CryptoManager::GetInstance().Decrypt(
            decoded_packet + sizeof(PacketHeader), 
            packet_size - sizeof(PacketHeader),
            conn.encryption_key
        );
    }
    
    // Acessar o cabeçalho
    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(decoded_packet);
    
    // Verificar checksum
    if (!header->VerifyChecksum(
            decoded_packet + sizeof(PacketHeader), 
            packet_size - sizeof(PacketHeader))) {
        std::cerr << "Checksum inválido de: " << conn.ip_address << std::endl;
        Security::SecurityManager::GetInstance().RegisterAttackAttempt(
            "INVALID_CHECKSUM", 3, conn_index, "Packet checksum verification failed"
        );
        return;
    }
    
    // Verificar timestamp (anti-replay)
    uint32_t current_time = static_cast<uint32_t>(std::time(nullptr));
    if (header->timestamp > 0 && 
        (current_time < header->timestamp || current_time - header->timestamp > 30)) {
        std::cerr << "Timestamp inválido, possível replay attack de: " << conn.ip_address << std::endl;
        Security::SecurityManager::GetInstance().RegisterAttackAttempt(
            "INVALID_TIMESTAMP", 4, conn_index, "Possible replay attack"
        );
        return;
    }
    
    // Verificar opcode válido
    PacketType opcode = static_cast<PacketType>(header->opcode);
    if (!IsValidOpcode(opcode)) {
        std::cerr << "Opcode inválido de: " << conn.ip_address << std::endl;
        Security::SecurityManager::GetInstance().RegisterAttackAttempt(
            "INVALID_OPCODE", 3, conn_index, 
            "Invalid opcode: " + std::to_string(header->opcode)
        );
        return;
    }
    
    // Processar pacote com o handler apropriado
    PacketHandler::ProcessPacket(conn_index, opcode, decoded_packet, packet_size);
}

bool NetworkManager::SendPacket(int conn_index, const void* packet_data, uint16_t packet_size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (conn_index < 0 || conn_index >= max_connections_) {
        std::cerr << "Índice de conexão inválido: " << conn_index << std::endl;
        return false;
    }
    
    ConnectionInfo& conn = connections_[conn_index];
    
    if (conn.state != ConnectionState::Connected) {
        std::cerr << "Tentativa de enviar para conexão fechada: " << conn_index << std::endl;
        return false;
    }
    
    if (packet_size < sizeof(PacketHeader) || packet_size > MAX_PACKET_SIZE) {
        std::cerr << "Tamanho de pacote inválido para envio: " << packet_size << std::endl;
        return false;
    }
    
    // Criar cópia do pacote para processamento
    uint8_t send_buffer[MAX_PACKET_SIZE];
    memcpy(send_buffer, packet_data, packet_size);
    
    // Acessar o cabeçalho
    PacketHeader* header = reinterpret_cast<PacketHeader*>(send_buffer);
    
    // Definir tamanho correto e timestamp
    header->size = packet_size;
    header->timestamp = static_cast<uint32_t>(std::time(nullptr));
    
    // Calcular checksum
    header->CalculateChecksum(
        send_buffer + sizeof(PacketHeader), 
        packet_size - sizeof(PacketHeader)
    );
    
    // Criptografar o pacote (se necessário)
    if (conn.encryption_enabled) {
        CryptoManager::GetInstance().Encrypt(
            send_buffer + sizeof(PacketHeader), 
            packet_size - sizeof(PacketHeader),
            conn.encryption_key
        );
    }
    
    // Enviar o pacote
    int bytes_sent = send(conn.socket, reinterpret_cast<const char*>(send_buffer), packet_size, 0);
    
    if (bytes_sent != packet_size) {
        std::cerr << "Erro ao enviar pacote para [" << conn_index << "]: " << conn.ip_address << std::endl;
        
        // Verificar erro fatal
#ifdef _WIN32
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
#endif
            CloseConnection(conn);
        }
        
        return false;
    }
    
    // Atualizar timestamp de atividade
    conn.last_activity = std::chrono::steady_clock::now();
    
    return true;
}

void NetworkManager::CloseConnection(ConnectionInfo& conn) {
    if (conn.socket >= 0) {
        CloseSocket(conn.socket);
        conn.socket = -1;
    }
    
    conn.state = ConnectionState::Disconnected;
    conn.recv_buffer_size = 0;
    conn.packet_size = 0;
    conn.encryption_enabled = false;
}

void NetworkManager::CloseSocket(int socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

bool NetworkManager::IsValidOpcode(PacketType opcode) {
    // Verificar se o opcode está dentro dos limites válidos
    uint16_t code = static_cast<uint16_t>(opcode);
    
    // Verificar pacotes do cliente
    if (code >= static_cast<uint16_t>(PacketType::C_LOGIN) &&
        code <= static_cast<uint16_t>(PacketType::C_ADMIN_COMMAND)) {
        return true;
    }
    
    // Verificar pacotes do servidor
    if (code >= static_cast<uint16_t>(PacketType::S_LOGIN_RESULT) &&
        code <= static_cast<uint16_t>(PacketType::S_SHOP_CLOSE)) {
        return true;
    }
    
    return false;
}

void NetworkManager::EnableEncryption(int conn_index, bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (conn_index >= 0 && conn_index < max_connections_) {
        ConnectionInfo& conn = connections_[conn_index];
        conn.encryption_enabled = enable;
    }
}

bool NetworkManager::IsConnectionActive(int conn_index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (conn_index >= 0 && conn_index < max_connections_) {
        return connections_[conn_index].state == ConnectionState::Connected;
    }
    
    return false;
}

const std::string NetworkManager::GetClientIP(int conn_index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (conn_index >= 0 && conn_index < max_connections_ && 
        connections_[conn_index].state == ConnectionState::Connected) {
        return connections_[conn_index].ip_address;
    }
    
    return "";
}

} // namespace Network
} // namespace WYDBR