/**
 * NetworkManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/network/NetworkManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "WYDPackets.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>

namespace wydbr {


/**
 * @file NetworkManager.h
 * @brief Gerenciador de rede do WYD
 * 
 * Este arquivo implementa o gerenciador de rede do WYD, responsável pela
 * comunicação entre cliente e servidor. Implementa o protocolo de rede do WYD
 * com melhorias de segurança e performance.
 */

namespace wyd {
namespace net {

// Forward declarations
class PacketProcessor;
class PacketHandler;
class SecurityManager;
class Connection;

// Tipos de callbacks para tratamento de pacotes
using PacketCallback = std::function<void(std::shared_ptr<Connection>, const void*, size_t)>;
using ConnectionCallback = std::function<void(std::shared_ptr<Connection>)>;
using ErrorCallback = std::function<void(std::shared_ptr<Connection>, int, const std::string&)>;

// Direção do tráfego de rede
enum /**
 * Classe WYDDirection
 * 
 * Esta classe implementa a funcionalidade WYDDirection conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDDirection
 * 
 * Esta classe implementa a funcionalidade WYDDirection conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Direction {
    INCOMING,   // Tráfego entrando (recebido)
    OUTGOING    // Tráfego saindo (enviado)
};

// Estatísticas de rede
struct NetworkStats {
    uint64_t bytesReceived;      // Total de bytes recebidos
    uint64_t bytesSent;          // Total de bytes enviados
    uint64_t packetsReceived;    // Total de pacotes recebidos
    uint64_t packetsSent;        // Total de pacotes enviados
    uint64_t invalidPackets;     // Pacotes inválidos recebidos
    uint64_t droppedPackets;     // Pacotes descartados (buffer cheio)
    uint64_t compressedBytes;    // Bytes comprimidos
    uint64_t uncompressedBytes;  // Bytes descomprimidos
    uint64_t connectionAttempts; // Tentativas de conexão
    uint64_t successfulConnections; // Conexões bem-sucedidas
    uint64_t failedConnections;  // Conexões /**
 * NetworkStats
 * 
 * Implementa a funcionalidade NetworkStats conforme especificação original.
 * @return Retorna falhas
 */
 falhas
    
    NetworkStats(){
        reset();
    }
    
    /**
 * reset
 * 
 * Implementa a funcionalidade reset conforme especificação original.
 */

    
    void reset(){
        bytesReceived = 0;
        bytesSent = 0;
        packetsReceived = 0;
        packetsSent = 0;
        invalidPackets = 0;
        droppedPackets = 0;
        compressedBytes = 0;
        uncompressedBytes = 0;
        connectionAttempts = 0;
        successfulConnections = 0;
        failedConnections = 0;
    }
};

// Configurações de rede
struct NetworkConfig {
    uint16_t port;                 // Porta de escuta (servidor) ou conexão (cliente)
    std::string address;           // Endereço de conexão (cliente) ou binding (servidor)
    bool useCompression;           // Usar compressão de dados
    bool useEncryption;            // Usar criptografia
    bool validateChecksum;         // Validar checksums de pacotes
    uint32_t maxConnections;       // Máximo de conexões simultâneas
    uint32_t receiveBufferSize;    // Tamanho do buffer de recepção
    uint32_t sendBufferSize;       // Tamanho do buffer de envio
    uint32_t packetQueueSize;      // Tamanho da fila de pacotes
    uint32_t heartbeatInterval;    // Intervalo de heartbeat em milissegundos
    uint32_t connectionTimeout;    // Timeout de conexão em milissegundos
    
