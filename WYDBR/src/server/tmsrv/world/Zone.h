/**
 * Zone.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/Zone.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef ZONE_H
#define ZONE_H

#include <vector>
#include <unordered_map>
#include <set>
#include <mutex>
#include <memory>
#include <functional>
#include <bitset>
#include <algorithm>

#include "../../../core/WYDTypes.h"
#include "WorldTypes.h"
#include "Position.h"

namespace wydbr {


/**
 * @file Zone.h
 * @brief Sistema avançado de zonas para divisão espacial do mundo
 * 
 * Este arquivo implementa o sistema de zonas do WYD, responsável pela
 * divisão espacial do mundo em áreas gerenciáveis. Cada zona contém
 * entidades, colisões, eventos e muito mais, permitindo carregamento
 * dinâmico de áreas conforme necessário para performance otimizada.
 */

namespace wyd {
namespace server {

// Forward declarations
class World;
class Player;
class NPC;
class Item;
class IWorldEntity;
class IWorldEvent;

/**
 * @brief Classe que representa uma zona no mundo
 * 
 * Uma zona é uma área do mundo que contém entidades, colisões, eventos e 
 * outras informações. O sistema de zonas é usado para divisão espacial
 * do mundo, permitindo carregar/descarregar áreas dinamicamente e 
 * otimizar a detecção de colisões e processamento de eventos.
 */
/**
 * Classe WYDZone
 * 
 * Esta classe implementa a funcionalidade WYDZone conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Zone {
public:
    /**
     * @brief Construtor
     * @param mapId ID do mapa
     * @param zoneX Coordenada X da zona
     * @param zoneY Coordenada Y da zona
     */
    Zone(WORD mapId, WORD zoneX, WORD zoneY);
    
    /**
     * @brief Destrutor
     */
    ~Zone();
    
    /**
     * @brief Inicializa a zona
     * @return true se a zona foi inicializada, false caso contrário
     */
    bool Initialize();
    
    /**
     * @brief Finaliza a zona
     */
    void Shutdown();
    
    /**
     * @brief Atualiza a zona
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    //=================================================================
    // Getters/Setters básicos
    //=================================================================
    
    /**
     * @brief Obtém o ID do mapa
     * @return ID do mapa
     */
    WORD GetMapId() const { return mapId_; }
    
    /**
     * @brief Obtém a coordenada X da zona
     * @return Coordenada X da zona
     */
    WORD GetZoneX() const { return zoneX_; }
    
    /**
     * @brief Obtém a coordenada Y da zona
     * @return Coordenada Y da zona
     */
    WORD GetZoneY() const { return zoneY_; }
    
    /**
     * @brief Obtém o ID da zona
     * @return ID da zona
     */
    DWORD GetZoneId() const { return zoneId_; }
    
    /**
     * @brief Obtém a área da zona
     * @return Área da zona
     */
    const Area& GetArea() const { return area_; }
    
    /**
     * @brief Verifica se a zona está ativa
     * @return true se a zona está ativa, false caso contrário
     */
    bool IsActive() const { return active_; }
    
    /**
     * @brief Define se a zona está ativa
     * @param active Se a zona deve ser ativada (true) ou desativada (false)
     */
    void SetActive(bool active);
    
    /**
     * @brief Obtém o número de jogadores na zona
     * @return Número de jogadores
     */
    size_t GetPlayerCount() const { return players_.size(); }
    
    /**
     * @brief Obtém o número de NPCs na zona
     * @return Número de NPCs
     */
    size_t GetNPCCount() const { return npcs_.size(); }
    
    /**
     * @brief Obtém o número de itens na zona
     * @return Número de itens
     */
    size_t GetItemCount() const { return items_.size(); }
    
    /**
     * @brief Obtém o número total de entidades na zona
     * @return Número total de entidades
     */
    size_t GetEntityCount() const { return players_.size() + npcs_.size() + items_.size(); }
    
    /**
     * @brief Obtém o timestamp da última atualização
     * @return Timestamp da última atualização
     */
    DWORD GetLastUpdateTime() const { return lastUpdateTime_; }
    
    //=================================================================
    // Gerenciamento de entidades
    //=================================================================
    
