/**
 * @file modern_renderer.h
 * @brief Sistema de renderização modernizado do WYDBR
 *
 * Este sistema implementa uma camada de abstração para renderização
 * que suporta múltiplas APIs gráficas (DirectX 11/12, Vulkan)
 * e gerencia otimizações de performance e qualidade visual.
 */

#ifndef WYDBR_MODERN_RENDERER_H
#define WYDBR_MODERN_RENDERER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace wydbr {
namespace graphics {

/**
 * @brief APIs gráficas suportadas
 */
enum class GraphicsAPI {
    DIRECTX_11,    ///< DirectX 11
    DIRECTX_12,    ///< DirectX 12
    VULKAN,        ///< Vulkan
    OPENGL,        ///< OpenGL (fallback)
    SOFTWARE       ///< Renderização por software (último recurso)
};

/**
 * @brief Níveis de qualidade para efeitos
 */
enum class QualityLevel {
    LOW,           ///< Baixa qualidade, alta performance
    MEDIUM,        ///< Equilíbrio entre qualidade e performance
    HIGH,          ///< Alta qualidade, mais exigente
    ULTRA,         ///< Qualidade máxima, muito exigente
    ADAPTIVE       ///< Adapta-se automaticamente com base na performance
};

/**
 * @brief Configurações de framerate
 */
enum class FramerateMode {
    UNLIMITED,     ///< Sem limite de FPS
    VSYNC,         ///< Sincronizado com a taxa de atualização do monitor
    FIXED_30,      ///< Fixo em 30 FPS
    FIXED_60,      ///< Fixo em 60 FPS
    FIXED_120,     ///< Fixo em 120 FPS
    ADAPTIVE       ///< Adapta-se para manter performance estável
};

/**
 * @brief Estrutura de configuração de renderização
 */
struct RenderConfig {
    GraphicsAPI api;                ///< API gráfica a ser usada
    QualityLevel quality;           ///< Nível de qualidade geral
    FramerateMode framerateMode;    ///< Modo de controle de framerate
    int resolutionWidth;            ///< Largura da resolução
    int resolutionHeight;           ///< Altura da resolução
    bool fullscreen;                ///< Modo de tela cheia
    bool vsync;                     ///< Sincronização vertical
    bool hdrEnabled;                ///< Suporte a HDR
    float gamma;                    ///< Ajuste de gamma
    float brightness;               ///< Ajuste de brilho
    float contrast;                 ///< Ajuste de contraste
    float shadowQuality;            ///< Qualidade das sombras (0.0 - 1.0)
    float effectDensity;            ///< Densidade dos efeitos (0.0 - 1.0)
    float textureQuality;           ///< Qualidade das texturas (0.0 - 1.0)
    float antialiasing;             ///< Nível de antialiasing (0.0 - 1.0)
    
