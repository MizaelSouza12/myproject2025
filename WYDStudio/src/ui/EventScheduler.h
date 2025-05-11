#ifndef WYDSTUDIO_EVENT_SCHEDULER_H
#define WYDSTUDIO_EVENT_SCHEDULER_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>
#include <chrono>
#include <variant>
#include <nlohmann/json.hpp>
#include "../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {

using json = nlohmann::json;

/**
 * @brief Tipo de evento de jogo
 */
enum class GameEventType {
    INVASION,            // Invasão de monstros
    BOSS_SPAWN,          // Aparecimento de chefe
    DUNGEON_OPENING,     // Abertura de masmorra
    PVP_TOURNAMENT,      // Torneio PvP
    GUILD_WAR,           // Guerra de guildas
    TREASURE_HUNT,       // Caça ao tesouro
    DROP_BOOST,          // Aumento de taxa de drop
    XP_BOOST,            // Aumento de taxa de experiência
    GOLD_BOOST,          // Aumento de taxa de dinheiro
    HOLIDAY_EVENT,       // Evento de feriado
    SPECIAL_MERCHANT,    // Comerciante especial
    QUEST_CHAIN,         // Cadeia de quests
    WORLD_BOSS,          // Chefe mundial
    WEATHER_CHANGE,      // Mudança climática
    RESOURCE_SPAWN,      // Aparecimento de recursos
    TERRITORY_CONTROL,   // Controle de território
    CUSTOM               // Personalizado
};

/**
 * @brief Frequência de recorrência de evento
 */
enum class EventRecurrence {
    ONCE,                // Uma vez
    DAILY,               // Diariamente
    WEEKLY,              // Semanalmente
    MONTHLY,             // Mensalmente
    YEARLY,              // Anualmente
    CUSTOM               // Personalizado
};

/**
 * @brief Estado de evento
 */
enum class EventState {
    SCHEDULED,           // Agendado para o futuro
    ACTIVE,              // Atualmente ativo
    COMPLETED,           // Concluído
    CANCELLED,           // Cancelado
    FAILED,              // Falhou ao executar
    PAUSED               // Pausado
};

/**
 * @brief Dados de temporizador de evento
 */
struct EventTimer {
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    int32_t durationMinutes;
    bool hasCountdown;
    int32_t countdownMinutes;
    bool autoStart;
    bool autoEnd;
    
    // Para eventos recorrentes
    EventRecurrence recurrence;
    std::string recurrencePattern;  // Ex: "MON,WED,FRI" para dias da semana
    std::string recurrenceTime;     // Ex: "18:00" para horário
    int32_t recurrenceInterval;     // Ex: 2 para "a cada 2 semanas"
    std::chrono::system_clock::time_point recurrenceEndDate;
    
    // Ajustes de fuso horário
    bool adjustForServerTime;
    std::string timeZone;
};

/**
 * @brief Dados de notificação de evento
 */
struct EventNotification {
    enum class Type {
        IN_GAME,             // Notificação no jogo
        SYSTEM_MESSAGE,      // Mensagem do sistema
        EMAIL,               // Email no jogo
        CHAT_ANNOUNCEMENT,   // Anúncio no chat
        WORLD_EFFECT,        // Efeito no mundo
        SOUND_EFFECT,        // Efeito sonoro
        UI_POPUP,            // Popup na interface
        MAP_MARKER,          // Marcador no mapa
        CUSTOM               // Personalizado
    };
    
    Type type;
    std::string title;
    std::string message;
    std::vector<int32_t> minutesBeforeStart;  // Quando enviar notificações (em minutos antes)
    bool notifyOnStart;
    bool notifyOnEnd;
    bool notifyOnChange;
    std::string iconPath;
    std::string soundPath;
    
