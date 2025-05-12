/**
 * GuildManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/guild/GuildManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef GUILDMANAGER_H
#define GUILDMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <set>
#include <variant>
#include <optional>
#include <queue>
#include <atomic>
#include <thread>
#include <shared_mutex>

#include "../../../core/WYDTypes.h"
#include "./types/GuildTypes.h"
#include "../player/CPlayer.h"
#include "../kernel/persistence/PersistenceManager.h"

namespace wydbr {


/**
 * @file GuildManager.h
 * @brief Sistema avançado de gerenciamento de guildas e guerras de território
 * 
 * Este arquivo contém a implementação do sistema de gerenciamento de guildas,
 * que suporta estruturas hierárquicas, gerenciamento de membros, conquista e
 * defesa de territórios, guerras organizadas, economia de guilda e benefícios
 * para os membros.
 */

namespace wyd {
namespace server {
namespace guild {

/**
 * @brief Configuração do gerenciador de guilda
 */
struct GuildManagerConfig {
    bool autosave;                      // Autosave de dados
    DWORD autosaveInterval;             // Intervalo de autosave (ms)
    bool asyncProcessing;               // Processamento assíncrono
    DWORD maxGuildsPerFaction;          // Máximo de guildas por facção
    DWORD guildCreationGoldCost;        // Custo em ouro para criar guilda
    DWORD minMembersForTerritory;       // Mínimo de membros para território
    bool enableTerritoryWars;           // Habilitar guerras de território
    DWORD territoryWarCooldown;         // Cooldown de guerra de território (horas)
    bool enableGuildSystem;             // Habilitar sistema de guilda
    bool enableGuildLeveling;           // Habilitar nivelamento de guilda
    DWORD minGuildLevel;                // Nível mínimo de guilda
    DWORD maxGuildLevel;                // Nível máximo de guilda
    DWORD structurePlacementRange;      // Alcance de colocação de estrutura
    bool telemetry;                     // Telemetria
    DWORD maxBattlesPerWar;             // Máximo de batalhas por guerra
    DWORD maxItemLevelInStorage;        // Nível máximo de item em armazenamento
    
    GuildManagerConfig()
        : autosave(true)
        , autosaveInterval(300000)
        , asyncProcessing(true)
        , maxGuildsPerFaction(100)
        , guildCreationGoldCost(1000000)
        , minMembersForTerritory(10)
        , enableTerritoryWars(true)
        , territoryWarCooldown(24)
        , enableGuildSystem(true)
        , enableGuildLeveling(true)
        , minGuildLevel(1)
        , maxGuildLevel(10)
        , structurePlacementRange(200)
        , telemetry(true)
        , maxBattlesPerWar(3)
        , maxItemLevelInStorage(10)
    {
    }
};

/**
 * @brief Estatísticas do gerenciador de guildas
 */
struct GuildManagerStats {
    DWORD totalGuilds;                  // Total de guildas
    DWORD totalMembers;                 // Total de membros
    DWORD totalTerritories;             // Total de territórios
    DWORD activeWars;                   // Guerras ativas
    DWORD totalStructures;              // Total de estruturas
    DWORD totalGuildSkills;             // Total de habilidades de guilda
    DWORD totalStorageItems;            // Total de itens em armazenamento
    DWORD totalAlliances;               // Total de alianças
    DWORD totalWarsCompleted;           // Total de guerras completadas
    DWORD warCasualties;                // Baixas de guerra
    
    GuildManagerStats()
        : totalGuilds(0)
        , totalMembers(0)
        , totalTerritories(0)
        , activeWars(0)
        , totalStructures(0)
        , totalGuildSkills(0)
        , totalStorageItems(0)
        , totalAlliances(0)
        , totalWarsCompleted(0)
        , warCasualties(0)
    {
    }
};

/**
 * @brief Gerenciador de guildas
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
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static GuildManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const GuildManagerConfig& config = GuildManagerConfig());
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Cria uma guilda
     * @param leaderID ID do líder
     * @param name Nome
     * @param tag Tag
     * @param type Tipo
     * @param errorReason Razão do erro (saída)
     * @return ID da guilda, ou 0 se falhar
     */
    DWORD CreateGuild(DWORD leaderID, const std::string& name, const std::string& tag, 
                    GuildType type, std::string& errorReason);
    
