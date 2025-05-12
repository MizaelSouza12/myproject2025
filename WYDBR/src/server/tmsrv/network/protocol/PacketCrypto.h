/**
 * PacketCrypto.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/network/protocol/PacketCrypto.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PACKETCRYPTO_H
#define PACKETCRYPTO_H

#include <vector>
#include <array>
#include <cstring>
#include <memory>
#include <random>

#include "PacketTypes.h"
#include "../../../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file PacketCrypto.h
 * @brief Sistema de criptografia do protocolo do WYD
 * 
 * Este arquivo contém a implementação da criptografia usada no protocolo
 * do WYD original, com melhorias para corrigir vulnerabilidades e bugs.
 * No WYD original, a criptografia era muito simples e vulnerável.
 */

namespace wyd {
namespace server {
namespace network {

// Tamanhos de chave
#define CRYPTO_KEY_SIZE        16  // Tamanho da chave em bytes
#define CRYPTO_IV_SIZE         16  // Tamanho do IV em bytes
#define CRYPTO_KEY_COUNT       16  // Número de chaves na tabela

// Tipos de criptografia
enum class CryptoType : BYTE {
    NONE = 0,           // Sem criptografia
    BASIC = 1,          // Criptografia básica (original do WYD)
    ADVANCED = 2,       // Criptografia avançada (melhoria)
    SYMMETRIC = 3,      // Criptografia simétrica (AES, etc.)
    HYBRID = 4,         // Criptografia híbrida (melhoria)
};

/**
 * @brief Estrutura de chave de criptografia
 * 
 * Esta estrutura armazena uma chave de criptografia e seu IV.
 * No WYD original, apenas uma tabela simples era usada.
 */
struct CryptoKey {
    std::array<BYTE, CRYPTO_KEY_SIZE> Key;   // Chave
    std::array<BYTE, CRYPTO_IV_SIZE> IV;     // Vetor de inicialização
    DWORD dwTimestamp;                       // Timestamp de criação
    BYTE byType;                             // Tipo de criptografia
    
    CryptoKey()
        : dwTimestamp(0)
        , byType(0)
    {
        Key.fill(0);
        IV.fill(0);
    }
};

/**
 * @brief Gerenciador de criptografia de pacotes
 * 
 * Esta classe gerencia a criptografia e descriptografia de pacotes.
 * No WYD original, isso era feito por funções simples que eram vulneráveis.
 */
/**
 * Classe WYDPacketCrypto
 * 
 * Esta classe implementa a funcionalidade WYDPacketCrypto conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CPacketCrypto {
public:
    /**
     * @brief Construtor
     * @param useHybrid Se deve usar criptografia híbrida
     */
    CPacketCrypto(bool useHybrid = true);
    
    /**
     * @brief Destrutor
     */
    ~CPacketCrypto();
    
    /**
     * @brief Inicializa o sistema de criptografia
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Encripta um pacote
     * @param header Cabeçalho do pacote
     * @param data Dados do pacote
     * @param dataSize Tamanho dos dados
     * @return true se encriptado com sucesso
     */
    bool EncryptPacket(STRUCT_PACKET_HEADER* header, BYTE* data, int dataSize);
    
    /**
     * @brief Descriptografa um pacote
     * @param header Cabeçalho do pacote
     * @param data Dados do pacote
     * @param dataSize Tamanho dos dados
     * @return true se descriptografado com sucesso
     */
    bool DecryptPacket(STRUCT_PACKET_HEADER* header, BYTE* data, int dataSize);
    
    /**
     * @brief Gera uma nova chave
     * @param keyID ID da chave (se especificado, sobrescreve; caso contrário, usa o próximo disponível)
     * @param type Tipo de criptografia
     * @return ID da chave gerada, ou -1 em caso de erro
     */
    int GenerateKey(int keyID = -1, CryptoType type = CryptoType::ADVANCED);
    
    /**
     * @brief Define uma chave específica
     * @param keyID ID da chave
     * @param key Chave
     * @param iv IV
     * @param type Tipo de criptografia
     * @return true se definida com sucesso
     */
    bool SetKey(int keyID, const BYTE* key, const BYTE* iv, CryptoType type);
    
    /**
     * @brief Obtém uma chave
     * @param keyID ID da chave
     * @return Ponteiro para a chave, ou nullptr se não existir
     */
    const CryptoKey* GetKey(int keyID) const;
    
    /**
     * @brief Obtém a chave mais recente
     * @return ID da chave mais recente
     */
    int GetMostRecentKeyID() const;
    
