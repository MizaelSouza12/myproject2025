/**
 * SkillManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/skill/SkillManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _SKILL_MANAGER_H_
#define _SKILL_MANAGER_H_

/**
 * @file SkillManager.h
 * @brief Gerenciador de habilidades do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de habilidades do WYDBR,
 * responsável por gerenciar o uso e efeitos de habilidades do jogo,
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

#include "../GlobalDef.h"
#include "../Basedef.h"
#include "../TM_Skill.h"

namespace wydbr {
namespace skill {

/**
 * @brief Resultado da operação de habilidade
 */
enum /**
 * Classe WYDSkillOperationResult
 * 
 * Esta classe implementa a funcionalidade WYDSkillOperationResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillOperationResult {
    SUCCESS = 0,                // Operação bem-sucedida
    INVALID_SKILL = 1,          // Habilidade inválida
    INVALID_TARGET = 2,         // Alvo inválido
    INVALID_POSITION = 3,       // Posição inválida
    NOT_LEARNED = 4,            // Não aprendida
    NOT_ENOUGH_MP = 5,          // MP insuficiente
    NOT_ENOUGH_HP = 6,          // HP insuficiente
    NOT_ENOUGH_ITEM = 7,        // Item insuficiente
    NOT_ENOUGH_STAMINA = 8,     // Stamina insuficiente
    ALREADY_CAST = 9,           // Já lançada
    ALREADY_LEARNED = 10,       // Já aprendida
    LEVEL_RESTRICTION = 11,     // Restrição de nível
    CLASS_RESTRICTION = 12,     // Restrição de classe
    WEAPON_RESTRICTION = 13,    // Restrição de arma
    ARMOR_RESTRICTION = 14,     // Restrição de armadura
    STATUS_RESTRICTION = 15,    // Restrição de status
    GUILD_RESTRICTION = 16,     // Restrição de guilda
    DISTANCE_RESTRICTION = 17,  // Restrição de distância
    MAP_RESTRICTION = 18,       // Restrição de mapa
    TIME_RESTRICTION = 19,      // Restrição de tempo
    AREA_RESTRICTION = 20,      // Restrição de área
    TARGET_RESTRICTION = 21,    // Restrição de alvo
    COOLDOWN_RESTRICTION = 22,  // Restrição de cooldown
    CANCELED = 23,              // Cancelada
    STUNNED = 24,               // Atordoado
    SILENCED = 25,              // Silenciado
    FEARERD = 26,               // Amedrontado
    RESTRICTED = 27,            // Restrito
    CASTER_DEAD = 28,           // Lançador morto
    TARGET_DEAD = 29,           // Alvo morto
    INVALID_STATE = 30,         // Estado inválido
    UNKNOWN_ERROR = 31          // Erro desconhecido
};

/**
 * @brief Tipo de evento de habilidade
 */
enum /**
 * Classe WYDSkillEventType
 * 
 * Esta classe implementa a funcionalidade WYDSkillEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillEventType {
    NONE = 0,                   // Nenhum
    LEARNED = 1,                // Aprendida
    FORGOTTEN = 2,              // Esquecida
    COOLDOWN_START = 3,         // Início de cooldown
    COOLDOWN_END = 4,           // Fim de cooldown
    CAST_START = 5,             // Início de lançamento
    CAST_COMPLETE = 6,          // Lançamento completo
    CAST_INTERRUPT = 7,         // Lançamento interrompido
    CAST_FAILED = 8,            // Lançamento falhou
    EFFECT_APPLIED = 9,         // Efeito aplicado
    EFFECT_REMOVED = 10,        // Efeito removido
    SKILL_USED = 11,            // Habilidade usada
    SKILL_HIT = 12,             // Habilidade acertou
    SKILL_MISS = 13,            // Habilidade errou
    SKILL_DODGE = 14,           // Habilidade esquivada
    SKILL_BLOCKED = 15,         // Habilidade bloqueada
    SKILL_RESISTED = 16,        // Habilidade resistida
    SKILL_CRITICAL = 17,        // Habilidade crítica
    SKILL_REFLECT = 18,         // Habilidade refletida
    BUFF_APPLIED = 19,          // Buff aplicado
    BUFF_REMOVED = 20,          // Buff removido
    DEBUFF_APPLIED = 21,        // Debuff aplicado
    DEBUFF_REMOVED = 22,        // Debuff removido
    SKILL_UPGRADED = 23,        // Habilidade melhorada
    SKILL_READY = 24,           // Habilidade pronta
    PASSIVE_TRIGGERED = 25,     // Passiva acionada
    SKILL_AREA_ENTERED = 26,    // Área de habilidade entrada
    SKILL_AREA_EXITED = 27,     // Área de habilidade saída
    SKILL_DURATION_END = 28,    // Duração de habilidade terminou
    CUSTOM_1 = 29,              // Personalizado 1
    CUSTOM_2 = 30,              // Personalizado 2
    CUSTOM_3 = 31,              // Personalizado 3
    UNKNOWN = 32                // Desconhecido
};

/**
 * @brief Estado da habilidade
 */
