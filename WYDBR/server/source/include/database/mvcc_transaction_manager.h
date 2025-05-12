/**
 * @file mvcc_transaction_manager.h
 * @brief Sistema de Controle de Concorrência Multiversão (MVCC)
 * 
 * Este arquivo implementa um sistema MVCC para gerenciar acessos concorrentes
 * a dados críticos, como inventário e status, permitindo isolamento de transações.
 */

#ifndef _MVCC_TRANSACTION_MANAGER_H_
#define _MVCC_TRANSACTION_MANAGER_H_

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <functional>
#include <chrono>

namespace wydbr {
namespace database {

/**
 * Representa uma versão de um objeto
 */
struct ObjectVersion {
    void* data;
    size_t dataSize;
    int version;
    std::string transactionId;
    bool committed;
    unsigned long long timestamp;
    
    ObjectVersion() : data(nullptr), dataSize(0), version(0), 
                    committed(false), timestamp(0) {}
};

/**
 * Status da transação
 */
enum TransactionStatus {
    TRANSACTION_ACTIVE,
    TRANSACTION_COMMITTED,
    TRANSACTION_ABORTED,
    TRANSACTION_UNKNOWN
};

/**
 * Gerencia transações MVCC
 */
class MVCCTransactionManager {
public:
    static MVCCTransactionManager& GetInstance();
    
    /**
     * Inicia uma nova transação
     * @return ID da transação
     */
    std::string BeginTransaction();
    
    /**
     * Finaliza uma transação com sucesso
     * @param transactionId ID da transação
     * @return true se sucesso, false caso contrário
     */
    bool CommitTransaction(const std::string& transactionId);
    
    /**
     * Cancela uma transação
     * @param transactionId ID da transação
     * @return true se sucesso, false caso contrário
     */
    bool AbortTransaction(const std::string& transactionId);
    
    /**
     * Obtém o status de uma transação
     * @param transactionId ID da transação
     * @return Status da transação
     */
    TransactionStatus GetTransactionStatus(const std::string& transactionId);
    
    /**
     * Lê um objeto dentro de uma transação
     * @param transactionId ID da transação
     * @param objectId ID do objeto
     * @param data Buffer para receber os dados
     * @param maxSize Tamanho máximo do buffer
     * @return Tamanho dos dados lidos ou -1 se erro
     */
    int ReadObject(const std::string& transactionId, const std::string& objectId, 
                void* data, size_t maxSize);
    
    /**
     * Escreve um objeto dentro de uma transação
     * @param transactionId ID da transação
     * @param objectId ID do objeto
     * @param data Dados a escrever
     * @param dataSize Tamanho dos dados
     * @return true se sucesso, false caso contrário
     */
    bool WriteObject(const std::string& transactionId, const std::string& objectId,
                    const void* data, size_t dataSize);
    
    /**
     * Bloqueia um objeto para acesso exclusivo
     * @param transactionId ID da transação
     * @param objectId ID do objeto
     * @return true se sucesso, false caso contrário
     */
    bool LockObject(const std::string& transactionId, const std::string& objectId);
    
    /**
     * Desbloqueia um objeto
     * @param transactionId ID da transação
     * @param objectId ID do objeto
     * @return true se sucesso, false caso contrário
     */
    bool UnlockObject(const std::string& transactionId, const std::string& objectId);
    
    /**
     * Executa manutenção do sistema MVCC
     * Limpa versões antigas e transações mortas
     */
    void PerformMaintenance();
    
private:
    MVCCTransactionManager();
    ~MVCCTransactionManager();
    
    // Versões de objetos
    std::map<std::string, std::vector<ObjectVersion>> m_objectVersions;
    
    // Bloqueios de objetos
    std::map<std::string, std::string> m_objectLocks;
    
    // Transações ativas
    std::map<std::string, TransactionStatus> m_transactions;
    
    // Tempos das transações
    std::map<std::string, unsigned long long> m_transactionTimes;
    
    // Mutex para controle de concorrência
    std::mutex m_mutex;
    
    // Gera ID único para transação
    std::string _GenerateTransactionId();
    
    // Limpa versões antigas de um objeto
    void _CleanupOldVersions(const std::string& objectId);
    
    // Obtém timestamp atual
    unsigned long long _GetTimestamp() const;
};

} // namespace database
} // namespace wydbr

#endif // _MVCC_TRANSACTION_MANAGER_H_
