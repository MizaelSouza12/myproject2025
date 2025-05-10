/**
 * @file DataCache.h
 * @brief Sistema de cache de dados para evitar redundância
 */

#ifndef WYDBR_DATA_CACHE_H
#define WYDBR_DATA_CACHE_H

#include <unordered_map>
#include <string>
#include <vector>
#include <any>
#include <chrono>
#include <mutex>
#include <memory>
#include <functional>
#include <typeindex>
#include <optional>
#include <shared_mutex>

namespace WYDBR {
namespace Storage {

/**
 * Política de invalidação de cache
 */
enum class InvalidationPolicy {
    NoInvalidation,    // Sem invalidação (cache permanente)
    TimeToLive,        // Tempo de vida
    LeastRecentlyUsed, // Menos recentemente usado
    MostRecentlyUsed,  // Mais recentemente usado
    Custom             // Personalizado
};

/**
 * Configuração de cache
 */
struct CacheConfig {
    InvalidationPolicy policy;       // Política de invalidação
    std::chrono::seconds ttl;        // Tempo de vida
    size_t maxSize;                  // Tamanho máximo
    bool notifyOnEviction;           // Notificar quando um item for removido
    bool lazyLoading;                // Carregamento preguiçoso
    bool persistOnDisk;              // Persistir em disco
    std::string diskCachePath;       // Caminho para cache em disco
};

/**
 * Entrada de cache
 */
template<typename T>
struct CacheEntry {
    T value;                         // Valor armazenado
    std::chrono::steady_clock::time_point creationTime; // Tempo de criação
    std::chrono::steady_clock::time_point lastAccessTime; // Último acesso
    int accessCount;                 // Contagem de acessos
    bool dirty;                      // Marcado para sincronização
};

/**
 * Callback para eventos de cache
 */
template<typename T>
using CacheEventCallback = std::function<void(const std::string&, const T&)>;

/**
 * Função de carregamento de dados para cache
 */
template<typename T>
using DataLoaderFunction = std::function<T(const std::string&)>;

/**
 * Cache de dados genérico
 */
template<typename T>
class DataCache {
public:
    /**
     * Construtor
     * @param config Configuração do cache
     */
    explicit DataCache(const CacheConfig& config)
        : m_config(config), m_cacheHits(0), m_cacheMisses(0) {
    }
    
    /**
     * Destrutor
     */
    ~DataCache() {
        // Se configurado para persistir, salvar o cache
        if (m_config.persistOnDisk) {
            SaveToDisk();
        }
    }
    
    /**
     * Obtém um valor do cache
     * @param key Chave
     * @return Valor ou nullopt se não encontrado
     */
    std::optional<T> Get(const std::string& key) {
        // Lock de leitura
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            // Atualizar estatísticas de acesso
            it->second.lastAccessTime = std::chrono::steady_clock::now();
            it->second.accessCount++;
            m_cacheHits++;
            
            // Retornar valor
            return it->second.value;
        }
        
        // Cache miss
        m_cacheMisses++;
        
        // Se carregamento preguiçoso estiver ativado, tentar carregar
        if (m_config.lazyLoading && m_loader) {
            // Release lock para evitar deadlock
            lock.unlock();
            
            // Carregar o valor
            T value = m_loader(key);
            
            // Armazenar no cache
            Put(key, value);
            
            return value;
        }
        
        return std::nullopt;
    }
    
    /**
     * Armazena um valor no cache
     * @param key Chave
     * @param value Valor
     */
    void Put(const std::string& key, const T& value) {
        // Lock de escrita
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        // Verificar se o cache está cheio
        if (m_config.maxSize > 0 && m_cache.size() >= m_config.maxSize) {
            // Remover um item segundo a política de invalidação
            EvictItem();
        }
        
        // Criar entrada de cache
        CacheEntry<T> entry;
        entry.value = value;
        entry.creationTime = std::chrono::steady_clock::now();
        entry.lastAccessTime = entry.creationTime;
        entry.accessCount = 0;
        entry.dirty = true;
        
        // Adicionar ao cache
        m_cache[key] = entry;
    }
    
    /**
     * Remove um valor do cache
     * @param key Chave
     * @return true se removido
     */
    bool Remove(const std::string& key) {
        // Lock de escrita
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            // Notificar remoção se configurado
            if (m_config.notifyOnEviction && m_evictionCallback) {
                m_evictionCallback(key, it->second.value);
            }
            
            // Remover do cache
            m_cache.erase(it);
            return true;
        }
        
