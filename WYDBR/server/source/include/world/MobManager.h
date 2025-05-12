/**
 * MobManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/world/MobManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _MOB_MANAGER_H_
#define _MOB_MANAGER_H_

/**
 * @file MobManager.h
 * @brief Gerenciador de mobs (monstros e NPCs) do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de mobs do WYDBR,
 * responsável por gerenciar os monstros e NPCs do jogo,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <random>

#include "../GlobalDef.h"
#include "../Basedef.h"
#include "../TM_Mob.h"
#include "MapManager.h"

namespace wydbr {
namespace world {

/**
 * @brief Tipo de mob
 */
enum /**
 * Classe WYDMobType
 * 
 * Esta classe implementa a funcionalidade WYDMobType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MobType {
    NONE = 0,               // Nenhum
    MONSTER = 1,            // Monstro
    NPC = 2,                // NPC
    MERCHANT = 3,           // Mercador
    QUEST = 4,              // Missão
    BOSS = 5,               // Chefe
    SUMMON = 6,             // Invocação
    EVENT = 7,              // Evento
    MOUNT = 8,              // Montaria
    PET = 9,                // Mascote
    GUARDIAN = 10,          // Guardião
    CUSTOM_1 = 11,          // Personalizado 1
    CUSTOM_2 = 12,          // Personalizado 2
    CUSTOM_3 = 13,          // Personalizado 3
    UNKNOWN = 14            // Desconhecido
};

/**
 * @brief Estado do mob
 */
enum /**
 * Classe WYDMobState
 * 
 * Esta classe implementa a funcionalidade WYDMobState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MobState {
    NONE = 0,               // Nenhum
    IDLE = 1,               // Ocioso
    ROAMING = 2,            // Perambulando
    AGGRO = 3,              // Agressivo
    COMBAT = 4,             // Combate
    RETURNING = 5,          // Retornando
    DEAD = 6,               // Morto
    DESPAWNED = 7,          // Desaparecido
    LOOT = 8,               // Saque
    CUSTOM_1 = 9,           // Personalizado 1
    CUSTOM_2 = 10,          // Personalizado 2
    CUSTOM_3 = 11,          // Personalizado 3
    UNKNOWN = 12            // Desconhecido
};

/**
 * @brief Comportamento do mob
 */
enum /**
 * Classe WYDMobBehavior
 * 
 * Esta classe implementa a funcionalidade WYDMobBehavior conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MobBehavior {
    NONE = 0,               // Nenhum
    PASSIVE = 1,            // Passivo
    AGGRESSIVE = 2,         // Agressivo
    NEUTRAL = 3,            // Neutro
    SENTINEL = 4,           // Sentinela
    COWARD = 5,             // Covarde
    TERRITORIAL = 6,        // Territorial
    SOCIAL = 7,             // Social
    CUSTOM_1 = 8,           // Personalizado 1
    CUSTOM_2 = 9,           // Personalizado 2
    CUSTOM_3 = 10,          // Personalizado 3
    UNKNOWN = 11            // Desconhecido
};

/**
 * @brief Flags do mob
 */
enum class MobFlags : uint32_t {
    NONE = 0,                       // Nenhum
    CAN_MOVE = 1 << 0,              // Pode mover
    CAN_ATTACK = 1 << 1,            // Pode atacar
    BOSS = 1 << 2,                  // Chefe
    ELITE = 1 << 3,                 // Elite
    RANGED = 1 << 4,                // À distância
    MAGIC = 1 << 5,                 // Mágico
    FLYING = 1 << 6,                // Voador
    VEHICLE = 1 << 7,               // Veículo
    MERCHANT = 1 << 8,              // Mercador
    QUEST_GIVER = 1 << 9,           // Entregador de missão
    ARENA_MASTER = 1 << 10,         // Mestre de arena
    GUILD_MASTER = 1 << 11,         // Mestre de guilda
    TELEPORTER = 1 << 12,           // Teleportador
    HEALER = 1 << 13,               // Curador
    BANKER = 1 << 14,               // Banqueiro
    AUCTIONEER = 1 << 15,           // Leiloeiro
    GUARD = 1 << 16,                // Guarda
    SUMMONED = 1 << 17,             // Invocado
    MOUNT = 1 << 18,                // Montaria
    PET = 1 << 19,                  // Mascote
    TAMED = 1 << 20,                // Domado
    CUSTOM_1 = 1 << 21,             // Personalizado 1
    CUSTOM_2 = 1 << 22,             // Personalizado 2
    CUSTOM_3 = 1 << 23,             // Personalizado 3
    UNKNOWN = 1 << 24               // Desconhecido
};

/**
 * @brief Família do mob
 */
