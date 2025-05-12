/**
 * @file priority_queue.cpp
 * @brief Implementação do arquivo priority_queue.h
 */

#include "priority_queue.h"
#include <chrono>
#include <thread>
#include <algorithm>

namespace wydbr {
namespace queue {

template <typename T>
PriorityQueue<T>::PriorityQueue(std::string name, std::size_t maxSize)
    : m_name(std::move(name))
    , m_maxSize(maxSize)
    , m_initialized(false)
    , m_shutdownRequested(false)
    , m_backPressureThreshold(0.8)
    , m_backPressureActive(false) {
    
    // Inicializar contadores de categorias
    m_categoryCounts.clear();
    
    // Inicializar estatísticas
    m_stats = QueueStats();
}

template <typename T>
PriorityQueue<T>::~PriorityQueue() {
    Shutdown();
}

template <typename T>
bool PriorityQueue<T>::Initialize(ProcessorFunc processor) {
    // Verificar se já foi inicializado
    if (m_initialized.load()) {
        return true;
    }
    
    // Armazenar processador
    m_processor = std::move(processor);
    
    // Resetar flag de shutdown
    m_shutdownRequested.store(false);
    
    // Iniciar thread de processamento
    m_processingThread = std::make_unique<std::thread>(&PriorityQueue::ProcessingLoop, this);
    
    // Marcar como inicializado
    m_initialized.store(true);
    
    return true;
}

template <typename T>
void PriorityQueue<T>::Shutdown() {
    // Se não está inicializado, não fazer nada
    if (!m_initialized.load()) {
        return;
    }
    
    // Solicitar encerramento
    m_shutdownRequested.store(true);
    
    // Notificar a thread de processamento
    m_condition.notify_all();
    
    // Aguardar a thread terminar
    if (m_processingThread && m_processingThread->joinable()) {
        m_processingThread->join();
    }
    
    // Limpar a fila
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        for (auto& q : m_queues) {
            while (!q.empty()) {
                q.pop();
            }
        }
    }
    
    // Marcar como não inicializado
    m_initialized.store(false);
}

template <typename T>
bool PriorityQueue<T>::Enqueue(const ItemType& item) {
    // Se não está inicializado ou shutdown foi solicitado, rejeitar
    if (!m_initialized.load() || m_shutdownRequested.load()) {
        return false;
    }
    
    // Verificar se é um item crítico (nunca rejeitado)
    bool isCritical = (item.priority == Priority::CRITICAL);
    
    // Verificar back pressure (exceto para itens críticos)
    if (!isCritical && ShouldApplyBackPressure()) {
        // Incrementar contador de rejeições
        std::lock_guard<std::mutex> statsLock(m_statsMutex);
        m_stats.totalRejected++;
        return false;
    }
    
    // Verificar limite da categoria
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        
        // Verificar limite da categoria (exceto para itens críticos)
        if (!isCritical) {
            auto it = m_categoryLimits.find(item.category);
            if (it != m_categoryLimits.end()) {
                auto& count = m_categoryCounts[item.category];
                if (count >= it->second) {
                    // Categoria atingiu o limite
                    std::lock_guard<std::mutex> statsLock(m_statsMutex);
                    m_stats.totalRejected++;
                    return false;
                }
            }
        }
        
        // Verificar se a fila está cheia
        if (Size() >= m_maxSize && !isCritical) {
            // Fila cheia
            std::lock_guard<std::mutex> statsLock(m_statsMutex);
            m_stats.totalRejected++;
            return false;
        }
        
        // Criar cópia do item
        ItemType newItem = item;
        
        // Adicionar timestamp
        using namespace std::chrono;
        newItem.timestamp = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
        
        // Adicionar à fila apropriada
        m_queues[static_cast<int>(newItem.priority)].push(newItem);
        
        // Incrementar contador da categoria
        m_categoryCounts[newItem.category]++;
        
        // Atualizar estatísticas
        {
            std::lock_guard<std::mutex> statsLock(m_statsMutex);
            m_stats.totalEnqueued++;
        }
        
        // Notificar a thread de processamento
        m_condition.notify_one();
        
        return true;
    }
}

template <typename T>
void PriorityQueue<T>::SetCategoryLimit(const std::string& category, std::size_t limit) {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_categoryLimits[category] = limit;
}

template <typename T>
void PriorityQueue<T>::SetBackPressureThreshold(double threshold) {
    m_backPressureThreshold = std::max(0.0, std::min(1.0, threshold));
}

