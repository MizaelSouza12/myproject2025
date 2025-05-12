/**
 * @file ThreadPool.h
 * @brief Sistema de pool de threads para processamento paralelo
 * @author WYDBR Team
 */

#ifndef WYDBR_THREADPOOL_H
#define WYDBR_THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <stdexcept>
#include <memory>
#include <type_traits>
#include <string>

#include "Logger.h"

namespace wydbr {
namespace core {

/**
 * @class ThreadPool
 * @brief Pool de threads para execução de tarefas em paralelo
 * 
 * Esta classe implementa um pool de threads para distribuir
 * a carga de trabalho entre múltiplas threads, aumentando a eficiência
 * em operações pesadas.
 */
class ThreadPool {
public:
    /**
     * @brief Construtor que inicializa o pool de threads
     * @param numThreads Número de threads no pool (0 = número de núcleos do sistema)
     * @param name Nome do pool para debug/logging
     */
    ThreadPool(size_t numThreads = 0, const std::string& name = "DefaultThreadPool") 
        : m_name(name), m_stop(false), m_paused(false), m_activeWorkers(0) {
        
        // Se numThreads for 0, usa o número de threads de hardware
        if (numThreads == 0) {
            numThreads = std::thread::hardware_concurrency();
            // Fallback se hardware_concurrency retornar 0
            if (numThreads == 0) {
                numThreads = 2;
            }
        }
        
        LOG_INFO("Inicializando ThreadPool '" + m_name + "' com " + std::to_string(numThreads) + " threads");
        
        try {
            m_workers.reserve(numThreads);
            
            // Cria as threads de trabalho
            for (size_t i = 0; i < numThreads; ++i) {
                m_workers.emplace_back([this, i] {
                    workerFunction(i);
                });
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Erro ao inicializar ThreadPool: " + std::string(e.what()));
            shutdown();
            throw;
        }
    }
    
    /**
     * @brief Destrutor que garante que todas as threads terminem
     */
    ~ThreadPool() {
        shutdown();
    }

    /**
     * @brief Encerra o pool de threads
     * @param waitForTasks Se deve aguardar as tarefas pendentes terminarem (true) ou cancelar tudo (false)
     */
    void shutdown(bool waitForTasks = true) {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            if (m_stop) {
                return;
            }
            
            m_stop = true;
            
            if (!waitForTasks) {
                // Limpa a fila se não deve esperar pelas tarefas
                while (!m_tasks.empty()) {
                    m_tasks.pop();
                }
            }
        }
        
        // Notifica todas as threads para verificarem a flag m_stop
        m_condition.notify_all();
        
        // Aguarda todas as threads terminarem
        for (auto& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        
        LOG_INFO("ThreadPool '" + m_name + "' encerrado");
    }

    /**
     * @brief Pausa a execução de novas tarefas
     * @note Tarefas já em execução continuarão até serem concluídas
     */
    void pause() {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_paused = true;
        LOG_DEBUG("ThreadPool '" + m_name + "' pausado");
    }

    /**
     * @brief Retoma a execução de tarefas
     */
    void resume() {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_paused = false;
            LOG_DEBUG("ThreadPool '" + m_name + "' retomado");
        }
        m_condition.notify_all();
    }

    /**
     * @brief Enfileira uma tarefa para execução
     * @tparam F Tipo da função
     * @tparam Args Tipos dos argumentos da função
     * @param f Função a ser executada
     * @param args Argumentos da função
     * @return std::future com resultado da execução
     */
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        
        using return_type = typename std::invoke_result<F, Args...>::type;
        
        // Cria uma tarefa compartilhada
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> result = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            // Não permite adicionar tarefas após shutdown()
            if (m_stop) {
                throw std::runtime_error("Tentativa de adicionar tarefa em ThreadPool encerrado");
            }
            
            // Adiciona a tarefa à fila
            m_tasks.emplace([task]() { (*task)(); });
        }
        
        // Notifica uma thread para processar a nova tarefa
        m_condition.notify_one();
        
        return result;
    }

    /**
     * @brief Retorna o número de threads no pool
     * @return Número de threads
     */
    size_t size() const {
        return m_workers.size();
    }

    /**
     * @brief Retorna o número de tarefas pendentes na fila
     * @return Número de tarefas
     */
    size_t queueSize() const {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        return m_tasks.size();
    }

    /**
     * @brief Retorna o número de trabalhadores ativos no momento
     * @return Número de trabalhadores ativos
     */
    size_t activeWorkers() const {
        return m_activeWorkers;
    }

    /**
     * @brief Verifica se o pool está parado
     * @return true se o pool está parado, false caso contrário
     */
    bool isStopped() const {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        return m_stop;
    }

    /**
     * @brief Verifica se o pool está pausado
     * @return true se o pool está pausado, false caso contrário
     */
    bool isPaused() const {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        return m_paused;
    }

    /**
     * @brief Obtém o nome do pool
     * @return Nome do pool
     */
    std::string getName() const {
        return m_name;
    }

private:
    /**
     * @brief Função executada por cada thread trabalhadora
     * @param workerId ID da thread trabalhadora
     */
    void workerFunction(size_t workerId) {
        LOG_DEBUG("Worker " + std::to_string(workerId) + " iniciado em ThreadPool '" + m_name + "'");

        while (true) {
            std::function<void()> task;
            
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                
                // Espera até que haja uma tarefa, ou o pool esteja parando, ou seja despausado
                m_condition.wait(lock, [this] { 
                    return m_stop || (!m_tasks.empty() && !m_paused); 
                });
                
                // Se o pool está parando e não há mais tarefas, encerra a thread
                if (m_stop && m_tasks.empty()) {
                    LOG_DEBUG("Worker " + std::to_string(workerId) + " finalizando em ThreadPool '" + m_name + "'");
                    return;
                }
                
                // Pega a próxima tarefa
                task = std::move(m_tasks.front());
                m_tasks.pop();
            }
            
            // Incrementa contador de workers ativos
            ++m_activeWorkers;
            
            try {
                // Executa a tarefa
                task();
            } catch (const std::exception& e) {
                LOG_ERROR("Exceção em worker " + std::to_string(workerId) + ": " + std::string(e.what()));
            } catch (...) {
                LOG_ERROR("Exceção desconhecida em worker " + std::to_string(workerId));
            }
            
            // Decrementa contador de workers ativos
            --m_activeWorkers;
        }
    }

private:
    std::string m_name;                               ///< Nome do pool para logging
    std::vector<std::thread> m_workers;               ///< Threads trabalhadoras
    std::queue<std::function<void()>> m_tasks;        ///< Fila de tarefas pendentes
    
    mutable std::mutex m_queueMutex;                  ///< Mutex para sincronização de acesso à fila
    std::condition_variable m_condition;              ///< Variável de condição para espera
    
    std::atomic<bool> m_stop;                         ///< Flag para sinalizar término do pool
    std::atomic<bool> m_paused;                       ///< Flag para pausar o processamento
    std::atomic<size_t> m_activeWorkers;              ///< Contador de workers ativamente processando
};

} // namespace core
} // namespace wydbr

#endif // WYDBR_THREADPOOL_H 