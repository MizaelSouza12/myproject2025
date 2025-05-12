/**
 * RenderingSystem.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/reverse/renderer/RenderingSystem.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef RENDERINGSYSTEM_H
#define RENDERINGSYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <array>
#include <bitset>
#include <mutex>
#include <atomic>
#include <cstdint>

namespace wydbr {


/**
 * @file RenderingSystem.h
 * @brief Sistema avançado de renderização 3D reconstruído do WYD original
 * 
 * Este arquivo contém a reconstrução do sistema de renderização 3D original do WYD,
 * incluindo o pipeline gráfico, gerenciamento de shaders, texturas, malhas e
 * efeitos de iluminação. Sistema foi reconstruído através da engenharia reversa
 * do executável original, mantendo total compatibilidade.
 */

namespace wyd {
namespace reverse {
namespace renderer {

// Tipos de dados para compatibilidade com o WYD Original
using WYDVertexID = uint32_t;
using WYDIndexID = uint32_t;
using WYDTextureID = uint32_t;
using WYDMeshID = uint32_t;
using WYDShaderID = uint32_t;
using WYDBufferID = uint32_t;
using WYDEffectID = uint32_t;

// Constantes originais do sistema de renderização
constexpr size_t MAX_VERTEX_BUFFERS = 256;
constexpr size_t MAX_INDEX_BUFFERS = 256;
constexpr size_t MAX_TEXTURES = 2048;
constexpr size_t MAX_MESHES = 2048;
constexpr size_t MAX_SHADERS = 128;
constexpr size_t MAX_EFFECTS = 256;
constexpr size_t MAX_RENDER_TARGETS = 8;
constexpr size_t MAX_LIGHTS = 32;
constexpr size_t TEXTURE_CACHE_SIZE = 256 * 1024 * 1024; // 256 MB
constexpr size_t MESH_CACHE_SIZE = 128 * 1024 * 1024; // 128 MB

/**
 * @brief Enumeração de formatos de vértice
 */
enum class VertexFormat : uint32_t {
    POSITION = 0x0001,                 // Posição (x, y, z)
    NORMAL = 0x0002,                   // Normal (nx, ny, nz)
    COLOR = 0x0004,                    // Cor (r, g, b, a)
    TEXCOORD0 = 0x0008,                // Coordenada de textura 0 (u, v)
    TEXCOORD1 = 0x0010,                // Coordenada de textura 1 (u, v)
    TEXCOORD2 = 0x0020,                // Coordenada de textura 2 (u, v)
    TEXCOORD3 = 0x0040,                // Coordenada de textura 3 (u, v)
    TANGENT = 0x0080,                  // Tangente (tx, ty, tz)
    BINORMAL = 0x0100,                 // Binormal (bx, by, bz)
    BLENDINDICES = 0x0200,             // Índices de blend (i0, i1, i2, i3)
    BLENDWEIGHT = 0x0400,              // Pesos de blend (w0, w1, w2, w3)
    POSITION2D = 0x0800,               // Posição 2D (x, y)
    CUSTOM = 0x1000,                   // Personalizado
};

/**
 * @brief Enumeração de tipos de primitivas
 */
enum class PrimitiveType : uint32_t {
    POINTLIST = 1,                     // Lista de pontos
    LINELIST = 2,                      // Lista de linhas
    LINESTRIP = 3,                     // Tira de linhas
    TRIANGLELIST = 4,                  // Lista de triângulos
    TRIANGLESTRIP = 5,                 // Tira de triângulos
    TRIANGLEFAN = 6,                   // Leque de triângulos
};

/**
 * @brief Enumeração de formatos de textura
 */
enum class TextureFormat : uint32_t {
    UNKNOWN = 0,                       // Desconhecido
    R8G8B8 = 20,                       // 24-bit RGB
    A8R8G8B8 = 21,                     // 32-bit ARGB
    X8R8G8B8 = 22,                     // 32-bit RGB
    R5G6B5 = 23,                       // 16-bit RGB
    X1R5G5B5 = 24,                     // 15-bit RGB
    A1R5G5B5 = 25,                     // 16-bit ARGB
    A4R4G4B4 = 26,                     // 16-bit ARGB
    R3G3B2 = 27,                       // 8-bit RGB
    A8 = 28,                           // 8-bit alpha
    A8R3G3B2 = 29,                     // 16-bit ARGB
    X4R4G4B4 = 30,                     // 12-bit RGB
    A8P8 = 40,                         // 8-bit alpha, 8-bit palette
    P8 = 41,                           // 8-bit palette
    L8 = 50,                           // 8-bit luminance
    A8L8 = 51,                         // 16-bit alpha, luminance
    A4L4 = 52,                         // 8-bit alpha, luminance
    V8U8 = 60,                         // 16-bit bump-map
    L6V5U5 = 61,                       // 16-bit bump-map with luminance
    X8L8V8U8 = 62,                     // 32-bit bump-map
    Q8W8V8U8 = 63,                     // 32-bit bump-map
    V16U16 = 64,                       // 32-bit bump-map
    A2W10V10U10 = 67,                  // 32-bit bump-map with alpha
    D16_LOCKABLE = 70,                 // 16-bit depth
    D32 = 71,                          // 32-bit depth
    D15S1 = 73,                        // 16-bit depth, 1-bit stencil
    D24S8 = 75,                        // 24-bit depth, 8-bit stencil
    D24X8 = 77,                        // 24-bit depth
    D24X4S4 = 79,                      // 24-bit depth, 4-bit stencil
    D16 = 80,                          // 16-bit depth
    L16 = 81,                          // 16-bit luminance
    DXT1 = 827611204,                  // DXT1 compressed texture
    DXT2 = 844388420,                  // DXT2 compressed texture
    DXT3 = 861165636,                  // DXT3 compressed texture
    DXT4 = 877942852,                  // DXT4 compressed texture
    DXT5 = 894720068,                  // DXT5 compressed texture
};

/**
 * @brief Enumeração de tipos de filtro de textura
 */
enum class TextureFilter : uint32_t {
    NONE = 0,                          // Sem filtro
    POINT = 1,                         // Filtro de ponto
    LINEAR = 2,                        // Filtro linear
    ANISOTROPIC = 3,                   // Filtro anisotrópico
    PYRAMIDALQUAD = 4,                 // Filtro quadrado piramidal
    GAUSSIANQUAD = 5,                  // Filtro quadrado gaussiano
    CONVOLUTIONMONO = 6,               // Filtro mono de convolução
};

/**
 * @brief Enumeração de modos de endereçamento de textura
 */
enum class TextureAddressMode : uint32_t {
    WRAP = 1,                          // Repetir
    MIRROR = 2,                        // Espelhar
    CLAMP = 3,                         // Fixar
    BORDER = 4,                        // Borda
    MIRRORONCE = 5,                    // Espelhar uma vez
};

/**
 * @brief Enumeração de tipos de shaders
 */
enum class ShaderType : uint32_t {
    VERTEX = 0,                        // Vertex shader
    PIXEL = 1,                         // Pixel shader
    EFFECT = 2,                        // Efeito
};

/**
 * @brief Enumeração de tipos de luzes
 */
enum class LightType : uint32_t {
    POINT = 1,                         // Luz pontual
    SPOT = 2,                          // Luz spot
    DIRECTIONAL = 3,                   // Luz direcional
};

/**
 * @brief Estrutura de uma matriz 4x4
 */
struct Matrix4x4 {
    float m[4][4];                     // Matriz /**
 * Matrix4x4
 * 
 * Implementa a funcionalidade Matrix4x4 conforme especificação original.
 * @return Retorna 4x4
 */
 4x4
    
