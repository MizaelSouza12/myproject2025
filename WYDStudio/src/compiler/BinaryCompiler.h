#ifndef WYDSTUDIO_BINARY_COMPILER_H
#define WYDSTUDIO_BINARY_COMPILER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <optional>
#include <filesystem>
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace compiler {

/**
 * @brief Contexto para compilação de dados
 */
struct CompilationContext {
    enum class SourceType {
        JSON,           // Dados em formato JSON
        CSV,            // Dados em formato CSV
        TEXT,           // Dados em formato textual
        XML,            // Dados em formato XML
        BINARY,         // Dados em formato binário
        CUSTOM          // Formato customizado
    };
    
    enum class TargetType {
        WYD_BINARY,     // Formato binário do WYD
        TEXTURE,        // Textura
        MODEL,          // Modelo 3D
        EFFECT,         // Efeito
        SOUND,          // Som
        SCRIPT,         // Script
        CUSTOM          // Formato customizado
    };
    
    enum class CompressionType {
        NONE,
        ZLIB,
        LZ4,
        RLE,
        CUSTOM
    };
    
    enum class EncryptionType {
        NONE,
        XOR,
        AES,
        CUSTOM
    };
    
    // Tipo de fonte e alvo
    SourceType sourceType;
    TargetType targetType;
    
    // Opções de processamento
    bool validate;
    bool optimize;
    bool verbose;
    
    // Opções de compressão e criptografia
    CompressionType compression;
    EncryptionType encryption;
    std::vector<uint8_t> encryptionKey;
    
    // Metadados
    std::string version;
    std::unordered_map<std::string, std::string> metadata;
    
    // Estado da compilação
    std::filesystem::path sourcePath;
    std::filesystem::path targetPath;
    std::string lastError;
    
    // Formato específico
    std::string formatName;
    
    // Opções específicas de formato
    std::unordered_map<std::string, std::string> formatOptions;
    
    CompilationContext() 
        : sourceType(SourceType::JSON),
          targetType(TargetType::WYD_BINARY),
          validate(true),
          optimize(false),
          verbose(false),
          compression(CompressionType::NONE),
          encryption(EncryptionType::NONE),
          version("1.0") {}
};

/**
 * @brief Resultado de uma operação de compilação
 */
struct CompilationResult {
    bool success;
    std::string errorMessage;
    
    // Métricas
    size_t sourceSize;
    size_t targetSize;
    double compressionRatio;
    
    // Tempos de execução (ms)
    uint64_t validationTime;
    uint64_t conversionTime;
    uint64_t postProcessingTime;
    uint64_t totalTime;
    
    // Informações adicionais
    std::unordered_map<std::string, std::string> metadata;
    
    CompilationResult() 
        : success(false),
          sourceSize(0),
          targetSize(0),
          compressionRatio(1.0),
          validationTime(0),
          conversionTime(0),
          postProcessingTime(0),
          totalTime(0) {}
};

/**
 * @brief Regra de compilação que define como converter dados de um formato para outro
 */
class CompilationRule {
public:
    using ProcessFunction = std::function<bool(std::vector<uint8_t>&, CompilationContext&)>;
    
    CompilationRule(const std::string& name, ProcessFunction processFunc);
    virtual ~CompilationRule() = default;
    
    std::string getName() const { return m_name; }
    bool apply(std::vector<uint8_t>& data, CompilationContext& context) const;
    
private:
    std::string m_name;
    ProcessFunction m_processFunc;
};

/**
 * @brief Compilador de formatos binários
 */
class BinaryCompiler {
public:
    BinaryCompiler(const std::string& sourceFormat, const std::string& targetFormat);
    virtual ~BinaryCompiler();
    
    // Configuração
    void addCompilationRule(const std::string& ruleName, const CompilationRule& rule);
    void setCompilationContext(const CompilationContext& context);
    CompilationContext& getCompilationContext() { return m_context; }
    const CompilationContext& getCompilationContext() const { return m_context; }
    
    // Operações de compilação
    bool compile(const std::string& sourcePath, const std::string& targetPath);
    bool compileFromMemory(const std::vector<uint8_t>& sourceData, std::vector<uint8_t>& targetData);
    
    // Resultados e status
    CompilationResult getLastResult() const { return m_lastResult; }
    std::string getSourceFormat() const { return m_sourceFormat; }
    std::string getTargetFormat() const { return m_targetFormat; }
    std::string getLastError() const { return m_lastResult.errorMessage; }
    
    // Eventos
    core::EventBus::SubscriptionID subscribeToProgress(std::function<void(float)> callback);
    
private:
    std::string m_sourceFormat;
    std::string m_targetFormat;
    std::unordered_map<std::string, CompilationRule> m_rules;
    CompilationContext m_context;
    CompilationResult m_lastResult;
    
    // Processamento interno
    bool validateSourceData(const std::vector<uint8_t>& data);
    bool applyCompilationRules(std::vector<uint8_t>& data);
    bool performPostProcessing(std::vector<uint8_t>& data);
    
    // Carregamento e salvamento
    bool loadSourceFile(const std::string& sourcePath, std::vector<uint8_t>& data);
    bool saveTargetFile(const std::string& targetPath, const std::vector<uint8_t>& data);
    
    // Event bus para notificações de progresso
    std::shared_ptr<core::EventBus> m_eventBus;
    void updateProgress(float progress);
    
    // Utilitários
    void initializeDefaults();
};

/**
 * @brief Fábrica para criação de compiladores binários
 */
class BinaryCompilerFactory {
public:
    static BinaryCompilerFactory& getInstance();
    
    // Registro de compiladores
    bool registerCompiler(const std::string& sourceFormat, 
                         const std::string& targetFormat,
                         std::function<std::unique_ptr<BinaryCompiler>()> creator);
    
    // Criação de compiladores
    std::unique_ptr<BinaryCompiler> createCompiler(const std::string& sourceFormat, 
                                                 const std::string& targetFormat);
    
    // Informações de suporte
    bool isCompilationSupported(const std::string& sourceFormat, const std::string& targetFormat) const;
    std::vector<std::string> getSupportedSourceFormats() const;
    std::vector<std::string> getSupportedTargetFormats(const std::string& sourceFormat) const;
    std::vector<std::pair<std::string, std::string>> getAllSupportedFormatPairs() const;
    
private:
    BinaryCompilerFactory() = default;
    ~BinaryCompilerFactory() = default;
    
    // Prevenir cópia
    BinaryCompilerFactory(const BinaryCompilerFactory&) = delete;
    BinaryCompilerFactory& operator=(const BinaryCompilerFactory&) = delete;
    
    // Registro de criadores de compiladores
    using CompilerCreator = std::function<std::unique_ptr<BinaryCompiler>()>;
    std::unordered_map<std::string, CompilerCreator> m_compilers;
    
    // Gera uma chave única para o par de formatos
    std::string generateKey(const std::string& sourceFormat, const std::string& targetFormat) const;
};

/**
 * @brief Processador de lote para compilação de múltiplos arquivos
 */
struct BatchProcessingJob {
    enum class Mode {
        COMPILE,   // Compilar arquivos fonte para destino
        DECOMPLE   // Descompilar arquivos fonte para destino
    };
    
    struct FileMapping {
        std::string sourcePath;
        std::string targetPath;
        std::string sourceFormat;
        std::string targetFormat;
        CompilationContext context;
    };
    
    Mode mode;
    std::vector<FileMapping> files;
    std::string baseSourceDir;
    std::string baseTargetDir;
    bool continueOnError;
    std::function<void(size_t completed, size_t total)> progressCallback;
    
    BatchProcessingJob() : mode(Mode::COMPILE), continueOnError(false) {}
};

/**
 * @brief Resultado de processamento em lote
 */
struct BatchResult {
    size_t totalFiles;
    size_t successCount;
    size_t failureCount;
    std::vector<std::pair<std::string, std::string>> failures; // path, error
    std::unordered_map<std::string, CompilationResult> results;
    uint64_t totalTimeMs;
    
    BatchResult() : totalFiles(0), successCount(0), failureCount(0), totalTimeMs(0) {}
};

/**
 * @brief Gerenciador central de compilação
 */
class CompilationManager {
public:
    CompilationManager();
    ~CompilationManager();
    
    // Registro manual de compiladores
    bool registerCompiler(const std::string& sourceFormat, 
                         const std::string& targetFormat, 
                         const BinaryCompiler& compiler);
    
    bool registerDecompiler(const std::string& sourceFormat, 
                           const std::string& targetFormat, 
                           const BinaryCompiler& decompiler);
    
    // Verificação de suporte
    bool canCompile(const std::string& sourceFormat, const std::string& targetFormat) const;
    bool canDecompile(const std::string& sourceFormat, const std::string& targetFormat) const;
    
    // Operações de compilação
    bool compile(const std::string& sourceFormat, 
                const std::string& targetFormat, 
                const std::string& sourcePath, 
                const std::string& targetPath);
    
    bool decompile(const std::string& sourceFormat, 
                  const std::string& targetFormat, 
                  const std::string& sourcePath, 
                  const std::string& targetPath);
    
    // Processamento em lote
    BatchResult batchProcess(const BatchProcessingJob& job);
    
    // Informações sobre compiladores
    void exportSupportMatrix(const std::string& outputPath) const;
    std::string getFormatsTable() const;
    
    // Utilitários
    bool detectFileFormat(const std::string& filePath, std::string& formatName) const;
    
private:
    // Formatador de IDs
    std::string generateCompilerId(const std::string& sourceFormat, const std::string& targetFormat) const;
    
    // Inicialização
    bool initializeDefaultCompilers();
    bool initializeDefaultDecompilers();
    
    // Registros
    BinaryCompilerFactory& m_compilerFactory;
    
    // Formatação de erros
    std::string formatErrorMessage(const std::string& operation, 
                                  const std::string& sourceFormat,
                                  const std::string& targetFormat,
                                  const std::string& message) const;
};

} // namespace compiler
} // namespace wydstudio

#endif // WYDSTUDIO_BINARY_COMPILER_H