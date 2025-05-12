#ifndef WYDSTUDIO_SECURITY_GUARDIAN_H
#define WYDSTUDIO_SECURITY_GUARDIAN_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <random>
#include <bitset>
#include "../core/MultitaskingCore.h"
#include "../network/ProtocolAnalyzer.h"

namespace wydstudio {
namespace security {

// Forward declarations
class SecurityMonitor;
class IntegrityVerifier;
class AnomalyDetector;
class CheatSignatureDatabase;
class MemoryProtector;
class NetworkValidator;
class ClientPatcher;
class ServerShield;
class HeuristicAnalyzer;
class ThreatIntelligence;

/**
 * @brief Nível de severidade para ameaças de segurança
 */
enum class ThreatSeverity {
    NONE,       // Nenhuma ameaça
    LOW,        // Ameaça de baixo risco, potencialmente falso positivo
    MEDIUM,     // Ameaça moderada, merecedora de atenção
    HIGH,       // Ameaça séria, ação recomendada
    CRITICAL    // Ameaça crítica, ação imediata necessária
};

/**
 * @brief Tipo de ameaça de segurança
 */
enum class ThreatType {
    UNKNOWN,                // Tipo desconhecido
    MEMORY_MANIPULATION,    // Manipulação de memória
    SPEED_HACK,             // Alteração de velocidade do jogo
    PACKET_INJECTION,       // Injeção de pacotes de rede
    PACKET_MANIPULATION,    // Manipulação de pacotes de rede
    CLIENT_MODIFICATION,    // Modificação no cliente do jogo
    DLL_INJECTION,          // Injeção de DLL
    BOT_AUTOMATION,         // Uso de bots para automação
    DEBUGGER_DETECTED,      // Uso de depurador detectado
    VIRTUAL_MACHINE,        // Execução em máquina virtual
    SANDBOX_DETECTED,       // Execução em ambiente sandbox
    CODE_HOOKING,           // Hooking de código
    INVALID_CHECKSUM,       // Checksum inválido
    DATA_TAMPERING,         // Adulteração de dados
    ABNORMAL_BEHAVIOR,      // Comportamento anormal
    PRIVILEGE_ESCALATION,   // Escalação de privilégios
    TIMING_ATTACK,          // Ataque de temporização
    ENCRYPTION_BYPASS,      // Bypass de criptografia
    API_ABUSE,              // Abuso de API
    RATE_ABUSE              // Abuso de taxa de requisições
};

/**
 * @brief Ação a ser tomada em resposta a uma ameaça
 */
enum class SecurityAction {
    NONE,                   // Nenhuma ação
    LOG,                    // Apenas registrar
    ALERT,                  // Alertar administrador
    WARN_PLAYER,            // Avisar o jogador
    RESTRICT_ACTIONS,       // Restringir ações do jogador
    TEMPORARY_BAN,          // Banir temporariamente
    PERMANENT_BAN,          // Banir permanentemente
    DISCONNECT,             // Desconectar jogador
    SILENT_MONITORING,      // Monitorar silenciosamente
    REDIRECT_TO_HONEYPOT,   // Redirecionar para honeypot
    APPLY_PATCH,            // Aplicar patch no cliente
    TERMINATE_CLIENT,       // Encerrar o cliente
    ROLLBACK_TRANSACTION,   // Reverter transação
    INVALIDATE_SESSION,     // Invalidar sessão
    RESET_ACCOUNT_STATE,    // Redefinir estado da conta
    CUSTOM                  // Ação personalizada
};

/**
 * @brief Contexto de segurança para o jogador
 */
struct PlayerSecurityContext {
    uint32_t accountId;
    uint32_t characterId;
    std::string playerName;
    std::string ipAddress;
    std::string macAddress;
    std::string hardwareId;
    std::string clientVersion;
    std::chrono::system_clock::time_point lastActivity;
    std::unordered_map<std::string, std::string> attributes;
    
    // Histórico de segurança
    uint32_t warningCount;
    uint32_t suspiciousActivityCount;
    bool previouslySuspended;
    bool monitoringEnabled;
    float trustScore; // 0.0 a 1.0
    
    PlayerSecurityContext() : accountId(0), characterId(0),
                           warningCount(0), suspiciousActivityCount(0),
                           previouslySuspended(false), monitoringEnabled(false),
                           trustScore(1.0f) {}
};

/**
 * @brief Relatório de ameaça de segurança
 */
struct ThreatReport {
    ThreatType type;
    ThreatSeverity severity;
    std::string description;
    std::optional<PlayerSecurityContext> playerContext;
    std::unordered_map<std::string, std::string> evidence;
    std::chrono::system_clock::time_point timestamp;
    bool confirmed;
    float confidenceScore; // 0.0 a 1.0
    SecurityAction recommendedAction;
    std::string uniqueIdentifier;
    
    ThreatReport() : type(ThreatType::UNKNOWN), 
                   severity(ThreatSeverity::NONE),
                   confirmed(false),
                   confidenceScore(0.0f),
                   recommendedAction(SecurityAction::NONE),
                   timestamp(std::chrono::system_clock::now()) {}
    
