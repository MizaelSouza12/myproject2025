/**
 * NetworkProtocol.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/reverse/network/NetworkProtocol.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef NETWORKPROTOCOL_H
#define NETWORKPROTOCOL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <array>
#include <bitset>
#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <cstdint>
#include <chrono>

namespace wydbr {


/**
 * @file NetworkProtocol.h
 * @brief Sistema avançado de protocolo de rede reconstruído do WYD original
 * 
 * Este arquivo contém a reconstrução do protocolo de rede original do WYD,
 * incluindo estruturas de pacotes, algoritmos de criptografia, compressão
 * e sistemas anti-cheat. Baseado em engenharia reversa completa do executável.
 */

namespace wyd {
namespace reverse {
namespace network {

// Constantes do protocolo original
constexpr uint16_t WYD_DEFAULT_PORT = 8281;       // Porta padrão
constexpr uint16_t WYD_LOGIN_PORT = 8282;         // Porta de login
constexpr uint32_t WYD_PROTOCOL_VERSION = 4012;   // Versão do protocolo
constexpr uint32_t WYD_PACKET_MAX_SIZE = 8192;    // Tamanho máximo de pacote
constexpr uint32_t WYD_HEADER_SIZE = 12;          // Tamanho do cabeçalho
constexpr uint32_t WYD_CHECKSUM_SALT = 0x5A2D1F8E; // Salt do checksum
constexpr uint32_t WYD_ENCRYPTION_KEY = 0x1F2A3D4C; // Chave de criptografia
constexpr uint32_t WYD_PACKET_QUEUE_SIZE = 128;   // Tamanho da fila de pacotes
constexpr uint32_t WYD_TIMEOUT_MS = 30000;        // Timeout (30 segundos)
constexpr uint32_t WYD_KEEPALIVE_INTERVAL_MS = 10000; // Intervalo de keepalive (10 segundos)
constexpr uint32_t WYD_RECONNECT_ATTEMPTS = 5;    // Tentativas de reconexão
constexpr uint32_t WYD_COMPRESSION_THRESHOLD = 128; // Limite de compressão

/**
 * @brief Tipo de pacote
 */
enum class PacketType : uint16_t {
    // Pacotes gerais
    NONE = 0x0000,                     // Nenhum
    KEEPALIVE = 0x0001,                // Keepalive
    DISCONNECT = 0x0002,               // Desconexão
    HANDSHAKE = 0x0003,                // Handshake
    HANDSHAKE_RESPONSE = 0x0004,       // Resposta de handshake
    ENCRYPTION_KEY = 0x0005,           // Chave de criptografia
    ERROR = 0x0006,                    // Erro
    
    // Login e autenticação
    LOGIN_REQUEST = 0x1001,            // Requisição de login
    LOGIN_RESPONSE = 0x1002,           // Resposta de login
    CREATE_ACCOUNT_REQUEST = 0x1003,   // Requisição de criação de conta
    CREATE_ACCOUNT_RESPONSE = 0x1004,  // Resposta de criação de conta
    CHANGE_PASSWORD_REQUEST = 0x1005,  // Requisição de mudança de senha
    CHANGE_PASSWORD_RESPONSE = 0x1006, // Resposta de mudança de senha
    SELECT_SERVER_REQUEST = 0x1007,    // Requisição de seleção de servidor
    SELECT_SERVER_RESPONSE = 0x1008,   // Resposta de seleção de servidor
    SERVER_LIST_REQUEST = 0x1009,      // Requisição de lista de servidores
    SERVER_LIST_RESPONSE = 0x100A,     // Resposta de lista de servidores
    
    // Criação e seleção de personagem
    CHARACTER_LIST_REQUEST = 0x2001,   // Requisição de lista de personagens
    CHARACTER_LIST_RESPONSE = 0x2002,  // Resposta de lista de personagens
    CREATE_CHARACTER_REQUEST = 0x2003, // Requisição de criação de personagem
    CREATE_CHARACTER_RESPONSE = 0x2004,// Resposta de criação de personagem
    DELETE_CHARACTER_REQUEST = 0x2005, // Requisição de exclusão de personagem
    DELETE_CHARACTER_RESPONSE = 0x2006,// Resposta de exclusão de personagem
    SELECT_CHARACTER_REQUEST = 0x2007, // Requisição de seleção de personagem
    SELECT_CHARACTER_RESPONSE = 0x2008,// Resposta de seleção de personagem
    
