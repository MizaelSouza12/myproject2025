/**
 * ConnectionManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/network/ConnectionManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

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
#include <set>

#include "../../../core/WYDTypes.h"
#include "NetworkTypes.h"
#include "../security/SecurityTypes.h"
#include "../kernel/KernelTypes.h"

namespace wydbr {


/**
 * @file ConnectionManager.h
 * @brief Gerenciador de conexões avançado
 * 
 * Este arquivo contém a definição do gerenciador de conexões avançado,
 * que gerencia conexões de rede, alocação de sockets, multiplexação de I/O,
 * e implementa mecanismos de tolerância a falhas e escalabilidade horizontal.
 */

namespace wyd {
namespace server {
namespace network {

/**
 * @brief Configuração do gerenciador de conexões
 */
struct ConnectionManagerConfig {
    bool useEventBasedIO;                   // Usar I/O baseado em eventos
    bool useMultithreading;                 // Usar multithreading
    bool useAsyncIO;                        // Usar I/O assíncrono
    bool useConnectionPooling;              // Usar pooling de conexões
    bool enableKeepalive;                   // Habilitar keepalive
    bool enableNagleAlgorithm;              // Habilitar algoritmo de Nagle
    bool enableLinger;                      // Habilitar linger
    bool reuseAddress;                      // Reutilizar endereço
    bool reusePort;                         // Reutilizar porta
    int socketSendBufferSize;              // Tamanho do buffer de envio de socket
    int socketReceiveBufferSize;           // Tamanho do buffer de recebimento de socket
    DWORD connectionTimeoutMs;             // Timeout de conexão (ms)
    DWORD heartbeatIntervalMs;             // Intervalo de heartbeat (ms)
    DWORD serviceIntervalMs;               // Intervalo de serviço (ms)
    DWORD threadCount;                     // Contagem de threads
    size_t maxConnections;                  // Máximo de conexões
    std::string defaultBindAddress;         // Endereço de bind padrão
    
    ConnectionManagerConfig()
        : useEventBasedIO(true)
        , useMultithreading(true)
        , useAsyncIO(true)
        , useConnectionPooling(true)
        , enableKeepalive(true)
        , enableNagleAlgorithm(false)
        , enableLinger(true)
        , reuseAddress(true)
        , reusePort(true)
        , socketSendBufferSize(1024 * 1024)
        , socketReceiveBufferSize(1024 * 1024)
        , connectionTimeoutMs(CONNECTION_TIMEOUT_MS)
        , heartbeatIntervalMs(HEARTBEAT_INTERVAL_MS)
        , serviceIntervalMs(1000)
        , threadCount(8)
        , maxConnections(MAX_CONNECTIONS)
        , defaultBindAddress("0.0.0.0")
    {
    }
};

/**
 * @brief Contexto de escuta
 */
struct ListenContext {
    DWORD listenerID;                     // ID do listener
    std::string bindAddress;               // Endereço de bind
    WORD port;                            // Porta
    ProtocolType protocol;                // Protocolo
    size_t maxConnections;                 // Máximo de conexões
    size_t listenBacklog;                  // Backlog de escuta
    bool active;                           // Ativo
    int socketHandle;                     // Handle do socket
    
