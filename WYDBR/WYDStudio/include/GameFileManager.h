#pragma once

#include "FileSystemConnector.h"
#include "ServerConnector.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <mutex>
#include <filesystem>

namespace WYDStudio {

/**
 * @enum FileOperation
 * @brief Tipos de operações com arquivos
 */
enum class FileOperation {
    CREATE,
    MODIFY,
    DELETE,
    MOVE,
    COPY,
    COMPILE,
    DECOMPILE
};

/**
 * @enum ServerFileStatus
 * @brief Status de sincronização do arquivo com o servidor
 */
enum class ServerFileStatus {
    NOT_APPLICABLE,     // Arquivo não relevante para o servidor
    IN_SYNC,            // Arquivo está em sincronia com o servidor
    MODIFIED_LOCALLY,   // Arquivo foi modificado localmente
    MODIFIED_ON_SERVER, // Arquivo foi modificado no servidor
    CONFLICT,           // Conflito entre versão local e servidor
    UNKNOWN             // Status desconhecido
};

/**
 * @struct FileOperationResult
 * @brief Resultado de uma operação com arquivo
 */
struct FileOperationResult {
    bool success;                   // Indica se a operação foi bem-sucedida
    std::string message;            // Mensagem detalhada sobre o resultado
    std::string backupPath;         // Caminho para o backup (se aplicável)
    int64_t timestamp;              // Timestamp da operação
    std::string originalPath;       // Caminho original do arquivo
    std::string newPath;            // Novo caminho (se aplicável, ex: operação MOVE)
    bool serverSyncRequired;        // Indica se sincronização com servidor é necessária
    ServerFileStatus serverStatus;  // Status em relação ao servidor
};

/**
 * @struct BackupInfo
 * @brief Informações sobre um backup
 */
struct BackupInfo {
    std::string backupPath;         // Caminho do arquivo de backup
    std::string originalPath;       // Caminho original do arquivo
    int64_t timestamp;              // Timestamp de criação do backup
    FileOperation operation;        // Operação que gerou o backup
    std::string description;        // Descrição adicional
};

/**
 * @struct FileMetadata
 * @brief Metadados de um arquivo do jogo
 */
struct FileMetadata {
    std::string path;               // Caminho do arquivo
    std::string type;               // Tipo de arquivo (json, bin, etc)
    int64_t size;                   // Tamanho em bytes
    int64_t lastModified;           // Timestamp da última modificação
    std::string format;             // Formato do arquivo (se aplicável)
    std::string version;            // Versão do arquivo (se aplicável)
    std::map<std::string, std::string> customData; // Dados adicionais específicos do tipo
    ServerFileStatus serverStatus;  // Status em relação ao servidor
};

/**
 * @struct DirectoryInfo
 * @brief Informações sobre um diretório
 */
struct DirectoryInfo {
    std::string path;                       // Caminho do diretório
    std::vector<std::string> files;         // Lista de arquivos
    std::vector<std::string> subdirectories;// Lista de subdiretórios
    int64_t totalSize;                      // Tamanho total em bytes
    int fileCount;                          // Quantidade de arquivos
    int directoryCount;                     // Quantidade de diretórios
};

/**
 * @class GameFileManager
 * @brief Gerencia operações com arquivos do jogo WYD
 * 
 * Esta classe é responsável por todas as operações de arquivo relacionadas ao jogo,
 * incluindo arquivos compilados, descompilados, configurações, e scripts.
 * Implementa mecanismo de backup automático para garantir segurança em cada operação.
 */
class GameFileManager {
public:
    /**
     * @brief Construtor
     * @param fsConnector Conector de sistema de arquivos
     * @param serverConnector Conector de servidor (opcional)
     */
    GameFileManager(std::shared_ptr<FileSystemConnector> fsConnector,
                   std::shared_ptr<ServerConnector> serverConnector = nullptr);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~GameFileManager();
    
