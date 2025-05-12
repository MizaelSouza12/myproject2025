#include "graphics_api_base.h"
#include "directx9/directx9_api.h"
// Incluir outros headers de API à medida que forem implementados
// #include "directx11/directx11_api.h"
// #include "vulkan/vulkan_api.h"
// #include "opengl/opengl_api.h"

namespace wydbr {
namespace graphics {

/**
 * @brief Cria uma instância da API gráfica solicitada
 * @param api Tipo de API a ser criada
 * @return Ponteiro único para a implementação da API, ou nullptr se não suportada
 */
std::unique_ptr<GraphicsAPIBase> createGraphicsAPI(GraphicsAPI api) {
    switch (api) {
        case GraphicsAPI::DIRECTX_9:
            return std::make_unique<DirectX9API>();
            
        case GraphicsAPI::DIRECTX_11:
            // Seria implementado quando tivermos a classe DirectX11API
            // return std::make_unique<DirectX11API>();
            return nullptr;
            
        case GraphicsAPI::DIRECTX_12:
            // Seria implementado quando tivermos a classe DirectX12API
            // return std::make_unique<DirectX12API>();
            return nullptr;
            
        case GraphicsAPI::VULKAN:
            // Seria implementado quando tivermos a classe VulkanAPI
            // return std::make_unique<VulkanAPI>();
            return nullptr;
            
        case GraphicsAPI::OPENGL:
            // Seria implementado quando tivermos a classe OpenGLAPI
            // return std::make_unique<OpenGLAPI>();
            return nullptr;
            
        case GraphicsAPI::SOFTWARE:
            // Seria implementado quando tivermos a classe SoftwareAPI
            // return std::make_unique<SoftwareAPI>();
            return nullptr;
            
        default:
            // API não suportada
            return nullptr;
    }
}

} // namespace graphics
} // namespace wydbr
