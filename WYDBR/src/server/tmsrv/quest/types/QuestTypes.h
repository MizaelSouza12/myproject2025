/**
 * QuestTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/types/QuestTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUESTTYPES_H
#define QUESTTYPES_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <memory>
#include <set>
#include <bitset>
#include <array>
#include <variant>
#include <optional>

#include "../../../../core/WYDTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file QuestTypes.h
 * @brief Definições de tipos para o sistema avançado de quests
 * 
 * Este arquivo contém as definições de tipos para o sistema avançado de quests,
 * que implementa um gerenciador de quests flexível, dinâmico e altamente personalizável,
 * incluindo múltiplos tipos de quests, objetivos complexos, recompensas dinâmicas e
 * sistemas de progresso persistentes.
 */

namespace wyd {
namespace server {
namespace quest {

// Constantes do sistema de quests
constexpr size_t MAX_ACTIVE_QUESTS = 50;           // Máximo de quests ativas por jogador
constexpr size_t MAX_QUEST_OBJECTIVES = 20;        // Máximo de objetivos por quest
constexpr size_t MAX_QUEST_REWARDS = 10;           // Máximo de recompensas por quest
constexpr size_t MAX_QUEST_STEPS = 15;             // Máximo de etapas por quest
constexpr size_t MAX_PREREQUISITES = 10;           // Máximo de pré-requisitos por quest
constexpr size_t MAX_QUEST_GROUPS = 100;           // Máximo de grupos de quests
constexpr size_t MAX_QUEST_CHAINS = 50;            // Máximo de cadeias de quests
constexpr size_t MAX_DIALOGUE_OPTIONS = 6;         // Máximo de opções de diálogo
constexpr size_t MAX_TRIGGER_CONDITIONS = 15;      // Máximo de condições de gatilho
constexpr size_t MAX_QUEST_TRIGGERS = 30;          // Máximo de gatilhos por quest
constexpr size_t MAX_ITEM_INTERACTIONS = 100;      // Máximo de interações de item
constexpr size_t MAX_NPC_INTERACTIONS = 200;       // Máximo de interações de NPC
constexpr size_t MAX_LOCATION_TRIGGERS = 150;      // Máximo de gatilhos de localização
constexpr size_t MAX_QUEST_NOTIFICATIONS = 50;     // Máximo de notificações de quest

/**
 * @brief Categoria de quest
 */
enum class QuestCategory : BYTE {
    MAIN_STORY = 0,              // História principal
    SIDE_STORY = 1,              // História secundária
    DAILY = 2,                   // Diária
    WEEKLY = 3,                  // Semanal
    MONTHLY = 4,                 // Mensal
    SEASONAL = 5,                // Sazonal
    EVENT = 6,                   // Evento
    GUILD = 7,                   // Guilda
    ACHIEVEMENT = 8,             // Conquista
    HUNTING = 9,                 // Caça
    GATHERING = 10,              // Coleta
    CRAFTING = 11,               // Criação
    TRADING = 12,                // Comércio
    EXPLORATION = 13,            // Exploração
    REPUTATION = 14,             // Reputação
    PVP = 15,                    // PvP
    DUNGEON = 16,                // Masmorra
    RAID = 17,                   // Raid
    HIDDEN = 18,                 // Escondida
    CUSTOM = 19,                 // Personalizada
};

/**
 * @brief Dificuldade de quest
 */
enum class QuestDifficulty : BYTE {
    TRIVIAL = 0,                 // Trivial
    EASY = 1,                    // Fácil
    NORMAL = 2,                  // Normal
    CHALLENGING = 3,             // Desafiadora
    HARD = 4,                    // Difícil
    VERY_HARD = 5,               // Muito difícil
    EXPERT = 6,                  // Especialista
    MASTER = 7,                  // Mestre
    LEGENDARY = 8,               // Lendária
    MYTHIC = 9,                  // Mítica
    CUSTOM = 10,                 // Personalizada
};

/**
 * @brief Status da quest
 */
enum class QuestStatus : BYTE {
    UNAVAILABLE = 0,             // Indisponível
    AVAILABLE = 1,               // Disponível
    ACTIVE = 2,                  // Ativa
    COMPLETED = 3,               // Completada
    FAILED = 4,                  // Falhou
    ABANDONED = 5,               // Abandonada
    ON_HOLD = 6,                 // Em espera
    EXPIRED = 7,                 // Expirada
    REPEATABLE = 8,              // Repetível
    HIDDEN = 9,                  // Escondida
    CUSTOM = 10,                 // Personalizada
};

/**
 * @brief Tipo de objetivo de quest
 */
enum class QuestObjectiveType : BYTE {
    KILL_MONSTER = 0,            // Matar monstro
    COLLECT_ITEM = 1,            // Coletar item
    TALK_TO_NPC = 2,             // Falar com NPC
    REACH_LOCATION = 3,          // Alcançar localização
    ESCORT_NPC = 4,              // Escoltar NPC
    DEFEND_LOCATION = 5,         // Defender localização
    USE_ITEM = 6,                // Usar item
    USE_SKILL = 7,               // Usar habilidade
    CRAFT_ITEM = 8,              // Criar item
    GATHER_RESOURCE = 9,         // Coletar recurso
    DELIVER_ITEM = 10,           // Entregar item
    DEFEAT_PLAYER = 11,          // Derrotar jogador
    COMPLETE_DUNGEON = 12,       // Completar masmorra
    TRIGGER_EVENT = 13,          // Acionar evento
    REACH_LEVEL = 14,            // Alcançar nível
    SPEND_CURRENCY = 15,         // Gastar moeda
    EARN_CURRENCY = 16,          // Ganhar moeda
    GAIN_REPUTATION = 17,        // Ganhar reputação
    WAIT_TIME = 18,              // Esperar tempo
    CUSTOM = 19,                 // Personalizado
};

/**
 * @brief Tipo de recompensa de quest
 */
enum class QuestRewardType : BYTE {
    EXPERIENCE = 0,              // Experiência
    ITEM = 1,                    // Item
    CURRENCY = 2,                // Moeda
    REPUTATION = 3,              // Reputação
    SKILL_POINT = 4,             // Ponto de habilidade
    ATTRIBUTE_POINT = 5,         // Ponto de atributo
    SKILL = 6,                   // Habilidade
    TITLE = 7,                   // Título
    TELEPORT = 8,                // Teleporte
    MOUNT = 9,                   // Montaria
    PET = 10,                    // Pet
    APPEARANCE = 11,             // Aparência
    BUFF = 12,                   // Buff
    UNLOCK_FEATURE = 13,         // Desbloquear recurso
    GUILD_POINTS = 14,           // Pontos de guilda
    CUSTOM = 15,                 // Personalizado
};

/**
 * @brief Tipo de interação de quest
 */
enum class QuestInteractionType : BYTE {
    DIALOGUE = 0,                // Diálogo
    ITEM_GIVE = 1,               // Dar item
    ITEM_TAKE = 2,               // Pegar item
    ITEM_EXAMINE = 3,            // Examinar item
    PURCHASE = 4,                // Comprar
    SELL = 5,                    // Vender
    LOCATION_ENTER = 6,          // Entrar em localização
    LOCATION_EXIT = 7,           // Sair de localização
    NPC_ATTACK = 8,              // Atacar NPC
    NPC_DEFEND = 9,              // Defender NPC
    NPC_FOLLOW = 10,             // Seguir NPC
    NPC_RESCUE = 11,             // Resgatar NPC
    EVENT_TRIGGER = 12,          // Acionar evento
    PUZZLE_SOLVE = 13,           // Resolver quebra-cabeça
    CUSTOM = 14,                 // Personalizado
};

/**
 * @brief Tipo de requisito de quest
 */
enum class QuestRequirementType : BYTE {
    LEVEL = 0,                   // Nível
    ITEM = 1,                    // Item
    SKILL = 2,                   // Habilidade
    REPUTATION = 3,              // Reputação
    QUEST_COMPLETED = 4,         // Quest completada
    ATTRIBUTE = 5,               // Atributo
    CLASS = 6,                   // Classe
    RACE = 7,                    // Raça
    FACTION = 8,                 // Facção
    GUILD_RANK = 9,              // Rank de guilda
    CURRENCY = 10,               // Moeda
    ACHIEVEMENT = 11,            // Conquista
    TIME_OF_DAY = 12,            // Hora do dia
    CUSTOM = 13,                 // Personalizado
};

/**
 * @brief Tipo de gatilho de quest
 */
enum class QuestTriggerType : BYTE {
    MONSTER_KILL = 0,            // Morte de monstro
    ITEM_ACQUISITION = 1,        // Aquisição de item
    NPC_INTERACTION = 2,         // Interação com NPC
    LOCATION_ARRIVAL = 3,        // Chegada em localização
    PLAYER_LEVEL = 4,            // Nível do jogador
    SKILL_USE = 5,               // Uso de habilidade
    TIME_ELAPSED = 6,            // Tempo passado
    RESOURCE_GATHERED = 7,       // Recurso coletado
    PLAYER_DEATH = 8,            // Morte do jogador
    PLAYER_RESURRECTION = 9,     // Ressurreição do jogador
    ITEM_USE = 10,               // Uso de item
    EVENT_PARTICIPATION = 11,    // Participação em evento
    DUNGEON_COMPLETION = 12,     // Conclusão de masmorra
    CUSTOM = 13,                 // Personalizado
};

/**
 * @brief Tipo de resposta de diálogo
 */
enum class DialogueResponseType : BYTE {
    ACCEPT_QUEST = 0,            // Aceitar quest
    DECLINE_QUEST = 1,           // Recusar quest
    CONTINUE_DIALOGUE = 2,       // Continuar diálogo
    END_DIALOGUE = 3,            // Encerrar diálogo
    GIVE_ITEM = 4,               // Dar item
    TAKE_ITEM = 5,               // Pegar item
    SHOW_SHOP = 6,               // Mostrar loja
    TELEPORT_PLAYER = 7,         // Teleportar jogador
    TRIGGER_EVENT = 8,           // Acionar evento
    CUSTOM = 9,                  // Personalizado
};

/**
 * @brief Modo de compartilhamento de quest
 */
enum class QuestSharingMode : BYTE {
    NOT_SHAREABLE = 0,           // Não compartilhável
    PARTY_ONLY = 1,              // Apenas grupo
    GUILD_ONLY = 2,              // Apenas guilda
    FRIENDS_ONLY = 3,            // Apenas amigos
    PUBLIC = 4,                  // Público
    CUSTOM = 5,                  // Personalizado
};

/**
 * @brief Modo de conclusão de quest
 */
enum class QuestCompletionMode : BYTE {
    INDIVIDUAL = 0,              // Individual
    PARTY_ANY = 1,               // Qualquer membro do grupo
    PARTY_ALL = 2,               // Todos os membros do grupo
    GUILD_ANY = 3,               // Qualquer membro da guilda
    GUILD_ALL = 4,               // Todos os membros da guilda
    CUSTOM = 5,                  // Personalizado
};

/**
 * @brief Objetivo de quest
 */
struct QuestObjective {
    DWORD objectiveID;                // ID do objetivo
    DWORD questID;                    // ID da quest
    QuestObjectiveType type;          // Tipo
    DWORD targetID;                   // ID do alvo
    std::string targetName;            // Nome do alvo
    DWORD currentCount;               // Contagem atual
    DWORD requiredCount;              // Contagem necessária
    bool completed;                    // Completo
    DWORD zoneID;                     // ID da zona
    std::string description;           // Descrição
    bool optional;                     // Opcional
    bool hidden;                       // Escondido
    DWORD timeLimit;                  // Limite de tempo (segundos)
    std::string script;                // Script
    
