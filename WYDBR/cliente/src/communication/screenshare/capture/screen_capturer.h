#ifndef WYDBR_SCREEN_CAPTURER_H
#define WYDBR_SCREEN_CAPTURER_H

#include <vector>
#include <string>
#include <atomic>
#include <memory>
#include <functional>
#include <mutex>

namespace wydbr {
namespace communication {

// Enumeração para tipo de captura
enum class CaptureType {
    GDI,             // Captura baseada em GDI (Windows)
    DXGI,            // Captura baseada em DXGI (Windows)
    X11,             // Captura baseada em X11 (Linux)
    WAYLAND,         // Captura baseada em Wayland (Linux)
    DIRECTX,         // Captura baseada em DirectX
    OPENGL,          // Captura baseada em OpenGL
    WINDOWS_GRAPHICS_CAPTURE // Captura baseada em Windows.Graphics.Capture API
};

// Estrutura para configuração do capturador
struct ScreenCapturerConfig {
    bool captureMouseCursor;
    bool captureBorderless;
    int frameRateLimit;
    CaptureType preferredCaptureType;
    int quality;
    bool useDirtyRectOptimization;
    bool useHardwareAcceleration;
    int threadPriority;
    
    // Construtor com valores padrão
    ScreenCapturerConfig() :
        captureMouseCursor(true),
        captureBorderless(true),
        frameRateLimit(60),
        preferredCaptureType(CaptureType::DXGI),
        quality(85),
        useDirtyRectOptimization(true),
        useHardwareAcceleration(true),
        threadPriority(2)
    {}
};

// Estrutura para descrever uma janela capturável
struct CaptureTarget {
    std::string title;
    std::string processName;
    uint64_t windowHandle;
    int posX;
    int posY;
    int width;
    int height;
    bool isFullscreen;
    bool isMinimized;
};

// Estrutura para retângulo sujo (região alterada)
struct DirtyRect {
    int x;
    int y;
    int width;
    int height;
};

// Estrutura para dados de frame capturado
struct CapturedFrame {
    std::vector<uint8_t> data;
    int width;
    int height;
    int stride;
    int format;
    uint64_t timestamp;
    bool hasDirtyRects;
    std::vector<DirtyRect> dirtyRects;
    bool hasMouseCursor;
    int mouseCursorX;
    int mouseCursorY;
    std::vector<uint8_t> mouseCursorData;
    int mouseCursorWidth;
    int mouseCursorHeight;
    int mouseCursorHotspotX;
    int mouseCursorHotspotY;
};

// Callback para frame capturado
using FrameCapturedCallback = std::function<void(const CapturedFrame& frame)>;

/**
 * @class ScreenCapturer
 * @brief Capturador de tela com alto desempenho
 * 
 * Captura a tela, janela de aplicativo ou região personalizada,
 * com otimizações para desempenho em sistemas de baixo recurso.
 */
class ScreenCapturer {
public:
    ScreenCapturer();
    ~ScreenCapturer();
    
    /**
     * @brief Inicializa o capturador de tela
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const ScreenCapturerConfig& config = ScreenCapturerConfig());
    
    /**
     * @brief Desliga o capturador e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Inicia a captura da tela inteira
     * @param displayIndex Índice do monitor (0 para o principal)
     * @return true se iniciado com sucesso
     */
    bool startCaptureScreen(int displayIndex = 0);
    
    /**
     * @brief Inicia a captura de uma janela de aplicativo
     * @param windowHandle Handle da janela (0 para janela do jogo atual)
     * @return true se iniciado com sucesso
     */
    bool startCaptureWindow(uint64_t windowHandle = 0);
    
    /**
     * @brief Inicia a captura de uma região personalizada
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param width Largura
     * @param height Altura
     * @return true se iniciado com sucesso
     */
    bool startCaptureRegion(int x, int y, int width, int height);
    
    /**
     * @brief Para a captura
     */
    void stopCapture();
    
    /**
     * @brief Verifica se está capturando
     * @return true se está capturando
     */
    bool isCapturing() const;
    
    /**
     * @brief Obtém a lista de janelas capturáveis
     * @return Lista de alvos de captura
     */
    std::vector<CaptureTarget> getAvailableWindows();
    
    /**
     * @brief Obtém a lista de monitores
     * @return Lista de alvos de captura (monitores)
     */
    std::vector<CaptureTarget> getAvailableScreens();
    