enum /**
 * Classe WYDSkillState
 * 
 * Esta classe implementa a funcionalidade WYDSkillState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillState {
    NONE = 0,                   // Nenhum
    READY = 1,                  // Pronta
    COOLDOWN = 2,               // Em cooldown
    CASTING = 3,                // Lançando
    CHANNELING = 4,             // Canalizando
    PREPARING = 5,              // Preparando
    DISABLED = 6,               // Desabilitada
    ACTIVE = 7,                 // Ativa
    TRIGGERED = 8,              // Acionada
    CUSTOM_1 = 9,               // Personalizado 1
    CUSTOM_2 = 10,              // Personalizado 2
    CUSTOM_3 = 11,              // Personalizado 3
    UNKNOWN = 12                // Desconhecido
};

/**
 * @brief Tipo de alvo da habilidade
 */
enum /**
 * Classe WYDSkillTargetType
 * 
 * Esta classe implementa a funcionalidade WYDSkillTargetType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillTargetType {
    NONE = 0,                   // Nenhum
    SELF = 1,                   // Si mesmo
    FRIENDLY = 2,               // Amigável
    ENEMY = 3,                  // Inimigo
    NEUTRAL = 4,                // Neutro
    AREA = 5,                   // Área
    GROUND = 6,                 // Chão
    CORPSE = 7,                 // Cadáver
    PET = 8,                    // Pet
    ALLY = 9,                   // Aliado
    PARTY = 10,                 // Grupo
    GUILD = 11,                 // Guilda
    NPC = 12,                   // NPC
    OBJECT = 13,                // Objeto
    CUSTOM_1 = 14,              // Personalizado 1
    CUSTOM_2 = 15,              // Personalizado 2
    CUSTOM_3 = 16,              // Personalizado 3
    UNKNOWN = 17                // Desconhecido
};

/**
 * @brief Tipo de habilidade
 */
enum /**
 * Classe WYDSkillCastType
 * 
 * Esta classe implementa a funcionalidade WYDSkillCastType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillCastType {
    NONE = 0,                   // Nenhum
    INSTANT = 1,                // Instantâneo
    CAST = 2,                   // Lançamento
    CHANNEL = 3,                // Canalização
    PASSIVE = 4,                // Passivo
    TOGGLE = 5,                 // Alternado
    CHARGE = 6,                 // Carregado
    COMBO = 7,                  // Combo
    AREA_TARGET = 8,            // Alvo em área
    LINE_TARGET = 9,            // Alvo em linha
    CONE_TARGET = 10,           // Alvo em cone
    SELF_TARGET = 11,           // Alvo em si mesmo
    AUTO_TARGET = 12,           // Alvo automático
    CUSTOM_1 = 13,              // Personalizado 1
    CUSTOM_2 = 14,              // Personalizado 2
    CUSTOM_3 = 15,              // Personalizado 3
    UNKNOWN = 16                // Desconhecido
};

/**
 * @brief Tipo de efeito de habilidade
 */
enum /**
 * Classe WYDSkillEffectType
 * 
 * Esta classe implementa a funcionalidade WYDSkillEffectType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillEffectType {
    NONE = 0,                   // Nenhum
    DAMAGE = 1,                 // Dano
    HEAL = 2,                   // Cura
    BUFF = 3,                   // Buff
    DEBUFF = 4,                 // Debuff
    TELEPORT = 5,               // Teleporte
    SUMMON = 6,                 // Invocar
    CONTROL = 7,                // Controle
    DISPEL = 8,                 // Dissipar
    TRANSFORM = 9,              // Transformar
    AREA_EFFECT = 10,           // Efeito em área
    PROJECTILE = 11,            // Projétil
    MOVEMENT = 12,              // Movimento
    REVIVAL = 13,               // Reviver
    RESOURCE = 14,              // Recurso
    STATUS = 15,                // Status
    TRAP = 16,                  // Armadilha
    STEALTH = 17,               // Furtividade
    SHIELD = 18,                // Escudo
    REFLECT = 19,               // Refletir
    DRAIN = 20,                 // Drenar
    TAUNT = 21,                 // Provocar
    FEAR = 22,                  // Medo
    STUN = 23,                  // Atordoar
    SILENCE = 24,               // Silenciar
    ROOT = 25,                  // Enraizar
    KNOCKBACK = 26,             // Repelir
    PULL = 27,                  // Puxar
    CLEANSE = 28,               // Limpar
    CUSTOM_1 = 29,              // Personalizado 1
    CUSTOM_2 = 30,              // Personalizado 2
    CUSTOM_3 = 31,              // Personalizado 3
    UNKNOWN = 32                // Desconhecido
};

/**
 * @brief Escola de habilidade
 */
