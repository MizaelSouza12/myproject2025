/**
 * Sistema de Trocas WYDBRASIL - Trade UI
 * 
 * Componente responsável pela integração dos elementos
 * de interface de usuário relacionados ao sistema de trocas.
 */

class TradeUI {
    constructor() {
        // Referências para elementos visuais
        this.activeWindows = new Map();
        this.eventHandlers = {};
        
        // Status da interface
        this.isReady = false;
        
        console.log('[TradeUI] Interface do sistema de trocas inicializada');
    }
    
    /**
     * Inicializa a interface do sistema de trocas
     * @param {object} config Configurações da interface
     * @returns {boolean} Sucesso da inicialização
     */
    initialize(config = {}) {
        // Configuração padrão
        this.config = {
            containerSelector: '#trade-container',
            theme: 'default',
            showItemDetails: true,
            animationsEnabled: true,
            sounds: true,
            autoAcceptThreshold: 0, // 0 = não auto-aceitar
            confirmationRequired: true,
            ...config
        };
        
        // Registrar ouvintes de eventos
        this._registerEventListeners();
        
        // Carregar recursos visuais
        this._loadResources();
        
        this.isReady = true;
        console.log('[TradeUI] Interface inicializada com sucesso');
        
        return true;
    }
    
    /**
     * Mostra a janela de solicitação de troca
     * @param {string} playerId ID do jogador que solicitou a troca
     * @param {string} playerName Nome do jogador
     * @param {string} requestId ID da solicitação
     * @returns {object} Status da operação
     */
    showTradeRequest(playerId, playerName, requestId) {
        if (!this.isReady) {
            return { success: false, error: 'ui_not_ready' };
        }
        
        console.log(`[TradeUI] Exibindo solicitação de troca de ${playerName} (${playerId}), ID: ${requestId}`);
        
        // Em uma implementação real, isto criaria uma janela na interface do jogo
        // Aqui apenas simulamos o comportamento
        
        // Verificar se já existe uma solicitação deste jogador
        if (this.activeWindows.has(`request_${playerId}`)) {
            return { success: false, error: 'request_already_active' };
        }
        
        // Criar objeto representando a janela
        const requestWindow = {
            id: `request_${requestId}`,
            type: 'trade_request',
            playerId,
            playerName,
            requestId,
            timestamp: Date.now()
        };
        
        // Registrar a janela ativa
        this.activeWindows.set(`request_${playerId}`, requestWindow);
        
        // Reproduzir som de notificação
        this._playSound('trade_request');
        
        // Simular exibição na interface
        console.log(`[TradeUI] Solicitação de troca exibida: "${playerName} deseja negociar com você."`);
        
        // Se auto-aceitar estiver configurado, simular aceitação automática
        if (this.config.autoAcceptThreshold > 0) {
            console.log(`[TradeUI] Auto-aceitar configurado: ${this.config.autoAcceptThreshold}ms`);
            setTimeout(() => {
                this._triggerEvent('requestResponse', {
                    requestId,
                    playerId,
                    accepted: true,
                    auto: true
                });
            }, this.config.autoAcceptThreshold);
        }
        
        return { success: true, windowId: requestWindow.id };
    }
    
    /**
     * Exibe a janela principal de troca
     * @param {string} tradeId ID da troca
     * @param {object} initiator Dados do jogador iniciador
     * @param {object} target Dados do jogador alvo
     * @returns {object} Status da operação
     */
    showTradeWindow(tradeId, initiator, target) {
        if (!this.isReady) {
            return { success: false, error: 'ui_not_ready' };
        }
        
        console.log(`[TradeUI] Exibindo janela de troca ID: ${tradeId}`);
        
        // Limpar solicitações anteriores relacionadas a estes jogadores
        if (this.activeWindows.has(`request_${initiator.id}`)) {
            this.activeWindows.delete(`request_${initiator.id}`);
        }
        
        if (this.activeWindows.has(`request_${target.id}`)) {
            this.activeWindows.delete(`request_${target.id}`);
        }
        
        // Verificar se já existe uma janela de troca aberta
        if (this.activeWindows.has(`trade_active`)) {
            return { success: false, error: 'trade_already_active' };
        }
        
        // Criar objeto representando a janela de troca
        const tradeWindow = {
            id: `trade_${tradeId}`,
            type: 'trade_window',
            tradeId,
            initiator: { ...initiator },
            target: { ...target },
            initiatorItems: [],
            targetItems: [],
            initiatorCurrency: 0,
            targetCurrency: 0,
            initiatorLocked: false,
            targetLocked: false,
            initiatorConfirmed: false,
            targetConfirmed: false,
            isLocal: target.isLocal || false, // Se o jogador local é o alvo
            localItems: [], // Lista de itens locais que podem ser adicionados
            timestamp: Date.now()
        };
        
        // Registrar a janela ativa
        this.activeWindows.set('trade_active', tradeWindow);
        
        // Reproduzir som de abertura
        this._playSound('trade_open');
        
        // Simular exibição na interface
        console.log(`[TradeUI] Janela de troca aberta entre ${initiator.name} e ${target.name}`);
        
        return { success: true, windowId: tradeWindow.id };
    }
    
