/**
 * @file TM_Skill.h
 * @brief Interface do sistema de habilidades para o TMStruct.h
 * 
 * Este arquivo define a interface entre o sistema de habilidades híbrido e o 
 * sistema original do WYD. Ele expõe apenas as estruturas compatíveis (32 bytes)
 * para manter a compatibilidade com o TMStruct.h original.
 */

#ifndef _TM_SKILL_H_
#define _TM_SKILL_H_

#include <stdint.h>

#pragma pack(push, 1)

/**
 * @brief Efeito de bônus para compatibilidade com o WYD original
 */
typedef struct _STRUCT_BONUSEFFECT {
    char cEffect;   // Tipo de efeito (1 byte)
    char cValue;    // Valor do efeito (1 byte)
} STRUCT_BONUSEFFECT;

/**
 * @brief Estrutura de skill compatível com o WYD original
 * 
 * Esta estrutura é idêntica à usada no WYD original, com 32 bytes de tamanho total.
 * Ela é usada para comunicação com o cliente e para manter compatibilidade com
 * os arquivos binários originais.
 */
typedef struct _STRUCT_SKILLDATA {
    char    Name[16];        // Nome da habilidade (0-15) - 16 bytes
    uint8_t Coin;            // Custo em moedas (16) - 1 byte 
    uint8_t Pos;             // Posição no grimório (17) - 1 byte
    uint8_t Circle;          // Círculo da habilidade (18) - 1 byte 
    uint8_t Unk[13];         // Bytes reservados/desconhecidos (19-31) - 13 bytes
    /* Tamanho Total: 32 bytes */
} STRUCT_SKILLDATA;

#pragma pack(pop)

// Funções para carregamento/salvamento de habilidades (para uso com o sistema original)
int TMLoadSkillData(const char* fileName);
int TMSaveSkillData(const char* fileName);

// Funções para integração com outros sistemas
int TMCalculateSkillDamage(int skillID, int str, int inte, int dex, int con);
int TMApplySkillEffect(int skillID, int casterID, int targetID);
int TMIsSkillInCooldown(int skillID, int charID);
void TMSetSkillCooldown(int skillID, int charID, int cooldownTime);
void TMUpdateSkillSystem(int tickTime);

#endif /* _TM_SKILL_H_ */
