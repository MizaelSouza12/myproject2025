/**
 * @file SkillEffectManager.h
 * @brief Gerenciador de efeitos de habilidades
 * 
 * Responsável por aplicar, controlar e remover efeitos de habilidades
 * como buffs, debuffs, DoTs (Damage over Time), etc.
 */

#ifndef WYDBR_SKILL_EFFECT_MANAGER_H_
#define WYDBR_SKILL_EFFECT_MANAGER_H_

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>

#include "../../common/types/ActorId.h"
#include "../../common/types/SkillEffect.h"
#include "../../common/types/CharacterStats.h"
#include "../../common/utils/TimerManager.h"

namespace wydbr {
namespace tmsrv {
namespace combat {

/**
 * @class SkillEffectManager
 * @brief Gerencia os efeitos de habilidades em entidades
 */
class SkillEffectManager {
public:
    /**
     * @brief Construtor
     * @param timerManager Referência para o gerenciador de timers
     */
    explicit SkillEffectManager(common::utils::TimerManager& timerManager);
    
    /**
     * @brief Destrutor
     */
    ~SkillEffectManager();
    
    /**
     * @brief Aplica um efeito a um alvo
     * 
     * @param targetId ID da entidade alvo
     * @param effect Efeito a ser aplicado
     * @param casterStats Estatísticas do lançador do efeito (para cálculos)
     * @param stackable Se o efeito pode acumular com efeitos similares
     * @return ID único do efeito aplicado ou 0 se falhou
     */
    uint32_t ApplyEffect(
        const common::types::ActorId& targetId,
        const common::types::SkillEffect& effect,
        const common::types::CharacterStats* casterStats = nullptr,
        bool stackable = false);
        
    /**
     * @brief Remove um efeito específico de um alvo
     * 
     * @param targetId ID da entidade alvo
     * @param effectId ID do efeito a ser removido
     * @return true se o efeito foi removido com sucesso
     */
    bool RemoveEffect(
        const common::types::ActorId& targetId,
        uint32_t effectId);
        
    /**
     * @brief Remove todos os efeitos de um tipo específico
     * 
     * @param targetId ID da entidade alvo
     * @param effectType Tipo de efeito a ser removido
     * @return Número de efeitos removidos
     */
    int RemoveEffectsByType(
        const common::types::ActorId& targetId,
        uint8_t effectType);
        
    /**
     * @brief Verifica se um alvo possui um efeito específico
     * 
     * @param targetId ID da entidade alvo
     * @param effectType Tipo de efeito a verificar
     * @return true se o alvo possui o efeito
     */
    bool HasEffect(
        const common::types::ActorId& targetId,
        uint8_t effectType) const;
        
    /**
     * @brief Obtém todos os efeitos ativos em um alvo
     * 
     * @param targetId ID da entidade alvo
     * @return Vector com todos os efeitos ativos
     */
    std::vector<common::types::SkillEffect> GetActiveEffects(
        const common::types::ActorId& targetId) const;
        
    /**
     * @brief Processa ticks de efeitos contínuos (DoT, HoT)
     * 
     * @param targetId ID da entidade alvo
     * @param deltaTime Tempo decorrido desde o último processamento
     */
    void ProcessEffectTicks(
        const common::types::ActorId& targetId,
        std::chrono::milliseconds deltaTime);
        
    /**
     * @brief Limpa todos os efeitos de um alvo (por exemplo, ao morrer)
     * 
     * @param targetId ID da entidade alvo
     * @param removePositive Se deve remover também efeitos positivos
     * @return Número de efeitos removidos
     */
    int ClearEffects(
        const common::types::ActorId& targetId,
        bool removePositive = true);
        
    /**
     * @brief Aplica modificadores de efeitos às estatísticas de um personagem
     * 
     * @param targetId ID da entidade alvo
     * @param baseStats Estatísticas base do personagem
     * @param resultStats Estatísticas resultantes após aplicação dos efeitos
     */
    void ApplyEffectsToStats(
        const common::types::ActorId& targetId,
        const common::types::CharacterStats& baseStats,
        common::types::CharacterStats& resultStats) const;

private:
    struct ActiveEffect {
        uint32_t id;
        common::types::SkillEffect effect;
        std::chrono::steady_clock::time_point endTime;
        std::chrono::steady_clock::time_point nextTickTime;
        const common::types::CharacterStats* casterStats;
    };
    
    using EffectMap = std::unordered_map<common::types::ActorId, std::vector<ActiveEffect>>;
    
    EffectMap activeEffects_;
    mutable std::mutex effectsMutex_;
    common::utils::TimerManager& timerManager_;
    uint32_t nextEffectId_;
    
    /**
     * @brief Programa a remoção automática de um efeito
     * 
     * @param targetId ID da entidade alvo
     * @param effectId ID do efeito
     * @param duration Duração do efeito
     */
    void ScheduleEffectRemoval(
        const common::types::ActorId& targetId,
        uint32_t effectId,
        std::chrono::milliseconds duration);
        
    /**
     * @brief Encontra um efeito ativo pelo ID
     * 
     * @param targetId ID da entidade alvo
     * @param effectId ID do efeito
     * @return Iterador para o efeito ou end() se não encontrado
     */
    std::vector<ActiveEffect>::iterator FindEffect(
        const common::types::ActorId& targetId,
        uint32_t effectId);
};

} // namespace combat
} // namespace tmsrv
} // namespace wydbr

#endif // WYDBR_SKILL_EFFECT_MANAGER_H_ 