/**
 * @file BanSystem.h
 * @brief Sistema de banimento do WYDBR 2.0
 */

#ifndef WYDBR_BAN_SYSTEM_H
#define WYDBR_BAN_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "wyd_core.h"

namespace WYDBR {

/**
 * @class BanSystem
 * @brief Gerencia o sistema de banimento do jogo
 */
class BanSystem {
public:
    static BanSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de banimentos
    bool BanAccount(uint32_t accountId, uint32_t duration, const std::string& reason);
    bool BanCharacter(uint32_t characterId, uint32_t duration, const std::string& reason);
    bool BanIP(const std::string& ipAddress, uint32_t duration, const std::string& reason);
    bool UnbanAccount(uint32_t accountId);
    bool UnbanCharacter(uint32_t characterId);
    bool UnbanIP(const std::string& ipAddress);

    // Verificações
    bool IsAccountBanned(uint32_t accountId) const;
    bool IsCharacterBanned(uint32_t characterId) const;
    bool IsIPBanned(const std::string& ipAddress) const;
    bool HasBanExpired(uint32_t banId) const;

    // Queries
    std::string GetBanReason(uint32_t banId) const;
    uint32_t GetBanDuration(uint32_t banId) const;
    uint32_t GetBanExpiryTime(uint32_t banId) const;
    std::vector<uint32_t> GetActiveBans() const;

    // Utilidades
    void UpdateBans(uint32_t deltaTime);
    void ProcessBanExpirations();
    void LogBanAction(uint32_t banId, const std::string& action);
    void NotifyBan(uint32_t banId);

private:
    BanSystem() = default;
    ~BanSystem() = default;

    // Prevenir cópias
    BanSystem(const BanSystem&) = delete;
    BanSystem& operator=(const BanSystem&) = delete;

    // Cálculos internos
    bool ValidateBan(uint32_t banId) const;
    bool CheckBanRequirements(uint32_t accountId) const;
    void UpdateBanStatus(uint32_t banId);
    void HandleBanExpiration(uint32_t banId);

    // Estado
    struct Ban {
        uint32_t id;
        uint32_t targetId;
        std::string targetType;
        std::string reason;
        uint32_t startTime;
        uint32_t duration;
        uint32_t adminId;
        bool isActive;
    };

    struct BanData {
        std::unordered_map<uint32_t, Ban> bans;
        std::unordered_map<uint32_t, std::vector<uint32_t>> accountBans;
        std::unordered_map<uint32_t, std::vector<uint32_t>> characterBans;
        std::unordered_map<std::string, std::vector<uint32_t>> ipBans;
    };

    BanData m_banData;
    uint32_t m_nextBanId;
};

} // namespace WYDBR

#endif // WYDBR_BAN_SYSTEM_H 