/**
 * @file ultra_resource_manager.h
 * @brief Sistema ultra-otimizado de gerenciamento de recursos gráficos
 *
 * Implementa um sistema avançado de gerenciamento de recursos que utiliza
 * técnicas de pool, streaming, compartilhamento e compressão para minimizar
 * o uso de memória e maximizar performance em hardwares limitados.
 */

#ifndef WYDBR_ULTRA_RESOURCE_MANAGER_H
#define WYDBR_ULTRA_RESOURCE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <bitset>
#include <chrono>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "ultra_optimized_renderer.h" // Para flags e configurações

namespace wydbr {
namespace graphics {

/**
 * @brief Tipos de recursos gráficos
 */
enum class ResourceType {
    TEXTURE,            ///< Texturas (difusas, normais, specular, etc)
    MESH,               ///< Modelos 3D
    SHADER,             ///< Programas de shader
    ANIMATION,          ///< Dados de animação
    PARTICLE_SYSTEM,    ///< Sistema de partículas
    EFFECT,             ///< Efeitos visuais
    FONT,               ///< Fontes de texto
    SOUND,              ///< Recursos de áudio (gerenciados junto com gráficos)
    UI_ELEMENT,         ///< Elementos de UI
    MATERIAL,           ///< Materiais
    VERTEX_BUFFER,      ///< Buffers de vértices
    INDEX_BUFFER,       ///< Buffers de índices
    CONSTANT_BUFFER,    ///< Buffers de constantes
    RENDER_TARGET,      ///< Alvos de renderização
    SKELETON,           ///< Esqueletos para animação
    TERRAIN_CHUNK,      ///< Chunks de terreno
    COMPOSITE           ///< Recursos compostos (contendo múltiplos tipos)
};

/**
 * @brief Prioridade de carga para recursos
 */
enum class LoadPriority {
    CRITICAL,           ///< Essencial, bloqueia thread até carregar
    HIGH,               ///< Alta prioridade, carrega assim que possível
    NORMAL,             ///< Prioridade normal
    LOW,                ///< Baixa prioridade, pode ser adiado
    BACKGROUND          ///< Carrega apenas quando sistema estiver ocioso
};

/**
 * @brief Estratégia de compressão de recursos
 */
enum class ResourceCompressionMode {
    NONE,                 ///< Sem compressão
    LOSSLESS,             ///< Compressão sem perda de qualidade
    OPTIMIZED,            ///< Equilíbrio entre qualidade e tamanho
    AGGRESSIVE,           ///< Compressão agressiva para economia de memória
    ADAPTIVE              ///< Adapta nível de compressão baseado em prioridade e visibilidade
};

/**
 * @brief Política de cache para recursos
 */
enum class CachePolicy {
    ALWAYS_KEEP,          ///< Nunca descarrega da memória
    KEEP_UNTIL_LOW_MEM,   ///< Mantém na memória até precisar de espaço
    KEEP_IF_USED_RECENTLY,///< Mantém se usado recentemente
    KEEP_IF_VISIBLE,      ///< Mantém apenas se visível
    LOAD_ON_DEMAND        ///< Carrega apenas quando necessário
};

/**
 * @brief Nível de detalhe (LOD) de um recurso
 */
enum class LODLevel {
    ULTRA,                ///< Qualidade máxima
    HIGH,                 ///< Alta qualidade
    MEDIUM,               ///< Qualidade média
    LOW,                  ///< Baixa qualidade
    MINIMUM               ///< Qualidade mínima
};

/**
 * @brief Política de streaming para recursos
 */
enum class StreamingPolicy {
    STREAM_FULL,          ///< Carrega recurso completo de uma vez
    STREAM_MIPMAP,        ///< Carrega mipmaps de texturas progressivamente
    STREAM_MESH_PARTS,    ///< Carrega partes de mesh progressivamente
    STREAM_LOD,           ///< Carrega LODs progressivamente
    HYBRID                ///< Adapta-se com base no tipo de recurso
};

/**
 * @brief Flags para controle fino de recursos
 */
enum ResourceFlags : uint32_t {
    NONE                   = 0,
    NO_MIPMAP              = 1 << 0,  ///< Não gerar mipmaps
    FORCE_POWER_OF_TWO     = 1 << 1,  ///< Força redimensionar para potência de 2
    ALLOW_COMPRESSION      = 1 << 2,  ///< Permite compressão
    KEEP_CPU_COPY          = 1 << 3,  ///< Mantém cópia na CPU após upload
    SHARABLE               = 1 << 4,  ///< Pode ser compartilhado entre instâncias
    STREAMABLE             = 1 << 5,  ///< Pode ser carregado progressivamente
    CACHE_PERSISTENTLY     = 1 << 6,  ///< Mantém em cache persistente
    DYNAMIC_UPDATE         = 1 << 7,  ///< Será atualizado frequentemente
    COMPUTE_DERIVATIVES    = 1 << 8,  ///< Computar mapas derivados (normais, etc)
    USE_COMPRESSED_FORMAT  = 1 << 9,  ///< Usar formato comprimido (ex: DXT)
    ALLOW_ASYNC_LOAD       = 1 << 10, ///< Permitir carregamento assíncrono
    PREFER_VRAM            = 1 << 11, ///< Preferência por armazenar na VRAM
    ALLOW_ATLAS_PACKING    = 1 << 12, ///< Pode ser empacotado em atlas
    USE_MESH_OPTIMIZATION  = 1 << 13, ///< Otimizar mesh automaticamente
    CONVERT_LEGACY_FORMAT  = 1 << 14, ///< Converter formatos legados automaticamente
    INSTANT_UPLOAD         = 1 << 15, ///< Upload imediato para GPU após carregamento
    LOW_PRECISION_FORMAT   = 1 << 16, ///< Usar formato de menor precisão quando possível
    ALLOW_PRECOMPUTE       = 1 << 17, ///< Permitir pré-cálculos para otimização
    SHARED_RESOURCE        = 1 << 18, ///< Recurso compartilhado entre múltiplos objetos
    VIRTUALIZED            = 1 << 19, ///< Usar paginação virtual para grande recurso
    NO_GARBAGE_COLLECT     = 1 << 20, ///< Não coletar em coleta de lixo
    HOT_RELOADABLE         = 1 << 21, ///< Pode ser recarregado sem reiniciar
    PRESERVE_ACROSS_ZONES  = 1 << 22, ///< Preservar ao trocar zonas de jogo
    SERIALIZE_WITH_SCENE   = 1 << 23, ///< Serializar junto com cena
    ALLOW_FALLBACK         = 1 << 24, ///< Permitir versão de fallback
    PREFER_INSTANCING      = 1 << 25, ///< Preferir uso de instancing
    BACKGROUND_PROCESS     = 1 << 26, ///< Processar em thread em segundo plano
    MEMORY_POOL_ALLOCATION = 1 << 27, ///< Alocar de pool de memória especializado
    GENERATED_CONTENT      = 1 << 28, ///< Conteúdo gerado, não carregado
    COMPOSITE_RESOURCE     = 1 << 29, ///< Recurso composto de múltiplos sub-recursos
    CRITICAL_PATH          = 1 << 30, ///< No caminho crítico de renderização
    EXPERIMENTAL           = 1 << 31  ///< Recurso experimental/em desenvolvimento
};

/**
 * @brief Status de carregamento de um recurso
 */
enum class LoadStatus {
    UNLOADED,            ///< Não carregado
    PENDING,             ///< Na fila para carregamento
    LOADING,             ///< Carregando
    PROCESSING,          ///< Processando após carga (compressão, otimização)
    READY,               ///< Pronto para uso
    STREAMING,           ///< Streaming progressivo em andamento
    FAILED,              ///< Falha no carregamento
    PLACEHOLDER          ///< Usando placeholder temporário
};

/**
 * @brief Identificador único de recurso
 */
class ResourceId {
public:
    ResourceId() : m_id(0) {}
    explicit ResourceId(uint64_t id) : m_id(id) {}
    explicit ResourceId(const std::string& path);
    
