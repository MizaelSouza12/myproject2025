/**
 * CrossServerEconomyManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/economy/crossserver/CrossServerEconomyManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef CROSSSERVERECONOMYMANAGER_H
#define CROSSSERVERECONOMYMANAGER_H

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
#include <condition_variable>

#include "../../../../core/WYDTypes.h"
#include "../EconomyTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file CrossServerEconomyManager.h
 * @brief Gerenciador de economia entre servidores
 * 
 * Este arquivo contém a definição do gerenciador de economia entre servidores,
 * responsável por sincronizar dados econômicos, inventários e transações 
 * entre múltiplos servidores do jogo.
 */

namespace wyd {
namespace server {
namespace economy {
namespace crossserver {

/**
 * @brief Tipo de transferência entre servidores
 */
enum class CrossServerTransferType : BYTE {
    PLAYER_MOVE = 0,           // Movimentação de jogador
    ITEM_TRANSFER = 1,         // Transferência de item
    CURRENCY_TRANSFER = 2,     // Transferência de moeda
    MARKET_TRANSACTION = 3,    // Transação de mercado
    MAIL_ITEM = 4,             // Item por correspondência
    GUILD_RESOURCE = 5,        // Recurso de guild
    AUCTION_BID = 6,           // Lance em leilão
    TRADE_COMPLETION = 7,      // Conclusão de troca
    ADMIN_ACTION = 8,          // Ação administrativa
    CUSTOM = 9,                // Personalizado
};

/**
 * @brief Status de transferência entre servidores
 */
enum class CrossServerTransferStatus : BYTE {
    PENDING = 0,               // Pendente
    COMPLETED = 1,             // Completada
    FAILED = 2,                // Falhou
    CANCELLED = 3,             // Cancelada
    PARTIAL = 4,               // Parcial
    PROCESSING = 5,            // Processando
    QUEUED = 6,                // Na fila
    ROLLBACK = 7,              // Reversão
    CUSTOM = 8,                // Personalizado
};

/**
 * @brief Tipo de sincronização de servidor
 */
enum class ServerSyncType : BYTE {
    FULL_SYNC = 0,             // Sincronização completa
    DIFFERENTIAL_SYNC = 1,     // Sincronização diferencial
    PLAYER_ONLY_SYNC = 2,      // Sincronização apenas de jogador
    MARKET_ONLY_SYNC = 3,      // Sincronização apenas de mercado
    CURRENCY_ONLY_SYNC = 4,    // Sincronização apenas de moeda
    ITEM_ONLY_SYNC = 5,        // Sincronização apenas de item
    INCREMENTAL_SYNC = 6,      // Sincronização incremental
    CUSTOM_SYNC = 7,           // Sincronização personalizada
};

/**
 * @brief Nível de prioridade de sincronização
 */
enum class SyncPriority : BYTE {
    LOW = 0,                   // Baixa
    MEDIUM = 1,                // Média
    HIGH = 2,                  // Alta
    CRITICAL = 3,              // Crítica
    EMERGENCY = 4,             // Emergência
    SCHEDULED = 5,             // Agendada
    CUSTOM = 6,                // Personalizada
};

/**
 * @brief Tipo de regra de conversão entre servidores
 */
enum class ConversionRuleType : BYTE {
    DIRECT = 0,                // Direta
    FIXED_RATE = 1,            // Taxa fixa
    DYNAMIC_RATE = 2,          // Taxa dinâmica
    TIERED = 3,                // Em níveis
    FORMULA_BASED = 4,         // Baseada em fórmula
    CAPPED = 5,                // Limitada
    TAXED = 6,                 // Taxada
    RESTRICTED = 7,            // Restrita
    CUSTOM = 8,                // Personalizada
};

/**
 * @brief Transferência entre servidores
 */
struct CrossServerTransfer {
    DWORD transferID;                // ID da transferência
    CrossServerTransferType type;    // Tipo
    DWORD sourceServerID;            // ID do servidor de origem
    DWORD targetServerID;            // ID do servidor de destino
    DWORD playerID;                  // ID do jogador
    std::string playerName;           // Nome do jogador
    WORD itemID;                     // ID do item
    BYTE itemCount;                  // Contagem de itens
    BYTE itemQuality;                // Qualidade do item
    DWORD currencyAmount;            // Valor de moeda
    CurrencyType currencyType;       // Tipo de moeda
    time_t requestTime;               // Hora da solicitação
    time_t completionTime;            // Hora de conclusão
    CrossServerTransferStatus status; // Status
    std::string errorMessage;         // Mensagem de erro
    std::string additionalData;       // Dados adicionais (JSON)
    
