/**
 * quest_structs.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/core/quest_structs.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUEST_STRUCTS_H
#define QUEST_STRUCTS_H

#include <stdint.h>

#pragma pack(push, 1) // Força alinhamento de 1 byte para estruturas compactas

namespace wydbr {


// Códigos de erro específicos do sistema de quests
#define QUEST_SUCCESS            0
#define QUEST_ERROR_INVALID_ID   1
#define QUEST_ERROR_LEVEL        2
#define QUEST_ERROR_ACTIVE       3
#define QUEST_ERROR_COMPLETE     4
#define QUEST_ERROR_COOLDOWN     5
#define QUEST_ERROR_ITEM_FULL    6
#define QUEST_ERROR_CONDITION    7

// Tipos de quests
#define QUEST_TYPE_MAIN          0
#define QUEST_TYPE_SIDE          1
#define QUEST_TYPE_DAILY         2
#define QUEST_TYPE_WEEKLY        3
#define QUEST_TYPE_GUILD         4
#define QUEST_TYPE_EVENT         5
#define QUEST_TYPE_HIDDEN        6
#define QUEST_TYPE_ACHIEVEMENT   7
#define QUEST_TYPE_TUTORIAL      8

// Tipos de objetivos
#define QUEST_OBJECTIVE_KILL         0
#define QUEST_OBJECTIVE_COLLECT      1
#define QUEST_OBJECTIVE_DELIVER      2
#define QUEST_OBJECTIVE_TALK         3
#define QUEST_OBJECTIVE_VISIT        4
#define QUEST_OBJECTIVE_ESCORT       5
#define QUEST_OBJECTIVE_DEFEND       6
#define QUEST_OBJECTIVE_USE_SKILL    7
#define QUEST_OBJECTIVE_CRAFT        8
#define QUEST_OBJECTIVE_COMPLETE     9
#define QUEST_OBJECTIVE_CUSTOM       10

// Estados de quests para players
#define QUEST_STATE_AVAILABLE    0
#define QUEST_STATE_ACTIVE       1
#define QUEST_STATE_COMPLETE     2
#define QUEST_STATE_FAILED       3
#define QUEST_STATE_COMPLETED    4
#define QUEST_STATE_COOLDOWN     5
#define QUEST_STATE_UNAVAILABLE  6

// Constantes e limites do sistema
#define MAX_QUESTS               1000    // Número máximo de definições de quests
#define MAX_ACTIVE_QUESTS        20      // Número máximo de quests ativas por jogador
#define MAX_OBJECTIVES           10      // Número máximo de objetivos por quest
#define MAX_REWARDS              6       // Número máximo de recompensas por quest
#define MAX_PREREQS              5       // Número máximo de pré-requisitos por quest
#define MAX_QUEST_ITEMS          4       // Máximo de itens relacionados a quests por jogador
#define MAX_DIALOGUE_LENGTH      256     // Tamanho máximo do diálogo de quest
#define MAX_QUEST_NAME_LENGTH    32      // Tamanho máximo do nome da quest
#define MAX_QUEST_DESC_LENGTH    128     // Tamanho máximo da descrição da quest
#define MAX_QUEST_HISTORY        100     // Histórico máximo de quests completadas

// Flag bits para QuestHeaders
#define QUEST_FLAG_SHAREABLE     0x01
#define QUEST_FLAG_AUTO_COMPLETE 0x02
#define QUEST_FLAG_REPEATABLE    0x04
#define QUEST_FLAG_FAIL_ON_DEATH 0x08
#define QUEST_FLAG_TIME_LIMIT    0x10
#define QUEST_FLAG_EPIC          0x20
#define QUEST_FLAG_HIDDEN_REWARD 0x40
#define QUEST_FLAG_GUILD_RELATED 0x80

// Flag bits para objetivos
#define OBJECTIVE_FLAG_OPTIONAL  0x01
#define OBJECTIVE_FLAG_HIDDEN    0x02
#define OBJECTIVE_FLAG_GROUP     0x04    // Objetivo pode ser completado em grupo
#define OBJECTIVE_FLAG_UNIQUE    0x08    // Apenas um jogador pode completar
#define OBJECTIVE_FLAG_SEQUENTIAL 0x10   // Deve ser completado em ordem

/**
 * Definição compacta de um objetivo de quest
 * Tamanho: 32 bytes
 */