    Matrix4x4(){
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }
};

/**
 * @brief Estrutura de um vetor 2D
 */
struct Vector2 {
    float x, y;                        // Componentes x, y
    
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}
};

/**
 * @brief Estrutura de um vetor 3D
 */
struct Vector3 {
    float x, y, z;                     // Componentes x, y, z
    
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

/**
 * @brief Estrutura de um vetor 4D
 */
struct Vector4 {
    float x, y, z, w;                  // Componentes x, y, z, w
    
    Vector4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

/**
 * @brief Estrutura de um quaternion
 */
struct Quaternion {
    float x, y, z, w;                  // Componentes x, y, z, w
    
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

/**
 * @brief Estrutura de uma cor
 */
struct Color {
    float r, g, b, a;                  // Componentes r, g, b, a
    
    Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
};

/**
 * @brief Estrutura de uma luz
 */
struct Light {
    LightType type;                    // Tipo de luz
    Vector3 position;                  // Posição
    Vector3 direction;                 // Direção
    Color diffuse;                     // Cor difusa
    Color specular;                    // Cor especular
    Color ambient;                     // Cor ambiente
    float range;                       // Alcance
    float falloff;                     // Queda
    float attenuation0;                // Atenuação constante
    float attenuation1;                // Atenuação linear
    float attenuation2;                // Atenuação quadrática
    float theta;                       // Ângulo interno do cone
    float phi;                         // Ângulo externo do cone
    
