/**
 * WYDAI.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/WYDAI.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_AI_H
#define WYD_AI_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <bitset>
#include <algorithm>
#include <cmath>
#include <queue>
#include <random>
#include "../../../core/WYDTypes.h"
#include "NPCTypes.h"

namespace wydbr {


/**
 * @file WYDAI.h
 * @brief Sistema avançado de inteligência artificial para NPCs
 * 
 * Este arquivo implementa um sistema sofisticado de inteligência artificial para NPCs,
 * fornecendo comportamentos complexos e realistas. O sistema utiliza uma combinação de
 * máquinas de estado, sistemas baseados em árvores de comportamento, e algoritmos de
 * pathfinding otimizados.
 * 
 * O sistema de AI inclui:
 * - Árvores de comportamento para decisões complexas
 * - Sistema de percepção sensorial
 * - Pathfinding com A* otimizado
 * - Comportamentos emergentes de grupo
 * - Sistema de memória para eventos passados
 * - Reatividade emocional e adaptabilidade
 */

namespace wyd {
namespace server {

// Forward declarations
class World;
class Zone;
class Player;
class NPC;

/**
 * @brief Tipo de evento sensorial para o sistema de percepção
 */
enum class SensoryEventType : BYTE {
    NONE = 0,
    VISUAL = 1,        // Evento visual (viu algo)
    AUDIO = 2,         // Evento auditivo (ouviu algo)
    DAMAGE = 3,        // Recebeu dano
    FRIENDLY_DEATH = 4,// Um aliado morreu
    ENEMY_DEATH = 5,   // Um inimigo morreu
    LOW_HEALTH = 6,    // Saúde baixa
    CALL_FOR_HELP = 7, // Chamado de ajuda de um aliado
    BUFF_APPLIED = 8,  // Efeito positivo aplicado
    DEBUFF_APPLIED = 9,// Efeito negativo aplicado
    SPELL_CAST = 10,   // Conjuração de magia
    RANGED_ATTACK = 11,// Ataque à distância
    MELEE_ATTACK = 12, // Ataque corpo a corpo
    ENEMY_FLEE = 13,   // Inimigo fugindo
    SURPRISE = 14      // Surpresa/susto
};

/**
 * @brief Prioridade de ação de AI
 */
enum class AIPriority : BYTE {
    LOWEST = 0,        // Prioridade mínima
    VERY_LOW = 1,      // Prioridade muito baixa
    LOW = 2,           // Prioridade baixa
    BELOW_NORMAL = 3,  // Prioridade abaixo do normal
    NORMAL = 4,        // Prioridade normal
    ABOVE_NORMAL = 5,  // Prioridade acima do normal
    HIGH = 6,          // Prioridade alta
    VERY_HIGH = 7,     // Prioridade muito alta
    CRITICAL = 8,      // Prioridade crítica
    OVERRIDE = 9       // Sobrepõe qualquer outra ação
};

/**
 * @brief Resultado de um nó da árvore de comportamento
 */
enum /**
 * Classe WYDBehaviorStatus
 * 
 * Esta classe implementa a funcionalidade WYDBehaviorStatus conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class BehaviorStatus {
    INVALID,      // Status inválido/desconhecido
    SUCCESS,      // A ação foi concluída com sucesso
    FAILURE,      // A ação falhou
    RUNNING,      // A ação está em andamento
    SUSPENDED     // A ação foi suspensa
};

/**
 * @brief Eventos sensoriais percebidos pelo NPC
 */
struct SensoryEvent {
    SensoryEventType type;  // Tipo de evento
    DWORD sourceId;         // ID da fonte do evento
    WORD x;                 // Posição X do evento
    WORD y;                 // Posição Y do evento
    DWORD value;            // Valor associado (dano, cura, etc.)
    DWORD timestamp;        // Timestamp do evento
    
    SensoryEvent()
        : type(SensoryEventType::NONE)
        , sourceId(0)
        , x(0)
        , y(0)
        , value(0)
        , timestamp(0)
    {
    }
};

/**
 * @brief Informações de memória do NPC
 */
struct MemoryRecord {
    DWORD targetId;         // ID do alvo (jogador, NPC)
    DWORD lastSeenTime;     // Último timestamp em que foi visto
    WORD lastKnownX;        // Última posição X conhecida
    WORD lastKnownY;        // Última posição Y conhecida
    float threat;           // Nível de ameaça
    bool isHostile;         // Se é hostil
    DWORD damageDealt;      // Dano causado pelo NPC ao alvo
    DWORD damageReceived;   // Dano recebido pelo NPC do alvo
    BYTE relationshipLevel; // Nível de relacionamento (-100 a +100)
    
