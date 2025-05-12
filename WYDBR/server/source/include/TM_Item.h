/**
 * TM_Item.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/TM_Item.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _TM_ITEM_H_
#define _TM_ITEM_H_

/**
 * @file TM_Item.h
 * @brief Estruturas de itens do WYD
 * 
 * Este arquivo contém as estruturas de dados de itens utilizadas no WYD,
 * mantendo compatibilidade binária exata com o cliente original.
 * 
 * Nota: Para verificação de compatibilidade binária, os tipos exatos do WYD
 * são importados de TMStruct.h. As estruturas aqui servem para uso interno
 * e garantem interoperabilidade com o resto do código.
 */

#include "GlobalDef.h"
#include "TMStruct.h" // Importa as estruturas originais do WYD

namespace wydbr {

// Estruturas de itens para compatibilidade binária
#pragma pack(push, 1)

/**
 * @brief Estrutura de efeito de item
 */
struct STRUCT_ITEMEFFECT
{
    uint8_t cEffect;
    uint8_t cValue;
};

/**
 * @brief Estrutura de requisitos de item
 */
struct STRUCT_ITEMREQUIRE
{
    uint8_t cClass;    // Classes que podem usar: 0=Todos, 1=TK, 2=FM, 4=BM, 8=HT
    uint8_t cLevel;    // Nível mínimo para uso
    uint16_t wStats;   // 1=Str, 2=Int, 4=Dex, 8=Con
};

/**
 * @brief Estrutura completa de item do WYD
 * 
 * Mantém compatibilidade binária exata com o cliente original.
 */
struct STRUCT_ITEM
{
    uint16_t sIndex;               // ID do item
    uint8_t stEffect[3][2];        // Efeitos aplicados ao item
    uint16_t sValue;               // Valor/quantidade/durabilidade
  
    /**
     * @brief Retorna o nível de refinamento do item
     * @return Nível de refinamento (0 a 9)
     */
    uint8_t GetItemLevel() const {
        return (uint8_t)((stEffect[0][0]) + ((stEffect[0][1]) << 8));
    }

    /**
     * @brief Define o nível de refinamento do item
     * @param level Nível de refinamento (0 a 9)
     */
    void SetItemLevel(uint8_t level) {
        if (level > 9) level = 9;
        stEffect[0][0] = level;
        stEffect[0][1] = 0;
    }

    /**
     * @brief Verifica se o item está montado (cavalo/etc)
     * @return true se montado, false caso contrário
     */
    bool IsMountItem() const {
        uint16_t index = sIndex;
        return (index >= 2360 && index < 2390) || (index >= 3980 && index < 3995);
    }

    /**
     * @brief Verifica se o item é um pergaminho de aprendizado
     * @return true se for pergaminho, false caso contrário
     */
    bool IsSkillBook() const {
        if (sIndex >= 5400 && sIndex < 6500) return true;
        return false;
    }

    /**
     * @brief Verifica se o item é um suprimento para criação
     * @return true se for suprimento, false caso contrário
     */
    bool IsCreationSupply() const {
        uint16_t index = sIndex;
        if (index >= 4100 && index < 4200) return true;
        return false;
    }

    /**
     * @brief Verifica se o item está vazio (nenhum item)
     * @return true se vazio, false caso contrário
     */
    bool IsEmpty() const {
        if (sIndex == 0) return true;
        return false;
    }

    /**
     * @brief Verifica se o item é equipável
     * @return true se equipável, false caso contrário
     */
    bool IsEquipable() const {
        uint16_t index = sIndex;
        if (index >= 0 && index < 6500) {
            if (index >= 2330 && index < 2390) return true;
            if (index >= 3200 && index < 3300) return true;
            if (index >= 3980 && index < 3995) return true;
            if (index >= 2000 && index < 2300) return true;
            if (index >= 2390 && index < 2400) return true;
            if (index >= 2400 && index < 2500) return true;
            if (index >= 2500 && index < 2550) return true;
            if (index >= 2800 && index < 3000) return true;
        }
        return false;
    }

