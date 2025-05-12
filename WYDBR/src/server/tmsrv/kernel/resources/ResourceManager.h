/**
 * ResourceManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/kernel/resources/ResourceManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>

#include "../../../../core/WYDTypes.h"
#include "../KernelTypes.h"
#include "../memory/MemoryManager.h"

namespace wydbr {


/**
 * @file ResourceManager.h
 * @brief Gerenciador de recursos dinâmicos
 * 
 * Este arquivo contém a definição do gerenciador de recursos dinâmicos,
 * que gerencia o carregamento, descarregamento e caching de recursos do jogo
 * usando estratégias avançadas de otimização de memória e disco.
 */

namespace wyd {
namespace server {
namespace kernel {
namespace resources {

/**
 * @brief Configuração do gerenciador de recursos
 */
struct ResourceManagerConfig {
    bool useResourceCaching;             // Usar cache de recursos
    bool useAsyncLoading;                // Usar carregamento assíncrono
    bool useCompression;                 // Usar compressão
    bool useHashing;                     // Usar hashing
    bool trackReferences;                // Rastrear referências
    bool validateIntegrity;              // Validar integridade
    bool usePreloading;                  // Usar pré-carregamento
    bool usePredictiveLoading;           // Usar carregamento preditivo
    size_t cacheSizeLimit;               // Limite de tamanho do cache
    CompressionAlgorithm compressionAlgorithm; // Algoritmo de compressão
    HashingAlgorithm hashingAlgorithm;   // Algoritmo de hashing
    DWORD asyncLoadingThreads;          // Threads de carregamento assíncrono
    std::string resourceBasePath;         // Caminho base dos recursos
    size_t lowMemoryThreshold;           // Limite de memória baixa
    
    ResourceManagerConfig()
        : useResourceCaching(true)
        , useAsyncLoading(true)
        , useCompression(true)
        , useHashing(true)
        , trackReferences(true)
        , validateIntegrity(true)
        , usePreloading(true)
        , usePredictiveLoading(true)
        , cacheSizeLimit(1024 * 1024 * 1024) // 1 GB
        , compressionAlgorithm(CompressionAlgorithm::ZSTD)
        , hashingAlgorithm(HashingAlgorithm::SHA256)
        , asyncLoadingThreads(4)
        , resourceBasePath("./resources")
        , lowMemoryThreshold(100 * 1024 * 1024) // 100 MB
    {
    }
};

/**
 * @brief Opções de carregamento de recurso
 */
struct ResourceLoadOptions {
    ResourceLoadPriority priority;       // Prioridade
    bool async;                           // Assíncrono
    bool compressed;                      // Comprimido
    bool cached;                          // Em cache
    bool forceReload;                     // Forçar recarregamento
    bool validateHash;                    // Validar hash
    CompressionAlgorithm compression;    // Algoritmo de compressão
    HashingAlgorithm hashing;            // Algoritmo de hashing
    DWORD timeout;                       // Timeout (ms)
    std::string customParameter;          // Parâmetro personalizado
    
    ResourceLoadOptions()
        : priority(ResourceLoadPriority::NORMAL)
        , async(true)
        , compressed(true)
        , cached(true)
        , forceReload(false)
        , validateHash(true)
        , compression(CompressionAlgorithm::ZSTD)
        , hashing(HashingAlgorithm::SHA256)
        , timeout(0)
        , customParameter("")
    {
    }
};

/**
 * @brief Configuração de grupo de recursos
 */
struct ResourceGroupConfig {
    std::string name;                     // Nome
    ResourceLoadStrategy loadStrategy;   // Estratégia de carregamento
    ResourceLoadPriority priority;       // Prioridade
    bool preload;                         // Pré-carregar
    bool keepLoaded;                      // Manter carregado
    std::string dependencies;             // Dependências
    
    ResourceGroupConfig()
        : name("")
        , loadStrategy(ResourceLoadStrategy::EAGER)
        , priority(ResourceLoadPriority::NORMAL)
        , preload(false)
        , keepLoaded(false)
        , dependencies("")
    {
    }
};

/**
 * @brief Resultado de carregamento de recurso
 */
struct ResourceLoadResult {
    DWORD resourceID;                    // ID do recurso
    ResourceStatus status;               // Status
    std::string error;                    // Erro
    DWORD loadTimeMs;                    // Tempo de carregamento (ms)
    size_t originalSize;                  // Tamanho original
    size_t loadedSize;                    // Tamanho carregado
    bool fromCache;                       // Se veio do cache
    