    ListenContext()
        : listenerID(0)
        , bindAddress("0.0.0.0")
        , port(0)
        , protocol(ProtocolType::TCP)
        , maxConnections(MAX_CONNECTIONS)
        , listenBacklog(DEFAULT_LISTEN_BACKLOG)
        , active(false)
        , socketHandle(-1)
    {
    }
};

/**
 * @brief Contexto de conexão
 */
struct ConnectionContext {
    DWORD connectionID;                   // ID da conexão
    ConnectionState state;                // Estado
    ConnectionType type;                  // Tipo
    ConnectionPriority priority;          // Prioridade
    std::string remoteAddress;             // Endereço remoto
    WORD remotePort;                      // Porta remota
    std::string localAddress;              // Endereço local
    WORD localPort;                       // Porta local
    int socketHandle;                     // Handle do socket
    time_t connectionTime;                 // Hora da conexão
    time_t lastActivityTime;               // Hora da última atividade
    DWORD accountID;                      // ID da conta
    DWORD characterID;                    // ID do personagem
    NetworkBuffer sendBuffer;              // Buffer de envio
    NetworkBuffer receiveBuffer;           // Buffer de recebimento
    std::vector<ChannelContext> channels;   // Canais
    std::mutex sendMutex;                  // Mutex de envio
    std::mutex receiveMutex;               // Mutex de recebimento
    ConnectionStats stats;                 // Estatísticas
    ConnectionConfig config;               // Configuração
    BYTE encryptionKey[16];                // Chave de criptografia
    DWORD compressionLevel;                // Nível de compressão
    bool authenticated;                    // Autenticado
    DWORD nextOutgoingSequence;           // Próxima sequência de saída
    DWORD nextExpectedIncomingSequence;   // Próxima sequência de entrada esperada
    DWORD disconnectReason;               // Razão de desconexão
    std::string disconnectMessage;          // Mensagem de desconexão
    
    ConnectionContext()
        : connectionID(0)
        , state(ConnectionState::DISCONNECTED)
        , type(ConnectionType::CLIENT)
        , priority(ConnectionPriority::MEDIUM)
        , remoteAddress("")
        , remotePort(0)
        , localAddress("")
        , localPort(0)
        , socketHandle(-1)
        , connectionTime(0)
        , lastActivityTime(0)
        , accountID(0)
        , characterID(0)
        , compressionLevel(0)
        , authenticated(false)
        , nextOutgoingSequence(0)
        , nextExpectedIncomingSequence(0)
        , disconnectReason(0)
        , disconnectMessage("")
    {
        std::memset(encryptionKey, 0, sizeof(encryptionKey));
        channels.resize(MAX_CHANNEL_COUNT);
        for (BYTE i = 0; i < MAX_CHANNEL_COUNT; i++) {
            channels[i].id = i;
        }
    }
};

/**
 * @brief Contexto de I/O
 */
struct IOContext {
    enum /**
 * Classe WYDIOOperation
 * 
 * Esta classe implementa a funcionalidade WYDIOOperation conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class IOOperation {
        NONE,
        ACCEPT,
        CONNECT,
        SEND,
        RECEIVE
    };
    
    IOOperation operation;                 // Operação
    DWORD connectionID;                   // ID da conexão
    NetworkBuffer buffer;                  // Buffer
    DWORD bytesTransferred;               // Bytes transferidos
    DWORD error;                          // Erro
    std::function<void(IOContext&)> callback; // Callback
    void* overlapped;                      // Overlapped (para I/O assíncrono)
    
    IOContext()
        : operation(IOOperation::NONE)
        , connectionID(0)
        , bytesTransferred(0)
        , error(0)
        , overlapped(nullptr)
    {
    }
};

/**
 * @brief Evento de conexão
 */
struct ConnectionEvent {
    enum /**
 * Classe WYDEventType
 * 
 * Esta classe implementa a funcionalidade WYDEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventType {
        CONNECTED,
        DISCONNECTED,
        DATA_RECEIVED,
        DATA_SENT,
        ERROR,
        STATE_CHANGED,
        TIMEOUT,
        CUSTOM
    };
    
    EventType type;                        // Tipo
    DWORD connectionID;                   // ID da conexão
    ConnectionState state;                // Estado
    void* data;                            // Dados
    size_t dataSize;                       // Tamanho dos dados
    DWORD error;                          // Erro
    std::string errorMessage;              // Mensagem de erro
    
    ConnectionEvent()
        : type(EventType::CONNECTED)
        , connectionID(0)
        , state(ConnectionState::DISCONNECTED)
        , data(nullptr)
        , dataSize(0)
        , error(0)
        , errorMessage("")
    {
    }
};

/**
 * @brief Callback de evento de conexão
 */
using ConnectionEventCallback = std::function<void(const ConnectionEvent&)>;

/**
 * @brief Gerenciador de conexões
 */
/**
 * Classe WYDConnectionManager
 * 
 * Esta classe implementa a funcionalidade WYDConnectionManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ConnectionManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static ConnectionManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const ConnectionManagerConfig& config);
    
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
     * @brief Inicia a escuta em um endereço e porta
     * @param address Endereço
     * @param port Porta
     * @param protocol Protocolo
     * @param maxConnections Máximo de conexões
     * @param listenBacklog Backlog de escuta
     * @return ID do listener, ou 0 se falhar
     */
    DWORD StartListen(const std::string& address, WORD port, 
                     ProtocolType protocol = ProtocolType::TCP,
                     size_t maxConnections = 0, size_t listenBacklog = 0);
    
