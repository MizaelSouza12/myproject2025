/**
 * PartyManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/social/PartyManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _PARTY_MANAGER_H_
#define _PARTY_MANAGER_H_

/**
 * @file PartyManager.h
 * @brief Gerenciador de grupos (parties) do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de grupos de jogadores do WYDBR,
 * permitindo a formação de equipes para missões e combate,
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
namespace social {

/**
 * @brief Estado do grupo
 */
enum /**
 * Classe WYDPartyState
 * 
 * Esta classe implementa a funcionalidade WYDPartyState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PartyState {
    NONE = 0,               // Nenhum
    FORMING = 1,            // Formando
    ACTIVE = 2,             // Ativo
    DISBANDED = 3,          // Disperso
    INCOMPLETE = 4,         // Incompleto
    CUSTOM_1 = 5,           // Personalizado 1
    CUSTOM_2 = 6,           // Personalizado 2
    CUSTOM_3 = 7,           // Personalizado 3
    UNKNOWN = 8             // Desconhecido
};

/**
 * @brief Tipo de grupo
 */
enum /**
 * Classe WYDPartyType
 * 
 * Esta classe implementa a funcionalidade WYDPartyType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PartyType {
    NONE = 0,               // Nenhum
    NORMAL = 1,             // Normal
    RAID = 2,               // Raid
    BATTLEFIELD = 3,        // Campo de batalha
    QUEST = 4,              // Missão
    DUNGEON = 5,            // Masmorra
    EVENT = 6,              // Evento
    CUSTOM_1 = 7,           // Personalizado 1
    CUSTOM_2 = 8,           // Personalizado 2
    CUSTOM_3 = 9,           // Personalizado 3
    UNKNOWN = 10            // Desconhecido
};

/**
 * @brief Configuração de distribuição de itens
 */
enum /**
 * Classe WYDPartyLootConfig
 * 
 * Esta classe implementa a funcionalidade WYDPartyLootConfig conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PartyLootConfig {
    NONE = 0,               // Nenhum
    FREE_FOR_ALL = 1,       // Livre para todos
    ROUND_ROBIN = 2,        // Rodízio
    MASTER_LOOT = 3,        // Mestre de saque
    GROUP_LOOT = 4,         // Saque em grupo
    NEED_BEFORE_GREED = 5,  // Necessidade antes de ganância
    CUSTOM_1 = 6,           // Personalizado 1
    CUSTOM_2 = 7,           // Personalizado 2
    CUSTOM_3 = 8,           // Personalizado 3
    UNKNOWN = 9             // Desconhecido
};

/**
 * @brief Configuração de distribuição de experiência
 */
enum /**
 * Classe WYDPartyExpConfig
 * 
 * Esta classe implementa a funcionalidade WYDPartyExpConfig conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PartyExpConfig {
    NONE = 0,               // Nenhum
    EQUAL = 1,              // Igual
    LEVEL_BASED = 2,        // Baseado em nível
    CONTRIBUTION = 3,       // Contribuição
    CUSTOM_1 = 4,           // Personalizado 1
    CUSTOM_2 = 5,           // Personalizado 2
    CUSTOM_3 = 6,           // Personalizado 3
    UNKNOWN = 7             // Desconhecido
};

/**
 * @brief Função do membro no grupo
 */
enum /**
 * Classe WYDPartyMemberRole
 * 
 * Esta classe implementa a funcionalidade WYDPartyMemberRole conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PartyMemberRole {
    NONE = 0,               // Nenhum
    LEADER = 1,             // Líder
    ASSISTANT = 2,          // Assistente
    TANK = 3,               // Tanque
    HEALER = 4,             // Curador
    DAMAGE = 5,             // Dano
    SUPPORT = 6,            // Suporte
    CUSTOM_1 = 7,           // Personalizado 1
    CUSTOM_2 = 8,           // Personalizado 2
    CUSTOM_3 = 9,           // Personalizado 3
    UNKNOWN = 10            // Desconhecido
};

/**
 * @brief Resultado da operação de grupo
 */
