/**
 * NPCManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/NPCManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef NPC_MANAGER_H
#define NPC_MANAGER_H

#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include <unordered_map>

#include "headers/wyd_core.h"
#include "src/server/tmsrv/npc/WYDMob.h"
#include "src/server/tmsrv/npc/WYDNPC.h"

namespace wydbr {


class TMServer;
class WYDMap;

/**
 * Gerenciador de NPCs e Mobs
 * 
 * Este sistema central gerencia todos os NPCs e monstros no mundo do jogo.
 * Controla o spawn, movimentação, comportamento, e interações com os jogadores.
 */
/**
 * Classe WYDNPCManager
 * 
 * Esta classe implementa a funcionalidade WYDNPCManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class NPCManager : public WYDSystemManager {
public:
    // /**
 * NPCManager
 * 
 * Implementa a funcionalidade NPCManager conforme especificação original.
 * @param server Parâmetro server
 * @return Retorna Construtor
 */
 Construtor
    NPCManager(TMServer* server);
    
    // Destrutor
    virtual ~NPCManager();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @return Retorna bool
 */

    bool initialize() override;
    
    // Atualização periódica
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp) override;
    
    // Desligamento
    /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 */

    void shutdown() override;
    
    // Carregamento de dados
    /**
 * loadMobData
 * 
 * Implementa a funcionalidade loadMobData conforme especificação original.
 * @param filename Parâmetro filename
 * @return Retorna bool
 */

    bool loadMobData(const std::string& filename);
    /**
 * loadNPCData
 * 
 * Implementa a funcionalidade loadNPCData conforme especificação original.
 * @param filename Parâmetro filename
 * @return Retorna bool
 */

    bool loadNPCData(const std::string& filename);
    /**
 * loadSpawnData
 * 
 * Implementa a funcionalidade loadSpawnData conforme especificação original.
 * @param filename Parâmetro filename
 * @return Retorna bool
 */

    bool loadSpawnData(const std::string& filename);
    
    // Criação de mobs e NPCs
    /**
 * createMob
 * 
 * Implementa a funcionalidade createMob conforme especificação original.
 * @param mobId Parâmetro mobId
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param 0 Parâmetro 0
 * @return Retorna uint32_t
 */

    uint32_t createMob(uint16_t mobId, uint16_t mapId, uint16_t x, uint16_t y, uint8_t direction = 0);
    /**
 * createNPC
 * 
 * Implementa a funcionalidade createNPC conforme especificação original.
 * @param npcId Parâmetro npcId
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param 0 Parâmetro 0
 * @return Retorna uint32_t
 */

    uint32_t createNPC(uint16_t npcId, uint16_t mapId, uint16_t x, uint16_t y, uint8_t direction = 0);
    
    // Spawn específico para quest ou evento
    /**
 * spawnQuestMob
 * 
 * Implementa a funcionalidade spawnQuestMob conforme especificação original.
 * @param mobId Parâmetro mobId
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param questId Parâmetro questId
 * @param 0 Parâmetro 0
 * @return Retorna uint32_t
 */

    uint32_t spawnQuestMob(uint16_t mobId, uint16_t mapId, uint16_t x, uint16_t y, uint32_t questId, uint32_t playerId = 0);
    /**
 * spawnEventMob
 * 
 * Implementa a funcionalidade spawnEventMob conforme especificação original.
 * @param mobId Parâmetro mobId
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param eventId Parâmetro eventId
 * @return Retorna uint32_t
 */

    uint32_t spawnEventMob(uint16_t mobId, uint16_t mapId, uint16_t x, uint16_t y, uint32_t eventId);
    
    // Gerenciamento de mobs e NPCs
    /**
 * getMob
 * 
 * Implementa a funcionalidade getMob conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @return Retorna WYDMob
 */

    WYDMob* getMob(uint32_t uniqueId);
    /**
 * getNPC
 * 
 * Implementa a funcionalidade getNPC conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @return Retorna WYDNPC
 */

    WYDNPC* getNPC(uint32_t uniqueId);
    /**
 * getEntity
 * 
 * Implementa a funcionalidade getEntity conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @return Retorna WYDEntity
 */

    WYDEntity* getEntity(uint32_t uniqueId);
    /**
 * removeMob
 * 
 * Implementa a funcionalidade removeMob conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @return Retorna bool
 */

    bool removeMob(uint32_t uniqueId);
    /**
 * removeNPC
 * 
 * Implementa a funcionalidade removeNPC conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @return Retorna bool
 */

    bool removeNPC(uint32_t uniqueId);
    
    // Zonas e áreas
    std::vector<uint32_t> getMobsInRange(uint16_t mapId, uint16_t centerX, uint16_t centerY, uint16_t radius);
    std::vector<uint32_t> getNPCsInRange(uint16_t mapId, uint16_t centerX, uint16_t centerY, uint16_t radius);
    std::vector<uint32_t> getMobsByType(uint16_t mobId, uint16_t mapId = 0);
    
    // Interações
    /**
 * moveMob
 * 
 * Implementa a funcionalidade moveMob conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @param targetX Parâmetro targetX
 * @param targetY Parâmetro targetY
 * @return Retorna bool
 */

    bool moveMob(uint32_t uniqueId, uint16_t targetX, uint16_t targetY);
    /**
 * attackMob
 * 
 * Implementa a funcionalidade attackMob conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @param targetId Parâmetro targetId
 * @param 0 Parâmetro 0
 * @return Retorna bool
 */

    bool attackMob(uint32_t uniqueId, uint32_t targetId, uint16_t skillId = 0);
    /**
 * talkToNPC
 * 
 * Implementa a funcionalidade talkToNPC conforme especificação original.
 * @param npcId Parâmetro npcId
 * @param playerId Parâmetro playerId
 * @param 0 Parâmetro 0
 * @return Retorna bool
 */

    bool talkToNPC(uint32_t npcId, uint32_t playerId, uint16_t dialogueOption = 0);
    
    // Gerenciamento do sistema de spawn
    /**
 * startWorldSpawns
 * 
 * Implementa a funcionalidade startWorldSpawns conforme especificação original.
 */

    void startWorldSpawns();
    /**
 * stopWorldSpawns
 * 
 * Implementa a funcionalidade stopWorldSpawns conforme especificação original.
 */

    void stopWorldSpawns();
    /**
 * resetMapSpawns
 * 
 * Implementa a funcionalidade resetMapSpawns conforme especificação original.
 * @param mapId Parâmetro mapId
 */

    void resetMapSpawns(uint16_t mapId);
    
    // Informações e estatísticas
    /**
 * getMobCount
 * 
 * Implementa a funcionalidade getMobCount conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna size_t
 */

    size_t getMobCount(uint16_t mapId = 0);
    /**
 * getNPCCount
 * 
 * Implementa a funcionalidade getNPCCount conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna size_t
 */

    size_t getNPCCount(uint16_t mapId = 0);
    std::map<uint16_t, uint32_t> getMobDistribution();
    
