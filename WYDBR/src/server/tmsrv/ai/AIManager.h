/**
 * AIManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/ai/AIManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <set>
#include <variant>
#include <optional>
#include <queue>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <deque>

#include "../../../core/WYDTypes.h"
#include "./types/AITypes.h"
#include "../player/CPlayer.h"
#include "../kernel/persistence/PersistenceManager.h"

namespace wydbr {


/**
 * @file AIManager.h
 * @brief Sistema avançado de gerenciamento de IA para NPCs e monstros
 * 
 * Este arquivo contém a implementação do sistema de gerenciamento de IA,
 * que suporta comportamentos complexos, árvores de decisão, máquinas de estado,
 * sistemas de percepção, memória, aprendizado e coordenação de grupos.
 */

namespace wyd {
namespace server {
namespace ai {

/**
 * @brief Configuração do gerenciador de IA
 */
struct AIManagerConfig {
    bool enableAI;                   // Habilitar IA
    bool asyncProcessing;            // Processamento assíncrono
    DWORD updateInterval;            // Intervalo de atualização (ms)
    DWORD memoryDuration;            // Duração da memória (ms)
    DWORD maxEntitiesPerProcessor;   // Máximo de entidades por processador
    DWORD maxGroupSize;              // Tamanho máximo de grupo
    bool enableGroupBehavior;        // Habilitar comportamento de grupo
    bool enablePerception;           // Habilitar percepção
    bool enableLearning;             // Habilitar aprendizado
    bool enablePrediction;           // Habilitar predição
    float defaultAggroRange;         // Alcance de aggro padrão
    float defaultPerceptionRange;    // Alcance de percepção padrão
    float defaultAssistRange;        // Alcance de assistência padrão
    DWORD pathfindingTimeout;        // Timeout de pathfinding (ms)
    bool useSpatialPartitioning;     // Usar particionamento espacial
    float threatDecayRate;           // Taxa de decaimento de ameaça
    bool enableTelemetry;            // Habilitar telemetria
    DWORD behaviorTreeMaxDepth;      // Profundidade máxima da árvore de comportamento
    DWORD maxBehaviorsPerEntity;     // Máximo de comportamentos por entidade
    DWORD battlefieldGridSize;       // Tamanho da grade de campo de batalha
    
    AIManagerConfig()
        : enableAI(true)
        , asyncProcessing(true)
        , updateInterval(100)
        , memoryDuration(300000)
        , maxEntitiesPerProcessor(1000)
        , maxGroupSize(MAX_GROUP_MEMBERS)
        , enableGroupBehavior(true)
        , enablePerception(true)
        , enableLearning(true)
        , enablePrediction(true)
        , defaultAggroRange(15.0f)
        , defaultPerceptionRange(20.0f)
        , defaultAssistRange(10.0f)
        , pathfindingTimeout(2000)
        , useSpatialPartitioning(true)
        , threatDecayRate(0.1f)
        , enableTelemetry(true)
        , behaviorTreeMaxDepth(10)
        , maxBehaviorsPerEntity(10)
        , battlefieldGridSize(25)
    {
    }
};

/**
 * @brief Estatísticas do gerenciador de IA
 */
struct AIManagerStats {
    DWORD totalEntities;             // Total de entidades
    DWORD activeEntities;            // Entidades ativas
    float averageUpdateTime;         // Tempo médio de atualização (ms)
    DWORD totalBehaviorTrees;        // Total de árvores de comportamento
    DWORD totalStateMachines;        // Total de máquinas de estado
    DWORD totalActiveGroups;         // Total de grupos ativos
    DWORD totalPerceptions;          // Total de percepções
    DWORD totalMemoryEntries;        // Total de entradas de memória
    DWORD totalPathsCalculated;      // Total de caminhos calculados
    DWORD totalTacticalPositions;    // Total de posições táticas
    
    AIManagerStats()
        : totalEntities(0)
        , activeEntities(0)
        , averageUpdateTime(0.0f)
        , totalBehaviorTrees(0)
        , totalStateMachines(0)
        , totalActiveGroups(0)
        , totalPerceptions(0)
        , totalMemoryEntries(0)
        , totalPathsCalculated(0)
        , totalTacticalPositions(0)
    {
    }
};

/**
 * @brief Status de execução de comportamento
 */
struct BehaviorExecutionStatus {
    bool success;                     // Sucesso
    NodeExecutionStatus status;       // Status
    std::string message;              // Mensagem
    float executionTime;              // Tempo de execução (ms)
    DWORD behaviorID;                 // ID do comportamento
    
    BehaviorExecutionStatus()
        : success(false)
        , status(NodeExecutionStatus::FAILURE)
        , message("")
        , executionTime(0.0f)
        , behaviorID(0)
    {
    }
};

/**
 * @brief Status de percepção
 */
struct PerceptionResult {
    bool detected;                    // Detectado
    float intensity;                  // Intensidade
    PerceptionType type;              // Tipo
    DWORD targetID;                   // ID do alvo
    float distance;                   // Distância
    
    PerceptionResult()
        : detected(false)
        , intensity(0.0f)
        , type(PerceptionType::VISUAL)
        , targetID(0)
        , distance(0.0f)
    {
    }
};

/**
 * @brief Estado de ameaça
 */
struct ThreatState {
    DWORD entityID;                   // ID da entidade
    DWORD targetID;                   // ID do alvo
    float threatValue;                // Valor de ameaça
    ThreatStatus status;              // Status
    DWORD highestThreatID;            // ID da maior ameaça
    
    ThreatState()
        : entityID(0)
        , targetID(0)
        , threatValue(0.0f)
        , status(ThreatStatus::NONE)
        , highestThreatID(0)
    {
    }
};

/**
 * @brief Resultado de avaliação de habilidade
 */
struct AbilityEvaluationResult {
    DWORD abilityID;                  // ID da habilidade
    float score;                      // Pontuação
    DWORD targetID;                   // ID do alvo
    bool success;                     // Sucesso
    std::map<std::string, float> factors; // Fatores
    
