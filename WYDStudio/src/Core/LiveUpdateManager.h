#ifndef WYDSTUDIO_LIVE_UPDATE_MANAGER_H
#define WYDSTUDIO_LIVE_UPDATE_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>
#include <queue>
#include <variant>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "WYDStudioFacade.h"
#include "../decompilers/AdvancedBinaryAnalyzer.h"

namespace wydstudio {
namespace core {

using json = nlohmann::json;
namespace fs = std::filesystem;

/**
 * @brief Tipos de arquivos suportados
 */
enum class FileType {
    ITEM_LIST,          // Lista de itens (ItemList.bin)
    ITEM_NAME,          // Nomes de itens (Itemname.bin)
    ITEM_HELP,          // Descrições de itens (itemhelp.bin)
    ITEM_PRICE,         // Preços de itens
    MAP_DATA,           // Dados de mapa
    MAP_HEIGHT,         // Altura do terreno
    MAP_ATTRIBUTE,      // Atributos do mapa
    NPC_DATA,           // Dados de NPCs
    MOB_DATA,           // Dados de monstros
    SKILL_DATA,         // Dados de habilidades
    QUEST_DATA,         // Dados de quests
    EVENT_DATA,         // Dados de eventos
    UI_COMPONENT,       // Componentes de interface (.wyt)
    UI_TEXTURE,         // Texturas de interface
    UI_CONFIG,          // Configuração de interface
    GAME_CONFIG,        // Configuração do jogo
    SERVER_CONFIG,      // Configuração do servidor
    TEXT_RESOURCE,      // Recursos de texto
    CUSTOM_BINARY,      // Binário personalizado
    JSON_DATA,          // Dados JSON
    XML_DATA,           // Dados XML
    INI_DATA,           // Dados INI
    CSV_DATA,           // Dados CSV
    TEXT_DATA           // Dados de texto puro
};

/**
 * @brief Tipos de operações live
 */
enum class LiveOperationType {
    ADD,                // Adicionar novo item/entidade
    MODIFY,             // Modificar item/entidade existente
    REMOVE,             // Remover item/entidade
    REPLACE_FILE,       // Substituir arquivo inteiro
    PATCH_FILE,         // Aplicar patch ao arquivo
    RELOAD_RESOURCE,    // Forçar recarga de recurso
    EXECUTE_SCRIPT,     // Executar script personalizado
    BROADCAST_NOTICE,   // Enviar notificação a jogadores
    RESTART_SERVICE     // Reiniciar serviço específico (sem reiniciar o servidor)
};

/**
 * @brief Estado da operação live
 */
enum class LiveOperationStatus {
    PENDING,            // Aguardando processamento
    IN_PROGRESS,        // Em processamento
    COMPLETED,          // Concluída com sucesso
    FAILED,             // Falhou
    REVERTED,           // Revertida
    PARTIAL_SUCCESS     // Parcialmente bem-sucedida
};

/**
 * @brief Resultado de decompilação
 */
struct DecompilationResult {
    bool success;
    std::string filePath;       // Caminho do arquivo decompilado
    std::string originalPath;   // Caminho do arquivo original
    std::string format;         // Formato do arquivo decompilado (JSON, XML, etc.)
    std::string errorMessage;   // Mensagem de erro se houver falha
    std::map<std::string, std::string> metadata; // Metadados adicionais
};

/**
 * @brief Resultado de compilação
 */
struct CompilationResult {
    bool success;
    std::string filePath;       // Caminho do arquivo compilado
    std::string sourcePath;     // Caminho do arquivo fonte
    std::string format;         // Formato do arquivo fonte
    std::string errorMessage;   // Mensagem de erro se houver falha
    std::map<std::string, std::string> metadata; // Metadados adicionais
};

/**
 * @brief Informações de mapeamento de arquivo
 */
struct FileMapping {
    std::string originalPath;   // Caminho do arquivo original (compilado)
    std::string decompPath;     // Caminho do arquivo decompilado
    FileType fileType;          // Tipo do arquivo
    std::string format;         // Formato do arquivo decompilado
    bool canEditCompiled;       // Se pode editar diretamente o compilado
    bool needsServerReload;     // Se requer recarga do servidor
    bool needsClientReload;     // Se requer recarga do cliente
    std::string decompilerModule; // Módulo para decompilação
    std::string compilerModule;   // Módulo para compilação
    std::map<std::string, std::string> metadata; // Metadados adicionais
};

/**
 * @brief Tipo de notificação de atualização
 */
enum class UpdateNotificationType {
    INFO,               // Informativo
    WARNING,            // Aviso
    ERROR,              // Erro
    SUCCESS,            // Sucesso
    PROGRESS            // Progresso
};

/**
 * @brief Notificação de atualização
 */
struct UpdateNotification {
    UpdateNotificationType type;
    std::string message;
    std::string componentId;
    std::string entityId;
    int32_t progress;           // Progresso (0-100)
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @brief Dados de operação live
 */
struct LiveOperationData {
    // Identificação
    std::string id;
    std::string name;
    std::string description;
    LiveOperationType type;
    
