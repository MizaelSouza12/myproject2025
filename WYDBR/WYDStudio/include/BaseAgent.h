#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace WYDStudio {

class AgentFileAccessProvider;
struct FileOperationResult;

/**
 * @enum AgentType
 * @brief Tipos de agentes no sistema
 */
enum class AgentType {
    ADMIN,          // Agente administrador (acesso total)
    GM,             // Agente game master (acesso limitado)
    DEVELOPER,      // Agente desenvolvedor (acesso a recursos técnicos)
    PLAYER_SUPPORT, // Agente de suporte ao jogador
    QUEST_HELPER,   // Agente de ajuda com quests
    UNKNOWN         // Tipo desconhecido
};

/**
 * @enum AgentCapability
 * @brief Capacidades que um agente pode ter
 */
enum class AgentCapability {
    FILE_ACCESS,       // Acesso a arquivos
    DATABASE_ACCESS,   // Acesso a banco de dados
    NETWORK_ACCESS,    // Acesso a rede
    GAME_KNOWLEDGE,    // Conhecimento sobre o jogo
    PLAYER_INTERACTION, // Interação com jogadores
    CODE_GENERATION,   // Geração de código
    SCRIPT_EXECUTION   // Execução de scripts
};

/**
 * @class BaseAgent
 * @brief Classe base para todos os agentes do sistema
 * 
 * Esta classe define a interface comum para todos os agentes,
 * incluindo inicialização, processamento de consultas e execução
 * de ações.
 */
class BaseAgent {
public:
    /**
     * @brief Construtor padrão
     */
    BaseAgent() : m_initialized(false) {}
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~BaseAgent() {}
    
    /**
     * @brief Inicializa o agente
     * @param modelPath Caminho para o modelo de IA
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const std::string& modelPath) = 0;
    
    /**
     * @brief Processa uma consulta e retorna uma resposta
     * @param query Consulta a ser processada
     * @return Resposta à consulta
     */
    virtual std::string ProcessQuery(const std::string& query) = 0;
    
    /**
     * @brief Executa uma ação com parâmetros
     * @param actionName Nome da ação a ser executada
     * @param parameters Parâmetros para a ação
     * @return Resultado da ação
     */
    virtual std::string ExecuteAction(const std::string& actionName, 
                                   const std::map<std::string, std::string>& parameters) = 0;
    
    /**
     * @brief Obtém o tipo do agente
     * @return Tipo do agente
     */
    virtual AgentType GetAgentType() const = 0;
    
    /**
     * @brief Obtém as capacidades do agente
     * @return Lista de capacidades
     */
    virtual std::vector<AgentCapability> GetCapabilities() const = 0;
    
    /**
     * @brief Verifica se o agente está inicializado
     * @return true se estiver inicializado
     */
    bool IsInitialized() const {
        return m_initialized;
    }
    
    /**
     * @brief Define o provedor de acesso a arquivos
     * @param provider Provedor de acesso a arquivos
     */
    void SetFileAccessProvider(std::shared_ptr<AgentFileAccessProvider> provider) {
        m_fileAccessProvider = provider;
    }
    
    // Métodos de acesso a arquivos
    
    /**
     * @brief Cria um arquivo
     * @param filePath Caminho do arquivo
     * @param content Conteúdo do arquivo
     * @param description Descrição da operação (para backup)
     * @return Resultado da operação
     */
    FileOperationResult CreateFile(const std::string& filePath, 
                                 const std::string& content,
                                 const std::string& description = "");
    
    /**
     * @brief Modifica um arquivo
     * @param filePath Caminho do arquivo
     * @param newContent Novo conteúdo do arquivo
     * @param description Descrição da operação (para backup)
     * @return Resultado da operação
     */
    FileOperationResult ModifyFile(const std::string& filePath, 
                                 const std::string& newContent,
                                 const std::string& description = "");
    
    /**
     * @brief Exclui um arquivo
     * @param filePath Caminho do arquivo
     * @param description Descrição da operação (para backup)
     * @return Resultado da operação
     */
    FileOperationResult DeleteFile(const std::string& filePath, 
                                 const std::string& description = "");
    
    /**
     * @brief Move um arquivo
     * @param sourcePath Caminho de origem
     * @param destPath Caminho de destino
     * @param description Descrição da operação (para backup)
     * @return Resultado da operação
     */
    FileOperationResult MoveFile(const std::string& sourcePath, 
                               const std::string& destPath,
                               const std::string& description = "");
    
    /**
     * @brief Copia um arquivo
     * @param sourcePath Caminho de origem
     * @param destPath Caminho de destino
     * @param description Descrição da operação (para backup)
     * @return Resultado da operação
     */
    FileOperationResult CopyFile(const std::string& sourcePath, 
                               const std::string& destPath,
                               const std::string& description = "");
    
    /**
     * @brief Lê o conteúdo de um arquivo
     * @param filePath Caminho do arquivo
     * @return Conteúdo do arquivo
     */
    std::string ReadFile(const std::string& filePath);
    
    /**
     * @brief Lista arquivos em um diretório
     * @param dirPath Caminho do diretório
     * @return Lista de caminhos de arquivos
     */
    std::vector<std::string> ListFiles(const std::string& dirPath);
    
    /**
     * @brief Restaura um arquivo a partir de um backup
     * @param backupPath Caminho do backup
     * @param overwrite Sobrescrever o arquivo original
     * @param outputPath Caminho de saída (opcional)
     * @param description Descrição da operação
     * @return Resultado da operação
     */
    FileOperationResult RestoreFromBackup(const std::string& backupPath, 
                                       bool overwrite = true,
                                       const std::string& outputPath = "",
                                       const std::string& description = "");
    
    /**
     * @brief Obtém backups disponíveis para um arquivo
     * @param filePath Caminho do arquivo
     * @return Lista de informações de backups
     */
    std::vector<struct FileBackupInfo> GetBackupsForFile(const std::string& filePath);
    
protected:
    // Capacidades do agente
    std::vector<AgentCapability> m_capabilities;
    
    // Status de inicialização
    bool m_initialized;
    
    // Provedor de acesso a arquivos
    std::shared_ptr<AgentFileAccessProvider> m_fileAccessProvider;
};

} // namespace WYDStudio