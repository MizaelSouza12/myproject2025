#ifndef WYDSTUDIO_ADVANCED_BINARY_ANALYZER_H
#define WYDSTUDIO_ADVANCED_BINARY_ANALYZER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <variant>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace wydstudio {
namespace decompilers {

using json = nlohmann::json;
namespace fs = std::filesystem;

/**
 * @brief Tipo de arquivo binário
 */
enum class BinaryFileType {
    ITEM_LIST,          // Lista de itens
    ITEM_NAME,          // Nomes de itens
    ITEM_HELP,          // Descrições de itens
    ITEM_PRICE,         // Preços de itens
    MAP_DATA,           // Dados de mapa
    MAP_HEIGHT,         // Altura do terreno
    MAP_ATTRIBUTE,      // Atributos do mapa
    NPC_DATA,           // Dados de NPCs
    MOB_DATA,           // Dados de monstros
    SKILL_DATA,         // Dados de habilidades
    QUEST_DATA,         // Dados de quests
    EVENT_DATA,         // Dados de eventos
    UI_COMPONENT,       // Componentes de interface (.wyt)
    UI_TEXTURE,         // Texturas de interface
    UI_CONFIG,          // Configuração de interface
    GAME_CONFIG,        // Configuração do jogo
    SERVER_CONFIG,      // Configuração do servidor
    TEXT_RESOURCE,      // Recursos de texto
    UNKNOWN             // Desconhecido
};

/**
 * @brief Formato de arquivo decompilado
 */
enum class DecompiledFormat {
    JSON,               // JSON (padrão)
    XML,                // XML
    CSV,                // CSV
    INI,                // INI
    YAML,               // YAML
    PLAIN_TEXT,         // Texto puro
    CUSTOM              // Formato personalizado
};

/**
 * @brief Estrutura de campo binário
 */
struct BinaryFieldStructure {
    std::string name;           // Nome do campo
    std::string type;           // Tipo do campo (int32, float, char[], etc.)
    int32_t offset;             // Offset em bytes
    int32_t size;               // Tamanho em bytes
    bool isArray;               // Se é um array
    int32_t arrayCount;         // Contagem de itens no array
    bool isOptional;            // Se o campo é opcional
    std::string description;    // Descrição do campo
    std::map<std::string, std::variant<std::string, int, bool>> attributes; // Atributos adicionais
};

/**
 * @brief Estrutura binária completa
 */
struct BinaryStructure {
    std::string name;           // Nome da estrutura
    std::string description;    // Descrição
    int32_t headerSize;         // Tamanho do cabeçalho em bytes
    int32_t recordSize;         // Tamanho de cada registro em bytes
    bool hasHeader;             // Se possui cabeçalho
    bool isFixedSize;           // Se tem tamanho fixo
    std::vector<BinaryFieldStructure> headerFields; // Campos do cabeçalho
    std::vector<BinaryFieldStructure> recordFields; // Campos de cada registro
    std::vector<uint8_t> signature;   // Assinatura para identificação
    std::map<std::string, std::variant<std::string, int, bool>> attributes; // Atributos adicionais
};

/**
 * @brief Resultado de detecção de arquivo
 */
struct BinaryDetectionResult {
    BinaryFileType fileType;
    std::string detectedFormat;
    double confidence;          // 0.0 a 1.0
    std::optional<BinaryStructure> structure;
    std::string errorMessage;
};

/**
 * @brief Mapeamento de estrutura binária
 */
struct BinaryMapping {
    std::string fileExtension;
    BinaryFileType fileType;
    BinaryStructure structure;
    std::vector<std::string> signatures;
    std::map<std::string, std::string> customPatterns;
};

/**
 * @brief Resultado de decompilação
 */
struct BinaryDecompilationResult {
    bool success;
    std::string targetPath;     // Caminho do arquivo decompilado
    std::string format;         // Formato do arquivo decompilado
    std::string errorMessage;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Resultado de compilação
 */
struct BinaryCompilationResult {
    bool success;
    std::string targetPath;     // Caminho do arquivo compilado
    std::string errorMessage;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Analisador avançado de arquivos binários
 */
class AdvancedBinaryAnalyzer {
public:
    /**
     * @brief Construtor
     */
    AdvancedBinaryAnalyzer();
    
    /**
     * @brief Destrutor
     */
    ~AdvancedBinaryAnalyzer();
    
    /**
     * @brief Inicializa o analisador
     * @param definitionsPath Caminho para as definições de estruturas binárias
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::string& definitionsPath);
    
    /**
     * @brief Detecta o tipo e estrutura de um arquivo binário
     * @param filePath Caminho do arquivo
     * @return Resultado da detecção
     */
    BinaryDetectionResult detectFile(const std::string& filePath);
    