    /**
     * @brief Para a escuta
     * @param listenerID ID do listener
     * @return true se parado com sucesso
     */
    bool StopListen(DWORD listenerID);
    
    /**
     * @brief Conecta a um servidor remoto
     * @param address Endereço
     * @param port Porta
     * @param protocol Protocolo
     * @param config Configuração
     * @return ID da conexão, ou 0 se falhar
     */
    DWORD Connect(const std::string& address, WORD port, 
                 ProtocolType protocol = ProtocolType::TCP,
                 const ConnectionConfig& config = ConnectionConfig());
    
    /**
     * @brief Disconecta uma conexão
     * @param connectionID ID da conexão
     * @param reason Razão
     * @param message Mensagem
     * @return true se desconectado com sucesso
     */
    bool Disconnect(DWORD connectionID, DWORD reason = 0, const std::string& message = "");
    
    /**
     * @brief Envia dados para uma conexão
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @param packetType Tipo de pacote
     * @param channel Canal
     * @param reliable Confiável
     * @return Bytes enviados, ou -1 se falhar
     */
    int Send(DWORD connectionID, const void* data, size_t size, WORD packetType,
            BYTE channel = 0, bool reliable = true);
    
    /**
     * @brief Envia um pacote
     * @param packet Pacote
     * @return Bytes enviados, ou -1 se falhar
     */
    int SendPacket(const NetworkPacket& packet);
    
    /**
     * @brief Envia dados para múltiplas conexões
     * @param connectionIDs IDs das conexões
     * @param data Dados
     * @param size Tamanho
     * @param packetType Tipo de pacote
     * @param channel Canal
     * @param reliable Confiável
     * @return Número de conexões para as quais o envio foi bem-sucedido
     */
    size_t SendToMultiple(const std::vector<DWORD>& connectionIDs, const void* data, size_t size,
                        WORD packetType, BYTE channel = 0, bool reliable = true);
    
    /**
     * @brief Envia dados para todas as conexões
     * @param data Dados
     * @param size Tamanho
     * @param packetType Tipo de pacote
     * @param channel Canal
     * @param reliable Confiável
     * @param excludeConnectionID ID de conexão a excluir
     * @return Número de conexões para as quais o envio foi bem-sucedido
     */
    size_t Broadcast(const void* data, size_t size, WORD packetType, BYTE channel = 0,
                   bool reliable = true, DWORD excludeConnectionID = 0);
    
    /**
     * @brief Recebe dados de uma conexão
     * @param connectionID ID da conexão
     * @param buffer Buffer
     * @param size Tamanho
     * @param packetType Tipo de pacote (saída)
     * @param channel Canal (saída)
     * @return Bytes recebidos, ou -1 se falhar
     */
    int Receive(DWORD connectionID, void* buffer, size_t size, WORD& packetType, BYTE& channel);
    
    /**
     * @brief Recebe um pacote
     * @param connectionID ID da conexão
     * @param packet Pacote (saída)
     * @return Bytes recebidos, ou -1 se falhar
     */
    int ReceivePacket(DWORD connectionID, NetworkPacket& packet);
    
