/**
 * KernelTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/kernel/KernelTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef KERNELTYPES_H
#define KERNELTYPES_H

#include <string>
#include <vector>
#include <map>
#include <array>
#include <bitset>
#include <chrono>
#include <ctime>
#include <functional>
#include <atomic>
#include <mutex>
#include <memory>

#include "../../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file KernelTypes.h
 * @brief Definições de tipos para o sistema de kernel e componentes base
 * 
 * Este arquivo contém as definições de tipos para o sistema de kernel avançado do WYD,
 * que corrige vários problemas de vazamento de memória, gerenciamento de recursos e
 * persistência do sistema original.
 */

namespace wyd {
namespace server {
namespace kernel {

// Constantes de sistema
constexpr size_t MAX_MEMORY_POOLS = 32;              // Máximo de pools de memória
constexpr size_t MAX_RESOURCE_GROUPS = 64;           // Máximo de grupos de recursos
constexpr size_t MAX_RESOURCE_LOADERS = 16;          // Máximo de carregadores de recursos
constexpr size_t MAX_RESOURCE_REFERENCES = 100000;   // Máximo de referências a recursos
constexpr size_t MAX_MEMORY_BLOCKS = 1000000;        // Máximo de blocos de memória
constexpr size_t MAX_PERSISTENCE_QUEUES = 16;        // Máximo de filas de persistência
constexpr size_t MAX_FILE_HANDLES = 1024;            // Máximo de handles de arquivo
constexpr size_t MAX_COMPRESSION_CONTEXTS = 32;      // Máximo de contextos de compressão
constexpr size_t MAX_ENCRYPTION_CONTEXTS = 32;       // Máximo de contextos de criptografia
constexpr size_t MAX_RESOURCE_CACHE_SIZE = 1024;     // Tamanho máximo do cache de recursos
constexpr size_t MAX_MEMORY_METRICS = 50;            // Máximo de métricas de memória
constexpr size_t MAX_MEMORY_HEAPS = 8;               // Máximo de heaps de memória
constexpr size_t MAX_PERSISTENCE_STRATEGIES = 10;    // Máximo de estratégias de persistência
constexpr size_t MAX_INSTRUMENTATION_CALLBACKS = 32; // Máximo de callbacks de instrumentação
constexpr size_t MAX_ASYNC_OPERATIONS = 10000;       // Máximo de operações assíncronas

/**
 * @brief Tipo de bloco de memória
 */
enum class MemoryBlockType : BYTE {
    GENERAL = 0,              // Geral
    PLAYER = 1,               // Jogador
    MOB = 2,                  // Mob
    ITEM = 3,                 // Item
    SKILL = 4,                // Skill
    EFFECT = 5,               // Efeito
    NETWORK = 6,              // Rede
    MAP = 7,                  // Mapa
    PATHFINDING = 8,          // Pathfinding
    DATABASE = 9,             // Banco de dados
    SCRIPTING = 10,           // Scripting
    RESOURCE = 11,            // Recurso
    GUILD = 12,               // Guild
    QUEST = 13,               // Quest
    MARKET = 14,              // Mercado
    CUSTOM = 15,              // Personalizado
};

/**
 * @brief Política de alocação de memória
 */
enum class MemoryAllocationPolicy : BYTE {
    FIRST_FIT = 0,            // Primeiro ajuste
    BEST_FIT = 1,             // Melhor ajuste
    WORST_FIT = 2,            // Pior ajuste
    NEXT_FIT = 3,             // Próximo ajuste
    BUDDY_SYSTEM = 4,         // Sistema de buddy
    SLAB = 5,                 // Slab
    POOL = 6,                 // Pool
    OBJECT_POOL = 7,          // Pool de objetos
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Tipo de recurso
 */
enum class ResourceType : BYTE {
    TEXTURE = 0,              // Textura
    MODEL = 1,                // Modelo
    SOUND = 2,                // Som
    MUSIC = 3,                // Música
    ANIMATION = 4,            // Animação
    MAP_DATA = 5,             // Dados de mapa
    SCRIPT = 6,               // Script
    CONFIG = 7,               // Configuração
    LOCALIZATION = 8,         // Localização
    EFFECT = 9,               // Efeito
    SHADER = 10,              // Shader
    UI = 11,                  // UI
    FONT = 12,                // Fonte
    DATABASE = 13,            // Banco de dados
    CUSTOM = 14,              // Personalizado
};

/**
 * @brief Estratégia de carregamento de recurso
 */
enum class ResourceLoadStrategy : BYTE {
    EAGER = 0,                // Ansioso
    LAZY = 1,                 // Preguiçoso
    STREAMING = 2,            // Streaming
    BACKGROUND = 3,           // Segundo plano
    PRIORITY_BASED = 4,       // Baseado em prioridade
    PREDICTIVE = 5,           // Preditivo
    DYNAMIC = 6,              // Dinâmico
    CUSTOM = 7,               // Personalizado
};

/**
 * @brief Status de recurso
 */
enum class ResourceStatus : BYTE {
    UNLOADED = 0,             // Não carregado
    LOADING = 1,              // Carregando
    LOADED = 2,               // Carregado
    FAILED = 3,               // Falhou
    UNLOADING = 4,            // Descarregando
    STREAMING = 5,            // Streaming
    CORRUPTED = 6,            // Corrompido
    CUSTOM = 7,               // Personalizado
};

/**
 * @brief Estratégia de persistência
 */
enum class PersistenceStrategy : BYTE {
    IMMEDIATE = 0,            // Imediato
    BATCHED = 1,              // Em lote
    JOURNALED = 2,            // Jornalizado
    DIFFERENTIAL = 3,         // Diferencial
    TRANSACTIONAL = 4,        // Transacional
    CHECKPOINTED = 5,         // Com checkpoint
    BACKGROUND = 6,           // Em segundo plano
    SCHEDULED = 7,            // Agendado
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Status de persistência
 */
enum class PersistenceStatus : BYTE {
    PENDING = 0,              // Pendente
    IN_PROGRESS = 1,          // Em progresso
    COMPLETED = 2,            // Completado
    FAILED = 3,               // Falhou
    QUEUED = 4,               // Na fila
    CANCELLED = 5,            // Cancelado
    RETRYING = 6,             // Repetindo
    CORRUPTED = 7,            // Corrompido
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Nível de criticidade de persistência
 */
enum class PersistenceCriticality : BYTE {
    LOW = 0,                  // Baixo
    MEDIUM = 1,               // Médio
    HIGH = 2,                 // Alto
    CRITICAL = 3,             // Crítico
    EMERGENCY = 4,            // Emergência
    CUSTOM = 5,               // Personalizado
};

/**
 * @brief Algoritmo de compressão
 */
enum class CompressionAlgorithm : BYTE {
    NONE = 0,                 // Nenhum
    ZLIB = 1,                 // zlib
    LZMA = 2,                 // LZMA
    LZ4 = 3,                  // LZ4
    ZSTD = 4,                 // Zstandard
    BROTLI = 5,               // Brotli
    CUSTOM = 6,               // Personalizado
};

/**
 * @brief Algoritmo de hashing
 */
enum class HashingAlgorithm : BYTE {
    NONE = 0,                 // Nenhum
    MD5 = 1,                  // MD5
    SHA1 = 2,                 // SHA-1
    SHA256 = 3,               // SHA-256
    SHA512 = 4,               // SHA-512
    XXHASH = 5,               // xxHash
    CUSTOM = 6,               // Personalizado
};

/**
 * @brief Algoritmo de criptografia
 */
enum class EncryptionAlgorithm : BYTE {
    NONE = 0,                 // Nenhum
    AES = 1,                  // AES
    BLOWFISH = 2,             // Blowfish
    TWOFISH = 3,              // Twofish
    RSA = 4,                  // RSA
    CHACHA20 = 5,             // ChaCha20
    CUSTOM = 6,               // Personalizado
};

/**
 * @brief Tipo de evento de memória
 */
enum class MemoryEventType : BYTE {
    ALLOCATION = 0,           // Alocação
    DEALLOCATION = 1,         // Desalocação
    REALLOCATION = 2,         // Realocação
    USAGE_THRESHOLD = 3,      // Limite de uso
    LEAK_DETECTED = 4,        // Vazamento detectado
    CORRUPTION = 5,           // Corrupção
    FRAGMENTATION = 6,        // Fragmentação
    CUSTOM = 7,               // Personalizado
};

/**
 * @brief Tipo de evento de recurso
 */
enum class ResourceEventType : BYTE {
    LOAD = 0,                 // Carregamento
    UNLOAD = 1,               // Descarregamento
    RELOAD = 2,               // Recarregamento
    UPDATE = 3,               // Atualização
    REFERENCE = 4,            // Referência
    DEREFERENCE = 5,          // Desreferência
    CACHE_HIT = 6,            // Acerto de cache
    CACHE_MISS = 7,           // Erro de cache
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Tipo de evento de persistência
 */
enum class PersistenceEventType : BYTE {
    SAVE = 0,                 // Salvar
    LOAD = 1,                 // Carregar
    BACKUP = 2,               // Backup
    RESTORE = 3,              // Restauração
    CHECKPOINT = 4,           // Checkpoint
    CORRUPTION = 5,           // Corrupção
    RECOVERY = 6,             // Recuperação
    ROLLBACK = 7,             // Rollback
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Prioridade de carregamento de recurso
 */
enum class ResourceLoadPriority : BYTE {
    LOWEST = 0,               // Mais baixa
    LOW = 1,                  // Baixa
    NORMAL = 2,               // Normal
    HIGH = 3,                 // Alta
    HIGHEST = 4,              // Mais alta
    CRITICAL = 5,             // Crítica
    CUSTOM = 6,               // Personalizada
};

/**
 * @brief Bloco de memória
 */
struct MemoryBlock {
    size_t blockID;                    // ID do bloco
    void* address;                      // Endereço
    size_t size;                        // Tamanho
    MemoryBlockType type;              // Tipo
    time_t allocationTime;              // Hora de alocação
    DWORD ownerID;                     // ID do dono
    std::string tag;                    // Tag
    DWORD stackTrace;                  // Stack trace
    bool isFreed;                       // Se está liberado
    DWORD threadID;                    // ID da thread
    
