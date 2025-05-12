/**
 * InventoryManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/item/InventoryManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _INVENTORY_MANAGER_H_
#define _INVENTORY_MANAGER_H_

/**
 * @file InventoryManager.h
 * @brief Gerenciador de inventário do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de inventário do WYDBR,
 * responsável por manipular os itens dos jogadores, armazenamento, equipamentos,
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

#include "../GlobalDef.h"
#include "../Basedef.h"
#include "../TM_Item.h"
#include "ItemManager.h"

namespace wydbr {
namespace item {

/**
 * @brief Códigos de resultado para operações de inventário
 */
enum /**
 * Classe WYDInventoryResult
 * 
 * Esta classe implementa a funcionalidade WYDInventoryResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InventoryResult {
    SUCCESS = 0,                     // Operação bem-sucedida
    FAILED = 1,                      // Falha genérica
    INVALID_SLOT = 2,                // Slot inválido
    INVALID_ITEM = 3,                // Item inválido
    NOT_ENOUGH_SPACE = 4,            // Espaço insuficiente
    INVALID_CHARACTER = 5,           // Personagem inválido
    ITEM_NOT_FOUND = 6,              // Item não encontrado
    ITEM_LOCKED = 7,                 // Item bloqueado
    CANT_EQUIP = 8,                  // Não pode equipar
    CANT_UNEQUIP = 9,                // Não pode desequipar
    CANT_USE = 10,                   // Não pode usar
    CANT_DROP = 11,                  // Não pode descartar
    CANT_MOVE = 12,                  // Não pode mover
    CANT_TRADE = 13,                 // Não pode negociar
    INSUFFICIENT_REQUIREMENTS = 14,  // Requisitos insuficientes
    INVENTORY_LOCKED = 15,           // Inventário bloqueado
    BANK_LOCKED = 16,                // Banco bloqueado
    STORAGE_LOCKED = 17,             // Armazenamento bloqueado
    INSUFFICIENT_GOLD = 18,          // Ouro insuficiente
    STACK_LIMIT_REACHED = 19,        // Limite de pilha atingido
    WEIGHT_LIMIT_REACHED = 20,       // Limite de peso atingido
    ALREADY_EQUIPPED = 21,           // Já equipado
    NOT_EQUIPPED = 22,               // Não equipado
    CUSTOM_1 = 23,                   // Personalizado 1
    CUSTOM_2 = 24,                   // Personalizado 2
    CUSTOM_3 = 25,                   // Personalizado 3
    UNKNOWN = 26                     // Desconhecido
};

/**
 * @brief Tipo de operação de inventário
 */
enum /**
 * Classe WYDInventoryOperation
 * 
 * Esta classe implementa a funcionalidade WYDInventoryOperation conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InventoryOperation {
    NONE = 0,               // Nenhum
    ADD = 1,                // Adicionar
    REMOVE = 2,             // Remover
    MOVE = 3,               // Mover
    EQUIP = 4,              // Equipar
    UNEQUIP = 5,            // Desequipar
    USE = 6,                // Usar
    DROP = 7,               // Descartar
    SPLIT = 8,              // Dividir
    STACK = 9,              // Empilhar
    LOCK = 10,              // Bloquear
    UNLOCK = 11,            // Desbloquear
    DEPOSIT = 12,           // Depositar
    WITHDRAW = 13,          // Retirar
    REPAIR = 14,            // Reparar
    UPGRADE = 15,           // Aprimorar
    TRADE = 16,             // Negociar
    CUSTOM_1 = 17,          // Personalizado 1
    CUSTOM_2 = 18,          // Personalizado 2
    CUSTOM_3 = 19,          // Personalizado 3
    UNKNOWN = 20            // Desconhecido
};

/**
 * @brief Tipo de armazenamento
 */
enum /**
 * Classe WYDStorageType
 * 
 * Esta classe implementa a funcionalidade WYDStorageType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class StorageType {
    NONE = 0,               // Nenhum
    INVENTORY = 1,          // Inventário
    EQUIPMENT = 2,          // Equipamento
    BANK = 3,               // Banco
    GUILD_STORAGE = 4,      // Armazenamento de guilda
    TRADE = 5,              // Negociação
    CUSTOM_1 = 6,           // Personalizado 1
    CUSTOM_2 = 7,           // Personalizado 2
    CUSTOM_3 = 8,           // Personalizado 3
    UNKNOWN = 9             // Desconhecido
};

/**
 * @brief Tipo de evento de inventário
 */
