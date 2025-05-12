/**
 * TM_Mob.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/TM_Mob.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _TM_MOB_H_
#define _TM_MOB_H_

/**
 * @file TM_Mob.h
 * @brief Estruturas de personagem do WYD
 * 
 * Este arquivo contém as estruturas de dados de personagens utilizadas no WYD,
 * mantendo compatibilidade binária exata com o cliente original.
 * 
 * Nota: Para verificação de compatibilidade binária, os tipos exatos do WYD
 * são importados de TMStruct.h. As estruturas aqui servem para uso interno
 * e garantem interoperabilidade com o resto do código.
 */

#include "GlobalDef.h"
#include "TM_Item.h"
#include "TMStruct.h" // Importa as estruturas originais do WYD

namespace wydbr {

// Estruturas para compatibilidade binária
#pragma pack(push, 1)

/**
 * @brief Estrutura de Skills do personagem
 */
struct STRUCT_SKILLBARS
{
    uint16_t SkillBar1[4];
    uint16_t SkillBar2[16];
};

/**
 * @brief Estrutura completa de personagem do WYD
 * 
 * Mantém compatibilidade binária exata com o cliente original.
 */
struct STRUCT_MOB
{
    char    MobName[NAME_LENGTH];
    char    Clan;
    char    Merchant;
    uint16_t Guild;
    uint8_t GuildLevel;
    
    uint8_t Class;             // 0-TK, 1-FM, 2-BM, 3-HT
    uint16_t AffectFlag;       // Efeitos que afetam o personagem
    uint32_t QuestFlag;        // Estado atual das quests
    
    // Status e atributos
    STRUCT_SCORE BaseScore;    // Atributos base
    STRUCT_SCORE CurrentScore; // Atributos com equipamentos
    
    // Equipamentos, inventário e armazenamento
    STRUCT_ITEM  Equip[MAX_EQUIP];             // Equipamentos
    STRUCT_ITEM  Inventory[MAX_INVENTORY];      // Inventário
    
    // Posição
    uint16_t TargetX;          // Posição X
    uint16_t TargetY;          // Posição Y
    
    // Habilidades
    STRUCT_SKILLBARS SkillBar;
    uint8_t SkillPoint;
    
    // Outros dados
    uint8_t Critical;
    uint8_t SaveMana;
    
    // Afecções
    STRUCT_AFFECT Affect[MAX_AFFECT];
    char Party;
    char GuildMemberType;
    
    // Tempo online
    int32_t  ScoreBonus;
    uint32_t LastNT;
    uint16_t  ManaSave;
    int32_t  SPX;
    int32_t  SPY;
    
    // Gold
    uint64_t Gold;
    
    // Exp e level
    uint64_t Exp;
    
    /**
     * @brief Limpa os dados do personagem (define como novo)
     */
    void Clear() {
        memset(this, 0, sizeof(STRUCT_MOB));
    }
    
    /**
     * @brief Obtém o nome da classe
     * @return Nome da classe
     */
    const char* GetClassName() const {
        static const char* ClassNames[] = {"TransKnight", "Foema", "Beastmaster", "Huntress"};
        if (Class >= 0 && Class <= 3) {
            return ClassNames[Class];
        }
        return "Unknown";
    }
    
    /**
     * @brief Verifica se o personagem é GM
     * @return true se GM, false caso contrário
     */
    bool IsGM() const {
        if (GuildLevel == 255) return true;
        return false;
    }
    
    /**
     * @brief Obtém o atributo mais alto do personagem
     * @return Índice do atributo mais alto (0-STR, 1-INT, 2-DEX, 3-CON)
     */
    int32_t GetMaxStat() const {
        uint16_t Stats[4] = {
            BaseScore.Str, 
            BaseScore.Int, 
            BaseScore.Dex, 
            BaseScore.Con
        };
        
        int32_t MaxIndex = 0;
        for (int32_t i = 1; i < 4; i++) {
            if (Stats[i] > Stats[MaxIndex]) {
                MaxIndex = i;
            }
        }
        
        return MaxIndex;
    }
    
    /**
     * @brief Calcula o total de pontos utilizados em atributos
     * @return Total de pontos gastos
     */
    int32_t GetTotalStat() const {
        return BaseScore.Str + BaseScore.Int + BaseScore.Dex + BaseScore.Con;
    }
    
    /**
     * @brief Obtém o número de afecções ativas
     * @return Número de afecções ativas
     */
    int32_t GetAffectCount() const {
        int32_t Count = 0;
        for (int32_t i = 0; i < MAX_AFFECT; i++) {
            if (Affect[i].Type != 0) Count++;
        }
        return Count;
    }
    
