/**
 * @file SecureRandom.h
 * @brief Gerador de números aleatórios criptograficamente seguro
 */

#ifndef WYDBR_SECURE_RANDOM_H
#define WYDBR_SECURE_RANDOM_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include <mutex>
#include <array>

namespace WYDBR {
namespace Security {

/**
 * Enumeração dos algoritmos de geração de entropia
 */
enum class EntropySource {
    HardwareRNG,     // Gerador de hardware (RDRAND/RDSEED)
    SystemRNG,       // /dev/urandom ou equivalente
    MixedSources,    // Combinação de fontes
    TimeBased,       // Baseado em tempo (menos seguro, fallback)
    Custom           // Fonte personalizada
};

/**
 * Classe para geração de números aleatórios criptograficamente seguros
 * 
 * Esta implementação corrige o problema de segurança crítico da versão anterior,
 * que usava seed previsível baseado apenas em time(nullptr).
 */
class SecureRandom {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerador
     */
    static SecureRandom& GetInstance();
    
    /**
     * Inicializa o gerador com a fonte de entropia especificada
     * @param source Fonte de entropia
     * @return true se inicializado com sucesso
     */
    bool Initialize(EntropySource source = EntropySource::MixedSources);
    
    /**
     * Obtém bytes aleatórios
     * @param buffer Buffer para receber os bytes
     * @param size Número de bytes a gerar
     * @return true se gerado com sucesso
     */
    bool GetRandomBytes(uint8_t* buffer, size_t size);
    
    /**
     * Obtém valor inteiro aleatório no intervalo [min, max]
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Valor aleatório
     */
    uint64_t GetRandomUInt64(uint64_t min = 0, uint64_t max = UINT64_MAX);
    
    /**
     * Obtém valor inteiro aleatório de 32 bits no intervalo [min, max]
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Valor aleatório
     */
    uint32_t GetRandomUInt32(uint32_t min = 0, uint32_t max = UINT32_MAX);
    
    /**
     * Obtém valor inteiro aleatório de 16 bits no intervalo [min, max]
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Valor aleatório
     */
    uint16_t GetRandomUInt16(uint16_t min = 0, uint16_t max = UINT16_MAX);
    
    /**
     * Obtém valor inteiro aleatório de 8 bits no intervalo [min, max]
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Valor aleatório
     */
    uint8_t GetRandomUInt8(uint8_t min = 0, uint8_t max = UINT8_MAX);
    
    /**
     * Obtém valor de ponto flutuante aleatório no intervalo [min, max)
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Valor aleatório
     */
    double GetRandomDouble(double min = 0.0, double max = 1.0);
    
    /**
     * Gera string aleatória de tamanho especificado
     * @param length Tamanho da string
     * @param charset Conjunto de caracteres a usar
     * @return String aleatória
     */
    std::string GetRandomString(size_t length, const std::string& charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    
    /**
     * Embaralha um vetor aleatoriamente
     * @param vec Vetor a embaralhar
     */
    template<typename T>
    void Shuffle(std::vector<T>& vec) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::shuffle(vec.begin(), vec.end(), m_generator);
    }
    
    /**
     * Verifica se o sistema suporta geração de números aleatórios por hardware
     * @return true se suportado
     */
    static bool IsHardwareRNGSupported();
    
    /**
     * Verifica se o sistema suporta geração de números aleatórios pelo sistema
     * @return true se suportado
     */
    static bool IsSystemRNGSupported();
    
private:
    // Construtor e destrutor privados (singleton)
    SecureRandom();
    ~SecureRandom();
    
    // Não permitir cópias
    SecureRandom(const SecureRandom&) = delete;
    SecureRandom& operator=(const SecureRandom&) = delete;
    
    // Geradores de números aleatórios
    std::mt19937_64 m_generator;
    
    // Mutex para thread-safety
    std::mutex m_mutex;
    
    // Fonte de entropia atual
    EntropySource m_entropySource;
    
    // Indicador de inicialização
    bool m_initialized;
    
    // Constante para uso em reseed periódico
    static constexpr size_t RESEED_INTERVAL = 10000;
    
    // Contador para reseed
    size_t m_generationCounter;
    
    // Buffer de pool de entropia
    std::array<uint8_t, 256> m_entropyPool;
    
    // Métodos privados
    void ReseedGenerator();
    bool SeedFromHardware();
    bool SeedFromSystem();
    bool SeedFromMixedSources();
    bool SeedFromTimeBased();
    void UpdateEntropyPool();
    void MixEntropyPool();
};

} // namespace Security
} // namespace WYDBR

#endif // WYDBR_SECURE_RANDOM_H
