/**
 * DatabaseSystem.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/reverse/database/DatabaseSystem.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef DATABASESYSTEM_H
#define DATABASESYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <array>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <set>
#include <queue>
#include <cstdint>
#include <variant>
#include <optional>

namespace wydbr {


/**
 * @file DatabaseSystem.h
 * @brief Sistema avançado de banco de dados reconstruído do WYD original
 * 
 * Este arquivo contém a reconstrução do sistema de banco de dados original do WYD,
 * incluindo schema completo, stored procedures, índices e sistema de cache
 * em memória. Implementa também mecanismos de recuperação de desastre e
 * alta disponibilidade.
 */

namespace wyd {
namespace reverse {
namespace database {

// Constantes do sistema de banco de dados
constexpr uint32_t DB_MAX_CONNECTIONS = 32;    // Máximo de conexões
constexpr uint32_t DB_CONNECTION_TIMEOUT = 30; // Timeout de conexão (segundos)
constexpr uint32_t DB_QUERY_TIMEOUT = 60;      // Timeout de consulta (segundos)
constexpr uint32_t DB_CACHE_SIZE_MB = 256;     // Tamanho do cache (MB)
constexpr uint32_t DB_MAX_QUERY_LENGTH = 8192; // Tamanho máximo de consulta
constexpr uint32_t DB_MAX_FIELD_LENGTH = 256;  // Tamanho máximo de campo
constexpr uint32_t DB_MAX_STORED_PROCEDURES = 128; // Máximo de stored procedures
constexpr uint32_t DB_MAX_INDICES = 256;       // Máximo de índices
constexpr uint32_t DB_BACKUP_INTERVAL = 3600;  // Intervalo de backup (segundos)
constexpr uint32_t DB_VACUUM_INTERVAL = 86400; // Intervalo de vacuum (segundos)
constexpr uint32_t DB_MAX_BATCH_SIZE = 1000;   // Tamanho máximo de batch

/**
 * @brief Tipo de banco de dados
 */
enum class DatabaseType : uint8_t {
    MYSQL = 0,                         // MySQL
    POSTGRESQL = 1,                    // PostgreSQL
    SQLITE = 2,                        // SQLite
    MSSQL = 3,                         // Microsoft SQL Server
    ORACLE = 4,                        // Oracle
    CUSTOM = 5,                        // Personalizado
};

/**
 * @brief Tipo de transação
 */
enum class TransactionType : uint8_t {
    READ_UNCOMMITTED = 0,              // Leitura não confirmada
    READ_COMMITTED = 1,                // Leitura confirmada
    REPEATABLE_READ = 2,               // Leitura repetível
    SERIALIZABLE = 3,                  // Serializável
};

/**
 * @brief Tipo de índice
 */
enum class IndexType : uint8_t {
    HASH = 0,                          // Hash
    BTREE = 1,                         // B-Tree
    RTREE = 2,                         // R-Tree
    BITMAP = 3,                        // Bitmap
    FULLTEXT = 4,                      // Texto completo
};

/**
 * @brief Operação de backup
 */
enum class BackupOperation : uint8_t {
    FULL = 0,                          // Completo
    INCREMENTAL = 1,                   // Incremental
    DIFFERENTIAL = 2,                  // Diferencial
    LOG = 3,                           // Log
};

/**
 * @brief Estado de conexão
 */
enum class ConnectionState : uint8_t {
    DISCONNECTED = 0,                  // Desconectado
    CONNECTING = 1,                    // Conectando
    CONNECTED = 2,                     // Conectado
    EXECUTING = 3,                     // Executando
    FETCHING = 4,                      // Buscando
    ERROR = 5,                         // Erro
};

/**
 * @brief Modo de cache
 */
enum class CacheMode : uint8_t {
    NONE = 0,                          // Nenhum
    READ_ONLY = 1,                     // Somente leitura
    WRITE_THROUGH = 2,                 // Escrita direta
    WRITE_BACK = 3,                    // Escrita posterior
    WRITE_AROUND = 4,                  // Escrita ao redor
};

/**
 * @brief Política de expiração de cache
 */
enum class CachePolicy : uint8_t {
    LRU = 0,                           // Menos recentemente usado
    LFU = 1,                           // Menos frequentemente usado
    FIFO = 2,                          // Primeiro a entrar, primeiro a sair
    ARC = 3,                           // Cache adaptativo
    MRU = 4,                           // Mais recentemente usado
    TTL = 5,                           // Tempo de vida
};

/**
 * @brief Tipo de campo
 */
enum class FieldType : uint8_t {
    INT8 = 0,                          // Integer 8-bit
    INT16 = 1,                         // Integer 16-bit
    INT32 = 2,                         // Integer 32-bit
    INT64 = 3,                         // Integer 64-bit
    UINT8 = 4,                         // Unsigned integer 8-bit
    UINT16 = 5,                        // Unsigned integer 16-bit
    UINT32 = 6,                        // Unsigned integer 32-bit
    UINT64 = 7,                         // Unsigned integer 64-bit
    FLOAT = 8,                         // Float 32-bit
    DOUBLE = 9,                        // Float 64-bit
    CHAR = 10,                         // Caractere
    VARCHAR = 11,                      // String
    TEXT = 12,                         // Texto longo
    BLOB = 13,                         // Binary Large Object
    DATE = 14,                         // Data
    TIME = 15,                         // Hora
    DATETIME = 16,                     // Data e hora
    TIMESTAMP = 17,                    // Timestamp
    BOOLEAN = 18,                      // Booleano
    JSON = 19,                         // JSON
    UUID = 20,                         // UUID
    BINARY = 21,                       // Binário
    ENUM = 22,                         // Enumeração
    DECIMAL = 23,                      // Decimal
    CUSTOM = 24,                       // Personalizado
};

/**
 * @brief Campo de banco de dados
 */
struct DatabaseField {
    std::string name;                  // Nome
    FieldType type;                    // Tipo
    uint32_t size;                     // Tamanho
    bool nullable;                     // Anulável
    bool primaryKey;                   // Chave primária
    bool autoIncrement;                // Auto incremento
    bool unique;                       // Único
    std::string defaultValue;          // Valor padrão
    std::string check;                 // Restrição de verificação
    std::string foreignKey;            // Chave estrangeira
    
