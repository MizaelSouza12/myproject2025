/**
 * QuestTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/QuestTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUESTTYPES_H
#define QUESTTYPES_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <array>
#include <bitset>

#include "../../../core/WYDTypes.h"
#include "../item/ItemTypes.h"
#include "../battle/BattleTypes.h"
#include "../world/WorldTypes.h"

namespace wydbr {


/**
 * @file QuestTypes.h
 * @brief Definições de tipos para o sistema de quests
 * 
 * Este arquivo contém as definições de tipos para o sistema avançado de quests,
 * que corrige várias limitações e bugs do sistema original do WYD.
 * O sistema original tinha quests extremamente limitadas, com pouca interatividade
 * e diversidade. Esta implementação oferece um framework flexível e poderoso.
 */

namespace wyd {
namespace server {
namespace quest {

// Constantes de sistema
constexpr int MAX_QUESTS_PER_PLAYER = 50;       // Máximo de quests por jogador
constexpr int MAX_ACTIVE_QUESTS = 20;           // Máximo de quests ativas simultaneamente
constexpr int MAX_COMPLETED_QUESTS = 1000;      // Máximo de quests completadas no histórico
constexpr int MAX_QUEST_OBJECTIVES = 10;        // Máximo de objetivos por quest
constexpr int MAX_QUEST_REWARDS = 10;           // Máximo de recompensas por quest
constexpr int MAX_QUEST_PREREQS = 5;            // Máximo de pré-requisitos por quest
constexpr int MAX_QUEST_BRANCHES = 10;          // Máximo de ramificações por quest
constexpr int MAX_DIALOGUE_OPTIONS = 8;         // Máximo de opções de diálogo
constexpr int MAX_QUEST_STATES = 20;            // Máximo de estados diferentes em uma quest
constexpr int MAX_QUEST_SCRIPT_SIZE = 8192;     // Tamanho máximo de script de quest
constexpr int MAX_QUEST_CHAINS = 100;           // Máximo de cadeias de quests
constexpr int MAX_QUESTS_PER_CHAIN = 20;        // Máximo de quests por cadeia
constexpr int MAX_DAILY_QUESTS = 15;            // Máximo de quests diárias
constexpr int MAX_WEEKLY_QUESTS = 10;           // Máximo de quests semanais

/**
 * @brief ID de Quest
 */
struct QuestID {
    WORD id;                        // Identificador único
    
    QuestID(WORD _id = 0) /**
 * id
 * 
 * Implementa a funcionalidade id conforme especificação original.
 * @param _id Parâmetro _id
 * @return Retorna :
 */
 : id(_id){}
    
    bool operator==(const QuestID& other) const {
        return id == other.id;
    }
    
    bool operator!=(const QuestID& other) const {
        return id != other.id;
    }
    
    bool operator<(const QuestID& other) const {
        return id < other.id;
    }
    