enum /**
 * Classe WYDInventoryEventType
 * 
 * Esta classe implementa a funcionalidade WYDInventoryEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InventoryEventType {
    NONE = 0,               // Nenhum
    ITEM_ADDED = 1,         // Item adicionado
    ITEM_REMOVED = 2,       // Item removido
    ITEM_MOVED = 3,         // Item movido
    ITEM_EQUIPPED = 4,      // Item equipado
    ITEM_UNEQUIPPED = 5,    // Item desequipado
    ITEM_USED = 6,          // Item usado
    ITEM_DROPPED = 7,       // Item descartado
    ITEM_SPLIT = 8,         // Item dividido
    ITEM_STACKED = 9,       // Item empilhado
    ITEM_LOCKED = 10,       // Item bloqueado
    ITEM_UNLOCKED = 11,     // Item desbloqueado
    ITEM_DEPOSITED = 12,    // Item depositado
    ITEM_WITHDRAWN = 13,    // Item retirado
    ITEM_REPAIRED = 14,     // Item reparado
    ITEM_UPGRADED = 15,     // Item aprimorado
    ITEM_TRADED = 16,       // Item negociado
    CUSTOM_1 = 17,          // Personalizado 1
    CUSTOM_2 = 18,          // Personalizado 2
    CUSTOM_3 = 19,          // Personalizado 3
    UNKNOWN = 20            // Desconhecido
};

/**
 * @brief Tipo de slot de inventário
 */
enum /**
 * Classe WYDSlotType
 * 
 * Esta classe implementa a funcionalidade WYDSlotType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SlotType {
    NONE = 0,               // Nenhum
    INVENTORY = 1,          // Inventário
    EQUIPMENT = 2,          // Equipamento
    BANK = 3,               // Banco
    GUILD_STORAGE = 4,      // Armazenamento de guilda
    TRADE = 5,              // Negociação
    CUSTOM_1 = 6,           // Personalizado 1
    CUSTOM_2 = 7,           // Personalizado 2
    CUSTOM_3 = 8,           // Personalizado 3
    UNKNOWN = 9             // Desconhecido
};

/**
 * @brief Evento de inventário
 */
struct InventoryEvent {
    uint32_t EventID;           // ID do evento
    uint32_t CharacterID;       // ID do personagem
    InventoryEventType Type;    // Tipo
    uint32_t ItemID;            // ID do item
    SlotType SourceSlotType;    // Tipo de slot de origem
    uint16_t SourceSlotIndex;   // Índice de slot de origem
    SlotType TargetSlotType;    // Tipo de slot de destino
    uint16_t TargetSlotIndex;   // Índice de slot de destino
    uint32_t Timestamp;         // Timestamp
    std::string Details;        // Detalhes
    
    InventoryEvent() : EventID(0), CharacterID(0), Type(InventoryEventType::NONE),
        ItemID(0), SourceSlotType(SlotType::NONE), SourceSlotIndex(0),
        TargetSlotType(SlotType::NONE), TargetSlotIndex(0), Timestamp(0) {}
};

/**
 * @brief Slot de inventário
 */
struct InventorySlot {
    STRUCT_ITEM Item;           // Item
    bool IsLocked;              // Está bloqueado
    uint32_t LockExpiration;    // Expiração do bloqueio
    
    InventorySlot() : IsLocked(false), LockExpiration(0) {
        memset(&Item, 0, sizeof(STRUCT_ITEM));
    }
};

/**
 * @brief Inventário do personagem
 */
struct CharacterInventory {
    uint32_t CharacterID;                   // ID do personagem
    InventorySlot Inventory[MAX_CARRY];     // Inventário
    InventorySlot Equipment[MAX_EQUIP];     // Equipamento
    InventorySlot Bank[MAX_STORE];          // Banco
    uint32_t Gold;                          // Ouro
    uint32_t BankGold;                      // Ouro no banco
    uint16_t TotalWeight;                   // Peso total
    uint16_t MaxWeight;                     // Peso máximo
    bool InventoryLocked;                   // Inventário bloqueado
    bool BankLocked;                        // Banco bloqueado
    
    CharacterInventory() : CharacterID(0), Gold(0), BankGold(0),
        TotalWeight(0), MaxWeight(0), InventoryLocked(false), BankLocked(false) {
        for (int i = 0; i < MAX_CARRY; i++) {
            memset(&Inventory[i], 0, sizeof(InventorySlot));
        }
        for (int i = 0; i < MAX_EQUIP; i++) {
            memset(&Equipment[i], 0, sizeof(InventorySlot));
        }
        for (int i = 0; i < MAX_STORE; i++) {
            memset(&Bank[i], 0, sizeof(InventorySlot));
        }
    }
};