enum /**
 * Classe WYDMobFamily
 * 
 * Esta classe implementa a funcionalidade WYDMobFamily conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MobFamily {
    NONE = 0,               // Nenhum
    HUMANOID = 1,           // Humanoide
    BEAST = 2,              // Besta
    UNDEAD = 3,             // Morto-vivo
    DEMON = 4,              // Demônio
    ELEMENTAL = 5,          // Elemental
    DRAGON = 6,             // Dragão
    GIANT = 7,              // Gigante
    INSECT = 8,             // Inseto
    PLANT = 9,              // Planta
    MECHANICAL = 10,        // Mecânico
    CUSTOM_1 = 11,          // Personalizado 1
    CUSTOM_2 = 12,          // Personalizado 2
    CUSTOM_3 = 13,          // Personalizado 3
    UNKNOWN = 14            // Desconhecido
};

/**
 * @brief Resultado da operação de mob
 */
enum /**
 * Classe WYDMobResult
 * 
 * Esta classe implementa a funcionalidade WYDMobResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MobResult {
    SUCCESS = 0,                    // Sucesso
    FAILED = 1,                     // Falha
    MOB_NOT_FOUND = 2,              // Mob não encontrado
    MOB_ALREADY_EXISTS = 3,         // Mob já existe
    INVALID_POSITION = 4,           // Posição inválida
    INVALID_STATE = 5,              // Estado inválido
    INVALID_BEHAVIOR = 6,           // Comportamento inválido
    INVALID_TYPE = 7,               // Tipo inválido
    INVALID_TEMPLATE = 8,           // Modelo inválido
    INVALID_SPAWN = 9,              // Spawn inválido
    CUSTOM_1 = 10,                  // Personalizado 1
    CUSTOM_2 = 11,                  // Personalizado 2
    CUSTOM_3 = 12,                  // Personalizado 3
    UNKNOWN = 13                    // Desconhecido
};

/**
 * @brief Área de spawn
 */
struct SpawnArea {
    uint32_t AreaID;                // ID da área
    uint16_t MapID;                 // ID do mapa
    uint16_t MinX;                  // X mínimo
    uint16_t MinY;                  // Y mínimo
    uint16_t MaxX;                  // X máximo
    uint16_t MaxY;                  // Y máximo
    std::vector<uint16_t> MobList;  // Lista de mobs
    uint16_t MaxCount;              // Contagem máxima
    uint32_t RespawnTime;           // Tempo de respawn
    uint32_t DespawnTime;           // Tempo de despawn
    bool Enabled;                   // Habilitado
    
    SpawnArea() : AreaID(0), MapID(0), MinX(0), MinY(0), MaxX(0), MaxY(0),
        MaxCount(0), RespawnTime(0), DespawnTime(0), Enabled(true) {}
};

/**
 * @brief Modelo de mob
 */
struct MobTemplate {
    uint16_t TemplateID;            // ID do modelo
    std::string Name;               // Nome
    MobType Type;                   // Tipo
    MobBehavior Behavior;           // Comportamento
    uint32_t Flags;                 // Flags
    MobFamily Family;               // Família
    uint8_t Level;                  // Nível
    uint16_t Attack;                // Ataque
    uint16_t Defense;               // Defesa
    uint32_t HP;                    // HP
    uint32_t MP;                    // MP
    uint16_t Str;                   // STR
    uint16_t Int;                   // INT
    uint16_t Dex;                   // DEX
    uint16_t Con;                   // CON
    uint8_t Speed;                  // Velocidade
    uint16_t Critical;              // Crítico
    uint16_t Dodge;                 // Esquiva
    uint16_t Block;                 // Bloqueio
    uint16_t Parry;                 // Aparar
    uint16_t FireResist;            // Resistência a fogo
    uint16_t IceResist;             // Resistência a gelo
    uint16_t LightningResist;       // Resistência a relâmpago
    uint16_t HolyResist;            // Resistência a sagrado
    uint16_t DarkResist;            // Resistência a sombrio
    uint16_t PoisonResist;          // Resistência a veneno
    uint16_t StunResist;            // Resistência a atordoamento
    uint32_t Experience;            // Experiência
    uint32_t Gold;                  // Ouro
    uint8_t ViewRange;              // Alcance de visão
    uint8_t AttackRange;            // Alcance de ataque
    uint8_t ChaseRange;             // Alcance de perseguição
    uint16_t Size;                  // Tamanho
    uint8_t AttackSpeed;            // Velocidade de ataque
    std::vector<uint16_t> Skills;   // Habilidades
    std::vector<uint16_t> Drops;    // Drops
    
