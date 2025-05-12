/**
 * @file ultra_optimized_renderer.h
 * @brief Sistema de renderização ultra-otimizado do WYDBR
 *
 * Sistema avançado de renderização que prioriza performance extrema mesmo em hardware limitado,
 * enquanto mantém capacidade para efeitos visuais modernos através de técnicas avançadas
 * de otimização e degradação inteligente.
 */

#ifndef WYDBR_ULTRA_OPTIMIZED_RENDERER_H
#define WYDBR_ULTRA_OPTIMIZED_RENDERER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <bitset>
#include <atomic>
#include <thread>

namespace wydbr {
namespace graphics {

/**
 * @brief APIs gráficas com foco em baixo consumo de recursos
 */
enum class LowLevelAPI {
    D3D9_FIXED,      ///< DirectX 9 com pipeline fixo (máxima compatibilidade)
    D3D9_SHADER,     ///< DirectX 9 com shaders simplificados 
    D3D11_BASIC,     ///< DirectX 11 com recursos básicos
    OPENGL_LEGACY,   ///< OpenGL 2.1/3.0 (compatibilidade ampla)
    OPENGL_ES,       ///< OpenGL ES 2.0/3.0 (modo low-end)
    VULKAN_LITE,     ///< Vulkan com otimizações para baixo consumo
    SOFTWARE_FAST    ///< Renderização por software altamente otimizada
};

/**
 * @brief Modos de otimização agressiva
 */
enum class OptimizationStrategy {
    BALANCED,        ///< Equilíbrio entre qualidade visual e performance
    PERFORMANCE,     ///< Prioriza FPS estável, reduzindo qualidade
    ULTRA_LOW,       ///< Modo extremo para hardware muito limitado
    MEMORY_SAVING,   ///< Foco em redução de consumo de memória
    BATTERY_SAVING,  ///< Modo para laptops, reduz consumo de energia
    DYNAMIC,         ///< Ajusta dinamicamente baseado na performance atual
    CUSTOM           ///< Configurações personalizadas pelo usuário
};

/**
 * @brief Modos de otimização de assets
 */
enum class AssetQuality {
    ORIGINAL,        ///< Assets originais sem modificação
    COMPRESSED,      ///< Compressão eficiente sem perda visual perceptível
    DOWNSCALED,      ///< Redução de resolução para economizar VRAM
    SIMPLIFIED,      ///< Geometria e texturas simplificadas
    MINIMAL          ///< Versão mais básica possível dos assets
};

/**
 * @brief Flags de otimização extrema (podem ser combinadas)
 */
enum OptimizationFlags : uint32_t {
    NONE                  = 0,
    DISABLE_FANCY_EFFECTS = 1 << 0,  ///< Desativa efeitos visuais avançados
    REDUCE_DRAW_DISTANCE  = 1 << 1,  ///< Reduz distância de renderização
    DISABLE_SHADOWS       = 1 << 2,  ///< Desativa sombras completamente
    SIMPLIFIED_LIGHTING   = 1 << 3,  ///< Usa modelo de iluminação simplificado
    LOWER_MODEL_DETAIL    = 1 << 4,  ///< Usa modelos com menos polígonos
    DISABLE_REFLECTIONS   = 1 << 5,  ///< Desativa reflexões em superfícies
    MINIMAL_PARTICLES     = 1 << 6,  ///< Reduz drasticamente sistema de partículas
    TEXTURE_REUSE         = 1 << 7,  ///< Reutiliza texturas para economizar memória
    BATCH_AGGRESSIVELY    = 1 << 8,  ///< Maximiza batching de draw calls
    SKIP_MINOR_UPDATES    = 1 << 9,  ///< Ignora atualizações visuais menores
    TEXTURE_COMPRESSION   = 1 << 10, ///< Usa compressão agressiva de texturas
    REDUCE_ANIMATION_FPS  = 1 << 11, ///< Reduz FPS das animações
    OCCLUSION_CULLING     = 1 << 12, ///< Ativa occlusion culling agressivo 
    USE_SPRITE_ATLASES    = 1 << 13, ///< Usa atlas de sprites para reduzir draw calls
    LIMIT_VISIBLE_PLAYERS = 1 << 14, ///< Limita número de jogadores renderizados
    PRECOMPUTE_LIGHTING   = 1 << 15, ///< Usa iluminação pré-computada quando possível
    USE_INSTANCING        = 1 << 16, ///< Usa instancing para objetos repetidos
    ASYNC_TEXTURE_LOADING = 1 << 17, ///< Carrega texturas assincronamente
    UNLOAD_DISTANT_AREAS  = 1 << 18, ///< Descarrega áreas distantes da memória
    DISABLE_VSYNC         = 1 << 19, ///< Desativa vsync para maximizar FPS
    MIPMAP_STREAMING      = 1 << 20, ///< Streaming de mipmaps para texturas
    TEXTURE_SHARING       = 1 << 21, ///< Compartilha texturas entre objetos similares
    MESH_LOD_SYSTEM       = 1 << 22, ///< Sistema dinâmico de LOD para meshes
    MEMORY_DEFRAG         = 1 << 23, ///< Desfragmentação periódica de memória
    THREADED_RENDERING    = 1 << 24, ///< Renderização multi-thread
    SHADER_SIMPLIFICATION = 1 << 25, ///< Usa shaders simplificados
    GPU_SKINNING          = 1 << 26, ///< Realiza skinning na GPU para personagens
    FRUSTUM_CULLING       = 1 << 27, ///< Culling agressivo de frustum
    FLUSH_GPU_MEMORY      = 1 << 28, ///< Libera regularmente recursos não usados
    DYNAMIC_RESOLUTION    = 1 << 29, ///< Ajusta resolução de renderização dinamicamente
    FRAME_PACER           = 1 << 30, ///< Sistema de pacing para estabilizar framerate
    PREDICTIVE_LOADING    = 1 << 31  ///< Precarregamento preditivo baseado em movimento
};

/**
 * @brief Configuração ultra-otimizada de renderização
 */
struct UltraOptimizedConfig {
    LowLevelAPI api;                         ///< API gráfica a ser usada
    OptimizationStrategy strategy;           ///< Estratégia principal de otimização
    AssetQuality assetQuality;               ///< Qualidade dos assets
    uint32_t optimizationFlags;              ///< Flags de otimização (combinação de OptimizationFlags)
    int targetFPS;                           ///< FPS alvo (0 = ilimitado)
    bool dynamicScaling;                     ///< Ativa scaling dinâmico de resolução
    float minScaleFactor;                    ///< Fator mínimo de escala (ex: 0.5 = 50% da resolução)
    int texturePoolSize;                     ///< Tamanho do pool de texturas em MB
    int geometryPoolSize;                    ///< Tamanho do pool de geometria em MB
    bool useMultithreading;                  ///< Usar renderização multi-thread
    int maxConcurrentThreads;                ///< Número máximo de threads para renderização
    bool aggressiveMemoryManagement;         ///< Gestão agressiva de memória
    bool intelligentCulling;                 ///< Sistema de culling avançado
    int maxDrawDistance;                     ///< Distância máxima de desenho
    int updateInterval;                      ///< Intervalo para atualização de objetos distantes (ms)
    int maxParticlesOnScreen;                ///< Número máximo de partículas simultaneamente
    int maxVisiblePlayers;                   ///< Número máximo de jogadores visíveis
    float lodBias;                           ///< Bias para sistema de LOD (-1.0 a 1.0)
    int baseResolutionWidth;                 ///< Largura base de resolução
    int baseResolutionHeight;                ///< Altura base de resolução
    bool adaptiveVerticalSync;               ///< VSync adaptativo
    int textureCompressionQuality;           ///< Qualidade da compressão de texturas (0-100)
    float shadowResolutionScale;             ///< Escala de resolução para sombras (0.0 - 1.0)
    bool useOcclusionQueries;                ///< Usar occlusion queries para otimização
    bool useMeshBatching;                    ///< Ativar batching de meshes
    float maxFrameTimeMs;                    ///< Tempo máximo permitido por frame (ms)
    int memoryDefragInterval;                ///< Intervalo para desfragmentação (em frames)
    bool prioritizeMainCharacter;            ///< Priorizar personagem principal na renderização
    