    static ThreatReport createLowThreat(ThreatType threatType, const std::string& desc) {
        ThreatReport report;
        report.type = threatType;
        report.severity = ThreatSeverity::LOW;
        report.description = desc;
        report.confidenceScore = 0.3f;
        report.recommendedAction = SecurityAction::LOG;
        return report;
    }
    
    static ThreatReport createMediumThreat(ThreatType threatType, const std::string& desc) {
        ThreatReport report;
        report.type = threatType;
        report.severity = ThreatSeverity::MEDIUM;
        report.description = desc;
        report.confidenceScore = 0.6f;
        report.recommendedAction = SecurityAction::ALERT;
        return report;
    }
    
    static ThreatReport createHighThreat(ThreatType threatType, const std::string& desc) {
        ThreatReport report;
        report.type = threatType;
        report.severity = ThreatSeverity::HIGH;
        report.description = desc;
        report.confidenceScore = 0.8f;
        report.recommendedAction = SecurityAction::RESTRICT_ACTIONS;
        return report;
    }
    
    static ThreatReport createCriticalThreat(ThreatType threatType, const std::string& desc) {
        ThreatReport report;
        report.type = threatType;
        report.severity = ThreatSeverity::CRITICAL;
        report.description = desc;
        report.confidenceScore = 0.95f;
        report.recommendedAction = SecurityAction::TEMPORARY_BAN;
        return report;
    }
};

/**
 * @brief Configuração de segurança
 */
struct SecurityConfig {
    // Configurações gerais
    bool enableActiveProtection;
    bool enablePassiveMonitoring;
    bool enableHeuristicAnalysis;
    bool enableSignatureChecking;
    bool enableNetworkValidation;
    bool enableMemoryProtection;
    bool enableClientPatching;
    bool enableTelemetry;
    
    // Limites
    uint32_t maxWarningsBeforeBan;
    uint32_t temporaryBanDurationMinutes;
    uint32_t rateLimit_ActionsPerMinute;
    uint32_t rateLimit_PacketsPerSecond;
    uint32_t checksumValidationFrequencySeconds;
    
    // Sensibilidade
    float heuristicAnalysisSensitivity; // 0.0 a 1.0
    float anomalyDetectionThreshold;    // 0.0 a 1.0
    float trustScoreDecayRate;          // Taxa de decaimento do score de confiança
    float trustScoreRecoveryRate;       // Taxa de recuperação do score de confiança
    
    // Funcionalidades avançadas
    bool enableHoneypot;
    bool enableEncryption;
    bool enableObfuscation;
    bool enableCodeDiversification;
    bool enableVirtualizationDetection;
    bool enableSandboxDetection;
    
    // Notificações
    bool notifyAdminOnHighThreats;
    bool notifyPlayerOnActionTaken;
    
    SecurityConfig() 
        : enableActiveProtection(true),
          enablePassiveMonitoring(true),
          enableHeuristicAnalysis(true),
          enableSignatureChecking(true),
          enableNetworkValidation(true),
          enableMemoryProtection(true),
          enableClientPatching(false),
          enableTelemetry(true),
          maxWarningsBeforeBan(3),
          temporaryBanDurationMinutes(1440), // 24 horas
          rateLimit_ActionsPerMinute(300),
          rateLimit_PacketsPerSecond(100),
          checksumValidationFrequencySeconds(300), // 5 minutos
          heuristicAnalysisSensitivity(0.7f),
          anomalyDetectionThreshold(0.8f),
          trustScoreDecayRate(0.01f),
          trustScoreRecoveryRate(0.001f),
          enableHoneypot(false),
          enableEncryption(true),
          enableObfuscation(true),
          enableCodeDiversification(false),
          enableVirtualizationDetection(true),
          enableSandboxDetection(true),
          notifyAdminOnHighThreats(true),
          notifyPlayerOnActionTaken(true) {}
};

/**
 * @brief Padrão de assinatura para detecção de cheats
 */
struct CheatSignature {
    enum class PatternType {
        MEMORY_PATTERN,     // Padrão na memória
        NETWORK_PATTERN,    // Padrão de rede
        BEHAVIOR_PATTERN,   // Padrão de comportamento
        API_CALL_PATTERN,   // Padrão de chamadas de API
        FILE_HASH,          // Hash de arquivo
        CODE_FINGERPRINT    // Fingerprint de código
    };
    
    std::string name;
    std::string description;
    PatternType type;
    ThreatType threatType;
    ThreatSeverity severity;
    
    // Padrão de detecção, dependendo do tipo
    std::variant<
        std::vector<uint8_t>,               // Padrão binário
        std::string,                        // String pattern ou hash
        std::function<bool(const void*)>    // Função de verificação customizada
    > pattern;
    
    // Máscaras e flags para padrões de memória/rede
    std::optional<std::vector<uint8_t>> mask;
    bool useWildcards;
    
