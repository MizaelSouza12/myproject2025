/**
 * @file AntiTamperManager.h
 * @brief Sistema de proteção contra adulteração e replay
 */

#ifndef WYDBR_ANTI_TAMPER_MANAGER_H
#define WYDBR_ANTI_TAMPER_MANAGER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>

namespace WYDBR {
namespace Security {

/**
 * Tipos de proteção contra adulteração
 */
enum class TamperProtectionType {
    CHECKSUM,        // Verificação de checksum
    HMAC,            // HMAC para verificação
    DIGITAL_SIGNATURE, // Assinatura digital
    ENCRYPTED        // Dados encriptados
};

/**
 * Níveis de proteção
 */
enum class ProtectionLevel {
    NONE,            // Sem proteção
    BASIC,           // Proteção básica
    NORMAL,          // Proteção normal
    HIGH,            // Alta proteção
    MAXIMUM          // Proteção máxima
};

/**
 * Configuração de proteção contra adulteração
 */
struct AntiTamperConfig {
    ProtectionLevel level;                 // Nível de proteção
    TamperProtectionType type;             // Tipo de proteção
    uint32_t nonceExpirationSeconds;       // Expiração de nonce em segundos
    uint32_t timestampToleranceSeconds;    // Tolerância de timestamp em segundos
    bool enforceSequenceVerification;      // Verificar sequência de pacotes
    bool detectReplayAttacks;              // Detectar ataques de replay
    uint32_t maxNonceCache;                // Tamanho máximo do cache de nonces
};

/**
 * Resultado de verificação anti-adulteração
 */
enum class TamperVerificationResult {
    VALID,                  // Dados válidos
    CHECKSUM_MISMATCH,      // Checksum não corresponde
    HMAC_INVALID,           // HMAC inválido
    SIGNATURE_INVALID,      // Assinatura inválida
    REPLAY_DETECTED,        // Ataque de replay detectado
    INVALID_SEQUENCE,       // Sequência inválida
    TIMESTAMP_INVALID,      // Timestamp inválido
    INTERNAL_ERROR          // Erro interno
};

/**
 * Classe de gerenciamento de proteção contra adulteração
 */
class AntiTamperManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static AntiTamperManager& GetInstance();
    
    /**
     * Inicializa o gerenciador
     * @param config Configuração de proteção
     * @return true se inicializado com sucesso
     */
    bool Initialize(const AntiTamperConfig& config);
    
    /**
     * Protege dados contra adulteração
     * @param data Dados a proteger
     * @param size Tamanho dos dados
     * @param sourceId ID da fonte (cliente/servidor)
     * @param packetType Tipo de pacote
     * @param sequenceNumber Número de sequência
     * @return Dados protegidos
     */
    std::vector<uint8_t> ProtectData(const uint8_t* data, size_t size, 
                                   uint32_t sourceId, uint16_t packetType, 
                                   uint32_t sequenceNumber);
    
    /**
     * Verifica proteção contra adulteração
     * @param data Dados protegidos
     * @param size Tamanho dos dados
     * @param sourceId ID da fonte (cliente/servidor)
     * @param packetType Tipo de pacote esperado
     * @param originalData Ponteiro para receber dados originais
     * @param originalSize Ponteiro para receber tamanho original
     * @return Resultado da verificação
     */
    TamperVerificationResult VerifyData(const uint8_t* data, size_t size,
                                      uint32_t sourceId, uint16_t packetType,
                                      std::vector<uint8_t>* originalData = nullptr);
    
    /**
     * Obtém a configuração atual
     * @return Configuração atual
     */
    AntiTamperConfig GetConfig() const;
    
    /**
     * Define a configuração
     * @param config Nova configuração
     */
    void SetConfig(const AntiTamperConfig& config);
    
    /**
     * Limpa nonces e sequências expirados
     * @return Número de entradas limpas
     */
    int CleanupExpiredEntries();
    
private:
    // Construtor e destrutor privados (singleton)
    AntiTamperManager();
    ~AntiTamperManager();
    
    // Não permitir cópias
    AntiTamperManager(const AntiTamperManager&) = delete;
    AntiTamperManager& operator=(const AntiTamperManager&) = delete;
    
    // Estrutura para rastreamento de sequência
    struct SequenceTracker {
        uint32_t lastSequence;       // Último número de sequência visto
        uint64_t lastTimestamp;      // Timestamp do último pacote
        std::unordered_map<uint64_t, uint64_t> nonces; // Mapa de nonces vistos
    };
    
    // Mapa de rastreadores por fonte e tipo
    std::unordered_map<uint64_t, SequenceTracker> m_sequenceTrackers;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Configuração
    AntiTamperConfig m_config;
    
    // Estado
    bool m_initialized;
    
    // Métodos privados
    uint32_t CalculateChecksum(const uint8_t* data, size_t size);
    std::vector<uint8_t> ComputeHMAC(const uint8_t* data, size_t size);
    std::vector<uint8_t> CreateSignature(const uint8_t* data, size_t size);
    bool VerifyChecksum(const uint8_t* data, size_t size, uint32_t checksum);
    bool VerifyHMAC(const uint8_t* data, size_t size, const std::vector<uint8_t>& hmac);
    bool VerifySignature(const uint8_t* data, size_t size, const std::vector<uint8_t>& signature);
    bool IsNonceUsed(uint32_t sourceId, uint16_t packetType, uint64_t nonce);
    void RecordNonce(uint32_t sourceId, uint16_t packetType, uint64_t nonce);
    bool IsSequenceValid(uint32_t sourceId, uint16_t packetType, uint32_t sequence);
    void UpdateSequence(uint32_t sourceId, uint16_t packetType, uint32_t sequence);
    uint64_t GetTrackerKey(uint32_t sourceId, uint16_t packetType);
    bool IsTimestampValid(uint64_t timestamp);
    uint64_t GenerateNonce();
};

} // namespace Security
} // namespace WYDBR

#endif // WYDBR_ANTI_TAMPER_MANAGER_H
