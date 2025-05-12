/**
 * @file skill_converters.h
 * @brief Funções para conversão entre diferentes formatos de estruturas de habilidades
 * 
 * Este arquivo contém as funções necessárias para converter entre a estrutura de 
 * habilidade compatível com o WYD original (32 bytes) e a estrutura estendida 
 * usada internamente (92 bytes). Isso permite manter a compatibilidade com os 
 * arquivos binários originais enquanto se utiliza funcionalidades avançadas.
 */

#ifndef _SKILL_CONVERTERS_H_
#define _SKILL_CONVERTERS_H_

#include "skill_structures.h"
#include <string.h>

/**
 * @brief Converte uma estrutura compatível para uma estrutura estendida
 * 
 * @param compat Ponteiro para a estrutura compatível de origem (32 bytes)
 * @param extended Ponteiro para a estrutura estendida de destino (92 bytes)
 * @return int 1 se sucesso, 0 se erro
 */
static inline int SkillConvertCompatToExtended(const STRUCT_SKILLDATA_COMPAT* compat, STRUCT_SKILLDATA_EXTENDED* extended) {
    if (!compat || !extended) {
        return 0;
    }

    // Zera a estrutura estendida
    memset(extended, 0, sizeof(STRUCT_SKILLDATA_EXTENDED));

    // Copia os dados compatíveis
    memcpy(extended->Name, compat->Name, sizeof(compat->Name));
    extended->Coin = compat->Coin;
    extended->Pos = compat->Pos;
    extended->Circle = compat->Circle;
    memcpy(extended->CompatUnk, compat->Unk, sizeof(compat->Unk));

    // Inicializa alguns valores padrões com base nos dados do WYD original
    // (A lógica de negócio real dependerá da implementação específica)
    switch (extended->Circle) {
        case 0: // Círculo 1
            extended->BaseDamage = 50;
            extended->CooldownTime = 1000; // 1 segundo
            extended->MaxLevel = 20;
            break;
        case 1: // Círculo 2
            extended->BaseDamage = 100;
            extended->CooldownTime = 3000; // 3 segundos
            extended->MaxLevel = 20;
            break;
        case 2: // Círculo 3
            extended->BaseDamage = 180;
            extended->CooldownTime = 6000; // 6 segundos
            extended->MaxLevel = 20;
            break;
        case 3: // Círculo 4
            extended->BaseDamage = 300;
            extended->CooldownTime = 12000; // 12 segundos
            extended->MaxLevel = 20;
            break;
        default:
            extended->BaseDamage = 30;
            extended->CooldownTime = 1000;
            extended->MaxLevel = 20;
    }

    // Os valores estendidos adicionais serão inicializados por padrão em 0
    // e são preenchidos posteriormente com base em arquivos de configuração

    return 1;
}

/**
 * @brief Converte uma estrutura estendida para uma estrutura compatível
 * 
 * @param extended Ponteiro para a estrutura estendida de origem (92 bytes)
 * @param compat Ponteiro para a estrutura compatível de destino (32 bytes)
 * @return int 1 se sucesso, 0 se erro
 */
static inline int SkillConvertExtendedToCompat(const STRUCT_SKILLDATA_EXTENDED* extended, STRUCT_SKILLDATA_COMPAT* compat) {
    if (!extended || !compat) {
        return 0;
    }

    // Zera a estrutura compatível
    memset(compat, 0, sizeof(STRUCT_SKILLDATA_COMPAT));

    // Copia os campos básicos necessários para compatibilidade
    memcpy(compat->Name, extended->Name, sizeof(compat->Name));
    compat->Coin = extended->Coin;
    compat->Pos = extended->Pos;
    compat->Circle = extended->Circle;
    memcpy(compat->Unk, extended->CompatUnk, sizeof(compat->Unk));

    return 1;
}

#endif /* _SKILL_CONVERTERS_H_ */