    DatabaseField()
        : name("")
        , type(FieldType::VARCHAR)
        , size(0)
        , nullable(true)
        , primaryKey(false)
        , autoIncrement(false)
        , unique(false)
        , defaultValue("")
        , check("")
        , foreignKey("")
    {}
};

/**
 * @brief Tabela de banco de dados
 */
struct DatabaseTable {
    std::string name;                  // Nome
    std::vector<DatabaseField> fields; // Campos
    std::vector<std::string> indices;  // Índices
    std::string primaryKey;            // Chave primária
    std::vector<std::string> uniqueKeys; // Chaves únicas
    std::vector<std::string> foreignKeys; // Chaves estrangeiras
    std::string schema;                // Schema
    std::string engine;                // Engine
    std::string collation;             // Collation
    
    DatabaseTable()
        : name("")
        , fields()
        , indices()
        , primaryKey("")
        , uniqueKeys()
        , foreignKeys()
        , schema("")
        , engine("InnoDB")
        , collation("utf8mb4_unicode_ci")
    {}
};

/**
 * @brief Índice de banco de dados
 */
struct DatabaseIndex {
    std::string name;                  // Nome
    std::string tableName;             // Nome da tabela
    std::vector<std::string> fields;   // Campos
    IndexType type;                    // Tipo
    bool unique;                       // Único
    std::string condition;             // Condição
    
    DatabaseIndex()
        : name("")
        , tableName("")
        , fields()
        , type(IndexType::BTREE)
        , unique(false)
        , condition("")
    {}
};

/**
 * @brief Stored procedure
 */
struct StoredProcedure {
    std::string name;                  // Nome
    std::string parameters;            // Parâmetros
    std::string returnType;            // Tipo de retorno
    std::string body;                  // Corpo
    std::string schema;                // Schema
    
    StoredProcedure()
        : name("")
        , parameters("")
        , returnType("")
        , body("")
        , schema("")
    {}
};

/**
 * @brief Trigger
 */
struct Trigger {
    std::string name;                  // Nome
    std::string tableName;             // Nome da tabela
    std::string timing;                // Timing (BEFORE, AFTER)
    std::string event;                 // Evento (INSERT, UPDATE, DELETE)
    std::string body;                  // Corpo
    
    Trigger()
        : name("")
        , tableName("")
        , timing("AFTER")
        , event("INSERT")
        , body("")
    {}
};

/**
 * @brief View
 */
struct DatabaseView {
    std::string name;                  // Nome
    std::string query;                 // Consulta
    std::string schema;                // Schema
    bool materialized;                 // Materializada
    
    DatabaseView()
        : name("")
        , query("")
        , schema("")
        , materialized(false)
    {}
};

/**
 * @brief Regra de particionamento
 */
struct PartitionRule {
    std::string name;                  // Nome
    std::string tableName;             // Nome da tabela
    std::string type;                  // Tipo (RANGE, LIST, HASH, KEY)
    std::string expression;            // Expressão
    std::vector<std::string> partitions; // Partições
    
    PartitionRule()
        : name("")
        , tableName("")
        , type("RANGE")
        , expression("")
        , partitions()
    {}
};

/**
 * @brief Esquema de banco de dados
 */
struct DatabaseSchema {
    std::string name;                  // Nome
    std::vector<DatabaseTable> tables; // Tabelas
    std::vector<DatabaseIndex> indices; // Índices
    std::vector<StoredProcedure> procedures; // Stored procedures
    std::vector<Trigger> triggers;     // Triggers
    std::vector<DatabaseView> views;   // Views
    std::vector<PartitionRule> partitions; // Regras de particionamento
    std::string charset;               // Charset
    std::string collation;             // Collation
    
    DatabaseSchema()
        : name("")
        , tables()
        , indices()
        , procedures()
        , triggers()
        , views()
        , partitions()
        , charset("utf8mb4")
        , collation("utf8mb4_unicode_ci")
    {}
};

/**
 * @brief Valor de banco de dados
 */
using DatabaseValue = std::variant<
    std::nullptr_t,
    int8_t,
    int16_t,
    int32_t,
    int64_t,
    uint8_t,
    uint16_t,
    uint32_t,
    uint64_t,
    float,
    double,
    bool,
    std::string,
    std::vector<uint8_t>
>;

/**
 * @brief Linha de banco de dados
 */
using DatabaseRow = std::vector<DatabaseValue>;

/**
 * @brief Resultado de consulta
 */
/**
 * Classe WYDQueryResult
 * 
 * Esta classe implementa a funcionalidade WYDQueryResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QueryResult {
public:
    /**
     * @brief Construtor
     */
    QueryResult()
        : columns_()
        , rows_()
        , affectedRows_(0)
        , lastInsertId_(0)
        , error_("")
        , success_(false)
        , queryTime_(0)
    {}
    
    /**
     * @brief Verifica se a consulta foi bem-sucedida
     * @return true se bem-sucedida
     */
    bool IsSuccess() const { return success_; }
    
    /**
     * @brief Obtém o erro
     * @return Erro
     */
    const std::string& GetError() const { return error_; }
    
    /**
     * @brief Obtém o tempo de consulta
     * @return Tempo em milissegundos
     */
    uint64_t GetQueryTime() const { return queryTime_; }
    
    /**
     * @brief Obtém o número de linhas afetadas
     * @return Número de linhas
     */
    uint64_t GetAffectedRows() const { return affectedRows_; }
    
    /**
     * @brief Obtém o último ID inserido
     * @return ID
     */
    uint64_t GetLastInsertId() const { return lastInsertId_; }
    
    /**
     * @brief Obtém o número de colunas
     * @return Número de colunas
     */
    size_t GetColumnCount() const { return columns_.size(); }
    
