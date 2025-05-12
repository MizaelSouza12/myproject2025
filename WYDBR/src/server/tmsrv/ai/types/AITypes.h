/**
 * AITypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/ai/types/AITypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef AITYPES_H
#define AITYPES_H

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
#include <ctime>
#include <deque>

#include "../../../../core/WYDTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file AITypes.h
 * @brief Definições de tipos para o sistema avançado de IA para NPCs e monstros
 * 
 * Este arquivo contém as definições de tipos para o sistema avançado de IA,
 * incluindo comportamentos, árvores de decisão, máquinas de estado, sistemas
 * de percepção, memória, aprendizado e coordenação de grupos.
 */

namespace wyd {
namespace server {
namespace ai {

// Constantes do sistema de IA
constexpr size_t MAX_AI_BEHAVIORS = 100;         // Máximo de comportamentos por entidade
constexpr size_t MAX_DECISION_NODES = 50;        // Máximo de nós de decisão por árvore
constexpr size_t MAX_BEHAVIOR_TREE_DEPTH = 10;   // Profundidade máxima da árvore de comportamento
constexpr size_t MAX_STATE_TRANSITIONS = 20;     // Máximo de transições por estado
constexpr size_t MAX_PERCEPTION_ENTRIES = 30;    // Máximo de entradas de percepção
constexpr size_t MAX_MEMORY_ENTRIES = 50;        // Máximo de entradas de memória
constexpr size_t MAX_PATH_POINTS = 100;          // Máximo de pontos de caminho
constexpr size_t MAX_GROUP_MEMBERS = 10;         // Máximo de membros por grupo
constexpr size_t MAX_TACTICAL_POSITIONS = 12;    // Máximo de posições táticas
constexpr size_t MAX_ABILITY_CONSIDERATIONS = 15; // Máximo de considerações por habilidade
constexpr size_t MAX_PATHING_NODES = 1000;       // Máximo de nós de caminho
constexpr size_t MAX_COMBAT_EVALUATIONS = 20;    // Máximo de avaliações de combate
constexpr size_t MAX_PERSONALITY_TRAITS = 10;    // Máximo de traços de personalidade
constexpr size_t MAX_SCHEDULES = 24;             // Máximo de agendas

/**
 * @brief Tipo de comportamento de IA
 */
enum class AIBehaviorType : BYTE {
    IDLE = 0,                 // Ocioso
    PATROL = 1,               // Patrulha
    FOLLOW = 2,               // Seguir
    ATTACK = 3,               // Atacar
    FLEE = 4,                 // Fugir
    DEFEND = 5,               // Defender
    HEAL = 6,                 // Curar
    ASSISTANCE = 7,           // Assistência
    GATHER = 8,               // Coletar
    TRANSPORT = 9,            // Transportar
    SEARCH = 10,              // Procurar
    AMBUSH = 11,              // Emboscar
    INVESTIGATE = 12,         // Investigar
    REST = 13,                // Descansar
    SOCIAL = 14,              // Social
    GUARD = 15,               // Guardar
    TRADE = 16,               // Comerciar
    CUSTOM = 17,              // Personalizado
};

/**
 * @brief Tipo de nó de decisão
 */
enum class DecisionNodeType : BYTE {
    SELECTOR = 0,             // Seletor
    SEQUENCE = 1,             // Sequência
    PARALLEL = 2,             // Paralelo
    DECORATOR = 3,            // Decorador
    CONDITION = 4,            // Condição
    ACTION = 5,               // Ação
    RANDOM = 6,               // Aleatório
    PRIORITY = 7,             // Prioridade
    INVERTER = 8,             // Inversor
    REPEATER = 9,             // Repetidor
    TIMER = 10,               // Temporizador
    COMPOSITE = 11,           // Composto
    CUSTOM = 12,              // Personalizado
};

/**
 * @brief Status de execução de nó
 */
enum class NodeExecutionStatus : BYTE {
    SUCCESS = 0,              // Sucesso
    FAILURE = 1,              // Falha
    RUNNING = 2,              // Executando
    INVALID = 3,              // Inválido
    CUSTOM = 4,               // Personalizado
};

/**
 * @brief Tipo de percepção
 */
enum class PerceptionType : BYTE {
    VISUAL = 0,               // Visual
    AUDITORY = 1,             // Auditiva
    PROXIMITY = 2,            // Proximidade
    DAMAGE = 3,               // Dano
    ABILITY = 4,              // Habilidade
    COMMUNICATION = 5,        // Comunicação
    ENVIRONMENTAL = 6,        // Ambiental
    TEMPORAL = 7,             // Temporal
    CUSTOM = 8,               // Personalizada
};

/**
 * @brief Tipo de entrada de memória
 */
enum class MemoryEntryType : BYTE {
    ENTITY = 0,               // Entidade
    LOCATION = 1,             // Localização
    DAMAGE = 2,               // Dano
    INTERACTION = 3,          // Interação
    PATH = 4,                 // Caminho
    PREFERENCE = 5,           // Preferência
    THREAT = 6,               // Ameaça
    ALLY = 7,                 // Aliado
    RESOURCE = 8,             // Recurso
    EVENT = 9,                // Evento
    CUSTOM = 10,              // Personalizado
};

/**
 * @brief Tipo de estratégia de grupo
 */
enum class GroupStrategyType : BYTE {
    NONE = 0,                 // Nenhuma
    FORMATION = 1,            // Formação
    SURROUND = 2,             // Cercar
    COORDINATE_ATTACK = 3,    // Ataque coordenado
    PINCER = 4,               // Pinça
    DEFENSIVE = 5,            // Defensiva
    SCATTER = 6,              // Dispersar
    DECOY = 7,                // Chamariz
    AMBUSH = 8,               // Emboscar
    OVERRUN = 9,              // Sobrepujar
    LEAPFROG = 10,            // Pulo de sapo
    CUSTOM = 11,              // Personalizada
};

/**
 * @brief Tipo de tática de combate
 */
enum class CombatTacticType : BYTE {
    AGGRESSIVE = 0,           // Agressiva
    DEFENSIVE = 1,            // Defensiva
    SUPPORTIVE = 2,           // Suporte
    KITING = 3,               // Kiting
    BURST = 4,                // Burst
    SUSTAINED = 5,            // Sustentada
    GUERRILLA = 6,            // Guerrilha
    BERSERKER = 7,            // Berserker
    RANGED = 8,               // À distância
    MELEE = 9,                // Corpo a corpo
    EVASIVE = 10,             // Evasiva
    CUSTOM = 11,              // Personalizada
};

/**
 * @brief Tipo de movimento de AI
 */
enum class AIMovementType : BYTE {
    WALK = 0,                 // Andar
    RUN = 1,                  // Correr
    TELEPORT = 2,             // Teleportar
    FLY = 3,                  // Voar
    SWIM = 4,                 // Nadar
    BURROW = 5,               // Escavar
    JUMP = 6,                 // Pular
    DASH = 7,                 // Avançar rapidamente
    STEALTH = 8,              // Furtividade
    FOLLOW_PATH = 9,          // Seguir caminho
    PURSUE = 10,              // Perseguir
    STRAFE = 11,              // Deslocar lateralmente
    CUSTOM = 12,              // Personalizado
};

/**
 * @brief Tipo de reação AI
 */
enum class AIReactionType : BYTE {
    ATTACK = 0,               // Atacar
    FLEE = 1,                 // Fugir
    CALL_FOR_HELP = 2,        // Pedir ajuda
    INVESTIGATE = 3,          // Investigar
    IGNORE = 4,               // Ignorar
    ALERT = 5,                // Alertar
    HIDE = 6,                 // Esconder
    SURRENDER = 7,            // Render-se
    AGGRESSIVE = 8,           // Agressivo
    DEFENSIVE = 9,            // Defensivo
    CURIOUS = 10,             // Curioso
    FRIENDLY = 11,            // Amigável
    CUSTOM = 12,              // Personalizado
};

/**
 * @brief Tipo de personalidade AI
 */
enum class AIPersonalityType : BYTE {
    AGGRESSIVE = 0,           // Agressivo
    CAUTIOUS = 1,             // Cauteloso
    CURIOUS = 2,              // Curioso
    COWARDLY = 3,             // Covarde
    SUPPORTIVE = 4,           // Solidário
    PROTECTIVE = 5,           // Protetor
    VENGEFUL = 6,             // Vingativo
    GREEDY = 7,               // Ganancioso
    HONORABLE = 8,            // Honrado
    UNPREDICTABLE = 9,        // Imprevisível
    LOYAL = 10,               // Leal
    CUSTOM = 11,              // Personalizado
};

/**
 * @brief Status de ameaça
 */
enum class ThreatStatus : BYTE {
    NONE = 0,                 // Nenhum
    LOW = 1,                  // Baixo
    MODERATE = 2,             // Moderado
    HIGH = 3,                 // Alto
    CRITICAL = 4,             // Crítico
    CUSTOM = 5,               // Personalizado
};

/**
 * @brief Comportamento de IA
 */
struct AIBehavior {
    DWORD behaviorID;          // ID do comportamento
    DWORD entityID;            // ID da entidade
    AIBehaviorType type;        // Tipo
    float priority;             // Prioridade
    bool enabled;               // Habilitado
    DWORD scriptID;            // ID do script
    std::map<std::string, std::string> parameters; // Parâmetros
    float cooldown;             // Cooldown
    time_t lastExecutionTime;    // Hora da última execução
    bool interruptible;          // Interruptível
    
