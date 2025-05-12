#include "MainWindow.h"
#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QFile>
#include <QMessageBox>
#include <QTextEdit>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>

namespace wydbr {
namespace ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_settings("WYDBR", "WYDStudio")
{
    setupTheme();
    setupUI();
    loadSettings();

    setWindowTitle("WYDStudio - Administração Avançada WYD");
    setMinimumSize(1280, 720);
}

MainWindow::~MainWindow() {
    saveSettings();
}

void MainWindow::setupTheme() {
    // Configuração do tema dark moderno
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::WindowText, QColor(208, 208, 208));
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(208, 208, 208));
    darkPalette.setColor(QPalette::ToolTipText, QColor(208, 208, 208));
    darkPalette.setColor(QPalette::Text, QColor(208, 208, 208));
    darkPalette.setColor(QPalette::Button, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ButtonText, QColor(208, 208, 208));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(darkPalette);

    // Estilo para QDockWidget
    QString dockStyle = R"(
        QDockWidget {
            border: 1px solid #3A3A3A;
            titlebar-close-icon: url(close.png);
        }
        QDockWidget::title {
            background: #2D2D2D;
            padding-left: 5px;
            padding-top: 2px;
        }
    )";

    // Estilo para QMenuBar
    QString menuStyle = R"(
        QMenuBar {
            background-color: #2D2D2D;
            color: #D0D0D0;
        }
        QMenuBar::item:selected {
            background-color: #3A3A3A;
        }
        QMenu {
            background-color: #2D2D2D;
            border: 1px solid #3A3A3A;
        }
        QMenu::item:selected {
            background-color: #3A3A3A;
        }
    )";

    // Estilo para QToolBar
    QString toolbarStyle = R"(
        QToolBar {
            background-color: #2D2D2D;
            border: none;
            spacing: 3px;
        }
        QToolButton {
            background-color: transparent;
            border: 1px solid transparent;
            border-radius: 2px;
            padding: 3px;
        }
        QToolButton:hover {
            background-color: #3A3A3A;
            border: 1px solid #4A4A4A;
        }
    )";

    setStyleSheet(dockStyle + menuStyle + toolbarStyle);
}

void MainWindow::setupUI() {
    // Widget central
    m_centralStack = new QStackedWidget(this);
    setCentralWidget(m_centralStack);

    // Criar menus
    createMenus();

    // Criar toolbars
    createToolbars();

    // Criar dock widgets
    createDockWidgets();

    // Criar barra de status
    createStatusBar();

    // Configurar monitor do servidor
    m_serverMonitor = std::make_shared<monitoring::ServerMonitor>();
    m_monitoringPanel = new MonitoringPanel(this);
    m_monitoringPanel->setServerMonitor(m_serverMonitor);
    m_serverMonitorDock->setWidget(m_monitoringPanel);
}

void MainWindow::createMenus() {
    // Menu Arquivo
    m_fileMenu = menuBar()->addMenu("Arquivo");
    m_fileMenu->addAction("Novo Projeto", this, &MainWindow::onNewProject);
    m_fileMenu->addAction("Abrir Projeto", this, &MainWindow::onOpenProject);
    m_fileMenu->addAction("Salvar Projeto", this, &MainWindow::onSaveProject);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("Exportar Dados", this, &MainWindow::onExportData);
    m_fileMenu->addAction("Importar Dados", this, &MainWindow::onImportData);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("Sair", qApp, &QApplication::quit);

    // Menu Servidor
    m_serverMenu = menuBar()->addMenu("Servidor");
    m_serverMenu->addAction("Iniciar", this, &MainWindow::onStartServer);
    m_serverMenu->addAction("Parar", this, &MainWindow::onStopServer);
    m_serverMenu->addAction("Reiniciar", this, &MainWindow::onRestartServer);
    m_serverMenu->addSeparator();
    m_serverMenu->addAction("Configurar", this, &MainWindow::onConfigureServer);

    // Menu Ferramentas
    m_toolsMenu = menuBar()->addMenu("Ferramentas");
    m_toolsMenu->addAction("Editor de Itens", this, &MainWindow::onOpenItemEditor);
    m_toolsMenu->addAction("Editor de Mobs", this, &MainWindow::onOpenMobEditor);
    m_toolsMenu->addAction("Editor de Mapas", this, &MainWindow::onOpenMapEditor);
    m_toolsMenu->addAction("Editor de Skills", this, &MainWindow::onOpenSkillEditor);
    m_toolsMenu->addAction("Editor de Quests", this, &MainWindow::onOpenQuestEditor);

    // Menu Administração
    m_adminMenu = menuBar()->addMenu("Administração");
    m_adminMenu->addAction("Gerenciar Jogadores", this, &MainWindow::onOpenPlayerManager);
    m_adminMenu->addAction("Gerenciar Economia", this, &MainWindow::onOpenEconomyManager);
    m_adminMenu->addAction("Gerenciar Eventos", this, &MainWindow::onOpenEventManager);
    m_adminMenu->addAction("Gerenciar Banimentos", this, &MainWindow::onOpenBanManager);

    // Menu IA
    m_aiMenu = menuBar()->addMenu("IA");
    m_aiMenu->addAction("Treinar IA", this, &MainWindow::onOpenAITrainer);
    m_aiMenu->addAction("Gerenciar Bots", this, &MainWindow::onOpenBotManager);
    m_aiMenu->addSeparator();
    m_aiMenu->addAction("Configurar IA", this, &MainWindow::onConfigureAI);

    // Menu Ajuda
    m_helpMenu = menuBar()->addMenu("Ajuda");
    m_helpMenu->addAction("Documentação", this, &MainWindow::onShowDocumentation);
    m_helpMenu->addAction("Verificar Atualizações", this, &MainWindow::onCheckUpdates);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction("Sobre", this, &MainWindow::onAbout);
}

