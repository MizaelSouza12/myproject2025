/**
 * GuildTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/guild/GuildTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef GUILDTYPES_H
#define GUILDTYPES_H

#include <string>
#include <vector>
#include <map>
#include <array>
#include <bitset>
#include <chrono>
#include <ctime>

#include "../../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file GuildTypes.h
 * @brief Definições de tipos para o sistema de guilds e wars
 * 
 * Este arquivo contém as definições de tipos para o sistema avançado de guilds e guild wars,
 * que corrige várias limitações e bugs do sistema original do WYD.
 * O sistema original tinha diversas falhas que permitiam exploits e apresentava
 * funcionalidades muito limitadas. Esta implementação oferece um sistema completo e robusto.
 */

namespace wyd {
namespace server {
namespace guild {

// Constantes de sistema
constexpr int MAX_GUILDS = 4096;                // Máximo de guilds no servidor
constexpr int MIN_GUILD_NAME_LENGTH = 4;        // Tamanho mínimo do nome da guild
constexpr int MAX_GUILD_NAME_LENGTH = 16;       // Tamanho máximo do nome da guild
constexpr int MAX_GUILD_MOTTO_LENGTH = 128;     // Tamanho máximo do lema da guild
constexpr int MAX_GUILD_NOTICE_LENGTH = 512;    // Tamanho máximo do aviso da guild
constexpr int MAX_GUILD_MEMBERS = 100;          // Máximo de membros na guild
constexpr int MAX_GUILD_RANKS = 10;             // Máximo de ranks na guild
constexpr int MAX_GUILD_RANK_NAME_LENGTH = 16;  // Tamanho máximo do nome do rank
constexpr int MAX_GUILD_ALLIANCES = 5;          // Máximo de alianças por guild
constexpr int MAX_GUILD_ENEMIES = 10;           // Máximo de guilds inimigas
constexpr int MAX_GUILD_WARS = 10;              // Máximo de guerras simultâneas
constexpr int MAX_GUILD_WAR_HISTORY = 100;      // Máximo de entradas no histórico de guerras
constexpr int MAX_GUILD_LOG_ENTRIES = 1000;     // Máximo de entradas no log da guild
constexpr int MAX_GUILD_BANK_ITEMS = 500;       // Máximo de itens no banco da guild
constexpr int MAX_GUILD_BANK_GOLD = 2000000000; // Máximo de ouro no banco da guild
constexpr int MAX_GUILD_SKILL_LEVEL = 10;       // Nível máximo de skill de guild
constexpr int MAX_GUILD_TERRITORIES = 20;       // Máximo de territórios por guild
constexpr int MAX_GUILD_MEMBERS_ONLINE = 100;   // Máximo de membros online simultaneamente

/**
 * @brief ID de Guild
 */
struct GuildID {
    WORD id;                        // Identificador único
    
    GuildID(WORD _id = 0) /**
 * id
 * 
 * Implementa a funcionalidade id conforme especificação original.
 * @param _id Parâmetro _id
 * @return Retorna :
 */
 : id(_id){}
    
    bool operator==(const GuildID& other) const {
        return id == other.id;
    }
    
    bool operator!=(const GuildID& other) const {
        return id != other.id;
    }
    
