/**
 * GuildWarManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/guild/war/GuildWarManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef GUILDWARMANAGER_H
#define GUILDWARMANAGER_H

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
#include <queue>
#include <condition_variable>

#include "../../../../core/WYDTypes.h"
#include "../GuildTypes.h"
#include "../../player/CPlayer.h"
#include "../management/GuildManager.h"

namespace wydbr {


/**
 * @file GuildWarManager.h
 * @brief Gerenciador do sistema de guerras de guild
 * 
 * Este arquivo contém a definição do gerenciador de guerras de guild,
 * responsável por administrar todas as guerras entre guilds, seus estados,
 * pontuações, e resultados. Corrige bugs críticos do sistema original do WYD
 * e adiciona funcionalidades avançadas.
 */

namespace wyd {
namespace server {
namespace guild {

// Forward declarations
class GuildManager;
class GuildTerritoryManager;

namespace war {

/**
 * @brief Estrutura de atualização de guerra
 */
struct WarUpdate {
    WORD warID;                 // ID da guerra
    WarState oldState;          // Estado antigo
    WarState newState;          // Novo estado
    DWORD attackerScore;        // Pontuação do atacante
    DWORD defenderScore;        // Pontuação do defensor
    DWORD attackerKills;        // Kills do atacante
    DWORD defenderKills;        // Kills do defensor
    BYTE winner;                // Vencedor (0=nenhum, 1=atacante, 2=defensor)
    std::string message;        // Mensagem de atualização
};

/**
 * @brief Gerenciador de guerras de guild
 */
/**
 * Classe WYDGuildWarManager
 * 
 * Esta classe implementa a funcionalidade WYDGuildWarManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildWarManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static GuildWarManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param guildManager Gerenciador de guild
     * @param territoryManager Gerenciador de território
     * @return true se inicializado com sucesso
     */
    bool Initialize(GuildManager* guildManager, GuildTerritoryManager* territoryManager);
    
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
     * @brief Carrega guerras
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadWars(const std::string& filePath);
    
    /**
     * @brief Salva guerras
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveWars(const std::string& filePath);
    
    /**
     * @brief Carrega histórico de guerras
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadWarHistory(const std::string& filePath);
    
    /**
     * @brief Salva histórico de guerras
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveWarHistory(const std::string& filePath);
    
    /**
     * @brief Declara uma guerra
     * @param attackerID ID da guild atacante
     * @param defenderID ID da guild defensora
     * @param type Tipo de guerra
     * @param goldWager Aposta em ouro
     * @param territoryID ID do território em disputa (0 se não for uma guerra territorial)
     * @param terms Termos da guerra
     * @param victoryCondition Condição de vitória
     * @param victoryValue Valor da condição de vitória
     * @param defeatCondition Condição de derrota
     * @param defeatValue Valor da condição de derrota
     * @param restrictions Restrições da guerra
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @param expiryHours Horas até expirar a declaração
     * @param errorReason Razão do erro (saída)
     * @return ID da guerra, ou 0 se falhar
     */
    WORD DeclareWar(GuildID attackerID, GuildID defenderID, WarType type, DWORD goldWager,
                   DWORD territoryID, const std::string& terms, VictoryCondition victoryCondition,
                   DWORD victoryValue, DefeatCondition defeatCondition, DWORD defeatValue,
                   const std::vector<WarRestriction>& restrictions, BYTE minLevel, BYTE maxLevel,
                   BYTE expiryHours, std::string& errorReason);
    
    /**
     * @brief Aceita uma guerra
     * @param warID ID da guerra
     * @param defenderID ID da guild defensora
     * @param errorReason Razão do erro (saída)
     * @return true se aceito com sucesso
     */
    bool AcceptWar(WORD warID, GuildID defenderID, std::string& errorReason);
    
    /**
     * @brief Rejeita uma guerra
     * @param warID ID da guerra
     * @param defenderID ID da guild defensora
     * @param errorReason Razão do erro (saída)
     * @return true se rejeitado com sucesso
     */
    bool RejectWar(WORD warID, GuildID defenderID, std::string& errorReason);
    
    /**
     * @brief Cancela uma guerra
     * @param warID ID da guerra
     * @param guildID ID da guild que está cancelando
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelWar(WORD warID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Inicia uma guerra
     * @param warID ID da guerra
     * @param locationMapID ID do mapa da localização
     * @param locationX Coordenada X da localização
     * @param locationY Coordenada Y da localização
     * @param errorReason Razão do erro (saída)
     * @return true se iniciado com sucesso
     */
    bool StartWar(WORD warID, DWORD locationMapID, WORD locationX, WORD locationY, std::string& errorReason);
    