    /**
     * @brief Construtor com valores padrão otimizados para hardware limitado
     */
    UltraOptimizedConfig() :
        api(LowLevelAPI::D3D9_SHADER),
        strategy(OptimizationStrategy::BALANCED),
        assetQuality(AssetQuality::COMPRESSED),
        optimizationFlags(OCCLUSION_CULLING | USE_SPRITE_ATLASES | BATCH_AGGRESSIVELY | 
                         USE_INSTANCING | FRUSTUM_CULLING | TEXTURE_COMPRESSION),
        targetFPS(60),
        dynamicScaling(true),
        minScaleFactor(0.6f),
        texturePoolSize(128),
        geometryPoolSize(64),
        useMultithreading(true),
        maxConcurrentThreads(2),
        aggressiveMemoryManagement(true),
        intelligentCulling(true),
        maxDrawDistance(1000),
        updateInterval(500),
        maxParticlesOnScreen(1000),
        maxVisiblePlayers(50),
        lodBias(0.0f),
        baseResolutionWidth(1280),
        baseResolutionHeight(720),
        adaptiveVerticalSync(true),
        textureCompressionQuality(75),
        shadowResolutionScale(0.5f),
        useOcclusionQueries(true),
        useMeshBatching(true),
        maxFrameTimeMs(16.0f),
        memoryDefragInterval(300),
        prioritizeMainCharacter(true)
    {}
    