    // Metadados
    float falsePositiveRate;  // Taxa histórica de falsos positivos
    uint32_t detectionCount;  // Número de detecções
    std::chrono::system_clock::time_point lastUpdated;
    std::string source;       // Fonte da assinatura
    
    CheatSignature() : type(PatternType::MEMORY_PATTERN),
                     threatType(ThreatType::UNKNOWN),
                     severity(ThreatSeverity::MEDIUM),
                     useWildcards(false),
                     falsePositiveRate(0.0f),
                     detectionCount(0) {}
};

/**
 * @brief Banco de dados de assinaturas de cheats
 */
class CheatSignatureDatabase {
public:
    CheatSignatureDatabase();
    ~CheatSignatureDatabase();
    
    // Gerenciamento de assinaturas
    bool addSignature(const CheatSignature& signature);
    bool updateSignature(const std::string& name, const CheatSignature& signature);
    bool removeSignature(const std::string& name);
    std::optional<CheatSignature> getSignature(const std::string& name) const;
    
    // Consultas
    std::vector<CheatSignature> getSignaturesForThreatType(ThreatType type) const;
    std::vector<CheatSignature> getSignaturesByPatternType(CheatSignature::PatternType type) const;
    std::vector<CheatSignature> getAllSignatures() const;
    
    // Verificação
    std::vector<std::string> checkMemoryPatterns(const void* memory, size_t size) const;
    std::vector<std::string> checkNetworkPatterns(const std::vector<uint8_t>& packet) const;
    std::vector<std::string> checkFileHash(const std::string& filePath) const;
    
    // Gestão de persistência
    bool loadFromFile(const std::string& filePath);
    bool saveToFile(const std::string& filePath) const;
    bool importFromJson(const std::string& jsonData);
    std::string exportToJson() const;
    
    // Atualizações
    bool updateFromServer(const std::string& serverUrl);
    std::chrono::system_clock::time_point getLastUpdateTime() const;
    
    // Estatísticas
    size_t getSignatureCount() const;
    size_t getDetectionCount() const;
    std::unordered_map<ThreatType, size_t> getDetectionsByType() const;
    
private:
    std::unordered_map<std::string, CheatSignature> m_signatures;
    std::chrono::system_clock::time_point m_lastUpdateTime;
    
    // Para otimizar a busca
    std::unordered_map<ThreatType, std::vector<std::string>> m_threatTypeIndex;
    std::unordered_map<CheatSignature::PatternType, std::vector<std::string>> m_patternTypeIndex;
    
    // Mecanismo de verificação otimizada para padrões de memória
    struct OptimizedPattern {
        std::vector<uint8_t> pattern;
        std::vector<uint8_t> mask;
        std::string signatureName;
    };
    std::vector<OptimizedPattern> m_optimizedMemoryPatterns;
    
    // Mutex para operações thread-safe
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    void rebuildIndices();
    void optimizePatterns();
    bool matchPattern(const void* data, size_t dataSize, 
                     const std::vector<uint8_t>& pattern,
                     const std::vector<uint8_t>& mask) const;
};

/**
 * @brief Monitor de segurança para um jogador
 */
class PlayerSecurityMonitor {
public:
    PlayerSecurityMonitor(const PlayerSecurityContext& playerContext);
    ~PlayerSecurityMonitor();
    
    // Estado
    const PlayerSecurityContext& getPlayerContext() const;
    void updatePlayerContext(const PlayerSecurityContext& context);
    float getTrustScore() const;
    
    // Monitoramento
    void recordAction(const std::string& actionType, 
                    const std::unordered_map<std::string, std::string>& details);
    void recordPacket(const network::DecodedPacket& packet);
    void recordWarning(const std::string& reason);
    void recordSuspiciousActivity(ThreatType type, const std::string& details);
    
    // Proteção
    void decreaseTrustScore(float amount);
    void increaseTrustScore(float amount);
    void resetTrustScore();
    
    // Histórico
    struct HistoryEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string type; // "action", "packet", "warning", "suspicious", etc.
        std::unordered_map<std::string, std::string> details;
    };
    
    std::vector<HistoryEntry> getRecentHistory(size_t maxEntries = 100) const;
    std::vector<HistoryEntry> getHistoryByType(const std::string& type, 
                                             size_t maxEntries = 100) const;
    
    // Estatísticas
    struct ActionStats {
        std::string actionType;
        uint32_t count;
        uint32_t countLast24Hours;
        uint32_t countLastHour;
        uint32_t countLastMinute;
        float actionsPerMinute;
    };
    
    std::vector<ActionStats> getActionStatistics() const;
    bool isRateLimited(const std::string& actionType) const;
    
private:
    PlayerSecurityContext m_playerContext;
    std::vector<HistoryEntry> m_history;
    
    // Contadores para estatísticas
    std::unordered_map<std::string, uint32_t> m_actionCounters;
    std::unordered_map<std::string, std::vector<std::chrono::system_clock::time_point>> m_actionTimestamps;
    