    CrossServerTransfer()
        : transferID(0)
        , type(CrossServerTransferType::PLAYER_MOVE)
        , sourceServerID(0)
        , targetServerID(0)
        , playerID(0)
        , playerName("")
        , itemID(0)
        , itemCount(0)
        , itemQuality(0)
        , currencyAmount(0)
        , currencyType(CurrencyType::GOLD)
        , requestTime(0)
        , completionTime(0)
        , status(CrossServerTransferStatus::PENDING)
        , errorMessage("")
        , additionalData("")
    {
    }
};

/**
 * @brief Sincronização de servidor
 */
struct ServerSync {
    DWORD syncID;                    // ID da sincronização
    ServerSyncType type;             // Tipo
    DWORD serverID;                  // ID do servidor
    time_t startTime;                 // Hora de início
    time_t endTime;                   // Hora de término
    SyncPriority priority;           // Prioridade
    CrossServerSyncStatus status;    // Status
    DWORD entitiesSynced;            // Entidades sincronizadas
    DWORD conflictsDetected;         // Conflitos detectados
    DWORD conflictsResolved;         // Conflitos resolvidos
    std::string errorMessage;         // Mensagem de erro
    
    ServerSync()
        : syncID(0)
        , type(ServerSyncType::FULL_SYNC)
        , serverID(0)
        , startTime(0)
        , endTime(0)
        , priority(SyncPriority::MEDIUM)
        , status(CrossServerSyncStatus::PENDING)
        , entitiesSynced(0)
        , conflictsDetected(0)
        , conflictsResolved(0)
        , errorMessage("")
    {
    }
};

/**
 * @brief Regra de conversão entre servidores
 */
struct ConversionRule {
    DWORD ruleID;                    // ID da regra
    ConversionRuleType type;         // Tipo
    DWORD sourceServerID;            // ID do servidor de origem
    DWORD targetServerID;            // ID do servidor de destino
    CurrencyType currencyType;       // Tipo de moeda
    float conversionRate;             // Taxa de conversão
    DWORD maxTransferAmount;         // Valor máximo de transferência
    DWORD minTransferAmount;         // Valor mínimo de transferência
    float taxRate;                    // Taxa de imposto
    std::string formulaExpression;    // Expressão de fórmula
    bool enabled;                     // Habilitada
    std::string restrictions;         // Restrições (JSON)
    
    ConversionRule()
        : ruleID(0)
        , type(ConversionRuleType::DIRECT)
        , sourceServerID(0)
        , targetServerID(0)
        , currencyType(CurrencyType::GOLD)
        , conversionRate(1.0f)
        , maxTransferAmount(0)
        , minTransferAmount(0)
        , taxRate(0.0f)
        , formulaExpression("")
        , enabled(true)
        , restrictions("")
    {
    }
};

/**
 * @brief Resolução de conflito
 */
struct ConflictResolution {
    DWORD resolutionID;              // ID da resolução
    DWORD conflictID;                // ID do conflito
    DWORD playerID;                  // ID do jogador
    std::string playerName;           // Nome do jogador
    DWORD adminID;                   // ID do administrador
    std::string resolution;           // Resolução
    time_t resolutionTime;            // Hora da resolução
    bool successful;                  // Bem-sucedida
    std::string notes;                // Notas
    
