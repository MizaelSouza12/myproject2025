/**
 * Sistema de Trocas WYDBRASIL - Trade Validation
 * 
 * Responsável por validar todos os aspectos de uma troca
 * entre jogadores, incluindo segurança e regras de negócio.
 */

class TradeValidation {
    constructor() {
        // Configurações de validação
        this.config = {
            minTradingLevel: 15,
            maxItemsPerTrade: 12,
            maxCurrencyPerTrade: 1000000000, // 1 bilhão
            maxTradeDistance: 10, // Distância máxima para troca em unidades do jogo
            requiredTradeZone: null, // null = qualquer zona, ou array de IDs de zonas
            tradeBannedCheck: true, // Verificar se jogador está banido de troca
            tradeCooldown: 0, // Tempo de espera entre trocas (em ms)
            validateItemOwnership: true, // Verificar posse real do item
            allowLockedItems: false, // Permitir itens bloqueados para troca
            allowEquippedItems: false, // Permitir itens equipados
            whitelist: { // Tipos de itens permitidos
                itemTypes: null, // null = todos, ou array de tipos (ex: ['weapon', 'armor'])
                rarities: null // null = todos, ou array de raridades (ex: ['common', 'rare'])
            },
            blacklist: { // Tipos de itens bloqueados
                itemTypes: ['quest'], // Itens de missão não podem ser trocados
                rarities: ['mythic'] // Itens míticos não podem ser trocados
            }
        };
        
        // Histórico de validações recentes para prevenir spam
        this.recentValidations = new Map();
        
        console.log('[TradeValidation] Sistema de validação de trocas inicializado');
    }
    
    /**
     * Verifica se um jogador pode iniciar uma troca
     * @param {object} initiator Jogador iniciador
     * @param {object} target Jogador alvo
     * @returns {object} Resultado da validação
     */
    canInitiateTrade(initiator, target) {
        // Verificações para o iniciador
        if (!initiator) {
            return { 
                valid: false, 
                error: 'initiator_not_found',
                message: 'Jogador iniciador não encontrado.'
            };
        }
        
        // Verificações para o alvo
        if (!target) {
            return { 
                valid: false, 
                error: 'target_not_found',
                message: 'Jogador alvo não encontrado.'
            };
        }
        
        // Verificar nível mínimo para iniciador
        if (this.config.minTradingLevel > 0 && initiator.level < this.config.minTradingLevel) {
            return {
                valid: false,
                error: 'initiator_level_too_low',
                message: `Você precisa ser nível ${this.config.minTradingLevel} ou maior para iniciar trocas.`
            };
        }
        
        // Verificar nível mínimo para alvo
        if (this.config.minTradingLevel > 0 && target.level < this.config.minTradingLevel) {
            return {
                valid: false,
                error: 'target_level_too_low',
                message: `O jogador alvo precisa ser nível ${this.config.minTradingLevel} ou maior para participar de trocas.`
            };
        }
        
        // Verificar se o iniciador está banido de trocas
        if (this.config.tradeBannedCheck && initiator.tradeBanned) {
            return {
                valid: false,
                error: 'initiator_trade_banned',
                message: 'Você está impedido de realizar trocas no momento.'
            };
        }
        
        // Verificar se o alvo está banido de trocas
        if (this.config.tradeBannedCheck && target.tradeBanned) {
            return {
                valid: false,
                error: 'target_trade_banned',
                message: 'O jogador alvo está impedido de realizar trocas no momento.'
            };
        }
        
        // Verificar se os jogadores estão no mesmo mapa
        if (initiator.mapId !== target.mapId) {
            return {
                valid: false,
                error: 'different_maps',
                message: 'Vocês precisam estar no mesmo mapa para realizar trocas.'
            };
        }
        
        // Verificar se os jogadores estão em uma zona de troca válida
        if (this.config.requiredTradeZone !== null) {
            const isInitiatorInTradeZone = Array.isArray(this.config.requiredTradeZone) ? 
                this.config.requiredTradeZone.includes(initiator.mapId) : 
                initiator.mapId === this.config.requiredTradeZone;
                
            if (!isInitiatorInTradeZone) {
                return {
                    valid: false,
                    error: 'not_in_trade_zone',
                    message: 'Você precisa estar em uma zona de troca para trocar itens.'
                };
            }
        }
        
        // Verificar distância entre jogadores
        if (this.config.maxTradeDistance > 0 && initiator.position && target.position) {
            const distance = this._calculateDistance(initiator.position, target.position);
            
            if (distance > this.config.maxTradeDistance) {
                return {
                    valid: false,
                    error: 'too_far_apart',
                    message: 'Vocês precisam estar mais próximos para realizar trocas.'
                };
            }
        }
        
        // Verificar cooldown de troca
        if (this.config.tradeCooldown > 0) {
            const lastTrade = this.recentValidations.get(initiator.id);
            const now = Date.now();
            
            if (lastTrade && (now - lastTrade) < this.config.tradeCooldown) {
                const remainingTime = Math.ceil((this.config.tradeCooldown - (now - lastTrade)) / 1000);
                
                return {
                    valid: false,
                    error: 'trade_cooldown',
                    message: `Você precisa aguardar ${remainingTime} segundos para iniciar outra troca.`
                };
            }
            
            // Registrar tempo desta validação
            this.recentValidations.set(initiator.id, now);
        }
        
        // Verificar estado dos jogadores (não podem estar em combate, etc.)
        if (initiator.state === 'in_combat') {
            return {
                valid: false,
                error: 'initiator_in_combat',
                message: 'Você não pode trocar itens durante o combate.'
            };
        }
        
        if (target.state === 'in_combat') {
            return {
                valid: false,
                error: 'target_in_combat',
                message: 'O jogador alvo está em combate e não pode trocar itens.'
            };
        }
        
        // Todas as verificações passaram
        return {
            valid: true
        };
    }
    
