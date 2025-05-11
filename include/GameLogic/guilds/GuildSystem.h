/**
 * @file GuildSystem.h
 * @brief Sistema de guildas do WYDBR 2.0
 */

#ifndef WYDBR_GUILD_SYSTEM_H
#define WYDBR_GUILD_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "wyd_core.h"

namespace WYDBR {

/**
 * @class GuildSystem
 * @brief Gerencia o sistema de guildas do jogo
 */
class GuildSystem {
public:
    static GuildSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de guildas
    bool CreateGuild(uint32_t leaderId, const std::string& name);
    bool DisbandGuild(uint32_t guildId);
    bool JoinGuild(uint32_t characterId, uint32_t guildId);
    bool LeaveGuild(uint32_t characterId);
    bool KickMember(uint32_t guildId, uint32_t memberId, uint32_t executorId);

    // Gerenciamento de cargos
    bool SetMemberRank(uint32_t guildId, uint32_t memberId, uint8_t rank);
    bool SetGuildLeader(uint32_t guildId, uint32_t newLeaderId);
    bool UpdateGuildPermissions(uint32_t guildId, uint8_t rank, uint32_t permissions);

    // Gerenciamento de recursos
    bool AddGuildGold(uint32_t guildId, uint32_t amount);
    bool RemoveGuildGold(uint32_t guildId, uint32_t amount);
    bool AddGuildItem(uint32_t guildId, uint32_t itemId, uint16_t amount);
    bool RemoveGuildItem(uint32_t guildId, uint32_t itemId, uint16_t amount);

    // Verificações
    bool IsGuildMember(uint32_t characterId, uint32_t guildId) const;
    bool IsGuildLeader(uint32_t characterId, uint32_t guildId) const;
    bool HasGuildPermission(uint32_t characterId, uint32_t permission) const;
    bool IsGuildNameAvailable(const std::string& name) const;

    // Queries
    uint32_t GetCharacterGuild(uint32_t characterId) const;
    uint8_t GetMemberRank(uint32_t characterId) const;
    std::vector<uint32_t> GetGuildMembers(uint32_t guildId) const;
    std::string GetGuildName(uint32_t guildId) const;

    // Utilidades
    void UpdateGuildStatus(uint32_t guildId);
    void ProcessGuildWar(uint32_t guildId1, uint32_t guildId2);
    void HandleGuildAlliance(uint32_t guildId1, uint32_t guildId2);
    void BroadcastGuildMessage(uint32_t guildId, const std::string& message);

private:
    GuildSystem() = default;
    ~GuildSystem() = default;

    // Prevenir cópias
    GuildSystem(const GuildSystem&) = delete;
    GuildSystem& operator=(const GuildSystem&) = delete;

    // Cálculos internos
    bool ValidateGuildName(const std::string& name) const;
    bool CheckGuildRequirements(uint32_t characterId) const;
    void UpdateGuildLevel(uint32_t guildId);
    void HandleGuildExpiration(uint32_t guildId);

    // Estado
    struct GuildMember {
        uint32_t characterId;
        uint8_t rank;
        uint32_t joinDate;
        uint32_t lastLogin;
        uint32_t contribution;
    };

    struct GuildData {
        std::string name;
        uint32_t leaderId;
        uint32_t creationDate;
        uint32_t level;
        uint32_t experience;
        uint32_t gold;
        std::vector<GuildMember> members;
        std::unordered_map<uint8_t, uint32_t> rankPermissions;
    };

    std::unordered_map<uint32_t, GuildData> m_guilds;
    std::unordered_map<uint32_t, uint32_t> m_characterGuilds;
};

} // namespace WYDBR

#endif // WYDBR_GUILD_SYSTEM_H 