    ConflictResolution()
        : resolutionID(0)
        , conflictID(0)
        , playerID(0)
        , playerName("")
        , adminID(0)
        , resolution("")
        , resolutionTime(0)
        , successful(false)
        , notes("")
    {
    }
};

/**
 * @brief Status econômico de servidor
 */
struct ServerEconomicStatus {
    DWORD serverID;                  // ID do servidor
    std::string serverName;           // Nome do servidor
    DWORD totalPlayers;              // Total de jogadores
    DWORD activePlayers;             // Jogadores ativos
    DWORD totalGold;                 // Total de ouro
    DWORD averageGoldPerPlayer;      // Média de ouro por jogador
    DWORD totalTransactions;         // Total de transações
    float inflationRate;              // Taxa de inflação
    float marketActivity;             // Atividade de mercado
    time_t lastUpdateTime;            // Hora da última atualização
    
    ServerEconomicStatus()
        : serverID(0)
        , serverName("")
        , totalPlayers(0)
        , activePlayers(0)
        , totalGold(0)
        , averageGoldPerPlayer(0)
        , totalTransactions(0)
        , inflationRate(0.0f)
        , marketActivity(0.0f)
        , lastUpdateTime(0)
    {
    }
};

/**
 * @brief Status de item entre servidores
 */
struct CrossServerItemStatus {
    DWORD statusID;                  // ID do status
    WORD itemID;                     // ID do item
    BYTE quality;                    // Qualidade
    float averageValueServer1;        // Valor médio no servidor 1
    float averageValueServer2;        // Valor médio no servidor 2
    float valueDifference;            // Diferença de valor
    bool arbitrageOpportunity;        // Oportunidade de arbitragem
    float arbitrageProfit;            // Lucro de arbitragem
    time_t lastUpdateTime;            // Hora da última atualização
    
    CrossServerItemStatus()
        : statusID(0)
        , itemID(0)
        , quality(0)
        , averageValueServer1(0.0f)
        , averageValueServer2(0.0f)
        , valueDifference(0.0f)
        , arbitrageOpportunity(false)
        , arbitrageProfit(0.0f)
        , lastUpdateTime(0)
    {
    }
};

/**
 * @brief Configuração de servidor em cluster
 */
struct ClusterServerConfig {
    DWORD configID;                  // ID da configuração
    DWORD serverID;                  // ID do servidor
    std::string serverName;           // Nome do servidor
    std::string connectionString;     // String de conexão
    bool primaryServer;               // Servidor primário
    bool economySyncEnabled;          // Sincronização de economia habilitada
    bool marketSyncEnabled;           // Sincronização de mercado habilitada
    bool playerSyncEnabled;           // Sincronização de jogador habilitada
    DWORD syncInterval;              // Intervalo de sincronização (minutos)
    TradingRestrictionLevel restrictions; // Restrições
    
    ClusterServerConfig()
        : configID(0)
        , serverID(0)
        , serverName("")
        , connectionString("")
        , primaryServer(false)
        , economySyncEnabled(true)
        , marketSyncEnabled(true)
        , playerSyncEnabled(true)
        , syncInterval(60)
        , restrictions(TradingRestrictionLevel::NONE)
    {
    }
};

/**
 * @brief Conflito de sincronização
 */
struct SyncConflict {
    DWORD conflictID;                // ID do conflito
    DWORD syncID;                    // ID da sincronização
    DWORD playerID;                  // ID do jogador
    std::string playerName;           // Nome do jogador
    std::string conflictType;         // Tipo de conflito
    std::string sourceData;           // Dados de origem
    std::string targetData;           // Dados de destino
    time_t detectionTime;             // Hora de detecção
    bool resolved;                    // Resolvido
    DWORD resolutionID;              // ID da resolução
    
