#ifndef WYDSTUDIO_DATABASE_MANAGER_H
#define WYDSTUDIO_DATABASE_MANAGER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <variant>
#include <chrono>
#include <future>
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace database {

/**
 * @brief Tipo de banco de dados
 */
enum class DatabaseType {
    MYSQL,
    POSTGRESQL,
    SQLITE,
    MSSQL,
    ORACLE,
    CUSTOM
};

/**
 * @brief Tipo de transação
 */
enum class TransactionIsolation {
    READ_UNCOMMITTED,
    READ_COMMITTED,
    REPEATABLE_READ,
    SERIALIZABLE
};

/**
 * @brief Valores de parâmetros para consultas SQL
 */
using QueryParameter = std::variant<
    std::monostate,           // NULL
    int32_t,                  // Integer
    uint32_t,                 // Unsigned integer
    int64_t,                  // Bigint
    uint64_t,                 // Unsigned bigint
    float,                    // Float
    double,                   // Double
    bool,                     // Boolean
    std::string,              // String/Text
    std::vector<uint8_t>,     // Binary data
    std::chrono::system_clock::time_point  // Timestamp
>;

/**
 * @brief Representação de uma coluna em um conjunto de resultados
 */
struct ResultColumn {
    std::string name;
    enum class Type {
        NULL_TYPE,
        INTEGER,
        UNSIGNED_INTEGER,
        BIGINT,
        UNSIGNED_BIGINT,
        FLOAT,
        DOUBLE,
        BOOLEAN,
        TEXT,
        BINARY,
        TIMESTAMP,
        DATE,
        TIME,
        GUID,
        JSON,
        CUSTOM
    } type;
    bool isNullable;
    
    ResultColumn() : type(Type::NULL_TYPE), isNullable(true) {}
    ResultColumn(const std::string& colName, Type colType, bool nullable)
        : name(colName), type(colType), isNullable(nullable) {}
};

/**
 * @brief Campo em um conjunto de resultados
 */
struct ResultField {
    QueryParameter value;
    bool isNull;
    
    ResultField() : isNull(true) {}
    
    template<typename T>
    ResultField(const T& val) : value(val), isNull(false) {}
    
    static ResultField createNull() {
        ResultField field;
        field.isNull = true;
        return field;
    }
    
    template<typename T>
    std::optional<T> getValue() const {
        if (isNull) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    std::string toString() const;
};

/**
 * @brief Linha em um conjunto de resultados
 */
class ResultRow {
public:
    ResultRow() = default;
    ResultRow(const std::vector<ResultField>& fields, 
             const std::vector<ResultColumn>& columns);
    
    // Acesso baseado em índice
    const ResultField& operator[](size_t index) const;
    
    // Acesso baseado em nome de coluna
    const ResultField& operator[](const std::string& columnName) const;
    
    // Métodos auxiliares
    bool hasColumn(const std::string& columnName) const;
    size_t getColumnCount() const;
    std::vector<std::string> getColumnNames() const;
    
    // Iteração
    using const_iterator = std::vector<ResultField>::const_iterator;
    const_iterator begin() const;
    const_iterator end() const;
    
    // Conversões com segurança de tipo
    template<typename T>
    std::optional<T> get(size_t index) const {
        if (index >= m_fields.size()) {
            return std::nullopt;
        }
        return m_fields[index].getValue<T>();
    }
    
    template<typename T>
    std::optional<T> get(const std::string& columnName) const {
        auto it = m_columnMap.find(columnName);
        if (it == m_columnMap.end()) {
            return std::nullopt;
        }
        return m_fields[it->second].getValue<T>();
    }
    
    template<typename T>
    T getOr(const std::string& columnName, const T& defaultValue) const {
        auto value = get<T>(columnName);
        return value.has_value() ? *value : defaultValue;
    }
    
    // Conversões para tipos específicos
    std::optional<int32_t> getInt(const std::string& columnName) const;
    std::optional<uint32_t> getUInt(const std::string& columnName) const;
    std::optional<int64_t> getBigInt(const std::string& columnName) const;
    std::optional<float> getFloat(const std::string& columnName) const;
    std::optional<double> getDouble(const std::string& columnName) const;
    std::optional<bool> getBool(const std::string& columnName) const;
    std::optional<std::string> getString(const std::string& columnName) const;
    std::optional<std::vector<uint8_t>> getBinary(const std::string& columnName) const;
    std::optional<std::chrono::system_clock::time_point> getTimestamp(const std::string& columnName) const;
    
    // Verificadores de nulo
    bool isNull(size_t index) const;
    bool isNull(const std::string& columnName) const;
    
private:
    std::vector<ResultField> m_fields;
    std::vector<ResultColumn> m_columns;
    std::unordered_map<std::string, size_t> m_columnMap;
};

/**
 * @brief Conjunto de resultados de uma consulta SQL
 */
class ResultSet {
public:
    ResultSet() = default;
    ResultSet(std::vector<ResultRow> rows, std::vector<ResultColumn> columns);
    
    // Acessadores
    const ResultRow& operator[](size_t index) const;
    size_t getRowCount() const;
    size_t getColumnCount() const;
    const std::vector<ResultColumn>& getColumns() const;
    
    // Verificadores
    bool isEmpty() const;
    bool hasColumn(const std::string& columnName) const;
    
    // Iteração
    using const_iterator = std::vector<ResultRow>::const_iterator;
    const_iterator begin() const;
    const_iterator end() const;
    
    // Facilidades
    ResultRow getFirstRow() const;
    std::optional<ResultField> getFirstRowField(const std::string& columnName) const;
    
    // Conversões
    template<typename T>
    std::vector<T> getColumn(const std::string& columnName) const {
        std::vector<T> result;
        for (const auto& row : m_rows) {
            auto value = row.get<T>(columnName);
            if (value.has_value()) {
                result.push_back(*value);
            } else {
                // O que fazer em caso de erro? Atualmente pula valores inválidos
            }
        }
        return result;
    }
    
    template<typename KeyType, typename ValueType>
    std::unordered_map<KeyType, ValueType> toMap(
        const std::string& keyColumn, 
        const std::string& valueColumn) const {
        
        std::unordered_map<KeyType, ValueType> result;
        for (const auto& row : m_rows) {
            auto key = row.get<KeyType>(keyColumn);
            auto value = row.get<ValueType>(valueColumn);
            
            if (key.has_value() && value.has_value()) {
                result[*key] = *value;
            }
        }
        return result;
    }
    
    // Exportação
    std::string toCSV(bool includeHeaders = true, char delimiter = ',') const;
    std::string toJSON() const;
    
private:
    std::vector<ResultRow> m_rows;
    std::vector<ResultColumn> m_columns;
    std::unordered_map<std::string, size_t> m_columnMap;
};

/**
 * @brief Configuração de conexão com banco de dados
 */
struct DatabaseConfig {
    DatabaseType type;
    std::string host;
    uint16_t port;
    std::string database;
    std::string username;
    std::string password;
    std::string connectionString;
    
    // Opções adicionais
    std::unordered_map<std::string, std::string> options;
    
    // Opções de conexão
    uint32_t connectionTimeout;       // em segundos
    uint32_t commandTimeout;          // em segundos
    uint32_t connectionPoolSize;      // número máximo de conexões
    bool useSSL;                      // usar conexão segura
    bool persistSecurityInfo;         // manter informações sensíveis
    
    // Comportamento em caso de erro
    bool reconnectOnFailure;
    uint32_t maxReconnectAttempts;
    uint32_t reconnectDelay;          // em milissegundos
    
    DatabaseConfig() : type(DatabaseType::MYSQL), port(0), 
                     connectionTimeout(30), commandTimeout(30),
                     connectionPoolSize(10), useSSL(false),
                     persistSecurityInfo(false), reconnectOnFailure(true),
                     maxReconnectAttempts(3), reconnectDelay(1000) {}
    
    static DatabaseConfig createMySQLConfig(
        const std::string& host, uint16_t port, 
        const std::string& database, 
        const std::string& username, 
        const std::string& password) {
        
        DatabaseConfig config;
        config.type = DatabaseType::MYSQL;
        config.host = host;
        config.port = port;
        config.database = database;
        config.username = username;
        config.password = password;
        return config;
    }
    
    static DatabaseConfig createPostgreSQLConfig(
        const std::string& host, uint16_t port, 
        const std::string& database, 
        const std::string& username, 
        const std::string& password) {
        
        DatabaseConfig config;
        config.type = DatabaseType::POSTGRESQL;
        config.host = host;
        config.port = port;
        config.database = database;
        config.username = username;
        config.password = password;
        return config;
    }
    
    static DatabaseConfig createSQLiteConfig(const std::string& filePath) {
        DatabaseConfig config;
        config.type = DatabaseType::SQLITE;
        config.database = filePath;
        return config;
    }
};

/**
 * @brief Opções para execução de consultas
 */
struct QueryOptions {
    uint32_t timeout;                    // tempo limite em segundos
    bool useTransaction;                 // usar transação
    TransactionIsolation isolation;      // nível de isolamento
    bool readOnly;                       // consulta somente leitura
    
    QueryOptions() : timeout(30), useTransaction(false),
                   isolation(TransactionIsolation::READ_COMMITTED),
                   readOnly(false) {}
};

/**
 * @brief Resultado de uma operação de banco de dados
 */
struct QueryResult {
    bool success;
    std::string errorMessage;
    uint32_t affectedRows;
    uint64_t lastInsertId;
    uint32_t executionTimeMs;
    
    QueryResult() : success(true), affectedRows(0), lastInsertId(0), executionTimeMs(0) {}
    
    static QueryResult createError(const std::string& error) {
        QueryResult result;
        result.success = false;
        result.errorMessage = error;
        return result;
    }
    
    static QueryResult createSuccess(uint32_t affected, uint64_t insertId) {
        QueryResult result;
        result.success = true;
        result.affectedRows = affected;
        result.lastInsertId = insertId;
        return result;
    }
};

/**
 * @brief Transação de banco de dados
 */
class Transaction {
public:
    Transaction(TransactionIsolation isolation = TransactionIsolation::READ_COMMITTED);
    virtual ~Transaction();
    
    virtual bool begin() = 0;
    virtual bool commit() = 0;
    virtual bool rollback() = 0;
    
    bool isActive() const;
    TransactionIsolation getIsolationLevel() const;
    
protected:
    bool m_active;
    TransactionIsolation m_isolation;
};

/**
 * @brief Conexão com banco de dados
 */
class DatabaseConnection {
public:
    DatabaseConnection(const DatabaseConfig& config);
    virtual ~DatabaseConnection();
    
    // Estado da conexão
    virtual bool isConnected() const = 0;
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual std::string getLastError() const = 0;
    
    // Execução de consultas
    virtual QueryResult executeNonQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) = 0;
    
    virtual std::optional<ResultSet> executeQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) = 0;
    
    virtual std::optional<ResultField> executeScalar(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) = 0;
    
    // Transações
    virtual std::unique_ptr<Transaction> beginTransaction(
        TransactionIsolation isolation = TransactionIsolation::READ_COMMITTED) = 0;
    
    // Informações da conexão
    const DatabaseConfig& getConfig() const;
    DatabaseType getType() const;
    std::string getDatabaseName() const;
    
protected:
    DatabaseConfig m_config;
    std::string m_lastError;
};

