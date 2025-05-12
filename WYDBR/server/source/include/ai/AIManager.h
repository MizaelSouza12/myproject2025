/**
 * AIManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/ai/AIManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _AI_MANAGER_H_
#define _AI_MANAGER_H_

/**
 * @file AIManager.h
 * @brief Gerenciador de inteligência artificial do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de inteligência artificial do WYDBR,
 * responsável pelo comportamento dos NPCs e monstros do jogo,
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
#include <random>

#include "../GlobalDef.h"
#include "../Basedef.h"

namespace wydbr {
namespace ai {

/**
 * @brief Tipo de cérebro (brain) de IA
 */
enum /**
 * Classe WYDAIBrainType
 * 
 * Esta classe implementa a funcionalidade WYDAIBrainType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AIBrainType {
    NONE = 0,               // Nenhum
    PASSIVE = 1,            // Passivo
    AGGRESSIVE = 2,         // Agressivo
    DEFENSIVE = 3,          // Defensivo
    SUPPORT = 4,            // Suporte
    COWARD = 5,             // Covarde
    BERSERKER = 6,          // Furioso
    SCOUT = 7,              // Explorador
    GUARD = 8,              // Guardião
    PATROL = 9,             // Patrulha
    WANDER = 10,            // Vaguear
    STATIC = 11,            // Estático
    FOLLOWER = 12,          // Seguidor
    SUMMONED = 13,          // Invocado
    BOSS = 14,              // Chefe
    ELITE = 15,             // Elite
    TRAINING = 16,          // Treinamento
    MERCHANT = 17,          // Mercador
    VILLAGER = 18,          // Aldeão
    QUEST = 19,             // Missão
    CUSTOM_1 = 20,          // Personalizado 1
    CUSTOM_2 = 21,          // Personalizado 2
    CUSTOM_3 = 22,          // Personalizado 3
    UNKNOWN = 23            // Desconhecido
};

/**
 * @brief Tipo de evento de IA
 */
enum /**
 * Classe WYDAIEventType
 * 
 * Esta classe implementa a funcionalidade WYDAIEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AIEventType {
    NONE = 0,               // Nenhum
    INIT = 1,               // Inicialização
    UPDATE = 2,             // Atualização
    DESTROY = 3,            // Destruição
    TARGET_FOUND = 4,       // Alvo encontrado
    TARGET_LOST = 5,        // Alvo perdido
    TARGET_CHANGED = 6,     // Alvo alterado
    ATTACKED = 7,           // Atacado
    DAMAGED = 8,            // Danificado
    HEALTH_LOW = 9,         // Saúde baixa
    HEALTH_CRITICAL = 10,   // Saúde crítica
    HEALTH_RECOVERED = 11,  // Saúde recuperada
    MANA_LOW = 12,          // Mana baixo
    MANA_CRITICAL = 13,     // Mana crítico
    MANA_RECOVERED = 14,    // Mana recuperado
    SKILL_USED = 15,        // Habilidade usada
    SKILL_COOLDOWN = 16,    // Cooldown de habilidade
    MOVEMENT_BLOCKED = 17,  // Movimento bloqueado
    STATE_CHANGED = 18,     // Estado alterado
    ALLY_DAMAGED = 19,      // Aliado danificado
    ALLY_KILLED = 20,       // Aliado morto
    ENEMY_KILLED = 21,      // Inimigo morto
    RESOURCE_FOUND = 22,    // Recurso encontrado
    THREAT_CHANGED = 23,    // Ameaça alterada
    PATH_FOUND = 24,        // Caminho encontrado
    PATH_BLOCKED = 25,      // Caminho bloqueado
    AREA_ENTERED = 26,      // Área entrada
    AREA_EXITED = 27,       // Área saída
    FLEE = 28,              // Fugir
    RETURN_HOME = 29,       // Retornar para casa
    REINFORCEMENT_CALLED = 30, // Reforço chamado
    REINFORCEMENT_ARRIVED = 31, // Reforço chegou
    SCRIPTED_TRIGGER = 32,  // Gatilho de script
    CUSTOM_1 = 33,          // Personalizado 1
    CUSTOM_2 = 34,          // Personalizado 2
    CUSTOM_3 = 35,          // Personalizado 3
    UNKNOWN = 36            // Desconhecido
};

/**
 * @brief Tipo de comportamento de IA
 */
