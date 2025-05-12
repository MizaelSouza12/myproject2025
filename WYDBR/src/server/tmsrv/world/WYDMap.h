/**
 * WYDMap.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/WYDMap.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_MAP_H
#define WYD_MAP_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <mutex>
#include <memory>

namespace wydbr {


/**
 * Flags de célula de mapa
 */
enum class CellFlag : uint8_t {
    BLOCKED   = 0x01, // Célula bloqueada (não caminhável)
    WATER     = 0x02, // Água
    SAFEZONE  = 0x04, // Zona segura
    PVPZONE   = 0x08, // Zona de PvP
    TELEPORT  = 0x10, // Ponto de teleporte
    CASTLE    = 0x20, // Área de castelo
    UNDERGROUND = 0x40, // Área subterrânea
    SPECIAL   = 0x80  // Área especial
};

// Forward declarations
class WorldManager;
class WYDHeightmap;
class WYDZone;
class WYDTrigger;
class WYDAtmosphere;

/**
 * Mapa do jogo
 * 
 * Representa um mapa completo do jogo, incluindo células, zonas, triggers,
 * entidades, heightmap, e outros elementos.
 */
/**
 * Classe WYDMap
 * 
 * Esta classe implementa a funcionalidade WYDMap conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDMap {
public:
    // /**
 * WYDMap
 * 
 * Implementa a funcionalidade WYDMap conforme especificação original.
 * @param worldManager Parâmetro worldManager
 * @param mapId Parâmetro mapId
 * @return Retorna Construtor
 */
 Construtor
    WYDMap(WorldManager* worldManager, const std::string& mapId);
    
    // Destrutor
    ~WYDMap();
    
    // Carregamento do mapa a partir de arquivo
    /**
 * loadFromFile
 * 
 * Implementa a funcionalidade loadFromFile conforme especificação original.
 * @param mapPath Parâmetro mapPath
 * @return Retorna bool
 */

    bool loadFromFile(const std::string& mapPath);
    
    // Inicialização de células
    /**
 * initializeCells
 * 
 * Implementa a funcionalidade initializeCells conforme especificação original.
 * @param width Parâmetro width
 * @param height Parâmetro height
 */

    void initializeCells(uint16_t width, uint16_t height);
    
    // Atualização de célula
    /**
 * updateCell
 * 
 * Implementa a funcionalidade updateCell conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param flags Parâmetro flags
 */

    void updateCell(uint16_t x, uint16_t y, uint8_t flags);
    
    // Propriedades básicas
    const std::string& getMapId() const { return mapId_; }
    const std::string& getName() const { return name_; }
    const std::string& getFilePath() const { return filePath_; }
    /**
 * getWidth
 * 
 * Implementa a funcionalidade getWidth conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getWidth() const{ return width_; }
    /**
 * getHeight
 * 
 * Implementa a funcionalidade getHeight conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getHeight() const{ return height_; }
    
    // Acesso a células
    /**
 * getCellFlags
 * 
 * Implementa a funcionalidade getCellFlags conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna uint8_t
 */

    uint8_t getCellFlags(uint16_t x, uint16_t y) const;
    /**
 * isValidPosition
 * 
 * Implementa a funcionalidade isValidPosition conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool isValidPosition(uint16_t x, uint16_t y) const;
    /**
 * isWalkable
 * 
 * Implementa a funcionalidade isWalkable conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool isWalkable(uint16_t x, uint16_t y) const;
    /**
 * hasCellFlag
 * 
 * Implementa a funcionalidade hasCellFlag conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param flag Parâmetro flag
 * @return Retorna bool
 */

    bool hasCellFlag(uint16_t x, uint16_t y, CellFlag flag) const;
    /**
 * isInSafeZone
 * 
 * Implementa a funcionalidade isInSafeZone conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool isInSafeZone(uint16_t x, uint16_t y) const;
    /**
 * isInCastleZone
 * 
 * Implementa a funcionalidade isInCastleZone conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool isInCastleZone(uint16_t x, uint16_t y) const;
    
    // Altura do terreno
    /**
 * getHeight
 * 
 * Implementa a funcionalidade getHeight conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna float
 */

    float getHeight(uint16_t x, uint16_t y) const;
    
    // Entidades no mapa
    /**
 * addEntity
 * 
 * Implementa a funcionalidade addEntity conforme especificação original.
 * @param entityId Parâmetro entityId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool addEntity(uint32_t entityId, uint16_t x, uint16_t y);
    /**
 * removeEntity
 * 
 * Implementa a funcionalidade removeEntity conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool removeEntity(uint32_t entityId);
    /**
 * moveEntity
 * 
 * Implementa a funcionalidade moveEntity conforme especificação original.
 * @param entityId Parâmetro entityId
 * @param newX Parâmetro newX
 * @param newY Parâmetro newY
 * @return Retorna bool
 */

    bool moveEntity(uint32_t entityId, uint16_t newX, uint16_t newY);
    /**
 * getEntityPosition
 * 
 * Implementa a funcionalidade getEntityPosition conforme especificação original.
 * @param entityId Parâmetro entityId
 * @param outX Parâmetro outX
 * @param outY Parâmetro outY
 * @return Retorna bool
 */

    bool getEntityPosition(uint32_t entityId, uint16_t& outX, uint16_t& outY) const;
    std::vector<uint32_t> getEntitiesInArea(uint16_t centerX, uint16_t centerY, uint16_t radius) const;
    
    // Pathfinding
    std::vector<std::pair<uint16_t, uint16_t>> findPath(
        uint16_t startX, uint16_t startY, 
        uint16_t endX, uint16_t endY, 
        uint16_t maxDistance = 50) const;
    
    // Verificação de linha de visão
    /**
 * hasLineOfSight
 * 
 * Implementa a funcionalidade hasLineOfSight conforme especificação original.
 * @param startX Parâmetro startX
 * @param startY Parâmetro startY
 * @param endX Parâmetro endX
 * @param endY Parâmetro endY
 * @return Retorna bool
 */

    bool hasLineOfSight(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY) const;
    
    // Zonas
    /**
 * getZone
 * 
 * Implementa a funcionalidade getZone conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna WYDZone
 */

    WYDZone* getZone(uint16_t x, uint16_t y);
    
    // Teleportes
    /**
 * hasTeleportTrigger
 * 
 * Implementa a funcionalidade hasTeleportTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param outDestMap Parâmetro outDestMap
 * @param outDestX Parâmetro outDestX
 * @param outDestY Parâmetro outDestY
 * @return Retorna bool
 */

    bool hasTeleportTrigger(uint16_t x, uint16_t y, std::string& outDestMap, uint16_t& outDestX, uint16_t& outDestY) const;
    
    // Sistema de altura
    /**
 * getHeightmap
 * 
 * Implementa a funcionalidade getHeightmap conforme especificação original.
 * @return Retorna WYDHeightmap
 */

    WYDHeightmap* getHeightmap() const{ return heightmap_.get(); }
    
    // Sistema de atmosfera
    /**
 * getAtmosphere
 * 
 * Implementa a funcionalidade getAtmosphere conforme especificação original.
 * @return Retorna WYDAtmosphere
 */

    WYDAtmosphere* getAtmosphere() const{ return atmosphere_.get(); }
    
    // Atualização do mapa
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp);
    
