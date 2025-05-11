#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

namespace WYD {
    class SistemaQuests {
    private:
        struct QuestObjective {
            enum class Type {
                KILL_MOB,
                COLLECT_ITEM,
                TALK_TO_NPC,
                REACH_LOCATION,
                CRAFT_ITEM,
                COMPLETE_QUEST
            };

            Type type;
            DWORD targetId;
            DWORD requiredAmount;
            DWORD currentAmount;
            bool isCompleted;
        };

        struct QuestReward {
            DWORD experience;
            DWORD gold;
            std::vector<ItemData> items;
            std::vector<DWORD> skills;
        };

        struct QuestData {
            DWORD id;
            std::string name;
            std::string description;
            BYTE level;
            std::vector<DWORD> prerequisites;
            std::vector<QuestObjective> objectives;
            QuestReward rewards;
            bool isRepeatable;
            DWORD cooldown;
        };

        struct QuestState {
            bool isActive;
            bool isCompleted;
            DWORD startTime;
            DWORD completionTime;
            std::vector<QuestObjective> currentObjectives;
        };

        std::unordered_map<DWORD, QuestData> questDatabase;
        std::unordered_map<DWORD, std::unordered_map<DWORD, QuestState>> characterQuests;

    public:
        SistemaQuests() = default;
        ~SistemaQuests() = default;

        // Inicialização
        void initializeQuestSystem() {
            // Inicializar quests base
            initializeBaseQuests();
        }

        // Gerenciamento de quests
        bool startQuest(DWORD characterId, DWORD questId) {
            // Verificar se a quest existe
            if (questDatabase.find(questId) == questDatabase.end()) {
                return false;
            }

            auto& characterQuestMap = characterQuests[characterId];
            
            // Verificar se a quest já está ativa ou completada
            auto it = characterQuestMap.find(questId);
            if (it != characterQuestMap.end()) {
                if (it->second.isActive || 
                    (it->second.isCompleted && !questDatabase[questId].isRepeatable)) {
                    return false;
                }
            }

            // Verificar pré-requisitos
            if (!checkQuestPrerequisites(characterId, questId)) {
                return false;
            }

            // Inicializar estado da quest
            QuestState state{};
            state.isActive = true;
            state.isCompleted = false;
            state.startTime = GetTickCount();
            state.currentObjectives = questDatabase[questId].objectives;
            characterQuestMap[questId] = state;

            return true;
        }

        bool updateQuestObjective(DWORD characterId, DWORD questId, 
                                QuestObjective::Type type, DWORD targetId, DWORD amount = 1) {
            auto& characterQuestMap = characterQuests[characterId];
            auto it = characterQuestMap.find(questId);
            
            if (it == characterQuestMap.end() || !it->second.isActive) {
                return false;
            }

            auto& state = it->second;
            bool objectiveUpdated = false;

            for (auto& objective : state.currentObjectives) {
                if (objective.type == type && objective.targetId == targetId && !objective.isCompleted) {
                    objective.currentAmount += amount;
                    if (objective.currentAmount >= objective.requiredAmount) {
                        objective.isCompleted = true;
                    }
                    objectiveUpdated = true;
                }
            }

            // Verificar se todos os objetivos foram completados
            if (objectiveUpdated && checkQuestCompletion(state)) {
                completeQuest(characterId, questId);
            }

            return objectiveUpdated;
        }

        bool completeQuest(DWORD characterId, DWORD questId) {
            auto& characterQuestMap = characterQuests[characterId];
            auto it = characterQuestMap.find(questId);
            
            if (it == characterQuestMap.end() || !it->second.isActive) {
                return false;
            }

            auto& state = it->second;
            const auto& quest = questDatabase[questId];

            // Aplicar recompensas
            applyQuestRewards(characterId, quest.rewards);

            // Atualizar estado
            state.isActive = false;
            state.isCompleted = true;
            state.completionTime = GetTickCount();

            return true;
        }

        // Getters
        const QuestData& getQuestData(DWORD questId) const {
            return questDatabase.at(questId);
        }

        bool isQuestActive(DWORD characterId, DWORD questId) const {
            auto it = characterQuests.at(characterId).find(questId);
            return (it != characterQuests.at(characterId).end()) ? it->second.isActive : false;
        }

        bool isQuestCompleted(DWORD characterId, DWORD questId) const {
            auto it = characterQuests.at(characterId).find(questId);
            return (it != characterQuests.at(characterId).end()) ? it->second.isCompleted : false;
        }

        std::vector<DWORD> getActiveQuests(DWORD characterId) const {
            std::vector<DWORD> activeQuests;
            const auto& characterQuestMap = characterQuests.at(characterId);
            
            for (const auto& [questId, state] : characterQuestMap) {
                if (state.isActive) {
                    activeQuests.push_back(questId);
                }
            }
            
            return activeQuests;
        }

    private:
        void initializeBaseQuests() {
            // Quest de tutorial
            QuestData tutorialQuest{};
            tutorialQuest.id = 1;
            tutorialQuest.name = "Tutorial";
            tutorialQuest.description = "Aprenda os básicos do jogo";
            tutorialQuest.level = 1;
            tutorialQuest.isRepeatable = false;
            tutorialQuest.cooldown = 0;

            // Objetivos do tutorial
            QuestObjective talkToGuide{};
            talkToGuide.type = QuestObjective::Type::TALK_TO_NPC;
            talkToGuide.targetId = 1; // ID do NPC guia
            talkToGuide.requiredAmount = 1;
            talkToGuide.currentAmount = 0;
            talkToGuide.isCompleted = false;
            tutorialQuest.objectives.push_back(talkToGuide);

            // Recompensas do tutorial
            QuestReward tutorialRewards{};
            tutorialRewards.experience = 100;
            tutorialRewards.gold = 50;
            tutorialQuest.rewards = tutorialRewards;

            questDatabase[tutorialQuest.id] = tutorialQuest;
        }

        bool checkQuestPrerequisites(DWORD characterId, DWORD questId) {
            const auto& quest = questDatabase[questId];
            const auto& characterQuestMap = characterQuests[characterId];

            for (DWORD prereqId : quest.prerequisites) {
                auto it = characterQuestMap.find(prereqId);
                if (it == characterQuestMap.end() || !it->second.isCompleted) {
                    return false;
                }
            }
            return true;
        }

        bool checkQuestCompletion(const QuestState& state) {
            for (const auto& objective : state.currentObjectives) {
                if (!objective.isCompleted) {
                    return false;
                }
            }
            return true;
        }

        void applyQuestRewards(DWORD characterId, const QuestReward& rewards) {
            // Implementar lógica de recompensas
            // - Adicionar experiência
            // - Adicionar ouro
            // - Adicionar itens
            // - Desbloquear habilidades
        }
    };
} 