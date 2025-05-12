/**
 * PacketCrypto.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/network/protocol/PacketCrypto.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "PacketCrypto.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/err.h>

namespace wydbr {


/**
 * @file PacketCrypto.cpp
 * @brief Implementação do sistema de criptografia do protocolo do WYD
 * 
 * Este arquivo contém a implementação da criptografia usada no protocolo
 * do WYD original, com melhorias para corrigir vulnerabilidades e bugs.
 */

namespace wyd {

} // namespace wydbr

namespace server {
namespace network {

CPacketCrypto::CPacketCrypto(bool useHybrid)
    : cryptoType_(CryptoType::ADVANCED)
    , useHybrid_(useHybrid)
    , mostRecentKeyID_(0)
{
    // Inicializa o gerador de números aleatórios com uma semente baseada no tempo
    unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    rng_.seed(seed);
}

CPacketCrypto::~CPacketCrypto() {
    // Limpa as chaves por seguranç/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param keys_ Parâmetro keys_
 * @return Retorna a
 */
a
    for(auto& key : keys_){
        key.Key.fill(0);
        key.IV.fill(0);
    }
}

bool CPacketCrypto::Initialize() {
    // Inicializa /**
 * OpenSSL_add_all_algorithms
 * 
 * Implementa a funcionalidade OpenSSL_add_all_algorithms conforme especificação original.
 * @return Retorna OpenSSL
 */
 OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    
    // Gera a chave /**
 * GenerateKey
 * 
 * Implementa a funcionalidade GenerateKey conforme especificação original.
 * @param 0 Parâmetro 0
 * @param cryptoType_ Parâmetro cryptoType_
 * @return Retorna inicial
 */
 inicial
    GenerateKey(0, cryptoType_);
    
    return true;
}

bool CPacketCrypto::EncryptPacket(STRUCT_PACKET_HEADER* header, BYTE* data, int dataSize) {
    if (!header || !data || dataSize <= 0) {
        return false;
    }
    
    // Verifica se o pacote já está encriptado
    if (header->IsEncrypted()) {
        return true; // Já está encriptado
    }
    
    // Obtém a chave mais recente
    int keyID = GetMostRecentKeyID();
    const CryptoKey* key = GetKey(keyID);
    if (!key) {
        return false; // Chave não encontrada
    }
    
    // Define o ID da chave no cabeçalho
    header->byKeyID = static_cast<BYTE>(keyID);
    
    // Encripta de acordo com o tipo de criptografia
    switch (static_cast<CryptoType>(key->byType)) {
        case /**
 * BasicEncrypt
 * 
 * Implementa a funcionalidade BasicEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna CryptoType::BASIC:
 */
 CryptoType::BASIC:
            BasicEncrypt(data, dataSize, *key);
            break;
            
        case /**
 * AdvancedEncrypt
 * 
 * Implementa a funcionalidade AdvancedEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna CryptoType::ADVANCED:
 */
 CryptoType::ADVANCED:
            AdvancedEncrypt(data, dataSize, *key);
            break;
            
        case CryptoType::SYMMETRIC: {
            // Para criptografia simétrica, precisamos de um buffer temporário
            std::vector<BYTE> tempBuffer(dataSize + 32); // Espaço extra para padding
            int outputSize = SymmetricEncrypt(data, dataSize, *key, tempBuffer.data(), tempBuffer.size());
            if (outputSize > 0 && outputSize <= dataSize) {
                // Copia de volta para o buffer original
                memcpy(data, tempBuffer.data(), outputSize);
            } else {
                // Fallback para criptografia avançada em caso de /**
 * AdvancedEncrypt
 * 
 * Implementa a funcionalidade AdvancedEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna erro
 */
 erro
                AdvancedEncrypt(data, dataSize, *key);
            }
            break;
        }
            
        case CryptoType::HYBRID: {
            // Híbrido: aplica camadas de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param useHybrid_ Parâmetro useHybrid_
 * @return Retorna criptografia
 */
 criptografia
            if(useHybrid_){
                // Primeiro aplica a criptografia avanç/**
 * AdvancedEncrypt
 * 
 * Implementa a funcionalidade AdvancedEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna ada
 */
ada
                AdvancedEncrypt(data, dataSize, *key);
                
                // Depois aplica a criptografia básica como camada /**
 * BasicEncrypt
 * 
 * Implementa a funcionalidade BasicEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna adicional
 */
 adicional
                BasicEncrypt(data, dataSize, *key);
            } else {
                // Se o híbrido estiver desativado, usa apenas avanç/**
 * AdvancedEncrypt
 * 
 * Implementa a funcionalidade AdvancedEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna ada
 */
ada
                AdvancedEncrypt(data, dataSize, *key);
            }
            break;
        }
            
        default:
            // Sem criptografia
            break;
    }
    
    // Marca o pacote como encriptado
    header->SetEncrypted(true);
    
    // Se for usar criptografia secundária
    if (useHybrid_ && static_cast<CryptoType>(key->byType) == CryptoType::HYBRID) {
        header->SetSecondaryEncryption(true);
    }
    
    return true;
}

bool CPacketCrypto::DecryptPacket(STRUCT_PACKET_HEADER* header, BYTE* data, int dataSize) {
    if (!header || !data || dataSize <= 0) {
        return false;
    }
    
    // Verifica se o pacote está encriptado
    if (!header->IsEncrypted()) {
        return true; // Não está encriptado
    }
    
    // Obtém a chave
    int keyID = header->byKeyID;
    const CryptoKey* key = GetKey(keyID);
    if (!key) {
        return false; // Chave não encontrada
    }
    
    // Descriptografa de acordo com o tipo de criptografia
    switch (static_cast<CryptoType>(key->byType)) {
        case /**
 * BasicDecrypt
 * 
 * Implementa a funcionalidade BasicDecrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna CryptoType::BASIC:
 */
 CryptoType::BASIC:
            BasicDecrypt(data, dataSize, *key);
            break;
            
        case /**
 * AdvancedDecrypt
 * 
 * Implementa a funcionalidade AdvancedDecrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna CryptoType::ADVANCED:
 */
 CryptoType::ADVANCED:
            AdvancedDecrypt(data, dataSize, *key);
            break;
            
        case CryptoType::SYMMETRIC: {
            // Para criptografia simétrica, precisamos de um buffer temporário
            std::vector<BYTE> tempBuffer(dataSize);
            int outputSize = SymmetricDecrypt(data, dataSize, *key, tempBuffer.data(), tempBuffer.size());
            if (outputSize > 0) {
                // Copia de volta para o buffer original
                memcpy(data, tempBuffer.data(), outputSize);
            } else {
                // Fallback para criptografia avançada em caso de /**
 * AdvancedDecrypt
 * 
 * Implementa a funcionalidade AdvancedDecrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna erro
 */
 erro
                AdvancedDecrypt(data, dataSize, *key);
            }
            break;
        }
            
        case CryptoType::HYBRID: {
            // Híbrido: desfaz as camadas de criptografia na ordem inversa
            if (header->HasSecondaryEncryption()) {
                // Primeiro desfaz a criptografia bá/**
 * BasicDecrypt
 * 
 * Implementa a funcionalidade BasicDecrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna sica
 */
sica
                BasicDecrypt(data, dataSize, *key);
                
                // Depois desfaz a criptografia avanç/**
 * AdvancedDecrypt
 * 
 * Implementa a funcionalidade AdvancedDecrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna ada
 */
ada
                AdvancedDecrypt(data, dataSize, *key);
            } else {
                // Se não tiver criptografia secundária, usa apenas avanç/**
 * AdvancedDecrypt
 * 
 * Implementa a funcionalidade AdvancedDecrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna ada
 */
ada
                AdvancedDecrypt(data, dataSize, *key);
            }
            break;
        }
            
        default:
            // Sem criptografia
            break;
    }
    
    // Marca o pacote como descriptografado
    header->SetEncrypted(false);
    header->SetSecondaryEncryption(false);
    
    return true;
}

int CPacketCrypto::GenerateKey(int keyID, CryptoType type) {
    // Se não foi especificado um keyID, usa o próximo disponí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna vel
 */
vel
    if(keyID < 0){
        // Avança para o próximo ID de chave
        keyID = (mostRecentKeyID_ + 1) % CRYPTO_KEY_COUNT;
    }
    
    // Verifica se o keyID é vá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CRYPTO_KEY_COUNT Parâmetro CRYPTO_KEY_COUNT
 * @return Retorna lido
 */
lido
    if(keyID < 0 || keyID >= CRYPTO_KEY_COUNT){
        return -1;
    }
    
    // Gera uma nova chave
    GenerateRandomKey(keys_[keyID].Key.data(), CRYPTO_KEY_SIZE);
    
    // Gera um novo IV
    GenerateIV(keys_[keyID].IV.data(), CRYPTO_IV_SIZE);
    
    // Define o timestamp e tipo
    keys_[keyID].dwTimestamp = static_cast<DWORD>(time(nullptr));
    keys_[keyID].byType = static_cast<BYTE>(type);
    
    // Atualiza o ID da chave mais recente
    mostRecentKeyID_ = keyID;
    
    return keyID;
}

bool CPacketCrypto::SetKey(int keyID, const BYTE* key, const BYTE* iv, CryptoType type) {
    // Verifica se o keyID é vá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !key Parâmetro !key
 * @return Retorna lido
 */
lido
    if(keyID < 0 || keyID >= CRYPTO_KEY_COUNT || !key){
        return false;
    }
    
    // Define a chave
    memcpy(keys_[keyID].Key.data(), key, CRYPTO_KEY_SIZE);
    
    // Define o IV (se fornecido)
    if (iv) {
        memcpy(keys_[keyID].IV.data(), iv, CRYPTO_IV_SIZE);
    } else {
        // Caso contrário, gera um novo IV
        GenerateIV(keys_[keyID].IV.data(), CRYPTO_IV_SIZE);
    }
    
    // Define o timestamp e tipo
    keys_[keyID].dwTimestamp = static_cast<DWORD>(time(nullptr));
    keys_[keyID].byType = static_cast<BYTE>(type);
    
    // Atualiza o ID da chave mais recente
    mostRecentKeyID_ = keyID;
    
    return true;
}

const CryptoKey* CPacketCrypto::GetKey(int keyID) const {
    // Verifica se o keyID é vá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CRYPTO_KEY_COUNT Parâmetro CRYPTO_KEY_COUNT
 * @return Retorna lido
 */
lido
    if(keyID < 0 || keyID >= CRYPTO_KEY_COUNT){
        return nullptr;
    }
    
    return &keys_[keyID];
}

int CPacketCrypto::GetMostRecentKeyID() const {
    return mostRecentKeyID_;
}

CryptoType CPacketCrypto::GetCryptoType() const {
    return cryptoType_;
}

void CPacketCrypto::SetCryptoType(CryptoType type) {
    cryptoType_ = type;
}

DWORD CPacketCrypto::CalculateChecksum(const BYTE* data, int dataSize) const {
    if (!data || dataSize <= 0) {
        return 0;
    }
    
    // Algoritmo de checksum simples (CRC32-like)
    DWORD checksum = 0xFFFFFFFF;
    for (int i = 0; i < dataSize; i++) {
        checksum ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (checksum & 1) {
                checksum = (checksum >> 1) ^ 0xEDB88320;
            } else {
                checksum >>= 1;
            }
        }
    }
    
