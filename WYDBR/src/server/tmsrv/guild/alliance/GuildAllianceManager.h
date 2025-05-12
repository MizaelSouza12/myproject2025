/**
 * GuildAllianceManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/guild/alliance/GuildAllianceManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef GUILDALLIANCEMANAGER_H
#define GUILDALLIANCEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>

#include "../../../../core/WYDTypes.h"
#include "../GuildTypes.h"
#include "../../player/CPlayer.h"
#include "../management/GuildManager.h"

namespace wydbr {


/**
 * @file GuildAllianceManager.h
 * @brief Gerenciador de alianças entre guilds
 * 
 * Este arquivo contém a definição do gerenciador de alianças entre guilds,
 * responsável por administrar todas as alianças, seus estados e relacionamentos.
 */

namespace wyd {
namespace server {
namespace guild {

// Forward declarations
class GuildManager;

namespace alliance {

/**
 * @brief Estrutura de atualização de aliança
 */
struct AllianceUpdate {
    WORD allianceID;               // ID da aliança
    AllianceLevel oldLevel;        // Nível antigo
    AllianceLevel newLevel;        // Novo nível
    bool active;                    // Se está ativa
    GuildID guild1ID;              // ID da guild 1
    GuildID guild2ID;              // ID da guild 2
    std::string message;           // Mensagem de atualização
};

/**
 * @brief Gerenciador de alianças entre guilds
 */
/**
 * Classe WYDGuildAllianceManager
 * 
 * Esta classe implementa a funcionalidade WYDGuildAllianceManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildAllianceManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static GuildAllianceManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param guildManager Gerenciador de guild
     * @return true se inicializado com sucesso
     */
    bool Initialize(GuildManager* guildManager);
    
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
     * @brief Cria uma nova aliança
     * @param guildID1 ID da guild 1
     * @param guildID2 ID da guild 2
     * @param level Nível de aliança
     * @param terms Termos
     * @param expiryDays Dias até expirar (0 para nunca)
     * @param errorReason Razão do erro (saída)
     * @return ID da aliança, ou 0 se falhar
     */
    WORD CreateAlliance(GuildID guildID1, GuildID guildID2, AllianceLevel level,
                      const std::string& terms, BYTE expiryDays, std::string& errorReason);
    
    /**
     * @brief Propõe uma aliança
     * @param proposerGuildID ID da guild proponente
     * @param targetGuildID ID da guild alvo
     * @param level Nível de aliança
     * @param terms Termos
     * @param expiryDays Dias até expirar (0 para nunca)
     * @param errorReason Razão do erro (saída)
     * @return ID da proposta, ou 0 se falhar
     */
    WORD ProposeAlliance(GuildID proposerGuildID, GuildID targetGuildID, AllianceLevel level,
                       const std::string& terms, BYTE expiryDays, std::string& errorReason);
    
    /**
     * @brief Aceita uma proposta de aliança
     * @param proposalID ID da proposta
     * @param acceptorGuildID ID da guild que está aceitando
     * @param errorReason Razão do erro (saída)
     * @return ID da aliança, ou 0 se falhar
     */
    WORD AcceptAllianceProposal(WORD proposalID, GuildID acceptorGuildID, std::string& errorReason);
    
    /**
     * @brief Rejeita uma proposta de aliança
     * @param proposalID ID da proposta
     * @param rejectorGuildID ID da guild que está rejeitando
     * @param errorReason Razão do erro (saída)
     * @return true se rejeitado com sucesso
     */
    bool RejectAllianceProposal(WORD proposalID, GuildID rejectorGuildID, std::string& errorReason);
    
    /**
     * @brief Cancela uma proposta de aliança
     * @param proposalID ID da proposta
     * @param cancelerGuildID ID da guild que está cancelando
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelAllianceProposal(WORD proposalID, GuildID cancelerGuildID, std::string& errorReason);
    
    /**
     * @brief Rompe uma aliança
     * @param allianceID ID da aliança
     * @param breakerGuildID ID da guild que está rompendo
     * @param reason Razão do rompimento
     * @param errorReason Razão do erro (saída)
     * @return true se rompido com sucesso
     */
    bool BreakAlliance(WORD allianceID, GuildID breakerGuildID, const std::string& reason,
                     std::string& errorReason);
    
