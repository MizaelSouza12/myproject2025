/**
 * GuildTerritoryManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/guild/territory/GuildTerritoryManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef GUILDTERRITORYMANAGER_H
#define GUILDTERRITORYMANAGER_H

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
 * @file GuildTerritoryManager.h
 * @brief Gerenciador de territórios de guild
 * 
 * Este arquivo contém a definição do gerenciador de territórios de guild,
 * responsável por administrar todos os territórios controlados por guilds,
 * seus recursos, construções e atributos.
 */

namespace wyd {
namespace server {
namespace guild {

// Forward declarations
class GuildManager;

namespace territory {

/**
 * @brief Estrutura de atualização de território
 */
struct TerritoryUpdate {
    WORD territoryID;             // ID do território
    TerritoryStatus oldStatus;    // Status antigo
    TerritoryStatus newStatus;    // Novo status
    GuildID oldOwnerID;           // ID do dono antigo
    GuildID newOwnerID;           // ID do novo dono
    std::string message;          // Mensagem de atualização
};

/**
 * @brief Gerenciador de territórios de guild
 */
/**
 * Classe WYDGuildTerritoryManager
 * 
 * Esta classe implementa a funcionalidade WYDGuildTerritoryManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GuildTerritoryManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static GuildTerritoryManager& GetInstance();
    
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
     * @brief Carrega territórios
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadTerritories(const std::string& filePath);
    
    /**
     * @brief Salva territórios
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveTerritories(const std::string& filePath);
    
    /**
     * @brief Cria um novo território
     * @param name Nome
     * @param type Tipo
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param radius Raio
     * @param resourceType Tipo de recurso
     * @param resourceQuantity Quantidade de recurso
     * @param productionRate Taxa de produção
     * @return ID do território, ou 0 se falhar
     */
    WORD CreateTerritory(const std::string& name, TerritoryType type, DWORD mapID,
                        WORD x, WORD y, WORD radius, TerritoryResourceType resourceType,
                        DWORD resourceQuantity, DWORD productionRate);
    
    /**
     * @brief Remove um território
     * @param territoryID ID do território
     * @return true se removido com sucesso
     */
    bool RemoveTerritory(WORD territoryID);
    
