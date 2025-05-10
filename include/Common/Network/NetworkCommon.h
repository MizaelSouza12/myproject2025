
/**
 * @file NetworkCommon.h
 * @brief Definições comuns para rede utilizadas por todos os componentes
 */

#ifndef WYDBR_NETWORK_COMMON_H
#define WYDBR_NETWORK_COMMON_H

#include <cstdint>
#include <string>
#include <vector>
#include <array>

namespace WYDBR {
namespace Network {

// Tipos de pacotes
enum class PacketType : uint16_t {
    // Pacotes de sistema
    NONE = 0,
    PING = 1,
    PONG = 2,
    DISCONNECT = 3,
    
    // Pacotes de autenticação
    AUTH_REQUEST = 100,
    AUTH_RESPONSE = 101,
    
    // Pacotes de jogo
    GAME_STATE = 200,
    PLAYER_MOVE = 201,
    PLAYER_ACTION = 202,
    
    // Pacotes de VoIP
    VOIP_DATA = 300,
    VOIP_CHANNEL_JOIN = 301,
    VOIP_CHANNEL_LEAVE = 302,
    
    // Pacotes de compartilhamento de tela
    SCREEN_DATA = 400,
    SCREEN_START = 401,
    SCREEN_STOP = 402,
    SCREEN_VIEW = 403,
    
    // Pacotes administrativos
    ADMIN_COMMAND = 500,
    ADMIN_RESPONSE = 501
};

// Estrutura de cabeçalho de pacote
#pragma pack(push, 1)
struct PacketHeader {
    uint16_t size;        // Tamanho total do pacote
    uint16_t type;        // Tipo do pacote (PacketType)
    uint32_t sequence;    // Número de sequência
    uint32_t timestamp;   // Timestamp
    uint16_t flags;       // Flags adicionais
};
#pragma pack(pop)

// Classe base para pacotes
class Packet {
public:
    Packet(PacketType type);
    virtual ~Packet() = default;
    
    // Serialização
    virtual std::vector<uint8_t> Serialize() const = 0;
    virtual bool Deserialize(const std::vector<uint8_t>& data) = 0;
    
    // Getters
    PacketType GetType() const;
    uint32_t GetSequence() const;
    uint32_t GetTimestamp() const;
    
    // Setters
    void SetSequence(uint32_t sequence);
    
protected:
    PacketHeader m_header;
};

// Estrutura de informação de cliente
struct ClientInfo {
    uint32_t id;
    std::string address;
    uint16_t port;
    std::string username;
    bool authenticated;
    int64_t lastActivity;
    
    // Métodos utilitários
    bool IsConnected() const;
    int64_t GetIdleTimeMs() const;
};

} // namespace Network
} // namespace WYDBR

#endif // WYDBR_NETWORK_COMMON_H