    MemoryRecord()
        : targetId(0)
        , lastSeenTime(0)
        , lastKnownX(0)
        , lastKnownY(0)
        , threat(0.0f)
        , isHostile(false)
        , damageDealt(0)
        , damageReceived(0)
        , relationshipLevel(0)
    {
    }
};

/**
 * @brief Sistema de percepção para NPCs
 */
/**
 * Classe WYDPerceptionSystem
 * 
 * Esta classe implementa a funcionalidade WYDPerceptionSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PerceptionSystem {
public:
    /**
     * @brief Construtor
     * @param npc Ponteiro para o NPC que usa este sistema
     */
    PerceptionSystem(NPC* npc);
    
    /**
     * @brief Destrutor
     */
    ~PerceptionSystem();
    
    /**
     * @brief Atualiza o sistema de percepção
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    /**
     * @brief Adiciona um evento sensorial
     * @param event Evento sensorial
     */
    void AddSensoryEvent(const SensoryEvent& event);
    
    /**
     * @brief Verifica se o NPC pode ver um alvo
     * @param targetId ID do alvo
     * @return true se o NPC pode ver o alvo, false caso contrário
     */
    bool CanSeeTarget(DWORD targetId) const;
    
    /**
     * @brief Verifica se o NPC pode ouvir um alvo
     * @param targetId ID do alvo
     * @return true se o NPC pode ouvir o alvo, false caso contrário
     */
    bool CanHearTarget(DWORD targetId) const;
    
    /**
     * @brief Adiciona informações de memória sobre um alvo
     * @param targetId ID do alvo
     * @param x Posição X
     * @param y Posição Y
     * @param isHostile Se o alvo é hostil
     */
    void AddMemory(DWORD targetId, WORD x, WORD y, bool isHostile = false);
    
    /**
     * @brief Atualiza informações de memória sobre um alvo
     * @param targetId ID do alvo
     * @param x Posição X
     * @param y Posição Y
     */
    void UpdateMemory(DWORD targetId, WORD x, WORD y);
    
    /**
     * @brief Atualiza o nível de ameaça de um alvo
     * @param targetId ID do alvo
     * @param threat Nível de ameaça
     */
    void UpdateThreat(DWORD targetId, float threat);
    
    /**
     * @brief Obtém o nível de ameaça de um alvo
     * @param targetId ID do alvo
     * @return Nível de ameaça
     */
    float GetThreat(DWORD targetId) const;
    
    /**
     * @brief Esquece um alvo (remove da memória)
     * @param targetId ID do alvo
     */
    void ForgetTarget(DWORD targetId);
    
    /**
     * @brief Obtém a memória de um alvo
     * @param targetId ID do alvo
     * @return Referência para a memória (nullptr se não existir)
     */
    const MemoryRecord* GetMemory(DWORD targetId) const;
    
    /**
     * @brief Obtém o alvo com maior ameaça
     * @return ID do alvo com maior ameaça (0 se nenhum)
     */
    DWORD GetHighestThreatTarget() const;
    
    /**
     * @brief Obtém alvos dentro do raio de percepção
     * @param radius Raio (0 para usar o raio de visão padrão)
     * @return Lista de IDs de alvos
     */
    std::vector<DWORD> GetTargetsInPerceptionRange(WORD radius = 0) const;
    
    /**
     * @brief Define o raio de visão
     * @param radius Raio em tiles
     */
    void SetVisionRadius(BYTE radius) { visionRadius_ = radius; }
    
    /**
     * @brief Define o raio de audição
     * @param radius Raio em tiles
     */
    void SetHearingRadius(BYTE radius) { hearingRadius_ = radius; }
    
    /**
     * @brief Define o tempo de memória
     * @param time Tempo em milissegundos
     */
    void SetMemoryDuration(DWORD time) { memoryDuration_ = time; }
    
private:
    NPC* npc_;                               // Ponteiro para o NPC
    std::unordered_map<DWORD, MemoryRecord> memory_; // Memória do NPC
    std::queue<SensoryEvent> eventQueue_;    // Fila de eventos sensoriais
    
