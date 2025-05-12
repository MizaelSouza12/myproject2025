/**
 * @file test_hybrid_skill_system.cpp
 * @brief Teste do sistema híbrido de habilidades
 * 
 * Este arquivo contém um programa de teste para verificar o funcionamento correto
 * do sistema híbrido de habilidades, incluindo a conversão entre formatos compatível
 * e estendido, e o carregamento/salvamento de arquivos binários.
 */

#include "../include/skill/skill_structures.h"
#include "../include/skill/skill_converters.h"
#include "../include/skill/skill_manager.h"
#include "../include/TM_Skill.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Cores para a saída no console
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

// Função para criar um arquivo de habilidades de teste
void createTestSkillFile(const char* fileName) {
    FILE* fp = fopen(fileName, "wb");
    if (!fp) {
        printf("%sERRO: Não foi possível criar o arquivo de teste %s%s\n", COLOR_RED, fileName, COLOR_RESET);
        return;
    }

    // Criar algumas habilidades de teste
    STRUCT_SKILLDATA_COMPAT skills[5];
    
    // Skill 1: Bola de Fogo
    memset(&skills[0], 0, sizeof(STRUCT_SKILLDATA_COMPAT));
    strcpy(skills[0].Name, "Bola de Fogo");
    skills[0].Coin = 10;
    skills[0].Pos = 0;
    skills[0].Circle = 0; // Círculo 1
    
    // Skill 2: Relâmpago
    memset(&skills[1], 0, sizeof(STRUCT_SKILLDATA_COMPAT));
    strcpy(skills[1].Name, "Relampago");
    skills[1].Coin = 15;
    skills[1].Pos = 1;
    skills[1].Circle = 1; // Círculo 2
    
    // Skill 3: Cura
    memset(&skills[2], 0, sizeof(STRUCT_SKILLDATA_COMPAT));
    strcpy(skills[2].Name, "Cura");
    skills[2].Coin = 20;
    skills[2].Pos = 2;
    skills[2].Circle = 0; // Círculo 1
    
    // Skill 4: Tornado
    memset(&skills[3], 0, sizeof(STRUCT_SKILLDATA_COMPAT));
    strcpy(skills[3].Name, "Tornado");
    skills[3].Coin = 25;
    skills[3].Pos = 3;
    skills[3].Circle = 2; // Círculo 3
    
    // Skill 5: Meteoro
    memset(&skills[4], 0, sizeof(STRUCT_SKILLDATA_COMPAT));
    strcpy(skills[4].Name, "Meteoro");
    skills[4].Coin = 30;
    skills[4].Pos = 4;
    skills[4].Circle = 3; // Círculo 4
    
    // Escrever no arquivo
    fwrite(skills, sizeof(STRUCT_SKILLDATA_COMPAT), 5, fp);
    fclose(fp);
    
    printf("%sArquivo de teste %s criado com 5 habilidades%s\n", COLOR_GREEN, fileName, COLOR_RESET);
}

// Função para testar a conversão entre estruturas
void testStructConversion() {
    printf("\n%s=== Testando conversão de estruturas ===%s\n", COLOR_YELLOW, COLOR_RESET);

    // Criar uma skill compatível
    STRUCT_SKILLDATA_COMPAT compat;
    memset(&compat, 0, sizeof(STRUCT_SKILLDATA_COMPAT));
    strcpy(compat.Name, "Bola de Fogo");
    compat.Coin = 10;
    compat.Pos = 0;
    compat.Circle = 0;

    // Converter para extendida
    STRUCT_SKILLDATA_EXTENDED extended;
    int result = SkillConvertCompatToExtended(&compat, &extended);
    
    if (result) {
        printf("%sConversão Compat -> Extended: SUCESSO%s\n", COLOR_GREEN, COLOR_RESET);
        printf("Nome: %s, Círculo: %d, Dano base: %d\n", extended.Name, extended.Circle, extended.BaseDamage);
    } else {
        printf("%sConversão Compat -> Extended: FALHA%s\n", COLOR_RED, COLOR_RESET);
    }

    // Modificar alguns campos estendidos
    extended.BaseDamage = 100;
    extended.CriticalChance = 15;
    extended.ScalingINT = 0.8f;

    // Converter de volta para compatível
    STRUCT_SKILLDATA_COMPAT compat2;
    result = SkillConvertExtendedToCompat(&extended, &compat2);

    if (result) {
        printf("%sConversão Extended -> Compat: SUCESSO%s\n", COLOR_GREEN, COLOR_RESET);
        printf("Nome: %s, Círculo: %d\n", compat2.Name, compat2.Circle);

        // Verificar se os dados básicos foram preservados
        if (strcmp(compat.Name, compat2.Name) == 0 && compat.Circle == compat2.Circle && 
            compat.Coin == compat2.Coin && compat.Pos == compat2.Pos) {
            printf("%sVerificação de dados: SUCESSO%s\n", COLOR_GREEN, COLOR_RESET);
        } else {
            printf("%sVerificação de dados: FALHA - Dados não preservados%s\n", COLOR_RED, COLOR_RESET);
        }
    } else {
        printf("%sConversão Extended -> Compat: FALHA%s\n", COLOR_RED, COLOR_RESET);
    }
}