    /**
     * @brief Decompila um arquivo binário
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo (ou UNKNOWN para autodetecção)
     * @param targetFormat Formato de destino (ou vazio para JSON)
     * @param targetPath Caminho de destino (ou vazio para automático)
     * @return Resultado da decompilação
     */
    BinaryDecompilationResult decompileFile(const std::string& filePath, 
                                           BinaryFileType fileType = BinaryFileType::UNKNOWN,
                                           const std::string& targetFormat = "",
                                           const std::string& targetPath = "");
    
    /**
     * @brief Compila um arquivo decompilado de volta para binário
     * @param filePath Caminho do arquivo fonte
     * @param fileType Tipo do arquivo
     * @param targetPath Caminho de destino (ou vazio para automático)
     * @return Resultado da compilação
     */
    BinaryCompilationResult compileFile(const std::string& filePath,
                                       BinaryFileType fileType,
                                       const std::string& targetPath = "");
    
    /**
     * @brief Obtém a estrutura binária para um tipo de arquivo
     * @param fileType Tipo do arquivo
     * @return Estrutura ou nulo se não encontrada
     */
    std::optional<BinaryStructure> getStructure(BinaryFileType fileType);
    
    /**
     * @brief Verifica se um arquivo pode ser editado diretamente
     * @param filePath Caminho do arquivo
     * @return true se pode ser editado diretamente
     */
    bool canEditDirectly(const std::string& filePath);
    
    /**
     * @brief Registra um novo mapeamento de estrutura binária
     * @param mapping Mapeamento a ser registrado
     * @return true se registrado com sucesso
     */
    bool registerMapping(const BinaryMapping& mapping);
    
    /**
     * @brief Atualiza um mapeamento existente
     * @param fileType Tipo do arquivo
     * @param mapping Novo mapeamento
     * @return true se atualizado com sucesso
     */
    bool updateMapping(BinaryFileType fileType, const BinaryMapping& mapping);
    
    /**
     * @brief Remove um mapeamento
     * @param fileType Tipo do arquivo
     * @return true se removido com sucesso
     */
    bool removeMapping(BinaryFileType fileType);
    
    /**
     * @brief Salva as definições de estruturas
     * @param outputPath Caminho de saída (ou vazio para padrão)
     * @return true se salvo com sucesso
     */
    bool saveDefinitions(const std::string& outputPath = "");
    
    /**
     * @brief Carrega as definições de estruturas
     * @param inputPath Caminho de entrada (ou vazio para padrão)
     * @return true se carregado com sucesso
     */
    bool loadDefinitions(const std::string& inputPath = "");
    
    /**
     * @brief Analisa a estrutura de um arquivo desconhecido
     * @param filePath Caminho do arquivo
     * @param sampleCount Número de amostras para análise
     * @return Estrutura inferida ou nulo se falhar
     */
    std::optional<BinaryStructure> analyzeUnknownStructure(const std::string& filePath, int32_t sampleCount = 10);
    
    /**
     * @brief Compara dois arquivos binários para detectar diferenças
     * @param filePath1 Caminho do primeiro arquivo
     * @param filePath2 Caminho do segundo arquivo
     * @param outputDiffPath Caminho para salvar as diferenças (ou vazio para não salvar)
     * @return Diferenças em formato JSON
     */
    json compareBinaryFiles(const std::string& filePath1, const std::string& filePath2, 
                           const std::string& outputDiffPath = "");
    
    /**
     * @brief Extrai um registro específico de um arquivo binário
     * @param filePath Caminho do arquivo
     * @param recordIndex Índice do registro
     * @param fileType Tipo do arquivo (ou UNKNOWN para autodetecção)
     * @return Registro em formato JSON ou nulo se falhar
     */
    std::optional<json> extractRecord(const std::string& filePath, int32_t recordIndex, 
                                    BinaryFileType fileType = BinaryFileType::UNKNOWN);
    
    /**
     * @brief Adiciona um registro a um arquivo binário
     * @param filePath Caminho do arquivo
     * @param recordData Dados do registro em JSON
     * @param fileType Tipo do arquivo
     * @return true se adicionado com sucesso
     */
    bool addRecord(const std::string& filePath, const json& recordData, BinaryFileType fileType);
    
    /**
     * @brief Atualiza um registro em um arquivo binário
     * @param filePath Caminho do arquivo
     * @param recordIndex Índice do registro
     * @param recordData Dados do registro em JSON
     * @param fileType Tipo do arquivo (ou UNKNOWN para autodetecção)
     * @return true se atualizado com sucesso
     */
    bool updateRecord(const std::string& filePath, int32_t recordIndex, 
                     const json& recordData, BinaryFileType fileType = BinaryFileType::UNKNOWN);
    
