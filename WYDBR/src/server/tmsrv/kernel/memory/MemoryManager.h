/**
 * MemoryManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/kernel/memory/MemoryManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

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
#include <cstdlib>
#include <cassert>

#include "../../../../core/WYDTypes.h"
#include "../KernelTypes.h"

namespace wydbr {


/**
 * @file MemoryManager.h
 * @brief Gerenciador de memória otimizado
 * 
 * Este arquivo contém a definição do gerenciador de memória avançado,
 * que corrige diversos problemas de vazamento e fragmentação de memória
 * do sistema original do WYD. Implementa múltiplas técnicas de alocação,
 * detecção de vazamentos, e pools específicos por tipo de objeto.
 */

namespace wyd {
namespace server {
namespace kernel {
namespace memory {

/**
 * @brief Configuração do gerenciador de memória
 */
struct MemoryManagerConfig {
    bool trackAllocations;               // Rastrear alocações
    bool detectLeaks;                    // Detectar vazamentos
    bool usePools;                       // Usar pools
    bool useDefragmentation;             // Usar desfragmentação
    bool logActivity;                    // Registrar atividade
    bool trackStackTraces;               // Rastrear stack traces
    bool useThreadSafeAllocators;        // Usar alocadores thread-safe
    bool enableInstrumentation;          // Habilitar instrumentação
    size_t defaultPoolSize;              // Tamanho padrão de pool
    size_t defragmentationInterval;      // Intervalo de desfragmentação (ms)
    MemoryAllocationPolicy defaultPolicy; // Política padrão
    std::string logFile;                  // Arquivo de log
    float minFreeRatio;                   // Relação mínima livre
    
    MemoryManagerConfig()
        : trackAllocations(true)
        , detectLeaks(true)
        , usePools(true)
        , useDefragmentation(true)
        , logActivity(true)
        , trackStackTraces(true)
        , useThreadSafeAllocators(true)
        , enableInstrumentation(true)
        , defaultPoolSize(1024 * 1024)
        , defragmentationInterval(30000)
        , defaultPolicy(MemoryAllocationPolicy::POOL)
        , logFile("memory_manager.log")
        , minFreeRatio(0.2f)
    {
    }
};

/**
 * @brief Gerenciador de memória
 */
/**
 * Classe WYDMemoryManager
 * 
 * Esta classe implementa a funcionalidade WYDMemoryManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MemoryManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static MemoryManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const MemoryManagerConfig& config);
    
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
     * @brief Aloca memória
     * @param size Tamanho
     * @param type Tipo de bloco
     * @param tag Tag
     * @param ownerID ID do dono
     * @return Ponteiro para a memória alocada, ou nullptr se falhar
     */
    void* Allocate(size_t size, MemoryBlockType type = MemoryBlockType::GENERAL, 
                  const std::string& tag = "", DWORD ownerID = 0);
    
    /**
     * @brief Aloca memória alinhada
     * @param size Tamanho
     * @param alignment Alinhamento
     * @param type Tipo de bloco
     * @param tag Tag
     * @param ownerID ID do dono
     * @return Ponteiro para a memória alocada, ou nullptr se falhar
     */
    void* AllocateAligned(size_t size, size_t alignment, MemoryBlockType type = MemoryBlockType::GENERAL,
                         const std::string& tag = "", DWORD ownerID = 0);
    
    /**
     * @brief Realoca memória
     * @param ptr Ponteiro
     * @param newSize Novo tamanho
     * @return Ponteiro para a memória realocada, ou nullptr se falhar
     */
    void* Reallocate(void* ptr, size_t newSize);
    
    /**
     * @brief Libera memória
     * @param ptr Ponteiro
     * @return true se liberado com sucesso
     */
    bool Free(void* ptr);
    
    /**
     * @brief Cria uma pool de memória
     * @param blockSize Tamanho do bloco
     * @param blockCount Contagem de blocos
     * @param type Tipo de bloco
     * @param name Nome
     * @param expandable Se é expansível
     * @param maxBlocks Máximo de blocos
     * @return ID da pool, ou 0 se falhar
     */
    size_t CreateMemoryPool(size_t blockSize, size_t blockCount, MemoryBlockType type,
                         const std::string& name, bool expandable = true, size_t maxBlocks = 0);
    
