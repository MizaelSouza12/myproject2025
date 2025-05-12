/**
 * @file Memory.h
 * @brief Sistema de gerenciamento de memória
 * @author WYDBR Team
 */

#ifndef WYDBR_MEMORY_H
#define WYDBR_MEMORY_H

#include <cstddef>
#include <cstdint>
#include <new>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <string>
#include <vector>
#include <atomic>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <type_traits>
#include <functional>

#include "Logger.h"

namespace wydbr {
namespace core {

/**
 * @class MemoryTracker
 * @brief Sistema para rastrear alocações de memória
 */
class MemoryTracker {
public:
    /**
     * @brief Estrutura para armazenar informações de alocação
     */
    struct AllocationInfo {
        size_t size;                  ///< Tamanho da alocação
        const char* file;             ///< Arquivo onde ocorreu a alocação
        int line;                     ///< Linha onde ocorreu a alocação
        const char* function;         ///< Função onde ocorreu a alocação
        const char* tag;              ///< Tag opcional para categorização
    };

    /**
     * @brief Estrutura para estatísticas agregadas de memória
     */
    struct MemoryStats {
        size_t totalAllocated;        ///< Total de memória alocada
        size_t totalDeallocated;      ///< Total de memória liberada
        size_t peakMemory;            ///< Pico de uso de memória
        size_t currentMemory;         ///< Uso atual de memória
        size_t allocationCount;       ///< Número de alocações
        size_t deallocationCount;     ///< Número de liberações
        std::unordered_map<std::string, size_t> taggedMemory; ///< Memória por tag
    };

    /**
     * @brief Obtém a instância singleton do rastreador de memória
     * @return Referência para a instância
     */
    static MemoryTracker& getInstance() {
        static MemoryTracker instance;
        return instance;
    }

    /**
     * @brief Registra uma nova alocação de memória
     * @param ptr Ponteiro para a memória alocada
     * @param size Tamanho da alocação
     * @param file Arquivo onde ocorreu a alocação
     * @param line Linha onde ocorreu a alocação
     * @param function Função onde ocorreu a alocação
     * @param tag Tag opcional para categorização
     */
    void trackAllocation(void* ptr, size_t size, const char* file, int line, 
                        const char* function, const char* tag = nullptr) {
        if (!m_enabled || ptr == nullptr) return;

        std::lock_guard<std::mutex> lock(m_mutex);
        
        AllocationInfo info {
            size,
            file ? file : "unknown",
            line,
            function ? function : "unknown",
            tag ? tag : "default"
        };
        
        m_allocations[ptr] = info;
        m_totalAllocated += size;
        m_currentMemory += size;
        m_allocationCount++;
        
        if (m_currentMemory > m_peakMemory) {
            m_peakMemory = m_currentMemory;
        }
        
        // Registra por tag
        m_taggedMemory[info.tag] += size;
        
        // Log detalhado se configurado
        if (m_verboseLogging) {
            std::stringstream ss;
            ss << "ALLOC: " << ptr << ", size: " << size << " bytes, at: " 
               << file << ":" << line << " (" << function << ")";
            LOG_DEBUG(ss.str());
        }
    }

    /**
     * @brief Registra a liberação de memória
     * @param ptr Ponteiro para a memória liberada
     */
    void trackDeallocation(void* ptr) {
        if (!m_enabled || ptr == nullptr) return;

        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_allocations.find(ptr);
        if (it != m_allocations.end()) {
            const AllocationInfo& info = it->second;
            
            m_totalDeallocated += info.size;
            m_currentMemory -= info.size;
            m_deallocationCount++;
            
            // Reduz o contador por tag
            auto tagIt = m_taggedMemory.find(info.tag);
            if (tagIt != m_taggedMemory.end()) {
                tagIt->second -= info.size;
                if (tagIt->second == 0) {
                    m_taggedMemory.erase(tagIt);
                }
            }
            
            // Log detalhado se configurado
            if (m_verboseLogging) {
                std::stringstream ss;
                ss << "FREE: " << ptr << ", size: " << info.size << " bytes";
                LOG_DEBUG(ss.str());
            }
            
            m_allocations.erase(it);
        } else if (m_verboseLogging) {
            LOG_WARNING("Tentativa de liberar ponteiro não rastreado: " + std::to_string((uintptr_t)ptr));
        }
    }

    /**
     * @brief Habilita ou desabilita o rastreamento de memória
     * @param enabled Estado de habilitação
     */
    void setEnabled(bool enabled) {
        m_enabled = enabled;
    }

