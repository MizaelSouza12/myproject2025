/**
 * @file CryptoManager.h
 * @brief Sistema de criptografia para comunicações sensíveis
 */

#ifndef WYDBR_CRYPTO_MANAGER_H
#define WYDBR_CRYPTO_MANAGER_H

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace WYDBR {
namespace Security {

/**
 * Tipos de algoritmos de criptografia
 */
enum class CryptoAlgorithm {
    AES_256_GCM,    // AES no modo GCM (recomendado)
    AES_256_CBC,    // AES no modo CBC
    ChaCha20Poly1305, // ChaCha20 com Poly1305 para autenticação
    NONE            // Sem criptografia
};

/**
 * Modos de operação
 */
enum class CryptoMode {
    ENCRYPT,        // Modo de encriptação
    DECRYPT         // Modo de decriptação
};

/**
 * Chave de criptografia
 */
struct CryptoKey {
    std::vector<uint8_t> keyData;    // Dados da chave
    std::vector<uint8_t> iv;         // Vetor de inicialização
    uint64_t creationTime;           // Timestamp de criação
    uint64_t expirationTime;         // Timestamp de expiração (0 = sem expiração)
    std::string keyId;               // ID único da chave
    CryptoAlgorithm algorithm;       // Algoritmo associado
    bool isValid;                    // Chave válida
};

/**
 * Resultado de operação criptográfica
 */
struct CryptoResult {
    std::vector<uint8_t> data;      // Dados resultantes
    bool success;                   // Operação bem-sucedida
    std::string error;              // Mensagem de erro (se houver)
    std::string keyId;              // ID da chave usada
};

/**
 * Classe para operações criptográficas
 */
class CryptoManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static CryptoManager& GetInstance();
    
    /**
     * Inicializa o gerenciador de criptografia
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * Encripta dados usando a chave especificada
     * @param data Dados a encriptar
     * @param size Tamanho dos dados
     * @param keyId ID da chave a usar (empty para chave padrão)
     * @return Resultado da operação
     */
    CryptoResult Encrypt(const uint8_t* data, size_t size, const std::string& keyId = "");
    
    /**
     * Decripta dados usando a chave especificada
     * @param data Dados a decriptar
     * @param size Tamanho dos dados
     * @param keyId ID da chave a usar (empty para chave padrão)
     * @return Resultado da operação
     */
    CryptoResult Decrypt(const uint8_t* data, size_t size, const std::string& keyId = "");
    
    /**
     * Encripta string
     * @param data String a encriptar
     * @param keyId ID da chave a usar (empty para chave padrão)
     * @return String encriptada em Base64
     */
    std::string EncryptString(const std::string& data, const std::string& keyId = "");
    
    /**
     * Decripta string
     * @param data String encriptada em Base64
     * @param keyId ID da chave a usar (empty para chave padrão)
     * @return String decriptada
     */
    std::string DecryptString(const std::string& data, const std::string& keyId = "");
    
    /**
     * Gera uma nova chave
     * @param algorithm Algoritmo a usar
     * @param expirationSeconds Tempo de expiração em segundos (0 = sem expiração)
     * @return ID da chave gerada
     */
    std::string GenerateKey(CryptoAlgorithm algorithm = CryptoAlgorithm::AES_256_GCM, 
                          uint64_t expirationSeconds = 0);
    
    /**
     * Define a chave padrão
     * @param keyId ID da chave
     * @return true se definida com sucesso
     */
    bool SetDefaultKey(const std::string& keyId);
    
    /**
     * Obtém o ID da chave padrão
     * @return ID da chave padrão
     */
    std::string GetDefaultKeyId() const;
    
    /**
     * Revoga uma chave
     * @param keyId ID da chave
     * @return true se revogada com sucesso
     */
    bool RevokeKey(const std::string& keyId);
    
    /**
     * Verifica se uma chave é válida
     * @param keyId ID da chave
     * @return true se válida
     */
    bool IsKeyValid(const std::string& keyId) const;
    
    /**
     * Exporta uma chave (apenas para armazenamento seguro)
     * @param keyId ID da chave
     * @return Dados da chave em formato seguro
     */
    std::vector<uint8_t> ExportKey(const std::string& keyId);
    
    /**
     * Importa uma chave
     * @param keyData Dados da chave
     * @return ID da chave importada
     */
    std::string ImportKey(const std::vector<uint8_t>& keyData);
    
    /**
     * Limpa chaves expiradas
     * @return Número de chaves limpas
     */
    int CleanupExpiredKeys();
    
private:
    // Construtor e destrutor privados (singleton)
    CryptoManager();
    ~CryptoManager();
    
    // Não permitir cópias
    CryptoManager(const CryptoManager&) = delete;
    CryptoManager& operator=(const CryptoManager&) = delete;
    
    // Mapa de chaves por ID
    std::unordered_map<std::string, CryptoKey> m_keys;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Chave padrão
    std::string m_defaultKeyId;
    
    // Estado
    bool m_initialized;
    
    // Métodos privados
    CryptoKey GenerateKeyInternal(CryptoAlgorithm algorithm, uint64_t expirationSeconds);
    std::vector<uint8_t> EncryptInternal(const uint8_t* data, size_t size, const CryptoKey& key);
    std::vector<uint8_t> DecryptInternal(const uint8_t* data, size_t size, const CryptoKey& key);
    std::string GenerateKeyId() const;
    std::vector<uint8_t> GenerateIV(CryptoAlgorithm algorithm) const;
    size_t GetKeySize(CryptoAlgorithm algorithm) const;
    size_t GetIVSize(CryptoAlgorithm algorithm) const;
};

} // namespace Security
} // namespace WYDBR

#endif // WYDBR_CRYPTO_MANAGER_H
