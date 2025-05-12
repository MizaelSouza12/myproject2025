/**
 * Sistema de Troca (Trading) WYDBRASIL
 * 
 * Sistema seguro e avançado para troca de itens entre jogadores
 * com validação em tempo real e proteção contra fraudes.
 */

const TradeManager = require('./core/TradeManager');
const TradeValidation = require('./validation/TradeValidation');
const TradeUI = require('./ui/TradeUI');
const TradeHistory = require('./history/TradeHistory');
const EventEmitter = require('events');

class TradingSystem extends EventEmitter {
    constructor(options = {}) {
        super();
        
        // Configurações
        this.config = {
            maxItemsPerTrade: 12,
            maxCurrencyPerTrade: 1000000000, // 1 bilhão
            minTradingLevel: 15,
            tradeDistanceLimit: 10,
            logEvents: true,
            uiEnabled: true,
            recordHistory: true,
            ...options
        };
        
        // Inicializar componentes
        this.manager = new TradeManager();
        this.validation = new TradeValidation();
        this.history = new TradeHistory();
        
        // Inicializar UI se habilitada
        if (this.config.uiEnabled) {
            this.ui = new TradeUI();
            this.ui.initialize({
                theme: options.theme || 'default',
                showItemDetails: options.showItemDetails !== false,
                animationsEnabled: options.animationsEnabled !== false,
                sounds: options.sounds !== false,
                autoAcceptThreshold: options.autoAcceptThreshold || 0,
                confirmationRequired: options.confirmationRequired !== false
            });
            
            // Registrar manipuladores de eventos da UI
            this._setupUIEventHandlers();
        }
        
        // Registrar manipuladores de eventos do manager de troca
        this._setupTradeManagerEventHandlers();
        
        console.log('[TradingSystem] Sistema de Troca WYDBRASIL inicializado');
    }
    
    /**
     * Inicia uma solicitação de troca entre dois jogadores
     * @param {string} initiatorId ID do jogador iniciador
     * @param {string} targetId ID do jogador alvo
     * @returns {object} Resultado da operação
     */
    initiateTradeRequest(initiatorId, targetId) {
        if (!initiatorId || !targetId) {
            return { success: false, error: 'invalid_player_ids' };
        }
        
        // Carregar dados dos jogadores (simulado)
        const initiator = this._getPlayerById(initiatorId);
        const target = this._getPlayerById(targetId);
        
        if (!initiator || !target) {
            return { success: false, error: 'player_not_found' };
        }
        
        // Validar a solicitação
        const validationResult = this.validation.canInitiateTrade(initiator, target);
        
        if (!validationResult.valid) {
            if (this.config.uiEnabled && initiator.isLocal) {
                this.ui.showNotification(validationResult.message, 'error');
            }
            
            this._logEvent('trade_request_rejected', {
                initiatorId,
                targetId,
                reason: validationResult.error
            });
            
            return { success: false, error: validationResult.error, message: validationResult.message };
        }
        
        // Iniciar a solicitação de troca
        const result = this.manager.initiateTradeRequest(initiatorId, targetId);
        
        if (result.success) {
            // Mostrar a solicitação na UI do jogador alvo
            if (this.config.uiEnabled && target.isLocal) {
                this.ui.showTradeRequest(initiatorId, initiator.name, result.tradeId);
            }
            
            this._logEvent('trade_request_sent', {
                initiatorId,
                targetId,
                tradeId: result.tradeId
            });
        }
        
        return result;
    }
    
