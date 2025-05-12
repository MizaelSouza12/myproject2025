/**
 * @file priority_queue.h
 * @brief Sistema de Filas Prioritárias
 *
 * Implementação do sistema de fila prioritária para o WYDBR.
 * Este sistema permite gerenciar filas de processamento com diferentes 
 * prioridades e mecanismos de back pressure.
 */

#ifndef WYDBR_QUEUE_PRIORITY_QUEUE_H
#define WYDBR_QUEUE_PRIORITY_QUEUE_H

#include <cstdint>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace wydbr {
namespace queue {

/**
 * @enum Priority
 * @brief Níveis de prioridade para itens na fila
 */
enum class Priority {
    CRITICAL = 0,   ///< Itens de prioridade crítica, nunca são rejeitados
    HIGH = 1,       ///< Alta prioridade, raramente rejeitados
    NORMAL = 2,     ///< Prioridade normal
    LOW = 3,        ///< Baixa prioridade, frequentemente rejeitados
    BACKGROUND = 4  ///< Prioridade mais baixa, apenas processados quando a fila está vazia
};

/**
 * @struct QueueItem
 * @brief Estrutura para representar um item na fila
 * @tparam T Tipo do conteúdo do item
 */
template <typename T>
struct QueueItem {
    T data;                     ///< Dados do item
    std::string id;             ///< Identificador único do item
    Priority priority;          ///< Nível de prioridade
    std::string category;       ///< Categoria do item (para agrupamento)
    int64_t timestamp;          ///< Timestamp de criação
    int retryCount;             ///< Contador de tentativas
    
    QueueItem(T _data, Priority _priority, std::string _category, std::string _id = "")
        : data(std::move(_data))
        , priority(_priority)
        , category(std::move(_category))
        , id(std::move(_id))
        , timestamp(0)
        , retryCount(0) {}
};

/**
 * @class PriorityQueue
 * @brief Implementação C++ do sistema de Filas Prioritárias
 * @tparam T Tipo dos itens armazenados na fila
 */
template <typename T>
class PriorityQueue {
public:
    using ItemType = QueueItem<T>;
    using ProcessorFunc = std::function<bool(const ItemType&)>;
    
    /**
     * @struct QueueStats
     * @brief Estrutura para estatísticas da fila
     */
    struct QueueStats {
        std::size_t totalEnqueued;          ///< Total de itens enfileirados
        std::size_t totalProcessed;         ///< Total de itens processados
        std::size_t totalFailed;            ///< Total de falhas
        std::size_t totalRejected;          ///< Total de itens rejeitados
        std::size_t totalTimedOut;          ///< Total de timeouts
        std::size_t totalRetries;           ///< Total de retentativas
        std::size_t backPressureEvents;     ///< Número de eventos de back pressure
        double averageWaitTime;             ///< Tempo médio de espera em ms
        double averageProcessingTime;       ///< Tempo médio de processamento em ms
        
        QueueStats()
            : totalEnqueued(0)
            , totalProcessed(0)
            , totalFailed(0)
            , totalRejected(0)
            , totalTimedOut(0)
            , totalRetries(0)
            , backPressureEvents(0)
            , averageWaitTime(0)
            , averageProcessingTime(0) {}
    };
    
    /**
     * @brief Construtor
     * @param name Nome da fila
     * @param maxSize Tamanho máximo da fila
     */
    PriorityQueue(std::string name, std::size_t maxSize = 1000);
    
    /**
     * @brief Destrutor
     */
    ~PriorityQueue();
    
    /**
     * @brief Inicializa a fila
     * @param processor Função de processamento dos itens
     * @return Verdadeiro se inicializado com sucesso
     */
    bool Initialize(ProcessorFunc processor);
    
    /**
     * @brief Encerra a fila
     */
    void Shutdown();
    
    /**
     * @brief Enfileira um item
     * @param item Item a ser enfileirado
     * @return Verdadeiro se enfileirado com sucesso
     */
    bool Enqueue(const ItemType& item);
    
    /**
     * @brief Define limites de back pressure para uma categoria
     * @param category Categoria a configurar
     * @param limit Limite de itens
     */
    void SetCategoryLimit(const std::string& category, std::size_t limit);
    
    /**
     * @brief Define threshold para ativar back pressure
     * @param threshold Percentual de carga (0.0-1.0) para ativar back pressure
     */
    void SetBackPressureThreshold(double threshold);
    
    /**
     * @brief Obtém estatísticas da fila
     * @return Estrutura com estatísticas
     */
    QueueStats GetStats() const;
    
    /**
     * @brief Verifica se o back pressure está ativo
     * @return Verdadeiro se o back pressure estiver ativo
     */
    bool IsBackPressureActive() const;
    
    /**
     * @brief Limpa a fila
     */
    void Clear();
    
    /**
     * @brief Obtém o tamanho atual da fila
     * @return Número de itens na fila
     */
    std::size_t Size() const;
    
private:
    // Nome da fila
    std::string m_name;
    
    // Tamanho máximo da fila
    std::size_t m_maxSize;
    
    // Função de processamento
    ProcessorFunc m_processor;
    
    // Status de inicialização
    std::atomic<bool> m_initialized;
    
    // Status de encerramento
    std::atomic<bool> m_shutdownRequested;
    
    // Fila prioritária para cada nível de prioridade
    std::array<std::queue<ItemType>, 5> m_queues;
    
    // Limites por categoria
    std::map<std::string, std::size_t> m_categoryLimits;
    
    // Contadores por categoria
    std::map<std::string, std::size_t> m_categoryCounts;
    
    // Estatísticas
    mutable std::mutex m_statsMutex;
    QueueStats m_stats;
    
    // Mutex e variável de condição para a fila
    mutable std::mutex m_queueMutex;
    std::condition_variable m_condition;
    
    // Threshold para ativar back pressure
    double m_backPressureThreshold;
    
    // Status de back pressure
    std::atomic<bool> m_backPressureActive;
    
    // Thread de processamento
    std::unique_ptr<std::thread> m_processingThread;
    
    // Método para a thread de processamento
    void ProcessingLoop();
    
    // Método para verificar back pressure
    bool ShouldApplyBackPressure() const;
    
    // Método para atualizar estatísticas
    void UpdateStats(const ItemType& item, bool success, double processingTime);
};

} // namespace queue
} // namespace wydbr

#endif // WYDBR_QUEUE_PRIORITY_QUEUE_H