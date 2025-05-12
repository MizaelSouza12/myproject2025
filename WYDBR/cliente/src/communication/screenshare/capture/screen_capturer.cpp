#include "screen_capturer.h"
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>
#include <iostream>

namespace wydbr {
namespace communication {

ScreenCapturer::ScreenCapturer() :
    m_initialized(false),
    m_isCapturing(false),
    m_activeMethod(CaptureType::DXGI),
    m_previousWidth(0),
    m_previousHeight(0),
    m_previousStride(0),
    m_threadShouldRun(false),
    m_nextCallbackId(1),
    m_platformData(nullptr)
{
    // Inicialização das estatísticas
    m_stats.fps = 0.0f;
    m_stats.avgCaptureTimeMs = 0.0f;
    m_stats.lastFrameSizeBytes = 0;
    m_stats.lastFrameTime = std::chrono::steady_clock::now();
}

ScreenCapturer::~ScreenCapturer() {
    shutdown();
}

bool ScreenCapturer::initialize(const ScreenCapturerConfig& config) {
    if (m_initialized) {
        std::cerr << "ScreenCapturer já inicializado" << std::endl;
        return false;
    }
    
    // Armazenar configuração
    m_config = config;
    
    // Detectar melhor método de captura disponível
    if (!detectCaptureMethod()) {
        std::cerr << "Nenhum método de captura disponível" << std::endl;
        return false;
    }
    
    // Inicializar método específico
    bool initSuccess = false;
    
    switch (m_activeMethod) {
        case CaptureType::GDI:
            initSuccess = initializeGDI();
            break;
            
        case CaptureType::DXGI:
            initSuccess = initializeDXGI();
            break;
            
        case CaptureType::X11:
            initSuccess = initializeX11();
            break;
            
        case CaptureType::WAYLAND:
            initSuccess = initializeWayland();
            break;
            
        case CaptureType::DIRECTX:
            initSuccess = initializeDirectX();
            break;
            
        case CaptureType::OPENGL:
            initSuccess = initializeOpenGL();
            break;
            
        case CaptureType::WINDOWS_GRAPHICS_CAPTURE:
            initSuccess = initializeWindowsGraphicsCapture();
            break;
            
        default:
            initSuccess = false;
            break;
    }
    
    if (!initSuccess) {
        std::cerr << "Falha ao inicializar método de captura" << std::endl;
        return false;
    }
    
    m_initialized = true;
    return true;
}

void ScreenCapturer::shutdown() {
    if (!m_initialized) return;
    
    // Parar captura se estiver rodando
    if (m_isCapturing) {
        stopCapture();
    }
    
    // Liberar recursos específicos da plataforma
    // TODO: Implementar quando tivermos as implementações específicas
    
    m_initialized = false;
}

bool ScreenCapturer::detectCaptureMethod() {
    // Usar método preferido da configuração, se disponível
    CaptureType preferred = m_config.preferredCaptureType;
    
    // Se método preferido não for disponível, detectar baseado no sistema
#ifdef _WIN32
    // No Windows, tentar primeiro o método mais moderno
    if (preferred == CaptureType::WINDOWS_GRAPHICS_CAPTURE) {
        // Verificar se Windows.Graphics.Capture está disponível (Windows 10 1809+)
        // TODO: Implementar detecção real
        return true;
    } else if (preferred == CaptureType::DXGI) {
        // Verificar se DXGI está disponível
        // TODO: Implementar detecção real
        return true;
    } else if (preferred == CaptureType::GDI) {
        // GDI sempre disponível no Windows
        m_activeMethod = CaptureType::GDI;
        return true;
    }
    
    // Autodetecção no Windows
    // Tentar métodos em ordem de preferência
    m_activeMethod = CaptureType::WINDOWS_GRAPHICS_CAPTURE;
    // Se não disponível, cair para DXGI
    m_activeMethod = CaptureType::DXGI;
    // Se não disponível, cair para GDI
    m_activeMethod = CaptureType::GDI;
    
    return true;
#elif defined(__linux__)
    // No Linux, verificar se Wayland ou X11
    if (preferred == CaptureType::WAYLAND) {
        // Verificar se estamos rodando em Wayland
        // TODO: Implementar detecção real
        const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
        if (waylandDisplay) {
            m_activeMethod = CaptureType::WAYLAND;
            return true;
        }
    } else if (preferred == CaptureType::X11) {
        // Verificar se X11 está disponível
        // TODO: Implementar detecção real
        const char* display = getenv("DISPLAY");
        if (display) {
            m_activeMethod = CaptureType::X11;
            return true;
        }
    }
    
    // Autodetecção no Linux
    const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
    if (waylandDisplay) {
        m_activeMethod = CaptureType::WAYLAND;
        return true;
    }
    
    const char* display = getenv("DISPLAY");
    if (display) {
        m_activeMethod = CaptureType::X11;
        return true;
    }
    
    // Nenhum método disponível
    return false;
#else
    // Para outros sistemas, tentar OpenGL
    m_activeMethod = CaptureType::OPENGL;
    return true;
#endif
}

// Métodos parciais de inicialização específica de API
bool ScreenCapturer::initializeGDI() {
    // TODO: Implementar quando tivermos as implementações específicas
    std::cout << "[ScreenCapturer] Inicializando GDI" << std::endl;
    return true;
}

bool ScreenCapturer::initializeDXGI() {
    // TODO: Implementar quando tivermos as implementações específicas
    std::cout << "[ScreenCapturer] Inicializando DXGI" << std::endl;
    return true;
}

bool ScreenCapturer::initializeX11() {
    // TODO: Implementar quando tivermos as implementações específicas
    std::cout << "[ScreenCapturer] Inicializando X11" << std::endl;
    return true;
}

bool ScreenCapturer::initializeWayland() {
    // TODO: Implementar quando tivermos as implementações específicas
    std::cout << "[ScreenCapturer] Inicializando Wayland" << std::endl;
    return true;
}

bool ScreenCapturer::initializeDirectX() {
    // TODO: Implementar quando tivermos as implementações específicas
    std::cout << "[ScreenCapturer] Inicializando DirectX" << std::endl;
    return true;
}

bool ScreenCapturer::initializeOpenGL() {
    // TODO: Implementar quando tivermos as implementações específicas
    std::cout << "[ScreenCapturer] Inicializando OpenGL" << std::endl;
    return true;
}

bool ScreenCapturer::initializeWindowsGraphicsCapture() {
    // TODO: Implementar quando tivermos as implementações específicas
    std::cout << "[ScreenCapturer] Inicializando Windows Graphics Capture" << std::endl;
    return true;
}

// Métodos de captura
bool ScreenCapturer::startCaptureScreen(int displayIndex) {
    if (!m_initialized) {
        std::cerr << "ScreenCapturer não inicializado" << std::endl;
        return false;
    }
    
    if (m_isCapturing) {
        stopCapture();
    }
    
    // TODO: Implementar quando tivermos as implementações específicas
    
    // Configurar alvo de captura
    m_currentTarget.windowHandle = 0;
    m_currentTarget.isFullscreen = true;
    m_currentTarget.title = "Screen " + std::to_string(displayIndex);
    
    // Iniciar thread de captura
    m_threadShouldRun = true;
    m_captureThread = std::make_unique<std::thread>(&ScreenCapturer::captureThread, this);
    
    m_isCapturing = true;
    return true;
}

void ScreenCapturer::stopCapture() {
    if (!m_isCapturing) return;
    
    // Parar thread de captura
    m_threadShouldRun = false;
    
    if (m_captureThread && m_captureThread->joinable()) {
        m_captureThread->join();
    }
    
    m_captureThread.reset();
    m_isCapturing = false;
}

bool ScreenCapturer::isCapturing() const {
    return m_isCapturing;
}

// Thread de captura
void ScreenCapturer::captureThread() {
    while (m_threadShouldRun) {
        // Calcular tempo para throttling baseado no limite de taxa de quadros
        const float frameTimeTarget = 1.0f / m_config.frameRateLimit;
        const auto startTime = std::chrono::steady_clock::now();
        
        // Capturar um quadro
        CapturedFrame frame;
        bool success = captureFrame(frame);
        
        if (success) {
            // Calcular estatísticas
            const auto endTime = std::chrono::steady_clock::now();
            const float captureTime = std::chrono::duration<float, std::milli>(endTime - startTime).count();
            
            const auto elapsedSinceLastFrame = std::chrono::duration<float>(
                endTime - m_stats.lastFrameTime).count();
            m_stats.lastFrameTime = endTime;
            
            m_stats.fps = 1.0f / elapsedSinceLastFrame;
            m_stats.avgCaptureTimeMs = (m_stats.avgCaptureTimeMs * 0.9f) + (captureTime * 0.1f);
            m_stats.lastFrameSizeBytes = static_cast<int>(frame.data.size());
            
            // Notificar callbacks
            std::lock_guard<std::mutex> lock(m_callbackMutex);
            
            for (const auto& callback : m_callbacks) {
                callback.second(frame);
            }
        }
        
        // Calcular quanto tempo esperar para manter a taxa de quadros alvo
        const auto frameEndTime = std::chrono::steady_clock::now();
        const float frameTime = std::chrono::duration<float>(frameEndTime - startTime).count();
        
        if (frameTime < frameTimeTarget) {
            const float sleepTime = (frameTimeTarget - frameTime) * 1000.0f;
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleepTime)));
        }
    }
}

