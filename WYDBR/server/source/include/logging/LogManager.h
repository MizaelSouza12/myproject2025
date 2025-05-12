/**
 * LogManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/logging/LogManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _LOG_MANAGER_H_
#define _LOG_MANAGER_H_

/**
 * @file LogManager.h
 * @brief Sistema de logging do servidor WYD - Baseado no código original
 * 
 * Implementa um sistema completo de logging para o servidor,
 * permitindo registro e análise de eventos, erros, transações e ações.
 * Mantém compatibilidade com o sistema original do WYD.
 */

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <fstream>
#include <functional>
#include <chrono>

namespace wydbr {

/**
 * @brief Nível de log
 */
enum LogLevel {
    LOG_DEBUG = 0,      // Debug (informações detalhadas, só aparecem em debug)
    LOG_INFO = 1,       // Informação (informações gerais)
    LOG_WARNING = 2,    // Aviso (condições potencialmente problemáticas)
    LOG_ERROR = 3,      // Erro (erros que não interrompem o funcionamento)
    LOG_CRITICAL = 4    // Crítico (erros graves que interrompem o funcionamento)
};

/**
 * @brief Categoria de log
 */
enum LogCategory {
    LOG_CATEGORY_SYSTEM = 0,     // Sistema geral
    LOG_CATEGORY_PLAYER = 1,     // Jogadores
    LOG_CATEGORY_NETWORK = 2,    // Rede
    LOG_CATEGORY_DATABASE = 3,   // Banco de dados
    LOG_CATEGORY_ITEM = 4,       // Itens
    LOG_CATEGORY_COMBAT = 5,     // Combate
    LOG_CATEGORY_QUEST = 6,      // Quests
    LOG_CATEGORY_CHAT = 7,       // Chat
    LOG_CATEGORY_ECONOMY = 8,    // Economia
    LOG_CATEGORY_ADMIN = 9,      // Administração
    LOG_CATEGORY_GUILD = 10,     // Guildas
    LOG_CATEGORY_AUTH = 11,      // Autenticação
    LOG_CATEGORY_SCRIPT = 12,    // Scripts
    LOG_CATEGORY_WORLD = 13,     // Mundo
    LOG_CATEGORY_EVENT = 14,     // Eventos
    LOG_CATEGORY_SECURITY = 15,  // Segurança
    LOG_CATEGORY_CUSTOM1 = 16,   // Personalizado 1
    LOG_CATEGORY_CUSTOM2 = 17,   // Personalizado 2
    LOG_CATEGORY_CUSTOM3 = 18,   // Personalizado 3
    LOG_CATEGORY_CUSTOM4 = 19    // Personalizado 4
};

/**
 * @brief Tipo de operação para auditoria
 */
enum AuditActionType {
    AUDIT_ACTION_LOGIN = 0,         // Login
    AUDIT_ACTION_LOGOUT = 1,        // Logout
    AUDIT_ACTION_ITEM_CREATE = 2,   // Criação de item
    AUDIT_ACTION_ITEM_DELETE = 3,   // Deleção de item
    AUDIT_ACTION_ITEM_MODIFY = 4,   // Modificação de item
    AUDIT_ACTION_ITEM_MOVE = 5,     // Movimentação de item
    AUDIT_ACTION_GOLD_ADD = 6,      // Adição de ouro
    AUDIT_ACTION_GOLD_REMOVE = 7,   // Remoção de ouro
    AUDIT_ACTION_EXP_ADD = 8,       // Adição de experiência
    AUDIT_ACTION_LEVEL_UP = 9,      // Subida de nível
    AUDIT_ACTION_SKILL_LEARN = 10,  // Aprendizado de habilidade
    AUDIT_ACTION_COMMAND = 11,      // Execução de comando
    AUDIT_ACTION_TRADE = 12,        // Comércio
    AUDIT_ACTION_SHOP = 13,         // Loja
    AUDIT_ACTION_CHAT = 14,         // Chat
    AUDIT_ACTION_PLAYER_KILL = 15,  // Kill de jogador
    AUDIT_ACTION_DEATH = 16,        // Morte
    AUDIT_ACTION_TELEPORT = 17,     // Teleporte
    AUDIT_ACTION_QUEST = 18,        // Quest
    AUDIT_ACTION_GUILD = 19,        // Guilda
    AUDIT_ACTION_SETTINGS = 20,     // Configurações
    AUDIT_ACTION_CUSTOM1 = 21,      // Personalizado 1
    AUDIT_ACTION_CUSTOM2 = 22,      // Personalizado 2
    AUDIT_ACTION_CUSTOM3 = 23,      // Personalizado 3
    AUDIT_ACTION_CUSTOM4 = 24       // Personalizado 4
};

/**
 * @brief Entrada de log
 */
struct LogEntry {
    uint32_t timestamp;          // Timestamp
    LogLevel level;              // Nível de log
    LogCategory category;        // Categoria de log
    std::string message;         // Mensagem
    std::string source;          // Fonte (arquivo:linha)
    std::string threadId;        // ID da thread
    
