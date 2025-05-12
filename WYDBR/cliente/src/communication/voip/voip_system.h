#ifndef WYDBR_VOIP_SYSTEM_H
#define WYDBR_VOIP_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <bitset>
#include <mutex>
#include <atomic>

namespace wydbr {
namespace communication {

// Enumeração para status do microfone
enum class MicrophoneStatus {
    AVAILABLE,    // Microfone está disponível e funcionando
    UNAVAILABLE,  // Microfone não está disponível
    DENIED,       // Permissão negada
    MUTED,        // Disponível, mas silenciado pelo usuário
    ERROR         // Erro ao acessar o microfone
};

// Enumeração para qualidade do áudio
enum class AudioQuality {
    LOW,          // Baixa qualidade (maior economia de banda)
    MEDIUM,       // Qualidade média (equilíbrio)
    HIGH,         // Alta qualidade (melhor som)
    ULTRA         // Qualidade ultra (sem compressão)
};

// Enumeração para modo de ativação do microfone
enum class MicActivationMode {
    PUSH_TO_TALK, // Ativo apenas quando tecla é pressionada
    VOICE,        // Ativação por detecção de voz
    ALWAYS_ON     // Sempre ativo
};

// Configuração do sistema VOIP
struct VoIPConfig {
    AudioQuality quality;
    MicActivationMode activationMode;
    int pushToTalkKey;      // Código da tecla para PTT
    float voiceThreshold;   // Limite para detecção de voz (0.0 a 1.0)
    int sampleRate;         // Taxa de amostragem
    int channels;           // Número de canais (1=mono, 2=stereo)
    bool enableEchoCancellation;
    bool enableNoiseSuppression;
    bool enableAGC;         // Controle automático de ganho
    bool enableSpatialAudio;
    bool enableVAD;         // Detecção de atividade de voz
    float masterVolume;     // Volume mestre (0.0 a 1.0)
    float microphoneGain;   // Ganho do microfone (0.0 a 1.0)
    
    // Construtor com valores padrão
    VoIPConfig() :
        quality(AudioQuality::MEDIUM),
        activationMode(MicActivationMode::PUSH_TO_TALK),
        pushToTalkKey(0x56), // 'V' key
        voiceThreshold(0.05f),
        sampleRate(48000),
        channels(1),
        enableEchoCancellation(true),
        enableNoiseSuppression(true),
        enableAGC(true),
        enableSpatialAudio(true),
        enableVAD(true),
        masterVolume(1.0f),
        microphoneGain(1.0f)
    {}
};

// Estrutura para representar um usuário VOIP
struct VoIPUser {
    std::string userId;
    std::string displayName;
    bool isSpeaking;
    bool isMuted;
    bool isDeafened;
    float signalStrength;  // 0.0 a 1.0
    float inputLevel;      // 0.0 a 1.0
    
    // Posição espacial
    float posX;
    float posY;
    float posZ;
    
    // Orientação espacial
    float dirX;
    float dirY;
    float dirZ;
};

// Callback para evento de alteração de estado de fala
using SpeakingStateCallback = std::function<void(const std::string& userId, bool isSpeaking)>;

// Callback para evento de alteração de volume
using VolumeChangeCallback = std::function<void(const std::string& userId, float volume)>;

// Callback para evento de erro
using ErrorCallback = std::function<void(int errorCode, const std::string& errorMessage)>;

// Callback para evento de alteração de conectividade
using ConnectivityCallback = std::function<void(bool isConnected)>;

/**
 * @class VoIPSystem
 * @brief Sistema de Voice over IP com suporte a áudio espacial
 * 
 * Sistema responsável por gerenciar a comunicação por voz entre jogadores,
 * incluindo suporte a áudio espacial 3D, canais de voz e grupos de conversa.
 */
class VoIPSystem {
public:
    /**
     * @brief Obtém a instância única do sistema (singleton)
     * @return Referência para a instância
     */
    static VoIPSystem& getInstance();
    