    // Cache de estatísticas
    std::unordered_map<std::string, ActionStats> m_cachedStats;
    std::chrono::system_clock::time_point m_lastStatsUpdate;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    void updateStatistics();
    void pruneTooOldHistory();
    float calculateActionsPerMinute(const std::string& actionType);
};

/**
 * @brief Verificador de integridade do cliente/servidor
 */
class IntegrityVerifier {
public:
    IntegrityVerifier();
    ~IntegrityVerifier();
    
    // Configuração
    void setChecksumValidationFrequency(uint32_t seconds);
    void setEnableCodeVerification(bool enable);
    void setEnableMemoryVerification(bool enable);
    void setEnableFileVerification(bool enable);
    
    // Verificações de integridade
    bool verifyClientIntegrity(uint32_t accountId, 
                             const std::string& clientVersion,
                             const std::unordered_map<std::string, std::string>& checksums);
    
    bool verifyServerIntegrity();
    bool verifyFileIntegrity(const std::string& filePath, const std::string& expectedChecksum);
    bool verifyMemoryIntegrity(uint32_t accountId);
    bool verifyCodeIntegrity(uint32_t accountId);
    
    // Checksums
    std::string calculateFileChecksum(const std::string& filePath) const;
    std::string calculateMemoryChecksum(const void* memory, size_t size) const;
    
    // Funções de desafio-resposta
    std::string generateChallenge(uint32_t accountId);
    bool verifyResponse(uint32_t accountId, const std::string& response);
    
private:
    uint32_t m_checksumValidationFrequencySeconds;
    bool m_enableCodeVerification;
    bool m_enableMemoryVerification;
    bool m_enableFileVerification;
    
    // Checksums conhecidos para arquivos do cliente
    std::unordered_map<std::string, std::string> m_knownFileChecksums;
    
    // Desafios ativos
    struct Challenge {
        std::string challenge;
        std::string expectedResponse;
        std::chrono::system_clock::time_point expirationTime;
    };
    
    std::unordered_map<uint32_t, Challenge> m_activeChallenge;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    std::string generateRandomString(size_t length) const;
    std::vector<uint8_t> diversifyCodeBlock(const std::vector<uint8_t>& codeBlock,
                                          uint32_t accountId) const;
};

/**
 * @brief Detector de anomalias comportamentais
 */
class AnomalyDetector {
public:
    AnomalyDetector();
    ~AnomalyDetector();
    
    // Configuração
    void setAnomalyThreshold(float threshold);
    void setAnalysisSensitivity(float sensitivity);
    void setEnableBehaviorModeling(bool enable);
    void setEnableClusterAnalysis(bool enable);
    
    // Análise
    void analyzePlayerBehavior(uint32_t accountId);
    void analyzeSystemBehavior();
    void analyzeNetworkBehavior();
    
    // Detecção de anomalias específicas
    bool detectSpeedHack(uint32_t accountId, 
                       const std::unordered_map<std::string, float>& metrics);
    
    bool detectTeleportHack(uint32_t accountId,
                          int32_t oldX, int32_t oldY, 
                          int32_t newX, int32_t newY,
                          uint32_t timestamp);
    
    bool detectResourceExploitation(uint32_t accountId,
                                 const std::string& resourceType,
                                 uint32_t amount,
                                 uint32_t timeSeconds);
    
    bool detectCombatAnomaly(uint32_t accountId,
                          const std::unordered_map<std::string, float>& combatMetrics);
    
    // Resultados e relatórios
    std::vector<ThreatReport> getDetectedAnomalies(uint32_t accountId) const;
    std::vector<ThreatReport> getSystemAnomalies() const;
    void clearAnomalies(uint32_t accountId);
    
    // Modelos comportamentais
    struct BehaviorProfile {
        std::unordered_map<std::string, float> metrics;
        std::unordered_map<std::string, std::pair<float, float>> normalRanges; // min, max
        std::chrono::system_clock::time_point lastUpdated;
    };
    
    BehaviorProfile getPlayerBehaviorProfile(uint32_t accountId) const;
    void updatePlayerBehaviorProfile(uint32_t accountId, const BehaviorProfile& profile);
    
private:
    float m_anomalyThreshold;
    float m_analysisSensitivity;
    bool m_enableBehaviorModeling;
    bool m_enableClusterAnalysis;
    
    // Perfis comportamentais
    std::unordered_map<uint32_t, BehaviorProfile> m_playerProfiles;
    BehaviorProfile m_systemProfile;
    BehaviorProfile m_networkProfile;
    
    // Anomalias detectadas
    std::unordered_map<uint32_t, std::vector<ThreatReport>> m_playerAnomalies;
    std::vector<ThreatReport> m_systemAnomalies;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    float calculateAnomalyScore(const std::unordered_map<std::string, float>& current,
                               const BehaviorProfile& profile);
    
    bool isValueAnomaly(float value, const std::pair<float, float>& normalRange);
    void updateNormalRange(std::pair<float, float>& range, float newValue);
};

/**
 * @brief Protetor de memória contra manipulação
 */
class MemoryProtector {
public:
    MemoryProtector();
    ~MemoryProtector();
    
