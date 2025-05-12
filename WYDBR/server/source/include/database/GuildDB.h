/**
 * GuildDB.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/database/GuildDB.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _GUILD_DB_H_
#define _GUILD_DB_H_

/**
 * @file GuildDB.h
 * @brief Gerenciador de banco de dados de guildas do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de banco de dados de guildas do WYDBR,
 * responsável por armazenar e gerenciar informações de guildas,
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
#include "DatabaseManager.h"

namespace wydbr {
namespace database {

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
    NONE = 0,               // Nenhum
    ACTIVE = 1,             // Ativa
    DISBANDED = 2,          // Dissolvida
    SUSPENDED = 3,          // Suspensa
    PENDING = 4,            // Pendente
    CUSTOM_1 = 5,           // Personalizado 1
    CUSTOM_2 = 6,           // Personalizado 2
    CUSTOM_3 = 7,           // Personalizado 3
    UNKNOWN = 8             // Desconhecido
};

/**
 * @brief Resultado da operação de guilda
 */
enum /**
 * Classe WYDGuildResult
 * 
 * Esta classe implementa a funcionalidade WYDGuildResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildResult {
    SUCCESS = 0,                    // Sucesso
    FAILED = 1,                     // Falha
    GUILD_EXISTS = 2,               // Guilda existe
    GUILD_NOT_FOUND = 3,            // Guilda não encontrada
    GUILD_FULL = 4,                 // Guilda cheia
    GUILD_DISBANDED = 5,            // Guilda dissolvida
    GUILD_SUSPENDED = 6,            // Guilda suspensa
    MEMBER_EXISTS = 7,              // Membro existe
    MEMBER_NOT_FOUND = 8,           // Membro não encontrado
    INVALID_NAME = 9,               // Nome inválido
    INVALID_RANK = 10,              // Rank inválido
    INVALID_POSITION = 11,          // Posição inválida
    NAME_TAKEN = 12,                // Nome já utilizado
    ALREADY_IN_GUILD = 13,          // Já está em uma guilda
    NOT_GUILD_LEADER = 14,          // Não é líder da guilda
    NOT_GUILD_OFFICER = 15,         // Não é oficial da guilda
    INSUFFICIENT_FUNDS = 16,        // Fundos insuficientes
    CUSTOM_1 = 17,                  // Personalizado 1
    CUSTOM_2 = 18,                  // Personalizado 2
    CUSTOM_3 = 19,                  // Personalizado 3
    UNKNOWN = 20                    // Desconhecido
};

/**
 * @brief Tipo de log de guilda
 */
enum /**
 * Classe WYDGuildLogType
 * 
 * Esta classe implementa a funcionalidade WYDGuildLogType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildLogType {
    NONE = 0,               // Nenhum
    CREATION = 1,           // Criação
    DISSOLUTION = 2,        // Dissolução
    MEMBER_JOIN = 3,        // Entrada de membro
    MEMBER_LEAVE = 4,       // Saída de membro
    MEMBER_KICK = 5,        // Expulsão de membro
    RANK_CHANGE = 6,        // Alteração de rank
    LEADER_CHANGE = 7,      // Alteração de líder
    ALLIANCE_FORMED = 8,    // Aliança formada
    ALLIANCE_BROKEN = 9,    // Aliança quebrada
    WAR_DECLARED = 10,      // Guerra declarada
    WAR_ENDED = 11,         // Guerra terminada
    TREASURY_DEPOSIT = 12,  // Depósito no tesouro
    TREASURY_WITHDRAW = 13, // Saque do tesouro
    LEVEL_UP = 14,          // Subida de nível
    SKILL_LEARN = 15,       // Aprendizado de habilidade
    NOTICE_CHANGE = 16,     // Alteração de aviso
    PENALTY = 17,           // Penalidade
    REWARD = 18,            // Recompensa
    CUSTOM_1 = 19,          // Personalizado 1
    CUSTOM_2 = 20,          // Personalizado 2
    CUSTOM_3 = 21,          // Personalizado 3
    UNKNOWN = 22            // Desconhecido
};

/**
 * @brief Posição do membro da guilda
 */
