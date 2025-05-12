/**
 * GlobalDef.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/GlobalDef.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _GLOBALDEF_H_
#define _GLOBALDEF_H_

/**
 * @file GlobalDef.h
 * @brief Definições globais para o servidor WYDBR
 * 
 * Este arquivo contém definições globais, constantes, macros e tipos
 * utilizados em todo o projeto WYDBR, mantendo compatibilidade binária
 * completa com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <random>
#include <ctime>
#include <cmath>
#include <limits>
#include <sys/stat.h>
#include <sys/types.h>

// Definição de namespace
namespace wydbr {

// Versão do servidor
#define SERVER_VERSION "1.0.0"
#define SERVER_BUILD 2025
#define PROTOCOL_VERSION 0x7B01 // 31489 em decimal

// Definições de tamanhos máximos
#define MAX_CLIENTS 1000
#define MAX_PLAYER_PER_ACCOUNT 4
#define NAME_LENGTH 16
#define ACCOUNTNAME_LENGTH 16
#define MAX_INVENTORY 64
#define MAX_EQUIP 16
#define MAX_PARTY 12
#define MAX_AFFECT 32
#define MAX_SKILL 24
#define MAX_MOB 30000
#define MAX_NPCGENERATOR 8192
#define MAX_CLAN 28
#define MAX_CMD 128
#define MAX_AUTOTRADETITLE 27
#define MAX_GROUNDITEMS 5000
#define MAX_ITEM 6500

// Configurações de rede
#define BASE_PORT 8281
#define CLIENT_TIMEOUT 180000 // 3 minutos em ms
#define MAX_MESSAGE_LENGTH 512
#define MAX_PACKET_SIZE 4096

// Definições de mapas
#define MAX_GRIDX 4096
#define MAX_GRIDY 4096
#define WARD_RANGE 12
#define HALFGRID 16
#define MAX_INCUBATOR 10

// IDs especiais
#define ITEM_SONGPYUN_LEVEL_REQUIRED 100
#define ITEM_SONG_REQUIRED_LEVEL 100
#define ITEM_UPGRADE_RARE 400
#define ITEM_STAR 633

// Constantes de chat
#define COLOR_WHISPER 5
#define COLOR_GUILD 7
#define COLOR_PARTY 6
#define COLOR_CLAN 8
#define COLOR_NORMAL 0
#define COLOR_GM 17
#define COLOR_GLOBAL 19
#define COLOR_NOTICE 11
#define SYSTEM_MESSAGE_COLOR 11

// Limites e constantes numéricas
#define LIMIT_GOLD 2000000000
#define LIMIT_CARRY 3000
#define BILLING_FREE 0
#define BILLING_NORMAL 1
#define BILLING_PREMIUM 2
#define BILLING_KING 3

// Arquivos de configuração
#define CONFIG_PORT_CLIENTS "wyd_clients.port"
#define CONFIG_PORT_SERVERS "wyd_servers.port"
#define CONFIG_IP_CLIENTS "wyd_clients.ip"
#define CONFIG_HOSTADDR_CLIENTS "wyd_clients.hostname"
#define CONFIG_HOSTNAME_CLIENTS "wyd_clients.hostname"
#define CTABLE_PATH "npc.txt"

// Tipos de variáveis
typedef uint32_t HCELL;
typedef uint32_t HEFFECT;
typedef uint32_t HSTALL;
typedef uint32_t HGUILD;
typedef uint64_t DHPROF;

// Enumerações

/**
 * @brief Tipos de efeitos
 */
enum EEFFECT
{
    EF_NONE = 0,
    EF_LEVEL = 1,
    EF_DAMAGE = 2,
    EF_AC = 3,
    EF_HP = 4,
    EF_MP = 5,
    EF_PUBLIC_QUERY = 6,
    EF_AUTOTRADE = 7,
    EF_MOVE_SPEED = 8,
    EF_CRITICAL = 9,
    EF_RECOVER = 10,
    EF_ACADD = 11,
    EF_RESIST1 = 12,
    EF_RESIST2 = 13,
    EF_RESIST3 = 14,
    EF_RESIST4 = 15,
    EF_CUPID = 16,
    EF_ATTACK_SPEED = 17,
    EF_ATTACK_RANGE = 18,
    EF_WITHDRAW_SPECIFIC = 19,
    EF_AFFECT_STRIKE = 20,
    EF_HPADD = 21,
    EF_MPADD = 22,
    EF_POISON = 23,
    EF_SLOW = 24,
    EF_DOUBLE = 25,
    EF_MOUNT = 26,
    EF_STUN = 27,
    EF_RESIST_SWORD = 28,
    EF_RESIST_ARROW = 29,
    EF_RESIST_MAGIC = 30,
    EF_RESIST_FIRE = 31,
    EF_RESIST_COLD = 32,
    EF_RESIST_POISON = 33,
    EF_RESIST_THUNDER = 34,
    EF_PC_PK_OPTION = 35,
    EF_PK_MODE = 36,
    EF_WEAPON_POWER = 37,
    EF_IMPROVE_POT = 38,
    EF_SKILL_JUDGE = 39,
    EF_SPEED_SPELL = 40
};

