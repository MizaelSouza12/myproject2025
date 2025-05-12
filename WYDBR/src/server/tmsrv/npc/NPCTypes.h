/**
 * NPCTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/NPCTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef NPC_TYPES_H
#define NPC_TYPES_H

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <functional>
#include "../../../core/WYDTypes.h"

namespace wydbr {


namespace wyd {
namespace server {

/**
 * @brief Configuração do sistema de NPCs
 */
struct NPCConfig {
    std::string npcDir;        // Diretório dos arquivos de NPC
    INT maxPerZone;            // Máximo de NPCs por zona
    DWORD spawnInterval;       // Intervalo de spawn em milissegundos
    WORD despawnDistance;      // Distância para despawn
    DWORD updateInterval;      // Intervalo de atualização em milissegundos
    
    NPCConfig() 
        : npcDir("data/npcs")
        , maxPerZone(500)
        , spawnInterval(60000)
        , despawnDistance(100)
        , updateInterval(1000)
    {
    }
};

/**
 * @brief Tipos de NPC
 */
enum class NPCType : BYTE {
    UNKNOWN = 0,        // Tipo desconhecido
    MONSTER = 1,        // Monstro hostil
    PASSIVE = 2,        // Monstro passivo (não ataca a menos que seja atacado)
    QUEST = 3,          // NPC de quest
    VENDOR = 4,         // Comerciante
    BANKER = 5,         // Banqueiro
    SKILL_TRAINER = 6,  // Treinador de habilidades
    GUARD = 7,          // Guarda (protege áreas)
    BOSS = 8,           // Chefe (com habilidades especiais e fases)
    CRITTER = 9,        // Criatura não combatente (decorativa)
    EVENT = 10          // NPC de evento especial
};

/**
 * @brief Classes de NPC
 */
enum class NPCClass : BYTE {
    UNKNOWN = 0,         // Classe desconhecida
    WARRIOR = 1,         // Guerreiro
    MAGE = 2,            // Mago
    ARCHER = 3,          // Arqueiro
    HEALER = 4,          // Curandeiro
    THIEF = 5,           // Ladrão
    BEAST = 6,           // Besta
    UNDEAD = 7,          // Morto-vivo
    DEMON = 8,           // Demônio
    ELEMENTAL = 9,       // Elemental
    DRAGON = 10,         // Dragão
    MECHANICAL = 11,     // Mecânico
    PLANT = 12,          // Planta
    HUMANOID = 13,       // Humanoide
    DIVINE = 14,         // Divino
    UNIQUE = 15          // Único (para bosses personalizados)
};

/**
 * @brief Tamanhos de NPC
 */
enum class NPCSize : BYTE {
    TINY = 0,           // Minúsculo
    SMALL = 1,          // Pequeno
    MEDIUM = 2,         // Médio
    LARGE = 3,          // Grande
    HUGE = 4,           // Enorme
    GARGANTUAN = 5      // Gigantesco
};

/**
 * @brief Tipos de resistência
 */
enum class ResistanceType : BYTE {
    PHYSICAL = 0,       // Física
    FIRE = 1,           // Fogo
    WATER = 2,          // Água
    EARTH = 3,          // Terra
    WIND = 4,           // Vento
    HOLY = 5,           // Sagrado
    DARK = 6,           // Trevas
    POISON = 7,         // Veneno
    STUN = 8,           // Atordoamento
    MAX_RESISTANCE = 9  // Marcador para o número máximo de resistências
};

/**
 * @brief Estados de AI
 */
enum class AIState : BYTE {
    IDLE = 0,           // Parado
    PATROL = 1,         // Patrulhando
    COMBAT = 2,         // Em combate
    EVADE = 3,          // Evadindo
    FLEE = 4,           // Fugindo
    FOLLOW = 5,         // Seguindo
    CASTING = 6,        // Conjurando habilidade
    STUNNED = 7,        // Atordoado
    DEAD = 8            // Morto
};

/**
 * @brief Flags de comportamento
 */