    /**
     * @brief Reivindica um território
     * @param territoryID ID do território
     * @param guildID ID da guild
     * @param errorReason Razão do erro (saída)
     * @return true se reivindicado com sucesso
     */
    bool ClaimTerritory(WORD territoryID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Abandona um território
     * @param territoryID ID do território
     * @param guildID ID da guild
     * @param errorReason Razão do erro (saída)
     * @return true se abandonado com sucesso
     */
    bool AbandonTerritory(WORD territoryID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Captura um território
     * @param territoryID ID do território
     * @param guildID ID da guild
     * @param errorReason Razão do erro (saída)
     * @return true se capturado com sucesso
     */
    bool CaptureTerritory(WORD territoryID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Perde um território
     * @param territoryID ID do território
     * @param reason Razão
     * @return true se perdido com sucesso
     */
    bool LoseTerritory(WORD territoryID, const std::string& reason);
    
    /**
     * @brief Melhora um território
     * @param territoryID ID do território
     * @param guildID ID da guild
     * @param errorReason Razão do erro (saída)
     * @return true se melhorado com sucesso
     */
    bool UpgradeTerritory(WORD territoryID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Melhora a defesa de um território
     * @param territoryID ID do território
     * @param guildID ID da guild
     * @param errorReason Razão do erro (saída)
     * @return true se melhorado com sucesso
     */
    bool UpgradeTerritoryDefense(WORD territoryID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Constrói um edifício no território
     * @param territoryID ID do território
     * @param guildID ID da guild
     * @param buildingType Tipo de edifício
     * @param errorReason Razão do erro (saída)
     * @return true se construído com sucesso
     */
    bool BuildTerritoryBuilding(WORD territoryID, GuildID guildID, TerritoryBuildingType buildingType,
                              std::string& errorReason);
    
    /**
     * @brief Remove um edifício do território
     * @param territoryID ID do território
     * @param guildID ID da guild
     * @param buildingType Tipo de edifício
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveTerritoryBuilding(WORD territoryID, GuildID guildID, TerritoryBuildingType buildingType,
                               std::string& errorReason);
    
    /**
     * @brief Coleta recursos de um território
     * @param territoryID ID do território
     * @param guildID ID da guild
     * @param errorReason Razão do erro (saída)
     * @return Quantidade coletada, ou 0 se falhar
     */
    DWORD CollectTerritoryResources(WORD territoryID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Verifica se pode coletar recursos de um território
     * @param territoryID ID do território
     * @param guildID ID da guild
     * @param errorReason Razão do erro (saída)
     * @return true se puder coletar
     */
    bool CanCollectTerritoryResources(WORD territoryID, GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Obtém um território
     * @param territoryID ID do território
     * @return Ponteiro para o território, ou nullptr se não existir
     */
    const GuildTerritory* GetTerritory(WORD territoryID) const;
    
    /**
     * @brief Obtém um território (para edição)
     * @param territoryID ID do território
     * @return Ponteiro para o território, ou nullptr se não existir
     */
    GuildTerritory* GetTerritoryForEdit(WORD territoryID);
    
    /**
     * @brief Obtém todos os territórios
     * @return Lista de territórios
     */
    std::vector<const GuildTerritory*> GetAllTerritories() const;
    
    /**
     * @brief Obtém territórios por tipo
     * @param type Tipo
     * @return Lista de territórios
     */
    std::vector<const GuildTerritory*> GetTerritoriesByType(TerritoryType type) const;
    
    /**
     * @brief Obtém territórios por recurso
     * @param resourceType Tipo de recurso
     * @return Lista de territórios
     */
    std::vector<const GuildTerritory*> GetTerritoriesByResource(TerritoryResourceType resourceType) const;
    
    /**
     * @brief Obtém territórios por guild
     * @param guildID ID da guild
     * @return Lista de territórios
     */
    std::vector<const GuildTerritory*> GetTerritoriesByGuild(GuildID guildID) const;
    
    /**
     * @brief Obtém o número de territórios por guild
     * @param guildID ID da guild
     * @return Número de territórios
     */
    size_t CountTerritoriesByGuild(GuildID guildID) const;
    
    /**
     * @brief Obtém territórios em uma área
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param radius Raio
     * @return Lista de territórios
     */
    std::vector<const GuildTerritory*> GetTerritoriesInArea(DWORD mapID, WORD x, WORD y, WORD radius) const;
    
    /**
     * @brief Verifica se um ponto está em um território
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return ID do território, ou 0 se não estiver
     */
    WORD IsPointInTerritory(DWORD mapID, WORD x, WORD y) const;
    
    /**
     * @brief Verifica se um ponto está em um território de uma guild
     * @param mapID ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param guildID ID da guild
     * @return true se estiver
     */
    bool IsPointInGuildTerritory(DWORD mapID, WORD x, WORD y, GuildID guildID) const;
    
    /**
     * @brief Registra um callback para atualizações de território
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterTerritoryUpdateCallback(std::function<void(const TerritoryUpdate&)> callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterTerritoryUpdateCallback(int callbackID);
    
    /**
     * @brief Obtém o próximo ID de território disponível
     * @return ID de território
     */
    WORD GetNextTerritoryID();
    
    /**
     * @brief Atualiza o status de um território
     * @param territoryID ID do território
     * @param newStatus Novo status
     * @return true se atualizado com sucesso
     */
    bool UpdateTerritoryStatus(WORD territoryID, TerritoryStatus newStatus);
    
    /**
     * @brief Obtém o custo de melhoria de um território
     * @param territoryID ID do território
     * @param goldCost Custo em ouro (saída)
     * @param resourceCost Custo em recursos (saída)
     * @return true se obtido com sucesso
     */
    bool GetTerritoryUpgradeCost(WORD territoryID, DWORD& goldCost, DWORD& resourceCost);
    
    /**
     * @brief Obtém o custo de melhoria da defesa de um território
     * @param territoryID ID do território
     * @param goldCost Custo em ouro (saída)
     * @param resourceCost Custo em recursos (saída)
     * @return true se obtido com sucesso
     */
    bool GetTerritoryDefenseUpgradeCost(WORD territoryID, DWORD& goldCost, DWORD& resourceCost);
    
    /**
     * @brief Obtém o custo de construção de um edifício
     * @param territoryID ID do território
     * @param buildingType Tipo de edifício
     * @param goldCost Custo em ouro (saída)
     * @param resourceCost Custo em recursos (saída)
     * @return true se obtido com sucesso
     */
    bool GetBuildingConstructionCost(WORD territoryID, TerritoryBuildingType buildingType,
                                   DWORD& goldCost, DWORD& resourceCost);
    
    /**
     * @brief Verifica se um edifício está construído
     * @param territoryID ID do território
     * @param buildingType Tipo de edifício
     * @return true se estiver construído
     */
    bool IsBuildingConstructed(WORD territoryID, TerritoryBuildingType buildingType);
    
    /**
     * @brief Conta edifícios de um território
     * @param territoryID ID do território
     * @return Número de edifícios
     */
    size_t CountTerritoryBuildings(WORD territoryID) const;
    
    /**
     * @brief Obtém o número máximo de edifícios permitidos
     * @param territoryID ID do território
     * @return Número máximo de edifícios
     */
    size_t GetMaxBuildingsAllowed(WORD territoryID) const;
    
    /**
     * @brief Produz recursos em um território
     * @param territoryID ID do território
     * @param elapsedMs Tempo decorrido em ms
     * @return Quantidade produzida
     */
    DWORD ProduceResources(WORD territoryID, DWORD elapsedMs);
    
    /**
     * @brief Transmite uma mensagem sobre um território
     * @param territoryID ID do território
     * @param message Mensagem
     */
    void BroadcastTerritoryMessage(WORD territoryID, const std::string& message);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    GuildTerritoryManager();
    
    /**
     * @brief Destrutor
     */
    ~GuildTerritoryManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    GuildTerritoryManager(const GuildTerritoryManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    GuildTerritoryManager& operator=(const GuildTerritoryManager&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Processa produção de recursos
     * @param elapsed Tempo decorrido
     */
    void ProcessResourceProduction(DWORD elapsed);
    
    /**
     * @brief Notifica callbacks de atualização
     * @param update Atualização
     */
    void NotifyTerritoryUpdateCallbacks(const TerritoryUpdate& update);
    
    /**
     * @brief Calcula distância entre pontos
     * @param x1 Coordenada X do ponto 1
     * @param y1 Coordenada Y do ponto 1
     * @param x2 Coordenada X do ponto 2
     * @param y2 Coordenada Y do ponto 2
     * @return Distância
     */
    float CalculateDistance(WORD x1, WORD y1, WORD x2, WORD y2) const;
    
    /**
     * @brief Marca um território como atualizado
     * @param territoryID ID do território
     */
    void MarkTerritoryAsUpdated(WORD territoryID);
    
    /**
     * @brief Calcula multiplicador de produção de recursos
     * @param territoryID ID do território
     * @return Multiplicador
     */
    float CalculateResourceProductionMultiplier(WORD territoryID) const;
    
    /**
     * @brief Calcula multiplicador de defesa
     * @param territoryID ID do território
     * @return Multiplicador
     */
    float CalculateDefenseMultiplier(WORD territoryID) const;
    
    /**
     * @brief Verifica se uma guild pode ter mais territórios
     * @param guildID ID da guild
     * @param errorReason Razão do erro (saída)
     * @return true se puder ter mais
     */
    bool CanGuildHaveMoreTerritories(GuildID guildID, std::string& errorReason);
    
    /**
     * @brief Calcula o número máximo de territórios para uma guild
     * @param guildID ID da guild
     * @return Número máximo de territórios
     */
    size_t CalculateMaxTerritoriesForGuild(GuildID guildID) const;
    
    // Gerenciador de guild
    GuildManager* guildManager_;
    
    // Territórios
    std::map<WORD, GuildTerritory> territories_;
    mutable std::mutex territoriesMutex_;
    
    // Índices para busca rápida
    std::map<GuildID, std::vector<WORD>> territoriesByGuild_;
    std::map<TerritoryType, std::vector<WORD>> territoriesByType_;
    std::map<TerritoryResourceType, std::vector<WORD>> territoriesByResourceType_;
    
    // Cache espacial: mapeia mapID -> {coordHash -> territoryIDs}
    std::map<DWORD, std::map<DWORD, std::vector<WORD>>> spatialIndex_;
    
    // Callbacks para atualizações
    std::map<int, std::function<void(const TerritoryUpdate&)>> updateCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contador para novos IDs
    WORD nextTerritoryID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Territórios atualizados recentemente (necessitam persistência)
    std::unordered_set<WORD> updatedTerritories_;
    std::mutex updatedTerritoriesMutex_;
    
    // Estado
    bool initialized_;
};

} // namespace territory
} // namespace guild
} // namespace server
} // namespace wyd

// Acesso global
#define g_GuildTerritoryManager wyd::server::guild::territory::GuildTerritoryManager::GetInstance()

#endif // GUILDTERRITORYMANAGER_H

} // namespace wydbr