    MobTemplate() : TemplateID(0), Type(MobType::NONE),
        Behavior(MobBehavior::NONE), Flags(0), Family(MobFamily::NONE),
        Level(0), Attack(0), Defense(0), HP(0), MP(0), Str(0), Int(0),
        Dex(0), Con(0), Speed(0), Critical(0), Dodge(0), Block(0),
        Parry(0), FireResist(0), IceResist(0), LightningResist(0),
        HolyResist(0), DarkResist(0), PoisonResist(0), StunResist(0),
        Experience(0), Gold(0), ViewRange(0), AttackRange(0),
        ChaseRange(0), Size(0), AttackSpeed(0) {}
};

/**
 * @brief Instância de mob
 */
struct MobInstance {
    uint32_t MobID;                 // ID do mob
    uint16_t TemplateID;            // ID do modelo
    uint16_t Model;                 // Modelo
    STRUCT_MOB MOB;                 // MOB
    MobState State;                 // Estado
    Position Position;              // Posição
    Position HomePosition;          // Posição inicial
    uint32_t OwnerID;               // ID do dono
    uint32_t TargetID;              // ID do alvo
    uint32_t SpawnTime;             // Tempo de spawn
    uint32_t DespawnTime;           // Tempo de despawn
    uint32_t LastMoveTime;          // Tempo do último movimento
    uint32_t LastAttackTime;        // Tempo do último ataque
    uint32_t DeathTime;             // Tempo da morte
    uint8_t Direction;              // Direção
    bool Visible;                   // Visível
    std::vector<uint32_t> AggroList;// Lista de aggro
    uint32_t SpawnAreaID;           // ID da área de spawn
    
    MobInstance() : MobID(0), TemplateID(0), Model(0), State(MobState::NONE),
        OwnerID(0), TargetID(0), SpawnTime(0), DespawnTime(0),
        LastMoveTime(0), LastAttackTime(0), DeathTime(0), Direction(0),
        Visible(true), SpawnAreaID(0) {
        memset(&MOB, 0, sizeof(STRUCT_MOB));
    }
};

/**
 * @brief Drop de item
 */
struct ItemDrop {
    uint16_t ItemID;                // ID do item
    uint16_t Chance;                // Chance
    uint16_t MinAmount;             // Quantidade mínima
    uint16_t MaxAmount;             // Quantidade máxima
    uint16_t MinLevel;              // Nível mínimo
    uint16_t MaxLevel;              // Nível máximo
    
    ItemDrop() : ItemID(0), Chance(0), MinAmount(0), MaxAmount(0),
        MinLevel(0), MaxLevel(0) {}
};

/**
 * @brief Tabela de drop
 */
struct DropTable {
    uint16_t TableID;               // ID da tabela
    std::string Name;               // Nome
    std::vector<ItemDrop> Drops;    // Drops
    
    DropTable() /**
 * TableID
 * 
 * Implementa a funcionalidade TableID conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 : TableID(0){}
};

/**
 * @brief Configuração de mob
 */
struct MobConfig {
    uint32_t MaxMobs;               // Máximo de mobs
    uint32_t MaxSpawnAreas;         // Máximo de áreas de spawn
    uint32_t DefaultRespawnTime;    // Tempo de respawn padrão
    uint32_t DefaultDespawnTime;    // Tempo de despawn padrão
    uint32_t DespawnDelay;          // Atraso de despawn
    uint32_t CorpseDecayTime;       // Tempo de deterioração do cadáver
    uint32_t UpdateInterval;        // Intervalo de atualização
    uint32_t MaxAggroListSize;      // Tamanho máximo da lista de aggro
    float ExpMultiplier;            // Multiplicador de experiência
    float GoldMultiplier;           // Multiplicador de ouro
    float DropRateMultiplier;       // Multiplicador de taxa de drop
    float RareDropRateMultiplier;   // Multiplicador de taxa de drop raro
    float UniqueDropRateMultiplier; // Multiplicador de taxa de drop único
    float BossDropRateMultiplier;   // Multiplicador de taxa de drop de chefe
    bool EnableDynamicSpawning;     // Habilitar spawn dinâmico
    uint32_t DynamicSpawnInterval;  // Intervalo de spawn dinâmico
    uint32_t MaxChaseDuration;      // Duração máxima de perseguição
    uint32_t MaxRouteDuration;      // Duração máxima de rota
    
