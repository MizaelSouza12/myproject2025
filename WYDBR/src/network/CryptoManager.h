/**
 * CryptoManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/network/CryptoManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef CRYPTOMANAGER_H
#define CRYPTOMANAGER_H

#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <memory>

#include "../core/WYDTypes.h"

namespace wydbr {


/**
 * @file CryptoManager.h
 * @brief Sistema de criptografia para o protocolo WYD
 * 
 * Este arquivo contém a definição do sistema de criptografia usado
 * para proteger a comunicação entre cliente e servidor no WYD.
 * Segue a estrutura original do WYD, mas com melhorias significativas
 * para corrigir vulnerabilidades e aumentar a segurança.
 */

namespace wyd {
namespace network {

/**
 * @brief Enum com tipos de criptografia suportados (original: não existia)
 * 
 * No WYD original, só havia um tipo de criptografia (muito fraca).
 * Aqui ampliamos as opções para permitir métodos mais seguros.
 */
enum CRYPTO_TYPE : BYTE {
    CRYPTO_NONE = 0,          // Sem criptografia
    CRYPTO_LEGACY = 1,        // Criptografia original do WYD (XOR simples)
    CRYPTO_IMPROVED = 2,      // Versão melhorada (RC4)
    CRYPTO_STRONG = 3,        // Criptografia forte (AES)
    CRYPTO_CUSTOM = 255       // Criptografia customizada
};

/**
 * @brief Classe base para criptografia (original: não existia)
 * 
 * No WYD original, a criptografia era implementada diretamente no código,
 * sem uma interface clara. Aqui criamos uma interface para permitir
 * diferentes implementações.
 */
/**
 * Classe WYDICryptoProvider
 * 
 * Esta classe implementa a funcionalidade WYDICryptoProvider conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ICryptoProvider {
public:
    /**
     * @brief Destrutor virtual
     */
    virtual ~ICryptoProvider() {}
    
    /**
     * @brief Inicializa a criptografia com uma chave
     * @param key Chave de criptografia
     * @param keySize Tamanho da chave
     * @return true se inicializado com sucesso, false caso contrário
     */
    virtual bool Initialize(const BYTE* key, size_t keySize) = 0;
    
    /**
     * @brief Criptografa dados
     * @param input Dados de entrada
     * @param inputSize Tamanho dos dados de entrada
     * @param output Buffer de saída
     * @param outputSize Tamanho do buffer de saída (entrada/saída)
     * @return true se criptografado com sucesso, false caso contrário
     */
    virtual bool Encrypt(const BYTE* input, size_t inputSize, 
                        BYTE* output, size_t& outputSize) = 0;
    
    /**
     * @brief Descriptografa dados
     * @param input Dados de entrada
     * @param inputSize Tamanho dos dados de entrada
     * @param output Buffer de saída
     * @param outputSize Tamanho do buffer de saída (entrada/saída)
     * @return true se descriptografado com sucesso, false caso contrário
     */
    virtual bool Decrypt(const BYTE* input, size_t inputSize, 
                        BYTE* output, size_t& outputSize) = 0;
    
    /**
     * @brief Reseta o estado da criptografia
     */
    virtual void Reset() = 0;
    
    /**
     * @brief Obtém o tipo de criptografia
     * @return Tipo de criptografia
     */
    virtual CRYPTO_TYPE GetType() const = 0;
    
    /**
     * @brief Verifica se a criptografia está inicializada
     * @return true se inicializada, false caso contrário
     */
    virtual bool IsInitialized() const = 0;
};

/**
 * @brief Implementação da criptografia original do WYD (XOR simples)
 * 
 * Esta implementação recria a criptografia original do WYD, que era
 * muito fraca (apenas XOR com uma chave fixa). Mantida por compatibilidade.
 */
/**
 * Classe WYDLegacyCrypto
 * 
 * Esta classe implementa a funcionalidade WYDLegacyCrypto conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CLegacyCrypto : public ICryptoProvider {
public:
    /**
     * @brief Construtor
     */
    CLegacyCrypto();
    
    /**
     * @brief Destrutor
     */
    virtual ~CLegacyCrypto();
    
    /**
     * @brief Inicializa a criptografia com uma chave
     * @param key Chave de criptografia
     * @param keySize Tamanho da chave
     * @return true se inicializado com sucesso, false caso contrário
     */
    virtual bool Initialize(const BYTE* key, size_t keySize) override;
    
    /**
     * @brief Criptografa dados
     * @param input Dados de entrada
     * @param inputSize Tamanho dos dados de entrada
     * @param output Buffer de saída
     * @param outputSize Tamanho do buffer de saída (entrada/saída)
     * @return true se criptografado com sucesso, false caso contrário
     */
    virtual bool Encrypt(const BYTE* input, size_t inputSize, 
                        BYTE* output, size_t& outputSize) override;
    
    /**
     * @brief Descriptografa dados
     * @param input Dados de entrada
     * @param inputSize Tamanho dos dados de entrada
     * @param output Buffer de saída
     * @param outputSize Tamanho do buffer de saída (entrada/saída)
     * @return true se descriptografado com sucesso, false caso contrário
     */
    virtual bool Decrypt(const BYTE* input, size_t inputSize, 
                        BYTE* output, size_t& outputSize) override;
    
    /**
     * @brief Reseta o estado da criptografia
     */
    virtual void Reset() override;
    
    /**
     * @brief Obtém o tipo de criptografia
     * @return Tipo de criptografia
     */
    virtual CRYPTO_TYPE GetType() const override;
    
