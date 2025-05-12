/**
 * MapManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/world/MapManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _MAP_MANAGER_H_
#define _MAP_MANAGER_H_

/**
 * @file MapManager.h
 * @brief Sistema de gerenciamento de mapas - Compatível com o formato original
 * 
 * Implementa o sistema de gerenciamento de mapas do servidor WYD,
 * mantendo compatibilidade total com o formato .wys original do cliente,
 * enquanto adiciona funcionalidades avançadas para gestão mais eficiente.
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <memory>
#include <functional>

#include "../GlobalDef.h"
#include "../TM_Mob.h"
#include "../TM_Item.h"

namespace wydbr {

/**
 * @brief Estrutura de célula de mapa
 */
struct MapCell {
    uint8_t attribute;       // Atributo da célula
    uint8_t height;          // Altura da célula
    uint8_t eventId;         // ID de evento na célula
    uint8_t oceanAttribute;  // Atributo de oceano
    
    MapCell() : attribute(CELL_ATTR_EMPTY), height(0), eventId(0), oceanAttribute(0) {}
};

/**
 * @brief Informações de terreno de mapa
 */
struct MapTerrain {
    uint16_t baseX;          // Posição X base
    uint16_t baseY;          // Posição Y base
    uint8_t type;            // Tipo de terreno
    uint8_t height;          // Altura de terreno
    uint16_t width;          // Largura
    uint16_t length;         // Comprimento
    
    MapTerrain() : baseX(0), baseY(0), type(0), height(0), width(0), length(0) {}
};

/**
 * @brief Ponto de rota para NPCs
 */
struct MapRoutePoint {
    int16_t x;               // Posição X
    int16_t y;               // Posição Y
    uint16_t wait;           // Tempo de espera
    uint8_t velocity;        // Velocidade
    uint8_t action;          // Ação
    
    MapRoutePoint() : x(0), y(0), wait(0), velocity(0), action(0) {}
};

/**
 * @brief Rota para NPCs
 */
struct MapRoute {
    uint16_t routeId;                      // ID da rota
    uint8_t loopType;                      // Tipo de loop
    uint8_t numPoints;                     // Número de pontos
    std::vector<MapRoutePoint> points;     // Pontos da rota
    
    MapRoute() : routeId(0), loopType(0), numPoints(0) {}
};

/**
 * @brief Ponto de teleporte
 */
struct MapTeleport {
    uint16_t srcX;           // Posição X de origem
    uint16_t srcY;           // Posição Y de origem
    uint16_t dstX;           // Posição X de destino
    uint16_t dstY;           // Posição Y de destino
    uint16_t dstMap;         // Mapa de destino
    uint8_t srcRange;        // Alcance de origem
    uint8_t dstRange;        // Alcance de destino
    uint8_t condition;       // Condição de teleporte
    uint8_t level;           // Nível requerido
    uint32_t cost;           // Custo
    
    MapTeleport() : srcX(0), srcY(0), dstX(0), dstY(0), dstMap(0), 
                   srcRange(0), dstRange(0), condition(0), level(0), cost(0) {}
};

/**
 * @brief Informações de NPC no mapa
 */
struct MapNPC {
    uint16_t npcId;          // ID do NPC
    uint16_t x;              // Posição X
    uint16_t y;              // Posição Y
    uint8_t rotation;        // Rotação
    uint8_t eventId;         // ID de evento
    uint16_t instanceId;     // ID da instância
    
    MapNPC() : npcId(0), x(0), y(0), rotation(0), eventId(0), instanceId(0) {}
};

/**
 * @brief Definição de evento de mapa
 */
struct MapEvent {
    uint16_t eventId;            // ID do evento
    uint16_t type;               // Tipo de evento
    uint16_t x;                  // Posição X
    uint16_t y;                  // Posição Y
    uint16_t radius;             // Raio
    uint32_t triggerTime;        // Tempo de gatilho
    uint32_t keepTime;           // Tempo de permanência
    uint32_t parameter1;         // Parâmetro 1
    uint32_t parameter2;         // Parâmetro 2
    uint32_t parameter3;         // Parâmetro 3
    std::string script;          // Script
    
