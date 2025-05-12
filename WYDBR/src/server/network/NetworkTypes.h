/**
 * @file NetworkTypes.h
 * @brief Tipos e estruturas para comunicação em rede
 * @author WYDBR Team
 */

#ifndef WYDBR_NETWORKTYPES_H
#define WYDBR_NETWORKTYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <memory>

namespace wydbr {
namespace server {
namespace network {

/**
 * @brief Tipo de socket
 */
enum class SocketType {
    TCP,     ///< Socket TCP
    UDP,     ///< Socket UDP
    WEB      ///< WebSocket
};

/**
 * @brief Estado de conexão
 */
enum class ConnectionState {
    DISCONNECTED,     ///< Desconectado
    CONNECTING,       ///< Em processo de conexão
    CONNECTED,        ///< Conectado
    AUTHENTICATING,   ///< Em processo de autenticação
    AUTHENTICATED,    ///< Autenticado
    HANDSHAKING,      ///< Em processo de handshake
    READY,            ///< Pronto para uso
    CLOSING           ///< Em processo de fechamento
};

/**
 * @brief Tipo de pacote
 */
enum class PacketType : uint8_t {
    SYSTEM = 0x00,     ///< Pacotes de sistema
    HANDSHAKE = 0x01,  ///< Handshake
    PING = 0x02,       ///< Ping/Pong
    AUTH = 0x10,       ///< Autenticação
    DATA = 0x20,       ///< Dados
    ACK = 0x30,        ///< Confirmação
    ERROR = 0xF0,      ///< Erro
    CUSTOM = 0xFF      ///< Pacote customizado
};

/**
 * @brief Códigos de erro de rede
 */
enum class NetworkError : uint16_t {
    NONE = 0,                   ///< Sem erro
    CONNECTION_FAILED = 1,      ///< Falha de conexão
    CONNECTION_TIMEOUT = 2,     ///< Timeout de conexão
    CONNECTION_REFUSED = 3,     ///< Conexão recusada
    CONNECTION_CLOSED = 4,      ///< Conexão fechada
    INVALID_PACKET = 10,        ///< Pacote inválido
    PACKET_TOO_LARGE = 11,      ///< Pacote muito grande
    INVALID_CHECKSUM = 12,      ///< Checksum inválido
    PROTOCOL_ERROR = 20,        ///< Erro de protocolo
    AUTH_FAILED = 30,           ///< Falha de autenticação
    AUTH_EXPIRED = 31,          ///< Autenticação expirada
    BANNED = 32,                ///< Endereço banido
    MAX_CONNECTIONS = 40,       ///< Máximo de conexões atingido
    INTERNAL_ERROR = 50,        ///< Erro interno
    INVALID_STATE = 51,         ///< Estado inválido
    SERVER_ERROR = 100,         ///< Erro do servidor
    CUSTOM_ERROR = 1000         ///< Erro personalizado
};

/**
 * @brief Configurações de conexão
 */
struct ConnectionConfig {
    SocketType socketType = SocketType::TCP;                ///< Tipo do socket
    uint32_t connectTimeoutMs = 5000;                       ///< Timeout de conexão em ms
    uint32_t readTimeoutMs = 30000;                         ///< Timeout de leitura em ms
    uint32_t writeTimeoutMs = 5000;                         ///< Timeout de escrita em ms
    uint32_t keepAliveIntervalMs = 30000;                   ///< Intervalo de keep-alive em ms
    uint32_t reconnectIntervalMs = 5000;                    ///< Intervalo de reconexão em ms
    uint32_t maxReconnectAttempts = 10;                     ///< Máximo de tentativas de reconexão
    bool autoReconnect = true;                              ///< Reconectar automaticamente
    bool noDelay = true;                                    ///< Desabilitar Nagle (TCP_NODELAY)
    int sendBufferSize = 65536;                             ///< Tamanho do buffer de envio
    int recvBufferSize = 65536;                             ///< Tamanho do buffer de recebimento
    bool reuseAddress = true;                               ///< Permitir reutilização de endereço
    bool enableEncryption = false;                          ///< Habilitar criptografia
    std::string encryptionKey = "";                         ///< Chave de criptografia
    std::vector<std::string> trustedCerts;                  ///< Certificados confiáveis
};

/**
 * @brief Cabeçalho de pacote
 */
#pragma pack(push, 1)
struct PacketHeader {
    uint16_t signature;      ///< Assinatura do pacote (0x4242)
    uint16_t size;           ///< Tamanho total do pacote (incluindo cabeçalho)
    uint8_t version;         ///< Versão do protocolo
    uint8_t type;            ///< Tipo do pacote
    uint16_t sequence;       ///< Número de sequência
    uint16_t flags;          ///< Flags do pacote
    uint16_t checksum;       ///< Checksum do pacote
};
#pragma pack(pop)

/**
 * @brief Flags de pacote
 */
enum class PacketFlag : uint16_t {
    NONE = 0x0000,            ///< Sem flags
    COMPRESSED = 0x0001,      ///< Pacote comprimido
    ENCRYPTED = 0x0002,       ///< Pacote criptografado
    FRAGMENTED = 0x0004,      ///< Pacote fragmentado
    LAST_FRAGMENT = 0x0008,   ///< Último fragmento
    REQUIRES_ACK = 0x0010,    ///< Requer confirmação
    PRIORITY = 0x0020,        ///< Prioridade alta
    SYSTEM = 0x0040,          ///< Pacote de sistema
    BROADCAST = 0x0080,       ///< Broadcast
    RESENT = 0x0100           ///< Pacote reenviado
};

/**
 * @brief Representação completa de um pacote de rede
 */
class Packet {
public:
    /**
     * @brief Construtor padrão
     */
    Packet() 
        : m_header({0x4242, sizeof(PacketHeader), 1, 0, 0, 0, 0}),
          m_payload() {}

