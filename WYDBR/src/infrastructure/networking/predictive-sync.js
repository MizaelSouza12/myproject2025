/**
 * Sistema de Sincronização Preditiva WYDBRASIL
 * 
 * Sistema avançado para sincronização de estado entre cliente e servidor
 * com predição de movimento e interpolação suave para corrigir discrepâncias.
 */

class PredictiveSyncSystem {
    constructor(options = {}) {
        // Configurações
        this.maxPredictionTime = options.maxPredictionTime || 200; // ms
        this.snapThreshold = options.snapThreshold || 500; // unidades de distância
        this.correctionRate = options.correctionRate || 0.2; // taxa de correção por frame
        this.smoothingFactor = options.smoothingFactor || 0.8; // fator de suavização
        this.inputBufferSize = options.inputBufferSize || 5; // tamanho do buffer de inputs
        
        // Estado
        this.entities = new Map();
        this.inputBuffer = new Map();
        this.predictionResults = new Map();
        this.serverState = new Map();
        this.clientPredictions = new Map();
        this.lastUpdateTime = Date.now();
        
        // Estatísticas
        this.stats = {
            updates: 0,
            predictions: 0,
            corrections: 0,
            snaps: 0,
            totalError: 0,
            maxError: 0,
            avgLatency: 0,
            totalLatency: 0,
            latencySamples: 0
        };
    }
    
    /**
     * Registra uma entidade no sistema
     * @param {string} entityId ID da entidade
     * @param {object} initialState Estado inicial
     */
    registerEntity(entityId, initialState) {
        this.entities.set(entityId, {
            id: entityId,
            position: { ...initialState.position },
            velocity: { ...initialState.velocity },
            lastUpdateTime: Date.now(),
            inputSequence: 0,
            predictionEnabled: true
        });
        
        this.inputBuffer.set(entityId, []);
        this.predictionResults.set(entityId, []);
        this.serverState.set(entityId, { ...initialState });
        this.clientPredictions.set(entityId, { ...initialState });
        
        console.log(`[PredictiveSync] Entity ${entityId} registered with initial position (${initialState.position.x}, ${initialState.position.y}, ${initialState.position.z})`);
    }
    
    /**
     * Processa um input do cliente
     * @param {string} entityId ID da entidade
     * @param {object} input Input do cliente
     * @returns {object} Resultado da predição
     */
    processInput(entityId, input) {
        const entity = this.entities.get(entityId);
        
        if (!entity) {
            console.warn(`[PredictiveSync] Entity ${entityId} not found`);
            return null;
        }
        
        // Incrementar sequência de input
        entity.inputSequence++;
        
        // Adicionar timestamp ao input
        const inputWithTimestamp = {
            ...input,
            timestamp: Date.now(),
            sequence: entity.inputSequence
        };
        
        // Adicionar ao buffer
        const buffer = this.inputBuffer.get(entityId);
        buffer.push(inputWithTimestamp);
        
        // Limitar tamanho do buffer
        if (buffer.length > this.inputBufferSize) {
            buffer.shift();
        }
        
        // Aplicar predição imediatamente (simular processamento do input)
        const prediction = this.predictEntityState(entityId, inputWithTimestamp);
        
        // Armazenar resultado da predição
        const predictionResults = this.predictionResults.get(entityId);
        predictionResults.push({
            input: inputWithTimestamp,
            prediction
        });
        
        // Limitar tamanho do histórico de predições
        if (predictionResults.length > 20) {
            predictionResults.shift();
        }
        
        // Atualizar estado do cliente
        this.clientPredictions.set(entityId, prediction);
        
        // Atualizar estatísticas
        this.stats.predictions++;
        
        // Enviar input para o servidor (simulação)
        this.simulateSendInputToServer(entityId, inputWithTimestamp);
        
        return prediction;
    }
    
    /**
     * Simula o envio de input para o servidor
     * @param {string} entityId ID da entidade
     * @param {object} input Input do cliente
     */
    simulateSendInputToServer(entityId, input) {
        // Em um ambiente real, isso enviaria o input para o servidor
        // e o servidor responderia com atualizações de estado
        
        // Simular latência de rede aleatória entre 50 e 150ms
        const latency = 50 + Math.random() * 100;
        
        // Atualizar estatísticas de latência
        this.stats.totalLatency += latency;
        this.stats.latencySamples++;
        this.stats.avgLatency = this.stats.totalLatency / this.stats.latencySamples;
        
        // Simular perda de pacote (10% de chance)
        if (Math.random() < 0.1) {
            console.log(`[NETWORK] Pacote perdido (cliente -> servidor): input`);
            return;
        }
        
        // Simular pacote fora de ordem (5% de chance)
        if (Math.random() < 0.05) {
            const delay = Math.random() * 200;
            console.log(`[NETWORK] Pacote fora de ordem (cliente -> servidor): input (atraso: ${delay}ms)`);
        }
        
        // Simular processamento do servidor e resposta
        setTimeout(() => {
            // Servidor processa o input e atualiza o estado
            const serverResult = this.simulateServerProcessing(entityId, input);
            
            // Simular perda de pacote (10% de chance)
            if (Math.random() < 0.1) {
                console.log(`[NETWORK] Pacote perdido (servidor -> cliente): state_update`);
                return;
            }
            
            // Simular envio de atualização de estado para o cliente
            this.receiveServerUpdate(entityId, serverResult, input.sequence);
        }, latency);
    }
    