typedef struct _QUEST_OBJECTIVE {
    uint8_t     type;                   // Tipo de objetivo (0-10)
    uint8_t     flags;                  // Flags de objetivo
    uint16_t    padding;                // Padding para alinhamento
    
    union {
        // Para objetivos de tipo KILL
        struct {
            uint16_t    mobId;          // ID do mob para matar
            uint16_t    count;          // Quantidade a matar
            uint8_t     reserved[24];   // Reservado para expansão
        } kill;
        
        // Para objetivos de tipo COLLECT ou DELIVER
        struct {
            uint16_t    itemId;         // ID do item para coletar/entregar
            uint16_t    count;          // Quantidade a coletar/entregar
            uint16_t    npcId;          // NPC para entregar (se DELIVER)
            uint8_t     reserved[22];   // Reservado para expansão
        } item;
        
        // Para objetivos de tipo TALK
        struct {
            uint16_t    npcId;          // NPC para falar
            uint8_t     reserved[26];   // Reservado para expansão
        } talk;
        
        // Para objetivos de tipo VISIT
        struct {
            uint16_t    mapId;          // Mapa para visitar
            uint16_t    x;              // Coordenada X
            uint16_t    y;              // Coordenada Y
            uint16_t    radius;         // Raio de proximidade
            uint8_t     reserved[20];   // Reservado para expansão
        } visit;
        
        // Raw bytes para acesso direto
        uint8_t raw[28];
    } data;
} QUEST_OBJECTIVE;

/**
 * Recompensa de quest
 * Tamanho: 16 bytes
 */
typedef struct _QUEST_REWARD {
    uint32_t    exp;                    // Experiência concedida
    uint32_t    gold;                   // Gold concedido
    uint16_t    itemId[MAX_QUEST_ITEMS]; // IDs dos itens de recompensa
    uint8_t     itemCount[MAX_QUEST_ITEMS]; // Quantidade de cada item
} QUEST_REWARD;

/**
 * Cabeçalho compacto de definição de quest
 * Tamanho: 64 bytes
 */
typedef struct _QUEST_HEADER {
    uint16_t    questId;                // ID único da quest
    uint8_t     type;                   // Tipo de quest (0-8)
    uint8_t     flags;                  // Flags da quest
    
    uint16_t    minLevel;               // Nível mínimo para fazer a quest
    uint16_t    maxLevel;               // Nível máximo para fazer a quest
    
    uint16_t    startNpcId;             // NPC para iniciar a quest
    uint16_t    endNpcId;               // NPC para completar a quest
    uint16_t    mapId;                  // Mapa principal da quest
    
    uint16_t    timeLimit;              // Limite de tempo em minutos (0 = sem limite)
    uint16_t    cooldown;               // Cooldown em minutos para repetir
    
    uint16_t    prereqCount;            // Número de pré-requisitos
    uint16_t    prereqQuests[MAX_PREREQS]; // IDs das quests pré-requisito
    
    uint8_t     objectiveCount;         // Número de objetivos
    uint8_t     requiredObjectives;     // Quantos objetivos são obrigatórios
    
    uint16_t    rewardMobId;            // Mob que dropa recompensa especial (0 = nenhum)
    uint16_t    rewardRate;             // Taxa de drop (0-10000)
    
    uint8_t     dialogueIndex;          // Índice no arquivo de diálogos
    uint8_t     scriptIndex;            // Índice no arquivo de scripts
    
    uint8_t     reserved[20];           // Reservado para expansão futura
    
    char        name[MAX_QUEST_NAME_LENGTH]; // Nome da quest (null-terminated)
} QUEST_HEADER;