    // Movimento e posição
    POSITION_UPDATE = 0x3001,          // Atualização de posição
    POSITION_SYNC = 0x3002,            // Sincronização de posição
    MOVE_REQUEST = 0x3003,             // Requisição de movimento
    MOVE_RESPONSE = 0x3004,            // Resposta de movimento
    TELEPORT_REQUEST = 0x3005,         // Requisição de teleporte
    TELEPORT_RESPONSE = 0x3006,        // Resposta de teleporte
    
    // Chat e comunicação
    CHAT_NORMAL = 0x4001,              // Chat normal
    CHAT_WHISPER = 0x4002,             // Chat sussurro
    CHAT_PARTY = 0x4003,               // Chat de grupo
    CHAT_GUILD = 0x4004,               // Chat de guilda
    CHAT_GLOBAL = 0x4005,              // Chat global
    CHAT_SYSTEM = 0x4006,              // Chat de sistema
    CHAT_GM = 0x4007,                  // Chat de GM
    
    // Combate
    ATTACK_REQUEST = 0x5001,           // Requisição de ataque
    ATTACK_RESPONSE = 0x5002,          // Resposta de ataque
    SKILL_REQUEST = 0x5003,            // Requisição de habilidade
    SKILL_RESPONSE = 0x5004,           // Resposta de habilidade
    DAMAGE_NOTIFICATION = 0x5005,      // Notificação de dano
    DEATH_NOTIFICATION = 0x5006,       // Notificação de morte
    RESURRECT_REQUEST = 0x5007,        // Requisição de ressurreição
    RESURRECT_RESPONSE = 0x5008,       // Resposta de ressurreição
    
    // Itens e inventário
    INVENTORY_REQUEST = 0x6001,        // Requisição de inventário
    INVENTORY_RESPONSE = 0x6002,       // Resposta de inventário
    ITEM_USE_REQUEST = 0x6003,         // Requisição de uso de item
    ITEM_USE_RESPONSE = 0x6004,        // Resposta de uso de item
    ITEM_DROP_REQUEST = 0x6005,        // Requisição de descarte de item
    ITEM_DROP_RESPONSE = 0x6006,       // Resposta de descarte de item
    ITEM_PICKUP_REQUEST = 0x6007,      // Requisição de coleta de item
    ITEM_PICKUP_RESPONSE = 0x6008,     // Resposta de coleta de item
    ITEM_MOVE_REQUEST = 0x6009,        // Requisição de movimentação de item
    ITEM_MOVE_RESPONSE = 0x600A,       // Resposta de movimentação de item
    
    // Comércio e mercado
    TRADE_REQUEST = 0x7001,            // Requisição de troca
    TRADE_RESPONSE = 0x7002,           // Resposta de troca
    TRADE_ITEM_ADD = 0x7003,           // Adição de item à troca
    TRADE_ITEM_REMOVE = 0x7004,        // Remoção de item da troca
    TRADE_CONFIRM = 0x7005,            // Confirmação de troca
    TRADE_CANCEL = 0x7006,             // Cancelamento de troca
    TRADE_COMPLETE = 0x7007,           // Conclusão de troca
    SHOP_LIST_REQUEST = 0x7008,        // Requisição de lista de loja
    SHOP_LIST_RESPONSE = 0x7009,       // Resposta de lista de loja
    SHOP_BUY_REQUEST = 0x700A,         // Requisição de compra de loja
    SHOP_BUY_RESPONSE = 0x700B,        // Resposta de compra de loja
    SHOP_SELL_REQUEST = 0x700C,        // Requisição de venda de loja
    SHOP_SELL_RESPONSE = 0x700D,       // Resposta de venda de loja
    
    // Grupo e raid
    PARTY_INVITE_REQUEST = 0x8001,     // Requisição de convite para grupo
    PARTY_INVITE_RESPONSE = 0x8002,    // Resposta de convite para grupo
    PARTY_LEAVE_REQUEST = 0x8003,      // Requisição de saída de grupo
    PARTY_LEAVE_RESPONSE = 0x8004,     // Resposta de saída de grupo
    PARTY_KICK_REQUEST = 0x8005,       // Requisição de expulsão de grupo
    PARTY_KICK_RESPONSE = 0x8006,      // Resposta de expulsão de grupo
    PARTY_DISSOLVE_REQUEST = 0x8007,   // Requisição de dissolução de grupo
    PARTY_DISSOLVE_RESPONSE = 0x8008,  // Resposta de dissolução de grupo
    