/**
 * @brief Tipos de itens
 */
enum ETYPE_ITEM
{
    ITEM_NONE = 0,
    ITEM_ARMOR = 1,
    ITEM_SHIELD = 2,
    ITEM_WEAPON = 3,
    ITEM_POTION = 4,
    ITEM_QUEST = 5,
    ITEM_MOUNT = 6,
    ITEM_SPECIAL = 7,
    ITEM_ACCESSORY = 8,
    ITEM_MATERIAL = 9,
    ITEM_ETC = 10
};

/**
 * @brief Tipos de subclasse
 */
enum ESUB_CLASSE
{
    SUBCLASSE_MORTAL = 0,
    SUBCLASSE_ARCH = 1,
    SUBCLASSE_CELESTIAL = 2,
};

/**
 * @brief Tipos de batalha
 */
enum ETIPO_BATALHA
{
    MORTAL = 1,
    ARCH = 2,
    CELESTIAL = 3,
    CLANNISTA = 4,
    GUILDNISTA = 5,
    REINO = 6,
    REI = 7,
    TORRE_RVR = 8
};

/**
 * @brief Estados do servidor
 */
enum ESERVER_STATE
{
    SERVER_CLOSED = 0,
    SERVER_REBOOT = 1,
    SERVER_NORMAL = 2,
    SERVER_SHUTTING_DOWN = 3,
    SERVER_MAINTENANCE = 4
};

/**
 * @brief Tipos de ação
 */
enum EACTION_TYPE
{
    ACTION_MOVE = 1,
    ACTION_ATTACK = 2,
    ACTION_SPELL = 3,
    ACTION_SOCIAL = 4,
    ACTION_ITEM_PICKUP = 5,
    ACTION_ITEM_DROP = 6,
    ACTION_TRADE_START = 7,
    ACTION_TRADE_CONFIRM = 8,
    ACTION_TRADE_CANCEL = 9,
    ACTION_WARP = 10,
    ACTION_KILL = 11,
    ACTION_HUNTING = 12,
    ACTION_COUNTER = 13
};

/**
 * @brief Tipos de classe
 */
enum EPLAYER_CLASS
{
    CLASS_TRANSKNIGHT = 0,
    CLASS_FOEMA = 1,
    CLASS_BEASTMASTER = 2,
    CLASS_HUNTRESS = 3
};

/**
 * @brief Efeitos de afecção
 */
enum EAFFECT
{
    AFF_NONE = 0,
    AFF_POISENED = 1,
    AFF_CURSED = 2,
    AFF_CONFUSED = 4,
    AFF_SILENCED = 8,
    AFF_MAGIC_SHIELD = 16,
    AFF_VISION = 32,
    AFF_DARKNESS = 64,
    AFF_SLEEPING = 128,
    AFF_SNOOP = 256,
    AFF_SUMMONED = 512,
    AFF_CRITICAL = 1024,
    AFF_SLOW = 2048,
    AFF_HASTE = 4096,
    AFF_CLEAR = 8192,
    AFF_STUN = 16384,
    AFF_PARRY = 32768,
    AFF_TRANSPARENT = 65536,
    AFF_HOLD = 131072,
    AFF_RESIST_COLD = 262144,
    AFF_RESIST_THUNDER = 524288,
    AFF_RESIST_WEAPON = 1048576,
    AFF_RESIST_FIRE = 2097152,
    AFF_RESIST_MAGIC = 4194304,
    AFF_HIDE = 8388608,
    AFF_LOW_HEALTH = 16777216,
    AFF_CHAOS = 33554432,
    AFF_INVISIBLE = 67108864,
    AFF_CLOAK = 134217728,
    AFF_DRAGON_SCALE = 268435456,
    AFF_ICE = 536870912,
    AFF_TELEPORT = 1073741824,
    AFF2_DRAGON_INVIS = 1,
    AFF2_DRAGON_HIDE = 2,
    AFF2_DRAGON_SKILL = 4
};

