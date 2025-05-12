/**
 * DBServerConnector.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/database/DBServerConnector.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/database/DBServerConnector.h"
#include "src/server/tmsrv/TMServer.h"

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <cstring>

// Para sockets (interface de rede)
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    #define SOCK_ERRNO WSAGetLastError()
    #define SOCK_EINTR WSAEINTR
    #define SOCK_EAGAIN WSAEWOULDBLOCK
    #define SOCK_CLOSE closesocket
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>

namespace wydbr {

    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define SOCK_ERRNO errno
    #define SOCK_EINTR EINTR
    #define SOCK_EAGAIN EAGAIN
    #define SOCK_CLOSE close
#endif

/**
 * Implementação da classe DBPacket
 */

// Constructor com apenas operação
DBPacket::DBPacket(DBOperation operation)
    : operation_(operation), sequence_(0), result_(DBResult::SUCCESS) {
}

// Constructor com operação e dados
DBPacket::DBPacket(DBOperation operation, const std::string& data)
    : operation_(operation), sequence_(0), data_(data), result_(DBResult::SUCCESS) {
}

// Serializa o pacote para transmissão
std::string DBPacket::serialize() const {
    // Formato:
    // [2 bytes] Tamanho total do pacote
    // [2 bytes] Código de operação
    // [4 bytes] Número de sequência
    // [1 byte]  Código de resultado
    // [n bytes] Dados (resto do pacote)
    
    // Calcula tamanho total: 9 bytes de cabeçalho + tamanho dos dados
    uint16_t totalSize = 9 + static_cast<uint16_t>(data_.size());
    
    // Cria buffer para pacote completo
    std::string packet;
    packet.resize(totalSize);
    
    // Escreve campos do cabeçalho
    packet[0] = totalSize & 0xFF;
    packet[1] = (totalSize >> 8) & 0xFF;
    packet[2] = static_cast<uint16_t>(operation_) & 0xFF;
    packet[3] = (static_cast<uint16_t>(operation_) >> 8) & 0xFF;
    packet[4] = sequence_ & 0xFF;
    packet[5] = (sequence_ >> 8) & 0xFF;
    packet[6] = (sequence_ >> 16) & 0xFF;
    packet[7] = (sequence_ >> 24) & 0xFF;
    packet[8] = static_cast<uint8_t>(result_);
    
    // Copia os dados
    if (!data_.empty()) {
        std::memcpy(&packet[9], data_.data(), data_.size());
    }
    
    return packet;
}

// Desserializa um pacote a partir dos dados recebidos
DBPacket DBPacket::deserialize(const std::string& data) {
    if (data.size() < 9) {
        throw std::runtime_error("Pacote de dados muito pequeno para ser um pacote DBPacket válido");
    }
    
    // Lê campos do cabeçalho
    uint16_t totalSize = static_cast<uint8_t>(data[0]) | (static_cast<uint8_t>(data[1]) << 8);
    uint16_t opCode = static_cast<uint8_t>(data[2]) | (static_cast<uint8_t>(data[3]) << 8);
    uint32_t sequence = static_cast<uint8_t>(data[4]) | 
                        (static_cast<uint8_t>(data[5]) << 8) | 
                        (static_cast<uint8_t>(data[6]) << 16) | 
                        (static_cast<uint8_t>(data[7]) << 24);
    uint8_t resultCode = static_cast<uint8_t>(data[8]);
    
    // Verifica tamanho total
    if (data.size() != totalSize) {
        throw std::runtime_error("Tamanho de pacote inválido");
    }
    
    // Cria pacote com operação
    DBPacket packet(static_cast<DBOperation>(opCode));
    
    // Define outros campos
    packet.setSequence(sequence);
    packet.setResult(static_cast<DBResult>(resultCode));
    
    // Extrai dados, se /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 9 Parâmetro 9
 * @return Retorna houver
 */
 houver
    if(totalSize > 9){
        packet.setData(data.substr(9));
    }
    
    return packet;
}

/**
 * Implementação de DBServerConnector
 */

// Constructor
DBServerConnector::DBServerConnector(TMServer* server)
    : WYDSystemManager(server), 
      threadRunning_(false), 
      sequenceCounter_(1),
      lastConnectionCheck_(std::chrono::steady_clock::now()) {
}

// Destructor
DBServerConnector::~DBServerConnector() {
    // Garantir que o thread seja finalizado /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 * @return Retorna corretamente
 */
 corretamente
    shutdown();
}

