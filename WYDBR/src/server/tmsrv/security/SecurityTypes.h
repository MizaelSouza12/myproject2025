/**
 * SecurityTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/security/SecurityTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef SECURITYTYPES_H
#define SECURITYTYPES_H

#include <string>
#include <vector>
#include <map>
#include <array>
#include <bitset>
#include <chrono>
#include <ctime>
#include <functional>
#include <atomic>
#include <mutex>
#include <memory>

#include "../../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file SecurityTypes.h
 * @brief Definições de tipos para o sistema de segurança avançado
 * 
 * Este arquivo contém as definições de tipos para o sistema de segurança avançado do WYD,
 * que implementa múltiplas camadas de proteção contra hacks, exploits e manipulações 
 * não autorizadas no jogo.
 */

namespace wyd {
namespace server {
namespace security {

// Constantes de sistema
constexpr size_t MAX_SECURITY_PROFILES = 100;         // Máximo de perfis de segurança
constexpr size_t MAX_DETECTION_RULES = 1000;          // Máximo de regras de detecção
constexpr size_t MAX_PACKET_VALIDATORS = 50;          // Máximo de validadores de pacote
constexpr size_t MAX_CLIENT_VALIDATORS = 50;          // Máximo de validadores de cliente
constexpr size_t MAX_BEHAVIOR_MONITORS = 50;          // Máximo de monitores de comportamento
constexpr size_t MAX_BLOCKLIST_ENTRIES = 10000;       // Máximo de entradas na lista de bloqueio
constexpr size_t MAX_SECURITY_LOGS = 1000000;         // Máximo de logs de segurança
constexpr size_t MAX_ENCRYPTION_KEYS = 100;           // Máximo de chaves de criptografia
constexpr size_t MAX_INTEGRITY_CHECKS = 1000;         // Máximo de verificações de integridade
constexpr size_t MAX_SECURITY_CALLBACKS = 100;        // Máximo de callbacks de segurança
constexpr size_t MAX_TIME_SYNC_ENTRIES = 10000;       // Máximo de entradas de sincronização de tempo
constexpr size_t MAX_PACKET_SIGNATURES = 10000;       // Máximo de assinaturas de pacote
constexpr size_t MAX_SECURITY_CHALLENGES = 1000;      // Máximo de desafios de segurança
constexpr size_t MAX_RATE_LIMITERS = 100;             // Máximo de limitadores de taxa
constexpr size_t MAX_PACKET_PATTERNS = 1000;          // Máximo de padrões de pacote
constexpr size_t MAX_ACTION_HANDLERS = 100;           // Máximo de manipuladores de ação

/**
 * @brief Tipo de detecção de segurança
 */
enum class SecurityDetectionType : BYTE {
    SPEED_HACK = 0,           // Hack de velocidade
    TELEPORT_HACK = 1,        // Hack de teleporte
    PACKET_INJECTION = 2,     // Injeção de pacote
    PACKET_MODIFICATION = 3,  // Modificação de pacote
    CLIENT_MODIFICATION = 4,  // Modificação de cliente
    MEMORY_HACK = 5,          // Hack de memória
    FUNCTION_HOOK = 6,        // Hook de função
    UNUSUAL_BEHAVIOR = 7,     // Comportamento incomum
    BOT_DETECTION = 8,        // Detecção de bot
    PROXY_DETECTION = 9,      // Detecção de proxy
    TIME_MANIPULATION = 10,   // Manipulação de tempo
    MULTI_CLIENT = 11,        // Multi-cliente
    EXPLOIT_ATTEMPT = 12,     // Tentativa de exploit
    CLIENT_TAMPERING = 13,    // Adulteração de cliente
    CUSTOM = 14,              // Personalizado
};

/**
 * @brief Nível de severidade de segurança
 */
enum class SecuritySeverity : BYTE {
    INFO = 0,                 // Informação
    LOW = 1,                  // Baixo
    MEDIUM = 2,               // Médio
    HIGH = 3,                 // Alto
    CRITICAL = 4,             // Crítico
    CUSTOM = 5,               // Personalizado
};

/**
 * @brief Tipo de validação de pacote
 */
enum class PacketValidationType : BYTE {
    CHECKSUM = 0,             // Checksum
    SEQUENCE = 1,             // Sequência
    SIZE = 2,                 // Tamanho
    CONTENT = 3,              // Conteúdo
    TIMING = 4,               // Tempo
    ENCRYPTION = 5,           // Criptografia
    SIGNATURE = 6,            // Assinatura
    RATE = 7,                 // Taxa
    PATTERN = 8,              // Padrão
    CUSTOM = 9,               // Personalizado
};

/**
 * @brief Tipo de validação de cliente
 */
enum class ClientValidationType : BYTE {
    HASH = 0,                 // Hash
    SIGNATURE = 1,            // Assinatura
    MODULE = 2,               // Módulo
    MEMORY = 3,               // Memória
    INTEGRITY = 4,            // Integridade
    BEHAVIOR = 5,             // Comportamento
    TIMING = 6,               // Tempo
    ENVIRONMENT = 7,          // Ambiente
    CHALLENGE = 8,            // Desafio
    CUSTOM = 9,               // Personalizado
};

/**
 * @brief Tipo de resposta de segurança
 */
enum class SecurityResponseType : BYTE {
    NONE = 0,                 // Nenhum
    LOG = 1,                  // Log
    ALERT = 2,                // Alerta
    DISCONNECT = 3,           // Desconexão
    TEMPORARY_BAN = 4,        // Banimento temporário
    PERMANENT_BAN = 5,        // Banimento permanente
    CHALLENGE = 6,            // Desafio
    RATE_LIMIT = 7,           // Limite de taxa
    REDIRECT = 8,             // Redirecionamento
    CUSTOM_ACTION = 9,        // Ação personalizada
};

/**
 * @brief Status de verificação de segurança
 */
enum class SecurityCheckStatus : BYTE {
    PENDING = 0,              // Pendente
    PASSED = 1,               // Aprovado
    FAILED = 2,               // Falhou
    ERROR = 3,                // Erro
    TIMEOUT = 4,              // Tempo esgotado
    CUSTOM = 5,               // Personalizado
};

/**
 * @brief Tipo de algoritmo de detecção
 */
enum class DetectionAlgorithmType : BYTE {
    THRESHOLD = 0,            // Limite
    PATTERN = 1,              // Padrão
    STATISTICAL = 2,          // Estatístico
    ANOMALY = 3,              // Anomalia
    MACHINE_LEARNING = 4,     // Aprendizado de máquina
    SIGNATURE_BASED = 5,      // Baseado em assinatura
    HEURISTIC = 6,            // Heurístico
    RULE_BASED = 7,           // Baseado em regra
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Tipo de algoritmo de criptografia
 */
enum class SecurityEncryptionType : BYTE {
    NONE = 0,                 // Nenhum
    XOR = 1,                  // XOR
    AES = 2,                  // AES
    RSA = 3,                  // RSA
    BLOWFISH = 4,             // Blowfish
    TWOFISH = 5,              // Twofish
    CHACHA20 = 6,             // ChaCha20
    CUSTOM = 7,               // Personalizado
};

/**
 * @brief Tipo de alvo de segurança
 */
enum class SecurityTargetType : BYTE {
    PLAYER = 0,               // Jogador
    ACCOUNT = 1,              // Conta
    IP_ADDRESS = 2,           // Endereço IP
    DEVICE = 3,               // Dispositivo
    CLIENT = 4,               // Cliente
    HWID = 5,                 // ID de hardware
    MAC_ADDRESS = 6,          // Endereço MAC
    SESSION = 7,              // Sessão
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Tipo de desafio de segurança
 */
enum class SecurityChallengeType : BYTE {
    SIMPLE = 0,               // Simples
    CAPTCHA = 1,              // CAPTCHA
    MEMORY_CHECK = 2,         // Verificação de memória
    CODE_EXECUTION = 3,       // Execução de código
    TIMING = 4,               // Tempo
    HARDWARE = 5,             // Hardware
    ENVIRONMENTAL = 6,        // Ambiental
    COMBINED = 7,             // Combinado
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Padrão de comportamento monitorado
 */
enum class BehaviorPatternType : BYTE {
    MOVEMENT = 0,             // Movimento
    COMBAT = 1,               // Combate
    INVENTORY = 2,            // Inventário
    TRADE = 3,                // Comércio
    SOCIAL = 4,               // Social
    QUEST = 5,                // Quest
    RESOURCE_GATHERING = 6,   // Coleta de recursos
    SKILL_USAGE = 7,          // Uso de skill
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Detecção de segurança
 */
struct SecurityDetection {
    DWORD detectionID;                // ID da detecção
    SecurityDetectionType type;       // Tipo
    DWORD accountID;                  // ID da conta
    DWORD characterID;                // ID do personagem
    std::string characterName;         // Nome do personagem
    std::string ipAddress;             // Endereço IP
    std::string deviceID;              // ID do dispositivo
    time_t detectionTime;              // Hora da detecção
    SecuritySeverity severity;        // Severidade
    std::string details;               // Detalhes
    SecurityResponseType response;    // Resposta
    bool verified;                     // Verificado
    DWORD confidence;                 // Confiança (0-100)
    
