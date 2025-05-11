#ifndef WYDSTUDIO_ITEM_STUDIO_H
#define WYDSTUDIO_ITEM_STUDIO_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>
#include <chrono>
#include <nlohmann/json.hpp>
#include "../core/WYDStudioFacade.h"
#include "../binary/ItemListHandler.h"

namespace wydstudio {
namespace ui {

using json = nlohmann::json;

/**
 * @brief Tipo de item
 */
enum class ItemType {
    WEAPON,     // Arma
    ARMOR,      // Armadura
    ACCESSORY,  // Acessório
    POTION,     // Poção
    MATERIAL,   // Material
    QUEST,      // Item de quest
    OTHER       // Outro
};

/**
 * @brief Raridade de item
 */
enum class ItemRarity {
    COMMON,     // Comum
    UNCOMMON,   // Incomum
    RARE,       // Raro
    EPIC,       // Épico
    LEGENDARY,  // Lendário
    MYTHIC      // Mítico
};

/**
 * @brief Slot de item
 */
enum class ItemSlot {
    NONE,        // Sem slot específico
    HEAD,        // Cabeça
    NECKLACE,    // Colar
    CHEST,       // Peito
    PANTS,       // Calças
    GLOVES,      // Luvas
    BOOTS,       // Botas
    RING,        // Anel
    BRACELET,    // Bracelete
    WEAPON,      // Arma principal
    SHIELD,      // Escudo/arma secundária
    MOUNT,       // Montaria
    COSTUME,     // Roupa de personagem
    ACCESSORY_1, // Acessório 1
    ACCESSORY_2  // Acessório 2
};

/**
 * @brief Atributos de item
 */
struct ItemStats {
    int32_t attack;        // Ataque físico
    int32_t defense;       // Defesa física
    int32_t magicAttack;   // Ataque mágico
    int32_t magicDefense;  // Defesa mágica
    int32_t hitRate;       // Taxa de acerto
    int32_t dodgeRate;     // Taxa de esquiva
    int32_t criticalRate;  // Taxa de crítico
    int32_t hpBonus;       // Bônus de HP
    int32_t mpBonus;       // Bônus de MP
    
    // Bônus elementais
    int32_t fireDmg;
    int32_t waterDmg;
    int32_t earthDmg;
    int32_t windDmg;
    int32_t lightDmg;
    int32_t darkDmg;
    
    // Resistências elementais
    int32_t fireRes;
    int32_t waterRes;
    int32_t earthRes;
    int32_t windRes;
    int32_t lightRes;
    int32_t darkRes;
    
    // Bônus de estado
    int32_t poisonRate;
    int32_t stunRate;
    int32_t freezeRate;
    int32_t burnRate;
    
    // Resistências a estado
    int32_t poisonRes;
    int32_t stunRes;
    int32_t freezeRes;
    int32_t burnRes;
    
    // Bônus de atributos
    int32_t strBonus;
    int32_t intBonus;
    int32_t dexBonus;
    int32_t conBonus;
    
    // Bônus especiais
    int32_t moveSpeed;
    int32_t attackSpeed;
    int32_t castSpeed;
    
    // Bônus percentuais
    float attackPct;
    float defensePct;
    float hpPct;
    float mpPct;
    float expPct;
    float goldPct;
    float dropRatePct;
};

/**
 * @brief Requisitos para uso de item
 */
struct ItemRequirements {
    int32_t level;          // Nível mínimo
    int32_t strength;       // FOR mínima
    int32_t intelligence;   // INT mínima
    int32_t dexterity;      // DES mínima
    int32_t constitution;   // CON mínima
    
    // Classes que podem usar
    bool usableByTK;        // Transcendental Knight
    bool usableByFM;        // Force Master
    bool usableByBM;        // Blade Master
    bool usableByHT;        // Holy Tide
    