// Inicialização
bool DBServerConnector::initialize() {
    std::cout << "[DBServerConnector] Inicializando..." << std::endl;
    
    // Inicializa a conexão
    connection_ = std::make_unique<DBServerConnection>();
    
    // Inicia o thread de processamento
    threadRunning_ = true;
    processingThread_ = std::thread(&DBServerConnector::processingLoop, this);
    
    return true;
}

// Atualização
void DBServerConnector::update(uint64_t timestamp) {
    // Verifica operações que atingiram /**
 * cleanupTimedOutOperations
 * 
 * Implementa a funcionalidade cleanupTimedOutOperations conforme especificação original.
 * @return Retorna timeout
 */
 timeout
    cleanupTimedOutOperations();
    
    // Verifica periodicamente a conexão (a cada 30 segundos)
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastConnectionCheck_).count();
    
    if (elapsed >= 30) {
        checkConnection();
        lastConnectionCheck_ = now;
    }
}

// Desligamento
void DBServerConnector::shutdown() {
    std::cout << "[DBServerConnector] Desligando..." << std::endl;
    
    // Finaliza o thread de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param threadRunning_ Parâmetro threadRunning_
 * @return Retorna processamento
 */
 processamento
    if(threadRunning_){
        threadRunning_ = false;
        
        // Notifica o thread para acordar, caso esteja esperando
        sendCondition_.notify_all();
        
        // Aguarda finalização do thread
        if (processingThread_.joinable()) {
            processingThread_.join();
        }
    }
    
    // Desconecta do /**
 * disconnect
 * 
 * Implementa a funcionalidade disconnect conforme especificação original.
 * @return Retorna servidor
 */
 servidor
    disconnect();
}

// Conecta ao servidor de banco de dados
bool DBServerConnector::connect(const std::string& host, uint16_t port) {
    std::lock_guard<std::mutex> lock(operationMutex_);
    
    if (!connection_) {
        std::cerr << "[DBServerConnector] Erro: conexão não inicializada" << std::endl;
        return false;
    }
    
    std::cout << "[DBServerConnector] Conectando a " << host << ":" << port << std::endl;
    
    if (connection_->connect(host, port)) {
        std::cout << "[DBServerConnector] Conexão estabelecida com sucesso" << std::endl;
        return true;
    } else {
        std::cerr << "[DBServerConnector] Falha ao conectar com o servidor" << std::endl;
        return false;
    }
}

// Desconecta do servidor
void DBServerConnector::disconnect() {
    if (connection_) {
        connection_->disconnect();
    }
}

// Verifica se está conectado
bool DBServerConnector::isConnected() const {
    return connection_ && connection_->isConnected();
}

// Executa uma operação no servidor de banco de dados
uint32_t DBServerConnector::executeOperation(
    DBOperation operation, 
    const std::string& data, 
    DBResponseCallback callback,
    uint32_t timeoutMs) {
    
    // Cria pacote
    /**
 * packet
 * 
 * Implementa a funcionalidade packet conforme especificação original.
 * @param operation Parâmetro operation
 * @param data Parâmetro data
 * @return Retorna DBPacket
 */

    DBPacket packet(operation, data);
    
    // Gera ID de sequência único
    uint32_t sequence = sequenceCounter_++;
    packet.setSequence(sequence);
    
    // Registra operação pendente, se houver /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param callback Parâmetro callback
 * @return Retorna callback
 */
 callback
    if(callback){
        std::lock_guard<std::mutex> lock(operationMutex_);
        
        PendingOperation pendingOp;
        pendingOp.operation = operation;
        pendingOp.sequence = sequence;
        pendingOp.startTime = std::chrono::steady_clock::now();
        pendingOp.timeoutMs = timeoutMs;
        pendingOp.callback = callback;
        
        pendingOperations_[sequence] = pendingOp;
    }
    
    // Adiciona pacote à fila de envio
    {
        std::lock_guard<std::mutex> lock(sendMutex_);
        sendQueue_.push(packet);
    }
    
    // Notifica thread de processamento
    sendCondition_.notify_one();
    
    return sequence;
}

// Autentica uma conta (wrapper para a operação)
bool DBServerConnector::authenticateAccount(
    const std::string& username, 
    const std::string& password, 
    DBResponseCallback callback) {
    
    // Em uma implementação real, deveríamos serializar os parâmetros
    // em um formato específico (ex: JSON, protobuf, etc.)
    // Aqui usaremos um formato simples delimitado por | para exemplo
    
    std::ostringstream oss;
    oss << username << "|" << password;
    
    // Executa a operação
    executeOperation(DBOperation::ACCOUNT_AUTH, oss.str(), callback);
    
    return true;
}