    /**
     * @brief Verifica se o rastreamento está habilitado
     * @return true se habilitado, false caso contrário
     */
    bool isEnabled() const {
        return m_enabled;
    }

    /**
     * @brief Habilita ou desabilita logging verboso
     * @param verbose Estado de verbosidade
     */
    void setVerboseLogging(bool verbose) {
        m_verboseLogging = verbose;
    }

    /**
     * @brief Verifica se o logging verboso está habilitado
     * @return true se habilitado, false caso contrário
     */
    bool isVerboseLogging() const {
        return m_verboseLogging;
    }
    
    /**
     * @brief Reseta todas as estatísticas
     */
    void reset() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_allocations.clear();
        m_taggedMemory.clear();
        m_totalAllocated = 0;
        m_totalDeallocated = 0;
        m_peakMemory = 0;
        m_currentMemory = 0;
        m_allocationCount = 0;
        m_deallocationCount = 0;
    }

    /**
     * @brief Obtém estatísticas de uso de memória
     * @return Estrutura com estatísticas
     */
    MemoryStats getStats() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        MemoryStats stats;
        stats.totalAllocated = m_totalAllocated;
        stats.totalDeallocated = m_totalDeallocated;
        stats.peakMemory = m_peakMemory;
        stats.currentMemory = m_currentMemory;
        stats.allocationCount = m_allocationCount;
        stats.deallocationCount = m_deallocationCount;
        stats.taggedMemory = m_taggedMemory;
        
        return stats;
    }

    /**
     * @brief Verifica se há vazamentos de memória
     * @return true se há vazamentos, false caso contrário
     */
    bool hasLeaks() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return !m_allocations.empty();
    }

    /**
     * @brief Gera um relatório de vazamentos de memória
     * @param detailed Se true, inclui detalhes de cada vazamento
     * @return String com o relatório
     */
    std::string generateLeakReport(bool detailed = false) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_allocations.empty()) {
            return "Nenhum vazamento de memória detectado.";
        }
        
        std::stringstream ss;
        size_t totalLeaked = 0;
        
        ss << "=== Relatório de Vazamentos de Memória ===" << std::endl;
        ss << "Total de " << m_allocations.size() << " vazamentos detectados." << std::endl;
        
        if (detailed) {
            int counter = 1;
            
            for (const auto& alloc : m_allocations) {
                const void* ptr = alloc.first;
                const AllocationInfo& info = alloc.second;
                
                totalLeaked += info.size;
                
                ss << counter++ << ". Ponteiro: " << ptr 
                   << ", Tamanho: " << info.size << " bytes"
                   << ", Local: " << info.file << ":" << info.line
                   << ", Função: " << info.function
                   << ", Tag: " << info.tag
                   << std::endl;
            }
        } else {
            // Agrupa vazamentos por arquivo/função
            std::unordered_map<std::string, std::pair<size_t, size_t>> leaksByLocation;
            
            for (const auto& alloc : m_allocations) {
                const AllocationInfo& info = alloc.second;
                totalLeaked += info.size;
                
                std::string location = std::string(info.file) + ":" + std::to_string(info.line) + " (" + info.function + ")";
                auto& entry = leaksByLocation[location];
                entry.first += info.size;
                entry.second++;
            }
            
            // Ordena por tamanho total (não é eficiente, mas é um relatório)
            std::vector<std::pair<std::string, std::pair<size_t, size_t>>> sortedLeaks(
                leaksByLocation.begin(), leaksByLocation.end());
            
            std::sort(sortedLeaks.begin(), sortedLeaks.end(), 
                [](const auto& a, const auto& b) {
                    return a.second.first > b.second.first;
                });
            
            for (const auto& leak : sortedLeaks) {
                ss << "Local: " << leak.first
                   << ", Vazamentos: " << leak.second.second
                   << ", Tamanho Total: " << formatSize(leak.second.first)
                   << std::endl;
            }
        }
        
        ss << "Total de memória vazada: " << formatSize(totalLeaked) << std::endl;
        ss << "===================================" << std::endl;
        
        return ss.str();
    }

    /**
     * @brief Gera um relatório resumido de uso de memória
     * @return String com o relatório
     */
    std::string generateSummaryReport() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::stringstream ss;
        ss << "=== Resumo de Uso de Memória ===" << std::endl;
        ss << "Memória total alocada: " << formatSize(m_totalAllocated) << std::endl;
        ss << "Memória total liberada: " << formatSize(m_totalDeallocated) << std::endl;
        ss << "Uso atual de memória: " << formatSize(m_currentMemory) << std::endl;
        ss << "Pico de uso de memória: " << formatSize(m_peakMemory) << std::endl;
        ss << "Número de alocações: " << m_allocationCount << std::endl;
        ss << "Número de liberações: " << m_deallocationCount << std::endl;
        
        if (!m_taggedMemory.empty()) {
            ss << "--- Uso de memória por tag ---" << std::endl;
            
            // Ordena por tamanho para melhor visualização
            std::vector<std::pair<std::string, size_t>> sortedTags(
                m_taggedMemory.begin(), m_taggedMemory.end());
            
            std::sort(sortedTags.begin(), sortedTags.end(), 
                [](const auto& a, const auto& b) {
                    return a.second > b.second;
                });
            
            for (const auto& tag : sortedTags) {
                ss << tag.first << ": " << formatSize(tag.second) << std::endl;
            }
        }
        
        ss << "==============================" << std::endl;
        return ss.str();
    }

