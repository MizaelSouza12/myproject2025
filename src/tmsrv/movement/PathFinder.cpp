/**
 * @file PathFinder.cpp
 * @brief Implementação corrigida do algoritmo A* para WYDBR 2.0
 */

#include "movement/PathFinder.h"
#include "../world/Position.h"
#include "../world/Grid.h"
#include "../entity/Entity.h"
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <queue>
#include <memory>

namespace WYDBR {
namespace Movement {

// Singleton instance
std::unique_ptr<PathFinder> PathFinder::s_instance;
std::once_flag PathFinder::s_onceFlag;

PathFinder& PathFinder::getInstance() {
    std::call_once(s_onceFlag, []() {
        s_instance.reset(new PathFinder());
    });
    return *s_instance;
}

PathFinder::PathFinder() 
    : m_maxNodesExplored(10000)
    , m_diagonalMovement(true)
    , m_diagonalCost(1.414f)  // Aproximação de sqrt(2)
    , m_cardinalCost(1.0f) {
    
    // Inicializações
    m_grid = Grid::getInstance();
    
    // Configurar movimentos possíveis
    if (m_diagonalMovement) {
        // 8 direções (incluindo diagonais)
        m_directions = {
            {0, 1},    // Norte
            {1, 1},    // Nordeste
            {1, 0},    // Leste
            {1, -1},   // Sudeste
            {0, -1},   // Sul
            {-1, -1},  // Sudoeste
            {-1, 0},   // Oeste
            {-1, 1}    // Noroeste
        };
        
        // Custos correspondentes
        m_directionCosts = {
            m_cardinalCost,   // Norte
            m_diagonalCost,   // Nordeste
            m_cardinalCost,   // Leste
            m_diagonalCost,   // Sudeste
            m_cardinalCost,   // Sul
            m_diagonalCost,   // Sudoeste
            m_cardinalCost,   // Oeste
            m_diagonalCost    // Noroeste
        };
    } else {
        // 4 direções (apenas cardeais)
        m_directions = {
            {0, 1},    // Norte
            {1, 0},    // Leste
            {0, -1},   // Sul
            {-1, 0}    // Oeste
        };
        
        // Custos correspondentes
        m_directionCosts = {
            m_cardinalCost,   // Norte
            m_cardinalCost,   // Leste
            m_cardinalCost,   // Sul
            m_cardinalCost    // Oeste
        };
    }
}

PathFinder::~PathFinder() {
    // Limpeza
}

std::vector<Position> PathFinder::findPath(const Position& start, const Position& target,
                                        const Entity* entity, PathfindingOptions options) {
    // Verificação rápida: destino igual à origem
    if (start == target) {
        return {start};
    }
    
    // Verificação rápida: destino não caminhável
    if (!isWalkable(target, entity)) {
        return {};
    }
    
    // Verificação rápida: distância máxima
    if (options.maxDistance > 0) {
        float distance = calculateHeuristic(start, target);
        if (distance > options.maxDistance) {
            return {};
        }
    }
    
    // Estruturas para o algoritmo A*
    auto compareNodes = [](const Node* a, const Node* b) {
        return a->f > b->f;  // Menor f primeiro
    };
    
    std::priority_queue<Node*, std::vector<Node*>, decltype(compareNodes)> 
        openSet(compareNodes);
    
    std::unordered_map<std::string, std::unique_ptr<Node>> nodes;
    std::unordered_set<std::string> closedSet;
    
    // Função para gerar chave única para uma posição
    auto getPositionKey = [](const Position& pos) {
        return std::to_string(pos.x) + "_" + std::to_string(pos.y) + "_" + std::to_string(pos.z);
    };
    
    // Cria nó inicial
    std::string startKey = getPositionKey(start);
    auto startNode = new Node{start, 0.0f, calculateHeuristic(start, target), nullptr};
    startNode->f = startNode->g + startNode->h;  // f = g + h
    
    nodes[startKey] = std::unique_ptr<Node>(startNode);
    openSet.push(startNode);
    
    // Contador de nós explorados para limitar processamento
    int nodesExplored = 0;
    int maxNodes = options.maxNodes > 0 ? options.maxNodes : m_maxNodesExplored;
    
    // Loop principal do A*
    while (!openSet.empty() && nodesExplored < maxNodes) {
        // Obtém nó com menor f
        Node* current = openSet.top();
        openSet.pop();
        nodesExplored++;
        
        // Chave do nó atual
        std::string currentKey = getPositionKey(current->pos);
        
        // Verifica se chegamos ao destino
        if (current->pos == target) {
            // Reconstrói e retorna o caminho
            std::vector<Position> path = reconstructPath(current);
            
            // Se houver limite de tamanho do caminho, aplicar
            if (options.maxPathLength > 0 && path.size() > options.maxPathLength) {
                path.resize(options.maxPathLength);
            }
            
            return path;
        }
        
        // Adiciona ao conjunto fechado
        closedSet.insert(currentKey);
        
        // Explora vizinhos
        for (size_t i = 0; i < m_directions.size(); ++i) {
            // Calcula nova posição
            Position neighborPos(
                current->pos.x + m_directions[i].first,
                current->pos.y + m_directions[i].second,
                current->pos.z
            );
            
            // Chave do vizinho
            std::string neighborKey = getPositionKey(neighborPos);
            
            // Pula se já estiver no conjunto fechado
            if (closedSet.find(neighborKey) != closedSet.end()) {
                continue;
            }
            
            // Verifica se é caminhável
            if (!isWalkable(neighborPos, entity)) {
                closedSet.insert(neighborKey);  // Marca como não caminhável
                continue;
            }
            
            // Custo do movimento para este vizinho
            float moveCost = m_directionCosts[i];
            
            // Se for movimento diagonal, verificar se os cardeais adjacentes são caminháveis
            if (moveCost > m_cardinalCost && options.checkDiagonalBlockage) {
                Position cardinalPos1(current->pos.x + m_directions[i].first, current->pos.y, current->pos.z);
                Position cardinalPos2(current->pos.x, current->pos.y + m_directions[i].second, current->pos.z);
                
                if (!isWalkable(cardinalPos1, entity) || !isWalkable(cardinalPos2, entity)) {
                    continue;  // Movimento diagonal bloqueado
                }
            }
            
            // Custo acumulado até este vizinho
            float gScore = current->g + moveCost;
            
            // Verifica se já existe um nó para esta posição
            auto nodeIt = nodes.find(neighborKey);
            bool isNewNode = nodeIt == nodes.end();
            
            // Cria novo nó ou atualiza existente se caminho for melhor
            if (isNewNode || gScore < nodeIt->second->g) {
                Node* neighborNode;
                
                if (isNewNode) {
                    // Cria novo nó
                    neighborNode = new Node{
                        neighborPos, 
                        gScore, 
                        calculateHeuristic(neighborPos, target),
                        current
                    };
                    neighborNode->f = gScore + neighborNode->h;
                    nodes[neighborKey] = std::unique_ptr<Node>(neighborNode);
                } else {
                    // Atualiza nó existente
                    neighborNode = nodeIt->second.get();
                    neighborNode->g = gScore;
                    neighborNode->f = gScore + neighborNode->h;
                    neighborNode->parent = current;
                }
                
                // Adiciona ao conjunto aberto
                openSet.push(neighborNode);
            }
        }
    }
    
    // Não encontrou caminho, verifica se deve retornar caminho parcial
    if (options.returnPartialPath && !nodes.empty()) {
        // Encontra o nó mais próximo do destino
        Node* bestNode = nullptr;
        float bestDistance = std::numeric_limits<float>::max();
        
        for (const auto& pair : nodes) {
            float distance = calculateHeuristic(pair.second->pos, target);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestNode = pair.second.get();
            }
        }
        
        // Reconstrói o caminho parcial
        if (bestNode) {
            std::vector<Position> partialPath = reconstructPath(bestNode);
            
            // Se houver limite de tamanho do caminho, aplicar
            if (options.maxPathLength > 0 && partialPath.size() > options.maxPathLength) {
                partialPath.resize(options.maxPathLength);
            }
            
            return partialPath;
        }
    }
    