    SecurityDetection()
        : detectionID(0)
        , type(SecurityDetectionType::UNUSUAL_BEHAVIOR)
        , accountID(0)
        , characterID(0)
        , characterName("")
        , ipAddress("")
        , deviceID("")
        , detectionTime(0)
        , severity(SecuritySeverity::MEDIUM)
        , details("")
        , response(SecurityResponseType::LOG)
        , verified(false)
        , confidence(0)
    {
    }
};

/**
 * @brief Regra de detecção de segurança
 */
struct SecurityDetectionRule {
    DWORD ruleID;                         // ID da regra
    std::string name;                      // Nome
    SecurityDetectionType type;           // Tipo
    DetectionAlgorithmType algorithm;     // Algoritmo
    float threshold;                       // Limite
    SecuritySeverity severity;            // Severidade
    SecurityResponseType response;        // Resposta
    bool enabled;                          // Habilitada
    std::string pattern;                   // Padrão
    std::string parameters;                // Parâmetros (JSON)
    bool logEvidence;                      // Registrar evidência
    DWORD minConfidence;                  // Confiança mínima
    
    SecurityDetectionRule()
        : ruleID(0)
        , name("")
        , type(SecurityDetectionType::UNUSUAL_BEHAVIOR)
        , algorithm(DetectionAlgorithmType::THRESHOLD)
        , threshold(0.0f)
        , severity(SecuritySeverity::MEDIUM)
        , response(SecurityResponseType::LOG)
        , enabled(true)
        , pattern("")
        , parameters("")
        , logEvidence(true)
        , minConfidence(50)
    {
    }
};

/**
 * @brief Validador de pacote
 */
struct PacketValidator {
    DWORD validatorID;                 // ID do validador
    std::string name;                   // Nome
    PacketValidationType type;         // Tipo
    bool enabled;                       // Habilitado
    WORD packetTypeFilter;             // Filtro de tipo de pacote
    std::string validationExpression;   // Expressão de validação
    SecurityResponseType failResponse; // Resposta a falha
    DWORD minPacketSize;               // Tamanho mínimo de pacote
    DWORD maxPacketSize;               // Tamanho máximo de pacote
    std::vector<BYTE> signatureBytes;   // Bytes de assinatura
    std::string parameters;             // Parâmetros (JSON)
    
