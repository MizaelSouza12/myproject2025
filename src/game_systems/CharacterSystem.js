const EventEmitter = require('events');
const logger = require('../utils/logger');

class CharacterSystem {
    constructor() {
        this.events = new EventEmitter();
        this.characters = new Map();
        this.initialized = false;
    }

    async initialize() {
        try {
            // Carregar dados dos personagens do banco de dados
            this.initialized = true;
            logger.info('Sistema de personagens inicializado com sucesso');
        } catch (error) {
            logger.error('Erro ao inicializar sistema de personagens:', error);
            throw error;
        }
    }

    async createCharacter(accountId, characterData) {
        try {
            // Validar dados do personagem
            if (!characterData.name || !characterData.class) {
                throw new Error('Dados do personagem inválidos');
            }

            // Criar personagem
            const character = {
                id: Date.now(),
                accountId,
                name: characterData.name,
                class: characterData.class,
                level: 1,
                experience: 0,
                stats: {
                    strength: 10,
                    dexterity: 10,
                    intelligence: 10,
                    constitution: 10
                },
                position: {
                    x: 2000,
                    y: 2000,
                    map: 0
                },
                inventory: [],
                skills: [],
                created: new Date(),
                lastLogin: new Date()
            };

            this.characters.set(character.id, character);
            this.events.emit('character:created', character);
            
            return character;
        } catch (error) {
            logger.error('Erro ao criar personagem:', error);
            throw error;
        }
    }

    async loadCharacter(characterId) {
        try {
            const character = this.characters.get(characterId);
            if (!character) {
                throw new Error('Personagem não encontrado');
            }
            return character;
        } catch (error) {
            logger.error('Erro ao carregar personagem:', error);
            throw error;
        }
    }

    async saveCharacter(characterId) {
        try {
            const character = this.characters.get(characterId);
            if (!character) {
                throw new Error('Personagem não encontrado');
            }
            // Salvar no banco de dados
            this.events.emit('character:saved', character);
        } catch (error) {
            logger.error('Erro ao salvar personagem:', error);
            throw error;
        }
    }

    async deleteCharacter(characterId) {
        try {
            const character = this.characters.get(characterId);
            if (!character) {
                throw new Error('Personagem não encontrado');
            }
            this.characters.delete(characterId);
            this.events.emit('character:deleted', character);
        } catch (error) {
            logger.error('Erro ao deletar personagem:', error);
            throw error;
        }
    }

    async updateCharacterPosition(characterId, x, y, map) {
        try {
            const character = this.characters.get(characterId);
            if (!character) {
                throw new Error('Personagem não encontrado');
            }
            character.position = { x, y, map };
            this.events.emit('character:moved', { characterId, position: character.position });
        } catch (error) {
            logger.error('Erro ao atualizar posição do personagem:', error);
            throw error;
        }
    }

    async addExperience(characterId, amount) {
        try {
            const character = this.characters.get(characterId);
            if (!character) {
                throw new Error('Personagem não encontrado');
            }
            character.experience += amount;
            
            // Verificar level up
            const newLevel = this.calculateLevel(character.experience);
            if (newLevel > character.level) {
                character.level = newLevel;
                this.events.emit('character:levelup', { characterId, level: newLevel });
            }
            
            this.events.emit('character:experience', { characterId, experience: character.experience });
        } catch (error) {
            logger.error('Erro ao adicionar experiência:', error);
            throw error;
        }
    }

    calculateLevel(experience) {
        // Fórmula de cálculo de level baseada na experiência
        return Math.floor(Math.sqrt(experience / 100)) + 1;
    }

    async shutdown() {
        try {
            // Salvar todos os personagens
            for (const [characterId] of this.characters) {
                await this.saveCharacter(characterId);
            }
            
            this.characters.clear();
            this.initialized = false;
            logger.info('Sistema de personagens desligado com sucesso');
        } catch (error) {
            logger.error('Erro ao desligar sistema de personagens:', error);
            throw error;
        }
    }
}

module.exports = CharacterSystem; 