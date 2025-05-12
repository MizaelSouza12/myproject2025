/**
 * DBCacheManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/dbsrv/DBCacheManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef DBCACHEMANAGER_H
#define DBCACHEMANAGER_H

#include <vector>
#include <unordered_map>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include <memory>

#include "DBTypes.h"
#include "../../core/WYDTypes.h"
#include "../../core/Logger.h"

namespace wydbr {


/**
 * @file DBCacheManager.h
 * @brief Gerenciador de cache do DBServer
 * 
 * Este arquivo contém a definição do gerenciador de cache para o DBServer,
 * que é usado para acelerar o acesso a dados frequentemente utilizados.
 * O WYD original tinha um sistema de cache, mas com vários problemas de performance
 * e concorrência que foram corrigidos nesta implementação.
 */

namespace wyd {
namespace server {

// Forward declarations
class CDBConnectionManager;

// Tipos de callback para eventos de cache
typedef std::function<void(DB_ENTITY_TYPE, DWORD, bool)> DBCacheEventCallback;

/**
 * @brief Política de cache para diferentes tipos de entidade
 * 
 * Esta estrutura define as políticas de cache para diferentes tipos de entidade,
 * como tempo de expiração, prioridade, etc.
 */
struct CachePolicy {
    DWORD dwExpirationTime;                 // Tempo de expiração em ms (0 = não expira)
    DWORD dwMaxEntries;                     // Número máximo de entradas (0 = sem limite)
    bool syncOnEviction;                    // Se deve sincronizar com o banco ao remover
    bool preloadEnabled;                    // Se deve pré-carregar dados
    DWORD dwEvictionThreshold;              // Limite para começar a remover (percentual do máximo)
    
    CachePolicy()
        : dwExpirationTime(0)
        , dwMaxEntries(0)
        , syncOnEviction(true)
        , preloadEnabled(false)
        , dwEvictionThreshold(80)           // 80% do máximo por padrão
    {
    }
};

/**
 * @brief Gerenciador de cache para o banco de dados
 * 
 * Esta classe gerencia o cache de entidades do banco de dados,
 * como contas, personagens, guildas, etc.
 * No WYD original, o sistema de cache era simples e tinha problemas de concorrência.
 */
/**
 * Classe WYDDBCacheManager
 * 
 * Esta classe implementa a funcionalidade WYDDBCacheManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CDBCacheManager {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static CDBCacheManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador de cache
     * @param syncInterval Intervalo entre sincronizações (em ms)
     * @return true se inicializado com sucesso
     */
    bool Initialize(DWORD syncInterval = 60000); // 1 minuto por padrão
    
    /**
     * @brief Finaliza o gerenciador de cache
     */
    void Shutdown();
    
    /**
     * @brief Define a política de cache para um tipo de entidade
     * @param entityType Tipo de entidade
     * @param policy Política de cache
     */
    void SetCachePolicy(DB_ENTITY_TYPE entityType, const CachePolicy& policy);
    
    /**
     * @brief Obtém a política de cache para um tipo de entidade
     * @param entityType Tipo de entidade
     * @return Política de cache
     */
    CachePolicy GetCachePolicy(DB_ENTITY_TYPE entityType) const;
    
    /**
     * @brief Adiciona uma entidade ao cache
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param data Ponteiro para os dados
     * @param dataSize Tamanho dos dados
     * @return true se adicionada com sucesso
     */
    bool AddToCache(DB_ENTITY_TYPE entityType, DWORD entityId, void* data, size_t dataSize);
    
    /**
     * @brief Obtém uma entidade do cache
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param outData Ponteiro para onde copiar os dados
     * @param maxSize Tamanho máximo permitido
     * @return true se encontrada e copiada com sucesso, false se não estiver no cache
     */
    bool GetFromCache(DB_ENTITY_TYPE entityType, DWORD entityId, void* outData, size_t maxSize);
    
    /**
     * @brief Verifica se uma entidade está no cache
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @return true se estiver no cache
     */
    bool IsInCache(DB_ENTITY_TYPE entityType, DWORD entityId) const;
    
