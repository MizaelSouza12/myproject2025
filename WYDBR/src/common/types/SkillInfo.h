/**
 * @file SkillInfo.h
 * @brief Define a estrutura para informações de habilidades usadas em combate
 */

#ifndef WYDBR_SKILL_INFO_H_
#define WYDBR_SKILL_INFO_H_

#include <cstdint>
#include <string>
#include <vector>
#include "SkillAttributes.h"
#include "SkillEffect.h"

namespace wydbr {
namespace common {
namespace types {

/**
 * @struct SkillInfo
 * @brief Informações de habilidades usadas durante o combate
 */
struct SkillInfo {
    // Identificação
    uint32_t skill_id = 0;              ///< ID da habilidade
    std::string name;                   ///< Nome da habilidade
    
    // Propriedades básicas
    SkillAttributes attributes;         ///< Atributos base da habilidade
    bool is_magical = false;            ///< Se é uma habilidade mágica
    bool always_hit = false;            ///< Se sempre acerta (ignora check de precisão)
    bool can_critical = true;           ///< Se pode causar crítico
    bool ignore_defense = false;        ///< Se ignora defesa do alvo
    bool ignore_resistance = false;     ///< Se ignora resistência do alvo
    
    // Modificadores de combate
    float accuracy_modifier = 0.0f;     ///< Modificador de precisão
    float critical_chance_modifier = 0.0f; ///< Modificador de chance crítica
    float critical_damage_modifier = 0.0f; ///< Modificador de dano crítico
    float resistance_penetration = 0.0f; ///< Penetração de resistência
    float physical_scaling = 1.0f;      ///< Escala de dano físico (para habilidades físicas)
    
    // Efeitos adicionais
    std::vector<SkillEffect> effects;   ///< Efeitos secundários da habilidade
    float effect_chance = 1.0f;         ///< Chance de aplicar os efeitos (0.0 a 1.0)
    
    // Controle de alvo
    uint8_t target_limit = 1;           ///< Limite de alvos (1 para single-target)
    bool require_target = true;         ///< Se requer um alvo específico
    bool can_target_self = false;       ///< Se pode direcionar a si mesmo
    
    /**
     * @brief Verifica se a habilidade tem efeitos secundários
     * @return true se tem efeitos secundários
     */
    bool HasSecondaryEffects() const {
        return !effects.empty();
    }
    
    /**
     * @brief Verifica se a habilidade é de área
     * @return true se for de área
     */
    bool IsAreaOfEffect() const {
        return target_limit > 1 || attributes.aoe_radius > 0.0f;
    }
    
    /**
     * @brief Verifica se a habilidade é de suporte/buff
     * @return true se for de suporte
     */
    bool IsSupport() const {
        return attributes.damage_modifier <= 0.0f && attributes.healing_modifier > 0.0f;
    }
    
    /**
     * @brief Verifica se a habilidade é de dano
     * @return true se for de dano
     */
    bool IsDamage() const {
        return attributes.damage_modifier > 0.0f;
    }
    
    /**
     * @brief Verifica se a habilidade tem cooldown
     * @return true se tiver cooldown
     */
    bool HasCooldown() const {
        return attributes.cooldown.count() > 0;
    }
    
    /**
     * @brief Verifica se a habilidade tem cast time
     * @return true se tiver cast time
     */
    bool HasCastTime() const {
        return attributes.cast_time.count() > 0;
    }
};

/**
 * @struct SkillId
 * @brief Identificador de habilidade
 */
struct SkillId {
    uint32_t id;
    
    SkillId() : id(0) {}
    explicit SkillId(uint32_t skillId) : id(skillId) {}
    
    operator uint32_t() const {
        return id;
    }
    
    bool operator==(const SkillId& other) const {
        return id == other.id;
    }
    
    bool operator!=(const SkillId& other) const {
        return id != other.id;
    }
};

} // namespace types
} // namespace common
} // namespace wydbr

#endif // WYDBR_SKILL_INFO_H_ 