/**
 * @brief Gerenciador de banco de dados
 */
class DatabaseManager {
public:
    // Singleton
    static DatabaseManager& getInstance();
    
    // Configuração
    bool initialize();
    bool shutdown();
    bool isInitialized() const { return m_initialized; }
    
    // Gerenciamento de conexões
    bool registerConnection(const std::string& name, const DatabaseConfig& config);
    bool removeConnection(const std::string& name);
    std::shared_ptr<DatabaseConnection> getConnection(const std::string& name);
    bool setDefaultConnection(const std::string& name);
    std::shared_ptr<DatabaseConnection> getDefaultConnection();
    
    // Helpers para acesso
    QueryResult executeNonQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const std::string& connectionName = "");
    
    std::optional<ResultSet> executeQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const std::string& connectionName = "");
    
    std::optional<ResultField> executeScalar(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const std::string& connectionName = "");
    
    // Execução assíncrona
    std::future<QueryResult> executeNonQueryAsync(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const std::string& connectionName = "");
    
    std::future<std::optional<ResultSet>> executeQueryAsync(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const std::string& connectionName = "");
    
    // Gerenciamento de transações
    std::unique_ptr<Transaction> beginTransaction(
        const std::string& connectionName = "",
        TransactionIsolation isolation = TransactionIsolation::READ_COMMITTED);
    
    // Informações
    std::vector<std::string> getRegisteredConnectionNames() const;
    bool hasConnection(const std::string& name) const;
    std::string getDefaultConnectionName() const;
    
    // Monitoramento e diagnóstico
    struct ConnectionStatistics {
        uint32_t openConnections;
        uint32_t totalQueries;
        uint32_t successfulQueries;
        uint32_t failedQueries;
        uint32_t totalTransactions;
        uint32_t activeTransactions;
        uint32_t averageQueryTimeMs;
        uint32_t peakQueryTimeMs;
    };
    
    ConnectionStatistics getStatistics(const std::string& connectionName = "") const;
    void resetStatistics(const std::string& connectionName = "");
    
    // Eventos
    core::EventBus::SubscriptionID subscribeToQueryEvents(
        std::function<void(const std::string&, const std::string&, uint32_t, bool)> callback);
    
