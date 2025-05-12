/**
 * database_client.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/database/database_client.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYDBR_DATABASE_CLIENT_H
#define WYDBR_DATABASE_CLIENT_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <mutex>
#include <future>
#include <atomic>

#include "../Basedef.h"
#include "../core/event_bus.h"

namespace wydbr {
namespace database {

// Forward declarations
class DatabaseClientImpl;

// Definição dos tipos de resultado
enum /**
 * Classe WYDDatabaseResultType
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseResultType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseResultType {
    SUCCESS,                // Operação bem-sucedida
    ERROR_CONNECTION,       // Erro de conexão
    ERROR_QUERY,            // Erro na consulta
    ERROR_TIMEOUT,          // Timeout na operação
    ERROR_PERMISSION,       // Erro de permissão
    ERROR_CONSTRAINT,       // Violação de restrição
    ERROR_TRANSACTION,      // Erro em transação
    ERROR_SYSTEM,           // Erro do sistema
    ERROR_UNKNOWN           // Erro desconhecido
};

// Códigos de erro detalhados
enum /**
 * Classe WYDDatabaseErrorCode
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseErrorCode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseErrorCode {
    NONE = 0,                      // Sem erro
    CONNECTION_FAILED = 1001,      // Falha na conexão
    CONNECTION_LOST = 1002,        // Conexão perdida
    CONNECTION_TIMEOUT = 1003,     // Timeout na conexão
    
    QUERY_INVALID = 2001,          // Consulta inválida
    QUERY_SYNTAX = 2002,           // Erro de sintaxe na consulta
    QUERY_TIMEOUT = 2003,          // Timeout na consulta
    
    PERMISSION_DENIED = 3001,      // Permissão negada
    PERMISSION_INVALID = 3002,     // Permissão inválida
    
    CONSTRAINT_UNIQUE = 4001,      // Violação de unicidade
    CONSTRAINT_FOREIGN_KEY = 4002, // Violação de chave estrangeira
    CONSTRAINT_CHECK = 4003,       // Violação de restrição de verificação
    CONSTRAINT_NOT_NULL = 4004,    // Violação de não-nulo
    
    TRANSACTION_DEADLOCK = 5001,   // Deadlock em transação
    TRANSACTION_ROLLBACK = 5002,   // Rollback de transação
    TRANSACTION_ISOLATION = 5003,  // Erro de isolamento de transação
    
    SYSTEM_RESOURCE = 6001,        // Erro de recursos do sistema
    SYSTEM_IO = 6002,              // Erro de I/O
    
    UNKNOWN = 9999                 // Erro desconhecido
};

// Estrutura para configuração do cliente de banco de dados
struct DatabaseConfig {
    std::string host;              // Host do banco de dados
    uint16_t port;                 // Porta do banco de dados
    std::string username;          // Nome de usuário
    std::string password;          // Senha
    std::string database;          // Nome do banco de dados
    std::string connectionString;  // String de conexão (alternativa)
    bool useMsSql;                 // Usar MSSQL? (false = MySQL/MariaDB)
    int maxConnections;            // Máximo de conexões no pool
    int connectionTimeout;         // Timeout para conexões (ms)
    int queryTimeout;              // Timeout para consultas (ms)
    bool usePooling;               // Usar pool de conexões?
    bool useCompression;           // Comprimir dados?
    bool usePreparedStatements;    // Usar prepared statements?
    bool logQueries;               // Registrar consultas?
    int reconnectInterval;         // Intervalo para reconexão (ms)
    int maxReconnectAttempts;      // Máximo de tentativas de reconexão
    
    // Construtor com valores padrão
    DatabaseConfig() :
        host("localhost"), port(3306),
        username("wydbr"), password(""),
        database("wydbr"), connectionString(""),
        useMsSql(false), maxConnections(10),
        connectionTimeout(5000), queryTimeout(30000),
        usePooling(true), useCompression(false),
        usePreparedStatements(true), logQueries(false),
        reconnectInterval(5000), maxReconnectAttempts(5) {}
};

// Estrutura para resultados de consulta
/**
 * Classe WYDDatabaseResult
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseResult {
public:
    // Construtor padrã/**
 * DatabaseResult
 * 
 * Implementa a funcionalidade DatabaseResult conforme especificação original.
 * @return Retorna o
 */