    /**
     * @brief Adiciona uma entidade à zona
     * @param entity Ponteiro para a entidade
     * @return true se a entidade foi adicionada, false caso contrário
     */
    bool AddEntity(IWorldEntity* entity);
    
    /**
     * @brief Remove uma entidade da zona
     * @param entityId ID da entidade
     * @return true se a entidade foi removida, false caso contrário
     */
    bool RemoveEntity(DWORD entityId);
    
    /**
     * @brief Verifica se uma entidade está na zona
     * @param entityId ID da entidade
     * @return true se a entidade está na zona, false caso contrário
     */
    bool HasEntity(DWORD entityId) const;
    
    /**
     * @brief Busca uma entidade pelo ID
     * @param entityId ID da entidade
     * @return Ponteiro para a entidade (nullptr se não existir)
     */
    IWorldEntity* GetEntity(DWORD entityId) const;
    
    /**
     * @brief Busca um jogador pelo ID
     * @param playerId ID do jogador
     * @return Ponteiro para o jogador (nullptr se não existir)
     */
    Player* GetPlayer(DWORD playerId) const;
    
    /**
     * @brief Busca um NPC pelo ID
     * @param npcId ID do NPC
     * @return Ponteiro para o NPC (nullptr se não existir)
     */
    NPC* GetNPC(DWORD npcId) const;
    
    /**
     * @brief Busca um item pelo ID
     * @param itemId ID do item
     * @return Ponteiro para o item (nullptr se não existir)
     */
    Item* GetItem(DWORD itemId) const;
    
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
     * @brief Notifica todos os jogadores na zona
     * @param packetData Dados do pacote
     * @param packetSize Tamanho do pacote
     * @param excludeId ID de jogador a excluir do envio
     * @return Número de jogadores notificados
     */
    INT NotifyPlayers(const void* packetData, size_t packetSize, DWORD excludeId = 0);
    
    /**
     * @brief Notifica todos os jogadores em uma área
     * @param area Área
     * @param packetData Dados do pacote
     * @param packetSize Tamanho do pacote
     * @param excludeId ID de jogador a excluir do envio
     * @return Número de jogadores notificados
     */
    INT NotifyPlayersInArea(const Area& area, const void* packetData, size_t packetSize, DWORD excludeId = 0);
    
    /**
     * @brief Notifica todos os jogadores em um raio
     * @param center Posição central
     * @param radius Raio
     * @param packetData Dados do pacote
     * @param packetSize Tamanho do pacote
     * @param excludeId ID de jogador a excluir do envio
     * @return Número de jogadores notificados
     */
    INT NotifyPlayersInRadius(const Position& center, float radius, const void* packetData, size_t packetSize, DWORD excludeId = 0);
    
    //=================================================================
    // Gerenciamento de visibilidade e sincronização
    //=================================================================
    
    /**
     * @brief Sincroniza o estado da zona para um jogador
     * @param playerId ID do jogador
     * @return true se a sincronização foi realizada, false caso contrário
     */
    bool SynchronizeToPlayer(DWORD playerId);
    
    /**
     * @brief Atualiza a visibilidade de entidades para um jogador
     * @param playerId ID do jogador
     * @return Número de entidades atualizadas
     */
    INT UpdateVisibilityForPlayer(DWORD playerId);
    
    /**
     * @brief Verifica a visibilidade entre duas entidades
     * @param entity1Id ID da primeira entidade
     * @param entity2Id ID da segunda entidade
     * @return true se as entidades são visíveis entre si, false caso contrário
     */
    bool AreEntitiesVisible(DWORD entity1Id, DWORD entity2Id) const;
    
    /**
     * @brief Envia informações de spawn de todas as entidades para um jogador
     * @param playerId ID do jogador
     * @return Número de entidades enviadas
     */
    INT SendAllEntitiesToPlayer(DWORD playerId);
    
    /**
     * @brief Envia informações de despawn de todas as entidades para um jogador
     * @param playerId ID do jogador
     * @return Número de entidades enviadas
     */
    INT SendAllEntitiesToPlayerAsRemoved(DWORD playerId);
    
    //=================================================================
    // Gerenciamento de portais e regiões
    //=================================================================
    
    /**
     * @brief Obtém um portal pelo ID
     * @param portalId ID do portal
     * @return Ponteiro para o portal (nullptr se não existir)
     */
    const Portal* GetPortal(WORD portalId) const;
    