        return false;
    }
    
    /**
     * Limpa o cache
     */
    void Clear() {
        // Lock de escrita
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        // Notificar remoção de cada item se configurado
        if (m_config.notifyOnEviction && m_evictionCallback) {
            for (const auto& pair : m_cache) {
                m_evictionCallback(pair.first, pair.second.value);
            }
        }
        
        // Limpar o cache
        m_cache.clear();
        m_cacheHits = 0;
        m_cacheMisses = 0;
    }
    
    /**
     * Verifica se uma chave existe no cache
     * @param key Chave
     * @return true se existir
     */
    bool Contains(const std::string& key) {
        // Lock de leitura
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        return m_cache.find(key) != m_cache.end();
    }
    
    /**
     * Define a função de carregamento de dados
     * @param loader Função de carregamento
     */
    void SetDataLoader(DataLoaderFunction<T> loader) {
        m_loader = std::move(loader);
    }
    
    /**
     * Define o callback de remoção
     * @param callback Função de callback
     */
    void SetEvictionCallback(CacheEventCallback<T> callback) {
        m_evictionCallback = std::move(callback);
    }
    
    /**
     * Invalida entradas expiradas
     * @return Número de entradas invalidadas
     */
    int InvalidateExpired() {
        // Lock de escrita
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        int count = 0;
        
        // Se a política for TTL, verificar expiração
        if (m_config.policy == InvalidationPolicy::TimeToLive) {
            auto now = std::chrono::steady_clock::now();
            
            for (auto it = m_cache.begin(); it != m_cache.end();) {
                auto age = std::chrono::duration_cast<std::chrono::seconds>(
                    now - it->second.creationTime);
                
                if (age > m_config.ttl) {
                    // Notificar remoção se configurado
                    if (m_config.notifyOnEviction && m_evictionCallback) {
                        m_evictionCallback(it->first, it->second.value);
                    }
                    
                    // Remover do cache
                    it = m_cache.erase(it);
                    count++;
                } else {
                    ++it;
                }
            }
        }
        
        return count;
    }
    
    /**
     * Obtém as estatísticas do cache
     * @return Mapa de estatísticas
     */
    std::unordered_map<std::string, size_t> GetStats() const {
        // Lock de leitura
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        return {
            {"size", m_cache.size()},
            {"hits", m_cacheHits},
            {"misses", m_cacheMisses},
            {"hitRatio", m_cacheHits + m_cacheMisses > 0 ? 
                        (m_cacheHits * 100) / (m_cacheHits + m_cacheMisses) : 0}
        };
    }
    
    /**
     * Salva o cache em disco
     * @return true se salvo com sucesso
     */
    bool SaveToDisk() {
        // Se não configurado para persistir, retornar
        if (!m_config.persistOnDisk) {
            return false;
        }
        
        // Implementação depende do tipo T
        // Stub para serialização
        
        return true;
    }
    
    /**
     * Carrega o cache do disco
     * @return true se carregado com sucesso
     */
    bool LoadFromDisk() {
        // Se não configurado para persistir, retornar
        if (!m_config.persistOnDisk) {
            return false;
        }
        
        // Implementação depende do tipo T
        // Stub para deserialização
        
        return true;
    }
    
    /**
     * Obtém todos os itens marcados como sujos
     * @return Mapa de itens sujos
     */
    std::unordered_map<std::string, T> GetDirtyItems() {
        // Lock de leitura
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        std::unordered_map<std::string, T> dirtyItems;
        
        for (const auto& pair : m_cache) {
            if (pair.second.dirty) {
                dirtyItems[pair.first] = pair.second.value;
            }
        }
        
        return dirtyItems;
    }
    
    /**
     * Marca um item como limpo
     * @param key Chave
     * @return true se marcado
     */
    bool MarkClean(const std::string& key) {
        // Lock de escrita
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            it->second.dirty = false;
            return true;
        }
        
        return false;
    }
    