private:
    // Referência ao gerenciador de mundo
    WorldManager* worldManager_;
    
    // Propriedades básicas
    std::string mapId_;
    std::string name_;
    std::string filePath_;
    uint16_t width_;
    uint16_t height_;
    
    // Células do mapa (formato flat para melhor desempenho)
    std::vector<uint8_t> cells_;
    
    // Sistema de altura
    std::unique_ptr<WYDHeightmap> heightmap_;
    
    // Clima e atmosfera
    std::unique_ptr<WYDAtmosphere> atmosphere_;
    
    // Zonas do mapa
    std::vector<std::unique_ptr<WYDZone>> zones_;
    
    // Gatilhos (triggers)
    std::vector<std::unique_ptr<WYDTrigger>> triggers_;
    
    // Rastreamento de entidades
    std::unordered_map<uint32_t, std::pair<uint16_t, uint16_t>> entityPositions_;
    std::map<std::pair<uint16_t, uint16_t>, std::set<uint32_t>> positionEntities_;
    mutable std::mutex entityMutex_;
    
    // Métodos privados auxiliares
    /**
 * isInBounds
 * 
 * Implementa a funcionalidade isInBounds conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool isInBounds(uint16_t x, uint16_t y) const;
    /**
 * calculateDistance
 * 
 * Implementa a funcionalidade calculateDistance conforme especificação original.
 * @param x1 Parâmetro x1
 * @param y1 Parâmetro y1
 * @param x2 Parâmetro x2
 * @param y2 Parâmetro y2
 * @return Retorna float
 */

    float calculateDistance(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const;
    std::pair<uint16_t, uint16_t> getZoneIndices(uint16_t x, uint16_t y) const;
    
    // Criação de elementos do mapa
    /**
 * createDefaultZones
 * 
 * Implementa a funcionalidade createDefaultZones conforme especificação original.
 */

    void createDefaultZones();
    /**
 * createDefaultTriggers
 * 
 * Implementa a funcionalidade createDefaultTriggers conforme especificação original.
 */

    void createDefaultTriggers();
};

#endif // WYD_MAP_H

} // namespace wydbr
