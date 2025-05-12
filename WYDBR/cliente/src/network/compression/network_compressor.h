#ifndef WYDBR_NETWORK_COMPRESSOR_H
#define WYDBR_NETWORK_COMPRESSOR_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <mutex>

namespace wydbr {
namespace network {

// Enumeração para algoritmo de compressão
enum class CompressionAlgorithm {
    LZ4,
    ZSTD,
    DEFLATE,
    BROTLI,
    NONE
};

// Estrutura para configuração de compressão
struct CompressionConfig {
    CompressionAlgorithm algorithm;
    int compressionLevel;
    int minSizeToCompress;
    bool useDictionaryCompression;
    bool adaptiveCompression;
    bool compressHeaders;
    int windowSize;
    
    // Construtor com valores padrão
    CompressionConfig() :
        algorithm(CompressionAlgorithm::ZSTD),
        compressionLevel(3),
        minSizeToCompress(64),
        useDictionaryCompression(true),
        adaptiveCompression(true),
        compressHeaders(false),
        windowSize(32768)
    {}
};

// Estrutura para estatísticas de compressão
struct CompressionStats {
    int64_t originalBytes;
    int64_t compressedBytes;
    int packetsCompressed;
    int packetsSkipped;
    float averageCompressionRatio;
    float averageCompressionTime;
    float averageDecompressionTime;
    int dictionarySize;
    int dictionaryEntries;
    
    // Construtor
    CompressionStats() :
        originalBytes(0),
        compressedBytes(0),
        packetsCompressed(0),
        packetsSkipped(0),
        averageCompressionRatio(0.0f),
        averageCompressionTime(0.0f),
        averageDecompressionTime(0.0f),
        dictionarySize(0),
        dictionaryEntries(0)
    {}
};

/**
 * @class NetworkCompressor
 * @brief Compressor de pacotes de rede
 * 
 * Sistema de compressão otimizado para pacotes de rede pequenos e frequentes,
 * com suporte a compressão adaptativa e baseada em dicionário.
 */
class NetworkCompressor {
public:
    NetworkCompressor();
    ~NetworkCompressor();
    
    /**
     * @brief Inicializa o compressor
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const CompressionConfig& config = CompressionConfig());
    
    /**
     * @brief Desliga o compressor e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Comprime dados
     * @param data Dados a comprimir
     * @param packetType Tipo do pacote (para compressão baseada em tipo)
     * @return Dados comprimidos, ou dados originais se a compressão não for eficaz
     */
    std::vector<uint8_t> compress(const std::vector<uint8_t>& data, uint8_t packetType = 0);
    
    /**
     * @brief Descomprime dados
     * @param compressedData Dados comprimidos
     * @param packetType Tipo do pacote (para descompressão baseada em tipo)
     * @return Dados descomprimidos
     */
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressedData, uint8_t packetType = 0);
    
    /**
     * @brief Comprime uma string
     * @param text Texto a comprimir
     * @param packetType Tipo do pacote (para compressão baseada em tipo)
     * @return Dados comprimidos
     */
    std::vector<uint8_t> compressText(const std::string& text, uint8_t packetType = 0);
    
    /**
     * @brief Descomprime para string
     * @param compressedData Dados comprimidos
     * @param packetType Tipo do pacote (para descompressão baseada em tipo)
     * @return Texto descomprimido
     */
    std::string decompressText(const std::vector<uint8_t>& compressedData, uint8_t packetType = 0);
    
    /**
     * @brief Define o algoritmo de compressão
     * @param algorithm Novo algoritmo
     */
    void setAlgorithm(CompressionAlgorithm algorithm);
    
    /**
     * @brief Obtém o algoritmo atual
     * @return Algoritmo atual
     */
    CompressionAlgorithm getAlgorithm() const;
    
    /**
     * @brief Define o nível de compressão
     * @param level Novo nível (0-9, dependendo do algoritmo)
     */
    void setCompressionLevel(int level);
    
