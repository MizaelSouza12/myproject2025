/**
 * WYDBR 2.0 - Implementação Gerada Automaticamente
 * Arquivo origem: sistema_clima_ambiente.txt
 * 
 * Este arquivo contém a definição C++ do sistema, encapsulada em JavaScript
 * para integração no projeto WYDBR 2.0.
 */

// Exporta o código C++ como uma string para ser processado pelo compilador C++
module.exports.cppCode = `
// ClimateSystem.h - Sistema de Clima e Ambiente
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
#include "TerrainSystem.h"

namespace WYDBR {
namespace Environment {

using json = nlohmann::json;
using namespace std::chrono_literals;

enum class ClimateType {
    TROPICAL,
    DESERT,
    TEMPERATE,
    CONTINENTAL,
    POLAR,
    MEDITERRANEAN,
    ALPINE,
    MAGICAL,
    CUSTOM
};

enum class WeatherType {
    CLEAR,
    CLOUDY,
    RAINY,
    STORMY,
    SNOWY,
    FOGGY,
    WINDY,
    HEATWAVE,
    BLIZZARD,
    MAGICAL,
    CUSTOM
};

enum class Season {
    SPRING,
    SUMMER,
    AUTUMN,
    WINTER,
    DRY,
    WET,
    CUSTOM
};

enum class TimeOfDay {
    DAWN,
    MORNING,
    NOON,
    AFTERNOON,
    DUSK,
    EVENING,
    MIDNIGHT,
    DEEP_NIGHT
};

struct WeatherState {
    WeatherType type;
    float intensity{1.0f}; // 0.0-2.0, 1.0 = normal
    float temperature{20.0f}; // Celsius
    float humidity{0.5f}; // 0.0-1.0
    float windSpeed{0.0f}; // m/s
    float windDirection{0.0f}; // degrees, 0 = North
    float precipitation{0.0f}; // mm/hour
    float visibility{1000.0f}; // meters
    float cloudCover{0.0f}; // 0.0-1.0
    json visualEffects;
    json gameplayEffects;
};

struct ClimateZone {
    std::string zoneId;
    std::string name;
    ClimateType type;
    std::string regionId;
    
    struct SeasonConfig {
        Season season;
        std::chrono::days duration{90};
        std::vector<float> temperatureRange; // [min, max]
        std::vector<float> humidityRange; // [min, max]
        std::vector<float> precipitationRange; // [min, max]
        std::unordered_map<WeatherType, float> weatherProbabilities;
        json seasonEffects;
    };
    
    std::vector<SeasonConfig> seasons;
    
    std::unordered_map<WeatherType, float> baseWeatherProbabilities;
    float baseTemperature{20.0f};
    float temperatureVariance{5.0f};
    float baseHumidity{0.5f};
    float baseWindSpeed{1.0f};
    float extremeWeatherChance{0.05f};
    std::vector<std::string> possibleDisasters;
    json zoneParameters;
};

struct TimeSystem {
    float dayLengthMinutes{60.0f}; // Real-time minutes per game day
    float timeScale{24.0f}; // Game hours per real-time hour
    uint32_t daysPerYear{360};
    uint32_t monthsPerYear{12};
    uint32_t daysPerMonth{30};
    uint32_t hoursPerDay{24};
    uint32_t minutesPerHour{60};
    uint32_t secondsPerMinute{60};
    std::string currentDate; // "Year-Month-Day"
    std::string currentTime; // "Hour:Minute:Second"
    TimeOfDay currentTimeOfDay{TimeOfDay::NOON};
    Season currentSeason{Season::SPRING};
    uint32_t currentYear{1};
    uint32_t currentMonth{1};
    uint32_t currentDay{1};
    uint32_t currentHour{12};
    uint32_t currentMinute{0};
    uint32_t currentSecond{0};
    float dayNightCycleProgress{0.5f}; // 0.0-1.0, 0 = midnight, 0.5 = noon
    json timeEffects;
};

struct EnvironmentalEffect {
    std::string effectId;
    std::string name;
    std::string description;
    std::string category; // "WEATHER", "SEASON", "TIME", "CLIMATE", "DISASTER"
    
    struct StatModifier {
        std::string statName;
        float value;
        bool isPercentage{true};
        bool isBuff{true};
    };
    
    std::vector<StatModifier> statModifiers;
    
    struct VisualEffect {
        std::string effectType; // "PARTICLE", "OVERLAY", "LIGHTING", "COLOR_SHIFT", "ANIMATION", "POST_PROCESS"
        std::string assetId;
        float intensity{1.0f};
        json visualParameters;
    };
    
    std::vector<VisualEffect> visualEffects;
    
    struct GameplayEffect {
        std::string effectType; // "MOVEMENT_SPEED", "DAMAGE", "RESOURCE_GENERATION", "SKILL_EFFICIENCY"
        float value;
        std::string applicationType; // "GLOBAL", "REGION", "PLAYER", "ENTITY_TYPE"
        std::vector<std::string> targetIds;
        json effectParameters;
    };
    
    std::vector<GameplayEffect> gameplayEffects;
    
    float intensityMultiplier{1.0f};
    std::chrono::seconds fadeDuration{5};
    bool stackable{false};
    uint32_t maxStacks{1};
    json effectParameters;
};

struct LocalWeatherState {
    std::string regionId;
    WeatherType currentWeather;
    WeatherType previousWeather;
    float intensity{1.0f};
    float temperature{20.0f};
    float humidity{0.5f};
    float windSpeed{0.0f};
    float windDirection{0.0f};
    float precipitation{0.0f};
    float visibility{1000.0f};
    float cloudCover{0.0f};
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point transitionEndTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    std::chrono::minutes duration{60};
    std::vector<std::string> activeEffectIds;
    json weatherState;
};

class ClimateSystem {
public:
    // Singleton
    static ClimateSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Zonas climáticas
    std::string createClimateZone(const ClimateZone& zone);
    bool updateClimateZone(const ClimateZone& zone);
    bool deleteClimateZone(const std::string& zoneId);
    ClimateZone getClimateZone(const std::string& zoneId) const;
    std::vector<ClimateZone> getAllClimateZones() const;
    ClimateZone getClimateZoneForRegion(const std::string& regionId) const;
    
    // Tempo
    LocalWeatherState getWeatherInRegion(const std::string& regionId) const;
    LocalWeatherState getWeatherAtLocation(float x, float y, float z) const;
    std::vector<LocalWeatherState> getAllRegionWeather() const;
    WeatherType getPredominantWeather() const;
    
    // Controle de tempo
    bool setRegionWeather(const std::string& regionId, 
                        WeatherType weatherType, 
                        float intensity = 1.0f,
                        std::chrono::minutes duration = 60min,
                        std::chrono::seconds transitionTime = 10s);
    
    bool setGlobalWeather(WeatherType weatherType, 
                         float intensity = 1.0f,
                         std::chrono::minutes duration = 60min,
                         std::chrono::seconds transitionTime = 10s);
    
    bool transitionWeather(const std::string& regionId,
                         WeatherType targetWeather,
                         std::chrono::seconds transitionTime = 30s);
    
    // Sistema de tempo (time)
    TimeSystem getTimeSystem() const;
    bool setTimeScale(float newTimeScale);
    bool setCurrentTime(uint32_t hour, uint32_t minute = 0, uint32_t second = 0);
    bool setCurrentDate(uint32_t year, uint32_t month, uint32_t day);
    bool advanceTime(std::chrono::minutes amount);
    bool setSeason(Season season);
    TimeOfDay getCurrentTimeOfDay() const;
    Season getCurrentSeason() const;
    float getDayNightCycleProgress() const;
    
    // Efeitos ambientais
    std::string createEnvironmentalEffect(const EnvironmentalEffect& effect);
    bool updateEnvironmentalEffect(const EnvironmentalEffect& effect);
    bool deleteEnvironmentalEffect(const std::string& effectId);
    EnvironmentalEffect getEnvironmentalEffect(const std::string& effectId) const;
    std::vector<EnvironmentalEffect> getAllEnvironmentalEffects() const;
    
    bool applyEnvironmentalEffect(const std::string& effectId, 
                               const std::string& regionId,
                               float intensity = 1.0f,
                               std::chrono::minutes duration = 30min);
    
    bool removeEnvironmentalEffect(const std::string& effectId, const std::string& regionId);
    std::vector<EnvironmentalEffect> getActiveEffectsInRegion(const std::string& regionId) const;
    std::vector<EnvironmentalEffect> getActiveEffectsAtLocation(float x, float y, float z) const;
    
    // Ciclos sazonais
    Season getSeasonInRegion(const std::string& regionId) const;
    std::vector<std::string> getRegionsInSeason(Season season) const;
    bool advanceSeason(const std::string& regionId = "");
    std::chrono::days getTimeUntilNextSeason(const std::string& regionId) const;
    float getSeasonProgress(const std::string& regionId) const;
    
    // Desastres naturais
    float getDisasterProbability(const std::string& regionId, WeatherType weatherType) const;
    std::vector<std::string> getPossibleDisasters(const std::string& regionId) const;
    bool willWeatherTriggerDisaster(const std::string& regionId) const;
    
    // Previsões e análise
    json generateWeatherForecast(const std::string& regionId, std::chrono::hours lookahead = 24h) const;
    json analyzeClimatePatterns(const std::string& regionId) const;
    json getHistoricalWeatherData(const std::string& regionId, std::chrono::days lookback = 30d) const;
    
    // Estatísticas e callbacks
    json getWeatherStatistics() const;
    void registerWeatherChangedCallback(std::function<void(const std::string&, WeatherType, WeatherType)> callback);
    void registerSeasonChangedCallback(std::function<void(const std::string&, Season)> callback);
    void registerDayNightCycleCallback(std::function<void(TimeOfDay)> callback);
    void registerEnvironmentalEffectCallback(std::function<void(const std::string&, const std::string&, bool)> callback);
    
private:
    ClimateSystem();
    ~ClimateSystem();
    
    // Atributos privados
    static std::unique_ptr<ClimateSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<World::WorldState> m_worldState;
    std::shared_ptr<World::TerrainSystem> m_terrainSystem;
    
    // Gerador de números aleatórios
    std::mt19937 m_rng;
    
    // Zonas climáticas
    std::unordered_map<std::string, ClimateZone> m_climateZones;
    std::unordered_map<std::string, std::string> m_regionToZoneMap; // regionId -> zoneId
    std::unordered_map<ClimateType, std::vector<std::string>> m_zonesByType;
    mutable std::shared_mutex m_zonesMutex;
    
    // Estados locais de tempo
    std::unordered_map<std::string, LocalWeatherState> m_regionWeather; // regionId -> state
    mutable std::shared_mutex m_weatherMutex;
    
    // Sistema de tempo (time)
    TimeSystem m_timeSystem;
    std::chrono::system_clock::time_point m_realTimeReference;
    std::chrono::seconds m_gameTimeReference{0};
    mutable std::mutex m_timeMutex;
    
    // Efeitos ambientais
    std::unordered_map<std::string, EnvironmentalEffect> m_environmentalEffects;
    std::unordered_map<std::string, std::string> m_effectCategories; // effectId -> category
    mutable std::mutex m_effectsMutex;
    
    // Aplicação de efeito
    struct AppliedEffect {
        std::string effectId;
        std::string regionId;
        float intensity;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
    };
    
    std::vector<AppliedEffect> m_appliedEffects;
    std::unordered_map<std::string, std::vector<size_t>> m_effectsByRegion; // regionId -> [indices]
    mutable std::mutex m_appliedEffectsMutex;
    
    // Dados históricos
    struct WeatherRecord {
        std::string regionId;
        WeatherType weatherType;
        float intensity;
        float temperature;
        float precipitation;
        std::chrono::system_clock::time_point recordTime;
    };
    
    std::vector<WeatherRecord> m_weatherHistory;
    std::unordered_map<std::string, std::vector<size_t>> m_historyByRegion; // regionId -> [indices]
    mutable std::mutex m_historyMutex;
    size_t m_maxHistorySize{10000};
    
    // Controle sazonal
    std::unordered_map<std::string, Season> m_regionSeasons; // regionId -> current season
    std::unordered_map<std::string, std::chrono::system_clock::time_point> m_seasonStartTimes; // regionId -> start time
    mutable std::mutex m_seasonsMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const std::string&, WeatherType, WeatherType)>> m_weatherChangedCallbacks;
    std::vector<std::function<void(const std::string&, Season)>> m_seasonChangedCallbacks;
    std::vector<std::function<void(TimeOfDay)>> m_dayNightCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&, bool)>> m_effectCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas e configuração
    struct WeatherStats {
        std::atomic<uint32_t> weatherChanges{0};
        std::atomic<uint32_t> extremeWeatherEvents{0};
        std::atomic<uint32_t> seasonChanges{0};
        std::atomic<uint32_t> daysPassed{0};
        std::atomic<uint32_t> effectsApplied{0};
        std::unordered_map<WeatherType, std::atomic<uint32_t>> weatherTypeCounts;
    };
    
    WeatherStats m_stats;
    
    std::chrono::minutes m_weatherUpdateInterval{5};
    std::chrono::seconds m_timeUpdateInterval{1};
    std::chrono::minutes m_naturalWeatherDuration{60}; // 1 hour
    std::chrono::seconds m_naturalWeatherTransition{30};
    float m_weatherVariationChance{0.2f}; // 20% chance per update
    float m_weatherIntensityVariance{0.3f};
    float m_temperatureVariance{5.0f};
    float m_precipitationVariance{2.0f};
    bool m_enableWeatherEffects{true};
    bool m_enableSeasonalEffects{true};
    bool m_enableTimeEffects{true};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_weatherUpdateThread;
    std::thread m_timeUpdateThread;
    std::thread m_effectsUpdateThread;
    
    // Métodos privados
    std::string generateZoneId();
    std::string generateEffectId();
    
    void weatherUpdateThread();
    void timeUpdateThread();
    void effectsUpdateThread();
    
    bool updateRegionWeather(const std::string& regionId);
    void updateAllRegionWeather();
    
    void updateGameTime();
    TimeOfDay calculateTimeOfDay(uint32_t hour, uint32_t minute) const;
    void updateDayNightCycle();
    
    Season calculateSeasonForDate(uint32_t month, uint32_t day) const;
    void updateRegionSeasons();
    std::chrono::days getSeasonDuration(const std::string& regionId, Season season) const;
    
    WeatherType determineNextWeather(const std::string& regionId, const WeatherType currentWeather) const;
    float calculateWeatherProbability(const std::string& regionId, WeatherType weatherType) const;
    bool checkExtremeWeatherConditions(const std::string& regionId) const;
    
    LocalWeatherState generateWeatherState(const std::string& regionId, 
                                        WeatherType weatherType, 
                                        float intensity);
    
    void transitionWeatherState(LocalWeatherState& currentState, 
                             const LocalWeatherState& targetState, 
                             float progress);
    
    void recordWeatherHistory(const std::string& regionId, 
                           WeatherType weatherType, 
                           float intensity, 
                           float temperature, 
                           float precipitation);
    
    void checkExpiredWeather();
    void checkExpiredEffects();
    
    float calculateTemperature(const std::string& regionId, Season season, TimeOfDay timeOfDay) const;
    float calculateHumidity(const std::string& regionId, Season season, WeatherType weatherType) const;
    float calculateWindSpeed(const std::string& regionId, WeatherType weatherType) const;
    
    void applyWeatherEffects(const std::string& regionId, const LocalWeatherState& state);
    void applySeasonEffects(const std::string& regionId, Season season);
    void applyTimeEffects(TimeOfDay timeOfDay);
    
    json createWeatherVisualEffects(WeatherType weatherType, float intensity);
    json createWeatherGameplayEffects(WeatherType weatherType, float intensity);
    
    void notifyWeatherChanged(const std::string& regionId, WeatherType oldWeather, WeatherType newWeather);
    void notifySeasonChanged(const std::string& regionId, Season newSeason);
    void notifyDayNightCycleChanged(TimeOfDay newTime);
    void notifyEnvironmentalEffectChanged(const std::string& regionId, const std::string& effectId, bool applied);
    
    std::string getRegionForLocation(float x, float y, float z) const;
    void pruneWeatherHistory();
};

// Implementação inline básica do singleton
inline ClimateSystem& ClimateSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<ClimateSystem>(new ClimateSystem());
    });
    return *s_instance;
}

} // namespace Environment
} // namespace WYDBR

// DayNightSystem.h - Ciclos de Dia e Noite
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <nlohmann/json.hpp>
#include "ClimateSystem.h"

namespace WYDBR {
namespace Environment {

using json = nlohmann::json;

enum class LightingPreset {
    DAWN,
    SUNRISE,
    MORNING,
    NOON,
    AFTERNOON,
    SUNSET,
    DUSK,
    NIGHT,
    MIDNIGHT,
    STORMY,
    FOGGY,
    MAGICAL,
    CUSTOM
};

struct CelestialBody {
    std::string celestialId;
    std::string name;
    std::string type; // "SUN", "MOON", "PLANET", "STAR", "COMET", "CUSTOM"
    
    struct OrbitalParameters {
        float orbitSpeed{1.0f}; // multiplier
        float inclination{0.0f}; // degrees
        float phaseOffset{0.0f}; // 0-1
        float eccentricity{0.0f};
        std::optional<float> minAltitude;
        std::optional<float> maxAltitude;
        bool visible{true};
    };
    
    OrbitalParameters orbitParams;
    
    struct VisualParameters {
        float size{1.0f};
        std::string textureId;
        std::string glowTextureId;
        std::array<float, 3> baseColor;
        float intensity{1.0f};
        bool castsShadows{true};
        bool affectsLighting{true};
        json visualEffects;
    };
    
    VisualParameters visualParams;
    
    struct GameplayEffects {
        std::unordered_map<std::string, float> statModifiers;
        std::vector<std::string> triggeredEvents;
        std::vector<std::string> specialEffects;
        float manaInfluence{0.0f};
        float tideInfluence{0.0f};
        float creatureActivityInfluence{0.0f};
    };
    
    GameplayEffects gameplayEffects;
    
    std::vector<std::string> specialPhases;
    bool enabled{true};
    json additionalParams;
};

struct LightingSettings {
    std::string presetId;
    LightingPreset preset;
    std::array<float, 3> ambientColor;
    std::array<float, 3> directionalColor;
    std::array<float, 3> skyColor;
    std::array<float, 3> horizonColor;
    std::array<float, 3> fogColor;
    float directionalIntensity{1.0f};
    float ambientIntensity{0.3f};
    float skyIntensity{1.0f};
    float fogDensity{0.0f};
    float shadowDistance{100.0f};
    float shadowStrength{1.0f};
    std::vector<std::array<float, 3>> colorGrading;
    json postProcessing;
};

struct SkyboxSettings {
    std::string skyboxId;
    std::string timeOfDay;
    std::string weatherCondition;
    std::string textureSet;
    
    struct CloudSettings {
        float coverage{0.5f};
        float density{0.5f};
        float speed{1.0f};
        std::array<float, 3> color;
        float altitude{5000.0f};
        std::string cloudType; // "CUMULUS", "STRATUS", "CIRRUS", "CUSTOM"
        bool volumetric{true};
        json cloudParameters;
    };
    
    CloudSettings clouds;
    
    struct StarSettings {
        float density{0.5f};
        float brightness{1.0f};
        std::string constellations;
        bool shootingStars{false};
        float twinkleIntensity{0.5f};
        bool enabled{true};
    };
    
    StarSettings stars;
    
    bool enableAtmosphericScattering{true};
    bool enableGodRays{true};
    bool enableDynamicReflections{true};
    json additionalSettings;
};

class DayNightSystem {
public:
    // Singleton
    static DayNightSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Corpos celestes
    std::string createCelestialBody(const CelestialBody& body);
    bool updateCelestialBody(const CelestialBody& body);
    bool deleteCelestialBody(const std::string& celestialId);
    CelestialBody getCelestialBody(const std::string& celestialId) const;
    std::vector<CelestialBody> getAllCelestialBodies() const;
    std::vector<CelestialBody> getVisibleCelestialBodies() const;
    
    // Controle de iluminação
    std::string createLightingPreset(const LightingSettings& settings);
    bool updateLightingPreset(const LightingSettings& settings);
    bool deleteLightingPreset(const std::string& presetId);
    LightingSettings getLightingPreset(const std::string& presetId) const;
    LightingSettings getCurrentLighting() const;
    bool setActiveLightingPreset(const std::string& presetId);
    
    // Skybox
    std::string createSkyboxPreset(const SkyboxSettings& settings);
    bool updateSkyboxPreset(const SkyboxSettings& settings);
    bool deleteSkyboxPreset(const std::string& skyboxId);
    SkyboxSettings getSkyboxPreset(const std::string& skyboxId) const;
    SkyboxSettings getCurrentSkybox() const;
    bool setActiveSkyboxPreset(const std::string& skyboxId);
    
    // Ciclo dia/noite
    LightingPreset getCurrentLightingPreset() const;
    float getDayProgress() const;
    float getSunAltitude() const;
    float getMoonAltitude() const;
    bool isNightTime() const;
    
    struct DayNightTransition {
        LightingPreset fromPreset;
        LightingPreset toPreset;
        float startProgress;
        float endProgress;
        float transitionDuration; // in game hours
    };
    
    bool defineDayNightTransition(const DayNightTransition& transition);
    std::vector<DayNightTransition> getAllTransitions() const;
    
    // Eventos celestes
    struct CelestialEvent {
        std::string eventId;
        std::string name;
        std::string description;
        std::string relatedCelestialId;
        std::string eventType; // "ALIGNMENT", "ECLIPSE", "METEOR_SHOWER", "COMET", "CUSTOM"
        float rarity{0.01f}; // 0.0-1.0
        std::chrono::hours duration{2};
        json effectParameters;
        std::vector<std::string> triggeredWorldEvents;
    };
    
    std::string defineCelestialEvent(const CelestialEvent& event);
    bool deleteCelestialEvent(const std::string& eventId);
    std::vector<CelestialEvent> getAllCelestialEvents() const;
    bool willCelestialEventOccur(std::chrono::hours lookahead = 24h) const;
    
    // Efeitos do ciclo
    std::unordered_map<std::string, float> getTimeOfDayEffects() const;
    json visualizeCurrentSkyState() const;
    
    // Callbacks
    void registerLightingChangedCallback(std::function<void(LightingPreset)> callback);
    void registerCelestialEventCallback(std::function<void(const CelestialEvent&)> callback);
    void registerSunriseCallback(std::function<void()> callback);
    void registerSunsetCallback(std::function<void()> callback);
    
private:
    DayNightSystem();
    ~DayNightSystem();
    
    // Atributos privados
    static std::unique_ptr<DayNightSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<ClimateSystem> m_climateSystem;
    
    // Corpos celestes
    std::unordered_map<std::string, CelestialBody> m_celestialBodies;
    std::unordered_map<std::string, size_t> m_celestialIndices; // type -> count (for auto-IDs)
    mutable std::mutex m_celestialMutex;
    
    // Iluminação
    std::unordered_map<std::string, LightingSettings> m_lightingPresets;
    std::unordered_map<LightingPreset, std::string> m_presetsByType;
    std::string m_activeLightingPresetId;
    mutable std::mutex m_lightingMutex;
    
    // Skybox
    std::unordered_map<std::string, SkyboxSettings> m_skyboxPresets;
    std::unordered_map<std::string, std::string> m_skyboxByTimeWeather; // "timeOfDay:weather" -> skyboxId
    std::string m_activeSkyboxId;
    mutable std::mutex m_skyboxMutex;
    
    // Transições
    std::vector<DayNightTransition> m_dayNightTransitions;
    mutable std::mutex m_transitionsMutex;
    
    // Eventos celestes
    std::unordered_map<std::string, CelestialEvent> m_celestialEvents;
    mutable std::mutex m_eventsMutex;
    
    // Callback
    std::vector<std::function<void(LightingPreset)>> m_lightingChangedCallbacks;
    std::vector<std::function<void(const CelestialEvent&)>> m_celestialEventCallbacks;
    std::vector<std::function<void()>> m_sunriseCallbacks;
    std::vector<std::function<void()>> m_sunsetCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estado
    TimeOfDay m_lastTimeOfDay;
    LightingPreset m_currentPreset{LightingPreset::NOON};
    LightingPreset m_lastPreset{LightingPreset::NOON};
    float m_sunAltitude{90.0f};
    float m_moonAltitude{-90.0f};
    float m_currentTransitionProgress{0.0f};
    
    // Configuração
    bool m_useDynamicLighting{true};
    bool m_weatherAffectsLighting{true};
    bool m_enableCelestialEvents{true};
    float m_celestialEventProbability{0.05f};
    
    // Métodos privados
    std::string generateCelestialId();
    std::string generatePresetId();
    std::string generateSkyboxId();
    std::string generateEventId();
    
    void updateDayNightState();
    void updateCelestialPositions();
    void updateLightingBasedOnTime();
    void checkForCelestialEvents();
    
    LightingSettings interpolateSettings(const LightingSettings& from, const LightingSettings& to, float t) const;
    LightingPreset determinePresetFromTimeOfDay(TimeOfDay timeOfDay) const;
    LightingSettings adjustLightingForWeather(const LightingSettings& settings, WeatherType weather, float intensity) const;
    
    std::string getSkyboxKeyForConditions(TimeOfDay timeOfDay, WeatherType weather) const;
    SkyboxSettings selectSkyboxForConditions(TimeOfDay timeOfDay, WeatherType weather) const;
    
    DayNightTransition findCurrentTransition(float dayProgress) const;
    float calculateTransitionProgress(float dayProgress, const DayNightTransition& transition) const;
    
    bool canCelestialEventOccur(const CelestialEvent& event) const;
    
    void notifyLightingChanged(LightingPreset preset);
    void notifyCelestialEvent(const CelestialEvent& event);
    void notifySunrise();
    void notifySunset();
};

// Implementação inline básica do singleton
inline DayNightSystem& DayNightSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<DayNightSystem>(new DayNightSystem());
    });
    return *s_instance;
}

} // namespace Environment
} // namespace WYDBR

// SeasonalSystem.h - Mudanças Sazonais
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <optional>
#include <nlohmann/json.hpp>
#include "ClimateSystem.h"

namespace WYDBR {
namespace Environment {

using json = nlohmann::json;

struct SeasonalTransformation {
    std::string transformId;
    std::string regionId;
    Season season;
    
    struct VisualChange {
        std::string entityType; // "TERRAIN", "VEGETATION", "WATER", "STRUCTURE", "ATMOSPHERE"
        std::string baseAssetId;
        std::string seasonalAssetId;
        float transitionDuration{3.0f}; // days
        bool affectsCollision{false};
        json visualParameters;
    };
    
    std::vector<VisualChange> visualChanges;
    
    struct ResourceChange {
        std::string resourceId;
        float availabilityMultiplier{1.0f};
        float qualityMultiplier{1.0f};
        bool disabled{false};
        std::vector<std::string> replacementResourceIds;
        json resourceParameters;
    };
    
    std::vector<ResourceChange> resourceChanges;
    
    struct NPCBehaviorChange {
        std::string npcType;
        std::string behaviorChange; // "MIGRATION", "HIBERNATE", "AGGRESSIVE", "PASSIVE", "BREEDING", "CUSTOM"
        std::optional<std::string> destinationRegion;
        float spawnRateMultiplier{1.0f};
        float aggressionMultiplier{1.0f};
        json behaviorParameters;
    };
    
    std::vector<NPCBehaviorChange> npcChanges;
    
    struct AccessibilityChange {
        std::string areaId;
        bool accessible{true};
        std::string blockingReason; // "SNOW", "FLOOD", "ICE", "GROWTH", "CUSTOM"
        std::optional<std::string> alternatePathId;
        float movementPenalty{0.0f};
        json accessParameters;
    };
    
    std::vector<AccessibilityChange> accessibilityChanges;
    
    struct GameplayModifier {
        std::string affectedStat;
        float value;
        bool isPercentage{true};
        std::string applicationType; // "GLOBAL", "REGION", "PLAYER_TYPE", "ENTITY_TYPE"
        std::vector<std::string> targetIds;
        json modifierParameters;
    };
    
    std::vector<GameplayModifier> gameplayModifiers;
    
    std::set<std::string> enabledEvents;
    std::set<std::string> disabledEvents;
    json seasonalParameters;
};

struct SeasonalEvent {
    std::string eventId;
    std::string name;
    std::string description;
    Season season;
    std::optional<std::string> regionId; // null = global
    
    struct EventSchedule {
        uint32_t startDay{1};
        uint32_t endDay{30};
        bool repeatsYearly{true};
        uint32_t minimumYear{1};
        uint32_t dayOffset{0}; // Random offset +/- this many days
    };
    
    EventSchedule schedule;
    
    struct EventContent {
        std::vector<std::string> npcsToSpawn;
        std::vector<std::string> itemsToAdd;
        std::vector<std::string> questsToActivate;
        std::vector<std::string> decorationsToPlace;
        std::vector<std::string> worldEventsToTrigger;
        json customContent;
    };
    
    EventContent content;
    
    std::string announcement;
    float importance{1.0f}; // 0-10
    bool enableRewards{true};
    bool enabled{true};
    json eventParameters;
};

struct BiomeSeasonalProfile {
    std::string profileId;
    std::string biomeName;
    
    struct SeasonDescriptor {
        Season season;
        std::string displayName;
        std::array<int, 3> dominantColors;
        std::vector<std::string> characteristicResourceIds;
        std::vector<std::string> characteristicCreatureIds;
        std::vector<std::string> seasonalEffectIds;
        float temperatureRange[2]; // min, max
        float precipitationRange[2]; // min, max
        json environmentalProperties;
    };
    
    std::vector<SeasonDescriptor> seasonDescriptors;
    
    std::unordered_map<Season, std::unordered_map<Season, float>> transitionMatrix; // from -> to -> transition duration (days)
    std::vector<std::string> yearRoundResourceIds;
    std::vector<std::string> yearRoundCreatureIds;
    json biomeParameters;
};

class SeasonalSystem {
public:
    // Singleton
    static SeasonalSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Transformações sazonais
    std::string createSeasonalTransformation(const SeasonalTransformation& transformation);
    bool updateSeasonalTransformation(const SeasonalTransformation& transformation);
    bool deleteSeasonalTransformation(const std::string& transformId);
    SeasonalTransformation getSeasonalTransformation(const std::string& transformId) const;
    std::vector<SeasonalTransformation> getTransformationsForRegion(const std::string& regionId) const;
    SeasonalTransformation getCurrentTransformation(const std::string& regionId) const;
    
    // Eventos sazonais
    std::string createSeasonalEvent(const SeasonalEvent& event);
    bool updateSeasonalEvent(const SeasonalEvent& event);
    bool deleteSeasonalEvent(const std::string& eventId);
    SeasonalEvent getSeasonalEvent(const std::string& eventId) const;
    std::vector<SeasonalEvent> getAllSeasonalEvents() const;
    std::vector<SeasonalEvent> getCurrentSeasonalEvents() const;
    std::vector<SeasonalEvent> getUpcomingEvents(std::chrono::days lookahead = 30d) const;
    
    // Perfis de bioma sazonal
    std::string createBiomeProfile(const BiomeSeasonalProfile& profile);
    bool updateBiomeProfile(const BiomeSeasonalProfile& profile);
    bool deleteBiomeProfile(const std::string& profileId);
    BiomeSeasonalProfile getBiomeProfile(const std::string& profileId) const;
    BiomeSeasonalProfile getProfileForBiome(const std::string& biomeName) const;
    
    // Controle e consulta
    bool applySeasonalTransformation(const std::string& regionId);
    bool startSeasonalEvent(const std::string& eventId);
    bool endSeasonalEvent(const std::string& eventId);
    float getSeasonTransitionProgress(const std::string& regionId) const;
    std::vector<std::string> getActiveSeasonalEvents() const;
    std::unordered_map<std::string, Season> getAllRegionSeasons() const;
    
    // Efeitos e modificadores
    json getCurrentSeasonalEffects(const std::string& regionId) const;
    std::vector<std::string> getSeasonalResources(const std::string& regionId) const;
    std::vector<std::string> getSeasonalCreatures(const std::string& regionId) const;
    std::unordered_map<std::string, float> getSeasonalModifiers(const std::string& regionId) const;
    
    // Previsão e análise
    json generateSeasonalForecast() const;
    json analyzeSeasonalPatterns(const std::string& regionId) const;
    
    // Callbacks
    void registerSeasonTransitionCallback(std::function<void(const std::string&, Season, Season, float)> callback);
    void registerSeasonalEventStartedCallback(std::function<void(const std::string&)> callback);
    void registerSeasonalEventEndedCallback(std::function<void(const std::string&)> callback);
    
private:
    SeasonalSystem();
    ~SeasonalSystem();
    
    // Atributos privados
    static std::unique_ptr<SeasonalSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<ClimateSystem> m_climateSystem;
    
    // Transformações sazonais
    std::unordered_map<std::string, SeasonalTransformation> m_seasonalTransformations;
    std::unordered_map<std::string, std::unordered_map<Season, std::string>> m_transformationsByRegionSeason; // regionId -> (season -> transformId)
    mutable std::mutex m_transformationsMutex;
    
    // Eventos sazonais
    std::unordered_map<std::string, SeasonalEvent> m_seasonalEvents;
    std::unordered_map<Season, std::vector<std::string>> m_eventsBySeason;
    std::unordered_map<std::string, std::vector<std::string>> m_eventsByRegion; // regionId -> [eventIds]
    std::set<std::string> m_activeEventIds;
    mutable std::mutex m_eventsMutex;
    
    // Perfis de bioma
    std::unordered_map<std::string, BiomeSeasonalProfile> m_biomeProfiles;
    std::unordered_map<std::string, std::string> m_profilesByBiome; // biomeName -> profileId
    mutable std::mutex m_profilesMutex;
    
    // Transições
    struct SeasonTransition {
        std::string regionId;
        Season fromSeason;
        Season toSeason;
        std::chrono::system_clock::time_point startTime;
        std::chrono::days duration;
        float progress{0.0f};
    };
    
    std::unordered_map<std::string, SeasonTransition> m_activeTransitions; // regionId -> transition
    mutable std::mutex m_transitionsMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const std::string&, Season, Season, float)>> m_transitionCallbacks;
    std::vector<std::function<void(const std::string&)>> m_eventStartCallbacks;
    std::vector<std::function<void(const std::string&)>> m_eventEndCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Configuração
    std::chrono::days m_defaultTransitionDuration{7};
    std::chrono::seconds m_transitionUpdateInterval{60};
    std::chrono::hours m_eventCheckInterval{1};
    float m_seasonalResourceMultiplierBase{1.5f};
    float m_offSeasonResourceMultiplier{0.5f};
    bool m_enableSeasonalEvents{true};
    bool m_enableVisualChanges{true};
    bool m_enableGameplayChanges{true};
    
    // Métodos privados
    std::string generateTransformationId();
    std::string generateEventId();
    std::string generateProfileId();
    
    void updateSeasonTransitions();
    void checkSeasonalEvents();
    
    bool applyVisualChanges(const SeasonalTransformation& transformation, float transitionProgress);
    bool applyResourceChanges(const SeasonalTransformation& transformation);
    bool applyNPCChanges(const SeasonalTransformation& transformation);
    bool applyAccessibilityChanges(const SeasonalTransformation& transformation);
    bool applyGameplayModifiers(const SeasonalTransformation& transformation);
    
    bool isEventInSeason(const SeasonalEvent& event) const;
    bool isEventActive(const SeasonalEvent& event) const;
    bool shouldEventStart(const SeasonalEvent& event) const;
    
    std::chrono::days calculateTransitionDuration(const std::string& regionId, Season fromSeason, Season toSeason) const;
    BiomeSeasonalProfile::SeasonDescriptor getSeasonDescriptor(const std::string& regionId, Season season) const;
    
    void notifySeasonTransition(const std::string& regionId, Season fromSeason, Season toSeason, float progress);
    void notifySeasonalEventStarted(const std::string& eventId);
    void notifySeasonalEventEnded(const std::string& eventId);
    
    std::string getBiomeForRegion(const std::string& regionId) const;
    json visualizeSeasonalTransition(const std::string& regionId, float progress) const;
};

// Implementação inline básica do singleton
inline SeasonalSystem& SeasonalSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<SeasonalSystem>(new SeasonalSystem());
    });
    return *s_instance;
}

} // namespace Environment
} // namespace WYDBR

// WeatherEffectsSystem.h - Efeitos Visuais Climáticos
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include "ClimateSystem.h"

namespace WYDBR {
namespace Environment {

using json = nlohmann::json;

struct ParticleEffect {
    std::string effectId;
    std::string name;
    WeatherType weatherType;
    
    struct ParticleParameters {
        std::string particleTextureId;
        uint32_t maxParticles{1000};
        float emissionRate{100.0f}; // particles per second
        float particleLifetime{5.0f}; // seconds
        float startSize{0.1f};
        float endSize{0.1f};
        std::array<float, 3> startColor;
        std::array<float, 3> endColor;
        float startAlpha{1.0f};
        float endAlpha{0.0f};
        bool useGravity{true};
        float gravityMultiplier{1.0f};
        std::array<float, 3> initialVelocity;
        std::array<float, 3> velocityVariance;
        json renderProperties;
    };
    
    ParticleParameters particleParams;
    
    struct EmissionArea {
        std::string areaType; // "BOX", "SPHERE", "CYLINDER", "GLOBAL"
        float width{100.0f};
        float height{50.0f};
        float depth{100.0f};
        float radius{50.0f};
        float heightOffset{10.0f}; // Above player/camera
        bool followPlayer{true};
        bool emitFromSurface{false};
        json areaParameters;
    };
    
    EmissionArea emissionArea;
    
    float intensityMultiplier{1.0f};
    bool enableCollision{true};
    float windInfluence{1.0f};
    std::array<float, 3> worldSpaceForce;
    json effectParameters;
};

struct WeatherSoundscape {
    std::string soundscapeId;
    std::string name;
    WeatherType weatherType;
    std::vector<std::string> ambientLoopIds;
    
    struct SoundLayer {
        std::string soundId;
        float volume{1.0f};
        float minInterval{5.0f}; // seconds
        float maxInterval{30.0f}; // seconds
        float minDistance{1.0f};
        float maxDistance{100.0f};
        float spatialBlend{1.0f}; // 0 = 2D, 1 = 3D
        bool useReverb{true};
        bool useOcclusion{true};
        json soundProperties;
    };
    
    std::vector<SoundLayer> randomSounds;
    
    struct ConditionalSound {
        std::string soundId;
        std::string condition; // "NEAR_WATER", "UNDER_COVER", "OPEN_SPACE", "INDOOR", "FOREST", etc.
        float volume{1.0f};
        float probability{0.5f};
        json conditionParameters;
    };
    
    std::vector<ConditionalSound> conditionalSounds;
    
    float intensityMultiplier{1.0f};
    float crossfadeDuration{2.0f}; // seconds
    float reverbAmount{0.5f};
    json soundscapeParameters;
};

struct VisualPostProcess {
    std::string postProcessId;
    std::string name;
    WeatherType weatherType;
    
    struct ColorGrading {
        std::array<float, 3> temperature;
        std::array<float, 3> tint;
        float contrast{1.0f};
        float saturation{1.0f};
        float brightness{1.0f};
        std::array<float, 3> colorFilter;
        json lookupTexture;
    };
    
    ColorGrading colorGrading;
    
    struct ScreenEffects {
        float vignetteIntensity{0.0f};
        float grainIntensity{0.0f};
        float chromaticAberration{0.0f};
        float motionBlurAmount{0.0f};
        float depthOfFieldDistance{100.0f};
        float depthOfFieldFalloff{0.5f};
        bool useBloom{false};
        float bloomIntensity{0.5f};
        json screenEffectParameters;
    };
    
    ScreenEffects screenEffects;
    
    struct EnvironmentalEffects {
        bool useWetness{false};
        float surfaceWetness{0.0f};
        bool usePuddles{false};
        float puddleAmount{0.0f};
        bool useSnowCoverage{false};
        float snowAmount{0.0f};
        bool useIce{false};
        float iceAmount{0.0f};
        json environmentalParameters;
    };
    
    EnvironmentalEffects environmentalEffects;
    
    float intensityMultiplier{1.0f};
    float transitionDuration{2.0f}; // seconds
    bool adjustForTimeOfDay{true};
    json postProcessParameters;
};

class WeatherEffectsSystem {
public:
    // Singleton
    static WeatherEffectsSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Efeitos de partículas
    std::string createParticleEffect(const ParticleEffect& effect);
    bool updateParticleEffect(const ParticleEffect& effect);
    bool deleteParticleEffect(const std::string& effectId);
    ParticleEffect getParticleEffect(const std::string& effectId) const;
    std::vector<ParticleEffect> getAllParticleEffects() const;
    std::vector<ParticleEffect> getParticleEffectsForWeather(WeatherType weatherType) const;
    
    // Soundscapes
    std::string createWeatherSoundscape(const WeatherSoundscape& soundscape);
    bool updateWeatherSoundscape(const WeatherSoundscape& soundscape);
    bool deleteWeatherSoundscape(const std::string& soundscapeId);
    WeatherSoundscape getWeatherSoundscape(const std::string& soundscapeId) const;
    std::vector<WeatherSoundscape> getAllWeatherSoundscapes() const;
    WeatherSoundscape getSoundscapeForWeather(WeatherType weatherType) const;
    
    // Post-processamento visual
    std::string createVisualPostProcess(const VisualPostProcess& postProcess);
    bool updateVisualPostProcess(const VisualPostProcess& postProcess);
    bool deleteVisualPostProcess(const std::string& postProcessId);
    VisualPostProcess getVisualPostProcess(const std::string& postProcessId) const;
    std::vector<VisualPostProcess> getAllVisualPostProcesses() const;
    VisualPostProcess getPostProcessForWeather(WeatherType weatherType) const;
    
    // Controle de efeitos
    json getActiveWeatherEffects(const std::string& regionId) const;
    json getWeatherEffectsAtLocation(float x, float y, float z) const;
    bool setParticleEffectIntensity(const std::string& effectId, float intensity);
    bool setSoundscapeVolume(const std::string& soundscapeId, float volume);
    bool setPostProcessIntensity(const std::string& postProcessId, float intensity);
    
    // Efeitos combinados
    struct WeatherEffectPreset {
        std::string presetId;
        std::string name;
        WeatherType weatherType;
        float intensity{1.0f};
        std::vector<std::string> particleEffectIds;
        std::string soundscapeId;
        std::string postProcessId;
        json additionalEffects;
    };
    
    std::string createWeatherEffectPreset(const WeatherEffectPreset& preset);
    bool updateWeatherEffectPreset(const WeatherEffectPreset& preset);
    bool deleteWeatherEffectPreset(const std::string& presetId);
    WeatherEffectPreset getWeatherEffectPreset(const std::string& presetId) const;
    std::vector<WeatherEffectPreset> getAllWeatherEffectPresets() const;
    
    // Aplicação de efeitos
    bool applyWeatherEffectPreset(const std::string& presetId, 
                               const std::string& regionId,
                               float intensity = 1.0f);
    
    bool transitionToWeatherEffects(const std::string& regionId, 
                                 WeatherType targetWeather,
                                 float targetIntensity = 1.0f,
                                 float transitionDuration = 2.0f);
    
    // Visualização e depuração
    json debugWeatherEffects() const;
    json getCurrentWeatherVisuals() const;
    
private:
    WeatherEffectsSystem();
    ~WeatherEffectsSystem();
    
    // Atributos privados
    static std::unique_ptr<WeatherEffectsSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<ClimateSystem> m_climateSystem;
    
    // Efeitos de partículas
    std::unordered_map<std::string, ParticleEffect> m_particleEffects;
    std::unordered_map<WeatherType, std::vector<std::string>> m_particleEffectsByWeather;
    mutable std::mutex m_particleMutex;
    
    // Soundscapes
    std::unordered_map<std::string, WeatherSoundscape> m_weatherSoundscapes;
    std::unordered_map<WeatherType, std::string> m_soundscapesByWeather;
    mutable std::mutex m_soundscapeMutex;
    
    // Post-processamento
    std::unordered_map<std::string, VisualPostProcess> m_visualPostProcesses;
    std::unordered_map<WeatherType, std::string> m_postProcessesByWeather;
    mutable std::mutex m_postProcessMutex;
    
    // Presets
    std::unordered_map<std::string, WeatherEffectPreset> m_weatherEffectPresets;
    std::unordered_map<WeatherType, std::string> m_presetsByWeather;
    mutable std::mutex m_presetsMutex;
    
    // Estado ativo
    struct ActiveEffects {
        std::string regionId;
        std::string activePresetId;
        float currentIntensity{1.0f};
        std::vector<std::string> activeParticleEffectIds;
        std::string activeSoundscapeId;
        std::string activePostProcessId;
        WeatherType currentWeather;
    };
    
    std::unordered_map<std::string, ActiveEffects> m_activeEffectsByRegion;
    mutable std::mutex m_activeEffectsMutex;
    
    // Configuração
    bool m_adjustEffectsForTimeOfDay{true};
    bool m_adjustEffectsForSeason{true};
    bool m_enableParticleCollision{true};
    bool m_enableWindInfluence{true};
    float m_globalEffectIntensityMultiplier{1.0f};
    
    // Métodos privados
    std::string generateEffectId();
    std::string generateSoundscapeId();
    std::string generatePostProcessId();
    std::string generatePresetId();
    
    ParticleEffect adjustParticleEffectForIntensity(const ParticleEffect& effect, float intensity) const;
    WeatherSoundscape adjustSoundscapeForIntensity(const WeatherSoundscape& soundscape, float intensity) const;
    VisualPostProcess adjustPostProcessForIntensity(const VisualPostProcess& postProcess, float intensity) const;
    
    ParticleEffect adjustParticleEffectForTimeOfDay(const ParticleEffect& effect, TimeOfDay timeOfDay) const;
    WeatherSoundscape adjustSoundscapeForTimeOfDay(const WeatherSoundscape& soundscape, TimeOfDay timeOfDay) const;
    VisualPostProcess adjustPostProcessForTimeOfDay(const VisualPostProcess& postProcess, TimeOfDay timeOfDay) const;
    
    bool applyParticleEffect(const std::string& effectId, const std::string& regionId, float intensity);
    bool applySoundscape(const std::string& soundscapeId, const std::string& regionId, float intensity);
    bool applyPostProcess(const std::string& postProcessId, const std::string& regionId, float intensity);
    
    WeatherEffectPreset findPresetForWeather(WeatherType weatherType) const;
    void updateActiveEffects();
    
    json createWeatherEffectDescriptor(const ActiveEffects& effects) const;
    json createWeatherParticleDescriptor(const ParticleEffect& effect, float intensity) const;
    json createWeatherSoundDescriptor(const WeatherSoundscape& soundscape, float intensity) const;
    json createWeatherVisualDescriptor(const VisualPostProcess& postProcess, float intensity) const;
};

// Implementação inline básica do singleton
inline WeatherEffectsSystem& WeatherEffectsSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<WeatherEffectsSystem>(new WeatherEffectsSystem());
    });
    return *s_instance;
}

} // namespace Environment
} // namespace WYDBR
`;

// Fornece uma API JavaScript para integração no projeto
module.exports.setupSystem = function(engineContext) {
  console.log('Inicializando sistema: sistema_clima_ambiente');
  
  // Inicializa o sistema no contexto do engine
  engineContext.registerSystem({
    name: 'sistema_clima_ambiente',
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
