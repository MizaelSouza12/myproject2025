/**
 * @file LogSystem.h
 * @brief Sistema de log do WYDBR 2.0
 */

#ifndef WYDBR_LOG_SYSTEM_H
#define WYDBR_LOG_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <mutex>
#include "wyd_core.h"

namespace WYDBR {

/**
 * @class LogSystem
 * @brief Gerencia o sistema de log do jogo
 */
class LogSystem {
public:
    static LogSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de logs
    void LogInfo(const std::string& message);
    void LogWarning(const std::string& message);
    void LogError(const std::string& message);
    void LogDebug(const std::string& message);
    void LogSecurity(const std::string& message);
    void LogGameplay(const std::string& message);
    void LogTrade(const std::string& message);
    void LogChat(const std::string& message);

    // Gerenciamento de arquivos
    bool CreateLogFile(const std::string& filename);
    bool CloseLogFile(const std::string& filename);
    bool RotateLogFile(const std::string& filename);
    bool ArchiveLogFile(const std::string& filename);

    // Verificações
    bool IsLogEnabled(uint8_t logType) const;
    bool IsLogFileOpen(const std::string& filename) const;
    bool HasLogPermission(uint8_t logType) const;
    bool IsLogRotating(const std::string& filename) const;

    // Queries
    std::string GetLogPath() const;
    uint32_t GetLogSize(const std::string& filename) const;
    std::vector<std::string> GetLogFiles() const;
    std::vector<std::string> GetLogContent(const std::string& filename) const;

    // Utilidades
    void UpdateLogs(uint32_t deltaTime);
    void ProcessLogRotation();
    void CleanOldLogs();
    void BackupLogs();

private:
    LogSystem() = default;
    ~LogSystem() = default;

    // Prevenir cópias
    LogSystem(const LogSystem&) = delete;
    LogSystem& operator=(const LogSystem&) = delete;

    // Cálculos internos
    bool ValidateLogType(uint8_t logType) const;
    bool CheckLogRequirements(const std::string& filename) const;
    void UpdateLogStatus(const std::string& filename);
    void HandleLogError(const std::string& filename);

    // Estado
    struct LogFile {
        std::string filename;
        std::ofstream file;
        uint32_t size;
        uint32_t maxSize;
        bool isRotating;
    };

    struct LogData {
        std::unordered_map<std::string, LogFile> files;
        std::unordered_map<uint8_t, bool> enabledTypes;
        std::string basePath;
        uint32_t maxFileSize;
        uint32_t maxFiles;
    };

    LogData m_logData;
    std::mutex m_logMutex;
};

} // namespace WYDBR

#endif // WYDBR_LOG_SYSTEM_H 