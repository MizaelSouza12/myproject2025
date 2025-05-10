/**
 * WYDBR 2.0 - Implementação Gerada Automaticamente
 * Arquivo origem: sistema_guerra_territorial.txt
 * 
 * Este arquivo contém a definição C++ do sistema, encapsulada em JavaScript
 * para integração no projeto WYDBR 2.0.
 */

// Exporta o código C++ como uma string para ser processado pelo compilador C++
module.exports.cppCode = `
// TerritorialWarSystem.h - Sistema de Guerra Territorial
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <deque>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <functional>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include "GameEngine.h"
#include "GuildSystem.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;
using namespace std::chrono_literals;

enum class TerritoryType {
    CITY,
    CASTLE,
    FORTRESS,
    OUTPOST,
    RESOURCE_NODE,
    SACRED_GROUND,
    VILLAGE,
    WILDERNESS,
    CUSTOM
};

enum class WarPhase {
    PEACE,
    DECLARATION,
    PREPARATION,
    BATTLE,
    CONQUEST,
    CEASEFIRE,
    AFTERMATH
};

enum class StructureType {
    WALL,
    TOWER,
    GATE,
    BARRACKS,
    ARMORY,
    TEMPLE,
    RESOURCE_COLLECTOR,
    TRAP,
    CUSTOM
};

enum class WarReward {
    TERRITORY_CONTROL,
    RESOURCE_INCOME,
    STAT_BONUS,
    SKILL_BONUS,
    EXPERIENCE_BONUS,
    REPUTATION,
    UNIQUE_ITEMS,
    CUSTOM
};

struct TerritoryDefinition {
    std::string territoryId;
    std::string name;
    TerritoryType type;
    std::vector<std::pair<float, float>> boundaries; // polygon vertices
    std::string mapId;
    uint32_t maxOccupancy;
    std::unordered_map<std::string, float> resourceRates; // resourceType -> units per hour
    std::vector<std::string> adjacentTerritories;
    uint32_t strategicValue;
    std::vector<std::string> allowedStructures;
    json specialFeatures;
    bool conquerable{true};
    uint32_t controlPoints{1000};
    json visualData;
};

struct Structure {
    std::string structureId;
    std::string name;
    StructureType type;
    std::string ownerId; // guild or player ID
    float positionX;
    float positionY;
    float positionZ;
    float rotation;
    uint32_t level{1};
    uint32_t maxHealth;
    uint32_t currentHealth;
    std::chrono::system_clock::time_point builtAt;
    std::unordered_map<std::string, uint32_t> statBonuses;
    json specialAbilities;
    bool destructible{true};
    bool active{true};
    std::vector<uint8_t> visualData;
};

struct TerritoryState {
    std::string territoryId;
    std::string controllingGuildId;
    std::chrono::system_clock::time_point controlledSince;
    uint32_t defenseStrength;
    std::map<std::string, uint32_t> resourceStockpile; // resourceType -> amount
    std::map<std::string, std::string> structures; // structureId -> structure
    uint32_t population;
    float prosperityLevel;
    float stabilityLevel;
    std::unordered_map<std::string, uint32_t> occupyingPlayers; // guildId -> count
    std::map<std::string, uint32_t> controlPointsPerGuild; // guildId -> points
    WarPhase currentPhase{WarPhase::PEACE};
    std::chrono::system_clock::time_point phaseEndTime;
    std::vector<std::string> activeWarDeclarations;
};

struct WarDeclaration {
    std::string declarationId;
    std::string attackingGuildId;
    std::string defendingGuildId;
    std::string targetTerritoryId;
    std::chrono::system_clock::time_point declaredAt;
    std::chrono::system_clock::time_point warStartTime;
    std::chrono::system_clock::time_point warEndTime;
    WarPhase currentPhase;
    json cassusBelli;
    bool accepted{false};
    bool concluded{false};
    std::string winnerGuildId;
    json warResults;
    std::set<std::string> participatingPlayersAttacker;
    std::set<std::string> participatingPlayersDefender;
};

struct WarStatistics {
    std::string guildId;
    uint32_t territoriesControlled{0};
    uint32_t territoriesCaptured{0};
    uint32_t territoriesLost{0};
    uint32_t warsWon{0};
    uint32_t warsLost{0};
    uint32_t structuresBuilt{0};
    uint32_t structuresDestroyed{0};
    uint32_t playersKilled{0};
    uint32_t playerDeaths{0};
    uint32_t totalWarDuration{0}; // in minutes
    json additionalStats;
};

struct BattleEvent {
    std::string eventId;
    std::string warDeclarationId;
    std::string territoryId;
    std::chrono::system_clock::time_point timestamp;
    std::string eventType; // "structure_built", "structure_destroyed", "player_death", "territory_capture", etc.
    std::string actorId;
    std::string targetId;
    json eventDetails;
};

class TerritorialWarSystem {
public:
    // Singleton
    static TerritorialWarSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Gerenciamento de territórios
    std::string createTerritory(const TerritoryDefinition& territory);
    bool updateTerritory(const TerritoryDefinition& territory);
    bool removeTerritory(const std::string& territoryId);
    TerritoryDefinition getTerritoryDefinition(const std::string& territoryId) const;
    std::vector<TerritoryDefinition> getAllTerritories() const;
    std::vector<TerritoryDefinition> getTerritoriesByType(TerritoryType type) const;
    
    // Controle de estruturas
    std::string buildStructure(const std::string& territoryId, 
                             const std::string& guildId, 
                             StructureType type,
                             float posX, float posY, float posZ,
                             float rotation = 0.0f);
    
    bool upgradeStructure(const std::string& structureId, uint32_t newLevel);
    bool repairStructure(const std::string& structureId, uint32_t amount);
    bool destroyStructure(const std::string& structureId, const std::string& destroyerId);
    Structure getStructure(const std::string& structureId) const;
    std::vector<Structure> getTerritoryStructures(const std::string& territoryId) const;
    
    // Estado do território
    TerritoryState getTerritoryState(const std::string& territoryId) const;
    std::vector<TerritoryState> getGuildTerritories(const std::string& guildId) const;
    bool collectTerritoryResources(const std::string& territoryId, const std::string& guildId);
    bool depositResourcesInTerritory(const std::string& territoryId, 
                                   const std::string& guildId,
                                   const std::map<std::string, uint32_t>& resources);
    
    // Declaração de guerra
    std::string declareWar(const std::string& attackingGuildId, 
                         const std::string& defendingGuildId,
                         const std::string& targetTerritoryId,
                         const json& cassusBelli = json());
    
    bool acceptWarDeclaration(const std::string& declarationId);
    bool cancelWarDeclaration(const std::string& declarationId);
    WarDeclaration getWarDeclaration(const std::string& declarationId) const;
    std::vector<WarDeclaration> getActiveWars() const;
    std::vector<WarDeclaration> getGuildWars(const std::string& guildId) const;
    
    // Mecânica de conquista
    bool captureControlPoint(const std::string& territoryId, 
                           const std::string& guildId, 
                           const std::string& playerId,
                           uint32_t amount = 1);
    
    bool abandonTerritory(const std::string& territoryId, const std::string& guildId);
    
    // Participação em guerra
    bool joinWar(const std::string& declarationId, const std::string& playerId, const std::string& guildId);
    bool leaveWar(const std::string& declarationId, const std::string& playerId);
    std::vector<std::string> getWarParticipants(const std::string& declarationId, bool attackers = true) const;
    
    // Benefícios de território
    json getTerritoryBenefits(const std::string& territoryId) const;
    std::unordered_map<std::string, float> getPlayerTerritoryBonuses(const std::string& playerId) const;
    std::unordered_map<std::string, float> getGuildTerritoryBonuses(const std::string& guildId) const;
    
    // Estatísticas
    WarStatistics getGuildWarStatistics(const std::string& guildId) const;
    std::vector<BattleEvent> getWarEvents(const std::string& declarationId, uint32_t limit = 100) const;
    json getTerritorialWarStats() const;
    
    // Controle de fases
    bool advanceWarPhase(const std::string& declarationId);
    bool setTerritoryPhase(const std::string& territoryId, WarPhase phase, std::chrono::minutes duration = 60min);
    std::chrono::system_clock::time_point getPhaseDuration(const std::string& territoryId) const;
    
    // Callbacks
    void registerTerritoryControlChangedCallback(std::function<void(const std::string&, const std::string&, const std::string&)> callback);
    void registerStructureStateChangedCallback(std::function<void(const Structure&, const std::string&)> callback);
    void registerWarPhaseChangedCallback(std::function<void(const std::string&, WarPhase)> callback);
    void registerBattleEventCallback(std::function<void(const BattleEvent&)> callback);
    
private:
    TerritorialWarSystem();
    ~TerritorialWarSystem();
    
    // Atributos privados
    static std::unique_ptr<TerritorialWarSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<Guild::GuildSystem> m_guildSystem;
    std::shared_ptr<Engine::GameEngine> m_gameEngine;
    
    // Dados de território
    std::unordered_map<std::string, TerritoryDefinition> m_territories;
    std::unordered_map<std::string, TerritoryState> m_territoryStates;
    std::unordered_map<TerritoryType, std::vector<std::string>> m_territoriesByType;
    mutable std::shared_mutex m_territoriesMutex;
    
    // Dados de estrutura
    std::unordered_map<std::string, Structure> m_structures;
    std::unordered_map<std::string, std::vector<std::string>> m_territoryStructures; // territoryId -> [structureIds]
    mutable std::shared_mutex m_structuresMutex;
    
    // Dados de guerra
    std::unordered_map<std::string, WarDeclaration> m_warDeclarations;
    std::unordered_map<std::string, std::vector<std::string>> m_activeWarsByTerritory; // territoryId -> [declarationIds]
    std::unordered_map<std::string, std::vector<std::string>> m_guildWars; // guildId -> [declarationIds]
    mutable std::shared_mutex m_warsMutex;
    
    // Estatísticas de guerra
    std::unordered_map<std::string, WarStatistics> m_guildWarStats; // guildId -> stats
    mutable std::mutex m_statsMutex;
    
    // Eventos de batalha
    std::deque<BattleEvent> m_battleEvents;
    std::unordered_map<std::string, std::vector<std::string>> m_warEvents; // declarationId -> [eventIds]
    mutable std::mutex m_eventsMutex;
    size_t m_maxBattleEvents{10000};
    
    // Callbacks registrados
    std::vector<std::function<void(const std::string&, const std::string&, const std::string&)>> m_controlChangeCallbacks;
    std::vector<std::function<void(const Structure&, const std::string&)>> m_structureCallbacks;
    std::vector<std::function<void(const std::string&, WarPhase)>> m_phaseCallbacks;
    std::vector<std::function<void(const BattleEvent&)>> m_battleEventCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas do sistema
    struct SystemStats {
        std::atomic<uint32_t> totalTerritories{0};
        std::atomic<uint32_t> totalStructures{0};
        std::atomic<uint32_t> totalWarsActive{0};
        std::atomic<uint32_t> totalWarsCompleted{0};
        std::atomic<uint32_t> totalBattleEvents{0};
        std::atomic<uint32_t> territoryOwnershipChanges{0};
        std::atomic<uint32_t> structuresBuilt{0};
        std::atomic<uint32_t> structuresDestroyed{0};
    };
    
    SystemStats m_systemStats;
    
    // Configuração
    std::chrono::minutes m_declarationPhaseLength{1440}; // 24 hours
    std::chrono::minutes m_preparationPhaseLength{1440}; // 24 hours
    std::chrono::minutes m_battlePhaseLength{120}; // 2 hours
    std::chrono::minutes m_conquestPhaseLength{60}; // 1 hour
    std::chrono::minutes m_ceasefirePhaseLength{4320}; // 3 days
    std::chrono::minutes m_resourceCollectionInterval{60}; // 1 hour
    uint32_t m_maxActiveWarsPerGuild{3};
    uint32_t m_minGuildLevelForWar{5};
    uint32_t m_maxStructuresPerTerritory{50};
    uint32_t m_maxStructureLevel{10};
    bool m_allowFriendlyFire{false};
    bool m_enablePeaceTimeProtection{true};
    bool m_requireWarAcceptance{true};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_warManagementThread;
    std::thread m_resourceGenerationThread;
    std::thread m_eventCleanupThread;
    
    // Métodos privados
    std::string generateTerritoryId();
    std::string generateStructureId();
    std::string generateDeclarationId();
    std::string generateEventId();
    
    bool isTerritoryCapturable(const std::string& territoryId, const std::string& guildId) const;
    bool canDeclareWar(const std::string& attackingGuildId, const std::string& defendingGuildId) const;
    bool validateStructurePlacement(const std::string& territoryId, StructureType type, float posX, float posY, float posZ) const;
    
    void warManagementThread();
    void resourceGenerationThread();
    void eventCleanupThread();
    
    bool updateTerritoryControl(const std::string& territoryId);
    void advanceWarPhasesAutomatically();
    void processBattleResults(const std::string& declarationId);
    void generateTerritoryResources();
    
    void recordBattleEvent(const std::string& warDeclarationId, 
                        const std::string& territoryId,
                        const std::string& eventType,
                        const std::string& actorId,
                        const std::string& targetId,
                        const json& eventDetails);
    
    void updateWarStatistics(const std::string& guildId, 
                           const std::string& statType,
                           int32_t value = 1);
    
    void notifyTerritoryControlChanged(const std::string& territoryId, 
                                    const std::string& oldGuildId,
                                    const std::string& newGuildId);
    
    void notifyStructureStateChanged(const Structure& structure, const std::string& changeType);
    void notifyWarPhaseChanged(const std::string& declarationId, WarPhase newPhase);
    void notifyBattleEvent(const BattleEvent& event);
    
    bool calculateWarVictory(const std::string& declarationId);
    void applyWarResults(const std::string& declarationId, const std::string& winnerGuildId);
    void awardWarBenefits(const std::string& territoryId, const std::string& guildId);
    
    TerritoryDefinition loadTerritoryDefinition(const std::string& territoryId) const;
    bool saveTerritoryDefinition(const TerritoryDefinition& territory);
    
    bool isPointInTerritory(const std::string& territoryId, float x, float y) const;
    bool areTerritoriesAdjacent(const std::string& territoryId1, const std::string& territoryId2) const;
    
    void pruneOldBattleEvents();
};

// Implementação inline básica do singleton
inline TerritorialWarSystem& TerritorialWarSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<TerritorialWarSystem>(new TerritorialWarSystem());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR

// ConquerableTerritories.h - Mecânica de Conquista de Áreas
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <chrono>
#include <optional>
#include <nlohmann/json.hpp>
#include "TerritorialWarSystem.h"
#include "WorldMap.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;

enum class ConquestType {
    OCCUPATION,
    DOMINATION,
    SIEGE,
    ASSAULT,
    INFILTRATION,
    DIPLOMATIC,
    CHALLENGE,
    RITUAL
};

enum class ConquestStatus {
    INACTIVE,
    SCOUTING,
    PREPARING,
    IN_PROGRESS,
    CONTESTED,
    SUCCESSFUL,
    FAILED,
    ABANDONED
};

enum class ConquestRequirement {
    MINIMUM_PARTICIPANTS,
    GUILD_LEVEL,
    RESOURCE_COST,
    TERRITORY_ADJACENCY,
    TIME_RESTRICTION,
    ITEM_REQUIREMENT,
    ACHIEVEMENT_REQUIREMENT,
    REPUTATION_REQUIREMENT
};

struct ConquestArea {
    std::string areaId;
    std::string name;
    TerritoryType territoryType;
    std::vector<std::pair<float, float>> boundaries;
    std::vector<std::string> linkedTerritoryIds;
    std::vector<ConquestType> allowedConquestTypes;
    std::unordered_map<ConquestRequirement, json> requirements;
    uint32_t strategicValue;
    std::vector<std::string> specialFeatures;
    bool contested{false};
    std::chrono::minutes cooldownBetweenAttempts{1440}; // 24 hours
    json visualData;
};

struct ConquestAttempt {
    std::string attemptId;
    std::string areaId;
    std::string guildId;
    ConquestType type;
    ConquestStatus status;
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    uint32_t progressPercent{0};
    std::unordered_map<std::string, json> participantContributions; // playerId -> contribution details
    std::unordered_map<std::string, json> objectiveProgress; // objectiveId -> progress
    json conquestData;
    std::string warDeclarationId;
    bool successful{false};
    std::string failureReason;
};

struct ConquestObjective {
    std::string objectiveId;
    std::string name;
    std::string description;
    std::string type; // "capture_point", "defeat_guardian", "hold_position", "destroy_structure", etc.
    uint32_t requiredProgress{100};
    json objectiveParameters;
    json rewards;
    std::vector<std::string> prerequisiteObjectives;
};

struct ConquestHistory {
    std::string areaId;
    std::deque<std::pair<std::string, std::string>> controlHistory; // (guildId, timestamp)
    std::vector<ConquestAttempt> recentAttempts;
    std::unordered_map<std::string, uint32_t> contestedCount; // guildId -> count
    std::chrono::system_clock::time_point lastConquestTime;
    uint32_t totalConquestAttempts{0};
    uint32_t successfulConquests{0};
    uint32_t longestControl{0}; // days
    json additionalData;
};

class ConquerableTerritories {
public:
    // Singleton
    static ConquerableTerritories& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Gerenciamento de áreas
    std::string createConquestArea(const ConquestArea& area);
    bool updateConquestArea(const ConquestArea& area);
    bool removeConquestArea(const std::string& areaId);
    ConquestArea getConquestArea(const std::string& areaId) const;
    std::vector<ConquestArea> getAllConquestAreas() const;
    std::vector<ConquestArea> getConquestAreasByType(TerritoryType type) const;
    
    // Gerenciamento de conquistas
    std::string initiateConquest(const std::string& areaId, 
                               const std::string& guildId,
                               ConquestType type,
                               const json& conquestData = json());
    
    bool updateConquestStatus(const std::string& attemptId, ConquestStatus status);
    bool updateConquestProgress(const std::string& attemptId, uint32_t progressPercent);
    bool abandonConquest(const std::string& attemptId);
    ConquestAttempt getConquestAttempt(const std::string& attemptId) const;
    std::vector<ConquestAttempt> getActiveConquestAttempts() const;
    std::vector<ConquestAttempt> getGuildConquestAttempts(const std::string& guildId) const;
    
    // Objetivos de conquista
    std::string createConquestObjective(const ConquestObjective& objective);
    bool updateObjective(const ConquestObjective& objective);
    bool removeObjective(const std::string& objectiveId);
    ConquestObjective getObjective(const std::string& objectiveId) const;
    std::vector<ConquestObjective> getAreaObjectives(const std::string& areaId) const;
    
    bool updateObjectiveProgress(const std::string& attemptId, 
                               const std::string& objectiveId,
                               uint32_t progress,
                               const json& progressDetails = json());
    
    bool completeObjective(const std::string& attemptId, 
                         const std::string& objectiveId,
                         const std::string& playerId);
    
    // Participação
    bool registerParticipant(const std::string& attemptId, 
                          const std::string& playerId,
                          const json& participantData = json());
    
    bool updateParticipantContribution(const std::string& attemptId,
                                     const std::string& playerId,
                                     const std::string& contributionType,
                                     uint32_t value);
    
    std::vector<std::string> getConquestParticipants(const std::string& attemptId) const;
    
    // Histórico e estatísticas
    ConquestHistory getAreaHistory(const std::string& areaId) const;
    std::vector<ConquestAttempt> getAreaRecentAttempts(const std::string& areaId, uint32_t limit = 10) const;
    json getConquestStatistics() const;
    json getGuildConquestStatistics(const std::string& guildId) const;
    
    // Integração
    bool linkAreaToTerritory(const std::string& areaId, const std::string& territoryId);
    bool transferAreaControl(const std::string& areaId, const std::string& guildId);
    bool canInitiateConquest(const std::string& areaId, 
                           const std::string& guildId,
                           ConquestType type,
                           std::string& failReason);
    
    // Callbacks
    void registerConquestStartedCallback(std::function<void(const ConquestAttempt&)> callback);
    void registerConquestStatusChangedCallback(std::function<void(const std::string&, ConquestStatus)> callback);
    void registerConquestCompletedCallback(std::function<void(const ConquestAttempt&, bool)> callback);
    void registerObjectiveCompletedCallback(std::function<void(const std::string&, const std::string&, const std::string&)> callback);
    
private:
    ConquerableTerritories();
    ~ConquerableTerritories();
    
    // Atributos privados
    static std::unique_ptr<ConquerableTerritories> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<TerritorialWarSystem> m_warSystem;
    std::shared_ptr<World::WorldMap> m_worldMap;
    
    // Dados de áreas de conquista
    std::unordered_map<std::string, ConquestArea> m_conquestAreas;
    std::unordered_map<TerritoryType, std::vector<std::string>> m_areasByType;
    mutable std::shared_mutex m_areasMutex;
    
    // Dados de tentativas de conquista
    std::unordered_map<std::string, ConquestAttempt> m_conquestAttempts;
    std::unordered_map<std::string, std::vector<std::string>> m_activeAttemptsByArea; // areaId -> [attemptIds]
    std::unordered_map<std::string, std::vector<std::string>> m_attemptsByGuild; // guildId -> [attemptIds]
    mutable std::shared_mutex m_attemptsMutex;
    
    // Objetivos
    std::unordered_map<std::string, ConquestObjective> m_objectives;
    std::unordered_map<std::string, std::vector<std::string>> m_objectivesByArea; // areaId -> [objectiveIds]
    mutable std::mutex m_objectivesMutex;
    
    // Histórico de conquista
    std::unordered_map<std::string, ConquestHistory> m_conquestHistory; // areaId -> history
    mutable std::mutex m_historyMutex;
    
    // Mapeamento de áreas para territórios
    std::unordered_map<std::string, std::string> m_areaToTerritoryMap; // areaId -> territoryId
    std::unordered_map<std::string, std::vector<std::string>> m_territoryToAreasMap; // territoryId -> [areaIds]
    mutable std::mutex m_mappingMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const ConquestAttempt&)>> m_conquestStartedCallbacks;
    std::vector<std::function<void(const std::string&, ConquestStatus)>> m_statusChangedCallbacks;
    std::vector<std::function<void(const ConquestAttempt&, bool)>> m_conquestCompletedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&, const std::string&)>> m_objectiveCompletedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas
    struct ConquestStats {
        std::atomic<uint32_t> totalAreas{0};
        std::atomic<uint32_t> totalAttemptsInitiated{0};
        std::atomic<uint32_t> totalAttemptsCompleted{0};
        std::atomic<uint32_t> totalAttemptsSucceeded{0};
        std::atomic<uint32_t> totalAttemptsFailed{0};
        std::atomic<uint32_t> totalObjectivesCompleted{0};
        std::atomic<uint32_t> totalParticipants{0};
    };
    
    ConquestStats m_stats;
    
    // Configuração
    uint32_t m_minPlayersForConquest{5};
    uint32_t m_maxConcurrentAttemptsPerGuild{2};
    uint32_t m_maxConcurrentAttemptsPerArea{3};
    std::chrono::minutes m_conquestTimeout{180}; // 3 hours
    bool m_requireAdjacentTerritory{true};
    bool m_allowContestedAreas{true};
    bool m_areaControlRequiredForBenefits{true};
    std::chrono::hours m_historyRetentionPeriod{720}; // 30 days
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_conquestTimeoutThread;
    std::thread m_historyMaintenanceThread;
    
    // Métodos privados
    std::string generateAreaId();
    std::string generateAttemptId();
    std::string generateObjectiveId();
    
    void conquestTimeoutThread();
    void historyMaintenanceThread();
    
    bool validateConquestRequirements(const std::string& areaId, 
                                   const std::string& guildId,
                                   ConquestType type);
    
    bool finishConquest(const std::string& attemptId, bool successful, const std::string& reason = "");
    void updateAreaContestStatus(const std::string& areaId);
    void applyConquestResults(const ConquestAttempt& attempt);
    
    void recordConquestHistory(const ConquestAttempt& attempt);
    void pruneConquestHistory();
    
    bool areAllPrerequisiteObjectivesCompleted(const std::string& attemptId, const std::string& objectiveId) const;
    bool checkObjectiveAutoCompletion(const std::string& attemptId, const std::string& objectiveId);
    
    void notifyConquestStarted(const ConquestAttempt& attempt);
    void notifyConquestStatusChanged(const std::string& attemptId, ConquestStatus status);
    void notifyConquestCompleted(const ConquestAttempt& attempt, bool successful);
    void notifyObjectiveCompleted(const std::string& attemptId, const std::string& objectiveId, const std::string& playerId);
    
    bool checkConquestCompletion(const std::string& attemptId);
    bool integrateTerritorialSystem(const std::string& areaId, const std::string& guildId);
};

// Implementação inline básica do singleton
inline ConquerableTerritories& ConquerableTerritories::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<ConquerableTerritories>(new ConquerableTerritories());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR

// ConstructionSystem.h - Estruturas Construtíveis e Destrutíveis
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>
#include "TerritorialWarSystem.h"
#include "ResourceSystem.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;

enum class BuildingState {
    BLUEPRINT,
    FOUNDATION,
    UNDER_CONSTRUCTION,
    COMPLETE,
    DAMAGED,
    REINFORCED,
    DESTROYED
};

enum class BuildingCategory {
    DEFENSIVE,
    RESIDENTIAL,
    PRODUCTION,
    MILITARY,
    ECONOMIC,
    SOCIAL,
    TRANSPORTATION,
    MAGICAL,
    CUSTOM
};

struct BuildingTemplate {
    std::string templateId;
    std::string name;
    StructureType structureType;
    BuildingCategory category;
    uint32_t maxHealth;
    uint32_t maxLevel;
    std::unordered_map<std::string, uint32_t> resourceCosts; // resourceType -> amount
    std::chrono::minutes constructionTime;
    std::unordered_map<std::string, float> statBonuses; // statType -> bonus
    std::vector<std::string> requiredTechnologies;
    std::vector<std::string> allowedUpgrades;
    std::vector<std::string> specialAbilities;
    json visualAssets;
    float minDistance{0.0f}; // minimum distance from other buildings
    json collisionData;
    bool destructible{true};
    uint32_t respawnTimeMinutes{0}; // if 0, doesn't respawn
};

struct Building {
    std::string buildingId;
    std::string templateId;
    std::string name;
    std::string ownerId; // guild or player ID
    std::string territoryId;
    float positionX;
    float positionY;
    float positionZ;
    float rotation;
    uint32_t level{1};
    BuildingState state{BuildingState::BLUEPRINT};
    uint32_t maxHealth;
    uint32_t currentHealth;
    uint32_t constructionProgress{0}; // 0-100%
    std::chrono::system_clock::time_point lastStateChange;
    std::optional<std::chrono::system_clock::time_point> completionTime;
    std::vector<std::string> activeUpgrades;
    std::unordered_map<std::string, float> currentBonuses;
    std::vector<uint8_t> instanceData; // serialized instance-specific data
    bool active{true};
};

struct BuildingUpgrade {
    std::string upgradeId;
    std::string name;
    std::string description;
    std::unordered_map<std::string, uint32_t> resourceCosts;
    std::chrono::minutes installationTime;
    std::unordered_map<std::string, float> statBonuses;
    std::vector<std::string> specialEffects;
    uint32_t requiredLevel{1};
    std::vector<std::string> prerequisiteUpgrades;
    bool visible{true};
};

struct ConstructionTask {
    std::string taskId;
    std::string buildingId;
    std::string workerId; // can be player, NPC, or workforce
    std::string taskType; // "construction", "repair", "upgrade", "demolish"
    uint32_t startProgress{0};
    uint32_t currentProgress{0};
    uint32_t targetProgress{100};
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> completionTime;
    bool paused{false};
    json taskDetails;
};

struct BuildingInteraction {
    std::string interactionId;
    std::string buildingId;
    std::string playerId;
    std::string interactionType; // "use", "access", "attack", "repair", etc.
    std::chrono::system_clock::time_point timestamp;
    json interactionData;
    bool successful{true};
};

class ConstructionSystem {
public:
    // Singleton
    static ConstructionSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Templates de construções
    std::string createBuildingTemplate(const BuildingTemplate& templ);
    bool updateBuildingTemplate(const BuildingTemplate& templ);
    bool removeBuildingTemplate(const std::string& templateId);
    BuildingTemplate getBuildingTemplate(const std::string& templateId) const;
    std::vector<BuildingTemplate> getAllBuildingTemplates() const;
    std::vector<BuildingTemplate> getBuildingTemplatesByCategory(BuildingCategory category) const;
    
    // Upgrades de construção
    std::string createBuildingUpgrade(const BuildingUpgrade& upgrade);
    bool updateBuildingUpgrade(const BuildingUpgrade& upgrade);
    bool removeBuildingUpgrade(const std::string& upgradeId);
    BuildingUpgrade getBuildingUpgrade(const std::string& upgradeId) const;
    std::vector<BuildingUpgrade> getAllBuildingUpgrades() const;
    std::vector<BuildingUpgrade> getCompatibleUpgrades(const std::string& templateId) const;
    
    // Criação e gerenciamento de construções
    std::string placeBuilding(const std::string& templateId,
                            const std::string& ownerId,
                            const std::string& territoryId,
                            float posX, float posY, float posZ,
                            float rotation = 0.0f);
    
    bool startConstruction(const std::string& buildingId, const std::string& workerId);
    bool updateConstructionProgress(const std::string& buildingId, uint32_t progress);
    bool cancelConstruction(const std::string& buildingId);
    bool finishConstruction(const std::string& buildingId);
    
    // Consulta e modificação de construções
    Building getBuilding(const std::string& buildingId) const;
    std::vector<Building> getBuildingsByOwner(const std::string& ownerId) const;
    std::vector<Building> getBuildingsByTerritory(const std::string& territoryId) const;
    std::vector<Building> getNearbyBuildings(float x, float y, float z, float radius) const;
    
    // Estado e modificação
    bool damageBuilding(const std::string& buildingId, uint32_t amount, const std::string& sourceId = "");
    bool repairBuilding(const std::string& buildingId, uint32_t amount, const std::string& repairerId = "");
    bool upgradeBuilding(const std::string& buildingId, uint32_t level);
    bool demolishBuilding(const std::string& buildingId, const std::string& initiatorId = "");
    bool installBuildingUpgrade(const std::string& buildingId, const std::string& upgradeId);
    bool removeBuildingUpgrade(const std::string& buildingId, const std::string& upgradeId);
    
    // Tarefas de construção
    std::string createConstructionTask(const std::string& buildingId, 
                                    const std::string& workerId,
                                    const std::string& taskType);
    
    bool updateConstructionTask(const std::string& taskId, uint32_t progress);
    bool pauseConstructionTask(const std::string& taskId);
    bool resumeConstructionTask(const std::string& taskId);
    bool cancelConstructionTask(const std::string& taskId);
    ConstructionTask getConstructionTask(const std::string& taskId) const;
    std::vector<ConstructionTask> getActiveTasks() const;
    std::vector<ConstructionTask> getTasksByWorker(const std::string& workerId) const;
    
    // Interações
    bool recordBuildingInteraction(const std::string& buildingId,
                                 const std::string& playerId,
                                 const std::string& interactionType,
                                 const json& interactionData = json());
    
    std::vector<BuildingInteraction> getBuildingInteractions(const std::string& buildingId, uint32_t limit = 100) const;
    std::vector<std::string> getCurrentOccupants(const std::string& buildingId) const;
    
    // Funcionalidade e bonificações
    json getBuildingBonuses(const std::string& buildingId) const;
    std::unordered_map<std::string, float> getOwnerBuildingBonuses(const std::string& ownerId) const;
    bool activateBuildingAbility(const std::string& buildingId, const std::string& abilityId, const json& parameters = json());
    
    // Validação
    bool canPlaceBuilding(const std::string& templateId, 
                        const std::string& territoryId,
                        float posX, float posY, float posZ,
                        std::string& failReason);
    
    bool hasRequiredResources(const std::string& ownerId, 
                            const std::unordered_map<std::string, uint32_t>& requiredResources);
    
    // Callbacks
    void registerBuildingStateChangedCallback(std::function<void(const Building&, BuildingState)> callback);
    void registerBuildingDamagedCallback(std::function<void(const std::string&, uint32_t, const std::string&)> callback);
    void registerBuildingCompletedCallback(std::function<void(const Building&)> callback);
    void registerBuildingDestroyedCallback(std::function<void(const Building&, const std::string&)> callback);
    
private:
    ConstructionSystem();
    ~ConstructionSystem();
    
    // Atributos privados
    static std::unique_ptr<ConstructionSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<TerritorialWarSystem> m_warSystem;
    std::shared_ptr<Economy::ResourceSystem> m_resourceSystem;
    
    // Templates de construção
    std::unordered_map<std::string, BuildingTemplate> m_buildingTemplates;
    std::unordered_map<BuildingCategory, std::vector<std::string>> m_templatesByCategory;
    mutable std::shared_mutex m_templatesMutex;
    
    // Upgrades de construção
    std::unordered_map<std::string, BuildingUpgrade> m_buildingUpgrades;
    std::unordered_map<std::string, std::vector<std::string>> m_compatibleUpgrades; // templateId -> [upgradeIds]
    mutable std::shared_mutex m_upgradesMutex;
    
    // Construções
    std::unordered_map<std::string, Building> m_buildings;
    std::unordered_map<std::string, std::vector<std::string>> m_buildingsByOwner; // ownerId -> [buildingIds]
    std::unordered_map<std::string, std::vector<std::string>> m_buildingsByTerritory; // territoryId -> [buildingIds]
    mutable std::shared_mutex m_buildingsMutex;
    
    // Tarefas de construção
    std::unordered_map<std::string, ConstructionTask> m_constructionTasks;
    std::unordered_map<std::string, std::string> m_buildingTasks; // buildingId -> taskId
    std::unordered_map<std::string, std::vector<std::string>> m_workerTasks; // workerId -> [taskIds]
    mutable std::shared_mutex m_tasksMutex;
    
    // Interações com construções
    std::unordered_map<std::string, std::vector<BuildingInteraction>> m_buildingInteractions; // buildingId -> interactions
    std::unordered_map<std::string, std::set<std::string>> m_buildingOccupants; // buildingId -> set of playerIds
    mutable std::mutex m_interactionsMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const Building&, BuildingState)>> m_stateChangedCallbacks;
    std::vector<std::function<void(const std::string&, uint32_t, const std::string&)>> m_damagedCallbacks;
    std::vector<std::function<void(const Building&)>> m_completedCallbacks;
    std::vector<std::function<void(const Building&, const std::string&)>> m_destroyedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas
    struct ConstructionStats {
        std::atomic<uint32_t> totalBuildings{0};
        std::atomic<uint32_t> activeBuildings{0};
        std::atomic<uint32_t> buildingsUnderConstruction{0};
        std::atomic<uint32_t> completedBuildings{0};
        std::atomic<uint32_t> destroyedBuildings{0};
        std::atomic<uint32_t> damagedBuildings{0};
        std::atomic<uint32_t> repairedBuildings{0};
        std::atomic<uint32_t> upgradedBuildings{0};
        std::atomic<uint32_t> activeTasks{0};
    };
    
    ConstructionStats m_stats;
    
    // Configuração
    uint32_t m_maxBuildingsPerTerritory{1000};
    uint32_t m_maxBuildingsPerOwner{100};
    uint32_t m_maxTasksPerWorker{5};
    float m_globalMinBuildingDistance{2.0f};
    std::chrono::seconds m_interactionHistoryRetention{86400}; // 24 hours
    bool m_allowOverlappingBuildings{false};
    bool m_requireResourcesForConstruction{true};
    bool m_requireResourcesForRepair{true};
    bool m_enableBuildingDecay{true};
    float m_decayRatePerDay{0.5f}; // % health loss per day
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_constructionProgressThread;
    std::thread m_buildingDecayThread;
    std::thread m_taskTimeoutThread;
    
    // Métodos privados
    std::string generateBuildingId();
    std::string generateTaskId();
    std::string generateInteractionId();
    
    void constructionProgressThread();
    void buildingDecayThread();
    void taskTimeoutThread();
    
    bool checkCollision(const std::string& templateId, float posX, float posY, float posZ, float rotation) const;
    bool checkTerrainSuitability(float posX, float posY, float posZ) const;
    bool enforceMinimumDistance(const std::string& templateId, float posX, float posY, float posZ) const;
    
    bool deductConstructionResources(const std::string& ownerId, const std::unordered_map<std::string, uint32_t>& resources);
    bool refundConstructionResources(const std::string& ownerId, const std::unordered_map<std::string, uint32_t>& resources, float refundPercentage = 0.75f);
    
    void updateBuildingState(Building& building, BuildingState newState);
    bool applyBuildingUpgrade(Building& building, const BuildingUpgrade& upgrade);
    void calculateBuildingEffectiveStats(Building& building);
    
    void checkAutoCompleteTask(const std::string& taskId);
    void processTaskProgress(ConstructionTask& task);
    
    bool isBuildingProtected(const std::string& buildingId) const;
    bool canBuildingBeModified(const std::string& buildingId, const std::string& actorId) const;
    
    std::unordered_map<std::string, uint32_t> calculateRepairCosts(const Building& building, uint32_t repairAmount) const;
    std::unordered_map<std::string, uint32_t> calculateUpgradeCosts(const Building& building, uint32_t targetLevel) const;
    
    void notifyBuildingStateChanged(const Building& building, BuildingState state);
    void notifyBuildingDamaged(const std::string& buildingId, uint32_t amount, const std::string& sourceId);
    void notifyBuildingCompleted(const Building& building);
    void notifyBuildingDestroyed(const Building& building, const std::string& destroyerId);
    
    void pruneOldInteractions();
    
    bool isWithinTerritory(const std::string& territoryId, float posX, float posY, float posZ) const;
    float calculateDistance(float x1, float y1, float z1, float x2, float y2, float z2) const;
};

// Implementação inline básica do singleton
inline ConstructionSystem& ConstructionSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<ConstructionSystem>(new ConstructionSystem());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR

// TerritoryControlBenefits.h - Benefícios para Controle de Território
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>
#include "TerritorialWarSystem.h"
#include "GuildSystem.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;

enum class BenefitType {
    STAT_BONUS,
    RESOURCE_GENERATION,
    SKILL_BONUS,
    EXPERIENCE_BONUS,
    UNLOCK_FEATURE,
    TAX_REDUCTION,
    SPECIAL_ABILITY,
    CUSTOM
};

enum class BenefitScope {
    INDIVIDUAL,
    GUILD,
    ALLIANCE,
    GLOBAL
};

enum class BenefitDuration {
    TEMPORARY,
    PERMANENT,
    CONDITIONAL,
    SCALING
};

struct TerritoryBenefit {
    std::string benefitId;
    std::string name;
    BenefitType type;
    BenefitScope scope;
    BenefitDuration duration;
    float value;
    std::string statOrSkillAffected;
    std::optional<std::chrono::minutes> temporaryDuration;
    std::unordered_map<std::string, float> scalingFactors; // factor -> value
    std::string requirementCondition;
    json additionalParameters;
    bool stackable{false};
    uint32_t maxStacks{1};
    bool visible{true};
};

struct TerritoryBenefitInstance {
    std::string instanceId;
    std::string benefitId;
    std::string territoryId;
    std::string targetId; // guild ID or player ID depending on scope
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    float currentValue;
    uint32_t currentStacks{1};
    bool active{true};
    std::string source; // how this benefit was obtained
    json instanceData;
};

class TerritoryControlBenefits {
public:
    // Singleton
    static TerritoryControlBenefits& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Definição de benefícios
    std::string createBenefit(const TerritoryBenefit& benefit);
    bool updateBenefit(const TerritoryBenefit& benefit);
    bool removeBenefit(const std::string& benefitId);
    TerritoryBenefit getBenefit(const std::string& benefitId) const;
    std::vector<TerritoryBenefit> getAllBenefits() const;
    std::vector<TerritoryBenefit> getBenefitsByType(BenefitType type) const;
    
    // Atribuição de benefícios
    std::string assignTerritoryBenefit(const std::string& benefitId,
                                     const std::string& territoryId,
                                     const std::string& targetId,
                                     float value = 0.0f,
                                     const std::string& source = "territory_control");
    
    bool removeTerritoryBenefit(const std::string& instanceId);
    bool updateBenefitValue(const std::string& instanceId, float newValue);
    bool addBenefitStack(const std::string& instanceId, uint32_t stacks = 1);
    
    // Consulta de benefícios
    TerritoryBenefitInstance getBenefitInstance(const std::string& instanceId) const;
    std::vector<TerritoryBenefitInstance> getPlayerBenefits(const std::string& playerId) const;
    std::vector<TerritoryBenefitInstance> getGuildBenefits(const std::string& guildId) const;
    std::vector<TerritoryBenefitInstance> getTerritoryBenefits(const std::string& territoryId) const;
    
    // Cálculo de efeitos
    float calculatePlayerStatBonus(const std::string& playerId, const std::string& statName) const;
    float calculateGuildStatBonus(const std::string& guildId, const std::string& statName) const;
    std::unordered_map<std::string, float> calculateAllPlayerBonuses(const std::string& playerId) const;
    
    // Gerenciamento de condições
    bool checkBenefitCondition(const std::string& instanceId);
    void validateAllBenefits();
    
    // Integração
    bool assignDefaultBenefitsForTerritory(const std::string& territoryId, const std::string& guildId);
    void handleTerritoryControlChange(const std::string& territoryId, const std::string& oldGuildId, const std::string& newGuildId);
    void handlePlayerGuildChange(const std::string& playerId, const std::string& oldGuildId, const std::string& newGuildId);
    
    // Callbacks
    void registerBenefitAssignedCallback(std::function<void(const TerritoryBenefitInstance&)> callback);
    void registerBenefitRemovedCallback(std::function<void(const std::string&, const std::string&, const std::string&)> callback);
    void registerBenefitUpdatedCallback(std::function<void(const TerritoryBenefitInstance&)> callback);
    
private:
    TerritoryControlBenefits();
    ~TerritoryControlBenefits();
    
    // Atributos privados
    static std::unique_ptr<TerritoryControlBenefits> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<TerritorialWarSystem> m_warSystem;
    std::shared_ptr<Guild::GuildSystem> m_guildSystem;
    
    // Definições de benefícios
    std::unordered_map<std::string, TerritoryBenefit> m_benefits;
    std::unordered_map<BenefitType, std::vector<std::string>> m_benefitsByType;
    mutable std::shared_mutex m_benefitsMutex;
    
    // Instâncias de benefícios
    std::unordered_map<std::string, TerritoryBenefitInstance> m_benefitInstances;
    std::unordered_map<std::string, std::vector<std::string>> m_playerBenefits; // playerId -> [instanceIds]
    std::unordered_map<std::string, std::vector<std::string>> m_guildBenefits; // guildId -> [instanceIds]
    std::unordered_map<std::string, std::vector<std::string>> m_territoryBenefits; // territoryId -> [instanceIds]
    mutable std::shared_mutex m_instancesMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const TerritoryBenefitInstance&)>> m_benefitAssignedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&, const std::string&)>> m_benefitRemovedCallbacks;
    std::vector<std::function<void(const TerritoryBenefitInstance&)>> m_benefitUpdatedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Cache temporário de bônus calculados
    struct BonusCache {
        std::unordered_map<std::string, std::unordered_map<std::string, float>> playerStatBonuses; // playerId -> (statName -> bonus)
        std::unordered_map<std::string, std::unordered_map<std::string, float>> guildStatBonuses; // guildId -> (statName -> bonus)
        std::chrono::system_clock::time_point lastUpdated;
        std::mutex mutex;
    };
    
    BonusCache m_bonusCache;
    
    // Estatísticas
    struct BenefitStats {
        std::atomic<uint32_t> totalBenefits{0};
        std::atomic<uint32_t> activeBenefitInstances{0};
        std::atomic<uint32_t> benefitsAssigned{0};
        std::atomic<uint32_t> benefitsRemoved{0};
        std::atomic<uint32_t> benefitsExpired{0};
        std::atomic<uint32_t> conditionalsReevaluated{0};
    };
    
    BenefitStats m_stats;
    
    // Configuração
    std::chrono::minutes m_temporaryBenefitMaxDuration{4320}; // 3 days
    std::chrono::minutes m_bonusCacheRefreshInterval{1};
    std::chrono::minutes m_conditionalBenefitCheckInterval{10};
    bool m_automaticDefaultBenefits{true};
    bool m_propagateGuildBenefitsToMembers{true};
    uint32_t m_maxActiveBenefitsPerPlayer{50};
    uint32_t m_maxActiveBenefitsPerGuild{100};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_temporaryBenefitsThread;
    std::thread m_conditionalBenefitsThread;
    std::thread m_bonusCacheThread;
    
    // Métodos privados
    std::string generateBenefitId();
    std::string generateInstanceId();
    
    void temporaryBenefitsThread();
    void conditionalBenefitsThread();
    void bonusCacheThread();
    
    float evaluateBenefitValue(const TerritoryBenefit& benefit, 
                             const std::string& territoryId,
                             const std::string& targetId);
    
    bool evaluateCondition(const std::string& condition, 
                         const std::string& territoryId,
                         const std::string& targetId);
    
    void cleanupExpiredBenefits();
    void invalidateBonusCache();
    void rebuildBonusCache();
    
    void updateIndices(const TerritoryBenefitInstance& instance, bool remove = false);
    
    void notifyBenefitAssigned(const TerritoryBenefitInstance& instance);
    void notifyBenefitRemoved(const std::string& instanceId, const std::string& benefitId, const std::string& targetId);
    void notifyBenefitUpdated(const TerritoryBenefitInstance& instance);
    
    std::vector<TerritoryBenefit> getDefaultBenefitsForTerritoryType(TerritoryType type) const;
    std::vector<std::string> getGuildMemberIds(const std::string& guildId) const;
    
    bool canBenefitBeAssigned(const TerritoryBenefit& benefit, 
                            const std::string& territoryId,
                            const std::string& targetId);
};

// Implementação inline básica do singleton
inline TerritoryControlBenefits& TerritoryControlBenefits::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<TerritoryControlBenefits>(new TerritoryControlBenefits());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR

// WarDeclarationSystem.h - Sistema de Declaração de Guerra
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
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>
#include "TerritorialWarSystem.h"
#include "GuildSystem.h"
#include "DiplomacySystem.h"

namespace WYDBR {
namespace Gameplay {

using json = nlohmann::json;
using namespace std::chrono_literals;

enum class DeclarationStatus {
    PROPOSED,
    PENDING,
    ACCEPTED,
    REJECTED,
    EXPIRED,
    CANCELLED,
    IN_PROGRESS,
    CONCLUDED
};

enum class WarType {
    TERRITORIAL,
    DOMINANCE,
    REVENGE,
    RESOURCE,
    POLITICAL,
    HONOR,
    CUSTOM
};

enum class CasusBelli {
    TERRITORIAL_EXPANSION,
    RESOURCE_ACQUISITION,
    REVENGE,
    PREEMPTIVE_STRIKE,
    TREATY_VIOLATION,
    HONOR_DISPUTE,
    LIBERATION,
    NONE
};

struct WarRequirement {
    std::string requirementId;
    std::string name;
    std::string description;
    std::string conditionType; // "guild_level", "territory_adjacent", "resources", "diplomatic_status", etc.
    json conditionParameters;
    bool mandatory{true};
    std::string failureMessage;
};

struct WarDeclarationTemplate {
    std::string templateId;
    std::string name;
    WarType type;
    std::vector<CasusBelli> allowedCasusBelli;
    std::vector<WarRequirement> requirements;
    std::chrono::minutes minPreparationTime{1440}; // 24 hours
    std::chrono::minutes maxPreparationTime{2880}; // 48 hours
    std::chrono::minutes minWarDuration{120}; // 2 hours
    std::chrono::minutes maxWarDuration{10080}; // 7 days
    std::unordered_map<std::string, uint32_t> resourceCosts; // resource -> amount
    uint32_t reputationCost{0};
    std::chrono::minutes peaceTreatyCooldown{10080}; // 7 days
    json victoryConditions;
    json defeatConditions;
    json drawConditions;
    bool requiresApproval{true};
    bool allowNegotiation{true};
};

struct WarDeclarationDetails {
    std::string declarationId;
    std::string templateId;
    std::string attackingGuildId;
    std::string defendingGuildId;
    std::vector<std::string> targetTerritoryIds;
    CasusBelli casusBelli;
    DeclarationStatus status;
    std::chrono::system_clock::time_point declaredAt;
    std::optional<std::chrono::system_clock::time_point> responseDeadline;
    std::optional<std::chrono::system_clock::time_point> warStartTime;
    std::optional<std::chrono::system_clock::time_point> warEndTime;
    std::string declaringUserId;
    std::optional<std::string> respondingUserId;
    std::string rejectionReason;
    json declarationTerms;
    json counterOffer;
    bool counterOfferAccepted{false};
    json warOutcome;
};

struct WarOfficial {
    std::string userId;
    std::string guildId;
    std::string role; // "general", "diplomat", "strategist", etc.
    std::vector<std::string> permissions; // "declare_war", "accept_war", "negotiate", "surrender", etc.
    bool active{true};
    std::chrono::system_clock::time_point appointedAt;
    std::optional<std::chrono::system_clock::time_point> expiresAt;
};

struct WarAction {
    std::string actionId;
    std::string declarationId;
    std::string userId;
    std::string guildId;
    std::string actionType; // "declare", "accept", "reject", "negotiate", "surrender", etc.
    std::chrono::system_clock::time_point timestamp;
    json actionDetails;
};

class WarDeclarationSystem {
public:
    // Singleton
    static WarDeclarationSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Templates de declaração
    std::string createWarDeclarationTemplate(const WarDeclarationTemplate& templ);
    bool updateWarDeclarationTemplate(const WarDeclarationTemplate& templ);
    bool removeWarDeclarationTemplate(const std::string& templateId);
    WarDeclarationTemplate getWarDeclarationTemplate(const std::string& templateId) const;
    std::vector<WarDeclarationTemplate> getAllWarDeclarationTemplates() const;
    
    // Requisitos de guerra
    std::string createWarRequirement(const WarRequirement& requirement);
    bool updateWarRequirement(const WarRequirement& requirement);
    bool removeWarRequirement(const std::string& requirementId);
    WarRequirement getWarRequirement(const std::string& requirementId) const;
    
    // Oficiais de guerra
    std::string appointWarOfficial(const std::string& userId, 
                                 const std::string& guildId,
                                 const std::string& role,
                                 const std::vector<std::string>& permissions);
    
    bool updateWarOfficialPermissions(const std::string& userId, 
                                    const std::string& guildId,
                                    const std::vector<std::string>& permissions);
    
    bool removeWarOfficial(const std::string& userId, const std::string& guildId);
    WarOfficial getWarOfficial(const std::string& userId, const std::string& guildId) const;
    std::vector<WarOfficial> getGuildWarOfficials(const std::string& guildId) const;
    
    // Declaração de guerra
    std::string declareWar(const std::string& templateId,
                         const std::string& attackingGuildId,
                         const std::string& defendingGuildId,
                         const std::vector<std::string>& targetTerritoryIds,
                         CasusBelli casusBelli,
                         const std::string& declaringUserId,
                         const json& declarationTerms = json());
    
    bool acceptWarDeclaration(const std::string& declarationId, 
                           const std::string& respondingUserId);
    
    bool rejectWarDeclaration(const std::string& declarationId, 
                           const std::string& respondingUserId,
                           const std::string& reason = "");
    
    bool cancelWarDeclaration(const std::string& declarationId, 
                           const std::string& userId);
    
    bool proposeCounterOffer(const std::string& declarationId,
                          const std::string& userId,
                          const json& counterOffer);
    
    bool acceptCounterOffer(const std::string& declarationId,
                         const std::string& userId);
    
    bool rejectCounterOffer(const std::string& declarationId,
                         const std::string& userId);
    
    bool surrenderWar(const std::string& declarationId,
                    const std::string& surrenderingGuildId,
                    const std::string& userId);
    
    bool concludeWar(const std::string& declarationId,
                   const std::string& winningGuildId,
                   const json& outcome);
    
    // Consulta de declarações
    WarDeclarationDetails getWarDeclaration(const std::string& declarationId) const;
    std::vector<WarDeclarationDetails> getGuildWarDeclarations(const std::string& guildId) const;
    std::vector<WarDeclarationDetails> getActiveWarDeclarations() const;
    std::vector<WarDeclarationDetails> getPendingWarDeclarations() const;
    std::vector<WarDeclarationDetails> getHistoricalWarDeclarations(const std::string& guildId, uint32_t limit = 10) const;
    
    // Ações de guerra
    bool recordWarAction(const std::string& declarationId,
                       const std::string& userId,
                       const std::string& guildId,
                       const std::string& actionType,
                       const json& actionDetails = json());
    
    std::vector<WarAction> getWarActions(const std::string& declarationId) const;
    
    // Validação
    bool canDeclareWar(const std::string& templateId,
                     const std::string& attackingGuildId,
                     const std::string& defendingGuildId,
                     std::string& failReason);
    
    bool hasWarPermission(const std::string& userId,
                        const std::string& guildId,
                        const std::string& permission);
    
    bool verifyWarRequirements(const std::string& templateId,
                             const std::string& attackingGuildId,
                             const std::string& defendingGuildId,
                             std::vector<std::string>& failedRequirements);
    
    // Integração
    bool notifyWarStart(const std::string& declarationId);
    bool notifyWarEnd(const std::string& declarationId, const std::string& winningGuildId);
    
    // Callbacks
    void registerWarDeclaredCallback(std::function<void(const WarDeclarationDetails&)> callback);
    void registerWarStatusChangedCallback(std::function<void(const std::string&, DeclarationStatus)> callback);
    void registerWarActionRecordedCallback(std::function<void(const WarAction&)> callback);
    void registerWarConcludedCallback(std::function<void(const std::string&, const std::string&, const json&)> callback);
    
private:
    WarDeclarationSystem();
    ~WarDeclarationSystem();
    
    // Atributos privados
    static std::unique_ptr<WarDeclarationSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<TerritorialWarSystem> m_warSystem;
    std::shared_ptr<Guild::GuildSystem> m_guildSystem;
    std::shared_ptr<Diplomacy::DiplomacySystem> m_diplomacySystem;
    
    // Templates de declaração
    std::unordered_map<std::string, WarDeclarationTemplate> m_warTemplates;
    mutable std::shared_mutex m_templatesMutex;
    
    // Requisitos de guerra
    std::unordered_map<std::string, WarRequirement> m_warRequirements;
    mutable std::mutex m_requirementsMutex;
    
    // Oficiais de guerra
    std::unordered_map<std::string, WarOfficial> m_warOfficials; // userId:guildId -> official
    std::unordered_map<std::string, std::vector<std::string>> m_guildOfficials; // guildId -> [userIds]
    mutable std::shared_mutex m_officialsMutex;
    
    // Declarações de guerra
    std::unordered_map<std::string, WarDeclarationDetails> m_warDeclarations;
    std::unordered_map<std::string, std::vector<std::string>> m_guildDeclarations; // guildId -> [declarationIds]
    std::unordered_map<DeclarationStatus, std::vector<std::string>> m_declarationsByStatus;
    std::vector<WarDeclarationDetails> m_historicalDeclarations;
    mutable std::shared_mutex m_declarationsMutex;
    
    // Ações de guerra
    std::unordered_map<std::string, std::vector<WarAction>> m_warActions; // declarationId -> [actions]
    mutable std::mutex m_actionsMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const WarDeclarationDetails&)>> m_warDeclaredCallbacks;
    std::vector<std::function<void(const std::string&, DeclarationStatus)>> m_statusChangedCallbacks;
    std::vector<std::function<void(const WarAction&)>> m_actionRecordedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&, const json&)>> m_warConcludedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas
    struct WarStats {
        std::atomic<uint32_t> totalDeclarations{0};
        std::atomic<uint32_t> activeWars{0};
        std::atomic<uint32_t> pendingDeclarations{0};
        std::atomic<uint32_t> rejectedDeclarations{0};
        std::atomic<uint32_t> completedWars{0};
        std::atomic<uint32_t> surrenderedWars{0};
        std::atomic<uint32_t> negotiatedSettlements{0};
    };
    
    WarStats m_stats;
    
    // Configuração
    std::chrono::hours m_declarationResponseDeadline{24};
    std::chrono::days m_historicalWarRetention{90};
    uint32_t m_maxActiveWarsPerGuild{3};
    uint32_t m_maxPendingDeclarationsPerGuild{5};
    uint32_t m_maxHistoricalWars{1000};
    bool m_enforceWarRequirements{true};
    bool m_enforceOfficialPermissions{true};
    bool m_allowSurrender{true};
    bool m_integrateWithDiplomacy{true};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_declarationTimeoutThread;
    std::thread m_warSchedulingThread;
    std::thread m_historicalDataThread;
    
    // Métodos privados
    std::string generateDeclarationId();
    std::string generateRequirementId();
    std::string generateActionId();
    
    void declarationTimeoutThread();
    void warSchedulingThread();
    void historicalDataThread();
    
    bool updateDeclarationStatus(const std::string& declarationId, DeclarationStatus newStatus);
    bool consumeWarResources(const std::string& guildId, const std::unordered_map<std::string, uint32_t>& resources);
    void updateWarIndices(const WarDeclarationDetails& declaration, bool remove = false);
    
    bool validateDeclarationParams(const std::string& templateId,
                                const std::string& attackingGuildId,
                                const std::string& defendingGuildId,
                                const std::vector<std::string>& targetTerritoryIds);
    
    bool canGuildsGoToWar(const std::string& attackingGuildId, const std::string& defendingGuildId) const;
    bool areTerritoriesValid(const std::vector<std::string>& territoryIds, const std::string& defendingGuildId) const;
    
    void archiveHistoricalWars();
    void pruneExpiredDeclarations();
    void scheduleWarEvents();
    
    bool finalizeWarPreparations(const std::string& declarationId);
    void startWarHostilities(const std::string& declarationId);
    
    void notifyWarDeclared(const WarDeclarationDetails& declaration);
    void notifyWarStatusChanged(const std::string& declarationId, DeclarationStatus status);
    void notifyWarActionRecorded(const WarAction& action);
    void notifyWarConcluded(const std::string& declarationId, const std::string& winningGuildId, const json& outcome);
    
    std::string getOfficialKey(const std::string& userId, const std::string& guildId) const;
    
    bool applyDiplomaticConsequences(const std::string& attackingGuildId, const std::string& defendingGuildId, const std::string& outcome);
};

// Implementação inline básica do singleton
inline WarDeclarationSystem& WarDeclarationSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<WarDeclarationSystem>(new WarDeclarationSystem());
    });
    return *s_instance;
}

} // namespace Gameplay
} // namespace WYDBR
`;

// Fornece uma API JavaScript para integração no projeto
module.exports.setupSystem = function(engineContext) {
  console.log('Inicializando sistema: sistema_guerra_territorial');
  
  // Inicializa o sistema no contexto do engine
  engineContext.registerSystem({
    name: 'sistema_guerra_territorial',
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