// Carrega um personagem
bool DBServerConnector::loadCharacter(
    uint32_t accountId, 
    const std::string& characterName, 
    DBResponseCallback callback) {
    
    std::ostringstream oss;
    oss << accountId << "|" << characterName;
    
    // Executa a operação
    executeOperation(DBOperation::CHAR_LOAD, oss.str(), callback);
    
    return true;
}

// Salva um personagem
bool DBServerConnector::saveCharacter(
    uint32_t accountId, 
    const std::string& characterData, 
    DBResponseCallback callback) {
    
    std::ostringstream oss;
    oss << accountId << "|" << characterData;
    
    // Executa a operação
    executeOperation(DBOperation::CHAR_SAVE, oss.str(), callback);
    
    return true;
}

// Obtém lista de personagens
bool DBServerConnector::getCharacterList(
    uint32_t accountId, 
    DBResponseCallback callback) {
    
    std::ostringstream oss;
    oss << accountId;
    
    // Executa a operação
    executeOperation(DBOperation::CHAR_LIST, oss.str(), callback);
    
    return true;
}

// Cria um personagem
bool DBServerConnector::createCharacter(
    uint32_t accountId, 
    const std::string& characterData, 
    DBResponseCallback callback) {
    
    std::ostringstream oss;
    oss << accountId << "|" << characterData;
    
    // Executa a operação
    executeOperation(DBOperation::CHAR_CREATE, oss.str(), callback);
    
    return true;
}

// Loop principal do thread de processamento
void DBServerConnector::processingLoop() {
    std::cout << "[DBServerConnector] Thread de processamento iniciado" << std::endl;
    
    while (threadRunning_) {
        // Processa pacotes da fila de envio
        /**
 * packetToSend
 * 
 * Implementa a funcionalidade packetToSend conforme especificação original.
 * @param DBOperation::SYS_SHUTDOWN Parâmetro DBOperation::SYS_SHUTDOWN
 * @return Retorna DBPacket
 */

        DBPacket packetToSend(DBOperation::SYS_SHUTDOWN); // Inicializa com valor dummy
        bool hasPacketToSend = false;
        
        {
            std::unique_lock<std::mutex> lock(sendMutex_);
            
            // Espera por pacotes para enviar ou sinal de shutdown
            sendCondition_.wait_for(lock, std::chrono::milliseconds(100), 
                [this]() { return !sendQueue_.empty() || !threadRunning_; });
            
            if (!threadRunning_) {
                break;
            }
            
            // Pega o próximo pacote da fila
            if (!sendQueue_.empty()) {
                packetToSend = sendQueue_.front();
                sendQueue_.pop();
                hasPacketToSend = true;
            }
        }
        
        // Envia o pacote, se houver
        if (hasPacketToSend && connection_ && connection_->isConnected()) {
            if (!connection_->send(packetToSend)) {
                std::cerr << "[DBServerConnector] Falha ao enviar pacote para DBServer" << std::endl;
                
                // Se falhou, tentamos reconectar
                // Em uma implementação real, usaríamos política de reconexão mais /**
 * disconnect
 * 
 * Implementa a funcionalidade disconnect conforme especificação original.
 * @return Retorna robusta
 */
 robusta
                disconnect();
                
                // Recoloca o pacote na fila para reenvio futuro
                std::lock_guard<std::mutex> lock(sendMutex_);
                sendQueue_.push(packetToSend);
                
                // Espera um pouco antes de tentar novamente
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
        }
        
        // Tenta receber resposta
        if (connection_ && connection_->isConnected()) {
            /**
 * response
 * 
 * Implementa a funcionalidade response conforme especificação original.
 * @param DBOperation::SYS_SHUTDOWN Parâmetro DBOperation::SYS_SHUTDOWN
 * @return Retorna DBPacket
 */

            DBPacket response(DBOperation::SYS_SHUTDOWN); // Inicializa com valor dummy
            
            if (connection_->receive(response, 50)) { // 50ms de timeout
                // Processa resposta /**
 * handleResponse
 * 
 * Implementa a funcionalidade handleResponse conforme especificação original.
 * @param response Parâmetro response
 * @return Retorna recebida
 */
 recebida
                handleResponse(response);
            }
        }
    }
    
    std::cout << "[DBServerConnector] Thread de processamento finalizado" << std::endl;
}

// Lida com uma resposta recebida
void DBServerConnector::handleResponse(const DBPacket& response) {
    // Busca operação pendente correspondente
    std::lock_guard<std::mutex> lock(operationMutex_);
    
    auto it = pendingOperations_.find(response.getSequence());
    if (it != pendingOperations_.end()) {
        // Encontrou operação pendente
        PendingOperation& op = it->second;
        
        // Executa /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param op.callback Parâmetro op.callback
 * @return Retorna callback
 */
 callback
        if(op.callback){
            op.callback(response, response.getResult() == DBResult::SUCCESS);
        }
        
        // Remove da lista de pendentes
        pendingOperations_.erase(it);
    } else {
        // Recebeu resposta para operação desconhecida
        std::cerr << "[DBServerConnector] Recebida resposta para operação desconhecida: " 
                  << static_cast<int>(response.getOperation()) 
                  << ", seq: " << response.getSequence() << std::endl;
    }
}

// Limpa operações que atingiram timeout
void DBServerConnector::cleanupTimedOutOperations() {
    std::lock_guard<std::mutex> lock(operationMutex_);
    
    auto now = std::chrono::steady_clock::now();
    
    // Para cada operação pendente
    auto it = pendingOperations_.begin();
    while (it != pendingOperations_.end()) {
        PendingOperation& op = it->second;
        
        // Calcula tempo decorrido
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - op.startTime).count();
        
        if (elapsed >= op.timeoutMs) {
            // Timeout atingido
            std::cerr << "[DBServerConnector] Operação atingiu timeout: "
                      << static_cast<int>(op.operation)
                      << ", seq: " << op.sequence << std::endl;
            
            // Cria pacote de resposta com erro de timeout
            /**
 * response
 * 
 * Implementa a funcionalidade response conforme especificação original.
 * @param op.operation Parâmetro op.operation
 * @return Retorna DBPacket
 */

            DBPacket response(op.operation);
            response.setSequence(op.sequence);
            response.setResult(DBResult::ERROR_TIMEOUT);
            
            // Executa /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param op.callback Parâmetro op.callback
 * @return Retorna callback
 */
 callback
            if(op.callback){
                op.callback(response, false);
            }
            
            // Remove da lista
            it = pendingOperations_.erase(it);
        } else {
            ++it;
        }
    }
}

