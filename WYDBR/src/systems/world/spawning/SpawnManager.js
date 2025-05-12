/**
 * Gerenciador de Spawns do WYDBRASIL
 * 
 * Controla o aparecimento dinâmico de criaturas, NPCs e recursos
 * no mundo do jogo, baseado em condições, zonas e regras específicas.
 */

const EventEmitter = require('events');

class SpawnManager extends EventEmitter {
    constructor() {
        super();
        this.spawnPoints = new Map();
        this.activeSpawns = new Map();
        this.spawnUpdateInterval = null;
        this.nextSpawnPointId = 1;
        this.nextSpawnId = 1;
        this.isRunning = false;
        
        // Tipos de spawns suportados
        this.spawnTypes = {
            'monster': {
                handler: this._spawnMonster.bind(this),
                despawnHandler: this._despawnMonster.bind(this)
            },
            'npc': {
                handler: this._spawnNPC.bind(this),
                despawnHandler: this._despawnNPC.bind(this)
            },
            'resource': {
                handler: this._spawnResource.bind(this),
                despawnHandler: this._despawnResource.bind(this)
            },
            'item': {
                handler: this._spawnItem.bind(this),
                despawnHandler: this._despawnItem.bind(this)
            }
        };
        
        console.log('[SpawnManager] Inicializado');
    }
    
    /**
     * Inicia o gerenciador de spawns
     */
    start() {
        if (this.isRunning) return;
        
        this.isRunning = true;
        
        // Atualizar spawns a cada 10 segundos
        this.spawnUpdateInterval = setInterval(() => {
            this._updateSpawnPoints();
        }, 10000);
        
        console.log('[SpawnManager] Sistema de spawns iniciado');
    }
    
    /**
     * Para o gerenciador de spawns
     */
    stop() {
        if (!this.isRunning) return;
        
        this.isRunning = false;
        
        if (this.spawnUpdateInterval) {
            clearInterval(this.spawnUpdateInterval);
            this.spawnUpdateInterval = null;
        }
        
        console.log('[SpawnManager] Sistema de spawns parado');
    }
    
    /**
     * Adiciona um novo ponto de spawn
     * @param {string} region Região do mundo
     * @param {object} spawnConfig Configuração do spawn
     * @returns {string} ID do ponto de spawn
     */
    addSpawnPoint(region, spawnConfig) {
        // Validar configuração
        if (!spawnConfig.type || !this.spawnTypes[spawnConfig.type]) {
            console.error(`[SpawnManager] Tipo de spawn inválido: ${spawnConfig.type}`);
            return null;
        }
        
        const spawnPointId = `spawn-point-${this.nextSpawnPointId++}`;
        
        // Configurar o ponto de spawn
        const spawnPoint = {
            id: spawnPointId,
            region: region,
            position: spawnConfig.position || { x: 0, y: 0, z: 0 },
            type: spawnConfig.type,
            entities: spawnConfig.entities || [],
            maxEntities: spawnConfig.maxEntities || 10,
            radius: spawnConfig.radius || 10,
            spawnRate: spawnConfig.spawnRate || 60, // segundos entre spawns
            despawnTime: spawnConfig.despawnTime || 300, // tempo de vida em segundos
            conditions: spawnConfig.conditions || {},
            spawnedEntities: [], // IDs das entidades ativas
            nextSpawnTime: Date.now() + (spawnConfig.initialDelay || 0) * 1000,
            enabled: spawnConfig.enabled !== false
        };
        
        this.spawnPoints.set(spawnPointId, spawnPoint);
        
        console.log(`[SpawnManager] Ponto de spawn adicionado: ${spawnPointId} na região ${region}`);
        
        return spawnPointId;
    }
    
    /**
     * Remove um ponto de spawn
     * @param {string} spawnPointId ID do ponto de spawn
     * @returns {boolean} Sucesso da operação
     */
    removeSpawnPoint(spawnPointId) {
        if (!this.spawnPoints.has(spawnPointId)) {
            console.error(`[SpawnManager] Ponto de spawn não encontrado: ${spawnPointId}`);
            return false;
        }
        
        const spawnPoint = this.spawnPoints.get(spawnPointId);
        
        // Remover todas as entidades associadas
        for (const entityId of spawnPoint.spawnedEntities) {
            this._despawnEntity(entityId);
        }
        
        this.spawnPoints.delete(spawnPointId);
        
        console.log(`[SpawnManager] Ponto de spawn removido: ${spawnPointId}`);
        
        return true;
    }
    
