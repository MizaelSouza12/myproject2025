/**
 * @file SystemIntegrator.cpp
 * @brief Implementação do sistema central de integração para WYDBR 2.0
 */

#include "SystemIntegrator.h"
#include "Security/SecurityManager.h"
#include "Network/NetworkManager.h"
#include "Database/DatabaseManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

namespace WYDBR {

// Singleton instance
std::unique_ptr<SystemIntegrator> SystemIntegrator::s_instance;
std::once_flag SystemIntegrator::s_onceFlag;

SystemIntegrator& SystemIntegrator::getInstance() {
    std::call_once(s_onceFlag, []() {
        s_instance.reset(new SystemIntegrator());
    });
    return *s_instance;
}

SystemIntegrator::SystemIntegrator() 
    : m_initialized(false)
    , m_running(false)
    , m_nextHandlerId(1)
    , m_stopEventProcessor(false) {
    
    // Inicializar com valores padrão
    m_config["log_level"] = "info";
    m_config["event_queue_size"] = "1000";
    m_config["max_concurrent_connections"] = "500";
    m_config["database_connection_timeout"] = "30";
}

SystemIntegrator::~SystemIntegrator() {
    // Garantir que o sistema foi desligado corretamente
    if (m_running) {
        shutdown();
    }
}

bool SystemIntegrator::initialize(const std::string& configPath) {
    // Prevenir inicialização dupla
    if (m_initialized) {
        return true;
    }
    
    try {
        // Carregar configurações
        if (!configPath.empty()) {
            if (!loadConfig(configPath)) {
                std::cerr << "Falha ao carregar configurações de: " << configPath << std::endl;
                return false;
            }
        }
        
        // Inicializar subsistemas em ordem apropriada
        initializeSubsystems();
        
        // Iniciar thread de processamento de eventos
        m_stopEventProcessor = false;
        m_eventProcessorThread = std::thread(&SystemIntegrator::processEventQueue, this);
        
        m_initialized = true;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro na inicialização do SystemIntegrator: " << e.what() << std::endl;
        return false;
    }
}

bool SystemIntegrator::start() {
    // Verificar se está inicializado
    if (!m_initialized) {
        std::cerr << "Tentativa de iniciar SystemIntegrator não inicializado" << std::endl;
        return false;
    }
    
    // Prevenir inicialização dupla
    if (m_running) {
        return true;
    }
    
    try {
        // Iniciar subsistemas em ordem apropriada
        if (m_databaseManager) {
            if (!m_databaseManager->start()) {
                std::cerr << "Falha ao iniciar DatabaseManager" << std::endl;
                return false;
            }
        }
        
        if (m_securityManager) {
            if (!m_securityManager->start()) {
                std::cerr << "Falha ao iniciar SecurityManager" << std::endl;
                // Rollback
                if (m_databaseManager) m_databaseManager->stop();
                return false;
            }
        }
        
        if (m_networkManager) {
            if (!m_networkManager->start()) {
                std::cerr << "Falha ao iniciar NetworkManager" << std::endl;
                // Rollback
                if (m_securityManager) m_securityManager->stop();
                if (m_databaseManager) m_databaseManager->stop();
                return false;
            }
        }
        
        // Registrar tempo de início
        m_startTime = std::chrono::system_clock::now();
        
        // Disparar evento de mudança de status
        Event statusEvent;
        statusEvent.type = Event::Type::SERVER_STATUS_CHANGED;
        statusEvent.source = "SystemIntegrator";
        statusEvent.data["status"] = "running";
        statusEvent.timestamp = std::chrono::system_clock::now();
        dispatchEvent(statusEvent);
        
        m_running = true;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro ao iniciar SystemIntegrator: " << e.what() << std::endl;
        return false;
    }
}

void SystemIntegrator::shutdown() {
    // Verificar se está em execução
    if (!m_running) {
        return;
    }
    
    try {
        // Disparar evento de mudança de status
        Event statusEvent;
        statusEvent.type = Event::Type::SERVER_STATUS_CHANGED;
        statusEvent.source = "SystemIntegrator";
        statusEvent.data["status"] = "shutting_down";
        statusEvent.timestamp = std::chrono::system_clock::now();
        dispatchEvent(statusEvent);
        
        // Parar subsistemas na ordem inversa da inicialização
        if (m_networkManager) {
            m_networkManager->stop();
        }
        
        if (m_securityManager) {
            m_securityManager->stop();
        }
        
        if (m_databaseManager) {
            m_databaseManager->stop();
        }
        
        // Parar thread de processamento de eventos
        m_stopEventProcessor = true;
        m_eventCondition.notify_all();
        
        if (m_eventProcessorThread.joinable()) {
            m_eventProcessorThread.join();
        }
        
        m_running = false;
        
        // Limpar filas de eventos
        {
            std::lock_guard<std::mutex> lock(m_eventQueueMutex);
            std::queue<Event> empty;
            std::swap(m_eventQueue, empty);
        }
        
        // Limpar handlers
        {
            std::lock_guard<std::mutex> lock(m_eventHandlersMutex);
            m_eventHandlers.clear();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erro durante shutdown do SystemIntegrator: " << e.what() << std::endl;
    }
}

bool SystemIntegrator::isInitialized() const {
    return m_initialized;
}

bool SystemIntegrator::isRunning() const {
    return m_running;
}

int SystemIntegrator::registerEventHandler(Event::Type type, EventHandler handler) {
    if (!handler) {
        return -1;
    }
    
    std::lock_guard<std::mutex> lock(m_eventHandlersMutex);
    
    int handlerId = m_nextHandlerId++;
    m_eventHandlers.push_back({handlerId, type, handler});
    
    return handlerId;
}

bool SystemIntegrator::unregisterEventHandler(int handlerId) {
    std::lock_guard<std::mutex> lock(m_eventHandlersMutex);
    
    auto it = std::find_if(m_eventHandlers.begin(), m_eventHandlers.end(),
                         [handlerId](const RegisteredHandler& handler) {
                             return handler.id == handlerId;
                         });
    
    if (it != m_eventHandlers.end()) {
        m_eventHandlers.erase(it);
        return true;
    }
    
    return false;
}

void SystemIntegrator::dispatchEvent(const Event& event) {
    if (!m_initialized) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(m_eventQueueMutex);
        
        // Verificar limite da fila
        int maxQueueSize = 1000;
        try {
            maxQueueSize = std::stoi(m_config["event_queue_size"]);
        } catch (...) {}
        
        if (m_eventQueue.size() >= static_cast<size_t>(maxQueueSize)) {
            std::cerr << "Alerta: Fila de eventos cheia, descartando eventos" << std::endl;
            return;
        }
        
        m_eventQueue.push(event);
    }
    
    m_eventCondition.notify_one();
}

Security::SecurityManager& SystemIntegrator::getSecurityManager() {
    if (!m_securityManager) {
        throw std::runtime_error("SecurityManager não inicializado");
    }
    return *m_securityManager;
}

Network::NetworkManager& SystemIntegrator::getNetworkManager() {
    if (!m_networkManager) {
        throw std::runtime_error("NetworkManager não inicializado");
    }
    return *m_networkManager;
}

Database::DatabaseManager& SystemIntegrator::getDatabaseManager() {
    if (!m_databaseManager) {
        throw std::runtime_error("DatabaseManager não inicializado");
    }
    return *m_databaseManager;
}

SystemIntegrator::SystemStatus SystemIntegrator::getSystemStatus() const {
    SystemStatus status;
    
    status.startTime = m_startTime;
    status.uptime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - m_startTime);
    
