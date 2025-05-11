#pragma once

#include "../core/WYDTypes.h"
#include "sistema_combate.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace WYD {
    class SistemaPersonagem {
    private:
        struct CharacterState {
            bool isOnline;
            bool isInCombat;
            bool isMoving;
            Position lastPosition;
            DWORD lastUpdateTime;
        };

        std::unordered_map<DWORD, CharacterData> characters;
        std::unordered_map<DWORD, CharacterState> characterStates;
        SistemaCombate& combatSystem;

    public:
        SistemaPersonagem(SistemaCombate& combat) : combatSystem(combat) {}
        ~SistemaPersonagem() = default;

        // Gerenciamento de personagem
        bool createCharacter(DWORD id, const std::string& name, ClassType classType) {
            if (characters.find(id) != characters.end()) return false;

            CharacterData character{};
            character.id = id;
            character.name = name;
            character.classType = classType;
            character.level = 1;
            character.experience = 0;
            character.gold = 0;
            character.position = {0, 0, 0};

            // Inicializar stats base
            initializeBaseStats(character);

            characters[id] = character;
            initializeCharacterState(id);
            return true;
        }

        bool deleteCharacter(DWORD id) {
            return characters.erase(id) > 0;
        }

        // Gerenciamento de estado
        void initializeCharacterState(DWORD id) {
            CharacterState state{};
            state.isOnline = false;
            state.isInCombat = false;
            state.isMoving = false;
            state.lastPosition = {0, 0, 0};
            state.lastUpdateTime = GetTickCount();
            characterStates[id] = state;
        }

        void setOnline(DWORD id, bool online) {
            auto& state = characterStates[id];
            state.isOnline = online;
            if (online) {
                state.lastUpdateTime = GetTickCount();
            }
        }

        // Sistema de movimento
        void updatePosition(DWORD id, const Position& newPosition) {
            auto& state = characterStates[id];
            state.lastPosition = newPosition;
            state.isMoving = true;
            state.lastUpdateTime = GetTickCount();

            auto& character = characters[id];
            character.position = newPosition;
        }

        // Sistema de experiência e nível
        bool addExperience(DWORD id, DWORD amount) {
            auto& character = characters[id];
            character.experience += amount;

            // Verificar level up
            DWORD expNeeded = calculateExpNeeded(character.level);
            if (character.experience >= expNeeded) {
                return levelUp(id);
            }
            return false;
        }

        bool levelUp(DWORD id) {
            auto& character = characters[id];
            character.level++;
            character.experience = 0;

            // Atualizar stats
            updateStatsOnLevelUp(character);
            return true;
        }

        // Sistema de inventário
        bool addItem(DWORD id, const ItemData& item) {
            auto& character = characters[id];
            return character.inventory.push_back(item);
        }

        bool removeItem(DWORD id, DWORD itemId) {
            auto& character = characters[id];
            auto& inventory = character.inventory;
            
            for (size_t i = 0; i < inventory.getSize(); i++) {
                if (inventory[i].id == itemId) {
                    // Remover item
                    for (size_t j = i; j < inventory.getSize() - 1; j++) {
                        inventory[j] = inventory[j + 1];
                    }
                    inventory.getSize()--;
                    return true;
                }
            }
            return false;
        }

        // Sistema de habilidades
        bool learnSkill(DWORD id, const SkillData& skill) {
            auto& character = characters[id];
            combatSystem.addSkill(id, skill);
            return true;
        }

        // Sistema de combate
        bool enterCombat(DWORD id) {
            auto& state = characterStates[id];
            if (!state.isOnline || state.isInCombat) return false;

            state.isInCombat = true;
            combatSystem.initializeCombatState(id);
            return true;
        }

        bool exitCombat(DWORD id) {
            auto& state = characterStates[id];
            state.isInCombat = false;
            return true;
        }

        // Getters
        const CharacterData& getCharacter(DWORD id) const {
            return characters.at(id);
        }

        const CharacterState& getCharacterState(DWORD id) const {
            return characterStates.at(id);
        }

    private:
        void initializeBaseStats(CharacterData& character) {
            CombatStats stats{};
            switch (character.classType) {
                case ClassType::WARRIOR:
                    stats.hp = 100;
                    stats.mp = 50;
                    stats.stamina = 100;
                    stats.attack = 10;
                    stats.defense = 8;
                    stats.magicAttack = 2;
                    stats.magicDefense = 4;
                    break;
                case ClassType::MAGE:
                    stats.hp = 60;
                    stats.mp = 100;
                    stats.stamina = 50;
                    stats.attack = 4;
                    stats.defense = 4;
                    stats.magicAttack = 12;
                    stats.magicDefense = 8;
                    break;
                // Adicionar outros casos para outras classes
                default:
                    break;
            }
            character.stats = stats;
        }

        void updateStatsOnLevelUp(CharacterData& character) {
            auto& stats = character.stats;
            switch (character.classType) {
                case ClassType::WARRIOR:
                    stats.hp += 10;
                    stats.mp += 5;
                    stats.stamina += 5;
                    stats.attack += 2;
                    stats.defense += 1;
                    stats.magicAttack += 1;
                    stats.magicDefense += 1;
                    break;
                case ClassType::MAGE:
                    stats.hp += 5;
                    stats.mp += 10;
                    stats.stamina += 3;
                    stats.attack += 1;
                    stats.defense += 1;
                    stats.magicAttack += 2;
                    stats.magicDefense += 2;
                    break;
                // Adicionar outros casos para outras classes
                default:
                    break;
            }
        }

        DWORD calculateExpNeeded(BYTE level) {
            return level * 1000; // Fórmula simples para exemplo
        }
    };
} 