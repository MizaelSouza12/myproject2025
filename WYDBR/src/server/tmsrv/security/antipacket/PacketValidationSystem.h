/**
 * PacketValidationSystem.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/security/antipacket/PacketValidationSystem.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PACKETVALIDATIONSYSTEM_H
#define PACKETVALIDATIONSYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <bitset>
#include <array>
#include <deque>

#include "../../../../core/WYDTypes.h"
#include "../SecurityTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file PacketValidationSystem.h
 * @brief Sistema de validação e proteção contra injeção de pacotes
 * 
 * Este arquivo contém a definição do sistema de validação de pacotes,
 * que implementa múltiplas camadas de proteção contra injeção e modificação
 * de pacotes, um dos métodos mais comuns de hacking no WYD original.
 */

namespace wyd {
namespace server {
namespace security {
namespace antipacket {

/**
 * @brief Configuração do sistema de validação de pacotes
 */
struct PacketValidationConfig {
    bool enabled;                          // Habilitado
    bool autoRespond;                      // Responder automaticamente
    bool logViolations;                    // Registrar violações
    bool useChecksum;                      // Usar checksum
    bool useSequenceValidation;            // Usar validação de sequência
    bool useContentValidation;             // Usar validação de conteúdo
    bool usePacketSignatures;              // Usar assinaturas de pacote
    bool useRateLimiting;                  // Usar limitação de taxa
    bool useDynamicEncryption;             // Usar criptografia dinâmica
    bool usePatternDetection;              // Usar detecção de padrão
    DWORD maxViolationsBeforeAction;      // Máximo de violações antes de ação
    DWORD checksumSeed;                   // Seed de checksum
    DWORD sequenceWindowSize;             // Tamanho da janela de sequência
    SecurityResponseType violationResponse; // Resposta a violação
    std::vector<WORD> exemptPacketTypes;    // Tipos de pacote isentos
    
    PacketValidationConfig()
        : enabled(true)
        , autoRespond(true)
        , logViolations(true)
        , useChecksum(true)
        , useSequenceValidation(true)
        , useContentValidation(true)
        , usePacketSignatures(true)
        , useRateLimiting(true)
        , useDynamicEncryption(true)
        , usePatternDetection(true)
        , maxViolationsBeforeAction(3)
        , checksumSeed(0x12345678)
        , sequenceWindowSize(100)
        , violationResponse(SecurityResponseType::DISCONNECT)
    {
    }
};

/**
 * @brief Tipo de violação de pacote
 */
enum class PacketViolationType : BYTE {
    CHECKSUM_MISMATCH = 0,      // Checksum inválido
    SEQUENCE_INVALID = 1,       // Sequência inválida
    SIZE_MISMATCH = 2,          // Tamanho inválido
    CONTENT_INVALID = 3,        // Conteúdo inválido
    RATE_EXCEEDED = 4,          // Taxa excedida
    ENCRYPTION_INVALID = 5,     // Criptografia inválida
    SIGNATURE_MATCH = 6,        // Assinatura correspondente
    PATTERN_DETECTED = 7,       // Padrão detectado
    TIMING_ANOMALY = 8,         // Anomalia de timing
    FORMAT_INVALID = 9,         // Formato inválido
    CUSTOM = 10,                // Personalizado
};

/**
 * @brief Contexto de validação de pacote
 */
struct PacketValidationContext {
    DWORD connectionID;         // ID da conexão
    DWORD accountID;            // ID da conta
    DWORD characterID;          // ID do personagem
    WORD packetType;            // Tipo de pacote
    size_t packetSize;          // Tamanho do pacote
    DWORD sequence;             // Sequência
    DWORD checksum;             // Checksum
    DWORD timestamp;            // Timestamp
    const void* data;            // Dados
    std::string ipAddress;       // Endereço IP
    DWORD sessionID;            // ID da sessão
    
    PacketValidationContext()
        : connectionID(0)
        , accountID(0)
        , characterID(0)
        , packetType(0)
        , packetSize(0)
        , sequence(0)
        , checksum(0)
        , timestamp(0)
        , data(nullptr)
        , ipAddress("")
        , sessionID(0)
    {
    }
};

/**
 * @brief Resultado de validação de pacote
 */
struct PacketValidationResult {
    bool valid;                  // Válido
    PacketViolationType violationType; // Tipo de violação
    std::string reason;          // Razão
    DWORD expectedValue;        // Valor esperado
    DWORD actualValue;          // Valor atual
    float confidence;            // Confiança
    
