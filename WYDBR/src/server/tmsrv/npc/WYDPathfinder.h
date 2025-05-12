/**
 * WYDPathfinder.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/WYDPathfinder.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_PATHFINDER_H
#define WYD_PATHFINDER_H

#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <memory>
#include <cstdint>

namespace wydbr {


class WYDMap;

/**
 * Sistema de pathfinding para NPCs e monstros
 * 
 * Implementa o algoritmo A* para encontrar caminhos eficientes no mundo de jogo.
 * Otimizado para ambientes de grande escala com muitas entidades.
 */
/**
 * Classe WYDPathfinder
 * 
 * Esta classe implementa a funcionalidade WYDPathfinder conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDPathfinder {
public:
    // Estrutura para representar uma célula no grid de navegação
    struct NavCell {
        uint16_t x;
        uint16_t y;
        
        bool operator==(const NavCell& other) const {
            return x == other.x && y == other.y;
        }
    };
    
    // Estrutura para armazenar informações de uma célula durante a busca A*
    struct NavNode {
        NavCell cell;
        float g; // custo do início até este nó
        float h; // estimativa heurística até o destino
        float f; // g + h
        NavCell parent;
        
        bool operator>(const NavNode& other) const {
            return f > other.f;
        }
    };
    
    // Hash para NavCell para uso em unordered_map
    struct NavCellHash {
        size_t operator()(const NavCell& cell) const {
            return (static_cast<size_t>(cell.x) << 16) | static_cast<size_t>(cell.y);
        }
    };
    
    // /**
 * WYDPathfinder
 * 
 * Implementa a funcionalidade WYDPathfinder conforme especificação original.
 * @param map Parâmetro map
 * @return Retorna Construtor
 */
 Construtor
    WYDPathfinder(WYDMap* map);
    
    // Destrutor
    ~WYDPathfinder();
    
    // Encontra um caminho entre pontos
    std::vector<std::pair<uint16_t, uint16_t>> findPath(
        uint16_t mapId, 
        uint16_t startX, uint16_t startY,
        uint16_t endX, uint16_t endY,
        uint16_t maxDistance = 0
    );
    
    // Versão que encontra o caminho até ficar próximo do alvo
    std::vector<std::pair<uint16_t, uint16_t>> findPathToEntity(
        uint16_t mapId,
        uint16_t startX, uint16_t startY,
        uint16_t targetX, uint16_t targetY,
        uint16_t targetRadius
    );
    
    // Verifica se um ponto é acessível diretamente (linha de visão)
    /**
 * hasLineOfSight
 * 
 * Implementa a funcionalidade hasLineOfSight conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param startX Parâmetro startX
 * @param startY Parâmetro startY
 * @param endX Parâmetro endX
 * @param endY Parâmetro endY
 * @return Retorna bool
 */

    bool hasLineOfSight(
        uint16_t mapId,
        uint16_t startX, uint16_t startY,
        uint16_t endX, uint16_t endY
    );
    
    // Encontra um ponto aleatório navegável próximo a um ponto de referência
    std::pair<uint16_t, uint16_t> findRandomPointNear(
        uint16_t mapId,
        uint16_t centerX, uint16_t centerY,
        uint16_t minDistance, uint16_t maxDistance
    );
    
    // Configurações
    /**
 * setMaxSearchNodes
 * 
 * Implementa a funcionalidade setMaxSearchNodes conforme especificação original.
 * @param maxNodes Parâmetro maxNodes
 */

    void setMaxSearchNodes(uint32_t maxNodes){ maxSearchNodes_ = maxNodes; }
    /**
 * enableCaching
 * 
 * Implementa a funcionalidade enableCaching conforme especificação original.
 * @param enable Parâmetro enable
 */

    void enableCaching(bool enable){ cachingEnabled_ = enable; }
    /**
 * clearCache
 * 
 * Implementa a funcionalidade clearCache conforme especificação original.
 */

    void clearCache(){ pathCache_.clear(); }
    
    // Utilitários
    /**
 * estimateDistance
 * 
 * Implementa a funcionalidade estimateDistance conforme especificação original.
 * @param x1 Parâmetro x1
 * @param y1 Parâmetro y1
 * @param x2 Parâmetro x2
 * @param y2 Parâmetro y2
 * @return Retorna float
 */

    float estimateDistance(
        uint16_t x1, uint16_t y1,
        uint16_t x2, uint16_t y2
    ) const;
    
    /**
 * isValidPosition
 * 
 * Implementa a funcionalidade isValidPosition conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    
    bool isValidPosition(uint16_t mapId, uint16_t x, uint16_t y) const;
    
private:
    // Referência ao mapa
    WYDMap* map_;
    
    // Configurações
    uint32_t maxSearchNodes_;
    bool cachingEnabled_;
    
    // Cache de caminhos recentes
    struct PathCacheKey {
        uint16_t mapId;
        uint16_t startX;
        uint16_t startY;
        uint16_t endX;
        uint16_t endY;
        
        bool operator==(const PathCacheKey& other) const {
            return mapId == other.mapId &&
                   startX == other.startX &&
                   startY == other.startY &&
                   endX == other.endX &&
                   endY == other.endY;
        }
    };
    
    struct PathCacheKeyHash {
        size_t operator()(const PathCacheKey& key) const {
            size_t h1 = std::hash<uint16_t>{}(key.mapId);
            size_t h2 = std::hash<uint16_t>{}(key.startX);
            size_t h3 = std::hash<uint16_t>{}(key.startY);
            size_t h4 = std::hash<uint16_t>{}(key.endX);
            size_t h5 = std::hash<uint16_t>{}(key.endY);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
        }
    };
    
    struct PathCacheEntry {
        std::vector<std::pair<uint16_t, uint16_t>> path;
        uint64_t timestamp;
    };
    
    std::unordered_map<PathCacheKey, PathCacheEntry, PathCacheKeyHash> pathCache_;
    
    // Implementação do algoritmo A*
    std::vector<std::pair<uint16_t, uint16_t>> findPathAStar(
        uint16_t mapId,
        uint16_t startX, uint16_t startY,
        uint16_t endX, uint16_t endY,
        uint16_t maxDistance,
        std::function<bool(uint16_t, uint16_t)> goalTest
    );
    
    // Implementação de busca de linha de visão (algoritmo de Bresenham)
    /**
 * checkLineOfSight
 * 
 * Implementa a funcionalidade checkLineOfSight conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param startX Parâmetro startX
 * @param startY Parâmetro startY
 * @param endX Parâmetro endX
 * @param endY Parâmetro endY
 * @return Retorna bool
 */

    bool checkLineOfSight(
        uint16_t mapId,
        uint16_t startX, uint16_t startY,
        uint16_t endX, uint16_t endY
    );
    
    // Constrói caminho a partir do nó final encontrado
    std::vector<std::pair<uint16_t, uint16_t>> reconstructPath(
        const std::unordered_map<NavCell, NavCell, NavCellHash>& cameFrom,
        const NavCell& current
    );
    
    // Obtém células vizinhas válidas para uma célula
    std::vector<NavCell> getNeighbors(uint16_t mapId, const NavCell& cell);
    
    // Verifica se uma posição específica é bloqueada
    /**
 * isBlocked
 * 
 * Implementa a funcionalidade isBlocked conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool isBlocked(uint16_t mapId, uint16_t x, uint16_t y) const;
    
    // Limpa entradas expiradas do cache
    /**
 * cleanupCache
 * 
 * Implementa a funcionalidade cleanupCache conforme especificação original.
 * @param currentTime Parâmetro currentTime
 */

    void cleanupCache(uint64_t currentTime);
};

#endif // WYD_PATHFINDER_H

} // namespace wydbr
