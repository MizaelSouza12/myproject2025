/**
 * ItemSystem.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/game/ItemSystem.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef ITEM_SYSTEM_H
#define ITEM_SYSTEM_H

#include "../core/WYDTypes.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>

namespace wydbr {


/**
 * @file ItemSystem.h
 * @brief Sistema de itens do WYD
 * 
 * Este arquivo implementa o sistema de itens do WYD, responsável pelo gerenciamento
 * de itens, inventário, e operações relacionadas. Implementa a lógica original,
 * mas com correções para os bugs conhecidos do jogo.
 */

namespace wyd {
namespace game {

// Forward declarations
class ItemInstance;
class ItemTemplate;
class ItemManager;

// Tipos de itens
enum class ItemCategory : BYTE {
    NONE = 0,
    WEAPON = 1,
    ARMOR = 2,
    SHIELD = 3,
    HELMET = 4,
    BOOTS = 5,
    GLOVES = 6,
    RING = 7,
    NECKLACE = 8,
    BELT = 9,
    EARRING = 10,
    BRACELET = 11,
    MOUNT = 12,
    POTION = 13,
    QUEST = 14,
    MATERIAL = 15,
    SPECIAL = 16,
    MISC = 17
};

// Tipo de arma
enum class WeaponType : BYTE {
    NONE = 0,
    DAGGER = 1,
    SWORD_1H = 2,
    SWORD_2H = 3,
    AXE_1H = 4,
    AXE_2H = 5,
    SPEAR = 6,
    BOW = 7,
    CROSSBOW = 8,
    STAFF = 9,
    WAND = 10,
    MACE = 11,
    SHIELD = 12
};

// Classe de arma
enum class WeaponClass : BYTE {
    NONE = 0,
    WARRIOR = 1,
    MAGE = 2,
    ARCHER = 3,
    THIEF = 4,
    PALADIN = 5,
    ALL = 255
};

// Tipo de armadura
enum class ArmorType : BYTE {
    NONE = 0,
    LIGHT = 1,
    MEDIUM = 2,
    HEAVY = 3,
    ROBE = 4
};

// Raridade de item
enum class ItemRarity : BYTE {
    COMMON = 0,
    UNCOMMON = 1,
    RARE = 2,
    EPIC = 3,
    LEGENDARY = 4,
    MYTHIC = 5,
    ARTIFACT = 6,
    QUEST = 7
};

// Slots de equipamento
enum class EquipSlot : BYTE {
    NONE = 0,
    WEAPON = 1,
    SHIELD = 2,
    HELMET = 3,
    ARMOR = 4,
    PANTS = 5,
    GLOVES = 6,
    BOOTS = 7,
    RING1 = 8,
    RING2 = 9,
    NECKLACE = 10,
    EARRING1 = 11,
    EARRING2 = 12,
    BELT = 13,
    BRACELET1 = 14,
    BRACELET2 = 15,
    MOUNT = 16
};

// Efeitos de item
enum class ItemEffect : WORD {
    NONE = 0,
    STR_INCREASE = 1,
    DEX_INCREASE = 2,
    CON_INCREASE = 3,
    INT_INCREASE = 4,
    WIS_INCREASE = 5,
    MAX_HP_INCREASE = 6,
    MAX_MP_INCREASE = 7,
    ATTACK_INCREASE = 8,
    DEFENSE_INCREASE = 9,
    MAGIC_ATTACK_INCREASE = 10,
    MAGIC_DEFENSE_INCREASE = 11,
    CRITICAL_INCREASE = 12,
    ATTACK_SPEED_INCREASE = 13,
    MOVE_SPEED_INCREASE = 14,
    EXPERIENCE_INCREASE = 15,
    GOLD_INCREASE = 16,
    DROP_RATE_INCREASE = 17,
    HP_RECOVERY = 18,
    MP_RECOVERY = 19,
    HP_CONSUME = 20,
    MP_CONSUME = 21,
    POISON = 22,
    STUN = 23,
    FREEZE = 24,
    BURN = 25,
    SILENCE = 26,
    ELEMENTAL_FIRE = 27,
    ELEMENTAL_WATER = 28,
    ELEMENTAL_EARTH = 29,
    ELEMENTAL_WIND = 30,
    ELEMENTAL_HOLY = 31,
    ELEMENTAL_DARK = 32,
    CUSTOM = 1000
};

// Tipos de gemas
enum class GemType : BYTE {
    NONE = 0,
    RUBY = 1,      // Fogo
    SAPPHIRE = 2,  // Água
    EMERALD = 3,   // Terra
    TOPAZ = 4,     // Vento
    DIAMOND = 5,   // Luz
    ONYX = 6,      // Trevas
    AMETHYST = 7,  // Envenenamento
    AQUAMARINE = 8,// Congelamento
    GARNET = 9,    // Queimadura
    CORAL = 10,    // Silêncio
    PEARL = 11,    // Proteção
    JADE = 12,     // Velocidade
    AMBER = 13,    // Luz de vela
    OPAL = 14      // Multi-elemental
};

// Status de durabilidade
enum class DurabilityStatus : BYTE {
    NORMAL = 0,    // >30% - Normal
    LOW = 1,       // 10-30% - Baixa
    CRITICAL = 2,  // <10% - Crítica
    BROKEN = 3     // 0% - Quebrado
};

// Flags de itens (para controle de comportamento)
enum class ItemFlags : DWORD {
    NONE = 0x00000000,
    TRADEABLE = 0x00000001,       // Pode ser comercializado entre jogadores
    SELLABLE = 0x00000002,        // Pode ser vendido para NPCs
    DROPPABLE = 0x00000004,       // Pode ser descartado
    BANKABLE = 0x00000008,        // Pode ser armazenado no banco
    BREAKABLE = 0x00000010,       // Perde durabilidade com uso
    REFINABLE = 0x00000020,       // Pode ser refinado
    SOCKETABLE = 0x00000040,      // Pode receber gemas
    ENCHANTABLE = 0x00000080,     // Pode ser encantado
    BOUND_ON_PICKUP = 0x00000100, // Ligado ao personagem ao pegar
    BOUND_ON_EQUIP = 0x00000200,  // Ligado ao personagem ao equipar
    QUEST_ITEM = 0x00000400,      // Item de quest
    MATERIAL = 0x00000800,        // Material para crafting
    CURRENCY = 0x00001000,        // Moeda/recurso
    CONSUMABLE = 0x00002000,      // Consumível (desaparece ao usar)
    MAGICAL = 0x00004000,         // Possui propriedades mágicas
    UNIQUE = 0x00008000,          // Único (só pode ter um)
    VALUABLE = 0x00010000,        // Valioso (alerta ao descartar)
    SOULBOUND = 0x00020000,       // Vinculado à alma (não pode ser transferido)
    TEMPORARY = 0x00040000        // Temporário (expira com o tempo)
};

// Operador de bit para ItemFlags
inline ItemFlags operator|(ItemFlags a, ItemFlags b) {
    return static_cast<ItemFlags>(static_cast<DWORD>(a) | static_cast<DWORD>(b));
}

inline ItemFlags operator&(ItemFlags a, ItemFlags b) {
    return static_cast<ItemFlags>(static_cast<DWORD>(a) & static_cast<DWORD>(b));
}

inline ItemFlags& operator|=(ItemFlags& a, ItemFlags b) {
    a = a | b;
    return a;
}

inline bool operator!(ItemFlags a) {
    return static_cast<DWORD>(a) == 0;
}

// Modelo de item (template/definição)
/**
 * Classe WYDItemTemplate
 * 
 * Esta classe implementa a funcionalidade WYDItemTemplate conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDItemTemplate
 * 
 * Esta classe implementa a funcionalidade WYDItemTemplate conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemTemplate {
public:
    // /**
 * ItemTemplate
 * 
 * Implementa a funcionalidade ItemTemplate conforme especificação original.
 * @param id Parâmetro id
 * @param name Parâmetro name
 * @return Retorna Construtor
 */
 Construtor
    ItemTemplate(ItemID id, const std::string& name);
    
