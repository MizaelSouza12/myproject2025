/**
 * @file AuthenticationManager.cpp
 * @brief Implementação do sistema de autenticação para VoIP
 */

#include "AuthenticationManager.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

namespace WYDBR {
namespace VoIP {

AuthenticationManager& AuthenticationManager::GetInstance()
{
    static AuthenticationManager instance;
    return instance;
}

AuthenticationManager::AuthenticationManager()
    : m_initialized(false)
    , m_maxFailedAttempts(5)
    , m_lockoutDuration(std::chrono::minutes(30))
    , m_passwordExpiryDays(std::chrono::days(90))
{
}

AuthenticationManager::~AuthenticationManager()
{
    Shutdown();
}

bool AuthenticationManager::Initialize()
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

void AuthenticationManager::Shutdown()
{
    std::lock_guard<std::mutex> lock(m_authMutex);
    m_users.clear();
    m_channels.clear();
    m_initialized = false;
}

bool AuthenticationManager::AuthenticateUser(const std::string& username, const std::string& password,
                                          const std::string& ip, ClientID& clientId)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_users.find(username);
    if (it == m_users.end()) {
        LogAuthenticationEvent(username, ip, false, "User not found");
        return false;
    }

    UserCredentials& user = it->second;

    // Verificar se o usuário está bloqueado
    if (!user.enabled) {
        LogAuthenticationEvent(username, ip, false, "User is locked");
        return false;
    }

    // Verificar se a senha expirou
    if (std::chrono::system_clock::now() > user.passwordExpiry) {
        LogAuthenticationEvent(username, ip, false, "Password expired");
        return false;
    }

    // Verificar se o usuário está bloqueado por tentativas falhas
    if (user.failedLoginAttempts >= m_maxFailedAttempts) {
        auto lockoutEnd = user.lastLogin + m_lockoutDuration;
        if (std::chrono::system_clock::now() < lockoutEnd) {
            LogAuthenticationEvent(username, ip, false, "Account locked due to failed attempts");
            return false;
        }
        user.failedLoginAttempts = 0;
    }

    // Verificar senha
    std::string hashedPassword = HashPassword(password, user.salt);
    if (hashedPassword != user.passwordHash) {
        user.failedLoginAttempts++;
        user.lastLogin = std::chrono::system_clock::now();
        LogAuthenticationEvent(username, ip, false, "Invalid password");
        return false;
    }

    // Autenticação bem-sucedida
    user.failedLoginAttempts = 0;
    user.lastLogin = std::chrono::system_clock::now();
    LogAuthenticationEvent(username, ip, true);

    // Gerar ID do cliente
    clientId = std::hash<std::string>{}(username + std::to_string(user.lastLogin.time_since_epoch().count()));

    return true;
}

bool AuthenticationManager::ChangePassword(const std::string& username, const std::string& oldPassword,
                                         const std::string& newPassword)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_users.find(username);
    if (it == m_users.end()) {
        return false;
    }

    UserCredentials& user = it->second;

    // Verificar senha antiga
    std::string hashedOldPassword = HashPassword(oldPassword, user.salt);
    if (hashedOldPassword != user.passwordHash) {
        return false;
    }

    // Validar nova senha
    if (!ValidatePassword(newPassword)) {
        return false;
    }

    // Atualizar senha
    user.salt = GenerateSalt();
    user.passwordHash = HashPassword(newPassword, user.salt);
    user.passwordExpiry = std::chrono::system_clock::now() + m_passwordExpiryDays;

    return true;
}

bool AuthenticationManager::ResetPassword(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_users.find(username);
    if (it == m_users.end()) {
        return false;
    }

    UserCredentials& user = it->second;

    // Gerar nova senha aleatória
    std::string newPassword = GenerateSalt().substr(0, 8);
    user.salt = GenerateSalt();
    user.passwordHash = HashPassword(newPassword, user.salt);
    user.passwordExpiry = std::chrono::system_clock::now() + m_passwordExpiryDays;

    // TODO: Enviar nova senha para o usuário

    return true;
}

bool AuthenticationManager::LockUser(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_users.find(username);
    if (it == m_users.end()) {
        return false;
    }

    it->second.enabled = false;
    return true;
}

bool AuthenticationManager::UnlockUser(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_users.find(username);
    if (it == m_users.end()) {
        return false;
    }

    it->second.enabled = true;
    it->second.failedLoginAttempts = 0;
    return true;
}

