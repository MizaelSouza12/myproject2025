/**
 * @file TransactionManager.cpp
 * @brief Implementação do gerenciador de transações
 */

#include "TransactionManager.h"
#include "../Common/Utils/Logger.h"
#include "../Entity/Entity.h"
#include "../Entity/Player.h"
#include "../Entity/NPC.h"
#include "../Entity/EntityManager.h"
#include <algorithm>

namespace WYDBR {

TransactionManager& TransactionManager::GetInstance() {
    static TransactionManager instance;
    return instance;
}

TransactionManager::TransactionManager()
    : m_nextTransactionId(1) {
}

TransactionManager::~TransactionManager() {
}

bool TransactionManager::Initialize() {
    LOG_INFO("Inicializando TransactionManager");
    return true;
}

bool TransactionManager::ProcessTransaction(Transaction& transaction) {
    // Validar transação antes de processar
    if (!ValidateTransaction(transaction)) {
        LOG_ERROR("Transação inválida: ID={}, Tipo={}", 
                  transaction.id, static_cast<int>(transaction.type));
        transaction.status = TransactionStatus::FAILED;
        AddTransactionToHistory(transaction);
        return false;
    }
    
    // Obter locks para entidades envolvidas (na mesma ordem para evitar deadlocks)
    uint32_t firstEntityId = std::min(transaction.sourceId, transaction.targetId);
    uint32_t secondEntityId = std::max(transaction.sourceId, transaction.targetId);
    
    std::unique_lock<std::mutex> firstLock(GetEntityLock(firstEntityId));
    
    // Se source e target forem diferentes, obter o segundo lock
    std::unique_lock<std::mutex> secondLock;
    if (firstEntityId != secondEntityId) {
        secondLock = std::unique_lock<std::mutex>(GetEntityLock(secondEntityId));
    }
    
    // Marcar como em processamento
    transaction.status = TransactionStatus::PROCESSING;
    
    // Processar com base no tipo
    bool success = false;
    
    try {
        switch (transaction.type) {
            case TransactionType::GOLD_TRANSFER:
                success = ProcessGoldTransfer(transaction);
                break;
                
            case TransactionType::ITEM_TRANSFER:
                success = ProcessItemTransfer(transaction);
                break;
                
            case TransactionType::ITEM_PURCHASE:
                success = ProcessItemPurchase(transaction);
                break;
                
            case TransactionType::ITEM_SALE:
                success = ProcessItemSale(transaction);
                break;
                
            case TransactionType::SKILL_PURCHASE:
                success = ProcessSkillPurchase(transaction);
                break;
                
            case TransactionType::QUEST_REWARD:
                success = ProcessQuestReward(transaction);
                break;
                
            case TransactionType::STORAGE_DEPOSIT:
                success = ProcessStorageDeposit(transaction);
                break;
                
            case TransactionType::STORAGE_WITHDRAW:
                success = ProcessStorageWithdraw(transaction);
                break;
                
            default:
                LOG_ERROR("Tipo de transação desconhecido: {}", 
                          static_cast<int>(transaction.type));
                success = false;
                break;
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao processar transação: {}", e.what());
        success = false;
    }
    catch (...) {
        LOG_ERROR("Exceção desconhecida ao processar transação");
        success = false;
    }
    
    // Atualizar status e adicionar ao histórico
    transaction.status = success ? TransactionStatus::COMPLETED : TransactionStatus::FAILED;
    AddTransactionToHistory(transaction);
    
    return success;
}

Transaction TransactionManager::CreateTransaction(
    TransactionType type,
    uint32_t sourceId,
    uint32_t targetId,
    uint32_t itemId,
    uint32_t quantity,
    uint64_t goldAmount,
    const std::string& description) {
    
    Transaction transaction;
    transaction.id = m_nextTransactionId.fetch_add(1);
    transaction.type = type;
    transaction.sourceId = sourceId;
    transaction.targetId = targetId;
    transaction.itemId = itemId;
    transaction.quantity = quantity;
    transaction.goldAmount = goldAmount;
    transaction.description = description;
    transaction.status = TransactionStatus::PENDING;
    transaction.timestamp = std::chrono::system_clock::now();
    
    return transaction;
}

std::vector<Transaction> TransactionManager::GetTransactionHistory(uint32_t entityId, size_t maxCount) {
    std::shared_lock<std::shared_mutex> lock(m_historyMutex);
    
    std::vector<Transaction> result;
    result.reserve(std::min(maxCount, m_transactionHistory.size()));
    
    // Filtrar transações para a entidade específica, da mais recente para a mais antiga
    for (auto it = m_transactionHistory.rbegin(); it != m_transactionHistory.rend() && result.size() < maxCount; ++it) {
        if (it->sourceId == entityId || it->targetId == entityId) {
            result.push_back(*it);
        }
    }
    
    return result;
}

std::mutex& TransactionManager::GetEntityLock(uint32_t entityId) {
    std::lock_guard<std::mutex> lock(m_entityLocksMutex);
    
    auto it = m_entityLocks.find(entityId);
    if (it == m_entityLocks.end()) {
        // Criar novo lock para esta entidade
        auto [newIt, inserted] = m_entityLocks.emplace(entityId, std::make_unique<std::mutex>());
        return *newIt->second;
    }
    
    return *it->second;
}

bool TransactionManager::ProcessGoldTransfer(Transaction& transaction) {
    auto& entityManager = EntityManager::GetInstance();
    
    Entity* source = entityManager.GetEntity(transaction.sourceId);
    Entity* target = entityManager.GetEntity(transaction.targetId);
    
    if (!source || !target) {
        LOG_ERROR("Entidade não encontrada: source={}, target={}", 
                  transaction.sourceId, transaction.targetId);
        return false;
    }
    
    // Verificar se a origem tem gold suficiente
    if (source->GetGold() < transaction.goldAmount) {
        LOG_ERROR("Gold insuficiente: source={}, required={}, available={}", 
                  transaction.sourceId, transaction.goldAmount, source->GetGold());
        return false;
    }
    
    // Verificar se o destino pode receber o gold (limite máximo)
    if (target->GetGold() + transaction.goldAmount > target->GetMaxGold()) {
        LOG_ERROR("Limite de gold excedido: target={}, current={}, adding={}, max={}", 
                  transaction.targetId, target->GetGold(), transaction.goldAmount, target->GetMaxGold());
        return false;
    }
    
    // Realizar a transferência
    source->SetGold(source->GetGold() - transaction.goldAmount);
    target->SetGold(target->GetGold() + transaction.goldAmount);
    
    LOG_INFO("Gold transferido: source={}, target={}, amount={}", 
              transaction.sourceId, transaction.targetId, transaction.goldAmount);
    
    return true;
}

bool TransactionManager::ProcessItemTransfer(Transaction& transaction) {
    // Implementação específica para transferência de itens
    // Similar ao ProcessGoldTransfer, mas para itens
    
    // [Implementação detalhada...]
    
    return true;
}

bool TransactionManager::ProcessItemPurchase(Transaction& transaction) {
    // Implementação específica para compra de itens
    
    // [Implementação detalhada...]
    
    return true;
}

bool TransactionManager::ProcessItemSale(Transaction& transaction) {
    // Implementação específica para venda de itens
    
    // [Implementação detalhada...]
    
    return true;
}

bool TransactionManager::ProcessSkillPurchase(Transaction& transaction) {
    // Implementação específica para compra de skills
    
    // [Implementação detalhada...]
    
    return true;
}

bool TransactionManager::ProcessQuestReward(Transaction& transaction) {
    // Implementação específica para recompensas de quests
    
    // [Implementação detalhada...]
    
    return true;
}

bool TransactionManager::ProcessStorageDeposit(Transaction& transaction) {
    // Implementação específica para depósito no storage
    
    // [Implementação detalhada...]
    
    return true;
}

bool TransactionManager::ProcessStorageWithdraw(Transaction& transaction) {
    // Implementação específica para retirada do storage
    
    // [Implementação detalhada...]
    
    return true;
}

bool TransactionManager::ValidateTransaction(const Transaction& transaction) {
    // Verificar se os IDs são válidos
    if (transaction.sourceId == 0 || transaction.targetId == 0) {
        LOG_ERROR("IDs de entidade inválidos: source={}, target={}", 
                  transaction.sourceId, transaction.targetId);
        return false;
    }
    
    // Verificar se as entidades existem
    auto& entityManager = EntityManager::GetInstance();
    if (!entityManager.EntityExists(transaction.sourceId) || 
        !entityManager.EntityExists(transaction.targetId)) {
        LOG_ERROR("Entidade não encontrada: source={}, target={}", 
                  transaction.sourceId, transaction.targetId);
        return false;
    }
    
    // Verificações específicas por tipo de transação
    switch (transaction.type) {
        case TransactionType::GOLD_TRANSFER:
        case TransactionType::ITEM_PURCHASE:
        case TransactionType::SKILL_PURCHASE:
            // Verificar se a origem tem gold suficiente
            if (!VerifyGoldAmount(transaction.sourceId, transaction.goldAmount)) {
                return false;
            }
            break;
            
        case TransactionType::ITEM_TRANSFER:
        case TransactionType::ITEM_SALE:
        case TransactionType::STORAGE_DEPOSIT:
            // Verificar se a origem tem o item
            if (!VerifyItemAvailability(transaction.sourceId, transaction.itemId, transaction.quantity)) {
                return false;
            }
            break;
            
        default:
            break;
    }
    
    return true;
}

bool TransactionManager::VerifyGoldAmount(uint32_t entityId, uint64_t amount) {
    auto& entityManager = EntityManager::GetInstance();
    Entity* entity = entityManager.GetEntity(entityId);
    
    if (!entity) {
        LOG_ERROR("Entidade não encontrada: {}", entityId);
        return false;
    }
    
    if (entity->GetGold() < amount) {
        LOG_ERROR("Gold insuficiente: entity={}, required={}, available={}", 
                  entityId, amount, entity->GetGold());
        return false;
    }
    
    return true;
}

bool TransactionManager::VerifyItemAvailability(uint32_t entityId, uint32_t itemId, uint32_t quantity) {
    auto& entityManager = EntityManager::GetInstance();
    Entity* entity = entityManager.GetEntity(entityId);
    
    if (!entity) {
        LOG_ERROR("Entidade não encontrada: {}", entityId);
        return false;
    }
    
    // Verificar se a entidade possui o item na quantidade necessária
    if (!entity->HasItem(itemId, quantity)) {
        LOG_ERROR("Item não disponível: entity={}, itemId={}, required={}", 
                  entityId, itemId, quantity);
        return false;
    }
    
    return true;
}

void TransactionManager::AddTransactionToHistory(const Transaction& transaction) {
    std::unique_lock<std::shared_mutex> lock(m_historyMutex);
    
    // Adicionar ao histórico
    m_transactionHistory.push_back(transaction);
    
    // Limitar tamanho do histórico (manter apenas as últimas 10000 transações)
    constexpr size_t MAX_HISTORY_SIZE = 10000;
    if (m_transactionHistory.size() > MAX_HISTORY_SIZE) {
        m_transactionHistory.erase(m_transactionHistory.begin(), 
                                  m_transactionHistory.begin() + 
                                  (m_transactionHistory.size() - MAX_HISTORY_SIZE));
    }
    
    // Log da transação
    const char* statusStr = "Unknown";
    switch (transaction.status) {
        case TransactionStatus::PENDING:    statusStr = "Pending"; break;
        case TransactionStatus::PROCESSING: statusStr = "Processing"; break;
        case TransactionStatus::COMPLETED:  statusStr = "Completed"; break;
        case TransactionStatus::FAILED:     statusStr = "Failed"; break;
        case TransactionStatus::CANCELLED:  statusStr = "Cancelled"; break;
    }
    
    LOG_INFO("Transação: ID={}, Tipo={}, Source={}, Target={}, Item={}, Qty={}, Gold={}, Status={}", 
             transaction.id, static_cast<int>(transaction.type), 
             transaction.sourceId, transaction.targetId, 
             transaction.itemId, transaction.quantity, 
             transaction.goldAmount, statusStr);
}

} // namespace WYDBR
