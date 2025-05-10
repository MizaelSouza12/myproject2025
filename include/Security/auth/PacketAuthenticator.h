/**
 * @file PacketAuthenticator.h
 * @brief Sistema de autenticação e assinatura de pacotes
 */

#ifndef WYDBR_PACKET_AUTHENTICATOR_H
#define WYDBR_PACKET_AUTHENTICATOR_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace WYDBR {
namespace Security {

/**
 * Nível de autenticação de pacotes
 */
enum class AuthenticationLevel {
    NONE,          // Sem autenticação
    BASIC,         // Autenticação básica (checksum)
    HMAC,          // HMAC para autenticação
    SIGNATURE      // Assinatura completa
};

/**
 * Algoritmo de hash para autenticação
 */
enum class HashAlgorithm {
    NONE,          // Sem hash
    MD5,           // MD5 (não recomendado, apenas para compatibilidade)
    SHA1,          // SHA-1 (não recomendado, apenas para compatibilidade)
    SHA256,        // SHA-256
    SHA512,        // SHA-512
    BLAKE2B        // BLAKE2b
};

/**
 * Resultado de verificação de pacote
 */
enum class VerificationResult {
    VALID,                  // Pacote válido
    INVALID_SIGNATURE,      // Assinatura inválida
    INVALID_TIMESTAMP,      // Timestamp inválido (muito antigo ou futuro)
    REPLAY_ATTACK,          // Ataque de replay detectado
    INVALID_KEY,            // Chave inválida ou expirada
    TAMPERED,               // Pacote adulterado
    INVALID_FORMAT,         // Formato inválido
    INTERNAL_ERROR          // Erro interno
};

/**
 * Configuração de autenticação
 */
struct AuthenticationConfig {
    AuthenticationLevel level;           // Nível de autenticação
    HashAlgorithm hashAlgorithm;         // Algoritmo de hash
    uint32_t timestampToleranceMs;       // Tolerância de timestamp em ms
    uint32_t nonceExpirationMs;          // Expiração de nonce em ms
    uint32_t maxNonceCache;              // Máximo de nonces em cache
    bool requireTimestamp;               // Requer timestamp
    bool requireNonce;                   // Requer nonce
};

/**
 * Classe para autenticação de pacotes
 */
class PacketAuthenticator {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o autenticador
     */
    static PacketAuthenticator& GetInstance();
    
    /**
     * Inicializa o autenticador
     * @param config Configuração de autenticação
     * @return true se inicializado com sucesso
     */
    bool Initialize(const AuthenticationConfig& config);
    
    /**
     * Autentica um pacote
     * @param packetData Dados originais do pacote
     * @param packetSize Tamanho do pacote
     * @param packetType Tipo do pacote
     * @param sequenceNumber Número de sequência
     * @param keyId ID da chave de autenticação (opcional)
     * @return Pacote autenticado
     */
    std::vector<uint8_t> AuthenticatePacket(const uint8_t* packetData, size_t packetSize, 
                                          uint16_t packetType, uint32_t sequenceNumber,
                                          const std::string& keyId = "");
    
    /**
     * Verifica autenticação de um pacote
     * @param packetData Dados do pacote com autenticação
     * @param packetSize Tamanho total do pacote
     * @param packetType Tipo de pacote esperado
     * @param originalData Ponteiro para receber dados originais (sem autenticação)
     * @param originalSize Ponteiro para receber tamanho original
     * @return Resultado da verificação
     */
    VerificationResult VerifyPacket(const uint8_t* packetData, size_t packetSize,
                                  uint16_t packetType, 
                                  std::vector<uint8_t>* originalData = nullptr);
    
    /**
     * Gera uma nova chave de autenticação
     * @param expirationSeconds Tempo de expiração em segundos (0 = sem expiração)
     * @return ID da chave gerada
     */
    std::string GenerateAuthKey(uint64_t expirationSeconds = 0);
    
    /**
     * Revoga uma chave de autenticação
     * @param keyId ID da chave
     * @return true se revogada com sucesso
     */
    bool RevokeAuthKey(const std::string& keyId);
    
    /**
     * Define a chave padrão
     * @param keyId ID da chave
     * @return true se definida com sucesso
     */
    bool SetDefaultKey(const std::string& keyId);
    
    /**
     * Obtém a configuração atual
     * @return Configuração atual
     */
    AuthenticationConfig GetConfig() const;
    
    /**
     * Define a configuração
     * @param config Nova configuração
     */
    void SetConfig(const AuthenticationConfig& config);
    
    /**
     * Limpa nonces expirados
     * @return Número de nonces limpos
     */
    int CleanupExpiredNonces();
    
private:
    // Construtor e destrutor privados (singleton)
    PacketAuthenticator();
    ~PacketAuthenticator();
    
    // Não permitir cópias
    PacketAuthenticator(const PacketAuthenticator&) = delete;
    PacketAuthenticator& operator=(const PacketAuthenticator&) = delete;
    
    // Estrutura de chave de autenticação
    struct AuthKey {
        std::vector<uint8_t> key;        // Dados da chave
        uint64_t creationTime;           // Timestamp de criação
        uint64_t expirationTime;         // Timestamp de expiração (0 = sem expiração)
        std::string keyId;               // ID único da chave
        bool isValid;                    // Chave válida
    };
    
    // Estrutura de nonce
    struct NonceEntry {
        uint64_t nonce;                  // Valor do nonce
        uint64_t timestamp;              // Timestamp de recebimento
        uint16_t packetType;             // Tipo de pacote
    };
    
    // Mapa de chaves por ID
    std::unordered_map<std::string, AuthKey> m_keys;
    
    // Cache de nonces para prevenir replay
    std::unordered_map<uint64_t, NonceEntry> m_nonceCache;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    mutable std::mutex m_nonceMutex;
    
    // Configuração
    AuthenticationConfig m_config;
    
    // Chave padrão
    std::string m_defaultKeyId;
    
    // Estado
    bool m_initialized;
    
    // Métodos privados
    std::vector<uint8_t> ComputeHMAC(const uint8_t* data, size_t size, const AuthKey& key);
    std::vector<uint8_t> ComputeSignature(const uint8_t* data, size_t size, const AuthKey& key);
    bool VerifyHMAC(const uint8_t* data, size_t size, const std::vector<uint8_t>& hmac, const AuthKey& key);
    bool VerifySignature(const uint8_t* data, size_t size, const std::vector<uint8_t>& signature, const AuthKey& key);
    bool IsNonceUsed(uint64_t nonce, uint16_t packetType);
    void RecordNonce(uint64_t nonce, uint16_t packetType);
    bool IsTimestampValid(uint64_t timestamp);
    uint64_t GenerateNonce();
    std::string GenerateKeyId() const;
    AuthKey GenerateKeyInternal(uint64_t expirationSeconds);
};

} // namespace Security
} // namespace WYDBR

#endif // WYDBR_PACKET_AUTHENTICATOR_H