    AIBehavior()
        : behaviorID(0)
        , entityID(0)
        , type(AIBehaviorType::IDLE)
        , priority(0.0f)
        , enabled(true)
        , scriptID(0)
        , cooldown(0.0f)
        , lastExecutionTime(0)
        , interruptible(true)
    {
    }
};

/**
 * @brief Nó de decisão
 */
struct DecisionNode {
    DWORD nodeID;              // ID do nó
    DWORD treeID;              // ID da árvore
    DecisionNodeType type;      // Tipo
    std::string name;           // Nome
    DWORD parentID;            // ID do pai
    std::vector<DWORD> childIDs; // IDs dos filhos
    std::string conditionExpression; // Expressão de condição
    std::string actionExpression; // Expressão de ação
    float priority;             // Prioridade
    bool isEntryPoint;          // Se é ponto de entrada
    NodeExecutionStatus lastStatus; // Último status
    
    DecisionNode()
        : nodeID(0)
        , treeID(0)
        , type(DecisionNodeType::ACTION)
        , name("")
        , parentID(0)
        , conditionExpression("")
        , actionExpression("")
        , priority(0.0f)
        , isEntryPoint(false)
        , lastStatus(NodeExecutionStatus::INVALID)
    {
    }
};

/**
 * @brief Árvore de comportamento
 */
struct BehaviorTree {
    DWORD treeID;              // ID da árvore
    DWORD entityID;            // ID da entidade
    std::string name;           // Nome
    DWORD rootNodeID;          // ID do nó raiz
    bool enabled;               // Habilitada
    DWORD currentNodeID;       // ID do nó atual
    time_t lastUpdateTime;       // Hora da última atualização
    time_t creationTime;         // Hora de criação
    std::string description;     // Descrição
    