    /**
 * WORD
 * 
 * Implementa a funcionalidade WORD conforme especificação original.
 * @return Retorna operator
 */

    
    operator WORD() const{
        return id;
    }
};

/**
 * @brief Tipo de Quest
 */
enum class QuestType : BYTE {
    NONE = 0,               // Nenhum tipo
    MAIN = 1,               // Quest principal (história)
    SIDE = 2,               // Quest secundária
    DAILY = 3,              // Quest diária
    WEEKLY = 4,             // Quest semanal
    ACHIEVEMENT = 5,        // Conquista
    EVENT = 6,              // Evento especial
    GUILD = 7,              // Quest de guild
    CLASS = 8,              // Quest de classe
    PROFESSION = 9,         // Quest de profissão
    REPEATABLE = 10,        // Quest repetível
    HIDDEN = 11,            // Quest escondida
    WORLD = 12,             // Quest de mundo (todos participam)
    TIMED = 13,             // Quest com tempo limitado
    DUNGEON = 14,           // Quest de masmorra
    RAID = 15,              // Quest de raid
    PVP = 16,               // Quest de PvP
    SPECIAL = 17,           // Quest especial
    CHAIN = 18,             // Parte de uma cadeia de quests
    COLLECTION = 19,        // Quest de coleção
    ESCORT = 20,            // Quest de escolta
    DELIVERY = 21,          // Quest de entrega
    CRAFTING = 22,          // Quest de crafting
    EXPLORATION = 23,       // Quest de exploração
    BOSS = 24,              // Quest de chefe
    HUNTING = 25,           // Quest de caça
    CUSTOM = 26,            // Quest customizada
};

/**
 * @brief Categoria de Quest
 */
enum class QuestCategory : BYTE {
    NONE = 0,               // Nenhuma categoria
    STORY = 1,              // História principal
    COMBAT = 2,             // Combate
    EXPLORATION = 3,        // Exploração
    COLLECTION = 4,         // Coleção
    CRAFTING = 5,           // Crafting
    REPUTATION = 6,         // Reputação
    PVP = 7,                // PvP
    DUNGEON = 8,            // Masmorras
    RAID = 9,               // Raids
    EVENT = 10,             // Eventos
    GUILD = 11,             // Guild
    ACHIEVEMENT = 12,       // Conquistas
    PROFESSION = 13,        // Profissões
    CLASS = 14,             // Classe
    DAILY = 15,             // Diárias
    WEEKLY = 16,            // Semanais
    SOCIAL = 17,            // Social
    WORLD = 18,             // Mundo
    SPECIAL = 19,           // Especial
    CUSTOM = 20,            // Customizada
};

/**
 * @brief Status de Quest
 */
enum class QuestStatus : BYTE {
    UNAVAILABLE = 0,        // Indisponível
    AVAILABLE = 1,          // Disponível
    IN_PROGRESS = 2,        // Em progresso
    COMPLETED = 3,          // Completada
    FAILED = 4,             // Falhou
    ABANDONED = 5,          // Abandonada
    ON_HOLD = 6,            // Em espera
    REPEATABLE = 7,         // Repetível
    LOCKED = 8,             // Bloqueada
    EXPIRED = 9,            // Expirada
    HIDDEN = 10,            // Escondida
    COMPLETED_PENDING = 11, // Completa mas pendente de entrega
    CUSTOM = 12,            // Status customizado
};

/**
 * @brief Tipo de Objetivo de Quest
 */
enum class QuestObjectiveType : BYTE {
    NONE = 0,               // Nenhum
    KILL = 1,               // Matar monstros
    COLLECT = 2,            // Coletar itens
    INTERACT = 3,           // Interagir com objeto
    TALK = 4,               // Falar com NPC
    DELIVER = 5,            // Entregar item
    ESCORT = 6,             // Escoltar NPC
    EXPLORE = 7,            // Explorar área
    CRAFT = 8,              // Craftar item
    USE_SKILL = 9,          // Usar skill
    COMPLETE_QUEST = 10,    // Completar outra quest
    REACH_LEVEL = 11,       // Atingir nível
    REACH_REPUTATION = 12,  // Atingir reputação
    WIN_PVP = 13,           // Vencer PvP
    COMPLETE_DUNGEON = 14,  // Completar masmorra
    COMPLETE_RAID = 15,     // Completar raid
    DEFEAT_BOSS = 16,       // Derrotar chefe
    SURVIVE_TIME = 17,      // Sobreviver por tempo
    ACHIEVE_COMBO = 18,     // Atingir combo
    EQUIP_ITEM = 19,        // Equipar item
    LEARN_SKILL = 20,       // Aprender skill
    JOIN_GUILD = 21,        // Entrar em guild
    SPEND_CURRENCY = 22,    // Gastar moeda
    GATHER_RESOURCE = 23,   // Coletar recurso
    FISH = 24,              // Pescar
    MINE = 25,              // Minerar
    HERB = 26,              // Coletar ervas
    TAME_CREATURE = 27,     // Domar criatura
    CUSTOM = 28,            // Customizado
};

/**
 * @brief Tipo de Recompensa de Quest
 */
enum class QuestRewardType : BYTE {
    NONE = 0,               // Nenhuma
    ITEM = 1,               // Item
    GOLD = 2,               // Ouro
    EXPERIENCE = 3,         // Experiência
    REPUTATION = 4,         // Reputação
    SKILL = 5,              // Skill
    TITLE = 6,              // Título
    CURRENCY = 7,           // Moeda especial
    TELEPORT = 8,           // Teleporte
    BUFF = 9,               // Buff temporário
    UNLOCKABLE = 10,        // Desbloqueio
    MOUNT = 11,             // Montaria
    PET = 12,               // Pet
    ATTRIBUTE_POINT = 13,   // Ponto de atributo
    SKILL_POINT = 14,       // Ponto de skill
    INVENTORY_SLOT = 15,    // Slot de inventário
    STORAGE_SLOT = 16,      // Slot de armazenamento
    GUILD_POINTS = 17,      // Pontos de guild
    CUSTOM = 18,            // Customizado
};

/**
 * @brief Tipo de Pré-requisito de Quest
 */
enum class QuestPrereqType : BYTE {
    NONE = 0,               // Nenhum
    QUEST_COMPLETED = 1,    // Quest completada
    QUEST_ACTIVE = 2,       // Quest ativa
    LEVEL = 3,              // Nível mínimo
    ITEM_OWNED = 4,         // Possui item
    REPUTATION = 5,         // Reputação mínima
    CLASS = 6,              // Classe específica
    RACE = 7,               // Raça específica
    SKILL_LEVEL = 8,        // Nível de skill
    GUILD_MEMBER = 9,       // Membro de guild
    GUILD_RANK = 10,        // Rank de guild
    ACHIEVEMENT = 11,       // Conquista obtida
    TIME_OF_DAY = 12,       // Hora do dia
    DAY_OF_WEEK = 13,       // Dia da semana
    STAT_VALUE = 14,        // Valor de atributo
    MONEY = 15,             // Dinheiro mínimo
    PROFESSION = 16,        // Profissão específica
    MAP_AREA = 17,          // Área de mapa
    QUEST_NOT_COMPLETED = 18, // Quest não completada
    PVP_RANK = 19,          // Rank de PvP
    CUSTOM = 20,            // Customizado
};

/**
 * @brief Flag de Quest
 */
enum class QuestFlag : BYTE {
    NONE = 0,               // Nenhuma flag
    SHARABLE = 1,           // Compartilhável
    AUTO_ACCEPT = 2,        // Aceita automaticamente
    AUTO_COMPLETE = 3,      // Completa automaticamente
    HIDE_REWARDS = 4,       // Esconde recompensas
    FAIL_ON_LOGOUT = 5,     // Falha ao desconectar
    FAIL_ON_DEATH = 6,      // Falha ao morrer
    REPEATABLE = 7,         // Repetível
    UNIQUE = 8,             // Única
    REQUIRED = 9,           // Obrigatória
    EXCLUSIVE = 10,         // Exclusiva
    TIMED = 11,             // Temporizada
    PARTY_SHARED = 12,      // Compartilhada com grupo
    STARTS_COMPLETE = 13,   // Começa completa
    HIDDEN_UNTIL_COMPLETE = 14, // Escondida até completar
    HIDDEN_REWARDS = 15,    // Recompensas escondidas
    SEQUENTIAL_OBJECTIVES = 16, // Objetivos sequenciais
    ALLIANCE_RESTRICTED = 17, // Restrita a aliança
    FACTION_RESTRICTED = 18, // Restrita a facção
    RANDOM_REWARDS = 19,    // Recompensas aleatórias
    BONUS_EXP = 20,         // EXP bônus
    BONUS_GOLD = 21,        // Ouro bônus
    BONUS_REP = 22,         // Reputação bônus
    GLOBAL_PROG_SHARE = 23, // Progresso compartilhado globalmente
    CUSTOM = 24,            // Customizada
};

/**
 * @brief Tipo de Script de Quest
 */
enum class QuestScriptType : BYTE {
    NONE = 0,               // Nenhum
    LUA = 1,                // Script Lua
    CUSTOM = 2,             // Script customizado
    EVENT_DRIVEN = 3,       // Baseado em eventos
    CONDITION_BASED = 4,    // Baseado em condições
    STATE_MACHINE = 5,      // Máquina de estados
    TRIGGER_BASED = 6,      // Baseado em gatilhos
    DIALOGUE_TREE = 7,      // Árvore de diálogo
};

/**
 * @brief Dificuldade de Quest
 */
enum class QuestDifficulty : BYTE {
    TRIVIAL = 0,            // Trivial
    EASY = 1,               // Fácil
    NORMAL = 2,             // Normal
    HARD = 3,               // Difícil
    VERY_HARD = 4,          // Muito difícil
    ELITE = 5,              // Elite
    LEGENDARY = 6,          // Lendária
    MYTHIC = 7,             // Mítica
    WORLD_BOSS = 8,         // Chefe mundial
    CUSTOM = 9,             // Customizada
};

/**
 * @brief Fase de Quest
 */
enum class QuestPhase : BYTE {
    INACTIVE = 0,           // Inativa
    AVAILABLE = 1,          // Disponível
    ACCEPTED = 2,           // Aceita
    IN_PROGRESS = 3,        // Em progresso
    READY_TO_COMPLETE = 4,  // Pronta para completar
    COMPLETE = 5,           // Completa
    FAILED = 6,             // Falhou
    ABANDONED = 7,          // Abandonada
    REVIEWING = 8,          // Em revisão
    REWARDED = 9,           // Recompensada
    EXPIRED = 10,           // Expirada
    CUSTOM = 11,            // Customizada
};

/**
 * @brief Tipo de Gatilho de Quest
 */
enum class QuestTriggerType : BYTE {
    NONE = 0,               // Nenhum
    ENTER_AREA = 1,         // Entrar em área
    LEAVE_AREA = 2,         // Sair de área
    KILL_MONSTER = 3,       // Matar monstro
    COLLECT_ITEM = 4,       // Coletar item
    TALK_TO_NPC = 5,        // Falar com NPC
    LEVEL_UP = 6,           // Subir de nível
    USE_SKILL = 7,          // Usar skill
    EQUIP_ITEM = 8,         // Equipar item
    TIME_PASSED = 9,        // Tempo passado
    HEALTH_THRESHOLD = 10,  // Limite de saúde
    QUEST_COMPLETED = 11,   // Quest completada
    QUEST_ACCEPTED = 12,    // Quest aceita
    PLAYER_DIED = 13,       // Jogador morreu
    BOSS_KILLED = 14,       // Chefe morto
    DUNGEON_ENTERED = 15,   // Entrou em masmorra
    RAID_COMPLETED = 16,    // Raid completada
    PLAYER_LOGIN = 17,      // Login do jogador
    PLAYER_LOGOUT = 18,     // Logout do jogador
    GAIN_REPUTATION = 19,   // Ganhar reputação
    LOSE_REPUTATION = 20,   // Perder reputação
    JOIN_GUILD = 21,        // Entrar em guild
    LEAVE_GUILD = 22,       // Sair de guild
    PARTY_FORMED = 23,      // Grupo formado
    PARTY_DISBANDED = 24,   // Grupo desfeito
    CRAFT_ITEM = 25,        // Craftar item
    CUSTOM = 26,            // Customizado
};

/**
 * @brief Tipo de Evento de Quest
 */
enum class QuestEventType : BYTE {
    NONE = 0,               // Nenhum
    START = 1,              // Iniciar quest
    ADVANCE = 2,            // Avançar quest
    COMPLETE = 3,           // Completar quest
    FAIL = 4,               // Falhar quest
    ABANDON = 5,            // Abandonar quest
    RESTART = 6,            // Reiniciar quest
    EXPIRE = 7,             // Expirar quest
    OBJECTIVE_COMPLETE = 8, // Completar objetivo
    OBJECTIVE_FAIL = 9,     // Falhar objetivo
    REWARD_GIVEN = 10,      // Recompensa dada
    TRIGGER_FIRED = 11,     // Gatilho acionado
    STATE_CHANGE = 12,      // Mudança de estado
    TIMER_START = 13,       // Iniciar temporizador
    TIMER_END = 14,         // Fim de temporizador
    DIALOGUE_START = 15,    // Iniciar diálogo
    DIALOGUE_END = 16,      // Fim de diálogo
    DIALOGUE_OPTION = 17,   // Opção de diálogo
    CUSTOM = 18,            // Customizado
};

/**
 * @brief Gênero de Quest
 */
enum class QuestGenre : BYTE {
    NONE = 0,               // Nenhum
    ACTION = 1,             // Ação
    ADVENTURE = 2,          // Aventura
    MYSTERY = 3,            // Mistério
    HORROR = 4,             // Horror
    COMEDY = 5,             // Comédia
    DRAMA = 6,              // Drama
    ROMANCE = 7,            // Romance
    FANTASY = 8,            // Fantasia
    SCIENCE_FICTION = 9,    // Ficção científica
    HISTORICAL = 10,        // Histórica
    EDUCATIONAL = 11,       // Educacional
    SURVIVAL = 12,          // Sobrevivência
    STEALTH = 13,           // Furtividade
    PUZZLE = 14,            // Quebra-cabeça
    STRATEGY = 15,          // Estratégia
    CUSTOM = 16,            // Customizado
};

/**
 * @brief Estado de Objetivo de Quest
 */
enum class QuestObjectiveState : BYTE {
    NONE = 0,               // Nenhum
    PENDING = 1,            // Pendente
    IN_PROGRESS = 2,        // Em progresso
    COMPLETED = 3,          // Completado
    FAILED = 4,             // Falhou
    DISABLED = 5,           // Desabilitado
    HIDDEN = 6,             // Escondido
    OPTIONAL = 7,           // Opcional
    EXPIRED = 8,            // Expirado
    CUSTOM = 9,             // Customizado
};

/**
 * @brief Definição de Objetivo de Quest
 */
struct QuestObjectiveDefinition {
    WORD objectiveID;                     // ID do objetivo
    QuestObjectiveType type;              // Tipo do objetivo
    std::string description;              // Descrição
    union {
        struct {                          // Para KILL
            WORD mobID;                   // ID do mob
            WORD count;                   // Quantidade
            BYTE minLevel;                // Nível mínimo
            BYTE maxLevel;                // Nível máximo
            BYTE mobClass;                // Classe do mob
            BYTE mobRace;                 // Raça do mob
        } kill;
        