    /**
     * Atualiza o estado da janela de troca
     * @param {string} tradeId ID da troca
     * @param {object} tradeState Novo estado da troca
     * @returns {object} Status da operação
     */
    updateTradeState(tradeId, tradeState) {
        const tradeWindow = this.activeWindows.get('trade_active');
        
        if (!tradeWindow || tradeWindow.tradeId !== tradeId) {
            return { success: false, error: 'trade_window_not_found' };
        }
        
        // Atualizar estado
        if (tradeState.initiatorItems !== undefined) {
            tradeWindow.initiatorItems = [...tradeState.initiatorItems];
        }
        
        if (tradeState.targetItems !== undefined) {
            tradeWindow.targetItems = [...tradeState.targetItems];
        }
        
        if (tradeState.initiatorCurrency !== undefined) {
            tradeWindow.initiatorCurrency = tradeState.initiatorCurrency;
        }
        
        if (tradeState.targetCurrency !== undefined) {
            tradeWindow.targetCurrency = tradeState.targetCurrency;
        }
        
        if (tradeState.initiatorLocked !== undefined) {
            const wasLocked = tradeWindow.initiatorLocked;
            tradeWindow.initiatorLocked = tradeState.initiatorLocked;
            
            if (!wasLocked && tradeState.initiatorLocked) {
                this._playSound('trade_lock');
            }
        }
        
        if (tradeState.targetLocked !== undefined) {
            const wasLocked = tradeWindow.targetLocked;
            tradeWindow.targetLocked = tradeState.targetLocked;
            
            if (!wasLocked && tradeState.targetLocked) {
                this._playSound('trade_lock');
            }
        }
        
        if (tradeState.initiatorConfirmed !== undefined) {
            const wasConfirmed = tradeWindow.initiatorConfirmed;
            tradeWindow.initiatorConfirmed = tradeState.initiatorConfirmed;
            
            if (!wasConfirmed && tradeState.initiatorConfirmed) {
                this._playSound('trade_confirm');
            }
        }
        
        if (tradeState.targetConfirmed !== undefined) {
            const wasConfirmed = tradeWindow.targetConfirmed;
            tradeWindow.targetConfirmed = tradeState.targetConfirmed;
            
            if (!wasConfirmed && tradeState.targetConfirmed) {
                this._playSound('trade_confirm');
            }
        }
        
        // Atualizar interface
        console.log(`[TradeUI] Estado da troca atualizado para: ${JSON.stringify({
            initiatorItems: tradeWindow.initiatorItems.length,
            targetItems: tradeWindow.targetItems.length,
            initiatorCurrency: tradeWindow.initiatorCurrency,
            targetCurrency: tradeWindow.targetCurrency,
            initiatorLocked: tradeWindow.initiatorLocked,
            targetLocked: tradeWindow.targetLocked,
            initiatorConfirmed: tradeWindow.initiatorConfirmed,
            targetConfirmed: tradeWindow.targetConfirmed
        })}`);
        
        // Verificar se ambos confirmaram para mostrar progresso
        if (tradeWindow.initiatorConfirmed && tradeWindow.targetConfirmed) {
            this._showTradeCompletionProgress();
        }
        
        return { success: true };
    }
    
    /**
     * Fecha a janela de troca
     * @param {string} tradeId ID da troca
     * @param {object} result Resultado da troca
     * @returns {object} Status da operação
     */
    closeTradeWindow(tradeId, result = {}) {
        const tradeWindow = this.activeWindows.get('trade_active');
        
        if (!tradeWindow || tradeWindow.tradeId !== tradeId) {
            return { success: false, error: 'trade_window_not_found' };
        }
        
        // Reproduzir som baseado no resultado
        if (result.success) {
            this._playSound('trade_success');
        } else if (result.cancelled) {
            this._playSound('trade_cancel');
        } else {
            this._playSound('trade_close');
        }
        
        // Exibir mensagem baseada no resultado
        if (result.success) {
            console.log('[TradeUI] Troca concluída com sucesso!');
            this._showTradeResult(tradeWindow, 'success', result);
        } else if (result.cancelled) {
            console.log(`[TradeUI] Troca cancelada: ${result.reason || 'Razão desconhecida'}`);
            this._showTradeResult(tradeWindow, 'cancelled', result);
        } else if (result.error) {
            console.log(`[TradeUI] Erro na troca: ${result.error}`);
            this._showTradeResult(tradeWindow, 'error', result);
        } else {
            console.log('[TradeUI] Troca fechada');
        }
        
        // Remover a janela ativa
        this.activeWindows.delete('trade_active');
        
        return { success: true };
    }
    
