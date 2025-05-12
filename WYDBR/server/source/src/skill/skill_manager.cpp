/**
 * @file skill_manager.cpp
 * @brief Implementação do gerenciador de habilidades
 * 
 * Este arquivo contém a implementação do gerenciador de habilidades, incluindo
 * o carregamento/salvamento de dados do formato binário original e a conversão 
 * entre formatos compatível e estendido.
 */

#include "../../include/skill/skill_manager.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Instância global
CSkillManager g_SkillManager;

/**
 * Construtor
 */
CSkillManager::CSkillManager() {
    // Inicialização segura
    Clear();
    // Inicializa gerador de números aleatórios (para cálculos de dano crítico)
    srand((unsigned int)time(NULL));
}

/**
 * Destrutor
 */
CSkillManager::~CSkillManager() {
    // Limpa todos os dados
    Clear();
}

/**
 * Inicializa o sistema
 */
int CSkillManager::Initialize() {
    // Limpa o estado anterior
    Clear();
    
    // Carrega os dados das habilidades
    int result = LoadSkillData();
    if (result <= 0) {
        // Falha ao carregar, inicializa com dados padrão
        // (Isso geralmente não deveria acontecer em produção)
        return 0;
    }
    
    return 1;
}

/**
 * Carrega os dados das habilidades do arquivo binário original
 */
int CSkillManager::LoadSkillData(const char* fileName) {
    // Limpa o estado antes de carregar
    Clear();
    
    // Abre o arquivo binário
    FILE* fp = fopen(fileName, "rb");
    if (!fp) {
        return -1; // Erro ao abrir o arquivo
    }
    
    // Lê o número de habilidades (não está no arquivo, então tentamos ler tudo)
    STRUCT_SKILLDATA_COMPAT tempSkill;
    int skillCount = 0;
    
    // Lê e converte cada habilidade
    while (fread(&tempSkill, sizeof(STRUCT_SKILLDATA_COMPAT), 1, fp) == 1) {
        // Verifica se o nome não está vazio (indica habilidade válida)
        if (tempSkill.Name[0] != '\0') {
            // Converte para o formato estendido
            SkillConvertCompatToExtended(&tempSkill, &m_SkillList[skillCount]);
            skillCount++;
            
            // Verifica limite de habilidades
            if (skillCount >= MAX_SKILL) {
                break;
            }
        }
    }
    
    // Fecha o arquivo
    fclose(fp);
    
    // Atualiza o contador de habilidades
    m_SkillCount = skillCount;
    
    return skillCount;
}

/**
 * Salva os dados das habilidades para o arquivo binário original
 */
int CSkillManager::SaveSkillData(const char* fileName) {
    // Abre o arquivo para escrita
    FILE* fp = fopen(fileName, "wb");
    if (!fp) {
        return -1; // Erro ao abrir o arquivo
    }
    
    // Converte e salva cada habilidade
    STRUCT_SKILLDATA_COMPAT tempSkill;
    int savedCount = 0;
    
    for (int i = 0; i < m_SkillCount; i++) {
        // Converte para o formato compatível
        if (SkillConvertExtendedToCompat(&m_SkillList[i], &tempSkill)) {
            // Escreve a habilidade no arquivo
            if (fwrite(&tempSkill, sizeof(STRUCT_SKILLDATA_COMPAT), 1, fp) == 1) {
                savedCount++;
            }
        }
    }
    
    // Fecha o arquivo
    fclose(fp);
    
    return savedCount;
}

/**
 * Obtém os dados de uma habilidade no formato estendido
 */
STRUCT_SKILLDATA_EXTENDED* CSkillManager::GetSkill(int skillID) {
    if (skillID < 0 || skillID >= m_SkillCount) {
        return NULL;
    }
    
    return &m_SkillList[skillID];
}

/**
 * Obtém os dados de uma habilidade no formato compatível
 */
int CSkillManager::GetSkillCompat(int skillID, STRUCT_SKILLDATA_COMPAT& outCompat) {
    // Verifica se o ID é válido
    if (skillID < 0 || skillID >= m_SkillCount) {
        return 0;
    }
    
    // Converte para o formato compatível
    return SkillConvertExtendedToCompat(&m_SkillList[skillID], &outCompat);
}

/**
 * Calcula o dano da habilidade com base nos atributos
 */