    // Configuração
    void setEnableVirtualization(bool enable);
    void setEnableObfuscation(bool enable);
    void setEnableChecksumVerification(bool enable);
    void setVerificationFrequency(uint32_t milliseconds);
    
    // Proteção de memória
    bool protectMemoryRegion(void* address, size_t size, const std::string& regionName);
    bool unprotectMemoryRegion(const std::string& regionName);
    bool isRegionProtected(const std::string& regionName) const;
    
    // Verificação
    bool verifyMemoryRegion(const std::string& regionName);
    bool verifyAllRegions();
    
    // Detecção de intrusões
    std::vector<ThreatReport> detectMemoryManipulation();
    bool detectDebugger();
    bool detectCodeHooks();
    
    // Medidas anti-depuração e anti-manipulação
    void installAntiDebugMeasures();
    void obfuscateMemory(void* address, size_t size);
    
    // Eventos
    core::EventBus::SubscriptionID subscribeToMemoryViolation(
        std::function<void(const std::string&, void*, size_t)> callback);
    
private:
    struct ProtectedRegion {
        void* address;
        size_t size;
        std::string name;
        std::vector<uint8_t> originalContents;
        std::string checksum;
        std::chrono::system_clock::time_point lastVerification;
        bool isVirtualized;
        bool isObfuscated;
    };
    
    bool m_enableVirtualization;
    bool m_enableObfuscation;
    bool m_enableChecksumVerification;
    uint32_t m_verificationFrequencyMs;
    
    // Regiões protegidas
    std::unordered_map<std::string, ProtectedRegion> m_protectedRegions;
    
    // Thread de verificação contínua
    std::thread m_verificationThread;
    std::atomic<bool> m_running;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Event bus para notificações
    std::shared_ptr<core::EventBus> m_eventBus;
    
    // Métodos auxiliares
    void verificationRoutine();
    std::string calculateChecksum(const void* data, size_t size) const;
    std::vector<uint8_t> backupMemoryRegion(const void* address, size_t size) const;
    bool compareMemoryRegions(const void* current, const std::vector<uint8_t>& original, size_t size) const;
};

/**
 * @brief Validador de pacotes de rede
 */
class NetworkValidator {
public:
    NetworkValidator();
    ~NetworkValidator();
    
    // Configuração
    void setEnableEncryption(bool enable);
    void setEnablePacketValidation(bool enable);
    void setEnableRateLimiting(bool enable);
    void setMaxPacketsPerSecond(uint32_t limit);
    
    // Análise e validação
    network::ValidationState validatePacket(const network::DecodedPacket& packet,
                                         uint32_t accountId);
    
    bool detectPacketInjection(const network::DecodedPacket& packet,
                             uint32_t accountId);
    
    bool detectPacketManipulation(const network::DecodedPacket& packet,
                                uint32_t accountId);
    
    // Fingerprinting
    std::string calculatePacketFingerprint(const network::DecodedPacket& packet) const;
    bool isKnownFingerprint(const std::string& fingerprint) const;
    void addKnownFingerprint(const std::string& fingerprint);
    
    // Proteção
    std::vector<uint8_t> encryptPacket(const std::vector<uint8_t>& packetData,
                                    uint32_t accountId);
    
    std::vector<uint8_t> decryptPacket(const std::vector<uint8_t>& encryptedData,
                                    uint32_t accountId);
    
    std::string generateEncryptionKey(uint32_t accountId);
    bool rotateEncryptionKey(uint32_t accountId);
    
    // Estatísticas de rede
    struct NetworkStats {
        uint32_t packetsProcessed;
        uint32_t packetsRejected;
        uint32_t bytesProcessed;
        uint32_t avgPacketSize;
        uint32_t peakPacketsPerSecond;
        uint32_t currentPacketsPerSecond;
        std::unordered_map<network::ValidationState, uint32_t> validationResults;
    };
    
    NetworkStats getNetworkStats(uint32_t accountId) const;
    NetworkStats getGlobalNetworkStats() const;
    
private:
    bool m_enableEncryption;
    bool m_enablePacketValidation;
    bool m_enableRateLimiting;
    uint32_t m_maxPacketsPerSecond;
    
    // Fingerprints conhecidos
    std::unordered_set<std::string> m_knownFingerprints;
    
    // Cache de chaves de encriptação
    struct EncryptionContext {
        std::string currentKey;
        std::string previousKey;
        std::chrono::system_clock::time_point lastRotation;
        uint32_t packetSequence;
    };
    
    std::unordered_map<uint32_t, EncryptionContext> m_encryptionContexts;
    
    // Estatísticas por conta
    std::unordered_map<uint32_t, NetworkStats> m_accountStats;
    NetworkStats m_globalStats;
    
    // Tracking de rate limites
    std::unordered_map<uint32_t, std::vector<std::chrono::system_clock::time_point>> m_packetTimestamps;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    bool isRateLimited(uint32_t accountId);
    void updatePacketStats(uint32_t accountId, const network::DecodedPacket& packet, network::ValidationState result);
    std::string obfuscateKey(const std::string& key, uint32_t accountId) const;
};

/**
 * @brief Aplicador de patches de segurança ao cliente
 */
class ClientPatcher {
public:
    ClientPatcher();
    ~ClientPatcher();
    
