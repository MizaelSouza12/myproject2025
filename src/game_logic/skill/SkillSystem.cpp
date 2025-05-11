#include "GameLogic/skill/SkillSystem.h"
#include "GameLogic/character/CharacterSystem.h"
#include "GameLogic/combat/CombatSystem.h"
#include <algorithm>

namespace WYDBR {

SkillSystem& SkillSystem::GetInstance() {
    static SkillSystem instance;
    return instance;
}

void SkillSystem::Initialize() {
    skills.clear();
    cooldowns.clear();
    learnedSkills.clear();
}

void SkillSystem::Shutdown() {
    skills.clear();
    cooldowns.clear();
    learnedSkills.clear();
}

void SkillSystem::RegisterSkill(const Skill& skill) {
    skills[skill.id] = skill;
}

const Skill* SkillSystem::GetSkill(uint16_t skillId) const {
    auto it = skills.find(skillId);
    return it != skills.end() ? &it->second : nullptr;
}

bool SkillSystem::CanLearnSkill(uint32_t characterId, uint16_t skillId) const {
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    const Skill* skill = GetSkill(skillId);
    
    if (!character || !skill) {
        return false;
    }
    
    // Verificar classe
    if (skill->requiredClass != character->Class) {
        return false;
    }
    
    // Verificar nível
    if (character->Level < skill->requiredLevel) {
        return false;
    }
    
    // Verificar se já aprendeu
    auto it = learnedSkills.find(characterId);
    if (it != learnedSkills.end()) {
        auto skillIt = std::find(it->second.begin(), it->second.end(), skillId);
        if (skillIt != it->second.end()) {
            return false;
        }
    }
    
    return true;
}

bool SkillSystem::LearnSkill(uint32_t characterId, uint16_t skillId) {
    if (!CanLearnSkill(characterId, skillId)) {
        return false;
    }
    
    learnedSkills[characterId].push_back(skillId);
    return true;
}

bool SkillSystem::CanUseSkill(uint32_t characterId, uint16_t skillId) const {
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    const Skill* skill = GetSkill(skillId);
    
    if (!character || !skill) {
        return false;
    }
    
    // Verificar se aprendeu a habilidade
    auto it = learnedSkills.find(characterId);
    if (it == learnedSkills.end()) {
        return false;
    }
    
    auto skillIt = std::find(it->second.begin(), it->second.end(), skillId);
    if (skillIt == it->second.end()) {
        return false;
    }
    
    // Verificar MP
    if (character->MP < skill->mpCost) {
        return false;
    }
    
    // Verificar cooldown
    if (IsSkillOnCooldown(characterId, skillId)) {
        return false;
    }
    
    return true;
}

bool SkillSystem::UseSkill(uint32_t characterId, uint16_t skillId, uint32_t targetId) {
    if (!CanUseSkill(characterId, skillId)) {
        return false;
    }
    
    const Skill* skill = GetSkill(skillId);
    Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    
    if (!skill || !character) {
        return false;
    }
    
    // Consumir MP
    character->MP -= skill->mpCost;
    
    // Aplicar cooldown
    cooldowns[characterId][skillId] = {skill->cooldown, skill->cooldown};
    
    // Aplicar efeitos
    for (const auto& effect : skill->effects) {
        if (effect.isAreaEffect) {
            // Efeito em área
            Position center = character->Position;
            CombatSystem::GetInstance().ProcessAreaEffect(characterId, center, effect.radius, skillId);
        } else if (targetId > 0) {
            // Efeito em alvo
            switch (effect.type) {
                case EFFECT_DAMAGE:
                    CombatSystem::GetInstance().ProcessSkill(characterId, targetId, skillId);
                    break;
                    
                case EFFECT_HEAL:
                    CharacterSystem::GetInstance().ApplyHeal(targetId, static_cast<uint32_t>(effect.value));
                    break;
                    
                case EFFECT_STATUS:
                    CharacterSystem::GetInstance().ApplyStatus(targetId, static_cast<uint32_t>(effect.value), effect.duration);
                    break;
            }
        }
    }
    
    return true;
}

void SkillSystem::Update(float deltaTime) {
    // Atualizar cooldowns
    for (auto& characterCooldowns : cooldowns) {
        for (auto it = characterCooldowns.second.begin(); it != characterCooldowns.second.end();) {
            it->second.remainingTime -= deltaTime;
            
            if (it->second.remainingTime <= 0.0f) {
                it = characterCooldowns.second.erase(it);
            } else {
                ++it;
            }
        }
    }
}

bool SkillSystem::IsSkillOnCooldown(uint32_t characterId, uint16_t skillId) const {
    auto it = cooldowns.find(characterId);
    if (it == cooldowns.end()) {
        return false;
    }
    
    auto skillIt = it->second.find(skillId);
    return skillIt != it->second.end();
}

float SkillSystem::GetSkillCooldownRemaining(uint32_t characterId, uint16_t skillId) const {
    auto it = cooldowns.find(characterId);
    if (it == cooldowns.end()) {
        return 0.0f;
    }
    
    auto skillIt = it->second.find(skillId);
    return skillIt != it->second.end() ? skillIt->second.remainingTime : 0.0f;
}

std::vector<const Skill*> SkillSystem::GetAvailableSkills(CharacterClass classType) const {
    std::vector<const Skill*> availableSkills;
    
    for (const auto& skill : skills) {
        if (skill.second.requiredClass == classType) {
            availableSkills.push_back(&skill.second);
        }
    }
    
    return availableSkills;
}

std::vector<const Skill*> SkillSystem::GetLearnedSkills(uint32_t characterId) const {
    std::vector<const Skill*> learnedSkillsList;
    
    auto it = learnedSkills.find(characterId);
    if (it != learnedSkills.end()) {
        for (uint16_t skillId : it->second) {
            const Skill* skill = GetSkill(skillId);
            if (skill) {
                learnedSkillsList.push_back(skill);
            }
        }
    }
    
    return learnedSkillsList;
}

} // namespace WYDBR 