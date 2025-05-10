/**
 * @file ScreenShareManager.h
 * @brief Gerenciador principal do sistema de compartilhamento de tela
 */

#ifndef WYDBR_SCREEN_SHARE_MANAGER_H
#define WYDBR_SCREEN_SHARE_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include "../capture/ScreenCaptureManager.h"
#include "../encoding/ScreenEncoder.h"
#include "../transport/ScreenTransport.h"

namespace WYDBR {
namespace ScreenShare {

/**
 * Estado do sistema de compartilhamento
 */
enum class ScreenShareState {
    Uninitialized,   // Não inicializado
    Initializing,    // Em processo de inicialização
    Ready,           // Pronto para uso
    Capturing,       // Capturando e compartilhando
    Viewing,         // Visualizando compartilhamento
    Error,           // Erro
    Shutdown         // Desligado
};

/**
 * Tipo de permissão
 */
enum class SharePermission {
    None,            // Sem permissão
    View,            // Pode apenas visualizar
    Control,         // Pode visualizar e controlar
    Admin            // Controle total
};

/**
 * Informações sobre um visualizador
 */
struct ViewerInfo {
    std::string userId;       // ID do usuário
    std::string userName;     // Nome do usuário
    std::string ipAddress;    // Endereço IP
    uint16_t port;            // Porta
    SharePermission permission; // Permissão
    bool isActive;            // Está ativo
    int64_t joinTime;         // Timestamp de entrada
};

/**
 * Configuração do gerenciador
 */
struct ScreenShareConfig {
    ScreenCaptureConfig captureConfig;     // Configuração de captura
    EncoderConfig encoderConfig;           // Configuração de codificação
    StreamTransportConfig transportConfig; // Configuração de transporte
    std::string userId;                    // ID do usuário local
    std::string userName;                  // Nome do usuário local
    int maxViewers;                        // Máximo de visualizadores (0 = ilimitado)
    bool requirePermission;                // Requer permissão para visualizar
    bool allowRemoteControl;               // Permite controle remoto
    int sessionTimeoutSeconds;             // Timeout da sessão em segundos
};

/**
 * Callback para eventos de compartilhamento
 */
using ScreenShareEventCallback = std::function<void(const std::string& eventType, const std::string& data)>;

/**
 * Gerenciador principal do sistema de compartilhamento de tela
 */
class ScreenShareManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static ScreenShareManager& GetInstance();
    
    /**
     * Inicializa o sistema
     * @param config Configuração do sistema
     * @return true se inicializado com sucesso
     */
    bool Initialize(const ScreenShareConfig& config);
    
    /**
     * Desliga o sistema
     */
    void Shutdown();
    
    /**
     * Obtém o estado atual
     * @return Estado atual
     */
    ScreenShareState GetState() const;
    
    /**
     * Inicia compartilhamento de tela
     * @return true se iniciado com sucesso
     */
    bool StartSharing();
    
    /**
     * Para compartilhamento de tela
     */
    void StopSharing();
    
    /**
     * Inicia visualização de tela compartilhada
     * @param hostId ID do host
     * @return true se iniciado com sucesso
     */
    bool StartViewing(const std::string& hostId);
    
    /**
     * Para visualização
     */
    void StopViewing();
    
    /**
     * Adiciona um visualizador permitido
     * @param userId ID do usuário
     * @param userName Nome do usuário
     * @param permission Permissão
     * @return true se adicionado com sucesso
     */
    bool AddViewer(const std::string& userId, const std::string& userName, SharePermission permission);
    
    /**
     * Remove um visualizador
     * @param userId ID do usuário
     * @return true se removido com sucesso
     */
    bool RemoveViewer(const std::string& userId);
    
    /**
     * Altera permissão de um visualizador
     * @param userId ID do usuário
     * @param permission Nova permissão
     * @return true se alterado com sucesso
     */
    bool ChangeViewerPermission(const std::string& userId, SharePermission permission);
    
    /**
     * Obtém lista de visualizadores atuais
     * @return Lista de informações de visualizadores
     */
    std::vector<ViewerInfo> GetViewers() const;
    
    /**
     * Obtém lista de hosts disponíveis
     * @return Lista de IDs e nomes de hosts
     */
    std::vector<std::pair<std::string, std::string>> GetAvailableHosts() const;
    
    /**
     * Verifica se está compartilhando
     * @return true se estiver compartilhando
     */
    bool IsSharing() const;
    
    /**
     * Verifica se está visualizando
     * @return true se estiver visualizando
     */
    bool IsViewing() const;
    
    /**
     * Registra callback para eventos
     * @param callback Função a ser chamada quando eventos ocorrerem
     */
    void SetEventCallback(ScreenShareEventCallback callback);
    
    /**
     * Obtém estatísticas do sistema
     * @return String formatada com estatísticas
     */
    std::string GetStatistics() const;
    
    /**
     * Envia comando de controle remoto
     * @param command Comando a ser enviado
     * @param data Dados adicionais
     * @return true se enviado com sucesso
     */
    bool SendRemoteControlCommand(const std::string& command, const std::string& data);
    
    /**
     * Ajusta configurações de qualidade
     * @param qualityLevel Nível de qualidade (0-100)
     * @return true se ajustado com sucesso
     */
    bool AdjustQuality(int qualityLevel);
    
private:
    // Implementação privada do singleton
    ScreenShareManager();
    ~ScreenShareManager();
    
    // Estado
    std::atomic<ScreenShareState> m_state;
    
    // Componentes
    ScreenCaptureManager& m_captureManager;
    ScreenEncoder& m_encoder;
    ScreenTransportManager& m_transportManager;
    
    // Configuração
    ScreenShareConfig m_config;
    
    // Visualizadores
    std::vector<ViewerInfo> m_viewers;
    mutable std::mutex m_viewersMutex;
    
    // Host atual (quando visualizando)
    std::string m_currentHostId;
    
    // Callback de eventos
    ScreenShareEventCallback m_eventCallback;
    
    // Mutex para operações thread-safe
    mutable std::mutex m_mutex;
    
    // Métodos privados
    void ProcessCapturedFrame(const CapturedFrame& frame);
    void ProcessReceivedFrame(const EncodedFrame& frame, const std::string& senderId);
    void HandleTransportEvent(const std::string& eventType, const std::string& data);
    bool SetupCapture();
    bool SetupEncoder();
    bool SetupTransport();
};

} // namespace ScreenShare
} // namespace WYDBR

#endif // WYDBR_SCREEN_SHARE_MANAGER_H
