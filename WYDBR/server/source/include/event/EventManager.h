/**
 * EventManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/event/EventManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _EVENT_MANAGER_H_
#define _EVENT_MANAGER_H_

/**
 * @file EventManager.h
 * @brief Gerenciador de eventos do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de eventos do WYDBR,
 * responsável por controlar os eventos do jogo e eventos de mundo,
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

namespace wydbr {
namespace event {

/**
 * @brief Estado do evento
 */
enum /**
 * Classe WYDEventState
 * 
 * Esta classe implementa a funcionalidade WYDEventState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventState {
    NONE = 0,               // Nenhum
    SCHEDULED = 1,          // Agendado
    RUNNING = 2,            // Em execução
    PAUSED = 3,             // Pausado
    FINISHED = 4,           // Finalizado
    CANCELED = 5,           // Cancelado
    FAILED = 6,             // Falhou
    WAITING = 7,            // Aguardando
    PENDING = 8,            // Pendente
    CUSTOM_1 = 9,           // Personalizado 1
    CUSTOM_2 = 10,          // Personalizado 2
    CUSTOM_3 = 11,          // Personalizado 3
    UNKNOWN = 12            // Desconhecido
};

/**
 * @brief Tipo de evento
 */
enum /**
 * Classe WYDEventType
 * 
 * Esta classe implementa a funcionalidade WYDEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventType {
    NONE = 0,               // Nenhum
    GLOBAL = 1,             // Global
    WORLD = 2,              // Mundo
    GUILD = 3,              // Guilda
    ARENA = 4,              // Arena
    PVP = 5,                // PvP
    DUNGEON = 6,            // Masmorra
    RAID = 7,               // Raid
    BATTLEFIELD = 8,        // Campo de batalha
    SEASONAL = 9,           // Sazonal
    QUEST = 10,             // Missão
    MARKET = 11,            // Mercado
    BOSS = 12,              // Chefe
    INVASION = 13,          // Invasão
    REWARD = 14,            // Recompensa
    MAINTENANCE = 15,       // Manutenção
    ANNOUNCEMENT = 16,      // Anúncio
    CUSTOM_1 = 17,          // Personalizado 1
    CUSTOM_2 = 18,          // Personalizado 2
    CUSTOM_3 = 19,          // Personalizado 3
    UNKNOWN = 20            // Desconhecido
};

/**
 * @brief Categoria do evento
 */
enum /**
 * Classe WYDEventCategory
 * 
 * Esta classe implementa a funcionalidade WYDEventCategory conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventCategory {
    NONE = 0,               // Nenhum
    COMBAT = 1,             // Combate
    EXPLORATION = 2,        // Exploração
    SOCIAL = 3,             // Social
    PROGRESSION = 4,        // Progressão
    ECONOMY = 5,            // Economia
    COMPETITION = 6,        // Competição
    ACHIEVEMENT = 7,        // Conquista
    STORYLINE = 8,          // Enredo
    HOLIDAY = 9,            // Feriado
    SPECIAL = 10,           // Especial
    SYSTEM = 11,            // Sistema
    CUSTOM_1 = 12,          // Personalizado 1
    CUSTOM_2 = 13,          // Personalizado 2
    CUSTOM_3 = 14,          // Personalizado 3
    UNKNOWN = 15            // Desconhecido
};

/**
 * @brief Recorrência do evento
 */
enum /**
 * Classe WYDEventRecurrence
 * 
 * Esta classe implementa a funcionalidade WYDEventRecurrence conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventRecurrence {
    NONE = 0,               // Nenhum
    ONE_TIME = 1,           // Uma vez
    DAILY = 2,              // Diário
    WEEKLY = 3,             // Semanal
    MONTHLY = 4,            // Mensal
    YEARLY = 5,             // Anual
    CUSTOM = 6,             // Personalizado
    UNKNOWN = 7             // Desconhecido
};

/**
 * @brief Nível de acesso do evento
 */
