/**
 * MobAI.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/entity/MobAI.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef MOBAI_H
#define MOBAI_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "../../../../core/WYDTypes.h"
#include "../WorldTypes.h"

namespace wydbr {


/**
 * @file MobAI.h
 * @brief Sistema de inteligência artificial para mobs
 * 
 * Este arquivo contém a definição do sistema de inteligência artificial
 * para mobs. No WYD original, os mobs tinham comportamentos muito simples
 * e previsíveis. Esta implementação oferece um sistema mais avançado e flexível.
 */

namespace wyd {
namespace server {
namespace world {
namespace entity {

// Forward declarations
class CMob;

/**
 * @brief Estados de AI
 */
enum class AIState : BYTE {
    IDLE = 0,           // Ocioso
    PATROL = 1,         // Patrulhando
    CHASE = 2,          // Perseguindo
    ATTACK = 3,         // Atacando
    FLEE = 4,           // Fugindo
    ASSIST = 5,         // Ajudando
    RETURN = 6,         // Retornando
    CASTING = 7,        // Conjurando
    STUNNED = 8,        // Atordoado
    DEAD = 9,           // Morto
    SCRIPT = 10,        // Controlado por script
    CUSTOM = 11,        // Personalizado
};

/**
 * @brief Eventos de AI
 */
enum class AIEvent : BYTE {
    NONE = 0,           // Nenhum
    TARGET_DETECTED = 1,// Alvo detectado
    ATTACKED = 2,       // Atacado
    HEALTH_LOW = 3,     // Saúde baixa
    TARGET_DIED = 4,    // Alvo morreu
    TARGET_LOST = 5,    // Alvo perdido
    ALLY_ATTACKED = 6,  // Aliado atacado
    SPELL_READY = 7,    // Spell pronta
    SPAWN = 8,          // Spawn
    DEATH = 9,          // Morte
    RESPAWN = 10,       // Respawn
    TIMER = 11,         // Timer
    COMMAND = 12,       // Comando
    CUSTOM = 13,        // Personalizado
};

/**
 * @brief Tipo de ação de AI
 */
enum class AIActionType : BYTE {
    NONE = 0,           // Nenhuma
    MOVE_TO = 1,        // Mover para
    ATTACK = 2,         // Atacar
    USE_SKILL = 3,      // Usar skill
    FLEE = 4,           // Fugir
    CHANGE_STATE = 5,   // Mudar estado
    CALL_FOR_HELP = 6,  // Chamar por ajuda
    SCRIPT = 7,         // Executar script
    SPAWN_MOB = 8,      // Spawnar mob
    SET_VARIABLE = 9,   // Definir variável
    WAIT = 10,          // Esperar
    CUSTOM = 11,        // Personalizado
};

/**
 * @brief Parâmetros de ação de AI
 */
struct AIActionParams {
    AIActionType type;              // Tipo de ação
    
    union {
        struct {                    // Para MOVE_TO
            WORD posX;
            WORD posY;
            WORD mapID;
        } move;
        
        struct {                    // Para ATTACK
            DWORD targetID;
        } attack;
        
        struct {                    // Para USE_SKILL
            WORD skillID;
            DWORD targetID;
        } skill;
        
        struct {                    // Para FLEE
            WORD distance;
            DWORD targetID;
        } flee;
        
        struct {                    // Para CHANGE_STATE
            AIState newState;
        } state;
        
        struct {                    // Para CALL_FOR_HELP
            DWORD range;
            WORD mobID;
        } help;
        
        struct {                    // Para SCRIPT
            char script[32];
        } script;
        
        struct {                    // Para SPAWN_MOB
            WORD mobID;
            WORD count;
            WORD range;
        } spawn;
        
        struct {                    // Para SET_VARIABLE
            char name[16];
            int value;
        } variable;
        
        struct {                    // Para WAIT
            DWORD duration;
        } wait;
        
        struct {                    // Para CUSTOM
            char data[32];
        } custom;
    };
    
    AIActionParams()
        /**
 * type
 * 
 * Implementa a funcionalidade type conforme especificação original.
 * @param AIActionType::NONE Parâmetro AIActionType::NONE
 * @return Retorna :
 */

        : type(AIActionType::NONE){
        memset(&move, 0, sizeof(move));
    }
};

/**
 * @brief Resultado de ação de AI
 */
enum class AIActionResult : BYTE {
    SUCCESS = 0,        // Sucesso
    FAILURE = 1,        // Falha
    RUNNING = 2,        // Em execução
    ERROR = 3,          // Erro
};

/**
 * @brief Transição de estado de AI
 */
struct AIStateTransition {
    AIState fromState;              // Estado de origem
    AIEvent event;                  // Evento
    AIState toState;                // Estado de destino
    AIActionParams action;          // Ação a executar na transição
    
