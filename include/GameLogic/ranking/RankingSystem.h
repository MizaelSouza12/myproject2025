/**
 * @file RankingSystem.h
 * @brief Sistema de ranking do WYDBR 2.0
 */

#ifndef WYDBR_RANKING_SYSTEM_H
#define WYDBR_RANKING_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "wyd_core.h"

namespace WYDBR {

/**
 * @class RankingSystem
 * @brief Gerencia o sistema de ranking do jogo
 */
class RankingSystem {
public:
    static RankingSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de rankings
    bool UpdateScore(uint32_t characterId, uint32_t score, uint8_t category);
    bool UpdateKills(uint32_t characterId, uint32_t kills);
    bool UpdateDeaths(uint32_t characterId, uint32_t deaths);
    bool UpdateWins(uint32_t characterId, uint32_t wins);
    bool UpdateLosses(uint32_t characterId, uint32_t losses);

    // Gerenciamento de categorias
    bool AddCategory(uint8_t categoryId, const std::string& name);
    bool RemoveCategory(uint8_t categoryId);
    bool UpdateCategory(uint8_t categoryId, const std::string& name);

    // Verificações
    bool HasScore(uint32_t characterId, uint8_t category) const;
    bool IsRanked(uint32_t characterId) const;
    bool IsCategoryValid(uint8_t category) const;
    bool HasMinimumScore(uint32_t characterId, uint8_t category) const;

    // Queries
    uint32_t GetScore(uint32_t characterId, uint8_t category) const;
    uint32_t GetRank(uint32_t characterId, uint8_t category) const;
    std::vector<uint32_t> GetTopPlayers(uint8_t category, uint32_t limit) const;
    std::vector<uint32_t> GetPlayerHistory(uint32_t characterId) const;

    // Utilidades
    void UpdateRankings(uint32_t deltaTime);
    void ProcessSeasonEnd();
    void ResetRankings();
    void SaveRankings();

private:
    RankingSystem() = default;
    ~RankingSystem() = default;

    // Prevenir cópias
    RankingSystem(const RankingSystem&) = delete;
    RankingSystem& operator=(const RankingSystem&) = delete;

    // Cálculos internos
    bool ValidateCategory(uint8_t category) const;
    bool CheckScoreRequirements(uint32_t characterId, uint8_t category) const;
    void UpdatePlayerRank(uint32_t characterId, uint8_t category);
    void HandleSeasonTransition();

    // Estado
    struct Category {
        std::string name;
        uint32_t minScore;
        uint32_t maxScore;
        bool isActive;
    };

    struct PlayerStats {
        uint32_t score;
        uint32_t kills;
        uint32_t deaths;
        uint32_t wins;
        uint32_t losses;
        uint32_t rank;
        uint32_t lastUpdate;
    };

    struct RankingData {
        std::unordered_map<uint8_t, Category> categories;
        std::unordered_map<uint32_t, std::unordered_map<uint8_t, PlayerStats>> playerStats;
        std::unordered_map<uint8_t, std::vector<uint32_t>> topPlayers;
    };

    RankingData m_rankings;
    uint32_t m_seasonStartTime;
    uint32_t m_seasonEndTime;
};

} // namespace WYDBR

#endif // WYDBR_RANKING_SYSTEM_H 