/**
 * WYDBR 2.0 - Implementação Gerada Automaticamente
 * Arquivo origem: sistema_cache_hierarquico.txt
 * 
 * Este arquivo contém a definição C++ do sistema, encapsulada em JavaScript
 * para integração no projeto WYDBR 2.0.
 */

const EventEmitter = require('events');
const logger = require('../utils/logger');

class CacheSystem {
    constructor() {
        this.events = new EventEmitter();
        this.initialized = false;
        this.caches = {
            l1: new Map(), // Cache em memória (mais rápido)
            l2: new Map(), // Cache secundário (maior capacidade)
            l3: new Map()  // Cache persistente (mais lento, mas permanente)
        };
        this.stats = {
            hits: { l1: 0, l2: 0, l3: 0 },
            misses: { l1: 0, l2: 0, l3: 0 }
        };
    }

    async initialize() {
        try {
            // Configurar limites de cache
            this.limits = {
                l1: 1000,  // 1000 itens
                l2: 10000, // 10000 itens
                l3: 100000 // 100000 itens
            };

            // Configurar TTL (Time To Live) em milissegundos
            this.ttl = {
                l1: 60000,    // 1 minuto
                l2: 300000,   // 5 minutos
                l3: 3600000   // 1 hora
            };

            // Iniciar limpeza periódica
            this.startCleanupInterval();

            this.initialized = true;
            logger.info('Sistema de cache inicializado com sucesso');
        } catch (error) {
            logger.error('Erro ao inicializar sistema de cache:', error);
            throw error;
        }
    }

    startCleanupInterval() {
        // Limpar caches expirados a cada minuto
        setInterval(() => {
            this.cleanup();
        }, 60000);
    }

    cleanup() {
        const now = Date.now();

        for (const level of ['l1', 'l2', 'l3']) {
            const cache = this.caches[level];
            for (const [key, value] of cache) {
                if (now - value.timestamp > this.ttl[level]) {
                    cache.delete(key);
                    this.events.emit('cache:expired', { level, key });
                }
            }
        }
    }

    async get(key, level = 'l1') {
        try {
            // Verificar L1
            if (level === 'l1') {
                const l1Result = this.caches.l1.get(key);
                if (l1Result && Date.now() - l1Result.timestamp <= this.ttl.l1) {
                    this.stats.hits.l1++;
                    return l1Result.value;
                }
                this.stats.misses.l1++;
            }

            // Verificar L2
            if (level === 'l1' || level === 'l2') {
                const l2Result = this.caches.l2.get(key);
                if (l2Result && Date.now() - l2Result.timestamp <= this.ttl.l2) {
                    this.stats.hits.l2++;
                    // Promover para L1
                    if (level === 'l1') {
                        this.set(key, l2Result.value, 'l1');
                    }
                    return l2Result.value;
                }
                this.stats.misses.l2++;
            }

            // Verificar L3
            const l3Result = this.caches.l3.get(key);
            if (l3Result && Date.now() - l3Result.timestamp <= this.ttl.l3) {
                this.stats.hits.l3++;
                // Promover para nível superior
                if (level === 'l1' || level === 'l2') {
                    this.set(key, l3Result.value, level);
                }
                return l3Result.value;
            }
            this.stats.misses.l3++;

            return null;
        } catch (error) {
            logger.error(`Erro ao recuperar cache para chave ${key}:`, error);
            throw error;
        }
    }

    async set(key, value, level = 'l1') {
        try {
            const cache = this.caches[level];
            const limit = this.limits[level];

            // Se o cache está cheio, remover item mais antigo
            if (cache.size >= limit) {
                const oldestKey = Array.from(cache.entries())
                    .sort(([,a], [,b]) => a.timestamp - b.timestamp)[0][0];
                cache.delete(oldestKey);
                this.events.emit('cache:evicted', { level, key: oldestKey });
            }

            const entry = {
                value,
                timestamp: Date.now()
            };

            cache.set(key, entry);
            this.events.emit('cache:set', { level, key });

            return true;
        } catch (error) {
            logger.error(`Erro ao definir cache para chave ${key}:`, error);
            throw error;
        }
    }

    async delete(key, level = 'l1') {
        try {
            const cache = this.caches[level];
            const result = cache.delete(key);
            if (result) {
                this.events.emit('cache:deleted', { level, key });
            }
            return result;
        } catch (error) {
            logger.error(`Erro ao deletar cache para chave ${key}:`, error);
            throw error;
        }
    }

    async clear(level) {
        try {
            if (level) {
                this.caches[level].clear();
                this.events.emit('cache:cleared', { level });
            } else {
                for (const l of ['l1', 'l2', 'l3']) {
                    this.caches[l].clear();
                }
                this.events.emit('cache:cleared', { level: 'all' });
            }
        } catch (error) {
            logger.error('Erro ao limpar cache:', error);
            throw error;
        }
    }

    getStats() {
        return {
            ...this.stats,
            size: {
                l1: this.caches.l1.size,
                l2: this.caches.l2.size,
                l3: this.caches.l3.size
            }
        };
    }

    async shutdown() {
        try {
            // Persistir cache L3 se necessário
            // Limpar todos os caches
            await this.clear();
            
            this.initialized = false;
            logger.info('Sistema de cache desligado com sucesso');
        } catch (error) {
            logger.error('Erro ao desligar sistema de cache:', error);
            throw error;
        }
    }
}

module.exports = CacheSystem;
