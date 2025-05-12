/**
 * ConnectionPool.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/network/ConnectionPool.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _CONNECTION_POOL_H_
#define _CONNECTION_POOL_H_

/**
 * @file ConnectionPool.h
 * @brief Pool de conexões para o servidor WYDBRASIL
 * 
 * Esta classe implementa um gerenciador de múltiplas conexões simultâneas para o servidor,
 * otimizando o uso de recursos de rede e melhorando o desempenho em alta concorrência.
 */

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <queue>
#include <chrono>

#include "../../include/CPSock.h"
#include "NetworkManager.h"

namespace wydbr {


/**
 * @class ConnectionPool
 * @brief Gerencia um pool de conexões de rede
 * 
 * Esta classe gerencia um conjunto de conexões de rede, permitindo a reutilização
 * eficiente de conexões e melhorando o desempenho em operações de rede intensivas.
 * Implementa técnicas avançadas de balanceamento de carga e failover.
 */
/**
 * Classe WYDConnectionPool
 * 
 * Esta classe implementa a funcionalidade WYDConnectionPool conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ConnectionPool {
public:
    /**
     * @brief Obtém a instância única do ConnectionPool (Singleton)
     * @return Referência para a instância única
     */
    static ConnectionPool& getInstance();
    
    /**
     * @brief Inicializa o pool de conexões
     * @param maxConnections Número máximo de conexões no pool
     * @param initialConnections Número inicial de conexões a criar
     * @param connectionTimeout Timeout para tentativas de conexão (ms)
     * @return true se inicializado com sucesso
     */
    bool initialize(int maxConnections = 50, int initialConnections = 5, int connectionTimeout = 5000);
    
    /**
     * @brief Adiciona um servidor ao pool
     * @param host Endereço do servidor
     * @param port Porta do servidor
     * @param weight Peso para balanceamento (maior = mais conexões)
     * @param isRequired Servidor é obrigatório para operação
     * @return ID do servidor ou -1 se falhou
     */
    int addServer(const std::string& host, int port, int weight = 1, bool isRequired = true);
    
    /**
     * @brief Remove um servidor do pool
     * @param serverId ID do servidor
     * @return true se removido com sucesso
     */
    bool removeServer(int serverId);
    
    /**
     * @brief Define função para autenticação nas conexões
     * @param authFunc Função de autenticação
     */
    void setAuthenticationFunction(std::function<bool(int, const std::string&, int)> authFunc);
    
    /**
     * @brief Obtém uma conexão do pool
     * @param preferredServerId ID do servidor preferido (opcional)
     * @param waitTimeout Tempo máximo de espera por conexão (ms)
     * @return ID da conexão ou -1 se indisponível
     */
    int getConnection(int preferredServerId = -1, int waitTimeout = 1000);
    
    /**
     * @brief Libera uma conexão de volta para o pool
     * @param connectionId ID da conexão
     * @return true se liberado com sucesso
     */
    bool releaseConnection(int connectionId);
    
    /**
     * @brief Executa uma operação com uma conexão do pool
     * @param operation Função para executar com a conexão
     * @param preferredServerId ID do servidor preferido (opcional)
     * @param waitTimeout Tempo máximo de espera por conexão (ms)
     * @param retries Número de tentativas em caso de falha
     * @return true se operação executada com sucesso
     */
    bool executeWithConnection(std::function<bool(int)> operation, 
                               int preferredServerId = -1, 
                               int waitTimeout = 1000,
                               int retries = 1);
    
    /**
     * @brief Verifica saúde de todos os servidores
     * @param removeUnhealthy Remove servidores não saudáveis
     * @return Número de servidores saudáveis
     */
    int checkServerHealth(bool removeUnhealthy = false);
    
    /**
     * @brief Define callback para eventos de saúde do servidor
     * @param callback Função de callback
     */
    void setServerHealthCallback(std::function<void(int, bool, const std::string&)> callback);
    
    /**
     * @brief Desativa temporariamente um servidor
     * @param serverId ID do servidor
     * @param duration Duração da desativação (ms)
     * @return true se desativado com sucesso
     */
    bool disableServer(int serverId, int duration = 30000);
    
    /**
     * @brief Reativa um servidor desativado
     * @param serverId ID do servidor
     * @return true se reativado com sucesso
     */
    bool enableServer(int serverId);
    
    /**
     * @brief Fecha todas as conexões e finaliza o pool
     */
    void shutdown();
    
    /**
     * @brief Obtém estatísticas do pool
     * @param totalConnections Total de conexões no pool (saída)
     * @param activeConnections Conexões ativas no momento (saída)
     * @param waitingRequests Requisições aguardando conexão (saída)
     * @param avgWaitTime Tempo médio de espera por conexão (ms) (saída)
     */
    void getStats(int& totalConnections, int& activeConnections, 
                 int& waitingRequests, float& avgWaitTime);
    
    /**
     * @brief Verifica estado atual de uma conexão
     * @param connectionId ID da conexão
     * @return true se conexão ativa e saudável
     */
    bool isConnectionHealthy(int connectionId);
    
