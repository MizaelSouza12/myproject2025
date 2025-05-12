/**
 * event_bus.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/core/event_bus.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _EVENT_BUS_H_
#define _EVENT_BUS_H_

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <mutex>
#include <memory>
#include <algorithm>
#include <queue>
#include <any>
#include <typeindex>
#include <typeinfo>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace wydbr {
namespace core {

/**
 * @brief Classe para armazenar um evento com tipo e dados
 */
/**
 * Classe WYDEvent
 * 
 * Esta classe implementa a funcionalidade WYDEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Event {
public:
    std::string type;
    std::any data;
    std::type_index dataType;
    
    template<typename T>
    Event(const std::string& eventType, const T& eventData)
        : type(eventType), data(eventData), dataType(typeid(T)) {}
    
    template<typename T>
    /**
 * is
 * 
 * Implementa a funcionalidade is conforme especificação original.
 * @return Retorna bool
 */

    bool is() const{
        return dataType == std::type_index(typeid(T));
    }
    
    template<typename T>
    const T& get() const {
        return std::any_cast<const T&>(data);
    }
};

/**
 * @brief Prioridade de manipuladores de eventos
 */
enum /**
 * Classe WYDEventHandlerPriority
 * 
 * Esta classe implementa a funcionalidade WYDEventHandlerPriority conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventHandlerPriority {
    LOWEST = 0,
    LOW = 1,
    NORMAL = 2,
    HIGH = 3,
    HIGHEST = 4,
    MONITOR = 5
};

/**
 * @brief Tipo de manipulador de eventos (sync ou async)
 */
enum /**
 * Classe WYDEventHandlerType
 * 
 * Esta classe implementa a funcionalidade WYDEventHandlerType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventHandlerType {
    SYNC,       // Síncrono - processado na thread de eventos
    ASYNC,      // Assíncrono - enviado para fila de processamento
    IMMEDIATE   // Imediato - processado na thread que publicou o evento
};

/**
 * @brief Configuração do manipulador de eventos
 */
struct EventHandlerConfig {
    EventHandlerPriority priority;
    EventHandlerType type;
    bool persistAfterError;
    
    EventHandlerConfig(
        EventHandlerPriority p = EventHandlerPriority::NORMAL,
        EventHandlerType t = EventHandlerType::SYNC,
        bool persist = true
    ) : priority(p), type(t), persistAfterError(persist) {}
};

/**
 * @class EventBus
 * @brief Implementação de sistema de eventos para comunicação entre componentes
 * 
 * O EventBus permite a comunicação desacoplada entre diferentes partes do sistema
 * através de publicação e assinatura de eventos. Componentes podem se inscrever
 * para tipos específicos de eventos e serão notificados quando tais eventos ocorrerem.
 */
/**
 * Classe WYDEventBus
 * 
 * Esta classe implementa a funcionalidade WYDEventBus conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventBus {
private:
    // Classe interna para armazenar informações de manipuladores de eventos
    struct EventHandlerInfo {
        std::function<void(const Event&)> handler;
        EventHandlerConfig config;
        std::string id;
        
        EventHandlerInfo(const std::function<void(const Event&)>& h, 
                         const EventHandlerConfig& c,
                         const std::string& handlerId)
            : handler(h), config(c), id(handlerId) {}
        
        bool operator<(const EventHandlerInfo& other) const {
            return static_cast<int>(config.priority) < static_cast<int>(other.config.priority);
        }
    };
    
    // Instância singleton
    static EventBus* instance_;
    
    // Mutex para sincronização de acesso
    mutable std::mutex mutex_;
    
    // Mapa de manipuladores para cada tipo de evento
    std::map<std::string, std::vector<EventHandlerInfo>> eventHandlers_;
    
    // Contador para IDs de manipuladores
    std::atomic<size_t> handlerIdCounter_;
    
    // Fila de eventos para processamento assíncrono
    std::queue<Event> eventQueue_;
    
    // Thread de processamento de eventos assíncronos
    std::unique_ptr<std::thread> asyncThread_;
    
    // Variáveis para sincronização da thread assíncrona
    std::condition_variable asyncCondition_;
    std::mutex asyncMutex_;
    std::atomic<bool> isRunning_;
    
    // Configuração de depuração
    bool debugMode_;
    
private:
    // Construtor privado (singleton)
    EventBus();
    
    // Destrutor
    ~EventBus();
    
    // Thread de processamento assíncrono
    /**
 * asyncProcessingThread
 * 
 * Implementa a funcionalidade asyncProcessingThread conforme especificação original.
 */

    void asyncProcessingThread();
    
    // Processa um evento de forma síncrona
    /**
 * processEvent
 * 
 * Implementa a funcionalidade processEvent conforme especificação original.
 * @param event Parâmetro event
 */

    void processEvent(const Event& event);
    
    // Gera um ID único para manipulador de eventos
    /**
 * generateHandlerId
 * 
 * Implementa a funcionalidade generateHandlerId conforme especificação original.
 * @return Retorna std::string
 */

    std::string generateHandlerId();
    
