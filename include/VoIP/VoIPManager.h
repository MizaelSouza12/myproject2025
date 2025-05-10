/**
 * @file VoIPManager.h
 * @brief Gerenciador principal do sistema VoIP
 */

#ifndef WYDBR_VOIP_MANAGER_H
#define WYDBR_VOIP_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include <unordered_map>

namespace WYDBR {
namespace VoIP {

// Forward declarations
class IAudioCapture;
class IAudioProcessor;
class IAudioCodec;
class INetworkTransport;
class IVoIPChannel;
struct AudioCaptureConfig;
struct AudioProcessingConfig;
struct CodecConfig;
struct TransportConfig;
struct ChannelConfig;
struct Participant;
enum class ChannelType;
enum class CodecType;
enum class TransportType;

/**
 * Estado do sistema VoIP
 */
enum class VoIPState {
    Uninitialized,  // Não inicializado
    Initializing,   // Em processo de inicialização
    Ready,          // Pronto para uso
    Active,         // Em uso ativo
    Error,          // Erro
    Shutdown        // Desligado
};

/**
 * Configuração do gerenciador VoIP
 */
struct VoIPManagerConfig {
    AudioCaptureConfig captureConfig;       // Configuração de captura
    AudioProcessingConfig processingConfig; // Configuração de processamento
    CodecConfig codecConfig;                // Configuração de codec
    TransportConfig transportConfig;        // Configuração de transporte
    uint32_t userId;                        // ID do usuário local
    std::string userName;                   // Nome do usuário local
    bool autoConnectLastChannel;            // Reconectar ao último canal
    bool enableVoiceActivity;               // Usar detecção de atividade de voz
    bool enablePushToTalk;                  // Usar push-to-talk
    bool enableEchoCancellation;            // Cancelamento de eco
    bool enableNoiseReduction;              // Redução de ruído
    bool enableAGC;                         // Controle automático de ganho
};

/**
 * Callback para eventos VoIP
 */
using VoIPEventCallback = std::function<void(const std::string& eventType, const std::string& data)>;

/**
 * Gerenciador principal do sistema VoIP
 */
class VoIPManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static VoIPManager& GetInstance();
    
    /**
     * Inicializa o sistema VoIP
     * @param config Configuração do sistema
     * @return true se inicializado com sucesso
     */
    bool Initialize(const VoIPManagerConfig& config);
    
    /**
     * Desliga o sistema VoIP
     */
    void Shutdown();
    
    /**
     * Obtém o estado atual do sistema
     * @return Estado atual
     */
    VoIPState GetState() const;
    
    /**
     * Cria um novo canal
     * @param config Configuração do canal
     * @return ID do canal criado ou string vazia se falhar
     */
    std::string CreateChannel(const ChannelConfig& config);
    
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
     * Obtém o ID do canal atual
     * @return ID do canal ou string vazia se não conectado
     */
    std::string GetCurrentChannelId() const;
    
    /**
     * Obtém lista de canais disponíveis
     * @param type Tipo de canal ou nullptr para todos
     * @return Lista de IDs de canais
     */
    std::vector<std::string> GetAvailableChannels(ChannelType* type = nullptr) const;
    
    /**
     * Inicia transmissão de áudio
     * @return true se iniciado com sucesso
     */
    bool StartTransmission();
    
    /**
     * Para transmissão de áudio
     */
    void StopTransmission();
    
    /**
     * Verifica se está transmitindo
     * @return true se estiver transmitindo
     */
    bool IsTransmitting() const;
    
    /**
     * Define o estado de mudo
     * @param muted Estado de mudo
     */
    void SetMuted(bool muted);
    
    /**
     * Verifica se está mudo
     * @return true se estiver mudo
     */
    bool IsMuted() const;
    
    /**
     * Define o volume de saída
     * @param volume Volume de 0.0 a 1.0
     */
    void SetOutputVolume(float volume);
    
    /**
     * Obtém o volume de saída
     * @return Volume de 0.0 a 1.0
     */
    float GetOutputVolume() const;
    
    /**
     * Define o volume de entrada
     * @param volume Volume de 0.0 a 1.0
     */
    void SetInputVolume(float volume);
    
    /**
     * Obtém o volume de entrada
     * @return Volume de 0.0 a 1.0
     */
    float GetInputVolume() const;
    
    /**
     * Registra callback para eventos
     * @param callback Função a ser chamada quando eventos ocorrerem
     */
    void SetEventCallback(VoIPEventCallback callback);
    
    /**
     * Obtém estatísticas do sistema
     * @return String formatada com estatísticas
     */
    std::string GetStatistics() const;
    
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
     * Obtém lista de participantes do canal atual
     * @return Lista de participantes
     */
    std::vector<Participant> GetCurrentChannelParticipants() const;
    
private:
    // Construtor e destrutor privados (singleton)
    VoIPManager();
    ~VoIPManager();
    
    // Não permitir cópias
    VoIPManager(const VoIPManager&) = delete;
    VoIPManager& operator=(const VoIPManager&) = delete;
    
    // Estado
    std::atomic<VoIPState> m_state;
    
    // Configuração
    VoIPManagerConfig m_config;
    
    // Componentes do sistema
    std::unique_ptr<IAudioCapture> m_audioCapture;
    std::unique_ptr<IAudioProcessor> m_audioProcessor;
    std::shared_ptr<IAudioCodec> m_audioCodec;
    std::shared_ptr<INetworkTransport> m_transport;
    
    // Canal atual
    std::string m_currentChannelId;
    std::shared_ptr<IVoIPChannel> m_currentChannel;
    
    // Controle de transmissão
    std::atomic<bool> m_isTransmitting;
    std::atomic<bool> m_isMuted;
    
    // Volumes
    float m_inputVolume;
    float m_outputVolume;
    
    // Callback de eventos
    VoIPEventCallback m_eventCallback;
    
    // Mutex para operações thread-safe
    mutable std::mutex m_mutex;
    
    // Métodos privados
    void ProcessCapturedAudio(const int16_t* audioData, size_t frameCount, int channels);
    void ProcessReceivedAudio(uint32_t userId, const int16_t* audioData, size_t sampleCount, int channels);
    void HandleChannelEvent(const std::string& channelId, const std::string& eventType, const std::string& data);
    bool SetupAudioCapture();
    bool SetupAudioProcessor();
    bool SetupAudioCodec();
    bool SetupNetworkTransport();
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_MANAGER_H
