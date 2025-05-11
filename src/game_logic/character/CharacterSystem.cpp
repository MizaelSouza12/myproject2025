#include "GameLogic/character/CharacterSystem.h"
#include <algorithm>
#include <cmath>

namespace WYDBR {

CharacterSystem& CharacterSystem::GetInstance() {
    static CharacterSystem instance;
    return instance;
}

void CharacterSystem::Initialize() {
    characters.clear();
    nextCharacterId = 1;
}

void CharacterSystem::Shutdown() {
    characters.clear();
}

uint32_t CharacterSystem::CreateCharacter(const std::string& name, CharacterClass characterClass) {
    Mob character;
    character.id = nextCharacterId++;
    character.name = name;
    character.Class = characterClass;
    character.Level = 1;
    character.Experience = 0;
    
    // Atributos base
    character.Health = BASE_HEALTH;
    character.MaxHealth = BASE_HEALTH;
    character.Mana = BASE_MANA;
    character.MaxMana = BASE_MANA;
    character.Strength = BASE_STRENGTH;
    character.Dexterity = BASE_DEXTERITY;
    character.Intelligence = BASE_INTELLIGENCE;
    character.Vitality = BASE_VITALITY;
    character.Agility = BASE_AGILITY;
    character.Luck = BASE_LUCK;
    
    // Bônus de classe
    switch (characterClass) {
        case CHARACTER_WARRIOR:
            character.Strength += 5;
            character.Vitality += 5;
            break;
        case CHARACTER_MAGE:
            character.Intelligence += 5;
            character.Mana += 50;
            character.MaxMana += 50;
            break;
        case CHARACTER_ARCHER:
            character.Dexterity += 5;
            character.Agility += 5;
            break;
        case CHARACTER_PRIEST:
            character.Intelligence += 3;
            character.Vitality += 3;
            character.Mana += 25;
            character.MaxMana += 25;
            break;
        case CHARACTER_ROGUE:
            character.Dexterity += 5;
            character.Agility += 5;
            break;
    }
    
    characters[character.id] = character;
    return character.id;
}

bool CharacterSystem::DeleteCharacter(uint32_t characterId) {
    return characters.erase(characterId) > 0;
}

const Mob* CharacterSystem::GetCharacter(uint32_t characterId) const {
    auto it = characters.find(characterId);
    return it != characters.end() ? &it->second : nullptr;
}

bool CharacterSystem::UpdateCharacter(const Mob& character) {
    auto it = characters.find(character.id);
    if (it == characters.end()) {
        return false;
    }
    
    it->second = character;
    return true;
}

bool CharacterSystem::UpdateHealth(uint32_t characterId, int32_t amount) {
    auto it = characters.find(characterId);
    if (it == characters.end()) {
        return false;
    }
    
    int32_t newHealth = static_cast<int32_t>(it->second.Health) + amount;
    newHealth = std::max(0, std::min(newHealth, static_cast<int32_t>(it->second.MaxHealth)));
    
    it->second.Health = static_cast<uint32_t>(newHealth);
    return true;
}

bool CharacterSystem::UpdateMana(uint32_t characterId, int32_t amount) {
    auto it = characters.find(characterId);
    if (it == characters.end()) {
        return false;
    }
    
    int32_t newMana = static_cast<int32_t>(it->second.Mana) + amount;
    newMana = std::max(0, std::min(newMana, static_cast<int32_t>(it->second.MaxMana)));
    
    it->second.Mana = static_cast<uint32_t>(newMana);
    return true;
}

bool CharacterSystem::UpdateExperience(uint32_t characterId, uint32_t amount) {
    auto it = characters.find(characterId);
    if (it == characters.end()) {
        return false;
    }
    
    it->second.Experience += amount;
    
    // Verificar level up
    while (it->second.Experience >= GetExperienceForNextLevel(it->second.Level)) {
        if (!LevelUp(characterId)) {
            break;
        }
    }
    
    return true;
}

bool CharacterSystem::LevelUp(uint32_t characterId) {
    auto it = characters.find(characterId);
    if (it == characters.end()) {
        return false;
    }
    
    // Aumentar nível
    it->second.Level++;
    
    // Aumentar atributos
    it->second.MaxHealth += 10 + (it->second.Vitality / 2);
    it->second.Health = it->second.MaxHealth;
    
    it->second.MaxMana += 5 + (it->second.Intelligence / 2);
    it->second.Mana = it->second.MaxMana;
    
    // Bônus de classe
    switch (it->second.Class) {
        case CHARACTER_WARRIOR:
            it->second.Strength += 2;
            it->second.Vitality += 2;
            break;
        case CHARACTER_MAGE:
            it->second.Intelligence += 2;
            it->second.MaxMana += 10;
            it->second.Mana = it->second.MaxMana;
            break;
        case CHARACTER_ARCHER:
            it->second.Dexterity += 2;
            it->second.Agility += 2;
            break;
        case CHARACTER_PRIEST:
            it->second.Intelligence += 2;
            it->second.Vitality += 1;
            it->second.MaxMana += 5;
            it->second.Mana = it->second.MaxMana;
            break;
        case CHARACTER_ROGUE:
            it->second.Dexterity += 2;
            it->second.Agility += 2;
            break;
    }
    
    return true;
}

uint32_t CharacterSystem::GetExperienceForNextLevel(uint32_t currentLevel) const {
    // Fórmula: 100 * (nível ^ 1.5)
    return static_cast<uint32_t>(100 * std::pow(currentLevel, 1.5));
}

void CharacterSystem::UpdateCharacter(uint32_t characterId, float deltaTime) {
    Mob* character = GetCharacter(characterId);
    if (!character) {
        return;
    }
    
    // Atualizar status
    static float statusTimer = 0.0f;
    statusTimer += deltaTime;
    
    if (statusTimer >= STATUS_UPDATE_INTERVAL) {
        // Atualizar duração dos status
        for (auto& status : character->StatusEffects) {
            status.Duration -= STATUS_UPDATE_INTERVAL;
            if (status.Duration <= 0.0f) {
                RemoveStatus(characterId, status.Type);
            }
        }
        
        statusTimer = 0.0f;
    }
    
    // Regeneração de HP e MP
    static float regenTimer = 0.0f;
    regenTimer += deltaTime;
    
    if (regenTimer >= REGEN_INTERVAL) {
        // Regenerar HP
        if (character->Health < character->MaxHealth) {
            uint32_t hpRegen = static_cast<uint32_t>(character->MaxHealth * HP_REGEN_RATE);
            character->Health = std::min(character->Health + hpRegen, character->MaxHealth);
        }
        
        // Regenerar MP
        if (character->Mana < character->MaxMana) {
            uint32_t mpRegen = static_cast<uint32_t>(character->MaxMana * MP_REGEN_RATE);
            character->Mana = std::min(character->Mana + mpRegen, character->MaxMana);
        }
        
        regenTimer = 0.0f;
    }
}

bool CharacterSystem::ApplyDamage(uint32_t characterId, uint32_t damage) {
    Mob* character = GetCharacter(characterId);
    if (!character || character->Health == 0) {
        return false;
    }
    
    // Aplicar dano
    if (damage >= character->Health) {
        character->Health = 0;
        return true; // Personagem morreu
    } else {
        character->Health -= damage;
        return false;
    }
}

void CharacterSystem::ApplyHeal(uint32_t characterId, uint32_t amount) {
    Mob* character = GetCharacter(characterId);
    if (!character || character->Health == 0) {
        return;
    }
    
    // Aplicar cura
    character->Health = std::min(character->Health + amount, character->MaxHealth);
}

void CharacterSystem::ApplyStatus(uint32_t characterId, uint32_t status, float duration) {
    Mob* character = GetCharacter(characterId);
    if (!character) {
        return;
    }
    
    // Verificar se o status já existe
    auto it = std::find_if(character->StatusEffects.begin(), character->StatusEffects.end(),
                          [status](const StatusEffect& effect) { return effect.Type == status; });
    
    if (it != character->StatusEffects.end()) {
        // Atualizar duração
        it->Duration = std::max(it->Duration, duration);
    } else {
        // Adicionar novo status
        StatusEffect effect;
        effect.Type = status;
        effect.Duration = duration;
        character->StatusEffects.push_back(effect);
    }
    
    // Atualizar flags de status
    character->Status |= status;
}

void CharacterSystem::RemoveStatus(uint32_t characterId, uint32_t status) {
    Mob* character = GetCharacter(characterId);
    if (!character) {
        return;
    }
    
    // Remover status da lista
    character->StatusEffects.erase(
        std::remove_if(character->StatusEffects.begin(), character->StatusEffects.end(),
                      [status](const StatusEffect& effect) { return effect.Type == status; }),
        character->StatusEffects.end()
    );
    
    // Atualizar flags de status
    character->Status &= ~status;
}

bool CharacterSystem::HasStatus(uint32_t characterId, uint32_t status) const {
    const Mob* character = GetCharacter(characterId);
    if (!character) {
        return false;
    }
    
    return (character->Status & status) != 0;
}

void CharacterSystem::UpdatePosition(uint32_t characterId, const Position& position) {
    Mob* character = GetCharacter(characterId);
    if (!character) {
        return;
    }
    
    character->Position = position;
}

Position CharacterSystem::GetPosition(uint32_t characterId) const {
    const Mob* character = GetCharacter(characterId);
    if (!character) {
        return {0.0f, 0.0f, 0.0f};
    }
    
    return character->Position;
}

bool CharacterSystem::IsAlive(uint32_t characterId) const {
    const Mob* character = GetCharacter(characterId);
    return character && character->Health > 0;
}

bool CharacterSystem::CanUseSkill(uint32_t characterId, uint16_t skillId) const {
    const Mob* character = GetCharacter(characterId);
    if (!character) {
        return false;
    }
    
    // Verificar se tem MP suficiente
    if (character->Mana < 10) { // Valor mínimo de MP para usar habilidade
        return false;
    }
    
    // Verificar se a habilidade está disponível
    if (character->SkillLevel[skillId] == 0) {
        return false;
    }
    
    return true;
}

bool CharacterSystem::UseSkill(uint32_t characterId, uint16_t skillId) {
    Mob* character = GetCharacter(characterId);
    if (!character || !CanUseSkill(characterId, skillId)) {
        return false;
    }
    
    // Consumir MP
    character->Mana -= 10; // Valor base de MP para usar habilidade
    
    return true;
}

} // namespace WYDBR 