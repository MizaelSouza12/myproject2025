/**
 * SkillConstants.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/skill/SkillConstants.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _SKILL_CONSTANTS_H_
#define _SKILL_CONSTANTS_H_

/**
 * @file SkillConstants.h
 * @brief Constantes e tipos enumerados para o sistema de habilidades
 * 
 * Este arquivo contém todas as constantes e tipos enumerados relacionados
 * ao sistema de habilidades do WYD, incluindo identificadores de tipos de
 * alvo, tipos de instância, tipos de efeito, etc.
 */

#include <stdint.h>

namespace wydbr {
namespace skill {

/**
 * Limites do sistema de habilidades
 */
enum SkillLimits {
    MAX_SKILL_ID = 255,               // Máximo ID de habilidade
    MAX_SKILL_LEVEL = 20,             // Máximo nível de habilidade
    MAX_SKILLS_PER_CHAR = 32,         // Máximo de habilidades por personagem
    MAX_SKILL_TARGETS = 16,           // Máximo de alvos por habilidade
    MAX_SKILL_COOLDOWN = 3600000,     // Máximo cooldown em milissegundos (1 hora)
    MAX_SKILL_DURATION = 3600000,     // Máxima duração em milissegundos (1 hora)
    MAX_SKILL_CASTING_TIME = 60000,   // Máximo tempo de casting em milissegundos (1 minuto)
    MAX_SKILL_RANGE = 30,             // Máximo alcance em células
    MAX_SKILL_AOE = 10,               // Máximo tamanho de área de efeito em células
    MAX_SKILL_CHAIN = 5,              // Máximo de saltos em habilidades de cadeia
    MAX_SKILL_COMBO = 8,              // Máximo de habilidades em um combo
    MAX_SKILL_CHARGE_STACKS = 5       // Máximo de cargas acumuláveis
};

/**
 * Tipos de alvo de habilidade
 */
enum SkillTargetType {
    SKILL_TARGET_NONE = 0,            // Nenhum alvo (erro)
    SKILL_TARGET_SELF = 1,            // Próprio lançador
    SKILL_TARGET_ALLY = 2,            // Aliado (incluindo self)
    SKILL_TARGET_ENEMY = 3,           // Inimigo
    SKILL_TARGET_GROUND = 4,          // Área no chão
    SKILL_TARGET_CORPSE = 5,          // Cadáver
    SKILL_TARGET_ITEM = 6,            // Item
    SKILL_TARGET_PET = 7,             // Mascote
    SKILL_TARGET_GROUP = 8,           // Grupo inteiro
    SKILL_TARGET_GUILD = 9,           // Guilda inteira
    SKILL_TARGET_NPC = 10,            // NPC específico
    SKILL_TARGET_PARTY_MEMBER = 11,   // Membro do grupo (excluindo self)
    SKILL_TARGET_ALL = 12,            // Todos (amigos e inimigos)
    SKILL_TARGET_MERCHANT = 13,       // Mercadores
    SKILL_TARGET_NEUTRAL = 14,        // Personagens neutros
    SKILL_TARGET_FRIENDLY_AREA = 15,  // Área afetando apenas aliados
    SKILL_TARGET_ENEMY_AREA = 16,     // Área afetando apenas inimigos
    SKILL_TARGET_ALL_AREA = 17,       // Área afetando todos
    SKILL_TARGET_PET_AREA = 18,       // Área afetando apenas mascotes
    SKILL_TARGET_SPECIAL = 19,        // Alvo especial (implementação customizada)
    SKILL_TARGET_UNKNOWN = 20         // Desconhecido
};

/**
 * Tipos de instância de habilidade (efeito imediato)
 */
enum SkillInstanceType {
    SKILL_INSTANCE_NONE = 0,          // Nenhum efeito imediato
    SKILL_INSTANCE_DAMAGE = 1,        // Dano físico
    SKILL_INSTANCE_MAGIC_DAMAGE = 2,  // Dano mágico
    SKILL_INSTANCE_HEAL = 3,          // Cura
    SKILL_INSTANCE_RESURRECT = 4,     // Ressurreição
    SKILL_INSTANCE_TELEPORT = 5,      // Teleporte
    SKILL_INSTANCE_DISPEL = 6,        // Dissipar efeitos
    SKILL_INSTANCE_SUMMON = 7,        // Invocar criatura
    SKILL_INSTANCE_TRANSFORM = 8,     // Transformação
    SKILL_INSTANCE_MP_DAMAGE = 9,     // Dano de MP
    SKILL_INSTANCE_MP_HEAL = 10,      // Cura de MP
    SKILL_INSTANCE_STEAL_HP = 11,     // Roubo de HP
    SKILL_INSTANCE_STEAL_MP = 12,     // Roubo de MP
    SKILL_INSTANCE_KNOCKBACK = 13,    // Empurrão
    SKILL_INSTANCE_PULL = 14,         // Puxão
    SKILL_INSTANCE_DASH = 15,         // Avanço rápido
    SKILL_INSTANCE_IMMUNITY = 16,     // Imunidade temporária
    SKILL_INSTANCE_SPECIAL = 17,      // Efeito especial (implementação customizada)
    SKILL_INSTANCE_UNKNOWN = 18       // Desconhecido
};

/**
 * Tipos de efeito periódico (tick)
 */
enum SkillTickType {
    SKILL_TICK_NONE = 0,              // Nenhum efeito periódico
    SKILL_TICK_DAMAGE = 1,            // Dano ao longo do tempo
    SKILL_TICK_HEAL = 2,              // Cura ao longo do tempo
    SKILL_TICK_MP_DAMAGE = 3,         // Dano de MP ao longo do tempo
    SKILL_TICK_MP_HEAL = 4,           // Cura de MP ao longo do tempo
    SKILL_TICK_STEAL_HP = 5,          // Roubo de HP ao longo do tempo
    SKILL_TICK_STEAL_MP = 6,          // Roubo de MP ao longo do tempo
    SKILL_TICK_SPECIAL = 7,           // Efeito especial (implementação customizada)
    SKILL_TICK_UNKNOWN = 8            // Desconhecido
};

/**
 * Tipos de efeito de status (affect)
 */
enum SkillAffectType {
    SKILL_AFFECT_NONE = 0,            // Nenhum efeito de status
    SKILL_AFFECT_STUN = 1,            // Atordoamento
    SKILL_AFFECT_SILENCE = 2,         // Silenciamento
    SKILL_AFFECT_ROOT = 3,            // Enraizamento
    SKILL_AFFECT_BLIND = 4,           // Cegueira
    SKILL_AFFECT_POISON = 5,          // Envenenamento
    SKILL_AFFECT_BURN = 6,            // Queimadura
    SKILL_AFFECT_FREEZE = 7,          // Congelamento
    SKILL_AFFECT_PARALYZE = 8,        // Paralisia
    SKILL_AFFECT_SLEEP = 9,           // Sono
    SKILL_AFFECT_FEAR = 10,           // Medo
    SKILL_AFFECT_CHARM = 11,          // Encantamento
    SKILL_AFFECT_SLOW = 12,           // Redução de velocidade
    SKILL_AFFECT_HASTE = 13,          // Aumento de velocidade
    SKILL_AFFECT_STR_BUFF = 14,       // Bônus de Força
    SKILL_AFFECT_DEX_BUFF = 15,       // Bônus de Destreza
    SKILL_AFFECT_INT_BUFF = 16,       // Bônus de Inteligência
    SKILL_AFFECT_CON_BUFF = 17,       // Bônus de Constituição
    SKILL_AFFECT_ATK_BUFF = 18,       // Bônus de Ataque
    SKILL_AFFECT_DEF_BUFF = 19,       // Bônus de Defesa
    SKILL_AFFECT_MATK_BUFF = 20,      // Bônus de Ataque Mágico
    SKILL_AFFECT_MDEF_BUFF = 21,      // Bônus de Defesa Mágica
    SKILL_AFFECT_ACC_BUFF = 22,       // Bônus de Precisão
    SKILL_AFFECT_EVA_BUFF = 23,       // Bônus de Evasão
    SKILL_AFFECT_CRIT_BUFF = 24,      // Bônus de Taxa Crítica
    SKILL_AFFECT_STR_DEBUFF = 25,     // Redução de Força
    SKILL_AFFECT_DEX_DEBUFF = 26,     // Redução de Destreza
    SKILL_AFFECT_INT_DEBUFF = 27,     // Redução de Inteligência
    SKILL_AFFECT_CON_DEBUFF = 28,     // Redução de Constituição
    SKILL_AFFECT_ATK_DEBUFF = 29,     // Redução de Ataque
    SKILL_AFFECT_DEF_DEBUFF = 30,     // Redução de Defesa
    SKILL_AFFECT_MATK_DEBUFF = 31,    // Redução de Ataque Mágico
    SKILL_AFFECT_MDEF_DEBUFF = 32,    // Redução de Defesa Mágica
    SKILL_AFFECT_ACC_DEBUFF = 33,     // Redução de Precisão
    SKILL_AFFECT_EVA_DEBUFF = 34,     // Redução de Evasão
    SKILL_AFFECT_CRIT_DEBUFF = 35,    // Redução de Taxa Crítica
    SKILL_AFFECT_SHIELD = 36,         // Escudo de dano
    SKILL_AFFECT_REFLECT = 37,        // Reflexão de dano
    SKILL_AFFECT_INVISIBLE = 38,      // Invisibilidade
    SKILL_AFFECT_DETECT = 39,         // Detecção de invisíveis
    SKILL_AFFECT_INVINCIBLE = 40,     // Invulnerabilidade
    SKILL_AFFECT_SPECIAL = 41,        // Efeito especial (implementação customizada)
    SKILL_AFFECT_UNKNOWN = 42         // Desconhecido
};

/**
 * Atributos utilizados para cálculo de habilidades
 */
enum SkillAttributeType {
    SKILL_ATTRIBUTE_NONE = 0,         // Nenhum atributo
    SKILL_ATTRIBUTE_STR = 1,          // Força
    SKILL_ATTRIBUTE_DEX = 2,          // Destreza
    SKILL_ATTRIBUTE_INT = 3,          // Inteligência
    SKILL_ATTRIBUTE_CON = 4,          // Constituição
    SKILL_ATTRIBUTE_ATK = 5,          // Ataque
    SKILL_ATTRIBUTE_DEF = 6,          // Defesa
    SKILL_ATTRIBUTE_MATK = 7,         // Ataque Mágico
    SKILL_ATTRIBUTE_MDEF = 8,         // Defesa Mágica
    SKILL_ATTRIBUTE_HP = 9,           // Pontos de Vida
    SKILL_ATTRIBUTE_MP = 10,          // Pontos de Mana
    SKILL_ATTRIBUTE_LEVEL = 11,       // Nível
    SKILL_ATTRIBUTE_SPECIAL = 12,     // Atributo especial (implementação customizada)
    SKILL_ATTRIBUTE_UNKNOWN = 13      // Desconhecido
};

/**
 * Classes que podem usar habilidades
 */
enum SkillClassType {
    SKILL_CLASS_ALL = 0,              // Todas as classes
    SKILL_CLASS_TRANS = 1,            // Somente transcendental
    SKILL_CLASS_ARCH = 2,             // Somente arch
    SKILL_CLASS_MORTAL = 3,           // Somente mortal
    SKILL_CLASS_TK = 4,               // Somente Transcendental Knight
    SKILL_CLASS_FM = 5,               // Somente Foema
    SKILL_CLASS_BM = 6,               // Somente Beast Master
    SKILL_CLASS_HT = 7,               // Somente Hunter
    SKILL_CLASS_SPECIALTK = 8,        // Transcendental Knight com especialização
    SKILL_CLASS_SPECIALFM = 9,        // Foema com especialização
    SKILL_CLASS_SPECIALBM = 10,       // Beast Master com especialização
    SKILL_CLASS_SPECIALHT = 11,       // Hunter com especialização
    SKILL_CLASS_MK = 12,              // Somente Mortal Knight
    SKILL_CLASS_FM_MG = 13,           // Somente Mortal Foema
    SKILL_CLASS_BM_MG = 14,           // Somente Mortal Beast Master
    SKILL_CLASS_UNKNOWN = 15          // Desconhecido
};

/**
 * Requisitos especiais para executar habilidades
 */
enum SkillRequirementType {
    SKILL_REQ_NONE = 0,               // Nenhum requisito especial
    SKILL_REQ_WEAPON = 1,             // Requer arma específica
    SKILL_REQ_SHIELD = 2,             // Requer escudo
    SKILL_REQ_MOUNT = 3,              // Requer montaria
    SKILL_REQ_NO_MOUNT = 4,           // Não pode estar montado
    SKILL_REQ_COMBAT = 5,             // Requer estar em combate
    SKILL_REQ_NO_COMBAT = 6,          // Requer não estar em combate
    SKILL_REQ_SPECIAL = 7,            // Requisito especial (implementação customizada)
    SKILL_REQ_UNKNOWN = 8             // Desconhecido
};

/**
 * Famílias de habilidades (para categorização)
 */
enum SkillFamilyType {
    SKILL_FAMILY_NONE = 0,            // Nenhuma família
    SKILL_FAMILY_MELEE = 1,           // Habilidades corpo-a-corpo
    SKILL_FAMILY_RANGED = 2,          // Habilidades de ataque à distância
    SKILL_FAMILY_MAGIC = 3,           // Habilidades mágicas
    SKILL_FAMILY_HEALING = 4,         // Habilidades de cura
    SKILL_FAMILY_BUFF = 5,            // Habilidades de melhoria
    SKILL_FAMILY_DEBUFF = 6,          // Habilidades de enfraquecimento
    SKILL_FAMILY_TRANSPORT = 7,       // Habilidades de transporte/movimento
    SKILL_FAMILY_TRANSFORM = 8,       // Habilidades de transformação
    SKILL_FAMILY_SUMMON = 9,          // Habilidades de invocação
    SKILL_FAMILY_SPECIAL = 10,        // Habilidades especiais (implementação customizada)
    SKILL_FAMILY_UNKNOWN = 11         // Desconhecido
};

/**
 * Elementos das habilidades
 */
enum SkillElementType {
    SKILL_ELEMENT_NONE = 0,           // Nenhum elemento
    SKILL_ELEMENT_FIRE = 1,           // Fogo
    SKILL_ELEMENT_WATER = 2,          // Água
    SKILL_ELEMENT_EARTH = 3,          // Terra
    SKILL_ELEMENT_WIND = 4,           // Vento
    SKILL_ELEMENT_LIGHTNING = 5,      // Relâmpago
    SKILL_ELEMENT_ICE = 6,            // Gelo
    SKILL_ELEMENT_POISON = 7,         // Veneno
    SKILL_ELEMENT_HOLY = 8,           // Sagrado
    SKILL_ELEMENT_DARK = 9,           // Sombrio
    SKILL_ELEMENT_UNDEAD = 10,        // Morto-vivo
    SKILL_ELEMENT_SPECIAL = 11,       // Elemento especial (implementação customizada)
    SKILL_ELEMENT_UNKNOWN = 12        // Desconhecido
};

/**
 * Identificadores de eventos do sistema de habilidades
 */
enum SkillEventType {
    SKILL_EVENT_NONE = 0,             // Nenhum evento
    SKILL_EVENT_LEARN = 1,            // Habilidade aprendida
    SKILL_EVENT_FORGET = 2,           // Habilidade esquecida
    SKILL_EVENT_CAST_START = 3,       // Início de lançamento
    SKILL_EVENT_CAST_SUCCESS = 4,     // Lançamento bem-sucedido
    SKILL_EVENT_CAST_FAIL = 5,        // Lançamento falhou
    SKILL_EVENT_CAST_CANCEL = 6,      // Lançamento cancelado
    SKILL_EVENT_EFFECT_APPLY = 7,     // Efeito aplicado
    SKILL_EVENT_EFFECT_REMOVE = 8,    // Efeito removido
    SKILL_EVENT_COOLDOWN_START = 9,   // Início de cooldown
    SKILL_EVENT_COOLDOWN_END = 10,    // Fim de cooldown
    SKILL_EVENT_LEVEL_UP = 11,        // Habilidade subiu de nível
    SKILL_EVENT_COMBO_START = 12,     // Início de combo
    SKILL_EVENT_COMBO_SUCCESS = 13,   // Combo bem-sucedido
    SKILL_EVENT_COMBO_FAIL = 14,      // Combo falhou
    SKILL_EVENT_SPECIAL = 15,         // Evento especial (implementação customizada)
    SKILL_EVENT_UNKNOWN = 16          // Desconhecido
};

/**
 * Identificadores de erro do sistema de habilidades
 */
enum SkillErrorType {
    SKILL_ERROR_NONE = 0,             // Nenhum erro
    SKILL_ERROR_INVALID_SKILL = 1,    // Habilidade inválida
    SKILL_ERROR_INVALID_LEVEL = 2,    // Nível inválido
    SKILL_ERROR_INVALID_TARGET = 3,   // Alvo inválido
    SKILL_ERROR_INVALID_POSITION = 4, // Posição inválida
    SKILL_ERROR_NOT_LEARNED = 5,      // Habilidade não aprendida
    SKILL_ERROR_NOT_ENOUGH_MP = 6,    // MP insuficiente
    SKILL_ERROR_NOT_ENOUGH_HP = 7,    // HP insuficiente
    SKILL_ERROR_NOT_ENOUGH_ITEM = 8,  // Item insuficiente
    SKILL_ERROR_COOLDOWN = 9,         // Em cooldown
    SKILL_ERROR_SILENCED = 10,        // Silenciado
    SKILL_ERROR_STUNNED = 11,         // Atordoado
    SKILL_ERROR_SLEEPING = 12,        // Dormindo
    SKILL_ERROR_FROZEN = 13,          // Congelado
    SKILL_ERROR_OUT_OF_RANGE = 14,    // Fora de alcance
    SKILL_ERROR_LINE_OF_SIGHT = 15,   // Sem linha de visão
    SKILL_ERROR_LEVEL_RESTRICTION = 16, // Nível insuficiente
    SKILL_ERROR_CLASS_RESTRICTION = 17, // Classe inválida
    SKILL_ERROR_WEAPON_RESTRICTION = 18, // Arma inválida
    SKILL_ERROR_SPECIAL = 19,         // Erro especial (implementação customizada)
    SKILL_ERROR_UNKNOWN = 20          // Desconhecido
};

} // namespace skill
} // namespace wydbr

#endif // _SKILL_CONSTANTS_H_