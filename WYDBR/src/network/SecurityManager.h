/**
 * SecurityManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/network/SecurityManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include "../core/WYDTypes.h"
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <random>

namespace wydbr {


/**
 * @file SecurityManager.h
 * @brief Gerenciador de segurança da rede do WYD
 * 
 * Este arquivo implementa o gerenciador de segurança do WYD, responsável pela
 * criptografia, autenticação, e proteção contra hacking e exploits.
 * Inclui implementações melhoradas dos algoritmos originais com correções de segurança.
 */

namespace wyd {
namespace net {

// Tipos de algoritmos de criptografia
enum /**
 * Classe WYDCryptoAlgorithm
 * 
 * Esta classe implementa a funcionalidade WYDCryptoAlgorithm conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CryptoAlgorithm {
    NONE,           // Sem criptografia
    XOR,            // Criptografia simples XOR (original do WYD)
    XOR_ENHANCED,   // Versão melhorada do XOR original
    AES128,         // AES 128-bit (para maior segurança)
    AES256          // AES 256-bit (para maior segurança)
};

// Modos de criptografia
enum /**
 * Classe WYDCryptoMode
 * 
 * Esta classe implementa a funcionalidade WYDCryptoMode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CryptoMode {
    ECB,            // Electronic Codebook (modo básico)
    CBC,            // Cipher Block Chaining (mais seguro)
    CTR             // Counter Mode (rápido e seguro)
};

// Configuração de segurança
struct SecurityConfig {
    CryptoAlgorithm algorithm;     // Algoritmo de criptografia
    CryptoMode mode;               // Modo de criptografia
    bool useChecksums;             // Usar checksums em pacotes
    bool validatePacketSize;       // Validar tamanhos de pacotes
    bool preventReplay;            // Evitar replay attacks
    bool detectSpeedHack;          // Detectar speed hacking
    bool detectWallHack;           // Detectar wall hacking
    bool loggingEnabled;           // Registrar eventos de segurança
    std::string securityLogFile;   // Arquivo de log de segurança
    
    SecurityConfig() 
        : algorithm(CryptoAlgorithm::XOR_ENHANCED)
        , mode(CryptoMode::CBC)
        , useChecksums(true)
        , validatePacketSize(true)
        , preventReplay(true)
        , detectSpeedHack(true)
        , detectWallHack(true)
        , loggingEnabled(true)
        , securityLogFile("logs/security.log")
    {
    }
};

// Contexto de segurança para uma conexão
struct SecurityContext {
    std::vector<BYTE> encryptionKey;   // Chave de criptografia
    std::vector<BYTE> decryptionKey;   // Chave de decriptografia (pode ser igual à de criptografia)
    std::vector<BYTE> iv;              // Vetor de inicialização para CBC/CTR
    DWORD lastIncomingSeqNum;          // Último número de sequência recebido
    DWORD lastOutgoingSeqNum;          // Último número de sequência enviado
    DWORD clientSeed;                  // Seed do cliente para geração de números aleatórios
    DWORD serverSeed;                  // Seed do servidor para geração de números aleatórios
    bool handshakeCompleted;           // Se o handshake de segurança foi concluído
    
    SecurityContext() 
        : lastIncomingSeqNum(0)
        , lastOutgoingSeqNum(0)
        , clientSeed(0)
        , serverSeed(0)
        , handshakeCompleted(false)
    {
    }
};

// Gerenciador de segurança
/**
 * Classe WYDSecurityManager
 * 
 * Esta classe implementa a funcionalidade WYDSecurityManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SecurityManager {
public:
    // Singleton
    static SecurityManager& getInstance();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @param config Parâmetro config
 * @return Retorna bool
 */

    bool initialize(const SecurityConfig& config);
    /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 */

    void shutdown();
    
    // Gerenciamento de contextos de segurança
    /**
 * createContext
 * 
 * Implementa a funcionalidade createContext conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t createContext();
    /**
 * destroyContext
 * 
 * Implementa a funcionalidade destroyContext conforme especificação original.
 * @param contextId Parâmetro contextId
 * @return Retorna bool
 */

    bool destroyContext(uint32_t contextId);
    
    // Geração de chaves
    /**
 * generateKey
 * 
 * Implementa a funcionalidade generateKey conforme especificação original.
 * @param contextId Parâmetro contextId
 * @param 16 Parâmetro 16
 */

    void generateKey(uint32_t contextId, size_t keySize = 16);
    /**
 * setKey
 * 
 * Implementa a funcionalidade setKey conforme especificação original.
 * @param contextId Parâmetro contextId
 * @param key Parâmetro key
 */

