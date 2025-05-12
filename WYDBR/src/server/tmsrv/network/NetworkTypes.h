/**
 * NetworkTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/network/NetworkTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef NETWORKTYPES_H
#define NETWORKTYPES_H

#include <string>
#include <vector>
#include <map>
#include <array>
#include <bitset>
#include <chrono>
#include <ctime>
#include <functional>
#include <atomic>
#include <mutex>
#include <memory>
#include <deque>

#include "../../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file NetworkTypes.h
 * @brief Definições de tipos para o sistema de rede
 * 
 * Este arquivo contém as definições de tipos para o sistema de rede avançado do WYD,
 * que implementa múltiplas camadas de otimização, compressão, segurança e escalabilidade
 * para resolver problemas conhecidos do sistema original.
 */

namespace wyd {
namespace server {
namespace network {

// Constantes de rede
constexpr size_t MAX_CONNECTIONS = 20000;           // Máximo de conexões simultâneas
constexpr size_t MAX_PACKET_SIZE = 65536;           // Tamanho máximo de pacote
constexpr size_t PACKET_HEADER_SIZE = 12;           // Tamanho do cabeçalho de pacote
constexpr size_t PACKET_BUFFER_SIZE = 1024 * 1024;  // Tamanho do buffer de pacote
constexpr size_t MAX_PACKET_TYPES = 65536;          // Máximo de tipos de pacote
constexpr size_t MAX_PACKET_HANDLERS = 1000;        // Máximo de manipuladores de pacote
constexpr size_t MAX_PACKET_FILTERS = 100;          // Máximo de filtros de pacote
constexpr size_t MAX_CHANNEL_COUNT = 16;            // Máximo de canais
constexpr size_t MAX_COMPRESSION_LEVEL = 9;         // Máximo nível de compressão
constexpr size_t MAX_ENCRYPTION_LEVEL = 256;        // Máximo nível de criptografia
constexpr size_t MAX_SERVER_INSTANCES = 100;        // Máximo de instâncias de servidor
constexpr size_t DEFAULT_LISTEN_BACKLOG = 128;      // Backlog de escuta padrão
constexpr DWORD CONNECTION_TIMEOUT_MS = 60000;      // Timeout de conexão (ms)
constexpr DWORD HEARTBEAT_INTERVAL_MS = 30000;      // Intervalo de heartbeat (ms)

/**
 * @brief Estado de conexão
 */
enum class ConnectionState : BYTE {
    DISCONNECTED = 0,         // Desconectado
    CONNECTING = 1,           // Conectando
    HANDSHAKING = 2,          // Handshaking
    AUTHENTICATING = 3,       // Autenticando
    CONNECTED = 4,            // Conectado
    DISCONNECTING = 5,        // Desconectando
    RECONNECTING = 6,         // Reconectando
    ERROR = 7,                // Erro
    TIMEOUT = 8,              // Timeout
    ZOMBIE = 9,               // Zumbi (pendente para limpeza)
    CUSTOM = 10,              // Personalizado
};

/**
 * @brief Tipo de conexão
 */
enum class ConnectionType : BYTE {
    CLIENT = 0,               // Cliente
    GAME_SERVER = 1,          // Servidor de jogo
    GAME_GATE = 2,            // Gate de jogo
    CONNECTOR = 3,            // Conector
    DATABASE = 4,             // Banco de dados
    ADMIN = 5,                // Administrador
    MONITOR = 6,              // Monitor
    TEST = 7,                 // Teste
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Prioridade de conexão
 */
enum class ConnectionPriority : BYTE {
    LOW = 0,                  // Baixa
    MEDIUM = 1,               // Média
    HIGH = 2,                 // Alta
    CRITICAL = 3,             // Crítica
    SYSTEM = 4,               // Sistema
    CUSTOM = 5,               // Personalizada
};

/**
 * @brief Tipo de protocolo
 */
enum class ProtocolType : BYTE {
    TCP = 0,                  // TCP
    UDP = 1,                  // UDP
    WEBSOCKET = 2,            // WebSocket
    HTTP = 3,                 // HTTP
    CUSTOM = 4,               // Personalizado
};

/**
 * @brief Tipo de compressão
 */
enum class NetworkCompressionType : BYTE {
    NONE = 0,                 // Nenhuma
    ZLIB = 1,                 // zlib
    LZMA = 2,                 // LZMA
    LZ4 = 3,                  // LZ4
    ZSTD = 4,                 // Zstandard
    BROTLI = 5,               // Brotli
    CUSTOM = 6,               // Personalizada
};

/**
 * @brief Tipo de criptografia
 */
enum class NetworkEncryptionType : BYTE {
    NONE = 0,                 // Nenhuma
    XOR = 1,                  // XOR
    AES = 2,                  // AES
    BLOWFISH = 3,             // Blowfish
    RSA = 4,                  // RSA
    TLS = 5,                  // TLS
    CUSTOM = 6,               // Personalizada
};

/**
 * @brief Tipo de canal
 */
enum class ChannelType : BYTE {
    RELIABLE = 0,             // Confiável
    UNRELIABLE = 1,           // Não confiável
    ORDERED = 2,              // Ordenado
    UNORDERED = 3,            // Não ordenado
    PRIORITY = 4,             // Prioridade
    FRAGMENTED = 5,           // Fragmentado
    COMPRESSED = 6,           // Comprimido
    ENCRYPTED = 7,            // Criptografado
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Tipo de evento de rede
 */
enum class NetworkEventType : BYTE {
    CONNECTED = 0,            // Conectado
    DISCONNECTED = 1,         // Desconectado
    DATA_RECEIVED = 2,        // Dados recebidos
    DATA_SENT = 3,            // Dados enviados
    ERROR = 4,                // Erro
    TIMEOUT = 5,              // Timeout
    HANDSHAKE = 6,            // Handshake
    HEARTBEAT = 7,            // Heartbeat
    RECONNECT = 8,            // Reconexão
    CUSTOM = 9,               // Personalizado
};

/**
 * @brief Tipo de estatística de rede
 */
enum class NetworkStatType : BYTE {
    PACKETS_SENT = 0,         // Pacotes enviados
    PACKETS_RECEIVED = 1,     // Pacotes recebidos
    BYTES_SENT = 2,           // Bytes enviados
    BYTES_RECEIVED = 3,       // Bytes recebidos
    PACKETS_DROPPED = 4,      // Pacotes descartados
    LATENCY = 5,              // Latência
    PACKET_LOSS = 6,          // Perda de pacote
    JITTER = 7,               // Jitter
    CONNECTION_COUNT = 8,     // Contagem de conexões
    BANDWIDTH_USAGE = 9,      // Uso de largura de banda
    CUSTOM = 10,              // Personalizado
};

/**
 * @brief Tipo de política de rede
 */
enum class NetworkPolicyType : BYTE {
    RATE_LIMITING = 0,        // Limitação de taxa
    CONGESTION_CONTROL = 1,   // Controle de congestionamento
    QUALITY_OF_SERVICE = 2,   // Qualidade de serviço
    LOAD_BALANCING = 3,       // Balanceamento de carga
    FAULT_TOLERANCE = 4,      // Tolerância a falhas
    AUTO_SCALING = 5,         // Auto-scaling
    TRAFFIC_SHAPING = 6,      // Modelagem de tráfego
    CUSTOM = 7,               // Personalizada
};

/**
 * @brief Tipo de erro de rede
 */
enum class NetworkErrorType : BYTE {
    NONE = 0,                 // Nenhum
    CONNECTION_REFUSED = 1,   // Conexão recusada
    CONNECTION_RESET = 2,     // Conexão resetada
    HOST_UNREACHABLE = 3,     // Host inalcançável
    TIMEOUT = 4,              // Timeout
    INVALID_PACKET = 5,       // Pacote inválido
    BUFFER_OVERFLOW = 6,      // Estouro de buffer
    AUTHENTICATION_FAILED = 7, // Autenticação falhou
    PROTOCOL_ERROR = 8,       // Erro de protocolo
    INTERNAL_ERROR = 9,       // Erro interno
    CUSTOM = 10,              // Personalizado
};

/**
 * @brief Cabeçalho de pacote
 */
struct PacketHeader {
    WORD size;                 // Tamanho
    WORD type;                 // Tipo
    DWORD sequence;            // Sequência
    DWORD checksum;            // Checksum
    