    // Configuração
    void setEnableAutoPatch(bool enable);
    void setRequireSignedPatches(bool require);
    void setPatchInterval(uint32_t hours);
    
    // Patching
    struct PatchInfo {
        std::string patchId;
        std::string version;
        std::string description;
        std::vector<uint8_t> patchData;
        std::string checksum;
        std::string signature;
        std::chrono::system_clock::time_point releaseDate;
        bool isMandatory;
    };
    
    bool applyPatch(uint32_t accountId, const PatchInfo& patch);
    bool verifyPatchApplied(uint32_t accountId, const std::string& patchId);
    std::vector<PatchInfo> getAvailablePatches(uint32_t accountId, const std::string& clientVersion);
    std::vector<std::string> getInstalledPatches(uint32_t accountId);
    
    // Gestão de patches
    bool addPatch(const PatchInfo& patch);
    bool removePatch(const std::string& patchId);
    PatchInfo getPatchInfo(const std::string& patchId) const;
    
    // Geração de patches
    PatchInfo generateSecurityPatch(const std::string& description,
                                 const std::vector<CheatSignature>& signatures,
                                 bool isMandatory = true);
    
private:
    bool m_enableAutoPatch;
    bool m_requireSignedPatches;
    uint32_t m_patchIntervalHours;
    
    // Patches disponíveis
    std::unordered_map<std::string, PatchInfo> m_availablePatches;
    
    // Patches instalados por conta
    std::unordered_map<uint32_t, std::unordered_set<std::string>> m_installedPatches;
    
    // Chaves para assinatura
    std::string m_privateKey;
    std::string m_publicKey;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    std::string signPatch(const PatchInfo& patch) const;
    bool verifyPatchSignature(const PatchInfo& patch) const;
    std::string generatePatchId() const;
};

/**
 * @brief Protetor do servidor WYD
 */
class ServerShield {
public:
    ServerShield();
    ~ServerShield();
    
    // Configuração
    void setEnableDDoSProtection(bool enable);
    void setEnableConnectionValidation(bool enable);
    void setMaxConnectionsPerIp(uint32_t max);
    
    // Proteção de servidor
    bool validateConnection(const std::string& ipAddress, uint32_t accountId);
    bool isIpBlocked(const std::string& ipAddress) const;
    bool blockIp(const std::string& ipAddress, uint32_t durationMinutes, const std::string& reason);
    bool unblockIp(const std::string& ipAddress);
    
    // Proteção DDoS
    bool detectDDoSAttack();
    void mitigateDDoSAttack();
    
    // Monitoramento de servidor
    struct ServerStatus {
        uint32_t activeConnections;
        uint32_t connectionAttemptsPerMinute;
        uint32_t packetsProcessedPerSecond;
        uint32_t rejectedConnectionsPerMinute;
        float cpuUsage;
        float ramUsage;
        float networkBandwidth;
    };
    
    ServerStatus getServerStatus() const;
    
private:
    bool m_enableDDoSProtection;
    bool m_enableConnectionValidation;
    uint32_t m_maxConnectionsPerIp;
    
    // Lista de IPs bloqueados
    struct BlockedIp {
        std::string reason;
        std::chrono::system_clock::time_point expirationTime;
    };
    
    std::unordered_map<std::string, BlockedIp> m_blockedIps;
    
    // Contadores de conexão por IP
    std::unordered_map<std::string, uint32_t> m_connectionsPerIp;
    
    // Estatísticas de conexão
    std::vector<std::chrono::system_clock::time_point> m_connectionAttempts;
    std::vector<std::chrono::system_clock::time_point> m_rejectedConnections;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    void cleanupExpiredIpBlocks();
    bool isConnectionRateLimited(const std::string& ipAddress);
    uint32_t calculateConnectionsPerMinute() const;
    uint32_t calculateRejectedConnectionsPerMinute() const;
};

/**
 * @brief Analisador heurístico para detecção de novos cheats
 */
class HeuristicAnalyzer {
public:
    HeuristicAnalyzer();
    ~HeuristicAnalyzer();
    
    // Configuração
    void setSensitivity(float sensitivity);
    void setEnableLearning(bool enable);
    void setDetectionThreshold(float threshold);
    
    // Análise heurística
    ThreatReport analyzePlayerBehavior(uint32_t accountId,
                                     const std::unordered_map<std::string, float>& metrics);
    
    ThreatReport analyzePacketPattern(const network::DecodedPacket& packet,
                                    uint32_t accountId);
    
    ThreatReport analyzeMemoryAccess(uint32_t accountId,
                                   void* address,
                                   size_t size,
                                   const std::string& accessType);
    
    // Aprendizado
    void learnNormalBehavior(uint32_t accountId,
                           const std::unordered_map<std::string, float>& metrics);
    
    void learnNormalPacketPattern(const network::DecodedPacket& packet);
    