    /**
     * Valida a inclusão de um item na troca
     * @param {object} trade Dados da troca
     * @param {string} playerId ID do jogador
     * @param {object} item Item a ser adicionado
     * @returns {object} Resultado da validação
     */
    validateAddItem(trade, playerId, item) {
        // Verificações básicas
        if (!trade || !playerId || !item) {
            return {
                valid: false,
                error: 'invalid_parameters',
                message: 'Parâmetros de validação inválidos.'
            };
        }
        
        // Verificar se o jogador está na troca
        const isInitiator = trade.initiatorId === playerId;
        const isTarget = trade.targetId === playerId;
        
        if (!isInitiator && !isTarget) {
            return {
                valid: false,
                error: 'player_not_in_trade',
                message: 'Você não está participando desta troca.'
            };
        }
        
        // Verificar se o jogador pode alterar a troca (não bloqueada/confirmada)
        if ((isInitiator && trade.initiatorLocked) || (isTarget && trade.targetLocked)) {
            return {
                valid: false,
                error: 'offer_locked',
                message: 'Você já bloqueou sua oferta e não pode alterá-la.'
            };
        }
        
        // Verificar limite de itens
        const itemsList = isInitiator ? trade.initiatorItems : trade.targetItems;
        
        if (itemsList.length >= this.config.maxItemsPerTrade) {
            return {
                valid: false,
                error: 'item_limit_reached',
                message: `Você só pode adicionar até ${this.config.maxItemsPerTrade} itens por troca.`
            };
        }
        
        // Verificar se o item já está na troca
        const existingItem = itemsList.find(i => i.id === item.id);
        if (existingItem) {
            return {
                valid: false,
                error: 'item_already_added',
                message: 'Este item já está incluído na troca.'
            };
        }
        
        // Verificar posse do item
        if (this.config.validateItemOwnership && item.ownerId !== playerId) {
            return {
                valid: false,
                error: 'not_item_owner',
                message: 'Você não possui este item.'
            };
        }
        
        // Verificar se o item está bloqueado para troca
        if (!this.config.allowLockedItems && item.tradeLocked) {
            return {
                valid: false,
                error: 'item_trade_locked',
                message: 'Este item está bloqueado para troca.'
            };
        }
        
        // Verificar se o item está equipado
        if (!this.config.allowEquippedItems && item.equipped) {
            return {
                valid: false,
                error: 'item_equipped',
                message: 'Você precisa desequipar o item antes de trocá-lo.'
            };
        }
        
        // Verificar lista branca de tipos de itens
        if (this.config.whitelist.itemTypes && this.config.whitelist.itemTypes.length > 0) {
            if (!this.config.whitelist.itemTypes.includes(item.type)) {
                return {
                    valid: false,
                    error: 'item_type_not_allowed',
                    message: `Itens do tipo ${item.type} não podem ser trocados.`
                };
            }
        }
        
        // Verificar lista negra de tipos de itens
        if (this.config.blacklist.itemTypes && this.config.blacklist.itemTypes.length > 0) {
            if (this.config.blacklist.itemTypes.includes(item.type)) {
                return {
                    valid: false,
                    error: 'item_type_blacklisted',
                    message: `Itens do tipo ${item.type} não podem ser trocados.`
                };
            }
        }
        
        // Verificar lista branca de raridades
        if (this.config.whitelist.rarities && this.config.whitelist.rarities.length > 0) {
            if (!this.config.whitelist.rarities.includes(item.rarity)) {
                return {
                    valid: false,
                    error: 'item_rarity_not_allowed',
                    message: `Itens de raridade ${item.rarity} não podem ser trocados.`
                };
            }
        }
        
        // Verificar lista negra de raridades
        if (this.config.blacklist.rarities && this.config.blacklist.rarities.length > 0) {
            if (this.config.blacklist.rarities.includes(item.rarity)) {
                return {
                    valid: false,
                    error: 'item_rarity_blacklisted',
                    message: `Itens de raridade ${item.rarity} não podem ser trocados.`
                };
            }
        }
        
        // Todas as verificações passaram
        return {
            valid: true
        };
    }
    
