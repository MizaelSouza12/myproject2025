/**
 * @file SecurityIntegration.h
 * @brief Camada de integração do sistema de segurança
 * 
 * Integra os componentes de segurança com o resto do sistema.
 */

#ifndef WYDBR_SECURITY_INTEGRATION_H
#define WYDBR_SECURITY_INTEGRATION_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>

#include "Security/crypto/SecureRandom.h"
#include "Security/crypto/CryptoManager.h"
#include "Security/auth/PacketAuthenticator.h"
#include "Security/protection/AntiTamperManager.h"
#include "Network/protocol/NetworkProtocol.h"

namespace WYDBR {
namespace Integration {

/**
 * Configuração da integração de segurança
 */
struct SecurityIntegrationConfig {
    bool enforceStrictMode;           // Modo estrito (rejeita tudo que não for autenticado)
    bool monitorNetworkTraffic;       // Monitorar tráfego de rede
    bool encryptAllCommunication;     // Encriptar toda comunicação
    int securityLevel;                // Nível de segurança (1-5)
    std::string securityPolicyFile;   // Arquivo de política de segurança
};

/**
 * Integração do sistema de segurança
 */
class SecurityIntegration {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o integrador
     */
    static SecurityIntegration& GetInstance();
    
    /**
     * Inicializa a integração
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const SecurityIntegrationConfig& config);
    
    /**
     * Integra sistema de segurança com rede
     * @return true se integrado com sucesso
     */
    bool IntegrateWithNetwork();
    
    /**
     * Integra sistema de segurança com VoIP
     * @return true se integrado com sucesso
     */
    bool IntegrateWithVoIP();
    
    /**
     * Integra sistema de segurança com compartilhamento de tela
     * @return true se integrado com sucesso
     */
    bool IntegrateWithScreenShare();
    
    /**
     * Integra sistema de segurança com servidores TMSrv/DBSrv
     * @return true se integrado com sucesso
     */
    bool IntegrateWithServers();
    
    /**
     * Processa um pacote para segurança
     * @param packetData Dados do pacote
     * @param packetSize Tamanho do pacote
     * @param packetType Tipo do pacote
     * @param outProcessedData Buffer para dados processados
     * @param outProcessedSize Tamanho dos dados processados
     * @return true se processado com sucesso
     */
    bool ProcessPacket(const uint8_t* packetData, size_t packetSize, 
                     Network::PacketType packetType,
                     std::vector<uint8_t>& outProcessedData);
    
    /**
     * Verifica um pacote recebido
     * @param packetData Dados do pacote
     * @param packetSize Tamanho do pacote
     * @param packetType Tipo do pacote
     * @param outOriginalData Buffer para dados originais
     * @return true se verificado com sucesso
     */
    bool VerifyPacket(const uint8_t* packetData, size_t packetSize,
                    Network::PacketType packetType,
                    std::vector<uint8_t>& outOriginalData);
    
    /**
     * Criptografa dados sensíveis
     * @param data Dados a criptografar
     * @param size Tamanho dos dados
     * @param outEncrypted Buffer para dados criptografados
     * @return true se criptografado com sucesso
     */
    bool EncryptSensitiveData(const uint8_t* data, size_t size, 
                           std::vector<uint8_t>& outEncrypted);
    
    /**
     * Descriptografa dados sensíveis
     * @param data Dados criptografados
     * @param size Tamanho dos dados
     * @param outDecrypted Buffer para dados descriptografados
     * @return true se descriptografado com sucesso
     */
    bool DecryptSensitiveData(const uint8_t* data, size_t size,
                           std::vector<uint8_t>& outDecrypted);
    
    /**
     * Verifica integridade do sistema de segurança
     * @return true se o sistema estiver íntegro
     */
    bool VerifySecurityIntegrity();
    
    /**
     * Obtém o nível de segurança atual
     * @return Nível de segurança (1-5)
     */
    int GetSecurityLevel() const;
    
    /**
     * Define o nível de segurança
     * @param level Nível de segurança (1-5)
     */
    void SetSecurityLevel(int level);
    
private:
    // Construtor e destrutor privados (singleton)
    SecurityIntegration();
    ~SecurityIntegration();
    
    // Não permitir cópias
    SecurityIntegration(const SecurityIntegration&) = delete;
    SecurityIntegration& operator=(const SecurityIntegration&) = delete;
    
    // Configuração
    SecurityIntegrationConfig m_config;
    
    // Componentes de segurança
    Security::SecureRandom* m_secureRandom;
    Security::CryptoManager* m_cryptoManager;
    Security::PacketAuthenticator* m_packetAuthenticator;
    Security::AntiTamperManager* m_antiTamperManager;
    
    // Estado
    bool m_initialized;
    bool m_networkIntegrated;
    bool m_voipIntegrated;
    bool m_screenShareIntegrated;
    bool m_serversIntegrated;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos privados
    void ApplySecurityPolicy();
    bool VerifyComponentIntegrity();
    void AdjustSecuritySettings(int level);
};

} // namespace Integration
} // namespace WYDBR

#endif // WYDBR_SECURITY_INTEGRATION_H
