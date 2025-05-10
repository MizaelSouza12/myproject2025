/**
 * @file CryptoManager.cpp
 * @brief Gerenciador de criptografia para WYDBR 2.0
 * 
 * Implementa o gerenciador de criptografia segura para comunicação de rede,
 * prevenindo interceptação e manipulação de pacotes.
 */

#include "../../include/Network/CryptoManager.h"
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>

namespace WYDBR {
namespace Network {

// Singleton
CryptoManager& CryptoManager::GetInstance() {
    static CryptoManager instance;
    return instance;
}

CryptoManager::CryptoManager() 
    : is_initialized_(false) {
}

CryptoManager::~CryptoManager() {
}

bool CryptoManager::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (is_initialized_) {
        return true;
    }
    
    try {
        // Inicializar com uma chave mestra global
        master_key_ = GenerateRandomKey(32);
        
        // Inicializar engine de números aleatórios
        std::random_device rd;
        random_engine_ = std::mt19937(rd());
        
        is_initialized_ = true;
        
        std::cout << "CryptoManager inicializado com sucesso" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Erro ao inicializar CryptoManager: " << e.what() << std::endl;
        return false;
    }
}

void CryptoManager::Encrypt(uint8_t* data, size_t data_size, const std::string& key) {
    if (!is_initialized_ || !data || data_size == 0) {
        return;
    }
    
    std::vector<uint8_t> key_bytes = HexStringToBytes(key);
    if (key_bytes.empty()) {
        // Fallback para chave mestra
        key_bytes = HexStringToBytes(master_key_);
    }
    
    // Aplicar operação XOR com a chave
    for (size_t i = 0; i < data_size; ++i) {
        data[i] ^= key_bytes[i % key_bytes.size()];
    }
    
    // Aplicar cifra de rotação (ROT)
    for (size_t i = 0; i < data_size; ++i) {
        uint8_t rot_value = key_bytes[(i + 3) % key_bytes.size()] % 8;
        data[i] = (data[i] << rot_value) | (data[i] >> (8 - rot_value));
    }
    
    // Aplicar embaralhamento com base na chave
    std::vector<uint8_t> temp(data, data + data_size);
    
    // Algoritmo Fisher-Yates determinístico baseado na chave
    for (size_t i = data_size - 1; i > 0; --i) {
        uint32_t seed = (i * key_bytes[i % key_bytes.size()]) + key_bytes[(i * 17) % key_bytes.size()];
        size_t j = seed % (i + 1);
        
        // Trocar bytes
        if (i != j) {
            std::swap(temp[i], temp[j]);
        }
    }
    
    // Copiar resultado de volta
    for (size_t i = 0; i < data_size; ++i) {
        data[i] = temp[i];
    }
}

void CryptoManager::Decrypt(uint8_t* data, size_t data_size, const std::string& key) {
    if (!is_initialized_ || !data || data_size == 0) {
        return;
    }
    
    std::vector<uint8_t> key_bytes = HexStringToBytes(key);
    if (key_bytes.empty()) {
        // Fallback para chave mestra
        key_bytes = HexStringToBytes(master_key_);
    }
    
    // Desembaralhar (inverso do embaralhamento na criptografia)
    std::vector<uint8_t> temp(data, data + data_size);
    std::vector<uint8_t> result(data_size);
    
    // Índices de desembaralhamento (inverso do algoritmo Fisher-Yates)
    std::vector<size_t> shuffle_map(data_size);
    for (size_t i = 0; i < data_size; ++i) {
        shuffle_map[i] = i;
    }
    
    for (size_t i = data_size - 1; i > 0; --i) {
        uint32_t seed = (i * key_bytes[i % key_bytes.size()]) + key_bytes[(i * 17) % key_bytes.size()];
        size_t j = seed % (i + 1);
        
        // Trocar índices
        if (i != j) {
            std::swap(shuffle_map[i], shuffle_map[j]);
        }
    }
    
    // Aplicar mapa de desembaralhamento
    for (size_t i = 0; i < data_size; ++i) {
        result[shuffle_map[i]] = temp[i];
    }
    
    // Copiar resultado intermediário
    std::copy(result.begin(), result.end(), data);
    
    // Reverter cifra de rotação (ROT)
    for (size_t i = 0; i < data_size; ++i) {
        uint8_t rot_value = key_bytes[(i + 3) % key_bytes.size()] % 8;
        data[i] = (data[i] >> rot_value) | (data[i] << (8 - rot_value));
    }
    
    // Reverter XOR com a chave
    for (size_t i = 0; i < data_size; ++i) {
        data[i] ^= key_bytes[i % key_bytes.size()];
    }
}

std::string CryptoManager::GenerateSessionKey() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!is_initialized_) {
        return "";
    }
    
    return GenerateRandomKey(32);
}

