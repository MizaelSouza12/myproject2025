/**
 * WorldTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/WorldTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WORLDTYPES_H
#define WORLDTYPES_H

#include <string>
#include <vector>
#include <map>
#include <array>
#include <functional>
#include <memory>
#include <chrono>

#include "../../../core/WYDTypes.h"
#include "../item/ItemTypes.h"
#include "../battle/BattleTypes.h"

namespace wydbr {


/**
 * @file WorldTypes.h
 * @brief Definições de tipos para o sistema de mundo do WYD
 * 
 * Este arquivo contém as definições de tipos para o sistema de mundo
 * do WYD, incluindo entidades, geradores, temporizadores, etc.
 * No WYD original, estes tipos estavam espalhados por vários arquivos
 * e muitas vezes tinham definições inconsistentes. Esta versão centraliza
 * e padroniza essas definições.
 */

namespace wyd {
namespace server {
namespace world {

// Constantes
#define MAX_WORLD_RANGE         4096    // Tamanho máximo do mundo em cada dimensão
#define MAX_SPAWN_POINTS        10000   // Número máximo de pontos de spawn no mundo
#define MAX_NPCS_PER_ZONE       5000    // Número máximo de NPCs por zona
#define MAX_WATER_AREAS         512     // Número máximo de áreas de água no mundo
#define MAX_FARM_AREAS          100     // Número máximo de áreas de coleta no mundo
#define MAX_EVENT_AREAS         50      // Número máximo de áreas de evento no mundo
#define MAX_DYNAMIC_EVENTS      20      // Número máximo de eventos dinâmicos simultâneos
#define MAX_PATH_NODES          100     // Número máximo de nós de caminho para pathing de NPCs
#define MAX_RESPAWN_TIME        600000  // Tempo máximo de respawn em ms (10 minutos)
#define MIN_RESPAWN_TIME        1000    // Tempo mínimo de respawn em ms (1 segundo)
#define BASE_MOB_ID             10000   // ID base para mobs (mobs têm IDs acima deste valor)
#define MAX_LEVEL               400     // Nível máximo
#define DEFAULT_SIGHT_RANGE     16      // Alcance de visão padrão em tiles
#define DEFAULT_ATTACK_RANGE    2       // Alcance de ataque padrão em tiles

// Define códigos de área para coordenadas de tile
constexpr /**
 * MakeAreaCode
 * 
 * Implementa a funcionalidade MakeAreaCode conforme especificação original.
 * @param byLevel Parâmetro byLevel
 * @param wX Parâmetro wX
 * @param wY Parâmetro wY
 * @return Retorna DWORD
 */
 DWORD MakeAreaCode(BYTE byLevel, WORD wX, WORD wY){
    return (static_cast<DWORD>(byLevel) << 24) | (static_cast<DWORD>(wX) << 12) | wY;
}

constexpr /**
 * GetAreaLevel
 * 
 * Implementa a funcionalidade GetAreaLevel conforme especificação original.
 * @param dwAreaCode Parâmetro dwAreaCode
 * @return Retorna BYTE
 */
 BYTE GetAreaLevel(DWORD dwAreaCode){
    return static_cast<BYTE>((dwAreaCode >> 24) & 0xFF);
}

constexpr /**
 * GetAreaX
 * 
 * Implementa a funcionalidade GetAreaX conforme especificação original.
 * @param dwAreaCode Parâmetro dwAreaCode
 * @return Retorna WORD
 */
 WORD GetAreaX(DWORD dwAreaCode){
    return static_cast<WORD>((dwAreaCode >> 12) & 0xFFF);
}

constexpr /**
 * GetAreaY
 * 
 * Implementa a funcionalidade GetAreaY conforme especificação original.
 * @param dwAreaCode Parâmetro dwAreaCode
 * @return Retorna WORD
 */
 WORD GetAreaY(DWORD dwAreaCode){
    return static_cast<WORD>(dwAreaCode & 0xFFF);
}

/**
 * @brief Tipos de entidade
 */
enum class EntityType : BYTE {
    NONE = 0,           // Nenhum
    PLAYER = 1,         // Jogador
    MOB = 2,            // Mob/monstro
    NPC = 3,            // NPC
    SUMMON = 4,         // Criatura invocada
    COLLECT = 5,        // Item colecionável
    TRIGGER = 6,        // Gatilho
    PROJECTILE = 7,     // Projétil
    OBJECT = 8,         // Objeto interativo
    EFFECT = 9,         // Efeito visual
    EVENT = 10,         // Entidade de evento
    MERCHANT = 11,      // Comerciante
    BOSS = 12,          // Chefe
    BARRIER = 13,       // Barreira
    CUSTOM = 14,        // Customizado
};

/**
 * @brief Tipos de movimento
 */
enum class MovementType : BYTE {
    NONE = 0,           // Nenhum movimento
    RANDOM = 1,         // Movimento aleatório
    PATROL = 2,         // Patrulha predefinida
    FOLLOW = 3,         // Seguir alvo
    FLEE = 4,           // Fugir de alvo
    SCRIPT = 5,         // Controlado por script
    CONTROLLED = 6,     // Controlado externamente
    STATIC = 7,         // Estático
    PATH = 8,           // Seguir caminho
    SWARM = 9,          // Movimento em bando
    CHASE = 10,         // Perseguir alvo agressivamente
    FLY = 11,           // Voar (3D)
    SWIM = 12,          // Nadar
    CLIMB = 13,         // Escalar
};

/**
 * @brief Tipos de agressividade
 */
enum class AggroType : BYTE {
    PASSIVE = 0,        // Não ataca
    DEFENSIVE = 1,      // Ataca se atacado
    AGGRESSIVE = 2,     // Ataca quando detecta
    FRENZIED = 3,       // Ataca qualquer coisa
    TERRITORIAL = 4,    // Ataca se invadir território
    NEUTRAL = 5,        // Nunca ataca
    SCRIPTED = 6,       // Controlado por script
    ASSIST = 7,         // Ajuda aliados
    SELECTIVE = 8,      // Ataca somente certos alvos
};

/**
 * @brief Tipos de loot
 */
enum class LootType : BYTE {
    NONE = 0,           // Nenhum loot
    INDIVIDUAL = 1,     // Loot individual
    GROUP = 2,          // Loot de grupo
    RAID = 3,           // Loot de raid
    LAST_HIT = 4,       // Loot para último hit
    MOST_DAMAGE = 5,    // Loot para maior dano
    ALL = 6,            // Loot para todos
    SCRIPTED = 7,       // Controlado por script
};

/**
 * @brief Tipos de spawn
 */
enum class SpawnType : BYTE {
    NONE = 0,           // Nenhum
    FIXED = 1,          // Posição fixa
    RANDOM_AREA = 2,    // Área aleatória
    SCRIPT = 3,         // Controlado por script
    EVENT = 4,          // Gerado por evento
    TRIGGER = 5,        // Gerado por gatilho
    SCHEDULED = 6,      // Programado
    CONDITIONAL = 7,    // Condicionado
    BOSS = 8,           // Chefe (com anúncio)
    INVASION = 9,       // Invasão
    DYNAMIC = 10,       // Dinâmico (baseado em jogadores)
};

/**
 * @brief Raças de mobs
 */
enum class MobRace : BYTE {
    NONE = 0,           // Nenhuma
    BEAST = 1,          // Besta
    UNDEAD = 2,         // Morto-vivo
    DEMON = 3,          // Demônio
    HUMANOID = 4,       // Humanóide
    ELEMENTAL = 5,      // Elemental
    INSECT = 6,         // Inseto
    PLANT = 7,          // Planta
    CONSTRUCT = 8,      // Construto
    DRAGON = 9,         // Dragão
    ABERRATION = 10,    // Aberração
    SPIRIT = 11,        // Espírito
    MAGICAL = 12,       // Mágico
    ORC = 13,           // Orc
    GIANT = 14,         // Gigante
    CUSTOM = 15,        // Customizado
};

/**
 * @brief Classes de mobs
 */
enum class MobClass : BYTE {
    NONE = 0,           // Nenhuma
    MELEE = 1,          // Corpo a corpo
    RANGED = 2,         // À distância
    MAGIC = 3,          // Mágico
    STEALTH = 4,        // Furtivo
    TANK = 5,           // Tanque
    HEALER = 6,         // Curador
    SUPPORT = 7,        // Suporte
    BOSS = 8,           // Chefe
    MINION = 9,         // Lacaio
    ELITE = 10,         // Elite
    SWARM = 11,         // Enxame
    CUSTOM = 12,        // Customizado
};

/**
 * @brief Tipos de drop
 */
enum class DropType : BYTE {
    NONE = 0,           // Nenhum
    ITEM = 1,           // Item
    GOLD = 2,           // Ouro
    QUEST = 3,          // Item de quest
    TOKEN = 4,          // Token/ficha
    RESOURCE = 5,       // Recurso
    EQUIP = 6,          // Equipamento
    CONSUMABLE = 7,     // Consumível
    KEY = 8,            // Chave
    CRAFTING = 9,       // Item de craft
    UNIQUE = 10,        // Item único
    CUSTOM = 11,        // Customizado
};

/**
 * @brief Configuração de um grupo de respawn
 */
struct SpawnGroupConfig {
    WORD wGroupID;                  // ID do grupo
    BYTE byLevel;                   // Nível do mapa
    WORD wMapID;                    // ID do mapa
    WORD wBaseX;                    // Coordenada X base
    WORD wBaseY;                    // Coordenada Y base
    WORD wRadius;                   // Raio do grupo
    WORD wMinCount;                 // Contagem mínima
    WORD wMaxCount;                 // Contagem máxima
    DWORD dwRespawnTimeMin;         // Tempo mínimo de respawn (ms)
    DWORD dwRespawnTimeMax;         // Tempo máximo de respawn (ms)
    DWORD dwDespawnTime;            // Tempo para desaparecer (ms)
    BYTE byDayTimeSpawn;            // Spawna durante o dia (1/0)
    BYTE byNightTimeSpawn;          // Spawna durante a noite (1/0)
    SpawnType eSpawnType;           // Tipo de spawn
    std::vector<WORD> vecMobIDs;    // IDs dos mobs a spawnar
    std::vector<BYTE> vecWeights;   // Pesos de cada mob
    std::string szComment;          // Comentário
    
