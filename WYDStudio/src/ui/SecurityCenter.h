#ifndef WYDSTUDIO_SECURITY_CENTER_H
#define WYDSTUDIO_SECURITY_CENTER_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>
#include <chrono>
#include <variant>
#include <nlohmann/json.hpp>
#include "../core/WYDStudioFacade.h"
#include "../security/SecurityGuardian.h"
#include "../network/NetworkMonitor.h"

namespace wydstudio {
namespace ui {

using json = nlohmann::json;

/**
 * @brief Tipo de ameaça detectada
 */
enum class ThreatType {
    CHEAT_ENGINE,        // CheatEngine ou ferramentas similares
    PACKET_MANIPULATION, // Manipulação de pacotes
    SPEED_HACK,          // Speed hack
    TELEPORT_HACK,       // Hack de teleporte
    WALL_HACK,           // Wall hack / NoClip
    ITEM_DUPE,           // Duplicação de itens
    GOLD_HACK,           // Hack de dinheiro
    MACRO_BOT,           // Bot / Automação
    MEMORY_ALTERATION,   // Alteração direta de memória
    CLIENT_MODIFICATION, // Modificação do cliente
    ROOTKIT,             // Rootkit / Escondendo processos
    IMPERSONATION,       // Personificação de outro jogador
    DATABASE_TAMPERING,  // Manipulação de banco de dados
    CUSTOM_EXPLOIT       // Exploit personalizado
};

/**
 * @brief Nível de severidade de ameaça
 */
enum class ThreatSeverity {
    LOW,        // Baixa severidade, possível falso positivo
    MEDIUM,     // Severidade média, suspeito
    HIGH,       // Alta severidade, quase certo
    CRITICAL    // Crítica, confirmado com certeza
};

/**
 * @brief Status de uma ameaça
 */
enum class ThreatStatus {
    DETECTED,      // Recém detectada
    INVESTIGATING, // Em investigação
    CONFIRMED,     // Confirmada
    MITIGATED,     // Mitigada
    FALSE_POSITIVE,// Falso positivo
    RESOLVED,      // Resolvida
    ARCHIVED       // Arquivada
};

/**
 * @brief Tipo de ação de mitigação
 */
enum class MitigationActionType {
    NONE,             // Nenhuma ação
    WARNING,          // Aviso ao jogador
    KICK,             // Expulsar do jogo
    TEMP_BAN,         // Banimento temporário
    PERM_BAN,         // Banimento permanente
    RESTRICT_TRADE,   // Restringir comércio
    RESTRICT_CHAT,    // Restringir chat
    RESTRICT_PVP,     // Restringir PvP
    ITEM_REMOVAL,     // Remover items
    GOLD_PENALTY,     // Penalidade em dinheiro
    QUARANTINE,       // Colocar em quarentena
    SURVEILLANCE,     // Colocar sob vigilância
    CUSTOM            // Ação personalizada
};

/**
 * @brief Tipo de evidência coletada
 */
enum class EvidenceType {
    PACKET_CAPTURE,     // Captura de pacotes
    MEMORY_DUMP,        // Dump de memória
    SCREENSHOT,         // Captura de tela
    VIDEO_RECORDING,    // Gravação de vídeo
    LOG_ENTRY,          // Entrada de log
    PLAYER_REPORT,      // Denúncia de jogador
    SYSTEM_DETECTION,   // Detecção automática do sistema
    ADMIN_OBSERVATION,  // Observação de admin
    STATISTICAL_ANOMALY,// Anomalia estatística
    METADATA            // Metadados diversos
};

/**
 * @brief Fonte de detecção de ameaça
 */
enum class DetectionSource {
    PLAYER_REPORT,      // Denúncia de jogador
    NETWORK_MONITOR,    // Monitor de rede
    CLIENT_PROTECTION,  // Proteção no cliente
    SERVER_VALIDATION,  // Validação no servidor
    STATISTICAL_ANALYSIS,// Análise estatística
    ADMIN_INVESTIGATION,// Investigação de admin
    MACHINE_LEARNING,   // Sistema de machine learning
    HONEYPOT,           // Sistema de honeypot
    SIGNATURE_MATCH,    // Correspondência de assinatura
    BEHAVIOR_ANALYSIS   // Análise comportamental
};

/**
 * @brief Informações de uma evidência específica
 */
struct SecurityEvidence {
    int32_t id;
    EvidenceType type;
    std::string description;
    std::string content;       // Conteúdo textual ou path para arquivo
    std::vector<uint8_t> data; // Dados binários (se aplicável)
    std::chrono::system_clock::time_point timestamp;
    std::string collectedBy;
    float relevanceScore;      // 0.0 a 1.0
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Dados de uma ação de mitigação
 */
struct MitigationAction {
    int32_t id;
    MitigationActionType type;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::string executedBy;
    bool successful;
    std::string result;
    std::chrono::system_clock::time_point expirationTime; // Para ações temporárias
    std::map<std::string, std::string> parameters;
};

/**
 * @brief Dados de um incidente de segurança
 */
struct SecurityIncident {
    int32_t id;
    ThreatType type;
    ThreatSeverity severity;
    ThreatStatus status;
    std::string title;
    std::string description;
    
