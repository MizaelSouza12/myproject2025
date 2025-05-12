/**
 * ScalingManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/network/scaling/ScalingManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef SCALINGMANAGER_H
#define SCALINGMANAGER_H

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
#include <set>

#include "../../../../core/WYDTypes.h"
#include "../NetworkTypes.h"
#include "../ConnectionManager.h"
#include "../../kernel/KernelTypes.h"

namespace wydbr {


/**
 * @file ScalingManager.h
 * @brief Sistema de escalabilidade e balanceamento de carga
 * 
 * Este arquivo contém a definição do sistema de escalabilidade e balanceamento
 * de carga, que permite o WYD ser executado com múltiplas instâncias de servidor,
 * proporcionando alta disponibilidade, tolerância a falhas e escalabilidade horizontal.
 */

namespace wyd {
namespace server {
namespace network {
namespace scaling {

/**
 * @brief Tipo de nó de servidor
 */
enum class ServerNodeType : BYTE {
    UNKNOWN = 0,             // Desconhecido
    GATEWAY = 1,             // Gateway
    GAME = 2,                // Jogo
    DATABASE = 3,            // Banco de dados
    AUTH = 4,                // Autenticação
    CHAT = 5,                // Chat
    MASTER = 6,              // Mestre
    REPLICA = 7,             // Réplica
    MONITOR = 8,             // Monitor
    ADMIN = 9,               // Administração
    MATCHMAKING = 10,        // Matchmaking
    SHARD = 11,              // Shard
    CUSTOM = 12,             // Personalizado
};

/**
 * @brief Status do nó de servidor
 */
enum class ServerNodeStatus : BYTE {
    OFFLINE = 0,             // Offline
    STARTING = 1,            // Iniciando
    ONLINE = 2,              // Online
    SHUTTING_DOWN = 3,       // Desligando
    ERROR = 4,               // Erro
    MAINTENANCE = 5,         // Manutenção
    STANDBY = 6,             // Em espera
    SYNCING = 7,             // Sincronizando
    OVERLOADED = 8,          // Sobrecarregado
    UNSTABLE = 9,            // Instável
    CUSTOM = 10,             // Personalizado
};

/**
 * @brief Algoritmo de balanceamento de carga
 */
enum class LoadBalancingAlgorithm : BYTE {
    ROUND_ROBIN = 0,         // Round Robin
    LEAST_CONNECTIONS = 1,   // Menos conexões
    LEAST_LOAD = 2,          // Menos carga
    RESPONSE_TIME = 3,       // Tempo de resposta
    IP_HASH = 4,             // Hash de IP
    WEIGHTED = 5,            // Ponderado
    RANDOM = 6,              // Aleatório
    ZONE_BASED = 7,          // Baseado em zona
    SESSION_BASED = 8,       // Baseado em sessão
    CUSTOM = 9,              // Personalizado
};

/**
 * @brief Tipo de política de escalabilidade
 */
enum class ScalingPolicyType : BYTE {
    NONE = 0,                // Nenhuma
    CPU_BASED = 1,           // Baseada em CPU
    MEMORY_BASED = 2,        // Baseada em memória
    CONNECTION_BASED = 3,    // Baseada em conexões
    LOAD_BASED = 4,          // Baseada em carga
    TIME_BASED = 5,          // Baseada em tempo
    SCHEDULE_BASED = 6,      // Baseada em agenda
    ZONE_BASED = 7,          // Baseada em zona
    CUSTOM = 8,              // Personalizada
};

/**
 * @brief Tipo de sincronização entre servidores
 */
enum class ServerSyncType : BYTE {
    NONE = 0,                // Nenhuma
    STATE_REPLICATION = 1,   // Replicação de estado
    MASTER_SLAVE = 2,        // Mestre-escravo
    MULTI_MASTER = 3,        // Multi-mestre
    SHARDING = 4,            // Sharding
    EVENTUAL = 5,            // Eventual
    STRONG = 6,              // Forte
    CUSTOM = 7,              // Personalizada
};

/**
 * @brief Nó de servidor
 */
struct ServerNode {
    DWORD nodeID;                     // ID do nó
    std::string name;                  // Nome
    ServerNodeType type;              // Tipo
    ServerNodeStatus status;          // Status
    std::string host;                  // Host
    WORD port;                        // Porta
    DWORD connectionID;               // ID da conexão
    float load;                        // Carga (0.0 - 1.0)
    DWORD connectionCount;            // Contagem de conexões
    DWORD maxConnections;             // Máximo de conexões
    float cpuUsage;                    // Uso de CPU (%)
    float memoryUsage;                 // Uso de memória (%)
    float networkUsage;                // Uso de rede (%)
    DWORD weight;                     // Peso
    time_t startTime;                  // Hora de início
    time_t lastUpdateTime;             // Hora da última atualização
    std::vector<DWORD> zoneIDs;        // IDs de zona
    std::string version;               // Versão
    bool isLeader;                     // Se é líder
    
