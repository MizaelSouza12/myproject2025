/**
 * ProtocolManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/network/protocol/ProtocolManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PROTOCOLMANAGER_H
#define PROTOCOLMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <bitset>

#include "../../../../core/WYDTypes.h"
#include "../NetworkTypes.h"
#include "../../security/SecurityTypes.h"
#include "../../kernel/KernelTypes.h"

namespace wydbr {


/**
 * @file ProtocolManager.h
 * @brief Gerenciador de protocolo avançado para comunicação de rede
 * 
 * Este arquivo contém a definição do gerenciador de protocolo avançado,
 * que implementa o protocolo de comunicação do WYD com suporte para compressão,
 * fragmentação, confiabilidade, ordenação, e priorização de pacotes.
 */

namespace wyd {
namespace server {
namespace network {
namespace protocol {

/**
 * @brief Configuração do gerenciador de protocolo
 */
struct ProtocolManagerConfig {
    bool usePacketFragmentation;              // Usar fragmentação de pacotes
    bool usePacketCompression;                // Usar compressão de pacotes
    bool usePacketEncryption;                 // Usar criptografia de pacotes
    bool usePacketCaching;                    // Usar cache de pacotes
    bool usePacketBatching;                   // Usar agrupamento de pacotes
    bool usePacketDeltaEncoding;              // Usar codificação delta para pacotes
    bool usePacketPriority;                   // Usar prioridade de pacotes
    bool usePacketAcknowledgment;             // Usar reconhecimento de pacotes
    bool usePacketOrdering;                   // Usar ordenação de pacotes
    size_t maxPacketSize;                     // Tamanho máximo de pacote
    size_t maxBatchSize;                      // Tamanho máximo de lote
    size_t fragmentSize;                      // Tamanho de fragmento
    DWORD acknowledgeTimeoutMs;               // Timeout de reconhecimento (ms)
    DWORD resendIntervalMs;                   // Intervalo de reenvio (ms)
    DWORD maxRetries;                         // Máximo de tentativas
    int compressionLevel;                     // Nível de compressão
    NetworkCompressionType compressionType;   // Tipo de compressão
    NetworkEncryptionType encryptionType;     // Tipo de criptografia
    
    ProtocolManagerConfig()
        : usePacketFragmentation(true)
        , usePacketCompression(true)
        , usePacketEncryption(true)
        , usePacketCaching(true)
        , usePacketBatching(true)
        , usePacketDeltaEncoding(true)
        , usePacketPriority(true)
        , usePacketAcknowledgment(true)
        , usePacketOrdering(true)
        , maxPacketSize(MAX_PACKET_SIZE)
        , maxBatchSize(10)
        , fragmentSize(1400) // Tamanho típico para evitar fragmentação IP
        , acknowledgeTimeoutMs(1000)
        , resendIntervalMs(500)
        , maxRetries(5)
        , compressionLevel(6)
        , compressionType(NetworkCompressionType::ZSTD)
        , encryptionType(NetworkEncryptionType::XOR)
    {
    }
};

/**
 * @brief Tipo de operação de protocolo
 */
enum class ProtocolOperation : BYTE {
    DATA = 0,                // Dados
    ACK = 1,                 // Reconhecimento
    NACK = 2,                // Reconhecimento negativo
    FRAGMENT = 3,            // Fragmento
    BATCH = 4,               // Lote
    HEARTBEAT = 5,           // Heartbeat
    CONNECT = 6,             // Conectar
    DISCONNECT = 7,          // Desconectar
    RELIABLE = 8,            // Confiável
    UNRELIABLE = 9,          // Não confiável
    COMPRESSED = 10,         // Comprimido
    ENCRYPTED = 11,          // Criptografado
    PING = 12,               // Ping
    PONG = 13,               // Pong
    CUSTOM = 14,             // Personalizado
};

/**
 * @brief Cabeçalho de protocolo
 */
struct ProtocolHeader {
    BYTE operation;          // Operação
    BYTE flags;              // Flags
    WORD size;               // Tamanho
    WORD type;               // Tipo
    DWORD sequence;          // Sequência
    DWORD ack;               // Reconhecimento
    
