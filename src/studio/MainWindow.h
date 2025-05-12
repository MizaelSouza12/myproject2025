#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <memory>

#include "ai/AIKnowledgeBase.h"
#include "ai/CommandTokenizer.h"
#include "ai/ContextAnalyzer.h"

namespace wydbr {
namespace studio {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // Slots do Menu Arquivo
    void onNewServer();
    void onOpenServer();
    void onSaveConfig();
    void onBackupServer();
    void onRestoreBackup();
    void onExit();

    // Slots do Menu Ferramentas
    void onConfigureServer();
    void onManageUsers();
    void onManageItems();
    void onManageWorld();
    void onManageEvents();
    void onViewLogs();
    void onMonitorServer();

    // Slots do Menu IA
    void onConfigureAI();
    void onViewPatterns();
    void onTrainAI();
    void onResetAI();

    // Slots do Menu Ajuda
    void onViewHelp();
    void onAbout();

private:
    void setupUI();
    void createMenus();
    void createToolbars();
    void createDockWidgets();
    void createStatusBar();
    void setupConnections();

    // Componentes da UI
    QTabWidget* m_tabWidget;
    QMenuBar* m_menuBar;
    QToolBar* m_mainToolBar;
    QStatusBar* m_statusBar;
    QDockWidget* m_serverDock;
    QDockWidget* m_playersDock;
    QDockWidget* m_logDock;
    QDockWidget* m_aiDock;

    // Sistema de IA
    std::unique_ptr<ai::AIKnowledgeBase> m_aiKnowledge;
    std::unique_ptr<ai::CommandTokenizer> m_tokenizer;
    std::unique_ptr<ai::ContextAnalyzer> m_analyzer;
};

} // namespace studio
} // namespace wydbr 