    SyncConflict()
        : conflictID(0)
        , syncID(0)
        , playerID(0)
        , playerName("")
        , conflictType("")
        , sourceData("")
        , targetData("")
        , detectionTime(0)
        , resolved(false)
        , resolutionID(0)
    {
    }
};

/**
 * @brief Callback para transferência entre servidores
 */
using CrossServerTransferCallback = std::function<void(const CrossServerTransfer& transfer)>;

/**
 * @brief Callback para sincronização de servidor
 */
using ServerSyncCallback = std::function<void(const ServerSync& sync)>;

/**
 * @brief Gerenciador de economia entre servidores
 */
/**
 * Classe WYDCrossServerEconomyManager
 * 
 * Esta classe implementa a funcionalidade WYDCrossServerEconomyManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CrossServerEconomyManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static CrossServerEconomyManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Carrega configurações de cluster
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadClusterConfigurations(const std::string& filePath);
    
    /**
     * @brief Salva configurações de cluster
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveClusterConfigurations(const std::string& filePath);
    
    /**
     * @brief Carrega regras de conversão
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadConversionRules(const std::string& filePath);
    
    /**
     * @brief Salva regras de conversão
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveConversionRules(const std::string& filePath);
    
    /**
     * @brief Carrega transferências pendentes
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadPendingTransfers(const std::string& filePath);
    
    /**
     * @brief Salva transferências pendentes
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SavePendingTransfers(const std::string& filePath);
    
    /**
     * @brief Adiciona um servidor ao cluster
     * @param serverID ID do servidor
     * @param serverName Nome do servidor
     * @param connectionString String de conexão
     * @param primaryServer Se é servidor primário
     * @param economySyncEnabled Se a sincronização de economia está habilitada
     * @param marketSyncEnabled Se a sincronização de mercado está habilitada
     * @param playerSyncEnabled Se a sincronização de jogador está habilitada
     * @param syncInterval Intervalo de sincronização (minutos)
     * @param restrictions Restrições
     * @param errorReason Razão do erro (saída)
     * @return ID da configuração, ou 0 se falhar
     */
    DWORD AddServerToCluster(DWORD serverID, const std::string& serverName, 
                           const std::string& connectionString, bool primaryServer,
                           bool economySyncEnabled, bool marketSyncEnabled, 
                           bool playerSyncEnabled, DWORD syncInterval,
                           TradingRestrictionLevel restrictions, std::string& errorReason);
    
    /**
     * @brief Remove um servidor do cluster
     * @param serverID ID do servidor
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveServerFromCluster(DWORD serverID, std::string& errorReason);
    
    /**
     * @brief Adiciona uma regra de conversão
     * @param type Tipo
     * @param sourceServerID ID do servidor de origem
     * @param targetServerID ID do servidor de destino
     * @param currencyType Tipo de moeda
     * @param conversionRate Taxa de conversão
     * @param maxTransferAmount Valor máximo de transferência
     * @param minTransferAmount Valor mínimo de transferência
     * @param taxRate Taxa de imposto
     * @param formulaExpression Expressão de fórmula
     * @param restrictions Restrições (JSON)
     * @param errorReason Razão do erro (saída)
     * @return ID da regra, ou 0 se falhar
     */
    DWORD AddConversionRule(ConversionRuleType type, DWORD sourceServerID, DWORD targetServerID,
                          CurrencyType currencyType, float conversionRate, DWORD maxTransferAmount,
                          DWORD minTransferAmount, float taxRate, const std::string& formulaExpression,
                          const std::string& restrictions, std::string& errorReason);
    
    /**
     * @brief Remove uma regra de conversão
     * @param ruleID ID da regra
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveConversionRule(DWORD ruleID, std::string& errorReason);
    
    /**
     * @brief Inicia uma transferência entre servidores
     * @param type Tipo
     * @param sourceServerID ID do servidor de origem
     * @param targetServerID ID do servidor de destino
     * @param playerID ID do jogador
     * @param playerName Nome do jogador
     * @param itemID ID do item
     * @param itemCount Contagem de itens
     * @param itemQuality Qualidade do item
     * @param currencyAmount Valor de moeda
     * @param currencyType Tipo de moeda
     * @param additionalData Dados adicionais (JSON)
     * @param errorReason Razão do erro (saída)
     * @return ID da transferência, ou 0 se falhar
     */
    DWORD InitiateTransfer(CrossServerTransferType type, DWORD sourceServerID, DWORD targetServerID,
                         DWORD playerID, const std::string& playerName, WORD itemID, BYTE itemCount,
                         BYTE itemQuality, DWORD currencyAmount, CurrencyType currencyType,
                         const std::string& additionalData, std::string& errorReason);
    