enum /**
 * Classe WYDSkillSchool
 * 
 * Esta classe implementa a funcionalidade WYDSkillSchool conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillSchool {
    NONE = 0,                   // Nenhum
    PHYSICAL = 1,               // Físico
    MAGICAL = 2,                // Mágico
    DIVINE = 3,                 // Divino
    NATURE = 4,                 // Natureza
    SHADOW = 5,                 // Sombra
    FIRE = 6,                   // Fogo
    WATER = 7,                  // Água
    EARTH = 8,                  // Terra
    AIR = 9,                    // Ar
    LIGHTNING = 10,             // Relâmpago
    ICE = 11,                   // Gelo
    POISON = 12,                // Veneno
    ARCANE = 13,                // Arcano
    HOLY = 14,                  // Sagrado
    UNHOLY = 15,                // Profano
    MENTAL = 16,                // Mental
    CUSTOM_1 = 17,              // Personalizado 1
    CUSTOM_2 = 18,              // Personalizado 2
    CUSTOM_3 = 19,              // Personalizado 3
    UNKNOWN = 20                // Desconhecido
};

/**
 * @brief Tipo de proteção contra skill
 */
enum /**
 * Classe WYDSkillResistType
 * 
 * Esta classe implementa a funcionalidade WYDSkillResistType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillResistType {
    NONE = 0,                   // Nenhum
    PHYSICAL = 1,               // Físico
    MAGICAL = 2,                // Mágico
    FIRE = 3,                   // Fogo
    WATER = 4,                  // Água
    EARTH = 5,                  // Terra
    AIR = 6,                    // Ar
    LIGHTNING = 7,              // Relâmpago
    ICE = 8,                    // Gelo
    POISON = 9,                 // Veneno
    HOLY = 10,                  // Sagrado
    SHADOW = 11,                // Sombra
    ARCANE = 12,                // Arcano
    DISEASE = 13,               // Doença
    MENTAL = 14,                // Mental
    CUSTOM_1 = 15,              // Personalizado 1
    CUSTOM_2 = 16,              // Personalizado 2
    CUSTOM_3 = 17,              // Personalizado 3
    UNKNOWN = 18                // Desconhecido
};

/**
 * @brief Tipo de requerimento de habilidade
 */
enum /**
 * Classe WYDSkillRequirementType
 * 
 * Esta classe implementa a funcionalidade WYDSkillRequirementType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillRequirementType {
    NONE = 0,                   // Nenhum
    LEVEL = 1,                  // Nível
    CLASS = 2,                  // Classe
    STAT = 3,                   // Estatística
    SKILL = 4,                  // Habilidade
    ITEM = 5,                   // Item
    WEAPON = 6,                 // Arma
    ARMOR = 7,                  // Armadura
    GUILD = 8,                  // Guilda
    QUEST = 9,                  // Missão
    MAP = 10,                   // Mapa
    TIME = 11,                  // Tempo
    STATUS = 12,                // Status
    COMBAT_STATE = 13,          // Estado de combate
    MOUNT = 14,                 // Montaria
    PET = 15,                   // Pet
    RESOURCE = 16,              // Recurso
    CUSTOM_1 = 17,              // Personalizado 1
    CUSTOM_2 = 18,              // Personalizado 2
    CUSTOM_3 = 19,              // Personalizado 3
    UNKNOWN = 20                // Desconhecido
};

/**
 * @brief Contexto de execução de habilidade
 */
struct SkillExecutionContext {
    uint32_t CasterID;              // ID do lançador
    uint32_t TargetID;              // ID do alvo
    uint16_t SkillID;               // ID da habilidade
    uint8_t SkillLevel;             // Nível da habilidade
    uint16_t PosX;                  // Posição X
    uint16_t PosY;                  // Posição Y
    uint8_t Direction;              // Direção
    uint32_t Timestamp;             // Timestamp
    bool IsForced;                  // É forçado
    bool IsServerTriggered;         // É acionado pelo servidor
    bool AllowCooldownBypass;       // Permite bypass de cooldown
    bool IgnoreRequirements;        // Ignora requerimentos
    bool IgnoreRestrictions;        // Ignora restrições
    
    SkillExecutionContext() : CasterID(0), TargetID(0), SkillID(0), SkillLevel(0),
        PosX(0), PosY(0), Direction(0), Timestamp(0), IsForced(false),
        IsServerTriggered(false), AllowCooldownBypass(false), IgnoreRequirements(false),
        IgnoreRestrictions(false) {}
};

/**
 * @brief Informações de execução de habilidade
 */
struct SkillExecutionInfo {
    uint32_t ExecutionID;           // ID da execução
    uint32_t CasterID;              // ID do lançador
    uint32_t TargetID;              // ID do alvo
    uint16_t SkillID;               // ID da habilidade
    uint8_t SkillLevel;             // Nível da habilidade
    uint16_t PosX;                  // Posição X
    uint16_t PosY;                  // Posição Y
    uint8_t Direction;              // Direção
    uint32_t StartTime;             // Tempo de início
    uint32_t EndTime;               // Tempo de fim
    uint32_t CooldownEndTime;       // Tempo de fim de cooldown
    SkillOperationResult Result;    // Resultado
    SkillState State;               // Estado
    uint32_t Damage;                // Dano
    uint32_t Healing;               // Cura
    bool IsCritical;                // É crítico
    bool IsDodged;                  // É esquivado
    bool IsBlocked;                 // É bloqueado
    bool IsResisted;                // É resistido
    std::vector<uint32_t> AffectedTargets; // Alvos afetados
    