    PacketValidator()
        : validatorID(0)
        , name("")
        , type(PacketValidationType::CHECKSUM)
        , enabled(true)
        , packetTypeFilter(0)
        , validationExpression("")
        , failResponse(SecurityResponseType::LOG)
        , minPacketSize(0)
        , maxPacketSize(0)
        , parameters("")
    {
    }
};

/**
 * @brief Validador de cliente
 */
struct ClientValidator {
    DWORD validatorID;                 // ID do validador
    std::string name;                   // Nome
    ClientValidationType type;         // Tipo
    bool enabled;                       // Habilitado
    DWORD validationInterval;          // Intervalo de validação (ms)
    std::string expectedValue;          // Valor esperado
    SecurityResponseType failResponse; // Resposta a falha
    std::string challengeData;          // Dados de desafio
    std::string validationCode;         // Código de validação
    std::string parameters;             // Parâmetros (JSON)
    
    ClientValidator()
        : validatorID(0)
        , name("")
        , type(ClientValidationType::HASH)
        , enabled(true)
        , validationInterval(60000)
        , expectedValue("")
        , failResponse(SecurityResponseType::LOG)
        , challengeData("")
        , validationCode("")
        , parameters("")
    {
    }
};

/**
 * @brief Monitor de comportamento
 */
struct BehaviorMonitor {
    DWORD monitorID;                   // ID do monitor
    std::string name;                   // Nome
    BehaviorPatternType type;          // Tipo
    bool enabled;                       // Habilitado
    float anomalyThreshold;             // Limite de anomalia
    DWORD samplingRate;                // Taxa de amostragem (ms)
    DWORD learningPeriod;              // Período de aprendizado (ms)
    SecurityResponseType alertResponse; // Resposta a alerta
    std::string parameters;             // Parâmetros (JSON)
    
