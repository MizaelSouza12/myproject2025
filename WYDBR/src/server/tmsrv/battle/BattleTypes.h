/**
 * BattleTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/battle/BattleTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef BATTLETYPES_H
#define BATTLETYPES_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstring>
#include <bitset>

#include "../../../core/WYDTypes.h"
#include "../item/ItemTypes.h"

namespace wydbr {


/**
 * @file BattleTypes.h
 * @brief Definição dos tipos de dados do Sistema de Combate
 * 
 * Este arquivo contém as definições das estruturas de dados usadas pelo Sistema de Combate,
 * seguindo exatamente a estrutura do código original do WYD, mas com melhorias
 * de design, comentários e correções de bugs conhecidos.
 */

namespace wyd {
namespace server {

// Definições globais do sistema de combate
// Estas constantes eram #defines no código original do WYD
#define MAX_AFFECT 32           // Máximo de efeitos em um personagem
#define MAX_SKILLBELT 20        // Máximo de skills no skillbelt
#define MAX_LEARN_SKILL 24      // Máximo de skills aprendidas
#define MAX_TARGET 13           // Máximo de alvos em area skills
#define MAX_DAMAGE_COUNT 15     // Máximo de histórico de dano
#define MAX_DAMAGE_DELAY 3000   // Atraso máximo para registro de dano

// Constantes de delay específicas do combate
#define BATTLE_DELAY_NORMAL    1600 // Delay normal de ataque (ms)
#define BATTLE_DELAY_SLOW      2000 // Delay lento (ms)
#define BATTLE_DELAY_FAST      1200 // Delay rápido (ms)
#define BATTLE_DELAY_INSTANT   100  // Delay instantâneo para skills (ms)

// Limites de atributos e stats
#define MAX_CRITICAL_CHANCE    50   // Chance máxima de crítico (%)
#define MAX_ABSORB             50   // Absorção máxima de dano (%)
#define MAX_MAGIC_RESIST       80   // Resistência mágica máxima (%)
#define MAX_MOVE_SPEED         500  // Velocidade máxima de movimento
#define MAX_ATTACK_SPEED       200  // Velocidade máxima de ataque

// Tipos de alvo para skills e ataques
enum class TargetType : BYTE {
    NONE = 0,              // Nenhum alvo (auto)
    SELF = 1,              // Auto-targetting
    ONE_ENEMY = 2,         // Um inimigo
    AREA_ENEMY = 3,        // Área de inimigos
    ONE_ALLY = 4,          // Um aliado
    AREA_ALLY = 5,         // Área de aliados
    ALL_ENEMIES = 6,       // Todos os inimigos
    ALL_ALLIES = 7,        // Todos os aliados
    CORPSE = 8,            // Cadáver
    GROUND = 9,            // No chão
};

// Tipos de efeitos visuais de skill
enum class SkillVFX : BYTE {
    NONE = 0,              // Nenhum efeito
    FIRE = 1,              // Fogo
    ICE = 2,               // Gelo
    LIGHTNING = 3,         // Raio
    EARTH = 4,             // Terra
    WIND = 5,              // Vento
    HOLY = 6,              // Sagrado
    DARK = 7,              // Sombrio
    POISON = 8,            // Veneno
    SOUL = 9,              // Alma
    HEAL = 10,             // Cura
    BUFF = 11,             // Buff positivo
    DEBUFF = 12,           // Debuff negativo
    SPECIAL = 13,          // Efeito especial
};

// Estados de animação
enum class AnimState : BYTE {
    NONE = 0,              // Sem animação
    IDLE = 1,              // Parado
    WALK = 2,              // Andando
    RUN = 3,               // Correndo
    ATTACK = 4,            // Atacando
    ATTACKED = 5,          // Sendo atacado
    DEATH = 6,             // Morrendo
    CAST = 7,              // Conjurando
    SKILL = 8,             // Usando skill
    SIT = 9,               // Sentado
    STAND = 10,            // Levantando
    COLLECT = 11,          // Coletando
    HORSE_IDLE = 12,       // Parado (montaria)
    HORSE_WALK = 13,       // Andando (montaria)
    HORSE_RUN = 14,        // Correndo (montaria)
    SPECIAL1 = 15,         // Especial 1
    SPECIAL2 = 16,         // Especial 2
    SPECIAL3 = 17,         // Especial 3
};

// Tipos de afetadores (buffs/debuffs)
enum class AffectType : BYTE {
    NONE = 0,
    // Buffs
    HP_UP = 1,             // Aumento de HP
    MP_UP = 2,             // Aumento de MP
    AC_UP = 3,             // Aumento de defesa
    DAMAGE_UP = 4,         // Aumento de dano
    CRITICAL_UP = 5,       // Aumento de crítico
    SPEED_UP = 6,          // Aumento de velocidade
    ATTACK_SPEED_UP = 7,   // Aumento de velocidade de ataque
    RESIST_ALL_UP = 8,     // Aumento de resistência a todos elementos
    MAGIC_SHIELD = 9,      // Escudo mágico
    STEALTH = 10,          // Furtividade
    BEAST_TRANSFORMATION = 11, // Transformação em besta
    DRAGON_TRANSFORMATION = 12, // Transformação em dragão
    // Debuffs
    POISON = 21,           // Veneno
    PARALYZE = 22,         // Paralisia
    SLOW = 23,             // Lentidão
    STUN = 24,             // Atordoamento
    FEAR = 25,             // Medo
    FREEZE = 26,           // Congelamento
    BLEED = 27,            // Sangramento
    CONFUSION = 28,        // Confusão
    BLIND = 29,            // Cegueira
    CURSE = 30,            // Maldição
    SLEEP = 31,            // Sono
    DISPEL_MAGIC = 32,     // Dissipar magia
    SILENCE = 33,          // Silêncio
    // Estados especiais
    PVP_PROTECTION = 40,   // Proteção PVP
    RESURRECTION_WAIT = 41, // Esperando ressurreição
    BONUS_EXP = 42,        // Bônus de experiência
    DIVINE_PROTECTION = 43, // Proteção divina
    BONUS_DROP = 44,       // Bônus de drop
    TOWN_TELEPORT = 45,    // Teleporte para cidade
    GUILD_SKILL = 46,      // Skill de guilda
    MOUNT = 47,            // Montaria
    INVISIBLE = 48,        // Invisibilidade
    SOUL_DRAIN = 49,       // Drenagem de alma
    MAX_AFFECT = 255       // Máximo
};

// Tipos de dano
enum class DamageType : BYTE {
    PHYSICAL = 0,          // Dano físico
    MAGIC = 1,             // Dano mágico
    RANGED = 2,            // Dano à distância
    FIRE = 3,              // Dano de fogo
    ICE = 4,               // Dano de gelo
    LIGHTNING = 5,         // Dano de raio
    EARTH = 6,             // Dano de terra
    WIND = 7,              // Dano de vento
    HOLY = 8,              // Dano sagrado
    DARK = 9,              // Dano sombrio
    POISON = 10,           // Dano de veneno
    TRUE = 11,             // Dano verdadeiro (ignora defesa)
    PERCENTAGE = 12,       // Dano percentual
    HEAL = 13,             // Cura
    DRAIN = 14,            // Drenagem de vida
    REFLECT = 15,          // Dano refletido
};

// Flags de combate (bitset)
enum class BattleFlag : BYTE {
    NONE = 0,
    CRITICAL = 1,          // Acerto crítico
    CRUSHING = 2,          // Golpe esmagador
    MISSED = 3,            // Errou o golpe
    BLOCKED = 4,           // Bloqueado
    PERFECT_DODGE = 5,     // Esquiva perfeita
    IMMUNE = 6,            // Imune
    REFLECT = 7,           // Refletido
    ABSORB = 8,            // Absorvido
    RESISTED = 9,          // Resistido
    DOUBLE_DAMAGE = 10,    // Dano dobrado
    BACKSTAB = 11,         // Ataque pelas costas
    NO_EXPERIENCE = 12,    // Sem experiência
    PIERCING = 13,         // Perfurante
    NO_DROP = 14,          // Sem drop
    MAX_FLAG = 16          // Máximo
};

// Tipos de controle para skills e ataques
enum class ControlType : BYTE {
    NONE = 0,              // Nenhum controle
    STUN = 1,              // Atordoamento
    KNOCKBACK = 2,         // Empurrar para trás
    PULL = 3,              // Puxar
    FEAR = 4,              // Medo
    ROOT = 5,              // Enraizar
    JUMP = 6,              // Pular
    TELEPORT = 7,          // Teleporte
    CHARM = 8,             // Encantamento
    CONFUSE = 9,           // Confusão
    TAUNT = 10,            // Provocar
    BANISH = 11,           // Banir
    INVULNERABLE = 12,     // Invulnerável
    SILENCE = 13,          // Silêncio
    PACIFY = 14,           // Pacificar
    SLOW = 15,             // Lentidão
    GROUND = 16,           // No chão
};

// Tipos de movimento
enum class MoveType : BYTE {
    NONE = 0,              // Sem movimento
    WALK = 1,              // Andando
    RUN = 2,               // Correndo
    JUMP = 3,              // Pulando
    DASH = 4,              // Corrida rápida
    TELEPORT = 5,          // Teleporte
    KNOCKBACK = 6,         // Empurrado para trás
    PULLED = 7,            // Puxado
    FALLING = 8,           // Caindo
    MOUNT = 9,             // Montaria
    FLY = 10,              // Voando
    SWIM = 11,             // Nadando
    BLINK = 12,            // Piscar (teleporte curto)
    GLIDE = 13,            // Planar
};

// Tipos de skills
enum class SkillType : BYTE {
    NONE = 0,              // Nenhum tipo
    ATTACK = 1,            // Ataque
    HEAL = 2,              // Cura
    BUFF = 3,              // Buff
    DEBUFF = 4,            // Debuff
    SUMMON = 5,            // Invocação
    TRANSFORM = 6,         // Transformação
    TELEPORT = 7,          // Teleporte
    RESURRECT = 8,         // Ressurreição
    CRAFT = 9,             // Craft
    SPECIAL = 10,          // Especial
    TOGGLE = 11,           // Toggle (liga/desliga)
    PASSIVE = 12,          // Passiva
    MERCHANT = 13,         // Mercador
    GUILD = 14,            // Guilda
    MOUNT = 15,            // Montaria
};

// Classes que podem usar a skill
enum class SkillClass : BYTE {
    ALL = 0,               // Todas as classes
    TK = 1,                // Transknight
    FM = 2,                // Foema
    BM = 3,                // Barbarian
    HT = 4,                // Huntress
    TK_FM = 5,             // TK ou FM
    TK_BM = 6,             // TK ou BM
    TK_HT = 7,             // TK ou HT
    FM_BM = 8,             // FM ou BM
    FM_HT = 9,             // FM ou HT
    BM_HT = 10,            // BM ou HT
    NOT_TK = 11,           // Não TK
    NOT_FM = 12,           // Não FM
    NOT_BM = 13,           // Não BM
    NOT_HT = 14,           // Não HT
};

// Estrutura para atributos de combate
struct BattleAttributes {
    // Atributos principais
    int Str;               // Força
    int Int;               // Inteligência
    int Dex;               // Destreza
    int Con;               // Constituição
    
