/**
 * @file SkillSystem.h
 * @brief Sistema de habilidades do WYDBR 2.0
 */

#ifndef WYDBR_SKILL_SYSTEM_H
#define WYDBR_SKILL_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "wyd_core.h"
#include "mob_struct.h"

namespace WYDBR {

/**
 * @class SkillSystem
 * @brief Gerencia o sistema de habilidades do jogo
 */
class SkillSystem {
public:
    static SkillSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de habilidades
    bool LearnSkill(uint32_t characterId, uint16_t skillId);
    bool UpgradeSkill(uint32_t characterId, uint16_t skillId);
    bool UseSkill(uint32_t characterId, uint16_t skillId, uint32_t targetId);
    bool CancelSkill(uint32_t characterId, uint16_t skillId);

    // Verificações
    bool HasSkill(uint32_t characterId, uint16_t skillId) const;
    bool CanUseSkill(uint32_t characterId, uint16_t skillId) const;
    bool IsSkillInCooldown(uint32_t characterId, uint16_t skillId) const;
    bool HasEnoughMP(uint32_t characterId, uint16_t skillId) const;

    // Queries
    uint8_t GetSkillLevel(uint32_t characterId, uint16_t skillId) const;
    uint32_t GetSkillCooldown(uint32_t characterId, uint16_t skillId) const;
    uint16_t GetSkillMPCost(uint32_t characterId, uint16_t skillId) const;
    std::vector<uint16_t> GetCharacterSkills(uint32_t characterId) const;

    // Utilidades
    void UpdateSkillCooldowns(uint32_t characterId, uint32_t deltaTime);
    void ResetSkillCooldowns(uint32_t characterId);
    void ApplySkillEffects(uint32_t casterId, uint32_t targetId, uint16_t skillId);
    void RemoveSkillEffects(uint32_t targetId, uint16_t skillId);

private:
    SkillSystem() = default;
    ~SkillSystem() = default;

    // Prevenir cópias
    SkillSystem(const SkillSystem&) = delete;
    SkillSystem& operator=(const SkillSystem&) = delete;

    // Cálculos internos
    bool ValidateSkill(uint16_t skillId) const;
    bool CheckSkillRequirements(uint32_t characterId, uint16_t skillId) const;
    void CalculateSkillDamage(uint32_t casterId, uint32_t targetId, uint16_t skillId);
    void ApplySkillBuffs(uint32_t targetId, uint16_t skillId);

    // Estado
    struct SkillData {
        uint8_t level;
        uint32_t lastUseTime;
        uint32_t cooldown;
        bool isActive;
    };

    struct CharacterSkills {
        std::unordered_map<uint16_t, SkillData> skills;
        std::vector<uint16_t> activeEffects;
    };

    std::unordered_map<uint32_t, CharacterSkills> m_characterSkills;
};

} // namespace WYDBR

#endif // WYDBR_SKILL_SYSTEM_H 