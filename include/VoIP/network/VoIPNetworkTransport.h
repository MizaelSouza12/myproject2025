/**
 * @file VoIPNetworkTransport.h
 * @brief Sistema de transporte de rede para VoIP
 */

#ifndef WYDBR_VOIP_NETWORK_TRANSPORT_H
#define WYDBR_VOIP_NETWORK_TRANSPORT_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace WYDBR {
namespace VoIP {

/**
 * Tipo de transporte de rede
 */
enum class TransportType {
    UDP,        // UDP - baixa latência, sem garantia
    TCP,        // TCP - confiável, maior latência
    WebRTC,     // WebRTC - otimizado para mídia
    QUIC,       // QUIC - novo protocolo da Google
    Custom      // Protocolo personalizado
};

/**
 * Qualidade de serviço (QoS)
 */
enum class QosLevel {
    BestEffort,  // Melhor esforço
    Voice,       // Otimizado para voz
    Video,       // Otimizado para vídeo
    Control      // Pacotes de controle (alta prioridade)
};

/**
 * Estatísticas de rede
 */
struct NetworkStats {
    double packetLossRate;       // Taxa de perda de pacotes (0.0-1.0)
    double roundTripTimeMs;      // Tempo de ida e volta em ms
    double jitterMs;             // Jitter em ms
    int64_t bytesSent;           // Total de bytes enviados
    int64_t bytesReceived;       // Total de bytes recebidos
    double bandwidthBps;         // Largura de banda estimada em bits/s
};

/**
 * Configuração de transporte
 */
struct TransportConfig {
    TransportType type;          // Tipo de transporte
    std::string localAddress;    // Endereço local para bind
    uint16_t localPort;          // Porta local para bind
    bool enableQos;              // Habilitar QoS
    bool enableEncryption;       // Habilitar criptografia
    int maxPacketSize;           // Tamanho máximo de pacote em bytes
    int jitterBufferMs;          // Tamanho do buffer de jitter em ms
};

/**
 * Callback para receber pacotes
 */
using PacketReceivedCallback = std::function<void(const uint8_t* data, size_t size, const std::string& sender)>;

/**
 * Interface de transporte de rede
 */
class INetworkTransport {
public:
    virtual ~INetworkTransport() = default;
    
    /**
     * Inicializa o transporte
     * @param config Configuração de transporte
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const TransportConfig& config) = 0;
    
    /**
     * Inicia o transporte
     * @param callback Função a ser chamada quando pacotes forem recebidos
     * @return true se iniciado com sucesso
     */
    virtual bool Start(PacketReceivedCallback callback) = 0;
    
    /**
     * Para o transporte
     */
    virtual void Stop() = 0;
    
    /**
     * Envia pacote para um destino
     * @param data Dados a serem enviados
     * @param size Tamanho dos dados
     * @param destinationAddress Endereço de destino
     * @param destinationPort Porta de destino
     * @param qos Nível de QoS
     * @return true se enviado com sucesso
     */
    virtual bool SendPacket(const uint8_t* data, size_t size, 
                          const std::string& destinationAddress, 
                          uint16_t destinationPort,
                          QosLevel qos = QosLevel::Voice) = 0;
    
    /**
     * Obtém estatísticas de rede
     * @return Estatísticas atuais
     */
    virtual NetworkStats GetStatistics() const = 0;
    
    /**
     * Obtém o tipo de transporte
     * @return Tipo de transporte
     */
    virtual TransportType GetType() const = 0;
    
    /**
     * Verifica se o transporte está ativo
     * @return true se estiver ativo
     */
    virtual bool IsActive() const = 0;
    
    /**
     * Define o nível máximo de jitter buffer
     * @param milliseconds Tamanho em ms
     * @return true se alterado com sucesso
     */
    virtual bool SetJitterBuffer(int milliseconds) = 0;
};

/**
 * Implementação de transporte UDP
 */
class UDPTransport : public INetworkTransport {
public:
    UDPTransport();
    ~UDPTransport() override;
    
    bool Initialize(const TransportConfig& config) override;
    bool Start(PacketReceivedCallback callback) override;
    void Stop() override;
    bool SendPacket(const uint8_t* data, size_t size, 
                  const std::string& destinationAddress, 
                  uint16_t destinationPort,
                  QosLevel qos = QosLevel::Voice) override;
    NetworkStats GetStatistics() const override;
    TransportType GetType() const override;
    bool IsActive() const override;
    bool SetJitterBuffer(int milliseconds) override;
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * Factory para criar transporte de rede
 */
class NetworkTransportFactory {
public:
    /**
     * Cria uma instância de transporte baseado no tipo
     * @param type Tipo de transporte
     * @return Ponteiro para o transporte
     */
    static std::unique_ptr<INetworkTransport> Create(TransportType type);
    
    /**
     * Obtém lista de transportes disponíveis
     * @return Lista de tipos de transporte
     */
    static std::vector<TransportType> GetAvailableTransports();
    
    /**
     * Determina o melhor transporte para as condições atuais
     * @return Tipo de transporte recomendado
     */
    static TransportType GetRecommendedTransport();
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_NETWORK_TRANSPORT_H