    BehaviorTree()
        : treeID(0)
        , entityID(0)
        , name("")
        , rootNodeID(0)
        , enabled(true)
        , currentNodeID(0)
        , lastUpdateTime(0)
        , creationTime(0)
        , description("")
    {
    }
};

/**
 * @brief Estado de máquina de estado
 */
struct AIState {
    DWORD stateID;             // ID do estado
    DWORD fsmID;               // ID da FSM
    std::string name;           // Nome
    AIBehaviorType behavior;    // Comportamento
    std::string entryAction;    // Ação de entrada
    std::string exitAction;     // Ação de saída
    std::string updateAction;   // Ação de atualização
    bool isInitialState;        // Se é estado inicial
    std::vector<std::pair<DWORD, std::string>> transitions; // Transições (stateID, condição)
    time_t entryTime;           // Hora de entrada
    time_t exitTime;            // Hora de saída
    
    AIState()
        : stateID(0)
        , fsmID(0)
        , name("")
        , behavior(AIBehaviorType::IDLE)
        , entryAction("")
        , exitAction("")
        , updateAction("")
        , isInitialState(false)
        , entryTime(0)
        , exitTime(0)
    {
    }
};

/**
 * @brief Máquina de estado finito
 */
struct StateMachine {
    DWORD fsmID;               // ID da FSM
    DWORD entityID;            // ID da entidade
    std::string name;           // Nome
    DWORD currentStateID;      // ID do estado atual
    DWORD previousStateID;     // ID do estado anterior
    bool enabled;               // Habilitada
    time_t lastUpdateTime;       // Hora da última atualização
    time_t creationTime;         // Hora de criação
    DWORD globalTransitionCount; // Contagem de transições globais
    