    NetworkConfig() 
        : port(8281)                // Porta padrão do WYD
        , address("0.0.0.0")        // Bind em todas as interfaces
        , useCompression(true)
        , useEncryption(true)
        , validateChecksum(true)
        , maxConnections(1000)
        , receiveBufferSize(8192)
        , sendBufferSize(8192)
        , packetQueueSize(1000)
        , heartbeatInterval(30000)  // 30 segundos
        , connectionTimeout(60000)  // 60 segundos
    {
    }
};

// Estados de conexão
enum /**
 * Classe WYDConnectionState
 * 
 * Esta classe implementa a funcionalidade WYDConnectionState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDConnectionState
 * 
 * Esta classe implementa a funcionalidade WYDConnectionState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ConnectionState {
    DISCONNECTED,    // Desconectado
    CONNECTING,      // Conectando
    CONNECTED,       // Conectado (fase inicial)
    AUTHENTICATING,  // Autenticando
    AUTHENTICATED,   // Autenticado
    INGAME,          // Em jogo
    DISCONNECTING    // Desconectando
};

// Classe que representa uma conexão (socket)
class Connection : public std::enable_shared_from_this<Connection> {
public:
    // Constructor//**
 * Connection
 * 
 * Implementa a funcionalidade Connection conforme especificação original.
 * @param socket Parâmetro socket
 * @param address Parâmetro address
 * @param port Parâmetro port
 * @return Retorna Destructor
 */
Destructor
    Connection(int socket, const std::string& address, uint16_t port);
    ~Connection();
    
    // Getters/Setters
    /**
 * getSocket
 * 
 * Implementa a funcionalidade getSocket conforme especificação original.
 * @return Retorna int
 */

    int getSocket() const{ return socket_; }
    const std::string& getAddress() const { return address_; }
    /**
 * getPort
 * 
 * Implementa a funcionalidade getPort conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getPort() const{ return port_; }
    /**
 * getId
 * 
 * Implementa a funcionalidade getId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getId() const{ return id_; }
    /**
 * getState
 * 
 * Implementa a funcionalidade getState conforme especificação original.
 * @return Retorna ConnectionState
 */

    ConnectionState getState() const{ return state_; }
    /**
 * setState
 * 
 * Implementa a funcionalidade setState conforme especificação original.
 * @param state Parâmetro state
 */

    void setState(ConnectionState state){ state_ = state; }
    /**
 * getLastActivity
 * 
 * Implementa a funcionalidade getLastActivity conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getLastActivity() const{ return lastActivity_; }
    /**
 * updateLastActivity
 * 
 * Implementa a funcionalidade updateLastActivity conforme especificação original.
 */

    void updateLastActivity(){ lastActivity_ = getCurrentTime(); }
    
    // Métodos principais
    /**
 * send
 * 
 * Implementa a funcionalidade send conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool send(const void* data, size_t size);
    /**
 * receive
 * 
 * Implementa a funcionalidade receive conforme especificação original.
 * @param buffer Parâmetro buffer
 * @param size Parâmetro size
 * @param bytesRead Parâmetro bytesRead
 * @return Retorna bool
 */

    bool receive(void* buffer, size_t size, size_t& bytesRead);
    /**
 * close
 * 
 * Implementa a funcionalidade close conforme especificação original.
 */

    void close();
    /**
 * isConnected
 * 
 * Implementa a funcionalidade isConnected conforme especificação original.
 * @return Retorna bool
 */

    bool isConnected() const;
    
    // Métodos de criptografia
    /**
 * setSecurityKey
 * 
 * Implementa a funcionalidade setSecurityKey conforme especificação original.
 * @param key Parâmetro key
 * @param keySize Parâmetro keySize
 */

    void setSecurityKey(const uint8_t* key, size_t keySize);
    /**
 * enableEncryption
 * 
 * Implementa a funcionalidade enableEncryption conforme especificação original.
 * @param enable Parâmetro enable
 */

    void enableEncryption(bool enable){ useEncryption_ = enable; }
    /**
 * isEncryptionEnabled
 * 
 * Implementa a funcionalidade isEncryptionEnabled conforme especificação original.
 * @return Retorna bool
 */

    bool isEncryptionEnabled() const{ return useEncryption_; }
    
    // Funções de pacote
    template<typename T>
    /**
 * sendPacket
 * 
 * Implementa a funcionalidade sendPacket conforme especificação original.
 * @param packet Parâmetro packet
 * @return Retorna bool
 */

