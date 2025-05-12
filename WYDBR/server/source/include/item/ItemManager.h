/**
 * ItemManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/item/ItemManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _ITEM_MANAGER_H_
#define _ITEM_MANAGER_H_

/**
 * @file ItemManager.h
 * @brief Gerenciador de itens do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de itens do WYDBR,
 * responsável por criar, gerenciar e controlar todos os itens do jogo,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <random>

#include "../GlobalDef.h"
#include "../Basedef.h"
#include "../TM_Item.h"

namespace wydbr {
namespace item {

/**
 * @brief Tipo de resultado de operação com item
 */
enum /**
 * Classe WYDItemOperationResult
 * 
 * Esta classe implementa a funcionalidade WYDItemOperationResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemOperationResult {
    SUCCESS = 0,             // Operação bem-sucedida
    INVALID_ITEM = 1,        // Item inválido
    INVALID_TARGET = 2,      // Alvo inválido
    NO_SPACE = 3,            // Sem espaço
    ITEM_NOT_FOUND = 4,      // Item não encontrado
    WRONG_POSITION = 5,      // Posição errada
    BIND_RESTRICTION = 6,    // Restrição de vinculação
    LEVEL_RESTRICTION = 7,   // Restrição de nível
    CLASS_RESTRICTION = 8,   // Restrição de classe
    COOLDOWN_RESTRICTION = 9, // Restrição de cooldown
    DURABILITY_DEPLETED = 10, // Durabilidade esgotada
    ALREADY_EQUIPPED = 11,   // Já equipado
    NOT_EQUIPPED = 12,       // Não equipado
    NOT_USABLE = 13,         // Não utilizável
    LIMIT_REACHED = 14,      // Limite atingido
    NOT_COMBINABLE = 15,     // Não combinável
    NOT_SUFFICIENT = 16,     // Insuficiente
    NOT_ENOUGH_MONEY = 17,   // Dinheiro insuficiente
    WRONG_STATE = 18,        // Estado errado
    EXPIRED = 19,            // Expirado
    INVALID_USE = 20,        // Uso inválido
    CANCELED = 21,           // Cancelado
    UNKNOWN_ERROR = 22       // Erro desconhecido
};

/**
 * @brief Tipo de operação de item
 */
enum /**
 * Classe WYDItemOperationType
 * 
 * Esta classe implementa a funcionalidade WYDItemOperationType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemOperationType {
    NONE = 0,                // Nenhuma
    CREATE = 1,              // Criar
    DELETE = 2,              // Deletar
    UPDATE = 3,              // Atualizar
    MOVE = 4,                // Mover
    SPLIT = 5,               // Dividir
    COMBINE = 6,             // Combinar
    EQUIP = 7,               // Equipar
    UNEQUIP = 8,             // Desequipar
    USE = 9,                 // Usar
    DROP = 10,               // Dropar
    PICKUP = 11,             // Pegar
    SELL = 12,               // Vender
    BUY = 13,                // Comprar
    TRADE = 14,              // Trocar
    SHOP = 15,               // Loja
    REFINE = 16,             // Refinar
    CRAFT = 17,              // Fabricar
    DONATE = 18,             // Doar
    STORE = 19,              // Armazenar
    RETRIEVE = 20,           // Recuperar
    IDENTIFY = 21,           // Identificar
    REPAIR = 22,             // Reparar
    DISASSEMBLE = 23,        // Desmontar
    SOCKET = 24,             // Inserir soquete
    ENHANCE = 25,            // Melhorar
    REMOVE_ENHANCEMENT = 26, // Remover melhoria
    APPLY_OPTION = 27,       // Aplicar opção
    REMOVE_OPTION = 28,      // Remover opção
    TRANSFORM = 29,          // Transformar
    CONVERT = 30,            // Converter
    EXPIRE = 31,             // Expirar
    CUSTOM_1 = 32,           // Personalizado 1
    CUSTOM_2 = 33,           // Personalizado 2
    CUSTOM_3 = 34,           // Personalizado 3
    UNKNOWN = 35             // Desconhecido
};

/**
 * @brief Tipo de evento de item
 */
enum /**
 * Classe WYDItemEventType
 * 
 * Esta classe implementa a funcionalidade WYDItemEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemEventType {
    NONE = 0,                // Nenhum
    CREATED = 1,             // Criado
    DELETED = 2,             // Deletado
    UPDATED = 3,             // Atualizado
    MOVED = 4,               // Movido
    EQUIPPED = 5,            // Equipado
    UNEQUIPPED = 6,          // Desequipado
    USED = 7,                // Usado
    DROPPED = 8,             // Dropado
    PICKED = 9,              // Pegado
    SOLD = 10,               // Vendido
    BOUGHT = 11,             // Comprado
    TRADED = 12,             // Trocado
    REFINED = 13,            // Refinado
    CRAFTED = 14,            // Fabricado
    STORED = 15,             // Armazenado
    RETRIEVED = 16,          // Recuperado
    IDENTIFIED = 17,         // Identificado
    REPAIRED = 18,           // Reparado
    DISASSEMBLED = 19,       // Desmontado
    SOCKETED = 20,           // Soquete inserido
    ENHANCED = 21,           // Melhorado
    OPTION_ADDED = 22,       // Opção adicionada
    OPTION_REMOVED = 23,     // Opção removida
    TRANSFORMED = 24,        // Transformado
    CONVERTED = 25,          // Convertido
    EXPIRED = 26,            // Expirado
    DURABILITY_CHANGED = 27, // Durabilidade alterada
    BROKEN = 28,             // Quebrado
    SPLIT = 29,              // Dividido
    COMBINED = 30,           // Combinado
    DONATED = 31,            // Doado
    CUSTOM_1 = 32,           // Personalizado 1
    CUSTOM_2 = 33,           // Personalizado 2
    CUSTOM_3 = 34,           // Personalizado 3
    UNKNOWN = 35             // Desconhecido
};

/**
 * @brief Tipo de origem de item
 */