    /**
     * @brief Cria uma conexão manualmente
     * @param socketHandle Handle do socket
     * @param remoteAddress Endereço remoto
     * @param remotePort Porta remota
     * @param type Tipo
     * @param config Configuração
     * @return ID da conexão, ou 0 se falhar
     */
    DWORD CreateConnection(int socketHandle, const std::string& remoteAddress, WORD remotePort,
                         ConnectionType type = ConnectionType::CLIENT,
                         const ConnectionConfig& config = ConnectionConfig());
    
    /**
     * @brief Destrói uma conexão manualmente
     * @param connectionID ID da conexão
     * @return true se destruído com sucesso
     */
    bool DestroyConnection(DWORD connectionID);
    
    /**
     * @brief Obtém o contexto de uma conexão
     * @param connectionID ID da conexão
     * @return Ponteiro para o contexto, ou nullptr se não existir
     */
    const ConnectionContext* GetConnectionContext(DWORD connectionID) const;
    
    /**
     * @brief Obtém as estatísticas de uma conexão
     * @param connectionID ID da conexão
     * @return Estatísticas
     */
    ConnectionStats GetConnectionStats(DWORD connectionID) const;
    
    /**
     * @brief Obtém o ID da conta de uma conexão
     * @param connectionID ID da conexão
     * @return ID da conta
     */
    DWORD GetConnectionAccountID(DWORD connectionID) const;
    
    /**
     * @brief Define o ID da conta de uma conexão
     * @param connectionID ID da conexão
     * @param accountID ID da conta
     * @return true se definido com sucesso
     */
    bool SetConnectionAccountID(DWORD connectionID, DWORD accountID);
    
    /**
     * @brief Obtém o ID do personagem de uma conexão
     * @param connectionID ID da conexão
     * @return ID do personagem
     */
    DWORD GetConnectionCharacterID(DWORD connectionID) const;
    
    /**
     * @brief Define o ID do personagem de uma conexão
     * @param connectionID ID da conexão
     * @param characterID ID do personagem
     * @return true se definido com sucesso
     */
    bool SetConnectionCharacterID(DWORD connectionID, DWORD characterID);
    
    /**
     * @brief Obtém as configurações de uma conexão
     * @param connectionID ID da conexão
     * @return Configurações
     */
    ConnectionConfig GetConnectionConfig(DWORD connectionID) const;
    
    /**
     * @brief Define as configurações de uma conexão
     * @param connectionID ID da conexão
     * @param config Configurações
     * @return true se definido com sucesso
     */
    bool SetConnectionConfig(DWORD connectionID, const ConnectionConfig& config);
    
    /**
     * @brief Obtém o endereço remoto de uma conexão
     * @param connectionID ID da conexão
     * @return Endereço remoto
     */
    std::string GetConnectionRemoteAddress(DWORD connectionID) const;
    
    /**
     * @brief Obtém a porta remota de uma conexão
     * @param connectionID ID da conexão
     * @return Porta remota
     */
    WORD GetConnectionRemotePort(DWORD connectionID) const;
    
    /**
     * @brief Verifica se uma conexão está autenticada
     * @param connectionID ID da conexão
     * @return true se estiver autenticada
     */
    bool IsConnectionAuthenticated(DWORD connectionID) const;
    
    /**
     * @brief Define o estado de autenticação de uma conexão
     * @param connectionID ID da conexão
     * @param authenticated Se está autenticada
     * @return true se definido com sucesso
     */
    bool SetConnectionAuthenticated(DWORD connectionID, bool authenticated);
    
    /**
     * @brief Obtém a latência de uma conexão
     * @param connectionID ID da conexão
     * @return Latência (ms)
     */
    float GetConnectionLatency(DWORD connectionID) const;
    
    /**
     * @brief Envia um heartbeat para uma conexão
     * @param connectionID ID da conexão
     * @return true se enviado com sucesso
     */
    bool SendHeartbeat(DWORD connectionID);
    
    /**
     * @brief Processa um heartbeat de uma conexão
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessHeartbeat(DWORD connectionID, const void* data, size_t size);
    
    /**
     * @brief Envia dados criptografados
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @param packetType Tipo de pacote
     * @param channel Canal
     * @return Bytes enviados, ou -1 se falhar
     */
    int SendEncrypted(DWORD connectionID, const void* data, size_t size, WORD packetType, BYTE channel = 0);
    