    bool sendPacket(const T& packet){
        return send(&packet, sizeof(T));
    }
    
    // Propriedades customizadas (para uso pelo jogo)
    /**
 * setProperty
 * 
 * Implementa a funcionalidade setProperty conforme especificação original.
 * @param key Parâmetro key
 * @param value Parâmetro value
 */

    void setProperty(const std::string& key, const std::string& value);
    /**
 * getProperty
 * 
 * Implementa a funcionalidade getProperty conforme especificação original.
 * @param key Parâmetro key
 * @return Retorna std::string
 */

    std::string getProperty(const std::string& key) const;
    /**
 * removeProperty
 * 
 * Implementa a funcionalidade removeProperty conforme especificação original.
 * @param key Parâmetro key
 */

    void removeProperty(const std::string& key);
    
    // Estatísticas de conexão
    /**
 * getBytesSent
 * 
 * Implementa a funcionalidade getBytesSent conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getBytesSent() const{ return bytesSent_; }
    /**
 * getBytesReceived
 * 
 * Implementa a funcionalidade getBytesReceived conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getBytesReceived() const{ return bytesReceived_; }
    /**
 * getPacketsSent
 * 
 * Implementa a funcionalidade getPacketsSent conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getPacketsSent() const{ return packetsSent_; }
    /**
 * getPacketsReceived
 * 
 * Implementa a funcionalidade getPacketsReceived conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getPacketsReceived() const{ return packetsReceived_; }
    
private:
    // Método auxiliar para obter tempo atual em milissegundos
    /**
 * getCurrentTime
 * 
 * Implementa a funcionalidade getCurrentTime conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getCurrentTime() const;
    
    // Atributos da conexão
    int socket_;                   // Socket descriptor
    std::string address_;          // Endereço IP
    uint16_t port_;                // Porta
    uint32_t id_;                  // ID único da conexão
    ConnectionState state_;        // Estado da conexão
    uint64_t lastActivity_;        // Timestamp da última atividade
    bool useEncryption_;           // Se usa criptografia
    
    // Buffers de envio/recepção
    std::vector<uint8_t> sendBuffer_;
    std::vector<uint8_t> receiveBuffer_;
    
    // Estatísticas
    uint64_t bytesSent_;
    uint64_t bytesReceived_;
    uint64_t packetsSent_;
    uint64_t packetsReceived_;
    
    // Propriedades customizadas
    std::unordered_map<std::string, std::string> properties_;
    
    // Chave de segurança
    std::vector<uint8_t> securityKey_;
    
    // Mutex para sincronização
    mutable std::mutex mutex_;
    
    // ID do próximo socket
    static std::atomic<uint32_t> nextId_;
};

// Processador de pacotes
/**
 * Classe WYDPacketProcessor
 * 
 * Esta classe implementa a funcionalidade WYDPacketProcessor conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDPacketProcessor
 * 
 * Esta classe implementa a funcionalidade WYDPacketProcessor conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PacketProcessor {
public:
    // Constructor//**
 * PacketProcessor
 * 
 * Implementa a funcionalidade PacketProcessor conforme especificação original.
 * @return Retorna Destructor
 */
Destructor
    PacketProcessor();
    ~PacketProcessor();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @param config Parâmetro config
 * @return Retorna bool
 */

    bool initialize(const NetworkConfig& config);
    /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 */

    void shutdown();
    
    // Registro de handlers
    /**
 * registerHandler
 * 
 * Implementa a funcionalidade registerHandler conforme especificação original.
 * @param packetType Parâmetro packetType
 * @param callback Parâmetro callback
 */

    void registerHandler(WORD packetType, PacketCallback callback);
    /**
 * unregisterHandler
 * 
 * Implementa a funcionalidade unregisterHandler conforme especificação original.
 * @param packetType Parâmetro packetType
 */

    void unregisterHandler(WORD packetType);
    
