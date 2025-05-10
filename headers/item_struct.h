/**
 * @file item_struct.h
 * @brief Definição das estruturas de itens do WYD
 * 
 * Este arquivo contém as declarações completas para itens
 * no sistema WYDBR, com correções de segurança e melhorias.
 */

#ifndef WYDBR_ITEM_STRUCT_H
#define WYDBR_ITEM_STRUCT_H

#include <cstdint>
#include <string>
#include "wyd_core.h"

namespace WYDBR {

// Tipo dos itens
enum class ItemType : uint8_t {
    Weapon = 0,
    Armor = 1,
    Helmet = 2,
    Shield = 3,
    Boots = 4,
    Gloves = 5,
    Ring = 6,
    Amulet = 7,
    Belt = 8,
    Earring = 9,
    Consumable = 10,
    Quest = 11,
    Material = 12,
    Gem = 13,
    Mount = 14,
    Special = 15
};

// Efeitos dos itens
enum class ItemEffect : uint16_t {
    None = 0,
    Str = 1,
    Dex = 2,
    Int = 3,
    Con = 4,
    Damage = 5,
    Defense = 6,
    HP = 7,
    MP = 8,
    MoveSpeed = 9,
    AttackSpeed = 10,
    HPRegen = 11,
    MPRegen = 12,
    Critical = 13,
    Resistance = 14,
    HPPercentage = 15,
    MPPercentage = 16,
    Experience = 17,
    Gold = 18,
    DropRate = 19,
    Immunity = 20
};

// Raridade dos itens (correção de segurança - antes era campo não inicializado)
enum class ItemRarity : uint8_t {
    Common = 0,
    Uncommon = 1,
    Rare = 2,
    Unique = 3,
    Legend = 4,
    Artifact = 5
};

// Estrutura completa de Item (com correções)
struct Item : public ItemBase {
    ItemType type = ItemType::Consumable;
    ItemRarity rarity = ItemRarity::Common;
    
    // Requisitos
    uint16_t reqStr = 0;
    uint16_t reqDex = 0;
    uint16_t reqInt = 0;
    uint16_t reqCon = 0;
    
    // Atributos
    int16_t bonusStr = 0;
    int16_t bonusDex = 0;
    int16_t bonusInt = 0;
    int16_t bonusCon = 0;
    
    // Status principais
    int16_t bonusHP = 0;
    int16_t bonusMP = 0;
    int16_t bonusDamage = 0;
    int16_t bonusDefense = 0;
    
    // Flags de segurança
    bool isTraded = false;      // Item foi negociado
    bool isBound = false;       // Item vinculado ao personagem
    bool isIdentified = true;   // Item foi identificado
    
    // Validações de segurança
    bool IsValid() const {
        // Verificar se ID é válido
        if (itemId == 0) return false;
        
        // Verificar se durabilidade é válida
        if (durability > maxDurability) return false;
        
        // Verificar se número de sockets é válido
        if (sockets > maxSockets) return false;
        
        return true;
    }
    
    // Calcular valor do item (segurança)
    uint32_t CalculateValue() const {
        uint32_t base = itemId * 100;
        uint32_t rarityMod = static_cast<uint32_t>(rarity) * 500;
        uint32_t gradeMod = grade * 250;
        uint32_t socketMod = sockets * 400;
        
        // Usar XOR para evitar overflow
        return (base ^ rarityMod) + (gradeMod ^ socketMod);
    }
    
    // Verificar checksum do item
    bool VerifyChecksum(uint32_t storedChecksum) const {
        return Utils::CalculateChecksum(this, sizeof(Item)) == storedChecksum;
    }
};

// Inventário com proteção contra buffer overflow
struct Inventory {
    static const int MAX_ITEMS = MAX_INVENTORY;
    
    Item items[MAX_ITEMS];
    bool occupied[MAX_ITEMS] = {false}; // Array para rastrear slots ocupados
    
    // Obter item com verificação de limites
    const Item* GetItem(int slot) const {
        if (slot < 0 || slot >= MAX_ITEMS || !occupied[slot]) {
            return nullptr;
        }
        return &items[slot];
    }
    
    // Adicionar item com verificação de segurança
    bool AddItem(const Item& item) {
        // Verificar se o item é válido
        if (!item.IsValid()) {
            return false;
        }
        
        // Encontrar slot vazio
        for (int i = 0; i < MAX_ITEMS; ++i) {
            if (!occupied[i]) {
                items[i] = item;
                occupied[i] = true;
                return true;
            }
        }
        
        return false; // Inventário cheio
    }
    
    // Remover item com verificação de segurança
    bool RemoveItem(int slot) {
        if (slot < 0 || slot >= MAX_ITEMS || !occupied[slot]) {
            return false;
        }
        
        // Limpar slot
        items[slot] = Item(); // Reset para valores padrão
        occupied[slot] = false;
        
        return true;
    }
    
    // Contar slots ocupados
    int CountOccupied() const {
        int count = 0;
        for (int i = 0; i < MAX_ITEMS; ++i) {
            if (occupied[i]) count++;
        }
        return count;
    }
};

} // namespace WYDBR

#endif // WYDBR_ITEM_STRUCT_H
