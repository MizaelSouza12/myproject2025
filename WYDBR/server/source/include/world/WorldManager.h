/**
 * WorldManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/world/WorldManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _WORLD_MANAGER_H_
#define _WORLD_MANAGER_H_

/**
 * @file WorldManager.h
 * @brief Gerenciador de mundo do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de mundo do WYDBR,
 * responsável por coordenar todos os subsistemas do mundo do jogo,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <chrono>

#include "../GlobalDef.h"
#include "../Basedef.h"
#include "MapManager.h"
#include "MobManager.h"

namespace wydbr {
namespace world {

/**
 * @brief Estado do mundo
 */
enum /**
 * Classe WYDWorldState
 * 
 * Esta classe implementa a funcionalidade WYDWorldState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WorldState {
    NONE = 0,               // Nenhum
    STARTING = 1,           // Iniciando
    RUNNING = 2,            // Executando
    SHUTTING_DOWN = 3,      // Desligando
    MAINTENANCE = 4,        // Manutenção
    CRASHED = 5,            // Crashado
    CUSTOM_1 = 6,           // Personalizado 1
    CUSTOM_2 = 7,           // Personalizado 2
    CUSTOM_3 = 8,           // Personalizado 3
    UNKNOWN = 9             // Desconhecido
};

/**
 * @brief Tipo de evento do mundo
 */
enum /**
 * Classe WYDWorldEventType
 * 
 * Esta classe implementa a funcionalidade WYDWorldEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WorldEventType {
    NONE = 0,               // Nenhum
    SPAWN = 1,              // Spawn
    BOSS = 2,               // Chefe
    PVP = 3,                // PVP
    ARENA = 4,              // Arena
    SIEGE = 5,              // Cerco
    INVASION = 6,           // Invasão
    WEATHER = 7,            // Clima
    TIME = 8,               // Tempo
    QUEST = 9,              // Missão
    FESTIVAL = 10,          // Festival
    TREASURE = 11,          // Tesouro
    COMPETITION = 12,       // Competição
    CUSTOM_1 = 13,          // Personalizado 1
    CUSTOM_2 = 14,          // Personalizado 2
    CUSTOM_3 = 15,          // Personalizado 3
    UNKNOWN = 16            // Desconhecido
};

/**
 * @brief Estado do evento do mundo
 */
enum /**
 * Classe WYDWorldEventState
 * 
 * Esta classe implementa a funcionalidade WYDWorldEventState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WorldEventState {
    NONE = 0,               // Nenhum
    SCHEDULED = 1,          // Agendado
    PREPARING = 2,          // Preparando
    RUNNING = 3,            // Executando
    FINISHING = 4,          // Finalizando
    COMPLETED = 5,          // Concluído
    FAILED = 6,             // Falhou
    CANCELED = 7,           // Cancelado
    CUSTOM_1 = 8,           // Personalizado 1
    CUSTOM_2 = 9,           // Personalizado 2
    CUSTOM_3 = 10,          // Personalizado 3
    UNKNOWN = 11            // Desconhecido
};

/**
 * @brief Tipo de condição de clima
 */
enum /**
 * Classe WYDWeatherCondition
 * 
 * Esta classe implementa a funcionalidade WYDWeatherCondition conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WeatherCondition {
    NONE = 0,               // Nenhum
    CLEAR = 1,              // Limpo
    CLOUDY = 2,             // Nublado
    RAIN = 3,               // Chuva
    STORM = 4,              // Tempestade
    SNOW = 5,               // Neve
    FOG = 6,                // Névoa
    SANDSTORM = 7,          // Tempestade de areia
    CUSTOM_1 = 8,           // Personalizado 1
    CUSTOM_2 = 9,           // Personalizado 2
    CUSTOM_3 = 10,          // Personalizado 3
    UNKNOWN = 11            // Desconhecido
};

/**
 * @brief Resultado da operação de mundo
 */
