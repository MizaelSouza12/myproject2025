/**
 * Sistema de Filas Prioritárias com Back Pressure WYDBRASIL
 * 
 * Sistema avançado de filas com suporte a prioridades,
 * back pressure, circuit breaker e rate limiting.
 */

class PriorityQueueSystem {
    constructor(options = {}) {
        // Prioridades padrão
        this.priorities = {
            CRITICAL: 1000,
            HIGH: 100,
            MEDIUM: 50,
            LOW: 10,
            BACKGROUND: 1
        };
        
        // Configurações
        this.maxQueueSize = options.maxQueueSize || 1000;
        this.backPressureThreshold = options.backPressureThreshold || 0.8;
        this.processingRate = options.processingRate || 100; // itens por segundo
        this.categoryLimits = options.categoryLimits || {};
        this.failureThreshold = options.failureThreshold || 0.5;
        this.recoveryTimeout = options.recoveryTimeout || 30000;
        
        // Estado
        this.queues = {};
        this.stats = {
            enqueued: 0,
            processed: 0,
            rejected: 0,
            errors: 0,
            categoryStats: {}
        };
        
        // Circuit breakers por categoria
        this.circuitBreakers = {};
        
        // Rate limiters por categoria
        this.rateLimiters = {};
        
        // Status do sistema
        this.status = 'healthy'; // healthy, back_pressure, circuit_open, shutting_down
    }
    
    /**
     * Enfileira um item para processamento
     * @param {string} category Categoria do item
     * @param {any} item Item a ser processado
     * @param {string} priority Prioridade do item
     * @returns {Promise} Promessa que resolve quando o item for processado
     */
    enqueue(category, item, priority = 'MEDIUM') {
        return new Promise((resolve, reject) => {
            // Verificar se o sistema está encerrando
            if (this.status === 'shutting_down') {
                const error = new Error('System is shutting down');
                error.code = 'shutting_down';
                reject(error);
                return;
            }
            
            // Verificar circuit breaker
            if (this.isCircuitOpen(category)) {
                const error = new Error(`Circuit is open for category ${category}`);
                error.code = 'circuit_open';
                reject(error);
                return;
            }
            
            // Verificar rate limiting
            if (this.isRateLimited(category)) {
                const error = new Error(`Rate limit exceeded for category ${category}`);
                error.code = 'rate_limited';
                reject(error);
                return;
            }
            
            // Calcular prioridade numérica
            const priorityValue = this.priorities[priority] || this.priorities.MEDIUM;
            
            // Criar objeto de tarefa
            const task = {
                id: `${category}-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`,
                item,
                category,
                priority: priorityValue,
                timestamp: Date.now(),
                resolve,
                reject
            };
            
            // Verificar back pressure
            if (this.shouldApplyBackPressure()) {
                // Em back pressure, apenas itens críticos são aceitos
                if (priority !== 'CRITICAL') {
                    const error = new Error('System is under back pressure');
                    error.code = 'back_pressure';
                    reject(error);
                    return;
                }
                
                // Atualizar status do sistema
                if (this.status !== 'back_pressure') {
                    this.status = 'back_pressure';
                    console.warn(`[PriorityQueue] System entered back pressure mode. Queue size: ${this.getTotalQueueSize()}`);
                }
            } else if (this.status === 'back_pressure') {
                // Sair do modo de back pressure quando a fila estiver abaixo do limite
                this.status = 'healthy';
                console.info(`[PriorityQueue] System exited back pressure mode. Queue size: ${this.getTotalQueueSize()}`);
            }
            
            // Inicializar fila da categoria se não existir
            if (!this.queues[category]) {
                this.queues[category] = [];
                
                // Inicializar estatísticas da categoria
                if (!this.stats.categoryStats[category]) {
                    this.stats.categoryStats[category] = {
                        enqueued: 0,
                        processed: 0,
                        rejected: 0,
                        errors: 0,
                        avgWaitTime: 0,
                        avgProcessingTime: 0
                    };
                }
            }
            
            // Verificar limite específico da categoria
            const categoryLimit = this.categoryLimits[category] || this.maxQueueSize;
            if (this.queues[category].length >= categoryLimit) {
                // Em limite de categoria, rejeitar ou substituir item de menor prioridade
                if (this.shouldRejectOrReplace(category, priorityValue)) {
                    const error = new Error(`Queue limit reached for category ${category}`);
                    error.code = 'queue_limit';
                    reject(error);
                    this.stats.rejected++;
                    this.stats.categoryStats[category].rejected++;
                    return;
                }
            }
            
            // Adicionar à fila
            this.queues[category].push(task);
            
            // Ordenar por prioridade (maior primeiro)
            this.queues[category].sort((a, b) => b.priority - a.priority);
            
            // Atualizar estatísticas
            this.stats.enqueued++;
            this.stats.categoryStats[category].enqueued++;
            
            // Processar a fila imediatamente se possível
            this.processQueue(category);
        });
    }
    