    uint64_t getValue() const { return m_id; }
    std::string toString() const;
    bool isValid() const { return m_id != 0; }
    
    bool operator==(const ResourceId& other) const { return m_id == other.m_id; }
    bool operator!=(const ResourceId& other) const { return m_id != other.m_id; }
    bool operator<(const ResourceId& other) const { return m_id < other.m_id; }
    
private:
    uint64_t m_id;
};

/**
 * @brief Metadados sobre um recurso
 */
struct ResourceMetadata {
    std::string path;                    ///< Caminho original do recurso
    ResourceType type;                   ///< Tipo do recurso
    size_t originalSize;                 ///< Tamanho original em bytes
    size_t compressedSize;               ///< Tamanho após compressão
    uint32_t width;                      ///< Largura (para texturas)
    uint32_t height;                     ///< Altura (para texturas)
    uint32_t depth;                      ///< Profundidade (para texturas 3D)
    uint32_t mipLevels;                  ///< Níveis de mipmap
    uint32_t arraySize;                  ///< Tamanho do array (para arrays de textura)
    uint32_t vertexCount;                ///< Número de vértices (para meshes)
    uint32_t triangleCount;              ///< Número de triângulos (para meshes)
    uint32_t submeshCount;               ///< Número de submeshes
    std::string format;                  ///< Formato interno
    std::chrono::system_clock::time_point lastModified; ///< Data da última modificação
    std::chrono::system_clock::time_point lastAccessed; ///< Data do último acesso
    uint32_t loadCount;                  ///< Número de vezes que foi carregado
    float loadTimeMs;                    ///< Tempo médio de carregamento (ms)
    float importanceScore;               ///< Score de importância (para priorização)
    std::unordered_map<std::string, std::string> tags; ///< Tags arbitrárias
    std::vector<ResourceId> dependencies; ///< Dependências deste recurso
};

/**
 * @brief Estado atual de um recurso
 */
struct ResourceState {
    ResourceId id;                       ///< ID do recurso
    LoadStatus status;                   ///< Status de carregamento
    size_t memoryUsage;                  ///< Uso de memória atual
    bool isResident;                     ///< Se está residente na memória
    bool isUploaded;                     ///< Se está carregado na GPU
    float loadProgress;                  ///< Progresso de carregamento (0.0 - 1.0)
    LODLevel currentLOD;                 ///< Nível de LOD atual
    std::chrono::system_clock::time_point lastUsed; ///< Última vez que foi usado
    uint32_t frameLastUsed;              ///< Frame em que foi usado pela última vez
    std::string error;                   ///< Mensagem de erro (se houver)
    bool isPlaceholder;                  ///< Se é um placeholder
    float distance;                      ///< Distância do espectador
    bool isVisible;                      ///< Se está atualmente visível
    uint32_t referenceCount;             ///< Contagem de referências
    bool isPinned;                       ///< Se está "fixado" (não pode ser descarregado)
};

/**
 * @brief Dados e metadados completos de um recurso
 */
struct ResourceDescriptor {
    ResourceId id;                         ///< ID único
    ResourceMetadata metadata;             ///< Metadados
    ResourceState state;                   ///< Estado atual
    ResourceCompressionMode compressionMode; ///< Modo de compressão
    CachePolicy cachePolicy;               ///< Política de cache
    LoadPriority priority;                 ///< Prioridade de carregamento
    StreamingPolicy streamingPolicy;       ///< Política de streaming
    uint32_t flags;                        ///< Flags de controle
    std::string category;                  ///< Categoria (para agrupamento)
    std::string variant;                   ///< Variante do recurso (ex: "winter", "summer")
    std::vector<LODLevel> availableLODs;   ///< LODs disponíveis
    std::unordered_map<std::string, std::string> parameters; ///< Parâmetros customizados
};

/**
 * @brief Callback para eventos de recursos
 */
using ResourceEventCallback = std::function<void(const ResourceId&, LoadStatus, float)>;

/**
 * @brief Política de substituição para cache
 */
enum class EvictionPolicy {
    LRU,                    ///< Menos recentemente usado
    LFU,                    ///< Menos frequentemente usado
    SIZE_BASED,             ///< Baseado no tamanho (maiores primeiro)
    PRIORITY_BASED,         ///< Baseado na prioridade
    DISTANCE_BASED,         ///< Baseado na distância da câmera
    HYBRID                  ///< Combinação de fatores
};

/**
 * @brief Configuração do sistema de recursos
 */
struct ResourceManagerConfig {
    size_t texturePoolSizeMB;              ///< Tamanho do pool de texturas (MB)
    size_t meshPoolSizeMB;                 ///< Tamanho do pool de meshes (MB)
    size_t shaderPoolSizeMB;               ///< Tamanho do pool de shaders (MB)
    size_t miscPoolSizeMB;                 ///< Tamanho do pool para outros recursos (MB)
    bool enableVirtualMemory;              ///< Habilitar sistema de memória virtual
    size_t virtualAddressSpaceGB;          ///< Tamanho do espaço de endereçamento virtual (GB)
    size_t physicalBackingStorageMB;       ///< Tamanho do armazenamento físico de backing (MB)
    size_t pageSizeKB;                     ///< Tamanho da página para memória virtual (KB)
    size_t streamingBufferSizeMB;          ///< Tamanho do buffer de streaming (MB)
    bool useCompression;                   ///< Habilitar compressão de recursos
    bool useAsyncLoading;                  ///< Habilitar carregamento assíncrono
    bool generateMipmaps;                  ///< Gerar mipmaps automaticamente
    bool optimizeMeshes;                   ///< Otimizar meshes automaticamente
    EvictionPolicy evictionPolicy;         ///< Política de substituição para cache
    float visibilityRadius;                ///< Raio de visibilidade para streaming (unidades)
    int maxConcurrentLoads;                ///< Número máximo de cargas concorrentes
    int preloadLevel;                      ///< Nível de pré-carregamento (0-5)
    bool usePersistentCache;               ///< Usar cache persistente em disco
    std::string diskCachePath;             ///< Caminho para cache em disco
    size_t diskCacheSizeMB;                ///< Tamanho máximo do cache em disco (MB)
    bool enableHotReload;                  ///< Permitir recarga a quente de recursos modificados
    bool useResourceSharing;               ///< Permitir compartilhamento de recursos
    bool preferReducedPrecision;           ///< Preferir formatos de precisão reduzida
    bool useTextureAtlases;                ///< Usar atlas de texturas
    int atlasSize;                         ///< Tamanho das texturas de atlas
    bool useNonPowerOfTwoOptimization;     ///< Otimizar texturas que não são potência de 2
    bool useMeshInstancing;                ///< Usar instancing para meshes repetidas
    bool useMemoryDefragmentation;         ///< Usar desfragmentação periódica de memória
    bool adaptiveQuality;                  ///< Ajustar qualidade baseado na distância
    int maxTextureSize;                    ///< Tamanho máximo de textura
    float lodBias;                         ///< Bias de LOD (-1.0 a 1.0)
    bool allowPlaceholders;                ///< Permitir placeholders para recursos não carregados
    bool aggressiveMemoryReclaim;          ///< Liberar memória agressivamente quando necessário
    
