/**
 * QuestSystem.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/QuestSystem.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUEST_SYSTEM_H
#define QUEST_SYSTEM_H

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>

#include "headers/quest_structs.h"
#include "src/server/tmsrv/TMSrvGlobal.h"

namespace wydbr {


// Forward declarations
class TMServer;
class NPCManager;
class TMCharacter;
class ItemManager;

/**
 * Sistema avançado de Quests - Implementação moderna baseada na estrutura original
 * 
 * Esta classe gerencia todo o sistema de quests com fidelidade à estrutura original,
 * mas com implementação moderna para corrigir bugs e aumentar a performance.
 * 
 * Características:
 * - Usa estruturas de dados exatas do cliente original para compatibilidade
 * - Corrige bugs conhecidos do sistema original
 * - Implementa thread-safety para operações concorrentes
 * - Adiciona logging avançado para diagnóstico e auditoria
 * - Suporta quests condicionais e eventos dinâmicos
 * - Otimização de memória com pool de objetos
 */
/**
 * Classe WYDQuestSystem
 * 
 * Esta classe implementa a funcionalidade WYDQuestSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDQuestSystem
 * 
 * Esta classe implementa a funcionalidade WYDQuestSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestSystem {
public:
    // Singleton (padrão utilizado no servidor original)
    /**
 * GetInstance
 * 
 * Implementa a funcionalidade GetInstance conforme especificação original.
 * @return Retorna QuestSystem
 */

    static QuestSystem* GetInstance();
    
    // Inicialização e finalização
    /**
 * Initialize
 * 
 * Implementa a funcionalidade Initialize conforme especificação original.
 * @param pServer Parâmetro pServer
 * @return Retorna bool
 */

    bool Initialize(TMServer* pServer);
    /**
 * Shutdown
 * 
 * Implementa a funcionalidade Shutdown conforme especificação original.
 */

    void Shutdown();
    
    // Carregamento de dados
    /**
 * LoadQuestData
 * 
 * Implementa a funcionalidade LoadQuestData conforme especificação original.
 * @param "data/quests/QuestList.bin" Parâmetro "data/quests/QuestList.bin"
 * @return Retorna bool
 */

    bool LoadQuestData(const char* filename = "data/quests/QuestList.bin");
    /**
 * LoadQuestDialogues
 * 
 * Implementa a funcionalidade LoadQuestDialogues conforme especificação original.
 * @param "data/quests/QuestDialogue.bin" Parâmetro "data/quests/QuestDialogue.bin"
 * @return Retorna bool
 */

    bool LoadQuestDialogues(const char* filename = "data/quests/QuestDialogue.bin");
    /**
 * LoadQuestRewards
 * 
 * Implementa a funcionalidade LoadQuestRewards conforme especificação original.
 * @param "data/quests/QuestRewards.bin" Parâmetro "data/quests/QuestRewards.bin"
 * @return Retorna bool
 */

    bool LoadQuestRewards(const char* filename = "data/quests/QuestRewards.bin");
    /**
 * LoadQuestScripts
 * 
 * Implementa a funcionalidade LoadQuestScripts conforme especificação original.
 * @param "data/quests/QuestScripts.bin" Parâmetro "data/quests/QuestScripts.bin"
 * @return Retorna bool
 */

    bool LoadQuestScripts(const char* filename = "data/quests/QuestScripts.bin");
    
    // Verifica se a quest existe
    /**
 * IsValidQuest
 * 
 * Implementa a funcionalidade IsValidQuest conforme especificação original.
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    bool IsValidQuest(uint16_t questId) const;
    
    // Consulta dados de quest
    /**
 * GetQuestData
 * 
 * Implementa a funcionalidade GetQuestData conforme especificação original.
 * @param questId Parâmetro questId
 * @return Retorna QUEST_DATA
 */

    const QUEST_DATA* GetQuestData(uint16_t questId) const;
    
    // Operações principais de quest
    /**
 * StartQuest
 * 
 * Implementa a funcionalidade StartQuest conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @param 0 Parâmetro 0
 * @return Retorna int
 */

    int StartQuest(TMCharacter* pChar, uint16_t questId, uint16_t npcId = 0);
    /**
 * CompleteQuest
 * 
 * Implementa a funcionalidade CompleteQuest conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @param 0 Parâmetro 0
 * @return Retorna int
 */

    int CompleteQuest(TMCharacter* pChar, uint16_t questId, uint16_t npcId = 0);
    /**
 * AbandonQuest
 * 
 * Implementa a funcionalidade AbandonQuest conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna int
 */

    int AbandonQuest(TMCharacter* pChar, uint16_t questId);
    
    // Atualização de progresso
    /**
 * UpdateKillProgress
 * 
 * Implementa a funcionalidade UpdateKillProgress conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param mobId Parâmetro mobId
 * @param 1 Parâmetro 1
 * @return Retorna bool
 */

    bool UpdateKillProgress(TMCharacter* pChar, uint16_t mobId, uint16_t count = 1);
    /**
 * UpdateItemProgress
 * 
 * Implementa a funcionalidade UpdateItemProgress conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param itemId Parâmetro itemId
 * @param 1 Parâmetro 1
 * @return Retorna bool
 */

    bool UpdateItemProgress(TMCharacter* pChar, uint16_t itemId, uint16_t count = 1);
    /**
 * UpdateTalkProgress
 * 
 * Implementa a funcionalidade UpdateTalkProgress conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param npcId Parâmetro npcId
 * @return Retorna bool
 */

    bool UpdateTalkProgress(TMCharacter* pChar, uint16_t npcId);
    /**
 * UpdateVisitProgress
 * 
 * Implementa a funcionalidade UpdateVisitProgress conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool UpdateVisitProgress(TMCharacter* pChar, uint16_t mapId, uint16_t x, uint16_t y);
    
    // Triggers específicos
    /**
 * TriggerQuestEvent
 * 
 * Implementa a funcionalidade TriggerQuestEvent conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param eventId Parâmetro eventId
 * @param nullptr Parâmetro nullptr
 * @return Retorna bool
 */

    bool TriggerQuestEvent(TMCharacter* pChar, const char* eventId, const char* param = nullptr);
    
    // Verificações de status
    /**
 * HasActiveQuest
 * 
 * Implementa a funcionalidade HasActiveQuest conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    bool HasActiveQuest(TMCharacter* pChar, uint16_t questId) const;
    /**
 * CanStartQuest
 * 
 * Implementa a funcionalidade CanStartQuest conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @param 0 Parâmetro 0
 * @return Retorna bool
 */

    bool CanStartQuest(TMCharacter* pChar, uint16_t questId, uint16_t npcId = 0) const;
    /**
 * HasCompletedQuest
 * 
 * Implementa a funcionalidade HasCompletedQuest conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    bool HasCompletedQuest(TMCharacter* pChar, uint16_t questId) const;
    /**
 * CheckQuestPrerequisites
 * 
 * Implementa a funcionalidade CheckQuestPrerequisites conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    bool CheckQuestPrerequisites(TMCharacter* pChar, uint16_t questId) const;
    
    // Obtenção de quests disponíveis
    /**
 * GetAvailableQuestsForNPC
 * 
 * Implementa a funcionalidade GetAvailableQuestsForNPC conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param npcId Parâmetro npcId
 * @param outQuestIds Parâmetro outQuestIds
 */

    void GetAvailableQuestsForNPC(TMCharacter* pChar, uint16_t npcId, std::vector<uint16_t>& outQuestIds) const;
    /**
 * GetCompletableQuestsForNPC
 * 
 * Implementa a funcionalidade GetCompletableQuestsForNPC conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param npcId Parâmetro npcId
 * @param outQuestIds Parâmetro outQuestIds
 */

    void GetCompletableQuestsForNPC(TMCharacter* pChar, uint16_t npcId, std::vector<uint16_t>& outQuestIds) const;
    
    // Diálogos de quest
    /**
 * GetQuestDialogue
 * 
 * Implementa a funcionalidade GetQuestDialogue conforme especificação original.
 * @param questId Parâmetro questId
 * @param dialogueType Parâmetro dialogueType
 * @param nullptr Parâmetro nullptr
 * @return Retorna char
 */

    const char* GetQuestDialogue(uint16_t questId, uint8_t dialogueType, TMCharacter* pChar = nullptr) const;
    
    // Acesso a dados externos
    /**
 * SetNPCManager
 * 
 * Implementa a funcionalidade SetNPCManager conforme especificação original.
 * @param pNPCManager Parâmetro pNPCManager
 */

    void SetNPCManager(NPCManager* pNPCManager){ m_pNPCManager = pNPCManager; }
    /**
 * SetItemManager
 * 
 * Implementa a funcionalidade SetItemManager conforme especificação original.
 * @param pItemManager Parâmetro pItemManager
 */

    void SetItemManager(ItemManager* pItemManager){ m_pItemManager = pItemManager; }
    
    // Atualização periódica (expiração, eventos automáticos)
    /**
 * Update
 * 
 * Implementa a funcionalidade Update conforme especificação original.
 * @param tick Parâmetro tick
 */

    void Update(uint64_t tick);
    
    // Estatísticas
    /**
 * GetQuestCount
 * 
 * Implementa a funcionalidade GetQuestCount conforme especificação original.
 * @return Retorna size_t
 */

    size_t GetQuestCount() const{ return m_questData.size(); }
    /**
 * GetActiveQuestCount
 * 
 * Implementa a funcionalidade GetActiveQuestCount conforme especificação original.
 * @return Retorna size_t
 */

    size_t GetActiveQuestCount() const{ return m_activeQuestCount.load(); }
    