    // Informações do alvo
    struct {
        std::string accountId;
        std::string accountName;
        std::string characterId;
        std::string characterName;
        std::string ipAddress;
        std::string hardwareId;
        std::map<std::string, std::string> additionalIdentifiers;
    } target;
    
    // Dados de detecção
    struct {
        DetectionSource source;
        std::chrono::system_clock::time_point detectionTime;
        std::string detectedBy;
        float confidenceScore; // 0.0 a 1.0
        std::map<std::string, float> detectionFactors;
    } detection;
    
    // Evidências coletadas
    std::vector<SecurityEvidence> evidences;
    
    // Ações de mitigação
    std::vector<MitigationAction> mitigationActions;
    
    // Histórico de alterações de status
    struct StatusChange {
        ThreatStatus oldStatus;
        ThreatStatus newStatus;
        std::chrono::system_clock::time_point timestamp;
        std::string changedBy;
        std::string reason;
    };
    std::vector<StatusChange> statusHistory;
    
    // Dados relacionados
    std::vector<int32_t> relatedIncidentIds;
    std::map<std::string, std::string> tags;
    std::map<std::string, std::string> notes;
};

/**
 * @brief Resultado de operações de segurança
 */
struct SecurityOperationResult {
    bool success;
    std::string message;
    std::variant<
        std::monostate,
        SecurityIncident,
        SecurityEvidence,
        MitigationAction
    > data;
};

/**
 * @brief Filtros para busca de incidentes
 */
struct IncidentFilter {
    std::optional<ThreatType> type;
    std::optional<ThreatSeverity> minSeverity;
    std::optional<ThreatStatus> status;
    std::optional<std::string> targetAccount;
    std::optional<std::string> targetCharacter;
    std::optional<std::string> targetIp;
    std::optional<DetectionSource> source;
    std::optional<std::chrono::system_clock::time_point> startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    std::optional<float> minConfidence;
    std::optional<std::string> containsTag;
    std::optional<std::string> textSearch;
};

/**
 * @brief Estatísticas de segurança
 */
struct SecurityStatistics {
    // Contagem por tipo de ameaça
    std::map<ThreatType, int32_t> incidentsByType;
    
    // Contagem por severidade
    std::map<ThreatSeverity, int32_t> incidentsBySeverity;
    
    // Contagem por status
    std::map<ThreatStatus, int32_t> incidentsByStatus;
    
    // Contagem por fonte de detecção
    std::map<DetectionSource, int32_t> incidentsBySource;
    
    // Estatísticas temporais
    struct {
        int32_t last24Hours;
        int32_t last7Days;
        int32_t last30Days;
        int32_t total;
    } temporalStats;
    
