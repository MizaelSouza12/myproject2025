/**
 * QuestEngine.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/engine/QuestEngine.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUESTENGINE_H
#define QUESTENGINE_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <condition_variable>

#include "../../../../core/WYDTypes.h"
#include "../QuestTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file QuestEngine.h
 * @brief Motor principal do sistema de quests
 * 
 * Este arquivo contém a definição do motor principal do sistema de quests,
 * responsável por gerenciar todas as quests do jogo, seus estados e progresso.
 * No WYD original, o sistema de quests era extremamente limitado e com vários bugs.
 * Esta implementação oferece um sistema moderno e extensível.
 */

namespace wyd {
namespace server {
namespace quest {

// Forward declarations
class QuestTemplate;
class QuestTemplateManager;
class QuestProgressManager;
class QuestRewardManager;
class QuestScriptManager;
class QuestTriggerManager;
class QuestValidatorManager;

/**
 * @brief Motor principal do sistema de quests
 * 
 * Esta classe gerencia todo o sistema de quests, incluindo templates,
 * progresso, recompensas, scripts, gatilhos e validação.
 */
/**
 * Classe WYDQuestEngine
 * 
 * Esta classe implementa a funcionalidade WYDQuestEngine conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestEngine {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static QuestEngine& GetInstance();
    
    /**
     * @brief Inicializa o motor
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o motor
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o motor
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Carrega configurações
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadConfig(const std::string& filePath);
    
    /**
     * @brief Salva configurações
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveConfig(const std::string& filePath);
    
    /**
     * @brief Carrega todos os dados de quest
     * @param directory Diretório base
     * @return true se carregado com sucesso
     */
    bool LoadAllQuestData(const std::string& directory);
    
    /**
     * @brief Salva todos os dados de quest
     * @param directory Diretório base
     * @return true se salvo com sucesso
     */
    bool SaveAllQuestData(const std::string& directory);
    
    /**
     * @brief Verifica se um jogador pode aceitar uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se puder aceitar
     */
    bool CanPlayerAcceptQuest(player::CPlayer* player, QuestID questID, std::string& errorReason);
    
    /**
     * @brief Verifica se um jogador pode completar uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se puder completar
     */
    bool CanPlayerCompleteQuest(player::CPlayer* player, QuestID questID, std::string& errorReason);
    
    /**
     * @brief Verifica se um jogador pode abandonar uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se puder abandonar
     */
    bool CanPlayerAbandonQuest(player::CPlayer* player, QuestID questID, std::string& errorReason);
    
    /**
     * @brief Aceita uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @param sourceNPC ID do NPC de origem (0 se não for de NPC)
     * @param sourceItem ID do item de origem (0 se não for de item)
     * @param errorReason Razão do erro (saída)
     * @return true se aceita com sucesso
     */
    bool AcceptQuest(player::CPlayer* player, QuestID questID, WORD sourceNPC, WORD sourceItem, std::string& errorReason);
    
    /**
     * @brief Completa uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @param chosenRewards Lista de IDs de recompensas escolhidas
     * @param targetNPC ID do NPC alvo (0 se não for para NPC)
     * @param errorReason Razão do erro (saída)
     * @return true se completada com sucesso
     */
    bool CompleteQuest(player::CPlayer* player, QuestID questID, const std::vector<WORD>& chosenRewards, 
                       WORD targetNPC, std::string& errorReason);
    
    /**
     * @brief Abandona uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se abandonada com sucesso
     */
    bool AbandonQuest(player::CPlayer* player, QuestID questID, std::string& errorReason);
    
    /**
     * @brief Avança objetivos de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @param count Contagem a avançar
     * @param sourceID ID da fonte (mob, item, etc.)
     * @return true se avançado com sucesso
     */
    bool AdvanceQuestObjective(player::CPlayer* player, QuestID questID, WORD objectiveID, 
                              DWORD count, DWORD sourceID);
    
    /**
     * @brief Avança objetivos por evento
     * @param player Jogador
     * @param eventType Tipo de evento
     * @param sourceID ID da fonte
     * @param targetID ID do alvo
     * @param count Contagem
     * @return Número de objetivos avançados
     */
    int AdvanceQuestObjectivesByEvent(player::CPlayer* player, QuestTriggerType eventType, 
                                      DWORD sourceID, DWORD targetID, DWORD count = 1);
    
    /**
     * @brief Avança estado de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param newStateID ID do novo estado
     * @param sourceNPC ID do NPC de origem (0 se não for de NPC)
     * @param errorReason Razão do erro (saída)
     * @return true se avançado com sucesso
     */
    bool AdvanceQuestState(player::CPlayer* player, QuestID questID, WORD newStateID, 
                          WORD sourceNPC, std::string& errorReason);
    
