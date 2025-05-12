#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include "../../common/types/ActorId.h"
#include "MapInfo.h"
#include "WorldRegion.h"
#include "InstanceType.h"

namespace wydbr::tmsrv::world {

/**
 * @class WorldInstance
 * @brief Representa uma instância de mapa no mundo do jogo.
 * 
 * Uma instância de mapa é uma cópia independente de um mapa onde os jogadores
 * podem interagir. Diferentes instâncias do mesmo mapa não interagem entre si.
 */
class WorldInstance {
public:
    /**
     * @brief Construtor
     * @param instanceId Identificador único da instância
     * @param mapId Identificador do mapa base
     * @param instanceType Tipo da instância (normal, dungeon, evento, etc)
     * @param mapInfo Informações do mapa base
     */
    WorldInstance(uint32_t instanceId, uint32_t mapId, uint8_t instanceType, const MapInfo& mapInfo);
    
    /**
     * @brief Destrutor
     */
    ~WorldInstance();
    
    /**
     * @brief Inicializa a instância
     * @return true se a inicialização for bem-sucedida, false caso contrário
     */
    bool initialize();
    
    /**
     * @brief Atualiza o estado da instância
     * @param deltaTime Tempo decorrido desde a última atualização em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Finaliza a instância e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Verifica se a instância deve ser removida
     * @return true se a instância deve ser removida, false caso contrário
     */
    bool shouldBeRemoved() const;
    
    /**
     * @brief Marca a instância para remoção
     */
    void markForRemoval();
    
    /**
     * @brief Adiciona um ator à instância
     * @param actorId Identificador único do ator
     * @param x Coordenada X inicial
     * @param y Coordenada Y inicial
     * @return true se o ator foi adicionado com sucesso, false caso contrário
     */
    bool addActor(const common::types::ActorId& actorId, float x, float y);
    
    /**
     * @brief Remove um ator da instância
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
     * @brief Obtém a região do mundo em que uma coordenada específica está localizada
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return Ponteiro para a região ou nullptr se não encontrada
     */
    WorldRegion* getRegionAt(float x, float y);
    
    /**
     * @brief Obtém o ID da instância
     * @return ID da instância
     */
    uint32_t getInstanceId() const;
    
    /**
     * @brief Obtém o ID do mapa base
     * @return ID do mapa base
     */
    uint32_t getMapId() const;
    
    /**
     * @brief Obtém o tipo da instância
     * @return Tipo da instância
     */
    uint8_t getInstanceType() const;
    
    /**
     * @brief Obtém as informações do mapa
     * @return Referência constante para as informações do mapa
     */
    const MapInfo& getMapInfo() const;
    
    /**
     * @brief Verifica se um ator está presente na instância
     * @param actorId Identificador único do ator
     * @return true se o ator está presente, false caso contrário
     */
    bool hasActor(const common::types::ActorId& actorId) const;
    
    /**
     * @brief Obtém a posição de um ator
     * @param actorId Identificador único do ator
     * @param outX Coordenada X de saída
     * @param outY Coordenada Y de saída
     * @return true se a posição foi obtida com sucesso, false caso contrário
     */
    bool getActorPosition(const common::types::ActorId& actorId, float& outX, float& outY) const;
    
    /**
     * @brief Obtém o número de atores na instância
     * @return Número de atores
     */
    size_t getActorCount() const;
    
    /**
     * @brief Verifica se a instância está vazia (sem atores)
     * @return true se a instância está vazia, false caso contrário
     */
    bool isEmpty() const;
    
    /**
     * @brief Define o tempo de vida da instância
     * @param lifetimeMs Tempo de vida em milissegundos (0 para infinito)
     */
    void setLifetime(uint32_t lifetimeMs);
    
    /**
     * @brief Obtém o tempo de vida restante da instância
     * @return Tempo de vida restante em milissegundos (0 para infinito)
     */
    uint32_t getRemainingLifetime() const;

private:
    // Estrutura para armazenar informações de posição de um ator
    struct ActorPosition {
        float x;
        float y;
        uint32_t regionId;
    };
    
    // ID da instância
    uint32_t m_instanceId;
    
    // ID do mapa base
    uint32_t m_mapId;
    
    // Tipo da instância
    uint8_t m_instanceType;
    
    // Informações do mapa
    MapInfo m_mapInfo;
    
    // Flag de inicialização
    bool m_initialized;
    
    // Flag de remoção
    bool m_markedForRemoval;
    
    // Regiões do mundo
    std::vector<std::unique_ptr<WorldRegion>> m_regions;
    
    // Mapa de posições dos atores (actorId -> ActorPosition)
    std::unordered_map<common::types::ActorId, ActorPosition> m_actorPositions;
    
    // Tempo de vida da instância (0 para infinito)
    uint32_t m_lifetimeMs;
    
    // Tempo de vida restante
    uint32_t m_remainingLifetimeMs;
    
    // Tempo desde a última atualização
    uint32_t m_timeSinceLastUpdate;
    
    // Inicializa as regiões do mundo
    bool initializeRegions();
    
    // Atualiza a região de um ator
    bool updateActorRegion(const common::types::ActorId& actorId, float x, float y);
};

} // namespace wydbr::tmsrv::world 