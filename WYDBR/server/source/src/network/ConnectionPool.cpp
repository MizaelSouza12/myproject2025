/**
 * ConnectionPool.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/network/ConnectionPool.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "ConnectionPool.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <sstream>

namespace wydbr {


/**
 * @file ConnectionPool.cpp
 * @brief Implementação do pool de conexões para o servidor WYDBRASIL
 * 
 * Gerencia um conjunto de conexões de rede, permitindo a reutilização eficiente
 * de conexões e otimizando o uso de recursos em aplicações com alta carga de rede.
 */

// Implementação do singleton
ConnectionPool& ConnectionPool::getInstance() {
    static ConnectionPool instance;
    return instance;
}

// Construtor
ConnectionPool::ConnectionPool() 
    : m_nextServerId(1), 
      m_nextConnectionId(1),
      m_maxConnections(50),
      m_initialConnections(5),
      m_connectionTimeout(5000),
      m_totalConnectionsCreated(0),
      m_connectionsInUse(0),
      m_waitingRequests(0),
      m_totalWaitTime(0),
      m_waitTimeSamples(0),
      m_active(false),
      m_initialized(false) {
}

// Destrutor
ConnectionPool::~ConnectionPool() {
    shutdown();
}

// Inicializa o pool de conexões
bool ConnectionPool::initialize(int maxConnections, int initialConnections, int connectionTimeout) {
    if (m_initialized) {
        return true;
    }
    
    m_maxConnections = maxConnections;
    m_initialConnections = initialConnections;
    m_connectionTimeout = connectionTimeout;
    
    m_totalConnectionsCreated = 0;
    m_connectionsInUse = 0;
    m_waitingRequests = 0;
    m_totalWaitTime = 0;
    m_waitTimeSamples = 0;
    
    m_active = true;
    
    // Iniciar threads
    m_healthThread = std::thread(&ConnectionPool::healthCheckThread, this);
    m_poolThread = std::thread(&ConnectionPool::poolManagementThread, this);
    
    m_initialized = true;
    return true;
}

// Adiciona um servidor ao pool
int ConnectionPool::addServer(const std::string& host, int port, int weight, bool isRequired) {
    if (!m_initialized) {
        return -1;
    }
    
    std::lock_guard<std::mutex> lock(m_serversMutex);
    
    // Criar informações do servidor
    int serverId = m_nextServerId++;
    Server server;
    server.host = host;
    server.port = port;
    server.weight = std::max(1, weight);
    server.isRequired = isRequired;
    server.isActive = true;
    server.healthCheckFailures = 0;
    
    m_servers[serverId] = server;
    
    // Criar conexões /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param std::min(m_initialConnections Parâmetro std::min(m_initialConnections
 * @param 2 Parâmetro 2
 * @return Retorna iniciais
 */
 iniciais
    for(int i = 0; i < std::min(m_initialConnections, m_maxConnections / 2); i++) {
        createConnection(serverId);
    }
    
    return serverId;
}

// Remove um servidor do pool
bool ConnectionPool::removeServer(int serverId) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> serversLock(m_serversMutex);
    
    auto it = m_servers.find(serverId);
    if (it == m_servers.end()) {
        return false;
    }
    
    // Verificar se servidor é obrigató/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param it->second.isRequired Parâmetro it->second.isRequired
 * @return Retorna rio
 */
rio
    if(it->second.isRequired){
        // Não permitir remover servidores obrigatórios
        return false;
    }
    
    // Fechar conexões associadas
    std::vector<int> connectionsToClose;
    
    {
        std::lock_guard<std::mutex> connectionsLock(m_connectionsMutex);
        
        for (const auto& pair : m_connections) {
            if (pair.second && pair.second->serverId == serverId) {
                connectionsToClose.push_back(pair.first);
            }
        }
    }
    
    // Fechar conexõ/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param connectionsToClose Parâmetro connectionsToClose
 * @return Retorna es
 */
es
    for(int connectionId : connectionsToClose){
        closeConnection(connectionId);
    }
    
    // Remover servidor
    m_servers.erase(it);
    
    return true;
}