    MemoryBlock()
        : blockID(0)
        , address(nullptr)
        , size(0)
        , type(MemoryBlockType::GENERAL)
        , allocationTime(0)
        , ownerID(0)
        , tag("")
        , stackTrace(0)
        , isFreed(false)
        , threadID(0)
    {
    }
};

/**
 * @brief Pool de memória
 */
struct MemoryPool {
    size_t poolID;                     // ID da pool
    size_t blockSize;                   // Tamanho do bloco
    size_t blockCount;                  // Contagem de blocos
    size_t usedBlocks;                  // Blocos usados
    MemoryBlockType type;              // Tipo
    void* baseAddress;                  // Endereço base
    std::string name;                   // Nome
    bool expandable;                    // Se é expansível
    size_t maxBlocks;                   // Máximo de blocos
    
    MemoryPool()
        : poolID(0)
        , blockSize(0)
        , blockCount(0)
        , usedBlocks(0)
        , type(MemoryBlockType::GENERAL)
        , baseAddress(nullptr)
        , name("")
        , expandable(false)
        , maxBlocks(0)
    {
    }
};

/**
 * @brief Referência de recurso
 */
struct ResourceReference {
    DWORD referenceID;                // ID da referência
    DWORD resourceID;                 // ID do recurso
    DWORD ownerID;                    // ID do dono
    time_t referenceTime;              // Hora da referência
    bool isPersistent;                 // Se é persistente
    std::string context;               // Contexto
    
