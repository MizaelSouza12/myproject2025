/**
 * WYDInventory.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/player/WYDInventory.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_INVENTORY_H
#define WYD_INVENTORY_H

#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <memory>

#include "headers/wyd_core.h"
#include "src/server/tmsrv/player/WYDItem.h"

namespace wydbr {


class WYDPlayer;

/**
 * Slots de equipamento possíveis
 */
enum class EquipSlot : uint8_t {
    WEAPON = 0,
    SHIELD = 1,
    HELMET = 2,
    ARMOR = 3,
    PANTS = 4,
    GLOVES = 5,
    BOOTS = 6,
    RING_L = 7,
    RING_R = 8,
    NECKLACE = 9,
    EARRING = 10,
    BELT = 11,
    MOUNT = 12,
    MANTLE = 13,
    COSTUME = 14,
    FACE = 15,
    PET = 16
};

/**
 * Sistema de inventário para jogadores
 */
/**
 * Classe WYDInventory
 * 
 * Esta classe implementa a funcionalidade WYDInventory conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDInventory {
public:
    // Definições gerais
    static const uint16_t MAX_INVENTORY_SLOTS = 64;
    static const uint16_t MAX_BANK_SLOTS = 120;
    static const uint16_t MAX_EQUIP_SLOTS = 17;
    
    // Construtor
    /**
 * WYDInventory
 * 
 * Implementa a funcionalidade WYDInventory conforme especificação original.
 * @param player Parâmetro player
 * @return Retorna explicit
 */

    explicit WYDInventory(WYDPlayer* player);
    
    // Destrutor
    ~WYDInventory();
    
    // Gerenciamento de itens no inventário
    /**
 * addItem
 * 
 * Implementa a funcionalidade addItem conforme especificação original.
 * @param item Parâmetro item
 * @param 0xFFFF Parâmetro 0xFFFF
 * @return Retorna bool
 */

    bool addItem(const WYDItem& item, uint16_t slot = 0xFFFF); // 0xFFFF = primeiro slot disponível
    /**
 * removeItem
 * 
 * Implementa a funcionalidade removeItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool removeItem(uint16_t slot);
    /**
 * moveItem
 * 
 * Implementa a funcionalidade moveItem conforme especificação original.
 * @param fromSlot Parâmetro fromSlot
 * @param toSlot Parâmetro toSlot
 * @return Retorna bool
 */

    bool moveItem(uint16_t fromSlot, uint16_t toSlot);
    /**
 * splitItem
 * 
 * Implementa a funcionalidade splitItem conforme especificação original.
 * @param slot Parâmetro slot
 * @param amount Parâmetro amount
 * @param 0xFFFF Parâmetro 0xFFFF
 * @return Retorna bool
 */

    bool splitItem(uint16_t slot, uint16_t amount, uint16_t targetSlot = 0xFFFF);
    /**
 * combineItems
 * 
 * Implementa a funcionalidade combineItems conforme especificação original.
 * @param slot1 Parâmetro slot1
 * @param slot2 Parâmetro slot2
 * @return Retorna bool
 */

    bool combineItems(uint16_t slot1, uint16_t slot2);
    
    // Acesso a itens
    /**
 * getItem
 * 
 * Implementa a funcionalidade getItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna WYDItem
 */

    WYDItem* getItem(uint16_t slot);
    /**
 * getItem
 * 
 * Implementa a funcionalidade getItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna WYDItem
 */

    const WYDItem* getItem(uint16_t slot) const;
    /**
 * hasItem
 * 
 * Implementa a funcionalidade hasItem conforme especificação original.
 * @param itemId Parâmetro itemId
 * @param 1 Parâmetro 1
 * @return Retorna bool
 */

    bool hasItem(uint32_t itemId, uint16_t amount = 1) const;
    /**
 * countItem
 * 
 * Implementa a funcionalidade countItem conforme especificação original.
 * @param itemId Parâmetro itemId
 * @return Retorna uint16_t
 */

    uint16_t countItem(uint32_t itemId) const;
    /**
 * findItem
 * 
 * Implementa a funcionalidade findItem conforme especificação original.
 * @param itemId Parâmetro itemId
 * @return Retorna uint16_t
 */

    uint16_t findItem(uint32_t itemId) const; // Retorna o slot ou 0xFFFF se não encontrado
    
    // Equipamento
    /**
 * equipItem
 * 
 * Implementa a funcionalidade equipItem conforme especificação original.
 * @param inventorySlot Parâmetro inventorySlot
 * @return Retorna bool
 */

    bool equipItem(uint16_t inventorySlot);
    /**
 * unequipItem
 * 
 * Implementa a funcionalidade unequipItem conforme especificação original.
 * @param slot Parâmetro slot
 * @param 0xFFFF Parâmetro 0xFFFF
 * @return Retorna bool
 */

    bool unequipItem(EquipSlot slot, uint16_t inventorySlot = 0xFFFF); // 0xFFFF = primeiro slot disponível
    /**
 * getEquippedItem
 * 
 * Implementa a funcionalidade getEquippedItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna WYDItem
 */

    WYDItem* getEquippedItem(EquipSlot slot);
    /**
 * getEquippedItem
 * 
 * Implementa a funcionalidade getEquippedItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna WYDItem
 */

    const WYDItem* getEquippedItem(EquipSlot slot) const;
    /**
 * isEquipSlotEmpty
 * 
 * Implementa a funcionalidade isEquipSlotEmpty conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool isEquipSlotEmpty(EquipSlot slot) const;
    /**
 * canEquipItem
 * 
 * Implementa a funcionalidade canEquipItem conforme especificação original.
 * @param item Parâmetro item
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool canEquipItem(const WYDItem& item, EquipSlot slot) const;
    
    // Banco de itens
    /**
 * depositItem
 * 
 * Implementa a funcionalidade depositItem conforme especificação original.
 * @param inventorySlot Parâmetro inventorySlot
 * @param 0xFFFF Parâmetro 0xFFFF
 * @return Retorna bool
 */

    bool depositItem(uint16_t inventorySlot, uint16_t bankSlot = 0xFFFF);
    /**
 * withdrawItem
 * 
 * Implementa a funcionalidade withdrawItem conforme especificação original.
 * @param bankSlot Parâmetro bankSlot
 * @param 0xFFFF Parâmetro 0xFFFF
 * @return Retorna bool
 */

    bool withdrawItem(uint16_t bankSlot, uint16_t inventorySlot = 0xFFFF);
    /**
 * moveBankItem
 * 
 * Implementa a funcionalidade moveBankItem conforme especificação original.
 * @param fromSlot Parâmetro fromSlot
 * @param toSlot Parâmetro toSlot
 * @return Retorna bool
 */

    bool moveBankItem(uint16_t fromSlot, uint16_t toSlot);
    /**
 * getBankItem
 * 
 * Implementa a funcionalidade getBankItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna WYDItem
 */

    WYDItem* getBankItem(uint16_t slot);
    /**
 * getBankItem
 * 
 * Implementa a funcionalidade getBankItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna WYDItem
 */

    const WYDItem* getBankItem(uint16_t slot) const;
    
    // Operações gerais
    /**
 * useItem
 * 
 * Implementa a funcionalidade useItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool useItem(uint16_t slot);
    /**
 * dropItem
 * 
 * Implementa a funcionalidade dropItem conforme especificação original.
 * @param slot Parâmetro slot
 * @param 0 Parâmetro 0
 * @return Retorna bool
 */

    bool dropItem(uint16_t slot, uint16_t amount = 0); // 0 = toda a pilha
    /**
 * findEmptySlot
 * 
 * Implementa a funcionalidade findEmptySlot conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t findEmptySlot() const;
    /**
 * findEmptyBankSlot
 * 
 * Implementa a funcionalidade findEmptyBankSlot conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t findEmptyBankSlot() const;
    /**
 * isInventoryFull
 * 
 * Implementa a funcionalidade isInventoryFull conforme especificação original.
 * @return Retorna bool
 */

    bool isInventoryFull() const;
    /**
 * isBankFull
 * 
 * Implementa a funcionalidade isBankFull conforme especificação original.
 * @return Retorna bool
 */

    bool isBankFull() const;
    
    // Verificações de requisitos de item
    /**
 * meetsItemRequirements
 * 
 * Implementa a funcionalidade meetsItemRequirements conforme especificação original.
 * @param item Parâmetro item
 * @return Retorna bool
 */

    bool meetsItemRequirements(const WYDItem& item) const;
    
    // Serialização/Desserialização
    std::vector<uint8_t> serialize() const;
    /**
 * deserialize
 * 
 * Implementa a funcionalidade deserialize conforme especificação original.
 * @param data Parâmetro data
 * @return Retorna bool
 */

    bool deserialize(const std::vector<uint8_t>& data);
    
    // Para atualização de estatísticas do jogador
    /**
 * recalculateEquipmentStats
 * 
 * Implementa a funcionalidade recalculateEquipmentStats conforme especificação original.
 */

    void recalculateEquipmentStats();
    
private:
    // Referência ao jogador
    WYDPlayer* player_;
    
    // Itens no inventário
    std::vector<std::unique_ptr<WYDItem>> inventory_;
    
    // Itens equipados
    std::map<EquipSlot, std::unique_ptr<WYDItem>> equipment_;
    
    // Itens no banco
    std::vector<std::unique_ptr<WYDItem>> bank_;
    
    // Mutex para acesso seguro
    mutable std::mutex mutex_;
    
    // Métodos auxiliares
    /**
 * isValidInventorySlot
 * 
 * Implementa a funcionalidade isValidInventorySlot conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool isValidInventorySlot(uint16_t slot) const;
    /**
 * isValidBankSlot
 * 
 * Implementa a funcionalidade isValidBankSlot conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool isValidBankSlot(uint16_t slot) const;
    /**
 * isSlotEmpty
 * 
 * Implementa a funcionalidade isSlotEmpty conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool isSlotEmpty(uint16_t slot) const;
    /**
 * isBankSlotEmpty
 * 
 * Implementa a funcionalidade isBankSlotEmpty conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool isBankSlotEmpty(uint16_t slot) const;
};

#endif // WYD_INVENTORY_H

} // namespace wydbr
