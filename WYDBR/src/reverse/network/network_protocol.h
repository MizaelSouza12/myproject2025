/**
 * @file network_protocol.h
 * @brief Definição do protocolo de rede para o WYDBR
 * 
 * Este arquivo define as interfaces e estruturas do protocolo de rede do WYDBR,
 * com base na engenharia reversa do sistema de sincronização preditiva do WYD MODERN 2025 STUDIO.
 * Implementa sincronização avançada, compressão de dados e recuperação de falhas.
 * 
 * @author WYDBR Team
 * @version 1.0.0
 * @date 2025-05-06
 */

#ifndef WYDBR_NETWORK_PROTOCOL_H
#define WYDBR_NETWORK_PROTOCOL_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace wydbr {
namespace network {

/**
 * @enum PacketType
 * @brief Tipos de pacotes de rede
 */
enum class PacketType : uint8_t {
    HANDSHAKE = 0x01,          ///< Inicialização de conexão
    HANDSHAKE_ACK = 0x02,      ///< Confirmação de handshake
    PING = 0x03,               ///< Verificação de latência
    PONG = 0x04,               ///< Resposta de ping
    STATE_UPDATE = 0x05,       ///< Atualização de estado
    INPUT = 0x06,              ///< Entrada do usuário
    SNAPSHOT = 0x07,           ///< Snapshot completo de estado
    DISCONNECT = 0x08,         ///< Desconexão
    RELIABLE_DATA = 0x09,      ///< Dados confiáveis
    UNRELIABLE_DATA = 0x0A,    ///< Dados não confiáveis
    ACKNOWLEDGMENT = 0x0B,     ///< Confirmação de recebimento
    FRAGMENT = 0x0C,           ///< Fragmento de pacote grande
    HEARTBEAT = 0x0D,          ///< Heartbeat de conexão
    SYNC_REQUEST = 0x0E,       ///< Solicitação de sincronização
    SYNC_RESPONSE = 0x0F,      ///< Resposta de sincronização
    PERMISSION_UPDATE = 0x10,  ///< Atualização de permissões
    TELEMETRY = 0x11,          ///< Dados de telemetria
    COMPRESSED_DATA = 0x12     ///< Dados comprimidos
};

/**
 * @enum PacketFlag
 * @brief Flags para pacotes
 */
enum class PacketFlag : uint8_t {
    NONE = 0x00,               ///< Sem flags
    RELIABLE = 0x01,           ///< Pacote confiável (requer ACK)
    COMPRESSED = 0x02,         ///< Dados comprimidos
    ENCRYPTED = 0x04,          ///< Dados criptografados
    FRAGMENTED = 0x08,         ///< Pacote fragmentado
    HIGH_PRIORITY = 0x10,      ///< Prioridade alta
    NEEDS_SEQUENCING = 0x20,   ///< Requer sequenciamento
    CONTAINS_TIMESTAMP = 0x40, ///< Contém timestamp
    DELTA_ENCODED = 0x80       ///< Codificação delta
};

/**
 * @enum CompressionAlgorithm
 * @brief Algoritmos de compressão suportados
 */
enum class CompressionAlgorithm : uint8_t {
    NONE = 0,        ///< Sem compressão
    ZSTD_FAST = 1,   ///< zstd otimizado para velocidade
    ZSTD_DEFAULT = 2,///< zstd com configuração padrão
    ZSTD_MAX = 3,    ///< zstd com compressão máxima
    DELTA = 4,       ///< Codificação delta (para estados similares)
    RLE = 5,         ///< Run-length encoding
    HUFFMAN = 6      ///< Compressão Huffman customizada
};

/**
 * @struct PacketHeader
 * @brief Cabeçalho de pacote
 */
#pragma pack(push, 1)
struct PacketHeader {
    uint8_t magic[2] = {'W', 'Y'};  ///< Bytes mágicos
    uint8_t version = 1;            ///< Versão do protocolo
    PacketType type;                ///< Tipo de pacote
    uint8_t flags;                  ///< Flags (bitmask de PacketFlag)
    uint16_t size;                  ///< Tamanho total do pacote
    uint32_t sequence;              ///< Número de sequência
    uint32_t ack;                   ///< Último pacote reconhecido
    uint32_t ackBitfield;           ///< Bitfield de reconhecimento
    uint64_t timestamp;             ///< Timestamp (opcional)
    uint8_t reserved[4];            ///< Reservado para uso futuro
};
#pragma pack(pop)

/**
 * @class Packet
 * @brief Representação de um pacote de rede
 */
/**
 * Classe WYDPacket
 * 
 * Esta classe implementa a funcionalidade WYDPacket conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Packet {
public:
    /**
     * @brief Construtor
     * @param type Tipo de pacote
     * @param flags Flags iniciais
     */
    explicit Packet(PacketType type, uint8_t flags = 0);
    