    /**
     * @brief Encerra uma guerra
     * @param warID ID da guerra
     * @param winner Vencedor (0=nenhum, 1=atacante, 2=defensor)
     * @param reason Razão do encerramento
     * @param errorReason Razão do erro (saída)
     * @return true se encerrado com sucesso
     */
    bool EndWar(WORD warID, BYTE winner, const std::string& reason, std::string& errorReason);
    
    /**
     * @brief Rende-se em uma guerra
     * @param warID ID da guerra
     * @param guildID ID da guild que está se rendendo
     * @param errorReason Razão do erro (saída)
     * @return true se rendido com sucesso
     */
    bool SurrenderWar(WORD warID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Registra um kill em uma guerra
     * @param warID ID da guerra
     * @param killerGuildID ID da guild do assassino
     * @param killerPlayerID ID do jogador assassino
     * @param victimGuildID ID da guild da vítima
     * @param victimPlayerID ID do jogador vítima
     * @param points Pontos ganhos
     * @return true se registrado com sucesso
     */
    bool RegisterWarKill(WORD warID, GuildID killerGuildID, DWORD killerPlayerID,
                        GuildID victimGuildID, DWORD victimPlayerID, DWORD points = 1);
    
    /**
     * @brief Registra pontos em uma guerra
     * @param warID ID da guerra
     * @param guildID ID da guild
     * @param points Pontos
     * @param reason Razão
     * @return true se registrado com sucesso
     */
    bool RegisterWarPoints(WORD warID, GuildID guildID, DWORD points, const std::string& reason);
    
    /**
     * @brief Verifica condição de vitória
     * @param warID ID da guerra
     * @return Vencedor (0=nenhum, 1=atacante, 2=defensor)
     */
    BYTE CheckVictoryCondition(WORD warID);
    
    /**
     * @brief Verifica condição de derrota
     * @param warID ID da guerra
     * @return Perdedor (0=nenhum, 1=atacante, 2=defensor)
     */
    BYTE CheckDefeatCondition(WORD warID);
    
    /**
     * @brief Obtém uma guerra
     * @param warID ID da guerra
     * @return Ponteiro para a guerra, ou nullptr se não existir
     */
    const GuildWar* GetWar(WORD warID) const;
    
    /**
     * @brief Obtém uma guerra (para edição)
     * @param warID ID da guerra
     * @return Ponteiro para a guerra, ou nullptr se não existir
     */
    GuildWar* GetWarForEdit(WORD warID);
    
    /**
     * @brief Obtém todas as guerras
     * @return Lista de guerras
     */
    std::vector<const GuildWar*> GetAllWars() const;
    
    /**
     * @brief Obtém guerras ativas
     * @return Lista de guerras ativas
     */
    std::vector<const GuildWar*> GetActiveWars() const;
    
    /**
     * @brief Obtém guerras pendentes
     * @return Lista de guerras pendentes
     */
    std::vector<const GuildWar*> GetPendingWars() const;
    
    /**
     * @brief Obtém guerras encerradas
     * @return Lista de guerras encerradas
     */
    std::vector<const GuildWar*> GetEndedWars() const;
    
    /**
     * @brief Obtém guerras por guild
     * @param guildID ID da guild
     * @return Lista de guerras
     */
    std::vector<const GuildWar*> GetWarsByGuild(GuildID guildID) const;
    
    /**
     * @brief Obtém guerras ativas por guild
     * @param guildID ID da guild
     * @return Lista de guerras ativas
     */
    std::vector<const GuildWar*> GetActiveWarsByGuild(GuildID guildID) const;
    
    /**
     * @brief Obtém guerras pendentes por guild
     * @param guildID ID da guild
     * @return Lista de guerras pendentes
     */
    std::vector<const GuildWar*> GetPendingWarsByGuild(GuildID guildID) const;
    
    /**
     * @brief Verifica se duas guilds estão em guerra
     * @param guild1ID ID da guild 1
     * @param guild2ID ID da guild 2
     * @return true se estiverem em guerra
     */
    bool AreGuildsAtWar(GuildID guild1ID, GuildID guild2ID) const;
    
    /**
     * @brief Obtém a guerra entre duas guilds
     * @param guild1ID ID da guild 1
     * @param guild2ID ID da guild 2
     * @return ID da guerra, ou 0 se não existir
     */
    WORD GetWarBetweenGuilds(GuildID guild1ID, GuildID guild2ID) const;
    
    /**
     * @brief Conta guerras
     * @return Número de guerras
     */
    size_t CountWars() const;
    
    /**
     * @brief Conta guerras ativas
     * @return Número de guerras ativas
     */
    size_t CountActiveWars() const;
    
    /**
     * @brief Conta guerras pendentes
     * @return Número de guerras pendentes
     */
    size_t CountPendingWars() const;
    
    /**
     * @brief Adiciona um jogador à guerra
     * @param warID ID da guerra
     * @param playerID ID do jogador
     * @param guildID ID da guild
     * @return true se adicionado com sucesso
     */
    bool AddPlayerToWar(WORD warID, DWORD playerID, GuildID guildID);
    
    /**
     * @brief Remove um jogador da guerra
     * @param warID ID da guerra
     * @param playerID ID do jogador
     * @return true se removido com sucesso
     */
    bool RemovePlayerFromWar(WORD warID, DWORD playerID);
    
    /**
     * @brief Verifica se um jogador está em uma guerra
     * @param warID ID da guerra
     * @param playerID ID do jogador
     * @return true se estiver na guerra
     */
    bool IsPlayerInWar(WORD warID, DWORD playerID) const;
    
    /**
     * @brief Verifica se um jogador está em qualquer guerra
     * @param playerID ID do jogador
     * @return ID da guerra, ou 0 se não estiver em guerra
     */
    WORD IsPlayerInAnyWar(DWORD playerID) const;
    
    /**
     * @brief Obtém a guerra em uma localização
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param radius Raio
     * @return ID da guerra, ou 0 se não existir
     */
    WORD GetWarAtLocation(DWORD mapID, WORD x, WORD y, WORD radius = 100) const;
    
    /**
     * @brief Atualiza o estado de um jogador na guerra
     * @param warID ID da guerra
     * @param playerID ID do jogador
     * @param status Status do jogador
     * @return true se atualizado com sucesso
     */
    bool UpdatePlayerWarStatus(WORD warID, DWORD playerID, WarMemberStatus status);
    
    /**
     * @brief Obtém o status de um jogador na guerra
     * @param warID ID da guerra
     * @param playerID ID do jogador
     * @return Status do jogador, ou INACTIVE se não estiver na guerra
     */
    WarMemberStatus GetPlayerWarStatus(WORD warID, DWORD playerID) const;
    
    /**
     * @brief Obtém jogadores em uma guerra
     * @param warID ID da guerra
     * @param guildID ID da guild (0 para todas)
     * @return Lista de IDs de jogadores
     */
    std::vector<DWORD> GetPlayersInWar(WORD warID, GuildID guildID = 0) const;
    
    /**
     * @brief Obtém jogadores ativos em uma guerra
     * @param warID ID da guerra
     * @param guildID ID da guild (0 para todas)
     * @return Lista de IDs de jogadores
     */
    std::vector<DWORD> GetActivePlayersInWar(WORD warID, GuildID guildID = 0) const;
    
    /**
     * @brief Adiciona uma entrada ao histórico de guerras
     * @param entry Entrada
     * @return true se adicionado com sucesso
     */
    bool AddWarHistoryEntry(const GuildWarHistoryEntry& entry);
    
    /**
     * @brief Obtém uma entrada do histórico de guerras
     * @param entryID ID da entrada
     * @return Ponteiro para a entrada, ou nullptr se não existir
     */
    const GuildWarHistoryEntry* GetWarHistoryEntry(WORD entryID) const;
    
    /**
     * @brief Obtém entradas do histórico de guerras por guild
     * @param guildID ID da guild
     * @return Lista de entradas
     */
    std::vector<const GuildWarHistoryEntry*> GetWarHistoryByGuild(GuildID guildID) const;
    
    /**
     * @brief Obtém as últimas entradas do histórico de guerras por guild
     * @param guildID ID da guild
     * @param count Número de entradas
     * @return Lista de entradas
     */
    std::vector<const GuildWarHistoryEntry*> GetRecentWarHistoryByGuild(GuildID guildID, size_t count) const;
    
    /**
     * @brief Obtém todas as entradas do histórico de guerras
     * @return Lista de entradas
     */
    std::vector<const GuildWarHistoryEntry*> GetAllWarHistory() const;
    
    /**
     * @brief Registra um callback para atualizações de guerra
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterWarUpdateCallback(std::function<void(const WarUpdate&)> callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterWarUpdateCallback(int callbackID);
    
    /**
     * @brief Obtém o próximo ID de guerra disponível
     * @return ID de guerra
     */
    WORD GetNextWarID();
    
    /**
     * @brief Verifica se um jogador pode participar de uma guerra
     * @param warID ID da guerra
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se puder participar
     */
    bool CanPlayerParticipateInWar(WORD warID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Verifica restrições de guerra para um jogador
     * @param warID ID da guerra
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se atender às restrições
     */
    bool CheckWarRestrictionsForPlayer(WORD warID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Transmite uma mensagem de guerra
     * @param warID ID da guerra
     * @param message Mensagem
     * @param guildID ID da guild (0 para todas)
     */
    void BroadcastWarMessage(WORD warID, const std::string& message, GuildID guildID = 0);
    
    /**
     * @brief Obtém o número total de jogadores em guerras
     * @return Número de jogadores
     */
    DWORD GetTotalPlayersInWars() const;
    
    /**
     * @brief Distribui recompensas de guerra
     * @param warID ID da guerra
     * @param winner Vencedor (0=nenhum, 1=atacante, 2=defensor)
     * @return true se distribuído com sucesso
     */
    bool DistributeWarRewards(WORD warID, BYTE winner);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    GuildWarManager();
    
    /**
     * @brief Destrutor
     */
    ~GuildWarManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    GuildWarManager(const GuildWarManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    GuildWarManager& operator=(const GuildWarManager&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Processa expiração de guerras
     */
    void ProcessWarExpirations();
    
    /**
     * @brief Verifica condições de guerra
     */
    void CheckWarConditions();
    
    /**
     * @brief Notifica callbacks de atualização
     * @param update Atualização
     */
    void NotifyWarUpdateCallbacks(const WarUpdate& update);
    
    /**
     * @brief Cria uma entrada no histórico de guerras
     * @param war Guerra
     * @return Entrada do histórico
     */
    GuildWarHistoryEntry CreateWarHistoryEntry(const GuildWar& war);
    
    /**
     * @brief Válida um tipo de guerra
     * @param type Tipo de guerra
     * @param goldWager Aposta em ouro
     * @param territoryID ID do território
     * @param errorReason Razão do erro (saída)
     * @return true se válido
     */
    bool ValidateWarType(WarType type, DWORD goldWager, DWORD territoryID, std::string& errorReason);
    
    /**
     * @brief Válida condição de vitória
     * @param condition Condição de vitória
     * @param value Valor
     * @param errorReason Razão do erro (saída)
     * @return true se válido
     */
    bool ValidateVictoryCondition(VictoryCondition condition, DWORD value, std::string& errorReason);
    
    /**
     * @brief Válida condição de derrota
     * @param condition Condição de derrota
     * @param value Valor
     * @param errorReason Razão do erro (saída)
     * @return true se válido
     */
    bool ValidateDefeatCondition(DefeatCondition condition, DWORD value, std::string& errorReason);
    
    /**
     * @brief Válida restrições de guerra
     * @param restrictions Restrições
     * @param errorReason Razão do erro (saída)
     * @return true se válido
     */
    bool ValidateWarRestrictions(const std::vector<WarRestriction>& restrictions, std::string& errorReason);
    
    /**
     * @brief Marca uma guerra como atualizada
     * @param warID ID da guerra
     */
    void MarkWarAsUpdated(WORD warID);
    
    /**
     * @brief Marca uma guerra como expirada
     * @param warID ID da guerra
     */
    void MarkWarAsExpired(WORD warID);
    
    /**
     * @brief Processa uma guerra territorial
     * @param warID ID da guerra
     * @param winner Vencedor (0=nenhum, 1=atacante, 2=defensor)
     */
    void ProcessTerritoryWar(WORD warID, BYTE winner);
    
    /**
     * @brief Processa uma guerra de cerco de castelo
     * @param warID ID da guerra
     * @param winner Vencedor (0=nenhum, 1=atacante, 2=defender)
     */
    void ProcessGuildSiegeWar(WORD warID, BYTE winner);
    
    // Gerenciadores
    GuildManager* guildManager_;
    GuildTerritoryManager* territoryManager_;
    
    // Guerras
    std::map<WORD, GuildWar> wars_;
    mutable std::mutex warsMutex_;
    
    // Histórico de guerras
    std::vector<GuildWarHistoryEntry> warHistory_;
    mutable std::mutex historyMutex_;
    
    // Índices para busca rápida
    std::map<GuildID, std::vector<WORD>> warsByGuild_;
    std::map<WarState, std::vector<WORD>> warsByState_;
    std::map<DWORD, std::map<DWORD, WarMemberStatus>> playerWarStatus_; // mapeia warID -> {playerID -> status}
    std::map<DWORD, WORD> playerActiveWars_; // mapeia playerID -> warID
    
    // Callbacks para atualizações
    std::map<int, std::function<void(const WarUpdate&)>> updateCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contador para novos IDs
    WORD nextWarID_;
    WORD nextWarHistoryID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Guerras atualizadas recentemente (necessitam persistência)
    std::unordered_set<WORD> updatedWars_;
    std::mutex updatedWarsMutex_;
    
    // Estado
    bool initialized_;
};

} // namespace war
} // namespace guild
} // namespace server
} // namespace wyd

// Acesso global
#define g_GuildWarManager wyd::server::guild::war::GuildWarManager::GetInstance()

#endif // GUILDWARMANAGER_H

} // namespace wydbr
