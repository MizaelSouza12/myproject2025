/**
 * InventoryManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/item/InventoryManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef INVENTORYMANAGER_H
#define INVENTORYMANAGER_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "ItemTypes.h"

namespace wydbr {


/**
 * @file InventoryManager.h
 * @brief Gerenciador de inventário para o WYD
 * 
 * Este arquivo contém a definição do gerenciador de inventário que
 * lida com operações de inventário de personagens. No WYD original,
 * essas operações estavam espalhadas em várias funções. Esta classe
 * organiza e aprimora essas funcionalidades.
 */

namespace wyd {
namespace server {

// Forward declarations
class CPlayer;

/**
 * @brief Resultado de operação de inventário
 * 
 * Esta enumeração contém os possíveis resultados de uma operação de inventário.
 * No WYD original, isso era feito com códigos de erro numéricos.
 */
enum /**
 * Classe WYDInventoryResult
 * 
 * Esta classe implementa a funcionalidade WYDInventoryResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InventoryResult {
    SUCCESS = 0,             // Operação bem-sucedida
    INVALID_SLOT = 1,        // Slot inválido
    EMPTY_SLOT = 2,          // Slot vazio
    ITEM_LOCKED = 3,         // Item bloqueado
    INVENTORY_FULL = 4,      // Inventário cheio
    NOT_ENOUGH_ITEMS = 5,    // Quantidade insuficiente
    NOT_ENOUGH_GOLD = 6,     // Gold insuficiente
    CANT_EQUIP = 7,          // Não pode equipar
    ALREADY_EQUIPPED = 8,    // Já equipado
    CANT_MOVE = 9,           // Não pode mover
    CANT_TRADE = 10,         // Não pode trocar
    CANT_DROP = 11,          // Não pode dropar
    CANT_USE = 12,           // Não pode usar
    BROKEN_ITEM = 13,        // Item quebrado
    REQUIREMENTS_NOT_MET = 14, // Requisitos não atendidos
    ITEMS_NOT_STACKABLE = 15,  // Itens não stackáveis
    STORAGE_FULL = 16,       // Cargo cheio
    DATABASE_ERROR = 17,     // Erro de banco de dados
    ALREADY_TRADING = 18,    // Já em comércio
    SHOP_UNAVAILABLE = 19,   // Loja indisponível
    AUCTION_UNAVAILABLE = 20, // Leilão indisponível
    MAIL_UNAVAILABLE = 21,   // Correio indisponível
    UNKNOWN_ERROR = 22,      // Erro desconhecido
};

/**
 * @brief Evento de inventário
 * 
 * Esta enumeração contém os possíveis eventos de inventário para logging.
 */
enum /**
 * Classe WYDInventoryEvent
 * 
 * Esta classe implementa a funcionalidade WYDInventoryEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InventoryEvent {
    ITEM_ADDED = 0,          // Item adicionado
    ITEM_REMOVED = 1,        // Item removido
    ITEM_MOVED = 2,          // Item movido
    ITEM_EQUIPPED = 3,       // Item equipado
    ITEM_UNEQUIPPED = 4,     // Item desequipado
    ITEM_USED = 5,           // Item usado
    ITEM_DROPPED = 6,        // Item dropado
    ITEM_TRADED = 7,         // Item trocado
    ITEM_BOUGHT = 8,         // Item comprado
    ITEM_SOLD = 9,           // Item vendido
    ITEM_REPAIRED = 10,      // Item reparado
    ITEM_REFINED = 11,       // Item refinado
    ITEM_STORED = 12,        // Item armazenado
    ITEM_RETRIEVED = 13,     // Item recuperado
    GOLD_ADDED = 14,         // Gold adicionado
    GOLD_REMOVED = 15,       // Gold removido
    GOLD_TRADED = 16,        // Gold trocado
};

/**
 * @brief Registro de evento de inventário para logging
 * 
 * Esta estrutura armazena informações sobre um evento de inventário
 * para fins de auditoria e logging.
 */
struct InventoryEventLog {
    InventoryEvent eventType;    // Tipo de evento
    DWORD timestamp;             // Timestamp do evento
    DWORD characterId;           // ID do personagem
    WORD slot;                   // Slot do inventário
    STRUCT_ITEM item;            // Item envolvido
    DWORD goldAmount;            // Quantidade de gold (se aplicável)
    DWORD targetCharId;          // ID do personagem alvo (se aplicável)
    
    InventoryEventLog()
        : eventType(InventoryEvent::ITEM_ADDED)
        , timestamp(0)
        , characterId(0)
        , slot(0)
        , goldAmount(0)
        , targetCharId(0)
    {
    }
};

/**
 * @brief Gerenciador de inventário (não existia no WYD original)
 * 
 * Esta classe gerencia operações de inventário para um personagem.
 * No WYD original, essas operações estavam espalhadas em várias funções
 * dentro da classe do personagem e em outros lugares. Esta classe
 * centraliza e organiza essas operações, além de adicionar logs e
 * verificações de segurança.
 */
/**
 * Classe WYDInventoryManager
 * 
 * Esta classe implementa a funcionalidade WYDInventoryManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CInventoryManager {
public:
    /**
     * @brief Construtor
     * @param player Ponteiro para o jogador dono do inventário
     */
    CInventoryManager(CPlayer* player);
    
