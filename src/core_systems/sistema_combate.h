#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace WYD {
    class SistemaCombate {
    private:
        struct CombatState {
            bool isAttacking;
            bool isDefending;
            bool isStunned;
            DWORD lastAttackTime;
            DWORD lastDefenseTime;
            DWORD stunEndTime;
        };

        std::unordered_map<DWORD, CombatState> combatStates;
        std::unordered_map<DWORD, CombatStats> baseStats;
        std::unordered_map<DWORD, std::vector<SkillData>> characterSkills;

    public:
        SistemaCombate() = default;
        ~SistemaCombate() = default;

        // Gerenciamento de estado de combate
        void initializeCombatState(DWORD characterId) {
            CombatState state{};
            state.isAttacking = false;
            state.isDefending = false;
            state.isStunned = false;
            state.lastAttackTime = 0;
            state.lastDefenseTime = 0;
            state.stunEndTime = 0;
            combatStates[characterId] = state;
        }

        // Sistema de dano
        DWORD calculateDamage(DWORD attackerId, DWORD targetId, const SkillData& skill) {
            auto& attackerStats = baseStats[attackerId];
            auto& targetStats = baseStats[targetId];

            // Cálculo base de dano
            DWORD baseDamage = attackerStats.attack + skill.damage;
            
            // Aplicar modificadores de defesa
            DWORD defenseReduction = targetStats.defense * 0.5f;
            baseDamage = (baseDamage > defenseReduction) ? (baseDamage - defenseReduction) : 1;

            // Chance de crítico
            if (rand() % 100 < attackerStats.criticalRate) {
                baseDamage *= (1.0f + (attackerStats.criticalDamage / 100.0f));
            }

            // Chance de esquiva
            if (rand() % 100 < targetStats.dodgeRate) {
                return 0;
            }

            // Chance de bloqueio
            if (rand() % 100 < targetStats.blockRate) {
                baseDamage *= 0.5f;
            }

            return static_cast<DWORD>(baseDamage);
        }

        // Sistema de habilidades
        bool useSkill(DWORD characterId, DWORD skillId, DWORD targetId) {
            auto& state = combatStates[characterId];
            if (state.isStunned) return false;

            auto& skills = characterSkills[characterId];
            auto skillIt = std::find_if(skills.begin(), skills.end(),
                [skillId](const SkillData& skill) { return skill.id == skillId; });

            if (skillIt == skills.end()) return false;

            auto& skill = *skillIt;
            auto& stats = baseStats[characterId];

            // Verificar cooldown
            DWORD currentTime = GetTickCount();
            if (currentTime - state.lastAttackTime < skill.cooldown) return false;

            // Verificar mana
            if (stats.mp < skill.manaCost) return false;

            // Aplicar dano
            DWORD damage = calculateDamage(characterId, targetId, skill);
            applyDamage(targetId, damage);

            // Atualizar estado
            stats.mp -= skill.manaCost;
            state.lastAttackTime = currentTime;

            return true;
        }

        // Sistema de defesa
        void startDefense(DWORD characterId) {
            auto& state = combatStates[characterId];
            if (!state.isStunned) {
                state.isDefending = true;
                state.lastDefenseTime = GetTickCount();
            }
        }

        void endDefense(DWORD characterId) {
            auto& state = combatStates[characterId];
            state.isDefending = false;
        }

        // Sistema de stun
        void applyStun(DWORD characterId, DWORD duration) {
            auto& state = combatStates[characterId];
            state.isStunned = true;
            state.stunEndTime = GetTickCount() + duration;
        }

        void updateStunStates() {
            DWORD currentTime = GetTickCount();
            for (auto& [id, state] : combatStates) {
                if (state.isStunned && currentTime >= state.stunEndTime) {
                    state.isStunned = false;
                }
            }
        }

        // Gerenciamento de stats
        void setBaseStats(DWORD characterId, const CombatStats& stats) {
            baseStats[characterId] = stats;
        }

        void addSkill(DWORD characterId, const SkillData& skill) {
            characterSkills[characterId].push_back(skill);
        }

        // Sistema de combate PvP
        bool initiatePvP(DWORD attackerId, DWORD targetId) {
            // Verificar condições de PvP (nível, área, etc)
            return true;
        }

        // Sistema de combate PvE
        bool initiatePvE(DWORD characterId, DWORD mobId) {
            // Verificar condições de PvE (nível, área, etc)
            return true;
        }

    private:
        void applyDamage(DWORD targetId, DWORD damage) {
            auto& stats = baseStats[targetId];
            stats.hp = (damage >= stats.hp) ? 0 : (stats.hp - damage);
        }
    };
} 