enum class NPCFlag : DWORD {
    NONE = 0x00000000,
    VENDOR = 0x00000001,             // É um vendedor
    BANKER = 0x00000002,             // É um banqueiro
    QUEST_GIVER = 0x00000004,        // Oferece quests
    SKILL_TRAINER = 0x00000008,      // Treina habilidades
    REPAIR = 0x00000010,             // Repara itens
    STORAGE = 0x00000020,            // Fornece armazenamento
    MAILBOX = 0x00000040,            // Fornece correio
    AUCTIONEER = 0x00000080,         // Gerencia leilões
    TRANSPORT = 0x00000100,          // Fornece transporte
    INNKEEPER = 0x00000200,          // Fornece hospedagem
    GUILD_MASTER = 0x00000400,       // Gerencia guildas
    ARENA_MASTER = 0x00000800,       // Gerencia arena
    BATTLEGROUND_MASTER = 0x00001000,// Gerencia campos de batalha
    PVP_MASTER = 0x00002000,         // Gerencia PvP
    STABLE_MASTER = 0x00004000,      // Gerencia montarias
    REFINER = 0x00008000,            // Refina itens
    ENCHANTER = 0x00010000,          // Encanta itens
    TELEPORTER = 0x00020000,         // Teleporta jogadores
    COLLECTOR = 0x00040000,          // Coleta itens
    CUSTOM_SCRIPT = 0x00080000,      // Usa script personalizado
    EVENT_NPC = 0x00100000,          // É um NPC de evento
    AGGRESSIVE = 0x01000000,         // É agressivo
    PASSIVE = 0x02000000,            // É passivo
    BOSS = 0x04000000,               // É um boss
    ELITE = 0x08000000,              // É elite
    RARE = 0x10000000,               // É raro
    UNIQUE = 0x20000000,             // É único
    INVULNERABLE = 0x40000000,       // É invulnerável
    IMMORTAL = 0x80000000            // É imortal
};

// Operador de bitwise OR para flags
inline NPCFlag operator|(NPCFlag a, NPCFlag b) {
    return static_cast<NPCFlag>(static_cast<DWORD>(a) | static_cast<DWORD>(b));
}

// Operador de bitwise AND para flags
inline NPCFlag operator&(NPCFlag a, NPCFlag b) {
    return static_cast<NPCFlag>(static_cast<DWORD>(a) & static_cast<DWORD>(b));
}

// Função para verificar se uma flag está ativa
inline /**
 * HasFlag
 * 
 * Implementa a funcionalidade HasFlag conforme especificação original.
 * @param flags Parâmetro flags
 * @param flag Parâmetro flag
 * @return Retorna bool
 */
 bool HasFlag(NPCFlag flags, NPCFlag flag){
    return (static_cast<DWORD>(flags) & static_cast<DWORD>(flag)) != 0;
}

/**
 * @brief Informações de um NPC
 */
struct NPCInfo {
    char name[32];                // Nome do NPC
    NPCType type;                 // Tipo de NPC
    NPCClass npcClass;            // Classe do NPC
    NPCSize size;                 // Tamanho do NPC
    NPCFlag flags;                // Flags de comportamento
    WORD level;                   // Nível do NPC
    DWORD maxHP;                  // HP máximo
    DWORD maxMP;                  // MP máximo
    WORD attackMin;               // Ataque mínimo
    WORD attackMax;               // Ataque máximo
    WORD defense;                 // Defesa
    WORD magicDefense;            // Defesa mágica
    WORD accuracy;                // Precisão
    WORD evasion;                 // Evasão
    BYTE moveSpeed;               // Velocidade de movimento
    BYTE attackSpeed;             // Velocidade de ataque
    BYTE attackRange;             // Alcance de ataque
    BYTE detectRange;             // Alcance de detecção
    BYTE viewRange;               // Alcance de visão
    BYTE resistances[9];          // Resistências
    WORD modelId;                 // ID do modelo 3D
    WORD spriteId;                // ID do sprite
    BYTE race;                    // Raça
    BYTE element;                 // Elemento
    
    // Comportamento
    bool isAggressive;            // Se o NPC é agressivo
    bool callForHelp;             // Se o NPC chama por ajuda
    BYTE fleeHealthPercent;       // Percentual de HP para fugir
    bool immuneToStatusEffects;   // Se é imune a efeitos de status
    BYTE aiType;                  // Tipo de AI
    
