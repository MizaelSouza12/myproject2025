/**
 * EventBus.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/core/EventBus.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYDBR_CORE_EVENT_BUS_H
#define WYDBR_CORE_EVENT_BUS_H

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <vector>
#include <mutex>
#include <any>
#include <typeindex>
#include <typeinfo>

namespace wydbr {
namespace core {

/**
 * @class Event
 * @brief Classe base para todos os eventos
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
     * @brief Destrutor virtual
     */
    virtual ~Event() = default;
    
    /**
     * @brief Obtém o nome do evento
     * @return Nome do evento
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Obtém o tipo do evento
     * @return Tipo do evento como std::type_index
     */
    virtual std::type_index getType() const = 0;
    
    /**
     * @brief Obtém o timestamp do evento
     * @return Timestamp do evento em milissegundos desde epoch
     */
    int64_t getTimestamp() const { return timestamp; }
    
    /**
     * @brief Define o timestamp do evento
     * @param ts Timestamp em milissegundos desde epoch
     */
    void setTimestamp(int64_t ts) { timestamp = ts; }

private:
    int64_t timestamp = 0; ///< Timestamp do evento
};

/**
 * @class EventBus
 * @brief Sistema de eventos baseado em publish-subscribe
 * 
 * Permite que componentes publiquem eventos e se inscrevam para
 * receber notificações de eventos específicos.
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
     * @brief Tipo de função de callback para eventos
     */
    using EventCallback = std::function<void(const Event&)>;
    
    /**
     * @brief Construtor
     */
    EventBus();
    
    /**
     * @brief Destrutor
     */
    ~EventBus();
    
    /**
     * @brief Publica um evento
     * @param event Evento a ser publicado
     */
    void publish(const Event& event);
    
    /**
     * @brief Inscreve um callback para um tipo específico de evento
     * @tparam T Tipo do evento
     * @param callback Função a ser chamada quando o evento ocorrer
     * @return ID da inscrição (usado para cancelar)
     */
    template<typename T>
    /**
 * subscribe
 * 
 * Implementa a funcionalidade subscribe conforme especificação original.
 * @param callback Parâmetro callback
 * @return Retorna uint32_t
 */

    uint32_t subscribe(EventCallback callback){
        static_assert(std::is_base_of<Event, T>::value, "T must derive from Event");
        
        std::lock_guard<std::mutex> lock(mutex);
        
        uint32_t id = nextSubscriptionId++;
        subscriptions[std::type_index(typeid(T))].push_back({id, callback});
        
        return id;
    }
    
    /**
     * @brief Inscreve um callback para eventos com um nome específico
     * @param eventName Nome do evento
     * @param callback Função a ser chamada quando o evento ocorrer
     * @return ID da inscrição (usado para cancelar)
     */
    uint32_t subscribeByName(const std::string& eventName, EventCallback callback);
    
    /**
     * @brief Cancela uma inscrição
     * @param subscriptionId ID da inscrição a ser cancelada
     * @return true se a inscrição foi encontrada e cancelada
     */
    bool unsubscribe(uint32_t subscriptionId);
    
    /**
     * @brief Cancela todas as inscrições para um tipo específico de evento
     * @tparam T Tipo do evento
     * @return Número de inscrições canceladas
     */
    template<typename T>
    /**
 * unsubscribeAll
 * 
 * Implementa a funcionalidade unsubscribeAll conforme especificação original.
 * @return Retorna size_t
 */

    size_t unsubscribeAll(){
        static_assert(std::is_base_of<Event, T>::value, "T must derive from Event");
        
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = subscriptions.find(std::type_index(typeid(T)));
        if (it == subscriptions.end()) {
            return 0;
        }
        
        size_t count = it->second.size();
        subscriptions.erase(it);
        
        return count;
    }
    
    /**
     * @brief Cancela todas as inscrições para eventos com um nome específico
     * @param eventName Nome do evento
     * @return Número de inscrições canceladas
     */
    size_t unsubscribeAllByName(const std::string& eventName);
    
    /**
     * @brief Limpa todas as inscrições
     */
    void clear();
    
    /**
     * @brief Obtém estatísticas do barramento de eventos
     * @return Estatísticas como um mapa de string para any
     */
    std::map<std::string, std::any> getStats() const;

private:
    struct Subscription {
        uint32_t id;
        EventCallback callback;
    };
    
    std::map<std::type_index, std::vector<Subscription>> subscriptions;
    std::map<std::string, std::vector<Subscription>> subscriptionsByName;
    uint32_t nextSubscriptionId;
    mutable std::mutex mutex;
    
    // Estatísticas
    std::atomic<uint64_t> totalEventsPublished;
    std::atomic<uint64_t> totalCallbacksInvoked;
};

} // namespace core
} // namespace wydbr

#endif // WYDBR_CORE_EVENT_BUS_H