    LogEntry() : timestamp(0), level(LOG_INFO), category(LOG_CATEGORY_SYSTEM) {}
    
    LogEntry(LogLevel _level, LogCategory _category, const std::string& _message,
             const std::string& _source = "", const std::string& _threadId = "")
        : level(_level), category(_category), message(_message),
          source(_source), threadId(_threadId) {
        timestamp = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    }
};

/**
 * @brief Entrada de auditoria
 */
struct AuditEntry {
    uint32_t timestamp;          // Timestamp
    int playerId;                // ID do jogador
    std::string playerName;      // Nome do jogador
    std::string playerIp;        // IP do jogador
    AuditActionType actionType;  // Tipo de ação
    std::string actionData;      // Dados da ação
    std::string targetData;      // Dados do alvo da ação
    std::string result;          // Resultado da ação
    
    AuditEntry() : timestamp(0), playerId(0), actionType(AUDIT_ACTION_LOGIN) {}
    
    AuditEntry(int _playerId, const std::string& _playerName,
               const std::string& _playerIp, AuditActionType _actionType,
               const std::string& _actionData, const std::string& _targetData = "",
               const std::string& _result = "")
        : playerId(_playerId), playerName(_playerName), playerIp(_playerIp),
          actionType(_actionType), actionData(_actionData),
          targetData(_targetData), result(_result) {
        timestamp = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    }
};

/**
 * @brief Status do servidor
 */
struct ServerStatus {
    uint32_t startTime;          // Timestamp de início
    uint32_t currentTime;        // Timestamp atual
    int connectedPlayers;        // Jogadores conectados
    int maxPlayers;              // Número máximo de jogadores
    int activeThreads;           // Threads ativas
    int activeMobs;              // Mobs ativos
    long long totalMemory;       // Memória total
    long long usedMemory;        // Memória usada
    float cpuUsage;              // Uso de CPU
    int activeSessions;          // Sessões ativas
    int networkQueueSize;        // Tamanho da fila de rede
    
    ServerStatus() : startTime(0), currentTime(0), connectedPlayers(0),
                   maxPlayers(0), activeThreads(0), activeMobs(0),
                   totalMemory(0), usedMemory(0), cpuUsage(0.0f),
                   activeSessions(0), networkQueueSize(0) {}
};

/**
 * @brief Estatísticas de log
 */
struct LogStats {
    int totalLogs;                                    // Total de logs
    std::map<LogLevel, int> logsByLevel;              // Logs por nível
    std::map<LogCategory, int> logsByCategory;        // Logs por categoria
    std::map<std::string, int> messageFrequency;      // Frequência de mensagens
    std::map<std::string, int> sourceFrequency;       // Frequência de fontes
    
    LogStats() /**
 * totalLogs
 * 
 * Implementa a funcionalidade totalLogs conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 : totalLogs(0){}
};

/**
 * @brief Estatísticas de auditoria
 */
struct AuditStats {
    int totalAudits;                                 // Total de auditorias
    std::map<AuditActionType, int> auditsByType;     // Auditorias por tipo
    std::map<int, int> auditsByPlayer;               // Auditorias por jogador
    std::map<std::string, int> auditsByIp;           // Auditorias por IP
    
    AuditStats() /**
 * totalAudits
 * 
 * Implementa a funcionalidade totalAudits conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 : totalAudits(0){}
};

/**
 * @brief Gerenciador de log global
 * 
 * Classe singleton responsável pelo gerenciamento de todos os
 * logs do servidor WYD, incluindo logs de sistema, auditoria e estatísticas.
 */
/**
 * Classe WYDLogManager
 * 
 * Esta classe implementa a funcionalidade WYDLogManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class LogManager {
public:
    /**
     * @brief Obtém a instância única do LogManager
     * @return Referência para a instância
     */
    static LogManager& GetInstance();