    return ~checksum;
}

std::string CPacketCrypto::HashPassword(const std::string& password, const std::string& salt) {
    // Usa SHA-256 para hash de senha
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    // Prepara o contexto SHA-256
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    
    // Adiciona o salt (se fornecido)
    std::string saltedPassword = password;
    if (!salt.empty()) {
        saltedPassword = salt + password;
    }
    
    // Atualiza o hash com a senha
    SHA256_Update(&sha256, saltedPassword.c_str(), saltedPassword.length());
    
    // Finaliza o /**
 * SHA256_Final
 * 
 * Implementa a funcionalidade SHA256_Final conforme especificação original.
 * @param hash Parâmetro hash
 * @param sha256 Parâmetro sha256
 * @return Retorna hash
 */
 hash
    SHA256_Final(hash, &sha256);
    
    // Converte para string hexadecimal
    std::string hashString;
    char hexBuffer[3];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        snprintf(hexBuffer, "%02x", hash[i]);
        hashString += hexBuffer;
    }
    
    return hashString;
}

bool CPacketCrypto::VerifyPassword(const std::string& password, const std::string& hash) {
    // Extrai o salt (se presente)
    std::string salt;
    size_t colonPos = hash.find(':');
    if (colonPos != std::string::npos) {
        salt = hash.substr(0, colonPos);
        std::string actualHash = hash.substr(colonPos + 1);
        return (HashPassword(password, salt) == actualHash);
    } else {
        // Sem salt
        return (HashPassword(password) == hash);
    }
}

