#include <gtest/gtest.h>
#include "GameLogic/combat/CombatSystem.h"
#include "GameLogic/character/CharacterSystem.h"
#include "GameLogic/skill/SkillSystem.h"

using namespace WYDBR;

class CombatSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        CharacterSystem::GetInstance().Initialize();
        SkillSystem::GetInstance().Initialize();
        CombatSystem::GetInstance().Initialize();
        
        // Criar personagens de teste
        warriorId = CharacterSystem::GetInstance().CreateCharacter("Warrior", CHARACTER_WARRIOR);
        mageId = CharacterSystem::GetInstance().CreateCharacter("Mage", CHARACTER_MAGE);
        archerId = CharacterSystem::GetInstance().CreateCharacter("Archer", CHARACTER_ARCHER);
        
        EXPECT_GT(warriorId, 0);
        EXPECT_GT(mageId, 0);
        EXPECT_GT(archerId, 0);
        
        // Configurar posições
        Mob* warrior = CharacterSystem::GetInstance().GetCharacter(warriorId);
        Mob* mage = CharacterSystem::GetInstance().GetCharacter(mageId);
        Mob* archer = CharacterSystem::GetInstance().GetCharacter(archerId);
        warrior->Position = {0.0f, 0.0f, 0.0f};
        mage->Position = {1.0f, 0.0f, 0.0f};
        archer->Position = {2.0f, 0.0f, 0.0f};
        
        // Configurar estatísticas de combate
        CombatStats warriorStats;
        warriorStats.minDamage = 10;
        warriorStats.maxDamage = 20;
        warriorStats.defense = 5;
        warriorStats.attackRate = 90;
        warriorStats.blockRate = 10;
        warriorStats.criticalRate = 20;
        warriorStats.criticalDamage = 150;
        warriorStats.skillDamage = 30;
        warriorStats.skillDefense = 5;
        
        CombatStats mageStats;
        mageStats.minDamage = 5;
        mageStats.maxDamage = 15;
        mageStats.defense = 8;
        mageStats.attackRate = 80;
        mageStats.blockRate = 20;
        mageStats.criticalRate = 10;
        mageStats.criticalDamage = 150;
        mageStats.skillDamage = 20;
        mageStats.skillDefense = 8;
        
        CombatSystem::GetInstance().UpdateCombatStats(warriorId, warriorStats);
        CombatSystem::GetInstance().UpdateCombatStats(mageId, mageStats);
        
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
        CombatSystem::GetInstance().Shutdown();
        SkillSystem::GetInstance().Shutdown();
        CharacterSystem::GetInstance().Shutdown();
    }
    
    uint32_t warriorId;
    uint32_t mageId;
    uint32_t archerId;
};

TEST_F(CombatSystemTest, BasicAttack) {
    auto& combatSystem = CombatSystem::GetInstance();
    
    // Guerreiro ataca mago
    CombatResult result = combatSystem.PerformBasicAttack(warriorId, mageId);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.affectedTargets.size(), 1);
    EXPECT_EQ(result.affectedTargets[0], mageId);
    EXPECT_EQ(result.damage.type, DAMAGE_TYPE_PHYSICAL);
    EXPECT_GT(result.damage.amount, 0);
    
    // Verificar dano aplicado
    const Mob* mage = CharacterSystem::GetInstance().GetCharacter(mageId);
    EXPECT_LT(mage->Health, mage->MaxHealth);
}

TEST_F(CombatSystemTest, RangedAttack) {
    auto& combatSystem = CombatSystem::GetInstance();
    
    // Arqueiro ataca guerreiro
    CombatResult result = combatSystem.PerformRangedAttack(archerId, warriorId);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.affectedTargets.size(), 1);
    EXPECT_EQ(result.affectedTargets[0], warriorId);
    EXPECT_EQ(result.damage.type, DAMAGE_TYPE_PHYSICAL);
    EXPECT_GT(result.damage.amount, 0);
    
    // Verificar dano aplicado
    const Mob* warrior = CharacterSystem::GetInstance().GetCharacter(warriorId);
    EXPECT_LT(warrior->Health, warrior->MaxHealth);
}

TEST_F(CombatSystemTest, SkillUsage) {
    auto& combatSystem = CombatSystem::GetInstance();
    
    // Mago usa habilidade em múltiplos alvos
    std::vector<uint32_t> targets = {warriorId, archerId};
    CombatResult result = combatSystem.UseSkill(mageId, 1, targets);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.affectedTargets.size(), 2);
    EXPECT_EQ(result.damage.type, DAMAGE_TYPE_MAGICAL);
    EXPECT_GT(result.damage.amount, 0);
    
    // Verificar dano aplicado
    const Mob* warrior = CharacterSystem::GetInstance().GetCharacter(warriorId);
    const Mob* archer = CharacterSystem::GetInstance().GetCharacter(archerId);
    EXPECT_LT(warrior->Health, warrior->MaxHealth);
    EXPECT_LT(archer->Health, archer->MaxHealth);
}