        struct {                          // Para COLLECT
            WORD itemID;                  // ID do item
            WORD count;                   // Quantidade
            BYTE quality;                 // Qualidade mínima
            BYTE keepOnComplete;          // Manter após completar
        } collect;
        
        struct {                          // Para INTERACT
            WORD objectID;                // ID do objeto
            WORD mapID;                   // ID do mapa
            WORD posX;                    // Posição X
            WORD posY;                    // Posição Y
            BYTE range;                   // Raio de interação
        } interact;
        
        struct {                          // Para TALK
            WORD npcID;                   // ID do NPC
            WORD dialogueID;              // ID do diálogo
            BYTE requireItem;             // Requer item
            WORD requiredItemID;          // ID do item requerido
        } talk;
        
        struct {                          // Para DELIVER
            WORD itemID;                  // ID do item
            WORD count;                   // Quantidade
            WORD receiverID;              // ID do receptor
        } deliver;
        
        struct {                          // Para ESCORT
            WORD npcID;                   // ID do NPC
            WORD startMapID;              // ID do mapa inicial
            WORD startX;                  // Posição X inicial
            WORD startY;                  // Posição Y inicial
            WORD endMapID;                // ID do mapa final
            WORD endX;                    // Posição X final
            WORD endY;                    // Posição Y final
            WORD timeLimit;               // Limite de tempo (segundos)
        } escort;
        
