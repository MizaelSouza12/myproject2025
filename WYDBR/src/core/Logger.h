/**
 * @file Logger.h
 * @brief Sistema de logging para o projeto WYDBR
 * @author WYDBR Team
 */

#ifndef WYDBR_LOGGER_H
#define WYDBR_LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <vector>

namespace wydbr {
namespace core {

/**
 * @enum LogLevel
 * @brief Níveis de log disponíveis
 */
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    NONE
};

/**
 * @struct LogConfig
 * @brief Configuração do sistema de log
 */
struct LogConfig {
    std::string logDirectory = "logs";
    std::string logFilePrefix = "wydbr";
    LogLevel consoleLevel = LogLevel::INFO;
    LogLevel fileLevel = LogLevel::DEBUG;
    bool enableConsoleLogging = true;
    bool enableFileLogging = true;
    bool enableColoredOutput = true;
    bool enableTimestamps = true;
    size_t maxFileSizeMB = 10;
    int maxLogFiles = 10;
};

/**
 * @class Logger
 * @brief Sistema de logging com suporte a múltiplos níveis e rotação de arquivos
 */
class Logger {
public:
    /**
     * @brief Obtém a instância singleton do logger
     * @return Referência para a instância do logger
     */
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    /**
     * @brief Configura o logger
     * @param config Configuração do logger
     */
    void configure(const LogConfig& config) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_config = config;
        
        // Cria diretório de logs se não existir
        if (m_config.enableFileLogging) {
            std::filesystem::create_directories(m_config.logDirectory);
            reopenLogFile();
        }
    }

    /**
     * @brief Registra uma mensagem no nível TRACE
     * @param message Mensagem a ser registrada
     * @param file Arquivo de origem
     * @param line Linha de origem
     */
    void trace(const std::string& message, const char* file = nullptr, int line = 0) {
        log(LogLevel::TRACE, message, file, line);
    }

    /**
     * @brief Registra uma mensagem no nível DEBUG
     * @param message Mensagem a ser registrada
     * @param file Arquivo de origem
     * @param line Linha de origem
     */
    void debug(const std::string& message, const char* file = nullptr, int line = 0) {
        log(LogLevel::DEBUG, message, file, line);
    }

    /**
     * @brief Registra uma mensagem no nível INFO
     * @param message Mensagem a ser registrada
     * @param file Arquivo de origem
     * @param line Linha de origem
     */
    void info(const std::string& message, const char* file = nullptr, int line = 0) {
        log(LogLevel::INFO, message, file, line);
    }

    /**
     * @brief Registra uma mensagem no nível WARNING
     * @param message Mensagem a ser registrada
     * @param file Arquivo de origem
     * @param line Linha de origem
     */
    void warning(const std::string& message, const char* file = nullptr, int line = 0) {
        log(LogLevel::WARNING, message, file, line);
    }

    /**
     * @brief Registra uma mensagem no nível ERROR
     * @param message Mensagem a ser registrada
     * @param file Arquivo de origem
     * @param line Linha de origem
     */
    void error(const std::string& message, const char* file = nullptr, int line = 0) {
        log(LogLevel::ERROR, message, file, line);
    }

    /**
     * @brief Registra uma mensagem no nível CRITICAL
     * @param message Mensagem a ser registrada
     * @param file Arquivo de origem
     * @param line Linha de origem
     */
    void critical(const std::string& message, const char* file = nullptr, int line = 0) {
        log(LogLevel::CRITICAL, message, file, line);
    }

