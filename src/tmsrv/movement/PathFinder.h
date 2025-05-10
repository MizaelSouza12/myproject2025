/**
 * @file PathFinder.h
 * @brief Implementação corrigida do algoritmo A* para WYDBR 2.0
 */

#ifndef WYDBR_PATH_FINDER_H
#define WYDBR_PATH_FINDER_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <utility>

namespace WYDBR {

// Forward declarations
class Position;
class Grid;
class Entity;
enum class TerrainType;

namespace Movement {

/**
 * @struct Node
 * @brief Nó para o algoritmo A*
 */
struct Node {
    Position pos;
    float g;         // Custo do caminho até aqui
    float h;         // Heurística (estimativa até o destino)
    float f;         // f = g + h
    Node* parent;    // Nó pai no caminho
};

/**
 * @struct PathfindingOptions
 * @brief Opções para controlar o algoritmo de pathfinding
 */
struct PathfindingOptions {
    int maxNodes = 0;                    // Máximo de nós a explorar (0 = usar valor padrão)
    int maxPathLength = 0;               // Tamanho máximo do caminho retornado (0 = sem limite)
    float maxDistance = 0.0f;            // Distância máxima do caminho (0 = sem limite)
    bool returnPartialPath = false;      // Retornar caminho parcial se completo não for encontrado
    bool smoothPath = true;              // Suavizar caminho retornado
    bool checkDiagonalBlockage = true;   // Verificar se movimentos diagonais estão bloqueados
};

/**
 * @class PathFinder
 * @brief Implementa algoritmo A* corrigido para pathfinding
 */
class PathFinder {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância única
     */
    static PathFinder& getInstance();
    
    /**
     * @brief Encontra caminho entre dois pontos
     * @param start Posição inicial
     * @param target Posição alvo
     * @param entity Entidade que se move (para verificar capacidades)
     * @param options Opções de pathfinding
     * @return Caminho encontrado (vazio se não for possível)
     */
    std::vector<Position> findPath(const Position& start, const Position& target,
                                 const Entity* entity = nullptr,
                                 PathfindingOptions options = PathfindingOptions());
    
    /**
     * @brief Estima distância entre duas posições
     * @param start Posição inicial
     * @param end Posição final
     * @return Distância estimada
     */
    float calculateHeuristic(const Position& start, const Position& end) const;
    
    /**
     * @brief Define o número máximo de nós a explorar
     * @param maxNodes Número máximo de nós
     */
    void setMaxNodesExplored(int maxNodes);
    
    /**
     * @brief Habilita ou desabilita movimento diagonal
     * @param enable true para habilitar, false para desabilitar
     */
    void setDiagonalMovement(bool enable);
    
    /**
     * @brief Define custos de movimento
     * @param cardinal Custo para movimento cardinal (ortogonal)
     * @param diagonal Custo para movimento diagonal
     */
    void setMovementCosts(float cardinal, float diagonal);

private:
    // Singleton
    static std::unique_ptr<PathFinder> s_instance;
    static std::once_flag s_onceFlag;
    
    PathFinder();
    ~PathFinder();
    
    // Métodos privados
    bool isWalkable(const Position& pos, const Entity* entity) const;
    std::vector<Position> reconstructPath(const Node* targetNode) const;
    
    // Referência ao grid do mundo
    std::shared_ptr<Grid> m_grid;
    
    // Configurações
    int m_maxNodesExplored;
    bool m_diagonalMovement;
    float m_diagonalCost;
    float m_cardinalCost;
    
    // Direções possíveis de movimento e custos
    std::vector<std::pair<int, int>> m_directions;
    std::vector<float> m_directionCosts;
};

} // namespace Movement
} // namespace WYDBR

#endif // WYDBR_PATH_FINDER_H