enum /**
 * Classe WYDAIBehaviorType
 * 
 * Esta classe implementa a funcionalidade WYDAIBehaviorType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AIBehaviorType {
    NONE = 0,               // Nenhum
    IDLE = 1,               // Ocioso
    WANDER = 2,             // Vaguear
    PATROL = 3,             // Patrulhar
    FOLLOW = 4,             // Seguir
    ATTACK = 5,             // Atacar
    FLEE = 6,               // Fugir
    HEAL = 7,               // Curar
    BUFF = 8,               // Buffar
    DEBUFF = 9,             // Debuffar
    RANGED_ATTACK = 10,     // Ataque à distância
    MELEE_ATTACK = 11,      // Ataque corpo a corpo
    AOE_ATTACK = 12,        // Ataque em área
    CHARGE = 13,            // Investida
    DEFEND = 14,            // Defender
    SUMMON = 15,            // Invocar
    CAST_SPELL = 16,        // Lançar feitiço
    TAUNT = 17,             // Provocar
    GATHER = 18,            // Coletar
    SEARCH = 19,            // Procurar
    EVADE = 20,             // Evadir
    CUSTOM_1 = 21,          // Personalizado 1
    CUSTOM_2 = 22,          // Personalizado 2
    CUSTOM_3 = 23,          // Personalizado 3
    UNKNOWN = 24            // Desconhecido
};

/**
 * @brief Tipo de estado de IA
 */
enum /**
 * Classe WYDAIState
 * 
 * Esta classe implementa a funcionalidade WYDAIState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AIState {
    NONE = 0,               // Nenhum
    IDLE = 1,               // Ocioso
    ALERT = 2,              // Alerta
    CHASE = 3,              // Perseguir
    ATTACK = 4,             // Atacar
    FLEE = 5,               // Fugir
    HEAL = 6,               // Curar
    WANDER = 7,             // Vaguear
    PATROL = 8,             // Patrulhar
    FOLLOW = 9,             // Seguir
    RETURN = 10,            // Retornar
    DEFEND = 11,            // Defender
    GATHER = 12,            // Coletar
    REST = 13,              // Descansar
    SEARCH = 14,            // Procurar
    DEAD = 15,              // Morto
    STUNNED = 16,           // Atordoado
    FEARED = 17,            // Amedrontado
    CONTROLLED = 18,        // Controlado
    CUSTOMIZED = 19,        // Personalizado
    UNKNOWN = 20            // Desconhecido
};

/**
 * @brief Tipo de parâmetro de IA
 */
enum /**
 * Classe WYDAIParameterType
 * 
 * Esta classe implementa a funcionalidade WYDAIParameterType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AIParameterType {
    NONE = 0,               // Nenhum
    AGGRESSION = 1,         // Agressão
    COURAGE = 2,            // Coragem
    LOYALTY = 3,            // Lealdade
    INTELLIGENCE = 4,       // Inteligência
    PERCEPTION = 5,         // Percepção
    TEAMWORK = 6,           // Trabalho em equipe
    RESILIENCE = 7,         // Resistência
    ADAPTABILITY = 8,       // Adaptabilidade
    CAUTION = 9,            // Cautela
    HUNTING = 10,           // Caça
    SOCIAL = 11,            // Social
    PATIENCE = 12,          // Paciência
    AGGRESSION_RADIUS = 13, // Raio de agressão
    CHASE_RADIUS = 14,      // Raio de perseguição
    FLEE_HEALTH = 15,       // Saúde para fugir
    HELP_RADIUS = 16,       // Raio de ajuda
    PATROL_RADIUS = 17,     // Raio de patrulha
    WANDER_RADIUS = 18,     // Raio de vagar
    ATTACK_RANGE = 19,      // Alcance de ataque
    SIGHT_RANGE = 20,       // Alcance de visão
    CHASE_DURATION = 21,    // Duração de perseguição
    ATTACK_DELAY = 22,      // Atraso de ataque
    MOVEMENT_SPEED = 23,    // Velocidade de movimento
    ATTACK_SPEED = 24,      // Velocidade de ataque
    CAST_SPEED = 25,        // Velocidade de lançamento
    REACTION_TIME = 26,     // Tempo de reação
    CUSTOM_1 = 27,          // Personalizado 1
    CUSTOM_2 = 28,          // Personalizado 2
    CUSTOM_3 = 29,          // Personalizado 3
    UNKNOWN = 30            // Desconhecido
};

/**
 * @brief Tipo de seleção de alvo
 */
