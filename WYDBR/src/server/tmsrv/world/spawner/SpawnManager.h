/**
 * SpawnManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/spawner/SpawnManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef SPAWNMANAGER_H
#define SPAWNMANAGER_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include <mutex>
#include <memory>
#include <thread>
#include <atomic>
#include <random>
#include <functional>
#include <chrono>

#include "../../../../core/WYDTypes.h"
#include "../WorldTypes.h"
#include "../entity/CMob.h"
#include "MobGroup.h"

namespace wydbr {


/**
 * @file SpawnManager.h
 * @brief Gerenciador de spawn de mobs
 * 
 * Este arquivo contém a definição do gerenciador de spawn de mobs,
 * que é responsável por gerenciar o aparecimento de mobs no mundo.
 * No WYD original, isso era feito de forma muito rudimentar, com vários
 * bugs e limitações. Esta implementação oferece um sistema mais robusto.
 */

namespace wyd {
namespace server {
namespace world {
namespace spawner {

// Forward declarations
class CMobTemplateManager;
class CWorldManager;

/**
 * @brief Gerenciador de spawn de mobs
 * 
 * Esta classe gerencia o spawn de mobs no mundo, incluindo grupos de spawn,
 * respawn de mobs, e balanceamento dinâmico com base na população de jogadores.
 */
/**
 * Classe WYDSpawnManager
 * 
 * Esta classe implementa a funcionalidade WYDSpawnManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CSpawnManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static CSpawnManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param templateManager Gerenciador de templates de mob
     * @param worldManager Gerenciador de mundo
     * @return true se inicializado com sucesso
     */
    bool Initialize(CMobTemplateManager* templateManager, CWorldManager* worldManager);
    
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
     * @brief Carrega configurações de spawn
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadSpawnConfigs(const std::string& filePath);
    
    /**
     * @brief Salva configurações de spawn
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveSpawnConfigs(const std::string& filePath);
    
    /**
     * @brief Carrega pontos de spawn
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadSpawnPoints(const std::string& filePath);
    
    /**
     * @brief Salva pontos de spawn
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveSpawnPoints(const std::string& filePath);
    
    /**
     * @brief Adiciona um grupo de spawn
     * @param config Configuração do grupo
     * @return true se adicionado com sucesso
     */
    bool AddSpawnGroup(const SpawnGroupConfig& config);
    
    /**
     * @brief Remove um grupo de spawn
     * @param groupID ID do grupo
     * @return true se removido com sucesso
     */
    bool RemoveSpawnGroup(WORD groupID);
    
    /**
     * @brief Obtém um grupo de spawn
     * @param groupID ID do grupo
     * @return Ponteiro para o grupo, ou nullptr se não existir
     */
    std::shared_ptr<MobGroup> GetSpawnGroup(WORD groupID);
    
    /**
     * @brief Obtém todos os grupos de spawn
     * @return Lista de grupos
     */
    std::vector<std::shared_ptr<MobGroup>> GetAllSpawnGroups();
    
    /**
     * @brief Spawna um mob
     * @param mobID ID do mob
     * @param x Posição X
     * @param y Posição Y
     * @param mapID ID do mapa
     * @param level Nível do mapa
     * @param groupID ID do grupo (0 para independente)
     * @return Ponteiro para o mob spawned, ou nullptr em caso de erro
     */
    std::shared_ptr<entity::CMob> SpawnMob(WORD mobID, WORD x, WORD y, WORD mapID, BYTE level, WORD groupID = 0);
    
    /**
     * @brief Despawna um mob
     * @param mobID ID do mob
     * @return true se despawnado com sucesso
     */
    bool DespawnMob(DWORD mobID);
    
    /**
     * @brief Respawna um mob
     * @param mobID ID do mob
     * @return true se respawnado com sucesso
     */
    bool RespawnMob(DWORD mobID);
    
    /**
     * @brief Adiciona um ponto de spawn
     * @param point Ponto de spawn
     * @return true se adicionado com sucesso
     */
    bool AddSpawnPoint(const SpawnPoint& point);
    