    /**
     * @brief Envia dados comprimidos
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @param packetType Tipo de pacote
     * @param channel Canal
     * @return Bytes enviados, ou -1 se falhar
     */
    int SendCompressed(DWORD connectionID, const void* data, size_t size, WORD packetType, BYTE channel = 0);
    
    /**
     * @brief Criptografa dados
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @return Bytes criptografados, ou -1 se falhar
     */
    int EncryptData(DWORD connectionID, void* data, size_t size);
    
    /**
     * @brief Descriptografa dados
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @return Bytes descriptografados, ou -1 se falhar
     */
    int DecryptData(DWORD connectionID, void* data, size_t size);
    
    /**
     * @brief Comprime dados
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @param compressedData Dados comprimidos (saída)
     * @param compressedSize Tamanho comprimido (saída)
     * @return Bytes comprimidos, ou -1 se falhar
     */
    int CompressData(DWORD connectionID, const void* data, size_t size, void** compressedData, size_t* compressedSize);
    
    /**
     * @brief Descomprime dados
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @param decompressedData Dados descomprimidos (saída)
     * @param decompressedSize Tamanho descomprimido (saída)
     * @return Bytes descomprimidos, ou -1 se falhar
     */
    int DecompressData(DWORD connectionID, const void* data, size_t size, void** decompressedData, size_t* decompressedSize);
    
    /**
     * @brief Calcula o checksum de um pacote
     * @param data Dados
     * @param size Tamanho
     * @return Checksum
     */
    DWORD CalculatePacketChecksum(const void* data, size_t size);
    
    /**
     * @brief Registra um callback para eventos de conexão
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterConnectionEventCallback(ConnectionEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterConnectionEventCallback(int callbackID);
    
    /**
     * @brief Obtém todas as conexões
     * @return Lista de IDs de conexão
     */
    std::vector<DWORD> GetAllConnections() const;
    
    /**
     * @brief Obtém conexões por tipo
     * @param type Tipo
     * @return Lista de IDs de conexão
     */
    std::vector<DWORD> GetConnectionsByType(ConnectionType type) const;
    
    /**
     * @brief Obtém conexões por estado
     * @param state Estado
     * @return Lista de IDs de conexão
     */
    std::vector<DWORD> GetConnectionsByState(ConnectionState state) const;
    
    /**
     * @brief Obtém o número de conexões ativas
     * @return Número de conexões
     */
    size_t GetActiveConnectionCount() const;
    
    /**
     * @brief Obtém o número de conexões por tipo
     * @param type Tipo
     * @return Número de conexões
     */
    size_t GetConnectionCountByType(ConnectionType type) const;
    
    /**
     * @brief Obtém a lista de listeners ativos
     * @return Lista de IDs de listener
     */
    std::vector<DWORD> GetActiveListeners() const;
    
    /**
     * @brief Reconecta uma conexão
     * @param connectionID ID da conexão
     * @return true se reconectado com sucesso
     */
    bool ReconnectConnection(DWORD connectionID);
    
    /**
     * @brief Define a prioridade de uma conexão
     * @param connectionID ID da conexão
     * @param priority Prioridade
     * @return true se definido com sucesso
     */
    bool SetConnectionPriority(DWORD connectionID, ConnectionPriority priority);
    
    /**
     * @brief Obtém a prioridade de uma conexão
     * @param connectionID ID da conexão
     * @return Prioridade
     */
    ConnectionPriority GetConnectionPriority(DWORD connectionID) const;
    
    /**
     * @brief Define o contexto de um canal
     * @param connectionID ID da conexão
     * @param channelID ID do canal
     * @param type Tipo
     * @param reliable Confiável
     * @param ordered Ordenado
     * @return true se definido com sucesso
     */
    bool SetChannelContext(DWORD connectionID, BYTE channelID, ChannelType type,
                         bool reliable, bool ordered);
    