    /**
     * @brief Remove uma entidade do cache
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param sync Se deve sincronizar com o banco antes de remover
     * @return true se removida com sucesso
     */
    bool RemoveFromCache(DB_ENTITY_TYPE entityType, DWORD entityId, bool sync = true);
    
    /**
     * @brief Marca uma entidade como suja (modificada, precisa sincronizar)
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @return true se marcada com sucesso
     */
    bool MarkAsDirty(DB_ENTITY_TYPE entityType, DWORD entityId);
    
    /**
     * @brief Atualiza uma entidade no cache
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param data Ponteiro para os novos dados
     * @param dataSize Tamanho dos dados
     * @return true se atualizada com sucesso
     */
    bool UpdateCache(DB_ENTITY_TYPE entityType, DWORD entityId, void* data, size_t dataSize);
    
    /**
     * @brief Sincroniza todas as entidades sujas com o banco
     * @return Número de entidades sincronizadas
     */
    int SyncDirtyEntities();
    
    /**
     * @brief Sincroniza uma entidade específica com o banco
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @return true se sincronizada com sucesso
     */
    bool SyncEntity(DB_ENTITY_TYPE entityType, DWORD entityId);
    
    /**
     * @brief Remove todas as entidades expiradas do cache
     * @return Número de entidades removidas
     */
    int EvictExpiredEntities();
    
    /**
     * @brief Limpa o cache completamente
     * @param sync Se deve sincronizar entidades sujas antes de limpar
     */
    void ClearCache(bool sync = true);
    
    /**
     * @brief Registra um callback para eventos de cache
     * @param callback Função de callback
     */
    void RegisterCacheEventCallback(const DBCacheEventCallback& callback);
    
    /**
     * @brief Pré-carrega entidades do banco para o cache
     * @param entityType Tipo de entidade
     * @param count Número de entidades a pré-carregar (0 = todas)
     * @return Número de entidades pré-carregadas
     */
    int PreloadEntities(DB_ENTITY_TYPE entityType, DWORD count = 0);
    
    /**
     * @brief Obtém estatísticas de cache
     * @return String com estatísticas
     */
    std::string GetCacheStats() const;
    
    /**
     * @brief Tenta adquirir um lock para uma entidade
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param lockOwner ID do owner do lock
     * @param timeout Timeout em ms (0 = sem timeout)
     * @return true se o lock foi adquirido
     */
    bool LockEntity(DB_ENTITY_TYPE entityType, DWORD entityId, DWORD lockOwner, DWORD timeout = 0);
    
    /**
     * @brief Libera o lock de uma entidade
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param lockOwner ID do owner do lock
     * @return true se o lock foi liberado
     */
    bool UnlockEntity(DB_ENTITY_TYPE entityType, DWORD entityId, DWORD lockOwner);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CDBCacheManager();
    