    /**
     * @brief Destrói uma pool de memória
     * @param poolID ID da pool
     * @return true se destruído com sucesso
     */
    bool DestroyMemoryPool(size_t poolID);
    
    /**
     * @brief Aloca de uma pool específica
     * @param poolID ID da pool
     * @param tag Tag
     * @param ownerID ID do dono
     * @return Ponteiro para a memória alocada, ou nullptr se falhar
     */
    void* AllocateFromPool(size_t poolID, const std::string& tag = "", DWORD ownerID = 0);
    
    /**
     * @brief Libera para uma pool específica
     * @param ptr Ponteiro
     * @param poolID ID da pool
     * @return true se liberado com sucesso
     */
    bool FreeToPool(void* ptr, size_t poolID);
    
    /**
     * @brief Inicia um heap de memória
     * @param initialSize Tamanho inicial
     * @param maxSize Tamanho máximo
     * @param policy Política
     * @param name Nome
     * @return ID do heap, ou 0 se falhar
     */
    size_t CreateHeap(size_t initialSize, size_t maxSize, MemoryAllocationPolicy policy, const std::string& name);
    
    /**
     * @brief Destrói um heap de memória
     * @param heapID ID do heap
     * @return true se destruído com sucesso
     */
    bool DestroyHeap(size_t heapID);
    
    /**
     * @brief Aloca de um heap específico
     * @param heapID ID do heap
     * @param size Tamanho
     * @param tag Tag
     * @param ownerID ID do dono
     * @return Ponteiro para a memória alocada, ou nullptr se falhar
     */
    void* AllocateFromHeap(size_t heapID, size_t size, const std::string& tag = "", DWORD ownerID = 0);
    
    /**
     * @brief Libera para um heap específico
     * @param ptr Ponteiro
     * @param heapID ID do heap
     * @return true se liberado com sucesso
     */
    bool FreeToHeap(void* ptr, size_t heapID);
    
    /**
     * @brief Aloca objeto
     * @tparam T Tipo do objeto
     * @param args Argumentos para o construtor
     * @return Ponteiro para o objeto alocado
     */
    template<typename T, typename... Args>
    /**
 * AllocateObject
 * 
 * Implementa a funcionalidade AllocateObject conforme especificação original.
 * @param args Parâmetro args
 * @return Retorna T
 */

    T* AllocateObject(Args&&... args){
        void* memory = Allocate(sizeof(T), DetermineMemoryTypeFromClass<T>(), 
                             typeid(T).name(), 0);
        if (memory) {
            return new(memory) T(std::forward<Args>(args)...);
        }
        return nullptr;
    }
    
    /**
     * @brief Libera objeto
     * @tparam T Tipo do objeto
     * @param obj Ponteiro para o objeto
     */
    template<typename T>
    /**
 * FreeObject
 * 
 * Implementa a funcionalidade FreeObject conforme especificação original.
 * @param obj Parâmetro obj
 */

    void FreeObject(T* obj){
        if (obj) {
            obj->~T();
            Free(obj);
        }
    }
    
    /**
     * @brief Desfragmenta memória
     * @return Número de bytes recuperados
     */
    size_t Defragment();
    
    /**
     * @brief Detecta vazamentos de memória
     * @return Lista de vazamentos
     */
    std::vector<MemoryLeak> DetectLeaks();
    
    /**
     * @brief Corrige vazamentos de memória
     * @return Número de vazamentos corrigidos
     */
    size_t FixLeaks();
    
    /**
     * @brief Obtém estatísticas de memória
     * @return Estatísticas
     */
    MemoryStatistics GetStatistics() const;
    
    /**
     * @brief Registra um callback para eventos de memória
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterEventCallback(MemoryEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterEventCallback(int callbackID);
    
    /**
     * @brief Obtém o bloco de memória para um ponteiro
     * @param ptr Ponteiro
     * @return Ponteiro para o bloco, ou nullptr se não encontrado
     */
    const MemoryBlock* GetMemoryBlock(void* ptr) const;
    