    /**
     * @brief Construtor com valores padrão
     */
    ResourceManagerConfig() :
        texturePoolSizeMB(128),
        meshPoolSizeMB(64),
        shaderPoolSizeMB(16),
        miscPoolSizeMB(32),
        enableVirtualMemory(true),
        virtualAddressSpaceGB(4),
        physicalBackingStorageMB(256),
        pageSizeKB(64),
        streamingBufferSizeMB(32),
        useCompression(true),
        useAsyncLoading(true),
        generateMipmaps(true),
        optimizeMeshes(true),
        evictionPolicy(EvictionPolicy::HYBRID),
        visibilityRadius(1000.0f),
        maxConcurrentLoads(8),
        preloadLevel(3),
        usePersistentCache(true),
        diskCachePath("cache/resources"),
        diskCacheSizeMB(1024),
        enableHotReload(true),
        useResourceSharing(true),
        preferReducedPrecision(false),
        useTextureAtlases(true),
        atlasSize(2048),
        useNonPowerOfTwoOptimization(true),
        useMeshInstancing(true),
        useMemoryDefragmentation(true),
        adaptiveQuality(true),
        maxTextureSize(2048),
        lodBias(0.0f),
        allowPlaceholders(true),
        aggressiveMemoryReclaim(false)
    {}
    
