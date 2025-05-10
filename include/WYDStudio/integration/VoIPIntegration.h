/**
 * @file VoIPIntegration.h
 * @brief Integração do sistema VoIP com WYDStudio
 */

#ifndef WYDBR_VOIP_INTEGRATION_H
#define WYDBR_VOIP_INTEGRATION_H

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace WYDBR {

// Forward declarations
namespace VoIP {
class VoIPManager;
struct VoIPManagerConfig;
struct ChannelConfig;
struct Participant;
enum class ChannelType;
enum class VoIPState;
}

/**
 * Classe para integração do VoIP no WYDStudio
 */
class VoIPIntegration {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para a integração
     */
    static VoIPIntegration& GetInstance();
    
    /**
     * Inicializa a integração do VoIP
     * @param userId ID do usuário
     * @param userName Nome do usuário
     * @return true se inicializado com sucesso
     */
    bool Initialize(uint32_t userId, const std::string& userName);
    
    /**
     * Desliga a integração do VoIP
     */
    void Shutdown();
    
    /**
     * Obtém o estado atual do sistema VoIP
     * @return Estado atual
     */
    VoIP::VoIPState GetVoIPState() const;
    
    /**
     * Cria um canal VoIP
     * @param name Nome do canal
     * @param type Tipo do canal
     * @param maxParticipants Máximo de participantes (0 = padrão)
     * @param password Senha (opcional)
     * @return ID do canal criado ou string vazia se falhar
     */
    std::string CreateChannel(const std::string& name, VoIP::ChannelType type, 
                            uint32_t maxParticipants = 0, const std::string& password = "");
    
    /**
     * Conecta a um canal
     * @param channelId ID do canal
     * @return true se conectado com sucesso
     */
    bool ConnectToChannel(const std::string& channelId);
    
    /**
     * Desconecta do canal atual
     * @return true se desconectado com sucesso
     */
    bool DisconnectFromChannel();
    
    /**
     * Inicia transmissão de áudio (push-to-talk)
     * @return true se iniciado com sucesso
     */
    bool StartSpeaking();
    
    /**
     * Para transmissão de áudio
     */
    void StopSpeaking();
    
    /**
     * Define o estado de mudo
     * @param muted Estado de mudo
     */
    void SetMuted(bool muted);
    
    /**
     * Obtém lista de canais disponíveis
     * @param type Tipo de canal ou nullptr para todos
     * @return Lista de informações de canais
     */
    std::vector<std::pair<std::string, std::string>> GetAvailableChannels(VoIP::ChannelType* type = nullptr) const;
    
    /**
     * Obtém lista de participantes do canal atual
     * @return Lista de participantes
     */
    std::vector<VoIP::Participant> GetCurrentChannelParticipants() const;
    
    /**
     * Silencia ou remove silêncio de um usuário localmente
     * @param userId ID do usuário
     * @param muted Estado de mudo
     * @return true se alterado com sucesso
     */
    bool SetUserLocalMute(uint32_t userId, bool muted);
    
    /**
     * Ajusta o volume de um usuário
     * @param userId ID do usuário
     * @param volumeMultiplier Multiplicador de volume (0.0-2.0)
     * @return true se ajustado com sucesso
     */
    bool SetUserVolume(uint32_t userId, float volumeMultiplier);
    
    /**
     * Define o volume de saída (global)
     * @param volume Volume de 0.0 a 1.0
     */
    void SetOutputVolume(float volume);
    
    /**
     * Define o volume de entrada (microfone)
     * @param volume Volume de 0.0 a 1.0
     */
    void SetInputVolume(float volume);
    
    /**
     * Verifica se um usuário está falando
     * @param userId ID do usuário
     * @return true se estiver falando
     */
    bool IsUserSpeaking(uint32_t userId) const;
    
    /**
     * Obtém estatísticas do sistema VoIP
     * @return String formatada com estatísticas
     */
    std::string GetStatistics() const;
    
private:
    // Construtor e destrutor privados (singleton)
    VoIPIntegration();
    ~VoIPIntegration();
    
    // Não permitir cópias
    VoIPIntegration(const VoIPIntegration&) = delete;
    VoIPIntegration& operator=(const VoIPIntegration&) = delete;
    
    // Gerenciador VoIP
    VoIP::VoIPManager& m_voipManager;
    
    // Estado
    bool m_initialized;
    
    // Dados do usuário
    uint32_t m_userId;
    std::string m_userName;
    
    // Mapa de usuários falando
    mutable std::mutex m_speakingUsersMutex;
    std::unordered_map<uint32_t, bool> m_speakingUsers;
    
    // Callback para eventos VoIP
    void HandleVoIPEvent(const std::string& eventType, const std::string& data);
};

} // namespace WYDBR

#endif // WYDBR_VOIP_INTEGRATION_H
