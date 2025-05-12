#ifndef WYDBR_NETWORK_MANAGER_H
#define WYDBR_NETWORK_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>

namespace wydbr {
namespace network {

// Enumeração para estado de conexão
enum class ConnectionState {
    DISCONNECTED,
    CONNECTING,
    AUTHENTICATING,
    CONNECTED,
    RECONNECTING,
    ERROR
};

// Enumeração para tipo de conexão
enum class ConnectionType {
    TCP,
    UDP,
    WEBSOCKET,
    HTTP,
    CUSTOM
};

// Enumeração para nível de compressão
enum class CompressionLevel {
    NONE,
    FAST,
    BALANCED,
    HIGH
};

// Enumeração para nível de encriptação
enum class EncryptionLevel {
    NONE,
    BASIC,
    MEDIUM,
    HIGH
};

// Enumeração para tipo de mensagem
enum class MessageType {
    SYSTEM,
    GAME_STATE,
    PLAYER_ACTION,
    CHAT,
    VOICE,
    SCREEN_SHARE,
    NOTIFICATION,
    HEARTBEAT
};

// Enumeração para prioridade de mensagem
enum class MessagePriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

// Estrutura para configuração de rede
struct NetworkConfig {
    std::string serverAddress;
    int serverPort;
    ConnectionType connectionType;
    bool useCompression;
    CompressionLevel compressionLevel;
    bool useEncryption;
    EncryptionLevel encryptionLevel;
    bool useReconnect;
    int reconnectAttempts;
    int reconnectDelayMs;
    int connectionTimeoutMs;
    int heartbeatIntervalMs;
    bool useBandwidthOptimization;
    bool useDeltaEncoding;
    int sendBufferSizeBytes;
    int receiveBufferSizeBytes;
    int maxPacketSizeBytes;
    bool useNagle;
    bool useMultipleConnections;
    int connectionsCount;
    
    // Construtor com valores padrão
    NetworkConfig() :
        serverPort(8080),
        connectionType(ConnectionType::TCP),
        useCompression(true),
        compressionLevel(CompressionLevel::BALANCED),
        useEncryption(true),
        encryptionLevel(EncryptionLevel::MEDIUM),
        useReconnect(true),
        reconnectAttempts(5),
        reconnectDelayMs(1000),
        connectionTimeoutMs(5000),
        heartbeatIntervalMs(30000),
        useBandwidthOptimization(true),
        useDeltaEncoding(true),
        sendBufferSizeBytes(65536),
        receiveBufferSizeBytes(65536),
        maxPacketSizeBytes(8192),
        useNagle(false),
        useMultipleConnections(false),
        connectionsCount(1)
    {}
};

// Estrutura para mensagem de rede
struct NetworkMessage {
    uint32_t id;
    MessageType type;
    MessagePriority priority;
    std::vector<uint8_t> data;
    uint64_t timestamp;
    bool reliable;
    bool compressed;
    bool encrypted;
    uint32_t sequenceNumber;
    uint32_t ackMask;
    uint16_t fragmentIndex;
    uint16_t fragmentCount;
    std::string channel;
};

// Estrutura para estatísticas de rede
struct NetworkStats {
    int64_t bytesSent;
    int64_t bytesReceived;
    int messagesSent;
    int messagesReceived;
    int messagesLost;
    int reconnectAttempts;
    int connectTime;
    float latency;
    float jitter;
    float packetLoss;
    int currentBandwidthDown;
    int currentBandwidthUp;
    int peakBandwidthDown;
    int peakBandwidthUp;
    int activeConnections;
};

// Callbacks para eventos de rede
using ConnectionStateCallback = std::function<void(ConnectionState state, const std::string& message)>;
using MessageReceivedCallback = std::function<void(const NetworkMessage& message)>;
using ErrorCallback = std::function<void(int errorCode, const std::string& errorMessage)>;
using StatisticsCallback = std::function<void(const NetworkStats& stats)>;
using ReconnectCallback = std::function<void(int attempt, int maxAttempts)>;

/**
 * @class NetworkManager
 * @brief Gerenciador de rede avançado
 * 
 * Sistema responsável pela comunicação de rede, com suporte a reconexão
 * automática, compressão, criptografia e otimização de banda.
 */
class NetworkManager {
public:
    /**
     * @brief Obtém a instância única do sistema (singleton)
     * @return Referência para a instância
     */
    static NetworkManager& getInstance();
    