private:
    MemoryTracker() 
        : m_enabled(true), 
          m_verboseLogging(false), 
          m_totalAllocated(0), 
          m_totalDeallocated(0),
          m_peakMemory(0),
          m_currentMemory(0),
          m_allocationCount(0),
          m_deallocationCount(0) {}
    
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;

    /**
     * @brief Formata um tamanho em bytes para uma string legível
     * @param size Tamanho em bytes
     * @return String formatada
     */
    static std::string formatSize(size_t size) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        double adjustedSize = static_cast<double>(size);
        
        while (adjustedSize >= 1024.0 && unitIndex < 4) {
            adjustedSize /= 1024.0;
            unitIndex++;
        }
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << adjustedSize << " " << units[unitIndex];
        return ss.str();
    }

private:
    std::atomic<bool> m_enabled;
    std::atomic<bool> m_verboseLogging;
    
    mutable std::mutex m_mutex;
    std::unordered_map<void*, AllocationInfo> m_allocations;
    std::unordered_map<std::string, size_t> m_taggedMemory;
    
    size_t m_totalAllocated;
    size_t m_totalDeallocated;
    size_t m_peakMemory;
    size_t m_currentMemory;
    size_t m_allocationCount;
    size_t m_deallocationCount;
};

/**
 * @brief Alocador de memória rastreado
 * @tparam T Tipo dos objetos alocados
 */
template <typename T>
class TrackedAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    template <typename U>
    struct rebind {
        using other = TrackedAllocator<U>;
    };

    TrackedAllocator() noexcept : m_tag("default") {}
    TrackedAllocator(const char* tag) noexcept : m_tag(tag ? tag : "default") {}
    template <typename U> TrackedAllocator(const TrackedAllocator<U>& other) noexcept : m_tag(other.getTag()) {}
    
    T* allocate(size_t n) {
        size_t size = n * sizeof(T);
        T* ptr = static_cast<T*>(std::malloc(size));
        
        if (!ptr) {
            throw std::bad_alloc();
        }
        
        MemoryTracker::getInstance().trackAllocation(
            ptr, size, "allocator", 0, "TrackedAllocator::allocate", m_tag);
            
        return ptr;
    }
    
    void deallocate(T* ptr, size_t) noexcept {
        if (ptr) {
            MemoryTracker::getInstance().trackDeallocation(ptr);
            std::free(ptr);
        }
    }
    
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new((void*)p) U(std::forward<Args>(args)...);
    }
    
    template <typename U>
    void destroy(U* p) {
        p->~U();
    }

    const char* getTag() const noexcept {
        return m_tag;
    }

private:
    const char* m_tag;
};

// Comparações de igualdade para os alocadores
template <typename T1, typename T2>
bool operator==(const TrackedAllocator<T1>&, const TrackedAllocator<T2>&) noexcept {
    return true;
}

template <typename T1, typename T2>
bool operator!=(const TrackedAllocator<T1>& a, const TrackedAllocator<T2>& b) noexcept {
    return !(a == b);
}

/**
 * @brief Vetor com alocação rastreada
 * @tparam T Tipo dos elementos
 */
template <typename T>
using TrackedVector = std::vector<T, TrackedAllocator<T>>;

/**
 * @brief String com alocação rastreada
 */
using TrackedString = std::basic_string<char, std::char_traits<char>, TrackedAllocator<char>>;