    /**
     * @brief Remove um ponto de spawn
     * @param pointID ID do ponto
     * @return true se removido com sucesso
     */
    bool RemoveSpawnPoint(DWORD pointID);
    
    /**
     * @brief Obtém um ponto de spawn
     * @param pointID ID do ponto
     * @return Ponto de spawn
     */
    SpawnPoint GetSpawnPoint(DWORD pointID) const;
    
    /**
     * @brief Obtém pontos de spawn em uma área
     * @param mapID ID do mapa
     * @param x Posição X central
     * @param y Posição Y central
     * @param radius Raio
     * @return Lista de pontos de spawn
     */
    std::vector<SpawnPoint> GetSpawnPointsInArea(WORD mapID, WORD x, WORD y, WORD radius) const;
    
    /**
     * @brief Obtém pontos de spawn para um grupo
     * @param groupID ID do grupo
     * @return Lista de pontos de spawn
     */
    std::vector<SpawnPoint> GetSpawnPointsForGroup(WORD groupID) const;
    
    /**
     * @brief Obtém pontos de spawn para um mapa
     * @param mapID ID do mapa
     * @return Lista de pontos de spawn
     */
    std::vector<SpawnPoint> GetSpawnPointsForMap(WORD mapID) const;
    
    /**
     * @brief Ativa um grupo de spawn
     * @param groupID ID do grupo
     * @return true se ativado com sucesso
     */
    bool ActivateSpawnGroup(WORD groupID);
    
    /**
     * @brief Desativa um grupo de spawn
     * @param groupID ID do grupo
     * @return true se desativado com sucesso
     */
    bool DeactivateSpawnGroup(WORD groupID);
    
    /**
     * @brief Ativa todos os grupos de spawn
     * @return Número de grupos ativados
     */
    int ActivateAllSpawnGroups();
    
    /**
     * @brief Desativa todos os grupos de spawn
     * @return Número de grupos desativados
     */
    int DeactivateAllSpawnGroups();
    
    /**
     * @brief Verifica se um grupo de spawn está ativo
     * @param groupID ID do grupo
     * @return true se ativo
     */
    bool IsSpawnGroupActive(WORD groupID) const;
    
    /**
     * @brief Obtém o status de um grupo de spawn
     * @param groupID ID do grupo
     * @return Status do grupo
     */
    SpawnGroupStatus GetSpawnGroupStatus(WORD groupID) const;
    
    /**
     * @brief Registra um callback para eventos de spawn
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterSpawnEventCallback(entity::EntityEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterSpawnEventCallback(int callbackID);
    
    /**
     * @brief Define a densidade de spawn
     * @param density Densidade (0.0 - 1.0)
     */
    void SetSpawnDensity(float density);
    
    /**
     * @brief Obtém a densidade de spawn
     * @return Densidade (0.0 - 1.0)
     */
    float GetSpawnDensity() const;
    
    /**
     * @brief Define o modo de balanceamento dinâmico
     * @param enabled Se está ativado
     */
    void SetDynamicBalancing(bool enabled);
    
    /**
     * @brief Verifica se o balanceamento dinâmico está ativado
     * @return true se ativado
     */
    bool IsDynamicBalancingEnabled() const;
    
    /**
     * @brief Atualiza a densidade com base na população de jogadores
     * @param playerCount Número de jogadores
     * @param capacity Capacidade máxima
     */
    void UpdateDensityBasedOnPopulation(int playerCount, int capacity);
    
    /**
     * @brief Obtém o próximo ID de ponto de spawn disponível
     * @return ID de ponto de spawn
     */
    DWORD GetNextSpawnPointID();
    
    /**
     * @brief Obtém o próximo ID de grupo de spawn disponível
     * @return ID de grupo de spawn
     */
    WORD GetNextSpawnGroupID();
    
    /**
     * @brief Calcula o tempo de respawn
     * @param minTime Tempo mínimo
     * @param maxTime Tempo máximo
     * @return Tempo de respawn em ms
     */
    DWORD CalculateRespawnTime(DWORD minTime, DWORD maxTime);
    