    // Flags de pacote
    static const BYTE FLAG_RELIABLE = 0x01;      // Confiável
    static const BYTE FLAG_FRAGMENTED = 0x02;    // Fragmentado
    static const BYTE FLAG_COMPRESSED = 0x04;    // Comprimido
    static const BYTE FLAG_ENCRYPTED = 0x08;     // Criptografado
    static const BYTE FLAG_BATCH = 0x10;         // Lote
    static const BYTE FLAG_ORDERED = 0x20;       // Ordenado
    static const BYTE FLAG_PRIORITY = 0x40;      // Prioridade
    static const BYTE FLAG_CUSTOM = 0x80;        // Personalizado
    
    ProtocolHeader()
        : operation(static_cast<BYTE>(ProtocolOperation::DATA))
        , flags(0)
        , size(0)
        , type(0)
        , sequence(0)
        , ack(0)
    {
    }
};

/**
 * @brief Cabeçalho de fragmento
 */
struct FragmentHeader {
    DWORD fragmentGroupID;    // ID do grupo de fragmentos
    WORD fragmentIndex;       // Índice de fragmento
    WORD fragmentCount;       // Contagem de fragmentos
    
    FragmentHeader()
        : fragmentGroupID(0)
        , fragmentIndex(0)
        , fragmentCount(0)
    {
    }
};

/**
 * @brief Cabeçalho de lote
 */
struct BatchHeader {
    BYTE count;               // Contagem
    
    BatchHeader()
        /**
 * count
 * 
 * Implementa a funcionalidade count conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */

        : count(0){
    }
};

/**
 * @brief Cabeçalho de pacote processado
 */
struct ProcessedPacketHeader {
    ProtocolHeader header;            // Cabeçalho de protocolo
    FragmentHeader fragmentHeader;    // Cabeçalho de fragmento
    BatchHeader batchHeader;          // Cabeçalho de lote
    BYTE channel;                     // Canal
    BYTE priority;                    // Prioridade
    
    ProcessedPacketHeader()
        : channel(0)
        , priority(0)
    {
    }
};

/**
 * @brief Estatísticas de protocolo
 */
struct ProtocolStats {
    DWORD packetsSent;                // Pacotes enviados
    DWORD packetsReceived;            // Pacotes recebidos
    DWORD packetsResent;              // Pacotes reenviados
    DWORD packetsAcknowledged;        // Pacotes reconhecidos
    DWORD packetsDropped;             // Pacotes descartados
    DWORD fragmentsSent;              // Fragmentos enviados
    DWORD fragmentsReceived;          // Fragmentos recebidos
    DWORD fragmentsReassembled;       // Fragmentos remontados
    DWORD batchesSent;                // Lotes enviados
    DWORD batchesReceived;            // Lotes recebidos
    DWORD compressedPackets;          // Pacotes comprimidos
    DWORD encryptedPackets;           // Pacotes criptografados
    float compressionRatio;           // Relação de compressão
    float packetLossRate;             // Taxa de perda de pacotes
    DWORD duplicatePackets;           // Pacotes duplicados
    DWORD outOfOrderPackets;          // Pacotes fora de ordem
    
    ProtocolStats()
        : packetsSent(0)
        , packetsReceived(0)
        , packetsResent(0)
        , packetsAcknowledged(0)
        , packetsDropped(0)
        , fragmentsSent(0)
        , fragmentsReceived(0)
        , fragmentsReassembled(0)
        , batchesSent(0)
        , batchesReceived(0)
        , compressedPackets(0)
        , encryptedPackets(0)
        , compressionRatio(0.0f)
        , packetLossRate(0.0f)
        , duplicatePackets(0)
        , outOfOrderPackets(0)
    {
    }
};

/**
 * @brief Pacote confiável
 */
struct ReliablePacket {
    NetworkPacket packet;             // Pacote
    DWORD sequence;                   // Sequência
    time_t sendTime;                  // Hora de envio
    DWORD retries;                    // Tentativas
    bool acknowledged;                // Reconhecido
    