enum /**
 * Classe WYDGuildPosition
 * 
 * Esta classe implementa a funcionalidade WYDGuildPosition conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildPosition {
    NONE = 0,               // Nenhum
    LEADER = 1,             // Líder
    SUB_LEADER = 2,         // Sub-líder
    OFFICER = 3,            // Oficial
    VETERAN = 4,            // Veterano
    MEMBER = 5,             // Membro
    RECRUIT = 6,            // Recruta
    CUSTOM_1 = 7,           // Personalizado 1
    CUSTOM_2 = 8,           // Personalizado 2
    CUSTOM_3 = 9,           // Personalizado 3
    UNKNOWN = 10            // Desconhecido
};

/**
 * @brief Relação de aliança/guerra
 */
enum /**
 * Classe WYDGuildRelation
 * 
 * Esta classe implementa a funcionalidade WYDGuildRelation conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildRelation {
    NONE = 0,               // Nenhum
    NEUTRAL = 1,            // Neutro
    ALLY = 2,               // Aliado
    ENEMY = 3,              // Inimigo
    CUSTOM_1 = 4,           // Personalizado 1
    CUSTOM_2 = 5,           // Personalizado 2
    CUSTOM_3 = 6,           // Personalizado 3
    UNKNOWN = 7             // Desconhecido
};

/**
 * @brief Membro da guilda
 */
struct GuildMember {
    uint32_t CharacterID;       // ID do personagem
    std::string Name;           // Nome
    uint8_t Level;              // Nível
    uint8_t Class;              // Classe
    GuildPosition Position;     // Posição
    uint32_t JoinDate;          // Data de entrada
    uint32_t LastLogin;         // Último login
    uint32_t Contribution;      // Contribuição
    bool Online;                // Online
    
    GuildMember() : CharacterID(0), Level(0), Class(0),
        Position(GuildPosition::RECRUIT), JoinDate(0), LastLogin(0),
        Contribution(0), Online(false) {}
};

/**
 * @brief Informações da guilda
 */
struct GuildInfo {
    uint32_t GuildID;           // ID da guilda
    std::string Name;           // Nome
    std::string Notice;         // Aviso
    std::string Description;    // Descrição
    uint32_t LeaderID;          // ID do líder
    std::string LeaderName;     // Nome do líder
    uint32_t CreationDate;      // Data de criação
    GuildState State;           // Estado
    uint32_t DisbandDate;       // Data de dissolução
    uint32_t Level;             // Nível
    uint32_t Experience;        // Experiência
    uint32_t Funds;             // Fundos
    uint16_t MemberCount;       // Contagem de membros
    uint16_t MaxMembers;        // Máximo de membros
    uint8_t Mark[16];           // Marca
    std::vector<GuildMember> Members; // Membros
    std::map<uint32_t, GuildRelation> Relations; // Relações
    
    GuildInfo() : GuildID(0), LeaderID(0), CreationDate(0), State(GuildState::ACTIVE),
        DisbandDate(0), Level(1), Experience(0), Funds(0), MemberCount(0),
        MaxMembers(50) {
        memset(Mark, 0, sizeof(Mark));
    }
};

/**
 * @brief Log de guilda
 */
struct GuildLog {
    uint32_t LogID;             // ID do log
    uint32_t GuildID;           // ID da guilda
    GuildLogType Type;          // Tipo
    uint32_t CharacterID;       // ID do personagem
    uint32_t TargetID;          // ID do alvo
    uint32_t Timestamp;         // Timestamp
    std::string Details;        // Detalhes
    
    GuildLog() : LogID(0), GuildID(0), Type(GuildLogType::NONE),
        CharacterID(0), TargetID(0), Timestamp(0) {}
};

/**
 * @brief Aplicação de guilda
 */
struct GuildApplication {
    uint32_t ApplicationID;     // ID da aplicação
    uint32_t GuildID;           // ID da guilda
    uint32_t CharacterID;       // ID do personagem
    std::string Message;        // Mensagem
    uint32_t ApplyDate;         // Data de aplicação
    bool Accepted;              // Aceito
    bool Rejected;              // Rejeitado
    uint32_t ProcessDate;       // Data de processamento
    uint32_t ProcessedBy;       // Processado por
    
    GuildApplication() : ApplicationID(0), GuildID(0), CharacterID(0),
        ApplyDate(0), Accepted(false), Rejected(false), ProcessDate(0),
        ProcessedBy(0) {}
};

/**
 * @brief Estatísticas de guilda
 */