// Define função para autenticação nas conexões
void ConnectionPool::setAuthenticationFunction(std::function<bool(int, const std::string&, int)> authFunc) {
    m_authFunction = authFunc;
}

// Obtém uma conexão do pool
int ConnectionPool::getConnection(int preferredServerId, int waitTimeout) {
    if (!m_initialized || !m_active) {
        return -1;
    }
    
    // Incrementar contador de requisições em espera
    m_waitingRequests++;
    
    // Registrar tempo inicial para estatísticas
    auto startWaitTime = std::chrono::steady_clock::now();
    
    // Tentar obter conexão disponível
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        
        // Se há conexões disponíveis
        if (!m_availableConnections.empty()) {
            // Obter próxima conexão
            int connectionId = m_availableConnections.front();
            m_availableConnections.pop();
            
            // Verificar se a conexão é válida e associada ao servidor preferido
            bool connectionValid = false;
            
            {
                std::lock_guard<std::mutex> connectionsLock(m_connectionsMutex);
                
                auto it = m_connections.find(connectionId);
                if (it != m_connections.end() && it->second) {
                    // Se há preferência por servidor e não é o preferido, procurar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param preferredServerId Parâmetro preferredServerId
 * @return Retorna outra
 */
 outra
                    if(preferredServerId >= 0 && it->second->serverId != preferredServerId){
                        // Devolver à fila e continuar
                        m_availableConnections.push(connectionId);
                    }
                    else {
                        // Marcar como em uso
                        it->second->inUse = true;
                        it->second->lastActivity = std::chrono::steady_clock::now();
                        m_connectionsInUse++;
                        connectionValid = true;
                    }
                }
            }
            
            if (connectionValid) {
                // Decrementar contador de requisições em espera
                m_waitingRequests--;
                
                // Registrar tempo de espera para estatísticas
                auto endWaitTime = std::chrono::steady_clock::now();
                auto waitDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endWaitTime - startWaitTime).count();
                m_totalWaitTime += waitDuration;
                m_waitTimeSamples++;
                
                return connectionId;
            }
        }
        
        // Se chegou aqui, não há conexões disponíveis para o servidor preferido
        // Tentar criar uma nova conexão
        int connectionId = -1;
        
        // Selecionar servidor
        int serverId = selectServer(preferredServerId);
        if (serverId >= 0) {
            connectionId = createConnection(serverId);
        }
        
        if (connectionId >= 0) {
            // Decrementar contador de requisições em espera
            m_waitingRequests--;
            
            // Registrar tempo de espera para estatísticas
            auto endWaitTime = std::chrono::steady_clock::now();
            auto waitDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
                endWaitTime - startWaitTime).count();
            m_totalWaitTime += waitDuration;
            m_waitTimeSamples++;
            
            return connectionId;
        }
        
        // Se ainda não há conexões disponíveis, aguardar
        auto waitResult = m_connectionAvailable.wait_for(
            lock, std::chrono::milliseconds(waitTimeout),
            [this, preferredServerId]() {
                // Verificar se há conexões disponíveis para o servidor preferido
                if (m_availableConnections.empty()) {
                    return false;
                }
                
                if (preferredServerId < 0) {
                    return true;
                }
                
                // Verificar se a conexão é do servidor preferido
                int connectionId = m_availableConnections.front();
                
                std::lock_guard<std::mutex> connectionsLock(m_connectionsMutex);
                auto it = m_connections.find(connectionId);
                if (it != m_connections.end() && it->second) {
                    return it->second->serverId == preferredServerId;
                }
                
                return false;
            });
        
        if (waitResult) {
            // Uma conexão está disponível
            int connectionId = m_availableConnections.front();
            m_availableConnections.pop();
            
            {
                std::lock_guard<std::mutex> connectionsLock(m_connectionsMutex);
                auto it = m_connections.find(connectionId);
                if (it != m_connections.end() && it->second) {
                    // Marcar como em uso
                    it->second->inUse = true;
                    it->second->lastActivity = std::chrono::steady_clock::now();
                    m_connectionsInUse++;
                }
                else {
                    connectionId = -1;
                }
            }
            
            // Decrementar contador de requisições em espera
            m_waitingRequests--;
            
            // Registrar tempo de espera para estatísticas
            auto endWaitTime = std::chrono::steady_clock::now();
            auto waitDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
                endWaitTime - startWaitTime).count();
            m_totalWaitTime += waitDuration;
            m_waitTimeSamples++;
            
            return connectionId;
        }
    }
    
    // Timeout - criar nova conexão como último recurso
    int serverId = selectServer(preferredServerId);
    int connectionId = -1;
    
    if (serverId >= 0) {
        connectionId = createConnection(serverId);
    }
    
    // Decrementar contador de requisições em espera
    m_waitingRequests--;
    
    // Registrar tempo de espera para estatísticas
    auto endWaitTime = std::chrono::steady_clock::now();
    auto waitDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endWaitTime - startWaitTime).count();
    m_totalWaitTime += waitDuration;
    m_waitTimeSamples++;
    
    return connectionId;
}

