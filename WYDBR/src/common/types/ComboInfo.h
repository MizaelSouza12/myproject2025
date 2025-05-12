/**
 * @file ComboInfo.h
 * @brief Define as estruturas para o sistema de combos
 */

#ifndef WYDBR_COMBO_INFO_H_
#define WYDBR_COMBO_INFO_H_

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

#include "SkillId.h"

namespace wydbr {
namespace common {
namespace types {

/**
 * @struct ComboEffect
 * @brief Efeito produzido ao completar um combo
 */
struct ComboEffect {
    uint8_t bonus_type;     ///< Tipo de bônus (0=dano, 1=velocidade de ataque, etc)
    float bonus_value;      ///< Valor do bônus
    uint32_t duration_ms;   ///< Duração em milissegundos
};

/**
 * @struct ComboDefinition
 * @brief Definição completa de um combo
 */
struct ComboDefinition {
    uint32_t combo_id = 0;              ///< ID único do combo
    std::string name;                   ///< Nome do combo
    std::string description;            ///< Descrição do combo
    uint8_t level = 1;                  ///< Nível/rank do combo
    uint8_t required_class = 0xFF;      ///< Classe requerida (0xFF = qualquer classe)
    std::vector<SkillId> skill_sequence; ///< Sequência de habilidades do combo
    std::vector<ComboEffect> effects;   ///< Efeitos produzidos ao completar o combo
    uint32_t time_window_ms = 5000;     ///< Janela de tempo para execução (ms)
    bool requires_same_target = false;  ///< Se requer o mesmo alvo para todas as habilidades
    uint32_t damage_bonus_percent = 0;  ///< Bônus percentual de dano
    uint32_t cooldown_ms = 0;           ///< Tempo de recarga após uso
    uint32_t icon_id = 0;               ///< ID do ícone
    uint32_t effect_id = 0;             ///< ID do efeito visual
};

/**
 * @struct ComboInfo
 * @brief Informações sobre um combo ativo para um jogador
 */
struct ComboInfo {
    uint32_t id = 0;                ///< ID do combo
    std::string name;               ///< Nome do combo
    uint8_t level = 0;              ///< Nível do combo
    uint8_t current_step = 0;       ///< Passo atual na sequência
    uint8_t total_steps = 0;        ///< Total de passos na sequência completa
};

/**
 * @struct ComboBonus
 * @brief Bônus de combo ativo
 */
struct ComboBonus {
    uint32_t id;                    ///< ID do bônus
    uint8_t type;                   ///< Tipo de bônus
    float value;                    ///< Valor do bônus
};

/**
 * @struct ComboResult
 * @brief Resultado do processamento de uma habilidade no sistema de combos
 */
struct ComboResult {
    bool combo_activated = false;   ///< Se um combo foi ativado (completo)
    bool combo_advanced = false;    ///< Se um combo avançou para o próximo passo
    uint32_t combo_id = 0;          ///< ID do combo em progresso/ativado
    std::string combo_name;         ///< Nome do combo
    uint8_t current_step = 0;       ///< Passo atual
    uint8_t total_steps = 0;        ///< Total de passos
};

} // namespace types
} // namespace common
} // namespace wydbr

#endif // WYDBR_COMBO_INFO_H_ 