    // Outros requisitos
    int32_t reputationLevel;
    std::string faction;
    bool questCompleted;
    int32_t questId;
};

/**
 * @brief Flags de item
 */
struct ItemFlags {
    bool tradeable;     // Pode ser comercializado
    bool storable;      // Pode ser guardado
    bool sellable;      // Pode ser vendido
    bool droppable;     // Pode ser descartado
    bool destroyable;   // Pode ser destruído
    bool questItem;     // Item de quest
    bool unique;        // Item único (apenas um por jogador)
    bool soulbound;     // Vinculado à alma (não pode ser trocado)
    bool accountBound;  // Vinculado à conta
    bool usable;        // Pode ser usado
    bool stackable;     // Pode ser empilhado
};

/**
 * @brief Efeitos de habilidade de item
 */
struct ItemSkillEffect {
    int32_t skillId;     // ID da habilidade
    std::string skillName; // Nome da habilidade
    float chance;        // Chance de ativação (0.0 a 1.0)
    int32_t cooldown;    // Tempo de recarga (segundos)
    std::string trigger; // Evento que ativa (hit, crit, equip, use, etc.)
    std::string effectScript; // Script de efeito
    std::map<std::string, std::string> parameters; // Parâmetros adicionais
};

/**
 * @brief Aparência de item
 */
struct ItemAppearance {
    int32_t modelId;        // ID do modelo
    int32_t textureId;      // ID da textura
    std::string colorPalette; // Paleta de cores
    bool hasGlowEffect;     // Tem efeito de brilho
    std::string glowColor;  // Cor do brilho
    float scale;            // Escala do modelo
    std::string particleEffect; // Efeito de partículas
    std::string trailEffect;   // Efeito de rastro
    std::string equipSound;    // Som ao equipar
    std::string useSound;      // Som ao usar
};

/**
 * @brief Modelo de UI para itens
 */
struct ItemUIModel {
    uint32_t id;           // ID único do item
    std::string name;      // Nome do item
    std::string description; // Descrição
    std::string category;  // Categoria
    ItemType type;         // Tipo
    ItemRarity rarity;     // Raridade
    ItemSlot slot;         // Slot de equipamento
    int32_t level;         // Nível do item
    int32_t price;         // Preço base
    
    // Características
    int32_t maxDurability;    // Durabilidade máxima
    int32_t weight;           // Peso
    int32_t maxStackSize;     // Tamanho máximo da pilha
    int32_t cooldown;         // Tempo de recarga (segundos)
    
    // Aparência
    ItemAppearance appearance;
    
    // Atributos
    ItemStats stats;
    
    // Requisitos
    ItemRequirements requirements;
    
    // Flags
    ItemFlags flags;
    
    // Upgrade e soquetes
    int32_t enhancementLevel;  // Nível de aprimoramento atual (+0, +1, etc.)
    int32_t maxUpgradeLevel;   // Nível máximo de aprimoramento
    int32_t sockets;           // Número atual de soquetes
    int32_t maxSockets;        // Número máximo de soquetes
    std::vector<uint32_t> socketedItemIds; // IDs dos itens encaixados
    
    // Efeitos especiais
    std::vector<ItemSkillEffect> skillEffects;
    
    // Metadados
    int32_t createdTimestamp;
    int32_t lastModifiedTimestamp;
    std::string createdBy;
    std::string version;
};

/**
 * @brief Resultado de operações relacionadas a itens
 */
struct ItemOperationResult {
    bool success;
    std::string message;
    std::optional<ItemUIModel> item;
};

/**
 * @brief Filtros de pesquisa para itens
 */
struct ItemFilter {
    std::optional<std::string> nameContains;
    std::optional<ItemType> type;
    std::optional<ItemRarity> minRarity;
    std::optional<ItemSlot> slot;
    std::optional<int32_t> minLevel;
    std::optional<int32_t> maxLevel;
    std::optional<std::string> category;
    std::optional<bool> tradeable;
    std::optional<bool> questItem;
};

/**
 * @brief Conversor entre tipos de string e enumeração
 */
inline std::string itemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::WEAPON: return "Weapon";
        case ItemType::ARMOR: return "Armor";
        case ItemType::ACCESSORY: return "Accessory";
        case ItemType::POTION: return "Potion";
        case ItemType::MATERIAL: return "Material";
        case ItemType::QUEST: return "Quest";
        case ItemType::OTHER: return "Other";
        default: return "Unknown";
    }
}

inline std::string itemRarityToString(ItemRarity rarity) {
    switch (rarity) {
        case ItemRarity::COMMON: return "Common";
        case ItemRarity::UNCOMMON: return "Uncommon";
        case ItemRarity::RARE: return "Rare";
        case ItemRarity::EPIC: return "Epic";
        case ItemRarity::LEGENDARY: return "Legendary";
        case ItemRarity::MYTHIC: return "Mythic";
        default: return "Unknown";
    }
}