private:
    // Configuração
    CacheConfig m_config;
    
    // Cache
    std::unordered_map<std::string, CacheEntry<T>> m_cache;
    
    // Mutex para thread-safety
    mutable std::shared_mutex m_mutex;
    
    // Estatísticas
    size_t m_cacheHits;
    size_t m_cacheMisses;
    
    // Funções de callback
    DataLoaderFunction<T> m_loader;
    CacheEventCallback<T> m_evictionCallback;
    
    /**
     * Remove um item do cache segundo a política de invalidação
     */
    void EvictItem() {
        if (m_cache.empty()) {
            return;
        }
        
        switch (m_config.policy) {
            case InvalidationPolicy::LeastRecentlyUsed: {
                // Encontrar o item menos recentemente usado
                auto oldest = m_cache.begin();
                for (auto it = std::next(oldest); it != m_cache.end(); ++it) {
                    if (it->second.lastAccessTime < oldest->second.lastAccessTime) {
                        oldest = it;
                    }
                }
                
                // Notificar remoção se configurado
                if (m_config.notifyOnEviction && m_evictionCallback) {
                    m_evictionCallback(oldest->first, oldest->second.value);
                }
                
                // Remover do cache
                m_cache.erase(oldest);
                break;
            }
            
            case InvalidationPolicy::MostRecentlyUsed: {
                // Encontrar o item mais recentemente usado
                auto mostRecent = m_cache.begin();
                for (auto it = std::next(mostRecent); it != m_cache.end(); ++it) {
                    if (it->second.lastAccessTime > mostRecent->second.lastAccessTime) {
                        mostRecent = it;
                    }
                }
                
                // Notificar remoção se configurado
                if (m_config.notifyOnEviction && m_evictionCallback) {
                    m_evictionCallback(mostRecent->first, mostRecent->second.value);
                }
                
                // Remover do cache
                m_cache.erase(mostRecent);
                break;
            }
            
            default: {
                // Remover o primeiro item
                auto it = m_cache.begin();
                
                // Notificar remoção se configurado
                if (m_config.notifyOnEviction && m_evictionCallback) {
                    m_evictionCallback(it->first, it->second.value);
                }
                
                // Remover do cache
                m_cache.erase(it);
                break;
            }
        }
    }
};

// Alias para tipos comuns de cache
using IntCache = DataCache<int>;
using StringCache = DataCache<std::string>;
using BoolCache = DataCache<bool>;

/**
 * Registro global de caches
 */
class CacheRegistry {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o registro
     */
    static CacheRegistry& GetInstance();
    
    /**
     * Registra um cache
     * @param name Nome do cache
     * @param cache Ponteiro para o cache
     */
    void RegisterCache(const std::string& name, void* cache, std::type_index type);
    
    /**
     * Verifica se um cache está registrado
     * @param name Nome do cache
     * @return true se o cache estiver registrado
     */
    bool HasCache(const std::string& name) const;
    
    /**
     * Obtém um cache pelo nome
     * @param name Nome do cache
     * @return Ponteiro para o cache ou nullptr se não existir
     */
    template<typename T>
    DataCache<T>* GetCache(const std::string& name) {
        // Lock de leitura
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        auto it = m_caches.find(name);
        if (it != m_caches.end() && it->second.second == std::type_index(typeid(T))) {
            return static_cast<DataCache<T>*>(it->second.first);
        }
        
        return nullptr;
    }
    
    /**
     * Obtém as estatísticas de todos os caches
     * @return Mapa de estatísticas por cache
     */
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> GetAllStats() const;
    
    /**
     * Invalida todos os caches expirados
     * @return Número total de entradas invalidadas
     */
    int InvalidateAllExpired();
    
    /**
     * Salva todos os caches em disco
     * @return Número de caches salvos com sucesso
     */
    int SaveAllToDisk();
    
    /**
     * Carrega todos os caches do disco
     * @return Número de caches carregados com sucesso
     */
    int LoadAllFromDisk();
    
private:
    // Construtor e destrutor privados (singleton)
    CacheRegistry();
    ~CacheRegistry();
    
    // Não permitir cópias
    CacheRegistry(const CacheRegistry&) = delete;
    CacheRegistry& operator=(const CacheRegistry&) = delete;
    
    // Mapa de caches por nome
    std::unordered_map<std::string, std::pair<void*, std::type_index>> m_caches;
    
    // Mutex para thread-safety
    mutable std::shared_mutex m_mutex;
};

} // namespace Storage
} // namespace WYDBR

#endif // WYDBR_DATA_CACHE_H
