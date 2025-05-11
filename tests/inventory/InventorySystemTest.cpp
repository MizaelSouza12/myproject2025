#include <gtest/gtest.h>
#include "GameLogic/inventory/InventorySystem.h"
#include "GameLogic/character/CharacterSystem.h"

using namespace WYDBR;

class InventorySystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        CharacterSystem::GetInstance().Initialize();
        InventorySystem::GetInstance().Initialize();
        
        // Criar personagem de teste
        characterId = CharacterSystem::GetInstance().CreateCharacter("Test", CHARACTER_WARRIOR);
        EXPECT_GT(characterId, 0);
        
        // Registrar itens de teste
        Item weapon;
        weapon.id = 1;
        weapon.name = "Test Sword";
        weapon.description = "A test sword";
        weapon.type = ITEM_TYPE_WEAPON;
        weapon.itemClass = CHARACTER_WARRIOR;
        weapon.level = 1;
        weapon.price = 1000;
        weapon.weight = 10;
        weapon.durability = 100;
        weapon.maxDurability = 100;
        
        Item armor;
        armor.id = 2;
        armor.name = "Test Armor";
        armor.description = "A test armor";
        armor.type = ITEM_TYPE_ARMOR;
        armor.itemClass = CHARACTER_WARRIOR;
        armor.level = 1;
        armor.price = 2000;
        armor.weight = 20;
        armor.durability = 200;
        armor.maxDurability = 200;
        
        Item consumable;
        consumable.id = 3;
        consumable.name = "Test Potion";
        consumable.description = "A test potion";
        consumable.type = ITEM_TYPE_CONSUMABLE;
        consumable.itemClass = CHARACTER_WARRIOR;
        consumable.level = 1;
        consumable.price = 100;
        consumable.weight = 1;
        consumable.durability = 1;
        consumable.maxDurability = 1;
        
        InventorySystem::GetInstance().RegisterItem(weapon);
        InventorySystem::GetInstance().RegisterItem(armor);
        InventorySystem::GetInstance().RegisterItem(consumable);
    }
    
    void TearDown() override {
        InventorySystem::GetInstance().Shutdown();
        CharacterSystem::GetInstance().Shutdown();
    }
    
    uint32_t characterId;
};

TEST_F(InventorySystemTest, RegisterAndGetItem) {
    auto& inventorySystem = InventorySystem::GetInstance();
    
    // Verificar arma registrada
    const Item* weapon = inventorySystem.GetItem(1);
    EXPECT_NE(weapon, nullptr);
    EXPECT_EQ(weapon->id, 1);
    EXPECT_EQ(weapon->name, "Test Sword");
    EXPECT_EQ(weapon->type, ITEM_TYPE_WEAPON);
    EXPECT_EQ(weapon->itemClass, CHARACTER_WARRIOR);
    EXPECT_EQ(weapon->level, 1);
    EXPECT_EQ(weapon->price, 1000);
    EXPECT_EQ(weapon->weight, 10);
    EXPECT_EQ(weapon->durability, 100);
    EXPECT_EQ(weapon->maxDurability, 100);
    
    // Verificar armadura registrada
    const Item* armor = inventorySystem.GetItem(2);
    EXPECT_NE(armor, nullptr);
    EXPECT_EQ(armor->id, 2);
    EXPECT_EQ(armor->name, "Test Armor");
    EXPECT_EQ(armor->type, ITEM_TYPE_ARMOR);
    
    // Verificar consumível registrado
    const Item* consumable = inventorySystem.GetItem(3);
    EXPECT_NE(consumable, nullptr);
    EXPECT_EQ(consumable->id, 3);
    EXPECT_EQ(consumable->name, "Test Potion");
    EXPECT_EQ(consumable->type, ITEM_TYPE_CONSUMABLE);
}

TEST_F(InventorySystemTest, AddAndRemoveItem) {
    auto& inventorySystem = InventorySystem::GetInstance();
    
    // Adicionar itens
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 1, 1)); // Arma
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 2, 1)); // Armadura
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 3, 5)); // 5 poções
    
    // Verificar quantidades
    EXPECT_EQ(inventorySystem.GetItemQuantity(characterId, 1), 1);
    EXPECT_EQ(inventorySystem.GetItemQuantity(characterId, 2), 1);
    EXPECT_EQ(inventorySystem.GetItemQuantity(characterId, 3), 5);
    
    // Remover itens
    EXPECT_TRUE(inventorySystem.RemoveItem(characterId, 1, 1));
    EXPECT_TRUE(inventorySystem.RemoveItem(characterId, 2, 1));
    EXPECT_TRUE(inventorySystem.RemoveItem(characterId, 3, 2));
    
    // Verificar quantidades após remoção
    EXPECT_EQ(inventorySystem.GetItemQuantity(characterId, 1), 0);
    EXPECT_EQ(inventorySystem.GetItemQuantity(characterId, 2), 0);
    EXPECT_EQ(inventorySystem.GetItemQuantity(characterId, 3), 3);
}

TEST_F(InventorySystemTest, EquipAndUnequipItem) {
    auto& inventorySystem = InventorySystem::GetInstance();
    
    // Adicionar itens
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 1, 1)); // Arma
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 2, 1)); // Armadura
    
    // Equipar itens
    EXPECT_TRUE(inventorySystem.EquipItem(characterId, 1));
    EXPECT_TRUE(inventorySystem.EquipItem(characterId, 2));
    
    // Verificar se estão equipados
    EXPECT_TRUE(inventorySystem.IsItemEquipped(characterId, 1));
    EXPECT_TRUE(inventorySystem.IsItemEquipped(characterId, 2));
    
    // Desequipar itens
    EXPECT_TRUE(inventorySystem.UnequipItem(characterId, 1));
    EXPECT_TRUE(inventorySystem.UnequipItem(characterId, 2));
    
    // Verificar se foram desequipados
    EXPECT_FALSE(inventorySystem.IsItemEquipped(characterId, 1));
    EXPECT_FALSE(inventorySystem.IsItemEquipped(characterId, 2));
}