    BehaviorMonitor()
        : monitorID(0)
        , name("")
        , type(BehaviorPatternType::MOVEMENT)
        , enabled(true)
        , anomalyThreshold(0.8f)
        , samplingRate(1000)
        , learningPeriod(3600000)
        , alertResponse(SecurityResponseType::LOG)
        , parameters("")
    {
    }
};

/**
 * @brief Entrada em lista de bloqueio
 */
struct BlocklistEntry {
    DWORD entryID;                     // ID da entrada
    SecurityTargetType targetType;     // Tipo de alvo
    std::string targetValue;            // Valor do alvo
    time_t creationTime;                // Hora de criação
    time_t expiryTime;                  // Hora de expiração
    std::string reason;                 // Razão
    DWORD creatorID;                   // ID do criador
    SecuritySeverity severity;         // Severidade
    bool active;                        // Ativo
    std::string notes;                  // Notas
    
    BlocklistEntry()
        : entryID(0)
        , targetType(SecurityTargetType::IP_ADDRESS)
        , targetValue("")
        , creationTime(0)
        , expiryTime(0)
        , reason("")
        , creatorID(0)
        , severity(SecuritySeverity::MEDIUM)
        , active(true)
        , notes("")
    {
    }
};

/**
 * @brief Perfil de segurança
 */
struct SecurityProfile {
    DWORD profileID;                   // ID do perfil
    std::string name;                   // Nome
    bool enabled;                       // Habilitado
    SecurityEncryptionType encryption; // Tipo de criptografia
    std::string encryptionKey;          // Chave de criptografia
    float packetValidationRate;         // Taxa de validação de pacote
    float clientValidationRate;         // Taxa de validação de cliente
    DWORD challengeFrequency;          // Frequência de desafio (ms)
    DWORD timeToleranceMs;             // Tolerância de tempo (ms)
    std::string allowedClientVersions;  // Versões de cliente permitidas
    
    SecurityProfile()
        : profileID(0)
        , name("")
        , enabled(true)
        , encryption(SecurityEncryptionType::XOR)
        , encryptionKey("")
        , packetValidationRate(1.0f)
        , clientValidationRate(1.0f)
        , challengeFrequency(300000)
        , timeToleranceMs(500)
        , allowedClientVersions("")
    {
    }
};

/**
 * @brief Log de segurança
 */
struct SecurityLog {
    DWORD logID;                       // ID do log
    time_t timestamp;                   // Timestamp
    SecurityDetectionType eventType;   // Tipo de evento
    SecuritySeverity severity;         // Severidade
    DWORD accountID;                   // ID da conta
    DWORD characterID;                 // ID do personagem
    std::string ipAddress;              // Endereço IP
    std::string message;                // Mensagem
    WORD packetType;                   // Tipo de pacote
    std::string rawData;                // Dados brutos
    bool resolved;                      // Resolvido
    
