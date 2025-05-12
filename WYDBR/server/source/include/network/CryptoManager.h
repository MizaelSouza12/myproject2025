/**
 * CryptoManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/network/CryptoManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _CRYPTO_MANAGER_H_
#define _CRYPTO_MANAGER_H_

/**
 * @file CryptoManager.h
 * @brief Gerenciador de criptografia do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de criptografia de rede do WYDBR,
 * responsável pela criptografia e descriptografia dos pacotes do jogo,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>

#include "../GlobalDef.h"
#include "../Basedef.h"

namespace wydbr {
namespace network {

/**
 * @brief Tipo de criptografia
 */
enum /**
 * Classe WYDCryptoType
 * 
 * Esta classe implementa a funcionalidade WYDCryptoType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CryptoType {
    NONE = 0,                   // Nenhum
    XOR = 1,                    // XOR
    SIMPLE_SUBSTITUTION = 2,    // Substituição simples
    BYTE_SHIFT = 3,             // Deslocamento de bytes
    RIJNDAEL = 4,               // Rijndael
    AES = 5,                    // AES
    BLOWFISH = 6,               // Blowfish
    RSA = 7,                    // RSA
    TWOFISH = 8,                // Twofish
    CUSTOM_1 = 9,               // Personalizado 1
    CUSTOM_2 = 10,              // Personalizado 2
    CUSTOM_3 = 11,              // Personalizado 3
    UNKNOWN = 12                // Desconhecido
};

/**
 * @brief Modo de compressão
 */
enum /**
 * Classe WYDCompressionMode
 * 
 * Esta classe implementa a funcionalidade WYDCompressionMode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CompressionMode {
    NONE = 0,                   // Nenhum
    ZLIB = 1,                   // ZLIB
    LZ4 = 2,                    // LZ4
    DEFLATE = 3,                // DEFLATE
    HUFFMAN = 4,                // Huffman
    LZMA = 5,                   // LZMA
    BZIP2 = 6,                  // BZIP2
    CUSTOM_1 = 7,               // Personalizado 1
    CUSTOM_2 = 8,               // Personalizado 2
    CUSTOM_3 = 9,               // Personalizado 3
    UNKNOWN = 10                // Desconhecido
};

/**
 * @brief Tipo de método de hash
 */
enum /**
 * Classe WYDHashMethod
 * 
 * Esta classe implementa a funcionalidade WYDHashMethod conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class HashMethod {
    NONE = 0,                   // Nenhum
    CRC32 = 1,                  // CRC32
    MD5 = 2,                    // MD5
    SHA1 = 3,                   // SHA1
    SHA256 = 4,                 // SHA256
    SHA512 = 5,                 // SHA512
    MURMUR = 6,                 // MurmurHash
    CUSTOM_1 = 7,               // Personalizado 1
    CUSTOM_2 = 8,               // Personalizado 2
    CUSTOM_3 = 9,               // Personalizado 3
    UNKNOWN = 10                // Desconhecido
};

/**
 * @brief Resultado da operação de criptografia
 */
enum /**
 * Classe WYDCryptoResult
 * 
 * Esta classe implementa a funcionalidade WYDCryptoResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CryptoResult {
    SUCCESS = 0,                // Sucesso
    FAILED = 1,                 // Falha
    INVALID_TYPE = 2,           // Tipo inválido
    INVALID_KEY = 3,            // Chave inválida
    INVALID_IV = 4,             // IV inválido
    INVALID_DATA = 5,           // Dados inválidos
    INVALID_SIZE = 6,           // Tamanho inválido
    BUFFER_TOO_SMALL = 7,       // Buffer muito pequeno
    COMPRESSION_FAILED = 8,     // Falha na compressão
    DECOMPRESSION_FAILED = 9,   // Falha na descompressão
    HASH_MISMATCH = 10,         // Hash não corresponde
    CUSTOM_1 = 11,              // Personalizado 1
    CUSTOM_2 = 12,              // Personalizado 2
    CUSTOM_3 = 13,              // Personalizado 3
    UNKNOWN = 14                // Desconhecido
};

/**
 * @brief Configuração de criptografia
 */
