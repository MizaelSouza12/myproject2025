/**
 * Gerenciador de Eventos de Mundo do WYDBRASIL
 * 
 * Responsável por criar, agendar, executar e gerenciar
 * eventos dinâmicos que ocorrem no mundo do jogo.
 */

const EventEmitter = require('events');

class WorldEventManager extends EventEmitter {
    constructor() {
        super();
        this.activeEvents = new Map();
        this.scheduledEvents = new Map();
        this.eventHistory = [];
        this.nextEventId = 1;
        this.isRunning = false;
        
        // Registrar tipos de eventos suportados
        this.registeredEventTypes = {
            'dragon_invasion': {
                name: 'Invasão de Dragões',
                handler: this._handleDragonInvasion.bind(this)
            },
            'resource_surge': {
                name: 'Surgimento de Recursos',
                handler: this._handleResourceSurge.bind(this)
            },
            'boss_raid': {
                name: 'Incursão de Chefe',
                handler: this._handleBossRaid.bind(this)
            },
            'war_declaration': {
                name: 'Declaração de Guerra',
                handler: this._handleWarDeclaration.bind(this)
            },
            'natural_disaster': {
                name: 'Desastre Natural',
                handler: this._handleNaturalDisaster.bind(this)
            }
        };
        
        console.log('[WorldEventManager] Inicializado');
    }
    
    /**
     * Inicia o gerenciador de eventos
     */
    start() {
        if (this.isRunning) return;
        
        this.isRunning = true;
        this._startScheduler();
        console.log('[WorldEventManager] Sistema de eventos de mundo iniciado');
    }
    
    /**
     * Para o gerenciador de eventos
     */
    stop() {
        this.isRunning = false;
        if (this.schedulerInterval) {
            clearInterval(this.schedulerInterval);
            this.schedulerInterval = null;
        }
        console.log('[WorldEventManager] Sistema de eventos de mundo parado');
    }
    
    /**
     * Agenda um novo evento para acontecer no mundo
     * @param {string} eventType Tipo de evento
     * @param {object} config Configuração do evento
     * @param {Date} scheduledTime Horário agendado (opcional, se null acontece imediatamente)
     * @returns {string} ID do evento agendado
     */
    scheduleEvent(eventType, config, scheduledTime = null) {
        // Validar tipo de evento
        if (!this.registeredEventTypes[eventType]) {
            console.error(`[WorldEventManager] Tipo de evento desconhecido: ${eventType}`);
            return null;
        }
        
        const eventId = `event-${Date.now()}-${this.nextEventId++}`;
        
        // Se não tiver horário agendado, iniciar imediatamente
        if (!scheduledTime) {
            this._startEvent(eventId, eventType, config);
            return eventId;
        }
        
        // Agendar o evento
        const scheduledEvent = {
            id: eventId,
            type: eventType,
            config: config,
            scheduledTime: scheduledTime
        };
        
        this.scheduledEvents.set(eventId, scheduledEvent);
        console.log(`[WorldEventManager] Evento ${eventType} agendado para ${scheduledTime.toLocaleString()}`);
        
        // Se o evento estiver agendado para logo, programar seu início
        const timeUntilEvent = scheduledTime.getTime() - Date.now();
        if (timeUntilEvent <= 30 * 60 * 1000) { // Menos de 30 minutos
            setTimeout(() => {
                this._startEvent(eventId, eventType, config);
            }, timeUntilEvent);
        }
        
        return eventId;
    }
    