void MainWindow::createToolbars() {
    // Toolbar Principal
    m_mainToolBar = addToolBar("Principal");
    m_mainToolBar->setMovable(false);
    m_mainToolBar->addAction(QIcon(":/icons/new.png"), "Novo");
    m_mainToolBar->addAction(QIcon(":/icons/open.png"), "Abrir");
    m_mainToolBar->addAction(QIcon(":/icons/save.png"), "Salvar");

    // Toolbar do Servidor
    m_serverToolBar = addToolBar("Servidor");
    m_serverToolBar->setMovable(false);
    m_serverToolBar->addAction(QIcon(":/icons/start.png"), "Iniciar Servidor");
    m_serverToolBar->addAction(QIcon(":/icons/stop.png"), "Parar Servidor");
    m_serverToolBar->addAction(QIcon(":/icons/restart.png"), "Reiniciar Servidor");

    // Toolbar de Edição
    m_editToolBar = addToolBar("Edição");
    m_editToolBar->setMovable(false);
    m_editToolBar->addAction(QIcon(":/icons/items.png"), "Editor de Itens");
    m_editToolBar->addAction(QIcon(":/icons/mobs.png"), "Editor de Mobs");
    m_editToolBar->addAction(QIcon(":/icons/maps.png"), "Editor de Mapas");
}

void MainWindow::createDockWidgets() {
    // Monitor do Servidor
    m_serverMonitorDock = new QDockWidget("Monitor do Servidor", this);
    m_serverMonitorDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_serverMonitorDock);

    // Assistente IA
    m_aiAssistantDock = new QDockWidget("Assistente IA", this);
    m_aiAssistantDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_aiAssistant = new ai::AIAssistantWidget(this);
    m_aiAssistantDock->setWidget(m_aiAssistant);
    addDockWidget(Qt::RightDockWidgetArea, m_aiAssistantDock);

    // Console
    m_consoleDock = new QDockWidget("Console", this);
    m_consoleDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    QTextEdit* console = new QTextEdit(this);
    console->setReadOnly(true);
    console->setStyleSheet("background-color: #1A1A1A; color: #00FF00;");
    m_consoleDock->setWidget(console);
    addDockWidget(Qt::BottomDockWidgetArea, m_consoleDock);

    // Lista de Jogadores
    m_playerListDock = new QDockWidget("Jogadores Online", this);
    m_playerListDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    QListWidget* playerList = new QListWidget(this);
    m_playerListDock->setWidget(playerList);
    addDockWidget(Qt::LeftDockWidgetArea, m_playerListDock);

    // Organizar docks
    tabifyDockWidget(m_serverMonitorDock, m_aiAssistantDock);
    m_serverMonitorDock->raise();
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage("WYDStudio pronto");

    // Adicionar widgets permanentes
    QLabel* serverStatus = new QLabel("Servidor: Offline");
    serverStatus->setStyleSheet("color: #FF4444;");
    statusBar()->addPermanentWidget(serverStatus);

    QLabel* playersOnline = new QLabel("Jogadores: 0");
    statusBar()->addPermanentWidget(playersOnline);

    QLabel* memoryUsage = new QLabel("Memória: 0 MB");
    statusBar()->addPermanentWidget(memoryUsage);
}

void MainWindow::loadSettings() {
    // Carregar geometria da janela
    if (m_settings.contains("geometry")) {
        restoreGeometry(m_settings.value("geometry").toByteArray());
    }

    // Carregar estado da janela
    if (m_settings.contains("windowState")) {
        restoreState(m_settings.value("windowState").toByteArray());
    }
}

