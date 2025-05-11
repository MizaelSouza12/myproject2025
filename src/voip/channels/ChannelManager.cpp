/**
 * @file ChannelManager.cpp
 * @brief Implementação do gerenciamento de salas de voz
 */

#include "ChannelManager.h"
#include <iostream>

namespace WYDBR {
namespace VoIP {

ChannelManager::ChannelManager()
    : m_nextChannelId(1)
    , m_onUserJoined(nullptr)
    , m_onUserLeft(nullptr)
    , m_onUserMuted(nullptr)
    , m_onUserDeafened(nullptr)
    , m_onUserModeratorChanged(nullptr) {
}

ChannelManager::~ChannelManager() {
}

bool ChannelManager::Initialize() {
    return true;
}

ChannelID ChannelManager::CreateChannel(const ChannelConfig& config) {
    if (!ValidateChannelConfig(config)) {
        std::cerr << "Configuração de canal inválida" << std::endl;
        return 0;
    }

    std::lock_guard<std::mutex> lock(m_channelMutex);

    Channel channel;
    channel.id = m_nextChannelId++;
    channel.config = config;

    m_channels[channel.id] = channel;
    return channel.id;
}

bool ChannelManager::DestroyChannel(ChannelID channelId) {
    std::lock_guard<std::mutex> lock(m_channelMutex);

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    // Notificar todos os usuários que o canal será destruído
    for (const auto& user : it->second.users) {
        if (m_onUserLeft) {
            m_onUserLeft(channelId, user.first);
        }
    }

    m_channels.erase(it);
    return true;
}

bool ChannelManager::UpdateChannelConfig(ChannelID channelId, const ChannelConfig& config) {
    if (!ValidateChannelConfig(config)) {
        std::cerr << "Configuração de canal inválida" << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> lock(m_channelMutex);

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    it->second.config = config;
    return true;
}

const ChannelConfig& ChannelManager::GetChannelConfig(ChannelID channelId) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_channelMutex));

    static const ChannelConfig defaultConfig = {};

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return defaultConfig;
    }

    return it->second.config;
}

bool ChannelManager::JoinChannel(ChannelID channelId, ClientID clientId, const std::string& password) {
    std::lock_guard<std::mutex> lock(m_channelMutex);

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    const auto& channel = it->second;

    // Verificar senha para canais privados
    if (channel.config.isPrivate && channel.config.password != password) {
        return false;
    }

    // Verificar limite de usuários
    if (channel.config.maxUsers > 0 && channel.users.size() >= channel.config.maxUsers) {
        return false;
    }

    // Verificar se o usuário já está no canal
    if (channel.users.find(clientId) != channel.users.end()) {
        return false;
    }

    // Adicionar usuário ao canal
    ChannelUser user;
    user.clientId = clientId;
    user.isMuted = false;
    user.isDeafened = false;
    user.isModerator = false;
    user.permissions = ChannelPermissions{true, true, true, false, false, false, false};

    it->second.users[clientId] = user;

    // Notificar que o usuário entrou
    if (m_onUserJoined) {
        m_onUserJoined(channelId, clientId);
    }

    return true;
}

bool ChannelManager::LeaveChannel(ChannelID channelId, ClientID clientId) {
    std::lock_guard<std::mutex> lock(m_channelMutex);

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    auto userIt = it->second.users.find(clientId);
    if (userIt == it->second.users.end()) {
        return false;
    }

    it->second.users.erase(userIt);

    // Notificar que o usuário saiu
    if (m_onUserLeft) {
        m_onUserLeft(channelId, clientId);
    }

    return true;
}

bool ChannelManager::SetUserMuted(ChannelID channelId, ClientID clientId, bool muted) {
    std::lock_guard<std::mutex> lock(m_channelMutex);

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    auto userIt = it->second.users.find(clientId);
    if (userIt == it->second.users.end()) {
        return false;
    }

    userIt->second.isMuted = muted;

    // Notificar mudança de mute
    if (m_onUserMuted) {
        m_onUserMuted(channelId, clientId, muted);
    }

    return true;
}

bool ChannelManager::SetUserDeafened(ChannelID channelId, ClientID clientId, bool deafened) {
    std::lock_guard<std::mutex> lock(m_channelMutex);

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    auto userIt = it->second.users.find(clientId);
    if (userIt == it->second.users.end()) {
        return false;
    }

    userIt->second.isDeafened = deafened;

    // Notificar mudança de deafen
    if (m_onUserDeafened) {
        m_onUserDeafened(channelId, clientId, deafened);
    }

    return true;
}