enum /**
 * Classe WYDAITargetSelectionType
 * 
 * Esta classe implementa a funcionalidade WYDAITargetSelectionType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AITargetSelectionType {
    NONE = 0,                  // Nenhum
    NEAREST = 1,               // Mais próximo
    LOWEST_HEALTH = 2,         // Menor saúde
    HIGHEST_HEALTH = 3,        // Maior saúde
    LOWEST_LEVEL = 4,          // Menor nível
    HIGHEST_LEVEL = 5,         // Maior nível
    LOWEST_DEFENSE = 6,        // Menor defesa
    HIGHEST_DEFENSE = 7,       // Maior defesa
    LOWEST_ATTACK = 8,         // Menor ataque
    HIGHEST_ATTACK = 9,        // Maior ataque
    RANDOM = 10,               // Aleatório
    MOST_THREATENING = 11,     // Mais ameaçador
    LEAST_THREATENING = 12,    // Menos ameaçador
    MOST_RECENT_ATTACKER = 13, // Atacante mais recente
    HEALER = 14,               // Curador
    MAGE = 15,                 // Mago
    TANK = 16,                 // Tanque
    CUSTOM_1 = 17,             // Personalizado 1
    CUSTOM_2 = 18,             // Personalizado 2
    CUSTOM_3 = 19,             // Personalizado 3
    UNKNOWN = 20               // Desconhecido
};

/**
 * @brief Tipo de seleção de habilidade
 */
enum /**
 * Classe WYDAISkillSelectionType
 * 
 * Esta classe implementa a funcionalidade WYDAISkillSelectionType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AISkillSelectionType {
    NONE = 0,               // Nenhum
    HIGHEST_DAMAGE = 1,     // Maior dano
    HIGHEST_THREAT = 2,     // Maior ameaça
    LOWEST_COOLDOWN = 3,    // Menor cooldown
    LOWEST_COST = 4,        // Menor custo
    RANDOM = 5,             // Aleatório
    PRIORITY = 6,           // Prioridade
    SEQUENCE = 7,           // Sequência
    SITUATIONAL = 8,        // Situacional
    CUSTOM_1 = 9,           // Personalizado 1
    CUSTOM_2 = 10,          // Personalizado 2
    CUSTOM_3 = 11,          // Personalizado 3
    UNKNOWN = 12            // Desconhecido
};

/**
 * @brief Tipo de condição de IA
 */
enum /**
 * Classe WYDAIConditionType
 * 
 * Esta classe implementa a funcionalidade WYDAIConditionType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AIConditionType {
    NONE = 0,               // Nenhum
    HEALTH_BELOW = 1,       // Saúde abaixo
    HEALTH_ABOVE = 2,       // Saúde acima
    MANA_BELOW = 3,         // Mana abaixo
    MANA_ABOVE = 4,         // Mana acima
    TARGET_HEALTH_BELOW = 5, // Saúde do alvo abaixo
    TARGET_HEALTH_ABOVE = 6, // Saúde do alvo acima
    TARGET_DISTANCE_BELOW = 7, // Distância do alvo abaixo
    TARGET_DISTANCE_ABOVE = 8, // Distância do alvo acima
    TARGET_COUNT_BELOW = 9, // Contagem de alvos abaixo
    TARGET_COUNT_ABOVE = 10, // Contagem de alvos acima
    ALLY_COUNT_BELOW = 11,  // Contagem de aliados abaixo
    ALLY_COUNT_ABOVE = 12,  // Contagem de aliados acima
    SKILL_AVAILABLE = 13,   // Habilidade disponível
    SKILL_COOLDOWN = 14,    // Cooldown de habilidade
    TIME_ELAPSED = 15,      // Tempo decorrido
    CUSTOM_1 = 16,          // Personalizado 1
    CUSTOM_2 = 17,          // Personalizado 2
    CUSTOM_3 = 18,          // Personalizado 3
    UNKNOWN = 19            // Desconhecido
};

/**
 * @brief Tipo de resposta de IA
 */
enum /**
 * Classe WYDAIResponseType
 * 
 * Esta classe implementa a funcionalidade WYDAIResponseType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AIResponseType {
    NONE = 0,               // Nenhum
    FLEE = 1,               // Fugir
    HEAL_SELF = 2,          // Curar a si mesmo
    HEAL_ALLY = 3,          // Curar aliado
    BUFF_SELF = 4,          // Buffar a si mesmo
    BUFF_ALLY = 5,          // Buffar aliado
    DEBUFF_ENEMY = 6,       // Debuffar inimigo
    USE_SPECIAL_SKILL = 7,  // Usar habilidade especial
    CALL_FOR_HELP = 8,      // Chamar por ajuda
    TAUNT = 9,              // Provocar
    CHANGE_TACTIC = 10,     // Mudar tática
    CHANGE_TARGET = 11,     // Mudar alvo
    CHANGE_STATE = 12,      // Mudar estado
    CUSTOM_1 = 13,          // Personalizado 1
    CUSTOM_2 = 14,          // Personalizado 2
    CUSTOM_3 = 15,          // Personalizado 3
    UNKNOWN = 16            // Desconhecido
};

/**
 * @brief Parâmetro de IA
 */
struct AIParameter {
    AIParameterType Type;   // Tipo
    float Value;            // Valor
    