    /**
     * Atualiza a configuração de um ponto de spawn
     * @param {string} spawnPointId ID do ponto de spawn
     * @param {object} updateConfig Novas configurações
     * @returns {boolean} Sucesso da operação
     */
    updateSpawnPoint(spawnPointId, updateConfig) {
        if (!this.spawnPoints.has(spawnPointId)) {
            console.error(`[SpawnManager] Ponto de spawn não encontrado: ${spawnPointId}`);
            return false;
        }
        
        const spawnPoint = this.spawnPoints.get(spawnPointId);
        
        // Atualizar propriedades
        for (const [key, value] of Object.entries(updateConfig)) {
            if (key !== 'id' && key !== 'spawnedEntities' && key !== 'nextSpawnTime') {
                spawnPoint[key] = value;
            }
        }
        
        console.log(`[SpawnManager] Ponto de spawn atualizado: ${spawnPointId}`);
        
        return true;
    }
    
    /**
     * Habilita ou desabilita um ponto de spawn
     * @param {string} spawnPointId ID do ponto de spawn
     * @param {boolean} enabled Estado de habilitação
     * @returns {boolean} Sucesso da operação
     */
    setSpawnPointEnabled(spawnPointId, enabled) {
        if (!this.spawnPoints.has(spawnPointId)) {
            console.error(`[SpawnManager] Ponto de spawn não encontrado: ${spawnPointId}`);
            return false;
        }
        
        const spawnPoint = this.spawnPoints.get(spawnPointId);
        spawnPoint.enabled = enabled;
        
        console.log(`[SpawnManager] Ponto de spawn ${spawnPointId} ${enabled ? 'habilitado' : 'desabilitado'}`);
        
        return true;
    }
    
    /**
     * Obtém um ponto de spawn pelo ID
     * @param {string} spawnPointId ID do ponto de spawn
     * @returns {object|null} Informações do ponto de spawn ou null se não existir
     */
    getSpawnPoint(spawnPointId) {
        return this.spawnPoints.has(spawnPointId) ? { ...this.spawnPoints.get(spawnPointId) } : null;
    }
    
    /**
     * Obtém todos os pontos de spawn em uma região
     * @param {string} region Nome da região
     * @returns {Array} Lista de pontos de spawn
     */
    getSpawnPointsByRegion(region) {
        return Array.from(this.spawnPoints.values())
            .filter(point => point.region === region)
            .map(point => ({ ...point }));
    }
    
    /**
     * Força o spawn imediato em um ponto específico
     * @param {string} spawnPointId ID do ponto de spawn
     * @param {number} count Quantidade de entidades para spawnar
     * @returns {Array} Lista de IDs das entidades spawnadas
     */
    forceSpawn(spawnPointId, count = 1) {
        if (!this.spawnPoints.has(spawnPointId)) {
            console.error(`[SpawnManager] Ponto de spawn não encontrado: ${spawnPointId}`);
            return [];
        }
        
        const spawnPoint = this.spawnPoints.get(spawnPointId);
        const spawnedIds = [];
        
        // Verificar se já atingiu o limite
        if (spawnPoint.spawnedEntities.length >= spawnPoint.maxEntities) {
            console.log(`[SpawnManager] Limite de entidades atingido para o ponto de spawn ${spawnPointId}`);
            return [];
        }
        
        // Calcular quantas entidades podem ser spawnadas
        const availableSlots = spawnPoint.maxEntities - spawnPoint.spawnedEntities.length;
        const spawnCount = Math.min(count, availableSlots);
        
        // Realizar os spawns
        for (let i = 0; i < spawnCount; i++) {
            const entityId = this._spawnEntity(spawnPoint);
            if (entityId) {
                spawnedIds.push(entityId);
            }
        }
        
        return spawnedIds;
    }
    
    /**
     * Remove todas as entidades spawnadas por um ponto
     * @param {string} spawnPointId ID do ponto de spawn
     * @returns {number} Quantidade de entidades removidas
     */
    clearSpawns(spawnPointId) {
        if (!this.spawnPoints.has(spawnPointId)) {
            console.error(`[SpawnManager] Ponto de spawn não encontrado: ${spawnPointId}`);
            return 0;
        }
        
        const spawnPoint = this.spawnPoints.get(spawnPointId);
        const entityCount = spawnPoint.spawnedEntities.length;
        
        // Remover todas as entidades
        for (const entityId of [...spawnPoint.spawnedEntities]) {
            this._despawnEntity(entityId);
        }
        
        console.log(`[SpawnManager] Removidas ${entityCount} entidades do ponto de spawn ${spawnPointId}`);
        
        return entityCount;
    }
    
