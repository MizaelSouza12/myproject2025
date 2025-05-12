#include "ultra_optimized_renderer.h"
#include "ultra_optimized_renderer_impl.h"
#include <iostream>

namespace wydbr {
namespace graphics {

// Implementação do singleton
UltraOptimizedRenderer& UltraOptimizedRenderer::getInstance() {
    static UltraOptimizedRenderer instance;
    return instance;
}

// Construtor
UltraOptimizedRenderer::UltraOptimizedRenderer() : pImpl(std::make_unique<Impl>()) {
    // Construtor privado - usar getInstance()
}

// Destrutor
UltraOptimizedRenderer::~UltraOptimizedRenderer() {
    // Destrutor definido aqui para permitir tipo incompleto no header
}

// Inicialização
bool UltraOptimizedRenderer::initialize(const RenderConfig& config) {
    return pImpl->initialize(config);
}

// Shutdown
void UltraOptimizedRenderer::shutdown() {
    pImpl->shutdown();
}

// Detecção de hardware
GraphicsHardwareInfo UltraOptimizedRenderer::detectHardware() {
    return pImpl->detectHardware();
}

// Auto-detecção de configuração
RenderConfig UltraOptimizedRenderer::autoDetectBestConfig() {
    return pImpl->autoDetectBestConfig();
}

// Atualização de configuração
bool UltraOptimizedRenderer::updateConfig(const RenderConfig& config) {
    return pImpl->updateConfig(config);
}

// Configurações adaptativas
void UltraOptimizedRenderer::applyAdaptiveSettings() {
    pImpl->applyAdaptiveSettings();
}

// Estatísticas
RenderStats UltraOptimizedRenderer::getStats() const {
    return pImpl->getStats();
}

// Callbacks de estatísticas
int UltraOptimizedRenderer::addStatsCallback(RenderStatsCallback callback) {
    return pImpl->addStatsCallback(callback);
}

bool UltraOptimizedRenderer::removeStatsCallback(int callbackId) {
    return pImpl->removeStatsCallback(callbackId);
}

// Otimização dinâmica
void UltraOptimizedRenderer::setDynamicOptimization(bool enabled) {
    pImpl->setDynamicOptimization(enabled);
}

// Configuração atual
const RenderConfig& UltraOptimizedRenderer::getCurrentConfig() const {
    return pImpl->getCurrentConfig();
}

// Limite de FPS personalizado
void UltraOptimizedRenderer::setCustomFramerateCap(int fps) {
    pImpl->setCustomFramerateCap(fps);
}

// Otimizações específicas para GPU
void UltraOptimizedRenderer::applyGPUSpecificOptimizations(const std::string& gpuVendor, const std::string& gpuModel) {
    pImpl->applyGPUSpecificOptimizations(gpuVendor, gpuModel);
}

} // namespace graphics
} // namespace wydbr
