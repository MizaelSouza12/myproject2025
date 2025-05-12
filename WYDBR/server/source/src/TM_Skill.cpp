/**
 * @file TM_Skill.cpp
 * @brief Implementação da camada de interface do sistema de habilidades
 * 
 * Este arquivo implementa a interface entre o sistema de habilidades híbrido
 * e o sistema original do WYD, proporcionando funções de compatibilidade.
 */

#include "../include/TM_Skill.h"
#include "../include/skill/skill_manager.h"

/**
 * @brief Carrega dados de habilidades de um arquivo binário
 * 
 * @param fileName Nome do arquivo
 * @return int Número de habilidades carregadas, ou -1 se erro
 */
int TMLoadSkillData(const char* fileName) {
    return g_SkillManager.LoadSkillData(fileName);
}

/**
 * @brief Salva dados de habilidades para um arquivo binário
 * 
 * @param fileName Nome do arquivo
 * @return int Número de habilidades salvas, ou -1 se erro
 */
int TMSaveSkillData(const char* fileName) {
    return g_SkillManager.SaveSkillData(fileName);
}

/**
 * @brief Calcula o dano de uma habilidade com base nos atributos
 * 
 * @param skillID ID da habilidade
 * @param str Força do personagem
 * @param inte Inteligência do personagem
 * @param dex Destreza do personagem
 * @param con Constituição do personagem
 * @return int Dano calculado
 */
int TMCalculateSkillDamage(int skillID, int str, int inte, int dex, int con) {
    return g_SkillManager.CalculateSkillDamage(skillID, str, inte, dex, con);
}

/**
 * @brief Aplica os efeitos de uma habilidade em um alvo
 * 
 * @param skillID ID da habilidade
 * @param casterID ID do lançador
 * @param targetID ID do alvo
 * @return int 1 se sucesso, 0 se erro
 */
int TMApplySkillEffect(int skillID, int casterID, int targetID) {
    return g_SkillManager.ApplySkillEffect(skillID, casterID, targetID);
}

/**
 * @brief Verifica se uma habilidade está em cooldown
 * 
 * @param skillID ID da habilidade
 * @param charID ID do personagem
 * @return int 1 se em cooldown, 0 se disponível
 */
int TMIsSkillInCooldown(int skillID, int charID) {
    return g_SkillManager.IsSkillInCooldown(skillID, charID);
}

/**
 * @brief Define o cooldown para uma habilidade
 * 
 * @param skillID ID da habilidade
 * @param charID ID do personagem
 * @param cooldownTime Tempo de cooldown em milissegundos
 */
void TMSetSkillCooldown(int skillID, int charID, int cooldownTime) {
    g_SkillManager.SetSkillCooldown(skillID, charID, cooldownTime);
}

/**
 * @brief Atualiza o sistema de habilidades
 * 
 * @param tickTime Tempo em milissegundos desde a última atualização
 */
void TMUpdateSkillSystem(int tickTime) {
    g_SkillManager.UpdateCooldowns(tickTime);
}