    AIStateTransition()
        : fromState(AIState::IDLE)
        , event(AIEvent::NONE)
        , toState(AIState::IDLE)
    {
    }
};

/**
 * @brief Definição de comportamento de AI
 */
struct AIBehavior {
    std::string name;                                  // Nome do comportamento
    std::vector<AIStateTransition> transitions;        // Transições de estado
    std::unordered_map<AIState, AIActionParams> stateActions; // Ações por estado
    
    AIBehavior()
        /**
 * name
 * 
 * Implementa a funcionalidade name conforme especificação original.
 * @param "Default" Parâmetro "Default"
 * @return Retorna :
 */

        : name("Default"){
    }
};

/**
 * @brief Classe base para AI de mobs
 * 
 * Esta classe define a interface comum para todas as AI de mobs.
 */
/**
 * Classe WYDMobAI
 * 
 * Esta classe implementa a funcionalidade WYDMobAI conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CMobAI {
public:
    /**
     * @brief Construtor
     * @param behaviors Comportamentos
     */
    CMobAI(const std::vector<AIBehavior>& behaviors = {});
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~CMobAI();
    
    /**
     * @brief Inicializa a AI
     * @param mob Mob associado
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(CMob* mob);
    
    /**
     * @brief Atualiza a AI
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void Update(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Processa um evento
     * @param mob Mob associado
     * @param event Evento
     * @param param Parâmetro adicional
     * @return true se processado com sucesso
     */
    virtual bool ProcessEvent(CMob* mob, AIEvent event, void* param = nullptr);
    
    /**
     * @brief Processa uma transição de estado
     * @param mob Mob associado
     * @param transition Transição
     * @return Resultado da ação
     */
    virtual AIActionResult ProcessTransition(CMob* mob, const AIStateTransition& transition);
    
    /**
     * @brief Executa uma ação
     * @param mob Mob associado
     * @param action Parâmetros da ação
     * @return Resultado da ação
     */
    virtual AIActionResult ExecuteAction(CMob* mob, const AIActionParams& action);
    
    /**
     * @brief Verifica se pode executar uma ação
     * @param mob Mob associado
     * @param action Parâmetros da ação
     * @return true se puder executar
     */
    virtual bool CanExecuteAction(CMob* mob, const AIActionParams& action);
    
    /**
     * @brief Obtém o estado atual
     * @return Estado atual
     */
    AIState GetState() const;
    
    /**
     * @brief Define o estado atual
     * @param state Estado
     * @param force Se deve forçar a mudança
     * @return true se definido com sucesso
     */
    bool SetState(AIState state, bool force = false);
    
    /**
     * @brief Obtém as transições para um estado
     * @param state Estado
     * @return Lista de transições
     */
    std::vector<AIStateTransition> GetTransitionsFromState(AIState state) const;
    
    /**
     * @brief Obtém a ação para um estado
     * @param state Estado
     * @return Parâmetros da ação
     */
    AIActionParams GetActionForState(AIState state) const;
    
    /**
     * @brief Adiciona um comportamento
     * @param behavior Comportamento
     * @return true se adicionado com sucesso
     */
    bool AddBehavior(const AIBehavior& behavior);
    
    /**
     * @brief Remove um comportamento
     * @param name Nome do comportamento
     * @return true se removido com sucesso
     */
    bool RemoveBehavior(const std::string& name);
    
    /**
     * @brief Ativa um comportamento
     * @param name Nome do comportamento
     * @return true se ativado com sucesso
     */
    bool ActivateBehavior(const std::string& name);
    
    /**
     * @brief Obtém o comportamento atual
     * @return Nome do comportamento
     */
    std::string GetCurrentBehavior() const;
    
    /**
     * @brief Define uma variável
     * @param name Nome da variável
     * @param value Valor
     */
    void SetVariable(const std::string& name, int value);
    
    /**
     * @brief Obtém uma variável
     * @param name Nome da variável
     * @param defaultValue Valor padrão
     * @return Valor da variável
     */
    int GetVariable(const std::string& name, int defaultValue = 0) const;
    
    /**
     * @brief Incrementa uma variável
     * @param name Nome da variável
     * @param amount Quantidade a incrementar
     * @return Novo valor
     */
    int IncrementVariable(const std::string& name, int amount = 1);
    