    /**
     * @brief Cancela uma transferência
     * @param transferID ID da transferência
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelTransfer(DWORD transferID, std::string& errorReason);
    
    /**
     * @brief Finaliza uma transferência
     * @param transferID ID da transferência
     * @param success Se foi bem-sucedida
     * @param errorMessage Mensagem de erro
     * @return true se finalizado com sucesso
     */
    bool FinalizeTransfer(DWORD transferID, bool success, const std::string& errorMessage = "");
    
    /**
     * @brief Obtém uma transferência
     * @param transferID ID da transferência
     * @return Ponteiro para a transferência, ou nullptr se não existir
     */
    const CrossServerTransfer* GetTransfer(DWORD transferID) const;
    
    /**
     * @brief Inicia uma sincronização de servidor
     * @param type Tipo
     * @param serverID ID do servidor
     * @param priority Prioridade
     * @param errorReason Razão do erro (saída)
     * @return ID da sincronização, ou 0 se falhar
     */
    DWORD InitiateServerSync(ServerSyncType type, DWORD serverID, SyncPriority priority, std::string& errorReason);
    
    /**
     * @brief Cancela uma sincronização
     * @param syncID ID da sincronização
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelServerSync(DWORD syncID, std::string& errorReason);
    
    /**
     * @brief Finaliza uma sincronização
     * @param syncID ID da sincronização
     * @param success Se foi bem-sucedida
     * @param entitiesSynced Entidades sincronizadas
     * @param conflictsDetected Conflitos detectados
     * @param conflictsResolved Conflitos resolvidos
     * @param errorMessage Mensagem de erro
     * @return true se finalizado com sucesso
     */
    bool FinalizeServerSync(DWORD syncID, bool success, DWORD entitiesSynced, DWORD conflictsDetected,
                          DWORD conflictsResolved, const std::string& errorMessage = "");
    
    /**
     * @brief Obtém uma sincronização
     * @param syncID ID da sincronização
     * @return Ponteiro para a sincronização, ou nullptr se não existir
     */
    const ServerSync* GetServerSync(DWORD syncID) const;
    
    /**
     * @brief Registra um conflito de sincronização
     * @param syncID ID da sincronização
     * @param playerID ID do jogador
     * @param playerName Nome do jogador
     * @param conflictType Tipo de conflito
     * @param sourceData Dados de origem
     * @param targetData Dados de destino
     * @return ID do conflito, ou 0 se falhar
     */
    DWORD RegisterSyncConflict(DWORD syncID, DWORD playerID, const std::string& playerName,
                             const std::string& conflictType, const std::string& sourceData,
                             const std::string& targetData);
    
    /**
     * @brief Resolve um conflito
     * @param conflictID ID do conflito
     * @param adminID ID do administrador
     * @param resolution Resolução
     * @param notes Notas
     * @param errorReason Razão do erro (saída)
     * @return ID da resolução, ou 0 se falhar
     */
    DWORD ResolveConflict(DWORD conflictID, DWORD adminID, const std::string& resolution,
                        const std::string& notes, std::string& errorReason);
    
    /**
     * @brief Obtém um conflito
     * @param conflictID ID do conflito
     * @return Ponteiro para o conflito, ou nullptr se não existir
     */
    const SyncConflict* GetConflict(DWORD conflictID) const;
    
