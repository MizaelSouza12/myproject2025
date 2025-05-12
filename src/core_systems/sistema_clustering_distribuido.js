/**
 * WYDBR 2.0 - Implementação Gerada Automaticamente
 * Arquivo origem: sistema_clustering_distribuido.txt
 * 
 * Este arquivo contém a definição C++ do sistema, encapsulada em JavaScript
 * para integração no projeto WYDBR 2.0.
 */

const EventEmitter = require('events');
const logger = require('../utils/logger');

class ClusterSystem {
    constructor() {
        this.events = new EventEmitter();
        this.initialized = false;
        this.nodes = new Map();
        this.leader = null;
        this.nodeId = Date.now().toString(36) + Math.random().toString(36).substr(2);
    }

    async initialize() {
        try {
            // Configurar nó atual
            this.nodes.set(this.nodeId, {
                id: this.nodeId,
                status: 'active',
                role: 'follower',
                lastHeartbeat: Date.now(),
                address: process.env.NODE_ADDRESS || 'localhost',
                port: parseInt(process.env.NODE_PORT) || 8000
            });

            // Iniciar eleição de líder
            await this.startLeaderElection();

            // Iniciar heartbeat
            this.startHeartbeat();

            this.initialized = true;
            logger.info('Sistema de clustering inicializado com sucesso');
        } catch (error) {
            logger.error('Erro ao inicializar sistema de clustering:', error);
            throw error;
        }
    }

    startHeartbeat() {
        setInterval(() => {
            this.sendHeartbeat();
            this.checkNodes();
        }, 5000); // A cada 5 segundos
    }

    async startLeaderElection() {
        // Implementação simplificada do algoritmo de eleição
        // Em um sistema real, isso seria mais complexo e distribuído
        if (!this.leader) {
            const activeNodes = Array.from(this.nodes.values())
                .filter(node => node.status === 'active')
                .sort((a, b) => a.id.localeCompare(b.id));

            if (activeNodes.length > 0) {
                const newLeader = activeNodes[0];
                this.leader = newLeader.id;
                
                if (this.leader === this.nodeId) {
                    this.nodes.get(this.nodeId).role = 'leader';
                    logger.info('Este nó foi eleito líder');
                    this.events.emit('cluster:leader_elected', { nodeId: this.nodeId });
                }
            }
        }
    }

    sendHeartbeat() {
        const now = Date.now();
        const node = this.nodes.get(this.nodeId);
        if (node) {
            node.lastHeartbeat = now;
            this.events.emit('cluster:heartbeat', { nodeId: this.nodeId, timestamp: now });
        }
    }

    checkNodes() {
        const now = Date.now();
        const timeout = 15000; // 15 segundos

        for (const [nodeId, node] of this.nodes) {
            if (now - node.lastHeartbeat > timeout) {
                node.status = 'inactive';
                this.events.emit('cluster:node_inactive', { nodeId });

                // Se o líder ficar inativo, iniciar nova eleição
                if (nodeId === this.leader) {
                    this.leader = null;
                    this.startLeaderElection();
                }
            }
        }
    }

    async addNode(nodeInfo) {
        try {
            this.nodes.set(nodeInfo.id, {
                ...nodeInfo,
                status: 'active',
                lastHeartbeat: Date.now()
            });

            this.events.emit('cluster:node_added', { nodeId: nodeInfo.id });

            // Se não houver líder, iniciar eleição
            if (!this.leader) {
                await this.startLeaderElection();
            }

            return true;
        } catch (error) {
            logger.error('Erro ao adicionar nó:', error);
            throw error;
        }
    }

    removeNode(nodeId) {
        try {
            const node = this.nodes.get(nodeId);
            if (node) {
                this.nodes.delete(nodeId);
                this.events.emit('cluster:node_removed', { nodeId });

                // Se o líder for removido, iniciar nova eleição
                if (nodeId === this.leader) {
                    this.leader = null;
                    this.startLeaderElection();
                }
            }
            return true;
        } catch (error) {
            logger.error('Erro ao remover nó:', error);
            throw error;
        }
    }

    isLeader() {
        return this.nodeId === this.leader;
    }

    getLeader() {
        return this.nodes.get(this.leader);
    }

    getNodes() {
        return Array.from(this.nodes.values());
    }

    getActiveNodes() {
        return Array.from(this.nodes.values())
            .filter(node => node.status === 'active');
    }

    getNodeInfo(nodeId) {
        return this.nodes.get(nodeId);
    }

    async shutdown() {
        try {
            // Notificar outros nós
            if (this.isLeader()) {
                this.events.emit('cluster:leader_stepping_down', { nodeId: this.nodeId });
            }

            // Remover este nó do cluster
            this.removeNode(this.nodeId);

            this.initialized = false;
            logger.info('Sistema de clustering desligado com sucesso');
        } catch (error) {
            logger.error('Erro ao desligar sistema de clustering:', error);
            throw error;
        }
    }
}

module.exports = ClusterSystem;
