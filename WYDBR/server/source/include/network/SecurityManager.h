/**
 * SecurityManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/network/SecurityManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _SECURITY_MANAGER_H_
#define _SECURITY_MANAGER_H_

/**
 * @file SecurityManager.h
 * @brief Gerenciador de segurança do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de segurança de rede do WYDBR,
 * responsável por proteger o servidor contra ataques e cheats,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>

#include "../GlobalDef.h"
#include "../Basedef.h"

namespace wydbr {
namespace network {

/**
 * @brief Tipo de violação de segurança
 */
enum /**
 * Classe WYDSecurityViolationType
 * 
 * Esta classe implementa a funcionalidade WYDSecurityViolationType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SecurityViolationType {
    NONE = 0,                       // Nenhum
    PACKET_FLOOD = 1,               // Inundação de pacotes
    INVALID_PACKET = 2,             // Pacote inválido
    PACKET_MANIPULATION = 3,        // Manipulação de pacote
    PACKET_INJECTION = 4,           // Injeção de pacote
    PACKET_REPLAY = 5,              // Repetição de pacote
    INVALID_SEQUENCE = 6,           // Sequência inválida
    INVALID_CHECKSUM = 7,           // Checksum inválido
    INVALID_STATE = 8,              // Estado inválido
    INVALID_COMMAND = 9,            // Comando inválido
    UNAUTHORIZED_ACCESS = 10,       // Acesso não autorizado
    SPEEDHACK = 11,                 // Speedhack
    CLIENT_MODIFICATION = 12,       // Modificação do cliente
    MEMORY_MANIPULATION = 13,       // Manipulação de memória
    PROCESS_INJECTION = 14,         // Injeção de processo
    IP_BAN = 15,                    // Ban de IP
    ACCOUNT_BAN = 16,               // Ban de conta
    SUSPICIOUS_BEHAVIOR = 17,       // Comportamento suspeito
    TELEPORT_HACK = 18,             // Hack de teleporte
    MAP_HACK = 19,                  // Hack de mapa
    ITEM_DUPLICATION = 20,          // Duplicação de item
    GOLD_DUPLICATION = 21,          // Duplicação de ouro
    XP_HACK = 22,                   // Hack de XP
    BOT_DETECTION = 23,             // Detecção de bot
    MULTI_CLIENT = 24,              // Multi cliente
    CUSTOM_1 = 25,                  // Personalizado 1
    CUSTOM_2 = 26,                  // Personalizado 2
    CUSTOM_3 = 27,                  // Personalizado 3
    UNKNOWN = 28                    // Desconhecido
};

/**
 * @brief Nível de severidade
 */
enum /**
 * Classe WYDSeverityLevel
 * 
 * Esta classe implementa a funcionalidade WYDSeverityLevel conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SeverityLevel {
    NONE = 0,                       // Nenhum
    INFO = 1,                       // Informação
    LOW = 2,                        // Baixo
    MEDIUM = 3,                     // Médio
    HIGH = 4,                       // Alto
    CRITICAL = 5,                   // Crítico
    CUSTOM_1 = 6,                   // Personalizado 1
    CUSTOM_2 = 7,                   // Personalizado 2
    CUSTOM_3 = 8,                   // Personalizado 3
    UNKNOWN = 9                     // Desconhecido
};

/**
 * @brief Ação de segurança
 */
enum /**
 * Classe WYDSecurityAction
 * 
 * Esta classe implementa a funcionalidade WYDSecurityAction conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SecurityAction {
    NONE = 0,                       // Nenhum
    LOG = 1,                        // Log
    WARN = 2,                       // Aviso
    DISCONNECT = 3,                 // Desconectar
    TEMP_BAN = 4,                   // Ban temporário
    PERM_BAN = 5,                   // Ban permanente
    ACCOUNT_LOCK = 6,               // Bloqueio de conta
    RATE_LIMIT = 7,                 // Limite de taxa
    BLOCK_PACKET = 8,               // Bloquear pacote
    CUSTOM_1 = 9,                   // Personalizado 1
    CUSTOM_2 = 10,                  // Personalizado 2
    CUSTOM_3 = 11,                  // Personalizado 3
    UNKNOWN = 12                    // Desconhecido
};

/**
 * @brief Regra de segurança
 */