    // Drops e recompensas
    float experienceModifier;     // Modificador de experiência
    float dropRateModifier;       // Modificador de taxa de drop
    float goldModifier;           // Modificador de ouro
    DWORD minGold;                // Ouro mínimo
    DWORD maxGold;                // Ouro má/**
 * NPCInfo
 * 
 * Implementa a funcionalidade NPCInfo conforme especificação original.
 * @return Retorna ximo
 */
ximo
    
    NPCInfo(){
        memset(this, 0, sizeof(NPCInfo));
        strncpy(name, "Unknown NPC");
        type = NPCType::UNKNOWN;
        npcClass = NPCClass::UNKNOWN;
        size = NPCSize::MEDIUM;
        flags = NPCFlag::NONE;
        level = 1;
        maxHP = 100;
        maxMP = 0;
        attackMin = 1;
        attackMax = 3;
        defense = 0;
        magicDefense = 0;
        accuracy = 10;
        evasion = 10;
        moveSpeed = 100;
        attackSpeed = 100;
        attackRange = 1;
        detectRange = 5;
        viewRange = 7;
        modelId = 0;
        spriteId = 0;
        race = 0;
        element = 0;
        isAggressive = false;
        callForHelp = false;
        fleeHealthPercent = 0;
        immuneToStatusEffects = false;
        aiType = 0;
        experienceModifier = 1.0f;
        dropRateModifier = 1.0f;
        goldModifier = 1.0f;
        minGold = 0;
        maxGold = 0;
        
        for (BYTE i = 0; i < 9; i++) {
            resistances[i] = 0;
        }
    }
};

/**
 * @brief Habilidade de NPC
 */
struct NPCSkill {
    WORD skillId;                 // ID da habilidade
    BYTE level;                   // Nível da habilidade
    DWORD cooldown;               // Cooldown em milissegundos
    WORD requiredHP;              // HP mínimo necessário (%)
    WORD requiredMP;              // MP mínimo necessário
    BYTE chance;                  // Chance de uso (0-100)
    BYTE priority;                // Prioridade (0-255)
    bool isAOE;                   // Se é de área
    WORD range;                   // Alcance
    WORD manaCost;                // Custo de mana
    
    NPCSkill() 
        : skillId(0)
        , level(1)
        , cooldown(0)
        , requiredHP(0)
        , requiredMP(0)
        , chance(100)
        , priority(0)
        , isAOE(false)
        , range(1)
        , manaCost(0)
    {
    }
};

/**
 * @brief Diálogo de NPC
 */
struct NPCDialog {
    WORD dialogId;                // ID do diálogo
    char text[256];               // Texto do diálogo
    std::vector<WORD> responses;  // IDs de respostas possíveis
    std::vector<WORD> nextDialogs;// IDs dos próximos diálogos
    WORD requiredQuestId;         // ID da quest necessária
    BYTE requiredQuestStatus;     // Status da quest necessária
    WORD itemReward;              // Item de recompensa
    WORD goldReward;              // Ouro de recompensa
    WORD expReward;               // Experiência de recompensa
    
    NPCDialog() 
        : dialogId(0)
        , requiredQuestId(0)
        , requiredQuestStatus(0)
        , itemReward(0)
        , goldReward(0)
        , expReward(0)
    {
        memset(text, 0, sizeof(text));
    }
};

/**
 * @brief Item de vendor
 */
struct VendorItem {
    WORD itemId;                  // ID do item
    WORD quantity;                // Quantidade disponível (0 = infinito)
    DWORD price;                  // Preço de venda
    BYTE refineLevel;             // Nível de refinamento
    BYTE restock;                 // Tempo para reabastecer (em minutos)
    DWORD lastRestock;            // Timestamp do último reabastecimento
    
    VendorItem() 
        : itemId(0)
        , quantity(0)
        , price(0)
        , refineLevel(0)
        , restock(0)
        , lastRestock(0)
    {
    }
};

/**
 * @brief Item de drop
 */
