#ifndef WYDSTUDIO_WORLD_ANALYZER_H
#define WYDSTUDIO_WORLD_ANALYZER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <array>
#include <bitset>
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace world {

/**
 * @brief Representa coordenadas no mundo do jogo
 */
struct WorldCoordinate {
    int32_t x;
    int32_t y;
    int32_t z;
    uint32_t mapId;
    
    WorldCoordinate() : x(0), y(0), z(0), mapId(0) {}
    WorldCoordinate(int32_t _x, int32_t _y, int32_t _z, uint32_t _mapId)
        : x(_x), y(_y), z(_z), mapId(_mapId) {}
    
    // Funções de distância e operadores
    float distanceTo(const WorldCoordinate& other) const;
    float distanceToXZ(const WorldCoordinate& other) const;
    bool operator==(const WorldCoordinate& other) const;
    bool operator!=(const WorldCoordinate& other) const;
    
    // Conversões
    std::string toString() const;
    static WorldCoordinate fromString(const std::string& str);
};

/**
 * @brief Atributos de terreno para uma posição no mundo
 */
struct TerrainAttributes {
    enum class Attribute : uint16_t {
        NONE                = 0x0000,
        WALKABLE            = 0x0001,  // Pode andar
        WATER               = 0x0002,  // Água
        CLIFF               = 0x0004,  // Penhasco
        DEEP_WATER          = 0x0008,  // Água profunda
        PVP_ALLOWED         = 0x0010,  // PVP permitido
        DUNGEON             = 0x0020,  // Dungeon
        TELEPORT            = 0x0040,  // Ponto de teleporte
        INDOOR              = 0x0080,  // Área interna
        SAFE_ZONE           = 0x0100,  // Zona segura
        TOWN                = 0x0200,  // Cidade
        FARM_ALLOWED        = 0x0400,  // Permitido uso de farm
        MOUNT_FORBIDDEN     = 0x0800,  // Proibido usar montaria
        FLY_ALLOWED         = 0x1000,  // Permitido voar
        GUILD_AREA          = 0x2000,  // Área de guild
        EVENT_AREA          = 0x4000,  // Área de evento
        CUSTOM              = 0x8000   // Atributo customizado
    };
    
    std::bitset<16> attributes;
    uint8_t groundType;     // Tipo de terreno (0 = normal, 1 = grama, 2 = neve, etc.)
    uint8_t visualEffect;   // Efeito visual ao andar (0 = nenhum, 1 = poeira, 2 = água, etc.)
    uint8_t soundEffect;    // Efeito sonoro ao andar (0 = normal, 1 = grama, 2 = neve, etc.)
    uint8_t weatherEffect;  // Efeito climático (0 = nenhum, 1 = chuva, 2 = neve, etc.)
    
    TerrainAttributes() : groundType(0), visualEffect(0), soundEffect(0), weatherEffect(0) {}
    
    bool hasAttribute(Attribute attr) const {
        return attributes.test(static_cast<size_t>(attr));
    }
    
    void setAttribute(Attribute attr, bool value = true) {
        attributes.set(static_cast<size_t>(attr), value);
    }
    
    uint16_t getRawAttributes() const {
        return static_cast<uint16_t>(attributes.to_ulong());
    }
    
    void setRawAttributes(uint16_t rawAttrs) {
        attributes = std::bitset<16>(rawAttrs);
    }
    
    bool isWalkable() const { return hasAttribute(Attribute::WALKABLE); }
    bool isWater() const { return hasAttribute(Attribute::WATER); }
    bool isDeepWater() const { return hasAttribute(Attribute::DEEP_WATER); }
    bool isPvpAllowed() const { return hasAttribute(Attribute::PVP_ALLOWED); }
    bool isSafeZone() const { return hasAttribute(Attribute::SAFE_ZONE); }
};

/**
 * @brief Informações básicas sobre um mapa do jogo
 */
struct MapInfo {
    uint32_t id;
    std::string name;
    std::string filename;
    uint32_t width;
    uint32_t height;
    
    struct Bounds {
        int32_t minX;
        int32_t minY;
        int32_t minZ;
        int32_t maxX;
        int32_t maxY;
        int32_t maxZ;
    } bounds;
    
    std::vector<uint32_t> connectedMaps;  // IDs de mapas conectados
    std::vector<std::pair<WorldCoordinate, WorldCoordinate>> portals;  // Pontos de teleporte: origem -> destino
    
    uint32_t defaultRespawnMapId;  // Mapa para respawn quando morre neste mapa
    WorldCoordinate defaultRespawnPoint;  // Ponto de respawn padrão
    
    // Atributos do mapa
    bool isPvpMap;
    bool isDungeon;
    bool isInstanceMap;
    uint32_t minLevel;
    uint32_t maxLevel;
    uint32_t baseMonsterLevel;
    
    MapInfo() : id(0), width(0), height(0), defaultRespawnMapId(0),
                isPvpMap(false), isDungeon(false), isInstanceMap(false),
                minLevel(0), maxLevel(0), baseMonsterLevel(0) {
        bounds = {0, 0, 0, 0, 0, 0};
    }
    
    bool isPointInBounds(int32_t x, int32_t y, int32_t z) const {
        return x >= bounds.minX && x <= bounds.maxX &&
               y >= bounds.minY && y <= bounds.maxY &&
               z >= bounds.minZ && z <= bounds.maxZ;
    }
    
    bool isCoordinateInBounds(const WorldCoordinate& coord) const {
        return isPointInBounds(coord.x, coord.y, coord.z);
    }
};

/**
 * @brief Representa áreas especiais dentro de um mapa
 */
struct SpecialArea {
    enum class Type {
        NONE,
        TOWN,
        SAFE_ZONE,
        PVP_ZONE,
        EVENT_AREA,
        BOSS_AREA,
        GUILD_TERRITORY,
        RESOURCE_AREA,
        QUEST_AREA,
        DANGER_ZONE,
        RESTRICTED_AREA,
        CUSTOM
    };
    
    uint32_t id;
    std::string name;
    Type type;
    uint32_t mapId;
    
    // Representação da área
    enum class ShapeType {
        RECTANGLE,
        CIRCLE,
        POLYGON,
        IRREGULAR
    } shapeType;
    
    // Para retângulos
    struct {
        int32_t minX, minY, minZ;
        int32_t maxX, maxY, maxZ;
    } rectangle;
    
    // Para círculos
    struct {
        int32_t centerX, centerY, centerZ;
        float radius;
    } circle;
    
    // Para polígonos
    std::vector<WorldCoordinate> polygonVertices;
    
    // Atributos da área
    std::unordered_map<std::string, std::string> attributes;
    
    // Verificadores de área
    bool containsPoint(int32_t x, int32_t y, int32_t z) const;
    bool containsCoordinate(const WorldCoordinate& coord) const;
    float distanceToPoint(int32_t x, int32_t y, int32_t z) const;
    float distanceToCoordinate(const WorldCoordinate& coord) const;
};

/**
 * @brief Informações sobre um objeto no mundo
 */
struct WorldObject {
    enum class Type {
        NONE,
        NPC,
        MONSTER,
        GATHERABLE,
        CHEST,
        DOOR,
        PORTAL,
        SIGN,
        DECORATIVE,
        INTERACTIVE,
        TRAP,
        TRIGGER,
        CUSTOM
    };
    
    uint32_t id;
    std::string name;
    Type type;
    WorldCoordinate position;
    float rotation;
    float scale;
    
    uint32_t modelId;
    uint32_t textureId;
    
    // Atributos específicos de tipo
    std::unordered_map<std::string, std::string> attributes;
    
    // Metadados
    uint32_t spawnGroupId;  // Grupo de spawn ao qual pertence
    uint32_t respawnTime;   // Tempo de respawn em segundos
    bool isUnique;          // Se é um objeto único no mundo
    
    WorldObject() : id(0), type(Type::NONE), rotation(0.0f), scale(1.0f),
                   modelId(0), textureId(0), spawnGroupId(0), respawnTime(0),
                   isUnique(false) {}
};

/**
 * @brief Grupo de spawn para objetos no mundo
 */
struct SpawnGroup {
    uint32_t id;
    std::string name;
    uint32_t mapId;
    
    // Área de spawn
    enum class SpawnAreaType {
        POINT,
        RECTANGLE,
        CIRCLE,
        RANDOM_POINTS
    } areaType;
    
    WorldCoordinate center;
    float radius;
    std::vector<WorldCoordinate> spawnPoints;
    
    // Configurações de spawn
    uint32_t maxObjects;          // Máximo de objetos ativos
    uint32_t respawnTimeMin;      // Tempo mínimo de respawn em segundos
    uint32_t respawnTimeMax;      // Tempo máximo de respawn em segundos
    uint32_t activationRadius;    // Raio de ativação em unidades de mundo
    
    // Tipos de objetos que podem spawnar
    std::vector<std::pair<uint32_t, float>> objectTypesWithProbability;  // ID do objeto, probabilidade
    
    // Cronograma
    struct TimeSchedule {
        bool enabled;
        uint32_t startHour;
        uint32_t endHour;
        std::bitset<7> activeDays;  // Dias da semana (0 = Domingo, 6 = Sábado)
    } schedule;
    
    SpawnGroup() : id(0), mapId(0), areaType(SpawnAreaType::POINT),
                 radius(0.0f), maxObjects(1), respawnTimeMin(300),
                 respawnTimeMax(600), activationRadius(100) {
        schedule.enabled = false;
        schedule.startHour = 0;
        schedule.endHour = 24;
        schedule.activeDays.set();  // Todos os dias ativos
    }
};

/**
 * @brief Rota de patrulha para NPCs e monstros
 */
struct PatrolRoute {
    uint32_t id;
    std::string name;
    uint32_t mapId;
    
    struct RoutePoint {
        WorldCoordinate position;
        uint32_t waitTime;  // Tempo de espera em milissegundos
        std::string action; // Ação a executar no ponto
    };
    
    std::vector<RoutePoint> points;
    bool isLoop;  // Se a rota reinicia após o último ponto
    float moveSpeed;  // Velocidade de movimento entre pontos
    
    PatrolRoute() : id(0), mapId(0), isLoop(true), moveSpeed(1.0f) {}
};

/**
 * @brief Estatísticas e métricas sobre um mapa
 */
struct MapMetrics {
    // Cobertura de terreno
    float walkablePercentage;
    float waterPercentage;
    float cliffPercentage;
    
    // Densidade de objetos
    uint32_t npcCount;
    uint32_t monsterCount;
    uint32_t resourceCount;
    uint32_t interactiveObjectCount;
    
    // Análises de caminho
    float averagePathLength;
    float pathComplexity;
    
    // Métricas de jogabilidade
    float explorationDifficulty;  // 0.0-1.0
    float combatDensity;          // 0.0-1.0
    float resourceDensity;        // 0.0-1.0
    float navigationComplexity;   // 0.0-1.0
    
    // Questões de design
    uint32_t unreachableAreaCount;
    uint32_t poorlyConnectedAreaCount;
    uint32_t crowdedAreaCount;
    uint32_t emptyAreaCount;
    
    MapMetrics() : walkablePercentage(0.0f), waterPercentage(0.0f), cliffPercentage(0.0f),
                 npcCount(0), monsterCount(0), resourceCount(0), interactiveObjectCount(0),
                 averagePathLength(0.0f), pathComplexity(0.0f),
                 explorationDifficulty(0.0f), combatDensity(0.0f), resourceDensity(0.0f),
                 navigationComplexity(0.0f), unreachableAreaCount(0), poorlyConnectedAreaCount(0),
                 crowdedAreaCount(0), emptyAreaCount(0) {}
};

/**
 * @brief Resultados de um caminho encontrado no mundo
 */
struct PathResult {
    enum class Status {
        SUCCESS,
        PARTIAL,
        FAILED_NO_PATH,
        FAILED_UNREACHABLE_DESTINATION,
        FAILED_INVALID_START,
        FAILED_INVALID_END,
        FAILED_TIMEOUT,
        FAILED_ERROR
    };
    
    Status status;
    std::vector<WorldCoordinate> path;
    float totalDistance;
    uint32_t computationTimeMs;
    std::string errorMessage;
    
    PathResult() : status(Status::FAILED_ERROR), totalDistance(0.0f), computationTimeMs(0) {}
};

/**
 * @brief Opções para análise de caminho
 */
struct PathfindingOptions {
    enum class Algorithm {
        A_STAR,
        DIJKSTRA,
        BREADTH_FIRST,
        JUMP_POINT_SEARCH,
        THETA_STAR,
        LAZY_THETA_STAR,
        CUSTOM
    };
    
    Algorithm algorithm;
    float heuristicWeight;       // Peso da heurística para A*
    uint32_t maxIterations;      // Número máximo de iterações
    uint32_t timeoutMs;          // Timeout em milissegundos
    bool allowDiagonals;         // Permitir movimento diagonal
    bool cutCorners;             // Permitir cortar cantos
    bool smoothPath;             // Suavizar caminho resultante
    float heightTolerance;       // Tolerância para diferenças de altura
    float obstacleAvoidance;     // 0.0 = ignorar obstáculos, 1.0 = evitar completamente
    
    PathfindingOptions() : algorithm(Algorithm::A_STAR), heuristicWeight(1.0f),
                         maxIterations(10000), timeoutMs(5000), allowDiagonals(true),
                         cutCorners(false), smoothPath(true), heightTolerance(1.0f),
                         obstacleAvoidance(1.0f) {}
};

/**
 * @brief Resultado de uma análise de mundo
 */
struct WorldAnalysisResult {
    enum class Status {
        SUCCESS,
        PARTIAL,
        FAILED,
        TIMEOUT
    };
    
    Status status;
    std::string errorMessage;
    
    // Métricas por mapa
    std::unordered_map<uint32_t, MapMetrics> mapMetrics;
    
    // Problemas encontrados
    struct WorldIssue {
        enum class Type {
            UNREACHABLE_AREA,
            POORLY_CONNECTED_AREA,
            RESOURCE_IMBALANCE,
            MONSTER_DENSITY_IMBALANCE,
            NPC_PLACEMENT_ISSUE,
            TERRAIN_ISSUE,
            BOTTLENECK,
            EMPTY_AREA,
            OVERCROWDED_AREA,
            UNREACHABLE_OBJECT,
            FLOATING_OBJECT,
            CUSTOM
        };
        
        Type type;
        std::string description;
        uint32_t mapId;
        std::optional<WorldCoordinate> location;
        float severity;  // 0.0-1.0
        bool isCritical;
    };
    
    std::vector<WorldIssue> issues;
    
    // Estatísticas globais
    uint32_t totalMapsAnalyzed;
    uint32_t totalAreaAnalyzed;
    uint32_t totalObjectsAnalyzed;
    uint32_t analysisTimeMs;
    
    WorldAnalysisResult() : status(Status::FAILED), totalMapsAnalyzed(0),
                          totalAreaAnalyzed(0), totalObjectsAnalyzed(0),
                          analysisTimeMs(0) {}
};

/**
 * @brief Opções para análise de mundo
 */
struct WorldAnalysisOptions {
    enum class AnalysisType {
        BASIC,
        STANDARD,
        COMPREHENSIVE,
        PERFORMANCE,
        CUSTOM
    };
    
    AnalysisType type;
    
    // Mapas a analisar
    std::vector<uint32_t> mapIds;  // Vazio = todos os mapas
    
    // Aspectos a analisar
    bool analyzeNavigation;     // Análise de navegação
    bool analyzeResourceDistribution;  // Distribuição de recursos
    bool analyzeMonsterPlacement;      // Posicionamento de monstros
    bool analyzeNpcPlacement;          // Posicionamento de NPCs
    bool analyzeTerrainBalance;        // Balanceamento de terreno
    bool analyzePacing;                // Análise de ritmo
    bool analyzeConnectivity;          // Conectividade entre áreas
    
    // Limiares para alertas
    float resourceImbalanceThreshold;   // 0.0-1.0
    float monsterDensityThreshold;      // 0.0-1.0
    float emptyAreaThreshold;           // 0.0-1.0
    float crowdedAreaThreshold;         // 0.0-1.0
    float poorConnectivityThreshold;    // 0.0-1.0
    
    // Opções de desempenho
    uint32_t sampleDensity;     // Densidade de amostragem para análise
    uint32_t timeoutMs;         // Timeout em milissegundos
    
    WorldAnalysisOptions() : type(AnalysisType::STANDARD),
                           analyzeNavigation(true), analyzeResourceDistribution(true),
                           analyzeMonsterPlacement(true), analyzeNpcPlacement(true),
                           analyzeTerrainBalance(true), analyzePacing(true),
                           analyzeConnectivity(true), resourceImbalanceThreshold(0.3f),
                           monsterDensityThreshold(0.3f), emptyAreaThreshold(0.7f),
                           crowdedAreaThreshold(0.7f), poorConnectivityThreshold(0.3f),
                           sampleDensity(100), timeoutMs(60000) {}
    
    static WorldAnalysisOptions createBasic() {
        WorldAnalysisOptions options;
        options.type = AnalysisType::BASIC;
        options.analyzeResourceDistribution = false;
        options.analyzePacing = false;
        options.analyzeTerrainBalance = false;
        options.sampleDensity = 50;
        return options;
    }
    
    static WorldAnalysisOptions createComprehensive() {
        WorldAnalysisOptions options;
        options.type = AnalysisType::COMPREHENSIVE;
        options.resourceImbalanceThreshold = 0.2f;
        options.monsterDensityThreshold = 0.2f;
        options.emptyAreaThreshold = 0.5f;
        options.crowdedAreaThreshold = 0.5f;
        options.poorConnectivityThreshold = 0.2f;
        options.sampleDensity = 200;
        options.timeoutMs = 300000;
        return options;
    }
    
    static WorldAnalysisOptions createPerformance() {
        WorldAnalysisOptions options;
        options.type = AnalysisType::PERFORMANCE;
        options.analyzeResourceDistribution = false;
        options.analyzeNpcPlacement = false;
        options.analyzeTerrainBalance = false;
        options.analyzePacing = false;
        options.resourceImbalanceThreshold = 0.5f;
        options.monsterDensityThreshold = 0.5f;
        options.emptyAreaThreshold = 0.9f;
        options.crowdedAreaThreshold = 0.9f;
        options.poorConnectivityThreshold = 0.5f;
        options.sampleDensity = 20;
        options.timeoutMs = 30000;
        return options;
    }
};

/**
 * @brief Analisador de mundo para o WYD
 */
class WorldAnalyzer {
public:
    WorldAnalyzer();
    ~WorldAnalyzer();
    
    // Carregamento de dados de mundo
    bool loadMapData(const std::string& directory);
    bool loadSingleMap(uint32_t mapId, const std::string& filePath);
    bool loadHeightMap(uint32_t mapId, const std::string& filePath);
    bool loadAttributeMap(uint32_t mapId, const std::string& filePath);
    bool loadObjectData(uint32_t mapId, const std::string& filePath);
    bool loadSpecialAreas(uint32_t mapId, const std::string& filePath);
    bool loadSpawnGroups(uint32_t mapId, const std::string& filePath);
    bool loadPatrolRoutes(uint32_t mapId, const std::string& filePath);
    
    // Verificação de caminho
    PathResult findPath(const WorldCoordinate& start, 
                       const WorldCoordinate& end,
                       const PathfindingOptions& options = PathfindingOptions());
    
    bool isPathPossible(const WorldCoordinate& start, 
                       const WorldCoordinate& end,
                       float* outDistance = nullptr);
    
    float getPathDistance(const WorldCoordinate& start, 
                         const WorldCoordinate& end);
    
    // Consultas de terreno
    float getHeightAt(uint32_t mapId, int32_t x, int32_t z);
    TerrainAttributes getTerrainAttributesAt(uint32_t mapId, int32_t x, int32_t z);
    bool isWalkable(uint32_t mapId, int32_t x, int32_t z);
    
    // Consultas de área
    std::vector<SpecialArea> getAreasAtPoint(uint32_t mapId, int32_t x, int32_t y, int32_t z);
    std::optional<SpecialArea> getAreaByName(const std::string& areaName);
    std::vector<SpecialArea> getAreasByType(SpecialArea::Type type);
    
    // Consultas de objetos
    std::vector<WorldObject> getObjectsInRadius(uint32_t mapId, 
                                              int32_t x, int32_t y, int32_t z,
                                              float radius);
    
    std::vector<WorldObject> getObjectsByType(WorldObject::Type type, uint32_t mapId = 0);
    std::optional<WorldObject> getObjectById(uint32_t objectId);
    std::vector<WorldObject> getObjectsByName(const std::string& name);
    
    // Análise de mundo
    WorldAnalysisResult analyzeWorld(const WorldAnalysisOptions& options = WorldAnalysisOptions());
    MapMetrics analyzeMap(uint32_t mapId, const WorldAnalysisOptions& options = WorldAnalysisOptions());
    
    // Funções específicas de análise
    std::vector<std::pair<WorldCoordinate, float>> findResourceHotspots(uint32_t mapId);
    std::vector<std::pair<WorldCoordinate, float>> findCombatHotspots(uint32_t mapId);
    std::vector<WorldCoordinate> findUnreachableAreas(uint32_t mapId);
    std::vector<WorldCoordinate> findNavigationBottlenecks(uint32_t mapId);
    
    // Visualizações e exportações
    bool exportMapHeatmap(uint32_t mapId, const std::string& outputPath,
                         const std::string& heatmapType = "walkable");
    
    bool exportNavigationGraph(uint32_t mapId, const std::string& outputPath);
    bool exportTerrainAnalysis(uint32_t mapId, const std::string& outputPath);
    bool exportWorldAnalysisReport(const std::string& outputPath);
    
    // Eventos e notificações
    core::EventBus::SubscriptionID subscribeToAnalysisProgress(std::function<void(float)> callback);
    core::EventBus::SubscriptionID subscribeToAnalysisCompletion(
        std::function<void(const WorldAnalysisResult&)> callback);
    
    // Funções utilitárias
    std::vector<uint32_t> getLoadedMapIds() const;
    std::optional<MapInfo> getMapInfo(uint32_t mapId) const;
    std::string getLastError() const { return m_lastError; }
    
private:
    struct MapData {
        MapInfo info;
        std::vector<std::vector<float>> heightMap;
        std::vector<std::vector<TerrainAttributes>> attributeMap;
        std::vector<WorldObject> objects;
        std::vector<SpecialArea> specialAreas;
        std::vector<SpawnGroup> spawnGroups;
        std::vector<PatrolRoute> patrolRoutes;
        
        // Dados de navegação pré-computados
        struct NavGraph {
            bool initialized;
            // Dados internos do grafo de navegação
        } navGraph;
        
        MapData() {
            navGraph.initialized = false;
        }
    };
    
    std::unordered_map<uint32_t, MapData> m_mapData;
    std::string m_lastError;
    
    // Estruturas de rápido acesso
    std::unordered_map<std::string, uint32_t> m_areaNameToId;
    std::unordered_map<uint32_t, WorldObject> m_objectsById;
    
    // Dados de análise
    std::unordered_map<uint32_t, MapMetrics> m_cachedMetrics;
    
    // Event bus para notificações
    std::shared_ptr<core::EventBus> m_eventBus;
    
    // Métodos internos
    bool loadWorldData(const std::string& directory);
    bool validateMapData(const MapData& data, std::string& error);
    void buildNavigationGraph(uint32_t mapId);
    void updateAnalysisProgress(float progress);
    
    // Algoritmos de pathfinding
    PathResult findPathAStar(const WorldCoordinate& start, 
                           const WorldCoordinate& end,
                           const PathfindingOptions& options);
    
    PathResult findPathDijkstra(const WorldCoordinate& start, 
                              const WorldCoordinate& end,
                              const PathfindingOptions& options);
    
    PathResult findPathJPS(const WorldCoordinate& start, 
                         const WorldCoordinate& end,
                         const PathfindingOptions& options);
    
    // Funções auxiliares de análise
    float calculateAreaConnectivity(uint32_t mapId, const WorldCoordinate& center, float radius);
    float calculateResourceDensity(uint32_t mapId, const WorldCoordinate& center, float radius);
    float calculateMonsterDensity(uint32_t mapId, const WorldCoordinate& center, float radius);
    
    // Utilitários
    std::vector<std::vector<float>> uniformSampleMap(uint32_t mapId, uint32_t sampleDensity);
};

} // namespace world
} // namespace wydstudio

#endif // WYDSTUDIO_WORLD_ANALYZER_H