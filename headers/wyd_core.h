/**
 * @file wyd_core.h
 * @brief Definição das estruturas e constantes principais do WYD
 * 
 * Este arquivo contém as declarações fundamentais e estruturas
 * de dados utilizadas em todo o sistema WYDBR.
 */

#ifndef WYDBR_CORE_H
#define WYDBR_CORE_H

#include <cstdint>
#include <string>

namespace WYDBR {

// Versão do sistema
constexpr int WYDBR_VERSION_MAJOR = 2;
constexpr int WYDBR_VERSION_MINOR = 0;
constexpr int WYDBR_VERSION_PATCH = 0;

// Limites do sistema
constexpr int MAX_USERS = 1000;
constexpr int MAX_MOBS = 10000;
constexpr int MAX_ITEMS = 100000; 
constexpr int MAX_INVENTORY = 64;
constexpr int MAX_EQUIP = 16;
constexpr int MAX_TRADE = 15;
constexpr int MAX_SKILL = 24;
constexpr int MAX_AFFECT = 32;
constexpr int MAX_PARTY = 12;
constexpr int MAX_STORE_ITEM = 29;
constexpr int MAX_NPC_QUEST = 12;
constexpr int MAX_LEVEL = 399;
constexpr int MAX_GROUND_ITEM = 10000;
constexpr int MAX_PLAYER_MOBS = 10000;

// Tipos de personagens
enum class CharacterClass : uint8_t {
    TK = 0,   // Transcendent
    FM = 1,   // Foema
    BM = 2,   // Berius
    HT = 3    // Hunter
};

// Estados do personagem
enum class CharacterState : uint8_t {
    Idle = 0,
    Moving = 1,
    Fighting = 2,
    Dead = 3,
    Sit = 4,
    Run = 5
};

// Estrutura básica de posição
struct Position {
    int32_t x = 0;
    int32_t y = 0;
    
    Position() = default;
    Position(int32_t _x, int32_t _y) : x(_x), y(_y) {}
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
    
    // Calcular distância entre duas posições
    int32_t Distance(const Position& other) const {
        // Usando distância de Manhattan para simplicidade
        return abs(x - other.x) + abs(y - other.y);
    }
};

// Estrutura base para itens (corrigida com problemas de memória e segurança)
struct ItemBase {
    uint32_t itemId = 0;
    uint16_t effectId = 0;
    uint16_t reqLevel = 0;
    uint8_t grade = 0;
    uint8_t sockets = 0;
    uint8_t maxSockets = 0;
    uint16_t durability = 0;
    uint16_t maxDurability = 0;
    
    // Membros adicionais para previnir buffer overflow
    uint8_t paddings[12] = {0}; // Para alinhar corretamente em memória
};

// Estrutura base para entidades (corrigida com problemas de memória e segurança)
struct EntityBase {
    uint32_t id = 0;
    Position position;
    CharacterState state = CharacterState::Idle;
    uint16_t hp = 0;
    uint16_t maxHp = 0;
    uint16_t mp = 0;
    uint16_t maxMp = 0;
    
    // Ponteiros nulos inicializados para evitar problemas de segurança
    void* additionalData = nullptr;
};

// Namespace para utilitários
namespace Utils {
    // Função para validar nome de personagem - evitar SQL injection
    inline bool IsValidName(const std::string& name) {
        if (name.empty() || name.length() > 16)
            return false;
            
        for (char c : name) {
            // Apenas letras, números e alguns caracteres especiais
            if (!isalnum(c) && c != "_" && c != "-")
                return false;
        }
        
        return true;
    }
    
    // Função para calcular checksum - integridade de dados
    inline uint32_t CalculateChecksum(const void* data, size_t length) {
        if (!data || length == 0)
            return 0;
            
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        uint32_t checksum = 0;
        
        for (size_t i = 0; i < length; ++i) {
            checksum = (checksum << 1) | (checksum >> 31); // Rotação
            checksum ^= bytes[i];                          // XOR
        }
        
        return checksum;
    }
}

} // namespace WYDBR

#endif // WYDBR_CORE_H