    // Getters
    /**
 * getId
 * 
 * Implementa a funcionalidade getId conforme especificação original.
 * @return Retorna ItemID
 */

    ItemID getId() const{ return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    /**
 * getCategory
 * 
 * Implementa a funcionalidade getCategory conforme especificação original.
 * @return Retorna ItemCategory
 */

    ItemCategory getCategory() const{ return category_; }
    /**
 * getRarity
 * 
 * Implementa a funcionalidade getRarity conforme especificação original.
 * @return Retorna ItemRarity
 */

    ItemRarity getRarity() const{ return rarity_; }
    /**
 * getEquipSlot
 * 
 * Implementa a funcionalidade getEquipSlot conforme especificação original.
 * @return Retorna EquipSlot
 */

    EquipSlot getEquipSlot() const{ return equipSlot_; }
    /**
 * getLevel
 * 
 * Implementa a funcionalidade getLevel conforme especificação original.
 * @return Retorna WORD
 */

    WORD getLevel() const{ return level_; }
    /**
 * getValue
 * 
 * Implementa a funcionalidade getValue conforme especificação original.
 * @return Retorna DWORD
 */

    DWORD getValue() const{ return value_; }
    /**
 * getWeight
 * 
 * Implementa a funcionalidade getWeight conforme especificação original.
 * @return Retorna WORD
 */

    WORD getWeight() const{ return weight_; }
    /**
 * getMaxStack
 * 
 * Implementa a funcionalidade getMaxStack conforme especificação original.
 * @return Retorna WORD
 */

    WORD getMaxStack() const{ return maxStack_; }
    /**
 * getMaxDurability
 * 
 * Implementa a funcionalidade getMaxDurability conforme especificação original.
 * @return Retorna WORD
 */

    WORD getMaxDurability() const{ return maxDurability_; }
    /**
 * getAttackMin
 * 
 * Implementa a funcionalidade getAttackMin conforme especificação original.
 * @return Retorna WORD
 */

    WORD getAttackMin() const{ return attackMin_; }
    /**
 * getAttackMax
 * 
 * Implementa a funcionalidade getAttackMax conforme especificação original.
 * @return Retorna WORD
 */

    WORD getAttackMax() const{ return attackMax_; }
    /**
 * getDefense
 * 
 * Implementa a funcionalidade getDefense conforme especificação original.
 * @return Retorna WORD
 */

    WORD getDefense() const{ return defense_; }
    /**
 * getRequiredStr
 * 
 * Implementa a funcionalidade getRequiredStr conforme especificação original.
 * @return Retorna BYTE
 */

    BYTE getRequiredStr() const{ return requiredStr_; }
    /**
 * getRequiredDex
 * 
 * Implementa a funcionalidade getRequiredDex conforme especificação original.
 * @return Retorna BYTE
 */

    BYTE getRequiredDex() const{ return requiredDex_; }
    /**
 * getRequiredCon
 * 
 * Implementa a funcionalidade getRequiredCon conforme especificação original.
 * @return Retorna BYTE
 */

    BYTE getRequiredCon() const{ return requiredCon_; }
    /**
 * getRequiredInt
 * 
 * Implementa a funcionalidade getRequiredInt conforme especificação original.
 * @return Retorna BYTE
 */

    BYTE getRequiredInt() const{ return requiredInt_; }
    /**
 * getRange
 * 
 * Implementa a funcionalidade getRange conforme especificação original.
 * @return Retorna WORD
 */

    WORD getRange() const{ return range_; }
    /**
 * getWeaponType
 * 
 * Implementa a funcionalidade getWeaponType conforme especificação original.
 * @return Retorna WeaponType
 */

    WeaponType getWeaponType() const{ return weaponType_; }
    /**
 * getArmorType
 * 
 * Implementa a funcionalidade getArmorType conforme especificação original.
 * @return Retorna ArmorType
 */

    ArmorType getArmorType() const{ return armorType_; }
    /**
 * getWeaponClass
 * 
 * Implementa a funcionalidade getWeaponClass conforme especificação original.
 * @return Retorna WeaponClass
 */

    WeaponClass getWeaponClass() const{ return weaponClass_; }
    /**
 * getFlags
 * 
 * Implementa a funcionalidade getFlags conforme especificação original.
 * @return Retorna ItemFlags
 */

    ItemFlags getFlags() const{ return flags_; }
    /**
 * getMaxSockets
 * 
 * Implementa a funcionalidade getMaxSockets conforme especificação original.
 * @return Retorna BYTE
 */

    BYTE getMaxSockets() const{ return maxSockets_; }
    /**
 * getRefineLevel
 * 
 * Implementa a funcionalidade getRefineLevel conforme especificação original.
 * @return Retorna BYTE
 */

    BYTE getRefineLevel() const{ return refineLevel_; }
    
    // Verificadores
    /**
 * isEquippable
 * 
 * Implementa a funcionalidade isEquippable conforme especificação original.
 * @return Retorna bool
 */

    bool isEquippable() const;
    /**
 * isWeapon
 * 
 * Implementa a funcionalidade isWeapon conforme especificação original.
 * @return Retorna bool
 */

    bool isWeapon() const;
    /**
 * isArmor
 * 
 * Implementa a funcionalidade isArmor conforme especificação original.
 * @return Retorna bool
 */

    bool isArmor() const;
    /**
 * isStackable
 * 
 * Implementa a funcionalidade isStackable conforme especificação original.
 * @return Retorna bool
 */

    bool isStackable() const;
    /**
 * isConsumable
 * 
 * Implementa a funcionalidade isConsumable conforme especificação original.
 * @return Retorna bool
 */

    bool isConsumable() const;
    /**
 * hasFlag
 * 
 * Implementa a funcionalidade hasFlag conforme especificação original.
 * @param flag Parâmetro flag
 * @return Retorna bool
 */

    bool hasFlag(ItemFlags flag) const;
    
    // Setters (para configuração de modelo)
    /**
 * setDescription
 * 
 * Implementa a funcionalidade setDescription conforme especificação original.
 * @param description Parâmetro description
 */

    void setDescription(const std::string& description){ description_ = description; }
    /**
 * setCategory
 * 
 * Implementa a funcionalidade setCategory conforme especificação original.
 * @param category Parâmetro category
 */

    void setCategory(ItemCategory category){ category_ = category; }
    /**
 * setRarity
 * 
 * Implementa a funcionalidade setRarity conforme especificação original.
 * @param rarity Parâmetro rarity
 */

    void setRarity(ItemRarity rarity){ rarity_ = rarity; }
    /**
 * setEquipSlot
 * 
 * Implementa a funcionalidade setEquipSlot conforme especificação original.
 * @param slot Parâmetro slot
 */

    void setEquipSlot(EquipSlot slot){ equipSlot_ = slot; }
    /**
 * setLevel
 * 
 * Implementa a funcionalidade setLevel conforme especificação original.
 * @param level Parâmetro level
 */

    void setLevel(WORD level){ level_ = level; }
    /**
 * setValue
 * 
 * Implementa a funcionalidade setValue conforme especificação original.
 * @param value Parâmetro value
 */

    void setValue(DWORD value){ value_ = value; }
    /**
 * setWeight
 * 
 * Implementa a funcionalidade setWeight conforme especificação original.
 * @param weight Parâmetro weight
 */

    void setWeight(WORD weight){ weight_ = weight; }
    /**
 * setMaxStack
 * 
 * Implementa a funcionalidade setMaxStack conforme especificação original.
 * @param maxStack Parâmetro maxStack
 */

    void setMaxStack(WORD maxStack){ maxStack_ = maxStack; }
    /**
 * setMaxDurability
 * 
 * Implementa a funcionalidade setMaxDurability conforme especificação original.
 * @param maxDurability Parâmetro maxDurability
 */

    void setMaxDurability(WORD maxDurability){ maxDurability_ = maxDurability; }
    /**
 * setAttack
 * 
 * Implementa a funcionalidade setAttack conforme especificação original.
 * @param min Parâmetro min
 * @param max Parâmetro max
 */

    void setAttack(WORD min, WORD max){ attackMin_ = min; attackMax_ = max; }
    /**
 * setDefense
 * 
 * Implementa a funcionalidade setDefense conforme especificação original.
 * @param defense Parâmetro defense
 */

    void setDefense(WORD defense){ defense_ = defense; }
    /**
 * setRequiredAttributes
 * 
 * Implementa a funcionalidade setRequiredAttributes conforme especificação original.
 * @param str Parâmetro str
 * @param dex Parâmetro dex
 * @param con Parâmetro con
 * @param intel Parâmetro intel
 */

    void setRequiredAttributes(BYTE str, BYTE dex, BYTE con, BYTE intel){
        requiredStr_ = str; requiredDex_ = dex; requiredCon_ = con; requiredInt_ = intel;
    }
    /**
 * setRange
 * 
 * Implementa a funcionalidade setRange conforme especificação original.
 * @param range Parâmetro range
 */

    void setRange(WORD range){ range_ = range; }
    /**
 * setWeaponType
 * 
 * Implementa a funcionalidade setWeaponType conforme especificação original.
 * @param type Parâmetro type
 */

    void setWeaponType(WeaponType type){ weaponType_ = type; }
    /**
 * setArmorType
 * 
 * Implementa a funcionalidade setArmorType conforme especificação original.
 * @param type Parâmetro type
 */

    void setArmorType(ArmorType type){ armorType_ = type; }
    /**
 * setWeaponClass
 * 
 * Implementa a funcionalidade setWeaponClass conforme especificação original.
 * @param weaponClass Parâmetro weaponClass
 */

    void setWeaponClass(WeaponClass weaponClass){ weaponClass_ = weaponClass; }
    /**
 * setFlags
 * 
 * Implementa a funcionalidade setFlags conforme especificação original.
 * @param flags Parâmetro flags
 */

    void setFlags(ItemFlags flags){ flags_ = flags; }
    /**
 * addFlag
 * 
 * Implementa a funcionalidade addFlag conforme especificação original.
 * @param flag Parâmetro flag
 */

    void addFlag(ItemFlags flag){ flags_ = flags_ | flag; }
    /**
 * removeFlag
 * 
 * Implementa a funcionalidade removeFlag conforme especificação original.
 * @param flag Parâmetro flag
 */

    void removeFlag(ItemFlags flag){ flags_ = static_cast<ItemFlags>(static_cast<DWORD>(flags_) & ~static_cast<DWORD>(flag)); }
    /**
 * setMaxSockets
 * 
 * Implementa a funcionalidade setMaxSockets conforme especificação original.
 * @param maxSockets Parâmetro maxSockets
 */

    void setMaxSockets(BYTE maxSockets){ maxSockets_ = maxSockets; }
    /**
 * setRefineLevel
 * 
 * Implementa a funcionalidade setRefineLevel conforme especificação original.
 * @param refineLevel Parâmetro refineLevel
 */

    void setRefineLevel(BYTE refineLevel){ refineLevel_ = refineLevel; }
    
    // Registro de efeitos
    /**
 * addEffect
 * 
 * Implementa a funcionalidade addEffect conforme especificação original.
 * @param effect Parâmetro effect
 * @param value Parâmetro value
 */

    void addEffect(ItemEffect effect, WORD value);
    /**
 * removeEffect
 * 
 * Implementa a funcionalidade removeEffect conforme especificação original.
 * @param effect Parâmetro effect
 */

    void removeEffect(ItemEffect effect);
    /**
 * getEffectValue
 * 
 * Implementa a funcionalidade getEffectValue conforme especificação original.
 * @param effect Parâmetro effect
 * @return Retorna WORD
 */

    WORD getEffectValue(ItemEffect effect) const;
    
    // Serialização
    /**
 * serialize
 * 
 * Implementa a funcionalidade serialize conforme especificação original.
 * @return Retorna std::string
 */

    std::string serialize() const;
    static std::unique_ptr<ItemTemplate> deserialize(const std::string& data);
    
    // Criação de instância
    std::unique_ptr<ItemInstance> createInstance() const;
    
private:
    ItemID id_;                    // ID único do item
    std::string name_;             // Nome do item
    std::string description_;      // Descrição do item
    ItemCategory category_;        // Categoria do item
    ItemRarity rarity_;            // Raridade do item
    EquipSlot equipSlot_;          // Slot de equipamento
    WORD level_;                   // Nível requerido
    DWORD value_;                  // Valor em ouro
    WORD weight_;                  // Peso do item
    WORD maxStack_;                // Máximo de itens por pilha
    WORD maxDurability_;           // Durabilidade máxima
    WORD attackMin_;               // Ataque mínimo
    WORD attackMax_;               // Ataque máximo
    WORD defense_;                 // Defesa
    BYTE requiredStr_;             // Força requerida
    BYTE requiredDex_;             // Destreza requerida
    BYTE requiredCon_;             // Constituição requerida
    BYTE requiredInt_;             // Inteligência requerida
    WORD range_;                   // Alcance (para armas)
    WeaponType weaponType_;        // Tipo de arma
    ArmorType armorType_;          // Tipo de armadura
    WeaponClass weaponClass_;      // Classe de arma
    ItemFlags flags_;              // Flags do item
    BYTE maxSockets_;              // Máximo de slots de gema
    BYTE refineLevel_;             // Nível de refinamento base
    
    // Efeitos do item
    std::unordered_map<ItemEffect, WORD> effects_;
};

// Instância de item (objeto real no jogo)
/**
 * Classe WYDItemInstance
 * 
 * Esta classe implementa a funcionalidade WYDItemInstance conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDItemInstance
 * 
 * Esta classe implementa a funcionalidade WYDItemInstance conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemInstance {
public:
    // Construtor//**
 * ItemInstance
 * 
 * Implementa a funcionalidade ItemInstance conforme especificação original.
 * @param template_ Parâmetro template_
 * @return Retorna Destrutor
 */
Destrutor
    ItemInstance(const ItemTemplate* template_);
    ItemInstance(ItemID templateId);
    ~ItemInstance();
    
