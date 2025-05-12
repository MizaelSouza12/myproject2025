/**
 * @file error-handler.h
 * @brief Gerenciador de Erros de Compressão
 *
 * Este arquivo define o gerenciador centralizado de erros para o sistema de compressão.
 * Implementa um singleton que registra, categoriza e pode executar estratégias de 
 * recuperação para diferentes tipos de erros encontrados durante a compressão.
 */

#ifndef WYDBR_COMPRESSION_ERROR_HANDLER_H
#define WYDBR_COMPRESSION_ERROR_HANDLER_H

#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <functional>

#include "compression/compression-exceptions.h"

namespace wydbr {
namespace compression {

// Tipo para função de callback de erro
using ErrorCallback = std::function<void(const ErrorLogEntry&)>;

// Tipo para estratégia de recuperação de erro
using RecoveryStrategy = std::function<bool(const std::map<std::string, std::string>&)>;

/**
 * @class ErrorHandler
 * @brief Gerenciador de erros de compressão
 * 
 * Singleton que gerencia todos os erros de compressão, registra eventos,
 * mantém histórico e implementa estratégias de recuperação.
 */
class ErrorHandler {
public:
    /**
     * @brief Obtém a instância única do singleton
     * @return Referência para a instância
     */
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    /**
     * @brief Registra um erro de compressão
     * @param errorType Tipo do erro
     * @param message Mensagem do erro
     * @param details Detalhes adicionais (opcional)
     * @param level Nível de log (opcional)
     * @param recovered Se o erro foi recuperado (opcional)
     */
    void logError(
        CompressionErrorType errorType,
        const std::string& message,
        const std::string& details = "",
        LogLevel level = LogLevel::ERROR,
        bool recovered = false
    );
    
    /**
     * @brief Registra uma exceção de compressão
     * @param ex Exceção a ser registrada
     * @param recovered Se o erro foi recuperado
     */
    void logException(
        const CompressionException& ex,
        bool recovered = false
    );
    
    /**
     * @brief Adiciona um callback para ser notificado de erros
     * @param callback Função a ser chamada quando ocorrer um erro
     * @return ID do callback para remoção posterior
     */
    int addErrorCallback(ErrorCallback callback);
    
    /**
     * @brief Remove um callback de erro
     * @param callbackId ID do callback a ser removido
     * @return true se o callback foi removido com sucesso
     */
    bool removeErrorCallback(int callbackId);
    
    /**
     * @brief Limpa o histórico de erros
     */
    void clearErrorLog();
    
    /**
     * @brief Obtém o histórico de erros
     * @param maxEntries Número máximo de entradas a retornar (0 = todas)
     * @return Vetor com as entradas de log
     */
    std::vector<ErrorLogEntry> getErrorLog(size_t maxEntries = 0) const;
    
    /**
     * @brief Obtém estatísticas de erros
     * @return Mapa com contagem de cada tipo de erro
     */
    std::map<CompressionErrorType, size_t> getErrorStats() const;
    
    /**
     * @brief Tenta recuperar de um erro
     * @param errorType Tipo do erro
     * @param context Contexto adicional para a recuperação
     * @return true se a recuperação foi bem-sucedida
     */
    bool attemptRecovery(
        CompressionErrorType errorType,
        const std::map<std::string, std::string>& context = {}
    );
    
private:
    // Construtor e operadores privados para evitar instanciação direta
    ErrorHandler() {}
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    
    // Inicializa estratégias de recuperação
    void initRecoveryStrategies();
    
    // Notifica todos os callbacks sobre um novo erro
    void notifyCallbacks(const ErrorLogEntry& entry);
    
    // Dados do singleton
    std::vector<ErrorLogEntry> m_errorLog;
    std::map<int, ErrorCallback> m_errorCallbacks;
    std::map<CompressionErrorType, RecoveryStrategy> m_recoveryStrategies;
    int m_nextCallbackId = 1;
    mutable std::mutex m_mutex;
};

/**
 * @brief Função global para registro de erros
 * @param errorType Tipo do erro
 * @param message Mensagem de erro
 * @param details Detalhes adicionais (opcional)
 * @param level Nível de severidade (opcional)
 * @param recovered Se o erro foi recuperado (opcional)
 */
inline void logCompressionError(
    CompressionErrorType errorType,
    const std::string& message,
    const std::string& details = "",
    LogLevel level = LogLevel::ERROR,
    bool recovered = false
) {
    ErrorHandler::getInstance().logError(
        errorType, message, details, level, recovered
    );
}

/**
 * @brief Função global para tentativa de recuperação de erros
 * @param errorType Tipo do erro
 * @param context Contexto adicional para recuperação
 * @return Verdadeiro se a recuperação foi bem-sucedida
 */
inline bool attemptCompressionRecovery(
    CompressionErrorType errorType,
    const std::map<std::string, std::string>& context = {}
) {
    return ErrorHandler::getInstance().attemptRecovery(errorType, context);
}

} // namespace compression
} // namespace wydbr

#endif // WYDBR_COMPRESSION_ERROR_HANDLER_H