struct CryptoConfig {
    CryptoType DefaultCryptoType;               // Tipo de criptografia padrão
    CompressionMode DefaultCompressionMode;     // Modo de compressão padrão
    HashMethod DefaultHashMethod;               // Método de hash padrão
    uint32_t MaxKeysPerSession;                 // Máximo de chaves por sessão
    uint32_t KeyRotationInterval;               // Intervalo de rotação de chave
    uint32_t KeySize;                           // Tamanho da chave
    uint32_t IVSize;                            // Tamanho do IV
    uint32_t HashSize;                          // Tamanho do hash
    uint32_t CompressionLevel;                  // Nível de compressão
    float CompressionThreshold;                 // Limite de compressão
    bool EnableCRC;                             // Habilitar CRC
    uint32_t MaxPlaintextSize;                  // Tamanho máximo do texto plano
    uint32_t MaxCiphertextSize;                 // Tamanho máximo do texto cifrado
    bool UseDynamicKeys;                        // Usar chaves dinâmicas
    bool UseSessionKeys;                        // Usar chaves de sessão
    bool UseDynamicIV;                          // Usar IV dinâmico
    
    CryptoConfig() : DefaultCryptoType(CryptoType::XOR),
        DefaultCompressionMode(CompressionMode::ZLIB),
        DefaultHashMethod(HashMethod::CRC32),
        MaxKeysPerSession(10), KeyRotationInterval(300),
        KeySize(16), IVSize(16), HashSize(4),
        CompressionLevel(6), CompressionThreshold(0.8f),
        EnableCRC(true), MaxPlaintextSize(8192),
        MaxCiphertextSize(9216), UseDynamicKeys(true),
        UseSessionKeys(true), UseDynamicIV(true) {}
};

/**
 * @brief Sessão criptográfica
 */
struct CryptoSession {
    uint32_t SessionID;                 // ID da sessão
    uint32_t EntityID;                  // ID da entidade
    CryptoType Type;                    // Tipo
    CompressionMode Compression;        // Compressão
    HashMethod Hash;                    // Hash
    std::vector<uint8_t> Key;           // Chave
    std::vector<uint8_t> IV;            // IV
    std::vector<uint8_t> NextKey;       // Próxima chave
    std::vector<uint8_t> NextIV;        // Próximo IV
    uint32_t CreationTime;              // Tempo de criação
    uint32_t LastRotationTime;          // Tempo da última rotação
    uint32_t SequenceNumber;            // Número de sequência
    bool Initialized;                   // Inicializado
    bool UseCompression;                // Usar compressão
    
    CryptoSession() : SessionID(0), EntityID(0), Type(CryptoType::NONE),
        Compression(CompressionMode::NONE), Hash(HashMethod::NONE),
        CreationTime(0), LastRotationTime(0), SequenceNumber(0),
        Initialized(false), UseCompression(false) {}
};

/**
 * @brief Checksum calculado
 */
struct CalculatedChecksum {
    std::vector<uint8_t> Data;          // Dados
    uint32_t Hash;                      // Hash
    HashMethod Method;                  // Método
    
    CalculatedChecksum() : Hash(0), Method(HashMethod::NONE) {}
};

/**
 * @brief Par de chaves RSA
 */
struct RSAKeyPair {
    std::vector<uint8_t> PublicKey;     // Chave pública
    std::vector<uint8_t> PrivateKey;    // Chave privada
    uint32_t KeySize;                   // Tamanho da chave
    
    RSAKeyPair() /**
 * KeySize
 * 
 * Implementa a funcionalidade KeySize conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 : KeySize(0){}
};

/**
 * @brief Par de dados de criptografia
 */
struct CryptoPair {
    std::vector<uint8_t> Plaintext;     // Texto plano
    std::vector<uint8_t> Ciphertext;    // Texto /**
 * CryptoPair
 * 
 * Implementa a funcionalidade CryptoPair conforme especificação original.
 * @return Retorna cifrado
 */
 cifrado
    
    CryptoPair(){}
};

/**
 * @brief Gerenciador de criptografia
 * 
 * Esta classe é responsável por gerenciar a criptografia de rede do servidor.
 */
/**
 * Classe WYDCryptoManager
 * 
 * Esta classe implementa a funcionalidade WYDCryptoManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CryptoManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static CryptoManager& getInstance() {
        static CryptoManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração de criptografia
     * @return true se carregado com sucesso
     */
    bool loadCryptoConfig();
    
