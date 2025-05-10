/**
 * @file AdminPanel.h
 * @brief Painel de administração unificado para WYDStudio
 */

#ifndef WYDBR_ADMIN_PANEL_H
#define WYDBR_ADMIN_PANEL_H

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace WYDBR {

// Forward declarations
class ServerInterface;
class TMSrvIntegration;
class DBSrvIntegration;

/**
 * Classe base para todos os painéis de administração
 */
class AdminPanelBase {
public:
    /**
     * Construtor
     */
    AdminPanelBase(const std::string& name);
    
    /**
     * Destrutor virtual
     */
    virtual ~AdminPanelBase() = default;
    
    /**
     * Inicializa o painel
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize() = 0;
    
    /**
     * Atualiza o painel
     */
    virtual void Update() = 0;
    
    /**
     * Renderiza o painel
     */
    virtual void Render() = 0;
    
    /**
     * Processa eventos de entrada
     * @param event Evento a ser processado
     * @return true se o evento foi processado
     */
    virtual bool ProcessEvent(const Event& event) = 0;
    
    /**
     * Obtém o nome do painel
     * @return Nome do painel
     */
    const std::string& GetName() const;
    
    /**
     * Verifica se o painel está ativo
     * @return true se o painel estiver ativo
     */
    bool IsActive() const;
    
    /**
     * Define se o painel está ativo
     * @param active Estado de ativação do painel
     */
    void SetActive(bool active);
    
private:
    std::string m_name;
    bool m_active;
};

/**
 * Painel de status do servidor
 */
class ServerStatusPanel : public AdminPanelBase {
public:
    /**
     * Construtor
     * @param tmsrv Integração do TMSrv
     * @param dbsrv Integração do DBSrv
     */
    ServerStatusPanel(TMSrvIntegration* tmsrv, DBSrvIntegration* dbsrv);
    
    /**
     * Destrutor
     */
    ~ServerStatusPanel() override;
    
    /**
     * Inicializa o painel
     * @return true se inicializado com sucesso
     */
    bool Initialize() override;
    
    /**
     * Atualiza o painel
     */
    void Update() override;
    
    /**
     * Renderiza o painel
     */
    void Render() override;
    
    /**
     * Processa eventos de entrada
     * @param event Evento a ser processado
     * @return true se o evento foi processado
     */
    bool ProcessEvent(const Event& event) override;
    
private:
    TMSrvIntegration* m_tmsrv;
    DBSrvIntegration* m_dbsrv;
    
    // Dados de status
    std::string m_tmsrvStatus;
    std::string m_dbsrvStatus;
    std::string m_tmsrvStats;
    std::string m_dbsrvStats;
    
    // Callbacks
    void OnTMSrvStatusChanged(ServerStatus status);
    void OnDBSrvStatusChanged(ServerStatus status);
    
    // Métodos auxiliares
    void RefreshStats();
    std::string ServerStatusToString(ServerStatus status);
};

/**
 * Painel de gerenciamento de jogadores
 */
class PlayerManagementPanel : public AdminPanelBase {
public:
    /**
     * Construtor
     * @param tmsrv Integração do TMSrv
     * @param dbsrv Integração do DBSrv
     */
    PlayerManagementPanel(TMSrvIntegration* tmsrv, DBSrvIntegration* dbsrv);
    
    /**
     * Destrutor
     */
    ~PlayerManagementPanel() override;
    
    /**
     * Inicializa o painel
     * @return true se inicializado com sucesso
     */
    bool Initialize() override;
    
    /**
     * Atualiza o painel
     */
    void Update() override;
    
    /**
     * Renderiza o painel
     */
    void Render() override;
    
    /**
     * Processa eventos de entrada
     * @param event Evento a ser processado
     * @return true se o evento foi processado
     */
    bool ProcessEvent(const Event& event) override;
    
private:
    TMSrvIntegration* m_tmsrv;
    DBSrvIntegration* m_dbsrv;
    
    // Dados de jogadores
    std::vector<ClientInfo> m_onlinePlayers;
    
    // Métodos auxiliares
    void RefreshPlayers();
    void KickPlayer(uint32_t playerId);
    void BanPlayer(uint32_t playerId, const std::string& reason, int durationHours);
    void ViewPlayerDetails(uint32_t playerId);
};

/**
 * Classe principal para o painel de administração unificado
 */
class AdminPanel {
public:
    /**
     * Construtor
     * @param tmsrv Integração do TMSrv
     * @param dbsrv Integração do DBSrv
     */
    AdminPanel(TMSrvIntegration* tmsrv, DBSrvIntegration* dbsrv);
    
    /**
     * Destrutor
     */
    ~AdminPanel();
    
    /**
     * Inicializa o painel de administração
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * Atualiza o painel de administração
     */
    void Update();
    
    /**
     * Renderiza o painel de administração
     */
    void Render();
    
    /**
     * Processa eventos de entrada
     * @param event Evento a ser processado
     * @return true se o evento foi processado
     */
    bool ProcessEvent(const Event& event);
    
    /**
     * Mostra o painel de administração
     */
    void Show();
    
    /**
     * Esconde o painel de administração
     */
    void Hide();
    
    /**
     * Verifica se o painel de administração está visível
     * @return true se estiver visível
     */
    bool IsVisible() const;
    
private:
    TMSrvIntegration* m_tmsrv;
    DBSrvIntegration* m_dbsrv;
    
    // Painéis
    std::vector<std::unique_ptr<AdminPanelBase>> m_panels;
    int m_activePanel;
    
    // Estado
    bool m_initialized;
    bool m_visible;
    
    // Métodos auxiliares
    void InitializePanels();
    void SwitchToPanel(int panelIndex);
};

} // namespace WYDBR

#endif // WYDBR_ADMIN_PANEL_H