o
    DatabaseResult();
    
    // Construtor com tipo de resultado e código de /**
 * DatabaseResult
 * 
 * Implementa a funcionalidade DatabaseResult conforme especificação original.
 * @param type Parâmetro type
 * @param DatabaseErrorCode::NONE Parâmetro DatabaseErrorCode::NONE
 * @return Retorna erro
 */
 erro
    DatabaseResult(DatabaseResultType type, DatabaseErrorCode errorCode = DatabaseErrorCode::NONE);
    
    // Verificações de resultado
    /**
 * isSuccess
 * 
 * Implementa a funcionalidade isSuccess conforme especificação original.
 * @return Retorna bool
 */

    bool isSuccess() const;
    /**
 * isError
 * 
 * Implementa a funcionalidade isError conforme especificação original.
 * @return Retorna bool
 */

    bool isError() const;
    /**
 * isTimeout
 * 
 * Implementa a funcionalidade isTimeout conforme especificação original.
 * @return Retorna bool
 */

    bool isTimeout() const;
    /**
 * isConnectionError
 * 
 * Implementa a funcionalidade isConnectionError conforme especificação original.
 * @return Retorna bool
 */

    bool isConnectionError() const;
    
    // Obtém informações de resultado
    /**
 * getResultType
 * 
 * Implementa a funcionalidade getResultType conforme especificação original.
 * @return Retorna DatabaseResultType
 */

    DatabaseResultType getResultType() const;
    /**
 * getErrorCode
 * 
 * Implementa a funcionalidade getErrorCode conforme especificação original.
 * @return Retorna DatabaseErrorCode
 */

    DatabaseErrorCode getErrorCode() const;
    const std::string& getErrorMessage() const;
    /**
 * getAffectedRows
 * 
 * Implementa a funcionalidade getAffectedRows conforme especificação original.
 * @return Retorna int
 */

    int getAffectedRows() const;
    /**
 * getLastInsertId
 * 
 * Implementa a funcionalidade getLastInsertId conforme especificação original.
 * @return Retorna int
 */

    int getLastInsertId() const;
    
    // Obtém dados da consulta
    /**
 * getRowCount
 * 
 * Implementa a funcionalidade getRowCount conforme especificação original.
 * @return Retorna int
 */

    int getRowCount() const;
    /**
 * getColumnCount
 * 
 * Implementa a funcionalidade getColumnCount conforme especificação original.
 * @return Retorna int
 */

    int getColumnCount() const;
    /**
 * hasRows
 * 
 * Implementa a funcionalidade hasRows conforme especificação original.
 * @return Retorna bool
 */

    bool hasRows() const;
    /**
 * hasColumn
 * 
 * Implementa a funcionalidade hasColumn conforme especificação original.
 * @param columnName Parâmetro columnName
 * @return Retorna bool
 */

    bool hasColumn(const std::string& columnName) const;
    
    // Acesso aos dados
    const std::vector<std::string>& getColumnNames() const;
    /**
 * getString
 * 
 * Implementa a funcionalidade getString conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna std::string
 */

    std::string getString(int row, const std::string& column) const;
    /**
 * getString
 * 
 * Implementa a funcionalidade getString conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna std::string
 */

    std::string getString(int row, int column) const;
    /**
 * getInt
 * 
 * Implementa a funcionalidade getInt conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna int
 */

    int getInt(int row, const std::string& column) const;
    /**
 * getInt
 * 
 * Implementa a funcionalidade getInt conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna int
 */

    int getInt(int row, int column) const;
    /**
 * getFloat
 * 
 * Implementa a funcionalidade getFloat conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna float
 */

    float getFloat(int row, const std::string& column) const;
    /**
 * getFloat
 * 
 * Implementa a funcionalidade getFloat conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna float
 */

    float getFloat(int row, int column) const;
    /**
 * getDouble
 * 
 * Implementa a funcionalidade getDouble conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna double
 */

    double getDouble(int row, const std::string& column) const;
    /**
 * getDouble
 * 
 * Implementa a funcionalidade getDouble conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna double
 */

    double getDouble(int row, int column) const;
    /**
 * getBool
 * 
 * Implementa a funcionalidade getBool conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna bool
 */

    bool getBool(int row, const std::string& column) const;
    /**
 * getBool
 * 
 * Implementa a funcionalidade getBool conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna bool
 */

    bool getBool(int row, int column) const;
    
    // Acesso a dados BLOB
    std::vector<unsigned char> getBlob(int row, const std::string& column) const;
    std::vector<unsigned char> getBlob(int row, int column) const;
    
    // Verificações de tipo
    /**
 * isNull
 * 
 * Implementa a funcionalidade isNull conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna bool
 */

    bool isNull(int row, const std::string& column) const;
    /**
 * isNull
 * 
 * Implementa a funcionalidade isNull conforme especificação original.
 * @param row Parâmetro row
 * @param column Parâmetro column
 * @return Retorna bool
 */

    bool isNull(int row, int column) const;
    
    // Acesso por mapa (key-value)
    std::map<std::string, std::string> getRowAsMap(int row) const;
    
    // Acesso à linha completa (vetor de valores)
    std::vector<std::string> getRow(int row) const;
    
    // Acesso à coluna completa (vetor de valores)
    std::vector<std::string> getColumn(const std::string& column) const;
    std::vector<std::string> getColumn(int column) const;
    
    // Define informações de resultado
    /**
 * setResultType
 * 
 * Implementa a funcionalidade setResultType conforme especificação original.
 * @param type Parâmetro type
 */

    void setResultType(DatabaseResultType type);
    /**
 * setErrorCode
 * 
 * Implementa a funcionalidade setErrorCode conforme especificação original.
 * @param errorCode Parâmetro errorCode
 */

    void setErrorCode(DatabaseErrorCode errorCode);
    /**
 * setErrorMessage
 * 
 * Implementa a funcionalidade setErrorMessage conforme especificação original.
 * @param message Parâmetro message
 */

    void setErrorMessage(const std::string& message);
    /**
 * setAffectedRows
 * 
 * Implementa a funcionalidade setAffectedRows conforme especificação original.
 * @param count Parâmetro count
 */

    void setAffectedRows(int count);
    /**
 * setLastInsertId
 * 
 * Implementa a funcionalidade setLastInsertId conforme especificação original.
 * @param id Parâmetro id
 */

    void setLastInsertId(int id);
    
    // Adiciona dados à consulta
    /**
 * addRow
 * 
 * Implementa a funcionalidade addRow conforme especificação original.
 * @param values Parâmetro values
 */

    void addRow(const std::vector<std::string>& values);
    /**
 * setColumnNames
 * 
 * Implementa a funcionalidade setColumnNames conforme especificação original.
 * @param names Parâmetro names
 */

    void setColumnNames(const std::vector<std::string>& names);
    
    // Limpa o resultado
    /**
 * clear
 * 
 * Implementa a funcionalidade clear conforme especificação original.
 */

    void clear();