    /**
     * @brief Inicializa o gerenciador de rede
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const NetworkConfig& config = NetworkConfig());
    
    /**
     * @brief Desliga o gerenciador e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Conecta ao servidor
     * @param serverAddress Endereço do servidor (sobrescreve a configuração)
     * @param serverPort Porta do servidor (sobrescreve a configuração)
     * @param authToken Token de autenticação
     * @return true se a conexão foi iniciada com sucesso
     */
    bool connect(const std::string& serverAddress = "", 
                int serverPort = 0,
                const std::string& authToken = "");
    
    /**
     * @brief Desconecta do servidor
     */
    void disconnect();
    
    /**
     * @brief Verifica se está conectado
     * @return true se conectado
     */
    bool isConnected() const;
    
    /**
     * @brief Obtém o estado atual da conexão
     * @return Estado da conexão
     */
    ConnectionState getConnectionState() const;
    
    /**
     * @brief Envia uma mensagem
     * @param type Tipo da mensagem
     * @param data Dados da mensagem
     * @param priority Prioridade da mensagem
     * @param reliable Se a mensagem deve ser enviada de forma confiável
     * @param channel Canal para a mensagem (para multiplexação)
     * @return ID da mensagem ou 0 se falhou
     */
    uint32_t sendMessage(MessageType type, 
                        const std::vector<uint8_t>& data,
                        MessagePriority priority = MessagePriority::NORMAL,
                        bool reliable = true,
                        const std::string& channel = "default");
    
    /**
     * @brief Envia uma mensagem de texto
     * @param text Texto da mensagem
     * @param type Tipo da mensagem
     * @param priority Prioridade da mensagem
     * @param reliable Se a mensagem deve ser enviada de forma confiável
     * @param channel Canal para a mensagem (para multiplexação)
     * @return ID da mensagem ou 0 se falhou
     */
    uint32_t sendTextMessage(const std::string& text,
                            MessageType type = MessageType::CHAT,
                            MessagePriority priority = MessagePriority::NORMAL,
                            bool reliable = true,
                            const std::string& channel = "default");
    
    /**
     * @brief Cria um canal de mensagens
     * @param channelName Nome do canal
     * @param reliable Se o canal deve ser confiável por padrão
     * @return true se o canal foi criado com sucesso
     */
    bool createChannel(const std::string& channelName, bool reliable = true);
    
    /**
     * @brief Remove um canal de mensagens
     * @param channelName Nome do canal
     * @return true se o canal foi removido com sucesso
     */
    bool removeChannel(const std::string& channelName);
    
    /**
     * @brief Obtém a lista de canais
     * @return Lista de nomes de canais
     */
    std::vector<std::string> getChannels() const;
    
    /**
     * @brief Define o nível de compressão
     * @param level Novo nível
     */
    void setCompressionLevel(CompressionLevel level);
    
    /**
     * @brief Obtém o nível de compressão atual
     * @return Nível de compressão
     */
    CompressionLevel getCompressionLevel() const;
    
    /**
     * @brief Define o nível de encriptação
     * @param level Novo nível
     */
    void setEncryptionLevel(EncryptionLevel level);
    
    /**
     * @brief Obtém o nível de encriptação atual
     * @return Nível de encriptação
     */
    EncryptionLevel getEncryptionLevel() const;
    
    /**
     * @brief Ativa ou desativa a reconexão automática
     * @param enable Se deve ativar
     */
    void enableReconnect(bool enable);
    
    /**
     * @brief Verifica se a reconexão automática está ativa
     * @return true se ativa
     */
    bool isReconnectEnabled() const;
    
    /**
     * @brief Define o número de tentativas de reconexão
     * @param attempts Número de tentativas
     */
    void setReconnectAttempts(int attempts);
    
    /**
     * @brief Obtém o número de tentativas de reconexão
     * @return Número de tentativas
     */
    int getReconnectAttempts() const;
    
    /**
     * @brief Define o intervalo de reconexão
     * @param delayMs Intervalo em milissegundos
     */
    void setReconnectDelay(int delayMs);
    