struct DropItem {
    WORD itemId;                  // ID do item
    float chance;                 // Chance de drop (0.0-1.0)
    WORD minAmount;               // Quantidade mínima
    WORD maxAmount;               // Quantidade máxima
    BYTE refineLevel;             // Nível de refinamento
    WORD requiredQuestId;         // ID da quest necessária
    BYTE requiredQuestStatus;     // Status da quest necessária
    
    DropItem() 
        : itemId(0)
        , chance(0.0f)
        , minAmount(1)
        , maxAmount(1)
        , refineLevel(0)
        , requiredQuestId(0)
        , requiredQuestStatus(0)
    {
    }
};

/**
 * @brief Configurações de boss
 */
struct BossSettings {
    BYTE phaseCount;              // Número de fases
    float phaseThresholds[3];     // Limiares de HP para cada fase (%)
    WORD phaseSkills[3][3];       // Habilidades especiais para cada fase
    bool summonMinionsOnPhase[3]; // Se deve invocar lacaios em cada fase
    WORD minionIds[3];            // IDs dos lacaios para cada fase
    BYTE minionCount[3];          // Número de lacaios para cada fase
    DWORD enrageTimer;            // Tempo para enraivecer (em milissegundos)
    WORD enrageBonus;             // Bônus de dano quando enraivecido (%)
    bool hasPhaseTransition;      // Se tem transição de fase
    BYTE immunePhase;             // Fase em que fica imune
    WORD specialItemDrop;         // Item especial que é dropado
    
    BossSettings() 
        : phaseCount(1)
        , enrageTimer(600000)
        , enrageBonus(0)
        , hasPhaseTransition(false)
        , immunePhase(0)
        , specialItemDrop(0)
    {
        for (BYTE i = 0; i < 3; i++) {
            phaseThresholds[i] = 0.0f;
            summonMinionsOnPhase[i] = false;
            minionIds[i] = 0;
            minionCount[i] = 0;
            
            for (BYTE j = 0; j < 3; j++) {
                phaseSkills[i][j] = 0;
            }
        }
    }
};

/**
 * @brief Template de NPC
 */
struct NPCTemplate {
    DWORD id;                       // ID do template
    NPCInfo info;                   // Informações básicas
    std::vector<NPCSkill> skills;   // Habilidades
    std::vector<NPCDialog> dialogs; // Diálogos
    std::vector<VendorItem> items;  // Itens (para vendors)
    std::vector<DropItem> drops;    // Drops
    BossSettings bossSettings;      // Configurações de boss (se aplicável)
    std::vector<WORD> quests;       // Quests relacionadas
    std::string customScript;       // Script personalizado
    
    NPCTemplate() /**
 * id
 * 
 * Implementa a funcionalidade id conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 : id(0){}
};

/**
 * @brief Informações de spawn de NPC
 */
struct NPCSpawnInfo {
    DWORD templateId;               // ID do template do NPC
    WORD mapId;                     // ID do mapa
    WORD x;                         // Posição X
    WORD y;                         // Posição Y
    DWORD respawnDelay;             // Atraso para respawn em milissegundos
    BYTE maxCount;                  // Número máximo de spawns
    BYTE currentCount;              // Número atual de spawns
    BYTE spawnRadius;               // Raio de spawn
    BYTE spawnTime;                 // Hora do dia para spawn (0-23, 24 para qualquer)
    WORD spawnEvent;                // Evento para spawn (0 para sempre)
    WORD despawnEvent;              // Evento para despawn (0 para nunca)
    bool spawnOnServerStart;        // Se deve spawnar ao iniciar o servidor
    
    NPCSpawnInfo() 
        : templateId(0)
        , mapId(0)
        , x(0)
        , y(0)
        , respawnDelay(60000)
        , maxCount(1)
        , currentCount(0)
        , spawnRadius(0)
        , spawnTime(24)
        , spawnEvent(0)
        , despawnEvent(0)
        , spawnOnServerStart(true)
    {
    }
};

/**
 * @brief Controlador avançado de IA para NPCs
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
    AIController(void* npc);
    
    /**
     * @brief Destrutor
     */
    ~AIController();
    