    /**
     * @brief Destrutor
     */
    ~Packet() = default;
    
    /**
     * @brief Adiciona uma flag ao pacote
     * @param flag Flag a ser adicionada
     */
    void addFlag(PacketFlag flag);
    
    /**
     * @brief Remove uma flag do pacote
     * @param flag Flag a ser removida
     */
    void removeFlag(PacketFlag flag);
    
    /**
     * @brief Verifica se o pacote tem uma flag
     * @param flag Flag a ser verificada
     * @return true se a flag está presente, false caso contrário
     */
    bool hasFlag(PacketFlag flag) const;
    
    /**
     * @brief Adiciona dados ao pacote
     * @param data Ponteiro para os dados
     * @param size Tamanho dos dados em bytes
     */
    void addData(const void* data, size_t size);
    
    /**
     * @brief Adiciona uma string ao pacote
     * @param str String a ser adicionada
     */
    void addString(const std::string& str);
    
    /**
     * @brief Adiciona um número ao pacote
     * @param value Valor a ser adicionado
     */
    template <typename T>
    /**
 * addNumber
 * 
 * Implementa a funcionalidade addNumber conforme especificação original.
 * @param value Parâmetro value
 */

    void addNumber(T value);
    
    /**
     * @brief Serializa o pacote para envio
     * @return Buffer serializado
     */
    std::vector<uint8_t> serialize() const;
    
    /**
     * @brief Deserializa um buffer para um pacote
     * @param data Ponteiro para os dados
     * @param size Tamanho dos dados em bytes
     * @return Pacote deserializado ou nullptr em caso de erro
     */
    static std::unique_ptr<Packet> deserialize(const void* data, size_t size);
    
    /**
     * @brief Comprime os dados do pacote
     * @param algorithm Algoritmo de compressão
     * @param level Nível de compressão
     * @return true se comprimido com sucesso, false caso contrário
     */
    bool compress(CompressionAlgorithm algorithm = CompressionAlgorithm::ZSTD_DEFAULT, int level = 3);
    
    /**
     * @brief Descomprime os dados do pacote
     * @return true se descomprimido com sucesso, false caso contrário
     */
    bool decompress();
    
    /**
     * @brief Criptografa os dados do pacote
     * @param key Chave de criptografia
     * @return true se criptografado com sucesso, false caso contrário
     */
    bool encrypt(const std::string& key);
    
    /**
     * @brief Descriptografa os dados do pacote
     * @param key Chave de criptografia
     * @return true se descriptografado com sucesso, false caso contrário
     */
    bool decrypt(const std::string& key);
    
    /**
     * @brief Obtém o tipo do pacote
     * @return Tipo do pacote
     */
    PacketType getType() const { return header_.type; }
    
    /**
     * @brief Obtém o tamanho dos dados
     * @return Tamanho dos dados em bytes
     */
    size_t getDataSize() const { return data_.size(); }
    
    /**
     * @brief Obtém os dados
     * @return Referência para os dados
     */
    const std::vector<uint8_t>& getData() const { return data_; }
    
private:
    PacketHeader header_;            ///< Cabeçalho do pacote
    std::vector<uint8_t> data_;      ///< Dados do pacote
    mutable std::vector<uint8_t> serializedCache_;  ///< Cache da serialização
};

