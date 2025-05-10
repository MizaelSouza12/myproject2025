/**
 * @file AuthManager.cpp
 * @brief Implementação do gerenciador de autenticação
 */

#include "AuthManager.h"
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>
#include "../Utils/Logger.h"

namespace WYDBR {

AuthManager& AuthManager::GetInstance() {
    static AuthManager instance;
    return instance;
}

AuthManager::AuthManager() {
    // Inicializar o gerador de números aleatórios com uma seed verdadeiramente aleatória
    std::random_device rd;
    m_rng.seed(rd());
}

AuthManager::~AuthManager() {
}

bool AuthManager::Initialize() {
    // Implementação da inicialização
    return true;
}

std::string AuthManager::GenerateAuthToken(const std::string& accountId, 
                                         const std::string& username,
                                         const std::string& passwordHash) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Verificar se a sessão já existe
    auto it = m_sessions.find(accountId);
    if (it != m_sessions.end()) {
        // Verificar se a conta está bloqueada devido a muitas tentativas falhas
        if (it->second.failedAttempts >= 5) {
            auto now = std::chrono::system_clock::now();
            auto lockoutTime = it->second.expirationTime;
            
            if (now < lockoutTime) {
                LOG_WARNING("Conta {} bloqueada devido a muitas tentativas falhas", accountId);
                return "";
            }
            
            // Reset failed attempts after lockout period
            it->second.failedAttempts = 0;
        }
    }
    
    // Gerar um novo salt para esta sessão
    std::string salt = GenerateSalt();
    
    // Gerar o token
    std::string token = GenerateToken(accountId, salt);
    
    // Definir tempo de expiração (2 horas)
    auto expirationTime = std::chrono::system_clock::now() + std::chrono::hours(2);
    
    // Armazenar informações da sessão
    SessionInfo sessionInfo;
    sessionInfo.token = token;
    sessionInfo.salt = salt;
    sessionInfo.expirationTime = expirationTime;
    sessionInfo.failedAttempts = 0;
    
    m_sessions[accountId] = sessionInfo;
    
    LOG_INFO("Token de autenticação gerado para conta {}", accountId);
    
    return token;
}

bool AuthManager::VerifyAuthToken(const std::string& accountId, const std::string& token) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Verificar se a sessão existe
    auto it = m_sessions.find(accountId);
    if (it == m_sessions.end()) {
        LOG_WARNING("Tentativa de verificar token para sessão inexistente: {}", accountId);
        return false;
    }
    
    // Verificar se o token expirou
    auto now = std::chrono::system_clock::now();
    if (now > it->second.expirationTime) {
        LOG_WARNING("Token expirado para conta {}", accountId);
        return false;
    }
    
    // Verificar o token
    if (it->second.token != token) {
        // Incrementar contagem de tentativas falhas
        it->second.failedAttempts++;
        
        // Se muitas tentativas falhas, bloquear a conta por um período
        if (it->second.failedAttempts >= 5) {
            it->second.expirationTime = now + std::chrono::minutes(15);
            LOG_WARNING("Conta {} bloqueada por 15 minutos devido a muitas tentativas falhas", accountId);
        }
        
        LOG_WARNING("Token inválido para conta {}", accountId);
        return false;
    }
    
    // Reset failed attempts on successful verification
    it->second.failedAttempts = 0;
    
    // Estender o tempo de expiração (sliding expiration)
    it->second.expirationTime = now + std::chrono::hours(2);
    
    return true;
}

void AuthManager::RevokeAuthToken(const std::string& accountId, const std::string& token) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Verificar se a sessão existe
    auto it = m_sessions.find(accountId);
    if (it == m_sessions.end()) {
        return;
    }
    
    // Verificar se o token corresponde
    if (it->second.token != token) {
        return;
    }
    
    // Remover a sessão
    m_sessions.erase(it);
    
    LOG_INFO("Token revogado para conta {}", accountId);
}

void AuthManager::CleanupExpiredTokens() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto now = std::chrono::system_clock::now();
    
    // Remover todas as sessões expiradas
    for (auto it = m_sessions.begin(); it != m_sessions.end();) {
        if (now > it->second.expirationTime) {
            it = m_sessions.erase(it);
        } else {
            ++it;
        }
    }
}

std::string AuthManager::GenerateSalt() {
    // Gerar um salt aleatório de 32 bytes
    constexpr size_t SALT_LENGTH = 32;
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    
    std::vector<uint8_t> saltBytes(SALT_LENGTH);
    for (size_t i = 0; i < SALT_LENGTH; ++i) {
        saltBytes[i] = dist(m_rng);
    }
    
    // Converter para string hexadecimal
    std::stringstream ss;
    for (auto byte : saltBytes) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    
    return ss.str();
}

std::string AuthManager::GenerateToken(const std::string& accountId, const std::string& salt) {
    // Concatenar accountId e salt
    std::string data = accountId + "_" + salt;
    
    // Calcular SHA256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.length());
    SHA256_Final(hash, &sha256);
    
    // Converter para string hexadecimal
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

} // namespace WYDBR