    void setKey(uint32_t contextId, const std::vector<BYTE>& key);
    
    // Métodos de criptografia
    /**
 * encrypt
 * 
 * Implementa a funcionalidade encrypt conforme especificação original.
 * @param contextId Parâmetro contextId
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool encrypt(uint32_t contextId, void* data, size_t size);
    /**
 * decrypt
 * 
 * Implementa a funcionalidade decrypt conforme especificação original.
 * @param contextId Parâmetro contextId
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool decrypt(uint32_t contextId, void* data, size_t size);
    
    // Métodos de validação
    /**
 * validatePacket
 * 
 * Implementa a funcionalidade validatePacket conforme especificação original.
 * @param contextId Parâmetro contextId
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool validatePacket(uint32_t contextId, const void* data, size_t size);
    /**
 * validateChecksum
 * 
 * Implementa a funcionalidade validateChecksum conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param expectedChecksum Parâmetro expectedChecksum
 * @return Retorna bool
 */

    bool validateChecksum(const void* data, size_t size, DWORD expectedChecksum);
    /**
 * validateSequence
 * 
 * Implementa a funcionalidade validateSequence conforme especificação original.
 * @param contextId Parâmetro contextId
 * @param sequenceNumber Parâmetro sequenceNumber
 * @return Retorna bool
 */

    bool validateSequence(uint32_t contextId, DWORD sequenceNumber);
    
    // Métodos de detecção de hacking
    /**
 * checkSpeedHack
 * 
 * Implementa a funcionalidade checkSpeedHack conforme especificação original.
 * @param contextId Parâmetro contextId
 * @param clientTick Parâmetro clientTick
 * @param serverTick Parâmetro serverTick
 * @return Retorna bool
 */

    bool checkSpeedHack(uint32_t contextId, DWORD clientTick, DWORD serverTick);
    /**
 * checkWallHack
 * 
 * Implementa a funcionalidade checkWallHack conforme especificação original.
 * @param contextId Parâmetro contextId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param destX Parâmetro destX
 * @param destY Parâmetro destY
 * @return Retorna bool
 */

    bool checkWallHack(uint32_t contextId, WORD x, WORD y, WORD destX, WORD destY);
    /**
 * checkTeleportHack
 * 
 * Implementa a funcionalidade checkTeleportHack conforme especificação original.
 * @param contextId Parâmetro contextId
 * @param oldX Parâmetro oldX
 * @param oldY Parâmetro oldY
 * @param newX Parâmetro newX
 * @param newY Parâmetro newY
 * @param timestamp Parâmetro timestamp
 * @return Retorna bool
 */

    bool checkTeleportHack(uint32_t contextId, WORD oldX, WORD oldY, WORD newX, WORD newY, DWORD timestamp);
    
    // Geração de tokens/seeds
    /**
 * generateToken
 * 
 * Implementa a funcionalidade generateToken conforme especificação original.
 * @return Retorna DWORD
 */

    DWORD generateToken();
    /**
 * generateSeed
 * 
 * Implementa a funcionalidade generateSeed conforme especificação original.
 * @return Retorna DWORD
 */

    DWORD generateSeed();
    
    // Utilitários
    /**
 * calculateChecksum
 * 
 * Implementa a funcionalidade calculateChecksum conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna DWORD
 */

    DWORD calculateChecksum(const void* data, size_t size);
    std::vector<BYTE> generateRandomBytes(size_t count);
    
private:
    // Construtor privado (singleton)
    SecurityManager();
    ~SecurityManager();
    
    // Implementações XOR do WYD (original e melhorada)
    /**
 * wydXorEncrypt
 * 
 * Implementa a funcionalidade wydXorEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param key Parâmetro key
 * @param keySize Parâmetro keySize
 */

    void wydXorEncrypt(void* data, size_t size, const BYTE* key, size_t keySize);
    /**
 * wydXorDecrypt
 * 
 * Implementa a funcionalidade wydXorDecrypt conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param key Parâmetro key
 * @param keySize Parâmetro keySize
 */

    void wydXorDecrypt(void* data, size_t size, const BYTE* key, size_t keySize);
    
    // Implementação melhorada com XOR
    /**
 * wydXorEnhancedEncrypt
 * 
 * Implementa a funcionalidade wydXorEnhancedEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param key Parâmetro key
 * @param keySize Parâmetro keySize
 * @param iv Parâmetro iv
 */

