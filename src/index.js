/**
 * WYDBR 2.0 - Ponto de Entrada Principal
 * 
 * Este arquivo é o ponto de entrada principal para o sistema WYDBR 2.0.
 * Ele inicializa o integrador de sistemas e gerencia o ciclo de vida da aplicação.
 */

require('dotenv').config();
const logger = require('./utils/logger');
const EventSystem = require('./core_systems/EventSystem');
const SecuritySystem = require('./core_systems/SecuritySystem');
const NetworkManager = require('./core_systems/NetworkManager');
const GameplayManager = require('./core_systems/GameplayManager');

// Importar sistemas de banco de dados e cache
const DatabaseSystem = require('./core_systems/sistema_banco_dados_distribuido');
const CacheSystem = require('./core_systems/sistema_cache_hierarquico');
const ClusterSystem = require('./core_systems/sistema_clustering_distribuido');

// Importar sistemas de gameplay
const CharacterSystem = require('./game_systems/CharacterSystem');
const CombatSystem = require('./game_systems/CombatSystem');

class WYDBRServer {
    constructor() {
        this.systems = new Map();
        this.initialized = false;
    }

    async initialize() {
        try {
            logger.info('Iniciando WYDBR 2.0...');

            // Inicializar sistemas core
            await this.initializeSystem('events', new EventSystem());
            await this.initializeSystem('security', new SecuritySystem());
            await this.initializeSystem('network', new NetworkManager());
            await this.initializeSystem('gameplay', new GameplayManager());

            // Inicializar banco de dados e cache
            await this.initializeSystem('database', new DatabaseSystem());
            await this.initializeSystem('cache', new CacheSystem());
            await this.initializeSystem('cluster', new ClusterSystem());

            // Inicializar sistemas de gameplay
            await this.initializeSystem('character', new CharacterSystem());
            await this.initializeSystem('combat', new CombatSystem());

            this.initialized = true;
            logger.info('WYDBR 2.0 inicializado com sucesso!');

            // Iniciar servidor de jogo
            const port = process.env.GAME_PORT || 8282;
            this.systems.get('network').startGameServer(port);
            logger.info(`Servidor de jogo rodando na porta ${port}`);

        } catch (error) {
            logger.error('Erro ao inicializar WYDBR 2.0:', error);
            throw error;
        }
    }

    async initializeSystem(name, system) {
        try {
            logger.info(`Inicializando sistema: ${name}`);
            await system.initialize();
            this.systems.set(name, system);
            logger.info(`Sistema ${name} inicializado com sucesso`);
        } catch (error) {
            logger.error(`Erro ao inicializar sistema ${name}:`, error);
            throw error;
        }
    }

    async shutdown() {
        try {
            logger.info('Desligando WYDBR 2.0...');
            
            // Desligar todos os sistemas na ordem inversa
            const systemNames = Array.from(this.systems.keys()).reverse();
            
            for (const name of systemNames) {
                const system = this.systems.get(name);
                await system.shutdown();
                logger.info(`Sistema ${name} desligado com sucesso`);
            }

            this.initialized = false;
            logger.info('WYDBR 2.0 desligado com sucesso');
        } catch (error) {
            logger.error('Erro ao desligar WYDBR 2.0:', error);
            throw error;
        }
    }
}

// Criar e iniciar o servidor
const server = new WYDBRServer();

// Tratamento de sinais para desligamento gracioso
process.on('SIGINT', async () => {
    logger.info('Sinal SIGINT recebido. Iniciando desligamento...');
    await server.shutdown();
    process.exit(0);
});

process.on('SIGTERM', async () => {
    logger.info('Sinal SIGTERM recebido. Iniciando desligamento...');
    await server.shutdown();
    process.exit(0);
});

// Iniciar o servidor
server.initialize().catch(error => {
    logger.error('Erro fatal ao iniciar o servidor:', error);
    process.exit(1);
});
