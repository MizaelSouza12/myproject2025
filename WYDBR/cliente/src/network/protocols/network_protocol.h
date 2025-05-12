#ifndef WYDBR_NETWORK_PROTOCOL_H
#define WYDBR_NETWORK_PROTOCOL_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>
#include <chrono>

namespace wydbr {
namespace network {

// Enumeração para tipo de pacote
enum class PacketType : uint8_t {
    HANDSHAKE = 0,
    HANDSHAKE_RESPONSE = 1,
    DISCONNECT = 2,
    PING = 3,
    PONG = 4,
    HEARTBEAT = 5,
    ACKNOWLEDGMENT = 6,
    DATA = 7,
    FRAGMENT = 8,
    FRAGMENT_END = 9,
    ERROR = 10,
    RESET = 11,
    ENCRYPTED = 12,
    COMPRESSED = 13,
    CONNECTION_QUALITY = 14,
    CHANNEL_OPEN = 15,
    CHANNEL_CLOSE = 16,
    RECONNECT = 17,
    RECONNECT_RESPONSE = 18,
    SESSION_DATA = 19,
    CONFIG_UPDATE = 20,
    RELIABLE_DATA = 21
};

// Enumeração para flags de pacote
enum class PacketFlag : uint8_t {
    NONE = 0,
    RELIABLE = (1 << 0),
    ORDERED = (1 << 1),
    ENCRYPTED = (1 << 2),
    COMPRESSED = (1 << 3),
    HIGH_PRIORITY = (1 << 4),
    CRITICAL = (1 << 5),
    FRAGMENTED = (1 << 6),
    CUSTOM = (1 << 7)
};

// Estrutura para configuração do protocolo
struct ProtocolConfig {
    uint16_t maxPacketSize;
    uint16_t fragmentSize;
    uint16_t connectionTimeoutMs;
    uint16_t heartbeatIntervalMs;
    uint8_t maxReliableResends;
    uint8_t maxOrderedMessages;
    uint16_t resendIntervalMs;
    uint8_t protocolVersion;
    bool useEncryption;
    bool useCompression;
    std::string encryptionKey;
    
    // Construtor com valores padrão
    ProtocolConfig() :
        maxPacketSize(1400),
        fragmentSize(1200),
        connectionTimeoutMs(5000),
        heartbeatIntervalMs(1000),
        maxReliableResends(5),
        maxOrderedMessages(64),
        resendIntervalMs(100),
        protocolVersion(1),
        useEncryption(true),
        useCompression(true)
    {}
};

// Cabeçalho de pacote
#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;         // Assinatura mágica (0x57594442) = 'WYDB'
    uint8_t version;        // Versão do protocolo
    PacketType type;        // Tipo do pacote
    uint8_t flags;          // Flags (bit field das PacketFlag)
    uint16_t channel;       // Canal do pacote
    uint32_t sequenceNumber; // Número de sequência
    uint32_t ackSequence;   // Último número de sequência recebido
    uint32_t ackBitfield;   // Bitfield de acks (32 pacotes anteriores)
    uint16_t size;          // Tamanho do payload (sem contar cabeçalho)
    uint16_t fragmentIndex; // Índice do fragmento (para pacotes fragmentados)
    uint16_t fragmentCount; // Total de fragmentos
    uint32_t checksum;      // Checksum do pacote
    uint64_t timestamp;     // Timestamp do envio
};
#pragma pack(pop)

// Estrutura para pacote completo
struct Packet {
    PacketHeader header;
    std::vector<uint8_t> payload;
};

// Estrutura para estatísticas do protocolo
struct ProtocolStats {
    int64_t bytesSent;
    int64_t bytesReceived;
    int packetsSent;
    int packetsReceived;
    int packetsLost;
    int packetsResent;
    int fragmentsSent;
    int fragmentsReceived;
    int corruptedPackets;
    int outOfOrderPackets;
    int duplicatePackets;
    int reliableSuccesses;
    int reliableFailures;
    float rtt;
    float packetLoss;
    float jitter;
    int currentBytesPerSecond;
    int peakBytesPerSecond;
};

// Callback para pacote recebido
using PacketReceivedCallback = std::function<void(const Packet& packet)>;

// Callback para erro de protocolo
using ProtocolErrorCallback = std::function<void(int errorCode, const std::string& errorMessage)>;

// Callback para evento de estatísticas
using ProtocolStatsCallback = std::function<void(const ProtocolStats& stats)>;

/**
 * @class NetworkProtocol
 * @brief Implementação de protocolo de rede customizado
 * 
 * Protocolo de rede específico para WYDBR com foco em confiabilidade,
 * eficiência e segurança para jogos com muitos jogadores simultâneos.
 */
class NetworkProtocol {
public:
    NetworkProtocol();
    ~NetworkProtocol();
    