    MobConfig() : MaxMobs(10000), MaxSpawnAreas(1000),
        DefaultRespawnTime(300), DefaultDespawnTime(0),
        DespawnDelay(60), CorpseDecayTime(300), UpdateInterval(1000),
        MaxAggroListSize(10), ExpMultiplier(1.0f), GoldMultiplier(1.0f),
        DropRateMultiplier(1.0f), RareDropRateMultiplier(1.0f),
        UniqueDropRateMultiplier(1.0f), BossDropRateMultiplier(1.0f),
        EnableDynamicSpawning(true), DynamicSpawnInterval(60),
        MaxChaseDuration(30), MaxRouteDuration(120) {}
};

/**
 * @brief Estatísticas de mob
 */
struct MobStats {
    uint32_t TotalMobs;             // Total de mobs
    uint32_t ActiveMobs;            // Mobs ativos
    uint32_t NpcCount;              // Contagem de NPCs
    uint32_t MonsterCount;          // Contagem de monstros
    uint32_t BossCount;             // Contagem de chefes
    uint32_t SummonCount;           // Contagem de invocações
    uint32_t PetCount;              // Contagem de mascotes
    uint32_t KillCount;             // Contagem de abates
    uint32_t RespawnCount;          // Contagem de respawns
    uint32_t DespawnCount;          // Contagem de despawns
    uint32_t SpawnAreas;            // Áreas de spawn
    
    MobStats() : TotalMobs(0), ActiveMobs(0), NpcCount(0),
        MonsterCount(0), BossCount(0), SummonCount(0), PetCount(0),
        KillCount(0), RespawnCount(0), DespawnCount(0), SpawnAreas(0) {}
};

/**
 * @brief Callback de morte de mob
 */
using MobDeathCallback = std::function<void(uint32_t mobID, uint32_t killerID)>;

/**
 * @brief Callback de spawn de mob
 */
using MobSpawnCallback = std::function<void(uint32_t mobID, const Position& position)>;

/**
 * @brief Gerenciador de mobs
 * 
 * Esta classe é responsável por gerenciar os mobs do jogo.
 */
/**
 * Classe WYDMobManager
 * 
 * Esta classe implementa a funcionalidade WYDMobManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MobManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static MobManager& getInstance() {
        static MobManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @param mapManager Gerenciador de mapas
     * @return true se inicializado com sucesso
     */
    bool initialize(MapManager* mapManager);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração de mob
     * @return true se carregado com sucesso
     */
    bool loadMobConfig();
    
    /**
     * @brief Carrega os modelos de mob
     * @return true se carregado com sucesso
     */
    bool loadMobTemplates();
    
    /**
     * @brief Carrega as áreas de spawn
     * @return true se carregado com sucesso
     */
    bool loadSpawnAreas();
    
    /**
     * @brief Carrega as tabelas de drop
     * @return true se carregado com sucesso
     */
    bool loadDropTables();
    
    /**
     * @brief Cria um mob
     * @param templateID ID do modelo
     * @param position Posição
     * @param direction Direção
     * @param ownerID ID do dono (0 para nenhum)
     * @param spawnAreaID ID da área de spawn (0 para nenhum)
     * @return ID do mob ou 0 em caso de erro
     */
    uint32_t createMob(uint16_t templateID, const Position& position, uint8_t direction, uint32_t ownerID = 0, uint32_t spawnAreaID = 0);
    
    /**
     * @brief Remove um mob
     * @param mobID ID do mob
     * @return Resultado da operação
     */
    MobResult removeMob(uint32_t mobID);
    
    /**
     * @brief Spawna uma área de spawn
     * @param areaID ID da área
     * @return Número de mobs spawnados
     */
    uint32_t spawnArea(uint32_t areaID);
    
    /**
     * @brief Despawna uma área de spawn
     * @param areaID ID da área
     * @return Número de mobs despawnados
     */
    uint32_t despawnArea(uint32_t areaID);
    
    /**
     * @brief Habilita uma área de spawn
     * @param areaID ID da área
     * @param enabled Habilitado
     * @return true se habilitado com sucesso
     */
    bool enableSpawnArea(uint32_t areaID, bool enabled);
    
    /**
     * @brief Mata um mob
     * @param mobID ID do mob
     * @param killerID ID do assassino (0 para nenhum)
     * @param generateLoot Gerar saque
     * @return Resultado da operação
     */
    MobResult killMob(uint32_t mobID, uint32_t killerID = 0, bool generateLoot = true);
    
    /**
     * @brief Revive um mob
     * @param mobID ID do mob
     * @return Resultado da operação
     */
    MobResult reviveMob(uint32_t mobID);
    
    /**
     * @brief Despawna um mob
     * @param mobID ID do mob
     * @return Resultado da operação
     */
    MobResult despawnMob(uint32_t mobID);
    
    /**
     * @brief Move um mob
     * @param mobID ID do mob
     * @param newPosition Nova posição
     * @param direction Direção
     * @return Resultado da operação
     */
    MobResult moveMob(uint32_t mobID, const Position& newPosition, uint8_t direction);
    