    // Guilda
    GUILD_CREATE_REQUEST = 0x9001,     // Requisição de criação de guilda
    GUILD_CREATE_RESPONSE = 0x9002,    // Resposta de criação de guilda
    GUILD_INVITE_REQUEST = 0x9003,     // Requisição de convite para guilda
    GUILD_INVITE_RESPONSE = 0x9004,    // Resposta de convite para guilda
    GUILD_LEAVE_REQUEST = 0x9005,      // Requisição de saída de guilda
    GUILD_LEAVE_RESPONSE = 0x9006,     // Resposta de saída de guilda
    GUILD_KICK_REQUEST = 0x9007,       // Requisição de expulsão de guilda
    GUILD_KICK_RESPONSE = 0x9008,      // Resposta de expulsão de guilda
    GUILD_DISSOLVE_REQUEST = 0x9009,   // Requisição de dissolução de guilda
    GUILD_DISSOLVE_RESPONSE = 0x900A,  // Resposta de dissolução de guilda
    GUILD_INFO_REQUEST = 0x900B,       // Requisição de informações de guilda
    GUILD_INFO_RESPONSE = 0x900C,      // Resposta de informações de guilda
    
    // Amigos e social
    FRIEND_LIST_REQUEST = 0xA001,      // Requisição de lista de amigos
    FRIEND_LIST_RESPONSE = 0xA002,     // Resposta de lista de amigos
    FRIEND_ADD_REQUEST = 0xA003,       // Requisição de adição de amigo
    FRIEND_ADD_RESPONSE = 0xA004,      // Resposta de adição de amigo
    FRIEND_REMOVE_REQUEST = 0xA005,    // Requisição de remoção de amigo
    FRIEND_REMOVE_RESPONSE = 0xA006,   // Resposta de remoção de amigo
    FRIEND_STATUS_UPDATE = 0xA007,     // Atualização de status de amigo
    
    // Quests e missões
    QUEST_LIST_REQUEST = 0xB001,       // Requisição de lista de quests
    QUEST_LIST_RESPONSE = 0xB002,      // Resposta de lista de quests
    QUEST_ACCEPT_REQUEST = 0xB003,     // Requisição de aceitação de quest
    QUEST_ACCEPT_RESPONSE = 0xB004,    // Resposta de aceitação de quest
    QUEST_COMPLETE_REQUEST = 0xB005,   // Requisição de conclusão de quest
    QUEST_COMPLETE_RESPONSE = 0xB006,  // Resposta de conclusão de quest
    QUEST_ABANDON_REQUEST = 0xB007,    // Requisição de abandono de quest
    QUEST_ABANDON_RESPONSE = 0xB008,   // Resposta de abandono de quest
    QUEST_PROGRESS_UPDATE = 0xB009,    // Atualização de progresso de quest
    