    /**
     * @brief Destrutor
     */
    ~CDBCacheManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CDBCacheManager(const CDBCacheManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CDBCacheManager& operator=(const CDBCacheManager&) = delete;
    
    /**
     * @brief Thread de sincronização periódica
     */
    void SyncThread();
    
    /**
     * @brief Thread de limpeza periódica
     */
    void CleanupThread();
    
    /**
     * @brief Decide se uma entidade deve ser removida do cache
     * @param entityType Tipo de entidade
     * @param entry Entrada de cache
     * @return true se deve ser removida
     */
    bool ShouldEvict(DB_ENTITY_TYPE entityType, const STRUCT_DBCACHEENTRY& entry) const;
    
    /**
     * @brief Notifica eventos de cache
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param isDirty Se a entidade está suja
     */
    void NotifyCacheEvent(DB_ENTITY_TYPE entityType, DWORD entityId, bool isDirty);
    
    /**
     * @brief Sincroniza uma entidade com o banco
     * @param entityType Tipo de entidade
     * @param entry Entrada de cache
     * @return true se sincronizada com sucesso
     */
    bool SyncEntityToDatabase(DB_ENTITY_TYPE entityType, STRUCT_DBCACHEENTRY& entry);
    
    /**
     * @brief Carrega uma entidade do banco para o cache
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @return Ponteiro para a entrada de cache, ou nullptr se falhou
     */
    STRUCT_DBCACHEENTRY* LoadEntityFromDatabase(DB_ENTITY_TYPE entityType, DWORD entityId);
    
    /**
     * @brief Gera a query SQL para carregar uma entidade
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @return Query SQL
     */
    std::string GenerateLoadQuery(DB_ENTITY_TYPE entityType, DWORD entityId) const;
    
    /**
     * @brief Gera a query SQL para salvar uma entidade
     * @param entityType Tipo de entidade
     * @param entry Entrada de cache
     * @return Query SQL
     */
    std::string GenerateSaveQuery(DB_ENTITY_TYPE entityType, const STRUCT_DBCACHEENTRY& entry) const;
    
    // Políticas de cache para cada tipo de entidade
    std::unordered_map<DB_ENTITY_TYPE, CachePolicy> cachePolicies_;
    std::mutex policyMutex_;
    
    // Cache principal (mapa de tipos de entidade para mapas de ID para entrada)
    std::unordered_map<DB_ENTITY_TYPE, std::unordered_map<DWORD, STRUCT_DBCACHEENTRY>> cache_;
    mutable std::shared_mutex cacheMutex_; // RW mutex para acesso ao cache
    
    // LRU (Least Recently Used) para cada tipo de entidade
    std::unordered_map<DB_ENTITY_TYPE, std::list<DWORD>> lruLists_;
    std::unordered_map<DB_ENTITY_TYPE, std::unordered_map<DWORD, std::list<DWORD>::iterator>> lruMap_;
    std::mutex lruMutex_;
    
    // Lista de entidades sujas para sincronização
    std::unordered_map<DB_ENTITY_TYPE, std::unordered_set<DWORD>> dirtyEntities_;
    std::mutex dirtyMutex_;
    
    // Callbacks para eventos
    std::vector<DBCacheEventCallback> eventCallbacks_;
    std::mutex callbackMutex_;
    
    // Threads
    std::thread syncThread_;
    std::thread cleanupThread_;
    std::atomic<bool> shutdownFlag_;
    
    // Sincronização
    DWORD syncInterval_;
    std::condition_variable syncCV_;
    std::mutex syncMutex_;
    
    // Estatísticas
    struct CacheStats {
        DWORD dwTotalCacheHits;             // Total de acertos de cache
        DWORD dwTotalCacheMisses;           // Total de erros de cache
        DWORD dwTotalCacheEvictions;        // Total de remoções de cache
        DWORD dwTotalEntities;              // Total de entidades no cache
        DWORD dwTotalDirtyEntities;         // Total de entidades sujas
        DWORD dwTotalSyncs;                 // Total de sincronizações
        DWORD dwTotalLocks;                 // Total de locks adquiridos
        DWORD dwLockContentions;            // Total de contenções de lock
        DWORD dwMaxCacheSize;               // Tamanho máximo do cache (bytes)
        DWORD dwCurrentCacheSize;           // Tamanho atual do cache (bytes)
        float fHitRatio;                    // Razão de acertos (%)
        
        CacheStats()
            : dwTotalCacheHits(0)
            , dwTotalCacheMisses(0)
            , dwTotalCacheEvictions(0)
            , dwTotalEntities(0)
            , dwTotalDirtyEntities(0)
            , dwTotalSyncs(0)
            , dwTotalLocks(0)
            , dwLockContentions(0)
            , dwMaxCacheSize(0)
            , dwCurrentCacheSize(0)
            , fHitRatio(0.0f)
        {
        }
    };
    
    mutable std::mutex statsMutex_;
    CacheStats stats_;
    
    bool initialized_;
};

// Definição global para facilitar o acesso
#define g_DBCache CDBCacheManager::GetInstance()

} // namespace server
} // namespace wyd

#endif // DBCACHEMANAGER_H

} // namespace wydbr