    AIParameter() : Type(AIParameterType::NONE), Value(0.0f) {}
    
    AIParameter(AIParameterType type, float value)
        : Type(type), Value(value) {}
};

/**
 * @brief Cérebro (brain) de IA
 */
struct AIBrain {
    uint32_t BrainID;                                   // ID do cérebro
    std::string Name;                                   // Nome
    AIBrainType Type;                                   // Tipo
    std::vector<AIParameter> Parameters;                // Parâmetros
    AITargetSelectionType TargetSelectionType;          // Tipo de seleção de alvo
    AISkillSelectionType SkillSelectionType;            // Tipo de seleção de habilidade
    std::map<AIState, std::vector<AIBehaviorType>> StateBehaviors; // Comportamentos de estado
    std::map<AIConditionType, AIResponseType> Responses; // Respostas a condições
    
    AIBrain() : BrainID(0), Type(AIBrainType::NONE),
        TargetSelectionType(AITargetSelectionType::NONE),
        SkillSelectionType(AISkillSelectionType::NONE) {}
};

/**
 * @brief Script de IA
 */
struct AIScript {
    uint32_t ScriptID;                  // ID do script
    std::string Name;                   // Nome
    std::string Description;            // Descrição
    std::string Source;                 // Código fonte
    uint32_t LastModified;              // Última modificação
    bool Compiled;                      // Compilado
    bool Enabled;                       // Habilitado
    
    AIScript() : ScriptID(0), LastModified(0), Compiled(false), Enabled(true) {}
};

/**
 * @brief Configuração de ponto de patrulha
 */
struct AIPatrolPoint {
    uint16_t X;                         // Posição X
    uint16_t Y;                         // Posição Y
    uint32_t WaitTime;                  // Tempo de espera
    
    AIPatrolPoint() : X(0), Y(0), WaitTime(0) {}
    
    AIPatrolPoint(uint16_t x, uint16_t y, uint32_t waitTime = 0)
        : X(x), Y(y), WaitTime(waitTime) {}
};

/**
 * @brief Configuração de patrulha
 */
struct AIPatrolConfig {
    uint32_t PatrolID;                  // ID da patrulha
    std::string Name;                   // Nome
    std::vector<AIPatrolPoint> Points;  // Pontos de patrulha
    bool Cyclic;                        // Cíclica
    bool Reverse;                       // Reversa
    uint32_t StartTime;                 // Tempo de início
    uint32_t EndTime;                   // Tempo de fim
    
    AIPatrolConfig() : PatrolID(0), Cyclic(true), Reverse(false),
        StartTime(0), EndTime(0) {}
};

/**
 * @brief Configuração de área territorial
 */
struct AITerritoryConfig {
    uint32_t TerritoryID;               // ID do território
    std::string Name;                   // Nome
    uint16_t CenterX;                   // Centro X
    uint16_t CenterY;                   // Centro Y
    uint16_t Radius;                    // Raio
    bool Aggressive;                    // Agressivo
    bool HomePoint;                     // Ponto inicial
    
    AITerritoryConfig() : TerritoryID(0), CenterX(0), CenterY(0),
        Radius(0), Aggressive(false), HomePoint(true) {}
};

/**
 * @brief Configuração de lista de habilidades
 */
struct AISkillList {
    uint32_t SkillListID;               // ID da lista
    std::string Name;                   // Nome
    std::vector<uint16_t> Skills;       // Habilidades
    std::vector<uint8_t> Priorities;    // Prioridades
    std::vector<uint8_t> Chances;       // Chances
    
    AISkillList() /**
 * SkillListID
 * 
 * Implementa a funcionalidade SkillListID conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 : SkillListID(0){}
};

/**
 * @brief Estado da IA de um mob
 */
struct AIMobState {
    uint32_t MobID;                     // ID do mob
    AIState CurrentState;               // Estado atual
    uint32_t CurrentTargetID;           // ID do alvo atual
    uint16_t HomeX;                     // Posição inicial X
    uint16_t HomeY;                     // Posição inicial Y
    uint32_t StateStartTime;            // Tempo de início do estado
    uint32_t LastUpdateTime;            // Último tempo de atualização
    uint32_t LastAttackTime;            // Último tempo de ataque
    uint32_t LastSkillTime;             // Último tempo de habilidade
    uint32_t LastMoveTime;              // Último tempo de movimento
    uint32_t LastTargetChangedTime;     // Último tempo de mudança de alvo
    uint32_t LastDamagedTime;           // Último tempo de dano recebido
    uint32_t LastFleeTime;              // Último tempo de fuga
    uint32_t ChaseStartTime;            // Tempo de início de perseguição
    uint16_t CurrentPatrolIndex;        // Índice de patrulha atual
    bool PatrolReverse;                 // Patrulha reversa
    std::map<uint32_t, uint32_t> ThreatTable; // Tabela de ameaça
    
