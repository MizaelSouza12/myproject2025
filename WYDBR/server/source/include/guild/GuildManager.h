/**
 * GuildManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/guild/GuildManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _GUILD_MANAGER_H_
#define _GUILD_MANAGER_H_

/**
 * @file GuildManager.h
 * @brief Gerenciador de guildas do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de guildas do WYDBR,
 * responsável por criar, gerenciar e controlar as guildas do jogo,
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

#include "../GlobalDef.h"
#include "../Basedef.h"

namespace wydbr {
namespace guild {

/**
 * @brief Estado da guilda
 */
enum /**
 * Classe WYDGuildState
 * 
 * Esta classe implementa a funcionalidade WYDGuildState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildState {
    NONE = 0,              // Nenhum
    ACTIVE = 1,            // Ativa
    INACTIVE = 2,          // Inativa
    DISBANDED = 3,         // Dissolvida
    PENDING = 4,           // Pendente
    BLOCKED = 5,           // Bloqueada
    DELETED = 6,           // Deletada
    CUSTOM_1 = 7,          // Personalizado 1
    CUSTOM_2 = 8,          // Personalizado 2
    CUSTOM_3 = 9,          // Personalizado 3
    UNKNOWN = 10           // Desconhecido
};

/**
 * @brief Nível de membro na guilda
 */
enum /**
 * Classe WYDGuildMemberLevel
 * 
 * Esta classe implementa a funcionalidade WYDGuildMemberLevel conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildMemberLevel {
    NONE = 0,              // Nenhum
    MEMBER = 1,            // Membro
    SENIOR = 2,            // Sênior
    CAPTAIN = 3,           // Capitão
    SUBMASTER = 4,         // Sub-mestre
    MASTER = 5,            // Mestre
    CUSTOM_1 = 6,          // Personalizado 1
    CUSTOM_2 = 7,          // Personalizado 2
    CUSTOM_3 = 8,          // Personalizado 3
    UNKNOWN = 9            // Desconhecido
};

/**
 * @brief Estado de guerra de guilda
 */
enum /**
 * Classe WYDGuildWarState
 * 
 * Esta classe implementa a funcionalidade WYDGuildWarState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildWarState {
    NONE = 0,              // Nenhum
    PENDING = 1,           // Pendente
    ACTIVE = 2,            // Ativa
    ENDED = 3,             // Finalizada
    SURRENDERED = 4,       // Rendição
    REFUSED = 5,           // Recusada
    CANCELED = 6,          // Cancelada
    CUSTOM_1 = 7,          // Personalizado 1
    CUSTOM_2 = 8,          // Personalizado 2
    CUSTOM_3 = 9,          // Personalizado 3
    UNKNOWN = 10           // Desconhecido
};

/**
 * @brief Tipo de aliança de guilda
 */
enum /**
 * Classe WYDGuildAllianceType
 * 
 * Esta classe implementa a funcionalidade WYDGuildAllianceType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildAllianceType {
    NONE = 0,              // Nenhum
    ALLY = 1,              // Aliado
    NEUTRAL = 2,           // Neutro
    ENEMY = 3,             // Inimigo
    VASSAL = 4,            // Vassalo
    SUZERAIN = 5,          // Suserano
    CUSTOM_1 = 6,          // Personalizado 1
    CUSTOM_2 = 7,          // Personalizado 2
    CUSTOM_3 = 8,          // Personalizado 3
    UNKNOWN = 9            // Desconhecido
};

/**
 * @brief Tipos de eventos de guilda
 */