    // Processamento de pacotes
    /**
 * processPacket
 * 
 * Implementa a funcionalidade processPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool processPacket(std::shared_ptr<Connection> connection, const void* data, size_t size);
    
    // Estatísticas
    /**
 * getProcessedPackets
 * 
 * Implementa a funcionalidade getProcessedPackets conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getProcessedPackets() const{ return processedPackets_; }
    /**
 * getInvalidPackets
 * 
 * Implementa a funcionalidade getInvalidPackets conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getInvalidPackets() const{ return invalidPackets_; }
    
private:
    // Validação de pacotes
    /**
 * validatePacket
 * 
 * Implementa a funcionalidade validatePacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool validatePacket(const void* data, size_t size);
    
    // Handlers de pacotes
    std::unordered_map<WORD, PacketCallback> handlers_;
    
    // Configuração
    NetworkConfig config_;
    
    // Estatísticas
    std::atomic<uint64_t> processedPackets_;
    std::atomic<uint64_t> invalidPackets_;
    
    // Flag de inicialização
    bool initialized_;
    
    // Mutex para sincronização
    mutable std::mutex mutex_;
};

// Gerenciador de rede
/**
 * Classe WYDNetworkManager
 * 
 * Esta classe implementa a funcionalidade WYDNetworkManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDNetworkManager
 * 
 * Esta classe implementa a funcionalidade WYDNetworkManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class NetworkManager {
public:
    // Singleton
    static NetworkManager& getInstance();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @param config Parâmetro config
 * @return Retorna bool
 */

    bool initialize(const NetworkConfig& config);
    /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 */

    void shutdown();
    
    // Gerenciamento de conexões
    /**
 * start
 * 
 * Implementa a funcionalidade start conforme especificação original.
 * @return Retorna bool
 */

    bool start(); // Inicia o servidor de rede
    /**
 * stop
 * 
 * Implementa a funcionalidade stop conforme especificação original.
 * @return Retorna bool
 */

    bool stop();  // Para o servidor de rede
    /**
 * connect
 * 
 * Implementa a funcionalidade connect conforme especificação original.
 * @param address Parâmetro address
 * @param port Parâmetro port
 * @return Retorna bool
 */

    bool connect(const std::string& address, uint16_t port); // Para modo cliente
    /**
 * disconnect
 * 
 * Implementa a funcionalidade disconnect conforme especificação original.
 */

    void disconnect(); // Para modo cliente
    
    // Envio/recepção de pacotes
    template<typename T>
    /**
 * sendPacket
 * 
 * Implementa a funcionalidade sendPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna bool
 */

    bool sendPacket(std::shared_ptr<Connection> connection, const T& packet){
        return connection && connection->sendPacket(packet);
    }
    
    template<typename T>
    bool broadcastPacket(const T& packet, std::function<bool(std::shared_ptr<Connection>)> filter = nullptr) {
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        
        for (auto& conn : connections_) {
            if (conn && conn->isConnected() && (!filter || filter(conn))) {
                conn->sendPacket(packet);
            }
        }
        
        return true;
    }
    
    // Registro de handlers
    /**
 * registerHandler
 * 
 * Implementa a funcionalidade registerHandler conforme especificação original.
 * @param packetType Parâmetro packetType
 * @param callback Parâmetro callback
 */

    void registerHandler(WORD packetType, PacketCallback callback);
    /**
 * setConnectionCallback
 * 
 * Implementa a funcionalidade setConnectionCallback conforme especificação original.
 * @param callback Parâmetro callback
 */

    void setConnectionCallback(ConnectionCallback callback);
    /**
 * setDisconnectionCallback
 * 
 * Implementa a funcionalidade setDisconnectionCallback conforme especificação original.
 * @param callback Parâmetro callback
 */

    void setDisconnectionCallback(ConnectionCallback callback);
    /**
 * setErrorCallback
 * 
 * Implementa a funcionalidade setErrorCallback conforme especificação original.
 * @param callback Parâmetro callback
 */

    void setErrorCallback(ErrorCallback callback);
    