    /**
     * @brief Atualiza a AI
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    /**
     * @brief Define o tipo de AI
     * @param aiType Tipo de AI
     */
    void SetAIType(BYTE aiType);
    
    /**
     * @brief Configura um parâmetro de AI
     * @param paramName Nome do parâmetro
     * @param value Valor do parâmetro
     */
    void SetParameter(const std::string& paramName, float value);
    
    /**
     * @brief Adiciona um comportamento à AI
     * @param behaviorName Nome do comportamento
     * @param priority Prioridade (0-100)
     */
    void AddBehavior(const std::string& behaviorName, BYTE priority);
    
    /**
     * @brief Remove um comportamento da AI
     * @param behaviorName Nome do comportamento
     */
    void RemoveBehavior(const std::string& behaviorName);
    
    /**
     * @brief Processa um evento
     * @param eventName Nome do evento
     * @param param1 Primeiro parâmetro (opcional)
     * @param param2 Segundo parâmetro (opcional)
     */
    void ProcessEvent(const std::string& eventName, DWORD param1 = 0, DWORD param2 = 0);
    
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
     * @brief Verifica se o NPC pode ver um alvo
     * @param targetId ID do alvo
     * @return true se pode ver o alvo, false caso contrário
     */
    bool CanSeeTarget(DWORD targetId);
    
    /**
     * @brief Verifica se o NPC está no alcance de ataque de um alvo
     * @param targetId ID do alvo
     * @return true se está no alcance, false caso contrário
     */
    bool IsInAttackRange(DWORD targetId);
    
    /**
     * @brief Registra um callback para ser chamado quando um evento ocorrer
     * @param eventName Nome do evento
     * @param callback Função de callback
     */
    void RegisterEventCallback(const std::string& eventName, std::function<void(DWORD, DWORD)> callback);
    
private:
    void* npc_;                       // Ponteiro para o NPC (void* para evitar dependência circular)
    BYTE aiType_;                     // Tipo de AI
    AIState currentState_;            // Estado atual da AI
    DWORD targetId_;                  // ID do alvo atual
    std::vector<std::pair<WORD, WORD>> pathPoints_; // Pontos no caminho calculado
    INT currentPathPoint_;            // Índice do ponto atual no caminho
    DWORD lastPathCalculation_;       // Timestamp do último cálculo de caminho
    DWORD lastSkillDecision_;         // Timestamp da última decisão de habilidade
    DWORD lastTargetEvaluation_;      // Timestamp da última avaliação de alvo
    DWORD lastStateChange_;           // Timestamp da última mudança de estado
    std::unordered_map<std::string, float> parameters_; // Parâmetros de configuração
    std::unordered_map<std::string, BYTE> behaviors_;   // Comportamentos ativos
    std::unordered_map<std::string, std::function<void(DWORD, DWORD)>> eventCallbacks_; // Callbacks de eventos
    
    // Utilitários
    /**
 * UpdateStateDecision
 * 
 * Implementa a funcionalidade UpdateStateDecision conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateStateDecision(DWORD deltaTime);
    /**
 * UpdateCombatState
 * 
 * Implementa a funcionalidade UpdateCombatState conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateCombatState(DWORD deltaTime);
    /**
 * UpdatePathfinding
 * 
 * Implementa a funcionalidade UpdatePathfinding conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdatePathfinding(DWORD deltaTime);
    /**
 * EvaluateThreats
 * 
 * Implementa a funcionalidade EvaluateThreats conforme especificação original.
 */

    void EvaluateThreats();
    /**
 * ProcessStateTransition
 * 
 * Implementa a funcionalidade ProcessStateTransition conforme especificação original.
 * @param newState Parâmetro newState
 */

    void ProcessStateTransition(AIState newState);
    /**
 * IsStuck
 * 
 * Implementa a funcionalidade IsStuck conforme especificação original.
 * @return Retorna bool
 */

    bool IsStuck() const;
    /**
 * GetTickCount
 * 
 * Implementa a funcionalidade GetTickCount conforme especificação original.
 * @return Retorna DWORD
 */

    DWORD GetTickCount() const;
};

} // namespace server
} // namespace wyd

#endif // NPC_TYPES_H

} // namespace wydbr