// Libera uma conexão de volta para o pool
bool ConnectionPool::releaseConnection(int connectionId) {
    if (!m_initialized) {
        return false;
    }
    
    bool connectionFound = false;
    
    {
        std::lock_guard<std::mutex> connectionsLock(m_connectionsMutex);
        
        auto it = m_connections.find(connectionId);
        if (it != m_connections.end() && it->second) {
            // Verificar se conexão está em /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param it->second->inUse Parâmetro it->second->inUse
 * @return Retorna uso
 */
 uso
            if(it->second->inUse){
                // Marcar como disponível
                it->second->inUse = false;
                it->second->lastActivity = std::chrono::steady_clock::now();
                m_connectionsInUse--;
                connectionFound = true;
            }
        }
    }
    
    if (connectionFound) {
        // Adicionar à fila de conexões disponíveis
        {
            std::lock_guard<std::mutex> queueLock(m_queueMutex);
            m_availableConnections.push(connectionId);
        }
        
        // Notificar uma thread em espera
        m_connectionAvailable.notify_one();
        
        return true;
    }
    
    return false;
}

// Executa uma operação com uma conexão do pool
bool ConnectionPool::executeWithConnection(std::function<bool(int)> operation, 
                                          int preferredServerId, 
                                          int waitTimeout,
                                          int retries) {
    if (!m_initialized || !m_active || !operation) {
        return false;
    }
    
    for (int attempt = 0; attempt < retries; attempt++) {
        // Obter conexão
        int connectionId = getConnection(preferredServerId, waitTimeout);
        
        if (connectionId < 0) {
            // Não foi possível obter conexão
            continue;
        }
        
        // Executar operação
        bool result = operation(connectionId);
        
        // Liberar conexã/**
 * releaseConnection
 * 
 * Implementa a funcionalidade releaseConnection conforme especificação original.
 * @param connectionId Parâmetro connectionId
 * @return Retorna o
 */
o
        releaseConnection(connectionId);
        
        if (result) {
            // Operação bem-sucedida
            return true;
        }
    }
    
    // Todas as tentativas falharam
    return false;
}

// Verifica saúde de todos os servidores
int ConnectionPool::checkServerHealth(bool removeUnhealthy) {
    if (!m_initialized) {
        return 0;
    }
    
    int healthyServers = 0;
    std::vector<int> serversToRemove;
    
    {
        std::lock_guard<std::mutex> lock(m_serversMutex);
        
        for (auto& pair : m_servers) {
            bool isHealthy = checkServerHealth(pair.first);
            
            if (isHealthy) {
                healthyServers++;
            }
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !pair.second.isRequired Parâmetro !pair.second.isRequired
 * @return Retorna else
 */

            else if(removeUnhealthy && !pair.second.isRequired){
                serversToRemove.push_back(pair.first);
            }
        }
    }
    
    // Remover servidores não saudáveis (não obrigatórios)
    if (removeUnhealthy) {
        for (int serverId : serversToRemove) {
            removeServer(serverId);
        }
    }
    
    return healthyServers;
}

