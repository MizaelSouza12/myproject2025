#include "GameLogic/combat/DamageCalculator.h"
#include "GameLogic/character/CharacterSystem.h"
#include <cmath>

namespace WYDBR {

uint32_t DamageCalculator::CalculateDamage(uint32_t attackerId, uint32_t targetId, AttackType type) {
    // Obter personagens
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    
    if (!attacker || !target) {
        return 0;
    }

    // Fatores base do WYD
    float attackPower = CalculateAttackPower(attackerId);
    float defense = CalculateDefense(targetId);
    float elementalBonus = CalculateElementalBonus(attackerId, targetId);
    
    // Cálculo base de dano (fórmula original do WYD)
    float baseDamage = (attackPower * 1.5f) - (defense * 0.5f);
    
    // Aplicar bônus elemental
    baseDamage *= (1.0f + elementalBonus);
    
    // Aplicar tipo de ataque
    switch (type) {
        case AttackType::NORMAL:
            baseDamage *= 1.0f;
            break;
        case AttackType::SKILL:
            baseDamage *= 1.2f;
            break;
        case AttackType::CRITICAL:
            baseDamage *= 1.5f;
            break;
        case AttackType::SKILL_CRITICAL:
            baseDamage *= 1.8f;
            break;
    }
    
    // Aplicar bônus de nível
    float levelBonus = 1.0f + (attacker->Level * 0.01f);
    baseDamage *= levelBonus;
    
    // Aplicar bônus de equipamento
    float equipmentBonus = 1.0f + (attacker->EquipBonus * 0.01f);
    baseDamage *= equipmentBonus;
    
    // Aplicar bônus de status
    float statusBonus = 1.0f;
    if (attacker->Status & STATUS_BERSERK) {
        statusBonus += 0.3f;
    }
    if (attacker->Status & STATUS_POWER) {
        statusBonus += 0.2f;
    }
    baseDamage *= statusBonus;
    
    // Aplicar redução de dano
    float damageReduction = 1.0f - (target->DamageReduction * 0.01f);
    baseDamage *= damageReduction;
    
    // Garantir dano mínimo
    baseDamage = std::max(baseDamage, 1.0f);
    
    return static_cast<uint32_t>(baseDamage);
}

uint32_t DamageCalculator::CalculateSkillDamage(uint32_t casterId, uint32_t targetId, uint16_t skillId) {
    // Obter personagens
    const Mob* caster = CharacterSystem::GetInstance().GetCharacter(casterId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    
    if (!caster || !target) {
        return 0;
    }
    
    // Fatores base
    float attackPower = CalculateAttackPower(casterId);
    float defense = CalculateDefense(targetId);
    float elementalBonus = CalculateElementalBonus(casterId, targetId);
    
    // Cálculo base de dano da habilidade
    float baseDamage = (attackPower * 2.0f) - (defense * 0.3f);
    
    // Aplicar bônus elemental
    baseDamage *= (1.0f + elementalBonus);
    
    // Aplicar bônus de nível da habilidade
    float skillLevelBonus = 1.0f + (caster->SkillLevel[skillId] * 0.1f);
    baseDamage *= skillLevelBonus;
    
    // Aplicar bônus de status
    float statusBonus = 1.0f;
    if (caster->Status & STATUS_BERSERK) {
        statusBonus += 0.3f;
    }
    if (caster->Status & STATUS_POWER) {
        statusBonus += 0.2f;
    }
    baseDamage *= statusBonus;
    
    // Aplicar redução de dano
    float damageReduction = 1.0f - (target->DamageReduction * 0.01f);
    baseDamage *= damageReduction;
    
    // Garantir dano mínimo
    baseDamage = std::max(baseDamage, 1.0f);
    
    return static_cast<uint32_t>(baseDamage);
}

uint32_t DamageCalculator::CalculateAreaDamage(uint32_t casterId, uint16_t skillId, float distance) {
    // Obter personagem
    const Mob* caster = CharacterSystem::GetInstance().GetCharacter(casterId);
    
    if (!caster) {
        return 0;
    }
    
    // Fatores base
    float attackPower = CalculateAttackPower(casterId);
    
    // Cálculo base de dano em área
    float baseDamage = attackPower * 1.5f;
    
    // Aplicar bônus de nível da habilidade
    float skillLevelBonus = 1.0f + (caster->SkillLevel[skillId] * 0.1f);
    baseDamage *= skillLevelBonus;
    
    // Aplicar penalidade de distância
    float distancePenalty = CalculateDistancePenalty(distance);
    baseDamage *= distancePenalty;
    
    // Aplicar bônus de status
    float statusBonus = 1.0f;
    if (caster->Status & STATUS_BERSERK) {
        statusBonus += 0.3f;
    }
    if (caster->Status & STATUS_POWER) {
        statusBonus += 0.2f;
    }
    baseDamage *= statusBonus;
    
    // Garantir dano mínimo
    baseDamage = std::max(baseDamage, 1.0f);
    
    return static_cast<uint32_t>(baseDamage);
}

float DamageCalculator::CalculateAttackPower(uint32_t attackerId) const {
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    if (!attacker) {
        return 0.0f;
    }
    
    // Fórmula original do WYD com melhorias
    float baseAttack = attacker->Attack;
    float strengthBonus = attacker->Strength * 0.5f;
    float dexterityBonus = attacker->Dexterity * 0.3f;
    float levelBonus = attacker->Level * 0.2f;
    
    return baseAttack + strengthBonus + dexterityBonus + levelBonus;
}

float DamageCalculator::CalculateDefense(uint32_t targetId) const {
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    if (!target) {
        return 0.0f;
    }
    
    // Fórmula original do WYD com melhorias
    float baseDefense = target->Defense;
    float vitalityBonus = target->Vitality * 0.4f;
    float levelBonus = target->Level * 0.15f;
    
    return baseDefense + vitalityBonus + levelBonus;
}

float DamageCalculator::CalculateElementalBonus(uint32_t attackerId, uint32_t targetId) const {
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    
    if (!attacker || !target) {
        return 0.0f;
    }
    
    // Sistema elemental do WYD com melhorias
    float bonus = 0.0f;
    
    // Verificar resistências e fraquezas
    if (attacker->Element == target->Weakness) {
        bonus += 0.3f;
    }
    if (attacker->Element == target->Resistance) {
        bonus -= 0.2f;
    }
    
    return bonus;
}

float DamageCalculator::CalculateCriticalMultiplier(uint32_t attackerId) const {
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    if (!attacker) {
        return 1.0f;
    }
    
    // Fórmula original do WYD com melhorias
    float baseMultiplier = 1.5f;
    float dexterityBonus = attacker->Dexterity * 0.01f;
    float levelBonus = attacker->Level * 0.005f;
    
    return baseMultiplier + dexterityBonus + levelBonus;
}

float DamageCalculator::CalculateDistancePenalty(float distance) const {
    // Penalidade de distância do WYD com melhorias
    if (distance <= 1.0f) {
        return 1.0f;
    }
    
    float penalty = 1.0f - (distance * 0.1f);
    return std::max(penalty, 0.1f);
}

} // namespace WYDBR 