    // Getters básicos
    /**
 * getInstanceId
 * 
 * Implementa a funcionalidade getInstanceId conforme especificação original.
 * @return Retorna DWORD
 */

    DWORD getInstanceId() const{ return instanceId_; }
    /**
 * getTemplateId
 * 
 * Implementa a funcionalidade getTemplateId conforme especificação original.
 * @return Retorna ItemID
 */

    ItemID getTemplateId() const{ return templateId_; }
    /**
 * getTemplate
 * 
 * Implementa a funcionalidade getTemplate conforme especificação original.
 * @return Retorna ItemTemplate
 */

    const ItemTemplate* getTemplate() const{ return template_; }
    
    // Properties
    /**
 * getDurability
 * 
 * Implementa a funcionalidade getDurability conforme especificação original.
 * @return Retorna WORD
 */

    WORD getDurability() const{ return durability_; }
    /**
 * getMaxDurability
 * 
 * Implementa a funcionalidade getMaxDurability conforme especificação original.
 * @return Retorna WORD
 */

    WORD getMaxDurability() const;
    /**
 * getAmount
 * 
 * Implementa a funcionalidade getAmount conforme especificação original.
 * @return Retorna WORD
 */

    WORD getAmount() const{ return amount_; }
    /**
 * getRefineLevel
 * 
 * Implementa a funcionalidade getRefineLevel conforme especificação original.
 * @return Retorna BYTE
 */

