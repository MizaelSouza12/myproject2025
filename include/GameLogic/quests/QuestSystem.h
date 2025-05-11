/**
 * @file QuestSystem.h
 * @brief Sistema de quests do WYDBR 2.0
 */

#ifndef WYDBR_QUEST_SYSTEM_H
#define WYDBR_QUEST_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "wyd_core.h"
#include "quest_structs.h"

namespace WYDBR {

/**
 * @class QuestSystem
 * @brief Gerencia o sistema de quests do jogo
 */
class QuestSystem {
public:
    static QuestSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de quests
    bool AcceptQuest(uint32_t characterId, uint32_t questId);
    bool CompleteQuest(uint32_t characterId, uint32_t questId);
    bool AbandonQuest(uint32_t characterId, uint32_t questId);
    bool UpdateQuestProgress(uint32_t characterId, uint32_t questId, uint32_t objectiveId, uint16_t progress);

    // Verificações
    bool HasQuest(uint32_t characterId, uint32_t questId) const;
    bool CanAcceptQuest(uint32_t characterId, uint32_t questId) const;
    bool IsQuestComplete(uint32_t characterId, uint32_t questId) const;
    bool IsQuestFailed(uint32_t characterId, uint32_t questId) const;

    // Queries
    QuestState GetQuestState(uint32_t characterId, uint32_t questId) const;
    uint16_t GetQuestProgress(uint32_t characterId, uint32_t questId, uint32_t objectiveId) const;
    std::vector<uint32_t> GetActiveQuests(uint32_t characterId) const;
    std::vector<uint32_t> GetAvailableQuests(uint32_t characterId) const;

    // Utilidades
    void UpdateQuestTimers(uint32_t characterId, uint32_t deltaTime);
    void CheckQuestPrerequisites(uint32_t characterId);
    void GiveQuestRewards(uint32_t characterId, uint32_t questId);
    void ResetQuestProgress(uint32_t characterId, uint32_t questId);

private:
    QuestSystem() = default;
    ~QuestSystem() = default;

    // Prevenir cópias
    QuestSystem(const QuestSystem&) = delete;
    QuestSystem& operator=(const QuestSystem&) = delete;

    // Cálculos internos
    bool ValidateQuest(uint32_t questId) const;
    bool CheckQuestRequirements(uint32_t characterId, uint32_t questId) const;
    void UpdateQuestObjectives(uint32_t characterId, uint32_t questId);
    void HandleQuestCompletion(uint32_t characterId, uint32_t questId);

    // Estado
    struct QuestProgress {
        QuestState state;
        uint32_t startTime;
        uint32_t completionTime;
        std::vector<uint16_t> objectiveProgress;
    };

    struct CharacterQuests {
        std::unordered_map<uint32_t, QuestProgress> quests;
        std::vector<uint32_t> completedQuests;
    };

    std::unordered_map<uint32_t, CharacterQuests> m_characterQuests;
    std::unordered_map<uint32_t, Quest> m_questDatabase;
};

} // namespace WYDBR

#endif // WYDBR_QUEST_SYSTEM_H 