enum /**
 * Classe WYDPartyResult
 * 
 * Esta classe implementa a funcionalidade WYDPartyResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PartyResult {
    SUCCESS = 0,            // Sucesso
    FAILED = 1,             // Falha
    PARTY_FULL = 2,         // Grupo cheio
    ALREADY_IN_PARTY = 3,   // Já está em um grupo
    NOT_IN_PARTY = 4,       // Não está em um grupo
    NOT_LEADER = 5,         // Não é o líder
    TARGET_NOT_FOUND = 6,   // Alvo não encontrado
    TARGET_ALREADY_IN_PARTY = 7, // Alvo já está em um grupo
    CANNOT_INVITE_SELF = 8, // Não pode convidar a si mesmo
    PARTY_NOT_FOUND = 9,    // Grupo não encontrado
    INVITATION_EXPIRED = 10,// Convite expirado
    LEVEL_RESTRICTION = 11, // Restrição de nível
    AREA_RESTRICTION = 12,  // Restrição de área
    CUSTOM_1 = 13,          // Personalizado 1
    CUSTOM_2 = 14,          // Personalizado 2
    CUSTOM_3 = 15,          // Personalizado 3
    UNKNOWN = 16            // Desconhecido
};

/**
 * @brief Membro do grupo
 */
struct PartyMember {
    uint32_t CharacterID;       // ID do personagem
    std::string Name;           // Nome
    uint8_t Level;              // Nível
    uint8_t Class;              // Classe
    PartyMemberRole Role;       // Função
    uint32_t JoinTime;          // Tempo de entrada
    bool Online;                // Online
    uint16_t MapID;             // ID do mapa
    uint16_t PosX;              // Posição X
    uint16_t PosY;              // Posição Y
    bool Ready;                 // Pronto
    
    PartyMember() : CharacterID(0), Level(0), Class(0), Role(PartyMemberRole::NONE),
        JoinTime(0), Online(false), MapID(0), PosX(0), PosY(0), Ready(false) {}
};

/**
 * @brief Grupo
 */
struct Party {
    uint32_t PartyID;           // ID do grupo
    uint32_t LeaderID;          // ID do líder
    std::string Name;           // Nome
    PartyState State;           // Estado
    PartyType Type;             // Tipo
    PartyLootConfig LootConfig; // Configuração de saque
    PartyExpConfig ExpConfig;   // Configuração de experiência
    uint32_t CreationTime;      // Tempo de criação
    uint8_t MinLevel;           // Nível mínimo
    uint8_t MaxLevel;           // Nível máximo
    uint8_t MaxMembers;         // Máximo de membros
    bool IsPublic;              // É público
    std::vector<PartyMember> Members; // Membros
    std::string Password;       // Senha
    
    Party() : PartyID(0), LeaderID(0), State(PartyState::NONE),
        Type(PartyType::NORMAL), LootConfig(PartyLootConfig::FREE_FOR_ALL),
        ExpConfig(PartyExpConfig::EQUAL), CreationTime(0), MinLevel(0),
        MaxLevel(0), MaxMembers(8), IsPublic(false) {}
};

/**
 * @brief Convite para grupo
 */
struct PartyInvitation {
    uint32_t InvitationID;      // ID do convite
    uint32_t PartyID;           // ID do grupo
    uint32_t InviterID;         // ID do convidador
    uint32_t InviteeID;         // ID do convidado
    uint32_t InviteTime;        // Tempo do convite
    uint32_t ExpirationTime;    // Tempo de expiração
    bool Accepted;              // Aceito
    bool Declined;              // Recusado
    bool Expired;               // Expirado
    
    PartyInvitation() : InvitationID(0), PartyID(0), InviterID(0),
        InviteeID(0), InviteTime(0), ExpirationTime(0), Accepted(false),
        Declined(false), Expired(false) {}
};

/**
 * @brief Estatísticas de grupo
 */
struct PartyStats {
    uint32_t PartyID;           // ID do grupo
    uint32_t TotalExp;          // Experiência total
    uint32_t TotalLoot;         // Saque total
    uint32_t MonstersKilled;    // Monstros mortos
    uint32_t QuestsCompleted;   // Missões completas
    uint32_t DungeonCount;      // Contagem de masmorras
    uint32_t RaidCount;         // Contagem de raids
    uint32_t PVPWins;           // Vitórias PVP
    uint32_t PVPLosses;         // Derrotas PVP
    uint32_t Duration;          // Duração
    
    PartyStats() : PartyID(0), TotalExp(0), TotalLoot(0), MonstersKilled(0),
        QuestsCompleted(0), DungeonCount(0), RaidCount(0), PVPWins(0),
        PVPLosses(0), Duration(0) {}
};

/**
 * @brief Estado global de grupos
 */
struct GlobalPartyState {
    uint32_t TotalParties;      // Total de grupos
    uint32_t ActiveParties;     // Grupos ativos
    uint32_t QuestParties;      // Grupos de missão
    uint32_t DungeonParties;    // Grupos de masmorra
    uint32_t RaidParties;       // Grupos de raid
    uint32_t AveragePartySize;  // Tamanho médio de grupo
    