    ResourceReference()
        : referenceID(0)
        , resourceID(0)
        , ownerID(0)
        , referenceTime(0)
        , isPersistent(false)
        , context("")
    {
    }
};

/**
 * @brief Recurso
 */
struct Resource {
    DWORD resourceID;                 // ID do recurso
    std::string name;                  // Nome
    ResourceType type;                // Tipo
    ResourceStatus status;            // Status
    void* data;                        // Dados
    size_t size;                       // Tamanho
    time_t loadTime;                   // Hora de carregamento
    time_t lastAccessTime;             // Hora do último acesso
    DWORD referenceCount;             // Contagem de referências
    ResourceLoadPriority priority;    // Prioridade
    std::string sourcePath;            // Caminho de origem
    HashingAlgorithm hashAlgorithm;   // Algoritmo de hashing
    std::string hash;                  // Hash
    CompressionAlgorithm compression; // Algoritmo de compressão
    size_t compressedSize;             // Tamanho comprimido
    bool isModified;                   // Se está modificado
    
    Resource()
        : resourceID(0)
        , name("")
        , type(ResourceType::TEXTURE)
        , status(ResourceStatus::UNLOADED)
        , data(nullptr)
        , size(0)
        , loadTime(0)
        , lastAccessTime(0)
        , referenceCount(0)
        , priority(ResourceLoadPriority::NORMAL)
        , sourcePath("")
        , hashAlgorithm(HashingAlgorithm::NONE)
        , hash("")
        , compression(CompressionAlgorithm::NONE)
        , compressedSize(0)
        , isModified(false)
    {
    }
};

/**
 * @brief Grupo de recursos
 */
struct ResourceGroup {
    DWORD groupID;                    // ID do grupo
    std::string name;                  // Nome
    ResourceLoadStrategy loadStrategy; // Estratégia de carregamento
    bool isLoaded;                     // Se está carregado
    DWORD resourceCount;              // Contagem de recursos
    ResourceLoadPriority priority;    // Prioridade
    std::vector<DWORD> resources;      // IDs de recursos
    