    AbilityEvaluationResult()
        : abilityID(0)
        , score(0.0f)
        , targetID(0)
        , success(false)
    {
    }
};

/**
 * @brief Contexto de execução de IA
 */
struct AIExecutionContext {
    DWORD entityID;                   // ID da entidade
    float deltaTime;                  // Tempo delta
    std::map<std::string, std::string> variables; // Variáveis
    std::vector<DWORD> nearbyEntities; // Entidades próximas
    float posX;                       // Posição X
    float posY;                       // Posição Y
    float posZ;                       // Posição Z
    DWORD zoneID;                     // ID da zona
    
    AIExecutionContext()
        : entityID(0)
        , deltaTime(0.0f)
        , posX(0.0f)
        , posY(0.0f)
        , posZ(0.0f)
        , zoneID(0)
    {
    }
};

/**
 * @brief Gerenciador de IA
 */
/**
 * Classe WYDAIManager
 * 
 * Esta classe implementa a funcionalidade WYDAIManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AIManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static AIManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const AIManagerConfig& config = AIManagerConfig());
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Registra uma entidade para gerenciamento de IA
     * @param entityID ID da entidade
     * @param name Nome
     * @param controllerType Tipo de controlador
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param zoneID ID da zona
     * @return ID do controlador, ou 0 se falhar
     */
    DWORD RegisterEntity(DWORD entityID, const std::string& name, DWORD controllerType, 
                       float posX, float posY, float posZ, DWORD zoneID);
    
    /**
     * @brief Remove uma entidade do gerenciamento de IA
     * @param entityID ID da entidade
     * @return true se removido com sucesso
     */
    bool UnregisterEntity(DWORD entityID);
    
    /**
     * @brief Atualiza a posição de uma entidade
     * @param entityID ID da entidade
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param zoneID ID da zona
     * @return true se atualizado com sucesso
     */
    bool UpdateEntityPosition(DWORD entityID, float posX, float posY, float posZ, DWORD zoneID);
    
    /**
     * @brief Cria uma árvore de comportamento
     * @param entityID ID da entidade
     * @param name Nome
     * @param description Descrição
     * @return ID da árvore, ou 0 se falhar
     */
    DWORD CreateBehaviorTree(DWORD entityID, const std::string& name, const std::string& description);
    
    /**
     * @brief Adiciona um nó a uma árvore de comportamento
     * @param treeID ID da árvore
     * @param type Tipo
     * @param name Nome
     * @param parentID ID do pai (0 para raiz)
     * @param conditionExpression Expressão de condição
     * @param actionExpression Expressão de ação
     * @param priority Prioridade
     * @return ID do nó, ou 0 se falhar
     */
    DWORD AddNodeToBehaviorTree(DWORD treeID, DecisionNodeType type, const std::string& name, 
                              DWORD parentID, const std::string& conditionExpression, 
                              const std::string& actionExpression, float priority);
    
    /**
     * @brief Remove um nó de uma árvore de comportamento
     * @param nodeID ID do nó
     * @return true se removido com sucesso
     */
    bool RemoveNodeFromBehaviorTree(DWORD nodeID);
    
    /**
     * @brief Obtém uma árvore de comportamento
     * @param treeID ID da árvore
     * @return Ponteiro para a árvore, ou nullptr se não existir
     */
    const BehaviorTree* GetBehaviorTree(DWORD treeID) const;
    
    /**
     * @brief Obtém a árvore de comportamento de uma entidade
     * @param entityID ID da entidade
     * @return Ponteiro para a árvore, ou nullptr se não existir
     */
    const BehaviorTree* GetEntityBehaviorTree(DWORD entityID) const;
    
    /**
     * @brief Executa uma árvore de comportamento
     * @param entityID ID da entidade
     * @param treeID ID da árvore (0 para a árvore da entidade)
     * @param context Contexto
     * @return Status de execução
     */
    BehaviorExecutionStatus ExecuteBehaviorTree(DWORD entityID, DWORD treeID, const AIExecutionContext& context);
    
    /**
     * @brief Cria uma máquina de estado
     * @param entityID ID da entidade
     * @param name Nome
     * @return ID da máquina, ou 0 se falhar
     */
    DWORD CreateStateMachine(DWORD entityID, const std::string& name);
    
    /**
     * @brief Adiciona um estado a uma máquina de estado
     * @param fsmID ID da máquina
     * @param name Nome
     * @param behavior Comportamento
     * @param entryAction Ação de entrada
     * @param exitAction Ação de saída
     * @param updateAction Ação de atualização
     * @param isInitialState Se é estado inicial
     * @return ID do estado, ou 0 se falhar
     */
    DWORD AddStateToFSM(DWORD fsmID, const std::string& name, AIBehaviorType behavior, 
                       const std::string& entryAction, const std::string& exitAction, 
                       const std::string& updateAction, bool isInitialState);
    
    /**
     * @brief Adiciona uma transição a um estado
     * @param stateID ID do estado
     * @param targetStateID ID do estado alvo
     * @param condition Condição
     * @return true se adicionado com sucesso
     */
    bool AddTransitionToState(DWORD stateID, DWORD targetStateID, const std::string& condition);
    
    /**
     * @brief Remove uma transição de um estado
     * @param stateID ID do estado
     * @param targetStateID ID do estado alvo
     * @return true se removido com sucesso
     */
    bool RemoveTransitionFromState(DWORD stateID, DWORD targetStateID);
    
    /**
     * @brief Obtém uma máquina de estado
     * @param fsmID ID da máquina
     * @return Ponteiro para a máquina, ou nullptr se não existir
     */
    const StateMachine* GetStateMachine(DWORD fsmID) const;
    
    /**
     * @brief Obtém a máquina de estado de uma entidade
     * @param entityID ID da entidade
     * @return Ponteiro para a máquina, ou nullptr se não existir
     */
    const StateMachine* GetEntityStateMachine(DWORD entityID) const;
    
    /**
     * @brief Atualiza uma máquina de estado
     * @param entityID ID da entidade
     * @param fsmID ID da máquina (0 para a máquina da entidade)
     * @param context Contexto
     * @return Status de execução
     */
    BehaviorExecutionStatus UpdateStateMachine(DWORD entityID, DWORD fsmID, const AIExecutionContext& context);
    