/**
 * @brief Armazenamento de guilda
 */
struct GuildStorage {
    uint32_t GuildID;                   // ID da guilda
    InventorySlot Items[MAX_STORE];     // Itens
    uint32_t Gold;                      // Ouro
    bool IsLocked;                      // Está bloqueado
    
    GuildStorage() : GuildID(0), Gold(0), IsLocked(false) {
        for (int i = 0; i < MAX_STORE; i++) {
            memset(&Items[i], 0, sizeof(InventorySlot));
        }
    }
};

/**
 * @brief Sessão de negociação
 */
struct TradeSession {
    uint32_t TradeID;               // ID da negociação
    uint32_t Character1ID;          // ID do personagem 1
    uint32_t Character2ID;          // ID do personagem 2
    InventorySlot Items1[MAX_TRADE]; // Itens do personagem 1
    InventorySlot Items2[MAX_TRADE]; // Itens do personagem 2
    uint32_t Gold1;                 // Ouro do personagem 1
    uint32_t Gold2;                 // Ouro do personagem 2
    bool IsConfirmed1;              // Está confirmado pelo personagem 1
    bool IsConfirmed2;              // Está confirmado pelo personagem 2
    uint32_t StartTime;             // Tempo de início
    bool IsCompleted;               // Está concluído
    bool IsCancelled;               // Está cancelado
    
    TradeSession() : TradeID(0), Character1ID(0), Character2ID(0),
        Gold1(0), Gold2(0), IsConfirmed1(false), IsConfirmed2(false),
        StartTime(0), IsCompleted(false), IsCancelled(false) {
        for (int i = 0; i < MAX_TRADE; i++) {
            memset(&Items1[i], 0, sizeof(InventorySlot));
            memset(&Items2[i], 0, sizeof(InventorySlot));
        }
    }
};

/**
 * @brief Estatísticas de inventário
 */
struct InventoryStats {
    uint32_t TotalItems;            // Total de itens
    uint32_t TotalEquipped;         // Total equipado
    uint32_t TotalBank;             // Total no banco
    uint32_t TotalGold;             // Total de ouro
    uint32_t TotalBankGold;         // Total de ouro no banco
    uint32_t TotalGuildGold;        // Total de ouro de guilda
    uint32_t ItemsAddedToday;       // Itens adicionados hoje
    uint32_t ItemsRemovedToday;     // Itens removidos hoje
    uint32_t ItemsTraded;           // Itens negociados
    
    InventoryStats() : TotalItems(0), TotalEquipped(0), TotalBank(0),
        TotalGold(0), TotalBankGold(0), TotalGuildGold(0),
        ItemsAddedToday(0), ItemsRemovedToday(0), ItemsTraded(0) {}
};

/**
 * @brief Configuração de inventário
 */
struct InventoryConfig {
    uint16_t MaxCarryWeight;        // Peso máximo para carregar
    uint16_t WeightPerStr;          // Peso por STR
    uint32_t BankFee;               // Taxa do banco
    uint32_t BankInterest;          // Juros do banco
    uint32_t TradeDistance;         // Distância de negociação
    uint32_t TradeTimeout;          // Timeout de negociação
    bool AllowNegativeGold;         // Permitir ouro negativo
    uint32_t DefaultLockDuration;   // Duração padrão de bloqueio
    
    InventoryConfig() : MaxCarryWeight(500), WeightPerStr(4),
        BankFee(1000), BankInterest(1), TradeDistance(5),
        TradeTimeout(120), AllowNegativeGold(false),
        DefaultLockDuration(3600) {}
};

/**
 * @brief Callback de inventário
 */
using InventoryCallback = std::function<void(const InventoryEvent&)>;

/**
 * @brief Gerenciador de inventário
 * 
 * Esta classe é responsável por gerenciar o inventário dos jogadores.
 */