/**
 * @class NetworkProtocol
 * @brief Implementação do protocolo de rede
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
     * @brief Construtor
     */
    NetworkProtocol();
    
    /**
     * @brief Destrutor
     */
    ~NetworkProtocol();
    
    /**
     * @brief Inicializa o protocolo
     * @param isServer Se é servidor ou cliente
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool initialize(bool isServer);
    
    /**
     * @brief Finaliza o protocolo
     */
    void shutdown();
    
    /**
     * @brief Define o callback para recebimento de pacotes
     * @param callback Função de callback
     */
    void setPacketReceivedCallback(std::function<void(const Packet&)> callback);
    
    /**
     * @brief Processa dados recebidos
     * @param data Dados recebidos
     * @param size Tamanho dos dados
     * @return Número de pacotes processados
     */
    int processIncomingData(const void* data, size_t size);
    
    /**
     * @brief Envia um pacote
     * @param packet Pacote a ser enviado
     * @return true se enfileirado para envio, false caso contrário
     */
    bool sendPacket(const Packet& packet);
    
    /**
     * @brief Atualiza o protocolo
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void update(float deltaTime);
    
    /**
     * @brief Obtém a latência atual
     * @return Latência em milissegundos
     */
    uint32_t getLatency() const;
    
    /**
     * @brief Obtém a taxa de perda de pacotes
     * @return Taxa de perda de pacotes (0.0-1.0)
     */
    float getPacketLoss() const;
    
    /**
     * @brief Obtém o jitter atual
     * @return Jitter em milissegundos
     */
    uint32_t getJitter() const;
    
private:
    bool isServer_ = false;             ///< Se é servidor
    bool initialized_ = false;          ///< Se foi inicializado
    uint32_t nextSequenceNumber_ = 0;   ///< Próximo número de sequência
    uint32_t remoteSequence_ = 0;       ///< Último número de sequência remoto
    uint64_t latestTimestamp_ = 0;      ///< Último timestamp
    uint32_t latency_ = 0;              ///< Latência atual
    float packetLoss_ = 0.0f;           ///< Taxa de perda de pacotes
    uint32_t jitter_ = 0;               ///< Jitter atual
    
    std::function<void(const Packet&)> packetCallback_; ///< Callback de pacote recebido
    
    std::vector<uint8_t> receiveBuffer_; ///< Buffer de recepção
    std::vector<Packet> sendQueue_;      ///< Fila de envio
    std::vector<Packet> reliableQueue_;  ///< Fila de pacotes confiáveis
    
    /**
     * @brief Processa um pacote recebido
     * @param packet Pacote recebido
     */
    void processPacket(const Packet& packet);
    
    /**
     * @brief Envia confirmação de recebimento
     * @param sequence Número de sequência a confirmar
     */
    void sendAcknowledgment(uint32_t sequence);
    
    /**
     * @brief Atualiza estatísticas de rede
     * @param packet Pacote recebido
     */
    void updateNetworkStats(const Packet& packet);
};

/**
 * @class PredictiveSyncManager
 * @brief Gerenciador de sincronização preditiva
 * 
 * Implementa sincronização de estado com previsão de movimento,
 * reconciliação de estado e correção de erros.
 */