    /**
     * Atualiza todos os pontos de spawn
     * @private
     */
    _updateSpawnPoints() {
        const now = Date.now();
        
        // Verificar cada ponto de spawn
        for (const [spawnPointId, spawnPoint] of this.spawnPoints.entries()) {
            // Pular pontos desabilitados
            if (!spawnPoint.enabled) continue;
            
            // Verificar se é hora de spawnar
            if (now >= spawnPoint.nextSpawnTime && spawnPoint.spawnedEntities.length < spawnPoint.maxEntities) {
                const entityId = this._spawnEntity(spawnPoint);
                
                // Calcular próximo spawn
                spawnPoint.nextSpawnTime = now + (spawnPoint.spawnRate * 1000);
            }
        }
        
        // Verificar entidades que devem ser despawnadas
        for (const [entityId, entity] of this.activeSpawns.entries()) {
            if (now >= entity.despawnTime) {
                this._despawnEntity(entityId);
            }
        }
    }
    
    /**
     * Gera uma nova entidade em um ponto de spawn
     * @param {object} spawnPoint Ponto de spawn
     * @returns {string|null} ID da entidade spawnada ou null em caso de falha
     * @private
     */
    _spawnEntity(spawnPoint) {
        // Verificar se o ponto já atingiu o limite
        if (spawnPoint.spawnedEntities.length >= spawnPoint.maxEntities) {
            return null;
        }
        
        // Verificar se há entidades disponíveis para spawn
        if (!spawnPoint.entities || spawnPoint.entities.length === 0) {
            console.error(`[SpawnManager] Nenhuma entidade configurada para o ponto de spawn ${spawnPoint.id}`);
            return null;
        }
        
        // Selecionar uma entidade aleatória baseada nos pesos
        const entityConfig = this._selectRandomEntity(spawnPoint.entities);
        
        if (!entityConfig) {
            console.error(`[SpawnManager] Falha ao selecionar entidade para o ponto de spawn ${spawnPoint.id}`);
            return null;
        }
        
        // Gerar posição aleatória dentro do raio do ponto
        const position = this._getRandomPosition(spawnPoint.position, spawnPoint.radius);
        
        // Criar ID único para a entidade
        const entityId = `spawn-entity-${this.nextSpawnId++}`;
        
        // Criar entidade
        const entity = {
            id: entityId,
            spawnPointId: spawnPoint.id,
            type: spawnPoint.type,
            entityType: entityConfig.id,
            position,
            spawnTime: Date.now(),
            despawnTime: Date.now() + (spawnPoint.despawnTime * 1000),
            config: { ...entityConfig }
        };
        
        // Adicionar ao ponto de spawn e à lista de entidades ativas
        spawnPoint.spawnedEntities.push(entityId);
        this.activeSpawns.set(entityId, entity);
        
        // Chamar handler específico do tipo
        if (this.spawnTypes[spawnPoint.type] && this.spawnTypes[spawnPoint.type].handler) {
            this.spawnTypes[spawnPoint.type].handler(entity);
        }
        
        console.log(`[SpawnManager] Entidade ${entityConfig.id} (${entityId}) spawnada no ponto ${spawnPoint.id}`);
        
        // Emitir evento de spawn
        this.emit('entity_spawned', {
            id: entityId,
            type: spawnPoint.type,
            entityType: entityConfig.id,
            position,
            region: spawnPoint.region
        });
        
        return entityId;
    }
    
    /**
     * Remove uma entidade spawnada
     * @param {string} entityId ID da entidade
     * @returns {boolean} Sucesso da operação
     * @private
     */
    _despawnEntity(entityId) {
        if (!this.activeSpawns.has(entityId)) {
            console.error(`[SpawnManager] Entidade não encontrada: ${entityId}`);
            return false;
        }
        
        const entity = this.activeSpawns.get(entityId);
        const spawnPointId = entity.spawnPointId;
        
        // Remover do ponto de spawn
        if (this.spawnPoints.has(spawnPointId)) {
            const spawnPoint = this.spawnPoints.get(spawnPointId);
            const index = spawnPoint.spawnedEntities.indexOf(entityId);
            
            if (index !== -1) {
                spawnPoint.spawnedEntities.splice(index, 1);
            }
        }
        
        // Chamar handler específico do tipo
        if (this.spawnTypes[entity.type] && this.spawnTypes[entity.type].despawnHandler) {
            this.spawnTypes[entity.type].despawnHandler(entity);
        }
        
        this.activeSpawns.delete(entityId);
        
        console.log(`[SpawnManager] Entidade ${entity.entityType} (${entityId}) despawnada`);
        
        // Emitir evento de despawn
        this.emit('entity_despawned', {
            id: entityId,
            type: entity.type,
            entityType: entity.entityType,
            position: entity.position
        });
        
        return true;
    }
    