    MapEvent() : eventId(0), type(0), x(0), y(0), radius(0), 
                triggerTime(0), keepTime(0), parameter1(0), 
                parameter2(0), parameter3(0) {}
};

/**
 * @brief Definição de área no mapa
 */
struct MapArea {
    uint16_t areaId;             // ID da área
    uint16_t type;               // Tipo de área
    uint16_t x1;                 // Posição X inicial
    uint16_t y1;                 // Posição Y inicial
    uint16_t x2;                 // Posição X final
    uint16_t y2;                 // Posição Y final
    uint16_t parameter1;         // Parâmetro 1
    uint16_t parameter2;         // Parâmetro 2
    
    MapArea() : areaId(0), type(0), x1(0), y1(0), x2(0), y2(0), 
               parameter1(0), parameter2(0) {}
};

/**
 * @brief Definição de clima de mapa
 */
struct MapWeatherConfig {
    uint8_t weatherType;         // Tipo de clima
    uint8_t intensity;           // Intensidade
    uint16_t interval;           // Intervalo
    uint16_t duration;           // Duração
    
    MapWeatherConfig() : weatherType(WEATHER_CLEAR), intensity(0), 
                        interval(0), duration(0) {}
};

/**
 * @brief Cabeçalho do arquivo .wys
 */
struct WysHeader {
    char signature[4];           // Assinatura "WYS "
    uint16_t version;            // Versão
    uint16_t width;              // Largura
    uint16_t height;             // Altura
    uint32_t attribute;          // Atributos do mapa
    uint16_t numTerrains;        // Número de terrenos
    uint16_t numRoutes;          // Número de rotas
    uint16_t numTeleports;       // Número de teleportes
    uint16_t numNPCs;            // Número de NPCs
    uint16_t numEvents;          // Número de eventos
    uint16_t numAreas;           // Número de áreas
    uint8_t weatherType;         // Tipo de clima
    uint8_t weatherIntensity;    // Intensidade do clima
    uint16_t reserved[6];        // /**
 * WysHeader
 * 
 * Implementa a funcionalidade WysHeader conforme especificação original.
 * @return Retorna Reservado
 */
 Reservado
    
    WysHeader(){
        signature[0] = 'W'; 
        signature[1] = 'Y'; 
        signature[2] = 'S'; 
        signature[3] = ' ';
        version = 0x100;
        width = 4096;
        height = 4096;
        attribute = MAP_ATTR_NORMAL;
        numTerrains = 0;
        numRoutes = 0;
        numTeleports = 0;
        numNPCs = 0;
        numEvents = 0;
        numAreas = 0;
        weatherType = WEATHER_CLEAR;
        weatherIntensity = 0;
        memset(reserved, 0, sizeof(reserved));
    }
};

/**
 * @brief Cache de altura
 */
struct HeightCache {
    uint16_t x;               // Posição X
    uint16_t y;               // Posição Y
    uint8_t height;           // Altura
    
    HeightCache() : x(0), y(0), height(0) {}
};

/**
 * @brief Informações do mapa
 */