    /**
     * @brief Obtém o intervalo de reconexão
     * @return Intervalo em milissegundos
     */
    int getReconnectDelay() const;
    
    /**
     * @brief Ativa ou desativa a otimização de banda
     * @param enable Se deve ativar
     */
    void enableBandwidthOptimization(bool enable);
    
    /**
     * @brief Verifica se a otimização de banda está ativa
     * @return true se ativa
     */
    bool isBandwidthOptimizationEnabled() const;
    
    /**
     * @brief Ativa ou desativa a codificação delta
     * @param enable Se deve ativar
     */
    void enableDeltaEncoding(bool enable);
    
    /**
     * @brief Verifica se a codificação delta está ativa
     * @return true se ativa
     */
    bool isDeltaEncodingEnabled() const;
    
    /**
     * @brief Obtém estatísticas de rede
     * @return Estatísticas de rede
     */
    NetworkStats getStatistics() const;
    
    /**
     * @brief Registra um callback para evento de alteração de estado de conexão
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerConnectionStateCallback(ConnectionStateCallback callback);
    
    /**
     * @brief Remove um callback de evento de alteração de estado de conexão
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterConnectionStateCallback(int callbackId);
    
    /**
     * @brief Registra um callback para evento de recebimento de mensagem
     * @param callback Função a ser chamada
     * @param filter Filtro de tipo de mensagem (opcional)
     * @return ID do callback (para remoção)
     */
    int registerMessageCallback(MessageReceivedCallback callback, 
                              MessageType filter = static_cast<MessageType>(-1));
    
    /**
     * @brief Remove um callback de evento de recebimento de mensagem
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterMessageCallback(int callbackId);
    
    /**
     * @brief Registra um callback para evento de erro
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerErrorCallback(ErrorCallback callback);
    
    /**
     * @brief Remove um callback de evento de erro
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterErrorCallback(int callbackId);
    
    /**
     * @brief Registra um callback para evento de estatísticas
     * @param callback Função a ser chamada
     * @param intervalMs Intervalo de atualização em milissegundos
     * @return ID do callback (para remoção)
     */
    int registerStatisticsCallback(StatisticsCallback callback, int intervalMs = 1000);
    
    /**
     * @brief Remove um callback de evento de estatísticas
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterStatisticsCallback(int callbackId);
    
    /**
     * @brief Registra um callback para evento de reconexão
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerReconnectCallback(ReconnectCallback callback);
    
    /**
     * @brief Remove um callback de evento de reconexão
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterReconnectCallback(int callbackId);
    
    /**
     * @brief Obtém o ping atual
     * @return Tempo de ping em milissegundos
     */
    float getPing() const;
    
    /**
     * @brief Obtém o jitter atual
     * @return Jitter em milissegundos
     */
    float getJitter() const;
    
    /**
     * @brief Obtém a perda de pacotes atual
     * @return Perda de pacotes em porcentagem
     */
    float getPacketLoss() const;
    
    /**
     * @brief Obtém a banda atual de download
     * @return Banda em bytes por segundo
     */
    int getDownloadBandwidth() const;
    
    /**
     * @brief Obtém a banda atual de upload
     * @return Banda em bytes por segundo
     */
    int getUploadBandwidth() const;
    
    /**
     * @brief Configura banda máxima de download
     * @param bytesPerSecond Máximo de bytes por segundo (0 = ilimitado)
     */
    void setMaxDownloadBandwidth(int bytesPerSecond);
    
    /**
     * @brief Configura banda máxima de upload
     * @param bytesPerSecond Máximo de bytes por segundo (0 = ilimitado)
     */
    void setMaxUploadBandwidth(int bytesPerSecond);
    
    /**
     * @brief Envia um ping para o servidor
     * @return true se o ping foi enviado com sucesso
     */
    bool sendPing();
    
private:
    // Construtor e destrutor privados (singleton)
    NetworkManager();
    ~NetworkManager();
    
    // Prevenir cópia (singleton)
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    
    // Implementação privada (Pimpl idiom)
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Aliases para facilitar o uso
using NetManager = NetworkManager;

} // namespace network
} // namespace wydbr

#endif // WYDBR_NETWORK_MANAGER_H
