#include "../../include/WYDStudioApp.h"
#include "../Agents/AdminAgent.h"
#include "../Agents/GameMasterAgent.h"
#include "../Agents/DeveloperAgent.h"
#include "../GUI/MainWindow.h"
#include <iostream>
#include <fstream>
#include <json/json.hpp> // Usando nlohmann/json para C++

using json = nlohmann::json;

namespace WYDStudio {

// Implementação privada
class WYDStudioApp::Impl {
public:
    Impl() : 
        m_isInitialized(false),
        m_isRunning(false) {
    }
    
    ~Impl() {
        Shutdown();
    }
    
    bool m_isInitialized;
    bool m_isRunning;
    std::string m_gamePath;
    std::string m_modelsPath;
    std::string m_configPath;
    std::unique_ptr<MainWindow> m_mainWindow;
    
    // Configurações
    json m_config;
    
    // Log
    void Log(const std::string& message) {
        std::cout << "[WYDStudio] " << message << std::endl;
        // Adicionar log para arquivo também
    }
};

WYDStudioApp::WYDStudioApp() : 
    m_pImpl(std::make_unique<Impl>()),
    m_serverConnector(std::make_shared<ServerConnector>()),
    m_databaseConnector(std::make_shared<DatabaseConnector>()),
    m_fileSystemConnector(std::make_shared<FileSystemConnector>()) {
}

WYDStudioApp::~WYDStudioApp() {
    // Destrutor automático via unique_ptr
}

bool WYDStudioApp::Initialize(const std::string& configPath) {
    if (m_pImpl->m_isInitialized) {
        return true; // Já inicializado
    }
    
    m_pImpl->Log("Iniciando WYDStudio...");
    
    // Carregar configuração
    if (!configPath.empty()) {
        m_pImpl->m_configPath = configPath;
        if (!LoadConfig(configPath)) {
            m_pImpl->Log("Aviso: Não foi possível carregar configuração. Usando padrões.");
        }
    }
    
    // Inicializar conectores
    if (!InitializeConnectors()) {
        m_pImpl->Log("Erro: Falha ao inicializar conectores.");
        return false;
    }
    
    // Inicializar agentes
    if (!InitializeAgents(m_pImpl->m_modelsPath)) {
        m_pImpl->Log("Erro: Falha ao inicializar agentes IA.");
        return false;
    }
    
    // Inicializar interface gráfica
    m_pImpl->m_mainWindow = std::make_unique<MainWindow>(
        std::shared_ptr<WYDStudioApp>(this)
    );
    
    if (!m_pImpl->m_mainWindow->Initialize()) {
        m_pImpl->Log("Erro: Falha ao inicializar interface gráfica.");
        return false;
    }
    
    m_pImpl->m_isInitialized = true;
    m_pImpl->Log("WYDStudio inicializado com sucesso!");
    
    return true;
}

void WYDStudioApp::Run() {
    if (!m_pImpl->m_isInitialized) {
        m_pImpl->Log("Erro: WYDStudio não foi inicializado.");
        return;
    }
    
    m_pImpl->m_isRunning = true;
    m_pImpl->Log("WYDStudio em execução...");
    
    // Loop principal
    while (m_pImpl->m_isRunning && m_pImpl->m_mainWindow->IsOpen()) {
        m_pImpl->m_mainWindow->ProcessEvents();
        m_pImpl->m_mainWindow->Render();
        
        // Processar outros eventos assíncronos aqui
    }
}

void WYDStudioApp::Shutdown() {
    if (!m_pImpl->m_isRunning) {
        return;
    }
    
    m_pImpl->Log("Encerrando WYDStudio...");
    
    // Salvar configurações antes de sair
    if (!m_pImpl->m_configPath.empty()) {
        SaveConfig(m_pImpl->m_configPath);
    }
    
    // Limpar recursos
    m_agents.clear();
    
    m_pImpl->m_isRunning = false;
    m_pImpl->m_isInitialized = false;
    
    m_pImpl->Log("WYDStudio encerrado.");
}

void WYDStudioApp::ShowDashboard() {
    if (m_pImpl->m_mainWindow) {
        m_pImpl->m_mainWindow->ShowDashboard();
    }
}

void WYDStudioApp::ShowAgentInterface(AgentType type) {
    if (m_pImpl->m_mainWindow) {
        m_pImpl->m_mainWindow->ShowAgentInterface(type);
    }
}

void WYDStudioApp::ShowDataExplorer() {
    if (m_pImpl->m_mainWindow) {
        m_pImpl->m_mainWindow->ShowDataExplorer();
    }
}

void WYDStudioApp::ShowLiveServerStats() {
    if (m_pImpl->m_mainWindow) {
        m_pImpl->m_mainWindow->ShowLiveServerStats();
    }
}

bool WYDStudioApp::ConnectToGameServers() {
    if (!m_serverConnector) {
        return false;
    }
    
    // Obter informações de conexão da configuração
    std::string ip = "127.0.0.1"; // Padrão para localhost
    int port = 8281; // Porta padrão do TMSrv
    std::string admin = "admin";
    std::string password = "admin";
    
    // Se tiver configuração, usar valores dela
    if (!m_pImpl->m_config.empty()) {
        try {
            auto& serverConfig = m_pImpl->m_config["server_connection"];
            ip = serverConfig.value("ip", ip);
            port = serverConfig.value("port", port);
            admin = serverConfig.value("admin_user", admin);
            password = serverConfig.value("admin_password", password);
        }
        catch (const std::exception& e) {
            m_pImpl->Log("Aviso: Erro ao ler configuração de servidor: " + std::string(e.what()));
        }
    }
    
    // Conectar ao TMSrv
    bool success = m_serverConnector->ConnectToTMSrv(ip, port, admin, password);
    
    if (success) {
        m_pImpl->Log("Conectado com sucesso ao servidor de jogo (TMSrv).");
    }
    else {
        m_pImpl->Log("Erro: Falha ao conectar ao servidor de jogo (TMSrv).");
    }
    
    return success;
}

bool WYDStudioApp::ConnectToGameDatabase() {
    if (!m_databaseConnector) {
        return false;
    }
    
    // Obter informações de conexão da configuração
    std::string connectionString = "Driver={SQL Server};Server=localhost;Database=WYD;Uid=sa;Pwd=123456;";
    
    // Se tiver configuração, usar valores dela
    if (!m_pImpl->m_config.empty()) {
        try {
            connectionString = m_pImpl->m_config.value("database_connection_string", connectionString);
        }
        catch (const std::exception& e) {
            m_pImpl->Log("Aviso: Erro ao ler configuração de banco de dados: " + std::string(e.what()));
        }
    }
    
    // Conectar ao banco de dados
    bool success = m_databaseConnector->Connect(connectionString);
    
    if (success) {
        m_pImpl->Log("Conectado com sucesso ao banco de dados do jogo.");
    }
    else {
        m_pImpl->Log("Erro: Falha ao conectar ao banco de dados do jogo.");
    }
    
    return success;
}

void WYDStudioApp::ActivateAgent(AgentType type, const std::string& query) {
    auto agent = GetAgent(type);
    if (!agent) {
        m_pImpl->Log("Erro: Agente não encontrado.");
        return;
    }
    
    std::string response = agent->ProcessQuery(query);
    
    // Exibir resposta na interface
    if (m_pImpl->m_mainWindow) {
        // Implementar exibição de resposta
    }
}

std::string WYDStudioApp::GetAgentResponse(AgentType type, const std::string& query) {
    auto agent = GetAgent(type);
    if (!agent) {
        return "Erro: Agente não disponível.";
    }
    
    return agent->ProcessQuery(query);
}

bool WYDStudioApp::SaveConfig(const std::string& configPath) {
    std::string path = configPath.empty() ? m_pImpl->m_configPath : configPath;
    
    if (path.empty()) {
        m_pImpl->Log("Erro: Caminho de configuração não especificado.");
        return false;
    }
    
    try {
        // Atualizar configuração com valores atuais
        m_pImpl->m_config["game_path"] = m_pImpl->m_gamePath;
        m_pImpl->m_config["models_path"] = m_pImpl->m_modelsPath;
        
        // Salvar para arquivo
        std::ofstream file(path);
        if (!file.is_open()) {
            m_pImpl->Log("Erro: Não foi possível abrir arquivo de configuração para escrita.");
            return false;
        }
        
        file << m_pImpl->m_config.dump(4); // Pretty print com 4 espaços
        file.close();
        
        m_pImpl->Log("Configuração salva com sucesso em: " + path);
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao salvar configuração: " + std::string(e.what()));
        return false;
    }
}

bool WYDStudioApp::LoadConfig(const std::string& configPath) {
    std::string path = configPath.empty() ? m_pImpl->m_configPath : configPath;
    
    if (path.empty()) {
        m_pImpl->Log("Erro: Caminho de configuração não especificado.");
        return false;
    }
    
    try {
        // Tentar abrir o arquivo
        std::ifstream file(path);
        if (!file.is_open()) {
            m_pImpl->Log("Aviso: Arquivo de configuração não encontrado. Será criado na inicialização.");
            return false;
        }
        
        // Carregar JSON
        file >> m_pImpl->m_config;
        file.close();
        
        // Extrair valores importantes
        m_pImpl->m_gamePath = m_pImpl->m_config.value("game_path", "");
        m_pImpl->m_modelsPath = m_pImpl->m_config.value("models_path", "");
        
        m_pImpl->Log("Configuração carregada com sucesso de: " + path);
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao carregar configuração: " + std::string(e.what()));
        return false;
    }
}

bool WYDStudioApp::SetGamePath(const std::string& gamePath) {
    // Verificar se o caminho existe
    // Simplificado para este exemplo
    m_pImpl->m_gamePath = gamePath;
    
    // Informar o conector de sistema de arquivos
    if (m_fileSystemConnector) {
        return m_fileSystemConnector->SetGameRootPath(gamePath);
    }
    
    return true;
}

bool WYDStudioApp::SetModelsPath(const std::string& modelsPath) {
    // Verificar se o caminho existe
    // Simplificado para este exemplo
    m_pImpl->m_modelsPath = modelsPath;
    
    // Reinicializar agentes se necessário
    if (!m_agents.empty()) {
        return InitializeAgents(modelsPath);
    }
    
    return true;
}

std::shared_ptr<BaseAgent> WYDStudioApp::GetAgent(AgentType type) {
    auto it = m_agents.find(type);
    if (it != m_agents.end()) {
        return it->second;
    }
    
    return nullptr;
}

bool WYDStudioApp::IsReady() const {
    return m_pImpl->m_isInitialized && 
           m_serverConnector && m_serverConnector->IsConnected() &&
           m_databaseConnector && m_databaseConnector->IsConnected();
}

std::map<std::string, std::string> WYDStudioApp::GetStatusReport() const {
    std::map<std::string, std::string> status;
    
    status["initialized"] = m_pImpl->m_isInitialized ? "Sim" : "Não";
    status["running"] = m_pImpl->m_isRunning ? "Sim" : "Não";
    status["server_connected"] = (m_serverConnector && m_serverConnector->IsConnected()) ? "Sim" : "Não";
    status["database_connected"] = (m_databaseConnector && m_databaseConnector->IsConnected()) ? "Sim" : "Não";
    status["game_path"] = m_pImpl->m_gamePath;
    status["models_path"] = m_pImpl->m_modelsPath;
    status["agents_loaded"] = std::to_string(m_agents.size());
    
    return status;
}

bool WYDStudioApp::InitializeAgents(const std::string& modelsPath) {
    m_pImpl->Log("Inicializando agentes IA...");
    
    try {
        // Limpar agentes existentes
        m_agents.clear();
        
        // Criar e inicializar agentes
        auto adminAgent = std::make_shared<AdminAgent>(m_serverConnector);
        if (!adminAgent->Initialize(modelsPath)) {
            m_pImpl->Log("Erro: Falha ao inicializar AdminAgent.");
            return false;
        }
        m_agents[AgentType::ADMIN] = adminAgent;
        
        auto gmAgent = std::make_shared<GameMasterAgent>(m_serverConnector);
        if (!gmAgent->Initialize(modelsPath)) {
            m_pImpl->Log("Erro: Falha ao inicializar GameMasterAgent.");
            return false;
        }
        m_agents[AgentType::GM] = gmAgent;
        
        auto devAgent = std::make_shared<DeveloperAgent>(m_serverConnector, m_fileSystemConnector);
        if (!devAgent->Initialize(modelsPath)) {
            m_pImpl->Log("Erro: Falha ao inicializar DeveloperAgent.");
            return false;
        }
        m_agents[AgentType::DEV] = devAgent;
        
        m_pImpl->Log("Agentes IA inicializados com sucesso.");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao inicializar agentes: " + std::string(e.what()));
        return false;
    }
}

bool WYDStudioApp::InitializeConnectors() {
    m_pImpl->Log("Inicializando conectores...");
    
    try {
        // Verificar se os conectores já foram criados
        if (!m_serverConnector) {
            m_serverConnector = std::make_shared<ServerConnector>();
        }
        
        if (!m_databaseConnector) {
            m_databaseConnector = std::make_shared<DatabaseConnector>();
        }
        
        if (!m_fileSystemConnector) {
            m_fileSystemConnector = std::make_shared<FileSystemConnector>();
        }
        
        // Inicializar FileSystemConnector com o caminho do jogo
        if (!m_pImpl->m_gamePath.empty()) {
            if (!m_fileSystemConnector->SetGameRootPath(m_pImpl->m_gamePath)) {
                m_pImpl->Log("Aviso: Falha ao definir caminho do jogo para o FileSystemConnector.");
            }
        }
        
        m_pImpl->Log("Conectores inicializados com sucesso.");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao inicializar conectores: " + std::string(e.what()));
        return false;
    }
}

} // namespace WYDStudio