    ResourceGroup()
        : groupID(0)
        , name("")
        , loadStrategy(ResourceLoadStrategy::EAGER)
        , isLoaded(false)
        , resourceCount(0)
        , priority(ResourceLoadPriority::NORMAL)
    {
    }
};

/**
 * @brief Operação de persistência
 */
struct PersistenceOperation {
    DWORD operationID;                // ID da operação
    PersistenceStrategy strategy;     // Estratégia
    PersistenceStatus status;         // Status
    PersistenceCriticality criticality; // Criticidade
    std::string targetPath;            // Caminho alvo
    void* data;                        // Dados
    size_t dataSize;                   // Tamanho dos dados
    time_t creationTime;               // Hora de criação
    time_t completionTime;             // Hora de conclusão
    DWORD retryCount;                 // Contagem de tentativas
    std::string error;                 // Erro
    
    PersistenceOperation()
        : operationID(0)
        , strategy(PersistenceStrategy::IMMEDIATE)
        , status(PersistenceStatus::PENDING)
        , criticality(PersistenceCriticality::MEDIUM)
        , targetPath("")
        , data(nullptr)
        , dataSize(0)
        , creationTime(0)
        , completionTime(0)
        , retryCount(0)
        , error("")
    {
    }
};

/**
 * @brief Estatísticas de memória
 */
struct MemoryStatistics {
    size_t totalAllocated;             // Total alocado
    size_t totalFreed;                 // Total liberado
    size_t currentUsage;               // Uso atual
    size_t peakUsage;                  // Uso de pico
    size_t allocationCount;            // Contagem de alocações
    size_t deallocationCount;          // Contagem de desalocações
    size_t failedAllocations;          // Alocações falhas
    float fragmentationRatio;          // Relação de fragmentação
    size_t largestFreeBlock;           // Maior bloco livre
    size_t smallestFreeBlock;          // Menor bloco livre
    
    MemoryStatistics()
        : totalAllocated(0)
        , totalFreed(0)
        , currentUsage(0)
        , peakUsage(0)
        , allocationCount(0)
        , deallocationCount(0)
        , failedAllocations(0)
        , fragmentationRatio(0.0f)
        , largestFreeBlock(0)
        , smallestFreeBlock(0)
    {
    }
};

/**
 * @brief Estatísticas de recursos
 */
struct ResourceStatistics {
    size_t totalResources;             // Total de recursos
    size_t loadedResources;            // Recursos carregados
    size_t failedResources;            // Recursos falhos
    size_t pendingResources;           // Recursos pendentes
    size_t totalSize;                  // Tamanho total
    size_t compressedSize;             // Tamanho comprimido
    float compressionRatio;            // Relação de compressão
    DWORD loadTime;                    // Tempo de carregamento
    DWORD unloadTime;                  // Tempo de descarregamento
    float cacheHitRatio;               // Relação de acerto de cache
    