        struct {                          // Para EXPLORE
            WORD mapID;                   // ID do mapa
            WORD centerX;                 // Posição X central
            WORD centerY;                 // Posição Y central
            WORD radius;                  // Raio
            BYTE discoverOnly;            // Apenas descobrir
        } explore;
        
        struct {                          // Para CRAFT
            WORD itemID;                  // ID do item
            WORD count;                   // Quantidade
            BYTE minQuality;              // Qualidade mínima
            BYTE professionRequired;      // Profissão requerida
            BYTE professionID;            // ID da profissão
            BYTE minProfessionLevel;      // Nível mínimo da profissão
        } craft;
        
        struct {                          // Para USE_SKILL
            WORD skillID;                 // ID da skill
            WORD count;                   // Quantidade de vezes
            WORD targetTypeID;            // ID do tipo de alvo
            BYTE inCombat;                // Em combate
        } useSkill;
        
        struct {                          // Para outros tipos
            DWORD param1;                 // Parâmetro 1
            DWORD param2;                 // Parâmetro 2
            DWORD param3;                 // Parâmetro 3
            DWORD param4;                 // Parâmetro 4
        } generic;
    } params;
    
    DWORD count;                          // Quantidade necessária
    DWORD counterType;                    // Tipo de contador
    DWORD timeLimit;                      // Limite de tempo (ms)
    BYTE optional;                        // Opcional
    BYTE hidden;                          // Escondido
    BYTE sequential;                      // Sequencial
    BYTE zoneRestricted;                  // Restrito a zona
    WORD zoneID;                          // ID da zona
    BYTE trackingType;                    // Tipo de rastreamento
    BYTE failOnDeath;                     // Falha ao morrer
    std::string customData;               // Dados customizados
    