    // Verificar status dos subsistemas
    status.databaseConnected = m_databaseManager && m_databaseManager->isConnected();
    status.networkActive = m_networkManager && m_networkManager->isRunning();
    status.securityActive = m_securityManager && m_securityManager->isActive();
    
    // Obter estatísticas
    status.activeSessions = m_networkManager ? m_networkManager->getActiveSessionCount() : 0;
    status.activeEntities = 0; // Aqui implementaríamos a lógica para obter o número de entidades ativas
    
    // Status de componentes individuais
    if (m_networkManager) {
        auto netStats = m_networkManager->getStatistics();
        status.componentStatus["network.connections"] = std::to_string(netStats.activeConnections);
        status.componentStatus["network.packetsReceived"] = std::to_string(netStats.packetsReceived);
        status.componentStatus["network.packetsSent"] = std::to_string(netStats.packetsSent);
        status.componentStatus["network.bytesReceived"] = std::to_string(netStats.bytesReceived);
        status.componentStatus["network.bytesSent"] = std::to_string(netStats.bytesSent);
    }
    
    if (m_databaseManager) {
        auto dbStats = m_databaseManager->getStatistics();
        status.componentStatus["database.queries"] = std::to_string(dbStats.queryCount);
        status.componentStatus["database.connectionPool"] = std::to_string(dbStats.activeConnections);
        status.componentStatus["database.avgQueryTime"] = std::to_string(dbStats.avgQueryTimeMs);
    }
    
