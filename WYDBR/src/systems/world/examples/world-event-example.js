/**
 * Exemplo de Evento Mundial - Invasão de Dragões
 * 
 * Este script demonstra como criar e gerenciar um evento de mundo
 * dinâmico usando o Sistema de Mundo do WYDBRASIL.
 */

const WorldSystem = require('../index');

// Iniciar sistema de mundo
const worldSystem = new WorldSystem();

console.log('======== Sistema de Eventos de Mundo WYDBRASIL ========');
console.log('          Demonstração de Invasão de Dragões           ');
console.log('=====================================================');

// Configuração do evento de invasão de dragões
const dragonInvasionConfig = {
    name: 'Invasão dos Dragões Antigos',
    description: 'Dragões antigos invadiram a região de Armia! Defenda a cidade!',
    difficulty: 'hard',
    minLevel: 200,
    duration: 120, // minutos
    rewards: {
        exp: 200000,
        gold: 5000000,
        items: [
            { id: 'dragon_scale', chance: 0.75, quantity: { min: 1, max: 5 } },
            { id: 'ancient_fire_stone', chance: 0.25, quantity: { min: 1, max: 2 } },
            { id: 'legendary_dragon_sword', chance: 0.05, quantity: { min: 1, max: 1 } }
        ]
    },
    waves: [
        {
            name: 'Dragões Menores',
            monsters: [
                { id: 'young_dragon', count: 15, level: 180 },
                { id: 'dragon_warrior', count: 8, level: 190 }
            ],
            triggerAt: 0 // minutos desde o início
        },
        {
            name: 'Dragões Elementais',
            monsters: [
                { id: 'fire_dragon', count: 5, level: 195 },
                { id: 'ice_dragon', count: 5, level: 195 },
                { id: 'thunder_dragon', count: 5, level: 195 }
            ],
            triggerAt: 30 // minutos desde o início
        },
        {
            name: 'Dragão Ancião',
            monsters: [
                { id: 'ancient_dragon', count: 1, level: 250 }
            ],
            triggerAt: 90 // minutos desde o início
        }
    ],
    regions: ['armia_field', 'armia_city', 'armia_castle'],
    weatherEffect: 'storm'
};

// Função para simular a progressão do evento
function simulateEventProgression(eventId) {
    console.log(`\nEvento iniciado com ID: ${eventId}`);
    
    // Simular passagem de tempo e progresso do evento
    console.log('\n1. Primeira onda de dragões surgiu!');
    console.log('   15 Dragões Jovens e 8 Guerreiros Dragão aparecem em Armia!');
    
    // Avançar o tempo para a segunda onda
    setTimeout(() => {
        console.log('\n2. O tempo mudou para Tempestade em Armia');
        worldSystem.changeWeather('armia_field', 'storm', 60);
        
        console.log('\n3. Segunda onda de dragões surgiu!');
        console.log('   5 Dragões de Fogo, 5 Dragões de Gelo e 5 Dragões de Trovão aparecem!');
        
        // Avançar o tempo para a onda final
        setTimeout(() => {
            console.log('\n4. Onda final: O Dragão Ancião apareceu!');
            console.log('   Um Dragão Ancião poderoso (Nível 250) apareceu em Armia Castle!');
            
            // Simular término do evento
            setTimeout(() => {
                console.log('\n5. Evento concluído com sucesso!');
                console.log('   Os heróis de Armia derrotaram a invasão de dragões!');
                console.log('   Recompensas distribuídas para 37 participantes');
                
                // Retornar o clima ao normal
                worldSystem.changeWeather('armia_field', 'clear', 0);
                console.log('\n6. O clima voltou ao normal em Armia');
                
                console.log('\n======== Demonstração Concluída ========');
            }, 5000);
        }, 5000);
    }, 5000);
}

// Executar demonstração
(async function runDemo() {
    console.log('Iniciando sistema de mundo...');
    worldSystem.start();
    
    console.log('\nAgendando evento de invasão de dragões para iniciar em 5 segundos...');
    
    // Agendar o evento para iniciar em 5 segundos
    const scheduledTime = new Date(Date.now() + 5000);
    const eventId = worldSystem.scheduleEvent('dragon_invasion', dragonInvasionConfig, scheduledTime);
    
    console.log(`Evento agendado com ID: ${eventId}`);
    console.log(`Horário agendado: ${scheduledTime.toLocaleTimeString()}`);
    
    // Simular a progressão do evento após o início
    setTimeout(() => {
        simulateEventProgression(eventId);
    }, 5000);
})();
