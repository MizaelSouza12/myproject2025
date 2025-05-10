/**
 * @file Pathfinder.h
 * @brief Implementação correta do algoritmo A* para pathfinding
 */

#ifndef WYDBR_PATHFINDER_H
#define WYDBR_PATHFINDER_H

#include <cstdint>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <limits>
#include <memory>
#include <cmath>

namespace WYDBR {
namespace GameLogic {

/**
 * Posição no mundo
 */
struct Position {
    int32_t x;
    int32_t y;
    int32_t z;
    
    // Construtor
    Position(int32_t _x = 0, int32_t _y = 0, int32_t _z = 0)
        : x(_x), y(_y), z(_z) {}
    
    // Operador de igualdade
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    // Operador de desigualdade
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

// Hash para Position
struct PositionHash {
    std::size_t operator()(const Position& pos) const {
        return static_cast<size_t>(pos.x) ^ 
               (static_cast<size_t>(pos.y) << 1) ^ 
               (static_cast<size_t>(pos.z) << 2);
    }
};

/**
 * Tipo de terreno
 */
enum class TerrainType {
    Ground,             // Terreno normal
    Water,              // Água
    Wall,               // Parede
    Obstacle,           // Obstáculo
    Elevation,          // Elevação
    Hole,               // Buraco
    Ice,                // Gelo
    Lava,               // Lava
    Custom              // Personalizado
};

/**
 * Permissões de movimento
 */
enum class MovementPermission {
    All,                // Todos
    None,               // Nenhum
    Fly,                // Voar
    Swim,               // Nadar
    Walk,               // Andar
    Climb,              // Escalar
    Teleport            // Teleportar
};

/**
 * Opções de pathfinding
 */
struct PathfindingOptions {
    bool allowDiagonal;           // Permite movimento diagonal
    bool cutCorners;              // Permite cortar cantos
    bool smoothPath;              // Suaviza o caminho
    int32_t maxIterations;        // Máximo de iterações
    int32_t maxPathLength;        // Tamanho máximo do caminho
    float heuristicWeight;        // Peso da heurística (1.0 = A*)
    float movementCost;           // Custo de movimento
    float diagonalCost;           // Custo diagonal
    MovementPermission permission; // Permissão de movimento
};

/**
 * Tipo de heurística
 */
enum class HeuristicType {
    Manhattan,          // Distância Manhattan (admissível)
    Euclidean,          // Distância Euclidiana (admissível)
    Chebyshev,          // Distância Chebyshev (admissível)
    Octile,             // Distância Octile (admissível)
    Custom              // Personalizado
};

/**
 * Resultado de pathfinding
 */
struct PathfindingResult {
    std::vector<Position> path;  // Caminho encontrado
    bool found;                  // Caminho encontrado
    int32_t iterations;          // Número de iterações
    float pathCost;              // Custo do caminho
    int64_t timeMs;              // Tempo de execução em ms
};

/**
 * Interface para mapas de navegação
 */
class INavigationMap {
public:
    virtual ~INavigationMap() = default;
    
    /**
     * Verifica se uma posição é transitável
     * @param pos Posição
     * @param permission Permissão de movimento
     * @return true se for transitável
     */
    virtual bool IsWalkable(const Position& pos, MovementPermission permission) const = 0;
    
    /**
     * Obtém os vizinhos de uma posição
     * @param pos Posição
     * @param options Opções de pathfinding
     * @return Lista de posições vizinhas
     */
    virtual std::vector<Position> GetNeighbors(const Position& pos, const PathfindingOptions& options) const = 0;
    
    /**
     * Obtém o custo de movimento entre duas posições
     * @param from Posição de origem
     * @param to Posição de destino
     * @param options Opções de pathfinding
     * @return Custo de movimento
     */
    virtual float GetMovementCost(const Position& from, const Position& to, const PathfindingOptions& options) const = 0;
    
    /**
     * Obtém o tipo de terreno em uma posição
     * @param pos Posição
     * @return Tipo de terreno
     */
    virtual TerrainType GetTerrainType(const Position& pos) const = 0;
    
    /**
     * Obtém a elevação em uma posição
     * @param pos Posição
     * @return Elevação
     */
    virtual float GetElevation(const Position& pos) const = 0;
    
    /**
     * Verifica se o movimento é diagonal
     * @param from Posição de origem
     * @param to Posição de destino
     * @return true se for diagonal
     */
    virtual bool IsDiagonalMovement(const Position& from, const Position& to) const = 0;
    
    /**
     * Verifica se é possível cortar um canto
     * @param from Posição de origem
     * @param to Posição de destino
     * @param options Opções de pathfinding
     * @return true se for possível
     */
    virtual bool CanCutCorner(const Position& from, const Position& to, const PathfindingOptions& options) const = 0;
};

/**
 * Implementação de A*
 */
class AStar {
public:
    /**
     * Construtor
     * @param navigationMap Mapa de navegação
     */
    explicit AStar(std::shared_ptr<INavigationMap> navigationMap);
    