    /**
     * @brief Construtor com tipo de pacote
     * @param type Tipo do pacote
     */
    Packet(PacketType type)
        : m_header({0x4242, sizeof(PacketHeader), 1, static_cast<uint8_t>(type), 0, 0, 0}),
          m_payload() {}

    /**
     * @brief Construtor com tipo e payload
     * @param type Tipo do pacote
     * @param payload Dados do pacote
     */
    Packet(PacketType type, const std::vector<uint8_t>& payload)
        : m_header({0x4242, static_cast<uint16_t>(sizeof(PacketHeader) + payload.size()), 
                   1, static_cast<uint8_t>(type), 0, 0, 0}),
          m_payload(payload) {
        updateChecksum();
    }

    /**
     * @brief Define o tipo do pacote
     * @param type Tipo do pacote
     */
    void setType(PacketType type) {
        m_header.type = static_cast<uint8_t>(type);
    }

    /**
     * @brief Obtém o tipo do pacote
     * @return Tipo do pacote
     */
    PacketType getType() const {
        return static_cast<PacketType>(m_header.type);
    }

    /**
     * @brief Define o número de sequência
     * @param sequence Número de sequência
     */
    void setSequence(uint16_t sequence) {
        m_header.sequence = sequence;
    }

    /**
     * @brief Obtém o número de sequência
     * @return Número de sequência
     */
    uint16_t getSequence() const {
        return m_header.sequence;
    }

    /**
     * @brief Define as flags do pacote
     * @param flags Flags do pacote
     */
    void setFlags(uint16_t flags) {
        m_header.flags = flags;
    }

    /**
     * @brief Adiciona uma flag ao pacote
     * @param flag Flag a ser adicionada
     */
    void addFlag(PacketFlag flag) {
        m_header.flags |= static_cast<uint16_t>(flag);
    }

    /**
     * @brief Remove uma flag do pacote
     * @param flag Flag a ser removida
     */
    void removeFlag(PacketFlag flag) {
        m_header.flags &= ~static_cast<uint16_t>(flag);
    }

    /**
     * @brief Verifica se uma flag está definida
     * @param flag Flag a ser verificada
     * @return true se a flag estiver definida, false caso contrário
     */
    bool hasFlag(PacketFlag flag) const {
        return (m_header.flags & static_cast<uint16_t>(flag)) != 0;
    }

    /**
     * @brief Define o payload do pacote
     * @param payload Payload do pacote
     */
    void setPayload(const std::vector<uint8_t>& payload) {
        m_payload = payload;
        m_header.size = static_cast<uint16_t>(sizeof(PacketHeader) + m_payload.size());
        updateChecksum();
    }

    /**
     * @brief Adiciona dados ao payload
     * @param data Dados a serem adicionados
     * @param size Tamanho dos dados
     */
    void addPayload(const void* data, size_t size) {
        const uint8_t* byteData = static_cast<const uint8_t*>(data);
        m_payload.insert(m_payload.end(), byteData, byteData + size);
        m_header.size = static_cast<uint16_t>(sizeof(PacketHeader) + m_payload.size());
        updateChecksum();
    }

    /**
     * @brief Obtém o payload do pacote
     * @return Payload do pacote
     */
    const std::vector<uint8_t>& getPayload() const {
        return m_payload;
    }