    Light() 
        : type(LightType::POINT)
        , position()
        , direction(0.0f, 0.0f, 1.0f)
        , diffuse(1.0f, 1.0f, 1.0f, 1.0f)
        , specular(0.0f, 0.0f, 0.0f, 0.0f)
        , ambient(0.0f, 0.0f, 0.0f, 0.0f)
        , range(1000.0f)
        , falloff(1.0f)
        , attenuation0(1.0f)
        , attenuation1(0.0f)
        , attenuation2(0.0f)
        , theta(0.0f)
        , phi(0.0f)
    {}
};

/**
 * @brief Estrutura de um material
 */
struct Material {
    Color diffuse;                     // Cor difusa
    Color ambient;                     // Cor ambiente
    Color specular;                    // Cor especular
    Color emissive;                    // Cor emissiva
    float power;                       // Potência especular
    
    Material()
        : diffuse(1.0f, 1.0f, 1.0f, 1.0f)
        , ambient(0.2f, 0.2f, 0.2f, 1.0f)
        , specular(0.0f, 0.0f, 0.0f, 0.0f)
        , emissive(0.0f, 0.0f, 0.0f, 0.0f)
        , power(0.0f)
    {}
};

/**
 * @brief Estrutura de uma câmera
 */
struct Camera {
    Vector3 position;                  // Posição
    Vector3 target;                    // Alvo
    Vector3 up;                        // Vetor up
    float fov;                         // Campo de visão
    float aspectRatio;                 // Proporção de aspecto
    float nearPlane;                   // Plano próximo
    float farPlane;                    // Plano distante
    Matrix4x4 view;                    // Matriz de visão
    Matrix4x4 projection;              // Matriz de projeção
    
    Camera()
        : position(0.0f, 0.0f, -5.0f)
        , target(0.0f, 0.0f, 0.0f)
        , up(0.0f, 1.0f, 0.0f)
        , fov(60.0f)
        , aspectRatio(1.333f)
        , nearPlane(1.0f)
        , farPlane(1000.0f)
        , view()
        , projection()
    {}
};

/**
 * @brief Estrutura de um vértice
 */
struct Vertex {
    Vector3 position;                  // Posição
    Vector3 normal;                    // Normal
    Vector2 texCoord;                  // Coordenada de textura
    Color color;                       // Cor
    
    Vertex()
        : position()
        , normal(0.0f, 0.0f, 1.0f)
        , texCoord()
        , color(1.0f, 1.0f, 1.0f, 1.0f)
    {}
};

/**
 * @brief Estrutura de um triângulo
 */
struct Triangle {
    uint32_t indices[3];               // Índices dos vértices
    
    Triangle() : indices{0, 0, 0} {}
    Triangle(uint32_t i0, uint32_t i1, uint32_t i2) : indices{i0, i1, i2} {}
};

/**
 * @brief Estrutura de um subconjunto de malha
 */
struct MeshSubset {
    uint32_t startIndex;               // Índice inicial
    uint32_t indexCount;               // Contagem de índices
    uint32_t startVertex;              // Vértice inicial
    uint32_t vertexCount;              // Contagem de vértices
    WYDTextureID textureID;            // ID da textura
    Material material;                 // Material
    
    MeshSubset()
        : startIndex(0)
        , indexCount(0)
        , startVertex(0)
        , vertexCount(0)
        , textureID(0)
        , material()
    {}
};

/**
 * @brief Estrutura de uma malha
 */
struct Mesh {
    std::vector<Vertex> vertices;      // Vértices
    std::vector<uint32_t> indices;     // Índices
    std::vector<MeshSubset> subsets;   // Subconjuntos
    Vector3 boundingBoxMin;            // Caixa delimitadora mínima
    Vector3 boundingBoxMax;            // Caixa delimitadora máxima
    float boundingSphereRadius;        // Raio da esfera delimitadora
    