public:
    // Obter instância singleton
    /**
 * getInstance
 * 
 * Implementa a funcionalidade getInstance conforme especificação original.
 * @return Retorna EventBus
 */

    static EventBus* getInstance();
    
    // Destruir a instância singleton
    /**
 * destroyInstance
 * 
 * Implementa a funcionalidade destroyInstance conforme especificação original.
 */

    static void destroyInstance();
    
    /**
     * @brief Inscreve um manipulador para um tipo de evento
     * 
     * @tparam T Tipo de dados do evento
     * @param eventType Nome do tipo de evento
     * @param handler Função manipuladora
     * @param config Configuração do manipulador
     * @return ID do manipulador (para cancelamento posterior)
     */
    template<typename T>
    std::string subscribe(const std::string& eventType, 
                          std::function<void(const T&)> handler,
                          const EventHandlerConfig& config = EventHandlerConfig()) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string handlerId = generateHandlerId();
        
        // Wrapper para converter o manipulador tipado para manipulador genérico
        auto eventHandler = [handler](const Event& event) {
            if (event.is<T>()) {
                handler(event.get<T>());
            }
        };
        
        eventHandlers_[eventType].emplace_back(eventHandler, config, handlerId);
        
        // Ordenar manipuladores por prioridade
        std::sort(eventHandlers_[eventType].begin(), eventHandlers_[eventType].end());
        
        if (debugMode_) {
            std::cout("[EventBus] Subscribed handler %s to event type '%s'\n", 
                   handlerId.c_str(), eventType.c_str());
        }
        
        return handlerId;
    }
    
    /**
     * @brief Cancela a inscrição de um manipulador
     * 
     * @param eventType Tipo de evento
     * @param handlerId ID do manipulador
     * @return true se o manipulador foi encontrado e removido
     */
    bool unsubscribe(const std::string& eventType, const std::string& handlerId);
    
    /**
     * @brief Cancela todas as inscrições para um tipo de evento
     * 
     * @param eventType Tipo de evento
     * @return Número de manipuladores removidos
     */
    int unsubscribeAll(const std::string& eventType);
    
    /**
     * @brief Publica um evento para todos os manipuladores registrados
     * 
     * @tparam T Tipo de dados do evento
     * @param eventType Tipo de evento
     * @param eventData Dados do evento
     * @return Número de manipuladores notificados
     */
    template<typename T>
    /**
 * publish
 * 
 * Implementa a funcionalidade publish conforme especificação original.
 * @param eventType Parâmetro eventType
 * @param eventData Parâmetro eventData
 * @return Retorna int
 */

    int publish(const std::string& eventType, const T& eventData){
        /**
 * event
 * 
 * Implementa a funcionalidade event conforme especificação original.
 * @param eventType Parâmetro eventType
 * @param eventData Parâmetro eventData
 * @return Retorna Event
 */

        Event event(eventType, eventData);
        std::vector<EventHandlerInfo> handlers;
        
        // Obter manipuladores dentro do lock
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (eventHandlers_.find(eventType) != eventHandlers_.end()) {
                handlers = eventHandlers_[eventType];
            }
        }
        
        if (handlers.empty()) {
            if (debugMode_) {
                std::cout("[EventBus] No handlers registered for event '%s'\n", eventType.c_str());
            }
            return 0;
        }
        
        int notifiedCount = 0;
        
        // Processar manipuladores com base no /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param handlers Parâmetro handlers
 * @return Retorna tipo
 */
 tipo
        for(const auto& handlerInfo : handlers){
            try {
                switch (handlerInfo.config.type) {
                    case EventHandlerType::IMMEDIATE:
                        handlerInfo.handler(event);
                        notifiedCount++;
                        break;
                        
                    case EventHandlerType::ASYNC:
                        {
                            std::lock_guard<std::mutex> asyncLock(asyncMutex_);
                            eventQueue_.push(event);
                            asyncCondition_.notify_one();
                            notifiedCount++;
                        }
                        break;
                        
                    case EventHandlerType::SYNC:
                        // Processados posteriormente na thread de eventos
                        {
                            std::lock_guard<std::mutex> asyncLock(asyncMutex_);
                            eventQueue_.push(event);
                            asyncCondition_.notify_one();
                            notifiedCount++;
                        }
                        break;
                }
            } catch (const std::exception& e) {
                if (debugMode_) {
                    std::cout("[EventBus] Error handling event '%s': %s\n", 
                           eventType.c_str(), e.what());
                }
                
                if (!handlerInfo.config.persistAfterError) {
                    unsubscribe(eventType, handlerInfo.id);
                }
            }
        }
        
        if (debugMode_) {
            std::cout("[EventBus] Published event '%s' to %d handlers\n", 
                   eventType.c_str(), notifiedCount);
        }
        
        return notifiedCount;
    }
    
    /**
     * @brief Verifica se há manipuladores registrados para um tipo de evento
     * 
     * @param eventType Tipo de evento
     * @return true se houver manipuladores registrados
     */
    bool hasHandlers(const std::string& eventType) const;
    
    /**
     * @brief Obtém o número de manipuladores registrados para um tipo de evento
     * 
     * @param eventType Tipo de evento
     * @return Número de manipuladores
     */
    size_t getHandlerCount(const std::string& eventType) const;
    
    /**
     * @brief Define o modo de depuração
     * 
     * @param enable true para ativar o modo de depuração
     */
    void setDebugMode(bool enable);
    
    /**
     * @brief Verifica se o modo de depuração está ativado
     * 
     * @return true se o modo de depuração estiver ativado
     */
    bool isDebugMode() const;
    
    /**
     * @brief Obtém o número de eventos na fila de processamento
     * 
     * @return Número de eventos na fila
     */
    size_t getQueueSize() const;
    
    /**
     * @brief Limpa a fila de eventos
     * 
     * @return Número de eventos removidos
     */
    size_t clearQueue();
    
    /**
     * @brief Processa eventos pendentes na fila de forma síncrona
     * 
     * @param maxEvents Número máximo de eventos a processar (0 = todos)
     * @return Número de eventos processados
     */
    size_t processEvents(size_t maxEvents = 0);
    
    /**
     * @brief Pausa o processamento de eventos assíncronos
     */
    void pauseProcessing();
    
    /**
     * @brief Retoma o processamento de eventos assíncronos
     */
    void resumeProcessing();
    
    /**
     * @brief Verifica se o processamento de eventos está pausado
     * 
     * @return true se o processamento estiver pausado
     */
    bool isProcessingPaused() const;
};