    ServerNode()
        : nodeID(0)
        , name("")
        , type(ServerNodeType::UNKNOWN)
        , status(ServerNodeStatus::OFFLINE)
        , host("")
        , port(0)
        , connectionID(0)
        , load(0.0f)
        , connectionCount(0)
        , maxConnections(0)
        , cpuUsage(0.0f)
        , memoryUsage(0.0f)
        , networkUsage(0.0f)
        , weight(100)
        , startTime(0)
        , lastUpdateTime(0)
        , version("")
        , isLeader(false)
    {
    }
};

/**
 * @brief Zona de jogo
 */
struct GameZone {
    DWORD zoneID;                     // ID da zona
    std::string name;                  // Nome
    DWORD masterNodeID;               // ID do nó mestre
    std::vector<DWORD> replicaNodeIDs; // IDs dos nós de réplica
    DWORD playerCount;                // Contagem de jogadores
    DWORD maxPlayers;                 // Máximo de jogadores
    bool active;                       // Ativa
    std::string description;           // Descrição
    
    GameZone()
        : zoneID(0)
        , name("")
        , masterNodeID(0)
        , playerCount(0)
        , maxPlayers(0)
        , active(false)
        , description("")
    {
    }
};

/**
 * @brief Política de escalabilidade
 */
struct ScalingPolicy {
    DWORD policyID;                   // ID da política
    std::string name;                  // Nome
    ScalingPolicyType type;           // Tipo
    ServerNodeType nodeType;          // Tipo de nó
    float scaleUpThreshold;            // Limite para escalar para cima
    float scaleDownThreshold;          // Limite para escalar para baixo
    DWORD minInstances;               // Mínimo de instâncias
    DWORD maxInstances;               // Máximo de instâncias
    DWORD cooldownMinutes;            // Minutos de cooldown
    bool autoScale;                    // Auto-escalar
    std::string schedule;              // Agenda
    std::map<std::string, std::string> parameters; // Parâmetros
    
    ScalingPolicy()
        : policyID(0)
        , name("")
        , type(ScalingPolicyType::NONE)
        , nodeType(ServerNodeType::GAME)
        , scaleUpThreshold(0.8f)
        , scaleDownThreshold(0.2f)
        , minInstances(1)
        , maxInstances(10)
        , cooldownMinutes(5)
        , autoScale(true)
        , schedule("")
    {
    }
};

/**
 * @brief Estatísticas do cluster
 */
struct ClusterStats {
    DWORD totalNodes;                  // Total de nós
    DWORD activeNodes;                 // Nós ativos
    DWORD totalConnections;            // Total de conexões
    DWORD totalPlayers;                // Total de jogadores
    float averageLoad;                 // Carga média
    float averageCpuUsage;             // Uso médio de CPU
    float averageMemoryUsage;          // Uso médio de memória
    float averageNetworkUsage;         // Uso médio de rede
    DWORD zoneMasterCount;             // Contagem de mestres de zona
    DWORD zoneReplicaCount;            // Contagem de réplicas de zona
    time_t lastScalingEventTime;        // Hora do último evento de escalabilidade
    time_t uptime;                      // Tempo de atividade
    
    ClusterStats()
        : totalNodes(0)
        , activeNodes(0)
        , totalConnections(0)
        , totalPlayers(0)
        , averageLoad(0.0f)
        , averageCpuUsage(0.0f)
        , averageMemoryUsage(0.0f)
        , averageNetworkUsage(0.0f)
        , zoneMasterCount(0)
        , zoneReplicaCount(0)
        , lastScalingEventTime(0)
        , uptime(0)
    {
    }
};

/**
 * @brief Evento de escalabilidade
 */
struct ScalingEvent {
    DWORD eventID;                    // ID do evento
    time_t timestamp;                  // Timestamp
    std::string eventType;             // Tipo de evento
    ServerNodeType nodeType;          // Tipo de nó
    DWORD nodeID;                     // ID do nó
    std::string reason;                // Razão
    bool successful;                   // Bem-sucedido
    std::string details;               // Detalhes
    
