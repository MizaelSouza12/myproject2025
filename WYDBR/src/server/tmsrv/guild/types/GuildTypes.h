/**
 * GuildTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/guild/types/GuildTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef GUILDTYPES_H
#define GUILDTYPES_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <memory>
#include <set>
#include <bitset>
#include <array>
#include <variant>
#include <optional>

#include "../../../../core/WYDTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file GuildTypes.h
 * @brief Definições de tipos para o sistema avançado de guildas e guerras de território
 * 
 * Este arquivo contém as definições de tipos para o sistema avançado de guildas,
 * incluindo gerenciamento de membros, alianças, guerras de território, estruturas
 * de guilda, níveis, habilidades e recursos.
 */

namespace wyd {
namespace server {
namespace guild {

// Constantes do sistema de guildas
constexpr size_t MAX_GUILD_MEMBERS = 100;         // Máximo de membros por guilda
constexpr size_t MAX_GUILD_RANKS = 10;            // Máximo de ranks por guilda
constexpr size_t MAX_GUILD_SKILLS = 25;           // Máximo de habilidades por guilda
constexpr size_t MAX_GUILD_LOGS = 1000;           // Máximo de logs por guilda
constexpr size_t MAX_GUILD_STRUCTURES = 20;       // Máximo de estruturas por guilda
constexpr size_t MAX_GUILD_STORAGE = 500;         // Máximo de itens no armazenamento
constexpr size_t MAX_GUILD_ALLIANCES = 10;        // Máximo de alianças por guilda
constexpr size_t MAX_GUILD_ENEMIES = 20;          // Máximo de inimigos por guilda
constexpr size_t MAX_ACTIVE_WARS = 5;             // Máximo de guerras ativas por guilda
constexpr size_t MAX_WAR_REWARDS = 10;            // Máximo de recompensas por guerra
constexpr size_t MAX_TERRITORY_RESOURCES = 15;    // Máximo de recursos por território
constexpr size_t MAX_TERRITORY_STRUCTURES = 25;   // Máximo de estruturas por território
constexpr size_t MAX_TERRITORY_UPGRADES = 10;     // Máximo de melhorias por território
constexpr size_t MAX_GUILD_QUESTS = 50;           // Máximo de quests por guilda
constexpr size_t MAX_GUILD_EVENTS = 20;           // Máximo de eventos por guilda
constexpr size_t MAX_GUILD_BUFFS = 15;            // Máximo de buffs por guilda

/**
 * @brief Tipos de guilda
 */
enum class GuildType : BYTE {
    PVE = 0,                  // PvE
    PVP = 1,                  // PvP
    SOCIAL = 2,               // Social
    CRAFTING = 3,             // Criação
    TRADING = 4,              // Comércio
    HUNTING = 5,              // Caça
    MIXED = 6,                // Mista
    CUSTOM = 7,               // Personalizada
};

/**
 * @brief Status da guilda
 */
enum class GuildStatus : BYTE {
    ACTIVE = 0,               // Ativa
    INACTIVE = 1,             // Inativa
    FORMING = 2,              // Em formação
    DISBANDED = 3,            // Dissolvida
    SUSPENDED = 4,            // Suspensa
    MERGING = 5,              // Em fusão
    CUSTOM = 6,               // Personalizado
};

/**
 * @brief Rank de membro de guilda
 */
enum class GuildRank : BYTE {
    LEADER = 0,               // Líder
    SECOND_LEADER = 1,        // Vice-líder
    ELDER = 2,                // Ancião
    VETERAN = 3,              // Veterano
    MEMBER = 4,               // Membro
    RECRUIT = 5,              // Recruta
    TRIAL = 6,                // Teste
    ALLY = 7,                 // Aliado
    CUSTOM_1 = 8,             // Personalizado 1
    CUSTOM_2 = 9,             // Personalizado 2
};

/**
 * @brief Categoria de habilidade de guilda
 */
enum class GuildSkillCategory : BYTE {
    COMBAT = 0,               // Combate
    PRODUCTION = 1,           // Produção
    UTILITY = 2,              // Utilidade
    TERRITORY = 3,            // Território
    STORAGE = 4,              // Armazenamento
    MEMBERSHIP = 5,           // Associação
    WAR = 6,                  // Guerra
    CUSTOM = 7,               // Personalizada
};

/**
 * @brief Tipo de estrutura de guilda
 */
enum class GuildStructureType : BYTE {
    HALL = 0,                 // Salão
    BARRACKS = 1,             // Quartel
    FORGE = 2,                // Forja
    STORAGE = 3,              // Armazenamento
    WORKSHOP = 4,             // Oficina
    LABORATORY = 5,           // Laboratório
    TEMPLE = 6,               // Templo
    TOWER = 7,                // Torre
    WALL = 8,                 // Muralha
    GATE = 9,                 // Portão
    FARM = 10,                // Fazenda
    MINE = 11,                // Mina
    CUSTOM = 12,              // Personalizada
};

/**
 * @brief Tipo de evento de guilda
 */
enum class GuildEventType : BYTE {
    MEMBER_JOIN = 0,          // Entrada de membro
    MEMBER_LEAVE = 1,         // Saída de membro
    RANK_CHANGE = 2,          // Mudança de rank
    SKILL_LEARNED = 3,        // Habilidade aprendida
    STRUCTURE_BUILT = 4,      // Estrutura construída
    WAR_DECLARED = 5,         // Guerra declarada
    WAR_ENDED = 6,            // Guerra encerrada
    ALLIANCE_FORMED = 7,      // Aliança formada
    TERRITORY_CONQUERED = 8,  // Território conquistado
    TERRITORY_LOST = 9,       // Território perdido
    CONTRIBUTION = 10,        // Contribuição
    GUILD_LEVEL_UP = 11,      // Subida de nível
    STORAGE_TRANSACTION = 12, // Transação de armazenamento
    CUSTOM = 13,              // Personalizado
};

/**
 * @brief Tipo de permissão de guilda
 */
enum class GuildPermission : BYTE {
    INVITE = 0,               // Convidar
    KICK = 1,                 // Expulsar
    PROMOTE = 2,              // Promover
    DEMOTE = 3,               // Rebaixar
    EDIT_MOTD = 4,            // Editar mensagem do dia
    EDIT_RANKS = 5,           // Editar ranks
    USE_GUILD_CHAT = 6,       // Usar chat da guilda
    WITHDRAW_FUNDS = 7,       // Retirar fundos
    DEPOSIT_FUNDS = 8,        // Depositar fundos
    DECLARE_WAR = 9,          // Declarar guerra
    FORM_ALLIANCE = 10,       // Formar aliança
    MANAGE_STRUCTURES = 11,   // Gerenciar estruturas
    USE_GUILD_STORAGE = 12,   // Usar armazenamento
    CUSTOM = 13,              // Personalizada
};

/**
 * @brief Tipo de item de armazenamento de guilda
 */
enum class GuildStorageItemType : BYTE {
    EQUIPMENT = 0,            // Equipamento
    CONSUMABLE = 1,           // Consumível
    MATERIAL = 2,             // Material
    QUEST = 3,                // Quest
    CURRENCY = 4,             // Moeda
    TROPHY = 5,               // Troféu
    CUSTOM = 6,               // Personalizado
};

/**
 * @brief Tipo de guerra de guilda
 */
enum class GuildWarType : BYTE {
    TERRITORY = 0,            // Território
    HONOR = 1,                // Honra
    RESOURCES = 2,            // Recursos
    SIEGE = 3,                // Cerco
    PRACTICE = 4,             // Prática
    VENDETTA = 5,             // Vingança
    CUSTOM = 6,               // Personalizada
};

/**
 * @brief Status de guerra de guilda
 */
enum class GuildWarStatus : BYTE {
    PROPOSED = 0,             // Proposta
    ACCEPTED = 1,             // Aceita
    ACTIVE = 2,               // Ativa
    ENDED = 3,                // Encerrada
    SURRENDERED = 4,          // Rendida
    EXPIRED = 5,              // Expirada
    CUSTOM = 6,               // Personalizado
};

/**
 * @brief Tipo de território
 */
enum class TerritoryType : BYTE {
    PLAIN = 0,                // Planície
    FOREST = 1,               // Floresta
    MOUNTAIN = 2,             // Montanha
    DESERT = 3,               // Deserto
    SWAMP = 4,                // Pântano
    COAST = 5,                // Costa
    DUNGEON = 6,              // Masmorra
    CITY = 7,                 // Cidade
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Tipo de recurso de território
 */
enum class TerritoryResourceType : BYTE {
    ORE = 0,                  // Minério
    WOOD = 1,                 // Madeira
    HERB = 2,                 // Erva
    ANIMAL = 3,               // Animal
    ENERGY = 4,               // Energia
    CRYSTAL = 5,              // Cristal
    FOOD = 6,                 // Comida
    WATER = 7,                // Água
    CUSTOM = 8,               // Personalizado
};

/**
 * @brief Status de território
 */
enum class TerritoryStatus : BYTE {
    UNOCCUPIED = 0,           // Desocupado
    OCCUPIED = 1,             // Ocupado
    CONTESTED = 2,            // Contestado
    PROTECTED = 3,            // Protegido
    RESTRICTED = 4,           // Restrito
    CUSTOM = 5,               // Personalizado
};

/**
 * @brief Tipo de relação entre guildas
 */
enum class GuildRelationType : BYTE {
    NEUTRAL = 0,              // Neutra
    FRIENDLY = 1,             // Amigável
    ALLIED = 2,               // Aliada
    HOSTILE = 3,              // Hostil
    AT_WAR = 4,               // Em guerra
    TRADE_PARTNER = 5,        // Parceira comercial
    CUSTOM = 6,               // Personalizada
};

/**
 * @brief Membro de guilda
 */
struct GuildMember {
    DWORD memberID;            // ID do membro
    DWORD guildID;             // ID da guilda
    DWORD playerID;            // ID do jogador
    std::string playerName;     // Nome do jogador
    GuildRank rank;             // Rank
    time_t joinDate;            // Data de entrada
    time_t lastActiveDate;      // Data da última atividade
    DWORD contribution;         // Contribuição
    std::bitset<32> permissions; // Permissões
    std::string note;           // Nota
    DWORD weeklyContribution;   // Contribuição semanal
    DWORD pvpScore;             // Pontuação PvP para a guilda
    