    /**
     * @brief Obtém informações sobre um ponteiro
     * @param ptr Ponteiro
     * @param outType Tipo (saída)
     * @param outSize Tamanho (saída)
     * @param outOwnerID ID do dono (saída)
     * @return true se as informações foram obtidas com sucesso
     */
    bool GetPointerInfo(void* ptr, MemoryBlockType& outType, size_t& outSize, DWORD& outOwnerID);
    
    /**
     * @brief Obtém alocações por tipo
     * @param type Tipo
     * @return Lista de blocos
     */
    std::vector<const MemoryBlock*> GetAllocationsByType(MemoryBlockType type) const;
    
    /**
     * @brief Obtém alocações por dono
     * @param ownerID ID do dono
     * @return Lista de blocos
     */
    std::vector<const MemoryBlock*> GetAllocationsByOwner(DWORD ownerID) const;
    
    /**
     * @brief Obtém alocações por tag
     * @param tag Tag
     * @return Lista de blocos
     */
    std::vector<const MemoryBlock*> GetAllocationsByTag(const std::string& tag) const;
    
    /**
     * @brief Obtém a pool de memória para um tipo
     * @param type Tipo
     * @return ID da pool, ou 0 se não existir
     */
    size_t GetPoolForType(MemoryBlockType type) const;
    
    /**
     * @brief Verifica se um ponteiro é válido
     * @param ptr Ponteiro
     * @return true se for válido
     */
    bool IsValidPointer(void* ptr) const;
    
    /**
     * @brief Obtém o uso total de memória
     * @return Uso total
     */
    size_t GetTotalMemoryUsage() const;
    
    /**
     * @brief Obtém o uso de memória por tipo
     * @param type Tipo
     * @return Uso de memória
     */
    size_t GetMemoryUsageByType(MemoryBlockType type) const;
    
    /**
     * @brief Imprime informações de uso de memória
     * @param detailed Se deve ser detalhado
     */
    void PrintMemoryUsage(bool detailed = false);
    
    /**
     * @brief Gera um relatório de uso de memória
     * @param outputFile Arquivo de saída
     * @return true se gerado com sucesso
     */
    bool GenerateMemoryReport(const std::string& outputFile);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    MemoryManager();
    
