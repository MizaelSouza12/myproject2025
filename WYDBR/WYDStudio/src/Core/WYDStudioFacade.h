#ifndef WYDSTUDIO_FACADE_H
#define WYDSTUDIO_FACADE_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <future>
#include <nlohmann/json.hpp>

// Declarações adiante
namespace wydstudio {
    namespace decompilers {
        class AdvancedBinaryAnalyzer;
    }
    
    namespace core {
        class LiveUpdateManager;
    }
    
    namespace security {
        class SecurityGuardian;
    }
    
    namespace ui {
        class UIDesigner;
    }
}

namespace wydstudio {
namespace core {

using json = nlohmann::json;

/**
 * @brief Resultado de operação
 */
struct OperationResult {
    bool success;
    std::string message;
    json data;
};

/**
 * @brief Tipo de log
 */
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * @brief Tipo de notificação
 */
enum class NotificationType {
    INFO,
    WARNING,
    ERROR,
    SUCCESS
};

/**
 * @brief Interface principal do WYDStudio
 * 
 * Esta classe atua como um facade para todos os subsistemas do WYDStudio,
 * fornecendo uma interface unificada para o restante do aplicativo.
 */
class WYDStudioFacade {
public:
    /**
     * @brief Construtor
     */
    WYDStudioFacade();
    
    /**
     * @brief Destrutor
     */
    ~WYDStudioFacade();
    
    /**
     * @brief Inicializa o WYDStudio
     * @param configPath Caminho opcional para arquivo de configuração
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::string& configPath = "");
    
    /**
     * @brief Finaliza o WYDStudio
     */
    void shutdown();
    
    /**
     * @brief Verifica se o WYDStudio está inicializado
     * @return true se estiver inicializado
     */
    bool isInitialized() const;
    
    // ===== Acesso a Subsistemas =====
    
    /**
     * @brief Obtém o analisador binário
     * @return Referência ao analisador binário
     */
    std::shared_ptr<decompilers::AdvancedBinaryAnalyzer> getBinaryAnalyzer();
    
    /**
     * @brief Obtém o gerenciador de atualização live
     * @return Referência ao gerenciador de atualização live
     */
    std::shared_ptr<core::LiveUpdateManager> getLiveUpdateManager();
    
    /**
     * @brief Obtém o guardião de segurança
     * @return Referência ao guardião de segurança
     */
    std::shared_ptr<security::SecurityGuardian> getSecurityGuardian();
    
    /**
     * @brief Obtém o designer de UI
     * @return Referência ao designer de UI
     */
    std::shared_ptr<ui::UIDesigner> getUIDesigner();
    
    // ===== Operações com Arquivos =====
    
    /**
     * @brief Verifica se um arquivo existe
     * @param filePath Caminho do arquivo
     * @return true se existir
     */
    bool fileExists(const std::string& filePath) const;
    
    /**
     * @brief Lê um arquivo binário
     * @param filePath Caminho do arquivo
     * @return Conteúdo do arquivo ou vetor vazio se falhar
     */
    std::vector<uint8_t> readBinaryFile(const std::string& filePath) const;
    
    /**
     * @brief Lê um arquivo de texto
     * @param filePath Caminho do arquivo
     * @return Conteúdo do arquivo ou string vazia se falhar
     */
    std::string readTextFile(const std::string& filePath) const;
    
    /**
     * @brief Escreve um arquivo binário
     * @param filePath Caminho do arquivo
     * @param data Dados a serem escritos
     * @return true se escrito com sucesso
     */
    bool writeBinaryFile(const std::string& filePath, const std::vector<uint8_t>& data);
    
    /**
     * @brief Escreve um arquivo de texto
     * @param filePath Caminho do arquivo
     * @param content Conteúdo a ser escrito
     * @return true se escrito com sucesso
     */
    bool writeTextFile(const std::string& filePath, const std::string& content);
    
    /**
     * @brief Cria um diretório
     * @param dirPath Caminho do diretório
     * @param recursive Se verdadeiro, cria diretórios intermediários
     * @return true se criado com sucesso
     */
    bool createDirectory(const std::string& dirPath, bool recursive = true);
    
