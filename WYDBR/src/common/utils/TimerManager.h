/**
 * @file TimerManager.h
 * @brief Gerenciador de tarefas temporizadas
 */

#ifndef WYDBR_TIMER_MANAGER_H_
#define WYDBR_TIMER_MANAGER_H_

#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>

namespace wydbr {
namespace common {
namespace utils {

/**
 * @class TimerManager
 * @brief Gerencia tarefas temporizadas com um pool de threads
 */
class TimerManager {
public:
    using Task = std::function<void()>;
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::milliseconds;

    /**
     * @brief Construtor
     * @param threadCount Número de threads no pool (padrão: número de núcleos)
     */
    explicit TimerManager(size_t threadCount = 0);

    /**
     * @brief Destrutor
     */
    ~TimerManager();

    /**
     * @brief Agenda uma tarefa para execução após um tempo específico
     * 
     * @param task Tarefa a executar
     * @param delay Atraso antes da execução
     * @return ID da tarefa agendada
     */
    uint64_t ScheduleTask(Task task, Duration delay);

    /**
     * @brief Agenda uma tarefa para execução em um momento específico
     * 
     * @param task Tarefa a executar
     * @param timePoint Momento para execução
     * @return ID da tarefa agendada
     */
    uint64_t ScheduleTaskAt(Task task, TimePoint timePoint);

    /**
     * @brief Agenda uma tarefa para execução periódica
     * 
     * @param task Tarefa a executar
     * @param interval Intervalo entre execuções
     * @param initialDelay Atraso inicial antes da primeira execução (padrão: igual ao intervalo)
     * @return ID da tarefa agendada
     */
    uint64_t SchedulePeriodicTask(Task task, Duration interval, Duration initialDelay = Duration::zero());

    /**
     * @brief Cancela uma tarefa agendada
     * 
     * @param taskId ID da tarefa a cancelar
     * @return true se a tarefa foi cancelada com sucesso
     */
    bool CancelTask(uint64_t taskId);

    /**
     * @brief Pausa todas as tarefas
     */
    void PauseAllTasks();

    /**
     * @brief Resume todas as tarefas pausadas
     */
    void ResumeAllTasks();

    /**
     * @brief Cancela todas as tarefas
     */
    void CancelAllTasks();

private:
    struct TimerTask {
        uint64_t id;
        TimePoint executionTime;
        Task task;
        Duration interval;  // Zero para tarefas não periódicas
        bool cancelled;

        // Operador de comparação para a priority queue
        bool operator>(const TimerTask& other) const {
            return executionTime > other.executionTime;
        }
    };

    using TaskQueue = std::priority_queue<TimerTask, std::vector<TimerTask>, std::greater<TimerTask>>;

    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    TaskQueue taskQueue_;
    std::vector<std::thread> workerThreads_;
    std::atomic<bool> running_;
    std::atomic<bool> paused_;
    std::atomic<uint64_t> nextTaskId_;

    /**
     * @brief Função principal do worker thread
     */
    void WorkerFunction();

    /**
     * @brief Adiciona uma tarefa à fila
     * 
     * @param task Tarefa a adicionar
     * @return ID da tarefa
     */
    uint64_t EnqueueTask(TimerTask task);
};

} // namespace utils
} // namespace common
} // namespace wydbr

#endif // WYDBR_TIMER_MANAGER_H_ 