    /**
     * @brief Registra uma mensagem em um nível específico
     * @param level Nível da mensagem
     * @param message Mensagem a ser registrada
     * @param file Arquivo de origem
     * @param line Linha de origem
     */
    void log(LogLevel level, const std::string& message, const char* file = nullptr, int line = 0) {
        if (level < m_config.consoleLevel && level < m_config.fileLevel) {
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::string formattedMessage = formatLogMessage(level, message, file, line);
        
        // Log para console
        if (m_config.enableConsoleLogging && level >= m_config.consoleLevel) {
            if (m_config.enableColoredOutput) {
                std::string colorCode = getColorForLevel(level);
                std::string resetCode = "\033[0m";
                std::cout << colorCode << formattedMessage << resetCode << std::endl;
            } else {
                std::cout << formattedMessage << std::endl;
            }
        }
        
        // Log para arquivo
        if (m_config.enableFileLogging && level >= m_config.fileLevel) {
            if (m_logFile.is_open()) {
                m_logFile << formattedMessage << std::endl;
                m_logFile.flush();
                
                // Verifica tamanho do arquivo para rotação
                m_logFile.seekp(0, std::ios::end);
                size_t fileSize = m_logFile.tellp();
                if (fileSize > m_config.maxFileSizeMB * 1024 * 1024) {
                    rotateLogFiles();
                }
            }
        }
    }

private:
    Logger() = default;
    ~Logger() {
        if (m_logFile.is_open()) {
            m_logFile.close();
        }
    }
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Formata a mensagem de log
    std::string formatLogMessage(LogLevel level, const std::string& message, const char* file, int line) {
        std::stringstream ss;
        
        // Adiciona timestamp
        if (m_config.enableTimestamps) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
            ss << std::put_time(std::localtime(&time), "[%Y-%m-%d %H:%M:%S") 
               << "." << std::setfill('0') << std::setw(3) << ms.count() << "] ";
        }
        
        // Nível do log
        ss << "[" << getLevelString(level) << "] ";
        
        // Informações de origem
        if (file != nullptr && line > 0) {
            std::filesystem::path filePath(file);
            ss << "[" << filePath.filename().string() << ":" << line << "] ";
        }
        
        // Mensagem
        ss << message;
        
        return ss.str();
    }
    
    // Obtém string para o nível de log
    std::string getLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    // Obtém código de cor ANSI para o nível
    std::string getColorForLevel(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "\033[90m"; // Cinza
            case LogLevel::DEBUG: return "\033[36m"; // Ciano
            case LogLevel::INFO: return "\033[32m";  // Verde
            case LogLevel::WARNING: return "\033[33m"; // Amarelo
            case LogLevel::ERROR: return "\033[31m"; // Vermelho
            case LogLevel::CRITICAL: return "\033[35m"; // Magenta
            default: return "\033[0m"; // Reset
        }
    }
    
    // Reabre o arquivo de log
    void reopenLogFile() {
        if (m_logFile.is_open()) {
            m_logFile.close();
        }
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << m_config.logDirectory << "/" << m_config.logFilePrefix << "_" 
           << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << ".log";
        
        m_currentLogFile = ss.str();
        m_logFile.open(m_currentLogFile, std::ios::app);
        
        if (!m_logFile.is_open()) {
            std::cerr << "Failed to open log file: " << m_currentLogFile << std::endl;
        }
    }
    
    // Rotaciona os arquivos de log
    void rotateLogFiles() {
        m_logFile.close();
        
        // Lista todos os arquivos de log
        std::vector<std::string> logFiles;
        for (const auto& entry : std::filesystem::directory_iterator(m_config.logDirectory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find(m_config.logFilePrefix) == 0 && filename.find(".log") != std::string::npos) {
                    logFiles.push_back(entry.path().string());
                }
            }
        }
        
        // Ordena por data/hora (mais recentes primeiro)
        std::sort(logFiles.begin(), logFiles.end(), std::greater<std::string>());
        
        // Remove arquivos antigos se exceder o máximo
        while (logFiles.size() >= static_cast<size_t>(m_config.maxLogFiles)) {
            std::filesystem::remove(logFiles.back());
            logFiles.pop_back();
        }
        
        // Cria novo arquivo de log
        reopenLogFile();
    }
    
    std::mutex m_mutex;
    LogConfig m_config;
    std::ofstream m_logFile;
    std::string m_currentLogFile;
};

// Macros para facilitar o uso do logger
#define LOG_TRACE(message) wydbr::core::Logger::getInstance().trace(message, __FILE__, __LINE__)
#define LOG_DEBUG(message) wydbr::core::Logger::getInstance().debug(message, __FILE__, __LINE__)
#define LOG_INFO(message) wydbr::core::Logger::getInstance().info(message, __FILE__, __LINE__)
#define LOG_WARNING(message) wydbr::core::Logger::getInstance().warning(message, __FILE__, __LINE__)
#define LOG_ERROR(message) wydbr::core::Logger::getInstance().error(message, __FILE__, __LINE__)
#define LOG_CRITICAL(message) wydbr::core::Logger::getInstance().critical(message, __FILE__, __LINE__)

} // namespace core
} // namespace wydbr

#endif // WYDBR_LOGGER_H 