#pragma once

#include "BaseAgent.h"
#include "AgentFileAccessProvider.h"
#include "GameFileManager.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace WYDStudio {

/**
 * @struct AgentPermissionConfig
 * @brief Configuração de permissões para um tipo de agente
 */
struct AgentPermissionConfig {
    std::vector<std::string> allowedDirectories;         // Diretórios permitidos
    std::vector<std::string> deniedDirectories;          // Diretórios negados (sobrescreve allowed)
    std::vector<std::string> allowedFileTypes;           // Tipos de arquivo permitidos
    std::vector<std::string> deniedFileTypes;            // Tipos de arquivo negados (sobrescreve allowed)
    std::vector<FileOperation> allowedOperations;        // Operações permitidas
    std::map<std::string, std::vector<FileOperation>> fileTypeSpecificPermissions;  // Permissões específicas por tipo de arquivo
    std::map<std::string, std::vector<FileOperation>> directorySpecificPermissions; // Permissões específicas por diretório
    bool includeSubdirectories;                          // Incluir subdiretórios nas permissões
    bool defaultAllow;                                   // Comportamento padrão: permitir ou negar
};

/**
 * @class AgentPermissionManager
 * @brief Gerencia permissões de acesso para diferentes tipos de agentes
 * 
 * Esta classe configura e aplica políticas de permissões para controlar 
 * quais tipos de agentes podem acessar quais arquivos e realizar quais operações.
 */
class AgentPermissionManager {
public:
    /**
     * @brief Construtor
     * @param fileAccessProvider Provedor de acesso a arquivos
     */
    AgentPermissionManager(std::shared_ptr<AgentFileAccessProvider> fileAccessProvider);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~AgentPermissionManager();
    
    /**
     * @brief Inicializa o gerenciador de permissões
     * @param configFilePath Caminho para arquivo de configuração (opcional)
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& configFilePath = "");
    
    /**
     * @brief Configura permissões para um tipo de agente
     * @param agentType Tipo de agente
     * @param config Configuração de permissões
     * @return true se configurado com sucesso
     */
    bool ConfigurePermissions(AgentType agentType, const AgentPermissionConfig& config);
    
    /**
     * @brief Aplica as configurações de permissão a um provedor de acesso a arquivos
     * @return true se aplicado com sucesso
     */
    bool ApplyPermissions();
    
    /**
     * @brief Carrega configurações de permissão de um arquivo
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadPermissionsFromFile(const std::string& filePath);
    
    /**
     * @brief Salva configurações de permissão em um arquivo
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SavePermissionsToFile(const std::string& filePath);
    
    /**
     * @brief Adiciona um diretório permitido para um tipo de agente
     * @param agentType Tipo de agente
     * @param directory Diretório
     * @param operations Operações permitidas (vazio para todas)
     * @return true se adicionado com sucesso
     */
    bool AddAllowedDirectory(AgentType agentType, 
                           const std::string& directory,
                           const std::vector<FileOperation>& operations = {});
    
    /**
     * @brief Adiciona um diretório negado para um tipo de agente
     * @param agentType Tipo de agente
     * @param directory Diretório
     * @return true se adicionado com sucesso
     */
    bool AddDeniedDirectory(AgentType agentType, const std::string& directory);
    
    /**
     * @brief Adiciona um tipo de arquivo permitido para um tipo de agente
     * @param agentType Tipo de agente
     * @param fileType Tipo de arquivo (extensão)
     * @param operations Operações permitidas (vazio para todas)
     * @return true se adicionado com sucesso
     */
    bool AddAllowedFileType(AgentType agentType, 
                          const std::string& fileType,
                          const std::vector<FileOperation>& operations = {});
    
    /**
     * @brief Adiciona um tipo de arquivo negado para um tipo de agente
     * @param agentType Tipo de agente
     * @param fileType Tipo de arquivo (extensão)
     * @return true se adicionado com sucesso
     */
    bool AddDeniedFileType(AgentType agentType, const std::string& fileType);
    
    /**
     * @brief Define as operações permitidas para um tipo de agente
     * @param agentType Tipo de agente
     * @param operations Operações permitidas
     * @return true se definido com sucesso
     */
    bool SetAllowedOperations(AgentType agentType, const std::vector<FileOperation>& operations);
    
    /**
     * @brief Define se subdiretórios são incluídos nas permissões
     * @param agentType Tipo de agente
     * @param include true para incluir subdiretórios
     * @return true se definido com sucesso
     */
    bool SetIncludeSubdirectories(AgentType agentType, bool include);
    
    /**
     * @brief Define o comportamento padrão para um tipo de agente
     * @param agentType Tipo de agente
     * @param allow true para permitir por padrão
     * @return true se definido com sucesso
     */
    bool SetDefaultAllow(AgentType agentType, bool allow);
    
    /**
     * @brief Registra um callback para log
     * @param logCallback Função de callback para log
     */
    void SetLogCallback(std::function<void(const std::string&)> logCallback);
    
    /**
     * @brief Criar configurações padrão para todos os tipos de agentes
     * @return true se criado com sucesso
     */
    bool CreateDefaultPermissions();
    
private:
    // Provedor de acesso a arquivos
    std::shared_ptr<AgentFileAccessProvider> m_fileAccessProvider;
    
    // Configurações de permissão por tipo de agente
    std::map<AgentType, AgentPermissionConfig> m_permissions;
    
    // Função de callback para log
    std::function<void(const std::string&)> m_logCallback;
    
    /**
     * @brief Registra uma mensagem no log
     * @param message Mensagem a ser registrada
     */
    void Log(const std::string& message);
    
    /**
     * @brief Obtém todas as operações de arquivo disponíveis
     * @return Lista de operações
     */
    std::vector<FileOperation> GetAllOperations();
};

} // namespace WYDStudio