/**
 * QuestRewardManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/rewards/QuestRewardManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUESTREWARDMANAGER_H
#define QUESTREWARDMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>

#include "../../../../core/WYDTypes.h"
#include "../QuestTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file QuestRewardManager.h
 * @brief Gerenciador de recompensas de quests
 * 
 * Este arquivo contém a definição do gerenciador de recompensas de quests,
 * responsável por gerenciar, processar e distribuir recompensas de quests.
 */

namespace wyd {
namespace server {
namespace quest {

// Forward declarations
class QuestTemplateManager;
class QuestProgressManager;

/**
 * @brief Gerenciador de recompensas de quests
 * 
 * Esta classe gerencia as recompensas de quests, incluindo validação,
 * processamento e distribuição.
 */
/**
 * Classe WYDQuestRewardManager
 * 
 * Esta classe implementa a funcionalidade WYDQuestRewardManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestRewardManager {
public:
    /**
     * @brief Construtor
     * @param templateManager Gerenciador de templates
     * @param progressManager Gerenciador de progresso
     */
    QuestRewardManager(QuestTemplateManager* templateManager, QuestProgressManager* progressManager);
    
    /**
     * @brief Destrutor
     */
    ~QuestRewardManager();
    
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
     * @brief Distribui recompensas de uma quest
     * @param player Jogador
     * @param questID ID da quest
     * @param chosenRewards Lista de IDs de recompensas escolhidas
     * @param errorReason Razão do erro (saída)
     * @return true se distribuído com sucesso
     */
    bool DistributeRewards(player::CPlayer* player, QuestID questID, 
                          const std::vector<WORD>& chosenRewards, std::string& errorReason);
    
    /**
     * @brief Distribui uma recompensa específica
     * @param player Jogador
     * @param questID ID da quest
     * @param rewardID ID da recompensa
     * @param errorReason Razão do erro (saída)
     * @return true se distribuído com sucesso
     */
    bool DistributeReward(player::CPlayer* player, QuestID questID, WORD rewardID, std::string& errorReason);
    
    /**
     * @brief Verifica se um jogador pode receber recompensas
     * @param player Jogador
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se o jogador pode receber recompensas
     */
    bool CanReceiveRewards(player::CPlayer* player, QuestID questID, std::string& errorReason);
    
    /**
     * @brief Verifica se uma escolha de recompensas é válida
     * @param player Jogador
     * @param questID ID da quest
     * @param chosenRewards Lista de IDs de recompensas escolhidas
     * @param errorReason Razão do erro (saída)
     * @return true se a escolha é válida
     */
    bool ValidateRewardChoices(player::CPlayer* player, QuestID questID, 
                              const std::vector<WORD>& chosenRewards, std::string& errorReason);
    
