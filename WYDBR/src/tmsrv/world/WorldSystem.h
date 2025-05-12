#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include "../../common/types/ActorId.h"
#include "../../common/utils/TimerManager.h"
#include "WorldInstance.h"
#include "MapInfo.h"
#include "WorldRegion.h"

namespace wydbr::tmsrv::world {

/**
 * @class WorldSystem
 * @brief Sistema responsável por gerenciar o mundo do jogo e suas instâncias.
 * 
 * O WorldSystem gerencia todas as instâncias de mapas, regiões e coordena
 * a interação entre jogadores e o ambiente do jogo.
 */
class WorldSystem {
public:
    /**
     * @brief Obtém a instância singleton do WorldSystem
     * @return Referência para a instância única
     */
    static WorldSystem& getInstance();

    /**
     * @brief Inicializa o sistema de mundo
     * @param configPath Caminho para o arquivo de configuração do mundo
     * @return true se a inicialização for bem-sucedida, false caso contrário
     */
    bool initialize(const std::string& configPath);

    /**
     * @brief Atualiza o estado do mundo e todas as suas instâncias
     * @param deltaTime Tempo decorrido desde a última atualização em milissegundos
     */
    void update(uint32_t deltaTime);

    /**
     * @brief Finaliza o sistema de mundo e libera recursos
     */
    void shutdown();

    /**
     * @brief Cria uma nova instância de mapa
     * @param mapId Identificador do mapa
     * @param instanceType Tipo de instância (normal, dungeon, evento, etc)
     * @return Ponteiro para a instância criada ou nullptr em caso de falha
     */
    std::shared_ptr<WorldInstance> createInstance(uint32_t mapId, uint8_t instanceType);

    /**
     * @brief Remove uma instância de mapa
     * @param instanceId Identificador da instância
     * @return true se a instância foi removida com sucesso, false caso contrário
     */
    bool removeInstance(uint32_t instanceId);

    /**
     * @brief Obtém uma instância de mapa pelo seu ID
     * @param instanceId Identificador da instância
     * @return Ponteiro para a instância ou nullptr se não encontrada
     */
    std::shared_ptr<WorldInstance> getInstance(uint32_t instanceId);

    /**
     * @brief Adiciona um ator (jogador, NPC, monstro) ao mundo
     * @param actorId Identificador único do ator
     * @param instanceId Identificador da instância onde o ator será adicionado
     * @param x Coordenada X inicial
     * @param y Coordenada Y inicial
     * @return true se o ator foi adicionado com sucesso, false caso contrário
     */
    bool addActor(const common::types::ActorId& actorId, uint32_t instanceId, float x, float y);

    /**
     * @brief Remove um ator do mundo
     * @param actorId Identificador único do ator
     * @return true se o ator foi removido com sucesso, false caso contrário
     */
    bool removeActor(const common::types::ActorId& actorId);

    /**
     * @brief Move um ator para uma nova posição
     * @param actorId Identificador único do ator
     * @param x Nova coordenada X
     * @param y Nova coordenada Y
     * @return true se o movimento foi realizado com sucesso, false caso contrário
     */
    bool moveActor(const common::types::ActorId& actorId, float x, float y);

    /**
     * @brief Teleporta um ator para outra instância ou posição
     * @param actorId Identificador único do ator
     * @param instanceId Identificador da instância de destino
     * @param x Coordenada X de destino
     * @param y Coordenada Y de destino
     * @return true se o teleporte foi realizado com sucesso, false caso contrário
     */
    bool teleportActor(const common::types::ActorId& actorId, uint32_t instanceId, float x, float y);

    /**
     * @brief Carrega as definições de mapas do arquivo
     * @param mapFilePath Caminho para o arquivo de definições de mapas
     * @return true se o carregamento foi bem-sucedido, false caso contrário
     */
    bool loadMapDefinitions(const std::string& mapFilePath);

    /**
     * @brief Obtém informações sobre um mapa específico
     * @param mapId Identificador do mapa
     * @return Ponteiro para as informações do mapa ou nullptr se não encontrado
     */
    const MapInfo* getMapInfo(uint32_t mapId) const;

    /**
     * @brief Obtém a região do mundo em que uma coordenada específica está localizada
     * @param instanceId Identificador da instância
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return Ponteiro para a região ou nullptr se não encontrada
     */
    WorldRegion* getRegionAt(uint32_t instanceId, float x, float y);

private:
    WorldSystem() = default;
    ~WorldSystem() = default;
    
    // Impede cópia e atribuição
    WorldSystem(const WorldSystem&) = delete;
    WorldSystem& operator=(const WorldSystem&) = delete;

    // Mapa de instâncias ativas (instanceId -> WorldInstance)
    std::unordered_map<uint32_t, std::shared_ptr<WorldInstance>> m_instances;
    
    // Mapa de definições de mapas (mapId -> MapInfo)
    std::unordered_map<uint32_t, MapInfo> m_mapDefinitions;
    
    // Mapeamento de atores para suas instâncias (actorId -> instanceId)
    std::unordered_map<common::types::ActorId, uint32_t> m_actorInstances;
    
    // Contador para geração de IDs de instâncias
    uint32_t m_nextInstanceId = 1;
    
    // Timer manager para eventos temporizados no mundo
    std::shared_ptr<common::utils::TimerManager> m_timerManager;
    
    // Flag de inicialização
    bool m_initialized = false;
};

} // namespace wydbr::tmsrv::world 