    /**
     * @brief Verifica se a criptografia está inicializada
     * @return true se inicializada, false caso contrário
     */
    virtual bool IsInitialized() const override;
    
private:
    bool initialized_;          // Se a criptografia está inicializada
    std::vector<BYTE> key_;     // Chave de criptografia
    BYTE counter_;              // Contador para a criptografia
};

/**
 * @brief Implementação de criptografia melhorada (RC4)
 * 
 * Esta implementação usa o algoritmo RC4, que é mais seguro que a
 * criptografia original do WYD, mas ainda é considerado fraco para
 * padrões modernos. Foi escolhido por ser simples e suficiente para
 * esta aplicação, sem sobrecarga significativa.
 */
/**
 * Classe WYDImprovedCrypto
 * 
 * Esta classe implementa a funcionalidade WYDImprovedCrypto conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CImprovedCrypto : public ICryptoProvider {
public:
    /**
     * @brief Construtor
     */
    CImprovedCrypto();
    
    /**
     * @brief Destrutor
     */
    virtual ~CImprovedCrypto();
    
    /**
     * @brief Inicializa a criptografia com uma chave
     * @param key Chave de criptografia
     * @param keySize Tamanho da chave
     * @return true se inicializado com sucesso, false caso contrário
     */
    virtual bool Initialize(const BYTE* key, size_t keySize) override;
    
    /**
     * @brief Criptografa dados
     * @param input Dados de entrada
     * @param inputSize Tamanho dos dados de entrada
     * @param output Buffer de saída
     * @param outputSize Tamanho do buffer de saída (entrada/saída)
     * @return true se criptografado com sucesso, false caso contrário
     */
    virtual bool Encrypt(const BYTE* input, size_t inputSize, 
                        BYTE* output, size_t& outputSize) override;
    
    /**
     * @brief Descriptografa dados
     * @param input Dados de entrada
     * @param inputSize Tamanho dos dados de entrada
     * @param output Buffer de saída
     * @param outputSize Tamanho do buffer de saída (entrada/saída)
     * @return true se descriptografado com sucesso, false caso contrário
     */
    virtual bool Decrypt(const BYTE* input, size_t inputSize, 
                        BYTE* output, size_t& outputSize) override;
    
    /**
     * @brief Reseta o estado da criptografia
     */
    virtual void Reset() override;
    
    /**
     * @brief Obtém o tipo de criptografia
     * @return Tipo de criptografia
     */
    virtual CRYPTO_TYPE GetType() const override;
    
    /**
     * @brief Verifica se a criptografia está inicializada
     * @return true se inicializada, false caso contrário
     */
    virtual bool IsInitialized() const override;
    
private:
    /**
     * @brief Inicializa o estado do algoritmo RC4
     */
    void InitializeRC4();
    
    /**
     * @brief Gera o próximo byte do keystream
     * @return Próximo byte do keystream
     */
    BYTE GetNextKeystreamByte();
    
    bool initialized_;                  // Se a criptografia está inicializada
    std::vector<BYTE> key_;             // Chave de criptografia
    std::array<BYTE, 256> sBox_;        // S-box para o algoritmo RC4
    BYTE i_, j_;                        // Índices para o algoritmo RC4
};

/**
 * @brief Gerenciador de criptografia (original: não existia separadamente)
 * 
 * No WYD original, a criptografia era implementada diretamente no código,
 * sem uma gestão centralizada. Aqui criamos um gerenciador para facilitar
 * o uso e permitir diferentes implementações.
 */
/**
 * Classe WYDCryptoManager
 * 
 * Esta classe implementa a funcionalidade WYDCryptoManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CCryptoManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static CCryptoManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Cria um provedor de criptografia
     * @param type Tipo de criptografia
     * @return Ponteiro para o provedor, ou nullptr se falhar
     */
    std::unique_ptr<ICryptoProvider> CreateProvider(CRYPTO_TYPE type);
    
    /**
     * @brief Gera uma chave aleatória
     * @param output Buffer para a chave
     * @param size Tamanho da chave
     * @return true se gerou com sucesso, false caso contrário
     */
    bool GenerateRandomKey(BYTE* output, size_t size);
    
    /**
     * @brief Gera um hash para uma senha
     * @param password Senha
     * @param output Buffer para o hash
     * @param size Tamanho do buffer
     * @return true se gerou com sucesso, false caso contrário
     */
    bool GeneratePasswordHash(const std::string& password, BYTE* output, size_t size);
    
    /**
     * @brief Verifica um hash de senha
     * @param password Senha
     * @param hash Hash a verificar
     * @param size Tamanho do hash
     * @return true se o hash corresponde à senha, false caso contrário
     */
    bool VerifyPasswordHash(const std::string& password, const BYTE* hash, size_t size);
    
    /**
     * @brief Gera um salt aleatório
     * @param output Buffer para o salt
     * @param size Tamanho do salt
     * @return true se gerou com sucesso, false caso contrário
     */
    bool GenerateRandomSalt(BYTE* output, size_t size);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CCryptoManager();
    
    /**
     * @brief Destrutor
     */
    ~CCryptoManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CCryptoManager(const CCryptoManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CCryptoManager& operator=(const CCryptoManager&) = delete;
    
    bool initialized_;  // Se o gerenciador está inicializado
};

// Definição global para facilitar o acesso
#define g_CryptoManager CCryptoManager::GetInstance()

} // namespace network
} // namespace wyd

#endif // CRYPTOMANAGER_H

} // namespace wydbr