/**
 * Classe WYDMapInfo
 * 
 * Esta classe implementa a funcionalidade WYDMapInfo conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MapInfo {
public:
    /**
     * @brief Construtor
     * @param mapId ID do mapa
     * @param name Nome do mapa
     */
    MapInfo(uint16_t mapId, const std::string& name);
    
    /**
     * @brief Destrutor
     */
    ~MapInfo();
    
    /**
     * @brief Inicializa o mapa a partir de um arquivo .wys
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadFromWysFile(const std::string& filePath);
    
    /**
     * @brief Salva o mapa para um arquivo .wys
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveToWysFile(const std::string& filePath);
    
    /**
     * @brief Inicializa as células do mapa
     * @param width Largura
     * @param height Altura
     * @param defaultAttribute Atributo padrão
     * @return true se inicializado com sucesso
     */
    bool InitializeCells(uint16_t width, uint16_t height, uint8_t defaultAttribute = CELL_ATTR_EMPTY);
    
    /**
     * @brief Obtém o atributo de uma célula
     * @param x Posição X
     * @param y Posição Y
     * @return Atributo da célula
     */
    uint8_t GetCellAttribute(uint16_t x, uint16_t y) const;
    
    /**
     * @brief Define o atributo de uma célula
     * @param x Posição X
     * @param y Posição Y
     * @param attribute Atributo
     */
    void SetCellAttribute(uint16_t x, uint16_t y, uint8_t attribute);
    
    /**
     * @brief Obtém a altura de uma célula
     * @param x Posição X
     * @param y Posição Y
     * @return Altura da célula
     */
    uint8_t GetCellHeight(uint16_t x, uint16_t y) const;
    
    /**
     * @brief Define a altura de uma célula
     * @param x Posição X
     * @param y Posição Y
     * @param height Altura
     */
    void SetCellHeight(uint16_t x, uint16_t y, uint8_t height);
    
    /**
     * @brief Verifica se uma posição é válida
     * @param x Posição X
     * @param y Posição Y
     * @return true se válida
     */
    bool IsValidPosition(uint16_t x, uint16_t y) const;
    
    /**
     * @brief Verifica se uma entidade pode mover para uma posição
     * @param x Posição X
     * @param y Posição Y
     * @param isFlying Se a entidade está voando
     * @return true se pode mover
     */
    bool CanMoveToPosition(uint16_t x, uint16_t y, bool isFlying = false) const;
    
    /**
     * @brief Adiciona um terreno
     * @param terrain Terreno
     * @return true se adicionado com sucesso
     */
    bool AddTerrain(const MapTerrain& terrain);
    
    /**
     * @brief Adiciona uma rota
     * @param route Rota
     * @return true se adicionada com sucesso
     */
    bool AddRoute(const MapRoute& route);
    
    /**
     * @brief Adiciona um teleporte
     * @param teleport Teleporte
     * @return true se adicionado com sucesso
     */
    bool AddTeleport(const MapTeleport& teleport);
    
    /**
     * @brief Adiciona um NPC ao mapa
     * @param npc NPC
     * @return true se adicionado com sucesso
     */
    bool AddNPC(const MapNPC& npc);
    
    /**
     * @brief Adiciona um evento
     * @param event Evento
     * @return true se adicionado com sucesso
     */
    bool AddEvent(const MapEvent& event);
    
    /**
     * @brief Adiciona uma área
     * @param area Área
     * @return true se adicionada com sucesso
     */
    bool AddArea(const MapArea& area);
    
    /**
     * @brief Encontra terrenos em uma área
     * @param x1 Posição X inicial
     * @param y1 Posição Y inicial
     * @param x2 Posição X final
     * @param y2 Posição Y final
     * @return Lista de índices de terrenos
     */
    std::vector<int> FindTerrainsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const;
    
    /**
     * @brief Encontra teleportes em uma área
     * @param x1 Posição X inicial
     * @param y1 Posição Y inicial
     * @param x2 Posição X final
     * @param y2 Posição Y final
     * @return Lista de índices de teleportes
     */
    std::vector<int> FindTeleportsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const;
    
    /**
     * @brief Encontra NPCs em uma área
     * @param x1 Posição X inicial
     * @param y1 Posição Y inicial
     * @param x2 Posição X final
     * @param y2 Posição Y final
     * @return Lista de índices de NPCs
     */
    std::vector<int> FindNPCsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const;
    
    /**
     * @brief Encontra eventos em uma área
     * @param x1 Posição X inicial
     * @param y1 Posição Y inicial
     * @param x2 Posição X final
     * @param y2 Posição Y final
     * @return Lista de índices de eventos
     */
    std::vector<int> FindEventsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const;
    
    /**
     * @brief Encontra áreas em uma área
     * @param x1 Posição X inicial
     * @param y1 Posição Y inicial
     * @param x2 Posição X final
     * @param y2 Posição Y final
     * @return Lista de índices de áreas
     */
    std::vector<int> FindAreasInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const;
    
    /**
     * @brief Encontra uma rota pelo ID
     * @param routeId ID da rota
     * @return Ponteiro para a rota (nullptr se não encontrar)
     */
    const MapRoute* FindRouteById(uint16_t routeId) const;
    
    /**
     * @brief Encontra um teleporte pela posição de origem
     * @param x Posição X
     * @param y Posição Y
     * @return Índice do teleporte (-1 se não encontrar)
     */
    int FindTeleportByPosition(uint16_t x, uint16_t y) const;
    
    /**
     * @brief Atualiza o clima
     * @param type Tipo de clima
     * @param intensity Intensidade
     */
    void UpdateWeather(uint8_t type, uint8_t intensity);
    
    /**
     * @brief Calcula o caminho entre dois pontos
     * @param startX Posição X inicial
     * @param startY Posição Y inicial
     * @param endX Posição X final
     * @param endY Posição Y final
     * @param maxDistance Distância máxima
     * @param isFlying Se a entidade está voando
     * @return Lista de pontos do caminho
     */
    std::vector<std::pair<uint16_t, uint16_t>> CalculatePath(
        uint16_t startX, uint16_t startY, 
        uint16_t endX, uint16_t endY, 
        uint16_t maxDistance = 0xFFFF, 
        bool isFlying = false) const;
    
    /**
     * @brief Imprime informações do mapa
     * @param detailed Se deve imprimir detalhes
     */
    void PrintInfo(bool detailed = false) const;
    
    // Getters
    /**
 * GetMapId
 * 
 * Implementa a funcionalidade GetMapId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t GetMapId() const{ return mapId_; }
    const std::string& GetName() const { return name_; }
    /**
 * GetWidth
 * 
 * Implementa a funcionalidade GetWidth conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t GetWidth() const{ return width_; }
    /**
 * GetHeight
 * 
 * Implementa a funcionalidade GetHeight conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t GetHeight() const{ return height_; }
    /**
 * GetAttribute
 * 
 * Implementa a funcionalidade GetAttribute conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t GetAttribute() const{ return attribute_; }
    /**
 * GetWeatherType
 * 
 * Implementa a funcionalidade GetWeatherType conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t GetWeatherType() const{ return weatherType_; }
    /**
 * GetWeatherIntensity
 * 
 * Implementa a funcionalidade GetWeatherIntensity conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t GetWeatherIntensity() const{ return weatherIntensity_; }
    const std::vector<MapTerrain>& GetTerrains() const { return terrains_; }
    const std::vector<MapRoute>& GetRoutes() const { return routes_; }
    const std::vector<MapTeleport>& GetTeleports() const { return teleports_; }
    const std::vector<MapNPC>& GetNPCs() const { return npcs_; }
    const std::vector<MapEvent>& GetEvents() const { return events_; }
    const std::vector<MapArea>& GetAreas() const { return areas_; }
    
    // Setters
    /**
 * SetName
 * 
 * Implementa a funcionalidade SetName conforme especificação original.
 * @param name Parâmetro name
 */

    void SetName(const std::string& name){ name_ = name; }
    /**
 * SetAttribute
 * 
 * Implementa a funcionalidade SetAttribute conforme especificação original.
 * @param attribute Parâmetro attribute
 */

    void SetAttribute(uint32_t attribute){ attribute_ = attribute; }
    
