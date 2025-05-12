#include "MainWindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>

namespace wydbr {
namespace studio {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Configuração inicial da janela
    setWindowTitle("WYD Studio - Administração Avançada");
    resize(1280, 720);

    // Centraliza a janela
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QApplication::desktop()->availableGeometry()
        )
    );

    // Inicializa o sistema de IA
    m_aiKnowledge = std::make_unique<ai::AIKnowledgeBase>();
    m_tokenizer = std::make_unique<ai::CommandTokenizer>();
    m_analyzer = std::make_unique<ai::ContextAnalyzer>();

    // Configura a interface
    setupUI();
    createMenus();
    createToolbars();
    createDockWidgets();
    createStatusBar();
    setupConnections();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    // Widget central com abas
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    // Adiciona abas principais
    m_tabWidget->addTab(new QWidget(), "Visão Geral");
    m_tabWidget->addTab(new QWidget(), "Jogadores");
    m_tabWidget->addTab(new QWidget(), "Items");
    m_tabWidget->addTab(new QWidget(), "Mundo");
    m_tabWidget->addTab(new QWidget(), "Eventos");
    m_tabWidget->addTab(new QWidget(), "Logs");
    m_tabWidget->addTab(new QWidget(), "Configurações");
}

void MainWindow::createMenus() {
    // Menu Arquivo
    auto fileMenu = menuBar()->addMenu("&Arquivo");
    fileMenu->addAction("&Novo Servidor", this, &MainWindow::onNewServer);
    fileMenu->addAction("&Abrir Servidor", this, &MainWindow::onOpenServer);
    fileMenu->addSeparator();
    fileMenu->addAction("&Salvar Configurações", this, &MainWindow::onSaveConfig);
    fileMenu->addSeparator();
    fileMenu->addAction("&Backup", this, &MainWindow::onBackupServer);
    fileMenu->addAction("&Restaurar", this, &MainWindow::onRestoreBackup);
    fileMenu->addSeparator();
    fileMenu->addAction("Sai&r", this, &MainWindow::onExit);

    // Menu Ferramentas
    auto toolsMenu = menuBar()->addMenu("&Ferramentas");
    toolsMenu->addAction("&Configurar Servidor", this, &MainWindow::onConfigureServer);
    toolsMenu->addSeparator();
    toolsMenu->addAction("Gerenciar &Usuários", this, &MainWindow::onManageUsers);
    toolsMenu->addAction("Gerenciar &Items", this, &MainWindow::onManageItems);
    toolsMenu->addAction("Gerenciar &Mundo", this, &MainWindow::onManageWorld);
    toolsMenu->addAction("Gerenciar &Eventos", this, &MainWindow::onManageEvents);
    toolsMenu->addSeparator();
    toolsMenu->addAction("&Visualizar Logs", this, &MainWindow::onViewLogs);
    toolsMenu->addAction("&Monitorar Servidor", this, &MainWindow::onMonitorServer);

    // Menu IA
    auto aiMenu = menuBar()->addMenu("&IA");
    aiMenu->addAction("&Configurar IA", this, &MainWindow::onConfigureAI);
    aiMenu->addAction("&Visualizar Padrões", this, &MainWindow::onViewPatterns);
    aiMenu->addAction("&Treinar IA", this, &MainWindow::onTrainAI);
    aiMenu->addSeparator();
    aiMenu->addAction("&Resetar IA", this, &MainWindow::onResetAI);

    // Menu Ajuda
    auto helpMenu = menuBar()->addMenu("A&juda");
    helpMenu->addAction("&Ajuda", this, &MainWindow::onViewHelp);
    helpMenu->addAction("&Sobre", this, &MainWindow::onAbout);
}

void MainWindow::createToolbars() {
    m_mainToolBar = addToolBar("Principal");
    m_mainToolBar->addAction("Novo Servidor", this, &MainWindow::onNewServer);
    m_mainToolBar->addAction("Abrir Servidor", this, &MainWindow::onOpenServer);
    m_mainToolBar->addAction("Salvar", this, &MainWindow::onSaveConfig);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction("Backup", this, &MainWindow::onBackupServer);
    m_mainToolBar->addAction("Restaurar", this, &MainWindow::onRestoreBackup);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction("Configurar", this, &MainWindow::onConfigureServer);
    m_mainToolBar->addAction("Monitorar", this, &MainWindow::onMonitorServer);
}

void MainWindow::createDockWidgets() {
    // Dock do Servidor
    m_serverDock = new QDockWidget("Status do Servidor", this);
    m_serverDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_serverDock);

    // Dock dos Jogadores
    m_playersDock = new QDockWidget("Jogadores Online", this);
    m_playersDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_playersDock);

    // Dock de Logs
    m_logDock = new QDockWidget("Log em Tempo Real", this);
    m_logDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, m_logDock);

    // Dock da IA
    m_aiDock = new QDockWidget("Assistente IA", this);
    m_aiDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_aiDock);
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage("Pronto");
}

void MainWindow::setupConnections() {
    // TODO: Implementar conexões de sinais e slots
}

// Slots do Menu Arquivo
void MainWindow::onNewServer() {
    QMessageBox::information(this, "Novo Servidor", "Criar novo servidor");
}

void MainWindow::onOpenServer() {
    QString path = QFileDialog::getExistingDirectory(this, "Abrir Servidor");
    if (!path.isEmpty()) {
        QMessageBox::information(this, "Abrir Servidor", "Abrindo servidor em: " + path);
    }
}

void MainWindow::onSaveConfig() {
    QMessageBox::information(this, "Salvar", "Salvando configurações");
}

void MainWindow::onBackupServer() {
    QMessageBox::information(this, "Backup", "Iniciando backup do servidor");
}

void MainWindow::onRestoreBackup() {
    QMessageBox::information(this, "Restaurar", "Restaurando backup");
}

void MainWindow::onExit() {
    close();
}

// Slots do Menu Ferramentas
void MainWindow::onConfigureServer() {
    QMessageBox::information(this, "Configurar", "Configurando servidor");
}

void MainWindow::onManageUsers() {
    QMessageBox::information(this, "Usuários", "Gerenciando usuários");
}

void MainWindow::onManageItems() {
    QMessageBox::information(this, "Items", "Gerenciando items");
}

void MainWindow::onManageWorld() {
    QMessageBox::information(this, "Mundo", "Gerenciando mundo");
}

void MainWindow::onManageEvents() {
    QMessageBox::information(this, "Eventos", "Gerenciando eventos");
}

void MainWindow::onViewLogs() {
    QMessageBox::information(this, "Logs", "Visualizando logs");
}

void MainWindow::onMonitorServer() {
    QMessageBox::information(this, "Monitor", "Monitorando servidor");
}

// Slots do Menu IA
void MainWindow::onConfigureAI() {
    QMessageBox::information(this, "IA", "Configurando IA");
}

void MainWindow::onViewPatterns() {
    QMessageBox::information(this, "Padrões", "Visualizando padrões de IA");
}

void MainWindow::onTrainAI() {
    QMessageBox::information(this, "Treinar", "Treinando IA");
}

void MainWindow::onResetAI() {
    QMessageBox::information(this, "Reset", "Resetando IA");
}

// Slots do Menu Ajuda
void MainWindow::onViewHelp() {
    QMessageBox::information(this, "Ajuda", "Visualizando ajuda");
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "Sobre",
        "WYD Studio - Sistema Avançado de Administração\n\n"
        "Versão: 2.0.0\n"
        "Desenvolvido por: WYDBR Team\n\n"
        "Sistema integrado de administração com IA para servidores WYD.");
}

} // namespace studio
} // namespace wydbr 