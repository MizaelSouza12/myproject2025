/**
 * NetworkManager.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/network/NetworkManager.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "NetworkManager.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <random>
#include <algorithm>

namespace wydbr {


/**
 * @file NetworkManager.cpp
 * @brief Implementação do gerenciador de rede do WYDBRASIL
 * 
 * Implementação completa do gerenciador de conexões de rede, compatível com o WYD original,
 * mas com melhorias de segurança e desempenho seguindo boas práticas modernas de C++.
 */

// Função para obter timestamp atual em ms
/**
 * getCurrentTimestamp
 * 
 * Implementa a funcionalidade getCurrentTimestamp conforme especificação original.
 * @return Retorna uint64_t
 */

static uint64_t getCurrentTimestamp(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

// Implementação do singleton
NetworkManager& NetworkManager::getInstance() {
    static NetworkManager instance;
    return instance;
}

// Construtor
NetworkManager::NetworkManager() 
    : m_listenSocket(nullptr), 
      m_serverActive(false), 
      m_nextConnectionId(1),
      m_activeConnections(0),
      m_maxConnections(MAX_CONNECTIONS),
      m_receiveBufferSize(RECV_BUFFER_SIZE),
      m_sendBufferSize(SEND_BUFFER_SIZE),
      m_initialized(false) {
}

// Destrutor
NetworkManager::~NetworkManager() {
    shutdown();
}

// Inicializa o gerenciador de rede
bool NetworkManager::initialize(int maxConnections, int receiveBufferSize, int sendBufferSize) {
    if (m_initialized) {
        return true; // Já inicializado
    }

    m_maxConnections = maxConnections;
    m_receiveBufferSize = receiveBufferSize;
    m_sendBufferSize = sendBufferSize;
    
    m_serverActive = false;
    m_activeConnections = 0;
    m_nextConnectionId = 1;
    
    if (!CPSock().WSAInitialize()) {
        return false;
    }
    
    m_initialized = true;
    return true;
}

// Inicia o servidor em uma porta específica
bool NetworkManager::startServer(const std::string& host, int port) {
    if (!m_initialized) {
        return false;
    }
    
    if (m_serverActive) {
        return true; // Servidor já iniciado
    }
    
    m_listenSocket = std::make_shared<CPSock>();
    if (!m_listenSocket->Initialize(m_receiveBufferSize, m_sendBufferSize)) {
        m_listenSocket = nullptr;
        return false;
    }
    
    if (m_listenSocket->Listen(host, port) < 0) {
        m_listenSocket = nullptr;
        return false;
    }
    
    m_serverActive = true;
    
    // Iniciar threads
    m_acceptThread = std::thread(&NetworkManager::acceptThread, this);
    m_receiveThread = std::thread(&NetworkManager::receiveThread, this);
    m_sendThread = std::thread(&NetworkManager::sendThread, this);
    m_maintenanceThread = std::thread(&NetworkManager::maintenanceThread, this);
    
    return true;
}

// Thread de aceitação de novas conexões
void NetworkManager::acceptThread() {
    while (m_serverActive) {
        if (!m_listenSocket) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        std::string clientIP;
        int clientPort = 0;
        
        // Aceitar nova conexão
        auto clientSocket = m_listenSocket->Accept(clientIP, clientPort);
        if (!clientSocket) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Verificar limite de conexõ/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_maxConnections Parâmetro m_maxConnections
 * @return Retorna es
 */
es
        if(m_activeConnections >= m_maxConnections){
            // Limite atingido, recusar conexã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_eventCallback Parâmetro m_eventCallback
 * @return Retorna o
 */
o
            if(m_eventCallback){
                m_eventCallback(NetEvent::CONNECTION_REFUSED, -1, nullptr);
            }
            continue;
        }
        
        // Criar nova conexão
        int connectionId = generateConnectionId();
        auto connection = std::make_shared<Connection>();
        connection->socket = clientSocket;
        connection->active = true;
        connection->bytesSent = 0;
        connection->bytesReceived = 0;
        connection->packetsSent = 0;
        connection->packetsReceived = 0;
        
        // Configurar informações da conexão
        connection->info.remoteIP = clientIP;
        connection->info.remotePort = clientPort;
        connection->info.connectionTime = getCurrentTimestamp();
        connection->info.lastActivity = connection->info.connectionTime;
        connection->info.connectionId = connectionId;
        connection->info.authenticated = false;
        connection->info.accountId = -1;
        connection->info.characterId = -1;
        
        // Adicionar à lista de conexões
        {
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            m_connections[connectionId] = connection;
        }
        
        m_activeConnections++;
        
        // Notificar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_eventCallback Parâmetro m_eventCallback
 * @return Retorna evento
 */
 evento
        if(m_eventCallback){
            m_eventCallback(NetEvent::ACCEPT_CONNECTION, connectionId, &connection->info);
        }
    }
}

// Thread de recebimento de dados
void NetworkManager::receiveThread() {
    while (m_serverActive) {
        std::vector<int> activeIds;
        
        // Obter IDs de conexões ativas
        {
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            for (const auto& pair : m_connections) {
                if (pair.second && pair.second->active) {
                    activeIds.push_back(pair.first);
                }
            }
        }
        
        if (activeIds.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Processar conexões /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param activeIds Parâmetro activeIds
 * @return Retorna ativas
 */
 ativas
        for(int id : activeIds){
            std::shared_ptr<Connection> connection;
            
            {
                std::lock_guard<std::mutex> lock(m_connectionsMutex);
                auto it = m_connections.find(id);
                if (it != m_connections.end()) {
                    connection = it->second;
                }
            }
            
            if (!connection || !connection->active) {
                continue;
            }
            
            // Receber dados
            {
                std::lock_guard<std::mutex> socketLock(connection->socketMutex);
                int bytesReceived = connection->socket->Receive();
                
                if (bytesReceived < 0) {
                    // Erro de recepção, fechar conexão
                    connection->active = false;
                    
                    if (m_eventCallback) {
                        m_eventCallback(NetEvent::ERROR, id, nullptr);
                    }
                    
                    closeConnection(id);
                    continue;
                }
                /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna else
 */

                else if(bytesReceived > 0){
                    // Atualizar estatísticas
                    connection->bytesReceived += bytesReceived;
                    connection->info.lastActivity = getCurrentTimestamp();
                    
                    // Processar pacotes recebidos
                    int errorCode = 0;
                    int errorType = 0;
                    
                    while (true) {
                        char* msg = connection->socket->ReadMessage(&errorCode, &errorType);
                        if (!msg) {
                            break;
                        }
                        
                        // Processar pacote recebido
                        connection->packetsReceived++;
                        
                        // Converter para formato do pacote
                        WORD packetType = 0;
                        BYTE* packetData = nullptr;
                        int packetSize = 0;
                        
                        if (connection->socket->ParsePacket((BYTE*)msg, strlen(msg), &packetType, &packetData, &packetSize)) {
                            // Se há um processador registrado, encaminhar o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_packetProcessor Parâmetro m_packetProcessor
 * @return Retorna pacote
 */
 pacote
                            if(m_packetProcessor){
                                m_packetProcessor(id, static_cast<PacketCommand>(packetType), packetData, packetSize);
                            }
                            
                            // Notificar evento de dados /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_eventCallback Parâmetro m_eventCallback
 * @return Retorna recebidos
 */
 recebidos
                            if(m_eventCallback){
                                m_eventCallback(NetEvent::DATA_RECEIVED, id, packetData);
                            }
                        }
                    }
                }
            }
        }
        
        // Pequena pausa para não consumir 100% da CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

// Thread de envio de dados
void NetworkManager::sendThread() {
    while (m_serverActive) {
        std::vector<int> activeIds;
        
        // Obter IDs de conexões ativas
        {
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            for (const auto& pair : m_connections) {
                if (pair.second && pair.second->active) {
                    activeIds.push_back(pair.first);
                }
            }
        }
        
        if (activeIds.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Processar conexões /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param activeIds Parâmetro activeIds
 * @return Retorna ativas
 */
 ativas
        for(int id : activeIds){
            std::shared_ptr<Connection> connection;
            
            {
                std::lock_guard<std::mutex> lock(m_connectionsMutex);
                auto it = m_connections.find(id);
                if (it != m_connections.end()) {
                    connection = it->second;
                }
            }
            
            if (!connection || !connection->active) {
                continue;
            }
            
            // Verificar se há pacotes para enviar
            bool hasPackets = false;
            
            {
                std::lock_guard<std::mutex> queueLock(connection->sendQueueMutex);
                hasPackets = !connection->sendQueue.empty();
            }
            
            if (hasPackets) {
                std::vector<BYTE> packet;
                
                // Obter próximo pacote
                {
                    std::lock_guard<std::mutex> queueLock(connection->sendQueueMutex);
                    packet = std::move(connection->sendQueue.front());
                    connection->sendQueue.pop();
                }
                
                // Enviar pacote
                {
                    std::lock_guard<std::mutex> socketLock(connection->socketMutex);
                    
                    int bytesSent = connection->socket->SendOneMessage(
                        reinterpret_cast<char*>(packet.data()), 
                        static_cast<int>(packet.size())
                    );
                    
                    if (bytesSent < 0) {
                        // Erro de envio, fechar conexão
                        connection->active = false;
                        
                        if (m_eventCallback) {
                            m_eventCallback(NetEvent::ERROR, id, nullptr);
                        }
                        
                        closeConnection(id);
                    }
                    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna else
 */

                    else if(bytesSent > 0){
                        // Atualizar estatísticas
                        connection->bytesSent += bytesSent;
                        connection->packetsSent++;
                        connection->info.lastActivity = getCurrentTimestamp();
                        
                        // Notificar evento de envio /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_eventCallback Parâmetro m_eventCallback
 * @return Retorna completo
 */
 completo
                        if(m_eventCallback){
                            m_eventCallback(NetEvent::SEND_COMPLETE, id, nullptr);
                        }
                    }
                }
            }
        }
        
        // Pequena pausa para não consumir 100% da CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

// Thread de manutenção (timeouts, keepalives)
void NetworkManager::maintenanceThread() {
    const int MAINTENANCE_INTERVAL_MS = 1000; // Verificação a cada 1 /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param m_serverActive Parâmetro m_serverActive
 * @return Retorna segundo
 */
 segundo
    
    while(m_serverActive){
        std::this_thread::sleep_for(std::chrono::milliseconds(MAINTENANCE_INTERVAL_MS));
        
        uint64_t currentTime = getCurrentTimestamp();
        std::vector<int> timeoutIds;
        
        // Verificar timeouts e enviar keepalives
        {
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            
            for (const auto& pair : m_connections) {
                if (!pair.second || !pair.second->active) {
                    continue;
                }
                
                uint64_t lastActivity = pair.second->info.lastActivity;
                int connectionId = pair.first;
                
                // Verificar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param IDLE_TIMEOUT Parâmetro IDLE_TIMEOUT
 * @return Retorna timeout
 */
 timeout
                if(currentTime - lastActivity > IDLE_TIMEOUT){
                    timeoutIds.push_back(connectionId);
                    continue;
                }
                
                // Enviar keepalive quando necessá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param PING_INTERVAL Parâmetro PING_INTERVAL
 * @return Retorna rio
 */
rio
                if(currentTime - lastActivity > PING_INTERVAL){
                    // Criar pacote keepalive (protocolo WYD padrão)
                    BYTE keepaliveData[4] = {0};
                    sendPacket(connectionId, PacketCommand::PKT_KEEPALIVE, keepaliveData, sizeof(keepaliveData));
                }
            }
        }
        
        // Fechar conexões com /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param timeoutIds Parâmetro timeoutIds
 * @return Retorna timeout
 */
 timeout
        for(int id : timeoutIds){
            if (m_eventCallback) {
                m_eventCallback(NetEvent::TIMEOUT, id, nullptr);
            }
            
            closeConnection(id);
        }
    }
}

// Conecta a um servidor remoto
int NetworkManager::connectToServer(const std::string& host, int port, int timeout) {
    if (!m_initialized) {
        return -1;
    }
    
    // Criar socket para conexão
    auto socket = std::make_shared<CPSock>();
    if (!socket->Initialize(m_receiveBufferSize, m_sendBufferSize)) {
        return -1;
    }
    
    // Tentar conectar
    if (!socket->Connect(host, port, timeout)) {
        return -1;
    }
    
    // Criar nova conexão
    int connectionId = generateConnectionId();
    auto connection = std::make_shared<Connection>();
    connection->socket = socket;
    connection->active = true;
    connection->bytesSent = 0;
    connection->bytesReceived = 0;
    connection->packetsSent = 0;
    connection->packetsReceived = 0;
    
    // Configurar informações da conexão
    connection->info.remoteIP = host;
    connection->info.remotePort = port;
    connection->info.connectionTime = getCurrentTimestamp();
    connection->info.lastActivity = connection->info.connectionTime;
    connection->info.connectionId = connectionId;
    connection->info.authenticated = false;
    connection->info.accountId = -1;
    connection->info.characterId = -1;
    
    // Obter informações do socket local
    char localIpBuffer[64] = {0};
    socket->GetPeerAddress(localIpBuffer, sizeof(localIpBuffer));
    connection->info.localIP = localIpBuffer;
    connection->info.localPort = socket->GetPeerPort();
    
    // Adicionar à lista de conexões
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        m_connections[connectionId] = connection;
    }
    
    m_activeConnections++;
    
    // Notificar evento de conexã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_eventCallback Parâmetro m_eventCallback
 * @return Retorna o
 */
o
    if(m_eventCallback){
        m_eventCallback(NetEvent::CONNECT, connectionId, &connection->info);
    }
    
    return connectionId;
}

// Fecha uma conexão específica
bool NetworkManager::closeConnection(int connectionId) {
    std::shared_ptr<Connection> connection;
    
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        auto it = m_connections.find(connectionId);
        if (it == m_connections.end() || !it->second) {
            return false;
        }
        
        connection = it->second;
        if (!connection->active) {
            return false;
        }
        
        // Marcar como inativo para evitar uso posterior
        connection->active = false;
    }
    
    // Fechar socket
    {
        std::lock_guard<std::mutex> socketLock(connection->socketMutex);
        connection->socket->CloseSocket();
    }
    
    // Notificar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_eventCallback Parâmetro m_eventCallback
 * @return Retorna evento
 */
 evento
    if(m_eventCallback){
        m_eventCallback(NetEvent::DISCONNECT, connectionId, &connection->info);
    }
    
    // Remover da lista de conexões (deixar um atraso para garantir que threads terminem o processamento)
    std::thread([this, connectionId]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        {
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            m_connections.erase(connectionId);
        }
        
        m_activeConnections--;
    }).detach();
    
    return true;
}

// Fecha todas as conexões e para o servidor
void NetworkManager::shutdown() {
    if (!m_serverActive) {
        return;
    }
    
    m_serverActive = false;
    
    // Fechar todas as conexões
    std::vector<int> activeConnections;
    
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        for (const auto& pair : m_connections) {
            if (pair.second && pair.second->active) {
                activeConnections.push_back(pair.first);
            }
        }
    }
    
    for (int id : activeConnections) {
        closeConnection(id);
    }
    
    // Fechar socket de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_listenSocket Parâmetro m_listenSocket
 * @return Retorna escuta
 */
 escuta
    if(m_listenSocket){
        m_listenSocket->CloseSocket();
        m_listenSocket = nullptr;
    }
    
    // Aguardar threads finalizarem
    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }
    
    if (m_receiveThread.joinable()) {
        m_receiveThread.join();
    }
    
    if (m_sendThread.joinable()) {
        m_sendThread.join();
    }
    
    if (m_maintenanceThread.joinable()) {
        m_maintenanceThread.join();
    }
    
    // Limpar lista de conexões
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        m_connections.clear();
    }
    
    m_activeConnections = 0;
}

// Envia dados para uma conexão específica
bool NetworkManager::sendPacket(int connectionId, PacketCommand cmd, const BYTE* data, int size) {
    if (!m_initialized || !m_serverActive) {
        return false;
    }
    
    std::shared_ptr<Connection> connection;
    
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        auto it = m_connections.find(connectionId);
        if (it == m_connections.end() || !it->second || !it->second->active) {
            return false;
        }
        
        connection = it->second;
    }
    
    // Criar pacote completo
    BYTE packetBuffer[MAX_PACKET_SIZE];
    int packetSize = connection->socket->CreatePacket(
        static_cast<WORD>(cmd), 
        data, 
        size, 
        packetBuffer, 
        sizeof(packetBuffer)
    );
    
    if (packetSize <= 0) {
        return false;
    }
    
    // Adicionar à fila de envio
    {
        std::lock_guard<std::mutex> queueLock(connection->sendQueueMutex);
        connection->sendQueue.emplace(packetBuffer, packetBuffer + packetSize);
    }
    
    // Notificar thread de envio
    connection->sendCondition.notify_one();
    
    return true;
}

