/**
 * @file compression-exceptions.h
 * @brief Definições de exceções para o sistema de compressão
 *
 * Este arquivo contém todas as definições de exceções e tipos de erro
 * usados pelo sistema de compressão dinâmica WYDBR.
 */

#ifndef WYDBR_COMPRESSION_COMPRESSION_EXCEPTIONS_H
#define WYDBR_COMPRESSION_COMPRESSION_EXCEPTIONS_H

#include <string>
#include <stdexcept>

namespace wydbr {
namespace compression {

/**
 * @brief Tipos de erro de compressão
 */
enum class CompressionErrorType {
    INVALID_DATA,      ///< Dados de entrada inválidos
    INVALID_ALGORITHM, ///< Algoritmo de compressão inválido
    COMPRESSION_FAILED,///< Falha geral na compressão
    DECOMPRESSION_FAILED, ///< Falha geral na descompressão
    BUFFER_TOO_SMALL,  ///< Buffer de destino muito pequeno
    NULL_POINTER,      ///< Ponteiro nulo passado para função
    CORRUPTED_DATA,    ///< Dados corrompidos
    FORMAT_MISMATCH,   ///< Formato incompatível
    MEMORY_ERROR,      ///< Erro de memória
    LIBRARY_ERROR,     ///< Erro em biblioteca externa
    UNKNOWN_ERROR      ///< Erro desconhecido
};

/**
 * @brief Converte tipo de erro para string
 * @param errorType Tipo de erro
 * @return Nome do tipo de erro
 */
inline std::string CompressionErrorTypeToString(CompressionErrorType errorType) {
    switch (errorType) {
        case CompressionErrorType::INVALID_DATA: return "INVALID_DATA";
        case CompressionErrorType::INVALID_ALGORITHM: return "INVALID_ALGORITHM";
        case CompressionErrorType::COMPRESSION_FAILED: return "COMPRESSION_FAILED";
        case CompressionErrorType::DECOMPRESSION_FAILED: return "DECOMPRESSION_FAILED";
        case CompressionErrorType::BUFFER_TOO_SMALL: return "BUFFER_TOO_SMALL";
        case CompressionErrorType::NULL_POINTER: return "NULL_POINTER";
        case CompressionErrorType::CORRUPTED_DATA: return "CORRUPTED_DATA";
        case CompressionErrorType::FORMAT_MISMATCH: return "FORMAT_MISMATCH";
        case CompressionErrorType::MEMORY_ERROR: return "MEMORY_ERROR";
        case CompressionErrorType::LIBRARY_ERROR: return "LIBRARY_ERROR";
        case CompressionErrorType::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
        default: return "UNDEFINED_ERROR";
    }
}

/**
 * @brief Níveis de log para erros de compressão
 */
enum class LogLevel {
    DEBUG,    ///< Informações detalhadas, úteis para debug
    INFO,     ///< Informações gerais
    WARNING,  ///< Avisos que devem ser observados
    ERROR,    ///< Erros que permitem a continuação do programa
    CRITICAL  ///< Erros críticos que podem impedir a continuação
};

/**
 * @brief Exceção para erros de compressão
 */
class CompressionException : public std::runtime_error {
public:
    /**
     * @brief Construtor padrão
     * @param message Mensagem de erro
     * @param errorType Tipo do erro
     * @param recoverable Se é possível recuperação
     * @param details Detalhes adicionais (opcional)
     */
    CompressionException(
        const std::string& message,
        CompressionErrorType errorType = CompressionErrorType::UNKNOWN_ERROR,
        bool recoverable = false,
        const std::string& details = ""
    ) : std::runtime_error(message),
        m_errorType(errorType),
        m_recoverable(recoverable),
        m_details(details)
    {
    }
    
    /**
     * @brief Obtém tipo do erro
     * @return Tipo do erro
     */
    CompressionErrorType getErrorType() const {
        return m_errorType;
    }
    
    /**
     * @brief Verifica se o erro é recuperável
     * @return Verdadeiro se for recuperável
     */
    bool isRecoverable() const {
        return m_recoverable;
    }
    
    /**
     * @brief Obtém detalhes do erro
     * @return Detalhes do erro
     */
    const std::string& getDetails() const {
        return m_details;
    }
    
    /**
     * @brief Obtém mensagem formatada com todos os detalhes
     * @return Mensagem completa
     */
    std::string getFormattedMessage() const {
        std::string result = what();
        result += " [" + CompressionErrorTypeToString(m_errorType) + "]";
        if (!m_details.empty()) {
            result += " - " + m_details;
        }
        result += m_recoverable ? " (Recuperável)" : " (Não recuperável)";
        return result;
    }
    
private:
    CompressionErrorType m_errorType; ///< Tipo do erro
    bool m_recoverable;               ///< Se é possível recuperação
    std::string m_details;            ///< Detalhes adicionais
};

/**
 * @brief Estrutura para armazenar entradas de log de erro
 */
struct ErrorLogEntry {
    CompressionErrorType errorType;  ///< Tipo do erro
    std::string message;             ///< Mensagem de erro
    std::string details;             ///< Detalhes adicionais
    LogLevel level;                  ///< Nível de severidade
    bool recovered;                  ///< Se o erro foi recuperado
    long long timestamp;             ///< Timestamp do erro (milissegundos)
    
    /**
     * @brief Construtor padrão
     */
    ErrorLogEntry() : 
        errorType(CompressionErrorType::UNKNOWN_ERROR),
        level(LogLevel::ERROR),
        recovered(false),
        timestamp(0)
    {}
    
    /**
     * @brief Construtor com parâmetros
     * @param type Tipo do erro
     * @param msg Mensagem de erro
     * @param det Detalhes adicionais (opcional)
     * @param lvl Nível de severidade (opcional)
     * @param rec Se o erro foi recuperado (opcional)
     */
    ErrorLogEntry(
        CompressionErrorType type,
        const std::string& msg,
        const std::string& det = "",
        LogLevel lvl = LogLevel::ERROR,
        bool rec = false
    ) : 
        errorType(type),
        message(msg),
        details(det),
        level(lvl),
        recovered(rec),
        timestamp(getCurrentTimestamp())
    {}
    
private:
    /**
     * @brief Obtém timestamp atual em milissegundos
     */
    static long long getCurrentTimestamp() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
    }
};

/**
 * @brief Função para registrar um erro de compressão
 * @param errorType Tipo do erro
 * @param message Mensagem de erro
 * @param details Detalhes adicionais (opcional)
 * @param level Nível de severidade (opcional)
 * @param recovered Se o erro foi recuperado (opcional)
 */
void logCompressionError(
    CompressionErrorType errorType,
    const std::string& message,
    const std::string& details = "",
    LogLevel level = LogLevel::ERROR,
    bool recovered = false
);

/**
 * @brief Função para tentar recuperação de um erro de compressão
 * @param errorType Tipo do erro
 * @param context Contexto adicional para recuperação
 * @return Verdadeiro se a recuperação for possível
 */
bool attemptCompressionRecovery(
    CompressionErrorType errorType,
    const std::map<std::string, std::string>& context
);

} // namespace compression
} // namespace wydbr

#endif // WYDBR_COMPRESSION_COMPRESSION_EXCEPTIONS_H