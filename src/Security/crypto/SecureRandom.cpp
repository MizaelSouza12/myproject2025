/**
 * @file SecureRandom.cpp
 * @brief Implementação do gerador de números aleatórios criptograficamente seguro
 */

#include "SecureRandom.h"
#include <chrono>
#include <cstring>
#include <fstream>
#include <functional>
#include <thread>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#include <intrin.h>
#pragma comment(lib, "bcrypt.lib")
#else
#include <cpuid.h>
#include <x86intrin.h>
#include <sys/random.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

namespace WYDBR {
namespace Security {

// Implementação do singleton
SecureRandom& SecureRandom::GetInstance() {
    static SecureRandom instance;
    return instance;
}

SecureRandom::SecureRandom()
    : m_entropySource(EntropySource::MixedSources),
      m_initialized(false),
      m_generationCounter(0) {
    
    // Inicialização padrão
    std::random_device rd;
    m_generator.seed(rd());
    
    // Preencher o pool de entropia inicial
    for (size_t i = 0; i < m_entropyPool.size(); ++i) {
        m_entropyPool[i] = static_cast<uint8_t>(rd() & 0xFF);
    }
}

SecureRandom::~SecureRandom() {
    // Limpar dados sensíveis
    std::fill(m_entropyPool.begin(), m_entropyPool.end(), 0);
}

bool SecureRandom::Initialize(EntropySource source) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_entropySource = source;
    
    // Inicializar baseado na fonte de entropia
    bool result = false;
    
    switch (source) {
        case EntropySource::HardwareRNG:
            result = SeedFromHardware();
            break;
            
        case EntropySource::SystemRNG:
            result = SeedFromSystem();
            break;
            
        case EntropySource::MixedSources:
            result = SeedFromMixedSources();
            break;
            
        case EntropySource::TimeBased:
            result = SeedFromTimeBased();
            break;
            
        case EntropySource::Custom:
            // Inicialização específica (a ser fornecida pelo cliente)
            result = true;
            break;
    }
    
    m_initialized = result;
    return result;
}

bool SecureRandom::GetRandomBytes(uint8_t* buffer, size_t size) {
    if (!m_initialized) {
        Initialize();
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Verificar necessidade de reseed
    if (m_generationCounter >= RESEED_INTERVAL) {
        ReseedGenerator();
    }
    
    // Gerar bytes aleatórios
    std::uniform_int_distribution<uint32_t> dist(0, 255);
    
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = static_cast<uint8_t>(dist(m_generator));
    }
    
    // Atualizar contador
    m_generationCounter += size;
    
    return true;
}

uint64_t SecureRandom::GetRandomUInt64(uint64_t min, uint64_t max) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::uniform_int_distribution<uint64_t> dist(min, max);
    uint64_t result = dist(m_generator);
    
    // Incrementar contador
    m_generationCounter += sizeof(uint64_t);
    
    // Verificar necessidade de reseed
    if (m_generationCounter >= RESEED_INTERVAL) {
        ReseedGenerator();
    }
    
    return result;
}

uint32_t SecureRandom::GetRandomUInt32(uint32_t min, uint32_t max) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::uniform_int_distribution<uint32_t> dist(min, max);
    uint32_t result = dist(m_generator);
    
    // Incrementar contador
    m_generationCounter += sizeof(uint32_t);
    
    // Verificar necessidade de reseed
    if (m_generationCounter >= RESEED_INTERVAL) {
        ReseedGenerator();
    }
    
    return result;
}

uint16_t SecureRandom::GetRandomUInt16(uint16_t min, uint16_t max) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::uniform_int_distribution<uint16_t> dist(min, max);
    uint16_t result = dist(m_generator);
    
    // Incrementar contador
    m_generationCounter += sizeof(uint16_t);
    
    // Verificar necessidade de reseed
    if (m_generationCounter >= RESEED_INTERVAL) {
        ReseedGenerator();
    }
    
    return result;
}