private:
    uint16_t mapId_;                      // ID do mapa
    std::string name_;                    // Nome do mapa
    uint16_t width_;                      // Largura
    uint16_t height_;                     // Altura
    uint32_t attribute_;                  // Atributos do mapa
    uint8_t weatherType_;                 // Tipo de clima
    uint8_t weatherIntensity_;            // Intensidade do clima
    
    // Dados principais
    std::vector<MapCell> cells_;          // Células
    std::vector<MapTerrain> terrains_;    // Terrenos
    std::vector<MapRoute> routes_;        // Rotas
    std::vector<MapTeleport> teleports_;  // Teleportes
    std::vector<MapNPC> npcs_;            // NPCs
    std::vector<MapEvent> events_;        // Eventos
    std::vector<MapArea> areas_;          // Áreas
    
    // Cache de altura para otimização
    mutable std::vector<HeightCache> heightCache_;
    
    // Estrutura de segmentos para otimização de pesquisa
    struct Segment {
        std::set<int> terrains;
        std::set<int> teleports;
        std::set<int> npcs;
        std::set<int> events;
        std::set<int> areas;
    };
    std::vector<std::vector<Segment>> segments_;
    
    /**
     * @brief Inicializa segmentos
     */
    void InitializeSegments();
    
    /**
     * @brief Obtém índices dos segmentos que contêm uma área
     * @param x1 Posição X inicial
     * @param y1 Posição Y inicial
     * @param x2 Posição X final
     * @param y2 Posição Y final
     * @return Lista de pares de índices (segX, segY)
     */
    std::vector<std::pair<int, int>> GetSegmentsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const;
    
    /**
     * @brief Lê estruturas do arquivo .wys
     * @param buffer Ponteiro para o buffer
     * @param size Tamanho do buffer
     * @return true se lido com sucesso
     */
    bool ReadWysStructures(const uint8_t* buffer, size_t size);
    
    /**
     * @brief Escreve estruturas no buffer
     * @param buffer Ponteiro para o buffer
     * @param bufferSize Tamanho do buffer
     * @param bytesWritten Número de bytes escritos
     * @return true se escrito com sucesso
     */
    bool WriteWysStructures(uint8_t* buffer, size_t bufferSize, size_t& bytesWritten);
    
    /**
     * @brief Calcula índice de célula a partir das coordenadas
     * @param x Posição X
     * @param y Posição Y
     * @return Índice da célula
     */
    size_t CellIndex(uint16_t x, uint16_t y) const;
};

