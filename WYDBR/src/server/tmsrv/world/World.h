/**
 * World.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/World.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WORLD_H
#define WORLD_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <queue>
#include <bitset>
#include <set>
#include <random>

#include "../../../core/WYDTypes.h"
#include "WorldTypes.h"
#include "Position.h"

namespace wydbr {


/**
 * @file World.h
 * @brief Sistema avançado de gerenciamento de mundo
 * 
 * Este arquivo implementa o sistema de mundo do WYD, responsável por
 * gerenciar mapas, zonas, entidades, colisões, eventos de mundo e
 * muito mais. O sistema é projetado para ser altamente eficiente,
 * com otimizações para divisão espacial, rastreamento de entidades e
 * sincronização entre zonas.
 */

namespace wyd {
namespace server {

// Forward declarations
class Zone;
class Player;
class NPC;
class Item;
class WorldEvent;

/**
 * @brief Tipos de evento de mundo
 */
enum class WorldEventType : BYTE {
    NONE = 0,            // Nenhum
    WEATHER_CHANGE = 1,  // Mudança de clima
    TIME_CHANGE = 2,     // Mudança de hora
    SPAWN = 3,           // Spawn de entidade
    DESPAWN = 4,         // Despawn de entidade
    PORTAL_OPEN = 5,     // Portal aberto
    PORTAL_CLOSE = 6,    // Portal fechado
    AREA_EFFECT = 7,     // Efeito de área
    BOSS_SPAWN = 8,      // Spawn de boss
    CASTLE_SIEGE = 9,    // Cerco ao castelo
    GUILD_WAR = 10,      // Guerra de guild
    DUNGEON_RESET = 11,  // Reset de masmorra
    SERVER_MESSAGE = 12, // Mensagem do servidor
    TREASURE = 13,       // Tesouro
    INVATION = 14,       // Invasão
    PLAGUE = 15,         // Praga
    METEOR_SHOWER = 16,  // Chuva de meteoros
    EARTHQUAKE = 17,     // Terremoto
    CUSTOM = 255         // Customizado
};

/**
 * @brief Interface para evento de mundo
 */
/**
 * Classe WYDIWorldEvent
 * 
 * Esta classe implementa a funcionalidade WYDIWorldEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class IWorldEvent {
public:
    /**
     * @brief Destrutor virtual
     */
    virtual ~IWorldEvent() {}
    
    /**
     * @brief Obtém o ID do evento
     * @return ID do evento
     */
    virtual DWORD GetID() const = 0;
    
    /**
     * @brief Obtém o tipo do evento
     * @return Tipo do evento
     */
    virtual WorldEventType GetType() const = 0;
    
    /**
     * @brief Inicializa o evento
     * @return true se o evento foi inicializado, false caso contrário
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Atualiza o evento
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    virtual void Update(DWORD deltaTime) = 0;
    
    /**
     * @brief Finaliza o evento
     */
    virtual void Shutdown() = 0;
    
    /**
     * @brief Verifica se o evento está ativo
     * @return true se o evento está ativo, false caso contrário
     */
    virtual bool IsActive() const = 0;
    
    /**
     * @brief Ativa o evento
     * @param active Se o evento deve ser ativado (true) ou desativado (false)
     */
    virtual void SetActive(bool active) = 0;
};

/**
 * @brief Tipo de entidade no mundo
 */
enum class WorldEntityType : BYTE {
    NONE = 0,       // Nenhum
    PLAYER = 1,     // Jogador
    NPC = 2,        // NPC
    ITEM = 3,       // Item
    PROJECTILE = 4, // Projétil
    EFFECT = 5,     // Efeito
    TRIGGER = 6,    // Gatilho
    PORTAL = 7,     // Portal
    CUSTOM = 255    // Customizado
};

/**
 * @brief Interface para entidade no mundo
 */
/**
 * Classe WYDIWorldEntity
 * 
 * Esta classe implementa a funcionalidade WYDIWorldEntity conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class IWorldEntity {
public:
    /**
     * @brief Destrutor virtual
     */
    virtual ~IWorldEntity() {}
    
