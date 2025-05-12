/**
 * @file advanced_effect_system.h
 * @brief Sistema avançado de efeitos visuais altamente otimizado
 *
 * Implementa um sistema capaz de renderizar efeitos visuais modernos
 * (partículas, pós-processamento, distorções) com uso mínimo de recursos,
 * utilizando técnicas avançadas de otimização para hardware limitado.
 */

#ifndef WYDBR_ADVANCED_EFFECT_SYSTEM_H
#define WYDBR_ADVANCED_EFFECT_SYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <bitset>
#include <atomic>

#include "../graphics/ultra_optimized_renderer.h"
#include "../graphics/ultra_resource_manager.h"

namespace wydbr {
namespace effects {

/**
 * @brief Tipos de efeitos suportados
 */
enum class EffectType {
    PARTICLE_SYSTEM,     ///< Sistema de partículas (fogo, fumaça, etc)
    DECAL,               ///< Decal (marcas no chão, sangue nas paredes, etc)
    POST_PROCESS,        ///< Efeito de pós-processamento
    VOLUMETRIC,          ///< Efeito volumétrico (névoa, raios, etc)
    DISTORTION,          ///< Distorção de tela (ondas de calor, etc)
    TRAIL,               ///< Rastros (de armas, movimentos, etc)
    LIGHTING,            ///< Efeitos de iluminação especial
    SCREEN_SPACE,        ///< Efeitos em espaço de tela (reflexões, sombras, etc)
    AMBIENT,             ///< Efeitos ambientais (chuva, neve, etc)
    BEAM,                ///< Feixes e raios
    PROCEDURAL,          ///< Efeitos gerados proceduralmente
    COMPOSITE            ///< Composto de múltiplos efeitos
};

/**
 * @brief Métodos de otimização para efeitos
 */
enum class EffectOptimizationMethod {
    NONE,                 ///< Sem otimização específica
    LOD_SCALING,          ///< Escala baseada em LOD
    DISTANCE_SCALING,     ///< Escala baseada em distância
    SCREEN_PERCENTAGE,    ///< Escala baseada em tamanho na tela
    IMPORTANCE_SCALING,   ///< Escala baseada em importância visual
    FPS_ADAPTIVE,         ///< Adapta-se baseado no FPS atual
    HYBRID_ADAPTIVE       ///< Combinação de métodos adaptativos
};

/**
 * @brief Qualidade dos efeitos
 */
enum class EffectQuality {
    ULTRA,               ///< Qualidade máxima, sem compromissos
    HIGH,                ///< Alta qualidade, pequenas otimizações
    MEDIUM,              ///< Qualidade média, bom equilíbrio
    LOW,                 ///< Qualidade baixa, foco em desempenho
    MINIMAL,             ///< Qualidade mínima, máximo desempenho
    ADAPTIVE             ///< Adapta-se automaticamente
};

/**
 * @brief Prioridade de efeito
 */
enum class EffectPriority {
    CRITICAL,            ///< Essencial para gameplay (sempre renderizado)
    HIGH,                ///< Alta prioridade (raramente removido)
    NORMAL,              ///< Prioridade normal
    LOW,                 ///< Baixa prioridade (primeiro a ser otimizado)
    COSMETIC             ///< Puramente cosmético (primeiro a ser desativado)
};

/**
 * @brief Flags de otimização para efeitos
 */
enum EffectOptimizationFlags : uint32_t {
    NONE                       = 0,
    REDUCE_PARTICLE_COUNT      = 1 << 0,   ///< Reduz quantidade de partículas
    SIMPLIFY_PHYSICS           = 1 << 1,   ///< Simplifica física de partículas
    REDUCE_TEXTURE_QUALITY     = 1 << 2,   ///< Reduz qualidade de texturas
    SKIP_MINOR_EFFECTS         = 1 << 3,   ///< Pula efeitos menores
    MERGE_SIMILAR_EFFECTS      = 1 << 4,   ///< Combina efeitos similares
    LIMIT_LIGHT_SOURCES        = 1 << 5,   ///< Limita fontes de luz por efeito
    USE_BILLBOARD_PARTICLES    = 1 << 6,   ///< Usa billboards em vez de meshes para partículas
    LIMIT_ACTIVE_DISTANCE      = 1 << 7,   ///< Limita distância de ativação
    DISABLE_MOTION_BLUR        = 1 << 8,   ///< Desativa motion blur em partículas
    REDUCE_OVERDRAW            = 1 << 9,   ///< Reduz overdraw/transparências
    FIX_SIMULATION_TIMESTEP    = 1 << 10,  ///< Usa timestep fixo para simulação
    SKIP_FRAMES                = 1 << 11,  ///< Pula frames na simulação
    REUSE_PARTICLES            = 1 << 12,  ///< Reutiliza partículas entre sistemas
    BATCH_SIMILAR_EFFECTS      = 1 << 13,  ///< Processa efeitos similares em batch
    DISABLE_GLOW               = 1 << 14,  ///< Desativa efeitos de brilho
    LIMIT_EFFECT_LIFESPAN      = 1 << 15,  ///< Limita tempo de vida de efeitos
    DISABLE_SHADOWS_FROM_FX    = 1 << 16,  ///< Desativa sombras de efeitos
    USE_LOW_RES_BUFFER         = 1 << 17,  ///< Renderiza em buffer de baixa resolução
    DISABLE_NORMAL_MAPS        = 1 << 18,  ///< Desativa normal maps em efeitos
    SIMPLIFIED_LIGHTING        = 1 << 19,  ///< Usa iluminação simplificada
    PRECOMPUTE_ANIMATIONS      = 1 << 20,  ///< Pré-calcula animações de partículas
    USE_SPRITE_SHEETS          = 1 << 21,  ///< Usa sprite sheets para animações
    LIMIT_MAX_INSTANCES        = 1 << 22,  ///< Limita número máximo de instâncias
    USE_LOD_VARIANTS           = 1 << 23,  ///< Usa variantes de LOD para efeitos
    CULL_TINY_EFFECTS          = 1 << 24,  ///< Remove efeitos muito pequenos na tela
    DISABLE_REFRACTION         = 1 << 25,  ///< Desativa efeitos de refração
    LIMIT_REFLECTIONS          = 1 << 26,  ///< Limita reflexões em efeitos
    USE_HALF_PRECISION         = 1 << 27,  ///< Usa cálculos em half precision
    DISABLE_VOLUMETRICS        = 1 << 28,  ///< Desativa efeitos volumétricos
    SIMPLIFIED_ALPHA           = 1 << 29,  ///< Usa teste alpha em vez de blend
    LIMIT_POST_PROCESSING      = 1 << 30,  ///< Limita passes de pós-processamento
    REUSE_RENDER_TARGETS       = 1 << 31   ///< Reutiliza render targets entre efeitos
};

/**
 * @brief Configurações para o sistema de efeitos
 */
struct EffectSystemConfig {
    EffectQuality globalQuality;              ///< Qualidade global padrão
    int maxActiveEffects;                     ///< Número máximo de efeitos ativos
    int maxParticlesTotal;                    ///< Número máximo de partículas no total
    int maxParticlesPerSystem;                ///< Máximo de partículas por sistema
    float visibilityDistance;                 ///< Distância de visibilidade para efeitos
    bool dynamicQualityAdjustment;            ///< Ajuste dinâmico de qualidade
    uint32_t optimizationFlags;               ///< Flags de otimização ativas
    float lodBias;                            ///< Bias para LOD de efeitos (-1.0 a 1.0)
    bool useInstancing;                       ///< Usar instancing para partículas
    int renderTargetScale;                    ///< Escala para render targets (%)
    bool useDeferredDecals;                   ///< Usar decals diferidos (mais eficiente)
    float particleSizeFactor;                 ///< Fator de escala para tamanho de partículas
    int maxLightsPerEffect;                   ///< Máximo de luzes por efeito
    float simulationTimeStep;                 ///< Passo de tempo para simulação
    bool useGPUSimulation;                    ///< Usar GPU para simulação de partículas
    float cullThresholdSize;                  ///< Tamanho mínimo na tela para renderizar (%)
    int maxCascadesPerEffect;                 ///< Máximo de cascades por efeito
    bool useTextureAtlas;                     ///< Usar atlas de texturas para partículas
    bool mergeDrawCalls;                      ///< Mesclar draw calls de efeitos similares
    float maxOverdrawFactor;                  ///< Fator máximo de overdraw permitido
    int maxPostProcessPasses;                 ///< Máximo de passes de pós-processamento
    bool useHalfPrecision;                    ///< Usar half precision em cálculos
    bool useThreadedSimulation;               ///< Usar simulação multi-thread
    int asyncSimulationThreads;               ///< Threads para simulação assíncrona
    float effectBudgetMs;                     ///< Orçamento de tempo para efeitos (ms)
    float priorityScalingFactor;              ///< Fator de escala baseado em prioridade
    bool adaptiveComplexity;                  ///< Ajustar complexidade baseado em FPS
    bool useSoftParticles;                    ///< Usar partículas soft (necessita de depth)
    float softParticleDepthScale;             ///< Escala de profundidade para partículas soft
    bool useBillboardOrientation;             ///< Usar orientação de billboard para partículas
    