private:
    DatabaseManager();
    ~DatabaseManager();
    
    // Prevenção de cópia
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    bool m_initialized;
    std::unordered_map<std::string, std::shared_ptr<DatabaseConnection>> m_connections;
    std::string m_defaultConnectionName;
    
    // Estatísticas
    std::unordered_map<std::string, ConnectionStatistics> m_statistics;
    
    // Multitarefa
    std::shared_ptr<core::TaskScheduler> m_taskScheduler;
    
    // Eventos
    std::shared_ptr<core::EventBus> m_eventBus;
    
    // Métodos auxiliares
    std::shared_ptr<DatabaseConnection> getConnectionOrDefault(const std::string& name);
    void updateStatistics(const std::string& connectionName, bool success, uint32_t timeMs);
};

/**
 * @brief Implementação de transação para MySQL
 */
class MySQLTransaction : public Transaction {
public:
    MySQLTransaction(std::shared_ptr<DatabaseConnection> connection,
                   TransactionIsolation isolation = TransactionIsolation::READ_COMMITTED);
    ~MySQLTransaction() override;
    
    bool begin() override;
    bool commit() override;
    bool rollback() override;
    
private:
    std::shared_ptr<DatabaseConnection> m_connection;
};

/**
 * @brief Implementação de conexão para MySQL
 */