    void wydXorEnhancedEncrypt(void* data, size_t size, const BYTE* key, size_t keySize, const BYTE* iv);
    /**
 * wydXorEnhancedDecrypt
 * 
 * Implementa a funcionalidade wydXorEnhancedDecrypt conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param key Parâmetro key
 * @param keySize Parâmetro keySize
 * @param iv Parâmetro iv
 */

    void wydXorEnhancedDecrypt(void* data, size_t size, const BYTE* key, size_t keySize, const BYTE* iv);
    
    // Implementações AES
    /**
 * aesEncrypt
 * 
 * Implementa a funcionalidade aesEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param key Parâmetro key
 * @param keySize Parâmetro keySize
 * @param iv Parâmetro iv
 * @param mode Parâmetro mode
 */

    void aesEncrypt(void* data, size_t size, const BYTE* key, size_t keySize, const BYTE* iv, CryptoMode mode);
    /**
 * aesDecrypt
 * 
 * Implementa a funcionalidade aesDecrypt conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param key Parâmetro key
 * @param keySize Parâmetro keySize
 * @param iv Parâmetro iv
 * @param mode Parâmetro mode
 */

    void aesDecrypt(void* data, size_t size, const BYTE* key, size_t keySize, const BYTE* iv, CryptoMode mode);
    
    // Configuração
    SecurityConfig config_;
    
    // Mapa de contextos de segurança
    std::unordered_map<uint32_t, SecurityContext> contexts_;
    std::mutex contextsMutex_;
    
    // Geradores de números aleatórios
    std::mt19937 randomEngine_;
    std::uniform_int_distribution<DWORD> randomDistribution_;
    
    // Flag de inicialização
    bool initialized_;
    
    // Próximo ID de contexto
    uint32_t nextContextId_;
};

// Funções utilitárias de segurança

// Calcula hash SHA-256
std::vector<BYTE> calculateSHA256(const void* data, size_t size);

// Calcula hash MD5 (para compatibilidade com sistemas legados)
std::vector<BYTE> calculateMD5(const void* data, size_t size);

// Calcula CRC32 (checksum simples e rápido)
/**
 * calculateCRC32
 * 
 * Implementa a funcionalidade calculateCRC32 conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna DWORD
 */

DWORD calculateCRC32(const void* data, size_t size);

// Codifica uma string em Base64
/**
 * encodeBase64
 * 
 * Implementa a funcionalidade encodeBase64 conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna std::string
 */

std::string encodeBase64(const void* data, size_t size);

// Decodifica uma string Base64
std::vector<BYTE> decodeBase64(const std::string& base64);

// Gera um UUID (para identificação única)
/**
 * generateUUID
 * 
 * Implementa a funcionalidade generateUUID conforme especificação original.
 * @return Retorna std::string
 */

std::string generateUUID();

// Implementação da criptografia original do WYD (para compatibilidade)
/**
 * Classe WYDOriginalCrypto
 * 
 * Esta classe implementa a funcionalidade WYDOriginalCrypto conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDOriginalCrypto {
public:
    /**
 * encryptAccount
 * 
 * Implementa a funcionalidade encryptAccount conforme especificação original.
 * @param account Parâmetro account
 * @param key Parâmetro key
 */

    static void encryptAccount(char* account, const BYTE* key);
    /**
 * decryptAccount
 * 
 * Implementa a funcionalidade decryptAccount conforme especificação original.
 * @param account Parâmetro account
 * @param key Parâmetro key
 */

    static void decryptAccount(char* account, const BYTE* key);
    /**
 * encryptPassword
 * 
 * Implementa a funcionalidade encryptPassword conforme especificação original.
 * @param password Parâmetro password
 * @param key Parâmetro key
 */

    static void encryptPassword(char* password, const BYTE* key);
    /**
 * decryptPassword
 * 
 * Implementa a funcionalidade decryptPassword conforme especificação original.
 * @param password Parâmetro password
 * @param key Parâmetro key
 */

    static void decryptPassword(char* password, const BYTE* key);
    /**
 * encryptPacket
 * 
 * Implementa a funcionalidade encryptPacket conforme especificação original.
 * @param packet Parâmetro packet
 * @param size Parâmetro size
 * @param key Parâmetro key
 */

    static void encryptPacket(void* packet, size_t size, const BYTE* key);
    /**
 * decryptPacket
 * 
 * Implementa a funcionalidade decryptPacket conforme especificação original.
 * @param packet Parâmetro packet
 * @param size Parâmetro size
 * @param key Parâmetro key
 */

    static void decryptPacket(void* packet, size_t size, const BYTE* key);
};

} // namespace net
} // namespace wyd

#endif // SECURITY_MANAGER_H

} // namespace wydbr
