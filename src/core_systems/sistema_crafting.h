#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

namespace WYD {
    class SistemaCrafting {
    private:
        struct CraftingMaterial {
            DWORD itemId;
            DWORD quantity;
        };

        struct CraftingRecipe {
            DWORD id;
            std::string name;
            std::vector<CraftingMaterial> materials;
            DWORD resultItemId;
            DWORD resultQuantity;
            DWORD requiredLevel;
            DWORD requiredSkill;
            DWORD requiredClass;
            DWORD successRate;
            DWORD goldCost;
        };

        struct CraftingSkill {
            DWORD level;
            DWORD experience;
            DWORD maxLevel;
        };

        std::unordered_map<DWORD, CraftingRecipe> recipeDatabase;
        std::unordered_map<DWORD, CraftingSkill> characterSkills; // characterId -> skill

    public:
        SistemaCrafting() = default;
        ~SistemaCrafting() = default;

        // Inicialização
        void initializeCraftingSystem() {
            // Inicializar receitas padrão
            initializeDefaultRecipes();
        }

        // Gerenciamento de receitas
        bool addRecipe(const CraftingRecipe& recipe) {
            if (recipeDatabase.find(recipe.id) != recipeDatabase.end()) {
                return false;
            }

            recipeDatabase[recipe.id] = recipe;
            return true;
        }

        bool removeRecipe(DWORD recipeId) {
            return recipeDatabase.erase(recipeId) > 0;
        }

        // Sistema de crafting
        bool craftItem(DWORD characterId, DWORD recipeId) {
            // Verificar se a receita existe
            auto recipeIt = recipeDatabase.find(recipeId);
            if (recipeIt == recipeDatabase.end()) {
                return false;
            }

            const auto& recipe = recipeIt->second;

            // Verificar nível do personagem
            auto skillIt = characterSkills.find(characterId);
            if (skillIt == characterSkills.end() || skillIt->second.level < recipe.requiredLevel) {
                return false;
            }

            // Verificar materiais
            if (!checkMaterials(characterId, recipe.materials)) {
                return false;
            }

            // Verificar custo em ouro
            if (!checkGoldCost(characterId, recipe.goldCost)) {
                return false;
            }

            // Calcular chance de sucesso
            float successChance = calculateSuccessChance(recipe, skillIt->second);
            bool success = (rand() % 100) < successChance;

            if (success) {
                // Remover materiais
                removeMaterials(characterId, recipe.materials);

                // Remover ouro
                removeGold(characterId, recipe.goldCost);

                // Adicionar item craftado
                addCraftedItem(characterId, recipe.resultItemId, recipe.resultQuantity);

                // Adicionar experiência
                addCraftingExperience(characterId, recipe);
            }

            return success;
        }

        // Sistema de habilidades
        bool addCraftingExperience(DWORD characterId, const CraftingRecipe& recipe) {
            auto& skill = characterSkills[characterId];
            skill.experience += calculateExperienceGain(recipe, skill.level);

            // Verificar level up
            if (skill.experience >= calculateExpNeeded(skill.level)) {
                return levelUpCraftingSkill(characterId);
            }
            return false;
        }

        bool levelUpCraftingSkill(DWORD characterId) {
            auto& skill = characterSkills[characterId];
            if (skill.level >= skill.maxLevel) {
                return false;
            }

            skill.level++;
            skill.experience = 0;
            return true;
        }

        // Getters
        const CraftingRecipe& getRecipe(DWORD recipeId) const {
            return recipeDatabase.at(recipeId);
        }

        const CraftingSkill& getCharacterSkill(DWORD characterId) const {
            return characterSkills.at(characterId);
        }

        std::vector<DWORD> getAvailableRecipes(DWORD characterId) const {
            std::vector<DWORD> available;
            const auto& skill = characterSkills.at(characterId);

            for (const auto& recipe : recipeDatabase) {
                if (recipe.second.requiredLevel <= skill.level) {
                    available.push_back(recipe.first);
                }
            }

            return available;
        }

    private:
        void initializeDefaultRecipes() {
            // Implementar inicialização das receitas padrão
        }

        bool checkMaterials(DWORD characterId, const std::vector<CraftingMaterial>& materials) {
            // Implementar verificação de materiais
            return true;
        }

        bool checkGoldCost(DWORD characterId, DWORD cost) {
            // Implementar verificação de ouro
            return true;
        }

        void removeMaterials(DWORD characterId, const std::vector<CraftingMaterial>& materials) {
            // Implementar remoção de materiais
        }

        void removeGold(DWORD characterId, DWORD amount) {
            // Implementar remoção de ouro
        }

        void addCraftedItem(DWORD characterId, DWORD itemId, DWORD quantity) {
            // Implementar adição de item craftado
        }

        float calculateSuccessChance(const CraftingRecipe& recipe, const CraftingSkill& skill) {
            float baseChance = recipe.successRate;
            float skillBonus = (skill.level - recipe.requiredLevel) * 2.0f;
            return std::min(100.0f, baseChance + skillBonus);
        }

        DWORD calculateExperienceGain(const CraftingRecipe& recipe, DWORD currentLevel) {
            return recipe.requiredLevel * 10;
        }

        DWORD calculateExpNeeded(DWORD level) {
            return level * 100; // Fórmula simples para exemplo
        }
    };
} 