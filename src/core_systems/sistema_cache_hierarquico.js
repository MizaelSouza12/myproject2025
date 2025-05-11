/**
 * WYDBR 2.0 - Implementação Gerada Automaticamente
 * Arquivo origem: sistema_cache_hierarquico.txt
 * 
 * Este arquivo contém a definição C++ do sistema, encapsulada em JavaScript
 * para integração no projeto WYDBR 2.0.
 */

// Exporta o código C++ como uma string para ser processado pelo compilador C++
module.exports.cppCode = `
// CacheManager.h - Sistema de Cache Hierárquico
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <functional>
#include <memory>
#include <optional>
#include <any>
#include <variant>
#include <nlohmann/json.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include "ClusterManager.h"
#include "StorageManager.h"
#include "SecurityProvider.h"

namespace WYDBR {
namespace Cache {

using json = nlohmann::json;
namespace ipc = boost::interprocess;

enum class CacheLevel {
    L1, // Memória local do processo
    L2, // Memória compartilhada entre processos
    L3  // Cache distribuído entre servidores
};

enum class CacheEvictionPolicy {
    LRU,
    LFU,
    FIFO,
    TTL_BASED,
    PRIORITY_BASED,
    CUSTOM
};

enum class CacheItemType {
    PLAYER,
    ENTITY,
    ITEM,
    MAP_CHUNK,
    BINARY_BLOB,
    CUSTOM
};

enum class CacheConsistencyModel {
    STRONG,
    EVENTUAL,
    CAUSAL,
    DELTA
};

struct CacheKey {
    std::string category;
    std::string id;
    
    bool operator==(const CacheKey& other) const {
        return category == other.category && id == other.id;
    }
    
    std::string toString() const {
        return category + ":" + id;
    }
};

struct CacheItemMetadata {
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastAccessed;
    std::chrono::system_clock::time_point expiresAt;
    uint64_t accessCount{0};
    uint32_t priority{0};
    std::string owner;
    uint32_t version{0};
    bool dirty{false};
    size_t size{0};
    CacheItemType type;
    CacheLevel level;
    std::unordered_map<std::string, std::string> tags;
};

struct CacheEntry {
    CacheKey key;
    std::any value;
    CacheItemMetadata metadata;
};

// Hash para CacheKey
struct CacheKeyHash {
    std::size_t operator()(const CacheKey& k) const {
        return std::hash<std::string>()(k.category) ^ std::hash<std::string>()(k.id);
    }
};

// Função para serialização e deserialização de dados
using DataSerializer = std::function<std::vector<uint8_t>(const std::any&)>;
using DataDeserializer = std::function<std::any(const std::vector<uint8_t>&)>;

// Funções para customização
using CacheEvictionFunction = std::function<std::vector<CacheKey>(const std::unordered_map<CacheKey, CacheEntry, CacheKeyHash>&, size_t targetCount)>;
using CacheMissHandler = std::function<std::optional<std::any>(const CacheKey&)>;
using CacheInvalidationListener = std::function<void(const CacheKey&)>;

class CacheManager {
public:
    // Singleton
    static CacheManager& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Interface principal L1 (local)
    template<typename T>
    bool set(const CacheKey& key, const T& value, std::chrono::milliseconds ttl = std::chrono::milliseconds::zero());
    
    template<typename T>
    std::optional<T> get(const CacheKey& key);
    
    template<typename T>
    std::optional<T> getOrCompute(const CacheKey& key, std::function<T()> computeFunc, std::chrono::milliseconds ttl = std::chrono::milliseconds::zero());
    
    bool remove(const CacheKey& key);
    bool contains(const CacheKey& key) const;
    
    // Interface L2 (compartilhada)
    template<typename T>
    bool setShared(const CacheKey& key, const T& value, std::chrono::milliseconds ttl = std::chrono::milliseconds::zero());
    
    template<typename T>
    std::optional<T> getShared(const CacheKey& key);
    
    bool removeShared(const CacheKey& key);
    bool containsShared(const CacheKey& key) const;
    
    // Interface L3 (distribuída)
    template<typename T>
    bool setDistributed(const CacheKey& key, const T& value, std::chrono::milliseconds ttl = std::chrono::milliseconds::zero());
    
    template<typename T>
    std::optional<T> getDistributed(const CacheKey& key);
    
    bool removeDistributed(const CacheKey& key);
    bool containsDistributed(const CacheKey& key) const;
    
    // Manipulação por nível
    void clear(CacheLevel level = CacheLevel::L1);
    void evict(CacheLevel level, size_t targetSize);
    size_t getCurrentSize(CacheLevel level) const;
    
    // Batching
    bool setBatch(const std::vector<std::pair<CacheKey, std::any>>& entries, CacheLevel level = CacheLevel::L1);
    std::vector<std::optional<std::any>> getBatch(const std::vector<CacheKey>& keys, CacheLevel level = CacheLevel::L1);
    bool removeBatch(const std::vector<CacheKey>& keys, CacheLevel level = CacheLevel::L1);
    
    // Configuração dinâmica
    void setEvictionPolicy(CacheLevel level, CacheEvictionPolicy policy);
    void setMaxSize(CacheLevel level, size_t maxSizeBytes);
    void setCacheMissHandler(CacheLevel level, CacheMissHandler handler);
    void setCustomEvictionFunction(CacheLevel level, CacheEvictionFunction function);
    
    // Invalidação
    void invalidate(const CacheKey& key);
    void invalidatePattern(const std::string& categoryPattern, const std::string& idPattern);
    void invalidateByTag(const std::string& tagKey, const std::string& tagValue);
    
    // Listeners
    void registerInvalidationListener(const std::string& category, CacheInvalidationListener listener);
    void unregisterInvalidationListener(const std::string& category);
    
    // Pre-fetching
    void registerPrefetchPattern(const std::string& triggerCategory, const std::string& targetCategory, 
                               std::function<std::vector<std::string>(const std::string&)> idMapper);
    void triggerPrefetch(const CacheKey& accessedKey);
    
    // Estatísticas e monitoramento
    json getCacheStats() const;
    json getItemStats(const CacheKey& key) const;
    std::vector<CacheKey> getKeys(const std::string& category = "", CacheLevel level = CacheLevel::L1) const;
    
private:
    CacheManager();
    ~CacheManager();
    
    // Atributos privados
    static std::unique_ptr<CacheManager> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<Cluster::ClusterManager> m_clusterManager;
    std::shared_ptr<Storage::StorageManager> m_storageManager;
    std::shared_ptr<Security::SecurityProvider> m_securityProvider;
    
    // Cache L1 (memória local)
    struct L1Cache {
        std::unordered_map<CacheKey, CacheEntry, CacheKeyHash> entries;
        mutable std::shared_mutex mutex;
        std::atomic<size_t> currentSizeBytes{0};
        size_t maxSizeBytes{1024 * 1024 * 512}; // 512MB default
        CacheEvictionPolicy evictionPolicy{CacheEvictionPolicy::LRU};
        CacheEvictionFunction customEvictionFunction;
        CacheMissHandler missHandler;
        
        struct LRUNode {
            CacheKey key;
            std::list<CacheKey>::iterator listIterator;
        };
        
        std::list<CacheKey> lruList;
        std::unordered_map<CacheKey, std::list<CacheKey>::iterator, CacheKeyHash> lruMap;
        std::mutex lruMutex;
        
        std::unordered_map<CacheKey, uint32_t, CacheKeyHash> lfuCounters;
        std::mutex lfuMutex;
    };
    
    L1Cache m_l1Cache;
    
    // Cache L2 (compartilhada entre processos)
    struct L2Cache {
        using SharedString = ipc::basic_string<char, std::char_traits<char>, 
            ipc::allocator<char, ipc::managed_shared_memory::segment_manager>>;
        
        using SharedValue = std::vector<uint8_t>;
        
        struct SharedKey {
            SharedString category;
            SharedString id;
            
            bool operator<(const SharedKey& other) const {
                return category < other.category || (category == other.category && id < other.id);
            }
        };
        
        struct SharedMetadata {
            uint64_t createdAt;
            uint64_t lastAccessed;
            uint64_t expiresAt;
            uint64_t accessCount;
            uint32_t priority;
            SharedString owner;
            uint32_t version;
            bool dirty;
            size_t size;
            uint32_t type;
            uint32_t level;
        };
        
        struct SharedEntry {
            SharedMetadata metadata;
            SharedValue value;
        };
        
        using KeyValueMap = ipc::map<SharedKey, SharedEntry, 
            std::less<SharedKey>, 
            ipc::allocator<std::pair<const SharedKey, SharedEntry>, 
                ipc::managed_shared_memory::segment_manager>>;
        
        std::unique_ptr<ipc::managed_shared_memory> segment;
        KeyValueMap* map{nullptr};
        std::mutex sharedMutex;
        std::atomic<size_t> currentSizeBytes{0};
        size_t maxSizeBytes{1024 * 1024 * 1024}; // 1GB default
        CacheEvictionPolicy evictionPolicy{CacheEvictionPolicy::LRU};
        CacheEvictionFunction customEvictionFunction;
        CacheMissHandler missHandler;
        bool enabled{true};
    };
    
    L2Cache m_l2Cache;
    
    // Cache L3 (distribuída)
    struct L3Cache {
        struct DistributedEntry {
            CacheKey key;
            std::vector<uint8_t> serializedValue;
            CacheItemMetadata metadata;
        };
        
        std::atomic<size_t> currentSizeBytes{0};
        size_t maxSizeBytes{1024 * 1024 * 1024 * 10}; // 10GB default
        CacheEvictionPolicy evictionPolicy{CacheEvictionPolicy::LRU};
        CacheEvictionFunction customEvictionFunction;
        CacheMissHandler missHandler;
        CacheConsistencyModel consistencyModel{CacheConsistencyModel::EVENTUAL};
        bool enabled{true};
    };
    
    L3Cache m_l3Cache;
    
    // Serialização/deserialização
    std::unordered_map<CacheItemType, DataSerializer> m_serializers;
    std::unordered_map<CacheItemType, DataDeserializer> m_deserializers;
    
    // Prefetching
    struct PrefetchRule {
        std::string triggerCategory;
        std::string targetCategory;
        std::function<std::vector<std::string>(const std::string&)> idMapper;
    };
    
    std::vector<PrefetchRule> m_prefetchRules;
    mutable std::mutex m_prefetchRulesMutex;
    
    // Invalidação
    std::unordered_map<std::string, std::vector<CacheInvalidationListener>> m_invalidationListeners;
    mutable std::mutex m_listenersMutex;
    
    // Métricas
    struct CacheStats {
        std::atomic<uint64_t> hits{0};
        std::atomic<uint64_t> misses{0};
        std::atomic<uint64_t> sets{0};
        std::atomic<uint64_t> removes{0};
        std::atomic<uint64_t> evictions{0};
        std::atomic<uint64_t> invalidations{0};
        std::atomic<uint64_t> prefetchTriggered{0};
        std::atomic<uint64_t> prefetchHits{0};
        std::unordered_map<CacheLevel, std::atomic<uint64_t>> levelHits;
        std::unordered_map<CacheLevel, std::atomic<uint64_t>> levelMisses;
    };
    
    CacheStats m_stats;
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_cleanupThread;
    std::thread m_syncThread;
    std::thread m_statsThread;
    
    // Configuração
    json m_config;
    std::string m_sharedMemoryName;
    size_t m_sharedMemorySize{1024 * 1024 * 1024}; // 1GB
    std::chrono::seconds m_cleanupInterval{30};
    std::chrono::seconds m_syncInterval{5};
    std::chrono::seconds m_statsInterval{60};
    bool m_enablePrefetching{true};
    bool m_enableCompression{true};
    bool m_validateData{true};
    
    // Métodos privados
    void initializeSharedMemory();
    void initializeDistributedCache();
    void registerDefaultSerializers();
    
    void cleanupThread();
    void syncThread();
    void statsThread();
    
    void updateMetadataOnAccess(CacheItemMetadata& metadata);
    bool isExpired(const CacheItemMetadata& metadata) const;
    size_t estimateObjectSize(const std::any& value) const;
    
    std::vector<CacheKey> evictLRU(CacheLevel level, size_t targetCount);
    std::vector<CacheKey> evictLFU(CacheLevel level, size_t targetCount);
    std::vector<CacheKey> evictFIFO(CacheLevel level, size_t targetCount);
    std::vector<CacheKey> evictTTL(CacheLevel level, size_t targetCount);
    std::vector<CacheKey> evictPriority(CacheLevel level, size_t targetCount);
    
    bool checkAndEvict(CacheLevel level, size_t sizeToAdd);
    
    void notifyInvalidationListeners(const CacheKey& key);
    
    std::vector<uint8_t> serializeValue(const std::any& value, CacheItemType type);
    std::any deserializeValue(const std::vector<uint8_t>& data, CacheItemType type);
    
    void processPrefetch(const CacheKey& key);
    
    // L2 cache helpers
    typename L2Cache::SharedKey createSharedKey(const CacheKey& key);
    typename L2Cache::SharedEntry createSharedEntry(const CacheEntry& entry);
    CacheKey fromSharedKey(const typename L2Cache::SharedKey& sharedKey);
    CacheEntry fromSharedEntry(const typename L2Cache::SharedEntry& sharedEntry, const CacheKey& key);
    
    // L3 cache helpers
    bool distributedSet(const CacheKey& key, const std::vector<uint8_t>& serializedValue, const CacheItemMetadata& metadata);
    std::optional<std::vector<uint8_t>> distributedGet(const CacheKey& key);
    void distributedInvalidate(const CacheKey& key);
    
    // Métodos para implementação template
    bool setAny(const CacheKey& key, const std::any& value, CacheLevel level, std::chrono::milliseconds ttl);
    std::optional<std::any> getAny(const CacheKey& key, CacheLevel level);
};

// Implementação inline de métodos template
template<typename T>
bool CacheManager::set(const CacheKey& key, const T& value, std::chrono::milliseconds ttl) {
    return setAny(key, std::any(value), CacheLevel::L1, ttl);
}

template<typename T>
std::optional<T> CacheManager::get(const CacheKey& key) {
    auto anyResult = getAny(key, CacheLevel::L1);
    if (!anyResult.has_value()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(*anyResult);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

template<typename T>
std::optional<T> CacheManager::getOrCompute(const CacheKey& key, std::function<T()> computeFunc, std::chrono::milliseconds ttl) {
    auto result = get<T>(key);
    if (result.has_value()) {
        return result;
    }
    
    // Cache miss, compute the value
    T computedValue = computeFunc();
    set<T>(key, computedValue, ttl);
    return computedValue;
}

template<typename T>
bool CacheManager::setShared(const CacheKey& key, const T& value, std::chrono::milliseconds ttl) {
    return setAny(key, std::any(value), CacheLevel::L2, ttl);
}

template<typename T>
std::optional<T> CacheManager::getShared(const CacheKey& key) {
    auto anyResult = getAny(key, CacheLevel::L2);
    if (!anyResult.has_value()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(*anyResult);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

template<typename T>
bool CacheManager::setDistributed(const CacheKey& key, const T& value, std::chrono::milliseconds ttl) {
    return setAny(key, std::any(value), CacheLevel::L3, ttl);
}

template<typename T>
std::optional<T> CacheManager::getDistributed(const CacheKey& key) {
    auto anyResult = getAny(key, CacheLevel::L3);
    if (!anyResult.has_value()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(*anyResult);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

// Implementação inline básica do singleton
inline CacheManager& CacheManager::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<CacheManager>(new CacheManager());
    });
    return *s_instance;
}

} // namespace Cache
} // namespace WYDBR

// MemoryCache.h - Cache L1 em Memória para Dados Frequentes
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <optional>
#include <variant>
#include <any>
#include <nlohmann/json.hpp>

namespace WYDBR {
namespace Cache {

using json = nlohmann::json;

template<typename K, typename V>
class MemoryCache {
public:
    using KeyType = K;
    using ValueType = V;
    
    // Configuração do cache
    struct Config {
        size_t maxEntries{10000};
        std::chrono::seconds defaultTTL{300}; // 5 minutos
        bool enableStatistics{true};
        bool enableCompression{false};
        uint32_t compressionThreshold{1024}; // bytes
        uint32_t compressionLevel{6}; // 0-9
    };
    
    // Estatísticas do cache
    struct Stats {
        std::atomic<uint64_t> hits{0};
        std::atomic<uint64_t> misses{0};
        std::atomic<uint64_t> inserts{0};
        std::atomic<uint64_t> updates{0};
        std::atomic<uint64_t> evictions{0};
        std::atomic<uint64_t> expirations{0};
        std::atomic<uint64_t> totalBytesStored{0};
        std::atomic<uint64_t> totalCompressedBytes{0};
    };
    
    // Metadados de um item de cache
    struct EntryMetadata {
        std::chrono::system_clock::time_point createdAt;
        std::chrono::system_clock::time_point expiresAt;
        std::chrono::system_clock::time_point lastAccessedAt;
        uint64_t accessCount{0};
        size_t sizeBytes{0};
        bool compressed{false};
    };
    
    // Item de cache
    struct Entry {
        V value;
        EntryMetadata metadata;
    };
    
    // Construtor
    explicit MemoryCache(const Config& config = Config()) : m_config(config) {}
    
    // Operações básicas
    bool put(const K& key, const V& value, std::chrono::seconds ttl = std::chrono::seconds::zero()) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        // Aplicar TTL padrão se não for especificado
        auto effectiveTTL = (ttl.count() > 0) ? ttl : m_config.defaultTTL;
        
        auto now = std::chrono::system_clock::now();
        auto expiresAt = now + effectiveTTL;
        
        // Verificar se a chave já existe
        auto it = m_data.find(key);
        bool isUpdate = (it != m_data.end());
        
        // Determinar tamanho
        size_t sizeBytes = estimateSize(value);
        
        // Comprimir se necessário
        V storedValue = value;
        bool compressed = false;
        if (m_config.enableCompression && sizeBytes > m_config.compressionThreshold) {
            auto compressedValue = compressValue(value);
            if (compressedValue.has_value()) {
                storedValue = *compressedValue;
                compressed = true;
                
                if (m_config.enableStatistics) {
                    size_t compressedSize = estimateSize(storedValue);
                    m_stats.totalCompressedBytes += compressedSize;
                }
            }
        }
        
        // Verificar capacidade e potencialmente remover itens
        if (!isUpdate && m_data.size() >= m_config.maxEntries) {
            evictOne();
        }
        
        // Criar ou atualizar entrada
        Entry entry;
        entry.value = std::move(storedValue);
        entry.metadata.createdAt = isUpdate ? it->second.metadata.createdAt : now;
        entry.metadata.expiresAt = expiresAt;
        entry.metadata.lastAccessedAt = now;
        entry.metadata.accessCount = isUpdate ? it->second.metadata.accessCount : 0;
        entry.metadata.sizeBytes = sizeBytes;
        entry.metadata.compressed = compressed;
        
        // Atualizar estatísticas
        if (m_config.enableStatistics) {
            if (isUpdate) {
                m_stats.updates++;
                m_stats.totalBytesStored -= it->second.metadata.sizeBytes;
            } else {
                m_stats.inserts++;
            }
            m_stats.totalBytesStored += sizeBytes;
        }
        
        // Atualizar LRU
        updateLRU(key);
        
        // Armazenar ou atualizar o valor
        m_data[key] = std::move(entry);
        
        return true;
    }
    
    std::optional<V> get(const K& key) {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        auto it = m_data.find(key);
        if (it == m_data.end()) {
            if (m_config.enableStatistics) {
                m_stats.misses++;
            }
            return std::nullopt;
        }
        
        // Verificar expiração
        auto now = std::chrono::system_clock::now();
        if (now > it->second.metadata.expiresAt) {
            if (m_config.enableStatistics) {
                m_stats.expirations++;
                m_stats.misses++;
            }
            
            // Converter para lock exclusivo e remover
            lock.unlock();
            std::unique_lock<std::shared_mutex> uniqueLock(m_mutex);
            
            // Verificar novamente após obter o lock exclusivo
            it = m_data.find(key);
            if (it != m_data.end() && now > it->second.metadata.expiresAt) {
                m_data.erase(it);
            }
            
            return std::nullopt;
        }
        
        // Recuperar valor
        V result = it->second.value;
        
        // Descomprimir se necessário
        if (it->second.metadata.compressed) {
            auto decompressedValue = decompressValue(result);
            if (decompressedValue.has_value()) {
                result = *decompressedValue;
            } else {
                return std::nullopt;
            }
        }
        
        // Atualizar estatísticas e metadados
        if (m_config.enableStatistics) {
            m_stats.hits++;
        }
        
        // Converter para lock exclusivo para atualizar metadados
        lock.unlock();
        std::unique_lock<std::shared_mutex> uniqueLock(m_mutex);
        
        // Verificar novamente após obter o lock exclusivo
        it = m_data.find(key);
        if (it != m_data.end()) {
            it->second.metadata.lastAccessedAt = now;
            it->second.metadata.accessCount++;
            
            // Atualizar LRU
            updateLRU(key);
        }
        
        return result;
    }
    
    bool remove(const K& key) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        auto it = m_data.find(key);
        if (it == m_data.end()) {
            return false;
        }
        
        // Atualizar estatísticas
        if (m_config.enableStatistics) {
            m_stats.totalBytesStored -= it->second.metadata.sizeBytes;
        }
        
        // Remover da LRU
        removeLRU(key);
        
        // Remover do cache
        m_data.erase(it);
        
        return true;
    }
    
    void clear() {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_data.clear();
        m_lruList.clear();
        
        if (m_config.enableStatistics) {
            m_stats.totalBytesStored = 0;
            m_stats.totalCompressedBytes = 0;
        }
    }
    
    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_data.size();
    }
    
    bool contains(const K& key) const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        auto it = m_data.find(key);
        if (it == m_data.end()) {
            return false;
        }
        
        // Verificar expiração
        auto now = std::chrono::system_clock::now();
        return now <= it->second.metadata.expiresAt;
    }
    
    // Gerenciamento de cache
    void setMaxEntries(size_t maxEntries) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_config.maxEntries = maxEntries;
        
        // Reduzir tamanho se necessário
        while (m_data.size() > m_config.maxEntries) {
            evictOne();
        }
    }
    
    void setDefaultTTL(std::chrono::seconds ttl) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_config.defaultTTL = ttl;
    }
    
    void enableCompression(bool enable, uint32_t threshold = 1024, uint32_t level = 6) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_config.enableCompression = enable;
        m_config.compressionThreshold = threshold;
        m_config.compressionLevel = level;
    }
    
    // Estatísticas
    Stats getStats() const {
        return m_stats;
    }
    
    void resetStats() {
        m_stats = Stats();
    }
    
    // Manutenção
    uint32_t removeExpired() {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        auto now = std::chrono::system_clock::now();
        uint32_t removedCount = 0;
        
        auto it = m_data.begin();
        while (it != m_data.end()) {
            if (now > it->second.metadata.expiresAt) {
                if (m_config.enableStatistics) {
                    m_stats.expirations++;
                    m_stats.totalBytesStored -= it->second.metadata.sizeBytes;
                }
                
                removeLRU(it->first);
                it = m_data.erase(it);
                removedCount++;
            } else {
                ++it;
            }
        }
        
        return removedCount;
    }
    
    std::vector<K> getKeys() const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        std::vector<K> keys;
        keys.reserve(m_data.size());
        
        for (const auto& pair : m_data) {
            keys.push_back(pair.first);
        }
        
        return keys;
    }
    
private:
    // Dados
    std::unordered_map<K, Entry> m_data;
    std::list<K> m_lruList;
    mutable std::shared_mutex m_mutex;
    
    // Configuração
    Config m_config;
    
    // Estatísticas
    Stats m_stats;
    
    // Métodos auxiliares
    void updateLRU(const K& key) {
        // Remover a chave da lista se já existir
        for (auto it = m_lruList.begin(); it != m_lruList.end();) {
            if (*it == key) {
                it = m_lruList.erase(it);
            } else {
                ++it;
            }
        }
        
        // Adicionar ao início da lista (como mais recentemente usado)
        m_lruList.push_front(key);
    }
    
    void removeLRU(const K& key) {
        for (auto it = m_lruList.begin(); it != m_lruList.end();) {
            if (*it == key) {
                it = m_lruList.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void evictOne() {
        if (m_lruList.empty()) {
            return;
        }
        
        // Remover o item menos recentemente usado
        K lruKey = m_lruList.back();
        m_lruList.pop_back();
        
        auto it = m_data.find(lruKey);
        if (it != m_data.end()) {
            if (m_config.enableStatistics) {
                m_stats.evictions++;
                m_stats.totalBytesStored -= it->second.metadata.sizeBytes;
            }
            
            m_data.erase(it);
        }
    }
    
    size_t estimateSize(const V& value) {
        // Implementação depende do tipo V
        // Esta é uma aproximação simples
        return sizeof(V);
    }
    
    std::optional<V> compressValue(const V& value) {
        // Implementação para compressão, depende do tipo V
        // Retorna valor comprimido ou nullopt se não for possível comprimir
        return std::nullopt;
    }
    
    std::optional<V> decompressValue(const V& compressedValue) {
        // Implementação para descompressão, depende do tipo V
        // Retorna valor descomprimido ou nullopt se não for possível descomprimir
        return std::nullopt;
    }
};

} // namespace Cache
} // namespace WYDBR

// SharedCache.h - Cache L2 Compartilhado Entre Processos
#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <optional>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace WYDBR {
namespace Cache {

namespace ipc = boost::interprocess;

template<typename KeyType, typename ValueType>
class SharedCache {
private:
    // Tipos para IPC
    using SharedMemory = ipc::managed_shared_memory;
    using SegmentManager = SharedMemory::segment_manager;
    using CharAllocator = ipc::allocator<char, SegmentManager>;
    using SharedString = ipc::basic_string<char, std::char_traits<char>, CharAllocator>;
    
    // Declaração de tipos para os valores armazenados
    using ValueAllocator = ipc::allocator<uint8_t, SegmentManager>;
    using SharedVector = ipc::vector<uint8_t, ValueAllocator>;
    
    // Chave para os dados compartilhados
    struct SharedKey {
        SharedString key;
        
        SharedKey(const KeyType& k, const CharAllocator& alloc) : key(alloc) {
            key = SharedString(k.c_str(), alloc);
        }
        
        bool operator<(const SharedKey& other) const {
            return key < other.key;
        }
    };
    
    // Metadados de uma entrada de cache
    struct SharedMetadata {
        uint64_t createdAt;
        uint64_t expiresAt;
        uint64_t lastAccessedAt;
        uint64_t accessCount;
        size_t sizeBytes;
        bool compressed;
    };
    
    // Entrada no cache compartilhado
    struct SharedEntry {
        SharedMetadata metadata;
        SharedVector data;
        
        SharedEntry(const ValueAllocator& alloc) : data(alloc) {}
    };
    
    // Tipos para o mapa de dados compartilhados
    using KeyAllocator = ipc::allocator<SharedKey, SegmentManager>;
    using ValuePairAllocator = ipc::allocator<std::pair<const SharedKey, SharedEntry>, SegmentManager>;
    using SharedMap = ipc::map<SharedKey, SharedEntry, std::less<SharedKey>, ValuePairAllocator>;
    
    // Estrutura de dados compartilhada principal
    struct SharedData {
        SharedMap entries;
        std::atomic<uint64_t> totalSizeBytes;
        std::atomic<uint64_t> hitCount;
        std::atomic<uint64_t> missCount;
        std::atomic<uint64_t> insertCount;
        std::atomic<uint64_t> updateCount;
        std::atomic<uint64_t> evictionCount;
        std::atomic<uint64_t> expirationCount;
        
        SharedData(const ValuePairAllocator& alloc) : entries(std::less<SharedKey>(), alloc), 
            totalSizeBytes(0), hitCount(0), missCount(0), insertCount(0), 
            updateCount(0), evictionCount(0), expirationCount(0) {}
    };
    
public:
    // Configuração do cache
    struct Config {
        std::string segmentName;
        size_t segmentSize{1024 * 1024 * 512}; // 512MB
        size_t maxEntries{100000};
        std::chrono::seconds defaultTTL{600}; // 10 minutos
        bool enableCompression{true};
        uint32_t compressionThreshold{4096}; // bytes
        uint32_t compressionLevel{6}; // 0-9
    };
    
    // Estatísticas do cache
    struct Stats {
        uint64_t hits{0};
        uint64_t misses{0};
        uint64_t inserts{0};
        uint64_t updates{0};
        uint64_t evictions{0};
        uint64_t expirations{0};
        uint64_t totalSizeBytes{0};
        size_t entryCount{0};
    };
    
    // Construtor
    explicit SharedCache(const Config& config = Config())
        : m_config(config), m_segment(nullptr), m_data(nullptr), m_mutex(nullptr) {
        initialize();
    }
    
    // Destrutor
    ~SharedCache() {
        try {
            if (m_mutex) {
                ipc::named_mutex::remove(getMutexName().c_str());
            }
        } catch (...) {
            // Ignorar exceções no destrutor
        }
    }
    
    // Inicialização
    bool initialize() {
        try {
            // Remover segmento se já existir
            SharedMemory::remove(m_config.segmentName.c_str());
            
            // Criar novo segmento
            m_segment = std::make_unique<SharedMemory>(
                ipc::create_only, 
                m_config.segmentName.c_str(), 
                m_config.segmentSize
            );
            
            // Criar mutex compartilhado
            ipc::named_mutex::remove(getMutexName().c_str());
            m_mutex = std::make_unique<ipc::named_mutex>(ipc::create_only, getMutexName().c_str());
            
            // Criar estrutura de dados compartilhada
            ValuePairAllocator alloc(m_segment->get_segment_manager());
            m_data = m_segment->construct<SharedData>("SharedCache")(alloc);
            
            return true;
        } catch (const std::exception& e) {
            // Tentar abrir um segmento existente
            try {
                m_segment = std::make_unique<SharedMemory>(
                    ipc::open_only, 
                    m_config.segmentName.c_str()
                );
                
                m_mutex = std::make_unique<ipc::named_mutex>(ipc::open_only, getMutexName().c_str());
                
                m_data = m_segment->find<SharedData>("SharedCache").first;
                
                return m_data != nullptr;
            } catch (...) {
                return false;
            }
        }
    }
    
    // Operações básicas
    bool put(const KeyType& key, const ValueType& value, std::chrono::seconds ttl = std::chrono::seconds::zero()) {
        if (!m_data || !m_mutex) {
            return false;
        }
        
        // Converter valor para bytes serializados
        std::vector<uint8_t> serializedValue = serializeValue(value);
        
        // Aplicar compressão se necessário
        bool compressed = false;
        if (m_config.enableCompression && serializedValue.size() > m_config.compressionThreshold) {
            auto compressedValue = compressData(serializedValue);
            if (!compressedValue.empty()) {
                serializedValue = compressedValue;
                compressed = true;
            }
        }
        
        // Adquirir lock exclusivo
        ipc::scoped_lock<ipc::named_mutex> lock(*m_mutex);
        
        // Criar alocadores
        CharAllocator charAlloc(m_segment->get_segment_manager());
        ValueAllocator valueAlloc(m_segment->get_segment_manager());
        
        // Aplicar TTL padrão se não for especificado
        auto effectiveTTL = (ttl.count() > 0) ? ttl : m_config.defaultTTL;
        
        // Converter para timestamp
        auto now = std::chrono::system_clock::now();
        auto nowMs = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
        auto expiresAt = nowMs + std::chrono::duration_cast<std::chrono::milliseconds>(effectiveTTL).count();
        
        // Verificar se a chave já existe
        SharedKey sharedKey(key, charAlloc);
        auto it = m_data->entries.find(sharedKey);
        bool isUpdate = (it != m_data->entries.end());
        
        // Verificar limites de tamanho
        if (!isUpdate && m_data->entries.size() >= m_config.maxEntries) {
            evictOne();
        }
        
        // Preparar entrada no cache
        SharedEntry entry(valueAlloc);
        entry.metadata.createdAt = isUpdate ? it->second.metadata.createdAt : nowMs;
        entry.metadata.expiresAt = expiresAt;
        entry.metadata.lastAccessedAt = nowMs;
        entry.metadata.accessCount = isUpdate ? it->second.metadata.accessCount : 0;
        entry.metadata.sizeBytes = serializedValue.size();
        entry.metadata.compressed = compressed;
        
        // Copiar dados
        entry.data.assign(serializedValue.begin(), serializedValue.end());
        
        // Atualizar estatísticas
        if (isUpdate) {
            m_data->totalSizeBytes -= it->second.metadata.sizeBytes;
            m_data->updateCount++;
        } else {
            m_data->insertCount++;
        }
        m_data->totalSizeBytes += serializedValue.size();
        
        // Inserir ou atualizar no mapa
        if (isUpdate) {
            it->second = std::move(entry);
        } else {
            m_data->entries.insert(std::make_pair(sharedKey, std::move(entry)));
        }
        
        return true;
    }
    
    std::optional<ValueType> get(const KeyType& key) {
        if (!m_data || !m_mutex) {
            return std::nullopt;
        }
        
        // Adquirir lock
        ipc::scoped_lock<ipc::named_mutex> lock(*m_mutex);
        
        // Construir chave compartilhada
        CharAllocator charAlloc(m_segment->get_segment_manager());
        SharedKey sharedKey(key, charAlloc);
        
        // Buscar no mapa
        auto it = m_data->entries.find(sharedKey);
        if (it == m_data->entries.end()) {
            m_data->missCount++;
            return std::nullopt;
        }
        
        // Verificar expiração
        auto now = std::chrono::system_clock::now();
        auto nowMs = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
        
        if (nowMs > it->second.metadata.expiresAt) {
            m_data->expirationCount++;
            m_data->missCount++;
            m_data->totalSizeBytes -= it->second.metadata.sizeBytes;
            m_data->entries.erase(it);
            return std::nullopt;
        }
        
        // Extrair dados
        std::vector<uint8_t> data(it->second.data.begin(), it->second.data.end());
        
        // Descomprimir se necessário
        if (it->second.metadata.compressed) {
            auto decompressedData = decompressData(data);
            if (decompressedData.empty()) {
                return std::nullopt;
            }
            data = decompressedData;
        }
        
        // Atualizar estatísticas e metadados
        m_data->hitCount++;
        it->second.metadata.lastAccessedAt = nowMs;
        it->second.metadata.accessCount++;
        
        // Deserializar para valor
        return deserializeValue(data);
    }
    
    bool remove(const KeyType& key) {
        if (!m_data || !m_mutex) {
            return false;
        }
        
        // Adquirir lock exclusivo
        ipc::scoped_lock<ipc::named_mutex> lock(*m_mutex);
        
        // Construir chave compartilhada
        CharAllocator charAlloc(m_segment->get_segment_manager());
        SharedKey sharedKey(key, charAlloc);
        
        // Buscar e remover
        auto it = m_data->entries.find(sharedKey);
        if (it == m_data->entries.end()) {
            return false;
        }
        
        m_data->totalSizeBytes -= it->second.metadata.sizeBytes;
        m_data->entries.erase(it);
        
        return true;
    }
    
    void clear() {
        if (!m_data || !m_mutex) {
            return;
        }
        
        // Adquirir lock exclusivo
        ipc::scoped_lock<ipc::named_mutex> lock(*m_mutex);
        
        m_data->entries.clear();
        m_data->totalSizeBytes = 0;
    }
    
    size_t size() const {
        if (!m_data || !m_mutex) {
            return 0;
        }
        
        // Adquirir lock compartilhado
        ipc::scoped_lock<ipc::named_mutex> lock(*m_mutex);
        
        return m_data->entries.size();
    }
    
    bool contains(const KeyType& key) const {
        if (!m_data || !m_mutex) {
            return false;
        }
        
        // Adquirir lock compartilhado
        ipc::scoped_lock<ipc::named_mutex> lock(*m_mutex);
        
        // Construir chave compartilhada
        CharAllocator charAlloc(m_segment->get_segment_manager());
        SharedKey sharedKey(key, charAlloc);
        
        // Buscar
        auto it = m_data->entries.find(sharedKey);
        if (it == m_data->entries.end()) {
            return false;
        }
        
        // Verificar expiração
        auto now = std::chrono::system_clock::now();
        auto nowMs = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
        
        return nowMs <= it->second.metadata.expiresAt;
    }
    
    // Estatísticas
    Stats getStats() const {
        Stats stats;
        
        if (!m_data || !m_mutex) {
            return stats;
        }
        
        // Adquirir lock compartilhado
        ipc::scoped_lock<ipc::named_mutex> lock(*m_mutex);
        
        stats.hits = m_data->hitCount;
        stats.misses = m_data->missCount;
        stats.inserts = m_data->insertCount;
        stats.updates = m_data->updateCount;
        stats.evictions = m_data->evictionCount;
        stats.expirations = m_data->expirationCount;
        stats.totalSizeBytes = m_data->totalSizeBytes;
        stats.entryCount = m_data->entries.size();
        
        return stats;
    }
    
    // Gerenciamento de cache
    void setMaxEntries(size_t maxEntries) {
        if (!m_data || !m_mutex) {
            return;
        }
        
        // Adquirir lock exclusivo
        ipc::scoped_lock<ipc::named_mutex> lock(*m_mutex);
        
        m_config.maxEntries = maxEntries;
        
        // Reduzir tamanho se necessário
        while (m_data->entries.size() > m_config.maxEntries) {
            evictOne();
        }
    }
    
    uint32_t removeExpired() {
        if (!m_data || !m_mutex) {
            return 0;
        }
        
        // Adquirir lock exclusivo
        ipc::scoped_lock<ipc::named_mutex> lock(*m_mutex);
        
        auto now = std::chrono::system_clock::now();
        auto nowMs = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
        
        uint32_t removedCount = 0;
        auto it = m_data->entries.begin();
        
        while (it != m_data->entries.end()) {
            if (nowMs > it->second.metadata.expiresAt) {
                m_data->totalSizeBytes -= it->second.metadata.sizeBytes;
                m_data->expirationCount++;
                it = m_data->entries.erase(it);
                removedCount++;
            } else {
                ++it;
            }
        }
        
        return removedCount;
    }
    
private:
    // Configuração e estado
    Config m_config;
    std::unique_ptr<SharedMemory> m_segment;
    SharedData* m_data;
    std::unique_ptr<ipc::named_mutex> m_mutex;
    
    // Métodos auxiliares
    std::string getMutexName() const {
        return m_config.segmentName + "_mutex";
    }
    
    void evictOne() {
        if (m_data->entries.empty()) {
            return;
        }
        
        // Aqui fazemos uma política LRU simplificada, removendo o registro com
        // o lastAccessedAt mais antigo
        auto oldest = m_data->entries.begin();
        uint64_t oldestAccess = std::numeric_limits<uint64_t>::max();
        
        for (auto it = m_data->entries.begin(); it != m_data->entries.end(); ++it) {
            if (it->second.metadata.lastAccessedAt < oldestAccess) {
                oldest = it;
                oldestAccess = it->second.metadata.lastAccessedAt;
            }
        }
        
        if (oldest != m_data->entries.end()) {
            m_data->totalSizeBytes -= oldest->second.metadata.sizeBytes;
            m_data->evictionCount++;
            m_data->entries.erase(oldest);
        }
    }
    
    // Serialização/deserialização - aqui são simplesmente exemplos
    std::vector<uint8_t> serializeValue(const ValueType& value) {
        // Serialização de example - implementação depende do tipo ValueType
        // Para este exemplo, apenas convertemos valor para array de bytes
        std::vector<uint8_t> result;
        result.resize(sizeof(ValueType));
        memcpy(result.data(), &value, sizeof(ValueType));
        return result;
    }
    
    std::optional<ValueType> deserializeValue(const std::vector<uint8_t>& data) {
        // Deserialização de exemplo - implementação depende do tipo ValueType
        if (data.size() != sizeof(ValueType)) {
            return std::nullopt;
        }
        
        ValueType value;
        memcpy(&value, data.data(), sizeof(ValueType));
        return value;
    }
    
    // Compressão/descompressão - aqui são simplesmente exemplos
    std::vector<uint8_t> compressData(const std::vector<uint8_t>& data) {
        // Implementação de compressão - depende da biblioteca usada
        // Aqui retornamos os dados originais como simplificação
        return data;
    }
    
    std::vector<uint8_t> decompressData(const std::vector<uint8_t>& compressedData) {
        // Implementação de descompressão - depende da biblioteca usada
        // Aqui retornamos os dados originais como simplificação
        return compressedData;
    }
};

} // namespace Cache
} // namespace WYDBR

// CacheInvalidationPolicy.h - Políticas de Invalidação Inteligentes
#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <chrono>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include "CacheManager.h"

namespace WYDBR {
namespace Cache {

using json = nlohmann::json;

enum class InvalidationStrategy {
    TARGETED,
    CATEGORY,
    PATTERN,
    TAG_BASED,
    TIME_BASED,
    DEPENDENCY_BASED,
    ACCESS_BASED
};

enum class InvalidationMode {
    IMMEDIATE,
    LAZY,
    TIMED,
    EVENT_BASED
};

class CacheInvalidationPolicy {
public:
    // Singleton
    static CacheInvalidationPolicy& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    
    // Registro de estratégias de invalidação
    void registerInvalidationStrategy(const std::string& category, InvalidationStrategy strategy);
    void registerDependencyRule(const std::string& sourceCategory, const std::string& targetCategory);
    void registerPatternRule(const std::string& category, const std::string& pattern);
    void registerTagRule(const std::string& category, const std::string& tagKey);
    void registerTimedInvalidation(const std::string& category, std::chrono::seconds interval);
    
    // Operações de invalidação
    void invalidateKey(const CacheKey& key);
    void invalidateCategory(const std::string& category);
    void invalidatePattern(const std::string& pattern);
    void invalidateTag(const std::string& tagKey, const std::string& tagValue);
    void invalidateDependencies(const CacheKey& key);
    
    // Configuração de modo
    void setInvalidationMode(InvalidationMode mode);
    InvalidationMode getInvalidationMode() const;
    
    // Invalidação programada
    void scheduleInvalidation(const CacheKey& key, std::chrono::system_clock::time_point when);
    void scheduleInvalidation(const std::string& category, std::chrono::system_clock::time_point when);
    
    // Callbacks de invalidação
    void registerInvalidationCallback(std::function<void(const CacheKey&)> callback);
    void registerInvalidationCallback(const std::string& category, std::function<void(const CacheKey&)> callback);
    
    // Processamento de invalidações
    void processQueuedInvalidations();
    
    // Estatísticas
    json getInvalidationStats() const;
    
private:
    CacheInvalidationPolicy();
    ~CacheInvalidationPolicy();
    
    // Atributos privados
    static std::unique_ptr<CacheInvalidationPolicy> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<CacheManager> m_cacheManager;
    
    // Estratégias por categoria
    std::unordered_map<std::string, InvalidationStrategy> m_strategies;
    std::mutex m_strategiesMutex;
    
    // Regras de dependência
    std::unordered_map<std::string, std::vector<std::string>> m_dependencies; // sourceCategory -> [targetCategories]
    std::mutex m_dependenciesMutex;
    
    // Regras de padrão
    std::unordered_map<std::string, std::vector<std::string>> m_patterns; // category -> [patterns]
    std::mutex m_patternsMutex;
    
    // Regras de tag
    std::unordered_set<std::string> m_tagKeys;
    std::mutex m_tagsMutex;
    
    // Invalidações baseadas em tempo
    struct TimedInvalidation {
        std::string category;
        std::chrono::seconds interval;
        std::chrono::system_clock::time_point lastInvalidation;
    };
    
    std::vector<TimedInvalidation> m_timedInvalidations;
    std::mutex m_timedMutex;
    
    // Invalidações programadas
    struct ScheduledInvalidation {
        std::variant<CacheKey, std::string> target; // key ou category
        std::chrono::system_clock::time_point scheduledTime;
    };
    
    std::vector<ScheduledInvalidation> m_scheduledInvalidations;
    std::mutex m_scheduledMutex;
    
    // Fila de invalidações para lazy mode
    std::vector<CacheKey> m_invalidationQueue;
    std::mutex m_queueMutex;
    
    // Callbacks
    std::vector<std::function<void(const CacheKey&)>> m_globalCallbacks;
    std::unordered_map<std::string, std::vector<std::function<void(const CacheKey&)>>> m_categoryCallbacks;
    std::mutex m_callbacksMutex;
    
    // Estado
    InvalidationMode m_mode{InvalidationMode::IMMEDIATE};
    std::mutex m_modeMutex;
    
    // Estatísticas
    struct InvalidationStats {
        std::atomic<uint64_t> totalInvalidations{0};
        std::atomic<uint64_t> targetedInvalidations{0};
        std::atomic<uint64_t> categoryInvalidations{0};
        std::atomic<uint64_t> patternInvalidations{0};
        std::atomic<uint64_t> tagInvalidations{0};
        std::atomic<uint64_t> timeInvalidations{0};
        std::atomic<uint64_t> dependencyInvalidations{0};
        std::atomic<uint64_t> scheduledInvalidations{0};
        std::atomic<uint64_t> queuedInvalidations{0};
    };
    
    InvalidationStats m_stats;
    
    // Métodos privados
    void executeInvalidation(const CacheKey& key);
    void executeInvalidation(const std::string& category);
    void executePatternInvalidation(const std::string& pattern);
    void executeTagInvalidation(const std::string& tagKey, const std::string& tagValue);
    
    bool keyMatchesPattern(const CacheKey& key, const std::string& pattern) const;
    std::vector<CacheKey> findKeysByPattern(const std::string& pattern) const;
    std::vector<CacheKey> findKeysByTag(const std::string& tagKey, const std::string& tagValue) const;
    std::vector<CacheKey> findDependentKeys(const CacheKey& key) const;
    
    void notifyInvalidationCallbacks(const CacheKey& key);
    void checkTimedInvalidations();
    void checkScheduledInvalidations();
    
    void enqueueInvalidation(const CacheKey& key);
};

// Implementação inline básica do singleton
inline CacheInvalidationPolicy& CacheInvalidationPolicy::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<CacheInvalidationPolicy>(new CacheInvalidationPolicy());
    });
    return *s_instance;
}

} // namespace Cache
} // namespace WYDBR

// PredictivePrefetcher.h - Sistema de Pre-fetching Preditivo
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include "CacheManager.h"

namespace WYDBR {
namespace Cache {

using json = nlohmann::json;

enum class PrefetchStrategy {
    RULE_BASED,
    ACCESS_PATTERN,
    TEMPORAL_LOCALITY,
    SPATIAL_LOCALITY,
    MACHINE_LEARNING
};

struct AccessPattern {
    CacheKey key;
    std::chrono::system_clock::time_point timestamp;
    std::string sessionId;
    std::string userId;
    std::vector<CacheKey> previousAccesses;
};

struct PrefetchRule {
    std::string name;
    std::string triggerCategory;
    std::vector<std::string> targetCategories;
    std::function<std::vector<std::string>(const std::string&)> idMapper;
    float confidence{1.0f};
    uint32_t priority{0};
    bool enabled{true};
};

class PredictivePrefetcher {
public:
    // Singleton
    static PredictivePrefetcher& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Registro de padrões de acesso
    void recordAccess(const CacheKey& key, const std::string& sessionId = "", const std::string& userId = "");
    
    // Registro de regras de prefetch
    void registerPrefetchRule(const PrefetchRule& rule);
    void registerPrefetchRule(const std::string& triggerCategory, 
                           const std::vector<std::string>& targetCategories,
                           std::function<std::vector<std::string>(const std::string&)> idMapper,
                           float confidence = 1.0f,
                           uint32_t priority = 0);
    
    void enableRule(const std::string& ruleName, bool enable);
    
    // Prefetch explícito
    void triggerPrefetch(const CacheKey& key);
    void triggerPrefetchForUser(const std::string& userId);
    void triggerPrefetchForSession(const std::string& sessionId);
    
    // Configuração de estratégia
    void setStrategy(PrefetchStrategy strategy);
    void setMaxPrefetchPerTrigger(uint32_t maxCount);
    void setMinConfidence(float minConfidence);
    
    // Estatísticas e monitoramento
    json getPrefetchStats() const;
    std::vector<std::string> getActiveRules() const;
    
private:
    PredictivePrefetcher();
    ~PredictivePrefetcher();
    
    // Atributos privados
    static std::unique_ptr<PredictivePrefetcher> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<CacheManager> m_cacheManager;
    
    // Configuração
    PrefetchStrategy m_strategy{PrefetchStrategy::RULE_BASED};
    uint32_t m_maxPrefetchPerTrigger{20};
    float m_minConfidence{0.2f};
    uint32_t m_maxHistoryPerUser{100};
    uint32_t m_maxHistoryPerSession{50};
    std::chrono::seconds m_accessHistoryWindow{3600}; // 1 hora
    std::chrono::milliseconds m_prefetchDelay{10}; // Atraso para não bloquear thread principal
    
    // Regras de prefetch
    std::vector<PrefetchRule> m_rules;
    std::unordered_map<std::string, std::vector<size_t>> m_categoryRuleIndex; // triggerCategory -> [ruleIndices]
    mutable std::shared_mutex m_rulesMutex;
    
    // Histórico de acesso
    struct AccessHistory {
        std::vector<AccessPattern> patterns;
        std::unordered_map<std::string, std::vector<CacheKey>> categorySequences; // category -> [keys]
        std::chrono::system_clock::time_point lastAccess;
    };
    
    std::unordered_map<std::string, AccessHistory> m_userAccessHistory; // userId -> AccessHistory
    std::unordered_map<std::string, AccessHistory> m_sessionAccessHistory; // sessionId -> AccessHistory
    std::vector<AccessPattern> m_globalAccessHistory;
    mutable std::shared_mutex m_historyMutex;
    
    // Estatísticas
    struct PrefetchStats {
        std::atomic<uint64_t> totalPrefetches{0};
        std::atomic<uint64_t> rulePrefetches{0};
        std::atomic<uint64_t> patternPrefetches{0};
        std::atomic<uint64_t> usedPrefetches{0};
        std::atomic<uint64_t> uniqueItemsPrefetched{0};
        std::atomic<uint64_t> totalAccessesRecorded{0};
    };
    
    PrefetchStats m_stats;
    
    // Estado
    std::atomic<bool> m_running{false};
    
    // Threads de trabalho
    std::thread m_analysisThread;
    std::thread m_prefetchThread;
    
    // Fila de prefetch
    struct PrefetchTask {
        CacheKey triggerKey;
        std::string sessionId;
        std::string userId;
        std::chrono::system_clock::time_point queuedAt;
        uint32_t priority{0};
        
        bool operator<(const PrefetchTask& other) const {
            return priority < other.priority;
        }
    };
    
    std::priority_queue<PrefetchTask> m_prefetchQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    
    // Métodos privados
    void analysisThread();
    void prefetchThread();
    
    std::vector<CacheKey> predictNextKeys(const CacheKey& currentKey, const std::string& sessionId, const std::string& userId);
    std::vector<CacheKey> applyRuleBased(const CacheKey& currentKey);
    std::vector<CacheKey> applyAccessPattern(const CacheKey& currentKey, const std::string& sessionId, const std::string& userId);
    std::vector<CacheKey> applyTemporalLocality(const std::string& sessionId, const std::string& userId);
    std::vector<CacheKey> applySpatialLocality(const CacheKey& currentKey);
    
    void executePrefetch(const std::vector<CacheKey>& keys);
    void updateAccessHistory();
    void detectAccessPatterns();
    
    void queuePrefetchTask(const PrefetchTask& task);
    void processPrefetchQueue();
};

// Implementação inline básica do singleton
inline PredictivePrefetcher& PredictivePrefetcher::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<PredictivePrefetcher>(new PredictivePrefetcher());
    });
    return *s_instance;
}

} // namespace Cache
} // namespace WYDBR
`;

