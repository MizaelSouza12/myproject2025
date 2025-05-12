/**
 * @file compression_error_test.cpp
 * @brief Testes do sistema de tratamento de exceções de compressão
 */

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <chrono>
#include <thread>

#include "compression/dynamic-compression.h"
#include "compression/adaptive-compressor.h"
#include "compression/compression-exceptions.h"
#include "compression/error-handler.h"

using namespace wydbr::compression;

// Contador de erros para callback
static int errorCounter = 0;

// Callback de erro para testes
void errorCallbackFunction(const ErrorLogEntry& entry) {
    std::cout << "[CALLBACK] Erro detectado: " << entry.message 
              << " [" << CompressionErrorTypeToString(entry.errorType) << "]" << std::endl;
    if (!entry.details.empty()) {
        std::cout << "  Detalhes: " << entry.details << std::endl;
    }
    errorCounter++;
}

// Testa erro de ponteiro nulo
void testNullPointerError() {
    std::cout << "\n=== Teste 1: Ponteiro Nulo ===" << std::endl;
    
    Dynamiccompression compressor;
    compressor.Initialize();
    
    // Modo sem exceções
    compressor.setExceptionMode(false);
    
    try {
        const void* nullPtr = nullptr;
        std::cout << "Tentando comprimir dados nulos (modo sem exceções)..." << std::endl;
        auto compressed = compressor.compress(nullPtr, 0, DataType::BINARY);
        std::cout << "Compressão retornou vetor vazio em vez de lançar exceção" << std::endl;
        assert(compressed.empty());
        
        // Verificar último erro
        ErrorLogEntry lastError = compressor.getLastError();
        std::cout << "Último erro: " << lastError.message << std::endl;
        assert(lastError.errorType == CompressionErrorType::NULL_POINTER);
        
        // Modo com exceções
        compressor.setExceptionMode(true);
        std::cout << "Tentando comprimir dados nulos (modo com exceções)..." << std::endl;
        try {
            compressed = compressor.compress(nullPtr, 0, DataType::BINARY);
            std::cout << "[FALHA] Exceção não foi lançada!" << std::endl;
            assert(false);
        }
        catch (const CompressionException& ex) {
            std::cout << "Exceção capturada como esperado: " << ex.what() << std::endl;
            assert(ex.getErrorType() == CompressionErrorType::NULL_POINTER);
        }
    }
    catch (const std::exception& ex) {
        std::cout << "[ERRO INESPERADO] " << ex.what() << std::endl;
        assert(false);
    }
    
    std::cout << "Teste de ponteiro nulo concluído com sucesso" << std::endl;
}

// Testa registro de erros e callbacks
void testErrorLogging() {
    std::cout << "\n=== Teste 2: Registro de Erros e Callbacks ===" << std::endl;
    
    // Limpar log de erros anterior
    ErrorHandler::getInstance().clearErrorLog();
    errorCounter = 0;
    
    Adaptivecompressor compressor;
    compressor.Initialize();
    compressor.setExceptionMode(false);
    
    // Registrar um callback
    int callbackId = compressor.addErrorCallback(errorCallbackFunction);
    std::cout << "Callback registrado com ID: " << callbackId << std::endl;
    
    // Gerar alguns erros
    std::cout << "Gerando erros propositalmente..." << std::endl;
    
    // Erro de ponteiro nulo
    compressor.compress(nullptr, 100, CompressionFormat::BINARY);
    
    // Erro de tamanho inválido
    const char* validPtr = "Teste";
    compressor.compress(validPtr, 0, CompressionFormat::TEXT);
    
    // Recuperar o log de erros
    std::vector<ErrorLogEntry> errorLog = ErrorHandler::getInstance().getErrorLog();
    std::cout << "Log de erros contém " << errorLog.size() << " entradas" << std::endl;
    
    // Verificar se o callback foi chamado
    std::cout << "Callback foi chamado " << errorCounter << " vezes" << std::endl;
    assert(errorCounter == 2);
    
    // Verificar se podemos remover o callback
    bool removed = compressor.removeErrorCallback(callbackId);
    std::cout << "Callback removido: " << (removed ? "Sim" : "Não") << std::endl;
    assert(removed);
    
    // Gerar mais um erro para verificar que o callback não é mais chamado
    errorCounter = 0;
    compressor.compress(nullptr, 100, CompressionFormat::BINARY);
    assert(errorCounter == 0);
    
    std::cout << "Teste de registro de erros concluído com sucesso" << std::endl;
}

