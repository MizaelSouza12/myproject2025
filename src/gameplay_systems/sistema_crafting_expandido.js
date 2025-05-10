/**
 * WYDBR 2.0 - Implementação Gerada Automaticamente
 * Arquivo origem: sistema_crafting_expandido.txt
 * 
 * Este arquivo contém a definição C++ do sistema, encapsulada em JavaScript
 * para integração no projeto WYDBR 2.0.
 */

// Exporta o código C++ como uma string para ser processado pelo compilador C++
module.exports.cppCode = `
// CraftingSystem.h - Sistema de Crafting Expandido
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <functional>
#include <memory>
#include <optional>
#include <random>
#include <nlohmann/json.hpp>
#include "ResourceSystem.h"
#include "PlayerManager.h"
#include "SkillSystem.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;
using namespace std::chrono_literals;

enum class CraftingResultType {
    ITEM,
    RESOURCE,
    CURRENCY,
    EXPERIENCE,
    SKILL_INCREASE,
    REPUTATION,
    UNLOCK,
    CUSTOM
};

enum class CraftingStationType {
    FORGE,
    WORKBENCH,
    ALCHEMY_LAB,
    COOKING_POT,
    ENCHANTING_TABLE,
    LOOM,
    ASSEMBLY_LINE,
    ANVIL,
    CUSTOM
};

enum class RecipeQuality {
    POOR,
    COMMON,
    UNCOMMON,
    RARE,
    EPIC,
    LEGENDARY,
    MYTHIC
};

enum class CraftingProcess {
    BASIC,
    SEQUENTIAL,
    PARALLEL,
    TIERED,
    MODULAR,
    COLLABORATIVE,
    EXPERIMENTAL
};

enum class RecipeDiscoveryMethod {
    DEFAULT,
    PURCHASE,
    QUEST_REWARD,
    SKILL_UNLOCK,
    EXPERIMENTATION,
    DECONSTRUCTION,
    REPUTATION,
    EXPLORATION,
    RANDOM
};

struct CraftingRecipe {
    std::string recipeId;
    std::string name;
    std::string description;
    RecipeQuality quality;
    CraftingProcess process;
    
    struct Ingredient {
        std::string itemId;
        uint32_t quantity;
        bool consumed{true};
        bool optional{false};
        float qualityContribution{1.0f};
    };
    
    std::vector<Ingredient> ingredients;
    
    struct RecipeResult {
        CraftingResultType type;
        std::string resultId;
        uint32_t minQuantity{1};
        uint32_t maxQuantity{1};
        float baseSuccessChance{1.0f};
        float qualityModifier{1.0f};
    };
    
    std::vector<RecipeResult> results;
    
    struct RecipeRequirement {
        std::string requirementType; // "skill", "level", "reputation", "item", "location", etc.
        std::string requirementId;
        float minValue{0.0f};
        bool consume{false};
    };
    
    std::vector<RecipeRequirement> requirements;
    
    CraftingStationType stationType;
    std::chrono::seconds baseCraftingTime{5};
    float experienceReward{0.0f};
    float skillGainMultiplier{1.0f};
    std::vector<std::string> tags;
    bool discovered{false};
    RecipeDiscoveryMethod discoveryMethod{RecipeDiscoveryMethod::DEFAULT};
    json visualData;
};

struct CraftingStation {
    std::string stationId;
    std::string name;
    CraftingStationType type;
    std::vector<std::string> supportedRecipeIds;
    float craftingSpeedModifier{1.0f};
    float qualityModifier{1.0f};
    float resourceConservationChance{0.0f};
    float criticalSuccessChance{0.0f};
    uint32_t maxQueueSize{5};
    uint32_t maxUsersAtOnce{1};
    std::optional<std::string> ownerId;
    std::optional<std::string> locationId;
    std::vector<std::string> craftingBonuses;
};

struct CraftingJob {
    std::string jobId;
    std::string playerId;
    std::string recipeId;
    std::string stationId;
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    float progress{0.0f};
    bool paused{false};
    json ingredientsUsed;
    std::vector<std::string> collaboratorIds;
    float qualityModifier{1.0f};
    json jobMetadata;
};

struct CraftingResult {
    std::string resultId;
    std::string jobId;
    std::string playerId;
    std::string recipeId;
    std::chrono::system_clock::time_point completedAt;
    bool success{true};
    float quality{1.0f};
    std::vector<std::pair<CraftingResultType, std::pair<std::string, uint32_t>>> results; // type -> (id, quantity)
    float experienceGained{0.0f};
    std::unordered_map<std::string, float> skillsGained;
    std::string criticalResultType; // "normal", "critical_success", "critical_failure"
    json resultMetadata;
};

struct ArtisanSpecialization {
    std::string specializationId;
    std::string name;
    std::string description;
    CraftingStationType primaryCraftType;
    float qualityBonus{0.0f};
    float speedBonus{0.0f};
    float resourceConservationChance{0.0f};
    float criticalSuccessChance{0.0f};
    float experienceMultiplier{1.0f};
    std::vector<std::string> uniqueRecipeIds;
    uint32_t requiredSkillLevel{0};
    std::vector<std::string> requiredAchievements;
    std::vector<std::string> perks;
};

struct PlayerCraftingData {
    std::string playerId;
    std::unordered_map<std::string, bool> knownRecipes; // recipeId -> discovered
    std::vector<std::string> specializationIds;
    std::unordered_map<CraftingStationType, uint32_t> craftingExperience;
    std::unordered_map<CraftingStationType, uint32_t> itemsCrafted;
    std::unordered_map<std::string, uint32_t> recipesCrafted; // recipeId -> count
    std::set<std::string> masteredRecipes;
    json craftingStats;
};

struct RecipeDiscovery {
    std::string discoveryId;
    std::string recipeId;
    RecipeDiscoveryMethod method;
    json discoveryRequirements;
    float discoveryChance{1.0f};
    bool repeatable{false};
    bool autoDiscover{false};
    std::optional<std::string> hint;
};

class CraftingSystem {
public:
    // Singleton
    static CraftingSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Gerenciamento de receitas
    std::string createRecipe(const CraftingRecipe& recipe);
    bool updateRecipe(const CraftingRecipe& recipe);
    bool deleteRecipe(const std::string& recipeId);
    CraftingRecipe getRecipe(const std::string& recipeId) const;
    std::vector<CraftingRecipe> getAllRecipes() const;
    std::vector<CraftingRecipe> getRecipesByTag(const std::string& tag) const;
    std::vector<CraftingRecipe> getRecipesByQuality(RecipeQuality quality) const;
    std::vector<CraftingRecipe> getRecipesByStationType(CraftingStationType stationType) const;
    
    // Gerenciamento de estações
    std::string createCraftingStation(const CraftingStation& station);
    bool updateCraftingStation(const CraftingStation& station);
    bool deleteCraftingStation(const std::string& stationId);
    CraftingStation getCraftingStation(const std::string& stationId) const;
    std::vector<CraftingStation> getAllCraftingStations() const;
    std::vector<CraftingStation> getCraftingStationsByType(CraftingStationType type) const;
    std::vector<CraftingStation> getCraftingStationsByOwner(const std::string& ownerId) const;
    
    // Especializações de artesão
    std::string createArtisanSpecialization(const ArtisanSpecialization& specialization);
    bool updateArtisanSpecialization(const ArtisanSpecialization& specialization);
    bool deleteArtisanSpecialization(const std::string& specializationId);
    ArtisanSpecialization getArtisanSpecialization(const std::string& specializationId) const;
    std::vector<ArtisanSpecialization> getAllArtisanSpecializations() const;
    
    // Dados de crafting do jogador
    PlayerCraftingData getPlayerCraftingData(const std::string& playerId) const;
    bool addRecipeToPlayer(const std::string& playerId, const std::string& recipeId);
    bool removeRecipeFromPlayer(const std::string& playerId, const std::string& recipeId);
    bool addSpecializationToPlayer(const std::string& playerId, const std::string& specializationId);
    bool removeSpecializationFromPlayer(const std::string& playerId, const std::string& specializationId);
    std::vector<CraftingRecipe> getPlayerKnownRecipes(const std::string& playerId) const;
    
    // Descoberta de receitas
    std::string createRecipeDiscovery(const RecipeDiscovery& discovery);
    bool updateRecipeDiscovery(const RecipeDiscovery& discovery);
    bool deleteRecipeDiscovery(const std::string& discoveryId);
    RecipeDiscovery getRecipeDiscovery(const std::string& discoveryId) const;
    std::vector<RecipeDiscovery> getAllRecipeDiscoveries() const;
    bool checkRecipeDiscovery(const std::string& playerId, const std::string& recipeId);
    bool triggerRecipeDiscovery(const std::string& playerId, RecipeDiscoveryMethod method, const json& context = json());
    
    // Jobs de crafting
    std::string startCraftingJob(const std::string& playerId, 
                               const std::string& recipeId, 
                               const std::string& stationId,
                               const json& ingredientsOverride = json());
    
    bool cancelCraftingJob(const std::string& jobId);
    bool pauseCraftingJob(const std::string& jobId);
    bool resumeCraftingJob(const std::string& jobId);
    bool updateCraftingProgress(const std::string& jobId, float progress);
    CraftingJob getCraftingJob(const std::string& jobId) const;
    std::vector<CraftingJob> getPlayerCraftingJobs(const std::string& playerId) const;
    std::vector<CraftingJob> getStationCraftingJobs(const std::string& stationId) const;
    
    // Colaboração de crafting
    bool addCollaboratorToJob(const std::string& jobId, const std::string& collaboratorId);
    bool removeCollaboratorFromJob(const std::string& jobId, const std::string& collaboratorId);
    std::vector<std::string> getJobCollaborators(const std::string& jobId) const;
    
    // Finalização e resultados
    CraftingResult completeCraftingJob(const std::string& jobId);
    CraftingResult getCraftingResult(const std::string& resultId) const;
    std::vector<CraftingResult> getPlayerCraftingResults(const std::string& playerId, uint32_t limit = 100) const;
    
    // Verificações e consultas
    bool canPlayerCraft(const std::string& playerId, const std::string& recipeId, std::string& reason);
    bool hasRequiredIngredients(const std::string& playerId, const std::string& recipeId, json& missingIngredients);
    float calculateCraftingTime(const std::string& playerId, const std::string& recipeId, const std::string& stationId) const;
    float calculateSuccessChance(const std::string& playerId, const std::string& recipeId, const std::string& stationId) const;
    float calculateResultQuality(const std::string& playerId, const std::string& recipeId, const std::string& stationId) const;
    
    // Estatísticas e análise
    json getCraftingStats() const;
    json getPlayerCraftingStats(const std::string& playerId) const;
    json getPopularRecipes(uint32_t limit = 10) const;
    
    // Callbacks
    void registerRecipeDiscoveredCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerCraftingStartedCallback(std::function<void(const CraftingJob&)> callback);
    void registerCraftingCompletedCallback(std::function<void(const CraftingResult&)> callback);
    void registerSpecializationUnlockedCallback(std::function<void(const std::string&, const std::string&)> callback);
    
private:
    CraftingSystem();
    ~CraftingSystem();
    
    // Atributos privados
    static std::unique_ptr<CraftingSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<Player::PlayerManager> m_playerManager;
    std::shared_ptr<Economy::ResourceSystem> m_resourceSystem;
    std::shared_ptr<Character::SkillSystem> m_skillSystem;
    
    // Gerador de número aleatório
    std::mt19937 m_rng;
    
    // Receitas
    std::unordered_map<std::string, CraftingRecipe> m_recipes;
    std::unordered_map<std::string, std::vector<std::string>> m_recipesByTag;
    std::unordered_map<RecipeQuality, std::vector<std::string>> m_recipesByQuality;
    std::unordered_map<CraftingStationType, std::vector<std::string>> m_recipesByStation;
    mutable std::shared_mutex m_recipesMutex;
    
    // Estações de crafting
    std::unordered_map<std::string, CraftingStation> m_craftingStations;
    std::unordered_map<CraftingStationType, std::vector<std::string>> m_stationsByType;
    std::unordered_map<std::string, std::vector<std::string>> m_stationsByOwner;
    std::unordered_map<std::string, std::vector<std::string>> m_stationsByLocation;
    mutable std::shared_mutex m_stationsMutex;
    
    // Especializações
    std::unordered_map<std::string, ArtisanSpecialization> m_specializations;
    std::unordered_map<CraftingStationType, std::vector<std::string>> m_specializationsByType;
    mutable std::mutex m_specializationsMutex;
    
    // Dados de jogadores
    std::unordered_map<std::string, PlayerCraftingData> m_playerCraftingData;
    mutable std::shared_mutex m_playerDataMutex;
    
    // Descobertas de receitas
    std::unordered_map<std::string, RecipeDiscovery> m_recipeDiscoveries;
    std::unordered_map<std::string, std::vector<std::string>> m_discoveriesByRecipe;
    std::unordered_map<RecipeDiscoveryMethod, std::vector<std::string>> m_discoveriesByMethod;
    mutable std::mutex m_discoveriesMutex;
    
    // Jobs de crafting
    std::unordered_map<std::string, CraftingJob> m_craftingJobs;
    std::unordered_map<std::string, std::vector<std::string>> m_jobsByPlayer;
    std::unordered_map<std::string, std::vector<std::string>> m_jobsByStation;
    mutable std::shared_mutex m_jobsMutex;
    
    // Resultados de crafting
    std::unordered_map<std::string, CraftingResult> m_craftingResults;
    std::unordered_map<std::string, std::vector<std::string>> m_resultsByPlayer;
    mutable std::mutex m_resultsMutex;
    
    // Colaboradores
    std::unordered_map<std::string, std::set<std::string>> m_jobCollaborators; // jobId -> set of playerIds
    mutable std::mutex m_collaboratorsMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const std::string&, const std::string&)>> m_recipeDiscoveredCallbacks;
    std::vector<std::function<void(const CraftingJob&)>> m_craftingStartedCallbacks;
    std::vector<std::function<void(const CraftingResult&)>> m_craftingCompletedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&)>> m_specializationUnlockedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas
    struct CraftingStats {
        std::atomic<uint64_t> totalCraftingAttempts{0};
        std::atomic<uint64_t> successfulCrafts{0};
        std::atomic<uint64_t> failedCrafts{0};
        std::atomic<uint64_t> criticalSuccesses{0};
        std::atomic<uint64_t> criticalFailures{0};
        std::atomic<uint64_t> recipeDiscoveries{0};
        std::atomic<uint64_t> totalResourcesUsed{0};
        std::atomic<uint64_t> totalItemsCrafted{0};
        std::unordered_map<std::string, std::atomic<uint32_t>> craftsByRecipe;
    };
    
    CraftingStats m_stats;
    
    // Configuração
    float m_baseCriticalSuccessChance{0.05f};
    float m_baseCriticalFailureChance{0.02f};
    float m_qualityVariance{0.2f};
    std::chrono::seconds m_jobExpirationTime{3600}; // 1 hour
    uint32_t m_maxCraftingJobsPerPlayer{5};
    uint32_t m_maxCollaboratorsPerJob{3};
    bool m_enableCollaborativeCrafting{true};
    bool m_enableSpecializationRequirements{true};
    bool m_enableRandomRecipeDiscovery{true};
    std::chrono::seconds m_craftingUpdateInterval{1};
    float m_craftingProgressPerUpdate{0.01f};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_craftingProgressThread;
    std::thread m_jobExpirationThread;
    std::thread m_randomDiscoveryThread;
    
    // Métodos privados
    std::string generateRecipeId();
    std::string generateStationId();
    std::string generateSpecializationId();
    std::string generateJobId();
    std::string generateResultId();
    std::string generateDiscoveryId();
    
    void craftingProgressThread();
    void jobExpirationThread();
    void randomDiscoveryThread();
    
    bool validateRecipe(const CraftingRecipe& recipe) const;
    bool validateCraftingStation(const CraftingStation& station) const;
    bool validateSpecialization(const ArtisanSpecialization& specialization) const;
    
    bool checkJobRequirements(const std::string& playerId, const std::string& recipeId, std::string& failReason);
    bool consumeIngredients(const std::string& playerId, const CraftingRecipe& recipe, const json& ingredientsOverride);
    bool validateStationAvailability(const std::string& stationId) const;
    
    float calculatePlayerSkillBonus(const std::string& playerId, const std::string& recipeId) const;
    float calculateStationBonus(const std::string& stationId, const std::string& recipeId) const;
    float calculateSpecializationBonus(const std::string& playerId, const CraftingStationType stationType) const;
    
    void updateCraftingJobs();
    void calculateCraftingJobProgress(CraftingJob& job);
    
    CraftingResult generateCraftingResult(const CraftingJob& job);
    bool distributeItems(const CraftingResult& result);
    void updatePlayerCraftingStats(const std::string& playerId, const CraftingResult& result);
    
    PlayerCraftingData ensurePlayerCraftingData(const std::string& playerId);
    
    void notifyRecipeDiscovered(const std::string& playerId, const std::string& recipeId);
    void notifyCraftingStarted(const CraftingJob& job);
    void notifyCraftingCompleted(const CraftingResult& result);
    void notifySpecializationUnlocked(const std::string& playerId, const std::string& specializationId);
    
    void updateRecipeIndices(const CraftingRecipe& recipe, bool remove = false);
    void updateStationIndices(const CraftingStation& station, bool remove = false);
    
    void checkForMastery(const std::string& playerId, const std::string& recipeId);
    void checkForSpecializationUnlocks(const std::string& playerId);
    
    float getQualityModifierRange(RecipeQuality quality) const;
    bool attemptRandomDiscovery(const std::string& playerId);
    bool meetDiscoveryRequirements(const std::string& playerId, const RecipeDiscovery& discovery, const json& context) const;
};

// Implementação inline básica do singleton
inline CraftingSystem& CraftingSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<CraftingSystem>(new CraftingSystem());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR

// RecipeDependencySystem.h - Receitas Interdependentes
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include "CraftingSystem.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;

enum class DependencyType {
    PREREQUISITE,
    UPGRADE,
    VARIANT,
    COMPONENT,
    COMPLEMENTARY,
    EXCLUSIVE
};

enum class RecipeTier {
    BASE,
    INTERMEDIATE,
    ADVANCED,
    EXPERT,
    MASTER,
    GRANDMASTER,
    LEGENDARY
};

struct RecipeDependency {
    std::string dependencyId;
    std::string sourceRecipeId;
    std::string targetRecipeId;
    DependencyType type;
    bool bidirectional{false};
    std::string description;
    json dependencyParams;
};

struct RecipeTree {
    std::string rootRecipeId;
    std::unordered_map<std::string, std::vector<std::string>> childrenByParent;
    std::unordered_map<std::string, std::string> parentByChild;
    std::unordered_map<std::string, DependencyType> dependencyTypes;
    std::set<std::string> recipeIds;
};

struct RecipeTierSystem {
    std::string tierSystemId;
    std::string name;
    std::unordered_map<RecipeTier, std::vector<std::string>> recipesByTier;
    std::unordered_map<std::string, RecipeTier> tierByRecipe;
    std::unordered_map<RecipeTier, std::unordered_map<std::string, uint32_t>> requirementsByTier;
};

class RecipeDependencySystem {
public:
    // Singleton
    static RecipeDependencySystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    
    // Gerenciamento de dependências
    std::string createDependency(const RecipeDependency& dependency);
    bool updateDependency(const RecipeDependency& dependency);
    bool deleteDependency(const std::string& dependencyId);
    RecipeDependency getDependency(const std::string& dependencyId) const;
    std::vector<RecipeDependency> getAllDependencies() const;
    
    // Consulta por receita
    std::vector<RecipeDependency> getRecipeDependencies(const std::string& recipeId, bool asSource = true) const;
    std::vector<std::string> getPrerequisiteRecipes(const std::string& recipeId) const;
    std::vector<std::string> getDependentRecipes(const std::string& recipeId) const;
    
    // Árvores de receitas
    RecipeTree getRecipeTree(const std::string& rootRecipeId) const;
    RecipeTree getFullDependencyGraph() const;
    std::vector<std::string> findRecipePath(const std::string& startRecipeId, const std::string& endRecipeId) const;
    
    // Tiers de receitas
    std::string createTierSystem(const RecipeTierSystem& tierSystem);
    bool updateTierSystem(const RecipeTierSystem& tierSystem);
    bool deleteTierSystem(const std::string& tierSystemId);
    RecipeTierSystem getTierSystem(const std::string& tierSystemId) const;
    
    bool assignRecipeToTier(const std::string& tierSystemId, const std::string& recipeId, RecipeTier tier);
    RecipeTier getRecipeTier(const std::string& tierSystemId, const std::string& recipeId) const;
    std::vector<std::string> getRecipesByTier(const std::string& tierSystemId, RecipeTier tier) const;
    
    // Verificação e análise
    bool checkDependenciesSatisfied(const std::string& recipeId, const std::string& playerId) const;
    std::vector<std::string> getUnsatisfiedDependencies(const std::string& recipeId, const std::string& playerId) const;
    std::vector<std::string> suggestNextRecipes(const std::string& playerId) const;
    json analyzeRecipeDependencyComplexity() const;
    
private:
    RecipeDependencySystem();
    ~RecipeDependencySystem();
    
    // Atributos privados
    static std::unique_ptr<RecipeDependencySystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<CraftingSystem> m_craftingSystem;
    
    // Dependências
    std::unordered_map<std::string, RecipeDependency> m_dependencies;
    std::unordered_map<std::string, std::vector<std::string>> m_dependenciesBySource; // recipeId -> [dependencyIds]
    std::unordered_map<std::string, std::vector<std::string>> m_dependenciesByTarget; // recipeId -> [dependencyIds]
    std::unordered_map<DependencyType, std::vector<std::string>> m_dependenciesByType;
    mutable std::shared_mutex m_dependenciesMutex;
    
    // Sistemas de tiers
    std::unordered_map<std::string, RecipeTierSystem> m_tierSystems;
    mutable std::mutex m_tierSystemsMutex;
    
    // Métodos privados
    std::string generateDependencyId();
    std::string generateTierSystemId();
    
    bool validateDependency(const RecipeDependency& dependency) const;
    void updateDependencyIndices(const RecipeDependency& dependency, bool remove = false);
    
    RecipeTree buildTreeFromRoot(const std::string& rootId, const std::set<std::string>& visited = {}) const;
    std::vector<std::string> findPathDFS(const std::string& currentId, const std::string& targetId, 
                                       std::set<std::string>& visited, std::vector<std::string>& currentPath) const;
    
    bool isCyclicHelper(const std::string& recipeId, std::set<std::string>& visited, 
                      std::set<std::string>& recursionStack) const;
    bool hasCyclicDependencies() const;
};

// Implementação inline básica do singleton
inline RecipeDependencySystem& RecipeDependencySystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<RecipeDependencySystem>(new RecipeDependencySystem());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR

// QualityVariationSystem.h - Qualidade Variável Baseada em Skill
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <random>
#include <nlohmann/json.hpp>
#include "CraftingSystem.h"
#include "SkillSystem.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;

enum class QualityTier {
    TERRIBLE,
    POOR,
    STANDARD,
    FINE,
    SUPERIOR,
    EXCELLENT,
    MASTERWORK,
    PERFECT
};

enum class QualityAttributeType {
    DAMAGE,
    DEFENSE,
    DURABILITY,
    WEIGHT,
    VALUE,
    EFFECT_POTENCY,
    EFFECT_DURATION,
    STAT_BONUS,
    APPEARANCE,
    CUSTOM
};

struct QualityDefinition {
    std::string qualityId;
    QualityTier tier;
    float minQualityValue;
    float maxQualityValue;
    std::string displayName;
    std::string description;
    float valueFactor{1.0f};
    json visualModifiers;
    std::unordered_map<QualityAttributeType, float> attributeModifiers;
};

struct QualitySkillMapping {
    std::string mappingId;
    CraftingStationType craftingType;
    RecipeQuality recipeQuality;
    std::string primarySkillId;
    float primarySkillWeight{1.0f};
    std::unordered_map<std::string, float> secondarySkills; // skillId -> weight
    float baseQualityFactor{1.0f};
    float qualityVarianceFactor{0.2f};
    float criticalSuccessBonusFactor{0.3f};
};

struct ItemQualityData {
    std::string itemId;
    QualityTier tier;
    float qualityValue;
    std::unordered_map<QualityAttributeType, float> attributeValues;
    std::string crafterId;
    std::string qualityDescription;
    std::chrono::system_clock::time_point createdAt;
    bool signedByCrafter{false};
    json qualityMetadata;
};

class QualityVariationSystem {
public:
    // Singleton
    static QualityVariationSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    
    // Definições de qualidade
    std::string createQualityDefinition(const QualityDefinition& definition);
    bool updateQualityDefinition(const QualityDefinition& definition);
    bool deleteQualityDefinition(const std::string& qualityId);
    QualityDefinition getQualityDefinition(const std::string& qualityId) const;
    QualityDefinition getQualityDefinitionByTier(QualityTier tier) const;
    std::vector<QualityDefinition> getAllQualityDefinitions() const;
    
    // Mapeamentos de skill para qualidade
    std::string createQualitySkillMapping(const QualitySkillMapping& mapping);
    bool updateQualitySkillMapping(const QualitySkillMapping& mapping);
    bool deleteQualitySkillMapping(const std::string& mappingId);
    QualitySkillMapping getQualitySkillMapping(const std::string& mappingId) const;
    QualitySkillMapping getQualitySkillMappingByType(CraftingStationType craftingType, 
                                                   RecipeQuality recipeQuality) const;
    std::vector<QualitySkillMapping> getAllQualitySkillMappings() const;
    
    // Dados de qualidade de itens
    std::string createItemQuality(const ItemQualityData& qualityData);
    bool updateItemQuality(const ItemQualityData& qualityData);
    bool deleteItemQuality(const std::string& itemId);
    ItemQualityData getItemQualityData(const std::string& itemId) const;
    std::vector<ItemQualityData> getItemsWithQualityTier(QualityTier tier) const;
    std::vector<ItemQualityData> getItemsCraftedByPlayer(const std::string& playerId) const;
    
    // Cálculos de qualidade
    float calculateQualityValue(const std::string& playerId, const std::string& recipeId, 
                              const std::string& stationId, bool isCriticalSuccess = false);
    
    QualityTier determineQualityTier(float qualityValue) const;
    
    std::unordered_map<QualityAttributeType, float> calculateItemAttributes(const std::string& recipeId, 
                                                                          QualityTier tier, 
                                                                          float qualityValue) const;
    
    float calculateItemValueModifier(QualityTier tier) const;
    
    // Assinaturas de artesão
    bool signItemAsCrafter(const std::string& itemId, const std::string& crafterId);
    bool verifyItemSignature(const std::string& itemId) const;
    std::string getCrafterSignature(const std::string& itemId) const;
    
    // Utilitários
    std::string getQualityDescription(const std::string& itemId) const;
    std::string generateQualityDescriptor(QualityTier tier, float qualityValue, CraftingStationType craftingType) const;
    json getQualityVisualModifiers(const std::string& itemId) const;
    
private:
    QualityVariationSystem();
    ~QualityVariationSystem();
    
    // Atributos privados
    static std::unique_ptr<QualityVariationSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<CraftingSystem> m_craftingSystem;
    std::shared_ptr<Character::SkillSystem> m_skillSystem;
    
    // Gerador aleatório
    std::mt19937 m_rng;
    
    // Definições de qualidade
    std::unordered_map<std::string, QualityDefinition> m_qualityDefinitions;
    std::unordered_map<QualityTier, std::string> m_definitionsByTier;
    mutable std::mutex m_definitionsMutex;
    
    // Mapeamentos de skill
    std::unordered_map<std::string, QualitySkillMapping> m_skillMappings;
    std::map<std::pair<CraftingStationType, RecipeQuality>, std::string> m_mappingsByType;
    mutable std::mutex m_mappingsMutex;
    
    // Qualidade de itens
    std::unordered_map<std::string, ItemQualityData> m_itemQualities;
    std::unordered_map<QualityTier, std::vector<std::string>> m_itemsByTier; // tier -> [itemIds]
    std::unordered_map<std::string, std::vector<std::string>> m_itemsByCrafter; // crafterId -> [itemIds]
    mutable std::mutex m_itemQualitiesMutex;
    
    // Configuração
    float m_baseQualityMinimum{0.3f};
    float m_skillLevelQualityFactor{0.01f};
    float m_stationQualityFactor{0.15f};
    float m_ingredientQualityFactor{0.2f};
    float m_specializationQualityFactor{0.1f};
    float m_criticalSuccessQualityBonus{0.3f};
    bool m_enableCrafterSignatures{true};
    bool m_displayQualityInItemName{true};
    
    // Métodos privados
    std::string generateQualityId();
    std::string generateMappingId();
    
    float getPlayerSkillQualityContribution(const std::string& playerId, const QualitySkillMapping& mapping) const;
    float getStationQualityContribution(const std::string& stationId) const;
    float getSpecializationQualityContribution(const std::string& playerId, CraftingStationType craftingType) const;
    
    void updateItemQualityIndices(const ItemQualityData& qualityData, bool remove = false);
    
    bool validateQualityDefinition(const QualityDefinition& definition) const;
    bool validateQualitySkillMapping(const QualitySkillMapping& mapping) const;
    
    QualityTier getNextQualityTier(QualityTier currentTier) const;
    QualityTier getPreviousQualityTier(QualityTier currentTier) const;
    
    float normalizeQualityValue(float rawValue) const;
    float getBaseCraftingQuality(RecipeQuality recipeQuality) const;
};

// Implementação inline básica do singleton
inline QualityVariationSystem& QualityVariationSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<QualityVariationSystem>(new QualityVariationSystem());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR

// ArtisanSpecializationSystem.h - Especialização de Artesãos
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>
#include "CraftingSystem.h"
#include "SkillSystem.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;

enum class MasteryType {
    GENERALIST,
    SPECIALIST,
    INNOVATOR,
    TRADITIONALIST,
    EXPERIMENTAL,
    PERFECTIONIST,
    EFFICIENT,
    CUSTOM
};

enum class SpecializationTier {
    APPRENTICE,
    JOURNEYMAN,
    EXPERT,
    MASTER,
    GRANDMASTER,
    LEGENDARY
};

struct ArtisanMastery {
    std::string masteryId;
    std::string name;
    std::string description;
    MasteryType type;
    CraftingStationType primaryCraftingType;
    std::vector<std::string> requiredRecipes;
    uint32_t requiredRecipesMastered{0};
    uint32_t requiredItemsCrafted{0};
    uint32_t requiredSkillLevel{0};
    std::vector<std::string> incompatibleMasteries;
    std::vector<std::string> unlockableRecipes;
    std::unordered_map<std::string, float> bonuses; // bonusType -> value
    std::chrono::seconds craftingTimeReduction{0};
    float resourceConsumptionReduction{0.0f};
    float qualityBonus{0.0f};
    json masteryPerks;
};

struct PlayerSpecializationData {
    std::string playerId;
    std::vector<std::string> masteryIds;
    std::unordered_map<CraftingStationType, uint32_t> specializationProgress;
    std::unordered_map<CraftingStationType, SpecializationTier> specializationTiers;
    std::unordered_map<std::string, uint32_t> masteryProgress; // masteryId -> progress
    std::unordered_map<std::string, std::chrono::system_clock::time_point> masteryUnlockTimes;
    std::set<std::string> masteredRecipes;
    json specializationStats;
};

struct MasteryRequirement {
    std::string requirementId;
    std::string masteryId;
    std::string requirementType; // "recipe_count", "skill_level", "specific_recipe", etc.
    json requirementParams;
    uint32_t requiredValue;
    std::string description;
};

struct SpecializationTierDefinition {
    SpecializationTier tier;
    uint32_t requiredTotalCrafts;
    uint32_t requiredUniqueRecipes;
    uint32_t requiredSkillLevel;
    float craftingTimeReduction;
    float resourceConservationChance;
    float qualityBonus;
    std::vector<std::string> unlockedMasteries;
    json tierBenefits;
};

class ArtisanSpecializationSystem {
public:
    // Singleton
    static ArtisanSpecializationSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    
    // Gerenciamento de masteries
    std::string createArtisanMastery(const ArtisanMastery& mastery);
    bool updateArtisanMastery(const ArtisanMastery& mastery);
    bool deleteArtisanMastery(const std::string& masteryId);
    ArtisanMastery getArtisanMastery(const std::string& masteryId) const;
    std::vector<ArtisanMastery> getAllArtisanMasteries() const;
    std::vector<ArtisanMastery> getMasteriesByCraftingType(CraftingStationType craftingType) const;
    std::vector<ArtisanMastery> getMasteriesByType(MasteryType type) const;
    
    // Requisitos de mastery
    std::string createMasteryRequirement(const MasteryRequirement& requirement);
    bool updateMasteryRequirement(const MasteryRequirement& requirement);
    bool deleteMasteryRequirement(const std::string& requirementId);
    MasteryRequirement getMasteryRequirement(const std::string& requirementId) const;
    std::vector<MasteryRequirement> getMasteryRequirements(const std::string& masteryId) const;
    
    // Tiers de especialização
    void setSpecializationTierDefinition(SpecializationTier tier, const SpecializationTierDefinition& definition);
    SpecializationTierDefinition getSpecializationTierDefinition(SpecializationTier tier) const;
    std::vector<SpecializationTierDefinition> getAllTierDefinitions() const;
    
    // Gerenciamento de jogadores
    PlayerSpecializationData getPlayerSpecializationData(const std::string& playerId) const;
    bool unlockMasteryForPlayer(const std::string& playerId, const std::string& masteryId);
    bool removeMasteryFromPlayer(const std::string& playerId, const std::string& masteryId);
    SpecializationTier getPlayerSpecializationTier(const std::string& playerId, CraftingStationType craftingType) const;
    bool updatePlayerMasteryProgress(const std::string& playerId, const std::string& masteryId, uint32_t progress);
    
    // Verificações e atributos
    bool canPlayerUnlockMastery(const std::string& playerId, const std::string& masteryId, std::string& reason) const;
    std::vector<std::string> getUnlockableMasteries(const std::string& playerId) const;
    float getPlayerQualityBonus(const std::string& playerId, CraftingStationType craftingType) const;
    float getPlayerResourceConservationChance(const std::string& playerId, CraftingStationType craftingType) const;
    std::chrono::seconds getPlayerCraftingTimeReduction(const std::string& playerId, CraftingStationType craftingType) const;
    
    // Atualizações de progresso
    void processCraftingResult(const std::string& playerId, const std::string& recipeId, bool success);
    void updatePlayerSpecializationProgress(const std::string& playerId, CraftingStationType craftingType, uint32_t amount = 1);
    void checkForTierUpgrades(const std::string& playerId);
    void checkForMasteryUnlocks(const std::string& playerId);
    
    // Receitas exclusive
    std::vector<std::string> getPlayerExclusiveRecipes(const std::string& playerId) const;
    std::vector<std::string> getMasteryExclusiveRecipes(const std::string& masteryId) const;
    bool isRecipeExclusive(const std::string& recipeId) const;
    
    // Callbacks
    void registerMasteryUnlockedCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerSpecializationTierChangedCallback(std::function<void(const std::string&, CraftingStationType, SpecializationTier)> callback);
    
private:
    ArtisanSpecializationSystem();
    ~ArtisanSpecializationSystem();
    
    // Atributos privados
    static std::unique_ptr<ArtisanSpecializationSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<CraftingSystem> m_craftingSystem;
    std::shared_ptr<Character::SkillSystem> m_skillSystem;
    
    // Masteries
    std::unordered_map<std::string, ArtisanMastery> m_masteries;
    std::unordered_map<CraftingStationType, std::vector<std::string>> m_masteriesByCraftingType;
    std::unordered_map<MasteryType, std::vector<std::string>> m_masteriesByType;
    mutable std::shared_mutex m_masteriesMutex;
    
    // Requisitos
    std::unordered_map<std::string, MasteryRequirement> m_masteryRequirements;
    std::unordered_map<std::string, std::vector<std::string>> m_requirementsByMastery; // masteryId -> [requirementIds]
    mutable std::mutex m_requirementsMutex;
    
    // Definições de tier
    std::unordered_map<SpecializationTier, SpecializationTierDefinition> m_tierDefinitions;
    mutable std::mutex m_tierDefinitionsMutex;
    
    // Dados de especialização de jogadores
    std::unordered_map<std::string, PlayerSpecializationData> m_playerSpecializations;
    mutable std::shared_mutex m_playerDataMutex;
    
    // Receitas exclusivas
    std::unordered_map<std::string, std::string> m_exclusiveRecipes; // recipeId -> masteryId
    mutable std::mutex m_exclusiveRecipesMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const std::string&, const std::string&)>> m_masteryUnlockedCallbacks;
    std::vector<std::function<void(const std::string&, CraftingStationType, SpecializationTier)>> m_tierChangedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Configuração
    uint32_t m_maxMasteriesPerPlayer{5};
    uint32_t m_maxMasteriesPerCraftingType{2};
    uint32_t m_recipeMasteryThreshold{50}; // crafts to master a recipe
    bool m_enforceIncompatibilities{true};
    bool m_autoCheckForUnlocks{true};
    bool m_progressiveTierBonuses{true};
    
    // Métodos privados
    std::string generateMasteryId();
    std::string generateRequirementId();
    
    bool validateMastery(const ArtisanMastery& mastery) const;
    bool validateMasteryRequirement(const MasteryRequirement& requirement) const;
    bool validateTierDefinition(const SpecializationTierDefinition& definition) const;
    
    PlayerSpecializationData ensurePlayerSpecialization(const std::string& playerId);
    void updateMasteryIndices(const ArtisanMastery& mastery, bool remove = false);
    
    bool checkMasteryRequirement(const std::string& playerId, const MasteryRequirement& requirement) const;
    bool checkAllMasteryRequirements(const std::string& playerId, const std::string& masteryId) const;
    
    bool upgradeTier(const std::string& playerId, CraftingStationType craftingType, SpecializationTier newTier);
    bool isIncompatibleMastery(const std::string& masteryId, const std::vector<std::string>& currentMasteries) const;
    
    float calculateCombinedMasteryBonus(const std::string& playerId, CraftingStationType craftingType, 
                                     const std::string& bonusType) const;
    
    void notifyMasteryUnlocked(const std::string& playerId, const std::string& masteryId);
    void notifySpecializationTierChanged(const std::string& playerId, CraftingStationType craftingType, SpecializationTier newTier);
};

// Implementação inline básica do singleton
inline ArtisanSpecializationSystem& ArtisanSpecializationSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<ArtisanSpecializationSystem>(new ArtisanSpecializationSystem());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR

// RecipeDiscoverySystem.h - Sistema de Descoberta de Receitas
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <memory>
#include <functional>
#include <optional>
#include <random>
#include <nlohmann/json.hpp>
#include "CraftingSystem.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;

enum class ExperimentationType {
    INGREDIENT_SUBSTITUTION,
    PROCESS_MODIFICATION,
    TOOL_VARIATION,
    MEASUREMENT_CHANGE,
    ENVIRONMENTAL_FACTOR,
    RANDOM_COMBINATION,
    CUSTOM
};

enum class DiscoveryRewardType {
    RECIPE,
    ITEM,
    RESOURCE,
    CURRENCY,
    EXPERIENCE,
    SKILL_POINT,
    REPUTATION,
    CUSTOM
};

struct ExperimentResult {
    std::string resultId;
    std::string playerId;
    ExperimentationType type;
    std::string baseRecipeId;
    json experimentParameters;
    std::chrono::system_clock::time_point performedAt;
    bool successful{false};
    std::optional<std::string> discoveredRecipeId;
    float noveltyScore{0.0f};
    float qualityFactor{1.0f};
    std::string resultDescription;
    json resultDetails;
};

struct RecipeClue {
    std::string clueId;
    std::string recipeId;
    std::string clueText;
    uint32_t clueLevel{1}; // 1 = vague, 10 = explicit
    std::vector<std::string> requiredClues;
    std::optional<std::string> locationId;
    std::optional<std::string> npcId;
    float discoveryChanceBonus{0.1f};
    std::string clueType; // "text", "image", "riddle", "ingredient_list", etc.
    json clueData;
};

struct DiscoveryActivity {
    std::string activityId;
    std::string name;
    std::string description;
    std::string activityType; // "deconstruction", "research", "ingredient_analysis", etc.
    std::chrono::seconds duration;
    std::unordered_map<std::string, uint32_t> resourceCosts;
    float discoveryChance{0.1f};
    float skillGainMultiplier{1.0f};
    std::vector<std::string> potentialDiscoveries;
    uint32_t cluesRevealed{1};
    std::vector<std::string> requirements;
    json activityParams;
};

struct PlayerDiscoveryData {
    std::string playerId;
    std::set<std::string> discoveredRecipes;
    std::set<std::string> knownClues;
    uint32_t totalExperiments{0};
    uint32_t successfulExperiments{0};
    std::chrono::seconds timeSpentExperimenting{0};
    std::unordered_map<ExperimentationType, uint32_t> experimentsByType;
    std::vector<std::string> recentExperimentResults;
    std::unordered_map<std::string, float> recipeDiscoveryProgress; // recipeId -> progress (0.0-1.0)
    json discoveryStats;
};

class RecipeDiscoverySystem {
public:
    // Singleton
    static RecipeDiscoverySystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Experimentação
    std::string performExperiment(const std::string& playerId, 
                                ExperimentationType type,
                                const std::string& baseRecipeId,
                                const json& experimentParameters);
    
    ExperimentResult getExperimentResult(const std::string& resultId) const;
    std::vector<ExperimentResult> getPlayerExperimentResults(const std::string& playerId) const;
    std::vector<ExperimentResult> getExperimentResultsByType(ExperimentationType type) const;
    
    // Dicas de receitas
    std::string createRecipeClue(const RecipeClue& clue);
    bool updateRecipeClue(const RecipeClue& clue);
    bool deleteRecipeClue(const std::string& clueId);
    RecipeClue getRecipeClue(const std::string& clueId) const;
    std::vector<RecipeClue> getRecipeClues(const std::string& recipeId) const;
    
    bool giveClueToPlayer(const std::string& playerId, const std::string& clueId);
    std::vector<RecipeClue> getPlayerClues(const std::string& playerId) const;
    float getPlayerDiscoveryChanceBonus(const std::string& playerId, const std::string& recipeId) const;
    
    // Atividades de descoberta
    std::string createDiscoveryActivity(const DiscoveryActivity& activity);
    bool updateDiscoveryActivity(const DiscoveryActivity& activity);
    bool deleteDiscoveryActivity(const std::string& activityId);
    DiscoveryActivity getDiscoveryActivity(const std::string& activityId) const;
    std::vector<DiscoveryActivity> getAllDiscoveryActivities() const;
    
    std::string startDiscoveryActivity(const std::string& playerId, const std::string& activityId);
    bool cancelDiscoveryActivity(const std::string& playerId, const std::string& activityId);
    std::vector<std::string> getActivePlayerActivities(const std::string& playerId) const;
    std::pair<bool, std::vector<std::string>> completeDiscoveryActivity(const std::string& playerId, const std::string& activityId);
    
    // Dados de jogador
    PlayerDiscoveryData getPlayerDiscoveryData(const std::string& playerId) const;
    bool discoverRecipe(const std::string& playerId, const std::string& recipeId);
    bool incrementRecipeDiscoveryProgress(const std::string& playerId, const std::string& recipeId, float amount);
    float getRecipeDiscoveryProgress(const std::string& playerId, const std::string& recipeId) const;
    
    // Recompensas e descobertas
    std::vector<std::string> getPlayerDiscoverableRecipes(const std::string& playerId) const;
    std::vector<std::string> getSuggestedExperiments(const std::string& playerId) const;
    json getDiscoveryRewards(const std::string& playerId, const std::string& recipeId) const;
    
    // Callbacks
    void registerExperimentPerformedCallback(std::function<void(const ExperimentResult&)> callback);
    void registerRecipeDiscoveredCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerActivityCompletedCallback(std::function<void(const std::string&, const std::string&, bool)> callback);
    
private:
    RecipeDiscoverySystem();
    ~RecipeDiscoverySystem();
    
    // Atributos privados
    static std::unique_ptr<RecipeDiscoverySystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<CraftingSystem> m_craftingSystem;
    
    // Gerador aleatório
    std::mt19937 m_rng;
    
    // Resultados de experimentos
    std::unordered_map<std::string, ExperimentResult> m_experimentResults;
    std::unordered_map<std::string, std::vector<std::string>> m_experimentsByPlayer;
    std::unordered_map<ExperimentationType, std::vector<std::string>> m_experimentsByType;
    mutable std::shared_mutex m_experimentsMutex;
    
    // Dicas de receitas
    std::unordered_map<std::string, RecipeClue> m_recipeClues;
    std::unordered_map<std::string, std::vector<std::string>> m_cluesByRecipe;
    mutable std::shared_mutex m_cluesMutex;
    
    // Atividades de descoberta
    std::unordered_map<std::string, DiscoveryActivity> m_discoveryActivities;
    mutable std::mutex m_activitiesMutex;
    
    // Atividades ativas de jogadores
    struct PlayerActivity {
        std::string playerId;
        std::string activityId;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point expectedEndTime;
        bool completed{false};
    };
    
    std::unordered_map<std::string, std::vector<PlayerActivity>> m_activePlayerActivities; // playerId -> activities
    mutable std::mutex m_activeActivitiesMutex;
    
    // Dados de descoberta de jogadores
    std::unordered_map<std::string, PlayerDiscoveryData> m_playerDiscoveryData;
    mutable std::shared_mutex m_playerDataMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const ExperimentResult&)>> m_experimentCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&)>> m_discoveryCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&, bool)>> m_activityCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Configuração
    float m_baseDiscoveryChance{0.05f};
    float m_skillDiscoveryFactor{0.001f}; // Per skill level
    float m_experimentSkillGainFactor{0.5f};
    uint32_t m_maxExperimentHistoryPerPlayer{50};
    std::chrono::minutes m_activityCheckInterval{1};
    bool m_allowDuplicateClues{false};
    uint32_t m_cluesPerRecipe{5};
    bool m_requireAllCluesForBonus{false};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_activityThread;
    
    // Métodos privados
    std::string generateResultId();
    std::string generateClueId();
    std::string generateActivityId();
    
    void activityThread();
    
    ExperimentResult processExperiment(const std::string& playerId, 
                                    ExperimentationType type,
                                    const std::string& baseRecipeId,
                                    const json& experimentParameters);
    
    std::string determineExperimentOutcome(const std::string& playerId, 
                                        ExperimentationType type,
                                        const std::string& baseRecipeId,
                                        const json& experimentParameters);
    
    float calculateDiscoveryChance(const std::string& playerId, 
                                const std::string& recipeId,
                                ExperimentationType type,
                                const json& parameters) const;
    
    std::string generateExperimentDescription(ExperimentationType type, 
                                           const std::string& baseRecipeId,
                                           const json& parameters,
                                           bool successful,
                                           const std::optional<std::string>& discoveredRecipeId) const;
    
    PlayerDiscoveryData ensurePlayerDiscoveryData(const std::string& playerId);
    void updatePlayerExperimentStats(const std::string& playerId, const ExperimentResult& result);
    
    bool validateRecipeClue(const RecipeClue& clue) const;
    bool validateDiscoveryActivity(const DiscoveryActivity& activity) const;
    
    std::vector<std::string> findDiscoverableRecipesForActivity(const std::string& activityId, const std::string& playerId) const;
    std::vector<std::string> generateCluesFromActivity(const std::string& activityId, const std::string& playerId, uint32_t count) const;
    
    void notifyExperimentPerformed(const ExperimentResult& result);
    void notifyRecipeDiscovered(const std::string& playerId, const std::string& recipeId);
    void notifyActivityCompleted(const std::string& playerId, const std::string& activityId, bool successful);
    
    void checkAndCompleteActivities();
    void processCompletedActivity(PlayerActivity& activity);
    float calculateActivitySuccessChance(const std::string& playerId, const DiscoveryActivity& activity) const;
};

// Implementação inline básica do singleton
inline RecipeDiscoverySystem& RecipeDiscoverySystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<RecipeDiscoverySystem>(new RecipeDiscoverySystem());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR
`;