    AIMobState() : MobID(0), CurrentState(AIState::IDLE), CurrentTargetID(0),
        HomeX(0), HomeY(0), StateStartTime(0), LastUpdateTime(0),
        LastAttackTime(0), LastSkillTime(0), LastMoveTime(0),
        LastTargetChangedTime(0), LastDamagedTime(0), LastFleeTime(0),
        ChaseStartTime(0), CurrentPatrolIndex(0), PatrolReverse(false) {}
};

/**
 * @brief Evento de IA
 */
struct AIEvent {
    uint32_t EventID;                   // ID do evento
    AIEventType Type;                   // Tipo
    uint32_t MobID;                     // ID do mob
    uint32_t TargetID;                  // ID do alvo
    AIState OldState;                   // Estado antigo
    AIState NewState;                   // Estado novo
    uint16_t PosX;                      // Posição X
    uint16_t PosY;                      // Posição Y
    uint32_t Data1;                     // Dados 1
    uint32_t Data2;                     // Dados 2
    uint32_t Timestamp;                 // Timestamp
    
    AIEvent() : EventID(0), Type(AIEventType::NONE), MobID(0), TargetID(0),
        OldState(AIState::NONE), NewState(AIState::NONE), PosX(0), PosY(0),
        Data1(0), Data2(0), Timestamp(0) {}
    
    AIEvent(AIEventType _type, uint32_t _mobID, uint32_t _targetID = 0,
           AIState _oldState = AIState::NONE, AIState _newState = AIState::NONE,
           uint16_t _posX = 0, uint16_t _posY = 0, uint32_t _data1 = 0, uint32_t _data2 = 0)
        : EventID(0), Type(_type), MobID(_mobID), TargetID(_targetID),
          OldState(_oldState), NewState(_newState), PosX(_posX), PosY(_posY),
          Data1(_data1), Data2(_data2), Timestamp(static_cast<uint32_t>(time(nullptr))) {}
};

/**
 * @brief Tipo de callback de evento de IA
 */
using AIEventCallback = std::function<void(const AIEvent&)>;

/**
 * @brief Tipo de callback de atualização de IA
 */
using AIUpdateCallback = std::function<void(uint32_t, AIMobState&, uint32_t)>;

/**
 * @brief Gerenciador de IA
 * 
 * Esta classe é responsável por gerenciar o comportamento dos NPCs e monstros do jogo.
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
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static AIManager& getInstance() {
        static AIManager instance;
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
     * @brief Carrega a configuração de cérebros (brains)
     * @return true se carregado com sucesso
     */
    bool loadBrainConfig();
    
    /**
     * @brief Carrega a configuração de scripts
     * @return true se carregado com sucesso
     */
    bool loadScriptConfig();
    
    /**
     * @brief Carrega a configuração de patrulhas
     * @return true se carregado com sucesso
     */
    bool loadPatrolConfig();
    
    /**
     * @brief Carrega a configuração de territórios
     * @return true se carregado com sucesso
     */
    bool loadTerritoryConfig();
    
    /**
     * @brief Carrega a configuração de listas de habilidades
     * @return true se carregado com sucesso
     */
    bool loadSkillListConfig();
    
    /**
     * @brief Registra um mob para controle de IA
     * @param mobID ID do mob
     * @param brainID ID do cérebro
     * @param scriptID ID do script
     * @param skillListID ID da lista de habilidades
     * @param patrolID ID da patrulha
     * @param territoryID ID do território
     * @param homeX Posição inicial X
     * @param homeY Posição inicial Y
     * @return true se registrado com sucesso
     */
    bool registerMob(uint32_t mobID, uint32_t brainID = 0, uint32_t scriptID = 0,
                    uint32_t skillListID = 0, uint32_t patrolID = 0, uint32_t territoryID = 0,
                    uint16_t homeX = 0, uint16_t homeY = 0);
    
    /**
     * @brief Remove um mob do controle de IA
     * @param mobID ID do mob
     * @return true se removido com sucesso
     */
    bool unregisterMob(uint32_t mobID);
    
    /**
     * @brief Atualiza um mob
     * @param mobID ID do mob
     * @param deltaTime Tempo decorrido em milissegundos
     * @return true se atualizado com sucesso
     */
    bool updateMob(uint32_t mobID, uint32_t deltaTime);
    
    /**
     * @brief Define o estado de um mob
     * @param mobID ID do mob
     * @param state Estado
     * @param targetID ID do alvo
     * @return true se definido com sucesso
     */
    bool setMobState(uint32_t mobID, AIState state, uint32_t targetID = 0);
    