    // Derivados de ataque
    int PhysicalDamageMin; // Dano físico mínimo
    int PhysicalDamageMax; // Dano físico máximo
    int MagicDamageMin;    // Dano mágico mínimo
    int MagicDamageMax;    // Dano mágico máximo
    int AttackRating;      // Taxa de acerto
    int AttackRange;       // Alcance de ataque
    int AttackSpeed;       // Velocidade de ataque
    
    // Derivados de defesa
    int Evasion;           // Evasão
    int PhysicalDefense;   // Defesa física
    int MagicDefense;      // Defesa mágica
    int FireResist;        // Resistência a fogo
    int IceResist;         // Resistência a gelo
    int LightningResist;   // Resistência a raio
    int HolyResist;        // Resistência a sagrado
    int DarkResist;        // Resistência a sombrio
    
    // Especiais
    int CriticalChance;    // Chance de crítico
    int CriticalDamage;    // Dano crítico (%)
    int BlockChance;       // Chance de bloqueio
    int Reflect;           // Reflexão de dano (%)
    int AbsorbHP;          // Absorção de HP (%)
    int AbsorbMP;          // Absorção de MP (%)
    int DamageReduction;   // Redução de dano (%)
    int DamageIncrease;    // Aumento de dano (%)
    
    // Mobilidade
    int MoveSpeed;         // Velocidade de movimento
    