struct SecurityRule {
    uint32_t RuleID;                // ID da regra
    std::string Name;               // Nome
    std::string Description;        // Descrição
    SecurityViolationType Type;     // Tipo
    SeverityLevel Severity;         // Severidade
    SecurityAction Action;          // Ação
    bool Enabled;                   // Habilitada
    uint32_t ThresholdCount;        // Contagem de limite
    uint32_t ThresholdTime;         // Tempo de limite
    uint32_t BanDuration;           // Duração do ban
    
    SecurityRule() : RuleID(0), Type(SecurityViolationType::NONE),
        Severity(SeverityLevel::NONE), Action(SecurityAction::NONE),
        Enabled(true), ThresholdCount(0), ThresholdTime(0), BanDuration(0) {}
};

/**
 * @brief Violação de segurança
 */
struct SecurityViolation {
    uint32_t ViolationID;           // ID da violação
    uint32_t RuleID;                // ID da regra
    uint32_t EntityID;              // ID da entidade
    std::string IP;                 // Endereço IP
    uint32_t AccountID;             // ID da conta
    uint32_t CharacterID;           // ID do personagem
    uint32_t Timestamp;             // Timestamp
    std::string Details;            // Detalhes
    SecurityViolationType Type;     // Tipo
    SeverityLevel Severity;         // Severidade
    SecurityAction Action;          // Ação
    bool Processed;                 // Processado
    
    SecurityViolation() : ViolationID(0), RuleID(0), EntityID(0),
        AccountID(0), CharacterID(0), Timestamp(0),
        Type(SecurityViolationType::NONE), Severity(SeverityLevel::NONE),
        Action(SecurityAction::NONE), Processed(false) {}
};

/**
 * @brief Estatísticas de segurança
 */
struct SecurityStats {
    uint32_t TotalViolations;       // Total de violações
    uint32_t TotalRules;            // Total de regras
    uint32_t ActiveBans;            // Bans ativos
    uint32_t TempBans;              // Bans temporários
    uint32_t PermBans;              // Bans permanentes
    uint32_t BlockedPackets;        // Pacotes bloqueados
    uint32_t Disconnections;        // Desconexões
    uint32_t RateLimits;            // Limites de taxa
    std::map<SecurityViolationType, uint32_t> ViolationsByType; // Violações por tipo
    std::map<SeverityLevel, uint32_t> ViolationsBySeverity; // Violações por severidade
    
    SecurityStats() : TotalViolations(0), TotalRules(0),
        ActiveBans(0), TempBans(0), PermBans(0),
        BlockedPackets(0), Disconnections(0), RateLimits(0) {}
};

/**
 * @brief Configuração de segurança
 */
struct SecurityConfig {
    bool EnablePacketValidation;    // Habilitar validação de pacote
    bool EnableChecksumValidation;  // Habilitar validação de checksum
    bool EnableSequenceValidation;  // Habilitar validação de sequência
    bool EnableStateValidation;     // Habilitar validação de estado
    bool EnablePacketRateLimit;     // Habilitar limite de taxa de pacote
    bool EnableIPBan;               // Habilitar ban de IP
    bool EnableAccountBan;          // Habilitar ban de conta
    bool EnableLogging;             // Habilitar log
    uint32_t MaxViolationStoreTime; // Tempo máximo de armazenamento de violação
    uint32_t MaxPacketsPerSecond;   // Máximo de pacotes por segundo
    uint32_t MaxConnectionsPerIP;   // Máximo de conexões por IP
    
    SecurityConfig() : EnablePacketValidation(true),
        EnableChecksumValidation(true), EnableSequenceValidation(true),
        EnableStateValidation(true), EnablePacketRateLimit(true),
        EnableIPBan(true), EnableAccountBan(true), EnableLogging(true),
        MaxViolationStoreTime(604800), MaxPacketsPerSecond(100),
        MaxConnectionsPerIP(5) {}
};