bool ChannelManager::SetUserModerator(ChannelID channelId, ClientID clientId, bool isModerator) {
    std::lock_guard<std::mutex> lock(m_channelMutex);

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    auto userIt = it->second.users.find(clientId);
    if (userIt == it->second.users.end()) {
        return false;
    }

    userIt->second.isModerator = isModerator;

    // Atualizar permissões
    userIt->second.permissions.canModerate = isModerator;
    userIt->second.permissions.canKick = isModerator;
    userIt->second.permissions.canBan = isModerator;

    // Notificar mudança de moderador
    if (m_onUserModeratorChanged) {
        m_onUserModeratorChanged(channelId, clientId, isModerator);
    }

    return true;
}

bool ChannelManager::SetUserPermissions(ChannelID channelId, ClientID clientId, const ChannelPermissions& permissions) {
    if (!ValidateUserPermissions(permissions)) {
        std::cerr << "Permissões de usuário inválidas" << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> lock(m_channelMutex);

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    auto userIt = it->second.users.find(clientId);
    if (userIt == it->second.users.end()) {
        return false;
    }

    userIt->second.permissions = permissions;
    return true;
}

std::vector<ChannelID> ChannelManager::GetChannels() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_channelMutex));

    std::vector<ChannelID> channels;
    channels.reserve(m_channels.size());

    for (const auto& channel : m_channels) {
        channels.push_back(channel.first);
    }

    return channels;
}

std::vector<ClientID> ChannelManager::GetChannelUsers(ChannelID channelId) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_channelMutex));

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return std::vector<ClientID>();
    }

    std::vector<ClientID> users;
    users.reserve(it->second.users.size());

    for (const auto& user : it->second.users) {
        users.push_back(user.first);
    }

    return users;
}

bool ChannelManager::IsUserInChannel(ChannelID channelId, ClientID clientId) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_channelMutex));

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    return it->second.users.find(clientId) != it->second.users.end();
}

bool ChannelManager::IsUserMuted(ChannelID channelId, ClientID clientId) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_channelMutex));

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    auto userIt = it->second.users.find(clientId);
    if (userIt == it->second.users.end()) {
        return false;
    }

    return userIt->second.isMuted;
}

bool ChannelManager::IsUserDeafened(ChannelID channelId, ClientID clientId) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_channelMutex));

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    auto userIt = it->second.users.find(clientId);
    if (userIt == it->second.users.end()) {
        return false;
    }

    return userIt->second.isDeafened;
}

bool ChannelManager::IsUserModerator(ChannelID channelId, ClientID clientId) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_channelMutex));

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return false;
    }

    auto userIt = it->second.users.find(clientId);
    if (userIt == it->second.users.end()) {
        return false;
    }

    return userIt->second.isModerator;
}

const ChannelPermissions& ChannelManager::GetUserPermissions(ChannelID channelId, ClientID clientId) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_channelMutex));

    static const ChannelPermissions defaultPermissions = {false, false, false, false, false, false, false};

    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        return defaultPermissions;
    }

    auto userIt = it->second.users.find(clientId);
    if (userIt == it->second.users.end()) {
        return defaultPermissions;
    }

    return userIt->second.permissions;
}

void ChannelManager::SetOnUserJoined(OnUserJoinedCallback callback) {
    m_onUserJoined = callback;
}

void ChannelManager::SetOnUserLeft(OnUserLeftCallback callback) {
    m_onUserLeft = callback;
}

void ChannelManager::SetOnUserMuted(OnUserMutedCallback callback) {
    m_onUserMuted = callback;
}

void ChannelManager::SetOnUserDeafened(OnUserDeafenedCallback callback) {
    m_onUserDeafened = callback;
}

void ChannelManager::SetOnUserModeratorChanged(OnUserModeratorChangedCallback callback) {
    m_onUserModeratorChanged = callback;
}

bool ChannelManager::ValidateChannelConfig(const ChannelConfig& config) const {
    if (config.name.empty()) {
        return false;
    }

    if (config.maxUsers < 0) {
        return false;
    }

    if (config.voiceActivityThreshold < 0.0f || config.voiceActivityThreshold > 1.0f) {
        return false;
    }

    return true;
}

bool ChannelManager::ValidateUserPermissions(const ChannelPermissions& permissions) const {
    // Verificar consistência das permissões
    if (permissions.canKick && !permissions.canModerate) {
        return false;
    }

    if (permissions.canBan && !permissions.canModerate) {
        return false;
    }

    return true;
}

} // namespace VoIP
} // namespace WYDBR 