    QuestObjectiveDefinition()
        : objectiveID(0)
        , type(QuestObjectiveType::NONE)
        , description("")
        , count(0)
        , counterType(0)
        , timeLimit(0)
        , optional(0)
        , hidden(0)
        , sequential(0)
        , zoneRestricted(0)
        , zoneID(0)
        , trackingType(0)
        , failOnDeath(0)
        , customData("")
    {
        memset(&params, 0, sizeof(params));
    }
};

/**
 * @brief Definição de Recompensa de Quest
 */
struct QuestRewardDefinition {
    WORD rewardID;                        // ID da recompensa
    QuestRewardType type;                 // Tipo da recompensa
    std::string description;              // Descrição
    union {
        struct {                          // Para ITEM
            WORD itemID;                  // ID do item
            WORD count;                   // Quantidade
            BYTE quality;                 // Qualidade
            BYTE bound;                   // Vinculado
            WORD enchantLevel;            // Nível de encantamento
            DWORD durability;             // Durabilidade
            BYTE attributeCount;          // Contagem de atributos
            BYTE socketCount;             // Contagem de soquetes
        } item;
        
        struct {                          // Para GOLD
            DWORD amount;                 // Quantidade
            BYTE bonusPercentage;         // Porcentagem de bônus
        } gold;
        
        struct {                          // Para EXPERIENCE
            DWORD amount;                 // Quantidade
            BYTE bonusPercentage;         // Porcentagem de bônus
            BYTE levelScaled;             // Escalado com nível
        } experience;
        
        struct {                          // Para REPUTATION
            WORD factionID;               // ID da facção
            DWORD amount;                 // Quantidade
            BYTE bonusPercentage;         // Porcentagem de bônus
        } reputation;
        
        struct {                          // Para SKILL
            WORD skillID;                 // ID da skill
            BYTE level;                   // Nível
            BYTE temporary;               // Temporário
            DWORD duration;               // Duração (ms)
        } skill;
        
        struct {                          // Para TITLE
            WORD titleID;                 // ID do título
            BYTE exclusive;               // Exclusivo
            BYTE autoEquip;               // Equipar automaticamente
        } title;
        
        struct {                          // Para CURRENCY
            WORD currencyID;              // ID da moeda
            DWORD amount;                 // Quantidade
            BYTE bonusPercentage;         // Porcentagem de bônus
        } currency;
        
        struct {                          // Para outros tipos
            DWORD param1;                 // Parâmetro 1
            DWORD param2;                 // Parâmetro 2
            DWORD param3;                 // Parâmetro 3
            DWORD param4;                 // Parâmetro 4
        } generic;
    } params;
    
    BYTE requiredClass;                   // Classe requerida
    BYTE classID;                         // ID da classe
    BYTE requiredLevel;                   // Nível requerido
    BYTE level;                           // Nível
    BYTE requiredReputation;              // Reputação requerida
    WORD factionID;                       // ID da facção
    BYTE reputationLevel;                 // Nível de reputação
    BYTE choiceReward;                    // Recompensa de escolha
    BYTE choiceGroup;                     // Grupo de escolha
    BYTE guaranteedDrop;                  // Drop garantido
    float dropChance;                     // Chance de drop
    BYTE hidden;                          // Escondido
    BYTE accountWide;                     // Para toda a conta
    std::string customData;               // Dados customizados
    
    QuestRewardDefinition()
        : rewardID(0)
        , type(QuestRewardType::NONE)
        , description("")
        , requiredClass(0)
        , classID(0)
        , requiredLevel(0)
        , level(0)
        , requiredReputation(0)
        , factionID(0)
        , reputationLevel(0)
        , choiceReward(0)
        , choiceGroup(0)
        , guaranteedDrop(1)
        , dropChance(100.0f)
        , hidden(0)
        , accountWide(0)
        , customData("")
    {
        memset(&params, 0, sizeof(params));
    }
};

/**
 * @brief Definição de Pré-requisito de Quest
 */
struct QuestPrereqDefinition {
    WORD prereqID;                        // ID do pré-requisito
    QuestPrereqType type;                 // Tipo do pré-requisito
    std::string description;              // Descrição
    union {
        struct {                          // Para QUEST_COMPLETED
            WORD questID;                 // ID da quest
            BYTE mustBeRecent;            // Deve ser recente
            DWORD timeWindow;             // Janela de tempo (ms)
        } questCompleted;
        