    PacketHeader()
        : size(0)
        , type(0)
        , sequence(0)
        , checksum(0)
    {
    }
};

/**
 * @brief Configuração de conexão
 */
struct ConnectionConfig {
    ProtocolType protocolType;             // Tipo de protocolo
    NetworkCompressionType compressionType; // Tipo de compressão
    NetworkEncryptionType encryptionType;  // Tipo de criptografia
    ConnectionPriority priority;           // Prioridade
    DWORD timeoutMs;                       // Timeout (ms)
    DWORD heartbeatIntervalMs;             // Intervalo de heartbeat (ms)
    size_t sendBufferSize;                 // Tamanho do buffer de envio
    size_t receiveBufferSize;              // Tamanho do buffer de recebimento
    bool noDelay;                          // Sem delay (TCP_NODELAY)
    bool keepAlive;                        // Keep-alive
    int compressionLevel;                  // Nível de compressão
    int encryptionLevel;                   // Nível de criptografia
    std::string encryptionKey;             // Chave de criptografia
    bool autoReconnect;                    // Auto-reconexão
    DWORD reconnectIntervalMs;             // Intervalo de reconexão (ms)
    DWORD maxReconnectAttempts;            // Máximo de tentativas de reconexão
    
    ConnectionConfig()
        : protocolType(ProtocolType::TCP)
        , compressionType(NetworkCompressionType::NONE)
        , encryptionType(NetworkEncryptionType::NONE)
        , priority(ConnectionPriority::MEDIUM)
        , timeoutMs(CONNECTION_TIMEOUT_MS)
        , heartbeatIntervalMs(HEARTBEAT_INTERVAL_MS)
        , sendBufferSize(PACKET_BUFFER_SIZE)
        , receiveBufferSize(PACKET_BUFFER_SIZE)
        , noDelay(true)
        , keepAlive(true)
        , compressionLevel(6)
        , encryptionLevel(128)
        , encryptionKey("")
        , autoReconnect(true)
        , reconnectIntervalMs(5000)
        , maxReconnectAttempts(5)
    {
    }
};

/**
 * @brief Estatísticas de conexão
 */
struct ConnectionStats {
    DWORD packetsSent;                     // Pacotes enviados
    DWORD packetsReceived;                 // Pacotes recebidos
    size_t bytesSent;                       // Bytes enviados
    size_t bytesReceived;                   // Bytes recebidos
    DWORD packetsDropped;                  // Pacotes descartados
    float latencyMs;                        // Latência (ms)
    float packetLossPercent;                // Perda de pacote (%)
    float jitterMs;                         // Jitter (ms)
    DWORD connectionDurationMs;            // Duração da conexão (ms)
    float compressionRatio;                 // Relação de compressão
    DWORD reconnectCount;                  // Contagem de reconexões
    DWORD errorCount;                      // Contagem de erros
    time_t lastActivityTime;                // Hora da última atividade
    
