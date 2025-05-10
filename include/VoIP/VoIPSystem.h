
/**
 * @file VoIPSystem.h
 * @brief Sistema de VoIP para WYDBR 2.0
 */

#ifndef WYDBR_VOIP_SYSTEM_H
#define WYDBR_VOIP_SYSTEM_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

namespace WYDBR {
namespace VoIP {

// Forward declarations
class AudioManager;
class VoIPNetworkManager;
class ChannelManager;
class CodecManager;

// Tipos
using VoIPChannelID = uint32_t;
using ClientID = uint32_t;

// Estrutura de pacote de áudio
struct AudioPacket {
    ClientID clientId;
    VoIPChannelID channelId;
    uint32_t timestamp;
    uint16_t sequenceNumber;
    std::vector<uint8_t> audioData;
};

// Estrutura de configuração do canal
struct ChannelConfig {
    std::string name;
    bool isPublic;
    bool isModerated;
    uint32_t maxClients;
};

/**
 * Classe principal do sistema VoIP
 */
class VoIPSystem {
public:
    VoIPSystem();
    ~VoIPSystem();
    
    /**
     * Inicializa o sistema VoIP
     * @param configPath Caminho para o arquivo de configuração
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(const std::string& configPath = "");
    
    /**
     * Inicia o sistema VoIP
     * @param port Porta para escutar (0 = usar valor da configuração)
     * @return true se o início foi bem-sucedido, false caso contrário
     */
    bool Start(uint16_t port = 0);
    
    /**
     * Encerra o sistema VoIP
     */
    void Shutdown();
    
    /**
     * Processa a lógica do sistema VoIP
     * @param deltaTime Tempo desde o último update em milissegundos
     */
    void Update(float deltaTime);
    
    // Gerenciamento de canais
    VoIPChannelID CreateChannel(const ChannelConfig& config);
    bool DestroyChannel(VoIPChannelID channelId);
    bool JoinChannel(VoIPChannelID channelId, ClientID clientId);
    bool LeaveChannel(VoIPChannelID channelId, ClientID clientId);
    
    // Gerenciamento de áudio
    bool SendAudio(const AudioPacket& packet);
    
    // Configuração
    void SetClientMuted(ClientID clientId, bool muted);
    void SetClientDeafened(ClientID clientId, bool deafened);
    
private:
    // Evitar cópias
    VoIPSystem(const VoIPSystem&) = delete;
    VoIPSystem& operator=(const VoIPSystem&) = delete;
    
    // Componentes internos
    std::unique_ptr<AudioManager> m_audioManager;
    std::unique_ptr<VoIPNetworkManager> m_networkManager;
    std::unique_ptr<ChannelManager> m_channelManager;
    std::unique_ptr<CodecManager> m_codecManager;
    
    // Estado
    bool m_initialized;
    bool m_running;
    uint16_t m_port;
    std::string m_configPath;
    
    // Métodos internos
    bool LoadConfig();
    bool InitializeManagers();
    void ProcessAudio();
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_SYSTEM_H
