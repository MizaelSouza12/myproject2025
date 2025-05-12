/**
 * DBConnectionManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/dbsrv/DBConnectionManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef DBCONNECTIONMANAGER_H
#define DBCONNECTIONMANAGER_H

#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>

#include "DBTypes.h"
#include "../../core/WYDTypes.h"
#include "../../core/Logger.h"

namespace wydbr {


/**
 * @file DBConnectionManager.h
 * @brief Gerenciador de conexões do DBServer
 * 
 * Este arquivo contém a definição do gerenciador de conexões do DBServer,
 * que é responsável por gerenciar as conexões com o banco de dados e as transações.
 * No WYD original, estas funcionalidades estavam espalhadas em vários arquivos.
 */

namespace wyd {
namespace server {

// Tipos de callback para eventos de conexão
typedef std::function<void(bool)> DBConnectionCallback;
// Tipos de callback para resultados de query
typedef std::function<void(const STRUCT_DBRESULT&)> DBQueryCallback;
// Tipo de callback para eventos de transação
typedef std::function<void(DWORD, DB_TRANSACTION_STATE)> DBTransactionCallback;

/**
 * @brief Estrutura de conexão com o banco (original: DB_CONNECTION)
 * 
 * Esta estrutura armazena informações sobre uma conexão com o banco de dados.
 * No WYD original, isso era gerenciado de maneira mais primitiva.
 */
struct STRUCT_DBCONNECTION {
    std::string connectionString;    // String de conexão
    DB_CONNECTION_STATE state;       // Estado da conexão
    DWORD dwLastPingTime;            // Hora do último ping
    DWORD dwLastErrorTime;           // Hora do último erro
    DWORD dwReconnectAttempts;       // Tentativas de reconexão
    DWORD dwConnectionID;            // ID da conexão
    bool isInUse;                    // Se está em uso
    void* pDBHandle;                 // Handle para a conexão com o banco (ODBC, MySQL, etc.)
    
    STRUCT_DBCONNECTION()
        : state(DB_CONN_DISCONNECTED)
        , dwLastPingTime(0)
        , dwLastErrorTime(0)
        , dwReconnectAttempts(0)
        , dwConnectionID(0)
        , isInUse(false)
        , pDBHandle(nullptr)
    {
    }
};

/**
 * @brief Gerenciador de conexões com o banco de dados
 * 
 * Esta classe gerencia as conexões com o banco de dados, transações e queries.
 * No WYD original, estas funcionalidades estavam distribuídas em vários arquivos
 * e frequentemente causavam problemas de concorrência e vazamentos de memória.
 */
/**
 * Classe WYDDBConnectionManager
 * 
 * Esta classe implementa a funcionalidade WYDDBConnectionManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CDBConnectionManager {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static CDBConnectionManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador de conexões
     * @param connectionString String de conexão com o banco
     * @param maxConnections Número máximo de conexões
     * @param autoReconnect Se deve reconectar automaticamente
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& connectionString, int maxConnections = 10, bool autoReconnect = true);
    
    /**
     * @brief Finaliza o gerenciador de conexões
     */
    void Shutdown();
    
    /**
     * @brief Executa uma query síncrona
     * @param query Estrutura da query
     * @return Resultado da query
     */
    STRUCT_DBRESULT ExecuteQuery(const STRUCT_DBQUERY& query);
    
    /**
     * @brief Executa uma query assíncrona
     * @param query Estrutura da query
     * @param callback Callback para quando a query for concluída
     * @return ID da query
     */
    DWORD ExecuteQueryAsync(const STRUCT_DBQUERY& query, const DBQueryCallback& callback);
    
    /**
     * @brief Cancela uma query assíncrona
     * @param queryID ID da query
     * @return true se cancelada com sucesso
     */
    bool CancelQuery(DWORD queryID);
    
    /**
     * @brief Inicia uma nova transação
     * @param timeout Timeout da transação (em ms)
     * @return ID da transação (0 em caso de erro)
     */
    DWORD BeginTransaction(DWORD timeout = 30000);
    