    /**
     * @brief Inicializa o protocolo
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const ProtocolConfig& config = ProtocolConfig());
    
    /**
     * @brief Desliga o protocolo e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Envia um pacote
     * @param type Tipo do pacote
     * @param payload Dados a enviar
     * @param flags Flags do pacote
     * @param channel Canal a usar
     * @return true se enviado com sucesso
     */
    bool sendPacket(PacketType type, 
                   const std::vector<uint8_t>& payload,
                   uint8_t flags = 0,
                   uint16_t channel = 0);
    
    /**
     * @brief Processa um pacote recebido
     * @param data Dados brutos recebidos
     * @return true se o pacote foi processado com sucesso
     */
    bool processRawPacket(const std::vector<uint8_t>& data);
    
    /**
     * @brief Envia dados de forma confiável
     * @param payload Dados a enviar
     * @param channel Canal a usar
     * @return true se o envio foi iniciado com sucesso
     */
    bool sendReliable(const std::vector<uint8_t>& payload, uint16_t channel = 0);
    
    /**
     * @brief Envia dados de forma ordenada
     * @param payload Dados a enviar
     * @param channel Canal a usar
     * @return true se o envio foi iniciado com sucesso
     */
    bool sendOrdered(const std::vector<uint8_t>& payload, uint16_t channel = 0);
    
    /**
     * @brief Envia dados não confiáveis
     * @param payload Dados a enviar
     * @param channel Canal a usar
     * @return true se enviado com sucesso
     */
    bool sendUnreliable(const std::vector<uint8_t>& payload, uint16_t channel = 0);
    
    /**
     * @brief Envia um ping para medir RTT
     * @return true se o ping foi enviado com sucesso
     */
    bool ping();
    
    /**
     * @brief Envia um heartbeat para manter a conexão ativa
     * @return true se o heartbeat foi enviado com sucesso
     */
    bool sendHeartbeat();
    
    /**
     * @brief Registra um callback para pacotes recebidos
     * @param callback Função a ser chamada
     * @param packetType Tipo específico de pacote ou -1 para todos
     * @return ID do callback (para remoção)
     */
    int registerPacketCallback(PacketReceivedCallback callback, 
                              PacketType packetType = static_cast<PacketType>(-1));
    
    /**
     * @brief Remove um callback de pacotes recebidos
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterPacketCallback(int callbackId);
    
    /**
     * @brief Registra um callback para erros de protocolo
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerErrorCallback(ProtocolErrorCallback callback);
    
    /**
     * @brief Remove um callback de erros de protocolo
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterErrorCallback(int callbackId);
    
    /**
     * @brief Registra um callback para estatísticas
     * @param callback Função a ser chamada
     * @param intervalMs Intervalo de atualização em milissegundos
     * @return ID do callback (para remoção)
     */
    int registerStatsCallback(ProtocolStatsCallback callback, int intervalMs = 1000);
    
    /**
     * @brief Remove um callback de estatísticas
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterStatsCallback(int callbackId);
    
    /**
     * @brief Processa lógica interna, como reenvio de pacotes
     * @param deltaTimeMs Tempo decorrido desde a última atualização
     */
    void update(float deltaTimeMs);
    
    /**
     * @brief Obtém estatísticas atuais do protocolo
     * @return Estatísticas
     */
    ProtocolStats getStatistics() const;
    
    /**
     * @brief Obtém o RTT atual
     * @return RTT em milissegundos
     */
    float getRTT() const;
    
    /**
     * @brief Define a chave de encriptação
     * @param key Chave
     */
    void setEncryptionKey(const std::string& key);
    
    /**
     * @brief Ativa ou desativa a encriptação
     * @param enable Se deve ativar
     */
    void enableEncryption(bool enable);
    
    /**
     * @brief Verifica se a encriptação está ativa
     * @return true se ativa
     */
    bool isEncryptionEnabled() const;
    
    /**
     * @brief Ativa ou desativa a compressão
     * @param enable Se deve ativar
     */
    void enableCompression(bool enable);
    
