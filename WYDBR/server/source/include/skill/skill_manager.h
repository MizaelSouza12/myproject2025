/**
 * @file skill_manager.h
 * @brief Gerenciador de habilidades do WYDBR
 * 
 * Este arquivo define o gerenciador de habilidades do WYDBR, que utiliza
 * um sistema híbrido para manter compatibilidade binária perfeita com o
 * cliente original do WYD enquanto oferece funcionalidades avançadas.
 */

#ifndef _SKILL_MANAGER_H_
#define _SKILL_MANAGER_H_

#include "skill_structures.h"
#include "skill_converters.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace wydbr {
namespace skill {

// Definições e constantes
#define MAX_SKILL           250   // Número máximo de habilidades no sistema
#define SKILL_PATH          "SkillData.bin"  // Caminho padrão para arquivo de habilidades

/**
 * @brief Classe gerenciadora do sistema de habilidades
 * 
 * Esta classe é responsável pelo carregamento, salvamento e manipulação
 * de todas as habilidades do sistema, incluindo a conversão entre os formatos
 * compatível e estendido.
 */
class SkillManager {
public:
    /**
     * Obtém a instância singleton do gerenciador
     * @return Referência para a instância
     */
    static SkillManager& GetInstance();
    
    /**
     * Carrega habilidades de um arquivo binário
     * @param filePath Caminho para o arquivo
     * @return true se bem-sucedido, false caso contrário
     */
    bool loadFromFile(const std::string& filePath);
    
    /**
     * Salva habilidades em um arquivo binário
     * @param filePath Caminho para o arquivo
     * @return true se bem-sucedido, false caso contrário
     */
    bool saveToFile(const std::string& filePath);
    
    /**
     * Obtém uma habilidade estendida pelo ID
     * @param skillId ID da habilidade
     * @return Ponteiro para a habilidade estendida ou nullptr se não encontrada
     */
    const STRUCT_SKILLDATA_EXTENDED* getSkill(uint16_t skillId) const;
    
    /**
     * Obtém uma habilidade compatível pelo ID
     * @param skillId ID da habilidade
     * @return Estrutura compatível da habilidade ou estrutura vazia se não encontrada
     */
    STRUCT_SKILLDATA_COMPAT getCompatSkill(uint16_t skillId) const;
    
    /**
     * Atualiza uma habilidade com nova versão estendida
     * @param skillId ID da habilidade
     * @param skill Nova versão estendida da habilidade
     * @return true se atualizada com sucesso, false caso contrário
     */
    bool updateSkill(uint16_t skillId, const STRUCT_SKILLDATA_EXTENDED& skill);
    
    /**
     * Atualiza uma habilidade com nova versão compatível
     * @param skillId ID da habilidade
     * @param skill Nova versão compatível da habilidade
     * @return true se atualizada com sucesso, false caso contrário
     */
    bool updateCompatSkill(uint16_t skillId, const STRUCT_SKILLDATA_COMPAT& skill);
    
    /**
     * Verifica se uma habilidade existe
     * @param skillId ID da habilidade
     * @return true se existir, false caso contrário
     */
    bool hasSkill(uint16_t skillId) const;
    
    /**
     * Obtém o número total de habilidades carregadas
     * @return Número de habilidades
     */
    size_t getSkillCount() const;
    
    /**
     * Verifica se as habilidades foram carregadas
     * @return true se carregadas, false caso contrário
     */
    bool isLoaded() const;
    
    /**
     * Calcula o efeito de uma habilidade com lógica estendida
     * @param skillId ID da habilidade
     * @param casterLevel Nível do conjurador
     * @param targetLevel Nível do alvo
     * @param baseStr FOR base do conjurador
     * @param baseDex DES base do conjurador
     * @param baseInt INT base do conjurador
     * @param baseCon CON base do conjurador
     * @return Dano ou efeito calculado
     */
    int calculateSkillEffect(uint16_t skillId, int casterLevel, int targetLevel, 
                         int baseStr, int baseDex, int baseInt, int baseCon);
    
    /**
     * Calcula o efeito de uma habilidade com parâmetros específicos
     * @param skill Habilidade estendida
     * @param casterLevel Nível do conjurador
     * @param targetLevel Nível do alvo
     * @param baseStr FOR base do conjurador
     * @param baseDex DES base do conjurador
     * @param baseInt INT base do conjurador
     * @param baseCon CON base do conjurador
     * @param targetDefense Defesa do alvo
     * @return Estrutura com resultado do cálculo
     */
    struct SkillCalculationResult {
        int damage;             // Dano total
        int baseEffect;         // Efeito base
        int scalingBonus;       // Bônus por escala de atributos
        bool critical;          // Se é crítico
        int penetration;        // Penetração de armadura
        int secondaryEffect;    // Valor do efeito secundário
        bool appliesSecondary;  // Se aplica efeito secundário
    };
    
    SkillCalculationResult calculateDetailedEffect(
        const STRUCT_SKILLDATA_EXTENDED& skill,
        int casterLevel, int targetLevel,
        int baseStr, int baseDex, int baseInt, int baseCon,
        int targetDefense);
    
    /**
     * Importa habilidades do formato de arquivo ItemList.bin original
     * @param filePath Caminho para o arquivo
     * @return Número de habilidades importadas ou -1 se erro
     */
    int importFromOriginalFormat(const std::string& filePath);
    
    /**
     * Cria arquivo de teste com habilidades de exemplo
     * @param filePath Caminho para o arquivo
     * @return true se bem-sucedido, false caso contrário
     */
    bool createTestSkillFile(const std::string& filePath);

private:
    // Construtor e destrutor privados (singleton)
    SkillManager();
    ~SkillManager();
    
    // Mapa de habilidades (ID -> STRUCT_SKILLDATA_EXTENDED)
    std::unordered_map<uint16_t, STRUCT_SKILLDATA_EXTENDED> m_skills;
    
    // Flag que indica se as habilidades foram carregadas
    bool m_loaded;
    
    // Mutex para acesso thread-safe
    mutable std::mutex m_mutex;
    
    // Cache das últimas execuções para otimização
    struct SkillCalculationCache {
        uint16_t skillId;
        int casterLevel;
        int targetLevel;
        int casterStats[4];
        int targetDefense;
        int result;
        unsigned int timestamp;
    };
    
    // Array de cache para melhorar performance em combate
    static const int CACHE_SIZE = 100;
    SkillCalculationCache m_calculationCache[CACHE_SIZE];
    int m_cacheIndex;
    
    // Limpa o cache de cálculos
    void _clearCalculationCache();
    
    // Busca no cache
    bool _findInCache(uint16_t skillId, int casterLevel, int targetLevel,
                    int baseStr, int baseDex, int baseInt, int baseCon,
                    int targetDefense, int& outResult);
    
    // Adiciona ao cache
    void _addToCache(uint16_t skillId, int casterLevel, int targetLevel,
                   int baseStr, int baseDex, int baseInt, int baseCon,
                   int targetDefense, int result);
};

// Variável global para acesso fácil ao gerenciador
extern SkillManager* g_skillManager;

} // namespace skill
} // namespace wydbr

#endif // _SKILL_MANAGER_H_