    GuildMember()
        : memberID(0)
        , guildID(0)
        , playerID(0)
        , playerName("")
        , rank(GuildRank::RECRUIT)
        , joinDate(0)
        , lastActiveDate(0)
        , contribution(0)
        , note("")
        , weeklyContribution(0)
        , pvpScore(0)
    {
    }
};

/**
 * @brief Rank personalizado de guilda
 */
struct GuildCustomRank {
    DWORD rankID;              // ID do rank
    DWORD guildID;             // ID da guilda
    BYTE level;                // Nível
    std::string name;           // Nome
    std::bitset<32> permissions; // Permissões
    std::string description;    // Descrição
    DWORD maxMembers;          // Máximo de membros
    
    GuildCustomRank()
        : rankID(0)
        , guildID(0)
        , level(0)
        , name("")
        , description("")
        , maxMembers(0)
    {
    }
};

/**
 * @brief Habilidade de guilda
 */
struct GuildSkill {
    DWORD skillID;             // ID da habilidade
    DWORD guildID;             // ID da guilda
    std::string name;           // Nome
    std::string description;    // Descrição
    GuildSkillCategory category; // Categoria
    BYTE level;                // Nível
    BYTE maxLevel;             // Nível máximo
    DWORD pointCost;           // Custo em pontos
    DWORD goldCost;            // Custo em ouro
    std::string effects;        // Efeitos
    bool active;                // Ativa
    DWORD cooldownSeconds;     // Cooldown (segundos)
    DWORD prerequisites;       // Pré-requisitos
    