TEST_F(InventorySystemTest, GetEquippedItems) {
    auto& inventorySystem = InventorySystem::GetInstance();
    
    // Adicionar e equipar itens
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 1, 1));
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 2, 1));
    EXPECT_TRUE(inventorySystem.EquipItem(characterId, 1));
    EXPECT_TRUE(inventorySystem.EquipItem(characterId, 2));
    
    // Obter itens equipados
    auto equippedItems = inventorySystem.GetEquippedItems(characterId);
    EXPECT_EQ(equippedItems.size(), 2);
    
    // Verificar itens equipados
    bool hasWeapon = false;
    bool hasArmor = false;
    
    for (const Item* item : equippedItems) {
        if (item->id == 1) {
            hasWeapon = true;
        } else if (item->id == 2) {
            hasArmor = true;
        }
    }
    
    EXPECT_TRUE(hasWeapon);
    EXPECT_TRUE(hasArmor);
}

TEST_F(InventorySystemTest, GetInventoryItems) {
    auto& inventorySystem = InventorySystem::GetInstance();
    
    // Adicionar itens
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 1, 1));
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 2, 1));
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 3, 5));
    
    // Obter itens do inventário
    auto inventoryItems = inventorySystem.GetInventoryItems(characterId);
    EXPECT_EQ(inventoryItems.size(), 3);
    
    // Verificar itens
    bool hasWeapon = false;
    bool hasArmor = false;
    bool hasConsumable = false;
    
    for (const Item* item : inventoryItems) {
        if (item->id == 1) {
            hasWeapon = true;
        } else if (item->id == 2) {
            hasArmor = true;
        } else if (item->id == 3) {
            hasConsumable = true;
        }
    }
    
    EXPECT_TRUE(hasWeapon);
    EXPECT_TRUE(hasArmor);
    EXPECT_TRUE(hasConsumable);
}

TEST_F(InventorySystemTest, WeightManagement) {
    auto& inventorySystem = InventorySystem::GetInstance();
    
    // Verificar peso inicial
    EXPECT_EQ(inventorySystem.GetTotalWeight(characterId), 0);
    
    // Adicionar itens
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 1, 1)); // 10 de peso
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 2, 1)); // 20 de peso
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 3, 5)); // 5 de peso
    
    // Verificar peso total
    EXPECT_EQ(inventorySystem.GetTotalWeight(characterId), 35);
    
    // Verificar peso máximo
    const Mob* character = CharacterSystem::GetInstance().GetCharacter(characterId);
    uint32_t expectedMaxWeight = 1000 + (character->Strength * 10);
    EXPECT_EQ(inventorySystem.GetMaxWeight(characterId), expectedMaxWeight);
    
    // Verificar sobrecarga
    EXPECT_FALSE(inventorySystem.IsInventoryOverweight(characterId));
}

TEST_F(InventorySystemTest, DurabilityManagement) {
    auto& inventorySystem = InventorySystem::GetInstance();
    
    // Adicionar item
    EXPECT_TRUE(inventorySystem.AddItem(characterId, 1, 1));
    
    // Verificar durabilidade inicial
    const InventorySlot* slot = inventorySystem.GetItemSlot(characterId, 1);
    EXPECT_NE(slot, nullptr);
    EXPECT_EQ(slot->durability, 100);
    
    // Reduzir durabilidade
    EXPECT_TRUE(inventorySystem.UpdateItemDurability(characterId, 1, -50));
    slot = inventorySystem.GetItemSlot(characterId, 1);
    EXPECT_NE(slot, nullptr);
    EXPECT_EQ(slot->durability, 50);
    
    // Aumentar durabilidade
    EXPECT_TRUE(inventorySystem.UpdateItemDurability(characterId, 1, 25));
    slot = inventorySystem.GetItemSlot(characterId, 1);
    EXPECT_NE(slot, nullptr);
    EXPECT_EQ(slot->durability, 75);
    
    // Reduzir durabilidade até zero
    EXPECT_TRUE(inventorySystem.UpdateItemDurability(characterId, 1, -100));
    slot = inventorySystem.GetItemSlot(characterId, 1);
    EXPECT_EQ(slot, nullptr); // Item deve ser removido
}

TEST_F(InventorySystemTest, InvalidOperations) {
    auto& inventorySystem = InventorySystem::GetInstance();
    
    // Tentar adicionar item inexistente
    EXPECT_FALSE(inventorySystem.AddItem(characterId, 999, 1));
    
    // Tentar remover item inexistente
    EXPECT_FALSE(inventorySystem.RemoveItem(characterId, 999, 1));
    
    // Tentar equipar item inexistente
    EXPECT_FALSE(inventorySystem.EquipItem(characterId, 999));
    
    // Tentar desequipar item inexistente
    EXPECT_FALSE(inventorySystem.UnequipItem(characterId, 999));
    
    // Tentar atualizar durabilidade de item inexistente
    EXPECT_FALSE(inventorySystem.UpdateItemDurability(characterId, 999, -10));
    
    // Tentar adicionar item de classe diferente
    Item mageItem;
    mageItem.id = 4;
    mageItem.type = ITEM_TYPE_WEAPON;
    mageItem.itemClass = CHARACTER_MAGE;
    mageItem.level = 1;
    inventorySystem.RegisterItem(mageItem);
    
    EXPECT_FALSE(inventorySystem.AddItem(characterId, 4, 1));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 