// Envia dados para todas as conexões
bool NetworkManager::broadcastPacket(PacketCommand cmd, const BYTE* data, int size) {
    if (!m_initialized || !m_serverActive) {
        return false;
    }
    
    std::vector<int> activeConnections;
    
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        for (const auto& pair : m_connections) {
            if (pair.second && pair.second->active) {
                activeConnections.push_back(pair.first);
            }
        }
    }
    
    bool success = true;
    
    for (int id : activeConnections) {
        if (!sendPacket(id, cmd, data, size)) {
            success = false;
        }
    }
    
    return success;
}

// Registra callback para eventos de rede
void NetworkManager::setEventCallback(NetworkEventCallback callback) {
    m_eventCallback = callback;
}

// Registra callback para processamento de pacotes
void NetworkManager::setPacketProcessor(PacketProcessorCallback callback) {
    m_packetProcessor = callback;
}

// Obtém estatísticas de rede para uma conexão
bool NetworkManager::getConnectionStats(int connectionId, uint64_t& bytesSent, uint64_t& bytesReceived, 
                                        uint32_t& packetsSent, uint32_t& packetsReceived) {
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_connections.find(connectionId);
    if (it == m_connections.end() || !it->second) {
        return false;
    }
    
    bytesSent = it->second->bytesSent;
    bytesReceived = it->second->bytesReceived;
    packetsSent = it->second->packetsSent;
    packetsReceived = it->second->packetsReceived;
    
    return true;
}