/**
 * @brief Utilidade para facilitar o gerenciamento de subscrições de eventos
 * 
 * Esta classe gerencia automaticamente as subscrições de eventos,
 * cancelando-as quando a instância é destruída.
 */
/**
 * Classe WYDEventSubscriptionManager
 * 
 * Esta classe implementa a funcionalidade WYDEventSubscriptionManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventSubscriptionManager {
private:
    std::vector<std::pair<std::string, std::string>> subscriptions_;
    
public:
    /**
     * @brief Construtor
     */
    EventSubscriptionManager() = default;
    
    /**
     * @brief Destrutor - cancela todas as subscrições
     */
    ~EventSubscriptionManager() {
        clear();
    }
    
    /**
     * @brief Adiciona uma subscrição para gerenciamento
     * 
     * @param eventType Tipo de evento
     * @param handlerId ID do manipulador
     */
    void add(const std::string& eventType, const std::string& handlerId) {
        subscriptions_.emplace_back(eventType, handlerId);
    }
    
    /**
     * @brief Subscreve um manipulador e gerencia automaticamente
     * 
     * @tparam T Tipo de dados do evento
     * @param eventType Tipo de evento
     * @param handler Função manipuladora
     * @param config Configuração do manipulador
     * @return ID do manipulador
     */
    template<typename T>
    std::string subscribe(const std::string& eventType, 
                          std::function<void(const T&)> handler,
                          const EventHandlerConfig& config = EventHandlerConfig()) {
        std::string handlerId = EventBus::getInstance()->subscribe<T>(eventType, handler, config);
        add(eventType, handlerId);
        return handlerId;
    }
    
    /**
     * @brief Remove uma subscrição específica
     * 
     * @param eventType Tipo de evento
     * @param handlerId ID do manipulador
     * @return true se removido com sucesso
     */
    bool remove(const std::string& eventType, const std::string& handlerId) {
        auto it = std::find(subscriptions_.begin(), subscriptions_.end(), 
                          std::make_pair(eventType, handlerId));
        if (it != subscriptions_.end()) {
            bool result = EventBus::getInstance()->unsubscribe(eventType, handlerId);
            subscriptions_.erase(it);
            return result;
        }
        return false;
    }
    
    /**
     * @brief Limpa todas as subscrições gerenciadas
     */
    void clear() {
        for (const auto& pair : subscriptions_) {
            EventBus::getInstance()->unsubscribe(pair.first, pair.second);
        }
        subscriptions_.clear();
    }
    
    /**
     * @brief Obtém o número de subscrições gerenciadas
     * 
     * @return Número de subscrições
     */
    size_t size() const {
        return subscriptions_.size();
    }
    
    /**
     * @brief Verifica se está vazio
     * 
     * @return true se não houver subscrições
     */
    bool empty() const {
        return subscriptions_.empty();
    }
};