enum /**
 * Classe WYDItemSourceType
 * 
 * Esta classe implementa a funcionalidade WYDItemSourceType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemSourceType {
    NONE = 0,                // Nenhuma
    MONSTER_DROP = 1,        // Drop de monstro
    NPC_SHOP = 2,            // Loja de NPC
    QUEST_REWARD = 3,        // Recompensa de missão
    PLAYER_CRAFT = 4,        // Fabricação de jogador
    PLAYER_REFINE = 5,       // Refinamento de jogador
    PLAYER_TRADE = 6,        // Comércio de jogador
    PLAYER_DROP = 7,         // Drop de jogador
    SYSTEM_GIFT = 8,         // Presente do sistema
    EVENT_REWARD = 9,        // Recompensa de evento
    MAIL_ATTACHMENT = 10,    // Anexo de correio
    GUILD_REWARD = 11,       // Recompensa de guilda
    ACHIEVEMENT = 12,        // Conquista
    DUNGEON_REWARD = 13,     // Recompensa de masmorra
    MARKETPLACE = 14,        // Mercado
    ITEM_CONVERSION = 15,    // Conversão de item
    ITEM_DISASSEMBLY = 16,   // Desmontagem de item
    ITEM_COMBINATION = 17,   // Combinação de item
    ADMINISTRATOR = 18,      // Administrador
    STARTER_ITEM = 19,       // Item inicial
    CUSTOM_1 = 20,           // Personalizado 1
    CUSTOM_2 = 21,           // Personalizado 2
    CUSTOM_3 = 22,           // Personalizado 3
    UNKNOWN = 23             // Desconhecido
};

/**
 * @brief Tipo de método de obtenção de item
 */
enum /**
 * Classe WYDItemAcquisitionMethod
 * 
 * Esta classe implementa a funcionalidade WYDItemAcquisitionMethod conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemAcquisitionMethod {
    NONE = 0,                // Nenhum
    LOOT = 1,                // Saque
    PURCHASE = 2,            // Compra
    REWARD = 3,              // Recompensa
    CRAFT = 4,               // Fabricação
    TRADE = 5,               // Comércio
    GIFT = 6,                // Presente
    QUEST = 7,               // Missão
    EVENT = 8,               // Evento
    MAIL = 9,                // Correio
    ACHIEVEMENT = 10,        // Conquista
    CONVERSION = 11,         // Conversão
    DISASSEMBLY = 12,        // Desmontagem
    COMBINATION = 13,        // Combinação
    STARTER = 14,            // Inicial
    ADMIN = 15,              // Administrador
    CUSTOM_1 = 16,           // Personalizado 1
    CUSTOM_2 = 17,           // Personalizado 2
    CUSTOM_3 = 18,           // Personalizado 3
    UNKNOWN = 19             // Desconhecido
};

/**
 * @brief Tipo de filtro de item
 */
enum /**
 * Classe WYDItemFilterType
 * 
 * Esta classe implementa a funcionalidade WYDItemFilterType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemFilterType {
    NONE = 0,                // Nenhum
    ITEM_ID = 1,             // ID do item
    ITEM_NAME = 2,           // Nome do item
    ITEM_TYPE = 3,           // Tipo do item
    ITEM_SUBTYPE = 4,        // Subtipo do item
    EQUIP_POSITION = 5,      // Posição de equipamento
    ITEM_LEVEL = 6,          // Nível do item
    EF_OPTION = 7,           // Opção de efeito
    SOCKET_COUNT = 8,        // Contagem de soquetes
    REFINE_LEVEL = 9,        // Nível de refinamento
    ANCIENT_LEVEL = 10,      // Nível ancestral
    ENHANCED = 11,           // Melhorado
    IDENTIFIED = 12,         // Identificado
    TRADABLE = 13,           // Negociável
    BOUND = 14,              // Vinculado
    STACKABLE = 15,          // Empilhável
    DURABILITY_RANGE = 16,   // Faixa de durabilidade
    EXPIRE_TIME = 17,        // Tempo de expiração
    ITEM_SET = 18,           // Conjunto de itens
    RARITY = 19,             // Raridade
    CUSTOM_1 = 20,           // Personalizado 1
    CUSTOM_2 = 21,           // Personalizado 2
    CUSTOM_3 = 22,           // Personalizado 3
    UNKNOWN = 23             // Desconhecido
};

/**
 * @brief Contexto de operação de item
 */
