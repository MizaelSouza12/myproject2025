/**
 * @file ScreenShareIntegration.h
 * @brief Integração do sistema de compartilhamento de tela com WYDStudio
 */

#ifndef WYDBR_SCREEN_SHARE_INTEGRATION_H
#define WYDBR_SCREEN_SHARE_INTEGRATION_H

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace WYDBR {

// Forward declarations
namespace ScreenShare {
class ScreenShareManager;
struct ScreenShareConfig;
struct ViewerInfo;
enum class ScreenShareState;
enum class SharePermission;
}

/**
 * Classe para integração do compartilhamento de tela no WYDStudio
 */
class ScreenShareIntegration {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para a integração
     */
    static ScreenShareIntegration& GetInstance();
    
    /**
     * Inicializa a integração
     * @param userId ID do usuário
     * @param userName Nome do usuário
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& userId, const std::string& userName);
    
    /**
     * Desliga a integração
     */
    void Shutdown();
    
    /**
     * Obtém estado atual do sistema
     * @return Estado atual
     */
    ScreenShare::ScreenShareState GetState() const;
    
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
     * @param hostName Nome do host
     * @return true se iniciado com sucesso
     */
    bool StartViewing(const std::string& hostId, const std::string& hostName);
    
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
    bool AddViewer(const std::string& userId, const std::string& userName, ScreenShare::SharePermission permission);
    
    /**
     * Remove um visualizador
     * @param userId ID do usuário
     * @return true se removido com sucesso
     */
    bool RemoveViewer(const std::string& userId);
    
    /**
     * Obtém lista de visualizadores atuais
     * @return Lista de informações de visualizadores
     */
    std::vector<ScreenShare::ViewerInfo> GetViewers() const;
    
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
     * Ajusta configurações de qualidade
     * @param qualityLevel Nível de qualidade (0-100)
     * @return true se ajustado com sucesso
     */
    bool AdjustQuality(int qualityLevel);
    
    /**
     * Envia comando de controle remoto
     * @param command Comando a ser enviado
     * @param data Dados adicionais
     * @return true se enviado com sucesso
     */
    bool SendRemoteControlCommand(const std::string& command, const std::string& data);
    
    /**
     * Obtém estatísticas do sistema
     * @return String formatada com estatísticas
     */
    std::string GetStatistics() const;
    
private:
    // Construtor e destrutor privados (singleton)
    ScreenShareIntegration();
    ~ScreenShareIntegration();
    
    // Não permitir cópias
    ScreenShareIntegration(const ScreenShareIntegration&) = delete;
    ScreenShareIntegration& operator=(const ScreenShareIntegration&) = delete;
    
    // Gerenciador de compartilhamento
    ScreenShare::ScreenShareManager& m_shareManager;
    
    // Estado
    bool m_initialized;
    
    // Dados do usuário
    std::string m_userId;
    std::string m_userName;
    
    // Callback para eventos
    void HandleScreenShareEvent(const std::string& eventType, const std::string& data);
};

} // namespace WYDBR

#endif // WYDBR_SCREEN_SHARE_INTEGRATION_H