// Define callback para eventos de saúde do servidor
void ConnectionPool::setServerHealthCallback(std::function<void(int, bool, const std::string&)> callback) {
    m_healthCallback = callback;
}

// Desativa temporariamente um servidor
bool ConnectionPool::disableServer(int serverId, int duration) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_serversMutex);
    
    auto it = m_servers.find(serverId);
    if (it == m_servers.end()) {
        return false;
    }
    
    // Verificar se servidor é obrigató/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param it->second.isRequired Parâmetro it->second.isRequired
 * @return Retorna rio
 */
rio
    if(it->second.isRequired){
        // Não permitir desativar servidores obrigatórios
        return false;
    }
    
    // Desativar servidor
    it->second.isActive = false;
    
    // Definir tempo para reativação
    it->second.reactivationTime = std::chrono::steady_clock::now() + 
                                 std::chrono::milliseconds(duration);
    
    // Notificar evento de saú/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_healthCallback Parâmetro m_healthCallback
 * @return Retorna de
 */
de
    if(m_healthCallback){
        std::ostringstream oss;
        oss << "Servidor desativado temporariamente por " << (duration / 1000) << " segundos";
        m_healthCallback(serverId, false, oss.str());
    }
    
    return true;
}

// Reativa um servidor desativado
bool ConnectionPool::enableServer(int serverId) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_serversMutex);
    
    auto it = m_servers.find(serverId);
    if (it == m_servers.end()) {
        return false;
    }
    
    // Reativar servidor
    it->second.isActive = true;
    it->second.healthCheckFailures = 0;
    
    // Notificar evento de saú/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_healthCallback Parâmetro m_healthCallback
 * @return Retorna de
 */
de
    if(m_healthCallback){
        m_healthCallback(serverId, true, "Servidor reativado manualmente");
    }
    
    return true;
}

// Fecha todas as conexões e finaliza o pool
void ConnectionPool::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    m_active = false;
    
    // Notificar threads em espera
    m_connectionAvailable.notify_all();
    
    // Aguardar threads finalizarem
    if (m_healthThread.joinable()) {
        m_healthThread.join();
    }
    
    if (m_poolThread.joinable()) {
        m_poolThread.join();
    }
    
    // Fechar todas as conexões
    std::vector<int> connectionsToClose;
    
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        
        for (const auto& pair : m_connections) {
            connectionsToClose.push_back(pair.first);
        }
    }
    
    // Fechar conexõ/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param connectionsToClose Parâmetro connectionsToClose
 * @return Retorna es
 */
es
    for(int connectionId : connectionsToClose){
        closeConnection(connectionId);
    }
    
    // Limpar filas
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        while (!m_availableConnections.empty()) {
            m_availableConnections.pop();
        }
    }
    
    m_initialized = false;
}

// Obtém estatísticas do pool
void ConnectionPool::getStats(int& totalConnections, int& activeConnections, 
                            int& waitingRequests, float& avgWaitTime) {
    totalConnections = m_totalConnectionsCreated;
    activeConnections = m_connectionsInUse;
    waitingRequests = m_waitingRequests;
    
    if (m_waitTimeSamples > 0) {
        avgWaitTime = static_cast<float>(m_totalWaitTime) / m_waitTimeSamples;
    }
    else {
        avgWaitTime = 0.0f;
    }
}

// Verifica estado atual de uma conexão
bool ConnectionPool::isConnectionHealthy(int connectionId) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    
    auto it = m_connections.find(connectionId);
    if (it == m_connections.end() || !it->second || !it->second->socket) {
        return false;
    }
    
    return it->second->socket->IsConnected();
}

