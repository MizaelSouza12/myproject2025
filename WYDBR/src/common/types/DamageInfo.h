/**
 * @file DamageInfo.h
 * @brief Define a estrutura para informações de dano
 */

#ifndef WYDBR_DAMAGE_INFO_H_
#define WYDBR_DAMAGE_INFO_H_

#include <cstdint>

namespace wydbr {
namespace common {
namespace types {

/**
 * @struct DamageInfo
 * @brief Estrutura que contém informações detalhadas sobre o dano
 */
struct DamageInfo {
    // Valores de dano
    int32_t base_damage = 0;        ///< Dano base antes de reduções
    int32_t total_damage = 0;       ///< Dano total final após reduções
    int32_t damage_reduction = 0;   ///< Quantidade de dano reduzido
    int32_t damage_absorbed = 0;    ///< Quantidade de dano absorvido por escudos/buffs
    int32_t damage_reflected = 0;   ///< Quantidade de dano refletido
    int32_t original_damage = 0;    ///< Dano original, para dano refletido
    
    // Flags de status
    bool is_critical = false;       ///< Se o ataque foi crítico
    bool is_miss = false;           ///< Se o ataque errou
    bool is_blocked = false;        ///< Se o ataque foi bloqueado
    bool is_evaded = false;         ///< Se o ataque foi esquivado
    bool is_reflected = false;      ///< Se o dano foi refletido
    bool is_dot = false;            ///< Se o dano é do tipo damage-over-time
    bool is_environmental = false;  ///< Se o dano é ambiental (armadilha, lava, etc)
    
    // Fonte do dano
    uint8_t damage_type = 0;        ///< Tipo de dano (0=físico, 1=fogo, 2=água, etc)
    uint32_t skill_id = 0;          ///< ID da habilidade, se o dano veio de uma habilidade
    uint32_t effect_id = 0;         ///< ID do efeito, se o dano veio de um efeito (DoT)
    
    /**
     * @brief Verifica se o dano efetivamente causou dano
     * @return true se causou dano
     */
    bool EffectivelyHit() const {
        return !is_miss && !is_evaded && total_damage > 0;
    }
    
    /**
     * @brief Calcula a porcentagem de redução de dano
     * @return Porcentagem de redução (0.0 a 1.0)
     */
    float GetReductionPercent() const {
        if (base_damage <= 0) {
            return 0.0f;
        }
        
        return static_cast<float>(damage_reduction) / base_damage;
    }
    
    /**
     * @brief Converte o tipo de dano para string para logging/debugging
     * @return String com o nome do tipo de dano
     */
    const char* GetDamageTypeName() const {
        switch (damage_type) {
            case 0: return "Físico";
            case 1: return "Fogo";
            case 2: return "Água";
            case 3: return "Terra";
            case 4: return "Ar";
            case 5: return "Luz";
            case 6: return "Trevas";
            case 7: return "Raio";
            case 8: return "Gelo";
            default: return "Desconhecido";
        }
    }
    
    /**
     * @brief Cria uma string de relatório resumido
     * @return Resumo do dano em formato de string
     */
    std::string GetSummary() const {
        std::string summary;
        
        if (is_miss) {
            summary = "ERRO";
        } else if (is_evaded) {
            summary = "ESQUIVA";
        } else if (is_blocked) {
            summary = "BLOQUEADO";
        } else {
            summary = std::to_string(total_damage);
            if (is_critical) {
                summary += " CRÍTICO";
            }
        }
        
        return summary;
    }
};

} // namespace types
} // namespace common
} // namespace wydbr

#endif // WYDBR_DAMAGE_INFO_H_ 