    SpawnGroupConfig()
        : wGroupID(0)
        , byLevel(0)
        , wMapID(0)
        , wBaseX(0)
        , wBaseY(0)
        , wRadius(0)
        , wMinCount(0)
        , wMaxCount(0)
        , dwRespawnTimeMin(MIN_RESPAWN_TIME)
        , dwRespawnTimeMax(MAX_RESPAWN_TIME)
        , dwDespawnTime(0)
        , byDayTimeSpawn(1)
        , byNightTimeSpawn(1)
        , eSpawnType(SpawnType::FIXED)
    {
    }
};

/**
 * @brief Configuração de drop
 */
struct DropConfig {
    WORD wDropID;                   // ID do drop
    DropType eDropType;             // Tipo de drop
    WORD wItemID;                   // ID do item (se for item)
    DWORD dwMinAmount;              // Quantidade mínima
    DWORD dwMaxAmount;              // Quantidade máxima
    float fChance;                  // Chance de drop (0.0 - 100.0)
    BYTE byMinLevel;                // Nível mínimo do jogador
    BYTE byMaxLevel;                // Nível máximo do jogador
    BYTE byQuestRequired;           // Requer quest específica (1/0)
    WORD wQuestID;                  // ID da quest
    BYTE byClassRestricted;         // Restrito a certas classes (1/0)
    BYTE byClassMask;               // Máscara de classes
    BYTE byRaceRestricted;          // Restrito a certas raças (1/0)
    BYTE byRaceMask;                // Máscara de raças
    BYTE byUnique;                  // Uniqued rop (1/0)
    BYTE byMinGrade;                // Grau mínimo
    BYTE byMaxGrade;                // Grau máximo
    
