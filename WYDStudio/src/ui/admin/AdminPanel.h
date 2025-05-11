#ifndef WYDSTUDIO_ADMIN_PANEL_H
#define WYDSTUDIO_ADMIN_PANEL_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include <QDockWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QTextEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QProgressBar>
#include <QSlider>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QStackedWidget>
#include <QAction>
#include <QIcon>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include <QTimer>
#include <QMap>
#include <QVector>
#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include "../../core/WYDStudioFacade.h"
#include "../common/FileBrowser.h"
#include "../common/FileBrowserButton.h"
#include "../models/ServerStatusModel.h"
#include "panels/PlayerManagerPanel.h"
#include "panels/WorldManagerPanel.h"
#include "panels/EconomyManagerPanel.h"
#include "panels/SecurityManagerPanel.h"
#include "panels/ServerConfigPanel.h"
#include "panels/ContentManagerPanel.h"
#include "panels/CommunicationPanel.h"
#include "panels/AnalyticsPanel.h"
#include "dialogs/AdminSettingsDialog.h"
#include "widgets/AIAssistantWidget.h"
#include "widgets/ServerStatusWidget.h"
#include "widgets/QuickActionsWidget.h"
#include "widgets/NotificationsWidget.h"

namespace wydstudio {
namespace ui {
namespace admin {

/**
 * @brief Configuração do painel de administração
 */
struct AdminPanelConfig {
    std::string serverAddress;
    int serverPort;
    std::string username;
    std::string defaultTheme;
    bool startMinimized;
    bool showTrayIcon;
    bool enableSounds;
    bool autoSaveSettings;
    int autoSaveInterval;
    bool showNotifications;
    bool autoReconnect;
    bool logActions;
    std::string logFilePath;
    std::string aiAssistantModel;
    bool enableAIAssistant;
    std::vector<std::string> favoriteQuickActions;
    std::vector<std::string> recentCommands;
    std::map<std::string, std::string> customSettings;
};

/**
 * @brief Estado atual do painel de administração
 */
enum class AdminPanelStatus {
    DISCONNECTED,    // Desconectado do servidor
    CONNECTING,      // Conectando ao servidor
    CONNECTED,       // Conectado ao servidor
    ADMIN_MODE,      // Modo administrador completo
    GM_MODE,         // Modo Game Master
    DEV_MODE,        // Modo desenvolvedor
    ERROR            // Estado de erro
};

/**
 * @brief Janela principal do painel de administração
 */
class AdminPanel : public QMainWindow {
    Q_OBJECT
    
public:
    /**
     * @brief Construtor
     * @param parent Widget pai
     */
    AdminPanel(QWidget* parent = nullptr);
    
    /**
     * @brief Destrutor
     */
    ~AdminPanel();
    
    /**
     * @brief Inicializa o painel de administração
     * @param facade Referência ao facade do WYDStudio
     * @param config Configuração do painel
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::shared_ptr<core::WYDStudioFacade>& facade, 
                   const AdminPanelConfig& config);
    
    /**
     * @brief Conecta ao servidor
     * @param address Endereço do servidor
     * @param port Porta do servidor
     * @param username Nome de usuário
     * @param password Senha
     * @return true se conectado com sucesso
     */
    bool connectToServer(const std::string& address, int port, 
                        const std::string& username, const std::string& password);
    
    /**
     * @brief Desconecta do servidor
     */
    void disconnectFromServer();
    
    /**
     * @brief Verifica se está conectado ao servidor
     * @return true se conectado
     */
    bool isConnected() const;
    
    /**
     * @brief Obtém o status atual do painel
     * @return Status atual
     */
    AdminPanelStatus getStatus() const;
    
    /**
     * @brief Executa um comando administrativo
     * @param command Comando a ser executado
     * @param params Parâmetros do comando
     * @return Resultado da execução
     */
    std::string executeAdminCommand(const std::string& command, 
                                  const std::map<std::string, std::string>& params = {});
    
    /**
     * @brief Executa um comando de IA
     * @param naturalLanguageCommand Comando em linguagem natural
     * @return Resultado da execução
     */
    std::string executeAICommand(const std::string& naturalLanguageCommand);
    
    /**
     * @brief Salva as configurações atuais
     * @param configPath Caminho para o arquivo de configuração (opcional)
     * @return true se salvo com sucesso
     */
    bool saveConfig(const std::string& configPath = "");
    
    /**
     * @brief Carrega configurações
     * @param configPath Caminho para o arquivo de configuração (opcional)
     * @return true se carregado com sucesso
     */
    bool loadConfig(const std::string& configPath = "");
    
    /**
     * @brief Define o tema do painel
     * @param themeName Nome do tema
     */
    void setTheme(const std::string& themeName);
    
    /**
     * @brief Adiciona uma notificação
     * @param title Título da notificação
     * @param message Mensagem da notificação
     * @param level Nível da notificação (info, warning, error, success)
     * @param autoHide Se verdadeiro, a notificação desaparece automaticamente
     */
    void addNotification(const std::string& title, const std::string& message, 
                        const std::string& level = "info", bool autoHide = true);
    