class MySQLConnection : public DatabaseConnection {
public:
    MySQLConnection(const DatabaseConfig& config);
    ~MySQLConnection() override;
    
    bool isConnected() const override;
    bool connect() override;
    bool disconnect() override;
    std::string getLastError() const override;
    
    QueryResult executeNonQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) override;
    
    std::optional<ResultSet> executeQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) override;
    
    std::optional<ResultField> executeScalar(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) override;
    
    std::unique_ptr<Transaction> beginTransaction(
        TransactionIsolation isolation = TransactionIsolation::READ_COMMITTED) override;
    
private:
    void* m_mysqlHandle;  // MYSQL*
    bool m_connected;
    
    // Métodos auxiliares
    bool prepareStatement(const std::string& sql, const std::vector<QueryParameter>& parameters, void*& stmt);
    void bindParameters(void* stmt, const std::vector<QueryParameter>& parameters);
    QueryResult processResult(void* stmt);
    std::optional<ResultSet> processQueryResult(void* stmt);
};

/**
 * @brief Implementação de transação para PostgreSQL
 */
class PostgreSQLTransaction : public Transaction {
public:
    PostgreSQLTransaction(std::shared_ptr<DatabaseConnection> connection,
                        TransactionIsolation isolation = TransactionIsolation::READ_COMMITTED);
    ~PostgreSQLTransaction() override;
    
    bool begin() override;
    bool commit() override;
    bool rollback() override;
    
private:
    std::shared_ptr<DatabaseConnection> m_connection;
};

/**
 * @brief Implementação de conexão para PostgreSQL
 */
