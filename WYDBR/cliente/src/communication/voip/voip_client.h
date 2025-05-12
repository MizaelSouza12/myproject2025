#ifndef WYDBR_VOIP_CLIENT_H
#define WYDBR_VOIP_CLIENT_H

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <memory>

namespace wydbr {
namespace communication {

// Enumeração para estado de conexão
enum class ConnectionState {
    DISCONNECTED,
    CONNECTING,
    AUTHENTICATING,
    CONNECTED,
    RECONNECTING,
    ERROR
};

// Enumeração para tipos de pacotes
enum class PacketType {
    AUDIO_DATA,
    CONTROL,
    PRESENCE,
    METADATA,
    HANDSHAKE,
    HEARTBEAT,
    ERROR
};

// Estrutura de configuração do cliente
struct VoIPClientConfig {
    std::string serverAddress;
    int serverPort;
    int reconnectAttempts;
    int reconnectDelayMs;
    int heartbeatIntervalMs;
    int timeoutMs;
    bool useSecureConnection;
    bool enablePacketAggregation;
    bool enablePacketLossRecovery;
    bool enableBandwidthAdaptation;
    int bufferSizeMs;
    
    // Construtor com valores padrão
    VoIPClientConfig() :
        serverPort(8090),
        reconnectAttempts(5),
        reconnectDelayMs(2000),
        heartbeatIntervalMs(5000),
        timeoutMs(10000),
        useSecureConnection(true),
        enablePacketAggregation(true),
        enablePacketLossRecovery(true),
        enableBandwidthAdaptation(true),
        bufferSizeMs(100)
    {}
};

// Estrutura de pacote de áudio
struct AudioPacket {
    uint32_t timestamp;
    uint32_t sequenceNumber;
    std::vector<uint8_t> data;
    std::string userId;
    uint8_t codec;
    uint8_t channels;
    uint16_t sampleRate;
    uint8_t frameSize;
};

// Callbacks
using ConnectionStateCallback = std::function<void(ConnectionState state, const std::string& message)>;
using AudioPacketCallback = std::function<void(const AudioPacket& packet)>;
using ErrorCallback = std::function<void(int errorCode, const std::string& errorMessage)>;
using UserPresenceCallback = std::function<void(const std::string& userId, bool isPresent)>;

/**
 * @class VoIPClient
 * @brief Cliente de rede para comunicação VOIP
 * 
 * Gerencia a conexão com o servidor VOIP, envio e recebimento de pacotes,
 * autenticação, e reconexão automática.
 */
class VoIPClient {
public:
    VoIPClient();
    ~VoIPClient();
    
    /**
     * @brief Inicializa o cliente VOIP
     * @param config Configuração do cliente
     * @return true se inicializado com sucesso
     */
    bool initialize(const VoIPClientConfig& config = VoIPClientConfig());
    
    /**
     * @brief Desliga o cliente e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Conecta ao servidor VOIP
     * @param username Nome de usuário
     * @param authToken Token de autenticação
     * @return true se a conexão foi iniciada com sucesso
     */
    bool connect(const std::string& username, const std::string& authToken);
    
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
     * @brief Envia um pacote de áudio
     * @param packet Pacote de áudio
     * @return true se enviado com sucesso
     */
    bool sendAudioPacket(const AudioPacket& packet);
    
    /**
     * @brief Entra em um canal
     * @param channelId ID do canal
     * @return true se a solicitação foi enviada com sucesso
     */
    bool joinChannel(const std::string& channelId);
    
    /**
     * @brief Sai de um canal
     * @param channelId ID do canal
     * @return true se a solicitação foi enviada com sucesso
     */
    bool leaveChannel(const std::string& channelId);
    
    /**
     * @brief Obtém a lista de canais disponíveis
     * @return Lista de IDs de canais
     */
    std::vector<std::string> getAvailableChannels();
    
    /**
     * @brief Obtém a lista de usuários em um canal
     * @param channelId ID do canal
     * @return Lista de IDs de usuários
     */
    std::vector<std::string> getUsersInChannel(const std::string& channelId);
    
    /**
     * @brief Envia dados de posição espacial
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param dirX Direção X
     * @param dirY Direção Y
     * @param dirZ Direção Z
     * @return true se enviado com sucesso
     */
    bool sendSpatialData(float posX, float posY, float posZ,
                         float dirX, float dirY, float dirZ);
    
    /**
     * @brief Define o estado de silêncio do microfone
     * @param muted Se está silenciado
     * @return true se o estado foi alterado com sucesso
     */
    bool setMute(bool muted);
    