private:
    DatabaseResultType resultType_;          // Tipo de resultado
    DatabaseErrorCode errorCode_;            // Código de erro
    std::string errorMessage_;               // Mensagem de erro
    int affectedRows_;                      // Linhas afetadas
    int lastInsertId_;                      // Último ID inserido
    
    std::vector<std::string> columnNames_;   // Nomes das colunas
    std::vector<std::vector<std::string>> rows_; // Linhas de resultados
    std::map<std::string, int> columnMap_;   // Mapa de índices de coluna
};

// Tipos de callback
using QueryCallback = std::function<void(const DatabaseResult&)>;
using ConnectCallback = std::function<void(bool success)>;
using DisconnectCallback = std::function<void(const std::string& reason)>;
using ErrorCallback = std::function<void(DatabaseErrorCode code, const std::string& message)>;

/**
 * @class DatabaseClient
 * @brief Cliente para interação com o banco de dados do WYDBR Server
 * 
 * Esta classe fornece uma interface unificada para operações de banco de dados,
 * abstraindo detalhes de conexão e oferecendo recursos avançados como pool de conexões,
 * prepared statements, transações e operações assíncronas.
 */
/**
 * Classe WYDDatabaseClient
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseClient conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseClient {
public:
    /**
     * @brief Construtor
     * @param name Nome identificador para este cliente
     * @param config Configuração do cliente de banco de dados
     */
    DatabaseClient(const std::string& name, const DatabaseConfig& config);
    
    /**
     * @brief Destrutor
     */
    ~DatabaseClient();
    
    /**
     * @brief Inicializa o cliente de banco de dados
     * @return true se inicializado com sucesso
     */
    bool init();
    
    /**
     * @brief Para o cliente de banco de dados e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Conecta ao banco de dados
     * @return true se conectado com sucesso
     */
    bool connect();
    
    /**
     * @brief Conecta ao banco de dados de forma assíncrona
     * @param callback Função de callback para notificação de conexão
     */
    void connectAsync(const ConnectCallback& callback);
    
    /**
     * @brief Desconecta do banco de dados
     */
    void disconnect();
    
    /**
     * @brief Executa uma consulta SQL
     * @param query Consulta SQL
     * @return Resultado da consulta
     */
    DatabaseResult executeQuery(const std::string& query);
    
    /**
     * @brief Executa uma consulta SQL de forma assíncrona
     * @param query Consulta SQL
     * @param callback Função de callback para notificação de resultado
     * @return ID da operação assíncrona
     */
    int executeQueryAsync(const std::string& query, const QueryCallback& callback);
    
    /**
     * @brief Executa uma consulta SQL com parâmetros (prepared statement)
     * @param query Consulta SQL com placeholders (?/@ para parâmetros)
     * @param params Parâmetros para a consulta
     * @return Resultado da consulta
     */
    DatabaseResult executeQueryParams(const std::string& query, const std::vector<std::string>& params);
    
    /**
     * @brief Executa uma consulta SQL com parâmetros de forma assíncrona
     * @param query Consulta SQL com placeholders
     * @param params Parâmetros para a consulta
     * @param callback Função de callback para notificação de resultado
     * @return ID da operação assíncrona
     */
    int executeQueryParamsAsync(const std::string& query, const std::vector<std::string>& params, 
                                const QueryCallback& callback);
    
    /**
     * @brief Executa várias consultas em uma transação
     * @param queries Vetor de consultas SQL
     * @return Resultado da transação
     */
    DatabaseResult executeTransaction(const std::vector<std::string>& queries);
    
    /**
     * @brief Executa várias consultas em uma transação de forma assíncrona
     * @param queries Vetor de consultas SQL
     * @param callback Função de callback para notificação de resultado
     * @return ID da operação assíncrona
     */
    int executeTransactionAsync(const std::vector<std::string>& queries, const QueryCallback& callback);
    
    /**
     * @brief Inicia uma transação
     * @return true se a transação foi iniciada com sucesso
     */
    bool beginTransaction();
    
    /**
     * @brief Confirma uma transação
     * @return true se a transação foi confirmada com sucesso
     */
    bool commitTransaction();
    
    /**
     * @brief Cancela uma transação
     * @return true se a transação foi cancelada com sucesso
     */
    bool rollbackTransaction();
    
    /**
     * @brief Escapa uma string para uso em consultas SQL
     * @param str String a ser escapada
     * @return String escapada
     */
    std::string escapeString(const std::string& str);
    
    /**
     * @brief Verifica se está conectado ao banco de dados
     * @return true se estiver conectado
     */
    bool isConnected() const;
    
    /**
     * @brief Verifica se uma operação assíncrona está em andamento
     * @param operationId ID da operação
     * @return true se a operação estiver em andamento
     */
    bool isOperationPending(int operationId) const;
    
    /**
     * @brief Cancela uma operação assíncrona
     * @param operationId ID da operação
     * @return true se a operação foi cancelada
     */
    bool cancelOperation(int operationId);
    
    /**
     * @brief Obtém estatísticas de uso
     * @return Mapa de estatísticas
     */
    std::map<std::string, int64_t> getStats() const;
    
    /**
     * @brief Define callback para eventos de conexão
     * @param callback Função de callback
     */
    void setConnectCallback(const ConnectCallback& callback);
    
    /**
     * @brief Define callback para eventos de desconexão
     * @param callback Função de callback
     */
    void setDisconnectCallback(const DisconnectCallback& callback);
    
    /**
     * @brief Define callback para eventos de erro
     * @param callback Função de callback
     */
    void setErrorCallback(const ErrorCallback& callback);
    
    /**
     * @brief Obtém o nome deste cliente
     * @return Nome do cliente
     */
    const std::string& getName() const;
    
    /**
     * @brief Verifica se o cliente está inicializado
     * @return true se inicializado
     */
    bool isInitialized() const;
    
    /**
     * @brief Obtém o host do banco de dados
     * @return Host do banco de dados
     */
    const std::string& getHost() const;
    
    /**
     * @brief Obtém a porta do banco de dados
     * @return Porta do banco de dados
     */
    uint16_t getPort() const;
    
    /**
     * @brief Obtém o nome do banco de dados
     * @return Nome do banco de dados
     */
    const std::string& getDatabase() const;
    
    /**
     * @brief Obtém o nome de usuário
     * @return Nome de usuário
     */
    const std::string& getUsername() const;
    
    /**
     * @brief Obtém a configuração atual do cliente
     * @return Configuração atual
     */
    const DatabaseConfig& getConfig() const;
    
    /**
     * @brief Ativa o modo de debug (logs detalhados)
     * @param enable true para ativar, false para desativar
     */
    void setDebugMode(bool enable);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se o modo de debug estiver ativado
     */
    bool isDebugMode() const;
    
    /**
     * @brief Aguarda a conclusão de todas as operações pendentes
     * @param timeoutMs Timeout em milissegundos (0 = sem timeout)
     * @return true se todas as operações foram concluídas
     */
    bool waitForPendingOperations(int timeoutMs = 0);
    
    /**
     * @brief Obtém o número de operações pendentes
     * @return Número de operações pendentes
     */
    int getPendingOperationsCount() const;
    
    /**
     * @brief Reconfigura o cliente de banco de dados
     * @param config Nova configuração
     * @return true se a reconfiguração foi bem-sucedida
     */
    bool reconfigure(const DatabaseConfig& config);
    
    /**
     * @brief Verifica a conexão com o banco de dados
     * @return true se a conexão estiver OK
     */
    bool ping();
    
    /**
     * @brief Reconecta ao banco de dados
     * @return true se reconectado com sucesso
     */
    bool reconnect();

private:
    // Implementação (usando pimpl idiom)
    std::unique_ptr<DatabaseClientImpl> impl_;
};

/**
 * @brief Converte um código de erro para string
 * @param code Código de erro
 * @return String representando o código de erro
 */
std::string errorCodeToString(DatabaseErrorCode code);

/**
 * @brief Converte um tipo de resultado para string
 * @param type Tipo de resultado
 * @return String representando o tipo de resultado
 */
std::string resultTypeToString(DatabaseResultType type);

} // namespace database
} // namespace wydbr

#endif // WYDBR_DATABASE_CLIENT_H