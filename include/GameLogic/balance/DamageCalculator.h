/**
 * @file DamageCalculator.h
 * @brief Sistema de cálculo de dano corrigido
 */

#ifndef WYDBR_DAMAGE_CALCULATOR_H
#define WYDBR_DAMAGE_CALCULATOR_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <mutex>
#include <random>

namespace WYDBR {
namespace GameLogic {

/**
 * Tipo de dano
 */
enum class DamageType {
    Physical,               // Dano físico
    Magical,                // Dano mágico
    True,                   // Dano verdadeiro (ignora resistências)
    Fire,                   // Dano de fogo
    Ice,                    // Dano de gelo
    Lightning,              // Dano de relâmpago
    Holy,                   // Dano sagrado
    Dark,                   // Dano sombrio
    Poison,                 // Dano de veneno
    Bleeding,               // Dano de sangramento
    Stamina                 // Dano de stamina
};

/**
 * Tipo de ataque
 */
enum class AttackType {
    Normal,                 // Ataque normal
    Skill,                  // Habilidade
    Spell,                  // Feitiço
    AreaOfEffect,           // Área de efeito
    DoT,                    // Dano ao longo do tempo
    Critical,               // Crítico
    CounterAttack           // Contra-ataque
};

/**
 * Resultado de dano
 */
struct DamageResult {
    int32_t rawDamage;           // Dano bruto calculado
    int32_t finalDamage;         // Dano final após modificadores
    bool isCritical;             // Foi um acerto crítico
    bool isEvaded;               // Foi esquivado
    bool isBlocked;              // Foi bloqueado
    bool isImmune;               // Alvo é imune
    bool isAbsorbed;             // Dano foi absorvido
    bool isReflected;            // Dano foi refletido
    DamageType damageType;       // Tipo de dano
    AttackType attackType;       // Tipo de ataque
    std::string effects;         // Efeitos adicionais
};

/**
 * Contexto de combate
 */
struct CombatContext {
    // Atacante
    int32_t attackerLevel;               // Nível do atacante
    int32_t attackerStrength;            // Força do atacante
    int32_t attackerIntelligence;        // Inteligência do atacante
    int32_t attackerDexterity;           // Destreza do atacante
    int32_t attackerCriticalRate;        // Taxa crítica do atacante
    int32_t attackerCriticalDamage;      // Dano crítico do atacante
    int32_t attackerPenetration;         // Penetração de armadura do atacante
    int32_t attackerWeaponDamage;        // Dano da arma do atacante
    int32_t attackerSkillLevel;          // Nível da habilidade do atacante
    
    // Defensor
    int32_t defenderLevel;               // Nível do defensor
    int32_t defenderVitality;            // Vitalidade do defensor
    int32_t defenderArmor;               // Armadura do defensor
    int32_t defenderResistance;          // Resistência mágica do defensor
    int32_t defenderEvasion;             // Evasão do defensor
    int32_t defenderBlock;               // Bloqueio do defensor
    std::unordered_map<DamageType, int32_t> defenderResistances; // Resistências específicas
    
    // Ambiente
    int32_t environmentFactor;           // Fator ambiental (%)
    int32_t randomnessFactor;            // Fator de aleatoriedade (%)
    
    // Skill/Spell
    int32_t skillPower;                  // Poder da habilidade/feitiço
    DamageType damageType;               // Tipo de dano
    AttackType attackType;               // Tipo de ataque
};

/**
 * Calculador de dano
 */
class DamageCalculator {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o calculador
     */
    static DamageCalculator& GetInstance();
    
    /**
     * Calcula o dano baseado no contexto
     * @param context Contexto de combate
     * @return Resultado do dano
     */
    DamageResult CalculateDamage(const CombatContext& context);
    
    /**
     * Calcula dano físico
     * @param context Contexto de combate
     * @return Resultado do dano
     */
    DamageResult CalculatePhysicalDamage(const CombatContext& context);
    
    /**
     * Calcula dano mágico
     * @param context Contexto de combate
     * @return Resultado do dano
     */
    DamageResult CalculateMagicalDamage(const CombatContext& context);
    
    /**
     * Calcula dano verdadeiro
     * @param context Contexto de combate
     * @return Resultado do dano
     */
    DamageResult CalculateTrueDamage(const CombatContext& context);
    
    /**
     * Calcula dano de habilidade
     * @param context Contexto de combate
     * @return Resultado do dano
     */
    DamageResult CalculateSkillDamage(const CombatContext& context);
    
    /**
     * Calcula se um ataque é crítico
     * @param criticalRate Taxa crítica
     * @param targetLevel Nível do alvo
     * @return true se for crítico
     */
    bool IsCriticalHit(int32_t criticalRate, int32_t targetLevel);
    
    /**
     * Calcula se um ataque é esquivado
     * @param evasion Evasão do defensor
     * @param dexterity Destreza do atacante
     * @return true se for esquivado
     */
    bool IsEvaded(int32_t evasion, int32_t dexterity);
    
    /**
     * Calcula se um ataque é bloqueado
     * @param block Bloqueio do defensor
     * @param strength Força do atacante
     * @return true se for bloqueado
     */
    bool IsBlocked(int32_t block, int32_t strength);
    
    /**
     * Calcula redução de dano por resistência
     * @param damage Dano base
     * @param resistance Resistência
     * @param penetration Penetração
     * @return Dano reduzido
     */
    int32_t ApplyResistance(int32_t damage, int32_t resistance, int32_t penetration);
    
    /**
     * Adiciona aleatoriedade ao dano
     * @param damage Dano base
     * @param randomnessFactor Fator de aleatoriedade (%)
     * @return Dano com aleatoriedade
     */
    int32_t ApplyRandomness(int32_t damage, int32_t randomnessFactor);
    
    /**
     * Limita o dano a um valor mínimo e máximo
     * @param damage Dano a limitar
     * @param minDamage Dano mínimo
     * @param maxDamage Dano máximo
     * @return Dano limitado
     */
    int32_t ClampDamage(int32_t damage, int32_t minDamage, int32_t maxDamage);
    
private:
    // Construtor e destrutor privados (singleton)
    DamageCalculator();
    ~DamageCalculator();
    
    // Não permitir cópias
    DamageCalculator(const DamageCalculator&) = delete;
    DamageCalculator& operator=(const DamageCalculator&) = delete;
    
    // Gerador de números aleatórios
    std::mt19937 m_rng;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Fatores de balanceamento
    struct {
        float physicalDamageFactor;
        float magicalDamageFactor;
        float criticalDamageFactor;
        float skillDamageFactor;
        float levelDifferenceScaling;
        float armorEffectiveness;
        float resistanceEffectiveness;
        float minDamagePercent;
        float maxDamagePercent;
    } m_balanceFactors;
    
    // Métodos privados
    void InitializeBalanceFactors();
    int32_t CalculateBaseDamage(int32_t attackStat, int32_t weaponDamage, int32_t skillPower);
    int32_t ApplyLevelDifference(int32_t damage, int32_t attackerLevel, int32_t defenderLevel);
    int32_t ApplyCriticalDamage(int32_t damage, int32_t criticalDamage);
};

} // namespace GameLogic
} // namespace WYDBR

#endif // WYDBR_DAMAGE_CALCULATOR_H