    /**
     * @brief Obtém o ID da entidade
     * @return ID da entidade
     */
    virtual DWORD GetID() const = 0;
    
    /**
     * @brief Obtém o tipo da entidade
     * @return Tipo da entidade
     */
    virtual WorldEntityType GetType() const = 0;
    
    /**
     * @brief Obtém a posição da entidade
     * @return Posição da entidade
     */
    virtual Position GetPosition() const = 0;
    
    /**
     * @brief Define a posição da entidade
     * @param position Nova posição
     * @return true se a posição foi alterada, false caso contrário
     */
    virtual bool SetPosition(const Position& position) = 0;
    
    /**
     * @brief Obtém o nome da entidade
     * @return Nome da entidade
     */
    virtual const char* GetName() const = 0;
    
    /**
     * @brief Obtém o modelo da entidade
     * @return ID do modelo da entidade
     */
    virtual WORD GetModelID() const = 0;
    
    /**
     * @brief Obtém o raio de colisão da entidade
     * @return Raio de colisão
     */
    virtual float GetCollisionRadius() const = 0;
    
    /**
     * @brief Verifica se a entidade está ativa
     * @return true se a entidade está ativa, false caso contrário
     */
    virtual bool IsActive() const = 0;
    
    /**
     * @brief Verifica se a entidade é visível
     * @return true se a entidade é visível, false caso contrário
     */
    virtual bool IsVisible() const = 0;
};

/**
 * @brief Gerenciador avançado de mundo
 * 
 * Esta classe gerencia todo o sistema de mundo do WYD, incluindo
 * mapas, zonas, entidades, colisões, eventos, etc.
 */