    // Outros
    PING_REQUEST = 0xF001,             // Requisição de ping
    PING_RESPONSE = 0xF002,            // Resposta de ping
    ANTICHEAT_REQUEST = 0xF003,        // Requisição de anticheat
    ANTICHEAT_RESPONSE = 0xF004,       // Resposta de anticheat
};

/**
 * @brief Código de erro de rede
 */
enum class NetworkError : uint16_t {
    NONE = 0x0000,                     // Nenhum
    UNKNOWN = 0x0001,                  // Desconhecido
    CONNECTION_FAILED = 0x0002,        // Falha na conexão
    CONNECTION_TIMEOUT = 0x0003,       // Timeout de conexão
    DISCONNECTED = 0x0004,             // Desconectado
    INVALID_PACKET = 0x0005,           // Pacote inválido
    INVALID_CHECKSUM = 0x0006,         // Checksum inválido
    ENCRYPTION_FAILED = 0x0007,        // Falha na criptografia
    DECRYPTION_FAILED = 0x0008,        // Falha na descriptografia
    COMPRESSION_FAILED = 0x0009,       // Falha na compressão
    DECOMPRESSION_FAILED = 0x000A,     // Falha na descompressão
    INVALID_PROTOCOL_VERSION = 0x000B, // Versão de protocolo inválida
    SERVER_FULL = 0x000C,              // Servidor cheio
    ACCOUNT_IN_USE = 0x000D,           // Conta em uso
    INVALID_CREDENTIALS = 0x000E,      // Credenciais inválidas
    BANNED = 0x000F,                   // Banido
    MAINTENANCE = 0x0010,              // Manutenção
    INVALID_CHARACTER = 0x0011,        // Personagem inválido
    INVALID_POSITION = 0x0012,         // Posição inválida
    INVALID_ITEM = 0x0013,             // Item inválido
    INSUFFICIENT_FUNDS = 0x0014,       // Fundos insuficientes
    INSUFFICIENT_SPACE = 0x0015,       // Espaço insuficiente
    INSUFFICIENT_LEVEL = 0x0016,       // Nível insuficiente
    INVALID_TARGET = 0x0017,           // Alvo inválido
    TARGET_NOT_FOUND = 0x0018,         // Alvo não encontrado
    TOO_FAR = 0x0019,                  // Muito longe
    ALREADY_DEAD = 0x001A,             // Já morto
    ALREADY_IN_PARTY = 0x001B,         // Já em grupo
    ALREADY_IN_GUILD = 0x001C,         // Já em guilda
    NOT_IN_PARTY = 0x001D,             // Não está em grupo
    NOT_IN_GUILD = 0x001E,             // Não está em guilda
    NOT_PARTY_LEADER = 0x001F,         // Não é líder do grupo
    NOT_GUILD_LEADER = 0x0020,         // Não é líder da guilda
    TRADE_BUSY = 0x0021,               // Ocupado em troca
    TRADE_CANCELLED = 0x0022,          // Troca cancelada
    QUEST_NOT_AVAILABLE = 0x0023,      // Quest não disponível
    QUEST_ALREADY_ACTIVE = 0x0024,     // Quest já ativa
    QUEST_NOT_COMPLETE = 0x0025,       // Quest não completa
    CHEAT_DETECTED = 0x0026,           // Cheat detectado
    PERMISSION_DENIED = 0x0027,        // Permissão negada
};

/**
 * @brief Status de conexão
 */
enum class ConnectionStatus : uint8_t {
    DISCONNECTED = 0,                  // Desconectado
    CONNECTING = 1,                    // Conectando
    AUTHENTICATING = 2,                // Autenticando
    CONNECTED = 3,                     // Conectado
    RECONNECTING = 4,                  // Reconectando
    DISCONNECTING = 5,                 // Desconectando
    ERROR = 6,                         // Erro
};

/**
 * @brief Flag de pacote
 */
enum class PacketFlag : uint8_t {
    NONE = 0x00,                       // Nenhuma
    ENCRYPTED = 0x01,                  // Criptografado
    COMPRESSED = 0x02,                 // Comprimido
    PRIORITY = 0x04,                   // Prioritário
    RELIABLE = 0x08,                   // Confiável
    FRAGMENTED = 0x10,                 // Fragmentado
    LAST_FRAGMENT = 0x20,              // Último fragmento
    ANTI_CHEAT = 0x40,                 // Anti-cheat
    SYSTEM = 0x80,                     // Sistema
};

/**
 * @brief Resultado de autenticação
 */
enum class AuthResult : uint8_t {
    SUCCESS = 0,                       // Sucesso
    INVALID_CREDENTIALS = 1,           // Credenciais inválidas
    ACCOUNT_IN_USE = 2,                // Conta em uso
    BANNED = 3,                        // Banido
    MAINTENANCE = 4,                   // Manutenção
    SERVER_FULL = 5,                   // Servidor cheio
    INVALID_VERSION = 6,               // Versão inválida
    UNKNOWN_ERROR = 7,                 // Erro desconhecido
};

/**
 * @brief Modo de criptografia
 */
enum class EncryptionMode : uint8_t {
    NONE = 0,                          // Nenhum
    XOR = 1,                           // XOR
    RC4 = 2,                           // RC4
    BLOWFISH = 3,                      // Blowfish
    AES = 4,                           // AES
};

/**
 * @brief Modo de compressão
 */
enum class CompressionMode : uint8_t {
    NONE = 0,                          // Nenhum
    ZLIB = 1,                          // ZLIB
    LZO = 2,                           // LZO
    LZ4 = 3,                           // LZ4
    CUSTOM = 4,                        // Personalizado
};

/**
 * @brief Cabeçalho de pacote
 */
#pragma pack(push, 1)
struct PacketHeader {
    uint16_t size;                      // Tamanho
    uint16_t type;                      // Tipo
    uint32_t sequence;                  // Sequência
    uint8_t flags;                      // Flags
    uint8_t fragments;                  // Fragmentos
    uint16_t checksum;                  // Checksum
    
    PacketHeader()
        : size(0)
        , type(0)
        , sequence(0)
        , flags(0)
        , fragments(0)
        , checksum(0)
    {}
};
#pragma pack(pop)

/**
 * @brief Pacote
 */
struct Packet {
    PacketHeader header;                // Cabeçalho
    std::vector<uint8_t> data;          // Dados
    uint32_t timestamp;                 // Timestamp
    uint8_t attempts;                   // Tentativas
    
    Packet()
        : header()
        , data()
        , timestamp(0)
        , attempts(0)
    {}
    
