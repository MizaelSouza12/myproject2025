/**
 * Logger.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/core/Logger.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

/**
 * @file Logger.h
 * @brief Sistema de logging avançado para o WYDBRASIL
 * 
 * Esta classe implementa um sistema de logging flexível e configurável
 * para rastreamento de eventos, depuração e monitoramento do servidor.
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <mutex>
#include <memory>
#include <chrono>
#include <thread>
#include <functional>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <ctime>
#include <iomanip>

namespace wydbr {


/**
 * @enum LogLevel
 * @brief Níveis de severidade para mensagens de log
 */
enum /**
 * Classe WYDLogLevel
 * 
 * Esta classe implementa a funcionalidade WYDLogLevel conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class LogLevel {
    TRACE,      // Informação detalhada para desenvolvimento/depuração
    DEBUG,      // Informação de depuração
    INFO,       // Informação geral
    WARNING,    // Aviso (potencial problema)
    ERROR,      // Erro (problema significativo)
    CRITICAL,   // Erro crítico (problema grave)
    SILENT      // Nível especial para desativar logs
};

/**
 * @struct LogEntry
 * @brief Estrutura para uma entrada de log
 */
struct LogEntry {
    LogLevel level;                           // Nível de severidade
    std::string message;                      // Mensagem do log
    std::string component;                    // Componente que gerou o log
    std::chrono::system_clock::time_point timestamp; // Timestamp da mensagem
    std::thread::id threadId;                 // ID da thread
    std::string file;                         // Arquivo fonte
    int line;                                 // Linha do arquivo
    std::unordered_map<std::string, std::string> attributes; // Atributos adicionais
    
    LogEntry() : level(LogLevel::INFO), line(0) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& comp = "")
        : level(lvl), message(msg), component(comp), 
          timestamp(std::chrono::system_clock::now()),
          threadId(std::this_thread::get_id()), line(0) {}
};

/**
 * @class LogSink
 * @brief Interface base para destinos de log
 */
/**
 * Classe WYDLogSink
 * 
 * Esta classe implementa a funcionalidade WYDLogSink conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class LogSink {
public:
    virtual ~LogSink() = default;
    
    /**
     * @brief Processa uma entrada de log
     * @param entry Entrada de log
     */
    virtual void process(const LogEntry& entry) = 0;
    
    /**
     * @brief Verifica se o sink aceita um nível de log
     * @param level Nível de log
     * @return true se o nível é aceito
     */
    virtual bool acceptsLevel(LogLevel level) const = 0;
    
    /**
     * @brief Fecha o sink, liberando recursos
     */
    virtual void close() = 0;
};

/**
 * @class ConsoleSink
 * @brief Sink para enviar logs ao console
 */
/**
 * Classe WYDConsoleSink
 * 
 * Esta classe implementa a funcionalidade WYDConsoleSink conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ConsoleSink : public LogSink {
public:
    /**
     * @brief Construtor
     * @param minLevel Nível mínimo para logar
     * @param colorized Usar cores no console
     */
    ConsoleSink(LogLevel minLevel = LogLevel::INFO, bool colorized = true);
    
    /**
 * process
 * 
 * Implementa a funcionalidade process conforme especificação original.
 * @param entry Parâmetro entry
 */

    
    void process(const LogEntry& entry) override;
    bool acceptsLevel(LogLevel level) const override;
    /**
 * close
 * 
 * Implementa a funcionalidade close conforme especificação original.
 */

    void close() override;
    
private:
    LogLevel m_minLevel;
    bool m_colorized;
    std::mutex m_mutex;
};

/**
 * @class FileSink
 * @brief Sink para enviar logs a um arquivo
 */
/**
 * Classe WYDFileSink
 * 
 * Esta classe implementa a funcionalidade WYDFileSink conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class FileSink : public LogSink {
public:
    /**
     * @brief Construtor
     * @param filename Nome do arquivo
     * @param minLevel Nível mínimo para logar
     * @param rotate Rotacionar arquivos de log
     * @param maxSize Tamanho máximo em bytes para rotação
     * @param maxFiles Número máximo de arquivos para rotação
     */
    FileSink(const std::string& filename, LogLevel minLevel = LogLevel::INFO,
            bool rotate = true, size_t maxSize = 10 * 1024 * 1024, int maxFiles = 5);
    
    /**
 * process
 * 
 * Implementa a funcionalidade process conforme especificação original.
 * @param entry Parâmetro entry
 */

    
    void process(const LogEntry& entry) override;
    bool acceptsLevel(LogLevel level) const override;
    /**
 * close
 * 
 * Implementa a funcionalidade close conforme especificação original.
 */

    void close() override;
    
