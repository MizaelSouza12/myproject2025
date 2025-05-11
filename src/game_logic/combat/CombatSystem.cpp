#include "GameLogic/combat/CombatSystem.h"
#include "GameLogic/character/CharacterSystem.h"
#include "GameLogic/skill/SkillSystem.h"
#include <random>
#include <cmath>
#include <algorithm>

namespace WYDBR {

CombatSystem& CombatSystem::GetInstance() {
    static CombatSystem instance;
    return instance;
}

void CombatSystem::Initialize() {
    combatStats.clear();
    activeEffects.clear();
}

void CombatSystem::Shutdown() {
    combatStats.clear();
    activeEffects.clear();
}

void CombatSystem::UpdateCombatStats(uint32_t characterId, const CombatStats& stats) {
    combatStats[characterId] = stats;
}

const CombatStats* CombatSystem::GetCombatStats(uint32_t characterId) const {
    auto it = combatStats.find(characterId);
    return it != combatStats.end() ? &it->second : nullptr;
}

CombatResult CombatSystem::ProcessAttack(uint32_t attackerId, uint32_t targetId) {
    CombatResult result = {};
    
    // Verificar se os personagens existem
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    
    if (!attacker || !target) {
        return result;
    }
    
    // Verificar se está no alcance
    if (!IsInRange(attackerId, targetId, 2.0f)) {
        return result;
    }
    
    // Verificar acerto
    if (!CheckHit(attackerId, targetId)) {
        return result;
    }
    
    // Verificar bloqueio
    result.blocked = CheckBlock(targetId);
    
    // Verificar crítico
    result.critical = CheckCritical(attackerId);
    
    // Calcular dano
    result.damage = CalculateDamage(attackerId, targetId, false);
    
    // Aplicar dano
    if (result.damage > 0) {
        CharacterSystem::GetInstance().ApplyDamage(targetId, result.damage);
    }
    
    result.hit = true;
    return result;
}

CombatResult CombatSystem::ProcessSkill(uint32_t attackerId, uint32_t targetId, uint16_t skillId) {
    CombatResult result = {};
    result.skillId = skillId;
    
    // Verificar se os personagens existem
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    
    if (!attacker || !target) {
        return result;
    }
    
    // Verificar se a habilidade existe
    const Skill* skill = SkillSystem::GetInstance().GetSkill(skillId);
    if (!skill) {
        return result;
    }
    
    // Verificar se está no alcance
    if (!IsInRange(attackerId, targetId, skill->range)) {
        return result;
    }
    
    // Verificar acerto
    if (!CheckHit(attackerId, targetId)) {
        return result;
    }
    
    // Verificar bloqueio
    result.blocked = CheckBlock(targetId);
    
    // Verificar crítico
    result.critical = CheckCritical(attackerId);
    
    // Calcular dano
    result.damage = CalculateDamage(attackerId, targetId, true);
    
    // Aplicar dano
    if (result.damage > 0) {
        CharacterSystem::GetInstance().ApplyDamage(targetId, result.damage);
    }
    
    result.hit = true;
    return result;
}

std::vector<CombatResult> CombatSystem::ProcessAreaEffect(uint32_t attackerId, const Position& center, float radius, uint16_t skillId) {
    std::vector<CombatResult> results;
    
    // Verificar se o atacante existe
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    if (!attacker) {
        return results;
    }
    
    // Verificar se a habilidade existe
    const Skill* skill = SkillSystem::GetInstance().GetSkill(skillId);
    if (!skill) {
        return results;
    }
    
    // Processar efeito em área para cada alvo no raio
    // TODO: Implementar busca de alvos no raio
    // Por enquanto, vamos apenas processar o atacante
    if (IsInArea(attackerId, center, radius)) {
        CombatResult result = ProcessSkill(attackerId, attackerId, skillId);
        results.push_back(result);
    }
    
    return results;
}

bool CombatSystem::IsInRange(uint32_t attackerId, uint32_t targetId, float range) const {
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    
    if (!attacker || !target) {
        return false;
    }
    
    float distance = CalculateDistance(attacker->Position, target->Position);
    return distance <= range;
}

bool CombatSystem::IsInArea(uint32_t targetId, const Position& center, float radius) const {
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    
    if (!target) {
        return false;
    }
    
    float distance = CalculateDistance(center, target->Position);
    return distance <= radius;
}

uint32_t CombatSystem::CalculateDamage(uint32_t attackerId, uint32_t targetId, bool isSkill) const {
    const CombatStats* attackerStats = GetCombatStats(attackerId);
    const CombatStats* targetStats = GetCombatStats(targetId);
    
    if (!attackerStats || !targetStats) {
        return 0;
    }
    
    // Gerar número aleatório para o dano
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> damageDist(attackerStats->minDamage, attackerStats->maxDamage);
    
    uint32_t baseDamage = damageDist(gen);
    
    // Aplicar modificadores
    if (isSkill) {
        baseDamage += attackerStats->skillDamage;
        baseDamage -= targetStats->skillDefense;
    }
    
    // Aplicar defesa
    baseDamage = (baseDamage > targetStats->defense) ? (baseDamage - targetStats->defense) : 1;
    
    return baseDamage;
}

bool CombatSystem::CheckHit(uint32_t attackerId, uint32_t targetId) const {
    const CombatStats* attackerStats = GetCombatStats(attackerId);
    const CombatStats* targetStats = GetCombatStats(targetId);
    
    if (!attackerStats || !targetStats) {
        return false;
    }
    
    // Gerar número aleatório para a taxa de acerto
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> hitDist(1, 100);
    
    uint32_t hitRoll = hitDist(gen);
    return hitRoll <= attackerStats->attackRate;
}

bool CombatSystem::CheckCritical(uint32_t attackerId) const {
    const CombatStats* attackerStats = GetCombatStats(attackerId);
    
    if (!attackerStats) {
        return false;
    }
    
    // Gerar número aleatório para a taxa de crítico
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> critDist(1, 100);
    
    uint32_t critRoll = critDist(gen);
    return critRoll <= attackerStats->criticalRate;
}

bool CombatSystem::CheckBlock(uint32_t targetId) const {
    const CombatStats* targetStats = GetCombatStats(targetId);
    
    if (!targetStats) {
        return false;
    }
    
    // Gerar número aleatório para a taxa de bloqueio
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> blockDist(1, 100);
    
    uint32_t blockRoll = blockDist(gen);
    return blockRoll <= targetStats->blockRate;
}

float CombatSystem::CalculateDistance(const Position& pos1, const Position& pos2) const {
    float dx = pos2.X - pos1.X;
    float dy = pos2.Y - pos1.Y;
    float dz = pos2.Z - pos1.Z;
    
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

CombatResult CombatSystem::PerformBasicAttack(uint32_t attackerId, uint32_t targetId) {
    CombatResult result;
    result.success = false;
    result.targetDied = false;
    
    // Verificar se os personagens existem
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    if (!attacker || !target) {
        return result;
    }
    
    // Verificar se o alvo está vivo
    if (target->Health == 0) {
        return result;
    }
    
    // Verificar esquiva
    if (RollDodge(targetId)) {
        return result;
    }
    
    // Calcular dano
    uint32_t damage = CalculatePhysicalDamage(attackerId, targetId);
    
    // Verificar crítico
    bool isCritical = RollCritical(attackerId);
    if (isCritical) {
        damage = static_cast<uint32_t>(damage * CRITICAL_DAMAGE_MULTIPLIER);
    }
    
    // Verificar bloqueio
    if (RollBlock(targetId)) {
        damage = static_cast<uint32_t>(damage * BLOCK_DAMAGE_REDUCTION);
    }
    
    // Aplicar dano
    Damage damageInfo;
    damageInfo.amount = damage;
    damageInfo.type = DAMAGE_TYPE_PHYSICAL;
    damageInfo.isCritical = isCritical;
    
    ApplyCombatEffects(targetId, damageInfo);
    
    // Verificar se o alvo morreu
    target = CharacterSystem::GetInstance().GetCharacter(targetId);
    result.targetDied = target->Health == 0;
    
    result.success = true;
    result.damage = damageInfo;
    result.affectedTargets.push_back(targetId);
    
    return result;
}

CombatResult CombatSystem::PerformRangedAttack(uint32_t attackerId, uint32_t targetId) {
    CombatResult result;
    result.success = false;
    result.targetDied = false;
    
    // Verificar se os personagens existem
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    if (!attacker || !target) {
        return result;
    }
    
    // Verificar se o alvo está vivo
    if (target->Health == 0) {
        return result;
    }
    
    // Verificar esquiva
    if (RollDodge(targetId)) {
        return result;
    }
    
    // Calcular dano
    uint32_t damage = CalculatePhysicalDamage(attackerId, targetId);
    
    // Verificar crítico
    bool isCritical = RollCritical(attackerId);
    if (isCritical) {
        damage = static_cast<uint32_t>(damage * CRITICAL_DAMAGE_MULTIPLIER);
    }
    
    // Verificar bloqueio
    if (RollBlock(targetId)) {
        damage = static_cast<uint32_t>(damage * BLOCK_DAMAGE_REDUCTION);
    }
    
    // Aplicar dano
    Damage damageInfo;
    damageInfo.amount = damage;
    damageInfo.type = DAMAGE_TYPE_PHYSICAL;
    damageInfo.isCritical = isCritical;
    
    ApplyCombatEffects(targetId, damageInfo);
    
    // Verificar se o alvo morreu
    target = CharacterSystem::GetInstance().GetCharacter(targetId);
    result.targetDied = target->Health == 0;
    
    result.success = true;
    result.damage = damageInfo;
    result.affectedTargets.push_back(targetId);
    
    return result;
}

CombatResult CombatSystem::UseSkill(uint32_t attackerId, uint32_t skillId, const std::vector<uint32_t>& targetIds) {
    CombatResult result;
    result.success = false;
    result.targetDied = false;
    
    // Verificar se o atacante existe
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    if (!attacker) {
        return result;
    }
    
    // Verificar se pode usar a habilidade
    if (!CanUseSkill(attackerId, skillId)) {
        return result;
    }
    
    // Processar cada alvo
    for (uint32_t targetId : targetIds) {
        const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
        if (!target || target->Health == 0) {
            continue;
        }
        
        // Verificar esquiva
        if (RollDodge(targetId)) {
            continue;
        }
        
        // Calcular dano mágico
        uint32_t damage = CalculateMagicalDamage(attackerId, targetId);
        
        // Verificar crítico
        bool isCritical = RollCritical(attackerId);
        if (isCritical) {
            damage = static_cast<uint32_t>(damage * CRITICAL_DAMAGE_MULTIPLIER);
        }
        
        // Verificar bloqueio
        if (RollBlock(targetId)) {
            damage = static_cast<uint32_t>(damage * BLOCK_DAMAGE_REDUCTION);
        }
        
        // Aplicar dano
        Damage damageInfo;
        damageInfo.amount = damage;
        damageInfo.type = DAMAGE_TYPE_MAGICAL;
        damageInfo.isCritical = isCritical;
        
        ApplyCombatEffects(targetId, damageInfo);
        
        // Verificar se o alvo morreu
        target = CharacterSystem::GetInstance().GetCharacter(targetId);
        if (target->Health == 0) {
            result.targetDied = true;
        }
        
        result.affectedTargets.push_back(targetId);
    }
    
    result.success = true;
    return result;
}

bool CombatSystem::CanUseSkill(uint32_t characterId, uint32_t skillId) const {
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    if (!character) {
        return false;
    }
    
    // Verificar mana
    if (character->Mana < 10) { // Valor mínimo de mana para usar habilidade
        return false;
    }
    
    // TODO: Verificar requisitos de nível, cooldown, etc.
    
    return true;
}

uint32_t CombatSystem::CalculatePhysicalDamage(uint32_t attackerId, uint32_t targetId) const {
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    if (!attacker || !target) {
        return 0;
    }
    
    // Dano base = Força * 2
    uint32_t baseDamage = attacker->Strength * 2;
    
    // Reduzir pela defesa física
    uint32_t defense = CalculatePhysicalDefense(targetId);
    if (defense >= baseDamage) {
        return 1; // Dano mínimo
    }
    
    return baseDamage - defense;
}

uint32_t CombatSystem::CalculateMagicalDamage(uint32_t attackerId, uint32_t targetId) const {
    const Mob* attacker = CharacterSystem::GetInstance().GetCharacter(attackerId);
    const Mob* target = CharacterSystem::GetInstance().GetCharacter(targetId);
    if (!attacker || !target) {
        return 0;
    }
    
    // Dano base = Inteligência * 2
    uint32_t baseDamage = attacker->Intelligence * 2;
    
    // Reduzir pela defesa mágica
    uint32_t defense = CalculateMagicalDefense(targetId);
    if (defense >= baseDamage) {
        return 1; // Dano mínimo
    }
    
    return baseDamage - defense;
}

uint32_t CombatSystem::CalculatePureDamage(uint32_t amount) const {
    return amount; // Dano puro não é reduzido
}

uint32_t CombatSystem::CalculatePhysicalDefense(uint32_t characterId) const {
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    if (!character) {
        return 0;
    }
    
    // Defesa física = Vitalidade
    return character->Vitality;
}

uint32_t CombatSystem::CalculateMagicalDefense(uint32_t characterId) const {
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    if (!character) {
        return 0;
    }
    
    // Defesa mágica = Inteligência
    return character->Intelligence;
}

bool CombatSystem::RollCritical(uint32_t characterId) const {
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    if (!character) {
        return false;
    }
    
    // Chance de crítico = Base + (Destreza / 100)
    float criticalChance = BASE_CRITICAL_CHANCE + (character->Dexterity / 100.0f);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    return dis(gen) < criticalChance;
}

bool CombatSystem::RollDodge(uint32_t characterId) const {
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    if (!character) {
        return false;
    }
    
    // Chance de esquiva = Base + (Agilidade / 100)
    float dodgeChance = BASE_DODGE_CHANCE + (character->Agility / 100.0f);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    return dis(gen) < dodgeChance;
}

bool CombatSystem::RollBlock(uint32_t characterId) const {
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    if (!character) {
        return false;
    }
    
    // Chance de bloqueio = Base + (Força / 100)
    float blockChance = BASE_BLOCK_CHANCE + (character->Strength / 100.0f);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    return dis(gen) < blockChance;
}

void CombatSystem::ApplyCombatEffects(uint32_t targetId, const Damage& damage) {
    // Aplicar dano
    CharacterSystem::GetInstance().UpdateHealth(targetId, -static_cast<int32_t>(damage.amount));
    
    // TODO: Aplicar outros efeitos (sangramento, veneno, etc.)
}

void CombatSystem::ProcessCombatEffects(uint32_t characterId, float deltaTime) {
    // TODO: Processar efeitos contínuos (sangramento, veneno, etc.)
}

} // namespace WYDBR 