    /**
     * @brief Obtém o número de itens no inventário
     * @return Número de itens no inventário
     */
    int32_t GetInventoryItemCount() const {
        int32_t Count = 0;
        for (int32_t i = 0; i < MAX_INVENTORY; i++) {
            if (Inventory[i].sIndex != 0) Count++;
        }
        return Count;
    }
    
    /**
     * @brief Verifica se o personagem tem algum equipamento
     * @return true se tiver algum equipamento, false caso contrário
     */
    bool HasEquipment() const {
        for (int32_t i = 0; i < MAX_EQUIP; i++) {
            if (Equip[i].sIndex != 0) return true;
        }
        return false;
    }
    
    /**
     * @brief Procura um slot vazio no inventário
     * @return Índice do slot vazio (-1 se não houver slot vazio)
     */
    int32_t FindEmptyInventorySlot() const {
        for (int32_t i = 0; i < MAX_INVENTORY; i++) {
            if (Inventory[i].sIndex == 0) return i;
        }
        return -1;
    }
    
    /**
     * @brief Verifica se o personagem tem espaço no inventário
     * @return true se tiver espaço, false caso contrário
     */
    bool HasInventorySpace() const {
        return FindEmptyInventorySlot() != -1;
    }
    
    /**
     * @brief Procura um item específico no inventário
     * @param ItemID ID do item a procurar
     * @return Índice do item (-1 se não encontrado)
     */
    int32_t FindItemInInventory(uint16_t ItemID) const {
        for (int32_t i = 0; i < MAX_INVENTORY; i++) {
            if (Inventory[i].sIndex == ItemID) return i;
        }
        return -1;
    }
    
    /**
     * @brief Conta a quantidade de um item específico no inventário
     * @param ItemID ID do item a contar
     * @return Quantidade total do item
     */
    int32_t CountItemInInventory(uint16_t ItemID) const {
        int32_t Count = 0;
        for (int32_t i = 0; i < MAX_INVENTORY; i++) {
            if (Inventory[i].sIndex == ItemID) Count += Inventory[i].sValue;
        }
        return Count;
    }
};

/**
 * @brief Estrutura de personagem no MobList.bin
 * 
 * Contém dados completos de um personagem ou NPC na lista do jogo.
 */
struct STRUCT_MOBLIST
{
    char    MobName[NAME_LENGTH];
    uint16_t   Unk1;
    uint16_t   Unk2;
    uint16_t   LevelOrLifeNPC;
    
    STRUCT_SCORE BaseScore;
    STRUCT_SCORE CurrentScore;
    
    STRUCT_ITEM  Equip[MAX_EQUIP];
    
    uint32_t Merchant;   // ID da lista de itens de venda (NPCs comerciantes)
    uint16_t Unk41;
    uint16_t SkillBar1[4];
    
    char   Unk3[36];
    
    uint16_t   Guild;
    uint8_t    ClassInfo;
    char   Unk4[3];
    
    uint16_t      Unk5;
    char   Unk6[14];
    
    uint32_t AiType;
    
    uint32_t Unk7;
    
    uint32_t QuestFlag;
    
    uint8_t  Unk8[4];
    
    uint32_t GuildLevel;
    
    uint8_t  Unk9[96];
};

/**
 * @brief Estrutura de interação entre personagens (Chat, Grupo, Comércio etc.)
 */
struct STRUCT_INTERACTION
{
    STRUCT_MOB Mob;
    char       Unk1[12]; // Valores desconhecidos
    uint16_t      Unk2;
    char       MobName[NAME_LENGTH];
    uint32_t      ClasseMaster;
    uint32_t      AffectFlag;
    uint32_t      AffectTick;
    STRUCT_AFFECT Affect[32];
    STRUCT_ITEM   Storage[128];
    uint16_t      Unk3;
    uint16_t      Unk4;
};

/**
 * @brief Estrutura de estatísticas de combate
 */
struct STRUCT_BATTLE_STATS
{
    uint32_t TotalBattles;       // Total de batalhas
    uint32_t TotalVictories;     // Total de vitórias
    uint32_t TotalDefeats;       // Total de derrotas
    uint32_t TotalDraws;         // Total de empates
    uint32_t TotalKills;         // Total de jogadores mortos
    uint32_t TotalDeaths;        // Total de mortes
    uint32_t CurrentWinStreak;   // Sequência atual de vitórias
    uint32_t MaxWinStreak;       // Maior sequência de vitórias
    uint32_t CurrentLoseStreak;  // Sequência atual de derrotas
    uint32_t MaxLoseStreak;      // Maior sequência de derrotas
    uint32_t LastBattleTime;     // Timestamp da última batalha
    uint32_t LastBattleId;       // ID da última batalha
    
    /**
     * @brief Limpa as estatísticas
     */
    void Clear() {
        memset(this, 0, sizeof(STRUCT_BATTLE_STATS));
    }
};

#pragma pack(pop)

} // namespace wydbr

#endif // _TM_MOB_H_