    Mesh()
        : vertices()
        , indices()
        , subsets()
        , boundingBoxMin(0.0f, 0.0f, 0.0f)
        , boundingBoxMax(0.0f, 0.0f, 0.0f)
        , boundingSphereRadius(0.0f)
    {}
};

/**
 * @brief Estrutura de um parâmetro de shader
 */
struct ShaderParameter {
    std::string name;                  // Nome
    uint32_t register_;                // Registrador
    uint32_t size;                     // Tamanho
    std::vector<float> data;           // Dados
    
    ShaderParameter()
        : name("")
        , register_(0)
        , size(0)
        , data()
    {}
};

/**
 * @brief Estrutura de um shader
 */
struct Shader {
    ShaderType type;                   // Tipo
    std::vector<uint8_t> byteCode;     // Código de bytes
    std::vector<ShaderParameter> parameters; // Parâmetros
    
    Shader()
        : type(ShaderType::VERTEX)
        , byteCode()
        , parameters()
    {}
};

/**
 * @brief Estrutura de um passe de efeito
 */
struct EffectPass {
    std::string name;                  // Nome
    WYDShaderID vertexShaderID;        // ID do vertex shader
    WYDShaderID pixelShaderID;         // ID do pixel shader
    std::vector<ShaderParameter> parameters; // Parâmetros
    
    EffectPass()
        : name("")
        , vertexShaderID(0)
        , pixelShaderID(0)
        , parameters()
    {}
};

/**
 * @brief Estrutura de uma técnica de efeito
 */
struct EffectTechnique {
    std::string name;                  // Nome
    std::vector<EffectPass> passes;    // Passes
    
    EffectTechnique()
        : name("")
        , passes()
    {}
};

/**
 * @brief Estrutura de um efeito
 */
struct Effect {
    std::string name;                  // Nome
    std::vector<EffectTechnique> techniques; // Técnicas
    std::vector<ShaderParameter> parameters; // Parâmetros
    
    Effect()
        : name("")
        , techniques()
        , parameters()
    {}
};

/**
 * @brief Estrutura de um quadro de renderização
 */
struct RenderFrame {
    uint32_t frameIndex;               // Índice do quadro
    float deltaTime;                   // Tempo delta
    Camera camera;                     // Câmera
    std::array<Light, MAX_LIGHTS> lights; // Luzes
    uint32_t lightCount;               // Contagem de luzes
    Matrix4x4 world;                   // Matriz de mundo
    
    RenderFrame()
        : frameIndex(0)
        , deltaTime(0.0f)
        , camera()
        , lights()
        , lightCount(0)
        , world()
    {}
};

/**
 * @brief Estrutura de estatísticas de renderização
 */
struct RenderStats {
    uint32_t frameCount;               // Contagem de quadros
    uint32_t drawCalls;                // Chamadas de desenho
    uint32_t triangleCount;            // Contagem de triângulos
    uint32_t vertexCount;              // Contagem de vértices
    uint32_t shaderSwitches;           // Trocas de shader
    uint32_t textureSwitches;          // Trocas de textura
    float frameTime;                   // Tempo de quadro
    float cpuTime;                     // Tempo de CPU
    float gpuTime;                     // Tempo de GPU
    
    RenderStats()
        : frameCount(0)
        , drawCalls(0)
        , triangleCount(0)
        , vertexCount(0)
        , shaderSwitches(0)
        , textureSwitches(0)
        , frameTime(0.0f)
        , cpuTime(0.0f)
        , gpuTime(0.0f)
    {}
    
    // Reinicia as estatísticas por quadro
    /**
 * Reset
 * 
 * Implementa a funcionalidade Reset conforme especificação original.
 */

