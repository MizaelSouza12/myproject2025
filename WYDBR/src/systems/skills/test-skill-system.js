/**
 * Teste do Sistema de Habilidades WYDBRASIL
 * 
 * Este script demonstra e valida as funcionalidades do sistema
 * de habilidades (skills) do WYDBRASIL, incluindo execução de
 * habilidades, cooldowns, e máquina de estados.
 */

// Simulação do sistema de habilidades
class SkillSystem {
    constructor() {
        this.skills = {};
        this.cooldowns = {};
        this.characters = {};
        this.states = {};
        this.animations = {};
        this.effects = {};
        this.events = [];
        
        console.log('[SkillSystem] Sistema de Habilidades inicializado');
    }
    
    // Registra uma habilidade
    registerSkill(characterId, skill) {
        if (!this.characters[characterId]) {
            this.characters[characterId] = { id: characterId, skills: {} };
        }
        
        this.characters[characterId].skills[skill.id] = skill;
        this.skills[skill.id] = skill;
        this.states[skill.id] = 'idle';
        this.cooldowns[skill.id] = { active: false, remaining: 0 };
        
        console.log(`[SkillSystem] Habilidade ${skill.id} registrada para ${characterId}`);
        return skill.id;
    }
    
    // Executa uma habilidade
    executeSkill(characterId, skillId, target = null) {
        // Verificar se a habilidade existe e pode ser executada
        if (!this.characters[characterId] || 
            !this.characters[characterId].skills[skillId] ||
            this.cooldowns[skillId].active) {
            console.log(`[SkillSystem] Falha ao executar ${skillId}: indisponível ou em cooldown`);
            return false;
        }
        
        const skill = this.skills[skillId];
        
        // Iniciar animação
        this.animations[skillId] = { playing: true, progress: 0 };
        
        // Aplicar efeitos
        this.applyEffects(skillId, characterId, target);
        
        // Atualizar estado
        this.changeState(skillId, 'casting');
        
        // Adicionar cooldown
        this.setCooldown(skillId, skill.cooldown || 5);
        
        // Registrar evento
        this.logEvent('skill_executed', { characterId, skillId, target });
        
        console.log(`[SkillSystem] Habilidade ${skillId} executada por ${characterId}`);
        return true;
    }
    
    // Aplica efeitos de uma habilidade
    applyEffects(skillId, casterId, targetId) {
        const skill = this.skills[skillId];
        const effects = [];
        
        if (skill.effects) {
            skill.effects.forEach(effect => {
                const appliedEffect = {
                    id: `${effect.id}_${Date.now()}`,
                    name: effect.name,
                    type: effect.type,
                    duration: effect.duration,
                    magnitude: effect.magnitude,
                    source: casterId,
                    target: targetId
                };
                
                effects.push(appliedEffect);
                this.logEvent('effect_applied', appliedEffect);
                
                console.log(`[SkillSystem] Efeito ${effect.name} aplicado por ${casterId}`);
            });
        }
        
        this.effects[skillId] = effects;
        return effects;
    }
    
    // Define um cooldown para uma habilidade
    setCooldown(skillId, duration) {
        this.cooldowns[skillId] = {
            active: true,
            remaining: duration,
            total: duration,
            startTime: Date.now()
        };
        
        console.log(`[SkillSystem] Cooldown de ${duration}s definido para ${skillId}`);
        
        // Simular a progressão do cooldown
        setTimeout(() => {
            this.cooldowns[skillId].active = false;
            this.cooldowns[skillId].remaining = 0;
            this.logEvent('cooldown_complete', { skillId });
            console.log(`[SkillSystem] Cooldown de ${skillId} concluído`);
        }, duration * 1000);
    }
    
    // Altera o estado de uma habilidade
    changeState(skillId, newState) {
        const oldState = this.states[skillId];
        this.states[skillId] = newState;
        
        this.logEvent('state_changed', { 
            skillId, 
            oldState, 
            newState,
            timestamp: Date.now()
        });
        
        console.log(`[SkillSystem] Estado de ${skillId} alterado: ${oldState} -> ${newState}`);
    }
    
    // Registra um evento
    logEvent(type, data) {
        const event = {
            type,
            data,
            timestamp: Date.now()
        };
        
        this.events.push(event);
    }
    
    // Obtém informações de uma habilidade
    getSkillInfo(characterId, skillId) {
        if (!this.characters[characterId] || !this.characters[characterId].skills[skillId]) {
            return null;
        }
        
        const skill = this.skills[skillId];
        const state = this.states[skillId];
        const cooldown = this.cooldowns[skillId];
        
        return {
            id: skill.id,
            name: skill.name,
            description: skill.description,
            state,
            cooldown,
            level: skill.level || 1,
            charges: skill.charges || { current: 1, max: 1 }
        };
    }
}

