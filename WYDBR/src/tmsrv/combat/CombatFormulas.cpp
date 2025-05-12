/**
 * @file CombatFormulas.cpp
 * @brief Implementação das fórmulas matemáticas do sistema de combate
 */

#include "CombatFormulas.h"
#include <algorithm>
#include <chrono>

namespace wydbr {
namespace tmsrv {
namespace combat {

CombatFormulas::CombatFormulas() : 
    rng_(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
    distribution_(0.0f, 1.0f) {
}

CombatFormulas::~CombatFormulas() {
}

int32_t CombatFormulas::CalculatePhysicalDamage(
    const common::types::CharacterStats& attacker,
    const common::types::ItemAttributes& weapon) const {
    
    // Fórmula básica: Força * (1 + Dano da Arma/100) + Bônus de Nível
    float strengthFactor = static_cast<float>(attacker.strength) * 1.5f;
    float weaponDamage = weapon.damage_min + 
        (weapon.damage_max - weapon.damage_min) * distribution_(const_cast<std::mt19937&>(rng_));
    float levelBonus = attacker.level * 0.5f;
    
    // Aplicar bônus de classe
    float classMultiplier = 1.0f;
    switch (attacker.class_type) {
        case 0: // Guerreiro
            classMultiplier = 1.2f;
            break;
        case 1: // Arqueiro
            classMultiplier = 1.1f;
            break;
        // Outros ajustes de classe
        default:
            classMultiplier = 1.0f;
    }
    
    int32_t baseDamage = static_cast<int32_t>(
        (strengthFactor * (1.0f + weaponDamage/100.0f) + levelBonus) * classMultiplier);
    
    // Adicionar bônus de masterização de arma
    if (weapon.weapon_type < 8 && attacker.masteries.size() > weapon.weapon_type) {
        float masteryBonus = attacker.masteries[weapon.weapon_type] * 0.01f;
        baseDamage = static_cast<int32_t>(baseDamage * (1.0f + masteryBonus));
    }
    
    return std::max(1, baseDamage);
}

int32_t CombatFormulas::CalculateMagicalDamage(
    const common::types::CharacterStats& attacker,
    const common::types::SkillAttributes& skill) const {
    
    // Fórmula básica: (Inteligência + Sabedoria/2) * Poder da Habilidade * Bônus de Nível
    float intFactor = static_cast<float>(attacker.intelligence);
    float wisFactor = static_cast<float>(attacker.wisdom) * 0.5f;
    float skillPower = skill.power_base + (skill.power_per_level * skill.level);
    float levelBonus = 1.0f + (attacker.level * 0.01f);
    
    // Aplicar bônus de classe
    float classMultiplier = 1.0f;
    switch (attacker.class_type) {
        case 2: // Mago
            classMultiplier = 1.3f;
            break;
        case 3: // Sacerdote
            classMultiplier = 1.15f;
            break;
        // Outros ajustes de classe
        default:
            classMultiplier = 1.0f;
    }
    
    int32_t baseDamage = static_cast<int32_t>(
        ((intFactor + wisFactor) * skillPower * levelBonus) * classMultiplier);
    
    // Adicionar bônus de especialização de elemento
    if (skill.element_type < 8 && attacker.elemental_masteries.size() > skill.element_type) {
        float elementBonus = attacker.elemental_masteries[skill.element_type] * 0.015f;
        baseDamage = static_cast<int32_t>(baseDamage * (1.0f + elementBonus));
    }
    
    return std::max(1, baseDamage);
}

float CombatFormulas::CalculateHitChance(
    int32_t attackerDex,
    int32_t defenderDex,
    float weaponPrecision) const {
    
    // Base: 70% + fator de destreza + precisão da arma
    float baseHitChance = 0.70f;
    float dexFactor = static_cast<float>(attackerDex - defenderDex) * 0.002f;
    float hitChance = baseHitChance + dexFactor + weaponPrecision;
    
    // Limitar entre 10% e 95%
    return std::clamp(hitChance, 0.10f, 0.95f);
}

float CombatFormulas::CalculateCriticalChance(
    int32_t attackerDex,
    float weaponCritical,
    float skillBonus) const {
    
    // Base: (Destreza/30 + Crítico da arma + Bônus da habilidade)%
    float dexFactor = static_cast<float>(attackerDex) / 30.0f * 0.01f;
    float critChance = dexFactor + weaponCritical + skillBonus;
    
    // Limitar entre 1% e 40%
    return std::clamp(critChance, 0.01f, 0.40f);
}

float CombatFormulas::CalculateCriticalMultiplier(
    int32_t attackerStr,
    float weaponMultiplier,
    float skillMultiplier) const {
    
    // Base: 150% + fator de força + multiplicadores
    float strFactor = std::sqrt(static_cast<float>(attackerStr)) * 0.01f;
    float critMultiplier = 1.5f + strFactor + weaponMultiplier + skillMultiplier;
    
    // Limitar entre 150% e 300%
    return std::clamp(critMultiplier, 1.5f, 3.0f);
}

int32_t CombatFormulas::ApplyDefenseReduction(
    int32_t damage, 
    int32_t defense,
    float armorPenetration) const {
    
    // Aplicar penetração de armadura
    float effectiveDefense = static_cast<float>(defense) * (1.0f - armorPenetration);
    
    // Fórmula de redução: dano * (1 - defesa/(defesa + 100*nível))
    // Usando nível médio de 100 como constante para simplificar
    float reductionFactor = effectiveDefense / (effectiveDefense + 10000.0f);
    reductionFactor = std::clamp(reductionFactor, 0.0f, 0.75f); // Máximo de 75% de redução
    
    int32_t reducedDamage = static_cast<int32_t>(damage * (1.0f - reductionFactor));
    return std::max(1, reducedDamage);
}

int32_t CombatFormulas::ApplyResistanceReduction(
    int32_t damage,
    int32_t resistance,
    float magicPenetration,
    uint8_t elementType,
    const std::array<float, 8>& targetResistances) const {
    
    // Aplicar penetração mágica à resistência base
    float effectiveResistance = static_cast<float>(resistance) * (1.0f - magicPenetration);
    
    // Redução base pela resistência mágica
    float baseReductionFactor = effectiveResistance / (effectiveResistance + 10000.0f);
    baseReductionFactor = std::clamp(baseReductionFactor, 0.0f, 0.70f); // Máximo de 70%
    
    // Adicionar resistência elemental (se aplicável)
    float elementalResistance = 0.0f;
    if (elementType < 8) {
        elementalResistance = targetResistances[elementType];
        elementalResistance = std::clamp(elementalResistance, -0.50f, 0.75f); // Entre -50% e 75%
    }
    
    // Resistência final é a combinação das duas (não pode reduzir mais que 90%)
    float finalReductionFactor = baseReductionFactor + elementalResistance;
    finalReductionFactor = std::clamp(finalReductionFactor, -0.50f, 0.90f);
    
    // Aplicar ao dano
    float multiplier = 1.0f - finalReductionFactor;
    int32_t reducedDamage = static_cast<int32_t>(damage * multiplier);
    return std::max(1, reducedDamage);
}

int32_t CombatFormulas::ApplyRandomVariation(
    int32_t damage,
    float variationPercent) const {
    
    // Gerar variação aleatória entre -variação% e +variação%
    float variation = (distribution_(const_cast<std::mt19937&>(rng_)) * 2.0f - 1.0f) * variationPercent;
    
    int32_t finalDamage = static_cast<int32_t>(damage * (1.0f + variation));
    return std::max(1, finalDamage);
}

int32_t CombatFormulas::CalculateReflectedDamage(
    int32_t damage,
    float reflectPercent) const {
    
    // Simples multiplicação pelo percentual de reflexão
    int32_t reflectedDamage = static_cast<int32_t>(damage * reflectPercent);
    return std::max(0, reflectedDamage); // Reflexão pode ser zero
}

} // namespace combat
} // namespace tmsrv
} // namespace wydbr 