    /**
     * @brief Obtém as recompensas possíveis para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return Lista de recompensas possíveis
     */
    std::vector<const QuestRewardDefinition*> GetPossibleRewards(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Obtém as recompensas garantidas para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return Lista de recompensas garantidas
     */
    std::vector<const QuestRewardDefinition*> GetGuaranteedRewards(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Obtém as recompensas de escolha para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @return Lista de recompensas de escolha
     */
    std::vector<const QuestRewardDefinition*> GetChoiceRewards(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Verifica se uma recompensa está disponível para um jogador
     * @param player Jogador
     * @param questID ID da quest
     * @param rewardID ID da recompensa
     * @return true se a recompensa está disponível
     */
    bool IsRewardAvailable(player::CPlayer* player, QuestID questID, WORD rewardID);
    
    /**
     * @brief Verifica se um jogador já recebeu uma recompensa
     * @param player Jogador
     * @param questID ID da quest
     * @param rewardID ID da recompensa
     * @return true se o jogador já recebeu a recompensa
     */
    bool HasReceivedReward(player::CPlayer* player, QuestID questID, WORD rewardID);
    
    /**
     * @brief Cria uma recompensa aleatória
     * @param player Jogador
     * @param questID ID da quest
     * @param minValue Valor mínimo
     * @param maxValue Valor máximo
     * @param rewardType Tipo de recompensa
     * @return Definição de recompensa
     */
    QuestRewardDefinition CreateRandomReward(player::CPlayer* player, QuestID questID, 
                                           DWORD minValue, DWORD maxValue, QuestRewardType rewardType);
    
    /**
     * @brief Calcula o valor de uma recompensa com bônus
     * @param player Jogador
     * @param baseValue Valor base
     * @param bonusPercentage Porcentagem de bônus
     * @return Valor com bônus
     */
    DWORD CalculateRewardValueWithBonus(player::CPlayer* player, DWORD baseValue, BYTE bonusPercentage);
    
    /**
     * @brief Registra uma recompensa como distribuída
     * @param player Jogador
     * @param questID ID da quest
     * @param rewardID ID da recompensa
     * @return true se registrado com sucesso
     */
    bool RegisterRewardDistributed(player::CPlayer* player, QuestID questID, WORD rewardID);
    
    /**
     * @brief Calcula a chance efetiva de uma recompensa
     * @param player Jogador
     * @param baseChance Chance base
     * @return Chance efetiva
     */
    float CalculateEffectiveRewardChance(player::CPlayer* player, float baseChance);
    
    /**
     * @brief Determina recompensas aleatórias
     * @param player Jogador
     * @param questID ID da quest
     * @return Lista de IDs de recompensas determinadas
     */
    std::vector<WORD> DetermineRandomRewards(player::CPlayer* player, QuestID questID);
    
    /**
     * @brief Atualiza o multiplicador de recompensas
     * @param multiplier Multiplicador
     */
    void SetRewardMultiplier(float multiplier);
    
    /**
     * @brief Obtém o multiplicador de recompensas
     * @return Multiplicador
     */
    float GetRewardMultiplier() const;
    
    /**
     * @brief Determina o nível de qualidade de um item de recompensa
     * @param player Jogador
     * @param baseQuality Qualidade base
     * @param questID ID da quest
     * @return Nível de qualidade
     */
    BYTE DetermineItemQuality(player::CPlayer* player, BYTE baseQuality, QuestID questID);
    
    /**
     * @brief Gera atributos para um item de recompensa
     * @param player Jogador
     * @param itemID ID do item
     * @param quality Qualidade
     * @param questID ID da quest
     * @return Lista de atributos
     */
    std::vector<std::pair<BYTE, WORD>> GenerateItemAttributes(player::CPlayer* player, 
                                                            WORD itemID, BYTE quality, QuestID questID);
    
    /**
     * @brief Cria uma descrição para uma recompensa
     * @param rewardDef Definição de recompensa
     * @param player Jogador
     * @return Descrição formatada
     */
    std::string CreateRewardDescription(const QuestRewardDefinition& rewardDef, player::CPlayer* player);
    
    /**
     * @brief Registra um callback para entrega de recompensa
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterRewardDeliveryCallback(std::function<void(player::CPlayer*, QuestID, WORD)> callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterRewardDeliveryCallback(int callbackID);
    
private:
    /**
     * @brief Desabilita o construtor de cópia
     */
    QuestRewardManager(const QuestRewardManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    QuestRewardManager& operator=(const QuestRewardManager&) = delete;
    
    /**
     * @brief Processa uma recompensa de item
     * @param player Jogador
     * @param reward Definição de recompensa
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se processado com sucesso
     */
    bool ProcessItemReward(player::CPlayer* player, const QuestRewardDefinition& reward, 
                          QuestID questID, std::string& errorReason);
    
    /**
     * @brief Processa uma recompensa de ouro
     * @param player Jogador
     * @param reward Definição de recompensa
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se processado com sucesso
     */
    bool ProcessGoldReward(player::CPlayer* player, const QuestRewardDefinition& reward, 
                          QuestID questID, std::string& errorReason);
    
    /**
     * @brief Processa uma recompensa de experiência
     * @param player Jogador
     * @param reward Definição de recompensa
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se processado com sucesso
     */
    bool ProcessExperienceReward(player::CPlayer* player, const QuestRewardDefinition& reward, 
                               QuestID questID, std::string& errorReason);
    
    /**
     * @brief Processa uma recompensa de reputação
     * @param player Jogador
     * @param reward Definição de recompensa
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se processado com sucesso
     */
    bool ProcessReputationReward(player::CPlayer* player, const QuestRewardDefinition& reward, 
                               QuestID questID, std::string& errorReason);
    
    /**
     * @brief Processa uma recompensa de skill
     * @param player Jogador
     * @param reward Definição de recompensa
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se processado com sucesso
     */
    bool ProcessSkillReward(player::CPlayer* player, const QuestRewardDefinition& reward, 
                          QuestID questID, std::string& errorReason);
    
    /**
     * @brief Processa uma recompensa de título
     * @param player Jogador
     * @param reward Definição de recompensa
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se processado com sucesso
     */
    bool ProcessTitleReward(player::CPlayer* player, const QuestRewardDefinition& reward, 
                          QuestID questID, std::string& errorReason);
    
    /**
     * @brief Processa uma recompensa de moeda
     * @param player Jogador
     * @param reward Definição de recompensa
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se processado com sucesso
     */
    bool ProcessCurrencyReward(player::CPlayer* player, const QuestRewardDefinition& reward, 
                             QuestID questID, std::string& errorReason);
    
    /**
     * @brief Processa outro tipo de recompensa
     * @param player Jogador
     * @param reward Definição de recompensa
     * @param questID ID da quest
     * @param errorReason Razão do erro (saída)
     * @return true se processado com sucesso
     */
    bool ProcessOtherReward(player::CPlayer* player, const QuestRewardDefinition& reward, 
                          QuestID questID, std::string& errorReason);
    
    /**
     * @brief Notifica os callbacks de entrega de recompensa
     * @param player Jogador
     * @param questID ID da quest
     * @param rewardID ID da recompensa
     */
    void NotifyRewardDeliveryCallbacks(player::CPlayer* player, QuestID questID, WORD rewardID);
    
    /**
     * @brief Verifica restrições de classe para uma recompensa
     * @param player Jogador
     * @param reward Definição de recompensa
     * @return true se as restrições são atendidas
     */
    bool CheckClassRestrictions(player::CPlayer* player, const QuestRewardDefinition& reward);
    
    /**
     * @brief Verifica restrições de nível para uma recompensa
     * @param player Jogador
     * @param reward Definição de recompensa
     * @return true se as restrições são atendidas
     */
    bool CheckLevelRestrictions(player::CPlayer* player, const QuestRewardDefinition& reward);
    
    /**
     * @brief Verifica restrições de reputação para uma recompensa
     * @param player Jogador
     * @param reward Definição de recompensa
     * @return true se as restrições são atendidas
     */
    bool CheckReputationRestrictions(player::CPlayer* player, const QuestRewardDefinition& reward);
    
    // Gerenciadores
    QuestTemplateManager* templateManager_;
    QuestProgressManager* progressManager_;
    
    // Callbacks para entrega de recompensa
    std::map<int, std::function<void(player::CPlayer*, QuestID, WORD)>> rewardDeliveryCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Configurações
    float rewardMultiplier_;
    
    // Estado
    bool initialized_;
};

} // namespace quest
} // namespace server
} // namespace wyd

#endif // QUESTREWARDMANAGER_H

} // namespace wydbr