/**
 * @brief Entrada de ban
 */
struct BanEntry {
    uint32_t BanID;                 // ID do ban
    std::string IP;                 // Endereço IP
    uint32_t AccountID;             // ID da conta
    uint32_t CharacterID;           // ID do personagem
    std::string Reason;             // Motivo
    uint32_t StartTime;             // Tempo de início
    uint32_t EndTime;               // Tempo de fim
    bool IsPermanent;               // É permanente
    uint32_t AdminID;               // ID do administrador
    
    BanEntry() : BanID(0), AccountID(0), CharacterID(0),
        StartTime(0), EndTime(0), IsPermanent(false), AdminID(0) {}
};

/**
 * @brief Token de segurança
 */
struct SecurityToken {
    uint32_t TokenID;               // ID do token
    uint32_t EntityID;              // ID da entidade
    std::string Token;              // Token
    uint32_t CreationTime;          // Tempo de criação
    uint32_t ExpirationTime;        // Tempo de expiração
    bool Used;                      // Usado
    std::string Purpose;            // Propósito
    
    SecurityToken() : TokenID(0), EntityID(0),
        CreationTime(0), ExpirationTime(0), Used(false) {}
};

/**
 * @brief Rate limiter
 */
struct RateLimiter {
    uint32_t EntityID;              // ID da entidade
    std::string IP;                 // Endereço IP
    uint32_t PacketCount;           // Contagem de pacotes
    uint32_t LastResetTime;         // Tempo do último reset
    bool Limited;                   // Limitado
    uint32_t LimitEndTime;          // Tempo de fim do limite
    
    RateLimiter() : EntityID(0), PacketCount(0),
        LastResetTime(0), Limited(false), LimitEndTime(0) {}
};

/**
 * @brief Callback de violação de segurança
 */
using SecurityViolationCallback = std::function<void(const SecurityViolation&)>;

/**
 * @brief Gerenciador de segurança
 * 
 * Esta classe é responsável por gerenciar a segurança de rede do servidor.
 */