    /**
     * @brief Construtor com valores padrão
     */
    EffectSystemConfig() :
        globalQuality(EffectQuality::MEDIUM),
        maxActiveEffects(500),
        maxParticlesTotal(10000),
        maxParticlesPerSystem(200),
        visibilityDistance(1000.0f),
        dynamicQualityAdjustment(true),
        optimizationFlags(MERGE_SIMILAR_EFFECTS | BATCH_SIMILAR_EFFECTS | REUSE_PARTICLES),
        lodBias(0.0f),
        useInstancing(true),
        renderTargetScale(100),
        useDeferredDecals(true),
        particleSizeFactor(1.0f),
        maxLightsPerEffect(2),
        simulationTimeStep(1.0f/60.0f),
        useGPUSimulation(true),
        cullThresholdSize(0.5f),
        maxCascadesPerEffect(1),
        useTextureAtlas(true),
        mergeDrawCalls(true),
        maxOverdrawFactor(2.0f),
        maxPostProcessPasses(3),
        useHalfPrecision(false),
        useThreadedSimulation(true),
        asyncSimulationThreads(2),
        effectBudgetMs(2.0f),
        priorityScalingFactor(1.0f),
        adaptiveComplexity(true),
        useSoftParticles(true),
        softParticleDepthScale(1.0f),
        useBillboardOrientation(true)
    {}
    
