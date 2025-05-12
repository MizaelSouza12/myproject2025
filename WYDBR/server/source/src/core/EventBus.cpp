/**
 * EventBus.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/core/EventBus.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "EventBus.h"
#include <iostream>
#include <algorithm>

namespace wydbr {


/**
 * @file EventBus.cpp
 * @brief Implementação do sistema de eventos do WYDBRASIL
 * 
 * Sistema central de eventos que permite a comunicação entre diferentes
 * componentes do servidor de forma desacoplada, eficiente e escalável.
 */

// Implementação do singleton
EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

// Construtor
EventBus::EventBus() 
    : m_nextSubscriberId(1),
      m_nextPeriodicEventId(1),
      m_useAsyncDispatch(true),
      m_maxQueueSize(1000),
      m_numWorkerThreads(2),
      m_totalEvents(0),
      m_totalProcessingTime(0),
      m_processedEvents(0),
      m_active(false),
      m_initialized(false) {
    
    m_statsStartTime = std::chrono::steady_clock::now();
}

// Destrutor
EventBus::~EventBus() {
    shutdown();
}

// Inicializa o barramento de eventos
bool EventBus::initialize(bool useAsyncDispatch, int maxQueueSize, int numWorkerThreads) {
    if (m_initialized) {
        return true;
    }
    
    m_useAsyncDispatch = useAsyncDispatch;
    m_maxQueueSize = maxQueueSize;
    m_numWorkerThreads = numWorkerThreads;
    
    m_totalEvents = 0;
    m_totalProcessingTime = 0;
    m_processedEvents = 0;
    m_statsStartTime = std::chrono::steady_clock::now();
    
    m_active = true;
    
    // Iniciar threads se modo assí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_useAsyncDispatch Parâmetro m_useAsyncDispatch
 * @return Retorna ncrono
 */
ncrono
    if(m_useAsyncDispatch){
        // Iniciar threads de processamento
        m_workerThreads.resize(m_numWorkerThreads);
        for (int i = 0; i < m_numWorkerThreads; i++) {
            m_workerThreads[i] = std::thread(&EventBus::asyncWorkerThread, this);
        }
    }
    
    // Iniciar thread de eventos periódicos
    m_periodicThread = std::thread(&EventBus::periodicEventThread, this);
    
    m_initialized = true;
    return true;
}

// Finaliza o barramento de eventos
void EventBus::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    m_active = false;
    
    // Notificar threads para finalizar
    m_asyncEventsCV.notify_all();
    m_periodicEventsCV.notify_all();
    
    // Aguardar threads de /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param m_workerThreads Parâmetro m_workerThreads
 * @return Retorna processamento
 */
 processamento
    for(auto& thread : m_workerThreads){
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    // Aguardar thread de eventos periódicos
    if (m_periodicThread.joinable()) {
        m_periodicThread.join();
    }
    
    // Limpar filas
    std::unique_lock<std::mutex> asyncLock(m_asyncEventsMutex);
    while (!m_asyncEvents.empty()) {
        m_asyncEvents.pop();
    }
    asyncLock.unlock();
    
    std::unique_lock<std::mutex> periodicLock(m_periodicEventsMutex);
    m_periodicEvents.clear();
    periodicLock.unlock();
    
    // Limpar assinantes
    std::unique_lock<std::mutex> subscribersLock(m_subscribersMutex);
    m_subscribers.clear();
    m_subscriberTypes.clear();
    subscribersLock.unlock();
    
    m_initialized = false;
}

// Cancela assinatura de um evento
bool EventBus::unsubscribe(SubscriberId subscriberId) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_subscribersMutex);
    
    auto typeIt = m_subscriberTypes.find(subscriberId);
    if (typeIt == m_subscriberTypes.end()) {
        return false;
    }
    
    auto eventType = typeIt->second;
    auto& subscribers = m_subscribers[eventType];
    
    auto it = std::find_if(subscribers.begin(), subscribers.end(),
                         [subscriberId](const Subscriber& s) { return s.id == subscriberId; });
    
    if (it != subscribers.end()) {
        subscribers.erase(it);
        m_subscriberTypes.erase(typeIt);
        return true;
    }
    
    return false;
}

// Cancela um evento periódico
bool EventBus::cancelPeriodicEvent(int periodicEventId) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_periodicEventsMutex);
    
    auto it = std::find_if(m_periodicEvents.begin(), m_periodicEvents.end(),
                          [periodicEventId](const PeriodicEvent& e) { return e.id == periodicEventId; });
    
    if (it != m_periodicEvents.end()) {
        it->active = false;
        return true;
    }
    
    return false;
}

// Obtém estatísticas do barramento de eventos
void EventBus::getStats(uint64_t& totalEvents, int& asyncQueueSize, float& processingTimeAvg, float& eventsPerSecond) {
    totalEvents = m_totalEvents;
    
    {
        std::lock_guard<std::mutex> lock(m_asyncEventsMutex);
        asyncQueueSize = static_cast<int>(m_asyncEvents.size());
    }
    
    uint64_t processedEvents = m_processedEvents;
    if (processedEvents > 0) {
        processingTimeAvg = static_cast<float>(m_totalProcessingTime) / processedEvents / 1000000.0f; // ns to ms
    }
    else {
        processingTimeAvg = 0.0f;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_statsStartTime).count();
    
    if (elapsed > 0) {
        eventsPerSecond = static_cast<float>(processedEvents) / elapsed;
    }
    else {
        eventsPerSecond = 0.0f;
    }
}

