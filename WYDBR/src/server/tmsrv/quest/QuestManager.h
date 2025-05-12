/**
 * QuestManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/QuestManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUESTMANAGER_H
#define QUESTMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <set>
#include <variant>
#include <optional>
#include <queue>
#include <atomic>
#include <thread>
#include <shared_mutex>

#include "../../../core/WYDTypes.h"
#include "./types/QuestTypes.h"
#include "../player/CPlayer.h"
#include "../kernel/persistence/PersistenceManager.h"

namespace wydbr {


/**
 * @file QuestManager.h
 * @brief Sistema avançado de gerenciamento de quests
 * 
 * Este arquivo contém a implementação do sistema de gerenciamento de quests,
 * que suporta quests dinâmicas, objetivos complexos, narrativas ramificadas,
 * sistema de recompensas flexível e integração com outros sistemas de jogo.
 */

namespace wyd {
namespace server {
namespace quest {

/**
 * @brief Configuração do gerenciador de quests
 */
struct QuestManagerConfig {
    bool autosave;                  // Autosave de progresso
    DWORD autosaveInterval;         // Intervalo de autosave (ms)
    bool preloadQuests;             // Pré-carregamento de quests
    bool enableCaching;             // Habilitar cache
    bool asyncLoading;              // Carregamento assíncrono
    bool enableTelemetry;           // Habilitar telemetria
    DWORD maxConcurrentQuests;      // Máximo de quests concorrentes
    DWORD notificationCacheTime;    // Tempo de cache de notificações (ms)
    bool persistProgress;           // Persistir progresso
    std::string questScriptDir;     // Diretório de scripts de quest
    
    QuestManagerConfig()
        : autosave(true)
        , autosaveInterval(60000)
        , preloadQuests(true)
        , enableCaching(true)
        , asyncLoading(true)
        , enableTelemetry(true)
        , maxConcurrentQuests(MAX_ACTIVE_QUESTS)
        , notificationCacheTime(300000)
        , persistProgress(true)
        , questScriptDir("scripts/quests")
    {
    }
};

/**
 * @brief Estatísticas do gerenciador de quests
 */
struct QuestManagerStats {
    DWORD loadedQuests;             // Quests carregadas
    DWORD activeQuestsCount;        // Contagem de quests ativas
    DWORD completedQuestsCount;     // Contagem de quests completadas
    DWORD totalObjectivesCompleted; // Total de objetivos completados
    DWORD totalRewardsGiven;        // Total de recompensas dadas
    DWORD totalQuestStarts;         // Total de inícios de quests
    DWORD totalQuestCompletions;    // Total de conclusões de quests
    float averageCompletionTime;    // Tempo médio de conclusão
    DWORD questChainCompletions;    // Conclusões de cadeias de quests
    DWORD activePlayerCount;        // Contagem de jogadores ativos
    
    QuestManagerStats()
        : loadedQuests(0)
        , activeQuestsCount(0)
        , completedQuestsCount(0)
        , totalObjectivesCompleted(0)
        , totalRewardsGiven(0)
        , totalQuestStarts(0)
        , totalQuestCompletions(0)
        , averageCompletionTime(0.0f)
        , questChainCompletions(0)
        , activePlayerCount(0)
    {
    }
};

/**
 * @brief Resultado de validação de objetivo
 */
struct ObjectiveValidationResult {
    bool success;                   // Sucesso
    DWORD currentCount;             // Contagem atual
    DWORD requiredCount;            // Contagem requerida
    QuestObjectiveType type;        // Tipo
    std::string message;            // Mensagem
    bool objectiveCompleted;        // Objetivo completado
    
    ObjectiveValidationResult()
        : success(false)
        , currentCount(0)
        , requiredCount(0)
        , type(QuestObjectiveType::KILL_MONSTER)
        , message("")
        , objectiveCompleted(false)
    {
    }
};

/**
 * @brief Resultado de entrega de recompensa
 */
struct RewardDeliveryResult {
    bool success;                   // Sucesso
    QuestRewardType type;           // Tipo
    DWORD amount;                   // Quantidade
    DWORD itemID;                   // ID do item
    std::string message;            // Mensagem
    bool delivered;                 // Entregue
    