        struct {                          // Para QUEST_ACTIVE
            WORD questID;                 // ID da quest
            BYTE specificObjective;       // Objetivo específico
            BYTE objectiveID;             // ID do objetivo
        } questActive;
        
        struct {                          // Para LEVEL
            BYTE minLevel;                // Nível mínimo
            BYTE maxLevel;                // Nível máximo
            BYTE exactLevel;              // Nível exato
        } level;
        
        struct {                          // Para ITEM_OWNED
            WORD itemID;                  // ID do item
            WORD count;                   // Quantidade
            BYTE consumeItem;             // Consumir item
            BYTE equipRequirement;        // Requisito de equipamento
        } itemOwned;
        
        struct {                          // Para REPUTATION
            WORD factionID;               // ID da facção
            BYTE minLevel;                // Nível mínimo
            BYTE maxLevel;                // Nível máximo
            BYTE exactLevel;              // Nível exato
        } reputation;
        
        struct {                          // Para outros tipos
            DWORD param1;                 // Parâmetro 1
            DWORD param2;                 // Parâmetro 2
            DWORD param3;                 // Parâmetro 3
            DWORD param4;                 // Parâmetro 4
        } generic;
    } params;
    
    BYTE failureVisible;                  // Falha visível
    BYTE autoRemoved;                     // Removido automaticamente
    BYTE accountWide;                     // Para toda a conta
    BYTE exclusive;                       // Exclusivo
    std::string customData;               // Dados customizados
    
    QuestPrereqDefinition()
        : prereqID(0)
        , type(QuestPrereqType::NONE)
        , description("")
        , failureVisible(1)
        , autoRemoved(0)
        , accountWide(0)
        , exclusive(0)
        , customData("")
    {
        memset(&params, 0, sizeof(params));
    }
};

/**
 * @brief Definição de Diálogo de Quest
 */
struct QuestDialogueOption {
    WORD optionID;                        // ID da opção
    std::string text;                     // Texto
    WORD nextStateID;                     // ID do próximo estado
    WORD actionID;                        // ID da ação
    BYTE requireItem;                     // Requer item
    WORD requiredItemID;                  // ID do item requerido
    BYTE consumeItem;                     // Consumir item
    BYTE requiredObjectiveComplete;       // Requer objetivo completo
    WORD objectiveID;                     // ID do objetivo
    BYTE hidden;                          // Escondido
    BYTE affectsReputation;               // Afeta reputação
    WORD factionID;                       // ID da facção
    int reputationChange;                 // Mudança de reputação
    BYTE disabled;                        // Desabilitado
    std::string customData;               // Dados customizados
    
    QuestDialogueOption()
        : optionID(0)
        , text("")
        , nextStateID(0)
        , actionID(0)
        , requireItem(0)
        , requiredItemID(0)
        , consumeItem(0)
        , requiredObjectiveComplete(0)
        , objectiveID(0)
        , hidden(0)
        , affectsReputation(0)
        , factionID(0)
        , reputationChange(0)
        , disabled(0)
        , customData("")
    {
    }
};

/**
 * @brief Definição de Estado de Quest
 */
struct QuestStateDefinition {
    WORD stateID;                         // ID do estado
    std::string name;                     // Nome
    std::string description;              // Descrição
    std::string narrativeText;            // Texto narrativo
    std::string dialogueText;             // Texto de diálogo
    WORD npcID;                           // ID do NPC
    std::vector<QuestDialogueOption> options; // Opções de diálogo
    WORD nextStateID;                     // ID do próximo estado
    DWORD timeLimit;                      // Limite de tempo (ms)
    BYTE isEndState;                      // É estado final
    BYTE completesQuest;                  // Completa a quest
    BYTE failsQuest;                      // Falha a quest
    BYTE isCheckpoint;                    // É checkpoint
    WORD actionOnEnter;                   // Ação ao entrar
    WORD actionOnExit;                    // Ação ao sair
    WORD actionOnTimeout;                 // Ação ao timeout
    BYTE hideObjectives;                  // Esconde objetivos
    BYTE hideRewards;                     // Esconde recompensas
    std::string scriptOnEnter;            // Script ao entrar
    std::string scriptOnExit;             // Script ao sair
    std::string customData;               // Dados customizados
    