// Limpa estatísticas do barramento de eventos
void EventBus::clearStats() {
    m_totalEvents = 0;
    m_totalProcessingTime = 0;
    m_processedEvents = 0;
    m_statsStartTime = std::chrono::steady_clock::now();
}

// Dispatcher de eventos interno
void EventBus::dispatchEvent(const std::any& eventData, const std::type_index& eventType) {
    std::vector<Subscriber> subscribersCopy;
    
    {
        std::lock_guard<std::mutex> lock(m_subscribersMutex);
        
        auto it = m_subscribers.find(eventType);
        if (it != m_subscribers.end()) {
            subscribersCopy = it->second;
        }
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Notificar todos os /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param subscribersCopy Parâmetro subscribersCopy
 * @return Retorna assinantes
 */
 assinantes
    for(const auto& subscriber : subscribersCopy){
        subscriber.handler->handle(eventData);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
    
    // Atualizar estatísticas
    m_totalProcessingTime += duration;
    m_processedEvents++;
}

// Thread de processamento assíncrono
void EventBus::asyncWorkerThread() {
    while (m_active) {
        AsyncEvent event{std::any{}, std::type_index(typeid(void))};
        bool hasEvent = false;
        
        {
            std::unique_lock<std::mutex> lock(m_asyncEventsMutex);
            
            m_asyncEventsCV.wait(lock, [this] {
                return !m_active || !m_asyncEvents.empty();
            });
            
            if (!m_active) {
                break;
            }
            
            if (!m_asyncEvents.empty()) {
                event = m_asyncEvents.top();
                
                // Verificar se o evento está programado para agora
                auto now = std::chrono::steady_clock::now();
                if (event.scheduleTime <= now) {
                    m_asyncEvents.pop();
                    hasEvent = true;
                }
                else {
                    // Esperar até o próximo evento ou notificação
                    m_asyncEventsCV.wait_until(lock, event.scheduleTime, [this] {
                        return !m_active || m_asyncEvents.empty() || 
                               m_asyncEvents.top().scheduleTime <= std::chrono::steady_clock::now();
                    });
                    
                    if (!m_active) {
                        break;
                    }
                    
                    // Verificar novamente se temos eventos prontos
                    if (!m_asyncEvents.empty() && 
                        m_asyncEvents.top().scheduleTime <= std::chrono::steady_clock::now()) {
                        event = m_asyncEvents.top();
                        m_asyncEvents.pop();
                        hasEvent = true;
                    }
                }
            }
        }
        
        if (hasEvent) {
            dispatchEvent(event.eventData, event.eventType);
        }
    }
}

// Thread de eventos periódicos
void EventBus::periodicEventThread() {
    while (m_active) {
        std::vector<PeriodicEvent*> eventsToPublish;
        auto now = std::chrono::steady_clock::now();
        auto nextWakeup = now + std::chrono::seconds(60); // Padrão: 1 minuto
        
        {
            std::unique_lock<std::mutex> lock(m_periodicEventsMutex);
            
            // Verificar eventos prontos para publicaçã/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param m_periodicEvents Parâmetro m_periodicEvents
 * @return Retorna o
 */
o
            for(auto& event : m_periodicEvents){
                if (event.active && event.nextTime <= now) {
                    eventsToPublish.push_back(&event);
                    
                    // Atualizar próximo tempo de execução
                    event.nextTime = now + std::chrono::milliseconds(event.intervalMs);
                    
                    // Atualizar contador de repetiçõ/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna es
 */
es
                    if(event.repeatCount > 0){
                        event.repeatCount--;
                        if (event.repeatCount == 0) {
                            event.active = false;
                        }
                    }
                }
                
                // Atualizar próximo tempo de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param nextWakeup Parâmetro nextWakeup
 * @return Retorna despertar
 */
 despertar
                if(event.active && event.nextTime < nextWakeup){
                    nextWakeup = event.nextTime;
                }
            }
            
            // Remover eventos inativos
            m_periodicEvents.erase(
                std::remove_if(m_periodicEvents.begin(), m_periodicEvents.end(),
                              [](const PeriodicEvent& e) { return !e.active; }),
                m_periodicEvents.end());
            
            // Esperar até o próximo evento ou notificação
            if (eventsToPublish.empty()) {
                m_periodicEventsCV.wait_until(lock, nextWakeup, [this, nextWakeup] {
                    return !m_active || 
                           std::any_of(m_periodicEvents.begin(), m_periodicEvents.end(),
                                     [now = std::chrono::steady_clock::now()](const PeriodicEvent& e) {
                                         return e.active && e.nextTime <= now;
                                     });
                });
                
                if (!m_active) {
                    break;
                }
                
                continue;
            }
        }
        
        // Publicar eventos /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param eventsToPublish Parâmetro eventsToPublish
 * @return Retorna prontos
 */
 prontos
        for(const auto& event : eventsToPublish){
            dispatchEvent(event->eventData, event->eventType);
        }
    }
}

// Obtém próximo ID de assinante
SubscriberId EventBus::getNextSubscriberId() {
    return m_nextSubscriberId++;
}

// Obtém próximo ID de evento periódico
int EventBus::getNextPeriodicEventId() {

} // namespace wydbr

    return m_nextPeriodicEventId++;
}