    /**
     * @brief Obtém o estado atual de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return ID do estado atual, ou 0 se não encontrado
     */
    WORD GetQuestCurrentState(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Obtém a definição de estado de uma quest
     * @param questID ID da quest
     * @param stateID ID do estado
     * @return Ponteiro para a definição, ou nullptr se não encontrado
     */
    const QuestStateDefinition* GetQuestStateDefinition(QuestID questID, WORD stateID);
    
    /**
     * @brief Processa um diálogo de quest
     * @param player Jogador
     * @param questID ID da quest
     * @param stateID ID do estado
     * @param optionID ID da opção
     * @param npcID ID do NPC
     * @param errorReason Razão do erro (saída)
     * @return ID do próximo estado, ou 0 se erro
     */
    WORD ProcessQuestDialogue(player::CPlayer* player, QuestID questID, WORD stateID, 
                             WORD optionID, WORD npcID, std::string& errorReason);
    
    /**
     * @brief Obtém quests disponíveis para um jogador
     * @param player Jogador
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetAvailableQuestsForPlayer(player::CPlayer* player);
    
    /**
     * @brief Obtém quests ativas para um jogador
     * @param player Jogador
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetActiveQuestsForPlayer(player::CPlayer* player);
    
    /**
     * @brief Obtém quests completadas para um jogador
     * @param player Jogador
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetCompletedQuestsForPlayer(player::CPlayer* player);
    
    /**
     * @brief Obtém o status de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return Status da quest
     */
    QuestStatus GetQuestStatusForPlayer(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Obtém o estado detalhado de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return Estado detalhado, ou nullptr se não encontrado
     */
    const PlayerQuestStatus* GetQuestDetails(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Obtém o template de uma quest
     * @param questID ID da quest
     * @return Template da quest, ou nullptr se não encontrado
     */
    const QuestTemplate* GetQuestTemplate(QuestID questID);
    
    /**
     * @brief Obtém todos os templates de quest
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetAllQuestTemplates();
    
    /**
     * @brief Adiciona um template de quest
     * @param tmpl Template de quest
     * @return true se adicionado com sucesso
     */
    bool AddQuestTemplate(const QuestTemplate& tmpl);
    
    /**
     * @brief Remove um template de quest
     * @param questID ID da quest
     * @return true se removido com sucesso
     */
    bool RemoveQuestTemplate(QuestID questID);
    
    /**
     * @brief Obtém quests disponíveis de um NPC
     * @param npcID ID do NPC
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetQuestsFromNPC(WORD npcID);
    
    /**
     * @brief Obtém quests disponíveis em um mapa
     * @param mapID ID do mapa
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetQuestsInMap(WORD mapID);
    
    /**
     * @brief Obtém quests de uma cadeia
     * @param chainID ID da cadeia
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetQuestsInChain(WORD chainID);
    
    /**
     * @brief Obtém quests de um tipo
     * @param type Tipo de quest
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetQuestsByType(QuestType type);
    
    /**
     * @brief Obtém quests de uma categoria
     * @param category Categoria de quest
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetQuestsByCategory(QuestCategory category);
    
    /**
     * @brief Obtém quests por nível
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetQuestsByLevel(BYTE minLevel, BYTE maxLevel);
    
    /**
     * @brief Adiciona uma cadeia de quests
     * @param chain Cadeia de quests
     * @return true se adicionada com sucesso
     */
    bool AddQuestChain(const QuestChain& chain);
    
    /**
     * @brief Remove uma cadeia de quests
     * @param chainID ID da cadeia
     * @return true se removida com sucesso
     */
    bool RemoveQuestChain(WORD chainID);
    
    /**
     * @brief Obtém uma cadeia de quests
     * @param chainID ID da cadeia
     * @return Cadeia de quests, ou nullptr se não encontrada
     */
    const QuestChain* GetQuestChain(WORD chainID);
    
    /**
     * @brief Obtém todas as cadeias de quests
     * @return Lista de cadeias
     */
    std::vector<const QuestChain*> GetAllQuestChains();
    
    /**
     * @brief Adiciona um gatilho de quest
     * @param trigger Gatilho de quest
     * @return true se adicionado com sucesso
     */
    bool AddQuestTrigger(const QuestTrigger& trigger);
    
    /**
     * @brief Remove um gatilho de quest
     * @param triggerID ID do gatilho
     * @return true se removido com sucesso
     */
    bool RemoveQuestTrigger(WORD triggerID);
    
    /**
     * @brief Obtém um gatilho de quest
     * @param triggerID ID do gatilho
     * @return Gatilho de quest, ou nullptr se não encontrado
     */
    const QuestTrigger* GetQuestTrigger(WORD triggerID);
    
    /**
     * @brief Obtém gatilhos por tipo
     * @param type Tipo de gatilho
     * @return Lista de gatilhos
     */
    std::vector<const QuestTrigger*> GetTriggersByType(QuestTriggerType type);
    
    /**
     * @brief Ativa um gatilho de quest
     * @param triggerType Tipo de gatilho
     * @param player Jogador
     * @param sourceID ID da fonte
     * @param targetID ID do alvo
     * @return Número de gatilhos ativados
     */
    int ActivateTriggers(QuestTriggerType triggerType, player::CPlayer* player, 
                        DWORD sourceID, DWORD targetID);
    
    /**
     * @brief Registra um callback para eventos de quest
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterEventCallback(QuestEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterEventCallback(int callbackID);
    
    /**
     * @brief Enfileira um evento de quest
     * @param event Evento de quest
     */
    void EnqueueQuestEvent(const QuestEvent& event);
    
    /**
     * @brief Define multiplicador de ganho de progresso
     * @param multiplier Multiplicador
     */
    void SetQuestProgressMultiplier(float multiplier);
    
    /**
     * @brief Obtém multiplicador de ganho de progresso
     * @return Multiplicador
     */
    float GetQuestProgressMultiplier() const;
    
    /**
     * @brief Define multiplicador de ganho de recompensas
     * @param multiplier Multiplicador
     */
    void SetQuestRewardMultiplier(float multiplier);
    
    /**
     * @brief Obtém multiplicador de ganho de recompensas
     * @return Multiplicador
     */
    float GetQuestRewardMultiplier() const;
    
    /**
     * @brief Define se o jogador pode ter múltiplas quests diárias
     * @param allow Se permite
     */
    void SetAllowMultipleDailyQuests(bool allow);
    
    /**
     * @brief Verifica se o jogador pode ter múltiplas quests diárias
     * @return true se permitido
     */
    bool AllowMultipleDailyQuests() const;
    
    /**
     * @brief Define se o jogador pode abandonar quests sem penalidade
     * @param allow Se permite
     */
    void SetAllowAbandonQuestWithoutPenalty(bool allow);
    
    /**
     * @brief Verifica se o jogador pode abandonar quests sem penalidade
     * @return true se permitido
     */
    bool AllowAbandonQuestWithoutPenalty() const;
    
    /**
     * @brief Define o limite máximo de quests ativas
     * @param limit Limite
     */
    void SetMaxActiveQuestsLimit(int limit);
    
    /**
     * @brief Obtém o limite máximo de quests ativas
     * @return Limite
     */
    int GetMaxActiveQuestsLimit() const;
    
    /**
     * @brief Adiciona progresso para quests de grupo
     * @param player Jogador
     * @param objectiveType Tipo de objetivo
     * @param sourceID ID da fonte
     * @param count Contagem
     * @return Número de objetivos avançados
     */
    int AddProgressForPartyQuests(player::CPlayer* player, QuestObjectiveType objectiveType, 
                                 DWORD sourceID, DWORD count);
    
    /**
     * @brief Reseta quests diárias
     * @param playerID ID do jogador (0 para todos)
     * @return Número de quests resetadas
     */
    int ResetDailyQuests(DWORD playerID = 0);
    
    /**
     * @brief Reseta quests semanais
     * @param playerID ID do jogador (0 para todos)
     * @return Número de quests resetadas
     */
    int ResetWeeklyQuests(DWORD playerID = 0);
    
    /**
     * @brief Checa se um jogador está próximo a objetivos
     * @param player Jogador
     * @param mapID ID do mapa
     * @param posX Posição X
     * @param posY Posição Y
     * @return Lista de IDs de objetivos próximos
     */
    std::vector<std::pair<QuestID, WORD>> CheckNearbyObjectives(player::CPlayer* player, 
                                                               WORD mapID, WORD posX, WORD posY);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    QuestEngine();
    
    /**
     * @brief Destrutor
     */
    ~QuestEngine();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    QuestEngine(const QuestEngine&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    QuestEngine& operator=(const QuestEngine&) = delete;
    
    /**
     * @brief Inicializa os gerenciadores
     * @return true se inicializado com sucesso
     */
    bool InitializeManagers();
    
    /**
     * @brief Processa eventos de quest
     */
    void ProcessQuestEvents();
    
    /**
     * @brief Thread de processamento de eventos
     */
    void EventProcessingThread();
    
    /**
     * @brief Notifica callbacks de evento
     * @param event Evento
     */
    void NotifyEventCallbacks(const QuestEvent& event);
    
    /**
     * @brief Verifica pré-requisitos de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se todos os pré-requisitos são atendidos
     */
    bool CheckQuestPrerequisites(player::CPlayer* player, QuestID questID, std::string& errorReason);
    
    /**
     * @brief Verifica se todos os objetivos de uma quest estão completos
     * @param player Jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se todos os objetivos estão completos
     */
    bool AreAllObjectivesComplete(player::CPlayer* player, QuestID questID, std::string& errorReason);
    
    /**
     * @brief Inicializa o progresso de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return true se inicializado com sucesso
     */
    bool InitializeQuestProgress(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Atualiza o status de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param newStatus Novo status
     * @return true se atualizado com sucesso
     */
    bool UpdateQuestStatus(player::CPlayer* player, QuestID questID, QuestStatus newStatus);
    
    /**
     * @brief Atualiza a fase de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param newPhase Nova fase
     * @return true se atualizado com sucesso
     */
    bool UpdateQuestPhase(player::CPlayer* player, QuestID questID, QuestPhase newPhase);
    
    /**
     * @brief Entrega recompensas de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param chosenRewards Lista de IDs de recompensas escolhidas
     * @param errorReason Razão do erro (saída)
     * @return true se entregue com sucesso
     */
    bool DeliverQuestRewards(player::CPlayer* player, QuestID questID, 
                            const std::vector<WORD>& chosenRewards, std::string& errorReason);
    
    /**
     * @brief Executa um script de quest
     * @param player Jogador
     * @param questID ID da quest
     * @param scriptType Tipo de script
     * @param scriptData Dados do script
     * @return true se executado com sucesso
     */
    bool ExecuteQuestScript(player::CPlayer* player, QuestID questID, 
                           QuestScriptType scriptType, const std::string& scriptData);
    
    /**
     * @brief Verifica se um jogador pode ter mais quests
     * @param player Jogador
     * @param questType Tipo de quest
     * @return true se puder ter mais quests
     */
    bool CanPlayerHaveMoreQuests(player::CPlayer* player, QuestType questType);
    
    /**
     * @brief Verifica se uma quest está disponível para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return true se disponível
     */
    bool IsQuestAvailableForPlayer(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Obtém o gerenciador de templates
     * @return Gerenciador de templates
     */
    QuestTemplateManager* GetTemplateManager();
    
    /**
     * @brief Obtém o gerenciador de progresso
     * @return Gerenciador de progresso
     */
    QuestProgressManager* GetProgressManager();
    
    /**
     * @brief Obtém o gerenciador de recompensas
     * @return Gerenciador de recompensas
     */
    QuestRewardManager* GetRewardManager();
    
    /**
     * @brief Obtém o gerenciador de scripts
     * @return Gerenciador de scripts
     */
    QuestScriptManager* GetScriptManager();
    
    /**
     * @brief Obtém o gerenciador de gatilhos
     * @return Gerenciador de gatilhos
     */
    QuestTriggerManager* GetTriggerManager();
    
    /**
     * @brief Obtém o gerenciador de validação
     * @return Gerenciador de validação
     */
    QuestValidatorManager* GetValidatorManager();
    
    // Gerenciadores
    std::unique_ptr<QuestTemplateManager> templateManager_;
    std::unique_ptr<QuestProgressManager> progressManager_;
    std::unique_ptr<QuestRewardManager> rewardManager_;
    std::unique_ptr<QuestScriptManager> scriptManager_;
    std::unique_ptr<QuestTriggerManager> triggerManager_;
    std::unique_ptr<QuestValidatorManager> validatorManager_;
    
    // Cadeias de quests
    std::map<WORD, QuestChain> questChains_;
    std::mutex chainsMutex_;
    
    // Callbacks de evento
    std::map<int, QuestEventCallback> eventCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Fila de eventos
    std::queue<QuestEvent> eventQueue_;
    std::mutex eventQueueMutex_;
    std::condition_variable eventQueueCV_;
    
    // Configurações
    float progressMultiplier_;
    float rewardMultiplier_;
    bool allowMultipleDailyQuests_;
    bool allowAbandonQuestWithoutPenalty_;
    int maxActiveQuestsLimit_;
    
    // Estado
    bool initialized_;
    std::atomic<bool> running_;
    std::thread eventThread_;
    
    // Cache de quests por NPC
    std::map<WORD, std::vector<QuestID>> questsByNPC_;
    
    // Cache de quests por mapa
    std::map<WORD, std::vector<QuestID>> questsByMap_;
    
    // Cache de quests por tipo
    std::map<QuestType, std::vector<QuestID>> questsByType_;
    
    // Cache de quests por categoria
    std::map<QuestCategory, std::vector<QuestID>> questsByCategory_;
};

// Acesso global
#define g_QuestEngine QuestEngine::GetInstance()

} // namespace quest
} // namespace server
} // namespace wyd

#endif // QUESTENGINE_H

} // namespace wydbr