    /**
     * @brief Define um multiplicador de experiência global
     * @param multiplier Multiplicador
     */
    void SetGlobalExpMultiplier(float multiplier);
    
    /**
     * @brief Obtém o multiplicador de experiência global
     * @return Multiplicador
     */
    float GetGlobalExpMultiplier() const;
    
    /**
     * @brief Define um multiplicador de drop global
     * @param multiplier Multiplicador
     */
    void SetGlobalDropMultiplier(float multiplier);
    
    /**
     * @brief Obtém o multiplicador de drop global
     * @return Multiplicador
     */
    float GetGlobalDropMultiplier() const;
    
    /**
     * @brief Ativa mobs noturno
     * @param active Se está ativo
     */
    void SetNightSpawnsActive(bool active);
    
    /**
     * @brief Verifica se mobs nolturno estão ativos
     * @return true se ativos
     */
    bool AreNightSpawnsActive() const;
    
    /**
     * @brief Ativa mobs diurnos
     * @param active Se está ativo
     */
    void SetDaySpawnsActive(bool active);
    
    /**
     * @brief Verifica se mobs diurnos estão ativos
     * @return true se ativos
     */
    bool AreDaySpawnsActive() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CSpawnManager();
    
    /**
     * @brief Destrutor
     */
    ~CSpawnManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CSpawnManager(const CSpawnManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CSpawnManager& operator=(const CSpawnManager&) = delete;
    
    /**
     * @brief Inicializa grupos de spawn
     */
    void InitializeSpawnGroups();
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Atualiza grupos de spawn
     * @param elapsed Tempo decorrido
     */
    void UpdateSpawnGroups(DWORD elapsed);
    
    /**
     * @brief Atualiza respawns
     * @param elapsed Tempo decorrido
     */
    void UpdateRespawns(DWORD elapsed);
    
    /**
     * @brief Atualiza balanceamento dinâmico
     * @param elapsed Tempo decorrido
     */
    void UpdateDynamicBalancing(DWORD elapsed);
    
    /**
     * @brief Atualiza eventos de spawn
     * @param elapsed Tempo decorrido
     */
    void UpdateSpawnEvents(DWORD elapsed);
    
    /**
     * @brief Notifica um evento de spawn
     * @param event Evento
     * @param entityID ID da entidade
     * @param param Parâmetro adicional
     */
    void NotifySpawnEvent(entity::EntityEvent event, DWORD entityID, const void* param = nullptr);
    
    // Gerenciadores
    CMobTemplateManager* templateManager_;
    CWorldManager* worldManager_;
    
    // Grupos de spawn
    std::map<WORD, std::shared_ptr<MobGroup>> spawnGroups_;
    std::mutex groupsMutex_;
    
    // Pontos de spawn
    std::map<DWORD, SpawnPoint> spawnPoints_;
    std::mutex pointsMutex_;
    
    // Mobs spawned
    std::map<DWORD, std::shared_ptr<entity::CMob>> spawnedMobs_;
    std::mutex mobsMutex_;
    
    // Callbacks de evento
    std::map<int, entity::EntityEventCallback> eventCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Configurações
    float spawnDensity_;
    bool dynamicBalancing_;
    float globalExpMultiplier_;
    float globalDropMultiplier_;
    bool nightSpawnsActive_;
    bool daySpawnsActive_;
    
    // Contadores
    DWORD nextSpawnPointID_;
    WORD nextSpawnGroupID_;
    
    // Estado
    bool initialized_;
    std::atomic<bool> running_;
    
    // Thread de update
    std::thread updateThread_;
    
    // Gerador de números aleatórios
    std::mt19937 rng_;
    
    // Tempos de última atualização
    DWORD lastGroupUpdateTime_;
    DWORD lastRespawnUpdateTime_;
    DWORD lastBalancingUpdateTime_;
    DWORD lastEventUpdateTime_;
};

// Acesso global
#define g_SpawnManager CSpawnManager::GetInstance()

} // namespace spawner
} // namespace world
} // namespace server
} // namespace wyd

#endif // SPAWNMANAGER_H

} // namespace wydbr