    /**
     * Simula o processamento do servidor
     * @param {string} entityId ID da entidade
     * @param {object} input Input do cliente
     * @returns {object} Resultado do processamento
     */
    simulateServerProcessing(entityId, input) {
        // Em um servidor real, isso processaria a física e lógica de jogo
        // conforme as regras do servidor, que podem ser diferentes do cliente
        
        // Para simplificar, usamos aqui uma implementação similar à predição do cliente,
        // mas com uma pequena variação para simular discrepâncias
        
        const serverState = this.serverState.get(entityId) || { position: { x: 0, y: 0, z: 0 }, velocity: { x: 0, y: 0, z: 0 } };
        
        // Aplicar input ao estado
        const moveSpeed = 5.0;
        const deltaTime = 16 / 1000; // 16ms em segundos
        
        let newVelocity = { x: 0, y: 0, z: 0 };
        
        if (input.moveForward) newVelocity.z -= moveSpeed;
        if (input.moveBackward) newVelocity.z += moveSpeed;
        if (input.moveLeft) newVelocity.x -= moveSpeed;
        if (input.moveRight) newVelocity.x += moveSpeed;
        if (input.jump) newVelocity.y += moveSpeed * 2;
        
        // Simulação simplificada de física
        // Introduzir uma pequena variação para simular diferenças de física
        const serverVariation = 1.0 + (Math.random() * 0.02 - 0.01); // ±1% de variação
        
        const newPosition = {
            x: serverState.position.x + newVelocity.x * deltaTime * serverVariation,
            y: serverState.position.y + newVelocity.y * deltaTime * serverVariation,
            z: serverState.position.z + newVelocity.z * deltaTime * serverVariation
        };
        
        // Atualizar o estado do servidor
        const newServerState = {
            position: newPosition,
            velocity: newVelocity,
            timestamp: Date.now(),
            inputSequence: input.sequence
        };
        
        this.serverState.set(entityId, newServerState);
        
        return newServerState;
    }
    
    /**
     * Recebe atualização de estado do servidor
     * @param {string} entityId ID da entidade
     * @param {object} serverState Estado do servidor
     * @param {number} inputSequence Sequência do input que gerou este estado
     */
    receiveServerUpdate(entityId, serverState, inputSequence) {
        const entity = this.entities.get(entityId);
        
        if (!entity) {
            console.warn(`[PredictiveSync] Entity ${entityId} not found for server update`);
            return;
        }
        
        // Armazenar estado do servidor
        this.serverState.set(entityId, serverState);
        
        // Obter a predição do cliente para esta sequência de input
        const predictionResults = this.predictionResults.get(entityId);
        const matchingPrediction = predictionResults.find(pr => pr.input.sequence === inputSequence);
        
        if (!matchingPrediction) {
            console.warn(`[PredictiveSync] No matching prediction found for input sequence ${inputSequence}`);
            return;
        }
        
        // Calcular erro entre a predição do cliente e o estado do servidor
        const error = this.calculateError(matchingPrediction.prediction.position, serverState.position);
        
        // Atualizar estatísticas de erro
        this.stats.totalError += error;
        this.stats.maxError = Math.max(this.stats.maxError, error);
        this.stats.updates++;
        
        // Verificar se o erro é muito grande e requer correção imediata (snap)
        if (error > this.snapThreshold) {
            console.log(`[SYNC] Erro de predição muito alto (${error.toFixed(2)}) para entidade ${entityId}. Aplicando snap.`);
            
            // Aplicar snap (correção imediata)
            entity.position = { ...serverState.position };
            this.clientPredictions.set(entityId, { ...serverState });
            
            // Atualizar estatísticas
            this.stats.snaps++;
            
            // Notificar cliente
            console.log(`[CLIENT] Correção para ${entityId}: erro=${error.toFixed(2)}, snap=true`);
        } else if (error > 0.01) {
            // Erro menor, aplicar correção suave
            this.applySmoothedCorrection(entityId, serverState, error);
            
            // Atualizar estatísticas
            this.stats.corrections++;
        }
    }
    