    /**
     * @brief Inicializa o gerenciador de log
     * @param logDir Diretório para arquivos de log
     * @param configPath Caminho para arquivo de configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& logDir, const std::string& configPath = "");

    /**
     * @brief Finaliza o gerenciador de log
     */
    void Finalize();
    
    /**
     * @brief Registra uma mensagem de log
     * @param level Nível de log
     * @param category Categoria de log
     * @param message Mensagem
     * @param source Fonte (arquivo:linha)
     * @param threadId ID da thread
     */
    void Log(LogLevel level, LogCategory category, const std::string& message,
             const std::string& source = "", const std::string& threadId = "");
    
    /**
     * @brief Registra uma entrada de auditoria
     * @param playerId ID do jogador
     * @param playerName Nome do jogador
     * @param playerIp IP do jogador
     * @param actionType Tipo de ação
     * @param actionData Dados da ação
     * @param targetData Dados do alvo da ação
     * @param result Resultado da ação
     */
    void Audit(int playerId, const std::string& playerName,
              const std::string& playerIp, AuditActionType actionType,
              const std::string& actionData, const std::string& targetData = "",
              const std::string& result = "");
    
    /**
     * @brief Registra mensagem de log de nível debug
     * @param category Categoria de log
     * @param message Mensagem
     * @param source Fonte (arquivo:linha)
     */
    void Debug(LogCategory category, const std::string& message, const std::string& source = "");
    
    /**
     * @brief Registra mensagem de log de nível info
     * @param category Categoria de log
     * @param message Mensagem
     * @param source Fonte (arquivo:linha)
     */
    void Info(LogCategory category, const std::string& message, const std::string& source = "");
    
    /**
     * @brief Registra mensagem de log de nível warning
     * @param category Categoria de log
     * @param message Mensagem
     * @param source Fonte (arquivo:linha)
     */
    void Warning(LogCategory category, const std::string& message, const std::string& source = "");
    
    /**
     * @brief Registra mensagem de log de nível error
     * @param category Categoria de log
     * @param message Mensagem
     * @param source Fonte (arquivo:linha)
     */
    void Error(LogCategory category, const std::string& message, const std::string& source = "");
    
    /**
     * @brief Registra mensagem de log de nível critical
     * @param category Categoria de log
     * @param message Mensagem
     * @param source Fonte (arquivo:linha)
     */
    void Critical(LogCategory category, const std::string& message, const std::string& source = "");
    
    /**
     * @brief Define o nível mínimo de log
     * @param level Nível mínimo
     */
    void SetLogLevel(LogLevel level);
    
    /**
     * @brief Define as categorias habilitadas
     * @param categories Vetor de categorias habilitadas
     */
    void SetEnabledCategories(const std::vector<LogCategory>& categories);
    
    /**
     * @brief Verifica se uma categoria está habilitada
     * @param category Categoria a verificar
     * @return true se a categoria estiver habilitada
     */
    bool IsCategoryEnabled(LogCategory category);
    
    /**
     * @brief Define o callback para log
     * @param callback Função de callback
     */
    void SetLogCallback(std::function<void(const LogEntry&)> callback);
    
    /**
     * @brief Define o callback para auditoria
     * @param callback Função de callback
     */
    void SetAuditCallback(std::function<void(const AuditEntry&)> callback);
    
    /**
     * @brief Obtém entradas de log
     * @param level Nível mínimo
     * @param category Categoria (LOG_CATEGORY_SYSTEM para todas)
     * @param maxEntries Número máximo de entradas
     * @param startTime Timestamp inicial (0 para todos)
     * @param endTime Timestamp final (0 para todos)
     * @return Vetor de entradas de log
     */
    std::vector<LogEntry> GetLogEntries(LogLevel level = LOG_INFO, 
                                      LogCategory category = LOG_CATEGORY_SYSTEM,
                                      int maxEntries = 100,
                                      uint32_t startTime = 0,
                                      uint32_t endTime = 0);
    
    /**
     * @brief Obtém entradas de auditoria
     * @param actionType Tipo de ação (AUDIT_ACTION_LOGIN para todas)
     * @param playerId ID do jogador (0 para todos)
     * @param maxEntries Número máximo de entradas
     * @param startTime Timestamp inicial (0 para todos)
     * @param endTime Timestamp final (0 para todos)
     * @return Vetor de entradas de auditoria
     */
    std::vector<AuditEntry> GetAuditEntries(AuditActionType actionType = AUDIT_ACTION_LOGIN,
                                          int playerId = 0,
                                          int maxEntries = 100,
                                          uint32_t startTime = 0,
                                          uint32_t endTime = 0);
    