struct GuildStats {
    uint32_t TotalGuilds;       // Total de guildas
    uint32_t ActiveGuilds;      // Guildas ativas
    uint32_t DisbandedGuilds;   // Guildas dissolvidas
    uint32_t TotalMembers;      // Total de membros
    float AverageMembers;       // Média de membros
    uint32_t NewGuildsToday;    // Novas guildas hoje
    uint32_t DisbandedToday;    // Dissolvidas hoje
    uint32_t TotalFunds;        // Total de fundos
    
    GuildStats() : TotalGuilds(0), ActiveGuilds(0), DisbandedGuilds(0),
        TotalMembers(0), AverageMembers(0.0f), NewGuildsToday(0),
        DisbandedToday(0), TotalFunds(0) {}
};

/**
 * @brief Configuração de guilda
 */
struct GuildConfig {
    uint32_t CreationCost;          // Custo de criação
    uint16_t MaxMembers;            // Máximo de membros
    uint16_t MaxAlliances;          // Máximo de alianças
    uint16_t MaxEnemies;            // Máximo de inimigos
    uint32_t DisbandDelay;          // Atraso de dissolução
    uint32_t ExpForLevelUp;         // Experiência para subir de nível
    float ExpMultiplier;            // Multiplicador de experiência
    uint8_t MinNameLength;          // Tamanho mínimo do nome
    uint8_t MaxNameLength;          // Tamanho máximo do nome
    uint8_t MaxLevel;               // Nível máximo
    bool AllowMarkChange;           // Permitir alteração de marca
    bool AutoSave;                  // Auto salvar
    uint32_t AutoSaveInterval;      // Intervalo de auto salvamento
    
    GuildConfig() : CreationCost(1000000), MaxMembers(50), MaxAlliances(10),
        MaxEnemies(10), DisbandDelay(604800), ExpForLevelUp(1000000),
        ExpMultiplier(1.5f), MinNameLength(3), MaxNameLength(12),
        MaxLevel(50), AllowMarkChange(true), AutoSave(true),
        AutoSaveInterval(300) {}
};

/**
 * @brief Tipo de callback de guilda
 */
using GuildCallback = std::function<void(const GuildInfo&, GuildState)>;

/**
 * @brief Gerenciador de banco de dados de guildas
 * 
 * Esta classe é responsável por gerenciar o banco de dados de guildas.
 */