    /**
     * Aplica correção suave à posição da entidade
     * @param {string} entityId ID da entidade
     * @param {object} serverState Estado do servidor
     * @param {number} error Magnitude do erro
     */
    applySmoothedCorrection(entityId, serverState, error) {
        const entity = this.entities.get(entityId);
        const clientPrediction = this.clientPredictions.get(entityId);
        
        // Calcular interpolação baseada no erro (maior erro = correção mais rápida)
        const interpolationFactor = Math.min(this.correctionRate * (error / 50), 0.5);
        
        // Interpolar posição
        entity.position = {
            x: entity.position.x + (serverState.position.x - entity.position.x) * interpolationFactor,
            y: entity.position.y + (serverState.position.y - entity.position.y) * interpolationFactor,
            z: entity.position.z + (serverState.position.z - entity.position.z) * interpolationFactor
        };
        
        // Atualizar predição do cliente com a correção
        this.clientPredictions.set(entityId, {
            position: { ...entity.position },
            velocity: { ...serverState.velocity },
            timestamp: Date.now()
        });
    }
    
    /**
     * Prediz o estado da entidade baseado no input
     * @param {string} entityId ID da entidade
     * @param {object} input Input do cliente
     * @returns {object} Estado predito
     */
    predictEntityState(entityId, input) {
        const entity = this.entities.get(entityId);
        const clientPrediction = this.clientPredictions.get(entityId) || entity;
        
        if (!entity || !entity.predictionEnabled) {
            return clientPrediction;
        }
        
        // Aplicar input ao estado
        const moveSpeed = 5.0;
        const deltaTime = 16 / 1000; // 16ms em segundos
        
        let newVelocity = { x: 0, y: 0, z: 0 };
        
        if (input.moveForward) newVelocity.z -= moveSpeed;
        if (input.moveBackward) newVelocity.z += moveSpeed;
        if (input.moveLeft) newVelocity.x -= moveSpeed;
        if (input.moveRight) newVelocity.x += moveSpeed;
        if (input.jump) newVelocity.y += moveSpeed * 2;
        
        // Simulação simplificada de física
        const newPosition = {
            x: clientPrediction.position.x + newVelocity.x * deltaTime,
            y: clientPrediction.position.y + newVelocity.y * deltaTime,
            z: clientPrediction.position.z + newVelocity.z * deltaTime
        };
        
        // Atualizar a entidade
        entity.position = { ...newPosition };
        entity.velocity = { ...newVelocity };
        entity.lastUpdateTime = Date.now();
        
        return {
            position: newPosition,
            velocity: newVelocity,
            timestamp: Date.now(),
            inputSequence: input.sequence
        };
    }
    
    /**
     * Calcula a magnitude do erro entre duas posições
     * @param {object} pos1 Primeira posição
     * @param {object} pos2 Segunda posição
     * @returns {number} Magnitude do erro
     */
    calculateError(pos1, pos2) {
        const dx = pos1.x - pos2.x;
        const dy = pos1.y - pos2.y;
        const dz = pos1.z - pos2.z;
        
        return Math.sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    /**
     * Atualiza estado do sistema (chamado a cada frame)
     */
    update() {
        const now = Date.now();
        const deltaTime = (now - this.lastUpdateTime) / 1000;
        this.lastUpdateTime = now;
        
        // Atualizar cada entidade com predição
        for (const [entityId, entity] of this.entities) {
            if (!entity.predictionEnabled) continue;
            
            // Aplicar eventual correção suave contínua
            const serverState = this.serverState.get(entityId);
            const clientPrediction = this.clientPredictions.get(entityId);
            
            if (serverState && clientPrediction) {
                const error = this.calculateError(clientPrediction.position, serverState.position);
                
                // Aplicar correção muito suave para evitar saltos
                if (error > 0.1) {
                    this.applySmoothedCorrection(entityId, serverState, error);
                }
            }
        }
    }
    
    /**
     * Habilita ou desabilita a predição para uma entidade
     * @param {string} entityId ID da entidade
     * @param {boolean} enabled Se a predição deve ser habilitada
     */
    setPredictionEnabled(entityId, enabled) {
        const entity = this.entities.get(entityId);
        
        if (entity) {
            entity.predictionEnabled = enabled;
            console.log(`[PredictiveSync] Prediction ${enabled ? 'enabled' : 'disabled'} for entity ${entityId}`);
        }
    }
    
    /**
     * Obtém estatísticas do sistema
     * @returns {object} Estatísticas
     */
    getStats() {
        return {
            entities: this.entities.size,
            updates: this.stats.updates,
            predictions: this.stats.predictions,
            corrections: this.stats.corrections,
            snaps: this.stats.snaps,
            avgError: this.stats.updates > 0 ? this.stats.totalError / this.stats.updates : 0,
            maxError: this.stats.maxError,
            avgLatency: this.stats.avgLatency
        };
    }
}

module.exports = PredictiveSyncSystem;