    /**
     * @brief Atualiza o nível de uma aliança
     * @param allianceID ID da aliança
     * @param newLevel Novo nível
     * @param updaterGuildID ID da guild que está atualizando
     * @param errorReason Razão do erro (saída)
     * @return true se atualizado com sucesso
     */
    bool UpdateAllianceLevel(WORD allianceID, AllianceLevel newLevel, GuildID updaterGuildID,
                           std::string& errorReason);
    
    /**
     * @brief Atualiza os termos de uma aliança
     * @param allianceID ID da aliança
     * @param newTerms Novos termos
     * @param updaterGuildID ID da guild que está atualizando
     * @param errorReason Razão do erro (saída)
     * @return true se atualizado com sucesso
     */
    bool UpdateAllianceTerms(WORD allianceID, const std::string& newTerms, GuildID updaterGuildID,
                           std::string& errorReason);
    
    /**
     * @brief Renova uma aliança
     * @param allianceID ID da aliança
     * @param renewerGuildID ID da guild que está renovando
     * @param additionalDays Dias adicionais (0 para usar o padrão)
     * @param errorReason Razão do erro (saída)
     * @return true se renovado com sucesso
     */
    bool RenewAlliance(WORD allianceID, GuildID renewerGuildID, BYTE additionalDays,
                     std::string& errorReason);
    
    /**
     * @brief Obtém uma aliança
     * @param allianceID ID da aliança
     * @return Ponteiro para a aliança, ou nullptr se não existir
     */
    const GuildAlliance* GetAlliance(WORD allianceID) const;
    
    /**
     * @brief Obtém uma aliança (para edição)
     * @param allianceID ID da aliança
     * @return Ponteiro para a aliança, ou nullptr se não existir
     */
    GuildAlliance* GetAllianceForEdit(WORD allianceID);
    
    /**
     * @brief Obtém todas as alianças
     * @return Lista de alianças
     */
    std::vector<const GuildAlliance*> GetAllAlliances() const;
    
    /**
     * @brief Obtém alianças ativas
     * @return Lista de alianças ativas
     */
    std::vector<const GuildAlliance*> GetActiveAlliances() const;
    
    /**
     * @brief Obtém alianças de uma guild
     * @param guildID ID da guild
     * @return Lista de alianças
     */
    std::vector<const GuildAlliance*> GetAlliancesByGuild(GuildID guildID) const;
    
    /**
     * @brief Obtém alianças ativas de uma guild
     * @param guildID ID da guild
     * @return Lista de alianças ativas
     */
    std::vector<const GuildAlliance*> GetActiveAlliancesByGuild(GuildID guildID) const;
    
    /**
     * @brief Obtém o nível de aliança entre duas guilds
     * @param guild1ID ID da guild 1
     * @param guild2ID ID da guild 2
     * @return Nível de aliança
     */
    AllianceLevel GetAllianceLevelBetweenGuilds(GuildID guild1ID, GuildID guild2ID) const;
    
    /**
     * @brief Verifica se duas guilds são aliadas
     * @param guild1ID ID da guild 1
     * @param guild2ID ID da guild 2
     * @return true se forem aliadas
     */
    bool AreGuildsAllied(GuildID guild1ID, GuildID guild2ID) const;
    
    /**
     * @brief Obtém o ID da aliança entre duas guilds
     * @param guild1ID ID da guild 1
     * @param guild2ID ID da guild 2
     * @return ID da aliança, ou 0 se não existir
     */
    WORD GetAllianceBetweenGuilds(GuildID guild1ID, GuildID guild2ID) const;
    
    /**
     * @brief Verifica se uma aliança existe
     * @param allianceID ID da aliança
     * @return true se existir
     */
    bool AllianceExists(WORD allianceID) const;
    
    /**
     * @brief Obtém propostas de aliança por guild
     * @param guildID ID da guild
     * @return Lista de IDs de propostas
     */
    std::vector<WORD> GetAllianceProposalsByGuild(GuildID guildID) const;
    
    /**
     * @brief Obtém propostas de aliança enviadas por uma guild
     * @param guildID ID da guild
     * @return Lista de IDs de propostas
     */
    std::vector<WORD> GetAllianceProposalsSentByGuild(GuildID guildID) const;
    