    /**
     * @brief Teleporta um mob
     * @param mobID ID do mob
     * @param newPosition Nova posição
     * @param direction Direção
     * @return Resultado da operação
     */
    MobResult teleportMob(uint32_t mobID, const Position& newPosition, uint8_t direction);
    
    /**
     * @brief Deixa um mob visível
     * @param mobID ID do mob
     * @param visible Visível
     * @return Resultado da operação
     */
    MobResult setMobVisible(uint32_t mobID, bool visible);
    
    /**
     * @brief Define o estado de um mob
     * @param mobID ID do mob
     * @param state Estado
     * @return Resultado da operação
     */
    MobResult setMobState(uint32_t mobID, MobState state);
    
    /**
     * @brief Define o alvo de um mob
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @return Resultado da operação
     */
    MobResult setMobTarget(uint32_t mobID, uint32_t targetID);
    
    /**
     * @brief Adiciona aggro a um mob
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @param amount Quantidade
     * @return Resultado da operação
     */
    MobResult addMobAggro(uint32_t mobID, uint32_t targetID, uint32_t amount);
    
    /**
     * @brief Remove aggro de um mob
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @return Resultado da operação
     */
    MobResult removeMobAggro(uint32_t mobID, uint32_t targetID);
    
    /**
     * @brief Limpa a lista de aggro de um mob
     * @param mobID ID do mob
     * @return Resultado da operação
     */
    MobResult clearMobAggro(uint32_t mobID);
    
    /**
     * @brief Obtém o alvo com mais aggro de um mob
     * @param mobID ID do mob
     * @return ID do alvo ou 0 se não encontrado
     */
    uint32_t getMobHighestAggroTarget(uint32_t mobID);
    
    /**
     * @brief Cria um NPC
     * @param templateID ID do modelo
     * @param position Posição
     * @param direction Direção
     * @return ID do NPC ou 0 em caso de erro
     */
    uint32_t createNPC(uint16_t templateID, const Position& position, uint8_t direction);
    
    /**
     * @brief Cria um pet
     * @param templateID ID do modelo
     * @param position Posição
     * @param direction Direção
     * @param ownerID ID do dono
     * @return ID do pet ou 0 em caso de erro
     */
    uint32_t createPet(uint16_t templateID, const Position& position, uint8_t direction, uint32_t ownerID);
    
    /**
     * @brief Cria um chefe
     * @param templateID ID do modelo
     * @param position Posição
     * @param direction Direção
     * @return ID do chefe ou 0 em caso de erro
     */
    uint32_t createBoss(uint16_t templateID, const Position& position, uint8_t direction);
    
    /**
     * @brief Invoca um mob
     * @param templateID ID do modelo
     * @param position Posição
     * @param direction Direção
     * @param ownerID ID do dono
     * @param duration Duração (0 para permanente)
     * @return ID do mob ou 0 em caso de erro
     */
    uint32_t summonMob(uint16_t templateID, const Position& position, uint8_t direction, uint32_t ownerID, uint32_t duration = 0);
    
    /**
     * @brief Gera o saque de um mob
     * @param mobID ID do mob
     * @param killerID ID do assassino
     * @return Número de itens gerados
     */
    uint32_t generateMobLoot(uint32_t mobID, uint32_t killerID);
    
    /**
     * @brief Concede experiência de um mob
     * @param mobID ID do mob
     * @param killerID ID do assassino
     * @return Quantidade de experiência
     */
    uint32_t grantMobExperience(uint32_t mobID, uint32_t killerID);
    
    /**
     * @brief Obtém um modelo de mob
     * @param templateID ID do modelo
     * @return Modelo ou nullptr se não encontrado
     */
    const MobTemplate* getMobTemplate(uint16_t templateID);
    
    /**
     * @brief Obtém a instância de um mob
     * @param mobID ID do mob
     * @return Instância ou nullptr se não encontrado
     */
    const MobInstance* getMobInstance(uint32_t mobID);
    
    /**
     * @brief Obtém uma área de spawn
     * @param areaID ID da área
     * @return Área ou nullptr se não encontrada
     */
    const SpawnArea* getSpawnArea(uint32_t areaID);
    
    /**
     * @brief Obtém as áreas de spawn em um mapa
     * @param mapID ID do mapa
     * @return Lista de IDs de área
     */
    std::vector<uint32_t> getSpawnAreasInMap(uint16_t mapID);
    
    /**
     * @brief Obtém uma tabela de drop
     * @param tableID ID da tabela
     * @return Tabela ou nullptr se não encontrada
     */
    const DropTable* getDropTable(uint16_t tableID);
    
    /**
     * @brief Obtém a tabela de drop de um mob
     * @param templateID ID do modelo
     * @return Tabela ou nullptr se não encontrada
     */
    const DropTable* getMobDropTable(uint16_t templateID);
    
