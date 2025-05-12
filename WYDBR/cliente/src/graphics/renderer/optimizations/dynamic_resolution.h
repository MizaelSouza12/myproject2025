#ifndef WYDBR_DYNAMIC_RESOLUTION_H
#define WYDBR_DYNAMIC_RESOLUTION_H

#include <chrono>
#include <vector>
#include <mutex>
#include <memory>
#include <functional>

namespace wydbr {
namespace graphics {

// Estrutura de configuração do sistema de resolução dinâmica
struct DynamicResolutionConfig {
    bool enabled;              // Se o sistema está ativado
    float targetFPS;           // FPS alvo
    float minScaleFactor;      // Fator mínimo de escala (ex: 0.5 = 50% da resolução)
    float maxScaleFactor;      // Fator máximo de escala (ex: 1.0 = 100% da resolução)
    float adjustmentSpeed;     // Velocidade de ajuste (0.0-1.0)
    int stabilityFrames;       // Número de frames para estabilização antes de ajuste
    float hysteresis;          // Margem de histerese para evitar oscilações
    bool useUpscaling;         // Se deve usar upscaling inteligente
    int upscalingQuality;      // Qualidade do upscaling (0-100)
    bool adaptToMovement;      // Se deve reduzir qualidade durante movimento rápido
    bool useCameraMotionHint;  // Se deve usar dicas de movimento de câmera
    
    // Construtor com valores padrão
    DynamicResolutionConfig() :
        enabled(true),
        targetFPS(60.0f),
        minScaleFactor(0.5f),
        maxScaleFactor(1.0f),
        adjustmentSpeed(0.1f),
        stabilityFrames(5),
        hysteresis(0.05f),
        useUpscaling(true),
        upscalingQuality(75),
        adaptToMovement(true),
        useCameraMotionHint(true)
    {}
};

// Callback para notificação de mudança de resolução
using ResolutionChangeCallback = std::function<void(int width, int height, float scaleFactor)>;

/**
 * @class DynamicResolutionSystem
 * @brief Sistema que ajusta dinamicamente a resolução de renderização para manter FPS alvo
 * 
 * Este sistema monitora a performance e ajusta a resolução de renderização,
 * combinada com técnicas de upscaling para manter um bom equilíbrio entre
 * qualidade visual e desempenho.
 */
class DynamicResolutionSystem {
public:
    DynamicResolutionSystem();
    ~DynamicResolutionSystem();
    
    /**
     * @brief Inicializa o sistema de resolução dinâmica
     * @param baseWidth Largura base da resolução
     * @param baseHeight Altura base da resolução
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(int baseWidth, int baseHeight, const DynamicResolutionConfig& config = DynamicResolutionConfig());
    
    /**
     * @brief Libera recursos do sistema
     */
    void shutdown();
    
    /**
     * @brief Atualiza o sistema a cada frame
     * @param deltaTimeMs Tempo decorrido desde o último frame em milissegundos
     * @param currentFPS FPS atual
     * @param isCameraMoving Se a câmera está em movimento
     * @param cameraSpeed Velocidade de movimento da câmera (0.0-1.0)
     */
    void update(float deltaTimeMs, float currentFPS, bool isCameraMoving = false, float cameraSpeed = 0.0f);
    
    /**
     * @brief Obtém a resolução atual
     * @param outWidth Largura atual (saída)
     * @param outHeight Altura atual (saída)
     * @param outScaleFactor Fator de escala atual (saída)
     */
    void getCurrentResolution(int& outWidth, int& outHeight, float& outScaleFactor) const;
    
    /**
     * @brief Define a resolução base (destino final do upscaling)
     * @param width Largura da resolução base
     * @param height Altura da resolução base
     * @return true se a mudança foi bem-sucedida
     */
    bool setBaseResolution(int width, int height);
    
    /**
     * @brief Define a configuração do sistema
     * @param config Nova configuração
     */
    void setConfig(const DynamicResolutionConfig& config);
    
    /**
     * @brief Obtém a configuração atual
     * @return Configuração atual
     */
    DynamicResolutionConfig getConfig() const;
    
    /**
     * @brief Força um fator de escala específico
     * @param scaleFactor Fator de escala forçado
     * @return true se o fator foi aplicado com sucesso
     */
    bool forceScaleFactor(float scaleFactor);
    
    /**
     * @brief Registra um callback para mudança de resolução
     * @param callback Função a ser chamada
     * @return ID do callback para remoção
     */
    int registerResolutionChangeCallback(ResolutionChangeCallback callback);
    
    /**
     * @brief Remove um callback registrado
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterResolutionChangeCallback(int callbackId);
    
    /**
     * @brief Define o método de upscaling
     * @param method Nome do método (ex: "bilinear", "lanczos", "fsr", "dlss")
     * @param quality Qualidade do upscaling (0-100)
     * @return true se o método foi definido com sucesso
     */
    bool setUpscalingMethod(const std::string& method, int quality);
    
    /**
     * @brief Verifica se o sistema está ativo
     * @return true se o sistema está ativo
     */
    bool isEnabled() const;
    
    /**
     * @brief Ativa ou desativa o sistema
     * @param enabled Novo estado
     */
    void setEnabled(bool enabled);

private:
    // Estrutura para histórico de frames
    struct FrameInfo {
        float fps;
        float deltaTimeMs;
        std::chrono::steady_clock::time_point timestamp;
        bool wasCameraMoving;
        float cameraSpeed;
    };
    
    // Métodos internos
    void calculateIdealScaleFactor();
    void applyResolutionChange(float newScaleFactor);
    void notifyResolutionChange();
    float computeSmoothedFPS() const;
    bool shouldAdjustResolution() const;
    
    // Dados e estado
    int m_baseWidth;
    int m_baseHeight;
    int m_currentWidth;
    int m_currentHeight;
    float m_currentScaleFactor;
    float m_targetScaleFactor;
    DynamicResolutionConfig m_config;
    std::vector<FrameInfo> m_frameHistory;
    int m_stableFrameCount;
    bool m_adjustingUp;
    std::string m_upscalingMethod;
    
    // Callbacks
    std::vector<std::pair<int, ResolutionChangeCallback>> m_callbacks;
    int m_nextCallbackId;
    
    // Thread safety
    mutable std::mutex m_mutex;
    
    // Implementação específica de upscaling
    std::unique_ptr<class UpscalingImplementation> m_upscaler;
};

} // namespace graphics
} // namespace wydbr

#endif // WYDBR_DYNAMIC_RESOLUTION_H