    PacketValidationResult()
        : valid(true)
        , violationType(PacketViolationType::CHECKSUM_MISMATCH)
        , reason("")
        , expectedValue(0)
        , actualValue(0)
        , confidence(0.0f)
    {
    }
};

/**
 * @brief Violação de pacote
 */
struct PacketViolation {
    DWORD violationID;           // ID da violação
    DWORD connectionID;          // ID da conexão
    DWORD accountID;             // ID da conta
    DWORD characterID;           // ID do personagem
    PacketViolationType type;    // Tipo
    WORD packetType;             // Tipo de pacote
    time_t timestamp;             // Timestamp
    std::string details;          // Detalhes
    std::string ipAddress;        // Endereço IP
    bool actioned;                // Ação tomada
    
    PacketViolation()
        : violationID(0)
        , connectionID(0)
        , accountID(0)
        , characterID(0)
        , type(PacketViolationType::CHECKSUM_MISMATCH)
        , packetType(0)
        , timestamp(0)
        , details("")
        , ipAddress("")
        , actioned(false)
    {
    }
};

/**
 * @brief Perfil de conexão para validação de pacote
 */
struct ConnectionProfile {
    DWORD connectionID;           // ID da conexão
    DWORD accountID;              // ID da conta
    DWORD characterID;            // ID do personagem
    DWORD lastSequence;           // Última sequência
    std::deque<DWORD> sequences;   // Sequências
    std::unordered_map<WORD, DWORD> packetCounts; // Contagens de pacote
    std::unordered_map<WORD, time_t> lastPacketTimes; // Últimos tempos de pacote
    BYTE encryptionKey[16];       // Chave de criptografia
    DWORD violations;             // Violações
    DWORD checksumSeed;           // Seed de checksum
    time_t profileCreationTime;    // Hora de criação do perfil
    std::string ipAddress;         // Endereço IP
    bool flagged;                  // Marcado
    std::vector<DWORD> violationIDs; // IDs de violação
    
    ConnectionProfile()
        : connectionID(0)
        , accountID(0)
        , characterID(0)
        , lastSequence(0)
        , violations(0)
        , checksumSeed(0)
        , profileCreationTime(0)
        , ipAddress("")
        , flagged(false)
    {
        std::memset(encryptionKey, 0, sizeof(encryptionKey));
    }
};

/**
 * @brief Limite de taxa de pacote
 */
struct PacketRateLimit {
    WORD packetType;             // Tipo de pacote
    DWORD maxRate;               // Taxa máxima
    DWORD timeWindowMs;          // Janela de tempo (ms)
    DWORD burstAllowance;        // Permissão de rajada
    SecurityResponseType exceededResponse; // Resposta a excedência
    std::string description;      // Descrição
    
    PacketRateLimit()
        : packetType(0)
        , maxRate(0)
        , timeWindowMs(1000)
        , burstAllowance(5)
        , exceededResponse(SecurityResponseType::LOG)
        , description("")
    {
    }
};

/**
 * @brief Callback de violação de pacote
 */
using PacketViolationCallback = std::function<void(const PacketViolation&)>;

/**
 * @brief Sistema de validação de pacotes
 */
/**
 * Classe WYDPacketValidationSystem
 * 
 * Esta classe implementa a funcionalidade WYDPacketValidationSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PacketValidationSystem {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static PacketValidationSystem& GetInstance();
    
    /**
     * @brief Inicializa o sistema
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const PacketValidationConfig& config);
    
    /**
     * @brief Finaliza o sistema
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o sistema
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Valida um pacote
     * @param context Contexto
     * @return Resultado da validação
     */
    PacketValidationResult ValidatePacket(const PacketValidationContext& context);
    
    /**
     * @brief Registra uma violação
     * @param connectionID ID da conexão
     * @param accountID ID da conta
     * @param characterID ID do personagem
     * @param type Tipo
     * @param packetType Tipo de pacote
     * @param details Detalhes
     * @param ipAddress Endereço IP
     * @return ID da violação, ou 0 se falhar
     */
    DWORD RegisterViolation(DWORD connectionID, DWORD accountID, DWORD characterID,
                          PacketViolationType type, WORD packetType,
                          const std::string& details, const std::string& ipAddress);
    
    /**
     * @brief Adiciona uma conexão
     * @param connectionID ID da conexão
     * @param accountID ID da conta
     * @param ipAddress Endereço IP
     * @return true se adicionado com sucesso
     */
    bool AddConnection(DWORD connectionID, DWORD accountID, const std::string& ipAddress);
    
    /**
     * @brief Remove uma conexão
     * @param connectionID ID da conexão
     * @return true se removido com sucesso
     */
    bool RemoveConnection(DWORD connectionID);
    