// Fornece uma API JavaScript para integração no projeto
module.exports.setupSystem = function(engineContext) {
  console.log('Inicializando sistema: sistema_crafting_expandido');
  
  // Inicializa o sistema no contexto do engine
  engineContext.registerSystem({
    name: 'sistema_crafting_expandido',
    source: module.exports.cppCode,
    dependencies: getSystemDependencies()
  });

  return {
    initialize: () => {
      // Código de inicialização específico do sistema
      return true;
    },
    
    shutdown: () => {
      // Código de desligamento do sistema
      return true;
    }
  };
};

// Retorna as dependências específicas deste sistema
function getSystemDependencies() {
  // Analisar o código para determinar dependências automaticamente
  const dependencies = [];
  const code = module.exports.cppCode;
  
  // Análise básica de dependências incluídas no arquivo C++
  const includeRegex = /#include\s+["<]([^">]+)[">]/g;
  let match;
  while ((match = includeRegex.exec(code)) !== null) {
    // Ignora includes padrão do C++
    if (!match[1].startsWith('string') && 
        !match[1].startsWith('vector') && 
        !match[1].startsWith('map') &&
        !match[1].startsWith('unordered_map') &&
        !match[1].startsWith('set') &&
        !match[1].startsWith('mutex') &&
        !match[1].startsWith('shared_mutex') &&
        !match[1].startsWith('atomic') &&
        !match[1].startsWith('chrono') &&
        !match[1].startsWith('thread') &&
        !match[1].startsWith('functional') &&
        !match[1].startsWith('memory') &&
        !match[1].startsWith('optional') &&
        !match[1].startsWith('random') &&
        !match[1].startsWith('variant') &&
        !match[1].startsWith('nlohmann/json')) {
      
      dependencies.push(match[1].replace('.h', ''));
    }
  }
  
  return dependencies;
}
