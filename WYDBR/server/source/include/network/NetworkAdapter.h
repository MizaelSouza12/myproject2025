/**
 * NetworkAdapter.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/network/NetworkAdapter.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYDBR_NETWORK_ADAPTER_H
#define WYDBR_NETWORK_ADAPTER_H

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>

namespace wydbr {
namespace network {

/**
 * @struct NetworkStats
 * @brief Estatísticas da rede
 */
struct NetworkStats {
    uint64_t bytesReceived;    ///< Total de bytes recebidos
    uint64_t bytesSent;        ///< Total de bytes enviados
    uint32_t packetsReceived;  ///< Total de pacotes recebidos
    uint32_t packetsSent;      ///< Total de pacotes enviados
    uint32_t activeConnections;///< Conexões ativas
    uint32_t totalConnections; ///< Total de conexões desde o início
    uint32_t droppedPackets;   ///< Pacotes descartados
    uint32_t invalidPackets;   ///< Pacotes inválidos
    float averagePing;         ///< Ping médio em ms
    float packetLossRate;      ///< Taxa de perda de pacotes
};

/**
 * @struct ConnectionInfo
 * @brief Informações sobre uma conexão
 */
struct ConnectionInfo {
    uint32_t connectionId;     ///< ID único da conexão
    std::string ipAddress;     ///< Endereço IP do cliente
    uint16_t port;             ///< Porta do cliente
    int64_t connectTime;       ///< Timestamp da conexão
    int64_t lastActivityTime;  ///< Timestamp da última atividade
    uint64_t bytesReceived;    ///< Bytes recebidos desta conexão
    uint64_t bytesSent;        ///< Bytes enviados para esta conexão
    float ping;                ///< Ping em ms
    std::string userAgent;     ///< User agent (se disponível)
    std::string authToken;     ///< Token de autenticação (se autenticado)
    bool encrypted;            ///< Se a conexão está criptografada
    bool authenticated;        ///< Se a conexão está autenticada
};

/**
 * @struct Packet
 * @brief Representação de um pacote de rede
 */
struct Packet {
    uint32_t connectionId;     ///< ID da conexão que enviou/receberá o pacote
    uint16_t opcode;           ///< Código de operação do pacote
    std::vector<uint8_t> data; ///< Dados do pacote
    int64_t timestamp;         ///< Timestamp de recebimento/envio
    uint32_t sequence;         ///< Número de sequência (para ordenação)
    bool reliable;             ///< Se o pacote precisa de confirmação de recebimento
    bool encrypted;            ///< Se o pacote está criptografado
};

/**
 * @class NetworkAdapter
 * @brief Interface para adaptadores de rede
 * 
 * Fornece uma interface comum para diferentes implementações de rede,
 * como sockets TCP/IP, WebSockets, etc.
 */
/**
 * Classe WYDNetworkAdapter
 * 
 * Esta classe implementa a funcionalidade WYDNetworkAdapter conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class NetworkAdapter {
public:
    /**
     * @brief Tipo de callback para processamento de pacotes
     */
    using PacketHandler = std::function<void(const Packet&)>;
    
    /**
     * @brief Tipo de callback para eventos de conexão
     */
    using ConnectionHandler = std::function<void(const ConnectionInfo&, bool)>;

    /**
     * @brief Destrutor virtual
     */
    virtual ~NetworkAdapter() = default;
    
    /**
     * @brief Inicializa o adaptador de rede
     * @param bindAddress Endereço para bind
     * @param port Porta para escutar
     * @return true se a inicialização foi bem-sucedida
     */
    virtual bool initialize(const std::string& bindAddress, uint16_t port) = 0;
    
    /**
     * @brief Inicia o adaptador de rede
     * @return true se o início foi bem-sucedido
     */
    virtual bool start() = 0;
    
    /**
     * @brief Fecha o adaptador de rede
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Envia um pacote
     * @param packet Pacote a ser enviado
     * @return true se o envio foi bem-sucedido
     */
    virtual bool sendPacket(const Packet& packet) = 0;
    
    /**
     * @brief Desconecta um cliente
     * @param connectionId ID da conexão a ser desconectada
     * @param reason Razão da desconexão
     */
    virtual void disconnect(uint32_t connectionId, const std::string& reason) = 0;
    
    /**
     * @brief Define o handler para pacotes recebidos
     * @param handler Função de callback para processamento de pacotes
     */
    virtual void setPacketHandler(PacketHandler handler) = 0;
    
    /**
     * @brief Define o handler para eventos de conexão/desconexão
     * @param handler Função de callback para eventos de conexão
     */
    virtual void setConnectionHandler(ConnectionHandler handler) = 0;
    
    /**
     * @brief Obtém informações sobre uma conexão
     * @param connectionId ID da conexão
     * @return Informações da conexão
     */
    virtual ConnectionInfo getConnectionInfo(uint32_t connectionId) const = 0;
    
    /**
     * @brief Obtém estatísticas de rede
     * @return Estatísticas atuais da rede
     */
    virtual NetworkStats getStats() const = 0;
    
    /**
     * @brief Verifica se o adaptador está em execução
     * @return true se o adaptador estiver em execução
     */
    virtual bool isRunning() const = 0;
    
    /**
     * @brief Obtém o número de conexões ativas
     * @return Número de conexões ativas
     */
    virtual uint32_t getActiveConnectionCount() const = 0;
    
    /**
     * @brief Configura o nível de compressão
     * @param level Nível de compressão (0 = desativado, 9 = máximo)
     */
    virtual void setCompressionLevel(int level) = 0;
    
    /**
     * @brief Configura a criptografia
     * @param enabled Se a criptografia deve ser ativada
     * @param key Chave de criptografia (se aplicável)
     */
    virtual void setEncryption(bool enabled, const std::string& key = "") = 0;
    
    /**
     * @brief Realiza tarefas de manutenção
     */
    virtual void performMaintenance() = 0;
};

} // namespace network
} // namespace wydbr

#endif // WYDBR_NETWORK_ADAPTER_H