    /**
     * @brief Atualiza o status econômico de um servidor
     * @param serverID ID do servidor
     * @param totalPlayers Total de jogadores
     * @param activePlayers Jogadores ativos
     * @param totalGold Total de ouro
     * @param averageGoldPerPlayer Média de ouro por jogador
     * @param totalTransactions Total de transações
     * @param inflationRate Taxa de inflação
     * @param marketActivity Atividade de mercado
     * @return true se atualizado com sucesso
     */
    bool UpdateServerEconomicStatus(DWORD serverID, DWORD totalPlayers, DWORD activePlayers,
                                 DWORD totalGold, DWORD averageGoldPerPlayer, DWORD totalTransactions,
                                 float inflationRate, float marketActivity);
    
    /**
     * @brief Obtém o status econômico de um servidor
     * @param serverID ID do servidor
     * @return Ponteiro para o status, ou nullptr se não existir
     */
    const ServerEconomicStatus* GetServerEconomicStatus(DWORD serverID) const;
    
    /**
     * @brief Atualiza o status de um item entre servidores
     * @param itemID ID do item
     * @param quality Qualidade
     * @param averageValueServer1 Valor médio no servidor 1
     * @param averageValueServer2 Valor médio no servidor 2
     * @return true se atualizado com sucesso
     */
    bool UpdateCrossServerItemStatus(WORD itemID, BYTE quality, float averageValueServer1, float averageValueServer2);
    
    /**
     * @brief Obtém o status de um item entre servidores
     * @param itemID ID do item
     * @param quality Qualidade
     * @return Ponteiro para o status, ou nullptr se não existir
     */
    const CrossServerItemStatus* GetCrossServerItemStatus(WORD itemID, BYTE quality) const;
    
    /**
     * @brief Obtém itens com oportunidade de arbitragem
     * @return Lista de status de itens
     */
    std::vector<const CrossServerItemStatus*> GetArbitrageOpportunities() const;
    
    /**
     * @brief Verifica transferências pendentes para um jogador
     * @param playerID ID do jogador
     * @return Lista de transferências
     */
    std::vector<const CrossServerTransfer*> GetPendingTransfersForPlayer(DWORD playerID) const;
    
    /**
     * @brief Converte moeda entre servidores
     * @param sourceServerID ID do servidor de origem
     * @param targetServerID ID do servidor de destino
     * @param currencyType Tipo de moeda
     * @param amount Valor
     * @return Valor convertido
     */
    DWORD ConvertCurrencyBetweenServers(DWORD sourceServerID, DWORD targetServerID, CurrencyType currencyType, DWORD amount);
    
    /**
     * @brief Calcula taxa de imposto para uma transferência
     * @param sourceServerID ID do servidor de origem
     * @param targetServerID ID do servidor de destino
     * @param currencyType Tipo de moeda
     * @param amount Valor
     * @return Valor do imposto
     */
    DWORD CalculateTransferTax(DWORD sourceServerID, DWORD targetServerID, CurrencyType currencyType, DWORD amount);
    
    /**
     * @brief Verifica restrições de transferência para um jogador
     * @param playerID ID do jogador
     * @param sourceServerID ID do servidor de origem
     * @param targetServerID ID do servidor de destino
     * @param errorReason Razão do erro (saída)
     * @return true se não houver restrições
     */
    bool CheckTransferRestrictions(DWORD playerID, DWORD sourceServerID, DWORD targetServerID, std::string& errorReason);
    
    /**
     * @brief Obtém todas as transferências
     * @param count Número de transferências
     * @param offset Deslocamento
     * @return Lista de transferências
     */
    std::vector<const CrossServerTransfer*> GetAllTransfers(size_t count, size_t offset = 0) const;
    
    /**
     * @brief Obtém transferências por status
     * @param status Status
     * @param count Número de transferências
     * @param offset Deslocamento
     * @return Lista de transferências
     */
    std::vector<const CrossServerTransfer*> GetTransfersByStatus(CrossServerTransferStatus status, size_t count, size_t offset = 0) const;
    