    /**
     * @brief Adiciona um limite de taxa
     * @param packetType Tipo de pacote
     * @param maxRate Taxa máxima
     * @param timeWindowMs Janela de tempo (ms)
     * @param burstAllowance Permissão de rajada
     * @param exceededResponse Resposta a excedência
     * @param description Descrição
     * @return true se adicionado com sucesso
     */
    bool AddRateLimit(WORD packetType, DWORD maxRate, DWORD timeWindowMs,
                    DWORD burstAllowance, SecurityResponseType exceededResponse,
                    const std::string& description);
    
    /**
     * @brief Remove um limite de taxa
     * @param packetType Tipo de pacote
     * @return true se removido com sucesso
     */
    bool RemoveRateLimit(WORD packetType);
    
    /**
     * @brief Adiciona uma assinatura de pacote
     * @param signature Assinatura
     * @return true se adicionado com sucesso
     */
    bool AddPacketSignature(const PacketSignature& signature);
    
    /**
     * @brief Remove uma assinatura de pacote
     * @param signatureID ID da assinatura
     * @return true se removido com sucesso
     */
    bool RemovePacketSignature(DWORD signatureID);
    
    /**
     * @brief Obtém o número de violações de uma conexão
     * @param connectionID ID da conexão
     * @return Número de violações
     */
    DWORD GetConnectionViolationCount(DWORD connectionID) const;
    
    /**
     * @brief Reseta o contador de violações de uma conexão
     * @param connectionID ID da conexão
     * @return true se resetado com sucesso
     */
    bool ResetConnectionViolations(DWORD connectionID);
    
    /**
     * @brief Marca uma conexão como suspeita
     * @param connectionID ID da conexão
     * @param reason Razão
     * @return true se marcado com sucesso
     */
    bool FlagConnectionAsSuspect(DWORD connectionID, const std::string& reason);
    
    /**
     * @brief Verifica se uma conexão está marcada como suspeita
     * @param connectionID ID da conexão
     * @return true se estiver marcada
     */
    bool IsConnectionFlagged(DWORD connectionID) const;
    
    /**
     * @brief Obtém violações por conexão
     * @param connectionID ID da conexão
     * @return Lista de violações
     */
    std::vector<PacketViolation> GetViolationsByConnection(DWORD connectionID) const;
    
    /**
     * @brief Obtém violações por conta
     * @param accountID ID da conta
     * @return Lista de violações
     */
    std::vector<PacketViolation> GetViolationsByAccount(DWORD accountID) const;
    
    /**
     * @brief Obtém violações por tipo
     * @param type Tipo
     * @return Lista de violações
     */
    std::vector<PacketViolation> GetViolationsByType(PacketViolationType type) const;
    
    /**
     * @brief Registra um callback para violações
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterViolationCallback(PacketViolationCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterViolationCallback(int callbackID);
    
    /**
     * @brief Aplica uma ação a uma conexão
     * @param connectionID ID da conexão
     * @param response Resposta
     * @param reason Razão
     * @return true se aplicado com sucesso
     */
    bool ApplyActionToConnection(DWORD connectionID, SecurityResponseType response, const std::string& reason);
    
    /**
     * @brief Gera um relatório de validação
     * @param connectionID ID da conexão
     * @return Relatório formatado
     */
    std::string GenerateValidationReport(DWORD connectionID);
    
    /**
     * @brief Obtém estatísticas de validação
     * @return String formatada com estatísticas
     */
    std::string GetValidationStatistics();
    
    /**
     * @brief Calcula o checksum de um pacote
     * @param data Dados
     * @param size Tamanho
     * @param seed Seed
     * @return Checksum
     */
    DWORD CalculatePacketChecksum(const void* data, size_t size, DWORD seed);
    
    /**
     * @brief Gera a próxima sequência para uma conexão
     * @param connectionID ID da conexão
     * @return Sequência
     */
    DWORD GenerateNextSequence(DWORD connectionID);
    
    /**
     * @brief Valida a sequência de um pacote
     * @param connectionID ID da conexão
     * @param sequence Sequência
     * @return true se válida
     */
    bool ValidateSequence(DWORD connectionID, DWORD sequence);
    
    /**
     * @brief Obtém a chave de criptografia de uma conexão
     * @param connectionID ID da conexão
     * @param key Chave (saída)
     * @return true se obtido com sucesso
     */
    bool GetEncryptionKey(DWORD connectionID, BYTE key[16]);
    
    /**
     * @brief Define a chave de criptografia de uma conexão
     * @param connectionID ID da conexão
     * @param key Chave
     * @return true se definido com sucesso
     */
    bool SetEncryptionKey(DWORD connectionID, const BYTE key[16]);
    
    /**
     * @brief Criptografa um pacote
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     * @return true se criptografado com sucesso
     */
    bool EncryptPacket(void* data, size_t size, const BYTE key[16]);
    
