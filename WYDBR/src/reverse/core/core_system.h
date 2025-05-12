/**
 * @file core_system.h
 * @brief Sistema de núcleo para o WYDBR
 * 
 * Este arquivo define as interfaces básicas e componentes do sistema de núcleo
 * do WYDBR, com base na engenharia reversa do WYD MODERN 2025 STUDIO.
 * 
 * @author WYDBR Team
 * @version 1.0.0
 * @date 2025-05-06
 */

#ifndef WYDBR_CORE_SYSTEM_H
#define WYDBR_CORE_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace wydbr {
namespace core {

/**
 * @enum EventPriority
 * @brief Prioridade de eventos do sistema
 */
enum /**
 * Classe WYDEventPriority
 * 
 * Esta classe implementa a funcionalidade WYDEventPriority conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventPriority {
    CRITICAL = 0,  ///< Prioridade crítica (mais alta)
    HIGH = 1,      ///< Prioridade alta
    NORMAL = 2,    ///< Prioridade normal
    LOW = 3,       ///< Prioridade baixa
    LOWEST = 4     ///< Prioridade mais baixa
};

/**
 * @class Event
 * @brief Classe base para eventos do sistema
 */
/**
 * Classe WYDEvent
 * 
 * Esta classe implementa a funcionalidade WYDEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Event {
public:
    /**
     * @brief Construtor
     * @param type Tipo de evento
     */
    explicit Event(const std::string& type) : type_(type), timestamp_(getCurrentTimestamp()) {}
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~Event() = default;
    
    /**
     * @brief Obtém o tipo do evento
     * @return Tipo do evento
     */
    const std::string& getType() const { return type_; }
    
    /**
     * @brief Obtém o timestamp do evento
     * @return Timestamp do evento em milissegundos
     */
    uint64_t getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Verifica se o evento foi cancelado
     * @return true se o evento foi cancelado, false caso contrário
     */
    bool isCanceled() const { return canceled_; }
    
    /**
     * @brief Cancela o evento
     */
    void cancel() { canceled_ = true; }
    
private:
    std::string type_;     ///< Tipo do evento
    uint64_t timestamp_;   ///< Timestamp de criação
    bool canceled_ = false; ///< Flag de cancelamento
    
    /**
     * @brief Obtém o timestamp atual
     * @return Timestamp atual em milissegundos
     */
    static uint64_t getCurrentTimestamp() {
        // Implementação dependente de plataforma
        return 0; // TODO: Implementar
    }
};

/**
 * @class EventListener
 * @brief Interface para escuta de eventos
 */
/**
 * Classe WYDEventListener
 * 
 * Esta classe implementa a funcionalidade WYDEventListener conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventListener {
public:
    /**
     * @brief Destrutor virtual
     */
    virtual ~EventListener() = default;
    
    /**
     * @brief Método chamado quando um evento é disparado
     * @param event Evento disparado
     */
    virtual void onEvent(const Event& event) = 0;
};

/**
 * @class EventBus
 * @brief Sistema de barramento de eventos
 */
/**
 * Classe WYDEventBus
 * 
 * Esta classe implementa a funcionalidade WYDEventBus conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventBus {
public:
    /**
     * @brief Construtor
     */
    EventBus() = default;
    
    /**
     * @brief Destrutor
     */
    ~EventBus() = default;
    
    /**
     * @brief Registra um listener para um tipo de evento
     * @param eventType Tipo de evento
     * @param listener Listener a ser registrado
     * @param priority Prioridade do listener
     * @return ID do registro
     */
    size_t addEventListener(const std::string& eventType, EventListener* listener, EventPriority priority = EventPriority::NORMAL);
    
    /**
     * @brief Remove um listener registrado
     * @param eventType Tipo de evento
     * @param id ID do registro
     * @return true se removido com sucesso, false caso contrário
     */
    bool removeEventListener(const std::string& eventType, size_t id);
    
    /**
     * @brief Dispara um evento
     * @param event Evento a ser disparado
     */
    void fireEvent(const Event& event);
    
private:
    // Estrutura para armazenar registro de listener
    struct ListenerEntry {
        EventListener* listener;
        EventPriority priority;
        size_t id;
    };
    
    // Mapa de tipo de evento para lista de listeners
    std::unordered_map<std::string, std::vector<ListenerEntry>> listeners_;
    size_t nextId_ = 1;
};

/**
 * @class System
 * @brief Interface base para sistemas
 */
/**
 * Classe WYDSystem
 * 
 * Esta classe implementa a funcionalidade WYDSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class System {
public:
    /**
     * @brief Destrutor virtual
     */
    virtual ~System() = default;
    
    /**
     * @brief Inicializa o sistema
     * @return true se inicializado com sucesso, false caso contrário
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Finaliza o sistema
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Atualiza o sistema
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    virtual void update(float deltaTime) = 0;
};

/**
 * @class SystemManager
 * @brief Gerenciador de sistemas
 */
/**
 * Classe WYDSystemManager
 * 
 * Esta classe implementa a funcionalidade WYDSystemManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SystemManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância singleton
     */
    static SystemManager& getInstance();
    
    /**
     * @brief Registra um sistema
     * @param name Nome do sistema
     * @param system Sistema a ser registrado
     * @return true se registrado com sucesso, false caso contrário
     */
    bool registerSystem(const std::string& name, std::shared_ptr<System> system);
    
    /**
     * @brief Obtém um sistema por nome
     * @param name Nome do sistema
     * @return Ponteiro para o sistema ou nullptr se não encontrado
     */
    std::shared_ptr<System> getSystem(const std::string& name);
    
    /**
     * @brief Inicializa todos os sistemas
     * @return true se todos os sistemas foram inicializados com sucesso, false caso contrário
     */
    bool initializeAll();
    
    /**
     * @brief Finaliza todos os sistemas
     */
    void shutdownAll();
    
    /**
     * @brief Atualiza todos os sistemas
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void updateAll(float deltaTime);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    SystemManager() = default;
    
    /**
     * @brief Construtor de cópia deletado (singleton)
     */
    SystemManager(const SystemManager&) = delete;
    
    /**
     * @brief Operador de atribuição deletado (singleton)
     */
    SystemManager& operator=(const SystemManager&) = delete;
    
    std::unordered_map<std::string, std::shared_ptr<System>> systems_; ///< Mapa de sistemas
    std::vector<std::string> systemOrder_; ///< Ordem de inicialização/atualização
};

} // namespace core
} // namespace wydbr

#endif // WYDBR_CORE_SYSTEM_H