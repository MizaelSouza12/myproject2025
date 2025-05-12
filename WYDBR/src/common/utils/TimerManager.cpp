/**
 * @file TimerManager.cpp
 * @brief Implementação do gerenciador de tarefas temporizadas
 */

#include "TimerManager.h"
#include <algorithm>
#include <iostream>
#include <thread>

namespace wydbr {
namespace common {
namespace utils {

TimerManager::TimerManager(size_t threadCount) 
    : running_(true), paused_(false), nextTaskId_(1) {
    
    // Se não for especificado o número de threads, usar o número de núcleos
    if (threadCount == 0) {
        threadCount = std::max(1u, std::thread::hardware_concurrency());
    }
    
    // Criar pool de threads
    for (size_t i = 0; i < threadCount; ++i) {
        workerThreads_.emplace_back(&TimerManager::WorkerFunction, this);
    }
}

TimerManager::~TimerManager() {
    // Sinalizar para todas as threads pararem
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        running_ = false;
    }
    
    // Notificar todas as threads para sair da espera
    queueCondition_.notify_all();
    
    // Esperar todas as threads terminarem
    for (std::thread& thread : workerThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

uint64_t TimerManager::ScheduleTask(Task task, Duration delay) {
    // Calcular o tempo de execução
    TimePoint executionTime = Clock::now() + delay;
    
    // Criar a tarefa
    TimerTask timerTask;
    timerTask.executionTime = executionTime;
    timerTask.task = std::move(task);
    timerTask.interval = Duration::zero();  // Não é periódica
    timerTask.cancelled = false;
    
    // Adicionar à fila
    return EnqueueTask(std::move(timerTask));
}

uint64_t TimerManager::ScheduleTaskAt(Task task, TimePoint timePoint) {
    // Criar a tarefa
    TimerTask timerTask;
    timerTask.executionTime = timePoint;
    timerTask.task = std::move(task);
    timerTask.interval = Duration::zero();  // Não é periódica
    timerTask.cancelled = false;
    
    // Adicionar à fila
    return EnqueueTask(std::move(timerTask));
}

uint64_t TimerManager::SchedulePeriodicTask(Task task, Duration interval, Duration initialDelay) {
    // Se o atraso inicial não foi especificado, usar o intervalo
    if (initialDelay == Duration::zero()) {
        initialDelay = interval;
    }
    
    // Calcular o tempo da primeira execução
    TimePoint firstExecutionTime = Clock::now() + initialDelay;
    
    // Criar a tarefa
    TimerTask timerTask;
    timerTask.executionTime = firstExecutionTime;
    timerTask.task = std::move(task);
    timerTask.interval = interval;  // Armazenar o intervalo para reescalonamento
    timerTask.cancelled = false;
    
    // Adicionar à fila
    return EnqueueTask(std::move(timerTask));
}

bool TimerManager::CancelTask(uint64_t taskId) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    // Encontrar e marcar a tarefa como cancelada
    // Nota: Não podemos removê-la diretamente da fila de prioridade,
    // então marcamos e ignoramos quando for a vez de executar
    bool found = false;
    std::vector<TimerTask> tasks;
    
    // Esvaziar a fila
    while (!taskQueue_.empty()) {
        TimerTask task = taskQueue_.top();
        taskQueue_.pop();
        
        if (task.id == taskId) {
            task.cancelled = true;
            found = true;
        }
        
        tasks.push_back(task);
    }
    
    // Reconstruir a fila
    for (const auto& task : tasks) {
        taskQueue_.push(task);
    }
    
    return found;
}

void TimerManager::PauseAllTasks() {
    paused_ = true;
}

void TimerManager::ResumeAllTasks() {
    paused_ = false;
    queueCondition_.notify_all();
}

void TimerManager::CancelAllTasks() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    // Limpar a fila
    while (!taskQueue_.empty()) {
        taskQueue_.pop();
    }
}

void TimerManager::WorkerFunction() {
    while (running_) {
        TimerTask currentTask;
        bool hasTask = false;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            // Esperar até que haja uma tarefa para executar ou o gerenciador seja finalizado
            auto waitPredicate = [this] {
                return !running_ || (!paused_ && !taskQueue_.empty() && 
                       taskQueue_.top().executionTime <= Clock::now());
            };
            
            if (!waitPredicate()) {
                if (taskQueue_.empty()) {
                    // Se não houver tarefas, esperar indefinidamente
                    queueCondition_.wait(lock, [this] { 
                        return !running_ || (!paused_ && !taskQueue_.empty()); 
                    });
                } else {
                    // Se houver tarefas, esperar até o tempo da próxima tarefa
                    queueCondition_.wait_until(lock, taskQueue_.top().executionTime, waitPredicate);
                }
            }
            
            // Verificar se devemos continuar
            if (!running_) {
                break;
            }
            
            // Verificar se estamos pausados
            if (paused_) {
                continue;
            }
            
            // Verificar se há tarefas para executar
            if (!taskQueue_.empty() && taskQueue_.top().executionTime <= Clock::now()) {
                currentTask = taskQueue_.top();
                taskQueue_.pop();
                hasTask = true;
                
                // Se for tarefa periódica, reagendar
                if (currentTask.interval.count() > 0 && !currentTask.cancelled) {
                    TimerTask nextTask = currentTask;
                    nextTask.executionTime = Clock::now() + currentTask.interval;
                    taskQueue_.push(nextTask);
                }
            }
        }
        
        // Executar a tarefa fora do lock para evitar bloqueios longos
        if (hasTask && !currentTask.cancelled) {
            try {
                currentTask.task();
            } catch (const std::exception& e) {
                // Log de erro
                std::cerr << "Exception in timer task: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown exception in timer task" << std::endl;
            }
        }
    }
}

uint64_t TimerManager::EnqueueTask(TimerTask task) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    // Atribuir um ID único
    task.id = nextTaskId_++;
    
    // Adicionar à fila
    taskQueue_.push(task);
    
    // Notificar uma thread para processar
    queueCondition_.notify_one();
    
    return task.id;
}

} // namespace utils
} // namespace common
} // namespace wydbr 