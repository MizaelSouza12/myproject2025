/**
 * DatabaseClient.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/database/DatabaseClient.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYDBR_DATABASE_CLIENT_H
#define WYDBR_DATABASE_CLIENT_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <future>
#include <functional>
#include <any>
#include <chrono>

namespace wydbr {
namespace database {

/**
 * @enum TransactionIsolationLevel
 * @brief Níveis de isolamento para transações
 */
enum /**
 * Classe WYDTransactionIsolationLevel
 * 
 * Esta classe implementa a funcionalidade WYDTransactionIsolationLevel conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TransactionIsolationLevel {
    READ_UNCOMMITTED,  ///< Permite leitura de dados não confirmados (mais rápido, menos seguro)
    READ_COMMITTED,    ///< Permite leitura apenas de dados confirmados
    REPEATABLE_READ,   ///< Garante que leituras repetidas retornem o mesmo resultado
    SERIALIZABLE       ///< Garante que transações em série sejam equivalentes a transações concorrentes
};

/**
 * @struct DatabaseStats
 * @brief Estatísticas do banco de dados
 */
struct DatabaseStats {
    uint64_t totalQueries;         ///< Total de consultas executadas
    uint64_t totalTransactions;    ///< Total de transações executadas
    uint64_t activeTransactions;   ///< Transações ativas
    uint64_t failedTransactions;   ///< Transações que falharam
    uint64_t totalRowsRead;        ///< Total de linhas lidas
    uint64_t totalRowsWritten;     ///< Total de linhas escritas
    uint64_t totalErrors;          ///< Total de erros
    uint64_t cacheHits;            ///< Hits no cache
    uint64_t cacheMisses;          ///< Misses no cache
    double averageQueryTime;       ///< Tempo médio de consulta em ms
    double averageTransactionTime; ///< Tempo médio de transação em ms
    std::chrono::system_clock::time_point lastBackupTime; ///< Timestamp do último backup
};

/**
 * @struct QueryResult
 * @brief Resultado de uma consulta SQL
 */
struct QueryResult {
    bool success;                   ///< Se a consulta foi bem-sucedida
    std::string error;              ///< Mensagem de erro (se houver)
    uint64_t rowsAffected;          ///< Número de linhas afetadas
    uint64_t insertId;              ///< ID de inserção (se aplicável)
    std::vector<std::string> columns;///< Nomes das colunas
    std::vector<std::vector<std::any>> rows; ///< Linhas de resultado
    double executionTime;           ///< Tempo de execução em ms
};

/**
 * @class DatabaseClient
 * @brief Interface para cliente de banco de dados
 * 
 * Fornece uma interface comum para diferentes implementações de banco de dados,
 * como PostgreSQL, MySQL, SQLite, etc.
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
     * @brief Tipo de callback para resultados de consulta
     */
    using QueryCallback = std::function<void(const QueryResult&)>;
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~DatabaseClient() = default;
    
    /**
     * @brief Conecta ao banco de dados
     * @param connectionString String de conexão
     * @return true se a conexão foi bem-sucedida
     */
    virtual bool connect(const std::string& connectionString) = 0;
    
    /**
     * @brief Desconecta do banco de dados
     */
    virtual void disconnect() = 0;
    
    /**
     * @brief Verifica se está conectado
     * @return true se estiver conectado
     */
    virtual bool isConnected() const = 0;
    
    /**
     * @brief Inicia uma transação
     * @param isolationLevel Nível de isolamento da transação
     * @return ID da transação ou string vazia se falhou
     */
    virtual std::string beginTransaction(TransactionIsolationLevel isolationLevel = TransactionIsolationLevel::READ_COMMITTED) = 0;
    
    /**
     * @brief Confirma uma transação
     * @param transactionId ID da transação
     * @return true se a transação foi confirmada com sucesso
     */
    virtual bool commitTransaction(const std::string& transactionId) = 0;
    
    /**
     * @brief Reverte uma transação
     * @param transactionId ID da transação
     * @return true se a transação foi revertida com sucesso
     */
    virtual bool rollbackTransaction(const std::string& transactionId) = 0;
    
    /**
     * @brief Executa uma consulta SQL
     * @param query Consulta SQL
     * @param params Parâmetros para a consulta
     * @return Resultado da consulta
     */
    virtual QueryResult executeQuery(const std::string& query, const std::vector<std::any>& params = {}) = 0;
    
    /**
     * @brief Executa uma consulta SQL assincronamente
     * @param query Consulta SQL
     * @param params Parâmetros para a consulta
     * @param callback Função de callback para o resultado
     */
    virtual void executeQueryAsync(const std::string& query, const std::vector<std::any>& params, QueryCallback callback) = 0;
    
    /**
     * @brief Executa uma consulta SQL como parte de uma transação
     * @param transactionId ID da transação
     * @param query Consulta SQL
     * @param params Parâmetros para a consulta
     * @return Resultado da consulta
     */
    virtual QueryResult executeTransactionQuery(const std::string& transactionId, const std::string& query, const std::vector<std::any>& params = {}) = 0;
    
    /**
     * @brief Obtém estatísticas do banco de dados
     * @return Estatísticas do banco de dados
     */
    virtual DatabaseStats getStats() const = 0;
    
    /**
     * @brief Realiza backup do banco de dados
     * @param backupPath Caminho para o arquivo de backup
     * @return true se o backup foi bem-sucedido
     */
    virtual bool backup(const std::string& backupPath) = 0;
    
    /**
     * @brief Restaura o banco de dados a partir de um backup
     * @param backupPath Caminho para o arquivo de backup
     * @return true se a restauração foi bem-sucedida
     */
    virtual bool restore(const std::string& backupPath) = 0;
    
    /**
     * @brief Define o nível de cache
     * @param level Nível de cache (0 = desativado, 9 = máximo)
     */
    virtual void setCacheLevel(int level) = 0;
    
    /**
     * @brief Limpa o cache
     */
    virtual void clearCache() = 0;
    
    /**
     * @brief Verifica a saúde do banco de dados
     * @return true se o banco de dados estiver saudável
     */
    virtual bool healthCheck() = 0;
    
    /**
     * @brief Realiza tarefas de manutenção
     * @param fullMaintenance Se true, realiza manutenção completa
     */
    virtual void performMaintenance(bool fullMaintenance = false) = 0;
};

} // namespace database
} // namespace wydbr

#endif // WYDBR_DATABASE_CLIENT_H