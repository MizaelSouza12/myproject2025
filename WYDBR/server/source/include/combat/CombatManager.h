/**
 * CombatManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/combat/CombatManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _COMBAT_MANAGER_H_
#define _COMBAT_MANAGER_H_

/**
 * @file CombatManager.h
 * @brief Gerenciador de combate do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de combate do WYDBR,
 * responsável por controlar as batalhas entre jogadores e NPCs,
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
#include <chrono>
#include <queue>

#include "../GlobalDef.h"
#include "../Basedef.h"

namespace wydbr {
namespace combat {

/**
 * @brief Tipo de ataque
 */
enum /**
 * Classe WYDAttackType
 * 
 * Esta classe implementa a funcionalidade WYDAttackType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AttackType {
    NONE = 0,               // Nenhum
    NORMAL = 1,             // Normal
    SKILL = 2,              // Habilidade
    RANGED = 3,             // À distância
    AOE = 4,                // Área de efeito
    DOT = 5,                // Dano ao longo do tempo
    CRITICAL = 6,           // Crítico
    MAGIC = 7,              // Mágico
    PHYSICAL = 8,           // Físico
    PIERCE = 9,             // Perfurante
    ELEMENTAL = 10,         // Elemental
    SIEGE = 11,             // Cerco
    COUNTER = 12,           // Contra-ataque
    REFLECT = 13,           // Reflexão
    TRAP = 14,              // Armadilha
    SUMMON = 15,            // Invocação
    CUSTOM_1 = 16,          // Personalizado 1
    CUSTOM_2 = 17,          // Personalizado 2
    CUSTOM_3 = 18,          // Personalizado 3
    UNKNOWN = 19            // Desconhecido
};

/**
 * @brief Tipo de defesa
 */
enum /**
 * Classe WYDDefenseType
 * 
 * Esta classe implementa a funcionalidade WYDDefenseType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DefenseType {
    NONE = 0,               // Nenhum
    BLOCK = 1,              // Bloqueio
    DODGE = 2,              // Esquiva
    PARRY = 3,              // Aparar
    RESIST = 4,             // Resistir
    ABSORB = 5,             // Absorver
    IMMUNITY = 6,           // Imunidade
    BARRIER = 7,            // Barreira
    SHIELD = 8,             // Escudo
    INVULNERABLE = 9,       // Invulnerável
    REFLECT = 10,           // Reflexão
    CUSTOM_1 = 11,          // Personalizado 1
    CUSTOM_2 = 12,          // Personalizado 2
    CUSTOM_3 = 13,          // Personalizado 3
    UNKNOWN = 14            // Desconhecido
};

/**
 * @brief Tipo de dano
 */
enum /**
 * Classe WYDDamageType
 * 
 * Esta classe implementa a funcionalidade WYDDamageType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DamageType {
    NONE = 0,               // Nenhum
    PHYSICAL = 1,           // Físico
    MAGICAL = 2,            // Mágico
    TRUE = 3,               // Verdadeiro
    FIRE = 4,               // Fogo
    WATER = 5,              // Água
    EARTH = 6,              // Terra
    AIR = 7,                // Ar
    LIGHT = 8,              // Luz
    DARK = 9,               // Trevas
    POISON = 10,            // Veneno
    DISEASE = 11,           // Doença
    BLEED = 12,             // Sangramento
    HOLY = 13,              // Sagrado
    UNHOLY = 14,            // Profano
    LIGHTNING = 15,         // Relâmpago
    ICE = 16,               // Gelo
    ACID = 17,              // Ácido
    CUSTOM_1 = 18,          // Personalizado 1
    CUSTOM_2 = 19,          // Personalizado 2
    CUSTOM_3 = 20,          // Personalizado 3
    UNKNOWN = 21            // Desconhecido
};

/**
 * @brief Estado de combate
 */
enum /**
 * Classe WYDCombatState
 * 
 * Esta classe implementa a funcionalidade WYDCombatState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CombatState {
    NONE = 0,               // Nenhum
    IDLE = 1,               // Ocioso
    IN_COMBAT = 2,          // Em combate
    FLEEING = 3,            // Fugindo
    DEAD = 4,               // Morto
    STUNNED = 5,            // Atordoado
    FEARED = 6,             // Amedrontado
    CONFUSED = 7,           // Confuso
    FROZEN = 8,             // Congelado
    IMMOBILIZED = 9,        // Imobilizado
    CHARMED = 10,           // Enfeitiçado
    INVULNERABLE = 11,      // Invulnerável
    INVISIBLE = 12,         // Invisível
    STEALTH = 13,           // Furtivo
    CUSTOM_1 = 14,          // Personalizado 1
    CUSTOM_2 = 15,          // Personalizado 2
    CUSTOM_3 = 16,          // Personalizado 3
    UNKNOWN = 17            // Desconhecido
};

/**
 * @brief Tipo de evento de combate
 */
enum /**
 * Classe WYDCombatEventType
 * 
 * Esta classe implementa a funcionalidade WYDCombatEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CombatEventType {
    NONE = 0,               // Nenhum
    ENTER_COMBAT = 1,       // Entrar em combate
    LEAVE_COMBAT = 2,       // Sair de combate
    ATTACK = 3,             // Atacar
    DEFEND = 4,             // Defender
    DAMAGE = 5,             // Dano
    HEAL = 6,               // Cura
    SKILL_USE = 7,          // Uso de habilidade
    DEATH = 8,              // Morte
    RESPAWN = 9,            // Ressurgir
    THREAT_CHANGE = 10,     // Mudança de ameaça
    STATUS_EFFECT = 11,     // Efeito de status
    STATUS_EFFECT_END = 12, // Fim de efeito de status
    MISS = 13,              // Errar
    DODGE = 14,             // Esquivar
    PARRY = 15,             // Aparar
    BLOCK = 16,             // Bloquear
    CRITICAL_HIT = 17,      // Acerto crítico
    RESIST = 18,            // Resistir
    REFLECT = 19,           // Refletir
    ABSORB = 20,            // Absorver
    INTERRUPT = 21,         // Interromper
    FLEE = 22,              // Fugir
    ASSIST = 23,            // Auxiliar
    CUSTOM_1 = 24,          // Personalizado 1
    CUSTOM_2 = 25,          // Personalizado 2
    CUSTOM_3 = 26,          // Personalizado 3
    UNKNOWN = 27            // Desconhecido
};

/**
 * @brief Tipo de geração de ameaça
 */
enum /**
 * Classe WYDThreatGenerationType
 * 
 * Esta classe implementa a funcionalidade WYDThreatGenerationType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ThreatGenerationType {
    NONE = 0,               // Nenhum
    DAMAGE = 1,             // Dano
    HEALING = 2,            // Cura
    SKILL_USE = 3,          // Uso de habilidade
    BUFF = 4,               // Bônus
    DEBUFF = 5,             // Penalidade
    TAUNT = 6,              // Provocação
    PROXIMITY = 7,          // Proximidade
    CUSTOM_1 = 8,           // Personalizado 1
    CUSTOM_2 = 9,           // Personalizado 2
    CUSTOM_3 = 10,          // Personalizado 3
    UNKNOWN = 11            // Desconhecido
};

/**
 * @brief Tipo de condição de combate
 */