/**
 * @brief Pool de objetos com alocação fixa para tipos específicos
 * @tparam T Tipo dos objetos na pool
 * @tparam BlockSize Tamanho de cada bloco de alocação
 */
template <typename T, size_t BlockSize = 4096>
class ObjectPool {
public:
    /**
     * @brief Construtor do pool de objetos
     * @param tag Tag para rastreamento de memória
     */
    explicit ObjectPool(const char* tag = "ObjectPool") : m_tag(tag ? tag : "ObjectPool") {
        static_assert(BlockSize >= sizeof(T), "BlockSize deve ser maior ou igual ao tamanho de T");
        static_assert(BlockSize >= sizeof(Node), "BlockSize deve ser maior ou igual ao tamanho de Node");
    }
    
    /**
     * @brief Destrutor do pool de objetos
     */
    ~ObjectPool() {
        for (auto block : m_blocks) {
            MemoryTracker::getInstance().trackDeallocation(block);
            std::free(block);
        }
    }

    /**
     * @brief Aloca e constrói um novo objeto
     * @tparam Args Tipos dos argumentos do construtor
     * @param args Argumentos para o construtor
     * @return Ponteiro para o objeto criado
     */
    template<typename... Args>
    T* allocate(Args&&... args) {
        void* memory = allocateRaw();
        return new(memory) T(std::forward<Args>(args)...);
    }

    /**
     * @brief Destrói e libera um objeto
     * @param object Objeto a ser liberado
     */
    void deallocate(T* object) {
        if (object) {
            object->~T();
            deallocateRaw(object);
        }
    }

    /**
     * @brief Aloca memória sem construir objeto
     * @return Ponteiro para memória alocada
     */
    void* allocateRaw() {
        if (!m_freeList) {
            // Aloca um novo bloco
            allocateBlock();
        }
        
        Node* node = m_freeList;
        m_freeList = m_freeList->next;
        
        return node;
    }

    /**
     * @brief Libera memória previamente alocada sem destruir objeto
     * @param ptr Ponteiro para memória a ser liberada
     */
    void deallocateRaw(void* ptr) {
        if (ptr) {
            Node* node = static_cast<Node*>(ptr);
            node->next = m_freeList;
            m_freeList = node;
        }
    }

    /**
     * @brief Obtém estatísticas de uso do pool
     * @return Par com (número de blocos, número de objetos por bloco)
     */
    std::pair<size_t, size_t> getStats() const {
        return std::make_pair(m_blocks.size(), m_objectsPerBlock);
    }

private:
    // Estrutura de nó para lista encadeada de blocos livres
    struct Node {
        Node* next;
    };

    /**
     * @brief Aloca um novo bloco de memória
     */
    void allocateBlock() {
        // Calcula quantos objetos cabem em um bloco
        m_objectsPerBlock = BlockSize / std::max(sizeof(T), sizeof(Node));
        if (m_objectsPerBlock == 0) m_objectsPerBlock = 1;
        
        // Aloca o bloco
        size_t blockSize = m_objectsPerBlock * std::max(sizeof(T), sizeof(Node));
        char* block = static_cast<char*>(std::malloc(blockSize));
        
        if (!block) {
            throw std::bad_alloc();
        }
        
        MemoryTracker::getInstance().trackAllocation(
            block, blockSize, "ObjectPool.cpp", __LINE__, "ObjectPool::allocateBlock", m_tag);
        
        m_blocks.push_back(block);
        
        // Inicializa a lista encadeada de espaços livres
        Node* currentNode = reinterpret_cast<Node*>(block);
        for (size_t i = 0; i < m_objectsPerBlock - 1; ++i) {
            currentNode->next = reinterpret_cast<Node*>(block + (i + 1) * std::max(sizeof(T), sizeof(Node)));
            currentNode = currentNode->next;
        }
        
        currentNode->next = nullptr;
        m_freeList = reinterpret_cast<Node*>(block);
    }

private:
    const char* m_tag;
    Node* m_freeList = nullptr;
    std::vector<char*> m_blocks;
    size_t m_objectsPerBlock = 0;
};

} // namespace core
} // namespace wydbr

// Macros para facilitar o uso
#define WYDBR_TRACK_ALLOC(ptr, size, tag) \
    wydbr::core::MemoryTracker::getInstance().trackAllocation(ptr, size, __FILE__, __LINE__, __func__, tag)

#define WYDBR_TRACK_FREE(ptr) \
    wydbr::core::MemoryTracker::getInstance().trackDeallocation(ptr)

#endif // WYDBR_MEMORY_H 