enum /**
 * Classe WYDEventAccessLevel
 * 
 * Esta classe implementa a funcionalidade WYDEventAccessLevel conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventAccessLevel {
    NONE = 0,               // Nenhum
    ALL = 1,                // Todos
    BEGINNER = 2,           // Iniciante
    INTERMEDIATE = 3,       // Intermediário
    ADVANCED = 4,           // Avançado
    ELITE = 5,              // Elite
    GUILD = 6,              // Guilda
    VIP = 7,                // VIP
    ADMIN = 8,              // Administrador
    CUSTOM_1 = 9,           // Personalizado 1
    CUSTOM_2 = 10,          // Personalizado 2
    CUSTOM_3 = 11,          // Personalizado 3
    UNKNOWN = 12            // Desconhecido
};

/**
 * @brief Trigger de evento
 */
enum /**
 * Classe WYDEventTrigger
 * 
 * Esta classe implementa a funcionalidade WYDEventTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventTrigger {
    NONE = 0,               // Nenhum
    TIME = 1,               // Tempo
    LOGIN = 2,              // Login
    LOGOUT = 3,             // Logout
    LEVEL_UP = 4,           // Subida de nível
    DEATH = 5,              // Morte
    KILL = 6,               // Abate
    ENTER_AREA = 7,         // Entrar em área
    LEAVE_AREA = 8,         // Sair de área
    ITEM_ACQUIRE = 9,       // Adquirir item
    ITEM_USE = 10,          // Usar item
    SKILL_USE = 11,         // Usar habilidade
    QUEST_START = 12,       // Iniciar missão
    QUEST_COMPLETE = 13,    // Completar missão
    TRADE = 14,             // Comércio
    GUILD_JOIN = 15,        // Entrar em guilda
    GUILD_LEAVE = 16,       // Sair de guilda
    SCRIPT = 17,            // Script
    API = 18,               // API
    CUSTOM_1 = 19,          // Personalizado 1
    CUSTOM_2 = 20,          // Personalizado 2
    CUSTOM_3 = 21,          // Personalizado 3
    UNKNOWN = 22            // Desconhecido
};

/**
 * @brief Notificação do evento
 */
enum /**
 * Classe WYDEventNotification
 * 
 * Esta classe implementa a funcionalidade WYDEventNotification conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventNotification {
    NONE = 0,               // Nenhum
    SYSTEM = 1,             // Sistema
    CHAT = 2,               // Chat
    POPUP = 3,              // Pop-up
    MAIL = 4,               // Correio
    SOUND = 5,              // Som
    ANIMATION = 6,          // Animação
    HUD = 7,                // HUD
    BROADCAST = 8,          // Broadcast
    TARGETED = 9,           // Direcionado
    CUSTOM_1 = 10,          // Personalizado 1
    CUSTOM_2 = 11,          // Personalizado 2
    CUSTOM_3 = 12,          // Personalizado 3
    UNKNOWN = 13            // Desconhecido
};

/**
 * @brief Resultado do evento
 */
enum /**
 * Classe WYDEventResult
 * 
 * Esta classe implementa a funcionalidade WYDEventResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventResult {
    NONE = 0,               // Nenhum
    SUCCESS = 1,            // Sucesso
    FAILURE = 2,            // Falha
    PARTIAL = 3,            // Parcial
    CANCELED = 4,           // Cancelado
    ERROR = 5,              // Erro
    TIMEOUT = 6,            // Timeout
    PENDING = 7,            // Pendente
    SCHEDULED = 8,          // Agendado
    CUSTOM_1 = 9,           // Personalizado 1
    CUSTOM_2 = 10,          // Personalizado 2
    CUSTOM_3 = 11,          // Personalizado 3
    UNKNOWN = 12            // Desconhecido
};

/**
 * @brief Tipo de recompensa do evento
 */