/**
 * @brief Gerenciador de mapas
 * 
 * Classe singleton responsável por gerenciar todos os mapas do servidor
 */
/**
 * Classe WYDMapManager
 * 
 * Esta classe implementa a funcionalidade WYDMapManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MapManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Referência para a instância
     */
    static MapManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param mapDir Diretório de mapas
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& mapDir = "./maps/");
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Finalize();
    
    /**
     * @brief Carrega um mapa a partir de um arquivo .wys
     * @param mapId ID do mapa
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadMap(uint16_t mapId, const std::string& filePath);
    
    /**
     * @brief Carrega todos os mapas de um diretório
     * @param dirPath Caminho do diretório
     * @return Número de mapas carregados
     */
    int LoadAllMaps(const std::string& dirPath);
    
    /**
     * @brief Descarrega um mapa
     * @param mapId ID do mapa
     * @return true se descarregado com sucesso
     */
    bool UnloadMap(uint16_t mapId);
    
    /**
     * @brief Salva um mapa para um arquivo .wys
     * @param mapId ID do mapa
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveMap(uint16_t mapId, const std::string& filePath);
    
    /**
     * @brief Salva todos os mapas
     * @param dirPath Caminho do diretório
     * @return Número de mapas salvos
     */
    int SaveAllMaps(const std::string& dirPath);
    
    /**
     * @brief Cria um novo mapa
     * @param mapId ID do mapa
     * @param name Nome do mapa
     * @param width Largura
     * @param height Altura
     * @param defaultAttribute Atributo padrão
     * @return true se criado com sucesso
     */
    bool CreateMap(uint16_t mapId, const std::string& name, uint16_t width = 4096, 
                 uint16_t height = 4096, uint8_t defaultAttribute = CELL_ATTR_EMPTY);
    
    /**
     * @brief Obtém um mapa pelo ID
     * @param mapId ID do mapa
     * @return Ponteiro para o mapa (nullptr se não existir)
     */
    MapInfo* GetMap(uint16_t mapId);
    
    /**
     * @brief Obtém um mapa pelo ID (constante)
     * @param mapId ID do mapa
     * @return Ponteiro constante para o mapa (nullptr se não existir)
     */
    const MapInfo* GetMap(uint16_t mapId) const;
    
    /**
     * @brief Processa a movimentação de uma entidade
     * @param entityId ID da entidade
     * @param entityType Tipo da entidade
     * @param mapId ID do mapa
     * @param srcX Posição X de origem
     * @param srcY Posição Y de origem
     * @param dstX Posição X de destino
     * @param dstY Posição Y de destino
     * @param isFlying Se a entidade está voando
     * @return true se pode mover
     */
    bool ProcessMovement(int entityId, int entityType, uint16_t mapId, 
                        uint16_t srcX, uint16_t srcY, 
                        uint16_t dstX, uint16_t dstY, 
                        bool isFlying = false);
    
    /**
     * @brief Processa um teleporte
     * @param entityId ID da entidade
     * @param entityType Tipo da entidade
     * @param mapId ID do mapa atual
     * @param x Posição X atual
     * @param y Posição Y atual
     * @param outMapId ID do mapa de destino
     * @param outX Posição X de destino
     * @param outY Posição Y de destino
     * @return true se teleportado com sucesso
     */
    bool ProcessTeleport(int entityId, int entityType, uint16_t mapId, 
                        uint16_t x, uint16_t y, 
                        uint16_t& outMapId, uint16_t& outX, uint16_t& outY);
    
    /**
     * @brief Verifica se uma posição é válida
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @return true se válida
     */
    bool IsValidPosition(uint16_t mapId, uint16_t x, uint16_t y) const;
    
    /**
     * @brief Verifica se uma entidade pode mover para uma posição
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @param isFlying Se a entidade está voando
     * @return true se pode mover
     */
    bool CanMoveToPosition(uint16_t mapId, uint16_t x, uint16_t y, bool isFlying = false) const;
    
    /**
     * @brief Obtém os atributos de um mapa
     * @param mapId ID do mapa
     * @return Atributos do mapa (0 se não existir)
     */
    uint32_t GetMapAttributes(uint16_t mapId) const;
    
    /**
     * @brief Verifica se um mapa tem um atributo específico
     * @param mapId ID do mapa
     * @param attribute Atributo
     * @return true se tiver o atributo
     */
    bool HasMapAttribute(uint16_t mapId, MapAttribute attribute) const;
    
    /**
     * @brief Obtém o atributo de uma célula
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @return Atributo da célula
     */
    uint8_t GetCellAttribute(uint16_t mapId, uint16_t x, uint16_t y) const;
    
    /**
     * @brief Verifica se uma célula tem um atributo específico
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @param attribute Atributo
     * @return true se tiver o atributo
     */
    bool HasCellAttribute(uint16_t mapId, uint16_t x, uint16_t y, CellAttribute attribute) const;
    
    /**
     * @brief Obtém a altura de uma célula
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @return Altura da célula
     */
    uint8_t GetCellHeight(uint16_t mapId, uint16_t x, uint16_t y) const;
    
    /**
     * @brief Calculou o caminho entre dois pontos
     * @param mapId ID do mapa
     * @param startX Posição X inicial
     * @param startY Posição Y inicial
     * @param endX Posição X final
     * @param endY Posição Y final
     * @param maxDistance Distância máxima
     * @param isFlying Se a entidade está voando
     * @return Lista de pontos do caminho
     */
    std::vector<std::pair<uint16_t, uint16_t>> CalculatePath(
        uint16_t mapId, uint16_t startX, uint16_t startY, 
        uint16_t endX, uint16_t endY, 
        uint16_t maxDistance = 0xFFFF, 
        bool isFlying = false) const;
    
    /**
     * @brief Atualiza o clima de um mapa
     * @param mapId ID do mapa
     * @param type Tipo de clima
     * @param intensity Intensidade
     * @return true se atualizado com sucesso
     */
    bool UpdateMapWeather(uint16_t mapId, uint8_t type, uint8_t intensity);
    
    /**
     * @brief Obtém os NPCs de um mapa
     * @param mapId ID do mapa
     * @return Lista de NPCs
     */
    std::vector<MapNPC> GetMapNPCs(uint16_t mapId) const;
    
    /**
     * @brief Obtém as rotas de um mapa
     * @param mapId ID do mapa
     * @return Lista de rotas
     */
    std::vector<MapRoute> GetMapRoutes(uint16_t mapId) const;
    
    /**
     * @brief Obtém os teleportes de um mapa
     * @param mapId ID do mapa
     * @return Lista de teleportes
     */
    std::vector<MapTeleport> GetMapTeleports(uint16_t mapId) const;
    
    /**
     * @brief Obtém as áreas de um mapa
     * @param mapId ID do mapa
     * @return Lista de áreas
     */
    std::vector<MapArea> GetMapAreas(uint16_t mapId) const;
    
    /**
     * @brief Registra um callback para teleporte
     * @param callback Função de callback
     */
    void RegisterTeleportCallback(std::function<void(int, int, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)> callback);
    
    /**
     * @brief Registra um callback para célula de evento
     * @param callback Função de callback
     */
    void RegisterEventCellCallback(std::function<void(int, int, uint16_t, uint16_t, uint16_t, uint8_t)> callback);
    
    /**
     * @brief Registra um callback para mudança de clima
     * @param callback Função de callback
     */
    void RegisterWeatherChangeCallback(std::function<void(uint16_t, uint8_t, uint8_t)> callback);
    
    /**
     * @brief Processa um tick de eventos
     * @param currentTick Tick atual
     */
    void ProcessEvents(uint32_t currentTick);
    
    /**
     * @brief Imprime informações de todos os mapas
     * @param detailed Se deve imprimir detalhes
     */
    void PrintAllMaps(bool detailed = false) const;
    
    /**
     * @brief Obtém a lista de IDs de mapas carregados
     * @return Lista de IDs de mapas
     */
    std::vector<uint16_t> GetLoadedMapIds() const;
    