    /**
     * @brief Inicializa o sistema VOIP
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const VoIPConfig& config = VoIPConfig());
    
    /**
     * @brief Desliga o sistema e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Conecta ao servidor de voz
     * @param serverAddress Endereço do servidor
     * @param port Porta do servidor
     * @param username Nome de usuário
     * @param authToken Token de autenticação
     * @return true se conectado com sucesso
     */
    bool connect(const std::string& serverAddress, int port, 
                 const std::string& username, const std::string& authToken);
    
    /**
     * @brief Desconecta do servidor de voz
     */
    void disconnect();
    
    /**
     * @brief Verifica se está conectado
     * @return true se conectado
     */
    bool isConnected() const;
    
    /**
     * @brief Entra em um canal de voz
     * @param channelId ID do canal
     * @return true se entrou com sucesso
     */
    bool joinChannel(const std::string& channelId);
    
    /**
     * @brief Sai de um canal de voz
     * @param channelId ID do canal
     * @return true se saiu com sucesso
     */
    bool leaveChannel(const std::string& channelId);
    
    /**
     * @brief Obtém a lista de canais disponíveis
     * @return Lista de IDs de canais
     */
    std::vector<std::string> getAvailableChannels() const;
    
    /**
     * @brief Obtém o canal atual
     * @return ID do canal atual ou string vazia se não estiver em nenhum canal
     */
    std::string getCurrentChannel() const;
    
    /**
     * @brief Obtém a lista de usuários no canal atual
     * @return Lista de informações de usuários
     */
    std::vector<VoIPUser> getUsersInCurrentChannel() const;
    
    /**
     * @brief Ativa o microfone
     * @return true se ativado com sucesso
     */
    bool enableMicrophone();
    
    /**
     * @brief Desativa o microfone
     */
    void disableMicrophone();
    
    /**
     * @brief Verifica se o microfone está ativo
     * @return true se ativo
     */
    bool isMicrophoneEnabled() const;
    
    /**
     * @brief Obtém o status atual do microfone
     * @return Status do microfone
     */
    MicrophoneStatus getMicrophoneStatus() const;
    
    /**
     * @brief Define o modo de ativação do microfone
     * @param mode Novo modo de ativação
     */
    void setMicActivationMode(MicActivationMode mode);
    
    /**
     * @brief Obtém o modo atual de ativação do microfone
     * @return Modo de ativação
     */
    MicActivationMode getMicActivationMode() const;
    
    /**
     * @brief Define o volume mestre do sistema
     * @param volume Volume (0.0 a 1.0)
     */
    void setMasterVolume(float volume);
    
    /**
     * @brief Obtém o volume mestre atual
     * @return Volume mestre
     */
    float getMasterVolume() const;
    
    /**
     * @brief Define o ganho do microfone
     * @param gain Ganho (0.0 a 1.0)
     */
    void setMicrophoneGain(float gain);
    
    /**
     * @brief Obtém o ganho atual do microfone
     * @return Ganho do microfone
     */
    float getMicrophoneGain() const;
    
    /**
     * @brief Silencia um usuário específico
     * @param userId ID do usuário
     * @param muted Se deve silenciar (true) ou não (false)
     * @return true se a operação foi bem-sucedida
     */
    bool muteUser(const std::string& userId, bool muted);
    
    /**
     * @brief Silencia todos os usuários
     * @param muted Se deve silenciar (true) ou não (false)
     */
    void muteAll(bool muted);
    
    /**
     * @brief Verifica se um usuário está silenciado
     * @param userId ID do usuário
     * @return true se silenciado
     */
    bool isUserMuted(const std::string& userId) const;
    
    /**
     * @brief Define o volume de um usuário específico
     * @param userId ID do usuário
     * @param volume Volume (0.0 a 1.0)
     * @return true se a operação foi bem-sucedida
     */
    bool setUserVolume(const std::string& userId, float volume);
    