    ScalingEvent()
        : eventID(0)
        , timestamp(0)
        , eventType("")
        , nodeType(ServerNodeType::UNKNOWN)
        , nodeID(0)
        , reason("")
        , successful(false)
        , details("")
    {
    }
};

/**
 * @brief Sincronização de servidores
 */
struct ServerSync {
    DWORD syncID;                     // ID da sincronização
    ServerSyncType type;              // Tipo
    DWORD sourceNodeID;               // ID do nó de origem
    DWORD targetNodeID;               // ID do nó de destino
    time_t startTime;                  // Hora de início
    time_t endTime;                    // Hora de término
    bool successful;                   // Bem-sucedida
    std::string errorMessage;          // Mensagem de erro
    DWORD bytesTransferred;           // Bytes transferidos
    
    ServerSync()
        : syncID(0)
        , type(ServerSyncType::NONE)
        , sourceNodeID(0)
        , targetNodeID(0)
        , startTime(0)
        , endTime(0)
        , successful(false)
        , errorMessage("")
        , bytesTransferred(0)
    {
    }
};

/**
 * @brief Configuração do gerenciador de escalabilidade
 */
struct ScalingManagerConfig {
    bool enabled;                     // Habilitado
    std::string nodeName;             // Nome do nó
    ServerNodeType nodeType;          // Tipo de nó
    std::string host;                 // Host
    WORD port;                        // Porta
    DWORD maxConnections;            // Máximo de conexões
    std::string masterHost;           // Host mestre
    WORD masterPort;                 // Porta mestre
    LoadBalancingAlgorithm loadBalancingAlgorithm; // Algoritmo de balanceamento de carga
    ServerSyncType syncType;          // Tipo de sincronização
    DWORD heartbeatIntervalMs;        // Intervalo de heartbeat (ms)
    DWORD statsUpdateIntervalMs;      // Intervalo de atualização de estatísticas (ms)
    DWORD syncIntervalMs;             // Intervalo de sincronização (ms)
    bool autoRegister;                // Auto-registrar
    bool autoJoinCluster;             // Auto-ingressar no cluster
    bool autoConfig;                  // Auto-configurar
    
    ScalingManagerConfig()
        : enabled(true)
        , nodeName("")
        , nodeType(ServerNodeType::GAME)
        , host("0.0.0.0")
        , port(0)
        , maxConnections(1000)
        , masterHost("127.0.0.1")
        , masterPort(8080)
        , loadBalancingAlgorithm(LoadBalancingAlgorithm::LEAST_LOAD)
        , syncType(ServerSyncType::STATE_REPLICATION)
        , heartbeatIntervalMs(5000)
        , statsUpdateIntervalMs(10000)
        , syncIntervalMs(60000)
        , autoRegister(true)
        , autoJoinCluster(true)
        , autoConfig(true)
    {
    }
};

/**
 * @brief Callback de evento de nó
 */
using NodeEventCallback = std::function<void(DWORD, ServerNodeStatus, const std::string&)>;

/**
 * @brief Callback de decisão de balanceamento de carga
 */
using LoadBalancingDecisionCallback = std::function<bool(DWORD, ServerNode&)>;

/**
 * @brief Gerenciador de escalabilidade
 */
/**
 * Classe WYDScalingManager
 * 
 * Esta classe implementa a funcionalidade WYDScalingManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ScalingManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static ScalingManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const ScalingManagerConfig& config);
    
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
     * @brief Registra um nó
     * @param node Nó
     * @return ID do nó, ou 0 se falhar
     */
    DWORD RegisterNode(const ServerNode& node);
    
    /**
     * @brief Remove um nó
     * @param nodeID ID do nó
     * @return true se removido com sucesso
     */
    bool UnregisterNode(DWORD nodeID);
    
    /**
     * @brief Conecta a um cluster
     * @param masterHost Host mestre
     * @param masterPort Porta mestre
     * @return true se conectado com sucesso
     */
    bool ConnectToCluster(const std::string& masterHost, WORD masterPort);
    
    /**
     * @brief Desconecta de um cluster
     * @return true se desconectado com sucesso
     */
    bool DisconnectFromCluster();
    