    /**
     * @brief Obtém endereço do servidor para uma conexão
     * @param connectionId ID da conexão
     * @return Endereço no formato "host:porta"
     */
    std::string getServerAddress(int connectionId);
    
    /**
     * @brief Executa uma operação em múltiplos servidores em paralelo
     * @param operation Função a executar em cada servidor
     * @param targetServerCount Número de servidores alvo (opcional)
     * @return Número de servidores onde a operação foi bem-sucedida
     */
    int executeOnMultipleServers(std::function<bool(int)> operation, int targetServerCount = -1);
    
private:
    // Construtor privado (singleton)
    ConnectionPool();
    
    // Destrutor
    ~ConnectionPool();
    
    // Impedir cópia e atribuição
    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;
    
    // Estrutura para um servidor no pool
    struct Server {
        std::string host;              // Endereço do servidor
        int port;                      // Porta do servidor
        int weight;                    // Peso para balanceamento
        bool isRequired;               // Servidor obrigatório
        bool isActive;                 // Servidor ativo
        std::chrono::steady_clock::time_point reactivationTime; // Tempo para reativação
        int healthCheckFailures;       // Contagem de falhas de saúde
        
        Server() : port(0), weight(1), isRequired(true), isActive(true), healthCheckFailures(0) {}
    };
    
    // Estrutura para uma conexão no pool
    struct Connection {
        int connectionId;              // ID da conexão
        int serverId;                  // ID do servidor associado
        std::shared_ptr<CPSock> socket; // Socket da conexão
        bool inUse;                    // Conexão em uso
        std::chrono::steady_clock::time_point lastActivity; // Última atividade
        
        Connection() : connectionId(-1), serverId(-1), inUse(false) {}
    };
    
    // Thread para monitoramento de saúde
    /**
 * healthCheckThread
 * 
 * Implementa a funcionalidade healthCheckThread conforme especificação original.
 */

    void healthCheckThread();
    
    // Thread para gerenciamento de pool
    /**
 * poolManagementThread
 * 
 * Implementa a funcionalidade poolManagementThread conforme especificação original.
 */

    void poolManagementThread();
    
    // Cria uma nova conexão para um servidor
    /**
 * createConnection
 * 
 * Implementa a funcionalidade createConnection conforme especificação original.
 * @param serverId Parâmetro serverId
 * @return Retorna int
 */

    int createConnection(int serverId);
    
    // Fecha uma conexão
    /**
 * closeConnection
 * 
 * Implementa a funcionalidade closeConnection conforme especificação original.
 * @param connectionId Parâmetro connectionId
 * @return Retorna bool
 */

    bool closeConnection(int connectionId);
    
    // Verifica saúde de um servidor específico
    /**
 * checkServerHealth
 * 
 * Implementa a funcionalidade checkServerHealth conforme especificação original.
 * @param serverId Parâmetro serverId
 * @return Retorna bool
 */

    bool checkServerHealth(int serverId);
    
    // Balanceador de carga - seleciona servidor baseado em peso e saúde
    /**
 * selectServer
 * 
 * Implementa a funcionalidade selectServer conforme especificação original.
 * @param -1 Parâmetro -1
 * @return Retorna int
 */

    int selectServer(int preferredServerId = -1);
    
    // Mapeia servidores para informações
    std::unordered_map<int, Server> m_servers;
    
    // Mutex para acesso a servidores
    std::mutex m_serversMutex;
    
    // Mapeia conexões para informações
    std::unordered_map<int, std::shared_ptr<Connection>> m_connections;
    
    // Mutex para acesso a conexões
    std::mutex m_connectionsMutex;
    
    // Fila de conexões disponíveis
    std::queue<int> m_availableConnections;
    
    // Mutex para fila de conexões
    std::mutex m_queueMutex;
    
    // Variável de condição para espera por conexões
    std::condition_variable m_connectionAvailable;
    
    // Função para autenticação
    std::function<bool(int, const std::string&, int)> m_authFunction;
    
    // Callback para eventos de saúde
    std::function<void(int, bool, const std::string&)> m_healthCallback;
    
    // Thread de verificação de saúde
    std::thread m_healthThread;
    
    // Thread de gerenciamento do pool
    std::thread m_poolThread;
    
    // Próximo ID de servidor
    std::atomic<int> m_nextServerId;
    
    // Próximo ID de conexão
    std::atomic<int> m_nextConnectionId;
    
    // Configurações
    int m_maxConnections;
    int m_initialConnections;
    int m_connectionTimeout;
    
    // Estatísticas
    std::atomic<int> m_totalConnectionsCreated;
    std::atomic<int> m_connectionsInUse;
    std::atomic<int> m_waitingRequests;
    std::atomic<uint64_t> m_totalWaitTime;
    std::atomic<int> m_waitTimeSamples;
    
    // Flag de atividade
    std::atomic<bool> m_active;
    
    // Inicializado
    bool m_initialized;
};

#endif // _CONNECTION_POOL_H_

} // namespace wydbr