    /**
     * @brief Obtém o contexto de um canal
     * @param connectionID ID da conexão
     * @param channelID ID do canal
     * @return Ponteiro para o contexto, ou nullptr se não existir
     */
    const ChannelContext* GetChannelContext(DWORD connectionID, BYTE channelID) const;
    
    /**
     * @brief Fragmenta um pacote grande
     * @param packet Pacote
     * @param maxFragmentSize Tamanho máximo de fragmento
     * @return Lista de pacotes fragmentados
     */
    std::vector<NetworkPacket> FragmentPacket(const NetworkPacket& packet, size_t maxFragmentSize);
    
    /**
     * @brief Monta um pacote fragmentado
     * @param fragments Fragmentos
     * @return Pacote completo, ou nullptr se não estiver completo
     */
    NetworkPacket* AssembleFragmentedPacket(const std::vector<PacketFragment>& fragments);
    
    /**
     * @brief Limpa fragmentos antigos
     * @param maxAgeMs Idade máxima (ms)
     * @return Número de fragmentos limpos
     */
    size_t CleanupStaleFragments(DWORD maxAgeMs = 30000);
    
    /**
     * @brief Define o tamanho do buffer de uma conexão
     * @param connectionID ID da conexão
     * @param sendBufferSize Tamanho do buffer de envio
     * @param receiveBufferSize Tamanho do buffer de recebimento
     * @return true se definido com sucesso
     */
    bool SetConnectionBufferSize(DWORD connectionID, size_t sendBufferSize, size_t receiveBufferSize);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    ConnectionManager();
    