// Obtém endereço do servidor para uma conexão
std::string ConnectionPool::getServerAddress(int connectionId) {
    if (!m_initialized) {
        return "";
    }
    
    int serverId = -1;
    
    {
        std::lock_guard<std::mutex> connectionsLock(m_connectionsMutex);
        
        auto it = m_connections.find(connectionId);
        if (it == m_connections.end() || !it->second) {
            return "";
        }
        
        serverId = it->second->serverId;
    }
    
    if (serverId < 0) {
        return "";
    }
    
    std::lock_guard<std::mutex> serversLock(m_serversMutex);
    
    auto it = m_servers.find(serverId);
    if (it == m_servers.end()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << it->second.host << ":" << it->second.port;
    return oss.str();
}

// Executa uma operação em múltiplos servidores em paralelo
int ConnectionPool::executeOnMultipleServers(std::function<bool(int)> operation, int targetServerCount) {
    if (!m_initialized || !m_active || !operation) {
        return 0;
    }
    
    // Obter lista de servidores ativos
    std::vector<int> activeServerIds;
    
    {
        std::lock_guard<std::mutex> lock(m_serversMutex);
        
        for (const auto& pair : m_servers) {
            if (pair.second.isActive) {
                activeServerIds.push_back(pair.first);
            }
        }
    }
    
    if (activeServerIds.empty()) {
        return 0;
    }
    
    // Definir número de servidores alvo
    if (targetServerCount <= 0 || targetServerCount > static_cast<int>(activeServerIds.size())) {
        targetServerCount = static_cast<int>(activeServerIds.size());
    }
    
    // Embaralhar lista para seleção aleatória
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(activeServerIds.begin(), activeServerIds.end(), g);
    
    // Truncar para o número alvo
    activeServerIds.resize(targetServerCount);
    
    // Executar operação em cada servidor
    int successCount = 0;
    
    for (int serverId : activeServerIds) {
        if (executeWithConnection([&](int connectionId) -> bool {
            /**
 * operation
 * 
 * Implementa a funcionalidade operation conforme especificação original.
 * @param connectionId Parâmetro connectionId
 * @return Retorna return
 */

            return operation(connectionId);
        }, serverId)) {
            successCount++;
        }
    }
    
    return successCount;
}

// Thread para monitoramento de saúde
void ConnectionPool::healthCheckThread() {
    const int healthCheckInterval = 10000; // 10 /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param m_active Parâmetro m_active
 * @return Retorna segundos
 */
 segundos
    
    while(m_active){
        std::this_thread::sleep_for(std::chrono::milliseconds(healthCheckInterval));
        
        if (!m_active) {
            break;
        }
        
        // Verificar saúde de todos os /**
 * checkServerHealth
 * 
 * Implementa a funcionalidade checkServerHealth conforme especificação original.
 * @return Retorna servidores
 */
 servidores
        checkServerHealth();
        
        // Verificar servidores para reativação
        {
            std::lock_guard<std::mutex> lock(m_serversMutex);
            
            auto now = std::chrono::steady_clock::now();
            
            for (auto& pair : m_servers) {
                if (!pair.second.isActive && now >= pair.second.reactivationTime) {
                    // Reativar servidor
                    pair.second.isActive = true;
                    pair.second.healthCheckFailures = 0;
                    
                    // Notificar evento de saú/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_healthCallback Parâmetro m_healthCallback
 * @return Retorna de
 */
de
                    if(m_healthCallback){
                        m_healthCallback(pair.first, true, "Servidor reativado automaticamente");
                    }
                }
            }
        }
    }
}

// Thread para gerenciamento de pool
void ConnectionPool::poolManagementThread() {
    const int poolManagementInterval = 5000; // 5 /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param m_active Parâmetro m_active
 * @return Retorna segundos
 */
 segundos
    
    while(m_active){
        std::this_thread::sleep_for(std::chrono::milliseconds(poolManagementInterval));
        
        if (!m_active) {
            break;
        }
        
        // Verificar conexões inativas
        std::vector<int> connectionsToClose;
        
        {
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            
            auto now = std::chrono::steady_clock::now();
            
            for (const auto& pair : m_connections) {
                if (pair.second && !pair.second->inUse) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - pair.second->lastActivity).count();
                    
                    // Fechar conexões inativas por mais de 5 /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 300000 Parâmetro 300000
 * @return Retorna minutos
 */
 minutos
                    if(elapsed > 300000){
                        connectionsToClose.push_back(pair.first);
                    }
                }
            }
        }
        
        // Fechar conexões /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param connectionsToClose Parâmetro connectionsToClose
 * @return Retorna inativas
 */
 inativas
        for(int connectionId : connectionsToClose){
            closeConnection(connectionId);
        }
        
        // Verificar se é necessário criar mais conexões
        int totalConnections = m_totalConnectionsCreated;
        int activeConnections = m_connectionsInUse;
        
        // Se mais de 80% das conexões estão em uso e não atingiu limite, criar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_maxConnections Parâmetro m_maxConnections
 * @return Retorna mais
 */
 mais
        if(totalConnections > 0 && 
            activeConnections > 0.8 * totalConnections && 
            totalConnections < m_maxConnections){
            
            // Criar mais conexões
            int serverId = selectServer();
            if (serverId >= 0) {
                createConnection(serverId);
            }
        }
    }
}