    /**
     * @brief Obtém o número de linhas
     * @return Número de linhas
     */
    size_t GetRowCount() const { return rows_.size(); }
    
    /**
     * @brief Obtém os nomes das colunas
     * @return Nomes das colunas
     */
    const std::vector<std::string>& GetColumns() const { return columns_; }
    
    /**
     * @brief Obtém as linhas
     * @return Linhas
     */
    const std::vector<DatabaseRow>& GetRows() const { return rows_; }
    
    /**
     * @brief Obtém o índice de uma coluna pelo nome
     * @param name Nome da coluna
     * @return Índice, ou -1 se não encontrado
     */
    int GetColumnIndex(const std::string& name) const {
        for (size_t i = 0; i < columns_.size(); ++i) {
            if (columns_[i] == name) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    /**
     * @brief Obtém um valor pelo índice de linha e coluna
     * @param row Índice da linha
     * @param col Índice da coluna
     * @return Valor
     */
    const DatabaseValue& GetValue(size_t row, size_t col) const {
        static const DatabaseValue nullValue = nullptr;
        if (row >= rows_.size() || col >= columns_.size()) {
            return nullValue;
        }
        return rows_[row][col];
    }
    
    /**
     * @brief Obtém um valor pelo índice de linha e nome de coluna
     * @param row Índice da linha
     * @param colName Nome da coluna
     * @return Valor
     */
    const DatabaseValue& GetValue(size_t row, const std::string& colName) const {
        int colIndex = GetColumnIndex(colName);
        if (colIndex < 0) {
            static const DatabaseValue nullValue = nullptr;
            return nullValue;
        }
        return GetValue(row, static_cast<size_t>(colIndex));
    }
    
    /**
     * @brief Define o status de sucesso
     * @param success Sucesso
     */
    void SetSuccess(bool success) { success_ = success; }
    
    /**
     * @brief Define o erro
     * @param error Erro
     */
    void SetError(const std::string& error) { error_ = error; }
    
    /**
     * @brief Define o tempo de consulta
     * @param queryTime Tempo em milissegundos
     */
    void SetQueryTime(uint64_t queryTime) { queryTime_ = queryTime; }
    
    /**
     * @brief Define o número de linhas afetadas
     * @param affectedRows Número de linhas
     */
    void SetAffectedRows(uint64_t affectedRows) { affectedRows_ = affectedRows; }
    
    /**
     * @brief Define o último ID inserido
     * @param lastInsertId ID
     */
    void SetLastInsertId(uint64_t lastInsertId) { lastInsertId_ = lastInsertId; }
    
    /**
     * @brief Define as colunas
     * @param columns Colunas
     */
    void SetColumns(const std::vector<std::string>& columns) { columns_ = columns; }
    
    /**
     * @brief Define as linhas
     * @param rows Linhas
     */
    void SetRows(const std::vector<DatabaseRow>& rows) { rows_ = rows; }
    
    /**
     * @brief Adiciona uma linha
     * @param row Linha
     */
    void AddRow(const DatabaseRow& row) { rows_.push_back(row); }
    
private:
    std::vector<std::string> columns_; // Colunas
    std::vector<DatabaseRow> rows_;    // Linhas
    uint64_t affectedRows_;            // Linhas afetadas
    uint64_t lastInsertId_;            // Último ID inserido
    std::string error_;                // Erro
    bool success_;                     // Sucesso
    uint64_t queryTime_;               // Tempo de consulta
};

/**
 * @brief Parâmetro de consulta
 */
struct QueryParameter {
    FieldType type;                    // Tipo
    DatabaseValue value;               // Valor
    
    QueryParameter()
        : type(FieldType::VARCHAR)
        , value("")
    {}
    
    QueryParameter(FieldType t, const DatabaseValue& v)
        : type(t)
        , value(v)
    {}
};

/**
 * @brief Configuração de conexão
 */
struct ConnectionConfig {
    std::string host;                  // Host
    uint16_t port;                     // Porta
    std::string username;              // Usuário
    std::string password;              // Senha
    std::string database;              // Banco de dados
    std::string charset;               // Charset
    uint32_t timeout;                  // Timeout
    uint32_t maxConnections;           // Máximo de conexões
    bool autoReconnect;                // Reconexão automática
    bool useSSL;                       // Usar SSL
    std::string sslCert;               // Certificado SSL
    std::string sslKey;                // Chave SSL
    std::string sslCA;                 // CA SSL
    
    ConnectionConfig()
        : host("localhost")
        , port(3306)
        , username("")
        , password("")
        , database("")
        , charset("utf8mb4")
        , timeout(30)
        , maxConnections(10)
        , autoReconnect(true)
        , useSSL(false)
        , sslCert("")
        , sslKey("")
        , sslCA("")
    {}
};

/**
 * @brief Configuração de pool de conexões
 */
struct ConnectionPoolConfig {
    uint32_t minConnections;           // Mínimo de conexões
    uint32_t maxConnections;           // Máximo de conexões
    uint32_t connectionTimeout;        // Timeout de conexão
    uint32_t idleTimeout;              // Timeout de inatividade
    bool validateConnection;           // Validar conexão
    std::string validationQuery;       // Consulta de validação
    
    ConnectionPoolConfig()
        : minConnections(5)
        , maxConnections(32)
        , connectionTimeout(30)
        , idleTimeout(600)
        , validateConnection(true)
        , validationQuery("SELECT 1")
    {}
};

/**
 * @brief Configuração de cache
 */
struct CacheConfig {
    CacheMode mode;                    // Modo
    CachePolicy policy;                // Política
    uint32_t size;                     // Tamanho
    uint32_t ttl;                      // Tempo de vida
    bool useCompression;               // Usar compressão
    uint32_t compressionLevel;         // Nível de compressão
    std::vector<std::string> skipTables; // Tabelas a pular
    
    CacheConfig()
        : mode(CacheMode::WRITE_THROUGH)
        , policy(CachePolicy::LRU)
        , size(DB_CACHE_SIZE_MB)
        , ttl(3600)
        , useCompression(true)
        , compressionLevel(6)
        , skipTables()
    {}
};

/**
 * @brief Configuração de replicação
 */
struct ReplicationConfig {
    bool enabled;                      // Habilitado
    std::string masterHost;            // Host mestre
    uint16_t masterPort;               // Porta mestre
    std::string masterUser;            // Usuário mestre
    std::string masterPassword;        // Senha mestre
    std::string replicaHost;           // Host réplica
    uint16_t replicaPort;              // Porta réplica
    std::string replicaUser;           // Usuário réplica
    std::string replicaPassword;       // Senha réplica
    uint32_t retryInterval;            // Intervalo de retry
    
    ReplicationConfig()
        : enabled(false)
        , masterHost("")
        , masterPort(3306)
        , masterUser("")
        , masterPassword("")
        , replicaHost("")
        , replicaPort(3306)
        , replicaUser("")
        , replicaPassword("")
        , retryInterval(60)
    {}
};

/**
 * @brief Configuração de backup
 */
struct BackupConfig {
    bool enabled;                      // Habilitado
    BackupOperation type;              // Tipo
    std::string path;                  // Caminho
    uint32_t interval;                 // Intervalo
    uint32_t retention;                // Retenção
    bool compress;                     // Comprimir
    bool encrypt;                      // Criptografar
    std::string encryptionKey;         // Chave de criptografia
    std::vector<std::string> excludeTables; // Tabelas a excluir
    
    BackupConfig()
        : enabled(true)
        , type(BackupOperation::FULL)
        , path("./backups")
        , interval(DB_BACKUP_INTERVAL)
        , retention(7)
        , compress(true)
        , encrypt(false)
        , encryptionKey("")
        , excludeTables()
    {}
};

/**
 * @brief Configuração de banco de dados
 */
struct DatabaseConfig {
    DatabaseType type;                 // Tipo
    ConnectionConfig connection;       // Conexão
    ConnectionPoolConfig pool;         // Pool
    CacheConfig cache;                 // Cache
    ReplicationConfig replication;     // Replicação
    BackupConfig backup;               // Backup
    bool logQueries;                   // Registrar consultas
    std::string logPath;               // Caminho de log
    uint32_t queryTimeout;             // Timeout de consulta
    bool automaticMigration;           // Migração automática
    std::string migrationPath;         // Caminho de migração
    
    DatabaseConfig()
        : type(DatabaseType::MYSQL)
        , connection()
        , pool()
        , cache()
        , replication()
        , backup()
        , logQueries(false)
        , logPath("./logs")
        , queryTimeout(DB_QUERY_TIMEOUT)
        , automaticMigration(false)
        , migrationPath("./migrations")
    {}
};

/**
 * @brief Estatísticas de banco de dados
 */
struct DatabaseStats {
    uint64_t queriesExecuted;          // Consultas executadas
    uint64_t queriesFailed;            // Consultas falhas
    uint64_t queryTotalTime;           // Tempo total de consulta
    uint64_t totalRows;                // Total de linhas
    uint64_t totalConnections;         // Total de conexões
    uint64_t activeConnections;        // Conexões ativas
    uint64_t cacheHits;                // Hits de cache
    uint64_t cacheMisses;              // Misses de cache
    uint64_t cacheSize;                // Tamanho do cache
    uint64_t backupsPerformed;         // Backups realizados
    uint64_t databaseSize;             // Tamanho do banco de dados
    
    DatabaseStats()
        : queriesExecuted(0)
        , queriesFailed(0)
        , queryTotalTime(0)
        , totalRows(0)
        , totalConnections(0)
        , activeConnections(0)
        , cacheHits(0)
        , cacheMisses(0)
        , cacheSize(0)
        , backupsPerformed(0)
        , databaseSize(0)
    {}
    
    // Reinicia as estatísticas
    /**
 * Reset
 * 
 * Implementa a funcionalidade Reset conforme especificação original.
 */

    void Reset(){
        queriesExecuted = 0;
        queriesFailed = 0;
        queryTotalTime = 0;
        totalRows = 0;
        totalConnections = 0;
        activeConnections = 0;
        cacheHits = 0;
        cacheMisses = 0;
        cacheSize = 0;
        backupsPerformed = 0;
        databaseSize = 0;
    }
};

/**
 * @brief Resultado de backup
 */
struct BackupResult {
    bool success;                      // Sucesso
    std::string filename;              // Nome do arquivo
    uint64_t size;                     // Tamanho
    uint64_t duration;                 // Duração
    std::string error;                 // Erro
    
    BackupResult()
        : success(false)
        , filename("")
        , size(0)
        , duration(0)
        , error("")
    {}
};

/**
 * @brief Resultado de migração
 */
struct MigrationResult {
    bool success;                      // Sucesso
    std::string version;               // Versão
    uint32_t scriptsApplied;           // Scripts aplicados
    std::string error;                 // Erro
    
    MigrationResult()
        : success(false)
        , version("")
        , scriptsApplied(0)
        , error("")
    {}
};

/**
 * @brief Builder de consulta
 */
/**
 * Classe WYDQueryBuilder
 * 
 * Esta classe implementa a funcionalidade WYDQueryBuilder conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QueryBuilder {
public:
    /**
     * @brief Construtor
     */
    QueryBuilder() 
        : query_("")
        , parameters_() 
    {}
    
    /**
     * @brief Inicia uma consulta SELECT
     * @param fields Campos
     * @param tableName Nome da tabela
     * @return Referência para este builder
     */
    QueryBuilder& Select(const std::string& fields, const std::string& tableName) {
        query_ = "SELECT " + fields + " FROM " + tableName;
        return *this;
    }
    
    /**
     * @brief Inicia uma consulta INSERT
     * @param tableName Nome da tabela
     * @param fields Campos
     * @return Referência para este builder
     */
    QueryBuilder& Insert(const std::string& tableName, const std::string& fields) {
        query_ = "INSERT INTO " + tableName + " (" + fields + ") VALUES ";
        return *this;
    }
    
    /**
     * @brief Inicia uma consulta UPDATE
     * @param tableName Nome da tabela
     * @return Referência para este builder
     */
    QueryBuilder& Update(const std::string& tableName) {
        query_ = "UPDATE " + tableName + " SET ";
        return *this;
    }
    
    /**
     * @brief Inicia uma consulta DELETE
     * @param tableName Nome da tabela
     * @return Referência para este builder
     */
    QueryBuilder& Delete(const std::string& tableName) {
        query_ = "DELETE FROM " + tableName;
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula WHERE
     * @param condition Condição
     * @return Referência para este builder
     */
    QueryBuilder& Where(const std::string& condition) {
        query_ += " WHERE " + condition;
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula AND
     * @param condition Condição
     * @return Referência para este builder
     */
    QueryBuilder& And(const std::string& condition) {
        query_ += " AND " + condition;
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula OR
     * @param condition Condição
     * @return Referência para este builder
     */
    QueryBuilder& Or(const std::string& condition) {
        query_ += " OR " + condition;
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula GROUP BY
     * @param fields Campos
     * @return Referência para este builder
     */
    QueryBuilder& GroupBy(const std::string& fields) {
        query_ += " GROUP BY " + fields;
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula ORDER BY
     * @param fields Campos
     * @return Referência para este builder
     */
    QueryBuilder& OrderBy(const std::string& fields) {
        query_ += " ORDER BY " + fields;
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula LIMIT
     * @param limit Limite
     * @return Referência para este builder
     */
    QueryBuilder& Limit(uint32_t limit) {
        query_ += " LIMIT " + std::to_string(limit);
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula OFFSET
     * @param offset Offset
     * @return Referência para este builder
     */
    QueryBuilder& Offset(uint32_t offset) {
        query_ += " OFFSET " + std::to_string(offset);
        return *this;
    }
    
    /**
     * @brief Adiciona valores para INSERT
     * @param values Valores
     * @return Referência para este builder
     */
    QueryBuilder& Values(const std::string& values) {
        query_ += "(" + values + ")";
        return *this;
    }
    
    /**
     * @brief Adiciona atribuições para UPDATE
     * @param assignments Atribuições
     * @return Referência para este builder
     */
    QueryBuilder& Set(const std::string& assignments) {
        query_ += assignments;
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula JOIN
     * @param type Tipo (INNER, LEFT, RIGHT)
     * @param tableName Nome da tabela
     * @param condition Condição
     * @return Referência para este builder
     */
    QueryBuilder& Join(const std::string& type, const std::string& tableName, const std::string& condition) {
        query_ += " " + type + " JOIN " + tableName + " ON " + condition;
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula HAVING
     * @param condition Condição
     * @return Referência para este builder
     */
    QueryBuilder& Having(const std::string& condition) {
        query_ += " HAVING " + condition;
        return *this;
    }
    
    /**
     * @brief Adiciona uma cláusula UNION
     * @param query Consulta
     * @return Referência para este builder
     */
    QueryBuilder& Union(const std::string& query) {
        query_ += " UNION " + query;
        return *this;
    }
    
    /**
     * @brief Adiciona um parâmetro
     * @param type Tipo
     * @param value Valor
     * @return Referência para este builder
     */
    QueryBuilder& AddParameter(FieldType type, const DatabaseValue& value) {
        parameters_.push_back(QueryParameter(type, value));
        return *this;
    }
    
    /**
     * @brief Obtém a consulta SQL
     * @return Consulta
     */
    const std::string& GetQuery() const {
        return query_;
    }
    
    /**
     * @brief Obtém os parâmetros
     * @return Parâmetros
     */
    const std::vector<QueryParameter>& GetParameters() const {
        return parameters_;
    }
    
    /**
     * @brief Limpa o builder
     */
    void Clear() {
        query_.clear();
        parameters_.clear();
    }
    
private:
    std::string query_;                // Consulta SQL
    std::vector<QueryParameter> parameters_; // Parâmetros
};

/**
 * @brief Entrada de cache
 */
struct CacheEntry {
    std::string key;                   // Chave
    std::vector<uint8_t> data;         // Dados
    time_t timestamp;                  // Timestamp
    time_t expiry;                     // Expiração
    uint32_t accessCount;              // Contagem de acesso
    uint32_t size;                     // Tamanho
    
    CacheEntry()
        : key("")
        , data()
        , timestamp(0)
        , expiry(0)
        , accessCount(0)
        , size(0)
    {}
};

/**
 * @brief Script de migração
 */
struct MigrationScript {
    std::string version;               // Versão
    std::string description;           // Descrição
    std::string upScript;              // Script de aplicação
    std::string downScript;            // Script de reversão
    bool applied;                      // Aplicado
    time_t appliedAt;                  // Aplicado em
    
    MigrationScript()
        : version("")
        , description("")
        , upScript("")
        , downScript("")
        , applied(false)
        , appliedAt(0)
    {}
};

/**
 * @brief Interface de conexão de banco de dados
 */
/**
 * Classe WYDIDatabaseConnection
 * 
 * Esta classe implementa a funcionalidade WYDIDatabaseConnection conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class IDatabaseConnection {
public:
    /**
     * @brief Destrutor
     */
    virtual ~IDatabaseConnection() {}
    
    /**
     * @brief Abre a conexão
     * @return true se aberta com sucesso
     */
    virtual bool Open() = 0;
    
    /**
     * @brief Fecha a conexão
     */
    virtual void Close() = 0;
    
    /**
     * @brief Executa uma consulta
     * @param query Consulta
     * @return Resultado
     */
    virtual QueryResult Execute(const std::string& query) = 0;
    
    /**
     * @brief Executa uma consulta com parâmetros
     * @param query Consulta
     * @param parameters Parâmetros
     * @return Resultado
     */
    virtual QueryResult Execute(const std::string& query, const std::vector<QueryParameter>& parameters) = 0;
    
    /**
     * @brief Inicia uma transação
     * @param type Tipo
     * @return true se iniciada com sucesso
     */
    virtual bool BeginTransaction(TransactionType type = TransactionType::READ_COMMITTED) = 0;
    
    /**
     * @brief Confirma uma transação
     * @return true se confirmada com sucesso
     */
    virtual bool CommitTransaction() = 0;
    
    /**
     * @brief Reverte uma transação
     * @return true se revertida com sucesso
     */
    virtual bool RollbackTransaction() = 0;
    
    /**
     * @brief Verifica se a conexão está ativa
     * @return true se ativa
     */
    virtual bool IsActive() = 0;
    
    /**
     * @brief Obtém o estado da conexão
     * @return Estado
     */
    virtual ConnectionState GetState() = 0;
    
    /**
     * @brief Obtém o tempo de atividade
     * @return Tempo em segundos
     */
    virtual uint32_t GetUptime() = 0;
    
    /**
     * @brief Obtém o tipo de banco de dados
     * @return Tipo
     */
    virtual DatabaseType GetType() = 0;
    
    /**
     * @brief Escapa uma string para uso em SQL
     * @param value String
     * @return String escapada
     */
    virtual std::string EscapeString(const std::string& value) = 0;
};

/**
 * @brief Interface de gerenciamento de banco de dados
 */
/**
 * Classe WYDIDatabaseManager
 * 
 * Esta classe implementa a funcionalidade WYDIDatabaseManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class IDatabaseManager {
public:
    /**
     * @brief Destrutor
     */
    virtual ~IDatabaseManager() {}
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const DatabaseConfig& config) = 0;
    
    /**
     * @brief Finaliza o gerenciador
     */
    virtual void Shutdown() = 0;
    
    /**
     * @brief Obtém uma conexão do pool
     * @return Ponteiro para a conexão, ou nullptr se falhar
     */
    virtual IDatabaseConnection* GetConnection() = 0;
    
    /**
     * @brief Libera uma conexão de volta para o pool
     * @param connection Conexão
     */
    virtual void ReleaseConnection(IDatabaseConnection* connection) = 0;
    
    /**
     * @brief Executa uma consulta
     * @param query Consulta
     * @return Resultado
     */
    virtual QueryResult ExecuteQuery(const std::string& query) = 0;
    
    /**
     * @brief Executa uma consulta com parâmetros
     * @param query Consulta
     * @param parameters Parâmetros
     * @return Resultado
     */
    virtual QueryResult ExecuteQuery(const std::string& query, const std::vector<QueryParameter>& parameters) = 0;
    
    /**
     * @brief Executa uma consulta com cache
     * @param query Consulta
     * @param ttl Tempo de vida (0 para usar o padrão)
     * @return Resultado
     */
    virtual QueryResult ExecuteQueryWithCache(const std::string& query, uint32_t ttl = 0) = 0;
    
    /**
     * @brief Executa uma consulta com cache e parâmetros
     * @param query Consulta
     * @param parameters Parâmetros
     * @param ttl Tempo de vida (0 para usar o padrão)
     * @return Resultado
     */
    virtual QueryResult ExecuteQueryWithCache(const std::string& query, const std::vector<QueryParameter>& parameters, uint32_t ttl = 0) = 0;
    
    /**
     * @brief Executa uma consulta em lote
     * @param queries Consultas
     * @return Resultados
     */
    virtual std::vector<QueryResult> ExecuteBatch(const std::vector<std::string>& queries) = 0;
    
    /**
     * @brief Executa uma transação
     * @param queries Consultas
     * @param type Tipo
     * @return true se executada com sucesso
     */
    virtual bool ExecuteTransaction(const std::vector<std::string>& queries, TransactionType type = TransactionType::READ_COMMITTED) = 0;
    
    /**
     * @brief Executa uma stored procedure
     * @param name Nome
     * @param parameters Parâmetros
     * @return Resultado
     */
    virtual QueryResult ExecuteStoredProcedure(const std::string& name, const std::vector<QueryParameter>& parameters) = 0;
    
    /**
     * @brief Executa um backup
     * @param operation Operação
     * @param path Caminho (vazio para usar o padrão)
     * @return Resultado
     */
    virtual BackupResult ExecuteBackup(BackupOperation operation = BackupOperation::FULL, const std::string& path = "") = 0;
    
    /**
     * @brief Restaura um backup
     * @param filename Nome do arquivo
     * @return true se restaurado com sucesso
     */
    virtual bool RestoreBackup(const std::string& filename) = 0;
    
    /**
     * @brief Executa migrações
     * @param version Versão alvo (vazio para a mais recente)
     * @return Resultado
     */
    virtual MigrationResult ExecuteMigrations(const std::string& version = "") = 0;
    
    /**
     * @brief Reverte migrações
     * @param version Versão alvo
     * @return Resultado
     */
    virtual MigrationResult RevertMigrations(const std::string& version) = 0;
    
    /**
     * @brief Obtém o esquema de banco de dados
     * @return Esquema
     */
    virtual DatabaseSchema GetSchema() = 0;
    
    /**
     * @brief Cria uma tabela
     * @param table Tabela
     * @return true se criada com sucesso
     */
    virtual bool CreateTable(const DatabaseTable& table) = 0;
    
    /**
     * @brief Altera uma tabela
     * @param table Tabela
     * @return true se alterada com sucesso
     */
    virtual bool AlterTable(const DatabaseTable& table) = 0;
    
    /**
     * @brief Exclui uma tabela
     * @param tableName Nome da tabela
     * @return true se excluída com sucesso
     */
    virtual bool DropTable(const std::string& tableName) = 0;
    
    /**
     * @brief Cria um índice
     * @param index Índice
     * @return true se criado com sucesso
     */
    virtual bool CreateIndex(const DatabaseIndex& index) = 0;
    
    /**
     * @brief Exclui um índice
     * @param indexName Nome do índice
     * @param tableName Nome da tabela
     * @return true se excluído com sucesso
     */
    virtual bool DropIndex(const std::string& indexName, const std::string& tableName) = 0;
    
    /**
     * @brief Cria uma stored procedure
     * @param procedure Stored procedure
     * @return true se criada com sucesso
     */
    virtual bool CreateStoredProcedure(const StoredProcedure& procedure) = 0;
    
    /**
     * @brief Exclui uma stored procedure
     * @param name Nome
     * @return true se excluída com sucesso
     */
    virtual bool DropStoredProcedure(const std::string& name) = 0;
    
    /**
     * @brief Cria uma trigger
     * @param trigger Trigger
     * @return true se criada com sucesso
     */
    virtual bool CreateTrigger(const Trigger& trigger) = 0;
    
    /**
     * @brief Exclui uma trigger
     * @param name Nome
     * @return true se excluída com sucesso
     */
    virtual bool DropTrigger(const std::string& name) = 0;
    
    /**
     * @brief Cria uma view
     * @param view View
     * @return true se criada com sucesso
     */
    virtual bool CreateView(const DatabaseView& view) = 0;
    
    /**
     * @brief Exclui uma view
     * @param name Nome
     * @return true se excluída com sucesso
     */
    virtual bool DropView(const std::string& name) = 0;
    
    /**
     * @brief Limpa o cache
     */
    virtual void ClearCache() = 0;
    
    /**
     * @brief Remove uma entrada do cache
     * @param key Chave
     * @return true se removida com sucesso
     */
    virtual bool RemoveCacheEntry(const std::string& key) = 0;
    
    /**
     * @brief Obtém estatísticas do banco de dados
     * @return Estatísticas
     */
    virtual DatabaseStats GetStats() = 0;
    
    /**
     * @brief Obtém a configuração
     * @return Configuração
     */
    virtual const DatabaseConfig& GetConfig() = 0;
    
    /**
     * @brief Verifica se o gerenciador está inicializado
     * @return true se inicializado
     */
    virtual bool IsInitialized() = 0;
};

/**
 * @brief Sistema de banco de dados
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
     * @return Referência para a instância
     */
    static DatabaseSystem& GetInstance();
    
    /**
     * @brief Inicializa o sistema
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const DatabaseConfig& config = DatabaseConfig());
    
    /**
     * @brief Finaliza o sistema
     */
    void Shutdown();
    
    /**
     * @brief Obtém o gerenciador de banco de dados
     * @return Ponteiro para o gerenciador
     */
    IDatabaseManager* GetManager() const { return manager_.get(); }
    
    /**
     * @brief Obtém uma conexão
     * @return Ponteiro para a conexão, ou nullptr se falhar
     */
    IDatabaseConnection* GetConnection() const { return manager_->GetConnection(); }
    
    /**
     * @brief Libera uma conexão
     * @param connection Conexão
     */
    void ReleaseConnection(IDatabaseConnection* connection) const { manager_->ReleaseConnection(connection); }
    
    /**
     * @brief Executa uma consulta
     * @param query Consulta
     * @return Resultado
     */
    QueryResult ExecuteQuery(const std::string& query) const { return manager_->ExecuteQuery(query); }
    
    /**
     * @brief Executa uma consulta com parâmetros
     * @param query Consulta
     * @param parameters Parâmetros
     * @return Resultado
     */
    QueryResult ExecuteQuery(const std::string& query, const std::vector<QueryParameter>& parameters) const {
        return manager_->ExecuteQuery(query, parameters);
    }
    
    /**
     * @brief Executa uma consulta construída por um builder
     * @param builder Builder
     * @return Resultado
     */
    QueryResult ExecuteQuery(const QueryBuilder& builder) const {
        return manager_->ExecuteQuery(builder.GetQuery(), builder.GetParameters());
    }
    
    /**
     * @brief Executa uma consulta com cache
     * @param query Consulta
     * @param ttl Tempo de vida (0 para usar o padrão)
     * @return Resultado
     */
    QueryResult ExecuteQueryWithCache(const std::string& query, uint32_t ttl = 0) const {
        return manager_->ExecuteQueryWithCache(query, ttl);
    }
    
    /**
     * @brief Executa uma consulta em lote
     * @param queries Consultas
     * @return Resultados
     */
    std::vector<QueryResult> ExecuteBatch(const std::vector<std::string>& queries) const {
        return manager_->ExecuteBatch(queries);
    }
    
    /**
     * @brief Executa uma transação
     * @param queries Consultas
     * @param type Tipo
     * @return true se executada com sucesso
     */
    bool ExecuteTransaction(const std::vector<std::string>& queries, TransactionType type = TransactionType::READ_COMMITTED) const {
        return manager_->ExecuteTransaction(queries, type);
    }
    
    /**
     * @brief Obtém o esquema de banco de dados
     * @return Esquema
     */
    DatabaseSchema GetSchema() const { return manager_->GetSchema(); }
    
    /**
     * @brief Executa um backup
     * @param operation Operação
     * @param path Caminho (vazio para usar o padrão)
     * @return Resultado
     */
    BackupResult ExecuteBackup(BackupOperation operation = BackupOperation::FULL, const std::string& path = "") const {
        return manager_->ExecuteBackup(operation, path);
    }
    
    /**
     * @brief Restaura um backup
     * @param filename Nome do arquivo
     * @return true se restaurado com sucesso
     */
    bool RestoreBackup(const std::string& filename) const { return manager_->RestoreBackup(filename); }
    
    /**
     * @brief Executa migrações
     * @param version Versão alvo (vazio para a mais recente)
     * @return Resultado
     */
    MigrationResult ExecuteMigrations(const std::string& version = "") const {
        return manager_->ExecuteMigrations(version);
    }
    
    /**
     * @brief Obtém estatísticas do banco de dados
     * @return Estatísticas
     */
    DatabaseStats GetStats() const { return manager_->GetStats(); }
    
    /**
     * @brief Limpa o cache
     */
    void ClearCache() const { manager_->ClearCache(); }
    
    /**
     * @brief Verifica se o sistema está inicializado
     * @return true se inicializado
     */
    bool IsInitialized() const { return initialized_ && manager_ && manager_->IsInitialized(); }
    
    /**
     * @brief Cria o esquema do WYD
     * @return true se criado com sucesso
     */
    bool CreateWYDSchema();
    
    /**
     * @brief Carrega o esquema do WYD
     * @return Esquema
     */
    DatabaseSchema LoadWYDSchema() const;
    
    /**
     * @brief Obtém tabelas do WYD
     * @return Tabelas
     */
    std::vector<DatabaseTable> GetWYDTables() const;
    
    /**
     * @brief Obtém procedimentos do WYD
     * @return Procedimentos
     */
    std::vector<StoredProcedure> GetWYDProcedures() const;
    
    /**
     * @brief Obtém triggers do WYD
     * @return Triggers
     */
    std::vector<Trigger> GetWYDTriggers() const;
    
    /**
     * @brief Obtém índices do WYD
     * @return Índices
     */
    std::vector<DatabaseIndex> GetWYDIndices() const;
    
    /**
     * @brief Executa procedimentos de manutenção
     * @return true se executados com sucesso
     */
    bool ExecuteMaintenanceProcedures();
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    DatabaseSystem();
    
    /**
     * @brief Destrutor
     */
    ~DatabaseSystem();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    DatabaseSystem(const DatabaseSystem&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    DatabaseSystem& operator=(const DatabaseSystem&) = delete;
    
    /**
     * @brief Cria o gerenciador de banco de dados apropriado
     * @param type Tipo
     * @return Ponteiro para o gerenciador
     */
    std::unique_ptr<IDatabaseManager> CreateManager(DatabaseType type);
    
    /**
     * @brief Thread de backup automático
     */
    void AutoBackupThread();
    
    /**
     * @brief Thread de manutenção
     */
    void MaintenanceThread();
    
    /**
     * @brief Cria a tabela de contas
     * @return Tabela
     */
    DatabaseTable CreateAccountTable() const;
    
    /**
     * @brief Cria a tabela de personagens
     * @return Tabela
     */
    DatabaseTable CreateCharacterTable() const;
    
    /**
     * @brief Cria a tabela de itens
     * @return Tabela
     */
    DatabaseTable CreateItemTable() const;
    
    /**
     * @brief Cria a tabela de inventários
     * @return Tabela
     */
    DatabaseTable CreateInventoryTable() const;
    
    /**
     * @brief Cria a tabela de guildas
     * @return Tabela
     */
    DatabaseTable CreateGuildTable() const;
    
    /**
     * @brief Cria a tabela de membros de guilda
     * @return Tabela
     */
    DatabaseTable CreateGuildMemberTable() const;
    
    /**
     * @brief Cria a tabela de habilidades
     * @return Tabela
     */
    DatabaseTable CreateSkillTable() const;
    
    /**
     * @brief Cria a tabela de quests
     * @return Tabela
     */
    DatabaseTable CreateQuestTable() const;
    
    /**
     * @brief Cria a tabela de amigos
     * @return Tabela
     */
    DatabaseTable CreateFriendTable() const;
    
    /**
     * @brief Cria a tabela de loja
     * @return Tabela
     */
    DatabaseTable CreateShopTable() const;
    
    /**
     * @brief Cria a tabela de transações
     * @return Tabela
     */
    DatabaseTable CreateTransactionTable() const;
    
    /**
     * @brief Cria a tabela de chat
     * @return Tabela
     */
    DatabaseTable CreateChatTable() const;
    
    /**
     * @brief Cria a tabela de logs
     * @return Tabela
     */
    DatabaseTable CreateLogTable() const;
    
    /**
     * @brief Cria a tabela de configurações
     * @return Tabela
     */
    DatabaseTable CreateConfigTable() const;
    
    /**
     * @brief Cria a stored procedure de inserção de personagem
     * @return Stored procedure
     */
    StoredProcedure CreateInsertCharacterProcedure() const;
    
    /**
     * @brief Cria a stored procedure de atualização de personagem
     * @return Stored procedure
     */
    StoredProcedure CreateUpdateCharacterProcedure() const;
    
    /**
     * @brief Cria a stored procedure de inserção de item
     * @return Stored procedure
     */
    StoredProcedure CreateInsertItemProcedure() const;
    
    /**
     * @brief Cria a stored procedure de atualização de item
     * @return Stored procedure
     */
    StoredProcedure CreateUpdateItemProcedure() const;
    
    /**
     * @brief Cria a stored procedure de inserção de transação
     * @return Stored procedure
     */
    StoredProcedure CreateInsertTransactionProcedure() const;
    
    /**
     * @brief Cria a stored procedure de inserção de log
     * @return Stored procedure
     */
    StoredProcedure CreateInsertLogProcedure() const;
    
    /**
     * @brief Cria a trigger de atualização de inventário
     * @return Trigger
     */
    Trigger CreateInventoryUpdateTrigger() const;
    
    /**
     * @brief Cria a trigger de atualização de guilda
     * @return Trigger
     */
    Trigger CreateGuildUpdateTrigger() const;
    
    /**
     * @brief Cria a trigger de inserção de log
     * @return Trigger
     */
    Trigger CreateLogInsertTrigger() const;
    
    // Gerenciador de banco de dados
    std::unique_ptr<IDatabaseManager> manager_;
    
    // Configuração
    DatabaseConfig config_;
    
    // Threads
    std::thread autoBackupThread_;
    std::thread maintenanceThread_;
    std::atomic<bool> running_;
    
    // Estado
    bool initialized_;
    
    // Mutex para sincronização
    std::mutex mutex_;
};

// Acesso global
#define g_DatabaseSystem DatabaseSystem::GetInstance()

} // namespace database
} // namespace reverse
} // namespace wyd

#endif // DATABASESYSTEM_H

} // namespace wydbr