private:
    std::string m_filename;
    LogLevel m_minLevel;
    bool m_rotate;
    size_t m_maxSize;
    int m_maxFiles;
    std::ofstream m_file;
    size_t m_currentSize;
    std::mutex m_mutex;
    
    /**
 * openFile
 * 
 * Implementa a funcionalidade openFile conforme especificação original.
 * @return Retorna bool
 */

    
    bool openFile();
    /**
 * rotateFiles
 * 
 * Implementa a funcionalidade rotateFiles conforme especificação original.
 */

    void rotateFiles();
};

/**
 * @class CallbackSink
 * @brief Sink para enviar logs a uma função de callback
 */
/**
 * Classe WYDCallbackSink
 * 
 * Esta classe implementa a funcionalidade WYDCallbackSink conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CallbackSink : public LogSink {
public:
    /**
     * @brief Tipo para função de callback
     */
    using Callback = std::function<void(const LogEntry&)>;
    
    /**
     * @brief Construtor
     * @param callback Função de callback
     * @param minLevel Nível mínimo para logar
     */
    CallbackSink(Callback callback, LogLevel minLevel = LogLevel::INFO);
    
    /**
 * process
 * 
 * Implementa a funcionalidade process conforme especificação original.
 * @param entry Parâmetro entry
 */

    
    void process(const LogEntry& entry) override;
    bool acceptsLevel(LogLevel level) const override;
    /**
 * close
 * 
 * Implementa a funcionalidade close conforme especificação original.
 */

    void close() override;
    
private:
    Callback m_callback;
    LogLevel m_minLevel;
    std::mutex m_mutex;
};

/**
 * @class Logger
 * @brief Sistema de logging avançado
 * 
 * Esta classe implementa um sistema de logging flexível e configurável
 * para rastreamento de eventos, depuração e monitoramento do servidor.
 */