    void markAsLegitimate(const ThreatReport& report);
    void markAsMalicious(const ThreatReport& report);
    
    // Geração de assinaturas
    CheatSignature generateSignatureFromReport(const ThreatReport& report);
    
private:
    float m_sensitivity;
    bool m_enableLearning;
    float m_detectionThreshold;
    
    // Modelos normalizados
    std::unordered_map<std::string, std::pair<float, float>> m_behaviorRanges; // métrica -> (média, desvio)
    std::unordered_map<std::string, float> m_packetFrequencies; // tipo de pacote -> frequência normalizada
    
    // Cache de análises recentes
    struct AnalysisCache {
        std::vector<ThreatReport> recentReports;
        std::chrono::system_clock::time_point lastCleanup;
    };
    
    std::unordered_map<uint32_t, AnalysisCache> m_analysisCache;
    
    // Métricas de eficácia
    uint32_t m_truePositives;
    uint32_t m_falsePositives;
    uint32_t m_trueNegatives;
    uint32_t m_falseNegatives;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    float calculateBehaviorAnomaly(const std::unordered_map<std::string, float>& metrics);
    float calculatePacketAnomaly(const network::DecodedPacket& packet);
    float calculateMemoryAnomaly(void* address, size_t size, const std::string& accessType);
    void updateNormalModel(const std::string& metric, float value);
    void cleanupOldCacheEntries(uint32_t accountId);
};

/**
 * @brief Sistema de inteligência de ameaças
 */
class ThreatIntelligence {
public:
    ThreatIntelligence();
    ~ThreatIntelligence();
    
    // Configuração
    void setEnableCloudReporting(bool enable);
    void setEnableCollaborativeSecurity(bool enable);
    void setTelemetryLevel(uint32_t level); // 0=desativado, 1=mínimo, 2=normal, 3=detalhado
    
    // Inteligência de ameaças
    bool reportThreat(const ThreatReport& report);
    bool queryKnownThreats(const std::string& identifier);
    std::vector<ThreatReport> getRecentThreats(size_t maxCount = 100) const;
    
    // Inteligência específica
    bool isKnownCheatingSoftware(const std::string& processName, const std::string& hash) const;
    bool isKnownMaliciousIp(const std::string& ipAddress) const;
    bool isKnownBotPattern(const std::unordered_map<std::string, float>& metrics) const;
    
    // Compartilhamento de dados
    bool shareLocalThreatData();
    bool fetchRemoteThreatData();
    
    // Estatísticas
    struct ThreatStats {
        uint32_t reportedThreats;
        uint32_t confirmedThreats;
        std::unordered_map<ThreatType, uint32_t> threatsByType;
        std::unordered_map<std::string, uint32_t> threatsByIp;
        std::chrono::system_clock::time_point lastUpdate;
    };
    
    ThreatStats getThreatStats() const;
    
private:
    bool m_enableCloudReporting;
    bool m_enableCollaborativeSecurity;
    uint32_t m_telemetryLevel;
    
    // Base de conhecimento local
    std::vector<ThreatReport> m_localThreats;
    std::unordered_set<std::string> m_knownCheatingSoftware;
    std::unordered_set<std::string> m_knownMaliciousIps;
    
    // Cache de consultas
    std::unordered_map<std::string, bool> m_threatQueryCache;
    
    // Estatísticas
    ThreatStats m_stats;
    
    // URL do serviço de inteligência
    std::string m_threatIntelligenceServiceUrl;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    bool submitToCloud(const ThreatReport& report);
    bool queryCloud(const std::string& identifier);
    std::string anonymizeThreatData(const ThreatReport& report) const;
    void updateThreatStats(const ThreatReport& report);
};

/**
 * @brief Monitor de segurança principal
 */
class SecurityMonitor {
public:
    SecurityMonitor();
    ~SecurityMonitor();
    
    // Configuração
    void setConfig(const SecurityConfig& config);
    SecurityConfig& getConfig();
    const SecurityConfig& getConfig() const;
    
    // Monitoramento de jogador
    std::shared_ptr<PlayerSecurityMonitor> getPlayerMonitor(uint32_t accountId);
    void removePlayerMonitor(uint32_t accountId);
    std::vector<uint32_t> getMonitoredPlayers() const;
    
    // Tratamento de ameaças
    void reportThreat(const ThreatReport& report);
    SecurityAction determineThreatAction(const ThreatReport& report) const;
    bool executeThreatAction(uint32_t accountId, SecurityAction action, const std::string& reason);
    
    // Notificações
    void notifyAdmin(const ThreatReport& report);
    void notifyPlayer(uint32_t accountId, const std::string& message);
    
    // Dashboard de segurança
    struct SecurityDashboard {
        uint32_t activePlayers;
        uint32_t monitoredPlayers;
        uint32_t detectedThreats;
        uint32_t actionsExecuted;
        
        std::unordered_map<ThreatSeverity, uint32_t> threatsBySeverity;
        std::unordered_map<ThreatType, uint32_t> threatsByType;
        std::unordered_map<SecurityAction, uint32_t> actionsTaken;
        