    ResourceLoadResult()
        : resourceID(0)
        , status(ResourceStatus::UNLOADED)
        , error("")
        , loadTimeMs(0)
        , originalSize(0)
        , loadedSize(0)
        , fromCache(false)
    {
    }
};

/**
 * @brief Callback de carregamento de recurso
 */
using ResourceLoadCallback = std::function<void(const ResourceLoadResult&)>;

/**
 * @brief Gerenciador de recursos
 */
/**
 * Classe WYDResourceManager
 * 
 * Esta classe implementa a funcionalidade WYDResourceManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ResourceManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static ResourceManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const ResourceManagerConfig& config);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Carrega um recurso
     * @param path Caminho
     * @param type Tipo
     * @param options Opções
     * @param callback Callback
     * @return ID do recurso, ou 0 se falhar
     */
    DWORD LoadResource(const std::string& path, ResourceType type, 
                     const ResourceLoadOptions& options = ResourceLoadOptions(),
                     ResourceLoadCallback callback = nullptr);
    
    /**
     * @brief Obtém um recurso
     * @param resourceID ID do recurso
     * @return Ponteiro para o recurso, ou nullptr se não existir
     */
    const Resource* GetResource(DWORD resourceID) const;
    
    /**
     * @brief Obtém um recurso pelo caminho
     * @param path Caminho
     * @return ID do recurso, ou 0 se não existir
     */
    DWORD GetResourceIDByPath(const std::string& path) const;
    
    /**
     * @brief Descarrega um recurso
     * @param resourceID ID do recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadResource(DWORD resourceID);
    
    /**
     * @brief Cria uma referência a um recurso
     * @param resourceID ID do recurso
     * @param ownerID ID do dono
     * @param context Contexto
     * @param isPersistent Se é persistente
     * @return ID da referência, ou 0 se falhar
     */
    DWORD CreateResourceReference(DWORD resourceID, DWORD ownerID, const std::string& context, bool isPersistent = false);
    
    /**
     * @brief Remove uma referência a um recurso
     * @param referenceID ID da referência
     * @return true se removido com sucesso
     */
    bool RemoveResourceReference(DWORD referenceID);
    
    /**
     * @brief Remove referências de um dono
     * @param ownerID ID do dono
     * @return Número de referências removidas
     */
    size_t RemoveOwnerReferences(DWORD ownerID);
    
    /**
     * @brief Cria um grupo de recursos
     * @param config Configuração
     * @return ID do grupo, ou 0 se falhar
     */
    DWORD CreateResourceGroup(const ResourceGroupConfig& config);
    
    /**
     * @brief Destrói um grupo de recursos
     * @param groupID ID do grupo
     * @return true se destruído com sucesso
     */
    bool DestroyResourceGroup(DWORD groupID);
    
    /**
     * @brief Adiciona um recurso a um grupo
     * @param groupID ID do grupo
     * @param resourceID ID do recurso
     * @return true se adicionado com sucesso
     */
    bool AddResourceToGroup(DWORD groupID, DWORD resourceID);
    
    /**
     * @brief Remove um recurso de um grupo
     * @param groupID ID do grupo
     * @param resourceID ID do recurso
     * @return true se removido com sucesso
     */
    bool RemoveResourceFromGroup(DWORD groupID, DWORD resourceID);
    
    /**
     * @brief Carrega um grupo de recursos
     * @param groupID ID do grupo
     * @param options Opções
     * @param callback Callback
     * @return true se carregado com sucesso
     */
    bool LoadResourceGroup(DWORD groupID, const ResourceLoadOptions& options = ResourceLoadOptions(),
                          ResourceLoadCallback callback = nullptr);
    
    /**
     * @brief Descarrega um grupo de recursos
     * @param groupID ID do grupo
     * @return true se descarregado com sucesso
     */
    bool UnloadResourceGroup(DWORD groupID);
    
    /**
     * @brief Obtém recursos por tipo
     * @param type Tipo
     * @return Lista de IDs de recursos
     */
    std::vector<DWORD> GetResourcesByType(ResourceType type) const;
    
    /**
     * @brief Obtém recursos por grupo
     * @param groupID ID do grupo
     * @return Lista de IDs de recursos
     */
    std::vector<DWORD> GetResourcesByGroup(DWORD groupID) const;
    
    /**
     * @brief Recarrega um recurso
     * @param resourceID ID do recurso
     * @param options Opções
     * @param callback Callback
     * @return true se recarregado com sucesso
     */
    bool ReloadResource(DWORD resourceID, const ResourceLoadOptions& options = ResourceLoadOptions(),
                       ResourceLoadCallback callback = nullptr);
    
    /**
     * @brief Obtém estatísticas de recursos
     * @return Estatísticas
     */
    ResourceStatistics GetStatistics() const;
    
    /**
     * @brief Libera o cache
     * @param threshold Limite (0 para liberar tudo)
     * @return Número de bytes liberados
     */
    size_t FreeCache(size_t threshold = 0);
    