    /**
     * @brief Obtém todas as sincronizações
     * @param count Número de sincronizações
     * @param offset Deslocamento
     * @return Lista de sincronizações
     */
    std::vector<const ServerSync*> GetAllServerSyncs(size_t count, size_t offset = 0) const;
    
    /**
     * @brief Obtém todas as regras de conversão
     * @return Lista de regras
     */
    std::vector<const ConversionRule*> GetAllConversionRules() const;
    
    /**
     * @brief Obtém regras de conversão por servidor
     * @param serverID ID do servidor
     * @return Lista de regras
     */
    std::vector<const ConversionRule*> GetConversionRulesByServer(DWORD serverID) const;
    
    /**
     * @brief Registra um callback para transferência
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterTransferCallback(CrossServerTransferCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterTransferCallback(int callbackID);
    
    /**
     * @brief Registra um callback para sincronização
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterSyncCallback(ServerSyncCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterSyncCallback(int callbackID);
    
    /**
     * @brief Habilita ou desabilita uma regra de conversão
     * @param ruleID ID da regra
     * @param enabled Se está habilitada
     * @param errorReason Razão do erro (saída)
     * @return true se atualizado com sucesso
     */
    bool SetConversionRuleEnabled(DWORD ruleID, bool enabled, std::string& errorReason);
    
    /**
     * @brief Atualiza uma taxa de conversão
     * @param ruleID ID da regra
     * @param newRate Nova taxa
     * @param errorReason Razão do erro (saída)
     * @return true se atualizado com sucesso
     */
    bool UpdateConversionRate(DWORD ruleID, float newRate, std::string& errorReason);
    
    /**
     * @brief Verifica o status de conexão com um servidor
     * @param serverID ID do servidor
     * @return true se conectado
     */
    bool CheckServerConnection(DWORD serverID);
    
    /**
     * @brief Obtém estatísticas de economia entre servidores
     * @return String formatada com estatísticas
     */
    std::string GetCrossServerEconomyStats();
    
    /**
     * @brief Verifica se um servidor existe
     * @param serverID ID do servidor
     * @return true se existir
     */
    bool ServerExists(DWORD serverID) const;
    
    /**
     * @brief Obtém o ID do servidor local
     * @return ID do servidor
     */
    DWORD GetLocalServerID() const;
    
    /**
     * @brief Define o ID do servidor local
     * @param serverID ID do servidor
     */
    void SetLocalServerID(DWORD serverID);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CrossServerEconomyManager();
    
