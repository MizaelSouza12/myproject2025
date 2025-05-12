/**
 * MobGroup.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/spawner/MobGroup.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef MOBGROUP_H
#define MOBGROUP_H

#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <random>
#include <string>

#include "../../../../core/WYDTypes.h"
#include "../WorldTypes.h"

namespace wydbr {


/**
 * @file MobGroup.h
 * @brief Classe de gerenciamento de grupos de mobs
 * 
 * Este arquivo contém a definição da classe que gerencia grupos de mobs,
 * permitindo o controle coordenado de spawns em áreas específicas.
 * No WYD original, não havia um conceito robusto de grupos, e a lógica
 * de spawn era rudimentar. Esta implementação oferece um sistema mais avançado.
 */

namespace wyd {
namespace server {
namespace world {
namespace entity {
    class CMob; // Forward declaration
}

namespace spawner {

/**
 * @brief Classe de gerenciamento de grupos de mobs
 * 
 * Esta classe é responsável por gerenciar um grupo de mobs, incluindo
 * a coordenação de spawns, respawns, e comportamento coletivo.
 */
/**
 * Classe WYDMobGroup
 * 
 * Esta classe implementa a funcionalidade WYDMobGroup conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MobGroup {
public:
    /**
     * @brief Construtor
     * @param config Configuração do grupo
     */
    MobGroup(const SpawnGroupConfig& config);
    
    /**
     * @brief Destrutor
     */
    ~MobGroup();
    
    /**
     * @brief Inicializa o grupo
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Atualiza o grupo
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Ativa o grupo
     * @return true se ativado com sucesso
     */
    bool Activate();
    
    /**
     * @brief Desativa o grupo
     * @return true se desativado com sucesso
     */
    bool Deactivate();
    
    /**
     * @brief Verifica se o grupo está ativo
     * @return true se ativo
     */
    bool IsActive() const;
    
    /**
     * @brief Obtém o status do grupo
     * @return Status do grupo
     */
    SpawnGroupStatus GetStatus() const;
    
    /**
     * @brief Adiciona uma entidade ao grupo
     * @param entity Entidade
     * @return true se adicionado com sucesso
     */
    bool AddEntity(std::shared_ptr<entity::CMob> entity);
    
    /**
     * @brief Remove uma entidade do grupo
     * @param entityID ID da entidade
     * @return true se removido com sucesso
     */
    bool RemoveEntity(DWORD entityID);
    
    /**
     * @brief Obtém uma entidade do grupo
     * @param entityID ID da entidade
     * @return Entidade
     */
    std::shared_ptr<entity::CMob> GetEntity(DWORD entityID) const;
    
    /**
     * @brief Obtém todas as entidades do grupo
     * @return Lista de entidades
     */
    std::vector<std::shared_ptr<entity::CMob>> GetAllEntities() const;
    
    /**
     * @brief Obtém o número de entidades no grupo
     * @return Número de entidades
     */
    WORD GetEntityCount() const;
    
    /**
     * @brief Obtém a configuração do grupo
     * @return Configuração do grupo
     */
    const SpawnGroupConfig& GetConfig() const;
    
    /**
     * @brief Define a configuração do grupo
     * @param config Configuração do grupo
     */
    void SetConfig(const SpawnGroupConfig& config);
    
    /**
     * @brief Obtém o ID do grupo
     * @return ID do grupo
     */
    WORD GetID() const;
    
    /**
     * @brief Escolhe um mob para spawnar
     * @return ID do mob
     */
    WORD SelectMobToSpawn() const;
    
    /**
     * @brief Escolhe um ponto de spawn
     * @param spawnPoints Lista de pontos de spawn
     * @return Ponto de spawn escolhido
     */
    SpawnPoint SelectSpawnPoint(const std::vector<SpawnPoint>& spawnPoints) const;
    
    /**
     * @brief Tenta spawnar um mob
     * @param mobID ID do mob
     * @param point Ponto de spawn
     * @return Entidade spawned, ou nullptr em caso de erro
     */
    std::shared_ptr<entity::CMob> SpawnMob(WORD mobID, const SpawnPoint& point);
    
    /**
     * @brief Despawna todos os mobs do grupo
     * @return Número de mobs despawnados
     */
    int DespawnAllMobs();
    
    /**
     * @brief Notifica a morte de uma entidade
     * @param entity Entidade
     */
    void NotifyEntityDeath(entity::CMob* entity);
    
    /**
     * @brief Notifica o respawn de uma entidade
     * @param entity Entidade
     */
    void NotifyEntityRespawn(entity::CMob* entity);
    
    /**
     * @brief Define o status do grupo
     * @param status Status
     */
    void SetStatus(SpawnerStatus status);
    
    /**
     * @brief Verifica se é um grupo noturno
     * @return true se for um grupo noturno
     */
    bool IsNightSpawn() const;
    
    /**
     * @brief Verifica se é um grupo diurno
     * @return true se for um grupo diurno
     */
    bool IsDaySpawn() const;
    
    /**
     * @brief Define o multiplicador de densidade
     * @param multiplier Multiplicador
     */
    void SetDensityMultiplier(float multiplier);
    
    /**
     * @brief Obtém o multiplicador de densidade
     * @return Multiplicador
     */
    float GetDensityMultiplier() const;
    
    /**
     * @brief Calcula a contagem máxima efetiva de mobs
     * @return Contagem máxima efetiva
     */
    WORD CalculateEffectiveMaxCount() const;
    
    /**
     * @brief Calcula o tempo de respawn
     * @return Tempo de respawn em ms
     */
    DWORD CalculateRespawnTime() const;
    
private:
    // Configuração do grupo
    SpawnGroupConfig config_;
    
    // Entidades no grupo
    std::unordered_map<DWORD, std::shared_ptr<entity::CMob>> entities_;
    std::mutex entitiesMutex_;
    
    // Status do grupo
    SpawnGroupStatus status_;
    std::mutex statusMutex_;
    
    // Estado
    bool active_;
    float densityMultiplier_;
    DWORD lastSpawnTime_;
    DWORD lastUpdateTime_;
    
    // Gerador de números aleatórios
    std::mt19937 rng_;
};

} // namespace spawner
} // namespace world
} // namespace server
} // namespace wyd

#endif // MOBGROUP_H

} // namespace wydbr