    QuestObjective()
        : objectiveID(0)
        , questID(0)
        , type(QuestObjectiveType::KILL_MONSTER)
        , targetID(0)
        , targetName("")
        , currentCount(0)
        , requiredCount(0)
        , completed(false)
        , zoneID(0)
        , description("")
        , optional(false)
        , hidden(false)
        , timeLimit(0)
        , script("")
    {
    }
};

/**
 * @brief Recompensa de quest
 */
struct QuestReward {
    DWORD rewardID;                   // ID da recompensa
    DWORD questID;                    // ID da quest
    QuestRewardType type;             // Tipo
    DWORD itemID;                     // ID do item
    DWORD amount;                     // Quantidade
    bool isChoice;                     // Se é escolha
    BYTE quality;                     // Qualidade
    std::string description;           // Descrição
    DWORD currencyType;               // Tipo de moeda
    bool delivered;                    // Entregue
    
    QuestReward()
        : rewardID(0)
        , questID(0)
        , type(QuestRewardType::EXPERIENCE)
        , itemID(0)
        , amount(0)
        , isChoice(false)
        , quality(0)
        , description("")
        , currencyType(0)
        , delivered(false)
    {
    }
};

/**
 * @brief Requisito de quest
 */
struct QuestRequirement {
    DWORD requirementID;              // ID do requisito
    DWORD questID;                    // ID da quest
    QuestRequirementType type;        // Tipo
    DWORD targetID;                   // ID do alvo
    DWORD value;                      // Valor
    bool inverse;                      // Inverso
    std::string description;           // Descrição
    bool optional;                     // Opcional
    
