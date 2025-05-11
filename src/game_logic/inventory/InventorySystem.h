#pragma once

#include <unordered_map>
#include <vector>
#include <cstdint>

namespace WYDBR {

// Tipos de itens
enum ItemType {
    ITEM_TYPE_WEAPON,
    ITEM_TYPE_ARMOR,
    ITEM_TYPE_CONSUMABLE,
    ITEM_TYPE_MATERIAL,
    ITEM_TYPE_QUEST,
    ITEM_TYPE_OTHER
};

// Estrutura do item
struct Item {
    uint32_t id;
    std::string name;
    std::string description;
    ItemType type;
    uint32_t itemClass;
    uint32_t level;
    uint32_t price;
    uint32_t weight;
    uint32_t durability;
    uint32_t maxDurability;
};

// Slot do inventário
struct InventorySlot {
    uint32_t itemId;
    uint32_t quantity;
    uint32_t durability;
    bool isEquipped;
};

// Sistema de inventário
class InventorySystem {
public:
    static InventorySystem& GetInstance();
    
    void Initialize();
    void Shutdown();
    
    // Gerenciamento de itens
    void RegisterItem(const Item& item);
    const Item* GetItem(uint32_t itemId) const;
    
    // Operações de inventário
    bool AddItem(uint32_t characterId, uint32_t itemId, uint32_t quantity);
    bool RemoveItem(uint32_t characterId, uint32_t itemId, uint32_t quantity);
    uint32_t GetItemQuantity(uint32_t characterId, uint32_t itemId) const;
    const InventorySlot* GetItemSlot(uint32_t characterId, uint32_t itemId) const;
    
    // Equipamento
    bool EquipItem(uint32_t characterId, uint32_t itemId);
    bool UnequipItem(uint32_t characterId, uint32_t itemId);
    bool IsItemEquipped(uint32_t characterId, uint32_t itemId) const;
    std::vector<const Item*> GetEquippedItems(uint32_t characterId) const;
    
    // Listagem de itens
    std::vector<const Item*> GetInventoryItems(uint32_t characterId) const;
    
    // Gerenciamento de peso
    uint32_t GetTotalWeight(uint32_t characterId) const;
    uint32_t GetMaxWeight(uint32_t characterId) const;
    bool IsInventoryFull(uint32_t characterId) const;
    bool IsInventoryOverweight(uint32_t characterId) const;
    
    // Gerenciamento de durabilidade
    bool UpdateItemDurability(uint32_t characterId, uint32_t itemId, int32_t amount);
    
private:
    InventorySystem() = default;
    ~InventorySystem() = default;
    InventorySystem(const InventorySystem&) = delete;
    InventorySystem& operator=(const InventorySystem&) = delete;
    
    static constexpr uint32_t BASE_MAX_WEIGHT = 1000;
    static constexpr uint32_t MAX_INVENTORY_SLOTS = 100;
    
    std::unordered_map<uint32_t, Item> items;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, InventorySlot>> inventories;
};

} // namespace WYDBR 