enum /**
 * Classe WYDCombatConditionType
 * 
 * Esta classe implementa a funcionalidade WYDCombatConditionType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CombatConditionType {
    NONE = 0,               // Nenhum
    HEALTH_PERCENTAGE = 1,  // Percentual de saúde
    MANA_PERCENTAGE = 2,    // Percentual de mana
    STAMINA_PERCENTAGE = 3, // Percentual de stamina
    TARGET_COUNT = 4,       // Contagem de alvos
    ALLY_COUNT = 5,         // Contagem de aliados
    ENEMY_COUNT = 6,        // Contagem de inimigos
    DISTANCE = 7,           // Distância
    STATUS_EFFECT = 8,      // Efeito de status
    SKILL_COOLDOWN = 9,     // Cooldown de habilidade
    TIME_IN_COMBAT = 10,    // Tempo em combate
    TARGET_TYPE = 11,       // Tipo de alvo
    WEAPON_TYPE = 12,       // Tipo de arma
    ARMOR_TYPE = 13,        // Tipo de armadura
    MAP_AREA = 14,          // Área do mapa
    CUSTOM_1 = 15,          // Personalizado 1
    CUSTOM_2 = 16,          // Personalizado 2
    CUSTOM_3 = 17,          // Personalizado 3
    UNKNOWN = 18            // Desconhecido
};

/**
 * @brief Tipo de modificador de combate
 */
enum /**
 * Classe WYDCombatModifierType
 * 
 * Esta classe implementa a funcionalidade WYDCombatModifierType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CombatModifierType {
    NONE = 0,               // Nenhum
    DAMAGE_DEALT = 1,       // Dano causado
    DAMAGE_TAKEN = 2,       // Dano recebido
    HEALING_DEALT = 3,      // Cura causada
    HEALING_RECEIVED = 4,   // Cura recebida
    CRITICAL_CHANCE = 5,    // Chance de crítico
    CRITICAL_DAMAGE = 6,    // Dano crítico
    ATTACK_SPEED = 7,       // Velocidade de ataque
    CAST_SPEED = 8,         // Velocidade de lançamento
    MOVEMENT_SPEED = 9,     // Velocidade de movimento
    DODGE_CHANCE = 10,      // Chance de esquiva
    PARRY_CHANCE = 11,      // Chance de aparar
    BLOCK_CHANCE = 12,      // Chance de bloquear
    RESIST_CHANCE = 13,     // Chance de resistir
    ACCURACY = 14,          // Precisão
    THREAT_GENERATION = 15, // Geração de ameaça
    THREAT_REDUCTION = 16,  // Redução de ameaça
    COOLDOWN_REDUCTION = 17,// Redução de cooldown
    RESOURCE_COST = 18,     // Custo de recurso
    DAMAGE_REFLECT = 19,    // Reflexão de dano
    CUSTOM_1 = 20,          // Personalizado 1
    CUSTOM_2 = 21,          // Personalizado 2
    CUSTOM_3 = 22,          // Personalizado 3
    UNKNOWN = 23            // Desconhecido
};

/**
 * @brief Tipo de efeito de status em combate
 */
enum /**
 * Classe WYDCombatStatusEffectType
 * 
 * Esta classe implementa a funcionalidade WYDCombatStatusEffectType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CombatStatusEffectType {
    NONE = 0,               // Nenhum
    STUN = 1,               // Atordoamento
    ROOT = 2,               // Enraizamento
    SILENCE = 3,            // Silenciamento
    BLIND = 4,              // Cegueira
    FEAR = 5,               // Medo
    CHARM = 6,              // Encanto
    CONFUSE = 7,            // Confusão
    SLEEP = 8,              // Sono
    SLOW = 9,               // Lentidão
    SNARE = 10,             // Impedimento
    KNOCKBACK = 11,         // Repulsão
    KNOCKUP = 12,           // Lançamento
    TAUNT = 13,             // Provocação
    DISARM = 14,            // Desarme
    PACIFY = 15,            // Pacificação
    INVISIBILITY = 16,      // Invisibilidade
    STEALTH = 17,           // Furtividade
    INVULNERABILITY = 18,   // Invulnerabilidade
    BLEED = 19,             // Sangramento
    POISON = 20,            // Veneno
    BURN = 21,              // Queimadura
    FREEZE = 22,            // Congelamento
    SHOCK = 23,             // Choque
    DISEASE = 24,           // Doença
    CURSE = 25,             // Maldição
    BERSERK = 26,           // Fúria
    HASTE = 27,             // Aceleração
    CUSTOM_1 = 28,          // Personalizado 1
    CUSTOM_2 = 29,          // Personalizado 2
    CUSTOM_3 = 30,          // Personalizado 3
    UNKNOWN = 31            // Desconhecido
};

/**
 * @brief Tipo de resistência
 */
enum /**
 * Classe WYDResistanceType
 * 
 * Esta classe implementa a funcionalidade WYDResistanceType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ResistanceType {
    NONE = 0,               // Nenhum
    PHYSICAL = 1,           // Físico
    MAGICAL = 2,            // Mágico
    FIRE = 3,               // Fogo
    WATER = 4,              // Água
    EARTH = 5,              // Terra
    AIR = 6,                // Ar
    LIGHT = 7,              // Luz
    DARK = 8,               // Trevas
    POISON = 9,             // Veneno
    DISEASE = 10,           // Doença
    BLEED = 11,             // Sangramento
    HOLY = 12,              // Sagrado
    UNHOLY = 13,            // Profano
    LIGHTNING = 14,         // Relâmpago
    ICE = 15,               // Gelo
    ACID = 16,              // Ácido
    STUN = 17,              // Atordoamento
    FEAR = 18,              // Medo
    CHARM = 19,             // Encanto
    CUSTOM_1 = 20,          // Personalizado 1
    CUSTOM_2 = 21,          // Personalizado 2
    CUSTOM_3 = 22,          // Personalizado 3
    UNKNOWN = 23            // Desconhecido
};

/**
 * @brief Tipo de zona de combate
 */
enum /**
 * Classe WYDCombatZoneType
 * 
 * Esta classe implementa a funcionalidade WYDCombatZoneType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CombatZoneType {
    NONE = 0,               // Nenhum
    PVE = 1,                // Jogador vs Ambiente
    PVP = 2,                // Jogador vs Jogador
    SANCTUARY = 3,          // Santuário
    ARENA = 4,              // Arena
    BATTLEFIELD = 5,        // Campo de batalha
    GUILD_WAR = 6,          // Guerra de guilda
    SIEGE = 7,              // Cerco
    DUEL = 8,               // Duelo
    RAID = 9,               // Ataque
    DUNGEON = 10,           // Masmorra
    WORLD_BOSS = 11,        // Chefe mundial
    EVENT = 12,             // Evento
    CUSTOM_1 = 13,          // Personalizado 1
    CUSTOM_2 = 14,          // Personalizado 2
    CUSTOM_3 = 15,          // Personalizado 3
    UNKNOWN = 16            // Desconhecido
};

/**
 * @brief Resultado de ataque
 */
