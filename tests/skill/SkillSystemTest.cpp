#include <gtest/gtest.h>
#include "GameLogic/skill/SkillSystem.h"
#include "GameLogic/character/CharacterSystem.h"

using namespace WYDBR;

class SkillSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        CharacterSystem::GetInstance().Initialize();
        SkillSystem::GetInstance().Initialize();
        
        // Criar personagem de teste
        characterId = CharacterSystem::GetInstance().CreateCharacter("Test", CHARACTER_WARRIOR);
        EXPECT_GT(characterId, 0);
        
        // Registrar habilidade de teste
        Skill skill;
        skill.id = 1;
        skill.name = "Test Skill";
        skill.description = "A test skill";
        skill.requiredClass = CHARACTER_WARRIOR;
        skill.requiredLevel = 1;
        skill.mpCost = 10;
        skill.cooldown = 5.0f;
        skill.castTime = 1.0f;
        skill.range = 10.0f;
        
        SkillEffect effect;
        effect.type = EFFECT_DAMAGE;
        effect.value = 50.0f;
        effect.duration = 0.0f;
        effect.radius = 0.0f;
        effect.isAreaEffect = false;
        skill.effects.push_back(effect);
        
        SkillSystem::GetInstance().RegisterSkill(skill);
    }
    
    void TearDown() override {
        SkillSystem::GetInstance().Shutdown();
        CharacterSystem::GetInstance().Shutdown();
    }
    
    uint32_t characterId;
};

TEST_F(SkillSystemTest, RegisterAndGetSkill) {
    auto& skillSystem = SkillSystem::GetInstance();
    
    // Verificar habilidade registrada
    const Skill* skill = skillSystem.GetSkill(1);
    EXPECT_NE(skill, nullptr);
    EXPECT_EQ(skill->id, 1);
    EXPECT_EQ(skill->name, "Test Skill");
    EXPECT_EQ(skill->requiredClass, CHARACTER_WARRIOR);
    EXPECT_EQ(skill->requiredLevel, 1);
    EXPECT_EQ(skill->mpCost, 10);
    EXPECT_FLOAT_EQ(skill->cooldown, 5.0f);
    EXPECT_FLOAT_EQ(skill->castTime, 1.0f);
    EXPECT_FLOAT_EQ(skill->range, 10.0f);
    EXPECT_EQ(skill->effects.size(), 1);
    EXPECT_EQ(skill->effects[0].type, EFFECT_DAMAGE);
    EXPECT_FLOAT_EQ(skill->effects[0].value, 50.0f);
}

TEST_F(SkillSystemTest, LearnSkill) {
    auto& skillSystem = SkillSystem::GetInstance();
    
    // Verificar se pode aprender
    EXPECT_TRUE(skillSystem.CanLearnSkill(characterId, 1));
    
    // Aprender habilidade
    EXPECT_TRUE(skillSystem.LearnSkill(characterId, 1));
    
    // Verificar se já aprendeu
    EXPECT_FALSE(skillSystem.CanLearnSkill(characterId, 1));
    
    // Verificar habilidades aprendidas
    auto learnedSkills = skillSystem.GetLearnedSkills(characterId);
    EXPECT_EQ(learnedSkills.size(), 1);
    EXPECT_EQ(learnedSkills[0]->id, 1);
}

TEST_F(SkillSystemTest, UseSkill) {
    auto& skillSystem = SkillSystem::GetInstance();
    auto& characterSystem = CharacterSystem::GetInstance();
    
    // Aprender habilidade
    EXPECT_TRUE(skillSystem.LearnSkill(characterId, 1));
    
    // Configurar MP
    Mob* character = characterSystem.GetCharacter(characterId);
    character->MP = 100;
    
    // Verificar se pode usar
    EXPECT_TRUE(skillSystem.CanUseSkill(characterId, 1));
    
    // Usar habilidade
    EXPECT_TRUE(skillSystem.UseSkill(characterId, 1));
    
    // Verificar MP consumido
    EXPECT_EQ(character->MP, 90);
    
    // Verificar cooldown
    EXPECT_TRUE(skillSystem.IsSkillOnCooldown(characterId, 1));
    EXPECT_FLOAT_EQ(skillSystem.GetSkillCooldownRemaining(characterId, 1), 5.0f);
    
    // Verificar se não pode usar em cooldown
    EXPECT_FALSE(skillSystem.CanUseSkill(characterId, 1));
}

TEST_F(SkillSystemTest, UpdateCooldown) {
    auto& skillSystem = SkillSystem::GetInstance();
    
    // Aprender e usar habilidade
    EXPECT_TRUE(skillSystem.LearnSkill(characterId, 1));
    EXPECT_TRUE(skillSystem.UseSkill(characterId, 1));
    
    // Atualizar cooldown
    skillSystem.Update(2.5f);
    
    // Verificar tempo restante
    EXPECT_TRUE(skillSystem.IsSkillOnCooldown(characterId, 1));
    EXPECT_FLOAT_EQ(skillSystem.GetSkillCooldownRemaining(characterId, 1), 2.5f);
    
    // Atualizar até o fim do cooldown
    skillSystem.Update(2.5f);
    
    // Verificar se cooldown acabou
    EXPECT_FALSE(skillSystem.IsSkillOnCooldown(characterId, 1));
    EXPECT_FLOAT_EQ(skillSystem.GetSkillCooldownRemaining(characterId, 1), 0.0f);
}

TEST_F(SkillSystemTest, AvailableSkills) {
    auto& skillSystem = SkillSystem::GetInstance();
    
    // Registrar habilidade para outra classe
    Skill skill;
    skill.id = 2;
    skill.name = "Mage Skill";
    skill.requiredClass = CHARACTER_MAGE;
    skill.requiredLevel = 1;
    skillSystem.RegisterSkill(skill);
    
    // Verificar habilidades disponíveis para guerreiro
    auto warriorSkills = skillSystem.GetAvailableSkills(CHARACTER_WARRIOR);
    EXPECT_EQ(warriorSkills.size(), 1);
    EXPECT_EQ(warriorSkills[0]->id, 1);
    
    // Verificar habilidades disponíveis para mago
    auto mageSkills = skillSystem.GetAvailableSkills(CHARACTER_MAGE);
    EXPECT_EQ(mageSkills.size(), 1);
    EXPECT_EQ(mageSkills[0]->id, 2);
}

TEST_F(SkillSystemTest, InvalidSkill) {
    auto& skillSystem = SkillSystem::GetInstance();
    
    // Verificar habilidade inexistente
    EXPECT_EQ(skillSystem.GetSkill(999), nullptr);
    EXPECT_FALSE(skillSystem.CanLearnSkill(characterId, 999));
    EXPECT_FALSE(skillSystem.LearnSkill(characterId, 999));
    EXPECT_FALSE(skillSystem.CanUseSkill(characterId, 999));
    EXPECT_FALSE(skillSystem.UseSkill(characterId, 999));
    EXPECT_FALSE(skillSystem.IsSkillOnCooldown(characterId, 999));
    EXPECT_FLOAT_EQ(skillSystem.GetSkillCooldownRemaining(characterId, 999), 0.0f);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 