#pragma once

#include <unordered_set>
#include <vector>
#include "../../common/types/ActorId.h"

namespace wydbr::tmsrv::world {

/**
 * @class WorldRegion
 * @brief Representa uma região dentro de uma instância de mapa.
 * 
 * Uma região é uma subdivisão de um mapa que contém um conjunto de atores.
 * As regiões são usadas para otimizar operações espaciais, como detecção
 * de colisão e visibilidade.
 */
class WorldRegion {
public:
    /**
     * @brief Construtor
     * @param regionId Identificador único da região
     * @param startX Coordenada X inicial da região
     * @param startY Coordenada Y inicial da região
     * @param endX Coordenada X final da região
     * @param endY Coordenada Y final da região
     */
    WorldRegion(uint32_t regionId, float startX, float startY, float endX, float endY);
    
    /**
     * @brief Destrutor
     */
    ~WorldRegion();
    
    /**
     * @brief Inicializa a região
     * @return true se a inicialização for bem-sucedida, false caso contrário
     */
    bool initialize();
    
    /**
     * @brief Atualiza o estado da região
     * @param deltaTime Tempo decorrido desde a última atualização em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Finaliza a região e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Adiciona um ator à região
     * @param actorId Identificador único do ator
     * @return true se o ator foi adicionado com sucesso, false caso contrário
     */
    bool addActor(const common::types::ActorId& actorId);
    
    /**
     * @brief Remove um ator da região
     * @param actorId Identificador único do ator
     * @return true se o ator foi removido com sucesso, false caso contrário
     */
    bool removeActor(const common::types::ActorId& actorId);
    
    /**
     * @brief Verifica se um ator está presente na região
     * @param actorId Identificador único do ator
     * @return true se o ator está presente, false caso contrário
     */
    bool hasActor(const common::types::ActorId& actorId) const;
    
    /**
     * @brief Obtém o ID da região
     * @return ID da região
     */
    uint32_t getRegionId() const;
    
    /**
     * @brief Obtém a coordenada X inicial da região
     * @return Coordenada X inicial
     */
    float getStartX() const;
    
    /**
     * @brief Obtém a coordenada Y inicial da região
     * @return Coordenada Y inicial
     */
    float getStartY() const;
    
    /**
     * @brief Obtém a coordenada X final da região
     * @return Coordenada X final
     */
    float getEndX() const;
    
    /**
     * @brief Obtém a coordenada Y final da região
     * @return Coordenada Y final
     */
    float getEndY() const;
    
    /**
     * @brief Obtém a largura da região
     * @return Largura da região
     */
    float getWidth() const;
    
    /**
     * @brief Obtém a altura da região
     * @return Altura da região
     */
    float getHeight() const;
    
    /**
     * @brief Verifica se um ponto está dentro da região
     * @param x Coordenada X do ponto
     * @param y Coordenada Y do ponto
     * @return true se o ponto está dentro da região, false caso contrário
     */
    bool containsPoint(float x, float y) const;
    
    /**
     * @brief Obtém todos os atores na região
     * @return Conjunto de IDs de atores
     */
    const std::unordered_set<common::types::ActorId>& getActors() const;
    
    /**
     * @brief Obtém o número de atores na região
     * @return Número de atores
     */
    size_t getActorCount() const;
    
    /**
     * @brief Verifica se a região está vazia (sem atores)
     * @return true se a região está vazia, false caso contrário
     */
    bool isEmpty() const;
    
    /**
     * @brief Obtém as regiões vizinhas
     * @return Vetor de ponteiros para regiões vizinhas
     */
    const std::vector<WorldRegion*>& getNeighbors() const;
    
    /**
     * @brief Adiciona uma região vizinha
     * @param neighbor Ponteiro para a região vizinha
     */
    void addNeighbor(WorldRegion* neighbor);

private:
    // ID da região
    uint32_t m_regionId;
    
    // Coordenadas da região
    float m_startX;
    float m_startY;
    float m_endX;
    float m_endY;
    
    // Dimensões da região
    float m_width;
    float m_height;
    
    // Conjunto de atores na região
    std::unordered_set<common::types::ActorId> m_actors;
    
    // Regiões vizinhas
    std::vector<WorldRegion*> m_neighbors;
    
    // Flag de inicialização
    bool m_initialized;
};

} // namespace wydbr::tmsrv::world 