    /**
     * @brief Destrutor
     */
    ~ConnectionManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    ConnectionManager(const ConnectionManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    ConnectionManager& operator=(const ConnectionManager&) = delete;
    
    /**
     * @brief Thread de aceitação
     * @param listenerID ID do listener
     */
    void AcceptThread(DWORD listenerID);
    
    /**
     * @brief Thread de serviço
     */
    void ServiceThread();
    
    /**
     * @brief Thread de I/O
     */
    void IOThread();
    
    /**
     * @brief Processa E/S de uma conexão
     * @param connectionID ID da conexão
     * @return true se processado com sucesso
     */
    bool ProcessConnectionIO(DWORD connectionID);
    
    /**
     * @brief Tenta aceitar uma conexão
     * @param listenerID ID do listener
     * @return ID da conexão aceita, ou 0 se nenhuma foi aceita
     */
    DWORD TryAcceptConnection(DWORD listenerID);
    
    /**
     * @brief Processa dados recebidos
     * @param connectionID ID da conexão
     * @return true se processado com sucesso
     */
    bool ProcessReceivedData(DWORD connectionID);
    
    /**
     * @brief Processa envio pendente
     * @param connectionID ID da conexão
     * @return true se processado com sucesso
     */
    bool ProcessPendingSend(DWORD connectionID);
    
    /**
     * @brief Processa heartbeats
     */
    void ProcessHeartbeats();
    
    /**
     * @brief Processa timeouts
     */
    void ProcessTimeouts();
    
    /**
     * @brief Notifica callbacks de evento
     * @param event Evento
     */
    void NotifyConnectionEventCallbacks(const ConnectionEvent& event);
    
    /**
     * @brief Cria um socket
     * @param protocol Protocolo
     * @param nonBlocking Se deve ser não bloqueante
     * @return Handle do socket, ou -1 se falhar
     */
    int CreateSocket(ProtocolType protocol, bool nonBlocking);
    
    /**
     * @brief Fecha um socket
     * @param socketHandle Handle do socket
     * @return true se fechado com sucesso
     */
    bool CloseSocket(int socketHandle);
    
    /**
     * @brief Configura um socket
     * @param socketHandle Handle do socket
     * @param sendBufferSize Tamanho do buffer de envio
     * @param receiveBufferSize Tamanho do buffer de recebimento
     * @param noDelay Sem delay (TCP_NODELAY)
     * @param keepAlive Keep-alive
     * @param reuseAddress Reutilizar endereço
     * @param reusePort Reutilizar porta
     * @return true se configurado com sucesso
     */
    bool ConfigureSocket(int socketHandle, int sendBufferSize, int receiveBufferSize,
                       bool noDelay, bool keepAlive, bool reuseAddress, bool reusePort);
    
    /**
     * @brief Configura um socket para escuta
     * @param socketHandle Handle do socket
     * @param address Endereço
     * @param port Porta
     * @param listenBacklog Backlog de escuta
     * @return true se configurado com sucesso
     */
    bool ConfigureListenSocket(int socketHandle, const std::string& address, WORD port, size_t listenBacklog);
    
    /**
     * @brief Atualiza as estatísticas de conexão
     * @param connectionID ID da conexão
     * @param bytesSent Bytes enviados
     * @param bytesReceived Bytes recebidos
     * @param packetsSent Pacotes enviados
     * @param packetsReceived Pacotes recebidos
     */
    void UpdateConnectionStats(DWORD connectionID, size_t bytesSent, size_t bytesReceived,
                              DWORD packetsSent, DWORD packetsReceived);
    
    /**
     * @brief Trata erros de socket
     * @param socketHandle Handle do socket
     * @param errorCode Código de erro
     * @return Mensagem de erro
     */
    std::string HandleSocketError(int socketHandle, int errorCode);
    
    /**
     * @brief Trata uma alteração de estado de conexão
     * @param connectionID ID da conexão
     * @param newState Novo estado
     * @return true se tratado com sucesso
     */
    bool HandleConnectionStateChange(DWORD connectionID, ConnectionState newState);
    
    /**
     * @brief Verifica se um ID de conexão é válido
     * @param connectionID ID da conexão
     * @return true se for válido
     */
    bool IsValidConnectionID(DWORD connectionID) const;
    
    /**
     * @brief Verifica se um ID de listener é válido
     * @param listenerID ID do listener
     * @return true se for válido
     */
    bool IsValidListenerID(DWORD listenerID) const;
    
    /**
     * @brief Gera um novo ID de conexão
     * @return ID de conexão
     */
    DWORD GenerateConnectionID();
    
    /**
     * @brief Gera um novo ID de listener
     * @return ID de listener
     */
    DWORD GenerateListenerID();
    
    // Configuração
    ConnectionManagerConfig config_;
    
    // Contextos de conexão
    std::unordered_map<DWORD, ConnectionContext> connections_;
    mutable std::mutex connectionsMutex_;
    
    // Contextos de escuta
    std::unordered_map<DWORD, ListenContext> listeners_;
    mutable std::mutex listenersMutex_;
    
    // Fragmentos de pacotes
    std::unordered_map<DWORD, std::unordered_map<DWORD, std::vector<PacketFragment>>> fragmentsByConnection_;
    mutable std::mutex fragmentsMutex_;
    
    // Índices para busca rápida
    std::unordered_map<ConnectionType, std::set<DWORD>> connectionsByType_;
    std::unordered_map<ConnectionState, std::set<DWORD>> connectionsByState_;
    
    // Callbacks para eventos
    std::map<int, ConnectionEventCallback> eventCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contadores para novos IDs
    std::atomic<DWORD> nextConnectionID_;
    std::atomic<DWORD> nextListenerID_;
    
    // Threads e sincronização
    std::vector<std::thread> acceptThreads_;
    std::vector<std::thread> ioThreads_;
    std::thread serviceThread_;
    std::atomic<bool> running_;
    std::mutex serviceMutex_;
    
    // Tempos
    time_t lastHeartbeatTime_;
    time_t lastTimeoutCheckTime_;
    
    // Conjuntos de sockets para I/O multiplexado
    int maxSocketHandle_;
    fd_set readSet_;
    fd_set writeSet_;
    fd_set errorSet_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_ConnectionManager ConnectionManager::GetInstance()

} // namespace network
} // namespace server
} // namespace wyd

#endif // CONNECTIONMANAGER_H

} // namespace wydbr