    /**
     * @brief Registra um comportamento
     * @param entityID ID da entidade
     * @param type Tipo
     * @param priority Prioridade
     * @param parameters Parâmetros
     * @param scriptID ID do script
     * @return ID do comportamento, ou 0 se falhar
     */
    DWORD RegisterBehavior(DWORD entityID, AIBehaviorType type, float priority, 
                         const std::map<std::string, std::string>& parameters = {}, 
                         DWORD scriptID = 0);
    
    /**
     * @brief Executa um comportamento
     * @param entityID ID da entidade
     * @param behaviorID ID do comportamento
     * @param context Contexto
     * @return Status de execução
     */
    BehaviorExecutionStatus ExecuteBehavior(DWORD entityID, DWORD behaviorID, const AIExecutionContext& context);
    
    /**
     * @brief Obtém comportamentos de uma entidade
     * @param entityID ID da entidade
     * @param enabledOnly Apenas habilitados
     * @return Lista de comportamentos
     */
    std::vector<AIBehavior> GetEntityBehaviors(DWORD entityID, bool enabledOnly = true) const;
    
    /**
     * @brief Habilita ou desabilita um comportamento
     * @param behaviorID ID do comportamento
     * @param enabled Habilitado
     * @return true se modificado com sucesso
     */
    bool EnableBehavior(DWORD behaviorID, bool enabled);
    
    /**
     * @brief Define a prioridade de um comportamento
     * @param behaviorID ID do comportamento
     * @param priority Prioridade
     * @return true se modificado com sucesso
     */
    bool SetBehaviorPriority(DWORD behaviorID, float priority);
    
    /**
     * @brief Cria um grupo de IA
     * @param name Nome
     * @param leaderID ID do líder
     * @param strategy Estratégia
     * @param formationSpacing Espaçamento de formação
     * @param formationPattern Padrão de formação
     * @param zoneID ID da zona
     * @return ID do grupo, ou 0 se falhar
     */
    DWORD CreateAIGroup(const std::string& name, DWORD leaderID, GroupStrategyType strategy, 
                      float formationSpacing, const std::string& formationPattern, DWORD zoneID);
    
    /**
     * @brief Adiciona um membro a um grupo
     * @param groupID ID do grupo
     * @param entityID ID da entidade
     * @return true se adicionado com sucesso
     */
    bool AddMemberToGroup(DWORD groupID, DWORD entityID);
    
    /**
     * @brief Remove um membro de um grupo
     * @param groupID ID do grupo
     * @param entityID ID da entidade
     * @return true se removido com sucesso
     */
    bool RemoveMemberFromGroup(DWORD groupID, DWORD entityID);
    
    /**
     * @brief Define o líder de um grupo
     * @param groupID ID do grupo
     * @param leaderID ID do líder
     * @return true se definido com sucesso
     */
    bool SetGroupLeader(DWORD groupID, DWORD leaderID);
    
    /**
     * @brief Define a estratégia de um grupo
     * @param groupID ID do grupo
     * @param strategy Estratégia
     * @return true se definido com sucesso
     */
    bool SetGroupStrategy(DWORD groupID, GroupStrategyType strategy);
    
    /**
     * @brief Obtém um grupo
     * @param groupID ID do grupo
     * @return Ponteiro para o grupo, ou nullptr se não existir
     */
    const AIGroup* GetGroup(DWORD groupID) const;
    
    /**
     * @brief Obtém grupos por zona
     * @param zoneID ID da zona
     * @return Lista de grupos
     */
    std::vector<AIGroup> GetGroupsByZone(DWORD zoneID) const;
    
    /**
     * @brief Cria uma posição tática
     * @param groupID ID do grupo
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param tacticType Tipo de tática
     * @param priority Prioridade
     * @param coverValue Valor de cobertura
     * @return ID da posição, ou 0 se falhar
     */
    DWORD CreateTacticalPosition(DWORD groupID, float posX, float posY, float posZ, 
                               CombatTacticType tacticType, float priority, DWORD coverValue);
    
    /**
     * @brief Atribui uma entidade a uma posição tática
     * @param positionID ID da posição
     * @param entityID ID da entidade
     * @return true se atribuído com sucesso
     */
    bool AssignEntityToPosition(DWORD positionID, DWORD entityID);
    
    /**
     * @brief Libera uma posição tática
     * @param positionID ID da posição
     * @return true se liberado com sucesso
     */
    bool ReleasePosition(DWORD positionID);
    
    /**
     * @brief Obtém posições táticas de um grupo
     * @param groupID ID do grupo
     * @return Lista de posições
     */
    std::vector<TacticalPosition> GetGroupTacticalPositions(DWORD groupID) const;
    
    /**
     * @brief Adiciona uma entrada de percepção
     * @param entityID ID da entidade
     * @param type Tipo
     * @param targetID ID do alvo
     * @param intensity Intensidade
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param expirationTime Hora de expiração
     * @return ID da percepção, ou 0 se falhar
     */
    DWORD AddPerception(DWORD entityID, PerceptionType type, DWORD targetID, float intensity, 
                      float posX, float posY, float posZ, time_t expirationTime = 0);
    
    /**
     * @brief Verifica se uma entidade percebe um alvo
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @param type Tipo
     * @return Resultado da percepção
     */
    PerceptionResult CanPerceiveTarget(DWORD entityID, DWORD targetID, PerceptionType type);
    
    /**
     * @brief Obtém percepções de uma entidade
     * @param entityID ID da entidade
     * @param type Tipo (PerceptionType::VISUAL para todos)
     * @return Lista de percepções
     */
    std::vector<PerceptionEntry> GetEntityPerceptions(DWORD entityID, PerceptionType type = PerceptionType::VISUAL) const;
    
    /**
     * @brief Adiciona uma entrada de memória
     * @param entityID ID da entidade
     * @param type Tipo
     * @param targetID ID do alvo
     * @param data Dados
     * @param importance Importância
     * @param persistent Persistente
     * @param expirationTime Hora de expiração
     * @return ID da memória, ou 0 se falhar
     */
    DWORD AddMemoryEntry(DWORD entityID, MemoryEntryType type, DWORD targetID, 
                       const std::string& data, float importance, bool persistent, 
                       time_t expirationTime = 0);
    