    /**
     * Processa a fila de uma categoria
     * @param {string} category Categoria da fila
     */
    processQueue(category) {
        if (!this.queues[category] || this.queues[category].length === 0) {
            return;
        }
        
        // Simular processamento (em implementação real, isso processaria de fato)
        // No ambiente de produção, isso seria implementado como um worker real
        // que executaria a lógica de processamento específica da categoria
        
        // Remover o item de maior prioridade da fila
        const task = this.queues[category].shift();
        
        // Simular processamento assíncrono
        const startTime = Date.now();
        
        // Em um sistema real, o processamento seria de fato realizado aqui
        const processSuccess = Math.random() > 0.1; // 10% de chance de falha para simulação
        
        setTimeout(() => {
            if (processSuccess) {
                // Atualizar estatísticas
                this.stats.processed++;
                this.stats.categoryStats[category].processed++;
                
                const waitTime = startTime - task.timestamp;
                const processingTime = Date.now() - startTime;
                
                // Atualizar tempos médios
                const catStats = this.stats.categoryStats[category];
                catStats.avgWaitTime = (catStats.avgWaitTime * (catStats.processed - 1) + waitTime) / catStats.processed;
                catStats.avgProcessingTime = (catStats.avgProcessingTime * (catStats.processed - 1) + processingTime) / catStats.processed;
                
                // Resolver promessa
                task.resolve({
                    success: true,
                    id: task.id,
                    waitTime,
                    processingTime
                });
                
                // Atualizar rate limiter
                this.updateRateLimiter(category);
                
                // Resetar contador de falhas do circuit breaker
                this.resetCircuitBreaker(category);
            } else {
                // Simular erro no processamento
                this.stats.errors++;
                this.stats.categoryStats[category].errors++;
                
                // Incrementar contador do circuit breaker
                this.incrementCircuitBreaker(category);
                
                // Rejeitar promessa
                const error = new Error(`Processing error for task ${task.id}`);
                error.code = 'processing_error';
                task.reject(error);
            }
            
            // Continuar processando a fila
            if (this.queues[category] && this.queues[category].length > 0) {
                this.processQueue(category);
            }
        }, 50); // Tempo simulado de processamento (50ms)
    }
    
    /**
     * Verifica se deve aplicar back pressure
     * @returns {boolean} Verdadeiro se deve aplicar back pressure
     */
    shouldApplyBackPressure() {
        const totalSize = this.getTotalQueueSize();
        return totalSize > this.maxQueueSize * this.backPressureThreshold;
    }
    
    /**
     * Verifica se deve rejeitar ou substituir um item
     * @param {string} category Categoria do item
     * @param {number} priority Prioridade do item
     * @returns {boolean} Verdadeiro se deve rejeitar
     */
    shouldRejectOrReplace(category, priority) {
        // Procurar pelo item com menor prioridade na fila
        const lowestPriorityItem = this.getLowestPriorityItem(category);
        
        // Se o novo item tiver prioridade maior que o item com menor prioridade,
        // substituir o item de menor prioridade
        if (lowestPriorityItem && priority > lowestPriorityItem.priority) {
            // Remover o item de menor prioridade
            this.queues[category] = this.queues[category].filter(t => t.id !== lowestPriorityItem.id);
            
            // Rejeitar a promessa do item removido
            const error = new Error('Item was replaced by higher priority item');
            error.code = 'replaced';
            lowestPriorityItem.reject(error);
            
            return false; // Não rejeitar o novo item
        }
        
        return true; // Rejeitar o novo item
    }
    
