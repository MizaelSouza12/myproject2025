const EventEmitter = require('events');
const logger = require('../utils/logger');

class CombatSystem {
    constructor() {
        this.events = new EventEmitter();
        this.combats = new Map();
        this.initialized = false;
    }

    async initialize() {
        try {
            this.initialized = true;
            logger.info('Sistema de combate inicializado com sucesso');
        } catch (error) {
            logger.error('Erro ao inicializar sistema de combate:', error);
            throw error;
        }
    }

    async startCombat(attackerId, targetId) {
        try {
            const combatId = `${attackerId}-${targetId}-${Date.now()}`;
            
            const combat = {
                id: combatId,
                attackerId,
                targetId,
                startTime: Date.now(),
                lastHitTime: Date.now(),
                hits: [],
                status: 'active'
            };

            this.combats.set(combatId, combat);
            this.events.emit('combat:started', combat);
            
            return combat;
        } catch (error) {
            logger.error('Erro ao iniciar combate:', error);
            throw error;
        }
    }

    async processHit(combatId, attackerStats, targetStats) {
        try {
            const combat = this.combats.get(combatId);
            if (!combat || combat.status !== 'active') {
                throw new Error('Combate não encontrado ou inativo');
            }

            // Cálculo de dano baseado nas stats
            const damage = this.calculateDamage(attackerStats, targetStats);
            
            const hit = {
                timestamp: Date.now(),
                damage,
                attackerId: combat.attackerId,
                targetId: combat.targetId
            };

            combat.hits.push(hit);
            combat.lastHitTime = Date.now();

            this.events.emit('combat:hit', { combatId, hit });
            
            return hit;
        } catch (error) {
            logger.error('Erro ao processar hit:', error);
            throw error;
        }
    }

    calculateDamage(attackerStats, targetStats) {
        // Fórmula básica de dano
        const baseDamage = attackerStats.strength * 2;
        const defense = targetStats.constitution;
        const damage = Math.max(1, baseDamage - defense);
        
        // Chance de crítico
        const criticalChance = attackerStats.dexterity / 100;
        if (Math.random() < criticalChance) {
            return damage * 2;
        }
        
        return damage;
    }

    async endCombat(combatId, reason = 'finished') {
        try {
            const combat = this.combats.get(combatId);
            if (!combat) {
                throw new Error('Combate não encontrado');
            }

            combat.status = 'finished';
            combat.endTime = Date.now();
            combat.endReason = reason;

            this.events.emit('combat:ended', combat);
            
            // Limpar combate após um tempo
            setTimeout(() => {
                this.combats.delete(combatId);
            }, 60000);

            return combat;
        } catch (error) {
            logger.error('Erro ao finalizar combate:', error);
            throw error;
        }
    }

    async getCombatStatus(combatId) {
        try {
            const combat = this.combats.get(combatId);
            if (!combat) {
                throw new Error('Combate não encontrado');
            }
            return {
                ...combat,
                duration: Date.now() - combat.startTime,
                totalDamage: combat.hits.reduce((sum, hit) => sum + hit.damage, 0),
                hitCount: combat.hits.length
            };
        } catch (error) {
            logger.error('Erro ao obter status do combate:', error);
            throw error;
        }
    }

    async shutdown() {
        try {
            // Finalizar todos os combates ativos
            for (const [combatId, combat] of this.combats) {
                if (combat.status === 'active') {
                    await this.endCombat(combatId, 'server_shutdown');
                }
            }
            
            this.combats.clear();
            this.initialized = false;
            logger.info('Sistema de combate desligado com sucesso');
        } catch (error) {
            logger.error('Erro ao desligar sistema de combate:', error);
            throw error;
        }
    }
}

module.exports = CombatSystem; 