    /**
     * @brief Obtém propostas de aliança recebidas por uma guild
     * @param guildID ID da guild
     * @return Lista de IDs de propostas
     */
    std::vector<WORD> GetAllianceProposalsReceivedByGuild(GuildID guildID) const;
    
    /**
     * @brief Registra um callback para atualizações de aliança
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterAllianceUpdateCallback(std::function<void(const AllianceUpdate&)> callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterAllianceUpdateCallback(int callbackID);
    
    /**
     * @brief Obtém o próximo ID de aliança disponível
     * @return ID de aliança
     */
    WORD GetNextAllianceID();
    
    /**
     * @brief Obtém o próximo ID de proposta disponível
     * @return ID de proposta
     */
    WORD GetNextProposalID();
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    GuildAllianceManager();
    
    /**
     * @brief Destrutor
     */
    ~GuildAllianceManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    GuildAllianceManager(const GuildAllianceManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    GuildAllianceManager& operator=(const GuildAllianceManager&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Processa expiração de alianças
     */
    void ProcessAllianceExpirations();
    
    /**
     * @brief Processa expiração de propostas
     */
    void ProcessProposalExpirations();
    
    /**
     * @brief Notifica callbacks de atualização
     * @param update Atualização
     */
    void NotifyAllianceUpdateCallbacks(const AllianceUpdate& update);
    
    /**
     * @brief Verifica se uma guild pode formar mais alianças
     * @param guildID ID da guild
     * @param errorReason Razão do erro (saída)
     * @return true se puder formar mais
     */
    bool CanGuildFormMoreAlliances(GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Verifica se um nível de aliança é válido
     * @param level Nível
     * @param errorReason Razão do erro (saída)
     * @return true se for válido
     */
    bool IsAllianceLevelValid(AllianceLevel level, std::string& errorReason);
    
    /**
     * @brief Verifica se duas guilds podem formar uma aliança
     * @param guild1ID ID da guild 1
     * @param guild2ID ID da guild 2
     * @param errorReason Razão do erro (saída)
     * @return true se puderem formar
     */
    bool CanGuildsFormAlliance(GuildID guild1ID, GuildID guild2ID, std::string& errorReason);
    
    /**
     * @brief Marca uma aliança como atualizada
     * @param allianceID ID da aliança
     */
    void MarkAllianceAsUpdated(WORD allianceID);
    
    /**
     * @brief Marca uma proposta como atualizada
     * @param proposalID ID da proposta
     */
    void MarkProposalAsUpdated(WORD proposalID);
    
    // Estrutura de proposta de aliança
    struct AllianceProposal {
        WORD proposalID;              // ID da proposta
        GuildID proposerGuildID;      // ID da guild proponente
        GuildID targetGuildID;        // ID da guild alvo
        AllianceLevel level;          // Nível proposto
        std::string terms;             // Termos propostos
        time_t proposalDate;           // Data da proposta
        time_t expiryDate;             // Data de expiração
    };
    
    // Gerenciador de guild
    GuildManager* guildManager_;
    
    // Alianças
    std::map<WORD, GuildAlliance> alliances_;
    mutable std::mutex alliancesMutex_;
    
    // Propostas
    std::map<WORD, AllianceProposal> proposals_;
    mutable std::mutex proposalsMutex_;
    
    // Índices para busca rápida
    std::map<GuildID, std::vector<WORD>> alliancesByGuild_;
    std::map<GuildID, std::vector<WORD>> proposalsSentByGuild_;
    std::map<GuildID, std::vector<WORD>> proposalsReceivedByGuild_;
    
    // Callbacks para atualizações
    std::map<int, std::function<void(const AllianceUpdate&)>> updateCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contadores para novos IDs
    WORD nextAllianceID_;
    WORD nextProposalID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Alianças e propostas atualizadas recentemente (necessitam persistência)
    std::unordered_set<WORD> updatedAlliances_;
    std::mutex updatedAlliancesMutex_;
    std::unordered_set<WORD> updatedProposals_;
    std::mutex updatedProposalsMutex_;
    
    // Estado
    bool initialized_;
};

} // namespace alliance
} // namespace guild
} // namespace server
} // namespace wyd

// Acesso global
#define g_GuildAllianceManager wyd::server::guild::alliance::GuildAllianceManager::GetInstance()

#endif // GUILDALLIANCEMANAGER_H

} // namespace wydbr
