/**
 * DBServer.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/dbsrv/DBServer.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef DBSERVER_H
#define DBSERVER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>

#include "DBTypes.h"
#include "DBConnectionManager.h"
#include "DBCacheManager.h"
#include "DBPacketManager.h"
#include "../../core/WYDTypes.h"
#include "../../core/Logger.h"
#include "../../network/SocketManager.h"

namespace wydbr {


/**
 * @file DBServer.h
 * @brief Servidor de banco de dados do WYD
 * 
 * Este arquivo contém a definição do servidor de banco de dados (DBServer)
 * do WYD, que era um componente crítico da arquitetura original.
 * O DBServer atua como intermediário entre os servidores de jogo (TMServer)
 * e o banco de dados, gerenciando cache, transações e persistência.
 */

namespace wyd {
namespace server {

// Callback para eventos do servidor
typedef std::function<void(DB_EVENT, DWORD, const char*)> DBServerEventCallback;

// Estrutura de configuração do servidor
struct DBServerConfig {
    std::string dbConnectionString;          // String de conexão com o banco
    int dbMaxConnections;                    // Número máximo de conexões com o banco
    bool dbAutoReconnect;                    // Se deve reconectar automaticamente
    
    int cacheSize;                           // Tamanho do cache em MB
    DWORD cacheSyncInterval;                 // Intervalo de sincronização do cache (ms)
    
    std::string bindIp;                      // IP para bind do servidor
    int bindPort;                            // Porta para bind do servidor
    int maxClients;                          // Número máximo de clientes
    
    int numPacketWorkers;                    // Número de workers para processamento de pacotes
    
    int logLevel;                            // Nível de log
    std::string logFile;                     // Arquivo de log
    
    DBServerConfig()
        : dbMaxConnections(10)
        , dbAutoReconnect(true)
        , cacheSize(128)                     // 128 MB por padrão
        , cacheSyncInterval(60000)           // 1 minuto por padrão
        , bindPort(8895)                     // Porta padrão do DBServer no WYD original
        , maxClients(100)
        , numPacketWorkers(4)
        , logLevel(1)
    {
        bindIp = "0.0.0.0";                  // Bind em todas as interfaces
    }
};

/**
 * @brief Servidor de banco de dados do WYD
 * 
 * Esta é a classe principal do DBServer, responsável por integrar todos
 * os componentes e gerenciar o ciclo de vida do servidor.
 */
/**
 * Classe WYDDBServer
 * 
 * Esta classe implementa a funcionalidade WYDDBServer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CDBServer {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static CDBServer& GetInstance();
    
    /**
     * @brief Inicializa o servidor
     * @param config Configuração do servidor
     * @return true se inicializado com sucesso
     */
    bool Initialize(const DBServerConfig& config);
    
    /**
     * @brief Finaliza o servidor
     */
    void Shutdown();
    
    /**
     * @brief Inicia o servidor
     * @return true se iniciado com sucesso
     */
    bool Start();
    
    /**
     * @brief Para o servidor
     */
    void Stop();
    
    /**
     * @brief Verifica se o servidor está rodando
     * @return true se estiver rodando
     */
    bool IsRunning() const;
    
    /**
     * @brief Obtém estatísticas do servidor
     * @return String com estatísticas
     */
    std::string GetStats() const;
    
    /**
     * @brief Registra um callback para eventos do servidor
     * @param callback Função de callback
     */
    void RegisterEventCallback(const DBServerEventCallback& callback);
    
    /**
     * @brief Obtém a configuração do servidor
     * @return Configuração do servidor
     */
    const DBServerConfig& GetConfig() const;
    
    /**
     * @brief Carrega a configuração de um arquivo
     * @param filePath Caminho para o arquivo
     * @return true se carregado com sucesso
     */
    bool LoadConfig(const std::string& filePath);
    
    /**
     * @brief Salva a configuração em um arquivo
     * @param filePath Caminho para o arquivo
     * @return true se salvo com sucesso
     */
    bool SaveConfig(const std::string& filePath);
    
    /**
     * @brief Executa manutenção no banco de dados
     * @param fullMaintenance Se é manutenção completa
     * @return true se executado com sucesso
     */
    bool PerformMaintenance(bool fullMaintenance = false);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CDBServer();
    