enum /**
 * Classe WYDWorldResult
 * 
 * Esta classe implementa a funcionalidade WYDWorldResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WorldResult {
    SUCCESS = 0,                    // Sucesso
    FAILED = 1,                     // Falha
    EVENT_NOT_FOUND = 2,            // Evento não encontrado
    EVENT_ALREADY_EXISTS = 3,       // Evento já existe
    EVENT_IN_PROGRESS = 4,          // Evento em andamento
    EVENT_NOT_RUNNING = 5,          // Evento não em execução
    INVALID_STATE = 6,              // Estado inválido
    INVALID_TIME = 7,               // Tempo inválido
    INVALID_MAP = 8,                // Mapa inválido
    INVALID_PARAMETER = 9,          // Parâmetro inválido
    INSUFFICIENT_RESOURCES = 10,    // Recursos insuficientes
    WORLD_SHUTTING_DOWN = 11,       // Mundo desligando
    WORLD_MAINTENANCE = 12,         // Mundo em manutenção
    PERMISSION_DENIED = 13,         // Permissão negada
    CUSTOM_1 = 14,                  // Personalizado 1
    CUSTOM_2 = 15,                  // Personalizado 2
    CUSTOM_3 = 16,                  // Personalizado 3
    UNKNOWN = 17                    // Desconhecido
};

/**
 * @brief Tipo de período do dia
 */
enum /**
 * Classe WYDDayPeriod
 * 
 * Esta classe implementa a funcionalidade WYDDayPeriod conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DayPeriod {
    NONE = 0,               // Nenhum
    DAWN = 1,               // Amanhecer
    MORNING = 2,            // Manhã
    NOON = 3,               // Meio-dia
    AFTERNOON = 4,          // Tarde
    EVENING = 5,            // Entardecer
    NIGHT = 6,              // Noite
    MIDNIGHT = 7,           // Meia-noite
    CUSTOM_1 = 8,           // Personalizado 1
    CUSTOM_2 = 9,           // Personalizado 2
    CUSTOM_3 = 10,          // Personalizado 3
    UNKNOWN = 11            // Desconhecido
};

/**
 * @brief Evento do mundo
 */
struct WorldEvent {
    uint32_t EventID;               // ID do evento
    std::string Name;               // Nome
    WorldEventType Type;            // Tipo
    WorldEventState State;          // Estado
    uint32_t StartTime;             // Tempo de início
    uint32_t EndTime;               // Tempo de fim
    uint32_t Duration;              // Duração
    std::vector<uint16_t> MapIDs;   // IDs dos mapas
    std::string Script;             // Script
    std::string Description;        // Descrição
    std::map<std::string, std::string> Parameters; // Parâmetros
    uint32_t ScheduleID;            // ID do agendamento
    
    WorldEvent() : EventID(0), Type(WorldEventType::NONE),
        State(WorldEventState::NONE), StartTime(0), EndTime(0),
        Duration(0), ScheduleID(0) {}
};

/**
 * @brief Agendamento de evento
 */
struct EventSchedule {
    uint32_t ScheduleID;            // ID do agendamento
    uint32_t EventID;               // ID do evento
    uint32_t NextRunTime;           // Próxima execução
    std::string CronExpression;     // Expressão cron
    bool Enabled;                   // Habilitado
    uint32_t RepeatCount;           // Contagem de repetições
    uint32_t CurrentCount;          // Contagem atual
    
    EventSchedule() : ScheduleID(0), EventID(0), NextRunTime(0),
        Enabled(true), RepeatCount(0), CurrentCount(0) {}
};

/**
 * @brief Clima do mundo
 */
struct WorldWeather {
    uint16_t MapID;                 // ID do mapa
    WeatherCondition Condition;     // Condição
    uint8_t Intensity;              // Intensidade
    uint32_t StartTime;             // Tempo de início
    uint32_t EndTime;               // Tempo de fim
    uint32_t TransitionTime;        // Tempo de transição
    
    WorldWeather() : MapID(0), Condition(WeatherCondition::NONE),
        Intensity(0), StartTime(0), EndTime(0), TransitionTime(0) {}
};

