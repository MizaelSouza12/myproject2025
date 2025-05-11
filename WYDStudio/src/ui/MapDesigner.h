#ifndef WYDSTUDIO_MAP_DESIGNER_H
#define WYDSTUDIO_MAP_DESIGNER_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>
#include <tuple>
#include <array>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "../binary/ItemListHandler.h"
#include "../core/WYDStudioFacade.h"
#include "../decompilers/AdvancedBinaryAnalyzer.h"
#include "../world/WorldAnalyzer.h"

namespace wydstudio {
namespace ui {

using json = nlohmann::json;

/**
 * @brief Tipos de terreno no mapa
 */
enum class TerrainType {
    PLAIN,
    GRASS,
    SAND,
    SNOW,
    SWAMP,
    ROCKY,
    WASTELAND,
    FOREST,
    WATER_SHALLOW,
    WATER_DEEP,
    LAVA,
    PATH_DIRT,
    PATH_STONE,
    CAVE_FLOOR,
    WALL,
    CLIFF
};

/**
 * @brief Tipos de objetos colocáveis no mapa
 */
enum class MapObjectType {
    TREE,
    ROCK,
    BUILDING,
    DECORATION,
    NPC,
    MOB_SPAWN,
    PORTAL,
    EFFECT,
    CHEST,
    SIGN,
    CUSTOM
};

/**
 * @brief Atributos especiais de uma célula do mapa
 */
enum class CellAttribute {
    NONE = 0,
    WALKABLE = 1,
    PK_ALLOWED = 2,
    SAFE_ZONE = 4,
    WATER = 8,
    LADDER = 16,
    EFFECT_ZONE = 32,
    NO_RECALL = 64,
    GUILD_AREA = 128,
    BATTLE_ARENA = 256,
    SPECIAL_EVENT = 512,
    NO_MOUNT = 1024
};

/**
 * @brief Objeto posicionado no mapa
 */
struct MapObject {
    MapObjectType type;
    int32_t id;
    int32_t x;
    int32_t y;
    int32_t z;
    float rotation;
    float scale;
    std::string name;
    std::map<std::string, std::string> properties;
};

/**
 * @brief Região especial no mapa
 */
struct MapRegion {
    std::string name;
    std::string type;
    int32_t x1, y1;
    int32_t x2, y2;
    std::map<std::string, std::string> properties;
};

/**
 * @brief Ponto de respawn de monstros
 */
struct MobSpawnPoint {
    int32_t mobId;
    std::string mobName;
    int32_t x, y;
    int32_t spawnRadius;
    int32_t maxMobs;
    int32_t respawnTime; // em segundos
    bool boss;
};

/**
 * @brief Portal conectando mapas
 */
struct MapPortal {
    int32_t id;
    std::string name;
    int32_t sourceX, sourceY;
    std::string destMap;
    int32_t destX, destY;
    bool requiresKeyItem;
    int32_t requiredItemId;
    int32_t minLevel;
};

/**
 * @brief Representação simplificada de um mapa para a interface
 */
struct MapUIModel {
    int32_t mapId;
    std::string name;
    int32_t width;
    int32_t height;
    
    // Informações gerais
    struct {
        std::string displayName; // Nome visível para jogadores
        std::string description;
        std::string environment; // Tipo de ambiente (outdoor, dungeon, etc.)
        int32_t baseLevel; // Nível base recomendado
        bool pvpEnabled;
        bool mountsAllowed;
        bool recallAllowed;
        bool isDungeon;
    } info;
    
    // Dados de terreno (simplificados para UI)
    struct {
        std::vector<uint8_t> heightMap; // Valores de 0-255 para altura
        std::vector<uint8_t> attributeMap; // Valores com flags de atributos
        std::vector<uint8_t> terrainTypeMap; // Tipo de terreno em cada célula
    } terrain;
    
    // Objetos e elementos do mapa
    std::vector<MapObject> objects;
    std::vector<MapRegion> regions;
    std::vector<MobSpawnPoint> mobSpawns;
    std::vector<MapPortal> portals;
    
    // Metadados
    struct {
        int32_t createdTimestamp;
        int32_t lastModifiedTimestamp;
        std::string author;
        std::string version;
        std::map<std::string, std::string> extraInfo;
    } metadata;
};

/**
 * @brief Resultado de operações relacionadas a mapas
 */
struct MapOperationResult {
    bool success;
    std::string message;
    std::optional<MapUIModel> map;
};

/**
 * @brief Filtros de pesquisa para mapas
 */
struct MapFilter {
    std::optional<std::string> nameContains;
    std::optional<std::string> environment;
    std::optional<int32_t> minBaseLevel;
    std::optional<int32_t> maxBaseLevel;
    std::optional<bool> isPvP;
    std::optional<bool> isDungeon;
    std::optional<bool> hasBoss;
};

/**
 * @brief Ação de pincel no editor de mapas
 */
struct BrushAction {
    enum class Type {
        TERRAIN,
        HEIGHT,
        ATTRIBUTE,
        OBJECT_PLACE,
        OBJECT_REMOVE
    };
    
    Type type;
    