    /**
     * @brief Confirma uma transação
     * @param transactionID ID da transação
     * @return true se confirmada com sucesso
     */
    bool CommitTransaction(DWORD transactionID);
    
    /**
     * @brief Reverte uma transação
     * @param transactionID ID da transação
     * @return true se revertida com sucesso
     */
    bool RollbackTransaction(DWORD transactionID);
    
    /**
     * @brief Executa várias queries em uma transação
     * @param queries Vetor de queries
     * @param timeout Timeout da transação (em ms)
     * @return true se todas as queries foram executadas com sucesso
     */
    bool ExecuteInTransaction(const std::vector<STRUCT_DBQUERY>& queries, DWORD timeout = 30000);
    
    /**
     * @brief Obtém o estado atual da conexão
     * @return Estado da conexão
     */
    DB_CONNECTION_STATE GetConnectionState() const;
    
    /**
     * @brief Registra um callback para eventos de conexão
     * @param callback Função de callback
     */
    void RegisterConnectionCallback(const DBConnectionCallback& callback);
    
    /**
     * @brief Registra um callback para eventos de transação
     * @param callback Função de callback
     */
    void RegisterTransactionCallback(const DBTransactionCallback& callback);
    
    /**
     * @brief Obtém estatísticas de conexão
     * @return String com estatísticas
     */
    std::string GetConnectionStats() const;
    
    /**
     * @brief Obtém estatísticas de queries
     * @return String com estatísticas
     */
    std::string GetQueryStats() const;
    
    /**
     * @brief Obtém estatísticas de transações
     * @return String com estatísticas
     */
    std::string GetTransactionStats() const;
    
    /**
     * @brief Verifica se há queries pendentes
     * @return true se houver queries pendentes
     */
    bool HasPendingQueries() const;
    
    /**
     * @brief Verifica se há transações pendentes
     * @return true se houver transações pendentes
     */
    bool HasPendingTransactions() const;
    
    /**
     * @brief Limpa todas as queries pendentes
     */
    void ClearPendingQueries();
    
    /**
     * @brief Limpa todas as transações pendentes
     */
    void ClearPendingTransactions();
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CDBConnectionManager();
    