    /**
     * Encontra um caminho entre dois pontos
     * @param start Posição inicial
     * @param goal Posição final
     * @param options Opções de pathfinding
     * @return Resultado do pathfinding
     */
    PathfindingResult FindPath(const Position& start, const Position& goal, 
                             const PathfindingOptions& options = PathfindingOptions());
    
    /**
     * Define o tipo de heurística
     * @param type Tipo de heurística
     */
    void SetHeuristicType(HeuristicType type);
    
    /**
     * Define uma função de heurística personalizada
     * @param heuristic Função de heurística
     */
    void SetCustomHeuristic(std::function<float(const Position&, const Position&)> heuristic);
    
    /**
     * Suaviza um caminho encontrado
     * @param path Caminho a suavizar
     * @param options Opções de pathfinding
     * @return Caminho suavizado
     */
    std::vector<Position> SmoothPath(const std::vector<Position>& path, const PathfindingOptions& options);
    
    /**
     * Simplifica um caminho removendo pontos desnecessários
     * @param path Caminho a simplificar
     * @return Caminho simplificado
     */
    std::vector<Position> SimplifyPath(const std::vector<Position>& path);
    
    /**
     * Verifica se há linha de visão entre dois pontos
     * @param from Posição de origem
     * @param to Posição de destino
     * @param options Opções de pathfinding
     * @return true se houver linha de visão
     */
    bool HasLineOfSight(const Position& from, const Position& to, const PathfindingOptions& options);
    
private:
    // Mapa de navegação
    std::shared_ptr<INavigationMap> m_navigationMap;
    
    // Tipo de heurística
    HeuristicType m_heuristicType;
    
    // Função de heurística personalizada
    std::function<float(const Position&, const Position&)> m_customHeuristic;
    
    // Nó para o algoritmo A*
    struct Node {
        Position position;
        float g;         // Custo do início até este nó
        float h;         // Heurística: estimativa do custo deste nó até o objetivo
        float f;         // f = g + h
        std::shared_ptr<Node> parent;
        
        // Construtor
        Node(const Position& pos, float gCost, float hCost, std::shared_ptr<Node> p = nullptr)
            : position(pos), g(gCost), h(hCost), f(gCost + hCost), parent(p) {}
        
        // Comparador para a fila de prioridade
        bool operator>(const Node& other) const {
            return f > other.f;
        }
    };
    
    // Métodos privados
    float CalculateHeuristic(const Position& from, const Position& to);
    float ManhattanDistance(const Position& from, const Position& to);
    float EuclideanDistance(const Position& from, const Position& to);
    float ChebyshevDistance(const Position& from, const Position& to);
    float OctileDistance(const Position& from, const Position& to);
    std::vector<Position> ReconstructPath(std::shared_ptr<Node> node);
    bool IsPositionInList(const Position& pos, const std::unordered_map<Position, float, PositionHash>& list);
};

/**
 * Gerenciador de pathfinding
 */
class PathfindingManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static PathfindingManager& GetInstance();
    
    /**
     * Registra um mapa de navegação
     * @param name Nome do mapa
     * @param map Mapa de navegação
     */
    void RegisterNavigationMap(const std::string& name, std::shared_ptr<INavigationMap> map);
    
    /**
     * Obtém um mapa de navegação
     * @param name Nome do mapa
     * @return Ponteiro para o mapa ou nullptr se não existir
     */
    std::shared_ptr<INavigationMap> GetNavigationMap(const std::string& name);
    
    /**
     * Encontra um caminho entre dois pontos
     * @param mapName Nome do mapa
     * @param start Posição inicial
     * @param goal Posição final
     * @param options Opções de pathfinding
     * @return Resultado do pathfinding
     */
    PathfindingResult FindPath(const std::string& mapName, const Position& start, 
                             const Position& goal, const PathfindingOptions& options = PathfindingOptions());
    
    /**
     * Define opções de pathfinding padrão
     * @param options Opções
     */
    void SetDefaultOptions(const PathfindingOptions& options);
    
    /**
     * Obtém opções de pathfinding padrão
     * @return Opções padrão
     */
    PathfindingOptions GetDefaultOptions() const;
    
private:
    // Construtor e destrutor privados (singleton)
    PathfindingManager();
    ~PathfindingManager();
    
    // Não permitir cópias
    PathfindingManager(const PathfindingManager&) = delete;
    PathfindingManager& operator=(const PathfindingManager&) = delete;
    
    // Mapas de navegação
    std::unordered_map<std::string, std::shared_ptr<INavigationMap>> m_navigationMaps;
    
    // Opções padrão
    PathfindingOptions m_defaultOptions;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
};

} // namespace GameLogic
} // namespace WYDBR

#endif // WYDBR_PATHFINDER_H