    /**
     * @brief Inicializa o gerenciador de arquivos
     * @param gamePath Diretório principal do jogo
     * @param backupPath Diretório para armazenar backups
     * @param maxBackupsPerFile Número máximo de backups por arquivo
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& gamePath, 
                   const std::string& backupPath = "backups",
                   int maxBackupsPerFile = 10);
    
    /**
     * @brief Define se a sincronização com servidor está ativa
     * @param enabled true para ativar sincronização
     */
    void SetServerSyncEnabled(bool enabled);
    
    /**
     * @brief Obtém o status atual da sincronização com servidor
     * @return true se a sincronização está ativa
     */
    bool IsServerSyncEnabled() const;
    
    /**
     * @brief Cria um novo arquivo
     * @param path Caminho para o novo arquivo
     * @param content Conteúdo do arquivo
     * @param description Descrição da operação para o backup
     * @return Resultado da operação
     */
    FileOperationResult CreateFile(const std::string& path, 
                                 const std::string& content,
                                 const std::string& description = "");
    
    /**
     * @brief Cria um novo arquivo binário
     * @param path Caminho para o novo arquivo
     * @param data Dados binários
     * @param size Tamanho dos dados
     * @param description Descrição da operação para o backup
     * @return Resultado da operação
     */
    FileOperationResult CreateBinaryFile(const std::string& path, 
                                       const void* data, 
                                       size_t size,
                                       const std::string& description = "");
    
    /**
     * @brief Modifica um arquivo existente
     * @param path Caminho do arquivo
     * @param newContent Novo conteúdo do arquivo
     * @param description Descrição da operação para o backup
     * @return Resultado da operação
     */
    FileOperationResult ModifyFile(const std::string& path, 
                                 const std::string& newContent,
                                 const std::string& description = "");
    
    /**
     * @brief Modifica uma parte específica de um arquivo
     * @param path Caminho do arquivo
     * @param oldContent Conteúdo a ser substituído
     * @param newContent Novo conteúdo
     * @param description Descrição da operação para o backup
     * @return Resultado da operação
     */
    FileOperationResult ModifyFilePart(const std::string& path, 
                                     const std::string& oldContent,
                                     const std::string& newContent,
                                     const std::string& description = "");
    
    /**
     * @brief Exclui um arquivo
     * @param path Caminho do arquivo a ser excluído
     * @param description Descrição da operação para o backup
     * @return Resultado da operação
     */
    FileOperationResult DeleteFile(const std::string& path,
                                 const std::string& description = "");
    
    /**
     * @brief Move um arquivo para outro local
     * @param sourcePath Caminho de origem
     * @param destinationPath Caminho de destino
     * @param description Descrição da operação para o backup
     * @return Resultado da operação
     */
    FileOperationResult MoveFile(const std::string& sourcePath,
                               const std::string& destinationPath,
                               const std::string& description = "");
    
    /**
     * @brief Copia um arquivo para outro local
     * @param sourcePath Caminho de origem
     * @param destinationPath Caminho de destino
     * @param description Descrição da operação para o backup
     * @return Resultado da operação
     */
    FileOperationResult CopyFile(const std::string& sourcePath,
                               const std::string& destinationPath,
                               const std::string& description = "");
    
    /**
     * @brief Compila um arquivo JSON para formato binário do jogo
     * @param jsonPath Caminho do arquivo JSON
     * @param binPath Caminho de saída para o arquivo binário (opcional)
     * @param description Descrição da operação para o backup
     * @return Resultado da operação
     */
    FileOperationResult CompileJsonToBinary(const std::string& jsonPath,
                                          const std::string& binPath = "",
                                          const std::string& description = "");
    
    /**
     * @brief Descompila um arquivo binário do jogo para JSON
     * @param binPath Caminho do arquivo binário
     * @param jsonPath Caminho de saída para o arquivo JSON (opcional)
     * @param description Descrição da operação para o backup
     * @return Resultado da operação
     */
    FileOperationResult DecompileBinaryToJson(const std::string& binPath,
                                            const std::string& jsonPath = "",
                                            const std::string& description = "");
    
    /**
     * @brief Lê o conteúdo de um arquivo como texto
     * @param path Caminho do arquivo
     * @return Conteúdo do arquivo
     */
    std::string ReadFileAsText(const std::string& path);
    