    // Alvos da notificação
    struct {
        bool allPlayers;
        int32_t minLevel;
        int32_t maxLevel;
        std::vector<std::string> specificPlayers;
        std::vector<std::string> factions;
        std::vector<std::string> guilds;
        std::vector<std::string> classes;
        std::string customTargetLogic;
    } targets;
};

/**
 * @brief Estágio de progresso de evento
 */
struct EventStage {
    int32_t stageId;
    std::string name;
    std::string description;
    int32_t durationMinutes;
    bool isOptional;
    
    // Condições para avançar
    struct {
        std::string type;              // time, kill, collect, etc.
        std::string target;            // ID ou nome do alvo
        int32_t quantity;
        bool sharedProgress;           // Se o progresso é compartilhado entre jogadores
        std::string completionLogic;   // Lógica personalizada
    } completion;
    
    // Elementos que mudam durante o estágio
    struct {
        std::vector<int32_t> spawnedMobs;
        std::vector<int32_t> spawnedNpcs;
        std::vector<int32_t> activatedPortals;
        std::vector<std::string> worldChanges;
        std::map<std::string, std::string> scriptedActions;
    } elements;
    
    // Recompensas do estágio
    struct {
        std::vector<std::pair<int32_t, int32_t>> itemRewards;  // itemId, quantidade
        int64_t goldReward;
        int64_t expReward;
        std::vector<std::string> specialRewards;
    } rewards;
};

/**
 * @brief Recompensa de evento
 */
struct EventReward {
    enum class Type {
        ITEM,                // Item
        GOLD,                // Dinheiro
        EXPERIENCE,          // Experiência
        SKILL_POINT,         // Ponto de habilidade
        REPUTATION,          // Reputação
        TITLE,               // Título
        SPECIAL_CURRENCY,    // Moeda especial
        CUSTOM               // Personalizado
    };
    
    Type type;
    int32_t resourceId;      // ID do recurso (item, título, etc.)
    std::string resourceName;
    int64_t quantity;
    
    // Condições para receber
    struct {
        int32_t minParticipationPoints;
        bool requireCompletion;
        bool topPlayersOnly;
        int32_t topPlayerCount;
        std::string customCondition;
    } conditions;
    
    // Distribuição das recompensas
    struct {
        bool scaleByContribution;
        bool equalDistribution;
        bool randomChance;
        float dropChance;    // 0.0 a 1.0
        std::string distributionLogic;
    } distribution;
};

/**
 * @brief Estatísticas de participação em evento
 */
struct EventParticipationStats {
    int32_t totalParticipants;
    int32_t activeParticipants;
    int32_t completedParticipants;
    std::map<std::string, int32_t> participantsByLevel;
    std::map<std::string, int32_t> participantsByClass;
    
    struct PlayerStats {
        std::string playerId;
        std::string playerName;
        int32_t participationPoints;
        int32_t contributionRank;
        std::vector<std::string> achievements;
        std::chrono::system_clock::time_point joinTime;
        std::chrono::system_clock::time_point lastActiveTime;
        bool completed;
        std::map<std::string, std::variant<int32_t, float, std::string>> customStats;
    };
    
    std::vector<PlayerStats> topPlayers;
    std::map<std::string, std::map<std::string, int32_t>> objectivesProgress;
};

/**
 * @brief Estatísticas gerais de eventos
 */
struct EventStatistics {
    struct EventTypeStats {
        int32_t totalRuns;
        int32_t averageParticipants;
        float averageCompletionRate;
        std::chrono::seconds averageDuration;
        int32_t totalRewardsDistributed;
    };
    
    std::map<GameEventType, EventTypeStats> statsByType;
    
    struct TimeStats {
        std::map<std::string, int32_t> participationByHour;
        std::map<std::string, int32_t> participationByDay;
        std::map<std::string, int32_t> participationByMonth;
    };
    
    TimeStats timeStats;
    
    struct PlayerStats {
        std::map<std::string, int32_t> participationByLevel;
        std::map<std::string, int32_t> participationByClass;
        std::map<std::string, int32_t> topParticipants;
    };
    