    /**
     * @brief Cria configuração para baixo desempenho
     * @return Configuração otimizada para hardware fraco
     */
    static EffectSystemConfig createLowEndConfig() {
        EffectSystemConfig config;
        config.globalQuality = EffectQuality::LOW;
        config.maxActiveEffects = 100;
        config.maxParticlesTotal = 2000;
        config.maxParticlesPerSystem = 50;
        config.visibilityDistance = 500.0f;
        config.optimizationFlags = REDUCE_PARTICLE_COUNT | SIMPLIFY_PHYSICS | 
                                 REDUCE_TEXTURE_QUALITY | SKIP_MINOR_EFFECTS |
                                 MERGE_SIMILAR_EFFECTS | LIMIT_LIGHT_SOURCES |
                                 USE_BILLBOARD_PARTICLES | LIMIT_ACTIVE_DISTANCE |
                                 DISABLE_MOTION_BLUR | REDUCE_OVERDRAW |
                                 FIX_SIMULATION_TIMESTEP | DISABLE_SHADOWS_FROM_FX |
                                 USE_LOW_RES_BUFFER | DISABLE_NORMAL_MAPS |
                                 SIMPLIFIED_LIGHTING | CULL_TINY_EFFECTS;
        config.lodBias = -0.5f;
        config.renderTargetScale = 50;
        config.maxLightsPerEffect = 1;
        config.useHalfPrecision = true;
        config.useGPUSimulation = false;
        config.maxPostProcessPasses = 1;
        config.asyncSimulationThreads = 1;
        config.useSoftParticles = false;
        return config;
    }
    
