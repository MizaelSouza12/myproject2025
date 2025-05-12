/**
 * Teste do Sistema de Trocas do WYDBRASIL
 * 
 * Execute este script para testar o Sistema de Trocas
 * e verificar suas funções principais.
 * 
 * node test-trading.js
 */

const TradingSystem = require('./index');

// Inicializar sistema de trocas
const tradingSystem = new TradingSystem({
    logEvents: true,
    uiEnabled: false, // Desativar UI para teste em console
    recordHistory: true
});

console.log('======== Sistema de Trocas WYDBRASIL ========');
console.log('      Demonstração de Simulação de Troca     ');
console.log('============================================');

// Criar jogadores simulados para teste
const players = {
    initiator: {
        id: 'player1',
        name: 'Guerreiro',
        isLocal: true,
        level: 30,
        state: 'idle',
        tradeBanned: false,
        mapId: 'valley_of_trials',
        position: { x: 100, y: 0, z: 100 },
        inventory: [
            { id: 'item1', name: 'Espada Longa +1', type: 'weapon', value: 500, ownerId: 'player1', tradeLocked: false, equipped: false, rarity: 'rare' },
            { id: 'item2', name: 'Poção de Cura Grande', type: 'consumable', value: 50, ownerId: 'player1', tradeLocked: false, equipped: false, rarity: 'common' },
            { id: 'item3', name: 'Elmo de Aço', type: 'armor', value: 300, ownerId: 'player1', tradeLocked: false, equipped: false, rarity: 'uncommon' },
            { id: 'item4', name: 'Amuleto do Dragão', type: 'accessory', value: 1200, ownerId: 'player1', tradeLocked: false, equipped: false, rarity: 'epic' },
            { id: 'item5', name: 'Pedra da Alma', type: 'material', value: 200, ownerId: 'player1', tradeLocked: false, equipped: false, rarity: 'uncommon' }
        ],
        inventorySize: 40,
        currency: 25000
    },
    target: {
        id: 'player2',
        name: 'Mago',
        isLocal: false,
        level: 28,
        state: 'idle',
        tradeBanned: false,
        mapId: 'valley_of_trials',
        position: { x: 105, y: 0, z: 98 },
        inventory: [
            { id: 'item6', name: 'Cajado Arcano', type: 'weapon', value: 650, ownerId: 'player2', tradeLocked: false, equipped: false, rarity: 'rare' },
            { id: 'item7', name: 'Tomo de Sabedoria', type: 'offhand', value: 300, ownerId: 'player2', tradeLocked: false, equipped: false, rarity: 'uncommon' },
            { id: 'item8', name: 'Manto Encantado', type: 'armor', value: 450, ownerId: 'player2', tradeLocked: false, equipped: false, rarity: 'rare' },
            { id: 'item9', name: 'Anel da Inteligência', type: 'accessory', value: 800, ownerId: 'player2', tradeLocked: true, equipped: false, rarity: 'rare' },
            { id: 'item10', name: 'Essência Mágica', type: 'material', value: 150, ownerId: 'player2', tradeLocked: false, equipped: false, rarity: 'common' }
        ],
        inventorySize: 40,
        currency: 18500
    },
    invalid: {
        id: 'player3',
        name: 'Novato',
        isLocal: false,
        level: 10, // Abaixo do nível mínimo para troca
        state: 'idle',
        tradeBanned: false,
        mapId: 'tutorial_area',
        position: { x: 500, y: 0, z: 500 },
        inventory: [],
        inventorySize: 20,
        currency: 500
    },
    banned: {
        id: 'player4',
        name: 'Trapaceiro',
        isLocal: false,
        level: 40,
        state: 'idle',
        tradeBanned: true, // Banido de troca
        mapId: 'valley_of_trials',
        position: { x: 105, y: 0, z: 105 },
        inventory: [],
        inventorySize: 40,
        currency: 50000
    }
};

// Sobrescrever função interna para testes
tradingSystem._getPlayerById = function(playerId) {
    if (playerId === 'player1') return players.initiator;
    if (playerId === 'player2') return players.target;
    if (playerId === 'player3') return players.invalid;
    if (playerId === 'player4') return players.banned;
    return null;
};

// Adicionar listeners para eventos
tradingSystem.on('tradeCompleted', (data) => {
    console.log(`[EVENTO] Troca concluída ID: ${data.tradeId}`);
});

tradingSystem.on('tradeCancelled', (data) => {
    console.log(`[EVENTO] Troca cancelada ID: ${data.tradeId}, Motivo: ${data.reason}`);
});

