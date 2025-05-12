/**
 * WYDItem.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/player/WYDItem.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_ITEM_H
#define WYD_ITEM_H

#include <string>
#include <vector>
#include <array>
#include <map>
#include <cstdint>

#include "headers/wyd_core.h"

namespace wydbr {


/**
 * Efeitos de item
 */
enum class ItemEffect : uint8_t {
    NONE = 0,
    STR = 1,
    DEX = 2,
    INT = 3,
    CON = 4,
    ATTACK = 5,
    DEFENSE = 6,
    HP = 7,
    MP = 8,
    CRITICAL = 9,
    ATTACK_SPEED = 10,
    DAMAGE_ABSORPTION = 11,
    REFLECT_DAMAGE = 12,
    HP_REGEN = 13,
    MP_REGEN = 14,
    MOVE_SPEED = 15,
    RESIST_FIRE = 16,
    RESIST_ICE = 17,
    RESIST_LIGHTNING = 18,
    RESIST_MAGIC = 19,
    RESIST_POISON = 20,
    FIRE_DAMAGE = 21,
    ICE_DAMAGE = 22,
    LIGHTNING_DAMAGE = 23,
    STUN_CHANCE = 24,
    SLOW_CHANCE = 25,
    POISON_CHANCE = 26,
    EXPERIENCE_BONUS = 27,
    GOLD_BONUS = 28,
    DROP_RATE_BONUS = 29,
    SKILL_DURATION = 30,
    SKILL_COOLDOWN = 31
};

/**
 * Classe para representar um item no jogo
 * 
 * Esta classe implementa tanto a lógica de itens quanto corresponde ao
 * formato binário usado pelo cliente original do WYD.
 */