    BYTE getRefineLevel() const{ return refineLevel_; }
    /**
 * getSockets
 * 
 * Implementa a funcionalidade getSockets conforme especificação original.
 * @return Retorna BYTE
 */

    BYTE getSockets() const{ return sockets_; }
    /**
 * getGem
 * 
 * Implementa a funcionalidade getGem conforme especificação original.
 * @param socketIndex Parâmetro socketIndex
 * @return Retorna GemType
 */

    GemType getGem(BYTE socketIndex) const;
    /**
 * getDurabilityStatus
 * 
 * Implementa a funcionalidade getDurabilityStatus conforme especificação original.
 * @return Retorna DurabilityStatus
 */

    DurabilityStatus getDurabilityStatus() const;
    
    // Flags
    /**
 * isSoulbound
 * 
 * Implementa a funcionalidade isSoulbound conforme especificação original.
 * @return Retorna bool
 */

    bool isSoulbound() const{ return soulbound_; }
    /**
 * isIdentified
 * 
 * Implementa a funcionalidade isIdentified conforme especificação original.
 * @return Retorna bool
 */

    bool isIdentified() const{ return identified_; }
    /**
 * isLocked
 * 
 * Implementa a funcionalidade isLocked conforme especificação original.
 * @return Retorna bool
 */

