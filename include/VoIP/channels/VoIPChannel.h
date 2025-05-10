/**
 * @file VoIPChannel.h
 * @brief Sistema de canais para comunicação VoIP
 */

#ifndef WYDBR_VOIP_CHANNEL_H
#define WYDBR_VOIP_CHANNEL_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <cstdint>

namespace WYDBR {
namespace VoIP {

// Forward declarations
class INetworkTransport;
class IAudioCodec;

/**
 * Tipo de canal VoIP
 */
enum class ChannelType {
    OneOnOne,       // Canal particular entre dois usuários
    Group,          // Canal de grupo para até 20 usuários
    Guild,          // Canal de guild para até 50 usuários
    Broadcast,      // Canal de broadcast para até 250 ouvintes
    Emergency       // Canal de emergência (para notificações do servidor)
};

/**
 * Permissão em canal
 */
enum class ChannelPermission {
    Listen,         // Pode apenas ouvir
    Speak,          // Pode falar e ouvir
    Moderate,       // Pode moderar (silenciar outros)
    Admin           // Administrador do canal
};

/**
 * Estado de um participante
 */
enum class ParticipantState {
    Inactive,       // Não está transmitindo nem recebendo
    Speaking,       // Está transmitindo áudio
    Listening,      // Está apenas ouvindo
    Muted,          // Está silenciado (não pode falar)
    Deafened        // Está com áudio desligado (não pode ouvir)
};

/**
 * Informações sobre um participante
 */
struct Participant {
    uint32_t userId;                 // ID do usuário
    std::string userName;            // Nome do usuário
    ChannelPermission permission;    // Permissão no canal
    ParticipantState state;          // Estado atual
    float volumeMultiplier;          // Multiplicador de volume (0.0-2.0)
    bool localMuted;                 // Silenciado localmente
    std::string ipAddress;           // Endereço IP
    uint16_t port;                   // Porta
};

/**
 * Configuração de canal VoIP
 */
struct ChannelConfig {
    std::string name;               // Nome do canal
    ChannelType type;               // Tipo do canal
    uint32_t creatorId;             // ID do criador
    bool encrypted;                 // Canal criptografado
    bool recordable;                // Canal pode ser gravado
    uint32_t maxParticipants;       // Máximo de participantes
    std::string password;           // Senha (opcional)
    bool autoDestroy;               // Destruir quando vazio
};

/**
 * Callback para eventos de canal
 */
using ChannelEventCallback = std::function<void(const std::string& channelId, const std::string& eventType, const std::string& data)>;

/**
 * Callback para receber áudio
 */
using AudioReceivedCallback = std::function<void(uint32_t userId, const int16_t* audioData, size_t sampleCount, int channels)>;

/**
 * Interface para canal VoIP
 */
class IVoIPChannel {
public:
    virtual ~IVoIPChannel() = default;
    
    /**
     * Inicializa o canal
     * @param config Configuração do canal
     * @param transport Transporte de rede
     * @param codec Codec de áudio
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const ChannelConfig& config, 
                          std::shared_ptr<INetworkTransport> transport,
                          std::shared_ptr<IAudioCodec> codec) = 0;
    
    /**
     * Obtém o ID único do canal
     * @return ID do canal
     */
    virtual std::string GetId() const = 0;
    
    /**
     * Obtém o nome do canal
     * @return Nome do canal
     */
    virtual std::string GetName() const = 0;
    
    /**
     * Obtém o tipo do canal
     * @return Tipo do canal
     */
    virtual ChannelType GetType() const = 0;
    
    /**
     * Adiciona um participante ao canal
     * @param participant Informações do participante
     * @return true se adicionado com sucesso
     */
    virtual bool AddParticipant(const Participant& participant) = 0;
    
    /**
     * Remove um participante do canal
     * @param userId ID do usuário
     * @return true se removido com sucesso
     */
    virtual bool RemoveParticipant(uint32_t userId) = 0;
    
    /**
     * Envia dados de áudio para o canal
     * @param audioData Dados de áudio PCM
     * @param sampleCount Número de amostras
     * @param channels Número de canais
     * @return true se enviado com sucesso
     */
    virtual bool SendAudio(const int16_t* audioData, size_t sampleCount, int channels) = 0;
    
    /**
     * Registra callback para receber áudio
     * @param callback Função a ser chamada quando áudio for recebido
     */
    virtual void SetAudioReceivedCallback(AudioReceivedCallback callback) = 0;
    
    /**
     * Registra callback para eventos do canal
     * @param callback Função a ser chamada quando eventos ocorrerem
     */
    virtual void SetEventCallback(ChannelEventCallback callback) = 0;
    