/**
 * Classe WYDInventoryManager
 * 
 * Esta classe implementa a funcionalidade WYDInventoryManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InventoryManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static InventoryManager& getInstance() {
        static InventoryManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @param itemManager Gerenciador de itens
     * @return true se inicializado com sucesso
     */
    bool initialize(ItemManager* itemManager);
    
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
     * @brief Carrega a configuração de inventário
     * @return true se carregado com sucesso
     */
    bool loadInventoryConfig();
    
    /**
     * @brief Carrega o inventário de um personagem
     * @param characterID ID do personagem
     * @return true se carregado com sucesso
     */
    bool loadCharacterInventory(uint32_t characterID);
    
    /**
     * @brief Salva o inventário de um personagem
     * @param characterID ID do personagem
     * @return true se salvo com sucesso
     */
    bool saveCharacterInventory(uint32_t characterID);
    
    /**
     * @brief Carrega o armazenamento de uma guilda
     * @param guildID ID da guilda
     * @return true se carregado com sucesso
     */
    bool loadGuildStorage(uint32_t guildID);
    
    /**
     * @brief Salva o armazenamento de uma guilda
     * @param guildID ID da guilda
     * @return true se salvo com sucesso
     */
    bool saveGuildStorage(uint32_t guildID);
    
    /**
     * @brief Adiciona um item ao inventário
     * @param characterID ID do personagem
     * @param item Item
     * @param slotIndex Índice do slot (0xFFFF para automático)
     * @param count Quantidade
     * @return Resultado da operação
     */
    InventoryResult addItemToInventory(uint32_t characterID, const STRUCT_ITEM& item, uint16_t slotIndex = 0xFFFF, uint16_t count = 0);
    
    /**
     * @brief Adiciona um item ao inventário por código e lote
     * @param characterID ID do personagem
     * @param itemID ID do item
     * @param itemLevel Nível do item
     * @param amount Quantidade
     * @param slotIndex Índice do slot (0xFFFF para automático)
     * @return Resultado da operação
     */
    InventoryResult addItemToInventoryByID(uint32_t characterID, uint16_t itemID, uint16_t itemLevel = 0, uint16_t amount = 1, uint16_t slotIndex = 0xFFFF);
    
    /**
     * @brief Remove um item do inventário
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @param count Quantidade (0 para todos)
     * @return Resultado da operação
     */
    InventoryResult removeItemFromInventory(uint32_t characterID, uint16_t slotIndex, uint16_t count = 0);
    
    /**
     * @brief Move um item dentro do inventário
     * @param characterID ID do personagem
     * @param sourceSlotIndex Índice do slot de origem
     * @param targetSlotIndex Índice do slot de destino
     * @param count Quantidade (0 para todos)
     * @return Resultado da operação
     */
    InventoryResult moveItemInInventory(uint32_t characterID, uint16_t sourceSlotIndex, uint16_t targetSlotIndex, uint16_t count = 0);
    
    /**
     * @brief Equipamento um item
     * @param characterID ID do personagem
     * @param inventorySlotIndex Índice do slot de inventário
     * @param equipmentSlotIndex Índice do slot de equipamento
     * @return Resultado da operação
     */
    InventoryResult equipItem(uint32_t characterID, uint16_t inventorySlotIndex, uint16_t equipmentSlotIndex);
    
    /**
     * @brief Desequipa um item
     * @param characterID ID do personagem
     * @param equipmentSlotIndex Índice do slot de equipamento
     * @param inventorySlotIndex Índice do slot de inventário (0xFFFF para automático)
     * @return Resultado da operação
     */
    InventoryResult unequipItem(uint32_t characterID, uint16_t equipmentSlotIndex, uint16_t inventorySlotIndex = 0xFFFF);
    
    /**
     * @brief Usa um item
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @param targetCharacterID ID do personagem alvo (0 para si mesmo)
     * @return Resultado da operação
     */
    InventoryResult useItem(uint32_t characterID, uint16_t slotIndex, uint32_t targetCharacterID = 0);
    
    /**
     * @brief Descarta um item
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @param count Quantidade (0 para todos)
     * @return Resultado da operação
     */
    InventoryResult dropItem(uint32_t characterID, uint16_t slotIndex, uint16_t count = 0);
    
    /**
     * @brief Divide um item
     * @param characterID ID do personagem
     * @param sourceSlotIndex Índice do slot de origem
     * @param targetSlotIndex Índice do slot de destino
     * @param count Quantidade
     * @return Resultado da operação
     */
    InventoryResult splitItem(uint32_t characterID, uint16_t sourceSlotIndex, uint16_t targetSlotIndex, uint16_t count);
    
    /**
     * @brief Empilha um item
     * @param characterID ID do personagem
     * @param sourceSlotIndex Índice do slot de origem
     * @param targetSlotIndex Índice do slot de destino
     * @return Resultado da operação
     */
    InventoryResult stackItem(uint32_t characterID, uint16_t sourceSlotIndex, uint16_t targetSlotIndex);
    
    /**
     * @brief Bloqueia um item
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @param duration Duração (0 para padrão)
     * @return Resultado da operação
     */
    InventoryResult lockItem(uint32_t characterID, uint16_t slotIndex, uint32_t duration = 0);
    
    /**
     * @brief Desbloqueia um item
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @return Resultado da operação
     */
    InventoryResult unlockItem(uint32_t characterID, uint16_t slotIndex);
    
    /**
     * @brief Deposita um item no banco
     * @param characterID ID do personagem
     * @param inventorySlotIndex Índice do slot de inventário
     * @param bankSlotIndex Índice do slot de banco (0xFFFF para automático)
     * @param count Quantidade (0 para todos)
     * @return Resultado da operação
     */
    InventoryResult depositItemToBank(uint32_t characterID, uint16_t inventorySlotIndex, uint16_t bankSlotIndex = 0xFFFF, uint16_t count = 0);
    
    /**
     * @brief Retira um item do banco
     * @param characterID ID do personagem
     * @param bankSlotIndex Índice do slot de banco
     * @param inventorySlotIndex Índice do slot de inventário (0xFFFF para automático)
     * @param count Quantidade (0 para todos)
     * @return Resultado da operação
     */
    InventoryResult withdrawItemFromBank(uint32_t characterID, uint16_t bankSlotIndex, uint16_t inventorySlotIndex = 0xFFFF, uint16_t count = 0);
    
    /**
     * @brief Deposita um item no armazenamento de guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @param inventorySlotIndex Índice do slot de inventário
     * @param guildSlotIndex Índice do slot de guilda (0xFFFF para automático)
     * @param count Quantidade (0 para todos)
     * @return Resultado da operação
     */
    InventoryResult depositItemToGuildStorage(uint32_t characterID, uint32_t guildID, uint16_t inventorySlotIndex, uint16_t guildSlotIndex = 0xFFFF, uint16_t count = 0);
    
    /**
     * @brief Retira um item do armazenamento de guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @param guildSlotIndex Índice do slot de guilda
     * @param inventorySlotIndex Índice do slot de inventário (0xFFFF para automático)
     * @param count Quantidade (0 para todos)
     * @return Resultado da operação
     */
    InventoryResult withdrawItemFromGuildStorage(uint32_t characterID, uint32_t guildID, uint16_t guildSlotIndex, uint16_t inventorySlotIndex = 0xFFFF, uint16_t count = 0);
    
    /**
     * @brief Repara um item
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @param npcID ID do NPC
     * @return Resultado da operação
     */
    InventoryResult repairItem(uint32_t characterID, uint16_t slotIndex, uint32_t npcID = 0);
    
    /**
     * @brief Aprimora um item
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @param upgradeType Tipo de aprimoramento
     * @param upgradeValue Valor de aprimoramento
     * @param npcID ID do NPC
     * @return Resultado da operação
     */
    InventoryResult upgradeItem(uint32_t characterID, uint16_t slotIndex, uint16_t upgradeType, uint16_t upgradeValue, uint32_t npcID = 0);
    
    /**
     * @brief Adiciona ouro
     * @param characterID ID do personagem
     * @param amount Quantidade
     * @return Resultado da operação
     */
    InventoryResult addGold(uint32_t characterID, uint32_t amount);
    
    /**
     * @brief Remove ouro
     * @param characterID ID do personagem
     * @param amount Quantidade
     * @return Resultado da operação
     */
    InventoryResult removeGold(uint32_t characterID, uint32_t amount);
    
    /**
     * @brief Deposita ouro no banco
     * @param characterID ID do personagem
     * @param amount Quantidade
     * @return Resultado da operação
     */
    InventoryResult depositGoldToBank(uint32_t characterID, uint32_t amount);
    
    /**
     * @brief Retira ouro do banco
     * @param characterID ID do personagem
     * @param amount Quantidade
     * @return Resultado da operação
     */
    InventoryResult withdrawGoldFromBank(uint32_t characterID, uint32_t amount);
    
    /**
     * @brief Deposita ouro no armazenamento de guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @param amount Quantidade
     * @return Resultado da operação
     */
    InventoryResult depositGoldToGuildStorage(uint32_t characterID, uint32_t guildID, uint32_t amount);
    
    /**
     * @brief Retira ouro do armazenamento de guilda
     * @param characterID ID do personagem
     * @param guildID ID da guilda
     * @param amount Quantidade
     * @return Resultado da operação
     */
    InventoryResult withdrawGoldFromGuildStorage(uint32_t characterID, uint32_t guildID, uint32_t amount);
    
    /**
     * @brief Inicia uma negociação
     * @param character1ID ID do personagem 1
     * @param character2ID ID do personagem 2
     * @return ID da negociação ou 0 em caso de erro
     */
    uint32_t startTrade(uint32_t character1ID, uint32_t character2ID);
    
    /**
     * @brief Adiciona um item à negociação
     * @param characterID ID do personagem
     * @param tradeID ID da negociação
     * @param inventorySlotIndex Índice do slot de inventário
     * @param tradeSlotIndex Índice do slot de negociação (0xFFFF para automático)
     * @param count Quantidade (0 para todos)
     * @return Resultado da operação
     */
    InventoryResult addItemToTrade(uint32_t characterID, uint32_t tradeID, uint16_t inventorySlotIndex, uint16_t tradeSlotIndex = 0xFFFF, uint16_t count = 0);
    
    /**
     * @brief Remove um item da negociação
     * @param characterID ID do personagem
     * @param tradeID ID da negociação
     * @param tradeSlotIndex Índice do slot de negociação
     * @return Resultado da operação
     */
    InventoryResult removeItemFromTrade(uint32_t characterID, uint32_t tradeID, uint16_t tradeSlotIndex);
    
    /**
     * @brief Adiciona ouro à negociação
     * @param characterID ID do personagem
     * @param tradeID ID da negociação
     * @param amount Quantidade
     * @return Resultado da operação
     */
    InventoryResult addGoldToTrade(uint32_t characterID, uint32_t tradeID, uint32_t amount);
    
    /**
     * @brief Remove ouro da negociação
     * @param characterID ID do personagem
     * @param tradeID ID da negociação
     * @param amount Quantidade (0 para todos)
     * @return Resultado da operação
     */
    InventoryResult removeGoldFromTrade(uint32_t characterID, uint32_t tradeID, uint32_t amount = 0);
    
    /**
     * @brief Confirma uma negociação
     * @param characterID ID do personagem
     * @param tradeID ID da negociação
     * @return Resultado da operação
     */
    InventoryResult confirmTrade(uint32_t characterID, uint32_t tradeID);
    
    /**
     * @brief Cancela uma negociação
     * @param characterID ID do personagem
     * @param tradeID ID da negociação
     * @return Resultado da operação
     */
    InventoryResult cancelTrade(uint32_t characterID, uint32_t tradeID);
    
    /**
     * @brief Conclui uma negociação
     * @param tradeID ID da negociação
     * @return Resultado da operação
     */
    InventoryResult completeTrade(uint32_t tradeID);
    
    /**
     * @brief Bloqueia o inventário de um personagem
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    InventoryResult lockInventory(uint32_t characterID);
    
    /**
     * @brief Desbloqueia o inventário de um personagem
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    InventoryResult unlockInventory(uint32_t characterID);
    
    /**
     * @brief Bloqueia o banco de um personagem
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    InventoryResult lockBank(uint32_t characterID);
    
    /**
     * @brief Desbloqueia o banco de um personagem
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    InventoryResult unlockBank(uint32_t characterID);
    
    /**
     * @brief Bloqueia o armazenamento de uma guilda
     * @param guildID ID da guilda
     * @return Resultado da operação
     */
    InventoryResult lockGuildStorage(uint32_t guildID);
    
    /**
     * @brief Desbloqueia o armazenamento de uma guilda
     * @param guildID ID da guilda
     * @return Resultado da operação
     */
    InventoryResult unlockGuildStorage(uint32_t guildID);
    
    /**
     * @brief Verifica se um item pode ser equipado
     * @param characterID ID do personagem
     * @param item Item
     * @return true se puder ser equipado
     */
    bool canEquipItem(uint32_t characterID, const STRUCT_ITEM& item);
    
    /**
     * @brief Verifica se um item pode ser usado
     * @param characterID ID do personagem
     * @param item Item
     * @return true se puder ser usado
     */
    bool canUseItem(uint32_t characterID, const STRUCT_ITEM& item);
    
    /**
     * @brief Verifica se um slot está vazio
     * @param slotType Tipo de slot
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @param guildID ID da guilda (para armazenamento de guilda)
     * @param tradeID ID da negociação (para negociação)
     * @return true se estiver vazio
     */
    bool isSlotEmpty(SlotType slotType, uint32_t characterID, uint16_t slotIndex, uint32_t guildID = 0, uint32_t tradeID = 0);
    
    /**
     * @brief Verifica o peso atual de um personagem
     * @param characterID ID do personagem
     * @return Peso atual
     */
    uint16_t getCurrentWeight(uint32_t characterID);
    
    /**
     * @brief Verifica o peso máximo de um personagem
     * @param characterID ID do personagem
     * @return Peso máximo
     */
    uint16_t getMaxWeight(uint32_t characterID);
    
    /**
     * @brief Recalcula o peso de um personagem
     * @param characterID ID do personagem
     * @return true se recalculado com sucesso
     */
    bool recalculateWeight(uint32_t characterID);
    
    /**
     * @brief Obtém o inventário de um personagem
     * @param characterID ID do personagem
     * @return Inventário ou nullptr se não encontrado
     */
    const CharacterInventory* getCharacterInventory(uint32_t characterID);
    
    /**
     * @brief Obtém o armazenamento de uma guilda
     * @param guildID ID da guilda
     * @return Armazenamento ou nullptr se não encontrado
     */
    const GuildStorage* getGuildStorage(uint32_t guildID);
    
    /**
     * @brief Obtém um item em um slot
     * @param slotType Tipo de slot
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @param guildID ID da guilda (para armazenamento de guilda)
     * @param tradeID ID da negociação (para negociação)
     * @return Item ou nullptr se não encontrado
     */
    const STRUCT_ITEM* getItemInSlot(SlotType slotType, uint32_t characterID, uint16_t slotIndex, uint32_t guildID = 0, uint32_t tradeID = 0);
    
    /**
     * @brief Obtém o ouro de um personagem
     * @param characterID ID do personagem
     * @return Ouro
     */
    uint32_t getCharacterGold(uint32_t characterID);
    
    /**
     * @brief Obtém o ouro no banco de um personagem
     * @param characterID ID do personagem
     * @return Ouro
     */
    uint32_t getCharacterBankGold(uint32_t characterID);
    
    /**
     * @brief Obtém o ouro no armazenamento de uma guilda
     * @param guildID ID da guilda
     * @return Ouro
     */
    uint32_t getGuildStorageGold(uint32_t guildID);
    
    /**
     * @brief Obtém uma sessão de negociação
     * @param tradeID ID da negociação
     * @return Sessão ou nullptr se não encontrada
     */
    const TradeSession* getTradeSession(uint32_t tradeID);
    
    /**
     * @brief Obtém as negociações ativas de um personagem
     * @param characterID ID do personagem
     * @return Lista de IDs de negociações
     */
    std::vector<uint32_t> getActiveTradesByCharacter(uint32_t characterID);
    
    /**
     * @brief Encontra itens no inventário de um personagem
     * @param characterID ID do personagem
     * @param itemID ID do item (0 para todos)
     * @param includeBank Incluir banco
     * @param includeEquipment Incluir equipamento
     * @return Lista de índices de slots
     */
    std::vector<uint16_t> findItemsInInventory(uint32_t characterID, uint16_t itemID = 0, bool includeBank = false, bool includeEquipment = false);
    
    /**
     * @brief Encontra um slot vazio no inventário
     * @param characterID ID do personagem
     * @param slotType Tipo de slot
     * @param guildID ID da guilda (para armazenamento de guilda)
     * @param tradeID ID da negociação (para negociação)
     * @return Índice do slot ou 0xFFFF se não encontrado
     */
    uint16_t findEmptySlot(uint32_t characterID, SlotType slotType, uint32_t guildID = 0, uint32_t tradeID = 0);
    
    /**
     * @brief Registra um callback de inventário
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerInventoryCallback(InventoryCallback callback);
    
    /**
     * @brief Remove um callback de inventário
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterInventoryCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém as estatísticas de inventário
     * @return Estatísticas
     */
    InventoryStats getInventoryStats() const;
    
    /**
     * @brief Obtém a configuração de inventário
     * @return Configuração
     */
    InventoryConfig getInventoryConfig() const;
    
    /**
     * @brief Define a configuração de inventário
     * @param config Configuração
     */
    void setInventoryConfig(const InventoryConfig& config);
    
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
    