    /**
     * @brief Destrutor
     */
    ~MemoryManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    MemoryManager(const MemoryManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Notifica callbacks de evento
     * @param eventType Tipo de evento
     * @param data Dados
     */
    void NotifyEventCallbacks(MemoryEventType eventType, const void* data);
    
    /**
     * @brief Rastreia uma alocação
     * @param ptr Ponteiro
     * @param size Tamanho
     * @param type Tipo
     * @param tag Tag
     * @param ownerID ID do dono
     * @return true se rastreado com sucesso
     */
    bool TrackAllocation(void* ptr, size_t size, MemoryBlockType type, const std::string& tag, DWORD ownerID);
    
    /**
     * @brief Rastreia uma desalocação
     * @param ptr Ponteiro
     * @return true se rastreado com sucesso
     */
    bool TrackDeallocation(void* ptr);
    
    /**
     * @brief Calcula o captura o stack trace atual
     * @return ID do stack trace
     */
    DWORD CaptureStackTrace();
    
    /**
     * @brief Determina o tipo de memória para uma classe
     * @tparam T Tipo da classe
     * @return Tipo de memória
     */
    template<typename T>
    /**
 * DetermineMemoryTypeFromClass
 * 
 * Implementa a funcionalidade DetermineMemoryTypeFromClass conforme especificação original.
 * @return Retorna MemoryBlockType
 */

    MemoryBlockType DetermineMemoryTypeFromClass(){
        // Exemplo: diferencia tipos comuns com base no nome da classe
        std::string typeName = typeid(T).name();
        
        if (typeName.find("Player") != std::string::npos) return MemoryBlockType::PLAYER;
        if (typeName.find("Mob") != std::string::npos) return MemoryBlockType::MOB;
        if (typeName.find("Item") != std::string::npos) return MemoryBlockType::ITEM;
        if (typeName.find("Skill") != std::string::npos) return MemoryBlockType::SKILL;
        if (typeName.find("Effect") != std::string::npos) return MemoryBlockType::EFFECT;
        if (typeName.find("Network") != std::string::npos) return MemoryBlockType::NETWORK;
        if (typeName.find("Map") != std::string::npos) return MemoryBlockType::MAP;
        if (typeName.find("Path") != std::string::npos) return MemoryBlockType::PATHFINDING;
        if (typeName.find("Database") != std::string::npos) return MemoryBlockType::DATABASE;
        if (typeName.find("Script") != std::string::npos) return MemoryBlockType::SCRIPTING;
        if (typeName.find("Resource") != std::string::npos) return MemoryBlockType::RESOURCE;
        if (typeName.find("Guild") != std::string::npos) return MemoryBlockType::GUILD;
        if (typeName.find("Quest") != std::string::npos) return MemoryBlockType::QUEST;
        if (typeName.find("Market") != std::string::npos) return MemoryBlockType::MARKET;
        
        return MemoryBlockType::GENERAL;
    }
    
    /**
     * @brief Verifica vazamentos periodicamente
     */
    void PeriodicLeakCheck();
    
    /**
     * @brief Verifica uso de memória
     */
    void CheckMemoryUsage();
    
    /**
     * @brief Encontra uma pool adequada para um tamanho
     * @param size Tamanho
     * @param type Tipo
     * @return ID da pool, ou 0 se não encontrar
     */
    size_t FindSuitablePool(size_t size, MemoryBlockType type);
    
    /**
     * @brief Inicializa pools padrão
     */
    void InitializeDefaultPools();
    
    /**
     * @brief Inicializa heaps padrão
     */
    void InitializeDefaultHeaps();
    
    /**
     * @brief Expande uma pool
     * @param poolID ID da pool
     * @param additionalBlocks Blocos adicionais
     * @return true se expandido com sucesso
     */
    bool ExpandPool(size_t poolID, size_t additionalBlocks);
    
    // Configuração
    MemoryManagerConfig config_;
    
    // Blocos de memória
    std::unordered_map<void*, MemoryBlock> memoryBlocks_;
    mutable std::mutex memoryBlocksMutex_;
    
    // Pools de memória
    std::map<size_t, MemoryPool> memoryPools_;
    mutable std::mutex memoryPoolsMutex_;
    
    // Mapeamento de tipo para pool
    std::map<MemoryBlockType, std::vector<size_t>> poolsByType_;
    
    // Heaps
    std::map<size_t, void*> heaps_;
    std::map<size_t, size_t> heapSizes_;
    std::map<void*, size_t> pointerToHeap_;
    mutable std::mutex heapsMutex_;
    
    // Estatísticas
    MemoryStatistics statistics_;
    mutable std::mutex statisticsMutex_;
    
    // Callbacks para eventos
    std::map<int, MemoryEventCallback> eventCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contadores para novos IDs
    std::atomic<size_t> nextBlockID_;
    std::atomic<size_t> nextPoolID_;
    std::atomic<size_t> nextHeapID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Tempos
    time_t lastDefragmentTime_;
    time_t lastLeakCheckTime_;
    time_t lastUsageCheckTime_;
    
    // Estado
    bool initialized_;
};

/**
 * @brief Delimitador de escopo de memória para rastreamento
 */
/**
 * Classe WYDMemoryScopeTracker
 * 
 * Esta classe implementa a funcionalidade WYDMemoryScopeTracker conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MemoryScopeTracker {
public:
    /**
     * @brief Construtor
     * @param tag Tag para alocações neste escopo
     * @param ownerID ID do dono para alocações neste escopo
     */
    MemoryScopeTracker(const std::string& tag, DWORD ownerID = 0);
    
    /**
     * @brief Destrutor
     */
    ~MemoryScopeTracker();
    
private:
    std::string previousTag_;     // Tag anterior
    DWORD previousOwnerID_;       // ID do dono anterior
};

/**
 * @brief Alocador personalizado WYD para uso com containeres STL
 */
template <typename T>
/**
 * Classe WYDAllocator
 * 
 * Esta classe implementa a funcionalidade WYDAllocator conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDAllocator {
public:
    using value_type = T;
    
    /**
     * @brief Construtor padrão
     */
    WYDAllocator() noexcept {}
    
    /**
     * @brief Construtor de cópia
     */
    template <typename U>
    WYDAllocator(const WYDAllocator<U>&) noexcept {}
    