    // Construtor padrão
    BattleAttributes()
        : Str(10), Int(10), Dex(10), Con(10),
          PhysicalDamageMin(1), PhysicalDamageMax(3),
          MagicDamageMin(1), MagicDamageMax(3),
          AttackRating(100), AttackRange(1), AttackSpeed(100),
          Evasion(0), PhysicalDefense(0), MagicDefense(0),
          FireResist(0), IceResist(0), LightningResist(0),
          HolyResist(0), DarkResist(0),
          CriticalChance(0), CriticalDamage(150),
          BlockChance(0), Reflect(0),
          AbsorbHP(0), AbsorbMP(0),
          DamageReduction(0), DamageIncrease(0),
          MoveSpeed(100)
    {
    }
};

/**
 * @brief Estrutura de efeito (buff/debuff) (original: STRUCT_AFFECT)
 * 
 * No WYD original, esta estrutura representava os efeitos temporários
 * aplicados a um personagem (buffs, debuffs, etc.).
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_AFFECT {
    BYTE byType;           // Tipo do efeito (AffectType)
    BYTE byValue;          // Valor do efeito
    DWORD dwTime;          // Tempo restante (em ms)
    
    STRUCT_AFFECT() 
        : byType(0)
        , byValue(0)
        , dwTime(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de skill (original: STRUCT_SKILL)
 * 
 * No WYD original, esta estrutura representava uma skill do personagem.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_SKILL {
    WORD wSkillIndex;      // Índice da skill
    BYTE bySkillLevel;     // Nível da skill
    BYTE byLocked;         // Se está bloqueada
    
    STRUCT_SKILL() 
        : wSkillIndex(0)
        , bySkillLevel(0)
        , byLocked(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Registro de dano (original: DAMAGE_RECORD)
 * 
 * No WYD original, esta estrutura armazenava o histórico de dano para
 * distribuição de experiência e drops.
 */
struct DamageRecord {
    DWORD EntityID;        // ID da entidade que causou o dano
    int Damage;            // Quantidade de dano causado
    DWORD Timestamp;       // Horário do dano (para expiração)
    