    QuestRequirement()
        : requirementID(0)
        , questID(0)
        , type(QuestRequirementType::LEVEL)
        , targetID(0)
        , value(0)
        , inverse(false)
        , description("")
        , optional(false)
    {
    }
};

/**
 * @brief Etapa de quest
 */
struct QuestStep {
    DWORD stepID;                     // ID da etapa
    DWORD questID;                    // ID da quest
    DWORD orderIndex;                 // Índice de ordem
    std::string title;                 // Título
    std::string description;           // Descrição
    std::vector<DWORD> objectiveIDs;   // IDs de objetivos
    bool completed;                    // Completa
    std::string completionText;        // Texto de conclusão
    DWORD npcID;                      // ID do NPC
    DWORD zoneID;                     // ID da zona
    
    QuestStep()
        : stepID(0)
        , questID(0)
        , orderIndex(0)
        , title("")
        , description("")
        , completed(false)
        , completionText("")
        , npcID(0)
        , zoneID(0)
    {
    }
};

/**
 * @brief Diálogo de quest
 */
struct QuestDialogue {
    DWORD dialogueID;                 // ID do diálogo
    DWORD questID;                    // ID da quest
    DWORD stepID;                     // ID da etapa
    DWORD npcID;                      // ID do NPC
    std::string text;                  // Texto
    std::vector<std::pair<std::string, DialogueResponseType>> responses; // Respostas
    DWORD nextDialogueID;             // ID do próximo diálogo
    bool repeatable;                   // Repetível
    std::string voiceOverFile;         // Arquivo de voz
    bool autoAdvance;                  // Avança automaticamente
    