uint8_t SecureRandom::GetRandomUInt8(uint8_t min, uint8_t max) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::uniform_int_distribution<uint16_t> dist(min, max);
    uint8_t result = static_cast<uint8_t>(dist(m_generator));
    
    // Incrementar contador
    m_generationCounter += sizeof(uint8_t);
    
    // Verificar necessidade de reseed
    if (m_generationCounter >= RESEED_INTERVAL) {
        ReseedGenerator();
    }
    
    return result;
}

double SecureRandom::GetRandomDouble(double min, double max) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::uniform_real_distribution<double> dist(min, max);
    double result = dist(m_generator);
    
    // Incrementar contador
    m_generationCounter += sizeof(double);
    
    // Verificar necessidade de reseed
    if (m_generationCounter >= RESEED_INTERVAL) {
        ReseedGenerator();
    }
    
    return result;
}

std::string SecureRandom::GetRandomString(size_t length, const std::string& charset) {
    std::string result;
    result.reserve(length);
    
    // Gerar bytes aleatórios para índices
    std::vector<uint8_t> indices(length);
    GetRandomBytes(indices.data(), length);
    
    // Converter para caracteres
    for (size_t i = 0; i < length; ++i) {
        size_t index = indices[i] % charset.length();
        result.push_back(charset[index]);
    }
    
    return result;
}

bool SecureRandom::IsHardwareRNGSupported() {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
    // Verificar suporte a RDRAND/RDSEED via CPUID
    uint32_t eax, ebx, ecx, edx;
    
#ifdef _WIN32
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    ecx = cpuInfo[2];
#else
    __cpuid(1, eax, ebx, ecx, edx);
#endif
    
    // Bit 30 de ECX indica suporte a RDRAND
    return (ecx & (1 << 30)) != 0;
#else
    return false;
#endif
}

bool SecureRandom::IsSystemRNGSupported() {
#ifdef _WIN32
    // Windows: verificar BCryptGenRandom
    BCRYPT_ALG_HANDLE hAlgorithm = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_RNG_ALGORITHM, NULL, 0);
    
    if (BCRYPT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
        return true;
    }
    
    return false;
#else
    // Linux/Unix: verificar /dev/urandom
    return access("/dev/urandom", R_OK) == 0;
#endif
}

void SecureRandom::ReseedGenerator() {
    // Atualizar pool de entropia
    UpdateEntropyPool();
    
    // Misturar pool
    MixEntropyPool();
    
    // Criar seed a partir do pool
    std::seed_seq seedSeq(m_entropyPool.begin(), m_entropyPool.end());
    
    // Reseed do gerador
    m_generator.seed(seedSeq);
    
    // Resetar contador
    m_generationCounter = 0;
}

bool SecureRandom::SeedFromHardware() {
    if (!IsHardwareRNGSupported()) {
        return false;
    }
    
    // Usar RDRAND para seed
    uint64_t seed[4] = {0};
    bool success = true;
    
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
    for (int i = 0; i < 4; ++i) {
#ifdef _WIN32
        // Windows x86/x64
        success &= _rdrand64_step(&seed[i]) != 0;
#else
        // GCC/Clang x86/x64
        unsigned char ok;
        asm volatile("rdrand %0; setc %1"
                 : "=r" (seed[i]), "=qm" (ok)
                 :
                 : "cc");
        success &= ok != 0;
#endif
    }
#endif
    
    if (success) {
        // Criar seed_seq
        std::seed_seq seedSeq(std::begin(seed), std::end(seed));
        m_generator.seed(seedSeq);
        
        // Atualizar pool de entropia
        for (size_t i = 0; i < m_entropyPool.size(); i += 8) {
            uint64_t value = 0;
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#ifdef _WIN32
            _rdrand64_step(&value);
#else
            asm volatile("rdrand %0" : "=r" (value) :: "cc");
#endif
#endif
            for (size_t j = 0; j < 8 && i + j < m_entropyPool.size(); ++j) {
                m_entropyPool[i + j] = static_cast<uint8_t>((value >> (j * 8)) & 0xFF);
            }
        }
        
        return true;
    }
    
    return false;
}