enum /**
 * Classe WYDAttackResult
 * 
 * Esta classe implementa a funcionalidade WYDAttackResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AttackResult {
    NONE = 0,               // Nenhum
    HIT = 1,                // Acerto
    CRITICAL = 2,           // Crítico
    MISS = 3,               // Erro
    DODGE = 4,              // Esquiva
    PARRY = 5,              // Aparar
    BLOCK = 6,              // Bloqueio
    RESIST = 7,             // Resistência
    IMMUNE = 8,             // Imunidade
    REFLECT = 9,            // Reflexão
    ABSORB = 10,            // Absorção
    EVADE = 11,             // Evasão
    CUSTOM_1 = 12,          // Personalizado 1
    CUSTOM_2 = 13,          // Personalizado 2
    CUSTOM_3 = 14,          // Personalizado 3
    UNKNOWN = 15            // Desconhecido
};

/**
 * @brief Gatilho de resposta
 */
enum /**
 * Classe WYDResponseTrigger
 * 
 * Esta classe implementa a funcionalidade WYDResponseTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ResponseTrigger {
    NONE = 0,               // Nenhum
    ON_HIT = 1,             // Ao ser atingido
    ON_DAMAGE = 2,          // Ao sofrer dano
    ON_DODGE = 3,           // Ao esquivar
    ON_PARRY = 4,           // Ao aparar
    ON_BLOCK = 5,           // Ao bloquear
    ON_CRITICAL = 6,        // Ao receber crítico
    ON_MISS = 7,            // Ao ser errado
    ON_STATUS_APPLIED = 8,  // Ao receber efeito de status
    ON_DEATH = 9,           // Ao morrer
    ON_HEALTH_THRESHOLD = 10,// Ao atingir limite de saúde
    ON_SKILL_USE = 11,      // Ao usar habilidade
    CUSTOM_1 = 12,          // Personalizado 1
    CUSTOM_2 = 13,          // Personalizado 2
    CUSTOM_3 = 14,          // Personalizado 3
    UNKNOWN = 15            // Desconhecido
};

/**
 * @brief Ataque
 */
struct Attack {
    uint32_t AttackID;          // ID do ataque
    uint32_t AttackerID;        // ID do atacante
    uint32_t TargetID;          // ID do alvo
    AttackType Type;            // Tipo
    DamageType DamageType;      // Tipo de dano
    uint32_t Damage;            // Dano
    uint16_t SkillID;           // ID da habilidade
    uint8_t SkillLevel;         // Nível da habilidade
    bool IsRanged;              // É à distância
    bool IsAOE;                 // É área de efeito
    float CriticalChance;       // Chance de crítico
    float CriticalMultiplier;   // Multiplicador de crítico
    uint32_t Timestamp;         // Timestamp
    uint16_t PosX;              // Posição X
    uint16_t PosY;              // Posição Y
    uint16_t Direction;         // Direção
    
    Attack() : AttackID(0), AttackerID(0), TargetID(0), Type(AttackType::NORMAL),
        DamageType(DamageType::PHYSICAL), Damage(0), SkillID(0), SkillLevel(0),
        IsRanged(false), IsAOE(false), CriticalChance(0.0f), CriticalMultiplier(1.0f),
        Timestamp(0), PosX(0), PosY(0), Direction(0) {}
};

/**
 * @brief Defesa
 */
struct Defense {
    uint32_t DefenseID;         // ID da defesa
    uint32_t DefenderID;        // ID do defensor
    uint32_t AttackerID;        // ID do atacante
    DefenseType Type;           // Tipo
    float BlockChance;          // Chance de bloqueio
    float BlockReduction;       // Redução de bloqueio
    float DodgeChance;          // Chance de esquiva
    float ParryChance;          // Chance de aparar
    float ResistChance;         // Chance de resistência
    std::map<ResistanceType, float> Resistances; // Resistências
    
    Defense() : DefenseID(0), DefenderID(0), AttackerID(0), Type(DefenseType::NONE),
        BlockChance(0.0f), BlockReduction(0.0f), DodgeChance(0.0f),
        ParryChance(0.0f), ResistChance(0.0f) {}
};

/**
 * @brief Resultado de combate
 */
struct CombatResult {
    uint32_t ResultID;          // ID do resultado
    uint32_t AttackID;          // ID do ataque
    uint32_t AttackerID;        // ID do atacante
    uint32_t DefenderID;        // ID do defensor
    AttackResult Result;        // Resultado
    uint32_t DamageDone;        // Dano causado
    uint32_t DamageAbsorbed;    // Dano absorvido
    uint32_t DamageReflected;   // Dano refletido
    bool IsCritical;            // É crítico
    uint32_t OverkillAmount;    // Quantidade de excesso de dano
    uint32_t HealingDone;       // Cura causada
    uint32_t Timestamp;         // Timestamp
    CombatStatusEffectType StatusEffect; // Efeito de status
    uint32_t StatusDuration;    // Duração do status
    uint32_t ThreatGenerated;   // Ameaça gerada
    
    CombatResult() : ResultID(0), AttackID(0), AttackerID(0), DefenderID(0),
        Result(AttackResult::NONE), DamageDone(0), DamageAbsorbed(0),
        DamageReflected(0), IsCritical(false), OverkillAmount(0),
        HealingDone(0), Timestamp(0), StatusEffect(CombatStatusEffectType::NONE),
        StatusDuration(0), ThreatGenerated(0) {}
};

/**
 * @brief Modificador de combate
 */
struct CombatModifier {
    uint32_t ModifierID;        // ID do modificador
    uint32_t EntityID;          // ID da entidade
    CombatModifierType Type;    // Tipo
    float Value;                // Valor
    uint32_t Duration;          // Duração
    uint32_t StartTime;         // Tempo de início
    uint32_t EndTime;           // Tempo de fim
    uint16_t SourceSkillID;     // ID da habilidade fonte
    uint32_t SourceEntityID;    // ID da entidade fonte
    bool IsBuff;                // É bônus
    bool IsDebuff;              // É penalidade
    bool IsTemporary;           // É temporário
    bool IsRemovable;           // É removível
    std::string Description;    // Descrição
    
    CombatModifier() : ModifierID(0), EntityID(0), Type(CombatModifierType::NONE),
        Value(0.0f), Duration(0), StartTime(0), EndTime(0), SourceSkillID(0),
        SourceEntityID(0), IsBuff(false), IsDebuff(false), IsTemporary(false),
        IsRemovable(true) {}
};

/**
 * @brief Efeito de status em combate
 */
