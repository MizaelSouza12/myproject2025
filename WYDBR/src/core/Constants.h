/**
 * @file Constants.h
 * @brief Constantes globais do projeto WYDBR
 * @author WYDBR Team
 */

#ifndef WYDBR_CONSTANTS_H
#define WYDBR_CONSTANTS_H

#include <cstdint>
#include <string>
#include <array>

namespace wydbr {
namespace constants {

/**
 * @brief Versão do sistema
 */
namespace version {
    constexpr uint16_t MAJOR = 2;
    constexpr uint16_t MINOR = 0;
    constexpr uint16_t PATCH = 0;
    constexpr const char* BUILD = "alpha";
    constexpr const char* FULL = "2.0.0-alpha";
    constexpr const char* DATE = __DATE__;
    constexpr const char* TIME = __TIME__;
}

/**
 * @brief Limites do sistema
 */
namespace limits {
    constexpr uint32_t MAX_PLAYERS = 10000;
    constexpr uint32_t MAX_CONNECTIONS = 20000;
    constexpr uint32_t MAX_GUILDS = 2000;
    constexpr uint32_t MAX_ITEMS = 5000;
    constexpr uint32_t MAX_MONSTERS = 10000;
    constexpr uint32_t MAX_NPCS = 1000;
    constexpr uint32_t MAX_SKILLS = 500;
    constexpr uint32_t MAX_MAPS = 4000;
    constexpr uint16_t MAX_LEVEL = 400;
    constexpr uint32_t MAX_INVENTORY = 64;
    constexpr uint32_t MAX_STORAGE = 128;
    constexpr uint32_t MAX_TRADE_ITEMS = 15;
    constexpr uint32_t MAX_PARTY_SIZE = 12;
    constexpr uint32_t MAX_GUILD_MEMBERS = 100;
    constexpr uint16_t MAX_NAME_LENGTH = 20;
    constexpr uint16_t MIN_NAME_LENGTH = 3;
    constexpr uint16_t MAX_PASSWORD_LENGTH = 32;
    constexpr uint16_t MIN_PASSWORD_LENGTH = 6;
    constexpr uint16_t MAX_MESSAGE_LENGTH = 512;
    constexpr uint16_t MAX_CHATROOM_NAME = 32;
    constexpr uint16_t MAX_PACKET_SIZE = 8192;
    constexpr uint16_t MAX_PATH_LENGTH = 256;
    constexpr uint16_t MAX_ACCOUNT_NAME_LENGTH = 32;
}

/**
 * @brief Caminhos de diretórios
 */
namespace paths {
    constexpr const char* CONFIG_DIR = "config";
    constexpr const char* DATA_DIR = "data";
    constexpr const char* LOGS_DIR = "logs";
    constexpr const char* SCRIPTS_DIR = "scripts";
    constexpr const char* DATABASE_DIR = "database";
    constexpr const char* MAPS_DIR = "data/maps";
    constexpr const char* ITEMS_DIR = "data/items";
    constexpr const char* MONSTERS_DIR = "data/monsters";
    constexpr const char* NPCS_DIR = "data/npcs";
    constexpr const char* SKILLS_DIR = "data/skills";
    constexpr const char* QUESTS_DIR = "data/quests";
    constexpr const char* EVENTS_DIR = "data/events";
    constexpr const char* ACCOUNT_DIR = "data/accounts";
    constexpr const char* CHARACTER_DIR = "data/characters";
    constexpr const char* GUILD_DIR = "data/guilds";
    constexpr const char* CACHE_DIR = "cache";
    constexpr const char* TEMP_DIR = "temp";
    constexpr const char* BACKUP_DIR = "backups";
}

/**
 * @brief Portas padrão dos servidores
 */
namespace ports {
    constexpr uint16_t DBSRV_PORT = 7514;
    constexpr uint16_t TMSRV_PORT = 7515;
    constexpr uint16_t WSKSRV_PORT = 7516;
    constexpr uint16_t NRSRV_PORT = 7517;
    constexpr uint16_t BISRV_PORT = 7518;
    constexpr uint16_t AGESRV_PORT = 7519;
    constexpr uint16_t CLIENT_PORT = 7520;
    constexpr uint16_t WEB_API_PORT = 8080;
    constexpr uint16_t ADMIN_PORT = 8081;
    constexpr uint16_t MONITOR_PORT = 7500;
}

/**
 * @brief Constantes de rede
 */
namespace network {
    constexpr uint16_t PACKET_HEADER_SIZE = 6;
    constexpr uint16_t PACKET_SIGNATURE = 0x4242; // 'BB'
    constexpr uint32_t CONNECTION_TIMEOUT_MS = 300000; // 5 minutos
    constexpr uint32_t PING_INTERVAL_MS = 30000; // 30 segundos
    constexpr uint32_t MAX_PACKET_RETRIES = 5;
    constexpr uint32_t PACKET_RESEND_DELAY_MS = 1000; // 1 segundo
    constexpr uint32_t SERVER_KEEP_ALIVE_INTERVAL = 15000; // 15 segundos
    constexpr uint8_t ENCRYPT_KEY[16] = {
        0x89, 0x45, 0xC5, 0xD4, 0xE2, 0xF8, 0xAB, 0x24,
        0x37, 0x9A, 0x0C, 0x6B, 0x5F, 0xDE, 0x12, 0x78
    };
    