// Cria uma nova conexão para um servidor
int ConnectionPool::createConnection(int serverId) {
    if (!m_initialized || !m_active) {
        return -1;
    }
    
    // Verificar limite de conexõ/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_maxConnections Parâmetro m_maxConnections
 * @return Retorna es
 */
es
    if(m_totalConnectionsCreated >= m_maxConnections){
        return -1;
    }
    
    // Obter informações do servidor
    std::string host;
    int port = 0;
    
    {
        std::lock_guard<std::mutex> lock(m_serversMutex);
        
        auto it = m_servers.find(serverId);
        if (it == m_servers.end() || !it->second.isActive) {
            return -1;
        }
        
        host = it->second.host;
        port = it->second.port;
    }
    
    // Criar socket
    auto socket = std::make_shared<CPSock>();
    if (!socket->Initialize()) {
        return -1;
    }
    
    // Conectar ao servidor
    if (!socket->Connect(host, port, m_connectionTimeout)) {
        return -1;
    }
    
    // Autenticar se necessá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_authFunction Parâmetro m_authFunction
 * @return Retorna rio
 */
rio
    if(m_authFunction){
        if (!m_authFunction(serverId, host, port)) {
            socket->CloseSocket();
            return -1;
        }
    }
    
    // Criar objeto de conexão
    auto connection = std::make_shared<Connection>();
    connection->connectionId = m_nextConnectionId++;
    connection->serverId = serverId;
    connection->socket = socket;
    connection->inUse = false;
    connection->lastActivity = std::chrono::steady_clock::now();
    
    // Adicionar à lista de conexões
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        m_connections[connection->connectionId] = connection;
    }
    
    // Adicionar à fila de disponíveis
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_availableConnections.push(connection->connectionId);
    }
    
    // Incrementar contador
    m_totalConnectionsCreated++;
    
    // Notificar threads em espera
    m_connectionAvailable.notify_one();
    
    return connection->connectionId;
}

// Fecha uma conexão
bool ConnectionPool::closeConnection(int connectionId) {
    if (!m_initialized) {
        return false;
    }
    
    std::shared_ptr<Connection> connection;
    
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        
        auto it = m_connections.find(connectionId);
        if (it == m_connections.end() || !it->second) {
            return false;
        }
        
        connection = it->second;
        
        // Remover da lista
        m_connections.erase(it);
    }
    
    // Fechar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param connection->socket Parâmetro connection->socket
 * @return Retorna socket
 */
 socket
    if(connection->socket){
        connection->socket->CloseSocket();
    }
    
    // Decrementar contador se estava em /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param connection->inUse Parâmetro connection->inUse
 * @return Retorna uso
 */
 uso
    if(connection->inUse){
        m_connectionsInUse--;
    }
    
    return true;
}