    /**
     * @brief Captura um único quadro
     * @param frame Estrutura de frame para popular (saída)
     * @return true se capturado com sucesso
     */
    bool captureFrame(CapturedFrame& frame);
    
    /**
     * @brief Registra um callback para evento de frame capturado
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerFrameCapturedCallback(FrameCapturedCallback callback);
    
    /**
     * @brief Remove um callback de evento de frame capturado
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterFrameCapturedCallback(int callbackId);
    
    /**
     * @brief Define a taxa de quadros limite
     * @param frameRate Taxa de quadros
     */
    void setFrameRateLimit(int frameRate);
    
    /**
     * @brief Obtém a taxa de quadros limite atual
     * @return Taxa de quadros
     */
    int getFrameRateLimit() const;
    
    /**
     * @brief Ativa ou desativa a captura do cursor do mouse
     * @param capture Se deve capturar
     */
    void setCaptureMouseCursor(bool capture);
    
    /**
     * @brief Verifica se está capturando o cursor
     * @return true se está capturando
     */
    bool isCapturingMouseCursor() const;
    
    /**
     * @brief Ativa ou desativa a otimização de retângulos sujos
     * @param enable Se deve ativar
     */
    void enableDirtyRectOptimization(bool enable);
    
    /**
     * @brief Verifica se a otimização está ativa
     * @return true se ativa
     */
    bool isDirtyRectOptimizationEnabled() const;
    
    /**
     * @brief Obtém informações sobre o alvo de captura atual
     * @return Alvo de captura atual
     */
    CaptureTarget getCurrentTarget() const;
    
    /**
     * @brief Obtém estatísticas de captura
     * @param outFps Taxa de quadros atual (saída)
     * @param outAvgCaptureTimeMs Tempo médio de captura em ms (saída)
     * @param outLastFrameSizeBytes Tamanho do último quadro em bytes (saída)
     */
    void getStats(float& outFps, float& outAvgCaptureTimeMs, int& outLastFrameSizeBytes) const;
    
    /**
     * @brief Ativa ou desativa a aceleração de hardware
     * @param enable Se deve ativar
     */
    void enableHardwareAcceleration(bool enable);
    
    /**
     * @brief Verifica se a aceleração está ativa
     * @return true se ativa
     */
    bool isHardwareAccelerationEnabled() const;
    
private:
    // Métodos internos
    void captureThread();
    bool detectCaptureMethod();
    std::vector<DirtyRect> detectDirtyRects(const std::vector<uint8_t>& currentFrame, 
                                          const std::vector<uint8_t>& previousFrame,
                                          int width, int height, int stride);
    void captureCursor(CapturedFrame& frame);
    
    // Métodos específicos de API
    bool initializeGDI();
    bool initializeDXGI();
    bool initializeX11();
    bool initializeWayland();
    bool initializeDirectX();
    bool initializeOpenGL();
    bool initializeWindowsGraphicsCapture();
    
    bool captureFrameGDI(CapturedFrame& frame);
    bool captureFrameDXGI(CapturedFrame& frame);
    bool captureFrameX11(CapturedFrame& frame);
    bool captureFrameWayland(CapturedFrame& frame);
    bool captureFrameDirectX(CapturedFrame& frame);
    bool captureFrameOpenGL(CapturedFrame& frame);
    bool captureFrameWindowsGraphicsCapture(CapturedFrame& frame);
    
    // Estado interno
    bool m_initialized;
    ScreenCapturerConfig m_config;
    std::atomic<bool> m_isCapturing;
    CaptureType m_activeMethod;
    CaptureTarget m_currentTarget;
    
    std::vector<uint8_t> m_previousFrameData;
    int m_previousWidth;
    int m_previousHeight;
    int m_previousStride;
    
    // Thread
    std::unique_ptr<std::thread> m_captureThread;
    std::atomic<bool> m_threadShouldRun;
    
    // Estatísticas
    struct Stats {
        std::atomic<float> fps;
        std::atomic<float> avgCaptureTimeMs;
        std::atomic<int> lastFrameSizeBytes;
        std::chrono::steady_clock::time_point lastFrameTime;
    } m_stats;
    
    // Callbacks
    std::vector<std::pair<int, FrameCapturedCallback>> m_callbacks;
    int m_nextCallbackId;
    mutable std::mutex m_callbackMutex;
    
    // Implementações específicas da plataforma
    void* m_platformData;
};

} // namespace communication
} // namespace wydbr

#endif // WYDBR_SCREEN_CAPTURER_H
