#ifndef WYDSTUDIO_MULTITASKING_CORE_H
#define WYDSTUDIO_MULTITASKING_CORE_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>
#include <any>

namespace wydstudio {
namespace core {

// Forward declarations
class TaskScheduler;
class EventBus;
class ResourceManager;

/**
 * @brief Representa uma tarefa executável no sistema multitarefa
 */
class Task {
public:
    enum class Priority {
        CRITICAL = 0,
        HIGH = 1,
        NORMAL = 2,
        LOW = 3,
        BACKGROUND = 4
    };

    enum class State {
        PENDING,
        RUNNING,
        PAUSED,
        COMPLETED,
        FAILED,
        CANCELED
    };

    using TaskID = uint64_t;
    using ResultVariant = std::variant<
        std::monostate,     // No result
        int,                // Integer result
        double,             // Floating point result
        std::string,        // String result 
        bool,               // Boolean result
        std::vector<uint8_t> // Binary data result
    >;

    using TaskFunction = std::function<ResultVariant(const std::unordered_map<std::string, std::any>&)>;

    Task(TaskID id, std::string name, TaskFunction func, 
         Priority priority = Priority::NORMAL,
         std::unordered_map<std::string, std::any> params = {});
    
    virtual ~Task() = default;

    // Core methods
    TaskID getId() const { return m_id; }
    std::string getName() const { return m_name; }
    Priority getPriority() const { return m_priority; }
    State getState() const { return m_state; }
    
    // Execution control
    void execute();
    bool cancel();
    bool pause();
    bool resume();
    
    // Result handling
    bool isCompleted() const { return m_state == State::COMPLETED; }
    bool hasFailed() const { return m_state == State::FAILED; }
    ResultVariant getResult() const { return m_result; }
    std::string getErrorMessage() const { return m_errorMessage; }
    
    // Dependencies
    void addDependency(TaskID dependency);
    bool areDependenciesMet() const;
    std::vector<TaskID> getDependencies() const { return m_dependencies; }

    // Progress tracking
    void setProgress(float progress);
    float getProgress() const { return m_progress; }
    
    // Metadata and debugging
    std::chrono::milliseconds getExecutionTime() const;
    std::string getStateString() const;
    void setTag(const std::string& tag) { m_tag = tag; }
    std::string getTag() const { return m_tag; }

private:
    TaskID m_id;
    std::string m_name;
    TaskFunction m_function;
    Priority m_priority;
    std::atomic<State> m_state;
    std::unordered_map<std::string, std::any> m_parameters;
    std::vector<TaskID> m_dependencies;
    ResultVariant m_result;
    std::string m_errorMessage;
    std::atomic<float> m_progress;
    std::string m_tag;
    
    // Timing
    std::chrono::steady_clock::time_point m_startTime;
    std::chrono::steady_clock::time_point m_endTime;
    std::chrono::milliseconds m_executionTime;
    
    // Mutex for state changes
    mutable std::mutex m_stateMutex;
};

/**
 * @brief Gerenciador de agendamento e execução de tarefas
 */
class TaskScheduler {
public:
    using TaskPtr = std::shared_ptr<Task>;
    
    TaskScheduler(size_t numThreads = std::thread::hardware_concurrency());
    ~TaskScheduler();
    
    // Task creation and management
    Task::TaskID scheduleTask(const std::string& name, 
                         Task::TaskFunction function,
                         Task::Priority priority = Task::Priority::NORMAL,
                         const std::unordered_map<std::string, std::any>& params = {});
    
    Task::TaskID scheduleDelayedTask(const std::string& name,
                                Task::TaskFunction function,
                                std::chrono::milliseconds delay,
                                Task::Priority priority = Task::Priority::NORMAL,
                                const std::unordered_map<std::string, std::any>& params = {});
    
    Task::TaskID schedulePeriodicTask(const std::string& name,
                                 Task::TaskFunction function,
                                 std::chrono::milliseconds interval,
                                 Task::Priority priority = Task::Priority::NORMAL,
                                 const std::unordered_map<std::string, std::any>& params = {});
    
    // Task manipulation
    bool cancelTask(Task::TaskID taskId);
    bool pauseTask(Task::TaskID taskId);
    bool resumeTask(Task::TaskID taskId);
    
    // Task dependencies
    bool addTaskDependency(Task::TaskID taskId, Task::TaskID dependencyId);
    
    // Task queries
    TaskPtr getTask(Task::TaskID taskId) const;
    std::vector<TaskPtr> getTasksByName(const std::string& name) const;
    std::vector<TaskPtr> getTasksByTag(const std::string& tag) const;
    std::vector<TaskPtr> getTasksByState(Task::State state) const;
    
    // Scheduler control
    void start();
    void stop();
    void pause();
    void resume();
    
    // Statistics and monitoring
    size_t getActiveTaskCount() const;
    size_t getPendingTaskCount() const;
    size_t getCompletedTaskCount() const;
    size_t getFailedTaskCount() const;
    float getSystemLoad() const;
    std::vector<std::pair<std::string, size_t>> getTaskTypeDistribution() const;

