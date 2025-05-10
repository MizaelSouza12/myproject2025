/**
 * @file CommunicationClient.h
 * @brief Interface de comunicação integrada para o cliente WYDBR
 */

#ifndef WYDBR_COMMUNICATION_CLIENT_H
#define WYDBR_COMMUNICATION_CLIENT_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "../../VoIP/VoIPManager.h"
#include "../../ScreenShare/ScreenShareManager.h"

namespace WYDBR {
namespace Client {

/**
 * Tipo de comunicação
 */
enum class CommunicationType {
    VoIP,               // Comunicação por voz
    ScreenShare,        // Compartilhamento de tela
    Both                // Ambos
};

/**
 * Configuração de comunicação do cliente
 */
struct CommunicationConfig {
    VoIP::VoIPManagerConfig voipConfig;              // Configuração VoIP
    ScreenShare::ScreenShareConfig screenShareConfig; // Configuração de compartilhamento
    bool autoConnectToLastChannel;                   // Reconectar ao último canal
    bool showNotifications;                          // Mostrar notificações de eventos
    bool enableHotkeys;                              // Habilitar teclas de atalho
    std::string pushToTalkKey;                       // Tecla para push-to-talk
    std::string screenShareKey;                      // Tecla para compartilhamento
};

/**
 * Callback para eventos de comunicação
 */
using CommunicationEventCallback = std::function<void(const std::string& eventType, 
                                                    CommunicationType commType, 
                                                    const std::string& data)>;

/**
 * Interface do cliente de comunicação integrada
 */
class CommunicationClient {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o cliente
     */
    static CommunicationClient& GetInstance();
    
    /**
     * Inicializa o cliente
     * @param config Configuração do cliente
     * @return true se inicializado com sucesso
     */
    bool Initialize(const CommunicationConfig& config);
    
    /**
     * Desliga o cliente
     */
    void Shutdown();
    
    // ---- Métodos VoIP ----
    
    /**
     * Conecta a um canal VoIP
     * @param channelId ID do canal
     * @return true se conectado com sucesso
     */
    bool ConnectToVoIPChannel(const std::string& channelId);
    
    /**
     * Desconecta do canal VoIP atual
     * @return true se desconectado com sucesso
     */
    bool DisconnectFromVoIPChannel();
    
    /**
     * Cria um canal VoIP
     * @param name Nome do canal
     * @param type Tipo do canal
     * @return ID do canal criado ou string vazia se falhar
     */
    std::string CreateVoIPChannel(const std::string& name, VoIP::ChannelType type);
    
    /**
     * Inicia transmissão de voz (push-to-talk)
     * @return true se iniciado com sucesso
     */
    bool StartSpeaking();
    
    /**
     * Para transmissão de voz
     */
    void StopSpeaking();
    
    /**
     * Define estado de mudo para VoIP
     * @param muted Estado de mudo
     */
    void SetVoIPMuted(bool muted);
    
    /**
     * Obtém lista de canais VoIP disponíveis
     * @return Lista de IDs e nomes de canais
     */
    std::vector<std::pair<std::string, std::string>> GetAvailableVoIPChannels() const;
    
    /**
     * Obtém lista de participantes do canal VoIP atual
     * @return Lista de IDs e nomes de participantes
     */
    std::vector<std::pair<std::string, std::string>> GetCurrentVoIPParticipants() const;
    
    // ---- Métodos de Compartilhamento de Tela ----
    
    /**
     * Inicia compartilhamento de tela
     * @return true se iniciado com sucesso
     */
    bool StartScreenSharing();
    
    /**
     * Para compartilhamento de tela
     */
    void StopScreenSharing();
    
    /**
     * Inicia visualização de tela compartilhada
     * @param hostId ID do host
     * @return true se iniciado com sucesso
     */
    bool StartScreenViewing(const std::string& hostId);
    
    /**
     * Para visualização de tela
     */
    void StopScreenViewing();
    
    /**
     * Obtém lista de hosts disponíveis para compartilhamento
     * @return Lista de IDs e nomes de hosts
     */
    std::vector<std::pair<std::string, std::string>> GetAvailableScreenHosts() const;
    
    /**
     * Ajusta qualidade do compartilhamento
     * @param qualityLevel Nível de qualidade (0-100)
     * @return true se ajustado com sucesso
     */
    bool AdjustScreenShareQuality(int qualityLevel);
    
    // ---- Métodos Comuns ----
    
    /**
     * Registra callback para eventos
     * @param callback Função a ser chamada quando eventos ocorrerem
     */
    void SetEventCallback(CommunicationEventCallback callback);
    
    /**
     * Obtém estatísticas de comunicação
     * @param type Tipo de comunicação
     * @return String formatada com estatísticas
     */
    std::string GetStatistics(CommunicationType type) const;
    
    /**
     * Processa tecla pressionada (para hotkeys)
     * @param keyCode Código da tecla
     * @param modifiers Modificadores (Ctrl, Alt, etc)
     * @return true se a tecla foi processada
     */
    bool ProcessKeyPress(int keyCode, int modifiers);
    
    /**
     * Processa tecla liberada
     * @param keyCode Código da tecla
     * @param modifiers Modificadores (Ctrl, Alt, etc)
     * @return true se a tecla foi processada
     */
    bool ProcessKeyRelease(int keyCode, int modifiers);
    
private:
    // Construtor e destrutor privados (singleton)
    CommunicationClient();
    ~CommunicationClient();
    
    // Não permitir cópias
    CommunicationClient(const CommunicationClient&) = delete;
    CommunicationClient& operator=(const CommunicationClient&) = delete;
    
    // Gerenciadores
    VoIP::VoIPManager& m_voipManager;
    ScreenShare::ScreenShareManager& m_screenShareManager;
    
    // Configuração
    CommunicationConfig m_config;
    
    // Estado
    bool m_initialized;
    
    // Dados do canal atual
    std::string m_currentVoIPChannelId;
    std::string m_currentScreenHostId;
    
    // Mutex para operações thread-safe
    mutable std::mutex m_mutex;
    
    // Callback de eventos
    CommunicationEventCallback m_eventCallback;
    
    // Callbacks específicos
    void HandleVoIPEvent(const std::string& eventType, const std::string& data);
    void HandleScreenShareEvent(const std::string& eventType, const std::string& data);
};

} // namespace Client
} // namespace WYDBR

#endif // WYDBR_COMMUNICATION_CLIENT_H