    /**
     * @brief Copia um arquivo
     * @param sourcePath Caminho de origem
     * @param destPath Caminho de destino
     * @return true se copiado com sucesso
     */
    bool copyFile(const std::string& sourcePath, const std::string& destPath);
    
    /**
     * @brief Move um arquivo
     * @param sourcePath Caminho de origem
     * @param destPath Caminho de destino
     * @return true se movido com sucesso
     */
    bool moveFile(const std::string& sourcePath, const std::string& destPath);
    
    /**
     * @brief Remove um arquivo
     * @param filePath Caminho do arquivo
     * @return true se removido com sucesso
     */
    bool removeFile(const std::string& filePath);
    
    /**
     * @brief Remove um diretório
     * @param dirPath Caminho do diretório
     * @param recursive Se verdadeiro, remove todo o conteúdo
     * @return true se removido com sucesso
     */
    bool removeDirectory(const std::string& dirPath, bool recursive = true);
    
    /**
     * @brief Lista arquivos em um diretório
     * @param dirPath Caminho do diretório
     * @param pattern Padrão de filtro (opcional)
     * @param recursive Se verdadeiro, lista recursivamente
     * @return Lista de caminhos de arquivos
     */
    std::vector<std::string> listFiles(const std::string& dirPath, 
                                     const std::string& pattern = "", 
                                     bool recursive = false) const;
    
    /**
     * @brief Lista diretórios em um diretório
     * @param dirPath Caminho do diretório
     * @param recursive Se verdadeiro, lista recursivamente
     * @return Lista de caminhos de diretórios
     */
    std::vector<std::string> listDirectories(const std::string& dirPath, 
                                          bool recursive = false) const;
    
    // ===== Operações com Configuração =====
    
    /**
     * @brief Obtém valor de configuração
     * @param key Chave da configuração
     * @param defaultValue Valor padrão se não encontrado
     * @return Valor da configuração ou valor padrão
     */
    template<typename T>
    T getConfigValue(const std::string& key, const T& defaultValue = T()) const;
    
    /**
     * @brief Define valor de configuração
     * @param key Chave da configuração
     * @param value Valor da configuração
     * @return true se definido com sucesso
     */
    template<typename T>
    bool setConfigValue(const std::string& key, const T& value);
    
    /**
     * @brief Salva a configuração
     * @param configPath Caminho do arquivo (ou vazio para padrão)
     * @return true se salvo com sucesso
     */
    bool saveConfig(const std::string& configPath = "");
    
    /**
     * @brief Carrega a configuração
     * @param configPath Caminho do arquivo (ou vazio para padrão)
     * @return true se carregado com sucesso
     */
    bool loadConfig(const std::string& configPath = "");
    
    // ===== Sistema de Log =====
    
    /**
     * @brief Registra mensagem de log
     * @param level Nível do log
     * @param message Mensagem
     * @param component Componente que gerou o log
     */
    void log(LogLevel level, const std::string& message, const std::string& component = "Main");
    
    /**
     * @brief Registra mensagem de debug
     * @param message Mensagem
     * @param component Componente que gerou o log
     */
    void logDebug(const std::string& message, const std::string& component = "Main");
    
    /**
     * @brief Registra mensagem de informação
     * @param message Mensagem
     * @param component Componente que gerou o log
     */
    void logInfo(const std::string& message, const std::string& component = "Main");
    
    /**
     * @brief Registra mensagem de aviso
     * @param message Mensagem
     * @param component Componente que gerou o log
     */
    void logWarning(const std::string& message, const std::string& component = "Main");
    
    /**
     * @brief Registra mensagem de erro
     * @param message Mensagem
     * @param component Componente que gerou o log
     */
    void logError(const std::string& message, const std::string& component = "Main");
    
    /**
     * @brief Registra mensagem crítica
     * @param message Mensagem
     * @param component Componente que gerou o log
     */
    void logCritical(const std::string& message, const std::string& component = "Main");
    