/**
 * Classe WYDLogger
 * 
 * Esta classe implementa a funcionalidade WYDLogger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Logger {
public:
    /**
     * @brief Obtém a instância única do Logger (Singleton)
     * @return Referência para a instância única
     */
    static Logger& getInstance();
    
    /**
     * @brief Inicializa o sistema de log
     * @param defaultLevel Nível padrão para mensagens sem nível específico
     * @param asyncMode Usar modo assíncrono para processamento de logs
     * @param flushInterval Intervalo para flush automático em ms (modo assíncrono)
     * @return true se inicializado com sucesso
     */
    bool initialize(LogLevel defaultLevel = LogLevel::INFO, bool asyncMode = true, 
                  int flushInterval = 1000);
    
    /**
     * @brief Finaliza o sistema de log
     */
    void shutdown();
    
    /**
     * @brief Adiciona um sink ao logger
     * @param sink Ponteiro para o sink
     */
    void addSink(std::shared_ptr<LogSink> sink);
    
    /**
     * @brief Remove um sink do logger
     * @param sink Ponteiro para o sink
     * @return true se removido com sucesso
     */
    bool removeSink(std::shared_ptr<LogSink> sink);
    
    /**
     * @brief Define o nível global mínimo para logar
     * @param level Nível mínimo
     */
    void setGlobalLevel(LogLevel level);
    
    /**
     * @brief Define o nível mínimo para um componente específico
     * @param component Nome do componente
     * @param level Nível mínimo
     */
    void setComponentLevel(const std::string& component, LogLevel level);
    
    /**
     * @brief Obtém o nível mínimo para um componente específico
     * @param component Nome do componente
     * @return Nível mínimo
     */
    LogLevel getComponentLevel(const std::string& component) const;
    
    /**
     * @brief Cria um sink para console
     * @param minLevel Nível mínimo para logar
     * @param colorized Usar cores no console
     * @return Ponteiro para o sink criado
     */
    std::shared_ptr<LogSink> createConsoleSink(LogLevel minLevel = LogLevel::INFO, bool colorized = true);
    
    /**
     * @brief Cria um sink para arquivo
     * @param filename Nome do arquivo
     * @param minLevel Nível mínimo para logar
     * @param rotate Rotacionar arquivos de log
     * @param maxSize Tamanho máximo em bytes para rotação
     * @param maxFiles Número máximo de arquivos para rotação
     * @return Ponteiro para o sink criado
     */
    std::shared_ptr<LogSink> createFileSink(const std::string& filename, LogLevel minLevel = LogLevel::INFO,
                                          bool rotate = true, size_t maxSize = 10 * 1024 * 1024, int maxFiles = 5);
    
    /**
     * @brief Cria um sink para callback
     * @param callback Função de callback
     * @param minLevel Nível mínimo para logar
     * @return Ponteiro para o sink criado
     */
    std::shared_ptr<LogSink> createCallbackSink(CallbackSink::Callback callback, LogLevel minLevel = LogLevel::INFO);
    
    /**
     * @brief Loga uma mensagem
     * @param level Nível de severidade
     * @param message Mensagem do log
     * @param component Componente que gerou o log
     * @param file Arquivo fonte
     * @param line Linha do arquivo
     */
    void log(LogLevel level, const std::string& message, const std::string& component = "",
           const std::string& file = "", int line = 0);
    
    /**
     * @brief Loga uma mensagem com atributos adicionais
     * @param level Nível de severidade
     * @param message Mensagem do log
     * @param attributes Atributos adicionais
     * @param component Componente que gerou o log
     * @param file Arquivo fonte
     * @param line Linha do arquivo
     */
    void logWithAttributes(LogLevel level, const std::string& message, 
                         const std::unordered_map<std::string, std::string>& attributes,
                         const std::string& component = "", const std::string& file = "", int line = 0);
    
    /**
     * @brief Loga uma mensagem com nível TRACE
     * @param message Mensagem do log
     * @param component Componente que gerou o log
     * @param file Arquivo fonte
     * @param line Linha do arquivo
     */
    void trace(const std::string& message, const std::string& component = "",
             const std::string& file = "", int line = 0);
    
    /**
     * @brief Loga uma mensagem com nível DEBUG
     * @param message Mensagem do log
     * @param component Componente que gerou o log
     * @param file Arquivo fonte
     * @param line Linha do arquivo
     */
    void debug(const std::string& message, const std::string& component = "",
             const std::string& file = "", int line = 0);
    
    /**
     * @brief Loga uma mensagem com nível INFO
     * @param message Mensagem do log
     * @param component Componente que gerou o log
     * @param file Arquivo fonte
     * @param line Linha do arquivo
     */
    void info(const std::string& message, const std::string& component = "",
            const std::string& file = "", int line = 0);
    
    /**
     * @brief Loga uma mensagem com nível WARNING
     * @param message Mensagem do log
     * @param component Componente que gerou o log
     * @param file Arquivo fonte
     * @param line Linha do arquivo
     */
    void warning(const std::string& message, const std::string& component = "",
               const std::string& file = "", int line = 0);
    
    /**
     * @brief Loga uma mensagem com nível ERROR
     * @param message Mensagem do log
     * @param component Componente que gerou o log
     * @param file Arquivo fonte
     * @param line Linha do arquivo
     */
    void error(const std::string& message, const std::string& component = "",
             const std::string& file = "", int line = 0);
    
    /**
     * @brief Loga uma mensagem com nível CRITICAL
     * @param message Mensagem do log
     * @param component Componente que gerou o log
     * @param file Arquivo fonte
     * @param line Linha do arquivo
     */
    void critical(const std::string& message, const std::string& component = "",
                const std::string& file = "", int line = 0);
    
    /**
     * @brief Formata um timestamp para string
     * @param timestamp Timestamp
     * @param format Formato (padrão ISO 8601)
     * @return String formatada
     */
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp,
                                       const std::string& format = "%Y-%m-%d %H:%M:%S");
    
    /**
     * @brief Converte um nível de log para string
     * @param level Nível de log
     * @return String representando o nível
     */
    static std::string levelToString(LogLevel level);
    
    /**
     * @brief Converte uma string para nível de log
     * @param levelStr String representando o nível
     * @param defaultLevel Nível padrão se a string for inválida
     * @return Nível de log
     */
    static LogLevel stringToLevel(const std::string& levelStr, LogLevel defaultLevel = LogLevel::INFO);
    
    /**
     * @brief Força o flush de todos os sinks
     */
    void flush();
    
    /**
     * @brief Define o intervalo de flush automático (modo assíncrono)
     * @param intervalMs Intervalo em milissegundos
     */
    void setFlushInterval(int intervalMs);
    
    /**
     * @brief Habilita ou desabilita todas as mensagens de log
     * @param enabled true para habilitar, false para desabilitar
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Verifica se o logger está habilitado
     * @return true se habilitado
     */
    bool isEnabled() const;
    
    /**
     * @brief Verifica se um nível de log está habilitado para um componente
     * @param level Nível de log
     * @param component Componente
     * @return true se o nível está habilitado
     */
    bool isLevelEnabled(LogLevel level, const std::string& component = "") const;
    