    /**
     * @brief Obtém o estado de um mob
     * @param mobID ID do mob
     * @return Estado do mob
     */
    AIState getMobState(uint32_t mobID);
    
    /**
     * @brief Obtém o alvo atual de um mob
     * @param mobID ID do mob
     * @return ID do alvo ou 0 se não tiver alvo
     */
    uint32_t getMobTarget(uint32_t mobID);
    
    /**
     * @brief Define o alvo de um mob
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @return true se definido com sucesso
     */
    bool setMobTarget(uint32_t mobID, uint32_t targetID);
    
    /**
     * @brief Notifica um evento para um mob
     * @param event Evento
     * @return true se processado com sucesso
     */
    bool notifyEvent(const AIEvent& event);
    
    /**
     * @brief Adiciona ameaça a um mob
     * @param mobID ID do mob
     * @param characterID ID do personagem
     * @param amount Quantidade de ameaça
     * @return true se adicionado com sucesso
     */
    bool addThreat(uint32_t mobID, uint32_t characterID, uint32_t amount);
    
    /**
     * @brief Remove ameaça de um mob
     * @param mobID ID do mob
     * @param characterID ID do personagem
     * @param amount Quantidade de ameaça
     * @return true se removido com sucesso
     */
    bool removeThreat(uint32_t mobID, uint32_t characterID, uint32_t amount);
    
    /**
     * @brief Limpa a tabela de ameaça de um mob
     * @param mobID ID do mob
     * @return true se limpo com sucesso
     */
    bool clearThreat(uint32_t mobID);
    
    /**
     * @brief Obtém a ameaça de um personagem para um mob
     * @param mobID ID do mob
     * @param characterID ID do personagem
     * @return Quantidade de ameaça
     */
    uint32_t getThreat(uint32_t mobID, uint32_t characterID);
    
    /**
     * @brief Obtém o personagem com maior ameaça para um mob
     * @param mobID ID do mob
     * @return ID do personagem ou 0 se não tiver ameaça
     */
    uint32_t getHighestThreat(uint32_t mobID);
    
    /**
     * @brief Cria um cérebro (brain)
     * @param name Nome
     * @param type Tipo
     * @param parameters Parâmetros
     * @param targetSelectionType Tipo de seleção de alvo
     * @param skillSelectionType Tipo de seleção de habilidade
     * @param stateBehaviors Comportamentos de estado
     * @param responses Respostas a condições
     * @return ID do cérebro ou 0 em caso de erro
     */
    uint32_t createBrain(const std::string& name, AIBrainType type,
                        const std::vector<AIParameter>& parameters,
                        AITargetSelectionType targetSelectionType,
                        AISkillSelectionType skillSelectionType,
                        const std::map<AIState, std::vector<AIBehaviorType>>& stateBehaviors,
                        const std::map<AIConditionType, AIResponseType>& responses);
    
    /**
     * @brief Remove um cérebro (brain)
     * @param brainID ID do cérebro
     * @return true se removido com sucesso
     */
    bool removeBrain(uint32_t brainID);
    
    /**
     * @brief Obtém um cérebro (brain)
     * @param brainID ID do cérebro
     * @return Cérebro
     */
    AIBrain getBrain(uint32_t brainID);
    
    /**
     * @brief Cria um script
     * @param name Nome
     * @param description Descrição
     * @param source Código fonte
     * @return ID do script ou 0 em caso de erro
     */
    uint32_t createScript(const std::string& name, const std::string& description, const std::string& source);
    
    /**
     * @brief Remove um script
     * @param scriptID ID do script
     * @return true se removido com sucesso
     */
    bool removeScript(uint32_t scriptID);
    
    /**
     * @brief Atualiza um script
     * @param scriptID ID do script
     * @param source Código fonte
     * @return true se atualizado com sucesso
     */
    bool updateScript(uint32_t scriptID, const std::string& source);
    
    /**
     * @brief Compila um script
     * @param scriptID ID do script
     * @return true se compilado com sucesso
     */
    bool compileScript(uint32_t scriptID);
    
    /**
     * @brief Habilita ou desabilita um script
     * @param scriptID ID do script
     * @param enabled Habilitado
     * @return true se alterado com sucesso
     */
    bool enableScript(uint32_t scriptID, bool enabled);
    
    /**
     * @brief Obtém um script
     * @param scriptID ID do script
     * @return Script
     */
    AIScript getScript(uint32_t scriptID);
    
    /**
     * @brief Cria uma patrulha
     * @param name Nome
     * @param points Pontos de patrulha
     * @param cyclic Cíclica
     * @param reverse Reversa
     * @param startTime Tempo de início
     * @param endTime Tempo de fim
     * @return ID da patrulha ou 0 em caso de erro
     */
    uint32_t createPatrol(const std::string& name, const std::vector<AIPatrolPoint>& points,
                         bool cyclic = true, bool reverse = false,
                         uint32_t startTime = 0, uint32_t endTime = 0);
    
