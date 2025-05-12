/**
 * @file skill_cache_manager.h
 * @brief Sistema de cache multi-camada para cálculos de habilidades
 * 
 * Este arquivo implementa um sistema de cache que otimiza cálculos repetitivos
 * de efeitos de habilidades para reduzir a carga do servidor em combates intensos.
 */

#ifndef _SKILL_CACHE_MANAGER_H_
#define _SKILL_CACHE_MANAGER_H_

#include <unordered_map>
#include <mutex>
#include <chrono>
#include <functional>

namespace wydbr {
namespace cache {

/**
 * Chave para o cache de habilidades
 */
struct SkillCacheKey {
    int skillId;
    int casterLevel;
    int casterStats[4];  // STR, INT, DEX, CON
    int targetLevel;
    int targetDefense;
    
    // Operador de igualdade para comparação de chaves
    bool operator==(const SkillCacheKey& other) const {
        return skillId == other.skillId &&
               casterLevel == other.casterLevel &&
               casterStats[0] == other.casterStats[0] &&
               casterStats[1] == other.casterStats[1] &&
               casterStats[2] == other.casterStats[2] &&
               casterStats[3] == other.casterStats[3] &&
               targetLevel == other.targetLevel &&
               targetDefense == other.targetDefense;
    }
};

/**
 * Função de hash para a chave de cache
 */
} // namespace cache
} // namespace wydbr

// Hash para SkillCacheKey (fora do namespace para STL)
namespace std {
    template <>
    struct hash<wydbr::cache::SkillCacheKey> {
        size_t operator()(const wydbr::cache::SkillCacheKey& key) const {
            // Combinar os hashes de cada campo
            size_t h1 = hash<int>()(key.skillId);
            size_t h2 = hash<int>()(key.casterLevel);
            size_t h3 = hash<int>()(key.targetLevel);
            
            // Combinar os hashes
            size_t result = h1;
            result = result * 31 + h2;
            result = result * 31 + h3;
            
            // Adicionar stats
            for (int i = 0; i < 4; i++) {
                result = result * 31 + hash<int>()(key.casterStats[i]);
            }
            
            // Adicionar defesa
            result = result * 31 + hash<int>()(key.targetDefense);
            
            return result;
        }
    };
}

namespace wydbr {
namespace cache {

/**
 * Resultado cacheado de cálculo de habilidade
 */
struct SkillCacheResult {
    int damage;
    int effectValue;
    int duration;
    bool critical;
    unsigned long long timestamp;
    
    SkillCacheResult() : damage(0), effectValue(0), duration(0), 
                        critical(false), timestamp(0) {}
};

/**
 * Gerenciador de cache multi-camada
 */
class SkillCacheManager {
public:
    static SkillCacheManager& GetInstance();
    
    /**
     * Tenta obter um resultado do cache
     * @param key Chave de cache
     * @param outResult Resultado (preenchido se encontrado)
     * @return true se encontrado no cache, false caso contrário
     */
    bool Get(const SkillCacheKey& key, SkillCacheResult& outResult);
    
    /**
     * Armazena um resultado no cache
     * @param key Chave de cache
     * @param result Resultado a armazenar
     */
    void Set(const SkillCacheKey& key, const SkillCacheResult& result);
    
    /**
     * Invalida entradas do cache por skillId
     * @param skillId ID da habilidade
     */
    void InvalidateBySkill(int skillId);
    
    /**
     * Invalida todas as entradas do cache
     */
    void Clear();
    
    /**
     * Estatísticas do cache
     */
    struct CacheStats {
        int hits;
        int misses;
        int entries;
        float hitRatio;
    };
    
    /**
     * Obtém estatísticas do cache
     * @return Estatísticas
     */
    CacheStats GetStats() const;
    
private:
    SkillCacheManager();
    ~SkillCacheManager();
    
    // Cache em memória
    std::unordered_map<SkillCacheKey, SkillCacheResult> m_memoryCache;
    
    // Mutex para acesso thread-safe
    std::mutex m_mutex;
    
    // Estatísticas
    CacheStats m_stats;
    
    // Limpa entradas antigas
    void _CleanupExpiredEntries();
};

} // namespace cache
} // namespace wydbr

#endif // _SKILL_CACHE_MANAGER_H_
