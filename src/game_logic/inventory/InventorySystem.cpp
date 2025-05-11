#include "GameLogic/inventory/InventorySystem.h"
#include "GameLogic/character/CharacterSystem.h"
#include <algorithm>

namespace WYDBR {

InventorySystem& InventorySystem::GetInstance() {
    static InventorySystem instance;
    return instance;
}

void InventorySystem::Initialize() {
    items.clear();
    inventories.clear();
}

void InventorySystem::Shutdown() {
    items.clear();
    inventories.clear();
}

void InventorySystem::RegisterItem(const Item& item) {
    items[item.id] = item;
}

const Item* InventorySystem::GetItem(uint32_t itemId) const {
    auto it = items.find(itemId);
    return it != items.end() ? &it->second : nullptr;
}

bool InventorySystem::AddItem(uint32_t characterId, uint32_t itemId, uint32_t quantity) {
    // Verificar se o item existe
    const Item* item = GetItem(itemId);
    if (!item) {
        return false;
    }
    
    // Verificar se o personagem existe
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    if (!character) {
        return false;
    }
    
    // Verificar nível requerido
    if (character->Level < item->level) {
        return false;
    }
    
    // Verificar peso
    uint32_t newWeight = GetTotalWeight(characterId) + (item->weight * quantity);
    if (newWeight > GetMaxWeight(characterId)) {
        return false;
    }
    
    // Adicionar item
    auto& inventory = inventories[characterId];
    auto it = inventory.find(itemId);
    
    if (it != inventory.end()) {
        // Item já existe, aumentar quantidade
        it->second.quantity += quantity;
    } else {
        // Novo item
        InventorySlot slot;
        slot.itemId = itemId;
        slot.quantity = quantity;
        slot.durability = item->durability;
        slot.isEquipped = false;
        inventory[itemId] = slot;
    }
    
    return true;
}

bool InventorySystem::RemoveItem(uint32_t characterId, uint32_t itemId, uint32_t quantity) {
    auto it = inventories.find(characterId);
    if (it == inventories.end()) {
        return false;
    }
    
    auto itemIt = it->second.find(itemId);
    if (itemIt == it->second.end()) {
        return false;
    }
    
    // Verificar quantidade
    if (itemIt->second.quantity < quantity) {
        return false;
    }
    
    // Remover item
    itemIt->second.quantity -= quantity;
    
    // Se quantidade chegou a zero, remover slot
    if (itemIt->second.quantity == 0) {
        it->second.erase(itemIt);
    }
    
    return true;
}

uint32_t InventorySystem::GetItemQuantity(uint32_t characterId, uint32_t itemId) const {
    auto it = inventories.find(characterId);
    if (it == inventories.end()) {
        return 0;
    }
    
    auto itemIt = it->second.find(itemId);
    return itemIt != it->second.end() ? itemIt->second.quantity : 0;
}

const InventorySlot* InventorySystem::GetItemSlot(uint32_t characterId, uint32_t itemId) const {
    auto it = inventories.find(characterId);
    if (it == inventories.end()) {
        return nullptr;
    }
    
    auto itemIt = it->second.find(itemId);
    return itemIt != it->second.end() ? &itemIt->second : nullptr;
}

bool InventorySystem::EquipItem(uint32_t characterId, uint32_t itemId) {
    // Verificar se o item existe
    const Item* item = GetItem(itemId);
    if (!item) {
        return false;
    }
    
    // Verificar se o personagem tem o item
    const InventorySlot* slot = GetItemSlot(characterId, itemId);
    if (!slot) {
        return false;
    }
    
    // Verificar se já está equipado
    if (slot->isEquipped) {
        return false;
    }
    
    // Verificar tipo do item
    if (item->type != ITEM_TYPE_WEAPON && item->type != ITEM_TYPE_ARMOR) {
        return false;
    }
    
    // Verificar classe do item
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    if (character->Class != item->itemClass) {
        return false;
    }
    
    // Equipar item
    inventories[characterId][itemId].isEquipped = true;
    
    return true;
}

bool InventorySystem::UnequipItem(uint32_t characterId, uint32_t itemId) {
    // Verificar se o item existe
    const Item* item = GetItem(itemId);
    if (!item) {
        return false;
    }
    
    // Verificar se o personagem tem o item
    const InventorySlot* slot = GetItemSlot(characterId, itemId);
    if (!slot) {
        return false;
    }
    
    // Verificar se está equipado
    if (!slot->isEquipped) {
        return false;
    }
    
    // Desequipar item
    inventories[characterId][itemId].isEquipped = false;
    
    return true;
}

bool InventorySystem::IsItemEquipped(uint32_t characterId, uint32_t itemId) const {
    const InventorySlot* slot = GetItemSlot(characterId, itemId);
    return slot ? slot->isEquipped : false;
}

std::vector<const Item*> InventorySystem::GetEquippedItems(uint32_t characterId) const {
    std::vector<const Item*> equippedItems;
    
    auto it = inventories.find(characterId);
    if (it == inventories.end()) {
        return equippedItems;
    }
    
    for (const auto& slot : it->second) {
        if (slot.second.isEquipped) {
            const Item* item = GetItem(slot.first);
            if (item) {
                equippedItems.push_back(item);
            }
        }
    }
    
    return equippedItems;
}

std::vector<const Item*> InventorySystem::GetInventoryItems(uint32_t characterId) const {
    std::vector<const Item*> inventoryItems;
    
    auto it = inventories.find(characterId);
    if (it == inventories.end()) {
        return inventoryItems;
    }
    
    for (const auto& slot : it->second) {
        const Item* item = GetItem(slot.first);
        if (item) {
            inventoryItems.push_back(item);
        }
    }
    
    return inventoryItems;
}

uint32_t InventorySystem::GetTotalWeight(uint32_t characterId) const {
    uint32_t totalWeight = 0;
    
    auto it = inventories.find(characterId);
    if (it == inventories.end()) {
        return totalWeight;
    }
    
    for (const auto& slot : it->second) {
        const Item* item = GetItem(slot.first);
        if (item) {
            totalWeight += item->weight * slot.second.quantity;
        }
    }
    
    return totalWeight;
}

uint32_t InventorySystem::GetMaxWeight(uint32_t characterId) const {
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    if (!character) {
        return 0;
    }
    
    // Peso base + bônus de força
    return BASE_MAX_WEIGHT + (character->Strength * 10);
}

bool InventorySystem::IsInventoryFull(uint32_t characterId) const {
    auto it = inventories.find(characterId);
    if (it == inventories.end()) {
        return false;
    }
    
    return it->second.size() >= MAX_INVENTORY_SLOTS;
}

bool InventorySystem::IsInventoryOverweight(uint32_t characterId) const {
    return GetTotalWeight(characterId) > GetMaxWeight(characterId);
}

bool InventorySystem::UpdateItemDurability(uint32_t characterId, uint32_t itemId, int32_t amount) {
    // Verificar se o item existe
    const Item* item = GetItem(itemId);
    if (!item) {
        return false;
    }
    
    // Verificar se o personagem tem o item
    auto it = inventories.find(characterId);
    if (it == inventories.end()) {
        return false;
    }
    
    auto itemIt = it->second.find(itemId);
    if (itemIt == it->second.end()) {
        return false;
    }
    
    // Atualizar durabilidade
    int32_t newDurability = static_cast<int32_t>(itemIt->second.durability) + amount;
    newDurability = std::max(0, std::min(newDurability, static_cast<int32_t>(item->maxDurability)));
    
    itemIt->second.durability = static_cast<uint32_t>(newDurability);
    
    // Se durabilidade chegou a zero, remover item
    if (itemIt->second.durability == 0) {
        it->second.erase(itemIt);
    }
    
    return true;
}

} // namespace WYDBR 