enum /**
 * Classe WYDEventRewardType
 * 
 * Esta classe implementa a funcionalidade WYDEventRewardType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventRewardType {
    NONE = 0,               // Nenhum
    ITEM = 1,               // Item
    GOLD = 2,               // Ouro
    EXPERIENCE = 3,         // Experiência
    SKILL = 4,              // Habilidade
    TITLE = 5,              // Título
    REPUTATION = 6,         // Reputação
    HONOR = 7,              // Honra
    TOKEN = 8,              // Token
    ACHIEVEMENT = 9,        // Conquista
    STAT_BOOST = 10,        // Aumento de atributo
    BUFF = 11,              // Buff
    CURRENCY = 12,          // Moeda
    ACCESS = 13,            // Acesso
    COSMETIC = 14,          // Cosmético
    MOUNT = 15,             // Montaria
    PET = 16,               // Pet
    CUSTOM_1 = 17,          // Personalizado 1
    CUSTOM_2 = 18,          // Personalizado 2
    CUSTOM_3 = 19,          // Personalizado 3
    UNKNOWN = 20            // Desconhecido
};

/**
 * @brief Recompensa de evento
 */
struct EventReward {
    EventRewardType Type;   // Tipo
    uint32_t ID;            // ID
    uint32_t Amount;        // Quantidade
    uint32_t Chance;        // Chance
    bool Guaranteed;        // Garantido
    uint8_t RequiredRank;   // Rank requerido
    std::string Description; // Descrição
    
    EventReward() : Type(EventRewardType::NONE), ID(0), Amount(0),
        Chance(100), Guaranteed(false), RequiredRank(0) {}
};

/**
 * @brief Requerimento de evento
 */
struct EventRequirement {
    uint8_t RequiredLevel;  // Nível requerido
    uint8_t MaxLevel;       // Nível máximo
    uint8_t RequiredClass;  // Classe requerida
    uint32_t RequiredItem;  // Item requerido
    uint32_t RequiredGold;  // Ouro requerido
    uint16_t RequiredSkill; // Habilidade requerida
    uint32_t RequiredQuest; // Missão requerida
    uint32_t RequiredGuild; // Guilda requerida
    uint32_t RequiredTitle; // Título requerido
    uint32_t RequiredAchievement; // Conquista requerida
    uint32_t RequiredReputation; // Reputação requerida
    bool RequireParty;      // Requer grupo
    uint8_t MinPartySize;   // Tamanho mínimo do grupo
    uint8_t MaxPartySize;   // Tamanho máximo do grupo
    
    EventRequirement() : RequiredLevel(0), MaxLevel(0), RequiredClass(0),
        RequiredItem(0), RequiredGold(0), RequiredSkill(0), RequiredQuest(0),
        RequiredGuild(0), RequiredTitle(0), RequiredAchievement(0),
        RequiredReputation(0), RequireParty(false), MinPartySize(0), MaxPartySize(0) {}
};

/**
 * @brief Configuração de período de evento
 */
struct EventPeriod {
    uint32_t StartTime;     // Tempo de início
    uint32_t EndTime;       // Tempo de fim
    uint32_t Duration;      // Duração
    uint32_t CooldownTime;  // Tempo de cooldown
    EventRecurrence Recurrence; // Recorrência
    uint32_t RecurrenceInterval; // Intervalo de recorrência
    uint32_t PreparationTime; // Tempo de preparação
    uint32_t WarningTime;   // Tempo de aviso
    
    EventPeriod() : StartTime(0), EndTime(0), Duration(0), CooldownTime(0),
        Recurrence(EventRecurrence::NONE), RecurrenceInterval(0),
        PreparationTime(0), WarningTime(0) {}
};

/**
 * @brief Configuração de evento
 */
struct EventConfig {
    uint32_t EventID;       // ID do evento
    std::string Name;       // Nome
    std::string Description; // Descrição
    EventType Type;         // Tipo
    EventCategory Category; // Categoria
    EventRecurrence Recurrence; // Recorrência
    EventAccessLevel AccessLevel; // Nível de acesso
    EventTrigger Trigger;   // Trigger
    EventPeriod Period;     // Período
    EventRequirement Requirements; // Requerimentos
    std::vector<EventReward> Rewards; // Recompensas
    std::vector<EventNotification> Notifications; // Notificações
    uint16_t MapID;         // ID do mapa
    uint16_t LocX;          // Localização X
    uint16_t LocY;          // Localização Y
    uint16_t Radius;        // Raio
    uint32_t MaxParticipants; // Máximo de participantes
    uint32_t MinParticipants; // Mínimo de participantes
    uint8_t MinLevel;       // Nível mínimo
    uint8_t MaxLevel;       // Nível máximo
    std::string ScriptName; // Nome do script
    bool Enabled;           // Habilitado
    