    /**
     * @brief Verifica se a compressão está ativa
     * @return true se ativa
     */
    bool isCompressionEnabled() const;
    
    /**
     * @brief Cria um canal
     * @param channelId ID do canal
     * @param reliable Se o canal é confiável por padrão
     * @param ordered Se o canal é ordenado por padrão
     * @return true se criado com sucesso
     */
    bool createChannel(uint16_t channelId, bool reliable = true, bool ordered = true);
    
    /**
     * @brief Fecha um canal
     * @param channelId ID do canal
     * @return true se fechado com sucesso
     */
    bool closeChannel(uint16_t channelId);

private:
    // Métodos internos
    bool serializePacket(const Packet& packet, std::vector<uint8_t>& outData);
    bool deserializePacket(const std::vector<uint8_t>& data, Packet& outPacket);
    
    uint32_t calculateChecksum(const std::vector<uint8_t>& data);
    bool verifyChecksum(const Packet& packet);
    
    bool sendFragmentedPacket(PacketType type, const std::vector<uint8_t>& payload, 
                             uint8_t flags, uint16_t channel);
    
    void processAcknowledgments(const PacketHeader& header);
    void updateRTTStatistics(const PacketHeader& header);
    
    void handleReliablePacket(const Packet& packet);
    void handleOrderedPacket(const Packet& packet);
    void handleFragmentedPacket(const Packet& packet);
    
    void resendReliablePackets();
    void cleanupOldPackets();
    
    std::vector<uint8_t> encryptPayload(const std::vector<uint8_t>& payload);
    std::vector<uint8_t> decryptPayload(const std::vector<uint8_t>& payload);
    
    std::vector<uint8_t> compressPayload(const std::vector<uint8_t>& payload);
    std::vector<uint8_t> decompressPayload(const std::vector<uint8_t>& payload);
    
    // Estado interno
    bool m_initialized;
    ProtocolConfig m_config;
    ProtocolStats m_stats;
    
    uint32_t m_nextSequenceNumber;
    uint32_t m_lastReceivedSequence;
    uint32_t m_ackBitfield;
    
    // Estrutura para rastreamento de pacotes confiáveis
    struct ReliablePacket {
        Packet packet;
        std::chrono::steady_clock::time_point sendTime;
        std::chrono::steady_clock::time_point nextResendTime;
        int sendCount;
        bool acked;
    };
    
    // Estrutura para rastreamento de fragmentos
    struct FragmentedPacket {
        std::vector<std::vector<uint8_t>> fragments;
        PacketType type;
        uint8_t flags;
        uint16_t channel;
        uint32_t sequenceNumber;
        std::chrono::steady_clock::time_point receiveStartTime;
        bool complete;
    };
    
    // Estrutura para rastreamento de pacotes ordenados
    struct OrderedChannel {
        uint32_t nextSequence;
        std::map<uint32_t, Packet> pendingPackets;
    };
    
    // Rastreadores de pacotes
    std::vector<ReliablePacket> m_reliablePackets;
    std::map<uint32_t, FragmentedPacket> m_fragmentedPackets;
    std::map<uint16_t, OrderedChannel> m_orderedChannels;
    
    // Rastreamento de RTT
    std::vector<float> m_rttSamples;
    float m_currentRTT;
    float m_smoothedRTT;
    float m_rttVariation;
    
    // Threads e timers
    std::chrono::steady_clock::time_point m_lastUpdateTime;
    std::chrono::steady_clock::time_point m_lastHeartbeatTime;
    
    // Callbacks
    struct CallbackInfo {
        PacketReceivedCallback callback;
        PacketType packetType;
    };
    
    struct StatsCallbackInfo {
        ProtocolStatsCallback callback;
        int intervalMs;
        std::chrono::steady_clock::time_point lastCallTime;
    };
    
    std::vector<std::pair<int, CallbackInfo>> m_packetCallbacks;
    std::vector<std::pair<int, ProtocolErrorCallback>> m_errorCallbacks;
    std::vector<std::pair<int, StatsCallbackInfo>> m_statsCallbacks;
    
    int m_nextCallbackId;
    
    // Thread safety
    mutable std::mutex m_statsMutex;
    mutable std::mutex m_packetsMutex;
    mutable std::mutex m_fragmentsMutex;
    mutable std::mutex m_callbackMutex;
    mutable std::mutex m_channelsMutex;
};

} // namespace network
} // namespace wydbr

#endif // WYDBR_NETWORK_PROTOCOL_H