// Iniciar bateria de testes
async function runTradeTests() {
    console.log('\n1. Testando validação inicial de troca:');
    console.log('--------------------------------------');
    
    // Teste 1: Validar iniciação de troca
    console.log('1.1. Teste tentando iniciar troca com jogador de nível baixo:');
    let result = tradingSystem.initiateTradeRequest('player1', 'player3');
    console.log(`Resultado: ${result.success ? 'Sucesso' : 'Falha'} - ${result.error || ''}`);
    console.log(`Mensagem: ${result.message || 'Nenhuma'}`);
    
    console.log('\n1.2. Teste tentando iniciar troca com jogador banido:');
    result = tradingSystem.initiateTradeRequest('player1', 'player4');
    console.log(`Resultado: ${result.success ? 'Sucesso' : 'Falha'} - ${result.error || ''}`);
    console.log(`Mensagem: ${result.message || 'Nenhuma'}`);
    
    // Teste 2: Iniciar troca válida
    console.log('\n2. Iniciando troca válida entre jogadores:');
    console.log('------------------------------------------');
    const tradeResult = tradingSystem.initiateTradeRequest('player1', 'player2');
    console.log(`Resultado: ${tradeResult.success ? 'Sucesso' : 'Falha'} - ${tradeResult.error || ''}`);
    
    if (!tradeResult.success) {
        console.log('Falha ao iniciar troca. Abortando teste.');
        return;
    }
    
    const tradeId = tradeResult.tradeId;
    console.log(`Troca iniciada com ID: ${tradeId}`);
    
    console.log('\n3. Simulando aceitação da solicitação de troca:');
    console.log('----------------------------------------------');
    const acceptResult = tradingSystem.respondToTradeRequest(tradeId, 'player2', true);
    console.log(`Resultado: ${acceptResult.success ? 'Sucesso' : 'Falha'} - ${acceptResult.error || ''}`);
    
    console.log('\n4. Testes de adição/remoção de itens:');
    console.log('-------------------------------------');
    
    // Teste de adição de item
    console.log('4.1. Adicionando item de arma do Guerreiro à troca:');
    let addResult = tradingSystem.addItem(tradeId, 'player1', players.initiator.inventory[0]);
    console.log(`Resultado: ${addResult.success ? 'Sucesso' : 'Falha'} - ${addResult.error || ''}`);
    
    console.log('\n4.2. Adicionando item material do Mago à troca:');
    addResult = tradingSystem.addItem(tradeId, 'player2', players.target.inventory[4]);
    console.log(`Resultado: ${addResult.success ? 'Sucesso' : 'Falha'} - ${addResult.error || ''}`);
    
    console.log('\n4.3. Tentando adicionar item bloqueado do Mago:');
    addResult = tradingSystem.addItem(tradeId, 'player2', players.target.inventory[3]);
    console.log(`Resultado: ${addResult.success ? 'Sucesso' : 'Falha'} - ${addResult.error || ''}`);
    console.log(`Mensagem: ${addResult.message || 'Nenhuma'}`);
    
    // Teste de remoção de item
    console.log('\n4.4. Removendo item do Guerreiro:');
    let removeResult = tradingSystem.removeItem(tradeId, 'player1', 'item1');
    console.log(`Resultado: ${removeResult.success ? 'Sucesso' : 'Falha'} - ${removeResult.error || ''}`);
    
    // Readicionar item para continuar teste
    console.log('\n4.5. Adicionando outro item do Guerreiro:');
    addResult = tradingSystem.addItem(tradeId, 'player1', players.initiator.inventory[1]);
    console.log(`Resultado: ${addResult.success ? 'Sucesso' : 'Falha'} - ${addResult.error || ''}`);
    
    // Teste de manipulação de moeda
    console.log('\n5. Testes com moeda:');
    console.log('-------------------');
    
    console.log('5.1. Definindo 5000 moedas para o Guerreiro:');
    let currencyResult = tradingSystem.setCurrency(tradeId, 'player1', 5000);
    console.log(`Resultado: ${currencyResult.success ? 'Sucesso' : 'Falha'} - ${currencyResult.error || ''}`);
    
    console.log('\n5.2. Definindo 3000 moedas para o Mago:');
    currencyResult = tradingSystem.setCurrency(tradeId, 'player2', 3000);
    console.log(`Resultado: ${currencyResult.success ? 'Sucesso' : 'Falha'} - ${currencyResult.error || ''}`);
    
    console.log('\n5.3. Tentando definir quantidade inválida de moedas:');
    currencyResult = tradingSystem.setCurrency(tradeId, 'player1', -100);
    console.log(`Resultado: ${currencyResult.success ? 'Sucesso' : 'Falha'} - ${currencyResult.error || ''}`);
    
    // Teste de confirmação de troca
    console.log('\n6. Teste do processo de confirmação:');
    console.log('----------------------------------');
    
    console.log('6.1. Bloqueando oferta do Guerreiro:');
    let lockResult = tradingSystem.lockOffer(tradeId, 'player1');
    console.log(`Resultado: ${lockResult.success ? 'Sucesso' : 'Falha'} - ${lockResult.error || ''}`);
    
    console.log('\n6.2. Tentando alterar moeda após bloqueio:');
    currencyResult = tradingSystem.setCurrency(tradeId, 'player1', 6000);
    console.log(`Resultado: ${currencyResult.success ? 'Sucesso' : 'Falha'} - ${currencyResult.error || ''}`);
    
    console.log('\n6.3. Desbloqueando oferta do Guerreiro:');
    let unlockResult = tradingSystem.unlockOffer(tradeId, 'player1');
    console.log(`Resultado: ${unlockResult.success ? 'Sucesso' : 'Falha'} - ${unlockResult.error || ''}`);
    
    console.log('\n6.4. Alterando moeda após desbloqueio:');
    currencyResult = tradingSystem.setCurrency(tradeId, 'player1', 2500);
    console.log(`Resultado: ${currencyResult.success ? 'Sucesso' : 'Falha'} - ${currencyResult.error || ''}`);
    
    console.log('\n6.5. Bloqueando ofertas dos dois jogadores:');
    lockResult = tradingSystem.lockOffer(tradeId, 'player1');
    console.log(`Guerreiro bloqueado: ${lockResult.success ? 'Sim' : 'Não'}`);
    
    lockResult = tradingSystem.lockOffer(tradeId, 'player2');
    console.log(`Mago bloqueado: ${lockResult.success ? 'Sim' : 'Não'}`);
    
    // Completar troca
    console.log('\n7. Completando a troca:');
    console.log('----------------------');
    
    console.log('7.1. Confirmando ofertas:');
    let confirmResult = tradingSystem.confirmTrade(tradeId, 'player1');
    console.log(`Guerreiro confirmou: ${confirmResult.success ? 'Sim' : 'Não'}`);
    
    // Aguardar um momento para simular passagem de tempo
    await new Promise(resolve => setTimeout(resolve, 1000));
    
    confirmResult = tradingSystem.confirmTrade(tradeId, 'player2');
    console.log(`Mago confirmou: ${confirmResult.success ? 'Sim' : 'Não'}`);
    
    // Verificar que a troca foi completada automaticamente
    console.log('\n7.2. Verificando resultado final da troca:');
    
    // Obter estatísticas do histórico
    await new Promise(resolve => setTimeout(resolve, 1000));
    const stats = tradingSystem.getPlayerTradeStats('player1');
    
    if (stats.success) {
        console.log('\n8. Estatísticas do Guerreiro após a troca:');
        console.log('----------------------------------------');
        console.log(`Total de trocas: ${stats.stats.totalTrades}`);
        console.log(`Trocas concluídas: ${stats.stats.completedTrades}`);
        console.log(`Trocas canceladas: ${stats.stats.cancelledTrades}`);
        console.log(`Itens recebidos: ${stats.stats.itemsReceived}`);
        console.log(`Itens dados: ${stats.stats.itemsGiven}`);
        console.log(`Moeda recebida: ${stats.stats.currencyReceived}`);
        console.log(`Moeda gasta: ${stats.stats.currencyGiven}`);
    }
    
    // Teste de troca cancelada
    console.log('\n9. Teste de cancelamento de troca:');
    console.log('--------------------------------');
    
    console.log('9.1. Iniciando nova troca:');
    const newTradeResult = tradingSystem.initiateTradeRequest('player1', 'player2');
    if (newTradeResult.success) {
        const newTradeId = newTradeResult.tradeId;
        console.log(`Nova troca iniciada com ID: ${newTradeId}`);
        
        console.log('\n9.2. Aceitando nova solicitação:');
        tradingSystem.respondToTradeRequest(newTradeId, 'player2', true);
        
        console.log('\n9.3. Adicionar alguns itens:');
        tradingSystem.addItem(newTradeId, 'player1', players.initiator.inventory[2]);
        tradingSystem.addItem(newTradeId, 'player2', players.target.inventory[1]);
        
        console.log('\n9.4. Cancelando troca:');
        const cancelResult = tradingSystem.cancelTrade(newTradeId, 'player1', 'user_decided_to_cancel');
        console.log(`Resultado: ${cancelResult.success ? 'Sucesso' : 'Falha'} - ${cancelResult.error || ''}`);
    }
    
    // Verificar estado atual do jogador
    console.log('\n10. Verificando estado atual do jogador:');
    console.log('--------------------------------------');
    const playerStatus = tradingSystem.isPlayerTrading('player1');
    console.log(`O Guerreiro está negociando? ${playerStatus.isTrading ? 'Sim' : 'Não'}`);
    
    console.log('\n======== Demonstração de Troca Concluída ========');
}

runTradeTests().catch(err => {
    console.error('Erro durante os testes:', err);
});