struct CombatStatusEffect {
    uint32_t EffectID;          // ID do efeito
    uint32_t TargetID;          // ID do alvo
    uint32_t CasterID;          // ID do lançador
    CombatStatusEffectType Type;// Tipo
    uint32_t Duration;          // Duração
    uint32_t StartTime;         // Tempo de início
    uint32_t EndTime;           // Tempo de fim
    uint32_t TickInterval;      // Intervalo de tick
    uint32_t LastTickTime;      // Último tempo de tick
    uint32_t DamagePerTick;     // Dano por tick
    uint32_t HealingPerTick;    // Cura por tick
    uint16_t SourceSkillID;     // ID da habilidade fonte
    bool IsRemovable;           // É removível
    bool IsStackable;           // É empilhável
    uint8_t Stacks;             // Pilhas
    uint8_t MaxStacks;          // Pilhas máximas
    
    CombatStatusEffect() : EffectID(0), TargetID(0), CasterID(0),
        Type(CombatStatusEffectType::NONE), Duration(0), StartTime(0),
        EndTime(0), TickInterval(0), LastTickTime(0), DamagePerTick(0),
        HealingPerTick(0), SourceSkillID(0), IsRemovable(true),
        IsStackable(false), Stacks(1), MaxStacks(1) {}
};

/**
 * @brief Estado de entidade em combate
 */
struct CombatEntityState {
    uint32_t EntityID;          // ID da entidade
    CombatState State;          // Estado
    uint32_t LastAttackTime;    // Último tempo de ataque
    uint32_t LastDamagedTime;   // Último tempo de dano
    uint32_t CombatStartTime;   // Tempo de início de combate
    uint32_t CombatEndTime;     // Tempo de fim de combate
    uint32_t LastTargetID;      // ID do último alvo
    std::map<uint32_t, uint32_t> ThreatTable; // Tabela de ameaça
    std::vector<uint32_t> ActiveModifiers; // Modificadores ativos
    std::vector<uint32_t> ActiveStatusEffects; // Efeitos de status ativos
    bool IsInCombat;            // Está em combate
    bool IsDead;                // Está morto
    float AggroRange;           // Alcance de agressão
    float LeashRange;           // Alcance de fixação
    
    CombatEntityState() : EntityID(0), State(CombatState::IDLE),
        LastAttackTime(0), LastDamagedTime(0), CombatStartTime(0),
        CombatEndTime(0), LastTargetID(0), IsInCombat(false),
        IsDead(false), AggroRange(0.0f), LeashRange(0.0f) {}
};

/**
 * @brief Evento de combate
 */
struct CombatEvent {
    uint32_t EventID;           // ID do evento
    CombatEventType Type;       // Tipo
    uint32_t EntityID;          // ID da entidade
    uint32_t TargetID;          // ID do alvo
    uint32_t SkillID;           // ID da habilidade
    uint32_t Timestamp;         // Timestamp
    uint32_t Value;             // Valor
    AttackResult Result;        // Resultado
    
    CombatEvent() : EventID(0), Type(CombatEventType::NONE), EntityID(0),
        TargetID(0), SkillID(0), Timestamp(0), Value(0),
        Result(AttackResult::NONE) {}
    
    CombatEvent(CombatEventType _type, uint32_t _entityID, uint32_t _targetID = 0,
               uint32_t _skillID = 0, uint32_t _value = 0, AttackResult _result = AttackResult::NONE)
        : EventID(0), Type(_type), EntityID(_entityID), TargetID(_targetID),
          SkillID(_skillID), Value(_value), Result(_result),
          Timestamp(static_cast<uint32_t>(time(nullptr))) {}
};

/**
 * @brief Zona de combate
 */
struct CombatZone {
    uint32_t ZoneID;            // ID da zona
    CombatZoneType Type;        // Tipo
    uint16_t MapID;             // ID do mapa
    uint16_t CenterX;           // Centro X
    uint16_t CenterY;           // Centro Y
    uint16_t Radius;            // Raio
    bool AllowPVP;              // Permite PVP
    bool AllowPVE;              // Permite PVE
    bool HasSafeSpots;          // Tem pontos seguros
    float DamageModifier;       // Modificador de dano
    float HealingModifier;      // Modificador de cura
    
    CombatZone() : ZoneID(0), Type(CombatZoneType::NONE), MapID(0),
        CenterX(0), CenterY(0), Radius(0), AllowPVP(false),
        AllowPVE(true), HasSafeSpots(false), DamageModifier(1.0f),
        HealingModifier(1.0f) {}
};

/**
 * @brief Configuração de combate
 */
struct CombatConfig {
    float DefaultCriticalChance;     // Chance de crítico padrão
    float DefaultCriticalMultiplier; // Multiplicador de crítico padrão
    float DefaultBlockChance;        // Chance de bloqueio padrão
    float DefaultBlockReduction;     // Redução de bloqueio padrão
    float DefaultDodgeChance;        // Chance de esquiva padrão
    float DefaultParryChance;        // Chance de aparar padrão
    float DefaultResistChance;       // Chance de resistência padrão
    float DefaultGlancingChance;     // Chance de raspão padrão
    float DefaultGlancingReduction;  // Redução de raspão padrão
    uint32_t BaseAttackSpeed;        // Velocidade de ataque base
    uint32_t BaseCastSpeed;          // Velocidade de lançamento base
    uint32_t CombatExitTime;         // Tempo de saída de combate
    float PVPDamageModifier;         // Modificador de dano PVP
    float PVEDamageModifier;         // Modificador de dano PVE
    float AOEDamageModifier;         // Modificador de dano AOE
    bool AllowFriendlyFire;          // Permite fogo amigo
    
    CombatConfig() : DefaultCriticalChance(0.05f), DefaultCriticalMultiplier(1.5f),
        DefaultBlockChance(0.05f), DefaultBlockReduction(0.5f),
        DefaultDodgeChance(0.05f), DefaultParryChance(0.05f),
        DefaultResistChance(0.05f), DefaultGlancingChance(0.1f),
        DefaultGlancingReduction(0.3f), BaseAttackSpeed(2000),
        BaseCastSpeed(2000), CombatExitTime(6000),
        PVPDamageModifier(0.5f), PVEDamageModifier(1.0f),
        AOEDamageModifier(0.8f), AllowFriendlyFire(false) {}
};

/**
 * @brief Estatísticas de combate
 */
struct CombatStats {
    uint32_t EntityID;          // ID da entidade
    uint32_t TotalDamageDealt;  // Dano total causado
    uint32_t TotalDamageTaken;  // Dano total recebido
    uint32_t TotalHealingDone;  // Cura total causada
    uint32_t TotalHealingReceived; // Cura total recebida
    uint32_t CriticalHits;      // Acertos críticos
    uint32_t Dodges;            // Esquivas
    uint32_t Parries;           // Aparadas
    uint32_t Blocks;            // Bloqueios
    uint32_t Misses;            // Erros
    uint32_t DamageSources;     // Fontes de dano
    uint32_t HighestDamageDealt;// Maior dano causado
    uint32_t HighestDamageTaken;// Maior dano recebido
    uint32_t TotalKills;        // Total de abates
    uint32_t TotalDeaths;       // Total de mortes
    std::map<DamageType, uint32_t> DamageByType; // Dano por tipo
    std::map<uint32_t, uint32_t> DamageBySkill; // Dano por habilidade
    