    // Alvo da operação
    std::string targetFile;
    std::string targetId;
    FileType fileType;
    
    // Dados da operação
    std::variant<std::string, json, std::vector<uint8_t>> data;
    std::map<std::string, std::variant<std::string, int, bool>> parameters;
    
    // Metadados
    std::string requestedBy;
    std::chrono::system_clock::time_point requestTime;
    std::chrono::system_clock::time_point executionTime;
    std::string clientAddress;
    
    // Estado
    LiveOperationStatus status;
    std::string statusMessage;
    int32_t progress;
    
    // Backup para reversão
    std::string backupPath;
    bool canRevert;
};

/**
 * @brief Configuração do gerenciador de atualização live
 */
struct LiveUpdateConfig {
    // Caminhos de diretórios
    std::string clientDir;
    std::string serverDir;
    std::string backupDir;
    std::string tempDir;
    
    // Configurações gerais
    bool autoDecompile;
    bool autoRecompile;
    bool createBackups;
    int32_t maxBackups;
    int32_t backupRetentionDays;
    
    // Monitoramento
    bool watchFileChanges;
    int32_t pollIntervalMs;
    std::vector<std::string> watchPaths;
    std::vector<std::string> excludePaths;
    
    // Limites e proteções
    int32_t maxConcurrentOperations;
    int32_t operationTimeoutSec;
    int32_t maxQueueSize;
    int32_t maxFileSize;
    
    // Conectividade com o servidor
    std::string serverApiUrl;
    std::string serverWebsocketUrl;
    std::string authToken;
    bool useSecureConnection;
    int32_t connectionTimeoutSec;
    int32_t reconnectIntervalSec;
    