    /**
     * @brief Construtor com valores padrão
     */
    RenderConfig() :
        api(GraphicsAPI::DIRECTX_11),
        quality(QualityLevel::MEDIUM),
        framerateMode(FramerateMode::VSYNC),
        resolutionWidth(1920),
        resolutionHeight(1080),
        fullscreen(false),
        vsync(true),
        hdrEnabled(false),
        gamma(1.0f),
        brightness(1.0f),
        contrast(1.0f),
        shadowQuality(0.7f),
        effectDensity(0.8f),
        textureQuality(0.8f),
        antialiasing(0.5f)
    {}
};

/**
 * @brief Informações de desempenho da renderização
 */
struct RenderStats {
    float fps;                     ///< FPS atual
    float frametime;               ///< Tempo do último frame (ms)
    float averageFrametime;        ///< Tempo médio de frame (ms)
    float gpuUsage;                ///< Uso da GPU (%)
    float cpuRenderUsage;          ///< Uso da CPU para renderização (%)
    size_t drawCalls;              ///< Número de chamadas de desenho
    size_t triangleCount;          ///< Triângulos renderizados
    size_t textureMemory;          ///< Memória de texturas usada (bytes)
    size_t bufferMemory;           ///< Memória de buffers usada (bytes)
    float batchEfficiency;         ///< Eficiência de batching (0.0 - 1.0)
};

/**
 * @brief Informações sobre o hardware gráfico
 */
struct GraphicsHardwareInfo {
    std::string gpuName;           ///< Nome da GPU
    std::string gpuVendor;         ///< Fabricante da GPU
    std::string driverVersion;     ///< Versão do driver
    size_t vramTotal;              ///< Memória total da GPU (bytes)
    size_t vramAvailable;          ///< Memória disponível da GPU (bytes)
    int maxTextureSize;            ///< Tamanho máximo de textura suportado
    std::vector<GraphicsAPI> supportedAPIs; ///< APIs suportadas pelo hardware
    bool supportsDXR;              ///< Suporte a ray tracing DirectX
    bool supportsHDR;              ///< Suporte a HDR
};

/**
 * @brief Interface de callback para eventos de renderização
 */
typedef std::function<void(const RenderStats&)> RenderStatsCallback;

/**
 * @class ModernRenderer
 * @brief Sistema de renderização moderno do WYDBR
 */
class ModernRenderer {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static ModernRenderer& getInstance();
    
    /**
     * @brief Inicializa o sistema de renderização
     * @param config Configuração inicial
     * @return Verdadeiro se inicializado com sucesso
     */
    bool initialize(const RenderConfig& config = RenderConfig());
    
    /**
     * @brief Finaliza o sistema de renderização
     */
    void shutdown();
    
    /**
     * @brief Detecta o hardware gráfico disponível
     * @return Informações sobre o hardware
     */
    GraphicsHardwareInfo detectHardware();
    
    /**
     * @brief Seleciona automaticamente a melhor API e configurações
     * @return Configuração otimizada para o hardware
     */
    RenderConfig autoDetectBestConfig();
    
    /**
     * @brief Atualiza as configurações de renderização
     * @param config Nova configuração
     * @return Verdadeiro se aplicado com sucesso
     */
    bool updateConfig(const RenderConfig& config);
    
    /**
     * @brief Aplica configurações adaptativas baseadas em performance
     */
    void applyAdaptiveSettings();
    
    /**
     * @brief Obtém estatísticas atuais de renderização
     * @return Estatísticas de renderização
     */
    RenderStats getStats() const;
    
    /**
     * @brief Registra callback para monitoramento de estatísticas
     * @param callback Função a ser chamada com estatísticas
     * @return ID do callback para remoção posterior
     */
    int addStatsCallback(RenderStatsCallback callback);
    
    /**
     * @brief Remove um callback de estatísticas
     * @param callbackId ID do callback a ser removido
     * @return Verdadeiro se removido com sucesso
     */
    bool removeStatsCallback(int callbackId);
    
    /**
     * @brief Ativa ou desativa a otimização dinâmica de performance
     * @param enabled Estado da otimização dinâmica
     */
    void setDynamicOptimization(bool enabled);
    
    /**
     * @brief Obtém configuração atual
     * @return Configuração atual
     */
    const RenderConfig& getCurrentConfig() const;
    
    /**
     * @brief Define limite de FPS personalizado
     * @param fps Limite de FPS (0 para ilimitado)
     */
    void setCustomFramerateCap(int fps);
    
    /**
     * @brief Aplica otimizações para GPU específica
     * @param gpuVendor Nome do fabricante da GPU
     * @param gpuModel Modelo da GPU
     */
    void applyGPUSpecificOptimizations(const std::string& gpuVendor, const std::string& gpuModel);
    
private:
    // Singleton
    ModernRenderer();
    ~ModernRenderer();
    ModernRenderer(const ModernRenderer&) = delete;
    ModernRenderer& operator=(const ModernRenderer&) = delete;
    
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace graphics
} // namespace wydbr

#endif // WYDBR_MODERN_RENDERER_H