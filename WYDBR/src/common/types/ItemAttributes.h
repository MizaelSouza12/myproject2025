/**
 * @file ItemAttributes.h
 * @brief Define a estrutura de atributos para itens
 */

#ifndef WYDBR_ITEM_ATTRIBUTES_H_
#define WYDBR_ITEM_ATTRIBUTES_H_

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace wydbr {
namespace common {
namespace types {

/**
 * @struct ItemAttributes
 * @brief Atributos e propriedades de um item
 */
struct ItemAttributes {
    // Identificação do item
    uint32_t template_id = 0;        ///< ID do modelo de item
    uint16_t grade = 0;              ///< Grau de qualidade do item (0=normal, 1=magic, 2=rare, 3=unique, etc)
    uint8_t tier = 0;                ///< Tier do item
    std::string custom_name;         ///< Nome customizado, se houver
    
    // Propriedades básicas
    uint16_t level_req = 1;          ///< Nível mínimo para usar
    uint8_t class_req = 0xFF;        ///< Classe requerida (0xFF = qualquer classe)
    uint32_t durability = 0;         ///< Durabilidade atual
    uint32_t max_durability = 0;     ///< Durabilidade máxima
    uint8_t binding_type = 0;        ///< Tipo de ligação (0=não ligado, 1=ligado ao pegar, 2=ligado ao equipar)
    bool tradeable = true;           ///< Se pode ser comercializado
    uint8_t socket_count = 0;        ///< Quantidade de soquetes
    std::vector<uint32_t> sockets;   ///< IDs de gemas/encantos nos soquetes
    
    // Atributos para armas
    float damage_min = 0.0f;         ///< Dano mínimo
    float damage_max = 0.0f;         ///< Dano máximo
    uint8_t damage_type = 0;         ///< Tipo de dano (0=físico, 1=fogo, 2=água, etc)
    uint8_t weapon_type = 0xFF;      ///< Tipo de arma (0=espada, 1=machado, etc)
    float attack_speed = 1.0f;       ///< Velocidade de ataque
    float range = 1.0f;              ///< Alcance do ataque
    float critical_chance = 0.0f;    ///< Chance de crítico
    float critical_multiplier = 0.0f; ///< Multiplicador de dano crítico
    float precision = 0.0f;          ///< Precisão adicional
    float armor_penetration = 0.0f;  ///< Penetração de armadura
    
    // Atributos para armaduras/equipamentos
    uint8_t equip_slot = 0xFF;       ///< Slot de equipamento (0=cabeça, 1=peito, etc)
    int32_t armor = 0;               ///< Valor de armadura
    int32_t magic_defense = 0;       ///< Defesa mágica
    
    // Bônus de estatísticas
    int32_t str_bonus = 0;           ///< Bônus de força
    int32_t vit_bonus = 0;           ///< Bônus de vitalidade
    int32_t dex_bonus = 0;           ///< Bônus de destreza
    int32_t int_bonus = 0;           ///< Bônus de inteligência
    int32_t wis_bonus = 0;           ///< Bônus de sabedoria
    
    int32_t hp_bonus = 0;            ///< Bônus de HP
    int32_t mp_bonus = 0;            ///< Bônus de MP
    
    // Resistências elementais
    std::vector<float> elemental_resists = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    // Propriedades especiais
    std::unordered_map<std::string, std::string> special_properties;
    
    // Informações visuais
    uint32_t model_id = 0;           ///< ID do modelo visual
    uint16_t color = 0;              ///< Cor do item
    uint8_t glow_effect = 0;         ///< Efeito de brilho
    
    /**
     * @brief Calcula o nível do item com base em seus atributos
     * @return Nível calculado do item
     */
    uint16_t CalculateItemLevel() const {
        float powerLevel = 0.0f;
        
        // Para armas
        if (weapon_type != 0xFF) {
            powerLevel += (damage_min + damage_max) * 0.5f;
            powerLevel += critical_chance * 50.0f;
            powerLevel += armor_penetration * 30.0f;
        }
        
        // Para armaduras
        if (equip_slot != 0xFF) {
            powerLevel += armor * 0.2f;
            powerLevel += magic_defense * 0.3f;
        }
        
        // Bônus de estatística
        powerLevel += (str_bonus + vit_bonus + dex_bonus + int_bonus + wis_bonus) * 2.0f;
        powerLevel += (hp_bonus / 10.0f) + (mp_bonus / 8.0f);
        
        // Resistências e outros fatores
        float totalResist = 0.0f;
        for (float resist : elemental_resists) {
            totalResist += resist;
        }
        powerLevel += totalResist * 10.0f;
        
        // Soquetes adicionam poder
        powerLevel += socket_count * 5.0f;
        
        // Ajustar com base no grade
        powerLevel *= (1.0f + (grade * 0.2f));
        
        return static_cast<uint16_t>(std::max(1.0f, powerLevel / 10.0f));
    }
    
    /**
     * @brief Verifica se o item está quebrado
     * @return true se o item está quebrado
     */
    bool IsBroken() const {
        return max_durability > 0 && durability == 0;
    }
    
    /**
     * @brief Verifica se os requisitos são atendidos
     * @param charLevel Nível do personagem
     * @param charClass Classe do personagem
     * @return true se os requisitos são atendidos
     */
    bool MeetsRequirements(uint16_t charLevel, uint8_t charClass) const {
        if (charLevel < level_req) {
            return false;
        }
        
        if (class_req != 0xFF && class_req != charClass) {
            return false;
        }
        
        return true;
    }
};

} // namespace types
} // namespace common
} // namespace wydbr

#endif // WYDBR_ITEM_ATTRIBUTES_H_ 