    /**
     * @brief Cria configuração para desempenho extremamente baixo
     * @return Configuração para hardware muito limitado
     */
    static EffectSystemConfig createUltraLowEndConfig() {
        EffectSystemConfig config;
        config.globalQuality = EffectQuality::MINIMAL;
        config.maxActiveEffects = 50;
        config.maxParticlesTotal = 500;
        config.maxParticlesPerSystem = 20;
        config.visibilityDistance = 200.0f;
        config.dynamicQualityAdjustment = true;
        config.optimizationFlags = REDUCE_PARTICLE_COUNT | SIMPLIFY_PHYSICS | 
                                 REDUCE_TEXTURE_QUALITY | SKIP_MINOR_EFFECTS |
                                 MERGE_SIMILAR_EFFECTS | LIMIT_LIGHT_SOURCES |
                                 USE_BILLBOARD_PARTICLES | LIMIT_ACTIVE_DISTANCE |
                                 DISABLE_MOTION_BLUR | REDUCE_OVERDRAW |
                                 FIX_SIMULATION_TIMESTEP | SKIP_FRAMES | 
                                 REUSE_PARTICLES | DISABLE_GLOW |
                                 LIMIT_EFFECT_LIFESPAN | DISABLE_SHADOWS_FROM_FX |
                                 USE_LOW_RES_BUFFER | DISABLE_NORMAL_MAPS |
                                 SIMPLIFIED_LIGHTING | DISABLE_REFRACTION |
                                 LIMIT_REFLECTIONS | USE_HALF_PRECISION |
                                 DISABLE_VOLUMETRICS | SIMPLIFIED_ALPHA |
                                 LIMIT_POST_PROCESSING;
        config.lodBias = -1.0f;
        config.useInstancing = false;
        config.renderTargetScale = 25;
        config.useDeferredDecals = false;
        config.particleSizeFactor = 0.75f;
        config.maxLightsPerEffect = 0;
        config.useGPUSimulation = false;
        config.cullThresholdSize = 2.0f;
        config.maxCascadesPerEffect = 0;
        config.maxPostProcessPasses = 0;
        config.useHalfPrecision = true;
        config.useThreadedSimulation = false;
        config.effectBudgetMs = 1.0f;
        config.useSoftParticles = false;
        return config;
    }
};

/**
 * @brief Identificador único de efeito
 */
class EffectId {
public:
    EffectId() : m_id(0) {}
    explicit EffectId(uint64_t id) : m_id(id) {}
    explicit EffectId(const std::string& name);
    
    uint64_t getValue() const { return m_id; }
    std::string toString() const;
    bool isValid() const { return m_id != 0; }
    