    if (m_securityManager) {
        auto secStats = m_securityManager->getStatistics();
        status.componentStatus["security.authAttempts"] = std::to_string(secStats.authAttempts);
        status.componentStatus["security.failedAuths"] = std::to_string(secStats.failedAuths);
        status.componentStatus["security.blockedIPs"] = std::to_string(secStats.blockedIPCount);
    }
    
    return status;
}

bool SystemIntegrator::loadConfig(const std::string& configPath) {
    try {
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            std::cerr << "Não foi possível abrir arquivo de configuração: " << configPath << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(configFile, line)) {
            // Remover comentários
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Ignorar linhas vazias
            if (line.empty() || std::all_of(line.begin(), line.end(), isspace)) {
                continue;
            }
            
            // Extrair chave e valor
            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                
                // Remover espaços em branco
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // Armazenar configuração
                m_config[key] = value;
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro ao carregar configurações: " << e.what() << std::endl;
        return false;
    }
}

void SystemIntegrator::initializeSubsystems() {
    // Criar e inicializar subsistemas principais
    
    // 1. Gerenciador de Banco de Dados
    m_databaseManager = std::make_unique<Database::DatabaseManager>();
    
    // Configurar com base nas configurações carregadas
    m_databaseManager->setConnectionPoolSize(
        std::stoi(m_config.count("db_connection_pool_size") ? 
                 m_config["db_connection_pool_size"] : "10"));
    
    m_databaseManager->setConnectionTimeout(
        std::stoi(m_config.count("database_connection_timeout") ? 
                 m_config["database_connection_timeout"] : "30"));
    
    if (!m_databaseManager->initialize()) {
        throw std::runtime_error("Falha ao inicializar DatabaseManager");
    }
    
    // 2. Gerenciador de Segurança
    m_securityManager = std::make_unique<Security::SecurityManager>();
    
    // Configurar com base nas configurações carregadas
    m_securityManager->setTokenLifetime(
        std::stoi(m_config.count("token_lifetime_minutes") ? 
                 m_config["token_lifetime_minutes"] : "60"));
    
    if (!m_securityManager->initialize()) {
        throw std::runtime_error("Falha ao inicializar SecurityManager");
    }
    
    // 3. Gerenciador de Rede
    m_networkManager = std::make_unique<Network::NetworkManager>();
    
    // Configurar com base nas configurações carregadas
    m_networkManager->setMaxConnections(
        std::stoi(m_config.count("max_concurrent_connections") ? 
                 m_config["max_concurrent_connections"] : "500"));
    
    if (!m_networkManager->initialize()) {
        throw std::runtime_error("Falha ao inicializar NetworkManager");
    }
    
    // Estabelecer dependências entre subsistemas
    m_networkManager->setSecurityManager(m_securityManager.get());
    m_securityManager->setDatabaseManager(m_databaseManager.get());
}

void SystemIntegrator::processEventQueue() {
    while (!m_stopEventProcessor) {
        std::vector<Event> eventsToProcess;
        
        {
            std::unique_lock<std::mutex> lock(m_eventQueueMutex);
            
            // Esperar até que haja eventos ou sinal de parada
            m_eventCondition.wait(lock, [this]() {
                return !m_eventQueue.empty() || m_stopEventProcessor;
            });
            
            if (m_stopEventProcessor && m_eventQueue.empty()) {
                break;
            }
            
            // Obter todos os eventos disponíveis
            while (!m_eventQueue.empty()) {
                eventsToProcess.push_back(m_eventQueue.front());
                m_eventQueue.pop();
            }
        }
        
        // Processar eventos
        std::vector<RegisteredHandler> handlers;
        
        {
            std::lock_guard<std::mutex> lock(m_eventHandlersMutex);
            handlers = m_eventHandlers;
        }
        
        for (const auto& event : eventsToProcess) {
            for (const auto& handler : handlers) {
                // Chamar handlers que se interessam por este tipo de evento
                if (handler.type == event.type || handler.type == Event::Type::GENERIC) {
                    try {
                        handler.handler(event);
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Erro no handler de evento: " << e.what() << std::endl;
                    }
                }
            }
        }
    }
}

} // namespace WYDBR