class PostgreSQLConnection : public DatabaseConnection {
public:
    PostgreSQLConnection(const DatabaseConfig& config);
    ~PostgreSQLConnection() override;
    
    bool isConnected() const override;
    bool connect() override;
    bool disconnect() override;
    std::string getLastError() const override;
    
    QueryResult executeNonQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) override;
    
    std::optional<ResultSet> executeQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) override;
    
    std::optional<ResultField> executeScalar(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) override;
    
    std::unique_ptr<Transaction> beginTransaction(
        TransactionIsolation isolation = TransactionIsolation::READ_COMMITTED) override;
    
private:
    void* m_pgConn;  // PGconn*
    bool m_connected;
    
    // Métodos auxiliares
    bool prepareParameters(const std::vector<QueryParameter>& parameters, 
                          std::vector<const char*>& paramValues,
                          std::vector<int>& paramLengths,
                          std::vector<int>& paramFormats);
    QueryResult processResult(void* result);
    std::optional<ResultSet> processQueryResult(void* result);
};

/**
 * @brief Implementação de transação para SQLite
 */
class SQLiteTransaction : public Transaction {
public:
    SQLiteTransaction(std::shared_ptr<DatabaseConnection> connection,
                    TransactionIsolation isolation = TransactionIsolation::READ_COMMITTED);
    ~SQLiteTransaction() override;
    
    bool begin() override;
    bool commit() override;
    bool rollback() override;
    
private:
    std::shared_ptr<DatabaseConnection> m_connection;
};

/**
 * @brief Implementação de conexão para SQLite
 */
class SQLiteConnection : public DatabaseConnection {
public:
    SQLiteConnection(const DatabaseConfig& config);
    ~SQLiteConnection() override;
    
    bool isConnected() const override;
    bool connect() override;
    bool disconnect() override;
    std::string getLastError() const override;
    
    QueryResult executeNonQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) override;
    
    std::optional<ResultSet> executeQuery(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) override;
    
    std::optional<ResultField> executeScalar(
        const std::string& sql, 
        const std::vector<QueryParameter>& parameters = {},
        const QueryOptions& options = QueryOptions()) override;
    
    std::unique_ptr<Transaction> beginTransaction(
        TransactionIsolation isolation = TransactionIsolation::READ_COMMITTED) override;
    
private:
    void* m_dbHandle;  // sqlite3*
    bool m_connected;
    
    // Métodos auxiliares
    bool prepareStatement(const std::string& sql, void*& stmt);
    bool bindParameters(void* stmt, const std::vector<QueryParameter>& parameters);
    QueryResult processResult(void* stmt);
    std::optional<ResultSet> processQueryResult(void* stmt);
};

/**
 * @brief Construtor de consultas SQL
 */
class QueryBuilder {
public:
    enum class JoinType {
        INNER,
        LEFT,
        RIGHT,
        FULL
    };
    
    QueryBuilder();
    
    // Construção da consulta
    QueryBuilder& select(const std::string& columns = "*");
    QueryBuilder& from(const std::string& table);
    QueryBuilder& join(const std::string& table, const std::string& condition, JoinType type = JoinType::INNER);
    QueryBuilder& leftJoin(const std::string& table, const std::string& condition);
    QueryBuilder& rightJoin(const std::string& table, const std::string& condition);
    QueryBuilder& where(const std::string& condition);
    QueryBuilder& andWhere(const std::string& condition);
    QueryBuilder& orWhere(const std::string& condition);
    QueryBuilder& groupBy(const std::string& columns);
    QueryBuilder& having(const std::string& condition);
    QueryBuilder& orderBy(const std::string& columns);
    QueryBuilder& limit(uint32_t count);
    QueryBuilder& offset(uint32_t count);
    
    // Consultas de manipulação
    QueryBuilder& insert(const std::string& table);
    QueryBuilder& values(const std::vector<std::string>& columns, const std::vector<QueryParameter>& params);
    QueryBuilder& update(const std::string& table);
    QueryBuilder& set(const std::string& column, const QueryParameter& value);
    QueryBuilder& set(const std::unordered_map<std::string, QueryParameter>& values);
    QueryBuilder& deleteFrom(const std::string& table);
    
    // Subconsultas
    QueryBuilder& subquery(const QueryBuilder& subquery, const std::string& alias);
    
