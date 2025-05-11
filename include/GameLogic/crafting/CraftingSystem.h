/**
 * @file CraftingSystem.h
 * @brief Sistema de crafting do WYDBR 2.0
 */

#ifndef WYDBR_CRAFTING_SYSTEM_H
#define WYDBR_CRAFTING_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "wyd_core.h"
#include "item_struct.h"

namespace WYDBR {

/**
 * @class CraftingSystem
 * @brief Gerencia o sistema de crafting do jogo
 */
class CraftingSystem {
public:
    static CraftingSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de receitas
    bool AddRecipe(uint32_t recipeId, const std::vector<Item>& materials, const Item& result);
    bool RemoveRecipe(uint32_t recipeId);
    bool UpdateRecipe(uint32_t recipeId, const std::vector<Item>& materials, const Item& result);

    // Processamento de crafting
    bool CraftItem(uint32_t characterId, uint32_t recipeId);
    bool CancelCrafting(uint32_t characterId);
    bool CollectCraftedItem(uint32_t characterId);

    // Verificações
    bool HasRecipe(uint32_t recipeId) const;
    bool CanCraft(uint32_t characterId, uint32_t recipeId) const;
    bool HasRequiredMaterials(uint32_t characterId, uint32_t recipeId) const;
    bool HasRequiredLevel(uint32_t characterId, uint32_t recipeId) const;

    // Queries
    const Item& GetRecipeResult(uint32_t recipeId) const;
    std::vector<Item> GetRecipeMaterials(uint32_t recipeId) const;
    std::vector<uint32_t> GetAvailableRecipes(uint32_t characterId) const;
    uint32_t GetCraftingProgress(uint32_t characterId) const;

    // Utilidades
    void UpdateCraftingProgress(uint32_t characterId, uint32_t deltaTime);
    void ApplyCraftingBonus(uint32_t characterId, float bonus);
    void HandleCraftingFailure(uint32_t characterId);
    void SaveCraftingHistory(uint32_t characterId, uint32_t recipeId);

private:
    CraftingSystem() = default;
    ~CraftingSystem() = default;

    // Prevenir cópias
    CraftingSystem(const CraftingSystem&) = delete;
    CraftingSystem& operator=(const CraftingSystem&) = delete;

    // Cálculos internos
    bool ValidateRecipe(uint32_t recipeId) const;
    bool CheckCraftingRequirements(uint32_t characterId, uint32_t recipeId) const;
    void ConsumeMaterials(uint32_t characterId, uint32_t recipeId);
    void CalculateCraftingTime(uint32_t characterId, uint32_t recipeId);

    // Estado
    struct Recipe {
        std::vector<Item> materials;
        Item result;
        uint32_t craftingTime;
        uint8_t requiredLevel;
        float successRate;
    };

    struct CraftingProgress {
        uint32_t recipeId;
        uint32_t startTime;
        uint32_t remainingTime;
        bool isComplete;
    };

    std::unordered_map<uint32_t, Recipe> m_recipes;
    std::unordered_map<uint32_t, CraftingProgress> m_craftingProgress;
};

} // namespace WYDBR

#endif // WYDBR_CRAFTING_SYSTEM_H 