/**
 * @file AuthenticationManager.h
 * @brief Sistema de autenticação para VoIP
 */

#ifndef WYDBR_VOIP_AUTHENTICATION_MANAGER_H
#define WYDBR_VOIP_AUTHENTICATION_MANAGER_H

#include "SecurityLogger.h"
#include "SecurityMonitor.h"
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <vector>
#include <chrono>

namespace WYDBR {
namespace VoIP {

using ClientID = uint32_t;
using ChannelID = uint32_t;

struct UserCredentials {
    std::string username;
    std::string passwordHash;
    std::string salt;
    std::vector<std::string> roles;
    bool enabled;
    std::chrono::system_clock::time_point lastLogin;
    std::chrono::system_clock::time_point passwordExpiry;
    size_t failedLoginAttempts;
};

struct ChannelCredentials {
    std::string name;
    std::string passwordHash;
    std::string salt;
    bool enabled;
    std::vector<std::string> allowedRoles;
    size_t maxUsers;
    bool requireAuthentication;
};

class AuthenticationManager {
public:
    static AuthenticationManager& GetInstance();

    bool Initialize();
    void Shutdown();

    // Autenticação de usuário
    bool AuthenticateUser(const std::string& username, const std::string& password, 
                         const std::string& ip, ClientID& clientId);
    bool ChangePassword(const std::string& username, const std::string& oldPassword,
                       const std::string& newPassword);
    bool ResetPassword(const std::string& username);
    bool LockUser(const std::string& username);
    bool UnlockUser(const std::string& username);
    bool IsUserLocked(const std::string& username) const;

    // Autenticação de canal
    bool AuthenticateChannel(const std::string& channelName, const std::string& password,
                            ClientID clientId, ChannelID& channelId);
    bool ChangeChannelPassword(const std::string& channelName, const std::string& oldPassword,
                              const std::string& newPassword);
    bool LockChannel(const std::string& channelName);
    bool UnlockChannel(const std::string& channelName);
    bool IsChannelLocked(const std::string& channelName) const;

    // Gerenciamento de usuários
    bool AddUser(const std::string& username, const std::string& password,
                const std::vector<std::string>& roles = {});
    bool RemoveUser(const std::string& username);
    bool UpdateUserRoles(const std::string& username, const std::vector<std::string>& roles);
    bool HasRole(const std::string& username, const std::string& role) const;
    std::vector<std::string> GetUserRoles(const std::string& username) const;

    // Gerenciamento de canais
    bool AddChannel(const std::string& channelName, const std::string& password,
                   const std::vector<std::string>& allowedRoles = {}, size_t maxUsers = 0);
    bool RemoveChannel(const std::string& channelName);
    bool UpdateChannelRoles(const std::string& channelName, 
                           const std::vector<std::string>& allowedRoles);
    bool IsUserAllowedInChannel(const std::string& username, const std::string& channelName) const;

    // Consultas
    bool UserExists(const std::string& username) const;
    bool ChannelExists(const std::string& channelName) const;
    std::vector<std::string> GetAllUsers() const;
    std::vector<std::string> GetAllChannels() const;
    size_t GetUserCount() const;
    size_t GetChannelCount() const;

private:
    AuthenticationManager();
    ~AuthenticationManager();

    AuthenticationManager(const AuthenticationManager&) = delete;
    AuthenticationManager& operator=(const AuthenticationManager&) = delete;

    std::string HashPassword(const std::string& password, const std::string& salt);
    std::string GenerateSalt();
    bool ValidatePassword(const std::string& password);
    void LogAuthenticationEvent(const std::string& username, const std::string& ip,
                               bool success, const std::string& details = "");

    std::unordered_map<std::string, UserCredentials> m_users;
    std::unordered_map<std::string, ChannelCredentials> m_channels;
    mutable std::mutex m_authMutex;
    bool m_initialized;
    size_t m_maxFailedAttempts;
    std::chrono::seconds m_lockoutDuration;
    std::chrono::days m_passwordExpiryDays;
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_AUTHENTICATION_MANAGER_H 