    /**
     * Responde a uma solicitação de troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador respondendo
     * @param {boolean} accept Aceitar ou recusar a troca
     * @returns {object} Resultado da operação
     */
    respondToTradeRequest(tradeId, playerId, accept) {
        if (!tradeId || !playerId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        const result = this.manager.respondToTradeRequest(tradeId, playerId, accept);
        
        if (result.success) {
            if (accept) {
                // Troca aceita, mostrar janela de troca para ambos os jogadores
                const trade = result.trade;
                
                // Carregar dados dos jogadores
                const initiator = this._getPlayerById(trade.initiatorId);
                const target = this._getPlayerById(trade.targetId);
                
                if (initiator && target) {
                    // Mostrar janela de troca para o iniciador se for local
                    if (this.config.uiEnabled && initiator.isLocal) {
                        this.ui.showTradeWindow(tradeId, initiator, target);
                    }
                    
                    // Mostrar janela de troca para o alvo se for local
                    if (this.config.uiEnabled && target.isLocal) {
                        this.ui.showTradeWindow(tradeId, initiator, target);
                    }
                }
                
                this._logEvent('trade_request_accepted', {
                    tradeId,
                    playerId
                });
            } else {
                this._logEvent('trade_request_declined', {
                    tradeId,
                    playerId
                });
            }
        }
        
        return result;
    }
    
    /**
     * Adiciona um item à troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @param {object} item Item a ser adicionado
     * @returns {object} Resultado da operação
     */
    addItem(tradeId, playerId, item) {
        if (!tradeId || !playerId || !item) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        // Obter dados da troca
        const tradeInfo = this.manager.getTradeInfo(tradeId);
        
        if (!tradeInfo) {
            return { success: false, error: 'trade_not_found' };
        }
        
        // Validar item
        const validationResult = this.validation.validateAddItem(tradeInfo, playerId, item);
        
        if (!validationResult.valid) {
            if (this.config.uiEnabled) {
                const player = this._getPlayerById(playerId);
                if (player && player.isLocal) {
                    this.ui.showNotification(validationResult.message, 'warning');
                }
            }
            
            return { 
                success: false, 
                error: validationResult.error,
                message: validationResult.message
            };
        }
        
        // Adicionar item à troca
        const result = this.manager.addItem(tradeId, playerId, item);
        
        if (result.success) {
            // Atualizar UI para ambos os jogadores
            if (this.config.uiEnabled) {
                this._updateTradeUI(tradeId, result.trade);
            }
            
            this._logEvent('trade_item_added', {
                tradeId,
                playerId,
                itemId: item.id
            });
        }
        
        return result;
    }
    
    /**
     * Remove um item da troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @param {string} itemId ID do item
     * @returns {object} Resultado da operação
     */
    removeItem(tradeId, playerId, itemId) {
        if (!tradeId || !playerId || !itemId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        const result = this.manager.removeItem(tradeId, playerId, itemId);
        
        if (result.success) {
            // Atualizar UI para ambos os jogadores
            if (this.config.uiEnabled) {
                this._updateTradeUI(tradeId, result.trade);
            }
            
            this._logEvent('trade_item_removed', {
                tradeId,
                playerId,
                itemId
            });
        }
        
        return result;
    }
    
    /**
     * Define a quantidade de moeda na troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @param {number} amount Quantidade de moeda
     * @returns {object} Resultado da operação
     */
    setCurrency(tradeId, playerId, amount) {
        if (!tradeId || !playerId || amount === undefined) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        // Obter dados da troca
        const tradeInfo = this.manager.getTradeInfo(tradeId);
        
        if (!tradeInfo) {
            return { success: false, error: 'trade_not_found' };
        }
        
        // Validar quantidade
        const validationResult = this.validation.validateCurrency(tradeInfo, playerId, amount);
        
        if (!validationResult.valid) {
            if (this.config.uiEnabled) {
                const player = this._getPlayerById(playerId);
                if (player && player.isLocal) {
                    this.ui.showNotification(validationResult.message, 'warning');
                }
            }
            
            return { 
                success: false, 
                error: validationResult.error,
                message: validationResult.message
            };
        }
        
        // Definir moeda na troca
        const result = this.manager.setCurrency(tradeId, playerId, amount);
        
        if (result.success) {
            // Atualizar UI para ambos os jogadores
            if (this.config.uiEnabled) {
                this._updateTradeUI(tradeId, result.trade);
            }
            
            this._logEvent('trade_currency_updated', {
                tradeId,
                playerId,
                amount
            });
        }
        
        return result;
    }
    
    /**
     * Bloqueia a oferta do jogador (não pode mais alterar)
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @returns {object} Resultado da operação
     */
    lockOffer(tradeId, playerId) {
        if (!tradeId || !playerId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        const result = this.manager.lockOffer(tradeId, playerId);
        
        if (result.success) {
            // Atualizar UI para ambos os jogadores
            if (this.config.uiEnabled) {
                this._updateTradeUI(tradeId, result.trade);
            }
            
            this._logEvent('trade_offer_locked', {
                tradeId,
                playerId
            });
        }
        
        return result;
    }
    
    /**
     * Desbloqueia a oferta do jogador
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @returns {object} Resultado da operação
     */
    unlockOffer(tradeId, playerId) {
        if (!tradeId || !playerId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        const result = this.manager.unlockOffer(tradeId, playerId);
        
        if (result.success) {
            // Atualizar UI para ambos os jogadores
            if (this.config.uiEnabled) {
                this._updateTradeUI(tradeId, result.trade);
            }
            
            this._logEvent('trade_offer_unlocked', {
                tradeId,
                playerId
            });
        }
        
        return result;
    }
    
    /**
     * Confirma a troca
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador
     * @returns {object} Resultado da operação
     */
    confirmTrade(tradeId, playerId) {
        if (!tradeId || !playerId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        const result = this.manager.confirmTrade(tradeId, playerId);
        
        if (result.success) {
            // Atualizar UI para ambos os jogadores
            if (this.config.uiEnabled) {
                this._updateTradeUI(tradeId, result.trade);
            }
            
            this._logEvent('trade_confirmed', {
                tradeId,
                playerId,
                bothConfirmed: result.bothConfirmed
            });
            
            // Se a troca foi completada, registrar no histórico
            if (result.completed) {
                this._handleTradeCompletion(tradeId, result.trade);
            }
        }
        
        return result;
    }
    
    /**
     * Cancela uma troca ativa
     * @param {string} tradeId ID da troca
     * @param {string} playerId ID do jogador (opcional)
     * @param {string} reason Motivo do cancelamento
     * @returns {object} Resultado da operação
     */
    cancelTrade(tradeId, playerId = null, reason = 'user_cancelled') {
        if (!tradeId) {
            return { success: false, error: 'invalid_parameters' };
        }
        
        const result = this.manager.cancelTrade(tradeId, reason);
        
        if (result.success) {
            // Obter dados da troca antes do fechamento
            const tradeInfo = this.manager.getTradeInfo(tradeId);
            
            // Registrar no histórico
            if (this.config.recordHistory && tradeInfo) {
                this.history.recordCancelledTrade(tradeInfo, reason);
            }
            
            // Fechar UI para ambos os jogadores
            if (this.config.uiEnabled) {
                // Emular obtenção de informações do jogador
                const initiator = tradeInfo ? this._getPlayerById(tradeInfo.initiatorId) : null;
                const target = tradeInfo ? this._getPlayerById(tradeInfo.targetId) : null;
                
                if (initiator && initiator.isLocal) {
                    this.ui.closeTradeWindow(tradeId, { cancelled: true, reason });
                }
                
                if (target && target.isLocal) {
                    this.ui.closeTradeWindow(tradeId, { cancelled: true, reason });
                }
            }
            
            this._logEvent('trade_cancelled', {
                tradeId,
                playerId: playerId || 'system',
                reason
            });
        }
        
        return result;
    }
    
    /**
     * Obtém o histórico de trocas de um jogador
     * @param {string} playerId ID do jogador
     * @param {object} options Opções de filtragem (limit, offset, sort, etc.)
     * @returns {object} Lista de registros de troca
     */
    getPlayerTradeHistory(playerId, options = {}) {
        if (!this.config.recordHistory) {
            return { success: false, error: 'history_disabled' };
        }
        
        return this.history.getPlayerTradeHistory(playerId, options);
    }
    
    /**
     * Obtém estatísticas de trocas para um jogador
     * @param {string} playerId ID do jogador
     * @returns {object} Estatísticas de trocas
     */
    getPlayerTradeStats(playerId) {
        if (!this.config.recordHistory) {
            return { success: false, error: 'history_disabled' };
        }
        
        return this.history.getPlayerTradeStats(playerId);
    }
    
    /**
     * Busca registros no histórico de trocas
     * @param {object} filters Critérios de busca
     * @param {object} options Opções de filtragem (limit, offset, sort, etc.)
     * @returns {object} Lista de registros e metadata
     */
    searchTradeHistory(filters = {}, options = {}) {
        if (!this.config.recordHistory) {
            return { success: false, error: 'history_disabled' };
        }
        
        return this.history.searchTrades(filters, options);
    }
    
    /**
     * Verifica se um jogador está em uma troca ativa
     * @param {string} playerId ID do jogador
     * @returns {object} Resultado da verificação
     */
    isPlayerTrading(playerId) {
        if (!playerId) {
            return { success: false, error: 'invalid_player_id' };
        }
        
        const isTrading = this.manager.isPlayerTrading(playerId);
        const tradeId = isTrading ? this.manager.getPlayerCurrentTrade(playerId) : null;
        
        return {
            success: true,
            isTrading,
            tradeId
        };
    }
    
    /**
     * Atualiza a interface de troca para ambos os jogadores
     * @param {string} tradeId ID da troca
     * @param {object} tradeData Dados da troca
     * @private
     */
    _updateTradeUI(tradeId, tradeData) {
        if (!this.config.uiEnabled || !tradeData) return;
        
        // Obter informações dos jogadores
        const initiator = this._getPlayerById(tradeData.initiatorId);
        const target = this._getPlayerById(tradeData.targetId);
        
        // Atualizar UI do iniciador se for local
        if (initiator && initiator.isLocal) {
            this.ui.updateTradeState(tradeId, tradeData);
        }
        
        // Atualizar UI do alvo se for local
        if (target && target.isLocal) {
            this.ui.updateTradeState(tradeId, tradeData);
        }
    }
    
    /**
     * Manipula a conclusão de uma troca
     * @param {string} tradeId ID da troca
     * @param {object} tradeData Dados da troca
     * @private
     */
    _handleTradeCompletion(tradeId, tradeData) {
        // Registrar no histórico
        if (this.config.recordHistory && tradeData) {
            this.history.recordCompletedTrade(tradeData);
        }
        
        // Fechar UI para ambos os jogadores
        if (this.config.uiEnabled) {
            // Emular obtenção de informações do jogador
            const initiator = this._getPlayerById(tradeData.initiatorId);
            const target = this._getPlayerById(tradeData.targetId);
            
            if (initiator && initiator.isLocal) {
                this.ui.closeTradeWindow(tradeId, { success: true });
            }
            
            if (target && target.isLocal) {
                this.ui.closeTradeWindow(tradeId, { success: true });
            }
        }
        
        this._logEvent('trade_completed', {
            tradeId,
            initiatorId: tradeData.initiatorId,
            targetId: tradeData.targetId,
            initiatorItems: tradeData.initiatorItems.length,
            targetItems: tradeData.targetItems.length,
            initiatorCurrency: tradeData.initiatorCurrency,
            targetCurrency: tradeData.targetCurrency
        });
        
        // Emitir evento global
        this.emit('tradeCompleted', {
            tradeId,
            tradeData
        });
    }
    
    /**
     * Configura os manipuladores de eventos para a UI
     * @private
     */
    _setupUIEventHandlers() {
        // Manipulador para resposta à solicitação de troca
        this.ui.on('requestResponse', (data) => {
            const { requestId, playerId, accepted } = data;
            this.respondToTradeRequest(requestId, playerId, accepted);
        });
        
        // Manipulador para adicionar item
        this.ui.on('addItem', (data) => {
            const { tradeId, playerId, item } = data;
            this.addItem(tradeId, playerId, item);
        });
        
        // Manipulador para remover item
        this.ui.on('removeItem', (data) => {
            const { tradeId, playerId, itemId } = data;
            this.removeItem(tradeId, playerId, itemId);
        });
        
        // Manipulador para definir moeda
        this.ui.on('setCurrency', (data) => {
            const { tradeId, playerId, amount } = data;
            this.setCurrency(tradeId, playerId, amount);
        });
        
        // Manipulador para bloquear oferta
        this.ui.on('lockOffer', (data) => {
            const { tradeId, playerId } = data;
            this.lockOffer(tradeId, playerId);
        });
        
        // Manipulador para desbloquear oferta
        this.ui.on('unlockOffer', (data) => {
            const { tradeId, playerId } = data;
            this.unlockOffer(tradeId, playerId);
        });
        
        // Manipulador para confirmar troca
        this.ui.on('confirmTrade', (data) => {
            const { tradeId, playerId } = data;
            this.confirmTrade(tradeId, playerId);
        });
        
        // Manipulador para cancelar troca
        this.ui.on('cancelTrade', (data) => {
            const { tradeId, playerId, reason } = data;
            this.cancelTrade(tradeId, playerId, reason);
        });
    }
    
    /**
     * Configura os manipuladores de eventos para o gerenciador de troca
     * @private
     */
    _setupTradeManagerEventHandlers() {
        // Manipulador para evento de solicitação de troca
        this.manager.on('trade_requested', (data) => {
            this.emit('tradeRequested', data);
        });
        
        // Manipulador para evento de troca aceita
        this.manager.on('trade_accepted', (data) => {
            this.emit('tradeAccepted', data);
        });
        
        // Manipulador para evento de troca recusada
        this.manager.on('trade_declined', (data) => {
            this.emit('tradeDeclined', data);
        });
        
        // Manipulador para evento de troca concluída
        this.manager.on('trade_completed', (data) => {
            this.emit('tradeCompleted', data);
        });
        
        // Manipulador para evento de troca cancelada
        this.manager.on('trade_cancelled', (data) => {
            this.emit('tradeCancelled', data);
        });
    }
    
    /**
     * Registra um evento no log
     * @param {string} eventType Tipo de evento
     * @param {object} data Dados do evento
     * @private
     */
    _logEvent(eventType, data) {
        if (this.config.logEvents) {
            console.log(`[TradingSystem] [${eventType}] ${JSON.stringify(data)}`);
        }
        
        // Emitir evento
        this.emit(eventType, data);
    }
    
    /**
     * Obtém um jogador pelo ID (simulado)
     * @param {string} playerId ID do jogador
     * @returns {object|null} Dados do jogador ou null se não encontrado
     * @private
     */
    _getPlayerById(playerId) {
        // Esta é uma função auxiliar para simulação
        // Em uma implementação real, consultaria o sistema de jogadores
        // Exemplo: return PlayerManager.getPlayer(playerId);
        return null;
    }
}

module.exports = TradingSystem;
