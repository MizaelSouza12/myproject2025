/**
 * SkillBinStructs.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/skill/SkillBinStructs.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _SKILL_BIN_STRUCTS_H_
#define _SKILL_BIN_STRUCTS_H_

/**
 * @file SkillBinStructs.h
 * @brief Estruturas de dados do arquivo Skill.bin
 * 
 * Este arquivo contém as estruturas de dados necessárias para
 * carregar e processar o arquivo Skill.bin do WYD.
 */

#include <stdint.h>
#include "../TMStruct.h"

namespace wydbr {
namespace skill {

/**
 * Cabeçalho do arquivo Skill.bin
 * 
 * Esta estrutura representa os primeiros bytes do arquivo Skill.bin,
 * incluindo assinatura, versão e contagem de habilidades.
 */
#pragma pack(push, 1)
struct BIN_HEADER {
    char Signature[4];          // "SKIL" ou similar
    uint32_t Version;           // Versão do arquivo
    uint32_t SkillCount;        // Número de habilidades
    uint32_t Reserved[5];       // Reservado para uso futuro
};
#pragma pack(pop)

/**
 * Entrada de habilidade no arquivo Skill.bin
 * 
 * Esta estrutura representa uma entrada no arquivo Skill.bin,
 * incluindo o ID da habilidade e os dados da habilidade.
 * 
 * Importante: O tamanho deve ser exatamente 34 bytes (2 + 32)
 */
#pragma pack(push, 1)
struct BIN_SKILL_ENTRY {
    uint16_t SkillID;           // ID da habilidade (2 bytes)
    STRUCT_SKILLDATA Data;      // Dados da habilidade (32 bytes)
};
#pragma pack(pop)

/**
 * Estrutura estendida de dados de habilidade
 * 
 * Esta estrutura estende a STRUCT_SKILLDATA incluindo campos
 * adicionais presentes no arquivo Skill.bin mas não diretamente
 * utilizados no processamento de habilidades.
 */
#pragma pack(push, 1)
struct SKILLDATA_EXTENDED : public STRUCT_SKILLDATA {
    char Name[32];              // Nome da habilidade
    char Description[256];      // Descrição da habilidade
    uint16_t IconID;            // ID do ícone
    uint16_t EffectID;          // ID do efeito visual
    uint16_t SoundID;           // ID do efeito sonoro
    uint8_t Class;              // Classe que pode usar a habilidade
    uint8_t Element;            // Elemento da habilidade
    uint16_t LearnLevel;        // Nível para aprender
    uint16_t MaxLevel;          // Nível máximo
    uint16_t UpgradeSkillID;    // ID da habilidade de upgrade
    uint16_t PrerequisiteSkill1; // Pré-requisito 1
    uint16_t PrerequisiteSkill2; // Pré-requisito 2
    uint8_t ReqSTR;             // Força necessária
    uint8_t ReqDEX;             // Destreza necessária
    uint8_t ReqINT;             // Inteligência necessária
    uint8_t ReqCON;             // Constituição necessária
    uint8_t Padding[26];        // Preenchimento para alinhamento
};
#pragma pack(pop)

// Verificação de tamanho em tempo de compilação
static_assert(sizeof(STRUCT_SKILLDATA) == 32, "STRUCT_SKILLDATA deve ter exatamente 32 bytes");
static_assert(sizeof(BIN_HEADER) == 32, "BIN_HEADER deve ter exatamente 32 bytes");
static_assert(sizeof(BIN_SKILL_ENTRY) == 34, "BIN_SKILL_ENTRY deve ter exatamente 34 bytes");
static_assert(sizeof(SKILLDATA_EXTENDED) == 380, "SKILLDATA_EXTENDED deve ter exatamente 380 bytes");

} // namespace skill
} // namespace wydbr

#endif // _SKILL_BIN_STRUCTS_H_