    bool isLocked() const{ return locked_; }
    /**
 * isExpired
 * 
 * Implementa a funcionalidade isExpired conforme especificação original.
 * @return Retorna bool
 */

    bool isExpired() const;
    /**
 * isBlessed
 * 
 * Implementa a funcionalidade isBlessed conforme especificação original.
 * @return Retorna bool
 */

    bool isBlessed() const{ return blessed_; }
    /**
 * isUnique
 * 
 * Implementa a funcionalidade isUnique conforme especificação original.
 * @return Retorna bool
 */

    bool isUnique() const;
    
    // Atributos dinâmicos
    /**
 * getAttackMin
 * 
 * Implementa a funcionalidade getAttackMin conforme especificação original.
 * @return Retorna WORD
 */

    WORD getAttackMin() const;
    /**
 * getAttackMax
 * 
 * Implementa a funcionalidade getAttackMax conforme especificação original.
 * @return Retorna WORD
 */

    WORD getAttackMax() const;
    /**
 * getDefense
 * 
 * Implementa a funcionalidade getDefense conforme especificação original.
 * @return Retorna WORD
 */

    WORD getDefense() const;
    /**
 * getValue
 * 
 * Implementa a funcionalidade getValue conforme especificação original.
 * @return Retorna WORD
 */

    WORD getValue() const;
    /**
 * getWeight
 * 
 * Implementa a funcionalidade getWeight conforme especificação original.
 * @return Retorna WORD
 */

    WORD getWeight() const;
    
    // Efeitos
    /**
 * getEffectValue
 * 
 * Implementa a funcionalidade getEffectValue conforme especificação original.
 * @param effect Parâmetro effect
 * @return Retorna WORD
 */

    WORD getEffectValue(ItemEffect effect) const;
    /**
 * addEffect
 * 
 * Implementa a funcionalidade addEffect conforme especificação original.
 * @param effect Parâmetro effect
 * @param value Parâmetro value
 */

    void addEffect(ItemEffect effect, WORD value);
    /**
 * removeEffect
 * 
 * Implementa a funcionalidade removeEffect conforme especificação original.
 * @param effect Parâmetro effect
 */

    void removeEffect(ItemEffect effect);
    
    // Operações
    /**
 * setDurability
 * 
 * Implementa a funcionalidade setDurability conforme especificação original.
 * @param durability Parâmetro durability
 */

    void setDurability(WORD durability);
    /**
 * decreaseDurability
 * 
 * Implementa a funcionalidade decreaseDurability conforme especificação original.
 * @param 1 Parâmetro 1
 */

    void decreaseDurability(WORD amount = 1);
    /**
 * repairDurability
 * 
 * Implementa a funcionalidade repairDurability conforme especificação original.
 * @param 0 Parâmetro 0
 */

    void repairDurability(WORD amount = 0); // 0 = total
    /**
 * increaseAmount
 * 
 * Implementa a funcionalidade increaseAmount conforme especificação original.
 * @param amount Parâmetro amount
 * @return Retorna bool
 */

    bool increaseAmount(WORD amount);
    /**
 * decreaseAmount
 * 
 * Implementa a funcionalidade decreaseAmount conforme especificação original.
 * @param amount Parâmetro amount
 * @return Retorna bool
 */

    bool decreaseAmount(WORD amount);
    /**
 * setAmount
 * 
 * Implementa a funcionalidade setAmount conforme especificação original.
 * @param amount Parâmetro amount
 */

    void setAmount(WORD amount);
    /**
 * canStack
 * 
 * Implementa a funcionalidade canStack conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna bool
 */

    bool canStack(const ItemInstance* other) const;
    /**
 * stackWith
 * 
 * Implementa a funcionalidade stackWith conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna bool
 */

    bool stackWith(ItemInstance* other);
    /**
 * split
 * 
 * Implementa a funcionalidade split conforme especificação original.
 * @param amount Parâmetro amount
 */

    void split(WORD amount);
    /**
 * addGem
 * 
 * Implementa a funcionalidade addGem conforme especificação original.
 * @param socketIndex Parâmetro socketIndex
 * @param gemType Parâmetro gemType
 * @return Retorna bool
 */