    StateMachine()
        : fsmID(0)
        , entityID(0)
        , name("")
        , currentStateID(0)
        , previousStateID(0)
        , enabled(true)
        , lastUpdateTime(0)
        , creationTime(0)
        , globalTransitionCount(0)
    {
    }
};

/**
 * @brief Entrada de percepção
 */
struct PerceptionEntry {
    DWORD perceptionID;        // ID da percepção
    DWORD entityID;            // ID da entidade
    PerceptionType type;        // Tipo
    DWORD targetID;            // ID do alvo
    float intensity;            // Intensidade
    time_t timestamp;            // Timestamp
    float posX;                 // Posição X
    float posY;                 // Posição Y
    float posZ;                 // Posição Z
    time_t expirationTime;       // Hora de expiração
    bool verified;              // Verificada
    
    PerceptionEntry()
        : perceptionID(0)
        , entityID(0)
        , type(PerceptionType::VISUAL)
        , targetID(0)
        , intensity(0.0f)
        , timestamp(0)
        , posX(0.0f)
        , posY(0.0f)
        , posZ(0.0f)
        , expirationTime(0)
        , verified(false)
    {
    }
};

/**
 * @brief Entrada de memória
 */
struct MemoryEntry {
    DWORD memoryID;            // ID da memória
    DWORD entityID;            // ID da entidade
    MemoryEntryType type;       // Tipo
    DWORD targetID;            // ID do alvo
    std::string data;           // Dados
    time_t creationTime;         // Hora de criação
    time_t lastUpdateTime;       // Hora da última atualização
    float importance;           // Importância
    time_t expirationTime;       // Hora de expiração
    bool persistent;            // Persistente
    
    MemoryEntry()
        : memoryID(0)
        , entityID(0)
        , type(MemoryEntryType::ENTITY)
        , targetID(0)
        , data("")
        , creationTime(0)
        , lastUpdateTime(0)
        , importance(0.0f)
        , expirationTime(0)
        , persistent(false)
    {
    }
};

/**
 * @brief Ponto de caminho para pathfinding
 */
struct PathPoint {
    DWORD pointID;             // ID do ponto
    DWORD pathID;              // ID do caminho
    float posX;                 // Posição X
    float posY;                 // Posição Y
    float posZ;                 // Posição Z
    DWORD order;               // Ordem
    bool reached;               // Alcançado
    float speedFactor;          // Fator de velocidade
    AIMovementType movementType; // Tipo de movimento
    std::string action;         // Ação
    
    PathPoint()
        : pointID(0)
        , pathID(0)
        , posX(0.0f)
        , posY(0.0f)
        , posZ(0.0f)
        , order(0)
        , reached(false)
        , speedFactor(1.0f)
        , movementType(AIMovementType::WALK)
        , action("")
    {
    }
};

/**
 * @brief Caminho
 */
struct AIPath {
    DWORD pathID;              // ID do caminho
    DWORD entityID;            // ID da entidade
    std::string name;           // Nome
    bool isComplete;            // Se está completo
    time_t creationTime;         // Hora de criação
    float totalDistance;        // Distância total
    DWORD currentPointIndex;   // Índice do ponto atual
    DWORD targetEntityID;      // ID da entidade alvo
    AIMovementType defaultMovement; // Movimento padrão
    
    AIPath()
        : pathID(0)
        , entityID(0)
        , name("")
        , isComplete(false)
        , creationTime(0)
        , totalDistance(0.0f)
        , currentPointIndex(0)
        , targetEntityID(0)
        , defaultMovement(AIMovementType::WALK)
    {
    }
};

/**
 * @brief Grupo de IA
 */
struct AIGroup {
    DWORD groupID;             // ID do grupo
    std::string name;           // Nome
    DWORD leaderID;            // ID do líder
    GroupStrategyType strategy; // Estratégia
    std::vector<DWORD> memberIDs; // IDs dos membros
    float formationSpacing;     // Espaçamento de formação
    std::string formationPattern; // Padrão de formação
    time_t formationTime;        // Hora de formação
    DWORD assignedZoneID;      // ID da zona atribuída
    bool active;                // Ativo
    
    AIGroup()
        : groupID(0)
        , name("")
        , leaderID(0)
        , strategy(GroupStrategyType::NONE)
        , formationSpacing(0.0f)
        , formationPattern("")
        , formationTime(0)
        , assignedZoneID(0)
        , active(true)
    {
    }
};

/**
 * @brief Posição tática
 */
struct TacticalPosition {
    DWORD positionID;          // ID da posição
    DWORD groupID;             // ID do grupo
    float posX;                 // Posição X
    float posY;                 // Posição Y
    float posZ;                 // Posição Z
    float priority;             // Prioridade
    CombatTacticType tacticType; // Tipo de tática
    DWORD assignedEntityID;    // ID da entidade atribuída
    bool occupied;              // Ocupada
    DWORD coverValue;          // Valor de cobertura
    
