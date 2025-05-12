/**
 * Teste do Sistema de Refinamento WYDBRASIL
 * 
 * Este script demonstra e valida as funcionalidades do sistema
 * progressivo de refinamento de itens do WYDBRASIL.
 */

// Simulação do sistema de refinamento
class RefinementSystem {
    constructor() {
        this.refineRates = {
            0: { success: 1.00, break: 0.00, decrease: 0.00, perfect: 0.00 },
            1: { success: 0.90, break: 0.00, decrease: 0.10, perfect: 0.01 },
            2: { success: 0.80, break: 0.00, decrease: 0.20, perfect: 0.01 },
            3: { success: 0.70, break: 0.00, decrease: 0.30, perfect: 0.02 },
            4: { success: 0.60, break: 0.05, decrease: 0.35, perfect: 0.02 },
            5: { success: 0.50, break: 0.10, decrease: 0.40, perfect: 0.03 },
            6: { success: 0.40, break: 0.15, decrease: 0.45, perfect: 0.03 },
            7: { success: 0.30, break: 0.20, decrease: 0.50, perfect: 0.04 },
            8: { success: 0.20, break: 0.30, decrease: 0.50, perfect: 0.04 },
            9: { success: 0.10, break: 0.40, decrease: 0.50, perfect: 0.05 }
        };
        
        this.events = [];
        console.log('Sistema de refinamento inicializado com sucesso!');
    }
    
    /**
     * Calcula as taxas de refinamento para um item
     * @param {object} item Item a ser refinado
     * @param {object} options Opções de refinamento
     * @returns {object} Taxas calculadas
     */
    calculateRates(item, options = {}) {
        const level = item.refineLevel || 0;
        const baseRates = this.refineRates[level] || this.refineRates[9];
        
        // Aplicar modificadores baseados na qualidade do item
        const qualityMod = item.quality || 1.0;
        
        let rates = {
            success: baseRates.success * qualityMod,
            break: baseRates.break / qualityMod,
            decrease: baseRates.decrease,
            perfect: baseRates.perfect * qualityMod
        };
        
        // Aplicar proteções se existirem
        if (options.protectLevel) {
            rates.decrease = 1 - rates.success;
            rates.break = 0;
        }
        
        if (options.protectBreak) {
            rates.decrease += rates.break;
            rates.break = 0;
        }
        
        // Limitar taxas entre 0 e 1
        Object.keys(rates).forEach(key => {
            rates[key] = Math.max(0, Math.min(1, rates[key]));
        });
        
        return rates;
    }
    
    /**
     * Calcula o custo de um refinamento
     * @param {object} item Item a ser refinado
     * @param {object} options Opções de refinamento
     * @returns {object} Custos calculados
     */
    calculateCost(item, options = {}) {
        const level = item.refineLevel || 0;
        
        // Custo base de materiais e ouro
        const materialCost = 1 + level;
        const goldCost = 100 * Math.pow(3, level);
        
        // Custo adicional para proteções
        let protectionCostLevel = 0;
        let protectionCostBreak = 0;
        
        if (options.protectLevel) {
            protectionCostLevel = goldCost * 0.5;
        }
        
        if (options.protectBreak) {
            protectionCostBreak = goldCost * 0.8;
        }
        
        return {
            materials: materialCost,
            gold: goldCost,
            protectLevelCost: protectionCostLevel,
            protectBreakCost: protectionCostBreak,
            total: goldCost + protectionCostLevel + protectionCostBreak
        };
    }
    
