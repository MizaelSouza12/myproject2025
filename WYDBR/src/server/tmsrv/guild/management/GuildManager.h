/**
 * GuildManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/guild/management/GuildManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef GUILDMANAGER_H
#define GUILDMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>

#include "../../../../core/WYDTypes.h"
#include "../GuildTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file GuildManager.h
 * @brief Gerenciador principal do sistema de guild
 * 
 * Este arquivo contém a definição do gerenciador principal de guilds,
 * responsável por toda a administração de guilds, membros, ranks,
 * alianças, e funcionalidades relacionadas. Sistemas de guerra e
 * territórios são gerenciados por classes específicas.
 */

namespace wyd {
namespace server {
namespace guild {

/**
 * @brief Evento de mudança de membro de guild
 */
struct GuildMemberChangeEvent {
    GuildID guildID;              // ID da guild
    std::string guildName;         // Nome da guild
    DWORD playerID;               // ID do jogador
    std::string playerName;        // Nome do jogador
    GuildSystemEventType eventType; // Tipo de evento
    time_t timestamp;              // Timestamp
};

/**
 * @brief Evento de mudança de guild
 */
struct GuildChangeEvent {
    GuildID guildID;              // ID da guild
    std::string guildName;         // Nome da guild
    GuildSystemEventType eventType; // Tipo de evento
    time_t timestamp;              // Timestamp
    std::string details;           // Detalhes adicionais
};

/**
 * @brief Gerenciador principal de guilds
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
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
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
     * @brief Carrega guilds
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadGuilds(const std::string& filePath);
    
    /**
     * @brief Salva guilds
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveGuilds(const std::string& filePath);
    
    /**
     * @brief Carrega alianças
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadAlliances(const std::string& filePath);
    
    /**
     * @brief Salva alianças
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveAlliances(const std::string& filePath);
    
    /**
     * @brief Cria uma nova guild
     * @param founderID ID do fundador
     * @param founderName Nome do fundador
     * @param guildName Nome da guild
     * @param motto Lema
     * @param errorReason Razão do erro (saída)
     * @return ID da guild, ou 0 se falhar
     */
    GuildID CreateGuild(DWORD founderID, const std::string& founderName,
                      const std::string& guildName, const std::string& motto,
                      std::string& errorReason);
    
    /**
     * @brief Dissolve uma guild
     * @param guildID ID da guild
     * @param masterID ID do mestre
     * @param errorReason Razão do erro (saída)
     * @return true se dissolvido com sucesso
     */
    bool DissolveGuild(GuildID guildID, DWORD masterID, std::string& errorReason);
    
    /**
     * @brief Adiciona um membro à guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param playerName Nome do jogador
     * @param playerLevel Nível do jogador
     * @param playerClass Classe do jogador
     * @param inviterID ID do convidador (0 se for o mestre)
     * @param errorReason Razão do erro (saída)
     * @return true se adicionado com sucesso
     */
    bool AddGuildMember(GuildID guildID, DWORD playerID, const std::string& playerName,
                      BYTE playerLevel, BYTE playerClass, DWORD inviterID,
                      std::string& errorReason);
    
    /**
     * @brief Remove um membro da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param kickerID ID de quem está removendo (0 se for o próprio)
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveGuildMember(GuildID guildID, DWORD playerID, DWORD kickerID,
                         std::string& errorReason);
    
    /**
     * @brief Promove um membro da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param promoterID ID de quem está promovendo
     * @param errorReason Razão do erro (saída)
     * @return true se promovido com sucesso
     */
    bool PromoteGuildMember(GuildID guildID, DWORD playerID, DWORD promoterID,
                          std::string& errorReason);
    
    /**
     * @brief Rebaixa um membro da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param demoterID ID de quem está rebaixando
     * @param errorReason Razão do erro (saída)
     * @return true se rebaixado com sucesso
     */
    bool DemoteGuildMember(GuildID guildID, DWORD playerID, DWORD demoterID,
                         std::string& errorReason);
    
    /**
     * @brief Define o rank de um membro da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param rank Rank
     * @param setterID ID de quem está definindo
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetGuildMemberRank(GuildID guildID, DWORD playerID, GuildRank rank,
                          DWORD setterID, std::string& errorReason);
    
    /**
     * @brief Define permissões de um membro da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param permissions Permissões
     * @param setterID ID de quem está definindo
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetGuildMemberPermissions(GuildID guildID, DWORD playerID,
                                 const std::bitset<32>& permissions,
                                 DWORD setterID, std::string& errorReason);
    
    /**
     * @brief Verifica se um membro tem uma permissão
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param permission Permissão
     * @return true se tiver a permissão
     */
    bool HasGuildPermission(GuildID guildID, DWORD playerID, GuildPermission permission);
    
