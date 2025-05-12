/**
 * TM_Constants.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/TM_Constants.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _TM_CONSTANTS_H_
#define _TM_CONSTANTS_H_

/**
 * @file TM_Constants.h
 * @brief Constantes do sistema WYD
 * 
 * Este arquivo contém todas as constantes do sistema WYDBR,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>

namespace wydbr {
namespace constants {

// Versão do servidor
constexpr uint32_t SERVER_VERSION_MAJOR = 1;
constexpr uint32_t SERVER_VERSION_MINOR = 0;
constexpr uint32_t SERVER_VERSION_PATCH = 0;
constexpr uint32_t SERVER_VERSION = (SERVER_VERSION_MAJOR << 16) | (SERVER_VERSION_MINOR << 8) | SERVER_VERSION_PATCH;
constexpr const char* SERVER_VERSION_STRING = "1.0.0";

// Configurações do servidor
constexpr uint16_t SERVER_PORT = 8281;
constexpr uint16_t SERVER_MAX_CONNECTIONS = 1000;
constexpr uint16_t SERVER_MAX_PLAYERS = 1000;
constexpr uint16_t SERVER_TICK_RATE = 33;
constexpr uint32_t SERVER_SAVE_INTERVAL = 300000;
constexpr uint16_t SERVER_AUTO_SAVE = 1;
constexpr uint16_t SERVER_ENABLE_PVP = 1;
constexpr uint16_t SERVER_ENABLE_GUILD_WAR = 1;
constexpr uint16_t SERVER_ENABLE_EVENTS = 1;
constexpr uint16_t SERVER_ENABLE_PREMIUM = 1;

// Taxas do servidor
constexpr float SERVER_EXP_RATE = 1.0f;
constexpr float SERVER_DROP_RATE = 1.0f;
constexpr float SERVER_GOLD_RATE = 1.0f;

// Limites de personagens
constexpr uint8_t CHARACTER_MAX_LEVEL = 255;
constexpr uint8_t CHARACTER_MAX_STAT = 200;
constexpr uint8_t CHARACTER_MAX_SPECIAL = 12;
constexpr uint8_t CHARACTER_MAX_SPECIAL_POINTS = 255;
constexpr uint8_t CHARACTER_MAX_SKILL_POINTS = 255;
constexpr uint8_t CHARACTER_MAX_SKILL_LEVEL = 12;
constexpr uint32_t CHARACTER_MAX_GOLD = 2000000000;
constexpr uint32_t CHARACTER_MAX_DONATE = 2000000000;
constexpr uint8_t CHARACTER_PER_ACCOUNT = 4;

// Classes de personagens
constexpr uint8_t CLASS_TK = 0;
constexpr uint8_t CLASS_FM = 1;
constexpr uint8_t CLASS_BM = 2;
constexpr uint8_t CLASS_HT = 3;

// Tipos de item
constexpr uint8_t ITEM_TYPE_WEAPON = 0;
constexpr uint8_t ITEM_TYPE_ARMOR = 1;
constexpr uint8_t ITEM_TYPE_HELM = 2;
constexpr uint8_t ITEM_TYPE_SHIELD = 3;
constexpr uint8_t ITEM_TYPE_ACCESSORY = 4;
constexpr uint8_t ITEM_TYPE_POTION = 5;
constexpr uint8_t ITEM_TYPE_QUEST = 6;
constexpr uint8_t ITEM_TYPE_MATERIAL = 7;
constexpr uint8_t ITEM_TYPE_MOUNT = 8;
constexpr uint8_t ITEM_TYPE_SPECIAL = 9;

// Slots de item
constexpr uint8_t ITEM_SLOT_HAND = 0;
constexpr uint8_t ITEM_SLOT_ARMOR = 1;
constexpr uint8_t ITEM_SLOT_HELM = 2;
constexpr uint8_t ITEM_SLOT_SHIELD = 3;
constexpr uint8_t ITEM_SLOT_NECKLACE = 4;
constexpr uint8_t ITEM_SLOT_RING = 5;
constexpr uint8_t ITEM_SLOT_BELT = 6;
constexpr uint8_t ITEM_SLOT_BOOTS = 7;
constexpr uint8_t ITEM_SLOT_MOUNT = 8;
constexpr uint8_t ITEM_SLOT_OTHER = 9;

// Efeitos de item
constexpr uint8_t ITEM_EFFECT_NONE = 0;
constexpr uint8_t ITEM_EFFECT_HP = 1;
constexpr uint8_t ITEM_EFFECT_MP = 2;
constexpr uint8_t ITEM_EFFECT_STR = 3;
constexpr uint8_t ITEM_EFFECT_INT = 4;
constexpr uint8_t ITEM_EFFECT_DEX = 5;
constexpr uint8_t ITEM_EFFECT_CON = 6;
constexpr uint8_t ITEM_EFFECT_DAMAGE = 7;
constexpr uint8_t ITEM_EFFECT_AC = 8;
constexpr uint8_t ITEM_EFFECT_RESIST = 9;
constexpr uint8_t ITEM_EFFECT_CRITICAL = 10;
constexpr uint8_t ITEM_EFFECT_ATTACK_SPEED = 11;
constexpr uint8_t ITEM_EFFECT_MOVE_SPEED = 12;
constexpr uint8_t ITEM_EFFECT_SANC = 13;
constexpr uint8_t ITEM_EFFECT_GRID = 14;
constexpr uint8_t ITEM_EFFECT_COUNT = 15;
constexpr uint8_t ITEM_EFFECT_UNIQUE = 16;
constexpr uint8_t ITEM_EFFECT_DONATION = 17;
constexpr uint8_t ITEM_EFFECT_NOTRADE = 18;
constexpr uint8_t ITEM_EFFECT_NOSALE = 19;
constexpr uint8_t ITEM_EFFECT_NODROP = 20;
constexpr uint8_t ITEM_EFFECT_MAGIC = 21;
constexpr uint8_t ITEM_EFFECT_SOCKET = 22;
constexpr uint8_t ITEM_EFFECT_COLOR = 23;
constexpr uint8_t ITEM_EFFECT_AMOUNT = 24;
constexpr uint8_t ITEM_EFFECT_TIME = 25;
constexpr uint8_t ITEM_EFFECT_TOGGLE = 26;
constexpr uint8_t ITEM_EFFECT_SPECIAL = 27;
constexpr uint8_t ITEM_EFFECT_LUCK = 28;
constexpr uint8_t ITEM_EFFECT_PERMANENT = 29;
constexpr uint8_t ITEM_EFFECT_TEMPORARY = 30;
constexpr uint8_t ITEM_EFFECT_ENCHANT = 31;

// Tipos de habilidade
constexpr uint8_t SKILL_TYPE_PASSIVE = 0;
constexpr uint8_t SKILL_TYPE_ACTIVE = 1;
constexpr uint8_t SKILL_TYPE_TOGGLE = 2;
constexpr uint8_t SKILL_TYPE_BUFF = 3;
constexpr uint8_t SKILL_TYPE_DEBUFF = 4;
constexpr uint8_t SKILL_TYPE_SPECIAL = 5;

// Tipos de afetamento
constexpr uint8_t AFFECT_TYPE_NONE = 0;
constexpr uint8_t AFFECT_TYPE_STUN = 1;
constexpr uint8_t AFFECT_TYPE_POISON = 2;
constexpr uint8_t AFFECT_TYPE_SLOW = 3;
constexpr uint8_t AFFECT_TYPE_SILENCE = 4;
constexpr uint8_t AFFECT_TYPE_BLIND = 5;
constexpr uint8_t AFFECT_TYPE_SPEED = 6;
constexpr uint8_t AFFECT_TYPE_STRENGTH = 7;
constexpr uint8_t AFFECT_TYPE_DEXTERITY = 8;
constexpr uint8_t AFFECT_TYPE_INTELLIGENCE = 9;
constexpr uint8_t AFFECT_TYPE_CONSTITUTION = 10;
constexpr uint8_t AFFECT_TYPE_DAMAGE = 11;
constexpr uint8_t AFFECT_TYPE_DEFENSE = 12;
constexpr uint8_t AFFECT_TYPE_HP = 13;
constexpr uint8_t AFFECT_TYPE_MP = 14;
constexpr uint8_t AFFECT_TYPE_RESIST = 15;
constexpr uint8_t AFFECT_TYPE_CRITICAL = 16;
constexpr uint8_t AFFECT_TYPE_DODGE = 17;
constexpr uint8_t AFFECT_TYPE_ATTACK_SPEED = 18;
constexpr uint8_t AFFECT_TYPE_INVISIBLE = 19;
constexpr uint8_t AFFECT_TYPE_MOUNT = 20;
constexpr uint8_t AFFECT_TYPE_TRANSFORM = 21;
constexpr uint8_t AFFECT_TYPE_PVP = 22;
constexpr uint8_t AFFECT_TYPE_IMMUNITY = 23;
constexpr uint8_t AFFECT_TYPE_BLEEDING = 24;
constexpr uint8_t AFFECT_TYPE_BURNING = 25;
constexpr uint8_t AFFECT_TYPE_FREEZING = 26;
constexpr uint8_t AFFECT_TYPE_LIGHTNING = 27;
constexpr uint8_t AFFECT_TYPE_FEAR = 28;
constexpr uint8_t AFFECT_TYPE_SLEEP = 29;
constexpr uint8_t AFFECT_TYPE_PARALYZE = 30;
constexpr uint8_t AFFECT_TYPE_ROOT = 31;

// Tipos de movimento
constexpr uint8_t MOVE_TYPE_WALK = 0;
constexpr uint8_t MOVE_TYPE_RUN = 1;
constexpr uint8_t MOVE_TYPE_TELEPORT = 2;
constexpr uint8_t MOVE_TYPE_JUMP = 3;
constexpr uint8_t MOVE_TYPE_DASH = 4;
constexpr uint8_t MOVE_TYPE_STOP = 5;
constexpr uint8_t MOVE_TYPE_KNOCKBACK = 6;
constexpr uint8_t MOVE_TYPE_FALL = 7;
constexpr uint8_t MOVE_TYPE_PULL = 8;
constexpr uint8_t MOVE_TYPE_WARP = 9;

// Tipos de ataque
constexpr uint8_t ATTACK_TYPE_NORMAL = 0;
constexpr uint8_t ATTACK_TYPE_CRITICAL = 1;
constexpr uint8_t ATTACK_TYPE_SKILL = 2;
constexpr uint8_t ATTACK_TYPE_MAGIC = 3;
constexpr uint8_t ATTACK_TYPE_COUNTER = 4;
constexpr uint8_t ATTACK_TYPE_REFLECT = 5;
constexpr uint8_t ATTACK_TYPE_DOT = 6;
constexpr uint8_t ATTACK_TYPE_COMBO = 7;
constexpr uint8_t ATTACK_TYPE_SPECIAL = 8;
constexpr uint8_t ATTACK_TYPE_PIERCE = 9;
constexpr uint8_t ATTACK_TYPE_SPLASH = 10;
constexpr uint8_t ATTACK_TYPE_AOE = 11;
constexpr uint8_t ATTACK_TYPE_RANGE = 12;
constexpr uint8_t ATTACK_TYPE_EFFECT = 13;
constexpr uint8_t ATTACK_TYPE_CHAIN = 14;
constexpr uint8_t ATTACK_TYPE_TRUE = 15;

// Tipos de dano
constexpr uint8_t DAMAGE_TYPE_PHYSICAL = 0;
constexpr uint8_t DAMAGE_TYPE_MAGIC = 1;
constexpr uint8_t DAMAGE_TYPE_FIRE = 2;
constexpr uint8_t DAMAGE_TYPE_WATER = 3;
constexpr uint8_t DAMAGE_TYPE_EARTH = 4;
constexpr uint8_t DAMAGE_TYPE_WIND = 5;
constexpr uint8_t DAMAGE_TYPE_POISON = 6;
constexpr uint8_t DAMAGE_TYPE_BLEEDING = 7;
constexpr uint8_t DAMAGE_TYPE_STUN = 8;
constexpr uint8_t DAMAGE_TYPE_SLOW = 9;
constexpr uint8_t DAMAGE_TYPE_ROOT = 10;
constexpr uint8_t DAMAGE_TYPE_CRITICAL = 11;
constexpr uint8_t DAMAGE_TYPE_PIERCE = 12;
constexpr uint8_t DAMAGE_TYPE_TRUE = 13;
constexpr uint8_t DAMAGE_TYPE_HOLY = 14;
constexpr uint8_t DAMAGE_TYPE_DARK = 15;
constexpr uint8_t DAMAGE_TYPE_NATURE = 16;
constexpr uint8_t DAMAGE_TYPE_ARCANE = 17;
constexpr uint8_t DAMAGE_TYPE_LIGHTNING = 18;
constexpr uint8_t DAMAGE_TYPE_FROST = 19;
constexpr uint8_t DAMAGE_TYPE_SHADOW = 20;
constexpr uint8_t DAMAGE_TYPE_CHAOS = 21;
constexpr uint8_t DAMAGE_TYPE_DIVINE = 22;
constexpr uint8_t DAMAGE_TYPE_DEMONIC = 23;
constexpr uint8_t DAMAGE_TYPE_PHYSICAL_PERCENT = 24;
constexpr uint8_t DAMAGE_TYPE_MAGIC_PERCENT = 25;
constexpr uint8_t DAMAGE_TYPE_HP_PERCENT = 26;
constexpr uint8_t DAMAGE_TYPE_MP_PERCENT = 27;
constexpr uint8_t DAMAGE_TYPE_HP_CURRENT = 28;
constexpr uint8_t DAMAGE_TYPE_MP_CURRENT = 29;
constexpr uint8_t DAMAGE_TYPE_HP_MAX = 30;
constexpr uint8_t DAMAGE_TYPE_MP_MAX = 31;

// Tipos de chat
constexpr uint8_t CHAT_TYPE_NORMAL = 0;
constexpr uint8_t CHAT_TYPE_WHISPER = 1;
constexpr uint8_t CHAT_TYPE_PARTY = 2;
constexpr uint8_t CHAT_TYPE_GUILD = 3;
constexpr uint8_t CHAT_TYPE_GLOBAL = 4;
constexpr uint8_t CHAT_TYPE_NOTICE = 5;
constexpr uint8_t CHAT_TYPE_SHOUT = 6;
constexpr uint8_t CHAT_TYPE_TRADE = 7;
constexpr uint8_t CHAT_TYPE_SYSTEM = 8;
constexpr uint8_t CHAT_TYPE_GM = 9;
constexpr uint8_t CHAT_TYPE_GM_WHISPER = 10;
constexpr uint8_t CHAT_TYPE_QUEST = 11;
constexpr uint8_t CHAT_TYPE_COMMAND = 12;
constexpr uint8_t CHAT_TYPE_EMOTION = 13;
constexpr uint8_t CHAT_TYPE_GROUP = 14;
constexpr uint8_t CHAT_TYPE_ALLY = 15;
constexpr uint8_t CHAT_TYPE_ENEMY = 16;
constexpr uint8_t CHAT_TYPE_ANNOUNCE = 17;
constexpr uint8_t CHAT_TYPE_PRIVATE = 18;
constexpr uint8_t CHAT_TYPE_CUSTOM = 19;
constexpr uint8_t CHAT_TYPE_AREA = 20;
constexpr uint8_t CHAT_TYPE_BATTLEFIELD = 21;
constexpr uint8_t CHAT_TYPE_EVENT = 22;
constexpr uint8_t CHAT_TYPE_HELP = 23;
constexpr uint8_t CHAT_TYPE_INFO = 24;
constexpr uint8_t CHAT_TYPE_WARNING = 25;
constexpr uint8_t CHAT_TYPE_ERROR = 26;
constexpr uint8_t CHAT_TYPE_DEBUG = 27;
constexpr uint8_t CHAT_TYPE_UNKNOWN = 28;
constexpr uint8_t CHAT_TYPE_RESERVED_1 = 29;
constexpr uint8_t CHAT_TYPE_RESERVED_2 = 30;
constexpr uint8_t CHAT_TYPE_RESERVED_3 = 31;

// Tipos de comando
constexpr uint8_t COMMAND_TYPE_PLAYER = 0;
constexpr uint8_t COMMAND_TYPE_GM = 1;
constexpr uint8_t COMMAND_TYPE_ADMIN = 2;
constexpr uint8_t COMMAND_TYPE_DEVELOPER = 3;
constexpr uint8_t COMMAND_TYPE_OWNER = 4;
constexpr uint8_t COMMAND_TYPE_SYSTEM = 5;
constexpr uint8_t COMMAND_TYPE_SCRIPT = 6;
constexpr uint8_t COMMAND_TYPE_EVENT = 7;
constexpr uint8_t COMMAND_TYPE_QUEST = 8;
constexpr uint8_t COMMAND_TYPE_NPC = 9;
constexpr uint8_t COMMAND_TYPE_CUSTOM = 10;
constexpr uint8_t COMMAND_TYPE_UNKNOWN = 11;
constexpr uint8_t COMMAND_TYPE_RESERVED_1 = 12;
constexpr uint8_t COMMAND_TYPE_RESERVED_2 = 13;
constexpr uint8_t COMMAND_TYPE_RESERVED_3 = 14;
constexpr uint8_t COMMAND_TYPE_RESERVED_4 = 15;

// Tipos de grupo
constexpr uint8_t PARTY_TYPE_NORMAL = 0;
constexpr uint8_t PARTY_TYPE_RAID = 1;
constexpr uint8_t PARTY_TYPE_ALLIANCE = 2;
constexpr uint8_t PARTY_TYPE_CUSTOM = 3;
constexpr uint8_t PARTY_TYPE_UNKNOWN = 4;
constexpr uint8_t PARTY_TYPE_RESERVED_1 = 5;
constexpr uint8_t PARTY_TYPE_RESERVED_2 = 6;
constexpr uint8_t PARTY_TYPE_RESERVED_3 = 7;

// Tipos de guilda
constexpr uint8_t GUILD_TYPE_NORMAL = 0;
constexpr uint8_t GUILD_TYPE_CLAN = 1;
constexpr uint8_t GUILD_TYPE_FACTION = 2;
constexpr uint8_t GUILD_TYPE_ALLIANCE = 3;
constexpr uint8_t GUILD_TYPE_CUSTOM = 4;
constexpr uint8_t GUILD_TYPE_UNKNOWN = 5;
constexpr uint8_t GUILD_TYPE_RESERVED_1 = 6;
constexpr uint8_t GUILD_TYPE_RESERVED_2 = 7;

// Tipos de evento
constexpr uint8_t EVENT_TYPE_NORMAL = 0;
constexpr uint8_t EVENT_TYPE_BOSS = 1;
constexpr uint8_t EVENT_TYPE_BATTLEFIELD = 2;
constexpr uint8_t EVENT_TYPE_QUEST = 3;
constexpr uint8_t EVENT_TYPE_GUILD = 4;
constexpr uint8_t EVENT_TYPE_PARTY = 5;
constexpr uint8_t EVENT_TYPE_SOLO = 6;
constexpr uint8_t EVENT_TYPE_RAID = 7;
constexpr uint8_t EVENT_TYPE_PVP = 8;
constexpr uint8_t EVENT_TYPE_PVE = 9;
constexpr uint8_t EVENT_TYPE_CUSTOM = 10;
constexpr uint8_t EVENT_TYPE_UNKNOWN = 11;
constexpr uint8_t EVENT_TYPE_RESERVED_1 = 12;
constexpr uint8_t EVENT_TYPE_RESERVED_2 = 13;
constexpr uint8_t EVENT_TYPE_RESERVED_3 = 14;
constexpr uint8_t EVENT_TYPE_RESERVED_4 = 15;

// Tipos de notificação
constexpr uint8_t NOTIFICATION_TYPE_NORMAL = 0;
constexpr uint8_t NOTIFICATION_TYPE_SYSTEM = 1;
constexpr uint8_t NOTIFICATION_TYPE_QUEST = 2;
constexpr uint8_t NOTIFICATION_TYPE_ITEM = 3;
constexpr uint8_t NOTIFICATION_TYPE_SKILL = 4;
constexpr uint8_t NOTIFICATION_TYPE_PARTY = 5;
constexpr uint8_t NOTIFICATION_TYPE_GUILD = 6;
constexpr uint8_t NOTIFICATION_TYPE_FRIEND = 7;
constexpr uint8_t NOTIFICATION_TYPE_TRADE = 8;
constexpr uint8_t NOTIFICATION_TYPE_BATTLE = 9;
constexpr uint8_t NOTIFICATION_TYPE_EVENT = 10;
constexpr uint8_t NOTIFICATION_TYPE_ACHIEVEMENT = 11;
constexpr uint8_t NOTIFICATION_TYPE_MAIL = 12;
constexpr uint8_t NOTIFICATION_TYPE_AUCTION = 13;
constexpr uint8_t NOTIFICATION_TYPE_RANKING = 14;
constexpr uint8_t NOTIFICATION_TYPE_CUSTOM = 15;
constexpr uint8_t NOTIFICATION_TYPE_UNKNOWN = 16;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_1 = 17;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_2 = 18;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_3 = 19;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_4 = 20;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_5 = 21;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_6 = 22;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_7 = 23;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_8 = 24;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_9 = 25;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_10 = 26;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_11 = 27;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_12 = 28;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_13 = 29;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_14 = 30;
constexpr uint8_t NOTIFICATION_TYPE_RESERVED_15 = 31;

// Tipos de erro
constexpr uint8_t ERROR_TYPE_NORMAL = 0;
constexpr uint8_t ERROR_TYPE_SYSTEM = 1;
constexpr uint8_t ERROR_TYPE_NETWORK = 2;
constexpr uint8_t ERROR_TYPE_DATABASE = 3;
constexpr uint8_t ERROR_TYPE_AUTHENTICATION = 4;
constexpr uint8_t ERROR_TYPE_AUTHORIZATION = 5;
constexpr uint8_t ERROR_TYPE_VALIDATION = 6;
constexpr uint8_t ERROR_TYPE_PERMISSION = 7;
constexpr uint8_t ERROR_TYPE_RESOURCE = 8;
constexpr uint8_t ERROR_TYPE_CONFIGURATION = 9;
constexpr uint8_t ERROR_TYPE_INITIALIZATION = 10;
constexpr uint8_t ERROR_TYPE_EXECUTION = 11;
constexpr uint8_t ERROR_TYPE_TIMEOUT = 12;
constexpr uint8_t ERROR_TYPE_OVERFLOW = 13;
constexpr uint8_t ERROR_TYPE_UNDERFLOW = 14;
constexpr uint8_t ERROR_TYPE_DIVIDE_BY_ZERO = 15;
constexpr uint8_t ERROR_TYPE_NULL_POINTER = 16;
constexpr uint8_t ERROR_TYPE_INVALID_PARAMETER = 17;
constexpr uint8_t ERROR_TYPE_INVALID_STATE = 18;
constexpr uint8_t ERROR_TYPE_INVALID_OPERATION = 19;
constexpr uint8_t ERROR_TYPE_NOT_FOUND = 20;
constexpr uint8_t ERROR_TYPE_ALREADY_EXISTS = 21;
constexpr uint8_t ERROR_TYPE_NOT_IMPLEMENTED = 22;
constexpr uint8_t ERROR_TYPE_NOT_SUPPORTED = 23;
constexpr uint8_t ERROR_TYPE_DEPRECATED = 24;
constexpr uint8_t ERROR_TYPE_EXPERIMENTAL = 25;
constexpr uint8_t ERROR_TYPE_CUSTOM = 26;
constexpr uint8_t ERROR_TYPE_UNKNOWN = 27;
constexpr uint8_t ERROR_TYPE_RESERVED_1 = 28;
constexpr uint8_t ERROR_TYPE_RESERVED_2 = 29;
constexpr uint8_t ERROR_TYPE_RESERVED_3 = 30;
constexpr uint8_t ERROR_TYPE_RESERVED_4 = 31;

// Tamanhos máximos de dados
constexpr uint16_t MAX_NAME_LENGTH = 16;
constexpr uint16_t MAX_MESSAGE_LENGTH = 128;
constexpr uint16_t MAX_PASSWORD_LENGTH = 16;
constexpr uint16_t MAX_VERSION_LENGTH = 24;
constexpr uint16_t MAX_DESCRIPTION_LENGTH = 128;
constexpr uint16_t MAX_PATH_LENGTH = 256;
constexpr uint16_t MAX_URL_LENGTH = 256;
constexpr uint16_t MAX_SQL_LENGTH = 4096;
constexpr uint16_t MAX_SCRIPT_LENGTH = 4096;
constexpr uint16_t MAX_CONFIG_LENGTH = 4096;

// Limites de dados
constexpr uint8_t MAX_INVENTORY_SLOTS = 64;
constexpr uint8_t MAX_EQUIP_SLOTS = 16;
constexpr uint8_t MAX_SKILL_SLOTS = 24;
constexpr uint8_t MAX_AFFECT_SLOTS = 32;
constexpr uint8_t MAX_PARTY_MEMBERS = 12;
constexpr uint8_t MAX_GUILD_MEMBERS = 50;
constexpr uint8_t MAX_FRIEND_LIST = 30;
constexpr uint8_t MAX_ENEMY_LIST = 30;
constexpr uint8_t MAX_MAIL_ITEMS = 5;
constexpr uint8_t MAX_AUCTION_ITEMS = 10;
constexpr uint8_t MAX_TRADE_ITEMS = 12;
constexpr uint8_t MAX_SHOP_ITEMS = 27;
constexpr uint8_t MAX_STORAGE_SLOTS = 120;
constexpr uint8_t MAX_GUILDBANK_SLOTS = 120;
constexpr uint8_t MAX_ACHIEVEMENT_SLOTS = 64;
constexpr uint8_t MAX_TITLE_SLOTS = 32;
constexpr uint8_t MAX_QUEST_SLOTS = 16;
constexpr uint8_t MAX_LOG_ENTRIES = 100;
constexpr uint8_t MAX_CHAT_HISTORY = 100;
constexpr uint8_t MAX_COMMAND_HISTORY = 50;
constexpr uint8_t MAX_MAP_ENTITIES = 255;
constexpr uint16_t MAX_MAP_WIDTH = 4096;
constexpr uint16_t MAX_MAP_HEIGHT = 4096;
constexpr uint16_t MAX_TELEPORT_POINTS = 255;
constexpr uint16_t MAX_SPAWN_POINTS = 255;
constexpr uint16_t MAX_EVENT_POINTS = 255;
constexpr uint16_t MAX_QUEST_POINTS = 255;
constexpr uint16_t MAX_SHOP_POINTS = 255;
constexpr uint16_t MAX_NPC_POINTS = 255;
constexpr uint16_t MAX_WARP_POINTS = 255;
constexpr uint16_t MAX_BOSS_POINTS = 255;

// Flags de personagem
constexpr uint32_t CHARACTER_FLAG_NONE = 0x00000000;
constexpr uint32_t CHARACTER_FLAG_ADMIN = 0x00000001;
constexpr uint32_t CHARACTER_FLAG_GM = 0x00000002;
constexpr uint32_t CHARACTER_FLAG_DEV = 0x00000004;
constexpr uint32_t CHARACTER_FLAG_TESTER = 0x00000008;
constexpr uint32_t CHARACTER_FLAG_PREMIUM = 0x00000010;
constexpr uint32_t CHARACTER_FLAG_VIP = 0x00000020;
constexpr uint32_t CHARACTER_FLAG_DONOR = 0x00000040;
constexpr uint32_t CHARACTER_FLAG_BANNED = 0x00000080;
constexpr uint32_t CHARACTER_FLAG_MUTED = 0x00000100;
constexpr uint32_t CHARACTER_FLAG_NEWBIE = 0x00000200;
constexpr uint32_t CHARACTER_FLAG_PVP = 0x00000400;
constexpr uint32_t CHARACTER_FLAG_PK = 0x00000800;
constexpr uint32_t CHARACTER_FLAG_PARTY = 0x00001000;
constexpr uint32_t CHARACTER_FLAG_GUILD = 0x00002000;
constexpr uint32_t CHARACTER_FLAG_AFK = 0x00004000;
constexpr uint32_t CHARACTER_FLAG_INVISIBLE = 0x00008000;
constexpr uint32_t CHARACTER_FLAG_INVULNERABLE = 0x00010000;
constexpr uint32_t CHARACTER_FLAG_NOATTACK = 0x00020000;
constexpr uint32_t CHARACTER_FLAG_NOVIP = 0x00040000;
constexpr uint32_t CHARACTER_FLAG_NOPREMIUM = 0x00080000;
constexpr uint32_t CHARACTER_FLAG_NOTRADE = 0x00100000;
constexpr uint32_t CHARACTER_FLAG_NOSHOP = 0x00200000;
constexpr uint32_t CHARACTER_FLAG_NOPARTY = 0x00400000;
constexpr uint32_t CHARACTER_FLAG_NOGUILD = 0x00800000;
constexpr uint32_t CHARACTER_FLAG_NOFRIEND = 0x01000000;
constexpr uint32_t CHARACTER_FLAG_NOPVP = 0x02000000;
constexpr uint32_t CHARACTER_FLAG_NOPK = 0x04000000;
constexpr uint32_t CHARACTER_FLAG_NOCHAT = 0x08000000;
constexpr uint32_t CHARACTER_FLAG_NOWHISPER = 0x10000000;
constexpr uint32_t CHARACTER_FLAG_NOCOMMAND = 0x20000000;
constexpr uint32_t CHARACTER_FLAG_NOSKILL = 0x40000000;
constexpr uint32_t CHARACTER_FLAG_NOITEM = 0x80000000;

// Flags de item
constexpr uint32_t ITEM_FLAG_NONE = 0x00000000;
constexpr uint32_t ITEM_FLAG_BIND_ON_EQUIP = 0x00000001;
constexpr uint32_t ITEM_FLAG_BIND_ON_PICKUP = 0x00000002;
constexpr uint32_t ITEM_FLAG_BIND_ON_USE = 0x00000004;
constexpr uint32_t ITEM_FLAG_BIND_ON_TRADE = 0x00000008;
constexpr uint32_t ITEM_FLAG_TRADABLE = 0x00000010;
constexpr uint32_t ITEM_FLAG_SELLABLE = 0x00000020;
constexpr uint32_t ITEM_FLAG_DROPABLE = 0x00000040;
constexpr uint32_t ITEM_FLAG_DESTROYABLE = 0x00000080;
constexpr uint32_t ITEM_FLAG_QUESTITEM = 0x00000100;
constexpr uint32_t ITEM_FLAG_UNIQUE = 0x00000200;
constexpr uint32_t ITEM_FLAG_STACKABLE = 0x00000400;
constexpr uint32_t ITEM_FLAG_ENCHANTABLE = 0x00000800;
constexpr uint32_t ITEM_FLAG_IDENTIFIABLE = 0x00001000;
constexpr uint32_t ITEM_FLAG_REPAIRABLE = 0x00002000;
constexpr uint32_t ITEM_FLAG_UPGRADABLE = 0x00004000;
constexpr uint32_t ITEM_FLAG_SOCKETABLE = 0x00008000;
constexpr uint32_t ITEM_FLAG_DECOMPOSABLE = 0x00010000;
constexpr uint32_t ITEM_FLAG_EXTRACTABLE = 0x00020000;
constexpr uint32_t ITEM_FLAG_REFINABLE = 0x00040000;
constexpr uint32_t ITEM_FLAG_EVENT = 0x00080000;
constexpr uint32_t ITEM_FLAG_CASH = 0x00100000;
constexpr uint32_t ITEM_FLAG_PREMIUM = 0x00200000;
constexpr uint32_t ITEM_FLAG_VIP = 0x00400000;
constexpr uint32_t ITEM_FLAG_DONATION = 0x00800000;
constexpr uint32_t ITEM_FLAG_SOULBOUND = 0x01000000;
constexpr uint32_t ITEM_FLAG_ACCOUNTBOUND = 0x02000000;
constexpr uint32_t ITEM_FLAG_TEMPORARY = 0x04000000;
constexpr uint32_t ITEM_FLAG_PERMANENT = 0x08000000;
constexpr uint32_t ITEM_FLAG_TIMED = 0x10000000;
constexpr uint32_t ITEM_FLAG_LIMITED = 0x20000000;
constexpr uint32_t ITEM_FLAG_COSMETIC = 0x40000000;
constexpr uint32_t ITEM_FLAG_FUNCTIONAL = 0x80000000;

// Flags de habilidade
constexpr uint32_t SKILL_FLAG_NONE = 0x00000000;
constexpr uint32_t SKILL_FLAG_PASSIVE = 0x00000001;
constexpr uint32_t SKILL_FLAG_ACTIVE = 0x00000002;
constexpr uint32_t SKILL_FLAG_TOGGLE = 0x00000004;
constexpr uint32_t SKILL_FLAG_BUFF = 0x00000008;
constexpr uint32_t SKILL_FLAG_DEBUFF = 0x00000010;
constexpr uint32_t SKILL_FLAG_HEAL = 0x00000020;
constexpr uint32_t SKILL_FLAG_DAMAGE = 0x00000040;
constexpr uint32_t SKILL_FLAG_PHYSICAL = 0x00000080;
constexpr uint32_t SKILL_FLAG_MAGICAL = 0x00000100;
constexpr uint32_t SKILL_FLAG_COOLDOWN = 0x00000200;
constexpr uint32_t SKILL_FLAG_CHANNELED = 0x00000400;
constexpr uint32_t SKILL_FLAG_INSTANT = 0x00000800;
constexpr uint32_t SKILL_FLAG_TARGET = 0x00001000;
constexpr uint32_t SKILL_FLAG_AOE = 0x00002000;
constexpr uint32_t SKILL_FLAG_PROJECTILE = 0x00004000;
constexpr uint32_t SKILL_FLAG_CHARGE = 0x00008000;
constexpr uint32_t SKILL_FLAG_DASH = 0x00010000;
constexpr uint32_t SKILL_FLAG_JUMP = 0x00020000;
constexpr uint32_t SKILL_FLAG_TELEPORT = 0x00040000;
constexpr uint32_t SKILL_FLAG_SUMMON = 0x00080000;
constexpr uint32_t SKILL_FLAG_TRANSFORM = 0x00100000;
constexpr uint32_t SKILL_FLAG_ULTIMATE = 0x00200000;
constexpr uint32_t SKILL_FLAG_PASSIVE_TOGGLE = 0x00400000;
constexpr uint32_t SKILL_FLAG_COMBO = 0x00800000;
constexpr uint32_t SKILL_FLAG_CHAIN = 0x01000000;
constexpr uint32_t SKILL_FLAG_COUNTER = 0x02000000;
constexpr uint32_t SKILL_FLAG_REFLECT = 0x04000000;
constexpr uint32_t SKILL_FLAG_STANCE = 0x08000000;
constexpr uint32_t SKILL_FLAG_FORM = 0x10000000;
constexpr uint32_t SKILL_FLAG_STEALTH = 0x20000000;
constexpr uint32_t SKILL_FLAG_DISABLE = 0x40000000;
constexpr uint32_t SKILL_FLAG_SPECIAL = 0x80000000;

} // namespace constants
} // namespace wydbr

#endif // _TM_CONSTANTS_H_