    /**
     * @brief Cria configuração predefinida para hardware muito limitado
     * @return Configuração ultra-otimizada para hardware muito fraco
     */
    static UltraOptimizedConfig createUltraLowEndConfig() {
        UltraOptimizedConfig config;
        config.api = LowLevelAPI::D3D9_FIXED;
        config.strategy = OptimizationStrategy::ULTRA_LOW;
        config.assetQuality = AssetQuality::MINIMAL;
        config.optimizationFlags = DISABLE_FANCY_EFFECTS | REDUCE_DRAW_DISTANCE | 
                                  DISABLE_SHADOWS | SIMPLIFIED_LIGHTING | 
                                  LOWER_MODEL_DETAIL | DISABLE_REFLECTIONS | 
                                  MINIMAL_PARTICLES | TEXTURE_REUSE |
                                  BATCH_AGGRESSIVELY | SKIP_MINOR_UPDATES | 
                                  TEXTURE_COMPRESSION | REDUCE_ANIMATION_FPS |
                                  OCCLUSION_CULLING | USE_SPRITE_ATLASES |
                                  LIMIT_VISIBLE_PLAYERS | UNLOAD_DISTANT_AREAS;
        config.targetFPS = 30;
        config.baseResolutionWidth = 800;
        config.baseResolutionHeight = 600;
        config.minScaleFactor = 0.5f;
        config.texturePoolSize = 64;
        config.geometryPoolSize = 32;
        config.useMultithreading = false;
        config.maxDrawDistance = 500;
        config.updateInterval = 1000;
        config.maxParticlesOnScreen = 100;
        config.maxVisiblePlayers = 15;
        config.shadowResolutionScale = 0.0f; // Desativa sombras
        config.textureCompressionQuality = 50;
        config.lodBias = -1.0f; // Usa sempre LODs mais baixos
        return config;
    }
};

/**
 * @brief Detalhes de desempenho interno para debugging e otimização
 */
struct PerformanceMetrics {
    float frameTime;                  ///< Tempo de processamento do frame (ms)
    float gpuTime;                    ///< Tempo gasto na GPU (ms)
    float cpuRenderTime;              ///< Tempo de CPU para renderização (ms)
    float cpuUpdateTime;              ///< Tempo de CPU para lógica/update (ms)
    float cpuWaitTime;                ///< Tempo de espera da CPU (ms)
    float memoryUsage;                ///< Uso de memória (MB)
    float vramUsage;                  ///< Uso de VRAM (MB)
    int drawCalls;                    ///< Número de draw calls por frame
    int triangleCount;                ///< Triângulos renderizados por frame
    int vertexCount;                  ///< Vértices processados por frame
    int textureSwaps;                 ///< Número de trocas de textura
    int shaderSwaps;                  ///< Número de trocas de shader
    int objectsRendered;              ///< Número de objetos renderizados
    int objectsCulled;                ///< Número de objetos removidos por culling
    int dynamicObjectsUpdated;        ///< Objetos dinâmicos atualizados
    float batchingEfficiency;         ///< Eficiência do batching (0.0-1.0)
    float cullingEfficiency;          ///< Eficiência do culling (0.0-1.0)
    int activeTextures;               ///< Texturas ativas na memória
    int cachedTextures;               ///< Texturas em cache
    int textureUploads;               ///< Uploads de textura neste frame
    int geometryUploads;              ///< Uploads de geometria neste frame
    float resourceLoadingTime;        ///< Tempo gasto carregando recursos (ms)
    float physicsTime;                ///< Tempo gasto em física (ms)
    float animationTime;              ///< Tempo gasto em animações (ms)
    float particleTime;               ///< Tempo gasto em sistemas de partículas (ms)
    float lightingTime;               ///< Tempo gasto calculando iluminação (ms)
    float uiRenderTime;               ///< Tempo gasto renderizando UI (ms)
    float worldRenderTime;            ///< Tempo gasto renderizando o mundo (ms)
    float characterRenderTime;        ///< Tempo gasto renderizando personagens (ms)
    float effectsRenderTime;          ///< Tempo gasto renderizando efeitos (ms)
    int frameNumber;                  ///< Número do frame atual
    float powerUsage;                 ///< Uso estimado de energia (watts)
    float thermalLoad;                ///< Carga térmica estimada (%)
    int threadUtilization;            ///< Utilização de threads (%)
};

/**
 * @brief Informações do sistema para diagnóstico
 */
struct SystemInfo {
    std::string cpuModel;
    int cpuCores;
    int cpuThreads;
    int ramTotalMB;
    int ramAvailableMB;
    std::string gpuModel;
    std::string gpuVendor;
    int gpuVRAMMB;
    std::string osVersion;
    int screenWidth;
    int screenHeight;
    int refreshRate;
    bool isLaptop;
    bool hasSSD;
    bool hasOnboardGPU;
    float batteryLevel;
    bool isBatteryCharging;
    bool isLowPowerMode;
    bool isThermallyThrottled;
    std::map<std::string, std::string> additionalInfo;
};

/**
 * @brief Callback para eventos de otimização
 */
typedef std::function<void(const PerformanceMetrics&, const UltraOptimizedConfig&)> OptimizationCallback;

/**
 * @class UltraOptimizedRenderer
 * @brief Sistema de renderização extremamente otimizado para hardware limitado
 */
class UltraOptimizedRenderer {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static UltraOptimizedRenderer& getInstance();
    