    DropConfig()
        : wDropID(0)
        , eDropType(DropType::NONE)
        , wItemID(0)
        , dwMinAmount(1)
        , dwMaxAmount(1)
        , fChance(0.0f)
        , byMinLevel(0)
        , byMaxLevel(MAX_LEVEL)
        , byQuestRequired(0)
        , wQuestID(0)
        , byClassRestricted(0)
        , byClassMask(0)
        , byRaceRestricted(0)
        , byRaceMask(0)
        , byUnique(0)
        , byMinGrade(0)
        , byMaxGrade(0)
    {
    }
};

/**
 * @brief Configuração de um ponto de spawn
 */
struct SpawnPoint {
    DWORD dwPointID;                // ID do ponto
    WORD wMapID;                    // ID do mapa
    WORD wPosX;                     // Posição X
    WORD wPosY;                     // Posição Y
    BYTE byLevel;                   // Nível do mapa
    WORD wGroupID;                  // ID do grupo (0 se for independente)
    BYTE byDirection;               // Direção inicial
    BYTE byFlags;                   // Flags adicionais
    
    SpawnPoint()
        : dwPointID(0)
        , wMapID(0)
        , wPosX(0)
        , wPosY(0)
        , byLevel(0)
        , wGroupID(0)
        , byDirection(0)
        , byFlags(0)
    {
    }
};

/**
 * @brief Configuração de uma tabela de drops
 */
struct DropTable {
    WORD wTableID;                  // ID da tabela
    std::vector<DropConfig> vecDrops; // Lista de drops
    
