/**
 * Sistema de Trocas WYDBRASIL - Trade Manager
 * 
 * Componente central que gerencia o estado e ciclo de vida
 * de trocas entre jogadores.
 */

const EventEmitter = require('events');
const { v4: uuidv4 } = require('uuid');

class TradeManager extends EventEmitter {
    constructor() {
        super();
        
        // Armazenar trocas em andamento
        this.activeTrades = new Map();
        this.tradeRequests = new Map();
        
        // Rastrear jogadores em troca
        this.playerTrades = new Map();
        
        // Limites e configurações
        this.config = {
            maxItemsPerTrade: 12,
            maxTradeLifetime: 5 * 60 * 1000, // 5 minutos
            tradeRequestTimeout: 60 * 1000,   // 1 minuto
            idleTimeout: 2 * 60 * 1000        // 2 minutos
        };
        
        // Iniciar limpeza periódica
        this._startCleanupTimer();
        
        console.log('[TradeManager] Gerenciador de trocas inicializado');
    }
    
    /**
     * Inicia uma solicitação de troca entre dois jogadores
     * @param {string} initiatorId ID do jogador iniciador
     * @param {string} targetId ID do jogador alvo
     * @returns {object} Resultado da operação com ID da troca
     */
    initiateTradeRequest(initiatorId, targetId) {
        if (!initiatorId || !targetId) {
            return { success: false, error: 'invalid_player_ids' };
        }
        
        if (initiatorId === targetId) {
            return { success: false, error: 'cannot_trade_with_self' };
        }
        
        // Verificar se qualquer um dos jogadores já está negociando
        if (this.playerTrades.has(initiatorId)) {
            return { success: false, error: 'initiator_already_trading' };
        }
        
        if (this.playerTrades.has(targetId)) {
            return { success: false, error: 'target_already_trading' };
        }
        
        // Criar ID da troca
        const tradeId = `trade_${Date.now()}_${uuidv4().substring(0, 8)}`;
        
        // Criar registro da solicitação
        const tradeRequest = {
            id: tradeId,
            initiatorId,
            targetId,
            createdAt: Date.now(),
            state: 'requested'
        };
        
        // Registrar solicitação
        this.tradeRequests.set(tradeId, tradeRequest);
        
        // Emitir evento
        this.emit('trade_requested', {
            tradeId,
            initiatorId,
            targetId
        });
        
        console.log(`[TradeManager] Solicitação de troca iniciada: ${tradeId} (${initiatorId} → ${targetId})`);
        
        return {
            success: true,
            tradeId
        };
    }
    
    /**
     * Responde a uma solicitação de troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador respondendo
     * @param {boolean} accept Aceitar ou recusar a troca
     * @returns {object} Resultado da operação
     */
    respondToTradeRequest(tradeId, playerId, accept) {
        // Obter solicitação
        const request = this.tradeRequests.get(tradeId);
        
        if (!request) {
            return { success: false, error: 'trade_request_not_found' };
        }
        
        // Verificar se o jogador é o alvo da solicitação
        if (request.targetId !== playerId) {
            return { success: false, error: 'not_trade_target' };
        }
        
        // Verificar estado
        if (request.state !== 'requested') {
            return { success: false, error: 'invalid_request_state' };
        }
        
        // Recusar troca
        if (!accept) {
            // Remover solicitação
            this.tradeRequests.delete(tradeId);
            
            // Emitir evento
            this.emit('trade_declined', {
                tradeId,
                initiatorId: request.initiatorId,
                targetId: request.targetId
            });
            
            console.log(`[TradeManager] Solicitação de troca recusada: ${tradeId}`);
            
            return {
                success: true,
                accepted: false
            };
        }
        
        // Aceitar troca
        // Verificar novamente se qualquer jogador entrou em troca no meio tempo
        if (this.playerTrades.has(request.initiatorId)) {
            this.tradeRequests.delete(tradeId);
            return { success: false, error: 'initiator_now_trading' };
        }
        
        if (this.playerTrades.has(request.targetId)) {
            this.tradeRequests.delete(tradeId);
            return { success: false, error: 'target_now_trading' };
        }
        
        // Criar troca ativa
        const trade = {
            id: tradeId,
            initiatorId: request.initiatorId,
            targetId: request.targetId,
            initiatorItems: [],
            targetItems: [],
            initiatorCurrency: 0,
            targetCurrency: 0,
            initiatorLocked: false,
            targetLocked: false,
            initiatorConfirmed: false,
            targetConfirmed: false,
            createdAt: request.createdAt,
            acceptedAt: Date.now(),
            updatedAt: Date.now(),
            state: 'active',
            lastActivity: Date.now()
        };
        
        // Registrar troca
        this.activeTrades.set(tradeId, trade);
        
        // Mapear jogadores para esta troca
        this.playerTrades.set(request.initiatorId, tradeId);
        this.playerTrades.set(request.targetId, tradeId);
        
        // Remover solicitação
        this.tradeRequests.delete(tradeId);
        
        // Emitir evento
        this.emit('trade_accepted', {
            tradeId,
            initiatorId: request.initiatorId,
            targetId: request.targetId
        });
        
        console.log(`[TradeManager] Solicitação de troca aceita: ${tradeId}`);
        
        return {
            success: true,
            accepted: true,
            trade
        };
    }
    