    // Constantes de operação de pacotes
    constexpr uint8_t PACKET_TYPE_SYSTEM = 0x00;
    constexpr uint8_t PACKET_TYPE_LOGIN = 0x10;
    constexpr uint8_t PACKET_TYPE_GAME = 0x20;
    constexpr uint8_t PACKET_TYPE_CHAT = 0x30;
    constexpr uint8_t PACKET_TYPE_GUILD = 0x40;
    constexpr uint8_t PACKET_TYPE_TRADE = 0x50;
    constexpr uint8_t PACKET_TYPE_PARTY = 0x60;
    constexpr uint8_t PACKET_TYPE_BATTLE = 0x70;
    constexpr uint8_t PACKET_TYPE_ITEM = 0x80;
    constexpr uint8_t PACKET_TYPE_MAP = 0x90;
    constexpr uint8_t PACKET_TYPE_ADMIN = 0xA0;
    constexpr uint8_t PACKET_TYPE_EVENT = 0xB0;
    constexpr uint8_t PACKET_TYPE_CUSTOM = 0xF0;
    constexpr uint8_t PACKET_TYPE_DISCONNECT = 0xFF;
}

/**
 * @brief Constantes de temporizadores
 */
namespace timers {
    constexpr uint32_t SERVER_TICK_RATE_MS = 100; // 10 ticks por segundo
    constexpr uint32_t MAP_UPDATE_INTERVAL_MS = 200; // 5 atualizações por segundo
    constexpr uint32_t CHARACTER_SAVE_INTERVAL_MS = 300000; // 5 minutos
    constexpr uint32_t MONSTER_SPAWN_INTERVAL_MS = 60000; // 1 minuto
    constexpr uint32_t MONSTER_AI_INTERVAL_MS = 1000; // 1 segundo
    constexpr uint32_t NPC_UPDATE_INTERVAL_MS = 5000; // 5 segundos
    constexpr uint32_t SKILL_COOLDOWN_TICK_MS = 100; // 100ms
    constexpr uint32_t WEATHER_CHANGE_INTERVAL_MS = 600000; // 10 minutos
    constexpr uint32_t DAY_NIGHT_CYCLE_MS = 3600000; // 1 hora
    constexpr uint32_t SERVER_STATUS_UPDATE_MS = 60000; // 1 minuto
    constexpr uint32_t DATABASE_BACKUP_INTERVAL_MS = 3600000; // 1 hora
    constexpr uint32_t LOG_ROTATE_INTERVAL_MS = 86400000; // 24 horas
    constexpr uint32_t MAINTENANCE_CHECK_INTERVAL_MS = 3600000; // 1 hora
}

/**
 * @brief Constantes de jogabilidade
 */
namespace gameplay {
    constexpr float BASE_EXP_RATE = 1.0f;
    constexpr float BASE_DROP_RATE = 1.0f;
    constexpr float BASE_GOLD_RATE = 1.0f;
    constexpr uint16_t BASE_STAT_POINTS = 10;
    constexpr uint16_t STAT_POINTS_PER_LEVEL = 3;
    constexpr uint16_t BASE_SKILL_POINTS = 1;
    constexpr uint16_t SKILL_POINTS_PER_LEVEL = 1;
    constexpr uint32_t BASE_HP = 100;
    constexpr uint32_t BASE_MP = 50;
    constexpr uint32_t BASE_STAMINA = 100;
    constexpr float BASE_MOVEMENT_SPEED = 1.0f;
    constexpr float BASE_ATTACK_SPEED = 1.0f;
    constexpr float BASE_CAST_SPEED = 1.0f;
    constexpr float BASE_CRITICAL_RATE = 5.0f; // 5%
    constexpr float MAX_CRITICAL_RATE = 80.0f; // 80%
    constexpr float BASE_CRITICAL_DAMAGE = 150.0f; // 150% do dano normal
    constexpr float BASE_BLOCK_RATE = 10.0f; // 10%
    constexpr float MAX_BLOCK_RATE = 70.0f; // 70%
    constexpr float BASE_DODGE_RATE = 5.0f; // 5%
    constexpr float MAX_DODGE_RATE = 60.0f; // 60%
    constexpr float BASE_HIT_RATE = 95.0f; // 95%
    constexpr float MAX_RESISTANCE = 80.0f; // 80% resistência máxima
    constexpr uint16_t MAX_STAT_VALUE = 999;
    constexpr uint16_t MAX_SKILL_LEVEL = 20;
    constexpr uint32_t RESPAWN_TIME_BASE_MS = 10000; // 10 segundos
    constexpr uint32_t PVP_PENALTY_TIME_MS = 300000; // 5 minutos
    constexpr uint16_t ITEM_DROP_DURATION_MS = 120000; // 2 minutos
    constexpr uint16_t MAX_SKILLS_PER_CHARACTER = 50;
    constexpr uint16_t MAX_BUFFS_PER_CHARACTER = 20;
    constexpr uint16_t MAX_DEBUFFS_PER_CHARACTER = 10;
}

/**
 * @brief IDs de mapeamento
 */
namespace ids {
    enum class ItemType : uint8_t {
        NONE = 0,
        WEAPON = 1,
        ARMOR = 2,
        HELMET = 3,
        SHIELD = 4,
        GLOVES = 5,
        BOOTS = 6,
        ACCESSORY = 7,
        CONSUMABLE = 8,
        MATERIAL = 9,
        QUEST = 10,
        CURRENCY = 11,
        SPECIAL = 12
    };
    