// Obtém informações de uma conexão
ConnectionInfo* NetworkManager::getConnectionInfo(int connectionId) {
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_connections.find(connectionId);
    if (it == m_connections.end() || !it->second) {
        return nullptr;
    }
    
    return &it->second->info;
}

// Define status de autenticação de uma conexão
bool NetworkManager::setAuthenticated(int connectionId, bool authenticated, int accountId) {
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_connections.find(connectionId);
    if (it == m_connections.end() || !it->second) {
        return false;
    }
    
    it->second->info.authenticated = authenticated;
    it->second->info.accountId = accountId;
    
    return true;
}

// Define ID do personagem para uma conexão
bool NetworkManager::setCharacterId(int connectionId, int characterId) {
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_connections.find(connectionId);
    if (it == m_connections.end() || !it->second) {
        return false;
    }
    
    it->second->info.characterId = characterId;
    
    return true;
}

// Obtém número de conexões ativas
int NetworkManager::getActiveConnectionCount() const {
    return m_activeConnections;
}

// Verifica se uma conexão está ativa
bool NetworkManager::isConnectionActive(int connectionId) const {
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_connections.find(connectionId);
    return (it != m_connections.end() && it->second && it->second->active);
}

// Obtém o endereço IP de uma conexão
std::string NetworkManager::getConnectionIP(int connectionId) const {
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_connections.find(connectionId);
    if (it == m_connections.end() || !it->second) {
        return "";
    }
    
    return it->second->info.remoteIP;
}

// Desconecta todas as conexões de um determinado IP
int NetworkManager::disconnectIP(const std::string& ip) {
    std::vector<int> matchingConnections;
    
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        for (const auto& pair : m_connections) {
            if (pair.second && pair.second->active && pair.second->info.remoteIP == ip) {
                matchingConnections.push_back(pair.first);
            }
        }
    }
    
    for (int id : matchingConnections) {
        closeConnection(id);
    }
    
    return static_cast<int>(matchingConnections.size());
}

// Obtém o tempo desde a última atividade da conexão
int64_t NetworkManager::getTimeSinceLastActivity(int connectionId) const {
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_connections.find(connectionId);
    if (it == m_connections.end() || !it->second) {
        return -1;
    }
    
    uint64_t currentTime = getCurrentTimestamp();
    return static_cast<int64_t>(currentTime - it->second->info.lastActivity);
}

// Gera ID único para conexão
int NetworkManager::generateConnectionId() {

} // namespace wydbr

    return m_nextConnectionId++;
}