/**
 * @file SkillAttributes.h
 * @brief Define a estrutura de atributos para habilidades
 */

#ifndef WYDBR_SKILL_ATTRIBUTES_H_
#define WYDBR_SKILL_ATTRIBUTES_H_

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

namespace wydbr {
namespace common {
namespace types {

/**
 * @struct SkillAttributes
 * @brief Atributos e propriedades de uma habilidade
 */
struct SkillAttributes {
    // Identificação da habilidade
    uint32_t skill_id = 0;           ///< ID da habilidade
    std::string name;                ///< Nome da habilidade
    std::string description;         ///< Descrição da habilidade
    uint8_t level = 1;               ///< Nível atual da habilidade
    uint8_t max_level = 10;          ///< Nível máximo da habilidade
    
    // Requisitos
    uint16_t character_level_req = 1; ///< Nível mínimo do personagem
    uint8_t class_req = 0xFF;        ///< Classe requerida (0xFF = qualquer classe)
    std::vector<uint32_t> prerequisite_skills; ///< IDs de habilidades requeridas
    
    // Propriedades básicas
    uint8_t skill_type = 0;          ///< Tipo de habilidade (0=ativa, 1=passiva, 2=buff, etc.)
    uint8_t target_type = 0;         ///< Tipo de alvo (0=self, 1=target, 2=aoe, etc.)
    uint8_t element_type = 0;        ///< Tipo elemental (0=neutro, 1=fogo, 2=água, etc.)
    float power_base = 0.0f;         ///< Poder base da habilidade
    float power_per_level = 0.0f;    ///< Poder adicional por nível
    
    // Custo e tempo
    int32_t mp_cost = 0;             ///< Custo de MP
    int32_t hp_cost = 0;             ///< Custo de HP (para algumas habilidades)
    int32_t stamina_cost = 0;        ///< Custo de stamina
    std::chrono::milliseconds cooldown{0}; ///< Tempo de recarga
    std::chrono::milliseconds cast_time{0}; ///< Tempo de conjuração
    std::chrono::milliseconds animation_time{0}; ///< Tempo da animação
    
    // Propriedades de área
    float range = 1.0f;              ///< Alcance da habilidade
    float aoe_radius = 0.0f;         ///< Raio de área de efeito
    float projectile_speed = 0.0f;   ///< Velocidade do projétil (se aplicável)
    
    // Modificadores
    float damage_modifier = 1.0f;    ///< Modificador de dano
    float healing_modifier = 0.0f;   ///< Modificador de cura
    float critical_modifier = 0.0f;  ///< Modificador de chance crítica
    float impact_modifier = 0.0f;    ///< Modificador de impacto (knockback, stun, etc.)
    
    // Escalamento com atributos
    float strength_scale = 0.0f;     ///< Escalamento com força
    float dexterity_scale = 0.0f;    ///< Escalamento com destreza
    float intelligence_scale = 0.0f; ///< Escalamento com inteligência
    float wisdom_scale = 0.0f;       ///< Escalamento com sabedoria
    
    // Informações visuais e de áudio
    uint32_t animation_id = 0;       ///< ID da animação
    uint32_t effect_id = 0;          ///< ID do efeito visual
    uint32_t sound_id = 0;           ///< ID do efeito sonoro
    uint32_t icon_id = 0;            ///< ID do ícone
    
    /**
     * @brief Calcula o poder total da habilidade considerando o nível
     * @return Poder total calculado
     */
    float CalculateTotalPower() const {
        return power_base + (power_per_level * (level - 1));
    }
    
    /**
     * @brief Calcula o custo de MP considerando o nível
     * @return Custo de MP calculado
     */
    int32_t CalculateMpCost() const {
        // Exemplo simples: custo aumenta 10% por nível
        return static_cast<int32_t>(mp_cost * (1.0f + (level - 1) * 0.1f));
    }
    
    /**
     * @brief Calcula o tempo de recarga considerando o nível
     * @return Tempo de recarga calculado
     */
    std::chrono::milliseconds CalculateCooldown() const {
        // Exemplo: redução de cooldown em níveis mais altos
        float cooldownReduction = std::min(0.4f, (level - 1) * 0.05f); // Máximo 40% de redução
        auto reducedCooldown = std::chrono::milliseconds(
            static_cast<int64_t>(cooldown.count() * (1.0f - cooldownReduction)));
        return reducedCooldown;
    }
    
    /**
     * @brief Verifica se os requisitos são atendidos
     * @param charLevel Nível do personagem
     * @param charClass Classe do personagem
     * @param learnedSkills Lista de IDs de habilidades já aprendidas
     * @return true se os requisitos são atendidos
     */
    bool MeetsRequirements(
        uint16_t charLevel, 
        uint8_t charClass,
        const std::vector<uint32_t>& learnedSkills) const {
        
        // Verificar nível e classe
        if (charLevel < character_level_req) {
            return false;
        }
        
        if (class_req != 0xFF && class_req != charClass) {
            return false;
        }
        
        // Verificar pré-requisitos de habilidades
        for (uint32_t prereqSkill : prerequisite_skills) {
            bool hasPrereq = false;
            for (uint32_t learnedSkill : learnedSkills) {
                if (prereqSkill == learnedSkill) {
                    hasPrereq = true;
                    break;
                }
            }
            
            if (!hasPrereq) {
                return false;
            }
        }
        
        return true;
    }
};

} // namespace types
} // namespace common
} // namespace wydbr

#endif // WYDBR_SKILL_ATTRIBUTES_H_ 