/**
 * Definição completa de quest (cabeçalho + objetivos)
 * Tamanho: 384 bytes
 */
typedef struct _QUEST_DATA {
    QUEST_HEADER header;                // Cabeçalho da quest (64 bytes)
    QUEST_OBJECTIVE objectives[MAX_OBJECTIVES]; // Objetivos (32 bytes * 10 = 320 bytes)
} QUEST_DATA;

/**
 * Estrutura de quest do jogador (estado atual)
 * Tamanho: 64 bytes
 */
typedef struct _PLAYER_QUEST {
    uint16_t    questId;                // ID da quest
    uint8_t     state;                  // Estado atual (0-6)
    uint8_t     flags;                  // Flags específicas do jogador
    
    uint32_t    startTime;              // Timestamp de início
    uint32_t    endTime;                // Timestamp de conclusão/falha
    uint32_t    cooldownEnd;            // Timestamp de fim do cooldown
    
    // Array de progresso dos objetivos
    // Significado depende do tipo de objetivo:
    // - Para KILL/COLLECT: contagem atual
    // - Para TALK/VISIT: 0 = não completo, 1 = completo
    // - Para objetivos complexos: bits de status
    uint32_t    progress[MAX_OBJECTIVES];
    
    uint8_t     reserved[16];           // Reservado para expansão
} PLAYER_QUEST;

/**
 * Estrutura para armazenar todas as quests ativas do jogador
 * Tamanho: 1280 bytes + 100 * 2 bytes = 1480 bytes
 */
typedef struct _PLAYER_QUEST_DATA {
    uint16_t        activeCount;                        // Número de quests ativas
    PLAYER_QUEST    activeQuests[MAX_ACTIVE_QUESTS];    // Quests ativas (64 * 20 = 1280 bytes)
    
    uint16_t        historyCount;                       // Número de quests no histórico
    uint16_t        completedQuests[MAX_QUEST_HISTORY]; // Histórico de quests completadas (2 * 100 = 200 bytes)
} PLAYER_QUEST_DATA;

/**
 * Item temporário de quest do jogador
 * Tamanho: 8 bytes
 */
typedef struct _QUEST_ITEM {
    uint16_t    itemId;                 // ID do item
    uint16_t    questId;                // ID da quest relacionada
    uint16_t    count;                  // Quantidade
    uint8_t     slot;                   // Slot no inventário (-1 se não está no inventário)
    uint8_t     flags;                  // Flags do item
} QUEST_ITEM;

/**
 * Pacote de dados de quest para cliente/servidor
 * Tamanho: variável
 */
typedef struct _MSG_QUEST {
    uint16_t    size;                   // Tamanho da mensagem
    uint16_t    msgType;                // Tipo da mensagem
    uint32_t    handle;                 // Handle do cliente/personagem
    
    uint16_t    questId;                // ID da quest
    uint8_t     action;                 // Ação (0 = info, 1 = iniciar, 2 = abandonar, 3 = completar, etc)
    uint8_t     result;                 // Resultado da ação
    
    union {
        // Para ações de quest
        struct {
            uint16_t    npcId;          // ID do NPC relacionado
            uint8_t     data[1];        // Dados variáveis
        } action;
        
        // Para listas de quests
        struct {
            uint16_t    count;          // Número de quests na lista
            uint16_t    ids[1];         // IDs das quests
        } list;
        
        // Para detalhes de uma quest
        struct {
            QUEST_DATA  data;           // Dados completos da quest
        } detail;
        
        // Para atualização de progresso
        struct {
            uint16_t    questId;        // ID da quest
            uint8_t     objectiveId;    // Índice do objetivo
            uint32_t    progress;       // Progresso atualizado
        } progress;
        
        // Raw bytes para acesso direto
        uint8_t raw[1];
    } data;
} MSG_QUEST;

#pragma pack(pop) // Restaura alinhamento padrão

#endif // QUEST_STRUCTS_H

} // namespace wydbr
