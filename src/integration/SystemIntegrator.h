/**
 * @file SystemIntegrator.h
 * @brief Sistema central de integração para WYDBR 2.0
 */

#ifndef WYDBR_SYSTEM_INTEGRATOR_H
#define WYDBR_SYSTEM_INTEGRATOR_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <thread>
#include <atomic>
#include <queue>
#include <variant>

namespace WYDBR {

// Forward declarations
namespace Security {
class SecurityManager;
}

namespace Network {
class NetworkManager;
}

namespace Database {
class DatabaseManager;
}

/**
 * @class SystemIntegrator
 * @brief Classe central para integração de todos os componentes do WYDBR 2.0
 */
class SystemIntegrator {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância única
     */
    static SystemIntegrator& getInstance();
    
    /**
     * @brief Inicializa o integrador com configurações
     * @param configPath Caminho para arquivo de configuração (opcional)
     * @return true se inicialização for bem-sucedida
     */
    bool initialize(const std::string& configPath = "");
    
    /**
     * @brief Inicia todos os sistemas integrados
     * @return true se todos os sistemas iniciarem com sucesso
     */
    bool start();
    
    /**
     * @brief Desliga todos os sistemas de forma ordenada
     */
    void shutdown();
    
    /**
     * @brief Verifica se o sistema está inicializado
     * @return true se inicializado
     */
    bool isInitialized() const;
    
    /**
     * @brief Verifica se o sistema está em execução
     * @return true se em execução
     */
    bool isRunning() const;
    
    /**
     * @struct Event
     * @brief Estrutura para eventos do sistema
     */
    struct Event {
        enum class Type {
            GENERIC,
            ENTITY_CREATED,
            ENTITY_DELETED,
            ENTITY_MOVED,
            PLAYER_LOGIN,
            PLAYER_LOGOUT,
            TRANSACTION_COMPLETED,
            SKILL_USED,
            ITEM_ACQUIRED,
            ITEM_USED,
            SECURITY_ALERT,
            SERVER_STATUS_CHANGED
        };
        
        Type type;
        std::string source;
        std::unordered_map<std::string, std::string> data;
        std::chrono::system_clock::time_point timestamp;
    };
    
    /**
     * @typedef EventHandler
     * @brief Tipo para handlers de eventos
     */
    using EventHandler = std::function<void(const Event&)>;
    
    /**
     * @brief Registra um handler para um tipo de evento
     * @param type Tipo de evento
     * @param handler Função handler
     * @return ID do handler (para desregistro posterior)
     */
    int registerEventHandler(Event::Type type, EventHandler handler);
    
    /**
     * @brief Remove um handler registrado
     * @param handlerId ID do handler a remover
     * @return true se handler for encontrado e removido
     */
    bool unregisterEventHandler(int handlerId);
    
    /**
     * @brief Dispara um evento no sistema
     * @param event Evento a disparar
     */
    void dispatchEvent(const Event& event);
    
    /**
     * @brief Obtém referência para o gerenciador de segurança
     * @return Referência para o gerenciador de segurança
     */
    Security::SecurityManager& getSecurityManager();
    
    /**
     * @brief Obtém referência para o gerenciador de rede
     * @return Referência para o gerenciador de rede
     */
    Network::NetworkManager& getNetworkManager();
    
    /**
     * @brief Obtém referência para o gerenciador de banco de dados
     * @return Referência para o gerenciador de banco de dados
     */
    Database::DatabaseManager& getDatabaseManager();
    
    /**
     * @struct SystemStatus
     * @brief Status dos subsistemas
     */
    struct SystemStatus {
        bool databaseConnected;
        bool networkActive;
        bool securityActive;
        int activeSessions;
        int activeEntities;
        std::chrono::system_clock::time_point startTime;
        std::chrono::seconds uptime;
        std::unordered_map<std::string, std::string> componentStatus;
    };
    
    /**
     * @brief Obtém status geral do sistema
     * @return Estrutura com informações de status
     */
    SystemStatus getSystemStatus() const;
    
private:
    // Singleton
    static std::unique_ptr<SystemIntegrator> s_instance;
    static std::once_flag s_onceFlag;
    
    SystemIntegrator();
    ~SystemIntegrator();
    
    // Estrutura interna para handlers registrados
    struct RegisteredHandler {
        int id;
        Event::Type type;
        EventHandler handler;
    };
    
    // Métodos privados
    bool loadConfig(const std::string& configPath);
    void initializeSubsystems();
    void processEventQueue();
    
    // Estado interno
    std::atomic<bool> m_initialized;
    std::atomic<bool> m_running;
    std::chrono::system_clock::time_point m_startTime;
    
    // Configurações
    std::unordered_map<std::string, std::string> m_config;
    
    // Sistema de eventos
    std::vector<RegisteredHandler> m_eventHandlers;
    std::queue<Event> m_eventQueue;
    std::mutex m_eventQueueMutex;
    std::mutex m_eventHandlersMutex;
    std::condition_variable m_eventCondition;
    std::atomic<int> m_nextHandlerId;
    std::thread m_eventProcessorThread;
    std::atomic<bool> m_stopEventProcessor;
    
    // Referências para subsistemas principais
    std::unique_ptr<Security::SecurityManager> m_securityManager;
    std::unique_ptr<Network::NetworkManager> m_networkManager;
    std::unique_ptr<Database::DatabaseManager> m_databaseManager;
};

} // namespace WYDBR

#endif // WYDBR_SYSTEM_INTEGRATOR_H