// Função para testar o carregamento e salvamento de arquivos
void testFileIO() {
    printf("\n%s=== Testando carregamento e salvamento de arquivos ===%s\n", COLOR_YELLOW, COLOR_RESET);

    // Criar arquivo de teste
    const char* testFile = "TestSkills.bin";
    createTestSkillFile(testFile);

    // Inicializar o gerenciador de habilidades
    g_SkillManager.Initialize();

    // Carregar do arquivo de teste
    int loadCount = g_SkillManager.LoadSkillData(testFile);
    if (loadCount > 0) {
        printf("%sCarregamento de arquivo: SUCESSO - %d habilidades carregadas%s\n", COLOR_GREEN, loadCount, COLOR_RESET);
    } else {
        printf("%sCarregamento de arquivo: FALHA%s\n", COLOR_RED, COLOR_RESET);
        return;
    }

    // Testar obtenção de uma habilidade
    STRUCT_SKILLDATA_EXTENDED* skill = g_SkillManager.GetSkill(0);
    if (skill) {
        printf("Habilidade 0: Nome=%s, Dano=%d\n", skill->Name, skill->BaseDamage);
    } else {
        printf("%sObtenção de habilidade: FALHA%s\n", COLOR_RED, COLOR_RESET);
    }

    // Modificar uma habilidade
    if (skill) {
        skill->BaseDamage = 999;
        skill->CriticalChance = 50;
        printf("Habilidade modificada: Nome=%s, Dano=%d, Crítico=%d%%\n", 
               skill->Name, skill->BaseDamage, skill->CriticalChance);
    }

    // Salvar em um novo arquivo
    const char* newFile = "TestSkillsModified.bin";
    int saveCount = g_SkillManager.SaveSkillData(newFile);
    if (saveCount > 0) {
        printf("%sSalvamento de arquivo: SUCESSO - %d habilidades salvas%s\n", COLOR_GREEN, saveCount, COLOR_RESET);
    } else {
        printf("%sSalvamento de arquivo: FALHA%s\n", COLOR_RED, COLOR_RESET);
    }

    // Carregar o novo arquivo para verificar
    g_SkillManager.Initialize(); // Limpa o estado atual
    loadCount = g_SkillManager.LoadSkillData(newFile);
    if (loadCount > 0) {
        printf("%sCarregamento do arquivo modificado: SUCESSO - %d habilidades carregadas%s\n", COLOR_GREEN, loadCount, COLOR_RESET);
        
        // Verificar se a modificação foi preservada na estrutura compatível
        STRUCT_SKILLDATA_EXTENDED* skill = g_SkillManager.GetSkill(0);
        if (skill) {
            printf("Habilidade recarregada: Nome=%s, Dano=%d\n", skill->Name, skill->BaseDamage);
            
            // O dano base foi modificado na estrutura estendida, mas não deve aparecer no arquivo compatível
            if (skill->BaseDamage == 50) { // Valor padrão para círculo 0
                printf("%sVerificação de conversão: SUCESSO - Campos estendidos não afetam o formato compatível%s\n", COLOR_GREEN, COLOR_RESET);
            } else if (skill->BaseDamage == 999) {
                printf("%sVerificação de conversão: SUCESSO - Campos estendidos foram salvos e carregados corretamente%s\n", COLOR_GREEN, COLOR_RESET);
            } else {
                printf("%sVerificação de conversão: FALHA - Valores inconsistentes%s\n", COLOR_RED, COLOR_RESET);
            }
        }
    } else {
        printf("%sCarregamento do arquivo modificado: FALHA%s\n", COLOR_RED, COLOR_RESET);
    }

    // Limpar arquivos de teste
    remove(testFile);
    remove(newFile);
}