    /**
     * @brief Destrutor
     */
    ~CrossServerEconomyManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CrossServerEconomyManager(const CrossServerEconomyManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CrossServerEconomyManager& operator=(const CrossServerEconomyManager&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Processa transferências pendentes
     */
    void ProcessPendingTransfers();
    
    /**
     * @brief Processa sincronizações agendadas
     */
    void ProcessScheduledSyncs();
    
    /**
     * @brief Notifica callbacks de transferência
     * @param transfer Transferência
     */
    void NotifyTransferCallbacks(const CrossServerTransfer& transfer);
    
    /**
     * @brief Notifica callbacks de sincronização
     * @param sync Sincronização
     */
    void NotifySyncCallbacks(const ServerSync& sync);
    
    /**
     * @brief Aplica regra de conversão
     * @param rule Regra
     * @param amount Valor
     * @return Valor convertido
     */
    DWORD ApplyConversionRule(const ConversionRule& rule, DWORD amount);
    
    /**
     * @brief Verifica status de oportunidades de arbitragem
     */
    void CheckArbitrageOpportunities();
    
    /**
     * @brief Verifica balanceamento econômico entre servidores
     */
    void CheckEconomicBalance();
    
    /**
     * @brief Recupera um jogador após falha
     * @param playerID ID do jogador
     * @param transferID ID da transferência
     * @return true se recuperado com sucesso
     */
    bool RecoverPlayerAfterFailure(DWORD playerID, DWORD transferID);
    
    /**
     * @brief Executa uma sincronização
     * @param syncID ID da sincronização
     * @return true se executado com sucesso
     */
    bool ExecuteServerSync(DWORD syncID);
    
    /**
     * @brief Executa uma transferência
     * @param transferID ID da transferência
     * @return true se executado com sucesso
     */
    bool ExecuteTransfer(DWORD transferID);
    
    /**
     * @brief Valida uma regra de conversão
     * @param rule Regra
     * @param errorReason Razão do erro (saída)
     * @return true se válida
     */
    bool ValidateConversionRule(const ConversionRule& rule, std::string& errorReason);
    
    // Configurações de cluster
    std::map<DWORD, ClusterServerConfig> serverConfigs_;
    mutable std::mutex serverConfigsMutex_;
    
    // Regras de conversão
    std::map<DWORD, ConversionRule> conversionRules_;
    mutable std::mutex conversionRulesMutex_;
    
    // Transferências
    std::map<DWORD, CrossServerTransfer> transfers_;
    mutable std::mutex transfersMutex_;
    
    // Sincronizações
    std::map<DWORD, ServerSync> syncs_;
    mutable std::mutex syncsMutex_;
    
    // Conflitos
    std::map<DWORD, SyncConflict> conflicts_;
    mutable std::mutex conflictsMutex_;
    
    // Resoluções
    std::map<DWORD, ConflictResolution> resolutions_;
    mutable std::mutex resolutionsMutex_;
    
    // Status econômico
    std::map<DWORD, ServerEconomicStatus> serverStatus_;
    mutable std::mutex serverStatusMutex_;
    
    // Status de item
    std::map<std::pair<WORD, BYTE>, CrossServerItemStatus> itemStatus_;
    mutable std::mutex itemStatusMutex_;
    
    // Índices para busca rápida
    std::map<DWORD, std::vector<DWORD>> transfersByPlayer_;
    std::map<CrossServerTransferStatus, std::vector<DWORD>> transfersByStatus_;
    std::map<DWORD, std::vector<DWORD>> syncsByServer_;
    std::map<DWORD, std::vector<DWORD>> rulesBySourceServer_;
    std::map<DWORD, std::vector<DWORD>> rulesByTargetServer_;
    std::map<DWORD, std::vector<DWORD>> conflictsBySync_;
    std::vector<DWORD> arbitrageOpportunities_;
    
    // Callbacks
    std::map<int, CrossServerTransferCallback> transferCallbacks_;
    std::mutex transferCallbacksMutex_;
    int nextTransferCallbackID_;
    
    std::map<int, ServerSyncCallback> syncCallbacks_;
    std::mutex syncCallbacksMutex_;
    int nextSyncCallbackID_;
    
    // Contadores para novos IDs
    std::atomic<DWORD> nextTransferID_;
    std::atomic<DWORD> nextSyncID_;
    std::atomic<DWORD> nextRuleID_;
    std::atomic<DWORD> nextConfigID_;
    std::atomic<DWORD> nextConflictID_;
    std::atomic<DWORD> nextResolutionID_;
    std::atomic<DWORD> nextStatusID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Queues
    std::queue<DWORD> pendingTransfers_;
    std::mutex pendingTransfersMutex_;
    
    std::queue<DWORD> scheduledSyncs_;
    std::mutex scheduledSyncsMutex_;
    
    // Servidor local
    DWORD localServerID_;
    
    // Configurações
    DWORD maxTransfersPerBatch_;
    DWORD transferProcessInterval_;
    DWORD syncProcessInterval_;
    DWORD connectionTimeout_;
    bool autoReconnect_;
    bool autoRecovery_;
    
    // Tempos
    time_t lastTransferProcessTime_;
    time_t lastSyncProcessTime_;
    time_t lastArbitrageCheckTime_;
    time_t lastBalanceCheckTime_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_CrossServerEconomyManager wyd::server::economy::crossserver::CrossServerEconomyManager::GetInstance()

} // namespace crossserver
} // namespace economy
} // namespace server
} // namespace wyd

#endif // CROSSSERVERECONOMYMANAGER_H

} // namespace wydbr