    SkillExecutionInfo() : ExecutionID(0), CasterID(0), TargetID(0), SkillID(0),
        SkillLevel(0), PosX(0), PosY(0), Direction(0), StartTime(0), EndTime(0),
        CooldownEndTime(0), Result(SkillOperationResult::SUCCESS), State(SkillState::NONE),
        Damage(0), Healing(0), IsCritical(false), IsDodged(false), IsBlocked(false),
        IsResisted(false) {}
};

/**
 * @brief Informações de uma habilidade
 */
struct SkillInfo {
    uint16_t SkillID;               // ID da habilidade
    std::string Name;               // Nome
    std::string Description;        // Descrição
    uint8_t MaxLevel;               // Nível máximo
    uint8_t Type;                   // Tipo
    uint16_t Icon;                  // Ícone
    uint16_t Animation;             // Animação
    uint16_t Effect;                // Efeito
    uint16_t Sound;                 // Som
    uint16_t CastTime;              // Tempo de lançamento
    uint16_t Cooldown;              // Cooldown
    uint16_t Duration;              // Duração
    uint16_t Range;                 // Alcance
    uint16_t AreaRadius;            // Raio da área
    uint16_t ManaCost;              // Custo de mana
    uint16_t HPCost;                // Custo de HP
    uint16_t StaminaCost;           // Custo de stamina
    uint16_t ItemCost;              // Custo de item
    uint16_t ItemID;                // ID do item
    uint8_t TargetType;             // Tipo de alvo
    uint8_t SchoolType;             // Tipo de escola
    uint8_t DamageType;             // Tipo de dano
    uint16_t BaseDamage;            // Dano base
    uint16_t BaseHealing;           // Cura base
    float CriticalChance;           // Chance de crítico
    float DamageMultiplier;         // Multiplicador de dano
    float HealingMultiplier;        // Multiplicador de cura
    float LifeSteal;                // Roubo de vida
    float ManaSteal;                // Roubo de mana
    bool IsPassive;                 // É passiva
    bool IsToggle;                  // É alternável
    bool IsAutoTarget;              // É alvo automático
    bool RequiresWeapon;            // Requer arma
    bool RequiresShield;            // Requer escudo
    bool CanBeBlocked;              // Pode ser bloqueado
    bool CanBeDodged;               // Pode ser esquivado
    bool CanBeResisted;             // Pode ser resistido
    uint8_t ClassRequired;          // Classe requerida
    uint8_t LevelRequired;          // Nível requerido
    uint8_t WeaponTypeRequired;     // Tipo de arma requerido
    uint8_t ArmorTypeRequired;      // Tipo de armadura requerido
    uint8_t StatRequired;           // Estatística requerida
    uint8_t StatRequiredValue;      // Valor de estatística requerido
    uint16_t SkillRequired;         // Habilidade requerida
    uint8_t SkillRequiredLevel;     // Nível de habilidade requerido
    
    SkillInfo() : SkillID(0), MaxLevel(0), Type(0), Icon(0), Animation(0),
        Effect(0), Sound(0), CastTime(0), Cooldown(0), Duration(0), Range(0),
        AreaRadius(0), ManaCost(0), HPCost(0), StaminaCost(0), ItemCost(0),
        ItemID(0), TargetType(0), SchoolType(0), DamageType(0), BaseDamage(0),
        BaseHealing(0), CriticalChance(0.0f), DamageMultiplier(1.0f),
        HealingMultiplier(1.0f), LifeSteal(0.0f), ManaSteal(0.0f),
        IsPassive(false), IsToggle(false), IsAutoTarget(false),
        RequiresWeapon(false), RequiresShield(false), CanBeBlocked(true),
        CanBeDodged(true), CanBeResisted(true), ClassRequired(0),
        LevelRequired(0), WeaponTypeRequired(0), ArmorTypeRequired(0),
        StatRequired(0), StatRequiredValue(0), SkillRequired(0),
        SkillRequiredLevel(0) {}
};

/**
 * @brief Estatísticas de habilidade
 */
struct SkillStats {
    uint32_t TotalUses;             // Total de usos
    uint32_t Hits;                  // Acertos
    uint32_t Misses;                // Erros
    uint32_t Criticals;             // Críticos
    uint32_t Dodged;                // Esquivados
    uint32_t Blocked;               // Bloqueados
    uint32_t Resisted;              // Resistidos
    uint32_t TotalDamage;           // Dano total
    uint32_t TotalHealing;          // Cura total
    uint32_t MaxDamage;             // Dano máximo
    uint32_t MinDamage;             // Dano mínimo
    uint32_t MaxHealing;            // Cura máxima
    uint32_t MinHealing;            // Cura mínima
    uint32_t TotalCasts;            // Total de lançamentos
    uint32_t FailedCasts;           // Lançamentos falhos
    uint32_t InterruptedCasts;      // Lançamentos interrompidos
    uint32_t CompletedCasts;        // Lançamentos completos
    uint32_t TotalManaUsed;         // Mana total usada
    uint32_t TotalHPUsed;           // HP total usado
    uint32_t TotalStaminaUsed;      // Stamina total usada
    uint32_t TotalItemsUsed;        // Total de itens usados
    