/**
 * Classe WYDSecurityManager
 * 
 * Esta classe implementa a funcionalidade WYDSecurityManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SecurityManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static SecurityManager& getInstance() {
        static SecurityManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração de segurança
     * @return true se carregado com sucesso
     */
    bool loadSecurityConfig();
    
    /**
     * @brief Carrega as regras de segurança
     * @return true se carregado com sucesso
     */
    bool loadSecurityRules();
    
    /**
     * @brief Carrega os bans
     * @return true se carregado com sucesso
     */
    bool loadBans();
    
    /**
     * @brief Adiciona uma regra
     * @param rule Regra
     * @return ID da regra ou 0 em caso de erro
     */
    uint32_t addRule(const SecurityRule& rule);
    
    /**
     * @brief Remove uma regra
     * @param ruleID ID da regra
     * @return true se removido com sucesso
     */
    bool removeRule(uint32_t ruleID);
    
    /**
     * @brief Habilita uma regra
     * @param ruleID ID da regra
     * @param enabled Habilitada
     * @return true se habilitado com sucesso
     */
    bool enableRule(uint32_t ruleID, bool enabled);
    
    /**
     * @brief Adiciona uma violação
     * @param entityID ID da entidade
     * @param type Tipo
     * @param details Detalhes
     * @param ip Endereço IP
     * @param accountID ID da conta
     * @param characterID ID do personagem
     * @return ID da violação ou 0 em caso de erro
     */
    uint32_t addViolation(uint32_t entityID, SecurityViolationType type, const std::string& details, const std::string& ip = "", uint32_t accountID = 0, uint32_t characterID = 0);
    
    /**
     * @brief Processa uma violação
     * @param violationID ID da violação
     * @return true se processado com sucesso
     */
    bool processViolation(uint32_t violationID);
    
    /**
     * @brief Verifica violações de segurança
     * @param entityID ID da entidade
     * @param type Tipo
     * @return true se houve violação
     */
    bool checkViolation(uint32_t entityID, SecurityViolationType type);
    
    /**
     * @brief Bane uma entidade
     * @param ip Endereço IP
     * @param accountID ID da conta
     * @param characterID ID do personagem
     * @param reason Motivo
     * @param duration Duração (0 para permanente)
     * @param adminID ID do administrador
     * @return ID do ban ou 0 em caso de erro
     */
    uint32_t banEntity(const std::string& ip, uint32_t accountID, uint32_t characterID, const std::string& reason, uint32_t duration = 0, uint32_t adminID = 0);
    
    /**
     * @brief Remove o ban de uma entidade
     * @param banID ID do ban
     * @return true se removido com sucesso
     */
    bool unbanEntity(uint32_t banID);
    
    /**
     * @brief Verifica se uma entidade está banida
     * @param ip Endereço IP
     * @param accountID ID da conta
     * @param characterID ID do personagem
     * @return true se estiver banida
     */
    bool isEntityBanned(const std::string& ip, uint32_t accountID = 0, uint32_t characterID = 0);
    
    /**
     * @brief Obtém os bans de uma entidade
     * @param ip Endereço IP
     * @param accountID ID da conta
     * @param characterID ID do personagem
     * @return Lista de IDs de ban
     */
    std::vector<uint32_t> getEntityBans(const std::string& ip, uint32_t accountID = 0, uint32_t characterID = 0);
    
    /**
     * @brief Obtém um ban
     * @param banID ID do ban
     * @return Ban ou nullptr se não encontrado
     */
    const BanEntry* getBan(uint32_t banID);
    
    /**
     * @brief Obtém todos os bans
     * @param activeOnly Apenas ativos
     * @return Lista de IDs de ban
     */
    std::vector<uint32_t> getAllBans(bool activeOnly = true);
    
    /**
     * @brief Gera um token de segurança
     * @param entityID ID da entidade
     * @param purpose Propósito
     * @param expirationTime Tempo de expiração (0 para padrão)
     * @return Token ou string vazia em caso de erro
     */
    std::string generateSecurityToken(uint32_t entityID, const std::string& purpose, uint32_t expirationTime = 0);
    
    /**
     * @brief Valida um token de segurança
     * @param entityID ID da entidade
     * @param token Token
     * @param purpose Propósito
     * @param markAsUsed Marcar como usado
     * @return true se for válido
     */
    bool validateSecurityToken(uint32_t entityID, const std::string& token, const std::string& purpose, bool markAsUsed = true);
    
    /**
     * @brief Verifica se um token de segurança foi usado
     * @param token Token
     * @return true se foi usado
     */
    bool isSecurityTokenUsed(const std::string& token);
    
    /**
     * @brief Limita a taxa de uma entidade
     * @param entityID ID da entidade
     * @param ip Endereço IP
     * @param duration Duração
     * @return true se limitado com sucesso
     */
    bool limitEntityRate(uint32_t entityID, const std::string& ip, uint32_t duration);
    
    /**
     * @brief Remove o limite de taxa de uma entidade
     * @param entityID ID da entidade
     * @return true se removido com sucesso
     */
    bool unlimitEntityRate(uint32_t entityID);
    
    /**
     * @brief Verifica se uma entidade está limitada
     * @param entityID ID da entidade
     * @return true se estiver limitada
     */
    bool isEntityRateLimited(uint32_t entityID);
    
    /**
     * @brief Atualiza o contador de pacotes de uma entidade
     * @param entityID ID da entidade
     * @param ip Endereço IP
     * @return true se não excedeu o limite
     */
    bool updatePacketCount(uint32_t entityID, const std::string& ip);
    
    /**
     * @brief Valida um pacote
     * @param connectionID ID da conexão
     * @param packetType Tipo do pacote
     * @param data Dados
     * @param size Tamanho
     * @param checksum Checksum
     * @param sequenceNumber Número de sequência
     * @param connectionState Estado da conexão
     * @return true se for válido
     */
    bool validatePacket(uint32_t connectionID, uint16_t packetType, const void* data, uint16_t size, uint32_t checksum, uint32_t sequenceNumber, uint32_t connectionState);
    
    /**
     * @brief Valida um checksum
     * @param data Dados
     * @param size Tamanho
     * @param checksum Checksum
     * @return true se for válido
     */
    bool validateChecksum(const void* data, uint16_t size, uint32_t checksum);
    
    /**
     * @brief Valida um número de sequência
     * @param connectionID ID da conexão
     * @param sequenceNumber Número de sequência
     * @return true se for válido
     */
    bool validateSequence(uint32_t connectionID, uint32_t sequenceNumber);
    
    /**
     * @brief Valida um estado de conexão
     * @param connectionID ID da conexão
     * @param packetType Tipo do pacote
     * @param connectionState Estado da conexão
     * @return true se for válido
     */
    bool validateState(uint32_t connectionID, uint16_t packetType, uint32_t connectionState);
    
    /**
     * @brief Registra um callback de violação de segurança
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerSecurityViolationCallback(SecurityViolationCallback callback);
    
    /**
     * @brief Remove um callback de violação de segurança
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterSecurityViolationCallback(uint32_t callbackId);
    
    /**
     * @brief Limpa violações antigas
     * @param maxAge Idade máxima
     * @return Número de violações limpas
     */
    uint32_t cleanupOldViolations(uint32_t maxAge = 0);
    
    /**
     * @brief Obtém estatísticas de segurança
     * @return Estatísticas
     */
    SecurityStats getSecurityStats() const;
    
    /**
     * @brief Obtém uma regra
     * @param ruleID ID da regra
     * @return Regra ou nullptr se não encontrada
     */
    const SecurityRule* getRule(uint32_t ruleID) const;
    
    /**
     * @brief Obtém todas as regras
     * @param enabledOnly Apenas habilitadas
     * @return Lista de IDs de regra
     */
    std::vector<uint32_t> getAllRules(bool enabledOnly = false) const;
    
    /**
     * @brief Obtém violações
     * @param entityID ID da entidade (0 para todas)
     * @param type Tipo (NONE para todas)
     * @param limit Limite (0 para todas)
     * @param offset Offset
     * @return Lista de IDs de violação
     */
    std::vector<uint32_t> getViolations(uint32_t entityID = 0, SecurityViolationType type = SecurityViolationType::NONE, uint32_t limit = 0, uint32_t offset = 0) const;
    
    /**
     * @brief Obtém uma violação
     * @param violationID ID da violação
     * @return Violação ou nullptr se não encontrada
     */
    const SecurityViolation* getViolation(uint32_t violationID) const;
    
    /**
     * @brief Obtém a configuração de segurança
     * @return Configuração
     */
    SecurityConfig getSecurityConfig() const;
    
    /**
     * @brief Define a configuração de segurança
     * @param config Configuração
     */
    void setSecurityConfig(const SecurityConfig& config);
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
private:
    /**
     * @brief Construtor privado
     */
    SecurityManager();
    
    /**
     * @brief Destrutor privado
     */
    ~SecurityManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    SecurityManager(const SecurityManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    SecurityManager& operator=(const SecurityManager&) = delete;
    
    // Configuração
    SecurityConfig _securityConfig;
    
    // Regras
    std::unordered_map<uint32_t, SecurityRule> _rules;
    std::atomic<uint32_t> _nextRuleID;
    
    // Violações
    std::unordered_map<uint32_t, SecurityViolation> _violations;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _entityViolations;
    std::atomic<uint32_t> _nextViolationID;
    
    // Bans
    std::unordered_map<uint32_t, BanEntry> _bans;
    std::unordered_map<std::string, std::vector<uint32_t>> _ipBans;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _accountBans;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _characterBans;
    std::atomic<uint32_t> _nextBanID;
    
    // Tokens
    std::unordered_map<uint32_t, SecurityToken> _tokens;
    std::unordered_map<std::string, uint32_t> _tokenToID;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _entityTokens;
    std::atomic<uint32_t> _nextTokenID;
    
    // Rate limiters
    std::unordered_map<uint32_t, RateLimiter> _rateLimiters;
    std::unordered_map<std::string, uint32_t> _ipToEntityID;
    
    // Sequências
    std::unordered_map<uint32_t, uint32_t> _connectionSequences;
    
    // Estatísticas
    SecurityStats _securityStats;
    
    // Callbacks
    std::vector<SecurityViolationCallback> _securityViolationCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _configMutex;
    mutable std::mutex _ruleMutex;
    mutable std::mutex _violationMutex;
    mutable std::mutex _banMutex;
    mutable std::mutex _tokenMutex;
    mutable std::mutex _rateLimiterMutex;
    mutable std::mutex _sequenceMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Timers
    uint32_t _banCheckTimer;
    uint32_t _tokenCheckTimer;
    uint32_t _rateLimiterCheckTimer;
    uint32_t _violationCleanupTimer;
    
    /**
     * @brief Checa bans expirados
     * @param currentTime Tempo atual
     */
    void checkExpiredBans(uint32_t currentTime);
    
    /**
     * @brief Checa tokens expirados
     * @param currentTime Tempo atual
     */
    void checkExpiredTokens(uint32_t currentTime);
    
    /**
     * @brief Checa rate limiters expirados
     * @param currentTime Tempo atual
     */
    void checkExpiredRateLimiters(uint32_t currentTime);
    
    /**
     * @brief Checa violações antigas
     * @param currentTime Tempo atual
     */
    void checkOldViolations(uint32_t currentTime);
    
    /**
     * @brief Aplica uma regra
     * @param violation Violação
     * @return true se aplicado com sucesso
     */
    bool applyRule(SecurityViolation& violation);
    
    /**
     * @brief Obtém uma regra por tipo
     * @param type Tipo
     * @return Regra ou nullptr se não encontrada
     */
    const SecurityRule* getRuleByType(SecurityViolationType type) const;
    
    /**
     * @brief Executa callbacks de violação de segurança
     * @param violation Violação
     */
    void executeSecurityViolationCallbacks(const SecurityViolation& violation);
    
    /**
     * @brief Atualiza estatísticas
     * @param violation Violação
     */
    void updateStats(const SecurityViolation& violation);
    
    /**
     * @brief Gera um token aleatório
     * @param length Comprimento
     * @return Token
     */
    std::string generateRandomToken(uint32_t length = 32);
    
    /**
     * @brief Obtém a ação para um tipo de violação
     * @param type Tipo
     * @param severity Severidade
     * @return Ação
     */
    SecurityAction getActionForViolation(SecurityViolationType type, SeverityLevel severity) const;
    
    /**
     * @brief Converte uma string para um tipo de violação
     * @param str String
     * @return Tipo
     */
    SecurityViolationType stringToViolationType(const std::string& str) const;
    
    /**
     * @brief Converte um tipo de violação para string
     * @param type Tipo
     * @return String
     */
    std::string violationTypeToString(SecurityViolationType type) const;
    
    /**
     * @brief Converte uma string para um nível de severidade
     * @param str String
     * @return Nível
     */
    SeverityLevel stringToSeverityLevel(const std::string& str) const;
    
    /**
     * @brief Converte um nível de severidade para string
     * @param level Nível
     * @return String
     */
    std::string severityLevelToString(SeverityLevel level) const;
    
    /**
     * @brief Converte uma string para uma ação de segurança
     * @param str String
     * @return Ação
     */
    SecurityAction stringToSecurityAction(const std::string& str) const;
    
    /**
     * @brief Converte uma ação de segurança para string
     * @param action Ação
     * @return String
     */
    std::string securityActionToString(SecurityAction action) const;
};

} // namespace network
} // namespace wydbr

#endif // _SECURITY_MANAGER_H_