    void Reset(){
        drawCalls = 0;
        triangleCount = 0;
        vertexCount = 0;
        shaderSwitches = 0;
        textureSwitches = 0;
        frameTime = 0.0f;
        cpuTime = 0.0f;
        gpuTime = 0.0f;
    }
};

/**
 * @brief Interface de dispositivo de renderização
 */
/**
 * Classe WYDIRenderDevice
 * 
 * Esta classe implementa a funcionalidade WYDIRenderDevice conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDIRenderDevice
 * 
 * Esta classe implementa a funcionalidade WYDIRenderDevice conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class IRenderDevice {
public:
    virtual ~IRenderDevice() {}

    // Inicialização e finalização
    virtual bool Initialize(void* windowHandle, uint32_t width, uint32_t height, bool fullscreen) = 0;
    virtual void Shutdown() = 0;

    // Gerenciamento de recursos
    virtual WYDVertexID CreateVertexBuffer(const void* data, uint32_t size, uint32_t stride, bool dynamic) = 0;
    virtual WYDIndexID CreateIndexBuffer(const void* data, uint32_t size, uint32_t stride, bool dynamic) = 0;
    virtual WYDTextureID CreateTexture(uint32_t width, uint32_t height, TextureFormat format, bool mipmap) = 0;
    virtual WYDTextureID LoadTexture(const std::string& filename) = 0;
    virtual WYDMeshID CreateMesh(const Mesh& mesh) = 0;
    virtual WYDMeshID LoadMesh(const std::string& filename) = 0;
    virtual WYDShaderID CreateShader(ShaderType type, const void* shaderCode, uint32_t codeSize) = 0;
    virtual WYDEffectID CreateEffect(const Effect& effect) = 0;

    // Liberação de recursos
    virtual void ReleaseVertexBuffer(WYDVertexID id) = 0;
    virtual void ReleaseIndexBuffer(WYDIndexID id) = 0;
    virtual void ReleaseTexture(WYDTextureID id) = 0;
    virtual void ReleaseMesh(WYDMeshID id) = 0;
    virtual void ReleaseShader(WYDShaderID id) = 0;
    virtual void ReleaseEffect(WYDEffectID id) = 0;

    // Estados de renderização
    virtual void SetRenderState(uint32_t state, uint32_t value) = 0;
    virtual void SetTexture(uint32_t stage, WYDTextureID id) = 0;
    virtual void SetTextureStageState(uint32_t stage, uint32_t state, uint32_t value) = 0;
    virtual void SetVertexShader(WYDShaderID id) = 0;
    virtual void SetPixelShader(WYDShaderID id) = 0;
    virtual void SetEffect(WYDEffectID id) = 0;
    virtual void SetEffectTechnique(WYDEffectID id, const std::string& techniqueName) = 0;
    virtual void SetEffectParameter(WYDEffectID id, const std::string& paramName, const void* data, uint32_t size) = 0;
    virtual void SetShaderParameter(WYDShaderID id, uint32_t register_, const void* data, uint32_t size) = 0;
    virtual void SetTransform(uint32_t transformType, const Matrix4x4& matrix) = 0;
    virtual void SetLight(uint32_t index, const Light& light) = 0;
    virtual void SetMaterial(const Material& material) = 0;

    // Comandos de desenho
    virtual void BeginScene() = 0;
    virtual void EndScene() = 0;
    virtual void Clear(uint32_t flags, Color color, float depth, uint32_t stencil) = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, float minZ, float maxZ) = 0;
    virtual void Present() = 0;
    virtual void DrawPrimitive(PrimitiveType type, uint32_t startVertex, uint32_t primitiveCount) = 0;
    virtual void DrawIndexedPrimitive(PrimitiveType type, uint32_t baseVertexIndex, uint32_t minVertexIndex, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount) = 0;
    virtual void DrawMesh(WYDMeshID id) = 0;

    // Consultas
    virtual void GetStats(RenderStats& stats) = 0;
    virtual bool GetDeviceCaps(uint32_t capsType, void* capsData) = 0;
    virtual bool IsTextureFormatSupported(TextureFormat format) = 0;
    virtual bool IsShaderModelSupported(uint32_t major, uint32_t minor) = 0;
};

/**
 * @brief Gerenciador do sistema de renderização
 */
/**
 * Classe WYDRenderingSystem
 * 
 * Esta classe implementa a funcionalidade WYDRenderingSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDRenderingSystem
 * 
 * Esta classe implementa a funcionalidade WYDRenderingSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class RenderingSystem {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static RenderingSystem& GetInstance();
    
    /**
     * @brief Inicializa o sistema de renderização
     * @param windowHandle Manipulador da janela
     * @param width Largura
     * @param height Altura
     * @param fullscreen Tela cheia
     * @return true se inicializado com sucesso
     */
    bool Initialize(void* windowHandle, uint32_t width, uint32_t height, bool fullscreen);
    
    /**
     * @brief Finaliza o sistema de renderização
     */
    void Shutdown();
    
    /**
     * @brief Obtém o dispositivo de renderização
     * @return Ponteiro para o dispositivo
     */
    IRenderDevice* GetDevice() const { return device_.get(); }
    
    /**
     * @brief Inicia um novo quadro de renderização
     * @param deltaTime Tempo delta
     * @return Referência para o quadro atual
     */
    RenderFrame& BeginFrame(float deltaTime);
    
