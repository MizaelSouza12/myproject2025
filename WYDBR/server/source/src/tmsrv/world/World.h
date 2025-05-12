/**
 * World.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/tmsrv/world/World.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYDBR_WORLD_H
#define WYDBR_WORLD_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>

#include "../../../include/Basedef.h"
#include "../player/WYDPlayer.h"

namespace wydbr {
namespace tmsrv {

/**
 * @brief Tipo de célula no mapa
 */
enum /**
 * Classe WYDCellType
 * 
 * Esta classe implementa a funcionalidade WYDCellType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CellType {
    NORMAL,     // Célula normal
    OBSTACLE,   // Obstáculo/parede
    WATER,      // Água
    SAFEZONE    // Zona segura (não permite PvP)
};

/**
 * @brief Estrutura de célula do mapa
 */
struct MapCell {
    CellType type;                      // Tipo da célula
    bool allowMovement;                 // Permite movimento
    bool allowAttack;                   // Permite ataque
    std::vector<uint32_t> entityIDs;    // IDs das entidades nesta célula
};

/**
 * @class World
 * @brief Classe que representa o mundo do jogo
 * 
 * Gerencia todos os aspectos do mundo, incluindo mapas, NPCs, 
 * clima, mobs, drops, e eventos mundiais.
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
     * @brief Constrói um novo mundo
     * @param name Nome do mundo
     */
    explicit World(const std::string& name);
    
    /**
     * @brief Destrutor
     */
    ~World();
    
    /**
     * @brief Inicializa o mundo
     * @return true se a inicialização foi bem-sucedida
     */
    bool initialize();
    
    /**
     * @brief Atualiza o estado do mundo
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void update(float deltaTime);
    
    /**
     * @brief Carrega um mapa
     * @param mapID ID do mapa
     * @param fileName Nome do arquivo do mapa
     * @return true se o mapa foi carregado com sucesso
     */
    bool loadMap(int mapID, const std::string& fileName);
    
    /**
     * @brief Registra um jogador no mundo
     * @param player Jogador a ser registrado
     * @return true se o jogador foi registrado com sucesso
     */
    bool registerPlayer(std::shared_ptr<WYDPlayer> player);
    
    /**
     * @brief Remove um jogador do mundo
     * @param playerID ID do jogador
     */
    void removePlayer(uint32_t playerID);
    
    /**
     * @brief Obtém um jogador pelo ID
     * @param playerID ID do jogador
     * @return Ponteiro para o jogador ou nullptr
     */
    std::shared_ptr<WYDPlayer> getPlayer(uint32_t playerID);
    
    /**
     * @brief Obtém o tipo de célula em uma posição
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return Tipo da célula
     */
    CellType getCellType(int mapID, int x, int y);
    
    /**
     * @brief Verifica se uma posição é válida
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return true se a posição for válida
     */
    bool isValidPosition(int mapID, int x, int y);
    
    /**
     * @brief Verifica se um movimento é válido
     * @param mapID ID do mapa
     * @param fromX Coordenada X inicial
     * @param fromY Coordenada Y inicial
     * @param toX Coordenada X final
     * @param toY Coordenada Y final
     * @return true se o movimento for válido
     */
    bool isValidMovement(int mapID, int fromX, int fromY, int toX, int toY);
    
    /**
     * @brief Obtém jogadores próximos a uma posição
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param radius Raio de busca
     * @return Vetor de IDs de jogadores próximos
     */
    std::vector<uint32_t> getNearbyPlayers(int mapID, int x, int y, int radius);
    
    /**
     * @brief Obtém mobs próximos a uma posição
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param radius Raio de busca
     * @return Vetor de IDs de mobs próximos
     */
    std::vector<uint32_t> getNearbyMobs(int mapID, int x, int y, int radius);
    
    /**
     * @brief Obtém entidades próximas a uma posição
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param radius Raio de busca
     * @return Vetor de IDs de entidades próximas
     */
    std::vector<uint32_t> getNearbyEntities(int mapID, int x, int y, int radius);
    
    /**
     * @brief Verifica se uma posição está em uma zona segura
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return true se a posição estiver em uma zona segura
     */
    bool isSafeZone(int mapID, int x, int y);
    
    /**
     * @brief Verifica se um mapa existe
     * @param mapID ID do mapa
     * @return true se o mapa existir
     */
    bool mapExists(int mapID) const;
    
    /**
     * @brief Envia mensagem para todos os jogadores
     * @param message Mensagem a ser enviada
     * @param mapID ID do mapa (0 para todos)
     */
    void broadcastMessage(const std::string& message, int mapID = 0);
    
    /**
     * @brief Envia pacote para todos os jogadores
     * @param packet Pacote a ser enviado
     * @param mapID ID do mapa (0 para todos)
     */
    void broadcastPacket(const MSG_STANDARD* packet, int mapID = 0);
    
    /**
     * @brief Cria um NPC no mundo
     * @param mobID ID do mob/NPC
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return ID da entidade criada ou 0 em caso de falha
     */
    uint32_t createNPC(int mobID, int mapID, int x, int y);
    
    /**
     * @brief Cria um mob no mundo
     * @param mobID ID do mob
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param count Quantidade a criar
     * @return IDs das entidades criadas
     */
    std::vector<uint32_t> createMobs(int mobID, int mapID, int x, int y, int count = 1);
    
    /**
     * @brief Registra um item no chão
     * @param item Item a ser registrado
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param ownerID ID do dono (0 para nenhum)
     * @param duration Duração em milissegundos (0 para padrão)
     * @return ID do item ou 0 em caso de falha
     */
    uint32_t dropItem(const STRUCT_ITEM& item, int mapID, int x, int y, uint32_t ownerID = 0, int duration = 0);
    
    /**
     * @brief Remove um item do chão
     * @param itemID ID do item
     * @return true se o item foi removido
     */
    bool removeDroppedItem(uint32_t itemID);
    
    /**
     * @brief Obtém o nome do mundo
     * @return Nome do mundo
     */
    std::string getName() const;
    
    /**
     * @brief Obtém o número de jogadores online
     * @return Número de jogadores
     */
    int getPlayerCount() const;
    
    /**
     * @brief Obtém o número de mobs no mundo
     * @return Número de mobs
     */
    int getMobCount() const;
    
    /**
     * @brief Obtém o número de itens no chão
     * @return Número de itens
     */
    int getDroppedItemCount() const;
    
    /**
     * @brief Verifica distância entre duas posições
     * @param x1 Coordenada X da primeira posição
     * @param y1 Coordenada Y da primeira posição
     * @param x2 Coordenada X da segunda posição
     * @param y2 Coordenada Y da segunda posição
     * @return Distância em células
     */
    static int getDistance(int x1, int y1, int x2, int y2);

