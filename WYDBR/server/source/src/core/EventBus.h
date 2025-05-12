/**
 * EventBus.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/core/EventBus.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _EVENT_BUS_H_
#define _EVENT_BUS_H_

/**
 * @file EventBus.h
 * @brief Sistema de eventos para comunicação entre componentes do WYDBRASIL
 * 
 * Esta classe implementa um sistema central de eventos que permite a comunicação
 * desacoplada entre diferentes componentes do servidor, seguindo o padrão de
 * projeto Observer de forma eficiente e escalável.
 */

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include <any>
#include <typeindex>
#include <typeinfo>
#include <variant>
#include <chrono>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>

namespace wydbr {


/**
 * @brief Tipo para ID de assinante
 */
using SubscriberId = int64_t;

/**
 * @brief Interface base para handler de eventos
 */
/**
 * Classe WYDIEventHandler
 * 
 * Esta classe implementa a funcionalidade WYDIEventHandler conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class IEventHandler {
public:
    virtual ~IEventHandler() = default;
    
    /**
     * @brief Processa um evento
     * @param eventData Dados do evento
     */
    virtual void handle(const std::any& eventData) = 0;
};

/**
 * @brief Handler de eventos para tipo específico
 * @tparam EventType Tipo do evento
 */
template<typename EventType>
/**
 * Classe WYDEventHandler
 * 
 * Esta classe implementa a funcionalidade WYDEventHandler conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventHandler : public IEventHandler {
public:
    /**
     * @brief Construtor
     * @param callback Função de callback
     */
    explicit EventHandler(std::function<void(const EventType&)> callback)
        : m_callback(std::move(callback)) {}
    
    /**
     * @brief Processa um evento
     * @param eventData Dados do evento
     */
    void handle(const std::any& eventData) override {
        try {
            const EventType& event = std::any_cast<const EventType&>(eventData);
            m_callback(event);
        }
        catch (const std::bad_any_cast&) {
            // Tipo incompatível, ignorar
        }
    }
    
private:
    std::function<void(const EventType&)> m_callback;
};

/**
 * @class EventBus
 * @brief Sistema central de eventos para comunicação entre componentes
 * 
 * Esta classe implementa um sistema de publicação/assinatura que permite a
 * comunicação desacoplada entre diferentes componentes do servidor.
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
     * @brief Obtém a instância única do EventBus (Singleton)
     * @return Referência para a instância única
     */
    static EventBus& getInstance();
    
    /**
     * @brief Inicializa o barramento de eventos
     * @param useAsyncDispatch Usar dispatching assíncrono
     * @param maxQueueSize Tamanho máximo da fila de eventos assíncronos
     * @param numWorkerThreads Número de threads de processamento
     * @return true se inicializado com sucesso
     */
    bool initialize(bool useAsyncDispatch = true, int maxQueueSize = 1000, int numWorkerThreads = 2);
    
    /**
     * @brief Finaliza o barramento de eventos
     */
    void shutdown();
    
    /**
     * @brief Assina um tipo de evento
     * @tparam EventType Tipo do evento
     * @param callback Função de callback
     * @param priority Prioridade do assinante (maior = mais prioritário)
     * @return ID do assinante
     */
    template<typename EventType>
    SubscriberId subscribe(std::function<void(const EventType&)> callback, int priority = 0);
    
    /**
     * @brief Cancela assinatura de um evento
     * @param subscriberId ID do assinante
     * @return true se cancelado com sucesso
     */
    bool unsubscribe(SubscriberId subscriberId);
    
    /**
     * @brief Publica um evento
     * @tparam EventType Tipo do evento
     * @param event Evento a ser publicado
     * @return true se publicado com sucesso
     */
    template<typename EventType>
    /**
 * publish
 * 
 * Implementa a funcionalidade publish conforme especificação original.
 * @param event Parâmetro event
 * @return Retorna bool
 */

    bool publish(const EventType& event);
    
    /**
     * @brief Publica um evento com atraso
     * @tparam EventType Tipo do evento
     * @param event Evento a ser publicado
     * @param delayMs Atraso em milissegundos
     * @return true se enfileirado com sucesso
     */
    template<typename EventType>
    /**
 * publishDelayed
 * 
 * Implementa a funcionalidade publishDelayed conforme especificação original.
 * @param event Parâmetro event
 * @param delayMs Parâmetro delayMs
 * @return Retorna bool
 */

    bool publishDelayed(const EventType& event, int delayMs);
    
    /**
     * @brief Publica um evento periodicamente
     * @tparam EventType Tipo do evento
     * @param event Evento a ser publicado
     * @param intervalMs Intervalo em milissegundos
     * @param repeatCount Número de repetições (0 = infinito)
     * @return ID do evento periódico (usado para cancelar)
     */
    template<typename EventType>
    /**
 * publishPeriodic
 * 
 * Implementa a funcionalidade publishPeriodic conforme especificação original.
 * @param event Parâmetro event
 * @param intervalMs Parâmetro intervalMs
 * @param 0 Parâmetro 0
 * @return Retorna int
 */

    int publishPeriodic(const EventType& event, int intervalMs, int repeatCount = 0);
    
    /**
     * @brief Cancela um evento periódico
     * @param periodicEventId ID do evento periódico
     * @return true se cancelado com sucesso
     */
    bool cancelPeriodicEvent(int periodicEventId);
    
    /**
     * @brief Obtém estatísticas do barramento de eventos
     * @param totalEvents Total de eventos publicados (saída)
     * @param asyncQueueSize Tamanho atual da fila assíncrona (saída)
     * @param processingTimeAvg Tempo médio de processamento em ms (saída)
     * @param eventsPerSecond Eventos processados por segundo (saída)
     */
    void getStats(uint64_t& totalEvents, int& asyncQueueSize, float& processingTimeAvg, float& eventsPerSecond);
    
    /**
     * @brief Limpa estatísticas do barramento de eventos
     */
    void clearStats();
    