    /**
     * @brief Atualiza o status de um nó
     * @param nodeID ID do nó
     * @param status Status
     * @param message Mensagem
     * @return true se atualizado com sucesso
     */
    bool UpdateNodeStatus(DWORD nodeID, ServerNodeStatus status, const std::string& message = "");
    
    /**
     * @brief Atualiza estatísticas de um nó
     * @param nodeID ID do nó
     * @param load Carga
     * @param connectionCount Contagem de conexões
     * @param cpuUsage Uso de CPU
     * @param memoryUsage Uso de memória
     * @param networkUsage Uso de rede
     * @return true se atualizado com sucesso
     */
    bool UpdateNodeStats(DWORD nodeID, float load, DWORD connectionCount,
                       float cpuUsage, float memoryUsage, float networkUsage);
    
    /**
     * @brief Atualiza estatísticas do nó local
     * @return true se atualizado com sucesso
     */
    bool UpdateLocalNodeStats();
    
    /**
     * @brief Obtém um nó
     * @param nodeID ID do nó
     * @return Ponteiro para o nó, ou nullptr se não existir
     */
    const ServerNode* GetNode(DWORD nodeID) const;
    
    /**
     * @brief Obtém todos os nós
     * @return Lista de nós
     */
    std::vector<ServerNode> GetAllNodes() const;
    
    /**
     * @brief Obtém nós por tipo
     * @param type Tipo
     * @return Lista de nós
     */
    std::vector<ServerNode> GetNodesByType(ServerNodeType type) const;
    
    /**
     * @brief Obtém nós por status
     * @param status Status
     * @return Lista de nós
     */
    std::vector<ServerNode> GetNodesByStatus(ServerNodeStatus status) const;
    
    /**
     * @brief Obtém o ID do nó local
     * @return ID do nó
     */
    DWORD GetLocalNodeID() const;
    
    /**
     * @brief Obtém o ID do nó mestre
     * @return ID do nó
     */
    DWORD GetMasterNodeID() const;
    
    /**
     * @brief Verifica se o nó local é o mestre
     * @return true se for o mestre
     */
    bool IsLocalNodeMaster() const;
    
    /**
     * @brief Registra uma zona
     * @param zone Zona
     * @return ID da zona, ou 0 se falhar
     */
    DWORD RegisterZone(const GameZone& zone);
    
    /**
     * @brief Remove uma zona
     * @param zoneID ID da zona
     * @return true se removido com sucesso
     */
    bool UnregisterZone(DWORD zoneID);
    
    /**
     * @brief Obtém uma zona
     * @param zoneID ID da zona
     * @return Ponteiro para a zona, ou nullptr se não existir
     */
    const GameZone* GetZone(DWORD zoneID) const;
    
    /**
     * @brief Obtém todas as zonas
     * @return Lista de zonas
     */
    std::vector<GameZone> GetAllZones() const;
    
    /**
     * @brief Atribui um nó mestre a uma zona
     * @param zoneID ID da zona
     * @param nodeID ID do nó
     * @return true se atribuído com sucesso
     */
    bool AssignZoneMaster(DWORD zoneID, DWORD nodeID);
    
    /**
     * @brief Adiciona um nó réplica a uma zona
     * @param zoneID ID da zona
     * @param nodeID ID do nó
     * @return true se adicionado com sucesso
     */
    bool AddZoneReplica(DWORD zoneID, DWORD nodeID);
    
    /**
     * @brief Remove um nó réplica de uma zona
     * @param zoneID ID da zona
     * @param nodeID ID do nó
     * @return true se removido com sucesso
     */
    bool RemoveZoneReplica(DWORD zoneID, DWORD nodeID);
    
    /**
     * @brief Atualiza a contagem de jogadores de uma zona
     * @param zoneID ID da zona
     * @param playerCount Contagem de jogadores
     * @return true se atualizado com sucesso
     */
    bool UpdateZonePlayerCount(DWORD zoneID, DWORD playerCount);
    
    /**
     * @brief Seleciona um nó para balanceamento de carga
     * @param nodeType Tipo de nó
     * @param connectionID ID da conexão
     * @return ID do nó selecionado, ou 0 se nenhum estiver disponível
     */
    DWORD SelectNodeForLoadBalancing(ServerNodeType nodeType, DWORD connectionID);
    