    /**
     * @brief Destrutor
     */
    ~CDBServer();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CDBServer(const CDBServer&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CDBServer& operator=(const CDBServer&) = delete;
    
    /**
     * @brief Inicializa o socket do servidor
     * @return true se inicializado com sucesso
     */
    bool InitializeSocket();
    
    /**
     * @brief Thread principal do servidor
     */
    void ServerThread();
    
    /**
     * @brief Thread de manutenção do servidor
     */
    void MaintenanceThread();
    
    /**
     * @brief Processa uma conexão de cliente
     * @param clientSocket Socket do cliente
     */
    void HandleClientConnection(SOCKET clientSocket);
    
    /**
     * @brief Função de callback para eventos de conexão com o banco
     * @param connected Se está conectado
     */
    void OnDatabaseConnectionEvent(bool connected);
    
    /**
     * @brief Função de callback para eventos de cache
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param isDirty Se está suja
     */
    void OnCacheEvent(DB_ENTITY_TYPE entityType, DWORD entityId, bool isDirty);
    
    /**
     * @brief Função de callback para eventos de pacote
     * @param packetType Tipo de pacote
     * @param result Resultado do processamento
     * @param clientId ID do cliente
     * @param processTime Tempo de processamento
     */
    void OnPacketEvent(DB_PACKET_TYPE packetType, DB_PACKET_RESULT result, 
                       DWORD clientId, DWORD processTime);
    
    /**
     * @brief Notifica eventos do servidor
     * @param eventType Tipo de evento
     * @param param Parâmetro do evento
     * @param message Mensagem do evento
     */
    void NotifyServerEvent(DB_EVENT eventType, DWORD param, const char* message);
    
    // Configuração
    DBServerConfig config_;
    
    // Estado do servidor
    std::atomic<bool> isRunning_;
    std::atomic<bool> shuttingDown_;
    
    // Socket e conexões
    SOCKET serverSocket_;
    std::unordered_map<SOCKET, DWORD> clientSockets_;
    std::mutex socketsMutex_;
    
    // Threads
    std::thread serverThread_;
    std::thread maintenanceThread_;
    
    // Callbacks
    std::vector<DBServerEventCallback> eventCallbacks_;
    std::mutex callbacksMutex_;
    
    // Estatísticas
    struct ServerStats {
        DWORD dwUptime;                   // Tempo de atividade (segundos)
        DWORD dwConnectedClients;         // Clientes conectados
        DWORD dwTotalConnections;         // Total de conexões desde o início
        DWORD dwTotalDisconnections;      // Total de desconexões desde o início
        DWORD dwTotalEvents;              // Total de eventos notificados
        DWORD dwTotalPacketsProcessed;    // Total de pacotes processados
        DWORD dwPacketsPerSecond;         // Pacotes por segundo (média)
        DWORD dwPeakConnections;          // Pico de conexões simultâneas
        DWORD dwStartTime;                // Hora de início do servidor
        DWORD dwLastMaintenanceTime;      // Hora da última manutenção
        
        ServerStats()
            : dwUptime(0)
            , dwConnectedClients(0)
            , dwTotalConnections(0)
            , dwTotalDisconnections(0)
            , dwTotalEvents(0)
            , dwTotalPacketsProcessed(0)
            , dwPacketsPerSecond(0)
            , dwPeakConnections(0)
            , dwStartTime(0)
            , dwLastMaintenanceTime(0)
        {
        }
    };
    
    ServerStats stats_;
    mutable std::mutex statsMutex_;
    
    // Inicialização
    bool initialized_;
    bool socketInitialized_;
    
    // Define as políticas de cache padrão
    /**
 * SetDefaultCachePolicies
 * 
 * Implementa a funcionalidade SetDefaultCachePolicies conforme especificação original.
 */

    void SetDefaultCachePolicies();
    
    // Registra os handlers de pacotes padrão
    /**
 * RegisterDefaultPacketHandlers
 * 
 * Implementa a funcionalidade RegisterDefaultPacketHandlers conforme especificação original.
 */

    void RegisterDefaultPacketHandlers();
    
    // Registra os callbacks padrão
    /**
 * RegisterCallbacks
 * 
 * Implementa a funcionalidade RegisterCallbacks conforme especificação original.
 */

    void RegisterCallbacks();
};

// Definição global para facilitar o acesso
#define g_DBServer CDBServer::GetInstance()

} // namespace server
} // namespace wyd

#endif // DBSERVER_H

} // namespace wydbr