    bool addGem(BYTE socketIndex, GemType gemType);
    /**
 * removeGem
 * 
 * Implementa a funcionalidade removeGem conforme especificação original.
 * @param socketIndex Parâmetro socketIndex
 * @return Retorna bool
 */

    bool removeGem(BYTE socketIndex);
    /**
 * setSoulbound
 * 
 * Implementa a funcionalidade setSoulbound conforme especificação original.
 * @param value Parâmetro value
 */

    void setSoulbound(bool value);
    /**
 * setIdentified
 * 
 * Implementa a funcionalidade setIdentified conforme especificação original.
 * @param value Parâmetro value
 */

    void setIdentified(bool value);
    /**
 * setLocked
 * 
 * Implementa a funcionalidade setLocked conforme especificação original.
 * @param value Parâmetro value
 */

    void setLocked(bool value);
    /**
 * setBlessed
 * 
 * Implementa a funcionalidade setBlessed conforme especificação original.
 * @param value Parâmetro value
 */

    void setBlessed(bool value);
    /**
 * refine
 * 
 * Implementa a funcionalidade refine conforme especificação original.
 * @return Retorna bool
 */

    bool refine();
    /**
 * refineForced
 * 
 * Implementa a funcionalidade refineForced conforme especificação original.
 * @return Retorna bool
 */

    bool refineForced(); // Forçar refinamento (não falha, para GM)
    /**
 * canRefine
 * 
 * Implementa a funcionalidade canRefine conforme especificação original.
 * @return Retorna bool
 */

    bool canRefine() const;
    /**
 * canUse
 * 
 * Implementa a funcionalidade canUse conforme especificação original.
 * @return Retorna bool
 */

    bool canUse() const;
    
    // Serialização
    /**
 * serialize
 * 
 * Implementa a funcionalidade serialize conforme especificação original.
 * @return Retorna std::string
 */

    std::string serialize() const;
    static std::unique_ptr<ItemInstance> deserialize(const std::string& data);
    
    // Conversão para/de estrutura de pacote
    /**
 * fromPacketData
 * 
 * Implementa a funcionalidade fromPacketData conforme especificação original.
 * @param packetItem Parâmetro packetItem
 */

    void fromPacketData(const wyd::Item& packetItem);
    /**
 * toPacketData
 * 
 * Implementa a funcionalidade toPacketData conforme especificação original.
 * @return Retorna wyd::Item
 */

    wyd::Item toPacketData() const;
    
    // Outras funções
    /**
 * getFullName
 * 
 * Implementa a funcionalidade getFullName conforme especificação original.
 * @return Retorna std::string
 */

    std::string getFullName() const;
    /**
 * getColoredName
 * 
 * Implementa a funcionalidade getColoredName conforme especificação original.
 * @return Retorna std::string
 */

    std::string getColoredName() const; // Nome com código de cor baseado em raridade
    
private:
    DWORD instanceId_;             // ID único da instância
    ItemID templateId_;            // ID do template
    const ItemTemplate* template_; // Pointer para o template (nulo se o item não existir)
    
    // Propriedades variáveis
    WORD durability_;              // Durabilidade atual
    WORD amount_;                  // Quantidade (para itens empilháveis)
    BYTE refineLevel_;             // Nível de refinamento (0 a 15)
    BYTE sockets_;                 // Número de sockets ativos
    GemType gems_[6];              // Gemas encaixadas
    
    // Flags específicas de instância
    bool soulbound_;               // Vinculado ao personagem
    bool identified_;              // Identificado
    bool locked_;                  // Bloqueado (não pode ser descartado)
    bool blessed_;                 // Abençoado (não quebra ao falhar refinamento)
    
    // Data de expiração (0 = sem expiração)
    DWORD expirationTime_;
    
    // Efeitos específicos da instância (encantamentos)
    std::unordered_map<ItemEffect, WORD> instanceEffects_;
    
    // ID do próximo item
    static std::atomic<DWORD> nextInstanceId_;
};

// Inventário (genérico, pode ser usado para personagem, banco, armazém, etc.)
/**
 * Classe WYDInventory
 * 
 * Esta classe implementa a funcionalidade WYDInventory conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDInventory
 * 
 * Esta classe implementa a funcionalidade WYDInventory conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Inventory {
public:
    // Construtor//**
 * Inventory
 * 
 * Implementa a funcionalidade Inventory conforme especificação original.
 * @param capacity Parâmetro capacity
 * @return Retorna Destrutor
 */
Destrutor
    Inventory(WORD capacity);
    ~Inventory();
    
    // Capacidade
    /**
 * getCapacity
 * 
 * Implementa a funcionalidade getCapacity conforme especificação original.
 * @return Retorna WORD
 */

    WORD getCapacity() const{ return capacity_; }
    /**
 * getUsedSlots
 * 
 * Implementa a funcionalidade getUsedSlots conforme especificação original.
 * @return Retorna WORD
 */

    WORD getUsedSlots() const;
    /**
 * getEmptySlots
 * 
 * Implementa a funcionalidade getEmptySlots conforme especificação original.
 * @return Retorna WORD
 */

    WORD getEmptySlots() const{ return capacity_ - getUsedSlots(); }
    /**
 * isFull
 * 
 * Implementa a funcionalidade isFull conforme especificação original.
 * @return Retorna bool
 */

    bool isFull() const{ return getEmptySlots() == 0; }
    /**
 * isEmpty
 * 
 * Implementa a funcionalidade isEmpty conforme especificação original.
 * @return Retorna bool
 */

    bool isEmpty() const{ return getUsedSlots() == 0; }
    
    // Acesso a itens
    /**
 * getItem
 * 
 * Implementa a funcionalidade getItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna ItemInstance
 */

    ItemInstance* getItem(WORD slot) const;
    std::vector<ItemInstance*> getAllItems() const;
    