    bool operator==(const EffectId& other) const { return m_id == other.m_id; }
    bool operator!=(const EffectId& other) const { return m_id != other.m_id; }
    bool operator<(const EffectId& other) const { return m_id < other.m_id; }
    
private:
    uint64_t m_id;
};

/**
 * @brief Estatísticas de performance do sistema de efeitos
 */
struct EffectSystemStats {
    int activeEffectCount;                ///< Número de efeitos ativos
    int totalParticleCount;               ///< Total de partículas ativas
    int visibleEffectCount;               ///< Efeitos visíveis no frame atual
    int culledEffectCount;                ///< Efeitos removidos por culling
    float simulationTimeMs;               ///< Tempo gasto em simulação (ms)
    float renderTimeMs;                   ///< Tempo gasto em renderização (ms)
    int drawCalls;                        ///< Número de draw calls
    int batchCount;                       ///< Número de batches
    float overdrawFactor;                 ///< Fator de overdraw médio
    int effectsPerType[12];               ///< Contagem por tipo de efeito
    float gpuMemoryUsageMB;               ///< Uso de memória GPU (MB)
    float cpuMemoryUsageMB;               ///< Uso de memória CPU (MB)
    int deferredDecalCount;               ///< Número de decals diferidos
    int lightSourceCount;                 ///< Número de fontes de luz de efeitos
    float particleUpdateTimeMs;           ///< Tempo para atualizar partículas (ms)
    float particleRenderTimeMs;           ///< Tempo para renderizar partículas (ms)
    float postProcessTimeMs;              ///< Tempo de pós-processamento (ms)
    int skippedEffectsLowPriority;        ///< Efeitos pulados por baixa prioridade
    int skippedEffectsBudget;             ///< Efeitos pulados por limite de orçamento
    int instancedBatches;                 ///< Batches usando instancing
    float adaptiveQualityScale;           ///< Escala adaptativa de qualidade atual
    int textureBindCount;                 ///< Trocas de textura
    int shaderBindCount;                  ///< Trocas de shader
    int renderTargetSwitches;             ///< Trocas de render target
    float currentEffectBudgetUsageMs;     ///< Uso atual do orçamento de tempo
    float highestParticleSystemCostMs;    ///< Sistema de partículas mais caro (ms)
    float averageParticleLifeMs;          ///< Vida média de partículas (ms)
    int reusedParticleCount;              ///< Partículas reutilizadas
    int asyncOperationsQueued;            ///< Operações assíncronas na fila
};

/**
 * @brief Parâmetros de um efeito
 */
struct EffectParameters {
    EffectType type;                      ///< Tipo de efeito
    EffectQuality quality;                ///< Qualidade específica
    EffectPriority priority;              ///< Prioridade do efeito
    std::string effectName;               ///< Nome do efeito
    std::string effectTemplate;           ///< Template/prefab do efeito
    float positionX;                      ///< Posição X no mundo
    float positionY;                      ///< Posição Y no mundo
    float positionZ;                      ///< Posição Z no mundo
    float rotationX;                      ///< Rotação X (graus)
    float rotationY;                      ///< Rotação Y (graus)
    float rotationZ;                      ///< Rotação Z (graus)
    float scale;                          ///< Escala geral do efeito
    float scaleX;                         ///< Escala no eixo X
    float scaleY;                         ///< Escala no eixo Y
    float scaleZ;                         ///< Escala no eixo Z
    float duration;                       ///< Duração do efeito (em segundos, 0 = infinito)
    float intensity;                      ///< Intensidade geral
    float speed;                          ///< Multiplicador de velocidade
    uint32_t colorARGB;                   ///< Cor em ARGB (tintagem)
    graphics::ResourceId textureId;       ///< Textura principal para o efeito
    graphics::ResourceId secondaryTextureId; ///< Textura secundária
    bool attachToEmitter;                 ///< Manter anexado ao emissor
    bool localSpace;                      ///< Usar espaço local
    bool castShadows;                     ///< Emitir sombras
    bool receiveLight;                    ///< Receber iluminação
    bool looping;                         ///< Efeito em loop
    bool usePhysics;                      ///< Usar física para partículas
    float delay;                          ///< Atraso antes de iniciar (segundos)
    float gravity;                        ///< Força da gravidade para o efeito
    float drag;                           ///< Arrasto aerodinâmico
    bool affectByWind;                    ///< Afetado pelo vento
    bool depthTest;                       ///< Usar teste de profundidade
    bool depthWrite;                      ///< Escrever no buffer de profundidade
    int particleLimit;                    ///< Limite específico de partículas
    bool useCustomShader;                 ///< Usar shader customizado
    std::string customShaderName;         ///< Nome do shader customizado
    int renderQueue;                      ///< Ordem de renderização
    float emissionRate;                   ///< Taxa de emissão de partículas
    std::unordered_map<std::string, float> customFloatParams; ///< Parâmetros float customizados
    std::unordered_map<std::string, int> customIntParams;     ///< Parâmetros int customizados
    std::unordered_map<std::string, bool> customBoolParams;   ///< Parâmetros bool customizados
    std::unordered_map<std::string, std::string> customStringParams; ///< Parâmetros string customizados
    
    /**
     * @brief Construtor com valores padrão
     */
    EffectParameters() :
        type(EffectType::PARTICLE_SYSTEM),
        quality(EffectQuality::MEDIUM),
        priority(EffectPriority::NORMAL),
        effectName(""),
        effectTemplate(""),
        positionX(0.0f),
        positionY(0.0f),
        positionZ(0.0f),
        rotationX(0.0f),
        rotationY(0.0f),
        rotationZ(0.0f),
        scale(1.0f),
        scaleX(1.0f),
        scaleY(1.0f),
        scaleZ(1.0f),
        duration(0.0f),
        intensity(1.0f),
        speed(1.0f),
        colorARGB(0xFFFFFFFF),
        attachToEmitter(false),
        localSpace(false),
        castShadows(false),
        receiveLight(true),
        looping(true),
        usePhysics(true),
        delay(0.0f),
        gravity(0.0f),
        drag(0.0f),
        affectByWind(false),
        depthTest(true),
        depthWrite(false),
        particleLimit(0),
        useCustomShader(false),
        customShaderName(""),
        renderQueue(0),
        emissionRate(10.0f)
    {}
};

/**
 * @brief Estado atual de um efeito
 */
struct EffectState {
    EffectId id;                         ///< ID do efeito
    bool isActive;                       ///< Se está ativo
    bool isVisible;                      ///< Se está visível
    bool isPaused;                       ///< Se está pausado
    float elapsedTime;                   ///< Tempo decorrido desde criação
    int particleCount;                   ///< Número atual de partículas
    float distance;                      ///< Distância atual do espectador
    EffectQuality currentQuality;        ///< Qualidade atual (pode ser adaptada)
    float screenSpaceSize;               ///< Tamanho em espaço de tela (%)
    bool wasCulled;                      ///< Se foi removido por culling
    float lastSimulationTimeMs;          ///< Tempo da última simulação (ms)
    float lastRenderTimeMs;              ///< Tempo da última renderização (ms)
    int drawCallCount;                   ///< Número de draw calls no último frame
    bool isTemporarilyDowngraded;        ///< Se foi temporariamente reduzido
    int instanceCount;                   ///< Número de instâncias
    int activeModifiers;                 ///< Número de modificadores ativos
};

/**
 * @brief Callback para eventos de efeitos
 */
using EffectCallback = std::function<void(const EffectId&, const EffectState&)>;

/**
 * @class AdvancedEffectSystem
 * @brief Sistema de efeitos visuais altamente otimizado
 */
class AdvancedEffectSystem {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static AdvancedEffectSystem& getInstance();
    
