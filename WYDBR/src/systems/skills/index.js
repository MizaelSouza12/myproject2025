/**
 * Sistema de Habilidades (Skills) WYDBRASIL
 * 
 * Sistema avançado para gerenciar habilidades de personagens
 * com suporte a efeitos visuais, animações e máquina de estados.
 */

const SkillManager = require('./core/SkillManager');
const EffectSystem = require('./effects/EffectSystem');
const AnimationController = require('./animations/AnimationController');
const SkillStateMachine = require('./state/SkillStateMachine');

class SkillSystem {
    constructor() {
        this.manager = new SkillManager();
        this.effects = new EffectSystem();
        this.animations = new AnimationController();
        this.stateMachine = new SkillStateMachine();
        console.log('Sistema de Habilidades WYDBRASIL inicializado');
    }
    
    /**
     * Registra uma nova habilidade no sistema
     * @param {object} skill Definição da habilidade
     * @returns {string} ID da habilidade registrada
     */
    registerSkill(skill) {
        return this.manager.registerSkill(skill);
    }
    
    /**
     * Executa uma habilidade para um personagem
     * @param {string} skillId ID da habilidade
     * @param {object} character Personagem executando a habilidade
     * @param {object} target Alvo da habilidade (opcional)
     * @param {object} options Opções adicionais
     * @returns {boolean} Sucesso da execução
     */
    executeSkill(skillId, character, target = null, options = {}) {
        // Verificar se a habilidade pode ser executada
        if (!this.manager.canExecuteSkill(skillId, character)) {
            return false;
        }
        
        // Iniciar animação
        const animationId = this.animations.startAnimation(skillId, character, target);
        
        // Aplicar efeitos
        const effectsApplied = this.effects.applySkillEffects(skillId, character, target);
        
        // Atualizar estado da habilidade
        this.stateMachine.transition(skillId, 'execute', { character, target });
        
        // Adicionar cooldown
        this.manager.addCooldown(skillId, character);
        
        return true;
    }
    
    /**
     * Verifica se uma habilidade está em cooldown
     * @param {string} skillId ID da habilidade
     * @param {object} character Personagem
     * @returns {object} Informações de cooldown
     */
    checkCooldown(skillId, character) {
        return this.manager.getCooldownInfo(skillId, character);
    }
    
    /**
     * Obtém a lista de habilidades de um personagem
     * @param {object} character Personagem
     * @returns {array} Lista de habilidades
     */
    getCharacterSkills(character) {
        return this.manager.getSkillsForCharacter(character);
    }
    
    /**
     * Obtém detalhes de uma habilidade específica
     * @param {string} skillId ID da habilidade
     * @returns {object} Detalhes da habilidade
     */
    getSkillDetails(skillId) {
        return this.manager.getSkillDetails(skillId);
    }
    
    /**
     * Inicia a máquina de estados para uma habilidade
     * @param {string} skillId ID da habilidade
     * @returns {boolean} Sucesso da operação
     */
    initializeSkillStateMachine(skillId) {
        return this.stateMachine.initialize(skillId);
    }
}

module.exports = SkillSystem;
