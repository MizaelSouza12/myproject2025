/**
 * @file ComboSystem.h
 * @brief Sistema de combos para encadeamento de habilidades
 * 
 * Gerencia combos e encadeamento de habilidades, incluindo bônus
 * por sequências específicas, janelas de tempo e condições especiais.
 */

#ifndef WYDBR_COMBO_SYSTEM_H_
#define WYDBR_COMBO_SYSTEM_H_

#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>

#include "../../common/types/ActorId.h"
#include "../../common/types/SkillId.h"
#include "../../common/types/ComboInfo.h"
#include "../../common/utils/TimerManager.h"

namespace wydbr {
namespace tmsrv {
namespace combat {

/**
 * @class ComboSystem
 * @brief Gerencia os combos de habilidades dos jogadores
 */
class ComboSystem {
public:
    /**
     * @brief Construtor
     * @param timerManager Referência para o gerenciador de timers
     */
    explicit ComboSystem(common::utils::TimerManager& timerManager);
    
    /**
     * @brief Destrutor
     */
    ~ComboSystem();
    
    /**
     * @brief Registra uma definição de combo
     * 
     * @param comboDefinition Definição completa do combo
     * @return ID do combo registrado
     */
    uint32_t RegisterCombo(const common::types::ComboDefinition& comboDefinition);
    
    /**
     * @brief Remove um combo registrado
     * 
     * @param comboId ID do combo a remover
     * @return true se o combo foi removido
     */
    bool UnregisterCombo(uint32_t comboId);
    
    /**
     * @brief Registra uso de habilidade e verifica combos
     * 
     * @param actorId ID do ator que usou a habilidade
     * @param skillId ID da habilidade usada
     * @param targetId ID do alvo da habilidade (opcional)
     * @param timestamp Momento do uso (padrão: agora)
     * @return Informações sobre combos ativados, se houver
     */
    common::types::ComboResult ProcessSkillUse(
        const common::types::ActorId& actorId,
        common::types::SkillId skillId,
        const common::types::ActorId* targetId = nullptr,
        std::chrono::steady_clock::time_point timestamp = std::chrono::steady_clock::now());
    
    /**
     * @brief Verifica o progresso atual do combo para um ator
     * 
     * @param actorId ID do ator
     * @return Informações sobre o combo em andamento
     */
    common::types::ComboInfo GetCurrentCombo(const common::types::ActorId& actorId) const;
    
    /**
     * @brief Reinicia o estado de combo de um ator
     * 
     * @param actorId ID do ator
     */
    void ResetCombo(const common::types::ActorId& actorId);
    
    /**
     * @brief Adiciona bônus de combo a um ator
     * 
     * @param actorId ID do ator
     * @param bonusType Tipo de bônus (dano, velocidade, etc)
     * @param bonusValue Valor do bônus (absoluto ou percentual)
     * @param duration Duração do bônus
     * @return ID do bônus para rastreamento
     */
    uint32_t AddComboBonus(
        const common::types::ActorId& actorId,
        uint8_t bonusType,
        float bonusValue,
        std::chrono::milliseconds duration);
    
    /**
     * @brief Remove um bônus de combo específico
     * 
     * @param actorId ID do ator
     * @param bonusId ID do bônus a remover
     * @return true se o bônus foi removido
     */
    bool RemoveComboBonus(
        const common::types::ActorId& actorId,
        uint32_t bonusId);
    
    /**
     * @brief Obtém todos os bônus ativos para um ator
     * 
     * @param actorId ID do ator
     * @return Vector com todos os bônus ativos
     */
    std::vector<common::types::ComboBonus> GetActiveBonuses(
        const common::types::ActorId& actorId) const;

private:
    struct ActiveCombo {
        common::types::ComboInfo info;
        std::chrono::steady_clock::time_point lastSkillTime;
        std::vector<common::types::SkillId> skillSequence;
        common::types::ActorId lastTarget;
    };
    
    struct ComboBonus {
        uint32_t id;
        uint8_t type;
        float value;
        std::chrono::steady_clock::time_point endTime;
    };
    
    using ActorComboMap = std::unordered_map<common::types::ActorId, ActiveCombo, common::types::ActorIdHash>;
    using ComboBonusMap = std::unordered_map<common::types::ActorId, std::vector<ComboBonus>, common::types::ActorIdHash>;
    
    std::vector<common::types::ComboDefinition> comboDefinitions_;
    ActorComboMap activePlayerCombos_;
    ComboBonusMap actorBonuses_;
    common::utils::TimerManager& timerManager_;
    mutable std::mutex comboMutex_;
    mutable std::mutex bonusMutex_;
    uint32_t nextComboId_;
    uint32_t nextBonusId_;
    
    /**
     * @brief Verifica se uma sequência de habilidades corresponde a um combo
     * 
     * @param skillSequence Sequência de habilidades usadas
     * @param targetId ID do último alvo (opcional)
     * @return O combo correspondente ou nullptr
     */
    const common::types::ComboDefinition* FindMatchingCombo(
        const std::vector<common::types::SkillId>& skillSequence,
        const common::types::ActorId* targetId) const;
    
    /**
     * @brief Aplica os efeitos de um combo completo
     * 
     * @param actorId ID do ator que completou o combo
     * @param combo Definição do combo completo
     * @param targetId ID do último alvo (opcional)
     */
    void ApplyComboEffects(
        const common::types::ActorId& actorId,
        const common::types::ComboDefinition& combo,
        const common::types::ActorId* targetId);
    
    /**
     * @brief Agenda a remoção automática de bônus de combo
     * 
     * @param actorId ID do ator
     * @param bonusId ID do bônus
     * @param duration Duração do bônus
     */
    void ScheduleBonusRemoval(
        const common::types::ActorId& actorId,
        uint32_t bonusId,
        std::chrono::milliseconds duration);
};

} // namespace combat
} // namespace tmsrv
} // namespace wydbr

#endif // WYDBR_COMBO_SYSTEM_H_ 