    EventConfig() : EventID(0), Type(EventType::NONE), Category(EventCategory::NONE),
        Recurrence(EventRecurrence::NONE), AccessLevel(EventAccessLevel::ALL),
        Trigger(EventTrigger::NONE), MapID(0), LocX(0), LocY(0), Radius(0),
        MaxParticipants(0), MinParticipants(0), MinLevel(0), MaxLevel(0),
        Enabled(true) {}
};

/**
 * @brief Instância de evento
 */
struct EventInstance {
    uint32_t InstanceID;    // ID da instância
    uint32_t EventID;       // ID do evento
    EventState State;       // Estado
    uint32_t StartTime;     // Tempo de início
    uint32_t EndTime;       // Tempo de fim
    uint32_t ParticipantCount; // Contagem de participantes
    EventResult Result;     // Resultado
    
    EventInstance() : InstanceID(0), EventID(0), State(EventState::NONE),
        StartTime(0), EndTime(0), ParticipantCount(0), Result(EventResult::NONE) {}
};

/**
 * @brief Participante de evento
 */
struct EventParticipant {
    uint32_t ParticipantID; // ID do participante
    uint32_t CharacterID;   // ID do personagem
    uint32_t InstanceID;    // ID da instância
    uint32_t JoinTime;      // Tempo de entrada
    uint32_t LeaveTime;     // Tempo de saída
    uint8_t Rank;           // Rank
    uint32_t Score;         // Pontuação
    EventResult Result;     // Resultado
    std::vector<EventReward> Rewards; // Recompensas
    
    EventParticipant() : ParticipantID(0), CharacterID(0), InstanceID(0),
        JoinTime(0), LeaveTime(0), Rank(0), Score(0), Result(EventResult::NONE) {}
};

/**
 * @brief Log de evento
 */
struct EventLog {
    uint32_t LogID;         // ID do log
    uint32_t InstanceID;    // ID da instância
    uint32_t Timestamp;     // Timestamp
    std::string Message;    // Mensagem
    uint8_t LogLevel;       // Nível do log
    
    EventLog() : LogID(0), InstanceID(0), Timestamp(0), LogLevel(0) {}
};

/**
 * @brief Estatísticas de evento
 */
struct EventStats {
    uint32_t EventID;       // ID do evento
    uint32_t InstanceCount; // Contagem de instâncias
    uint32_t ParticipantCount; // Contagem de participantes
    uint32_t SuccessCount;  // Contagem de sucessos
    uint32_t FailureCount;  // Contagem de falhas
    uint32_t CancelCount;   // Contagem de cancelamentos
    float AverageScore;     // Pontuação média
    float AverageDuration;  // Duração média
    uint32_t TopScore;      // Maior pontuação
    uint32_t LastRunTime;   // Último tempo de execução
    
    EventStats() : EventID(0), InstanceCount(0), ParticipantCount(0),
        SuccessCount(0), FailureCount(0), CancelCount(0), AverageScore(0.0f),
        AverageDuration(0.0f), TopScore(0), LastRunTime(0) {}
};

/**
 * @brief Estado global de evento
 */
struct EventGlobalState {
    uint32_t CurrentActiveEvents; // Eventos ativos atuais
    uint32_t TotalEventsToday;   // Total de eventos hoje
    uint32_t NextEventTime;      // Tempo do próximo evento
    uint32_t NextEventID;        // ID do próximo evento
    uint32_t SpecialEventID;     // ID do evento especial
    bool MaintenanceMode;        // Modo de manutenção
    
    EventGlobalState() : CurrentActiveEvents(0), TotalEventsToday(0),
        NextEventTime(0), NextEventID(0), SpecialEventID(0),
        MaintenanceMode(false) {}
};

/**
 * @brief Tipo de callback de evento
 */
using EventCallback = std::function<EventResult(const EventInstance&)>;

/**
 * @brief Tipo de callback de participante
 */
using ParticipantCallback = std::function<void(const EventParticipant&, const EventInstance&)>;