    /**
     * @brief Remove uma patrulha
     * @param patrolID ID da patrulha
     * @return true se removido com sucesso
     */
    bool removePatrol(uint32_t patrolID);
    
    /**
     * @brief Obtém uma patrulha
     * @param patrolID ID da patrulha
     * @return Patrulha
     */
    AIPatrolConfig getPatrol(uint32_t patrolID);
    
    /**
     * @brief Cria um território
     * @param name Nome
     * @param centerX Centro X
     * @param centerY Centro Y
     * @param radius Raio
     * @param aggressive Agressivo
     * @param homePoint Ponto inicial
     * @return ID do território ou 0 em caso de erro
     */
    uint32_t createTerritory(const std::string& name, uint16_t centerX, uint16_t centerY,
                            uint16_t radius, bool aggressive = false, bool homePoint = true);
    
    /**
     * @brief Remove um território
     * @param territoryID ID do território
     * @return true se removido com sucesso
     */
    bool removeTerritory(uint32_t territoryID);
    
    /**
     * @brief Obtém um território
     * @param territoryID ID do território
     * @return Território
     */
    AITerritoryConfig getTerritory(uint32_t territoryID);
    
    /**
     * @brief Cria uma lista de habilidades
     * @param name Nome
     * @param skills Habilidades
     * @param priorities Prioridades
     * @param chances Chances
     * @return ID da lista ou 0 em caso de erro
     */
    uint32_t createSkillList(const std::string& name, const std::vector<uint16_t>& skills,
                            const std::vector<uint8_t>& priorities, const std::vector<uint8_t>& chances);
    
    /**
     * @brief Remove uma lista de habilidades
     * @param skillListID ID da lista
     * @return true se removido com sucesso
     */
    bool removeSkillList(uint32_t skillListID);
    
    /**
     * @brief Obtém uma lista de habilidades
     * @param skillListID ID da lista
     * @return Lista de habilidades
     */
    AISkillList getSkillList(uint32_t skillListID);
    
    /**
     * @brief Registra um callback de evento
     * @param type Tipo de evento
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerEventCallback(AIEventType type, AIEventCallback callback);
    
    /**
     * @brief Remove um callback de evento
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEventCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de atualização
     * @param mobID ID do mob
     * @param callback Função de callback
     * @return true se registrado com sucesso
     */
    bool registerUpdateCallback(uint32_t mobID, AIUpdateCallback callback);
    
    /**
     * @brief Remove um callback de atualização
     * @param mobID ID do mob
     * @return true se removido com sucesso
     */
    bool unregisterUpdateCallback(uint32_t mobID);
    
    /**
     * @brief Seleciona um alvo para um mob
     * @param mobID ID do mob
     * @param state Estado atual
     * @return ID do alvo ou 0 se não encontrado
     */
    uint32_t selectTarget(uint32_t mobID, AIState state);
    
    /**
     * @brief Seleciona uma habilidade para um mob
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @return ID da habilidade ou 0 se não encontrado
     */
    uint16_t selectSkill(uint32_t mobID, uint32_t targetID);
    
    /**
     * @brief Seleciona um comportamento para um mob
     * @param mobID ID do mob
     * @param state Estado atual
     * @return Comportamento
     */
    AIBehaviorType selectBehavior(uint32_t mobID, AIState state);
    
    /**
     * @brief Responde a uma condição
     * @param mobID ID do mob
     * @param condition Condição
     * @param value Valor
     * @return Resposta
     */
    AIResponseType respondToCondition(uint32_t mobID, AIConditionType condition, float value);
    
    /**
     * @brief Verifica se uma condição é verdadeira
     * @param mobID ID do mob
     * @param condition Condição
     * @param value Valor
     * @return true se for verdadeira
     */
    bool checkCondition(uint32_t mobID, AIConditionType condition, float value);
    
    /**
     * @brief Obtém o próximo ponto de patrulha para um mob
     * @param mobID ID do mob
     * @param posX Posição X (saída)
     * @param posY Posição Y (saída)
     * @return true se encontrado
     */
    bool getNextPatrolPoint(uint32_t mobID, uint16_t& posX, uint16_t& posY);
    
    /**
     * @brief Verifica se um mob está em seu território
     * @param mobID ID do mob
     * @param posX Posição X
     * @param posY Posição Y
     * @return true se estiver
     */
    bool isInTerritory(uint32_t mobID, uint16_t posX, uint16_t posY);
    
    /**
     * @brief Verifica se um alvo está no alcance de visão
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @return true se estiver
     */
    bool isTargetInSight(uint32_t mobID, uint32_t targetID);
    