    TacticalPosition()
        : positionID(0)
        , groupID(0)
        , posX(0.0f)
        , posY(0.0f)
        , posZ(0.0f)
        , priority(0.0f)
        , tacticType(CombatTacticType::AGGRESSIVE)
        , assignedEntityID(0)
        , occupied(false)
        , coverValue(0)
    {
    }
};

/**
 * @brief Avaliação de habilidade
 */
struct AbilityEvaluation {
    DWORD evaluationID;        // ID da avaliação
    DWORD entityID;            // ID da entidade
    DWORD abilityID;           // ID da habilidade
    float score;                // Pontuação
    DWORD targetID;            // ID do alvo
    time_t evaluationTime;       // Hora da avaliação
    std::map<std::string, float> considerations; // Considerações
    bool selected;              // Selecionada
    
    AbilityEvaluation()
        : evaluationID(0)
        , entityID(0)
        , abilityID(0)
        , score(0.0f)
        , targetID(0)
        , evaluationTime(0)
        , selected(false)
    {
    }
};

/**
 * @brief Tabela de ameaças
 */
struct ThreatTable {
    DWORD tableID;             // ID da tabela
    DWORD entityID;            // ID da entidade
    std::map<DWORD, float> threats; // Ameaças (entityID, valor)
    DWORD highestThreatID;     // ID da maior ameaça
    time_t lastUpdateTime;       // Hora da última atualização
    DWORD aggroRadius;         // Raio de aggro
    float threatDecayRate;      // Taxa de decaimento de ameaça
    float threatTransferRate;   // Taxa de transferência de ameaça
    
    ThreatTable()
        : tableID(0)
        , entityID(0)
        , highestThreatID(0)
        , lastUpdateTime(0)
        , aggroRadius(0)
        , threatDecayRate(0.0f)
        , threatTransferRate(0.0f)
    {
    }
};

/**
 * @brief Avaliação de combate
 */
struct CombatEvaluation {
    DWORD evaluationID;        // ID da avaliação
    DWORD entityID;            // ID da entidade
    DWORD targetID;            // ID do alvo
    float threatRatio;          // Relação de ameaça
    float healthRatio;          // Relação de vida
    float damagePerSecond;      // Dano por segundo
    float survivalTime;         // Tempo de sobrevivência
    float victoryChance;        // Chance de vitória
    CombatTacticType suggestedTactic; // Tática sugerida
    time_t evaluationTime;       // Hora da avaliação
    
    CombatEvaluation()
        : evaluationID(0)
        , entityID(0)
        , targetID(0)
        , threatRatio(0.0f)
        , healthRatio(0.0f)
        , damagePerSecond(0.0f)
        , survivalTime(0.0f)
        , victoryChance(0.0f)
        , suggestedTactic(CombatTacticType::DEFENSIVE)
        , evaluationTime(0)
    {
    }
};

/**
 * @brief Traço de personalidade
 */
struct PersonalityTrait {
    DWORD traitID;             // ID do traço
    AIPersonalityType type;     // Tipo
    float value;                // Valor
    std::string description;     // Descrição
    std::map<std::string, float> modifiers; // Modificadores
    bool active;                // Ativo
    
    PersonalityTrait()
        : traitID(0)
        , type(AIPersonalityType::CAUTIOUS)
        , value(0.0f)
        , description("")
        , active(true)
    {
    }
};

/**
 * @brief Perfil de personalidade
 */
struct PersonalityProfile {
    DWORD profileID;           // ID do perfil
    DWORD entityID;            // ID da entidade
    std::string name;           // Nome
    std::vector<PersonalityTrait> traits; // Traços
    AIReactionType defaultReaction; // Reação padrão
    float adaptability;         // Adaptabilidade
    float curiosity;            // Curiosidade
    float aggressiveness;       // Agressividade
    float cautiousness;         // Cautela
    float sociability;          // Sociabilidade
    