/**
 * @brief Tempo do mundo
 */
struct WorldTime {
    uint32_t CurrentTime;           // Tempo atual
    float TimeScale;                // Escala de tempo
    uint32_t DayLengthMinutes;      // Comprimento do dia em minutos
    uint32_t DawnStartMinute;       // Início do amanhecer em minutos
    uint32_t MorningStartMinute;    // Início da manhã em minutos
    uint32_t NoonStartMinute;       // Início do meio-dia em minutos
    uint32_t AfternoonStartMinute;  // Início da tarde em minutos
    uint32_t EveningStartMinute;    // Início do entardecer em minutos
    uint32_t NightStartMinute;      // Início da noite em minutos
    uint32_t MidnightStartMinute;   // Início da meia-noite em minutos
    
    WorldTime() : CurrentTime(0), TimeScale(1.0f), DayLengthMinutes(60),
        DawnStartMinute(0), MorningStartMinute(10), NoonStartMinute(20),
        AfternoonStartMinute(30), EveningStartMinute(40),
        NightStartMinute(50), MidnightStartMinute(55) {}
};

/**
 * @brief Configuração do mundo
 */
struct WorldConfig {
    std::string WorldName;          // Nome do mundo
    uint32_t TickRate;              // Taxa de tick
    uint32_t SaveInterval;          // Intervalo de salvamento
    bool EnableWeatherSystem;       // Habilitar sistema de clima
    bool EnableTimeSystem;          // Habilitar sistema de tempo
    bool EnableEventSystem;         // Habilitar sistema de eventos
    bool EnableDynamicEvents;       // Habilitar eventos dinâmicos
    uint32_t EventTickRate;         // Taxa de tick de eventos
    uint32_t MaxConcurrentEvents;   // Máximo de eventos simultâneos
    uint32_t MinEventSpacing;       // Espaçamento mínimo de eventos
    std::string EventScriptsPath;   // Caminho dos scripts de eventos
    
    WorldConfig() : TickRate(1000), SaveInterval(300),
        EnableWeatherSystem(true), EnableTimeSystem(true),
        EnableEventSystem(true), EnableDynamicEvents(true),
        EventTickRate(5000), MaxConcurrentEvents(5),
        MinEventSpacing(600) {
        WorldName = "WYDBR";
        EventScriptsPath = "data/scripts/events/";
    }
};

/**
 * @brief Estatísticas do mundo
 */
struct WorldStats {
    uint32_t TotalPlayers;          // Total de jogadores
    uint32_t OnlinePlayers;         // Jogadores online
    uint32_t TotalMobs;             // Total de mobs
    uint32_t ActiveEvents;          // Eventos ativos
    uint32_t TotalEventsRun;        // Total de eventos executados
    uint32_t Uptime;                // Tempo de atividade
    uint32_t MemoryUsage;           // Uso de memória
    uint32_t CPUUsage;              // Uso de CPU
    
    WorldStats() : TotalPlayers(0), OnlinePlayers(0), TotalMobs(0),
        ActiveEvents(0), TotalEventsRun(0), Uptime(0),
        MemoryUsage(0), CPUUsage(0) {}
};

/**
 * @brief Callback de evento do mundo
 */
using WorldEventCallback = std::function<void(const WorldEvent&, WorldEventState)>;

/**
 * @brief Callback de clima do mundo
 */
using WorldWeatherCallback = std::function<void(const WorldWeather&)>;

/**
 * @brief Callback de tempo do mundo
 */
using WorldTimeCallback = std::function<void(const WorldTime&, DayPeriod)>;

/**
 * @brief Gerenciador do mundo
 * 
 * Esta classe é responsável por gerenciar o mundo do jogo.
 */