    SkillStats() : TotalUses(0), Hits(0), Misses(0), Criticals(0),
        Dodged(0), Blocked(0), Resisted(0), TotalDamage(0), TotalHealing(0),
        MaxDamage(0), MinDamage(0), MaxHealing(0), MinHealing(0),
        TotalCasts(0), FailedCasts(0), InterruptedCasts(0), CompletedCasts(0),
        TotalManaUsed(0), TotalHPUsed(0), TotalStaminaUsed(0), TotalItemsUsed(0) {}
};

/**
 * @brief Efeito de habilidade
 */
struct SkillEffect {
    uint32_t EffectID;              // ID do efeito
    uint16_t SkillID;               // ID da habilidade
    uint8_t SkillLevel;             // Nível da habilidade
    uint32_t CasterID;              // ID do lançador
    uint32_t TargetID;              // ID do alvo
    SkillEffectType Type;           // Tipo
    uint32_t StartTime;             // Tempo de início
    uint32_t EndTime;               // Tempo de fim
    uint32_t Duration;              // Duração
    uint32_t Interval;              // Intervalo
    uint32_t TickCount;             // Contagem de ticks
    uint32_t LastTickTime;          // Último tempo de tick
    uint32_t Stacks;                // Pilhas
    uint32_t MaxStacks;             // Pilhas máximas
    uint32_t StackRefreshTime;      // Tempo de atualização de pilha
    bool CanRefresh;                // Pode atualizar
    bool CanStack;                  // Pode empilhar
    bool IsPositive;                // É positivo
    bool IsRemovable;               // É removível
    bool IsHidden;                  // É escondido
    int32_t ModHP;                  // Modificador de HP
    int32_t ModMP;                  // Modificador de MP
    int32_t ModStamina;             // Modificador de stamina
    int32_t ModStr;                 // Modificador de força
    int32_t ModInt;                 // Modificador de inteligência
    int32_t ModDex;                 // Modificador de destreza
    int32_t ModCon;                 // Modificador de constituição
    int32_t ModAttack;              // Modificador de ataque
    int32_t ModDefense;             // Modificador de defesa
    int32_t ModMagicAttack;         // Modificador de ataque mágico
    int32_t ModMagicDefense;        // Modificador de defesa mágica
    int32_t ModAttackSpeed;         // Modificador de velocidade de ataque
    int32_t ModMoveSpeed;           // Modificador de velocidade de movimento
    int32_t ModCastSpeed;           // Modificador de velocidade de lançamento
    int32_t ModCriticalChance;      // Modificador de chance de crítico
    int32_t ModCriticalDamage;      // Modificador de dano crítico
    int32_t ModHitRate;             // Modificador de taxa de acerto
    int32_t ModDodgeRate;           // Modificador de taxa de esquiva
    int32_t ModBlockRate;           // Modificador de taxa de bloqueio
    int32_t ModResistRate;          // Modificador de taxa de resistência
    
    SkillEffect() : EffectID(0), SkillID(0), SkillLevel(0), CasterID(0),
        TargetID(0), Type(SkillEffectType::NONE), StartTime(0), EndTime(0),
        Duration(0), Interval(0), TickCount(0), LastTickTime(0), Stacks(0),
        MaxStacks(0), StackRefreshTime(0), CanRefresh(true), CanStack(false),
        IsPositive(true), IsRemovable(true), IsHidden(false), ModHP(0),
        ModMP(0), ModStamina(0), ModStr(0), ModInt(0), ModDex(0), ModCon(0),
        ModAttack(0), ModDefense(0), ModMagicAttack(0), ModMagicDefense(0),
        ModAttackSpeed(0), ModMoveSpeed(0), ModCastSpeed(0), ModCriticalChance(0),
        ModCriticalDamage(0), ModHitRate(0), ModDodgeRate(0), ModBlockRate(0),
        ModResistRate(0) {}
};

/**
 * @brief Evento de habilidade
 */
struct SkillEvent {
    uint32_t EventID;               // ID do evento
    SkillEventType Type;            // Tipo
    uint16_t SkillID;               // ID da habilidade
    uint8_t SkillLevel;             // Nível da habilidade
    uint32_t CasterID;              // ID do lançador
    uint32_t TargetID;              // ID do alvo
    uint32_t ExecutionID;           // ID da execução
    uint32_t EffectID;              // ID do efeito
    uint32_t Value;                 // Valor
    uint32_t Timestamp;             // Timestamp
    
    SkillEvent() : EventID(0), Type(SkillEventType::NONE), SkillID(0),
        SkillLevel(0), CasterID(0), TargetID(0), ExecutionID(0), EffectID(0),
        Value(0), Timestamp(0) {}
    
    SkillEvent(SkillEventType _type, uint16_t _skillID, uint8_t _skillLevel,
              uint32_t _casterID, uint32_t _targetID, uint32_t _executionID = 0,
              uint32_t _effectID = 0, uint32_t _value = 0)
        : EventID(0), Type(_type), SkillID(_skillID), SkillLevel(_skillLevel),
          CasterID(_casterID), TargetID(_targetID), ExecutionID(_executionID),
          EffectID(_effectID), Value(_value),
          Timestamp(static_cast<uint32_t>(time(nullptr))) {}
};