    /**
     * Atualiza a lista de itens disponíveis para troca
     * @param {Array} items Lista de itens
     * @returns {object} Status da operação
     */
    updateAvailableItems(items) {
        const tradeWindow = this.activeWindows.get('trade_active');
        
        if (!tradeWindow) {
            return { success: false, error: 'trade_window_not_found' };
        }
        
        tradeWindow.localItems = [...items];
        
        console.log(`[TradeUI] Lista de itens disponíveis atualizada: ${items.length} itens`);
        
        return { success: true };
    }
    
    /**
     * Mostra uma notificação relacionada a troca
     * @param {string} message Mensagem da notificação
     * @param {string} type Tipo de notificação (info, warning, error, success)
     * @param {object} options Opções adicionais
     * @returns {object} Status da operação
     */
    showNotification(message, type = 'info', options = {}) {
        if (!this.isReady) {
            return { success: false, error: 'ui_not_ready' };
        }
        
        // Definir som baseado no tipo
        let sound;
        switch (type) {
            case 'error':
                sound = 'notification_error';
                break;
            case 'warning':
                sound = 'notification_warning';
                break;
            case 'success':
                sound = 'notification_success';
                break;
            default:
                sound = 'notification_info';
        }
        
        // Reproduzir som
        this._playSound(sound);
        
        // Exibir notificação
        console.log(`[TradeUI] Notificação (${type}): ${message}`);
        
        return { success: true };
    }
    
    /**
     * Define um manipulador de eventos para interface
     * @param {string} eventName Nome do evento
     * @param {function} handler Função manipuladora
     */
    on(eventName, handler) {
        if (typeof handler !== 'function') {
            console.error(`[TradeUI] Handler inválido para evento ${eventName}`);
            return;
        }
        
        this.eventHandlers[eventName] = handler;
        console.log(`[TradeUI] Handler registrado para evento '${eventName}'`);
    }
    
    /**
     * Remove um manipulador de eventos
     * @param {string} eventName Nome do evento
     */
    off(eventName) {
        if (this.eventHandlers[eventName]) {
            delete this.eventHandlers[eventName];
            console.log(`[TradeUI] Handler removido para evento '${eventName}'`);
        }
    }
    
    /**
     * Dispara um evento da interface
     * @param {string} eventName Nome do evento
     * @param {object} data Dados do evento
     * @private
     */
    _triggerEvent(eventName, data) {
        if (this.eventHandlers[eventName]) {
            console.log(`[TradeUI] Disparando evento '${eventName}'`);
            this.eventHandlers[eventName](data);
        }
    }
    
    /**
     * Registra ouvintes de eventos de interface
     * @private
     */
    _registerEventListeners() {
        // Em uma implementação real, registraria ouvintes DOM
        // Aqui, simulamos alguns eventos principais
        
        console.log('[TradeUI] Ouvintes de eventos registrados');
    }
    
    /**
     * Carrega recursos visuais necessários
     * @private
     */
    _loadResources() {
        // Em uma implementação real, carregaria imagens, sons, etc.
        // Aqui apenas simulamos o comportamento
        
        console.log('[TradeUI] Recursos visuais carregados');
    }
    
    /**
     * Reproduz um som da interface
     * @param {string} soundId ID do som
     * @private
     */
    _playSound(soundId) {
        if (!this.config.sounds) return;
        
        // Em uma implementação real, reproduziria um som
        // Aqui apenas registramos que o som foi solicitado
        
        console.log(`[TradeUI] Som reproduzido: ${soundId}`);
    }
    
    /**
     * Exibe o progresso de conclusão da troca
     * @private
     */
    _showTradeCompletionProgress() {
        // Em uma implementação real, mostraria uma barra de progresso
        console.log('[TradeUI] Exibindo progresso de conclusão da troca...');
    }
    
    /**
     * Exibe o resultado da troca
     * @param {object} tradeWindow Dados da janela de troca
     * @param {string} resultType Tipo de resultado (success, cancelled, error)
     * @param {object} details Detalhes adicionais
     * @private
     */
    _showTradeResult(tradeWindow, resultType, details) {
        // Em uma implementação real, mostraria uma janela de resultado
        
        switch (resultType) {
            case 'success':
                console.log('[TradeUI] Troca concluída! Itens trocados:');
                if (tradeWindow.isLocal) {
                    console.log(`  Recebidos: ${tradeWindow.initiatorItems.length} itens e ${tradeWindow.initiatorCurrency} moedas`);
                    console.log(`  Enviados: ${tradeWindow.targetItems.length} itens e ${tradeWindow.targetCurrency} moedas`);
                } else {
                    console.log(`  Recebidos: ${tradeWindow.targetItems.length} itens e ${tradeWindow.targetCurrency} moedas`);
                    console.log(`  Enviados: ${tradeWindow.initiatorItems.length} itens e ${tradeWindow.initiatorCurrency} moedas`);
                }
                break;
                
            case 'cancelled':
                console.log(`[TradeUI] Troca cancelada: ${details.reason || 'Razão desconhecida'}`);
                break;
                
            case 'error':
                console.log(`[TradeUI] Erro na troca: ${details.error || 'Erro desconhecido'}`);
                break;
        }
    }
}

module.exports = TradeUI;