    SecurityLog()
        : logID(0)
        , timestamp(0)
        , eventType(SecurityDetectionType::UNUSUAL_BEHAVIOR)
        , severity(SecuritySeverity::INFO)
        , accountID(0)
        , characterID(0)
        , ipAddress("")
        , message("")
        , packetType(0)
        , rawData("")
        , resolved(false)
    {
    }
};

/**
 * @brief Desafio de segurança
 */
struct SecurityChallenge {
    DWORD challengeID;                 // ID do desafio
    SecurityChallengeType type;        // Tipo
    DWORD accountID;                   // ID da conta
    time_t issueTime;                   // Hora de emissão
    time_t expiryTime;                  // Hora de expiração
    std::string challengeData;          // Dados do desafio
    std::string expectedResponse;       // Resposta esperada
    bool completed;                     // Completado
    SecurityCheckStatus status;        // Status
    std::string responseData;           // Dados de resposta
    time_t responseTime;                // Hora de resposta
    
    SecurityChallenge()
        : challengeID(0)
        , type(SecurityChallengeType::SIMPLE)
        , accountID(0)
        , issueTime(0)
        , expiryTime(0)
        , challengeData("")
        , expectedResponse("")
        , completed(false)
        , status(SecurityCheckStatus::PENDING)
        , responseData("")
        , responseTime(0)
    {
    }
};

/**
 * @brief Verificação de integridade
 */
struct IntegrityCheck {
    DWORD checkID;                     // ID da verificação
    std::string target;                 // Alvo
    std::string checksum;               // Checksum
    std::string expectedChecksum;       // Checksum esperado
    time_t checkTime;                   // Hora da verificação
    SecurityCheckStatus status;        // Status
    std::string details;                // Detalhes
    
    IntegrityCheck()
        : checkID(0)
        , target("")
        , checksum("")
        , expectedChecksum("")
        , checkTime(0)
        , status(SecurityCheckStatus::PENDING)
        , details("")
    {
    }
};

/**
 * @brief Limitador de taxa
 */
struct RateLimiter {
    DWORD limiterID;                   // ID do limitador
    std::string name;                   // Nome
    WORD packetType;                   // Tipo de pacote
    DWORD maxRate;                     // Taxa máxima
    DWORD timeWindowMs;                // Janela de tempo (ms)
    bool enabled;                       // Habilitado
    SecurityResponseType exceededResponse; // Resposta a excedência
    DWORD burstAllowance;              // Permissão de rajada
    
    RateLimiter()
        : limiterID(0)
        , name("")
        , packetType(0)
        , maxRate(0)
        , timeWindowMs(1000)
        , enabled(true)
        , exceededResponse(SecurityResponseType::LOG)
        , burstAllowance(0)
    {
    }
};

/**
 * @brief Estatísticas de segurança
 */
struct SecurityStatistics {
    DWORD totalDetections;              // Total de detecções
    DWORD activeBlocklistEntries;       // Entradas ativas na lista de bloqueio
    DWORD challengesIssued;             // Desafios emitidos
    DWORD challengesPassed;             // Desafios aprovados
    DWORD challengesFailed;             // Desafios falhos
    DWORD packetValidationFailures;     // Falhas de validação de pacote
    DWORD clientValidationFailures;     // Falhas de validação de cliente
    DWORD rateLimitExceeded;            // Limite de taxa excedido
    float falsePositiveRate;            // Taxa de falsos positivos
    DWORD criticalAlerts;               // Alertas críticos
    
    SecurityStatistics()
        : totalDetections(0)
        , activeBlocklistEntries(0)
        , challengesIssued(0)
        , challengesPassed(0)
        , challengesFailed(0)
        , packetValidationFailures(0)
        , clientValidationFailures(0)
        , rateLimitExceeded(0)
        , falsePositiveRate(0.0f)
        , criticalAlerts(0)
    {
    }
};

/**
 * @brief Registro de tempo de cliente
 */
struct ClientTimeRecord {
    DWORD recordID;                    // ID do registro
    DWORD accountID;                   // ID da conta
    DWORD characterID;                 // ID do personagem
    DWORD clientTime;                  // Tempo do cliente
    DWORD serverTime;                  // Tempo do servidor
    DWORD variance;                    // Variância
    time_t timestamp;                   // Timestamp
    bool flagged;                       // Marcado
    