    /**
     * @brief Aloca memória
     * @param n Número de elementos
     * @return Ponteiro para a memória alocada
     */
    T* allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
            throw std::bad_alloc();
        }
        
        if (auto p = static_cast<T*>(MemoryManager::GetInstance().Allocate(n * sizeof(T)))) {
            return p;
        }
        
        throw std::bad_alloc();
    }
    
    /**
     * @brief Desaloca memória
     * @param p Ponteiro para a memória
     * @param n Número de elementos
     */
    void deallocate(T* p, std::size_t n) noexcept {
        MemoryManager::GetInstance().Free(p);
    }
};

/**
 * @brief Comparação de alocadores
 */
template <class T, class U>
bool operator==(const WYDAllocator<T>&, const WYDAllocator<U>&) { return true; }

template <class T, class U>
bool operator!=(const WYDAllocator<T>&, const WYDAllocator<U>&) { return false; }

/**
 * @brief Smart pointer personalizado WYD
 */
template <typename T>
/**
 * Classe WYDUniquePtr
 * 
 * Esta classe implementa a funcionalidade WYDUniquePtr conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDUniquePtr {
public:
    /**
     * @brief Construtor padrão
     */
    WYDUniquePtr() /**
 * ptr_
 * 
 * Implementa a funcionalidade ptr_ conforme especificação original.
 * @param nullptr Parâmetro nullptr
 * @return Retorna :
 */
 : ptr_(nullptr){}
    
    /**
     * @brief Construtor com ponteiro
     */
    explicit WYDUniquePtr(T* ptr) /**
 * ptr_
 * 
 * Implementa a funcionalidade ptr_ conforme especificação original.
 * @param ptr Parâmetro ptr
 * @return Retorna :
 */
 : ptr_(ptr){}
    
    /**
     * @brief Construtor de movimentação
     */
    WYDUniquePtr(WYDUniquePtr&& other) noexcept /**
 * ptr_
 * 
 * Implementa a funcionalidade ptr_ conforme especificação original.
 * @param other.ptr_ Parâmetro other.ptr_
 * @return Retorna :
 */
 : ptr_(other.ptr_){
        other.ptr_ = nullptr;
    }
    
    /**
     * @brief Operador de atribuição de movimentação
     */
    WYDUniquePtr& operator=(WYDUniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    WYDUniquePtr(const WYDUniquePtr&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    WYDUniquePtr& operator=(const WYDUniquePtr&) = delete;
    
    /**
     * @brief Destrutor
     */
    ~WYDUniquePtr() {
        reset();
    }
    
    /**
     * @brief Operador de desreferência
     */
    T& operator*() const {
        return *ptr_;
    }
    
    /**
     * @brief Operador de acesso a membro
     */
    T* operator->() const {
        return ptr_;
    }
    
    /**
     * @brief Obtém o ponteiro
     */
    T* get() const {
        return ptr_;
    }
    
    /**
     * @brief Libera o ponteiro
     */
    void reset() {
        if (ptr_) {
            MemoryManager::GetInstance().FreeObject(ptr_);
            ptr_ = nullptr;
        }
    }
    
    /**
     * @brief Libera o ponteiro e atribui um novo
     */
    void reset(T* ptr) {
        reset();
        ptr_ = ptr;
    }
    
    /**
     * @brief Libera o ponteiro sem liberar o objeto
     */
    T* release() {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }
    
    /**
     * @brief Operador de conversão explícita para bool
     */
    explicit /**
 * bool
 * 
 * Implementa a funcionalidade bool conforme especificação original.
 * @return Retorna operator
 */
 operator bool() const{
        return ptr_ != nullptr;
    }
    
private:
    T* ptr_;
};

/**
 * @brief Cria um WYDUniquePtr
 */
template<typename T, typename... Args>
WYDUniquePtr<T> MakeUnique(Args&&... args) {
    return WYDUniquePtr<T>(MemoryManager::GetInstance().AllocateObject<T>(std::forward<Args>(args)...));
}

// Acesso global
#define g_MemoryManager MemoryManager::GetInstance()

} // namespace memory
} // namespace kernel
} // namespace server
} // namespace wyd

#endif // MEMORYMANAGER_H

} // namespace wydbr