// Utilitários
std::vector<DirtyRect> ScreenCapturer::detectDirtyRects(
    const std::vector<uint8_t>& currentFrame, 
    const std::vector<uint8_t>& previousFrame,
    int width, int height, int stride)
{
    // Se não temos frame anterior ou as dimensões mudaram, todo o frame é "sujo"
    if (previousFrame.empty() || 
        width != m_previousWidth || 
        height != m_previousHeight || 
        stride != m_previousStride) {
        
        DirtyRect fullFrame;
        fullFrame.x = 0;
        fullFrame.y = 0;
        fullFrame.width = width;
        fullFrame.height = height;
        
        return { fullFrame };
    }
    
    // Implementação simplificada, dividindo a tela em grades
    const int GRID_SIZE = 16;
    const int blockWidth = (width + GRID_SIZE - 1) / GRID_SIZE;
    const int blockHeight = (height + GRID_SIZE - 1) / GRID_SIZE;
    
    std::vector<DirtyRect> dirtyRects;
    
    for (int y = 0; y < height; y += blockHeight) {
        for (int x = 0; x < width; x += blockWidth) {
            int blockW = std::min(blockWidth, width - x);
            int blockH = std::min(blockHeight, height - y);
            
            bool isDirty = false;
            
            // Verificar se este bloco tem pixels diferentes
            for (int by = 0; by < blockH && !isDirty; ++by) {
                for (int bx = 0; bx < blockW && !isDirty; ++bx) {
                    int px = x + bx;
                    int py = y + by;
                    
                    int offset = py * stride + px * 4; // 4 bytes por pixel (BGRA/RGBA)
                    
                    // Comparar pixels
                    if (offset + 3 < currentFrame.size() && offset + 3 < previousFrame.size()) {
                        if (currentFrame[offset] != previousFrame[offset] ||
                            currentFrame[offset + 1] != previousFrame[offset + 1] ||
                            currentFrame[offset + 2] != previousFrame[offset + 2]) {
                            isDirty = true;
                        }
                    }
                }
            }
            
            if (isDirty) {
                DirtyRect rect;
                rect.x = x;
                rect.y = y;
                rect.width = blockW;
                rect.height = blockH;
                dirtyRects.push_back(rect);
            }
        }
    }
    
    return dirtyRects;
}

// Métodos auxiliares
void ScreenCapturer::captureCursor(CapturedFrame& frame) {
    // TODO: Implementar quando tivermos as implementações específicas de plataforma
}

} // namespace communication
} // namespace wydbr