    DamageRecord()
        : EntityID(0)
        , Damage(0)
        , Timestamp(0)
    {
    }
    
    DamageRecord(DWORD entityID, int damage, DWORD timestamp)
        : EntityID(entityID)
        , Damage(damage)
        , Timestamp(timestamp)
    {
    }
};

/**
 * @brief Definição de skill (original: SKILL_DATA)
 * 
 * No WYD original, esta estrutura definia as características de uma skill.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_SKILL_DATA {
    WORD wSkillIndex;      // Índice da skill
    char szName[32];       // Nome da skill
    char szDescription[128]; // Descrição da skill
    
    BYTE bySkillType;      // Tipo da skill (SkillType)
    BYTE byTargetType;     // Tipo de alvo (TargetType)
    BYTE byVFX;            // Efeito visual (SkillVFX)
    BYTE byDamageType;     // Tipo de dano (DamageType)
    
    BYTE byLevel;          // Nível base
    BYTE byMaxLevel;       // Nível máximo
    BYTE byLevelInterval;  // Intervalo de nível para evolução
    BYTE byClass;          // Classe que pode usar (SkillClass)
    
    WORD wMPCost;          // Custo de MP
    WORD wHPCost;          // Custo de HP
    WORD wRange;           // Alcance
    WORD wArea;            // Área de efeito
    
    WORD wCastTime;        // Tempo de cast (ms)
    WORD wCooldown;        // Tempo de recarga (ms)
    WORD wDuration;        // Duração do efeito (ms)
    WORD wAnimationDelay;  // Delay da animação (ms)
    
    WORD wDamageBase;      // Dano base
    WORD wDamagePerLevel;  // Dano adicional por nível
    WORD wValueBase;       // Valor base
    WORD wValuePerLevel;   // Valor adicional por nível
    
    BYTE bySuccessRate;    // Taxa de sucesso base (%)
    BYTE bySuccessPerLevel; // Taxa adicional por nível (%)
    BYTE byControlType;    // Tipo de controle (ControlType)
    BYTE byControlValue;   // Valor do /**
 * STRUCT_SKILL_DATA
 * 
 * Implementa a funcionalidade STRUCT_SKILL_DATA conforme especificação original.
 * @return Retorna controle
 */
 controle
    