    CombatStats() : EntityID(0), TotalDamageDealt(0), TotalDamageTaken(0),
        TotalHealingDone(0), TotalHealingReceived(0), CriticalHits(0),
        Dodges(0), Parries(0), Blocks(0), Misses(0), DamageSources(0),
        HighestDamageDealt(0), HighestDamageTaken(0), TotalKills(0),
        TotalDeaths(0) {}
};

/**
 * @brief Log de combate
 */
struct CombatLog {
    uint32_t LogID;             // ID do log
    uint32_t Timestamp;         // Timestamp
    CombatEventType EventType;  // Tipo de evento
    uint32_t SourceID;          // ID da fonte
    uint32_t TargetID;          // ID do alvo
    uint32_t AbilityID;         // ID da habilidade
    int32_t Value;              // Valor
    AttackResult Result;        // Resultado
    DamageType DamageType;      // Tipo de dano
    bool IsCritical;            // É crítico
    std::string LogText;        // Texto do log
    
    CombatLog() : LogID(0), Timestamp(0), EventType(CombatEventType::NONE),
        SourceID(0), TargetID(0), AbilityID(0), Value(0),
        Result(AttackResult::NONE), DamageType(DamageType::NONE),
        IsCritical(false) {}
};

/**
 * @brief Estado global do combate
 */
struct GlobalCombatState {
    uint32_t TotalCombats;      // Total de combates
    uint32_t ActiveCombats;     // Combates ativos
    uint32_t TotalKills;        // Total de abates
    uint32_t TotalDeaths;       // Total de mortes
    uint32_t TotalDamageDealt;  // Dano total causado
    uint32_t TotalHealingDone;  // Cura total causada
    uint32_t PVPCombats;        // Combates PVP
    uint32_t PVECombats;        // Combates PVE
    float GlobalDamageModifier; // Modificador de dano global
    float GlobalHealingModifier;// Modificador de cura global
    bool PVPEnabled;            // PVP habilitado
    bool WorldPVPEnabled;       // PVP mundial habilitado
    
    GlobalCombatState() : TotalCombats(0), ActiveCombats(0), TotalKills(0),
        TotalDeaths(0), TotalDamageDealt(0), TotalHealingDone(0),
        PVPCombats(0), PVECombats(0), GlobalDamageModifier(1.0f),
        GlobalHealingModifier(1.0f), PVPEnabled(true), WorldPVPEnabled(false) {}
};

/**
 * @brief Resposta automática
 */
struct AutoResponse {
    uint32_t ResponseID;        // ID da resposta
    uint32_t EntityID;          // ID da entidade
    ResponseTrigger Trigger;    // Gatilho
    uint16_t SkillID;           // ID da habilidade
    CombatStatusEffectType StatusEffect; // Efeito de status
    uint32_t Duration;          // Duração
    uint32_t Cooldown;          // Cooldown
    uint32_t LastTriggeredTime; // Último tempo de acionamento
    float TriggerChance;        // Chance de acionamento
    CombatConditionType Condition; // Condição
    float ConditionValue;       // Valor da condição
    
    AutoResponse() : ResponseID(0), EntityID(0), Trigger(ResponseTrigger::NONE),
        SkillID(0), StatusEffect(CombatStatusEffectType::NONE), Duration(0),
        Cooldown(0), LastTriggeredTime(0), TriggerChance(1.0f),
        Condition(CombatConditionType::NONE), ConditionValue(0.0f) {}
};

/**
 * @brief Tipo de callback de combate
 */
using CombatCallback = std::function<bool(const CombatEvent&)>;

/**
 * @brief Tipo de callback de dano
 */
using DamageCallback = std::function<uint32_t(uint32_t, uint32_t, uint32_t, DamageType, AttackType, bool)>;

/**
 * @brief Gerenciador de combate
 * 
 * Esta classe é responsável por gerenciar o combate entre jogadores e NPCs.
 */
