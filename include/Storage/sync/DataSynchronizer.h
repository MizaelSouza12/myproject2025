/**
 * @file DataSynchronizer.h
 * @brief Sistema de sincronização de dados entre memória e banco de dados
 */

#ifndef WYDBR_DATA_SYNCHRONIZER_H
#define WYDBR_DATA_SYNCHRONIZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace WYDBR {
namespace Storage {

/**
 * Tipos de operação de sincronização
 */
enum class SyncOperation {
    Create,             // Criar
    Update,             // Atualizar
    Delete,             // Excluir
    Bulk                // Operação em lote
};

/**
 * Estado de sincronização
 */
enum class SyncState {
    Pending,            // Pendente
    InProgress,         // Em progresso
    Completed,          // Concluído
    Failed,             // Falhou
    Retrying            // Tentando novamente
};

/**
 * Prioridade de sincronização
 */
enum class SyncPriority {
    Low,                // Baixa prioridade
    Normal,             // Prioridade normal
    High,               // Alta prioridade
    Critical            // Prioridade crítica
};

/**
 * Item de sincronização
 */
struct SyncItem {
    std::string entityType;      // Tipo de entidade
    std::string entityId;        // ID da entidade
    SyncOperation operation;     // Operação
    std::string data;            // Dados em formato JSON
    uint64_t timestamp;          // Timestamp
    SyncPriority priority;       // Prioridade
    int retryCount;              // Contagem de tentativas
    SyncState state;             // Estado
    std::string errorMessage;    // Mensagem de erro
};

/**
 * Callback para eventos de sincronização
 */
using SyncEventCallback = std::function<void(const SyncItem&)>;

/**
 * Comparador para fila de prioridade
 */
struct SyncItemComparator {
    bool operator()(const SyncItem& a, const SyncItem& b) const {
        // Ordenar primeiro por prioridade, depois por timestamp
        if (static_cast<int>(a.priority) != static_cast<int>(b.priority)) {
            return static_cast<int>(a.priority) < static_cast<int>(b.priority);
        }
        return a.timestamp > b.timestamp;
    }
};

/**
 * Configuração do sincronizador
 */
struct SyncConfig {
    std::chrono::milliseconds syncInterval;     // Intervalo de sincronização
    int maxRetries;                            // Máximo de tentativas
    bool persistQueue;                         // Persistir fila em disco
    bool autoRetry;                            // Tentar novamente automaticamente
    int maxQueueSize;                          // Tamanho máximo da fila
    int bulkSize;                              // Tamanho de lote
};

/**
 * Sistema de sincronização de dados
 */
class DataSynchronizer {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o sincronizador
     */
    static DataSynchronizer& GetInstance();
    
    /**
     * Inicializa o sincronizador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const SyncConfig& config);
    
    /**
     * Inicia o sincronizador
     * @return true se iniciado com sucesso
     */
    bool Start();
    
    /**
     * Para o sincronizador
     */
    void Stop();
    
    /**
     * Enfileira um item para sincronização
     * @param item Item a sincronizar
     * @return true se enfileirado com sucesso
     */
    bool Enqueue(const SyncItem& item);
    
    /**
     * Enfileira um lote de itens para sincronização
     * @param items Itens a sincronizar
     * @return Número de itens enfileirados com sucesso
     */
    int EnqueueBatch(const std::vector<SyncItem>& items);
    
    /**
     * Enfileira uma operação de criação
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param data Dados em formato JSON
     * @param priority Prioridade
     * @return true se enfileirado com sucesso
     */
    bool EnqueueCreate(const std::string& entityType, const std::string& entityId, 
                     const std::string& data, SyncPriority priority = SyncPriority::Normal);
    
    /**
     * Enfileira uma operação de atualização
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param data Dados em formato JSON
     * @param priority Prioridade
     * @return true se enfileirado com sucesso
     */
    bool EnqueueUpdate(const std::string& entityType, const std::string& entityId, 
                     const std::string& data, SyncPriority priority = SyncPriority::Normal);
    
    /**
     * Enfileira uma operação de exclusão
     * @param entityType Tipo de entidade
     * @param entityId ID da entidade
     * @param priority Prioridade
     * @return true se enfileirado com sucesso
     */
    bool EnqueueDelete(const std::string& entityType, const std::string& entityId, 
                     SyncPriority priority = SyncPriority::Normal);
    
    /**
     * Processa a fila de sincronização
     * @param maxItems Número máximo de itens a processar (0 = todos)
     * @return Número de itens processados
     */
    int ProcessQueue(int maxItems = 0);
    
    /**
     * Registra um callback para eventos de sincronização
     * @param callback Função a ser chamada quando eventos ocorrerem
     */
    void RegisterEventCallback(SyncEventCallback callback);
    
    /**
     * Obtém estatísticas de sincronização
     * @return Mapa de estatísticas
     */
    std::unordered_map<std::string, int64_t> GetStatistics() const;
    
    /**
     * Limpa a fila de sincronização
     * @return Número de itens removidos
     */
    int ClearQueue();
    
    /**
     * Tenta novamente itens falhos
     * @return Número de itens repostos na fila
     */
    int RetryFailedItems();
    
    /**
     * Salva a fila em disco
     * @return true se salvo com sucesso
     */
    bool SaveQueueToDisk();
    
    /**
     * Carrega a fila do disco
     * @return true se carregado com sucesso
     */
    bool LoadQueueFromDisk();
    
    /**
     * Verifica se o sincronizador está em execução
     * @return true se estiver em execução
     */
    bool IsRunning() const;
    
private:
    // Construtor e destrutor privados (singleton)
    DataSynchronizer();
    ~DataSynchronizer();
    
    // Não permitir cópias
    DataSynchronizer(const DataSynchronizer&) = delete;
    DataSynchronizer& operator=(const DataSynchronizer&) = delete;
    
    // Configuração
    SyncConfig m_config;
    
    // Fila de sincronização com prioridade
    std::priority_queue<SyncItem, std::vector<SyncItem>, SyncItemComparator> m_queue;
    
    // Fila de itens falhos
    std::vector<SyncItem> m_failedItems;
    
    // Thread de sincronização
    std::thread m_syncThread;
    
    // Sinais para controle da thread
    std::atomic<bool> m_running;
    std::condition_variable m_wakeupSignal;
    
    // Mutex para thread-safety
    mutable std::mutex m_queueMutex;
    mutable std::mutex m_failedMutex;
    std::mutex m_wakeMutex;
    
    // Callbacks
    std::vector<SyncEventCallback> m_callbacks;
    std::mutex m_callbacksMutex;
    
    // Estatísticas
    mutable std::mutex m_statsMutex;
    int64_t m_totalEnqueued;
    int64_t m_totalProcessed;
    int64_t m_totalFailed;
    int64_t m_totalSuccessful;
    int64_t m_totalRetries;
    
    // Métodos privados
    void SyncThreadFunc();
    bool ProcessSyncItem(SyncItem& item);
    void NotifyCallbacks(const SyncItem& item);
};

} // namespace Storage
} // namespace WYDBR

#endif // WYDBR_DATA_SYNCHRONIZER_H