    enum class CharacterClass : uint8_t {
        NONE = 0,
        WARRIOR = 1,
        ARCHER = 2,
        MAGE = 3,
        CLERIC = 4,
        THIEF = 5,
        PALADIN = 6,
        RANGER = 7,
        WARLOCK = 8,
        BISHOP = 9,
        ASSASSIN = 10,
        CUSTOM_CLASS_1 = 20,
        CUSTOM_CLASS_2 = 21,
        GM_CLASS = 99
    };
    
    enum class ElementType : uint8_t {
        NEUTRAL = 0,
        FIRE = 1,
        WATER = 2,
        EARTH = 3,
        WIND = 4,
        LIGHT = 5,
        DARK = 6,
        POISON = 7,
        HOLY = 8
    };
    
    enum class StatusEffect : uint8_t {
        NONE = 0,
        POISON = 1,
        BLIND = 2,
        SILENCE = 3,
        STUN = 4,
        SLEEP = 5,
        PARALYZE = 6,
        CONFUSION = 7,
        CURSE = 8,
        FROZEN = 9,
        BURN = 10,
        BLEED = 11,
        ROOT = 12,
        FEAR = 13,
        CHARM = 14,
        INVINCIBLE = 50,
        INVISIBLE = 51,
        HASTE = 52,
        SLOW = 53,
        REGEN = 54,
        BARRIER = 55,
        REFLECT = 56
    };
    
    enum class MapProperty : uint8_t {
        NONE = 0,
        SAFE_ZONE = 1,
        PVP_ZONE = 2,
        TOWN = 3,
        DUNGEON = 4,
        FIELD = 5,
        ARENA = 6,
        GUILD_AREA = 7,
        INSTANCE = 8,
        UNDERWATER = 9,
        NO_RECALL = 10,
        NO_TELEPORT = 11,
        NO_FLY = 12
    };
    
    enum class GuildRank : uint8_t {
        NONE = 0,
        MASTER = 1,
        SUB_MASTER = 2,
        ELITE = 3,
        MEMBER = 4,
        TRIAL = 5
    };
}

/**
 * @brief Configurações padrão de servidor
 */
namespace defaults {
    constexpr const char* CONFIG_FILE = "server.cfg";
    constexpr const char* DB_HOST = "localhost";
    constexpr const char* DB_USER = "wydbr";
    constexpr const char* DB_NAME = "wydbr";
    constexpr uint16_t DB_PORT = 3306;
    constexpr uint16_t MAX_THREADS = 8;
    constexpr uint16_t SOCKET_TIMEOUT_SEC = 30;
    constexpr uint16_t MAX_CONNECTIONS_PER_IP = 5;
    constexpr bool MAINTENANCE_MODE = false;
    constexpr uint32_t MAX_LOG_SIZE_MB = 100;
    constexpr uint32_t MAX_LOG_FILES = 10;
    constexpr const char* LOG_LEVEL = "INFO";
    constexpr bool ALLOW_MULTIPLE_CLIENTS = false;
    constexpr bool ENABLE_ANTICHEAT = true;
    constexpr bool ENABLE_CHAT_FILTER = true;
    constexpr bool ENABLE_GLOBAL_CHAT = true;
    constexpr bool ENABLE_TRADE = true;
    constexpr bool ENABLE_PVP = true;
    constexpr bool ENABLE_AUTO_SAVE = true;
    constexpr uint32_t AUTO_SAVE_INTERVAL_MIN = 15;
    constexpr bool LOG_CHAT = true;
    constexpr bool LOG_TRADES = true;
    constexpr bool LOG_PLAYER_KILLS = true;
    constexpr bool LOG_LOGINS = true;
    constexpr bool ENABLE_CUSTOM_COMMANDS = true;
    constexpr bool ENABLE_EVENT_SYSTEM = true;
}

} // namespace constants
} // namespace wydbr

#endif // WYDBR_CONSTANTS_H 