// Verifica conexão periodicamente
void DBServerConnector::checkConnection() {
    if (!isConnected()) {
        std::cerr << "[DBServerConnector] Conexão perdida. Tentando reconectar..." << std::endl;
        
        // Em uma implementação real, usaríamos os parâmetros salvos
        // ou tentaríamos reconectar usando configuração do servidor
        // Por enquanto, apenas logamos o evento
    }
}

/**
 * Implementação de DBServerConnection
 */

// Constructor
DBServerConnection::DBServerConnection() 
    : socket_(INVALID_SOCKET), port_(0), connected_(false) {
    
    // Inicializa biblioteca de sockets no Windows
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[DBServerConnection] Falha ao inicializar WinSock" << std::endl;
    }
    #endif
}

// Destructor
DBServerConnection::~DBServerConnection() {
    // Desconecta, se necessá/**
 * disconnect
 * 
 * Implementa a funcionalidade disconnect conforme especificação original.
 * @return Retorna rio
 */
rio
    disconnect();
    
    // Encerra biblioteca de sockets no Windows
    #ifdef /**
 * WSACleanup
 * 
 * Implementa a funcionalidade WSACleanup conforme especificação original.
 * @return Retorna _WIN32
 */
 _WIN32
    WSACleanup();
    #endif
}