/**
 * @brief Tipos de pacotes
 */
enum EPACKET_TYPE
{
    PKT_KEEP_ALIVE = 0x2D9,
    PKT_INITIAL = 0x213,
    PKT_ACCOUNT_LOGIN = 0x20D,
    PKT_CHARACTER_LOGOUT = 0x2C9,
    PKT_CHARACTER_CREATE = 0x20F,
    PKT_ACCOUNT_LOGOUT = 0x15D,
    PKT_SERVER_CHANGE = 0x2E0,
    PKT_CHAT = 0x334,
    PKT_MOVE = 0x37C,
    PKT_ATTACK = 0x37D,
    PKT_ITEM_DROP = 0x178,
    PKT_MOVEMENT_ON_CMD = 0x2BE,
    PKT_MAP_TELEPORT_CONFIRM = 0x37E,
    PKT_MAP_TELEPORT = 0x37F,
    PKT_QUEST_HISTORY = 0x28F,
    PKT_QUEST_UPDATE = 0x2FA,
    PKT_GAME_OPT = 0x181,
    PKT_CHARACTER_LOGIN = 0x20A,
    PKT_NUMERIC_CHAR_SELECT = 0x38A,
    PKT_CHARACTER_DELETE = 0x326,
    PKT_PARTY_TARGET = 0x286,
    PKT_PARTY_MEMBER = 0x287,
    PKT_PARTY_LEADER = 0x288,
    PKT_PARTY_KICK = 0x329,
    PKT_PARTY_INFO = 0x3AB,
    PKT_QUEST = 0x390,
    PKT_ITEM_GET = 0x171,
    PKT_TRADE_START = 0x383,
    PKT_TRADE_CLS = 0x382,
    PKT_TRADE_ACCEPT = 0x384,
    PKT_TRADE_SET_ITEM = 0x385,
    PKT_TRADE_SET_GOLD = 0x386,
    PKT_SHOP_LIST = 0x19E,
    PKT_SHOP_OPEN = 0x339,
    PKT_SHOP_CLOSE = 0x33A,
    PKT_SHOP_BUY = 0x379,
    PKT_SHOP_ITEM = 0x2D0,
    PKT_ITEM_REMOVE = 0x2D1,
    PKT_ITEM_EQUIP = 0x36A,
    PKT_COMMAND = 0x36C,
    PKT_SKILL_USE = 0x39D,
    PKT_SKILLIN = 0x3A0,
    PKT_SKILLINPOINT = 0x39F,
    PKT_EFFECT = 0x2D7,
    PKT_ITEM_USE = 0x3A1,
    PKT_STORAGE_GET = 0x2D2,
    PKT_STORAGE_OPEN = 0x33C,
    PKT_STORAGE_CLOSE = 0x33D,
    PKT_STORAGE_ITEM = 0x2D3,
    PKT_STORAGE_GOLD = 0x33E,
    PKT_STORAGE_PUT = 0x2D4,
    PKT_GUILD_MEMBER_LIST = 0x2DB,
    PKT_GUILD_WAR_LIST = 0x2DD,
    PKT_GUILD_LIST = 0x2DC,
    PKT_GUILD_GET_LAST_NOTICE = 0x2CD,
    PKT_GUILD_SET_LAST_NOTICE = 0x2CE,
    PKT_GUILD_CREATE = 0x2C6,
    PKT_GUILD_DISBAND = 0x2C7,
    PKT_GUILD_NOTICE = 0x2CC,
    PKT_GUILD_INFO_UPDATE = 0x2CF,
    PKT_GUILD_INVITE = 0x2C8,
    PKT_GUILD_KICK = 0x2CB,
    PKT_GUILD_JOIN = 0x2CA,
    PKT_TMSRV_CONNECT = 0x3DA,
    PKT_TMSRV_DISCONNECT = 0x3DB,
    PKT_TMSRV_CHARACTER_INFO = 0x3DC,
    PKT_TMSRV_CHARACTER_LIST = 0x3DD,
    PKT_TMSRV_CHARACTER_EQUIP = 0x3DE
};

// Estruturas para compatibilidade binária
#pragma pack(push, 1)

/**
 * @brief Estrutura de score de status
 */
struct STRUCT_SCORE
{
    int32_t Level;
    int32_t Ac;
    int32_t Hp;
    int32_t MaxHp;
    int32_t Mp;
    int32_t MaxMp;
    uint8_t Str;
    uint8_t Int;
    uint8_t Dex;
    uint8_t Con;
    uint16_t Special[4];
};