void CPacketCrypto::BasicEncrypt(BYTE* data, int dataSize, const CryptoKey& key) {
    if (!data || dataSize <= 0) {
        return;
    }
    
    // Esta é uma reimplementação da criptografia básica do WYD
    // No WYD original, era uma implementação muito simples de XOR
    
    // Usa o primeiro byte da chave como seed
    BYTE seed = key.Key[0];
    
    // Encripta os /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna dados
 */
 dados
    for(int i = 0; i < dataSize; i++){
        // No WYD original, isso era um simples XOR com uma sequência previsível
        // Nossa implementação é um pouco mais robusta, mas mantém a compatibilidade
        data[i] ^= key.Key[i % CRYPTO_KEY_SIZE] ^ seed;
        seed = static_cast<BYTE>((seed * 0x343FD + 0x269EC3) & 0xFF);
    }
}

void CPacketCrypto::BasicDecrypt(BYTE* data, int dataSize, const CryptoKey& key) {
    // A descriptografia básica é idêntica à encriptação (operações de XOR são reversíveis)
    BasicEncrypt(data, dataSize, key);
}

void CPacketCrypto::AdvancedEncrypt(BYTE* data, int dataSize, const CryptoKey& key) {
    if (!data || dataSize <= 0) {
        return;
    }
    
    // Versão mais avançada da criptografia do WYD
    // Usa a chave completa e o IV para uma criptografia mais robusta
    
    // Inicializa o contexto
    BYTE state[256];
    BYTE j = 0;
    
    // Inicializa o estado (algoritmo baseado em RC4, mas modificado)
    for (int i = 0; i < 256; i++) {
        state[i] = static_cast<BYTE>(i);
    }
    
    // Mistura o estado com a chave e o /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna IV
 */
 IV
    for(int i = 0; i < 256; i++){
        j = static_cast<BYTE>(j + state[i] + key.Key[i % CRYPTO_KEY_SIZE] + key.IV[i % CRYPTO_IV_SIZE]);
        std::swap(state[i], state[j]);
    }
    
    // Encripta os dados
    BYTE i = 0;
    j = 0;
    for (int k = 0; k < dataSize; k++) {
        i = static_cast<BYTE>(i + 1);
        j = static_cast<BYTE>(j + state[i]);
        std::swap(state[i], state[j]);
        BYTE t = static_cast<BYTE>(state[i] + state[j]);
        data[k] ^= state[t];
    }
}

