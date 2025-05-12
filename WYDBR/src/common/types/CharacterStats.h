/**
 * @file CharacterStats.h
 * @brief Define a estrutura de estatísticas para personagens
 */

#ifndef WYDBR_CHARACTER_STATS_H_
#define WYDBR_CHARACTER_STATS_H_

#include <cstdint>
#include <vector>
#include <string>

namespace wydbr {
namespace common {
namespace types {

/**
 * @struct CharacterStats
 * @brief Estatísticas completas de um personagem
 */
struct CharacterStats {
    // Estatísticas básicas
    int32_t strength = 10;       ///< Força - afeta dano físico, carga, etc.
    int32_t vitality = 10;       ///< Vitalidade - afeta HP, resistência física, etc.
    int32_t dexterity = 10;      ///< Destreza - afeta precisão, evasão, crítico, etc.
    int32_t intelligence = 10;   ///< Inteligência - afeta dano mágico, MP, etc.
    int32_t wisdom = 10;         ///< Sabedoria - afeta resistência mágica, regeneração de MP, etc.
    
    // Estatísticas derivadas
    int32_t max_hp = 100;            ///< HP máximo
    int32_t max_mp = 100;            ///< MP máximo
    int32_t max_stamina = 100;       ///< Stamina máxima
    int32_t defense = 0;             ///< Defesa física
    int32_t magic_resistance = 0;    ///< Resistência mágica
    
    // Informações do personagem
    uint8_t class_type = 0;      ///< Tipo da classe (0=guerreiro, 1=arqueiro, etc.)
    uint16_t level = 1;          ///< Nível do personagem
    uint64_t experience = 0;     ///< Experiência atual
    
    // Resistências elementais: fogo, água, terra, ar, luz, trevas, raio, gelo
    std::vector<float> elemental_resistances = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    // Masterias de armas: espadas, machados, lanças, arcos, cajados, adagas, etc.
    std::vector<float> masteries = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    // Maestrias elementais: fogo, água, terra, ar, luz, trevas, raio, gelo
    std::vector<float> elemental_masteries = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    // Atributos adicionais
    float attack_speed = 1.0f;       ///< Velocidade de ataque (1.0 = normal)
    float movement_speed = 1.0f;     ///< Velocidade de movimento (1.0 = normal)
    float casting_speed = 1.0f;      ///< Velocidade de conjuração (1.0 = normal)
    float cooldown_reduction = 0.0f;  ///< Redução de tempo de recarga (0.0 = sem redução)
    float hp_regen = 0.0f;           ///< Regeneração de HP por segundo
    float mp_regen = 0.0f;           ///< Regeneração de MP por segundo
    float stamina_regen = 0.0f;      ///< Regeneração de stamina por segundo
    
    // Status especiais
    bool is_invulnerable = false;    ///< Se o personagem é invulnerável
    bool is_invisible = false;       ///< Se o personagem está invisível
    bool can_fly = false;            ///< Se o personagem pode voar
    
    /**
     * @brief Calcula o HP máximo baseado nos atributos
     * @return HP máximo calculado
     */
    int32_t CalculateMaxHP() const {
        int32_t baseHP = 100;
        
        // HP por classe
        switch (class_type) {
            case 0: // Guerreiro
                baseHP = 120;
                break;
            case 1: // Arqueiro
                baseHP = 100;
                break;
            case 2: // Mago
                baseHP = 80;
                break;
            case 3: // Sacerdote
                baseHP = 90;
                break;
            // Outras classes
            default:
                baseHP = 100;
        }
        
        // HP por nível e vitalidade
        return baseHP + (level * 10) + (vitality * 5);
    }
    
    /**
     * @brief Calcula o MP máximo baseado nos atributos
     * @return MP máximo calculado
     */
    int32_t CalculateMaxMP() const {
        int32_t baseMP = 100;
        
        // MP por classe
        switch (class_type) {
            case 0: // Guerreiro
                baseMP = 60;
                break;
            case 1: // Arqueiro
                baseMP = 80;
                break;
            case 2: // Mago
                baseMP = 120;
                break;
            case 3: // Sacerdote
                baseMP = 110;
                break;
            // Outras classes
            default:
                baseMP = 100;
        }
        
        // MP por nível, inteligência e sabedoria
        return baseMP + (level * 5) + (intelligence * 3) + (wisdom * 2);
    }
    
    /**
     * @brief Atualiza as estatísticas derivadas
     */
    void UpdateDerivedStats() {
        max_hp = CalculateMaxHP();
        max_mp = CalculateMaxMP();
        
        // Atualizar outras estatísticas derivadas conforme necessário
    }
};

} // namespace types
} // namespace common
} // namespace wydbr

#endif // WYDBR_CHARACTER_STATS_H_ 