    /**
     * @brief Inicializa o sistema com detecção automática de hardware
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Inicializa o sistema com configuração específica
     * @param config Configuração de renderização
     * @return true se inicializado com sucesso
     */
    bool initialize(const UltraOptimizedConfig& config);
    
    /**
     * @brief Finaliza o sistema
     */
    void shutdown();
    
    /**
     * @brief Analisa o sistema e cria a configuração mais apropriada
     * @param fullAnalysis se verdadeiro, realiza análise completa (mais lenta)
     * @return Configuração ideal para o hardware atual
     */
    UltraOptimizedConfig analyzeSystemAndCreateOptimalConfig(bool fullAnalysis = true);
    
    /**
     * @brief Atualiza a configuração em tempo real
     * @param config Nova configuração
     * @param smoothTransition Se verdadeiro, transiciona suavemente para a nova configuração
     * @return true se atualizado com sucesso
     */
    bool updateConfiguration(const UltraOptimizedConfig& config, bool smoothTransition = true);
    
    /**
     * @brief Ajusta dinamicamente configurações baseado na performance atual
     * @param targetFPS FPS desejado
     * @param maxQualityReduction Redução máxima de qualidade permitida (0.0-1.0)
     */
    void enableDynamicOptimization(int targetFPS, float maxQualityReduction = 0.5f);
    
    /**
     * @brief Desativa otimização dinâmica
     */
    void disableDynamicOptimization();
    
    /**
     * @brief Ativa ou desativa modos de otimização específicos
     * @param flags Flags de otimização (combinação de OptimizationFlags)
     * @param enable Verdadeiro para ativar, falso para desativar
     */
    void setOptimizationFlags(uint32_t flags, bool enable);
    
    /**
     * @brief Verifica se uma flag de otimização está ativa
     * @param flag Flag a verificar
     * @return Verdadeiro se a flag estiver ativa
     */
    bool isOptimizationEnabled(OptimizationFlags flag) const;
    