    /**
     * Altera o estado de um participante
     * @param userId ID do usuário
     * @param state Novo estado
     * @return true se alterado com sucesso
     */
    virtual bool SetParticipantState(uint32_t userId, ParticipantState state) = 0;
    
    /**
     * Obtém informações de um participante
     * @param userId ID do usuário
     * @return Informações do participante ou nullptr se não encontrado
     */
    virtual const Participant* GetParticipant(uint32_t userId) const = 0;
    
    /**
     * Obtém lista de todos os participantes
     * @return Lista de participantes
     */
    virtual std::vector<Participant> GetAllParticipants() const = 0;
    
    /**
     * Verifica se um usuário está no canal
     * @param userId ID do usuário
     * @return true se estiver no canal
     */
    virtual bool HasParticipant(uint32_t userId) const = 0;
    
    /**
     * Verifica se o canal está ativo
     * @return true se estiver ativo
     */
    virtual bool IsActive() const = 0;
    
    /**
     * Ajusta o volume de um participante
     * @param userId ID do usuário
     * @param volumeMultiplier Multiplicador de volume (0.0-2.0)
     * @return true se ajustado com sucesso
     */
    virtual bool SetParticipantVolume(uint32_t userId, float volumeMultiplier) = 0;
    
    /**
     * Silencia ou remove silêncio de um participante localmente
     * @param userId ID do usuário
     * @param muted Estado de mudo
     * @return true se alterado com sucesso
     */
    virtual bool SetLocalMute(uint32_t userId, bool muted) = 0;
};

/**
 * Implementação de canal VoIP
 */
class VoIPChannel : public IVoIPChannel {
public:
    VoIPChannel();
    ~VoIPChannel() override;
    
    bool Initialize(const ChannelConfig& config, 
                  std::shared_ptr<INetworkTransport> transport,
                  std::shared_ptr<IAudioCodec> codec) override;
    std::string GetId() const override;
    std::string GetName() const override;
    ChannelType GetType() const override;
    bool AddParticipant(const Participant& participant) override;
    bool RemoveParticipant(uint32_t userId) override;
    bool SendAudio(const int16_t* audioData, size_t sampleCount, int channels) override;
    void SetAudioReceivedCallback(AudioReceivedCallback callback) override;
    void SetEventCallback(ChannelEventCallback callback) override;
    bool SetParticipantState(uint32_t userId, ParticipantState state) override;
    const Participant* GetParticipant(uint32_t userId) const override;
    std::vector<Participant> GetAllParticipants() const override;
    bool HasParticipant(uint32_t userId) const override;
    bool IsActive() const override;
    bool SetParticipantVolume(uint32_t userId, float volumeMultiplier) override;
    bool SetLocalMute(uint32_t userId, bool muted) override;
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * Gerenciador de canais VoIP
 */
class VoIPChannelManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static VoIPChannelManager& GetInstance();
    
    /**
     * Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * Cria um novo canal
     * @param config Configuração do canal
     * @return ID do canal criado ou string vazia se falhar
     */
    std::string CreateChannel(const ChannelConfig& config);
    
    /**
     * Destrói um canal
     * @param channelId ID do canal
     * @return true se destruído com sucesso
     */
    bool DestroyChannel(const std::string& channelId);
    
    /**
     * Obtém um canal pelo ID
     * @param channelId ID do canal
     * @return Ponteiro para o canal ou nullptr se não encontrado
     */
    std::shared_ptr<IVoIPChannel> GetChannel(const std::string& channelId);
    
    /**
     * Obtém lista de todos os canais
     * @return Lista de IDs de canais
     */
    std::vector<std::string> GetAllChannels() const;
    
    /**
     * Obtém lista de canais de um tipo específico
     * @param type Tipo de canal
     * @return Lista de IDs de canais
     */
    std::vector<std::string> GetChannelsByType(ChannelType type) const;
    
private:
    // Construtor e destrutor privados (singleton)
    VoIPChannelManager();
    ~VoIPChannelManager();
    
    // Não permitir cópias
    VoIPChannelManager(const VoIPChannelManager&) = delete;
    VoIPChannelManager& operator=(const VoIPChannelManager&) = delete;
    
    // Mapa de canais por ID
    std::unordered_map<std::string, std::shared_ptr<IVoIPChannel>> m_channels;
    mutable std::mutex m_channelsMutex;
    
    // Transporte e codec padrão
    std::shared_ptr<INetworkTransport> m_defaultTransport;
    std::shared_ptr<IAudioCodec> m_defaultCodec;
    
    // Estado
    bool m_initialized;
    
    // Gera ID único para canal
    std::string GenerateChannelId() const;
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_CHANNEL_H
