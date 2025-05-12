/**
 * @file database_system.h
 * @brief Sistema de banco de dados com MVCC para o WYDBR
 * 
 * Este arquivo define as interfaces e estruturas do sistema de banco de dados
 * do WYDBR, implementando controle de concorrência multiversão (MVCC) com
 * suporte a transações, isolation levels e sharding.
 * 
 * @author WYDBR Team
 * @version 1.0.0
 * @date 2025-05-06
 */

#ifndef WYDBR_DATABASE_SYSTEM_H
#define WYDBR_DATABASE_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <variant>

namespace wydbr {
namespace database {

/**
 * @enum TransactionState
 * @brief Estados possíveis para uma transação
 */
enum /**
 * Classe WYDTransactionState
 * 
 * Esta classe implementa a funcionalidade WYDTransactionState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TransactionState {
    INITIALIZING,    ///< Transação criada mas não iniciada
    ACTIVE,          ///< Transação em andamento
    COMMITTING,      ///< Em processo de confirmação
    COMMITTED,       ///< Confirmada com sucesso
    ROLLING_BACK,    ///< Em processo de desfazimento
    ROLLED_BACK,     ///< Desfeita com sucesso
    ABORTED,         ///< Abortada por erro
    TIMED_OUT        ///< Expirada por timeout
};

/**
 * @enum OperationType
 * @brief Tipos de operação em transações
 */
enum /**
 * Classe WYDOperationType
 * 
 * Esta classe implementa a funcionalidade WYDOperationType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class OperationType {
    READ,            ///< Leitura
    CREATE,          ///< Criação
    UPDATE,          ///< Atualização
    DELETE           ///< Remoção
};

/**
 * @enum IsolationLevel
 * @brief Níveis de isolamento para transações
 */
enum /**
 * Classe WYDIsolationLevel
 * 
 * Esta classe implementa a funcionalidade WYDIsolationLevel conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class IsolationLevel {
    READ_UNCOMMITTED,    ///< Permite leituras de dados não confirmados
    READ_COMMITTED,      ///< Permite leituras apenas de dados confirmados
    REPEATABLE_READ,     ///< Garante que leituras repetidas serão iguais
    SERIALIZABLE         ///< Isolamento completo de outras transações
};

/**
 * @enum StorageType
 * @brief Tipos de armazenamento de dados
 */
enum /**
 * Classe WYDStorageType
 * 
 * Esta classe implementa a funcionalidade WYDStorageType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class StorageType {
    MEMORY,          ///< Armazenamento em memória
    DISK,            ///< Armazenamento em disco
    NETWORK,         ///< Armazenamento em rede
    HYBRID           ///< Combinação de tipos
};

/**
 * @enum ShardingStrategy
 * @brief Estratégias de sharding
 */
enum /**
 * Classe WYDShardingStrategy
 * 
 * Esta classe implementa a funcionalidade WYDShardingStrategy conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ShardingStrategy {
    NONE,                ///< Sem sharding
    HASH,                ///< Baseado em hash da chave
    RANGE,               ///< Baseado em intervalo de valores
    DIRECTORY,           ///< Baseado em tabela de diretório
    GEOLOCATION,         ///< Baseado em localização geográfica
    CUSTOM               ///< Estratégia personalizada
};

/**
 * @typedef TransactionId
 * @brief Tipo para identificador de transação
 */
using TransactionId = std::string;

/**
 * @typedef ResourceKey
 * @brief Tipo para chave de recurso
 */
using ResourceKey = std::string;

/**
 * @typedef ResourceType
 * @brief Tipo para tipo de recurso
 */
using ResourceType = std::string;

/**
 * @typedef Timestamp
 * @brief Tipo para timestamp
 */
using Timestamp = uint64_t;

/**
 * @typedef Version
 * @brief Tipo para versão
 */
using Version = uint64_t;

/**
 * @typedef UserId
 * @brief Tipo para identificador de usuário
 */
using UserId = std::string;

/**
 * @typedef Value
 * @brief Tipo para valor armazenado
 */
using Value = std::variant<std::nullptr_t, bool, int64_t, double, std::string, std::vector<uint8_t>>;

/**
 * @struct OperationLog
 * @brief Registro de operação em uma transação
 */
struct OperationLog {
    std::string id;                  ///< ID único da operação
    OperationType type;              ///< Tipo de operação
    ResourceKey resource;            ///< Recurso afetado
    ResourceType resourceType;       ///< Tipo de recurso
    std::optional<std::string> key;  ///< Chave afetada (opcional)
    Value value;                     ///< Valor novo
    std::optional<Value> previousValue; ///< Valor anterior (opcional)
    Timestamp timestamp;             ///< Timestamp da operação
    std::unordered_map<std::string, std::string> metadata; ///< Metadados
    std::vector<std::string> dependsOn; ///< Dependências da operação
    