bool AuthenticationManager::IsUserLocked(const std::string& username) const
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    auto it = m_users.find(username);
    if (it == m_users.end()) {
        return false;
    }

    return !it->second.enabled;
}

bool AuthenticationManager::AuthenticateChannel(const std::string& channelName, const std::string& password,
                                              ClientID clientId, ChannelID& channelId)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_channels.find(channelName);
    if (it == m_channels.end()) {
        return false;
    }

    ChannelCredentials& channel = it->second;

    // Verificar se o canal está habilitado
    if (!channel.enabled) {
        return false;
    }

    // Verificar se o canal requer autenticação
    if (!channel.requireAuthentication) {
        channelId = std::hash<std::string>{}(channelName);
        return true;
    }

    // Verificar senha
    std::string hashedPassword = HashPassword(password, channel.salt);
    if (hashedPassword != channel.passwordHash) {
        return false;
    }

    // Gerar ID do canal
    channelId = std::hash<std::string>{}(channelName);

    return true;
}

bool AuthenticationManager::ChangeChannelPassword(const std::string& channelName, const std::string& oldPassword,
                                                const std::string& newPassword)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_channels.find(channelName);
    if (it == m_channels.end()) {
        return false;
    }

    ChannelCredentials& channel = it->second;

    // Verificar senha antiga
    std::string hashedOldPassword = HashPassword(oldPassword, channel.salt);
    if (hashedOldPassword != channel.passwordHash) {
        return false;
    }

    // Validar nova senha
    if (!ValidatePassword(newPassword)) {
        return false;
    }

    // Atualizar senha
    channel.salt = GenerateSalt();
    channel.passwordHash = HashPassword(newPassword, channel.salt);

    return true;
}

bool AuthenticationManager::LockChannel(const std::string& channelName)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_channels.find(channelName);
    if (it == m_channels.end()) {
        return false;
    }

    it->second.enabled = false;
    return true;
}

bool AuthenticationManager::UnlockChannel(const std::string& channelName)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_channels.find(channelName);
    if (it == m_channels.end()) {
        return false;
    }

    it->second.enabled = true;
    return true;
}

bool AuthenticationManager::IsChannelLocked(const std::string& channelName) const
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    auto it = m_channels.find(channelName);
    if (it == m_channels.end()) {
        return false;
    }

    return !it->second.enabled;
}

bool AuthenticationManager::AddUser(const std::string& username, const std::string& password,
                                  const std::vector<std::string>& roles)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized || m_users.find(username) != m_users.end()) {
        return false;
    }

    if (!ValidatePassword(password)) {
        return false;
    }

    UserCredentials user;
    user.username = username;
    user.salt = GenerateSalt();
    user.passwordHash = HashPassword(password, user.salt);
    user.roles = roles;
    user.enabled = true;
    user.lastLogin = std::chrono::system_clock::now();
    user.passwordExpiry = user.lastLogin + m_passwordExpiryDays;
    user.failedLoginAttempts = 0;

    m_users[username] = user;
    return true;
}

bool AuthenticationManager::RemoveUser(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    return m_users.erase(username) > 0;
}

bool AuthenticationManager::UpdateUserRoles(const std::string& username, const std::vector<std::string>& roles)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_users.find(username);
    if (it == m_users.end()) {
        return false;
    }

    it->second.roles = roles;
    return true;
}

bool AuthenticationManager::HasRole(const std::string& username, const std::string& role) const
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    auto it = m_users.find(username);
    if (it == m_users.end()) {
        return false;
    }

    return std::find(it->second.roles.begin(), it->second.roles.end(), role) != it->second.roles.end();
}

std::vector<std::string> AuthenticationManager::GetUserRoles(const std::string& username) const
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    auto it = m_users.find(username);
    if (it == m_users.end()) {
        return std::vector<std::string>();
    }

    return it->second.roles;
}

bool AuthenticationManager::AddChannel(const std::string& channelName, const std::string& password,
                                     const std::vector<std::string>& allowedRoles, size_t maxUsers)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized || m_channels.find(channelName) != m_channels.end()) {
        return false;
    }

    if (!ValidatePassword(password)) {
        return false;
    }

    ChannelCredentials channel;
    channel.name = channelName;
    channel.salt = GenerateSalt();
    channel.passwordHash = HashPassword(password, channel.salt);
    channel.enabled = true;
    channel.allowedRoles = allowedRoles;
    channel.maxUsers = maxUsers;
    channel.requireAuthentication = !password.empty();

    m_channels[channelName] = channel;
    return true;
}

