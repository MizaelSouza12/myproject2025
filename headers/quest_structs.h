/**
 * @file quest_structs.h
 * @brief Definição das estruturas de quests do WYD
 * 
 * Este arquivo contém as declarações completas para o sistema de quests
 * no WYDBR, com correções de segurança e melhorias.
 */

#ifndef WYDBR_QUEST_STRUCTS_H
#define WYDBR_QUEST_STRUCTS_H

#include <cstdint>
#include <string>
#include "wyd_core.h"
#include "item_struct.h"

namespace WYDBR {

// Tipo de quest
enum class QuestType : uint8_t {
    Kill = 0,
    Collect = 1,
    Escort = 2,
    Explore = 3,
    Craft = 4,
    Delivery = 5,
    Dialog = 6,
    Boss = 7,
    Special = 8
};

// Estado da quest
enum class QuestState : uint8_t {
    Unavailable = 0,  // Não disponível
    Available = 1,    // Disponível mas não aceita
    Active = 2,       // Aceita e em progresso
    Complete = 3,     // Completa mas não entregue
    Finished = 4,     // Entregue
    Failed = 5        // Falhou
};

// Requerimento de quest
struct QuestRequirement {
    QuestType type = QuestType::Kill;
    uint32_t targetId = 0;      // ID do alvo (mob, item, NPC)
    uint16_t amount = 0;        // Quantidade necessária
    uint16_t currentAmount = 0; // Quantidade atual
    
    // Verificação de progresso
    bool IsComplete() const {
        return currentAmount >= amount;
    }
    
    // Atualização segura de progresso
    bool UpdateProgress(uint32_t id, uint16_t count) {
        // Verificar se é o alvo correto
        if (id != targetId) {
            return false;
        }
        
        // Prevenir overflow
        uint32_t newAmount = static_cast<uint32_t>(currentAmount) + count;
        if (newAmount > amount) {
            currentAmount = amount;
        } else {
            currentAmount = static_cast<uint16_t>(newAmount);
        }
        
        return true;
    }
};

// Recompensa de quest
struct QuestReward {
    uint32_t experience = 0;
    uint32_t gold = 0;
    Item items[5];             // Até 5 itens de recompensa
    uint8_t itemCount = 0;     // Número de itens
    
    // Verificação de segurança ao adicionar item
    bool AddItem(const Item& item) {
        if (itemCount >= 5) {
            return false;
        }
        
        items[itemCount++] = item;
        return true;
    }
};

// Informações completas da quest
struct Quest {
    uint32_t questId = 0;
    char name[32] = {0};
    char description[256] = {0};
    uint8_t minLevel = 0;
    uint8_t maxLevel = 100;
    bool repeatable = false;
    uint32_t questFlags = 0;    // Flags de comportamento
    
    QuestState state = QuestState::Unavailable;
    QuestRequirement requirements[MAX_NPC_QUEST];
    uint8_t requirementCount = 0;
    
    QuestReward reward;
    
    uint32_t npcStartId = 0;    // NPC que oferece a quest
    uint32_t npcEndId = 0;      // NPC que conclui a quest
    
    uint32_t startTime = 0;     // Quando a quest foi iniciada
    uint32_t timeLimit = 0;     // Tempo limite (0 = sem limite)
    
    uint32_t previousQuestId = 0; // Quest necessária como pré-requisito
    
    // Verificar se os requisitos da quest foram cumpridos
    bool AreRequirementsMet() const {
        for (uint8_t i = 0; i < requirementCount; ++i) {
            if (!requirements[i].IsComplete()) {
                return false;
            }
        }
        return true;
    }
    
    // Verificar se a quest expirou
    bool HasExpired(uint32_t currentTime) const {
        if (timeLimit == 0) {
            return false; // Sem limite de tempo
        }
        
        return (currentTime - startTime) > timeLimit;
    }
    
    // Verificar se o jogador atende aos requisitos
    bool CanAccept(uint8_t playerLevel, uint32_t completedQuests[], size_t completedQuestCount) const {
        // Verificar nível
        if (playerLevel < minLevel || playerLevel > maxLevel) {
            return false;
        }
        
        // Verificar quest prévia (se necessário)
        if (previousQuestId != 0) {
            bool foundPreviousQuest = false;
            for (size_t i = 0; i < completedQuestCount; ++i) {
                if (completedQuests[i] == previousQuestId) {
                    foundPreviousQuest = true;
                    break;
                }
            }
            
            if (!foundPreviousQuest) {
                return false;
            }
        }
        
        return true;
    }
    
    // Registrar progresso em uma tarefa
    bool UpdateTaskProgress(QuestType type, uint32_t targetId, uint16_t count) {
        if (state != QuestState::Active) {
            return false;
        }
        
        bool updated = false;
        for (uint8_t i = 0; i < requirementCount; ++i) {
            QuestRequirement& req = requirements[i];
            if (req.type == type && req.targetId == targetId) {
                updated = req.UpdateProgress(targetId, count);
            }
        }
        
        // Verificar se a quest foi concluída
        if (updated && AreRequirementsMet()) {
            state = QuestState::Complete;
        }
        
        return updated;
    }
    
