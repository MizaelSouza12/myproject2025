#ifndef WYDBR_ULTRA_OPTIMIZED_RENDERER_IMPL_H
#define WYDBR_ULTRA_OPTIMIZED_RENDERER_IMPL_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <chrono>
#include <algorithm>
#include <bitset>

#include "ultra_optimized_renderer.h"

namespace wydbr {
namespace graphics {

/**
 * @brief Implementação interna do UltraOptimizedRenderer
 */
class UltraOptimizedRenderer::Impl {
public:
    Impl();
    ~Impl();
    
    bool initialize(const RenderConfig& config);
    void shutdown();
    
    GraphicsHardwareInfo detectHardware();
    RenderConfig autoDetectBestConfig();
    bool updateConfig(const RenderConfig& config);
    void applyAdaptiveSettings();
    RenderStats getStats() const;
    int addStatsCallback(RenderStatsCallback callback);
    bool removeStatsCallback(int callbackId);
    void setDynamicOptimization(bool enabled);
    const RenderConfig& getCurrentConfig() const;
    void setCustomFramerateCap(int fps);
    void applyGPUSpecificOptimizations(const std::string& gpuVendor, const std::string& gpuModel);

private:
    // Métodos internos
    bool initializeGraphicsAPI();
    void updateRenderStats();
    bool selectBestAPI();
    void applyLowEndOptimizations();
    void monitorPerformance();
    void startPerformanceMonitoringThread();
    void stopPerformanceMonitoringThread();
    void adjustDynamicSettings(float currentFPS, float targetFPS);
    void notifyStatsCallbacks();
    void applyHardwareSpecificFixes();
    
    // Objetos e estado interno
    RenderConfig m_config;
    GraphicsHardwareInfo m_hardwareInfo;
    RenderStats m_stats;
    std::vector<std::pair<int, RenderStatsCallback>> m_statsCallbacks;
    int m_nextCallbackId;
    std::mutex m_callbackMutex;
    
    bool m_initialized;
    bool m_dynamicOptimizationEnabled;
    int m_customFramerateCap;
    
    // Threads e performance
    std::unique_ptr<std::thread> m_perfMonitorThread;
    std::atomic<bool> m_perfMonitorRunning;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastFrameTime;
    std::vector<float> m_frameTimeHistory;
    
    // API e interface gráfica
    std::unique_ptr<class GraphicsAPIBase> m_graphicsAPI;
    GraphicsAPI m_selectedAPI;
    
    // Módulos específicos de API
    std::unique_ptr<class DirectX9Module> m_dx9Module;
    std::unique_ptr<class DirectX11Module> m_dx11Module;
    std::unique_ptr<class VulkanModule> m_vulkanModule;
    std::unique_ptr<class OpenGLModule> m_openglModule;
    
    // Sistema de otimizações
    std::unique_ptr<class OcclusionSystem> m_occlusionSystem;
    std::unique_ptr<class DynamicResolutionSystem> m_dynamicResSystem;
    std::unique_ptr<class BatchOptimizer> m_batchOptimizer;
    std::unique_ptr<class LODManager> m_lodManager;
};

} // namespace graphics
} // namespace wydbr

#endif // WYDBR_ULTRA_OPTIMIZED_RENDERER_IMPL_H
