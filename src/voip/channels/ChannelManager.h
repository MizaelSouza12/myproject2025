/**
 * @file ChannelManager.h
 * @brief Gerenciamento de salas de voz para VoIP
 */

#ifndef WYDBR_CHANNEL_MANAGER_H
#define WYDBR_CHANNEL_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

namespace WYDBR {
namespace VoIP {

using ClientID = uint32_t;
using ChannelID = uint32_t;

struct ChannelConfig {
    std::string name;
    std::string password;
    bool isPrivate;
    bool isModerated;
    int maxUsers;
    bool enableRecording;
    bool enableVoiceActivity;
    float voiceActivityThreshold;
    bool enableNoiseReduction;
    bool enableEchoCancellation;
    bool enableAutomaticGainControl;
};

struct ChannelPermissions {
    bool canSpeak;
    bool canMute;
    bool canDeafen;
    bool canKick;
    bool canBan;
    bool canModerate;
    bool canRecord;
};

struct ChannelUser {
    ClientID clientId;
    bool isMuted;
    bool isDeafened;
    bool isModerator;
    ChannelPermissions permissions;
};

class ChannelManager {
public:
    ChannelManager();
    ~ChannelManager();

    bool Initialize();
    
    // Gerenciamento de canais
    ChannelID CreateChannel(const ChannelConfig& config);
    bool DestroyChannel(ChannelID channelId);
    bool UpdateChannelConfig(ChannelID channelId, const ChannelConfig& config);
    const ChannelConfig& GetChannelConfig(ChannelID channelId) const;
    
    // Gerenciamento de usuários
    bool JoinChannel(ChannelID channelId, ClientID clientId, const std::string& password = "");
    bool LeaveChannel(ChannelID channelId, ClientID clientId);
    bool SetUserMuted(ChannelID channelId, ClientID clientId, bool muted);
    bool SetUserDeafened(ChannelID channelId, ClientID clientId, bool deafened);
    bool SetUserModerator(ChannelID channelId, ClientID clientId, bool isModerator);
    bool SetUserPermissions(ChannelID channelId, ClientID clientId, const ChannelPermissions& permissions);
    
    // Consultas
    std::vector<ChannelID> GetChannels() const;
    std::vector<ClientID> GetChannelUsers(ChannelID channelId) const;
    bool IsUserInChannel(ChannelID channelId, ClientID clientId) const;
    bool IsUserMuted(ChannelID channelId, ClientID clientId) const;
    bool IsUserDeafened(ChannelID channelId, ClientID clientId) const;
    bool IsUserModerator(ChannelID channelId, ClientID clientId) const;
    const ChannelPermissions& GetUserPermissions(ChannelID channelId, ClientID clientId) const;
    
    // Eventos
    using OnUserJoinedCallback = std::function<void(ChannelID, ClientID)>;
    using OnUserLeftCallback = std::function<void(ChannelID, ClientID)>;
    using OnUserMutedCallback = std::function<void(ChannelID, ClientID, bool)>;
    using OnUserDeafenedCallback = std::function<void(ChannelID, ClientID, bool)>;
    using OnUserModeratorChangedCallback = std::function<void(ChannelID, ClientID, bool)>;
    
    void SetOnUserJoined(OnUserJoinedCallback callback);
    void SetOnUserLeft(OnUserLeftCallback callback);
    void SetOnUserMuted(OnUserMutedCallback callback);
    void SetOnUserDeafened(OnUserDeafenedCallback callback);
    void SetOnUserModeratorChanged(OnUserModeratorChangedCallback callback);

private:
    struct Channel {
        ChannelID id;
        ChannelConfig config;
        std::unordered_map<ClientID, ChannelUser> users;
    };
    
    ChannelID m_nextChannelId;
    std::unordered_map<ChannelID, Channel> m_channels;
    mutable std::mutex m_channelMutex;
    
    // Callbacks
    OnUserJoinedCallback m_onUserJoined;
    OnUserLeftCallback m_onUserLeft;
    OnUserMutedCallback m_onUserMuted;
    OnUserDeafenedCallback m_onUserDeafened;
    OnUserModeratorChangedCallback m_onUserModeratorChanged;
    
    bool ValidateChannelConfig(const ChannelConfig& config) const;
    bool ValidateUserPermissions(const ChannelPermissions& permissions) const;
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_CHANNEL_MANAGER_H 