TEST_F(CombatSystemTest, CriticalHit) {
    auto& combatSystem = CombatSystem::GetInstance();
    
    // Configurar personagem com alta chance de crítico
    Mob* warrior = CharacterSystem::GetInstance().GetCharacter(warriorId);
    warrior->Dexterity = 100; // Aumenta chance de crítico
    
    // Realizar vários ataques para verificar crítico
    bool hasCritical = false;
    for (int i = 0; i < 10; i++) {
        CombatResult result = combatSystem.PerformBasicAttack(warriorId, mageId);
        if (result.damage.isCritical) {
            hasCritical = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasCritical);
}

TEST_F(CombatSystemTest, Dodge) {
    auto& combatSystem = CombatSystem::GetInstance();
    
    // Configurar personagem com alta chance de esquiva
    Mob* mage = CharacterSystem::GetInstance().GetCharacter(mageId);
    mage->Agility = 100; // Aumenta chance de esquiva
    
    // Realizar vários ataques para verificar esquiva
    bool hasDodged = false;
    for (int i = 0; i < 10; i++) {
        CombatResult result = combatSystem.PerformBasicAttack(warriorId, mageId);
        if (!result.success) {
            hasDodged = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasDodged);
}

TEST_F(CombatSystemTest, Block) {
    auto& combatSystem = CombatSystem::GetInstance();
    
    // Configurar personagem com alta chance de bloqueio
    Mob* warrior = CharacterSystem::GetInstance().GetCharacter(warriorId);
    warrior->Strength = 100; // Aumenta chance de bloqueio
    
    // Realizar vários ataques para verificar bloqueio
    bool hasBlocked = false;
    for (int i = 0; i < 10; i++) {
        CombatResult result = combatSystem.PerformBasicAttack(mageId, warriorId);
        if (result.damage.amount < combatSystem.CalculatePhysicalDamage(mageId, warriorId)) {
            hasBlocked = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasBlocked);
}

TEST_F(CombatSystemTest, DamageCalculation) {
    auto& combatSystem = CombatSystem::GetInstance();
    
    // Configurar personagens
    Mob* warrior = CharacterSystem::GetInstance().GetCharacter(warriorId);
    Mob* mage = CharacterSystem::GetInstance().GetCharacter(mageId);
    
    warrior->Strength = 50;
    mage->Vitality = 20;
    
    // Calcular dano físico
    uint32_t physicalDamage = combatSystem.CalculatePhysicalDamage(warriorId, mageId);
    EXPECT_EQ(physicalDamage, 80); // (50 * 2) - 20
    
    // Calcular dano mágico
    warrior->Intelligence = 50;
    mage->Intelligence = 20;
    
    uint32_t magicalDamage = combatSystem.CalculateMagicalDamage(warriorId, mageId);
    EXPECT_EQ(magicalDamage, 80); // (50 * 2) - 20
    
    // Calcular dano puro
    uint32_t pureDamage = combatSystem.CalculatePureDamage(100);
    EXPECT_EQ(pureDamage, 100);
}

TEST_F(CombatSystemTest, DefenseCalculation) {
    auto& combatSystem = CombatSystem::GetInstance();
    
    // Configurar personagem
    Mob* warrior = CharacterSystem::GetInstance().GetCharacter(warriorId);
    warrior->Vitality = 50;
    warrior->Intelligence = 30;
    
    // Verificar defesa física
    uint32_t physicalDefense = combatSystem.CalculatePhysicalDefense(warriorId);
    EXPECT_EQ(physicalDefense, 50);
    
    // Verificar defesa mágica
    uint32_t magicalDefense = combatSystem.CalculateMagicalDefense(warriorId);
    EXPECT_EQ(magicalDefense, 30);
}

TEST_F(CombatSystemTest, InvalidOperations) {
    auto& combatSystem = CombatSystem::GetInstance();
    
    // Tentar atacar personagem inexistente
    CombatResult result = combatSystem.PerformBasicAttack(warriorId, 999);
    EXPECT_FALSE(result.success);
    
    // Tentar usar habilidade sem mana
    Mob* mage = CharacterSystem::GetInstance().GetCharacter(mageId);
    mage->Mana = 0;
    result = combatSystem.UseSkill(mageId, 1, {warriorId});
    EXPECT_FALSE(result.success);
    
    // Tentar atacar personagem morto
    mage->Health = 0;
    result = combatSystem.PerformBasicAttack(warriorId, mageId);
    EXPECT_FALSE(result.success);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 