    /**
     * @brief Obtém entradas de log recentes
     * @param count Número máximo de entradas
     * @param level Nível mínimo de log
     * @return Entradas de log em formato JSON
     */
    json getRecentLogs(int count = 100, LogLevel level = LogLevel::INFO);
    
    // ===== Sistema de Notificação =====
    
    /**
     * @brief Envia notificação
     * @param type Tipo de notificação
     * @param message Mensagem
     * @param title Título (opcional)
     * @param autoHide Se verdadeiro, desaparece automaticamente
     */
    void notify(NotificationType type, const std::string& message, 
               const std::string& title = "", bool autoHide = true);
    
    /**
     * @brief Registra callback para notificações
     * @param callback Função a ser chamada quando houver notificação
     * @return ID do callback
     */
    int registerNotificationCallback(
        std::function<void(NotificationType, const std::string&, const std::string&)> callback);
    
    /**
     * @brief Remove callback de notificação
     * @param callbackId ID do callback
     */
    void unregisterNotificationCallback(int callbackId);
    
    // ===== Operações Assíncronas =====
    
    /**
     * @brief Executa uma operação assíncrona
     * @param operation Função que representa a operação
     * @return Future para obter o resultado
     */
    template<typename T>
    std::future<T> runAsync(std::function<T()> operation);
    
    /**
     * @brief Registra uma tarefa para execução periódica
     * @param task Função que representa a tarefa
     * @param intervalMs Intervalo em milissegundos
     * @return ID da tarefa
     */
    int scheduleTask(std::function<void()> task, int intervalMs);
    
    /**
     * @brief Cancela uma tarefa agendada
     * @param taskId ID da tarefa
     */
    void cancelTask(int taskId);
    
    // ===== Utilitários =====
    
    /**
     * @brief Obtém caminho base do aplicativo
     * @return Caminho base
     */
    std::string getBasePath() const;
    
    /**
     * @brief Obtém caminho para dados do aplicativo
     * @return Caminho para dados
     */
    std::string getDataPath() const;
    
    /**
     * @brief Obtém caminho para arquivos temporários
     * @return Caminho para temporários
     */
    std::string getTempPath() const;
    
    /**
     * @brief Obtém caminho para configuração
     * @return Caminho para configuração
     */
    std::string getConfigPath() const;
    
    /**
     * @brief Gera ID único
     * @param prefix Prefixo para o ID
     * @return ID único
     */
    std::string generateUniqueId(const std::string& prefix = "") const;
    
    /**
     * @brief Converte struct para JSON
     * @param data Objeto a ser convertido
     * @return JSON resultante
     */
    template<typename T>
    json toJson(const T& data) const;
    
    /**
     * @brief Converte JSON para struct
     * @param jsonData JSON a ser convertido
     * @param data Objeto onde será armazenado o resultado
     * @return true se convertido com sucesso
     */
    template<typename T>
    bool fromJson(const json& jsonData, T& data) const;

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Estado de inicialização
    bool m_initialized;
    
    // Caminhos base
    std::string m_basePath;
    std::string m_dataPath;
    std::string m_tempPath;
    std::string m_configPath;
    
    // Configuração
    json m_config;
    
    // Subsistemas
    std::shared_ptr<decompilers::AdvancedBinaryAnalyzer> m_binaryAnalyzer;
    std::shared_ptr<core::LiveUpdateManager> m_liveUpdateManager;
    std::shared_ptr<security::SecurityGuardian> m_securityGuardian;
    std::shared_ptr<ui::UIDesigner> m_uiDesigner;
    
    // Callbacks de notificação
    std::map<int, std::function<void(NotificationType, const std::string&, const std::string&)>> m_notificationCallbacks;
    int m_nextCallbackId;
    
    // Tarefas agendadas
    std::map<int, std::function<void()>> m_scheduledTasks;
    int m_nextTaskId;
    
    // Auxiliares
    bool initializeSubsystems();
    bool initializeDirectories();
    void cleanupTempFiles();
};

}} // namespace wydstudio::core

#endif // WYDSTUDIO_FACADE_H