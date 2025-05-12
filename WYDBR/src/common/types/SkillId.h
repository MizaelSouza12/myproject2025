/**
 * @file SkillId.h
 * @brief Define o identificador único para habilidades
 */

#ifndef WYDBR_SKILL_ID_H_
#define WYDBR_SKILL_ID_H_

#include <cstdint>
#include <functional>

namespace wydbr {
namespace common {
namespace types {

/**
 * @struct SkillId
 * @brief Identificador de habilidade
 */
struct SkillId {
    uint32_t id;
    
    SkillId() : id(0) {}
    explicit SkillId(uint32_t skillId) : id(skillId) {}
    
    operator uint32_t() const {
        return id;
    }
    
    bool operator==(const SkillId& other) const {
        return id == other.id;
    }
    
    bool operator!=(const SkillId& other) const {
        return id != other.id;
    }
};

/**
 * @struct SkillIdHash
 * @brief Função de hash para SkillId
 */
struct SkillIdHash {
    std::size_t operator()(const SkillId& skillId) const {
        return std::hash<uint32_t>()(skillId.id);
    }
};

} // namespace types
} // namespace common
} // namespace wydbr

#endif // WYDBR_SKILL_ID_H_ 