    DropTable()
        /**
 * wTableID
 * 
 * Implementa a funcionalidade wTableID conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */

        : wTableID(0){
    }
};

/**
 * @brief Configuração de um template de mob
 */
struct MobTemplate {
    WORD wMobID;                    // ID do mob
    std::string szName;             // Nome
    std::string szSubtitle;         // Subtítulo
    MobRace eRace;                  // Raça
    MobClass eClass;                // Classe
    BYTE byLevel;                   // Nível
    BYTE byRegenHP;                 // Regeneração de HP por tick
    BYTE byRegenMP;                 // Regeneração de MP por tick
    BYTE bySize;                    // Tamanho (em tiles)
    WORD wSightRange;               // Alcance de visão
    WORD wAttackRange;              // Alcance de ataque
    DWORD dwMaxHP;                  // HP máximo
    DWORD dwMaxMP;                  // MP máximo
    WORD wAttackPower;              // Poder de ataque
    WORD wMagicPower;               // Poder mágico
    WORD wDefense;                  // Defesa
    WORD wResistance;               // Resistência
    BYTE byStr;                     // Força
    BYTE byDex;                     // Destreza
    BYTE byInt;                     // Inteligência
    BYTE byCon;                     // Constituição
    WORD wMoveSpeed;                // Velocidade de movimento
    WORD wAttackSpeed;              // Velocidade de ataque
    BYTE byMagicLevel;              // Nível de magia
    float fEvasion;                 // Taxa de evasão
    float fAccuracy;                // Taxa de acerto
    float fCritical;                // Taxa de crítico
    WORD wDropTableID;              // ID da tabela de drops
    DWORD dwExperience;             // Experiência
    DWORD dwGold;                   // Ouro
    MovementType eMoveType;         // Tipo de movimento
    AggroType eAggroType;           // Tipo de agressividade
    LootType eLootType;             // Tipo de loot
    WORD wModelID;                  // ID do modelo
    WORD wSkillID1;                 // ID da skill 1
    WORD wSkillID2;                 // ID da skill 2
    WORD wSkillID3;                 // ID da skill 3
    WORD wSkillID4;                 // ID da skill 4
    BYTE byResistFire;              // Resistência ao fogo
    BYTE byResistIce;               // Resistência ao gelo
    BYTE byResistLightning;         // Resistência ao raio
    BYTE byResistPoison;            // Resistência ao veneno
    BYTE byResistHoly;              // Resistência ao sagrado
    BYTE byResistDark;              // Resistência à trevas
    BYTE byIsElite;                 // É um elite (1/0)
    BYTE byIsBoss;                  // É um chefe (1/0)
    BYTE byIsAggressive;            // É agressivo (1/0)
    BYTE byNoBarding;               // Não aceita montar (1/0)
    BYTE byNoReflect;               // Não reflete dano (1/0)
    BYTE byNoTaming;                // Não pode ser domado (1/0)
    BYTE byNoDungeon;               // Não aparece em masmorras (1/0)
    BYTE byNoKnockback;             // Não sofre knockback (1/0)
    WORD wFamilyID;                 // ID da família
    WORD wSummonID;                 // ID do summon
    BYTE byElement;                 // Elemento
    BYTE byElementPower;            // Poder elemental
    BYTE byRunOnLowHealth;          // Foge com HP baixo (1/0)
    BYTE byCallForHelp;             // Chama por ajuda (1/0)
    BYTE bySummonOnDeath;           // Invoca ao morrer (1/0)
    BYTE byNoExp;                   // Não dá experiência (1/0)
    BYTE byNoItem;                  // Não dropa itens (1/0)
    BYTE byRespawnGroup;            // Respawna em grupo (1/0)
    BYTE byMoveArea;                // Área de movimento
    std::string szAI;               // Script de IA
    