    GlobalPartyState() : TotalParties(0), ActiveParties(0), QuestParties(0),
        DungeonParties(0), RaidParties(0), AveragePartySize(0) {}
};

/**
 * @brief Configuração de grupo
 */
struct PartyConfig {
    uint32_t DefaultMaxMembers;     // Máximo de membros padrão
    uint32_t InvitationExpiration;  // Expiração do convite
    bool AllowCrossRealm;           // Permitir cross-realm
    bool AllowCrossClass;           // Permitir cross-class Bool AllowCrossLevel;           // Permitir cross-level
    uint8_t LevelRestriction;       // Restrição de nível
    bool AutoAcceptInvitation;      // Auto aceitar convite
    bool AutoLeaderTransfer;        // Auto transferir líder
    
    PartyConfig() : DefaultMaxMembers(8), InvitationExpiration(60),
        AllowCrossRealm(false), AllowCrossClass(true), AllowCrossLevel(true),
        LevelRestriction(10), AutoAcceptInvitation(false), AutoLeaderTransfer(true) {}
};

/**
 * @brief Tipo de callback de grupo
 */
using PartyCallback = std::function<void(const Party&, uint32_t)>;

/**
 * @brief Gerenciador de grupos
 * 
 * Esta classe é responsável por gerenciar os grupos de jogadores.
 */