    /**
     * @brief Criar configuração para sistema de baixa memória
     */
    static ResourceManagerConfig createLowMemoryConfig() {
        ResourceManagerConfig config;
        config.texturePoolSizeMB = 64;
        config.meshPoolSizeMB = 32;
        config.shaderPoolSizeMB = 8;
        config.miscPoolSizeMB = 16;
        config.virtualAddressSpaceGB = 2;
        config.physicalBackingStorageMB = 128;
        config.streamingBufferSizeMB = 16;
        config.preferReducedPrecision = true;
        config.maxTextureSize = 1024;
        config.lodBias = -0.5f;
        config.aggressiveMemoryReclaim = true;
        config.maxConcurrentLoads = 4;
        config.preloadLevel = 2;
        return config;
    }
    
    /**
     * @brief Criar configuração para sistema absolutamente mínimo
     */
    static ResourceManagerConfig createUltraLowMemoryConfig() {
        ResourceManagerConfig config;
        config.texturePoolSizeMB = 32;
        config.meshPoolSizeMB = 16;
        config.shaderPoolSizeMB = 4;
        config.miscPoolSizeMB = 8;
        config.virtualAddressSpaceGB = 1;
        config.physicalBackingStorageMB = 64;
        config.streamingBufferSizeMB = 8;
        config.preferReducedPrecision = true;
        config.maxTextureSize = 512;
        config.lodBias = -1.0f;
        config.aggressiveMemoryReclaim = true;
        config.maxConcurrentLoads = 2;
        config.preloadLevel = 1;
        config.visibilityRadius = 500.0f;
        return config;
    }
};

/**
 * @brief Estatísticas do gerenciador de recursos
 */
struct ResourceManagerStats {
    size_t totalMemoryUsage;              ///< Uso total de memória
    size_t vramUsage;                     ///< Uso de VRAM
    size_t ramUsage;                      ///< Uso de RAM
    size_t diskCacheUsage;                ///< Uso de cache em disco
    uint32_t loadedResources;             ///< Total de recursos carregados
    uint32_t pendingResources;            ///< Recursos na fila de carregamento
    uint32_t streamingResources;          ///< Recursos em streaming
    uint32_t cachedResources;             ///< Recursos em cache
    uint32_t failedResources;             ///< Recursos com falha de carregamento
    uint32_t evictedResources;            ///< Recursos descarregados por pressão de memória
    uint32_t sharedResources;             ///< Recursos compartilhados
    float loadTimeAvgMs;                  ///< Tempo médio de carregamento
    float processTimeAvgMs;               ///< Tempo médio de processamento
    float uploadTimeAvgMs;                ///< Tempo médio de upload para GPU
    uint32_t poolFragmentation;           ///< Fragmentação do pool de memória (%)
    float cacheHitRate;                   ///< Taxa de acerto no cache
    uint32_t resourcesPerType[17];        ///< Recursos por tipo
    uint32_t asyncOperationsQueued;       ///< Operações assíncronas na fila
    uint32_t asyncOperationsProcessing;   ///< Operações assíncronas em processamento
    uint32_t framesSinceLastDefrag;       ///< Frames desde última desfragmentação
    size_t totalReadBytes;                ///< Total de bytes lidos do armazenamento
    float diskReadThroughputMBs;          ///< Taxa de leitura do disco (MB/s)
    float streamingThroughputMBs;         ///< Taxa de streaming (MB/s)
    float compressionRatio;               ///< Taxa média de compressão
    uint32_t placeholdersActive;          ///< Placeholders ativos
    uint32_t resourcesAtMaxLOD;           ///< Recursos no LOD máximo
    uint32_t resourcesAtMinLOD;           ///< Recursos no LOD mínimo
    uint32_t atlasesPacked;               ///< Número de atlas empacotados
    float highestMemoryPressure;          ///< Maior pressão de memória (0.0-1.0)
    float currentMemoryPressure;          ///< Pressão atual de memória (0.0-1.0)
};

/**
 * @class UltraResourceManager
 * @brief Gerenciador avançado de recursos gráficos para sistemas limitados
 */
class UltraResourceManager {
public:
    /**
     * @brief Obtém a instância do singleton
     */
    static UltraResourceManager& getInstance();
    