    /**
     * @brief Destrutor
     */
    ~CInventoryManager();
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Obtém o inventário do jogador
     * @return Referência para o inventário
     */
    STRUCT_INVENTORY& GetInventory();
    
    /**
     * @brief Obtém o inventário do jogador (const)
     * @return Referência const para o inventário
     */
    const STRUCT_INVENTORY& GetInventory() const;
    
    /**
     * @brief Obtém um item pelo slot
     * @param slot Slot do item
     * @return Ponteiro para o item, ou nullptr se slot inválido
     */
    STRUCT_ITEM* GetItem(int slot);
    
    /**
     * @brief Obtém um item pelo slot (const)
     * @param slot Slot do item
     * @return Ponteiro const para o item, ou nullptr se slot inválido
     */
    const STRUCT_ITEM* GetItem(int slot) const;
    
    /**
     * @brief Obtém um item equipado pelo slot
     * @param slot Slot de equipamento
     * @return Ponteiro para o item, ou nullptr se slot inválido
     */
    STRUCT_ITEM* GetEquippedItem(int slot);
    
    /**
     * @brief Obtém um item equipado pelo slot (const)
     * @param slot Slot de equipamento
     * @return Ponteiro const para o item, ou nullptr se slot inválido
     */
    const STRUCT_ITEM* GetEquippedItem(int slot) const;
    
    /**
     * @brief Obtém a quantidade de gold
     * @return Quantidade de gold
     */
    DWORD GetGold() const;
    
    /**
     * @brief Adiciona um item ao inventário
     * @param item Item a adicionar
     * @param autoStack Auto-agrupar com itens similares se stackável
     * @param notify Notificar o jogador
     * @return Par com o resultado da operação e o slot onde foi adicionado
     */
    std::pair<InventoryResult, int> AddItem(const STRUCT_ITEM& item, bool autoStack = true, bool notify = true);
    
    /**
     * @brief Remove um item do inventário
     * @param slot Slot do item
     * @param amount Quantidade a remover (para stackáveis)
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult RemoveItem(int slot, WORD amount = 0, bool notify = true);
    
    /**
     * @brief Remove um item pelo ID
     * @param itemId ID do item
     * @param amount Quantidade a remover
     * @param notify Notificar o jogador
     * @return Par com o resultado da operação e a quantidade removida
     */
    std::pair<InventoryResult, WORD> RemoveItemById(WORD itemId, WORD amount, bool notify = true);
    
    /**
     * @brief Move um item dentro do inventário
     * @param srcSlot Slot de origem
     * @param dstSlot Slot de destino
     * @param amount Quantidade a mover (para stackáveis)
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult MoveItem(int srcSlot, int dstSlot, WORD amount = 0, bool notify = true);
    
    /**
     * @brief Equipa um item
     * @param srcSlot Slot do inventário
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult EquipItem(int srcSlot, bool notify = true);
    
    /**
     * @brief Desequipa um item
     * @param equipSlot Slot de equipamento
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult UnequipItem(int equipSlot, bool notify = true);
    
    /**
     * @brief Usa um item
     * @param slot Slot do item
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult UseItem(int slot, bool notify = true);
    
    /**
     * @brief Dropa um item no chão
     * @param slot Slot do item
     * @param amount Quantidade a dropar (para stackáveis)
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult DropItem(int slot, WORD amount = 0, bool notify = true);
    
    /**
     * @brief Adiciona gold ao inventário
     * @param amount Quantidade a adicionar
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult AddGold(DWORD amount, bool notify = true);
    
    /**
     * @brief Remove gold do inventário
     * @param amount Quantidade a remover
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult RemoveGold(DWORD amount, bool notify = true);
    
    /**
     * @brief Repara um item
     * @param slot Slot do item
     * @param free Se a reparação é gratuita
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult RepairItem(int slot, bool free = false, bool notify = true);
    
    /**
     * @brief Armazena um item no cargo
     * @param srcSlot Slot do inventário
     * @param amount Quantidade a armazenar (para stackáveis)
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult StoreItem(int srcSlot, WORD amount = 0, bool notify = true);
    
    /**
     * @brief Recupera um item do cargo
     * @param cargoSlot Slot do cargo
     * @param amount Quantidade a recuperar (para stackáveis)
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult RetrieveItem(int cargoSlot, WORD amount = 0, bool notify = true);
    
    /**
     * @brief Verifica se um item pode ser equipado
     * @param slot Slot do item
     * @return true se o item puder ser equipado
     */
    bool CanEquipItem(int slot) const;
    
    /**
     * @brief Verifica se um item pode ser usado
     * @param slot Slot do item
     * @return true se o item puder ser usado
     */
    bool CanUseItem(int slot) const;
    