    /**
     * Cancela um evento agendado
     * @param {string} eventId ID do evento
     * @returns {boolean} Sucesso da operação
     */
    cancelEvent(eventId) {
        // Tentar cancelar um evento agendado
        if (this.scheduledEvents.has(eventId)) {
            const event = this.scheduledEvents.get(eventId);
            this.scheduledEvents.delete(eventId);
            console.log(`[WorldEventManager] Evento agendado ${event.type} (${eventId}) cancelado`);
            this.emit('event_cancelled', { id: eventId, type: event.type });
            return true;
        }
        
        // Tentar cancelar um evento ativo
        if (this.activeEvents.has(eventId)) {
            const event = this.activeEvents.get(eventId);
            
            // Adicionar ao histórico
            this.eventHistory.push({
                ...event,
                endTime: new Date(),
                status: 'cancelled'
            });
            
            this.activeEvents.delete(eventId);
            console.log(`[WorldEventManager] Evento ativo ${event.type} (${eventId}) cancelado`);
            this.emit('event_cancelled', { id: eventId, type: event.type });
            return true;
        }
        
        console.error(`[WorldEventManager] Evento ${eventId} não encontrado para cancelar`);
        return false;
    }
    
    /**
     * Obtém todos os eventos ativos no momento
     * @returns {Array} Lista de eventos ativos
     */
    getActiveEvents() {
        return Array.from(this.activeEvents.values());
    }
    
    /**
     * Obtém todos os eventos agendados
     * @returns {Array} Lista de eventos agendados
     */
    getScheduledEvents() {
        return Array.from(this.scheduledEvents.values());
    }
    
    /**
     * Obtém o histórico de eventos
     * @param {number} limit Limite de eventos para retornar
     * @returns {Array} Histórico de eventos
     */
    getEventHistory(limit = 100) {
        return this.eventHistory.slice(-limit);
    }
    
    /**
     * Inicia o verificador periódico de eventos agendados
     * @private
     */
    _startScheduler() {
        // Verificar eventos agendados a cada minuto
        this.schedulerInterval = setInterval(() => {
            const now = new Date();
            
            // Verificar eventos que devem iniciar
            for (const [eventId, event] of this.scheduledEvents.entries()) {
                if (event.scheduledTime <= now) {
                    this.scheduledEvents.delete(eventId);
                    this._startEvent(eventId, event.type, event.config);
                }
            }
        }, 60 * 1000);
    }
    
    /**
     * Inicia um evento no mundo
     * @param {string} eventId ID do evento
     * @param {string} eventType Tipo do evento
     * @param {object} config Configuração do evento
     * @private
     */
    _startEvent(eventId, eventType, config) {
        // Verificar se o evento já foi iniciado ou não existe
        if (this.activeEvents.has(eventId) || !this.registeredEventTypes[eventType]) {
            return false;
        }
        
        const eventInfo = {
            id: eventId,
            type: eventType,
            name: config.name || this.registeredEventTypes[eventType].name,
            config: config,
            startTime: new Date(),
            progress: 0,
            status: 'active'
        };
        
        this.activeEvents.set(eventId, eventInfo);
        
        // Chamar o handler específico do tipo de evento
        this.registeredEventTypes[eventType].handler(eventId, config);
        
        console.log(`[WorldEventManager] Evento ${eventType} (${eventId}) iniciado`);
        this.emit('event_started', { id: eventId, type: eventType, name: eventInfo.name });
        
        return true;
    }
    
    /**
     * Finaliza um evento ativo
     * @param {string} eventId ID do evento
     * @param {string} status Status final (completed, failed, cancelled)
     * @param {object} results Resultados do evento
     * @private
     */
    _finishEvent(eventId, status = 'completed', results = {}) {
        if (!this.activeEvents.has(eventId)) {
            return false;
        }
        
        const event = this.activeEvents.get(eventId);
        
        // Adicionar ao histórico
        this.eventHistory.push({
            ...event,
            endTime: new Date(),
            duration: (new Date() - event.startTime) / 1000, // duração em segundos
            status: status,
            results: results
        });
        
        this.activeEvents.delete(eventId);
        
        console.log(`[WorldEventManager] Evento ${event.type} (${eventId}) finalizado com status: ${status}`);
        this.emit('event_finished', {
            id: eventId,
            type: event.type,
            name: event.name,
            status: status,
            results: results
        });
        
        return true;
    }
    