    // Operações com itens
    /**
 * addItem
 * 
 * Implementa a funcionalidade addItem conforme especificação original.
 * @param item Parâmetro item
 * @param 0xFFFF Parâmetro 0xFFFF
 * @return Retorna bool
 */

    bool addItem(std::unique_ptr<ItemInstance> item, WORD preferredSlot = 0xFFFF);
    /**
 * removeItem
 * 
 * Implementa a funcionalidade removeItem conforme especificação original.
 * @param slot Parâmetro slot
 * @param 0 Parâmetro 0
 * @return Retorna bool
 */

    bool removeItem(WORD slot, WORD amount = 0);
    /**
 * moveItem
 * 
 * Implementa a funcionalidade moveItem conforme especificação original.
 * @param fromSlot Parâmetro fromSlot
 * @param toSlot Parâmetro toSlot
 * @return Retorna bool
 */

    bool moveItem(WORD fromSlot, WORD toSlot);
    /**
 * swapItems
 * 
 * Implementa a funcionalidade swapItems conforme especificação original.
 * @param slotA Parâmetro slotA
 * @param slotB Parâmetro slotB
 * @return Retorna bool
 */

    bool swapItems(WORD slotA, WORD slotB);
    /**
 * stackItems
 * 
 * Implementa a funcionalidade stackItems conforme especificação original.
 * @param sourceSlot Parâmetro sourceSlot
 * @param targetSlot Parâmetro targetSlot
 * @return Retorna bool
 */

    bool stackItems(WORD sourceSlot, WORD targetSlot);
    /**
 * splitItem
 * 
 * Implementa a funcionalidade splitItem conforme especificação original.
 * @param slot Parâmetro slot
 * @param amount Parâmetro amount
 * @param targetSlot Parâmetro targetSlot
 * @return Retorna bool
 */

    bool splitItem(WORD slot, WORD amount, WORD targetSlot);
    
    // Pesquisa
    /**
 * findItemByTemplateId
 * 
 * Implementa a funcionalidade findItemByTemplateId conforme especificação original.
 * @param templateId Parâmetro templateId
 * @return Retorna WORD
 */

    WORD findItemByTemplateId(ItemID templateId) const;
    /**
 * findItemByInstanceId
 * 
 * Implementa a funcionalidade findItemByInstanceId conforme especificação original.
 * @param instanceId Parâmetro instanceId
 * @return Retorna WORD
 */

    WORD findItemByInstanceId(DWORD instanceId) const;
    /**
 * findEmptySlot
 * 
 * Implementa a funcionalidade findEmptySlot conforme especificação original.
 * @return Retorna WORD
 */

    WORD findEmptySlot() const;
    
    // Verificações
    /**
 * hasItem
 * 
 * Implementa a funcionalidade hasItem conforme especificação original.
 * @param templateId Parâmetro templateId
 * @param 1 Parâmetro 1
 * @return Retorna bool
 */

    bool hasItem(ItemID templateId, WORD amount = 1) const;
    /**
 * hasSpace
 * 
 * Implementa a funcionalidade hasSpace conforme especificação original.
 * @param templateId Parâmetro templateId
 * @param 1 Parâmetro 1
 * @return Retorna bool
 */

    bool hasSpace(ItemID templateId, WORD amount = 1) const;
    /**
 * isSlotValid
 * 
 * Implementa a funcionalidade isSlotValid conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool isSlotValid(WORD slot) const{ return slot < capacity_; }
    
    // Serialização
    /**
 * serialize
 * 
 * Implementa a funcionalidade serialize conforme especificação original.
 * @return Retorna std::string
 */

    std::string serialize() const;
    static std::unique_ptr<Inventory> deserialize(const std::string& data);
    
    // Eventos
    void setOnItemAddedCallback(std::function<void(WORD, ItemInstance*)> callback) {
        onItemAdded_ = callback;
    }
    
    void setOnItemRemovedCallback(std::function<void(WORD, ItemInstance*)> callback) {
        onItemRemoved_ = callback;
    }
    
    void setOnItemMovedCallback(std::function<void(WORD, WORD, ItemInstance*)> callback) {
        onItemMoved_ = callback;
    }
    
private:
    WORD capacity_;        // Capacidade total do inventário
    std::vector<std::unique_ptr<ItemInstance>> items_; // Itens (slot -> item)
    
    // Callbacks de evento
    std::function<void(WORD, ItemInstance*)> onItemAdded_;
    std::function<void(WORD, ItemInstance*)> onItemRemoved_;
    std::function<void(WORD, WORD, ItemInstance*)> onItemMoved_;
    
    // Mutex para thread safety
    mutable std::mutex mutex_;
};

// Equipamento (inventário especial para itens equipados)
/**
 * Classe WYDEquipment
 * 
 * Esta classe implementa a funcionalidade WYDEquipment conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDEquipment
 * 
 * Esta classe implementa a funcionalidade WYDEquipment conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Equipment {
public:
    // Construtor//**
 * Equipment
 * 
 * Implementa a funcionalidade Equipment conforme especificação original.
 * @return Retorna Destrutor
 */