/**
 * Classe WYDPredictiveSyncManager
 * 
 * Esta classe implementa a funcionalidade WYDPredictiveSyncManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PredictiveSyncManager {
public:
    /**
     * @brief Construtor
     * @param protocol Protocolo de rede a ser usado
     */
    explicit PredictiveSyncManager(NetworkProtocol& protocol);
    
    /**
     * @brief Destrutor
     */
    ~PredictiveSyncManager();
    
    /**
     * @brief Inicializa o gerenciador
     * @param isAuthority Se tem autoridade sobre os estados
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool initialize(bool isAuthority);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Registra uma entidade para sincronização
     * @param entityId ID da entidade
     * @param initialState Estado inicial
     * @param predictionConfig Configuração de previsão
     * @return true se registrado com sucesso, false caso contrário
     */
    bool registerEntity(uint32_t entityId, const std::vector<uint8_t>& initialState, 
                        const std::unordered_map<std::string, float>& predictionConfig);
    
    /**
     * @brief Remove uma entidade registrada
     * @param entityId ID da entidade
     * @return true se removido com sucesso, false caso contrário
     */
    bool unregisterEntity(uint32_t entityId);
    
    /**
     * @brief Atualiza o estado local de uma entidade
     * @param entityId ID da entidade
     * @param state Novo estado
     * @param timestamp Timestamp da atualização
     * @return true se atualizado com sucesso, false caso contrário
     */
    bool updateLocalState(uint32_t entityId, const std::vector<uint8_t>& state, uint64_t timestamp);
    
    /**
     * @brief Aplica entrada de usuário a uma entidade
     * @param entityId ID da entidade
     * @param input Entrada do usuário
     * @param timestamp Timestamp da entrada
     * @return true se aplicado com sucesso, false caso contrário
     */
    bool applyInput(uint32_t entityId, const std::vector<uint8_t>& input, uint64_t timestamp);
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void update(float deltaTime);
    
    /**
     * @brief Define a função de callback para estados reconciliados
     * @param callback Função de callback
     */
    void setStateReconciliationCallback(std::function<void(uint32_t, const std::vector<uint8_t>&)> callback);
    
    /**
     * @brief Define a função de simulação para previsão
     * @param callback Função de simulação
     */
    void setSimulationCallback(std::function<std::vector<uint8_t>(uint32_t, const std::vector<uint8_t>&, const std::vector<uint8_t>&)> callback);
    
private:
    NetworkProtocol& protocol_;        ///< Protocolo de rede
    bool isAuthority_ = false;         ///< Se tem autoridade
    bool initialized_ = false;         ///< Se foi inicializado
    
    struct EntityState {
        uint32_t entityId;             ///< ID da entidade
        std::vector<uint8_t> state;    ///< Estado atual
        std::vector<uint8_t> lastVerifiedState;  ///< Último estado verificado
        uint64_t timestamp;            ///< Timestamp do estado
        std::vector<std::pair<std::vector<uint8_t>, uint64_t>> inputHistory;  ///< Histórico de inputs
        std::unordered_map<std::string, float> predictionConfig;  ///< Configuração de previsão
        float errorAccumulator;        ///< Acumulador de erro
        bool needsReset;               ///< Se precisa de reset
    };
    
    std::unordered_map<uint32_t, EntityState> entities_;  ///< Entidades registradas
    
    std::function<void(uint32_t, const std::vector<uint8_t>&)> reconciliationCallback_;  ///< Callback de reconciliação
    std::function<std::vector<uint8_t>(uint32_t, const std::vector<uint8_t>&, const std::vector<uint8_t>&)> simulationCallback_;  ///< Callback de simulação
    
    /**
     * @brief Processa uma atualização de estado recebida
     * @param entityId ID da entidade
     * @param state Estado recebido
     * @param timestamp Timestamp do estado
     */
    void processStateUpdate(uint32_t entityId, const std::vector<uint8_t>& state, uint64_t timestamp);
    
    /**
     * @brief Realiza reconciliação de estado
     * @param entityId ID da entidade
     * @param authoritativeState Estado autoritativo
     * @param timestamp Timestamp do estado
     * @return true se reconciliado com sucesso, false caso contrário
     */
    bool reconcileState(uint32_t entityId, const std::vector<uint8_t>& authoritativeState, uint64_t timestamp);
    
    /**
     * @brief Envia atualização de estado para clientes
     * @param entityId ID da entidade
     */
    void sendStateUpdate(uint32_t entityId);
    
    /**
     * @brief Calcula erro entre estados
     * @param state1 Primeiro estado
     * @param state2 Segundo estado
     * @return Valor do erro
     */
    float calculateStateError(const std::vector<uint8_t>& state1, const std::vector<uint8_t>& state2);
};

} // namespace network
} // namespace wydbr

#endif // WYDBR_NETWORK_PROTOCOL_H