    /**
     * @brief Pesquisa logs por texto
     * @param searchText Texto a pesquisar
     * @param maxEntries Número máximo de entradas
     * @return Vetor de entradas de log
     */
    std::vector<LogEntry> SearchLogs(const std::string& searchText, int maxEntries = 100);
    
    /**
     * @brief Pesquisa auditoria por texto
     * @param searchText Texto a pesquisar
     * @param maxEntries Número máximo de entradas
     * @return Vetor de entradas de auditoria
     */
    std::vector<AuditEntry> SearchAudit(const std::string& searchText, int maxEntries = 100);
    
    /**
     * @brief Obtém estatísticas de log
     * @param startTime Timestamp inicial (0 para todos)
     * @param endTime Timestamp final (0 para todos)
     * @return Estatísticas de log
     */
    LogStats GetLogStats(uint32_t startTime = 0, uint32_t endTime = 0);
    
    /**
     * @brief Obtém estatísticas de auditoria
     * @param startTime Timestamp inicial (0 para todos)
     * @param endTime Timestamp final (0 para todos)
     * @return Estatísticas de auditoria
     */
    AuditStats GetAuditStats(uint32_t startTime = 0, uint32_t endTime = 0);
    
    /**
     * @brief Obtém status do servidor
     * @return Status do servidor
     */
    ServerStatus GetServerStatus();
    
    /**
     * @brief Atualiza status do servidor
     * @param status Status do servidor
     */
    void UpdateServerStatus(const ServerStatus& status);
    
    /**
     * @brief Limpa logs antigos
     * @param days Número de dias a manter
     * @return Número de entradas removidas
     */
    int CleanupOldLogs(int days);
    
    /**
     * @brief Rotaciona arquivos de log
     * @return true se rotacionado com sucesso
     */
    bool RotateLogs();
    
    /**
     * @brief Define um nome personalizado para categoria
     * @param category Categoria
     * @param name Nome personalizado
     */
    void SetCategoryName(LogCategory category, const std::string& name);
    
    /**
     * @brief Obtém o nome de uma categoria
     * @param category Categoria
     * @return Nome da categoria
     */
    std::string GetCategoryName(LogCategory category);
    
    /**
     * @brief Define um nome personalizado para tipo de ação
     * @param actionType Tipo de ação
     * @param name Nome personalizado
     */
    void SetActionTypeName(AuditActionType actionType, const std::string& name);
    
    /**
     * @brief Obtém o nome de um tipo de ação
     * @param actionType Tipo de ação
     * @return Nome do tipo de ação
     */
    std::string GetActionTypeName(AuditActionType actionType);
    
    /**
     * @brief Exporta logs para um arquivo
     * @param filePath Caminho do arquivo
     * @param format Formato do arquivo ("csv", "json", "txt")
     * @param level Nível mínimo
     * @param category Categoria (LOG_CATEGORY_SYSTEM para todas)
     * @param startTime Timestamp inicial (0 para todos)
     * @param endTime Timestamp final (0 para todos)
     * @return true se exportado com sucesso
     */
    bool ExportLogs(const std::string& filePath, const std::string& format,
                  LogLevel level = LOG_INFO,
                  LogCategory category = LOG_CATEGORY_SYSTEM,
                  uint32_t startTime = 0,
                  uint32_t endTime = 0);
    
    /**
     * @brief Exporta auditoria para um arquivo
     * @param filePath Caminho do arquivo
     * @param format Formato do arquivo ("csv", "json", "txt")
     * @param actionType Tipo de ação (AUDIT_ACTION_LOGIN para todas)
     * @param playerId ID do jogador (0 para todos)
     * @param startTime Timestamp inicial (0 para todos)
     * @param endTime Timestamp final (0 para todos)
     * @return true se exportado com sucesso
     */
    bool ExportAudit(const std::string& filePath, const std::string& format,
                   AuditActionType actionType = AUDIT_ACTION_LOGIN,
                   int playerId = 0,
                   uint32_t startTime = 0,
                   uint32_t endTime = 0);

/**
 * LogManager
 * 
 * Implementa a funcionalidade LogManager conforme especificação original.
 * @return Retorna private:
 */


private:
    LogManager();  // Construtor privado (singleton)
    ~LogManager(); // Destrutor privado (singleton)
    