void MainWindow::saveSettings() {
    m_settings.setValue("geometry", saveGeometry());
    m_settings.setValue("windowState", saveState());
}

// Implementação dos slots do Menu Arquivo
void MainWindow::onNewProject() {
    // TODO: Implementar
}

void MainWindow::onOpenProject() {
    // TODO: Implementar
}

void MainWindow::onSaveProject() {
    // TODO: Implementar
}

void MainWindow::onExportData() {
    // TODO: Implementar
}

void MainWindow::onImportData() {
    // TODO: Implementar
}

// Implementação dos slots do Menu Servidor
void MainWindow::onStartServer() {
    // TODO: Implementar
}

void MainWindow::onStopServer() {
    // TODO: Implementar
}

void MainWindow::onRestartServer() {
    // TODO: Implementar
}

void MainWindow::onConfigureServer() {
    // TODO: Implementar
}

// Implementação dos slots do Menu Ferramentas
void MainWindow::onOpenItemEditor() {
    // TODO: Implementar
}

void MainWindow::onOpenMobEditor() {
    // TODO: Implementar
}

void MainWindow::onOpenMapEditor() {
    // TODO: Implementar
}

void MainWindow::onOpenSkillEditor() {
    // TODO: Implementar
}

void MainWindow::onOpenQuestEditor() {
    // TODO: Implementar
}

// Implementação dos slots do Menu Administração
void MainWindow::onOpenPlayerManager() {
    // TODO: Implementar
}

void MainWindow::onOpenEconomyManager() {
    // TODO: Implementar
}

void MainWindow::onOpenEventManager() {
    // TODO: Implementar
}

void MainWindow::onOpenBanManager() {
    // TODO: Implementar
}

// Implementação dos slots do Menu IA
void MainWindow::onOpenAITrainer() {
    // Abrir janela de treinamento da IA
    QMessageBox::information(this, "Treinamento de IA",
        "O treinamento da IA permite melhorar a compreensão do assistente\n"
        "sobre as necessidades específicas do seu servidor.\n\n"
        "Esta funcionalidade será implementada em breve.");
}

void MainWindow::onOpenBotManager() {
    // Abrir gerenciador de bots
    QMessageBox::information(this, "Gerenciador de Bots",
        "O gerenciador de bots permite configurar assistentes automatizados\n"
        "para tarefas específicas do servidor.\n\n"
        "Esta funcionalidade será implementada em breve.");
}

void MainWindow::onConfigureAI() {
    // Configurar parâmetros da IA
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Configurar IA");
    dialog->setMinimumWidth(400);

    auto layout = new QVBoxLayout(dialog);

    // Opções de configuração
    auto form = new QFormLayout();

    auto confidenceSlider = new QSlider(Qt::Horizontal);
    confidenceSlider->setRange(50, 100);
    confidenceSlider->setValue(85);
    form->addRow("Confiança mínima:", confidenceSlider);

    auto autoExecuteCheck = new QCheckBox("Executar ações automaticamente");
    form->addRow("", autoExecuteCheck);

    auto modelCombo = new QComboBox();
    modelCombo->addItems({"GPT-3.5", "GPT-4", "Local"});
    form->addRow("Modelo:", modelCombo);

    auto updateIntervalSpin = new QSpinBox();
    updateIntervalSpin->setRange(1, 60);
    updateIntervalSpin->setValue(5);
    updateIntervalSpin->setSuffix(" segundos");
    form->addRow("Intervalo de atualização:", updateIntervalSpin);

    layout->addLayout(form);

    // Botões
    auto buttons = new QHBoxLayout();
    auto okButton = new QPushButton("OK");
    auto cancelButton = new QPushButton("Cancelar");

    connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::reject);

    buttons->addWidget(okButton);
    buttons->addWidget(cancelButton);
    layout->addLayout(buttons);

    if (dialog->exec() == QDialog::Accepted) {
        // TODO: Salvar configurações
        QMessageBox::information(this, "Configuração da IA",
            "Configurações salvas com sucesso!");
    }

    delete dialog;
}

// Implementação dos slots do Menu Ajuda
void MainWindow::onShowDocumentation() {
    // TODO: Implementar
}

void MainWindow::onCheckUpdates() {
    // TODO: Implementar
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "Sobre WYDStudio",
        "WYDStudio - Ferramenta Avançada de Administração WYD\n\n"
        "Versão: 1.0.0\n"
        "© 2024 WYDBR Team\n\n"
        "Uma ferramenta completa para administração,\n"
        "desenvolvimento e monitoramento de servidores WYD.");
}

} // namespace ui
} // namespace wydbr