/**
 * @brief Gerenciador de eventos
 * 
 * Esta classe é responsável por gerenciar os eventos do jogo e eventos de mundo.
 */
/**
 * Classe WYDEventManager
 * 
 * Esta classe implementa a funcionalidade WYDEventManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static EventManager& getInstance() {
        static EventManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
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
     * @brief Carrega a configuração de eventos
     * @return true se carregado com sucesso
     */
    bool loadEventConfig();
    
    /**
     * @brief Cria um evento
     * @param config Configuração
     * @return ID do evento ou 0 em caso de erro
     */
    uint32_t createEvent(const EventConfig& config);
    
    /**
     * @brief Remove um evento
     * @param eventID ID do evento
     * @return true se removido com sucesso
     */
    bool removeEvent(uint32_t eventID);
    
    /**
     * @brief Atualiza um evento
     * @param config Configuração
     * @return true se atualizado com sucesso
     */
    bool updateEvent(const EventConfig& config);
    
    /**
     * @brief Habilita ou desabilita um evento
     * @param eventID ID do evento
     * @param enabled Habilitado
     * @return true se alterado com sucesso
     */
    bool enableEvent(uint32_t eventID, bool enabled);
    
    /**
     * @brief Obtém a configuração de um evento
     * @param eventID ID do evento
     * @return Configuração
     */
    EventConfig getEventConfig(uint32_t eventID);
    
    /**
     * @brief Inicia uma instância de evento manualmente
     * @param eventID ID do evento
     * @return ID da instância ou 0 em caso de erro
     */
    uint32_t startEvent(uint32_t eventID);
    
    /**
     * @brief Para uma instância de evento
     * @param instanceID ID da instância
     * @param result Resultado
     * @return true se parado com sucesso
     */
    bool stopEvent(uint32_t instanceID, EventResult result);
    
    /**
     * @brief Pausa uma instância de evento
     * @param instanceID ID da instância
     * @return true se pausado com sucesso
     */
    bool pauseEvent(uint32_t instanceID);
    
    /**
     * @brief Retoma uma instância de evento
     * @param instanceID ID da instância
     * @return true se retomado com sucesso
     */
    bool resumeEvent(uint32_t instanceID);
    
    /**
     * @brief Cancela uma instância de evento
     * @param instanceID ID da instância
     * @return true se cancelado com sucesso
     */
    bool cancelEvent(uint32_t instanceID);
    
    /**
     * @brief Registra um personagem em um evento
     * @param instanceID ID da instância
     * @param characterID ID do personagem
     * @return ID do participante ou 0 em caso de erro
     */
    uint32_t registerParticipant(uint32_t instanceID, uint32_t characterID);
    
    /**
     * @brief Remove um participante de um evento
     * @param instanceID ID da instância
     * @param characterID ID do personagem
     * @return true se removido com sucesso
     */
    bool unregisterParticipant(uint32_t instanceID, uint32_t characterID);
    
    /**
     * @brief Atualiza a pontuação de um participante
     * @param instanceID ID da instância
     * @param characterID ID do personagem
     * @param score Pontuação
     * @return true se atualizado com sucesso
     */
    bool updateParticipantScore(uint32_t instanceID, uint32_t characterID, uint32_t score);
    
    /**
     * @brief Define o rank de um participante
     * @param instanceID ID da instância
     * @param characterID ID do personagem
     * @param rank Rank
     * @return true se definido com sucesso
     */
    bool setParticipantRank(uint32_t instanceID, uint32_t characterID, uint8_t rank);
    
    /**
     * @brief Concede uma recompensa a um participante
     * @param instanceID ID da instância
     * @param characterID ID do personagem
     * @param reward Recompensa
     * @return true se concedido com sucesso
     */
    bool giveParticipantReward(uint32_t instanceID, uint32_t characterID, const EventReward& reward);
    
    /**
     * @brief Concede recompensas com base no rank
     * @param instanceID ID da instância
     * @return true se concedido com sucesso
     */
    bool giveRankBasedRewards(uint32_t instanceID);
    
    /**
     * @brief Obtém os participantes de um evento
     * @param instanceID ID da instância
     * @return Lista de participantes
     */
    std::vector<EventParticipant> getEventParticipants(uint32_t instanceID);
    
    /**
     * @brief Obtém um participante de um evento
     * @param instanceID ID da instância
     * @param characterID ID do personagem
     * @return Participante
     */
    EventParticipant getEventParticipant(uint32_t instanceID, uint32_t characterID);
    
    /**
     * @brief Obtém as instâncias de um evento
     * @param eventID ID do evento
     * @return Lista de instâncias
     */
    std::vector<EventInstance> getEventInstances(uint32_t eventID);
    
    /**
     * @brief Obtém uma instância de evento
     * @param instanceID ID da instância
     * @return Instância
     */
    EventInstance getEventInstance(uint32_t instanceID);
    
    /**
     * @brief Obtém as estatísticas de um evento
     * @param eventID ID do evento
     * @return Estatísticas
     */
    EventStats getEventStats(uint32_t eventID);
    
    /**
     * @brief Obtém o estado global de eventos
     * @return Estado global
     */
    EventGlobalState getGlobalState();
    
    /**
     * @brief Registra um callback de evento
     * @param eventID ID do evento
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerEventCallback(uint32_t eventID, EventCallback callback);
    
    /**
     * @brief Remove um callback de evento
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEventCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de participante
     * @param eventID ID do evento
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerParticipantCallback(uint32_t eventID, ParticipantCallback callback);
    
    /**
     * @brief Remove um callback de participante
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterParticipantCallback(uint32_t callbackId);
    
    /**
     * @brief Adiciona um log a uma instância de evento
     * @param instanceID ID da instância
     * @param message Mensagem
     * @param logLevel Nível do log
     * @return ID do log ou 0 em caso de erro
     */
    uint32_t addEventLog(uint32_t instanceID, const std::string& message, uint8_t logLevel = 0);
    
    /**
     * @brief Obtém os logs de uma instância de evento
     * @param instanceID ID da instância
     * @return Lista de logs
     */
    std::vector<EventLog> getEventLogs(uint32_t instanceID);
    
    /**
     * @brief Envia uma notificação de evento
     * @param instanceID ID da instância
     * @param message Mensagem
     * @param type Tipo de notificação
     * @param targetID ID do alvo (0 para todos)
     * @return true se enviado com sucesso
     */
    bool sendEventNotification(uint32_t instanceID, const std::string& message, EventNotification type, uint32_t targetID = 0);
    
    /**
     * @brief Agenda um evento
     * @param eventID ID do evento
     * @param startTime Tempo de início
     * @param endTime Tempo de fim
     * @param recurrence Recorrência
     * @param recurrenceInterval Intervalo de recorrência
     * @return true se agendado com sucesso
     */
    bool scheduleEvent(uint32_t eventID, uint32_t startTime, uint32_t endTime, EventRecurrence recurrence = EventRecurrence::ONE_TIME, uint32_t recurrenceInterval = 0);
    
    /**
     * @brief Verifica se um personagem atende aos requisitos de um evento
     * @param eventID ID do evento
     * @param characterID ID do personagem
     * @return true se atender
     */
    bool meetsEventRequirements(uint32_t eventID, uint32_t characterID);
    
    /**
     * @brief Verifica se um personagem pode participar de um evento
     * @param eventID ID do evento
     * @param characterID ID do personagem
     * @return true se puder participar
     */
    bool canParticipate(uint32_t eventID, uint32_t characterID);
    
    /**
     * @brief Obtém o próximo evento agendado
     * @param eventID ID do evento (saída)
     * @param startTime Tempo de início (saída)
     * @return true se encontrado
     */
    bool getNextScheduledEvent(uint32_t& eventID, uint32_t& startTime);
    
    /**
     * @brief Obtém os eventos ativos
     * @return Lista de IDs de instâncias
     */
    std::vector<uint32_t> getActiveEvents();
    
    /**
     * @brief Obtém os eventos agendados
     * @return Lista de IDs de eventos
     */
    std::vector<uint32_t> getScheduledEvents();
    
    /**
     * @brief Obtém eventos por tipo
     * @param type Tipo
     * @return Lista de IDs de eventos
     */
    std::vector<uint32_t> getEventsByType(EventType type);
    
    /**
     * @brief Obtém eventos por categoria
     * @param category Categoria
     * @return Lista de IDs de eventos
     */
    std::vector<uint32_t> getEventsByCategory(EventCategory category);
    
    /**
     * @brief Obtém eventos por nível de acesso
     * @param accessLevel Nível de acesso
     * @return Lista de IDs de eventos
     */
    std::vector<uint32_t> getEventsByAccessLevel(EventAccessLevel accessLevel);
    
    /**
     * @brief Ativa o modo de manutenção
     * @param enabled Habilitado
     */
    void setMaintenanceMode(bool enabled);
    
    /**
     * @brief Verifica se o modo de manutenção está ativado
     * @return true se estiver ativado
     */
    bool isMaintenanceMode() const;
    
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
    
    /**
     * @brief Define o modo de log
     * @param logging Modo de log
     */
    void setLoggingEnabled(bool logging);
    
    /**
     * @brief Verifica se o modo de log está ativado
     * @return true se estiver ativado
     */
    bool isLoggingEnabled() const;
    