        std::vector<ThreatReport> recentThreats;
        std::chrono::system_clock::time_point lastUpdate;
    };
    
    SecurityDashboard getDashboard() const;
    void updateDashboard();
    
    // Exportação de logs
    bool exportSecurityLogs(const std::string& filePath) const;
    
private:
    SecurityConfig m_config;
    
    // Monitores de jogador
    std::unordered_map<uint32_t, std::shared_ptr<PlayerSecurityMonitor>> m_playerMonitors;
    
    // Histórico de ameaças
    std::vector<ThreatReport> m_threatHistory;
    
    // Contadores de ações
    std::unordered_map<SecurityAction, uint32_t> m_actionCounts;
    
    // Dashboard
    SecurityDashboard m_dashboard;
    std::chrono::system_clock::time_point m_lastDashboardUpdate;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos auxiliares
    void pruneOldThreats();
    void updateActionStats(SecurityAction action);
    float calculateThreatScore(const ThreatReport& report) const;
};

/**
 * @brief Sistema de proteção completo
 */
class SecurityGuardian {
public:
    // Singleton
    static SecurityGuardian& getInstance();
    
    // Inicialização
    bool initialize(const SecurityConfig& config);
    bool shutdown();
    bool isInitialized() const { return m_initialized; }
    
    // Acesso a subsistemas
    SecurityMonitor& getSecurityMonitor() { return *m_securityMonitor; }
    IntegrityVerifier& getIntegrityVerifier() { return *m_integrityVerifier; }
    AnomalyDetector& getAnomalyDetector() { return *m_anomalyDetector; }
    CheatSignatureDatabase& getSignatureDatabase() { return *m_signatureDatabase; }
    MemoryProtector& getMemoryProtector() { return *m_memoryProtector; }
    NetworkValidator& getNetworkValidator() { return *m_networkValidator; }
    ClientPatcher& getClientPatcher() { return *m_clientPatcher; }
    ServerShield& getServerShield() { return *m_serverShield; }
    HeuristicAnalyzer& getHeuristicAnalyzer() { return *m_heuristicAnalyzer; }
    ThreatIntelligence& getThreatIntelligence() { return *m_threatIntelligence; }
    
    // Acesso a configuração
    SecurityConfig& getConfig() { return m_config; }
    const SecurityConfig& getConfig() const { return m_config; }
    
    // Operações de alto nível
    bool protectClient(uint32_t accountId, const std::string& clientVersion);
    bool protectServer();
    bool validateClientSecurity(uint32_t accountId, 
                             const std::unordered_map<std::string, std::string>& securityData);
    
    // Validação de pacotes
    network::ValidationState validatePacket(const network::DecodedPacket& packet,
                                         uint32_t accountId);
    
    // Proteção de memória
    bool protectCriticalMemory();
    
    // Monitoramento de jogador
    void monitorPlayerAction(uint32_t accountId,
                           const std::string& actionType,
                           const std::unordered_map<std::string, std::string>& details);
    
    // Detecção e resposta a ameaças
    void reportSuspiciousActivity(uint32_t accountId,
                                ThreatType threatType,
                                const std::string& details,
                                ThreatSeverity severity = ThreatSeverity::MEDIUM);
    
    // Eventos
    core::EventBus::SubscriptionID subscribeToSecurityEvents(
        std::function<void(const ThreatReport&)> callback);
    
    // Métricas e estatísticas
    SecurityMonitor::SecurityDashboard getSecurityDashboard() const;
    
private:
    SecurityGuardian();
    ~SecurityGuardian();
    
    // Prevenção de cópia
    SecurityGuardian(const SecurityGuardian&) = delete;
    SecurityGuardian& operator=(const SecurityGuardian&) = delete;
    
    bool m_initialized;
    SecurityConfig m_config;
    
    // Componentes de segurança
    std::unique_ptr<SecurityMonitor> m_securityMonitor;
    std::unique_ptr<IntegrityVerifier> m_integrityVerifier;
    std::unique_ptr<AnomalyDetector> m_anomalyDetector;
    std::unique_ptr<CheatSignatureDatabase> m_signatureDatabase;
    std::unique_ptr<MemoryProtector> m_memoryProtector;
    std::unique_ptr<NetworkValidator> m_networkValidator;
    std::unique_ptr<ClientPatcher> m_clientPatcher;
    std::unique_ptr<ServerShield> m_serverShield;
    std::unique_ptr<HeuristicAnalyzer> m_heuristicAnalyzer;
    std::unique_ptr<ThreatIntelligence> m_threatIntelligence;
    
    // Manutenção do sistema
    std::thread m_maintenanceThread;
    std::atomic<bool> m_running;
    
    // Event bus
    std::shared_ptr<core::EventBus> m_eventBus;
    
    // Rotinas de manutenção
    void maintenanceRoutine();
    void loadSignatures();
    void updateThreatIntelligence();
    void cleanupOldData();
};

} // namespace security
} // namespace wydstudio

#endif // WYDSTUDIO_SECURITY_GUARDIAN_H