/**
 * Classe WYDGuildDB
 * 
 * Esta classe implementa a funcionalidade WYDGuildDB conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildDB {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static GuildDB& getInstance() {
        static GuildDB instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @param dbManager Gerenciador de banco de dados
     * @return true se inicializado com sucesso
     */
    bool initialize(DatabaseManager* dbManager);
    
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
     * @brief Carrega a configuração de guildas
     * @return true se carregado com sucesso
     */
    bool loadGuildConfig();
    
    /**
     * @brief Cria uma guilda
     * @param name Nome
     * @param leaderID ID do líder
     * @param funds Fundos
     * @param mark Marca
     * @return ID da guilda ou 0 em caso de erro
     */
    uint32_t createGuild(const std::string& name, uint32_t leaderID, uint32_t funds = 0, const uint8_t mark[16] = nullptr);
    
    /**
     * @brief Marca uma guilda para dissolução
     * @param guildID ID da guilda
     * @param leaderID ID do líder
     * @return Resultado da operação
     */
    GuildResult markGuildForDissolution(uint32_t guildID, uint32_t leaderID);
    
    /**
     * @brief Cancela a dissolução de uma guilda
     * @param guildID ID da guilda
     * @param leaderID ID do líder
     * @return Resultado da operação
     */
    GuildResult cancelGuildDissolution(uint32_t guildID, uint32_t leaderID);
    
    /**
     * @brief Dissolve uma guilda imediatamente
     * @param guildID ID da guilda
     * @return Resultado da operação
     */
    GuildResult dissolveGuildImmediately(uint32_t guildID);
    
    /**
     * @brief Suspende uma guilda
     * @param guildID ID da guilda
     * @param reason Motivo
     * @return Resultado da operação
     */
    GuildResult suspendGuild(uint32_t guildID, const std::string& reason);
    
    /**
     * @brief Remove a suspensão de uma guilda
     * @param guildID ID da guilda
     * @return Resultado da operação
     */
    GuildResult unsuspendGuild(uint32_t guildID);
    
    /**
     * @brief Adiciona um membro à guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param position Posição
     * @return Resultado da operação
     */
    GuildResult addGuildMember(uint32_t guildID, uint32_t characterID, GuildPosition position = GuildPosition::RECRUIT);
    
    /**
     * @brief Remove um membro da guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param isKick É expulsão
     * @return Resultado da operação
     */
    GuildResult removeGuildMember(uint32_t guildID, uint32_t characterID, bool isKick = false);
    
    /**
     * @brief Altera a posição de um membro
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param position Posição
     * @return Resultado da operação
     */
    GuildResult changeGuildMemberPosition(uint32_t guildID, uint32_t characterID, GuildPosition position);
    
    /**
     * @brief Altera o líder da guilda
     * @param guildID ID da guilda
     * @param currentLeaderID ID do líder atual
     * @param newLeaderID ID do novo líder
     * @return Resultado da operação
     */
    GuildResult changeGuildLeader(uint32_t guildID, uint32_t currentLeaderID, uint32_t newLeaderID);
    
    /**
     * @brief Altera o nome da guilda
     * @param guildID ID da guilda
     * @param leaderID ID do líder
     * @param name Nome
     * @return Resultado da operação
     */
    GuildResult changeGuildName(uint32_t guildID, uint32_t leaderID, const std::string& name);
    
    /**
     * @brief Altera o aviso da guilda
     * @param guildID ID da guilda
     * @param officerID ID do oficial
     * @param notice Aviso
     * @return Resultado da operação
     */
    GuildResult changeGuildNotice(uint32_t guildID, uint32_t officerID, const std::string& notice);
    
    /**
     * @brief Altera a descrição da guilda
     * @param guildID ID da guilda
     * @param officerID ID do oficial
     * @param description Descrição
     * @return Resultado da operação
     */
    GuildResult changeGuildDescription(uint32_t guildID, uint32_t officerID, const std::string& description);
    
    /**
     * @brief Altera a marca da guilda
     * @param guildID ID da guilda
     * @param leaderID ID do líder
     * @param mark Marca
     * @return Resultado da operação
     */
    GuildResult changeGuildMark(uint32_t guildID, uint32_t leaderID, const uint8_t mark[16]);
    
    /**
     * @brief Define uma relação entre guildas
     * @param guildID ID da guilda
     * @param targetGuildID ID da guilda alvo
     * @param relation Relação
     * @return Resultado da operação
     */
    GuildResult setGuildRelation(uint32_t guildID, uint32_t targetGuildID, GuildRelation relation);
    
    /**
     * @brief Adiciona experiência à guilda
     * @param guildID ID da guilda
     * @param exp Experiência
     * @return Resultado da operação
     */
    GuildResult addGuildExperience(uint32_t guildID, uint32_t exp);
    
    /**
     * @brief Altera os fundos da guilda
     * @param guildID ID da guilda
     * @param funds Fundos
     * @param relative Relativo
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    GuildResult changeGuildFunds(uint32_t guildID, uint32_t funds, bool relative = true, uint32_t characterID = 0);
    
    /**
     * @brief Adiciona contribuição a um membro
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param contribution Contribuição
     * @return Resultado da operação
     */
    GuildResult addMemberContribution(uint32_t guildID, uint32_t characterID, uint32_t contribution);
    
    /**
     * @brief Define o status online de um membro
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param online Online
     * @return Resultado da operação
     */
    GuildResult setMemberOnline(uint32_t guildID, uint32_t characterID, bool online);
    
    /**
     * @brief Atualiza informações de um membro
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param level Nível
     * @param className Classe
     * @return Resultado da operação
     */
    GuildResult updateMemberInfo(uint32_t guildID, uint32_t characterID, uint8_t level = 0, uint8_t className = 0);
    
    /**
     * @brief Aplica a uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @param message Mensagem
     * @return ID da aplicação ou 0 em caso de erro
     */
    uint32_t applyToGuild(uint32_t guildID, uint32_t characterID, const std::string& message = "");
    
    /**
     * @brief Aceita uma aplicação
     * @param applicationID ID da aplicação
     * @param officerID ID do oficial
     * @return Resultado da operação
     */
    GuildResult acceptApplication(uint32_t applicationID, uint32_t officerID);
    
    /**
     * @brief Rejeita uma aplicação
     * @param applicationID ID da aplicação
     * @param officerID ID do oficial
     * @return Resultado da operação
     */
    GuildResult rejectApplication(uint32_t applicationID, uint32_t officerID);
    
    /**
     * @brief Carrega uma guilda
     * @param guildID ID da guilda
     * @return Informações da guilda ou nullptr se não encontrada
     */
    const GuildInfo* loadGuild(uint32_t guildID);
    
    /**
     * @brief Carrega todas as guildas
     * @return true se carregado com sucesso
     */
    bool loadAllGuilds();
    
    /**
     * @brief Salva uma guilda
     * @param guildInfo Informações da guilda
     * @return Resultado da operação
     */
    GuildResult saveGuild(const GuildInfo& guildInfo);
    
    /**
     * @brief Salva uma guilda imediatamente
     * @param guildID ID da guilda
     * @return Resultado da operação
     */
    GuildResult saveGuildNow(uint32_t guildID);
    
    /**
     * @brief Salva todas as guildas
     * @return true se salvo com sucesso
     */
    bool saveAllGuilds();
    
    /**
     * @brief Obtém uma guilda por nome
     * @param name Nome
     * @return ID da guilda ou 0 se não encontrada
     */
    uint32_t getGuildByName(const std::string& name) const;
    
    /**
     * @brief Obtém a guilda de um personagem
     * @param characterID ID do personagem
     * @return ID da guilda ou 0 se não encontrada
     */
    uint32_t getCharacterGuild(uint32_t characterID) const;
    
    /**
     * @brief Verifica se um personagem é membro de uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @return true se for membro
     */
    bool isGuildMember(uint32_t guildID, uint32_t characterID) const;
    
    /**
     * @brief Verifica se um personagem é oficial de uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @return true se for oficial
     */
    bool isGuildOfficer(uint32_t guildID, uint32_t characterID) const;
    
    /**
     * @brief Verifica se um personagem é líder de uma guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @return true se for líder
     */
    bool isGuildLeader(uint32_t guildID, uint32_t characterID) const;
    
    /**
     * @brief Obtém as aplicações pendentes de uma guilda
     * @param guildID ID da guilda
     * @return Lista de aplicações
     */
    std::vector<GuildApplication> getPendingApplications(uint32_t guildID) const;
    
    /**
     * @brief Obtém uma aplicação
     * @param applicationID ID da aplicação
     * @return Aplicação ou nullptr se não encontrada
     */
    const GuildApplication* getApplication(uint32_t applicationID) const;
    
    /**
     * @brief Obtém as aplicações de um personagem
     * @param characterID ID do personagem
     * @return Lista de aplicações
     */
    std::vector<GuildApplication> getCharacterApplications(uint32_t characterID) const;
    
    /**
     * @brief Obtém um membro da guilda
     * @param guildID ID da guilda
     * @param characterID ID do personagem
     * @return Membro ou nullptr se não encontrado
     */
    const GuildMember* getGuildMember(uint32_t guildID, uint32_t characterID) const;
    
    /**
     * @brief Obtém os membros de uma guilda
     * @param guildID ID da guilda
     * @return Lista de membros
     */
    std::vector<GuildMember> getGuildMembers(uint32_t guildID) const;
    
    /**
     * @brief Obtém os membros online de uma guilda
     * @param guildID ID da guilda
     * @return Lista de membros
     */
    std::vector<GuildMember> getOnlineGuildMembers(uint32_t guildID) const;
    
    /**
     * @brief Obtém a relação entre guildas
     * @param guildID1 ID da guilda 1
     * @param guildID2 ID da guilda 2
     * @return Relação
     */
    GuildRelation getGuildRelation(uint32_t guildID1, uint32_t guildID2) const;
    
    /**
     * @brief Obtém as guildas aliadas
     * @param guildID ID da guilda
     * @return Lista de IDs de guildas
     */
    std::vector<uint32_t> getAlliedGuilds(uint32_t guildID) const;
    
    /**
     * @brief Obtém as guildas inimigas
     * @param guildID ID da guilda
     * @return Lista de IDs de guildas
     */
    std::vector<uint32_t> getEnemyGuilds(uint32_t guildID) const;
    
    /**
     * @brief Obtém todas as guildas
     * @param activeOnly Apenas ativas
     * @return Lista de IDs de guildas
     */
    std::vector<uint32_t> getAllGuilds(bool activeOnly = true) const;
    
    /**
     * @brief Obtém guildas por nível
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @return Lista de IDs de guildas
     */
    std::vector<uint32_t> getGuildsByLevel(uint32_t minLevel, uint32_t maxLevel) const;
    
    /**
     * @brief Obtém logs de uma guilda
     * @param guildID ID da guilda
     * @param limit Limite (0 para todos)
     * @param offset Deslocamento
     * @return Lista de logs
     */
    std::vector<GuildLog> getGuildLogs(uint32_t guildID, uint32_t limit = 0, uint32_t offset = 0) const;
    
    /**
     * @brief Adiciona um log
     * @param guildID ID da guilda
     * @param type Tipo
     * @param characterID ID do personagem
     * @param targetID ID do alvo
     * @param details Detalhes
     * @return ID do log
     */
    uint32_t addGuildLog(uint32_t guildID, GuildLogType type, uint32_t characterID = 0, uint32_t targetID = 0, const std::string& details = "");
    
    /**
     * @brief Registra um callback de guilda
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerGuildCallback(GuildCallback callback);
    
    /**
     * @brief Remove um callback de guilda
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterGuildCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém estatísticas de guildas
     * @return Estatísticas
     */
    GuildStats getGuildStats() const;
    
    /**
     * @brief Obtém a configuração de guildas
     * @return Configuração
     */
    GuildConfig getGuildConfig() const;
    
    /**
     * @brief Define a configuração de guildas
     * @param config Configuração
     */
    void setGuildConfig(const GuildConfig& config);
    
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
    GuildDB();
    
    /**
     * @brief Destrutor privado
     */
    ~GuildDB();
    
    /**
     * @brief Construtor de cópia privado
     */
    GuildDB(const GuildDB&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    GuildDB& operator=(const GuildDB&) = delete;
    
    // Gerenciador de banco de dados
    DatabaseManager* _dbManager;
    
    // Cache de guildas
    std::unordered_map<uint32_t, GuildInfo> _guildCache;
    std::unordered_map<std::string, uint32_t> _nameToID;
    std::unordered_map<uint32_t, uint32_t> _characterToGuild;
    std::atomic<uint32_t> _nextGuildID;
    
    // Guildas modificadas
    std::unordered_set<uint32_t> _modifiedGuilds;
    
    // Aplicações
    std::unordered_map<uint32_t, GuildApplication> _applications;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _guildApplications;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _characterApplications;
    std::atomic<uint32_t> _nextApplicationID;
    
    // Logs
    std::unordered_map<uint32_t, std::vector<GuildLog>> _guildLogs;
    std::atomic<uint32_t> _nextLogID;
    
    // Estatísticas
    GuildStats _guildStats;
    
    // Configuração
    GuildConfig _guildConfig;
    
    // Callbacks
    std::vector<GuildCallback> _guildCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _guildMutex;
    mutable std::mutex _applicationMutex;
    mutable std::mutex _logMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Timers
    uint32_t _autoSaveTimer;
    uint32_t _dissolutionCheckTimer;
    
    /**
     * @brief Salva guildas modificadas
     */
    void saveModifiedGuilds();
    
    /**
     * @brief Verifica guildas marcadas para dissolução
     * @param currentTime Tempo atual
     */
    void checkGuildsForDissolution(uint32_t currentTime);
    
    /**
     * @brief Atualiza as estatísticas
     */
    void updateGuildStats();
    
    /**
     * @brief Executa callbacks de guilda
     * @param guild Guilda
     * @param oldState Estado antigo
     */
    void executeGuildCallbacks(const GuildInfo& guild, GuildState oldState);
    
    /**
     * @brief Atualiza os índices de uma guilda
     * @param guildInfo Informações da guilda
     */
    void updateGuildIndices(const GuildInfo& guildInfo);
    
    /**
     * @brief Carrega uma guilda do banco de dados
     * @param guildID ID da guilda
     * @return Informações da guilda ou nullptr se não encontrada
     */
    const GuildInfo* loadGuildFromDB(uint32_t guildID);
    
    /**
     * @brief Salva uma guilda no banco de dados
     * @param guildInfo Informações da guilda
     * @return true se salvo com sucesso
     */
    bool saveGuildToDB(const GuildInfo& guildInfo);
    
    /**
     * @brief Valida um nome de guilda
     * @param name Nome
     * @return true se for válido
     */
    bool isValidGuildName(const std::string& name);
};

} // namespace database
} // namespace wydbr

#endif // _GUILD_DB_H_