    MobTemplate()
        : wMobID(0)
        , szName("Mob")
        , szSubtitle("")
        , eRace(MobRace::NONE)
        , eClass(MobClass::NONE)
        , byLevel(1)
        , byRegenHP(0)
        , byRegenMP(0)
        , bySize(1)
        , wSightRange(DEFAULT_SIGHT_RANGE)
        , wAttackRange(DEFAULT_ATTACK_RANGE)
        , dwMaxHP(100)
        , dwMaxMP(0)
        , wAttackPower(10)
        , wMagicPower(0)
        , wDefense(0)
        , wResistance(0)
        , byStr(10)
        , byDex(10)
        , byInt(10)
        , byCon(10)
        , wMoveSpeed(5)
        , wAttackSpeed(10)
        , byMagicLevel(0)
        , fEvasion(0.0f)
        , fAccuracy(80.0f)
        , fCritical(5.0f)
        , wDropTableID(0)
        , dwExperience(10)
        , dwGold(0)
        , eMoveType(MovementType::RANDOM)
        , eAggroType(AggroType::PASSIVE)
        , eLootType(LootType::INDIVIDUAL)
        , wModelID(0)
        , wSkillID1(0)
        , wSkillID2(0)
        , wSkillID3(0)
        , wSkillID4(0)
        , byResistFire(0)
        , byResistIce(0)
        , byResistLightning(0)
        , byResistPoison(0)
        , byResistHoly(0)
        , byResistDark(0)
        , byIsElite(0)
        , byIsBoss(0)
        , byIsAggressive(0)
        , byNoBarding(0)
        , byNoReflect(0)
        , byNoTaming(0)
        , byNoDungeon(0)
        , byNoKnockback(0)
        , wFamilyID(0)
        , wSummonID(0)
        , byElement(0)
        , byElementPower(0)
        , byRunOnLowHealth(0)
        , byCallForHelp(0)
        , bySummonOnDeath(0)
        , byNoExp(0)
        , byNoItem(0)
        , byRespawnGroup(0)
        , byMoveArea(0)
        , szAI("")
    {
    }
};

/**
 * @brief Status de um spawner
 */
enum class SpawnerStatus : BYTE {
    INACTIVE = 0,       // Inativo
    ACTIVE = 1,         // Ativo
    SPAWNING = 2,       // Gerando
    PAUSED = 3,         // Pausado
    WAITING = 4,        // Aguardando
    ERROR = 5,          // Erro
};

/**
 * @brief Status de uma entidade gerada
 */
enum class SpawnedEntityStatus : BYTE {
    NONE = 0,           // Nenhum
    SPAWNING = 1,       // Gerando
    ALIVE = 2,          // Vivo
    DEAD = 3,           // Morto
    DESPAWNING = 4,     // Desaparecendo
    RESPAWNING = 5,     // Reaparecendo
    WAITING = 6,        // Aguardando
};

/**
 * @brief Eventos de entidade
 */
enum class EntityEvent : BYTE {
    NONE = 0,           // Nenhum
    SPAWN = 1,          // Spawn
    DEATH = 2,          // Morte
    RESPAWN = 3,        // Respawn
    DESPAWN = 4,        // Despawn
    DAMAGE = 5,         // Dano
    HEAL = 6,           // Cura
    ATTACK = 7,         // Ataque
    SKILL = 8,          // Uso de skill
    MOVE = 9,           // Movimento
    AGGRO = 10,         // Agressão
    EVADE = 11,         // Evasão
    BLOCK = 12,         // Bloqueio
    CRITICAL = 13,      // Crítico
    DEBUFF = 14,        // Debuff
    BUFF = 15,          // Buff
    STUN = 16,          // Stun
    ROOT = 17,          // Root
    SILENCE = 18,       // Silenciado
    BLIND = 19,         // Cego
    FEAR = 20,          // Medo
    CUSTOM = 21,        // Customizado
};

/**
 * @brief Callback para eventos de entidade
 */
using EntityEventCallback = std::function<void(EntityEvent, DWORD, const void*)>;

/**
 * @brief Status de um grupo de spawn
 */
struct SpawnGroupStatus {
    WORD wGroupID;                  // ID do grupo
    WORD wCurrentCount;             // Contagem atual
    WORD wMaxCount;                 // Contagem máxima
    SpawnerStatus eStatus;          // Status
    DWORD dwLastSpawnTime;          // Timestamp do último spawn
    std::vector<DWORD> vecEntityIDs; // IDs das entidades geradas
    