struct ItemOperationContext {
    uint32_t CharacterID;            // ID do personagem
    uint16_t TargetX;                // X do alvo
    uint16_t TargetY;                // Y do alvo
    uint32_t TargetID;               // ID do alvo
    uint32_t Gold;                   // Ouro
    uint32_t Quantity;               // Quantidade
    uint8_t InventoryType;           // Tipo de inventário
    uint8_t SourceSlot;              // Slot de origem
    uint8_t TargetSlot;              // Slot de destino
    uint8_t EquipPosition;           // Posição de equipamento
    bool IsAdmin;                    // É administrador
    ItemOperationType OperationType; // Tipo de operação
    ItemSourceType SourceType;       // Tipo de origem
    
    ItemOperationContext() : CharacterID(0), TargetX(0), TargetY(0), TargetID(0),
        Gold(0), Quantity(0), InventoryType(0), SourceSlot(0), TargetSlot(0),
        EquipPosition(0), IsAdmin(false), OperationType(ItemOperationType::NONE),
        SourceType(ItemSourceType::NONE) {}
};

/**
 * @brief Contexto de criação de item
 */
struct ItemCreationContext {
    uint16_t ItemID;                       // ID do item
    uint32_t Quantity;                     // Quantidade
    uint8_t Refinement;                    // Refinamento
    bool BindOnCreate;                     // Vinculado ao criar
    bool Identified;                       // Identificado
    bool GenerateOptions;                  // Gerar opções
    ItemSourceType SourceType;             // Tipo de origem
    ItemAcquisitionMethod AcquisitionMethod; // Método de aquisição
    uint32_t ExpirationTime;               // Tempo de expiração
    
    ItemCreationContext() : ItemID(0), Quantity(1), Refinement(0),
        BindOnCreate(false), Identified(true), GenerateOptions(false),
        SourceType(ItemSourceType::NONE), AcquisitionMethod(ItemAcquisitionMethod::NONE),
        ExpirationTime(0) {}
};

/**
 * @brief Contexto de uso de item
 */
struct ItemUseContext {
    uint32_t CharacterID;            // ID do personagem
    uint8_t InventoryType;           // Tipo de inventário
    uint8_t Slot;                    // Slot
    uint32_t TargetID;               // ID do alvo
    uint16_t TargetX;                // X do alvo
    uint16_t TargetY;                // Y do alvo
    
    ItemUseContext() : CharacterID(0), InventoryType(0), Slot(0),
        TargetID(0), TargetX(0), TargetY(0) {}
};

/**
 * @brief Contexto de filtro de item
 */
struct ItemFilterContext {
    std::vector<ItemFilterType> FilterTypes; // Tipos de filtro
    std::vector<uint32_t> FilterValues;      // Valores de filtro
    std::vector<std::string> FilterStrings;  // Strings de /**
 * ItemFilterContext
 * 
 * Implementa a funcionalidade ItemFilterContext conforme especificação original.
 * @return Retorna filtro
 */
 filtro
    
    ItemFilterContext(){}
};

/**
 * @brief Contexto de drop de item
 */
struct ItemDropContext {
    uint16_t MobID;                  // ID do mob
    uint8_t MobLevel;                // Nível do mob
    bool IsBoss;                     // É chefe
    bool IsRaidBoss;                 // É chefe de raid
    bool IsElite;                    // É elite
    uint16_t MapID;                  // ID do mapa
    uint16_t PosX;                   // Posição X
    uint16_t PosY;                   // Posição Y
    uint32_t KillerID;               // ID do matador
    float DropRateBonus;             // Bônus de taxa de drop
    uint8_t MobRank;                 // Rank do mob
    uint8_t MobRarity;               // Raridade do mob
    
    ItemDropContext() : MobID(0), MobLevel(0), IsBoss(false),
        IsRaidBoss(false), IsElite(false), MapID(0), PosX(0), PosY(0),
        KillerID(0), DropRateBonus(1.0f), MobRank(0), MobRarity(0) {}
};

/**
 * @brief Contexto de refinamento de item
 */
struct ItemRefineContext {
    uint32_t CharacterID;            // ID do personagem
    uint8_t InventoryType;           // Tipo de inventário
    uint8_t ItemSlot;                // Slot do item
    uint8_t MaterialSlot;            // Slot do material
    uint8_t ProtectionSlot;          // Slot de proteção
    bool UseProtection;              // Usar proteção
    float SuccessRateBonus;          // Bônus de taxa de sucesso
    
    ItemRefineContext() : CharacterID(0), InventoryType(0), ItemSlot(0),
        MaterialSlot(0), ProtectionSlot(0), UseProtection(false),
        SuccessRateBonus(0.0f) {}
};

/**
 * @brief Contexto de soquete de item
 */
struct ItemSocketContext {
    uint32_t CharacterID;            // ID do personagem
    uint8_t InventoryType;           // Tipo de inventário
    uint8_t ItemSlot;                // Slot do item
    uint8_t GemSlot;                 // Slot da gema
    uint8_t SocketIndex;             // Índice do soquete
    uint8_t ProtectionSlot;          // Slot de proteção
    bool UseProtection;              // Usar proteção
    
    ItemSocketContext() : CharacterID(0), InventoryType(0), ItemSlot(0),
        GemSlot(0), SocketIndex(0), ProtectionSlot(0), UseProtection(false) {}
};

/**
 * @brief Configuração de item de loja
 */