    /**
     * @brief Obtém o nível de compressão atual
     * @return Nível atual
     */
    int getCompressionLevel() const;
    
    /**
     * @brief Define o tamanho mínimo para comprimir
     * @param minSize Tamanho mínimo em bytes
     */
    void setMinSizeToCompress(int minSize);
    
    /**
     * @brief Obtém o tamanho mínimo para comprimir
     * @return Tamanho mínimo em bytes
     */
    int getMinSizeToCompress() const;
    
    /**
     * @brief Ativa ou desativa a compressão baseada em dicionário
     * @param enable Se deve ativar
     */
    void enableDictionaryCompression(bool enable);
    
    /**
     * @brief Verifica se a compressão baseada em dicionário está ativa
     * @return true se ativa
     */
    bool isDictionaryCompressionEnabled() const;
    
    /**
     * @brief Ativa ou desativa a compressão adaptativa
     * @param enable Se deve ativar
     */
    void enableAdaptiveCompression(bool enable);
    
    /**
     * @brief Verifica se a compressão adaptativa está ativa
     * @return true se ativa
     */
    bool isAdaptiveCompressionEnabled() const;
    
    /**
     * @brief Adiciona uma sequência ao dicionário
     * @param sequence Sequência a adicionar
     * @param weight Peso da sequência (frequência de uso)
     */
    void addToDictionary(const std::vector<uint8_t>& sequence, int weight = 1);
    
    /**
     * @brief Limpa o dicionário
     */
    void clearDictionary();
    
    /**
     * @brief Salva o dicionário em um arquivo
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool saveDictionary(const std::string& filePath) const;
    
    /**
     * @brief Carrega o dicionário de um arquivo
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool loadDictionary(const std::string& filePath);
    
    /**
     * @brief Obtém estatísticas de compressão
     * @return Estatísticas
     */
    CompressionStats getStatistics() const;
    
    /**
     * @brief Reseta as estatísticas
     */
    void resetStatistics();
    
    /**
     * @brief Verifica se dados específicos podem ser comprimidos eficientemente
     * @param data Dados a verificar
     * @return true se a compressão for recomendada
     */
    bool shouldCompress(const std::vector<uint8_t>& data) const;
    
    /**
     * @brief Estima a compressão para um pacote
     * @param data Dados a comprimir
     * @return Razão de compressão estimada (0.0-1.0, menor é melhor)
     */
    float estimateCompressionRatio(const std::vector<uint8_t>& data) const;

private:
    // Implementações específicas de algoritmos
    std::vector<uint8_t> compressLZ4(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressLZ4(const std::vector<uint8_t>& compressedData);
    
    std::vector<uint8_t> compressZSTD(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressZSTD(const std::vector<uint8_t>& compressedData);
    
    std::vector<uint8_t> compressDeflate(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressDeflate(const std::vector<uint8_t>& compressedData);
    
    std::vector<uint8_t> compressBrotli(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressBrotli(const std::vector<uint8_t>& compressedData);
    
    // Métodos de dicionário
    void buildDictionary();
    void updateDictionary(const std::vector<uint8_t>& data);
    
    // Estado interno
    bool m_initialized;
    CompressionConfig m_config;
    CompressionStats m_stats;
    
    // Dicionários de compressão por tipo de pacote
    std::unordered_map<uint8_t, std::vector<uint8_t>> m_dictionaries;
    
    // Dados para construção de dicionário adaptativo
    struct SequenceInfo {
        std::vector<uint8_t> data;
        int frequency;
        int lastUsedTimestamp;
    };
    std::vector<SequenceInfo> m_dictionarySequences;
    
    // Contextos específicos de compressão
    void* m_lz4Context;
    void* m_zstdContext;
    void* m_deflateContext;
    void* m_brotliContext;
    
    // Thread safety
    mutable std::mutex m_mutex;
    mutable std::mutex m_statsMutex;
};

} // namespace network
} // namespace wydbr

#endif // WYDBR_NETWORK_COMPRESSOR_H