    RewardDeliveryResult()
        : success(false)
        , type(QuestRewardType::EXPERIENCE)
        , amount(0)
        , itemID(0)
        , message("")
        , delivered(false)
    {
    }
};

/**
 * @brief Evento de atualização de quest
 */
struct QuestUpdateEvent {
    DWORD playerID;                 // ID do jogador
    DWORD questID;                  // ID da quest
    QuestStatus oldStatus;          // Status antigo
    QuestStatus newStatus;          // Status novo
    std::string details;            // Detalhes
    time_t timestamp;               // Timestamp
    
    QuestUpdateEvent()
        : playerID(0)
        , questID(0)
        , oldStatus(QuestStatus::UNAVAILABLE)
        , newStatus(QuestStatus::UNAVAILABLE)
        , details("")
        , timestamp(0)
    {
    }
};

/**
 * @brief Gerenciador de quests
 */
/**
 * Classe WYDQuestManager
 * 
 * Esta classe implementa a funcionalidade WYDQuestManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static QuestManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const QuestManagerConfig& config = QuestManagerConfig());
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Carrega uma quest
     * @param questID ID da quest
     * @return Ponteiro para a quest, ou nullptr se falhar
     */
    const Quest* LoadQuest(DWORD questID);
    
    /**
     * @brief Carrega todas as quests
     * @return true se carregado com sucesso
     */
    bool LoadAllQuests();
    
    /**
     * @brief Obtém uma quest
     * @param questID ID da quest
     * @return Ponteiro para a quest, ou nullptr se não existir
     */
    const Quest* GetQuest(DWORD questID) const;
    
    /**
     * @brief Verifica se um jogador pode iniciar uma quest
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se pode iniciar
     */
    bool CanStartQuest(DWORD playerID, DWORD questID, std::string& errorReason);
    
    /**
     * @brief Inicia uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se iniciado com sucesso
     */
    bool StartQuest(DWORD playerID, DWORD questID, std::string& errorReason);
    
    /**
     * @brief Completa uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se completado com sucesso
     */
    bool CompleteQuest(DWORD playerID, DWORD questID, std::string& errorReason);
    
    /**
     * @brief Falha uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param reason Razão
     * @return true se falhou com sucesso
     */
    bool FailQuest(DWORD playerID, DWORD questID, const std::string& reason);
    
    /**
     * @brief Abandona uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @return true se abandonado com sucesso
     */
    bool AbandonQuest(DWORD playerID, DWORD questID);
    
    /**
     * @brief Verifica se uma quest foi completada por um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @return true se completado
     */
    bool IsQuestCompleted(DWORD playerID, DWORD questID) const;
    
    /**
     * @brief Verifica se uma quest está ativa para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @return true se ativo
     */
    bool IsQuestActive(DWORD playerID, DWORD questID) const;
    
    /**
     * @brief Obtém todas as quests ativas para um jogador
     * @param playerID ID do jogador
     * @return Lista de IDs de quests
     */
    std::vector<DWORD> GetActiveQuests(DWORD playerID) const;
    
    /**
     * @brief Obtém todas as quests completadas por um jogador
     * @param playerID ID do jogador
     * @return Lista de IDs de quests
     */
    std::vector<DWORD> GetCompletedQuests(DWORD playerID) const;
    
    /**
     * @brief Obtém todas as quests disponíveis para um jogador
     * @param playerID ID do jogador
     * @return Lista de IDs de quests
     */
    std::vector<DWORD> GetAvailableQuests(DWORD playerID) const;
    
    /**
     * @brief Obtém todas as quests por categoria
     * @param category Categoria
     * @return Lista de IDs de quests
     */
    std::vector<DWORD> GetQuestsByCategory(QuestCategory category) const;
    
    /**
     * @brief Obtém todas as quests por nível
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @return Lista de IDs de quests
     */
    std::vector<DWORD> GetQuestsByLevel(DWORD minLevel, DWORD maxLevel) const;
    
    /**
     * @brief Atualiza progresso de objetivo para um jogador
     * @param playerID ID do jogador
     * @param objectiveType Tipo de objetivo
     * @param targetID ID do alvo
     * @param count Contagem
     * @param zoneID ID da zona
     * @return true se atualizado com sucesso
     */
    bool UpdateObjectiveProgress(DWORD playerID, QuestObjectiveType objectiveType, 
                               DWORD targetID, DWORD count, DWORD zoneID = 0);
    
    /**
     * @brief Obtém o progresso de uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @return Ponteiro para o progresso, ou nullptr se não existir
     */
    const PlayerQuestProgress* GetQuestProgress(DWORD playerID, DWORD questID) const;
    
