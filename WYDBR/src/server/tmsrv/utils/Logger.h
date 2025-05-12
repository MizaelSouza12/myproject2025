/**
 * Logger.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/utils/Logger.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <sstream>
#include <cstdarg>

namespace wydbr {


namespace wyd {
namespace server {

/**
 * @brief Níveis de log
 */
enum /**
 * Classe WYDLogLevel
 * 
 * Esta classe implementa a funcionalidade WYDLogLevel conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class LogLevel {
    DEBUG,  // Informações detalhadas para debug
    INFO,   // Informações gerais
    WARN,   // Avisos
    ERROR,  // Erros
    FATAL   // Erros fatais
};

/**
 * @brief Converte LogLevel para string
 * @param level Nível de log
 * @return String representando o nível de log
 */
inline /**
 * LogLevelToString
 * 
 * Implementa a funcionalidade LogLevelToString conforme especificação original.
 * @param level Parâmetro level
 * @return Retorna std::string
 */
 std::string LogLevelToString(LogLevel level){
    switch (level) {
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO:  return "INFO";
    case LogLevel::WARN:  return "WARN";
    case LogLevel::ERROR: return "ERROR";
    case LogLevel::FATAL: return "FATAL";
    default:              return "UNKNOWN";
    }
}

/**
 * @brief Classe para gerenciamento de logs
 * 
 * Esta classe gerencia os logs do sistema, permitindo escrever mensagens
 * em um arquivo de log com diferentes níveis de severidade.
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
     * @brief Construtor
     * @param filename Nome do arquivo de log
     * @param minLevel Nível mínimo de log
     */
    Logger(const std::string& filename, LogLevel minLevel = LogLevel::INFO)
        : filename_(filename), minLevel_(minLevel), maxFileSize_(10 * 1024 * 1024), maxBackupCount_(5) {
        
        // Garante que o diretório existe
        /**
 * path
 * 
 * Implementa a funcionalidade path conforme especificação original.
 * @param filename_ Parâmetro filename_
 * @return Retorna std::filesystem::path
 */

        std::filesystem::path path(filename_);
        std::filesystem::create_directories(path.parent_path());
        
        // Abre o arquivo de log
        file_.open(filename_, std::ios::app);
        if (!file_.is_open()) {
            std::cerr << "Erro ao abrir arquivo de log: " << filename_ << std::endl;
        }
        
        // Registro /**
 * Write
 * 
 * Implementa a funcionalidade Write conforme especificação original.
 * @param LogLevel::INFO Parâmetro LogLevel::INFO
 * @param -----------" Parâmetro -----------"
 * @return Retorna inicial
 */
 inicial
        Write(LogLevel::INFO, "----------- Log iniciado -----------");
    }
    
    /**
     * @brief Destrutor
     */
    ~Logger() {
        if (file_.is_open()) {
            Write(LogLevel::INFO, "----------- Log finalizado -----------");
            file_.close();
        }
    }
    
    /**
     * @brief Define o nível mínimo de log
     * @param level Nível mínimo de log
     */
    void SetMinLevel(LogLevel level) {
        minLevel_ = level;
    }
    
    /**
     * @brief Define o tamanho máximo do arquivo de log
     * @param size Tamanho máximo em bytes
     */
    void SetMaxFileSize(size_t size) {
        maxFileSize_ = size;
    }
    
    /**
     * @brief Define o número máximo de arquivos de backup
     * @param count Número máximo de arquivos de backup
     */
    void SetMaxBackupCount(int count) {
        maxBackupCount_ = count;
    }
    
    /**
     * @brief Escreve uma mensagem no log
     * @param level Nível de log
     * @param format Formato da mensagem (estilo printf)
     * @param ... Argumentos para o formato
     */
    void Write(LogLevel level, const char* format, ...) {
        if (level < minLevel_) {
            return;
        }
        
        // Rotaciona o arquivo se necessá/**
 * RotateLogIfNeeded
 * 
 * Implementa a funcionalidade RotateLogIfNeeded conforme especificação original.
 * @return Retorna rio
 */
rio
        RotateLogIfNeeded();
        
        // Formata a mensagem
        char buffer[4096];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        // Formata o log
        std::stringstream ss;
        ss << GetTimestamp() << " [" << LogLevelToString(level) << "] " << buffer;
        
        // Escreve no arquivo e no console
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (file_.is_open()) {
            file_ << ss.str() << std::endl;
            file_.flush();
        }
        
        // Debug e info apenas no console se /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param LogLevel::WARN Parâmetro LogLevel::WARN
 * @return Retorna solicitado
 */
 solicitado
        if(level >= LogLevel::WARN){
            std::cout << ss.str() << std::endl;
        }
    }
    
    /**
     * @brief Escreve uma mensagem de debug no log
     * @param format Formato da mensagem (estilo printf)
     * @param ... Argumentos para o formato
     */
    void Debug(const char* format, ...) {
        if (LogLevel::DEBUG < minLevel_) {
            return;
        }
        
        char buffer[4096];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Write(LogLevel::DEBUG, "%s", buffer);
    }
    
    /**
     * @brief Escreve uma mensagem de informação no log
     * @param format Formato da mensagem (estilo printf)
     * @param ... Argumentos para o formato
     */
    void Info(const char* format, ...) {
        if (LogLevel::INFO < minLevel_) {
            return;
        }
        
        char buffer[4096];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Write(LogLevel::INFO, "%s", buffer);
    }
    
    /**
     * @brief Escreve uma mensagem de aviso no log
     * @param format Formato da mensagem (estilo printf)
     * @param ... Argumentos para o formato
     */
    void Warn(const char* format, ...) {
        if (LogLevel::WARN < minLevel_) {
            return;
        }
        
        char buffer[4096];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Write(LogLevel::WARN, "%s", buffer);
    }
    
    /**
     * @brief Escreve uma mensagem de erro no log
     * @param format Formato da mensagem (estilo printf)
     * @param ... Argumentos para o formato
     */
    void Error(const char* format, ...) {
        if (LogLevel::ERROR < minLevel_) {
            return;
        }
        
        char buffer[4096];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Write(LogLevel::ERROR, "%s", buffer);
    }
    
    /**
     * @brief Escreve uma mensagem de erro fatal no log
     * @param format Formato da mensagem (estilo printf)
     * @param ... Argumentos para o formato
     */
    void Fatal(const char* format, ...) {
        if (LogLevel::FATAL < minLevel_) {
            return;
        }
        
        char buffer[4096];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Write(LogLevel::FATAL, "%s", buffer);
    }
    