    /**
     * @brief Registra um callback para eventos de recurso
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterEventCallback(ResourceEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterEventCallback(int callbackID);
    
    /**
     * @brief Obtém dados do recurso
     * @param resourceID ID do recurso
     * @return Ponteiro para os dados, ou nullptr se não existir
     */
    void* GetResourceData(DWORD resourceID);
    
    /**
     * @brief Obtém o tamanho do recurso
     * @param resourceID ID do recurso
     * @return Tamanho, ou 0 se não existir
     */
    size_t GetResourceSize(DWORD resourceID) const;
    
    /**
     * @brief Obtém o caminho do recurso
     * @param resourceID ID do recurso
     * @return Caminho, ou string vazia se não existir
     */
    std::string GetResourcePath(DWORD resourceID) const;
    
    /**
     * @brief Obtém referências por recurso
     * @param resourceID ID do recurso
     * @return Lista de IDs de referências
     */
    std::vector<DWORD> GetReferencesByResource(DWORD resourceID) const;
    
    /**
     * @brief Obtém referências por dono
     * @param ownerID ID do dono
     * @return Lista de IDs de referências
     */
    std::vector<DWORD> GetReferencesByOwner(DWORD ownerID) const;
    
    /**
     * @brief Verifica se um recurso existe
     * @param path Caminho
     * @return true se existir
     */
    bool ResourceExists(const std::string& path) const;
    
    /**
     * @brief Verifica se um recurso está carregado
     * @param resourceID ID do recurso
     * @return true se estiver carregado
     */
    bool IsResourceLoaded(DWORD resourceID) const;
    
    /**
     * @brief Faz prefetch de recursos
     * @param paths Lista de caminhos
     * @param priority Prioridade
     * @return Número de recursos em prefetch
     */
    size_t PrefetchResources(const std::vector<std::string>& paths, ResourceLoadPriority priority = ResourceLoadPriority::LOW);
    
    /**
     * @brief Mapeia um recurso na memória (mmap)
     * @param resourceID ID do recurso
     * @return Ponteiro para o mapeamento, ou nullptr se falhar
     */
    void* MapResourceToMemory(DWORD resourceID);
    
    /**
     * @brief Desmapeia um recurso da memória
     * @param resourceID ID do recurso
     * @return true se desmapeado com sucesso
     */
    bool UnmapResourceFromMemory(DWORD resourceID);
    
    /**
     * @brief Comprime dados
     * @param data Dados
     * @param size Tamanho
     * @param algorithm Algoritmo
     * @param compressedSize Tamanho comprimido (saída)
     * @return Ponteiro para os dados comprimidos, ou nullptr se falhar
     */
    void* CompressData(const void* data, size_t size, CompressionAlgorithm algorithm, size_t& compressedSize);
    
    /**
     * @brief Descomprime dados
     * @param data Dados
     * @param compressedSize Tamanho comprimido
     * @param algorithm Algoritmo
     * @param originalSize Tamanho original (saída)
     * @return Ponteiro para os dados descomprimidos, ou nullptr se falhar
     */
    void* DecompressData(const void* data, size_t compressedSize, CompressionAlgorithm algorithm, size_t& originalSize);
    
    /**
     * @brief Calcula hash de dados
     * @param data Dados
     * @param size Tamanho
     * @param algorithm Algoritmo
     * @return Hash, ou string vazia se falhar
     */
    std::string CalculateHash(const void* data, size_t size, HashingAlgorithm algorithm);
    
    /**
     * @brief Verifica hash de dados
     * @param data Dados
     * @param size Tamanho
     * @param expectedHash Hash esperado
     * @param algorithm Algoritmo
     * @return true se o hash for válido
     */
    bool VerifyHash(const void* data, size_t size, const std::string& expectedHash, HashingAlgorithm algorithm);
    
    /**
     * @brief Define o caminho base dos recursos
     * @param path Caminho
     */
    void SetResourceBasePath(const std::string& path);
    
    /**
     * @brief Obtém o caminho base dos recursos
     * @return Caminho
     */
    std::string GetResourceBasePath() const;
    
    /**
     * @brief Define o limite de tamanho do cache
     * @param sizeLimit Limite
     */
    void SetCacheSizeLimit(size_t sizeLimit);
    
    /**
     * @brief Obtém o limite de tamanho do cache
     * @return Limite
     */
    size_t GetCacheSizeLimit() const;
    
    /**
     * @brief Obtém o uso atual do cache
     * @return Uso
     */
    size_t GetCurrentCacheUsage() const;
    
