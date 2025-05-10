/**
 * @file NetworkProtocol.h
 * @brief Definições unificadas de protocolo de rede
 */

#ifndef WYDBR_NETWORK_PROTOCOL_H
#define WYDBR_NETWORK_PROTOCOL_H

#include <cstdint>
#include <string>
#include <array>
#include <vector>

namespace WYDBR {
namespace Network {

// Definição de versão do protocolo
constexpr uint16_t PROTOCOL_VERSION = 0x0100; // Versão 1.0

// Tamanho máximo de pacote
constexpr size_t MAX_PACKET_SIZE = 8192;

// Flags de protocolo
enum class ProtocolFlags : uint16_t {
    None        = 0x0000,
    Compressed  = 0x0001,
    Encrypted   = 0x0002,
    Fragmented  = 0x0004,
    HasChecksum = 0x0008,
    Priority    = 0x0010,
    Reliable    = 0x0020,
    NeedsAck    = 0x0040,
    IsAck       = 0x0080,
    LastFragment= 0x0100
};

// Operador para combinar flags
inline ProtocolFlags operator|(ProtocolFlags a, ProtocolFlags b) {
    return static_cast<ProtocolFlags>(
        static_cast<uint16_t>(a) | static_cast<uint16_t>(b)
    );
}

// Operador para verificar flag
inline bool operator&(ProtocolFlags a, ProtocolFlags b) {
    return (static_cast<uint16_t>(a) & static_cast<uint16_t>(b)) != 0;
}

/**
 * Tipos de pacotes - UNIFICADOS para servidor e cliente
 * Prefixados com 0x00 para evitar conflitos com versão herdada
 */
enum class PacketType : uint16_t {
    // Pacotes de sistema - 0x00xx
    Ping                    = 0x0001,
    Pong                    = 0x0002,
    Handshake               = 0x0003,
    Disconnect              = 0x0004,
    Acknowledge             = 0x0005,
    Fragment                = 0x0006,
    
    // Pacotes de autenticação - 0x01xx
    LoginRequest            = 0x0101,
    LoginResponse           = 0x0102,
    LogoutRequest           = 0x0103,
    LogoutResponse          = 0x0104,
    TokenRefresh            = 0x0105,
    TokenResponse           = 0x0106,
    
    // Pacotes de movimento - 0x02xx
    Movement                = 0x0201,
    MovementAck             = 0x0202,
    Teleport                = 0x0203,
    TeleportAck             = 0x0204,
    PathRequest             = 0x0205,
    PathResponse            = 0x0206,
    
    // Pacotes de combate - 0x03xx
    Attack                  = 0x0301,
    AttackResult            = 0x0302,
    SkillUse                = 0x0303,
    SkillResult             = 0x0304,
    DamageReceived          = 0x0305,
    HealReceived            = 0x0306,
    StatusEffect            = 0x0307,
    
    // Pacotes de item/inventário - 0x04xx
    ItemMove                = 0x0401,
    ItemUse                 = 0x0402,
    ItemDrop                = 0x0403,
    ItemPickup              = 0x0404,
    InventoryUpdate         = 0x0405,
    ItemEquip               = 0x0406,
    ItemUnequip             = 0x0407,
    
    // Pacotes de chat - 0x05xx
    ChatMessage             = 0x0501,
    ChatWhisper             = 0x0502,
    ChatParty               = 0x0503,
    ChatGuild               = 0x0504,
    ChatSystem              = 0x0505,
    
    // Pacotes de grupo/guild - 0x06xx
    PartyInvite             = 0x0601,
    PartyJoin               = 0x0602,
    PartyLeave              = 0x0603,
    PartyUpdate             = 0x0604,
    GuildInvite             = 0x0605,
    GuildJoin               = 0x0606,
    GuildLeave              = 0x0607,
    GuildUpdate             = 0x0608,
    
    // Pacotes de comércio - 0x07xx
    TradeRequest            = 0x0701,
    TradeAccept             = 0x0702,
    TradeDecline            = 0x0703,
    TradeAddItem            = 0x0704,
    TradeRemoveItem         = 0x0705,
    TradeSetMoney           = 0x0706,
    TradeConfirm            = 0x0707,
    TradeCancel             = 0x0708,
    TradeComplete           = 0x0709,
    
    // Pacotes de VoIP - 0x08xx
    VoiceData               = 0x0801,
    VoiceJoinChannel        = 0x0802,
    VoiceLeaveChannel       = 0x0803,
    VoiceChannelUpdate      = 0x0804,
    VoiceSetMute            = 0x0805,
    