/**
 * Classe WYDWorld
 * 
 * Esta classe implementa a funcionalidade WYDWorld conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class World {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static World& getInstance();
    
    /**
     * @brief Inicializa o sistema de mundo
     * @param config Configuração do mundo
     * @return true se o sistema foi inicializado, false caso contrário
     */
    bool Initialize(const WorldConfig& config);
    
    /**
     * @brief Finaliza o sistema de mundo
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o sistema de mundo
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    //=================================================================
    // Gerenciamento de mapas
    //=================================================================
    
    /**
     * @brief Carrega um mapa
     * @param mapId ID do mapa
     * @param filename Nome do arquivo
     * @return true se o mapa foi carregado, false caso contrário
     */
    bool LoadMap(WORD mapId, const std::string& filename);
    
    /**
     * @brief Descarrega um mapa
     * @param mapId ID do mapa
     * @return true se o mapa foi descarregado, false caso contrário
     */
    bool UnloadMap(WORD mapId);
    
    /**
     * @brief Obtém informações sobre um mapa
     * @param mapId ID do mapa
     * @return Ponteiro para as informações do mapa (nullptr se não existir)
     */
    const MapInfo* GetMapInfo(WORD mapId) const;
    
    /**
     * @brief Cria uma instância de mapa
     * @param baseMapId ID do mapa base
     * @param ownerId ID do proprietário (0 para servidor)
     * @param instanceName Nome da instância
     * @return ID do mapa instanciado (0 se falhar)
     */
    WORD CreateMapInstance(WORD baseMapId, DWORD ownerId = 0, const std::string& instanceName = "");
    
    /**
     * @brief Destrói uma instância de mapa
     * @param instanceId ID da instância
     * @return true se a instância foi destruída, false caso contrário
     */
    bool DestroyMapInstance(WORD instanceId);
    
    /**
     * @brief Verifica se um mapa é uma instância
     * @param mapId ID do mapa
     * @return true se o mapa é uma instância, false caso contrário
     */
    bool IsMapInstance(WORD mapId) const;
    
    /**
     * @brief Obtém o ID do mapa base de uma instância
     * @param instanceId ID da instância
     * @return ID do mapa base (0 se não for uma instância)
     */
    WORD GetBaseMapID(WORD instanceId) const;
    
    /**
     * @brief Obtém o proprietário de uma instância
     * @param instanceId ID da instância
     * @return ID do proprietário (0 se não for uma instância ou for do servidor)
     */
    DWORD GetInstanceOwner(WORD instanceId) const;
    
    /**
     * @brief Define o clima de um mapa
     * @param mapId ID do mapa
     * @param weather Novo clima
     * @param duration Duração em milissegundos (0 para permanente)
     * @return true se o clima foi alterado, false caso contrário
     */
    bool SetMapWeather(WORD mapId, Weather weather, DWORD duration = 0);
    
    /**
     * @brief Define a hora do dia de um mapa
     * @param mapId ID do mapa
     * @param timeOfDay Nova hora do dia (0-23)
     * @return true se a hora foi alterada, false caso contrário
     */
    bool SetMapTimeOfDay(WORD mapId, BYTE timeOfDay);
    
    /**
     * @brief Ativa ou desativa uma região
     * @param mapId ID do mapa
     * @param regionId ID da região
     * @param active Se a região deve ser ativada (true) ou desativada (false)
     * @return true se a região foi alterada, false caso contrário
     */
    bool SetRegionActive(WORD mapId, WORD regionId, bool active);
    
    /**
     * @brief Ativa ou desativa um portal
     * @param mapId ID do mapa
     * @param portalId ID do portal
     * @param active Se o portal deve ser ativado (true) ou desativado (false)
     * @return true se o portal foi alterado, false caso contrário
     */
    bool SetPortalActive(WORD mapId, WORD portalId, bool active);
    
    /**
     * @brief Verifica se uma posição é passável
     * @param position Posição
     * @return true se a posição é passável, false caso contrário
     */
    bool IsPositionWalkable(const Position& position) const;
    
    /**
     * @brief Obtém a altura do terreno em uma posição
     * @param mapId ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return Altura do terreno (0 se inválido)
     */
    WORD GetTerrainHeight(WORD mapId, WORD x, WORD y) const;
    
    /**
     * @brief Obtém o tipo de terreno em uma posição
     * @param mapId ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return Tipo de terreno
     */
    TerrainType GetTerrainType(WORD mapId, WORD x, WORD y) const;
    
    /**
     * @brief Verifica se uma posição está dentro de uma região
     * @param position Posição
     * @param regionId ID da região (0 para verificar todas)
     * @return ID da região em que a posição está (0 se nenhuma)
     */
    WORD IsPositionInRegion(const Position& position, WORD regionId = 0) const;
    
    /**
     * @brief Verifica se uma área contém entidades
     * @param area Área
     * @param entityType Tipo de entidade (NONE para todos)
     * @return true se a área contém entidades, false caso contrário
     */
    bool DoesAreaContainEntities(const Area& area, WorldEntityType entityType = WorldEntityType::NONE) const;
    
    //=================================================================
    // Gerenciamento de zonas
    //=================================================================
    
    /**
     * @brief Obtém uma zona pelo ID
     * @param mapId ID do mapa
     * @param zoneX Coordenada X da zona
     * @param zoneY Coordenada Y da zona
     * @return Ponteiro para a zona (nullptr se não existir)
     */
    Zone* GetZone(WORD mapId, WORD zoneX, WORD zoneY);
    
    /**
     * @brief Obtém a zona que contém uma posição
     * @param position Posição
     * @return Ponteiro para a zona (nullptr se não existir)
     */
    Zone* GetZoneAt(const Position& position);
    
    /**
     * @brief Obtém todas as zonas em uma área
     * @param area Área
     * @return Lista de ponteiros para zonas
     */
    std::vector<Zone*> GetZonesInArea(const Area& area);
    
    /**
     * @brief Obtém todas as zonas em um raio
     * @param center Posição central
     * @param radius Raio
     * @return Lista de ponteiros para zonas
     */
    std::vector<Zone*> GetZonesInRadius(const Position& center, WORD radius);
    
    /**
     * @brief Ativa todas as zonas em uma área
     * @param area Área
     * @return Número de zonas ativadas
     */
    INT ActivateZonesInArea(const Area& area);
    
    /**
     * @brief Desativa zonas não utilizadas
     * @param keepActiveRadius Raio para manter zonas ativas ao redor de jogadores
     * @return Número de zonas desativadas
     */
    INT DeactivateUnusedZones(WORD keepActiveRadius = 2);
    
    //=================================================================
    // Gerenciamento de entidades
    //=================================================================
    
    /**
     * @brief Registra uma entidade no mundo
     * @param entity Ponteiro para a entidade
     * @return true se a entidade foi registrada, false caso contrário
     */
    bool RegisterEntity(IWorldEntity* entity);
    
    /**
     * @brief Remove uma entidade do mundo
     * @param entityId ID da entidade
     * @return true se a entidade foi removida, false caso contrário
     */
    bool UnregisterEntity(DWORD entityId);
    
    /**
     * @brief Busca uma entidade pelo ID
     * @param entityId ID da entidade
     * @return Ponteiro para a entidade (nullptr se não existir)
     */
    IWorldEntity* GetEntity(DWORD entityId);
    
    /**
     * @brief Busca um jogador pelo ID
     * @param playerId ID do jogador
     * @return Ponteiro para o jogador (nullptr se não existir)
     */
    Player* GetPlayer(DWORD playerId);
    
    /**
     * @brief Busca um NPC pelo ID
     * @param npcId ID do NPC
     * @return Ponteiro para o NPC (nullptr se não existir)
     */
    NPC* GetNPC(DWORD npcId);
    
    /**
     * @brief Busca um item pelo ID
     * @param itemId ID do item
     * @return Ponteiro para o item (nullptr se não existir)
     */
    Item* GetItem(DWORD itemId);
    
    /**
     * @brief Teleporta uma entidade para uma posição
     * @param entityId ID da entidade
     * @param destination Posição de destino
     * @return true se a entidade foi teleportada, false caso contrário
     */
    bool TeleportEntity(DWORD entityId, const Position& destination);
    
    /**
     * @brief Move uma entidade para uma posição
     * @param entityId ID da entidade
     * @param destination Posição de destino
     * @param speed Velocidade (0 para instantâneo)
     * @return true se a entidade foi movida, false caso contrário
     */
    bool MoveEntity(DWORD entityId, const Position& destination, float speed = 0.0f);
    
    /**
     * @brief Verifica a visibilidade entre duas entidades
     * @param entityId1 ID da primeira entidade
     * @param entityId2 ID da segunda entidade
     * @return true se as entidades são visíveis entre si, false caso contrário
     */
    bool AreEntitiesVisible(DWORD entityId1, DWORD entityId2) const;
    
    /**
     * @brief Obtém todas as entidades em uma área
     * @param area Área
     * @param entityType Tipo de entidade (NONE para todos)
     * @return Lista de IDs de entidades
     */
    std::vector<DWORD> GetEntitiesInArea(const Area& area, WorldEntityType entityType = WorldEntityType::NONE) const;
    
    /**
     * @brief Obtém todas as entidades em um raio
     * @param center Posição central
     * @param radius Raio
     * @param entityType Tipo de entidade (NONE para todos)
     * @return Lista de IDs de entidades
     */
    std::vector<DWORD> GetEntitiesInRadius(const Position& center, float radius, WorldEntityType entityType = WorldEntityType::NONE) const;
    
    /**
     * @brief Obtém a entidade mais próxima de uma posição
     * @param position Posição
     * @param radius Raio máximo de busca
     * @param entityType Tipo de entidade (NONE para todos)
     * @param excludeId ID de entidade a excluir da busca
     * @return ID da entidade mais próxima (0 se nenhuma)
     */
    DWORD GetNearestEntity(const Position& position, float radius, WorldEntityType entityType = WorldEntityType::NONE, DWORD excludeId = 0) const;
    
    /**
     * @brief Envia um pacote para todas as entidades em uma área
     * @param area Área
     * @param packetData Dados do pacote
     * @param packetSize Tamanho do pacote
     * @param excludeId ID de entidade a excluir do envio
     * @return Número de entidades que receberam o pacote
     */
    INT BroadcastToArea(const Area& area, const void* packetData, size_t packetSize, DWORD excludeId = 0);
    
    /**
     * @brief Envia um pacote para todas as entidades em um raio
     * @param center Posição central
     * @param radius Raio
     * @param packetData Dados do pacote
     * @param packetSize Tamanho do pacote
     * @param excludeId ID de entidade a excluir do envio
     * @return Número de entidades que receberam o pacote
     */
    INT BroadcastToRadius(const Position& center, float radius, const void* packetData, size_t packetSize, DWORD excludeId = 0);
    
    //=================================================================
    // Gerenciamento de eventos
    //=================================================================
    
    /**
     * @brief Registra um evento de mundo
     * @param event Ponteiro para o evento
     * @return true se o evento foi registrado, false caso contrário
     */
    bool RegisterEvent(std::shared_ptr<IWorldEvent> event);
    
    /**
     * @brief Remove um evento de mundo
     * @param eventId ID do evento
     * @return true se o evento foi removido, false caso contrário
     */
    bool UnregisterEvent(DWORD eventId);
    
    /**
     * @brief Busca um evento pelo ID
     * @param eventId ID do evento
     * @return Ponteiro para o evento (nullptr se não existir)
     */
    std::shared_ptr<IWorldEvent> GetEvent(DWORD eventId);
    
    /**
     * @brief Ativa ou desativa um evento
     * @param eventId ID do evento
     * @param active Se o evento deve ser ativado (true) ou desativado (false)
     * @return true se o evento foi alterado, false caso contrário
     */
    bool SetEventActive(DWORD eventId, bool active);
    
    /**
     * @brief Agenda um evento para ocorrer após um intervalo
     * @param delay Intervalo em milissegundos
     * @param event Ponteiro para o evento
     * @return ID do evento agendado (0 se falhar)
     */
    DWORD ScheduleEvent(DWORD delay, std::shared_ptr<IWorldEvent> event);
    
    /**
     * @brief Cancela um evento agendado
     * @param scheduledId ID do evento agendado
     * @return true se o evento foi cancelado, false caso contrário
     */
    bool CancelScheduledEvent(DWORD scheduledId);
    
    //=================================================================
    // Pathfinding e navegação
    //=================================================================
    
    /**
     * @brief Calcula um caminho entre duas posições
     * @param start Posição inicial
     * @param end Posição final
     * @param maxDistance Distância máxima (0 para ilimitado)
     * @return Lista de posições no caminho (vazia se não for possível)
     */
    std::vector<Position> CalculatePath(const Position& start, const Position& end, float maxDistance = 0.0f);
    
    /**
     * @brief Calcula a distância do caminho entre duas posições
     * @param start Posição inicial
     * @param end Posição final
     * @return Distância do caminho (-1 se não for possível)
     */
    float CalculatePathDistance(const Position& start, const Position& end);
    
    /**
     * @brief Verifica a linha de visão entre duas posições
     * @param start Posição inicial
     * @param end Posição final
     * @return true se há linha de visão, false caso contrário
     */
    bool HasLineOfSight(const Position& start, const Position& end);
    
    /**
     * @brief Encontra a posição mais próxima passável
     * @param center Posição central
     * @param maxRadius Raio máximo de busca
     * @return Posição passável mais próxima (igual à central se já for passável)
     */
    Position FindNearestWalkablePosition(const Position& center, float maxRadius = 10.0f);
    
    /**
     * @brief Traça um raio a partir de uma posição
     * @param start Posição inicial
     * @param direction Direção
     * @param maxDistance Distância máxima
     * @param stopOnCollision Se deve parar ao colidir
     * @param hitEntityId Retorna o ID da entidade atingida (por referência)
     * @return Posição final do raio
     */
    Position RayCast(const Position& start, BYTE direction, float maxDistance, bool stopOnCollision, DWORD& hitEntityId);
    
    //=================================================================
    // Detecção de colisão
    //=================================================================
    
    /**
     * @brief Verifica a colisão em uma linha
     * @param start Posição inicial
     * @param end Posição final
     * @param collisionRadius Raio de colisão
     * @param entityType Tipo de entidade para colisão (NONE para apenas terreno)
     * @param excludeId ID de entidade a excluir da colisão
     * @return true se há colisão, false caso contrário
     */
    bool CheckLineCollision(const Position& start, const Position& end, float collisionRadius = 0.0f, WorldEntityType entityType = WorldEntityType::NONE, DWORD excludeId = 0);
    
    /**
     * @brief Verifica a colisão em um movimento
     * @param entity Entidade que está se movendo
     * @param destination Posição de destino
     * @param entityType Tipo de entidade para colisão (NONE para apenas terreno)
     * @param slidingAllowed Se permite deslizar ao longo das paredes
     * @param slidingResult Resultado do deslizamento (por referência)
     * @return true se há colisão, false caso contrário
     */
    bool CheckMovementCollision(IWorldEntity* entity, const Position& destination, WorldEntityType entityType = WorldEntityType::NONE, bool slidingAllowed = false, Position* slidingResult = nullptr);
    
    /**
     * @brief Verifica a colisão de uma área
     * @param area Área
     * @param entityType Tipo de entidade para colisão (NONE para apenas terreno)
     * @param excludeId ID de entidade a excluir da colisão
     * @return true se há colisão, false caso contrário
     */
    bool CheckAreaCollision(const Area& area, WorldEntityType entityType = WorldEntityType::NONE, DWORD excludeId = 0);
    
    /**
     * @brief Verifica a colisão de uma área circular
     * @param center Posição central
     * @param radius Raio
     * @param entityType Tipo de entidade para colisão (NONE para apenas terreno)
     * @param excludeId ID de entidade a excluir da colisão
     * @return true se há colisão, false caso contrário
     */
    bool CheckCircleCollision(const Position& center, float radius, WorldEntityType entityType = WorldEntityType::NONE, DWORD excludeId = 0);
    
    //=================================================================
    // Utilitários
    //=================================================================
    
    /**
     * @brief Obtém o número total de entidades
     * @return Número de entidades
     */
    size_t GetEntityCount() const { return entities_.size(); }
    
    /**
     * @brief Obtém o número de jogadores online
     * @return Número de jogadores
     */
    size_t GetPlayerCount() const { return playerEntities_.size(); }
    
    /**
     * @brief Obtém o número de NPCs
     * @return Número de NPCs
     */
    size_t GetNPCCount() const { return npcEntities_.size(); }
    
    /**
     * @brief Obtém o número de itens
     * @return Número de itens
     */
    size_t GetItemCount() const { return itemEntities_.size(); }
    
    /**
     * @brief Obtém o número de mapas carregados
     * @return Número de mapas
     */
    size_t GetMapCount() const { return maps_.size(); }
    
    /**
     * @brief Obtém o número de zonas ativas
     * @return Número de zonas
     */
    size_t GetActiveZoneCount() const;
    
    /**
     * @brief Obtém a configuração do sistema de mundo
     * @return Configuração do mundo
     */
    const WorldConfig& GetConfig() const { return config_; }
    
    /**
     * @brief Executa uma função para cada entidade
     * @param callback Função de callback (recebe ponteiro para entidade)
     */
    void ForEachEntity(std::function<void(IWorldEntity*)> callback);
    
    /**
     * @brief Executa uma função para cada jogador
     * @param callback Função de callback (recebe ponteiro para jogador)
     */
    void ForEachPlayer(std::function<void(Player*)> callback);
    
    /**
     * @brief Executa uma função para cada NPC
     * @param callback Função de callback (recebe ponteiro para NPC)
     */
    void ForEachNPC(std::function<void(NPC*)> callback);
    
    /**
     * @brief Executa uma função para cada item
     * @param callback Função de callback (recebe ponteiro para item)
     */
    void ForEachItem(std::function<void(Item*)> callback);
    
    /**
     * @brief Executa uma função para cada mapa
     * @param callback Função de callback (recebe MapInfo)
     */
    void ForEachMap(std::function<void(const MapInfo&)> callback);
    
    /**
     * @brief Executa uma função para cada zona
     * @param callback Função de callback (recebe ponteiro para zona)
     */
    void ForEachZone(std::function<void(Zone*)> callback);
    
    /**
     * @brief Obtém o portal em uma posição
     * @param position Posição
     * @param interactRadius Raio de interação
     * @return Ponteiro para o portal (nullptr se não existir)
     */
    const Portal* GetPortalAt(const Position& position, float interactRadius = 2.0f) const;
    
    /**
     * @brief Obtém o tamanho da grade de zonas
     * @return Tamanho da grade em tiles
     */
    WORD GetZoneGridSize() const { return config_.gridSize; }
    
    /**
     * @brief Obtém o timestamp atual do mundo
     * @return Timestamp em milissegundos
     */
    DWORD GetTickCount() const;
    
    /**
     * @brief Converte coordenadas de mundo para coordenadas de zona
     * @param mapId ID do mapa
     * @param x Coordenada X do mundo
     * @param y Coordenada Y do mundo
     * @param zoneX Coordenada X da zona (por referência)
     * @param zoneY Coordenada Y da zona (por referência)
     */
    void WorldToZoneCoordinates(WORD mapId, WORD x, WORD y, WORD& zoneX, WORD& zoneY) const;
    
    /**
     * @brief Converte coordenadas de zona para coordenadas de mundo
     * @param mapId ID do mapa
     * @param zoneX Coordenada X da zona
     * @param zoneY Coordenada Y da zona
     * @param x Coordenada X do mundo (por referência)
     * @param y Coordenada Y do mundo (por referência)
     */
    void ZoneToWorldCoordinates(WORD mapId, WORD zoneX, WORD zoneY, WORD& x, WORD& y) const;
    
    /**
     * @brief Gera uma posição aleatória em um mapa
     * @param mapId ID do mapa
     * @param region Região para gerar a posição (0 para todo o mapa)
     * @return Posição aleatória
     */
    Position GenerateRandomPosition(WORD mapId, WORD region = 0);
    
    /**
     * @brief Gera uma posição aleatória em um raio ao redor de um ponto
     * @param center Posição central
     * @param minRadius Raio mínimo
     * @param maxRadius Raio máximo
     * @param mustBeWalkable Se a posição deve ser passável
     * @return Posição aleatória
     */
    Position GenerateRandomPositionInRadius(const Position& center, float minRadius, float maxRadius, bool mustBeWalkable = true);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    World();
    
    /**
     * @brief Destrutor privado (singleton)
     */
    ~World();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    World(const World&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    World& operator=(const World&) = delete;
    
    /**
     * @brief Cria zonas para um mapa
     * @param mapId ID do mapa
     * @return true se as zonas foram criadas, false caso contrário
     */
    bool CreateZonesForMap(WORD mapId);
    
    /**
     * @brief Atualiza as entidades
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void UpdateEntities(DWORD deltaTime);
    
    /**
     * @brief Atualiza os eventos
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void UpdateEvents(DWORD deltaTime);
    
    /**
     * @brief Processa eventos agendados
     */
    void ProcessScheduledEvents();
    
    /**
     * @brief Atualiza o clima global
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void UpdateWeather(DWORD deltaTime);
    
    /**
     * @brief Atualiza o tempo global
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void UpdateTime(DWORD deltaTime);
    
    /**
     * @brief Calcula o caminho usando o algoritmo A*
     * @param start Posição inicial
     * @param end Posição final
     * @param maxNodes Número máximo de nós a verificar
     * @return Lista de posições no caminho
     */
    std::vector<Position> CalculatePathAStar(const Position& start, const Position& end, int maxNodes = 1000);
    
    /**
     * @brief Encontra o ID da zona
     * @param mapId ID do mapa
     * @param zoneX Coordenada X da zona
     * @param zoneY Coordenada Y da zona
     * @return ID da zona
     */
    DWORD GetZoneID(WORD mapId, WORD zoneX, WORD zoneY) const;
    
    /**
     * @brief Verifica se uma zona existe
     * @param mapId ID do mapa
     * @param zoneX Coordenada X da zona
     * @param zoneY Coordenada Y da zona
     * @return true se a zona existe, false caso contrário
     */
    bool ZoneExists(WORD mapId, WORD zoneX, WORD zoneY) const;
    
    /**
     * @brief Cria uma zona
     * @param mapId ID do mapa
     * @param zoneX Coordenada X da zona
     * @param zoneY Coordenada Y da zona
     * @return Ponteiro para a zona (nullptr se falhar)
     */
    Zone* CreateZone(WORD mapId, WORD zoneX, WORD zoneY);
    
    /**
     * @brief Destrói uma zona
     * @param mapId ID do mapa
     * @param zoneX Coordenada X da zona
     * @param zoneY Coordenada Y da zona
     * @return true se a zona foi destruída, false caso contrário
     */
    bool DestroyZone(WORD mapId, WORD zoneX, WORD zoneY);
    
    /**
     * @brief Notifica zonas de uma mudança de entidade
     * @param entity Entidade
     * @param oldPosition Posição antiga
     * @param newPosition Nova posição
     */
    void NotifyZonesOfEntityChange(IWorldEntity* entity, const Position& oldPosition, const Position& newPosition);
    
    //=================================================================
    // Membros privados
    //=================================================================
    
    WorldConfig config_;                                                // Configuração do mundo
    
    std::unordered_map<WORD, MapInfo> maps_;                            // Mapas carregados
    std::unordered_map<WORD, WORD> instanceToBaseMap_;                  // Mapeamento de instância para mapa base
    std::unordered_map<WORD, DWORD> instanceOwners_;                    // Proprietários de instâncias
    
    std::unordered_map<DWORD, std::unique_ptr<Zone>> zones_;            // Zonas
    std::unordered_map<DWORD, IWorldEntity*> entities_;                 // Todas as entidades
    std::unordered_map<DWORD, Player*> playerEntities_;                 // Jogadores
    std::unordered_map<DWORD, NPC*> npcEntities_;                       // NPCs
    std::unordered_map<DWORD, Item*> itemEntities_;                     // Itens
    
    std::unordered_map<DWORD, std::shared_ptr<IWorldEvent>> events_;    // Eventos
    std::multimap<DWORD, std::pair<DWORD, std::shared_ptr<IWorldEvent>>> scheduledEvents_; // Eventos agendados
    
    WORD nextMapId_;                                                    // Próximo ID de mapa disponível
    DWORD nextEventId_;                                                 // Próximo ID de evento disponível
    
    DWORD lastUpdateTime_;                                              // Timestamp da última atualização
    DWORD worldTime_;                                                   // Tempo global do mundo em minutos
    
    bool initialized_;                                                  // Se o sistema foi inicializado
    
    std::mt19937 rng_;                                                  // Gerador de números aleatórios
    
    mutable std::recursive_mutex worldMutex_;                           // Mutex para sincronização
    
    // Constantes
    static const int MAX_LINE_OF_SIGHT_DISTANCE = 50;                   // Distância máxima de linha de visão
    static const int MAX_PATHFINDING_NODES = 1000;                      // Número máximo de nós para pathfinding
};

} // namespace server
} // namespace wyd

#endif // WORLD_H

} // namespace wydbr