    /**
     * @brief Modifica o emblema da guild
     * @param guildID ID da guild
     * @param emblem Novo emblema
     * @param modifierID ID de quem está modificando
     * @param errorReason Razão do erro (saída)
     * @return true se modificado com sucesso
     */
    bool ModifyGuildEmblem(GuildID guildID, const GuildEmblem& emblem,
                         DWORD modifierID, std::string& errorReason);
    
    /**
     * @brief Modifica o lema da guild
     * @param guildID ID da guild
     * @param motto Novo lema
     * @param modifierID ID de quem está modificando
     * @param errorReason Razão do erro (saída)
     * @return true se modificado com sucesso
     */
    bool ModifyGuildMotto(GuildID guildID, const std::string& motto,
                        DWORD modifierID, std::string& errorReason);
    
    /**
     * @brief Modifica o aviso da guild
     * @param guildID ID da guild
     * @param notice Novo aviso
     * @param modifierID ID de quem está modificando
     * @param errorReason Razão do erro (saída)
     * @return true se modificado com sucesso
     */
    bool ModifyGuildNotice(GuildID guildID, const std::string& notice,
                         DWORD modifierID, std::string& errorReason);
    
    /**
     * @brief Define um rank personalizado
     * @param guildID ID da guild
     * @param rankID ID do rank
     * @param name Nome
     * @param permissions Permissões
     * @param order Ordem
     * @param setterID ID de quem está definindo
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetCustomRank(GuildID guildID, BYTE rankID, const std::string& name,
                     const std::bitset<32>& permissions, BYTE order,
                     DWORD setterID, std::string& errorReason);
    
    /**
     * @brief Remove um rank personalizado
     * @param guildID ID da guild
     * @param rankID ID do rank
     * @param removerID ID de quem está removendo
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveCustomRank(GuildID guildID, BYTE rankID, DWORD removerID,
                        std::string& errorReason);
    
    /**
     * @brief Forma uma aliança
     * @param guild1ID ID da guild 1
     * @param guild2ID ID da guild 2
     * @param level Nível de aliança
     * @param terms Termos
     * @param expiryDays Dias até expirar (0 para nunca)
     * @param errorReason Razão do erro (saída)
     * @return ID da aliança, ou 0 se falhar
     */
    WORD FormAlliance(GuildID guild1ID, GuildID guild2ID, AllianceLevel level,
                    const std::string& terms, BYTE expiryDays,
                    std::string& errorReason);
    