    BYTE visionRadius_;                      // Raio de visão
    BYTE hearingRadius_;                     // Raio de audição
    DWORD memoryDuration_;                   // Tempo de duração da memória
    DWORD lastPerceptionUpdate_;             // Timestamp da última atualização
    
    // Funções auxiliares
    /**
 * TargetInLineOfSight
 * 
 * Implementa a funcionalidade TargetInLineOfSight conforme especificação original.
 * @param targetId Parâmetro targetId
 * @return Retorna bool
 */

    bool TargetInLineOfSight(DWORD targetId) const;
    /**
 * ProcessSensoryEvents
 * 
 * Implementa a funcionalidade ProcessSensoryEvents conforme especificação original.
 */

    void ProcessSensoryEvents();
    /**
 * CleanupMemory
 * 
 * Implementa a funcionalidade CleanupMemory conforme especificação original.
 * @param currentTime Parâmetro currentTime
 */

    void CleanupMemory(DWORD currentTime);
};

/**
 * @brief Nó base da árvore de comportamento
 */
/**
 * Classe WYDBehaviorNode
 * 
 * Esta classe implementa a funcionalidade WYDBehaviorNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class BehaviorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     */
    BehaviorNode(const std::string& name);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~BehaviorNode();
    
    /**
     * @brief Inicializa o nó
     * @param ai Ponteiro para o AIController
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    virtual bool Initialize(AIController* ai);
    
    /**
     * @brief Executa o nó
     * @return Status do comportamento
     */
    virtual BehaviorStatus Execute() = 0;
    
    /**
     * @brief Reseta o estado do nó
     */
    virtual void Reset();
    
    /**
     * @brief Termina a execução do nó
     */
    virtual void Terminate(BehaviorStatus status);
    
    /**
     * @brief Obtém o nome do nó
     * @return Nome do nó
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief Obtém o status do nó
     * @return Status do nó
     */
    BehaviorStatus GetStatus() const { return status_; }
    
protected:
    std::string name_;         // Nome do nó
    BehaviorStatus status_;    // Status atual
    AIController* ai_;         // Ponteiro para o AIController
    bool initialized_;         // Se o nó foi inicializado
};

/**
 * @brief Nó composto da árvore de comportamento (possui filhos)
 */
/**
 * Classe WYDCompositeBehaviorNode
 * 
 * Esta classe implementa a funcionalidade WYDCompositeBehaviorNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CompositeBehaviorNode : public BehaviorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     */
    CompositeBehaviorNode(const std::string& name);
    
    /**
     * @brief Destrutor
     */
    virtual ~CompositeBehaviorNode();
    
    /**
     * @brief Inicializa o nó e seus filhos
     * @param ai Ponteiro para o AIController
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(AIController* ai) override;
    
    /**
     * @brief Adiciona um nó filho
     * @param child Ponteiro para o nó filho
     */
    void AddChild(std::unique_ptr<BehaviorNode> child);
    
    /**
     * @brief Reseta o estado do nó e seus filhos
     */
    void Reset() override;
    
protected:
    std::vector<std::unique_ptr<BehaviorNode>> children_; // Nós filhos
};

/**
 * @brief Nó Sequence - executa os filhos em sequência, parando no primeiro que falha
 */
/**
 * Classe WYDSequenceNode
 * 
 * Esta classe implementa a funcionalidade WYDSequenceNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SequenceNode : public CompositeBehaviorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     */
    SequenceNode(const std::string& name);
    
    /**
     * @brief Executa o nó
     * @return Status do comportamento
     */
    BehaviorStatus Execute() override;
    
private:
    size_t currentChild_; // Índice do filho atual
};

/**
 * @brief Nó Selector - executa os filhos em sequência, parando no primeiro que sucede
 */
/**
 * Classe WYDSelectorNode
 * 
 * Esta classe implementa a funcionalidade WYDSelectorNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SelectorNode : public CompositeBehaviorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     */
    SelectorNode(const std::string& name);
    
    /**
     * @brief Executa o nó
     * @return Status do comportamento
     */
    BehaviorStatus Execute() override;
    
private:
    size_t currentChild_; // Índice do filho atual
};

