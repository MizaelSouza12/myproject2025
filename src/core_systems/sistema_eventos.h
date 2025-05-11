#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace WYD {
    class SistemaEventos {
    private:
        struct EventData {
            DWORD id;
            std::string name;
            std::string description;
            DWORD startTime;
            DWORD endTime;
            DWORD minLevel;
            DWORD maxLevel;
            DWORD maxParticipants;
            DWORD currentParticipants;
            bool isActive;
            std::vector<DWORD> rewards;
        };

        struct EventParticipant {
            DWORD characterId;
            DWORD joinTime;
            DWORD score;
            bool isActive;
        };

        struct EventReward {
            DWORD itemId;
            DWORD quantity;
            DWORD minScore;
            DWORD maxScore;
        };

        std::unordered_map<DWORD, EventData> eventDatabase;
        std::unordered_map<DWORD, std::vector<EventParticipant>> eventParticipants;
        std::unordered_map<DWORD, std::vector<EventReward>> eventRewards;

        // Callbacks para eventos
        std::unordered_map<DWORD, std::function<void(DWORD, DWORD)>> eventStartCallbacks;
        std::unordered_map<DWORD, std::function<void(DWORD, DWORD)>> eventEndCallbacks;
        std::unordered_map<DWORD, std::function<void(DWORD, DWORD)>> participantJoinCallbacks;
        std::unordered_map<DWORD, std::function<void(DWORD, DWORD)>> participantLeaveCallbacks;

    public:
        SistemaEventos() = default;
        ~SistemaEventos() = default;

        // Inicialização
        void initializeEventSystem() {
            // Inicializar eventos padrão
            initializeDefaultEvents();
        }

        // Gerenciamento de eventos
        bool createEvent(const EventData& event) {
            if (eventDatabase.find(event.id) != eventDatabase.end()) {
                return false;
            }

            eventDatabase[event.id] = event;
            eventParticipants[event.id] = std::vector<EventParticipant>();
            return true;
        }

        bool removeEvent(DWORD eventId) {
            if (eventDatabase.erase(eventId) > 0) {
                eventParticipants.erase(eventId);
                eventRewards.erase(eventId);
                return true;
            }
            return false;
        }

        // Participação em eventos
        bool joinEvent(DWORD eventId, DWORD characterId) {
            auto eventIt = eventDatabase.find(eventId);
            if (eventIt == eventDatabase.end() || !eventIt->second.isActive) {
                return false;
            }

            auto& event = eventIt->second;
            auto& participants = eventParticipants[eventId];

            // Verificar limite de participantes
            if (participants.size() >= event.maxParticipants) {
                return false;
            }

            // Verificar se já está participando
            auto it = std::find_if(participants.begin(), participants.end(),
                [characterId](const EventParticipant& p) {
                    return p.characterId == characterId;
                });

            if (it != participants.end()) {
                return false;
            }

            // Adicionar participante
            EventParticipant participant{};
            participant.characterId = characterId;
            participant.joinTime = GetTickCount();
            participant.score = 0;
            participant.isActive = true;
            participants.push_back(participant);

            // Chamar callback
            auto callbackIt = participantJoinCallbacks.find(eventId);
            if (callbackIt != participantJoinCallbacks.end()) {
                callbackIt->second(eventId, characterId);
            }

            return true;
        }

        bool leaveEvent(DWORD eventId, DWORD characterId) {
            auto& participants = eventParticipants[eventId];
            
            auto it = std::find_if(participants.begin(), participants.end(),
                [characterId](const EventParticipant& p) {
                    return p.characterId == characterId;
                });

            if (it == participants.end()) {
                return false;
            }

            it->isActive = false;

            // Chamar callback
            auto callbackIt = participantLeaveCallbacks.find(eventId);
            if (callbackIt != participantLeaveCallbacks.end()) {
                callbackIt->second(eventId, characterId);
            }

            return true;
        }

        // Sistema de pontuação
        bool addScore(DWORD eventId, DWORD characterId, DWORD points) {
            auto& participants = eventParticipants[eventId];
            
            auto it = std::find_if(participants.begin(), participants.end(),
                [characterId](const EventParticipant& p) {
                    return p.characterId == characterId;
                });

            if (it == participants.end() || !it->isActive) {
                return false;
            }

            it->score += points;
            return true;
        }

        // Sistema de recompensas
        bool addReward(DWORD eventId, const EventReward& reward) {
            eventRewards[eventId].push_back(reward);
            return true;
        }

        bool distributeRewards(DWORD eventId) {
            auto& participants = eventParticipants[eventId];
            auto& rewards = eventRewards[eventId];

            // Ordenar participantes por pontuação
            std::sort(participants.begin(), participants.end(),
                [](const EventParticipant& a, const EventParticipant& b) {
                    return a.score > b.score;
                });

            // Distribuir recompensas
            for (const auto& participant : participants) {
                if (!participant.isActive) continue;

                for (const auto& reward : rewards) {
                    if (participant.score >= reward.minScore && participant.score <= reward.maxScore) {
                        // Implementar distribuição de recompensa
                        distributeReward(participant.characterId, reward);
                    }
                }
            }

            return true;
        }

        // Callbacks
        void registerEventStartCallback(DWORD eventId, std::function<void(DWORD, DWORD)> callback) {
            eventStartCallbacks[eventId] = callback;
        }

        void registerEventEndCallback(DWORD eventId, std::function<void(DWORD, DWORD)> callback) {
            eventEndCallbacks[eventId] = callback;
        }

        void registerParticipantJoinCallback(DWORD eventId, std::function<void(DWORD, DWORD)> callback) {
            participantJoinCallbacks[eventId] = callback;
        }

        void registerParticipantLeaveCallback(DWORD eventId, std::function<void(DWORD, DWORD)> callback) {
            participantLeaveCallbacks[eventId] = callback;
        }

        // Getters
        const EventData& getEventData(DWORD eventId) const {
            return eventDatabase.at(eventId);
        }

        const std::vector<EventParticipant>& getEventParticipants(DWORD eventId) const {
            return eventParticipants.at(eventId);
        }

        std::vector<DWORD> getActiveEvents() const {
            std::vector<DWORD> active;
            for (const auto& event : eventDatabase) {
                if (event.second.isActive) {
                    active.push_back(event.first);
                }
            }
            return active;
        }

    private:
        void initializeDefaultEvents() {
            // Implementar inicialização dos eventos padrão
        }

        void distributeReward(DWORD characterId, const EventReward& reward) {
            // Implementar distribuição de recompensa
        }
    };
} 