    /**
     * @brief Dissolve uma guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param errorReason Razão do erro (saída)
     * @return true se dissolvido com sucesso
     */
    bool DissolveGuild(DWORD guildID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Obtém uma guilda
     * @param guildID ID da guilda
     * @return Ponteiro para a guilda, ou nullptr se não existir
     */
    const Guild* GetGuild(DWORD guildID) const;
    
    /**
     * @brief Obtém uma guilda por nome
     * @param name Nome
     * @return Ponteiro para a guilda, ou nullptr se não existir
     */
    const Guild* GetGuildByName(const std::string& name) const;
    
    /**
     * @brief Obtém uma guilda por tag
     * @param tag Tag
     * @return Ponteiro para a guilda, ou nullptr se não existir
     */
    const Guild* GetGuildByTag(const std::string& tag) const;
    
    /**
     * @brief Obtém a guilda de um jogador
     * @param playerID ID do jogador
     * @return Ponteiro para a guilda, ou nullptr se não estiver em uma
     */
    const Guild* GetPlayerGuild(DWORD playerID) const;
    
    /**
     * @brief Convida um jogador para uma guilda
     * @param guildID ID da guilda
     * @param inviterID ID do convidador
     * @param targetPlayerID ID do jogador alvo
     * @param errorReason Razão do erro (saída)
     * @return true se convidado com sucesso
     */
    bool InvitePlayerToGuild(DWORD guildID, DWORD inviterID, DWORD targetPlayerID, std::string& errorReason);
    
    /**
     * @brief Responde a um convite de guilda
     * @param playerID ID do jogador
     * @param guildID ID da guilda
     * @param accept Se aceita
     * @param errorReason Razão do erro (saída)
     * @return true se respondido com sucesso
     */
    bool RespondToGuildInvite(DWORD playerID, DWORD guildID, bool accept, std::string& errorReason);
    
    /**
     * @brief Remove um membro de uma guilda
     * @param guildID ID da guilda
     * @param removerID ID do removedor
     * @param targetPlayerID ID do jogador alvo
     * @param reason Razão
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveGuildMember(DWORD guildID, DWORD removerID, DWORD targetPlayerID, 
                         const std::string& reason, std::string& errorReason);
    
    /**
     * @brief Sai de uma guilda
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se saiu com sucesso
     */
    bool LeaveGuild(DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Promove um membro de guilda
     * @param guildID ID da guilda
     * @param promoterID ID do promotor
     * @param targetPlayerID ID do jogador alvo
     * @param errorReason Razão do erro (saída)
     * @return true se promovido com sucesso
     */
    bool PromoteGuildMember(DWORD guildID, DWORD promoterID, DWORD targetPlayerID, std::string& errorReason);
    
    /**
     * @brief Rebaixa um membro de guilda
     * @param guildID ID da guilda
     * @param demoterID ID do rebaixador
     * @param targetPlayerID ID do jogador alvo
     * @param errorReason Razão do erro (saída)
     * @return true se rebaixado com sucesso
     */
    bool DemoteGuildMember(DWORD guildID, DWORD demoterID, DWORD targetPlayerID, std::string& errorReason);
    
    /**
     * @brief Define permissões para um rank de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param rank Rank
     * @param permissions Permissões
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetGuildRankPermissions(DWORD guildID, DWORD playerID, GuildRank rank, 
                               const std::bitset<32>& permissions, std::string& errorReason);
    
    /**
     * @brief Transfere liderança de guilda
     * @param guildID ID da guilda
     * @param currentLeaderID ID do líder atual
     * @param newLeaderID ID do novo líder
     * @param errorReason Razão do erro (saída)
     * @return true se transferido com sucesso
     */
    bool TransferGuildLeadership(DWORD guildID, DWORD currentLeaderID, DWORD newLeaderID, std::string& errorReason);
    
    /**
     * @brief Define o anúncio de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param announcement Anúncio
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetGuildAnnouncement(DWORD guildID, DWORD playerID, const std::string& announcement, std::string& errorReason);
    
    /**
     * @brief Define a descrição de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param description Descrição
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetGuildDescription(DWORD guildID, DWORD playerID, const std::string& description, std::string& errorReason);
    
    /**
     * @brief Obtém o nível de uma guilda
     * @param guildID ID da guilda
     * @return Nível, ou 0 se falhar
     */
    BYTE GetGuildLevel(DWORD guildID) const;
    
    /**
     * @brief Obtém os membros de uma guilda
     * @param guildID ID da guilda
     * @return Lista de membros
     */
    std::vector<GuildMember> GetGuildMembers(DWORD guildID) const;
    
    /**
     * @brief Obtém as habilidades de uma guilda
     * @param guildID ID da guilda
     * @return Lista de habilidades
     */
    std::vector<GuildSkill> GetGuildSkills(DWORD guildID) const;
    
    /**
     * @brief Aprende uma habilidade de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param skillID ID da habilidade
     * @param errorReason Razão do erro (saída)
     * @return true se aprendido com sucesso
     */
    bool LearnGuildSkill(DWORD guildID, DWORD playerID, DWORD skillID, std::string& errorReason);
    
    /**
     * @brief Melhora uma habilidade de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param skillID ID da habilidade
     * @param errorReason Razão do erro (saída)
     * @return true se melhorado com sucesso
     */
    bool UpgradeGuildSkill(DWORD guildID, DWORD playerID, DWORD skillID, std::string& errorReason);
    
    /**
     * @brief Obtém o rank de um membro de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @return Rank, ou GuildRank::RECRUIT se falhar
     */
    GuildRank GetGuildMemberRank(DWORD guildID, DWORD playerID) const;
    
    /**
     * @brief Verifica se um jogador tem uma permissão de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @param permission Permissão
     * @return true se tiver a permissão
     */
    bool HasGuildPermission(DWORD guildID, DWORD playerID, GuildPermission permission) const;
    
    /**
     * @brief Obtém o registro de uma guilda
     * @param guildID ID da guilda
     * @param count Contagem
     * @return Lista de registros
     */
    std::vector<GuildLog> GetGuildLog(DWORD guildID, size_t count = 100) const;
    
    /**
     * @brief Adiciona um item ao armazenamento de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @param itemID ID do item
     * @param quantity Quantidade
     * @param quality Qualidade
     * @param notes Notas
     * @param tab Aba
     * @param errorReason Razão do erro (saída)
     * @return ID do armazenamento, ou 0 se falhar
     */
    DWORD AddItemToGuildStorage(DWORD guildID, DWORD playerID, DWORD itemID, WORD quantity, 
                              BYTE quality, const std::string& notes, BYTE tab, std::string& errorReason);
    
    /**
     * @brief Remove um item do armazenamento de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @param storageID ID do armazenamento
     * @param quantity Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveItemFromGuildStorage(DWORD guildID, DWORD playerID, DWORD storageID, 
                                  WORD quantity, std::string& errorReason);
    
    /**
     * @brief Obtém itens do armazenamento de guilda
     * @param guildID ID da guilda
     * @param tab Aba
     * @return Lista de itens
     */
    std::vector<GuildStorageItem> GetGuildStorageItems(DWORD guildID, BYTE tab = 0) const;
    
    /**
     * @brief Constrói uma estrutura de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @param type Tipo
     * @param name Nome
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param territoryID ID do território
     * @param errorReason Razão do erro (saída)
     * @return ID da estrutura, ou 0 se falhar
     */
    DWORD BuildGuildStructure(DWORD guildID, DWORD playerID, GuildStructureType type, 
                            const std::string& name, float posX, float posY, float posZ, 
                            DWORD territoryID, std::string& errorReason);
    
    /**
     * @brief Melhora uma estrutura de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @param structureID ID da estrutura
     * @param errorReason Razão do erro (saída)
     * @return true se melhorado com sucesso
     */
    bool UpgradeGuildStructure(DWORD guildID, DWORD playerID, DWORD structureID, std::string& errorReason);
    
    /**
     * @brief Repara uma estrutura de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @param structureID ID da estrutura
     * @param errorReason Razão do erro (saída)
     * @return true se reparado com sucesso
     */
    bool RepairGuildStructure(DWORD guildID, DWORD playerID, DWORD structureID, std::string& errorReason);
    
    /**
     * @brief Obtém as estruturas de uma guilda
     * @param guildID ID da guilda
     * @param territoryID ID do território (0 para todos)
     * @return Lista de estruturas
     */
    std::vector<GuildStructure> GetGuildStructures(DWORD guildID, DWORD territoryID = 0) const;
    
    /**
     * @brief Conquista um território
     * @param guildID ID da guilda
     * @param territoryID ID do território
     * @param errorReason Razão do erro (saída)
     * @return true se conquistado com sucesso
     */
    bool ConquerTerritory(DWORD guildID, DWORD territoryID, std::string& errorReason);
    
    /**
     * @brief Perde um território
     * @param guildID ID da guilda
     * @param territoryID ID do território
     * @param reason Razão
     * @return true se perdido com sucesso
     */
    bool LoseTerritory(DWORD guildID, DWORD territoryID, const std::string& reason);
    
    /**
     * @brief Obtém os territórios de uma guilda
     * @param guildID ID da guilda
     * @return Lista de territórios
     */
    std::vector<Territory> GetGuildTerritories(DWORD guildID) const;
    
    /**
     * @brief Obtém todos os territórios
     * @return Lista de territórios
     */
    std::vector<Territory> GetAllTerritories() const;
    
    /**
     * @brief Obtém um território
     * @param territoryID ID do território
     * @return Ponteiro para o território, ou nullptr se não existir
     */
    const Territory* GetTerritory(DWORD territoryID) const;
    
    /**
     * @brief Declara guerra a outra guilda
     * @param attackerGuildID ID da guilda atacante
     * @param playerID ID do jogador que solicitou
     * @param defenderGuildID ID da guilda defensora
     * @param type Tipo
     * @param territoryID ID do território (0 se não for por território)
     * @param terms Termos
     * @param errorReason Razão do erro (saída)
     * @return ID da guerra, ou 0 se falhar
     */
    DWORD DeclareGuildWar(DWORD attackerGuildID, DWORD playerID, DWORD defenderGuildID, 
                        GuildWarType type, DWORD territoryID, const std::string& terms, 
                        std::string& errorReason);
    
    /**
     * @brief Responde a uma declaração de guerra
     * @param defenderGuildID ID da guilda defensora
     * @param playerID ID do jogador que solicitou
     * @param warID ID da guerra
     * @param accept Se aceita
     * @param errorReason Razão do erro (saída)
     * @return true se respondido com sucesso
     */
    bool RespondToWarDeclaration(DWORD defenderGuildID, DWORD playerID, DWORD warID, 
                               bool accept, std::string& errorReason);
    
    /**
     * @brief Agenda uma batalha de guerra
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param warID ID da guerra
     * @param startTime Hora de início
     * @param locationID ID da localização
     * @param errorReason Razão do erro (saída)
     * @return ID da batalha, ou 0 se falhar
     */
    DWORD ScheduleWarBattle(DWORD guildID, DWORD playerID, DWORD warID, time_t startTime, 
                          DWORD locationID, std::string& errorReason);
    
    /**
     * @brief Obtém guerras ativas
     * @param guildID ID da guilda (0 para todas)
     * @param status Status (GuildWarStatus::ACTIVE para ativas)
     * @return Lista de guerras
     */
    std::vector<GuildWar> GetWars(DWORD guildID = 0, GuildWarStatus status = GuildWarStatus::ACTIVE) const;
    
    /**
     * @brief Obtém uma guerra
     * @param warID ID da guerra
     * @return Ponteiro para a guerra, ou nullptr se não existir
     */
    const GuildWar* GetWar(DWORD warID) const;
    
    /**
     * @brief Obtém batalhas de uma guerra
     * @param warID ID da guerra
     * @return Lista de batalhas
     */
    std::vector<GuildWarBattle> GetWarBattles(DWORD warID) const;
    
    /**
     * @brief Registra contribuição em batalha
     * @param warID ID da guerra
     * @param battleID ID da batalha
     * @param playerID ID do jogador
     * @param memberID ID do membro
     * @param contribution Contribuição
     * @return true se registrado com sucesso
     */
    bool RegisterWarContribution(DWORD warID, DWORD battleID, DWORD playerID, DWORD memberID, 
                               const GuildWarContribution& contribution);
    
    /**
     * @brief Encerra uma guerra
     * @param warID ID da guerra
     * @param winnerGuildID ID da guilda vencedora (0 para empate)
     * @param playerID ID do jogador que solicitou
     * @param errorReason Razão do erro (saída)
     * @return true se encerrado com sucesso
     */
    bool EndGuildWar(DWORD warID, DWORD winnerGuildID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Forma uma aliança entre guildas
     * @param guildID1 ID da guilda 1
     * @param playerID ID do jogador que solicitou
     * @param guildID2 ID da guilda 2
     * @param type Tipo
     * @param terms Termos
     * @param expiryDate Data de expiração
     * @param errorReason Razão do erro (saída)
     * @return ID da relação, ou 0 se falhar
     */
    DWORD FormGuildAlliance(DWORD guildID1, DWORD playerID, DWORD guildID2, 
                          GuildRelationType type, const std::string& terms, 
                          time_t expiryDate, std::string& errorReason);
    
    /**
     * @brief Responde a uma proposta de aliança
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param relationID ID da relação
     * @param accept Se aceita
     * @param errorReason Razão do erro (saída)
     * @return true se respondido com sucesso
     */
    bool RespondToAllianceProposal(DWORD guildID, DWORD playerID, DWORD relationID, 
                                 bool accept, std::string& errorReason);
    
    /**
     * @brief Encerra uma relação entre guildas
     * @param relationID ID da relação
     * @param guildID ID da guilda que solicitou
     * @param playerID ID do jogador que solicitou
     * @param errorReason Razão do erro (saída)
     * @return true se encerrado com sucesso
     */
    bool EndGuildRelation(DWORD relationID, DWORD guildID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Obtém relações de uma guilda
     * @param guildID ID da guilda
     * @param type Tipo (GuildRelationType::NEUTRAL para todos)
     * @return Lista de relações
     */
    std::vector<GuildRelation> GetGuildRelations(DWORD guildID, 
                                               GuildRelationType type = GuildRelationType::NEUTRAL) const;
    
    /**
     * @brief Obtém o tipo de relação entre duas guildas
     * @param guildID1 ID da guilda 1
     * @param guildID2 ID da guilda 2
     * @return Tipo de relação
     */
    GuildRelationType GetGuildRelationType(DWORD guildID1, DWORD guildID2) const;
    
    /**
     * @brief Contribui com ouro para uma guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @param amount Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se contribuído com sucesso
     */
    bool ContributeGoldToGuild(DWORD guildID, DWORD playerID, DWORD amount, std::string& errorReason);
    
    /**
     * @brief Saca ouro de uma guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @param amount Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se sacado com sucesso
     */
    bool WithdrawGoldFromGuild(DWORD guildID, DWORD playerID, DWORD amount, std::string& errorReason);
    
    /**
     * @brief Adiciona pontos de experiência a uma guilda
     * @param guildID ID da guilda
     * @param amount Quantidade
     * @param source Fonte
     * @return true se adicionado com sucesso
     */
    bool AddGuildExperience(DWORD guildID, DWORD amount, const std::string& source);
    
    /**
     * @brief Obtém as estatísticas do gerenciador
     * @return Estatísticas
     */
    GuildManagerStats GetStatistics() const;
    
    /**
     * @brief Gera log de guilda
     * @param guildID ID da guilda
     * @param eventType Tipo de evento
     * @param actorID ID do ator
     * @param targetID ID do alvo
     * @param details Detalhes
     * @return ID do log, ou 0 se falhar
     */
    DWORD LogGuildEvent(DWORD guildID, GuildEventType eventType, DWORD actorID, 
                      DWORD targetID, const std::string& details);
    
    /**
     * @brief Verifica se um nome de guilda está disponível
     * @param name Nome
     * @return true se estiver disponível
     */
    bool IsGuildNameAvailable(const std::string& name) const;
    
    /**
     * @brief Verifica se uma tag de guilda está disponível
     * @param tag Tag
     * @return true se estiver disponível
     */
    bool IsGuildTagAvailable(const std::string& tag) const;
    
    /**
     * @brief Obtém o número de membros online de uma guilda
     * @param guildID ID da guilda
     * @return Número de membros online
     */
    DWORD GetGuildOnlineMemberCount(DWORD guildID) const;
    
    /**
     * @brief Define a facção de uma guilda
     * @param guildID ID da guilda
     * @param factionID ID da facção
     * @param playerID ID do jogador que solicitou
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetGuildFaction(DWORD guildID, DWORD factionID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Adiciona um buff de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param name Nome
     * @param description Descrição
     * @param effects Efeitos
     * @param duration Duração (segundos)
     * @param level Nível
     * @param errorReason Razão do erro (saída)
     * @return ID do buff, ou 0 se falhar
     */
    DWORD AddGuildBuff(DWORD guildID, DWORD playerID, const std::string& name, 
                      const std::string& description, const std::string& effects, 
                      DWORD duration, DWORD level, std::string& errorReason);
    
    /**
     * @brief Obtém buffs de uma guilda
     * @param guildID ID da guilda
     * @param activeOnly Apenas ativos
     * @return Lista de buffs
     */
    std::vector<GuildBuff> GetGuildBuffs(DWORD guildID, bool activeOnly = true) const;
    
    /**
     * @brief Adiciona uma quest de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador que solicitou
     * @param name Nome
     * @param description Descrição
     * @param objectiveType Tipo de objetivo
     * @param objectiveTarget Alvo do objetivo
     * @param objectiveCount Contagem do objetivo
     * @param rewardType Tipo de recompensa
     * @param rewardAmount Quantidade de recompensa
     * @param difficulty Dificuldade
     * @param duration Duração (segundos)
     * @param errorReason Razão do erro (saída)
     * @return ID da quest, ou 0 se falhar
     */
    DWORD AddGuildQuest(DWORD guildID, DWORD playerID, const std::string& name, 
                       const std::string& description, DWORD objectiveType, DWORD objectiveTarget, 
                       DWORD objectiveCount, DWORD rewardType, DWORD rewardAmount, 
                       DWORD difficulty, DWORD duration, std::string& errorReason);
    
    /**
     * @brief Obtém quests de uma guilda
     * @param guildID ID da guilda
     * @param completedOnly Apenas completas
     * @return Lista de quests
     */
    std::vector<GuildQuest> GetGuildQuests(DWORD guildID, bool completedOnly = false) const;
    
    /**
     * @brief Atualiza progresso de quest de guilda
     * @param guildID ID da guilda
     * @param questID ID da quest
     * @param count Contagem
     * @return true se atualizado com sucesso
     */
    bool UpdateGuildQuestProgress(DWORD guildID, DWORD questID, DWORD count);
    
    /**
     * @brief Completa uma quest de guilda
     * @param guildID ID da guilda
     * @param questID ID da quest
     * @param playerID ID do jogador que solicitou
     * @param errorReason Razão do erro (saída)
     * @return true se completado com sucesso
     */
    bool CompleteGuildQuest(DWORD guildID, DWORD questID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Calcula e aplica renda de território
     * @param territoryID ID do território
     * @return Quantidade gerada
     */
    DWORD CalculateAndApplyTerritoryIncome(DWORD territoryID);
    
    /**
     * @brief Adiciona uma notificação de guilda
     * @param guildID ID da guilda
     * @param message Mensagem
     * @param priority Prioridade
     * @param creatorID ID do criador (0 para sistema)
     * @return ID da notificação, ou 0 se falhar
     */
    DWORD AddGuildNotification(DWORD guildID, const std::string& message, DWORD priority, DWORD creatorID = 0);
    
    /**
     * @brief Obtém notificações de uma guilda
     * @param guildID ID da guilda
     * @param count Contagem
     * @return Lista de notificações
     */
    std::vector<GuildNotification> GetGuildNotifications(DWORD guildID, size_t count = 10) const;
    
    /**
     * @brief Registra um callback para eventos de guilda
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterGuildEventCallback(GuildEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterGuildEventCallback(int callbackID);
    
    /**
     * @brief Registra um callback para verificação de permissão
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterGuildPermissionCheckCallback(GuildPermissionCheckCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterGuildPermissionCheckCallback(int callbackID);
    
    /**
     * @brief Registra um callback para contribuição de guerra
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterGuildWarContributionCallback(GuildWarContributionCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterGuildWarContributionCallback(int callbackID);
    
    /**
     * @brief Registra um callback para atualização de território
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterTerritoryUpdateCallback(TerritoryUpdateCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterTerritoryUpdateCallback(int callbackID);
    
    /**
     * @brief Salva dados de guilda
     * @param guildID ID da guilda (0 para todas)
     * @return true se salvo com sucesso
     */
    bool SaveGuildData(DWORD guildID = 0);
    
    /**
     * @brief Carrega dados de guilda
     * @param guildID ID da guilda (0 para todas)
     * @return true se carregado com sucesso
     */
    bool LoadGuildData(DWORD guildID = 0);
    
    /**
     * @brief Processa eventos de guerra
     * @param elapsed Tempo decorrido
     */
    void ProcessWarEvents(DWORD elapsed);
    
    /**
     * @brief Processa eventos de território
     * @param elapsed Tempo decorrido
     */
    void ProcessTerritoryEvents(DWORD elapsed);
    
    /**
     * @brief Processa eventos de buff
     * @param elapsed Tempo decorrido
     */
    void ProcessBuffEvents(DWORD elapsed);
    
    /**
     * @brief Processa eventos de quest
     * @param elapsed Tempo decorrido
     */
    void ProcessQuestEvents(DWORD elapsed);
    
    /**
     * @brief Processa eventos de relação
     * @param elapsed Tempo decorrido
     */
    void ProcessRelationEvents(DWORD elapsed);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    GuildManager();
    
    /**
     * @brief Destrutor
     */
    ~GuildManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    GuildManager(const GuildManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    GuildManager& operator=(const GuildManager&) = delete;
    
    /**
     * @brief Thread de autosave
     */
    void AutosaveThread();
    
    /**
     * @brief Thread de processamento de eventos
     */
    void EventsThread();
    
    /**
     * @brief Notifica callbacks de evento de guilda
     * @param guildID ID da guilda
     * @param eventType Tipo de evento
     * @param actorID ID do ator
     * @param targetID ID do alvo
     * @param details Detalhes
     */
    void NotifyGuildEventCallbacks(DWORD guildID, GuildEventType eventType, DWORD actorID, DWORD targetID, const std::string& details);
    
    /**
     * @brief Verifica permissão de guilda
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @param permission Permissão
     * @return true se tiver a permissão
     */
    bool CheckGuildPermission(DWORD guildID, DWORD playerID, GuildPermission permission) const;
    
    /**
     * @brief Calcula o próximo nível de guilda
     * @param guildID ID da guilda
     * @return true se subiu de nível
     */
    bool CalculateGuildLevelUp(DWORD guildID);
    
    /**
     * @brief Atualiza a classificação de guildas
     */
    void UpdateGuildRankings();
    
    /**
     * @brief Carrega uma guilda do banco de dados
     * @param guildID ID da guilda
     * @return Ponteiro para a guilda, ou nullptr se falhar
     */
    Guild* LoadGuildFromDB(DWORD guildID);
    
    /**
     * @brief Carrega membros de guilda do banco de dados
     * @param guildID ID da guilda
     * @return Lista de membros
     */
    std::vector<GuildMember> LoadGuildMembersFromDB(DWORD guildID);
    
    /**
     * @brief Carrega habilidades de guilda do banco de dados
     * @param guildID ID da guilda
     * @return Lista de habilidades
     */
    std::vector<GuildSkill> LoadGuildSkillsFromDB(DWORD guildID);
    
    /**
     * @brief Carrega ranks de guilda do banco de dados
     * @param guildID ID da guilda
     * @return Lista de ranks
     */
    std::vector<GuildCustomRank> LoadGuildRanksFromDB(DWORD guildID);
    
    /**
     * @brief Carrega estruturas de guilda do banco de dados
     * @param guildID ID da guilda
     * @return Lista de estruturas
     */
    std::vector<GuildStructure> LoadGuildStructuresFromDB(DWORD guildID);
    
    /**
     * @brief Carrega itens de armazenamento de guilda do banco de dados
     * @param guildID ID da guilda
     * @return Lista de itens
     */
    std::vector<GuildStorageItem> LoadGuildStorageFromDB(DWORD guildID);
    
    /**
     * @brief Carrega registros de guilda do banco de dados
     * @param guildID ID da guilda
     * @param count Contagem
     * @return Lista de registros
     */
    std::vector<GuildLog> LoadGuildLogFromDB(DWORD guildID, size_t count);
    
    /**
     * @brief Carrega relações de guilda do banco de dados
     * @param guildID ID da guilda
     * @return Lista de relações
     */
    std::vector<GuildRelation> LoadGuildRelationsFromDB(DWORD guildID);
    
    /**
     * @brief Carrega territórios de guilda do banco de dados
     * @param guildID ID da guilda
     * @return Lista de territórios
     */
    std::vector<Territory> LoadGuildTerritoriesFromDB(DWORD guildID);
    
    /**
     * @brief Carrega guerras de guilda do banco de dados
     * @param guildID ID da guilda
     * @param status Status
     * @return Lista de guerras
     */
    std::vector<GuildWar> LoadGuildWarsFromDB(DWORD guildID, GuildWarStatus status);
    
    /**
     * @brief Carrega batalhas de guerra do banco de dados
     * @param warID ID da guerra
     * @return Lista de batalhas
     */
    std::vector<GuildWarBattle> LoadWarBattlesFromDB(DWORD warID);
    
    /**
     * @brief Carrega buffs de guilda do banco de dados
     * @param guildID ID da guilda
     * @param activeOnly Apenas ativos
     * @return Lista de buffs
     */
    std::vector<GuildBuff> LoadGuildBuffsFromDB(DWORD guildID, bool activeOnly);
    
    /**
     * @brief Carrega quests de guilda do banco de dados
     * @param guildID ID da guilda
     * @param completedOnly Apenas completas
     * @return Lista de quests
     */
    std::vector<GuildQuest> LoadGuildQuestsFromDB(DWORD guildID, bool completedOnly);
    
    /**
     * @brief Carrega notificações de guilda do banco de dados
     * @param guildID ID da guilda
     * @param count Contagem
     * @return Lista de notificações
     */
    std::vector<GuildNotification> LoadGuildNotificationsFromDB(DWORD guildID, size_t count);
    
    /**
     * @brief Calcula a pontuação de território para uma guilda
     * @param guildID ID da guilda
     * @return Pontuação
     */
    DWORD CalculateTerritoryScore(DWORD guildID) const;
    
    /**
     * @brief Calcula a pontuação de guerra para uma guilda
     * @param guildID ID da guilda
     * @return Pontuação
     */
    DWORD CalculateWarScore(DWORD guildID) const;
    
    /**
     * @brief Calcula a pontuação total para uma guilda
     * @param guildID ID da guilda
     * @return Pontuação
     */
    DWORD CalculateTotalScore(DWORD guildID) const;
    
    /**
     * @brief Verifica se um jogador está online
     * @param playerID ID do jogador
     * @return true se estiver online
     */
    bool IsPlayerOnline(DWORD playerID) const;
    
    /**
     * @brief Obtém o ID de membro para um jogador
     * @param guildID ID da guilda
     * @param playerID ID do jogador
     * @return ID do membro, ou 0 se não existir
     */
    DWORD GetMemberID(DWORD guildID, DWORD playerID) const;
    
    /**
     * @brief Verifica se uma guilda pode possuir mais um território
     * @param guildID ID da guilda
     * @return true se puder
     */
    bool CanGuildOwnMoreTerritories(DWORD guildID) const;
    
    /**
     * @brief Verifica se uma guilda pode engajar em mais uma guerra
     * @param guildID ID da guilda
     * @return true se puder
     */
    bool CanGuildEngageInMoreWars(DWORD guildID) const;
    
    /**
     * @brief Verifica se uma guilda pode formar mais uma aliança
     * @param guildID ID da guilda
     * @return true se puder
     */
    bool CanGuildFormMoreAlliances(DWORD guildID) const;
    
    /**
     * @brief Adiciona uma tarefa assíncrona à fila
     * @param task Tarefa
     */
    void AddAsyncTask(std::function<void()> task);
    
    /**
     * @brief Atualiza estatísticas
     */
    void UpdateStatistics();
    
    // Configuração
    GuildManagerConfig config_;
    
    // Guildas
    std::unordered_map<DWORD, Guild> guilds_;
    mutable std::shared_mutex guildsMutex_;
    
    // Membros
    std::unordered_map<DWORD, std::unordered_map<DWORD, GuildMember>> members_;
    mutable std::shared_mutex membersMutex_;
    
    // Ranks
    std::unordered_map<DWORD, std::vector<GuildCustomRank>> ranks_;
    mutable std::shared_mutex ranksMutex_;
    
    // Habilidades
    std::unordered_map<DWORD, std::unordered_map<DWORD, GuildSkill>> skills_;
    mutable std::shared_mutex skillsMutex_;
    
    // Estruturas
    std::unordered_map<DWORD, std::unordered_map<DWORD, GuildStructure>> structures_;
    mutable std::shared_mutex structuresMutex_;
    
    // Armazenamento
    std::unordered_map<DWORD, std::unordered_map<DWORD, GuildStorageItem>> storageItems_;
    mutable std::shared_mutex storageItemsMutex_;
    
    // Logs
    std::unordered_map<DWORD, std::vector<GuildLog>> logs_;
    mutable std::shared_mutex logsMutex_;
    
    // Relações
    std::unordered_map<DWORD, std::vector<GuildRelation>> relations_;
    mutable std::shared_mutex relationsMutex_;
    
    // Territórios
    std::unordered_map<DWORD, Territory> territories_;
    mutable std::shared_mutex territoriesMutex_;
    
    // Guerras
    std::unordered_map<DWORD, GuildWar> wars_;
    mutable std::shared_mutex warsMutex_;
    
    // Batalhas
    std::unordered_map<DWORD, std::vector<GuildWarBattle>> battles_;
    mutable std::shared_mutex battlesMutex_;
    
    // Contribuições de guerra
    std::unordered_map<DWORD, std::vector<GuildWarContribution>> warContributions_;
    mutable std::shared_mutex warContributionsMutex_;
    
    // Buffs
    std::unordered_map<DWORD, std::vector<GuildBuff>> buffs_;
    mutable std::shared_mutex buffsMutex_;
    
    // Quests
    std::unordered_map<DWORD, std::vector<GuildQuest>> quests_;
    mutable std::shared_mutex questsMutex_;
    
    // Notificações
    std::unordered_map<DWORD, std::vector<GuildNotification>> notifications_;
    mutable std::shared_mutex notificationsMutex_;
    
    // Índices
    std::unordered_map<std::string, DWORD> guildNameIndex_;
    std::unordered_map<std::string, DWORD> guildTagIndex_;
    std::unordered_map<DWORD, DWORD> playerGuildIndex_;
    mutable std::shared_mutex indicesMutex_;
    
    // Callbacks
    std::map<int, GuildEventCallback> guildEventCallbacks_;
    std::mutex guildEventCallbacksMutex_;
    int nextGuildEventCallbackID_;
    
    std::map<int, GuildPermissionCheckCallback> permissionCheckCallbacks_;
    std::mutex permissionCheckCallbacksMutex_;
    int nextPermissionCheckCallbackID_;
    
    std::map<int, GuildWarContributionCallback> warContributionCallbacks_;
    std::mutex warContributionCallbacksMutex_;
    int nextWarContributionCallbackID_;
    
    std::map<int, TerritoryUpdateCallback> territoryUpdateCallbacks_;
    std::mutex territoryUpdateCallbacksMutex_;
    int nextTerritoryUpdateCallbackID_;
    
    // Threads
    std::thread autosaveThread_;
    std::thread eventsThread_;
    std::atomic<bool> running_;
    
    // Tarefas assíncronas
    std::queue<std::function<void()>> asyncTasks_;
    std::mutex asyncTasksMutex_;
    std::condition_variable asyncTasksCV_;
    
    // Estatísticas
    GuildManagerStats stats_;
    mutable std::mutex statsMutex_;
    
    // Estado
    bool initialized_;
    time_t lastAutosaveTime_;
    time_t lastWarEventTime_;
    time_t lastTerritoryEventTime_;
    time_t lastBuffEventTime_;
    time_t lastQuestEventTime_;
    time_t lastRelationEventTime_;
    time_t lastRankingUpdateTime_;
    
    // IDs únicos
    std::atomic<DWORD> nextGuildID_;
    std::atomic<DWORD> nextMemberID_;
    std::atomic<DWORD> nextRankID_;
    std::atomic<DWORD> nextSkillID_;
    std::atomic<DWORD> nextStructureID_;
    std::atomic<DWORD> nextStorageID_;
    std::atomic<DWORD> nextLogID_;
    std::atomic<DWORD> nextRelationID_;
    std::atomic<DWORD> nextTerritoryID_;
    std::atomic<DWORD> nextWarID_;
    std::atomic<DWORD> nextBattleID_;
    std::atomic<DWORD> nextContributionID_;
    std::atomic<DWORD> nextBuffID_;
    std::atomic<DWORD> nextQuestID_;
    std::atomic<DWORD> nextNotificationID_;
};

// Acesso global
#define g_GuildManager GuildManager::GetInstance()

} // namespace guild
} // namespace server
} // namespace wyd

#endif // GUILDMANAGER_H

} // namespace wydbr
