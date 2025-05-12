#ifndef WYDBR_DIRECTX9_API_H
#define WYDBR_DIRECTX9_API_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>

#include "../graphics_api_base.h"

// Declarações antecipadas para evitar inclusão de windows.h/d3d9.h neste header
struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DSurface9;
struct IDirect3DTexture9;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;
struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;
struct ID3DXEffect;
struct D3DPRESENT_PARAMETERS;

namespace wydbr {
namespace graphics {

/**
 * @class DirectX9API
 * @brief Implementação da API gráfica usando DirectX 9
 * 
 * Esta classe implementa a interface GraphicsAPIBase usando DirectX 9,
 * priorizando máxima compatibilidade com hardware antigo.
 */
class DirectX9API : public GraphicsAPIBase {
public:
    DirectX9API();
    ~DirectX9API() override;
    
    bool initialize(const RenderConfig& config) override;
    void shutdown() override;
    
    void beginFrame() override;
    void endFrame() override;
    void clearScreen(float r, float g, float b, float a) override;
    
    GraphicsHardwareInfo getCapabilities() override;
    bool isAvailable() override;
    std::string getName() override;
    RenderStats getStats() override;
    
    uint32_t createTexture(int width, int height, int format, bool useMipmaps) override;
    uint32_t createVertexBuffer(size_t sizeBytes, bool dynamic) override;
    uint32_t createIndexBuffer(size_t sizeBytes, bool dynamic) override;
    
    void applyOptimizations(const RenderConfig& config) override;
    bool setResolution(int width, int height, bool fullscreen) override;
    void setVSync(bool enabled) override;
    
    RenderConfig validateConfig(const RenderConfig& config) override;
    uint32_t createRenderState(const std::string& debugName) override;
    void setRenderState(uint32_t stateId) override;
    void releaseResource(uint32_t resourceId) override;
    void present() override;
    
    GraphicsAPI getAPIType() override;
    uint32_t createShader(const std::string& shaderCode, int shaderType) override;
    uint32_t createPipeline(uint32_t vertexShaderId, uint32_t pixelShaderId) override;
    
private:
    // Métodos de implementação interna
    bool createDevice(const RenderConfig& config);
    bool resetDevice(int width, int height, bool fullscreen);
    void setupRenderStates();
    void detectCapabilities();
    uint32_t generateResourceId();
    void releaseAllResources();
    void configureShaderCompiler();
    
    // Helpers para conversão de enums para tipos DirectX
    int getD3DFormat(int format);
    int getD3DUsage(bool dynamic);
    int getD3DShaderType(int shaderType);
    
    // Objeto DirectX
    IDirect3D9* m_d3d;
    IDirect3DDevice9* m_device;
    D3DPRESENT_PARAMETERS* m_presentParams;
    
    // Estado e recursos
    bool m_initialized;
    RenderConfig m_config;
    GraphicsHardwareInfo m_capabilities;
    RenderStats m_stats;
    uint32_t m_nextResourceId;
    std::mutex m_resourceMutex;
    
    // Mapeamento de recursos
    std::unordered_map<uint32_t, IDirect3DTexture9*> m_textures;
    std::unordered_map<uint32_t, IDirect3DVertexBuffer9*> m_vertexBuffers;
    std::unordered_map<uint32_t, IDirect3DIndexBuffer9*> m_indexBuffers;
    std::unordered_map<uint32_t, IDirect3DVertexShader9*> m_vertexShaders;
    std::unordered_map<uint32_t, IDirect3DPixelShader9*> m_pixelShaders;
    std::unordered_map<uint32_t, ID3DXEffect*> m_effects;
    std::unordered_map<uint32_t, void*> m_renderStates;
    
    // Estado de renderização atual
    uint32_t m_currentState;
    bool m_vsyncEnabled;
    int m_width;
    int m_height;
    bool m_isFullscreen;
    
    // Estatísticas e otimização
    size_t m_drawCallCount;
    size_t m_triangleCount;
    size_t m_batchCount;
    size_t m_textureSwapCount;
    bool m_supportsShaders;
    bool m_supportsMipMapping;
    int m_maxTextureSize;
};

} // namespace graphics
} // namespace wydbr

#endif // WYDBR_DIRECTX9_API_H