    /**
     * @brief Descriptografa um pacote
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     * @return true se descriptografado com sucesso
     */
    bool DecryptPacket(void* data, size_t size, const BYTE key[16]);
    
    /**
     * @brief Define o tipo de resposta a violação padrão
     * @param response Resposta
     */
    void SetDefaultViolationResponse(SecurityResponseType response);
    
    /**
     * @brief Obtém o tipo de resposta a violação padrão
     * @return Resposta
     */
    SecurityResponseType GetDefaultViolationResponse() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    PacketValidationSystem();
    
    /**
     * @brief Destrutor
     */
    ~PacketValidationSystem();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    PacketValidationSystem(const PacketValidationSystem&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    PacketValidationSystem& operator=(const PacketValidationSystem&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Valida o checksum de um pacote
     * @param context Contexto
     * @return Resultado
     */
    PacketValidationResult ValidateChecksum(const PacketValidationContext& context);
    
    /**
     * @brief Valida a sequência de um pacote
     * @param context Contexto
     * @return Resultado
     */
    PacketValidationResult ValidateSequenceNumber(const PacketValidationContext& context);
    
    /**
     * @brief Valida o conteúdo de um pacote
     * @param context Contexto
     * @return Resultado
     */
    PacketValidationResult ValidateContent(const PacketValidationContext& context);
    
    /**
     * @brief Valida a taxa de pacotes
     * @param context Contexto
     * @return Resultado
     */
    PacketValidationResult ValidatePacketRate(const PacketValidationContext& context);
    
    /**
     * @brief Verifica assinaturas de pacote
     * @param context Contexto
     * @return Resultado
     */
    PacketValidationResult CheckPacketSignatures(const PacketValidationContext& context);
    
    /**
     * @brief Detecta padrões em pacotes
     * @param context Contexto
     * @return Resultado
     */
    PacketValidationResult DetectPacketPatterns(const PacketValidationContext& context);
    
    /**
     * @brief Valida o tamanho de um pacote
     * @param context Contexto
     * @return Resultado
     */
    PacketValidationResult ValidatePacketSize(const PacketValidationContext& context);
    
    /**
     * @brief Atualiza perfis de conexão
     */
    void UpdateConnectionProfiles();
    
    /**
     * @brief Notifica callbacks de violação
     * @param violation Violação
     */
    void NotifyViolationCallbacks(const PacketViolation& violation);
    
    /**
     * @brief Verifica se um tipo de pacote está isento
     * @param packetType Tipo de pacote
     * @return true se estiver isento
     */
    bool IsPacketTypeExempt(WORD packetType) const;
    
    /**
     * @brief Obtém o limite de taxa para um tipo de pacote
     * @param packetType Tipo de pacote
     * @return Ponteiro para o limite, ou nullptr se não existir
     */
    const PacketRateLimit* GetRateLimitForPacketType(WORD packetType) const;
    
    /**
     * @brief Gera uma chave de criptografia
     * @param key Chave (saída)
     */
    void GenerateEncryptionKey(BYTE key[16]);
    
    /**
     * @brief Rotaciona uma chave de criptografia
     * @param key Chave (entrada/saída)
     */
    void RotateEncryptionKey(BYTE key[16]);
    
    // Configuração
    PacketValidationConfig config_;
    
    // Perfis de conexão
    std::unordered_map<DWORD, ConnectionProfile> connectionProfiles_;
    mutable std::mutex profilesMutex_;
    
    // Violações
    std::vector<PacketViolation> violations_;
    std::mutex violationsMutex_;
    
    // Limites de taxa
    std::unordered_map<WORD, PacketRateLimit> rateLimits_;
    mutable std::mutex rateLimitsMutex_;
    
    // Assinaturas de pacote
    std::vector<PacketSignature> packetSignatures_;
    mutable std::mutex signaturesMutex_;
    
    // Conexões marcadas
    std::unordered_set<DWORD> flaggedConnections_;
    std::unordered_map<DWORD, std::string> flagReasons_;
    mutable std::mutex flaggedConnectionsMutex_;
    
    // Callbacks para violações
    std::map<int, PacketViolationCallback> violationCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contador para novos IDs
    std::atomic<DWORD> nextViolationID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Estatísticas
    DWORD totalPacketsProcessed_;
    DWORD totalViolationsDetected_;
    std::unordered_map<PacketViolationType, DWORD> violationsByType_;
    mutable std::mutex statisticsMutex_;
    
    // Limites de tamanho de pacote
    std::unordered_map<WORD, std::pair<size_t, size_t>> packetSizeLimits_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_PacketValidationSystem PacketValidationSystem::GetInstance()

} // namespace antipacket
} // namespace security
} // namespace server
} // namespace wyd

#endif // PACKETVALIDATIONSYSTEM_H

} // namespace wydbr