    ReliablePacket()
        : sequence(0)
        , sendTime(0)
        , retries(0)
        , acknowledged(false)
    {
    }
};

/**
 * @brief Callback de pacote processado
 */
using ProcessedPacketCallback = std::function<void(DWORD, const NetworkPacket&, bool)>;

/**
 * @brief Gerenciador de protocolo
 */
/**
 * Classe WYDProtocolManager
 * 
 * Esta classe implementa a funcionalidade WYDProtocolManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ProtocolManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static ProtocolManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const ProtocolManagerConfig& config);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Processa um pacote para envio
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @param packetType Tipo de pacote
     * @param channel Canal
     * @param reliable Confiável
     * @param compressed Comprimido
     * @param encrypted Criptografado
     * @param priority Prioridade
     * @param ordered Ordenado
     * @return Pacote processado, ou nullptr se falhar
     */
    NetworkPacket* ProcessOutgoingPacket(DWORD connectionID, const void* data, size_t size,
                                      WORD packetType, BYTE channel = 0, bool reliable = true,
                                      bool compressed = true, bool encrypted = true,
                                      BYTE priority = 0, bool ordered = true);
    
    /**
     * @brief Processa um pacote recebido
     * @param connectionID ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @param processed Se foi processado (saída)
     * @param packetsToSend Pacotes a enviar (saída)
     * @return Pacote processado, ou nullptr se não for completamente processado
     */
    NetworkPacket* ProcessIncomingPacket(DWORD connectionID, const void* data, size_t size,
                                      bool& processed, std::vector<NetworkPacket>& packetsToSend);
    
    /**
     * @brief Registra um callback para pacotes processados
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterProcessedPacketCallback(ProcessedPacketCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterProcessedPacketCallback(int callbackID);
    
    /**
     * @brief Reconhece um pacote
     * @param connectionID ID da conexão
     * @param sequence Sequência
     * @return true se reconhecido com sucesso
     */
    bool AcknowledgePacket(DWORD connectionID, DWORD sequence);
    
    /**
     * @brief Obtém estatísticas de protocolo para uma conexão
     * @param connectionID ID da conexão
     * @return Estatísticas
     */
    ProtocolStats GetProtocolStats(DWORD connectionID) const;
    
    /**
     * @brief Cria um pacote de ping
     * @param connectionID ID da conexão
     * @return Pacote
     */
    NetworkPacket CreatePingPacket(DWORD connectionID);
    
    /**
     * @brief Cria um pacote de pong
     * @param connectionID ID da conexão
     * @param pingPacket Pacote de ping
     * @return Pacote
     */
    NetworkPacket CreatePongPacket(DWORD connectionID, const NetworkPacket& pingPacket);
    
    /**
     * @brief Calcula a latência com base em pacotes de ping/pong
     * @param connectionID ID da conexão
     * @param pingTime Hora do ping
     * @param pongTime Hora do pong
     * @return Latência (ms)
     */
    float CalculateLatency(DWORD connectionID, time_t pingTime, time_t pongTime);
    
    /**
     * @brief Limpa o estado de uma conexão
     * @param connectionID ID da conexão
     * @return true se limpo com sucesso
     */
    bool ClearConnectionState(DWORD connectionID);
    
    /**
     * @brief Define o tamanho máximo de pacote
     * @param maxSize Tamanho máximo
     */
    void SetMaxPacketSize(size_t maxSize);
    
    /**
     * @brief Obtém o tamanho máximo de pacote
     * @return Tamanho máximo
     */
    size_t GetMaxPacketSize() const;
    
    /**
     * @brief Define o tamanho de fragmento
     * @param size Tamanho
     */
    void SetFragmentSize(size_t size);
    
    /**
     * @brief Obtém o tamanho de fragmento
     * @return Tamanho
     */
    size_t GetFragmentSize() const;
    
    /**
     * @brief Define o nível de compressão
     * @param level Nível
     */
    void SetCompressionLevel(int level);
    
    /**
     * @brief Obtém o nível de compressão
     * @return Nível
     */
    int GetCompressionLevel() const;
    
    /**
     * @brief Define o tipo de compressão
     * @param type Tipo
     */
    void SetCompressionType(NetworkCompressionType type);
    
    /**
     * @brief Obtém o tipo de compressão
     * @return Tipo
     */
    NetworkCompressionType GetCompressionType() const;
    
    /**
     * @brief Define o tipo de criptografia
     * @param type Tipo
     */
    void SetEncryptionType(NetworkEncryptionType type);
    