    /**
     * @brief Obtém entradas de memória de uma entidade
     * @param entityID ID da entidade
     * @param type Tipo (MemoryEntryType::ENTITY para todos)
     * @return Lista de entradas
     */
    std::vector<MemoryEntry> GetEntityMemory(DWORD entityID, MemoryEntryType type = MemoryEntryType::ENTITY) const;
    
    /**
     * @brief Cria um caminho
     * @param entityID ID da entidade
     * @param name Nome
     * @param targetEntityID ID da entidade alvo
     * @param defaultMovement Movimento padrão
     * @return ID do caminho, ou 0 se falhar
     */
    DWORD CreatePath(DWORD entityID, const std::string& name, DWORD targetEntityID, AIMovementType defaultMovement);
    
    /**
     * @brief Adiciona um ponto a um caminho
     * @param pathID ID do caminho
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param order Ordem
     * @param speedFactor Fator de velocidade
     * @param movementType Tipo de movimento
     * @param action Ação
     * @return ID do ponto, ou 0 se falhar
     */
    DWORD AddPointToPath(DWORD pathID, float posX, float posY, float posZ, DWORD order, 
                       float speedFactor, AIMovementType movementType, const std::string& action);
    
    /**
     * @brief Obtém um caminho
     * @param pathID ID do caminho
     * @return Ponteiro para o caminho, ou nullptr se não existir
     */
    const AIPath* GetPath(DWORD pathID) const;
    
    /**
     * @brief Obtém caminhos de uma entidade
     * @param entityID ID da entidade
     * @return Lista de caminhos
     */
    std::vector<AIPath> GetEntityPaths(DWORD entityID) const;
    
    /**
     * @brief Define o ponto atual de um caminho
     * @param pathID ID do caminho
     * @param pointIndex Índice do ponto
     * @return true se definido com sucesso
     */
    bool SetCurrentPathPoint(DWORD pathID, DWORD pointIndex);
    
    /**
     * @brief Marca um ponto de caminho como alcançado
     * @param pathID ID do caminho
     * @param pointID ID do ponto
     * @return true se marcado com sucesso
     */
    bool MarkPathPointReached(DWORD pathID, DWORD pointID);
    
    /**
     * @brief Calcula um caminho entre dois pontos
     * @param entityID ID da entidade
     * @param startX Início X
     * @param startY Início Y
     * @param startZ Início Z
     * @param endX Fim X
     * @param endY Fim Y
     * @param endZ Fim Z
     * @param zoneID ID da zona
     * @param maxDistance Distância máxima
     * @return ID do caminho, ou 0 se falhar
     */
    DWORD CalculatePath(DWORD entityID, float startX, float startY, float startZ, 
                      float endX, float endY, float endZ, DWORD zoneID, float maxDistance);
    
    /**
     * @brief Cria uma tabela de ameaças
     * @param entityID ID da entidade
     * @param aggroRadius Raio de aggro
     * @param threatDecayRate Taxa de decaimento de ameaça
     * @param threatTransferRate Taxa de transferência de ameaça
     * @return ID da tabela, ou 0 se falhar
     */
    DWORD CreateThreatTable(DWORD entityID, DWORD aggroRadius, float threatDecayRate, float threatTransferRate);
    
    /**
     * @brief Adiciona ameaça a uma entidade
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @param amount Quantidade
     * @return Valor de ameaça atual
     */
    float AddThreat(DWORD entityID, DWORD targetID, float amount);
    
    /**
     * @brief Reduz ameaça de uma entidade
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @param amount Quantidade
     * @return Valor de ameaça atual
     */
    float ReduceThreat(DWORD entityID, DWORD targetID, float amount);
    
    /**
     * @brief Remove toda a ameaça de uma entidade
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @return true se removido com sucesso
     */
    bool ClearThreat(DWORD entityID, DWORD targetID);
    
    /**
     * @brief Obtém o estado de ameaça de uma entidade
     * @param entityID ID da entidade
     * @return Estado de ameaça
     */
    ThreatState GetThreatState(DWORD entityID) const;
    
    /**
     * @brief Obtém a tabela de ameaças de uma entidade
     * @param entityID ID da entidade
     * @return Ponteiro para a tabela, ou nullptr se não existir
     */
    const ThreatTable* GetThreatTable(DWORD entityID) const;
    
    /**
     * @brief Avalia habilidades para uma entidade
     * @param entityID ID da entidade
     * @param abilityIDs IDs das habilidades
     * @param targetID ID do alvo
     * @return Lista de resultados de avaliação
     */
    std::vector<AbilityEvaluationResult> EvaluateAbilities(DWORD entityID, 
                                                        const std::vector<DWORD>& abilityIDs, 
                                                        DWORD targetID);
    
    /**
     * @brief Registra uma avaliação de habilidade
     * @param entityID ID da entidade
     * @param abilityID ID da habilidade
     * @param targetID ID do alvo
     * @param considerations Considerações
     * @return ID da avaliação, ou 0 se falhar
     */
    DWORD RegisterAbilityEvaluation(DWORD entityID, DWORD abilityID, DWORD targetID, 
                                 const std::map<std::string, float>& considerations);
    
    /**
     * @brief Obtém avaliações de habilidade de uma entidade
     * @param entityID ID da entidade
     * @return Lista de avaliações
     */
    std::vector<AbilityEvaluation> GetEntityAbilityEvaluations(DWORD entityID) const;
    
    /**
     * @brief Realiza uma avaliação de combate
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @return Avaliação de combate
     */
    CombatEvaluation EvaluateCombat(DWORD entityID, DWORD targetID);
    
    /**
     * @brief Registra uma avaliação de combate
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @param threatRatio Relação de ameaça
     * @param healthRatio Relação de vida
     * @param damagePerSecond Dano por segundo
     * @param survivalTime Tempo de sobrevivência
     * @param victoryChance Chance de vitória
     * @param suggestedTactic Tática sugerida
     * @return ID da avaliação, ou 0 se falhar
     */
    DWORD RegisterCombatEvaluation(DWORD entityID, DWORD targetID, float threatRatio, 
                                float healthRatio, float damagePerSecond, float survivalTime, 
                                float victoryChance, CombatTacticType suggestedTactic);
    