    /**
     * Obtém o item com menor prioridade na fila
     * @param {string} category Categoria da fila
     * @returns {object} Item com menor prioridade
     */
    getLowestPriorityItem(category) {
        if (!this.queues[category] || this.queues[category].length === 0) {
            return null;
        }
        
        return this.queues[category].reduce((lowest, current) => {
            return current.priority < lowest.priority ? current : lowest;
        }, this.queues[category][0]);
    }
    
    /**
     * Obtém o tamanho total de todas as filas
     * @returns {number} Tamanho total das filas
     */
    getTotalQueueSize() {
        return Object.values(this.queues).reduce((sum, queue) => sum + queue.length, 0);
    }
    
    /**
     * Verifica se o circuit breaker está aberto para uma categoria
     * @param {string} category Categoria a verificar
     * @returns {boolean} Verdadeiro se o circuit breaker estiver aberto
     */
    isCircuitOpen(category) {
        if (!this.circuitBreakers[category]) {
            this.circuitBreakers[category] = {
                status: 'closed',
                failureCount: 0,
                lastFailure: 0,
                totalRequests: 0,
                successfulRequests: 0
            };
        }
        
        const breaker = this.circuitBreakers[category];
        
        // Verificar se o circuit breaker está aberto
        if (breaker.status === 'open') {
            // Verificar se já passou o tempo de recuperação
            const now = Date.now();
            if (now - breaker.lastFailure > this.recoveryTimeout) {
                // Transição para half-open
                breaker.status = 'half-open';
                console.info(`[PriorityQueue] Circuit breaker for ${category} changed from open to half-open`);
            } else {
                return true; // Circuit breaker ainda aberto
            }
        }
        
        return false;
    }
    
    /**
     * Incrementa o contador de falhas do circuit breaker
     * @param {string} category Categoria a incrementar
     */
    incrementCircuitBreaker(category) {
        if (!this.circuitBreakers[category]) {
            this.circuitBreakers[category] = {
                status: 'closed',
                failureCount: 0,
                lastFailure: 0,
                totalRequests: 0,
                successfulRequests: 0
            };
        }
        
        const breaker = this.circuitBreakers[category];
        
        // Incrementar contadores
        breaker.failureCount++;
        breaker.totalRequests++;
        breaker.lastFailure = Date.now();
        
        // Calcular taxa de falha
        const failureRate = breaker.failureCount / breaker.totalRequests;
        
        // Verificar se deve abrir o circuit breaker
        if (breaker.status === 'closed' && failureRate >= this.failureThreshold) {
            breaker.status = 'open';
            console.warn(`[PriorityQueue] Circuit breaker for ${category} opened. Failure rate: ${failureRate.toFixed(2)}`);
        } else if (breaker.status === 'half-open') {
            // Se falhar em half-open, voltar para open
            breaker.status = 'open';
            console.warn(`[PriorityQueue] Circuit breaker for ${category} reopened after failure in half-open state`);
        }
    }
    
    /**
     * Reseta o contador de falhas do circuit breaker
     * @param {string} category Categoria a resetar
     */
    resetCircuitBreaker(category) {
        if (!this.circuitBreakers[category]) {
            return;
        }
        
        const breaker = this.circuitBreakers[category];
        
        // Incrementar contador de sucesso
        breaker.totalRequests++;
        breaker.successfulRequests++;
        
        // Se estiver em half-open e tiver sucesso, fechar o circuit breaker
        if (breaker.status === 'half-open') {
            breaker.status = 'closed';
            breaker.failureCount = 0;
            console.info(`[PriorityQueue] Circuit breaker for ${category} closed after successful request in half-open state`);
        }
        
        // Periodicamente resetar contadores para evitar que erros antigos afetem muito o cálculo
        const now = Date.now();
        if (now - breaker.lastFailure > this.recoveryTimeout * 2) {
            breaker.failureCount = 0;
            breaker.totalRequests = 0;
            breaker.successfulRequests = 0;
        }
    }
    