/**
 * @brief Nó Parallel - executa todos os filhos simultaneamente
 */
/**
 * Classe WYDParallelNode
 * 
 * Esta classe implementa a funcionalidade WYDParallelNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ParallelNode : public CompositeBehaviorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     * @param successPolicy Política de sucesso (quantos filhos precisam ter sucesso)
     * @param failurePolicy Política de falha (quantos filhos precisam falhar)
     */
    ParallelNode(const std::string& name, int successPolicy, int failurePolicy);
    
    /**
     * @brief Executa o nó
     * @return Status do comportamento
     */
    BehaviorStatus Execute() override;
    
private:
    int successPolicy_; // Número de filhos que precisam ter sucesso para o nó ter sucesso
    int failurePolicy_; // Número de filhos que precisam falhar para o nó falhar
};

/**
 * @brief Nó Decorator - modifica o comportamento de um único filho
 */
/**
 * Classe WYDDecoratorNode
 * 
 * Esta classe implementa a funcionalidade WYDDecoratorNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DecoratorNode : public BehaviorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     * @param child Ponteiro para o nó filho
     */
    DecoratorNode(const std::string& name, std::unique_ptr<BehaviorNode> child);
    
    /**
     * @brief Destrutor
     */
    virtual ~DecoratorNode();
    
    /**
     * @brief Inicializa o nó e seu filho
     * @param ai Ponteiro para o AIController
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(AIController* ai) override;
    
    /**
     * @brief Reseta o estado do nó e seu filho
     */
    void Reset() override;
    
protected:
    std::unique_ptr<BehaviorNode> child_; // Nó filho
};

/**
 * @brief Nó Inverter - inverte o resultado do filho (sucesso -> falha, falha -> sucesso)
 */
/**
 * Classe WYDInverterNode
 * 
 * Esta classe implementa a funcionalidade WYDInverterNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InverterNode : public DecoratorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     * @param child Ponteiro para o nó filho
     */
    InverterNode(const std::string& name, std::unique_ptr<BehaviorNode> child);
    
    /**
     * @brief Executa o nó
     * @return Status do comportamento
     */
    BehaviorStatus Execute() override;
};

/**
 * @brief Nó Repeater - repete o filho um número específico de vezes
 */
/**
 * Classe WYDRepeaterNode
 * 
 * Esta classe implementa a funcionalidade WYDRepeaterNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class RepeaterNode : public DecoratorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     * @param child Ponteiro para o nó filho
     * @param count Número de repetições (0 para infinito)
     */
    RepeaterNode(const std::string& name, std::unique_ptr<BehaviorNode> child, int count = 0);
    
    /**
     * @brief Executa o nó
     * @return Status do comportamento
     */
    BehaviorStatus Execute() override;
    
    /**
     * @brief Reseta o estado do nó
     */
    void Reset() override;
    
private:
    int count_;        // Número de repetições desejadas
    int currentCount_; // Contagem atual
};

/**
 * @brief Nó Conditional - executa o filho apenas se uma condição for atendida
 */
/**
 * Classe WYDConditionalNode
 * 
 * Esta classe implementa a funcionalidade WYDConditionalNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ConditionalNode : public DecoratorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     * @param child Ponteiro para o nó filho
     * @param condition Função de condição
     */
    ConditionalNode(
        const std::string& name,
        std::unique_ptr<BehaviorNode> child,
        std::function<bool(AIController*)> condition);
    
    /**
     * @brief Executa o nó
     * @return Status do comportamento
     */
    BehaviorStatus Execute() override;
    
private:
    std::function<bool(AIController*)> condition_; // Função de condição
};

/**
 * @brief Nó de ação - executa uma ação específica
 */
/**
 * Classe WYDActionNode
 * 
 * Esta classe implementa a funcionalidade WYDActionNode conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ActionNode : public BehaviorNode {
public:
    /**
     * @brief Construtor
     * @param name Nome do nó
     * @param action Função de ação
     */
    ActionNode(
        const std::string& name,
        std::function<BehaviorStatus(AIController*)> action);
    
    /**
     * @brief Executa o nó
     * @return Status do comportamento
     */
    BehaviorStatus Execute() override;
    
private:
    std::function<BehaviorStatus(AIController*)> action_; // Função de ação
};

/**
 * @brief Algoritmo de pathfinding A*
 */
