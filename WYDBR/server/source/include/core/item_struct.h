/**
 * item_struct.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/core/item_struct.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef ITEM_STRUCT_H
#define ITEM_STRUCT_H

/**
 * Definição da estrutura de Item do WYD com base na engenharia reversa
 * 
 * Esta estrutura é baseada na análise do arquivo ItemList.bin
 * Tamanho total: 72 bytes (conforme metadata em ItemList.json)
 */

#include <stdint.h>

#pragma pack(push, 1)  // Importante: garante que não há padding entre os campos

namespace wydbr {


/**
 * Estrutura principal de item do WYD
 * 
 * Esta é uma definição preliminar e será refinada conforme a 
 * engenharia reversa progride.
 */
typedef struct _STRUCT_ITEM {
    // Cabeçalho do item - informações básicas
    uint16_t Index;              // ID do item
    uint16_t ItemId;             // Subtipo do item
    uint8_t  Type;               // Tipo principal do item (arma, armadura, etc)
    uint8_t  SubType;            // Subtipo secundário
    uint8_t  Level;              // Nível base do item
    
    // Atributos básicos
    uint8_t  Refinement;         // Nível de refinamento (+0 a +9)
    uint8_t  Quality;            // Qualidade (normal, superior, raro, etc)
    uint8_t  Durability;         // Durabilidade atual
    uint8_t  MaxDurability;      // Durabilidade máxima
    
    // Atributos adicionais
    uint16_t Attack;             // Ataque (para armas)
    uint16_t Defense;            // Defesa (para armaduras)
    uint16_t ReqLevel;           // Nível requerido
    uint16_t ReqStr;             // Força requerida
    uint16_t ReqDex;             // Destreza requerida
    uint16_t ReqInt;             // Inteligência requerida
    uint16_t ReqCon;             // Constituição requerida
    
    // Efeitos especiais
    uint8_t  Effects[8];         // Efeitos especiais
    
    // Valores de socket/gema
    uint16_t Sockets[4];         // Slots para gemas/aprimoramentos
    
    // Tempo e flags
    uint32_t CreationTime;       // Timestamp de criação
    uint32_t ExpirationTime;     // Timestamp de expiração (0 = sem expiração)
    uint32_t Flags;              // Flags diversas (tradeable, sellable, etc)
    
    // Padding para completar 72 bytes
    uint8_t  Reserved[8];        // Bytes reservados para expansão futura
    
} STRUCT_ITEM;

// Definição de constantes para tipos de itens
enum ITEM_TYPE {
    ITEM_TYPE_WEAPON     = 0,    // Armas
    ITEM_TYPE_ARMOR      = 1,    // Armaduras
    ITEM_TYPE_CONSUMABLE = 2,    // Consumíveis
    ITEM_TYPE_MATERIAL   = 3,    // Materiais de crafting
    ITEM_TYPE_MISC       = 4,    // Itens diversos
    ITEM_TYPE_QUEST      = 5     // Itens de quest
};

// Definição de constantes para qualidade de itens
enum ITEM_QUALITY {
    ITEM_QUALITY_NORMAL      = 0,    // Normal (branco)
    ITEM_QUALITY_SUPERIOR    = 1,    // Superior (azul)
    ITEM_QUALITY_RARE        = 2,    // Raro (amarelo)
    ITEM_QUALITY_UNIQUE      = 3,    // Único (laranja)
    ITEM_QUALITY_SET         = 4,    // Set (verde)
    ITEM_QUALITY_LEGENDARY   = 5     // Lendário (roxo)
};

#pragma pack(pop)  // Restaura o alinhamento original

#endif // ITEM_STRUCT_H

} // namespace wydbr