    ConnectionStats()
        : packetsSent(0)
        , packetsReceived(0)
        , bytesSent(0)
        , bytesReceived(0)
        , packetsDropped(0)
        , latencyMs(0.0f)
        , packetLossPercent(0.0f)
        , jitterMs(0.0f)
        , connectionDurationMs(0)
        , compressionRatio(1.0f)
        , reconnectCount(0)
        , errorCount(0)
        , lastActivityTime(0)
    {
    }
};

/**
 * @brief Buffer de rede
 */
struct NetworkBuffer {
    size_t capacity;                        // Capacidade
    size_t dataSize;                        // Tamanho dos dados
    size_t readPosition;                    // Posição de leitura
    size_t writePosition;                   // Posição de escrita
    BYTE* buffer;                          // Buffer
    bool dynamicSize;                       // Tamanho dinâmico
    
    NetworkBuffer()
        : capacity(0)
        , dataSize(0)
        , readPosition(0)
        , writePosition(0)
        , buffer(nullptr)
        , dynamicSize(false)
    {
    }
    
    NetworkBuffer(size_t initialCapacity, bool dynamic = false)
        : capacity(initialCapacity)
        , dataSize(0)
        , readPosition(0)
        , writePosition(0)
        , buffer(nullptr)
        , dynamicSize(dynamic)
    {
        if (capacity > 0) {
            buffer = new BYTE[capacity];
            std::memset(buffer, 0, capacity);
        }
    }
    
