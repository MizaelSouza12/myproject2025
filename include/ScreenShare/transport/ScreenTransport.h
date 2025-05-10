/**
 * @file ScreenTransport.h
 * @brief Sistema de transporte para compartilhamento de tela
 */

#ifndef WYDBR_SCREEN_TRANSPORT_H
#define WYDBR_SCREEN_TRANSPORT_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <atomic>
#include <mutex>
#include "../encoding/ScreenEncoder.h"

namespace WYDBR {
namespace ScreenShare {

/**
 * Protocolo de transporte
 */
enum class StreamTransportProtocol {
    TCP,            // TCP - confiável, maior latência
    UDP,            // UDP - menor latência, menos confiável
    QUIC,           // QUIC - moderno, baixa latência
    WebRTC,         // WebRTC - otimizado para mídia
    Custom          // Protocolo personalizado
};

/**
 * Modo de distribuição
 */
enum class StreamDistributionMode {
    PeerToPeer,     // Diretamente entre peers
    ClientServer,   // Via servidor central
    Hybrid          // Combinação dos dois modos
};

/**
 * Configuração de transporte
 */
struct StreamTransportConfig {
    StreamTransportProtocol protocol;      // Protocolo de transporte
    StreamDistributionMode mode;           // Modo de distribuição
    std::string serverAddress;             // Endereço do servidor (se aplicável)
    uint16_t serverPort;                   // Porta do servidor
    int maxBandwidthKbps;                  // Largura de banda máxima em kbps
    bool enableEncryption;                 // Habilitar criptografia
    bool enableErrorCorrection;            // Habilitar correção de erros
    int jitterBufferMs;                    // Tamanho do buffer de jitter em ms
    int maxPacketSizeBytes;                // Tamanho máximo de pacote em bytes
    int maxLatencyMs;                      // Latência máxima aceitável em ms
};

/**
 * Estatísticas de transporte
 */
struct StreamTransportStats {
    double sendBitrateKbps;         // Taxa de bits de envio em kbps
    double receiveBitrateKbps;      // Taxa de bits de recebimento em kbps
    double packetLossRate;          // Taxa de perda de pacotes (0.0-1.0)
    double roundTripTimeMs;         // Tempo de ida e volta em ms
    int packetsLost;                // Número de pacotes perdidos
    int packetsSent;                // Número de pacotes enviados
    int packetsReceived;            // Número de pacotes recebidos
    double currentJitterMs;         // Jitter atual em ms
    int keyframesEncoded;           // Número de keyframes codificados
    int keyframesDecoded;           // Número de keyframes decodificados
    int64_t bytesSent;              // Total de bytes enviados
    int64_t bytesReceived;          // Total de bytes recebidos
    double networkUtilization;      // Utilização da rede (0.0-1.0)
};

/**
 * Callback para receber frames
 */
using StreamFrameReceivedCallback = std::function<void(const EncodedFrame& frame, const std::string& senderId)>;

/**
 * Interface para transporte de fluxo de vídeo
 */
class IStreamTransport {
public:
    virtual ~IStreamTransport() = default;
    
    /**
     * Inicializa o transporte
     * @param config Configuração de transporte
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const StreamTransportConfig& config) = 0;
    
    /**
     * Inicia o transporte
     * @param callback Função a ser chamada quando frames forem recebidos
     * @return true se iniciado com sucesso
     */
    virtual bool Start(StreamFrameReceivedCallback callback) = 0;
    
    /**
     * Para o transporte
     */
    virtual void Stop() = 0;
    
    /**
     * Envia frame para destino específico
     * @param frame Frame a ser enviado
     * @param destinationId ID do destinatário (vazio para todos)
     * @return true se enviado com sucesso
     */
    virtual bool SendFrame(const EncodedFrame& frame, const std::string& destinationId = "") = 0;
    
    /**
     * Solicita keyframe ao remetente
     * @param senderId ID do remetente
     * @return true se solicitado com sucesso
     */
    virtual bool RequestKeyframe(const std::string& senderId) = 0;
    
    /**
     * Adiciona destinatário permitido
     * @param userId ID do usuário
     * @param address Endereço do destinatário
     * @param port Porta do destinatário
     * @return true se adicionado com sucesso
     */
    virtual bool AddAllowedReceiver(const std::string& userId, const std::string& address, uint16_t port) = 0;
    