    /**
     * @brief Cria uma sessão criptográfica
     * @param entityID ID da entidade
     * @param type Tipo
     * @param compression Compressão
     * @param hash Hash
     * @param useCompression Usar compressão
     * @return ID da sessão ou 0 em caso de erro
     */
    uint32_t createSession(uint32_t entityID, CryptoType type = CryptoType::NONE, CompressionMode compression = CompressionMode::NONE, HashMethod hash = HashMethod::NONE, bool useCompression = true);
    
    /**
     * @brief Remove uma sessão criptográfica
     * @param sessionID ID da sessão
     * @return true se removido com sucesso
     */
    bool removeSession(uint32_t sessionID);
    
    /**
     * @brief Criptografa dados
     * @param sessionID ID da sessão
     * @param plaintext Texto plano
     * @param plaintextSize Tamanho do texto plano
     * @param ciphertext Texto cifrado
     * @param maxCiphertextSize Tamanho máximo do texto cifrado
     * @param actualCiphertextSize Tamanho real do texto cifrado
     * @return Resultado da operação
     */
    CryptoResult encrypt(uint32_t sessionID, const void* plaintext, uint32_t plaintextSize, void* ciphertext, uint32_t maxCiphertextSize, uint32_t& actualCiphertextSize);
    
    /**
     * @brief Descriptografa dados
     * @param sessionID ID da sessão
     * @param ciphertext Texto cifrado
     * @param ciphertextSize Tamanho do texto cifrado
     * @param plaintext Texto plano
     * @param maxPlaintextSize Tamanho máximo do texto plano
     * @param actualPlaintextSize Tamanho real do texto plano
     * @return Resultado da operação
     */
    CryptoResult decrypt(uint32_t sessionID, const void* ciphertext, uint32_t ciphertextSize, void* plaintext, uint32_t maxPlaintextSize, uint32_t& actualPlaintextSize);
    
    /**
     * @brief Criptografa dados
     * @param sessionID ID da sessão
     * @param plaintext Texto plano
     * @return Par de dados criptografados
     */
    CryptoPair encrypt(uint32_t sessionID, const std::vector<uint8_t>& plaintext);
    
    /**
     * @brief Descriptografa dados
     * @param sessionID ID da sessão
     * @param ciphertext Texto cifrado
     * @return Par de dados criptografados
     */
    CryptoPair decrypt(uint32_t sessionID, const std::vector<uint8_t>& ciphertext);
    
    /**
     * @brief Comprime dados
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param compressed Dados comprimidos
     * @param maxCompressedSize Tamanho máximo dos dados comprimidos
     * @param actualCompressedSize Tamanho real dos dados comprimidos
     * @param mode Modo
     * @param level Nível
     * @return Resultado da operação
     */
    CryptoResult compress(const void* data, uint32_t dataSize, void* compressed, uint32_t maxCompressedSize, uint32_t& actualCompressedSize, CompressionMode mode = CompressionMode::NONE, uint32_t level = 0);
    
    /**
     * @brief Descomprime dados
     * @param compressed Dados comprimidos
     * @param compressedSize Tamanho dos dados comprimidos
     * @param data Dados
     * @param maxDataSize Tamanho máximo dos dados
     * @param actualDataSize Tamanho real dos dados
     * @param mode Modo
     * @return Resultado da operação
     */
    CryptoResult decompress(const void* compressed, uint32_t compressedSize, void* data, uint32_t maxDataSize, uint32_t& actualDataSize, CompressionMode mode = CompressionMode::NONE);
    
    /**
     * @brief Comprime dados
     * @param data Dados
     * @param mode Modo
     * @param level Nível
     * @return Dados comprimidos
     */
    std::vector<uint8_t> compress(const std::vector<uint8_t>& data, CompressionMode mode = CompressionMode::NONE, uint32_t level = 0);
    
