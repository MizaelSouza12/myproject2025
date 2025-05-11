#include <gtest/gtest.h>
#include "GameLogic/character/CharacterSystem.h"

using namespace WYDBR;

class CharacterSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        CharacterSystem::GetInstance().Initialize();
    }
    
    void TearDown() override {
        CharacterSystem::GetInstance().Shutdown();
    }
};

TEST_F(CharacterSystemTest, CreateAndDeleteCharacter) {
    auto& characterSystem = CharacterSystem::GetInstance();
    
    // Criar personagem
    uint32_t characterId = characterSystem.CreateCharacter("Test", CHARACTER_WARRIOR);
    EXPECT_GT(characterId, 0);
    
    // Verificar personagem criado
    const Mob* character = characterSystem.GetCharacter(characterId);
    EXPECT_NE(character, nullptr);
    EXPECT_EQ(character->name, "Test");
    EXPECT_EQ(character->Class, CHARACTER_WARRIOR);
    EXPECT_EQ(character->Level, 1);
    EXPECT_EQ(character->Experience, 0);
    
    // Verificar atributos base
    EXPECT_EQ(character->Health, 100);
    EXPECT_EQ(character->MaxHealth, 100);
    EXPECT_EQ(character->Mana, 50);
    EXPECT_EQ(character->MaxMana, 50);
    EXPECT_EQ(character->Strength, 15); // 10 base + 5 bônus
    EXPECT_EQ(character->Dexterity, 10);
    EXPECT_EQ(character->Intelligence, 10);
    EXPECT_EQ(character->Vitality, 15); // 10 base + 5 bônus
    EXPECT_EQ(character->Agility, 10);
    EXPECT_EQ(character->Luck, 5);
    
    // Deletar personagem
    EXPECT_TRUE(characterSystem.DeleteCharacter(characterId));
    EXPECT_EQ(characterSystem.GetCharacter(characterId), nullptr);
}

TEST_F(CharacterSystemTest, UpdateCharacter) {
    auto& characterSystem = CharacterSystem::GetInstance();
    
    // Criar personagem
    uint32_t characterId = characterSystem.CreateCharacter("Test", CHARACTER_WARRIOR);
    EXPECT_GT(characterId, 0);
    
    // Atualizar personagem
    Mob updatedCharacter;
    updatedCharacter.id = characterId;
    updatedCharacter.name = "Updated";
    updatedCharacter.Class = CHARACTER_WARRIOR;
    updatedCharacter.Level = 2;
    updatedCharacter.Experience = 100;
    updatedCharacter.Health = 150;
    updatedCharacter.MaxHealth = 150;
    updatedCharacter.Mana = 75;
    updatedCharacter.MaxMana = 75;
    updatedCharacter.Strength = 20;
    updatedCharacter.Dexterity = 15;
    updatedCharacter.Intelligence = 12;
    updatedCharacter.Vitality = 18;
    updatedCharacter.Agility = 13;
    updatedCharacter.Luck = 7;
    
    EXPECT_TRUE(characterSystem.UpdateCharacter(updatedCharacter));
    
    // Verificar atualização
    const Mob* character = characterSystem.GetCharacter(characterId);
    EXPECT_NE(character, nullptr);
    EXPECT_EQ(character->name, "Updated");
    EXPECT_EQ(character->Level, 2);
    EXPECT_EQ(character->Experience, 100);
    EXPECT_EQ(character->Health, 150);
    EXPECT_EQ(character->MaxHealth, 150);
    EXPECT_EQ(character->Mana, 75);
    EXPECT_EQ(character->MaxMana, 75);
    EXPECT_EQ(character->Strength, 20);
    EXPECT_EQ(character->Dexterity, 15);
    EXPECT_EQ(character->Intelligence, 12);
    EXPECT_EQ(character->Vitality, 18);
    EXPECT_EQ(character->Agility, 13);
    EXPECT_EQ(character->Luck, 7);
}

TEST_F(CharacterSystemTest, UpdateHealth) {
    auto& characterSystem = CharacterSystem::GetInstance();
    
    // Criar personagem
    uint32_t characterId = characterSystem.CreateCharacter("Test", CHARACTER_WARRIOR);
    EXPECT_GT(characterId, 0);
    
    // Reduzir vida
    EXPECT_TRUE(characterSystem.UpdateHealth(characterId, -50));
    const Mob* character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Health, 50);
    
    // Aumentar vida
    EXPECT_TRUE(characterSystem.UpdateHealth(characterId, 25));
    character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Health, 75);
    
    // Tentar ultrapassar vida máxima
    EXPECT_TRUE(characterSystem.UpdateHealth(characterId, 100));
    character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Health, character->MaxHealth);
    
    // Tentar reduzir abaixo de zero
    EXPECT_TRUE(characterSystem.UpdateHealth(characterId, -200));
    character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Health, 0);
}

