#pragma once

#include "BaseAgent.h"
#include "GameFileManager.h"
#include "AgentFileAccessProvider.h"
#include "AgentPermissionManager.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <filesystem>

namespace WYDStudio {

/**
 * @class AgentFileSystem
 * @brief Interface simplificada para integração de agentes com o sistema de arquivos
 * 
 * Esta classe atua como um wrapper sobre o complexo sistema de permissões e acesso a arquivos,
 * fornecendo uma API mais simples para os agentes utilizarem em suas operações.
 */
class AgentFileSystem {
public:
    /**
     * @brief Construtor
     */
    AgentFileSystem();
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~AgentFileSystem();
    
    /**
     * @brief Inicializa o sistema de arquivos para agentes
     * @param rootPath Caminho raiz do jogo
     * @param backupPath Caminho para armazenar backups
     * @param permissionsPath Caminho para arquivo de permissões (opcional)
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& rootPath, 
                   const std::string& backupPath,
                   const std::string& permissionsPath = "");
    
    /**
     * @brief Registra um agente no sistema
     * @param agent Agente a ser registrado
     * @param name Nome do agente
     * @return true se registrado com sucesso
     */
    bool RegisterAgent(std::shared_ptr<BaseAgent> agent, const std::string& name);
    
    /**
     * @brief Define configurações de permissão padrão
     * @return true se configurado com sucesso
     */
    bool SetupDefaultPermissions();
    
    /**
     * @brief Permite um diretório específico para um tipo de agente
     * @param agentType Tipo de agente
     * @param directory Diretório a ser permitido
     * @param operations Operações permitidas (vazio para todas)
     * @return true se configurado com sucesso
     */
    bool AllowDirectory(AgentType agentType, 
                       const std::string& directory,
                       const std::vector<FileOperation>& operations = {});
    
    /**
     * @brief Permite um tipo de arquivo específico para um tipo de agente
     * @param agentType Tipo de agente
     * @param fileType Tipo de arquivo (extensão)
     * @param operations Operações permitidas (vazio para todas)
     * @return true se configurado com sucesso
     */
    bool AllowFileType(AgentType agentType, 
                      const std::string& fileType,
                      const std::vector<FileOperation>& operations = {});
    
    /**
     * @brief Define as operações permitidas para um tipo de agente
     * @param agentType Tipo de agente
     * @param operations Operações permitidas
     * @return true se configurado com sucesso
     */
    bool AllowOperations(AgentType agentType, const std::vector<FileOperation>& operations);
    
    /**
     * @brief Obtém o gerenciador de arquivos
     * @return Ponteiro para o gerenciador de arquivos
     */
    std::shared_ptr<GameFileManager> GetFileManager() const;
    
    /**
     * @brief Obtém o provedor de acesso a arquivos
     * @return Ponteiro para o provedor de acesso a arquivos
     */
    std::shared_ptr<AgentFileAccessProvider> GetFileAccessProvider() const;
    
    /**
     * @brief Obtém o gerenciador de permissões
     * @return Ponteiro para o gerenciador de permissões
     */
    std::shared_ptr<AgentPermissionManager> GetPermissionManager() const;
    
    /**
     * @brief Registra um callback para log
     * @param logCallback Função de callback para log
     */
    void SetLogCallback(std::function<void(const std::string&)> logCallback);
    
    /**
     * @brief Aplica as configurações de permissão e finaliza a inicialização
     * @return true se aplicado com sucesso
     */
    bool ApplyPermissionsAndFinalize();
    
    /**
     * @brief Salva as configurações de permissão em um arquivo
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SavePermissions(const std::string& filePath);
    
private:
    // Componentes do sistema
    std::shared_ptr<GameFileManager> m_fileManager;
    std::shared_ptr<AgentFileAccessProvider> m_fileAccessProvider;
    std::shared_ptr<AgentPermissionManager> m_permissionManager;
    
    // Função de callback para log
    std::function<void(const std::string&)> m_logCallback;
    
    /**
     * @brief Registra uma mensagem no log
     * @param message Mensagem a ser registrada
     */
    void Log(const std::string& message);
};

} // namespace WYDStudio