    /**
     * Valida a quantidade de moeda a ser incluída na troca
     * @param {object} trade Dados da troca
     * @param {string} playerId ID do jogador
     * @param {number} amount Quantidade de moeda
     * @returns {object} Resultado da validação
     */
    validateCurrency(trade, playerId, amount) {
        // Verificações básicas
        if (!trade || !playerId || amount === undefined) {
            return {
                valid: false,
                error: 'invalid_parameters',
                message: 'Parâmetros de validação inválidos.'
            };
        }
        
        // Verificar se o jogador está na troca
        const isInitiator = trade.initiatorId === playerId;
        const isTarget = trade.targetId === playerId;
        
        if (!isInitiator && !isTarget) {
            return {
                valid: false,
                error: 'player_not_in_trade',
                message: 'Você não está participando desta troca.'
            };
        }
        
        // Verificar se o jogador pode alterar a troca (não bloqueada/confirmada)
        if ((isInitiator && trade.initiatorLocked) || (isTarget && trade.targetLocked)) {
            return {
                valid: false,
                error: 'offer_locked',
                message: 'Você já bloqueou sua oferta e não pode alterá-la.'
            };
        }
        
        // Verificar se a quantidade é válida
        if (isNaN(amount) || amount < 0) {
            return {
                valid: false,
                error: 'invalid_currency_amount',
                message: 'A quantidade de moeda precisa ser um número positivo.'
            };
        }
        
        // Verificar limite máximo de moeda
        if (amount > this.config.maxCurrencyPerTrade) {
            return {
                valid: false,
                error: 'currency_limit_exceeded',
                message: `Você só pode trocar até ${this.config.maxCurrencyPerTrade} moedas por vez.`
            };
        }
        
        // Verificar se o jogador tem moeda suficiente
        const player = this._getPlayerById(playerId);
        
        if (player && player.currency < amount) {
            return {
                valid: false,
                error: 'insufficient_currency',
                message: 'Você não tem moeda suficiente para esta troca.'
            };
        }
        
        // Todas as verificações passaram
        return {
            valid: true
        };
    }
    
