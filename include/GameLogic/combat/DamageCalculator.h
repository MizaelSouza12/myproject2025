/**
 * @file DamageCalculator.h
 * @brief Calculador de dano do WYDBR 2.0
 */

#ifndef WYDBR_DAMAGE_CALCULATOR_H
#define WYDBR_DAMAGE_CALCULATOR_H

#include "wyd_core.h"

namespace WYDBR {

/**
 * @class DamageCalculator
 * @brief Calculador de dano do WYDBR 2.0
 * 
 * Este sistema calcula o dano causado em combate, incluindo:
 * - Dano básico
 * - Dano de habilidades
 * - Dano em área
 * - Bônus elementais
 * - Bônus de status
 * - Redução de dano
 */
class DamageCalculator {
public:
    /**
     * @brief Calcula o dano de um ataque
     * @param attackerId ID do atacante
     * @param targetId ID do alvo
     * @param type Tipo do ataque
     * @return Quantidade de dano
     */
    uint32_t CalculateDamage(uint32_t attackerId, uint32_t targetId, AttackType type);
    
    /**
     * @brief Calcula o dano de uma habilidade
     * @param casterId ID do conjurador
     * @param targetId ID do alvo
     * @param skillId ID da habilidade
     * @return Quantidade de dano
     */
    uint32_t CalculateSkillDamage(uint32_t casterId, uint32_t targetId, uint16_t skillId);
    
    /**
     * @brief Calcula o dano de um efeito em área
     * @param casterId ID do conjurador
     * @param skillId ID da habilidade
     * @param distance Distância do centro do efeito
     * @return Quantidade de dano
     */
    uint32_t CalculateAreaDamage(uint32_t casterId, uint16_t skillId, float distance);

private:
    /**
     * @brief Calcula o poder de ataque
     * @param attackerId ID do atacante
     * @return Poder de ataque
     */
    float CalculateAttackPower(uint32_t attackerId) const;
    
    /**
     * @brief Calcula a defesa
     * @param targetId ID do alvo
     * @return Valor de defesa
     */
    float CalculateDefense(uint32_t targetId) const;
    
    /**
     * @brief Calcula o bônus elemental
     * @param attackerId ID do atacante
     * @param targetId ID do alvo
     * @return Bônus elemental
     */
    float CalculateElementalBonus(uint32_t attackerId, uint32_t targetId) const;
    
    /**
     * @brief Calcula o multiplicador de dano crítico
     * @param attackerId ID do atacante
     * @return Multiplicador de dano crítico
     */
    float CalculateCriticalMultiplier(uint32_t attackerId) const;
    
    /**
     * @brief Calcula a penalidade de distância
     * @param distance Distância
     * @return Penalidade de distância
     */
    float CalculateDistancePenalty(float distance) const;
};

} // namespace WYDBR

#endif // WYDBR_DAMAGE_CALCULATOR_H 