void CPacketCrypto::AdvancedDecrypt(BYTE* data, int dataSize, const CryptoKey& key) {
    // A descriptografia avançada também é idêntica à encriptaçã/**
 * AdvancedEncrypt
 * 
 * Implementa a funcionalidade AdvancedEncrypt conforme especificação original.
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param key Parâmetro key
 * @return Retorna o
 */
o
    AdvancedEncrypt(data, dataSize, key);
}

int CPacketCrypto::SymmetricEncrypt(const BYTE* data, int dataSize, const CryptoKey& key, 
                                   BYTE* output, int outputSize) {
    if (!data || dataSize <= 0 || !output || outputSize <= 0) {
        return -1;
    }
    
    // Usa AES-256-CBC para criptografia simétrica
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return -1;
    }
    
    // Inicializa a criptografia
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.Key.data(), key.IV.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    
    // Encripta os dados
    int len = 0;
    if (EVP_EncryptUpdate(ctx, output, &len, data, dataSize) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    
    int outputLen = len;
    
    // Finaliza a criptografia
    if (EVP_EncryptFinal_ex(ctx, output + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    
    outputLen += len;
    
    // Libera o /**
 * EVP_CIPHER_CTX_free
 * 
 * Implementa a funcionalidade EVP_CIPHER_CTX_free conforme especificação original.
 * @param ctx Parâmetro ctx
 * @return Retorna contexto
 */
 contexto
    EVP_CIPHER_CTX_free(ctx);
    
    return outputLen;
}

int CPacketCrypto::SymmetricDecrypt(const BYTE* data, int dataSize, const CryptoKey& key, 
                                   BYTE* output, int outputSize) {
    if (!data || dataSize <= 0 || !output || outputSize <= 0) {
        return -1;
    }
    
    // Usa AES-256-CBC para descriptografia simétrica
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return -1;
    }
    
    // Inicializa a descriptografia
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.Key.data(), key.IV.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    
    // Descriptografa os dados
    int len = 0;
    if (EVP_DecryptUpdate(ctx, output, &len, data, dataSize) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    
    int outputLen = len;
    
    // Finaliza a descriptografia
    int ret = EVP_DecryptFinal_ex(ctx, output + len, &len);
    if (ret != 1) {
        // Em caso de erro, ainda podemos usar os dados parciais
        // Isso é útil para pacotes corrompidos ou quando o padding está errado
    }
    
    outputLen += len;
    
    // Libera o /**
 * EVP_CIPHER_CTX_free
 * 
 * Implementa a funcionalidade EVP_CIPHER_CTX_free conforme especificação original.
 * @param ctx Parâmetro ctx
 * @return Retorna contexto
 */
 contexto
    EVP_CIPHER_CTX_free(ctx);
    
    return outputLen;
}

void CPacketCrypto::GenerateIV(BYTE* iv, int size) {
    if (!iv || size <= 0) {
        return;
    }
    
    // Gera um IV aleatório usando /**
 * RAND_bytes
 * 
 * Implementa a funcionalidade RAND_bytes conforme especificação original.
 * @param iv Parâmetro iv
 * @param size Parâmetro size
 * @return Retorna OpenSSL
 */
 OpenSSL
    RAND_bytes(iv, size);
}

void CPacketCrypto::GenerateRandomKey(BYTE* key, int size) {
    if (!key || size <= 0) {
        return;
    }
    
    // Gera uma chave aleatória usando /**
 * RAND_bytes
 * 
 * Implementa a funcionalidade RAND_bytes conforme especificação original.
 * @param key Parâmetro key
 * @param size Parâmetro size
 * @return Retorna OpenSSL
 */
 OpenSSL
    RAND_bytes(key, size);
}

void CPacketCrypto::MixKeys(const BYTE* key1, const BYTE* key2, BYTE* output, int size) {
    if (!key1 || !key2 || !output || size <= 0) {
        return;
    }
    
    // Mistura duas /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna chaves
 */
 chaves
    for(int i = 0; i < size; i++){
        output[i] = static_cast<BYTE>(key1[i] ^ key2[i] ^ (i * 37));
    }
}

void CPacketCrypto::DeriveKeyFromPassword(const std::string& password, const std::string& salt, 
                                         int iterations, BYTE* key, int keySize) {
    if (!key || keySize <= 0) {
        return;
    }
    
    // Deriva uma chave a partir de uma senha usando PBKDF2
    PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), 
                      reinterpret_cast<const unsigned char*>(salt.c_str()), salt.length(), 
                      iterations, EVP_sha256(), keySize, key);
}

} // namespace network
} // namespace server
} // namespace wyd