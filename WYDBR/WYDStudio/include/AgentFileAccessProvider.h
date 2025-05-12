#pragma once

#include "BaseAgent.h"
#include "GameFileManager.h"
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>

namespace WYDStudio {

/**
 * @struct AgentFileAction
 * @brief Registro de uma ação com arquivo realizada por um agente
 */
struct AgentFileAction {
    AgentType agentType;                // Tipo do agente que realizou a ação
    std::string agentName;              // Nome do agente
    FileOperation operation;            // Operação realizada
    std::string targetPath;             // Caminho do arquivo alvo
    std::string description;            // Descrição da ação
    int64_t timestamp;                  // Timestamp da ação
    std::string backupPath;             // Caminho do backup (se disponível)
    bool success;                       // Resultado da operação
};

/**
 * @class AgentFileAccessProvider
 * @brief Fornece acesso controlado a arquivos do jogo para agentes
 * 
 * Esta classe atua como um intermediário entre os agentes IA e o sistema
 * de arquivos, garantindo que todas as operações sejam registradas, rastreáveis
 * e seguras, com backups automáticos.
 */
class AgentFileAccessProvider {
public:
    /**
     * @brief Construtor
     * @param fileManager Gerenciador de arquivos do jogo
     */
    AgentFileAccessProvider(std::shared_ptr<GameFileManager> fileManager);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~AgentFileAccessProvider();
    
    /**
     * @brief Registra um agente para permitir acesso a arquivos
     * @param agent Agente a ser registrado
     * @param type Tipo do agente
     * @param name Nome do agente
     * @return true se registrado com sucesso
     */
    bool RegisterAgent(std::shared_ptr<BaseAgent> agent, 
                      AgentType type,
                      const std::string& name);
    
    /**
     * @brief Define permissões por tipo de arquivo para um tipo de agente
     * @param agentType Tipo do agente
     * @param fileExtensions Lista de extensões de arquivo
     * @param operations Operações permitidas
     * @return true se definido com sucesso
     */
    bool SetFileTypePermissions(AgentType agentType, 
                              const std::vector<std::string>& fileExtensions,
                              const std::vector<FileOperation>& operations);
    
    /**
     * @brief Define permissões por diretório para um tipo de agente
     * @param agentType Tipo do agente
     * @param directoryPaths Lista de caminhos de diretórios
     * @param operations Operações permitidas
     * @param includeSubdirectories true para incluir subdiretórios
     * @return true se definido com sucesso
     */
    bool SetDirectoryPermissions(AgentType agentType, 
                               const std::vector<std::string>& directoryPaths,
                               const std::vector<FileOperation>& operations,
                               bool includeSubdirectories = true);
    
    /**
     * @brief Permite que um agente crie um novo arquivo
     * @param agent Agente solicitante
     * @param path Caminho para o novo arquivo
     * @param content Conteúdo do arquivo
     * @param description Descrição da ação
     * @return Resultado da operação
     */
    FileOperationResult AgentCreateFile(BaseAgent* agent, 
                                      const std::string& path, 
                                      const std::string& content,
                                      const std::string& description);
    
    /**
     * @brief Permite que um agente modifique um arquivo existente
     * @param agent Agente solicitante
     * @param path Caminho do arquivo
     * @param newContent Novo conteúdo do arquivo
     * @param description Descrição da ação
     * @return Resultado da operação
     */
    FileOperationResult AgentModifyFile(BaseAgent* agent, 
                                      const std::string& path, 
                                      const std::string& newContent,
                                      const std::string& description);
    
    /**
     * @brief Permite que um agente modifique parte de um arquivo
     * @param agent Agente solicitante
     * @param path Caminho do arquivo
     * @param oldContent Conteúdo a ser substituído
     * @param newContent Novo conteúdo
     * @param description Descrição da ação
     * @return Resultado da operação
     */
    FileOperationResult AgentModifyFilePart(BaseAgent* agent, 
                                          const std::string& path, 
                                          const std::string& oldContent,
                                          const std::string& newContent,
                                          const std::string& description);
    
    /**
     * @brief Permite que um agente exclua um arquivo
     * @param agent Agente solicitante
     * @param path Caminho do arquivo
     * @param description Descrição da ação
     * @return Resultado da operação
     */
    FileOperationResult AgentDeleteFile(BaseAgent* agent, 
                                      const std::string& path,
                                      const std::string& description);
    