    /**
     * @brief Cria um perfil de personalidade
     * @param entityID ID da entidade
     * @param name Nome
     * @param defaultReaction Reação padrão
     * @param adaptability Adaptabilidade
     * @param curiosity Curiosidade
     * @param aggressiveness Agressividade
     * @param cautiousness Cautela
     * @param sociability Sociabilidade
     * @return ID do perfil, ou 0 se falhar
     */
    DWORD CreatePersonalityProfile(DWORD entityID, const std::string& name, 
                                AIReactionType defaultReaction, float adaptability, 
                                float curiosity, float aggressiveness, 
                                float cautiousness, float sociability);
    
    /**
     * @brief Adiciona um traço a um perfil de personalidade
     * @param profileID ID do perfil
     * @param type Tipo
     * @param value Valor
     * @param description Descrição
     * @param modifiers Modificadores
     * @return ID do traço, ou 0 se falhar
     */
    DWORD AddPersonalityTrait(DWORD profileID, AIPersonalityType type, float value, 
                           const std::string& description, 
                           const std::map<std::string, float>& modifiers);
    
    /**
     * @brief Obtém o perfil de personalidade de uma entidade
     * @param entityID ID da entidade
     * @return Ponteiro para o perfil, ou nullptr se não existir
     */
    const PersonalityProfile* GetEntityPersonalityProfile(DWORD entityID) const;
    
    /**
     * @brief Cria uma agenda
     * @param entityID ID da entidade
     * @param name Nome
     * @param repeating Repetitiva
     * @param dayMask Máscara de dia
     * @return ID da agenda, ou 0 se falhar
     */
    DWORD CreateSchedule(DWORD entityID, const std::string& name, bool repeating, BYTE dayMask);
    
    /**
     * @brief Adiciona um item a uma agenda
     * @param scheduleID ID da agenda
     * @param hour Hora
     * @param minute Minuto
     * @param duration Duração (minutos)
     * @param behavior Comportamento
     * @param locationID ID da localização
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param description Descrição
     * @return ID do item, ou 0 se falhar
     */
    DWORD AddScheduleItem(DWORD scheduleID, BYTE hour, BYTE minute, DWORD duration, 
                        AIBehaviorType behavior, DWORD locationID, float posX, float posY, 
                        float posZ, const std::string& description);
    
    /**
     * @brief Obtém a agenda de uma entidade
     * @param entityID ID da entidade
     * @return Ponteiro para a agenda, ou nullptr se não existir
     */
    const Schedule* GetEntitySchedule(DWORD entityID) const;
    
    /**
     * @brief Obtém o item atual de uma agenda
     * @param scheduleID ID da agenda
     * @return Ponteiro para o item, ou nullptr se não existir
     */
    const ScheduleItem* GetCurrentScheduleItem(DWORD scheduleID) const;
    
    /**
     * @brief Registra um callback para atualização de IA
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterAIUpdateCallback(AIUpdateCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterAIUpdateCallback(int callbackID);
    
    /**
     * @brief Registra um callback para percepção
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterPerceptionCallback(PerceptionCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterPerceptionCallback(int callbackID);
    
    /**
     * @brief Registra um callback para grupo de IA
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterAIGroupCallback(AIGroupCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterAIGroupCallback(int callbackID);
    
    /**
     * @brief Registra um callback para ameaça
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterThreatCallback(ThreatCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterThreatCallback(int callbackID);
    
    /**
     * @brief Registra um callback para transição de estado
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterStateTransitionCallback(StateTransitionCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterStateTransitionCallback(int callbackID);
    
    /**
     * @brief Obtém as estatísticas do gerenciador
     * @return Estatísticas
     */
    AIManagerStats GetStatistics() const;
    
    /**
     * @brief Encontra entidades numa área
     * @param centerX Centro X
     * @param centerY Centro Y
     * @param centerZ Centro Z
     * @param radius Raio
     * @param zoneID ID da zona
     * @return Lista de IDs de entidades
     */
    std::vector<DWORD> FindEntitiesInArea(float centerX, float centerY, float centerZ, 
                                      float radius, DWORD zoneID) const;
    
    /**
     * @brief Encontra entidades numa área por tipo
     * @param centerX Centro X
     * @param centerY Centro Y
     * @param centerZ Centro Z
     * @param radius Raio
     * @param zoneID ID da zona
     * @param controllerType Tipo de controlador
     * @return Lista de IDs de entidades
     */
    std::vector<DWORD> FindEntitiesInAreaByType(float centerX, float centerY, float centerZ, 
                                            float radius, DWORD zoneID, DWORD controllerType) const;
    
    /**
     * @brief Obtém o controlador de IA de uma entidade
     * @param entityID ID da entidade
     * @return Ponteiro para o controlador, ou nullptr se não existir
     */
    const AIController* GetEntityController(DWORD entityID) const;
    
    /**
     * @brief Define o comportamento atual de uma entidade
     * @param entityID ID da entidade
     * @param behaviorID ID do comportamento
     * @return true se definido com sucesso
     */
    bool SetEntityCurrentBehavior(DWORD entityID, DWORD behaviorID);
    
    /**
     * @brief Força uma transição de estado
     * @param fsmID ID da máquina
     * @param targetStateID ID do estado alvo
     * @return true se forçado com sucesso
     */
    bool ForceStateTransition(DWORD fsmID, DWORD targetStateID);
    
    /**
     * @brief Avalia os arredores de uma entidade
     * @param entityID ID da entidade
     * @param radius Raio
     * @return Lista de percepções
     */
    std::vector<PerceptionResult> EvaluateSurroundings(DWORD entityID, float radius);
    
    /**
     * @brief Processa a agenda de uma entidade
     * @param entityID ID da entidade
     * @param currentTime Hora atual
     * @return ID do item atual, ou 0 se nenhum
     */
    DWORD ProcessEntitySchedule(DWORD entityID, time_t currentTime);
    