    /**
     * @brief Verifica se um alvo está no alcance de ataque
     * @param mobID ID do mob
     * @param targetID ID do alvo
     * @return true se estiver
     */
    bool isTargetInAttackRange(uint32_t mobID, uint32_t targetID);
    
    /**
     * @brief Calcula a distância entre dois IDs
     * @param id1 ID 1
     * @param id2 ID 2
     * @return Distância
     */
    float calculateDistance(uint32_t id1, uint32_t id2);
    
    /**
     * @brief Calcula a distância entre duas posições
     * @param x1 X 1
     * @param y1 Y 1
     * @param x2 X 2
     * @param y2 Y 2
     * @return Distância
     */
    float calculateDistance(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    
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
    
    /**
     * @brief Define o intervalo de atualização
     * @param interval Intervalo em milissegundos
     */
    void setUpdateInterval(uint32_t interval);
    
    /**
     * @brief Obtém o intervalo de atualização
     * @return Intervalo em milissegundos
     */
    uint32_t getUpdateInterval() const;
    
private:
    /**
     * @brief Construtor privado
     */
    AIManager();
    
    /**
     * @brief Destrutor privado
     */
    ~AIManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    AIManager(const AIManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    AIManager& operator=(const AIManager&) = delete;
    
    // Cérebros (brains)
    std::unordered_map<uint32_t, AIBrain> _brains;
    std::atomic<uint32_t> _nextBrainID;
    
    // Scripts
    std::unordered_map<uint32_t, AIScript> _scripts;
    std::atomic<uint32_t> _nextScriptID;
    
    // Patrulhas
    std::unordered_map<uint32_t, AIPatrolConfig> _patrols;
    std::atomic<uint32_t> _nextPatrolID;
    
    // Territórios
    std::unordered_map<uint32_t, AITerritoryConfig> _territories;
    std::atomic<uint32_t> _nextTerritoryID;
    
    // Listas de habilidades
    std::unordered_map<uint32_t, AISkillList> _skillLists;
    std::atomic<uint32_t> _nextSkillListID;
    
    // Mobs
    std::unordered_map<uint32_t, AIMobState> _mobStates;
    std::unordered_map<uint32_t, uint32_t> _mobBrains;
    std::unordered_map<uint32_t, uint32_t> _mobScripts;
    std::unordered_map<uint32_t, uint32_t> _mobSkillLists;
    std::unordered_map<uint32_t, uint32_t> _mobPatrols;
    std::unordered_map<uint32_t, uint32_t> _mobTerritories;
    
    // Callbacks
    std::unordered_map<uint32_t, AIEventCallback> _eventCallbacks;
    std::unordered_map<AIEventType, std::vector<uint32_t>> _eventCallbacksByType;
    std::unordered_map<uint32_t, AIUpdateCallback> _updateCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Configurações
    uint32_t _updateInterval;
    uint32_t _updateTimer;
    
    // Gerador de números aleatórios
    std::mt19937 _rng;
    
    // Thread safety
    mutable std::mutex _brainMutex;
    mutable std::mutex _scriptMutex;
    mutable std::mutex _patrolMutex;
    mutable std::mutex _territoryMutex;
    mutable std::mutex _skillListMutex;
    mutable std::mutex _mobMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    /**
     * @brief Atualiza todos os mobs
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void updateMobs(uint32_t deltaTime);
    
    /**
     * @brief Atualiza o estado de um mob
     * @param mobID ID do mob
     * @param state Estado atual
     * @param currentTime Tempo atual
     * @return Novo estado
     */
    AIState updateMobState(uint32_t mobID, AIMobState& state, uint32_t currentTime);
    
    /**
     * @brief Loga um evento de IA
     * @param event Evento
     */
    void logAIEvent(const AIEvent& event);
    
    /**
     * @brief Notifica os callbacks de evento
     * @param event Evento
     */
    void notifyEventCallbacks(const AIEvent& event);
    
    /**
     * @brief Executa o callback de atualização
     * @param mobID ID do mob
     * @param state Estado do mob
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void executeUpdateCallback(uint32_t mobID, AIMobState& state, uint32_t deltaTime);
    
    /**
     * @brief Verifica as condições de resposta
     * @param mobID ID do mob
     */
    void checkResponseConditions(uint32_t mobID);
    
    /**
     * @brief Carrega dados de AI do banco de dados
     * @return true se carregado com sucesso
     */
    bool loadAIFromDatabase();
    
    /**
     * @brief Salva dados de AI no banco de dados
     * @return true se salvo com sucesso
     */
    bool saveAIToDatabase();
};

} // namespace ai
} // namespace wydbr

#endif // _AI_MANAGER_H_