    /**
     * @brief Remove um registro de um arquivo binário
     * @param filePath Caminho do arquivo
     * @param recordIndex Índice do registro
     * @param fileType Tipo do arquivo (ou UNKNOWN para autodetecção)
     * @return true se removido com sucesso
     */
    bool removeRecord(const std::string& filePath, int32_t recordIndex, 
                     BinaryFileType fileType = BinaryFileType::UNKNOWN);
    
    /**
     * @brief Obtém o número de registros em um arquivo binário
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo (ou UNKNOWN para autodetecção)
     * @return Número de registros ou -1 se falhar
     */
    int32_t getRecordCount(const std::string& filePath, BinaryFileType fileType = BinaryFileType::UNKNOWN);
    
    /**
     * @brief Verifica se um arquivo binário é válido
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo (ou UNKNOWN para autodetecção)
     * @return true se válido
     */
    bool isValidBinaryFile(const std::string& filePath, BinaryFileType fileType = BinaryFileType::UNKNOWN);
    
    /**
     * @brief Repara um arquivo binário corrompido
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo (ou UNKNOWN para autodetecção)
     * @param backupPath Caminho para backup (ou vazio para automático)
     * @return true se reparado com sucesso
     */
    bool repairBinaryFile(const std::string& filePath, BinaryFileType fileType = BinaryFileType::UNKNOWN,
                         const std::string& backupPath = "");
    
    /**
     * @brief Obtém metadados de um arquivo binário
     * @param filePath Caminho do arquivo
     * @return Metadados em formato JSON ou nulo se falhar
     */
    std::optional<json> getBinaryMetadata(const std::string& filePath);
    
    /**
     * @brief Procura por valores específicos em um arquivo binário
     * @param filePath Caminho do arquivo
     * @param searchCriteria Critérios de busca em JSON
     * @param fileType Tipo do arquivo (ou UNKNOWN para autodetecção)
     * @return Resultados da busca em JSON
     */
    json searchInBinaryFile(const std::string& filePath, const json& searchCriteria,
                           BinaryFileType fileType = BinaryFileType::UNKNOWN);
    
    // Conversão entre enumerações e strings
    static std::string binaryFileTypeToString(BinaryFileType type);
    static BinaryFileType stringToBinaryFileType(const std::string& typeStr);
    static std::string decompiledFormatToString(DecompiledFormat format);
    static DecompiledFormat stringToDecompiledFormat(const std::string& formatStr);

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Mapeamentos de estruturas binárias
    std::map<BinaryFileType, BinaryMapping> m_mappings;
    
    // Caminho de definições
    std::string m_definitionsPath;
    
    // Métodos auxiliares
    bool loadStandardDefinitions();
    json parseHeader(const std::vector<uint8_t>& data, const BinaryStructure& structure);
    json parseRecord(const std::vector<uint8_t>& data, int32_t offset, const BinaryStructure& structure);
    std::vector<uint8_t> serializeHeader(const json& headerData, const BinaryStructure& structure);
    std::vector<uint8_t> serializeRecord(const json& recordData, const BinaryStructure& structure);
    
    // Detecção e análise
    BinaryFileType detectFileTypeBySignature(const std::vector<uint8_t>& header);
    BinaryFileType detectFileTypeByExtension(const std::string& filePath);
    BinaryFileType detectFileTypeByContent(const std::vector<uint8_t>& data);
    
    // Utilidades para decompilação/compilação
    std::string generateDecompiledPath(const std::string& filePath, const std::string& format);
    std::string generateCompiledPath(const std::string& filePath, BinaryFileType fileType);
    bool writeJsonFile(const json& data, const std::string& filePath);
    std::optional<json> readJsonFile(const std::string& filePath);
    
    // Conversão entre formatos
    json convertWYTtoJson(const std::vector<uint8_t>& data);
    std::vector<uint8_t> convertJsonToWYT(const json& data);
    
    // Análise de padrões
    bool detectPatterns(const std::vector<uint8_t>& data, std::map<std::string, int32_t>& patternOffsets);
    bool validateStructure(const BinaryStructure& structure);
    
    // Métodos específicos por tipo
    BinaryDecompilationResult decompileItemList(const std::string& filePath, const std::string& targetPath);
    BinaryDecompilationResult decompileItemName(const std::string& filePath, const std::string& targetPath);
    BinaryDecompilationResult decompileItemHelp(const std::string& filePath, const std::string& targetPath);
    BinaryDecompilationResult decompileUIComponent(const std::string& filePath, const std::string& targetPath);
    
    BinaryCompilationResult compileItemList(const std::string& filePath, const std::string& targetPath);
    BinaryCompilationResult compileItemName(const std::string& filePath, const std::string& targetPath);
    BinaryCompilationResult compileItemHelp(const std::string& filePath, const std::string& targetPath);
    BinaryCompilationResult compileUIComponent(const std::string& filePath, const std::string& targetPath);
};

}}  // namespace wydstudio::decompilers

#endif // WYDSTUDIO_ADVANCED_BINARY_ANALYZER_H