    bool operator<(const GuildID& other) const {
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
 * @brief Níveis de Guild
 */
enum class GuildLevel : BYTE {
    NOVICE = 0,              // Novata (Nível 0)
    RECOGNIZED = 1,          // Reconhecida (Nível 1)
    ESTABLISHED = 2,         // Estabelecida (Nível 2)
    RESPECTED = 3,           // Respeitada (Nível 3)
    DISTINGUISHED = 4,       // Distinta (Nível 4)
    PROMINENT = 5,           // Proeminente (Nível 5)
    PRESTIGIOUS = 6,         // Prestigiosa (Nível 6)
    LEGENDARY = 7,           // Lendária (Nível 7)
    MYTHICAL = 8,            // Mítica (Nível 8)
    DIVINE = 9,              // Divina (Nível 9)
    TRANSCENDENT = 10,       // Transcendente (Nível 10)
};

/**
 * @brief Emblema da Guild
 */
struct GuildEmblem {
    WORD background;              // Fundo
    WORD symbol;                  // Símbolo
    BYTE backgroundColor;         // Cor de fundo
    BYTE symbolColor;             // Cor do símbolo
    BYTE symbolRotation;          // Rotação do símbolo
    BYTE symbolScale;             // Escala do símbolo
    
    GuildEmblem()
        : background(0)
        , symbol(0)
        , backgroundColor(0)
        , symbolColor(0)
        , symbolRotation(0)
        , symbolScale(100)
    {
    }
};

/**
 * @brief Posição de um membro na Guild
 */
enum class GuildRank : BYTE {
    MEMBER = 0,              // Membro
    SENIOR = 1,              // Sênior
    OFFICER = 2,             // Oficial
    LIEUTENANT = 3,          // Tenente
    CAPTAIN = 4,             // Capitão
    GENERAL = 5,             // General
    ADVISOR = 6,             // Conselheiro
    VICE_MASTER = 7,         // Vice-Mestre
    MASTER = 8,              // Mestre
    FOUNDER = 9,             // Fundador
};

/**
 * @brief Permissões de Guild
 */
enum class GuildPermission : BYTE {
    INVITE = 0,              // Convidar membros
    KICK = 1,                // Expulsar membros
    PROMOTE = 2,             // Promover membros
    DEMOTE = 3,              // Rebaixar membros
    MODIFY_MOTTO = 4,        // Modificar lema
    MODIFY_NOTICE = 5,       // Modificar aviso
    MODIFY_EMBLEM = 6,       // Modificar emblema
    MODIFY_RANKS = 7,        // Modificar ranks
    DECLARE_WAR = 8,         // Declarar guerra
    ACCEPT_WAR = 9,          // Aceitar guerra
    SURRENDER_WAR = 10,      // Render-se em guerra
    ALLY = 11,               // Aliar-se
    BREAK_ALLIANCE = 12,     // Romper aliança
    USE_GUILD_SKILLS = 13,   // Usar skills de guild
    WITHDRAW_BANK = 14,      // Sacar do banco
    DEPOSIT_BANK = 15,       // Depositar no banco
    VIEW_BANK = 16,          // Ver banco
    VIEW_LOG = 17,           // Ver log
    MODIFY_TAX = 18,         // Modificar taxa
    CLAIM_TERRITORY = 19,    // Reivindicar território
    ABANDON_TERRITORY = 20,  // Abandonar território
    DISBAND_GUILD = 21,      // Dissolver guild
    SCHEDULE_EVENTS = 22,    // Agendar eventos
    MANAGE_SKILLS = 23,      // Gerenciar skills
};

/**
 * @brief Nível de Aliança
 */
enum class AllianceLevel : BYTE {
    NONE = 0,                // Nenhuma
    FRIENDLY = 1,            // Amigável
    ALLIED = 2,              // Aliada
    CONFEDERATED = 3,        // Confederada
    VASSAL = 4,              // Vassala
    PROTECTORATE = 5,        // Protetorado
    INTEGRATED = 6,          // Integrada
};

/**
 * @brief Estado de Guerra
 */
enum class WarState : BYTE {
    NONE = 0,                // Nenhum
    DECLARED = 1,            // Declarada
    ACCEPTED = 2,            // Aceita
    ONGOING = 3,             // Em andamento
    RESOLVED = 4,            // Resolvida
    SURRENDERED = 5,         // Rendição
    ABANDONED = 6,           // Abandonada
    EXPIRED = 7,             // Expirada
    DRAW = 8,                // Empate
};

/**
 * @brief Tipo de Guerra
 */
enum class WarType : BYTE {
    NONE = 0,                // Nenhum
    SKIRMISH = 1,            // Escaramuça
    FORMAL = 2,              // Formal
    CONQUEST = 3,            // Conquista
    SIEGE = 4,               // Cerco
    TOTAL = 5,               // Total
    TOURNAMENT = 6,          // Torneio
    GUILD_SIEGE = 7,         // Cerco de Castelo
    TERRITORY = 8,           // Disputa de território
    CUSTOM = 9,              // Personalizada
};

/**
 * @brief Condição de Vitória
 */
enum class VictoryCondition : BYTE {
    NONE = 0,                // Nenhuma
    KILLS = 1,               // Número de kills
    POINTS = 2,              // Pontos
    OBJECTIVES = 3,          // Objetivos
    TIME_LIMIT = 4,          // Limite de tempo
    LEADER_KILL = 5,         // Matar líder
    FLAG_CAPTURE = 6,        // Capturar bandeira
    TERRITORY_CONTROL = 7,   // Controle de território
    RESOURCE_COLLECTION = 8, // Coleta de recursos
    RELIC_CAPTURE = 9,       // Capturar relíquia
    DOMINATION = 10,         // Dominação
    CUSTOM = 11,             // Personalizada
};

/**
 * @brief Condição de Derrota
 */
enum class DefeatCondition : BYTE {
    NONE = 0,                // Nenhuma
    SURRENDER = 1,           // Rendição
    ALL_DEAD = 2,            // Todos mortos
    LEADER_DEAD = 3,         // Líder morto
    TIME_EXPIRED = 4,        // Tempo expirado
    SCORE_THRESHOLD = 5,     // Limite de pontuação
    OBJECTIVE_FAILED = 6,    // Objetivo falhou
    TERRITORY_LOST = 7,      // Território perdido
    RESOURCE_EXHAUSTED = 8,  // Recursos esgotados
    RETREAT = 9,             // Retirada
    CUSTOM = 10,             // Personalizada
};

/**
 * @brief Restrições de Guerra
 */
enum class WarRestriction : BYTE {
    NONE = 0,                // Nenhuma
    NO_HEALING = 1,          // Sem cura
    NO_POTIONS = 2,          // Sem poções
    NO_MOUNTS = 3,           // Sem montarias
    NO_SKILLS = 4,           // Sem skills
    NO_RESURRECTION = 5,     // Sem ressurreição
    NO_PARTY = 6,            // Sem grupo
    LEVEL_RESTRICTED = 7,    // Restrição de nível
    CLASS_RESTRICTED = 8,    // Restrição de classe
    EQUIPMENT_RESTRICTED = 9, // Restrição de equipamento
    LOCATION_RESTRICTED = 10, // Restrição de local
    TIME_RESTRICTED = 11,    // Restrição de tempo
    CUSTOM = 12,             // Personalizada
};

/**
 * @brief Log da Guild
 */
enum class GuildLogType : BYTE {
    NONE = 0,                // Nenhum
    MEMBER_JOINED = 1,       // Membro entrou
    MEMBER_LEFT = 2,         // Membro saiu
    MEMBER_KICKED = 3,       // Membro expulso
    MEMBER_PROMOTED = 4,     // Membro promovido
    MEMBER_DEMOTED = 5,      // Membro rebaixado
    WAR_DECLARED = 6,        // Guerra declarada
    WAR_ACCEPTED = 7,        // Guerra aceita
    WAR_ENDED = 8,           // Guerra terminada
    ALLIANCE_FORMED = 9,     // Aliança formada
    ALLIANCE_BROKEN = 10,    // Aliança rompida
    TERRITORY_CLAIMED = 11,  // Território reivindicado
    TERRITORY_LOST = 12,     // Território perdido
    SKILL_LEARNED = 13,      // Skill aprendida
    SKILL_UPGRADED = 14,     // Skill melhorada
    BANK_DEPOSIT = 15,       // Depósito no banco
    BANK_WITHDRAWAL = 16,    // Saque do banco
    EMBLEM_CHANGED = 17,     // Emblema alterado
    MOTTO_CHANGED = 18,      // Lema alterado
    NOTICE_CHANGED = 19,     // Aviso alterado
    LEVEL_UP = 20,           // Subiu de nível
    DONATION = 21,           // Doação
    TAX_COLLECTED = 22,      // Taxa coletada
    EVENT_SCHEDULED = 23,    // Evento agendado
    EVENT_COMPLETED = 24,    // Evento concluído
    CUSTOM = 25,             // Personalizado
};

/**
 * @brief Status de um membro na guerra
 */
enum class WarMemberStatus : BYTE {
    INACTIVE = 0,            // Inativo
    STANDBY = 1,             // De prontidão
    ACTIVE = 2,              // Ativo
    WOUNDED = 3,             // Ferido
    DEAD = 4,                // Morto
    RETREATED = 5,           // Retirado
    DESERTED = 6,            // Desertor
    HERO = 7,                // Herói
    CUSTOM = 8,              // Personalizado
};

/**
 * @brief Tipos de eventos de guild
 */
enum class GuildEventType : BYTE {
    NONE = 0,                // Nenhum
    MEETING = 1,             // Reunião
    TRAINING = 2,            // Treinamento
    RAID = 3,                // Raid
    CEREMONY = 4,            // Cerimônia
    DEFENSE = 5,             // Defesa
    BATTLE = 6,              // Batalha
    HARVEST = 7,             // Colheita
    HUNT = 8,                // Caçada
    TOURNAMENT = 9,          // Torneio
    CELEBRATION = 10,        // Celebração
    EXPEDITION = 11,         // Expedição
    SIEGE = 12,              // Cerco
    CUSTOM = 13,             // Personalizado
};

/**
 * @brief Tipos de skills de guild
 */
enum class GuildSkillType : BYTE {
    NONE = 0,                // Nenhum
    COMBAT = 1,              // Combate
    ECONOMY = 2,             // Economia
    RESOURCES = 3,           // Recursos
    TERRITORY = 4,           // Território
    DEFENSE = 5,             // Defesa
    LOGISTICS = 6,           // Logística
    PRODUCTION = 7,          // Produção
    RESEARCH = 8,            // Pesquisa
    SOCIAL = 9,              // Social
    SPECIAL = 10,            // Especial
    CUSTOM = 11,             // Personalizado
};

/**
 * @brief Tipos de contribuições de guild
 */
enum class GuildContributionType : BYTE {
    NONE = 0,                // Nenhum
    GOLD = 1,                // Ouro
    ITEMS = 2,               // Itens
    WAR_PARTICIPATION = 3,   // Participação em guerra
    QUEST_COMPLETION = 4,    // Conclusão de quest
    TERRITORY_CAPTURE = 5,   // Captura de território
    ENEMY_KILLS = 6,         // Kills de inimigos
    RECRUITMENT = 7,         // Recrutamento
    EVENT_PARTICIPATION = 8, // Participação em evento
    DONATION = 9,            // Doação
    CUSTOM = 10,             // Personalizado
};

/**
 * @brief Tipos de recursos de território
 */
enum class TerritoryResourceType : BYTE {
    NONE = 0,                // Nenhum
    GOLD = 1,                // Ouro
    LUMBER = 2,              // Madeira
    STONE = 3,               // Pedra
    ORE = 4,                 // Minério
    HERBS = 5,               // Ervas
    FOOD = 6,                // Comida
    ENERGY = 7,              // Energia
    ESSENCE = 8,             // Essência
    CUSTOM = 9,              // Personalizado
};

/**
 * @brief Tipos de territórios
 */
enum class TerritoryType : BYTE {
    NONE = 0,                // Nenhum
    RESOURCE = 1,            // Recurso
    STRATEGIC = 2,           // Estratégico
    FORTRESS = 3,            // Fortaleza
    CASTLE = 4,              // Castelo
    VILLAGE = 5,             // Vila
    CITY = 6,                // Cidade
    MINE = 7,                // Mina
    FARM = 8,                // Fazenda
    OUTPOST = 9,             // Posto avançado
    RUINS = 10,              // Ruínas
    SACRED_SITE = 11,        // Local sagrado
    TRADING_POST = 12,       // Posto comercial
    CUSTOM = 13,             // Personalizado
};

/**
 * @brief Tipos de edifícios de território
 */
enum class TerritoryBuildingType : BYTE {
    NONE = 0,                // Nenhum
    HEADQUARTERS = 1,        // Quartel-general
    BARRACKS = 2,            // Quartel
    ARMORY = 3,              // Arsenal
    WAREHOUSE = 4,           // Armazém
    WORKSHOP = 5,            // Oficina
    LABORATORY = 6,          // Laboratório
    TOWER = 7,               // Torre
    WALL = 8,                // Muralha
    GATE = 9,                // Portão
    RESOURCE_NODE = 10,      // Nó de recurso
    TEMPLE = 11,             // Templo
    MONUMENT = 12,           // Monumento
    MARKET = 13,             // Mercado
    CUSTOM = 14,             // Personalizado
};

/**
 * @brief Status de um território
 */
enum class TerritoryStatus : BYTE {
    UNCLAIMED = 0,           // Não reivindicado
    CLAIMED = 1,             // Reivindicado
    CONTESTED = 2,           // Contestado
    SECURED = 3,             // Seguro
    DEVELOPED = 4,           // Desenvolvido
    FORTIFIED = 5,           // Fortificado
    UNDER_ATTACK = 6,        // Sob ataque
    DAMAGED = 7,             // Danificado
    DEPLETED = 8,            // Esgotado
    ABANDONED = 9,           // Abandonado
    CUSTOM = 10,             // Personalizado
};

/**
 * @brief Status de um territoório
 */
enum class TaxCollectionStatus : BYTE {
    NOT_COLLECTED = 0,       // Não coletado
    PENDING = 1,             // Pendente
    COLLECTED = 2,           // Coletado
    FAILED = 3,              // Falhou
    PARTIAL = 4,             // Parcial
    EXEMPT = 5,              // Isento
    CUSTOM = 6,              // Personalizado
};

/**
 * @brief Membro da Guild
 */
struct GuildMember {
    DWORD playerID;                           // ID do jogador
    std::string name;                         // Nome
    BYTE level;                                // Nível
    BYTE classID;                              // Classe
    GuildRank rank;                           // Rank
    time_t joinDate;                           // Data de entrada
    time_t lastOnlineDate;                     // Última vez online
    bool online;                               // Se está online
    DWORD contributionTotal;                  // Contribuição total
    DWORD contributionWeekly;                 // Contribuição semanal
    DWORD killsTotal;                         // Kills totais
    DWORD deathsTotal;                        // Mortes totais
    DWORD killsInWar;                         // Kills em guerra
    DWORD deathsInWar;                        // Mortes em guerra
    BYTE warParticipationCount;               // Contagem de participação em guerra
    std::bitset<32> permissions;               // Permissões
    time_t lastPromotionDate;                  // Data da última promoção
    BYTE demeritPoints;                       // Pontos de demérito
    WarMemberStatus warStatus;                // Status na guerra
    std::string note;                          // Nota
    
    GuildMember()
        : playerID(0)
        , name("")
        , level(0)
        , classID(0)
        , rank(GuildRank::MEMBER)
        , joinDate(0)
        , lastOnlineDate(0)
        , online(false)
        , contributionTotal(0)
        , contributionWeekly(0)
        , killsTotal(0)
        , deathsTotal(0)
        , killsInWar(0)
        , deathsInWar(0)
        , warParticipationCount(0)
        , lastPromotionDate(0)
        , demeritPoints(0)
        , warStatus(WarMemberStatus::INACTIVE)
        , note("")
    {
        permissions.reset();
    }
};

/**
 * @brief Rank personalizado da Guild
 */
struct GuildCustomRank {
    BYTE rankID;                               // ID do rank
    std::string name;                          // Nome
    std::bitset<32> permissions;               // Permissões
    BYTE order;                                // Ordem (posição na hierarquia)
    
    GuildCustomRank()
        : rankID(0)
        , name("")
        , order(0)
    {
        permissions.reset();
    }
};

/**
 * @brief Entrada de log da Guild
 */
struct GuildLogEntry {
    DWORD logID;                              // ID do log
    GuildLogType type;                        // Tipo
    time_t timestamp;                          // Timestamp
    DWORD actorID;                            // ID do ator
    DWORD targetID;                           // ID do alvo
    std::string actorName;                     // Nome do ator
    std::string targetName;                    // Nome do alvo
    std::string description;                   // Descrição
    
    GuildLogEntry()
        : logID(0)
        , type(GuildLogType::NONE)
        , timestamp(0)
        , actorID(0)
        , targetID(0)
        , actorName("")
        , targetName("")
        , description("")
    {
    }
};

/**
 * @brief Item do banco da Guild
 */
struct GuildBankItem {
    DWORD slotID;                             // ID do slot
    WORD itemID;                              // ID do item
    WORD quantity;                            // Quantidade
    BYTE quality;                             // Qualidade
    BYTE enchantLevel;                        // Nível de encantamento
    BYTE bound;                               // Vinculado
    DWORD timestamp;                          // Timestamp
    DWORD playerID;                           // ID do jogador que depositou
    std::string playerName;                    // Nome do jogador que depositou
    std::string note;                          // Nota
    
    GuildBankItem()
        : slotID(0)
        , itemID(0)
        , quantity(0)
        , quality(0)
        , enchantLevel(0)
        , bound(0)
        , timestamp(0)
        , playerID(0)
        , playerName("")
        , note("")
    {
    }
};

/**
 * @brief Skill da Guild
 */
struct GuildSkill {
    WORD skillID;                             // ID da skill
    std::string name;                          // Nome
    std::string description;                   // Descrição
    GuildSkillType type;                      // Tipo
    BYTE level;                               // Nível
    BYTE maxLevel;                            // Nível máximo
    DWORD experienceRequired;                 // Experiência necessária
    DWORD goldCost;                           // Custo em ouro
    DWORD resourceCost;                       // Custo em recursos
    DWORD cooldown;                           // Cooldown (ms)
    DWORD duration;                           // Duração (ms)
    BYTE passive;                             // Se é passiva
    std::string effects;                       // Efeitos
    
    GuildSkill()
        : skillID(0)
        , name("")
        , description("")
        , type(GuildSkillType::NONE)
        , level(0)
        , maxLevel(10)
        , experienceRequired(0)
        , goldCost(0)
        , resourceCost(0)
        , cooldown(0)
        , duration(0)
        , passive(0)
        , effects("")
    {
    }
};

/**
 * @brief Aliança entre Guilds
 */
struct GuildAlliance {
    WORD allianceID;                          // ID da aliança
    GuildID guildID1;                         // ID da guild 1
    GuildID guildID2;                         // ID da guild 2
    AllianceLevel level;                      // Nível de aliança
    time_t formationDate;                      // Data de formação
    time_t expiryDate;                         // Data de expiração
    std::string terms;                         // Termos
    bool active;                               // Se está ativa
    
    GuildAlliance()
        : allianceID(0)
        , guildID1(0)
        , guildID2(0)
        , level(AllianceLevel::NONE)
        , formationDate(0)
        , expiryDate(0)
        , terms("")
        , active(false)
    {
    }
};

/**
 * @brief Guerra entre Guilds
 */
struct GuildWar {
    WORD warID;                               // ID da guerra
    GuildID attackerID;                       // ID do atacante
    GuildID defenderID;                       // ID do defensor
    std::string attackerName;                  // Nome do atacante
    std::string defenderName;                  // Nome do defensor
    WarState state;                           // Estado
    WarType type;                             // Tipo
    time_t declarationDate;                    // Data da declaração
    time_t startDate;                          // Data de início
    time_t endDate;                            // Data de fim
    time_t expiryDate;                         // Data de expiração
    DWORD attackerScore;                      // Pontuação do atacante
    DWORD defenderScore;                      // Pontuação do defensor
    DWORD attackerKills;                      // Kills do atacante
    DWORD defenderKills;                      // Kills do defensor
    DWORD attackerDeaths;                     // Mortes do atacante
    DWORD defenderDeaths;                     // Mortes do defensor
    DWORD locationMapID;                      // ID do mapa da localização
    WORD locationX;                           // Coordenada X da localização
    WORD locationY;                           // Coordenada Y da localização
    VictoryCondition victoryCondition;        // Condição de vitória
    DefeatCondition defeatCondition;          // Condição de derrota
    DWORD victoryValue;                       // Valor de vitória
    DWORD defeatValue;                        // Valor de derrota
    DWORD goldWager;                          // Aposta em ouro
    DWORD territoryID;                        // ID do território em disputa
    BYTE winner;                              // Vencedor (0=nenhum, 1=atacante, 2=defensor)
    std::string terms;                         // Termos
    std::vector<WarRestriction> restrictions;  // Restrições
    BYTE minLevel;                            // Nível mínimo
    BYTE maxLevel;                            // Nível máximo
    
    GuildWar()
        : warID(0)
        , attackerID(0)
        , defenderID(0)
        , attackerName("")
        , defenderName("")
        , state(WarState::NONE)
        , type(WarType::NONE)
        , declarationDate(0)
        , startDate(0)
        , endDate(0)
        , expiryDate(0)
        , attackerScore(0)
        , defenderScore(0)
        , attackerKills(0)
        , defenderKills(0)
        , attackerDeaths(0)
        , defenderDeaths(0)
        , locationMapID(0)
        , locationX(0)
        , locationY(0)
        , victoryCondition(VictoryCondition::NONE)
        , defeatCondition(DefeatCondition::NONE)
        , victoryValue(0)
        , defeatValue(0)
        , goldWager(0)
        , territoryID(0)
        , winner(0)
        , terms("")
        , minLevel(0)
        , maxLevel(0)
    {
    }
};

/**
 * @brief Entrada no histórico de guerras
 */
struct GuildWarHistoryEntry {
    WORD entryID;                             // ID da entrada
    WORD warID;                               // ID da guerra
    GuildID guildID1;                         // ID da guild 1
    GuildID guildID2;                         // ID da guild 2
    std::string guild1Name;                    // Nome da guild 1
    std::string guild2Name;                    // Nome da guild 2
    WarType type;                             // Tipo
    time_t startDate;                          // Data de início
    time_t endDate;                            // Data de fim
    BYTE winner;                              // Vencedor (0=nenhum, 1=guild1, 2=guild2)
    DWORD guild1Score;                        // Pontuação da guild 1
    DWORD guild2Score;                        // Pontuação da guild 2
    DWORD guild1Kills;                        // Kills da guild 1
    DWORD guild2Kills;                        // Kills da guild 2
    std::string result;                        // Resultado
    
    GuildWarHistoryEntry()
        : entryID(0)
        , warID(0)
        , guildID1(0)
        , guildID2(0)
        , guild1Name("")
        , guild2Name("")
        , type(WarType::NONE)
        , startDate(0)
        , endDate(0)
        , winner(0)
        , guild1Score(0)
        , guild2Score(0)
        , guild1Kills(0)
        , guild2Kills(0)
        , result("")
    {
    }
};

/**
 * @brief Evento da Guild
 */
struct GuildEvent {
    WORD eventID;                             // ID do evento
    GuildEventType type;                      // Tipo
    std::string name;                          // Nome
    std::string description;                   // Descrição
    time_t scheduleDate;                       // Data agendada
    time_t endDate;                            // Data de fim
    DWORD durationMinutes;                    // Duração em minutos
    DWORD organizerID;                        // ID do organizador
    std::string organizerName;                 // Nome do organizador
    DWORD locationMapID;                      // ID do mapa da localização
    WORD locationX;                           // Coordenada X da localização
    WORD locationY;                           // Coordenada Y da localização
    BYTE recurring;                           // Se é recorrente
    BYTE minLevel;                            // Nível mínimo
    BYTE maxLevel;                            // Nível máximo
    BYTE requiredRank;                        // Rank exigido
    BYTE requiredWarStatus;                   // Status de guerra exigido
    BYTE maxParticipants;                     // Máximo de participantes
    std::vector<DWORD> participants;           // IDs dos participantes
    
    GuildEvent()
        : eventID(0)
        , type(GuildEventType::NONE)
        , name("")
        , description("")
        , scheduleDate(0)
        , endDate(0)
        , durationMinutes(0)
        , organizerID(0)
        , organizerName("")
        , locationMapID(0)
        , locationX(0)
        , locationY(0)
        , recurring(0)
        , minLevel(0)
        , maxLevel(0)
        , requiredRank(0)
        , requiredWarStatus(0)
        , maxParticipants(0)
    {
    }
};

/**
 * @brief Território da Guild
 */
struct GuildTerritory {
    WORD territoryID;                         // ID do território
    std::string name;                          // Nome
    TerritoryType type;                       // Tipo
    TerritoryStatus status;                   // Status
    GuildID ownerID;                          // ID do dono
    time_t claimDate;                          // Data de reivindicação
    DWORD locationMapID;                      // ID do mapa da localização
    WORD locationX;                           // Coordenada X da localização
    WORD locationY;                           // Coordenada Y da localização
    WORD radius;                              // Raio
    DWORD resourceQuantity;                   // Quantidade de recursos
    TerritoryResourceType resourceType;       // Tipo de recurso
    DWORD productionRate;                     // Taxa de produção
    DWORD maxStorage;                         // Armazenamento máximo
    DWORD currentStorage;                     // Armazenamento atual
    BYTE level;                               // Nível
    BYTE maxLevel;                            // Nível máximo
    DWORD upgradeGoldCost;                    // Custo de melhoria em ouro
    DWORD upgradeResourceCost;                // Custo de melhoria em recursos
    DWORD defenseLevel;                       // Nível de defesa
    DWORD maxDefenseLevel;                    // Nível máximo de defesa
    DWORD lastCollectionTime;                 // Hora da última coleta
    DWORD collectionInterval;                 // Intervalo de coleta (ms)
    std::vector<TerritoryBuildingType> buildings; // Edifícios construídos
    
    GuildTerritory()
        : territoryID(0)
        , name("")
        , type(TerritoryType::NONE)
        , status(TerritoryStatus::UNCLAIMED)
        , ownerID(0)
        , claimDate(0)
        , locationMapID(0)
        , locationX(0)
        , locationY(0)
        , radius(0)
        , resourceQuantity(0)
        , resourceType(TerritoryResourceType::NONE)
        , productionRate(0)
        , maxStorage(0)
        , currentStorage(0)
        , level(1)
        , maxLevel(10)
        , upgradeGoldCost(0)
        , upgradeResourceCost(0)
        , defenseLevel(0)
        , maxDefenseLevel(10)
        , lastCollectionTime(0)
        , collectionInterval(86400000) // 24 horas
    {
    }
};

/**
 * @brief Info de Guild
 */
struct GuildInfo {
    GuildID id;                               // ID
    std::string name;                          // Nome
    std::string motto;                         // Lema
    std::string notice;                        // Aviso
    time_t creationDate;                       // Data de criação
    DWORD founderID;                          // ID do fundador
    std::string founderName;                   // Nome do fundador
    DWORD masterID;                           // ID do mestre
    std::string masterName;                    // Nome do mestre
    GuildLevel level;                         // Nível
    DWORD experience;                         // Experiência
    DWORD experienceToNextLevel;              // Experiência para o próximo nível
    DWORD gold;                               // Ouro
    GuildEmblem emblem;                       // Emblema
    BYTE memberCount;                         // Contagem de membros
    BYTE maxMemberCount;                      // Contagem máxima de membros
    BYTE onlineMemberCount;                   // Contagem de membros online
    WORD totalWarVictories;                   // Total de vitórias em guerra
    WORD totalWarDefeats;                     // Total de derrotas em guerra
    WORD totalWarDraws;                       // Total de empates em guerra
    WORD activeWarCount;                      // Contagem de guerras ativas
    WORD allianceCount;                       // Contagem de alianças
    WORD territoryCount;                      // Contagem de territórios
    BYTE taxRate;                             // Taxa
    DWORD weeklyTaxCollection;                // Coleta de taxa semanal
    DWORD totalTaxCollection;                 // Coleta de taxa total
    DWORD lastActivityTime;                   // Hora da última atividade
    GuildCustomRank customRanks[MAX_GUILD_RANKS]; // Ranks personalizados
    
    GuildInfo()
        : id(0)
        , name("")
        , motto("")
        , notice("")
        , creationDate(0)
        , founderID(0)
        , founderName("")
        , masterID(0)
        , masterName("")
        , level(GuildLevel::NOVICE)
        , experience(0)
        , experienceToNextLevel(0)
        , gold(0)
        , memberCount(0)
        , maxMemberCount(MAX_GUILD_MEMBERS)
        , onlineMemberCount(0)
        , totalWarVictories(0)
        , totalWarDefeats(0)
        , totalWarDraws(0)
        , activeWarCount(0)
        , allianceCount(0)
        , territoryCount(0)
        , taxRate(0)
        , weeklyTaxCollection(0)
        , totalTaxCollection(0)
        , lastActivityTime(0)
    {
    }
};

/**
 * @brief Evento de Guild
 */
enum class GuildSystemEventType : BYTE {
    NONE = 0,                // Nenhum
    GUILD_CREATED = 1,       // Guild criada
    GUILD_DISBANDED = 2,     // Guild dissolvida
    GUILD_LEVELED_UP = 3,    // Guild subiu de nível
    WAR_DECLARED = 4,        // Guerra declarada
    WAR_ACCEPTED = 5,        // Guerra aceita
    WAR_ENDED = 6,           // Guerra encerrada
    TERRITORY_CLAIMED = 7,   // Território reivindicado
    TERRITORY_LOST = 8,      // Território perdido
    ALLIANCE_FORMED = 9,     // Aliança formada
    ALLIANCE_BROKEN = 10,    // Aliança rompida
    MEMBER_JOINED = 11,      // Membro entrou
    MEMBER_LEFT = 12,        // Membro saiu
    MEMBER_KICKED = 13,      // Membro expulso
    MEMBER_PROMOTED = 14,    // Membro promovido
    MEMBER_DEMOTED = 15,     // Membro rebaixado
    SKILL_LEARNED = 16,      // Skill aprendida
    SIEGE_STARTED = 17,      // Cerco iniciado
    SIEGE_ENDED = 18,        // Cerco encerrado
    CASTLE_CAPTURED = 19,    // Castelo capturado
    CASTLE_LOST = 20,        // Castelo perdido
    CUSTOM = 21,             // Personalizado
};

/**
 * @brief Callback para eventos do sistema de guild
 */
using GuildSystemEventCallback = std::function<void(GuildSystemEventType, const void*)>;

// Final de namespace
} // namespace guild
} // namespace server
} // namespace wyd

#endif // GUILDTYPES_H

} // namespace wydbr