private:
    /**
     * @brief Construtor privado
     */
    EventManager();
    
    /**
     * @brief Destrutor privado
     */
    ~EventManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    EventManager(const EventManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    EventManager& operator=(const EventManager&) = delete;
    
    // Eventos
    std::unordered_map<uint32_t, EventConfig> _events;
    std::atomic<uint32_t> _nextEventID;
    
    // Instâncias
    std::unordered_map<uint32_t, EventInstance> _instances;
    std::map<uint32_t, std::vector<uint32_t>> _eventInstances;
    std::atomic<uint32_t> _nextInstanceID;
    
    // Participantes
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, EventParticipant>> _participants;
    std::atomic<uint32_t> _nextParticipantID;
    
    // Logs
    std::unordered_map<uint32_t, std::vector<EventLog>> _logs;
    std::atomic<uint32_t> _nextLogID;
    
    // Estatísticas
    std::unordered_map<uint32_t, EventStats> _stats;
    
    // Estado global
    EventGlobalState _globalState;
    
    // Callbacks
    std::unordered_map<uint32_t, EventCallback> _eventCallbacks;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _eventCallbacksByEvent;
    std::unordered_map<uint32_t, ParticipantCallback> _participantCallbacks;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _participantCallbacksByEvent;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _eventMutex;
    mutable std::mutex _instanceMutex;
    mutable std::mutex _participantMutex;
    mutable std::mutex _logMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    /**
     * @brief Verifica eventos agendados
     * @param currentTime Tempo atual
     */
    void checkScheduledEvents(uint32_t currentTime);
    
    /**
     * @brief Atualiza instâncias ativas
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void updateActiveInstances(uint32_t deltaTime);
    
    /**
     * @brief Executa um callback de evento
     * @param instanceID ID da instância
     * @return Resultado
     */
    EventResult executeEventCallback(uint32_t instanceID);
    
    /**
     * @brief Executa um callback de participante
     * @param instanceID ID da instância
     * @param characterID ID do personagem
     */
    void executeParticipantCallback(uint32_t instanceID, uint32_t characterID);
    
    /**
     * @brief Atualiza as estatísticas de um evento
     * @param eventID ID do evento
     */
    void updateEventStats(uint32_t eventID);
    
    /**
     * @brief Atualiza o estado global
     */
    void updateGlobalState();
    
    /**
     * @brief Processa o resultado de um evento
     * @param instanceID ID da instância
     * @param result Resultado
     */
    void processEventResult(uint32_t instanceID, EventResult result);
    
    /**
     * @brief Gera recompensas para um evento
     * @param instanceID ID da instância
     */
    void generateEventRewards(uint32_t instanceID);
    
    /**
     * @brief Carrega dados de eventos do banco de dados
     * @return true se carregado com sucesso
     */
    bool loadEventsFromDatabase();
    
    /**
     * @brief Salva dados de eventos no banco de dados
     * @return true se salvo com sucesso
     */
    bool saveEventsToDatabase();
};

} // namespace event
} // namespace wydbr

#endif // _EVENT_MANAGER_H_