#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QSettings>
#include "monitoring/ServerMonitor.h"
#include "ui/MonitoringPanel.h"
#include "../ai/AIAssistantWidget.h"

namespace wydbr {
namespace ui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // Slots do Menu Arquivo
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onExportData();
    void onImportData();

    // Slots do Menu Servidor
    void onStartServer();
    void onStopServer();
    void onRestartServer();
    void onConfigureServer();

    // Slots do Menu Ferramentas
    void onOpenItemEditor();
    void onOpenMobEditor();
    void onOpenMapEditor();
    void onOpenSkillEditor();
    void onOpenQuestEditor();

    // Slots do Menu Administração
    void onOpenPlayerManager();
    void onOpenEconomyManager();
    void onOpenEventManager();
    void onOpenBanManager();

    // Slots do Menu IA
    void onOpenAITrainer();
    void onOpenBotManager();
    void onConfigureAI();

    // Slots do Menu Ajuda
    void onShowDocumentation();
    void onCheckUpdates();
    void onAbout();

private:
    void setupUI();
    void createMenus();
    void createToolbars();
    void createDockWidgets();
    void createStatusBar();
    void loadSettings();
    void saveSettings();
    void setupTheme();

    // Widgets principais
    QStackedWidget* m_centralStack;
    QDockWidget* m_serverMonitorDock;
    QDockWidget* m_aiAssistantDock;
    QDockWidget* m_consoleDock;
    QDockWidget* m_playerListDock;

    // Menus
    QMenu* m_fileMenu;
    QMenu* m_serverMenu;
    QMenu* m_toolsMenu;
    QMenu* m_adminMenu;
    QMenu* m_aiMenu;
    QMenu* m_helpMenu;

    // Toolbars
    QToolBar* m_mainToolBar;
    QToolBar* m_serverToolBar;
    QToolBar* m_editToolBar;

    // Configurações
    QSettings m_settings;

    // Monitor do servidor
    std::shared_ptr<monitoring::ServerMonitor> m_serverMonitor;
    MonitoringPanel* m_monitoringPanel;
    ai::AIAssistantWidget* m_aiAssistant;
};

} // namespace ui
} // namespace wydbr