/**
 * Classe WYDCombatManager
 * 
 * Esta classe implementa a funcionalidade WYDCombatManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CombatManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static CombatManager& getInstance() {
        static CombatManager instance;
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
     * @brief Carrega a configuração de combate
     * @return true se carregado com sucesso
     */
    bool loadCombatConfig();
    
    /**
     * @brief Executa um ataque
     * @param attackerID ID do atacante
     * @param targetID ID do alvo
     * @param attackType Tipo de ataque
     * @param skillID ID da habilidade (0 para ataque normal)
     * @param skillLevel Nível da habilidade
     * @param damageType Tipo de dano
     * @return Resultado do combate
     */
    CombatResult executeAttack(uint32_t attackerID, uint32_t targetID, AttackType attackType = AttackType::NORMAL,
                              uint16_t skillID = 0, uint8_t skillLevel = 0, DamageType damageType = DamageType::PHYSICAL);
    
    /**
     * @brief Calcula o dano de um ataque
     * @param attackerID ID do atacante
     * @param targetID ID do alvo
     * @param baseDamage Dano base
     * @param attackType Tipo de ataque
     * @param damageType Tipo de dano
     * @param isCritical É crítico
     * @return Dano calculado
     */
    uint32_t calculateDamage(uint32_t attackerID, uint32_t targetID, uint32_t baseDamage,
                           AttackType attackType = AttackType::NORMAL,
                           DamageType damageType = DamageType::PHYSICAL,
                           bool isCritical = false);
    
    /**
     * @brief Aplica dano a uma entidade
     * @param targetID ID do alvo
     * @param damage Dano
     * @param attackerID ID do atacante
     * @param damageType Tipo de dano
     * @param attackType Tipo de ataque
     * @param isCritical É crítico
     * @return Dano real aplicado
     */
    uint32_t applyDamage(uint32_t targetID, uint32_t damage, uint32_t attackerID = 0,
                       DamageType damageType = DamageType::PHYSICAL,
                       AttackType attackType = AttackType::NORMAL,
                       bool isCritical = false);
    
    /**
     * @brief Aplica cura a uma entidade
     * @param targetID ID do alvo
     * @param healing Cura
     * @param healerID ID do curador
     * @param skillID ID da habilidade
     * @param isCritical É crítico
     * @return Cura real aplicada
     */
    uint32_t applyHealing(uint32_t targetID, uint32_t healing, uint32_t healerID = 0,
                        uint16_t skillID = 0, bool isCritical = false);
    
    /**
     * @brief Verifica se um alvo é válido
     * @param attackerID ID do atacante
     * @param targetID ID do alvo
     * @return true se for válido
     */
    bool isValidTarget(uint32_t attackerID, uint32_t targetID);
    
    /**
     * @brief Entra em combate
     * @param entityID ID da entidade
     * @param targetID ID do alvo
     * @return true se entrou em combate
     */
    bool enterCombat(uint32_t entityID, uint32_t targetID);
    
    /**
     * @brief Sai de combate
     * @param entityID ID da entidade
     * @return true se saiu de combate
     */
    bool exitCombat(uint32_t entityID);
    
    /**
     * @brief Verifica se uma entidade está em combate
     * @param entityID ID da entidade
     * @return true se estiver em combate
     */
    bool isInCombat(uint32_t entityID) const;
    
    /**
     * @brief Adiciona um efeito de status
     * @param effect Efeito
     * @return ID do efeito ou 0 em caso de erro
     */
    uint32_t addStatusEffect(const CombatStatusEffect& effect);
    
    /**
     * @brief Remove um efeito de status
     * @param effectID ID do efeito
     * @return true se removido com sucesso
     */
    bool removeStatusEffect(uint32_t effectID);
    
    /**
     * @brief Remove efeitos de status de uma entidade
     * @param entityID ID da entidade
     * @param type Tipo (NONE para todos)
     * @return Número de efeitos removidos
     */
    uint32_t removeStatusEffects(uint32_t entityID, CombatStatusEffectType type = CombatStatusEffectType::NONE);
    
    /**
     * @brief Verifica se uma entidade tem um efeito de status
     * @param entityID ID da entidade
     * @param type Tipo
     * @return true se tiver
     */
    bool hasStatusEffect(uint32_t entityID, CombatStatusEffectType type) const;
    
    /**
     * @brief Obtém efeitos de status de uma entidade
     * @param entityID ID da entidade
     * @return Lista de efeitos
     */
    std::vector<CombatStatusEffect> getStatusEffects(uint32_t entityID) const;
    
    /**
     * @brief Adiciona um modificador de combate
     * @param modifier Modificador
     * @return ID do modificador ou 0 em caso de erro
     */
    uint32_t addCombatModifier(const CombatModifier& modifier);
    
    /**
     * @brief Remove um modificador de combate
     * @param modifierID ID do modificador
     * @return true se removido com sucesso
     */
    bool removeCombatModifier(uint32_t modifierID);
    
    /**
     * @brief Remove modificadores de combate de uma entidade
     * @param entityID ID da entidade
     * @param type Tipo (NONE para todos)
     * @return Número de modificadores removidos
     */
    uint32_t removeCombatModifiers(uint32_t entityID, CombatModifierType type = CombatModifierType::NONE);
    
    /**
     * @brief Verifica se uma entidade tem um modificador de combate
     * @param entityID ID da entidade
     * @param type Tipo
     * @return true se tiver
     */
    bool hasCombatModifier(uint32_t entityID, CombatModifierType type) const;
    
    /**
     * @brief Obtém modificadores de combate de uma entidade
     * @param entityID ID da entidade
     * @return Lista de modificadores
     */
    std::vector<CombatModifier> getCombatModifiers(uint32_t entityID) const;
    
    /**
     * @brief Gera ameaça
     * @param sourceID ID da fonte
     * @param targetID ID do alvo
     * @param amount Quantidade
     * @param type Tipo
     * @return true se gerado com sucesso
     */
    bool generateThreat(uint32_t sourceID, uint32_t targetID, uint32_t amount, ThreatGenerationType type = ThreatGenerationType::DAMAGE);
    
    /**
     * @brief Reduz ameaça
     * @param sourceID ID da fonte
     * @param targetID ID do alvo
     * @param amount Quantidade
     * @return true se reduzido com sucesso
     */
    bool reduceThreat(uint32_t sourceID, uint32_t targetID, uint32_t amount);
    
    /**
     * @brief Limpa a tabela de ameaça
     * @param entityID ID da entidade
     * @return true se limpo com sucesso
     */
    bool clearThreatTable(uint32_t entityID);
    
    /**
     * @brief Obtém a tabela de ameaça
     * @param entityID ID da entidade
     * @return Tabela de ameaça
     */
    std::map<uint32_t, uint32_t> getThreatTable(uint32_t entityID) const;
    
    /**
     * @brief Obtém a entidade com maior ameaça
     * @param entityID ID da entidade
     * @return ID da entidade com maior ameaça ou 0 se não houver
     */
    uint32_t getHighestThreatEntity(uint32_t entityID) const;
    
    /**
     * @brief Processa a morte de uma entidade
     * @param entityID ID da entidade
     * @param killerID ID do assassino
     * @return true se processado com sucesso
     */
    bool processDeath(uint32_t entityID, uint32_t killerID = 0);
    
    /**
     * @brief Ressuscita uma entidade
     * @param entityID ID da entidade
     * @param ressurectorID ID do ressuscitador
     * @param healthPercentage Percentual de saúde
     * @return true se ressuscitado com sucesso
     */
    bool resurrect(uint32_t entityID, uint32_t ressurectorID = 0, float healthPercentage = 1.0f);
    
    /**
     * @brief Verifica se uma entidade está morta
     * @param entityID ID da entidade
     * @return true se estiver morta
     */
    bool isDead(uint32_t entityID) const;
    
    /**
     * @brief Cria uma zona de combate
     * @param type Tipo
     * @param mapID ID do mapa
     * @param centerX Centro X
     * @param centerY Centro Y
     * @param radius Raio
     * @param allowPVP Permite PVP
     * @param allowPVE Permite PVE
     * @return ID da zona ou 0 em caso de erro
     */
    uint32_t createCombatZone(CombatZoneType type, uint16_t mapID, uint16_t centerX, uint16_t centerY, uint16_t radius,
                             bool allowPVP = false, bool allowPVE = true);
    
    /**
     * @brief Remove uma zona de combate
     * @param zoneID ID da zona
     * @return true se removido com sucesso
     */
    bool removeCombatZone(uint32_t zoneID);
    
    /**
     * @brief Verifica se uma posição está em uma zona de combate
     * @param mapID ID do mapa
     * @param posX Posição X
     * @param posY Posição Y
     * @param zoneType Tipo de zona (NONE para qualquer)
     * @return ID da zona ou 0 se não estiver
     */
    uint32_t isInCombatZone(uint16_t mapID, uint16_t posX, uint16_t posY, CombatZoneType zoneType = CombatZoneType::NONE) const;
    
    /**
     * @brief Obtém o tipo de zona em uma posição
     * @param mapID ID do mapa
     * @param posX Posição X
     * @param posY Posição Y
     * @return Tipo de zona
     */
    CombatZoneType getCombatZoneType(uint16_t mapID, uint16_t posX, uint16_t posY) const;
    
    /**
     * @brief Obtém o resultado de um ataque
     * @param attackerID ID do atacante
     * @param targetID ID do alvo
     * @param attackType Tipo de ataque
     * @param damageType Tipo de dano
     * @param skillID ID da habilidade
     * @return Resultado do ataque
     */
    AttackResult getAttackResult(uint32_t attackerID, uint32_t targetID, AttackType attackType = AttackType::NORMAL,
                               DamageType damageType = DamageType::PHYSICAL, uint16_t skillID = 0);
    
    /**
     * @brief Verifica se uma entidade pode atacar outra
     * @param attackerID ID do atacante
     * @param targetID ID do alvo
     * @return true se puder atacar
     */
    bool canAttack(uint32_t attackerID, uint32_t targetID);
    
    /**
     * @brief Verifica se uma entidade é hostil a outra
     * @param entityID1 ID da entidade 1
     * @param entityID2 ID da entidade 2
     * @return true se for hostil
     */
    bool isHostile(uint32_t entityID1, uint32_t entityID2);
    
    /**
     * @brief Adiciona uma resposta automática
     * @param response Resposta
     * @return ID da resposta ou 0 em caso de erro
     */
    uint32_t addAutoResponse(const AutoResponse& response);
    
    /**
     * @brief Remove uma resposta automática
     * @param responseID ID da resposta
     * @return true se removido com sucesso
     */
    bool removeAutoResponse(uint32_t responseID);
    
    /**
     * @brief Processa respostas automáticas
     * @param entityID ID da entidade
     * @param trigger Gatilho
     * @param sourceID ID da fonte
     * @param value Valor
     * @return Número de respostas processadas
     */
    uint32_t processAutoResponses(uint32_t entityID, ResponseTrigger trigger, uint32_t sourceID = 0, uint32_t value = 0);
    
    /**
     * @brief Obtém as estatísticas de combate de uma entidade
     * @param entityID ID da entidade
     * @return Estatísticas
     */
    CombatStats getCombatStats(uint32_t entityID) const;
    
    /**
     * @brief Reseta as estatísticas de combate de uma entidade
     * @param entityID ID da entidade
     * @return true se resetado com sucesso
     */
    bool resetCombatStats(uint32_t entityID);
    
    /**
     * @brief Atualiza as estatísticas de combate de uma entidade
     * @param entityID ID da entidade
     * @param damageDealt Dano causado
     * @param damageTaken Dano recebido
     * @param healingDone Cura causada
     * @param healingReceived Cura recebida
     * @param isCritical É crítico
     * @param isDodge É esquiva
     * @param isParry É aparada
     * @param isBlock É bloqueio
     * @param isMiss É erro
     * @param damageType Tipo de dano
     * @param skillID ID da habilidade
     * @return true se atualizado com sucesso
     */
    bool updateCombatStats(uint32_t entityID, uint32_t damageDealt = 0, uint32_t damageTaken = 0,
                          uint32_t healingDone = 0, uint32_t healingReceived = 0,
                          bool isCritical = false, bool isDodge = false, bool isParry = false,
                          bool isBlock = false, bool isMiss = false,
                          DamageType damageType = DamageType::NONE, uint16_t skillID = 0);
    
    /**
     * @brief Registra um abate
     * @param killerID ID do assassino
     * @param victimID ID da vítima
     * @return true se registrado com sucesso
     */
    bool registerKill(uint32_t killerID, uint32_t victimID);
    
    /**
     * @brief Registra uma morte
     * @param entityID ID da entidade
     * @param killerID ID do assassino
     * @return true se registrado com sucesso
     */
    bool registerDeath(uint32_t entityID, uint32_t killerID = 0);
    
    /**
     * @brief Adiciona um log de combate
     * @param log Log
     * @return ID do log ou 0 em caso de erro
     */
    uint32_t addCombatLog(const CombatLog& log);
    
    /**
     * @brief Obtém logs de combate de uma entidade
     * @param entityID ID da entidade
     * @param limit Limite (0 para todos)
     * @param offset Deslocamento
     * @return Lista de logs
     */
    std::vector<CombatLog> getCombatLogs(uint32_t entityID, uint32_t limit = 0, uint32_t offset = 0) const;
    
    /**
     * @brief Registra um callback de combate
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerCombatCallback(CombatCallback callback);
    
    /**
     * @brief Remove um callback de combate
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterCombatCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de dano
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerDamageCallback(DamageCallback callback);
    
    /**
     * @brief Remove um callback de dano
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterDamageCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém o tempo desde o último ataque
     * @param entityID ID da entidade
     * @return Tempo em milissegundos
     */
    uint32_t getTimeSinceLastAttack(uint32_t entityID) const;
    
    /**
     * @brief Obtém o tempo desde o último dano
     * @param entityID ID da entidade
     * @return Tempo em milissegundos
     */
    uint32_t getTimeSinceLastDamaged(uint32_t entityID) const;
    
    /**
     * @brief Obtém o tempo em combate
     * @param entityID ID da entidade
     * @return Tempo em milissegundos
     */
    uint32_t getTimeInCombat(uint32_t entityID) const;
    
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
     * @brief Define o fator de dano global
     * @param factor Fator
     */
    void setGlobalDamageFactor(float factor);
    
    /**
     * @brief Obtém o fator de dano global
     * @return Fator
     */
    float getGlobalDamageFactor() const;
    
    /**
     * @brief Define o fator de cura global
     * @param factor Fator
     */
    void setGlobalHealingFactor(float factor);
    
    /**
     * @brief Obtém o fator de cura global
     * @return Fator
     */
    float getGlobalHealingFactor() const;
    
    /**
     * @brief Habilita ou desabilita o PVP
     * @param enabled Habilitado
     */
    void setPVPEnabled(bool enabled);
    
    /**
     * @brief Verifica se o PVP está habilitado
     * @return true se estiver habilitado
     */
    bool isPVPEnabled() const;
    
    /**
     * @brief Habilita ou desabilita o PVP mundial
     * @param enabled Habilitado
     */
    void setWorldPVPEnabled(bool enabled);
    
    /**
     * @brief Verifica se o PVP mundial está habilitado
     * @return true se estiver habilitado
     */
    bool isWorldPVPEnabled() const;
    
    /**
     * @brief Obtém a configuração de combate
     * @return Configuração
     */
    CombatConfig getCombatConfig() const;
    
    /**
     * @brief Define a configuração de combate
     * @param config Configuração
     */
    void setCombatConfig(const CombatConfig& config);
    
    /**
     * @brief Obtém o estado global de combate
     * @return Estado global
     */
    GlobalCombatState getGlobalCombatState() const;
    