    /**
     * @brief Define a memória de uma entidade
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @param value Valor
     * @param type Tipo
     * @return true se definido com sucesso
     */
    bool SetEntityMemory(DWORD entityID, DWORD targetID, const std::string& value, MemoryEntryType type);
    
    /**
     * @brief Encontra um valor na memória de uma entidade
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @param type Tipo
     * @return Valor, ou string vazia se não encontrado
     */
    std::string FindInEntityMemory(DWORD entityID, DWORD targetID, MemoryEntryType type) const;
    
    /**
     * @brief Aplica um comportamento a um grupo
     * @param groupID ID do grupo
     * @param behaviorType Tipo de comportamento
     * @param parameters Parâmetros
     * @return Número de entidades afetadas
     */
    size_t ApplyBehaviorToGroup(DWORD groupID, AIBehaviorType behaviorType, 
                             const std::map<std::string, std::string>& parameters = {});
    
    /**
     * @brief Coordena um grupo para um ataque
     * @param groupID ID do grupo
     * @param targetID ID do alvo
     * @param tacticType Tipo de tática
     * @return Número de entidades coordenadas
     */
    size_t CoordinateGroupAttack(DWORD groupID, DWORD targetID, CombatTacticType tacticType);
    
    /**
     * @brief Obtém o melhor alvo para uma entidade
     * @param entityID ID da entidade
     * @param maxTargets Máximo de alvos
     * @param maxDistance Distância máxima
     * @return Lista de IDs de alvos ordenados
     */
    std::vector<DWORD> GetBestTargetsForEntity(DWORD entityID, size_t maxTargets, float maxDistance);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    AIManager();
    