    // Notificações
    bool notifyPlayers;
    bool notifyAdmins;
    bool logAllOperations;
    std::string notificationFormat;
};

/**
 * @brief Gerenciador de atualização live para o WYDStudio
 */
class LiveUpdateManager {
public:
    /**
     * @brief Construtor
     */
    LiveUpdateManager(const std::shared_ptr<WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~LiveUpdateManager();
    
    /**
     * @brief Inicializa o gerenciador com configuração
     * @param config Configuração para o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize(const LiveUpdateConfig& config);
    
    /**
     * @brief Inicia o serviço de atualização live
     * @return true se iniciado com sucesso
     */
    bool start();
    
    /**
     * @brief Para o serviço de atualização live
     */
    void stop();
    
    /**
     * @brief Realiza varredura inicial de arquivos
     * @return true se varredura concluída com sucesso
     */
    bool scanFiles();
    
    /**
     * @brief Verifica se o serviço está em execução
     * @return true se em execução
     */
    bool isRunning() const;
    
    /**
     * @brief Enfileira uma operação live
     * @param operation Operação a enfileirar
     * @return ID da operação ou string vazia se falhar
     */
    std::string queueOperation(const LiveOperationData& operation);
    
    /**
     * @brief Cancela uma operação pendente
     * @param operationId ID da operação
     * @return true se cancelada com sucesso
     */
    bool cancelOperation(const std::string& operationId);
    
    /**
     * @brief Obtém status de uma operação
     * @param operationId ID da operação
     * @return Status da operação
     */
    LiveOperationStatus getOperationStatus(const std::string& operationId);
    
    /**
     * @brief Obtém detalhes de uma operação
     * @param operationId ID da operação
     * @return Dados da operação ou nulo se não encontrada
     */
    std::optional<LiveOperationData> getOperationDetails(const std::string& operationId);
    
    /**
     * @brief Obtém operações recentes
     * @param count Número máximo de operações
     * @param includeCompleted Incluir operações concluídas
     * @return Lista de operações
     */
    std::vector<LiveOperationData> getRecentOperations(int32_t count = 10, bool includeCompleted = true);
    
    /**
     * @brief Obtém notificações recentes
     * @param count Número máximo de notificações
     * @return Lista de notificações
     */
    std::vector<UpdateNotification> getRecentNotifications(int32_t count = 10);
    
    /**
     * @brief Decompila um arquivo para formato editável
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo
     * @param forcedFormat Formato forçado para decompilação (ou vazio para automático)
     * @return Resultado da decompilação
     */
    DecompilationResult decompileFile(const std::string& filePath, FileType fileType, const std::string& forcedFormat = "");
    
    /**
     * @brief Compila um arquivo para formato binário
     * @param filePath Caminho do arquivo fonte
     * @param fileType Tipo do arquivo
     * @param targetPath Caminho de destino (ou vazio para padrão)
     * @return Resultado da compilação
     */
    CompilationResult compileFile(const std::string& filePath, FileType fileType, const std::string& targetPath = "");
    
    /**
     * @brief Obtém mapeamento de um arquivo
     * @param filePath Caminho do arquivo
     * @return Informações de mapeamento ou nulo se não encontrado
     */
    std::optional<FileMapping> getFileMapping(const std::string& filePath);
    
    /**
     * @brief Obtém mapeamento por tipo de arquivo
     * @param fileType Tipo do arquivo
     * @return Lista de mapeamentos
     */
    std::vector<FileMapping> getFileMappingsByType(FileType fileType);
    
    /**
     * @brief Adiciona um callback para notificações
     * @param callback Função a ser chamada quando houver notificação
     * @return ID do callback (para remoção posterior)
     */
    int32_t addNotificationCallback(std::function<void(const UpdateNotification&)> callback);
    
    /**
     * @brief Remove um callback
     * @param callbackId ID do callback
     */
    void removeNotificationCallback(int32_t callbackId);
    
    /**
     * @brief Aplica uma alteração ao servidor em tempo real
     * @param operationId ID da operação
     * @return true se aplicada com sucesso
     */
    bool applyToLiveServer(const std::string& operationId);
    
    /**
     * @brief Reverte uma operação
     * @param operationId ID da operação
     * @return true se revertida com sucesso
     */
    bool revertOperation(const std::string& operationId);
    
    /**
     * @brief Verifica se um arquivo pode ser editado diretamente em formato compilado
     * @param filePath Caminho do arquivo
     * @return true se pode ser editado diretamente
     */
    bool canEditCompiledFile(const std::string& filePath);
    
    /**
     * @brief Verifica se uma atualização requer reinício do servidor
     * @param fileType Tipo do arquivo
     * @return true se requer reinício
     */
    bool requiresServerRestart(FileType fileType);
    
    /**
     * @brief Obtém estatísticas de atualizações
     * @return Estatísticas em formato JSON
     */
    json getUpdateStatistics();
    
    /**
     * @brief Força uma recarga de recursos no servidor
     * @param resourceType Tipo de recurso (ou vazio para todos)
     * @return true se recarga iniciada com sucesso
     */
    bool forceServerResourceReload(const std::string& resourceType = "");
    
    /**
     * @brief Atualiza configuração em tempo de execução
     * @param newConfig Nova configuração
     * @return true se configuração atualizada com sucesso
     */
    bool updateConfig(const LiveUpdateConfig& newConfig);
    
    /**
     * @brief Registra um tipo de arquivo personalizado
     * @param extension Extensão do arquivo
     * @param fileType Tipo do arquivo
     * @param canEditCompiled Se pode editar compilado
     * @param decompilerModule Módulo decompilador
     * @param compilerModule Módulo compilador
     * @return true se registrado com sucesso
     */
    bool registerCustomFileType(const std::string& extension, FileType fileType, 
                               bool canEditCompiled, const std::string& decompilerModule,
                               const std::string& compilerModule);

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade
    std::shared_ptr<WYDStudioFacade> m_facade;
    
    // Configuração
    LiveUpdateConfig m_config;
    
    // Estado do gerenciador
    std::atomic<bool> m_running;
    
    // Threads de trabalho
    std::thread m_fileWatcherThread;
    std::thread m_operationProcessorThread;
    std::thread m_serverConnectionThread;
    
    // Fila de operações
    std::queue<LiveOperationData> m_operationQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    
    // Operações em andamento e históricas
    std::map<std::string, LiveOperationData> m_activeOperations;
    std::vector<LiveOperationData> m_completedOperations;
    std::mutex m_operationsMutex;
    
    // Callbacks de notificação
    std::map<int32_t, std::function<void(const UpdateNotification&)>> m_notificationCallbacks;
    std::mutex m_callbacksMutex;
    int32_t m_nextCallbackId;
    
    // Notificações recentes
    std::vector<UpdateNotification> m_recentNotifications;
    std::mutex m_notificationsMutex;
    
    // Mapeamento de arquivos
    std::map<std::string, FileMapping> m_fileMappings;
    std::mutex m_mappingsMutex;
    
    // Métodos auxiliares
    bool initializeDirectories();
    bool loadFileMappings();
    void saveFileMappings();
    
    void fileWatcherLoop();
    void processOperation(LiveOperationData& operation);
    void operationProcessorLoop();
    void serverConnectionLoop();
    
    void addFileMapping(const FileMapping& mapping);
    void updateFileMapping(const FileMapping& mapping);
    void removeFileMapping(const std::string& filePath);
    
    void addNotification(const UpdateNotification& notification);
    void notifyCallbacks(const UpdateNotification& notification);
    
    std::string generateOperationId();
    bool createBackup(const std::string& filePath, std::string& backupPath);
    bool detectFileType(const std::string& filePath, FileType& detectedType);
    
    bool processAddOperation(LiveOperationData& operation);
    bool processModifyOperation(LiveOperationData& operation);
    bool processRemoveOperation(LiveOperationData& operation);
    bool processReplaceFileOperation(LiveOperationData& operation);
    bool processPatchFileOperation(LiveOperationData& operation);
    bool processReloadResourceOperation(LiveOperationData& operation);
    bool processExecuteScriptOperation(LiveOperationData& operation);
    bool processBroadcastNoticeOperation(LiveOperationData& operation);
    bool processRestartServiceOperation(LiveOperationData& operation);
    
    // Comunicação com o servidor
    bool connectToServer();
    bool sendToServer(const std::string& message);
    bool receiveFromServer(std::string& message);
    void handleServerMessage(const std::string& message);
    
    // Métodos de utilidade para tarefas comuns
    std::string getDecompilationTarget(const std::string& originalPath, const std::string& format);
    std::string getCompilationTarget(const std::string& sourcePath);
    bool ensureDirectoryExists(const std::string& dirPath);
    void cleanupOldBackups();
    
    // Utilitários de conversão
    static std::string fileTypeToString(FileType type);
    static FileType stringToFileType(const std::string& typeStr);
    static std::string operationTypeToString(LiveOperationType type);
    static LiveOperationType stringToOperationType(const std::string& typeStr);
    static std::string operationStatusToString(LiveOperationStatus status);
    static LiveOperationStatus stringToOperationStatus(const std::string& statusStr);
};

}}  // namespace wydstudio::core

#endif // WYDSTUDIO_LIVE_UPDATE_MANAGER_H