    // Parâmetros específicos por tipo
    union {
        TerrainType terrain;
        uint8_t height;
        int32_t attribute;
        MapObjectType objectType;
    } param;
    
    int32_t size;       // Tamanho do pincel (raio)
    float intensity;    // Intensidade do efeito (0.0-1.0)
    std::string customData; // Dados adicionais para tipos específicos
};

/**
 * @brief Designer visual de mapas
 */
class MapDesigner {
public:
    /**
     * @brief Construtor
     * @param facade Referência ao facade principal do WYDStudio
     */
    MapDesigner(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~MapDesigner();
    
    /**
     * @brief Carrega lista de mapas do servidor
     * @return true se carregado com sucesso
     */
    bool loadMaps();
    
    /**
     * @brief Obtém mapa pelo ID
     * @param mapId ID do mapa
     * @return Modelo do mapa ou nulo se não encontrado
     */
    std::optional<MapUIModel> getMapById(int32_t mapId);
    
    /**
     * @brief Busca mapas com filtragem
     * @param filter Filtros de busca
     * @return Lista de mapas que correspondem aos filtros
     */
    std::vector<MapUIModel> searchMaps(const MapFilter& filter);
    
    /**
     * @brief Cria um novo mapa com valores padrão
     * @param name Nome do novo mapa
     * @param width Largura do mapa
     * @param height Altura do mapa
     * @return Resultado da operação contendo o novo mapa
     */
    MapOperationResult createNewMap(const std::string& name, int32_t width, int32_t height);
    
    /**
     * @brief Cria um novo mapa a partir de um template
     * @param name Nome do novo mapa
     * @param templateId ID do template a usar
     * @return Resultado da operação contendo o novo mapa
     */
    MapOperationResult createMapFromTemplate(const std::string& name, int32_t templateId);
    
    /**
     * @brief Salva alterações em um mapa
     * @param map Modelo do mapa modificado
     * @return Resultado da operação
     */
    MapOperationResult saveMap(const MapUIModel& map);
    
    /**
     * @brief Duplica um mapa existente
     * @param mapId ID do mapa a ser duplicado
     * @param newName Nome do mapa duplicado
     * @return Resultado da operação contendo o mapa duplicado
     */
    MapOperationResult duplicateMap(int32_t mapId, const std::string& newName);
    
    /**
     * @brief Remove um mapa
     * @param mapId ID do mapa a ser removido
     * @return Resultado da operação
     */
    MapOperationResult removeMap(int32_t mapId);
    
    /**
     * @brief Aplica uma ação de pincel em uma área do mapa
     * @param mapId ID do mapa
     * @param action Ação do pincel
     * @param centerX Coordenada X central
     * @param centerY Coordenada Y central
     * @return Resultado da operação
     */
    MapOperationResult applyBrush(int32_t mapId, const BrushAction& action, int32_t centerX, int32_t centerY);
    
    /**
     * @brief Adiciona um objeto ao mapa
     * @param mapId ID do mapa
     * @param object Objeto a adicionar
     * @return Resultado da operação
     */
    MapOperationResult addObject(int32_t mapId, const MapObject& object);
    
    /**
     * @brief Remove um objeto do mapa
     * @param mapId ID do mapa
     * @param objectIndex Índice do objeto a remover
     * @return Resultado da operação
     */
    MapOperationResult removeObject(int32_t mapId, int32_t objectIndex);
    
    /**
     * @brief Adiciona uma região ao mapa
     * @param mapId ID do mapa
     * @param region Região a adicionar
     * @return Resultado da operação
     */
    MapOperationResult addRegion(int32_t mapId, const MapRegion& region);
    
    /**
     * @brief Remove uma região do mapa
     * @param mapId ID do mapa
     * @param regionIndex Índice da região a remover
     * @return Resultado da operação
     */
    MapOperationResult removeRegion(int32_t mapId, int32_t regionIndex);
    
    /**
     * @brief Adiciona um ponto de respawn de monstros
     * @param mapId ID do mapa
     * @param spawn Ponto de respawn a adicionar
     * @return Resultado da operação
     */
    MapOperationResult addMobSpawn(int32_t mapId, const MobSpawnPoint& spawn);
    
    /**
     * @brief Remove um ponto de respawn de monstros
     * @param mapId ID do mapa
     * @param spawnIndex Índice do ponto de respawn a remover
     * @return Resultado da operação
     */
    MapOperationResult removeMobSpawn(int32_t mapId, int32_t spawnIndex);
    
    /**
     * @brief Adiciona um portal ao mapa
     * @param mapId ID do mapa
     * @param portal Portal a adicionar
     * @return Resultado da operação
     */
    MapOperationResult addPortal(int32_t mapId, const MapPortal& portal);
    
    /**
     * @brief Remove um portal do mapa
     * @param mapId ID do mapa
     * @param portalIndex Índice do portal a remover
     * @return Resultado da operação
     */
    MapOperationResult removePortal(int32_t mapId, int32_t portalIndex);
    
    /**
     * @brief Gera uma miniatura do mapa
     * @param mapId ID do mapa
     * @param width Largura da miniatura
     * @param height Altura da miniatura
     * @return Dados da imagem em formato PNG
     */
    std::vector<uint8_t> generateMapThumbnail(int32_t mapId, int32_t width, int32_t height);
    
    /**
     * @brief Fornece uma renderização 3D do mapa
     * @param mapId ID do mapa
     * @param viewAngle Ângulo de visualização
     * @param zoom Nível de zoom
     * @return Dados da imagem renderizada
     */
    std::vector<uint8_t> render3DMapView(int32_t mapId, float viewAngle, float zoom);
    
    /**
     * @brief Aplica um gerador procedural a uma área do mapa
     * @param mapId ID do mapa
     * @param generatorType Tipo de gerador
     * @param x1 Coordenada X inicial
     * @param y1 Coordenada Y inicial
     * @param x2 Coordenada X final
     * @param y2 Coordenada Y final
     * @param parameters Parâmetros do gerador
     * @return Resultado da operação
     */
    MapOperationResult applyProceduralGenerator(
        int32_t mapId, 
        const std::string& generatorType, 
        int32_t x1, int32_t y1, 
        int32_t x2, int32_t y2,
        const std::map<std::string, std::string>& parameters);
    
    /**
     * @brief Exporta um mapa para um arquivo
     * @param mapId ID do mapa a exportar
     * @param exportPath Caminho do arquivo de exportação
     * @return Resultado da operação
     */
    MapOperationResult exportMap(int32_t mapId, const std::string& exportPath);
    
    /**
     * @brief Importa um mapa de um arquivo
     * @param importPath Caminho do arquivo de importação
     * @return Resultado da operação
     */
    MapOperationResult importMap(const std::string& importPath);
    
    /**
     * @brief Verifica a integridade e jogabilidade de um mapa
     * @param mapId ID do mapa a verificar
     * @return Lista de problemas encontrados (vazia se OK)
     */
    std::vector<std::string> validateMap(int32_t mapId);
    
    /**
     * @brief Obtém a lista de todos os ambientes de mapa disponíveis
     * @return Lista de ambientes
     */
    std::vector<std::string> getMapEnvironments();
    
    /**
     * @brief Obtém a lista de todos os tipos de terreno disponíveis
     * @return Lista de tipos de terreno
     */
    std::vector<std::pair<TerrainType, std::string>> getTerrainTypes();
    
    /**
     * @brief Obtém a lista de todos os tipos de objetos disponíveis
     * @return Lista de tipos de objetos
     */
    std::vector<std::pair<MapObjectType, std::string>> getObjectTypes();
    
    /**
     * @brief Obtém a lista de todos os modelos de objetos disponíveis
     * @param type Tipo de objeto
     * @return Lista de modelos
     */
    std::vector<std::pair<int32_t, std::string>> getObjectModels(MapObjectType type);
    
    /**
     * @brief Recarrega todos os mapas do disco
     * @return true se recarregado com sucesso
     */
    bool reloadMaps();
    
    /**
     * @brief Aplica mudanças ao servidor sem reiniciar
     * @return true se aplicado com sucesso
     */
    bool applyChangesToLiveServer();
    
    /**
     * @brief Registra callback para notificações de alterações em mapas
     * @param callback Função de callback
     * @return ID de registro (para cancelamento posterior)
     */
    uint32_t registerChangeCallback(std::function<void(const MapUIModel&)> callback);
    
    /**
     * @brief Cancela registro de callback
     * @param callbackId ID do callback a cancelar
     */
    void unregisterChangeCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém o histórico de operações no mapa (para undo/redo)
     * @param mapId ID do mapa
     * @return Lista de operações, da mais recente para a mais antiga
     */
    std::vector<std::string> getMapEditHistory(int32_t mapId);
    
    /**
     * @brief Desfaz a última operação no mapa
     * @param mapId ID do mapa
     * @return Resultado da operação
     */
    MapOperationResult undoLastMapOperation(int32_t mapId);
    
    /**
     * @brief Refaz a última operação desfeita
     * @param mapId ID do mapa
     * @return Resultado da operação
     */
    MapOperationResult redoMapOperation(int32_t mapId);

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade principal
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Utilitários de conversão
    MapUIModel convertInternalToUIModel(const world::MapData& internalData);
    world::MapData convertUIModelToInternal(const MapUIModel& uiModel);
    
    // Cache de mapas
    std::map<int32_t, MapUIModel> m_mapCache;
    
    // Histórico de operações por mapa
    std::map<int32_t, std::vector<std::string>> m_mapHistory;
    std::map<int32_t, int32_t> m_historyPosition; // Posição atual para redo
    
    // Validação de mapas
    bool validateMapIntegrity(const MapUIModel& map);
    
    // Gerenciamento de arquivos
    std::string getMapFilePath(int32_t mapId);
    std::string getMapAttributeFilePath(int32_t mapId);
    
    // Utilitários de serialização
    json serializeMapToJson(const MapUIModel& map);
    MapUIModel deserializeMapFromJson(const json& json);
};

}} // namespace wydstudio::ui

#endif // WYDSTUDIO_MAP_DESIGNER_H