    /**
     * @brief Verifica se um mob é um chefe
     * @param mobID ID do mob
     * @return true se for um chefe
     */
    bool isMobBoss(uint32_t mobID);
    
    /**
     * @brief Verifica se um mob é um NPC
     * @param mobID ID do mob
     * @return true se for um NPC
     */
    bool isMobNPC(uint32_t mobID);
    
    /**
     * @brief Verifica se um mob está morto
     * @param mobID ID do mob
     * @return true se estiver morto
     */
    bool isMobDead(uint32_t mobID);
    
    /**
     * @brief Verifica se um mob está visível
     * @param mobID ID do mob
     * @return true se estiver visível
     */
    bool isMobVisible(uint32_t mobID);
    
    /**
     * @brief Obtém o estado de um mob
     * @param mobID ID do mob
     * @return Estado
     */
    MobState getMobState(uint32_t mobID);
    
    /**
     * @brief Obtém o tipo de um mob
     * @param mobID ID do mob
     * @return Tipo
     */
    MobType getMobType(uint32_t mobID);
    
    /**
     * @brief Obtém o comportamento de um mob
     * @param mobID ID do mob
     * @return Comportamento
     */
    MobBehavior getMobBehavior(uint32_t mobID);
    
    /**
     * @brief Obtém o nível de um mob
     * @param mobID ID do mob
     * @return Nível
     */
    uint8_t getMobLevel(uint32_t mobID);
    
    /**
     * @brief Obtém o dono de um mob
     * @param mobID ID do mob
     * @return ID do dono ou 0 se não encontrado
     */
    uint32_t getMobOwner(uint32_t mobID);
    
    /**
     * @brief Obtém o alvo de um mob
     * @param mobID ID do mob
     * @return ID do alvo ou 0 se não encontrado
     */
    uint32_t getMobTarget(uint32_t mobID);
    
    /**
     * @brief Obtém a posição de um mob
     * @param mobID ID do mob
     * @return Posição ou posição com MapID=0 se não encontrado
     */
    Position getMobPosition(uint32_t mobID);
    
    /**
     * @brief Obtém a posição inicial de um mob
     * @param mobID ID do mob
     * @return Posição ou posição com MapID=0 se não encontrado
     */
    Position getMobHomePosition(uint32_t mobID);
    
    /**
     * @brief Obtém a direção de um mob
     * @param mobID ID do mob
     * @return Direção ou 0 se não encontrado
     */
    uint8_t getMobDirection(uint32_t mobID);
    
    /**
     * @brief Obtém o tempo de respawn de um mob
     * @param mobID ID do mob
     * @return Tempo ou 0 se não encontrado
     */
    uint32_t getMobRespawnTime(uint32_t mobID);
    
    /**
     * @brief Obtém o tempo de despawn de um mob
     * @param mobID ID do mob
     * @return Tempo ou 0 se não encontrado
     */
    uint32_t getMobDespawnTime(uint32_t mobID);
    
    /**
     * @brief Atualiza o comportamento de um mob
     * @param mobID ID do mob
     * @param deltaTime Tempo decorrido
     * @return true se atualizado com sucesso
     */
    bool updateMobBehavior(uint32_t mobID, uint32_t deltaTime);
    
    /**
     * @brief Registra um callback de morte de mob
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerMobDeathCallback(MobDeathCallback callback);
    
    /**
     * @brief Remove um callback de morte de mob
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterMobDeathCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de spawn de mob
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerMobSpawnCallback(MobSpawnCallback callback);
    
    /**
     * @brief Remove um callback de spawn de mob
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterMobSpawnCallback(uint32_t callbackId);
    
    /**
     * @brief Cria uma área de spawn
     * @param mapID ID do mapa
     * @param minX X mínimo
     * @param minY Y mínimo
     * @param maxX X máximo
     * @param maxY Y máximo
     * @param mobList Lista de mobs
     * @param maxCount Contagem máxima
     * @param respawnTime Tempo de respawn
     * @param despawnTime Tempo de despawn
     * @return ID da área ou 0 em caso de erro
     */
    uint32_t createSpawnArea(uint16_t mapID, uint16_t minX, uint16_t minY, uint16_t maxX, uint16_t maxY, const std::vector<uint16_t>& mobList, uint16_t maxCount, uint32_t respawnTime, uint32_t despawnTime);
    
    /**
     * @brief Remove uma área de spawn
     * @param areaID ID da área
     * @return true se removido com sucesso
     */
    bool removeSpawnArea(uint32_t areaID);
    