/**
 * Classe WYDAStarPathfinder
 * 
 * Esta classe implementa a funcionalidade WYDAStarPathfinder conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AStarPathfinder {
public:
    /**
     * @brief Construtor
     * @param world Ponteiro para o mundo
     */
    AStarPathfinder(World* world);
    
    /**
     * @brief Destrutor
     */
    ~AStarPathfinder();
    
    /**
     * @brief Encontra um caminho entre dois pontos
     * @param startX Posição X inicial
     * @param startY Posição Y inicial
     * @param endX Posição X final
     * @param endY Posição Y final
     * @param mapId ID do mapa
     * @param maxIterations Número máximo de iterações (0 para ilimitado)
     * @return Lista de pontos no caminho
     */
    std::vector<std::pair<WORD, WORD>> FindPath(
        WORD startX, WORD startY,
        WORD endX, WORD endY,
        WORD mapId,
        int maxIterations = 1000);
    
    /**
     * @brief Verifica se uma posição é passável
     * @param x Posição X
     * @param y Posição Y
     * @param mapId ID do mapa
     * @return true se a posição é passável, false caso contrário
     */
    bool IsPassable(WORD x, WORD y, WORD mapId) const;
    
    /**
     * @brief Define se deve considerar NPCs como obstáculos
     * @param value Se deve considerar NPCs como obstáculos
     */
    void SetConsiderNPCsAsObstacles(bool value) { considerNPCsAsObstacles_ = value; }
    
    /**
     * @brief Define se deve considerar jogadores como obstáculos
     * @param value Se deve considerar jogadores como obstáculos
     */
    void SetConsiderPlayersAsObstacles(bool value) { considerPlayersAsObstacles_ = value; }
    
private:
    // Estrutura para um nó no algoritmo A*
    struct PathNode {
        WORD x, y;      // Posição
        float g;        // Custo do início até este nó
        float h;        // Heurística (estimativa do custo até o objetivo)
        float f;        // Função de avaliação f = g + h
        PathNode* parent;// Nó pai
        
        PathNode(WORD _x, WORD _y, float _g, float _h, PathNode* _parent)
            : x(_x), y(_y), g(_g), h(_h), f(_g + _h), parent(_parent) {}
        
        // Operador de comparação para a fila de prioridade
        bool operator<(const PathNode& other) const {
            return f > other.f; // Maior f tem menor prioridade
        }
    };
    
    World* world_;                         // Ponteiro para o mundo
    bool considerNPCsAsObstacles_;         // Se deve considerar NPCs como obstáculos
    bool considerPlayersAsObstacles_;      // Se deve considerar jogadores como obstáculos
    
    // Funções auxiliares
    /**
 * CalculateHeuristic
 * 
 * Implementa a funcionalidade CalculateHeuristic conforme especificação original.
 * @param x1 Parâmetro x1
 * @param y1 Parâmetro y1
 * @param x2 Parâmetro x2
 * @param y2 Parâmetro y2
 * @return Retorna float
 */

    float CalculateHeuristic(WORD x1, WORD y1, WORD x2, WORD y2) const;
    /**
 * IsNodeInList
 * 
 * Implementa a funcionalidade IsNodeInList conforme especificação original.
 * @param list Parâmetro list
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool IsNodeInList(const std::vector<PathNode*>& list, WORD x, WORD y) const;
    /**
 * GetNodeFromList
 * 
 * Implementa a funcionalidade GetNodeFromList conforme especificação original.
 * @param list Parâmetro list
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna PathNode
 */

    PathNode* GetNodeFromList(std::vector<PathNode*>& list, WORD x, WORD y) const;
};

/**
 * @brief Sistema emocional para NPCs
 */