    // Pacotes de compartilhamento de tela - 0x09xx
    ScreenData              = 0x0901,
    ScreenStartShare        = 0x0902,
    ScreenStopShare         = 0x0903,
    ScreenJoinView          = 0x0904,
    ScreenLeaveView         = 0x0905,
    ScreenControl           = 0x0906,
    
    // Pacotes administrativos - 0x0Axx
    AdminCommand            = 0x0A01,
    AdminResponse           = 0x0A02,
    SystemStatus            = 0x0A03,
    SystemControl           = 0x0A04,
    
    // Pacotes de leilão - 0x0Bxx
    AuctionList             = 0x0B01,
    AuctionBid              = 0x0B02,
    AuctionCreate           = 0x0B03,
    AuctionCancel           = 0x0B04,
    AuctionComplete         = 0x0B05,
    
    // Pacotes de evento - 0x0Cxx
    EventStart              = 0x0C01,
    EventEnd                = 0x0C02,
    EventUpdate             = 0x0C03,
    EventJoin               = 0x0C04,
    EventLeave              = 0x0C05,
    
    // Pacotes de missão - 0x0Dxx
    QuestAccept             = 0x0D01,
    QuestComplete           = 0x0D02,
    QuestUpdate             = 0x0D03,
    QuestAbandon            = 0x0D04,
    QuestList               = 0x0D05,
    
    // Máximo definido para validação
    MaxPacketType           = 0x0FFF
};

/**
 * Prioridade de pacote
 */
enum class PacketPriority : uint8_t {
    Low = 0,         // Baixa prioridade
    Normal = 1,      // Prioridade normal
    High = 2,        // Alta prioridade
    Critical = 3     // Prioridade crítica
};

/**
 * Cabeçalho de pacote unificado
 * Tamanho total: 16 bytes
 */
#pragma pack(push, 1)
struct PacketHeader {
    uint16_t signature;      // Assinatura (sempre 'WY')
    uint16_t version;        // Versão do protocolo
    uint16_t type;           // Tipo de pacote (PacketType)
    uint16_t flags;          // Flags (ProtocolFlags)
    uint16_t length;         // Comprimento total do pacote incluindo cabeçalho
    uint16_t sequence;       // Número de sequência
    uint32_t timestamp;      // Timestamp em ms
    uint16_t checksum;       // Checksum do pacote
};
#pragma pack(pop)

/**
 * Interface para estruturas de pacotes
 */
class IPacket {
public:
    virtual ~IPacket() = default;
    
    /**
     * Serializa o pacote
     * @param buffer Buffer para serialização
     * @param bufferSize Tamanho do buffer
     * @return Número de bytes escritos ou -1 se falhar
     */
    virtual int Serialize(uint8_t* buffer, size_t bufferSize) const = 0;
    
    /**
     * Deserializa o pacote
     * @param buffer Buffer com dados
     * @param bufferSize Tamanho do buffer
     * @return Número de bytes lidos ou -1 se falhar
     */
    virtual int Deserialize(const uint8_t* buffer, size_t bufferSize) = 0;
    
    /**
     * Obtém o tipo de pacote
     * @return Tipo do pacote
     */
    virtual PacketType GetType() const = 0;
    
    /**
     * Obtém o tamanho serializado
     * @return Tamanho do pacote serializado
     */
    virtual size_t GetSerializedSize() const = 0;
    
    /**
     * Obtém a prioridade do pacote
     * @return Prioridade
     */
    virtual PacketPriority GetPriority() const = 0;
    
    /**
     * Verifica se o pacote requer confirmação
     * @return true se requer confirmação
     */
    virtual bool RequiresAck() const = 0;
};

/**
 * Classe base para pacotes
 */
template<PacketType PT, typename PayloadType>
class Packet : public IPacket {
public:
    Packet() : 
        m_header{},
        m_payload{} {
        
        // Inicializar cabeçalho
        m_header.signature = ('W' << 8) | 'Y';
        m_header.version = PROTOCOL_VERSION;
        m_header.type = static_cast<uint16_t>(PT);
        m_header.flags = static_cast<uint16_t>(ProtocolFlags::None);
        m_header.length = sizeof(PacketHeader) + sizeof(PayloadType);
        m_header.sequence = 0; // Será definido pelo sistema
        m_header.timestamp = 0; // Será definido pelo sistema
        m_header.checksum = 0; // Será calculado na serialização
    }
    
