#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace WYD {
    class SistemaInventario {
    private:
        struct InventorySlot {
            ItemData item;
            bool isEquipped;
            DWORD quantity;
        };

        struct Inventory {
            static const size_t MAX_SLOTS = 50;
            std::vector<InventorySlot> slots;
            DWORD gold;
        };

        std::unordered_map<DWORD, Inventory> inventories;

    public:
        SistemaInventario() = default;
        ~SistemaInventario() = default;

        // Inicialização
        void initializeInventory(DWORD characterId) {
            Inventory inv;
            inv.slots.resize(MAX_SLOTS);
            inv.gold = 0;
            inventories[characterId] = inv;
        }

        // Gerenciamento de itens
        bool addItem(DWORD characterId, const ItemData& item, DWORD quantity = 1) {
            auto& inv = inventories[characterId];
            
            // Procurar slot vazio ou com mesmo item
            for (size_t i = 0; i < MAX_SLOTS; i++) {
                if (inv.slots[i].item.id == 0) {
                    // Slot vazio encontrado
                    inv.slots[i].item = item;
                    inv.slots[i].quantity = quantity;
                    inv.slots[i].isEquipped = false;
                    return true;
                }
                else if (inv.slots[i].item.id == item.id && 
                         inv.slots[i].item.type == item.type) {
                    // Mesmo item encontrado, incrementar quantidade
                    inv.slots[i].quantity += quantity;
                    return true;
                }
            }
            return false; // Inventário cheio
        }

        bool removeItem(DWORD characterId, size_t slotIndex, DWORD quantity = 1) {
            auto& inv = inventories[characterId];
            if (slotIndex >= MAX_SLOTS) return false;

            auto& slot = inv.slots[slotIndex];
            if (slot.item.id == 0) return false;

            if (slot.quantity <= quantity) {
                // Remover item completamente
                slot.item = ItemData{};
                slot.quantity = 0;
                slot.isEquipped = false;
            }
            else {
                // Reduzir quantidade
                slot.quantity -= quantity;
            }
            return true;
        }

        // Sistema de equipamento
        bool equipItem(DWORD characterId, size_t slotIndex) {
            auto& inv = inventories[characterId];
            if (slotIndex >= MAX_SLOTS) return false;

            auto& slot = inv.slots[slotIndex];
            if (slot.item.id == 0) return false;

            // Verificar se o item pode ser equipado
            if (slot.item.type != ItemType::WEAPON && 
                slot.item.type != ItemType::ARMOR && 
                slot.item.type != ItemType::ACCESSORY) {
                return false;
            }

            // Desequipar item do mesmo tipo se existir
            for (size_t i = 0; i < MAX_SLOTS; i++) {
                if (inv.slots[i].isEquipped && 
                    inv.slots[i].item.type == slot.item.type) {
                    inv.slots[i].isEquipped = false;
                    break;
                }
            }

            slot.isEquipped = true;
            return true;
        }

        bool unequipItem(DWORD characterId, size_t slotIndex) {
            auto& inv = inventories[characterId];
            if (slotIndex >= MAX_SLOTS) return false;

            auto& slot = inv.slots[slotIndex];
            if (!slot.isEquipped) return false;

            slot.isEquipped = false;
            return true;
        }

        // Sistema de moeda
        bool addGold(DWORD characterId, DWORD amount) {
            auto& inv = inventories[characterId];
            inv.gold += amount;
            return true;
        }

        bool removeGold(DWORD characterId, DWORD amount) {
            auto& inv = inventories[characterId];
            if (inv.gold < amount) return false;
            inv.gold -= amount;
            return true;
        }

        // Sistema de troca
        bool tradeItems(DWORD fromCharacterId, DWORD toCharacterId, 
                       size_t fromSlot, size_t toSlot, DWORD quantity = 1) {
            auto& fromInv = inventories[fromCharacterId];
            auto& toInv = inventories[toCharacterId];

            if (fromSlot >= MAX_SLOTS || toSlot >= MAX_SLOTS) return false;

            auto& fromSlotData = fromInv.slots[fromSlot];
            auto& toSlotData = toInv.slots[toSlot];

            if (fromSlotData.item.id == 0 || fromSlotData.quantity < quantity) return false;

            // Verificar se o slot de destino está vazio ou pode receber mais do mesmo item
            if (toSlotData.item.id != 0 && 
                (toSlotData.item.id != fromSlotData.item.id || 
                 toSlotData.item.type != fromSlotData.item.type)) {
                return false;
            }

            // Realizar a troca
            if (toSlotData.item.id == 0) {
                toSlotData.item = fromSlotData.item;
                toSlotData.quantity = quantity;
            }
            else {
                toSlotData.quantity += quantity;
            }

            fromSlotData.quantity -= quantity;
            if (fromSlotData.quantity == 0) {
                fromSlotData.item = ItemData{};
                fromSlotData.isEquipped = false;
            }

            return true;
        }

        // Getters
        const Inventory& getInventory(DWORD characterId) const {
            return inventories.at(characterId);
        }

        DWORD getGold(DWORD characterId) const {
            return inventories.at(characterId).gold;
        }

        // Verificações
        bool isSlotEmpty(DWORD characterId, size_t slotIndex) const {
            const auto& inv = inventories.at(characterId);
            return slotIndex < MAX_SLOTS && inv.slots[slotIndex].item.id == 0;
        }

        bool isItemEquipped(DWORD characterId, size_t slotIndex) const {
            const auto& inv = inventories.at(characterId);
            return slotIndex < MAX_SLOTS && inv.slots[slotIndex].isEquipped;
        }

        size_t getItemCount(DWORD characterId, DWORD itemId) const {
            const auto& inv = inventories.at(characterId);
            DWORD count = 0;
            for (const auto& slot : inv.slots) {
                if (slot.item.id == itemId) {
                    count += slot.quantity;
                }
            }
            return count;
        }
    };
} 