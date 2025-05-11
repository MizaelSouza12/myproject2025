/**
 * @file EventSystem.h
 * @brief Sistema de eventos do WYDBR 2.0
 */

#ifndef WYDBR_EVENT_SYSTEM_H
#define WYDBR_EVENT_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include "wyd_core.h"

namespace WYDBR {

/**
 * @class EventSystem
 * @brief Gerencia o sistema de eventos do jogo
 */
class EventSystem {
public:
    static EventSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de eventos
    bool RegisterEvent(uint32_t eventId, const std::string& name, uint32_t duration);
    bool StartEvent(uint32_t eventId);
    bool StopEvent(uint32_t eventId);
    bool PauseEvent(uint32_t eventId);

    // Participação em eventos
    bool JoinEvent(uint32_t characterId, uint32_t eventId);
    bool LeaveEvent(uint32_t characterId, uint32_t eventId);
    bool IsParticipating(uint32_t characterId, uint32_t eventId) const;

    // Verificações
    bool IsEventActive(uint32_t eventId) const;
    bool IsEventFull(uint32_t eventId) const;
    bool CanJoinEvent(uint32_t characterId, uint32_t eventId) const;
    bool HasEventStarted(uint32_t eventId) const;

    // Queries
    std::string GetEventName(uint32_t eventId) const;
    uint32_t GetEventDuration(uint32_t eventId) const;
    uint32_t GetEventRemainingTime(uint32_t eventId) const;
    std::vector<uint32_t> GetEventParticipants(uint32_t eventId) const;

    // Utilidades
    void UpdateEvents(uint32_t deltaTime);
    void ProcessEventRewards(uint32_t eventId);
    void BroadcastEventMessage(uint32_t eventId, const std::string& message);
    void HandleEventCompletion(uint32_t eventId);

private:
    EventSystem() = default;
    ~EventSystem() = default;

    // Prevenir cópias
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;

    // Cálculos internos
    bool ValidateEvent(uint32_t eventId) const;
    bool CheckEventRequirements(uint32_t characterId, uint32_t eventId) const;
    void UpdateEventProgress(uint32_t eventId);
    void HandleEventFailure(uint32_t eventId);

    // Estado
    struct Event {
        std::string name;
        uint32_t duration;
        uint32_t startTime;
        uint32_t endTime;
        bool isActive;
        uint32_t maxParticipants;
        std::vector<uint32_t> participants;
    };

    struct EventProgress {
        uint32_t eventId;
        uint32_t startTime;
        uint32_t remainingTime;
        bool isComplete;
    };

    std::unordered_map<uint32_t, Event> m_events;
    std::unordered_map<uint32_t, EventProgress> m_eventProgress;
};

} // namespace WYDBR

#endif // WYDBR_EVENT_SYSTEM_H 