private:
    // Construtor/destrutor privados (singleton)
    QuestSystem();
    ~QuestSystem();
    
    // Estruturas de dados internas (modernizadas)
    struct QuestDialogueSet {
        std::string start;
        std::string progress;
        std::string complete;
        std::string fail;
    };
    
    struct PlayerQuestContext {
        uint32_t characterId;
        uint32_t lastUpdate;
        uint32_t questFlags;
        std::vector<uint16_t> trackedMobs;         // Mobs rastreados pelas quests ativas
        std::vector<uint16_t> trackedItems;        // Itens rastreados pelas quests ativas
        std::vector<uint16_t> trackedNPCs;         // NPCs rastreados pelas quests ativas
    };
    
    // Atualizações internas
    /**
 * GetPlayerQuest
 * 
 * Implementa a funcionalidade GetPlayerQuest conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna PLAYER_QUEST
 */

    PLAYER_QUEST* GetPlayerQuest(TMCharacter* pChar, uint16_t questId) const;
    /**
 * UpdateQuestProgress
 * 
 * Implementa a funcionalidade UpdateQuestProgress conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @param objectiveIndex Parâmetro objectiveIndex
 * @param progress Parâmetro progress
 * @return Retorna bool
 */

    bool UpdateQuestProgress(TMCharacter* pChar, uint16_t questId, uint8_t objectiveIndex, uint32_t progress);
    /**
 * GiveQuestRewards
 * 
 * Implementa a funcionalidade GiveQuestRewards conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    bool GiveQuestRewards(TMCharacter* pChar, uint16_t questId);
    /**
 * UpdatePlayerQuestContext
 * 
 * Implementa a funcionalidade UpdatePlayerQuestContext conforme especificação original.
 * @param pChar Parâmetro pChar
 */

    void UpdatePlayerQuestContext(TMCharacter* pChar);
    /**
 * ProcessAutoCompletion
 * 
 * Implementa a funcionalidade ProcessAutoCompletion conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param pQuest Parâmetro pQuest
 * @return Retorna bool
 */

    bool ProcessAutoCompletion(TMCharacter* pChar, PLAYER_QUEST* pQuest);
    
    // Helpers internos
    /**
 * IsQuestObjectiveComplete
 * 
 * Implementa a funcionalidade IsQuestObjectiveComplete conforme especificação original.
 * @param pQuestData Parâmetro pQuestData
 * @param pPlayerQuest Parâmetro pPlayerQuest
 * @param objectiveIndex Parâmetro objectiveIndex
 * @return Retorna bool
 */

    bool IsQuestObjectiveComplete(const QUEST_DATA* pQuestData, const PLAYER_QUEST* pPlayerQuest, uint8_t objectiveIndex) const;
    /**
 * HasAllRequiredObjectivesComplete
 * 
 * Implementa a funcionalidade HasAllRequiredObjectivesComplete conforme especificação original.
 * @param pQuestData Parâmetro pQuestData
 * @param pPlayerQuest Parâmetro pPlayerQuest
 * @return Retorna bool
 */

    bool HasAllRequiredObjectivesComplete(const QUEST_DATA* pQuestData, const PLAYER_QUEST* pPlayerQuest) const;
    /**
 * HasQuestItemSpace
 * 
 * Implementa a funcionalidade HasQuestItemSpace conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    bool HasQuestItemSpace(TMCharacter* pChar, uint16_t questId) const;
    /**
 * RemoveQuestItems
 * 
 * Implementa a funcionalidade RemoveQuestItems conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 */

    void RemoveQuestItems(TMCharacter* pChar, uint16_t questId);
    
    // Lógica de quests avançada
    /**
 * ProcessTimeLimitedQuests
 * 
 * Implementa a funcionalidade ProcessTimeLimitedQuests conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param tick Parâmetro tick
 */

    void ProcessTimeLimitedQuests(TMCharacter* pChar, uint64_t tick);
    /**
 * ProcessQuestCooldowns
 * 
 * Implementa a funcionalidade ProcessQuestCooldowns conforme especificação original.
 * @param pChar Parâmetro pChar
 */

    void ProcessQuestCooldowns(TMCharacter* pChar);
    /**
 * CheckGuildQuestRequirements
 * 
 * Implementa a funcionalidade CheckGuildQuestRequirements conforme especificação original.
 * @param pChar Parâmetro pChar
 */

    void CheckGuildQuestRequirements(TMCharacter* pChar);
    
    // Geradores de script para client
    /**
 * GenerateQuestFloatingText
 * 
 * Implementa a funcionalidade GenerateQuestFloatingText conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @param text Parâmetro text
 */

    void GenerateQuestFloatingText(TMCharacter* pChar, uint16_t questId, const char* text);
    /**
 * SendQuestProgressUpdate
 * 
 * Implementa a funcionalidade SendQuestProgressUpdate conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 */

    void SendQuestProgressUpdate(TMCharacter* pChar, uint16_t questId);
    
    // Cache e otimização
    /**
 * CacheQuestRequirements
 * 
 * Implementa a funcionalidade CacheQuestRequirements conforme especificação original.
 */

    void CacheQuestRequirements();
    /**
 * BuildQuestDependencyGraph
 * 
 * Implementa a funcionalidade BuildQuestDependencyGraph conforme especificação original.
 */

    void BuildQuestDependencyGraph();
    
    // Debugging e diagnóstico
    /**
 * LogQuestAction
 * 
 * Implementa a funcionalidade LogQuestAction conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @param action Parâmetro action
 * @param result Parâmetro result
 */

    void LogQuestAction(TMCharacter* pChar, uint16_t questId, const char* action, int result);
    
