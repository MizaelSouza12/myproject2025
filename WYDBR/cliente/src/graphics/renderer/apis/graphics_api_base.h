#ifndef WYDBR_GRAPHICS_API_BASE_H
#define WYDBR_GRAPHICS_API_BASE_H

#include <string>
#include <memory>
#include <vector>

#include "../../renderer/core/ultra_optimized_renderer.h"

namespace wydbr {
namespace graphics {

/**
 * @class GraphicsAPIBase
 * @brief Interface base para todas as implementações de API gráfica
 * 
 * Esta classe define a interface comum que todas as APIs gráficas
 * (DirectX, Vulkan, OpenGL) devem implementar para serem usadas
 * pelo sistema de renderização.
 */
class GraphicsAPIBase {
public:
    virtual ~GraphicsAPIBase() = default;
    
    /**
     * @brief Inicializa a API gráfica
     * @param config Configuração de renderização
     * @return true se inicializada com sucesso
     */
    virtual bool initialize(const RenderConfig& config) = 0;
    
    /**
     * @brief Desliga a API gráfica e libera recursos
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Inicia o processo de renderização de um frame
     */
    virtual void beginFrame() = 0;
    
    /**
     * @brief Finaliza o processo de renderização de um frame
     */
    virtual void endFrame() = 0;
    
    /**
     * @brief Limpa o buffer de cor e profundidade
     * @param r Componente vermelho (0-1)
     * @param g Componente verde (0-1)
     * @param b Componente azul (0-1)
     * @param a Componente alpha (0-1)
     */
    virtual void clearScreen(float r, float g, float b, float a) = 0;
    
    /**
     * @brief Obtém capacidades e informações da API
     * @return Informações sobre o hardware gráfico
     */
    virtual GraphicsHardwareInfo getCapabilities() = 0;
    
    /**
     * @brief Verifica se a API está disponível no sistema
     * @return true se disponível
     */
    virtual bool isAvailable() = 0;
    
    /**
     * @brief Recupera o nome da API
     * @return Nome da API (ex: "DirectX 11")
     */
    virtual std::string getName() = 0;
    
    /**
     * @brief Obtém estatísticas de renderização específicas da API
     * @return Estatísticas de renderização
     */
    virtual RenderStats getStats() = 0;
    
    /**
     * @brief Cria um recurso de textura
     * @param width Largura em pixels
     * @param height Altura em pixels
     * @param format Formato da textura
     * @param useMipmaps Se deve gerar mipmaps
     * @return ID do recurso criado ou 0 se falhar
     */
    virtual uint32_t createTexture(int width, int height, int format, bool useMipmaps) = 0;
    
    /**
     * @brief Cria um buffer de vértices
     * @param sizeBytes Tamanho em bytes
     * @param dynamic Se o buffer será atualizado frequentemente
     * @return ID do recurso criado ou 0 se falhar
     */
    virtual uint32_t createVertexBuffer(size_t sizeBytes, bool dynamic) = 0;
    
    /**
     * @brief Cria um buffer de índices
     * @param sizeBytes Tamanho em bytes
     * @param dynamic Se o buffer será atualizado frequentemente
     * @return ID do recurso criado ou 0 se falhar
     */
    virtual uint32_t createIndexBuffer(size_t sizeBytes, bool dynamic) = 0;
    
    /**
     * @brief Aplica otimizações específicas da API
     * @param config Configuração atual
     */
    virtual void applyOptimizations(const RenderConfig& config) = 0;
    
    /**
     * @brief Define a resolução de renderização
     * @param width Largura em pixels
     * @param height Altura em pixels
     * @param fullscreen Se deve usar modo de tela cheia
     * @return true se a mudança foi bem-sucedida
     */
    virtual bool setResolution(int width, int height, bool fullscreen) = 0;
    
    /**
     * @brief Define o modo de sincronização vertical
     * @param enabled Se vsync deve estar ativado
     */
    virtual void setVSync(bool enabled) = 0;
    
    /**
     * @brief Verifica e corrige configurações de renderização para compatibilidade
     * @param config Configuração a ser verificada e corrigida
     * @return Configuração corrigida
     */
    virtual RenderConfig validateConfig(const RenderConfig& config) = 0;
    
    /**
     * @brief Cria objetos de estado de renderização
     * @param debugName Nome para debugging
     * @return ID do estado ou 0 se falhar
     */
    virtual uint32_t createRenderState(const std::string& debugName) = 0;
    
    /**
     * @brief Define o estado de renderização atual
     * @param stateId ID do estado a ser usado
     */
    virtual void setRenderState(uint32_t stateId) = 0;
    
    /**
     * @brief Libera um recurso criado pela API
     * @param resourceId ID do recurso
     */
    virtual void releaseResource(uint32_t resourceId) = 0;
    
    /**
     * @brief Apresenta o buffer de trás na tela
     */
    virtual void present() = 0;
    
    /**
     * @brief Recupera o tipo de API
     * @return Tipo da API
     */
    virtual GraphicsAPI getAPIType() = 0;
    
    /**
     * @brief Cria um shader
     * @param shaderCode Código do shader
     * @param shaderType Tipo do shader (vertex, pixel, etc.)
     * @return ID do shader ou 0 se falhar
     */
    virtual uint32_t createShader(const std::string& shaderCode, int shaderType) = 0;
    
    /**
     * @brief Cria uma pipeline de renderização
     * @param vertexShaderId ID do vertex shader
     * @param pixelShaderId ID do pixel shader
     * @return ID da pipeline ou 0 se falhar
     */
    virtual uint32_t createPipeline(uint32_t vertexShaderId, uint32_t pixelShaderId) = 0;
};

// Factory para criar implementações específicas de API
std::unique_ptr<GraphicsAPIBase> createGraphicsAPI(GraphicsAPI api);

} // namespace graphics
} // namespace wydbr

#endif // WYDBR_GRAPHICS_API_BASE_H