    GuildSkill()
        : skillID(0)
        , guildID(0)
        , name("")
        , description("")
        , category(GuildSkillCategory::COMBAT)
        , level(0)
        , maxLevel(10)
        , pointCost(0)
        , goldCost(0)
        , effects("")
        , active(true)
        , cooldownSeconds(0)
        , prerequisites(0)
    {
    }
};

/**
 * @brief Estrutura de guilda
 */
struct GuildStructure {
    DWORD structureID;         // ID da estrutura
    DWORD guildID;             // ID da guilda
    GuildStructureType type;    // Tipo
    std::string name;           // Nome
    BYTE level;                // Nível
    BYTE maxLevel;             // Nível máximo
    DWORD constructionCost;    // Custo de construção
    DWORD maintenanceCost;     // Custo de manutenção
    DWORD territoryID;         // ID do território
    float posX;                 // Posição X
    float posY;                 // Posição Y
    float posZ;                 // Posição Z
    std::string effects;        // Efeitos
    DWORD health;              // Vida
    DWORD maxHealth;           // Vida máxima
    time_t constructedDate;     // Data de construção
    time_t lastUpgradeDate;     // Data da última melhoria
    time_t lastRepairDate;      // Data do último reparo
    
    GuildStructure()
        : structureID(0)
        , guildID(0)
        , type(GuildStructureType::HALL)
        , name("")
        , level(1)
        , maxLevel(10)
        , constructionCost(0)
        , maintenanceCost(0)
        , territoryID(0)
        , posX(0.0f)
        , posY(0.0f)
        , posZ(0.0f)
        , effects("")
        , health(100)
        , maxHealth(100)
        , constructedDate(0)
        , lastUpgradeDate(0)
        , lastRepairDate(0)
    {
    }
};

/**
 * @brief Item de armazenamento de guilda
 */
struct GuildStorageItem {
    DWORD storageID;           // ID do armazenamento
    DWORD guildID;             // ID da guilda
    DWORD itemID;              // ID do item
    WORD quantity;             // Quantidade
    GuildStorageItemType type;  // Tipo
    std::string notes;          // Notas
    DWORD depositorID;         // ID do depositante
    time_t depositDate;         // Data do depósito
    BYTE tab;                  // Aba
    BYTE quality;              // Qualidade
    bool locked;                // Bloqueado
    