    PlayerStats playerStats;
    
    std::map<std::string, std::variant<int32_t, float, std::string>> additionalStats;
};

/**
 * @brief Representação completa de um evento de jogo
 */
struct GameEventModel {
    int32_t id;
    std::string name;
    GameEventType type;
    
    // Informações gerais
    struct {
        std::string displayName;
        std::string description;
        std::string shortDescription;
        int32_t minLevel;
        int32_t recommendedLevel;
        int32_t maxParticipants;
        bool isGuildEvent;
        bool isPvPEnabled;
        std::string category;
        std::string difficulty;  // easy, normal, hard, etc.
    } info;
    
    // Temporização do evento
    EventTimer timing;
    
    // Notificações
    std::vector<EventNotification> notifications;
    
    // Localização e área
    struct {
        int32_t mapId;
        std::string mapName;
        int32_t centerX;
        int32_t centerY;
        int32_t radius;
        bool instanced;
        int32_t maxInstances;
        std::vector<std::pair<int32_t, int32_t>> areas;  // Coordenadas de áreas adicionais
        std::vector<int32_t> relatedMaps;
    } location;
    
    // Estágios do evento
    std::vector<EventStage> stages;
    
    // Recompensas
    std::vector<EventReward> rewards;
    
    // NPCs e Mobs relacionados
    struct {
        std::vector<int32_t> npcIds;
        std::vector<int32_t> mobIds;
        std::map<int32_t, std::vector<std::pair<int32_t, int32_t>>> spawnLocations;  // entityId -> [(x, y)]
    } entities;
    
    // Requisitos para participação
    struct {
        bool hasRequirements;
        int32_t requiredLevel;
        std::vector<int32_t> requiredItems;
        std::vector<int32_t> requiredQuests;
        int32_t requiredGold;
        int32_t requiredReputation;
        std::vector<std::string> allowedClasses;
        bool guildRequired;
        int32_t minGuildMembers;
        std::string customRequirements;
    } requirements;
    
    // Sistema de pontuação
    struct {
        bool hasScoring;
        std::map<std::string, int32_t> pointsForActions;  // "kill", "collect", etc.
        bool displayLeaderboard;
        bool saveHighScores;
        std::string scoringLogic;
    } scoring;
    
    // Estado atual
    EventState state;
    std::chrono::system_clock::time_point lastRunTime;
    std::chrono::system_clock::time_point nextRunTime;
    
    // Histórico de execuções
    struct EventRunRecord {
        int32_t runId;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        EventState finalState;
        int32_t participantCount;
        int32_t completionCount;
        std::map<std::string, std::variant<int32_t, float, std::string>> runStats;
    };
    
    std::vector<EventRunRecord> runHistory;
    
    // Regras adicionais
    struct {
        bool allowLateJoin;
        bool allowReconnect;
        bool saveProgressOnDisconnect;
        bool resetOnFail;
        int32_t failCooldownMinutes;
        bool allowSoloAttempt;
        std::map<std::string, std::string> customRules;
    } rules;
    
    // Efeitos especiais
    struct {
        std::string weatherEffect;
        std::string skyboxEffect;
        std::string musicTheme;
        std::string ambientSound;
        std::vector<std::string> worldEffects;
        std::map<std::string, std::string> customEffects;
    } effects;
    
    // Metadados
    struct {
        int32_t createdTimestamp;
        int32_t lastModifiedTimestamp;
        std::string author;
        std::string version;
        std::map<std::string, std::string> extraInfo;
    } metadata;
};

/**
 * @brief Resultado de operações relacionadas a eventos
 */
struct EventOperationResult {
    bool success;
    std::string message;
    std::optional<GameEventModel> event;
};

/**
 * @brief Filtros de pesquisa para eventos
 */
