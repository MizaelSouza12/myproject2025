#ifndef WYDBR_OCCLUSION_SYSTEM_H
#define WYDBR_OCCLUSION_SYSTEM_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <bitset>
#include <mutex>
#include <atomic>

namespace wydbr {
namespace graphics {

// Estrutura que define um volume de visão (frustum)
struct ViewFrustum {
    // 6 planos: Near, Far, Left, Right, Top, Bottom
    struct Plane {
        float a, b, c, d; // Coeficientes do plano (ax + by + cz + d = 0)
    };
    
    Plane planes[6];
};

// Estrutura que define uma caixa delimitadora (bounding box)
struct BoundingBox {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
};

// Estrutura que define uma esfera delimitadora (bounding sphere)
struct BoundingSphere {
    float x, y, z; // Centro
    float radius;  // Raio
};

/**
 * @class OcclusionSystem
 * @brief Sistema avançado de occlusion culling para otimização de renderização
 * 
 * Este sistema implementa várias técnicas de occlusion culling para evitar
 * o processamento e renderização de objetos que não são visíveis na cena.
 */
class OcclusionSystem {
public:
    OcclusionSystem();
    ~OcclusionSystem();
    
    /**
     * @brief Inicializa o sistema de occlusion culling
     * @param useHardwareOcclusion Se deve usar occlusion queries de hardware quando disponível
     * @param usePortals Se deve usar sistema de portais para oclusão
     * @param useHierarchical Se deve usar occlusion culling hierárquico
     * @return true se inicializado com sucesso
     */
    bool initialize(bool useHardwareOcclusion = true, bool usePortals = true, bool useHierarchical = true);
    
    /**
     * @brief Libera recursos do sistema
     */
    void shutdown();
    
    /**
     * @brief Atualiza o frustum de visualização
     * @param frustum Novo frustum de visualização
     */
    void updateViewFrustum(const ViewFrustum& frustum);
    
    /**
     * @brief Adiciona um objeto ao sistema de oclusão
     * @param objectId ID único do objeto
     * @param boundingBox Caixa delimitadora do objeto
     * @param boundingSphere Esfera delimitadora do objeto (opcional)
     * @param isOccluder Se o objeto também é um occluder
     * @param priority Prioridade do objeto (objetos com maior prioridade são testados primeiro)
     * @return true se adicionado com sucesso
     */
    bool addObject(uint32_t objectId, const BoundingBox& boundingBox, 
                   const BoundingSphere* boundingSphere = nullptr, 
                   bool isOccluder = false, int priority = 0);
    
    /**
     * @brief Remove um objeto do sistema de oclusão
     * @param objectId ID do objeto a remover
     * @return true se removido com sucesso
     */
    bool removeObject(uint32_t objectId);
    
    /**
     * @brief Atualiza a posição/dimensões de um objeto
     * @param objectId ID do objeto
     * @param boundingBox Nova caixa delimitadora
     * @param boundingSphere Nova esfera delimitadora (opcional)
     * @return true se atualizado com sucesso
     */
    bool updateObject(uint32_t objectId, const BoundingBox& boundingBox, 
                      const BoundingSphere* boundingSphere = nullptr);
    
    /**
     * @brief Executa o culling e determina quais objetos são visíveis
     * @param viewPosX Posição X da câmera
     * @param viewPosY Posição Y da câmera
     * @param viewPosZ Posição Z da câmera
     * @param dirX Direção X da visualização
     * @param dirY Direção Y da visualização
     * @param dirZ Direção Z da visualização
     */
    void performCulling(float viewPosX, float viewPosY, float viewPosZ,
                        float dirX, float dirY, float dirZ);
    
    /**
     * @brief Verifica se um objeto específico é visível
     * @param objectId ID do objeto
     * @return true se o objeto é visível
     */
    bool isObjectVisible(uint32_t objectId) const;
    
    /**
     * @brief Obtém a lista de objetos visíveis
     * @return Lista de IDs de objetos visíveis
     */
    std::vector<uint32_t> getVisibleObjects() const;
    
    /**
     * @brief Obtém estatísticas do sistema de oclusão
     * @param totalObjects Total de objetos no sistema
     * @param visibleObjects Total de objetos considerados visíveis
     * @param culledByFrustum Total de objetos removidos pelo frustum culling
     * @param culledByOcclusion Total de objetos removidos pelo occlusion culling
     */
    void getStatistics(int& totalObjects, int& visibleObjects, 
                       int& culledByFrustum, int& culledByOcclusion) const;
    
    /**
     * @brief Define o modo de otimização
     * @param aggressiveCulling Se deve usar culling mais agressivo para melhor performance
     * @param conservativeRasterization Se deve usar rasterização conservativa
     */
    void setOptimizationMode(bool aggressiveCulling, bool conservativeRasterization);
    
    /**
     * @brief Verifica se um objeto está dentro do frustum
     * @param boundingBox Caixa delimitadora do objeto
     * @return true se o objeto está dentro do frustum
     */
    bool isInFrustum(const BoundingBox& boundingBox) const;
    
    /**
     * @brief Verifica se um objeto está dentro do frustum
     * @param boundingSphere Esfera delimitadora do objeto
     * @return true se o objeto está dentro do frustum
     */
    bool isInFrustum(const BoundingSphere& boundingSphere) const;

private:
    // Estrutura interna para rastreamento de objetos
    struct ObjectData {
        BoundingBox boundingBox;
        BoundingSphere boundingSphere;
        bool hasBoundingSphere;
        bool isOccluder;
        int priority;
        bool wasVisible;
        bool isVisible;
    };
    
    // Estrutura para agrupamento hierárquico
    struct HierarchicalGroup {
        BoundingBox boundingBox;
        std::vector<uint32_t> objectIds;
        bool isVisible;
    };
    
    // Métodos internos
    void buildHierarchy();
    void performFrustumCulling();
    void performOcclusionCulling();
    void updateOccluders();
    bool testOcclusion(uint32_t objectId);
    
    // Dados e estado
    ViewFrustum m_viewFrustum;
    std::unordered_map<uint32_t, ObjectData> m_objects;
    std::vector<uint32_t> m_occluders;
    std::vector<uint32_t> m_visibleObjects;
    std::vector<HierarchicalGroup> m_hierarchicalGroups;
    
    bool m_initialized;
    bool m_useHardwareOcclusion;
    bool m_usePortals;
    bool m_useHierarchical;
    bool m_aggressiveCulling;
    bool m_conservativeRasterization;
    
    mutable std::mutex m_objectsMutex;
    
    // Estatísticas
    int m_totalObjects;
    int m_visibleObjects;
    int m_culledByFrustum;
    int m_culledByOcclusion;
    
    // Implementação específica de hardware occlusion
    std::unique_ptr<class HardwareOcclusionQuery> m_hardwareOcclusion;
};

} // namespace graphics
} // namespace wydbr

#endif // WYDBR_OCCLUSION_SYSTEM_H