    /**
     * @brief Destrutor
     */
    ~CDBConnectionManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CDBConnectionManager(const CDBConnectionManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CDBConnectionManager& operator=(const CDBConnectionManager&) = delete;
    
    /**
     * @brief Conecta ao banco de dados
     * @return true se conectado com sucesso
     */
    bool Connect();
    
    /**
     * @brief Desconecta do banco de dados
     */
    void Disconnect();
    
    /**
     * @brief Obtém uma conexão disponível
     * @return Ponteiro para a conexão, ou nullptr se não houver
     */
    STRUCT_DBCONNECTION* GetAvailableConnection();
    
    /**
     * @brief Libera uma conexão
     * @param connection Ponteiro para a conexão
     */
    void ReleaseConnection(STRUCT_DBCONNECTION* connection);
    
    /**
     * @brief Thread de processamento de queries assíncronas
     */
    void QueryProcessingThread();
    
    /**
     * @brief Thread de monitoramento de conexões
     */
    void ConnectionMonitorThread();
    
    /**
     * @brief Thread de timeout de transações
     */
    void TransactionTimeoutThread();
    
    /**
     * @brief Processa uma query
     * @param query Estrutura da query
     * @param connection Ponteiro para a conexão
     * @return Resultado da query
     */
    STRUCT_DBRESULT ProcessQuery(const STRUCT_DBQUERY& query, STRUCT_DBCONNECTION* connection);
    
    /**
     * @brief Notifica eventos de conexão
     * @param connected Se está conectado
     */
    void NotifyConnectionEvent(bool connected);
    
    /**
     * @brief Notifica eventos de transação
     * @param transactionID ID da transação
     * @param state Estado da transação
     */
    void NotifyTransactionEvent(DWORD transactionID, DB_TRANSACTION_STATE state);
    
    // Parâmetros de configuração
    std::string connectionString_;          // String de conexão
    int maxConnections_;                    // Número máximo de conexões
    bool autoReconnect_;                    // Se deve reconectar automaticamente
    bool initialized_;                      // Se o gerenciador está inicializado
    
    // Estado da conexão
    std::atomic<DB_CONNECTION_STATE> connectionState_; // Estado da conexão
    
    // Pool de conexões
    std::vector<STRUCT_DBCONNECTION> connectionPool_; // Pool de conexões
    std::mutex connectionPoolMutex_;                  // Mutex para o pool
    std::condition_variable connectionPoolCV_;        // Variável de condição para o pool
    
    // Queries assíncronas
    struct AsyncQueryTask {
        DWORD queryID;                      // ID da query
        STRUCT_DBQUERY query;               // Estrutura da query
        DBQueryCallback callback;           // Callback para quando a query for concluída
        DWORD startTime;                    // Hora de início da query
        
        AsyncQueryTask()
            : queryID(0)
            , startTime(0)
        {
        }
    };
    
    std::queue<AsyncQueryTask> queryQueue_;         // Fila de queries assíncronas
    std::mutex queryQueueMutex_;                    // Mutex para a fila de queries
    std::condition_variable queryQueueCV_;          // Variável de condição para a fila
    DWORD nextQueryID_;                             // Próximo ID de query
    
    // Transações
    std::unordered_map<DWORD, STRUCT_DBTRANSACTION> transactions_; // Mapa de transações
    std::mutex transactionsMutex_;                               // Mutex para as transações
    DWORD nextTransactionID_;                                    // Próximo ID de transação
    
    // Callbacks
    std::vector<DBConnectionCallback> connectionCallbacks_;   // Callbacks para eventos de conexão
    std::vector<DBTransactionCallback> transactionCallbacks_; // Callbacks para eventos de transação
    std::mutex callbacksMutex_;                               // Mutex para os callbacks
    
    // Threads
    std::vector<std::thread> queryThreads_;                   // Threads de processamento de queries
    std::thread connectionMonitorThread_;                     // Thread de monitoramento de conexões
    std::thread transactionTimeoutThread_;                    // Thread de timeout de transações
    std::atomic<bool> shutdownFlag_;                          // Flag para shutdown
    
    // Estatísticas
    struct DBStats {
        DWORD dwTotalQueries;               // Total de queries executadas
        DWORD dwSuccessfulQueries;          // Queries bem-sucedidas
        DWORD dwFailedQueries;              // Queries com erro
        DWORD dwTotalTransactions;          // Total de transações
        DWORD dwSuccessfulTransactions;     // Transações confirmadas
        DWORD dwRolledBackTransactions;     // Transações revertidas
        DWORD dwTotalConnectionAttempts;    // Total de tentativas de conexão
        DWORD dwSuccessfulConnections;      // Conexões bem-sucedidas
        DWORD dwFailedConnections;          // Falhas de conexão
        DWORD dwTotalReconnections;         // Total de reconexões
        DWORD dwMaxQueryTime;               // Tempo máximo de execução de query (ms)
        DWORD dwTotalQueryTime;             // Tempo total de execução de queries (ms)
        DWORD dwQueriesPerSecond;           // Queries por segundo (média)
        
        DBStats()
            : dwTotalQueries(0)
            , dwSuccessfulQueries(0)
            , dwFailedQueries(0)
            , dwTotalTransactions(0)
            , dwSuccessfulTransactions(0)
            , dwRolledBackTransactions(0)
            , dwTotalConnectionAttempts(0)
            , dwSuccessfulConnections(0)
            , dwFailedConnections(0)
            , dwTotalReconnections(0)
            , dwMaxQueryTime(0)
            , dwTotalQueryTime(0)
            , dwQueriesPerSecond(0)
        {
        }
    };
    
    DBStats stats_;                         // Estatísticas
    std::mutex statsMutex_;                 // Mutex para as estatísticas
};

// Definição global para facilitar o acesso
#define g_DBConnection CDBConnectionManager::GetInstance()

} // namespace server
} // namespace wyd

#endif // DBCONNECTIONMANAGER_H

} // namespace wydbr
