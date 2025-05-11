/**
 * @file CombatSystem.h
 * @brief Sistema de combate do WYDBR 2.0
 */

#ifndef WYDBR_COMBAT_SYSTEM_H
#define WYDBR_COMBAT_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include "wyd_core.h"
#include "GameLogic/combat/DamageCalculator.h"

namespace WYDBR {

/**
 * @struct CombatStats
 * @brief Estatísticas de combate de um personagem
 */
struct CombatStats {
    uint32_t minDamage;      ///< Dano mínimo
    uint32_t maxDamage;      ///< Dano máximo
    uint32_t defense;        ///< Defesa
    uint32_t attackRate;     ///< Taxa de ataque
    uint32_t blockRate;      ///< Taxa de bloqueio
    uint32_t criticalRate;   ///< Taxa de crítico
    uint32_t criticalDamage; ///< Dano crítico
    uint32_t skillDamage;    ///< Dano de habilidade
    uint32_t skillDefense;   ///< Defesa contra habilidades
};

/**
 * @struct CombatResult
 * @brief Resultado de um ataque
 */
struct CombatResult {
    bool hit;                ///< Se acertou o ataque
    bool critical;           ///< Se foi crítico
    bool blocked;            ///< Se foi bloqueado
    uint32_t damage;         ///< Dano causado
    uint32_t skillId;        ///< ID da habilidade usada (0 para ataque normal)
};

/**
 * @class CombatSystem
 * @brief Sistema de combate do WYDBR 2.0
 * 
 * Este sistema gerencia todo o combate do jogo, incluindo:
 * - Cálculo de dano
 * - Taxas de acerto/crítico/bloqueio
 * - Efeitos de habilidades
 * - Combate em área
 */
class CombatSystem {
public:
    /**
     * @brief Obtém a instância única do sistema de combate
     * @return Referência para a instância do sistema de combate
     */
    static CombatSystem& GetInstance();
    
    /**
     * @brief Inicializa o sistema de combate
     */
    void Initialize();
    
    /**
     * @brief Finaliza o sistema de combate
     */
    void Shutdown();
    
    /**
     * @brief Atualiza as estatísticas de combate de um personagem
     * @param characterId ID do personagem
     * @param stats Novas estatísticas
     */
    void UpdateCombatStats(uint32_t characterId, const CombatStats& stats);
    
    /**
     * @brief Obtém as estatísticas de combate de um personagem
     * @param characterId ID do personagem
     * @return Ponteiro para as estatísticas
     */
    const CombatStats* GetCombatStats(uint32_t characterId) const;
    
    /**
     * @brief Processa um ataque normal
     * @param attackerId ID do atacante
     * @param targetId ID do alvo
     * @return Resultado do ataque
     */
    CombatResult ProcessAttack(uint32_t attackerId, uint32_t targetId);
    
    /**
     * @brief Processa um ataque com habilidade
     * @param attackerId ID do atacante
     * @param targetId ID do alvo
     * @param skillId ID da habilidade
     * @return Resultado do ataque
     */
    CombatResult ProcessSkill(uint32_t attackerId, uint32_t targetId, uint16_t skillId);
    
    /**
     * @brief Processa um efeito em área
     * @param attackerId ID do atacante
     * @param center Centro do efeito
     * @param radius Raio do efeito
     * @param skillId ID da habilidade
     * @return Lista de resultados do ataque
     */
    std::vector<CombatResult> ProcessAreaEffect(uint32_t attackerId, const Position& center, float radius, uint16_t skillId);
    
    /**
     * @brief Verifica se um alvo está no alcance de um atacante
     * @param attackerId ID do atacante
     * @param targetId ID do alvo
     * @param range Alcance do ataque
     * @return true se está no alcance, false caso contrário
     */
    bool IsInRange(uint32_t attackerId, uint32_t targetId, float range) const;
    
    /**
     * @brief Verifica se um alvo está no alcance de um efeito em área
     * @param targetId ID do alvo
     * @param center Centro do efeito
     * @param radius Raio do efeito
     * @return true se está no alcance, false caso contrário
     */
    bool IsInArea(uint32_t targetId, const Position& center, float radius) const;

private:
    CombatSystem() = default;
    ~CombatSystem() = default;
    
    // Prevenir cópias
    CombatSystem(const CombatSystem&) = delete;
    CombatSystem& operator=(const CombatSystem&) = delete;
    
    // Mapa de estatísticas de combate
    std::unordered_map<uint32_t, CombatStats> combatStats;
    
    // Funções auxiliares
    uint32_t CalculateDamage(uint32_t attackerId, uint32_t targetId, bool isSkill) const;
    bool CheckHit(uint32_t attackerId, uint32_t targetId) const;
    bool CheckCritical(uint32_t attackerId) const;
    bool CheckBlock(uint32_t targetId) const;
    float CalculateDistance(const Position& pos1, const Position& pos2) const;
};

} // namespace WYDBR

#endif // WYDBR_COMBAT_SYSTEM_H 