    /**
     * @brief Termina o quadro atual de renderização
     */
    void EndFrame();
    
    /**
     * @brief Define a câmera atual
     * @param camera Câmera
     */
    void SetCamera(const Camera& camera);
    
    /**
     * @brief Adiciona uma luz ao quadro atual
     * @param light Luz
     * @return Índice da luz, ou -1 se falhar
     */
    int AddLight(const Light& light);
    
    /**
     * @brief Remove uma luz do quadro atual
     * @param index Índice da luz
     */
    void RemoveLight(uint32_t index);
    
    /**
     * @brief Desenha uma malha
     * @param meshID ID da malha
     * @param worldMatrix Matriz de mundo
     */
    void DrawMesh(WYDMeshID meshID, const Matrix4x4& worldMatrix);
    
    /**
     * @brief Renderiza a cena completa
     */
    void RenderScene();
    
    /**
     * @brief Carrega uma textura WYT
     * @param filename Nome do arquivo
     * @return ID da textura, ou 0 se falhar
     */
    WYDTextureID LoadWYTTexture(const std::string& filename);
    
    /**
     * @brief Carrega uma malha WYP
     * @param filename Nome do arquivo
     * @return ID da malha, ou 0 se falhar
     */
    WYDMeshID LoadWYPMesh(const std::string& filename);
    
    /**
     * @brief Salva uma textura no formato WYT
     * @param textureID ID da textura
     * @param filename Nome do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveWYTTexture(WYDTextureID textureID, const std::string& filename);
    
    /**
     * @brief Salva uma malha no formato WYP
     * @param meshID ID da malha
     * @param filename Nome do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveWYPMesh(WYDMeshID meshID, const std::string& filename);
    
    /**
     * @brief Obtém as estatísticas de renderização
     * @return Estatísticas
     */
    const RenderStats& GetStats() const { return stats_; }
    
    /**
     * @brief Redefine resolução e modo de exibição
     * @param width Largura
     * @param height Altura
     * @param fullscreen Tela cheia
     * @return true se redefinido com sucesso
     */
    bool ResetDisplay(uint32_t width, uint32_t height, bool fullscreen);
    
    /**
     * @brief Verifica se o sistema de renderização foi inicializado
     * @return true se inicializado
     */
    bool IsInitialized() const { return initialized_; }
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    RenderingSystem();
    