    // Não encontrou nenhum caminho
    return {};
}

float PathFinder::calculateHeuristic(const Position& start, const Position& end) const {
    // Calcular distância
    float dx = std::abs(static_cast<float>(end.x - start.x));
    float dy = std::abs(static_cast<float>(end.y - start.y));
    float dz = std::abs(static_cast<float>(end.z - start.z)) * 10.0f;  // Movimento vertical é mais custoso
    
    // Se o movimento diagonal é permitido, usar distância octil (variação da chebyshev)
    if (m_diagonalMovement) {
        // D2 * (dx + dy) + (D - 2*D2) * std::min(dx, dy)
        // onde D é o custo de movimento cardinal e D2 é a diferença entre o custo diagonal e cardinal
        float D = m_cardinalCost;
        float D2 = m_diagonalCost - m_cardinalCost;
        
        float hXY = D * (dx + dy) + (D2) * std::min(dx, dy);
        return hXY + dz;  // Adicionar custo vertical
    } else {
        // Distância de Manhattan
        return m_cardinalCost * (dx + dy + dz);
    }
}

bool PathFinder::isWalkable(const Position& pos, const Entity* entity) const {
    // Verifica limites do mundo
    if (!m_grid->isWithinBounds(pos)) {
        return false;
    }
    
    // Verifica se terreno é caminhável para todos
    if (!m_grid->isWalkable(pos)) {
        return false;
    }
    
    // Verificações específicas para entidade (se fornecida)
    if (entity) {
        // Verifica habilidades específicas da entidade
        TerrainType terrain = m_grid->getTerrainType(pos);
        if (!entity->canTraverse(terrain)) {
            return false;
        }
        
        // Verifica colisão com outras entidades
        if (m_grid->hasEntityAt(pos) && !entity->canPassThrough()) {
            return false;
        }
    }
    
    return true;
}

std::vector<Position> PathFinder::reconstructPath(const Node* targetNode) const {
    std::vector<Position> path;
    
    // Reconstruir caminho do destino até a origem
    for (const Node* current = targetNode; current != nullptr; current = current->parent) {
        path.push_back(current->pos);
    }
    
    // Inverter para ter caminho da origem ao destino
    std::reverse(path.begin(), path.end());
    
    return path;
}

void PathFinder::setMaxNodesExplored(int maxNodes) {
    m_maxNodesExplored = maxNodes > 0 ? maxNodes : 1000;
}

void PathFinder::setDiagonalMovement(bool enable) {
    if (m_diagonalMovement != enable) {
        m_diagonalMovement = enable;
        
        // Atualizar direções disponíveis
        if (m_diagonalMovement) {
            // 8 direções (incluindo diagonais)
            m_directions = {
                {0, 1},    // Norte
                {1, 1},    // Nordeste
                {1, 0},    // Leste
                {1, -1},   // Sudeste
                {0, -1},   // Sul
                {-1, -1},  // Sudoeste
                {-1, 0},   // Oeste
                {-1, 1}    // Noroeste
            };
            
            // Custos correspondentes
            m_directionCosts = {
                m_cardinalCost,   // Norte
                m_diagonalCost,   // Nordeste
                m_cardinalCost,   // Leste
                m_diagonalCost,   // Sudeste
                m_cardinalCost,   // Sul
                m_diagonalCost,   // Sudoeste
                m_cardinalCost,   // Oeste
                m_diagonalCost    // Noroeste
            };
        } else {
            // 4 direções (apenas cardeais)
            m_directions = {
                {0, 1},    // Norte
                {1, 0},    // Leste
                {0, -1},   // Sul
                {-1, 0}    // Oeste
            };
            
            // Custos correspondentes
            m_directionCosts = {
                m_cardinalCost,   // Norte
                m_cardinalCost,   // Leste
                m_cardinalCost,   // Sul
                m_cardinalCost    // Oeste
            };
        }
    }
}

void PathFinder::setMovementCosts(float cardinal, float diagonal) {
    m_cardinalCost = cardinal > 0.0f ? cardinal : 1.0f;
    m_diagonalCost = diagonal > 0.0f ? diagonal : 1.414f;
    
    // Atualizar custos de direção
    if (m_diagonalMovement) {
        m_directionCosts = {
            m_cardinalCost,   // Norte
            m_diagonalCost,   // Nordeste
            m_cardinalCost,   // Leste
            m_diagonalCost,   // Sudeste
            m_cardinalCost,   // Sul
            m_diagonalCost,   // Sudoeste
            m_cardinalCost,   // Oeste
            m_diagonalCost    // Noroeste
        };
    } else {
        m_directionCosts = {
            m_cardinalCost,   // Norte
            m_cardinalCost,   // Leste
            m_cardinalCost,   // Sul
            m_cardinalCost    // Oeste
        };
    }
}

} // namespace Movement
} // namespace WYDBR