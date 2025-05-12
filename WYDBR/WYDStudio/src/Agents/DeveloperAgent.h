#pragma once

#include "../../include/BaseAgent.h"
#include "../../include/GameStructs.h"
#include "../../include/ServerConnector.h"
#include "../../include/FileSystemConnector.h"
#include <memory>

namespace WYDStudio {

/**
 * @class DeveloperAgent
 * @brief Agente de IA para funções de desenvolvedor
 * 
 * Este agente é responsável por funções técnicas como correção de bugs,
 * implementação de recursos e otimização de desempenho.
 */
class DeveloperAgent : public BaseAgent {
public:
    /**
     * @brief Construtor
     * @param serverConnector Ponteiro para o conector de servidor
     * @param fileSystemConnector Ponteiro para o conector de sistema de arquivos
     */
    DeveloperAgent(std::shared_ptr<ServerConnector> serverConnector,
                  std::shared_ptr<FileSystemConnector> fileSystemConnector);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~DeveloperAgent() override;
    
    /**
     * @brief Inicializa o agente com modelos de IA
     * @param modelsPath Caminho para os modelos de IA
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const std::string& modelsPath) override;
    
    /**
     * @brief Processa uma consulta e retorna uma resposta
     * @param query Consulta em linguagem natural
     * @return Resposta gerada pelo agente
     */
    virtual std::string ProcessQuery(const std::string& query) override;
    
    /**
     * @brief Executa uma ação com base na intenção do usuário
     * @param intent Intenção interpretada da consulta do usuário
     * @param params Parâmetros extraídos da consulta
     * @return true se a ação foi executada com sucesso
     */
    virtual bool ExecuteAction(const std::string& intent, 
                              const std::vector<std::string>& params) override;
    
    /**
     * @brief Verifica se o agente pode lidar com determinada intenção
     * @param intent A intenção a ser verificada
     * @return true se o agente pode lidar com a intenção
     */
    virtual bool CanHandleIntent(const std::string& intent) const override;
    
    /**
     * @brief Obtém o tipo do agente
     * @return Tipo do agente
     */
    virtual AgentType GetAgentType() const override;
    
    /**
     * @brief Processa uma solicitação de correção de bug
     * @param request Solicitação em linguagem natural
     * @return true se a solicitação foi processada com sucesso
     */
    bool HandleBugFixRequest(const std::string& request);
    
    /**
     * @brief Processa uma solicitação de implementação de recurso
     * @param request Solicitação em linguagem natural
     * @return true se a solicitação foi processada com sucesso
     */
    bool HandleFeatureImplementationRequest(const std::string& request);
    
    /**
     * @brief Processa uma solicitação de otimização de desempenho
     * @param request Solicitação em linguagem natural
     * @return true se a solicitação foi processada com sucesso
     */
    bool HandlePerformanceOptimizationRequest(const std::string& request);
    
    /**
     * @brief Implanta uma correção rápida
     * @param fixPath Caminho para o arquivo de correção
     * @return true se a implantação foi bem-sucedida
     */
    bool DeployHotfix(const std::string& fixPath);
    
    /**
     * @brief Analisa o desempenho do servidor
     * @return true se a análise foi bem-sucedida
     */
    bool AnalyzeServerPerformance();
    
    /**
     * @brief Testa um recurso específico
     * @param featureName Nome do recurso
     * @return true se o teste foi bem-sucedido
     */
    bool TestFeature(const std::string& featureName);
    
    /**
     * @brief Compila e implanta uma nova versão
     * @param version Número da versão
     * @return true se a compilação e implantação foram bem-sucedidas
     */
    bool CompileAndDeploy(const std::string& version);
    
    /**
     * @brief Gera um relatório de inconsistências nos dados do jogo
     * @return Relatório em formato de texto
     */
    std::string GenerateDataConsistencyReport();
    
    /**
     * @brief Verifica a integridade dos arquivos do jogo
     * @return true se todos os arquivos estão íntegros
     */
    bool CheckGameFilesIntegrity();
    
    /**
     * @brief Otimiza o banco de dados do jogo
     * @return true se a otimização foi bem-sucedida
     */
    bool OptimizeGameDatabase();
    
    /**
     * @brief Monitora o uso de recursos do servidor
     * @param duration Duração do monitoramento em segundos
     * @return Relatório de uso de recursos
     */
    std::string MonitorServerResources(int duration);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
    std::shared_ptr<ServerConnector> m_serverConnector;
    std::shared_ptr<FileSystemConnector> m_fileSystemConnector;
};

} // namespace WYDStudio