struct ShopItemConfig {
    uint16_t ItemID;                 // ID do item
    uint16_t Quantity;               // Quantidade
    uint32_t Price;                  // Preço
    uint8_t Refinement;              // Refinamento
    uint8_t SocketCount;             // Contagem de soquetes
    bool Identified;                 // Identificado
    bool Tradable;                   // Negociável
    uint32_t RestockTime;            // Tempo de reabastecimento
    uint32_t StockLimit;             // Limite de estoque
    uint32_t CurrentStock;           // Estoque atual
    uint32_t LastRestockTime;        // Último tempo de reabastecimento
    
    ShopItemConfig() : ItemID(0), Quantity(1), Price(0), Refinement(0),
        SocketCount(0), Identified(true), Tradable(true), RestockTime(0),
        StockLimit(0), CurrentStock(0), LastRestockTime(0) {}
};

/**
 * @brief Configuração de loja
 */
struct ShopConfig {
    uint32_t ShopID;                 // ID da loja
    std::string Name;                // Nome
    uint16_t MapID;                  // ID do mapa
    uint16_t PosX;                   // Posição X
    uint16_t PosY;                   // Posição Y
    uint8_t Type;                    // Tipo
    std::vector<ShopItemConfig> Items; // Itens
    
    ShopConfig() : ShopID(0), MapID(0), PosX(0), PosY(0), Type(0) {}
};

/**
 * @brief Configuração de drop de item
 */
struct ItemDropConfig {
    uint16_t ItemID;                 // ID do item
    uint16_t MinQuantity;            // Quantidade mínima
    uint16_t MaxQuantity;            // Quantidade máxima
    float Probability;               // Probabilidade
    uint8_t MinMobLevel;             // Nível mínimo do mob
    uint8_t MaxMobLevel;             // Nível máximo do mob
    uint8_t MinRefinement;           // Refinamento mínimo
    uint8_t MaxRefinement;           // Refinamento máximo
    bool CanBeBound;                 // Pode ser vinculado
    bool Identified;                 // Identificado
    float AddOptionProbability;      // Probabilidade de adicionar opção
    
    ItemDropConfig() : ItemID(0), MinQuantity(1), MaxQuantity(1),
        Probability(0.0f), MinMobLevel(0), MaxMobLevel(255),
        MinRefinement(0), MaxRefinement(0), CanBeBound(false),
        Identified(true), AddOptionProbability(0.0f) {}
};

/**
 * @brief Configuração de grupo de drop
 */
struct DropGroupConfig {
    uint32_t GroupID;                // ID do grupo
    std::string Name;                // Nome
    std::vector<ItemDropConfig> Items; // Itens
    float DropRate;                  // Taxa de drop
    bool ExclusiveDrop;              // Drop exclusivo
    
    DropGroupConfig() : GroupID(0), DropRate(0.0f), ExclusiveDrop(false) {}
};

/**
 * @brief Evento de item
 */
struct ItemEvent {
    uint32_t EventID;                // ID do evento
    ItemEventType Type;              // Tipo
    uint32_t CharacterID;            // ID do personagem
    uint16_t ItemID;                 // ID do item
    uint32_t Quantity;               // Quantidade
    uint8_t InventoryType;           // Tipo de inventário
    uint8_t Slot;                    // Slot
    ItemSourceType SourceType;       // Tipo de origem
    ItemOperationType OperationType; // Tipo de operação
    uint32_t Timestamp;              // Timestamp
    
    ItemEvent() : EventID(0), Type(ItemEventType::NONE), CharacterID(0),
        ItemID(0), Quantity(0), InventoryType(0), Slot(0),
        SourceType(ItemSourceType::NONE), OperationType(ItemOperationType::NONE),
        Timestamp(0) {}
    
    ItemEvent(ItemEventType _type, uint32_t _characterID, uint16_t _itemID,
             uint32_t _quantity, uint8_t _inventoryType, uint8_t _slot,
             ItemSourceType _sourceType, ItemOperationType _operationType)
        : EventID(0), Type(_type), CharacterID(_characterID), ItemID(_itemID),
          Quantity(_quantity), InventoryType(_inventoryType), Slot(_slot),
          SourceType(_sourceType), OperationType(_operationType),
          Timestamp(static_cast<uint32_t>(time(nullptr))) {}
};

/**
 * @brief Probabilidade de refinamento
 */
struct RefineProbability {
    float SuccessRate;               // Taxa de sucesso
    float BreakRate;                 // Taxa de quebra
    float DegradeRate;               // Taxa de degradação
    
    RefineProbability() : SuccessRate(0.0f), BreakRate(0.0f), DegradeRate(0.0f) {}
    
    RefineProbability(float _successRate, float _breakRate, float _degradeRate)
        : SuccessRate(_successRate), BreakRate(_breakRate), DegradeRate(_degradeRate) {}
};

/**
 * @brief Tipo de callback de evento de item
 */
using ItemEventCallback = std::function<void(const ItemEvent&)>;

/**
 * @brief Tipo de callback de uso de item
 */
using ItemUseCallback = std::function<ItemOperationResult(const STRUCT_ITEM&, const ItemUseContext&)>;

/**
 * @brief Gerenciador de itens
 * 
 * Esta classe é responsável por gerenciar os itens do jogo,
 * incluindo criação, modificação, uso, drop, etc.
 */