    ~NetworkBuffer() {
        if (buffer) {
            delete[] buffer;
            buffer = nullptr;
        }
    }
};

/**
 * @brief Endereço de rede
 */
struct NetworkAddress {
    std::string host;                       // Host
    WORD port;                             // Porta
    std::string protocol;                   // Protocolo
    std::string toString;                   // Representação em string
    
    NetworkAddress()
        : host("")
        , port(0)
        , protocol("tcp")
        , toString("")
    {
    }
    
    NetworkAddress(const std::string& _host, WORD _port, const std::string& _protocol = "tcp")
        : host(_host)
        , port(_port)
        , protocol(_protocol)
    {
        toString = host + ":" + std::to_string(port) + "/" + protocol;
    }
};

/**
 * @brief Pacote de rede
 */
struct NetworkPacket {
    PacketHeader header;                    // Cabeçalho
    std::vector<BYTE> data;                 // Dados
    DWORD connectionID;                    // ID da conexão
    time_t timestamp;                       // Timestamp
    BYTE channel;                          // Canal
    bool reliable;                          // Confiável
    bool fragmented;                        // Fragmentado
    WORD fragmentCount;                    // Contagem de fragmentos
    WORD fragmentIndex;                    // Índice de fragmento
    DWORD fragmentGroupID;                 // ID do grupo de fragmentos
    bool compressed;                        // Comprimido
    bool encrypted;                         // Criptografado
    
    NetworkPacket()
        : connectionID(0)
        , timestamp(0)
        , channel(0)
        , reliable(true)
        , fragmented(false)
        , fragmentCount(0)
        , fragmentIndex(0)
        , fragmentGroupID(0)
        , compressed(false)
        , encrypted(false)
    {
    }
};

/**
 * @brief Fragmento de pacote
 */
struct PacketFragment {
    DWORD fragmentGroupID;                 // ID do grupo de fragmentos
    WORD fragmentIndex;                    // Índice de fragmento
    WORD fragmentCount;                    // Contagem de fragmentos
    std::vector<BYTE> data;                 // Dados
    bool received;                          // Recebido
    
    PacketFragment()
        : fragmentGroupID(0)
        , fragmentIndex(0)
        , fragmentCount(0)
        , received(false)
    {
    }
};

/**
 * @brief Contexto de canal
 */
struct ChannelContext {
    ChannelType type;                       // Tipo
    BYTE id;                               // ID
    bool reliable;                          // Confiável
    bool ordered;                           // Ordenado
    DWORD nextSequence;                    // Próxima sequência
    std::deque<NetworkPacket> sendQueue;     // Fila de envio
    std::deque<NetworkPacket> receiveQueue;  // Fila de recebimento
    std::map<DWORD, std::vector<PacketFragment>> fragmentGroups; // Grupos de fragmentos
    
