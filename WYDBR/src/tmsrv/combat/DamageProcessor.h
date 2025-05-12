/**
 * @file DamageProcessor.h
 * @brief Sistema de processamento de dano
 * 
 * Responsável por calcular e aplicar dano, considerando todas as
 * fórmulas de combate, habilidades, efeitos e resistências.
 */

#ifndef WYDBR_DAMAGE_PROCESSOR_H_
#define WYDBR_DAMAGE_PROCESSOR_H_

#include <memory>
#include <vector>
#include <functional>
#include "CombatFormulas.h"
#include "SkillEffectManager.h"
#include "../../common/types/ActorId.h"
#include "../../common/types/CharacterStats.h"
#include "../../common/types/DamageInfo.h"
#include "../../common/types/SkillInfo.h"

namespace wydbr {
namespace tmsrv {
namespace combat {

// Callback para notificar sobre dano
using DamageCallback = std::function<void(
    const common::types::ActorId& source,
    const common::types::ActorId& target,
    const common::types::DamageInfo& damageInfo)>;

/**
 * @class DamageProcessor
 * @brief Processa cálculos de dano entre entidades
 */
class DamageProcessor {
public:
    /**
     * @brief Construtor
     * @param formulas Referência para o objeto de fórmulas de combate
     * @param effectManager Referência para o gerenciador de efeitos
     */
    DamageProcessor(
        const CombatFormulas& formulas,
        SkillEffectManager& effectManager);
    
    /**
     * @brief Destrutor
     */
    ~DamageProcessor();
    
    /**
     * @brief Configura callbacks para eventos de dano
     * 
     * @param damageDealtCallback Chamado quando o dano é aplicado
     * @param damageTakenCallback Chamado quando o alvo recebe o dano
     */
    void SetCallbacks(
        DamageCallback damageDealtCallback,
        DamageCallback damageTakenCallback);
    
    /**
     * @brief Processa um ataque básico
     * 
     * @param attackerId ID do atacante
     * @param targetId ID do alvo
     * @param attackerStats Estatísticas do atacante
     * @param targetStats Estatísticas do alvo
     * @param weaponInfo Informações da arma
     * @return Estrutura com informações do dano causado
     */
    common::types::DamageInfo ProcessBasicAttack(
        const common::types::ActorId& attackerId,
        const common::types::ActorId& targetId,
        const common::types::CharacterStats& attackerStats,
        const common::types::CharacterStats& targetStats,
        const common::types::ItemAttributes& weaponInfo);
    
    /**
     * @brief Processa uma habilidade
     * 
     * @param attackerId ID do atacante
     * @param targetId ID do alvo
     * @param attackerStats Estatísticas do atacante
     * @param targetStats Estatísticas do alvo
     * @param skillInfo Informações da habilidade
     * @return Estrutura com informações do dano causado
     */
    common::types::DamageInfo ProcessSkill(
        const common::types::ActorId& attackerId,
        const common::types::ActorId& targetId,
        const common::types::CharacterStats& attackerStats,
        const common::types::CharacterStats& targetStats,
        const common::types::SkillInfo& skillInfo);
    
    /**
     * @brief Processa dano de reflecte
     * 
     * @param originalAttackerId ID do atacante original
     * @param reflectorId ID de quem reflete o dano
     * @param originalDamage Informações do dano original
     * @param reflectPercent Porcentagem de reflexo (0.0 a 1.0)
     * @return Estrutura com informações do dano refletido
     */
    common::types::DamageInfo ProcessReflectedDamage(
        const common::types::ActorId& originalAttackerId,
        const common::types::ActorId& reflectorId,
        const common::types::DamageInfo& originalDamage,
        float reflectPercent);
    
    /**
     * @brief Processa dano ao longo do tempo (DoT)
     * 
     * @param effectSourceId ID da fonte do efeito
     * @param targetId ID do alvo
     * @param effectInfo Informações do efeito
     * @param sourceStats Estatísticas da fonte (pode ser null para efeitos ambientais)
     * @return Estrutura com informações do dano causado
     */
    common::types::DamageInfo ProcessDamageOverTime(
        const common::types::ActorId& effectSourceId,
        const common::types::ActorId& targetId,
        const common::types::SkillEffect& effectInfo,
        const common::types::CharacterStats* sourceStats);
    
    /**
     * @brief Processa dano ambiental (armadilhas, lava, etc)
     * 
     * @param targetId ID do alvo
     * @param targetStats Estatísticas do alvo
     * @param damageType Tipo de dano (fogo, físico, etc)
     * @param baseDamage Dano base
     * @param ignoreResistance Se deve ignorar resistências
     * @return Estrutura com informações do dano causado
     */
    common::types::DamageInfo ProcessEnvironmentalDamage(
        const common::types::ActorId& targetId,
        const common::types::CharacterStats& targetStats,
        uint8_t damageType,
        int32_t baseDamage,
        bool ignoreResistance = false);

private:
    const CombatFormulas& formulas_;
    SkillEffectManager& effectManager_;
    DamageCallback damageDealtCallback_;
    DamageCallback damageTakenCallback_;
    
    /**
     * @brief Aplica modificadores finais ao dano
     * 
     * @param sourceId ID da fonte do dano
     * @param targetId ID do alvo
     * @param baseDamage Dano base calculado
     * @param damageType Tipo de dano
     * @param attackerStats Estatísticas do atacante (pode ser null)
     * @param targetStats Estatísticas do alvo
     * @param isCritical Se o ataque foi crítico
     * @param penetration Penetração de resistência
     * @return Estrutura com informações do dano final
     */
    common::types::DamageInfo FinalizeAndNotifyDamage(
        const common::types::ActorId& sourceId,
        const common::types::ActorId& targetId,
        int32_t baseDamage,
        uint8_t damageType,
        const common::types::CharacterStats* attackerStats,
        const common::types::CharacterStats& targetStats,
        bool isCritical,
        float penetration);
};

} // namespace combat
} // namespace tmsrv
} // namespace wydbr

#endif // WYDBR_DAMAGE_PROCESSOR_H_ 