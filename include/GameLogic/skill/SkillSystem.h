/**
 * @file SkillSystem.h
 * @brief Sistema de habilidades do WYDBR 2.0
 */

#ifndef WYDBR_SKILL_SYSTEM_H
#define WYDBR_SKILL_SYSTEM_H

#include <unordered_map>
#include <vector>
#include "wyd_core.h"

namespace WYDBR {

/**
 * @struct SkillEffect
 * @brief Efeito de uma habilidade
 */
struct SkillEffect {
    uint32_t type;        ///< Tipo do efeito
    float value;          ///< Valor do efeito
    float duration;       ///< Duração do efeito em segundos
    float radius;         ///< Raio do efeito
    bool isAreaEffect;    ///< Se é um efeito em área
};

/**
 * @struct Skill
 * @brief Definição de uma habilidade
 */
struct Skill {
    uint16_t id;                  ///< ID da habilidade
    std::string name;             ///< Nome da habilidade
    std::string description;      ///< Descrição da habilidade
    CharacterClass requiredClass; ///< Classe requerida
    uint8_t requiredLevel;        ///< Nível requerido
    uint32_t mpCost;              ///< Custo de MP
    float cooldown;               ///< Tempo de recarga em segundos
    float castTime;               ///< Tempo de conjuração em segundos
    float range;                  ///< Alcance da habilidade
    std::vector<SkillEffect> effects; ///< Efeitos da habilidade
};

/**
 * @class SkillSystem
 * @brief Sistema de habilidades do WYDBR 2.0
 * 
 * Este sistema gerencia todas as habilidades do jogo, incluindo:
 * - Definição de habilidades
 * - Aprendizado de habilidades
 * - Uso de habilidades
 * - Efeitos de habilidades
 * - Cooldowns
 */
class SkillSystem {
public:
    /**
     * @brief Obtém a instância única do sistema de habilidades
     * @return Referência para a instância do sistema de habilidades
     */
    static SkillSystem& GetInstance();
    
    /**
     * @brief Inicializa o sistema de habilidades
     */
    void Initialize();
    
    /**
     * @brief Finaliza o sistema de habilidades
     */
    void Shutdown();
    
    /**
     * @brief Registra uma nova habilidade
     * @param skill Habilidade a ser registrada
     */
    void RegisterSkill(const Skill& skill);
    
    /**
     * @brief Obtém uma habilidade
     * @param skillId ID da habilidade
     * @return Ponteiro para a habilidade
     */
    const Skill* GetSkill(uint16_t skillId) const;
    
    /**
     * @brief Verifica se um personagem pode aprender uma habilidade
     * @param characterId ID do personagem
     * @param skillId ID da habilidade
     * @return true se pode aprender, false caso contrário
     */
    bool CanLearnSkill(uint32_t characterId, uint16_t skillId) const;
    
    /**
     * @brief Aprende uma habilidade
     * @param characterId ID do personagem
     * @param skillId ID da habilidade
     * @return true se aprendeu com sucesso, false caso contrário
     */
    bool LearnSkill(uint32_t characterId, uint16_t skillId);
    
    /**
     * @brief Verifica se um personagem pode usar uma habilidade
     * @param characterId ID do personagem
     * @param skillId ID da habilidade
     * @return true se pode usar, false caso contrário
     */
    bool CanUseSkill(uint32_t characterId, uint16_t skillId) const;
    
    /**
     * @brief Usa uma habilidade
     * @param characterId ID do personagem
     * @param skillId ID da habilidade
     * @param targetId ID do alvo (opcional)
     * @return true se usou com sucesso, false caso contrário
     */
    bool UseSkill(uint32_t characterId, uint16_t skillId, uint32_t targetId = 0);
    
    /**
     * @brief Atualiza o estado das habilidades
     * @param deltaTime Tempo desde a última atualização
     */
    void Update(float deltaTime);
    
    /**
     * @brief Verifica se uma habilidade está em cooldown
     * @param characterId ID do personagem
     * @param skillId ID da habilidade
     * @return true se está em cooldown, false caso contrário
     */
    bool IsSkillOnCooldown(uint32_t characterId, uint16_t skillId) const;
    
    /**
     * @brief Obtém o tempo restante de cooldown de uma habilidade
     * @param characterId ID do personagem
     * @param skillId ID da habilidade
     * @return Tempo restante em segundos
     */
    float GetSkillCooldownRemaining(uint32_t characterId, uint16_t skillId) const;
    
    /**
     * @brief Obtém todas as habilidades disponíveis para uma classe
     * @param classType Classe do personagem
     * @return Lista de habilidades disponíveis
     */
    std::vector<const Skill*> GetAvailableSkills(CharacterClass classType) const;
    
    /**
     * @brief Obtém todas as habilidades aprendidas por um personagem
     * @param characterId ID do personagem
     * @return Lista de habilidades aprendidas
     */
    std::vector<const Skill*> GetLearnedSkills(uint32_t characterId) const;

private:
    SkillSystem() = default;
    ~SkillSystem() = default;
    
    // Prevenir cópias
    SkillSystem(const SkillSystem&) = delete;
    SkillSystem& operator=(const SkillSystem&) = delete;
    
    // Mapa de habilidades
    std::unordered_map<uint16_t, Skill> skills;
    
    // Mapa de cooldowns
    struct CooldownInfo {
        float remainingTime;
        float totalTime;
    };
    std::unordered_map<uint32_t, std::unordered_map<uint16_t, CooldownInfo>> cooldowns;
    
    // Mapa de habilidades aprendidas
    std::unordered_map<uint32_t, std::vector<uint16_t>> learnedSkills;
};

} // namespace WYDBR

#endif // WYDBR_SKILL_SYSTEM_H 