    /**
     * @brief Inicializa o sistema de efeitos
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Inicializa o sistema com configuração personalizada
     * @param config Configuração do sistema
     * @return true se inicializado com sucesso
     */
    bool initialize(const EffectSystemConfig& config);
    
    /**
     * @brief Finaliza o sistema
     */
    void shutdown();
    
    /**
     * @brief Analisa o sistema e cria configuração ótima
     * @param lowEnd Se o sistema é considerado de baixo desempenho
     * @return Configuração ideal
     */
    EffectSystemConfig createOptimalConfig(bool lowEnd = false);
    
    /**
     * @brief Cria um efeito de acordo com parâmetros
     * @param params Parâmetros do efeito
     * @return ID do efeito criado
     */
    EffectId createEffect(const EffectParameters& params);
    
    /**
     * @brief Cria um efeito a partir de um template
     * @param templateName Nome do template
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param scale Escala opcional
     * @return ID do efeito criado
     */
    EffectId createEffectFromTemplate(const std::string& templateName,
                                     float posX, float posY, float posZ,
                                     float scale = 1.0f);
    
    /**
     * @brief Destrói um efeito
     * @param effectId ID do efeito
     */
    void destroyEffect(const EffectId& effectId);
    
    /**
     * @brief Atualiza o sistema de efeitos
     * @param deltaTime Tempo decorrido desde o último frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Pausa um efeito
     * @param effectId ID do efeito
     */
    void pauseEffect(const EffectId& effectId);
    
    /**
     * @brief Continua um efeito pausado
     * @param effectId ID do efeito
     */
    void resumeEffect(const EffectId& effectId);
    
    /**
     * @brief Redefine um efeito (reinicia)
     * @param effectId ID do efeito
     */
    void resetEffect(const EffectId& effectId);
    
    /**
     * @brief Atualiza parâmetros de um efeito existente
     * @param effectId ID do efeito
     * @param params Novos parâmetros
     */
    void updateEffectParameters(const EffectId& effectId, const EffectParameters& params);
    
    /**
     * @brief Atualiza posição de um efeito
     * @param effectId ID do efeito
     * @param posX Nova posição X
     * @param posY Nova posição Y
     * @param posZ Nova posição Z
     */
    void setEffectPosition(const EffectId& effectId, float posX, float posY, float posZ);
    
    /**
     * @brief Atualiza rotação de um efeito
     * @param effectId ID do efeito
     * @param rotX Rotação X em graus
     * @param rotY Rotação Y em graus
     * @param rotZ Rotação Z em graus
     */
    void setEffectRotation(const EffectId& effectId, float rotX, float rotY, float rotZ);
    
    /**
     * @brief Atualiza escala de um efeito
     * @param effectId ID do efeito
     * @param scale Nova escala uniforme
     */
    void setEffectScale(const EffectId& effectId, float scale);
    
    /**
     * @brief Atualiza escala de um efeito (não uniforme)
     * @param effectId ID do efeito
     * @param scaleX Escala X
     * @param scaleY Escala Y
     * @param scaleZ Escala Z
     */
    void setEffectScale(const EffectId& effectId, float scaleX, float scaleY, float scaleZ);
    