    GuildStorageItem()
        : storageID(0)
        , guildID(0)
        , itemID(0)
        , quantity(0)
        , type(GuildStorageItemType::MATERIAL)
        , notes("")
        , depositorID(0)
        , depositDate(0)
        , tab(0)
        , quality(0)
        , locked(false)
    {
    }
};

/**
 * @brief Log de guilda
 */
struct GuildLog {
    DWORD logID;               // ID do log
    DWORD guildID;             // ID da guilda
    time_t timestamp;           // Timestamp
    GuildEventType eventType;   // Tipo de evento
    DWORD actorID;             // ID do ator
    std::string actorName;      // Nome do ator
    DWORD targetID;            // ID do alvo
    std::string targetName;     // Nome do alvo
    std::string details;        // Detalhes
    
    GuildLog()
        : logID(0)
        , guildID(0)
        , timestamp(0)
        , eventType(GuildEventType::MEMBER_JOIN)
        , actorID(0)
        , actorName("")
        , targetID(0)
        , targetName("")
        , details("")
    {
    }
};

/**
 * @brief Relação entre guildas
 */
struct GuildRelation {
    DWORD relationID;          // ID da relação
    DWORD guildID1;            // ID da guilda 1
    DWORD guildID2;            // ID da guilda 2
    GuildRelationType type;     // Tipo
    time_t startDate;           // Data de início
    time_t expiryDate;          // Data de expiração
    std::string terms;          // Termos
    bool active;                // Ativa
    
    GuildRelation()
        : relationID(0)
        , guildID1(0)
        , guildID2(0)
        , type(GuildRelationType::NEUTRAL)
        , startDate(0)
        , expiryDate(0)
        , terms("")
        , active(true)
    {
    }
};

/**
 * @brief Guerra de guilda
 */
struct GuildWar {
    DWORD warID;               // ID da guerra
    DWORD attackerGuildID;     // ID da guilda atacante
    DWORD defenderGuildID;     // ID da guilda defensora
    GuildWarType type;          // Tipo
    GuildWarStatus status;      // Status
    time_t declaredDate;        // Data de declaração
    time_t startDate;           // Data de início
    time_t endDate;             // Data de término
    DWORD attackerScore;       // Pontuação da atacante
    DWORD defenderScore;       // Pontuação da defensora
    DWORD territoryID;         // ID do território
    std::string terms;          // Termos
    std::string rewards;        // Recompensas
    DWORD winnerGuildID;       // ID da guilda vencedora
    bool scheduled;             // Agendada
    
