/**
 * @file ScreenCaptureManager.h
 * @brief Sistema de captura de tela para compartilhamento
 */

#ifndef WYDBR_SCREEN_CAPTURE_MANAGER_H
#define WYDBR_SCREEN_CAPTURE_MANAGER_H

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>

namespace WYDBR {
namespace ScreenShare {

/**
 * Formato de pixel para imagem capturada
 */
enum class PixelFormat {
    RGB24,      // 3 bytes por pixel (R,G,B)
    RGBA32,     // 4 bytes por pixel (R,G,B,A)
    BGRA32,     // 4 bytes por pixel (B,G,R,A)
    NV12,       // Formato YUV para codificação mais eficiente
    YUY2        // Outro formato YUV
};

/**
 * Método de captura de tela
 */
enum class CaptureMethod {
    GDI,            // GDI - compatível, mais lento
    DXGI,           // DXGI - para DirectX, rápido
    WindowsGraphicsCapture, // Windows 10+ API moderna
    OpenGL,         // Captura OpenGL
    Vulkan          // Captura Vulkan
};

/**
 * Configuração para captura de tela
 */
struct ScreenCaptureConfig {
    int width;              // Largura da captura
    int height;             // Altura da captura
    PixelFormat format;     // Formato de pixel
    CaptureMethod method;   // Método de captura
    int frameRate;          // Taxa de frames alvo (1-60)
    bool captureCursor;     // Capturar cursor
    int monitorIndex;       // Índice do monitor (-1 = primário)
    void* windowHandle;     // Handle da janela (nullptr = tela cheia)
    bool useHardwareAcceleration; // Usar aceleração de hardware se disponível
};

/**
 * Estrutura para um frame capturado
 */
struct CapturedFrame {
    uint8_t* data;          // Dados da imagem
    size_t dataSize;        // Tamanho dos dados em bytes
    int width;              // Largura da imagem
    int height;             // Altura da imagem
    PixelFormat format;     // Formato de pixel
    int64_t timestamp;      // Timestamp de captura
    bool keyFrame;          // É um keyframe (para codificação)
};

/**
 * Callback para receber frames capturados
 */
using FrameCaptureCallback = std::function<void(const CapturedFrame& frame)>;

/**
 * Interface base para captura de tela
 */
class IScreenCapture {
public:
    virtual ~IScreenCapture() = default;
    
    /**
     * Inicializa o sistema de captura
     * @param config Configuração para captura
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const ScreenCaptureConfig& config) = 0;
    
    /**
     * Inicia a captura de tela
     * @param callback Função a ser chamada com frames capturados
     * @return true se iniciado com sucesso
     */
    virtual bool Start(FrameCaptureCallback callback) = 0;
    
    /**
     * Para a captura de tela
     */
    virtual void Stop() = 0;
    
    /**
     * Verifica se está capturando
     * @return true se está capturando
     */
    virtual bool IsCapturing() const = 0;
    
    /**
     * Obtém lista de monitores disponíveis
     * @return Lista de descrições de monitores
     */
    virtual std::vector<std::string> GetAvailableMonitors() = 0;
    
    /**
     * Obtém lista de janelas capturáveis
     * @return Lista de descrições de janelas
     */
    virtual std::vector<std::string> GetCapturableWindows() = 0;
    
    /**
     * Obtém a taxa de frames atual
     * @return Frames por segundo
     */
    virtual float GetCurrentFPS() const = 0;
    
    /**
     * Obtém estatísticas de captura
     * @return String com estatísticas formatadas
     */
    virtual std::string GetCaptureStats() const = 0;
    
    /**
     * Captura imediata de um único frame
     * @return Frame capturado
     */
    virtual CapturedFrame CaptureFrame() = 0;
};

/**
 * Implementação de captura de tela para Windows
 */
class WindowsScreenCapture : public IScreenCapture {
public:
    WindowsScreenCapture();
    ~WindowsScreenCapture() override;
    
    bool Initialize(const ScreenCaptureConfig& config) override;
    bool Start(FrameCaptureCallback callback) override;
    void Stop() override;
    bool IsCapturing() const override;
    std::vector<std::string> GetAvailableMonitors() override;
    std::vector<std::string> GetCapturableWindows() override;
    float GetCurrentFPS() const override;
    std::string GetCaptureStats() const override;
    CapturedFrame CaptureFrame() override;
    
private:
    // Implementação usando APIs Windows
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * Factory para criar sistema de captura de tela apropriado
 */
class ScreenCaptureFactory {
public:
    /**
     * Cria instância de captura de tela adequada para a plataforma
     * @return Ponteiro para sistema de captura
     */
    static std::unique_ptr<IScreenCapture> Create();
    
    /**
     * Determina o melhor método de captura disponível
     * @return Método de captura recomendado
     */
    static CaptureMethod DetectBestCaptureMethod();
};

/**
 * Gerenciador de alto nível para captura de tela
 */
class ScreenCaptureManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static ScreenCaptureManager& GetInstance();
    
    /**
     * Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * Inicia a captura com a configuração atual
     * @param callback Função para receber frames
     * @return true se iniciado com sucesso
     */
    bool StartCapture(FrameCaptureCallback callback);
    
    /**
     * Para a captura atual
     */
    void StopCapture();
    
    /**
     * Configura os parâmetros de captura
     * @param config Nova configuração
     * @return true se configuração é válida
     */
    bool Configure(const ScreenCaptureConfig& config);
    
    /**
     * Obtém a configuração atual
     * @return Configuração atual
     */
    ScreenCaptureConfig GetCurrentConfig() const;
    
    /**
     * Obtém estatísticas do sistema de captura
     * @return String formatada com estatísticas
     */
    std::string GetStatistics() const;
    
private:
    // Implementação privada do singleton
    ScreenCaptureManager();
    ~ScreenCaptureManager();
    
    // Membros privados
    std::unique_ptr<IScreenCapture> m_captureImpl;
    ScreenCaptureConfig m_currentConfig;
    mutable std::mutex m_configMutex;
    std::atomic<bool> m_isInitialized;
    std::atomic<bool> m_isCapturing;
};

} // namespace ScreenShare
} // namespace WYDBR

#endif // WYDBR_SCREEN_CAPTURE_MANAGER_H
