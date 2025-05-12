/**
 * Sistema de Trocas WYDBRASIL - Trade History
 * 
 * Gerencia o histórico de trocas realizadas, permitindo
 * auditoria, investigação e geração de relatórios.
 */

class TradeHistory {
    constructor() {
        // Armazenar histórico em memória (em produção seria um banco de dados)
        this.tradeRecords = [];
        this.playerTradeIndex = new Map(); // Mapa de jogador -> array de IDs de trocas
        
        // Configurações
        this.maxInMemoryRecords = 1000; // Número máximo de registros mantidos em memória
        
        console.log('[TradeHistory] Sistema de histórico de trocas inicializado');
    }
    
    /**
     * Registra uma troca completa no histórico
     * @param {object} trade Dados da troca concluída
     * @returns {object} ID do registro e status
     */
    recordCompletedTrade(trade) {
        if (!trade || trade.state !== 'completed') {
            return { success: false, error: 'invalid_trade' };
        }
        
        // Criar registro com timestamp atual
        const record = {
            id: `trade_record_${Date.now()}_${Math.floor(Math.random() * 1000)}`,
            tradeId: trade.id,
            initiatorId: trade.initiatorId,
            targetId: trade.targetId,
            initiatorItems: [...trade.initiatorItems],
            targetItems: [...trade.targetItems],
            initiatorCurrency: trade.initiatorCurrency,
            targetCurrency: trade.targetCurrency,
            createdAt: trade.createdAt,
            completedAt: trade.completedAt || Date.now(),
            recordedAt: Date.now()
        };
        
        // Adicionar ao histórico
        this.tradeRecords.push(record);
        
        // Indexar por jogador
        this._indexTradeForPlayer(trade.initiatorId, record.id);
        this._indexTradeForPlayer(trade.targetId, record.id);
        
        // Limitar tamanho do histórico em memória
        if (this.tradeRecords.length > this.maxInMemoryRecords) {
            // Remove os registros mais antigos
            const removed = this.tradeRecords.splice(0, this.tradeRecords.length - this.maxInMemoryRecords);
            
            // Atualizar índices para os registros removidos
            for (const oldRecord of removed) {
                this._removeTradeFromIndex(oldRecord.initiatorId, oldRecord.id);
                this._removeTradeFromIndex(oldRecord.targetId, oldRecord.id);
            }
            
            console.log(`[TradeHistory] Removidos ${removed.length} registros antigos do histórico em memória`);
        }
        
        console.log(`[TradeHistory] Troca registrada: ${record.id} (originada de ${trade.id})`);
        
        return { success: true, recordId: record.id };
    }
    
    /**
     * Registra uma troca cancelada no histórico
     * @param {object} trade Dados da troca cancelada
     * @param {string} reason Motivo do cancelamento
     * @returns {object} ID do registro e status
     */
    recordCancelledTrade(trade, reason = 'unknown') {
        if (!trade) {
            return { success: false, error: 'invalid_trade' };
        }
        
        // Criar registro com timestamp atual
        const record = {
            id: `cancel_record_${Date.now()}_${Math.floor(Math.random() * 1000)}`,
            tradeId: trade.id,
            initiatorId: trade.initiatorId,
            targetId: trade.targetId,
            initiatorItems: [...(trade.initiatorItems || [])],
            targetItems: [...(trade.targetItems || [])],
            initiatorCurrency: trade.initiatorCurrency || 0,
            targetCurrency: trade.targetCurrency || 0,
            state: 'cancelled',
            reason: reason,
            createdAt: trade.createdAt,
            cancelledAt: trade.updatedAt || Date.now(),
            recordedAt: Date.now()
        };
        
        // Adicionar ao histórico
        this.tradeRecords.push(record);
        
        // Indexar por jogador
        this._indexTradeForPlayer(trade.initiatorId, record.id);
        this._indexTradeForPlayer(trade.targetId, record.id);
        
        console.log(`[TradeHistory] Cancelamento registrado: ${record.id} (troca ${trade.id}), motivo: ${reason}`);
        
        return { success: true, recordId: record.id };
    }
    