    // Thread pool management
    void setThreadCount(size_t count);
    size_t getThreadCount() const;
    
private:
    struct DelayedTask {
        TaskPtr task;
        std::chrono::steady_clock::time_point scheduledTime;
        
        bool operator>(const DelayedTask& other) const {
            return scheduledTime > other.scheduledTime;
        }
    };
    
    struct PeriodicTaskInfo {
        TaskPtr task;
        Task::TaskFunction function;
        std::chrono::milliseconds interval;
        std::unordered_map<std::string, std::any> params;
        Task::Priority priority;
        std::chrono::steady_clock::time_point nextExecutionTime;
    };
    
    void workerThread();
    void processDelayedTasks();
    void processPeriodicTasks();
    Task::TaskID generateTaskId();
    TaskPtr selectNextTask();
    
    // Task collections
    std::unordered_map<Task::TaskID, TaskPtr> m_tasks;
    std::priority_queue<TaskPtr, std::vector<TaskPtr>, 
                        std::function<bool(const TaskPtr&, const TaskPtr&)>> m_taskQueue;
    std::priority_queue<DelayedTask, std::vector<DelayedTask>, 
                         std::greater<DelayedTask>> m_delayedTasks;
    std::vector<PeriodicTaskInfo> m_periodicTasks;
    
    // Thread management
    std::vector<std::thread> m_workers;
    std::atomic<bool> m_running;
    std::atomic<bool> m_paused;
    size_t m_threadCount;
    
    // Synchronization
    mutable std::mutex m_tasksMutex;
    mutable std::mutex m_queueMutex;
    mutable std::mutex m_delayedMutex;
    mutable std::mutex m_periodicMutex;
    std::condition_variable m_condition;
    
    // Statistics
    std::atomic<size_t> m_activeTaskCount;
    std::atomic<size_t> m_pendingTaskCount;
    std::atomic<size_t> m_completedTaskCount;
    std::atomic<size_t> m_failedTaskCount;
    std::atomic<Task::TaskID> m_nextTaskId;
    
    // Thread for delayed tasks
    std::thread m_delayedTasksThread;
    std::thread m_periodicTasksThread;
};

/**
 * @brief Representa um evento no sistema
 */
class Event {
public:
    using EventID = uint64_t;
    using EventType = std::string;
    using EventData = std::unordered_map<std::string, std::any>;
    
    Event(EventType type, EventData data = {});
    virtual ~Event() = default;
    
    EventID getId() const { return m_id; }
    EventType getType() const { return m_type; }
    EventData getData() const { return m_data; }
    std::chrono::system_clock::time_point getTimestamp() const { return m_timestamp; }
    