/**
 * Classe WYDEmotionalSystem
 * 
 * Esta classe implementa a funcionalidade WYDEmotionalSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EmotionalSystem {
public:
    /**
     * @brief Construtor
     * @param npc Ponteiro para o NPC
     */
    EmotionalSystem(NPC* npc);
    
    /**
     * @brief Destrutor
     */
    ~EmotionalSystem();
    
    /**
     * @brief Atualiza o estado emocional
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    /**
     * @brief Adiciona um estímulo emocional
     * @param emotion Emoção (0=medo, 1=raiva, 2=alegria, 3=tristeza)
     * @param intensity Intensidade do estímulo
     * @param decayRate Taxa de decaimento
     */
    void AddStimulus(BYTE emotion, float intensity, float decayRate = 0.1f);
    
    /**
     * @brief Obtém o nível de uma emoção
     * @param emotion Emoção (0=medo, 1=raiva, 2=alegria, 3=tristeza)
     * @return Nível da emoção (0.0-1.0)
     */
    float GetEmotionLevel(BYTE emotion) const;
    
    /**
     * @brief Obtém a emoção dominante
     * @return Índice da emoção dominante
     */
    BYTE GetDominantEmotion() const;
    
    /**
     * @brief Verifica se está em estado de pânico
     * @return true se está em pânico, false caso contrário
     */
    bool IsPanicking() const;
    
    /**
     * @brief Verifica se está em estado de fúria
     * @return true se está em fúria, false caso contrário
     */
    bool IsEnraged() const;
    
    /**
     * @brief Calcula um modificador baseado no estado emocional
     * @param baseValue Valor base
     * @param emotionalFactor Fator emocional (-1.0 a 1.0)
     * @return Valor modificado
     */
    float CalculateEmotionalModifier(float baseValue, float emotionalFactor) const;
    
private:
    NPC* npc_;                   // Ponteiro para o NPC
    std::array<float, 4> emotions_; // Níveis de emoção (medo, raiva, alegria, tristeza)
    std::array<float, 4> decayRates_; // Taxas de decaimento para cada emoção
    DWORD lastUpdateTime_;       // Timestamp da última atualização
    
    // Constantes
    static constexpr float PANIC_THRESHOLD = 0.75f;  // Limiar de pânico
    static constexpr float RAGE_THRESHOLD = 0.75f;   // Limiar de fúria
    
    // Funções auxiliares
    /**
 * DecayEmotions
 * 
 * Implementa a funcionalidade DecayEmotions conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void DecayEmotions(DWORD deltaTime);
    /**
 * ApplyEmotionalEffects
 * 
 * Implementa a funcionalidade ApplyEmotionalEffects conforme especificação original.
 */

    void ApplyEmotionalEffects();
};

/**
 * @brief Implementação avançada do controlador de AI para NPCs
 */