    /**
     * @brief Converte um timestamp para string formatada
     * @param timestamp Timestamp
     * @return String formatada
     */
    std::string FormatTimestamp(uint32_t timestamp);
    
    /**
     * @brief Obtém a string de nível de log
     * @param level Nível de log
     * @return String do nível
     */
    std::string GetLevelString(LogLevel level);
    
    /**
     * @brief Escreve uma entrada de log no arquivo
     * @param entry Entrada de log
     */
    void WriteLogToFile(const LogEntry& entry);
    
    /**
     * @brief Escreve uma entrada de auditoria no arquivo
     * @param entry Entrada de auditoria
     */
    void WriteAuditToFile(const AuditEntry& entry);
    
    /**
     * @brief Carrega configurações
     * @param configPath Caminho para arquivo de configuração
     * @return true se carregado com sucesso
     */
    bool LoadConfig(const std::string& configPath);
    
    /**
     * @brief Cria um arquivo de log com cabeçalho
     * @param filePath Caminho do arquivo
     * @param header Cabeçalho
     * @return true se criado com sucesso
     */
    bool CreateLogFile(const std::string& filePath, const std::string& header);
    
    // Atributos privados
    static LogManager* instance_;                             // Instância única (singleton)
    bool initialized_;                                        // Flag de inicialização
    
    // Configurações
    std::string logDir_;                                      // Diretório de logs
    LogLevel minLogLevel_;                                    // Nível mínimo de log
    std::vector<LogCategory> enabledCategories_;              // Categorias habilitadas
    bool consoleOutput_;                                      // Saída para console
    bool fileOutput_;                                         // Saída para arquivo
    bool databaseOutput_;                                     // Saída para banco de dados
    bool auditEnabled_;                                       // Auditoria habilitada
    int logRotationDays_;                                     // Dias para rotação de logs
    int maxLogEntries_;                                       // Número máximo de entradas em memória
    
    // Arquivos de log
    std::ofstream logFile_;                                   // Arquivo de log
    std::ofstream auditFile_;                                 // Arquivo de auditoria
    std::string logFilePath_;                                 // Caminho do arquivo de log
    std::string auditFilePath_;                               // Caminho do arquivo de auditoria
    
    // Dados
    std::vector<LogEntry> logEntries_;                        // Entradas de log em memória
    std::vector<AuditEntry> auditEntries_;                    // Entradas de auditoria em memória
    ServerStatus serverStatus_;                               // Status do servidor
    LogStats logStats_;                                       // Estatísticas de log
    AuditStats auditStats_;                                   // Estatísticas de auditoria
    
    // Nomes personalizados
    std::map<LogCategory, std::string> categoryNames_;        // Nomes de categorias
    std::map<AuditActionType, std::string> actionTypeNames_;  // Nomes de tipos de ação
    
    // Callbacks
    std::function<void(const LogEntry&)> logCallback_;        // Callback para log
    std::function<void(const AuditEntry&)> auditCallback_;    // Callback para auditoria
    
    // Mutex para acesso seguro
    std::mutex logMutex_;                                     // Mutex para acesso ao log
    
    // Desabilita cópias (singleton)
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
};

// Macros para facilitar o logging
#ifdef _DEBUG
#define LOG_DEBUG(category, message) \
    wydbr::LogManager::GetInstance().Debug(category, message, __FILE__ ":" + std::to_string(__LINE__))
#else
#define LOG_DEBUG(category, message)
#endif

#define LOG_INFO(category, message) \
    wydbr::LogManager::GetInstance().Info(category, message, __FILE__ ":" + std::to_string(__LINE__))

#define LOG_WARNING(category, message) \
    wydbr::LogManager::GetInstance().Warning(category, message, __FILE__ ":" + std::to_string(__LINE__))

#define LOG_ERROR(category, message) \
    wydbr::LogManager::GetInstance().Error(category, message, __FILE__ ":" + std::to_string(__LINE__))

#define LOG_CRITICAL(category, message) \
    wydbr::LogManager::GetInstance().Critical(category, message, __FILE__ ":" + std::to_string(__LINE__))

// Macro para facilitar auditoria
#define AUDIT(playerId, playerName, playerIp, actionType, actionData, targetData, result) \
    wydbr::LogManager::GetInstance().Audit(playerId, playerName, playerIp, actionType, actionData, targetData, result)

} // namespace wydbr

#endif // _LOG_MANAGER_H_