    Packet(PacketType type, const void* packetData, uint16_t packetSize)
        : header()
        , data(static_cast<const uint8_t*>(packetData), static_cast<const uint8_t*>(packetData) + packetSize)
        , timestamp(0)
        , attempts(0)
    {
        header.size = packetSize + sizeof(PacketHeader);
        header.type = static_cast<uint16_t>(type);
    }
};

/**
 * @brief Gerenciador de protocolo
 */
/**
 * Classe WYDNetworkProtocol
 * 
 * Esta classe implementa a funcionalidade WYDNetworkProtocol conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class NetworkProtocol {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static NetworkProtocol& GetInstance();
    
    /**
     * @brief Inicializa o protocolo
     * @param mode Modo de criptografia
     * @param key Chave de criptografia
     * @param compressionMode Modo de compressão
     * @param protocolVersion Versão do protocolo
     * @return true se inicializado com sucesso
     */
    bool Initialize(EncryptionMode mode = EncryptionMode::XOR, 
                  uint32_t key = WYD_ENCRYPTION_KEY,
                  CompressionMode compressionMode = CompressionMode::ZLIB,
                  uint32_t protocolVersion = WYD_PROTOCOL_VERSION);
    
    /**
     * @brief Finaliza o protocolo
     */
    void Shutdown();
    
    /**
     * @brief Codifica um pacote para envio
     * @param packet Pacote
     * @param outBuffer Buffer de saída
     * @param outSize Tamanho de saída
     * @return true se codificado com sucesso
     */
    bool EncodePacket(const Packet& packet, uint8_t*& outBuffer, uint32_t& outSize);
    
    /**
     * @brief Decodifica um pacote recebido
     * @param buffer Buffer
     * @param size Tamanho
     * @param outPacket Pacote de saída
     * @return true se decodificado com sucesso
     */
    bool DecodePacket(const uint8_t* buffer, uint32_t size, Packet& outPacket);
    
    /**
     * @brief Cria um pacote
     * @param type Tipo
     * @param data Dados
     * @param size Tamanho
     * @param flags Flags
     * @return Pacote criado
     */
    Packet CreatePacket(PacketType type, const void* data, uint16_t size, uint8_t flags = 0);
    
    /**
     * @brief Cria um pacote de erro
     * @param errorCode Código de erro
     * @param message Mensagem
     * @return Pacote criado
     */
    Packet CreateErrorPacket(NetworkError errorCode, const std::string& message = "");
    
    /**
     * @brief Cria um pacote de keepalive
     * @return Pacote criado
     */
    Packet CreateKeepAlivePacket();
    
    /**
     * @brief Cria um pacote de handshake
     * @param clientVersion Versão do cliente
     * @param clientId ID do cliente
     * @return Pacote criado
     */
    Packet CreateHandshakePacket(uint32_t clientVersion, uint32_t clientId);
    
    /**
     * @brief Cria um pacote de resposta de handshake
     * @param serverVersion Versão do servidor
     * @param serverId ID do servidor
     * @param sessionKey Chave de sessão
     * @return Pacote criado
     */
    Packet CreateHandshakeResponsePacket(uint32_t serverVersion, uint32_t serverId, uint32_t sessionKey);
    
    /**
     * @brief Calcula o checksum de um pacote
     * @param packet Pacote
     * @return Checksum calculado
     */
    uint16_t CalculateChecksum(const Packet& packet);
    
    /**
     * @brief Verifica o checksum de um pacote
     * @param packet Pacote
     * @return true se o checksum for válido
     */
    bool VerifyChecksum(const Packet& packet);
    
    /**
     * @brief Fragmenta um pacote grande
     * @param packet Pacote
     * @param maxSize Tamanho máximo
     * @return Lista de pacotes fragmentados
     */
    std::vector<Packet> FragmentPacket(const Packet& packet, uint16_t maxSize = WYD_PACKET_MAX_SIZE);
    
    /**
     * @brief Reúne um pacote fragmentado
     * @param fragments Fragmentos
     * @param outPacket Pacote de saída
     * @return true se reunido com sucesso
     */
    bool ReassemblePacket(const std::vector<Packet>& fragments, Packet& outPacket);
    
    /**
     * @brief Define o modo de criptografia
     * @param mode Modo
     * @param key Chave
     * @return true se definido com sucesso
     */
    bool SetEncryptionMode(EncryptionMode mode, uint32_t key);
    
    /**
     * @brief Define o modo de compressão
     * @param mode Modo
     * @return true se definido com sucesso
     */
    bool SetCompressionMode(CompressionMode mode);
    