enum /**
 * Classe WYDGuildEventType
 * 
 * Esta classe implementa a funcionalidade WYDGuildEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildEventType {
    NONE = 0,              // Nenhum
    CREATED = 1,           // Criada
    DISBANDED = 2,         // Dissolvida
    MEMBER_JOINED = 3,     // Membro entrou
    MEMBER_LEFT = 4,       // Membro saiu
    MEMBER_KICKED = 5,     // Membro expulso
    MEMBER_PROMOTED = 6,   // Membro promovido
    MEMBER_DEMOTED = 7,    // Membro rebaixado
    LEADER_CHANGED = 8,    // Líder alterado
    NOTICE_CHANGED = 9,    // Aviso alterado
    MARK_CHANGED = 10,     // Marca alterada
    ALLIANCE_CREATED = 11, // Aliança criada
    ALLIANCE_BROKEN = 12,  // Aliança quebrada
    WAR_DECLARED = 13,     // Guerra declarada
    WAR_ACCEPTED = 14,     // Guerra aceita
    WAR_REFUSED = 15,      // Guerra recusada
    WAR_SURRENDERED = 16,  // Guerra rendida
    WAR_ENDED = 17,        // Guerra finalizada
    LEVEL_UP = 18,         // Subiu de nível
    TERRITORY_GAINED = 19, // Território ganho
    TERRITORY_LOST = 20,   // Território perdido
    DONATION = 21,         // Doação
    CUSTOM_1 = 22,         // Personalizado 1
    CUSTOM_2 = 23,         // Personalizado 2
    CUSTOM_3 = 24,         // Personalizado 3
    UNKNOWN = 25           // Desconhecido
};

/**
 * @brief Membro da guilda
 */
struct GuildMember {
    uint32_t MemberID;             // ID do membro
    uint32_t CharacterID;          // ID do personagem
    std::string Name;              // Nome
    uint8_t Level;                 // Nível
    uint8_t Class;                 // Classe
    GuildMemberLevel GuildLevel;   // Nível na guilda
    uint32_t Contribution;         // Contribuição
    uint32_t JoinTime;             // Tempo de entrada
    uint32_t LastLoginTime;        // Último login
    uint32_t LastLogoutTime;       // Último logout
    bool Online;                   // Online
    std::string Note;              // Observação
    
    GuildMember() : MemberID(0), CharacterID(0), Level(0), Class(0),
        GuildLevel(GuildMemberLevel::MEMBER), Contribution(0), JoinTime(0),
        LastLoginTime(0), LastLogoutTime(0), Online(false) {}
};

/**
 * @brief Histórico da guilda
 */
struct GuildHistory {
    uint32_t EventID;              // ID do evento
    GuildEventType Type;           // Tipo
    uint32_t Timestamp;            // Timestamp
    uint32_t ActorID;              // ID do ator
    uint32_t TargetID;             // ID do alvo
    uint32_t Data1;                // Dado 1
    uint32_t Data2;                // Dado 2
    std::string Description;       // Descrição
    
    GuildHistory() : EventID(0), Type(GuildEventType::NONE), Timestamp(0),
        ActorID(0), TargetID(0), Data1(0), Data2(0) {}
};

/**
 * @brief Aliança de guilda
 */
struct GuildAlliance {
    uint32_t AllianceID;           // ID da aliança
    uint32_t Guild1ID;             // ID da guilda 1
    uint32_t Guild2ID;             // ID da guilda 2
    GuildAllianceType Type;        // Tipo
    uint32_t StartTime;            // Tempo de início
    uint32_t EndTime;              // Tempo de fim
    std::string Description;       // Descrição
    
    GuildAlliance() : AllianceID(0), Guild1ID(0), Guild2ID(0),
        Type(GuildAllianceType::NONE), StartTime(0), EndTime(0) {}
};

/**
 * @brief Guerra de guilda
 */
struct GuildWar {
    uint32_t WarID;                // ID da guerra
    uint32_t Attacker;             // Atacante
    uint32_t Defender;             // Defensor
    GuildWarState State;           // Estado
    uint32_t StartTime;            // Tempo de início
    uint32_t EndTime;              // Tempo de fim
    uint32_t Duration;             // Duração
    uint32_t WinnerID;             // ID do vencedor
    uint32_t AttackerScore;        // Pontuação do atacante
    uint32_t DefenderScore;        // Pontuação do defensor
    uint32_t AttackerKills;        // Abates do atacante
    uint32_t DefenderKills;        // Abates do defensor
    std::string Description;       // Descrição
    
    GuildWar() : WarID(0), Attacker(0), Defender(0), State(GuildWarState::NONE),
        StartTime(0), EndTime(0), Duration(0), WinnerID(0), AttackerScore(0),
        DefenderScore(0), AttackerKills(0), DefenderKills(0) {}
};

/**
 * @brief Definição da guilda
 */