/**
 * Classe WYDItemManager
 * 
 * Esta classe implementa a funcionalidade WYDItemManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static ItemManager& getInstance() {
        static ItemManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração de itens
     * @return true se carregado com sucesso
     */
    bool loadItemConfig();
    
    /**
     * @brief Carrega a configuração de lojas
     * @return true se carregado com sucesso
     */
    bool loadShopConfig();
    
    /**
     * @brief Carrega a configuração de drops
     * @return true se carregado com sucesso
     */
    bool loadDropConfig();
    
    /**
     * @brief Carrega a configuração de refinamento
     * @return true se carregado com sucesso
     */
    bool loadRefineConfig();
    
    /**
     * @brief Carrega a configuração de soquetes
     * @return true se carregado com sucesso
     */
    bool loadSocketConfig();
    
    /**
     * @brief Cria um item
     * @param ctx Contexto de criação
     * @param item Item criado (saída)
     * @return true se criado com sucesso
     */
    bool createItem(const ItemCreationContext& ctx, STRUCT_ITEM& item);
    
    /**
     * @brief Cria um item por ID
     * @param itemID ID do item
     * @param quantity Quantidade
     * @param refinement Refinamento
     * @param item Item criado (saída)
     * @return true se criado com sucesso
     */
    bool createItemByID(uint16_t itemID, uint32_t quantity = 1, uint8_t refinement = 0, STRUCT_ITEM& item = STRUCT_ITEM());
    
    /**
     * @brief Cria um item por nome
     * @param itemName Nome do item
     * @param quantity Quantidade
     * @param refinement Refinamento
     * @param item Item criado (saída)
     * @return true se criado com sucesso
     */
    bool createItemByName(const std::string& itemName, uint32_t quantity = 1, uint8_t refinement = 0, STRUCT_ITEM& item = STRUCT_ITEM());
    
    /**
     * @brief Adiciona um item ao inventário
     * @param characterID ID do personagem
     * @param item Item
     * @param sourceType Tipo de origem
     * @param operationType Tipo de operação
     * @param slot Slot (saída)
     * @return Resultado da operação
     */
    ItemOperationResult addItemToInventory(uint32_t characterID, const STRUCT_ITEM& item, ItemSourceType sourceType = ItemSourceType::NONE, ItemOperationType operationType = ItemOperationType::CREATE, uint8_t& slot = *(uint8_t*)0);
    
    /**
     * @brief Remove um item do inventário
     * @param characterID ID do personagem
     * @param inventoryType Tipo de inventário
     * @param slot Slot
     * @param quantity Quantidade
     * @param operationType Tipo de operação
     * @return Resultado da operação
     */
    ItemOperationResult removeItemFromInventory(uint32_t characterID, uint8_t inventoryType, uint8_t slot, uint32_t quantity = 0, ItemOperationType operationType = ItemOperationType::DELETE);
    
    /**
     * @brief Move um item no inventário
     * @param characterID ID do personagem
     * @param sourceInventoryType Tipo de inventário de origem
     * @param sourceSlot Slot de origem
     * @param targetInventoryType Tipo de inventário de destino
     * @param targetSlot Slot de destino
     * @param quantity Quantidade
     * @return Resultado da operação
     */
    ItemOperationResult moveItem(uint32_t characterID, uint8_t sourceInventoryType, uint8_t sourceSlot, uint8_t targetInventoryType, uint8_t targetSlot, uint32_t quantity = 0);
    
    /**
     * @brief Equipamento item
     * @param characterID ID do personagem
     * @param inventoryType Tipo de inventário
     * @param slot Slot
     * @param equipPosition Posição de equipamento
     * @return Resultado da operação
     */
    ItemOperationResult equipItem(uint32_t characterID, uint8_t inventoryType, uint8_t slot, uint8_t equipPosition);
    
    /**
     * @brief Desequipa item
     * @param characterID ID do personagem
     * @param equipPosition Posição de equipamento
     * @return Resultado da operação
     */
    ItemOperationResult unequipItem(uint32_t characterID, uint8_t equipPosition);
    
    /**
     * @brief Usa um item
     * @param ctx Contexto de uso
     * @return Resultado da operação
     */
    ItemOperationResult useItem(const ItemUseContext& ctx);
    
    /**
     * @brief Dropa um item
     * @param characterID ID do personagem
     * @param inventoryType Tipo de inventário
     * @param slot Slot
     * @param quantity Quantidade
     * @param mapID ID do mapa
     * @param posX Posição X
     * @param posY Posição Y
     * @return Resultado da operação
     */
    ItemOperationResult dropItem(uint32_t characterID, uint8_t inventoryType, uint8_t slot, uint32_t quantity, uint16_t mapID, uint16_t posX, uint16_t posY);
    
    /**
     * @brief Pega um item
     * @param characterID ID do personagem
     * @param dropID ID do drop
     * @return Resultado da operação
     */
    ItemOperationResult pickupItem(uint32_t characterID, uint32_t dropID);
    
    /**
     * @brief Vende um item
     * @param characterID ID do personagem
     * @param inventoryType Tipo de inventário
     * @param slot Slot
     * @param quantity Quantidade
     * @param shopID ID da loja
     * @param price Preço (saída)
     * @return Resultado da operação
     */
    ItemOperationResult sellItem(uint32_t characterID, uint8_t inventoryType, uint8_t slot, uint32_t quantity, uint32_t shopID, uint32_t& price);
    
    /**
     * @brief Compra um item
     * @param characterID ID do personagem
     * @param shopID ID da loja
     * @param itemIndex Índice do item
     * @param quantity Quantidade
     * @param price Preço (saída)
     * @return Resultado da operação
     */
    ItemOperationResult buyItem(uint32_t characterID, uint32_t shopID, uint32_t itemIndex, uint32_t quantity, uint32_t& price);
    
    /**
     * @brief Refina um item
     * @param ctx Contexto de refinamento
     * @param resultItem Item resultante (saída)
     * @return Resultado da operação
     */
    ItemOperationResult refineItem(const ItemRefineContext& ctx, STRUCT_ITEM& resultItem);
    
    /**
     * @brief Adiciona soquete a um item
     * @param ctx Contexto de soquete
     * @param resultItem Item resultante (saída)
     * @return Resultado da operação
     */
    ItemOperationResult socketItem(const ItemSocketContext& ctx, STRUCT_ITEM& resultItem);
    
    /**
     * @brief Gera drops para um mob
     * @param ctx Contexto de drop
     * @param drops Drops gerados (saída)
     * @return true se gerado com sucesso
     */
    bool generateDrops(const ItemDropContext& ctx, std::vector<STRUCT_ITEM>& drops);
    
    /**
     * @brief Obtém a probabilidade de refinamento para um item
     * @param item Item
     * @param successRateBonus Bônus de taxa de sucesso
     * @return Probabilidade de refinamento
     */
    RefineProbability getRefineProbability(const STRUCT_ITEM& item, float successRateBonus = 0.0f);
    
    /**
     * @brief Calcula o preço de venda de um item
     * @param item Item
     * @param quantity Quantidade
     * @param characterID ID do personagem
     * @return Preço
     */
    uint32_t calculateSellPrice(const STRUCT_ITEM& item, uint32_t quantity = 1, uint32_t characterID = 0);
    
    /**
     * @brief Verifica se um item é utilizável
     * @param item Item
     * @param characterID ID do personagem
     * @return true se for utilizável
     */
    bool isItemUsable(const STRUCT_ITEM& item, uint32_t characterID = 0);
    
    /**
     * @brief Verifica se um item é equipável
     * @param item Item
     * @param characterID ID do personagem
     * @return true se for equipável
     */
    bool isItemEquippable(const STRUCT_ITEM& item, uint32_t characterID = 0);
    
    /**
     * @brief Verifica se um item é refinável
     * @param item Item
     * @return true se for refinável
     */
    bool isItemRefinable(const STRUCT_ITEM& item);
    
    /**
     * @brief Verifica se um item é socketável
     * @param item Item
     * @return true se for socketável
     */
    bool isItemSocketable(const STRUCT_ITEM& item);
    
    /**
     * @brief Obtém o índice de equipamento para um item
     * @param item Item
     * @return Índice de equipamento ou -1 se não for equipável
     */
    int getEquipmentIndex(const STRUCT_ITEM& item);
    
    /**
     * @brief Obtém a informação de um item
     * @param itemID ID do item
     * @return Item base ou nullptr se não encontrado
     */
    const STRUCT_ITEMLIST* getItemBaseByID(uint16_t itemID);
    
    /**
     * @brief Obtém a informação de um item por nome
     * @param itemName Nome do item
     * @return Item base ou nullptr se não encontrado
     */
    const STRUCT_ITEMLIST* getItemBaseByName(const std::string& itemName);
    
    /**
     * @brief Obtém a configuração de uma loja
     * @param shopID ID da loja
     * @return Configuração da loja
     */
    const ShopConfig& getShopConfig(uint32_t shopID);
    
    /**
     * @brief Obtém o nome de um item
     * @param itemID ID do item
     * @return Nome
     */
    std::string getItemName(uint16_t itemID);
    
    /**
     * @brief Obtém o nome completo de um item com refinamento
     * @param item Item
     * @return Nome completo
     */
    std::string getFullItemName(const STRUCT_ITEM& item);
    
    /**
     * @brief Atualiza o estoque de lojas
     */
    void updateShopStock();
    
    /**
     * @brief Registra um callback de evento
     * @param type Tipo de evento
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerEventCallback(ItemEventType type, ItemEventCallback callback);
    
    /**
     * @brief Remove um callback de evento
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEventCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de uso de item
     * @param itemID ID do item
     * @param callback Função de callback
     * @return true se registrado com sucesso
     */
    bool registerItemUseCallback(uint16_t itemID, ItemUseCallback callback);
    
    /**
     * @brief Filtra itens com base em critérios
     * @param items Lista de itens
     * @param ctx Contexto de filtro
     * @return Lista de itens filtrados
     */
    std::vector<STRUCT_ITEM> filterItems(const std::vector<STRUCT_ITEM>& items, const ItemFilterContext& ctx);
    
    /**
     * @brief Obtém a descrição de um item
     * @param item Item
     * @param characterID ID do personagem
     * @return Descrição
     */
    std::string getItemDescription(const STRUCT_ITEM& item, uint32_t characterID = 0);
    
    /**
     * @brief Obtém os requisitos de um item
     * @param item Item
     * @return Requisitos
     */
    std::string getItemRequirements(const STRUCT_ITEM& item);
    
    /**
     * @brief Obtém as estatísticas de um item
     * @param item Item
     * @return Estatísticas
     */
    std::string getItemStats(const STRUCT_ITEM& item);
    
    /**
     * @brief Obtém a meta de um item
     * @param item Item
     * @return Meta
     */
    std::string getItemMeta(const STRUCT_ITEM& item);
    
    /**
     * @brief Verifica se um item está disponível para uso
     * @param characterID ID do personagem
     * @param inventoryType Tipo de inventário
     * @param slot Slot
     * @return true se estiver disponível
     */
    bool isItemAvailable(uint32_t characterID, uint8_t inventoryType, uint8_t slot);
    
    /**
     * @brief Verifica se o inventário tem espaço
     * @param characterID ID do personagem
     * @param inventoryType Tipo de inventário
     * @param requiredSlots Slots requeridos
     * @return true se tiver espaço
     */
    bool hasInventorySpace(uint32_t characterID, uint8_t inventoryType, uint8_t requiredSlots = 1);
    
    /**
     * @brief Encontra o primeiro slot vazio no inventário
     * @param characterID ID do personagem
     * @param inventoryType Tipo de inventário
     * @return Slot vazio ou 255 se não encontrado
     */
    uint8_t findEmptySlot(uint32_t characterID, uint8_t inventoryType);
    
    /**
     * @brief Verifica se um personagem tem um item específico
     * @param characterID ID do personagem
     * @param itemID ID do item
     * @param quantity Quantidade
     * @param inventoryType Tipo de inventário
     * @return true se tiver o item
     */
    bool hasItem(uint32_t characterID, uint16_t itemID, uint32_t quantity = 1, uint8_t inventoryType = 0);
    
    /**
     * @brief Encontra um item no inventário
     * @param characterID ID do personagem
     * @param itemID ID do item
     * @param inventoryType Tipo de inventário
     * @param slot Slot (saída)
     * @return true se encontrado
     */
    bool findItem(uint32_t characterID, uint16_t itemID, uint8_t inventoryType, uint8_t& slot);
    
    /**
     * @brief Consome um item
     * @param characterID ID do personagem
     * @param itemID ID do item
     * @param quantity Quantidade
     * @param inventoryType Tipo de inventário
     * @return true se consumido com sucesso
     */
    bool consumeItem(uint32_t characterID, uint16_t itemID, uint32_t quantity = 1, uint8_t inventoryType = 0);
    
    /**
     * @brief Conta a quantidade de um item
     * @param characterID ID do personagem
     * @param itemID ID do item
     * @param inventoryType Tipo de inventário
     * @return Quantidade
     */
    uint32_t countItem(uint32_t characterID, uint16_t itemID, uint8_t inventoryType = 0);
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
    /**
     * @brief Define o modo de log
     * @param logging Modo de log
     */
    void setLoggingEnabled(bool logging);
    
    /**
     * @brief Verifica se o modo de log está ativado
     * @return true se estiver ativado
     */
    bool isLoggingEnabled() const;
    
    /**
     * @brief Define o fator de taxa de drop
     * @param factor Fator
     */
    void setDropRateFactor(float factor);
    
    /**
     * @brief Obtém o fator de taxa de drop
     * @return Fator
     */
    float getDropRateFactor() const;
    
    /**
     * @brief Define o fator de taxa de refinamento
     * @param factor Fator
     */
    void setRefinementRateFactor(float factor);
    
    /**
     * @brief Obtém o fator de taxa de refinamento
     * @return Fator
     */
    float getRefinementRateFactor() const;
    
    /**
     * @brief Define o fator de taxa de opção
     * @param factor Fator
     */
    void setOptionRateFactor(float factor);
    
    /**
     * @brief Obtém o fator de taxa de opção
     * @return Fator
     */
    float getOptionRateFactor() const;
    
    /**
     * @brief Define o fator de preço de venda
     * @param factor Fator
     */
    void setSellPriceFactor(float factor);
    
    /**
     * @brief Obtém o fator de preço de venda
     * @return Fator
     */
    float getSellPriceFactor() const;
    
    /**
     * @brief Define o fator de preço de compra
     * @param factor Fator
     */
    void setBuyPriceFactor(float factor);
    
    /**
     * @brief Obtém o fator de preço de compra
     * @return Fator
     */
    float getBuyPriceFactor() const;
    
    /**
     * @brief Define o fator de taxa de socket
     * @param factor Fator
     */
    void setSocketRateFactor(float factor);
    
    /**
     * @brief Obtém o fator de taxa de socket
     * @return Fator
     */
    float getSocketRateFactor() const;
    
