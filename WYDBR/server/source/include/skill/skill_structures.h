/**
 * @file skill_structures.h
 * @brief Definições de estruturas relacionadas ao sistema de habilidades do WYD
 * 
 * Este arquivo contém as definições de estruturas de dados para o sistema de habilidades
 * do WYD, incluindo estruturas originais (32 bytes) para compatibilidade com os arquivos
 * binários originais e estruturas estendidas (92 bytes) para recursos avançados.
 */

#ifndef _SKILL_STRUCTURES_H_
#define _SKILL_STRUCTURES_H_

#include <stdint.h>

#pragma pack(push, 1)

/**
 * @brief Estrutura de skill original do WYD, usada para compatibilidade com o cliente original
 * 
 * Esta estrutura possui exatamente 32 bytes, conforme a estrutura original do WYD.
 * É usada para comunicação com o cliente e para leitura/escrita em arquivo binário.
 */
typedef struct _STRUCT_SKILLDATA_COMPAT {
    char    Name[16];        // Nome da habilidade (0-15) - 16 bytes
    uint8_t Coin;            // Custo em moedas (16) - 1 byte 
    uint8_t Pos;             // Posição no grimório (17) - 1 byte
    uint8_t Circle;          // Círculo da habilidade (18) - 1 byte 
    uint8_t Unk[13];         // Bytes reservados/desconhecidos (19-31) - 13 bytes
    /* Tamanho Total: 32 bytes */
} STRUCT_SKILLDATA_COMPAT;

/**
 * @brief Estrutura de skill estendida para funcionalidades avançadas
 * 
 * Esta estrutura estendida possui 92 bytes e é usada internamente para
 * implementação de funcionalidades avançadas no sistema de habilidades.
 * A conversão entre a estrutura compatível e a estendida é feita automaticamente.
 */
typedef struct _STRUCT_SKILLDATA_EXTENDED {
    /* Campos compatíveis (32 bytes) */
    char    Name[16];        // Nome da habilidade - 16 bytes
    uint8_t Coin;            // Custo em moedas - 1 byte
    uint8_t Pos;             // Posição no grimório - 1 byte
    uint8_t Circle;          // Círculo da habilidade - 1 byte
    uint8_t CompatUnk[13];   // Bytes reservados/desconhecidos - 13 bytes

    /* Campos estendidos (60 bytes adicionais) */
    uint8_t Element;         // Elemento da habilidade (fogo, água, terra, etc.)
    uint16_t BaseDamage;     // Dano base da habilidade
    uint16_t BaseHealing;    // Cura base da habilidade
    float   ScalingSTR;      // Multiplicador de força
    float   ScalingINT;      // Multiplicador de inteligência
    float   ScalingDEX;      // Multiplicador de destreza
    float   ScalingCON;      // Multiplicador de constituição
    uint16_t CooldownTime;   // Tempo de recarga em milissegundos
    uint16_t CastTime;       // Tempo de lançamento em milissegundos
    uint8_t  CriticalChance; // Chance de crítico (0-100)
    uint8_t  CriticalMultiplier; // Multiplicador de dano crítico (em %)
    uint8_t  SkillCategory;  // Categoria da habilidade (física, mágica, etc.)
    uint8_t  TargetType;     // Tipo de alvo (único, área, si mesmo, etc.)
    uint16_t AreaOfEffect;   // Raio de efeito em área
    uint16_t Range;          // Alcance da habilidade
    uint8_t  MaxLevel;       // Nível máximo da habilidade
    uint8_t  RequiredWeapon; // Arma necessária para usar a habilidade
    uint8_t  StatusEffects[8]; // Efeitos de status que podem ser aplicados
    uint8_t  ChainSkillID;   // ID da habilidade que pode ser encadeada
    uint8_t  ChainTimeWindow; // Janela de tempo para encadeamento (em ms)
    uint8_t  AnimationID;    // ID da animação associada
    uint8_t  SoundEffectID;  // ID do efeito sonoro associado
    uint8_t  ParticleEffectID; // ID do efeito de partículas associado
    uint8_t  ExtendedUnk[7]; // Bytes reservados para expansão futura
    /* Tamanho Total: 92 bytes */
} STRUCT_SKILLDATA_EXTENDED;

#pragma pack(pop)

#endif /* _SKILL_STRUCTURES_H_ */
