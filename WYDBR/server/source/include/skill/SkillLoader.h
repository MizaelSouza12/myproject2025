/**
 * SkillLoader.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/skill/SkillLoader.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _SKILL_LOADER_H_
#define _SKILL_LOADER_H_

/**
 * @file SkillLoader.h
 * @brief Carregador de habilidades do WYD
 * 
 * Este arquivo contém as funções para carregar as habilidades do WYD
 * a partir de arquivos binários, mantendo a compatibilidade com
 * o formato original do jogo.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

#include "../Basedef.h"
#include "../TM_Skill.h"
#include "../TMStruct.h"
#include "SkillBinStructs.h"
#include "SkillConstants.h"

namespace wydbr {
namespace skill {

/**
 * @brief Classe para carregar dados de habilidades do arquivo Skill.bin
 */
class SkillLoader {
public:
    /**
     * Construtor
     */
    SkillLoader();

    /**
     * Destrutor
     */
    ~SkillLoader();

    /**
     * Carrega as habilidades do arquivo binário
     * 
     * @param filePath Caminho para o arquivo Skill.bin
     * @return true se o carregamento foi bem-sucedido, false caso contrário
     */
    bool loadFromFile(const std::string& filePath);

    /**
     * Retorna o número de habilidades carregadas
     * 
     * @return Número de habilidades carregadas
     */
    size_t getSkillCount() const;

    /**
     * Retorna os dados de uma habilidade específica
     * 
     * @param skillId ID da habilidade
     * @return Ponteiro para os dados da habilidade, ou nullptr se não for encontrada
     */
    const STRUCT_SKILLDATA* getSkillData(uint16_t skillId) const;

    /**
     * Retorna todas as habilidades carregadas
     * 
     * @return Mapa de ID da habilidade para dados de habilidade
     */
    const std::map<uint16_t, STRUCT_SKILLDATA>& getAllSkills() const;

    /**
     * Verifica se uma habilidade específica existe
     * 
     * @param skillId ID da habilidade
     * @return true se a habilidade existe, false caso contrário
     */
    bool hasSkill(uint16_t skillId) const;

    /**
     * Calcula o dano base de uma habilidade
     * 
     * @param skillId ID da habilidade
     * @param level Nível da habilidade
     * @param baseDamage Dano base do jogador
     * @return Dano calculado
     */
    int32_t calculateSkillDamage(uint16_t skillId, uint8_t level, int32_t baseDamage) const;

    /**
     * Verifica se uma habilidade é passiva
     * 
     * @param skillId ID da habilidade
     * @return true se a habilidade é passiva, false caso contrário
     */
    bool isPassiveSkill(uint16_t skillId) const;

    /**
     * Verifica se uma habilidade é agressiva
     * 
     * @param skillId ID da habilidade
     * @return true se a habilidade é agressiva, false caso contrário
     */
    bool isAggressiveSkill(uint16_t skillId) const;

    /**
     * Obtém o tipo de alvo de uma habilidade
     * 
     * @param skillId ID da habilidade
     * @return Tipo de alvo (SkillTargetType) ou SKILL_TARGET_NONE se a habilidade não existir
     */
    SkillTargetType getTargetType(uint16_t skillId) const;

    /**
     * Obtém o tipo de instância de uma habilidade
     * 
     * @param skillId ID da habilidade
     * @return Tipo de instância (SkillInstanceType) ou SKILL_INSTANCE_NONE se a habilidade não existir
     */
    SkillInstanceType getInstanceType(uint16_t skillId) const;

    /**
     * Obtém o alcance de uma habilidade
     * 
     * @param skillId ID da habilidade
     * @return Alcance da habilidade ou 0 se a habilidade não existir
     */
    int32_t getRange(uint16_t skillId) const;

    /**
     * Obtém o custo de mana de uma habilidade
     * 
     * @param skillId ID da habilidade
     * @param level Nível da habilidade
     * @return Custo de mana ou 0 se a habilidade não existir
     */
    int32_t getManaCost(uint16_t skillId, uint8_t level = 1) const;

    /**
     * Obtém o cooldown de uma habilidade
     * 
     * @param skillId ID da habilidade
     * @param level Nível da habilidade
     * @return Cooldown em milissegundos ou 0 se a habilidade não existir
     */
    int32_t getCooldown(uint16_t skillId, uint8_t level = 1) const;

    /**
     * Salva os dados de habilidades para um arquivo binário
     * 
     * @param filePath Caminho para o arquivo de saída
     * @return true se a operação foi bem-sucedida, false caso contrário
     */
    bool saveToFile(const std::string& filePath) const;

    /**
     * Cria uma habilidade e a adiciona ao carregador
     * 
     * @param skillId ID da habilidade
     * @param skillData Dados da habilidade
     * @return true se a habilidade foi criada com sucesso, false caso contrário
     */
    bool createSkill(uint16_t skillId, const STRUCT_SKILLDATA& skillData);

    /**
     * Atualiza uma habilidade existente
     * 
     * @param skillId ID da habilidade
     * @param skillData Novos dados da habilidade
     * @return true se a habilidade foi atualizada com sucesso, false caso contrário
     */
    bool updateSkill(uint16_t skillId, const STRUCT_SKILLDATA& skillData);

    /**
     * Remove uma habilidade do carregador
     * 
     * @param skillId ID da habilidade a ser removida
     * @return true se a habilidade foi removida com sucesso, false caso contrário
     */
    bool removeSkill(uint16_t skillId);

private:
    /**
     * Valida o cabeçalho do arquivo Skill.bin
     * 
     * @param header Cabeçalho do arquivo
     * @return true se o cabeçalho é válido, false caso contrário
     */
    bool validateFileHeader(const BIN_HEADER& header) const;

    /**
     * Processa os dados brutos da habilidade
     * 
     * @param skillId ID da habilidade
     * @param rawData Dados brutos da habilidade
     * @return true se o processamento foi bem-sucedido, false caso contrário
     */
    bool processRawSkillData(uint16_t skillId, const STRUCT_SKILLDATA& rawData);

    /**
     * Escreve o cabeçalho no arquivo binário
     * 
     * @param stream Stream de saída
     * @return true se a operação foi bem-sucedida, false caso contrário
     */
    bool writeFileHeader(std::ofstream& stream) const;

    /**
     * Verifica se uma habilidade está dentro dos limites válidos
     * 
     * @param skillId ID da habilidade
     * @return true se a habilidade está dentro dos limites, false caso contrário
     */
    bool isValidSkillId(uint16_t skillId) const;

    /**
     * Converte o Target numérico para o tipo enumerado
     * 
     * @param targetValue Valor Target da habilidade
     * @return Tipo de alvo (SkillTargetType)
     */
    SkillTargetType convertTargetValue(int32_t targetValue) const;

    /**
     * Converte o InstanceType numérico para o tipo enumerado
     * 
     * @param instanceTypeValue Valor InstanceType da habilidade
     * @return Tipo de instância (SkillInstanceType)
     */
    SkillInstanceType convertInstanceTypeValue(int32_t instanceTypeValue) const;

    // Mapa de habilidades carregadas
    std::map<uint16_t, STRUCT_SKILLDATA> m_skills;
    
    // Flag para indicar se as habilidades foram carregadas
    bool m_loaded;
    
    // Número de habilidades carregadas
    size_t m_skillCount;
    
    // Versão usada para salvar arquivos
    uint32_t m_fileVersion;
};

// Instância singleton global
extern std::unique_ptr<SkillLoader> g_skillLoader;

} // namespace skill
} // namespace wydbr

#endif // _SKILL_LOADER_H_