private:
    // Instância singleton
    static QuestSystem* ms_instance;
    
    // Dados fundamentais
    std::unordered_map<uint16_t, QUEST_DATA> m_questData;
    std::unordered_map<uint16_t, QUEST_REWARD> m_questRewards;
    std::unordered_map<uint16_t, QuestDialogueSet> m_questDialogues;
    
    // Índices para lookups rápidos
    std::unordered_map<uint16_t, std::vector<uint16_t>> m_questsByNPC;        // NPC ID -> Quest IDs
    std::unordered_map<uint16_t, std::vector<uint16_t>> m_questsByMap;        // Map ID -> Quest IDs
    std::unordered_map<uint16_t, std::vector<uint16_t>> m_questsByMob;        // Mob ID -> Quest IDs
    std::unordered_map<uint16_t, std::vector<uint16_t>> m_questsByItem;       // Item ID -> Quest IDs
    std::unordered_map<uint16_t, std::vector<uint16_t>> m_questsByLevel;      // Level -> Quest IDs
    std::unordered_map<uint16_t, std::vector<uint16_t>> m_questDependencies;  // Quest ID -> Dependent Quest IDs
    std::unordered_map<uint32_t, PlayerQuestContext> m_playerContexts;        // Char ID -> Quest Context
    
    // Contadores e limites
    std::atomic<size_t> m_activeQuestCount;
    std::atomic<uint64_t> m_questStartedCount;
    std::atomic<uint64_t> m_questCompletedCount;
    
    // Semáforos e mutexes para thread safety
    mutable std::mutex m_questDataMutex;
    mutable std::mutex m_playerMutex;
    
    // Referências a outros sistemas
    TMServer* m_pServer;
    NPCManager* m_pNPCManager;
    ItemManager* m_pItemManager;
    
    // Estado
    bool m_initialized;
    uint64_t m_lastUpdateTick;
};

#endif // QUEST_SYSTEM_H

} // namespace wydbr