    STRUCT_SKILL_DATA(){
        memset(this, 0, sizeof(STRUCT_SKILL_DATA));
    }
};
#pragma pack(pop)

/**
 * @brief Resultado de um ataque ou skill
 * 
 * Esta estrutura armazena todos os dados resultantes de um ataque ou skill,
 * incluindo dano, efeitos, flags, etc.
 */
struct BattleResult {
    int Damage;                        // Dano total causado
    int AbsorbedHP;                    // HP absorvido
    int AbsorbedMP;                    // MP absorvido
    DamageType Type;                   // Tipo de dano
    std::bitset<16> Flags;             // Flags (BattleFlag)
    std::vector<std::pair<AffectType, BYTE>> AddedEffects; // Efeitos adicionados
    std::vector<AffectType> RemovedEffects; // Efeitos removidos
    ControlType ControlEffect;         // Efeito de controle
    int ControlValue;                  // Valor do controle
    WORD SkillID;                      // ID da skill usada
    bool IsSkill;                      // Se foi causado por skill
    DWORD AttackerID;                  // ID do atacante
    DWORD TargetID;                    // ID do alvo
    DWORD Timestamp;                   // Horário do ataque
    
    BattleResult()
        : Damage(0)
        , AbsorbedHP(0)
        , AbsorbedMP(0)
        , Type(DamageType::PHYSICAL)
        , ControlEffect(ControlType::NONE)
        , ControlValue(0)
        , SkillID(0)
        , IsSkill(false)
        , AttackerID(0)
        , TargetID(0)
        , Timestamp(0)
    {
    }
    
    // Métodos para definir flags
    /**
 * SetFlag
 * 
 * Implementa a funcionalidade SetFlag conforme especificação original.
 * @param flag Parâmetro flag
 * @param true Parâmetro true
 */

    void SetFlag(BattleFlag flag, bool value = true){
        Flags.set(static_cast<size_t>(flag), value);
    }
    
    // Verifica se uma flag está definida
    /**
 * HasFlag
 * 
 * Implementa a funcionalidade HasFlag conforme especificação original.
 * @param flag Parâmetro flag
 * @return Retorna bool
 */

    bool HasFlag(BattleFlag flag) const{
        return Flags.test(static_cast<size_t>(flag));
    }
    
    // Adiciona um efeito
    /**
 * AddEffect
 * 
 * Implementa a funcionalidade AddEffect conforme especificação original.
 * @param type Parâmetro type
 * @param value Parâmetro value
 */

    void AddEffect(AffectType type, BYTE value){
        AddedEffects.emplace_back(type, value);
    }
    
    // Remove um efeito
    /**
 * RemoveEffect
 * 
 * Implementa a funcionalidade RemoveEffect conforme especificação original.
 * @param type Parâmetro type
 */

    void RemoveEffect(AffectType type){
        RemovedEffects.push_back(type);
    }
    
    // Define o efeito de controle
    /**
 * SetControlEffect
 * 
 * Implementa a funcionalidade SetControlEffect conforme especificação original.
 * @param type Parâmetro type
 * @param value Parâmetro value
 */

    void SetControlEffect(ControlType type, int value){
        ControlEffect = type;
        ControlValue = value;
    }
    
    // Retorna se o ataque foi bem-sucedido
    /**
 * IsSuccessful
 * 
 * Implementa a funcionalidade IsSuccessful conforme especificação original.
 * @return Retorna bool
 */

    bool IsSuccessful() const{
        return !HasFlag(BattleFlag::MISSED) && 
               !HasFlag(BattleFlag::PERFECT_DODGE) && 
               !HasFlag(BattleFlag::IMMUNE);
    }
};

} // namespace server
} // namespace wyd

#endif // BATTLETYPES_H

} // namespace wydbr