/**
 * @brief Estrutura de efeito da spell
 */
struct STRUCT_SPELL
{
    uint16_t SkillPoint;
    uint16_t SkillBar[4];
    uint8_t SkillBar1[20];
};

/**
 * @brief Estrutura de Afecção
 */
struct STRUCT_AFFECT
{
    uint8_t Type;
    uint8_t Level;
    uint16_t Value;
    uint32_t Time;
};

/**
 * @brief Estrutura de posição do personagem
 */
struct STRUCT_POSITION
{
    int32_t X;
    int32_t Y;
};

/**
 * @brief Estrutura de quest do personagem
 */
struct STRUCT_QUEST
{
    uint16_t Spawn;
    uint16_t Status;
    uint16_t Value;
};

/**
 * @brief Estrutura de header do pacote
 */
struct STRUCT_PHEADER
{
    uint16_t Size;
    uint16_t Type;
    uint16_t ClientId;
    uint16_t CheckSum;
};

// Cabeçalho completo inclui as estruturas de dados básicas
// Estas são apenas as definições globais compartilhadas

#pragma pack(pop)

// Funções de utilidade

/**
 * @brief Calcula tempo decorrido em milissegundos
 * @param start Tempo inicial
 * @return Tempo decorrido em milissegundos
 */
inline /**
 * GetTickCount
 * 
 * Implementa a funcionalidade GetTickCount conforme especificação original.
 * @param start Parâmetro start
 * @return Retorna uint64_t
 */
 uint64_t GetTickCount(const std::chrono::steady_clock::time_point& start){
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

/**
 * @brief Obtém timestamp atual
 * @return Timestamp atual em segundos
 */
inline /**
 * GetCurrentTime
 * 
 * Implementa a funcionalidade GetCurrentTime conforme especificação original.
 * @return Retorna uint32_t
 */
 uint32_t GetCurrentTime(){
    return static_cast<uint32_t>(time(nullptr));
}

/**
 * @brief Gera um número aleatório entre min e max
 * @param min Valor mínimo
 * @param max Valor máximo
 * @return Número aleatório entre min e max
 */
inline /**
 * Rand
 * 
 * Implementa a funcionalidade Rand conforme especificação original.
 * @param min Parâmetro min
 * @param max Parâmetro max
 * @return Retorna int32_t
 */
 int32_t Rand(int32_t min, int32_t max){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int32_t> dist(min, max);
    /**
 * dist
 * 
 * Implementa a funcionalidade dist conforme especificação original.
 * @param gen Parâmetro gen
 * @return Retorna return
 */

    return dist(gen);
}

/**
 * @brief Calcula a distância entre dois pontos
 * @param x1 Coordenada X do ponto 1
 * @param y1 Coordenada Y do ponto 1
 * @param x2 Coordenada X do ponto 2
 * @param y2 Coordenada Y do ponto 2
 * @return Distância entre os pontos
 */
inline /**
 * Distance
 * 
 * Implementa a funcionalidade Distance conforme especificação original.
 * @param x1 Parâmetro x1
 * @param y1 Parâmetro y1
 * @param x2 Parâmetro x2
 * @param y2 Parâmetro y2
 * @return Retorna float
 */
 float Distance(int32_t x1, int32_t y1, int32_t x2, int32_t y2){
    return std::sqrt(static_cast<float>((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

/**
 * @brief Verifica se o diretório existe
 * @param path Caminho do diretório
 * @return true se existe, false caso contrário
 */
inline /**
 * DirExists
 * 
 * Implementa a funcionalidade DirExists conforme especificação original.
 * @param path Parâmetro path
 * @return Retorna bool
 */
 bool DirExists(const std::string& path){
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

/**
 * @brief Cria um diretório
 * @param path Caminho do diretório
 * @return true se criado com sucesso ou já existe, false caso contrário
 */
inline /**
 * CreateDir
 * 
 * Implementa a funcionalidade CreateDir conforme especificação original.
 * @param path Parâmetro path
 * @return Retorna bool
 */
 bool CreateDir(const std::string& path){
    if (DirExists(path)) {
        return true;
    }
    
#ifdef _WIN32
    return _mkdir(path.c_str()) == 0;
#else
    return mkdir(path.c_str(), 0755) == 0;
#endif
}

} // namespace wydbr

#endif // _GLOBALDEF_H_