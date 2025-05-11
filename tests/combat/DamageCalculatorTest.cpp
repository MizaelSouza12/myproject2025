#include <gtest/gtest.h>
#include "GameLogic/combat/DamageCalculator.h"
#include "GameLogic/character/CharacterSystem.h"

using namespace WYDBR;

class DamageCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Configurar personagens de teste
        Mob attacker;
        attacker.Level = 50;
        attacker.Attack = 100;
        attacker.Defense = 50;
        attacker.Strength = 100;
        attacker.Dexterity = 80;
        attacker.Vitality = 70;
        attacker.Element = ELEMENT_FIRE;
        attacker.EquipBonus = 20;
        attacker.Status = 0;
        attacker.DamageReduction = 10;
        
        Mob target;
        target.Level = 45;
        target.Attack = 80;
        target.Defense = 60;
        target.Strength = 80;
        target.Dexterity = 60;
        target.Vitality = 90;
        target.Element = ELEMENT_ICE;
        target.Weakness = ELEMENT_FIRE;
        target.Resistance = ELEMENT_WATER;
        target.DamageReduction = 15;
        
        // Registrar personagens
        CharacterSystem::GetInstance().Initialize();
        CharacterSystem::GetInstance().CreateCharacter("Attacker", CHARACTER_WARRIOR);
        CharacterSystem::GetInstance().CreateCharacter("Target", CHARACTER_WARRIOR);
    }
    
    void TearDown() override {
        CharacterSystem::GetInstance().Shutdown();
    }
};

TEST_F(DamageCalculatorTest, BasicDamageCalculation) {
    DamageCalculator calculator;
    
    uint32_t damage = calculator.CalculateDamage(1, 2, AttackType::NORMAL);
    EXPECT_GT(damage, 0);
}

TEST_F(DamageCalculatorTest, SkillDamageCalculation) {
    DamageCalculator calculator;
    
    uint32_t damage = calculator.CalculateSkillDamage(1, 2, 1);
    EXPECT_GT(damage, 0);
}

TEST_F(DamageCalculatorTest, AreaDamageCalculation) {
    DamageCalculator calculator;
    
    uint32_t damage = calculator.CalculateAreaDamage(1, 1, 5.0f);
    EXPECT_GT(damage, 0);
}

TEST_F(DamageCalculatorTest, ElementalBonus) {
    DamageCalculator calculator;
    
    // Teste com elemento fraco
    uint32_t damageWithWeakness = calculator.CalculateDamage(1, 2, AttackType::NORMAL);
    
    // Alterar elemento do alvo
    Mob* target = CharacterSystem::GetInstance().GetCharacter(2);
    target->Weakness = ELEMENT_WATER;
    
    uint32_t damageWithoutWeakness = calculator.CalculateDamage(1, 2, AttackType::NORMAL);
    
    EXPECT_GT(damageWithWeakness, damageWithoutWeakness);
}

TEST_F(DamageCalculatorTest, CriticalDamage) {
    DamageCalculator calculator;
    
    uint32_t normalDamage = calculator.CalculateDamage(1, 2, AttackType::NORMAL);
    uint32_t criticalDamage = calculator.CalculateDamage(1, 2, AttackType::CRITICAL);
    
    EXPECT_GT(criticalDamage, normalDamage);
}

TEST_F(DamageCalculatorTest, DistancePenalty) {
    DamageCalculator calculator;
    
    uint32_t closeDamage = calculator.CalculateAreaDamage(1, 1, 1.0f);
    uint32_t farDamage = calculator.CalculateAreaDamage(1, 1, 10.0f);
    
    EXPECT_GT(closeDamage, farDamage);
}

TEST_F(DamageCalculatorTest, StatusEffects) {
    DamageCalculator calculator;
    
    // Teste sem status
    uint32_t normalDamage = calculator.CalculateDamage(1, 2, AttackType::NORMAL);
    
    // Aplicar status BERSERK
    Mob* attacker = CharacterSystem::GetInstance().GetCharacter(1);
    attacker->Status |= STATUS_BERSERK;
    
    uint32_t berserkDamage = calculator.CalculateDamage(1, 2, AttackType::NORMAL);
    
    EXPECT_GT(berserkDamage, normalDamage);
}

TEST_F(DamageCalculatorTest, DefenseCalculation) {
    DamageCalculator calculator;
    
    // Teste com defesa normal
    uint32_t normalDamage = calculator.CalculateDamage(1, 2, AttackType::NORMAL);
    
    // Aumentar defesa
    Mob* target = CharacterSystem::GetInstance().GetCharacter(2);
    target->Defense *= 2;
    
    uint32_t highDefenseDamage = calculator.CalculateDamage(1, 2, AttackType::NORMAL);
    
    EXPECT_GT(normalDamage, highDefenseDamage);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 