    /**
     * Adiciona um item à troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @param {object} item Item a ser adicionado
     * @returns {object} Resultado da operação
     */
    addItem(tradeId, playerId, item) {
        // Validar parâmetros
        if (!tradeId || !playerId || !item) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        // Obter troca
        const trade = this.activeTrades.get(tradeId);
        
        if (!trade) {
            return { success: false, error: 'trade_not_found' };
        }
        
        // Verificar estado da troca
        if (trade.state !== 'active') {
            return { success: false, error: 'invalid_trade_state' };
        }
        
        // Determinar se o jogador é o iniciador ou o alvo
        const isInitiator = playerId === trade.initiatorId;
        const isTarget = playerId === trade.targetId;
        
        if (!isInitiator && !isTarget) {
            return { success: false, error: 'player_not_in_trade' };
        }
        
        // Verificar se o jogador já bloqueou a oferta
        if ((isInitiator && trade.initiatorLocked) || (isTarget && trade.targetLocked)) {
            return { success: false, error: 'offer_locked' };
        }
        
        // Determinar a lista de itens do jogador
        const itemsList = isInitiator ? trade.initiatorItems : trade.targetItems;
        
        // Verificar limite de itens
        if (itemsList.length >= this.config.maxItemsPerTrade) {
            return { success: false, error: 'item_limit_reached' };
        }
        
        // Verificar se o item já está na troca
        const existingItem = itemsList.find(i => i.id === item.id);
        if (existingItem) {
            return { success: false, error: 'item_already_added' };
        }
        
        // Adicionar item
        itemsList.push({
            ...item,
            addedAt: Date.now()
        });
        
        // Atualizar estado da troca
        trade.updatedAt = Date.now();
        trade.lastActivity = Date.now();
        
        // Se o jogador havia confirmado, remover a confirmação
        if (isInitiator && trade.initiatorConfirmed) {
            trade.initiatorConfirmed = false;
        }
        if (isTarget && trade.targetConfirmed) {
            trade.targetConfirmed = false;
        }
        
        console.log(`[TradeManager] Item adicionado: ${item.id} à troca ${tradeId} por ${playerId}`);
        
        // Emitir evento
        this.emit('trade_item_added', {
            tradeId,
            playerId,
            item
        });
        
        return {
            success: true,
            trade
        };
    }
    