    QuestDialogue()
        : dialogueID(0)
        , questID(0)
        , stepID(0)
        , npcID(0)
        , text("")
        , nextDialogueID(0)
        , repeatable(false)
        , voiceOverFile("")
        , autoAdvance(false)
    {
    }
};

/**
 * @brief Gatilho de quest
 */
struct QuestTrigger {
    DWORD triggerID;                  // ID do gatilho
    QuestTriggerType type;            // Tipo
    DWORD questID;                    // ID da quest
    DWORD targetID;                   // ID do alvo
    DWORD zoneID;                     // ID da zona
    std::vector<std::pair<std::string, std::string>> conditions; // Condições
    std::string action;                // Ação
    bool oneTime;                      // Uma vez
    bool active;                       // Ativo
    
    QuestTrigger()
        : triggerID(0)
        , type(QuestTriggerType::MONSTER_KILL)
        , questID(0)
        , targetID(0)
        , zoneID(0)
        , action("")
        , oneTime(true)
        , active(true)
    {
    }
};

/**
 * @brief Progresso de quest do jogador
 */
struct PlayerQuestProgress {
    DWORD progressID;                 // ID do progresso
    DWORD playerID;                   // ID do jogador
    DWORD questID;                    // ID da quest
    QuestStatus status;               // Status
    DWORD currentStepID;              // ID da etapa atual
    time_t startTime;                  // Hora de início
    time_t completionTime;             // Hora de conclusão
    time_t expiryTime;                 // Hora de expiração
    DWORD failCount;                  // Contagem de falhas
    DWORD completionCount;            // Contagem de conclusões
    time_t lastResetTime;              // Última hora de reset
    std::map<DWORD, DWORD> objectiveProgress; // Progresso dos objetivos
    std::vector<DWORD> chosenRewards;  // Recompensas escolhidas
    