    /**
     * @brief Obtém o tipo de criptografia
     * @return Tipo
     */
    NetworkEncryptionType GetEncryptionType() const;
    
    /**
     * @brief Cria um lote de pacotes
     * @param packets Pacotes
     * @return Pacote de lote
     */
    NetworkPacket CreateBatchPacket(const std::vector<NetworkPacket>& packets);
    
    /**
     * @brief Extrai pacotes de um lote
     * @param batchPacket Pacote de lote
     * @return Lista de pacotes
     */
    std::vector<NetworkPacket> ExtractBatchPacket(const NetworkPacket& batchPacket);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    ProtocolManager();
    
    /**
     * @brief Destrutor
     */
    ~ProtocolManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    ProtocolManager(const ProtocolManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    ProtocolManager& operator=(const ProtocolManager&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Processa retransmissões
     */
    void ProcessRetransmissions();
    
    /**
     * @brief Processa reconhecimentos
     */
    void ProcessAcknowledgments();
    
    /**
     * @brief Notifica callbacks de pacote processado
     * @param connectionID ID da conexão
     * @param packet Pacote
     * @param reliable Se é confiável
     */
    void NotifyProcessedPacketCallbacks(DWORD connectionID, const NetworkPacket& packet, bool reliable);
    
    /**
     * @brief Fragmenta um pacote
     * @param packet Pacote
     * @param fragmentSize Tamanho de fragmento
     * @return Lista de pacotes fragmentados
     */
    std::vector<NetworkPacket> FragmentPacket(const NetworkPacket& packet, size_t fragmentSize);
    
    /**
     * @brief Monta um pacote a partir de fragmentos
     * @param connectionID ID da conexão
     * @param fragments Fragmentos
     * @return Pacote montado, ou nullptr se não estiver completo
     */
    NetworkPacket* ReassemblePacket(DWORD connectionID, const std::vector<PacketFragment>& fragments);
    
    /**
     * @brief Comprime um pacote
     * @param packet Pacote
     * @return Pacote comprimido, ou nullptr se falhar
     */
    NetworkPacket* CompressPacket(const NetworkPacket& packet);
    
    /**
     * @brief Descomprime um pacote
     * @param packet Pacote
     * @return Pacote descomprimido, ou nullptr se falhar
     */
    NetworkPacket* DecompressPacket(const NetworkPacket& packet);
    
    /**
     * @brief Criptografa um pacote
     * @param packet Pacote
     * @param encryptionKey Chave de criptografia
     * @return Pacote criptografado, ou nullptr se falhar
     */
    NetworkPacket* EncryptPacket(const NetworkPacket& packet, const BYTE* encryptionKey);
    
    /**
     * @brief Descriptografa um pacote
     * @param packet Pacote
     * @param encryptionKey Chave de criptografia
     * @return Pacote descriptografado, ou nullptr se falhar
     */
    NetworkPacket* DecryptPacket(const NetworkPacket& packet, const BYTE* encryptionKey);
    
    /**
     * @brief Processa o cabeçalho de um pacote
     * @param data Dados
     * @param size Tamanho
     * @param header Cabeçalho (saída)
     * @return Tamanho do cabeçalho, ou 0 se falhar
     */
    size_t ProcessPacketHeader(const void* data, size_t size, ProcessedPacketHeader& header);
    
    /**
     * @brief Serializa o cabeçalho de um pacote
     * @param header Cabeçalho
     * @param data Dados (saída)
     * @param size Tamanho (entrada/saída)
     * @return Tamanho do cabeçalho, ou 0 se falhar
     */
    size_t SerializePacketHeader(const ProcessedPacketHeader& header, void* data, size_t& size);
    
    /**
     * @brief Calcula o checksum de um pacote
     * @param data Dados
     * @param size Tamanho
     * @return Checksum
     */
    DWORD CalculatePacketChecksum(const void* data, size_t size);
    
    /**
     * @brief Verifica o checksum de um pacote
     * @param data Dados
     * @param size Tamanho
     * @param expectedChecksum Checksum esperado
     * @return true se for válido
     */
    bool VerifyPacketChecksum(const void* data, size_t size, DWORD expectedChecksum);
    
    /**
     * @brief Gera um ID de grupo de fragmentos
     * @return ID
     */
    DWORD GenerateFragmentGroupID();
    
    /**
     * @brief Obtém a próxima sequência para uma conexão
     * @param connectionID ID da conexão
     * @return Sequência
     */
    DWORD GetNextSequence(DWORD connectionID);
    
    /**
     * @brief Codifica a compressão ZLIB
     * @param data Dados
     * @param size Tamanho
     * @param compressedData Dados comprimidos (saída)
     * @param compressedSize Tamanho comprimido (saída)
     * @param level Nível
     * @return true se comprimido com sucesso
     */
    bool ZlibCompress(const void* data, size_t size, void** compressedData, size_t* compressedSize, int level);
    
    /**
     * @brief Decodifica a compressão ZLIB
     * @param data Dados
     * @param size Tamanho
     * @param decompressedData Dados descomprimidos (saída)
     * @param decompressedSize Tamanho descomprimido (saída)
     * @return true se descomprimido com sucesso
     */
    bool ZlibDecompress(const void* data, size_t size, void** decompressedData, size_t* decompressedSize);
    
    /**
     * @brief Codifica a compressão ZSTD
     * @param data Dados
     * @param size Tamanho
     * @param compressedData Dados comprimidos (saída)
     * @param compressedSize Tamanho comprimido (saída)
     * @param level Nível
     * @return true se comprimido com sucesso
     */
    bool ZstdCompress(const void* data, size_t size, void** compressedData, size_t* compressedSize, int level);
    
    /**
     * @brief Decodifica a compressão ZSTD
     * @param data Dados
     * @param size Tamanho
     * @param decompressedData Dados descomprimidos (saída)
     * @param decompressedSize Tamanho descomprimido (saída)
     * @return true se descomprimido com sucesso
     */
    bool ZstdDecompress(const void* data, size_t size, void** decompressedData, size_t* decompressedSize);
    
    /**
     * @brief Codifica a criptografia XOR
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     * @return true se criptografado com sucesso
     */
    bool XorEncrypt(void* data, size_t size, const BYTE* key);
    
    /**
     * @brief Decodifica a criptografia XOR
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     * @return true se descriptografado com sucesso
     */
    bool XorDecrypt(void* data, size_t size, const BYTE* key);
    
    // Configuração
    ProtocolManagerConfig config_;
    
    // Pacotes confiáveis por conexão
    std::unordered_map<DWORD, std::map<DWORD, ReliablePacket>> reliablePackets_;
    mutable std::mutex reliablePacketsMutex_;
    
    // Fragmentos por conexão
    std::unordered_map<DWORD, std::unordered_map<DWORD, std::vector<PacketFragment>>> fragmentsByConnection_;
    mutable std::mutex fragmentsMutex_;
    
    // Estatísticas por conexão
    std::unordered_map<DWORD, ProtocolStats> statsByConnection_;
    mutable std::mutex statsMutex_;
    
    // Sequências por conexão
    std::unordered_map<DWORD, DWORD> sequencesByConnection_;
    mutable std::mutex sequencesMutex_;
    
    // Sequências recebidas por conexão
    std::unordered_map<DWORD, std::bitset<65536>> receivedSequences_;
    mutable std::mutex receivedSequencesMutex_;
    
    // Buffers de lote por conexão
    std::unordered_map<DWORD, std::vector<NetworkPacket>> batchBuffersByConnection_;
    mutable std::mutex batchBuffersMutex_;
    
    // Chaves de criptografia por conexão
    std::unordered_map<DWORD, std::array<BYTE, 16>> encryptionKeysByConnection_;
    mutable std::mutex encryptionKeysMutex_;
    
    // Callbacks para pacotes processados
    std::map<int, ProcessedPacketCallback> processedPacketCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contador para novos IDs
    std::atomic<DWORD> nextFragmentGroupID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Tempos
    time_t lastRetransmissionTime_;
    time_t lastAcknowledgmentTime_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_ProtocolManager ProtocolManager::GetInstance()

} // namespace protocol
} // namespace network
} // namespace server
} // namespace wyd

#endif // PROTOCOLMANAGER_H

} // namespace wydbr