    /**
     * Obtém o histórico de trocas de um jogador
     * @param {string} playerId ID do jogador
     * @param {object} options Opções de filtragem (limit, offset, sort, etc.)
     * @returns {object} Lista de registros e metadata
     */
    getPlayerTradeHistory(playerId, options = {}) {
        if (!playerId) {
            return { success: false, error: 'invalid_player' };
        }
        
        // Configurar opções de filtragem
        const limit = options.limit || 50;
        const offset = options.offset || 0;
        const sort = options.sort || 'newest'; // 'newest' ou 'oldest'
        
        // Obter IDs de trocas do jogador
        const tradeIds = this.playerTradeIndex.get(playerId) || [];
        
        // Filtrar registros
        let records = tradeIds
            .map(id => this.tradeRecords.find(record => record.id === id))
            .filter(record => record !== undefined);
        
        // Ordenar
        records = records.sort((a, b) => {
            const timeA = a.completedAt || a.cancelledAt || a.recordedAt;
            const timeB = b.completedAt || b.cancelledAt || b.recordedAt;
            
            return sort === 'newest' ? timeB - timeA : timeA - timeB;
        });
        
        // Aplicar paginação
        const paginatedRecords = records.slice(offset, offset + limit);
        
        return {
            success: true,
            records: paginatedRecords,
            pagination: {
                total: records.length,
                limit,
                offset,
                hasMore: offset + limit < records.length
            }
        };
    }
    
    /**
     * Obtém detalhes de um registro específico
     * @param {string} recordId ID do registro
     * @returns {object} Detalhes do registro
     */
    getTradeDetails(recordId) {
        if (!recordId) {
            return { success: false, error: 'invalid_record_id' };
        }
        
        const record = this.tradeRecords.find(r => r.id === recordId);
        
        if (!record) {
            return { success: false, error: 'record_not_found' };
        }
        
        return {
            success: true,
            record
        };
    }
    
    /**
     * Gera estatísticas de trocas para um jogador
     * @param {string} playerId ID do jogador
     * @returns {object} Estatísticas de trocas
     */
    getPlayerTradeStats(playerId) {
        if (!playerId) {
            return { success: false, error: 'invalid_player' };
        }
        
        // Obter registros de trocas do jogador
        const tradeIds = this.playerTradeIndex.get(playerId) || [];
        const records = tradeIds
            .map(id => this.tradeRecords.find(record => record.id === id))
            .filter(record => record !== undefined);
        
        // Inicializar estatísticas
        const stats = {
            totalTrades: records.length,
            completedTrades: 0,
            cancelledTrades: 0,
            itemsReceived: 0,
            itemsGiven: 0,
            currencyReceived: 0,
            currencyGiven: 0,
            mostTradedWithPlayers: [],
            tradingPartners: new Map(),
            recentTrades: []
        };
        
        // Calcular estatísticas
        for (const record of records) {
            // Determinar se o jogador é iniciador ou alvo
            const isInitiator = record.initiatorId === playerId;
            const partnerId = isInitiator ? record.targetId : record.initiatorId;
            
            // Contabilizar trocas
            if (record.state === 'cancelled') {
                stats.cancelledTrades++;
            } else {
                stats.completedTrades++;
                
                // Contabilizar itens e moeda
                if (isInitiator) {
                    stats.itemsGiven += record.initiatorItems.length;
                    stats.itemsReceived += record.targetItems.length;
                    stats.currencyGiven += record.initiatorCurrency;
                    stats.currencyReceived += record.targetCurrency;
                } else {
                    stats.itemsGiven += record.targetItems.length;
                    stats.itemsReceived += record.initiatorItems.length;
                    stats.currencyGiven += record.targetCurrency;
                    stats.currencyReceived += record.initiatorCurrency;
                }
            }
            
            // Adicionar parceiro de troca
            if (!stats.tradingPartners.has(partnerId)) {
                stats.tradingPartners.set(partnerId, 0);
            }
            stats.tradingPartners.set(partnerId, stats.tradingPartners.get(partnerId) + 1);
            
            // Adicionar às trocas recentes
            if (stats.recentTrades.length < 5) {
                stats.recentTrades.push({
                    recordId: record.id,
                    tradeId: record.tradeId,
                    partnerId,
                    date: record.completedAt || record.cancelledAt || record.recordedAt,
                    status: record.state === 'cancelled' ? 'cancelled' : 'completed'
                });
            }
        }
        
        // Ordenar parceiros de troca por quantidade e obter os mais frequentes
        const sortedPartners = [...stats.tradingPartners.entries()]
            .sort((a, b) => b[1] - a[1])
            .slice(0, 5)
            .map(([id, count]) => ({ id, count }));
        
        stats.mostTradedWithPlayers = sortedPartners;
        delete stats.tradingPartners; // Remover o Map temporário
        
        return {
            success: true,
            stats
        };
    }
    
