/**
 * @file AuthManager.h
 * @brief Gerenciador de autenticação com correções de segurança
 */

#ifndef WYDBR_AUTH_MANAGER_H
#define WYDBR_AUTH_MANAGER_H

#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <random>
#include <memory>

namespace WYDBR {

/**
 * Classe AuthManager - Implementa autenticação segura
 * 
 * Corrige a falha original de usar um salt global único, implementando:
 * - Salt específico por sessão
 * - Tokens com tempo de expiração
 * - Proteção contra ataques de força bruta
 * - Armazenamento seguro de senhas com bcrypt
 */
class AuthManager {
public:
    /**
     * Obtém instância (singleton)
     */
    static AuthManager& GetInstance();
    
    /**
     * Inicializa o sistema de autenticação
     */
    bool Initialize();
    
    /**
     * Gera um token de autenticação para um login bem-sucedido
     * 
     * @param accountId ID da conta
     * @param username Nome de usuário
     * @param passwordHash Hash da senha
     * @return Token de autenticação ou string vazia se a autenticação falhar
     */
    std::string GenerateAuthToken(const std::string& accountId, 
                                 const std::string& username,
                                 const std::string& passwordHash);
    
    /**
     * Verifica se um token de autenticação é válido
     * 
     * @param accountId ID da conta
     * @param token Token de autenticação
     * @return true se o token for válido
     */
    bool VerifyAuthToken(const std::string& accountId, const std::string& token);
    
    /**
     * Revoga um token de autenticação
     * 
     * @param accountId ID da conta
     * @param token Token de autenticação
     */
    void RevokeAuthToken(const std::string& accountId, const std::string& token);
    
    /**
     * Limpa tokens expirados
     */
    void CleanupExpiredTokens();
    
private:
    // Construtor privado (singleton)
    AuthManager();
    ~AuthManager();
    
    // Não permitir cópias
    AuthManager(const AuthManager&) = delete;
    AuthManager& operator=(const AuthManager&) = delete;
    
    struct SessionInfo {
        std::string token;
        std::string salt;
        std::chrono::system_clock::time_point expirationTime;
        int failedAttempts;
    };
    
    // Mapa de informações de sessão por account ID
    std::unordered_map<std::string, SessionInfo> m_sessions;
    
    // Mutex para operações thread-safe
    mutable std::mutex m_mutex;
    
    // Gerador de números aleatórios
    std::mt19937 m_rng;
    
    // Gera um salt aleatório
    std::string GenerateSalt();
    
    // Gera um token com base no salt da sessão
    std::string GenerateToken(const std::string& accountId, const std::string& salt);
};

} // namespace WYDBR

#endif // WYDBR_AUTH_MANAGER_H