    PersonalityProfile()
        : profileID(0)
        , entityID(0)
        , name("")
        , defaultReaction(AIReactionType::INVESTIGATE)
        , adaptability(0.5f)
        , curiosity(0.5f)
        , aggressiveness(0.5f)
        , cautiousness(0.5f)
        , sociability(0.5f)
    {
    }
};

/**
 * @brief Item de agenda
 */
struct ScheduleItem {
    DWORD scheduleItemID;      // ID do item de agenda
    DWORD scheduleID;          // ID da agenda
    BYTE hour;                 // Hora
    BYTE minute;               // Minuto
    DWORD duration;            // Duração (minutos)
    AIBehaviorType behavior;    // Comportamento
    DWORD locationID;          // ID da localização
    float posX;                 // Posição X
    float posY;                 // Posição Y
    float posZ;                 // Posição Z
    std::string description;     // Descrição
    bool active;                // Ativo
    
    ScheduleItem()
        : scheduleItemID(0)
        , scheduleID(0)
        , hour(0)
        , minute(0)
        , duration(0)
        , behavior(AIBehaviorType::IDLE)
        , locationID(0)
        , posX(0.0f)
        , posY(0.0f)
        , posZ(0.0f)
        , description("")
        , active(true)
    {
    }
};

/**
 * @brief Agenda
 */
struct Schedule {
    DWORD scheduleID;          // ID da agenda
    DWORD entityID;            // ID da entidade
    std::string name;           // Nome
    std::vector<ScheduleItem> items; // Itens
    bool repeating;             // Repetitiva
    BYTE dayMask;              // Máscara de dia
    bool active;                // Ativa
    time_t lastUpdateTime;       // Hora da última atualização
    DWORD currentItemID;       // ID do item atual
    
    Schedule()
        : scheduleID(0)
        , entityID(0)
        , name("")
        , repeating(true)
        , dayMask(0xFF) // Todos os dias
        , active(true)
        , lastUpdateTime(0)
        , currentItemID(0)
    {
    }
};

/**
 * @brief Controlador de IA completo
 */
struct AIController {
    DWORD controllerID;        // ID do controlador
    DWORD entityID;            // ID da entidade
    std::string name;           // Nome
    bool enabled;               // Habilitado
    DWORD behaviorTreeID;      // ID da árvore de comportamento
    DWORD stateMachineID;      // ID da máquina de estado
    DWORD currentBehaviorID;   // ID do comportamento atual
    DWORD groupID;             // ID do grupo
    DWORD threatTableID;       // ID da tabela de ameaças
    DWORD personalityProfileID; // ID do perfil de personalidade
    DWORD scheduleID;          // ID da agenda
    float perceptionRange;      // Alcance de percepção
    float aggroRange;           // Alcance de aggro
    float assistRange;          // Alcance de assistência
    time_t lastUpdateTime;       // Hora da última atualização
    time_t creationTime;         // Hora de criação
    
    AIController()
        : controllerID(0)
        , entityID(0)
        , name("")
        , enabled(true)
        , behaviorTreeID(0)
        , stateMachineID(0)
        , currentBehaviorID(0)
        , groupID(0)
        , threatTableID(0)
        , personalityProfileID(0)
        , scheduleID(0)
        , perceptionRange(0.0f)
        , aggroRange(0.0f)
        , assistRange(0.0f)
        , lastUpdateTime(0)
        , creationTime(0)
    {
    }
};

/**
 * @brief Tipos de dados serializáveis para o sistema de AI
 */
using AIDataVariant = std::variant<
    AIBehavior,
    DecisionNode,
    BehaviorTree,
    AIState,
    StateMachine,
    PerceptionEntry,
    MemoryEntry,
    PathPoint,
    AIPath,
    AIGroup,
    TacticalPosition,
    AbilityEvaluation,
    ThreatTable,
    CombatEvaluation,
    PersonalityTrait,
    PersonalityProfile,
    ScheduleItem,
    Schedule,
    AIController
>;

/**
 * @brief Callback de atualização de AI
 */
using AIUpdateCallback = std::function<void(DWORD, DWORD, AIBehaviorType)>;

/**
 * @brief Callback de percepção
 */
using PerceptionCallback = std::function<void(DWORD, DWORD, PerceptionType, float)>;

/**
 * @brief Callback de grupo AI
 */
using AIGroupCallback = std::function<void(DWORD, DWORD, GroupStrategyType)>;

/**
 * @brief Callback de ameaça
 */
using ThreatCallback = std::function<void(DWORD, DWORD, float, ThreatStatus)>;

/**
 * @brief Callback de transição de estado
 */
using StateTransitionCallback = std::function<void(DWORD, DWORD, DWORD)>;

} // namespace ai
} // namespace server
} // namespace wyd

#endif // AITYPES_H

} // namespace wydbr