    /**
     * @brief Atualiza a estratégia de carga de um grupo
     * @param groupID ID do grupo
     * @param strategy Estratégia
     * @return true se atualizado com sucesso
     */
    bool UpdateGroupLoadStrategy(DWORD groupID, ResourceLoadStrategy strategy);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    ResourceManager();
    
    /**
     * @brief Destrutor
     */
    ~ResourceManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    ResourceManager(const ResourceManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    /**
     * @brief Thread de carregamento assíncrono
     */
    void AsyncLoadThread();
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Carrega um recurso internamente
     * @param path Caminho
     * @param type Tipo
     * @param options Opções
     * @param callback Callback
     * @return Resultado do carregamento
     */
    ResourceLoadResult LoadResourceInternal(const std::string& path, ResourceType type, 
                                         const ResourceLoadOptions& options,
                                         ResourceLoadCallback callback);
    
    /**
     * @brief Atualiza a ordem de carregamento baseada em prioridade
     */
    void UpdateLoadingPriorities();
    
    /**
     * @brief Notifica callbacks de evento
     * @param eventType Tipo de evento
     * @param data Dados
     */
    void NotifyEventCallbacks(ResourceEventType eventType, const void* data);
    
    /**
     * @brief Gerencia recursos de baixa prioridade
     */
    void ManageLowPriorityResources();
    
    /**
     * @brief Processa a fila de carregamento
     */
    void ProcessLoadingQueue();
    
    /**
     * @brief Gerencia o ciclo de vida dos recursos
     */
    void ManageResourceLifecycle();
    
    /**
     * @brief Carrega um arquivo de recurso
     * @param path Caminho
     * @param outSize Tamanho (saída)
     * @return Ponteiro para os dados, ou nullptr se falhar
     */
    void* LoadResourceFile(const std::string& path, size_t& outSize);
    
    /**
     * @brief Adiciona ao cache
     * @param resourceID ID do recurso
     * @return true se adicionado com sucesso
     */
    bool AddToCache(DWORD resourceID);
    
    /**
     * @brief Remove do cache
     * @param resourceID ID do recurso
     * @return true se removido com sucesso
     */
    bool RemoveFromCache(DWORD resourceID);
    
    /**
     * @brief Obtém o tipo de recurso pelo caminho
     * @param path Caminho
     * @return Tipo
     */
    ResourceType GetResourceTypeFromPath(const std::string& path) const;
    
    /**
     * @brief Realiza carregamento preditivo
     */
    void PerformPredictiveLoading();
    
    // Configuração
    ResourceManagerConfig config_;
    
    // Recursos
    std::unordered_map<DWORD, Resource> resources_;
    mutable std::mutex resourcesMutex_;
    
    // Grupos de recursos
    std::unordered_map<DWORD, ResourceGroup> resourceGroups_;
    mutable std::mutex resourceGroupsMutex_;
    
    // Referências
    std::unordered_map<DWORD, ResourceReference> references_;
    mutable std::mutex referencesMutex_;
    
    // Índices para busca rápida
    std::unordered_map<std::string, DWORD> resourcePaths_;
    std::unordered_map<ResourceType, std::vector<DWORD>> resourcesByType_;
    std::unordered_map<DWORD, std::vector<DWORD>> resourcesByGroup_;
    std::unordered_map<DWORD, std::vector<DWORD>> referencesByResource_;
    std::unordered_map<DWORD, std::vector<DWORD>> referencesByOwner_;
    
    // Mapeamentos
    std::unordered_map<DWORD, void*> mmapHandles_;
    
    // Cache
    std::vector<DWORD> cacheList_;
    std::unordered_set<DWORD> cacheSet_;
    size_t currentCacheSize_;
    mutable std::mutex cacheMutex_;
    
    // Estatísticas
    ResourceStatistics statistics_;
    mutable std::mutex statisticsMutex_;
    
    // Callbacks para eventos
    std::map<int, ResourceEventCallback> eventCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contadores para novos IDs
    std::atomic<DWORD> nextResourceID_;
    std::atomic<DWORD> nextGroupID_;
    std::atomic<DWORD> nextReferenceID_;
    
    // Filas de carregamento
    struct LoadRequest {
        std::string path;
        ResourceType type;
        ResourceLoadOptions options;
        ResourceLoadCallback callback;
    };
    
    std::vector<LoadRequest> loadingQueue_;
    std::mutex loadingQueueMutex_;
    std::condition_variable loadingQueueCV_;
    
    // Threads e sincronização
    std::vector<std::thread> asyncLoadThreads_;
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Tempos
    time_t lastCacheCleanupTime_;
    time_t lastPredictiveLoadingTime_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_ResourceManager ResourceManager::GetInstance()

} // namespace resources
} // namespace kernel
} // namespace server
} // namespace wyd

#endif // RESOURCEMANAGER_H

} // namespace wydbr