    /**
     * @brief Destrutor
     */
    ~AIManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    AIManager(const AIManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    AIManager& operator=(const AIManager&) = delete;
    
    /**
     * @brief Thread de atualização principal
     */
    void UpdateThread();
    
    /**
     * @brief Thread de percepção
     */
    void PerceptionThread();
    
    /**
     * @brief Thread de pathfinding
     */
    void PathfindingThread();
    
    /**
     * @brief Thread de comportamento de grupo
     */
    void GroupBehaviorThread();
    
    /**
     * @brief Thread de tomada de decisão
     */
    void DecisionMakingThread();
    
    /**
     * @brief Notifica callbacks de atualização de IA
     * @param entityID ID da entidade
     * @param behaviorID ID do comportamento
     * @param behaviorType Tipo de comportamento
     */
    void NotifyAIUpdateCallbacks(DWORD entityID, DWORD behaviorID, AIBehaviorType behaviorType);
    
    /**
     * @brief Notifica callbacks de percepção
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @param type Tipo
     * @param intensity Intensidade
     */
    void NotifyPerceptionCallbacks(DWORD entityID, DWORD targetID, PerceptionType type, float intensity);
    
    /**
     * @brief Notifica callbacks de grupo de IA
     * @param groupID ID do grupo
     * @param leaderID ID do líder
     * @param strategy Estratégia
     */
    void NotifyAIGroupCallbacks(DWORD groupID, DWORD leaderID, GroupStrategyType strategy);
    
    /**
     * @brief Notifica callbacks de ameaça
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @param threatValue Valor de ameaça
     * @param status Status
     */
    void NotifyThreatCallbacks(DWORD entityID, DWORD targetID, float threatValue, ThreatStatus status);
    
    /**
     * @brief Notifica callbacks de transição de estado
     * @param fsmID ID da máquina
     * @param oldStateID ID do estado antigo
     * @param newStateID ID do estado novo
     */
    void NotifyStateTransitionCallbacks(DWORD fsmID, DWORD oldStateID, DWORD newStateID);
    
    /**
     * @brief Avalia condições de nó
     * @param nodeID ID do nó
     * @param context Contexto
     * @return Resultado da avaliação
     */
    bool EvaluateNodeCondition(DWORD nodeID, const AIExecutionContext& context);
    
    /**
     * @brief Executa ação de nó
     * @param nodeID ID do nó
     * @param context Contexto
     * @return Status de execução
     */
    NodeExecutionStatus ExecuteNodeAction(DWORD nodeID, const AIExecutionContext& context);
    
    /**
     * @brief Processa transições de estado
     * @param fsmID ID da máquina
     * @param context Contexto
     * @return ID do novo estado, ou 0 se nenhuma transição
     */
    DWORD ProcessStateTransitions(DWORD fsmID, const AIExecutionContext& context);
    
    /**
     * @brief Executa ação de estado
     * @param stateID ID do estado
     * @param actionType Tipo de ação (entry, exit, update)
     * @param context Contexto
     * @return Status de execução
     */
    NodeExecutionStatus ExecuteStateAction(DWORD stateID, const std::string& actionType, const AIExecutionContext& context);
    
    /**
     * @brief Calcula distância entre dois pontos
     * @param x1 X1
     * @param y1 Y1
     * @param z1 Z1
     * @param x2 X2
     * @param y2 Y2
     * @param z2 Z2
     * @return Distância
     */
    float CalculateDistance(float x1, float y1, float z1, float x2, float y2, float z2) const;
    
    /**
     * @brief Avalia se uma entidade pode perceber um alvo
     * @param sourceID ID da fonte
     * @param targetID ID do alvo
     * @param type Tipo
     * @param sourcePos Posição da fonte
     * @param targetPos Posição do alvo
     * @return Resultado da percepção
     */
    PerceptionResult EvaluatePerception(DWORD sourceID, DWORD targetID, PerceptionType type, 
                                    const std::tuple<float, float, float>& sourcePos, 
                                    const std::tuple<float, float, float>& targetPos);
    
    /**
     * @brief Calcula a distribuição de formação
     * @param groupID ID do grupo
     * @param pattern Padrão
     * @param spacing Espaçamento
     * @return Lista de posições relativas
     */
    std::vector<std::tuple<float, float, float>> CalculateFormationPositions(DWORD groupID, 
                                                                        const std::string& pattern, 
                                                                        float spacing);
    
    /**
     * @brief Decai valores de ameaça
     * @param elapsed Tempo decorrido
     */
    void DecayThreatValues(DWORD elapsed);
    
    /**
     * @brief Limpa entradas expiradas
     * @param elapsed Tempo decorrido
     */
    void CleanupExpiredEntries(DWORD elapsed);
    
    /**
     * @brief Atualiza comportamentos de entidade
     * @param entityID ID da entidade
     * @param elapsed Tempo decorrido
     * @return Status de execução
     */
    BehaviorExecutionStatus UpdateEntityBehaviors(DWORD entityID, DWORD elapsed);
    
    /**
     * @brief Adiciona uma entidade a uma partição espacial
     * @param entityID ID da entidade
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param zoneID ID da zona
     */
    void AddEntityToSpatialPartition(DWORD entityID, float posX, float posY, float posZ, DWORD zoneID);
    
    /**
     * @brief Remove uma entidade de uma partição espacial
     * @param entityID ID da entidade
     */
    void RemoveEntityFromSpatialPartition(DWORD entityID);
    
    /**
     * @brief Atualiza a partição espacial de uma entidade
     * @param entityID ID da entidade
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param zoneID ID da zona
     */
    void UpdateEntitySpatialPartition(DWORD entityID, float posX, float posY, float posZ, DWORD zoneID);
    
    /**
     * @brief Obtém entidades em uma partição espacial
     * @param centerX Centro X
     * @param centerY Centro Y
     * @param centerZ Centro Z
     * @param radius Raio
     * @param zoneID ID da zona
     * @return Lista de IDs de entidades
     */
    std::vector<DWORD> GetEntitiesInSpatialPartition(float centerX, float centerY, float centerZ, 
                                                float radius, DWORD zoneID) const;
    
    /**
     * @brief Carrega controladores de IA do banco de dados
     * @return true se carregado com sucesso
     */
    bool LoadControllersFromDB();
    
    /**
     * @brief Carrega comportamentos do banco de dados
     * @param entityID ID da entidade
     * @return Lista de comportamentos
     */
    std::vector<AIBehavior> LoadBehaviorsFromDB(DWORD entityID);
    
    /**
     * @brief Carrega árvores de comportamento do banco de dados
     * @param entityID ID da entidade
     * @return Lista de árvores
     */
    std::vector<BehaviorTree> LoadBehaviorTreesFromDB(DWORD entityID);
    
    /**
     * @brief Carrega nós de decisão do banco de dados
     * @param treeID ID da árvore
     * @return Lista de nós
     */
    std::vector<DecisionNode> LoadDecisionNodesFromDB(DWORD treeID);
    
    /**
     * @brief Carrega máquinas de estado do banco de dados
     * @param entityID ID da entidade
     * @return Lista de máquinas
     */
    std::vector<StateMachine> LoadStateMachinesFromDB(DWORD entityID);
    
    /**
     * @brief Carrega estados do banco de dados
     * @param fsmID ID da máquina
     * @return Lista de estados
     */
    std::vector<AIState> LoadStatesFromDB(DWORD fsmID);
    
    /**
     * @brief Carrega percepções do banco de dados
     * @param entityID ID da entidade
     * @return Lista de percepções
     */
    std::vector<PerceptionEntry> LoadPerceptionsFromDB(DWORD entityID);
    
    /**
     * @brief Carrega memória do banco de dados
     * @param entityID ID da entidade
     * @return Lista de entradas
     */
    std::vector<MemoryEntry> LoadMemoryFromDB(DWORD entityID);
    
    /**
     * @brief Carrega grupos do banco de dados
     * @param zoneID ID da zona
     * @return Lista de grupos
     */
    std::vector<AIGroup> LoadGroupsFromDB(DWORD zoneID);
    
    /**
     * @brief Carrega caminhos do banco de dados
     * @param entityID ID da entidade
     * @return Lista de caminhos
     */
    std::vector<AIPath> LoadPathsFromDB(DWORD entityID);
    
    /**
     * @brief Carrega tabelas de ameaças do banco de dados
     * @param entityID ID da entidade
     * @return Ponteiro para a tabela, ou nullptr se não existir
     */
    ThreatTable* LoadThreatTableFromDB(DWORD entityID);
    
    /**
     * @brief Carrega perfil de personalidade do banco de dados
     * @param entityID ID da entidade
     * @return Ponteiro para o perfil, ou nullptr se não existir
     */
    PersonalityProfile* LoadPersonalityProfileFromDB(DWORD entityID);
    
    /**
     * @brief Carrega agenda do banco de dados
     * @param entityID ID da entidade
     * @return Ponteiro para a agenda, ou nullptr se não existir
     */
    Schedule* LoadScheduleFromDB(DWORD entityID);
    
    /**
     * @brief Adiciona uma tarefa assíncrona à fila
     * @param task Tarefa
     */
    void AddAsyncTask(std::function<void()> task);
    
    /**
     * @brief Atualiza estatísticas
     */
    void UpdateStatistics();
    
    // Configuração
    AIManagerConfig config_;
    
    // Controladores
    std::unordered_map<DWORD, AIController> controllers_;
    mutable std::shared_mutex controllersMutex_;
    
    // Comportamentos
    std::unordered_map<DWORD, std::unordered_map<DWORD, AIBehavior>> behaviors_;
    mutable std::shared_mutex behaviorsMutex_;
    
    // Árvores de comportamento
    std::unordered_map<DWORD, BehaviorTree> behaviorTrees_;
    mutable std::shared_mutex behaviorTreesMutex_;
    
    // Nós de decisão
    std::unordered_map<DWORD, std::unordered_map<DWORD, DecisionNode>> decisionNodes_;
    mutable std::shared_mutex decisionNodesMutex_;
    
    // Máquinas de estado
    std::unordered_map<DWORD, StateMachine> stateMachines_;
    mutable std::shared_mutex stateMachinesMutex_;
    
    // Estados
    std::unordered_map<DWORD, std::unordered_map<DWORD, AIState>> states_;
    mutable std::shared_mutex statesMutex_;
    
    // Percepções
    std::unordered_map<DWORD, std::vector<PerceptionEntry>> perceptions_;
    mutable std::shared_mutex perceptionsMutex_;
    
    // Memória
    std::unordered_map<DWORD, std::vector<MemoryEntry>> memories_;
    mutable std::shared_mutex memoriesMutex_;
    
    // Grupos
    std::unordered_map<DWORD, AIGroup> groups_;
    mutable std::shared_mutex groupsMutex_;
    
    // Posições táticas
    std::unordered_map<DWORD, std::vector<TacticalPosition>> tacticalPositions_;
    mutable std::shared_mutex tacticalPositionsMutex_;
    
    // Caminhos
    std::unordered_map<DWORD, AIPath> paths_;
    mutable std::shared_mutex pathsMutex_;
    
    // Pontos de caminho
    std::unordered_map<DWORD, std::vector<PathPoint>> pathPoints_;
    mutable std::shared_mutex pathPointsMutex_;
    
    // Tabelas de ameaças
    std::unordered_map<DWORD, ThreatTable> threatTables_;
    mutable std::shared_mutex threatTablesMutex_;
    
    // Avaliações de habilidade
    std::unordered_map<DWORD, std::vector<AbilityEvaluation>> abilityEvaluations_;
    mutable std::shared_mutex abilityEvaluationsMutex_;
    
    // Avaliações de combate
    std::unordered_map<DWORD, std::unordered_map<DWORD, CombatEvaluation>> combatEvaluations_;
    mutable std::shared_mutex combatEvaluationsMutex_;
    
    // Perfis de personalidade
    std::unordered_map<DWORD, PersonalityProfile> personalityProfiles_;
    mutable std::shared_mutex personalityProfilesMutex_;
    
    // Agendas
    std::unordered_map<DWORD, Schedule> schedules_;
    mutable std::shared_mutex schedulesMutex_;
    
    // Itens de agenda
    std::unordered_map<DWORD, std::vector<ScheduleItem>> scheduleItems_;
    mutable std::shared_mutex scheduleItemsMutex_;
    
    // Índices
    std::unordered_map<DWORD, DWORD> entityControllerIndex_;
    std::unordered_map<DWORD, std::vector<DWORD>> entityBehaviorIndex_;
    std::unordered_map<DWORD, DWORD> entityBehaviorTreeIndex_;
    std::unordered_map<DWORD, DWORD> entityStateMachineIndex_;
    std::unordered_map<DWORD, DWORD> entityGroupIndex_;
    std::unordered_map<DWORD, DWORD> entityThreatTableIndex_;
    std::unordered_map<DWORD, DWORD> entityPersonalityProfileIndex_;
    std::unordered_map<DWORD, DWORD> entityScheduleIndex_;
    mutable std::shared_mutex indicesMutex_;
    
    // Particionamento espacial
    std::unordered_map<DWORD, std::unordered_map<DWORD, std::set<DWORD>>> spatialPartitions_;
    std::unordered_map<DWORD, std::tuple<DWORD, DWORD, DWORD>> entityPartitionIndex_;
    mutable std::shared_mutex spatialPartitionsMutex_;
    
    // Callbacks
    std::map<int, AIUpdateCallback> aiUpdateCallbacks_;
    std::mutex aiUpdateCallbacksMutex_;
    int nextAIUpdateCallbackID_;
    
    std::map<int, PerceptionCallback> perceptionCallbacks_;
    std::mutex perceptionCallbacksMutex_;
    int nextPerceptionCallbackID_;
    
    std::map<int, AIGroupCallback> aiGroupCallbacks_;
    std::mutex aiGroupCallbacksMutex_;
    int nextAIGroupCallbackID_;
    
    std::map<int, ThreatCallback> threatCallbacks_;
    std::mutex threatCallbacksMutex_;
    int nextThreatCallbackID_;
    
    std::map<int, StateTransitionCallback> stateTransitionCallbacks_;
    std::mutex stateTransitionCallbacksMutex_;
    int nextStateTransitionCallbackID_;
    
    // Threads
    std::thread updateThread_;
    std::thread perceptionThread_;
    std::thread pathfindingThread_;
    std::thread groupBehaviorThread_;
    std::thread decisionMakingThread_;
    std::atomic<bool> running_;
    
    // Filas de tarefas
    std::queue<std::function<void()>> updateTasks_;
    std::mutex updateTasksMutex_;
    std::condition_variable updateTasksCV_;
    
    std::queue<std::function<void()>> perceptionTasks_;
    std::mutex perceptionTasksMutex_;
    std::condition_variable perceptionTasksCV_;
    
    std::queue<std::function<void()>> pathfindingTasks_;
    std::mutex pathfindingTasksMutex_;
    std::condition_variable pathfindingTasksCV_;
    
    std::queue<std::function<void()>> groupBehaviorTasks_;
    std::mutex groupBehaviorTasksMutex_;
    std::condition_variable groupBehaviorTasksCV_;
    
    std::queue<std::function<void()>> decisionMakingTasks_;
    std::mutex decisionMakingTasksMutex_;
    std::condition_variable decisionMakingTasksCV_;
    
    // Estatísticas
    AIManagerStats stats_;
    mutable std::mutex statsMutex_;
    
    // Estado
    bool initialized_;
    time_t lastCleanupTime_;
    
    // IDs únicos
    std::atomic<DWORD> nextControllerID_;
    std::atomic<DWORD> nextBehaviorID_;
    std::atomic<DWORD> nextTreeID_;
    std::atomic<DWORD> nextNodeID_;
    std::atomic<DWORD> nextFsmID_;
    std::atomic<DWORD> nextStateID_;
    std::atomic<DWORD> nextPerceptionID_;
    std::atomic<DWORD> nextMemoryID_;
    std::atomic<DWORD> nextGroupID_;
    std::atomic<DWORD> nextPositionID_;
    std::atomic<DWORD> nextPathID_;
    std::atomic<DWORD> nextPointID_;
    std::atomic<DWORD> nextThreatTableID_;
    std::atomic<DWORD> nextEvaluationID_;
    std::atomic<DWORD> nextProfileID_;
    std::atomic<DWORD> nextTraitID_;
    std::atomic<DWORD> nextScheduleID_;
    std::atomic<DWORD> nextScheduleItemID_;
};

// Acesso global
#define g_AIManager AIManager::GetInstance()

} // namespace ai
} // namespace server
} // namespace wyd

#endif // AIMANAGER_H

} // namespace wydbr