    /**
     * @brief Verifica se uma variável existe
     * @param name Nome da variável
     * @return true se existir
     */
    bool HasVariable(const std::string& name) const;
    
protected:
    /**
     * @brief Atualiza o estado IDLE
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateIdleState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado PATROL
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdatePatrolState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado CHASE
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateChaseState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado ATTACK
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateAttackState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado FLEE
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateFleeState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado ASSIST
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateAssistState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado RETURN
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateReturnState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado CASTING
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateCastingState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado STUNNED
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateStunnedState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado DEAD
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateDeadState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado SCRIPT
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateScriptState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Atualiza o estado CUSTOM
     * @param mob Mob associado
     * @param elapsed Tempo decorrido
     */
    virtual void UpdateCustomState(CMob* mob, DWORD elapsed);
    
    /**
     * @brief Troca para um novo estado
     * @param mob Mob associado
     * @param newState Novo estado
     * @param executeAction Se deve executar a ação do estado
     * @return true se trocado com sucesso
     */
    bool SwitchState(CMob* mob, AIState newState, bool executeAction = true);
    
    // Estado atual
    AIState currentState_;
    
    // Comportamentos disponíveis
    std::vector<AIBehavior> behaviors_;
    
    // Índice do comportamento atual
    int currentBehaviorIndex_;
    
    // Variáveis
    std::unordered_map<std::string, int> variables_;
    
    // Timestamp da última atualização
    DWORD lastUpdateTime_;
    
    // Última posição de patrulha
    WORD lastPatrolX_;
    WORD lastPatrolY_;
    
    // Alvo atual
    DWORD targetID_;
    
    // Tempo restante da ação atual
    DWORD actionTimeRemaining_;
    
    // Estado de inicialização
    bool initialized_;
};

/**
 * @brief Fábrica de AI de mobs
 * 
 * Esta classe é responsável por criar AIs de mobs com base em templates.
 */
/**
 * Classe WYDMobAIFactory
 * 
 * Esta classe implementa a funcionalidade WYDMobAIFactory conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CMobAIFactory {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static CMobAIFactory& GetInstance();
    
    /**
     * @brief Cria uma AI para um mob
     * @param mob Mob
     * @return AI criada
     */
    std::shared_ptr<CMobAI> CreateAI(CMob* mob);
    
    /**
     * @brief Cria uma AI a partir de um script
     * @param scriptName Nome do script
     * @return AI criada
     */
    std::shared_ptr<CMobAI> CreateAIFromScript(const std::string& scriptName);
    
    /**
     * @brief Cria uma AI com base em um tipo
     * @param type Tipo de AI
     * @return AI criada
     */
    std::shared_ptr<CMobAI> CreateAIByType(const std::string& type);
    
    /**
     * @brief Registra uma fábrica de AI
     * @param type Tipo de AI
     * @param factory Função de fábrica
     * @return true se registrado com sucesso
     */
    bool RegisterAIFactory(const std::string& type, std::function<std::shared_ptr<CMobAI>()> factory);
    
    /**
     * @brief Remove uma fábrica de AI
     * @param type Tipo de AI
     * @return true se removido com sucesso
     */
    bool UnregisterAIFactory(const std::string& type);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CMobAIFactory();
    
    /**
     * @brief Destrutor
     */
    ~CMobAIFactory();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CMobAIFactory(const CMobAIFactory&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CMobAIFactory& operator=(const CMobAIFactory&) = delete;
    
    /**
     * @brief Cria uma AI padrão
     * @return AI padrão
     */
    std::shared_ptr<CMobAI> CreateDefaultAI();
    
    /**
     * @brief Cria uma AI agressiva
     * @return AI agressiva
     */
    std::shared_ptr<CMobAI> CreateAggressiveAI();
    
    /**
     * @brief Cria uma AI defensiva
     * @return AI defensiva
     */
    std::shared_ptr<CMobAI> CreateDefensiveAI();
    
    /**
     * @brief Cria uma AI passiva
     * @return AI passiva
     */
    std::shared_ptr<CMobAI> CreatePassiveAI();
    
    /**
     * @brief Cria uma AI de chefe
     * @return AI de chefe
     */
    std::shared_ptr<CMobAI> CreateBossAI();
    
    /**
     * @brief Carrega scripts de AI
     */
    void LoadAIScripts();
    
    // Funções de fábrica registradas
    std::unordered_map<std::string, std::function<std::shared_ptr<CMobAI>()>> factories_;
    
    // Scripts de AI carregados
    std::unordered_map<std::string, AIBehavior> scripts_;
};

// Acesso global
#define g_MobAIFactory CMobAIFactory::GetInstance()

} // namespace entity
} // namespace world
} // namespace server
} // namespace wyd

#endif // MOBAI_H

} // namespace wydbr
