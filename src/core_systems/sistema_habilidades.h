#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace WYD {
    class SistemaHabilidades {
    private:
        struct SkillState {
            DWORD lastUseTime;
            BYTE currentLevel;
            bool isUnlocked;
        };

        struct SkillEffect {
            enum class Type {
                DAMAGE,
                HEAL,
                BUFF,
                DEBUFF,
                TELEPORT,
                SUMMON
            };

            Type type;
            DWORD value;
            DWORD duration;
            DWORD radius;
        };

        struct SkillRequirement {
            BYTE level;
            DWORD gold;
            std::vector<DWORD> requiredItems;
            std::vector<DWORD> requiredSkills;
        };

        std::unordered_map<DWORD, std::unordered_map<DWORD, SkillState>> characterSkills;
        std::unordered_map<DWORD, SkillData> skillDatabase;
        std::unordered_map<DWORD, std::vector<SkillEffect>> skillEffects;
        std::unordered_map<DWORD, SkillRequirement> skillRequirements;

    public:
        SistemaHabilidades() = default;
        ~SistemaHabilidades() = default;

        // Inicialização
        void initializeSkillSystem() {
            // Inicializar habilidades base para cada classe
            initializeClassSkills(ClassType::WARRIOR);
            initializeClassSkills(ClassType::MAGE);
            initializeClassSkills(ClassType::ARCHER);
            // ... outras classes
        }

        // Gerenciamento de habilidades
        bool learnSkill(DWORD characterId, DWORD skillId) {
            auto& characterSkillMap = characterSkills[characterId];
            
            // Verificar se a habilidade existe
            if (skillDatabase.find(skillId) == skillDatabase.end()) {
                return false;
            }

            // Verificar requisitos
            if (!checkSkillRequirements(characterId, skillId)) {
                return false;
            }

            // Inicializar estado da habilidade
            SkillState state{};
            state.lastUseTime = 0;
            state.currentLevel = 1;
            state.isUnlocked = true;
            characterSkillMap[skillId] = state;

            return true;
        }

        bool upgradeSkill(DWORD characterId, DWORD skillId) {
            auto& characterSkillMap = characterSkills[characterId];
            auto it = characterSkillMap.find(skillId);
            
            if (it == characterSkillMap.end() || !it->second.isUnlocked) {
                return false;
            }

            auto& state = it->second;
            if (state.currentLevel >= 10) { // Máximo nível
                return false;
            }

            // Verificar requisitos para upgrade
            if (!checkUpgradeRequirements(characterId, skillId, state.currentLevel + 1)) {
                return false;
            }

            state.currentLevel++;
            return true;
        }

        // Sistema de uso de habilidades
        bool useSkill(DWORD characterId, DWORD skillId, DWORD targetId) {
            auto& characterSkillMap = characterSkills[characterId];
            auto it = characterSkillMap.find(skillId);
            
            if (it == characterSkillMap.end() || !it->second.isUnlocked) {
                return false;
            }

            auto& state = it->second;
            auto& skill = skillDatabase[skillId];

            // Verificar cooldown
            DWORD currentTime = GetTickCount();
            if (currentTime - state.lastUseTime < skill.cooldown) {
                return false;
            }

            // Aplicar efeitos da habilidade
            applySkillEffects(characterId, targetId, skillId, state.currentLevel);

            // Atualizar estado
            state.lastUseTime = currentTime;
            return true;
        }

        // Sistema de efeitos
        void applySkillEffects(DWORD casterId, DWORD targetId, DWORD skillId, BYTE level) {
            auto& effects = skillEffects[skillId];
            for (const auto& effect : effects) {
                switch (effect.type) {
                    case SkillEffect::Type::DAMAGE:
                        applyDamageEffect(casterId, targetId, effect, level);
                        break;
                    case SkillEffect::Type::HEAL:
                        applyHealEffect(casterId, targetId, effect, level);
                        break;
                    case SkillEffect::Type::BUFF:
                        applyBuffEffect(casterId, targetId, effect, level);
                        break;
                    case SkillEffect::Type::DEBUFF:
                        applyDebuffEffect(casterId, targetId, effect, level);
                        break;
                    case SkillEffect::Type::TELEPORT:
                        applyTeleportEffect(casterId, targetId, effect, level);
                        break;
                    case SkillEffect::Type::SUMMON:
                        applySummonEffect(casterId, targetId, effect, level);
                        break;
                }
            }
        }

        // Getters
        const SkillData& getSkillData(DWORD skillId) const {
            return skillDatabase.at(skillId);
        }

        BYTE getSkillLevel(DWORD characterId, DWORD skillId) const {
            auto it = characterSkills.at(characterId).find(skillId);
            return (it != characterSkills.at(characterId).end()) ? it->second.currentLevel : 0;
        }

        bool isSkillUnlocked(DWORD characterId, DWORD skillId) const {
            auto it = characterSkills.at(characterId).find(skillId);
            return (it != characterSkills.at(characterId).end()) ? it->second.isUnlocked : false;
        }

    private:
        void initializeClassSkills(ClassType classType) {
            switch (classType) {
                case ClassType::WARRIOR:
                    initializeWarriorSkills();
                    break;
                case ClassType::MAGE:
                    initializeMageSkills();
                    break;
                case ClassType::ARCHER:
                    initializeArcherSkills();
                    break;
                // ... outras classes
            }
        }

        void initializeWarriorSkills() {
            // Inicializar habilidades do guerreiro
            SkillData skill{};
            skill.id = 1;
            skill.name = "Slash";
            skill.level = 1;
            skill.cooldown = 2000;
            skill.manaCost = 10;
            skill.damage = 50;
            skill.range = 2;
            skill.targetType = 1; // Single target
            skillDatabase[skill.id] = skill;

            // Adicionar efeitos
            SkillEffect effect{};
            effect.type = SkillEffect::Type::DAMAGE;
            effect.value = 50;
            effect.duration = 0;
            effect.radius = 0;
            skillEffects[skill.id].push_back(effect);
        }

        void initializeMageSkills() {
            // Inicializar habilidades do mago
            SkillData skill{};
            skill.id = 101;
            skill.name = "Fireball";
            skill.level = 1;
            skill.cooldown = 3000;
            skill.manaCost = 20;
            skill.damage = 70;
            skill.range = 5;
            skill.targetType = 1; // Single target
            skillDatabase[skill.id] = skill;

            // Adicionar efeitos
            SkillEffect effect{};
            effect.type = SkillEffect::Type::DAMAGE;
            effect.value = 70;
            effect.duration = 0;
            effect.radius = 0;
            skillEffects[skill.id].push_back(effect);
        }

        void initializeArcherSkills() {
            // Inicializar habilidades do arqueiro
            SkillData skill{};
            skill.id = 201;
            skill.name = "Precise Shot";
            skill.level = 1;
            skill.cooldown = 2500;
            skill.manaCost = 15;
            skill.damage = 60;
            skill.range = 8;
            skill.targetType = 1; // Single target
            skillDatabase[skill.id] = skill;

            // Adicionar efeitos
            SkillEffect effect{};
            effect.type = SkillEffect::Type::DAMAGE;
            effect.value = 60;
            effect.duration = 0;
            effect.radius = 0;
            skillEffects[skill.id].push_back(effect);
        }

        bool checkSkillRequirements(DWORD characterId, DWORD skillId) {
            const auto& requirements = skillRequirements[skillId];
            // Implementar verificação de requisitos
            return true;
        }

        bool checkUpgradeRequirements(DWORD characterId, DWORD skillId, BYTE newLevel) {
            // Implementar verificação de requisitos para upgrade
            return true;
        }

        void applyDamageEffect(DWORD casterId, DWORD targetId, const SkillEffect& effect, BYTE level) {
            // Implementar lógica de dano
        }

        void applyHealEffect(DWORD casterId, DWORD targetId, const SkillEffect& effect, BYTE level) {
            // Implementar lógica de cura
        }

        void applyBuffEffect(DWORD casterId, DWORD targetId, const SkillEffect& effect, BYTE level) {
            // Implementar lógica de buff
        }

        void applyDebuffEffect(DWORD casterId, DWORD targetId, const SkillEffect& effect, BYTE level) {
            // Implementar lógica de debuff
        }

        void applyTeleportEffect(DWORD casterId, DWORD targetId, const SkillEffect& effect, BYTE level) {
            // Implementar lógica de teleporte
        }

        void applySummonEffect(DWORD casterId, DWORD targetId, const SkillEffect& effect, BYTE level) {
            // Implementar lógica de invocação
        }
    };
} 