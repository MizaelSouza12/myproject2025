#ifndef WYDSTUDIO_ITEM_LIST_HANDLER_H
#define WYDSTUDIO_ITEM_LIST_HANDLER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include <functional>
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace binary {

/**
 * @brief Estrutura que representa um atributo de item
 */
struct ItemAttribute {
    enum class Type {
        NONE,
        STRENGTH,
        DEXTERITY,
        INTELLIGENCE,
        CONSTITUTION,
        WISDOM,
        HP,
        MP,
        ATTACK,
        DEFENSE,
        ATTACK_SPEED,
        MOVE_SPEED,
        MAGIC_ATTACK,
        MAGIC_DEFENSE,
        CRITICAL_RATE,
        CRITICAL_DAMAGE,
        ACCURACY,
        EVASION,
        HP_REGEN,
        MP_REGEN,
        ELEMENTAL_ATTACK,
        ELEMENTAL_DEFENSE,
        REFLECT_DAMAGE,
        ABSORB_HP,
        ABSORB_MP,
        SPECIAL_EFFECT
    };

    enum class EffectType {
        NONE,
        POISON,
        PARALYZE,
        SLOW,
        STUN,
        SILENCE,
        BLIND,
        INVISIBLE,
        IMMUNE_PHYSICAL,
        IMMUNE_MAGICAL,
        DISPEL_DEBUFFS,
        INVULNERABLE,
        SKILL_ENHANCE,
        SKILL_COST_REDUCE,
        COOLDOWN_REDUCE,
        ITEM_DROP_RATE,
        EXP_RATE,
        GOLD_RATE
    };

    Type type;
    int16_t value;
    uint8_t level;      // Usado para requisitos de nível
    uint8_t probability; // Probabilidade de aparecer (0-100)
    EffectType effectType;
    uint32_t effectDuration; // Duração em milissegundos
    float effectValue;    // Valor para efeitos especiais

    ItemAttribute() : type(Type::NONE), value(0), level(0), probability(100), 
                     effectType(EffectType::NONE), effectDuration(0), effectValue(0.0f) {}
};

/**
 * @brief Estrutura que representa um item
 */
struct Item {
    enum class Type {
        NONE,
        WEAPON,
        ARMOR,
        HELMET,
        SHIELD,
        BOOTS,
        GLOVES,
        BELT,
        NECKLACE,
        EARRING,
        RING,
        CLOAK,
        POTION,
        SCROLL,
        QUEST_ITEM,
        MATERIAL,
        MOUNT,
        PET,
        SKILLBOOK,
        SPECIAL
    };

    enum class Rarity {
        COMMON,
        UNCOMMON,
        RARE,
        EPIC,
        LEGENDARY,
        MYTHIC,
        UNIQUE,
        SET_ITEM
    };

    enum class Element {
        NONE,
        FIRE,
        WATER,
        EARTH,
        WIND,
        LIGHT,
        DARK
    };

    enum class Flag {
        NONE              = 0x00000000,
        TRADEABLE         = 0x00000001,
        STORABLE          = 0x00000002,
        DROPPABLE         = 0x00000004,
        SELLABLE          = 0x00000008,
        ETERNAL           = 0x00000010, // Não pode ser destruído
        UNIQUE_EQUIP      = 0x00000020, // Só pode equipar um
        BOUND_ON_PICKUP   = 0x00000040, // Liga ao personagem ao pegar
        BOUND_ON_EQUIP    = 0x00000080, // Liga ao personagem ao equipar
        QUEST_ITEM        = 0x00000100, // Item de quest
        STACKABLE         = 0x00000200, // Pode empilhar
        UPGRADEABLE       = 0x00000400, // Pode ser melhorado
        REFINABLE         = 0x00000800, // Pode ser refinado
        ENCHANTABLE       = 0x00001000, // Pode ser encantado
        SOCKET_ITEM       = 0x00002000, // Item com soquetes
        GEM               = 0x00004000, // Gema para soquete
        SET_PIECE         = 0x00008000, // Parte de um conjunto
        CONSUMABLE        = 0x00010000, // Item consumível
        DECOMPOSABLE      = 0x00020000, // Pode ser decomposto
        REPAIRABLE        = 0x00040000, // Pode ser reparado
        DURABILITY        = 0x00080000, // Tem durabilidade
        CLASS_RESTRICTED  = 0x00100000, // Restrito a certas classes
        LEVEL_RESTRICTED  = 0x00200000, // Restrito por nível
        TIME_LIMITED      = 0x00400000, // Expira após certo tempo
        SELLABLE_TO_NPC   = 0x00800000, // Pode ser vendido para NPC
        LORE_ITEM         = 0x01000000, // Item de história do jogo
        EVENT_ITEM        = 0x02000000, // Item de evento
        VIP_ITEM          = 0x04000000, // Item de VIP
        PVP_ITEM          = 0x08000000, // Item de PVP
        GUILD_ITEM        = 0x10000000, // Item de guild
        TRANSFORMATION    = 0x20000000, // Item de transformação
        MOUNT             = 0x40000000, // Item de montaria
        PET               = 0x80000000  // Item de mascote
    };

    uint32_t id;                       // ID único do item
    std::string internalName;          // Nome interno para referência
    Type type;                         // Tipo do item
    Rarity rarity;                     // Raridade
    Element element;                   // Elemento
    uint32_t flags;                    // Combinação de flags
    
    uint16_t maxStackSize;             // Tamanho máximo da pilha (para itens empilháveis)
    uint16_t maxDurability;            // Durabilidade máxima
    
    uint32_t buyPrice;                 // Preço de compra
    uint32_t sellPrice;                // Preço de venda
    
    uint16_t requiredLevel;            // Nível mínimo para usar
    uint16_t requiredClass;            // Bitmap de classes que podem usar (0 = todas)
    
    uint8_t minSocketCount;            // Número mínimo de soquetes
    uint8_t maxSocketCount;            // Número máximo de soquetes
    
    uint8_t minUpgradeLevel;           // Nível mínimo de upgrade
    uint8_t maxUpgradeLevel;           // Nível máximo de upgrade
    
    uint32_t setID;                    // ID do conjunto (se for parte de um conjunto)
    
    uint32_t modelID;                  // ID do modelo 3D
    uint32_t textureID;                // ID da textura
    uint32_t iconID;                   // ID do ícone
    
    uint32_t effectID;                 // ID do efeito visual
    uint32_t soundID;                  // ID do som
    
    uint32_t cooldown;                 // Tempo de recarga em ms
    uint32_t duration;                 // Duração de efeitos em ms
    
    std::vector<ItemAttribute> baseAttributes;   // Atributos base fixos
    std::vector<ItemAttribute> randomAttributes; // Pool de atributos aleatórios
    
    uint32_t dropChance;               // Chance de drop (0-100000)
    uint32_t weight;                   // Peso do item

    Item() : id(0), type(Type::NONE), rarity(Rarity::COMMON), element(Element::NONE),
            flags(0), maxStackSize(1), maxDurability(0), buyPrice(0), sellPrice(0),
            requiredLevel(0), requiredClass(0), minSocketCount(0), maxSocketCount(0),
            minUpgradeLevel(0), maxUpgradeLevel(0), setID(0), modelID(0), textureID(0),
            iconID(0), effectID(0), soundID(0), cooldown(0), duration(0), dropChance(0),
            weight(0) {}
            
    // Métodos de conveniência
    bool hasFlag(Flag flag) const {
        return (flags & static_cast<uint32_t>(flag)) != 0;
    }
    
    void setFlag(Flag flag, bool value) {
        if (value) {
            flags |= static_cast<uint32_t>(flag);
        } else {
            flags &= ~static_cast<uint32_t>(flag);
        }
    }
    
    bool isWeapon() const { return type == Type::WEAPON; }
    bool isArmor() const { 
        return type == Type::ARMOR || type == Type::HELMET || 
               type == Type::SHIELD || type == Type::BOOTS || 
               type == Type::GLOVES || type == Type::BELT; 
    }
    bool isAccessory() const {
        return type == Type::NECKLACE || type == Type::EARRING || 
               type == Type::RING || type == Type::CLOAK;
    }
    bool isConsumable() const {
        return type == Type::POTION || type == Type::SCROLL || 
               hasFlag(Flag::CONSUMABLE);
    }
    bool isEquippable() const {
        return isWeapon() || isArmor() || isAccessory();
    }
};

/**
 * @brief Estrutura que representa a coleção completa de itens
 */
struct ItemList {
    uint32_t version;                              // Versão do arquivo
    std::unordered_map<uint32_t, Item> items;      // Mapa de ID para Item
    std::unordered_map<std::string, uint32_t> nameToId; // Mapa de nome para ID
    
    // Metadados e validação
    uint32_t checksum;                             // Checksum para validação
    uint32_t lastUpdate;                           // Timestamp da última atualização
    std::string formatVersion;                     // Versão do formato
    
    ItemList() : version(0), checksum(0), lastUpdate(0) {}
};

/**
 * @brief Filtro para busca de itens
 */
struct ItemFilter {
    std::optional<Item::Type> type;
    std::optional<Item::Rarity> rarity;
    std::optional<Item::Element> element;
    std::optional<uint32_t> minLevel;
    std::optional<uint32_t> maxLevel;
    std::optional<uint32_t> classRestriction;
    std::optional<Item::Flag> hasFlag;
    std::optional<Item::Flag> doesntHaveFlag;
    std::optional<uint32_t> minPrice;
    std::optional<uint32_t> maxPrice;
    std::optional<std::string> nameContains;
    std::optional<uint32_t> setID;
    std::optional<uint32_t> modelID;
    std::optional<ItemAttribute::Type> hasAttribute;
};

/**
 * @brief Manipulador para arquivos ItemList.bin
 */
class ItemListHandler {
public:
    using ItemChangedCallback = std::function<void(uint32_t itemId, const Item& oldItem, const Item& newItem)>;
    using ItemRemovedCallback = std::function<void(uint32_t itemId, const Item& removedItem)>;
    using ItemAddedCallback = std::function<void(uint32_t itemId, const Item& addedItem)>;
    
    ItemListHandler();
    ~ItemListHandler();
    
    // Carregamento e salvamento
    bool loadFromBinary(const std::string& filePath);
    bool loadFromJSON(const std::string& filePath);
    bool saveToBinary(const std::string& filePath);
    bool saveToJSON(const std::string& filePath);
    
    // Acesso à lista de itens
    const ItemList& getItemList() const { return m_itemList; }
    ItemList& getItemListMutable() { return m_itemList; }
    
    // Operações CRUD básicas
    bool addItem(const Item& item);
    bool updateItem(uint32_t itemId, const Item& item);
    bool removeItem(uint32_t itemId);
    
    // Operações em lote
    size_t addItems(const std::vector<Item>& items);
    size_t updateItems(const std::unordered_map<uint32_t, Item>& items);
    size_t removeItems(const std::vector<uint32_t>& itemIds);
    
    // Operações de busca
    const Item* findItem(uint32_t itemId) const;
    const Item* findItemByName(const std::string& internalName) const;
    std::vector<Item> findItems(const ItemFilter& filter) const;
    
    // Métodos específicos
    std::vector<Item> findItemsByType(Item::Type type) const;
    std::vector<Item> findItemsByRarity(Item::Rarity rarity) const;
    std::vector<Item> findItemsByLevel(uint32_t minLevel, uint32_t maxLevel) const;
    std::vector<Item> findItemsForClass(uint32_t classId) const;
    std::vector<Item> findItemsInSet(uint32_t setId) const;
    std::vector<Item> findWeapons() const;
    std::vector<Item> findArmor() const;
    std::vector<Item> findAccessories() const;
    std::vector<Item> findConsumables() const;
    std::vector<Item> findMaterials() const;
    std::vector<Item> findQuestItems() const;
    
    // Eventos
    void setItemChangedCallback(ItemChangedCallback callback) { m_itemChangedCallback = callback; }
    void setItemRemovedCallback(ItemRemovedCallback callback) { m_itemRemovedCallback = callback; }
    void setItemAddedCallback(ItemAddedCallback callback) { m_itemAddedCallback = callback; }
    
    // Estatísticas
    size_t getItemCount() const { return m_itemList.items.size(); }
    std::unordered_map<Item::Type, size_t> getItemTypeDistribution() const;
    std::unordered_map<Item::Rarity, size_t> getItemRarityDistribution() const;
    
    // Validação
    bool validateItemList();
    bool validateItem(const Item& item, std::string& error);
    bool itemExists(uint32_t itemId) const;
    
    // Erro handling
    std::string getLastError() const { return m_lastError; }
    
    // Métodos de pesquisa avançada
    std::vector<Item> findItemsWithAttribute(ItemAttribute::Type attrType) const;
    std::vector<Item> findItemsWithAttributeValue(ItemAttribute::Type attrType, int16_t minValue) const;
    std::vector<Item> findUpgradableItems() const;
    std::vector<Item> findItemsWithSockets() const;
    
    // Utilitários
    uint32_t generateUniqueItemId() const;
    void rebuildNameToIdMap();
    bool isValidItemType(Item::Type type) const;
    bool isValidRarity(Item::Rarity rarity) const;
    bool isValidElement(Item::Element element) const;
    static std::string itemTypeToString(Item::Type type);
    static std::string rarityToString(Item::Rarity rarity);
    static std::string elementToString(Item::Element element);
    static std::string attributeTypeToString(ItemAttribute::Type type);
    static Item createEmptyItem();

private:
    ItemList m_itemList;
    std::string m_lastError;
    std::string m_lastLoadedFile;
    bool m_modified;
    
    // Callbacks para notificação de mudanças
    ItemChangedCallback m_itemChangedCallback;
    ItemRemovedCallback m_itemRemovedCallback;
    ItemAddedCallback m_itemAddedCallback;
    
    // Métodos internos
    bool validateItemListStructure();
    bool updateItemReferences();
    bool recalculateChecksum();
    uint32_t calculateItemListChecksum() const;
    
    // Parsing e serialização
    bool parseBinaryItemList(const std::vector<uint8_t>& data);
    bool parseJSONItemList(const std::string& jsonData);
    std::vector<uint8_t> serializeToBinary() const;
    std::string serializeToJSON() const;
    
    // Helpers
    Item parseItemFromBinary(const std::vector<uint8_t>& data, size_t& offset);
    std::vector<uint8_t> serializeItemToBinary(const Item& item) const;
    void fireItemChangedEvent(uint32_t itemId, const Item& oldItem, const Item& newItem);
    void fireItemRemovedEvent(uint32_t itemId, const Item& removedItem);
    void fireItemAddedEvent(uint32_t itemId, const Item& addedItem);
};

} // namespace binary
} // namespace wydstudio

#endif // WYDSTUDIO_ITEM_LIST_HANDLER_H