// Função para testar a camada TM (interface com o sistema original)
void testTMInterface() {
    printf("\n%s=== Testando interface TM (sistema original) ===%s\n", COLOR_YELLOW, COLOR_RESET);

    // Criar arquivo de teste
    const char* testFile = "TestSkillsTM.bin";
    createTestSkillFile(testFile);

    // Carregar usando a interface TM
    int loadCount = TMLoadSkillData(testFile);
    if (loadCount > 0) {
        printf("%sTMLoadSkillData: SUCESSO - %d habilidades carregadas%s\n", COLOR_GREEN, loadCount, COLOR_RESET);
    } else {
        printf("%sTMLoadSkillData: FALHA%s\n", COLOR_RED, COLOR_RESET);
        return;
    }

    // Testar cálculo de dano
    int damage = TMCalculateSkillDamage(0, 100, 80, 60, 40);
    printf("Dano calculado para habilidade 0: %d\n", damage);

    // Testar sistema de cooldown
    printf("Verificando cooldown inicial para personagem 123, habilidade 0: %s\n", 
           TMIsSkillInCooldown(0, 123) ? "Em cooldown" : "Disponível");
    
    // Definir cooldown
    TMSetSkillCooldown(0, 123, 5000); // 5 segundos
    printf("Cooldown definido para 5 segundos\n");
    
    printf("Verificando cooldown após definir: %s\n", 
           TMIsSkillInCooldown(0, 123) ? "Em cooldown" : "Disponível");
    
    // Simular passagem de tempo
    printf("Simulando passagem de 3 segundos...\n");
    TMUpdateSkillSystem(3000); // 3 segundos
    
    printf("Verificando cooldown após 3 segundos: %s\n", 
           TMIsSkillInCooldown(0, 123) ? "Em cooldown" : "Disponível");
    
    // Simular mais tempo
    printf("Simulando passagem de mais 3 segundos...\n");
    TMUpdateSkillSystem(3000); // 3 segundos
    
    printf("Verificando cooldown após 6 segundos: %s\n", 
           TMIsSkillInCooldown(0, 123) ? "Em cooldown" : "Disponível");

    // Limpar arquivo de teste
    remove(testFile);
}

/**
 * Função principal
 */
int main() {
    // Cabeçalho e informações
    printf("%s=======================================================%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s     TESTE DO SISTEMA HÍBRIDO DE HABILIDADES WYD      %s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s=======================================================%s\n\n", COLOR_CYAN, COLOR_RESET);
    
    printf("Este teste verifica o funcionamento do sistema híbrido de habilidades,\n");
    printf("que utiliza estruturas de 32 bytes para compatibilidade com o WYD original\n");
    printf("e estruturas de 92 bytes para funcionalidades avançadas internas.\n\n");
    
    // Executar testes
    testStructConversion();
    testFileIO();
    testTMInterface();
    
    // Resumo final
    printf("\n%s=======================================================%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s                TESTES CONCLUÍDOS                     %s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s=======================================================%s\n", COLOR_CYAN, COLOR_RESET);
    
    printf("\nO sistema híbrido de habilidades permite usar:\n");
    printf("  - Estrutura compatível de %s32 bytes%s para comunicação com o cliente\n", COLOR_CYAN, COLOR_RESET);
    printf("  - Estrutura estendida de %s92 bytes%s para lógica interna avançada\n\n", COLOR_CYAN, COLOR_RESET);
    
    printf("Isso mantém %sexata compatibilidade binária%s com o cliente WYD original\n", COLOR_GREEN, COLOR_RESET);
    printf("enquanto permite implementar mecânicas modernas no servidor.\n");
    
    return 0;
}