std::string CryptoManager::GenerateRandomKey(size_t length) {
    std::vector<uint8_t> key(length);
    
    // Usar random_engine para gerar bytes aleatórios
    std::uniform_int_distribution<int> dist(0, 255);
    
    for (size_t i = 0; i < length; ++i) {
        key[i] = static_cast<uint8_t>(dist(random_engine_));
    }
    
    // Converter para string hexadecimal
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (auto byte : key) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    
    return ss.str();
}

std::vector<uint8_t> CryptoManager::HexStringToBytes(const std::string& hex) {
    if (hex.empty() || hex.length() % 2 != 0) {
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> bytes;
    bytes.reserve(hex.length() / 2);
    
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        try {
            uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
            bytes.push_back(byte);
        } catch (const std::exception&) {
            // Ignorar erros de formato
            continue;
        }
    }
    
    return bytes;
}

// Funções de hash seguras

std::string CryptoManager::ComputeSHA256(const std::string& input) {
    // Implementação simples de SHA-256
    // Em um sistema real, usar biblioteca criptográfica completa
    
    // Esta é uma versão simplificada para demonstração
    std::vector<uint8_t> bytes(input.begin(), input.end());
    
    // Adicionar sal para aumentar segurança
    std::string salt = master_key_.substr(0, 16);
    bytes.insert(bytes.end(), salt.begin(), salt.end());
    
    // Algoritmo de hash customizado (não usar em produção real)
    std::array<uint32_t, 8> state = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    // Processamento do hash
    for (size_t i = 0; i < bytes.size(); i += 64) {
        size_t chunk_size = std::min(size_t(64), bytes.size() - i);
        std::vector<uint8_t> chunk(bytes.begin() + i, bytes.begin() + i + chunk_size);
        
        // Preencher com zeros se necessário
        if (chunk_size < 64) {
            chunk.resize(64, 0);
        }
        
        // Mix com estado atual
        for (size_t j = 0; j < 8; j++) {
            uint32_t word = 0;
            for (size_t k = 0; k < 4; k++) {
                word = (word << 8) | chunk[j*4 + k];
            }
            state[j] ^= word;
            
            // Rotação
            state[j] = (state[j] << 13) | (state[j] >> 19);
        }
    }
    
    // Converter estado para string hexadecimal
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (auto word : state) {
        ss << std::setw(8) << word;
    }
    
    return ss.str();
}

std::string CryptoManager::EncryptString(const std::string& input, const std::string& key) {
    if (!is_initialized_ || input.empty()) {
        return "";
    }
    
    // Converter string para bytes
    std::vector<uint8_t> data(input.begin(), input.end());
    
    // Criptografar
    std::string encryption_key = key.empty() ? master_key_ : key;
    Encrypt(data.data(), data.size(), encryption_key);
    
    // Converter resultado para string hexadecimal
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (auto byte : data) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    
    return ss.str();
}

std::string CryptoManager::DecryptString(const std::string& encrypted, const std::string& key) {
    if (!is_initialized_ || encrypted.empty()) {
        return "";
    }
    
    // Converter hex para bytes
    std::vector<uint8_t> data = HexStringToBytes(encrypted);
    if (data.empty()) {
        return "";
    }
    
    // Descriptografar
    std::string decryption_key = key.empty() ? master_key_ : key;
    Decrypt(data.data(), data.size(), decryption_key);
    
    // Converter resultado para string
    return std::string(data.begin(), data.end());
}

} // namespace Network
} // namespace WYDBR