    /**
     * @brief Inicializa o sistema com configuração padrão
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Inicializa o sistema com configuração personalizada
     * @param config Configuração do gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize(const ResourceManagerConfig& config);
    
    /**
     * @brief Finaliza o sistema
     */
    void shutdown();
    
    /**
     * @brief Analisa o sistema e determina a configuração ideal
     * @param lowMemory Se o sistema deve usar menos memória
     * @return Configuração otimizada
     */
    ResourceManagerConfig createOptimalConfig(bool lowMemory = false);
    
    /**
     * @brief Carrega um recurso
     * @param path Caminho para o recurso
     * @param type Tipo de recurso
     * @param priority Prioridade de carregamento
     * @param flags Flags adicionais
     * @return ID do recurso
     */
    ResourceId loadResource(const std::string& path, 
                           ResourceType type, 
                           LoadPriority priority = LoadPriority::NORMAL,
                           uint32_t flags = 0);
    
    /**
     * @brief Carrega um recurso com descritor completo
     * @param descriptor Descritor completo do recurso
     * @return ID do recurso
     */
    ResourceId loadResource(const ResourceDescriptor& descriptor);
    
    /**
     * @brief Inicia carregamento assíncrono
     * @param path Caminho para o recurso
     * @param type Tipo de recurso
     * @param callback Função de callback para notificar conclusão
     * @param priority Prioridade de carregamento
     * @return ID do recurso
     */
    ResourceId loadResourceAsync(const std::string& path,
                                ResourceType type,
                                ResourceEventCallback callback,
                                LoadPriority priority = LoadPriority::NORMAL);
    
