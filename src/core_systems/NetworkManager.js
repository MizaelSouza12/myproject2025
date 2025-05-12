// NetworkManager Implementation
const EventEmitter = require('events');
const net = require('net');
const logger = require('../utils/logger');

class NetworkManager {
    constructor() {
        this.events = new EventEmitter();
        this.initialized = false;
        this.servers = new Map();
        this.connections = new Map();
    }

    async initialize() {
        try {
            this.initialized = true;
            logger.info('Sistema de rede inicializado com sucesso');
        } catch (error) {
            logger.error('Erro ao inicializar sistema de rede:', error);
            throw error;
        }
    }

    startGameServer(port) {
        if (this.servers.has('game')) {
            throw new Error('Servidor de jogo já está rodando');
        }

        const server = net.createServer((socket) => {
            const clientId = `${socket.remoteAddress}:${socket.remotePort}`;
            
            logger.info(`Nova conexão: ${clientId}`);
            this.connections.set(clientId, socket);

            socket.on('data', (data) => {
                try {
                    this.handleGameData(clientId, data);
                } catch (error) {
                    logger.error(`Erro ao processar dados do cliente ${clientId}:`, error);
                }
            });

            socket.on('error', (error) => {
                logger.error(`Erro na conexão ${clientId}:`, error);
                this.handleDisconnect(clientId);
            });

            socket.on('close', () => {
                logger.info(`Conexão fechada: ${clientId}`);
                this.handleDisconnect(clientId);
            });
        });

        server.on('error', (error) => {
            logger.error('Erro no servidor de jogo:', error);
            this.events.emit('server:error', { type: 'game', error });
        });

        server.listen(port, () => {
            logger.info(`Servidor de jogo rodando na porta ${port}`);
            this.servers.set('game', server);
            this.events.emit('server:started', { type: 'game', port });
        });
    }

    handleGameData(clientId, data) {
        try {
            // Aqui você implementaria o processamento dos pacotes do jogo
            // Por exemplo, decodificar o pacote e emitir eventos apropriados
            this.events.emit('network:data', { clientId, data });
        } catch (error) {
            logger.error(`Erro ao processar dados do jogo:`, error);
            throw error;
        }
    }

    handleDisconnect(clientId) {
        const socket = this.connections.get(clientId);
        if (socket) {
            socket.destroy();
            this.connections.delete(clientId);
            this.events.emit('client:disconnected', { clientId });
        }
    }

    broadcast(data, excludeClient = null) {
        for (const [clientId, socket] of this.connections) {
            if (clientId !== excludeClient && socket.writable) {
                try {
                    socket.write(data);
                } catch (error) {
                    logger.error(`Erro ao enviar broadcast para ${clientId}:`, error);
                }
            }
        }
    }

    sendTo(clientId, data) {
        const socket = this.connections.get(clientId);
        if (socket && socket.writable) {
            try {
                socket.write(data);
                return true;
            } catch (error) {
                logger.error(`Erro ao enviar dados para ${clientId}:`, error);
                return false;
            }
        }
        return false;
    }

    getConnectedClients() {
        return Array.from(this.connections.keys());
    }

    async shutdown() {
        try {
            // Fechar todas as conexões
            for (const [clientId] of this.connections) {
                this.handleDisconnect(clientId);
            }

            // Fechar todos os servidores
            for (const [type, server] of this.servers) {
                server.close(() => {
                    logger.info(`Servidor ${type} fechado`);
                });
            }

            this.servers.clear();
            this.initialized = false;
            logger.info('Sistema de rede desligado com sucesso');
        } catch (error) {
            logger.error('Erro ao desligar sistema de rede:', error);
            throw error;
        }
    }
}

module.exports = NetworkManager;