    /**
     * @brief Cria uma tabela de drop
     * @param name Nome
     * @param drops Drops
     * @return ID da tabela ou 0 em caso de erro
     */
    uint16_t createDropTable(const std::string& name, const std::vector<ItemDrop>& drops);
    
    /**
     * @brief Remove uma tabela de drop
     * @param tableID ID da tabela
     * @return true se removido com sucesso
     */
    bool removeDropTable(uint16_t tableID);
    
    /**
     * @brief Adiciona um drop a uma tabela
     * @param tableID ID da tabela
     * @param drop Drop
     * @return true se adicionado com sucesso
     */
    bool addDropToTable(uint16_t tableID, const ItemDrop& drop);
    
    /**
     * @brief Remove um drop de uma tabela
     * @param tableID ID da tabela
     * @param itemID ID do item
     * @return true se removido com sucesso
     */
    bool removeDropFromTable(uint16_t tableID, uint16_t itemID);
    
    /**
     * @brief Obtém os mobs de um tipo em uma área
     * @param centerPosition Posição central
     * @param radius Raio
     * @param type Tipo
     * @return Lista de IDs de mob
     */
    std::vector<uint32_t> getMobsOfTypeInArea(const Position& centerPosition, uint32_t radius, MobType type);
    
    /**
     * @brief Obtém os NPCs em uma área
     * @param centerPosition Posição central
     * @param radius Raio
     * @return Lista de IDs de NPC
     */
    std::vector<uint32_t> getNPCsInArea(const Position& centerPosition, uint32_t radius);
    
    /**
     * @brief Obtém os monstros em uma área
     * @param centerPosition Posição central
     * @param radius Raio
     * @return Lista de IDs de monstro
     */
    std::vector<uint32_t> getMonstersInArea(const Position& centerPosition, uint32_t radius);
    
    /**
     * @brief Obtém os pets de um proprietário
     * @param ownerID ID do proprietário
     * @return Lista de IDs de pet
     */
    std::vector<uint32_t> getOwnerPets(uint32_t ownerID);
    
    /**
     * @brief Obtém os summons de um proprietário
     * @param ownerID ID do proprietário
     * @return Lista de IDs de summon
     */
    std::vector<uint32_t> getOwnerSummons(uint32_t ownerID);
    
    /**
     * @brief Obtém os mobs por modelo
     * @param templateID ID do modelo
     * @return Lista de IDs de mob
     */
    std::vector<uint32_t> getMobsByTemplate(uint16_t templateID);
    
    /**
     * @brief Obtém estatísticas de mob
     * @return Estatísticas
     */
    MobStats getMobStats() const;
    
    /**
     * @brief Obtém a configuração de mob
     * @return Configuração
     */
    MobConfig getMobConfig() const;
    
    /**
     * @brief Define a configuração de mob
     * @param config Configuração
     */
    void setMobConfig(const MobConfig& config);
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
private:
    /**
     * @brief Construtor privado
     */
    MobManager();
    