    /**
     * @brief Permite que um agente mova um arquivo
     * @param agent Agente solicitante
     * @param sourcePath Caminho de origem
     * @param destinationPath Caminho de destino
     * @param description Descrição da ação
     * @return Resultado da operação
     */
    FileOperationResult AgentMoveFile(BaseAgent* agent, 
                                    const std::string& sourcePath,
                                    const std::string& destinationPath,
                                    const std::string& description);
    
    /**
     * @brief Permite que um agente copie um arquivo
     * @param agent Agente solicitante
     * @param sourcePath Caminho de origem
     * @param destinationPath Caminho de destino
     * @param description Descrição da ação
     * @return Resultado da operação
     */
    FileOperationResult AgentCopyFile(BaseAgent* agent, 
                                    const std::string& sourcePath,
                                    const std::string& destinationPath,
                                    const std::string& description);
    
    /**
     * @brief Permite que um agente compile um arquivo JSON para binário
     * @param agent Agente solicitante
     * @param jsonPath Caminho do arquivo JSON
     * @param binPath Caminho para o arquivo binário (opcional)
     * @param description Descrição da ação
     * @return Resultado da operação
     */
    FileOperationResult AgentCompileJsonToBinary(BaseAgent* agent, 
                                               const std::string& jsonPath,
                                               const std::string& binPath,
                                               const std::string& description);
    
    /**
     * @brief Permite que um agente descompile um arquivo binário para JSON
     * @param agent Agente solicitante
     * @param binPath Caminho do arquivo binário
     * @param jsonPath Caminho para o arquivo JSON (opcional)
     * @param description Descrição da ação
     * @return Resultado da operação
     */
    FileOperationResult AgentDecompileBinaryToJson(BaseAgent* agent, 
                                                 const std::string& binPath,
                                                 const std::string& jsonPath,
                                                 const std::string& description);
    
    /**
     * @brief Permite que um agente leia o conteúdo de um arquivo
     * @param agent Agente solicitante
     * @param path Caminho do arquivo
     * @return Conteúdo do arquivo
     */
    std::string AgentReadFile(BaseAgent* agent, 
                             const std::string& path);
    
    /**
     * @brief Permite que um agente liste o conteúdo de um diretório
     * @param agent Agente solicitante
     * @param directory Caminho do diretório
     * @param recursive true para incluir subdiretórios
     * @param filter Filtro para extensões
     * @return Informações do diretório
     */
    DirectoryInfo AgentListDirectory(BaseAgent* agent, 
                                   const std::string& directory,
                                   bool recursive = false,
                                   const std::string& filter = "");
    
    /**
     * @brief Restaura um arquivo a partir de um backup
     * @param agent Agente solicitante
     * @param backupPath Caminho do backup
     * @param restoreToOriginalPath true para restaurar no caminho original
     * @param alternativePath Caminho alternativo para restauração
     * @param description Descrição da ação
     * @return Resultado da operação
     */
    FileOperationResult AgentRestoreBackup(BaseAgent* agent, 
                                         const std::string& backupPath,
                                         bool restoreToOriginalPath = true,
                                         const std::string& alternativePath = "",
                                         const std::string& description = "Restauração de backup");
    
    /**
     * @brief Obtém o histórico de ações com arquivos realizadas por agentes
     * @param agentType Tipo de agente para filtrar (opcional)
     * @param maxEntries Número máximo de entradas a retornar (0 para todas)
     * @return Lista de ações com arquivos
     */
    std::vector<AgentFileAction> GetAgentFileActionHistory(AgentType agentType = AgentType::UNKNOWN,
                                                        int maxEntries = 0);
    
    /**
     * @brief Obtém uma lista de backups disponíveis para um arquivo
     * @param path Caminho do arquivo
     * @return Lista de informações de backup
     */
    std::vector<BackupInfo> GetBackupsForFile(const std::string& path);
    
    /**
     * @brief Registra um callback para log
     * @param logCallback Função de callback para log
     */
    void SetLogCallback(std::function<void(const std::string&)> logCallback);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
    
    // GameFileManager compartilhado
    std::shared_ptr<GameFileManager> m_fileManager;
    
    // Métodos auxiliares privados
    bool HasPermission(BaseAgent* agent, 
                     const std::string& path, 
                     FileOperation operation);
    
    AgentType GetAgentType(BaseAgent* agent);
    
    std::string GetAgentName(BaseAgent* agent);
    
    void LogAgentAction(BaseAgent* agent, 
                       FileOperation operation,
                       const std::string& targetPath,
                       const std::string& description,
                       bool success,
                       const std::string& backupPath = "");
};

} // namespace WYDStudio