/**
 * Classe WYDItem
 * 
 * Esta classe implementa a funcionalidade WYDItem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDItem
 * 
 * Esta classe implementa a funcionalidade WYDItem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDItem : public WYDObject {
public:
    // /**
 * WYDItem
 * 
 * Implementa a funcionalidade WYDItem conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna Constructores
 */
 Constructores
    WYDItem(uint32_t id);
    WYDItem(uint32_t id, uint16_t itemId, uint8_t type, uint8_t subType);
    
    // Destructor
    virtual ~WYDItem() = default;
    
    // Propriedades básicas
    /**
 * getItemId
 * 
 * Implementa a funcionalidade getItemId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getItemId() const{ return itemId_; }
    /**
 * getType
 * 
 * Implementa a funcionalidade getType conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getType() const{ return type_; }
    /**
 * getSubType
 * 
 * Implementa a funcionalidade getSubType conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getSubType() const{ return subType_; }
    /**
 * getLevel
 * 
 * Implementa a funcionalidade getLevel conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getLevel() const{ return level_; }
    /**
 * getRefinement
 * 
 * Implementa a funcionalidade getRefinement conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getRefinement() const{ return refinement_; }
    /**
 * getQuality
 * 
 * Implementa a funcionalidade getQuality conforme especificação original.
 * @return Retorna wyd::ItemQuality
 */

    wyd::ItemQuality getQuality() const{ return static_cast<wyd::ItemQuality>(quality_); }
    /**
 * getDurability
 * 
 * Implementa a funcionalidade getDurability conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getDurability() const{ return durability_; }
    /**
 * getMaxDurability
 * 
 * Implementa a funcionalidade getMaxDurability conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getMaxDurability() const{ return maxDurability_; }
    
    // Atributos de combate
    /**
 * getAttack
 * 
 * Implementa a funcionalidade getAttack conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getAttack() const{ return attack_; }
    /**
 * getDefense
 * 
 * Implementa a funcionalidade getDefense conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getDefense() const{ return defense_; }
    
    // Requisitos
    /**
 * getReqLevel
 * 
 * Implementa a funcionalidade getReqLevel conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getReqLevel() const{ return reqLevel_; }
    /**
 * getReqStr
 * 
 * Implementa a funcionalidade getReqStr conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getReqStr() const{ return reqStr_; }
    /**
 * getReqDex
 * 
 * Implementa a funcionalidade getReqDex conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getReqDex() const{ return reqDex_; }
    /**
 * getReqInt
 * 
 * Implementa a funcionalidade getReqInt conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getReqInt() const{ return reqInt_; }
    /**
 * getReqCon
 * 
 * Implementa a funcionalidade getReqCon conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getReqCon() const{ return reqCon_; }
    
    // Efeitos especiais
    /**
 * getEffect
 * 
 * Implementa a funcionalidade getEffect conforme especificação original.
 * @param index Parâmetro index
 * @return Retorna uint8_t
 */

    uint8_t getEffect(size_t index) const{ return (index < 8) ? effects_[index] : 0; }
    
    // Sockets
    /**
 * getSocket
 * 
 * Implementa a funcionalidade getSocket conforme especificação original.
 * @param index Parâmetro index
 * @return Retorna uint16_t
 */

    uint16_t getSocket(size_t index) const{ return (index < 4) ? sockets_[index] : 0; }
    
    // Tempos e validade
    /**
 * getCreationTime
 * 
 * Implementa a funcionalidade getCreationTime conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getCreationTime() const{ return creationTime_; }
    /**
 * getExpirationTime
 * 
 * Implementa a funcionalidade getExpirationTime conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getExpirationTime() const{ return expirationTime_; }
    /**
 * isExpired
 * 
 * Implementa a funcionalidade isExpired conforme especificação original.
 * @return Retorna bool
 */

    bool isExpired() const;
    
    // Flags e estado
    /**
 * getFlags
 * 
 * Implementa a funcionalidade getFlags conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getFlags() const{ return flags_; }
    /**
 * hasFlag
 * 
 * Implementa a funcionalidade hasFlag conforme especificação original.
 * @param flag Parâmetro flag
 * @return Retorna bool
 */

    bool hasFlag(uint32_t flag) const{ return (flags_ & flag) != 0; }
    /**
 * isEquipped
 * 
 * Implementa a funcionalidade isEquipped conforme especificação original.
 * @return Retorna bool
 */

    bool isEquipped() const{ /**
 * hasFlag
 * 
 * Implementa a funcionalidade hasFlag conforme especificação original.
 * @param ITEM_FLAG_EQUIPPED Parâmetro ITEM_FLAG_EQUIPPED
 * @return Retorna return
 */
 return hasFlag(ITEM_FLAG_EQUIPPED); }
    /**
 * isTrading
 * 
 * Implementa a funcionalidade isTrading conforme especificação original.
 * @return Retorna bool
 */

    bool isTrading() const{ /**
 * hasFlag
 * 
 * Implementa a funcionalidade hasFlag conforme especificação original.
 * @param ITEM_FLAG_TRADING Parâmetro ITEM_FLAG_TRADING
 * @return Retorna return
 */
 return hasFlag(ITEM_FLAG_TRADING); }
    /**
 * isBound
 * 
 * Implementa a funcionalidade isBound conforme especificação original.
 * @return Retorna bool
 */

    bool isBound() const{ /**
 * hasFlag
 * 
 * Implementa a funcionalidade hasFlag conforme especificação original.
 * @param ITEM_FLAG_BOUND Parâmetro ITEM_FLAG_BOUND
 * @return Retorna return
 */
 return hasFlag(ITEM_FLAG_BOUND); }
    /**
 * isStackable
 * 
 * Implementa a funcionalidade isStackable conforme especificação original.
 * @return Retorna bool
 */

    bool isStackable() const;
    /**
 * getMaxStackSize
 * 
 * Implementa a funcionalidade getMaxStackSize conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getMaxStackSize() const;
    
    // Quantidade (para itens empilháveis)
    /**
 * getQuantity
 * 
 * Implementa a funcionalidade getQuantity conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getQuantity() const{ return quantity_; }
    /**
 * setQuantity
 * 
 * Implementa a funcionalidade setQuantity conforme especificação original.
 * @param quantity Parâmetro quantity
 */

    void setQuantity(uint16_t quantity){ quantity_ = quantity; }
    /**
 * addQuantity
 * 
 * Implementa a funcionalidade addQuantity conforme especificação original.
 * @param amount Parâmetro amount
 * @return Retorna bool
 */

    bool addQuantity(uint16_t amount);
    /**
 * removeQuantity
 * 
 * Implementa a funcionalidade removeQuantity conforme especificação original.
 * @param amount Parâmetro amount
 * @return Retorna bool
 */

    bool removeQuantity(uint16_t amount);
    
    // Setters
    /**
 * setLevel
 * 
 * Implementa a funcionalidade setLevel conforme especificação original.
 * @param level Parâmetro level
 */

    void setLevel(uint8_t level){ level_ = level; }
    /**
 * setRefinement
 * 
 * Implementa a funcionalidade setRefinement conforme especificação original.
 * @param refinement Parâmetro refinement
 */

    void setRefinement(uint8_t refinement){ refinement_ = refinement; }
    /**
 * setQuality
 * 
 * Implementa a funcionalidade setQuality conforme especificação original.
 * @param quality Parâmetro quality
 */

    void setQuality(wyd::ItemQuality quality){ quality_ = static_cast<uint8_t>(quality); }
    /**
 * setDurability
 * 
 * Implementa a funcionalidade setDurability conforme especificação original.
 * @param durability Parâmetro durability
 */

    void setDurability(uint8_t durability){ durability_ = durability; }
    
    // Manipulação de durabilidade
    /**
 * reduceDurability
 * 
 * Implementa a funcionalidade reduceDurability conforme especificação original.
 * @param 1 Parâmetro 1
 * @return Retorna bool
 */

    bool reduceDurability(uint8_t amount = 1);
    /**
 * repair
 * 
 * Implementa a funcionalidade repair conforme especificação original.
 */

    void repair();
    
    // Manipulação de refinamento
    /**
 * refine
 * 
 * Implementa a funcionalidade refine conforme especificação original.
 * @param false Parâmetro false
 * @return Retorna bool
 */

    bool refine(bool useProtection = false);
    /**
 * degradeRefinement
 * 
 * Implementa a funcionalidade degradeRefinement conforme especificação original.
 * @return Retorna bool
 */

    bool degradeRefinement();
    
    // Socket
    /**
 * addSocket
 * 
 * Implementa a funcionalidade addSocket conforme especificação original.
 * @param socketId Parâmetro socketId
 * @param index Parâmetro index
 * @return Retorna bool
 */

    bool addSocket(uint16_t socketId, size_t index);
    /**
 * removeSocket
 * 
 * Implementa a funcionalidade removeSocket conforme especificação original.
 * @param index Parâmetro index
 * @return Retorna bool
 */

    bool removeSocket(size_t index);
    
    // Tempo
    /**
 * setExpirationTime
 * 
 * Implementa a funcionalidade setExpirationTime conforme especificação original.
 * @param time Parâmetro time
 */

    void setExpirationTime(uint32_t time){ expirationTime_ = time; }
    /**
 * extendExpirationTime
 * 
 * Implementa a funcionalidade extendExpirationTime conforme especificação original.
 * @param seconds Parâmetro seconds
 */

    void extendExpirationTime(uint32_t seconds);
    
    // Flags
    /**
 * addFlag
 * 
 * Implementa a funcionalidade addFlag conforme especificação original.
 * @param flag Parâmetro flag
 */

    void addFlag(uint32_t flag){ flags_ |= flag; }
    /**
 * removeFlag
 * 
 * Implementa a funcionalidade removeFlag conforme especificação original.
 * @param flag Parâmetro flag
 */

    void removeFlag(uint32_t flag){ flags_ &= ~flag; }
    
    // Serialização/Desserialização
    std::vector<uint8_t> serialize() const override;
    /**
 * deserialize
 * 
 * Implementa a funcionalidade deserialize conforme especificação original.
 * @param data Parâmetro data
 */

    void deserialize(const std::vector<uint8_t>& data) override;
    
    // Helpers
    /**
 * isWeapon
 * 
 * Implementa a funcionalidade isWeapon conforme especificação original.
 * @return Retorna bool
 */

    bool isWeapon() const{ return type_ == static_cast<uint8_t>(wyd::ItemType::WEAPON); }
    /**
 * isArmor
 * 
 * Implementa a funcionalidade isArmor conforme especificação original.
 * @return Retorna bool
 */

    bool isArmor() const{ return type_ == static_cast<uint8_t>(wyd::ItemType::ARMOR); }
    /**
 * isConsumable
 * 
 * Implementa a funcionalidade isConsumable conforme especificação original.
 * @return Retorna bool
 */

    bool isConsumable() const{ return type_ == static_cast<uint8_t>(wyd::ItemType::CONSUMABLE); }
    /**
 * isMaterial
 * 
 * Implementa a funcionalidade isMaterial conforme especificação original.
 * @return Retorna bool
 */

    bool isMaterial() const{ return type_ == static_cast<uint8_t>(wyd::ItemType::MATERIAL); }
    /**
 * canBeRefined
 * 
 * Implementa a funcionalidade canBeRefined conforme especificação original.
 * @return Retorna bool
 */

    bool canBeRefined() const;
    /**
 * calculateEffectValue
 * 
 * Implementa a funcionalidade calculateEffectValue conforme especificação original.
 * @param effect Parâmetro effect
 * @return Retorna uint16_t
 */

    uint16_t calculateEffectValue(ItemEffect effect) const;
    
    // Flags de item constantes
    static const uint32_t ITEM_FLAG_EQUIPPED = 0x01;
    static const uint32_t ITEM_FLAG_TRADING = 0x02;
    static const uint32_t ITEM_FLAG_BOUND = 0x04;
    static const uint32_t ITEM_FLAG_REFINED = 0x08;
    static const uint32_t ITEM_FLAG_SOCKET = 0x10;
    static const uint32_t ITEM_FLAG_LIMITED = 0x20;
    static const uint32_t ITEM_FLAG_NONDROP = 0x40;
    static const uint32_t ITEM_FLAG_UNIQUE = 0x80;
    