bool SecureRandom::SeedFromSystem() {
    bool success = false;
    
#ifdef _WIN32
    // Windows: BCryptGenRandom
    BCRYPT_ALG_HANDLE hAlgorithm = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_RNG_ALGORITHM, NULL, 0);
    
    if (BCRYPT_SUCCESS(status)) {
        // Gerar bytes aleatórios
        uint32_t buffer[8] = {0};
        status = BCryptGenRandom(hAlgorithm, reinterpret_cast<PUCHAR>(buffer), sizeof(buffer), 0);
        
        if (BCRYPT_SUCCESS(status)) {
            // Criar seed_seq
            std::seed_seq seedSeq(std::begin(buffer), std::end(buffer));
            m_generator.seed(seedSeq);
            success = true;
        }
        
        // Preencher pool de entropia
        BCryptGenRandom(hAlgorithm, reinterpret_cast<PUCHAR>(m_entropyPool.data()), 
                         static_cast<ULONG>(m_entropyPool.size()), 0);
        
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
    }
#else
    // Linux/Unix: /dev/urandom
    std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
    
    if (urandom) {
        // Gerar bytes aleatórios
        uint32_t buffer[8] = {0};
        urandom.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
        
        if (urandom) {
            // Criar seed_seq
            std::seed_seq seedSeq(std::begin(buffer), std::end(buffer));
            m_generator.seed(seedSeq);
            success = true;
        }
        
        // Preencher pool de entropia
        urandom.read(reinterpret_cast<char*>(m_entropyPool.data()), m_entropyPool.size());
        
        urandom.close();
    }
#endif
    
    return success;
}

bool SecureRandom::SeedFromMixedSources() {
    // Combinar várias fontes de entropia
    bool hardwareOk = SeedFromHardware();
    bool systemOk = SeedFromSystem();
    
    // Se ambas falharem, usar fonte baseada em tempo
    if (!hardwareOk && !systemOk) {
        return SeedFromTimeBased();
    }
    
    // Adicionar mais entropia com base em tempo
    uint64_t timeValues[4] = {0};
    
    // Timestamp de alta resolução
    timeValues[0] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    
    // Tempo de CPU
    timeValues[1] = std::clock();
    
    // Informações de thread
    timeValues[2] = std::hash<std::thread::id>{}(std::this_thread::get_id());
    
    // Timestamp com segundos
    timeValues[3] = std::chrono::system_clock::now().time_since_epoch().count();
    
    // Combinar com o gerador atual
    for (size_t i = 0; i < 4; ++i) {
        m_generator.discard(timeValues[i] % 100);
        uint64_t v = m_generator();
        timeValues[i] ^= v;
    }
    
    // Aplicar mix final
    std::seed_seq finalSeed(std::begin(timeValues), std::end(timeValues));
    m_generator.seed(finalSeed);
    
    // Atualizar pool de entropia
    for (size_t i = 0; i < 32; ++i) {
        uint64_t v = m_generator();
        for (size_t j = 0; j < 8 && i * 8 + j < m_entropyPool.size(); ++j) {
            m_entropyPool[i * 8 + j] ^= static_cast<uint8_t>((v >> (j * 8)) & 0xFF);
        }
    }
    
    // Misturar o pool para maior segurança
    MixEntropyPool();
    
    return true;
}

