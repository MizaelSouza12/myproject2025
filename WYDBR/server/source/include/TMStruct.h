/**
 * @file TMStruct.h
 * @brief Definições das estruturas originais do WYD
 * 
 * Este arquivo contém as definições exatas das estruturas do WYD original,
 * mantendo a compatibilidade binária perfeita para serialização e desserialização.
 * 
 * Estas estruturas devem ser mantidas EXATAMENTE como no cliente original,
 * incluindo tipos, tamanhos, ordem dos campos e alinhamento.
 */

#ifndef _TMSTRUCT_H_
#define _TMSTRUCT_H_

/**
 * Estrutura para efeito/bônus em itens (2 bytes)
 */
struct STRUCT_BONUSEFFECT
{
    char cEffect;  // Tipo do efeito
    char cValue;   // Valor do efeito
};

/**
 * Estrutura para itens (armas, armaduras, consumíveis, etc.) (8 bytes)
 */
struct STRUCT_ITEM
{
    short sIndex;                         // ID do item
    STRUCT_BONUSEFFECT stEffect[3];       // Efeitos aplicados ao item
};

/**
 * Estrutura para efeito/buff em personagem (8 bytes)
 */
struct STRUCT_AFFECT
{
    char Type;      // Tipo do efeito
    char Level;     // Nível do efeito
    short Value;    // Valor do efeito
    int Time;       // Duração do efeito
};

/**
 * Estrutura para posição no mapa (4 bytes)
 */
struct STRUCT_POSITION
{
    short X;        // Coordenada X
    short Y;        // Coordenada Y
};

/**
 * Estrutura para dados de habilidades (32 bytes)
 * 
 * Esta estrutura representa os dados fundamentais de uma habilidade no WYD.
 * É utilizada no carregamento das habilidades a partir do arquivo Skill.bin.
 * 
 * Importante: Esta estrutura DEVE ter exatamente 32 bytes para manter
 * compatibilidade binária perfeita com o cliente original.
 */
#pragma pack(push, 1)
struct STRUCT_SKILLDATA
{
    short sIndex;          // 0-1    : Índice da skill
    short sSkillPoint;     // 2-3    : Pontos necessários para a skill
    unsigned char sType;   // 4      : Tipo de skill (passiva, ativa, buff, etc.)
    unsigned char sTarget; // 5      : Alvo (self, inimigo único, área, etc.)
    unsigned char sRange;  // 6      : Alcance da skill
    unsigned char sState;  // 7      : Estado necessário para usar (em pé, sentado, etc.)
    unsigned char sLevel;  // 8      : Nível da skill
    unsigned char sUnk1;   // 9      : Desconhecido
    unsigned char sUnk2;   // 10     : Desconhecido
    unsigned char sUnk3;   // 11     : Desconhecido 
    short sEfeito1;        // 12-13  : Efeito 1 (dano, cura, etc.)
    short sEfeito2;        // 14-15  : Efeito 2
    short sEfeito3;        // 16-17  : Efeito 3
    unsigned char sManaCost; // 18     : Custo de mana
    unsigned char sStamCost; // 19     : Custo de stamina
    short sDuration;       // 20-21  : Duração do efeito
    short sCooldown;       // 22-23  : Tempo de espera para usar novamente
    unsigned char sUnk4;   // 24     : Desconhecido
    unsigned char sUnk5;   // 25     : Desconhecido
    short sUnk6;           // 26-27  : Desconhecido
    int   iReserved;       // 28-31  : Reservado
};
#pragma pack(pop)

#endif // _TMSTRUCT_H_