    ClientTimeRecord()
        : recordID(0)
        , accountID(0)
        , characterID(0)
        , clientTime(0)
        , serverTime(0)
        , variance(0)
        , timestamp(0)
        , flagged(false)
    {
    }
};

/**
 * @brief Assinatura de pacote
 */
struct PacketSignature {
    DWORD signatureID;                 // ID da assinatura
    WORD packetType;                   // Tipo de pacote
    std::vector<BYTE> signatureBytes;   // Bytes de assinatura
    std::string description;            // Descrição
    bool isHackSignature;               // Se é assinatura de hack
    SecuritySeverity severity;         // Severidade
    SecurityResponseType response;     // Resposta
    
    PacketSignature()
        : signatureID(0)
        , packetType(0)
        , description("")
        , isHackSignature(false)
        , severity(SecuritySeverity::MEDIUM)
        , response(SecurityResponseType::LOG)
    {
    }
};

/**
 * @brief Manipulador de ação de segurança
 */
struct SecurityActionHandler {
    DWORD handlerID;                    // ID do manipulador
    std::string name;                    // Nome
    SecurityResponseType responseType;  // Tipo de resposta
    std::string scriptPath;              // Caminho do script
    std::string parameters;              // Parâmetros (JSON)
    bool enabled;                        // Habilitado
    std::string description;             // Descrição
    
    SecurityActionHandler()
        : handlerID(0)
        , name("")
        , responseType(SecurityResponseType::CUSTOM_ACTION)
        , scriptPath("")
        , parameters("")
        , enabled(true)
        , description("")
    {
    }
};

/**
 * @brief Padrão de comportamento anormal
 */
struct AnomalyPattern {
    DWORD patternID;                    // ID do padrão
    std::string name;                    // Nome
    std::string pattern;                 // Padrão
    BehaviorPatternType behaviorType;   // Tipo de comportamento
    float anomalyScore;                  // Pontuação de anomalia
    std::string description;             // Descrição
    bool isActive;                       // Está ativo
    
    AnomalyPattern()
        : patternID(0)
        , name("")
        , pattern("")
        , behaviorType(BehaviorPatternType::MOVEMENT)
        , anomalyScore(0.0f)
        , description("")
        , isActive(true)
    {
    }
};

/**
 * @brief Controle de tempo anti-speedhack
 */
struct TimeControl {
    DWORD windowSize;                   // Tamanho da janela (ms)
    DWORD maxDeviation;                 // Desvio máximo (ms)
    DWORD smoothingFactor;              // Fator de suavização
    DWORD checkInterval;                // Intervalo de verificação (ms)
    DWORD maxAllowedAcceleration;       // Aceleração máxima permitida
    DWORD synchronizationInterval;      // Intervalo de sincronização (ms)
    DWORD toleranceThreshold;           // Limite de tolerância
    bool enabled;                        // Habilitado
    
    TimeControl()
        : windowSize(10000)
        , maxDeviation(1000)
        , smoothingFactor(5)
        , checkInterval(2000)
        , maxAllowedAcceleration(300)
        , synchronizationInterval(60000)
        , toleranceThreshold(500)
        , enabled(true)
    {
    }
};

/**
 * @brief Callback de detecção de segurança
 */
using SecurityDetectionCallback = std::function<void(const SecurityDetection&)>;

/**
 * @brief Callback de validação de pacote
 */
using PacketValidationCallback = std::function<bool(WORD, const void*, size_t)>;

/**
 * @brief Callback de validação de cliente
 */
using ClientValidationCallback = std::function<bool(DWORD, const void*, size_t)>;

} // namespace security
} // namespace server
} // namespace wyd

#endif // SECURITYTYPES_H

} // namespace wydbr