struct EventFilter {
    std::optional<GameEventType> type;
    std::optional<std::string> nameContains;
    std::optional<EventState> state;
    std::optional<std::string> category;
    std::optional<int32_t> minLevel;
    std::optional<int32_t> maxLevel;
    std::optional<std::chrono::system_clock::time_point> startAfter;
    std::optional<std::chrono::system_clock::time_point> startBefore;
    std::optional<bool> isRecurring;
    std::optional<int32_t> mapId;
};

/**
 * @brief Agendador de Eventos do WYDStudio
 */
class EventScheduler {
public:
    /**
     * @brief Construtor
     * @param facade Referência ao facade principal do WYDStudio
     */
    EventScheduler(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~EventScheduler();
    
    /**
     * @brief Inicializa o agendador de eventos
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Carrega lista de eventos do servidor
     * @return true se carregado com sucesso
     */
    bool loadEvents();
    
    /**
     * @brief Obtém evento pelo ID
     * @param eventId ID do evento
     * @return Modelo do evento ou nulo se não encontrado
     */
    std::optional<GameEventModel> getEventById(int32_t eventId);
    
    /**
     * @brief Busca eventos com filtragem
     * @param filter Filtros de busca
     * @return Lista de eventos que correspondem aos filtros
     */
    std::vector<GameEventModel> searchEvents(const EventFilter& filter);
    
    /**
     * @brief Cria um novo evento com valores padrão
     * @param name Nome do novo evento
     * @param type Tipo do evento
     * @return Resultado da operação contendo o novo evento
     */
    EventOperationResult createNewEvent(const std::string& name, GameEventType type);
    
    /**
     * @brief Cria um novo evento a partir de um template
     * @param name Nome do novo evento
     * @param templateId ID do template a usar
     * @return Resultado da operação contendo o novo evento
     */
    EventOperationResult createEventFromTemplate(const std::string& name, int32_t templateId);
    
    /**
     * @brief Salva alterações em um evento
     * @param event Modelo do evento modificado
     * @return Resultado da operação
     */
    EventOperationResult saveEvent(const GameEventModel& event);
    
    /**
     * @brief Duplica um evento existente
     * @param eventId ID do evento a ser duplicado
     * @param newName Nome do evento duplicado
     * @return Resultado da operação contendo o evento duplicado
     */
    EventOperationResult duplicateEvent(int32_t eventId, const std::string& newName);
    
    /**
     * @brief Remove um evento
     * @param eventId ID do evento a ser removido
     * @return Resultado da operação
     */
    EventOperationResult removeEvent(int32_t eventId);
    
    /**
     * @brief Inicia um evento manualmente
     * @param eventId ID do evento
     * @return Resultado da operação
     */
    EventOperationResult startEvent(int32_t eventId);
    
    /**
     * @brief Para um evento manualmente
     * @param eventId ID do evento
     * @return Resultado da operação
     */
    EventOperationResult stopEvent(int32_t eventId);
    
    /**
     * @brief Pausa um evento
     * @param eventId ID do evento
     * @return Resultado da operação
     */
    EventOperationResult pauseEvent(int32_t eventId);
    
    /**
     * @brief Retoma um evento pausado
     * @param eventId ID do evento
     * @return Resultado da operação
     */
    EventOperationResult resumeEvent(int32_t eventId);
    
    /**
     * @brief Reinicia um evento
     * @param eventId ID do evento
     * @return Resultado da operação
     */
    EventOperationResult restartEvent(int32_t eventId);
    
    /**
     * @brief Agenda um evento para uma data específica
     * @param eventId ID do evento
     * @param startTime Data e hora de início
     * @return Resultado da operação
     */
    EventOperationResult scheduleEvent(
        int32_t eventId,
        std::chrono::system_clock::time_point startTime);
    
    /**
     * @brief Configura recorrência de um evento
     * @param eventId ID do evento
     * @param recurrence Tipo de recorrência
     * @param pattern Padrão de recorrência
     * @return Resultado da operação
     */
    EventOperationResult setEventRecurrence(
        int32_t eventId,
        EventRecurrence recurrence,
        const std::string& pattern);
    
    /**
     * @brief Adiciona um estágio a um evento
     * @param eventId ID do evento
     * @param stage Dados do estágio
     * @return Resultado da operação
     */
    EventOperationResult addEventStage(int32_t eventId, const EventStage& stage);
    
    /**
     * @brief Atualiza um estágio de evento
     * @param eventId ID do evento
     * @param stage Dados atualizados do estágio
     * @return Resultado da operação
     */
    EventOperationResult updateEventStage(int32_t eventId, const EventStage& stage);
    
    /**
     * @brief Remove um estágio de evento
     * @param eventId ID do evento
     * @param stageId ID do estágio
     * @return Resultado da operação
     */
    EventOperationResult removeEventStage(int32_t eventId, int32_t stageId);
    
    /**
     * @brief Adiciona uma recompensa a um evento
     * @param eventId ID do evento
     * @param reward Dados da recompensa
     * @return Resultado da operação
     */
    EventOperationResult addEventReward(int32_t eventId, const EventReward& reward);
    
    /**
     * @brief Remove uma recompensa de um evento
     * @param eventId ID do evento
     * @param rewardIndex Índice da recompensa
     * @return Resultado da operação
     */
    EventOperationResult removeEventReward(int32_t eventId, int32_t rewardIndex);
    
    /**
     * @brief Adiciona uma notificação a um evento
     * @param eventId ID do evento
     * @param notification Dados da notificação
     * @return Resultado da operação
     */
    EventOperationResult addEventNotification(int32_t eventId, const EventNotification& notification);
    
    /**
     * @brief Remove uma notificação de um evento
     * @param eventId ID do evento
     * @param notificationIndex Índice da notificação
     * @return Resultado da operação
     */
    EventOperationResult removeEventNotification(int32_t eventId, int32_t notificationIndex);
    
    /**
     * @brief Obtém a lista de próximos eventos agendados
     * @param count Número máximo de eventos a retornar
     * @param includeActive Se deve incluir eventos ativos
     * @return Lista de eventos agendados
     */
    std::vector<GameEventModel> getUpcomingEvents(int32_t count, bool includeActive = true);
    
    /**
     * @brief Obtém eventos atualmente ativos
     * @return Lista de eventos ativos
     */
    std::vector<GameEventModel> getActiveEvents();
    
    /**
     * @brief Obtém estatísticas de eventos
     * @param timeFrame Período de tempo em dias para análise
     * @return Estatísticas de eventos
     */
    EventStatistics getEventStatistics(int32_t timeFrame = 30);
    
    /**
     * @brief Obtém estatísticas de participação de um evento específico
     * @param eventId ID do evento
     * @param runId ID da execução (0 = atual/última)
     * @return Estatísticas de participação
     */
    EventParticipationStats getEventParticipationStats(int32_t eventId, int32_t runId = 0);
    
    /**
     * @brief Gera um calendário de eventos
     * @param startDate Data inicial
     * @param days Número de dias a incluir
     * @param format Formato do calendário ("json", "html", "ical")
     * @return Dados do calendário gerado
     */
    std::variant<std::string, std::vector<uint8_t>> generateEventCalendar(
        std::chrono::system_clock::time_point startDate,
        int32_t days,
        const std::string& format);
    
    /**
     * @brief Simula a execução de um evento
     * @param eventId ID do evento
     * @param participantCount Número simulado de participantes
     * @return Relatório da simulação
     */
    std::string simulateEventExecution(int32_t eventId, int32_t participantCount);
    
    /**
     * @brief Fornece uma pré-visualização do evento no jogo
     * @param event Modelo do evento a ser visualizado
     * @return Dados de visualização (imagem, informações, etc.)
     */
    std::vector<uint8_t> generateEventPreview(const GameEventModel& event);
    
    /**
     * @brief Exporta eventos para um arquivo
     * @param eventIds Lista de IDs dos eventos a exportar
     * @param exportPath Caminho do arquivo de exportação
     * @return Resultado da operação
     */
    EventOperationResult exportEvents(const std::vector<int32_t>& eventIds, const std::string& exportPath);
    
    /**
     * @brief Importa eventos de um arquivo
     * @param importPath Caminho do arquivo de importação
     * @return Resultado da operação
     */
    EventOperationResult importEvents(const std::string& importPath);
    
    /**
     * @brief Verifica a integridade de um evento
     * @param eventId ID do evento a verificar
     * @return Lista de problemas encontrados (vazia se OK)
     */
    std::vector<std::string> validateEvent(int32_t eventId);
    
    /**
     * @brief Obtém a lista de todos os tipos de evento disponíveis
     * @return Lista de tipos de evento
     */
    std::vector<std::pair<GameEventType, std::string>> getEventTypes();
    
    /**
     * @brief Obtém a lista de todas as categorias de evento disponíveis
     * @return Lista de categorias
     */
    std::vector<std::string> getEventCategories();
    
    /**
     * @brief Obtém a lista de todos os templates de evento disponíveis
     * @return Lista de templates
     */
    std::vector<std::pair<int32_t, std::string>> getEventTemplates();
    
    /**
     * @brief Recarrega todos os eventos do disco
     * @return true se recarregado com sucesso
     */
    bool reloadEvents();
    
    /**
     * @brief Aplica mudanças ao servidor sem reiniciar
     * @return true se aplicado com sucesso
     */
    bool applyChangesToLiveServer();
    
    /**
     * @brief Registra callback para notificações de alterações em eventos
     * @param eventType Tipo de notificação ("create", "update", "start", "end", etc.)
     * @param callback Função de callback
     * @return ID de registro (para cancelamento posterior)
     */
    uint32_t registerEventCallback(
        const std::string& eventType,
        std::function<void(const GameEventModel&)> callback);
    
    /**
     * @brief Cancela registro de callback
     * @param callbackId ID do callback a cancelar
     */
    void unregisterCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém o histórico de operações no evento (para undo/redo)
     * @param eventId ID do evento
     * @return Lista de operações, da mais recente para a mais antiga
     */
    std::vector<std::string> getEventEditHistory(int32_t eventId);
    
    /**
     * @brief Desfaz a última operação no evento
     * @param eventId ID do evento
     * @return Resultado da operação
     */
    EventOperationResult undoLastEventOperation(int32_t eventId);
    
    /**
     * @brief Refaz a última operação desfeita
     * @param eventId ID do evento
     * @return Resultado da operação
     */
    EventOperationResult redoEventOperation(int32_t eventId);

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade principal
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Cache e dados internos
    std::map<int32_t, GameEventModel> m_eventCache;
    
    // Histórico de operações por evento
    std::map<int32_t, std::vector<std::string>> m_eventHistory;
    std::map<int32_t, int32_t> m_historyPosition; // Posição atual para redo
    
    // Validação de eventos
    bool validateEventIntegrity(const GameEventModel& event);
    
    // Gerenciamento de arquivos
    std::string getEventsFilePath();
    
    // Utilitários de serialização
    json serializeEventToJson(const GameEventModel& event);
    GameEventModel deserializeEventFromJson(const json& json);
    
    // Geração de IDs únicos
    int32_t generateUniqueEventId();
    int32_t generateUniqueStageId(const GameEventModel& event);
    int32_t generateUniqueRunId(const GameEventModel& event);
    
    // Processamento de recorrência
    std::chrono::system_clock::time_point calculateNextOccurrence(
        const EventTimer& timer,
        std::chrono::system_clock::time_point after = std::chrono::system_clock::now());
};

}} // namespace wydstudio::ui

#endif // WYDSTUDIO_EVENT_SCHEDULER_H