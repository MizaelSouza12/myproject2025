/**
 * @file CombatFormulas.h
 * @brief Define as fórmulas matemáticas utilizadas no sistema de combate
 * 
 * Este arquivo contém todas as fórmulas usadas para calcular dano,
 * evasão, precisão, crítico e outros aspectos do sistema de combate.
 */

#ifndef WYDBR_COMBAT_FORMULAS_H_
#define WYDBR_COMBAT_FORMULAS_H_

#include <cmath>
#include <random>
#include <memory>
#include "../../common/types/CharacterStats.h"
#include "../../common/types/ItemAttributes.h"
#include "../../common/types/SkillAttributes.h"
#include "../../common/utils/MathUtils.h"

namespace wydbr {
namespace tmsrv {
namespace combat {

/**
 * @class CombatFormulas
 * @brief Implementa as fórmulas de cálculo para o sistema de combate
 */
class CombatFormulas {
public:
    /**
     * @brief Construtor padrão
     */
    CombatFormulas();

    /**
     * @brief Destrutor virtual
     */
    virtual ~CombatFormulas();

    /**
     * @brief Calcula o dano base de um ataque físico
     * 
     * @param attacker Estatísticas do atacante
     * @param weapon Atributos da arma
     * @return O dano base calculado
     */
    int32_t CalculatePhysicalDamage(
        const common::types::CharacterStats& attacker,
        const common::types::ItemAttributes& weapon) const;

    /**
     * @brief Calcula o dano base de um ataque mágico
     * 
     * @param attacker Estatísticas do atacante
     * @param skill Atributos da habilidade
     * @return O dano base calculado
     */
    int32_t CalculateMagicalDamage(
        const common::types::CharacterStats& attacker,
        const common::types::SkillAttributes& skill) const;

    /**
     * @brief Calcula a chance de acerto do ataque
     * 
     * @param attackerDex Destreza do atacante
     * @param defenderDex Destreza do defensor
     * @param weaponPrecision Precisão da arma
     * @return A probabilidade de acerto (0.0 a 1.0)
     */
    float CalculateHitChance(
        int32_t attackerDex,
        int32_t defenderDex,
        float weaponPrecision) const;
        
    /**
     * @brief Calcula a chance de acerto crítico
     * 
     * @param attackerDex Destreza do atacante
     * @param weaponCritical Bônus de crítico da arma
     * @param skillBonus Bônus de crítico da habilidade
     * @return A probabilidade de crítico (0.0 a 1.0)
     */
    float CalculateCriticalChance(
        int32_t attackerDex,
        float weaponCritical,
        float skillBonus) const;
        
    /**
     * @brief Calcula o multiplicador de dano crítico
     * 
     * @param attackerStr Força do atacante
     * @param weaponMultiplier Multiplicador de crítico da arma
     * @param skillMultiplier Multiplicador de crítico da habilidade
     * @return O multiplicador de dano para críticos
     */
    float CalculateCriticalMultiplier(
        int32_t attackerStr,
        float weaponMultiplier,
        float skillMultiplier) const;
        
    /**
     * @brief Calcula a redução de dano pela defesa
     * 
     * @param damage Dano base calculado
     * @param defense Defesa do alvo
     * @param armorPenetration Penetração de armadura do ataque
     * @return O dano reduzido após aplicação da defesa
     */
    int32_t ApplyDefenseReduction(
        int32_t damage, 
        int32_t defense,
        float armorPenetration) const;
        
    /**
     * @brief Calcula a redução de dano mágico pela resistência
     * 
     * @param damage Dano mágico base calculado
     * @param resistance Resistência mágica do alvo
     * @param magicPenetration Penetração mágica do ataque
     * @param elementType Tipo de elemento do ataque
     * @param targetResistances Resistências elementais do alvo
     * @return O dano reduzido após aplicação das resistências
     */
    int32_t ApplyResistanceReduction(
        int32_t damage,
        int32_t resistance,
        float magicPenetration,
        uint8_t elementType,
        const std::array<float, 8>& targetResistances) const;

    /**
     * @brief Aplica variação aleatória ao dano
     * 
     * @param damage Dano base calculado
     * @param variationPercent Porcentagem de variação (padrão: 10%)
     * @return O dano com variação aleatória aplicada
     */
    int32_t ApplyRandomVariation(
        int32_t damage,
        float variationPercent = 0.1f) const;

    /**
     * @brief Calcula o dano refletido por efeitos de reflexão
     * 
     * @param damage Dano original recebido
     * @param reflectPercent Porcentagem de reflexão
     * @return A quantidade de dano refletido
     */
    int32_t CalculateReflectedDamage(
        int32_t damage,
        float reflectPercent) const;

private:
    std::mt19937 rng_;
    std::uniform_real_distribution<float> distribution_;
};

} // namespace combat
} // namespace tmsrv
} // namespace wydbr

#endif // WYDBR_COMBAT_FORMULAS_H_ 