struct GuildDefinition {
    uint32_t GuildID;              // ID da guilda
    std::string Name;              // Nome
    std::string Mark;              // Marca
    std::string Notice;            // Aviso
    uint32_t LeaderID;             // ID do líder
    uint8_t Level;                 // Nível
    uint32_t Experience;           // Experiência
    uint32_t Gold;                 // Ouro
    uint32_t Score;                // Pontuação
    uint32_t Reputation;           // Reputação
    uint32_t Territory;            // Território
    uint32_t CreationTime;         // Tempo de criação
    uint32_t LastActivityTime;     // Último tempo de atividade
    uint32_t TotalMembers;         // Total de membros
    uint32_t OnlineMembers;        // Membros online
    uint32_t MaxMembers;           // Máximo de membros
    GuildState State;              // Estado
    std::vector<GuildMember> Members; // Membros
    std::vector<GuildHistory> History; // Histórico
    
    GuildDefinition() : GuildID(0), LeaderID(0), Level(1), Experience(0),
        Gold(0), Score(0), Reputation(0), Territory(0), CreationTime(0),
        LastActivityTime(0), TotalMembers(0), OnlineMembers(0), MaxMembers(0),
        State(GuildState::ACTIVE) {}
};

/**
 * @brief Guerra de guilda ativa
 */
struct ActiveGuildWar {
    uint32_t WarID;                // ID da guerra
    uint32_t Attacker;             // Atacante
    uint32_t Defender;             // Defensor
    uint32_t StartTime;            // Tempo de início
    uint32_t EndTime;              // Tempo de fim
    uint32_t Duration;             // Duração
    uint32_t AttackerScore;        // Pontuação do atacante
    uint32_t DefenderScore;        // Pontuação do defensor
    uint32_t AttackerKills;        // Abates do atacante
    uint32_t DefenderKills;        // Abates do defensor
    
    ActiveGuildWar() : WarID(0), Attacker(0), Defender(0), StartTime(0),
        EndTime(0), Duration(0), AttackerScore(0), DefenderScore(0),
        AttackerKills(0), DefenderKills(0) {}
};

/**
 * @brief Evento de guilda
 */
struct GuildEvent {
    uint32_t EventID;              // ID do evento
    GuildEventType Type;           // Tipo
    uint32_t GuildID;              // ID da guilda
    uint32_t ActorID;              // ID do ator
    uint32_t TargetID;             // ID do alvo
    uint32_t Data1;                // Dado 1
    uint32_t Data2;                // Dado 2
    uint32_t Timestamp;            // Timestamp
    
    GuildEvent() : EventID(0), Type(GuildEventType::NONE), GuildID(0),
        ActorID(0), TargetID(0), Data1(0), Data2(0), Timestamp(0) {}
    
    GuildEvent(GuildEventType _type, uint32_t _guildID, uint32_t _actorID = 0,
               uint32_t _targetID = 0, uint32_t _data1 = 0, uint32_t _data2 = 0)
        : EventID(0), Type(_type), GuildID(_guildID), ActorID(_actorID),
          TargetID(_targetID), Data1(_data1), Data2(_data2),
          Timestamp(static_cast<uint32_t>(time(nullptr))) {}
};

/**
 * @brief Tipo de callback de evento de guilda
 */
using GuildEventCallback = std::function<void(const GuildEvent&)>;

/**
 * @brief Gerenciador de guildas
 * 
 * Esta classe é responsável por gerenciar as guildas do jogo,
 * incluindo criação, modificação, dissolução, guerra, aliança, etc.
 */