    /**
     * @brief Registra um callback para eventos de quest
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterQuestEventCallback(QuestEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterQuestEventCallback(int callbackID);
    
    /**
     * @brief Registra um callback para validação de objetivo
     * @param objectiveType Tipo de objetivo
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterObjectiveValidationCallback(QuestObjectiveType objectiveType, QuestObjectiveValidationCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterObjectiveValidationCallback(int callbackID);
    
    /**
     * @brief Registra um callback para entrega de recompensa
     * @param rewardType Tipo de recompensa
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterRewardDeliveryCallback(QuestRewardType rewardType, QuestRewardDeliveryCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterRewardDeliveryCallback(int callbackID);
    
    /**
     * @brief Registra um callback para verificação de requisito
     * @param requirementType Tipo de requisito
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterRequirementCheckCallback(QuestRequirementType requirementType, QuestRequirementCheckCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterRequirementCheckCallback(int callbackID);
    
    /**
     * @brief Registra um callback para processamento de gatilho
     * @param triggerType Tipo de gatilho
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterTriggerProcessCallback(QuestTriggerType triggerType, QuestTriggerProcessCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterTriggerProcessCallback(int callbackID);
    
    /**
     * @brief Processa um gatilho de quest
     * @param playerID ID do jogador
     * @param triggerType Tipo de gatilho
     * @param targetID ID do alvo
     * @param zoneID ID da zona
     * @param data Dados adicionais
     * @return true se processado com sucesso
     */
    bool ProcessQuestTrigger(DWORD playerID, QuestTriggerType triggerType, 
                          DWORD targetID, DWORD zoneID, const std::string& data);
    
    /**
     * @brief Adiciona uma notificação de quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param message Mensagem
     * @param objectiveType Tipo de objetivo
     * @param objectiveID ID do objetivo
     * @return ID da notificação, ou 0 se falhar
     */
    DWORD AddQuestNotification(DWORD playerID, DWORD questID, const std::string& message, 
                             QuestObjectiveType objectiveType, DWORD objectiveID);
    
    /**
     * @brief Obtém notificações não lidas para um jogador
     * @param playerID ID do jogador
     * @return Lista de notificações
     */
    std::vector<QuestNotification> GetUnreadNotifications(DWORD playerID) const;
    
    /**
     * @brief Marca notificações como lidas para um jogador
     * @param playerID ID do jogador
     * @param notificationIDs Lista de IDs de notificações
     * @return true se marcado com sucesso
     */
    bool MarkNotificationsAsRead(DWORD playerID, const std::vector<DWORD>& notificationIDs);
    
    /**
     * @brief Avança para a próxima etapa de uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @return true se avançado com sucesso
     */
    bool AdvanceQuestStep(DWORD playerID, DWORD questID);
    
    /**
     * @brief Obtém a etapa atual de uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @return Ponteiro para a etapa, ou nullptr se não existir
     */
    const QuestStep* GetCurrentQuestStep(DWORD playerID, DWORD questID) const;
    
    /**
     * @brief Obtém os objetivos de uma etapa de quest
     * @param stepID ID da etapa
     * @return Lista de objetivos
     */
    std::vector<QuestObjective> GetStepObjectives(DWORD stepID) const;
    
    /**
     * @brief Obtém as recompensas de uma quest
     * @param questID ID da quest
     * @return Lista de recompensas
     */
    std::vector<QuestReward> GetQuestRewards(DWORD questID) const;
    
    /**
     * @brief Entrega recompensas de uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param chosenRewardIDs IDs das recompensas escolhidas
     * @return true se entregue com sucesso
     */
    bool DeliverQuestRewards(DWORD playerID, DWORD questID, const std::vector<DWORD>& chosenRewardIDs);
    
    /**
     * @brief Adiciona uma quest a um grupo
     * @param questID ID da quest
     * @param groupID ID do grupo
     * @return true se adicionado com sucesso
     */
    bool AddQuestToGroup(DWORD questID, DWORD groupID);
    
    /**
     * @brief Remove uma quest de um grupo
     * @param questID ID da quest
     * @param groupID ID do grupo
     * @return true se removido com sucesso
     */
    bool RemoveQuestFromGroup(DWORD questID, DWORD groupID);
    
    /**
     * @brief Obtém um grupo de quests
     * @param groupID ID do grupo
     * @return Ponteiro para o grupo, ou nullptr se não existir
     */
    const QuestGroup* GetQuestGroup(DWORD groupID) const;
    