/**
 * Classe WYDAIController
 * 
 * Esta classe implementa a funcionalidade WYDAIController conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AIController {
public:
    /**
     * @brief Construtor
     * @param npc Ponteiro para o NPC que este controlador vai gerenciar
     */
    AIController(NPC* npc);
    
    /**
     * @brief Destrutor
     */
    ~AIController();
    
    /**
     * @brief Inicializa o controlador
     * @param aiType Tipo de AI
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(BYTE aiType);
    
    /**
     * @brief Atualiza a AI
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    /**
     * @brief Processa um evento
     * @param eventType Tipo de evento
     * @param param1 Primeiro parâmetro (opcional)
     * @param param2 Segundo parâmetro (opcional)
     * @param param3 Terceiro parâmetro (opcional)
     */
    void ProcessEvent(SensoryEventType eventType, DWORD param1 = 0, DWORD param2 = 0, DWORD param3 = 0);
    
    /**
     * @brief Muda o estado da AI
     * @param newState Novo estado
     */
    void ChangeState(AIState newState);
    
    /**
     * @brief Define o tipo de AI
     * @param aiType Tipo de AI
     */
    void SetAIType(BYTE aiType);
    
    /**
     * @brief Obtém o tipo de AI
     * @return Tipo de AI
     */
    BYTE GetAIType() const { return aiType_; }
    
    /**
     * @brief Obtém o estado atual da AI
     * @return Estado atual
     */
    AIState GetCurrentState() const { return currentState_; }
    
    /**
     * @brief Obtém o NPC associado
     * @return Ponteiro para o NPC
     */
    NPC* GetNPC() const { return npc_; }
    
    /**
     * @brief Define um parâmetro de comportamento
     * @param name Nome do parâmetro
     * @param value Valor do parâmetro
     */
    void SetParameter(const std::string& name, float value);
    
    /**
     * @brief Obtém um parâmetro de comportamento
     * @param name Nome do parâmetro
     * @param defaultValue Valor padrão se o parâmetro não existir
     * @return Valor do parâmetro
     */
    float GetParameter(const std::string& name, float defaultValue = 0.0f) const;
    
    /**
     * @brief Adiciona um comportamento
     * @param name Nome do comportamento
     * @param priority Prioridade do comportamento
     */
    void AddBehavior(const std::string& name, BYTE priority);
    
    /**
     * @brief Remove um comportamento
     * @param name Nome do comportamento
     */
    void RemoveBehavior(const std::string& name);
    
    /**
     * @brief Verifica se um comportamento está ativo
     * @param name Nome do comportamento
     * @return true se o comportamento está ativo, false caso contrário
     */
    bool HasBehavior(const std::string& name) const;
    
    /**
     * @brief Obtém o sistema de percepção
     * @return Ponteiro para o sistema de percepção
     */
    PerceptionSystem* GetPerceptionSystem() const { return perceptionSystem_.get(); }
    
    /**
     * @brief Obtém o sistema emocional
     * @return Ponteiro para o sistema emocional
     */
    EmotionalSystem* GetEmotionalSystem() const { return emotionalSystem_.get(); }
    
    /**
     * @brief Escolhe a melhor habilidade para a situação atual
     * @return ID da habilidade escolhida (0 se nenhuma for apropriada)
     */
    WORD ChooseBestSkill();
    
    /**
     * @brief Escolhe o melhor alvo para a situação atual
     * @return ID do alvo escolhido (0 se nenhum for apropriado)
     */
    DWORD ChooseBestTarget();
    
    /**
     * @brief Avalia se deve fugir da situação atual
     * @return true se deve fugir, false caso contrário
     */
    bool ShouldFlee();
    
    /**
     * @brief Avalia se deve chamar por ajuda
     * @return true se deve chamar por ajuda, false caso contrário
     */
    bool ShouldCallForHelp();
    
    /**
     * @brief Obtém o pathfinder
     * @return Ponteiro para o pathfinder
     */
    AStarPathfinder* GetPathfinder() const { return pathfinder_.get(); }
    
    /**
     * @brief Calcula um caminho até um alvo
     * @param targetX Posição X alvo
     * @param targetY Posição Y alvo
     * @return true se um caminho foi encontrado, false caso contrário
     */
    bool CalculatePathToTarget(WORD targetX, WORD targetY);
    
    /**
     * @brief Obtém o próximo ponto no caminho
     * @param outX Posição X de saída
     * @param outY Posição Y de saída
     * @return true se há um próximo ponto, false caso contrário
     */
    bool GetNextPathPoint(WORD& outX, WORD& outY);
    
    /**
     * @brief Verifica se o NPC está no alcance de ataque de um alvo
     * @param targetId ID do alvo
     * @return true se está no alcance, false caso contrário
     */
    bool IsInAttackRange(DWORD targetId);
    
    /**
     * @brief Verifica se o NPC está encurralado
     * @return true se está encurralado, false caso contrário
     */
    bool IsCornerTrapped() const;
    
    /**
     * @brief Verifica se o NPC está cercado
     * @return true se está cercado, false caso contrário
     */
    bool IsSurrounded() const;
    
    /**
     * @brief Encontra a melhor posição de fuga
     * @param outX Posição X de saída
     * @param outY Posição Y de saída
     * @return true se encontrou uma posição, false caso contrário
     */
    bool FindBestFleePosition(WORD& outX, WORD& outY);
    
    /**
     * @brief Encontra a melhor posição para ataque à distância
     * @param targetId ID do alvo
     * @param outX Posição X de saída
     * @param outY Posição Y de saída
     * @return true se encontrou uma posição, false caso contrário
     */
    bool FindBestRangedPosition(DWORD targetId, WORD& outX, WORD& outY);
    
    /**
     * @brief Calcula a melhor formação para um grupo
     * @param allies Lista de IDs de aliados
     * @param targetId ID do alvo
     * @param outPositions Posições de saída para cada aliado
     * @return true se a formação foi calculada, false caso contrário
     */
    bool CalculateGroupFormation(
        const std::vector<DWORD>& allies,
        DWORD targetId,
        std::vector<std::pair<WORD, WORD>>& outPositions);
    
    /**
     * @brief Registra um callback para eventos
     * @param eventType Tipo de evento
     * @param callback Função de callback
     */
    void RegisterEventCallback(
        SensoryEventType eventType,
        std::function<void(DWORD, DWORD, DWORD)> callback);
    