// Verifica saúde de um servidor específico
bool ConnectionPool::checkServerHealth(int serverId) {
    if (!m_initialized) {
        return false;
    }
    
    // Obter informações do servidor
    std::string host;
    int port = 0;
    bool wasActive = false;
    
    {
        std::lock_guard<std::mutex> lock(m_serversMutex);
        
        auto it = m_servers.find(serverId);
        if (it == m_servers.end()) {
            return false;
        }
        
        host = it->second.host;
        port = it->second.port;
        wasActive = it->second.isActive;
    }
    
    // Criar socket temporário para teste
    CPSock testSocket;
    if (!testSocket.Initialize()) {
        return false;
    }
    
    // Tentar conectar
    bool isHealthy = testSocket.Connect(host, port, 2000); // Timeout de 2 segundos
    
    // Fechar socket
    testSocket.CloseSocket();
    
    // Atualizar estado do servidor
    {
        std::lock_guard<std::mutex> lock(m_serversMutex);
        
        auto it = m_servers.find(serverId);
        if (it == m_servers.end()) {
            return false;
        }
        
        if (isHealthy) {
            // Servidor saudá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !it->second.isActive Parâmetro !it->second.isActive
 * @return Retorna vel
 */
vel
            if(!it->second.isActive){
                // Se estava inativo, reativar
                it->second.isActive = true;
                it->second.healthCheckFailures = 0;
                
                // Notificar evento de saú/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_healthCallback Parâmetro m_healthCallback
 * @return Retorna de
 */
de
                if(m_healthCallback){
                    m_healthCallback(serverId, true, "Servidor recuperado");
                }
            }
        }
        else {
            // Servidor não saudável
            it->second.healthCheckFailures++;
            
            // Se falhar 3 vezes consecutivas, /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !it->second.isRequired Parâmetro !it->second.isRequired
 * @return Retorna desativar
 */
 desativar
            if(it->second.isActive && it->second.healthCheckFailures >= 3 && !it->second.isRequired){
                it->second.isActive = false;
                
                // Definir tempo para reativação
                it->second.reactivationTime = std::chrono::steady_clock::now() + 
                                           std::chrono::milliseconds(30000); // 30 segundos
                
                // Notificar evento de saú/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_healthCallback Parâmetro m_healthCallback
 * @return Retorna de
 */
de
                if(m_healthCallback){
                    std::ostringstream oss;
                    oss << "Servidor falhou verificação de saúde " << it->second.healthCheckFailures 
                        << " vezes, desativado temporariamente";
                    m_healthCallback(serverId, false, oss.str());
                }
            }
        }
    }
    
    return isHealthy;
}

// Balanceador de carga - seleciona servidor baseado em peso e saúde
int ConnectionPool::selectServer(int preferredServerId) {

} // namespace wydbr

    if (!m_initialized) {
        return -1;
    }
    
    // Se há servidor preferido, verificar se está disponí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna vel
 */
vel
    if(preferredServerId >= 0){
        std::lock_guard<std::mutex> lock(m_serversMutex);
        
        auto it = m_servers.find(preferredServerId);
        if (it != m_servers.end() && it->second.isActive) {
            return preferredServerId;
        }
    }
    
    // Selecionar servidor baseado em peso
    std::vector<std::pair<int, int>> activeServers; // (id, peso)
    int totalWeight = 0;
    
    {
        std::lock_guard<std::mutex> lock(m_serversMutex);
        
        for (const auto& pair : m_servers) {
            if (pair.second.isActive) {
                activeServers.emplace_back(pair.first, pair.second.weight);
                totalWeight += pair.second.weight;
            }
        }
    }
    
    if (activeServers.empty()) {
        return -1;
    }
    
    // Selecionar servidor aleatoriamente baseado em peso
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, totalWeight);
    
    int selected = dist(gen);
    int cumulativeWeight = 0;
    
    for (const auto& server : activeServers) {
        cumulativeWeight += server.second;
        
        if (selected <= cumulativeWeight) {
            return server.first;
        }
    }
    
    // Fallback - retornar o primeiro servidor
    return activeServers[0].first;
}