private:
    /**
     * @brief Construtor privado
     */
    InventoryManager();
    
    /**
     * @brief Destrutor privado
     */
    ~InventoryManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    InventoryManager(const InventoryManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    InventoryManager& operator=(const InventoryManager&) = delete;
    
    // Gerenciador de itens
    ItemManager* _itemManager;
    
    // Cache de inventário
    std::unordered_map<uint32_t, CharacterInventory> _characterInventories;
    std::unordered_map<uint32_t, GuildStorage> _guildStorages;
    std::unordered_map<uint32_t, TradeSession> _tradeSessions;
    std::atomic<uint32_t> _nextTradeID;
    
    // Inventários modificados
    std::unordered_set<uint32_t> _modifiedCharacterInventories;
    std::unordered_set<uint32_t> _modifiedGuildStorages;
    
    // Negociações
    std::unordered_map<uint32_t, std::vector<uint32_t>> _characterTrades;
    
    // Eventos
    std::vector<InventoryEvent> _events;
    std::atomic<uint32_t> _nextEventID;
    
    // Estatísticas
    InventoryStats _inventoryStats;
    
    // Configuração
    InventoryConfig _inventoryConfig;
    
    // Callbacks
    std::vector<InventoryCallback> _inventoryCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _inventoryMutex;
    mutable std::mutex _guildStorageMutex;
    mutable std::mutex _tradeMutex;
    mutable std::mutex _eventMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Timers
    uint32_t _autoSaveTimer;
    uint32_t _tradeExpirationTimer;
    uint32_t _itemLockExpirationTimer;
    
    /**
     * @brief Checa negociações expiradas
     * @param currentTime Tempo atual
     */
    void checkExpiredTrades(uint32_t currentTime);
    
    /**
     * @brief Checa bloqueios de itens expirados
     * @param currentTime Tempo atual
     */
    void checkExpiredItemLocks(uint32_t currentTime);
    
    /**
     * @brief Salva inventários modificados
     */
    void saveModifiedInventories();
    
    /**
     * @brief Adiciona um evento
     * @param characterID ID do personagem
     * @param type Tipo
     * @param itemID ID do item
     * @param sourceSlotType Tipo de slot de origem
     * @param sourceSlotIndex Índice de slot de origem
     * @param targetSlotType Tipo de slot de destino
     * @param targetSlotIndex Índice de slot de destino
     * @param details Detalhes
     * @return ID do evento
     */
    uint32_t addEvent(uint32_t characterID, InventoryEventType type, uint32_t itemID,
                   SlotType sourceSlotType, uint16_t sourceSlotIndex,
                   SlotType targetSlotType, uint16_t targetSlotIndex,
                   const std::string& details = "");
    
    /**
     * @brief Executa callbacks de inventário
     * @param event Evento
     */
    void executeInventoryCallbacks(const InventoryEvent& event);
    
    /**
     * @brief Atualiza as estatísticas
     */
    void updateInventoryStats();
    
    /**
     * @brief Obtém um slot
     * @param slotType Tipo de slot
     * @param characterID ID do personagem
     * @param slotIndex Índice do slot
     * @param guildID ID da guilda (para armazenamento de guilda)
     * @param tradeID ID da negociação (para negociação)
     * @return Slot ou nullptr se não encontrado
     */
    InventorySlot* getSlot(SlotType slotType, uint32_t characterID, uint16_t slotIndex, uint32_t guildID = 0, uint32_t tradeID = 0);
    
    /**
     * @brief Calcula o peso de um item
     * @param item Item
     * @return Peso
     */
    uint16_t calculateItemWeight(const STRUCT_ITEM& item);
    
    /**
     * @brief Verifica se um item pode ser empilhado com outro
     * @param item1 Item 1
     * @param item2 Item 2
     * @return true se puder ser empilhado
     */
    bool canStackItems(const STRUCT_ITEM& item1, const STRUCT_ITEM& item2);
    
    /**
     * @brief Empilha dois itens
     * @param targetItem Item alvo
     * @param sourceItem Item fonte
     * @param count Quantidade (0 para todos)
     * @return Quantidade restante
     */
    uint16_t stackItems(STRUCT_ITEM& targetItem, STRUCT_ITEM& sourceItem, uint16_t count = 0);
    
    /**
     * @brief Divide um item
     * @param sourceItem Item fonte
     * @param targetItem Item alvo
     * @param count Quantidade
     * @return true se dividido com sucesso
     */
    bool splitItem(STRUCT_ITEM& sourceItem, STRUCT_ITEM& targetItem, uint16_t count);
    
    /**
     * @brief Carrega o inventário de um personagem do banco de dados
     * @param characterID ID do personagem
     * @return true se carregado com sucesso
     */
    bool loadCharacterInventoryFromDB(uint32_t characterID);
    
    /**
     * @brief Salva o inventário de um personagem no banco de dados
     * @param characterInventory Inventário
     * @return true se salvo com sucesso
     */
    bool saveCharacterInventoryToDB(const CharacterInventory& characterInventory);
    
    /**
     * @brief Carrega o armazenamento de uma guilda do banco de dados
     * @param guildID ID da guilda
     * @return true se carregado com sucesso
     */
    bool loadGuildStorageFromDB(uint32_t guildID);
    
    /**
     * @brief Salva o armazenamento de uma guilda no banco de dados
     * @param guildStorage Armazenamento
     * @return true se salvo com sucesso
     */
    bool saveGuildStorageToDB(const GuildStorage& guildStorage);
};

} // namespace item
} // namespace wydbr

#endif // _INVENTORY_MANAGER_H_