/**
 * @file ActorId.h
 * @brief Define o identificador único para atores no jogo
 */

#ifndef WYDBR_ACTOR_ID_H_
#define WYDBR_ACTOR_ID_H_

#include <cstdint>
#include <functional>

namespace wydbr {
namespace common {
namespace types {

/**
 * @struct ActorId
 * @brief Estrutura para representar um identificador único de ator no jogo
 * 
 * Um ator pode ser um jogador, NPC, monstro, objeto interativo, etc.
 */
struct ActorId {
    uint8_t type;   ///< Tipo de ator (0=jogador, 1=NPC, 2=monstro, etc)
    uint32_t id;    ///< ID único dentro do tipo
    uint16_t world; ///< ID do mundo/instância
    
    /**
     * @brief Construtor padrão, cria um ID inválido
     */
    ActorId() : type(0xFF), id(0), world(0) {}
    
    /**
     * @brief Construtor explícito
     */
    ActorId(uint8_t t, uint32_t i, uint16_t w = 0) : type(t), id(i), world(w) {}
    
    /**
     * @brief Verifica se este ID é válido
     * @return true se o ID é válido
     */
    bool IsValid() const {
        return type != 0xFF && id != 0;
    }
    
    /**
     * @brief Operador de igualdade
     */
    bool operator==(const ActorId& other) const {
        return type == other.type && id == other.id && world == other.world;
    }
    
    /**
     * @brief Operador de desigualdade
     */
    bool operator!=(const ActorId& other) const {
        return !(*this == other);
    }
    
    /**
     * @brief Operador de menor que (para ordenação)
     */
    bool operator<(const ActorId& other) const {
        if (type != other.type) return type < other.type;
        if (world != other.world) return world < other.world;
        return id < other.id;
    }
};

/**
 * @struct ActorIdHash
 * @brief Função de hash para ActorId
 */
struct ActorIdHash {
    std::size_t operator()(const ActorId& actorId) const {
        std::size_t h1 = std::hash<uint8_t>()(actorId.type);
        std::size_t h2 = std::hash<uint32_t>()(actorId.id);
        std::size_t h3 = std::hash<uint16_t>()(actorId.world);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

} // namespace types
} // namespace common
} // namespace wydbr

#endif // WYDBR_ACTOR_ID_H_ 