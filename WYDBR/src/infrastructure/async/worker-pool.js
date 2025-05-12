/**
 * Sistema de Worker Pool WYDBRASIL
 * 
 * Pool de workers para processamento assíncrono de tarefas
 * com balanceamento de carga e recuperação automática.
 */

class WorkerPool {
    constructor(options = {}) {
        this.workers = [];
        this.queue = [];
        this.running = [];
        this.size = options.size || 4;
        this.timeout = options.timeout || 30000;
        this.maxRetries = options.maxRetries || 3;
        
        this.initialize();
    }
    
    initialize() {
        // Criar workers
        for (let i = 0; i < this.size; i++) {
            this.createWorker(i);
        }
    }
    
    createWorker(id) {
        const worker = {
            id: `worker-${id}`,
            busy: false,
            task: null,
            failCount: 0,
            processingTime: 0,
            taskCount: 0
        };
        
        this.workers.push(worker);
        return worker;
    }
    
    submitTask(task, priority = 1) {
        return new Promise((resolve, reject) => {
            const queuedTask = {
                id: `task-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`,
                task,
                priority,
                resolve,
                reject,
                submitted: Date.now(),
                retries: 0
            };
            
            this.queue.push(queuedTask);
            
            // Ordenar fila por prioridade (maior primeiro)
            this.queue.sort((a, b) => b.priority - a.priority);
            
            // Processar a fila
            this.processQueue();
        });
    }
    
    processQueue() {
        // Verificar se há tarefas na fila
        if (this.queue.length === 0) {
            return;
        }
        
        // Verificar se há workers disponíveis
        const availableWorker = this.workers.find(w => !w.busy);
        
        if (!availableWorker) {
            return;
        }
        
        // Pegar a próxima tarefa da fila
        const nextTask = this.queue.shift();
        
        // Marcar worker como ocupado
        availableWorker.busy = true;
        availableWorker.task = nextTask;
        availableWorker.startTime = Date.now();
        
        // Adicionar à lista de tarefas em execução
        this.running.push({
            taskId: nextTask.id,
            workerId: availableWorker.id,
            startTime: availableWorker.startTime,
            timeout: setTimeout(() => {
                this.handleTaskTimeout(availableWorker, nextTask);
            }, this.timeout)
        });
        
        // Executar a tarefa
        Promise.resolve()
            .then(() => nextTask.task())
            .then(result => {
                this.completeTask(availableWorker, nextTask, result);
            })
            .catch(error => {
                this.handleTaskError(availableWorker, nextTask, error);
            });
    }
    
    completeTask(worker, task, result) {
        // Limpar timeout
        const runningTask = this.running.find(t => t.taskId === task.id);
        if (runningTask) {
            clearTimeout(runningTask.timeout);
        }
        
        // Remover da lista de tarefas em execução
        this.running = this.running.filter(t => t.taskId !== task.id);
        
        // Atualizar estatísticas do worker
        worker.busy = false;
        worker.processingTime += (Date.now() - worker.startTime);
        worker.taskCount++;
        
        // Resolver a promessa da tarefa
        task.resolve(result);
        
        // Continuar processando a fila
        this.processQueue();
    }
    
    handleTaskError(worker, task, error) {
        console.error(`[WorkerPool] Error in task ${task.id}: ${error.message}`);
        
        // Limpar timeout
        const runningTask = this.running.find(t => t.taskId === task.id);
        if (runningTask) {
            clearTimeout(runningTask.timeout);
        }
        
        // Remover da lista de tarefas em execução
        this.running = this.running.filter(t => t.taskId !== task.id);
        
        // Incrementar contador de falhas do worker
        worker.failCount++;
        
        // Verificar se deve tentar novamente
        if (task.retries < this.maxRetries) {
            console.warn(`[WorkerPool] Retrying task ${task.id} (attempt ${task.retries + 1})`);
            
            // Incrementar contador de tentativas
            task.retries++;
            
            // Colocar a tarefa novamente na fila
            this.queue.unshift(task);
        } else {
            // Limite de tentativas atingido
            console.error(`[WorkerPool] Max retries reached for task ${task.id}`);
            
            // Rejeitar a promessa da tarefa
            task.reject(error);
        }
        
        // Liberar o worker
        worker.busy = false;
        
        // Reiniciar o worker se tiver muitas falhas
        if (worker.failCount >= 5) {
            console.warn(`[WorkerPool] Worker ${worker.id} has too many failures, restarting`);
            this.restartWorker(worker);
        }
        
        // Continuar processando a fila
        this.processQueue();
    }
    
    handleTaskTimeout(worker, task) {
        console.error(`[WorkerPool] Task ${task.id} timed out after ${this.timeout}ms`);
        
        // Incrementar contador de falhas do worker
        worker.failCount++;
        
        // Remover da lista de tarefas em execução
        this.running = this.running.filter(t => t.taskId !== task.id);
        
        // Verificar se deve tentar novamente
        if (task.retries < this.maxRetries) {
            console.warn(`[WorkerPool] Retrying task ${task.id} after timeout (attempt ${task.retries + 1})`);
            
            // Incrementar contador de tentativas
            task.retries++;
            
            // Colocar a tarefa novamente na fila
            this.queue.unshift(task);
        } else {
            // Limite de tentativas atingido
            console.error(`[WorkerPool] Max retries reached for task ${task.id} after timeout`);
            
            // Rejeitar a promessa da tarefa
            task.reject(new Error(`Task timed out after ${this.timeout}ms and ${task.retries} retries`));
        }
        
        // Liberar o worker
        worker.busy = false;
        
        // Reiniciar o worker se tiver muitas falhas
        if (worker.failCount >= 5) {
            console.warn(`[WorkerPool] Worker ${worker.id} has too many timeouts, restarting`);
            this.restartWorker(worker);
        }
        
        // Continuar processando a fila
        this.processQueue();
    }
    
    restartWorker(worker) {
        // Remover worker da lista
        const index = this.workers.findIndex(w => w.id === worker.id);
        
        if (index !== -1) {
            this.workers.splice(index, 1);
        }
        
        // Criar novo worker com o mesmo ID
        const id = parseInt(worker.id.split('-')[1], 10);
        this.createWorker(id);
    }
    
    // Limpar o pool e todos os workers
    shutdown() {
        // Limpar todos os timeouts
        this.running.forEach(task => {
            clearTimeout(task.timeout);
        });
        
        // Rejeitar todas as tarefas na fila
        this.queue.forEach(task => {
            task.reject(new Error('Worker pool is shutting down'));
        });
        
        // Limpar arrays
        this.workers = [];
        this.queue = [];
        this.running = [];
    }
    
    // Obter estatísticas do pool
    getStats() {
        return {
            workers: this.workers.length,
            active: this.workers.filter(w => w.busy).length,
            queueSize: this.queue.length,
            runningTasks: this.running.length,
            totalProcessed: this.workers.reduce((sum, w) => sum + w.taskCount, 0),
            avgProcessingTime: this.getTotalProcessingTime() / Math.max(1, this.getTotalTaskCount()),
            failRate: this.getTotalFailCount() / Math.max(1, this.getTotalTaskCount() + this.getTotalFailCount())
        };
    }
    
    getTotalProcessingTime() {
        return this.workers.reduce((sum, w) => sum + w.processingTime, 0);
    }
    
    getTotalTaskCount() {
        return this.workers.reduce((sum, w) => sum + w.taskCount, 0);
    }
    
    getTotalFailCount() {
        return this.workers.reduce((sum, w) => sum + w.failCount, 0);
    }
}

module.exports = WorkerPool;
