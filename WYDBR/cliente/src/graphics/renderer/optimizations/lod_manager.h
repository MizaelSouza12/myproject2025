#ifndef WYDBR_LOD_MANAGER_H
#define WYDBR_LOD_MANAGER_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <string>

namespace wydbr {
namespace graphics {

// Definição de níveis de LOD (Level of Detail)
enum class LODLevel {
    ULTRA,     ///< Máximo detalhe
    HIGH,      ///< Alto detalhe
    MEDIUM,    ///< Detalhe médio
    LOW,       ///< Baixo detalhe
    VERY_LOW   ///< Detalhe mínimo
};

// Estrutura que define um conjunto de LOD para um objeto
struct LODSet {
    std::string objectName;
    std::unordered_map<LODLevel, uint32_t> meshIds;  // Mapeamento de nível de LOD para ID de mesh
    std::unordered_map<LODLevel, uint32_t> textureIds; // Mapeamento de nível de LOD para ID de textura
    float switchDistances[5]; // Distâncias para troca entre os 5 níveis (ULTRA, HIGH, MEDIUM, LOW, VERY_LOW)
    bool useTextureVariants; // Se deve usar variantes de textura para cada nível de LOD
    bool useMeshVariants;   // Se deve usar variantes de mesh para cada nível de LOD
};

// Callback para notificação de troca de LOD
using LODSwitchCallback = std::function<void(uint32_t objectId, LODLevel newLevel, 
                                           uint32_t newMeshId, uint32_t newTextureId)>;

/**
 * @class LODManager
 * @brief Gerenciador dinâmico de níveis de detalhe (LOD)
 * 
 * Este sistema gerencia a troca dinâmica entre diferentes níveis de detalhe
 * para objetos no mundo, com base na distância do observador e prioridade visual.
 */
class LODManager {
public:
    LODManager();
    ~LODManager();
    
    /**
     * @brief Inicializa o gerenciador de LOD
     * @param globalLODBias Bias global para ajustar distâncias de LOD (-1.0 a 1.0)
     * @param useAdaptiveLOD Se deve ajustar LOD baseado em performance
     * @return true se inicializado com sucesso
     */
    bool initialize(float globalLODBias = 0.0f, bool useAdaptiveLOD = true);
    
    /**
     * @brief Libera recursos do sistema
     */
    void shutdown();
    
    /**
     * @brief Adiciona um conjunto de LOD para um objeto
     * @param objectId ID do objeto
     * @param lodSet Conjunto de LOD
     * @return true se adicionado com sucesso
     */
    bool addLODSet(uint32_t objectId, const LODSet& lodSet);
    
    /**
     * @brief Remove um objeto do gerenciador de LOD
     * @param objectId ID do objeto
     * @return true se removido com sucesso
     */
    bool removeObject(uint32_t objectId);
    
    /**
     * @brief Atualiza o nível de LOD baseado na distância
     * @param objectId ID do objeto
     * @param distance Distância do observador
     * @return true se o nível de LOD foi atualizado
     */
    bool updateLOD(uint32_t objectId, float distance);
    
    /**
     * @brief Atualiza todos os objetos gerenciados
     * @param viewPosX Posição X do observador
     * @param viewPosY Posição Y do observador
     * @param viewPosZ Posição Z do observador
     * @param currentFPS FPS atual para ajuste adaptativo
     */
    void updateAll(float viewPosX, float viewPosY, float viewPosZ, float currentFPS);
    
    /**
     * @brief Obtém o nível de LOD atual de um objeto
     * @param objectId ID do objeto
     * @return Nível de LOD atual
     */
    LODLevel getCurrentLOD(uint32_t objectId) const;
    
    /**
     * @brief Obtém os IDs de recursos atuais de um objeto
     * @param objectId ID do objeto
     * @param outMeshId ID do mesh atual (saída)
     * @param outTextureId ID da textura atual (saída)
     * @return true se os IDs foram obtidos com sucesso
     */
    bool getCurrentResources(uint32_t objectId, uint32_t& outMeshId, uint32_t& outTextureId) const;
    
    /**
     * @brief Força um nível específico de LOD para um objeto
     * @param objectId ID do objeto
     * @param level Nível de LOD forçado
     * @return true se o nível foi forçado com sucesso
     */
    bool forceLODLevel(uint32_t objectId, LODLevel level);
    
    /**
     * @brief Define o bias global de LOD
     * @param bias Valor do bias (-1.0 a 1.0)
     */
    void setGlobalLODBias(float bias);
    
    /**
     * @brief Define o modo adaptativo
     * @param enabled Se o modo adaptativo está ativado
     * @param targetFPS FPS alvo para ajuste adaptativo
     */
    void setAdaptiveMode(bool enabled, float targetFPS = 60.0f);
    
    /**
     * @brief Registra um callback para notificação de troca de LOD
     * @param callback Função a ser chamada quando ocorrer uma troca
     * @return ID do callback para remoção
     */
    int registerLODSwitchCallback(LODSwitchCallback callback);
    
    /**
     * @brief Remove um callback registrado
     * @param callbackId ID do callback a ser removido
     * @return true se removido com sucesso
     */
    bool unregisterLODSwitchCallback(int callbackId);
    
    /**
     * @brief Obtém estatísticas do gerenciador de LOD
     * @param objectCount Total de objetos gerenciados
     * @param objectsAtHighestLOD Objetos no nível mais alto de detalhe
     * @param objectsAtLowestLOD Objetos no nível mais baixo de detalhe
     * @param averageLODLevel Nível médio de LOD (0=ULTRA, 4=VERY_LOW)
     */
    void getStatistics(int& objectCount, int& objectsAtHighestLOD, 
                       int& objectsAtLowestLOD, float& averageLODLevel) const;

private:
    // Estrutura para rastreamento interno de objetos
    struct ObjectState {
        LODSet lodSet;
        LODLevel currentLevel;
        uint32_t currentMeshId;
        uint32_t currentTextureId;
        float lastDistance;
        float position[3]; // x, y, z
        float importance;  // Fator de importância visual (0-1)
    };
    
    // Notifica callbacks sobre mudança de LOD
    void notifyLODChange(uint32_t objectId, LODLevel newLevel, 
                         uint32_t newMeshId, uint32_t newTextureId);
    
    // Calcula distâncias efetivas de LOD baseadas no bias global
    void calculateEffectiveDistances(float* distances, float bias);
    
    // Ajusta o bias de LOD baseado na performance atual
    void adjustLODBasedOnPerformance(float currentFPS);
    
    // Dados e estado
    std::unordered_map<uint32_t, ObjectState> m_objects;
    std::vector<std::pair<int, LODSwitchCallback>> m_callbacks;
    int m_nextCallbackId;
    
    // Configuração e estado
    bool m_initialized;
    float m_globalLODBias;
    bool m_useAdaptiveLOD;
    float m_targetFPS;
    float m_dynamicLODScale;
    
    // Estatísticas
    int m_objectCount;
    int m_objectsAtHighestLOD;
    int m_objectsAtLowestLOD;
    float m_averageLODLevel;
    
    // Thread safety
    mutable std::mutex m_objectsMutex;
    mutable std::mutex m_callbackMutex;
};

} // namespace graphics
} // namespace wydbr

#endif // WYDBR_LOD_MANAGER_H