    /**
     * Remove destinatário permitido
     * @param userId ID do usuário
     * @return true se removido com sucesso
     */
    virtual bool RemoveAllowedReceiver(const std::string& userId) = 0;
    
    /**
     * Obtém estatísticas de transporte
     * @return Estatísticas atuais
     */
    virtual StreamTransportStats GetStatistics() const = 0;
    
    /**
     * Altera limite de largura de banda
     * @param maxBandwidthKbps Novo limite em kbps
     * @return true se alterado com sucesso
     */
    virtual bool ChangeBandwidthLimit(int maxBandwidthKbps) = 0;
    
    /**
     * Verifica se está ativo
     * @return true se estiver ativo
     */
    virtual bool IsActive() const = 0;
};

/**
 * Implementação de transporte
 */
class StreamTransport : public IStreamTransport {
public:
    StreamTransport();
    ~StreamTransport() override;
    
    bool Initialize(const StreamTransportConfig& config) override;
    bool Start(StreamFrameReceivedCallback callback) override;
    void Stop() override;
    bool SendFrame(const EncodedFrame& frame, const std::string& destinationId = "") override;
    bool RequestKeyframe(const std::string& senderId) override;
    bool AddAllowedReceiver(const std::string& userId, const std::string& address, uint16_t port) override;
    bool RemoveAllowedReceiver(const std::string& userId) override;
    StreamTransportStats GetStatistics() const override;
    bool ChangeBandwidthLimit(int maxBandwidthKbps) override;
    bool IsActive() const override;
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * Factory para criar transporte de fluxo
 */
class StreamTransportFactory {
public:
    /**
     * Cria instância de transporte baseado no protocolo
     * @param protocol Protocolo de transporte
     * @return Ponteiro para o transporte
     */
    static std::unique_ptr<IStreamTransport> Create(StreamTransportProtocol protocol);
    
    /**
     * Obtém lista de protocolos disponíveis
     * @return Lista de protocolos
     */
    static std::vector<StreamTransportProtocol> GetAvailableProtocols();
    
    /**
     * Determina o melhor protocolo para as condições atuais
     * @return Protocolo recomendado
     */
    static StreamTransportProtocol GetRecommendedProtocol();
};

/**
 * Gerenciador de transporte para compartilhamento de tela
 */
class ScreenTransportManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static ScreenTransportManager& GetInstance();
    
    /**
     * Inicializa o gerenciador
     * @param config Configuração de transporte
     * @return true se inicializado com sucesso
     */
    bool Initialize(const StreamTransportConfig& config);
    
    /**
     * Inicia o transporte
     * @param callback Função para receber frames
     * @return true se iniciado com sucesso
     */
    bool Start(StreamFrameReceivedCallback callback);
    
    /**
     * Para o transporte
     */
    void Stop();
    
    /**
     * Processa frame codificado para envio
     * @param frame Frame codificado
     */
    void ProcessEncodedFrame(const EncodedFrame& frame);
    
    /**
     * Adiciona destinatário permitido
     * @param userId ID do usuário
     * @param address Endereço do destinatário
     * @param port Porta do destinatário
     * @return true se adicionado com sucesso
     */
    bool AddAllowedReceiver(const std::string& userId, const std::string& address, uint16_t port);
    
    /**
     * Remove destinatário permitido
     * @param userId ID do usuário
     * @return true se removido com sucesso
     */
    bool RemoveAllowedReceiver(const std::string& userId);
    
    /**
     * Solicita keyframe ao remetente
     * @param senderId ID do remetente
     */
    void RequestKeyframe(const std::string& senderId);
    
    /**
     * Obtém estatísticas de transporte
     * @return Estatísticas atuais
     */
    StreamTransportStats GetStatistics() const;
    
private:
    // Implementação privada do singleton
    ScreenTransportManager();
    ~ScreenTransportManager();
    
    // Membros privados
    std::unique_ptr<IStreamTransport> m_transport;
    StreamTransportConfig m_config;
    mutable std::mutex m_mutex;
    std::atomic<bool> m_isInitialized;
    std::atomic<bool> m_isActive;
};

} // namespace ScreenShare
} // namespace WYDBR

#endif // WYDBR_SCREEN_TRANSPORT_H