    GuildWar()
        : warID(0)
        , attackerGuildID(0)
        , defenderGuildID(0)
        , type(GuildWarType::TERRITORY)
        , status(GuildWarStatus::PROPOSED)
        , declaredDate(0)
        , startDate(0)
        , endDate(0)
        , attackerScore(0)
        , defenderScore(0)
        , territoryID(0)
        , terms("")
        , rewards("")
        , winnerGuildID(0)
        , scheduled(false)
    {
    }
};

/**
 * @brief Batalha de guerra de guilda
 */
struct GuildWarBattle {
    DWORD battleID;            // ID da batalha
    DWORD warID;               // ID da guerra
    time_t startTime;           // Hora de início
    time_t endTime;             // Hora de término
    DWORD attackerScore;       // Pontuação da atacante
    DWORD defenderScore;       // Pontuação da defensora
    DWORD locationID;          // ID da localização
    DWORD winnerGuildID;       // ID da guilda vencedora
    std::string battleLog;      // Log da batalha
    std::string rewards;        // Recompensas
    
    GuildWarBattle()
        : battleID(0)
        , warID(0)
        , startTime(0)
        , endTime(0)
        , attackerScore(0)
        , defenderScore(0)
        , locationID(0)
        , winnerGuildID(0)
        , battleLog("")
        , rewards("")
    {
    }
};

/**
 * @brief Contribuição de membro em guerra
 */
struct GuildWarContribution {
    DWORD contributionID;      // ID da contribuição
    DWORD warID;               // ID da guerra
    DWORD battleID;            // ID da batalha
    DWORD memberID;            // ID do membro
    DWORD kills;               // Abates
    DWORD deaths;              // Mortes
    DWORD structuresDamaged;   // Estruturas danificadas
    DWORD structuresDestroyed; // Estruturas destruídas
    DWORD healing;             // Cura
    DWORD damageDealt;         // Dano causado
    DWORD points;              // Pontos
    
    GuildWarContribution()
        : contributionID(0)
        , warID(0)
        , battleID(0)
        , memberID(0)
        , kills(0)
        , deaths(0)
        , structuresDamaged(0)
        , structuresDestroyed(0)
        , healing(0)
        , damageDealt(0)
        , points(0)
    {
    }
};

/**
 * @brief Território
 */
struct Territory {
    DWORD territoryID;         // ID do território
    std::string name;           // Nome
    TerritoryType type;         // Tipo
    TerritoryStatus status;     // Status
    DWORD ownerGuildID;        // ID da guilda proprietária
    DWORD mapID;               // ID do mapa
    float centerX;              // Centro X
    float centerY;              // Centro Y
    float radius;               // Raio
    time_t captureDate;         // Data de captura
    DWORD controlPoints;       // Pontos de controle
    DWORD value;               // Valor
    DWORD dailyIncome;         // Renda diária
    std::vector<std::pair<TerritoryResourceType, DWORD>> resources; // Recursos
    DWORD maxStructures;       // Máximo de estruturas
    DWORD taxRate;             // Taxa de imposto
    
    Territory()
        : territoryID(0)
        , name("")
        , type(TerritoryType::PLAIN)
        , status(TerritoryStatus::UNOCCUPIED)
        , ownerGuildID(0)
        , mapID(0)
        , centerX(0.0f)
        , centerY(0.0f)
        , radius(0.0f)
        , captureDate(0)
        , controlPoints(0)
        , value(0)
        , dailyIncome(0)
        , maxStructures(0)
        , taxRate(0)
    {
    }
};

/**
 * @brief Buff de guilda
 */
struct GuildBuff {
    DWORD buffID;              // ID do buff
    DWORD guildID;             // ID da guilda
    std::string name;           // Nome
    std::string description;    // Descrição
    std::string effects;        // Efeitos
    time_t startTime;           // Hora de início
    time_t endTime;             // Hora de término
    DWORD activatorID;         // ID do ativador
    DWORD cost;                // Custo
    DWORD level;               // Nível
    bool active;                // Ativo
    