    PlayerQuestProgress()
        : progressID(0)
        , playerID(0)
        , questID(0)
        , status(QuestStatus::UNAVAILABLE)
        , currentStepID(0)
        , startTime(0)
        , completionTime(0)
        , expiryTime(0)
        , failCount(0)
        , completionCount(0)
        , lastResetTime(0)
    {
    }
};

/**
 * @brief Grupo de quests
 */
struct QuestGroup {
    DWORD groupID;                    // ID do grupo
    std::string name;                  // Nome
    std::string description;           // Descrição
    QuestCategory category;           // Categoria
    std::vector<DWORD> questIDs;       // IDs de quests
    bool isSequential;                 // Se é sequencial
    std::string icon;                  // Ícone
    bool active;                       // Ativo
    
    QuestGroup()
        : groupID(0)
        , name("")
        , description("")
        , category(QuestCategory::MAIN_STORY)
        , isSequential(false)
        , icon("")
        , active(true)
    {
    }
};

/**
 * @brief Cadeia de quests
 */
struct QuestChain {
    DWORD chainID;                    // ID da cadeia
    std::string name;                  // Nome
    std::string description;           // Descrição
    std::vector<DWORD> questIDs;       // IDs de quests
    bool repeatable;                   // Repetível
    DWORD resetInterval;              // Intervalo de reset (segundos)
    std::string rewardDescription;     // Descrição da recompensa
    bool active;                       // Ativa
    
    QuestChain()
        : chainID(0)
        , name("")
        , description("")
        , repeatable(false)
        , resetInterval(0)
        , rewardDescription("")
        , active(true)
    {
    }
};

/**
 * @brief Notificação de quest
 */
struct QuestNotification {
    DWORD notificationID;             // ID da notificação
    DWORD playerID;                   // ID do jogador
    DWORD questID;                    // ID da quest
    std::string message;               // Mensagem
    time_t timestamp;                  // Timestamp
    bool read;                         // Lida
    QuestObjectiveType objectiveType; // Tipo de objetivo
    DWORD objectiveID;                // ID do objetivo
    