    /**
     * Valida se uma troca está pronta para ser completada
     * @param {object} trade Dados da troca
     * @returns {object} Resultado da validação
     */
    validateCompletion(trade) {
        // Verificações básicas
        if (!trade) {
            return {
                valid: false,
                error: 'invalid_trade',
                message: 'Dados da troca inválidos.'
            };
        }
        
        // Verificar se a troca está no estado correto
        if (trade.state !== 'active') {
            return {
                valid: false,
                error: 'invalid_trade_state',
                message: 'Esta troca não está ativa.'
            };
        }
        
        // Verificar se ambos jogadores confirmaram
        if (!trade.initiatorConfirmed || !trade.targetConfirmed) {
            return {
                valid: false,
                error: 'not_all_confirmed',
                message: 'Ambos os jogadores precisam confirmar a troca.'
            };
        }
        
        // Verificar se as ofertas estão bloqueadas
        if (!trade.initiatorLocked || !trade.targetLocked) {
            return {
                valid: false,
                error: 'not_all_locked',
                message: 'Ambas as ofertas precisam estar bloqueadas.'
            };
        }
        
        // Verificar se os jogadores ainda existem
        const initiator = this._getPlayerById(trade.initiatorId);
        const target = this._getPlayerById(trade.targetId);
        
        if (!initiator || !target) {
            return {
                valid: false,
                error: 'player_not_found',
                message: 'Um dos jogadores não está mais disponível.'
            };
        }
        
        // Verificar se os jogadores ainda possuem todos os itens oferecidos
        const initiatorItemsValid = this._validatePlayerStillHasItems(trade.initiatorId, trade.initiatorItems);
        if (!initiatorItemsValid.valid) {
            return initiatorItemsValid;
        }
        
        const targetItemsValid = this._validatePlayerStillHasItems(trade.targetId, trade.targetItems);
        if (!targetItemsValid.valid) {
            return targetItemsValid;
        }
        
        // Verificar se os jogadores têm espaço suficiente para receber os itens
        const initiatorSpaceValid = this._validateInventorySpace(
            trade.initiatorId, 
            trade.targetItems.length
        );
        if (!initiatorSpaceValid.valid) {
            return initiatorSpaceValid;
        }
        
        const targetSpaceValid = this._validateInventorySpace(
            trade.targetId, 
            trade.initiatorItems.length
        );
        if (!targetSpaceValid.valid) {
            return targetSpaceValid;
        }
        
        // Verificar se os jogadores têm moeda suficiente
        if (initiator.currency < trade.initiatorCurrency) {
            return {
                valid: false,
                error: 'initiator_insufficient_currency',
                message: 'O iniciador não tem moeda suficiente para completar a troca.'
            };
        }
        
        if (target.currency < trade.targetCurrency) {
            return {
                valid: false,
                error: 'target_insufficient_currency',
                message: 'O alvo não tem moeda suficiente para completar a troca.'
            };
        }
        
        // Todas as verificações passaram
        return {
            valid: true
        };
    }
    
    /**
     * Verifica se um jogador ainda possui todos os itens oferecidos
     * @param {string} playerId ID do jogador
     * @param {Array} items Lista de itens oferecidos
     * @returns {object} Resultado da validação
     * @private
     */
    _validatePlayerStillHasItems(playerId, items) {
        const player = this._getPlayerById(playerId);
        
        if (!player) {
            return {
                valid: false,
                error: 'player_not_found',
                message: 'Jogador não encontrado.'
            };
        }
        
        // Verificar cada item oferecido
        for (const item of items) {
            // Procurar o item no inventário do jogador
            const playerHasItem = player.inventory.some(
                invItem => invItem.id === item.id && invItem.ownerId === player.id
            );
            
            if (!playerHasItem) {
                return {
                    valid: false,
                    error: 'item_no_longer_owned',
                    message: `O item "${item.name}" não está mais disponível.`
                };
            }
        }
        
        return {
            valid: true
        };
    }
    
    /**
     * Verifica se um jogador tem espaço suficiente no inventário
     * @param {string} playerId ID do jogador
     * @param {number} itemCount Quantidade de itens a receber
     * @returns {object} Resultado da validação
     * @private
     */
    _validateInventorySpace(playerId, itemCount) {
        const player = this._getPlayerById(playerId);
        
        if (!player) {
            return {
                valid: false,
                error: 'player_not_found',
                message: 'Jogador não encontrado.'
            };
        }
        
        // Calcular slots livres no inventário
        const freeSlots = player.inventorySize - player.inventory.length;
        
        if (freeSlots < itemCount) {
            return {
                valid: false,
                error: 'insufficient_inventory_space',
                message: 'Espaço insuficiente no inventário para receber os itens.'
            };
        }
        
        return {
            valid: true
        };
    }
    
    /**
     * Calcula a distância entre dois pontos
     * @param {object} pos1 Posição 1 (x, y, z)
     * @param {object} pos2 Posição 2 (x, y, z)
     * @returns {number} Distância calculada
     * @private
     */
    _calculateDistance(pos1, pos2) {
        if (!pos1 || !pos2) return Infinity;
        
        const dx = (pos2.x || 0) - (pos1.x || 0);
        const dy = (pos2.y || 0) - (pos1.y || 0);
        const dz = (pos2.z || 0) - (pos1.z || 0);
        
        return Math.sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    /**
     * Obtém um jogador pelo ID
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

module.exports = TradeValidation;