    /**
     * @brief Adiciona uma quest a uma cadeia
     * @param questID ID da quest
     * @param chainID ID da cadeia
     * @param position Posição
     * @return true se adicionado com sucesso
     */
    bool AddQuestToChain(DWORD questID, DWORD chainID, size_t position = 0);
    
    /**
     * @brief Remove uma quest de uma cadeia
     * @param questID ID da quest
     * @param chainID ID da cadeia
     * @return true se removido com sucesso
     */
    bool RemoveQuestFromChain(DWORD questID, DWORD chainID);
    
    /**
     * @brief Obtém uma cadeia de quests
     * @param chainID ID da cadeia
     * @return Ponteiro para a cadeia, ou nullptr se não existir
     */
    const QuestChain* GetQuestChain(DWORD chainID) const;
    
    /**
     * @brief Obtém estatísticas do gerenciador
     * @return Estatísticas
     */
    QuestManagerStats GetStatistics() const;
    
    /**
     * @brief Obtém eventos recentes de quests
     * @param count Número de eventos
     * @param offset Deslocamento
     * @return Lista de eventos
     */
    std::vector<QuestEvent> GetRecentQuestEvents(size_t count, size_t offset = 0) const;
    
    /**
     * @brief Compartilha uma quest com outro jogador
     * @param playerID ID do jogador
     * @param targetPlayerID ID do jogador alvo
     * @param questID ID da quest
     * @return true se compartilhado com sucesso
     */
    bool ShareQuest(DWORD playerID, DWORD targetPlayerID, DWORD questID);
    
    /**
     * @brief Define a visibilidade de uma quest
     * @param questID ID da quest
     * @param visible Se é visível
     * @return true se definido com sucesso
     */
    bool SetQuestVisibility(DWORD questID, bool visible);
    
    /**
     * @brief Redefine o progresso de uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @return true se redefinido com sucesso
     */
    bool ResetQuestProgress(DWORD playerID, DWORD questID);
    
    /**
     * @brief Obtém diálogos de quest para uma etapa
     * @param stepID ID da etapa
     * @return Lista de diálogos
     */
    std::vector<QuestDialogue> GetQuestDialogues(DWORD stepID) const;
    
    /**
     * @brief Verifica triggers para um jogador
     * @param playerID ID do jogador
     * @param zoneID ID da zona
     * @return true se verificado com sucesso
     */
    bool CheckQuestTriggers(DWORD playerID, DWORD zoneID);
    
    /**
     * @brief Salva progresso de quests para um jogador
     * @param playerID ID do jogador
     * @return true se salvo com sucesso
     */
    bool SavePlayerQuestProgress(DWORD playerID);
    
    /**
     * @brief Carrega progresso de quests para um jogador
     * @param playerID ID do jogador
     * @return true se carregado com sucesso
     */
    bool LoadPlayerQuestProgress(DWORD playerID);
    
    /**
     * @brief Verifica requisitos de uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param failedRequirements Lista de requisitos falhados (saída)
     * @return true se todos os requisitos forem atendidos
     */
    bool CheckQuestRequirements(DWORD playerID, DWORD questID, std::vector<QuestRequirement>& failedRequirements);
    
    /**
     * @brief Valida um objetivo para um jogador
     * @param playerID ID do jogador
     * @param objectiveID ID do objetivo
     * @param data Dados adicionais
     * @return Resultado da validação
     */
    ObjectiveValidationResult ValidateObjective(DWORD playerID, DWORD objectiveID, const std::string& data);
    
    /**
     * @brief Entrega uma recompensa para um jogador
     * @param playerID ID do jogador
     * @param rewardID ID da recompensa
     * @return Resultado da entrega
     */
    RewardDeliveryResult DeliverReward(DWORD playerID, DWORD rewardID);
    
    /**
     * @brief Verifica se um requisito é atendido para um jogador
     * @param playerID ID do jogador
     * @param requirementID ID do requisito
     * @return true se o requisito for atendido
     */
    bool CheckRequirement(DWORD playerID, DWORD requirementID);
    
    /**
     * @brief Carrega e ativa quests para um jogador
     * @param playerID ID do jogador
     * @return true se carregado com sucesso
     */
    bool ActivatePlayerQuests(DWORD playerID);
    
    /**
     * @brief Desativa quests para um jogador
     * @param playerID ID do jogador
     * @return true se desativado com sucesso
     */
    bool DeactivatePlayerQuests(DWORD playerID);
    