    /**
     * @brief Verifica se o item é uma arma
     * @return true se for arma, false caso contrário
     */
    bool IsWeapon() const {
        if (sIndex >= 0 && sIndex < 6500) {
            if (sIndex >= 2000 && sIndex < 2300) return true;
            if (sIndex >= 2390 && sIndex < 2400) return true;
        }
        return false;
    }

    /**
     * @brief Verifica se o item é uma armadura
     * @return true se for armadura, false caso contrário
     */
    bool IsArmor() const {
        if (sIndex >= 2400 && sIndex < 2500) return true;
        return false;
    }

    /**
     * @brief Verifica se o item pode ser vendido
     * @return true se pode ser vendido, false caso contrário
     */
    bool IsSellable() const {
        if (sIndex >= 4010 && sIndex <= 4015) return false;
        if (sIndex >= 4026 && sIndex <= 4029) return false;
        if (sIndex == 4049) return false;
        if (sIndex >= 4041 && sIndex <= 4043) return false;
        if (sIndex == 3997) return false;
        if (sIndex == 3998) return false;
        return true;
    }

    /**
     * @brief Verifica se o item pode ser trocado entre jogadores
     * @return true se pode ser trocado, false caso contrário
     */
    bool IsExchangeable() const {
        if (sIndex >= 4010 && sIndex <= 4015) return false;
        if (sIndex >= 4026 && sIndex <= 4029) return false;
        if (sIndex == 4049) return false;
        if (sIndex >= 4041 && sIndex <= 4043) return false;
        if (sIndex == 3997) return false;
        if (sIndex == 3998) return false;
        return true;
    }

    /**
     * @brief Verifica se o item pode ser armazenado no banco
     * @return true se pode ser armazenado, false caso contrário
     */
    bool IsStorable() const {
        if (sIndex >= 4010 && sIndex <= 4015) return false;
        if (sIndex >= 4026 && sIndex <= 4029) return false;
        if (sIndex == 4049) return false;
        if (sIndex >= 4041 && sIndex <= 4043) return false;
        if (sIndex == 3997) return false;
        if (sIndex == 3998) return false;
        if (sIndex >= 4052 && sIndex <= 4055) return false;
        return true;
    }

    /**
     * @brief Verifica se o item pode ser dropado no chão
     * @return true se pode ser dropado, false caso contrário
     */
    bool IsDropable() const {
        if (sIndex >= 4010 && sIndex <= 4015) return false;
        if (sIndex >= 4026 && sIndex <= 4029) return false;
        if (sIndex == 4049) return false;
        if (sIndex >= 4041 && sIndex <= 4043) return false;
        if (sIndex == 3997) return false;
        if (sIndex == 3998) return false;
        if (sIndex >= 4052 && sIndex <= 4055) return false;
        return true;
    }

    /**
     * @brief Limpa os dados do item (define como vazio)
     */
    void Clear() {
        sIndex = 0;
        memset(stEffect, 0, sizeof(stEffect));
        sValue = 0;
    }
};

/**
 * @brief Estrutura de Item no ItemList.bin
 * 
 * Contém dados completos de um item na lista de itens do jogo.
 */
struct STRUCT_ITEMLIST
{
    char Name[64];
    uint16_t IndexMesh;
    uint16_t IndexTexture;
    uint16_t IndexVisualEffect;
    uint16_t Level;
    uint16_t Str;
    uint16_t Int;
    uint16_t Dex;
    uint16_t Con;
    STRUCT_ITEMEFFECT stEffect[12];
    STRUCT_ITEMREQUIRE stRequire;
    int32_t Price;
    uint16_t nUnique;
    int16_t ClassFlag;
    uint8_t stSell;
    char Description[256];
};

/**
 * @brief Estrutura de banco de itens do jogador
 */
struct STRUCT_STORAGE
{
    uint64_t Gold;
    STRUCT_ITEM Item[128];
    char Name[16];
};

#pragma pack(pop)

} // namespace wydbr

#endif // _TM_ITEM_H_