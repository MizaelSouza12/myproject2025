/**
 * @file DamageProcessor.cpp
 * @brief Implementação do sistema de processamento de dano
 */

#include "DamageProcessor.h"
#include <algorithm>
#include <random>

namespace wydbr {
namespace tmsrv {
namespace combat {

DamageProcessor::DamageProcessor(
    const CombatFormulas& formulas,
    SkillEffectManager& effectManager)
    : formulas_(formulas),
      effectManager_(effectManager),
      damageDealtCallback_(nullptr),
      damageTakenCallback_(nullptr) {
}

DamageProcessor::~DamageProcessor() {
}

void DamageProcessor::SetCallbacks(
    DamageCallback damageDealtCallback,
    DamageCallback damageTakenCallback) {
    
    damageDealtCallback_ = damageDealtCallback;
    damageTakenCallback_ = damageTakenCallback;
}

common::types::DamageInfo DamageProcessor::ProcessBasicAttack(
    const common::types::ActorId& attackerId,
    const common::types::ActorId& targetId,
    const common::types::CharacterStats& attackerStats,
    const common::types::CharacterStats& targetStats,
    const common::types::ItemAttributes& weaponInfo) {
    
    // Verificar acerto/erro
    float hitChance = formulas_.CalculateHitChance(
        attackerStats.dexterity,
        targetStats.dexterity,
        weaponInfo.precision);
        
    // Gerar número aleatório para determinar acerto
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float hitRoll = dist(gen);
    
    // Se o ataque errou, retornar dano zero
    if (hitRoll > hitChance) {
        common::types::DamageInfo missInfo;
        missInfo.total_damage = 0;
        missInfo.is_miss = true;
        missInfo.damage_type = weaponInfo.damage_type;
        
        // Notificar erro (se callback estiver configurado)
        if (damageDealtCallback_) {
            damageDealtCallback_(attackerId, targetId, missInfo);
        }
        
        if (damageTakenCallback_) {
            damageTakenCallback_(attackerId, targetId, missInfo);
        }
        
        return missInfo;
    }
    
    // Calcular dano base
    int32_t baseDamage = formulas_.CalculatePhysicalDamage(
        attackerStats,
        weaponInfo);
    
    // Verificar crítico
    float critChance = formulas_.CalculateCriticalChance(
        attackerStats.dexterity,
        weaponInfo.critical_chance,
        0.0f); // Sem bônus de habilidade para ataque básico
        
    float critRoll = dist(gen);
    bool isCritical = (critRoll < critChance);
    
    // Se for crítico, aplicar multiplicador
    if (isCritical) {
        float critMultiplier = formulas_.CalculateCriticalMultiplier(
            attackerStats.strength,
            weaponInfo.critical_multiplier,
            0.0f); // Sem bônus de habilidade para ataque básico
            
        baseDamage = static_cast<int32_t>(baseDamage * critMultiplier);
    }
    
    // Finalizar cálculos e notificar
    return FinalizeAndNotifyDamage(
        attackerId,
        targetId,
        baseDamage,
        weaponInfo.damage_type,
        &attackerStats,
        targetStats,
        isCritical,
        weaponInfo.armor_penetration);
}

common::types::DamageInfo DamageProcessor::ProcessSkill(
    const common::types::ActorId& attackerId,
    const common::types::ActorId& targetId,
    const common::types::CharacterStats& attackerStats,
    const common::types::CharacterStats& targetStats,
    const common::types::SkillInfo& skillInfo) {
    
    // Se a habilidade não causa dano, retornar zero
    if (skillInfo.attributes.power_base <= 0) {
        common::types::DamageInfo noDamageInfo;
        noDamageInfo.total_damage = 0;
        noDamageInfo.damage_type = skillInfo.attributes.element_type;
        noDamageInfo.skill_id = skillInfo.skill_id;
        return noDamageInfo;
    }
    
    // Verificar acerto (algumas habilidades sempre acertam)
    bool isHit = true;
    if (!skillInfo.always_hit) {
        float hitChance = formulas_.CalculateHitChance(
            attackerStats.dexterity,
            targetStats.dexterity,
            skillInfo.accuracy_modifier);
            
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float hitRoll = dist(gen);
        
        isHit = (hitRoll <= hitChance);
    }
    
    // Se o ataque errou, retornar dano zero
    if (!isHit) {
        common::types::DamageInfo missInfo;
        missInfo.total_damage = 0;
        missInfo.is_miss = true;
        missInfo.damage_type = skillInfo.attributes.element_type;
        missInfo.skill_id = skillInfo.skill_id;
        
        // Notificar erro
        if (damageDealtCallback_) {
            damageDealtCallback_(attackerId, targetId, missInfo);
        }
        
        if (damageTakenCallback_) {
            damageTakenCallback_(attackerId, targetId, missInfo);
        }
        
        return missInfo;
    }
    
    // Calcular dano base
    int32_t baseDamage;
    if (skillInfo.is_magical) {
        baseDamage = formulas_.CalculateMagicalDamage(
            attackerStats,
            skillInfo.attributes);
    } else {
        // Para habilidades físicas, adaptar o cálculo
        common::types::ItemAttributes weaponAttrs;
        weaponAttrs.damage_min = skillInfo.attributes.power_base;
        weaponAttrs.damage_max = skillInfo.attributes.power_base * 1.2f;
        weaponAttrs.damage_type = skillInfo.attributes.element_type;
        
        baseDamage = formulas_.CalculatePhysicalDamage(
            attackerStats,
            weaponAttrs);
            
        // Aplicar escala da habilidade
        baseDamage = static_cast<int32_t>(baseDamage * skillInfo.physical_scaling);
    }
    
    // Verificar crítico (algumas habilidades têm chance própria de crítico)
    bool isCritical = false;
    if (skillInfo.can_critical) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        float critChance = formulas_.CalculateCriticalChance(
            attackerStats.dexterity,
            0.0f, // Sem bônus de arma para habilidades
            skillInfo.critical_chance_modifier);
            
        float critRoll = dist(gen);
        isCritical = (critRoll < critChance);
        
        // Se for crítico, aplicar multiplicador
        if (isCritical) {
            float critMultiplier = formulas_.CalculateCriticalMultiplier(
                skillInfo.is_magical ? attackerStats.intelligence : attackerStats.strength,
                0.0f, // Sem bônus de arma para habilidades
                skillInfo.critical_damage_modifier);
                
            baseDamage = static_cast<int32_t>(baseDamage * critMultiplier);
        }
    }
    