// Conecta ao servidor
bool DBServerConnection::connect(const std::string& host, uint16_t port) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Já está conectado?
    if (connected_) {
        disconnect();
    }
    
    // Cria socket
    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        std::cerr << "[DBServerConnection] Falha ao criar socket: " << SOCK_ERRNO << std::endl;
        return false;
    }
    
    // Configura endereço do servidor
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    // Converte endereço
    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "[DBServerConnection] Endereço inválido ou não suportado: " << host << std::endl;
        SOCK_CLOSE(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }
    
    // Conecta ao servidor
    if (::connect(socket_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[DBServerConnection] Falha ao conectar: " << SOCK_ERRNO << std::endl;
        SOCK_CLOSE(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }
    
    // Configura socket como não-bloqueante
    #ifdef _WIN32
    u_long mode = 1;
    if (ioctlsocket(socket_, FIONBIO, &mode) != 0) {
        std::cerr << "[DBServerConnection] Falha ao configurar socket como não-bloqueante" << std::endl;
        SOCK_CLOSE(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }
    #else
    int flags = fcntl(socket_, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "[DBServerConnection] Falha ao obter flags do socket" << std::endl;
        SOCK_CLOSE(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }
    
    if (fcntl(socket_, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "[DBServerConnection] Falha ao configurar socket como não-bloqueante" << std::endl;
        SOCK_CLOSE(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }
    #endif
    
    // Conectado com sucesso
    host_ = host;
    port_ = port;
    connected_ = true;
    
    return true;
}

// Desconecta do servidor
void DBServerConnection::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (socket_ != INVALID_SOCKET) {
        SOCK_CLOSE(socket_);
        socket_ = INVALID_SOCKET;
    }
    
    connected_ = false;
}

// Verifica se está conectado
bool DBServerConnection::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_;
}

// Envia um pacote
bool DBServerConnection::send(const DBPacket& packet) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!connected_ || socket_ == INVALID_SOCKET) {
        return false;
    }
    
    // Serializa o pacote
    std::string data = packet.serialize();
    
    // Envia todos os bytes
    size_t totalSent = 0;
    while (totalSent < data.size()) {
        int sent = ::send(socket_, data.c_str() + totalSent, data.size() - totalSent, 0);
        
        if (sent == SOCKET_ERROR) {
            int error = SOCK_ERRNO;
            
            // Verificações específicas de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param SOCK_EINTR Parâmetro SOCK_EINTR
 * @return Retorna erro
 */
 erro
            if(error == SOCK_EINTR){
                // Interrompido por sinal, tenta novamente
                continue;
            } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param SOCK_EAGAIN Parâmetro SOCK_EAGAIN
 * @return Retorna else
 */
 else if(error == SOCK_EAGAIN){
                // Socket não bloqueante, tenta novamente
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            } else {
                // Erro real
                std::cerr << "[DBServerConnection] Erro ao enviar dados: " << error << std::endl;
                connected_ = false;
                return false;
            }
        }
        
        totalSent += sent;
    }
    
    return true;
}

// Recebe um pacote (com timeout)
bool DBServerConnection::receive(DBPacket& outPacket, uint32_t timeoutMs) {

} // namespace wydbr

    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!connected_ || socket_ == INVALID_SOCKET) {
        return false;
    }
    
    // Configura estruturas para select
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(socket_, &readSet);
    
    // Configura timeout
    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;
    
    // Espera por dados disponíveis
    int result = select(socket_ + 1, &readSet, NULL, NULL, &tv);
    
    if (result == 0) {
        // Timeout
        return false;
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param SOCKET_ERROR Parâmetro SOCKET_ERROR
 * @return Retorna else
 */
 else if(result == SOCKET_ERROR){
        // Erro
        int error = SOCK_ERRNO;
        std::cerr << "[DBServerConnection] Erro em select: " << error << std::endl;
        connected_ = false;
        return false;
    }
    
    // Dados disponíveis, lê
    char buffer[4096];
    int received = recv(socket_, buffer, sizeof(buffer), 0);
    
    if (received == SOCKET_ERROR) {
        int error = SOCK_ERRNO;
        
        // Verificações específicas de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param SOCK_EINTR Parâmetro SOCK_EINTR
 * @return Retorna erro
 */
 erro
        if(error == SOCK_EINTR){
            // Interrompido por sinal
            return false;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param SOCK_EAGAIN Parâmetro SOCK_EAGAIN
 * @return Retorna else
 */
 else if(error == SOCK_EAGAIN){
            // Socket não bloqueante, sem dados
            return false;
        } else {
            // Erro real
            std::cerr << "[DBServerConnection] Erro ao receber dados: " << error << std::endl;
            connected_ = false;
            return false;
        }
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna else
 */
 else if(received == 0){
        // Conexão fechada pelo servidor
        std::cerr << "[DBServerConnection] Conexão fechada pelo servidor" << std::endl;
        connected_ = false;
        return false;
    }
    
    // Adiciona ao buffer de recepção
    receiveBuffer_.append(buffer, received);
    
    // Tenta extrair um pacote completo
    if (receiveBuffer_.size() >= 2) {
        // Lê tamanho do pacote
        uint16_t packetSize = static_cast<uint8_t>(receiveBuffer_[0]) | 
                             (static_cast<uint8_t>(receiveBuffer_[1]) << 8);
        
        // Verifica se temos um pacote completo
        if (receiveBuffer_.size() >= packetSize) {
            try {
                // Desserializa o pacote
                outPacket = DBPacket::deserialize(receiveBuffer_.substr(0, packetSize));
                
                // Remove pacote processado do buffer
                receiveBuffer_ = receiveBuffer_.substr(packetSize);
                
                return true;
            } catch (const std::exception& e) {
                std::cerr << "[DBServerConnection] Erro ao desserializar pacote: " << e.what() << std::endl;
                
                // Remove dados inválidos do buffer (reinicia)
                receiveBuffer_.clear();
                return false;
            }
        }
    }
    
    // Não temos um pacote completo ainda
    return false;
}