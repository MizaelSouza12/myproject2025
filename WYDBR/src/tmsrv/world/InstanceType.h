#pragma once

#include <cstdint>

namespace wydbr::tmsrv::world {

/**
 * @enum InstanceType
 * @brief Enumeração dos tipos de instâncias de mapa.
 * 
 * Define os diferentes tipos de instâncias que podem ser criadas
 * no sistema de mundo.
 */
enum class InstanceType : uint8_t {
    // Instância normal (mapa padrão)
    NORMAL = 0,
    
    // Instância permanente (não é removida automaticamente)
    PERMANENT = 1,
    
    // Instância de dungeon (específica para dungeons)
    DUNGEON = 2,
    
    // Instância de raid (para raids com vários jogadores)
    RAID = 3,
    
    // Instância de arena (para PvP)
    ARENA = 4,
    
    // Instância de evento (para eventos especiais)
    EVENT = 5,
    
    // Instância de guilda (para conteúdo de guilda)
    GUILD = 6,
    
    // Instância de missão (para missões específicas)
    QUEST = 7,
    
    // Instância de tutorial (para o tutorial do jogo)
    TUTORIAL = 8,
    
    // Instância de teste (para testes de desenvolvimento)
    TEST = 9,
    
    // Valor máximo (para validação)
    MAX_VALUE = 10
};

/**
 * @brief Converte um tipo de instância para string
 * @param type Tipo de instância
 * @return Nome do tipo de instância como string
 */
inline const char* InstanceTypeToString(InstanceType type) {
    switch (type) {
        case InstanceType::NORMAL:    return "Normal";
        case InstanceType::PERMANENT: return "Permanente";
        case InstanceType::DUNGEON:   return "Dungeon";
        case InstanceType::RAID:      return "Raid";
        case InstanceType::ARENA:     return "Arena";
        case InstanceType::EVENT:     return "Evento";
        case InstanceType::GUILD:     return "Guilda";
        case InstanceType::QUEST:     return "Missão";
        case InstanceType::TUTORIAL:  return "Tutorial";
        case InstanceType::TEST:      return "Teste";
        default:                      return "Desconhecido";
    }
}

/**
 * @brief Verifica se um valor é um tipo de instância válido
 * @param value Valor a ser verificado
 * @return true se o valor é um tipo de instância válido, false caso contrário
 */
inline bool IsValidInstanceType(uint8_t value) {
    return value < static_cast<uint8_t>(InstanceType::MAX_VALUE);
}

} // namespace wydbr::tmsrv::world 