/**
 * Classe WYDWorldManager
 * 
 * Esta classe implementa a funcionalidade WYDWorldManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WorldManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static WorldManager& getInstance() {
        static WorldManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @param mapManager Gerenciador de mapas
     * @param mobManager Gerenciador de mobs
     * @return true se inicializado com sucesso
     */
    bool initialize(MapManager* mapManager, MobManager* mobManager);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração do mundo
     * @return true se carregado com sucesso
     */
    bool loadWorldConfig();
    
    /**
     * @brief Carrega os eventos do mundo
     * @return true se carregado com sucesso
     */
    bool loadWorldEvents();
    
    /**
     * @brief Carrega os agendamentos de eventos
     * @return true se carregado com sucesso
     */
    bool loadEventSchedules();
    
    /**
     * @brief Inicia o mundo
     * @return true se iniciado com sucesso
     */
    bool startWorld();
    
    /**
     * @brief Para o mundo
     * @param reason Motivo
     * @return true se parado com sucesso
     */
    bool stopWorld(const std::string& reason);
    
    /**
     * @brief Define o estado do mundo
     * @param state Estado
     * @return true se definido com sucesso
     */
    bool setWorldState(WorldState state);
    
    /**
     * @brief Obtém o estado do mundo
     * @return Estado
     */
    WorldState getWorldState() const;
    
    /**
     * @brief Verifica se o mundo está em execução
     * @return true se estiver em execução
     */
    bool isWorldRunning() const;
    
    /**
     * @brief Cria um evento
     * @param name Nome
     * @param type Tipo
     * @param duration Duração
     * @param mapIDs IDs dos mapas
     * @param script Script
     * @param description Descrição
     * @param parameters Parâmetros
     * @return ID do evento ou 0 em caso de erro
     */
    uint32_t createEvent(const std::string& name, WorldEventType type, uint32_t duration, const std::vector<uint16_t>& mapIDs, const std::string& script, const std::string& description, const std::map<std::string, std::string>& parameters);
    
    /**
     * @brief Remove um evento
     * @param eventID ID do evento
     * @return Resultado da operação
     */
    WorldResult removeEvent(uint32_t eventID);
    
    /**
     * @brief Inicia um evento
     * @param eventID ID do evento
     * @return Resultado da operação
     */
    WorldResult startEvent(uint32_t eventID);
    
    /**
     * @brief Para um evento
     * @param eventID ID do evento
     * @return Resultado da operação
     */
    WorldResult stopEvent(uint32_t eventID);
    
    /**
     * @brief Agenda um evento
     * @param eventID ID do evento
     * @param cronExpression Expressão cron
     * @param enabled Habilitado
     * @param repeatCount Contagem de repetições
     * @return ID do agendamento ou 0 em caso de erro
     */
    uint32_t scheduleEvent(uint32_t eventID, const std::string& cronExpression, bool enabled, uint32_t repeatCount);
    
    /**
     * @brief Remove um agendamento
     * @param scheduleID ID do agendamento
     * @return true se removido com sucesso
     */
    bool removeSchedule(uint32_t scheduleID);
    
    /**
     * @brief Habilita um agendamento
     * @param scheduleID ID do agendamento
     * @param enabled Habilitado
     * @return true se habilitado com sucesso
     */
    bool enableSchedule(uint32_t scheduleID, bool enabled);
    
    /**
     * @brief Define o clima
     * @param mapID ID do mapa
     * @param condition Condição
     * @param intensity Intensidade
     * @param duration Duração
     * @param transitionTime Tempo de transição
     * @return true se definido com sucesso
     */
    bool setWeather(uint16_t mapID, WeatherCondition condition, uint8_t intensity, uint32_t duration, uint32_t transitionTime);
    
    /**
     * @brief Define o clima para todos os mapas
     * @param condition Condição
     * @param intensity Intensidade
     * @param duration Duração
     * @param transitionTime Tempo de transição
     * @return true se definido com sucesso
     */
    bool setWeatherForAllMaps(WeatherCondition condition, uint8_t intensity, uint32_t duration, uint32_t transitionTime);
    
    /**
     * @brief Obtém o clima
     * @param mapID ID do mapa
     * @return Clima ou clima com MapID=0 se não encontrado
     */
    WorldWeather getWeather(uint16_t mapID) const;
    
    /**
     * @brief Define a escala de tempo
     * @param timeScale Escala de tempo
     * @return true se definido com sucesso
     */
    bool setTimeScale(float timeScale);
    
    /**
     * @brief Define o tempo
     * @param time Tempo
     * @return true se definido com sucesso
     */
    bool setTime(uint32_t time);
    
    /**
     * @brief Obtém o tempo
     * @return Tempo
     */
    WorldTime getTime() const;
    
    /**
     * @brief Obtém o período do dia
     * @return Período
     */
    DayPeriod getDayPeriod() const;
    
    /**
     * @brief Obtém um evento
     * @param eventID ID do evento
     * @return Evento ou nullptr se não encontrado
     */
    const WorldEvent* getEvent(uint32_t eventID) const;
    
    /**
     * @brief Obtém um agendamento
     * @param scheduleID ID do agendamento
     * @return Agendamento ou nullptr se não encontrado
     */
    const EventSchedule* getSchedule(uint32_t scheduleID) const;
    
    /**
     * @brief Obtém os eventos ativos
     * @return Lista de IDs de evento
     */
    std::vector<uint32_t> getActiveEvents() const;
    
    /**
     * @brief Obtém todos os eventos
     * @return Lista de IDs de evento
     */
    std::vector<uint32_t> getAllEvents() const;
    
    /**
     * @brief Obtém os agendamentos de um evento
     * @param eventID ID do evento
     * @return Lista de IDs de agendamento
     */
    std::vector<uint32_t> getEventSchedules(uint32_t eventID) const;
    
    /**
     * @brief Obtém o tempo de atividade do mundo
     * @return Tempo em segundos
     */
    uint32_t getWorldUptime() const;
    
    /**
     * @brief Obtém a data e hora de início do mundo
     * @return Timestamp
     */
    uint32_t getWorldStartTime() const;
    
    /**
     * @brief Salva o estado do mundo
     * @return true se salvo com sucesso
     */
    bool saveWorldState();
    
    /**
     * @brief Registra um callback de evento
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerEventCallback(WorldEventCallback callback);
    
    /**
     * @brief Remove um callback de evento
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEventCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de clima
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerWeatherCallback(WorldWeatherCallback callback);
    
    /**
     * @brief Remove um callback de clima
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterWeatherCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de tempo
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerTimeCallback(WorldTimeCallback callback);
    
    /**
     * @brief Remove um callback de tempo
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterTimeCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém estatísticas do mundo
     * @return Estatísticas
     */
    WorldStats getWorldStats() const;
    
    /**
     * @brief Obtém a configuração do mundo
     * @return Configuração
     */
    WorldConfig getWorldConfig() const;
    
    /**
     * @brief Define a configuração do mundo
     * @param config Configuração
     */
    void setWorldConfig(const WorldConfig& config);
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
private:
    /**
     * @brief Construtor privado
     */
    WorldManager();
    
    /**
     * @brief Destrutor privado
     */
    ~WorldManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    WorldManager(const WorldManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    WorldManager& operator=(const WorldManager&) = delete;
    
    // Gerenciadores
    MapManager* _mapManager;
    MobManager* _mobManager;
    
    // Estado do mundo
    WorldState _worldState;
    uint32_t _worldStartTime;
    
    // Eventos
    std::unordered_map<uint32_t, WorldEvent> _events;
    std::atomic<uint32_t> _nextEventID;
    
    // Agendamentos
    std::unordered_map<uint32_t, EventSchedule> _schedules;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _eventSchedules;
    std::atomic<uint32_t> _nextScheduleID;
    
    // Clima
    std::unordered_map<uint16_t, WorldWeather> _weather;
    
    // Tempo
    WorldTime _time;
    DayPeriod _dayPeriod;
    std::chrono::system_clock::time_point _lastTimeUpdate;
    
    // Configuração
    WorldConfig _worldConfig;
    
    // Estatísticas
    WorldStats _worldStats;
    
    // Callbacks
    std::vector<WorldEventCallback> _eventCallbacks;
    std::vector<WorldWeatherCallback> _weatherCallbacks;
    std::vector<WorldTimeCallback> _timeCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _stateMutex;
    mutable std::mutex _eventMutex;
    mutable std::mutex _scheduleMutex;
    mutable std::mutex _weatherMutex;
    mutable std::mutex _timeMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Timers
    uint32_t _eventTimer;
    uint32_t _weatherTimer;
    uint32_t _timeTimer;
    uint32_t _saveTimer;
    uint32_t _statsTimer;
    
    /**
     * @brief Checa agendamentos
     * @param currentTime Tempo atual
     */
    void checkSchedules(uint32_t currentTime);
    
    /**
     * @brief Checa eventos
     * @param currentTime Tempo atual
     */
    void checkEvents(uint32_t currentTime);
    
    /**
     * @brief Checa clima
     * @param currentTime Tempo atual
     */
    void checkWeather(uint32_t currentTime);
    
    /**
     * @brief Checa tempo
     * @param currentTime Tempo atual
     */
    void checkTime(uint32_t currentTime);
    
    /**
     * @brief Atualiza estatísticas
     */
    void updateWorldStats();
    
    /**
     * @brief Atualiza o tempo do mundo
     * @param deltaTime Tempo decorrido
     */
    void updateWorldTime(uint32_t deltaTime);
    
    /**
     * @brief Calcula o próximo tempo de execução de uma expressão cron
     * @param cronExpression Expressão cron
     * @param lastRunTime Último tempo de execução
     * @return Próximo tempo de execução
     */
    uint32_t calculateNextCronTime(const std::string& cronExpression, uint32_t lastRunTime);
    
    /**
     * @brief Carrega um evento do arquivo
     * @param eventID ID do evento
     * @param filename Nome do arquivo
     * @return true se carregado com sucesso
     */
    bool loadEventFromFile(uint32_t eventID, const std::string& filename);
    
    /**
     * @brief Carrega um agendamento do arquivo
     * @param scheduleID ID do agendamento
     * @param filename Nome do arquivo
     * @return true se carregado com sucesso
     */
    bool loadScheduleFromFile(uint32_t scheduleID, const std::string& filename);
    
    /**
     * @brief Executa um script de evento
     * @param eventID ID do evento
     * @param action Ação
     * @return true se executado com sucesso
     */
    bool executeEventScript(uint32_t eventID, const std::string& action);
    
    /**
     * @brief Executa callbacks de evento
     * @param event Evento
     * @param oldState Estado antigo
     */
    void executeEventCallbacks(const WorldEvent& event, WorldEventState oldState);
    
    /**
     * @brief Executa callbacks de clima
     * @param weather Clima
     */
    void executeWeatherCallbacks(const WorldWeather& weather);
    
    /**
     * @brief Executa callbacks de tempo
     * @param time Tempo
     * @param oldPeriod Período antigo
     */
    void executeTimeCallbacks(const WorldTime& time, DayPeriod oldPeriod);
    
    /**
     * @brief Atualiza o período do dia
     */
    void updateDayPeriod();
    
    /**
     * @brief Calcula o período do dia
     * @param minuteOfDay Minuto do dia
     * @return Período
     */
    DayPeriod calculateDayPeriod(uint32_t minuteOfDay);
    
    /**
     * @brief Parseia uma expressão cron
     * @param cronExpression Expressão cron
     * @param minute Minuto
     * @param hour Hora
     * @param dayOfMonth Dia do mês
     * @param month Mês
     * @param dayOfWeek Dia da semana
     * @return true se parseado com sucesso
     */
    bool parseCronExpression(const std::string& cronExpression, std::vector<int>& minute, std::vector<int>& hour, std::vector<int>& dayOfMonth, std::vector<int>& month, std::vector<int>& dayOfWeek);
};

} // namespace world
} // namespace wydbr

#endif // _WORLD_MANAGER_H_