    /**
     * Busca registros no histórico
     * @param {object} filters Critérios de busca
     * @param {object} options Opções de filtragem (limit, offset, sort, etc.)
     * @returns {object} Lista de registros e metadata
     */
    searchTrades(filters = {}, options = {}) {
        // Configurar opções de filtragem
        const limit = options.limit || 50;
        const offset = options.offset || 0;
        const sort = options.sort || 'newest'; // 'newest' ou 'oldest'
        
        // Filtrar registros
        let records = [...this.tradeRecords];
        
        // Aplicar filtros
        if (filters.playerId) {
            records = records.filter(r => 
                r.initiatorId === filters.playerId || r.targetId === filters.playerId
            );
        }
        
        if (filters.startDate) {
            const startTimestamp = new Date(filters.startDate).getTime();
            records = records.filter(r => 
                (r.completedAt || r.cancelledAt || r.recordedAt) >= startTimestamp
            );
        }
        
        if (filters.endDate) {
            const endTimestamp = new Date(filters.endDate).getTime();
            records = records.filter(r => 
                (r.completedAt || r.cancelledAt || r.recordedAt) <= endTimestamp
            );
        }
        
        if (filters.state) {
            records = records.filter(r => r.state === filters.state);
        }
        
        if (filters.minCurrency) {
            records = records.filter(r => 
                r.initiatorCurrency >= filters.minCurrency || r.targetCurrency >= filters.minCurrency
            );
        }
        
        if (filters.itemId) {
            records = records.filter(r => {
                const initiatorHasItem = r.initiatorItems.some(item => item.id === filters.itemId);
                const targetHasItem = r.targetItems.some(item => item.id === filters.itemId);
                return initiatorHasItem || targetHasItem;
            });
        }
        
        // Ordenar
        records = records.sort((a, b) => {
            const timeA = a.completedAt || a.cancelledAt || a.recordedAt;
            const timeB = b.completedAt || b.cancelledAt || b.recordedAt;
            
            return sort === 'newest' ? timeB - timeA : timeA - timeB;
        });
        
        // Aplicar paginação
        const paginatedRecords = records.slice(offset, offset + limit);
        
        return {
            success: true,
            records: paginatedRecords,
            pagination: {
                total: records.length,
                limit,
                offset,
                hasMore: offset + limit < records.length
            }
        };
    }
    
    /**
     * Gera relatório de trocas por período
     * @param {Date} startDate Data de início
     * @param {Date} endDate Data de fim
     * @returns {object} Relatório de trocas
     */
    generateTradeReport(startDate, endDate) {
        // Converter para timestamps
        const startTimestamp = startDate ? new Date(startDate).getTime() : 0;
        const endTimestamp = endDate ? new Date(endDate).getTime() : Date.now();
        
        // Filtrar registros pelo período
        const records = this.tradeRecords.filter(r => {
            const timestamp = r.completedAt || r.cancelledAt || r.recordedAt;
            return timestamp >= startTimestamp && timestamp <= endTimestamp;
        });
        
        // Inicializar estatísticas
        const report = {
            period: {
                startDate: new Date(startTimestamp),
                endDate: new Date(endTimestamp)
            },
            tradeCounts: {
                total: records.length,
                completed: 0,
                cancelled: 0
            },
            currency: {
                total: 0,
                average: 0,
                max: 0
            },
            items: {
                total: 0,
                average: 0,
                topItemTypes: []
            },
            players: {
                unique: new Set(),
                mostActive: []
            },
            itemTypeCount: new Map()
        };
        
        // Mapear atividade dos jogadores
        const playerActivity = new Map();
        
        // Calcular estatísticas
        for (const record of records) {
            // Contabilizar trocas
            if (record.state === 'cancelled') {
                report.tradeCounts.cancelled++;
            } else {
                report.tradeCounts.completed++;
                
                // Contabilizar moeda
                const totalCurrency = record.initiatorCurrency + record.targetCurrency;
                report.currency.total += totalCurrency;
                report.currency.max = Math.max(report.currency.max, totalCurrency);
                
                // Contabilizar itens
                const allItems = [...record.initiatorItems, ...record.targetItems];
                report.items.total += allItems.length;
                
                // Contabilizar tipos de itens
                allItems.forEach(item => {
                    const type = item.type || 'unknown';
                    if (!report.itemTypeCount.has(type)) {
                        report.itemTypeCount.set(type, 0);
                    }
                    report.itemTypeCount.set(type, report.itemTypeCount.get(type) + 1);
                });
            }
            
            // Contabilizar jogadores
            report.players.unique.add(record.initiatorId);
            report.players.unique.add(record.targetId);
            
            // Atualizar atividade dos jogadores
            [record.initiatorId, record.targetId].forEach(playerId => {
                if (!playerActivity.has(playerId)) {
                    playerActivity.set(playerId, 0);
                }
                playerActivity.set(playerId, playerActivity.get(playerId) + 1);
            });
        }
        
        // Calcular médias
        report.currency.average = report.tradeCounts.completed > 0 
            ? Math.round(report.currency.total / report.tradeCounts.completed) 
            : 0;
            
        report.items.average = report.tradeCounts.completed > 0 
            ? Math.round(report.items.total / report.tradeCounts.completed) 
            : 0;
        
        // Obter tipos de itens mais populares
        report.items.topItemTypes = [...report.itemTypeCount.entries()]
            .sort((a, b) => b[1] - a[1])
            .slice(0, 5)
            .map(([type, count]) => ({ type, count }));
        
        // Obter jogadores mais ativos
        report.players.mostActive = [...playerActivity.entries()]
            .sort((a, b) => b[1] - a[1])
            .slice(0, 5)
            .map(([playerId, count]) => ({ playerId, count }));
        
        // Converter Set para número
        report.players.unique = report.players.unique.size;
        
        // Remover Map temporário
        delete report.itemTypeCount;
        
        return {
            success: true,
            report
        };
    }
    