private:
    /**
     * @brief Obtém o timestamp atual formatado
     * @return Timestamp formatado
     */
    std::string GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime_r(&time), "%Y-%m-%d %H:%M:%S") 
           << '.' << std::setfill('0') << std::setw(3) << ms.count();
        
        return ss.str();
    }
    
    /**
     * @brief Rotaciona o arquivo de log se necessário
     */
    void RotateLogIfNeeded() {
        if (!file_.is_open()) {
            return;
        }
        
        // Verifica o tamanho do arquivo
        file_.flush();
        size_t fileSize = std::filesystem::file_size(filename_);
        
        if (fileSize >= maxFileSize_) {
            // Fecha o arquivo atual
            file_.close();
            
            // Remove o backup mais antigo se necessário
            std::string oldestBackup = filename_ + "." + std::to_string(maxBackupCount_);
            if (std::filesystem::exists(oldestBackup)) {
                std::filesystem::remove(oldestBackup);
            }
            
            // Move os backups /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param --i Parâmetro --i
 * @return Retorna existentes
 */
 existentes
            for(int i = maxBackupCount_ - 1; i >= 1; --i){
                std::string oldBackup = filename_ + "." + std::to_string(i);
                std::string newBackup = filename_ + "." + std::to_string(i + 1);
                
                if (std::filesystem::exists(oldBackup)) {
                    std::filesystem::rename(oldBackup, newBackup);
                }
            }
            
            // Move o arquivo atual para o primeiro backup
            std::string firstBackup = filename_ + ".1";
            std::filesystem::rename(filename_, firstBackup);
            
            // Abre um novo arquivo de log
            file_.open(filename_, std::ios::app);
            if (!file_.is_open()) {
                std::cerr << "Erro ao abrir novo arquivo de log após rotação: " << filename_ << std::endl;
            }
        }
    }
    
    std::string filename_;          // Nome do arquivo de log
    std::ofstream file_;            // Stream do arquivo de log
    LogLevel minLevel_;             // Nível mínimo de log
    size_t maxFileSize_;            // Tamanho máximo do arquivo de log
    int maxBackupCount_;            // Número máximo de arquivos de backup
    std::mutex mutex_;              // Mutex para thread safety
};

} // namespace server
} // namespace wyd

#endif // LOGGER_H

} // namespace wydbr