private:
    /**
     * @brief Atualiza os mobs do mundo
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void updateMobs(float deltaTime);
    
    /**
     * @brief Atualiza os itens no chão
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void updateDroppedItems(float deltaTime);
    
    /**
     * @brief Verifica colisão entre duas entidades
     * @param mapID ID do mapa
     * @param x1 Coordenada X da primeira entidade
     * @param y1 Coordenada Y da primeira entidade
     * @param x2 Coordenada X da segunda entidade
     * @param y2 Coordenada Y da segunda entidade
     * @return true se houver colisão
     */
    bool checkCollision(int mapID, int x1, int y1, int x2, int y2);
    
    /**
     * @brief Processa o respawn de mobs
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void processRespawns(float deltaTime);
    
    /**
     * @brief Atualiza eventos do mundo
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void updateWorldEvents(float deltaTime);
    
    /**
     * @brief Obtém o índice da célula a partir das coordenadas
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param mapWidth Largura do mapa
     * @return Índice da célula
     */
    int getCellIndex(int x, int y, int mapWidth) const;
    
    /**
     * @brief Atualiza a célula com a posição da entidade
     * @param entityID ID da entidade
     * @param mapID ID do mapa
     * @param oldX Coordenada X antiga
     * @param oldY Coordenada Y antiga
     * @param newX Coordenada X nova
     * @param newY Coordenada Y nova
     */
    void updateEntityCell(uint32_t entityID, int mapID, int oldX, int oldY, int newX, int newY);

private:
    std::string m_name;                   // Nome do mundo
    
    // Mapas do mundo
    struct Map {
        int mapID;
        std::string name;
        int width;
        int height;
        int attributes;                    // Atributos do mapa
        std::vector<MapCell> cells;        // Células do mapa
    };
    std::map<int, Map> m_maps;             // Mapas por ID
    
    // Entidades no mundo
    std::mutex m_playersMutex;
    std::map<uint32_t, std::shared_ptr<WYDPlayer>> m_players; // Jogadores por ID
    
    struct Mob {
        uint32_t entityID;
        int mobID;
        int mapID;
        int x;
        int y;
        int direction;
        int hp;
        int level;
        // Demais atributos
    };
    std::mutex m_mobsMutex;
    std::map<uint32_t, Mob> m_mobs;        // Mobs por ID
    
    struct DroppedItem {
        uint32_t itemID;
        STRUCT_ITEM item;
        int mapID;
        int x;
        int y;
        uint32_t ownerID;                  // ID do jogador dono
        int64_t dropTime;                  // Timestamp do drop
        int64_t expireTime;                // Timestamp de expiração
    };
    std::mutex m_itemsMutex;
    std::map<uint32_t, DroppedItem> m_droppedItems; // Itens no chão por ID
    
    // Geradores de IDs
    std::atomic<uint32_t> m_nextEntityID;  // Próximo ID de entidade
    std::atomic<uint32_t> m_nextItemID;    // Próximo ID de item
    
    // Estado do mundo
    std::atomic<bool> m_initialized;       // Indica se o mundo foi inicializado
    int64_t m_worldTick;                   // Tick do mundo
    
    // Pontos de spawns
    struct SpawnPoint {
        int mapID;
        int x;
        int y;
        int mobID;
        int count;
        int respawnTime;                   // Tempo de respawn em segundos
        int64_t lastSpawnTime;             // Último timestamp de spawn
    };
    std::vector<SpawnPoint> m_spawnPoints;
};

} // namespace tmsrv
} // namespace wydbr

#endif // WYDBR_WORLD_H