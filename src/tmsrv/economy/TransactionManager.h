/**
 * @file TransactionManager.h
 * @brief Gerenciador de transações com segurança contra race conditions
 */

#ifndef WYDBR_TRANSACTION_MANAGER_H
#define WYDBR_TRANSACTION_MANAGER_H

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <atomic>
#include <chrono>

namespace WYDBR {

/**
 * Enum para tipo de transação
 */
enum class TransactionType {
    NONE,
    GOLD_TRANSFER,
    ITEM_TRANSFER,
    ITEM_PURCHASE,
    ITEM_SALE,
    SKILL_PURCHASE,
    QUEST_REWARD,
    STORAGE_DEPOSIT,
    STORAGE_WITHDRAW
};

/**
 * Enum para status de transação
 */
enum class TransactionStatus {
    PENDING,
    PROCESSING,
    COMPLETED,
    FAILED,
    CANCELLED
};

/**
 * Estrutura para transação
 */
struct Transaction {
    uint64_t id;
    TransactionType type;
    uint32_t sourceId;
    uint32_t targetId;
    uint32_t itemId;
    uint32_t quantity;
    uint64_t goldAmount;
    std::string description;
    TransactionStatus status;
    std::chrono::system_clock::time_point timestamp;
};

/**
 * Classe TransactionManager - Gerencia transações de forma segura
 * 
 * Corrige as race conditions da implementação original adicionando:
 * - Locks por entidade para prevenir race conditions
 * - Transações atômicas com rollback automático
 * - Log de auditoria detalhado
 * - Verificação de integridade antes de cada transação
 */
class TransactionManager {
public:
    /**
     * Obtém a instância (singleton)
     */
    static TransactionManager& GetInstance();
    
    /**
     * Inicializa o sistema de transações
     */
    bool Initialize();
    
    /**
     * Processa uma transação
     * 
     * @param transaction Transação a ser processada
     * @return true se a transação foi bem sucedida
     */
    bool ProcessTransaction(Transaction& transaction);
    
    /**
     * Cria uma nova transação
     * 
     * @param type Tipo da transação
     * @param sourceId ID da entidade de origem
     * @param targetId ID da entidade de destino
     * @param itemId ID do item (0 se não envolver item)
     * @param quantity Quantidade (1 se não aplicável)
     * @param goldAmount Quantidade de ouro (0 se não envolver ouro)
     * @param description Descrição da transação
     * @return Nova transação
     */
    Transaction CreateTransaction(
        TransactionType type,
        uint32_t sourceId,
        uint32_t targetId,
        uint32_t itemId = 0,
        uint32_t quantity = 1,
        uint64_t goldAmount = 0,
        const std::string& description = ""
    );
    
    /**
     * Obtém histórico de transações para uma entidade
     * 
     * @param entityId ID da entidade
     * @param maxCount Número máximo de transações a retornar
     * @return Lista de transações
     */
    std::vector<Transaction> GetTransactionHistory(uint32_t entityId, size_t maxCount = 100);
    
private:
    // Construtor privado (singleton)
    TransactionManager();
    ~TransactionManager();
    
    // Não permitir cópias
    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;
    
    // ID de transação atual
    std::atomic<uint64_t> m_nextTransactionId;
    
    // Histórico de transações
    std::vector<Transaction> m_transactionHistory;
    mutable std::shared_mutex m_historyMutex;
    
    // Locks por entidade
    std::unordered_map<uint32_t, std::unique_ptr<std::mutex>> m_entityLocks;
    mutable std::mutex m_entityLocksMutex;
    
    // Obter ou criar lock para uma entidade
    std::mutex& GetEntityLock(uint32_t entityId);
    
    // Processar diferentes tipos de transações
    bool ProcessGoldTransfer(Transaction& transaction);
    bool ProcessItemTransfer(Transaction& transaction);
    bool ProcessItemPurchase(Transaction& transaction);
    bool ProcessItemSale(Transaction& transaction);
    bool ProcessSkillPurchase(Transaction& transaction);
    bool ProcessQuestReward(Transaction& transaction);
    bool ProcessStorageDeposit(Transaction& transaction);
    bool ProcessStorageWithdraw(Transaction& transaction);
    
    // Verificações de integridade
    bool ValidateTransaction(const Transaction& transaction);
    bool VerifyGoldAmount(uint32_t entityId, uint64_t amount);
    bool VerifyItemAvailability(uint32_t entityId, uint32_t itemId, uint32_t quantity);
    
    // Adicionar transação ao histórico
    void AddTransactionToHistory(const Transaction& transaction);
};

} // namespace WYDBR

#endif // WYDBR_TRANSACTION_MANAGER_H