private:
    // Propriedades básicas (72 bytes conforme ItemList.bin)
    uint16_t itemId_;           // ID do item no arquivo ItemList.bin
    uint8_t type_;              // Tipo do item (arma, armadura, etc.)
    uint8_t subType_;           // Subtipo do item
    uint8_t level_;             // Nível do item (0-255)
    uint8_t refinement_;        // Nível de refinamento (0-15)
    uint8_t quality_;           // Qualidade do item (normal, raro, etc.)
    uint8_t durability_;        // Durabilidade atual
    uint8_t maxDurability_;     // Durabilidade máxima
    uint16_t attack_;           // Bônus de ataque base
    uint16_t defense_;          // Bônus de defesa base
    uint16_t reqLevel_;         // Nível requerido para uso
    uint16_t reqStr_;           // Força requerida
    uint16_t reqDex_;           // Destreza requerida
    uint16_t reqInt_;           // Inteligência requerida
    uint16_t reqCon_;           // Constituição requerida
    uint8_t effects_[8];        // Efeitos especiais
    uint16_t sockets_[4];       // Slots para pedras (sockets)
    uint32_t creationTime_;     // Timestamp de criação
    uint32_t expirationTime_;   // Timestamp de expiração (0 = nunca expira)
    uint32_t flags_;            // Flags de estado
    uint8_t reserved_[19];      // Bytes reservados
    
    // Propriedades adicionais não contidas no formato original
    uint16_t quantity_;         // Quantidade (para itens empilháveis)
    
    // Métodos auxiliares
    /**
 * getRefinementSuccessRate
 * 
 * Implementa a funcionalidade getRefinementSuccessRate conforme especificação original.
 * @return Retorna float
 */

    float getRefinementSuccessRate() const;
    /**
 * getRefinementBreakRate
 * 
 * Implementa a funcionalidade getRefinementBreakRate conforme especificação original.
 * @return Retorna float
 */

    float getRefinementBreakRate() const;
    /**
 * updateStatsFromRefinement
 * 
 * Implementa a funcionalidade updateStatsFromRefinement conforme especificação original.
 */

    void updateStatsFromRefinement();
    /**
 * applyRefinementBonuses
 * 
 * Implementa a funcionalidade applyRefinementBonuses conforme especificação original.
 */

    void applyRefinementBonuses();
};

#endif // WYD_ITEM_H

} // namespace wydbr
