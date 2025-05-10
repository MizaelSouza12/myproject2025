/**
 * @file UDPProtocolHandler.h
 * @brief Manipulador de protocolo UDP com controle de sequência
 */

#ifndef WYDBR_UDP_PROTOCOL_HANDLER_H
#define WYDBR_UDP_PROTOCOL_HANDLER_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <chrono>
#include <queue>
#include <deque>

namespace WYDBR {
namespace Network {

/**
 * Opções do protocolo UDP
 */
struct UDPProtocolOptions {
    uint32_t maxSequenceHistory;       // Máximo de sequências a manter no histórico
    uint32_t packetTimeoutMs;          // Timeout para pacotes em ms
    uint32_t maxRetransmissions;       // Máximo de retransmissões
    uint32_t maxOutOfOrderPackets;     // Máximo de pacotes fora de ordem a armazenar
    uint32_t initialRTTMs;             // RTT inicial em ms
    bool enableReliability;            // Habilitar confiabilidade
    bool enableFragmentation;          // Habilitar fragmentação
    bool enableOrdering;               // Habilitar ordenação
    bool enableAcknowledgment;         // Habilitar confirmação
    bool enableFlowControl;            // Habilitar controle de fluxo
    uint32_t fragmentSize;             // Tamanho máximo de fragmento
    uint32_t maxFragmentsPerPacket;    // Máximo de fragmentos por pacote
};

/**
 * Estado de um pacote
 */
enum class PacketState {
    PENDING,        // Pendente de envio
    SENT,           // Enviado, aguardando confirmação
    ACKNOWLEDGED,   // Confirmado
    LOST,           // Perdido
    RESENT          // Reenviado
};

/**
 * Opções de entrega de pacote
 */
enum class DeliveryOption {
    UNRELIABLE,               // Entrega não confiável
    RELIABLE,                 // Entrega confiável
    RELIABLE_ORDERED,         // Entrega confiável e ordenada
    RELIABLE_SEQUENCED,       // Entrega confiável e sequenciada (descarta antigos)
    UNRELIABLE_SEQUENCED      // Entrega não confiável e sequenciada
};

/**
 * Estrutura de pacote UDP
 */
struct UDPPacket {
    uint16_t protocolId;           // ID do protocolo
    uint32_t sequence;             // Número de sequência
    uint16_t ack;                  // Número de confirmação
    uint32_t ackBitfield;          // Bitfield de confirmação
    uint8_t flags;                 // Flags do pacote
    uint16_t fragmentId;           // ID do fragmento
    uint16_t fragmentCount;        // Contagem de fragmentos
    uint16_t fragmentIndex;        // Índice do fragmento
    uint16_t dataSize;             // Tamanho dos dados
    std::vector<uint8_t> data;     // Dados do pacote
    
    // Metadados (não transmitidos)
    uint64_t timestamp;            // Timestamp de envio
    uint32_t retransmissions;      // Contagem de retransmissões
    PacketState state;             // Estado do pacote
    DeliveryOption deliveryOption; // Opção de entrega
};

/**
 * Flags de pacote
 */
enum class PacketFlag : uint8_t {
    NONE         = 0x00,
    ACK          = 0x01,
    RELIABLE     = 0x02,
    ORDERED      = 0x04,
    SEQUENCED    = 0x08,
    FRAGMENT     = 0x10,
    LAST_FRAGMENT = 0x20,
    CONTROL      = 0x40,
    ENCRYPTED    = 0x80
};

// Operador bitwise OR para PacketFlag
inline PacketFlag operator|(PacketFlag a, PacketFlag b) {
    return static_cast<PacketFlag>(
        static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
    );
}

// Operador bitwise AND para PacketFlag
inline bool operator&(PacketFlag a, PacketFlag b) {
    return (static_cast<uint8_t>(a) & static_cast<uint8_t>(b)) != 0;
}

/**
 * Callback para processamento de pacotes recebidos
 */
using PacketReceivedCallback = std::function<void(const UDPPacket& packet)>;

/**
 * Callback para envio de pacotes
 */
using PacketSendCallback = std::function<bool(const void* data, size_t size)>;

/**
 * Callback para estado de pacote
 */
using PacketStateCallback = std::function<void(uint32_t sequence, PacketState state)>;

/**
 * Manipulador de protocolo UDP
 * 
 * Esta implementação corrige os problemas do protocolo UDP original,
 * adicionando controle de sequência e mecanismos de confiabilidade.
 */
class UDPProtocolHandler {
public:
    /**
     * Construtor
     * @param protocolId ID único do protocolo
     * @param options Opções do protocolo
     */
    UDPProtocolHandler(uint16_t protocolId, const UDPProtocolOptions& options);
    
    /**
     * Destrutor
     */
    ~UDPProtocolHandler();
    