inline std::string itemSlotToString(ItemSlot slot) {
    switch (slot) {
        case ItemSlot::NONE: return "None";
        case ItemSlot::HEAD: return "Head";
        case ItemSlot::NECKLACE: return "Necklace";
        case ItemSlot::CHEST: return "Chest";
        case ItemSlot::PANTS: return "Pants";
        case ItemSlot::GLOVES: return "Gloves";
        case ItemSlot::BOOTS: return "Boots";
        case ItemSlot::RING: return "Ring";
        case ItemSlot::BRACELET: return "Bracelet";
        case ItemSlot::WEAPON: return "Weapon";
        case ItemSlot::SHIELD: return "Shield";
        case ItemSlot::MOUNT: return "Mount";
        case ItemSlot::COSTUME: return "Costume";
        case ItemSlot::ACCESSORY_1: return "Accessory 1";
        case ItemSlot::ACCESSORY_2: return "Accessory 2";
        default: return "Unknown";
    }
}

/**
 * @brief Editor de Itens do WYDStudio
 */
class ItemStudio {
public:
    /**
     * @brief Construtor
     * @param facade Referência ao facade principal do WYDStudio
     */
    ItemStudio(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~ItemStudio();
    
    /**
     * @brief Inicializa o editor de itens
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Obtém a lista de todos os itens
     * @return Lista de todos os itens disponíveis
     */
    std::vector<ItemUIModel> getAllItems();
    
    /**
     * @brief Busca itens com filtragem
     * @param filter Filtros de busca
     * @return Lista de itens que correspondem aos filtros
     */
    std::vector<ItemUIModel> searchItems(const ItemFilter& filter);
    
    /**
     * @brief Obtém um item pelo ID
     * @param itemId ID do item
     * @return Modelo do item ou nulo se não encontrado
     */
    std::optional<ItemUIModel> getItemById(uint32_t itemId);
    
    /**
     * @brief Obtém itens por categoria
     * @param category Nome da categoria
     * @return Lista de itens na categoria especificada
     */
    std::vector<ItemUIModel> getItemsByCategory(const std::string& category);
    
    /**
     * @brief Obtém um novo modelo de item com valores padrão
     * @return Modelo de item padrão
     */
    ItemUIModel getNewItemTemplate();
    
    /**
     * @brief Cria um novo item com valores padrão
     * @param name Nome do novo item (opcional)
     * @return Resultado da operação contendo o novo item
     */
    ItemOperationResult createNewItem(const std::string& name = "New Item");
    
    /**
     * @brief Duplica um item existente
     * @param itemId ID do item a ser duplicado
     * @param newName Nome opcional para o item duplicado
     * @return Resultado da operação contendo o item duplicado
     */
    ItemOperationResult duplicateItem(uint32_t itemId, const std::string& newName = "");
    
    /**
     * @brief Salva alterações em um item
     * @param item Modelo do item modificado
     * @return Resultado da operação
     */
    ItemOperationResult saveItem(const ItemUIModel& item);
    
    /**
     * @brief Remove um item
     * @param itemId ID do item a ser removido
     * @return Resultado da operação
     */
    ItemOperationResult removeItem(uint32_t itemId);
    
    /**
     * @brief Importa itens de um arquivo
     * @param filePath Caminho do arquivo de importação
     * @return Resultado da operação
     */
    ItemOperationResult importItems(const std::string& filePath);
    
    /**
     * @brief Exporta itens para um arquivo
     * @param filePath Caminho do arquivo de exportação
     * @param format Formato de exportação ("json", "xml", "csv", "bin")
     * @return Resultado da operação
     */
    ItemOperationResult exportItems(const std::string& filePath, const std::string& format = "json");
    
    /**
     * @brief Aplica alterações ao servidor sem reiniciar
     * @return true se aplicado com sucesso
     */
    bool applyChangesToServer();
    
    /**
     * @brief Adiciona um efeito de habilidade a um item
     * @param itemId ID do item
     * @param effect Dados do efeito
     * @return Resultado da operação
     */
    ItemOperationResult addSkillEffect(uint32_t itemId, const ItemSkillEffect& effect);
    
    /**
     * @brief Remove um efeito de habilidade de um item
     * @param itemId ID do item
     * @param skillId ID da habilidade
     * @return Resultado da operação
     */
    ItemOperationResult removeSkillEffect(uint32_t itemId, int32_t skillId);
    
    /**
     * @brief Define o slot de equipamento para um item
     * @param itemId ID do item
     * @param slot Slot de equipamento
     * @return Resultado da operação
     */
    ItemOperationResult setItemSlot(uint32_t itemId, ItemSlot slot);
    
    /**
     * @brief Atualiza a aparência de um item
     * @param itemId ID do item
     * @param appearance Dados de aparência
     * @return Resultado da operação
     */
    ItemOperationResult setItemAppearance(uint32_t itemId, const ItemAppearance& appearance);
    
    /**
     * @brief Define os requisitos para uso de um item
     * @param itemId ID do item
     * @param requirements Requisitos
     * @return Resultado da operação
     */
    ItemOperationResult setItemRequirements(uint32_t itemId, const ItemRequirements& requirements);
    
    /**
     * @brief Define flags para um item
     * @param itemId ID do item
     * @param flags Flags
     * @return Resultado da operação
     */
    ItemOperationResult setItemFlags(uint32_t itemId, const ItemFlags& flags);
    
    /**
     * @brief Adiciona um soquete a um item
     * @param itemId ID do item
     * @return Resultado da operação
     */
    ItemOperationResult addSocket(uint32_t itemId);
    
    /**
     * @brief Remove um soquete de um item
     * @param itemId ID do item
     * @return Resultado da operação
     */
    ItemOperationResult removeSocket(uint32_t itemId);
    
    /**
     * @brief Encaixa um item em um soquete
     * @param itemId ID do item base
     * @param socketIndex Índice do soquete (0-based)
     * @param socketItemId ID do item a ser encaixado
     * @return Resultado da operação
     */
    ItemOperationResult socketItem(uint32_t itemId, int32_t socketIndex, uint32_t socketItemId);
    
    /**
     * @brief Remove um item encaixado
     * @param itemId ID do item base
     * @param socketIndex Índice do soquete (0-based)
     * @return Resultado da operação
     */
    ItemOperationResult unsocketItem(uint32_t itemId, int32_t socketIndex);
    
    /**
     * @brief Verifica a integridade de um item
     * @param itemId ID do item a verificar
     * @return Lista de problemas encontrados (vazia se OK)
     */
    std::vector<std::string> validateItem(uint32_t itemId);
    
    /**
     * @brief Balanceia atributos de um item com base no nível e raridade
     * @param itemId ID do item a balancear
     * @return Resultado da operação
     */
    ItemOperationResult balanceItem(uint32_t itemId);
    
    /**
     * @brief Converte um item para formato binário original
     * @param itemId ID do item
     * @return Dados binários do item
     */
    std::vector<uint8_t> convertItemToBinary(uint32_t itemId);
    
    /**
     * @brief Converte dados binários para modelo de item
     * @param binData Dados binários
     * @return Modelo do item convertido
     */
    std::optional<ItemUIModel> convertBinaryToItem(const std::vector<uint8_t>& binData);
    
    /**
     * @brief Obtém a lista de todas as categorias de item disponíveis
     * @return Lista de categorias
     */
    std::vector<std::string> getItemCategories();
    
    /**
     * @brief Recarrega todos os itens do disco
     * @return true se recarregado com sucesso
     */
    bool reloadItems();
    
    /**
     * @brief Registra callback para notificações de alterações em itens
     * @param callback Função de callback
     * @return ID de registro (para cancelamento posterior)
     */
    uint32_t registerChangeCallback(std::function<void(const ItemUIModel&)> callback);
    
    /**
     * @brief Cancela registro de callback
     * @param callbackId ID do callback a cancelar
     */
    void unregisterChangeCallback(uint32_t callbackId);
    
    /**
     * @brief Carrega biblioteca de ícones de itens
     * @param directoryPath Caminho da pasta de ícones
     * @return true se carregado com sucesso
     */
    bool loadIconLibrary(const std::string& directoryPath);
    
    /**
     * @brief Obtém um ícone para um item específico
     * @param itemId ID do item
     * @return Dados da imagem do ícone
     */
    std::vector<uint8_t> getItemIcon(uint32_t itemId);
    
    /**
     * @brief Define um ícone para um item
     * @param itemId ID do item
     * @param iconData Dados da imagem do ícone
     * @return true se definido com sucesso
     */
    bool setItemIcon(uint32_t itemId, const std::vector<uint8_t>& iconData);
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade principal
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Utilitários
    ItemUIModel createDefaultItemByType(ItemType type);
    json serializeItemToJson(const ItemUIModel& item);
    ItemUIModel deserializeItemFromJson(const json& json);
    
    // Validação de itens
    bool validateItemIntegrity(const ItemUIModel& item, std::vector<std::string>& issues);
    
    // Geração de IDs
    uint32_t generateUniqueItemId();
    
    // Manipulação de arquivo binário
    bool loadItemsFromBinary(const std::string& filePath);
    bool saveItemsToBinary(const std::string& filePath);
    
    // Conversão entre formatos internos e binários
    std::vector<uint8_t> convertModelToBinary(const ItemUIModel& item);
    ItemUIModel convertBinaryToModel(const std::vector<uint8_t>& data);
};

}} // namespace wydstudio::ui

#endif // WYDSTUDIO_ITEM_STUDIO_H