private:
    /**
     * @brief Construtor privado
     */
    ItemManager();
    
    /**
     * @brief Destrutor privado
     */
    ~ItemManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    ItemManager(const ItemManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    ItemManager& operator=(const ItemManager&) = delete;
    
    // Configurações de itens
    std::unordered_map<uint16_t, STRUCT_ITEMLIST> _itemBase;
    std::unordered_map<std::string, uint16_t> _itemNameToID;
    
    // Configurações de lojas
    std::unordered_map<uint32_t, ShopConfig> _shops;
    std::atomic<uint32_t> _nextShopID;
    
    // Configurações de drops
    std::unordered_map<uint32_t, DropGroupConfig> _dropGroups;
    std::unordered_map<uint16_t, std::vector<uint32_t>> _mobToDropGroups;
    std::atomic<uint32_t> _nextDropGroupID;
    
    // Configurações de refinamento
    std::unordered_map<uint8_t, RefineProbability> _refineRates;
    std::unordered_map<uint16_t, std::unordered_map<uint8_t, RefineProbability>> _itemRefineRates;
    
    // Fatores
    float _dropRateFactor;
    float _refinementRateFactor;
    float _optionRateFactor;
    float _sellPriceFactor;
    float _buyPriceFactor;
    float _socketRateFactor;
    
    // Callbacks
    std::unordered_map<uint32_t, ItemEventCallback> _eventCallbacks;
    std::unordered_map<ItemEventType, std::vector<uint32_t>> _eventCallbacksByType;
    std::unordered_map<uint16_t, ItemUseCallback> _itemUseCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Timer
    uint32_t _shopUpdateTimer;
    
    // Gerador de números aleatórios
    std::mt19937 _rng;
    
    // Thread safety
    mutable std::mutex _itemMutex;
    mutable std::mutex _shopMutex;
    mutable std::mutex _dropMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    /**
     * @brief Gera um ID de item
     * @return ID
     */
    uint16_t generateUniqueItemID();
    
    /**
     * @brief Gera um ID de loja
     * @return ID
     */
    uint32_t generateShopID();
    
    /**
     * @brief Gera um ID de grupo de drop
     * @return ID
     */
    uint32_t generateDropGroupID();
    
    /**
     * @brief Executa o callback de uso de item
     * @param item Item
     * @param ctx Contexto de uso
     * @return Resultado da operação
     */
    ItemOperationResult executeItemUseCallback(const STRUCT_ITEM& item, const ItemUseContext& ctx);
    
    /**
     * @brief Calcula o valor base de um item
     * @param item Item
     * @return Valor base
     */
    uint32_t calculateBaseValue(const STRUCT_ITEM& item);
    
    /**
     * @brief Gera opções aleatórias para um item
     * @param item Item
     * @param mobLevel Nível do mob
     * @param isBoss É chefe
     * @param isElite É elite
     * @return Item com opções
     */
    STRUCT_ITEM generateRandomOptions(const STRUCT_ITEM& item, uint8_t mobLevel = 0, bool isBoss = false, bool isElite = false);
    
    /**
     * @brief Gera um refinamento aleatório para um item
     * @param item Item
     * @param mobLevel Nível do mob
     * @param isBoss É chefe
     * @param isElite É elite
     * @param minRefinement Refinamento mínimo
     * @param maxRefinement Refinamento máximo
     * @return Refinamento
     */
    uint8_t generateRandomRefinement(const STRUCT_ITEM& item, uint8_t mobLevel = 0, bool isBoss = false, bool isElite = false, uint8_t minRefinement = 0, uint8_t maxRefinement = 0);
    
    /**
     * @brief Loga um evento de item
     * @param event Evento
     */
    void logItemEvent(const ItemEvent& event);
    
    /**
     * @brief Notifica os callbacks de evento
     * @param event Evento
     */
    void notifyEventCallbacks(const ItemEvent& event);
    
    /**
     * @brief Obtém um número aleatório entre min e max
     * @param min Mínimo
     * @param max Máximo
     * @return Número aleatório
     */
    int getRandomInt(int min, int max);
    
    /**
     * @brief Obtém um número aleatório entre 0 e 1
     * @return Número aleatório
     */
    float getRandomFloat();
    
    /**
     * @brief Verifica se um evento aleatório ocorre com base na probabilidade
     * @param probability Probabilidade
     * @return true se o evento ocorrer
     */
    bool randomChance(float probability);
    
    /**
     * @brief Calcula durabilidade para um item novo
     * @param itemID ID do item
     * @return Durabilidade
     */
    uint16_t calculateDurability(uint16_t itemID);
    
    /**
     * @brief Cria um item totalmente aleatório
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @param allowTypes Tipos permitidos
     * @param mobLevel Nível do mob
     * @param isBoss É chefe
     * @param isElite É elite
     * @return Item aleatório
     */
    STRUCT_ITEM createRandomItem(int minLevel = 1, int maxLevel = 100, uint32_t allowTypes = 0xFFFFFFFF, uint8_t mobLevel = 0, bool isBoss = false, bool isElite = false);
    
    /**
     * @brief Adiciona um socket a um item
     * @param item Item
     * @param resultItem Item resultante (saída)
     * @return true se adicionado com sucesso
     */
    bool addSocketToItem(const STRUCT_ITEM& item, STRUCT_ITEM& resultItem);
    
    /**
     * @brief Adiciona uma gema a um socket
     * @param item Item
     * @param gemItem Gema
     * @param socketIndex Índice do socket
     * @param resultItem Item resultante (saída)
     * @return true se adicionado com sucesso
     */
    bool addGemToSocket(const STRUCT_ITEM& item, const STRUCT_ITEM& gemItem, uint8_t socketIndex, STRUCT_ITEM& resultItem);
    
    /**
     * @brief Carrega dados de itens do banco de dados
     * @return true se carregado com sucesso
     */
    bool loadItemsFromDatabase();
    
    /**
     * @brief Salva dados de itens no banco de dados
     * @return true se salvo com sucesso
     */
    bool saveItemsToDatabase();
};

} // namespace item
} // namespace wydbr

#endif // _ITEM_MANAGER_H_