/**
 * MapManager
 * 
 * Implementa a funcionalidade MapManager conforme especificação original.
 * @return Retorna private:
 */

    
private:
    MapManager();  // Construtor privado (singleton)
    ~MapManager(); // Destrutor privado (singleton)
    
    static MapManager* instance_;         // Instância única (singleton)
    bool initialized_;                     // Flag de inicialização
    std::string mapDir_;                   // Diretório de mapas
    
    // Dados principais
    std::map<uint16_t, std::unique_ptr<MapInfo>> maps_; // Mapas por ID
    
    // Dados temporários
    struct ScheduledWeather {
        uint16_t mapId;
        uint8_t type;
        uint8_t intensity;
        uint32_t startTime;
        uint32_t endTime;
    };
    std::vector<ScheduledWeather> scheduledWeather_; // Climas agendados
    
    // Callbacks
    std::function<void(int, int, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)> teleportCallback_;
    std::function<void(int, int, uint16_t, uint16_t, uint16_t, uint8_t)> eventCellCallback_;
    std::function<void(uint16_t, uint8_t, uint8_t)> weatherChangeCallback_;
    
    // Mutex para proteção de dados
    mutable std::mutex mapMutex_;
    
    // Desabilita cópias (singleton)
    MapManager(const MapManager&) = delete;
    MapManager& operator=(const MapManager&) = delete;
};

} // namespace wydbr

#endif // _MAP_MANAGER_H_