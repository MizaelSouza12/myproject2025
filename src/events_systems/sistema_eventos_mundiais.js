/**
 * WYDBR 2.0 - Implementação Gerada Automaticamente
 * Arquivo origem: sistema_eventos_mundiais.txt
 * 
 * Este arquivo contém a definição C++ do sistema, encapsulada em JavaScript
 * para integração no projeto WYDBR 2.0.
 */

// Exporta o código C++ como uma string para ser processado pelo compilador C++
module.exports.cppCode = `
// WorldEventSystem.h - Sistema de Eventos Mundiais
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
#include "WorldState.h"
#include "PlayerManager.h"

namespace WYDBR {
namespace Events {

using json = nlohmann::json;
using namespace std::chrono_literals;

enum class WorldEventType {
    INVASION,
    NATURAL_DISASTER,
    FESTIVAL,
    UPRISING,
    WAR,
    ECONOMIC_CRISIS,
    DISCOVERY,
    SEASONAL,
    CELESTIAL,
    PLAGUE,
    CUSTOM
};

enum class EventTrigger {
    SCHEDULED,
    PLAYER_ACTION,
    THRESHOLD,
    RANDOM,
    CHAINED,
    ADMIN,
    SEASONAL,
    CUSTOM
};

enum class EventPhase {
    PREPARATION,
    ACTIVE,
    ESCALATION,
    CLIMAX,
    RESOLUTION,
    AFTERMATH
};

enum class EventScope {
    GLOBAL,
    REGIONAL,
    LOCAL,
    GUILD,
    FACTION,
    PLAYER_GROUP,
    CUSTOM
};

struct WorldEventDefinition {
    std::string eventId;
    std::string name;
    std::string description;
    WorldEventType type;
    EventScope scope;
    
    struct EventRequirement {
        std::string conditionType; // "PLAYER_COUNT", "WORLD_STATE", "TIME_ELAPSED", "RESOURCE_LEVEL", etc.
        json parameters;
        bool inverse{false};
    };
    
    std::vector<EventRequirement> requirements;
    
    struct EventPhaseDefinition {
        EventPhase phase;
        std::string description;
        std::chrono::minutes minDuration{15};
        std::chrono::minutes maxDuration{60};
        float intensityMultiplier{1.0f};
        std::vector<std::string> triggerableEvents;
        std::vector<std::string> spawnEntities;
        json phaseEffects;
    };
    
    std::vector<EventPhaseDefinition> phases;
    
    struct EventOutcome {
        std::string outcomeId;
        std::string name;
        std::string description;
        float probability{0.1f};
        json worldStateChanges;
        std::vector<std::string> followupEvents;
        json rewards;
    };
    
    std::vector<EventOutcome> possibleOutcomes;
    
    std::chrono::minutes minDuration{60};
    std::chrono::minutes maxDuration{120};
    std::chrono::minutes cooldown{1440}; // 24 hours
    
    struct EventLocation {
        std::string locationType; // "POINT", "CIRCLE", "REGION", "GLOBAL"
        float x{0.0f};
        float y{0.0f};
        float z{0.0f};
        float radius{0.0f};
        std::string regionId;
    };
    
    std::vector<EventLocation> possibleLocations;
    
    bool repeatable{true};
    uint32_t maxInstances{0}; // 0 = unlimited
    float importance{1.0f}; // 0-10, influences event priority
    std::vector<std::string> tags;
    std::string category;
    json eventParameters;
};

struct WorldEventInstance {
    std::string instanceId;
    std::string eventId;
    WorldEventType type;
    EventPhase currentPhase{EventPhase::PREPARATION};
    EventScope scope;
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    std::chrono::system_clock::time_point phaseStartTime;
    std::optional<std::chrono::system_clock::time_point> nextPhaseTime;
    
    struct ActiveLocation {
        std::string locationType;
        float x{0.0f};
        float y{0.0f};
        float z{0.0f};
        float radius{0.0f};
        std::string regionId;
        json locationData;
    };
    
    std::vector<ActiveLocation> activeLocations;
    std::set<std::string> affectedRegions;
    std::set<std::string> affectedPlayers;
    std::set<std::string> participatingPlayers;
    std::map<std::string, int64_t> playerContributions; // playerId -> contribution score
    std::string status; // "PENDING", "ACTIVE", "COMPLETED", "FAILED", "CANCELED"
    float intensity{1.0f};
    float progressPercent{0.0f};
    std::optional<std::string> outcomeId;
    EventTrigger trigger{EventTrigger::SCHEDULED};
    std::optional<std::string> triggerSource; // Player ID, previous event ID, etc.
    std::vector<std::string> chainedEventInstances;
    json eventState;
    json worldStateChanges;
};

struct EventParticipation {
    std::string participationId;
    std::string instanceId;
    std::string playerId;
    std::chrono::system_clock::time_point joinTime;
    std::optional<std::chrono::system_clock::time_point> leaveTime;
    int64_t contribution{0};
    json participationDetails;
    std::vector<std::string> completedObjectives;
    std::vector<std::string> rewardsEarned;
    std::string rank; // "S", "A", "B", "C", etc. based on contribution
    bool eligible{true};
};

struct EventSchedule {
    std::string scheduleId;
    std::string eventId;
    std::string scheduleType; // "ONE_TIME", "RECURRING", "DAILY", "WEEKLY", "MONTHLY", "SEASONAL"
    std::optional<std::chrono::system_clock::time_point> scheduledTime;
    std::optional<std::chrono::minutes> interval;
    std::optional<std::string> cronExpression;
    std::optional<uint32_t> dayOfWeek; // 0-6, Sunday = 0
    std::optional<uint32_t> dayOfMonth; // 1-31
    std::optional<uint32_t> month; // 1-12
    std::optional<uint32_t> hour; // 0-23
    std::optional<uint32_t> minute; // 0-59
    EventTrigger trigger{EventTrigger::SCHEDULED};
    bool enabled{true};
    std::chrono::system_clock::time_point nextOccurrence;
    uint32_t occurrenceCount{0};
    uint32_t maxOccurrences{0}; // 0 = unlimited
};

struct EventReactionChain {
    std::string chainId;
    std::string triggerEventId;
    
    struct ChainLink {
        std::string eventId;
        std::chrono::minutes delay{0};
        float probability{1.0f};
        std::string condition; // Expression to evaluate
        float intensityMultiplier{1.0f};
        bool propagateLocation{true};
        bool propagateAffectedRegions{true};
        bool requiresPhase; // Only trigger from a specific phase
        EventPhase requiredPhase{EventPhase::ACTIVE};
    };
    
    std::vector<ChainLink> chain;
    bool enabled{true};
    bool allowLoops{false};
    uint32_t maxChainDepth{5};
    float initialProbability{1.0f};
    std::string description;
};

class WorldEventSystem {
public:
    // Singleton
    static WorldEventSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Definições de eventos
    std::string createEventDefinition(const WorldEventDefinition& definition);
    bool updateEventDefinition(const WorldEventDefinition& definition);
    bool deleteEventDefinition(const std::string& eventId);
    WorldEventDefinition getEventDefinition(const std::string& eventId) const;
    std::vector<WorldEventDefinition> getAllEventDefinitions() const;
    std::vector<WorldEventDefinition> getEventDefinitionsByType(WorldEventType type) const;
    std::vector<WorldEventDefinition> getEventDefinitionsByTag(const std::string& tag) const;
    
    // Instâncias de eventos
    std::string triggerEvent(const std::string& eventId, 
                          EventTrigger trigger = EventTrigger::ADMIN,
                          const std::optional<std::string>& triggerSource = std::nullopt,
                          const json& parameters = json());
    
    bool cancelEvent(const std::string& instanceId);
    bool advanceEventPhase(const std::string& instanceId, EventPhase targetPhase);
    bool setEventIntensity(const std::string& instanceId, float intensity);
    WorldEventInstance getEventInstance(const std::string& instanceId) const;
    std::vector<WorldEventInstance> getActiveEvents() const;
    std::vector<WorldEventInstance> getEventsInRegion(const std::string& regionId) const;
    std::vector<WorldEventInstance> getEventsByType(WorldEventType type) const;
    
    // Participação em eventos
    std::string registerParticipation(const std::string& instanceId, const std::string& playerId);
    bool updateParticipationContribution(const std::string& participationId, int64_t contributionDelta);
    bool completeEventObjective(const std::string& participationId, const std::string& objectiveId);
    EventParticipation getParticipation(const std::string& participationId) const;
    std::vector<EventParticipation> getPlayerParticipations(const std::string& playerId) const;
    std::vector<EventParticipation> getEventParticipations(const std::string& instanceId) const;
    
    // Agendamento
    std::string scheduleEvent(const EventSchedule& schedule);
    bool updateEventSchedule(const EventSchedule& schedule);
    bool deleteEventSchedule(const std::string& scheduleId);
    EventSchedule getEventSchedule(const std::string& scheduleId) const;
    std::vector<EventSchedule> getAllEventSchedules() const;
    std::vector<EventSchedule> getUpcomingEvents(std::chrono::hours lookahead = 24h) const;
    
    // Encadeamento de eventos
    std::string createReactionChain(const EventReactionChain& chain);
    bool updateReactionChain(const EventReactionChain& chain);
    bool deleteReactionChain(const std::string& chainId);
    EventReactionChain getReactionChain(const std::string& chainId) const;
    std::vector<EventReactionChain> getAllReactionChains() const;
    std::vector<EventReactionChain> getReactionChainsForEvent(const std::string& eventId) const;
    
    // Estado do mundo e efeitos
    json getEventWorldEffects(const std::string& instanceId) const;
    json getCurrentWorldEventEffects(const std::string& regionId = "") const;
    bool isAreaAffectedByEvent(float x, float y, float z, WorldEventType type = WorldEventType::CUSTOM) const;
    std::vector<WorldEventInstance> getEventsAffectingLocation(float x, float y, float z) const;
    std::vector<WorldEventInstance> getEventsAffectingPlayer(const std::string& playerId) const;
    
    // Recompensas e resultados
    json determineEventOutcome(const std::string& instanceId);
    json calculatePlayerRewards(const std::string& participationId);
    bool distributeEventRewards(const std::string& instanceId);
    json getEventOutcomeDetails(const std::string& instanceId) const;
    
    // Análise e estatísticas
    json getEventSystemStats() const;
    json getPlayerEventStats(const std::string& playerId) const;
    json getRegionEventHistory(const std::string& regionId, uint32_t limit = 10) const;
    
    // Notificações e callbacks
    void registerEventTriggeredCallback(std::function<void(const WorldEventInstance&)> callback);
    void registerEventPhaseChangedCallback(std::function<void(const std::string&, EventPhase)> callback);
    void registerEventCompletedCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerPlayerParticipationCallback(std::function<void(const std::string&, const std::string&)> callback);
    
private:
    WorldEventSystem();
    ~WorldEventSystem();
    
    // Atributos privados
    static std::unique_ptr<WorldEventSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<World::WorldState> m_worldState;
    std::shared_ptr<Player::PlayerManager> m_playerManager;
    
    // Gerador de números aleatórios
    std::mt19937 m_rng;
    
    // Definições de eventos
    std::unordered_map<std::string, WorldEventDefinition> m_eventDefinitions;
    std::unordered_map<WorldEventType, std::vector<std::string>> m_definitionsByType;
    std::unordered_map<std::string, std::vector<std::string>> m_definitionsByTag;
    mutable std::shared_mutex m_definitionsMutex;
    
    // Instâncias de eventos
    std::unordered_map<std::string, WorldEventInstance> m_eventInstances;
    std::set<std::string> m_activeInstanceIds;
    std::unordered_map<std::string, std::vector<std::string>> m_instancesByRegion; // regionId -> [instanceIds]
    std::unordered_map<WorldEventType, std::vector<std::string>> m_instancesByType;
    mutable std::shared_mutex m_instancesMutex;
    
    // Participação em eventos
    std::unordered_map<std::string, EventParticipation> m_participations;
    std::unordered_map<std::string, std::vector<std::string>> m_participationsByPlayer; // playerId -> [participationIds]
    std::unordered_map<std::string, std::vector<std::string>> m_participationsByEvent; // instanceId -> [participationIds]
    mutable std::shared_mutex m_participationsMutex;
    
    // Agendamento
    std::unordered_map<std::string, EventSchedule> m_eventSchedules;
    std::multimap<std::chrono::system_clock::time_point, std::string> m_scheduleTimeline; // nextOccurrence -> scheduleId
    mutable std::shared_mutex m_schedulesMutex;
    
    // Encadeamento de eventos
    std::unordered_map<std::string, EventReactionChain> m_reactionChains;
    std::unordered_map<std::string, std::vector<std::string>> m_chainsByTriggerEvent; // eventId -> [chainIds]
    mutable std::shared_mutex m_chainsMutex;
    
    // Histórico de eventos
    struct EventRecord {
        std::string instanceId;
        std::string eventId;
        WorldEventType type;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::set<std::string> affectedRegions;
        std::string outcomeId;
        std::string status;
        float intensity;
        uint32_t participantCount;
        json summary;
    };
    
    std::vector<EventRecord> m_eventHistory;
    std::unordered_map<std::string, std::vector<size_t>> m_historyByRegion; // regionId -> [indices in m_eventHistory]
    mutable std::mutex m_historyMutex;
    size_t m_maxHistorySize{1000};
    
    // Callbacks registrados
    std::vector<std::function<void(const WorldEventInstance&)>> m_eventTriggeredCallbacks;
    std::vector<std::function<void(const std::string&, EventPhase)>> m_phaseChangedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&)>> m_eventCompletedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&)>> m_participationCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas
    struct EventStats {
        std::atomic<uint32_t> totalEventsTriggered{0};
        std::atomic<uint32_t> totalRandomEvents{0};
        std::atomic<uint32_t> totalScheduledEvents{0};
        std::atomic<uint32_t> totalPlayerTriggeredEvents{0};
        std::atomic<uint32_t> totalChainedEvents{0};
        std::atomic<uint32_t> totalParticipations{0};
        std::atomic<uint32_t> totalCompletedEvents{0};
        std::atomic<uint32_t> totalFailedEvents{0};
        std::atomic<uint32_t> totalCanceledEvents{0};
        std::unordered_map<WorldEventType, std::atomic<uint32_t>> eventsByType;
    };
    
    EventStats m_stats;
    
    // Configuração
    std::chrono::seconds m_eventUpdateInterval{10};
    std::chrono::minutes m_scheduleCheckInterval{1};
    std::chrono::minutes m_randomEventInterval{30};
    float m_randomEventProbability{0.1f};
    float m_eventIntensityVariance{0.2f};
    uint32_t m_maxConcurrentEvents{10};
    bool m_enableRandomEvents{true};
    bool m_enableChainedEvents{true};
    uint32_t m_rewardContributionWeight{70}; // percentage
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_eventUpdateThread;
    std::thread m_scheduleCheckThread;
    std::thread m_randomEventThread;
    std::thread m_chainReactionThread;
    
    // Métodos privados
    std::string generateEventId();
    std::string generateInstanceId();
    std::string generateParticipationId();
    std::string generateScheduleId();
    std::string generateChainId();
    
    void eventUpdateThread();
    void scheduleCheckThread();
    void randomEventThread();
    void chainReactionThread();
    
    bool validateEventDefinition(const WorldEventDefinition& definition) const;
    bool meetEventRequirements(const WorldEventDefinition& definition, const json& parameters = json()) const;
    
    WorldEventInstance createEventInstance(const std::string& eventId, 
                                        EventTrigger trigger,
                                        const std::optional<std::string>& triggerSource,
                                        const json& parameters);
    
    bool advanceEventProgress(WorldEventInstance& instance);
    bool checkPhaseTransition(WorldEventInstance& instance);
    void processActiveEvents();
    
    std::vector<std::string> getRegionsForLocation(float x, float y, float z) const;
    void updateEventRegions(WorldEventInstance& instance);
    void updateAffectedPlayers(WorldEventInstance& instance);
    
    void checkSchedules();
    void updateScheduleTimeline();
    std::chrono::system_clock::time_point calculateNextOccurrence(const EventSchedule& schedule) const;
    
    bool shouldTriggerRandomEvent();
    std::string selectRandomEvent();
    
    void processEventChains(const std::string& instanceId, EventPhase currentPhase);
    bool evaluateChainCondition(const std::string& condition, const WorldEventInstance& sourceInstance) const;
    
    std::string determineRandomOutcome(const WorldEventDefinition& definition);
    void applyWorldEffects(WorldEventInstance& instance);
    void recordEventToHistory(const WorldEventInstance& instance);
    
    json calculateEventRewards(const WorldEventInstance& instance);
    std::string determineParticipationRank(int64_t contribution, int64_t topContribution) const;
    
    void notifyEventTriggered(const WorldEventInstance& instance);
    void notifyEventPhaseChanged(const std::string& instanceId, EventPhase newPhase);
    void notifyEventCompleted(const std::string& instanceId, const std::string& outcomeId);
    void notifyPlayerParticipation(const std::string& instanceId, const std::string& playerId);
    
    void pruneEventHistory();
};

// Implementação inline básica do singleton
inline WorldEventSystem& WorldEventSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<WorldEventSystem>(new WorldEventSystem());
    });
    return *s_instance;
}

} // namespace Events
} // namespace WYDBR

// NaturalDisasterSystem.h - Invasões e Catástrofes Naturais
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <memory>
#include <functional>
#include <optional>
#include <random>
#include <nlohmann/json.hpp>
#include "WorldEventSystem.h"
#include "TerrainSystem.h"

namespace WYDBR {
namespace Events {

using json = nlohmann::json;

enum class DisasterType {
    EARTHQUAKE,
    TSUNAMI,
    VOLCANIC_ERUPTION,
    HURRICANE,
    TORNADO,
    FLOOD,
    WILDFIRE,
    DROUGHT,
    BLIZZARD,
    METEOR_STRIKE,
    MAGICAL_CATASTROPHE,
    CUSTOM
};

enum class DisasterSeverity {
    MINOR,
    MODERATE,
    MAJOR,
    SEVERE,
    CATASTROPHIC
};

enum class TerrainEffect {
    DESTRUCTION,
    ALTERATION,
    CONTAMINATION,
    OBSTRUCTION,
    ENHANCEMENT,
    CREATION,
    TEMPORARY,
    PERMANENT
};

struct NaturalDisasterTemplate {
    std::string templateId;
    std::string name;
    std::string description;
    DisasterType type;
    
    struct DisasterVisual {
        std::string visualType; // "PARTICLES", "MODEL", "TERRAIN_CHANGE", "WEATHER_EFFECT", "SOUND", "SCREEN_EFFECT"
        std::string assetId;
        float scale{1.0f};
        float intensity{1.0f};
        std::chrono::seconds duration{0}; // 0 = permanent
        json visualParameters;
    };
    
    std::vector<DisasterVisual> visuals;
    
    struct TerrainImpact {
        TerrainEffect effect;
        std::string terrainType; // "GROUND", "WATER", "VEGETATION", "STRUCTURE", "ALL"
        float impactRadius{10.0f};
        float intensity{1.0f};
        std::string replacementTerrainId;
        std::chrono::hours duration{0}; // 0 = permanent
        json impactParameters;
    };
    
    std::vector<TerrainImpact> terrainImpacts;
    
    struct GameplayEffect {
        std::string effectType; // "DAMAGE", "STATUS_EFFECT", "MOVEMENT_PENALTY", "RESOURCE_DEPLETION", etc.
        float value{1.0f};
        std::chrono::seconds duration{300};
        float radius{20.0f};
        std::string statusEffectId;
        json effectParameters;
    };
    
    std::vector<GameplayEffect> gameplayEffects;
    
    DisasterSeverity baseSeverity{DisasterSeverity::MODERATE};
    float damageMultiplier{1.0f};
    float spreadRate{0.0f}; // Units per second (0 = doesn't spread)
    std::chrono::minutes minDuration{10};
    std::chrono::minutes maxDuration{60};
    std::vector<DisasterType> possibleSecondaryDisasters;
    float secondaryDisasterProbability{0.1f};
    bool allowPlayersToMitigate{true};
    std::vector<std::string> mitigationMethods; // "WATER", "MAGIC", "ENGINEERING", etc.
    std::vector<std::string> preventionStructures;
    std::vector<std::string> preferredTerrainTypes;
    json templateParameters;
};

struct NaturalDisasterInstance {
    std::string instanceId;
    std::string templateId;
    DisasterType type;
    DisasterSeverity severity;
    float currentIntensity{1.0f};
    
    struct DisasterLocation {
        float originX;
        float originY;
        float originZ;
        float currentRadius{0.0f};
        float maxRadius{100.0f};
        float spreadSpeed{0.0f};
        std::vector<std::pair<float, float>> areaBounds; // For non-circular disasters
        std::string regionId;
    };
    
    DisasterLocation location;
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    std::string status; // "BUILDING", "ACTIVE", "WANING", "ENDED", "MITIGATED"
    float damage{0.0f};
    std::set<std::string> affectedStructureIds;
    std::set<std::string> affectedPlayerIds;
    std::vector<std::string> secondaryDisasterIds;
    std::optional<std::string> relatedEventId;
    json currentEffects;
    json disasterState;
};

struct DisasterMitigation {
    std::string mitigationId;
    std::string disasterId;
    std::string mitigationType; // "PLAYER_ACTION", "AUTOMATIC", "STRUCTURE", "WEATHER_CHANGE"
    std::optional<std::string> playerId;
    std::optional<std::string> structureId;
    std::chrono::system_clock::time_point appliedAt;
    float effectivenessRating{0.0f}; // 0.0-1.0
    float intensityReduction{0.0f};
    std::chrono::minutes durationReduction{0};
    std::string status; // "APPLIED", "FAILED", "PARTIAL", "COMPLETE"
    json mitigationDetails;
};

struct TerrainDamageRecord {
    std::string recordId;
    std::string disasterId;
    float locationX;
    float locationY;
    float locationZ;
    std::string originalTerrainType;
    std::string newTerrainType;
    TerrainEffect effect;
    std::chrono::system_clock::time_point timestamp;
    std::optional<std::chrono::system_clock::time_point> restorationTime;
    float damageRadius{1.0f};
    bool permanent{false};
    std::string damagedStructureId;
    json damageDetails;
};

class NaturalDisasterSystem {
public:
    // Singleton
    static NaturalDisasterSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Templates de desastres
    std::string createDisasterTemplate(const NaturalDisasterTemplate& templ);
    bool updateDisasterTemplate(const NaturalDisasterTemplate& templ);
    bool deleteDisasterTemplate(const std::string& templateId);
    NaturalDisasterTemplate getDisasterTemplate(const std::string& templateId) const;
    std::vector<NaturalDisasterTemplate> getAllDisasterTemplates() const;
    std::vector<NaturalDisasterTemplate> getDisasterTemplatesByType(DisasterType type) const;
    
    // Instâncias de desastres
    std::string triggerDisaster(const std::string& templateId, 
                             float x, float y, float z,
                             DisasterSeverity severity = DisasterSeverity::MODERATE,
                             const std::optional<std::string>& relatedEventId = std::nullopt);
    
    bool endDisaster(const std::string& instanceId);
    bool updateDisasterIntensity(const std::string& instanceId, float intensityDelta);
    NaturalDisasterInstance getDisasterInstance(const std::string& instanceId) const;
    std::vector<NaturalDisasterInstance> getActiveDisasters() const;
    std::vector<NaturalDisasterInstance> getDisastersInRegion(const std::string& regionId) const;
    std::vector<NaturalDisasterInstance> getDisastersByType(DisasterType type) const;
    
    // Mitigação
    std::string applyDisasterMitigation(const std::string& disasterId,
                                     const std::string& mitigationType,
                                     const std::optional<std::string>& playerId = std::nullopt,
                                     const std::optional<std::string>& structureId = std::nullopt,
                                     const json& mitigationDetails = json());
    
    bool updateMitigationEffectiveness(const std::string& mitigationId, float newEffectiveness);
    DisasterMitigation getMitigation(const std::string& mitigationId) const;
    std::vector<DisasterMitigation> getDisasterMitigations(const std::string& disasterId) const;
    std::vector<DisasterMitigation> getPlayerMitigations(const std::string& playerId) const;
    
    // Efeitos no terreno
    std::string recordTerrainDamage(const std::string& disasterId,
                                 float x, float y, float z,
                                 const std::string& originalTerrainType,
                                 const std::string& newTerrainType,
                                 TerrainEffect effect,
                                 float radius = 1.0f,
                                 bool permanent = false,
                                 const std::optional<std::string>& damagedStructureId = std::nullopt);
    
    bool restoreTerrainDamage(const std::string& recordId);
    TerrainDamageRecord getTerrainDamageRecord(const std::string& recordId) const;
    std::vector<TerrainDamageRecord> getDisasterTerrainDamage(const std::string& disasterId) const;
    std::vector<TerrainDamageRecord> getTerrainDamageInArea(float x, float y, float z, float radius) const;
    std::vector<TerrainDamageRecord> getPermanentTerrainChanges() const;
    
    // Análise e previsão
    float calculateDisasterRiskForRegion(const std::string& regionId) const;
    json generateDisasterForecast(std::chrono::hours lookahead = 24h) const;
    std::vector<std::string> getHighRiskRegions() const;
    json getDisasterImpactAnalysis(const std::string& instanceId) const;
    
    // Callbacks
    void registerDisasterTriggeredCallback(std::function<void(const NaturalDisasterInstance&)> callback);
    void registerDisasterIntensityChangedCallback(std::function<void(const std::string&, float)> callback);
    void registerTerrainDamagedCallback(std::function<void(const TerrainDamageRecord&)> callback);
    void registerMitigationAppliedCallback(std::function<void(const DisasterMitigation&)> callback);
    
private:
    NaturalDisasterSystem();
    ~NaturalDisasterSystem();
    
    // Atributos privados
    static std::unique_ptr<NaturalDisasterSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<WorldEventSystem> m_worldEventSystem;
    std::shared_ptr<World::TerrainSystem> m_terrainSystem;
    
    // Gerador de números aleatórios
    std::mt19937 m_rng;
    
    // Templates de desastres
    std::unordered_map<std::string, NaturalDisasterTemplate> m_disasterTemplates;
    std::unordered_map<DisasterType, std::vector<std::string>> m_templatesByType;
    mutable std::shared_mutex m_templatesMutex;
    
    // Instâncias de desastres
    std::unordered_map<std::string, NaturalDisasterInstance> m_disasterInstances;
    std::set<std::string> m_activeDisasterIds;
    std::unordered_map<std::string, std::vector<std::string>> m_disastersByRegion; // regionId -> [instanceIds]
    std::unordered_map<DisasterType, std::vector<std::string>> m_disastersByType;
    mutable std::shared_mutex m_instancesMutex;
    
    // Mitigações
    std::unordered_map<std::string, DisasterMitigation> m_mitigations;
    std::unordered_map<std::string, std::vector<std::string>> m_mitigationsByDisaster; // disasterId -> [mitigationIds]
    std::unordered_map<std::string, std::vector<std::string>> m_mitigationsByPlayer; // playerId -> [mitigationIds]
    mutable std::shared_mutex m_mitigationsMutex;
    
    // Danos ao terreno
    std::unordered_map<std::string, TerrainDamageRecord> m_terrainDamage;
    std::unordered_map<std::string, std::vector<std::string>> m_damageByDisaster; // disasterId -> [recordIds]
    std::multimap<std::pair<int, int>, std::string> m_damageByGridCell; // (gridX, gridY) -> recordIds
    std::set<std::string> m_permanentDamageIds;
    mutable std::shared_mutex m_damageMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const NaturalDisasterInstance&)>> m_disasterTriggeredCallbacks;
    std::vector<std::function<void(const std::string&, float)>> m_intensityChangedCallbacks;
    std::vector<std::function<void(const TerrainDamageRecord&)>> m_terrainDamagedCallbacks;
    std::vector<std::function<void(const DisasterMitigation&)>> m_mitigationAppliedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Configuração
    uint32_t m_gridCellSize{100}; // meters
    std::chrono::seconds m_disasterUpdateInterval{1};
    float m_naturalMitigationRate{0.01f}; // Intensity reduction per update
    float m_maxMitigationEffect{0.9f}; // Max intensity reduction from all mitigations
    float m_disasterDamageMultiplier{1.0f};
    bool m_enableSecondaryDisasters{true};
    uint32_t m_maxConcurrentDisasters{5};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_disasterUpdateThread;
    std::thread m_terrainRestoreThread;
    
    // Métodos privados
    std::string generateTemplateId();
    std::string generateInstanceId();
    std::string generateMitigationId();
    std::string generateDamageRecordId();
    
    void disasterUpdateThread();
    void terrainRestoreThread();
    
    void updateDisasterState(NaturalDisasterInstance& instance);
    void applyDisasterEffects(NaturalDisasterInstance& instance);
    void checkForSecondaryDisasters(NaturalDisasterInstance& instance);
    
    void calculateAffectedEntities(NaturalDisasterInstance& instance);
    void applyTerrainEffects(const NaturalDisasterInstance& instance, const NaturalDisasterTemplate& templ);
    void applyGameplayEffects(const NaturalDisasterInstance& instance, const NaturalDisasterTemplate& templ);
    
    float calculateEffectiveMitigation(const NaturalDisasterInstance& instance) const;
    bool isLocationAffectedByDisaster(float x, float y, float z, const NaturalDisasterInstance& instance) const;
    
    std::pair<int, int> getGridCellForLocation(float x, float y) const;
    std::vector<std::pair<int, int>> getGridCellsInRadius(float x, float y, float radius) const;
    
    void notifyDisasterTriggered(const NaturalDisasterInstance& instance);
    void notifyDisasterIntensityChanged(const std::string& instanceId, float newIntensity);
    void notifyTerrainDamaged(const TerrainDamageRecord& record);
    void notifyMitigationApplied(const DisasterMitigation& mitigation);
    
    void checkTemporaryTerrainDamage();
    float calculateRegionRiskFactor(const std::string& regionId) const;
    bool isRegionProneToDisasterType(const std::string& regionId, DisasterType type) const;
};

// Implementação inline básica do singleton
inline NaturalDisasterSystem& NaturalDisasterSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<NaturalDisasterSystem>(new NaturalDisasterSystem());
    });
    return *s_instance;
}

} // namespace Events
} // namespace WYDBR

// WorldChangeSystem.h - Mudanças Permanentes no Mundo
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>
#include "WorldEventSystem.h"
#include "WorldState.h"

namespace WYDBR {
namespace Events {

using json = nlohmann::json;

enum class ChangeType {
    TERRAIN,
    STRUCTURE,
    NPC,
    RESOURCE,
    FACTION,
    ACCESSIBILITY,
    WEATHER,
    QUEST,
    LORE,
    CUSTOM
};

enum class ChangeOrigin {
    EVENT,
    PLAYER_ACTION,
    ADMIN,
    SCRIPTED,
    TIME_BASED,
    THRESHOLD,
    CUSTOM
};

enum class ChangePermanence {
    TEMPORARY,
    SEMI_PERMANENT,
    PERMANENT,
    CYCLICAL
};

struct WorldChangeDefinition {
    std::string changeId;
    std::string name;
    std::string description;
    ChangeType type;
    ChangePermanence permanence;
    
    struct ChangeTrigger {
        std::string triggerType; // "EVENT_OUTCOME", "PLAYER_COUNT", "RESOURCE_LEVEL", "TIME_ELAPSED", "ADMIN", etc.
        json parameters;
        std::optional<std::string> relatedEventId;
        std::optional<std::string> relatedObjectiveId;
    };
    
    std::vector<ChangeTrigger> possibleTriggers;
    
    struct ChangeEffect {
        std::string effectType; // "ADD", "REMOVE", "MODIFY", "TRANSFORM"
        std::string targetType; // "TERRAIN", "BUILDING", "NPC", "RESOURCE_NODE", etc.
        std::string targetId;
        json beforeState;
        json afterState;
        json parameters;
    };
    
    std::vector<ChangeEffect> effects;
    
    struct VisualChange {
        std::string visualType; // "MODEL", "TEXTURE", "PARTICLE", "LIGHTING", "SOUND", "WEATHER"
        std::string beforeAssetId;
        std::string afterAssetId;
        json visualParameters;
    };
    
    std::vector<VisualChange> visualChanges;
    
    struct LocationInfo {
        std::string locationType; // "POINT", "AREA", "REGION", "GLOBAL"
        float originX{0.0f};
        float originY{0.0f};
        float originZ{0.0f};
        float radius{0.0f};
        std::string regionId;
        std::vector<std::pair<float, float>> areaBounds;
    };
    
    std::vector<LocationInfo> affectedLocations;
    
    std::optional<std::chrono::system_clock::time_point> validFrom;
    std::optional<std::chrono::system_clock::time_point> validUntil;
    std::optional<std::chrono::hours> duration;
    bool reversible{true};
    uint32_t requiredPlayerContribution{0};
    float worldImpact{1.0f}; // 0-10 scale
    std::vector<std::string> tags;
    std::vector<std::string> requiredPreviousChanges;
    std::vector<std::string> excludedWithChanges;
    json changeParameters;
};

struct WorldChangeInstance {
    std::string instanceId;
    std::string changeId;
    ChangeType type;
    ChangePermanence permanence;
    ChangeOrigin origin;
    std::chrono::system_clock::time_point triggeredAt;
    std::optional<std::chrono::system_clock::time_point> completedAt;
    std::optional<std::chrono::system_clock::time_point> expiresAt;
    std::string status; // "PENDING", "IN_PROGRESS", "COMPLETED", "REVERTED", "PARTIAL", "FAILED"
    float progress{0.0f};
    std::optional<std::string> triggerEventId;
    std::optional<std::string> initiatorId; // Player or admin ID
    std::set<std::string> contributingPlayers;
    std::unordered_map<std::string, uint32_t> playerContributions; // playerId -> contribution
    std::vector<std::string> affectedRegions;
    std::vector<std::string> affectedEntityIds;
    bool applied{false};
    bool announced{false};
    json instanceParameters;
    json changeResult;
};

struct ChangeReactionChain {
    std::string chainId;
    std::string triggerChangeId;
    
    struct ChainedChange {
        std::string changeId;
        std::chrono::minutes delay{0};
        std::string condition;
        float probability{1.0f};
        bool requiresSuccess{true};
        float worldStateThreshold{0.0f};
    };
    
    std::vector<ChainedChange> chainedChanges;
    bool enabled{true};
    std::string description;
};

struct WorldChangeHistory {
    std::string historyId;
    std::string regionId;
    std::vector<std::string> changeInstanceIds;
    std::chrono::system_clock::time_point firstChangeTime;
    std::chrono::system_clock::time_point lastChangeTime;
    std::unordered_map<ChangeType, uint32_t> changeTypeCount;
    json regionStateBefore;
    json regionStateCurrent;
    json historicalData;
};

class WorldChangeSystem {
public:
    // Singleton
    static WorldChangeSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Definições de mudanças
    std::string createChangeDefinition(const WorldChangeDefinition& definition);
    bool updateChangeDefinition(const WorldChangeDefinition& definition);
    bool deleteChangeDefinition(const std::string& changeId);
    WorldChangeDefinition getChangeDefinition(const std::string& changeId) const;
    std::vector<WorldChangeDefinition> getAllChangeDefinitions() const;
    std::vector<WorldChangeDefinition> getChangeDefinitionsByType(ChangeType type) const;
    std::vector<WorldChangeDefinition> getChangeDefinitionsByTag(const std::string& tag) const;
    
    // Instâncias de mudanças
    std::string triggerWorldChange(const std::string& changeId, 
                                ChangeOrigin origin = ChangeOrigin::ADMIN,
                                const std::optional<std::string>& triggerEventId = std::nullopt,
                                const std::optional<std::string>& initiatorId = std::nullopt,
                                const json& parameters = json());
    
    bool updateChangeProgress(const std::string& instanceId, float progress);
    bool completeWorldChange(const std::string& instanceId);
    bool revertWorldChange(const std::string& instanceId);
    WorldChangeInstance getChangeInstance(const std::string& instanceId) const;
    std::vector<WorldChangeInstance> getActiveChanges() const;
    std::vector<WorldChangeInstance> getChangesInRegion(const std::string& regionId) const;
    std::vector<WorldChangeInstance> getPermanentChanges() const;
    
    // Contribuição de jogadores
    bool addPlayerContribution(const std::string& instanceId, 
                             const std::string& playerId, 
                             uint32_t contribution);
    
    uint32_t getPlayerContribution(const std::string& instanceId, 
                                 const std::string& playerId) const;
    
    std::vector<std::pair<std::string, uint32_t>> getTopContributors(const std::string& instanceId, 
                                                                  uint32_t limit = 10) const;
    
    // Reações em cadeia
    std::string createReactionChain(const ChangeReactionChain& chain);
    bool updateReactionChain(const ChangeReactionChain& chain);
    bool deleteReactionChain(const std::string& chainId);
    ChangeReactionChain getReactionChain(const std::string& chainId) const;
    std::vector<ChangeReactionChain> getAllReactionChains() const;
    std::vector<ChangeReactionChain> getChainsForChange(const std::string& changeId) const;
    
    // Histórico de mudanças
    WorldChangeHistory getRegionChangeHistory(const std::string& regionId) const;
    std::vector<WorldChangeInstance> getHistoricalChanges(const std::string& regionId) const;
    json getWorldChangeTimeline() const;
    
    // Impactos no mundo
    bool isAreaAffectedByChange(float x, float y, float z, ChangeType type = ChangeType::CUSTOM) const;
    json getCurrentWorldEffects(const std::string& regionId = "") const;
    
    // Análise e visualização
    json visualizeWorldChanges(const std::string& regionId = "") const;
    json analyzeChangeImpact(const std::string& instanceId) const;
    std::vector<std::string> getMostChangedRegions(uint32_t limit = 5) const;
    
    // Callbacks
    void registerChangeTriggeredCallback(std::function<void(const WorldChangeInstance&)> callback);
    void registerChangeCompletedCallback(std::function<void(const std::string&, bool)> callback);
    void registerChangeRevertedCallback(std::function<void(const std::string&)> callback);
    
private:
    WorldChangeSystem();
    ~WorldChangeSystem();
    
    // Atributos privados
    static std::unique_ptr<WorldChangeSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<World::WorldState> m_worldState;
    std::shared_ptr<WorldEventSystem> m_worldEventSystem;
    
    // Definições de mudanças
    std::unordered_map<std::string, WorldChangeDefinition> m_changeDefinitions;
    std::unordered_map<ChangeType, std::vector<std::string>> m_definitionsByType;
    std::unordered_map<std::string, std::vector<std::string>> m_definitionsByTag;
    mutable std::shared_mutex m_definitionsMutex;
    
    // Instâncias de mudanças
    std::unordered_map<std::string, WorldChangeInstance> m_changeInstances;
    std::set<std::string> m_activeInstanceIds;
    std::set<std::string> m_permanentChangeIds;
    std::unordered_map<std::string, std::vector<std::string>> m_instancesByRegion; // regionId -> [instanceIds]
    mutable std::shared_mutex m_instancesMutex;
    
    // Reações em cadeia
    std::unordered_map<std::string, ChangeReactionChain> m_reactionChains;
    std::unordered_map<std::string, std::vector<std::string>> m_chainsByTriggerChange; // changeId -> [chainIds]
    mutable std::shared_mutex m_chainsMutex;
    
    // Histórico de mudanças
    std::unordered_map<std::string, WorldChangeHistory> m_changeHistory; // regionId -> history
    mutable std::shared_mutex m_historyMutex;
    
    // Dependências entre mudanças
    std::unordered_map<std::string, std::set<std::string>> m_changeDependencies; // changeId -> set of required changeIds
    std::unordered_map<std::string, std::set<std::string>> m_changeExclusions; // changeId -> set of excluded changeIds
    mutable std::mutex m_dependenciesMutex;
    
    // Programação de mudanças temporárias
    struct ScheduledAction {
        std::string instanceId;
        std::chrono::system_clock::time_point scheduledTime;
        std::string actionType; // "START", "COMPLETE", "REVERT"
    };
    
    std::multimap<std::chrono::system_clock::time_point, ScheduledAction> m_scheduledActions;
    mutable std::mutex m_scheduleMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const WorldChangeInstance&)>> m_changeTriggeredCallbacks;
    std::vector<std::function<void(const std::string&, bool)>> m_changeCompletedCallbacks;
    std::vector<std::function<void(const std::string&)>> m_changeRevertedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas e configuração
    uint32_t m_maxActiveChanges{10};
    std::chrono::seconds m_changeUpdateInterval{10};
    std::chrono::minutes m_scheduleCheckInterval{1};
    float m_chainReactionProbability{0.8f};
    bool m_enableChainReactions{true};
    bool m_requireDependenciesMet{true};
    bool m_autoRevertTemporaryChanges{true};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_changeProgressThread;
    std::thread m_scheduleCheckThread;
    std::thread m_chainReactionThread;
    
    // Métodos privados
    std::string generateChangeId();
    std::string generateInstanceId();
    std::string generateChainId();
    std::string generateHistoryId();
    
    void changeProgressThread();
    void scheduleCheckThread();
    void chainReactionThread();
    
    bool validateChangeDefinition(const WorldChangeDefinition& definition) const;
    bool validateInstanceForInitiation(const std::string& changeId, 
                                    ChangeOrigin origin, 
                                    const std::optional<std::string>& initiatorId) const;
    
    bool checkChangeDependencies(const std::string& changeId) const;
    bool checkChangeExclusions(const std::string& changeId) const;
    
    bool applyWorldChange(WorldChangeInstance& instance);
    bool revertWorldChangeEffects(WorldChangeInstance& instance);
    
    void updateChangesInRegion(WorldChangeHistory& history, const WorldChangeInstance& instance);
    void scheduleTemporaryChange(const WorldChangeInstance& instance);
    
    bool processScheduledAction(const ScheduledAction& action);
    void checkPendingContributions();
    
    void processReactionChains(const std::string& instanceId);
    bool evaluateChainCondition(const std::string& condition, const WorldChangeInstance& sourceInstance) const;
    
    void notifyChangeTriggered(const WorldChangeInstance& instance);
    void notifyChangeCompleted(const std::string& instanceId, bool success);
    void notifyChangeReverted(const std::string& instanceId);
    
    json createChangeEffectWorld(const WorldChangeDefinition& definition, 
                               const WorldChangeInstance& instance);
    
    bool isChangeValid(const std::string& changeId) const;
    bool isChangeInstanceActive(const std::string& instanceId) const;
};

// Implementação inline básica do singleton
inline WorldChangeSystem& WorldChangeSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<WorldChangeSystem>(new WorldChangeSystem());
    });
    return *s_instance;
}

} // namespace Events
} // namespace WYDBR
`;

// Fornece uma API JavaScript para integração no projeto
module.exports.setupSystem = function(engineContext) {
  console.log('Inicializando sistema: sistema_eventos_mundiais');
  
  // Inicializa o sistema no contexto do engine
  engineContext.registerSystem({
    name: 'sistema_eventos_mundiais',
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
