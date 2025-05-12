/**
 * QuestProgressManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/progress/QuestProgressManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUESTPROGRESSMANAGER_H
#define QUESTPROGRESSMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <memory>
#include <functional>

#include "../../../../core/WYDTypes.h"
#include "../QuestTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file QuestProgressManager.h
 * @brief Gerenciador de progresso de quests
 * 
 * Este arquivo contém a definição do gerenciador de progresso de quests,
 * responsável por rastrear, atualizar e gerenciar o progresso de quests para todos os jogadores.
 */

namespace wyd {
namespace server {
namespace quest {

// Forward declarations
class QuestTemplateManager;

/**
 * @brief Gerenciador de progresso de quests
 * 
 * Esta classe gerencia o progresso de quests para todos os jogadores,
 * incluindo rastreamento, atualização e persistência.
 */
/**
 * Classe WYDQuestProgressManager
 * 
 * Esta classe implementa a funcionalidade WYDQuestProgressManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestProgressManager {
public:
    /**
     * @brief Construtor
     * @param templateManager Gerenciador de templates
     */
    QuestProgressManager(QuestTemplateManager* templateManager);
    
    /**
     * @brief Destrutor
     */
    ~QuestProgressManager();
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Carrega progresso de quests de todos os jogadores
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadAllProgress(const std::string& filePath);
    