    /**
     * @brief Exibe uma mensagem de status na barra de status
     * @param message Mensagem a ser exibida
     * @param timeout Tempo em milissegundos para exibir a mensagem (0 = permanente)
     */
    void showStatusMessage(const std::string& message, int timeout = 3000);
    
    /**
     * @brief Registra uma ação no log
     * @param action Descrição da ação
     * @param category Categoria da ação
     * @param success Se verdadeiro, a ação foi bem-sucedida
     */
    void logAction(const std::string& action, const std::string& category, bool success = true);
    
    /**
     * @brief Adiciona um comando recente
     * @param command Comando executado
     */
    void addRecentCommand(const std::string& command);
    
    /**
     * @brief Adiciona uma ação rápida aos favoritos
     * @param actionName Nome da ação
     * @param actionCommand Comando da ação
     */
    void addFavoriteQuickAction(const std::string& actionName, const std::string& actionCommand);
    
    /**
     * @brief Remove uma ação rápida dos favoritos
     * @param actionName Nome da ação
     */
    void removeFavoriteQuickAction(const std::string& actionName);
    
    /**
     * @brief Obtém o painel de gerenciamento de jogadores
     * @return Referência ao painel
     */
    panels::PlayerManagerPanel* getPlayerManagerPanel();
    
    /**
     * @brief Obtém o painel de gerenciamento de mundo
     * @return Referência ao painel
     */
    panels::WorldManagerPanel* getWorldManagerPanel();
    
    /**
     * @brief Obtém o painel de gerenciamento de economia
     * @return Referência ao painel
     */
    panels::EconomyManagerPanel* getEconomyManagerPanel();
    
    /**
     * @brief Obtém o painel de gerenciamento de segurança
     * @return Referência ao painel
     */
    panels::SecurityManagerPanel* getSecurityManagerPanel();
    
    /**
     * @brief Obtém o painel de configuração do servidor
     * @return Referência ao painel
     */
    panels::ServerConfigPanel* getServerConfigPanel();
    
    /**
     * @brief Obtém o painel de gerenciamento de conteúdo
     * @return Referência ao painel
     */
    panels::ContentManagerPanel* getContentManagerPanel();
    
    /**
     * @brief Obtém o painel de comunicação
     * @return Referência ao painel
     */
    panels::CommunicationPanel* getCommunicationPanel();
    
    /**
     * @brief Obtém o painel de análise
     * @return Referência ao painel
     */
    panels::AnalyticsPanel* getAnalyticsPanel();
    
public slots:
    /**
     * @brief Slot para reconectar ao servidor
     */
    void reconnectToServer();
    
    /**
     * @brief Slot para mostrar/esconder o assistente de IA
     * @param show Se verdadeiro, mostra o assistente
     */
    void showAIAssistant(bool show);
    
    /**
     * @brief Slot para mostrar/esconder o painel de notificações
     * @param show Se verdadeiro, mostra o painel
     */
    void showNotifications(bool show);
    
    /**
     * @brief Slot para mostrar/esconder o painel de ações rápidas
     * @param show Se verdadeiro, mostra o painel
     */
    void showQuickActions(bool show);
    
    /**
     * @brief Slot para mostrar/esconder o painel de status do servidor
     * @param show Se verdadeiro, mostra o painel
     */
    void showServerStatus(bool show);
    
    /**
     * @brief Slot para alternar o modo tela cheia
     * @param fullscreen Se verdadeiro, ativa o modo tela cheia
     */
    void setFullscreen(bool fullscreen);
    
    /**
     * @brief Slot para mostrar as configurações do painel
     */
    void showSettings();
    
    /**
     * @brief Slot para mostrar informações sobre o painel
     */
    void showAbout();
    
    /**
     * @brief Slot para mostrar a ajuda
     */
    void showHelp();
    
    /**
     * @brief Slot para atualizar o status do servidor
     */
    void updateServerStatus();
    
signals:
    /**
     * @brief Sinal emitido quando o status do painel muda
     * @param status Novo status
     */
    void statusChanged(AdminPanelStatus status);
    
    /**
     * @brief Sinal emitido quando uma notificação é adicionada
     * @param title Título da notificação
     * @param message Mensagem da notificação
     * @param level Nível da notificação
     */
    void notificationAdded(const std::string& title, const std::string& message, const std::string& level);
    
    /**
     * @brief Sinal emitido quando um comando é executado
     * @param command Comando executado
     * @param result Resultado da execução
     * @param success Se verdadeiro, o comando foi bem-sucedido
     */
    void commandExecuted(const std::string& command, const std::string& result, bool success);
    
    /**
     * @brief Sinal emitido quando um assistente de IA responde
     * @param command Comando original
     * @param response Resposta do assistente
     * @param actions Ações sugeridas pelo assistente
     */
    void aiAssistantResponded(const std::string& command, const std::string& response, 
                             const std::vector<std::string>& actions);
    
protected:
    /**
     * @brief Manipulador de evento de fechamento da janela
     * @param event Evento de fechamento
     */
    void closeEvent(QCloseEvent* event) override;
    