    /**
     * @brief Destrutor
     */
    ~RenderingSystem();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    RenderingSystem(const RenderingSystem&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    RenderingSystem& operator=(const RenderingSystem&) = delete;
    
    /**
     * @brief Cria o dispositivo de renderização apropriado
     * @return true se criado com sucesso
     */
    bool CreateDevice();
    
    /**
     * @brief Decodifica um cabeçalho de arquivo WYT
     * @param data Dados
     * @param size Tamanho
     * @param header Cabeçalho (saída)
     * @return true se decodificado com sucesso
     */
    bool DecodeWYTHeader(const uint8_t* data, uint32_t size, void* header);
    
    /**
     * @brief Decodifica um cabeçalho de arquivo WYP
     * @param data Dados
     * @param size Tamanho
     * @param header Cabeçalho (saída)
     * @return true se decodificado com sucesso
     */
    bool DecodeWYPHeader(const uint8_t* data, uint32_t size, void* header);
    
    /**
     * @brief Codifica um cabeçalho de arquivo WYT
     * @param header Cabeçalho
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se codificado com sucesso
     */
    bool EncodeWYTHeader(const void* header, uint8_t*& data, uint32_t& size);
    
    /**
     * @brief Codifica um cabeçalho de arquivo WYP
     * @param header Cabeçalho
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se codificado com sucesso
     */
    bool EncodeWYPHeader(const void* header, uint8_t*& data, uint32_t& size);
    
    /**
     * @brief Descomprime dados de textura WYT
     * @param compressedData Dados comprimidos
     * @param compressedSize Tamanho comprimido
     * @param uncompressedData Dados descomprimidos (saída)
     * @param uncompressedSize Tamanho descomprimido (saída)
     * @return true se descomprimido com sucesso
     */
    bool DecompressWYTData(const uint8_t* compressedData, uint32_t compressedSize, uint8_t*& uncompressedData, uint32_t& uncompressedSize);
    
    /**
     * @brief Descomprime dados de malha WYP
     * @param compressedData Dados comprimidos
     * @param compressedSize Tamanho comprimido
     * @param uncompressedData Dados descomprimidos (saída)
     * @param uncompressedSize Tamanho descomprimido (saída)
     * @return true se descomprimido com sucesso
     */
    bool DecompressWYPData(const uint8_t* compressedData, uint32_t compressedSize, uint8_t*& uncompressedData, uint32_t& uncompressedSize);
    
    /**
     * @brief Comprime dados de textura para o formato WYT
     * @param uncompressedData Dados descomprimidos
     * @param uncompressedSize Tamanho descomprimido
     * @param compressedData Dados comprimidos (saída)
     * @param compressedSize Tamanho comprimido (saída)
     * @return true se comprimido com sucesso
     */
    bool CompressWYTData(const uint8_t* uncompressedData, uint32_t uncompressedSize, uint8_t*& compressedData, uint32_t& compressedSize);
    
    /**
     * @brief Comprime dados de malha para o formato WYP
     * @param uncompressedData Dados descomprimidos
     * @param uncompressedSize Tamanho descomprimido
     * @param compressedData Dados comprimidos (saída)
     * @param compressedSize Tamanho comprimido (saída)
     * @return true se comprimido com sucesso
     */
    bool CompressWYPData(const uint8_t* uncompressedData, uint32_t uncompressedSize, uint8_t*& compressedData, uint32_t& compressedSize);
    
    /**
     * @brief Converte uma textura para o formato interno
     * @param data Dados
     * @param width Largura
     * @param height Altura
     * @param srcFormat Formato fonte
     * @param dstFormat Formato destino
     * @param dstData Dados destino (saída)
     * @param dstSize Tamanho destino (saída)
     * @return true se convertido com sucesso
     */
    bool ConvertTexture(const uint8_t* data, uint32_t width, uint32_t height, TextureFormat srcFormat, TextureFormat dstFormat, uint8_t*& dstData, uint32_t& dstSize);
    
    /**
     * @brief Gera mipmaps para uma textura
     * @param data Dados
     * @param width Largura
     * @param height Altura
     * @param format Formato
     * @param mipData Dados de mipmap (saída)
     * @param mipSize Tamanho de mipmap (saída)
     * @param mipCount Contagem de mipmaps (saída)
     * @return true se gerado com sucesso
     */
    bool GenerateMipmaps(const uint8_t* data, uint32_t width, uint32_t height, TextureFormat format, uint8_t**& mipData, uint32_t*& mipSize, uint32_t& mipCount);
    
    /**
     * @brief Calcula a iluminação para uma malha
     * @param mesh Malha
     * @param lights Luzes
     * @param lightCount Contagem de luzes
     * @param material Material
     */
    void CalculateLighting(Mesh& mesh, const Light* lights, uint32_t lightCount, const Material& material);
    
    /**
     * @brief Calcula as normais de uma malha
     * @param mesh Malha
     */
    void CalculateNormals(Mesh& mesh);
    
    /**
     * @brief Calcula as tangentes de uma malha
     * @param mesh Malha
     */
    void CalculateTangents(Mesh& mesh);
    
    /**
     * @brief Otimiza uma malha
     * @param mesh Malha
     */
    void OptimizeMesh(Mesh& mesh);
    
    /**
     * @brief Calcula a caixa delimitadora de uma malha
     * @param mesh Malha
     */
    void CalculateBoundingBox(Mesh& mesh);
    
    /**
     * @brief Calcula a esfera delimitadora de uma malha
     * @param mesh Malha
     */
    void CalculateBoundingSphere(Mesh& mesh);

    // Dispositivo de renderização
    std::unique_ptr<IRenderDevice> device_;
    
    // Quadro atual
    RenderFrame currentFrame_;
    
    // Estatísticas
    RenderStats stats_;
    
    // Cache de texturas
    std::unordered_map<std::string, WYDTextureID> textureCache_;
    
    // Cache de malhas
    std::unordered_map<std::string, WYDMeshID> meshCache_;
    
    // Estado
    bool initialized_;
    uint32_t width_;
    uint32_t height_;
    bool fullscreen_;
    uint32_t frameCount_;
    
    // Mutex para sincronização
    std::mutex mutex_;
};

// Acesso global
#define g_RenderingSystem RenderingSystem::GetInstance()

} // namespace renderer
} // namespace reverse
} // namespace wyd

#endif // RENDERINGSYSTEM_H

} // namespace wydbr
