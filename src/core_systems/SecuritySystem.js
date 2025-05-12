// SecuritySystem Implementation
const EventEmitter = require('events');
const logger = require('../utils/logger');

class SecuritySystem {
    constructor() {
        this.events = new EventEmitter();
        this.initialized = false;
        this.blockedIPs = new Set();
        this.connectionCounts = new Map();
        this.rateLimits = new Map();
    }

    async initialize() {
        try {
            // Configurar limites
            this.maxConnectionsPerIP = process.env.MAX_CONNECTIONS_PER_IP || 5;
            this.rateLimitWindow = process.env.RATE_LIMIT_WINDOW || 60000; // 1 minuto
            this.rateLimitMax = process.env.RATE_LIMIT_MAX || 100;

            // Iniciar limpeza periódica
            this.startCleanupInterval();

            this.initialized = true;
            logger.info('Sistema de segurança inicializado com sucesso');
        } catch (error) {
            logger.error('Erro ao inicializar sistema de segurança:', error);
            throw error;
        }
    }

    startCleanupInterval() {
        // Limpar contadores a cada minuto
        setInterval(() => {
            this.connectionCounts.clear();
            this.rateLimits.clear();
            logger.debug('Contadores de segurança limpos');
        }, this.rateLimitWindow);
    }

    isIPBlocked(ip) {
        return this.blockedIPs.has(ip);
    }

    blockIP(ip, reason) {
        this.blockedIPs.add(ip);
        logger.warn(`IP bloqueado: ${ip}, Motivo: ${reason}`);
        this.events.emit('security:ip_blocked', { ip, reason });
    }

    unblockIP(ip) {
        this.blockedIPs.delete(ip);
        logger.info(`IP desbloqueado: ${ip}`);
        this.events.emit('security:ip_unblocked', { ip });
    }

    checkConnection(ip) {
        if (this.isIPBlocked(ip)) {
            return false;
        }

        // Verificar número de conexões
        const currentConnections = this.connectionCounts.get(ip) || 0;
        if (currentConnections >= this.maxConnectionsPerIP) {
            this.blockIP(ip, 'Excesso de conexões');
            return false;
        }

        this.connectionCounts.set(ip, currentConnections + 1);
        return true;
    }

    checkRateLimit(ip) {
        if (this.isIPBlocked(ip)) {
            return false;
        }

        const now = Date.now();
        const userRateLimit = this.rateLimits.get(ip) || { count: 0, firstRequest: now };

        // Resetar contador se passou a janela de tempo
        if (now - userRateLimit.firstRequest > this.rateLimitWindow) {
            userRateLimit.count = 0;
            userRateLimit.firstRequest = now;
        }

        userRateLimit.count++;

        if (userRateLimit.count > this.rateLimitMax) {
            this.blockIP(ip, 'Rate limit excedido');
            return false;
        }

        this.rateLimits.set(ip, userRateLimit);
        return true;
    }

    validatePacket(packet) {
        // Implementar validação de pacotes aqui
        // Por exemplo, verificar tamanho, formato, etc.
        return true;
    }

    async shutdown() {
        try {
            // Limpar todos os dados
            this.blockedIPs.clear();
            this.connectionCounts.clear();
            this.rateLimits.clear();

            this.initialized = false;
            logger.info('Sistema de segurança desligado com sucesso');
        } catch (error) {
            logger.error('Erro ao desligar sistema de segurança:', error);
            throw error;
        }
    }
}

module.exports = SecuritySystem;