private:
    // Estruturas de dados para templates de mobs
    struct MobTemplate {
        uint16_t id;
        std::string name;
        uint16_t level;
        uint32_t hp;
        uint32_t mp;
        uint16_t strength;
        uint16_t dexterity;
        uint16_t intelligence;
        uint16_t constitution;
        uint16_t attack;
        uint16_t defense;
        uint16_t aggroRange;
        uint16_t moveSpeed;
        uint16_t attackSpeed;
        bool isBoss;
        bool isAggressive;
        uint16_t skills[8];
        uint16_t dropItems[16];
        uint8_t dropRates[16];
        uint32_t experience;
        uint32_t gold;
    };
    
    // Estrutura para ponto de spawn
    struct SpawnPoint {
        uint16_t mobId;
        uint16_t mapId;
        uint16_t x;
        uint16_t y;
        uint16_t range;
        uint16_t amount;
        uint32_t respawnTime;
        uint64_t lastSpawnTime;
        std::vector<uint32_t> spawnedMobs;
    };
    
    // Templates de mobs e NPCs
    std::unordered_map<uint16_t, MobTemplate> mobTemplates_;
    std::unordered_map<uint16_t, MobTemplate> npcTemplates_; // Usa mesma estrutura, mas com flags diferentes
    
    // Instâncias ativas de mobs e NPCs
    std::unordered_map<uint32_t, std::unique_ptr<WYDMob>> activeMobs_;
    std::unordered_map<uint32_t, std::unique_ptr<WYDNPC>> activeNPCs_;
    
    // Pontos de spawn
    std::vector<SpawnPoint> spawnPoints_;
    
    // Índices para busca eficiente
    std::unordered_map<uint16_t, std::vector<uint32_t>> mobsByMap_;     // mapId -> [mobId, mobId, ...]
    std::unordered_map<uint16_t, std::vector<uint32_t>> npcsByMap_;     // mapId -> [npcId, npcId, ...]
    std::unordered_map<uint16_t, std::vector<uint32_t>> mobsByType_;    // mobId -> [uniqueId, uniqueId, ...]
    
    // Estado do sistema
    bool spawnSystemActive_;
    uint64_t lastSpawnCheck_;
    uint64_t lastMobUpdate_;
    uint32_t nextUniqueId_;
    
    // Mutex para proteção
    mutable std::mutex mobMutex_;
    mutable std::mutex npcMutex_;
    mutable std::mutex spawnMutex_;
    
    // Métodos auxiliares
    /**
 * updateSpawns
 * 
 * Implementa a funcionalidade updateSpawns conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateSpawns(uint64_t timestamp);
    /**
 * updateMobs
 * 
 * Implementa a funcionalidade updateMobs conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateMobs(uint64_t timestamp);
    /**
 * updateNPCs
 * 
 * Implementa a funcionalidade updateNPCs conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateNPCs(uint64_t timestamp);
    /**
 * createMobInstance
 * 
 * Implementa a funcionalidade createMobInstance conforme especificação original.
 * @param tmpl Parâmetro tmpl
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param direction Parâmetro direction
 * @return Retorna WYDMob
 */

    WYDMob* createMobInstance(const MobTemplate& tmpl, uint16_t mapId, uint16_t x, uint16_t y, uint8_t direction);
    /**
 * createNPCInstance
 * 
 * Implementa a funcionalidade createNPCInstance conforme especificação original.
 * @param tmpl Parâmetro tmpl
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param direction Parâmetro direction
 * @return Retorna WYDNPC
 */

    WYDNPC* createNPCInstance(const MobTemplate& tmpl, uint16_t mapId, uint16_t x, uint16_t y, uint8_t direction);
    /**
 * checkMobsInRange
 * 
 * Implementa a funcionalidade checkMobsInRange conforme especificação original.
 * @param mob Parâmetro mob
 * @param radius Parâmetro radius
 */

    void checkMobsInRange(WYDMob* mob, uint16_t radius);
    /**
 * processMobDeath
 * 
 * Implementa a funcionalidade processMobDeath conforme especificação original.
 * @param mob Parâmetro mob
 * @param killerId Parâmetro killerId
 */

    void processMobDeath(WYDMob* mob, uint32_t killerId);
    /**
 * isValidSpawnLocation
 * 
 * Implementa a funcionalidade isValidSpawnLocation conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool isValidSpawnLocation(uint16_t mapId, uint16_t x, uint16_t y);
};

#endif // NPC_MANAGER_H

} // namespace wydbr