// Funções utilitárias para facilitar a publicação e subscrição de eventos

/**
 * @brief Publica um evento no barramento global
 * 
 * @tparam T Tipo de dados do evento
 * @param eventType Tipo de evento
 * @param eventData Dados do evento
 * @return Número de manipuladores notificados
 */
template<typename T>
inline /**
 * PublishEvent
 * 
 * Implementa a funcionalidade PublishEvent conforme especificação original.
 * @param eventType Parâmetro eventType
 * @param eventData Parâmetro eventData
 * @return Retorna int
 */
 int PublishEvent(const std::string& eventType, const T& eventData){
    return EventBus::getInstance()->publish<T>(eventType, eventData);
}

/**
 * @brief Subscreve um manipulador para um tipo de evento
 * 
 * @tparam T Tipo de dados do evento
 * @param eventType Tipo de evento
 * @param handler Função manipuladora
 * @param config Configuração do manipulador
 * @return ID do manipulador
 */
template<typename T>
inline std::string SubscribeEvent(const std::string& eventType, 
                                std::function<void(const T&)> handler,
                                const EventHandlerConfig& config = EventHandlerConfig()) {
    return EventBus::getInstance()->subscribe<T>(eventType, handler, config);
}

/**
 * @brief Cancela a subscrição de um manipulador
 * 
 * @param eventType Tipo de evento
 * @param handlerId ID do manipulador
 * @return true se o manipulador foi encontrado e removido
 */
inline /**
 * UnsubscribeEvent
 * 
 * Implementa a funcionalidade UnsubscribeEvent conforme especificação original.
 * @param eventType Parâmetro eventType
 * @param handlerId Parâmetro handlerId
 * @return Retorna bool
 */
 bool UnsubscribeEvent(const std::string& eventType, const std::string& handlerId){
    return EventBus::getInstance()->unsubscribe(eventType, handlerId);
}

} // namespace core
} // namespace wydbr

#endif // _EVENT_BUS_H_