    /**
     * @brief Obtém o volume atual de um usuário
     * @param userId ID do usuário
     * @return Volume do usuário
     */
    float getUserVolume(const std::string& userId) const;
    
    /**
     * @brief Ativa ou desativa o áudio espacial
     * @param enabled Se deve ativar (true) ou não (false)
     */
    void enableSpatialAudio(bool enabled);
    
    /**
     * @brief Verifica se o áudio espacial está ativo
     * @return true se ativo
     */
    bool isSpatialAudioEnabled() const;
    
    /**
     * @brief Atualiza a posição e orientação do ouvinte (jogador local)
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param dirX Direção X
     * @param dirY Direção Y
     * @param dirZ Direção Z
     */
    void updateListenerPosition(float posX, float posY, float posZ,
                               float dirX, float dirY, float dirZ);
    
    /**
     * @brief Atualiza a posição e orientação de um usuário específico
     * @param userId ID do usuário
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param dirX Direção X
     * @param dirY Direção Y
     * @param dirZ Direção Z
     * @return true se a operação foi bem-sucedida
     */
    bool updateUserPosition(const std::string& userId,
                           float posX, float posY, float posZ,
                           float dirX, float dirY, float dirZ);
    
    /**
     * @brief Define a distância máxima para áudio espacial
     * @param distance Distância máxima em unidades do mundo
     */
    void setSpatialMaxDistance(float distance);
    
    /**
     * @brief Obtém a distância máxima atual para áudio espacial
     * @return Distância máxima
     */
    float getSpatialMaxDistance() const;
    
    /**
     * @brief Define a qualidade do áudio
     * @param quality Nova qualidade
     */
    void setAudioQuality(AudioQuality quality);
    
    /**
     * @brief Obtém a qualidade atual do áudio
     * @return Qualidade do áudio
     */
    AudioQuality getAudioQuality() const;
    
    /**
     * @brief Registra um callback para evento de alteração de estado de fala
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerSpeakingStateCallback(SpeakingStateCallback callback);
    
    /**
     * @brief Remove um callback de evento de estado de fala
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterSpeakingStateCallback(int callbackId);
    
    /**
     * @brief Registra um callback para evento de alteração de volume
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerVolumeChangeCallback(VolumeChangeCallback callback);
    
    /**
     * @brief Remove um callback de evento de alteração de volume
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterVolumeChangeCallback(int callbackId);
    
    /**
     * @brief Registra um callback para evento de erro
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerErrorCallback(ErrorCallback callback);
    
    /**
     * @brief Remove um callback de evento de erro
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterErrorCallback(int callbackId);
    
    /**
     * @brief Registra um callback para evento de alteração de conectividade
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerConnectivityCallback(ConnectivityCallback callback);
    
    /**
     * @brief Remove um callback de evento de alteração de conectividade
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterConnectivityCallback(int callbackId);
    
    /**
     * @brief Obtém a latência da comunicação de voz
     * @return Latência em milissegundos
     */
    int getVoiceLatency() const;
    
    /**
     * @brief Obtém a taxa de perda de pacotes
     * @return Taxa de perda (0.0 a 1.0)
     */
    float getPacketLossRate() const;
    
    /**
     * @brief Obtém a taxa de bits atual
     * @return Taxa de bits em kbps
     */
    int getBitrate() const;
    
private:
    // Construtor e destrutor privados (singleton)
    VoIPSystem();
    ~VoIPSystem();
    
    // Prevenir cópia (singleton)
    VoIPSystem(const VoIPSystem&) = delete;
    VoIPSystem& operator=(const VoIPSystem&) = delete;
    
    // Implementação privada (Pimpl idiom)
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Aliases para facilitar o uso
using VoIP = VoIPSystem;

} // namespace communication
} // namespace wydbr

#endif // WYDBR_VOIP_SYSTEM_H