    /**
     * @brief Registra um callback para eventos de nó
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterNodeEventCallback(NodeEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterNodeEventCallback(int callbackID);
    
    /**
     * @brief Registra um callback para decisões de balanceamento de carga
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterLoadBalancingDecisionCallback(LoadBalancingDecisionCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterLoadBalancingDecisionCallback(int callbackID);
    
    /**
     * @brief Obtém estatísticas do cluster
     * @return Estatísticas
     */
    ClusterStats GetClusterStats() const;
    
    /**
     * @brief Registra uma política de escalabilidade
     * @param policy Política
     * @return ID da política, ou 0 se falhar
     */
    DWORD RegisterScalingPolicy(const ScalingPolicy& policy);
    
    /**
     * @brief Remove uma política de escalabilidade
     * @param policyID ID da política
     * @return true se removido com sucesso
     */
    bool UnregisterScalingPolicy(DWORD policyID);
    
    /**
     * @brief Obtém uma política de escalabilidade
     * @param policyID ID da política
     * @return Ponteiro para a política, ou nullptr se não existir
     */
    const ScalingPolicy* GetScalingPolicy(DWORD policyID) const;
    
    /**
     * @brief Obtém todas as políticas de escalabilidade
     * @return Lista de políticas
     */
    std::vector<ScalingPolicy> GetAllScalingPolicies() const;
    
    /**
     * @brief Aplica políticas de escalabilidade
     * @return Número de políticas aplicadas
     */
    size_t ApplyScalingPolicies();
    
    /**
     * @brief Inicia uma sincronização entre servidores
     * @param sourceNodeID ID do nó de origem
     * @param targetNodeID ID do nó de destino
     * @param type Tipo
     * @return ID da sincronização, ou 0 se falhar
     */
    DWORD StartServerSync(DWORD sourceNodeID, DWORD targetNodeID, ServerSyncType type);
    
    /**
     * @brief Finaliza uma sincronização
     * @param syncID ID da sincronização
     * @param successful Se foi bem-sucedida
     * @param errorMessage Mensagem de erro
     * @return true se finalizado com sucesso
     */
    bool FinishServerSync(DWORD syncID, bool successful, const std::string& errorMessage = "");
    
    /**
     * @brief Obtém uma sincronização
     * @param syncID ID da sincronização
     * @return Ponteiro para a sincronização, ou nullptr se não existir
     */
    const ServerSync* GetServerSync(DWORD syncID) const;
    
    /**
     * @brief Obtém todas as sincronizações
     * @return Lista de sincronizações
     */
    std::vector<ServerSync> GetAllServerSyncs() const;
    
    /**
     * @brief Registra um evento de escalabilidade
     * @param eventType Tipo de evento
     * @param nodeType Tipo de nó
     * @param nodeID ID do nó
     * @param reason Razão
     * @param successful Se foi bem-sucedido
     * @param details Detalhes
     * @return ID do evento, ou 0 se falhar
     */
    DWORD RegisterScalingEvent(const std::string& eventType, ServerNodeType nodeType, DWORD nodeID,
                             const std::string& reason, bool successful, const std::string& details);
    
    /**
     * @brief Obtém um evento de escalabilidade
     * @param eventID ID do evento
     * @return Ponteiro para o evento, ou nullptr se não existir
     */
    const ScalingEvent* GetScalingEvent(DWORD eventID) const;
    
    /**
     * @brief Obtém todos os eventos de escalabilidade
     * @param count Número de eventos
     * @param offset Deslocamento
     * @return Lista de eventos
     */
    std::vector<ScalingEvent> GetAllScalingEvents(size_t count, size_t offset = 0) const;
    
    /**
     * @brief Escolhe um novo líder
     * @return ID do nó escolhido, ou 0 se nenhum estiver disponível
     */
    DWORD ElectNewLeader();
    
    /**
     * @brief Envia um heartbeat para todos os nós
     * @return Número de nós para os quais o envio foi bem-sucedido
     */
    size_t SendHeartbeatToAllNodes();
    
    /**
     * @brief Processa um heartbeat recebido
     * @param nodeID ID do nó
     * @param timestamp Timestamp
     * @return true se processado com sucesso
     */
    bool ProcessReceivedHeartbeat(DWORD nodeID, time_t timestamp);
    
    /**
     * @brief Verifica se um nó está ativo
     * @param nodeID ID do nó
     * @return true se estiver ativo
     */
    bool IsNodeAlive(DWORD nodeID) const;
    