    // Gerenciamento de conexões
    std::shared_ptr<Connection> getConnection(uint32_t id);
    std::vector<std::shared_ptr<Connection>> getConnections();
    /**
 * closeConnection
 * 
 * Implementa a funcionalidade closeConnection conforme especificação original.
 * @param id Parâmetro id
 */

    void closeConnection(uint32_t id);
    /**
 * closeAllConnections
 * 
 * Implementa a funcionalidade closeAllConnections conforme especificação original.
 */

    void closeAllConnections();
    
    // Estatísticas
    /**
 * getStats
 * 
 * Implementa a funcionalidade getStats conforme especificação original.
 * @return Retorna NetworkStats
 */

    NetworkStats getStats() const;
    /**
 * resetStats
 * 
 * Implementa a funcionalidade resetStats conforme especificação original.
 */

    void resetStats();
    
private:
    // Construtor/Destrutor (privados para singleton)
    NetworkManager();
    ~NetworkManager();
    
    // Thread principal de processamento de rede
    /**
 * networkThread
 * 
 * Implementa a funcionalidade networkThread conforme especificação original.
 */

    void networkThread();
    
    // Thread de processamento de pacotes
    /**
 * packetProcessingThread
 * 
 * Implementa a funcionalidade packetProcessingThread conforme especificação original.
 */

    void packetProcessingThread();
    
    // Thread de manutenção de conexões
    /**
 * connectionMaintenanceThread
 * 
 * Implementa a funcionalidade connectionMaintenanceThread conforme especificação original.
 */

    void connectionMaintenanceThread();
    
    // Handlers internos
    /**
 * handleNewConnection
 * 
 * Implementa a funcionalidade handleNewConnection conforme especificação original.
 * @param clientSocket Parâmetro clientSocket
 * @param clientAddress Parâmetro clientAddress
 * @param clientPort Parâmetro clientPort
 */

    void handleNewConnection(int clientSocket, const std::string& clientAddress, uint16_t clientPort);
    /**
 * handleDisconnection
 * 
 * Implementa a funcionalidade handleDisconnection conforme especificação original.
 * @param connection Parâmetro connection
 */

    void handleDisconnection(std::shared_ptr<Connection> connection);
    /**
 * handleError
 * 
 * Implementa a funcionalidade handleError conforme especificação original.
 * @param connection Parâmetro connection
 * @param errorCode Parâmetro errorCode
 * @param errorMessage Parâmetro errorMessage
 */

    void handleError(std::shared_ptr<Connection> connection, int errorCode, const std::string& errorMessage);
    
    // Configuração
    NetworkConfig config_;
    
    // Socket do servidor
    int serverSocket_;
    
    // Lista de conexões
    std::vector<std::shared_ptr<Connection>> connections_;
    mutable std::mutex connectionsMutex_;
    
    // Fila de pacotes para processamento assíncrono
    struct QueuedPacket {
        std::shared_ptr<Connection> connection;
        std::vector<uint8_t> data;
    };
    std::queue<QueuedPacket> packetQueue_;
    std::mutex packetQueueMutex_;
    std::condition_variable packetQueueCondition_;
    
    // Threads de trabalho
    std::thread networkThread_;
    std::thread packetThread_;
    std::thread maintenanceThread_;
    
    // Sinais de controle
    std::atomic<bool> running_;
    std::atomic<bool> stopping_;
    
    // Processador de pacotes
    std::unique_ptr<PacketProcessor> packetProcessor_;
    
    // Callbacks
    ConnectionCallback connectionCallback_;
    ConnectionCallback disconnectionCallback_;
    ErrorCallback errorCallback_;
    
    // Estatísticas
    NetworkStats stats_;
    mutable std::mutex statsMutex_;
    
    // Outros membros
    std::atomic<uint32_t> nextConnectionId_;
};

// Alguns tipos úteis para melhorar legibilidade
using ConnectionPtr = std::shared_ptr<Connection>;
using ConnectionList = std::vector<ConnectionPtr>;

} // namespace net
} // namespace wyd

#endif // NETWORK_MANAGER_H

} // namespace wydbr