    QuestNotification()
        : notificationID(0)
        , playerID(0)
        , questID(0)
        , message("")
        , timestamp(0)
        , read(false)
        , objectiveType(QuestObjectiveType::KILL_MONSTER)
        , objectiveID(0)
    {
    }
};

/**
 * @brief Dados completos de uma quest
 */
struct Quest {
    DWORD questID;                    // ID da quest
    std::string title;                 // Título
    std::string description;           // Descrição
    QuestCategory category;           // Categoria
    QuestDifficulty difficulty;       // Dificuldade
    DWORD levelMin;                   // Nível mínimo
    DWORD levelMax;                   // Nível máximo
    bool repeatable;                   // Repetível
    DWORD repeatCooldown;             // Cooldown de repetição (segundos)
    DWORD timeLimit;                  // Limite de tempo (segundos)
    DWORD startNpcID;                 // ID do NPC inicial
    DWORD endNpcID;                   // ID do NPC final
    DWORD startZoneID;                // ID da zona inicial
    std::vector<DWORD> prerequisiteQuestIDs; // IDs de quests pré-requisitas
    std::string startText;             // Texto inicial
    std::string endText;               // Texto final
    DWORD failureConditions;          // Condições de falha
    QuestSharingMode sharingMode;     // Modo de compartilhamento
    QuestCompletionMode completionMode; // Modo de conclusão
    bool autoAccept;                   // Auto-aceitar
    bool autoComplete;                 // Auto-completar
    std::string scriptFile;            // Arquivo de script
    DWORD groupID;                    // ID do grupo
    DWORD chainID;                    // ID da cadeia
    bool active;                       // Ativa
    bool hidden;                       // Escondida
    DWORD faction;                    // Facção
    
    Quest()
        : questID(0)
        , title("")
        , description("")
        , category(QuestCategory::MAIN_STORY)
        , difficulty(QuestDifficulty::NORMAL)
        , levelMin(1)
        , levelMax(0)
        , repeatable(false)
        , repeatCooldown(0)
        , timeLimit(0)
        , startNpcID(0)
        , endNpcID(0)
        , startZoneID(0)
        , startText("")
        , endText("")
        , failureConditions(0)
        , sharingMode(QuestSharingMode::NOT_SHAREABLE)
        , completionMode(QuestCompletionMode::INDIVIDUAL)
        , autoAccept(false)
        , autoComplete(false)
        , scriptFile("")
        , groupID(0)
        , chainID(0)
        , active(true)
        , hidden(false)
        , faction(0)
    {
    }
};

/**
 * @brief Evento de quest
 */
struct QuestEvent {
    DWORD eventID;                    // ID do evento
    DWORD questID;                    // ID da quest
    DWORD playerID;                   // ID do jogador
    QuestStatus oldStatus;            // Status antigo
    QuestStatus newStatus;            // Status novo
    time_t timestamp;                  // Timestamp
    std::string details;               // Detalhes
    
    QuestEvent()
        : eventID(0)
        , questID(0)
        , playerID(0)
        , oldStatus(QuestStatus::UNAVAILABLE)
        , newStatus(QuestStatus::UNAVAILABLE)
        , timestamp(0)
        , details("")
    {
    }
};

/**
 * @brief Tipos de dados serializáveis para o sistema de quest
 */
using QuestDataVariant = std::variant<
    Quest,
    QuestObjective,
    QuestReward,
    QuestRequirement,
    QuestStep,
    QuestDialogue,
    QuestTrigger,
    PlayerQuestProgress,
    QuestGroup,
    QuestChain,
    QuestNotification,
    QuestEvent
>;

/**
 * @brief Callback de evento de quest
 */
using QuestEventCallback = std::function<void(DWORD, DWORD, QuestStatus, QuestStatus, const std::string&)>;

/**
 * @brief Callback de validação de objetivo
 */
using QuestObjectiveValidationCallback = std::function<bool(DWORD, const QuestObjective&, DWORD)>;

/**
 * @brief Callback de entrega de recompensa
 */
using QuestRewardDeliveryCallback = std::function<bool(DWORD, const QuestReward&)>;

/**
 * @brief Callback de verificação de requisito
 */
using QuestRequirementCheckCallback = std::function<bool(DWORD, const QuestRequirement&)>;

/**
 * @brief Callback de processamento de gatilho
 */
using QuestTriggerProcessCallback = std::function<bool(DWORD, const QuestTrigger&, const std::string&)>;

} // namespace quest
} // namespace server
} // namespace wyd

#endif // QUESTTYPES_H

} // namespace wydbr