    /**
     * @brief Obtém o portal na posição
     * @param position Posição
     * @param interactRadius Raio de interação
     * @return Ponteiro para o portal (nullptr se não existir)
     */
    const Portal* GetPortalAt(const Position& position, float interactRadius = 2.0f) const;
    
    /**
     * @brief Obtém todos os portais na zona
     * @return Lista de ponteiros para portais
     */
    std::vector<const Portal*> GetAllPortals() const;
    
    /**
     * @brief Obtém uma região pelo ID
     * @param regionId ID da região
     * @return Ponteiro para a região (nullptr se não existir)
     */
    const Region* GetRegion(WORD regionId) const;
    
    /**
     * @brief Obtém a região na posição
     * @param position Posição
     * @return Ponteiro para a região (nullptr se não existir)
     */
    const Region* GetRegionAt(const Position& position) const;
    
    /**
     * @brief Obtém todas as regiões na zona
     * @return Lista de ponteiros para regiões
     */
    std::vector<const Region*> GetAllRegions() const;
    
    //=================================================================
    // Detecção de colisão e pathfinding
    //=================================================================
    
    /**
     * @brief Verifica se uma posição é passável
     * @param position Posição
     * @return true se a posição é passável, false caso contrário
     */
    bool IsPositionWalkable(const Position& position) const;
    
    /**
     * @brief Obtém a altura do terreno em uma posição
     * @param position Posição
     * @return Altura do terreno (0 se inválido)
     */
    WORD GetTerrainHeight(const Position& position) const;
    
    /**
     * @brief Obtém o tipo de terreno em uma posição
     * @param position Posição
     * @return Tipo de terreno
     */
    TerrainType GetTerrainType(const Position& position) const;
    
    /**
     * @brief Verifica a linha de visão entre duas posições
     * @param start Posição inicial
     * @param end Posição final
     * @return true se há linha de visão, false caso contrário
     */
    bool HasLineOfSight(const Position& start, const Position& end) const;
    
    /**
     * @brief Verifica a colisão em uma linha
     * @param start Posição inicial
     * @param end Posição final
     * @param collisionRadius Raio de colisão
     * @param entityType Tipo de entidade para colisão (NONE para apenas terreno)
     * @param excludeId ID de entidade a excluir da colisão
     * @return true se há colisão, false caso contrário
     */
    bool CheckLineCollision(const Position& start, const Position& end, float collisionRadius = 0.0f, WorldEntityType entityType = WorldEntityType::NONE, DWORD excludeId = 0) const;
    
    //=================================================================
    // Gerenciamento de eventos
    //=================================================================
    
    /**
     * @brief Agenda um evento para a zona
     * @param delay Atraso em milissegundos
     * @param event Evento
     * @return ID do evento agendado (0 se falhar)
     */
    DWORD ScheduleEvent(DWORD delay, std::shared_ptr<IWorldEvent> event);
    
    /**
     * @brief Cancela um evento agendado
     * @param eventId ID do evento
     * @return true se o evento foi cancelado, false caso contrário
     */
    bool CancelEvent(DWORD eventId);
    
    /**
     * @brief Processa os eventos agendados
     */
    void ProcessEvents();
    
    //=================================================================
    // Utilitários
    //=================================================================
    
    /**
     * @brief Executa uma função para cada jogador na zona
     * @param callback Função a ser executada (recebe ponteiro para Player)
     */
    void ForEachPlayer(std::function<void(Player*)> callback);
    
    /**
     * @brief Executa uma função para cada NPC na zona
     * @param callback Função a ser executada (recebe ponteiro para NPC)
     */
    void ForEachNPC(std::function<void(NPC*)> callback);
    
    /**
     * @brief Executa uma função para cada item na zona
     * @param callback Função a ser executada (recebe ponteiro para Item)
     */
    void ForEachItem(std::function<void(Item*)> callback);
    
    /**
     * @brief Gera uma posição aleatória na zona
     * @param mustBeWalkable Se a posição deve ser passável
     * @return Posição aleatória
     */
    Position GenerateRandomPosition(bool mustBeWalkable = true) const;
    