bool SecureRandom::SeedFromTimeBased() {
    // Usar combinação de vários valores de tempo como seed
    // Nota: Isso não é criptograficamente seguro, mas é um fallback
    
    // Array para armazenar valores de tempo
    uint64_t timeValues[8];
    
    // Timestamp de alta resolução
    timeValues[0] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    
    // Tempo de CPU
    timeValues[1] = std::clock();
    
    // ID do thread atual
    timeValues[2] = std::hash<std::thread::id>{}(std::this_thread::get_id());
    
    // Timestamp em segundos
    timeValues[3] = std::chrono::system_clock::now().time_since_epoch().count();
    
    // PID do processo
#ifdef _WIN32
    timeValues[4] = GetCurrentProcessId();
#else
    timeValues[4] = getpid();
#endif
    
    // Ponteiro this
    timeValues[5] = reinterpret_cast<uintptr_t>(this);
    
    // Memória alocada
    timeValues[6] = std::hash<void*>{}(static_cast<void*>(new int));
    
    // Delay aleatório para adicionar mais entropia
    std::this_thread::sleep_for(std::chrono::nanoseconds(timeValues[0] % 1000));
    
    // Timestamp após o delay
    timeValues[7] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    
    // Criar seed_seq
    std::seed_seq seedSeq(std::begin(timeValues), std::end(timeValues));
    
    // Seed do gerador
    m_generator.seed(seedSeq);
    
    // Preencher pool de entropia
    for (size_t i = 0; i < m_entropyPool.size(); i += 8) {
        uint64_t value = m_generator();
        for (size_t j = 0; j < 8 && i + j < m_entropyPool.size(); ++j) {
            m_entropyPool[i + j] = static_cast<uint8_t>((value >> (j * 8)) & 0xFF);
        }
    }
    
    return true;
}

void SecureRandom::UpdateEntropyPool() {
    // Coletar mais entropia para o pool
    uint8_t newEntropy[64] = {0};
    
    // Coletar entropia do sistema operacional
    bool systemOk = false;
    
#ifdef _WIN32
    // Windows: BCryptGenRandom
    BCRYPT_ALG_HANDLE hAlgorithm = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_RNG_ALGORITHM, NULL, 0);
    
    if (BCRYPT_SUCCESS(status)) {
        systemOk = BCRYPT_SUCCESS(BCryptGenRandom(hAlgorithm, newEntropy, sizeof(newEntropy), 0));
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
    }
#else
    // Linux/Unix: getrandom ou /dev/urandom
#if defined(__linux__) && defined(SYS_getrandom)
    systemOk = (getrandom(newEntropy, sizeof(newEntropy), 0) == sizeof(newEntropy));
#else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0) {
        systemOk = (read(fd, newEntropy, sizeof(newEntropy)) == sizeof(newEntropy));
        close(fd);
    }
#endif
#endif
    
    // Se o sistema falhar, usar o gerador atual
    if (!systemOk) {
        for (size_t i = 0; i < sizeof(newEntropy); i += 8) {
            uint64_t value = m_generator();
            for (size_t j = 0; j < 8 && i + j < sizeof(newEntropy); ++j) {
                newEntropy[i + j] = static_cast<uint8_t>((value >> (j * 8)) & 0xFF);
            }
        }
    }
    
    // Adicionar entropia de tempo
    uint64_t timeValue = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    for (size_t i = 0; i < 8; ++i) {
        newEntropy[i] ^= static_cast<uint8_t>((timeValue >> (i * 8)) & 0xFF);
    }
    
    // XOR com o pool atual
    for (size_t i = 0; i < sizeof(newEntropy) && i < m_entropyPool.size(); ++i) {
        m_entropyPool[i] ^= newEntropy[i];
    }
}

void SecureRandom::MixEntropyPool() {
    // Misturar o pool de entropia para maior segurança
    // Implementação de rotação de bits e mistura
    
    // Versão simplificada do algoritmo de mistura
    for (int round = 0; round < 3; ++round) {
        for (size_t i = 0; i < m_entropyPool.size(); ++i) {
            // Rotacionar para a esquerda
            uint8_t prev = (i > 0) ? m_entropyPool[i - 1] : m_entropyPool[m_entropyPool.size() - 1];
            m_entropyPool[i] = (m_entropyPool[i] << 1) | (prev >> 7);
            
            // XOR com valor subsequente
            size_t nextIdx = (i + 7) % m_entropyPool.size();
            m_entropyPool[i] ^= m_entropyPool[nextIdx];
        }
    }
}

} // namespace Security
} // namespace WYDBR