    /**
     * Atualiza o progresso de um evento ativo
     * @param {string} eventId ID do evento
     * @param {number} progress Progresso (0-100)
     * @param {object} data Dados adicionais sobre o progresso
     * @private
     */
    _updateEventProgress(eventId, progress, data = {}) {
        if (!this.activeEvents.has(eventId)) {
            return false;
        }
        
        const event = this.activeEvents.get(eventId);
        event.progress = progress;
        event.lastUpdate = new Date();
        
        if (data) {
            event.data = { ...event.data, ...data };
        }
        
        this.emit('event_progress', {
            id: eventId,
            type: event.type,
            name: event.name,
            progress: progress,
            data: data
        });
        
        return true;
    }
    
    // Handlers específicos para cada tipo de evento
    
    /**
     * Handler para evento de invasão de dragões
     * @param {string} eventId ID do evento
     * @param {object} config Configuração do evento
     * @private
     */
    _handleDragonInvasion(eventId, config) {
        console.log(`[WorldEventManager] Iniciando invasão de dragões: ${config.name}`);
        this.emit('dragon_invasion_started', { id: eventId, ...config });
        
        // Implementar lógica das ondas de invasão
        let currentWave = 0;
        const waves = config.waves || [];
        
        const progressEvent = () => {
            if (!this.activeEvents.has(eventId)) return; // Evento foi cancelado
            
            if (currentWave < waves.length) {
                const wave = waves[currentWave];
                const progress = (currentWave / waves.length) * 100;
                
                console.log(`[WorldEventManager] Onda ${currentWave + 1} de invasão de dragões: ${wave.name}`);
                this._updateEventProgress(eventId, progress, { currentWave: currentWave + 1, waveName: wave.name });
                
                this.emit('dragon_invasion_wave', {
                    id: eventId,
                    wave: currentWave + 1,
                    name: wave.name,
                    monsters: wave.monsters
                });
                
                currentWave++;
                
                // Programar próxima onda ou finalização
                if (currentWave < waves.length) {
                    const nextWaveDelay = (waves[currentWave].triggerAt - (wave.triggerAt || 0)) * 60 * 1000;
                    setTimeout(progressEvent, nextWaveDelay);
                } else {
                    // Última onda, finalizar após a duração configurada
                    const finalDuration = (config.duration || 30) * 60 * 1000 - (wave.triggerAt || 0) * 60 * 1000;
                    setTimeout(() => {
                        this._finishEvent(eventId, 'completed', {
                            totalWaves: waves.length,
                            participantsCount: Math.floor(Math.random() * 50) + 10 // Simulação
                        });
                    }, finalDuration);
                }
            }
        };
        
        // Iniciar primeira onda
        progressEvent();
    }
    
    /**
     * Handler para evento de surgimento de recursos
     * @param {string} eventId ID do evento
     * @param {object} config Configuração do evento
     * @private
     */
    _handleResourceSurge(eventId, config) {
        console.log(`[WorldEventManager] Iniciando surgimento de recursos: ${config.name}`);
        
        // Implementar lógica de surgimento e esgotamento de recursos
        const duration = (config.duration || 60) * 60 * 1000;
        
        // Simular progresso durante o evento
        const interval = setInterval(() => {
            if (!this.activeEvents.has(eventId)) {
                clearInterval(interval);
                return;
            }
            
            const event = this.activeEvents.get(eventId);
            const elapsedTime = Date.now() - event.startTime;
            const progress = Math.min(100, (elapsedTime / duration) * 100);
            
            this._updateEventProgress(eventId, progress);
            
            if (progress >= 100) {
                clearInterval(interval);
                this._finishEvent(eventId, 'completed', {
                    resourcesGathered: Math.floor(Math.random() * 10000) + 5000
                });
            }
        }, 60 * 1000);
    }
    