    /**
     * @brief Serializa o pacote para envio
     * @return Dados serializados (cabeçalho + payload)
     */
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> result;
        result.reserve(m_header.size);
        
        // Adiciona o cabeçalho
        const uint8_t* headerBytes = reinterpret_cast<const uint8_t*>(&m_header);
        result.insert(result.end(), headerBytes, headerBytes + sizeof(PacketHeader));
        
        // Adiciona o payload
        result.insert(result.end(), m_payload.begin(), m_payload.end());
        
        return result;
    }

    /**
     * @brief Deserializa dados para um pacote
     * @param data Dados a serem deserializados
     * @param size Tamanho dos dados
     * @return true se a deserialização foi bem-sucedida, false caso contrário
     */
    bool deserialize(const void* data, size_t size) {
        if (size < sizeof(PacketHeader)) {
            return false;
        }
        
        // Copia o cabeçalho
        const uint8_t* byteData = static_cast<const uint8_t*>(data);
        std::memcpy(&m_header, byteData, sizeof(PacketHeader));
        
        // Verifica o tamanho do pacote
        if (m_header.size > size || m_header.size < sizeof(PacketHeader)) {
            return false;
        }
        
        // Copia o payload
        size_t payloadSize = m_header.size - sizeof(PacketHeader);
        m_payload.resize(payloadSize);
        if (payloadSize > 0) {
            std::memcpy(m_payload.data(), byteData + sizeof(PacketHeader), payloadSize);
        }
        
        // Verifica o checksum
        uint16_t originalChecksum = m_header.checksum;
        m_header.checksum = 0;
        uint16_t calculatedChecksum = calculateChecksum();
        m_header.checksum = originalChecksum;
        
        return calculatedChecksum == originalChecksum;
    }

    /**
     * @brief Atualiza o checksum do pacote
     */
    void updateChecksum() {
        m_header.checksum = 0;
        m_header.checksum = calculateChecksum();
    }

    /**
     * @brief Obtém o tamanho do pacote
     * @return Tamanho do pacote
     */
    uint16_t getSize() const {
        return m_header.size;
    }

    /**
     * @brief Adiciona um valor inteiro ao payload
     * @tparam T Tipo do valor
     * @param value Valor a ser adicionado
     */
    template<typename T>
    void addValue(T value) {
        const uint8_t* valuePtr = reinterpret_cast<const uint8_t*>(&value);
        m_payload.insert(m_payload.end(), valuePtr, valuePtr + sizeof(T));
        m_header.size = static_cast<uint16_t>(sizeof(PacketHeader) + m_payload.size());
        updateChecksum();
    }

    /**
     * @brief Adiciona uma string ao payload
     * @param str String a ser adicionada
     */
    void addString(const std::string& str) {
        // Adiciona o tamanho da string
        uint16_t length = static_cast<uint16_t>(str.length());
        addValue(length);
        
        // Adiciona os caracteres
        if (length > 0) {
            m_payload.insert(m_payload.end(), str.begin(), str.end());
            m_header.size = static_cast<uint16_t>(sizeof(PacketHeader) + m_payload.size());
            updateChecksum();
        }
    }

    /**
     * @brief Obtém um valor do payload
     * @tparam T Tipo do valor
     * @param offset Deslocamento no payload
     * @return Valor lido
     */
    template<typename T>
    T getValue(size_t offset) const {
        if (offset + sizeof(T) > m_payload.size()) {
            // Erro: tentativa de leitura além do payload
            return T();
        }
        
        T value;
        std::memcpy(&value, m_payload.data() + offset, sizeof(T));
        return value;
    }