    // Estatísticas de ações de mitigação
    std::map<MitigationActionType, int32_t> mitigationsByType;
    
    // Estatísticas de reincidência
    struct {
        int32_t uniqueAccounts;
        int32_t repeatOffenders;
        float repeatRate;
    } recidivismStats;
    
    // Estatísticas de eficácia
    struct {
        float detectionRate;
        float falsePositiveRate;
        float mitigationSuccessRate;
        float averageResolutionTimeHours;
    } effectivenessStats;
};

/**
 * @brief Regra de detecção de ameaças
 */
struct DetectionRule {
    int32_t id;
    std::string name;
    std::string description;
    ThreatType threatType;
    ThreatSeverity severity;
    bool enabled;
    
    // Critérios da regra
    struct {
        std::string logic;         // Ex: "AND", "OR", "SEQUENCE"
        std::vector<std::string> conditions;
        std::map<std::string, std::string> parameters;
    } criteria;
    
    // Ações automáticas
    struct {
        bool autoMitigate;
        MitigationActionType mitigation;
        std::map<std::string, std::string> mitigationParams;
        bool notifyAdmin;
        bool collectEvidence;
        std::vector<EvidenceType> evidenceToCollect;
    } actions;
    
    // Metadados
    struct {
        std::chrono::system_clock::time_point createdAt;
        std::chrono::system_clock::time_point updatedAt;
        std::string author;
        int32_t version;
        std::map<std::string, std::string> tags;
    } metadata;
};

/**
 * @brief Centro de Segurança do WYDStudio
 */
class SecurityCenter {
public:
    /**
     * @brief Construtor
     * @param facade Referência ao facade principal do WYDStudio
     */
    SecurityCenter(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~SecurityCenter();
    
    /**
     * @brief Inicializa o centro de segurança
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Obtém incidentes de segurança com filtragem
     * @param filter Filtros a aplicar
     * @param maxResults Número máximo de resultados
     * @param offset Deslocamento para paginação
     * @return Lista de incidentes correspondentes
     */
    std::vector<SecurityIncident> getIncidents(
        const IncidentFilter& filter,
        int32_t maxResults = 100,
        int32_t offset = 0);
    
    /**
     * @brief Obtém um incidente pelo ID
     * @param incidentId ID do incidente
     * @return Incidente ou nulo se não encontrado
     */
    std::optional<SecurityIncident> getIncidentById(int32_t incidentId);
    
    /**
     * @brief Cria um novo incidente de segurança
     * @param incident Dados do incidente
     * @return Resultado da operação
     */
    SecurityOperationResult createIncident(const SecurityIncident& incident);
    
    /**
     * @brief Atualiza um incidente existente
     * @param incident Dados atualizados do incidente
     * @return Resultado da operação
     */
    SecurityOperationResult updateIncident(const SecurityIncident& incident);
    
    /**
     * @brief Altera o status de um incidente
     * @param incidentId ID do incidente
     * @param newStatus Novo status
     * @param reason Motivo da alteração
     * @return Resultado da operação
     */
    SecurityOperationResult changeIncidentStatus(
        int32_t incidentId,
        ThreatStatus newStatus,
        const std::string& reason);
    
    /**
     * @brief Adiciona uma evidência a um incidente
     * @param incidentId ID do incidente
     * @param evidence Dados da evidência
     * @return Resultado da operação
     */
    SecurityOperationResult addEvidence(
        int32_t incidentId,
        const SecurityEvidence& evidence);
    
    /**
     * @brief Remove uma evidência de um incidente
     * @param incidentId ID do incidente
     * @param evidenceId ID da evidência
     * @return Resultado da operação
     */
    SecurityOperationResult removeEvidence(
        int32_t incidentId,
        int32_t evidenceId);
    
    /**
     * @brief Aplica uma ação de mitigação a um incidente
     * @param incidentId ID do incidente
     * @param action Dados da ação
     * @return Resultado da operação
     */
    SecurityOperationResult applyMitigationAction(
        int32_t incidentId,
        const MitigationAction& action);
    
    /**
     * @brief Reverte uma ação de mitigação
     * @param incidentId ID do incidente
     * @param actionId ID da ação
     * @param reason Motivo da reversão
     * @return Resultado da operação
     */
    SecurityOperationResult revertMitigationAction(
        int32_t incidentId,
        int32_t actionId,
        const std::string& reason);
    
    /**
     * @brief Vincula incidentes relacionados
     * @param sourceIncidentId ID do incidente de origem
     * @param targetIncidentId ID do incidente a vincular
     * @return Resultado da operação
     */
    SecurityOperationResult linkIncidents(
        int32_t sourceIncidentId,
        int32_t targetIncidentId);
    
    /**
     * @brief Desvincula incidentes relacionados
     * @param sourceIncidentId ID do incidente de origem
     * @param targetIncidentId ID do incidente a desvincular
     * @return Resultado da operação
     */
    SecurityOperationResult unlinkIncidents(
        int32_t sourceIncidentId,
        int32_t targetIncidentId);
    
    /**
     * @brief Obtém estatísticas de segurança
     * @param timeFrame Período de tempo (em dias, 0 = todos)
     * @return Estatísticas de segurança
     */
    SecurityStatistics getStatistics(int32_t timeFrame = 30);
    
    /**
     * @brief Gera um relatório de segurança
     * @param filter Filtros para incluir no relatório
     * @param includeEvidences Se deve incluir evidências detalhadas
     * @param format Formato do relatório ("pdf", "html", "json", "txt")
     * @return Dados do relatório gerado
     */
    std::vector<uint8_t> generateSecurityReport(
        const IncidentFilter& filter,
        bool includeEvidences,
        const std::string& format);
    
    /**
     * @brief Obtém regras de detecção
     * @param enabledOnly Se deve retornar apenas regras ativas
     * @return Lista de regras de detecção
     */
    std::vector<DetectionRule> getDetectionRules(bool enabledOnly = false);
    
    /**
     * @brief Obtém uma regra de detecção pelo ID
     * @param ruleId ID da regra
     * @return Regra ou nulo se não encontrada
     */
    std::optional<DetectionRule> getDetectionRuleById(int32_t ruleId);
    
    /**
     * @brief Cria uma nova regra de detecção
     * @param rule Dados da regra
     * @return Resultado da operação
     */
    SecurityOperationResult createDetectionRule(const DetectionRule& rule);
    
    /**
     * @brief Atualiza uma regra de detecção existente
     * @param rule Dados atualizados da regra
     * @return Resultado da operação
     */
    SecurityOperationResult updateDetectionRule(const DetectionRule& rule);
    
    /**
     * @brief Remove uma regra de detecção
     * @param ruleId ID da regra a remover
     * @return Resultado da operação
     */
    SecurityOperationResult removeDetectionRule(int32_t ruleId);
    
    /**
     * @brief Ativa ou desativa uma regra de detecção
     * @param ruleId ID da regra
     * @param enabled Estado desejado
     * @return Resultado da operação
     */
    SecurityOperationResult setRuleEnabled(int32_t ruleId, bool enabled);
    
    /**
     * @brief Testa uma regra de detecção contra dados históricos
     * @param rule Regra a testar
     * @param daysToAnalyze Dias de dados para analisar
     * @return Resultado do teste com estatísticas
     */
    std::map<std::string, std::variant<int32_t, float, std::string>> testDetectionRule(
        const DetectionRule& rule,
        int32_t daysToAnalyze);
    
    /**
     * @brief Executa uma verificação de segurança completa no servidor
     * @param scanType Tipo de verificação ("quick", "standard", "deep")
     * @return Resultado da verificação
     */
    SecurityOperationResult runSecurityScan(const std::string& scanType);
    
    /**
     * @brief Executa uma investigação dirigida a um alvo específico
     * @param targetType Tipo de alvo ("account", "character", "ip", "hwid")
     * @param targetId Identificador do alvo
     * @return Resultado da investigação
     */
    SecurityOperationResult investigateTarget(
        const std::string& targetType,
        const std::string& targetId);
    
    /**
     * @brief Obtém histórico de incidentes para um alvo
     * @param targetType Tipo de alvo ("account", "character", "ip", "hwid")
     * @param targetId Identificador do alvo
     * @return Lista de incidentes associados ao alvo
     */
    std::vector<SecurityIncident> getTargetIncidentHistory(
        const std::string& targetType,
        const std::string& targetId);
    
    /**
     * @brief Registra um callback para notificações de incidentes
     * @param callback Função de callback
     * @return ID de registro (para cancelamento posterior)
     */
    uint32_t registerIncidentCallback(
        std::function<void(const SecurityIncident&)> callback);
    
    /**
     * @brief Cancela registro de callback
     * @param callbackId ID do callback a cancelar
     */
    void unregisterCallback(uint32_t callbackId);
    
    /**
     * @brief Define o nível de proteção global
     * @param level Nível de proteção (0-100, com 100 sendo o mais rigoroso)
     * @return true se aplicado com sucesso
     */
    bool setProtectionLevel(int32_t level);
    
    /**
     * @brief Obtém o nível de proteção global atual
     * @return Nível de proteção
     */
    int32_t getProtectionLevel() const;
    
    /**
     * @brief Define configurações de segurança específicas
     * @param settings Mapa de configurações e valores
     * @return true se aplicado com sucesso
     */
    bool setSecuritySettings(const std::map<std::string, std::string>& settings);
    
    /**
     * @brief Obtém configurações de segurança atuais
     * @return Mapa de configurações e valores
     */
    std::map<std::string, std::string> getSecuritySettings() const;

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade principal
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Componentes de segurança
    std::shared_ptr<security::SecurityGuardian> m_securityGuardian;
    std::shared_ptr<network::NetworkMonitor> m_networkMonitor;
    
    // Cache e dados internos
    std::map<int32_t, SecurityIncident> m_incidentCache;
    std::map<int32_t, DetectionRule> m_ruleCache;
    int32_t m_currentProtectionLevel;
    std::map<std::string, std::string> m_securitySettings;
    
    // Índices de pesquisa
    struct {
        std::multimap<std::string, int32_t> accountToIncidents;
        std::multimap<std::string, int32_t> characterToIncidents;
        std::multimap<std::string, int32_t> ipToIncidents;
        std::multimap<std::string, int32_t> hwidToIncidents;
    } m_indices;
    
    // Método de inicialização de caches
    void initializeCaches();
    void buildSearchIndices();
    
    // Métodos de acesso a dados
    void loadIncidents();
    void loadDetectionRules();
    void saveIncident(const SecurityIncident& incident);
    void saveDetectionRule(const DetectionRule& rule);
    
    // Métodos de validação
    bool validateIncident(const SecurityIncident& incident, std::string& errorMessage);
    bool validateEvidence(const SecurityEvidence& evidence, std::string& errorMessage);
    bool validateMitigationAction(const MitigationAction& action, std::string& errorMessage);
    bool validateDetectionRule(const DetectionRule& rule, std::string& errorMessage);
    
    // Métodos auxiliares
    int32_t generateUniqueId(const std::string& entityType);
    std::string serializeIncidentToJson(const SecurityIncident& incident);
    SecurityIncident deserializeIncidentFromJson(const std::string& json);
    std::string serializeRuleToJson(const DetectionRule& rule);
    DetectionRule deserializeRuleFromJson(const std::string& json);
};

}} // namespace wydstudio::ui

#endif // WYDSTUDIO_SECURITY_CENTER_H