    // Parâmetros
    QueryBuilder& addParameter(const QueryParameter& param);
    
    // Construção da SQL
    std::string getSql() const;
    std::vector<QueryParameter> getParameters() const;
    
    // Execução direta
    QueryResult execute(const std::string& connectionName = "");
    std::optional<ResultSet> executeQuery(const std::string& connectionName = "");
    std::optional<ResultField> executeScalar(const std::string& connectionName = "");
    
    // Reset
    void clear();
    
private:
    enum class QueryType {
        SELECT,
        INSERT,
        UPDATE,
        DELETE
    };
    
    QueryType m_type;
    std::string m_select;
    std::string m_from;
    std::vector<std::string> m_joins;
    std::vector<std::string> m_wheres;
    std::string m_groupBy;
    std::string m_having;
    std::string m_orderBy;
    std::string m_limit;
    std::string m_offset;
    
    std::string m_insertTable;
    std::vector<std::string> m_insertColumns;
    std::string m_updateTable;
    std::vector<std::string> m_sets;
    std::string m_deleteTable;
    
    std::vector<QueryParameter> m_parameters;
};

/**
 * @brief Mapeador objeto-relacional
 */
class OrmManager {
public:
    // Singleton
    static OrmManager& getInstance();
    
    // Inicialização
    bool initialize();
    bool shutdown();
    bool isInitialized() const { return m_initialized; }
    
    // Definição de mapeamento
    struct EntityMapping {
        std::string entityName;
        std::string tableName;
        std::string primaryKeyField;
        std::string primaryKeyColumn;
        bool autoIncrement;
        
        struct FieldMapping {
            std::string fieldName;
            std::string columnName;
            bool isRequired;
            bool isReadOnly;
            std::string defaultValue;
            
            FieldMapping() : isRequired(true), isReadOnly(false) {}
        };
        
        std::vector<FieldMapping> fields;
        
        EntityMapping() : autoIncrement(true) {}
    };
    
    bool registerEntityMapping(const EntityMapping& mapping);
    bool unregisterEntityMapping(const std::string& entityName);
    std::optional<EntityMapping> getEntityMapping(const std::string& entityName) const;
    
    // Operações CRUD
    template<typename T>
    std::vector<T> findAll(const std::string& tableName = "",
                         const std::string& where = "",
                         const std::vector<QueryParameter>& parameters = {},
                         const std::string& connectionName = "") {
        // Implementação específica para cada tipo será fornecida nas especializações
        return std::vector<T>();
    }
    
    template<typename T>
    std::optional<T> findById(const QueryParameter& id,
                           const std::string& tableName = "",
                           const std::string& connectionName = "") {
        // Implementação específica para cada tipo será fornecida nas especializações
        return std::nullopt;
    }
    
    template<typename T>
    bool insert(T& entity,
              const std::string& tableName = "",
              const std::string& connectionName = "") {
        // Implementação específica para cada tipo será fornecida nas especializações
        return false;
    }
    
    template<typename T>
    bool update(const T& entity,
              const std::string& tableName = "",
              const std::string& connectionName = "") {
        // Implementação específica para cada tipo será fornecida nas especializações
        return false;
    }
    
    template<typename T>
    bool remove(const T& entity,
              const std::string& tableName = "",
              const std::string& connectionName = "") {
        // Implementação específica para cada tipo será fornecida nas especializações
        return false;
    }
    
    bool removeById(const std::string& entityName,
                  const QueryParameter& id,
                  const std::string& tableName = "",
                  const std::string& connectionName = "");
    
    // Facilidades para consulta
    QueryBuilder createQuery(const std::string& entityName = "");
    
private:
    OrmManager();
    ~OrmManager();
    
    // Prevenção de cópia
    OrmManager(const OrmManager&) = delete;
    OrmManager& operator=(const OrmManager&) = delete;
    
    bool m_initialized;
    std::unordered_map<std::string, EntityMapping> m_entityMappings;
    
    // Referência ao gerenciador de banco de dados
    DatabaseManager& m_dbManager;
};

} // namespace database
} // namespace wydstudio

#endif // WYDSTUDIO_DATABASE_MANAGER_H