    /**
     * @brief Obtém métricas de performance para análise
     * @return Métricas de performance atuais
     */
    PerformanceMetrics getPerformanceMetrics() const;
    
    /**
     * @brief Obtém informações do sistema
     * @return Informações do sistema detectadas
     */
    const SystemInfo& getSystemInfo() const;
    
    /**
     * @brief Registra um callback para ser notificado sobre otimizações
     * @param callback Função a ser chamada durante otimizações
     * @return ID do callback para remoção
     */
    int registerOptimizationCallback(OptimizationCallback callback);
    
    /**
     * @brief Remove um callback de otimização
     * @param callbackId ID do callback a remover
     * @return true se removido com sucesso
     */
    bool unregisterOptimizationCallback(int callbackId);
    
    /**
     * @brief Define configurações para um dispositivo específico
     * @param deviceId Identificador único do dispositivo
     * @param config Configuração otimizada para o dispositivo
     */
    void setDeviceSpecificConfig(const std::string& deviceId, const UltraOptimizedConfig& config);
    
    /**
     * @brief Salva configurações atuais em perfil
     * @param profileName Nome do perfil
     * @return true se salvo com sucesso
     */
    bool saveConfigurationProfile(const std::string& profileName);
    
    /**
     * @brief Carrega configurações de perfil
     * @param profileName Nome do perfil
     * @return true se carregado com sucesso
     */
    bool loadConfigurationProfile(const std::string& profileName);
    
    /**
     * @brief Obtém configuração atual
     * @return Configuração atual
     */
    const UltraOptimizedConfig& getCurrentConfig() const;
    
    /**
     * @brief Executa benchmark para determinar configurações ideais
     * @param duration Duração do benchmark em segundos
     * @param comprehensive Se verdadeiro, executa testes completos (mais lento)
     * @return Configuração recomendada com base no benchmark
     */
    UltraOptimizedConfig runBenchmark(int duration = 10, bool comprehensive = false);
    
    /**
     * @brief Inicializa sistema de alocação de memória otimizado
     * @param memorySizeMB Tamanho do pool de memória em MB (0 = automático)
     */
    void initializeOptimizedMemorySystem(int memorySizeMB = 0);
    
    /**
     * @brief Executa pré-carregamento otimizado de recursos
     * @param priority Prioridade dos recursos (1-5, 5 = mais alta)
     */
    void preloadEssentialResources(int priority = 3);
    
    /**
     * @brief Cria configuração equilibrada ideal para o perfil do sistema
     * @return Configuração otimizada
     */
    UltraOptimizedConfig createBalancedConfig();
    
    /**
     * @brief Define tamanho da área de streaming
     * @param radiusInUnits Raio ao redor do jogador para streaming (unidades de mundo)
     */
    void setStreamingRadius(float radiusInUnits);
    
    /**
     * @brief Purga recursos não utilizados da memória
     * @param aggressiveCleanup Se verdadeiro, limpa mais agressivamente
     */
    void purgeUnusedResources(bool aggressiveCleanup = false);
    
private:
    // Singleton
    UltraOptimizedRenderer();
    ~UltraOptimizedRenderer();
    UltraOptimizedRenderer(const UltraOptimizedRenderer&) = delete;
    UltraOptimizedRenderer& operator=(const UltraOptimizedRenderer&) = delete;
    
    // Implementação privada
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Funções auxiliares para gerenciamento de memória otimizado
namespace memory {
    /**
     * @brief Aloca memória do pool otimizado
     * @param sizeBytes Tamanho em bytes
     * @param alignment Alinhamento necessário
     * @param tag Tag para rastreamento
     * @return Ponteiro para memória alocada
     */
    void* allocateOptimized(size_t sizeBytes, size_t alignment = 16, const char* tag = nullptr);
    
    /**
     * @brief Libera memória do pool otimizado
     * @param ptr Ponteiro para memória a liberar
     */
    void freeOptimized(void* ptr);
    
    /**
     * @brief Inicializa subsistema de memória otimizado
     * @param sizeBytes Tamanho do pool em bytes
     */
    void initializeMemorySubsystem(size_t sizeBytes);
    
    /**
     * @brief Realiza desfragmentação do pool de memória
     */
    void defragmentMemory();
}

} // namespace graphics
} // namespace wydbr

#endif // WYDBR_ULTRA_OPTIMIZED_RENDERER_H