    /**
     * @brief Descomprime dados
     * @param compressed Dados comprimidos
     * @param mode Modo
     * @return Dados descomprimidos
     */
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed, CompressionMode mode = CompressionMode::NONE);
    
    /**
     * @brief Calcula o hash de dados
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param method Método
     * @return Hash
     */
    uint32_t calculateHash(const void* data, uint32_t dataSize, HashMethod method = HashMethod::NONE);
    
    /**
     * @brief Calcula o hash de dados
     * @param data Dados
     * @param method Método
     * @return Hash
     */
    uint32_t calculateHash(const std::vector<uint8_t>& data, HashMethod method = HashMethod::NONE);
    
    /**
     * @brief Calcula o checksum de dados
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @return Checksum calculado
     */
    CalculatedChecksum calculateChecksum(const void* data, uint32_t dataSize);
    
    /**
     * @brief Calcula o checksum de dados
     * @param data Dados
     * @return Checksum calculado
     */
    CalculatedChecksum calculateChecksum(const std::vector<uint8_t>& data);
    
    /**
     * @brief Verifica o checksum de dados
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param hash Hash
     * @return true se for válido
     */
    bool verifyChecksum(const void* data, uint32_t dataSize, uint32_t hash);
    
    /**
     * @brief Verifica o checksum de dados
     * @param data Dados
     * @param hash Hash
     * @return true se for válido
     */
    bool verifyChecksum(const std::vector<uint8_t>& data, uint32_t hash);
    
    /**
     * @brief Gera uma chave aleatória
     * @param keySize Tamanho da chave
     * @return Chave
     */
    std::vector<uint8_t> generateRandomKey(uint32_t keySize = 0);
    
    /**
     * @brief Gera um IV aleatório
     * @param ivSize Tamanho do IV
     * @return IV
     */
    std::vector<uint8_t> generateRandomIV(uint32_t ivSize = 0);
    
    /**
     * @brief Gera um par de chaves RSA
     * @param keySize Tamanho da chave
     * @return Par de chaves
     */
    RSAKeyPair generateRSAKeyPair(uint32_t keySize = 0);
    
    /**
     * @brief Rotaciona as chaves de uma sessão
     * @param sessionID ID da sessão
     * @return true se rotacionado com sucesso
     */
    bool rotateSessionKeys(uint32_t sessionID);
    
    /**
     * @brief Define a chave de uma sessão
     * @param sessionID ID da sessão
     * @param key Chave
     * @return true se definido com sucesso
     */
    bool setSessionKey(uint32_t sessionID, const std::vector<uint8_t>& key);
    
    /**
     * @brief Define o IV de uma sessão
     * @param sessionID ID da sessão
     * @param iv IV
     * @return true se definido com sucesso
     */
    bool setSessionIV(uint32_t sessionID, const std::vector<uint8_t>& iv);
    
    /**
     * @brief Define a próxima chave de uma sessão
     * @param sessionID ID da sessão
     * @param key Chave
     * @return true se definido com sucesso
     */
    bool setSessionNextKey(uint32_t sessionID, const std::vector<uint8_t>& key);
    
    /**
     * @brief Define o próximo IV de uma sessão
     * @param sessionID ID da sessão
     * @param iv IV
     * @return true se definido com sucesso
     */
    bool setSessionNextIV(uint32_t sessionID, const std::vector<uint8_t>& iv);
    
    /**
     * @brief Obtém a sessão de uma entidade
     * @param entityID ID da entidade
     * @return ID da sessão ou 0 se não encontrado
     */
    uint32_t getEntitySession(uint32_t entityID) const;
    
    /**
     * @brief Obtém uma sessão
     * @param sessionID ID da sessão
     * @return Sessão ou nullptr se não encontrada
     */
    const CryptoSession* getSession(uint32_t sessionID) const;
    
    /**
     * @brief Obtém a configuração de criptografia
     * @return Configuração
     */
    CryptoConfig getCryptoConfig() const;
    
    /**
     * @brief Define a configuração de criptografia
     * @param config Configuração
     */
    void setCryptoConfig(const CryptoConfig& config);
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
private:
    /**
     * @brief Construtor privado
     */
    CryptoManager();
    
    /**
     * @brief Destrutor privado
     */
    ~CryptoManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    CryptoManager(const CryptoManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    CryptoManager& operator=(const CryptoManager&) = delete;
    
    // Configuração
    CryptoConfig _cryptoConfig;
    
    // Sessões
    std::unordered_map<uint32_t, CryptoSession> _sessions;
    std::unordered_map<uint32_t, uint32_t> _entityToSession;
    std::atomic<uint32_t> _nextSessionID;
    
    // Thread safety
    mutable std::mutex _configMutex;
    mutable std::mutex _sessionMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Timers
    uint32_t _keyRotationTimer;
    
    /**
     * @brief Checa sessões para rotação de chaves
     * @param currentTime Tempo atual
     */
    void checkSessionsForKeyRotation(uint32_t currentTime);
    
    /**
     * @brief Inicializa os módulos de criptografia
     * @return true se inicializado com sucesso
     */
    bool initializeCryptoModules();
    
    /**
     * @brief Finaliza os módulos de criptografia
     */
    void shutdownCryptoModules();
    
    /**
     * @brief Criptografa dados com XOR
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param keySize Tamanho da chave
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @return Resultado da operação
     */
    CryptoResult encryptXOR(const void* data, uint32_t dataSize, const void* key, uint32_t keySize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize);
    
    /**
     * @brief Descriptografa dados com XOR
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param keySize Tamanho da chave
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @return Resultado da operação
     */
    CryptoResult decryptXOR(const void* data, uint32_t dataSize, const void* key, uint32_t keySize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize);
    
    /**
     * @brief Criptografa dados com substituição simples
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param keySize Tamanho da chave
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @return Resultado da operação
     */
    CryptoResult encryptSimpleSubstitution(const void* data, uint32_t dataSize, const void* key, uint32_t keySize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize);
    
    /**
     * @brief Descriptografa dados com substituição simples
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param keySize Tamanho da chave
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @return Resultado da operação
     */
    CryptoResult decryptSimpleSubstitution(const void* data, uint32_t dataSize, const void* key, uint32_t keySize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize);
    
    /**
     * @brief Criptografa dados com deslocamento de bytes
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param keySize Tamanho da chave
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @return Resultado da operação
     */
    CryptoResult encryptByteShift(const void* data, uint32_t dataSize, const void* key, uint32_t keySize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize);
    
    /**
     * @brief Descriptografa dados com deslocamento de bytes
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param keySize Tamanho da chave
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @return Resultado da operação
     */
    CryptoResult decryptByteShift(const void* data, uint32_t dataSize, const void* key, uint32_t keySize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize);
    
    /**
     * @brief Criptografa dados com AES
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param keySize Tamanho da chave
     * @param iv IV
     * @param ivSize Tamanho do IV
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @return Resultado da operação
     */
    CryptoResult encryptAES(const void* data, uint32_t dataSize, const void* key, uint32_t keySize, const void* iv, uint32_t ivSize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize);
    
    /**
     * @brief Descriptografa dados com AES
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param key Chave
     * @param keySize Tamanho da chave
     * @param iv IV
     * @param ivSize Tamanho do IV
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @return Resultado da operação
     */
    CryptoResult decryptAES(const void* data, uint32_t dataSize, const void* key, uint32_t keySize, const void* iv, uint32_t ivSize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize);
    
    /**
     * @brief Comprime dados com ZLIB
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @param level Nível
     * @return Resultado da operação
     */
    CryptoResult compressZLIB(const void* data, uint32_t dataSize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize, uint32_t level);
    
    /**
     * @brief Descomprime dados com ZLIB
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param output Saída
     * @param maxOutputSize Tamanho máximo da saída
     * @param actualOutputSize Tamanho real da saída
     * @return Resultado da operação
     */
    CryptoResult decompressZLIB(const void* data, uint32_t dataSize, void* output, uint32_t maxOutputSize, uint32_t& actualOutputSize);
    
    /**
     * @brief Calcula o hash CRC32
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @return Hash
     */
    uint32_t calculateCRC32(const void* data, uint32_t dataSize);
    
    /**
     * @brief Calcula o hash MD5
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @return Hash
     */
    uint32_t calculateMD5(const void* data, uint32_t dataSize);
    
    /**
     * @brief Converte um tipo de criptografia para string
     * @param type Tipo
     * @return String
     */
    std::string cryptoTypeToString(CryptoType type) const;
    
    /**
     * @brief Converte um modo de compressão para string
     * @param mode Modo
     * @return String
     */
    std::string compressionModeToString(CompressionMode mode) const;
    
    /**
     * @brief Converte um método de hash para string
     * @param method Método
     * @return String
     */
    std::string hashMethodToString(HashMethod method) const;
};

} // namespace network
} // namespace wydbr

#endif // _CRYPTO_MANAGER_H_