    PacketType GetType() const override {
        return PT;
    }
    
    size_t GetSerializedSize() const override {
        return sizeof(PacketHeader) + sizeof(PayloadType);
    }
    
    PacketPriority GetPriority() const override {
        return PacketPriority::Normal;
    }
    
    bool RequiresAck() const override {
        return false;
    }
    
    int Serialize(uint8_t* buffer, size_t bufferSize) const override {
        if (bufferSize < GetSerializedSize()) {
            return -1;
        }
        
        // Copiar cabeçalho
        PacketHeader header = m_header;
        
        // Calcular checksum
        header.checksum = CalculateChecksum(&header, sizeof(header) - sizeof(uint16_t), 
                                        &m_payload, sizeof(PayloadType));
        
        // Copiar dados
        memcpy(buffer, &header, sizeof(header));
        memcpy(buffer + sizeof(header), &m_payload, sizeof(PayloadType));
        
        return static_cast<int>(GetSerializedSize());
    }
    
    int Deserialize(const uint8_t* buffer, size_t bufferSize) override {
        if (bufferSize < sizeof(PacketHeader)) {
            return -1;
        }
        
        // Copiar cabeçalho
        memcpy(&m_header, buffer, sizeof(PacketHeader));
        
        // Verificar assinatura
        if (m_header.signature != (('W' << 8) | 'Y')) {
            return -1;
        }
        
        // Verificar versão
        if (m_header.version != PROTOCOL_VERSION) {
            return -1;
        }
        
        // Verificar tipo
        if (m_header.type != static_cast<uint16_t>(PT)) {
            return -1;
        }
        
        // Verificar tamanho
        if (m_header.length != sizeof(PacketHeader) + sizeof(PayloadType)) {
            return -1;
        }
        
        if (bufferSize < m_header.length) {
            return -1;
        }
        
        // Verificar checksum
        uint16_t originalChecksum = m_header.checksum;
        m_header.checksum = 0;
        
        uint16_t calculatedChecksum = CalculateChecksum(&m_header, sizeof(m_header),
                                                    buffer + sizeof(PacketHeader),
                                                    sizeof(PayloadType));
        
        if (calculatedChecksum != originalChecksum) {
            return -1;
        }
        
        // Restaurar checksum
        m_header.checksum = originalChecksum;
        
        // Copiar payload
        memcpy(&m_payload, buffer + sizeof(PacketHeader), sizeof(PayloadType));
        
        return static_cast<int>(m_header.length);
    }
    
    // Getters/setters para os campos
    void SetSequence(uint16_t sequence) { m_header.sequence = sequence; }
    uint16_t GetSequence() const { return m_header.sequence; }
    
    void SetTimestamp(uint32_t timestamp) { m_header.timestamp = timestamp; }
    uint32_t GetTimestamp() const { return m_header.timestamp; }
    
    void AddFlag(ProtocolFlags flag) { 
        m_header.flags |= static_cast<uint16_t>(flag); 
    }
    
    bool HasFlag(ProtocolFlags flag) const { 
        return (m_header.flags & static_cast<uint16_t>(flag)) != 0; 
    }
    
    PayloadType& GetPayload() { return m_payload; }
    const PayloadType& GetPayload() const { return m_payload; }
    
private:
    PacketHeader m_header;
    PayloadType m_payload;
    
    // Calcula checksum para o pacote
    static uint16_t CalculateChecksum(const void* header, size_t headerSize,
                                  const void* payload, size_t payloadSize) {
        // Implementação de CRC-16
        const uint8_t* data1 = static_cast<const uint8_t*>(header);
        const uint8_t* data2 = static_cast<const uint8_t*>(payload);
        
        uint16_t crc = 0xFFFF;
        
        // Process header
        for (size_t i = 0; i < headerSize; i++) {
            crc ^= data1[i];
            for (uint8_t j = 0; j < 8; j++) {
                if (crc & 0x0001) {
                    crc = (crc >> 1) ^ 0xA001;
                } else {
                    crc = crc >> 1;
                }
            }
        }
        
        // Process payload
        for (size_t i = 0; i < payloadSize; i++) {
            crc ^= data2[i];
            for (uint8_t j = 0; j < 8; j++) {
                if (crc & 0x0001) {
                    crc = (crc >> 1) ^ 0xA001;
                } else {
                    crc = crc >> 1;
                }
            }
        }
        
        return crc;
    }
};

} // namespace Network
} // namespace WYDBR

#endif // WYDBR_NETWORK_PROTOCOL_H