    /**
     * Handler para evento de incursão de chefe
     * @param {string} eventId ID do evento
     * @param {object} config Configuração do evento
     * @private
     */
    _handleBossRaid(eventId, config) {
        console.log(`[WorldEventManager] Iniciando incursão de chefe: ${config.name}`);
        
        // Duração máxima do evento
        const duration = (config.duration || 120) * 60 * 1000;
        
        // Simular progresso da luta contra o chefe
        const interval = setInterval(() => {
            if (!this.activeEvents.has(eventId)) {
                clearInterval(interval);
                return;
            }
            
            const event = this.activeEvents.get(eventId);
            const elapsedTime = Date.now() - event.startTime;
            
            // Simular HP do chefe diminuindo
            const bossHpPercentage = Math.max(0, 100 - (elapsedTime / duration) * 100);
            const progress = 100 - bossHpPercentage;
            
            this._updateEventProgress(eventId, progress, { bossHpPercentage });
            
            // Verificar se o chefe foi derrotado
            if (bossHpPercentage <= 0) {
                clearInterval(interval);
                this._finishEvent(eventId, 'completed', {
                    bossDefeated: true,
                    timeToDefeat: elapsedTime / 1000 // em segundos
                });
            }
        }, 5 * 1000);
        
        // Finalizar o evento após a duração máxima se o chefe não for derrotado
        setTimeout(() => {
            if (this.activeEvents.has(eventId)) {
                clearInterval(interval);
                this._finishEvent(eventId, 'failed', {
                    bossDefeated: false,
                    reason: 'timeout'
                });
            }
        }, duration);
    }
    
    /**
     * Handler para evento de declaração de guerra
     * @param {string} eventId ID do evento
     * @param {object} config Configuração do evento
     * @private
     */
    _handleWarDeclaration(eventId, config) {
        console.log(`[WorldEventManager] Iniciando declaração de guerra: ${config.name}`);
        
        // Implementar lógica de guerra entre facções/guildas
        const duration = (config.duration || 180) * 60 * 1000;
        let attackerScore = 0;
        let defenderScore = 0;
        
        // Simular batalhas durante a guerra
        const interval = setInterval(() => {
            if (!this.activeEvents.has(eventId)) {
                clearInterval(interval);
                return;
            }
            
            // Simular pontuação das facções
            attackerScore += Math.floor(Math.random() * 100);
            defenderScore += Math.floor(Math.random() * 100);
            
            const event = this.activeEvents.get(eventId);
            const elapsedTime = Date.now() - event.startTime;
            const progress = Math.min(100, (elapsedTime / duration) * 100);
            
            this._updateEventProgress(eventId, progress, {
                attackerScore,
                defenderScore,
                attackerName: config.attacker,
                defenderName: config.defender
            });
            
            if (progress >= 100) {
                clearInterval(interval);
                const winner = attackerScore > defenderScore ? config.attacker : config.defender;
                this._finishEvent(eventId, 'completed', {
                    winner,
                    attackerScore,
                    defenderScore
                });
            }
        }, 10 * 60 * 1000); // Atualizar a cada 10 minutos
    }
    
    /**
     * Handler para evento de desastre natural
     * @param {string} eventId ID do evento
     * @param {object} config Configuração do evento
     * @private
     */
    _handleNaturalDisaster(eventId, config) {
        console.log(`[WorldEventManager] Iniciando desastre natural: ${config.name}`);
        
        // Implementar lógica de desastres naturais
        const duration = (config.duration || 30) * 60 * 1000;
        
        // Simular impacto do desastre
        setTimeout(() => {
            if (!this.activeEvents.has(eventId)) return;
            
            this._updateEventProgress(eventId, 50, {
                disasterType: config.disasterType,
                affectedAreas: config.regions
            });
            
            // Finalizar o evento após a duração
            setTimeout(() => {
                if (!this.activeEvents.has(eventId)) return;
                
                this._finishEvent(eventId, 'completed', {
                    damage: Math.floor(Math.random() * 1000) + 500,
                    affectedPlayers: Math.floor(Math.random() * 200) + 50
                });
            }, duration / 2);
        }, duration / 2);
    }
}

module.exports = WorldEventManager;