#pragma once

#include "BaseAgent.h"
#include "ServerConnector.h"
#include "DatabaseConnector.h"
#include "FileSystemConnector.h"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace WYDStudio {

/**
 * @class WYDStudioApp
 * @brief Classe principal do aplicativo WYDStudio
 * 
 * Esta classe gerencia todos os componentes do WYDStudio, incluindo
 * os agentes de IA, conectores de servidor/banco de dados e interface gráfica.
 */
class WYDStudioApp {
public:
    /**
     * @brief Construtor padrão
     */
    WYDStudioApp();
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~WYDStudioApp();
    
    /**
     * @brief Inicializa o aplicativo
     * @param configPath Caminho para o arquivo de configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& configPath = "");
    
    /**
     * @brief Executa o loop principal do aplicativo
     */
    void Run();
    
    /**
     * @brief Finaliza o aplicativo
     */
    void Shutdown();
    
    /**
     * @brief Exibe o painel principal
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
     * @brief Conecta aos servidores do jogo
     * @return true se conectado com sucesso
     */
    bool ConnectToGameServers();
    
    /**
     * @brief Conecta ao banco de dados do jogo
     * @return true se conectado com sucesso
     */
    bool ConnectToGameDatabase();
    
    /**
     * @brief Ativa um agente específico para processar uma consulta
     * @param type Tipo do agente
     * @param query Consulta em linguagem natural
     */
    void ActivateAgent(AgentType type, const std::string& query);
    
    /**
     * @brief Obtém a resposta de um agente
     * @param type Tipo do agente
     * @param query Consulta em linguagem natural
     * @return Resposta do agente
     */
    std::string GetAgentResponse(AgentType type, const std::string& query);
    
    /**
     * @brief Salva as configurações atuais
     * @param configPath Caminho para o arquivo de configuração
     * @return true se salvo com sucesso
     */
    bool SaveConfig(const std::string& configPath = "");
    
    /**
     * @brief Carrega configurações
     * @param configPath Caminho para o arquivo de configuração
     * @return true se carregado com sucesso
     */
    bool LoadConfig(const std::string& configPath = "");
    
    /**
     * @brief Define o caminho para os arquivos do jogo
     * @param gamePath Caminho para o diretório do jogo
     * @return true se o caminho é válido
     */
    bool SetGamePath(const std::string& gamePath);
    
    /**
     * @brief Define o caminho para os modelos de IA
     * @param modelsPath Caminho para os modelos de IA
     * @return true se o caminho é válido
     */
    bool SetModelsPath(const std::string& modelsPath);
    
    /**
     * @brief Obtém um agente pelo tipo
     * @param type Tipo do agente
     * @return Ponteiro para o agente
     */
    std::shared_ptr<BaseAgent> GetAgent(AgentType type);
    
    /**
     * @brief Verifica se todos os componentes estão prontos
     * @return true se todos os componentes estão inicializados e conectados
     */
    bool IsReady() const;
    
    /**
     * @brief Obtém relatório de status de todos os componentes
     * @return Mapa com status dos componentes
     */
    std::map<std::string, std::string> GetStatusReport() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
    
    // Conectores principais
    std::shared_ptr<ServerConnector> m_serverConnector;
    std::shared_ptr<DatabaseConnector> m_databaseConnector;
    std::shared_ptr<FileSystemConnector> m_fileSystemConnector;
    
    // Agentes de IA
    std::map<AgentType, std::shared_ptr<BaseAgent>> m_agents;
    
    // Inicializa todos os agentes
    bool InitializeAgents(const std::string& modelsPath);
    
    // Inicializa conectores
    bool InitializeConnectors();
};

} // namespace WYDStudio