    /**
     * @brief Obtém uma string do payload
     * @param offset Deslocamento no payload
     * @return String lida
     */
    std::string getString(size_t offset) const {
        if (offset + sizeof(uint16_t) > m_payload.size()) {
            // Erro: não é possível ler o tamanho da string
            return "";
        }
        
        // Lê o tamanho da string
        uint16_t length = getValue<uint16_t>(offset);
        offset += sizeof(uint16_t);
        
        if (offset + length > m_payload.size()) {
            // Erro: string além do payload
            return "";
        }
        
        // Lê os caracteres
        return std::string(reinterpret_cast<const char*>(m_payload.data() + offset), length);
    }

private:
    /**
     * @brief Calcula o checksum do pacote
     * @return Checksum calculado
     */
    uint16_t calculateChecksum() const {
        uint32_t sum = 0;
        
        // Soma os bytes do cabeçalho
        const uint16_t* headerWords = reinterpret_cast<const uint16_t*>(&m_header);
        for (size_t i = 0; i < sizeof(PacketHeader) / sizeof(uint16_t); ++i) {
            sum += headerWords[i];
        }
        
        // Soma os bytes do payload
        for (size_t i = 0; i < m_payload.size(); i += 2) {
            uint16_t word;
            if (i + 1 < m_payload.size()) {
                word = (static_cast<uint16_t>(m_payload[i]) << 8) | m_payload[i + 1];
            } else {
                word = static_cast<uint16_t>(m_payload[i]) << 8;
            }
            sum += word;
        }
        
        // Aplica carry
        while (sum >> 16) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        
        // Complemento de 1
        return static_cast<uint16_t>(~sum);
    }

private:
    PacketHeader m_header;           ///< Cabeçalho do pacote
    std::vector<uint8_t> m_payload;  ///< Payload do pacote
};

/**
 * @brief Representa uma conexão de rede
 */
struct ConnectionInfo {
    uint32_t id;                                  ///< ID da conexão
    std::string address;                          ///< Endereço (IP:porta)
    SocketType type;                              ///< Tipo de socket
    ConnectionState state;                        ///< Estado da conexão
    std::chrono::steady_clock::time_point connectTime; ///< Momento da conexão
    std::chrono::steady_clock::time_point lastActivity; ///< Última atividade
    uint32_t bytesSent;                           ///< Bytes enviados
    uint32_t bytesReceived;                       ///< Bytes recebidos
    uint16_t packetsSent;                         ///< Pacotes enviados
    uint16_t packetsReceived;                     ///< Pacotes recebidos
    uint16_t pingMs;                              ///< Ping em ms
    bool encrypted;                               ///< Conexão criptografada
    bool authenticated;                           ///< Conexão autenticada
    std::string clientVersion;                    ///< Versão do cliente
};

/**
 * @brief Representa um endereço de rede
 */
class NetworkAddress {
public:
    /**
     * @brief Construtor padrão
     */
    NetworkAddress() : m_ip("0.0.0.0"), m_port(0) {}

    /**
     * @brief Construtor com IP e porta
     * @param ip Endereço IP
     * @param port Porta
     */
    NetworkAddress(const std::string& ip, uint16_t port) : m_ip(ip), m_port(port) {}

    /**
     * @brief Construtor a partir de string no formato "IP:porta"
     * @param address String no formato "IP:porta"
     */
    explicit NetworkAddress(const std::string& address) {
        size_t pos = address.find_last_of(':');
        if (pos != std::string::npos) {
            m_ip = address.substr(0, pos);
            try {
                m_port = static_cast<uint16_t>(std::stoi(address.substr(pos + 1)));
            } catch (...) {
                m_port = 0;
            }
        } else {
            m_ip = address;
            m_port = 0;
        }
    }

    /**
     * @brief Obtém o endereço IP
     * @return Endereço IP
     */
    const std::string& getIP() const {
        return m_ip;
    }

    /**
     * @brief Define o endereço IP
     * @param ip Novo endereço IP
     */
    void setIP(const std::string& ip) {
        m_ip = ip;
    }

    /**
     * @brief Obtém a porta
     * @return Porta
     */
    uint16_t getPort() const {
        return m_port;
    }

    /**
     * @brief Define a porta
     * @param port Nova porta
     */
    void setPort(uint16_t port) {
        m_port = port;
    }

    /**
     * @brief Obtém a representação em string
     * @return String no formato "IP:porta"
     */
    std::string toString() const {
        return m_ip + ":" + std::to_string(m_port);
    }

    /**
     * @brief Operador de igualdade
     * @param other Outro endereço para comparação
     * @return true se os endereços são iguais
     */
    bool operator==(const NetworkAddress& other) const {
        return m_ip == other.m_ip && m_port == other.m_port;
    }

    /**
     * @brief Operador de desigualdade
     * @param other Outro endereço para comparação
     * @return true se os endereços são diferentes
     */
    bool operator!=(const NetworkAddress& other) const {
        return !(*this == other);
    }

private:
    std::string m_ip;  ///< Endereço IP
    uint16_t m_port;   ///< Porta
};

/**
 * @brief Functor hash para NetworkAddress
 */
struct NetworkAddressHash {
    std::size_t operator()(const NetworkAddress& address) const {
        return std::hash<std::string>()(address.toString());
    }
};

} // namespace network
} // namespace server
} // namespace wydbr

#endif // WYDBR_NETWORKTYPES_H 