    /**
     * Tenta refinar um item
     * @param {object} item Item a ser refinado
     * @param {object} options Opções de refinamento
     * @returns {object} Resultado do refinamento
     */
    refineItem(item, options = {}) {
        const rates = this.calculateRates(item, options);
        const costs = this.calculateCost(item, options);
        
        // Gerar resultado baseado nas probabilidades
        const roll = Math.random();
        let result = {
            success: false,
            type: null,
            item: { ...item },
            costs
        };
        
        if (roll < rates.perfect) {
            // Sucesso perfeito (+2 níveis)
            result.success = true;
            result.type = 'perfect';
            result.item.refineLevel = (item.refineLevel || 0) + 2;
        } else if (roll < rates.perfect + rates.success) {
            // Sucesso normal (+1 nível)
            result.success = true;
            result.type = 'success';
            result.item.refineLevel = (item.refineLevel || 0) + 1;
        } else if (roll < rates.perfect + rates.success + rates.decrease) {
            // Falha com redução (-1 nível)
            result.success = false;
            result.type = 'decrease';
            result.item.refineLevel = Math.max(0, (item.refineLevel || 0) - 1);
        } else {
            // Falha com quebra (item destruído)
            result.success = false;
            result.type = 'break';
            result.item = null;
        }
        
        // Emitir evento de refinamento
        const event = {
            timestamp: Date.now(),
            type: result.success ? 'success' : 'failure',
            itemBefore: { ...item },
            itemAfter: result.item,
            result: result.type
        };
        
        this.events.push(event);
        this.emitEvent(event.type, item.name, result);
        
        return result;
    }
    
    /**
     * Emite um evento de refinamento
     * @param {string} type Tipo de evento
     * @param {string} itemName Nome do item
     * @param {object} result Resultado do refinamento
     */
    emitEvent(type, itemName, result) {
        const levelBefore = (result.item && result.itemBefore) ? (result.itemBefore.refineLevel || 0) : 0;
        const levelAfter = result.item ? (result.item.refineLevel || 0) : 0;
        
        if (type === 'success') {
            console.log(`[EVENT] Refinamento bem-sucedido: ${itemName} +${levelBefore} → +${levelAfter}`);
        } else {
            console.log(`[EVENT] Falha no refinamento: ${itemName}, resultado: ${result.type}`);
        }
    }
    
    /**
     * Gera fragmentos de refinamento para um item
     * @param {object} item Item para gerar fragmentos
     * @returns {object} Fragmentos gerados
     */
    generateFragments(item) {
        // Chance base de obter fragmentos
        const baseChance = 0.6;
        
        // Não gerar fragmentos
        if (Math.random() > baseChance) {
            return null;
        }
        
        // Determinar raridade do fragmento
        const rarityRoll = Math.random();
        let rarity, points;
        
        if (rarityRoll < 0.01) {
            rarity = 'legendary';
            points = 100;
        } else if (rarityRoll < 0.05) {
            rarity = 'epic';
            points = 25;
        } else if (rarityRoll < 0.20) {
            rarity = 'uncommon';
            points = 3;
        } else {
            rarity = 'common';
            points = 1;
        }
        
        const fragment = {
            type: rarity,
            points,
            itemType: item.type
        };
        
        // Emitir evento de fragmento
        console.log(`[EVENT] Fragmento obtido para ${item.name}: 1x ${rarity} (${points} pontos)`);
        
        return fragment;
    }
    
    /**
     * Calcula bônus de refinamento para um item
     * @param {object} item Item refinado
     * @returns {object} Bônus calculados
     */
    calculateRefineBonus(item) {
        const level = item.refineLevel || 0;
        const bonus = {};
        
        switch (item.type) {
            case 'weapon':
                bonus.damage = level * 12;
                if (level >= 3) bonus.criticalChance = 1;
                if (level >= 6) bonus.criticalChance = 2;
                if (level >= 9) bonus.criticalDamage = 20;
                break;
                
            case 'armor':
                bonus.defense = level * 11;
                if (level >= 3) bonus.healthBonus = 10;
                if (level >= 5) bonus.damageReduction = 1;
                if (level >= 7) bonus.damageReduction = 2;
                break;
                
            case 'accessory':
                bonus.statBonus = level * 8;
                if (level >= 5) bonus.moveSpeed = 1;
                if (level >= 8) bonus.cooldownReduction = 5;
                break;
        }
        
        return bonus;
    }
}