    /**
     * @brief Descarrega um recurso
     * @param resourceId ID do recurso
     * @param force Forçar descarregamento mesmo que ainda referenciado
     * @return true se descarregado com sucesso
     */
    bool unloadResource(const ResourceId& resourceId, bool force = false);
    
    /**
     * @brief Obtém estado atual de um recurso
     * @param resourceId ID do recurso
     * @return Estado atual
     */
    ResourceState getResourceState(const ResourceId& resourceId) const;
    
    /**
     * @brief Obtém metadados de um recurso
     * @param resourceId ID do recurso
     * @return Metadados do recurso
     */
    ResourceMetadata getResourceMetadata(const ResourceId& resourceId) const;
    
    /**
     * @brief Obtém descritor completo de um recurso
     * @param resourceId ID do recurso
     * @return Descritor completo
     */
    ResourceDescriptor getResourceDescriptor(const ResourceId& resourceId) const;
    
    /**
     * @brief Atualiza a prioridade de um recurso
     * @param resourceId ID do recurso
     * @param priority Nova prioridade
     */
    void updateResourcePriority(const ResourceId& resourceId, LoadPriority priority);
    
    /**
     * @brief Registra um callback para eventos de recursos
     * @param callback Função a ser chamada
     * @return ID do callback para remoção
     */
    int registerEventCallback(ResourceEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEventCallback(int callbackId);
    
    /**
     * @brief Pré-carrega recursos importantes
     * @param paths Caminhos para os recursos
     * @param priority Prioridade de carregamento
     */
    void preloadResources(const std::vector<std::string>& paths, LoadPriority priority = LoadPriority::HIGH);
    
    /**
     * @brief Define a posição do espectador para otimização
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param z Coordenada Z
     */
    void setViewerPosition(float x, float y, float z);
    
    /**
     * @brief Define a direção do olhar para otimização
     * @param dirX Componente X da direção
     * @param dirY Componente Y da direção
     * @param dirZ Componente Z da direção
     */
    void setViewDirection(float dirX, float dirY, float dirZ);
    
    /**
     * @brief Libera recursos não utilizados
     * @param aggressive Se verdadeiro, é mais agressivo na liberação
     */
    void purgeUnusedResources(bool aggressive = false);
    
    /**
     * @brief Desfragmenta o pool de memória
     */
    void defragmentMemory();
    
    /**
     * @brief Atualiza o sistema de recursos (deve ser chamado a cada frame)
     * @param deltaTimeMs Tempo desde último frame (ms)
     */
    void update(float deltaTimeMs);
    
    /**
     * @brief Obtém estatísticas do gerenciador
     * @return Estatísticas atuais
     */
    ResourceManagerStats getStats() const;
    
    /**
     * @brief Processa todas as operações pendentes
     * @param timeoutMs Tempo máximo para processar (0 = sem limite)
     * @return Número de operações processadas
     */
    int processQueue(uint32_t timeoutMs = 0);
    
    /**
     * @brief "Fixa" um recurso na memória (impede descarregamento)
     * @param resourceId ID do recurso
     */
    void pinResource(const ResourceId& resourceId);
    
    /**
     * @brief "Desfixa" um recurso (permite descarregamento normal)
     * @param resourceId ID do recurso
     */
    void unpinResource(const ResourceId& resourceId);
    
    /**
     * @brief Lista recursos carregados de um tipo específico
     * @param type Tipo de recurso
     * @return Lista de IDs de recursos
     */
    std::vector<ResourceId> listLoadedResources(ResourceType type) const;
    
    /**
     * @brief Define o nível de LOD para todos os recursos
     * @param level Nível de LOD global
     */
    void setGlobalLODLevel(LODLevel level);
    
    /**
     * @brief Define o nível de LOD para um recurso específico
     * @param resourceId ID do recurso
     * @param level Nível de LOD desejado
     */
    void setResourceLOD(const ResourceId& resourceId, LODLevel level);
    
    /**
     * @brief Notifica uso de um recurso
     * @param resourceId ID do recurso
     * @param wasVisible Se o recurso estava visível
     */
    void notifyResourceUsed(const ResourceId& resourceId, bool wasVisible = true);
    
    /**
     * @brief Configura o sistema para modo econômico
     * @param enabled Se o modo econômico está ativo
     */
    void setEconomyMode(bool enabled);
    
    /**
     * @brief Verifica se um recurso está carregado
     * @param resourceId ID do recurso
     * @return true se carregado e pronto para uso
     */
    bool isResourceLoaded(const ResourceId& resourceId) const;
    
    /**
     * @brief Obtém a configuração atual
     * @return Configuração atual
     */
    const ResourceManagerConfig& getCurrentConfig() const;
    
    /**
     * @brief Atualiza a configuração em runtime
     * @param config Nova configuração
     * @return true se atualizada com sucesso
     */
    bool updateConfig(const ResourceManagerConfig& config);
    
    /**
     * @brief Lista todos os recursos no sistema
     * @return Vetor com IDs de recursos
     */
    std::vector<ResourceId> listAllResources() const;
    
    /**
     * @brief Obtém recursos por tag
     * @param tag Tag para filtrar
     * @param value Valor da tag (opcional)
     * @return Lista de recursos que correspondem à tag
     */
    std::vector<ResourceId> getResourcesByTag(const std::string& tag, const std::string& value = "") const;
    
private:
    // Implementação do singleton
    UltraResourceManager();
    ~UltraResourceManager();
    UltraResourceManager(const UltraResourceManager&) = delete;
    UltraResourceManager& operator=(const UltraResourceManager&) = delete;
    
    // Implementação privada
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace graphics
} // namespace wydbr

#endif // WYDBR_ULTRA_RESOURCE_MANAGER_H