    /**
     * @brief Obtém as células de mapa (grid) na zona
     * @return Grade de células
     */
    const std::vector<MapCell>& GetMapCells() const { return mapCells_; }
    
    /**
     * @brief Converte coordenadas globais para coordenadas locais
     * @param globalX Coordenada X global
     * @param globalY Coordenada Y global
     * @param localX Coordenada X local (saída)
     * @param localY Coordenada Y local (saída)
     * @return true se a conversão foi bem-sucedida, false caso contrário
     */
    bool GlobalToLocalCoordinates(WORD globalX, WORD globalY, WORD& localX, WORD& localY) const;
    
    /**
     * @brief Converte coordenadas locais para coordenadas globais
     * @param localX Coordenada X local
     * @param localY Coordenada Y local
     * @param globalX Coordenada X global (saída)
     * @param globalY Coordenada Y global (saída)
     * @return true se a conversão foi bem-sucedida, false caso contrário
     */
    bool LocalToGlobalCoordinates(WORD localX, WORD localY, WORD& globalX, WORD& globalY) const;
    
    /**
     * @brief Verifica se uma posição está dentro da zona
     * @param position Posição
     * @return true se a posição está dentro da zona, false caso contrário
     */
    bool ContainsPosition(const Position& position) const;
    
private:
    /**
     * @brief Carrega os dados da zona
     * @return true se os dados foram carregados, false caso contrário
     */
    bool LoadData();
    
    /**
     * @brief Descarrega os dados da zona
     */
    void UnloadData();
    
    /**
     * @brief Carrega os portais da zona
     * @return true se os portais foram carregados, false caso contrário
     */
    bool LoadPortals();
    
    /**
     * @brief Carrega as regiões da zona
     * @return true se as regiões foram carregadas, false caso contrário
     */
    bool LoadRegions();
    
    /**
     * @brief Carrega os objetos da zona
     * @return true se os objetos foram carregados, false caso contrário
     */
    bool LoadObjects();
    
    /**
     * @brief Carrega o navmesh da zona
     * @return true se o navmesh foi carregado, false caso contrário
     */
    bool LoadNavMesh();
    
    /**
     * @brief Gera um objeto do jogo a partir de um objeto de cenário
     * @param sceneryObject Objeto de cenário
     * @return true se o objeto foi gerado, false caso contrário
     */
    bool GenerateObjectFromScenery(const SceneryObject& sceneryObject);
    
    //=================================================================
    // Membros privados
    //=================================================================
    
    WORD mapId_;                                                  // ID do mapa
    WORD zoneX_;                                                  // Coordenada X da zona
    WORD zoneY_;                                                  // Coordenada Y da zona
    DWORD zoneId_;                                                // ID único da zona
    Area area_;                                                   // Área da zona
    bool active_;                                                 // Se a zona está ativa
    
    std::vector<MapCell> mapCells_;                               // Células de mapa
    NavMesh navMesh_;                                             // Mesh de navegação
    std::vector<const Portal*> portals_;                          // Portais na zona
    std::vector<const Region*> regions_;                          // Regiões na zona
    std::vector<const SceneryObject*> objects_;                   // Objetos na zona
    std::vector<const AreaEffectZone*> effectZones_;              // Zonas de efeito
    
    std::unordered_map<DWORD, Player*> players_;                  // Jogadores na zona
    std::unordered_map<DWORD, NPC*> npcs_;                        // NPCs na zona
    std::unordered_map<DWORD, Item*> items_;                      // Itens na zona
    
    std::multimap<DWORD, std::pair<DWORD, std::shared_ptr<IWorldEvent>>> scheduledEvents_; // Eventos agendados
    
    DWORD lastUpdateTime_;                                        // Timestamp da última atualização
    DWORD lastEventCheck_;                                        // Timestamp da última verificação de eventos
    DWORD nextEventId_;                                           // Próximo ID de evento
    
    bool initialized_;                                            // Se a zona foi inicializada
    bool dataLoaded_;                                             // Se os dados foram carregados
    
    mutable std::mutex mutex_;                                    // Mutex para thread safety
    
    static const WORD ZONE_LOCAL_SIZE = 64;                       // Tamanho da zona em tiles locais
};

} // namespace server
} // namespace wyd

#endif // ZONE_H

} // namespace wydbr