    /**
     * @brief Verifica se um item pode ser dropado
     * @param slot Slot do item
     * @return true se o item puder ser dropado
     */
    bool CanDropItem(int slot) const;
    
    /**
     * @brief Verifica se um item pode ser vendido
     * @param slot Slot do item
     * @return true se o item puder ser vendido
     */
    bool CanSellItem(int slot) const;
    
    /**
     * @brief Verifica se um item pode ser trocado
     * @param slot Slot do item
     * @return true se o item puder ser trocado
     */
    bool CanTradeItem(int slot) const;
    
    /**
     * @brief Verifica se um item atende aos requisitos do jogador
     * @param slot Slot do item
     * @return true se o item atender aos requisitos
     */
    bool MeetsRequirements(int slot) const;
    
    /**
     * @brief Verifica se o inventário está cheio
     * @return true se o inventário estiver cheio
     */
    bool IsInventoryFull() const;
    
    /**
     * @brief Conta quantos slots estão vazios
     * @return Número de slots vazios
     */
    int CountEmptySlots() const;
    
    /**
     * @brief Procura um slot vazio
     * @return Slot vazio, ou -1 se não houver
     */
    int FindEmptySlot() const;
    
    /**
     * @brief Conta quantos itens de um ID específico existem
     * @param itemId ID do item
     * @return Quantidade total
     */
    int CountItemsById(WORD itemId) const;
    
    /**
     * @brief Procura um item pelo ID
     * @param itemId ID do item
     * @return Slot do item, ou -1 se não encontrado
     */
    int FindItemById(WORD itemId) const;
    
    /**
     * @brief Bloqueia ou desbloqueia um item
     * @param slot Slot do item
     * @param locked true para bloquear, false para desbloquear
     * @param notify Notificar o jogador
     * @return Resultado da operação
     */
    InventoryResult SetItemLocked(int slot, bool locked, bool notify = true);
    
    /**
     * @brief Salva o inventário no banco de dados
     * @return true se salvo com sucesso, false caso contrário
     */
    bool SaveInventory();
    
    /**
     * @brief Carrega o inventário do banco de dados
     * @return true se carregado com sucesso, false caso contrário
     */
    bool LoadInventory();
    
    /**
     * @brief Limpa o inventário
     */
    void ClearInventory();
    
    /**
     * @brief Envia o inventário para o cliente
     */
    void SendInventoryToClient();
    
    /**
     * @brief Envia atualização de gold para o cliente
     */
    void SendGoldUpdate();
    
    /**
     * @brief Envia atualização de um slot para o cliente
     * @param slot Slot a atualizar
     */
    void SendSlotUpdate(int slot);
    
    /**
     * @brief Registra um evento de inventário
     * @param eventType Tipo de evento
     * @param slot Slot envolvido
     * @param item Item envolvido
     * @param goldAmount Quantidade de gold (se aplicável)
     * @param targetCharId ID do personagem alvo (se aplicável)
     */
    void LogInventoryEvent(InventoryEvent eventType, WORD slot, const STRUCT_ITEM& item, 
                          DWORD goldAmount = 0, DWORD targetCharId = 0);
    
private:
    /**
     * @brief Verifica se um slot é válido
     * @param slot Slot a verificar
     * @return true se o slot for válido
     */
    bool IsValidSlot(int slot) const;
    
    /**
     * @brief Verifica se um slot de equipamento é válido
     * @param slot Slot a verificar
     * @return true se o slot for válido
     */
    bool IsValidEquipSlot(int slot) const;
    
    /**
     * @brief Obtém o slot de equipamento de um item pelo tipo
     * @param itemId ID do item
     * @return Slot de equipamento, ou -1 se não for equipável
     */
    int GetEquipSlotByItemType(WORD itemId) const;
    
    /**
     * @brief Obtém o preço de reparo de um item
     * @param slot Slot do item
     * @return Preço de reparo
     */
    DWORD GetRepairPrice(int slot) const;
    
    /**
     * @brief Atualiza a interface do usuário após uma operação de inventário
     * @param slot Slot atualizado
     */
    void UpdateClientUI(int slot);
    
    /**
     * @brief Cria uma notificação de evento de inventário
     * @param eventType Tipo de evento
     * @param slot Slot envolvido
     * @param result Resultado da operação
     */
    void NotifyInventoryEvent(InventoryEvent eventType, int slot, InventoryResult result);
    
    CPlayer* player_;                              // Jogador dono do inventário
    STRUCT_INVENTORY inventory_;                   // Inventário do jogador
    STRUCT_ITEM equippedItems_[MAX_EQUIP_SLOT];    // Itens equipados
    std::mutex inventoryMutex_;                    // Mutex para proteção
    std::vector<InventoryEventLog> eventLog_;      // Log de eventos
    bool initialized_;                             // Flag de inicialização
};

} // namespace server
} // namespace wyd

#endif // INVENTORYMANAGER_H

} // namespace wydbr