    /**
     * @brief Salva progresso de quests de todos os jogadores
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveAllProgress(const std::string& filePath);
    
    /**
     * @brief Carrega progresso de quests de um jogador
     * @param player Jogador
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadPlayerProgress(player::CPlayer* player, const std::string& filePath);
    
    /**
     * @brief Salva progresso de quests de um jogador
     * @param player Jogador
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SavePlayerProgress(player::CPlayer* player, const std::string& filePath);
    
    /**
     * @brief Inicializa progresso de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return true se inicializado com sucesso
     */
    bool InitializeQuestProgress(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Remove progresso de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return true se removido com sucesso
     */
    bool RemoveQuestProgress(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Obtém o progresso de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return Status da quest, ou nullptr se não encontrado
     */
    const PlayerQuestStatus* GetQuestStatus(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Obtém o progresso de uma quest para um jogador (para edição)
     * @param player Jogador
     * @param questID ID da quest
     * @return Status da quest, ou nullptr se não encontrado
     */
    PlayerQuestStatus* GetQuestStatusForEdit(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Obtém todas as quests ativas de um jogador
     * @param player Jogador
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetActiveQuests(player::CPlayer* player) const;
    
    /**
     * @brief Obtém todas as quests completadas de um jogador
     * @param player Jogador
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetCompletedQuests(player::CPlayer* player) const;
    
    /**
     * @brief Obtém todas as quests falhadas de um jogador
     * @param player Jogador
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetFailedQuests(player::CPlayer* player) const;
    
    /**
     * @brief Obtém todas as quests abandonadas de um jogador
     * @param player Jogador
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetAbandonedQuests(player::CPlayer* player) const;
    
    /**
     * @brief Obtém todas as quests rastreadas de um jogador
     * @param player Jogador
     * @return Lista de IDs de quests
     */
    std::vector<QuestID> GetTrackedQuests(player::CPlayer* player) const;
    
    /**
     * @brief Atualiza o status de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @param status Novo status
     * @return true se atualizado com sucesso
     */
    bool UpdateQuestStatus(player::CPlayer* player, QuestID questID, QuestStatus status);
    
    /**
     * @brief Atualiza a fase de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @param phase Nova fase
     * @return true se atualizado com sucesso
     */
    bool UpdateQuestPhase(player::CPlayer* player, QuestID questID, QuestPhase phase);
    
    /**
     * @brief Atualiza o estado de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @param stateID Novo estado
     * @return true se atualizado com sucesso
     */
    bool UpdateQuestState(player::CPlayer* player, QuestID questID, WORD stateID);
    
    /**
     * @brief Avança o progresso de um objetivo de quest
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @param count Contagem a avançar
     * @return true se avançado com sucesso
     */
    bool AdvanceObjectiveProgress(player::CPlayer* player, QuestID questID, WORD objectiveID, DWORD count);
    
    /**
     * @brief Define o progresso de um objetivo de quest
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @param count Nova contagem
     * @return true se definido com sucesso
     */
    bool SetObjectiveProgress(player::CPlayer* player, QuestID questID, WORD objectiveID, DWORD count);
    
    /**
     * @brief Verifica se todos os objetivos de uma quest estão completos
     * @param player Jogador
     * @param questID ID da quest
     * @return true se todos os objetivos estão completos
     */
    bool AreAllObjectivesComplete(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Marca um objetivo como completo
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @return true se marcado com sucesso
     */
    bool CompleteObjective(player::CPlayer* player, QuestID questID, WORD objectiveID);
    
    /**
     * @brief Verifica se um objetivo está completo
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @return true se o objetivo está completo
     */
    bool IsObjectiveComplete(player::CPlayer* player, QuestID questID, WORD objectiveID) const;
    
    /**
     * @brief Obtém o progresso de um objetivo
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @return Progresso atual, ou 0 se não encontrado
     */
    DWORD GetObjectiveProgress(player::CPlayer* player, QuestID questID, WORD objectiveID) const;
    
    /**
     * @brief Define o estado de um objetivo
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @param state Novo estado
     * @return true se definido com sucesso
     */
    bool SetObjectiveState(player::CPlayer* player, QuestID questID, WORD objectiveID, QuestObjectiveState state);
    
    /**
     * @brief Obtém o estado de um objetivo
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @return Estado do objetivo
     */
    QuestObjectiveState GetObjectiveState(player::CPlayer* player, QuestID questID, WORD objectiveID) const;
    
    /**
     * @brief Marca uma quest como rastreada
     * @param player Jogador
     * @param questID ID da quest
     * @param tracked Se deve ser rastreada
     * @return true se definido com sucesso
     */
    bool SetQuestTracked(player::CPlayer* player, QuestID questID, bool tracked);
    
    /**
     * @brief Verifica se uma quest está rastreada
     * @param player Jogador
     * @param questID ID da quest
     * @return true se a quest está rastreada
     */
    bool IsQuestTracked(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Marca um objetivo como rastreado
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @param tracked Se deve ser rastreado
     * @return true se definido com sucesso
     */
    bool SetObjectiveTracked(player::CPlayer* player, QuestID questID, WORD objectiveID, bool tracked);
    
    /**
     * @brief Verifica se um objetivo está rastreado
     * @param player Jogador
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @return true se o objetivo está rastreado
     */
    bool IsObjectiveTracked(player::CPlayer* player, QuestID questID, WORD objectiveID) const;
    
    /**
     * @brief Define o tempo de expiração de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param expiryTime Tempo de expiração
     * @return true se definido com sucesso
     */
    bool SetQuestExpiryTime(player::CPlayer* player, QuestID questID, DWORD expiryTime);
    
    /**
     * @brief Obtém o tempo de expiração de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @return Tempo de expiração, ou 0 se não existir
     */
    DWORD GetQuestExpiryTime(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Verifica se uma quest expirou
     * @param player Jogador
     * @param questID ID da quest
     * @param currentTime Tempo atual
     * @return true se a quest expirou
     */
    bool HasQuestExpired(player::CPlayer* player, QuestID questID, DWORD currentTime) const;
    
    /**
     * @brief Registra uma recompensa como recebida
     * @param player Jogador
     * @param questID ID da quest
     * @param rewardID ID da recompensa
     * @return true se registrado com sucesso
     */
    bool MarkRewardReceived(player::CPlayer* player, QuestID questID, WORD rewardID);
    
    /**
     * @brief Verifica se uma recompensa foi recebida
     * @param player Jogador
     * @param questID ID da quest
     * @param rewardID ID da recompensa
     * @return true se a recompensa foi recebida
     */
    bool IsRewardReceived(player::CPlayer* player, QuestID questID, WORD rewardID) const;
    
    /**
     * @brief Reseta o progresso de uma quest para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return true se resetado com sucesso
     */
    bool ResetQuestProgress(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Marca a hora de aceitação de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param timestamp Timestamp
     * @return true se definido com sucesso
     */
    bool SetQuestAcceptTime(player::CPlayer* player, QuestID questID, DWORD timestamp);
    
    /**
     * @brief Obtém a hora de aceitação de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @return Timestamp, ou 0 se não existir
     */
    DWORD GetQuestAcceptTime(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Marca a hora de conclusão de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param timestamp Timestamp
     * @return true se definido com sucesso
     */
    bool SetQuestCompleteTime(player::CPlayer* player, QuestID questID, DWORD timestamp);
    
    /**
     * @brief Obtém a hora de conclusão de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @return Timestamp, ou 0 se não existir
     */
    DWORD GetQuestCompleteTime(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Incrementa o contador de quests completadas
     * @param player Jogador
     * @param questID ID da quest
     * @return Novo valor do contador
     */
    BYTE IncrementQuestCompletionCounter(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Obtém o número de vezes que uma quest foi completada
     * @param player Jogador
     * @param questID ID da quest
     * @return Número de vezes completada
     */
    BYTE GetQuestCompletionCount(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Verifica se um jogador já completou uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @return true se o jogador já completou a quest
     */
    bool HasPlayerCompletedQuest(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Verifica se um jogador pode aceitar mais quests
     * @param player Jogador
     * @param questType Tipo de quest
     * @return true se o jogador pode aceitar mais quests
     */
    bool CanPlayerAcceptMoreQuests(player::CPlayer* player, QuestType questType) const;
    
    /**
     * @brief Reseta quests diárias para um jogador
     * @param player Jogador
     * @return Número de quests resetadas
     */
    int ResetDailyQuestsForPlayer(player::CPlayer* player);
    
    /**
     * @brief Reseta quests semanais para um jogador
     * @param player Jogador
     * @return Número de quests resetadas
     */
    int ResetWeeklyQuestsForPlayer(player::CPlayer* player);
    
    /**
     * @brief Limpa o progresso de quests desatualizadas
     * @param player Jogador
     * @return Número de entradas removidas
     */
    int CleanupOutdatedQuestProgress(player::CPlayer* player);
    
    /**
     * @brief Atualiza o multiplicador de progresso
     * @param multiplier Multiplicador
     */
    void SetProgressMultiplier(float multiplier);
    
    /**
     * @brief Obtém o multiplicador de progresso
     * @return Multiplicador
     */
    float GetProgressMultiplier() const;
    
    /**
     * @brief Define o número máximo de quests ativas permitidas
     * @param limit Limite
     */
    void SetMaxActiveQuestsLimit(int limit);
    
    /**
     * @brief Obtém o número máximo de quests ativas permitidas
     * @return Limite
     */
    int GetMaxActiveQuestsLimit() const;
    
    /**
     * @brief Obtém dados de progresso personalizado de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @return Dados personalizados, ou string vazia se não existir
     */
    std::string GetQuestCustomData(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Define dados de progresso personalizado de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param data Dados personalizados
     * @return true se definido com sucesso
     */
    bool SetQuestCustomData(player::CPlayer* player, QuestID questID, const std::string& data);
    
    /**
     * @brief Atualiza o último tempo de atualização de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param timestamp Timestamp
     * @return true se definido com sucesso
     */
    bool UpdateQuestLastUpdateTime(player::CPlayer* player, QuestID questID, DWORD timestamp);
    
    /**
     * @brief Obtém o último tempo de atualização de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @return Timestamp, ou 0 se não existir
     */
    DWORD GetQuestLastUpdateTime(player::CPlayer* player, QuestID questID) const;
    
    /**
     * @brief Registra um callback para alterações de status
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterStatusChangeCallback(std::function<void(player::CPlayer*, QuestID, QuestStatus, QuestStatus)> callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterStatusChangeCallback(int callbackID);
    
private:
    /**
     * @brief Desabilita o construtor de cópia
     */
    QuestProgressManager(const QuestProgressManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    QuestProgressManager& operator=(const QuestProgressManager&) = delete;
    
    /**
     * @brief Carrega progresso de quests de um jogador de JSON
     * @param player Jogador
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadPlayerProgressFromJSON(player::CPlayer* player, const std::string& filePath);
    
    /**
     * @brief Salva progresso de quests de um jogador para JSON
     * @param player Jogador
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SavePlayerProgressToJSON(player::CPlayer* player, const std::string& filePath);
    
    /**
     * @brief Cria um nome de arquivo para o progresso de um jogador
     * @param player Jogador
     * @return Nome do arquivo
     */
    std::string CreatePlayerProgressFilename(player::CPlayer* player) const;
    
    /**
     * @brief Notifica os callbacks de alteração de status
     * @param player Jogador
     * @param questID ID da quest
     * @param oldStatus Status antigo
     * @param newStatus Novo status
     */
    void NotifyStatusChangeCallbacks(player::CPlayer* player, QuestID questID, QuestStatus oldStatus, QuestStatus newStatus);
    
    /**
     * @brief Atualiza os índices internos
     * @param player Jogador
     * @param questID ID da quest
     * @param status Status
     * @param add Se é para adicionar (true) ou remover (false)
     */
    void UpdateInternalIndices(player::CPlayer* player, QuestID questID, QuestStatus status, bool add);
    
    /**
     * @brief Obtém progresso para todos os jogadores
     * @return Mapa de progresso por jogador
     */
    std::map<DWORD, std::map<QuestID, PlayerQuestStatus>> GetAllPlayersProgress() const;
    
    // Gerenciador de templates
    QuestTemplateManager* templateManager_;
    
    // Progresso por jogador
    std::map<DWORD, std::map<QuestID, PlayerQuestStatus>> questProgressByPlayer_;
    mutable std::mutex progressMutex_;
    
    // Índices para busca rápida
    std::map<DWORD, std::set<QuestID>> activeQuestsByPlayer_;
    std::map<DWORD, std::set<QuestID>> completedQuestsByPlayer_;
    std::map<DWORD, std::set<QuestID>> failedQuestsByPlayer_;
    std::map<DWORD, std::set<QuestID>> abandonedQuestsByPlayer_;
    std::map<DWORD, std::set<QuestID>> trackedQuestsByPlayer_;
    
    // Callbacks para alterações de status
    std::map<int, std::function<void(player::CPlayer*, QuestID, QuestStatus, QuestStatus)>> statusChangeCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Configurações
    float progressMultiplier_;
    int maxActiveQuestsLimit_;
    
    // Estado
    bool initialized_;
};

} // namespace quest
} // namespace server
} // namespace wyd

#endif // QUESTPROGRESSMANAGER_H

} // namespace wydbr