bool AuthenticationManager::RemoveChannel(const std::string& channelName)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    return m_channels.erase(channelName) > 0;
}

bool AuthenticationManager::UpdateChannelRoles(const std::string& channelName,
                                             const std::vector<std::string>& allowedRoles)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!m_initialized) {
        return false;
    }

    auto it = m_channels.find(channelName);
    if (it == m_channels.end()) {
        return false;
    }

    it->second.allowedRoles = allowedRoles;
    return true;
}

bool AuthenticationManager::IsUserAllowedInChannel(const std::string& username,
                                                 const std::string& channelName) const
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    auto userIt = m_users.find(username);
    if (userIt == m_users.end()) {
        return false;
    }

    auto channelIt = m_channels.find(channelName);
    if (channelIt == m_channels.end()) {
        return false;
    }

    // Se não houver roles permitidas, qualquer usuário pode entrar
    if (channelIt->second.allowedRoles.empty()) {
        return true;
    }

    // Verificar se o usuário tem alguma das roles permitidas
    for (const auto& role : userIt->second.roles) {
        if (std::find(channelIt->second.allowedRoles.begin(),
                     channelIt->second.allowedRoles.end(), role) != channelIt->second.allowedRoles.end()) {
            return true;
        }
    }

    return false;
}

bool AuthenticationManager::UserExists(const std::string& username) const
{
    std::lock_guard<std::mutex> lock(m_authMutex);
    return m_users.find(username) != m_users.end();
}

bool AuthenticationManager::ChannelExists(const std::string& channelName) const
{
    std::lock_guard<std::mutex> lock(m_authMutex);
    return m_channels.find(channelName) != m_channels.end();
}

std::vector<std::string> AuthenticationManager::GetAllUsers() const
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    std::vector<std::string> users;
    users.reserve(m_users.size());

    for (const auto& pair : m_users) {
        users.push_back(pair.first);
    }

    return users;
}

std::vector<std::string> AuthenticationManager::GetAllChannels() const
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    std::vector<std::string> channels;
    channels.reserve(m_channels.size());

    for (const auto& pair : m_channels) {
        channels.push_back(pair.first);
    }

    return channels;
}

size_t AuthenticationManager::GetUserCount() const
{
    std::lock_guard<std::mutex> lock(m_authMutex);
    return m_users.size();
}

size_t AuthenticationManager::GetChannelCount() const
{
    std::lock_guard<std::mutex> lock(m_authMutex);
    return m_channels.size();
}

std::string AuthenticationManager::HashPassword(const std::string& password, const std::string& salt)
{
    std::string saltedPassword = password + salt;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, saltedPassword.c_str(), saltedPassword.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

std::string AuthenticationManager::GenerateSalt()
{
    unsigned char salt[32];
    RAND_bytes(salt, sizeof(salt));

    std::stringstream ss;
    for (int i = 0; i < sizeof(salt); i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(salt[i]);
    }

    return ss.str();
}

bool AuthenticationManager::ValidatePassword(const std::string& password)
{
    // Verificar comprimento mínimo
    if (password.length() < 8) {
        return false;
    }

    // Verificar se contém pelo menos uma letra maiúscula
    bool hasUpper = false;
    // Verificar se contém pelo menos uma letra minúscula
    bool hasLower = false;
    // Verificar se contém pelo menos um número
    bool hasNumber = false;
    // Verificar se contém pelo menos um caractere especial
    bool hasSpecial = false;

    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasNumber = true;
        else hasSpecial = true;
    }

    return hasUpper && hasLower && hasNumber && hasSpecial;
}

void AuthenticationManager::LogAuthenticationEvent(const std::string& username, const std::string& ip,
                                                 bool success, const std::string& details)
{
    SecurityEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.type = success ? "LoginSuccess" : "LoginFailure";
    event.source = username;
    event.details = details.empty() ? (success ? "Successful login" : "Failed login") : details;
    event.severity = success ? SecurityLogLevel::INFO : SecurityLogLevel::WARNING;

    SecurityMonitor::GetInstance().AddEvent(event);

    if (!success) {
        SecurityMonitor::GetInstance().AddFailedLogin(username, ip);
    }
}

} // namespace VoIP
} // namespace WYDBR 