// Fornece uma API JavaScript para integração no projeto
module.exports.setupSystem = function(engineContext) {
  console.log('Inicializando sistema: sistema_cache_hierarquico');
  
  // Valida dependências necessárias
  const requiredDependencies = ['sistema_banco_dados_distribuido', 'sistema_eventos_distribuidos'];
  const missingDeps = requiredDependencies.filter(dep => !engineContext.systems[dep]);
  
  if (missingDeps.length > 0) {
    throw new Error(`Dependências necessárias não encontradas: ${missingDeps.join(', ')}`);
  }
  
  // Inicializa o sistema no contexto do engine
  engineContext.registerSystem({
    name: 'sistema_cache_hierarquico',
    source: module.exports.cppCode,
    dependencies: getSystemDependencies()
  });

  let isInitialized = false;
  let isShuttingDown = false;
  let cacheStats = {
    hits: 0,
    misses: 0,
    evictions: 0,
    memoryUsage: 0,
    lastHealthCheck: null
  };

  return {
    initialize: () => {
      if (isInitialized) {
        console.warn('Sistema de cache já inicializado');
        return true;
      }

      try {
        // Inicialização do sistema
        console.log('Inicializando sistema de cache hierárquico...');
        
        // Aqui você pode adicionar lógica de inicialização real
        // Por exemplo, configurar níveis de cache, pools de memória, etc.
        
        isInitialized = true;
        console.log('Sistema de cache inicializado com sucesso');
        return true;
      } catch (error) {
        console.error('Erro ao inicializar sistema de cache:', error);
        return false;
      }
    },
    
    shutdown: () => {
      if (!isInitialized || isShuttingDown) {
        console.warn('Sistema de cache não está inicializado ou já está sendo desligado');
        return true;
      }

      try {
        isShuttingDown = true;
        console.log('Desligando sistema de cache hierárquico...');
        
        // Aqui você pode adicionar lógica de desligamento real
        // Por exemplo, persistir cache, limpar memória, etc.
        
        isInitialized = false;
        isShuttingDown = false;
        console.log('Sistema de cache desligado com sucesso');
        return true;
      } catch (error) {
        console.error('Erro ao desligar sistema de cache:', error);
        return false;
      }
    },

    // Métodos adicionais para monitoramento e diagnóstico
    getStatus: () => ({
      initialized: isInitialized,
      shuttingDown: isShuttingDown,
      dependencies: getSystemDependencies(),
      stats: cacheStats
    }),

    // Métodos para manipulação do cache
    getCacheStats: () => ({ ...cacheStats }),
    updateCacheStats: (newStats) => {
      cacheStats = {
        ...cacheStats,
        ...newStats,
        lastHealthCheck: new Date()
      };
    },
    resetCacheStats: () => {
      cacheStats = {
        hits: 0,
        misses: 0,
        evictions: 0,
        memoryUsage: 0,
        lastHealthCheck: new Date()
      };
    }
  };
};

// Retorna as dependências específicas deste sistema
function getSystemDependencies() {
  // Analisar o código para determinar dependências automaticamente
  const dependencies = [];
  const code = module.exports.cppCode;
  
  // Análise básica de dependências incluídas no arquivo C++
  const includeRegex = /#include\s+["<]([^">]+)[">]/g;
  let match;
  while ((match = includeRegex.exec(code)) !== null) {
    // Ignora includes padrão do C++
    if (!match[1].startsWith('string') && 
        !match[1].startsWith('vector') && 
        !match[1].startsWith('map') &&
        !match[1].startsWith('unordered_map') &&
        !match[1].startsWith('set') &&
        !match[1].startsWith('mutex') &&
        !match[1].startsWith('shared_mutex') &&
        !match[1].startsWith('atomic') &&
        !match[1].startsWith('chrono') &&
        !match[1].startsWith('thread') &&
        !match[1].startsWith('functional') &&
        !match[1].startsWith('memory') &&
        !match[1].startsWith('optional') &&
        !match[1].startsWith('random') &&
        !match[1].startsWith('variant') &&
        !match[1].startsWith('nlohmann/json')) {
      
      dependencies.push(match[1].replace('.h', ''));
    }
  }
  
  return dependencies;
}