    /**
     * @brief Manipulador de evento de redimensionamento
     * @param event Evento de redimensionamento
     */
    void resizeEvent(QResizeEvent* event) override;
    
    /**
     * @brief Manipulador de evento de tecla pressionada
     * @param event Evento de tecla
     */
    void keyPressEvent(QKeyEvent* event) override;
    
    /**
     * @brief Manipulador de evento de ícone da bandeja
     * @param reason Razão da ativação
     */
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    
private:
    // Interface do usuário
    QTabWidget* m_mainTabWidget;
    
    // Abas principais
    panels::PlayerManagerPanel* m_playerManagerPanel;
    panels::WorldManagerPanel* m_worldManagerPanel;
    panels::EconomyManagerPanel* m_economyManagerPanel;
    panels::SecurityManagerPanel* m_securityManagerPanel;
    panels::ServerConfigPanel* m_serverConfigPanel;
    panels::ContentManagerPanel* m_contentManagerPanel;
    panels::CommunicationPanel* m_communicationPanel;
    panels::AnalyticsPanel* m_analyticsPanel;
    
    // Widgets auxiliares
    widgets::AIAssistantWidget* m_aiAssistantWidget;
    widgets::ServerStatusWidget* m_serverStatusWidget;
    widgets::QuickActionsWidget* m_quickActionsWidget;
    widgets::NotificationsWidget* m_notificationsWidget;
    
    // Docas
    QDockWidget* m_aiAssistantDock;
    QDockWidget* m_serverStatusDock;
    QDockWidget* m_quickActionsDock;
    QDockWidget* m_notificationsDock;
    
    // Barras de ferramentas e menus
    QToolBar* m_mainToolBar;
    QToolBar* m_statusToolBar;
    QMenuBar* m_menuBar;
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_toolsMenu;
    QMenu* m_helpMenu;
    
    // Ações
    QAction* m_connectAction;
    QAction* m_disconnectAction;
    QAction* m_saveConfigAction;
    QAction* m_loadConfigAction;
    QAction* m_exitAction;
    QAction* m_settingsAction;
    QAction* m_fullscreenAction;
    QAction* m_showAIAssistantAction;
    QAction* m_showServerStatusAction;
    QAction* m_showQuickActionsAction;
    QAction* m_showNotificationsAction;
    QAction* m_helpAction;
    QAction* m_aboutAction;
    
    // Status
    QStatusBar* m_statusBar;
    QLabel* m_statusLabel;
    QLabel* m_connectionLabel;
    QLabel* m_serverTimeLabel;
    QLabel* m_usernameLabel;
    QLabel* m_versionLabel;
    QProgressBar* m_statusProgressBar;
    
    // Ícone da bandeja
    QSystemTrayIcon* m_trayIcon;
    QMenu* m_trayMenu;
    
    // Timers
    QTimer* m_autoSaveTimer;
    QTimer* m_statusUpdateTimer;
    QTimer* m_reconnectTimer;
    
    // Referência ao facade
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Configuração
    AdminPanelConfig m_config;
    
    // Estado atual
    AdminPanelStatus m_status;
    std::string m_connectedServerAddress;
    int m_connectedServerPort;
    std::string m_connectedUsername;
    bool m_isFullscreen;
    
    // Thread de monitoramento
    std::thread m_monitorThread;
    std::atomic<bool> m_monitorRunning;
    
    // Inicialização da UI
    void createUI();
    void createActions();
    void createMenus();
    void createToolbars();
    void createStatusBar();
    void createDockWidgets();
    void createTrayIcon();
    void connectSignals();
    
    // Atualização de UI
    void updateTitle();
    void updateStatusBar();
    void updateTrayIcon();
    void updateUIForStatus();
    
    // Monitoramento
    void startMonitor();
    void stopMonitor();
    void monitorLoop();
    
    // Tratamento de comandos
    bool parseAICommand(const std::string& naturalLanguageCommand, 
                      std::string& parsedCommand, 
                      std::map<std::string, std::string>& parsedParams);
    
    bool executeDropEventCommand(const std::string& itemName, const std::string& mobName, 
                               const std::string& mapName, int dropRate, int duration);
    
    bool executeBroadcastCommand(const std::string& message, const std::string& color = "", 
                              bool isGlobal = true);
    
    bool executePlayerCommand(const std::string& command, const std::string& playerName, 
                           const std::map<std::string, std::string>& params = {});
    
    bool executeWorldCommand(const std::string& command, 
                          const std::map<std::string, std::string>& params = {});
    
    bool executeServerCommand(const std::string& command, 
                           const std::map<std::string, std::string>& params = {});
};

}}} // namespace wydstudio::ui::admin

#endif // WYDSTUDIO_ADMIN_PANEL_H