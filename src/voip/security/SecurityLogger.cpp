/**
 * @file SecurityLogger.cpp
 * @brief Implementação do sistema de logging para segurança do VoIP
 */

#include "SecurityLogger.h"
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace WYDBR {
namespace VoIP {

SecurityLogger& SecurityLogger::GetInstance()
{
    static SecurityLogger instance;
    return instance;
}

SecurityLogger::SecurityLogger()
    : m_initialized(false)
    , m_running(false)
    , m_maxLogSize(10 * 1024 * 1024) // 10MB
    , m_maxLogFiles(10)
{
}

SecurityLogger::~SecurityLogger()
{
    Shutdown();
}

bool SecurityLogger::Initialize(const std::string& logPath, SecurityLogLevel minLevel)
{
    std::lock_guard<std::mutex> lock(m_logMutex);

    if (m_initialized) {
        return true;
    }

    m_logPath = logPath;
    m_minLevel = minLevel;

    // Criar diretório de logs se não existir
    std::filesystem::path logDir = std::filesystem::path(logPath).parent_path();
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);
    }

    // Abrir arquivo de log
    m_logFile.open(logPath, std::ios::app);
    if (!m_logFile.is_open()) {
        return false;
    }

    m_initialized = true;
    m_running = true;
    m_lastRotation = std::chrono::system_clock::now();

    // Iniciar thread de processamento
    m_logThread = std::thread(&SecurityLogger::ProcessLogQueue, this);

    return true;
}

void SecurityLogger::Shutdown()
{
    {
        std::lock_guard<std::mutex> lock(m_logMutex);
        if (!m_initialized) {
            return;
        }

        m_running = false;
    }

    if (m_logThread.joinable()) {
        m_logThread.join();
    }

    std::lock_guard<std::mutex> lock(m_logMutex);
    if (m_logFile.is_open()) {
        m_logFile.close();
    }

    m_initialized = false;
}

void SecurityLogger::Log(SecurityLogLevel level, const std::string& message,
                        const std::string& source, const std::string& details)
{
    if (!m_initialized || level < m_minLevel) {
        return;
    }

    SecurityLogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.message = message;
    entry.source = source;
    entry.details = details;

    std::lock_guard<std::mutex> lock(m_logMutex);
    m_logQueue.push(entry);
}

void SecurityLogger::SetLogLevel(SecurityLogLevel level)
{
    std::lock_guard<std::mutex> lock(m_logMutex);
    m_minLevel = level;
}

SecurityLogLevel SecurityLogger::GetLogLevel() const
{
    std::lock_guard<std::mutex> lock(m_logMutex);
    return m_minLevel;
}

bool SecurityLogger::IsInitialized() const
{
    return m_initialized;
}

const std::string& SecurityLogger::GetLogPath() const
{
    return m_logPath;
}

void SecurityLogger::ProcessLogQueue()
{
    while (m_running) {
        SecurityLogEntry entry;
        bool hasEntry = false;

        {
            std::lock_guard<std::mutex> lock(m_logMutex);
            if (!m_logQueue.empty()) {
                entry = m_logQueue.front();
                m_logQueue.pop();
                hasEntry = true;
            }
        }

        if (hasEntry) {
            std::string formattedEntry = FormatLogEntry(entry);
            std::lock_guard<std::mutex> lock(m_logMutex);
            m_logFile << formattedEntry << std::endl;
            m_logFile.flush();

            // Verificar rotação de log
            if (m_logFile.tellp() >= m_maxLogSize) {
                RotateLogFile();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

std::string SecurityLogger::FormatLogEntry(const SecurityLogEntry& entry)
{
    std::stringstream ss;

    // Formatar timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

    // Adicionar nível de log
    ss << " [" << GetLogLevelString(entry.level) << "]";

    // Adicionar fonte
    if (!entry.source.empty()) {
        ss << " [" << entry.source << "]";
    }

    // Adicionar mensagem
    ss << " " << entry.message;

    // Adicionar detalhes
    if (!entry.details.empty()) {
        ss << " - " << entry.details;
    }

    return ss.str();
}

std::string SecurityLogger::GetLogLevelString(SecurityLogLevel level)
{
    switch (level) {
        case SecurityLogLevel::DEBUG:
            return "DEBUG";
        case SecurityLogLevel::INFO:
            return "INFO";
        case SecurityLogLevel::WARNING:
            return "WARNING";
        case SecurityLogLevel::ERROR:
            return "ERROR";
        case SecurityLogLevel::CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

void SecurityLogger::RotateLogFile()
{
    if (!m_logFile.is_open()) {
        return;
    }

    m_logFile.close();

    // Renomear arquivos de log existentes
    for (size_t i = m_maxLogFiles - 1; i > 0; --i) {
        std::string oldName = m_logPath + "." + std::to_string(i);
        std::string newName = m_logPath + "." + std::to_string(i + 1);
        
        if (std::filesystem::exists(oldName)) {
            if (i == m_maxLogFiles - 1) {
                std::filesystem::remove(oldName);
            } else {
                std::filesystem::rename(oldName, newName);
            }
        }
    }

    // Renomear arquivo de log atual
    std::filesystem::rename(m_logPath, m_logPath + ".1");

    // Criar novo arquivo de log
    m_logFile.open(m_logPath, std::ios::app);
    m_lastRotation = std::chrono::system_clock::now();
}

} // namespace VoIP
} // namespace WYDBR 