    /**
     * @brief Define a versão do protocolo
     * @param version Versão
     */
    void SetProtocolVersion(uint32_t version);
    
    /**
     * @brief Obtém a versão do protocolo
     * @return Versão
     */
    uint32_t GetProtocolVersion() const;
    
    /**
     * @brief Criptografa dados
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se criptografado com sucesso
     */
    bool EncryptData(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Descriptografa dados
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se descriptografado com sucesso
     */
    bool DecryptData(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Comprime dados
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se comprimido com sucesso
     */
    bool CompressData(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Descomprime dados
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se descomprimido com sucesso
     */
    bool DecompressData(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Gera uma chave de sessão
     * @return Chave gerada
     */
    uint32_t GenerateSessionKey();
    
    /**
     * @brief Ativa a detecção de cheat
     * @param enabled Habilitado
     */
    void EnableCheatDetection(bool enabled);
    
    /**
     * @brief Verifica se a detecção de cheat está ativada
     * @return true se ativada
     */
    bool IsCheatDetectionEnabled() const;
    
    /**
     * @brief Verifica se o protocolo foi inicializado
     * @return true se inicializado
     */
    bool IsInitialized() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    NetworkProtocol();
    
    /**
     * @brief Destrutor
     */
    ~NetworkProtocol();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    NetworkProtocol(const NetworkProtocol&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    NetworkProtocol& operator=(const NetworkProtocol&) = delete;
    
    /**
     * @brief Criptografa dados com XOR
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     * @param outData Dados de saída
     * @return true se criptografado com sucesso
     */
    bool EncryptXOR(const uint8_t* data, uint32_t size, uint32_t key, uint8_t* outData);
    
    /**
     * @brief Criptografa dados com RC4
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     * @param outData Dados de saída
     * @return true se criptografado com sucesso
     */
    bool EncryptRC4(const uint8_t* data, uint32_t size, uint32_t key, uint8_t* outData);
    
    /**
     * @brief Criptografa dados com Blowfish
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se criptografado com sucesso
     */
    bool EncryptBlowfish(const uint8_t* data, uint32_t size, uint32_t key, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Descriptografa dados com Blowfish
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se descriptografado com sucesso
     */
    bool DecryptBlowfish(const uint8_t* data, uint32_t size, uint32_t key, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Comprime dados com ZLIB
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se comprimido com sucesso
     */
    bool CompressZLIB(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Descomprime dados com ZLIB
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se descomprimido com sucesso
     */
    bool DecompressZLIB(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Comprime dados com LZO
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se comprimido com sucesso
     */
    bool CompressLZO(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Descomprime dados com LZO
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se descomprimido com sucesso
     */
    bool DecompressLZO(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Comprime dados com LZ4
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se comprimido com sucesso
     */
    bool CompressLZ4(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Descomprime dados com LZ4
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se descomprimido com sucesso
     */
    bool DecompressLZ4(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Comprime dados com o algoritmo personalizado do WYD
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se comprimido com sucesso
     */
    bool CompressCustom(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Descomprime dados com o algoritmo personalizado do WYD
     * @param data Dados
     * @param size Tamanho
     * @param outData Dados de saída
     * @param outSize Tamanho de saída
     * @return true se descomprimido com sucesso
     */
    bool DecompressCustom(const uint8_t* data, uint32_t size, uint8_t*& outData, uint32_t& outSize);
    
    /**
     * @brief Inicializa a tabela RC4
     * @param key Chave
     * @param keyLength Tamanho da chave
     * @param sbox S-box (saída)
     */
    void InitRC4(const uint8_t* key, uint32_t keyLength, uint8_t* sbox);
    
    /**
     * @brief Criptografa ou descriptografa com RC4
     * @param data Dados
     * @param size Tamanho
     * @param sbox S-box
     * @param outData Dados de saída
     */
    void RC4(const uint8_t* data, uint32_t size, uint8_t* sbox, uint8_t* outData);
    
    /**
     * @brief Inicializa as estruturas Blowfish
     * @param key Chave
     * @param keyLength Tamanho da chave
     */
    void InitBlowfish(const uint8_t* key, uint32_t keyLength);
    
    /**
     * @brief Criptografa um bloco com Blowfish
     * @param l Esquerda
     * @param r Direita
     */
    void BlowfishEncryptBlock(uint32_t& l, uint32_t& r);
    
    /**
     * @brief Descriptografa um bloco com Blowfish
     * @param l Esquerda
     * @param r Direita
     */
    void BlowfishDecryptBlock(uint32_t& l, uint32_t& r);
    
    /**
     * @brief Função F do Blowfish
     * @param x Entrada
     * @return Saída
     */
    uint32_t BlowfishF(uint32_t x);
    
    // Configuração
    EncryptionMode encryptionMode_;
    CompressionMode compressionMode_;
    uint32_t encryptionKey_;
    uint32_t protocolVersion_;
    bool cheatDetectionEnabled_;
    
    // Estado
    bool initialized_;
    uint32_t sequenceCounter_;
    
    // Estruturas Blowfish
    std::array<uint32_t, 18> blowfishP_;
    std::array<std::array<uint32_t, 256>, 4> blowfishS_;
    
    // Mutex para sincronização
    std::mutex mutex_;
};

/**
 * @brief Interface de socket de rede
 */
/**
 * Classe WYDINetworkSocket
 * 
 * Esta classe implementa a funcionalidade WYDINetworkSocket conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class INetworkSocket {
public:
    /**
     * @brief Destrutor
     */
    virtual ~INetworkSocket() {}
    
    /**
     * @brief Inicializa o socket
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Finaliza o socket
     */
    virtual void Shutdown() = 0;
    
    /**
     * @brief Conecta a um servidor
     * @param host Endereço
     * @param port Porta
     * @return true se conectado com sucesso
     */
    virtual bool Connect(const std::string& host, uint16_t port) = 0;
    
    /**
     * @brief Desconecta do servidor
     */
    virtual void Disconnect() = 0;
    
    /**
     * @brief Envia dados
     * @param data Dados
     * @param size Tamanho
     * @return Bytes enviados, ou -1 se falhar
     */
    virtual int Send(const void* data, int size) = 0;
    
    /**
     * @brief Recebe dados
     * @param data Buffer
     * @param size Tamanho máximo
     * @return Bytes recebidos, ou -1 se falhar
     */
    virtual int Receive(void* data, int size) = 0;
    
    /**
     * @brief Verifica se o socket está conectado
     * @return true se conectado
     */
    virtual bool IsConnected() const = 0;
    
    /**
     * @brief Obtém o endereço remoto
     * @return Endereço
     */
    virtual std::string GetRemoteAddress() const = 0;
    
    /**
     * @brief Obtém a porta remota
     * @return Porta
     */
    virtual uint16_t GetRemotePort() const = 0;
    
    /**
     * @brief Define o timeout de recebimento
     * @param milliseconds Milissegundos
     */
    virtual void SetReceiveTimeout(int milliseconds) = 0;
    
    /**
     * @brief Define o timeout de envio
     * @param milliseconds Milissegundos
     */
    virtual void SetSendTimeout(int milliseconds) = 0;
    
    /**
     * @brief Define o modo de bloqueio
     * @param blocking Bloqueante
     */
    virtual void SetBlocking(bool blocking) = 0;
};

/**
 * @brief Gerenciador de conexão
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
     * @brief Callback de recebimento de pacote
     */
    using PacketReceiveCallback = std::function<void(const Packet&)>;
    
    /**
     * @brief Callback de mudança de status
     */
    using StatusChangeCallback = std::function<void(ConnectionStatus, ConnectionStatus, const std::string&)>;
    
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static ConnectionManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param protocol Protocolo
     * @return true se inicializado com sucesso
     */
    bool Initialize(NetworkProtocol* protocol = nullptr);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Conecta a um servidor
     * @param host Endereço
     * @param port Porta
     * @return true se conectado com sucesso
     */
    bool Connect(const std::string& host, uint16_t port);
    
    /**
     * @brief Desconecta do servidor
     */
    void Disconnect();
    
    /**
     * @brief Envia um pacote
     * @param packet Pacote
     * @return true se enviado com sucesso
     */
    bool SendPacket(const Packet& packet);
    
    /**
     * @brief Processa pacotes recebidos
     */
    void ProcessPackets();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo delta
     */
    void Update(float deltaTime);
    
    /**
     * @brief Registra um callback de recebimento de pacote
     * @param callback Callback
     * @param type Tipo de pacote (0 para todos)
     * @return ID do callback
     */
    int RegisterPacketCallback(PacketReceiveCallback callback, uint16_t type = 0);
    
    /**
     * @brief Remove um callback de recebimento de pacote
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterPacketCallback(int callbackId);
    
    /**
     * @brief Registra um callback de mudança de status
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterStatusCallback(StatusChangeCallback callback);
    
    /**
     * @brief Remove um callback de mudança de status
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterStatusCallback(int callbackId);
    
    /**
     * @brief Obtém o status da conexão
     * @return Status
     */
    ConnectionStatus GetStatus() const;
    
    /**
     * @brief Obtém o endereço do servidor
     * @return Endereço
     */
    std::string GetServerAddress() const;
    
    /**
     * @brief Obtém a porta do servidor
     * @return Porta
     */
    uint16_t GetServerPort() const;
    
    /**
     * @brief Obtém o ping
     * @return Ping em milissegundos
     */
    uint32_t GetPing() const;
    
    /**
     * @brief Verifica se está conectado
     * @return true se conectado
     */
    bool IsConnected() const;
    
    /**
     * @brief Envia um pacote de keepalive
     * @return true se enviado com sucesso
     */
    bool SendKeepAlive();
    
    /**
     * @brief Envia um pacote de ping
     * @return true se enviado com sucesso
     */
    bool SendPing();
    
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
     * @brief Thread de recebimento
     */
    void ReceiveThread();
    
    /**
     * @brief Processa um pacote recebido
     * @param packet Pacote
     */
    void ProcessReceivedPacket(const Packet& packet);
    
    /**
     * @brief Processa um pacote de erro
     * @param packet Pacote
     */
    void ProcessErrorPacket(const Packet& packet);
    
    /**
     * @brief Processa um pacote de resposta de ping
     * @param packet Pacote
     */
    void ProcessPingResponse(const Packet& packet);
    
    /**
     * @brief Define o status da conexão
     * @param status Status
     * @param reason Razão
     */
    void SetStatus(ConnectionStatus status, const std::string& reason = "");
    
    /**
     * @brief Tenta reconectar
     * @return true se reconectado com sucesso
     */
    bool TryReconnect();
    
    /**
     * @brief Cria um socket
     * @return Ponteiro para o socket
     */
    INetworkSocket* CreateSocket();
    
    /**
     * @brief Envia pacotes da fila
     */
    void SendQueuedPackets();
    
    /**
     * @brief Envia pacotes prioritários
     */
    void SendPriorityPackets();
    
    /**
     * @brief Verifica pacotes com timeout
     */
    void CheckTimeoutPackets();
    
    /**
     * @brief Notifica callbacks de pacote
     * @param packet Pacote
     */
    void NotifyPacketCallbacks(const Packet& packet);
    
    /**
     * @brief Notifica callbacks de status
     * @param oldStatus Status antigo
     * @param newStatus Status novo
     * @param reason Razão
     */
    void NotifyStatusCallbacks(ConnectionStatus oldStatus, ConnectionStatus newStatus, const std::string& reason);
    
    // Socket
    std::unique_ptr<INetworkSocket> socket_;
    
    // Protocolo
    NetworkProtocol* protocol_;
    bool ownProtocol_;
    
    // Fila de pacotes
    std::queue<Packet> sendQueue_;
    std::queue<Packet> priorityQueue_;
    std::queue<Packet> receiveQueue_;
    std::mutex sendQueueMutex_;
    std::mutex priorityQueueMutex_;
    std::mutex receiveQueueMutex_;
    
    // Pacotes confiáveis
    std::map<uint32_t, Packet> reliablePackets_;
    std::mutex reliablePacketsMutex_;
    
    // Pacotes fragmentados
    std::map<uint32_t, std::vector<Packet>> fragmentedPackets_;
    std::mutex fragmentedPacketsMutex_;
    
    // Callbacks
    std::map<int, std::pair<PacketReceiveCallback, uint16_t>> packetCallbacks_;
    std::mutex packetCallbacksMutex_;
    int nextPacketCallbackId_;
    
    std::map<int, StatusChangeCallback> statusCallbacks_;
    std::mutex statusCallbacksMutex_;
    int nextStatusCallbackId_;
    
    // Thread de recebimento
    std::thread receiveThread_;
    std::atomic<bool> running_;
    
    // Estado
    ConnectionStatus status_;
    std::string serverAddress_;
    uint16_t serverPort_;
    uint32_t ping_;
    uint32_t lastKeepAliveTime_;
    uint32_t lastPingTime_;
    uint32_t reconnectAttempts_;
    uint32_t sessionKey_;
    
    // Estatísticas
    uint32_t packetsSent_;
    uint32_t packetsReceived_;
    uint32_t bytesReceived_;
    uint32_t bytesSent_;
    uint32_t packetsLost_;
    float packetLossRate_;
    
    // Mutex para sincronização
    std::mutex mutex_;
};

// Acesso global
#define g_NetworkProtocol NetworkProtocol::GetInstance()
#define g_ConnectionManager ConnectionManager::GetInstance()

} // namespace network
} // namespace reverse
} // namespace wyd

#endif // NETWORKPROTOCOL_H

} // namespace wydbr
