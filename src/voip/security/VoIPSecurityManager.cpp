/**
 * @file VoIPSecurityManager.cpp
 * @brief Implementação do gerenciamento de segurança para VoIP
 */

#include "VoIPSecurityManager.h"
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <algorithm>
#include <stdexcept>

namespace WYDBR {
namespace VoIP {

VoIPSecurityManager::VoIPSecurityManager()
    : m_initialized(false)
{
}

VoIPSecurityManager::~VoIPSecurityManager()
{
    // Limpar chaves
    m_channelKeys.clear();
    m_userKeys.clear();
}

bool VoIPSecurityManager::Initialize(const SecurityConfig& config)
{
    std::lock_guard<std::mutex> lock(m_securityMutex);
    
    if (!ValidateConfig(config)) {
        return false;
    }
    
    m_config = config;
    
    if (m_config.enableEncryption && !InitializeEncryption()) {
        return false;
    }
    
    if (m_config.enableAuthentication && !InitializeAuthentication()) {
        return false;
    }
    
    m_initialized = true;
    return true;
}

std::vector<uint8_t> VoIPSecurityManager::EncryptData(const std::vector<uint8_t>& data, const std::string& key)
{
    if (!m_initialized || !m_config.enableEncryption) {
        return data;
    }
    
    std::lock_guard<std::mutex> lock(m_securityMutex);
    
    try {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            throw std::runtime_error("Failed to create cipher context");
        }
        
        // Gerar IV aleatório
        unsigned char iv[16];
        if (RAND_bytes(iv, sizeof(iv)) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to generate IV");
        }
        
        // Inicializar criptografia
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, 
            reinterpret_cast<const unsigned char*>(key.c_str()), iv) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize encryption");
        }
        
        // Preparar buffer de saída
        std::vector<uint8_t> encrypted(data.size() + EVP_MAX_BLOCK_LENGTH);
        int len1 = 0, len2 = 0;
        
        // Criptografar dados
        if (EVP_EncryptUpdate(ctx, encrypted.data(), &len1, 
            data.data(), static_cast<int>(data.size())) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to encrypt data");
        }
        
        // Finalizar criptografia
        if (EVP_EncryptFinal_ex(ctx, encrypted.data() + len1, &len2) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to finalize encryption");
        }
        
        EVP_CIPHER_CTX_free(ctx);
        
        // Adicionar IV ao início dos dados criptografados
        encrypted.insert(encrypted.begin(), iv, iv + sizeof(iv));
        encrypted.resize(len1 + len2 + sizeof(iv));
        
        return encrypted;
    }
    catch (const std::exception& e) {
        // Log error
        return data;
    }
}

std::vector<uint8_t> VoIPSecurityManager::DecryptData(const std::vector<uint8_t>& encryptedData, const std::string& key)
{
    if (!m_initialized || !m_config.enableEncryption) {
        return encryptedData;
    }
    
    std::lock_guard<std::mutex> lock(m_securityMutex);
    
    try {
        if (encryptedData.size() < 16) {
            throw std::runtime_error("Invalid encrypted data size");
        }
        
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            throw std::runtime_error("Failed to create cipher context");
        }
        
        // Extrair IV
        unsigned char iv[16];
        std::copy(encryptedData.begin(), encryptedData.begin() + 16, iv);
        
        // Inicializar descriptografia
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
            reinterpret_cast<const unsigned char*>(key.c_str()), iv) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize decryption");
        }
        
        // Preparar buffer de saída
        std::vector<uint8_t> decrypted(encryptedData.size() - 16);
        int len1 = 0, len2 = 0;
        
        // Descriptografar dados
        if (EVP_DecryptUpdate(ctx, decrypted.data(), &len1,
            encryptedData.data() + 16, static_cast<int>(encryptedData.size() - 16)) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to decrypt data");
        }
        
        // Finalizar descriptografia
        if (EVP_DecryptFinal_ex(ctx, decrypted.data() + len1, &len2) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to finalize decryption");
        }
        
        EVP_CIPHER_CTX_free(ctx);
        
        decrypted.resize(len1 + len2);
        return decrypted;
    }
    catch (const std::exception& e) {
        // Log error
        return encryptedData;
    }
}