    /**
     * @brief Define a cor de um efeito
     * @param effectId ID do efeito
     * @param color Cor em formato ARGB
     */
    void setEffectColor(const EffectId& effectId, uint32_t colorARGB);
    
    /**
     * @brief Define a intensidade de um efeito
     * @param effectId ID do efeito
     * @param intensity Nova intensidade
     */
    void setEffectIntensity(const EffectId& effectId, float intensity);
    
    /**
     * @brief Define a velocidade de um efeito
     * @param effectId ID do efeito
     * @param speed Novo multiplicador de velocidade
     */
    void setEffectSpeed(const EffectId& effectId, float speed);
    
    /**
     * @brief Verifica se um efeito está ativo
     * @param effectId ID do efeito
     * @return true se o efeito estiver ativo
     */
    bool isEffectActive(const EffectId& effectId) const;
    
    /**
     * @brief Obtém o estado atual de um efeito
     * @param effectId ID do efeito
     * @return Estado atual
     */
    EffectState getEffectState(const EffectId& effectId) const;
    
    /**
     * @brief Registra um callback para eventos de efeito
     * @param callback Função a ser chamada
     * @return ID do callback para remoção
     */
    int registerEffectCallback(EffectCallback callback);
    
    /**
     * @brief Remove um callback registrado
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEffectCallback(int callbackId);
    
    /**
     * @brief Atualiza configuração em tempo real
     * @param config Nova configuração
     * @return true se atualizada com sucesso
     */
    bool updateConfig(const EffectSystemConfig& config);
    
    /**
     * @brief Obtém a configuração atual
     * @return Configuração atual
     */
    const EffectSystemConfig& getCurrentConfig() const;
    
    /**
     * @brief Obtém estatísticas de performance do sistema
     * @return Estatísticas atuais
     */
    EffectSystemStats getStats() const;
    
    /**
     * @brief Define a posição do visualizador para otimização
     * @param posX Posição X do visualizador
     * @param posY Posição Y do visualizador
     * @param posZ Posição Z do visualizador
     */
    void setViewPosition(float posX, float posY, float posZ);
    
    /**
     * @brief Define a direção da visão
     * @param dirX Direção X
     * @param dirY Direção Y
     * @param dirZ Direção Z
     */
    void setViewDirection(float dirX, float dirY, float dirZ);
    
    /**
     * @brief Pausa todos os efeitos
     */
    void pauseAllEffects();
    
    /**
     * @brief Continua todos os efeitos
     */
    void resumeAllEffects();
    
    /**
     * @brief Destrói todos os efeitos
     */
    void destroyAllEffects();
    
    /**
     * @brief Pré-carrega templates para uso posterior
     * @param templateNames Lista de nomes de templates
     */
    void preloadEffectTemplates(const std::vector<std::string>& templateNames);
    
    /**
     * @brief Define a qualidade global
     * @param quality Nova qualidade global
     */
    void setGlobalQuality(EffectQuality quality);
    
    /**
     * @brief Define flags de otimização
     * @param flags Novas flags
     */
    void setOptimizationFlags(uint32_t flags);
    
    /**
     * @brief Define o modo adaptativo
     * @param enabled Estado do modo adaptativo
     * @param targetFPS FPS alvo para adaptação
     */
    void setAdaptiveMode(bool enabled, int targetFPS = 60);
    
    /**
     * @brief Lista todos os efeitos ativos
     * @return Lista de IDs de efeitos
     */
    std::vector<EffectId> listActiveEffects() const;
    
    /**
     * @brief Obtém contagem de efeitos por tipo
     * @param type Tipo de efeito
     * @return Número de efeitos desse tipo
     */
    int getEffectCountByType(EffectType type) const;
    
    /**
     * @brief Ativa ou desativa um tipo específico de efeito
     * @param type Tipo de efeito
     * @param enabled Estado de ativação
     */
    void setEffectTypeEnabled(EffectType type, bool enabled);
    
private:
    // Singleton
    AdvancedEffectSystem();
    ~AdvancedEffectSystem();
    AdvancedEffectSystem(const AdvancedEffectSystem&) = delete;
    AdvancedEffectSystem& operator=(const AdvancedEffectSystem&) = delete;
    
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace effects
} // namespace wydbr

#endif // WYDBR_ADVANCED_EFFECT_SYSTEM_H