// Executar demonstração do sistema de habilidades
function runSkillSystemDemo() {
    console.log('======== Sistema de Habilidades WYDBRASIL ========');
    console.log('      Demonstração e Validação do Sistema          ');
    console.log('=================================================');
    
    // Inicializar sistema
    const skillSystem = new SkillSystem();
    
    // Criar personagem de teste
    const characterId = 'test_player';
    
    // Definir habilidades
    const skills = [
        {
            id: 'fireball',
            name: 'Bola de Fogo',
            description: 'Lança uma bola de fogo que causa dano a um alvo',
            type: 'attack',
            element: 'fire',
            cooldown: 4,
            castTime: 1.2,
            charges: { current: 1, max: 1 },
            effects: [
                { id: 'fire_dmg', name: 'Dano de Fogo', type: 'damage', magnitude: 150, duration: 0 },
                { id: 'burn', name: 'Queimadura', type: 'dot', magnitude: 25, duration: 3 }
            ]
        },
        {
            id: 'heal',
            name: 'Cura',
            description: 'Recupera pontos de vida do alvo',
            type: 'heal',
            element: 'light',
            cooldown: 8,
            castTime: 2.0,
            charges: { current: 1, max: 1 },
            effects: [
                { id: 'healing', name: 'Cura', type: 'heal', magnitude: 300, duration: 0 },
                { id: 'regen', name: 'Regeneração', type: 'hot', magnitude: 40, duration: 5 }
            ]
        },
        {
            id: 'dash',
            name: 'Investida',
            description: 'Avança rapidamente em uma direção',
            type: 'movement',
            cooldown: 6,
            castTime: 0,
            charges: { current: 2, max: 2 },
            effects: [
                { id: 'dash', name: 'Investida', type: 'movement', magnitude: 10, duration: 0 },
                { id: 'speed', name: 'Velocidade', type: 'buff', magnitude: 20, duration: 2 }
            ]
        },
        {
            id: 'slash',
            name: 'Golpe de Espada',
            description: 'Ataca com a espada em área',
            type: 'attack',
            element: 'physical',
            cooldown: 3,
            castTime: 0.8,
            charges: { current: 1, max: 1 },
            effects: [
                { id: 'phys_dmg', name: 'Dano Físico', type: 'damage', magnitude: 180, duration: 0 },
                { id: 'bleed', name: 'Sangramento', type: 'dot', magnitude: 15, duration: 4 }
            ]
        },
        {
            id: 'block',
            name: 'Bloquear',
            description: 'Bloqueia ataques inimigos',
            type: 'defense',
            cooldown: 10,
            castTime: 0.5,
            charges: { current: 1, max: 1 },
            effects: [
                { id: 'block', name: 'Bloqueio', type: 'defense', magnitude: 70, duration: 3 },
                { id: 'counter', name: 'Contra-ataque', type: 'buff', magnitude: 30, duration: 3 }
            ]
        }
    ];
    
    // Registrar habilidades para o personagem
    console.log('\n------- Registrando habilidades -------');
    skills.forEach(skill => {
        skillSystem.registerSkill(characterId, skill);
        console.log(`Habilidade registrada: ${skill.name} (${skill.id})`);
    });
    
    // Demonstração de execução de habilidades
    console.log('\n------- Demonstração de Execução -------');
    
    // Executar algumas habilidades
    console.log('\nExecutando Bola de Fogo...');
    const target1 = 'enemy1';
    const result1 = skillSystem.executeSkill(characterId, 'fireball', target1);
    
    console.log(`Resultado: ${result1 ? 'Sucesso' : 'Falha'}`);
    
    // Tentar executar uma habilidade em cooldown
    console.log('\nTentando executar Bola de Fogo novamente (deve falhar devido ao cooldown)...');
    const result2 = skillSystem.executeSkill(characterId, 'fireball', target1);
    
    console.log(`Resultado: ${result2 ? 'Sucesso' : 'Falha - Cooldown ativo'}`);
    
    // Executar outra habilidade
    console.log('\nExecutando Cura...');
    const target2 = 'ally1';
    const result3 = skillSystem.executeSkill(characterId, 'heal', target2);
    
    console.log(`Resultado: ${result3 ? 'Sucesso' : 'Falha'}`);
    
    // Verificar estado das habilidades
    console.log('\n------- Estado das Habilidades -------');
    skills.forEach(skill => {
        const info = skillSystem.getSkillInfo(characterId, skill.id);
        console.log(`${info.name}:`);
        console.log(`  Estado: ${info.state}`);
        console.log(`  Cooldown: ${info.cooldown.active ? 'Sim' : 'Não'}`);
        console.log(`  Cargas: ${info.charges.current}/${info.charges.max}`);
    });
    
    // Demonstrar progresso do cooldown
    console.log('\n------- Simulação de Progressão -------');
    console.log('Aguardando cooldowns (simulação rápida)...');
    
    // Simulação do tempo passando (em um caso real, isso seria tratado pelo sistema de atualização)
    setTimeout(() => {
        console.log('\n------- Estado Final -------');
        skills.forEach(skill => {
            const info = skillSystem.getSkillInfo(characterId, skill.id);
            console.log(`${info.name}:`);
            console.log(`  Estado: ${info.state}`);
            console.log(`  Cooldown: ${info.cooldown.active ? 'Sim' : 'Não'}`);
            console.log(`  Cargas: ${info.charges.current}/${info.charges.max}`);
        });
        
        // Estatísticas do sistema
        console.log('\n------- Estatísticas -------');
        console.log(`Total de eventos: ${skillSystem.events.length}`);
        console.log(`Habilidades executadas: ${skillSystem.events.filter(e => e.type === 'skill_executed').length}`);
        console.log(`Mudanças de estado: ${skillSystem.events.filter(e => e.type === 'state_changed').length}`);
        console.log(`Efeitos aplicados: ${skillSystem.events.filter(e => e.type === 'effect_applied').length}`);
        
        console.log('\n======== Demonstração concluída ========');
    }, 5000);
}

// Executar a demonstração
runSkillSystemDemo();
