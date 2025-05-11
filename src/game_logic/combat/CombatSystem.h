#pragma once

#include <unordered_map>
#include <vector>
#include <cstdint>

namespace WYDBR {

// Tipos de dano
enum DamageType {
    DAMAGE_TYPE_PHYSICAL,
    DAMAGE_TYPE_MAGICAL,
    DAMAGE_TYPE_PURE
};

// Estrutura de dano
struct Damage {
    uint32_t amount;
    DamageType type;
    bool isCritical;
};

// Estrutura de resultado de combate
struct CombatResult {
    bool success;
    Damage damage;
    bool targetDied;
    std::vector<uint32_t> affectedTargets;
};

// Sistema de combate
class CombatSystem {
public:
    static CombatSystem& GetInstance();
    
    void Initialize();
    void Shutdown();
    
    // Ataques básicos
    CombatResult PerformBasicAttack(uint32_t attackerId, uint32_t targetId);
    CombatResult PerformRangedAttack(uint32_t attackerId, uint32_t targetId);
    
    // Habilidades
    CombatResult UseSkill(uint32_t attackerId, uint32_t skillId, const std::vector<uint32_t>& targetIds);
    bool CanUseSkill(uint32_t characterId, uint32_t skillId) const;
    
    // Cálculos de dano
    uint32_t CalculatePhysicalDamage(uint32_t attackerId, uint32_t targetId) const;
    uint32_t CalculateMagicalDamage(uint32_t attackerId, uint32_t targetId) const;
    uint32_t CalculatePureDamage(uint32_t amount) const;
    
    // Cálculos de defesa
    uint32_t CalculatePhysicalDefense(uint32_t characterId) const;
    uint32_t CalculateMagicalDefense(uint32_t characterId) const;
    
    // Cálculos de chance
    bool RollCritical(uint32_t characterId) const;
    bool RollDodge(uint32_t characterId) const;
    bool RollBlock(uint32_t characterId) const;
    
    // Efeitos de combate
    void ApplyCombatEffects(uint32_t targetId, const Damage& damage);
    void ProcessCombatEffects(uint32_t characterId, float deltaTime);
    
private:
    CombatSystem() = default;
    ~CombatSystem() = default;
    CombatSystem(const CombatSystem&) = delete;
    CombatSystem& operator=(const CombatSystem&) = delete;
    
    static constexpr float BASE_CRITICAL_CHANCE = 0.05f;
    static constexpr float BASE_DODGE_CHANCE = 0.05f;
    static constexpr float BASE_BLOCK_CHANCE = 0.05f;
    static constexpr float CRITICAL_DAMAGE_MULTIPLIER = 2.0f;
    static constexpr float BLOCK_DAMAGE_REDUCTION = 0.5f;
    
    std::unordered_map<uint32_t, std::vector<uint32_t>> activeEffects;
};

} // namespace WYDBR 