    // Finalizar cálculos e notificar
    common::types::DamageInfo damageInfo = FinalizeAndNotifyDamage(
        attackerId,
        targetId,
        baseDamage,
        skillInfo.attributes.element_type,
        &attackerStats,
        targetStats,
        isCritical,
        skillInfo.resistance_penetration);
        
    // Adicionar ID da habilidade à estrutura de dano
    damageInfo.skill_id = skillInfo.skill_id;
    
    // Aplicar efeitos adicionais da habilidade, se houver
    for (const auto& effect : skillInfo.effects) {
        effectManager_.ApplyEffect(targetId, effect, &attackerStats, false);
    }
    
    return damageInfo;
}

common::types::DamageInfo DamageProcessor::ProcessReflectedDamage(
    const common::types::ActorId& originalAttackerId,
    const common::types::ActorId& reflectorId,
    const common::types::DamageInfo& originalDamage,
    float reflectPercent) {
    
    // Calcular dano refletido
    int32_t reflectedAmount = formulas_.CalculateReflectedDamage(
        originalDamage.total_damage,
        reflectPercent);
    
    // Criar estrutura de dano refletido
    common::types::DamageInfo reflectInfo;
    reflectInfo.total_damage = reflectedAmount;
    reflectInfo.damage_type = originalDamage.damage_type;
    reflectInfo.is_reflected = true;
    reflectInfo.original_damage = originalDamage.total_damage;
    
    // Notificar
    if (damageDealtCallback_) {
        damageDealtCallback_(reflectorId, originalAttackerId, reflectInfo);
    }
    
    if (damageTakenCallback_) {
        damageTakenCallback_(reflectorId, originalAttackerId, reflectInfo);
    }
    
    return reflectInfo;
}

common::types::DamageInfo DamageProcessor::ProcessDamageOverTime(
    const common::types::ActorId& effectSourceId,
    const common::types::ActorId& targetId,
    const common::types::SkillEffect& effectInfo,
    const common::types::CharacterStats* sourceStats) {
    
    // Calcular dano base do tick
    int32_t tickDamage = static_cast<int32_t>(effectInfo.value);
    
    // Se tiver estatísticas da fonte, pode escalar com algum atributo
    if (sourceStats) {
        switch (effectInfo.scaling_stat) {
            case 0: // Força
                tickDamage += static_cast<int32_t>(sourceStats->strength * effectInfo.scaling_factor);
                break;
            case 1: // Vitalidade
                tickDamage += static_cast<int32_t>(sourceStats->vitality * effectInfo.scaling_factor);
                break;
            case 2: // Destreza
                tickDamage += static_cast<int32_t>(sourceStats->dexterity * effectInfo.scaling_factor);
                break;
            case 3: // Inteligência
                tickDamage += static_cast<int32_t>(sourceStats->intelligence * effectInfo.scaling_factor);
                break;
            case 4: // Sabedoria
                tickDamage += static_cast<int32_t>(sourceStats->wisdom * effectInfo.scaling_factor);
                break;
            // Outros casos conforme necessário
        }
    }
    
    // DoT geralmente não causa acerto crítico
    
    // Criar estrutura de dano
    common::types::DamageInfo dotInfo;
    dotInfo.total_damage = tickDamage;
    dotInfo.damage_type = effectInfo.element_type;
    dotInfo.is_dot = true;
    dotInfo.effect_id = effectInfo.effect_id;
    
    // Notificar
    if (damageDealtCallback_) {
        damageDealtCallback_(effectSourceId, targetId, dotInfo);
    }
    
    if (damageTakenCallback_) {
        damageTakenCallback_(effectSourceId, targetId, dotInfo);
    }
    
    return dotInfo;
}

common::types::DamageInfo DamageProcessor::ProcessEnvironmentalDamage(
    const common::types::ActorId& targetId,
    const common::types::CharacterStats& targetStats,
    uint8_t damageType,
    int32_t baseDamage,
    bool ignoreResistance) {
    
    // Criar ID de fonte para dano ambiental (ID especial com tipo de dano)
    common::types::ActorId environmentId;
    environmentId.type = 99; // Tipo especial para ambiente
    environmentId.id = damageType; // ID baseado no tipo de dano
    
    // Criar estrutura de dano
    common::types::DamageInfo envDamageInfo;
    envDamageInfo.total_damage = baseDamage;
    envDamageInfo.damage_type = damageType;
    envDamageInfo.is_environmental = true;
    
    // Aplicar resistências se não for para ignorá-las
    if (!ignoreResistance) {
        // Compilar resistências elementais do alvo
        std::array<float, 8> targetResistances = {0.0f};
        for (size_t i = 0; i < std::min(targetStats.elemental_resistances.size(), targetResistances.size()); ++i) {
            targetResistances[i] = targetStats.elemental_resistances[i];
        }
        
        // Aplicar redução de dano
        int32_t reducedDamage = formulas_.ApplyResistanceReduction(
            baseDamage,
            targetStats.magic_resistance,
            0.0f, // Sem penetração para dano ambiental
            damageType,
            targetResistances);
            
        envDamageInfo.total_damage = reducedDamage;
        envDamageInfo.damage_reduction = baseDamage - reducedDamage;
    }
    
    // Dano ambiental geralmente não causa críticos
    
    // Aplicar variação aleatória ao dano
    envDamageInfo.total_damage = formulas_.ApplyRandomVariation(
        envDamageInfo.total_damage,
        0.05f); // Variação de 5%
    
    // Notificar
    if (damageTakenCallback_) {
        damageTakenCallback_(environmentId, targetId, envDamageInfo);
    }
    
    return envDamageInfo;
}

common::types::DamageInfo DamageProcessor::FinalizeAndNotifyDamage(
    const common::types::ActorId& sourceId,
    const common::types::ActorId& targetId,
    int32_t baseDamage,
    uint8_t damageType,
    const common::types::CharacterStats* attackerStats,
    const common::types::CharacterStats& targetStats,
    bool isCritical,
    float penetration) {
    
    common::types::DamageInfo damageInfo;
    damageInfo.base_damage = baseDamage;
    damageInfo.damage_type = damageType;
    damageInfo.is_critical = isCritical;
    
    // Aplicar redução de dano com base no tipo
    if (damageType == 0) { // Físico
        damageInfo.total_damage = formulas_.ApplyDefenseReduction(
            baseDamage,
            targetStats.defense,
            penetration);
    } else { // Mágico/Elemental
        // Compilar resistências elementais do alvo
        std::array<float, 8> targetResistances = {0.0f};
        for (size_t i = 0; i < std::min(targetStats.elemental_resistances.size(), targetResistances.size()); ++i) {
            targetResistances[i] = targetStats.elemental_resistances[i];
        }
        
        damageInfo.total_damage = formulas_.ApplyResistanceReduction(
            baseDamage,
            targetStats.magic_resistance,
            penetration,
            damageType,
            targetResistances);
    }
    
    // Calcular redução de dano
    damageInfo.damage_reduction = baseDamage - damageInfo.total_damage;
    
    // Aplicar variação aleatória ao dano final
    damageInfo.total_damage = formulas_.ApplyRandomVariation(
        damageInfo.total_damage);
    
    // Notificar sobre o dano
    if (damageDealtCallback_) {
        damageDealtCallback_(sourceId, targetId, damageInfo);
    }
    
    if (damageTakenCallback_) {
        damageTakenCallback_(sourceId, targetId, damageInfo);
    }
    
    return damageInfo;
}

} // namespace combat
} // namespace tmsrv
} // namespace wydbr 