    /**
     * Inicializa o manipulador
     * @param sendCallback Callback para envio de pacotes
     * @param receivedCallback Callback para pacotes recebidos
     * @param stateCallback Callback para estados de pacote (opcional)
     * @return true se inicializado com sucesso
     */
    bool Initialize(PacketSendCallback sendCallback, 
                   PacketReceivedCallback receivedCallback,
                   PacketStateCallback stateCallback = nullptr);
    
    /**
     * Envia dados pela rede
     * @param data Dados a enviar
     * @param size Tamanho dos dados
     * @param deliveryOption Opção de entrega
     * @return Número de sequência ou 0 se falhar
     */
    uint32_t Send(const void* data, size_t size, DeliveryOption deliveryOption = DeliveryOption::RELIABLE);
    
    /**
     * Processa dados recebidos
     * @param data Dados recebidos
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool ProcessReceived(const void* data, size_t size);
    
    /**
     * Atualiza o manipulador
     * @param deltaTimeMs Tempo decorrido em ms
     */
    void Update(uint32_t deltaTimeMs);
    
    /**
     * Obtém estatísticas de rede
     * @return String formatada com estatísticas
     */
    std::string GetStatistics() const;
    
    /**
     * Limpa o histórico de pacotes
     */
    void ClearHistory();
    
    /**
     * Obtém o RTT (Round Trip Time) atual
     * @return RTT em ms
     */
    uint32_t GetRTT() const;
    
    /**
     * Obtém a taxa de perda de pacotes
     * @return Taxa de perda (0.0-1.0)
     */
    float GetPacketLossRate() const;
    
    /**
     * Define a capacidade do canal (para controle de fluxo)
     * @param bytesPerSecond Bytes por segundo
     */
    void SetChannelCapacity(uint32_t bytesPerSecond);
    
private:
    // ID do protocolo
    uint16_t m_protocolId;
    
    // Opções do protocolo
    UDPProtocolOptions m_options;
    
    // Callbacks
    PacketSendCallback m_sendCallback;
    PacketReceivedCallback m_receivedCallback;
    PacketStateCallback m_stateCallback;
    
    // Sequências
    uint32_t m_localSequence;     // Próxima sequência local
    uint32_t m_remoteSequence;    // Última sequência remota recebida
    
    // Histórico de pacotes enviados
    struct SentPacketInfo {
        UDPPacket packet;         // Pacote enviado
        uint64_t sentTime;        // Tempo de envio
        uint64_t lastResendTime;  // Tempo da última retransmissão
    };
    std::unordered_map<uint32_t, SentPacketInfo> m_sentPackets;
    
    // Histórico de pacotes recebidos para detecção de duplicatas
    std::unordered_map<uint32_t, uint64_t> m_receivedPackets;
    
    // Cache de pacotes ordenados fora de ordem
    std::map<uint32_t, UDPPacket> m_outOfOrderPackets;
    
    // Fragmentos em progresso
    struct FragmentAssembly {
        std::vector<UDPPacket> fragments;       // Fragmentos recebidos
        uint16_t fragmentCount;                // Total de fragmentos esperados
        uint64_t firstFragmentTime;            // Tempo do primeiro fragmento
        bool complete;                         // Conjunto completo
    };
    std::unordered_map<uint16_t, FragmentAssembly> m_fragmentAssembly;
    
    // Coisas relacionadas a RTT e controle de fluxo
    uint32_t m_rtt;                          // RTT atual em ms
    uint32_t m_rttVariance;                  // Variância do RTT
    uint32_t m_bytesPerSecond;               // Taxa atual
    uint32_t m_congestionWindow;             // Janela de congestionamento
    uint64_t m_lastUpdateTime;               // Último tempo de atualização
    
    // Estatísticas
    uint32_t m_totalSent;                    // Total enviado
    uint32_t m_totalReceived;                // Total recebido
    uint32_t m_totalLost;                    // Total perdido
    uint32_t m_totalAcknowledged;            // Total confirmado
    uint32_t m_totalResent;                  // Total reenviado
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos privados
    UDPPacket CreatePacket(const void* data, size_t size, DeliveryOption option);
    std::vector<UDPPacket> FragmentPacket(const UDPPacket& packet);
    bool ReassembleFragments(const UDPPacket& fragment);
    void SendAck(uint32_t sequence);
    void ProcessAck(const UDPPacket& packet);
    void UpdateRTT(uint32_t sequence, uint64_t sentTime);
    void UpdateFlowControl(uint32_t bytesAcked, uint32_t lostPackets);
    bool IsSequenceNewer(uint32_t s1, uint32_t s2);
    uint32_t GenerateAckBitfield();
    void NotifyPacketState(uint32_t sequence, PacketState state);
    void DeliverOutOfOrderPackets();
    std::vector<uint8_t> SerializePacket(const UDPPacket& packet);
    bool DeserializePacket(const void* data, size_t size, UDPPacket& outPacket);
};

} // namespace Network
} // namespace WYDBR

#endif // WYDBR_UDP_PROTOCOL_HANDLER_H