    /**
     * @brief Obtém o tipo de criptografia atual
     * @return Tipo de criptografia
     */
    CryptoType GetCryptoType() const;
    
    /**
     * @brief Define o tipo de criptografia
     * @param type Tipo de criptografia
     */
    void SetCryptoType(CryptoType type);
    
    /**
     * @brief Calcula o checksum de um pacote
     * @param data Dados do pacote
     * @param dataSize Tamanho dos dados
     * @return Checksum
     */
    DWORD CalculateChecksum(const BYTE* data, int dataSize) const;
    
    /**
     * @brief Gera um hash para uma senha
     * @param password Senha
     * @param salt Salt (opcional)
     * @return Hash da senha
     */
    std::string HashPassword(const std::string& password, const std::string& salt = "");
    
    /**
     * @brief Verifica uma senha contra um hash
     * @param password Senha
     * @param hash Hash
     * @return true se a senha estiver correta
     */
    bool VerifyPassword(const std::string& password, const std::string& hash);
    
private:
    /**
     * @brief Implementação da criptografia básica (original do WYD)
     * @param data Dados a encriptar
     * @param dataSize Tamanho dos dados
     * @param key Chave
     */
    void BasicEncrypt(BYTE* data, int dataSize, const CryptoKey& key);
    
    /**
     * @brief Implementação da descriptografia básica (original do WYD)
     * @param data Dados a descriptografar
     * @param dataSize Tamanho dos dados
     * @param key Chave
     */
    void BasicDecrypt(BYTE* data, int dataSize, const CryptoKey& key);
    
    /**
     * @brief Implementação da criptografia avançada (melhoria)
     * @param data Dados a encriptar
     * @param dataSize Tamanho dos dados
     * @param key Chave
     */
    void AdvancedEncrypt(BYTE* data, int dataSize, const CryptoKey& key);
    
    /**
     * @brief Implementação da descriptografia avançada (melhoria)
     * @param data Dados a descriptografar
     * @param dataSize Tamanho dos dados
     * @param key Chave
     */
    void AdvancedDecrypt(BYTE* data, int dataSize, const CryptoKey& key);
    
    /**
     * @brief Implementação da criptografia simétrica (AES, etc.)
     * @param data Dados a encriptar
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param output Buffer de saída
     * @param outputSize Tamanho do buffer de saída
     * @return Tamanho dos dados encriptados, ou -1 em caso de erro
     */
    int SymmetricEncrypt(const BYTE* data, int dataSize, const CryptoKey& key, 
                         BYTE* output, int outputSize);
    
    /**
     * @brief Implementação da descriptografia simétrica (AES, etc.)
     * @param data Dados a descriptografar
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param output Buffer de saída
     * @param outputSize Tamanho do buffer de saída
     * @return Tamanho dos dados descriptografados, ou -1 em caso de erro
     */
    int SymmetricDecrypt(const BYTE* data, int dataSize, const CryptoKey& key, 
                         BYTE* output, int outputSize);
    
    /**
     * @brief Gera um IV aleatório
     * @param iv Buffer para o IV
     * @param size Tamanho do buffer
     */
    void GenerateIV(BYTE* iv, int size);
    
    /**
     * @brief Gera uma chave aleatória
     * @param key Buffer para a chave
     * @param size Tamanho do buffer
     */
    void GenerateRandomKey(BYTE* key, int size);
    
    /**
     * @brief Mistura duas chaves
     * @param key1 Primeira chave
     * @param key2 Segunda chave
     * @param output Buffer de saída
     * @param size Tamanho do buffer
     */
    void MixKeys(const BYTE* key1, const BYTE* key2, BYTE* output, int size);
    
    /**
     * @brief Deriva uma chave a partir de uma senha
     * @param password Senha
     * @param salt Salt
     * @param iterations Número de iterações
     * @param key Buffer para a chave
     * @param keySize Tamanho da chave
     */
    void DeriveKeyFromPassword(const std::string& password, const std::string& salt, 
                              int iterations, BYTE* key, int keySize);
    
    // Tabela de chaves
    std::array<CryptoKey, CRYPTO_KEY_COUNT> keys_;
    
    // Tipo de criptografia
    CryptoType cryptoType_;
    
    // Uso de híbrido (combinação de vários métodos)
    bool useHybrid_;
    
    // Gerador de números aleatórios
    std::mt19937 rng_;
    
    // ID da chave mais recente
    int mostRecentKeyID_;
};

} // namespace network
} // namespace server
} // namespace wyd

#endif // PACKETCRYPTO_H

} // namespace wydbr
