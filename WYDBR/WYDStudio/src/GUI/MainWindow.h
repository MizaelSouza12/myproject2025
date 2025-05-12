#pragma once

#include "../../include/WYDStudioApp.h"
#include <memory>
#include <string>
#include <functional>
#include <vector>

namespace WYDStudio {

/**
 * @enum UITheme
 * @brief Temas de interface disponíveis
 */
enum class UITheme {
    DARK,
    LIGHT,
    CLASSIC,
    MODERN,
    CUSTOM
};

/**
 * @class MainWindow
 * @brief Janela principal da aplicação WYDStudio
 * 
 * Esta classe gerencia a interface gráfica principal do WYDStudio,
 * incluindo menus, painéis e interações com o usuário.
 */
class MainWindow {
public:
    /**
     * @brief Construtor
     * @param app Ponteiro para a aplicação principal
     */
    MainWindow(std::shared_ptr<WYDStudioApp> app);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~MainWindow();
    
    /**
     * @brief Inicializa a janela principal
     * @param title Título da janela
     * @param width Largura inicial
     * @param height Altura inicial
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& title = "WYD Studio", 
                   int width = 1280, 
                   int height = 720);
    
    /**
     * @brief Renderiza a janela
     */
    void Render();
    
    /**
     * @brief Processa eventos da interface
     */
    void ProcessEvents();
    
    /**
     * @brief Define o tema da interface
     * @param theme Tema a ser aplicado
     */
    void SetTheme(UITheme theme);
    
    /**
     * @brief Define a função de callback para eventos de interface
     * @param callback Função a ser chamada quando ocorrer um evento
     */
    void SetEventCallback(std::function<void(const std::string&, const std::string&)> callback);
    
    /**
     * @brief Exibe uma mensagem de notificação
     * @param message Mensagem a ser exibida
     * @param title Título da notificação
     * @param isError true se for uma mensagem de erro
     */
    void ShowNotification(const std::string& message, 
                         const std::string& title = "Notificação",
                         bool isError = false);
    
    /**
     * @brief Exibe uma caixa de diálogo de confirmação
     * @param message Mensagem a ser exibida
     * @param title Título da caixa de diálogo
     * @return true se o usuário confirmou
     */
    bool ShowConfirmationDialog(const std::string& message, 
                               const std::string& title = "Confirmação");
    
    /**
     * @brief Exibe uma caixa de diálogo de entrada de texto
     * @param message Mensagem a ser exibida
     * @param title Título da caixa de diálogo
     * @param defaultValue Valor padrão
     * @return Texto inserido pelo usuário
     */
    std::string ShowInputDialog(const std::string& message, 
                              const std::string& title = "Entrada",
                              const std::string& defaultValue = "");
    
    /**
     * @brief Exibe o painel de dashboard
     */
    void ShowDashboard();
    
    /**
     * @brief Exibe a interface de um agente específico
     * @param type Tipo do agente
     */
    void ShowAgentInterface(AgentType type);
    
    /**
     * @brief Exibe o explorador de dados do jogo
     */
    void ShowDataExplorer();
    
    /**
     * @brief Exibe estatísticas em tempo real do servidor
     */
    void ShowLiveServerStats();
    
    /**
     * @brief Exibe a interface de configurações
     */
    void ShowSettings();
    
    /**
     * @brief Exibe a interface de console de comandos
     */
    void ShowCommandConsole();
    
    /**
     * @brief Exibe a interface de logs do sistema
     */
    void ShowSystemLogs();
    
    /**
     * @brief Exibe a interface de visualização de entidades do jogo
     * @param entityType Tipo de entidade (item, mob, npc, etc.)
     */
    void ShowEntityViewer(const std::string& entityType);
    
    /**
     * @brief Atualiza o status da aplicação na interface
     * @param status Status atual
     */
    void UpdateStatus(const std::string& status);
    
    /**
     * @brief Verifica se a janela está aberta
     * @return true se a janela está aberta
     */
    bool IsOpen() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
    std::shared_ptr<WYDStudioApp> m_app;
    
    // Renderiza os diferentes componentes da UI
    void RenderMainMenu();
    void RenderStatusBar();
    void RenderToolbar();
    void RenderSidebar();
    void RenderMainContent();
    void RenderAgentChat(AgentType type);
    void RenderConsole();
    void RenderDataTable(const std::vector<std::vector<std::string>>& data, 
                         const std::vector<std::string>& headers);
};

} // namespace WYDStudio