    /**
     * @brief Define o estado de surdez (não ouvir outros)
     * @param deafened Se está surdo
     * @return true se o estado foi alterado com sucesso
     */
    bool setDeafen(bool deafened);
    
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
     * @brief Registra um callback para evento de recebimento de pacote de áudio
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerAudioPacketCallback(AudioPacketCallback callback);
    
    /**
     * @brief Remove um callback de evento de recebimento de pacote de áudio
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterAudioPacketCallback(int callbackId);
    
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
     * @brief Registra um callback para evento de presença de usuário
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerUserPresenceCallback(UserPresenceCallback callback);
    
    /**
     * @brief Remove um callback de evento de presença de usuário
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterUserPresenceCallback(int callbackId);
    
    /**
     * @brief Obtém estatísticas de rede
     * @param outPacketLossRate Taxa de perda de pacotes (saída)
     * @param outLatency Latência em ms (saída)
     * @param outJitter Jitter em ms (saída)
     * @param outBitrate Taxa de bits em kbps (saída)
     */
    void getNetworkStats(float& outPacketLossRate, int& outLatency,
                         float& outJitter, int& outBitrate) const;
    
    /**
     * @brief Define o tamanho do buffer de jitter
     * @param sizeMs Tamanho em milissegundos
     */
    void setJitterBufferSize(int sizeMs);
    
    /**
     * @brief Obtém o tamanho atual do buffer de jitter
     * @return Tamanho em milissegundos
     */
    int getJitterBufferSize() const;
    
    /**
     * @brief Envia um ping para o servidor
     * @return true se o ping foi enviado com sucesso
     */
    bool sendPing();
    
    /**
     * @brief Obtém o ping atual
     * @return Tempo de ping em milissegundos ou -1 se não disponível
     */
    int getPing() const;

private:
    // Estrutura para armazenar um pacote genérico
    struct NetworkPacket {
        PacketType type;
        std::vector<uint8_t> data;
        uint32_t timestamp;
        uint32_t sequenceNumber;
    };
    
    // Processamento de rede e pacotes
    void networkThread();
    void processIncomingPackets();
    void handleConnectionStateChange(ConnectionState newState, const std::string& message);
    void handleAudioPacket(const std::vector<uint8_t>& data);
    void handleControlPacket(const std::vector<uint8_t>& data);
    void handlePresencePacket(const std::vector<uint8_t>& data);
    void handleErrorPacket(const std::vector<uint8_t>& data);
    void sendHeartbeat();
    void attemptReconnect();
    
    // Empacotamento e desempacotamento
    std::vector<uint8_t> packAudioPacket(const AudioPacket& packet);
    AudioPacket unpackAudioPacket(const std::vector<uint8_t>& data);
    std::vector<uint8_t> packControlPacket(uint8_t controlType, const std::vector<uint8_t>& payload);
    
    // Utilitários
    uint32_t getTimestamp();
    uint32_t generateSequenceNumber();
    
    // Estado
    bool m_initialized;
    std::atomic<ConnectionState> m_connectionState;
    VoIPClientConfig m_config;
    std::string m_username;
    std::string m_authToken;
    std::string m_currentChannel;
    std::atomic<bool> m_muted;
    std::atomic<bool> m_deafened;
    
    // Estatísticas de rede
    struct NetworkStats {
        std::atomic<float> packetLossRate;
        std::atomic<int> latency;
        std::atomic<float> jitter;
        std::atomic<int> bitrate;
        std::atomic<int> ping;
        std::vector<int> pingHistory;
        mutable std::mutex statsMutex;
    } m_stats;
    
    // Buffer de jitter
    int m_jitterBufferSize;
    
    // Thread de rede
    std::unique_ptr<std::thread> m_networkThread;
    std::atomic<bool> m_threadsShouldRun;
    
    // Filas de pacotes
    std::queue<NetworkPacket> m_incomingPackets;
    std::queue<NetworkPacket> m_outgoingPackets;
    mutable std::mutex m_incomingQueueMutex;
    mutable std::mutex m_outgoingQueueMutex;
    
    // Callbacks
    struct Callbacks {
        std::vector<std::pair<int, ConnectionStateCallback>> connectionState;
        std::vector<std::pair<int, AudioPacketCallback>> audioPacket;
        std::vector<std::pair<int, ErrorCallback>> error;
        std::vector<std::pair<int, UserPresenceCallback>> userPresence;
        int nextCallbackId;
        mutable std::mutex mutex;
    } m_callbacks;
    
    // Implementação de rede
    std::unique_ptr<class WebSocketClient> m_socket;
    
    // Contadores
    uint32_t m_nextSequenceNumber;
};

} // namespace communication
} // namespace wydbr

#endif // WYDBR_VOIP_CLIENT_H