    ResourceStatistics()
        : totalResources(0)
        , loadedResources(0)
        , failedResources(0)
        , pendingResources(0)
        , totalSize(0)
        , compressedSize(0)
        , compressionRatio(0.0f)
        , loadTime(0)
        , unloadTime(0)
        , cacheHitRatio(0.0f)
    {
    }
};

/**
 * @brief Estatísticas de persistência
 */
struct PersistenceStatistics {
    size_t totalOperations;            // Total de operações
    size_t successfulOperations;       // Operações bem-sucedidas
    size_t failedOperations;           // Operações falhas
    size_t pendingOperations;          // Operações pendentes
    DWORD averageSaveTime;            // Tempo médio de salvamento
    DWORD averageLoadTime;            // Tempo médio de carregamento
    size_t totalDataSaved;             // Total de dados salvos
    size_t totalDataLoaded;            // Total de dados carregados
    size_t corruptionCount;            // Contagem de corrupções
    size_t recoveryCount;              // Contagem de recuperações
    
    PersistenceStatistics()
        : totalOperations(0)
        , successfulOperations(0)
        , failedOperations(0)
        , pendingOperations(0)
        , averageSaveTime(0)
        , averageLoadTime(0)
        , totalDataSaved(0)
        , totalDataLoaded(0)
        , corruptionCount(0)
        , recoveryCount(0)
    {
    }
};

/**
 * @brief Vazamento de memória detectado
 */
struct MemoryLeak {
    size_t blockID;                    // ID do bloco
    MemoryBlockType type;              // Tipo
    size_t size;                        // Tamanho
    time_t allocationTime;              // Hora de alocação
    DWORD ownerID;                     // ID do dono
    std::string tag;                    // Tag
    DWORD stackTrace;                  // Stack trace
    DWORD threadID;                    // ID da thread
    
    MemoryLeak()
        : blockID(0)
        , type(MemoryBlockType::GENERAL)
        , size(0)
        , allocationTime(0)
        , ownerID(0)
        , tag("")
        , stackTrace(0)
        , threadID(0)
    {
    }
};

/**
 * @brief Evento assíncrono
 */
struct AsyncEvent {
    DWORD eventID;                     // ID do evento
    std::string eventType;              // Tipo de evento
    void* data;                         // Dados
    size_t dataSize;                    // Tamanho dos dados
    time_t timestamp;                   // Timestamp
    DWORD sourceID;                    // ID da fonte
    DWORD targetID;                    // ID do alvo
    int priority;                      // Prioridade
    bool processed;                     // Se foi processado
    
    AsyncEvent()
        : eventID(0)
        , eventType("")
        , data(nullptr)
        , dataSize(0)
        , timestamp(0)
        , sourceID(0)
        , targetID(0)
        , priority(0)
        , processed(false)
    {
    }
};

/**
 * @brief Configuração de instrumentação de memória
 */
struct MemoryInstrumentationConfig {
    bool trackAllocations;              // Rastrear alocações
    bool detectLeaks;                   // Detectar vazamentos
    bool trackStackTraces;              // Rastrear stack traces
    bool collectStatistics;             // Coletar estatísticas
    bool logEvents;                     // Registrar eventos
    size_t samplingRate;                // Taxa de amostragem
    bool alertOnThreshold;              // Alertar em limite
    size_t usageThreshold;              // Limite de uso
    bool detectCorruption;              // Detectar corrupção
    
    MemoryInstrumentationConfig()
        : trackAllocations(true)
        , detectLeaks(true)
        , trackStackTraces(true)
        , collectStatistics(true)
        , logEvents(true)
        , samplingRate(100)
        , alertOnThreshold(true)
        , usageThreshold(0)
        , detectCorruption(true)
    {
    }
};

/**
 * @brief Callback de eventos de memória
 */
using MemoryEventCallback = std::function<void(MemoryEventType, const void*)>;

/**
 * @brief Callback de eventos de recurso
 */
using ResourceEventCallback = std::function<void(ResourceEventType, const void*)>;

/**
 * @brief Callback de eventos de persistência
 */
using PersistenceEventCallback = std::function<void(PersistenceEventType, const void*)>;

} // namespace kernel
} // namespace server
} // namespace wyd

#endif // KERNELTYPES_H

} // namespace wydbr