    /**
     * @brief Limpa cache para um jogador
     * @param playerID ID do jogador
     * @return true se limpo com sucesso
     */
    bool ClearPlayerCache(DWORD playerID);
    
    /**
     * @brief Gera log de evento de quest
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param oldStatus Status antigo
     * @param newStatus Status novo
     * @param details Detalhes
     * @return ID do evento, ou 0 se falhar
     */
    DWORD LogQuestEvent(DWORD playerID, DWORD questID, QuestStatus oldStatus, QuestStatus newStatus, const std::string& details);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    QuestManager();
    
    /**
     * @brief Destrutor
     */
    ~QuestManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    QuestManager(const QuestManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    QuestManager& operator=(const QuestManager&) = delete;
    
    /**
     * @brief Thread de autosave
     */
    void AutosaveThread();
    
    /**
     * @brief Thread de processamento assíncrono
     */
    void AsyncProcessingThread();
    
    /**
     * @brief Notifica callbacks de evento de quest
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param oldStatus Status antigo
     * @param newStatus Status novo
     * @param details Detalhes
     */
    void NotifyQuestEventCallbacks(DWORD playerID, DWORD questID, QuestStatus oldStatus, QuestStatus newStatus, const std::string& details);
    
    /**
     * @brief Carrega uma quest do banco de dados
     * @param questID ID da quest
     * @return Ponteiro para a quest, ou nullptr se falhar
     */
    Quest* LoadQuestFromDB(DWORD questID);
    
    /**
     * @brief Valida um objetivo específico para um jogador
     * @param playerID ID do jogador
     * @param objective Objetivo
     * @param targetID ID do alvo
     * @param count Contagem
     * @return Resultado da validação
     */
    ObjectiveValidationResult ValidateSpecificObjective(DWORD playerID, const QuestObjective& objective, DWORD targetID, DWORD count);
    
    /**
     * @brief Processa gatilhos de quest para um jogador
     * @param playerID ID do jogador
     * @param zoneID ID da zona
     * @param triggerType Tipo de gatilho
     * @param targetID ID do alvo
     * @return true se processado com sucesso
     */
    bool ProcessQuestTriggersForPlayer(DWORD playerID, DWORD zoneID, QuestTriggerType triggerType, DWORD targetID);
    
    /**
     * @brief Adiciona uma tarefa assíncrona à fila
     * @param task Tarefa
     */
    void AddAsyncTask(std::function<void()> task);
    
    /**
     * @brief Atualiza estatísticas
     */
    void UpdateStatistics();
    
    /**
     * @brief Carrega objetivos de uma quest
     * @param questID ID da quest
     * @return Lista de objetivos
     */
    std::vector<QuestObjective> LoadQuestObjectives(DWORD questID);
    
    /**
     * @brief Carrega recompensas de uma quest
     * @param questID ID da quest
     * @return Lista de recompensas
     */
    std::vector<QuestReward> LoadQuestRewards(DWORD questID);
    
    /**
     * @brief Carrega requisitos de uma quest
     * @param questID ID da quest
     * @return Lista de requisitos
     */
    std::vector<QuestRequirement> LoadQuestRequirements(DWORD questID);
    
    /**
     * @brief Carrega etapas de uma quest
     * @param questID ID da quest
     * @return Lista de etapas
     */
    std::vector<QuestStep> LoadQuestSteps(DWORD questID);
    
    /**
     * @brief Carrega diálogos de uma quest
     * @param questID ID da quest
     * @return Lista de diálogos
     */
    std::vector<QuestDialogue> LoadQuestDialogues(DWORD questID);
    
    /**
     * @brief Carrega gatilhos de uma quest
     * @param questID ID da quest
     * @return Lista de gatilhos
     */
    std::vector<QuestTrigger> LoadQuestTriggers(DWORD questID);
    
    /**
     * @brief Busca no cache o progresso de uma quest para um jogador
     * @param playerID ID do jogador
     * @param questID ID da quest
     * @param createIfNotExists Criar se não existir
     * @return Ponteiro para o progresso, ou nullptr se não existir
     */
    PlayerQuestProgress* GetPlayerQuestProgressFromCache(DWORD playerID, DWORD questID, bool createIfNotExists = false);
    
    // Configuração
    QuestManagerConfig config_;
    
    // Quests
    std::unordered_map<DWORD, Quest> quests_;
    mutable std::shared_mutex questsMutex_;
    
    // Objetivos
    std::unordered_map<DWORD, QuestObjective> objectives_;
    mutable std::shared_mutex objectivesMutex_;
    
    // Recompensas
    std::unordered_map<DWORD, QuestReward> rewards_;
    mutable std::shared_mutex rewardsMutex_;
    
    // Requisitos
    std::unordered_map<DWORD, QuestRequirement> requirements_;
    mutable std::shared_mutex requirementsMutex_;
    
    // Etapas
    std::unordered_map<DWORD, QuestStep> steps_;
    mutable std::shared_mutex stepsMutex_;
    
    // Diálogos
    std::unordered_map<DWORD, QuestDialogue> dialogues_;
    mutable std::shared_mutex dialoguesMutex_;
    
    // Gatilhos
    std::unordered_map<DWORD, QuestTrigger> triggers_;
    mutable std::shared_mutex triggersMutex_;
    
    // Progresso dos jogadores
    std::unordered_map<DWORD, std::unordered_map<DWORD, PlayerQuestProgress>> playerProgress_;
    mutable std::shared_mutex playerProgressMutex_;
    
    // Grupos
    std::unordered_map<DWORD, QuestGroup> groups_;
    mutable std::shared_mutex groupsMutex_;
    
    // Cadeias
    std::unordered_map<DWORD, QuestChain> chains_;
    mutable std::shared_mutex chainsMutex_;
    
    // Notificações
    std::unordered_map<DWORD, std::vector<QuestNotification>> notifications_;
    mutable std::shared_mutex notificationsMutex_;
    
    // Eventos
    std::vector<QuestEvent> events_;
    mutable std::shared_mutex eventsMutex_;
    
    // Jogadores ativos
    std::set<DWORD> activePlayers_;
    mutable std::shared_mutex activePlayersMutex_;
    
    // Quests por categoria
    std::unordered_map<QuestCategory, std::vector<DWORD>> questsByCategory_;
    
    // Quests por nível
    std::map<DWORD, std::vector<DWORD>> questsByLevelMin_;
    std::map<DWORD, std::vector<DWORD>> questsByLevelMax_;
    
    // Callbacks
    std::map<int, QuestEventCallback> questEventCallbacks_;
    std::mutex questEventCallbacksMutex_;
    int nextQuestEventCallbackID_;
    
    std::map<int, std::pair<QuestObjectiveType, QuestObjectiveValidationCallback>> objectiveValidationCallbacks_;
    std::mutex objectiveValidationCallbacksMutex_;
    int nextObjectiveValidationCallbackID_;
    
    std::map<int, std::pair<QuestRewardType, QuestRewardDeliveryCallback>> rewardDeliveryCallbacks_;
    std::mutex rewardDeliveryCallbacksMutex_;
    int nextRewardDeliveryCallbackID_;
    
    std::map<int, std::pair<QuestRequirementType, QuestRequirementCheckCallback>> requirementCheckCallbacks_;
    std::mutex requirementCheckCallbacksMutex_;
    int nextRequirementCheckCallbackID_;
    
    std::map<int, std::pair<QuestTriggerType, QuestTriggerProcessCallback>> triggerProcessCallbacks_;
    std::mutex triggerProcessCallbacksMutex_;
    int nextTriggerProcessCallbackID_;
    
    // Tarefas assíncronas
    std::queue<std::function<void()>> asyncTasks_;
    std::mutex asyncTasksMutex_;
    std::condition_variable asyncTasksCV_;
    
    // Threads
    std::thread autosaveThread_;
    std::thread asyncProcessingThread_;
    std::atomic<bool> running_;
    
    // Estatísticas
    QuestManagerStats stats_;
    mutable std::mutex statsMutex_;
    
    // Estado
    bool initialized_;
    time_t lastAutosaveTime_;
    
    // IDs únicos
    std::atomic<DWORD> nextQuestID_;
    std::atomic<DWORD> nextObjectiveID_;
    std::atomic<DWORD> nextRewardID_;
    std::atomic<DWORD> nextRequirementID_;
    std::atomic<DWORD> nextStepID_;
    std::atomic<DWORD> nextDialogueID_;
    std::atomic<DWORD> nextTriggerID_;
    std::atomic<DWORD> nextNotificationID_;
    std::atomic<DWORD> nextGroupID_;
    std::atomic<DWORD> nextChainID_;
    std::atomic<DWORD> nextEventID_;
};

// Acesso global
#define g_QuestManager QuestManager::GetInstance()

} // namespace quest
} // namespace server
} // namespace wyd

#endif // QUESTMANAGER_H

} // namespace wydbr
