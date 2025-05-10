/**
 * @file CryptoManager.h
 * @brief Gerenciador de criptografia para WYDBR 2.0
 * 
 * Define o gerenciador de criptografia segura para comunicação de rede,
 * prevenindo interceptação e manipulação de pacotes.
 */

#ifndef WYDBR_CRYPTO_MANAGER_H
#define WYDBR_CRYPTO_MANAGER_H

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <random>

namespace WYDBR {
namespace Network {

/**
 * @class CryptoManager
 * @brief Gerencia criptografia e funções hash para segurança
 */
class CryptoManager {
public:
    // Método Singleton
    static CryptoManager& GetInstance();
    
    // Destrutor
    ~CryptoManager();
    
    /**
     * @brief Inicializa o sistema de criptografia
     * @return true se a inicialização foi bem-sucedida
     */
    bool Initialize();
    
    /**
     * @brief Criptografa dados usando a chave fornecida
     * @param data Ponteiro para os dados a serem criptografados
     * @param data_size Tamanho dos dados em bytes
     * @param key Chave de criptografia (string hexadecimal)
     */
    void Encrypt(uint8_t* data, size_t data_size, const std::string& key);
    
    /**
     * @brief Descriptografa dados usando a chave fornecida
     * @param data Ponteiro para os dados a serem descriptografados
     * @param data_size Tamanho dos dados em bytes
     * @param key Chave de criptografia (string hexadecimal)
     */
    void Decrypt(uint8_t* data, size_t data_size, const std::string& key);
    
    /**
     * @brief Gera uma chave de sessão aleatória e segura
     * @return Chave como string hexadecimal
     */
    std::string GenerateSessionKey();
    
    /**
     * @brief Calcula o hash SHA-256 de uma string
     * @param input String de entrada
     * @return Hash SHA-256 como string hexadecimal
     */
    std::string ComputeSHA256(const std::string& input);
    
    /**
     * @brief Criptografa uma string completa
     * @param input String a ser criptografada
     * @param key Chave de criptografia (opcional, usa chave mestra se vazia)
     * @return String criptografada em formato hexadecimal
     */
    std::string EncryptString(const std::string& input, const std::string& key = "");
    
    /**
     * @brief Descriptografa uma string completa
     * @param encrypted String criptografada em formato hexadecimal
     * @param key Chave de criptografia (opcional, usa chave mestra se vazia)
     * @return String descriptografada
     */
    std::string DecryptString(const std::string& encrypted, const std::string& key = "");
    
private:
    // Construtor privado (singleton)
    CryptoManager();
    
    // Métodos auxiliares
    std::string GenerateRandomKey(size_t length);
    std::vector<uint8_t> HexStringToBytes(const std::string& hex);
    
    // Estado interno
    bool is_initialized_;
    std::string master_key_;
    std::mt19937 random_engine_;
    
    // Mutex para thread-safety
    std::mutex mutex_;
};

} // namespace Network
} // namespace WYDBR

#endif // WYDBR_CRYPTO_MANAGER_H