    QuestStateDefinition()
        : stateID(0)
        , name("")
        , description("")
        , narrativeText("")
        , dialogueText("")
        , npcID(0)
        , nextStateID(0)
        , timeLimit(0)
        , isEndState(0)
        , completesQuest(0)
        , failsQuest(0)
        , isCheckpoint(0)
        , actionOnEnter(0)
        , actionOnExit(0)
        , actionOnTimeout(0)
        , hideObjectives(0)
        , hideRewards(0)
        , scriptOnEnter("")
        , scriptOnExit("")
        , customData("")
    {
    }
};

/**
 * @brief Template de Quest
 */
struct QuestTemplate {
    QuestID questID;                      // ID da quest
    std::string name;                     // Nome
    std::string description;              // Descrição
    std::string summary;                  // Resumo
    std::string objectiveText;            // Texto de objetivos
    std::string completionText;           // Texto de conclusão
    QuestType type;                       // Tipo da quest
    QuestCategory category;               // Categoria da quest
    QuestGenre genre;                     // Gênero da quest
    QuestDifficulty difficulty;           // Dificuldade da quest
    BYTE minLevel;                        // Nível mínimo
    BYTE maxLevel;                        // Nível máximo
    BYTE recommendedLevel;                // Nível recomendado
    BYTE recommendedPlayers;              // Jogadores recomendados
    WORD suggestedGroupSize;              // Tamanho de grupo sugerido
    WORD startNPC;                        // NPC inicial
    WORD endNPC;                          // NPC final
    WORD startItem;                       // Item inicial
    WORD endItem;                         // Item final
    WORD startMap;                        // Mapa inicial
    WORD startX;                          // Posição X inicial
    WORD startY;                          // Posição Y inicial
    WORD endMap;                          // Mapa final
    WORD endX;                            // Posição X final
    WORD endY;                            // Posição Y final
    DWORD timeLimit;                      // Limite de tempo (ms)
    DWORD resetTime;                      // Tempo de reset (ms)
    DWORD cooldownTime;                   // Tempo de cooldown (ms)
    BYTE shareableProgress;               // Progresso compartilhável
    BYTE repeatableDaily;                 // Repetível diariamente
    BYTE repeatableWeekly;                // Repetível semanalmente
    BYTE repeatableMonthly;               // Repetível mensalmente
    BYTE accountWide;                     // Para toda a conta
    BYTE requiredFaction;                 // Facção requerida
    BYTE factionID;                       // ID da facção
    QuestScriptType scriptType;           // Tipo de script
    std::string scriptData;               // Dados de script
    std::bitset<32> questFlags;           // Flags de quest
    WORD startDialogueState;              // Estado de diálogo inicial
    WORD questChainID;                    // ID da cadeia de quests
    BYTE positionInChain;                 // Posição na cadeia
    BYTE isChainRequired;                 // É requerido na cadeia
    BYTE isAutoStart;                     // Inicia automaticamente
    BYTE isAutoComplete;                  // Completa automaticamente
    BYTE sharesProgressWith;              // Compartilha progresso com
    WORD sharedQuestID;                   // ID da quest compartilhada
    
    // Coleções de componentes
    std::vector<QuestObjectiveDefinition> objectives; // Objetivos
    std::vector<QuestRewardDefinition> rewards;      // Recompensas
    std::vector<QuestPrereqDefinition> prerequisites; // Pré-requisitos
    std::vector<QuestStateDefinition> states;        // Estados
    
    // Custom data
    std::string customData;               // Dados customizados
    
    QuestTemplate()
        : questID(0)
        , name("")
        , description("")
        , summary("")
        , objectiveText("")
        , completionText("")
        , type(QuestType::NONE)
        , category(QuestCategory::NONE)
        , genre(QuestGenre::NONE)
        , difficulty(QuestDifficulty::NORMAL)
        , minLevel(1)
        , maxLevel(MAX_LEVEL)
        , recommendedLevel(1)
        , recommendedPlayers(1)
        , suggestedGroupSize(1)
        , startNPC(0)
        , endNPC(0)
        , startItem(0)
        , endItem(0)
        , startMap(0)
        , startX(0)
        , startY(0)
        , endMap(0)
        , endX(0)
        , endY(0)
        , timeLimit(0)
        , resetTime(0)
        , cooldownTime(0)
        , shareableProgress(0)
        , repeatableDaily(0)
        , repeatableWeekly(0)
        , repeatableMonthly(0)
        , accountWide(0)
        , requiredFaction(0)
        , factionID(0)
        , scriptType(QuestScriptType::NONE)
        , scriptData("")
        , startDialogueState(0)
        , questChainID(0)
        , positionInChain(0)
        , isChainRequired(0)
        , isAutoStart(0)
        , isAutoComplete(0)
        , sharesProgressWith(0)
        , sharedQuestID(0)
        , customData("")
    {
        questFlags.reset();
    }
};

/**
 * @brief Status de Objetivo de Quest de um jogador
 */
struct PlayerQuestObjectiveStatus {
    WORD objectiveID;                     // ID do objetivo
    QuestObjectiveState state;            // Estado do objetivo
    DWORD currentCount;                   // Contagem atual
    DWORD timestamp;                      // Timestamp
    DWORD expiryTime;                     // Tempo de expiração
    BYTE trackingEnabled;                 // Rastreamento ativado
    std::string customData;               // Dados customizados
    