TEST_F(CharacterSystemTest, UpdateMana) {
    auto& characterSystem = CharacterSystem::GetInstance();
    
    // Criar personagem
    uint32_t characterId = characterSystem.CreateCharacter("Test", CHARACTER_WARRIOR);
    EXPECT_GT(characterId, 0);
    
    // Reduzir mana
    EXPECT_TRUE(characterSystem.UpdateMana(characterId, -25));
    const Mob* character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Mana, 25);
    
    // Aumentar mana
    EXPECT_TRUE(characterSystem.UpdateMana(characterId, 15));
    character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Mana, 40);
    
    // Tentar ultrapassar mana máxima
    EXPECT_TRUE(characterSystem.UpdateMana(characterId, 100));
    character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Mana, character->MaxMana);
    
    // Tentar reduzir abaixo de zero
    EXPECT_TRUE(characterSystem.UpdateMana(characterId, -100));
    character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Mana, 0);
}

TEST_F(CharacterSystemTest, ExperienceAndLevelUp) {
    auto& characterSystem = CharacterSystem::GetInstance();
    
    // Criar personagem
    uint32_t characterId = characterSystem.CreateCharacter("Test", CHARACTER_WARRIOR);
    EXPECT_GT(characterId, 0);
    
    // Verificar experiência inicial
    const Mob* character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Level, 1);
    EXPECT_EQ(character->Experience, 0);
    
    // Adicionar experiência suficiente para level up
    uint32_t expForNextLevel = characterSystem.GetExperienceForNextLevel(1);
    EXPECT_TRUE(characterSystem.UpdateExperience(characterId, expForNextLevel));
    
    // Verificar level up
    character = characterSystem.GetCharacter(characterId);
    EXPECT_EQ(character->Level, 2);
    EXPECT_EQ(character->Experience, expForNextLevel);
    
    // Verificar aumento de atributos
    EXPECT_GT(character->MaxHealth, 100);
    EXPECT_GT(character->MaxMana, 50);
    EXPECT_GT(character->Strength, 15);
    EXPECT_GT(character->Vitality, 15);
}

TEST_F(CharacterSystemTest, ClassBonuses) {
    auto& characterSystem = CharacterSystem::GetInstance();
    
    // Criar personagens de diferentes classes
    uint32_t warriorId = characterSystem.CreateCharacter("Warrior", CHARACTER_WARRIOR);
    uint32_t mageId = characterSystem.CreateCharacter("Mage", CHARACTER_MAGE);
    uint32_t archerId = characterSystem.CreateCharacter("Archer", CHARACTER_ARCHER);
    uint32_t priestId = characterSystem.CreateCharacter("Priest", CHARACTER_PRIEST);
    uint32_t rogueId = characterSystem.CreateCharacter("Rogue", CHARACTER_ROGUE);
    
    // Verificar bônus de guerreiro
    const Mob* warrior = characterSystem.GetCharacter(warriorId);
    EXPECT_EQ(warrior->Strength, 15); // 10 base + 5 bônus
    EXPECT_EQ(warrior->Vitality, 15); // 10 base + 5 bônus
    
    // Verificar bônus de mago
    const Mob* mage = characterSystem.GetCharacter(mageId);
    EXPECT_EQ(mage->Intelligence, 15); // 10 base + 5 bônus
    EXPECT_EQ(mage->Mana, 100); // 50 base + 50 bônus
    EXPECT_EQ(mage->MaxMana, 100);
    
    // Verificar bônus de arqueiro
    const Mob* archer = characterSystem.GetCharacter(archerId);
    EXPECT_EQ(archer->Dexterity, 15); // 10 base + 5 bônus
    EXPECT_EQ(archer->Agility, 15); // 10 base + 5 bônus
    
    // Verificar bônus de sacerdote
    const Mob* priest = characterSystem.GetCharacter(priestId);
    EXPECT_EQ(priest->Intelligence, 13); // 10 base + 3 bônus
    EXPECT_EQ(priest->Vitality, 13); // 10 base + 3 bônus
    EXPECT_EQ(priest->Mana, 75); // 50 base + 25 bônus
    EXPECT_EQ(priest->MaxMana, 75);
    
    // Verificar bônus de ladino
    const Mob* rogue = characterSystem.GetCharacter(rogueId);
    EXPECT_EQ(rogue->Dexterity, 15); // 10 base + 5 bônus
    EXPECT_EQ(rogue->Agility, 15); // 10 base + 5 bônus
}

TEST_F(CharacterSystemTest, InvalidOperations) {
    auto& characterSystem = CharacterSystem::GetInstance();
    
    // Tentar obter personagem inexistente
    EXPECT_EQ(characterSystem.GetCharacter(999), nullptr);
    
    // Tentar atualizar personagem inexistente
    Mob invalidCharacter;
    invalidCharacter.id = 999;
    EXPECT_FALSE(characterSystem.UpdateCharacter(invalidCharacter));
    
    // Tentar atualizar vida de personagem inexistente
    EXPECT_FALSE(characterSystem.UpdateHealth(999, 100));
    
    // Tentar atualizar mana de personagem inexistente
    EXPECT_FALSE(characterSystem.UpdateMana(999, 100));
    
    // Tentar atualizar experiência de personagem inexistente
    EXPECT_FALSE(characterSystem.UpdateExperience(999, 100));
    
    // Tentar deletar personagem inexistente
    EXPECT_FALSE(characterSystem.DeleteCharacter(999));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 