    /**
     * Remove um item da troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @param {string} itemId ID do item
     * @returns {object} Resultado da operação
     */
    removeItem(tradeId, playerId, itemId) {
        // Validar parâmetros
        if (!tradeId || !playerId || !itemId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        // Obter troca
        const trade = this.activeTrades.get(tradeId);
        
        if (!trade) {
            return { success: false, error: 'trade_not_found' };
        }
        
        // Verificar estado da troca
        if (trade.state !== 'active') {
            return { success: false, error: 'invalid_trade_state' };
        }
        
        // Determinar se o jogador é o iniciador ou o alvo
        const isInitiator = playerId === trade.initiatorId;
        const isTarget = playerId === trade.targetId;
        
        if (!isInitiator && !isTarget) {
            return { success: false, error: 'player_not_in_trade' };
        }
        
        // Verificar se o jogador já bloqueou a oferta
        if ((isInitiator && trade.initiatorLocked) || (isTarget && trade.targetLocked)) {
            return { success: false, error: 'offer_locked' };
        }
        
        // Determinar a lista de itens do jogador
        const itemsList = isInitiator ? trade.initiatorItems : trade.targetItems;
        
        // Encontrar o item
        const itemIndex = itemsList.findIndex(i => i.id === itemId);
        
        if (itemIndex === -1) {
            return { success: false, error: 'item_not_found' };
        }
        
        // Remover item
        itemsList.splice(itemIndex, 1);
        
        // Atualizar estado da troca
        trade.updatedAt = Date.now();
        trade.lastActivity = Date.now();
        
        // Se o jogador havia confirmado, remover a confirmação
        if (isInitiator && trade.initiatorConfirmed) {
            trade.initiatorConfirmed = false;
        }
        if (isTarget && trade.targetConfirmed) {
            trade.targetConfirmed = false;
        }
        
        console.log(`[TradeManager] Item removido: ${itemId} da troca ${tradeId} por ${playerId}`);
        
        // Emitir evento
        this.emit('trade_item_removed', {
            tradeId,
            playerId,
            itemId
        });
        
        return {
            success: true,
            trade
        };
    }
    
    /**
     * Define a quantidade de moeda na troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @param {number} amount Quantidade de moeda
     * @returns {object} Resultado da operação
     */
    setCurrency(tradeId, playerId, amount) {
        // Validar parâmetros
        if (!tradeId || !playerId || amount === undefined) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        // Converter para número se necessário
        amount = Number(amount);
        
        // Validar quantidade
        if (isNaN(amount) || amount < 0) {
            return { success: false, error: 'invalid_currency_amount' };
        }
        
        // Obter troca
        const trade = this.activeTrades.get(tradeId);
        
        if (!trade) {
            return { success: false, error: 'trade_not_found' };
        }
        
        // Verificar estado da troca
        if (trade.state !== 'active') {
            return { success: false, error: 'invalid_trade_state' };
        }
        
        // Determinar se o jogador é o iniciador ou o alvo
        const isInitiator = playerId === trade.initiatorId;
        const isTarget = playerId === trade.targetId;
        
        if (!isInitiator && !isTarget) {
            return { success: false, error: 'player_not_in_trade' };
        }
        
        // Verificar se o jogador já bloqueou a oferta
        if ((isInitiator && trade.initiatorLocked) || (isTarget && trade.targetLocked)) {
            return { success: false, error: 'offer_locked' };
        }
        
        // Definir moeda
        if (isInitiator) {
            trade.initiatorCurrency = amount;
        } else {
            trade.targetCurrency = amount;
        }
        
        // Atualizar estado da troca
        trade.updatedAt = Date.now();
        trade.lastActivity = Date.now();
        
        // Se o jogador havia confirmado, remover a confirmação
        if (isInitiator && trade.initiatorConfirmed) {
            trade.initiatorConfirmed = false;
        }
        if (isTarget && trade.targetConfirmed) {
            trade.targetConfirmed = false;
        }
        
        console.log(`[TradeManager] Moeda definida: ${amount} na troca ${tradeId} por ${playerId}`);
        
        // Emitir evento
        this.emit('trade_currency_set', {
            tradeId,
            playerId,
            amount
        });
        
        return {
            success: true,
            trade
        };
    }
    
    /**
     * Bloqueia a oferta do jogador (não pode mais alterar)
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @returns {object} Resultado da operação
     */
    lockOffer(tradeId, playerId) {
        // Validar parâmetros
        if (!tradeId || !playerId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        // Obter troca
        const trade = this.activeTrades.get(tradeId);
        
        if (!trade) {
            return { success: false, error: 'trade_not_found' };
        }
        
        // Verificar estado da troca
        if (trade.state !== 'active') {
            return { success: false, error: 'invalid_trade_state' };
        }
        
        // Determinar se o jogador é o iniciador ou o alvo
        const isInitiator = playerId === trade.initiatorId;
        const isTarget = playerId === trade.targetId;
        
        if (!isInitiator && !isTarget) {
            return { success: false, error: 'player_not_in_trade' };
        }
        
        // Bloquear oferta
        if (isInitiator) {
            if (trade.initiatorLocked) {
                return { success: false, error: 'offer_already_locked' };
            }
            trade.initiatorLocked = true;
        } else {
            if (trade.targetLocked) {
                return { success: false, error: 'offer_already_locked' };
            }
            trade.targetLocked = true;
        }
        
        // Atualizar estado da troca
        trade.updatedAt = Date.now();
        trade.lastActivity = Date.now();
        
        console.log(`[TradeManager] Oferta bloqueada por ${playerId} na troca ${tradeId}`);
        
        // Emitir evento
        this.emit('trade_offer_locked', {
            tradeId,
            playerId
        });
        
        return {
            success: true,
            trade
        };
    }
    
    /**
     * Desbloqueia a oferta do jogador
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @returns {object} Resultado da operação
     */
    unlockOffer(tradeId, playerId) {
        // Validar parâmetros
        if (!tradeId || !playerId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        // Obter troca
        const trade = this.activeTrades.get(tradeId);
        
        if (!trade) {
            return { success: false, error: 'trade_not_found' };
        }
        
        // Verificar estado da troca
        if (trade.state !== 'active') {
            return { success: false, error: 'invalid_trade_state' };
        }
        
        // Determinar se o jogador é o iniciador ou o alvo
        const isInitiator = playerId === trade.initiatorId;
        const isTarget = playerId === trade.targetId;
        
        if (!isInitiator && !isTarget) {
            return { success: false, error: 'player_not_in_trade' };
        }
        
        // Verificar se o jogador já confirmou
        if ((isInitiator && trade.initiatorConfirmed) || (isTarget && trade.targetConfirmed)) {
            return { success: false, error: 'trade_already_confirmed' };
        }
        
        // Desbloquear oferta
        if (isInitiator) {
            if (!trade.initiatorLocked) {
                return { success: false, error: 'offer_not_locked' };
            }
            trade.initiatorLocked = false;
            trade.initiatorConfirmed = false;
        } else {
            if (!trade.targetLocked) {
                return { success: false, error: 'offer_not_locked' };
            }
            trade.targetLocked = false;
            trade.targetConfirmed = false;
        }
        
        // Atualizar estado da troca
        trade.updatedAt = Date.now();
        trade.lastActivity = Date.now();
        
        console.log(`[TradeManager] Oferta desbloqueada por ${playerId} na troca ${tradeId}`);
        
        // Emitir evento
        this.emit('trade_offer_unlocked', {
            tradeId,
            playerId
        });
        
        return {
            success: true,
            trade
        };
    }
    
    /**
     * Confirma a troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @returns {object} Resultado da operação
     */
    confirmTrade(tradeId, playerId) {
        // Validar parâmetros
        if (!tradeId || !playerId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        // Obter troca
        const trade = this.activeTrades.get(tradeId);
        
        if (!trade) {
            return { success: false, error: 'trade_not_found' };
        }
        
        // Verificar estado da troca
        if (trade.state !== 'active') {
            return { success: false, error: 'invalid_trade_state' };
        }
        
        // Determinar se o jogador é o iniciador ou o alvo
        const isInitiator = playerId === trade.initiatorId;
        const isTarget = playerId === trade.targetId;
        
        if (!isInitiator && !isTarget) {
            return { success: false, error: 'player_not_in_trade' };
        }
        
        // Verificar se o jogador bloqueou a oferta
        if (isInitiator && !trade.initiatorLocked) {
            return { success: false, error: 'offer_not_locked' };
        }
        
        if (isTarget && !trade.targetLocked) {
            return { success: false, error: 'offer_not_locked' };
        }
        
        // Confirmar troca
        if (isInitiator) {
            if (trade.initiatorConfirmed) {
                return { success: false, error: 'already_confirmed' };
            }
            trade.initiatorConfirmed = true;
        } else {
            if (trade.targetConfirmed) {
                return { success: false, error: 'already_confirmed' };
            }
            trade.targetConfirmed = true;
        }
        
        // Atualizar estado da troca
        trade.updatedAt = Date.now();
        trade.lastActivity = Date.now();
        
        console.log(`[TradeManager] Troca confirmada por ${playerId} (troca ${tradeId})`);
        
        let bothConfirmed = false;
        let tradeCompleted = false;
        
        // Verificar se ambos confirmaram
        if (trade.initiatorConfirmed && trade.targetConfirmed) {
            bothConfirmed = true;
            
            // Completar a troca
            trade.state = 'completed';
            trade.completedAt = Date.now();
            
            // Remover as referências dos jogadores
            this.playerTrades.delete(trade.initiatorId);
            this.playerTrades.delete(trade.targetId);
            
            // Executar a troca de itens (em um sistema real, haveria lógica para transferir itens)
            
            // Emitir evento de troca concluída
            this.emit('trade_completed', {
                tradeId,
                trade
            });
            
            console.log(`[TradeManager] Troca concluída: ${tradeId}`);
            
            tradeCompleted = true;
        }
        
        // Emitir evento de confirmação
        this.emit('trade_confirmation', {
            tradeId,
            playerId,
            bothConfirmed
        });
        
        return {
            success: true,
            bothConfirmed,
            completed: tradeCompleted,
            trade
        };
    }
    
    /**
     * Cancela uma troca ativa
     * @param {string} tradeId ID da troca
     * @param {string} reason Motivo do cancelamento
     * @returns {object} Resultado da operação
     */
    cancelTrade(tradeId, reason = 'user_cancelled') {
        // Validar parâmetros
        if (!tradeId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        // Verificar se é uma solicitação de troca
        const request = this.tradeRequests.get(tradeId);
        if (request) {
            // Remover solicitação
            this.tradeRequests.delete(tradeId);
            
            // Emitir evento
            this.emit('trade_request_cancelled', {
                tradeId,
                initiatorId: request.initiatorId,
                targetId: request.targetId,
                reason
            });
            
            console.log(`[TradeManager] Solicitação de troca cancelada: ${tradeId}`);
            
            return {
                success: true,
                cancelledRequest: true
            };
        }
        
        // Obter troca
        const trade = this.activeTrades.get(tradeId);
        
        if (!trade) {
            return { success: false, error: 'trade_not_found' };
        }
        
        // Verificar se a troca já foi completada ou cancelada
        if (trade.state === 'completed') {
            return { success: false, error: 'trade_already_completed' };
        }
        
        if (trade.state === 'cancelled') {
            return { success: false, error: 'trade_already_cancelled' };
        }
        
        // Atualizar estado da troca
        trade.state = 'cancelled';
        trade.cancelledAt = Date.now();
        trade.cancelReason = reason;
        
        // Remover as referências dos jogadores
        this.playerTrades.delete(trade.initiatorId);
        this.playerTrades.delete(trade.targetId);
        
        console.log(`[TradeManager] Troca cancelada: ${tradeId}, motivo: ${reason}`);
        
        // Emitir evento
        this.emit('trade_cancelled', {
            tradeId,
            trade,
            reason
        });
        
        return {
            success: true,
            trade
        };
    }
    
    /**
     * Obtém informações sobre uma troca
     * @param {string} tradeId ID da troca
     * @returns {object|null} Dados da troca ou null se não encontrada
     */
    getTradeInfo(tradeId) {
        return this.activeTrades.get(tradeId) || this.tradeRequests.get(tradeId) || null;
    }
    
    /**
     * Verifica se um jogador está em uma troca ativa
     * @param {string} playerId ID do jogador
     * @returns {boolean} Verdadeiro se o jogador está em uma troca
     */
    isPlayerTrading(playerId) {
        return this.playerTrades.has(playerId);
    }
    
    /**
     * Obtém o ID da troca atual de um jogador
     * @param {string} playerId ID do jogador
     * @returns {string|null} ID da troca ou null se o jogador não está em troca
     */
    getPlayerCurrentTrade(playerId) {
        return this.playerTrades.get(playerId) || null;
    }
    
    /**
     * Inicia o temporizador de limpeza periódica
     * @private
     */
    _startCleanupTimer() {
        this._cleanupInterval = setInterval(() => {
            this._cleanupExpiredTrades();
        }, 60 * 1000); // Executar a cada minuto
    }
    
    /**
     * Limpa trocas e solicitações expiradas
     * @private
     */
    _cleanupExpiredTrades() {
        const now = Date.now();
        
        // Limpar solicitações expiradas
        for (const [id, request] of this.tradeRequests.entries()) {
            const age = now - request.createdAt;
            
            if (age > this.config.tradeRequestTimeout) {
                this.tradeRequests.delete(id);
                
                // Emitir evento
                this.emit('trade_request_expired', {
                    tradeId: id,
                    initiatorId: request.initiatorId,
                    targetId: request.targetId
                });
                
                console.log(`[TradeManager] Solicitação de troca expirada: ${id}`);
            }
        }
        
        // Limpar trocas ociosas
        for (const [id, trade] of this.activeTrades.entries()) {
            if (trade.state !== 'active') continue;
            
            const idleTime = now - trade.lastActivity;
            const totalTime = now - trade.createdAt;
            
            if (idleTime > this.config.idleTimeout || totalTime > this.config.maxTradeLifetime) {
                // Cancelar troca
                this.cancelTrade(id, idleTime > this.config.idleTimeout ? 'idle_timeout' : 'max_lifetime_exceeded');
            }
        }
    }
    
    /**
     * Encerra o gerenciador de trocas
     */
    shutdown() {
        if (this._cleanupInterval) {
            clearInterval(this._cleanupInterval);
            this._cleanupInterval = null;
        }
        
        console.log('[TradeManager] Gerenciador de trocas encerrado');
    }
}

module.exports = TradeManager;