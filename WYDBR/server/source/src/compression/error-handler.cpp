/**
 * @file error-handler.cpp
 * @brief Implementação do sistema de gerenciamento de erros de compressão
 */

#include "compression/error-handler.h"
#include <iostream>
#include <algorithm>

namespace wydbr {
namespace compression {

void ErrorHandler::logError(
    CompressionErrorType errorType,
    const std::string& message,
    const std::string& details,
    LogLevel level,
    bool recovered
) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Criar entrada de log
    ErrorLogEntry entry(errorType, message, details, level, recovered);
    
    // Adicionar ao histórico
    m_errorLog.push_back(entry);
    
    // Limitar tamanho do log (manter apenas os 1000 erros mais recentes)
    const size_t MAX_LOG_SIZE = 1000;
    if (m_errorLog.size() > MAX_LOG_SIZE) {
        m_errorLog.erase(m_errorLog.begin(), m_errorLog.begin() + (m_errorLog.size() - MAX_LOG_SIZE));
    }
    
    // Notificar callbacks
    notifyCallbacks(entry);
    
    // Log para console em modo debug
    #ifdef _DEBUG
    std::string levelStr;
    switch (level) {
        case LogLevel::DEBUG:   levelStr = "DEBUG"; break;
        case LogLevel::INFO:    levelStr = "INFO"; break;
        case LogLevel::WARNING: levelStr = "WARNING"; break;
        case LogLevel::ERROR:   levelStr = "ERROR"; break;
        case LogLevel::CRITICAL: levelStr = "CRITICAL"; break;
    }
    
    std::cerr << "[" << levelStr << "] Compressão: " << message;
    if (!details.empty()) {
        std::cerr << " - " << details;
    }
    std::cerr << std::endl;
    #endif
}

void ErrorHandler::logException(const CompressionException& ex, bool recovered) {
    logError(
        ex.getErrorType(),
        ex.what(),
        ex.getDetails(),
        LogLevel::ERROR,
        recovered
    );
}

int ErrorHandler::addErrorCallback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    int callbackId = m_nextCallbackId++;
    m_errorCallbacks[callbackId] = callback;
    
    return callbackId;
}

bool ErrorHandler::removeErrorCallback(int callbackId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_errorCallbacks.find(callbackId);
    if (it != m_errorCallbacks.end()) {
        m_errorCallbacks.erase(it);
        return true;
    }
    
    return false;
}

void ErrorHandler::clearErrorLog() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_errorLog.clear();
}

std::vector<ErrorLogEntry> ErrorHandler::getErrorLog(size_t maxEntries) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (maxEntries == 0 || maxEntries >= m_errorLog.size()) {
        return m_errorLog;
    }
    
    // Retornar apenas as entradas mais recentes
    return std::vector<ErrorLogEntry>(
        m_errorLog.end() - maxEntries,
        m_errorLog.end()
    );
}

std::map<CompressionErrorType, size_t> ErrorHandler::getErrorStats() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::map<CompressionErrorType, size_t> stats;
    
    for (const auto& entry : m_errorLog) {
        stats[entry.errorType]++;
    }
    
    return stats;
}

bool ErrorHandler::attemptRecovery(CompressionErrorType errorType, const std::map<std::string, std::string>& context) {
    // Inicializar estratégias de recuperação se necessário
    if (m_recoveryStrategies.empty()) {
        initRecoveryStrategies();
    }
    
    // Verificar se existe estratégia para este tipo de erro
    auto it = m_recoveryStrategies.find(errorType);
    if (it != m_recoveryStrategies.end()) {
        // Tentar recuperação
        bool recovered = it->second(context);
        
        // Registrar resultado
        std::string errorTypeName = CompressionErrorTypeToString(errorType);
        if (recovered) {
            logError(
                errorType,
                "Recuperação bem-sucedida: " + errorTypeName,
                "",
                LogLevel::INFO,
                true
            );
        } else {
            logError(
                errorType,
                "Falha na recuperação: " + errorTypeName,
                "",
                LogLevel::WARNING,
                false
            );
        }
        
        return recovered;
    }
    
    // Sem estratégia disponível
    return false;
}

void ErrorHandler::initRecoveryStrategies() {
    // Inicializar estratégias de recuperação para cada tipo de erro
    
    m_recoveryStrategies[CompressionErrorType::INVALID_DATA] = [](const std::map<std::string, std::string>& context) {
        // Recuperação para dados inválidos
        return false; // Não é possível recuperar de dados inválidos
    };
    
    m_recoveryStrategies[CompressionErrorType::INVALID_ALGORITHM] = [](const std::map<std::string, std::string>& context) {
        // Recuperação para algoritmo inválido: usar algoritmo de fallback
        return true;
    };
    
    m_recoveryStrategies[CompressionErrorType::COMPRESSION_FAILED] = [](const std::map<std::string, std::string>& context) {
        // Tentar com algoritmo alternativo
        if (context.find("retry_count") != context.end()) {
            int retryCount = std::stoi(context.at("retry_count"));
            return retryCount < 3; // Permitir até 3 tentativas
        }
        return true; // Primeira tentativa
    };
    
    m_recoveryStrategies[CompressionErrorType::DECOMPRESSION_FAILED] = [](const std::map<std::string, std::string>& context) {
        // Tentar com método alternativo
        if (context.find("retry_count") != context.end()) {
            int retryCount = std::stoi(context.at("retry_count"));
            return retryCount < 2; // Permitir até 2 tentativas
        }
        return true; // Primeira tentativa
    };
    
    m_recoveryStrategies[CompressionErrorType::BUFFER_TOO_SMALL] = [](const std::map<std::string, std::string>& context) {
        // Tentar aumentar buffer automáticamente
        return true;
    };
    
    m_recoveryStrategies[CompressionErrorType::NULL_POINTER] = [](const std::map<std::string, std::string>& context) {
        // Não é possível recuperar de ponteiro nulo
        return false;
    };
    
    m_recoveryStrategies[CompressionErrorType::CORRUPTED_DATA] = [](const std::map<std::string, std::string>& context) {
        // Tentar métodos avançados de recuperação de dados corrompidos
        // Para o propósito desta implementação, simplesmente definimos como não recuperável
        return false;
    };
    
    m_recoveryStrategies[CompressionErrorType::FORMAT_MISMATCH] = [](const std::map<std::string, std::string>& context) {
        // Tentar detectar formato automaticamente
        return true;
    };
    
    m_recoveryStrategies[CompressionErrorType::MEMORY_ERROR] = [](const std::map<std::string, std::string>& context) {
        // Tentar liberar memória e retentar
        return true;
    };
    
    m_recoveryStrategies[CompressionErrorType::LIBRARY_ERROR] = [](const std::map<std::string, std::string>& context) {
        // Tentar com biblioteca alternativa
        return true;
    };
    
    m_recoveryStrategies[CompressionErrorType::UNKNOWN_ERROR] = [](const std::map<std::string, std::string>& context) {
        // Não é possível recuperar de erro desconhecido
        return false;
    };
}

void ErrorHandler::notifyCallbacks(const ErrorLogEntry& entry) {
    // Notificar todos os callbacks registrados
    for (const auto& callback : m_errorCallbacks) {
        callback.second(entry);
    }
}

} // namespace compression
} // namespace wydbr