private:
    // Construtor privado (singleton)
    Logger();
    
    // Destrutor
    ~Logger();
    
    // Impedir cópia e atribuição
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Thread de processamento assíncrono
    /**
 * asyncThread
 * 
 * Implementa a funcionalidade asyncThread conforme especificação original.
 */

    void asyncThread();
    
    // Processa uma entrada de log
    /**
 * processLogEntry
 * 
 * Implementa a funcionalidade processLogEntry conforme especificação original.
 * @param entry Parâmetro entry
 */

    void processLogEntry(const LogEntry& entry);
    
    // Sinks para logs
    std::vector<std::shared_ptr<LogSink>> m_sinks;
    
    // Mutex para acesso aos sinks
    std::mutex m_sinksMutex;
    
    // Nível global mínimo para logar
    std::atomic<LogLevel> m_globalLevel;
    
    // Níveis por componente
    std::unordered_map<std::string, LogLevel> m_componentLevels;
    
    // Mutex para acesso aos níveis por componente
    std::mutex m_levelsMutex;
    
    // Flag para modo assíncrono
    bool m_asyncMode;
    
    // Fila para logs assíncronos
    std::queue<LogEntry> m_asyncQueue;
    
    // Mutex para acesso à fila assíncrona
    std::mutex m_asyncQueueMutex;
    
    // Variável de condição para notificação de novos logs
    std::condition_variable m_asyncQueueCV;
    
    // Thread para processamento assíncrono
    std::thread m_asyncThread;
    
    // Flag para thread ativa
    std::atomic<bool> m_asyncThreadActive;
    
    // Intervalo para flush automático
    std::atomic<int> m_flushIntervalMs;
    
    // Flag para logger habilitado
    std::atomic<bool> m_enabled;
    
    // Flag para logger inicializado
    bool m_initialized;
};

// Macros de conveniência para logging
#define LOG_TRACE(message, component) \
    Logger::getInstance().trace(message, component, __FILE__, __LINE__)

#define LOG_DEBUG(message, component) \
    Logger::getInstance().debug(message, component, __FILE__, __LINE__)

#define LOG_INFO(message, component) \
    Logger::getInstance().info(message, component, __FILE__, __LINE__)

#define LOG_WARNING(message, component) \
    Logger::getInstance().warning(message, component, __FILE__, __LINE__)

#define LOG_ERROR(message, component) \
    Logger::getInstance().error(message, component, __FILE__, __LINE__)

#define LOG_CRITICAL(message, component) \
    Logger::getInstance().critical(message, component, __FILE__, __LINE__)

#define LOG_WITH_ATTRS(level, message, attributes, component) \
    Logger::getInstance().logWithAttributes(level, message, attributes, component, __FILE__, __LINE__)

// Macro para verificar se um nível está habilitado antes de construir mensagem
#define IS_LOG_ENABLED(level, component) \
    Logger::getInstance().isLevelEnabled(level, component)

#endif // _LOGGER_H_

} // namespace wydbr