    /**
     * Limpa registros antigos ou específicos
     * @param {object} options Opções de limpeza (olderThan, recordIds, etc.)
     * @returns {object} Status e contagem de registros removidos
     */
    cleanupRecords(options = {}) {
        let removed = 0;
        
        // Limpar registros específicos
        if (options.recordIds && Array.isArray(options.recordIds)) {
            for (const recordId of options.recordIds) {
                const index = this.tradeRecords.findIndex(r => r.id === recordId);
                
                if (index !== -1) {
                    const record = this.tradeRecords[index];
                    this._removeTradeFromIndex(record.initiatorId, recordId);
                    this._removeTradeFromIndex(record.targetId, recordId);
                    this.tradeRecords.splice(index, 1);
                    removed++;
                }
            }
        }
        
        // Limpar registros mais antigos que a data especificada
        if (options.olderThan) {
            const cutoffTimestamp = new Date(options.olderThan).getTime();
            
            const oldRecords = this.tradeRecords.filter(record => {
                const timestamp = record.completedAt || record.cancelledAt || record.recordedAt;
                return timestamp < cutoffTimestamp;
            });
            
            for (const record of oldRecords) {
                const index = this.tradeRecords.indexOf(record);
                
                if (index !== -1) {
                    this._removeTradeFromIndex(record.initiatorId, record.id);
                    this._removeTradeFromIndex(record.targetId, record.id);
                    this.tradeRecords.splice(index, 1);
                    removed++;
                }
            }
        }
        
        console.log(`[TradeHistory] Limpeza executada: ${removed} registros removidos`);
        
        return {
            success: true,
            removed
        };
    }
    
    /**
     * Adiciona uma troca ao índice de um jogador
     * @param {string} playerId ID do jogador
     * @param {string} recordId ID do registro
     * @private
     */
    _indexTradeForPlayer(playerId, recordId) {
        if (!this.playerTradeIndex.has(playerId)) {
            this.playerTradeIndex.set(playerId, []);
        }
        
        this.playerTradeIndex.get(playerId).push(recordId);
    }
    
    /**
     * Remove uma troca do índice de um jogador
     * @param {string} playerId ID do jogador
     * @param {string} recordId ID do registro
     * @private
     */
    _removeTradeFromIndex(playerId, recordId) {
        if (!this.playerTradeIndex.has(playerId)) {
            return;
        }
        
        const trades = this.playerTradeIndex.get(playerId);
        const index = trades.indexOf(recordId);
        
        if (index !== -1) {
            trades.splice(index, 1);
            
            // Se não houver mais trocas, remover o jogador do índice
            if (trades.length === 0) {
                this.playerTradeIndex.delete(playerId);
            }
        }
    }
}

module.exports = TradeHistory;