    GuildBuff()
        : buffID(0)
        , guildID(0)
        , name("")
        , description("")
        , effects("")
        , startTime(0)
        , endTime(0)
        , activatorID(0)
        , cost(0)
        , level(1)
        , active(true)
    {
    }
};

/**
 * @brief Quest de guilda
 */
struct GuildQuest {
    DWORD questID;             // ID da quest
    DWORD guildID;             // ID da guilda
    std::string name;           // Nome
    std::string description;    // Descrição
    DWORD objectiveType;       // Tipo de objetivo
    DWORD objectiveTarget;     // Alvo do objetivo
    DWORD objectiveCount;      // Contagem do objetivo
    DWORD currentCount;        // Contagem atual
    time_t startTime;           // Hora de início
    time_t endTime;             // Hora de término
    DWORD rewardType;          // Tipo de recompensa
    DWORD rewardAmount;        // Quantidade de recompensa
    bool completed;             // Completa
    DWORD difficulty;          // Dificuldade
    
    GuildQuest()
        : questID(0)
        , guildID(0)
        , name("")
        , description("")
        , objectiveType(0)
        , objectiveTarget(0)
        , objectiveCount(0)
        , currentCount(0)
        , startTime(0)
        , endTime(0)
        , rewardType(0)
        , rewardAmount(0)
        , completed(false)
        , difficulty(0)
    {
    }
};

/**
 * @brief Notificação de guilda
 */
struct GuildNotification {
    DWORD notificationID;      // ID da notificação
    DWORD guildID;             // ID da guilda
    std::string message;        // Mensagem
    time_t timestamp;           // Timestamp
    DWORD priority;            // Prioridade
    bool systemGenerated;       // Gerada pelo sistema
    DWORD creatorID;           // ID do criador
    
    GuildNotification()
        : notificationID(0)
        , guildID(0)
        , message("")
        , timestamp(0)
        , priority(0)
        , systemGenerated(false)
        , creatorID(0)
    {
    }
};

/**
 * @brief Dados completos de uma guilda
 */
struct Guild {
    DWORD guildID;             // ID da guilda
    std::string name;           // Nome
    std::string tag;            // Tag
    GuildType type;             // Tipo
    GuildStatus status;         // Status
    DWORD leaderID;            // ID do líder
    time_t foundingDate;        // Data de fundação
    BYTE level;                // Nível
    DWORD experience;          // Experiência
    DWORD skillPoints;         // Pontos de habilidade
    DWORD gold;                // Ouro
    DWORD maxMembers;          // Máximo de membros
    std::string description;    // Descrição
    std::string announcement;   // Anúncio
    DWORD emblemID;            // ID do emblema
    DWORD territoryCount;      // Contagem de territórios
    DWORD faction;             // Facção
    DWORD ranking;             // Classificação
    time_t lastActivityDate;    // Data da última atividade
    
    Guild()
        : guildID(0)
        , name("")
        , tag("")
        , type(GuildType::MIXED)
        , status(GuildStatus::FORMING)
        , leaderID(0)
        , foundingDate(0)
        , level(1)
        , experience(0)
        , skillPoints(0)
        , gold(0)
        , maxMembers(MAX_GUILD_MEMBERS)
        , description("")
        , announcement("")
        , emblemID(0)
        , territoryCount(0)
        , faction(0)
        , ranking(0)
        , lastActivityDate(0)
    {
    }
};

/**
 * @brief Tipos de dados serializáveis para o sistema de guilda
 */
using GuildDataVariant = std::variant<
    Guild,
    GuildMember,
    GuildCustomRank,
    GuildSkill,
    GuildStructure,
    GuildStorageItem,
    GuildLog,
    GuildRelation,
    GuildWar,
    GuildWarBattle,
    GuildWarContribution,
    Territory,
    GuildBuff,
    GuildQuest,
    GuildNotification
>;

/**
 * @brief Callback de evento de guilda
 */
using GuildEventCallback = std::function<void(DWORD, GuildEventType, DWORD, DWORD, const std::string&)>;

/**
 * @brief Callback de verificação de permissão
 */
using GuildPermissionCheckCallback = std::function<bool(DWORD, DWORD, GuildPermission)>;

/**
 * @brief Callback de contribuição de guerra
 */
using GuildWarContributionCallback = std::function<void(DWORD, DWORD, DWORD, DWORD)>;

/**
 * @brief Callback de atualização de território
 */
using TerritoryUpdateCallback = std::function<void(DWORD, DWORD, TerritoryStatus)>;

} // namespace guild
} // namespace server
} // namespace wyd

#endif // GUILDTYPES_H

} // namespace wydbr
