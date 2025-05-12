// Updated EventSystem Implementation
const EventEmitter = require('events');
const logger = require('../utils/logger');

class EventSystem {
    constructor() {
        this.emitter = new EventEmitter();
        this.handlers = new Map();
        this.initialized = false;
    }

    async initialize() {
        try {
            this.initialized = true;
            logger.info('Sistema de eventos inicializado com sucesso');
        } catch (error) {
            logger.error('Erro ao inicializar sistema de eventos:', error);
            throw error;
        }
    }

    on(event, handler) {
        this.handlers.set(handler, event);
        this.emitter.on(event, handler);
        logger.debug(`Handler registered for event: ${event}`);
    }

    off(event, handler) {
        this.handlers.delete(handler);
        this.emitter.off(event, handler);
        logger.debug(`Handler removed for event: ${event}`);
    }

    emit(event, data) {
        this.emitter.emit(event, data);
        logger.debug(`Event emitted: ${event}`, { data });
    }

    clearAll() {
        this.handlers.forEach((event, handler) => {
            this.off(event, handler);
        });
        logger.info('All event handlers cleared');
    }

    async shutdown() {
        try {
            this.clearAll();
            this.initialized = false;
            logger.info('Sistema de eventos desligado com sucesso');
        } catch (error) {
            logger.error('Erro ao desligar sistema de eventos:', error);
            throw error;
        }
    }
}

module.exports = EventSystem;
