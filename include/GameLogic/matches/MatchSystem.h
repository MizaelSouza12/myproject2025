/**
 * @file MatchSystem.h
 * @brief Sistema de partidas do WYDBR 2.0
 */

#ifndef WYDBR_MATCH_SYSTEM_H
#define WYDBR_MATCH_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "wyd_core.h"

namespace WYDBR {

/**
 * @class MatchSystem
 * @brief Gerencia o sistema de partidas do jogo
 */
class MatchSystem {
public:
    static MatchSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de partidas
    bool CreateMatch(uint32_t matchId, uint8_t matchType, uint32_t maxPlayers);
    bool JoinMatch(uint32_t characterId, uint32_t matchId);
    bool LeaveMatch(uint32_t characterId, uint32_t matchId);
    bool StartMatch(uint32_t matchId);
    bool EndMatch(uint32_t matchId);

    // Gerenciamento de times
    bool CreateTeam(uint32_t matchId, uint8_t teamId);
    bool JoinTeam(uint32_t characterId, uint32_t matchId, uint8_t teamId);
    bool LeaveTeam(uint32_t characterId, uint32_t matchId);
    bool SwitchTeam(uint32_t characterId, uint32_t matchId, uint8_t newTeamId);

    // Verificações
    bool IsInMatch(uint32_t characterId) const;
    bool IsMatchFull(uint32_t matchId) const;
    bool IsMatchStarted(uint32_t matchId) const;
    bool IsMatchEnded(uint32_t matchId) const;

    // Queries
    uint32_t GetCharacterMatch(uint32_t characterId) const;
    uint8_t GetCharacterTeam(uint32_t characterId) const;
    std::vector<uint32_t> GetMatchPlayers(uint32_t matchId) const;
    std::vector<uint32_t> GetTeamPlayers(uint32_t matchId, uint8_t teamId) const;

    // Utilidades
    void UpdateMatches(uint32_t deltaTime);
    void ProcessMatchResults(uint32_t matchId);
    void HandleMatchTimeout(uint32_t matchId);
    void BroadcastMatchMessage(uint32_t matchId, const std::string& message);

private:
    MatchSystem() = default;
    ~MatchSystem() = default;

    // Prevenir cópias
    MatchSystem(const MatchSystem&) = delete;
    MatchSystem& operator=(const MatchSystem&) = delete;

    // Cálculos internos
    bool ValidateMatch(uint32_t matchId) const;
    bool CheckMatchRequirements(uint32_t characterId, uint32_t matchId) const;
    void UpdateMatchProgress(uint32_t matchId);
    void HandleMatchFailure(uint32_t matchId);

    // Estado
    struct Match {
        uint8_t type;
        uint32_t maxPlayers;
        uint32_t startTime;
        uint32_t endTime;
        bool isActive;
        std::unordered_map<uint8_t, std::vector<uint32_t>> teams;
    };

    struct MatchProgress {
        uint32_t matchId;
        uint32_t startTime;
        uint32_t remainingTime;
        bool isComplete;
    };

    std::unordered_map<uint32_t, Match> m_matches;
    std::unordered_map<uint32_t, MatchProgress> m_matchProgress;
    std::unordered_map<uint32_t, uint32_t> m_characterMatches;
};

} // namespace WYDBR

#endif // WYDBR_MATCH_SYSTEM_H 