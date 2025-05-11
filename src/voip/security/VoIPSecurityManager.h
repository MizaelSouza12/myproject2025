/**
 * @file VoIPSecurityManager.h
 * @brief Gerenciamento de segurança para VoIP
 */

#ifndef WYDBR_VOIP_SECURITY_MANAGER_H
#define WYDBR_VOIP_SECURITY_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

namespace WYDBR {
namespace VoIP {

using ClientID = uint32_t;
using ChannelID = uint32_t;

struct SecurityConfig {
    bool enableEncryption;
    std::string encryptionType;
    bool enableAuthentication;
    bool enableChannelEncryption;
    bool enableUserAuthentication;
    bool enableChannelAuthentication;
    bool enableIPFiltering;
    std::vector<std::string> allowedIPs;
};

class VoIPSecurityManager {
public:
    VoIPSecurityManager();
    ~VoIPSecurityManager();

    bool Initialize(const SecurityConfig& config);
    
    // Criptografia
    std::vector<uint8_t> EncryptData(const std::vector<uint8_t>& data, const std::string& key);
    std::vector<uint8_t> DecryptData(const std::vector<uint8_t>& encryptedData, const std::string& key);
    
    // Autenticação
    bool AuthenticateUser(ClientID clientId, const std::string& username, const std::string& password);
    bool AuthenticateChannel(ChannelID channelId, const std::string& password);
    bool ValidateIP(const std::string& ip);
    
    // Gerenciamento de chaves
    std::string GenerateKey();
    bool SetChannelKey(ChannelID channelId, const std::string& key);
    bool SetUserKey(ClientID clientId, const std::string& key);
    
    // Verificações de segurança
    bool IsEncryptionEnabled() const;
    bool IsAuthenticationEnabled() const;
    bool IsChannelEncryptionEnabled() const;
    bool IsUserAuthenticationEnabled() const;
    bool IsChannelAuthenticationEnabled() const;
    bool IsIPFilteringEnabled() const;
    
    // Configuração
    const SecurityConfig& GetConfig() const;
    bool UpdateConfig(const SecurityConfig& config);

private:
    SecurityConfig m_config;
    std::unordered_map<ChannelID, std::string> m_channelKeys;
    std::unordered_map<ClientID, std::string> m_userKeys;
    mutable std::mutex m_securityMutex;
    
    bool m_initialized;
    
    // Funções auxiliares
    bool InitializeEncryption();
    bool InitializeAuthentication();
    bool ValidateConfig(const SecurityConfig& config) const;
    std::string GenerateRandomKey(size_t length);
    bool IsIPAllowed(const std::string& ip) const;
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_SECURITY_MANAGER_H 