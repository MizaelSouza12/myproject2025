/**
 * @file NetworkIntegration.h
 * @brief Camada de integração do sistema de rede
 * 
 * Integra os componentes de rede com o resto do sistema.
 */

#ifndef WYDBR_NETWORK_INTEGRATION_H
#define WYDBR_NETWORK_INTEGRATION_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <unordered_map>

#include "Network/protocol/NetworkProtocol.h"
#include "Network/protocol/UDPProtocolHandler.h"
#include "Network/ratelimit/RateLimiter.h"
#include "Network/packet/PacketFragmentation.h"
#include "Security/crypto/CryptoManager.h"

namespace WYDBR {
namespace Integration {

/**
 * Configuração da integração de rede
 */
struct NetworkIntegrationConfig {
    bool enableCompression;           // Habilitar compressão
    bool enableRateLimiting;          // Habilitar limitação de taxa
    bool enablePacketFragmentation;   // Habilitar fragmentação de pacotes
    bool enableReliableTransport;     // Habilitar transporte confiável
    int bufferSizeKB;                 // Tamanho do buffer em KB
    std::string networkConfigFile;    // Arquivo de configuração de rede
};

/**
 * Callback para pacotes recebidos
 */
using PacketReceivedCallback = std::function<void(Network::PacketType, const void*, size_t)>;

/**
 * Integração do sistema de rede
 */
class NetworkIntegration {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o integrador
     */
    static NetworkIntegration& GetInstance();
    
    /**
     * Inicializa a integração
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const NetworkIntegrationConfig& config);
    
    /**
     * Integra sistema de rede com segurança
     * @return true se integrado com sucesso
     */
    bool IntegrateWithSecurity();
    
    /**
     * Integra sistema de rede com VoIP
     * @return true se integrado com sucesso
     */
    bool IntegrateWithVoIP();
    
    /**
     * Integra sistema de rede com compartilhamento de tela
     * @return true se integrado com sucesso
     */
    bool IntegrateWithScreenShare();
    
    /**
     * Registra um handler para tipo de pacote
     * @param packetType Tipo de pacote
     * @param callback Callback para pacotes deste tipo
     * @return true se registrado com sucesso
     */
    bool RegisterPacketHandler(Network::PacketType packetType, PacketReceivedCallback callback);
    
    /**
     * Envia um pacote
     * @param packetType Tipo de pacote
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param reliable Usar transporte confiável
     * @return true se enviado com sucesso
     */
    bool SendPacket(Network::PacketType packetType, const void* data, size_t size, bool reliable = true);
    
    /**
     * Processa um pacote recebido
     * @param data Dados recebidos
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool ProcessReceivedData(const void* data, size_t size);
    
    /**
     * Verifica integridade do sistema de rede
     * @return true se o sistema estiver íntegro
     */
    bool VerifyNetworkIntegrity();
    
    /**
     * Obtém estatísticas de rede
     * @return String formatada com estatísticas
     */
    std::string GetNetworkStatistics() const;
    
    /**
     * Configura limite de taxa para tipo de pacote
     * @param packetType Tipo de pacote
     * @param maxPerSecond Máximo de pacotes por segundo
     * @param burstAllowance Tolerância para burst
     * @return true se configurado com sucesso
     */
    bool ConfigureRateLimit(Network::PacketType packetType, uint32_t maxPerSecond, uint32_t burstAllowance);
    
private:
    // Construtor e destrutor privados (singleton)
    NetworkIntegration();
    ~NetworkIntegration();
    
    // Não permitir cópias
    NetworkIntegration(const NetworkIntegration&) = delete;
    NetworkIntegration& operator=(const NetworkIntegration&) = delete;
    
    // Configuração
    NetworkIntegrationConfig m_config;
    
    // Componentes de rede
    Network::UDPProtocolHandler* m_protocolHandler;
    Network::RateLimiter* m_rateLimiter;
    Network::PacketFragmenter* m_packetFragmenter;
    
    // Handlers de pacotes
    std::unordered_map<uint16_t, PacketReceivedCallback> m_packetHandlers;
    
    // Estado
    bool m_initialized;
    bool m_securityIntegrated;
    bool m_voipIntegrated;
    bool m_screenShareIntegrated;
    
    // Estatísticas
    uint64_t m_packetsSent;
    uint64_t m_packetsReceived;
    uint64_t m_bytesSent;
    uint64_t m_bytesReceived;
    uint64_t m_packetsDropped;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    mutable std::mutex m_handlersMutex;
    
    // Métodos privados
    void LoadNetworkConfig();
    bool VerifyComponentIntegrity();
    bool PreprocessPacket(const void* data, size_t size, std::vector<uint8_t>& outProcessedData);
    bool PostprocessPacket(Network::PacketType packetType, const void* data, size_t size, std::vector<uint8_t>& outProcessedData);
    bool HandleFragmentedPacket(const void* data, size_t size, std::vector<uint8_t>& outCompleteData);
    void UpdateStatistics(bool sending, size_t bytes, bool dropped = false);
};

} // namespace Integration
} // namespace WYDBR

#endif // WYDBR_NETWORK_INTEGRATION_H