/**
 * Classe WYDPartyManager
 * 
 * Esta classe implementa a funcionalidade WYDPartyManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PartyManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static PartyManager& getInstance() {
        static PartyManager instance;
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
     * @brief Carrega a configuração de grupos
     * @return true se carregado com sucesso
     */
    bool loadPartyConfig();
    
    /**
     * @brief Cria um grupo
     * @param leaderID ID do líder
     * @param name Nome
     * @param type Tipo
     * @param maxMembers Máximo de membros
     * @param isPublic É público
     * @param password Senha
     * @return ID do grupo ou 0 em caso de erro
     */
    uint32_t createParty(uint32_t leaderID, const std::string& name = "", PartyType type = PartyType::NORMAL,
                        uint8_t maxMembers = 0, bool isPublic = false, const std::string& password = "");
    
    /**
     * @brief Remove um grupo
     * @param partyID ID do grupo
     * @return Resultado da operação
     */
    PartyResult disbandParty(uint32_t partyID);
    
    /**
     * @brief Convida um jogador para um grupo
     * @param partyID ID do grupo
     * @param inviterID ID do convidador
     * @param inviteeID ID do convidado
     * @return ID do convite ou 0 em caso de erro
     */
    uint32_t inviteToParty(uint32_t partyID, uint32_t inviterID, uint32_t inviteeID);
    
    /**
     * @brief Aceita um convite para grupo
     * @param invitationID ID do convite
     * @return Resultado da operação
     */
    PartyResult acceptInvitation(uint32_t invitationID);
    
    /**
     * @brief Recusa um convite para grupo
     * @param invitationID ID do convite
     * @return Resultado da operação
     */
    PartyResult declineInvitation(uint32_t invitationID);
    
    /**
     * @brief Adiciona um jogador a um grupo
     * @param partyID ID do grupo
     * @param characterID ID do personagem
     * @param role Função
     * @return Resultado da operação
     */
    PartyResult addMemberToParty(uint32_t partyID, uint32_t characterID, PartyMemberRole role = PartyMemberRole::NONE);
    
    /**
     * @brief Remove um jogador de um grupo
     * @param partyID ID do grupo
     * @param characterID ID do personagem
     * @param isKick É expulsão
     * @return Resultado da operação
     */
    PartyResult removeMemberFromParty(uint32_t partyID, uint32_t characterID, bool isKick = false);
    
    /**
     * @brief Transfere a liderança de um grupo
     * @param partyID ID do grupo
     * @param currentLeaderID ID do líder atual
     * @param newLeaderID ID do novo líder
     * @return Resultado da operação
     */
    PartyResult transferLeadership(uint32_t partyID, uint32_t currentLeaderID, uint32_t newLeaderID);
    
    /**
     * @brief Define a função de um membro do grupo
     * @param partyID ID do grupo
     * @param characterID ID do personagem
     * @param role Função
     * @return Resultado da operação
     */
    PartyResult setMemberRole(uint32_t partyID, uint32_t characterID, PartyMemberRole role);
    
    /**
     * @brief Define a configuração de saque do grupo
     * @param partyID ID do grupo
     * @param lootConfig Configuração de saque
     * @return Resultado da operação
     */
    PartyResult setLootConfig(uint32_t partyID, PartyLootConfig lootConfig);
    
    /**
     * @brief Define a configuração de experiência do grupo
     * @param partyID ID do grupo
     * @param expConfig Configuração de experiência
     * @return Resultado da operação
     */
    PartyResult setExpConfig(uint32_t partyID, PartyExpConfig expConfig);
    
    /**
     * @brief Define se um membro está pronto
     * @param partyID ID do grupo
     * @param characterID ID do personagem
     * @param ready Pronto
     * @return Resultado da operação
     */
    PartyResult setMemberReady(uint32_t partyID, uint32_t characterID, bool ready);
    
    /**
     * @brief Atualiza a posição de um membro
     * @param partyID ID do grupo
     * @param characterID ID do personagem
     * @param mapID ID do mapa
     * @param posX Posição X
     * @param posY Posição Y
     * @return Resultado da operação
     */
    PartyResult updateMemberPosition(uint32_t partyID, uint32_t characterID, uint16_t mapID, uint16_t posX, uint16_t posY);
    
    /**
     * @brief Define o status online de um membro
     * @param partyID ID do grupo
     * @param characterID ID do personagem
     * @param online Online
     * @return Resultado da operação
     */
    PartyResult setMemberOnline(uint32_t partyID, uint32_t characterID, bool online);
    
    /**
     * @brief Obtém o grupo de um jogador
     * @param characterID ID do personagem
     * @return ID do grupo ou 0 se não estiver em um grupo
     */
    uint32_t getCharacterParty(uint32_t characterID) const;
    
    /**
     * @brief Verifica se um jogador está em um grupo
     * @param characterID ID do personagem
     * @return true se estiver em um grupo
     */
    bool isInParty(uint32_t characterID) const;
    
    /**
     * @brief Verifica se um jogador é o líder de um grupo
     * @param characterID ID do personagem
     * @param partyID ID do grupo
     * @return true se for o líder
     */
    bool isPartyLeader(uint32_t characterID, uint32_t partyID) const;
    
    /**
     * @brief Obtém um grupo
     * @param partyID ID do grupo
     * @return Grupo ou nullptr se não encontrado
     */
    const Party* getParty(uint32_t partyID) const;
    
    /**
     * @brief Obtém um membro de um grupo
     * @param partyID ID do grupo
     * @param characterID ID do personagem
     * @return Membro ou nullptr se não encontrado
     */
    const PartyMember* getPartyMember(uint32_t partyID, uint32_t characterID) const;
    
    /**
     * @brief Obtém os membros de um grupo
     * @param partyID ID do grupo
     * @return Lista de membros
     */
    std::vector<PartyMember> getPartyMembers(uint32_t partyID) const;
    
    /**
     * @brief Obtém o líder de um grupo
     * @param partyID ID do grupo
     * @return ID do líder ou 0 se não encontrado
     */
    uint32_t getPartyLeader(uint32_t partyID) const;
    
    /**
     * @brief Obtém os convites de um jogador
     * @param characterID ID do personagem
     * @return Lista de IDs de convites
     */
    std::vector<uint32_t> getCharacterInvitations(uint32_t characterID) const;
    
    /**
     * @brief Obtém um convite
     * @param invitationID ID do convite
     * @return Convite ou nullptr se não encontrado
     */
    const PartyInvitation* getInvitation(uint32_t invitationID) const;
    
    /**
     * @brief Procura grupos públicos
     * @param type Tipo
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @return Lista de IDs de grupos
     */
    std::vector<uint32_t> findPublicParties(PartyType type = PartyType::NORMAL, uint8_t minLevel = 0, uint8_t maxLevel = 0) const;
    
    /**
     * @brief Procura grupos por nome
     * @param name Nome
     * @return Lista de IDs de grupos
     */
    std::vector<uint32_t> findPartiesByName(const std::string& name) const;
    
    /**
     * @brief Procura grupos por tipo
     * @param type Tipo
     * @return Lista de IDs de grupos
     */
    std::vector<uint32_t> findPartiesByType(PartyType type) const;
    
    /**
     * @brief Distribui experiência para o grupo
     * @param partyID ID do grupo
     * @param exp Experiência
     * @param sourceID ID da fonte
     * @return true se distribuído com sucesso
     */
    bool distributeExp(uint32_t partyID, uint32_t exp, uint32_t sourceID = 0);
    
    /**
     * @brief Verifica quem obtém um item em um grupo
     * @param partyID ID do grupo
     * @param itemID ID do item
     * @param sourceID ID da fonte
     * @return ID do personagem ou 0 se não houver vencedor
     */
    uint32_t getLootWinner(uint32_t partyID, uint32_t itemID, uint32_t sourceID = 0);
    
    /**
     * @brief Registra estatísticas para um grupo
     * @param partyID ID do grupo
     * @param exp Experiência
     * @param loot Saque
     * @param monstersKilled Monstros mortos
     * @param questsCompleted Missões completas
     * @param dungeonCount Contagem de masmorras
     * @param raidCount Contagem de raids
     * @param pvpWins Vitórias PVP
     * @param pvpLosses Derrotas PVP
     * @return true se registrado com sucesso
     */
    bool registerPartyStats(uint32_t partyID, uint32_t exp = 0, uint32_t loot = 0,
                          uint32_t monstersKilled = 0, uint32_t questsCompleted = 0,
                          uint32_t dungeonCount = 0, uint32_t raidCount = 0,
                          uint32_t pvpWins = 0, uint32_t pvpLosses = 0);
    
    /**
     * @brief Obtém estatísticas de um grupo
     * @param partyID ID do grupo
     * @return Estatísticas
     */
    PartyStats getPartyStats(uint32_t partyID) const;
    
    /**
     * @brief Verifica o nível médio de um grupo
     * @param partyID ID do grupo
     * @return Nível médio
     */
    uint8_t getPartyAverageLevel(uint32_t partyID) const;
    
    /**
     * @brief Verifica o mapa atual de um grupo
     * @param partyID ID do grupo
     * @return ID do mapa ou 0 se não estiverem no mesmo mapa
     */
    uint16_t getPartyCurrentMap(uint32_t partyID) const;
    
    /**
     * @brief Verifica se todos os membros de um grupo estão prontos
     * @param partyID ID do grupo
     * @return true se todos estiverem prontos
     */
    bool isPartyReady(uint32_t partyID) const;
    
    /**
     * @brief Envia uma mensagem para um grupo
     * @param partyID ID do grupo
     * @param senderID ID do remetente
     * @param message Mensagem
     * @return true se enviado com sucesso
     */
    bool sendPartyMessage(uint32_t partyID, uint32_t senderID, const std::string& message);
    
    /**
     * @brief Registra um callback de grupo
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerPartyCallback(PartyCallback callback);
    
    /**
     * @brief Remove um callback de grupo
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterPartyCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém o estado global de grupos
     * @return Estado global
     */
    GlobalPartyState getGlobalState() const;
    
    /**
     * @brief Obtém a configuração de grupo
     * @return Configuração
     */
    PartyConfig getPartyConfig() const;
    
    /**
     * @brief Define a configuração de grupo
     * @param config Configuração
     */
    void setPartyConfig(const PartyConfig& config);
    
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
    PartyManager();
    
    /**
     * @brief Destrutor privado
     */
    ~PartyManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    PartyManager(const PartyManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    PartyManager& operator=(const PartyManager&) = delete;
    
    // Grupos
    std::unordered_map<uint32_t, Party> _parties;
    std::unordered_map<uint32_t, uint32_t> _characterParties;
    std::atomic<uint32_t> _nextPartyID;
    
    // Convites
    std::unordered_map<uint32_t, PartyInvitation> _invitations;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _characterInvitations;
    std::atomic<uint32_t> _nextInvitationID;
    
    // Estatísticas
    std::unordered_map<uint32_t, PartyStats> _partyStats;
    GlobalPartyState _globalState;
    
    // Configuração
    PartyConfig _partyConfig;
    
    // Callbacks
    std::vector<PartyCallback> _partyCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _partyMutex;
    mutable std::mutex _invitationMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Timers
    uint32_t _invitationExpirationTimer;
    
    /**
     * @brief Verifica convites expirados
     * @param currentTime Tempo atual
     */
    void checkExpiredInvitations(uint32_t currentTime);
    
    /**
     * @brief Atualiza o estado global
     */
    void updateGlobalState();
    
    /**
     * @brief Executa callbacks de grupo
     * @param party Grupo
     * @param characterID ID do personagem
     */
    void executePartyCallbacks(const Party& party, uint32_t characterID);
    
    /**
     * @brief Calcula a distribuição de experiência
     * @param party Grupo
     * @param totalExp Experiência total
     * @param sourceID ID da fonte
     * @return Mapa de ID de personagem para experiência
     */
    std::map<uint32_t, uint32_t> calculateExpDistribution(const Party& party, uint32_t totalExp, uint32_t sourceID);
    
    /**
     * @brief Carrega dados de grupos do banco de dados
     * @return true se carregado com sucesso
     */
    bool loadPartiesFromDatabase();
    
    /**
     * @brief Salva dados de grupos no banco de dados
     * @return true se salvo com sucesso
     */
    bool savePartiesToDatabase();
};

} // namespace social
} // namespace wydbr

#endif // _PARTY_MANAGER_H_