    /**
     * @brief Destrutor privado
     */
    ~MobManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    MobManager(const MobManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    MobManager& operator=(const MobManager&) = delete;
    
    // Gerenciador de mapas
    MapManager* _mapManager;
    
    // Modelos de mob
    std::unordered_map<uint16_t, MobTemplate> _mobTemplates;
    
    // Instâncias de mob
    std::unordered_map<uint32_t, MobInstance> _mobInstances;
    std::atomic<uint32_t> _nextMobID;
    
    // Áreas de spawn
    std::unordered_map<uint32_t, SpawnArea> _spawnAreas;
    std::unordered_map<uint16_t, std::vector<uint32_t>> _mapSpawnAreas;
    std::atomic<uint32_t> _nextSpawnAreaID;
    
    // Tabelas de drop
    std::unordered_map<uint16_t, DropTable> _dropTables;
    std::atomic<uint16_t> _nextDropTableID;
    
    // Mobs por tipo
    std::unordered_map<MobType, std::vector<uint32_t>> _mobsByType;
    
    // Mobs por modelo
    std::unordered_map<uint16_t, std::vector<uint32_t>> _mobsByTemplate;
    
    // Mobs por proprietário
    std::unordered_map<uint32_t, std::vector<uint32_t>> _mobsByOwner;
    
    // Configuração
    MobConfig _mobConfig;
    
    // Estatísticas
    MobStats _mobStats;
    
    // Callbacks
    std::vector<MobDeathCallback> _mobDeathCallbacks;
    std::vector<MobSpawnCallback> _mobSpawnCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _templateMutex;
    mutable std::mutex _instanceMutex;
    mutable std::mutex _spawnAreaMutex;
    mutable std::mutex _dropTableMutex;
    mutable std::mutex _indexMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Geradores aleatórios
    std::mt19937 _rng;
    
    // Timers
    uint32_t _updateTimer;
    uint32_t _respawnTimer;
    uint32_t _despawnTimer;
    
    /**
     * @brief Checa mobs para respawn
     * @param currentTime Tempo atual
     */
    void checkMobsForRespawn(uint32_t currentTime);
    
    /**
     * @brief Checa mobs para despawn
     * @param currentTime Tempo atual
     */
    void checkMobsForDespawn(uint32_t currentTime);
    
    /**
     * @brief Atualiza comportamentos de mobs
     * @param deltaTime Tempo decorrido
     */
    void updateMobBehaviors(uint32_t deltaTime);
    
    /**
     * @brief Atualiza estatísticas
     */
    void updateMobStats();
    
    /**
     * @brief Executa callbacks de morte de mob
     * @param mobID ID do mob
     * @param killerID ID do assassino
     */
    void executeMobDeathCallbacks(uint32_t mobID, uint32_t killerID);
    
    /**
     * @brief Executa callbacks de spawn de mob
     * @param mobID ID do mob
     * @param position Posição
     */
    void executeMobSpawnCallbacks(uint32_t mobID, const Position& position);
    
    /**
     * @brief Inicializa um mob
     * @param mobInstance Instância do mob
     * @param templateID ID do modelo
     * @param position Posição
     * @param direction Direção
     * @param ownerID ID do dono
     * @param spawnAreaID ID da área de spawn
     * @return true se inicializado com sucesso
     */
    bool initializeMob(MobInstance& mobInstance, uint16_t templateID, const Position& position, uint8_t direction, uint32_t ownerID, uint32_t spawnAreaID);
    
    /**
     * @brief Carrega o modelo de mob do arquivo
     * @param templateID ID do modelo
     * @param filename Nome do arquivo
     * @return true se carregado com sucesso
     */
    bool loadMobTemplateFromFile(uint16_t templateID, const std::string& filename);
    
    /**
     * @brief Carrega a área de spawn do arquivo
     * @param areaID ID da área
     * @param filename Nome do arquivo
     * @return true se carregado com sucesso
     */
    bool loadSpawnAreaFromFile(uint32_t areaID, const std::string& filename);
    
    /**
     * @brief Carrega a tabela de drop do arquivo
     * @param tableID ID da tabela
     * @param filename Nome do arquivo
     * @return true se carregado com sucesso
     */
    bool loadDropTableFromFile(uint16_t tableID, const std::string& filename);
    
    /**
     * @brief Calcula o dano de um mob
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @return Dano
     */
    uint32_t calculateMobDamage(uint32_t mobID, uint32_t targetID);
    
    /**
     * @brief Verifica se um mob pode ver um alvo
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @return true se puder ver
     */
    bool canMobSeeTarget(uint32_t mobID, uint32_t targetID);
    
    /**
     * @brief Verifica se um mob pode atacar um alvo
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @return true se puder atacar
     */
    bool canMobAttackTarget(uint32_t mobID, uint32_t targetID);
    
    /**
     * @brief Obtém uma posição de spawn aleatória em uma área
     * @param areaID ID da área
     * @return Posição ou posição com MapID=0 se não encontrada
     */
    Position getRandomSpawnPosition(uint32_t areaID);
    
    /**
     * @brief Respawna um mob
     * @param mobID ID do mob
     * @return Resultado da operação
     */
    MobResult respawnMob(uint32_t mobID);
    
    /**
     * @brief Atualiza a lista de aggro de um mob
     * @param mobID ID do mob
     * @return true se atualizado com sucesso
     */
    bool updateMobAggroList(uint32_t mobID);
    
    /**
     * @brief Atualiza os índices de um mob
     * @param mobID ID do mob
     * @param oldType Tipo antigo
     * @param newType Tipo novo
     * @param oldTemplateID ID do modelo antigo
     * @param newTemplateID ID do modelo novo
     * @param oldOwnerID ID do dono antigo
     * @param newOwnerID ID do dono novo
     */
    void updateMobIndices(uint32_t mobID, MobType oldType, MobType newType, uint16_t oldTemplateID, uint16_t newTemplateID, uint32_t oldOwnerID, uint32_t newOwnerID);
    
    /**
     * @brief Rola um item de uma tabela de drop
     * @param tableID ID da tabela
     * @param level Nível
     * @return ID do item ou 0 se não encontrado
     */
    uint16_t rollDropTableItem(uint16_t tableID, uint8_t level);
    
    /**
     * @brief Obtém a chance de drop de um item
     * @param drop Drop
     * @param level Nível
     * @return Chance
     */
    float getDropChance(const ItemDrop& drop, uint8_t level);
};

} // namespace world
} // namespace wydbr

#endif // _MOB_MANAGER_H_