bool VoIPSecurityManager::AuthenticateUser(ClientID clientId, const std::string& username, const std::string& password)
{
    if (!m_initialized || !m_config.enableUserAuthentication) {
        return true;
    }
    
    std::lock_guard<std::mutex> lock(m_securityMutex);
    
    // Implementar autenticação de usuário
    // TODO: Implementar autenticação real
    return true;
}

bool VoIPSecurityManager::AuthenticateChannel(ChannelID channelId, const std::string& password)
{
    if (!m_initialized || !m_config.enableChannelAuthentication) {
        return true;
    }
    
    std::lock_guard<std::mutex> lock(m_securityMutex);
    
    // Implementar autenticação de canal
    // TODO: Implementar autenticação real
    return true;
}

bool VoIPSecurityManager::ValidateIP(const std::string& ip)
{
    if (!m_initialized || !m_config.enableIPFiltering) {
        return true;
    }
    
    std::lock_guard<std::mutex> lock(m_securityMutex);
    return IsIPAllowed(ip);
}

std::string VoIPSecurityManager::GenerateKey()
{
    return GenerateRandomKey(32); // 256 bits
}

bool VoIPSecurityManager::SetChannelKey(ChannelID channelId, const std::string& key)
{
    if (!m_initialized || !m_config.enableChannelEncryption) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_securityMutex);
    m_channelKeys[channelId] = key;
    return true;
}

bool VoIPSecurityManager::SetUserKey(ClientID clientId, const std::string& key)
{
    if (!m_initialized || !m_config.enableEncryption) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_securityMutex);
    m_userKeys[clientId] = key;
    return true;
}

bool VoIPSecurityManager::IsEncryptionEnabled() const
{
    return m_initialized && m_config.enableEncryption;
}

bool VoIPSecurityManager::IsAuthenticationEnabled() const
{
    return m_initialized && m_config.enableAuthentication;
}

bool VoIPSecurityManager::IsChannelEncryptionEnabled() const
{
    return m_initialized && m_config.enableChannelEncryption;
}

bool VoIPSecurityManager::IsUserAuthenticationEnabled() const
{
    return m_initialized && m_config.enableUserAuthentication;
}

bool VoIPSecurityManager::IsChannelAuthenticationEnabled() const
{
    return m_initialized && m_config.enableChannelAuthentication;
}

bool VoIPSecurityManager::IsIPFilteringEnabled() const
{
    return m_initialized && m_config.enableIPFiltering;
}

const SecurityConfig& VoIPSecurityManager::GetConfig() const
{
    return m_config;
}

bool VoIPSecurityManager::UpdateConfig(const SecurityConfig& config)
{
    if (!ValidateConfig(config)) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_securityMutex);
    m_config = config;
    return true;
}

bool VoIPSecurityManager::InitializeEncryption()
{
    // Inicializar OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    
    // Verificar se o tipo de criptografia é suportado
    if (m_config.encryptionType != "AES-256-CBC") {
        return false;
    }
    
    return true;
}

bool VoIPSecurityManager::InitializeAuthentication()
{
    // Inicializar sistema de autenticação
    // TODO: Implementar inicialização real
    return true;
}

bool VoIPSecurityManager::ValidateConfig(const SecurityConfig& config) const
{
    if (config.enableEncryption && config.encryptionType.empty()) {
        return false;
    }
    
    if (config.enableIPFiltering && config.allowedIPs.empty()) {
        return false;
    }
    
    return true;
}

std::string VoIPSecurityManager::GenerateRandomKey(size_t length)
{
    std::vector<unsigned char> key(length);
    if (RAND_bytes(key.data(), static_cast<int>(length)) != 1) {
        return "";
    }
    
    return std::string(key.begin(), key.end());
}

bool VoIPSecurityManager::IsIPAllowed(const std::string& ip) const
{
    return std::find(m_config.allowedIPs.begin(), m_config.allowedIPs.end(), ip) != m_config.allowedIPs.end();
}

} // namespace VoIP
} // namespace WYDBR 