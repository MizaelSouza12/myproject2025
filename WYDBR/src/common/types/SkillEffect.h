/**
 * @file SkillEffect.h
 * @brief Define a estrutura para efeitos de habilidades
 */

#ifndef WYDBR_SKILL_EFFECT_H_
#define WYDBR_SKILL_EFFECT_H_

#include <cstdint>
#include <string>
#include <chrono>

namespace wydbr {
namespace common {
namespace types {

/**
 * @struct SkillEffect
 * @brief Estrutura para representar efeitos de habilidades como buffs, debuffs, DoT, HoT, etc.
 */
struct SkillEffect {
    // Identificação do efeito
    uint32_t effect_id = 0;         ///< ID único do efeito
    std::string name;               ///< Nome do efeito
    std::string description;        ///< Descrição do efeito
    
    // Propriedades temporais
    std::chrono::milliseconds duration{0};         ///< Duração total do efeito
    std::chrono::milliseconds tick_interval{0};    ///< Intervalo entre ticks para efeitos contínuos (DoT/HoT)
    
    // Propriedades de efeito
    uint8_t effect_type = 0;        ///< Tipo de efeito (0=buff, 1=debuff, 2=DoT, 3=HoT, etc.)
    uint8_t element_type = 0;       ///< Tipo elemental (0=neutro, 1=fogo, 2=água, etc.)
    float value = 0.0f;             ///< Valor base do efeito
    bool is_percent_modifier = false; ///< Se o valor é percentual (ex: +20% vs +20)
    bool is_removable = true;       ///< Se o efeito pode ser removido por itens/habilidades
    bool is_visible = true;         ///< Se o efeito é visível para o jogador
    
    // Escalamento
    uint8_t scaling_stat = 0xFF;    ///< Estatística para escalamento (0=STR, 1=VIT, etc., 0xFF=nenhuma)
    float scaling_factor = 0.0f;    ///< Fator de escalamento
    
    // Modificadores de estatísticas
    uint8_t stat_type = 0xFF;       ///< Tipo de estatística modificada (0=STR, 1=VIT, etc., 0xFF=nenhuma)
    
    // Visualização
    uint32_t icon_id = 0;           ///< ID do ícone
    uint32_t visual_effect_id = 0;  ///< ID do efeito visual
    
    // Estágios (para efeitos que se intensificam com o tempo)
    uint8_t max_stacks = 1;         ///< Número máximo de acúmulos
    uint8_t current_stacks = 1;     ///< Número atual de acúmulos
    
    /**
     * @brief Calcula o valor efetivo do efeito considerando acúmulos
     * @return Valor calculado
     */
    float CalculateEffectiveValue() const {
        return value * current_stacks;
    }
    
    /**
     * @brief Verifica se o efeito é um buff (efeito positivo)
     * @return true se for um buff
     */
    bool IsBuff() const {
        return effect_type < 100; // Convenção: efeito_tipo < 100 são buffs, >= 100 são debuffs
    }
    
    /**
     * @brief Verifica se o efeito causa dano ao longo do tempo
     * @return true se for um DoT
     */
    bool IsDamageOverTime() const {
        return effect_type == 2; // 2 = DoT na convenção deste exemplo
    }
    
    /**
     * @brief Verifica se o efeito causa cura ao longo do tempo
     * @return true se for um HoT
     */
    bool IsHealOverTime() const {
        return effect_type == 3; // 3 = HoT na convenção deste exemplo
    }
    
    /**
     * @brief Verifica se o efeito tem ticks periódicos
     * @return true se tiver ticks periódicos
     */
    bool HasPeriodicTicks() const {
        return tick_interval.count() > 0;
    }
    
    /**
     * @brief Calcula quantos ticks ocorrerão durante toda a duração
     * @return Número total de ticks
     */
    int CalculateTotalTicks() const {
        if (tick_interval.count() <= 0) {
            return 0;
        }
        
        return static_cast<int>(duration.count() / tick_interval.count()) + 1;
    }
};

} // namespace types
} // namespace common
} // namespace wydbr

#endif // WYDBR_SKILL_EFFECT_H_ 