    // Calcular recompensa baseada no nível do jogador (segurança)
    void CalculateScaledReward(uint8_t playerLevel) {
        // Escalar experiência baseada no nível
        float levelMultiplier = 1.0f + (playerLevel - minLevel) * 0.1f;
        
        // Limitar multiplicador para prevenir exploits
        if (levelMultiplier < 0.5f) levelMultiplier = 0.5f;
        if (levelMultiplier > 2.0f) levelMultiplier = 2.0f;
        
        uint32_t scaledExp = static_cast<uint32_t>(reward.experience * levelMultiplier);
        
        // Impedir overflow
        if (scaledExp < reward.experience) {
            scaledExp = UINT32_MAX;
        }
        
        reward.experience = scaledExp;
    }
    
    // Verificar e sanitizar strings (segurança)
    void SanitizeStrings() {
        // Garantir terminador nulo para nome
        name[sizeof(name) - 1] = '\0';
        
        // Garantir terminador nulo para descrição
        description[sizeof(description) - 1] = '\0';
        
        // Remover caracteres potencialmente perigosos
        for (size_t i = 0; i < sizeof(name) - 1 && name[i] != '\0'; ++i) {
            if (name[i] < 32 || name[i] > 126) {
                name[i] = '_';
            }
        }
        
        for (size_t i = 0; i < sizeof(description) - 1 && description[i] != '\0'; ++i) {
            if (description[i] < 32 || description[i] > 126) {
                description[i] = '_';
            }
        }
    }
};

// Rastreador de quests do jogador
struct QuestLog {
    static const int MAX_ACTIVE_QUESTS = 10;
    static const int MAX_COMPLETED_QUESTS = 100;
    
    Quest activeQuests[MAX_ACTIVE_QUESTS];
    uint8_t activeQuestCount = 0;
    
    uint32_t completedQuestIds[MAX_COMPLETED_QUESTS];
    uint16_t completedQuestCount = 0;
    
    // Adicionar uma quest à lista ativa
    bool AddQuest(const Quest& quest) {
        // Verificar se já atingiu o limite de quests ativas
        if (activeQuestCount >= MAX_ACTIVE_QUESTS) {
            return false;
        }
        
        // Verificar se a quest já está ativa
        for (uint8_t i = 0; i < activeQuestCount; ++i) {
            if (activeQuests[i].questId == quest.questId) {
                return false;
            }
        }
        
        // Adicionar a quest
        activeQuests[activeQuestCount++] = quest;
        return true;
    }
    
    // Remover uma quest da lista ativa
    bool RemoveQuest(uint32_t questId) {
        for (uint8_t i = 0; i < activeQuestCount; ++i) {
            if (activeQuests[i].questId == questId) {
                // Deslocar todas as quests após esta
                for (uint8_t j = i; j < activeQuestCount - 1; ++j) {
                    activeQuests[j] = activeQuests[j + 1];
                }
                
                // Reduzir contador de quests ativas
                activeQuestCount--;
                return true;
            }
        }
        
        return false;
    }
    
    // Completar uma quest
    bool CompleteQuest(uint32_t questId) {
        // Encontrar a quest ativa
        for (uint8_t i = 0; i < activeQuestCount; ++i) {
            if (activeQuests[i].questId == questId) {
                // Verificar se a quest está pronta para ser completada
                if (activeQuests[i].state == QuestState::Complete) {
                    // Adicionar à lista de quests completadas
                    if (completedQuestCount < MAX_COMPLETED_QUESTS) {
                        completedQuestIds[completedQuestCount++] = questId;
                    }
                    
                    // Remover da lista de ativas
                    RemoveQuest(questId);
                    return true;
                }
                break;
            }
        }
        
        return false;
    }
    
    // Verificar se a quest foi completada
    bool IsQuestCompleted(uint32_t questId) const {
        for (uint16_t i = 0; i < completedQuestCount; ++i) {
            if (completedQuestIds[i] == questId) {
                return true;
            }
        }
        
        return false;
    }
    
    // Atualizar progresso de todas as quests ativas
    void UpdateAllQuestsProgress(QuestType type, uint32_t targetId, uint16_t count) {
        for (uint8_t i = 0; i < activeQuestCount; ++i) {
            Quest& quest = activeQuests[i];
            
            if (quest.state == QuestState::Active) {
                quest.UpdateTaskProgress(type, targetId, count);
            }
        }
    }
    
    // Verificar quests expiradas
    void CheckExpiredQuests(uint32_t currentTime) {
        for (uint8_t i = 0; i < activeQuestCount; ++i) {
            Quest& quest = activeQuests[i];
            
            if (quest.state == QuestState::Active && quest.HasExpired(currentTime)) {
                quest.state = QuestState::Failed;
            }
        }
    }
};

} // namespace WYDBR

#endif // WYDBR_QUEST_STRUCTS_H