template <typename T>
typename PriorityQueue<T>::QueueStats PriorityQueue<T>::GetStats() const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    return m_stats;
}

template <typename T>
bool PriorityQueue<T>::IsBackPressureActive() const {
    return m_backPressureActive.load();
}

template <typename T>
void PriorityQueue<T>::Clear() {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    
    // Limpar todas as filas
    for (auto& q : m_queues) {
        while (!q.empty()) {
            q.pop();
        }
    }
    
    // Resetar contadores de categorias
    for (auto& pair : m_categoryCounts) {
        pair.second = 0;
    }
}

template <typename T>
std::size_t PriorityQueue<T>::Size() const {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    
    std::size_t total = 0;
    for (const auto& q : m_queues) {
        total += q.size();
    }
    
    return total;
}

template <typename T>
void PriorityQueue<T>::ProcessingLoop() {
    while (!m_shutdownRequested.load()) {
        ItemType item = {}; // Valor padrão
        bool hasItem = false;
        
        // Obter próximo item
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            // Aguardar até ter um item ou shutdown for solicitado
            m_condition.wait(lock, [this]() {
                for (const auto& q : m_queues) {
                    if (!q.empty()) {
                        return true;
                    }
                }
                return m_shutdownRequested.load();
            });
            
            // Se shutdown foi solicitado, sair
            if (m_shutdownRequested.load()) {
                break;
            }
            
            // Encontrar a fila de maior prioridade com itens
            for (std::size_t i = 0; i < m_queues.size(); ++i) {
                if (!m_queues[i].empty()) {
                    item = m_queues[i].front();
                    m_queues[i].pop();
                    
                    // Decrementar contador da categoria
                    auto it = m_categoryCounts.find(item.category);
                    if (it != m_categoryCounts.end() && it->second > 0) {
                        it->second--;
                    }
                    
                    hasItem = true;
                    break;
                }
            }
        }
        
        // Processar o item
        if (hasItem) {
            using namespace std::chrono;
            
            // Calcular tempo de espera
            auto now = duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()
            ).count();
            
            double waitTime = static_cast<double>(now - item.timestamp);
            
            // Processar o item e medir tempo
            auto startTime = high_resolution_clock::now();
            bool success = m_processor(item);
            auto endTime = high_resolution_clock::now();
            
            // Calcular tempo de processamento
            double processingTime = duration_cast<milliseconds>(
                endTime - startTime
            ).count();
            
            // Atualizar estatísticas
            UpdateStats(item, success, processingTime);
            
            // Verificar back pressure baseado na carga atual
            double load = static_cast<double>(Size()) / m_maxSize;
            m_backPressureActive.store(load >= m_backPressureThreshold);
        }
    }
}

template <typename T>
bool PriorityQueue<T>::ShouldApplyBackPressure() const {
    // Verificar se o back pressure está ativo
    if (m_backPressureActive.load()) {
        // Incrementar contador de eventos de back pressure
        std::lock_guard<std::mutex> statsLock(m_statsMutex);
        m_stats.backPressureEvents++;
        return true;
    }
    
    return false;
}

template <typename T>
void PriorityQueue<T>::UpdateStats(const ItemType& item, bool success, double processingTime) {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    if (success) {
        m_stats.totalProcessed++;
        
        // Atualizar tempo médio de processamento
        if (m_stats.totalProcessed == 1) {
            m_stats.averageProcessingTime = processingTime;
            m_stats.averageWaitTime = 0; // Será atualizado depois
        } else {
            double alpha = 1.0 / m_stats.totalProcessed;
            m_stats.averageProcessingTime = (1.0 - alpha) * m_stats.averageProcessingTime + alpha * processingTime;
        }
        
        // Calcular tempo de espera
        using namespace std::chrono;
        auto now = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
        
        double waitTime = static_cast<double>(now - item.timestamp);
        
        // Atualizar tempo médio de espera
        if (m_stats.totalProcessed == 1) {
            m_stats.averageWaitTime = waitTime;
        } else {
            double alpha = 1.0 / m_stats.totalProcessed;
            m_stats.averageWaitTime = (1.0 - alpha) * m_stats.averageWaitTime + alpha * waitTime;
        }
    } else {
        m_stats.totalFailed++;
    }
}

// Explicitamente instanciar o template para tipos comuns
template class PriorityQueue<std::string>;
template class PriorityQueue<int>;
template class PriorityQueue<std::vector<uint8_t>>;
template class PriorityQueue<std::map<std::string, std::string>>;

} // namespace queue
} // namespace wydbr