    /**
     * Seleciona uma entidade aleatória com base nos pesos
     * @param {Array} entities Lista de entidades com pesos
     * @returns {object|null} Entidade selecionada ou null se falhar
     * @private
     */
    _selectRandomEntity(entities) {
        // Verificar se há entidades
        if (!entities || entities.length === 0) {
            return null;
        }
        
        // Calcular soma total dos pesos
        const totalWeight = entities.reduce((sum, entity) => sum + (entity.weight || 1), 0);
        
        // Gerar número aleatório entre 0 e o peso total
        const random = Math.random() * totalWeight;
        
        // Selecionar entidade com base no peso
        let cumulativeWeight = 0;
        
        for (const entity of entities) {
            cumulativeWeight += (entity.weight || 1);
            
            if (random <= cumulativeWeight) {
                return entity;
            }
        }
        
        // Fallback: retornar a primeira entidade
        return entities[0];
    }
    
    /**
     * Gera uma posição aleatória dentro de um raio
     * @param {object} center Posição central
     * @param {number} radius Raio máximo
     * @returns {object} Posição aleatória
     * @private
     */
    _getRandomPosition(center, radius) {
        const angle = Math.random() * Math.PI * 2;
        const distance = Math.random() * radius;
        
        return {
            x: center.x + Math.cos(angle) * distance,
            y: center.y,
            z: center.z + Math.sin(angle) * distance
        };
    }
    
    /**
     * Verifica se as condições para spawn são atendidas
     * @param {object} conditions Condições a verificar
     * @returns {boolean} Se as condições são atendidas
     * @private
     */
    _checkConditions(conditions) {
        // Implementação básica, poderia verificar hora do dia, clima, etc.
        return true;
    }
    
    // Handlers específicos para cada tipo de spawn
    
    /**
     * Handler para spawn de monstros
     * @param {object} entity Entidade a ser spawnada
     * @private
     */
    _spawnMonster(entity) {
        // Aqui seria integrado com o sistema de combate/monstros
        console.log(`[SpawnManager] Monstro ${entity.entityType} spawnado em ${JSON.stringify(entity.position)}`);
    }
    
    /**
     * Handler para spawn de NPCs
     * @param {object} entity Entidade a ser spawnada
     * @private
     */
    _spawnNPC(entity) {
        // Aqui seria integrado com o sistema de NPCs
        console.log(`[SpawnManager] NPC ${entity.entityType} spawnado em ${JSON.stringify(entity.position)}`);
    }
    
    /**
     * Handler para spawn de recursos
     * @param {object} entity Entidade a ser spawnada
     * @private
     */
    _spawnResource(entity) {
        // Aqui seria integrado com o sistema de recursos/coleta
        console.log(`[SpawnManager] Recurso ${entity.entityType} spawnado em ${JSON.stringify(entity.position)}`);
    }
    
    /**
     * Handler para spawn de itens
     * @param {object} entity Entidade a ser spawnada
     * @private
     */
    _spawnItem(entity) {
        // Aqui seria integrado com o sistema de itens
        console.log(`[SpawnManager] Item ${entity.entityType} spawnado em ${JSON.stringify(entity.position)}`);
    }
    
    /**
     * Handler para despawn de monstros
     * @param {object} entity Entidade a ser despawnada
     * @private
     */
    _despawnMonster(entity) {
        // Aqui seria integrado com o sistema de combate/monstros
        console.log(`[SpawnManager] Monstro ${entity.entityType} despawnado`);
    }
    
    /**
     * Handler para despawn de NPCs
     * @param {object} entity Entidade a ser despawnada
     * @private
     */
    _despawnNPC(entity) {
        // Aqui seria integrado com o sistema de NPCs
        console.log(`[SpawnManager] NPC ${entity.entityType} despawnado`);
    }
    
    /**
     * Handler para despawn de recursos
     * @param {object} entity Entidade a ser despawnada
     * @private
     */
    _despawnResource(entity) {
        // Aqui seria integrado com o sistema de recursos/coleta
        console.log(`[SpawnManager] Recurso ${entity.entityType} despawnado`);
    }
    
    /**
     * Handler para despawn de itens
     * @param {object} entity Entidade a ser despawnada
     * @private
     */
    _despawnItem(entity) {
        // Aqui seria integrado com o sistema de itens
        console.log(`[SpawnManager] Item ${entity.entityType} despawnado`);
    }
}

module.exports = SpawnManager;