    /**
     * @brief Recupera de uma falha de nó
     * @param nodeID ID do nó
     * @return true se recuperado com sucesso
     */
    bool RecoverFromNodeFailure(DWORD nodeID);
    
    /**
     * @brief Define o algoritmo de balanceamento de carga
     * @param algorithm Algoritmo
     */
    void SetLoadBalancingAlgorithm(LoadBalancingAlgorithm algorithm);
    
    /**
     * @brief Obtém o algoritmo de balanceamento de carga
     * @return Algoritmo
     */
    LoadBalancingAlgorithm GetLoadBalancingAlgorithm() const;
    
    /**
     * @brief Define o tipo de sincronização
     * @param type Tipo
     */
    void SetSyncType(ServerSyncType type);
    
    /**
     * @brief Obtém o tipo de sincronização
     * @return Tipo
     */
    ServerSyncType GetSyncType() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    ScalingManager();
    
    /**
     * @brief Destrutor
     */
    ~ScalingManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    ScalingManager(const ScalingManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    ScalingManager& operator=(const ScalingManager&) = delete;
    
    /**
     * @brief Thread de heartbeat
     */
    void HeartbeatThread();
    
    /**
     * @brief Thread de atualização de estatísticas
     */
    void StatsUpdateThread();
    
    /**
     * @brief Thread de sincronização
     */
    void SyncThread();
    
    /**
     * @brief Thread de escalonamento
     */
    void ScalingThread();
    
    /**
     * @brief Notifica callbacks de evento de nó
     * @param nodeID ID do nó
     * @param status Status
     * @param message Mensagem
     */
    void NotifyNodeEventCallbacks(DWORD nodeID, ServerNodeStatus status, const std::string& message);
    
    /**
     * @brief Processamento de balanceamento de carga Round Robin
     * @param nodeType Tipo de nó
     * @return ID do nó selecionado, ou 0 se nenhum estiver disponível
     */
    DWORD RoundRobinLoadBalancing(ServerNodeType nodeType);
    
    /**
     * @brief Processamento de balanceamento de carga por menos conexões
     * @param nodeType Tipo de nó
     * @return ID do nó selecionado, ou 0 se nenhum estiver disponível
     */
    DWORD LeastConnectionsLoadBalancing(ServerNodeType nodeType);
    
    /**
     * @brief Processamento de balanceamento de carga por menos carga
     * @param nodeType Tipo de nó
     * @return ID do nó selecionado, ou 0 se nenhum estiver disponível
     */
    DWORD LeastLoadLoadBalancing(ServerNodeType nodeType);
    
    /**
     * @brief Processamento de balanceamento de carga por tempo de resposta
     * @param nodeType Tipo de nó
     * @return ID do nó selecionado, ou 0 se nenhum estiver disponível
     */
    DWORD ResponseTimeLoadBalancing(ServerNodeType nodeType);
    
    /**
     * @brief Processamento de balanceamento de carga por hash de IP
     * @param nodeType Tipo de nó
     * @param connectionID ID da conexão
     * @return ID do nó selecionado, ou 0 se nenhum estiver disponível
     */
    DWORD IPHashLoadBalancing(ServerNodeType nodeType, DWORD connectionID);
    
    /**
     * @brief Processamento de balanceamento de carga ponderado
     * @param nodeType Tipo de nó
     * @return ID do nó selecionado, ou 0 se nenhum estiver disponível
     */
    DWORD WeightedLoadBalancing(ServerNodeType nodeType);
    
    /**
     * @brief Processamento de balanceamento de carga aleatório
     * @param nodeType Tipo de nó
     * @return ID do nó selecionado, ou 0 se nenhum estiver disponível
     */
    DWORD RandomLoadBalancing(ServerNodeType nodeType);
    
    /**
     * @brief Processamento de balanceamento de carga baseado em zona
     * @param nodeType Tipo de nó
     * @param zoneID ID da zona
     * @return ID do nó selecionado, ou 0 se nenhum estiver disponível
     */
    DWORD ZoneBasedLoadBalancing(ServerNodeType nodeType, DWORD zoneID);
    
    /**
     * @brief Verifica se deve escalar para cima
     * @param policy Política
     * @return true se deve escalar para cima
     */
    bool ShouldScaleUp(const ScalingPolicy& policy);
    
    /**
     * @brief Verifica se deve escalar para baixo
     * @param policy Política
     * @return true se deve escalar para baixo
     */
    bool ShouldScaleDown(const ScalingPolicy& policy);
    
    /**
     * @brief Sobe uma nova instância
     * @param nodeType Tipo de nó
     * @param policyID ID da política
     * @return ID do nó, ou 0 se falhar
     */
    DWORD SpinUpInstance(ServerNodeType nodeType, DWORD policyID);
    
    /**
     * @brief Desce uma instância
     * @param nodeID ID do nó
     * @param policyID ID da política
     * @return true se bem-sucedido
     */
    bool SpinDownInstance(DWORD nodeID, DWORD policyID);
    
    /**
     * @brief Calcula estatísticas do cluster
     * @return Estatísticas
     */
    ClusterStats CalculateClusterStats() const;
    
    /**
     * @brief Verifica nós offline
     */
    void CheckOfflineNodes();
    
    /**
     * @brief Gera um novo ID de nó
     * @return ID de nó
     */
    DWORD GenerateNodeID();
    
    /**
     * @brief Gera um novo ID de zona
     * @return ID de zona
     */
    DWORD GenerateZoneID();
    
    /**
     * @brief Gera um novo ID de política
     * @return ID de política
     */
    DWORD GeneratePolicyID();
    
    /**
     * @brief Gera um novo ID de sincronização
     * @return ID de sincronização
     */
    DWORD GenerateSyncID();
    
    /**
     * @brief Gera um novo ID de evento
     * @return ID de evento
     */
    DWORD GenerateEventID();
    
    // Configuração
    ScalingManagerConfig config_;
    
    // Nós
    std::unordered_map<DWORD, ServerNode> nodes_;
    mutable std::mutex nodesMutex_;
    
    // Zonas
    std::unordered_map<DWORD, GameZone> zones_;
    mutable std::mutex zonesMutex_;
    
    // Políticas de escalabilidade
    std::unordered_map<DWORD, ScalingPolicy> scalingPolicies_;
    mutable std::mutex scalingPoliciesMutex_;
    
    // Sincronizações
    std::unordered_map<DWORD, ServerSync> serverSyncs_;
    mutable std::mutex serverSyncsMutex_;
    
    // Eventos de escalabilidade
    std::vector<ScalingEvent> scalingEvents_;
    mutable std::mutex scalingEventsMutex_;
    
    // Índices para busca rápida
    std::unordered_map<ServerNodeType, std::set<DWORD>> nodesByType_;
    std::unordered_map<ServerNodeStatus, std::set<DWORD>> nodesByStatus_;
    std::unordered_map<DWORD, std::set<DWORD>> nodesByZone_;
    
    // Callbacks para eventos de nó
    std::map<int, NodeEventCallback> nodeEventCallbacks_;
    std::mutex nodeCallbacksMutex_;
    int nextNodeCallbackID_;
    
    // Callbacks para decisões de balanceamento de carga
    std::map<int, LoadBalancingDecisionCallback> loadBalancingDecisionCallbacks_;
    std::mutex loadBalancingCallbacksMutex_;
    int nextLoadBalancingCallbackID_;
    
    // Contadores para balanceamento de carga
    std::atomic<size_t> roundRobinCounter_;
    
    // Contadores para novos IDs
    std::atomic<DWORD> nextNodeID_;
    std::atomic<DWORD> nextZoneID_;
    std::atomic<DWORD> nextPolicyID_;
    std::atomic<DWORD> nextSyncID_;
    std::atomic<DWORD> nextEventID_;
    
    // Threads e sincronização
    std::thread heartbeatThread_;
    std::thread statsUpdateThread_;
    std::thread syncThread_;
    std::thread scalingThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Tempos
    time_t lastHeartbeatTime_;
    time_t lastStatsUpdateTime_;
    time_t lastSyncTime_;
    time_t lastScalingTime_;
    
    // IDs importantes
    DWORD localNodeID_;
    DWORD masterNodeID_;
    
    // Estado
    bool initialized_;
    bool connectedToCluster_;
    LoadBalancingAlgorithm loadBalancingAlgorithm_;
    ServerSyncType syncType_;
};

// Acesso global
#define g_ScalingManager ScalingManager::GetInstance()

} // namespace scaling
} // namespace network
} // namespace server
} // namespace wyd

#endif // SCALINGMANAGER_H

} // namespace wydbr