    /**
     * Verifica se a categoria atingiu o limite de rate
     * @param {string} category Categoria a verificar
     * @returns {boolean} Verdadeiro se atingiu o limite
     */
    isRateLimited(category) {
        if (!this.rateLimiters[category]) {
            this.rateLimiters[category] = {
                tokens: 100,
                lastRefill: Date.now(),
                capacity: 100,
                refillRate: 10 // tokens por segundo
            };
        }
        
        const limiter = this.rateLimiters[category];
        
        // Refill tokens based on time elapsed
        const now = Date.now();
        const timeDelta = now - limiter.lastRefill;
        const tokensToAdd = (timeDelta / 1000) * limiter.refillRate;
        
        limiter.tokens = Math.min(limiter.capacity, limiter.tokens + tokensToAdd);
        limiter.lastRefill = now;
        
        // Check if we have enough tokens
        if (limiter.tokens < 1) {
            return true; // Rate limited
        }
        
        // Consume a token
        limiter.tokens -= 1;
        return false;
    }
    
    /**
     * Atualiza o rate limiter após processamento bem-sucedido
     * @param {string} category Categoria a atualizar
     */
    updateRateLimiter(category) {
        if (!this.rateLimiters[category]) {
            return;
        }
        
        // Poderia ajustar o rate limiter baseado em sucesso/falha
        // Mas isso é apenas um exemplo simples
    }
    
    /**
     * Obtém o status do sistema de filas
     * @returns {object} Status do sistema
     */
    getStatus() {
        return {
            status: this.status,
            queues: Object.keys(this.queues).map(category => ({
                category,
                size: this.queues[category].length,
                circuitStatus: this.circuitBreakers[category]?.status || 'closed'
            })),
            totalEnqueued: this.stats.enqueued,
            totalProcessed: this.stats.processed,
            totalRejected: this.stats.rejected,
            totalErrors: this.stats.errors,
            totalQueueSize: this.getTotalQueueSize()
        };
    }
    
    /**
     * Obtém estatísticas detalhadas do sistema
     * @returns {object} Estatísticas detalhadas
     */
    getDetailedStats() {
        return {
            overall: {
                enqueued: this.stats.enqueued,
                processed: this.stats.processed,
                rejected: this.stats.rejected,
                errors: this.stats.errors,
                successRate: this.stats.processed / Math.max(1, this.stats.processed + this.stats.errors)
            },
            categories: Object.entries(this.stats.categoryStats).map(([category, stats]) => ({
                category,
                stats,
                queueSize: this.queues[category]?.length || 0,
                circuitStatus: this.circuitBreakers[category]?.status || 'closed',
                highestPriority: this.queues[category]?.length > 0 ? this.queues[category][0].priority : 0
            })),
            system: {
                status: this.status,
                maxQueueSize: this.maxQueueSize,
                backPressureThreshold: this.backPressureThreshold,
                processingRate: this.processingRate,
                uptime: process.uptime()
            }
        };
    }
    
    /**
     * Inicia o desligamento controlado do sistema
     * @returns {Promise} Promessa que resolve quando o sistema for desligado
     */
    shutdown() {
        return new Promise((resolve, reject) => {
            // Marcar sistema como encerrando
            this.status = 'shutting_down';
            console.info('[PriorityQueue] System is shutting down...');
            
            // Rejeitar todas as tarefas na fila
            Object.entries(this.queues).forEach(([category, queue]) => {
                queue.forEach(task => {
                    const error = new Error('System is shutting down');
                    error.code = 'shutting_down';
                    task.reject(error);
                    this.stats.rejected++;
                });
                
                // Limpar a fila
                this.queues[category] = [];
            });
            
            // Resolver quando todas as filas estiverem vazias
            resolve({
                status: 'shutdown_complete',
                stats: this.getDetailedStats()
            });
        });
    }
}

module.exports = PriorityQueueSystem;