private:
    /**
     * @brief Construtor privado
     */
    CombatManager();
    
    /**
     * @brief Destrutor privado
     */
    ~CombatManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    CombatManager(const CombatManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    CombatManager& operator=(const CombatManager&) = delete;
    
    // Estados de entidade
    std::unordered_map<uint32_t, CombatEntityState> _entityStates;
    
    // Ataques
    std::atomic<uint32_t> _nextAttackID;
    
    // Defesas
    std::unordered_map<uint32_t, Defense> _entityDefenses;
    std::atomic<uint32_t> _nextDefenseID;
    
    // Resultados
    std::atomic<uint32_t> _nextResultID;
    std::queue<CombatResult> _recentResults;
    
    // Efeitos de status
    std::unordered_map<uint32_t, CombatStatusEffect> _statusEffects;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _entityStatusEffects;
    std::atomic<uint32_t> _nextEffectID;
    
    // Modificadores
    std::unordered_map<uint32_t, CombatModifier> _combatModifiers;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _entityModifiers;
    std::atomic<uint32_t> _nextModifierID;
    
    // Zonas
    std::unordered_map<uint32_t, CombatZone> _combatZones;
    std::unordered_map<uint16_t, std::vector<uint32_t>> _mapZones;
    std::atomic<uint32_t> _nextZoneID;
    
    // Respostas automáticas
    std::unordered_map<uint32_t, AutoResponse> _autoResponses;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _entityResponses;
    std::unordered_map<ResponseTrigger, std::vector<uint32_t>> _triggerResponses;
    std::atomic<uint32_t> _nextResponseID;
    
    // Estatísticas
    std::unordered_map<uint32_t, CombatStats> _combatStats;
    
    // Logs
    std::unordered_map<uint32_t, std::vector<CombatLog>> _combatLogs;
    std::atomic<uint32_t> _nextLogID;
    
    // Configuração
    CombatConfig _combatConfig;
    GlobalCombatState _globalState;
    
    // Callbacks
    std::vector<CombatCallback> _combatCallbacks;
    std::vector<DamageCallback> _damageCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _entityMutex;
    mutable std::mutex _attackMutex;
    mutable std::mutex _defenseMutex;
    mutable std::mutex _resultMutex;
    mutable std::mutex _effectMutex;
    mutable std::mutex _modifierMutex;
    mutable std::mutex _zoneMutex;
    mutable std::mutex _responseMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _logMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    // Gerador de números aleatórios
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _distribution;
    
    // Timers
    uint32_t _combatExitTimer;
    uint32_t _statusEffectTimer;
    uint32_t _modifierTimer;
    
    /**
     * @brief Atualiza estados de entidades
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void updateEntityStates(uint32_t deltaTime);
    
    /**
     * @brief Atualiza efeitos de status
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void updateStatusEffects(uint32_t deltaTime);
    
    /**
     * @brief Atualiza modificadores
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void updateModifiers(uint32_t deltaTime);
    
    /**
     * @brief Calcula a chance de acerto
     * @param attackerID ID do atacante
     * @param targetID ID do alvo
     * @param attackType Tipo de ataque
     * @param skillID ID da habilidade
     * @return Chance de acerto
     */
    float calculateHitChance(uint32_t attackerID, uint32_t targetID, AttackType attackType, uint16_t skillID);
    
    /**
     * @brief Calcula a chance de crítico
     * @param attackerID ID do atacante
     * @param targetID ID do alvo
     * @param attackType Tipo de ataque
     * @param skillID ID da habilidade
     * @return Chance de crítico
     */
    float calculateCriticalChance(uint32_t attackerID, uint32_t targetID, AttackType attackType, uint16_t skillID);
    
    /**
     * @brief Calcula a chance de esquiva
     * @param targetID ID do alvo
     * @param attackerID ID do atacante
     * @param attackType Tipo de ataque
     * @return Chance de esquiva
     */
    float calculateDodgeChance(uint32_t targetID, uint32_t attackerID, AttackType attackType);
    
    /**
     * @brief Calcula a chance de aparar
     * @param targetID ID do alvo
     * @param attackerID ID do atacante
     * @param attackType Tipo de ataque
     * @return Chance de aparar
     */
    float calculateParryChance(uint32_t targetID, uint32_t attackerID, AttackType attackType);
    
    /**
     * @brief Calcula a chance de bloqueio
     * @param targetID ID do alvo
     * @param attackerID ID do atacante
     * @param attackType Tipo de ataque
     * @return Chance de bloqueio
     */
    float calculateBlockChance(uint32_t targetID, uint32_t attackerID, AttackType attackType);
    
    /**
     * @brief Calcula a chance de resistência
     * @param targetID ID do alvo
     * @param attackerID ID do atacante
     * @param damageType Tipo de dano
     * @return Chance de resistência
     */
    float calculateResistChance(uint32_t targetID, uint32_t attackerID, DamageType damageType);
    
    /**
     * @brief Calcula o multiplicador de dano
     * @param attackerID ID do atacante
     * @param targetID ID do alvo
     * @param damageType Tipo de dano
     * @param attackType Tipo de ataque
     * @param isCritical É crítico
     * @return Multiplicador de dano
     */
    float calculateDamageMultiplier(uint32_t attackerID, uint32_t targetID, DamageType damageType, AttackType attackType, bool isCritical);
    
    /**
     * @brief Processa a aplicação de um efeito de status
     * @param effect Efeito
     * @return true se processado com sucesso
     */
    bool processStatusEffectApplication(const CombatStatusEffect& effect);
    
    /**
     * @brief Processa a remoção de um efeito de status
     * @param effectID ID do efeito
     * @return true se processado com sucesso
     */
    bool processStatusEffectRemoval(uint32_t effectID);
    
    /**
     * @brief Processa o tick de um efeito de status
     * @param effectID ID do efeito
     * @param currentTime Tempo atual
     * @return true se processado com sucesso
     */
    bool processStatusEffectTick(uint32_t effectID, uint32_t currentTime);
    
    /**
     * @brief Executa callbacks de combate
     * @param event Evento
     * @return true se processado com sucesso
     */
    bool executeCombatCallbacks(const CombatEvent& event);
    
    /**
     * @brief Executa callbacks de dano
     * @param attackerID ID do atacante
     * @param targetID ID do alvo
     * @param damage Dano
     * @param damageType Tipo de dano
     * @param attackType Tipo de ataque
     * @param isCritical É crítico
     * @return Dano modificado
     */
    uint32_t executeDamageCallbacks(uint32_t attackerID, uint32_t targetID, uint32_t damage, DamageType damageType, AttackType attackType, bool isCritical);
    
    /**
     * @brief Loga um evento de combate
     * @param event Evento
     * @return ID do log
     */
    uint32_t logCombatEvent(const CombatEvent& event);
    
    /**
     * @brief Gera um número aleatório entre min e max
     * @param min Mínimo
     * @param max Máximo
     * @return Número aleatório
     */
    int getRandomInt(int min, int max);
    
    /**
     * @brief Gera um número aleatório entre 0 e 1
     * @return Número aleatório
     */
    float getRandomFloat();
    
    /**
     * @brief Verifica se um evento aleatório ocorre com base na probabilidade
     * @param probability Probabilidade
     * @return true se o evento ocorrer
     */
    bool randomChance(float probability);
    
    /**
     * @brief Carrega dados de combate do banco de dados
     * @return true se carregado com sucesso
     */
    bool loadCombatFromDatabase();
    
    /**
     * @brief Salva dados de combate no banco de dados
     * @return true se salvo com sucesso
     */
    bool saveCombatToDatabase();
};

} // namespace combat
} // namespace wydbr

#endif // _COMBAT_MANAGER_H_