    template<typename T>
    T getDataValue(const std::string& key, const T& defaultValue = T()) const {
        auto it = m_data.find(key);
        if (it != m_data.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    bool hasData(const std::string& key) const {
        return m_data.find(key) != m_data.end();
    }
    
private:
    EventID m_id;
    EventType m_type;
    EventData m_data;
    std::chrono::system_clock::time_point m_timestamp;
    
    static std::atomic<EventID> s_nextEventId;
};

/**
 * @brief Sistema de barramento de eventos para comunicação entre componentes
 */
class EventBus {
public:
    using EventHandler = std::function<void(const Event&)>;
    using SubscriptionID = uint64_t;
    
    EventBus();
    ~EventBus();
    
    // Subscription management
    SubscriptionID subscribe(const Event::EventType& eventType, EventHandler handler);
    SubscriptionID subscribeToAll(EventHandler handler);
    bool unsubscribe(SubscriptionID subscriptionId);
    
    // Event publishing
    void publish(const Event& event);
    void publishAsync(const Event& event);
    
    // Event querying
    std::vector<Event> getEventHistory(const Event::EventType& eventType, 
                                      size_t maxCount = 100) const;
    std::vector<Event> getRecentEvents(size_t maxCount = 100) const;
    
    // Statistics
    size_t getSubscriberCount(const Event::EventType& eventType) const;
    size_t getTotalEventCount() const;
    std::vector<std::pair<Event::EventType, size_t>> getEventTypeDistribution() const;
    
private:
    struct Subscription {
        SubscriptionID id;
        Event::EventType eventType;
        EventHandler handler;
        bool isGlobal;
    };
    
    void dispatchEvent(const Event& event);
    SubscriptionID generateSubscriptionId();
    
    // Subscriptions
    std::unordered_map<SubscriptionID, Subscription> m_subscriptions;
    std::unordered_map<Event::EventType, std::vector<SubscriptionID>> m_typeToSubscriptions;
    std::vector<SubscriptionID> m_globalSubscriptions;
    
    // Event history
    std::vector<Event> m_eventHistory;
    size_t m_maxHistorySize;
    
    // Statistics
    std::unordered_map<Event::EventType, size_t> m_eventCounts;
    std::atomic<size_t> m_totalEventCount;
    
    // Synchronization
    mutable std::mutex m_subscriptionsMutex;
    mutable std::mutex m_historyMutex;
    
    // Task scheduler for async events
    std::shared_ptr<TaskScheduler> m_taskScheduler;
    std::atomic<SubscriptionID> m_nextSubscriptionId;
};

/**
 * @brief Gerenciador de recursos compartilhados no sistema
 */
class ResourceManager {
public:
    using ResourceID = std::string;
    
    ResourceManager();
    ~ResourceManager();
    
    // Resource registration
    template<typename T>
    bool registerResource(const ResourceID& id, std::shared_ptr<T> resource) {
        std::lock_guard<std::mutex> lock(m_resourcesMutex);
        if (m_resources.find(id) != m_resources.end()) {
            return false; // Resource already exists
        }
        m_resources[id] = std::static_pointer_cast<void>(resource);
        m_resourceTypes[id] = typeid(T).name();
        return true;
    }
    
    // Resource retrieval
    template<typename T>
    std::shared_ptr<T> getResource(const ResourceID& id) const {
        std::lock_guard<std::mutex> lock(m_resourcesMutex);
        auto it = m_resources.find(id);
        if (it == m_resources.end()) {
            return nullptr;
        }
        
        try {
            return std::static_pointer_cast<T>(it->second);
        } catch (const std::bad_cast&) {
            return nullptr;
        }
    }
    
    // Resource management
    bool unregisterResource(const ResourceID& id);
    bool hasResource(const ResourceID& id) const;
    std::string getResourceType(const ResourceID& id) const;
    std::vector<ResourceID> getAllResourceIds() const;
    std::vector<ResourceID> getResourcesByType(const std::string& typeName) const;
    
    // Resource batch operations
    template<typename T>
    std::unordered_map<ResourceID, std::shared_ptr<T>> getAllResourcesOfType() const {
        std::lock_guard<std::mutex> lock(m_resourcesMutex);
        std::unordered_map<ResourceID, std::shared_ptr<T>> result;
        std::string targetType = typeid(T).name();
        
        for (const auto& [id, resource] : m_resources) {
            if (m_resourceTypes.at(id) == targetType) {
                result[id] = std::static_pointer_cast<T>(resource);
            }
        }
        
        return result;
    }
    
private:
    mutable std::mutex m_resourcesMutex;
    std::unordered_map<ResourceID, std::shared_ptr<void>> m_resources;
    std::unordered_map<ResourceID, std::string> m_resourceTypes;
};

/**
 * @brief Núcleo do sistema multitarefa que coordena todos os componentes
 */
class MultitaskingCore {
public:
    // Padrão Singleton para acesso global
    static MultitaskingCore& getInstance();
    
    // Inicialização e finalização
    void initialize(size_t numThreads = std::thread::hardware_concurrency());
    void shutdown();
    bool isInitialized() const { return m_initialized; }
    
    // Acesso aos subsistemas
    TaskScheduler& getTaskScheduler() { return *m_taskScheduler; }
    EventBus& getEventBus() { return *m_eventBus; }
    ResourceManager& getResourceManager() { return *m_resourceManager; }
    
    // Métodos de conveniência para tarefas
    Task::TaskID runTask(const std::string& name, 
                        Task::TaskFunction function,
                        Task::Priority priority = Task::Priority::NORMAL,
                        const std::unordered_map<std::string, std::any>& params = {});
    
    // Métodos de conveniência para eventos
    void publishEvent(const std::string& eventType, 
                     const std::unordered_map<std::string, std::any>& eventData = {});
    
    // Métodos de conveniência para recursos
    template<typename T>
    bool registerSharedResource(const std::string& id, std::shared_ptr<T> resource) {
        return m_resourceManager->registerResource<T>(id, resource);
    }
    
    template<typename T>
    std::shared_ptr<T> getSharedResource(const std::string& id) {
        return m_resourceManager->getResource<T>(id);
    }
    
    // Estatísticas e monitoramento
    std::string getSystemStatus() const;
    float getSystemLoad() const;
    void logSystemMetrics();
    
private:
    MultitaskingCore();
    ~MultitaskingCore();
    
    // Prevenção de cópia e atribuição
    MultitaskingCore(const MultitaskingCore&) = delete;
    MultitaskingCore& operator=(const MultitaskingCore&) = delete;
    
    // Componentes principais
    std::unique_ptr<TaskScheduler> m_taskScheduler;
    std::unique_ptr<EventBus> m_eventBus;
    std::unique_ptr<ResourceManager> m_resourceManager;
    
    // Estado do sistema
    std::atomic<bool> m_initialized;
    
    // Logging e monitoramento
    std::thread m_monitoringThread;
    std::atomic<bool> m_monitoringActive;
    std::chrono::seconds m_monitoringInterval;
    
    void monitoringRoutine();
};

} // namespace core
} // namespace wydstudio

#endif // WYDSTUDIO_MULTITASKING_CORE_H