private:
    // Membros principais
    NPC* npc_;                                   // Ponteiro para o NPC
    AIState currentState_;                       // Estado atual da AI
    BYTE aiType_;                                // Tipo de AI
    
    // Sistemas auxiliares
    std::unique_ptr<PerceptionSystem> perceptionSystem_;  // Sistema de percepção
    std::unique_ptr<EmotionalSystem> emotionalSystem_;    // Sistema emocional
    std::unique_ptr<BehaviorNode> behaviorTree_;          // Árvore de comportamento
    std::unique_ptr<AStarPathfinder> pathfinder_;         // Pathfinder
    
    // Parâmetros de comportamento
    std::unordered_map<std::string, float> parameters_;   // Parâmetros
    std::unordered_map<std::string, BYTE> behaviors_;     // Comportamentos
    
    // Pathfinding
    std::vector<std::pair<WORD, WORD>> currentPath_;      // Caminho atual
    size_t currentPathIndex_;                            // Índice atual no caminho
    DWORD lastPathCalculation_;                          // Timestamp do último cálculo de caminho
    
    // Tomada de decisão
    DWORD lastDecisionTime_;                             // Timestamp da última decisão
    DWORD lastTargetEvaluation_;                         // Timestamp da última avaliação de alvo
    DWORD lastSkillEvaluation_;                          // Timestamp da última avaliação de habilidade
    DWORD lastStateChangeTime_;                          // Timestamp da última mudança de estado
    std::unordered_map<DWORD, float> targetScores_;      // Scores de alvos
    
    // Callbacks de eventos
    std::unordered_map<SensoryEventType, std::vector<std::function<void(DWORD, DWORD, DWORD)>>> eventCallbacks_;
    
    // Sistema de randomização
    std::mt19937 rng_;                                   // Gerador de números aleatórios
    
    // Métodos privados
    
    /**
     * @brief Inicializa a árvore de comportamento
     * @param aiType Tipo de AI
     * @return Ponteiro para o nó raiz da árvore
     */
    std::unique_ptr<BehaviorNode> InitializeBehaviorTree(BYTE aiType);
    
    /**
     * @brief Atualiza o estado baseado no estado emocional
     */
    void UpdateEmotionalState();
    
    /**
     * @brief Atualiza informações sobre alvos
     */
    void UpdateTargetInformation();
    
    /**
     * @brief Avalia todos os alvos potenciais
     */
    void EvaluateTargets();
    
    /**
     * @brief Atualiza o estado da AI para combate
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void UpdateCombatState(DWORD deltaTime);
    
    /**
     * @brief Atualiza o estado da AI para patrulha
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void UpdatePatrolState(DWORD deltaTime);
    
    /**
     * @brief Atualiza o estado da AI para fuga
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void UpdateFleeState(DWORD deltaTime);
    
    /**
     * @brief Atualiza o estado da AI para idle
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void UpdateIdleState(DWORD deltaTime);
    
    /**
     * @brief Verifica a melhor ação atual
     * @return true se decidiu por uma nova ação, false caso contrário
     */
    bool DecideBestAction();
    
    /**
     * @brief Verifica se está preso em um local
     * @return true se está preso, false caso contrário
     */
    bool IsStuck() const;
    
    /**
     * @brief Evento de mudança de estado
     * @param oldState Estado anterior
     * @param newState Novo estado
     */
    void OnStateChange(AIState oldState, AIState newState);
    
    /**
     * @brief Calcula o valor tático de uma posição
     * @param x Posição X
     * @param y Posição Y
     * @param targetId ID do alvo
     * @return Valor tático (maior é melhor)
     */
    float CalculatePositionTacticalValue(WORD x, WORD y, DWORD targetId) const;
    
    /**
     * @brief Obtém o timestamp atual
     * @return Timestamp atual em milissegundos
     */
    DWORD GetTickCount() const;
    
    /**
     * @brief Gera um número aleatório
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Número aleatório
     */
    int GetRandomInt(int min, int max);
    
    /**
     * @brief Gera um número aleatório float
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Número aleatório
     */
    float GetRandomFloat(float min, float max);
};

} // namespace server
} // namespace wyd

#endif // WYD_AI_H

} // namespace wydbr