int CSkillManager::CalculateSkillDamage(int skillID, int str, int inte, int dex, int con) {
    // Verifica se o ID é válido
    if (skillID < 0 || skillID >= m_SkillCount) {
        return 0;
    }
    
    // Obtém a habilidade
    STRUCT_SKILLDATA_EXTENDED* skill = &m_SkillList[skillID];
    
    // Calcula o dano base
    int damage = skill->BaseDamage;
    
    // Aplica os modificadores de atributos (fórmula avançada)
    damage += (int)(str * skill->ScalingSTR);
    damage += (int)(inte * skill->ScalingINT);
    damage += (int)(dex * skill->ScalingDEX);
    damage += (int)(con * skill->ScalingCON);
    
    // Aplica chance de crítico
    if (skill->CriticalChance > 0) {
        int roll = rand() % 100;
        if (roll < skill->CriticalChance) {
            // Dano crítico!
            damage = (damage * (100 + skill->CriticalMultiplier)) / 100;
        }
    }
    
    return damage;
}

/**
 * Aplica os efeitos da habilidade no alvo
 */
int CSkillManager::ApplySkillEffect(int skillID, int casterID, int targetID) {
    // Verifica se o ID é válido
    if (skillID < 0 || skillID >= m_SkillCount) {
        return 0;
    }
    
    // Aqui seria aplicada a lógica real de aplicação da habilidade,
    // que depende da integração com os sistemas de personagem, combate, etc.
    // Para o propósito desta implementação híbrida, apenas retornamos sucesso.
    
    return 1;
}

/**
 * Verifica se a habilidade está em cooldown
 */
int CSkillManager::IsSkillInCooldown(int skillID, int charID) {
    // Percorre a lista de cooldowns
    for (int i = 0; i < m_CooldownCount; i++) {
        if (m_Cooldowns[i].skillID == skillID && m_Cooldowns[i].charID == charID) {
            // Encontrou o cooldown, verifica se ainda está ativo
            return (m_Cooldowns[i].remainingTime > 0) ? 1 : 0;
        }
    }
    
    // Cooldown não encontrado, habilidade está disponível
    return 0;
}

/**
 * Define o cooldown para uma habilidade
 */
void CSkillManager::SetSkillCooldown(int skillID, int charID, int cooldownTime) {
    // Primeiro, verifica se já existe um cooldown para esta habilidade/personagem
    for (int i = 0; i < m_CooldownCount; i++) {
        if (m_Cooldowns[i].skillID == skillID && m_Cooldowns[i].charID == charID) {
            // Encontrou, atualiza o tempo
            m_Cooldowns[i].remainingTime = cooldownTime;
            return;
        }
    }
    
    // Não encontrou, adiciona um novo cooldown
    if (m_CooldownCount < 1000) {
        m_Cooldowns[m_CooldownCount].charID = charID;
        m_Cooldowns[m_CooldownCount].skillID = skillID;
        m_Cooldowns[m_CooldownCount].remainingTime = cooldownTime;
        m_CooldownCount++;
    }
}

/**
 * Atualiza os cooldowns do sistema
 */
void CSkillManager::UpdateCooldowns(int tickTime) {
    // Atualiza todos os cooldowns ativos
    for (int i = 0; i < m_CooldownCount; i++) {
        if (m_Cooldowns[i].remainingTime > 0) {
            // Reduz o tempo restante
            m_Cooldowns[i].remainingTime -= tickTime;
            
            // Se o cooldown terminou, zera o tempo
            if (m_Cooldowns[i].remainingTime < 0) {
                m_Cooldowns[i].remainingTime = 0;
            }
        }
    }
    
    // Otimização: elimina cooldowns terminados
    int activeCount = 0;
    for (int i = 0; i < m_CooldownCount; i++) {
        if (m_Cooldowns[i].remainingTime > 0) {
            // Ainda ativo, move para o início da lista se necessário
            if (i != activeCount) {
                m_Cooldowns[activeCount] = m_Cooldowns[i];
            }
            activeCount++;
        }
    }
    
    // Atualiza o contador
    m_CooldownCount = activeCount;
}

/**
 * Limpa os dados internos
 */
void CSkillManager::Clear() {
    // Zera todos os dados
    memset(m_SkillList, 0, sizeof(m_SkillList));
    m_SkillCount = 0;
    
    // Limpa os cooldowns
    memset(m_Cooldowns, 0, sizeof(m_Cooldowns));
    m_CooldownCount = 0;
}