    /**
     * @brief Lê o conteúdo de um arquivo binário
     * @param path Caminho do arquivo
     * @return Vetor de bytes com o conteúdo
     */
    std::vector<uint8_t> ReadFileAsBinary(const std::string& path);
    
    /**
     * @brief Cria um backup manual de um arquivo
     * @param path Caminho do arquivo
     * @param description Descrição do backup
     * @return Informações sobre o backup criado
     */
    BackupInfo CreateManualBackup(const std::string& path,
                                 const std::string& description = "Backup manual");
    
    /**
     * @brief Restaura um arquivo a partir de um backup
     * @param backupPath Caminho do arquivo de backup
     * @param restoreToOriginalPath true para restaurar no caminho original
     * @param alternativePath Caminho alternativo para restauração
     * @return Resultado da operação
     */
    FileOperationResult RestoreFromBackup(const std::string& backupPath,
                                        bool restoreToOriginalPath = true,
                                        const std::string& alternativePath = "");
    
    /**
     * @brief Obtém a lista de backups disponíveis para um arquivo
     * @param path Caminho do arquivo
     * @return Lista de informações de backup
     */
    std::vector<BackupInfo> GetBackupsForFile(const std::string& path);
    
    /**
     * @brief Obtém todos os backups disponíveis
     * @return Mapa de caminho de arquivo para lista de backups
     */
    std::map<std::string, std::vector<BackupInfo>> GetAllBackups();
    
    /**
     * @brief Limpa backups antigos
     * @param keepCount Número de backups mais recentes a manter
     * @param olderThan Remover backups mais antigos que esta data (0 para ignorar)
     * @return Número de backups removidos
     */
    int CleanupOldBackups(int keepCount = 0, int64_t olderThan = 0);
    
    /**
     * @brief Sincroniza arquivos com o servidor
     * @param paths Lista de caminhos para sincronizar (vazio para todos)
     * @param direction Direção da sincronização (0=ambos, 1=local->servidor, 2=servidor->local)
     * @return Mapa de caminhos para resultados de operações
     */
    std::map<std::string, FileOperationResult> SyncWithServer(
        const std::vector<std::string>& paths = {},
        int direction = 0);
    
    /**
     * @brief Obtém a lista de arquivos em um diretório
     * @param directory Caminho do diretório
     * @param recursive true para incluir subdiretórios
     * @param filter Filtro para extensões (ex: ".json,.bin")
     * @return Informações sobre o diretório
     */
    DirectoryInfo ListDirectory(const std::string& directory,
                              bool recursive = false,
                              const std::string& filter = "");
    
    /**
     * @brief Obtém metadados de um arquivo
     * @param path Caminho do arquivo
     * @return Metadados do arquivo
     */
    FileMetadata GetFileMetadata(const std::string& path);
    
    /**
     * @brief Cria um diretório
     * @param path Caminho do diretório
     * @param createParents true para criar diretórios pais se necessário
     * @return true se criado com sucesso
     */
    bool CreateDirectory(const std::string& path, bool createParents = true);
    
    /**
     * @brief Exclui um diretório
     * @param path Caminho do diretório
     * @param recursive true para excluir recursivamente
     * @return true se excluído com sucesso
     */
    bool DeleteDirectory(const std::string& path, bool recursive = false);
    
    /**
     * @brief Verifica se um arquivo existe
     * @param path Caminho do arquivo
     * @return true se o arquivo existe
     */
    bool FileExists(const std::string& path);
    
    /**
     * @brief Verifica se um diretório existe
     * @param path Caminho do diretório
     * @return true se o diretório existe
     */
    bool DirectoryExists(const std::string& path);
    
    /**
     * @brief Registra um callback para log
     * @param logCallback Função de callback para log
     */
    void SetLogCallback(std::function<void(const std::string&)> logCallback);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
    
    // Métodos auxiliares privados
    BackupInfo CreateBackup(const std::string& path, 
                          FileOperation operation,
                          const std::string& description);
    
    std::string GenerateBackupPath(const std::string& originalPath, 
                                 FileOperation operation);
    
    bool ApplyServerSync(const std::string& path, 
                        const FileOperationResult& result);
};

} // namespace WYDStudio