/**
 * @brief Tipo de callback de habilidade
 */
using SkillCallback = std::function<SkillOperationResult(const SkillExecutionContext&, SkillExecutionInfo&)>;

/**
 * @brief Tipo de callback de efeito de habilidade
 */
using SkillEffectCallback = std::function<void(const SkillEffect&, uint32_t)>;

/**
 * @brief Tipo de callback de evento de habilidade
 */
using SkillEventCallback = std::function<void(const SkillEvent&)>;

/**
 * @brief Gerenciador de habilidades
 * 
 * Esta classe é responsável por gerenciar o uso e efeitos de habilidades do jogo.
 */
/**
 * Classe WYDSkillManager
 * 
 * Esta classe implementa a funcionalidade WYDSkillManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static SkillManager& getInstance() {
        static SkillManager instance;
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
     * @brief Carrega a configuração de habilidades
     * @return true se carregado com sucesso
     */
    bool loadSkillConfig();
    
    /**
     * @brief Carrega as habilidades de um personagem
     * @param characterID ID do personagem
     * @return true se carregado com sucesso
     */
    bool loadCharacterSkills(uint32_t characterID);
    
    /**
     * @brief Salva as habilidades de um personagem
     * @param characterID ID do personagem
     * @return true se salvo com sucesso
     */
    bool saveCharacterSkills(uint32_t characterID);
    
    /**
     * @brief Executa uma habilidade
     * @param ctx Contexto de execução
     * @param info Informações de execução (saída)
     * @return Resultado da operação
     */
    SkillOperationResult executeSkill(const SkillExecutionContext& ctx, SkillExecutionInfo& info);
    
    /**
     * @brief Executa uma habilidade por ID
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @param targetID ID do alvo
     * @param posX Posição X
     * @param posY Posição Y
     * @param info Informações de execução (saída)
     * @return Resultado da operação
     */
    SkillOperationResult executeSkillByID(uint32_t characterID, uint16_t skillID, uint32_t targetID = 0, uint16_t posX = 0, uint16_t posY = 0, SkillExecutionInfo& info = *(SkillExecutionInfo*)0);
    
    /**
     * @brief Executa uma habilidade por nome
     * @param characterID ID do personagem
     * @param skillName Nome da habilidade
     * @param targetID ID do alvo
     * @param posX Posição X
     * @param posY Posição Y
     * @param info Informações de execução (saída)
     * @return Resultado da operação
     */
    SkillOperationResult executeSkillByName(uint32_t characterID, const std::string& skillName, uint32_t targetID = 0, uint16_t posX = 0, uint16_t posY = 0, SkillExecutionInfo& info = *(SkillExecutionInfo*)0);
    
    /**
     * @brief Cancela uma habilidade em execução
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return Resultado da operação
     */
    SkillOperationResult cancelSkill(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Aprende uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @param level Nível
     * @return Resultado da operação
     */
    SkillOperationResult learnSkill(uint32_t characterID, uint16_t skillID, uint8_t level = 1);
    
    /**
     * @brief Esquece uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return Resultado da operação
     */
    SkillOperationResult forgetSkill(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Melhora uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return Resultado da operação
     */
    SkillOperationResult upgradeSkill(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Verifica se uma habilidade está em cooldown
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @param remainingTime Tempo restante (saída)
     * @return true se estiver em cooldown
     */
    bool isSkillOnCooldown(uint32_t characterID, uint16_t skillID, uint32_t& remainingTime);
    
    /**
     * @brief Redefine o cooldown de uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return Resultado da operação
     */
    SkillOperationResult resetCooldown(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Redefine todos os cooldowns de um personagem
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    SkillOperationResult resetAllCooldowns(uint32_t characterID);
    
    /**
     * @brief Adiciona um efeito de habilidade
     * @param effect Efeito
     * @return ID do efeito ou 0 em caso de erro
     */
    uint32_t addSkillEffect(const SkillEffect& effect);
    
    /**
     * @brief Remove um efeito de habilidade
     * @param effectID ID do efeito
     * @return Resultado da operação
     */
    SkillOperationResult removeSkillEffect(uint32_t effectID);
    
    /**
     * @brief Remove efeitos de habilidade por tipo
     * @param characterID ID do personagem
     * @param type Tipo
     * @return Número de efeitos removidos
     */
    uint32_t removeSkillEffectsByType(uint32_t characterID, SkillEffectType type);
    
    /**
     * @brief Remove efeitos de habilidade por ID de habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return Número de efeitos removidos
     */
    uint32_t removeSkillEffectsBySkillID(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Remove efeitos de habilidade por lançador
     * @param characterID ID do personagem
     * @param casterID ID do lançador
     * @return Número de efeitos removidos
     */
    uint32_t removeSkillEffectsByCaster(uint32_t characterID, uint32_t casterID);
    
    /**
     * @brief Remove todos os efeitos de habilidade de um personagem
     * @param characterID ID do personagem
     * @return Número de efeitos removidos
     */
    uint32_t removeAllSkillEffects(uint32_t characterID);
    
    /**
     * @brief Obtém os efeitos de habilidade de um personagem
     * @param characterID ID do personagem
     * @return Lista de efeitos
     */
    std::vector<SkillEffect> getCharacterEffects(uint32_t characterID);
    
    /**
     * @brief Obtém os efeitos de habilidade de um personagem por tipo
     * @param characterID ID do personagem
     * @param type Tipo
     * @return Lista de efeitos
     */
    std::vector<SkillEffect> getCharacterEffectsByType(uint32_t characterID, SkillEffectType type);
    
    /**
     * @brief Obtém os efeitos de habilidade de um personagem por ID de habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return Lista de efeitos
     */
    std::vector<SkillEffect> getCharacterEffectsBySkillID(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Obtém um efeito de habilidade pelo ID
     * @param effectID ID do efeito
     * @return Efeito
     */
    SkillEffect getSkillEffect(uint32_t effectID);
    
    /**
     * @brief Atualiza um efeito de habilidade
     * @param effect Efeito
     * @return Resultado da operação
     */
    SkillOperationResult updateSkillEffect(const SkillEffect& effect);
    
    /**
     * @brief Verifica se uma habilidade está disponível
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return true se estiver disponível
     */
    bool isSkillAvailable(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Verifica se um personagem conhece uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return true se conhecer
     */
    bool hasSkill(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Obtém o nível de uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return Nível ou 0 se não conhecer
     */
    uint8_t getSkillLevel(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Verifica se um personagem tem recursos para usar uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return true se tiver recursos
     */
    bool hasResourcesForSkill(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Consome recursos para uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return Resultado da operação
     */
    SkillOperationResult consumeResourcesForSkill(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Verifica se uma habilidade atende aos requerimentos
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return true se atender
     */
    bool meetsSkillRequirements(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Calcula o dano de uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @param targetID ID do alvo
     * @param isCritical É crítico
     * @return Dano
     */
    uint32_t calculateSkillDamage(uint32_t characterID, uint16_t skillID, uint32_t targetID, bool& isCritical);
    
    /**
     * @brief Calcula a cura de uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @param targetID ID do alvo
     * @param isCritical É crítico
     * @return Cura
     */
    uint32_t calculateSkillHealing(uint32_t characterID, uint16_t skillID, uint32_t targetID, bool& isCritical);
    
    /**
     * @brief Obtém a informação de uma habilidade
     * @param skillID ID da habilidade
     * @return Informação ou nullptr se não encontrada
     */
    const SkillInfo* getSkillInfo(uint16_t skillID) const;
    
    /**
     * @brief Obtém a informação de uma habilidade por nome
     * @param skillName Nome da habilidade
     * @return Informação ou nullptr se não encontrada
     */
    const SkillInfo* getSkillInfoByName(const std::string& skillName) const;
    
    /**
     * @brief Obtém as habilidades de um personagem
     * @param characterID ID do personagem
     * @return Lista de habilidades
     */
    std::vector<STRUCT_SKILL> getCharacterSkills(uint32_t characterID);
    
    /**
     * @brief Obtém o nome de uma habilidade
     * @param skillID ID da habilidade
     * @return Nome
     */
    std::string getSkillName(uint16_t skillID) const;
    
    /**
     * @brief Obtém a descrição de uma habilidade
     * @param skillID ID da habilidade
     * @param level Nível
     * @return Descrição
     */
    std::string getSkillDescription(uint16_t skillID, uint8_t level = 1) const;
    
    /**
     * @brief Obtém as estatísticas de uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return Estatísticas
     */
    SkillStats getSkillStats(uint32_t characterID, uint16_t skillID);
    
    /**
     * @brief Atualiza as estatísticas de uma habilidade
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @param stats Estatísticas
     */
    void updateSkillStats(uint32_t characterID, uint16_t skillID, const SkillStats& stats);
    
    /**
     * @brief Registra um callback de habilidade
     * @param skillID ID da habilidade
     * @param callback Função de callback
     * @return true se registrado com sucesso
     */
    bool registerSkillCallback(uint16_t skillID, SkillCallback callback);
    
    /**
     * @brief Remove um callback de habilidade
     * @param skillID ID da habilidade
     * @return true se removido com sucesso
     */
    bool unregisterSkillCallback(uint16_t skillID);
    
    /**
     * @brief Registra um callback de efeito de habilidade
     * @param type Tipo de efeito
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerEffectCallback(SkillEffectType type, SkillEffectCallback callback);
    
    /**
     * @brief Remove um callback de efeito de habilidade
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEffectCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de evento de habilidade
     * @param type Tipo de evento
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerEventCallback(SkillEventType type, SkillEventCallback callback);
    
    /**
     * @brief Remove um callback de evento de habilidade
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEventCallback(uint32_t callbackId);
    
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
     * @brief Define o fator de multiplicação de dano
     * @param factor Fator
     */
    void setDamageMultiplier(float factor);
    
    /**
     * @brief Obtém o fator de multiplicação de dano
     * @return Fator
     */
    float getDamageMultiplier() const;
    
    /**
     * @brief Define o fator de multiplicação de cura
     * @param factor Fator
     */
    void setHealingMultiplier(float factor);
    
    /**
     * @brief Obtém o fator de multiplicação de cura
     * @return Fator
     */
    float getHealingMultiplier() const;
    
    /**
     * @brief Define o fator de multiplicação de duração
     * @param factor Fator
     */
    void setDurationMultiplier(float factor);
    
    /**
     * @brief Obtém o fator de multiplicação de duração
     * @return Fator
     */
    float getDurationMultiplier() const;
    
    /**
     * @brief Define o fator de multiplicação de cooldown
     * @param factor Fator
     */
    void setCooldownMultiplier(float factor);
    
    /**
     * @brief Obtém o fator de multiplicação de cooldown
     * @return Fator
     */
    float getCooldownMultiplier() const;
    
    /**
     * @brief Define o fator de multiplicação de custo
     * @param factor Fator
     */
    void setCostMultiplier(float factor);
    
    /**
     * @brief Obtém o fator de multiplicação de custo
     * @return Fator
     */
    float getCostMultiplier() const;
    
private:
    /**
     * @brief Construtor privado
     */
    SkillManager();
    
    /**
     * @brief Destrutor privado
     */
    ~SkillManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    SkillManager(const SkillManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    SkillManager& operator=(const SkillManager&) = delete;
    
    // Habilidades
    std::unordered_map<uint16_t, SkillInfo> _skillInfoMap;
    std::unordered_map<std::string, uint16_t> _skillNameToID;
    
    // Habilidades de personagens
    std::unordered_map<uint32_t, std::vector<STRUCT_SKILL>> _characterSkills;
    std::unordered_map<uint32_t, std::unordered_map<uint16_t, SkillStats>> _characterSkillStats;
    
    // Execuções em andamento
    std::unordered_map<uint32_t, SkillExecutionInfo> _activeExecutions;
    std::unordered_map<uint32_t, std::unordered_map<uint16_t, uint32_t>> _skillCooldowns;
    std::atomic<uint32_t> _nextExecutionID;
    
    // Efeitos
    std::unordered_map<uint32_t, SkillEffect> _activeEffects;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _characterEffects;
    std::atomic<uint32_t> _nextEffectID;
    
    // Callbacks
    std::unordered_map<uint16_t, SkillCallback> _skillCallbacks;
    std::unordered_map<uint32_t, SkillEffectCallback> _effectCallbacks;
    std::unordered_map<SkillEffectType, std::vector<uint32_t>> _effectCallbacksByType;
    std::unordered_map<uint32_t, SkillEventCallback> _eventCallbacks;
    std::unordered_map<SkillEventType, std::vector<uint32_t>> _eventCallbacksByType;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Fatores de multiplicação
    float _damageMultiplier;
    float _healingMultiplier;
    float _durationMultiplier;
    float _cooldownMultiplier;
    float _costMultiplier;
    
    // Geradores de números aleatórios
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _distribution;
    
    // Thread safety
    mutable std::mutex _skillMutex;
    mutable std::mutex _characterSkillMutex;
    mutable std::mutex _executionMutex;
    mutable std::mutex _effectMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    // Timer
    uint32_t _effectUpdateTimer;
    
    /**
     * @brief Atualiza os efeitos de habilidade
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void updateEffects(uint32_t deltaTime);
    
    /**
     * @brief Verifica e processa os efeitos de tick
     * @param currentTime Tempo atual
     */
    void processEffectTicks(uint32_t currentTime);
    
    /**
     * @brief Executa o callback de habilidade
     * @param skillID ID da habilidade
     * @param ctx Contexto de execução
     * @param info Informações de execução
     * @return Resultado da operação
     */
    SkillOperationResult executeSkillCallback(uint16_t skillID, const SkillExecutionContext& ctx, SkillExecutionInfo& info);
    
    /**
     * @brief Executa os callbacks de efeito de habilidade
     * @param effect Efeito
     * @param tickTime Tempo de tick
     */
    void executeEffectCallbacks(const SkillEffect& effect, uint32_t tickTime);
    
    /**
     * @brief Executa os callbacks de evento de habilidade
     * @param event Evento
     */
    void executeEventCallbacks(const SkillEvent& event);
    
    /**
     * @brief Loga um evento de habilidade
     * @param event Evento
     */
    void logSkillEvent(const SkillEvent& event);
    
    /**
     * @brief Calcula o próximo tempo de tick de um efeito
     * @param effect Efeito
     * @param currentTime Tempo atual
     * @return Próximo tempo de tick
     */
    uint32_t calculateNextTickTime(const SkillEffect& effect, uint32_t currentTime);
    
    /**
     * @brief Verifica se uma habilidade precisa ser atualizada
     * @param characterID ID do personagem
     * @param skillID ID da habilidade
     * @return true se precisar
     */
    bool needsSkillUpdate(uint32_t characterID, uint16_t skillID);
    
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
     * @brief Carrega dados de habilidades do banco de dados
     * @return true se carregado com sucesso
     */
    bool loadSkillsFromDatabase();
    
    /**
     * @brief Salva dados de habilidades no banco de dados
     * @return true se salvo com sucesso
     */
    bool saveSkillsToDatabase();
};

} // namespace skill
} // namespace wydbr

#endif // _SKILL_MANAGER_H_