    /**
     * @brief Rompe uma aliança
     * @param allianceID ID da aliança
     * @param guildID ID da guild que está rompendo
     * @param errorReason Razão do erro (saída)
     * @return true se rompido com sucesso
     */
    bool BreakAlliance(WORD allianceID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Obtém uma aliança
     * @param allianceID ID da aliança
     * @return Ponteiro para a aliança, ou nullptr se não existir
     */
    const GuildAlliance* GetAlliance(WORD allianceID) const;
    
    /**
     * @brief Obtém alianças por guild
     * @param guildID ID da guild
     * @return Lista de alianças
     */
    std::vector<const GuildAlliance*> GetAlliancesByGuild(GuildID guildID) const;
    
    /**
     * @brief Verifica se duas guilds são aliadas
     * @param guild1ID ID da guild 1
     * @param guild2ID ID da guild 2
     * @return Nível de aliança
     */
    AllianceLevel GetAllianceLevel(GuildID guild1ID, GuildID guild2ID) const;
    
    /**
     * @brief Deposita ouro no banco da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param amount Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se depositado com sucesso
     */
    bool DepositGuildBankGold(GuildID guildID, DWORD playerID, DWORD amount,
                            std::string& errorReason);
    
    /**
     * @brief Saca ouro do banco da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param amount Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se sacado com sucesso
     */
    bool WithdrawGuildBankGold(GuildID guildID, DWORD playerID, DWORD amount,
                             std::string& errorReason);
    
    /**
     * @brief Obtém o saldo do banco da guild
     * @param guildID ID da guild
     * @return Saldo, ou 0 se a guild não existir
     */
    DWORD GetGuildBankGold(GuildID guildID) const;
    
    /**
     * @brief Deposita um item no banco da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param item Item
     * @param note Nota
     * @param errorReason Razão do erro (saída)
     * @return ID do slot, ou 0 se falhar
     */
    DWORD DepositGuildBankItem(GuildID guildID, DWORD playerID, const GuildBankItem& item,
                              const std::string& note, std::string& errorReason);
    
    /**
     * @brief Saca um item do banco da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param slotID ID do slot
     * @param errorReason Razão do erro (saída)
     * @return Item sacado, ou nullptr se falhar
     */
    const GuildBankItem* WithdrawGuildBankItem(GuildID guildID, DWORD playerID,
                                            DWORD slotID, std::string& errorReason);
    
    /**
     * @brief Obtém os itens do banco da guild
     * @param guildID ID da guild
     * @return Lista de itens
     */
    std::vector<const GuildBankItem*> GetGuildBankItems(GuildID guildID) const;
    
    /**
     * @brief Adiciona uma entrada ao log da guild
     * @param guildID ID da guild
     * @param type Tipo
     * @param actorID ID do ator
     * @param actorName Nome do ator
     * @param targetID ID do alvo
     * @param targetName Nome do alvo
     * @param description Descrição
     * @return true se adicionado com sucesso
     */
    bool AddGuildLogEntry(GuildID guildID, GuildLogType type, DWORD actorID,
                        const std::string& actorName, DWORD targetID,
                        const std::string& targetName, const std::string& description);
    
    /**
     * @brief Obtém as entradas do log da guild
     * @param guildID ID da guild
     * @param count Número de entradas
     * @param offset Deslocamento
     * @return Lista de entradas
     */
    std::vector<const GuildLogEntry*> GetGuildLog(GuildID guildID, size_t count, size_t offset = 0) const;
    
    /**
     * @brief Verifica se a guild existe
     * @param guildID ID da guild
     * @return true se existir
     */
    bool GuildExists(GuildID guildID) const;
    
    /**
     * @brief Verifica se um jogador está em uma guild
     * @param playerID ID do jogador
     * @return ID da guild, ou 0 se não estiver
     */
    GuildID GetPlayerGuild(DWORD playerID) const;
    
    /**
     * @brief Verifica se um jogador é fundador de uma guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @return true se for fundador
     */
    bool IsGuildFounder(GuildID guildID, DWORD playerID) const;
    
    /**
     * @brief Verifica se um jogador é mestre de uma guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @return true se for mestre
     */
    bool IsGuildMaster(GuildID guildID, DWORD playerID) const;
    
    /**
     * @brief Obtém o rank de um membro da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @return Rank, ou MEMBER se não for membro
     */
    GuildRank GetGuildMemberRank(GuildID guildID, DWORD playerID) const;
    
    /**
     * @brief Obtém a guild pelo nome
     * @param guildName Nome da guild
     * @return ID da guild, ou 0 se não existir
     */
    GuildID GetGuildByName(const std::string& guildName) const;
    
    /**
     * @brief Obtém o nome da guild
     * @param guildID ID da guild
     * @return Nome da guild, ou string vazia se não existir
     */
    std::string GetGuildName(GuildID guildID) const;
    
    /**
     * @brief Obtém a informação da guild
     * @param guildID ID da guild
     * @return Ponteiro para a informação, ou nullptr se não existir
     */
    const GuildInfo* GetGuildInfo(GuildID guildID) const;
    
    /**
     * @brief Obtém todas as guilds
     * @return Lista de IDs de guilds
     */
    std::vector<GuildID> GetAllGuilds() const;
    
    /**
     * @brief Obtém o membro da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @return Ponteiro para o membro, ou nullptr se não existir
     */
    const GuildMember* GetGuildMember(GuildID guildID, DWORD playerID) const;
    
    /**
     * @brief Obtém todos os membros da guild
     * @param guildID ID da guild
     * @return Lista de membros
     */
    std::vector<const GuildMember*> GetGuildMembers(GuildID guildID) const;
    
    /**
     * @brief Obtém membros da guild por rank
     * @param guildID ID da guild
     * @param rank Rank
     * @return Lista de membros
     */
    std::vector<const GuildMember*> GetGuildMembersByRank(GuildID guildID, GuildRank rank) const;
    
    /**
     * @brief Obtém membros online da guild
     * @param guildID ID da guild
     * @return Lista de membros
     */
    std::vector<const GuildMember*> GetOnlineGuildMembers(GuildID guildID) const;
    
    /**
     * @brief Marca um membro como online
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @return true se marcado com sucesso
     */
    bool MarkGuildMemberOnline(GuildID guildID, DWORD playerID);
    
    /**
     * @brief Marca um membro como offline
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @return true se marcado com sucesso
     */
    bool MarkGuildMemberOffline(GuildID guildID, DWORD playerID);
    
    /**
     * @brief Adiciona experiência à guild
     * @param guildID ID da guild
     * @param amount Quantidade
     * @return true se adicionado com sucesso
     */
    bool AddGuildExperience(GuildID guildID, DWORD amount);
    
    /**
     * @brief Obtém o nível da guild
     * @param guildID ID da guild
     * @return Nível da guild
     */
    GuildLevel GetGuildLevel(GuildID guildID) const;
    
    /**
     * @brief Adiciona contribuição a um membro
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param amount Quantidade
     * @param contributionType Tipo de contribuição
     * @return true se adicionado com sucesso
     */
    bool AddMemberContribution(GuildID guildID, DWORD playerID, DWORD amount,
                             GuildContributionType contributionType);
    
    /**
     * @brief Obtém a contribuição de um membro
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @return Contribuição
     */
    DWORD GetMemberContribution(GuildID guildID, DWORD playerID) const;
    
    /**
     * @brief Obtém contribuição semanal de um membro
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @return Contribuição semanal
     */
    DWORD GetMemberWeeklyContribution(GuildID guildID, DWORD playerID) const;
    
    /**
     * @brief Define a taxa da guild
     * @param guildID ID da guild
     * @param taxRate Taxa
     * @param setterID ID de quem está definindo
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetGuildTaxRate(GuildID guildID, BYTE taxRate, DWORD setterID, std::string& errorReason);
    
    /**
     * @brief Obtém a taxa da guild
     * @param guildID ID da guild
     * @return Taxa
     */
    BYTE GetGuildTaxRate(GuildID guildID) const;
    
    /**
     * @brief Coleta taxa da guild
     * @param guildID ID da guild
     * @param playerID ID do jogador
     * @param amount Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se coletado com sucesso
     */
    bool CollectGuildTax(GuildID guildID, DWORD playerID, DWORD amount, std::string& errorReason);
    
    /**
     * @brief Transfere a liderança da guild
     * @param guildID ID da guild
     * @param currentMasterID ID do mestre atual
     * @param newMasterID ID do novo mestre
     * @param errorReason Razão do erro (saída)
     * @return true se transferido com sucesso
     */
    bool TransferGuildLeadership(GuildID guildID, DWORD currentMasterID, DWORD newMasterID,
                               std::string& errorReason);
    
    /**
     * @brief Obtém o próximo ID de guild disponível
     * @return ID de guild
     */
    GuildID GetNextGuildID();
    
    /**
     * @brief Obtém o próximo ID de aliança disponível
     * @return ID de aliança
     */
    WORD GetNextAllianceID();
    
    /**
     * @brief Registra um callback para eventos de guild
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterGuildEventCallback(std::function<void(GuildSystemEventType, const void*)> callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterGuildEventCallback(int callbackID);
    
    /**
     * @brief Transmite uma mensagem para a guild
     * @param guildID ID da guild
     * @param message Mensagem
     * @param senderID ID do remetente (0 para sistema)
     * @param senderName Nome do remetente (vazio para sistema)
     */
    void BroadcastGuildMessage(GuildID guildID, const std::string& message,
                             DWORD senderID = 0, const std::string& senderName = "");
    
    /**
     * @brief Obtém guilds por ordem de classificação
     * @param sortType Tipo de classificação (0=nível, 1=membros, 2=guerras vencidas, 3=territórios)
     * @param count Número de guilds
     * @return Lista de IDs de guilds
     */
    std::vector<GuildID> GetGuildsByRanking(BYTE sortType, size_t count) const;
    
    /**
     * @brief Agenda um evento de guild
     * @param guildID ID da guild
     * @param event Evento
     * @param errorReason Razão do erro (saída)
     * @return ID do evento, ou 0 se falhar
     */
    WORD ScheduleGuildEvent(GuildID guildID, const GuildEvent& event, std::string& errorReason);
    
    /**
     * @brief Cancela um evento de guild
     * @param guildID ID da guild
     * @param eventID ID do evento
     * @param cancelerID ID de quem está cancelando
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelGuildEvent(GuildID guildID, WORD eventID, DWORD cancelerID, std::string& errorReason);
    
    /**
     * @brief Obtém eventos da guild
     * @param guildID ID da guild
     * @return Lista de eventos
     */
    std::vector<const GuildEvent*> GetGuildEvents(GuildID guildID) const;
    
    /**
     * @brief Adiciona participante a um evento
     * @param guildID ID da guild
     * @param eventID ID do evento
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se adicionado com sucesso
     */
    bool AddEventParticipant(GuildID guildID, WORD eventID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Remove participante de um evento
     * @param guildID ID da guild
     * @param eventID ID do evento
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveEventParticipant(GuildID guildID, WORD eventID, DWORD playerID, std::string& errorReason);
    
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
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Processa resetamento semanal
     */
    void ProcessWeeklyReset();
    
    /**
     * @brief Notifica callbacks de evento
     * @param eventType Tipo de evento
     * @param data Dados do evento
     */
    void NotifyGuildEventCallbacks(GuildSystemEventType eventType, const void* data);
    
    /**
     * @brief Verifica se um nome de guild é válido
     * @param guildName Nome da guild
     * @param errorReason Razão do erro (saída)
     * @return true se for válido
     */
    bool IsGuildNameValid(const std::string& guildName, std::string& errorReason) const;
    
    /**
     * @brief Verifica se um nome já está em uso
     * @param guildName Nome da guild
     * @return true se já estiver em uso
     */
    bool IsGuildNameInUse(const std::string& guildName) const;
    
    /**
     * @brief Verifica se um jogador pode criar uma guild
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se puder criar
     */
    bool CanPlayerCreateGuild(DWORD playerID, std::string& errorReason) const;
    
    /**
     * @brief Obtém permissões para um rank
     * @param rank Rank
     * @return Permissões
     */
    std::bitset<32> GetDefaultPermissionsForRank(GuildRank rank) const;
    
    /**
     * @brief Obtém experiência necessária para o próximo nível
     * @param level Nível atual
     * @return Experiência necessária
     */
    DWORD GetExpForNextLevel(GuildLevel level) const;
    
    /**
     * @brief Marca uma guild como atualizada
     * @param guildID ID da guild
     */
    void MarkGuildAsUpdated(GuildID guildID);
    
    /**
     * @brief Marca uma aliança como atualizada
     * @param allianceID ID da aliança
     */
    void MarkAllianceAsUpdated(WORD allianceID);
    
    /**
     * @brief Calcula o nível com base na experiência
     * @param experience Experiência
     * @return Nível
     */
    GuildLevel CalculateLevelFromExp(DWORD experience) const;
    
    /**
     * @brief Atualiza os índices internos
     */
    void UpdateInternalIndices();
    
    // Guilds
    std::map<GuildID, GuildInfo> guilds_;
    mutable std::mutex guildsMutex_;
    
    // Membros por guild
    std::map<GuildID, std::map<DWORD, GuildMember>> members_;
    mutable std::mutex membersMutex_;
    
    // Itens de banco por guild
    std::map<GuildID, std::map<DWORD, GuildBankItem>> bankItems_;
    mutable std::mutex bankItemsMutex_;
    
    // Logs por guild
    std::map<GuildID, std::vector<GuildLogEntry>> logs_;
    mutable std::mutex logsMutex_;
    
    // Alianças
    std::map<WORD, GuildAlliance> alliances_;
    mutable std::mutex alliancesMutex_;
    
    // Eventos por guild
    std::map<GuildID, std::vector<GuildEvent>> events_;
    mutable std::mutex eventsMutex_;
    
    // Índices para busca rápida
    std::map<std::string, GuildID> guildsByName_;
    std::map<DWORD, GuildID> playerGuilds_;
    std::map<GuildID, std::vector<WORD>> alliancesByGuild_;
    
    // Callbacks para eventos
    std::map<int, std::function<void(GuildSystemEventType, const void*)>> eventCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contadores para novos IDs
    GuildID nextGuildID_;
    WORD nextAllianceID_;
    DWORD nextLogID_;
    DWORD nextBankItemID_;
    WORD nextEventID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Guilds e alianças atualizadas recentemente (necessitam persistência)
    std::unordered_set<GuildID> updatedGuilds_;
    std::mutex updatedGuildsMutex_;
    std::unordered_set<WORD> updatedAlliances_;
    std::mutex updatedAlliancesMutex_;
    
    // Controle de resetamento semanal
    time_t lastWeeklyReset_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_GuildManager GuildManager::GetInstance()

} // namespace guild
} // namespace server
} // namespace wyd

#endif // GUILDMANAGER_H

} // namespace wydbr