// Testa estratégias de recuperação
void testRecoveryStrategies() {
    std::cout << "\n=== Teste 3: Estratégias de Recuperação ===" << std::endl;
    
    // Teste para o algoritmo inválido (deve usar algoritmo padrão)
    std::map<std::string, std::string> context;
    bool recovered = ErrorHandler::getInstance().attemptRecovery(CompressionErrorType::INVALID_ALGORITHM, context);
    std::cout << "Recuperação para INVALID_ALGORITHM: " << (recovered ? "Sucesso" : "Falha") << std::endl;
    assert(recovered);
    
    // Teste para falha de compressão (deve permitir até 3 tentativas)
    for (int i = 0; i < 4; i++) {
        context["retry_count"] = std::to_string(i);
        recovered = ErrorHandler::getInstance().attemptRecovery(CompressionErrorType::COMPRESSION_FAILED, context);
        std::cout << "Recuperação para COMPRESSION_FAILED (tentativa " << i+1 << "): " 
                  << (recovered ? "Sucesso" : "Falha") << std::endl;
        assert(recovered == (i < 3));  // Verdadeiro para i < 3, falso para i >= 3
    }
    
    // Teste para erro de dados corrompidos (não é possível recuperar)
    recovered = ErrorHandler::getInstance().attemptRecovery(CompressionErrorType::CORRUPTED_DATA, context);
    std::cout << "Recuperação para CORRUPTED_DATA: " << (recovered ? "Sucesso" : "Falha") << std::endl;
    assert(!recovered);
    
    std::cout << "Teste de estratégias de recuperação concluído com sucesso" << std::endl;
}

// Testa estatísticas de erro
void testErrorStatistics() {
    std::cout << "\n=== Teste 4: Estatísticas de Erros ===" << std::endl;
    
    // Limpar log de erros anterior
    ErrorHandler::getInstance().clearErrorLog();
    
    // Gerar diferentes tipos de erros
    logCompressionError(CompressionErrorType::NULL_POINTER, "Teste de estatísticas 1");
    logCompressionError(CompressionErrorType::NULL_POINTER, "Teste de estatísticas 2");
    logCompressionError(CompressionErrorType::INVALID_DATA, "Teste de estatísticas 3");
    logCompressionError(CompressionErrorType::COMPRESSION_FAILED, "Teste de estatísticas 4");
    
    // Obter estatísticas
    auto stats = ErrorHandler::getInstance().getErrorStats();
    std::cout << "Estatísticas de erro:" << std::endl;
    for (const auto& pair : stats) {
        std::cout << "- " << CompressionErrorTypeToString(pair.first) << ": " << pair.second << std::endl;
    }
    
    // Verificar contagens
    assert(stats[CompressionErrorType::NULL_POINTER] == 2);
    assert(stats[CompressionErrorType::INVALID_DATA] == 1);
    assert(stats[CompressionErrorType::COMPRESSION_FAILED] == 1);
    
    std::cout << "Teste de estatísticas de erro concluído com sucesso" << std::endl;
}

// Função principal
int main() {
    std::cout << "====================================" << std::endl;
    std::cout << "  TESTE DO SISTEMA DE EXCEÇÕES DE   " << std::endl;
    std::cout << "           COMPRESSÃO               " << std::endl;
    std::cout << "====================================" << std::endl;
    
    try {
        testNullPointerError();
        testErrorLogging();
        testRecoveryStrategies();
        testErrorStatistics();
        
        std::cout << "\n=====================================" << std::endl;
        std::cout << " TODOS OS TESTES CONCLUÍDOS COM SUCESSO " << std::endl;
        std::cout << "=====================================" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "\nERRO FATAL: " << ex.what() << std::endl;
        return 1;
    }
    
    return 0;
}