    /**
     * @brief Cria a operação inversa para rollback
     * @return Operação inversa ou nullopt se não for necessária
     */
    std::optional<OperationLog> createInverse() const;
};

/**
 * @class Transaction
 * @brief Representa uma transação no sistema
 */
/**
 * Classe WYDTransaction
 * 
 * Esta classe implementa a funcionalidade WYDTransaction conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Transaction {
public:
    /**
     * @brief Construtor
     * @param id ID da transação
     * @param isolationLevel Nível de isolamento
     * @param timeout Timeout em milissegundos
     */
    Transaction(TransactionId id, IsolationLevel isolationLevel = IsolationLevel::READ_COMMITTED, uint32_t timeout = 30000);
    
    /**
     * @brief Destrutor
     */
    ~Transaction() = default;
    
    /**
     * @brief Inicia a transação
     */
    void begin();
    
    /**
     * @brief Registra uma operação de leitura
     * @param resource Recurso lido
     * @param resourceType Tipo de recurso
     * @param key Chave lida
     * @param value Valor lido
     * @param metadata Metadados da operação
     * @return Registro da operação
     */
    OperationLog read(const ResourceKey& resource, const ResourceType& resourceType, 
                      const std::string& key, const Value& value,
                      const std::unordered_map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Registra uma operação de criação
     * @param resource Recurso criado
     * @param resourceType Tipo de recurso
     * @param key Chave criada
     * @param value Valor criado
     * @param metadata Metadados da operação
     * @return Registro da operação
     */
    OperationLog create(const ResourceKey& resource, const ResourceType& resourceType, 
                        const std::string& key, const Value& value,
                        const std::unordered_map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Registra uma operação de atualização
     * @param resource Recurso atualizado
     * @param resourceType Tipo de recurso
     * @param key Chave atualizada
     * @param value Novo valor
     * @param previousValue Valor anterior
     * @param metadata Metadados da operação
     * @return Registro da operação
     */
    OperationLog update(const ResourceKey& resource, const ResourceType& resourceType, 
                        const std::string& key, const Value& value, const Value& previousValue,
                        const std::unordered_map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Registra uma operação de remoção
     * @param resource Recurso removido
     * @param resourceType Tipo de recurso
     * @param key Chave removida
     * @param previousValue Valor anterior
     * @param metadata Metadados da operação
     * @return Registro da operação
     */
    OperationLog remove(const ResourceKey& resource, const ResourceType& resourceType, 
                        const std::string& key, const Value& previousValue,
                        const std::unordered_map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Verifica se a transação expirou
     * @return true se expirou, false caso contrário
     */
    bool isExpired() const;
    
    /**
     * @brief Verifica se a transação tem operações de escrita
     * @return true se tem escritas, false caso contrário
     */
    bool hasWrites() const;
    
    /**
     * @brief Obtém todas as operações de escrita
     * @return Vetor com operações de escrita
     */
    std::vector<OperationLog> getWriteOperations() const;
    
    /**
     * @brief Obtém todas as operações de leitura
     * @return Vetor com operações de leitura
     */
    std::vector<OperationLog> getReadOperations() const;
    
    /**
     * @brief Obtém estatísticas da transação
     * @return Mapa com estatísticas
     */
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> getStats() const;
    
    /**
     * @brief Obtém o ID da transação
     * @return ID da transação
     */
    const TransactionId& getId() const { return id_; }
    
    /**
     * @brief Obtém o estado da transação
     * @return Estado da transação
     */
    TransactionState getState() const { return state_; }
    
    /**
     * @brief Define o estado da transação
     * @param state Novo estado
     */
    void setState(TransactionState state) { state_ = state; }
    
    /**
     * @brief Obtém o nível de isolamento
     * @return Nível de isolamento
     */
    IsolationLevel getIsolationLevel() const { return isolationLevel_; }
    
    /**
     * @brief Obtém o timeout da transação
     * @return Timeout em milissegundos
     */
    uint32_t getTimeout() const { return timeout_; }
    
    /**
     * @brief Obtém o timestamp de início
     * @return Timestamp de início
     */
    Timestamp getStartTime() const { return startTime_; }
    
    /**
     * @brief Obtém o timestamp de término
     * @return Timestamp de término ou 0 se não terminou
     */
    Timestamp getEndTime() const { return endTime_; }
    
    /**
     * @brief Define o timestamp de término
     * @param timestamp Timestamp de término
     */
    void setEndTime(Timestamp timestamp) { endTime_ = timestamp; }
    
    /**
     * @brief Obtém a versão da transação
     * @return Versão da transação
     */
    Version getVersion() const { return version_; }
    
    /**
     * @brief Obtém as operações da transação
     * @return Vetor com operações
     */
    const std::vector<OperationLog>& getOperations() const { return operations_; }
    
    /**
     * @brief Obtém metadados da transação
     * @return Mapa com metadados
     */
    const std::unordered_map<std::string, std::string>& getMetadata() const { return metadata_; }
    
    /**
     * @brief Define um metadado da transação
     * @param key Chave do metadado
     * @param value Valor do metadado
     */
    void setMetadata(const std::string& key, const std::string& value) { metadata_[key] = value; }
    
    /**
     * @brief Incrementa o contador de tentativas
     * @return Novo valor do contador
     */
    uint32_t incrementRetryCount() { return ++retryCount_; }
    
    /**
     * @brief Obtém o contador de tentativas
     * @return Contador de tentativas
     */
    uint32_t getRetryCount() const { return retryCount_; }
    
    /**
     * @brief Obtém o ID do usuário associado
     * @return ID do usuário ou string vazia
     */
    const UserId& getUserId() const { return userId_; }
    
    /**
     * @brief Verifica se a transação é somente leitura
     * @return true se for somente leitura, false caso contrário
     */
    bool isReadOnly() const { return readOnly_; }
    
private:
    TransactionId id_;                 ///< ID da transação
    TransactionState state_;           ///< Estado atual
    IsolationLevel isolationLevel_;    ///< Nível de isolamento
    uint32_t timeout_;                 ///< Timeout em milissegundos
    Timestamp startTime_;              ///< Timestamp de início
    Timestamp endTime_;                ///< Timestamp de término
    Version version_;                  ///< Versão da transação
    std::vector<OperationLog> operations_; ///< Operações da transação
    std::unordered_map<std::string, std::string> metadata_; ///< Metadados
    TransactionId parentTransactionId_; ///< ID da transação pai (se houver)
    UserId userId_;                    ///< ID do usuário
    bool readOnly_;                    ///< Se é somente leitura
    uint32_t retryCount_;              ///< Contador de tentativas
};

/**
 * @class MVCCTransactionManager
 * @brief Gerenciador de transações com MVCC
 */
/**
 * Classe WYDMVCCTransactionManager
 * 
 * Esta classe implementa a funcionalidade WYDMVCCTransactionManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MVCCTransactionManager {
public:
    /**
     * @typedef TransactionCallback
     * @brief Tipo para callback de eventos de transação
     */
    using TransactionCallback = std::function<void(const TransactionId&, const std::unordered_map<std::string, std::string>&)>;
    
    /**
     * @brief Construtor
     * @param options Opções do gerenciador
     */
    explicit MVCCTransactionManager(const std::unordered_map<std::string, std::string>& options = {});
    
    /**
     * @brief Destrutor
     */
    ~MVCCTransactionManager();
    
    /**
     * @brief Inicia uma nova transação
     * @param options Opções da transação
     * @return Nova transação
     */
    std::shared_ptr<Transaction> begin(const std::unordered_map<std::string, std::string>& options = {});
    
    /**
     * @brief Obtém uma transação pelo ID
     * @param txId ID da transação
     * @return Ponteiro para a transação ou nullptr
     */
    std::shared_ptr<Transaction> getTransaction(const TransactionId& txId);
    
    /**
     * @brief Confirma uma transação
     * @param txId ID da transação
     * @return true se confirmada com sucesso, false caso contrário
     */
    bool commit(const TransactionId& txId);
    
    /**
     * @brief Desfaz uma transação
     * @param txId ID da transação
     * @return true se desfeita com sucesso, false caso contrário
     */
    bool rollback(const TransactionId& txId);
    
    /**
     * @brief Executa uma ação dentro de uma transação
     * @param action Função a ser executada
     * @param options Opções da transação
     * @return Resultado da transação (sucesso/erro)
     */
    template <typename Func>
    /**
 * executeInTransaction
 * 
 * Implementa a funcionalidade executeInTransaction conforme especificação original.
 * @param action Parâmetro action
 * @param std::unordered_map<std::string Parâmetro std::unordered_map<std::string
 * @param {} Parâmetro {}
 * @return Retorna bool
 */

    bool executeInTransaction(Func action, const std::unordered_map<std::string, std::string>& options = {});
    
    /**
     * @brief Registra um callback para evento de transação
     * @param eventType Tipo de evento
     * @param callback Função de callback
     * @return ID do registro
     */
    uint64_t registerCallback(const std::string& eventType, TransactionCallback callback);
    
    /**
     * @brief Remove um callback registrado
     * @param eventType Tipo de evento
     * @param callbackId ID do registro
     * @return true se removido com sucesso, false caso contrário
     */
    bool unregisterCallback(const std::string& eventType, uint64_t callbackId);
    
    /**
     * @brief Obtém estatísticas do gerenciador
     * @return Mapa com estatísticas
     */
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> getStats() const;
    
private:
    std::unordered_map<std::string, std::string> options_;  ///< Opções do gerenciador
    std::unordered_map<TransactionId, std::shared_ptr<Transaction>> activeTransactions_;  ///< Transações ativas
    std::unordered_map<ResourceKey, TransactionId> commitLock_;  ///< Locks de commit
    
    // Estatísticas
    struct Statistics {
        uint64_t transactionsCreated = 0;
        uint64_t transactionsCommitted = 0;
        uint64_t transactionsRolledBack = 0;
        uint64_t transactionsAborted = 0;
        uint64_t transactionsExpired = 0;
        uint64_t conflictsDetected = 0;
        uint64_t writesPerformed = 0;
        uint64_t readsPerformed = 0;
        Timestamp lastActivity = 0;
    } stats_;
    
    // Callbacks
    struct CallbackEntry {
        uint64_t id;
        TransactionCallback callback;
    };
    std::unordered_map<std::string, std::vector<CallbackEntry>> callbacks_;
    uint64_t nextCallbackId_ = 1;
    
    // Mutex para proteção de acesso concorrente
    mutable std::shared_mutex mutex_;
    
    /**
     * @brief Verifica transações expiradas
     */
    void checkExpiredTransactions();
    
    /**
     * @brief Limpa dados antigos
     */
    void cleanupOldData();
    
    /**
     * @brief Dispara um evento de transação
     * @param eventType Tipo de evento
     * @param txId ID da transação
     * @param data Dados adicionais
     */
    void fireEvent(const std::string& eventType, const TransactionId& txId, const std::unordered_map<std::string, std::string>& data = {});
    
    /**
     * @brief Tenta adquirir lock para commit
     * @param tx Transação
     * @return true se adquirido, false caso contrário
     */
    bool acquireCommitLock(const Transaction& tx);
    
    /**
     * @brief Libera locks de commit
     * @param tx Transação
     */
    void releaseCommitLock(const Transaction& tx);
    
    /**
     * @brief Verifica conflitos de transação
     * @param tx Transação a verificar
     * @return true se há conflito, false caso contrário
     */
    bool hasConflicts(const Transaction& tx);
    
    /**
     * @brief Aplica operações da transação
     * @param tx Transação
     * @return true se aplicado com sucesso, false caso contrário
     */
    bool applyOperations(const Transaction& tx);
};

/**
 * @class ShardManager
 * @brief Gerenciador de shards para distribuição de dados
 */
/**
 * Classe WYDShardManager
 * 
 * Esta classe implementa a funcionalidade WYDShardManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ShardManager {
public:
    /**
     * @brief Construtor
     * @param strategy Estratégia de sharding
     * @param numShards Número inicial de shards
     */
    ShardManager(ShardingStrategy strategy = ShardingStrategy::HASH, uint32_t numShards = 4);
    
    /**
     * @brief Destrutor
     */
    ~ShardManager();
    
    /**
     * @brief Obtém o shard para uma chave
     * @param key Chave
     * @return ID do shard
     */
    uint32_t getShardForKey(const std::string& key);
    
    /**
     * @brief Adiciona um novo shard
     * @return ID do novo shard
     */
    uint32_t addShard();
    
    /**
     * @brief Remove um shard
     * @param shardId ID do shard
     * @return true se removido com sucesso, false caso contrário
     */
    bool removeShard(uint32_t shardId);
    
    /**
     * @brief Rebalanceia os dados entre shards
     * @param progressCallback Função de callback para progresso
     * @return true se rebalanceado com sucesso, false caso contrário
     */
    bool rebalance(std::function<void(float)> progressCallback = nullptr);
    
    /**
     * @brief Obtém o número de shards
     * @return Número de shards
     */
    uint32_t getShardCount() const;
    
    /**
     * @brief Verifica se um shard está disponível
     * @param shardId ID do shard
     * @return true se disponível, false caso contrário
     */
    bool isShardAvailable(uint32_t shardId) const;
    
    /**
     * @brief Obtém estatísticas de sharding
     * @return Mapa com estatísticas
     */
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> getStats() const;
    
private:
    ShardingStrategy strategy_;     ///< Estratégia de sharding
    uint32_t shardCount_;           ///< Número de shards
    std::vector<bool> shardStatus_; ///< Status dos shards (disponível/indisponível)
    
    // Estatísticas
    struct Statistics {
        uint64_t keyLookups = 0;
        uint64_t rebalanceOperations = 0;
        uint64_t shardAdditions = 0;
        uint64_t shardRemovals = 0;
        uint64_t keyMigrations = 0;
    } stats_;
    
    // Mutex para proteção de acesso concorrente
    mutable std::shared_mutex mutex_;
    
    /**
     * @brief Calcula hash para uma chave
     * @param key Chave
     * @return Valor do hash
     */
    uint64_t calculateHash(const std::string& key) const;
    
    /**
     * @brief Migra chaves entre shards
     * @param fromShard Shard de origem
     * @param toShard Shard de destino
     * @param keyPercent Porcentagem de chaves a migrar
     * @return Número de chaves migradas
     */
    uint64_t migrateKeys(uint32_t fromShard, uint32_t toShard, float keyPercent);
};

/**
 * @class DatabaseSystem
 * @brief Sistema completo de banco de dados
 * 
 * Fornece uma interface unificada para gerenciamento de transações,
 * armazenamento e sharding.
 */
/**
 * Classe WYDDatabaseSystem
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseSystem {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância singleton
     */
    static DatabaseSystem& getInstance();
    
    /**
     * @brief Inicializa o sistema
     * @param options Opções de inicialização
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool initialize(const std::unordered_map<std::string, std::string>& options = {});
    
    /**
     * @brief Finaliza o sistema
     */
    void shutdown();
    
    /**
     * @brief Obtém o gerenciador de transações
     * @return Referência para o gerenciador de transações
     */
    MVCCTransactionManager& getTransactionManager();
    
    /**
     * @brief Obtém o gerenciador de shards
     * @return Referência para o gerenciador de shards
     */
    ShardManager& getShardManager();
    
    /**
     * @brief Cria uma transação
     * @param options Opções da transação
     * @return Nova transação
     */
    std::shared_ptr<Transaction> createTransaction(const std::unordered_map<std::string, std::string>& options = {});
    
    /**
     * @brief Lê um valor
     * @param txId ID da transação
     * @param resource Recurso
     * @param key Chave
     * @return Valor lido ou std::nullopt em caso de erro
     */
    std::optional<Value> read(const TransactionId& txId, const ResourceKey& resource, const std::string& key);
    
    /**
     * @brief Cria um valor
     * @param txId ID da transação
     * @param resource Recurso
     * @param key Chave
     * @param value Valor
     * @return true se criado com sucesso, false caso contrário
     */
    bool create(const TransactionId& txId, const ResourceKey& resource, const std::string& key, const Value& value);
    
    /**
     * @brief Atualiza um valor
     * @param txId ID da transação
     * @param resource Recurso
     * @param key Chave
     * @param value Novo valor
     * @return true se atualizado com sucesso, false caso contrário
     */
    bool update(const TransactionId& txId, const ResourceKey& resource, const std::string& key, const Value& value);
    
    /**
     * @brief Remove um valor
     * @param txId ID da transação
     * @param resource Recurso
     * @param key Chave
     * @return true se removido com sucesso, false caso contrário
     */
    bool remove(const TransactionId& txId, const ResourceKey& resource, const std::string& key);
    
    /**
     * @brief Confirma uma transação
     * @param txId ID da transação
     * @return true se confirmada com sucesso, false caso contrário
     */
    bool commit(const TransactionId& txId);
    
    /**
     * @brief Desfaz uma transação
     * @param txId ID da transação
     * @return true se desfeita com sucesso, false caso contrário
     */
    bool rollback(const TransactionId& txId);
    
    /**
     * @brief Executa ação em uma transação
     * @param action Função a ser executada
     * @param options Opções da transação
     * @return Resultado da transação (sucesso/erro)
     */
    template <typename Func>
    /**
 * executeInTransaction
 * 
 * Implementa a funcionalidade executeInTransaction conforme especificação original.
 * @param action Parâmetro action
 * @param std::unordered_map<std::string Parâmetro std::unordered_map<std::string
 * @param {} Parâmetro {}
 * @return Retorna bool
 */

    bool executeInTransaction(Func action, const std::unordered_map<std::string, std::string>& options = {});
    
    /**
     * @brief Obtém estatísticas do sistema
     * @return Mapa com estatísticas
     */
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> getStats() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    DatabaseSystem();
    
    /**
     * @brief Construtor de cópia deletado (singleton)
     */
    DatabaseSystem(const DatabaseSystem&) = delete;
    
    /**
     * @brief Operador de atribuição deletado (singleton)
     */
    DatabaseSystem& operator=(const DatabaseSystem&) = delete;
    
    bool initialized_ = false;                         ///< Se foi inicializado
    std::unique_ptr<MVCCTransactionManager> txManager_; ///< Gerenciador de transações
    std::unique_ptr<ShardManager> shardManager_;        ///< Gerenciador de shards
    
    // Storage layer (simplificado)
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<Version, Value>>> storage_;
    mutable std::shared_mutex storageMutex_;
};

} // namespace database
} // namespace wydbr

#endif // WYDBR_DATABASE_SYSTEM_H