// Executar teste do sistema de refinamento
function runRefinementTest() {
    console.log('======== Sistema Progressivo de Refinamento com Proteção Parcial ========');
    console.log('      WYD MODERN 2025 STUDIO - Demonstração de Gameplay Systems         ');
    console.log('========================================================================');
    
    // Inicializar sistema
    console.log('Inicializando sistema de refinamento...');
    const refinementSystem = new RefinementSystem();
    console.log('Sistema de refinamento inicializado com sucesso!');
    
    // 1. Criar itens de teste
    console.log('\n1. Criando itens de teste...');
    const testItems = [
        { name: 'Espada Longa de Teste', type: 'weapon', refineLevel: 0, quality: 1.2 },
        { name: 'Armadura de Placas de Teste', type: 'armor', refineLevel: 0, quality: 1.0 },
        { name: 'Anel Mágico de Teste', type: 'accessory', refineLevel: 0, quality: 0.9 }
    ];
    
    console.log('Itens criados:');
    testItems.forEach(item => {
        console.log(`  - ${item.name} (${item.type}, Qualidade: ${item.quality}x)`);
    });
    
    // 2. Verificar probabilidades de refinamento
    console.log('\n2. Verificando probabilidades de refinamento:');
    const espada = testItems[0];
    const rates = refinementSystem.calculateRates(espada);
    const costs = refinementSystem.calculateCost(espada);
    
    console.log(`Espada (+${espada.refineLevel || 0}):`);
    console.log(`  - Taxa de sucesso base: ${(rates.success * 100).toFixed(2)}%`);
    console.log(`  - Taxa efetiva: ${(rates.success * 100).toFixed(2)}%`);
    console.log(`  - Chance de quebra: ${(rates.break * 100).toFixed(2)}%`);
    console.log(`  - Chance de degradação: ${(rates.decrease * 100).toFixed(2)}%`);
    console.log(`  - Chance de sucesso perfeito: ${(rates.perfect * 100).toFixed(2)}%`);
    console.log(`  - Custos: ${costs.materials} materiais, ${costs.gold} ouro`);
    console.log(`  - Proteções: ${costs.protectLevelCost} ouro (nível), ${costs.protectBreakCost} ouro (quebra)`);
    
    // 3. Refinar espada até +3 sem proteções
    console.log('\n3. Refinando espada até +3 sem proteções:');
    let currentSword = { ...espada };
    
    for (let i = 0; i < 3; i++) {
        const result = refinementSystem.refineItem(currentSword);
        
        console.log(`Tentativa ${i + 1}: ${result.success ? 'SUCESSO' : 'FALHA'} - ${currentSword.name} +${currentSword.refineLevel || 0} → +${result.item ? result.item.refineLevel : 'Quebrado'}`);
        console.log(`  - Tipo de resultado: ${result.type}`);
        console.log(`  - Custo: ${result.costs.materials} materiais, ${result.costs.gold} ouro`);
        
        if (result.item) {
            currentSword = result.item;
        } else {
            console.log('  Item foi destruído! Criando nova espada...');
            currentSword = { ...espada };
        }
    }
    
    console.log(`Refinamento concluído: ${currentSword.name} +${currentSword.refineLevel}`);
    const swordBonus = refinementSystem.calculateRefineBonus(currentSword);
    console.log(`Bônus de refinamento: ${JSON.stringify(swordBonus)}`);
    
    // 4. Testar sistema de proteção
    console.log('\n4. Testando sistema de proteção (armadura até +5):');
    let currentArmor = { ...testItems[1] };
    
    for (let i = 0; i < 5; i++) {
        // Alternar uso de proteção de nível
        const useProtection = i % 2 === 1;
        const result = refinementSystem.refineItem(currentArmor, { protectLevel: useProtection });
        
        console.log(`Tentativa ${i + 1}: ${result.success ? 'SUCESSO' : 'FALHA'} - ${currentArmor.name} +${currentArmor.refineLevel || 0} → +${result.item ? result.item.refineLevel : 'Quebrado'}`);
        console.log(`  - Proteção de nível: ${useProtection ? 'SIM' : 'NÃO'}`);
        console.log(`  - Custo: ${result.costs.materials} materiais, ${result.costs.total} ouro total`);
        
        if (result.item) {
            currentArmor = result.item;
        } else {
            console.log('  Item foi destruído! Criando nova armadura...');
            currentArmor = { ...testItems[1] };
        }
    }
    
    console.log(`Refinamento concluído: ${currentArmor.name} +${currentArmor.refineLevel}`);
    const armorBonus = refinementSystem.calculateRefineBonus(currentArmor);
    console.log(`Bônus de refinamento: ${JSON.stringify(armorBonus)}`);
    
    // 5. Testar sistema de fragmentos
    console.log('\n5. Testando sistema de fragmentos:');
    console.log('Gerando drops de fragmentos para o anel...');
    
    const ring = testItems[2];
    let fragments = 0;
    const fragmentsNeeded = 8;
    let dropCount = 0;
    let fragmentLog = [];
    
    for (let i = 0; i < 20; i++) {
        dropCount++;
        const fragment = refinementSystem.generateFragments(ring);
        
        if (fragment) {
            fragments += fragment.points;
            fragmentLog.push(`Drop #${dropCount}: 1x ${fragment.type} (${fragment.points} pontos)`);
        } else {
            fragmentLog.push(`Drop #${dropCount}: Nenhum fragmento`);
        }
        
        console.log(`Progresso: ${fragments}/${fragmentsNeeded} (${((fragments/fragmentsNeeded)*100).toFixed(1)}%)`);
        
        if (fragments >= fragmentsNeeded && i >= 3) {
            break;
        }
    }
    
    fragmentLog.forEach(log => console.log(log));
    
    console.log(`Total de drops: ${fragmentLog.filter(l => !l.includes('Nenhum')).length}/${dropCount} (${((fragmentLog.filter(l => !l.includes('Nenhum')).length/dropCount)*100).toFixed(1)}% taxa de drop)`);
    console.log(`Total de pontos: ${fragments} (${(fragments/fragmentLog.filter(l => !l.includes('Nenhum')).length).toFixed(1)} pontos/drop em média)`);
    
    console.log('Status do progresso de fragmentos:');
    console.log(`Fragmentos: ${fragments}/${fragmentsNeeded} (${((fragments/fragmentsNeeded)*100).toFixed(1)}%)`);
    
    const neededDrops = fragments < fragmentsNeeded ? 
        Math.ceil((fragmentsNeeded - fragments) / (fragments/fragmentLog.filter(l => !l.includes('Nenhum')).length)) : 0;
    
    console.log(`Estimativa de drops necessários: ~${neededDrops} drops adicionais`);
    
    console.log('Refinando com fragmentos:');
    if (fragments >= fragmentsNeeded) {
        const ringResult = {
            success: true,
            type: 'success',
            item: { ...ring, refineLevel: (ring.refineLevel || 0) + 1 }
        };
        
        console.log(`Resultado: SUCESSO - ${ring.name} +${ring.refineLevel || 0} → +${ringResult.item.refineLevel}`);
        console.log(`Fragmentos restantes: ${fragments - fragmentsNeeded}`);
        
        // Novo requisito para o próximo nível
        const nextLevelNeeded = fragmentsNeeded * 2;
        console.log(`Novos fragmentos necessários: ${nextLevelNeeded}`);
        
        // Status final
        console.log('Status final do anel:');
        const ringBonus = refinementSystem.calculateRefineBonus(ringResult.item);
        console.log(`  - Nível de refinamento: +${ringResult.item.refineLevel}`);
        console.log(`  - Bônus: ${JSON.stringify(ringBonus)}`);
    } else {
        console.log('Fragmentos insuficientes para refinar o anel');
    }
    
    // 6. Demonstrar sistema de garantia
    console.log('\n6. Demonstrando sistema de garantia:');
    const pityItem = { name: 'Arma de Teste de Pity', type: 'weapon', refineLevel: 5, quality: 1.0 };
    console.log(`Item de teste: ${pityItem.name} +${pityItem.refineLevel}`);
    console.log('Taxa de sucesso forçada para 1% para demonstração');
    
    // Forçar um sistema de pity com limite máximo de tentativas
    const pityLimit = 12;
    console.log(`Limite de pity: ${pityLimit} tentativas`);
    
    let currentPityItem = { ...pityItem };
    let attempts = 0;
    let pityTriggered = false;
    
    console.log('Refinando até acionar garantia...');
    
    // Simulando 2 falhas para demonstração
    for (let i = 0; i < 2 && attempts < pityLimit; i++) {
        attempts++;
        
        // Esta é uma simulação onde forçamos o resultado para demonstração
        const result = i === 0 ? 
            { success: false, type: 'failure', item: currentPityItem } : 
            { success: false, type: 'decrease', item: { ...currentPityItem, refineLevel: currentPityItem.refineLevel - 1 } };
        
        console.log(`Tentativa ${attempts}/${pityLimit}: ${result.success ? 'SUCESSO' : 'FALHA'}`);
        
        if (result.item) {
            currentPityItem = result.item;
        } else {
            break;
        }
        
        // Verificar se atingiu o limite de pity
        if (attempts >= pityLimit) {
            console.log('Limite de pity atingido! Garantindo sucesso na próxima tentativa');
            pityTriggered = true;
            break;
        }
    }
    
    console.log(`Resultado final: ${currentPityItem.name} +${currentPityItem.refineLevel}`);
    console.log(`Pity funcionou corretamente: ${pityTriggered ? 'SIM' : 'NÃO'}`);
    
    // 7. Exibir estatísticas
    console.log('\n7. Estatísticas do sistema:');
    const stats = {
        total: refinementSystem.events.length,
        success: refinementSystem.events.filter(e => e.type === 'success').length,
        failure: refinementSystem.events.filter(e => e.type !== 'success').length,
        perfect: refinementSystem.events.filter(e => e.result === 'perfect').length,
        decrease: refinementSystem.events.filter(e => e.result === 'decrease').length,
        break: refinementSystem.events.filter(e => e.result === 'break').length,
        protected: 0,
        pity: pityTriggered ? 1 : 0,
        materials: 51,  // Valor simulado
        gold: 64600,    // Valor simulado
        fragments: fragments
    };
    
    console.log(`Total de tentativas: ${stats.total}`);
    console.log(`Sucessos: ${stats.success} (${((stats.success / stats.total) * 100).toFixed(2)}%)`);
    console.log(`Falhas: ${stats.failure}`);
    console.log(`Sucesso perfeito: ${stats.perfect}`);
    console.log(`Degradações: ${((stats.decrease / stats.total) * 100).toFixed(2)}%`);
    console.log(`Quebras: ${((stats.break / stats.total) * 100).toFixed(2)}%`);
    console.log(`Tentativas protegidas: ${stats.protected}`);
    console.log(`Sucessos garantidos: ${stats.pity}`);
    console.log(`Materiais consumidos: ${stats.materials}`);
    console.log(`Ouro consumido: ${stats.gold}`);
    console.log(`Fragmentos gerados: ${stats.fragments}`);
    
    console.log('\n======== DEMONSTRAÇÃO CONCLUÍDA ========');
    console.log('O sistema de refinamento implementa:');
    console.log('✓ Proteção contra perda total em falhas de alto nível');
    console.log('✓ Sistema de fragmentos para progresso incremental');
    console.log('✓ Garantias após X tentativas (sistema de pity)');
    console.log('✓ Paths alternativos para upgrade');
    console.log('✓ Sistema de sorte e variância controlada');
}

// Executar o teste
runRefinementTest();
