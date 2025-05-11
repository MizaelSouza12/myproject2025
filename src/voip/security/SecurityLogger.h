/**
 * @file SecurityLogger.h
 * @brief Sistema de logging para segurança do VoIP
 */

#ifndef WYDBR_VOIP_SECURITY_LOGGER_H
#define WYDBR_VOIP_SECURITY_LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <memory>
#include <queue>
#include <thread>
#include <atomic>

namespace WYDBR {
namespace VoIP {

enum class SecurityLogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct SecurityLogEntry {
    std::chrono::system_clock::time_point timestamp;
    SecurityLogLevel level;
    std::string message;
    std::string source;
    std::string details;
};

class SecurityLogger {
public:
    static SecurityLogger& GetInstance();

    bool Initialize(const std::string& logPath, SecurityLogLevel minLevel = SecurityLogLevel::INFO);
    void Shutdown();

    void Log(SecurityLogLevel level, const std::string& message, 
             const std::string& source = "", const std::string& details = "");

    void SetLogLevel(SecurityLogLevel level);
    SecurityLogLevel GetLogLevel() const;

    bool IsInitialized() const;
    const std::string& GetLogPath() const;

private:
    SecurityLogger();
    ~SecurityLogger();

    SecurityLogger(const SecurityLogger&) = delete;
    SecurityLogger& operator=(const SecurityLogger&) = delete;

    void ProcessLogQueue();
    std::string FormatLogEntry(const SecurityLogEntry& entry);
    std::string GetLogLevelString(SecurityLogLevel level);
    void RotateLogFile();

    std::string m_logPath;
    std::ofstream m_logFile;
    SecurityLogLevel m_minLevel;
    bool m_initialized;
    std::mutex m_logMutex;
    std::queue<SecurityLogEntry> m_logQueue;
    std::thread m_logThread;
    std::atomic<bool> m_running;
    std::chrono::system_clock::time_point m_lastRotation;
    size_t m_maxLogSize;
    size_t m_maxLogFiles;
};

#define SECURITY_LOG_DEBUG(message, source, details) \
    SecurityLogger::GetInstance().Log(SecurityLogLevel::DEBUG, message, source, details)

#define SECURITY_LOG_INFO(message, source, details) \
    SecurityLogger::GetInstance().Log(SecurityLogLevel::INFO, message, source, details)

#define SECURITY_LOG_WARNING(message, source, details) \
    SecurityLogger::GetInstance().Log(SecurityLogLevel::WARNING, message, source, details)

#define SECURITY_LOG_ERROR(message, source, details) \
    SecurityLogger::GetInstance().Log(SecurityLogLevel::ERROR, message, source, details)

#define SECURITY_LOG_CRITICAL(message, source, details) \
    SecurityLogger::GetInstance().Log(SecurityLogLevel::CRITICAL, message, source, details)

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_SECURITY_LOGGER_H 