private:
    // Construtor privado (singleton)
    EventBus();
    
    // Destrutor
    ~EventBus();
    
    // Impedir cópia e atribuição
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    // Estrutura para assinante
    struct Subscriber {
        SubscriberId id;                    // ID do assinante
        std::shared_ptr<IEventHandler> handler; // Handler do evento
        int priority;                       // Prioridade do assinante
        
        Subscriber(SubscriberId id, std::shared_ptr<IEventHandler> handler, int priority)
            : id(id), handler(std::move(handler)), priority(priority) {}
            
        // Operador de comparação para ordenação por prioridade (decrescente)
        bool operator<(const Subscriber& other) const {
            return priority > other.priority;
        }
    };
    
    // Estrutura para evento assíncrono
    struct AsyncEvent {
        std::any eventData;                 // Dados do evento
        std::type_index eventType;          // Tipo do evento
        std::chrono::steady_clock::time_point scheduleTime; // Tempo programado
        
        AsyncEvent(std::any eventData, std::type_index eventType, 
                  std::chrono::steady_clock::time_point scheduleTime = std::chrono::steady_clock::now())
            : eventData(std::move(eventData)), eventType(eventType), scheduleTime(scheduleTime) {}
            
        // Operador de comparação para ordenação por tempo (crescente)
        bool operator>(const AsyncEvent& other) const {
            return scheduleTime > other.scheduleTime;
        }
    };
    
    // Estrutura para evento periódico
    struct PeriodicEvent {
        int id;                             // ID do evento periódico
        std::any eventData;                 // Dados do evento
        std::type_index eventType;          // Tipo do evento
        int intervalMs;                     // Intervalo em milissegundos
        int repeatCount;                    // Número de repetições restantes
        std::chrono::steady_clock::time_point nextTime; // Próximo tempo de execução
        bool active;                        // Evento ativo
        
        PeriodicEvent(int id, std::any eventData, std::type_index eventType, 
                     int intervalMs, int repeatCount)
            : id(id), eventData(std::move(eventData)), eventType(eventType), 
              intervalMs(intervalMs), repeatCount(repeatCount), active(true) {
            nextTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(intervalMs);
        }
    };
    
    // Dispatcher de eventos interno
    /**
 * dispatchEvent
 * 
 * Implementa a funcionalidade dispatchEvent conforme especificação original.
 * @param eventData Parâmetro eventData
 * @param eventType Parâmetro eventType
 */

    void dispatchEvent(const std::any& eventData, const std::type_index& eventType);
    
    // Thread de processamento assíncrono
    /**
 * asyncWorkerThread
 * 
 * Implementa a funcionalidade asyncWorkerThread conforme especificação original.
 */

    void asyncWorkerThread();
    
    // Thread de eventos periódicos
    /**
 * periodicEventThread
 * 
 * Implementa a funcionalidade periodicEventThread conforme especificação original.
 */

    void periodicEventThread();
    
    // Obtém próximo ID de assinante
    /**
 * getNextSubscriberId
 * 
 * Implementa a funcionalidade getNextSubscriberId conforme especificação original.
 * @return Retorna SubscriberId
 */

    SubscriberId getNextSubscriberId();
    
    // Obtém próximo ID de evento periódico
    /**
 * getNextPeriodicEventId
 * 
 * Implementa a funcionalidade getNextPeriodicEventId conforme especificação original.
 * @return Retorna int
 */

    int getNextPeriodicEventId();
    
    // Mapeia tipos de eventos para listas de assinantes
    std::unordered_map<std::type_index, std::vector<Subscriber>> m_subscribers;
    
    // Mapeia IDs de assinantes para tipos de eventos
    std::unordered_map<SubscriberId, std::type_index> m_subscriberTypes;
    
    // Mutex para acesso à lista de assinantes
    std::mutex m_subscribersMutex;
    
    // Fila de eventos assíncronos
    std::priority_queue<AsyncEvent, std::vector<AsyncEvent>, std::greater<AsyncEvent>> m_asyncEvents;
    
    // Mutex para acesso à fila de eventos assíncronos
    std::mutex m_asyncEventsMutex;
    
    // Variável de condição para notificar threads de processamento
    std::condition_variable m_asyncEventsCV;
    
    // Lista de eventos periódicos
    std::vector<PeriodicEvent> m_periodicEvents;
    
    // Mutex para acesso à lista de eventos periódicos
    std::mutex m_periodicEventsMutex;
    
    // Variável de condição para notificar thread de eventos periódicos
    std::condition_variable m_periodicEventsCV;
    
    // Threads de processamento assíncrono
    std::vector<std::thread> m_workerThreads;
    
    // Thread de eventos periódicos
    std::thread m_periodicThread;
    
    // Contador de ID de assinante
    std::atomic<SubscriberId> m_nextSubscriberId;
    
    // Contador de ID de evento periódico
    std::atomic<int> m_nextPeriodicEventId;
    
    // Configurações
    bool m_useAsyncDispatch;
    int m_maxQueueSize;
    int m_numWorkerThreads;
    
    // Estatísticas
    std::atomic<uint64_t> m_totalEvents;
    std::atomic<uint64_t> m_totalProcessingTime;
    std::atomic<uint64_t> m_processedEvents;
    std::chrono::steady_clock::time_point m_statsStartTime;
    
    // Flag de atividade
    std::atomic<bool> m_active;
    
    // Inicializado
    bool m_initialized;
};