Destrutor
    Equipment();
    ~Equipment();
    
    // Acesso a itens
    /**
 * getItem
 * 
 * Implementa a funcionalidade getItem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna ItemInstance
 */

    ItemInstance* getItem(EquipSlot slot) const;
    std::vector<ItemInstance*> getAllItems() const;
    
    // Operações com itens
    /**
 * equipItem
 * 
 * Implementa a funcionalidade equipItem conforme especificação original.
 * @param item Parâmetro item
 * @return Retorna bool
 */

    bool equipItem(std::unique_ptr<ItemInstance> item);
    std::unique_ptr<ItemInstance> unequipItem(EquipSlot slot);
    
    // Verificações
    /**
 * isSlotEmpty
 * 
 * Implementa a funcionalidade isSlotEmpty conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    bool isSlotEmpty(EquipSlot slot) const;
    /**
 * canEquip
 * 
 * Implementa a funcionalidade canEquip conforme especificação original.
 * @param item Parâmetro item
 * @return Retorna bool
 */

    bool canEquip(const ItemInstance* item) const;
    
    // Serialização
    /**
 * serialize
 * 
 * Implementa a funcionalidade serialize conforme especificação original.
 * @return Retorna std::string
 */

    std::string serialize() const;
    static std::unique_ptr<Equipment> deserialize(const std::string& data);
    
    // Eventos
    void setOnItemEquippedCallback(std::function<void(EquipSlot, ItemInstance*)> callback) {
        onItemEquipped_ = callback;
    }
    
    void setOnItemUnequippedCallback(std::function<void(EquipSlot, ItemInstance*)> callback) {
        onItemUnequipped_ = callback;
    }
    
private:
    std::unordered_map<EquipSlot, std::unique_ptr<ItemInstance>> equippedItems_;
    
    // Callbacks de evento
    std::function<void(EquipSlot, ItemInstance*)> onItemEquipped_;
    std::function<void(EquipSlot, ItemInstance*)> onItemUnequipped_;
    
    // Mutex para thread safety
    mutable std::mutex mutex_;
};

// Gerenciador de itens
/**
 * Classe WYDItemManager
 * 
 * Esta classe implementa a funcionalidade WYDItemManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDItemManager
 * 
 * Esta classe implementa a funcionalidade WYDItemManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemManager {
public:
    // Singleton
    static ItemManager& getInstance();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @return Retorna bool
 */

    bool initialize();
    /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 */

    void shutdown();
    
    // Carregamento de dados
    /**
 * loadItemTemplates
 * 
 * Implementa a funcionalidade loadItemTemplates conforme especificação original.
 * @param filename Parâmetro filename
 * @return Retorna bool
 */

    bool loadItemTemplates(const std::string& filename);
    /**
 * loadItemEffects
 * 
 * Implementa a funcionalidade loadItemEffects conforme especificação original.
 * @param filename Parâmetro filename
 * @return Retorna bool
 */

    bool loadItemEffects(const std::string& filename);
    /**
 * saveItemTemplates
 * 
 * Implementa a funcionalidade saveItemTemplates conforme especificação original.
 * @param filename Parâmetro filename
 * @return Retorna bool
 */

    bool saveItemTemplates(const std::string& filename);
    
    // Acesso a templates
    /**
 * getItemTemplate
 * 
 * Implementa a funcionalidade getItemTemplate conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna ItemTemplate
 */

    const ItemTemplate* getItemTemplate(ItemID id) const;
    std::vector<const ItemTemplate*> getItemTemplatesByCategory(ItemCategory category) const;
    std::vector<const ItemTemplate*> getItemTemplatesByLevel(WORD level, WORD maxLevel = 0) const;
    
    // Criação de itens
    std::unique_ptr<ItemInstance> createItem(ItemID templateId, WORD amount = 1);
    std::unique_ptr<ItemInstance> createRandomItem(ItemCategory category, WORD level, ItemRarity minRarity = ItemRarity::COMMON);
    
    // Registro/remoção de templates
    /**
 * registerItemTemplate
 * 
 * Implementa a funcionalidade registerItemTemplate conforme especificação original.
 * @param itemTemplate Parâmetro itemTemplate
 * @return Retorna bool
 */

    bool registerItemTemplate(std::unique_ptr<ItemTemplate> itemTemplate);
    /**
 * unregisterItemTemplate
 * 
 * Implementa a funcionalidade unregisterItemTemplate conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna bool
 */

    bool unregisterItemTemplate(ItemID id);
    
    // Gerenciamento de templates
    bool modifyItemTemplate(ItemID id, std::function<void(ItemTemplate*)> modifier);
    
    // Verificação, validação e reparo de itens
    /**
 * isValidItem
 * 
 * Implementa a funcionalidade isValidItem conforme especificação original.
 * @param item Parâmetro item
 * @return Retorna bool
 */

    bool isValidItem(const ItemInstance* item) const;
    /**
 * repairInvalidItem
 * 
 * Implementa a funcionalidade repairInvalidItem conforme especificação original.
 * @param item Parâmetro item
 * @return Retorna bool
 */

    bool repairInvalidItem(ItemInstance* item) const;
    
    // Utilitários
    /**
 * getItemColor
 * 
 * Implementa a funcionalidade getItemColor conforme especificação original.
 * @param rarity Parâmetro rarity
 * @return Retorna std::string
 */

    std::string getItemColor(ItemRarity rarity) const;
    /**
 * getItemEffectDescription
 * 
 * Implementa a funcionalidade getItemEffectDescription conforme especificação original.
 * @param effect Parâmetro effect
 * @param value Parâmetro value
 * @return Retorna std::string
 */

    std::string getItemEffectDescription(ItemEffect effect, WORD value) const;
    
private:
    // Construtor/Destrutor privados (singleton)
    ItemManager();
    ~ItemManager();
    
    // Templates de itens
    std::unordered_map<ItemID, std::unique_ptr<ItemTemplate>> itemTemplates_;
    
    // Índices para busca rápida
    std::unordered_map<ItemCategory, std::vector<ItemID>> itemsByCategory_;
    std::unordered_map<WORD, std::vector<ItemID>> itemsByLevel_;
    
    // Descrições de efeitos
    std::unordered_map<ItemEffect, std::string> effectDescriptions_;
    
    // Cores de raridade
    std::unordered_map<ItemRarity, std::string> rarityColors_;
    
    // Flag de inicialização
    bool initialized_;
    
    // Mutex para thread safety
    mutable std::mutex mutex_;
};

} // namespace game
} // namespace wyd

#endif // ITEM_SYSTEM_H

} // namespace wydbr
