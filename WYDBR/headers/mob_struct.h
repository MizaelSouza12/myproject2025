/**
 * mob_struct.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file headers/mob_struct.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef MOB_STRUCT_H
#define MOB_STRUCT_H

/**
 * Definição da estrutura de Mob/Character do WYD com base na engenharia reversa
 * 
 * Esta estrutura é baseada na análise preliminar do protocolo e arquivos binários
 * Tamanho total estimado: 368 bytes
 */

#include <stdint.h>
#include "item_struct.h"

#pragma pack(push, 1)  // Importante: garante que não há padding entre os campos

namespace wydbr {


/**
 * Estrutura principal de um Mob/Character
 * 
 * Esta é uma definição preliminar e será refinada conforme a
 * engenharia reversa progride.
 */
typedef struct _STRUCT_MOB {
    // Informações básicas de identificação
    char        Name[16];           // Nome do personagem/mob
    uint16_t    MobId;              // ID único do mob/npc (0 para players)
    uint8_t     Merchant;           // Flag para NPCs mercantes
    uint8_t     Class;              // Classe do personagem
    uint8_t     AffectInfo[32];     // Efeitos ativos no personagem
    
    // Atributos de level e experiência
    uint16_t    Level;              // Nível do personagem/mob
    uint64_t    Experience;         // Experiência atual
    uint32_t    NextLevel;          // Experiência para o próximo nível
    
    // Atributos Principais
    uint16_t    Strength;           // Força
    uint16_t    Dexterity;          // Destreza
    uint16_t    Intelligence;       // Inteligência
    uint16_t    Constitution;       // Constituição
    uint16_t    StatPoint;          // Pontos de atributo não distribuídos
    uint16_t    SkillPoint;         // Pontos de habilidade não distribuídos
    
    // Atributos Secundários derivados
    uint32_t    MaxHP;              // HP Máximo
    uint32_t    HP;                 // HP Atual
    uint32_t    MaxMP;              // MP Máximo
    uint32_t    MP;                 // MP Atual
    
    // Atributos Ofensivos/Defensivos
    uint16_t    Attack;             // Poder de Ataque
    uint16_t    Defense;            // Defesa
    uint16_t    Resistance;         // Resistência Mágica
    uint16_t    Accuracy;           // Precisão
    uint16_t    Dodge;              // Esquiva
    uint16_t    Critical;           // Chance de crítico
    
    // Posicionamento
    uint16_t    PosX;               // Posição X no mapa
    uint16_t    PosY;               // Posição Y no mapa
    uint8_t     Direction;          // Direção (0-7, 8 direções)
    uint16_t    MapId;              // ID do mapa atual
    
    // Habilidades e Skills
    uint8_t     SkillBar[16];       // Barra de habilidades equipadas
    uint8_t     LearnedSkill[64];   // Habilidades aprendidas (bitmap)
    
    // Equipamentos e Inventário
    uint32_t    Gold;               // Ouro carregado
    STRUCT_ITEM Equip[16];          // Itens equipados (slots específicos)
    STRUCT_ITEM Inventory[64];      // Inventário principal
    
    // Informações de Guild
    uint32_t    GuildId;            // ID da guild (0 se não tiver)
    uint8_t     GuildLevel;         // Nível na guild
    char        GuildName[16];      // Nome da guild
    
    // Informações sociais
    uint8_t     CapeInfo;           // Informação sobre a capa (facção)
    uint8_t     QuestInfo[32];      // Informações de quests ativas/completas
    
    // Atributos específicos de jogadores
    uint32_t    LastLogon;          // Timestamp do último login
    uint32_t    LastLogout;         // Timestamp do último logout
    uint32_t    CreateTime;         // Timestamp de criação do personagem
    uint8_t     AccountStatus;      // Status da conta (normal, GM, etc)
    uint8_t     PKStatus;           // Status de PK (assassino, etc)
    uint16_t    PKCount;            // Contador de PKs
    uint16_t    DeathCount;         // Contador de mortes
    
} STRUCT_MOB;

// Definição de classes (classes de personagem)
enum MOB_CLASS {
    MOB_CLASS_TRANSKNIGHT = 0,     // TK
    MOB_CLASS_FOEMA       = 1,     // FM
    MOB_CLASS_BEASTMASTER = 2,     // BM
    MOB_CLASS_HUNTRESS    = 3,     // HT
    MOB_CLASS_NPC         = 4,     // NPC
    MOB_CLASS_MONSTER     = 5      // Monstro comum
};

// Status de PK
enum PK_STATUS {
    PK_STATUS_NORMAL  = 0,         // Normal
    PK_STATUS_PK      = 1,         // PK (matou outros jogadores)
    PK_STATUS_CRIMINAL = 2         // Criminal (PKs excessivos)
};

// Status de Conta
enum ACCOUNT_STATUS {
    ACCOUNT_STATUS_NORMAL = 0,     // Jogador normal
    ACCOUNT_STATUS_GAMEMASTER = 1, // GM
    ACCOUNT_STATUS_DEVELOPER  = 2, // Desenvolvedor
    ACCOUNT_STATUS_ADMIN = 3       // Administrador
};

#pragma pack(pop)  // Restaura o alinhamento original

#endif // MOB_STRUCT_H

} // namespace wydbr
