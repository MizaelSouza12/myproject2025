#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

namespace WYDBR {

// Classes de personagem
enum CharacterClass {
    CHARACTER_WARRIOR,
    CHARACTER_MAGE,
    CHARACTER_ARCHER,
    CHARACTER_PRIEST,
    CHARACTER_ROGUE
};

// Estrutura do personagem
struct Mob {
    uint32_t id;
    std::string name;
    CharacterClass Class;
    uint32_t Level;
    uint32_t Experience;
    uint32_t Health;
    uint32_t MaxHealth;
    uint32_t Mana;
    uint32_t MaxMana;
    uint32_t Strength;
    uint32_t Dexterity;
    uint32_t Intelligence;
    uint32_t Vitality;
    uint32_t Agility;
    uint32_t Luck;
};

// Sistema de personagem
class CharacterSystem {
public:
    static CharacterSystem& GetInstance();
    
    void Initialize();
    void Shutdown();
    
    // Criação e remoção de personagens
    uint32_t CreateCharacter(const std::string& name, CharacterClass characterClass);
    bool DeleteCharacter(uint32_t characterId);
    
    // Gerenciamento de personagens
    const Mob* GetCharacter(uint32_t characterId) const;
    bool UpdateCharacter(const Mob& character);
    
    // Atributos
    bool UpdateHealth(uint32_t characterId, int32_t amount);
    bool UpdateMana(uint32_t characterId, int32_t amount);
    bool UpdateExperience(uint32_t characterId, uint32_t amount);
    
    // Nível
    bool LevelUp(uint32_t characterId);
    uint32_t GetExperienceForNextLevel(uint32_t currentLevel) const;
    
private:
    CharacterSystem() = default;
    ~CharacterSystem() = default;
    CharacterSystem(const CharacterSystem&) = delete;
    CharacterSystem& operator=(const CharacterSystem&) = delete;
    
    static constexpr uint32_t BASE_HEALTH = 100;
    static constexpr uint32_t BASE_MANA = 50;
    static constexpr uint32_t BASE_STRENGTH = 10;
    static constexpr uint32_t BASE_DEXTERITY = 10;
    static constexpr uint32_t BASE_INTELLIGENCE = 10;
    static constexpr uint32_t BASE_VITALITY = 10;
    static constexpr uint32_t BASE_AGILITY = 10;
    static constexpr uint32_t BASE_LUCK = 5;
    
    uint32_t nextCharacterId = 1;
    std::unordered_map<uint32_t, Mob> characters;
};

} // namespace WYDBR 