    PlayerQuestObjectiveStatus()
        : objectiveID(0)
        , state(QuestObjectiveState::PENDING)
        , currentCount(0)
        , timestamp(0)
        , expiryTime(0)
        , trackingEnabled(1)
        , customData("")
    {
    }
};

/**
 * @brief Status de Quest de um jogador
 */
struct PlayerQuestStatus {
    QuestID questID;                      // ID da quest
    QuestStatus status;                   // Status da quest
    QuestPhase phase;                     // Fase da quest
    DWORD acceptTime;                     // Tempo de aceitação
    DWORD lastUpdateTime;                 // Último tempo de atualização
    DWORD completeTime;                   // Tempo de conclusão
    DWORD expiryTime;                     // Tempo de expiração
    WORD currentState;                    // Estado atual
    BYTE tracked;                         // Rastreada
    BYTE failed;                          // Falhou
    BYTE abandoned;                       // Abandonada
    BYTE completedTimes;                  // Vezes completada
    DWORD lastResetTime;                  // Último tempo de reset
    
    // Status de objetivos
    std::vector<PlayerQuestObjectiveStatus> objectiveStatus; // Status de objetivos
    
    // Recompensas já recebidas
    std::vector<WORD> rewardsReceived;    // Recompensas recebidas
    
    // Dados customizados
    std::string customData;               // Dados customizados
    
    PlayerQuestStatus()
        : questID(0)
        , status(QuestStatus::UNAVAILABLE)
        , phase(QuestPhase::INACTIVE)
        , acceptTime(0)
        , lastUpdateTime(0)
        , completeTime(0)
        , expiryTime(0)
        , currentState(0)
        , tracked(0)
        , failed(0)
        , abandoned(0)
        , completedTimes(0)
        , lastResetTime(0)
        , customData("")
    {
    }
};

/**
 * @brief Cadeia de Quests
 */
struct QuestChain {
    WORD chainID;                         // ID da cadeia
    std::string name;                     // Nome
    std::string description;              // Descrição
    QuestCategory category;               // Categoria
    std::vector<QuestID> quests;          // Quests na cadeia
    BYTE requiresOrder;                   // Requer ordem
    BYTE repeatable;                      // Repetível
    DWORD resetTime;                      // Tempo de reset (ms)
    WORD finalRewardID;                   // ID da recompensa final
    BYTE accountWide;                     // Para toda a conta
    std::string customData;               // Dados customizados
    
    QuestChain()
        : chainID(0)
        , name("")
        , description("")
        , category(QuestCategory::NONE)
        , requiresOrder(1)
        , repeatable(0)
        , resetTime(0)
        , finalRewardID(0)
        , accountWide(0)
        , customData("")
    {
    }
};

/**
 * @brief Gatilho de Quest
 */
struct QuestTrigger {
    WORD triggerID;                       // ID do gatilho
    QuestTriggerType type;                // Tipo do gatilho
    std::string description;              // Descrição
    union {
        struct {                          // Para ENTER_AREA
            WORD mapID;                   // ID do mapa
            WORD centerX;                 // Posição X central
            WORD centerY;                 // Posição Y central
            WORD radius;                  // Raio
        } enterArea;
        
        struct {                          // Para KILL_MONSTER
            WORD mobID;                   // ID do mob
            BYTE minLevel;                // Nível mínimo
            BYTE maxLevel;                // Nível máximo
        } killMonster;
        
        struct {                          // Para outros tipos
            DWORD param1;                 // Parâmetro 1
            DWORD param2;                 // Parâmetro 2
            DWORD param3;                 // Parâmetro 3
            DWORD param4;                 // Parâmetro 4
        } generic;
    } params;
    
    WORD targetQuestID;                   // ID da quest alvo
    QuestEventType eventType;             // Tipo de evento
    BYTE onceOnly;                        // Apenas uma vez
    BYTE accountWide;                     // Para toda a conta
    BYTE shareable;                       // Compartilhável
    std::string customData;               // Dados customizados
    
    QuestTrigger()
        : triggerID(0)
        , type(QuestTriggerType::NONE)
        , description("")
        , targetQuestID(0)
        , eventType(QuestEventType::NONE)
        , onceOnly(1)
        , accountWide(0)
        , shareable(0)
        , customData("")
    {
        memset(&params, 0, sizeof(params));
    }
};

/**
 * @brief Evento de Quest
 */
struct QuestEvent {
    DWORD eventID;                        // ID do evento
    QuestEventType type;                  // Tipo do evento
    QuestID questID;                      // ID da quest
    WORD objectiveID;                     // ID do objetivo
    WORD stateID;                         // ID do estado
    DWORD sourceID;                       // ID da fonte
    DWORD targetID;                       // ID do alvo
    DWORD timestamp;                      // Timestamp
    std::string customData;               // Dados customizados
    
    QuestEvent()
        : eventID(0)
        , type(QuestEventType::NONE)
        , questID(0)
        , objectiveID(0)
        , stateID(0)
        , sourceID(0)
        , targetID(0)
        , timestamp(0)
        , customData("")
    {
    }
};

// Callback para eventos de quest
using QuestEventCallback = std::function<void(const QuestEvent&)>;

// Final de namespace
} // namespace quest
} // namespace server
} // namespace wyd

#endif // QUESTTYPES_H

} // namespace wydbr