    SpawnGroupStatus()
        : wGroupID(0)
        , wCurrentCount(0)
        , wMaxCount(0)
        , eStatus(SpawnerStatus::INACTIVE)
        , dwLastSpawnTime(0)
    {
    }
};

/**
 * @brief Status de evento de mundo
 */
struct WorldEventStatus {
    WORD wEventID;                  // ID do evento
    std::string szName;             // Nome
    bool bActive;                   // Ativo
    DWORD dwStartTime;              // Timestamp de início
    DWORD dwEndTime;                // Timestamp de fim
    BYTE byEventType;               // Tipo de evento
    WORD wMapID;                    // ID do mapa
    WORD wPosX;                     // Posição X
    WORD wPosY;                     // Posição Y
    WORD wRadius;                   // Raio
    std::vector<DWORD> vecEntities; // Entidades relacionadas
    
    WorldEventStatus()
        : wEventID(0)
        , szName("")
        , bActive(false)
        , dwStartTime(0)
        , dwEndTime(0)
        , byEventType(0)
        , wMapID(0)
        , wPosX(0)
        , wPosY(0)
        , wRadius(0)
    {
    }
};

// Final de namespace
} // namespace world
} // namespace server
} // namespace wyd

#endif // WORLDTYPES_H

} // namespace wydbr
