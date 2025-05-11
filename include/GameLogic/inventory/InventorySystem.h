/**
 * @file InventorySystem.h
 * @brief Sistema de inventário do WYDBR 2.0
 */

#ifndef WYDBR_INVENTORY_SYSTEM_H
#define WYDBR_INVENTORY_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "wyd_core.h"
#include "item_struct.h"

namespace WYDBR {

/**
 * @struct Item
 * @brief Definição de um item
 */
struct Item {
    uint32_t id;                ///< ID do item
    std::string name;           ///< Nome do item
    std::string description;    ///< Descrição do item
    ItemType type;              ///< Tipo do item
    ItemClass itemClass;        ///< Classe do item
    uint8_t level;              ///< Nível requerido
    uint32_t price;             ///< Preço do item
    uint32_t weight;            ///< Peso do item
    uint32_t durability;        ///< Durabilidade do item
    uint32_t maxDurability;     ///< Durabilidade máxima
    std::vector<ItemEffect> effects; ///< Efeitos do item
};

/**
 * @struct InventorySlot
 * @brief Slot do inventário
 */
struct InventorySlot {
    uint32_t itemId;            ///< ID do item
    uint32_t quantity;          ///< Quantidade
    uint32_t durability;        ///< Durabilidade atual
    bool isEquipped;            ///< Se está equipado
};

/**
 * @class InventorySystem
 * @brief Sistema de inventário do WYDBR 2.0
 * 
 * Este sistema gerencia todo o inventário do jogo, incluindo:
 * - Itens
 * - Equipamentos
 * - Consumíveis
 * - Materiais
 * - Moedas
 * - Peso
 * - Durabilidade
 */
class InventorySystem {
public:
    /**
     * @brief Obtém a instância única do sistema de inventário
     * @return Referência para a instância do sistema de inventário
     */
    static InventorySystem& GetInstance();
    
    /**
     * @brief Inicializa o sistema de inventário
     */
    void Initialize();
    
    /**
     * @brief Finaliza o sistema de inventário
     */
    void Shutdown();
    
    /**
     * @brief Registra um novo item
     * @param item Item a ser registrado
     */
    void RegisterItem(const Item& item);
    
    /**
     * @brief Obtém um item
     * @param itemId ID do item
     * @return Ponteiro para o item
     */
    const Item* GetItem(uint32_t itemId) const;
    
    /**
     * @brief Adiciona um item ao inventário
     * @param characterId ID do personagem
     * @param itemId ID do item
     * @param quantity Quantidade
     * @return true se adicionou com sucesso, false caso contrário
     */
    bool AddItem(uint32_t characterId, uint32_t itemId, uint32_t quantity = 1);
    
    /**
     * @brief Remove um item do inventário
     * @param characterId ID do personagem
     * @param itemId ID do item
     * @param quantity Quantidade
     * @return true se removeu com sucesso, false caso contrário
     */
    bool RemoveItem(uint32_t characterId, uint32_t itemId, uint32_t quantity = 1);
    
    /**
     * @brief Obtém a quantidade de um item no inventário
     * @param characterId ID do personagem
     * @param itemId ID do item
     * @return Quantidade do item
     */
    uint32_t GetItemQuantity(uint32_t characterId, uint32_t itemId) const;
    
    /**
     * @brief Obtém o slot de um item no inventário
     * @param characterId ID do personagem
     * @param itemId ID do item
     * @return Ponteiro para o slot
     */
    const InventorySlot* GetItemSlot(uint32_t characterId, uint32_t itemId) const;
    
    /**
     * @brief Equipa um item
     * @param characterId ID do personagem
     * @param itemId ID do item
     * @return true se equipou com sucesso, false caso contrário
     */
    bool EquipItem(uint32_t characterId, uint32_t itemId);
    
    /**
     * @brief Desequipa um item
     * @param characterId ID do personagem
     * @param itemId ID do item
     * @return true se desequipou com sucesso, false caso contrário
     */
    bool UnequipItem(uint32_t characterId, uint32_t itemId);
    
    /**
     * @brief Verifica se um item está equipado
     * @param characterId ID do personagem
     * @param itemId ID do item
     * @return true se está equipado, false caso contrário
     */
    bool IsItemEquipped(uint32_t characterId, uint32_t itemId) const;
    
    /**
     * @brief Obtém todos os itens equipados
     * @param characterId ID do personagem
     * @return Lista de itens equipados
     */
    std::vector<const Item*> GetEquippedItems(uint32_t characterId) const;
    
    /**
     * @brief Obtém todos os itens do inventário
     * @param characterId ID do personagem
     * @return Lista de itens
     */
    std::vector<const Item*> GetInventoryItems(uint32_t characterId) const;
    
    /**
     * @brief Obtém o peso total do inventário
     * @param characterId ID do personagem
     * @return Peso total
     */
    uint32_t GetTotalWeight(uint32_t characterId) const;
    
    /**
     * @brief Obtém o peso máximo do inventário
     * @param characterId ID do personagem
     * @return Peso máximo
     */
    uint32_t GetMaxWeight(uint32_t characterId) const;
    
    /**
     * @brief Verifica se o inventário está cheio
     * @param characterId ID do personagem
     * @return true se está cheio, false caso contrário
     */
    bool IsInventoryFull(uint32_t characterId) const;
    
    /**
     * @brief Verifica se o inventário está sobrecarregado
     * @param characterId ID do personagem
     * @return true se está sobrecarregado, false caso contrário
     */
    bool IsInventoryOverweight(uint32_t characterId) const;
    
    /**
     * @brief Atualiza a durabilidade de um item
     * @param characterId ID do personagem
     * @param itemId ID do item
     * @param amount Quantidade a ser alterada
     * @return true se atualizou com sucesso, false caso contrário
     */
    bool UpdateItemDurability(uint32_t characterId, uint32_t itemId, int32_t amount);

private:
    InventorySystem() = default;
    ~InventorySystem() = default;
    
    // Prevenir cópias
    InventorySystem(const InventorySystem&) = delete;
    InventorySystem& operator=(const InventorySystem&) = delete;
    
    // Mapa de itens
    std::unordered_map<uint32_t, Item> items;
    
    // Mapa de inventários
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, InventorySlot>> inventories;
    
    // Constantes
    static constexpr uint32_t MAX_INVENTORY_SLOTS = 50;  ///< Número máximo de slots
    static constexpr uint32_t BASE_MAX_WEIGHT = 1000;    ///< Peso máximo base
};

} // namespace WYDBR

#endif // WYDBR_INVENTORY_SYSTEM_H 