// Implementação dos templates

template<typename EventType>
SubscriberId EventBus::subscribe(std::function<void(const EventType&)> callback, int priority) {
    if (!m_initialized || !callback) {
        return -1;
    }
    
    SubscriberId subscriberId = getNextSubscriberId();
    
    auto handler = std::make_shared<EventHandler<EventType>>(std::move(callback));
    
    std::lock_guard<std::mutex> lock(m_subscribersMutex);
    
    // Adicionar à lista de assinantes para este tipo de evento
    std::type_index eventType(typeid(EventType));
    m_subscribers[eventType].emplace_back(subscriberId, handler, priority);
    
    // Ordenar por prioridade
    std::sort(m_subscribers[eventType].begin(), m_subscribers[eventType].end());
    
    // Mapear ID para tipo de evento
    m_subscriberTypes[subscriberId] = eventType;
    
    return subscriberId;
}

template<typename EventType>
bool EventBus::publish(const EventType& event) {
    if (!m_initialized) {
        return false;
    }
    
    std::type_index eventType(typeid(EventType));
    
    // Incrementar contador de eventos
    m_totalEvents++;
    
    if (m_useAsyncDispatch) {
        // Publicação assíncrona
        std::unique_lock<std::mutex> lock(m_asyncEventsMutex);
        
        // Verificar tamanho da fila
        if (m_asyncEvents.size() >= static_cast<size_t>(m_maxQueueSize)) {
            return false;
        }
        
        // Adicionar à fila
        m_asyncEvents.emplace(event, eventType);
        lock.unlock();
        
        // Notificar uma thread de processamento
        m_asyncEventsCV.notify_one();
    }
    else {
        // Publicação sí/**
 * dispatchEvent
 * 
 * Implementa a funcionalidade dispatchEvent conforme especificação original.
 * @param event Parâmetro event
 * @param eventType Parâmetro eventType
 * @return Retorna ncrona
 */
ncrona
        dispatchEvent(event, eventType);
    }
    
    return true;
}

template<typename EventType>
bool EventBus::publishDelayed(const EventType& event, int delayMs) {
    if (!m_initialized || delayMs < 0) {
        return false;
    }
    
    std::type_index eventType(typeid(EventType));
    
    // Incrementar contador de eventos
    m_totalEvents++;
    
    // Calcular tempo programado
    auto scheduleTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);
    
    // Adicionar à fila
    std::unique_lock<std::mutex> lock(m_asyncEventsMutex);
    
    // Verificar tamanho da fila
    if (m_asyncEvents.size() >= static_cast<size_t>(m_maxQueueSize)) {
        return false;
    }
    
    m_asyncEvents.emplace(event, eventType, scheduleTime);
    lock.unlock();
    
    // Notificar uma thread de processamento
    m_asyncEventsCV.notify_one();
    
    return true;
}

template<typename EventType>
int EventBus::publishPeriodic(const EventType& event, int intervalMs, int repeatCount) {
    if (!m_initialized || intervalMs <= 0) {
        return -1;
    }
    
    std::type_index eventType(typeid(EventType));
    
    int periodicEventId = getNextPeriodicEventId();
    
    // Adicionar à lista de eventos periódicos
    std::unique_lock<std::mutex> lock(m_periodicEventsMutex);
    
    m_periodicEvents.emplace_back(periodicEventId, event, eventType, intervalMs, repeatCount);
    lock.unlock();
    
    // Notificar thread de eventos periódicos
    m_periodicEventsCV.notify_one();
    
    return periodicEventId;
}

#endif // _EVENT_BUS_H_

} // namespace wydbr