/**
 * Classe WYDGuildManager
 * 
 * Esta classe implementa a funcionalidade WYDGuildManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static GuildManager& getInstance() {
        static GuildManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
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
     * @brief Carrega as guildas
     * @return true se carregado com sucesso
     */
    bool loadGuilds();
    
    /**
     * @brief Salva as guildas
     * @return true se salvo com sucesso
     */
    bool saveGuilds();
    
    /**
     * @brief Cria uma guilda
     * @param name Nome
     * @param leaderID ID do líder
     * @param notice Aviso
     * @param guildID ID da guilda (saída)
     * @return true se criado com sucesso
     */
    bool createGuild(const std::string& name, uint32_t leaderID, const std::string& notice, uint32_t& guildID);
    
    /**
     * @brief Dissolve uma guilda
     * @param guildID ID da guilda
     * @param byAdmin Dissolvido por administrador
     * @return true se dissolvido com sucesso
     */
    bool disbandGuild(uint32_t guildID, bool byAdmin = false);
    
    /**
     * @brief Adiciona um membro a uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param name Nome
     * @param level Nível
     * @param classID Classe
     * @param guildLevel Nível na guilda
     * @return true se adicionado com sucesso
     */
    bool addMember(uint32_t guildID, uint32_t characterID, const std::string& name, uint8_t level, uint8_t classID, GuildMemberLevel guildLevel = GuildMemberLevel::MEMBER);
    
    /**
     * @brief Remove um membro de uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param kicked Expulso
     * @param byLeader Pelo líder
     * @return true se removido com sucesso
     */
    bool removeMember(uint32_t guildID, uint32_t characterID, bool kicked = false, bool byLeader = false);
    
    /**
     * @brief Atualiza um membro de uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param level Nível
     * @param classID Classe
     * @param contribution Contribuição
     * @return true se atualizado com sucesso
     */
    bool updateMember(uint32_t guildID, uint32_t characterID, uint8_t level, uint8_t classID, uint32_t contribution = 0);
    
    /**
     * @brief Define o status online de um membro
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param online Online
     * @return true se definido com sucesso
     */
    bool setMemberOnline(uint32_t guildID, uint32_t characterID, bool online);
    
    /**
     * @brief Promove um membro de uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param newLevel Novo nível
     * @return true se promovido com sucesso
     */
    bool promoteMember(uint32_t guildID, uint32_t characterID, GuildMemberLevel newLevel);
    
    /**
     * @brief Rebaixa um membro de uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param newLevel Novo nível
     * @return true se rebaixado com sucesso
     */
    bool demoteMember(uint32_t guildID, uint32_t characterID, GuildMemberLevel newLevel);
    
    /**
     * @brief Altera o líder de uma guilda
     * @param guildID ID da guilda
     * @param newLeaderID ID do novo líder
     * @return true se alterado com sucesso
     */
    bool changeLeader(uint32_t guildID, uint32_t newLeaderID);
    
    /**
     * @brief Altera o aviso de uma guilda
     * @param guildID ID da guilda
     * @param notice Aviso
     * @return true se alterado com sucesso
     */
    bool updateNotice(uint32_t guildID, const std::string& notice);
    
    /**
     * @brief Altera a marca de uma guilda
     * @param guildID ID da guilda
     * @param mark Marca
     * @return true se alterado com sucesso
     */
    bool updateMark(uint32_t guildID, const std::string& mark);
    
    /**
     * @brief Adiciona experiência a uma guilda
     * @param guildID ID da guilda
     * @param exp Experiência
     * @return true se adicionado com sucesso
     */
    bool addExperience(uint32_t guildID, uint32_t exp);
    
    /**
     * @brief Adiciona ouro a uma guilda
     * @param guildID ID da guilda
     * @param gold Ouro
     * @return true se adicionado com sucesso
     */
    bool addGold(uint32_t guildID, uint32_t gold);
    
    /**
     * @brief Remove ouro de uma guilda
     * @param guildID ID da guilda
     * @param gold Ouro
     * @return true se removido com sucesso
     */
    bool removeGold(uint32_t guildID, uint32_t gold);
    
    /**
     * @brief Adiciona pontuação a uma guilda
     * @param guildID ID da guilda
     * @param score Pontuação
     * @return true se adicionado com sucesso
     */
    bool addScore(uint32_t guildID, uint32_t score);
    
    /**
     * @brief Adiciona reputação a uma guilda
     * @param guildID ID da guilda
     * @param reputation Reputação
     * @return true se adicionado com sucesso
     */
    bool addReputation(uint32_t guildID, uint32_t reputation);
    
    /**
     * @brief Adiciona território a uma guilda
     * @param guildID ID da guilda
     * @param territory Território
     * @return true se adicionado com sucesso
     */
    bool addTerritory(uint32_t guildID, uint32_t territory);
    
    /**
     * @brief Remove território de uma guilda
     * @param guildID ID da guilda
     * @param territory Território
     * @return true se removido com sucesso
     */
    bool removeTerritory(uint32_t guildID, uint32_t territory);
    
    /**
     * @brief Cria uma aliança entre guildas
     * @param guild1ID ID da guilda 1
     * @param guild2ID ID da guilda 2
     * @param type Tipo
     * @param allianceID ID da aliança (saída)
     * @return true se criado com sucesso
     */
    bool createAlliance(uint32_t guild1ID, uint32_t guild2ID, GuildAllianceType type, uint32_t& allianceID);
    
    /**
     * @brief Remove uma aliança
     * @param allianceID ID da aliança
     * @return true se removido com sucesso
     */
    bool removeAlliance(uint32_t allianceID);
    
    /**
     * @brief Declara guerra a uma guilda
     * @param attackerID ID do atacante
     * @param defenderID ID do defensor
     * @param warID ID da guerra (saída)
     * @return true se declarado com sucesso
     */
    bool declareWar(uint32_t attackerID, uint32_t defenderID, uint32_t& warID);
    
    /**
     * @brief Aceita uma guerra
     * @param warID ID da guerra
     * @return true se aceito com sucesso
     */
    bool acceptWar(uint32_t warID);
    
    /**
     * @brief Recusa uma guerra
     * @param warID ID da guerra
     * @return true se recusado com sucesso
     */
    bool refuseWar(uint32_t warID);
    
    /**
     * @brief Rende-se em uma guerra
     * @param warID ID da guerra
     * @param guildID ID da guilda que se rendeu
     * @return true se rendido com sucesso
     */
    bool surrenderWar(uint32_t warID, uint32_t guildID);
    
    /**
     * @brief Finaliza uma guerra
     * @param warID ID da guerra
     * @param winnerID ID do vencedor
     * @return true se finalizado com sucesso
     */
    bool endWar(uint32_t warID, uint32_t winnerID);
    
    /**
     * @brief Adiciona pontuação a uma guerra
     * @param warID ID da guerra
     * @param guildID ID da guilda
     * @param score Pontuação
     * @return true se adicionado com sucesso
     */
    bool addWarScore(uint32_t warID, uint32_t guildID, uint32_t score);
    
    /**
     * @brief Adiciona abate a uma guerra
     * @param warID ID da guerra
     * @param guildID ID da guilda
     * @param count Contagem
     * @return true se adicionado com sucesso
     */
    bool addWarKill(uint32_t warID, uint32_t guildID, uint32_t count = 1);
    
    /**
     * @brief Obtém uma guilda pelo ID
     * @param guildID ID da guilda
     * @return Definição da guilda
     */
    const GuildDefinition& getGuild(uint32_t guildID) const;
    
    /**
     * @brief Obtém uma guilda pelo nome
     * @param name Nome
     * @return Definição da guilda
     */
    const GuildDefinition& getGuildByName(const std::string& name) const;
    
    /**
     * @brief Obtém um membro de uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @return Membro da guilda
     */
    const GuildMember& getMember(uint32_t guildID, uint32_t characterID) const;
    
    /**
     * @brief Obtém uma aliança pelo ID
     * @param allianceID ID da aliança
     * @return Aliança da guilda
     */
    const GuildAlliance& getAlliance(uint32_t allianceID) const;
    
    /**
     * @brief Obtém uma guerra pelo ID
     * @param warID ID da guerra
     * @return Guerra da guilda
     */
    const GuildWar& getWar(uint32_t warID) const;
    
    /**
     * @brief Obtém as alianças de uma guilda
     * @param guildID ID da guilda
     * @param type Tipo
     * @return Lista de alianças
     */
    std::vector<GuildAlliance> getGuildAlliances(uint32_t guildID, GuildAllianceType type = GuildAllianceType::NONE) const;
    
    /**
     * @brief Obtém as guerras de uma guilda
     * @param guildID ID da guilda
     * @param state Estado
     * @return Lista de guerras
     */
    std::vector<GuildWar> getGuildWars(uint32_t guildID, GuildWarState state = GuildWarState::NONE) const;
    
    /**
     * @brief Obtém o histórico de uma guilda
     * @param guildID ID da guilda
     * @param count Contagem
     * @return Lista de histórico
     */
    std::vector<GuildHistory> getGuildHistory(uint32_t guildID, uint32_t count = 10) const;
    
    /**
     * @brief Verifica se um personagem está em uma guilda
     * @param characterID ID do personagem
     * @return true se estiver em uma guilda
     */
    bool isCharacterInGuild(uint32_t characterID) const;
    
    /**
     * @brief Obtém a guilda de um personagem
     * @param characterID ID do personagem
     * @return ID da guilda ou 0 se não estiver em uma guilda
     */
    uint32_t getCharacterGuild(uint32_t characterID) const;
    
    /**
     * @brief Verifica se um personagem é o líder de uma guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se for o líder
     */
    bool isGuildLeader(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem é um oficial de uma guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se for um oficial
     */
    bool isGuildOfficer(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem pode convidar para uma guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se puder convidar
     */
    bool canInviteToGuild(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem pode expulsar de uma guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se puder expulsar
     */
    bool canKickFromGuild(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem pode promover em uma guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se puder promover
     */
    bool canPromoteInGuild(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem pode rebaixar em uma guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se puder rebaixar
     */
    bool canDemoteInGuild(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem pode alterar o aviso de uma guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se puder alterar o aviso
     */
    bool canChangeGuildNotice(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem pode alterar a marca de uma guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se puder alterar a marca
     */
    bool canChangeGuildMark(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem pode declarar guerra
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se puder declarar guerra
     */
    bool canDeclareWar(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem pode aceitar guerra
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se puder aceitar guerra
     */
    bool canAcceptWar(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se um personagem pode se render em guerra
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @return true se puder se render
     */
    bool canSurrenderWar(uint32_t characterID, uint32_t guildID) const;
    
    /**
     * @brief Verifica se uma guilda pode ser criada
     * @param name Nome
     * @param leaderID ID do líder
     * @return true se puder ser criada
     */
    bool canCreateGuild(const std::string& name, uint32_t leaderID) const;
    
    /**
     * @brief Verifica se duas guildas são aliadas
     * @param guild1ID ID da guilda 1
     * @param guild2ID ID da guilda 2
     * @return true se forem aliadas
     */
    bool areGuildsAllied(uint32_t guild1ID, uint32_t guild2ID) const;
    
    /**
     * @brief Verifica se duas guildas são inimigas
     * @param guild1ID ID da guilda 1
     * @param guild2ID ID da guilda 2
     * @return true se forem inimigas
     */
    bool areGuildsEnemies(uint32_t guild1ID, uint32_t guild2ID) const;
    
    /**
     * @brief Verifica se duas guildas estão em guerra
     * @param guild1ID ID da guilda 1
     * @param guild2ID ID da guilda 2
     * @return true se estiverem em guerra
     */
    bool areGuildsAtWar(uint32_t guild1ID, uint32_t guild2ID) const;
    
    /**
     * @brief Obtém a guerra ativa entre duas guildas
     * @param guild1ID ID da guilda 1
     * @param guild2ID ID da guilda 2
     * @return ID da guerra ou 0 se não estiverem em guerra
     */
    uint32_t getActiveWarBetweenGuilds(uint32_t guild1ID, uint32_t guild2ID) const;
    
    /**
     * @brief Registra um novo evento de guilda
     * @param event Evento
     */
    void logGuildEvent(const GuildEvent& event);
    
    /**
     * @brief Registra um evento de histórico para uma guilda
     * @param guildID ID da guilda
     * @param type Tipo
     * @param actorID ID do ator
     * @param targetID ID do alvo
     * @param data1 Dado 1
     * @param data2 Dado 2
     * @param description Descrição
     */
    void addGuildHistory(uint32_t guildID, GuildEventType type, uint32_t actorID = 0,
                        uint32_t targetID = 0, uint32_t data1 = 0, uint32_t data2 = 0,
                        const std::string& description = "");
    
    /**
     * @brief Registra um callback de evento
     * @param type Tipo
     * @param callback Callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerEventCallback(GuildEventType type, GuildEventCallback callback);
    
    /**
     * @brief Remove um callback de evento
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEventCallback(uint32_t callbackId);
    
    /**
     * @brief Dispara um evento
     * @param event Evento
     */
    void fireEvent(const GuildEvent& event);
    
    /**
     * @brief Obtém o ranking de guildas
     * @param count Contagem
     * @param orderBy Ordenar por
     * @return Lista de guildas
     */
    std::vector<GuildDefinition> getGuildRanking(uint32_t count = 10, const std::string& orderBy = "level") const;
    
    /**
     * @brief Obtém as guerras ativas
     * @return Lista de guerras ativas
     */
    std::vector<ActiveGuildWar> getActiveWars() const;
    
    /**
     * @brief Obtém o número de guildas
     * @return Número de guildas
     */
    uint32_t getGuildCount() const;
    
    /**
     * @brief Obtém o número de guerras
     * @return Número de guerras
     */
    uint32_t getWarCount() const;
    
    /**
     * @brief Obtém o número de alianças
     * @return Número de alianças
     */
    uint32_t getAllianceCount() const;
    
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
    
    /**
     * @brief Define o modo de log
     * @param logging Modo de log
     */
    void setLoggingEnabled(bool logging);
    
    /**
     * @brief Verifica se o modo de log está ativado
     * @return true se estiver ativado
     */
    bool isLoggingEnabled() const;
    
private:
    /**
     * @brief Construtor privado
     */
    GuildManager();
    
    /**
     * @brief Destrutor privado
     */
    ~GuildManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    GuildManager(const GuildManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    GuildManager& operator=(const GuildManager&) = delete;
    
    // Guildas
    std::unordered_map<uint32_t, GuildDefinition> _guilds;
    std::unordered_map<std::string, uint32_t> _guildsByName;
    std::unordered_map<uint32_t, uint32_t> _characterGuilds;
    std::atomic<uint32_t> _nextGuildID;
    
    // Alianças
    std::unordered_map<uint32_t, GuildAlliance> _alliances;
    std::map<uint32_t, std::vector<uint32_t>> _guildAlliances;
    std::atomic<uint32_t> _nextAllianceID;
    
    // Guerras
    std::unordered_map<uint32_t, GuildWar> _wars;
    std::map<uint32_t, std::vector<uint32_t>> _guildWars;
    std::vector<ActiveGuildWar> _activeWars;
    std::atomic<uint32_t> _nextWarID;
    
    // Eventos
    std::unordered_map<uint32_t, GuildEventCallback> _eventCallbacks;
    std::unordered_map<GuildEventType, std::vector<uint32_t>> _eventCallbacksByType;
    std::atomic<uint32_t> _nextCallbackID;
    std::atomic<uint32_t> _nextEventID;
    
    // Timers
    uint32_t _saveTimer;
    uint32_t _warUpdateTimer;
    
    // Thread safety
    mutable std::mutex _guildMutex;
    mutable std::mutex _allianceMutex;
    mutable std::mutex _warMutex;
    mutable std::mutex _eventMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    /**
     * @brief Atualiza guerras
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void updateWars(uint32_t deltaTime);
    
    /**
     * @brief Verifica se um nome de guilda já existe
     * @param name Nome
     * @return true se existir
     */
    bool guildNameExists(const std::string& name) const;
    
    /**
     * @brief Gera um ID de guilda
     * @return ID
     */
    uint32_t generateGuildID();
    
    /**
     * @brief Gera um ID de aliança
     * @return ID
     */
    uint32_t generateAllianceID();
    
    /**
     * @brief Gera um ID de guerra
     * @return ID
     */
    uint32_t generateWarID();
    
    /**
     * @brief Adiciona uma guerra à lista de guerras ativas
     * @param warID ID da guerra
     */
    void addActiveWar(uint32_t warID);
    
    /**
     * @brief Remove uma guerra da lista de guerras ativas
     * @param warID ID da guerra
     */
    void removeActiveWar(uint32_t warID);
    
    /**
     * @brief Atualiza o nível de uma guilda
     * @param guildID ID da guilda
     * @return true se atualizado com sucesso
     */
    bool updateGuildLevel(uint32_t guildID);
    
    /**
     * @brief Obtém a experiência necessária para um nível
     * @param level Nível
     * @return Experiência
     */
    uint32_t getExpForLevel(uint8_t level) const;
    
    /**
     * @brief Notifica os callbacks de evento
     * @param event Evento
     */
    void notifyEventCallbacks(const GuildEvent& event);
};

} // namespace guild
} // namespace wydbr

#endif // _GUILD_MANAGER_H_