    ChannelContext()
        : type(ChannelType::RELIABLE)
        , id(0)
        , reliable(true)
        , ordered(true)
        , nextSequence(0)
    {
    }
};

/**
 * @brief Evento de rede
 */
struct NetworkEvent {
    NetworkEventType type;                  // Tipo
    DWORD connectionID;                    // ID da conexão
    void* data;                             // Dados
    size_t dataSize;                        // Tamanho dos dados
    time_t timestamp;                       // Timestamp
    WORD packetType;                       // Tipo de pacote
    NetworkErrorType error;                // Erro
    std::string errorMessage;               // Mensagem de erro
    
    NetworkEvent()
        : type(NetworkEventType::CONNECTED)
        , connectionID(0)
        , data(nullptr)
        , dataSize(0)
        , timestamp(0)
        , packetType(0)
        , error(NetworkErrorType::NONE)
        , errorMessage("")
    {
    }
};

/**
 * @brief Filtro de pacote
 */
struct PacketFilter {
    DWORD filterID;                        // ID do filtro
    std::string name;                       // Nome
    bool enabled;                           // Habilitado
    std::vector<WORD> includedTypes;        // Tipos incluídos
    std::vector<WORD> excludedTypes;        // Tipos excluídos
    std::function<bool(const NetworkPacket&)> filterFunction; // Função de filtro
    
    PacketFilter()
        : filterID(0)
        , name("")
        , enabled(true)
    {
    }
};

/**
 * @brief Política de rede
 */
struct NetworkPolicy {
    DWORD policyID;                        // ID da política
    NetworkPolicyType type;                // Tipo
    std::string name;                       // Nome
    bool enabled;                           // Habilitada
    std::map<std::string, std::string> parameters; // Parâmetros
    
    NetworkPolicy()
        : policyID(0)
        , type(NetworkPolicyType::RATE_LIMITING)
        , name("")
        , enabled(true)
    {
    }
};

/**
 * @brief Configuração do servidor
 */
struct ServerConfig {
    std::string bindAddress;                // Endereço de bind
    WORD port;                             // Porta
    ProtocolType protocol;                 // Protocolo
    size_t maxConnections;                  // Máximo de conexões
    size_t listenBacklog;                   // Backlog de escuta
    bool reuseAddress;                      // Reutilizar endereço
    bool reusePort;                         // Reutilizar porta
    bool nonBlocking;                       // Não bloqueante
    DWORD acceptThreadCount;               // Contagem de threads de aceitação
    DWORD workThreadCount;                 // Contagem de threads de trabalho
    
    ServerConfig()
        : bindAddress("0.0.0.0")
        , port(8080)
        , protocol(ProtocolType::TCP)
        , maxConnections(MAX_CONNECTIONS)
        , listenBacklog(DEFAULT_LISTEN_BACKLOG)
        , reuseAddress(true)
        , reusePort(true)
        , nonBlocking(true)
        , acceptThreadCount(1)
        , workThreadCount(4)
    {
    }
};

/**
 * @brief Configuração do cliente
 */
struct ClientConfig {
    std::string serverAddress;              // Endereço do servidor
    WORD serverPort;                       // Porta do servidor
    ProtocolType protocol;                 // Protocolo
    bool nonBlocking;                       // Não bloqueante
    DWORD connectTimeoutMs;                // Timeout de conexão (ms)
    DWORD reconnectIntervalMs;             // Intervalo de reconexão (ms)
    DWORD maxReconnectAttempts;            // Máximo de tentativas de reconexão
    
    ClientConfig()
        : serverAddress("127.0.0.1")
        , serverPort(8080)
        , protocol(ProtocolType::TCP)
        , nonBlocking(true)
        , connectTimeoutMs(5000)
        , reconnectIntervalMs(5000)
        , maxReconnectAttempts(5)
    {
    }
};

/**
 * @brief Callback de evento de rede
 */
using NetworkEventCallback = std::function<void(const NetworkEvent&)>;

/**
 * @brief Manipulador de pacote
 */
using PacketHandler = std::function<void(const NetworkPacket&)>;

} // namespace network
} // namespace server
} // namespace wyd

#endif // NETWORKTYPES_H

} // namespace wydbr
