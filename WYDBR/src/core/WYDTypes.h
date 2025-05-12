/**
 * WYDTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/core/WYDTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_TYPES_H
#define WYD_TYPES_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cstring>
#include <cmath>

namespace wydbr {


/**
 * @file WYDTypes.h
 * @brief Definições de tipos fundamentais do WYD
 * 
 * Este arquivo contém as definições de tipos fundamentais usados em todo o sistema WYD.
 * Mantém compatibilidade com os tipos originais, mas adiciona segurança e funcionalidades
 * modernas para evitar bugs comuns do sistema original.
 */

namespace wyd {

// Tipos fundamentais alinhados com os tipos originais do WYD
using BYTE = uint8_t;          // 8 bits, sem sinal
using WORD = uint16_t;         // 16 bits, sem sinal
using DWORD = uint32_t;        // 32 bits, sem sinal
using QWORD = uint64_t;        // 64 bits, sem sinal

using SBYTE = int8_t;          // 8 bits, com sinal  
using SWORD = int16_t;         // 16 bits, com sinal
using SDWORD = int32_t;        // 32 bits, com sinal
using SQWORD = int64_t;        // 64 bits, com sinal

using INT = int32_t;           // 32 bits, com sinal
using UINT = uint32_t;         // 32 bits, sem sinal

using FLOAT = float;           // 32 bits, ponto flutuante
using DOUBLE = double;         // 64 bits, ponto flutuante

using CHAR = char;             // Caractere (8 bits)
using WCHAR = wchar_t;         // Caractere unicode (16 bits)

using BOOL = int32_t;          // Boolean (32 bits para compatibilidade)
constexpr BOOL TRUE = 1;       // Valor verdadeiro
constexpr BOOL FALSE = 0;      // Valor falso

// Tipos de ponteiros seguros (evitam bugs comuns de null pointers e dangling pointers)
template<typename T>
using SafePtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

// Alias para strings (compatível com a API moderna)
using String = std::string;
using WString = std::wstring;

// Tipos para identificadores
using ObjectID = DWORD;        // ID de objeto (personagem, NPC, item, etc)
using PlayerID = DWORD;        // ID de jogador
using AccountID = DWORD;       // ID de conta
using SkillID = WORD;          // ID de habilidade
using ItemID = WORD;           // ID de item
using NPCID = WORD;            // ID de NPC
using MapID = WORD;            // ID de mapa
using GuildID = DWORD;         // ID de guild

// Constantes do sistema
constexpr INT MAX_PLAYER = 1000;           // Máximo de jogadores por servidor
constexpr INT MAX_ENEMY = 10000;           // Máximo de inimigos por servidor
constexpr INT MAX_ITEM_SLOT = 64;          // Máximo de slots de inventário 
constexpr INT MAX_EQUIP_SLOT = 16;         // Máximo de slots de equipamento
constexpr INT MAX_SKILLBAR = 20;           // Máximo de slots na barra de habilidades
constexpr INT MAX_TRADE_SLOT = 15;         // Máximo de slots de comércio
constexpr INT MAX_STORAGE_SLOT = 120;      // Máximo de slots de armazenamento
constexpr INT MAX_AFFECT = 32;             // Máximo de efeitos (buffs/debuffs)
constexpr INT MAX_LEVEL = 400;             // Nível máximo de personagem
constexpr INT MAX_SKILL_LEVEL = 20;        // Nível máximo de habilidade
constexpr INT MAX_PARTY = 12;              // Máximo de membros em grupo
constexpr INT MAX_GUILD_MEMBER = 50;       // Máximo de membros em guild

// Tamanhos de pacotes
constexpr INT DEFAULT_PACKET_SIZE = 256;   // Tamanho padrão de pacote
constexpr INT MAX_PACKET_SIZE = 4096;      // Tamanho máximo de pacote

// Enumeradores centrais do sistema
enum class CharacterClass : BYTE {
    TRANSKNIGHT = 0,
    FOEMA = 1,
    BEASTMASTER = 2,
    HUNTRESS = 3,
    TRANS_FOEMA = 4,
    TRANS_BEASTMASTER = 5,
    TRANS_HUNTRESS = 6,
    DEFAULT = 0xFF
};

enum class WeaponType : BYTE {
    SWORD = 0,
    AXE = 1,
    SPEAR = 2,
    BOW = 3,
    STAFF = 4,
    WAND = 5,
    KATAN = 6,
    KNUCKLE = 7,
    UNKNOWN = 0xFF
};

enum class ArmorType : BYTE {
    HEAVY = 0,
    LIGHT = 1,
    ROBE = 2,
    UNKNOWN = 0xFF
};

enum class ItemType : BYTE {
    WEAPON = 0,
    ARMOR = 1,
    HELMET = 2,
    SHIELD = 3,
    BOOTS = 4,
    GLOVES = 5,
    RING = 6,
    AMULET = 7,
    BELT = 8,
    EARRING = 9,
    CONSUMABLE = 10,
    QUEST = 11,
    MATERIAL = 12,
    RIDE = 13,
    MOUNT = 14,
    GUILD = 15,
    UNKNOWN = 0xFF
};

enum class SkillType : BYTE {
    PASSIVE = 0,
    ACTIVE = 1,
    BUFF = 2,
    DEBUFF = 3,
    HEAL = 4,
    SUMMON = 5,
    TELEPORT = 6,
    TRANSFORM = 7,
    UNKNOWN = 0xFF
};

enum class ElementType : BYTE {
    NONE = 0,
    FIRE = 1,
    WATER = 2,
    EARTH = 3,
    WIND = 4,
    HOLY = 5,
    DARK = 6,
    UNKNOWN = 0xFF
};

enum class EffectType : BYTE {
    NONE = 0,
    POISON = 1,
    SLOW = 2,
    STUN = 3,
    BLEED = 4,
    SILENCE = 5,
    BLIND = 6,
    FROZEN = 7,
    BURN = 8,
    PARALYZE = 9,
    HASTE = 10,
    STEALTH = 11,
    INVULNERABLE = 12,
    UNKNOWN = 0xFF
};

// Estrutura de versão para compatibilidade
struct Version {
    BYTE major;            // Versão principal
    BYTE minor;            // Versão secundária
    WORD build;            // Número da build
    
    // Construtores
    Version() : major(0), minor(0), build(0) {}
    Version(BYTE maj, BYTE min, WORD bld) : major(maj), minor(min), build(bld) {}
    
    // Operadores de comparação
    bool operator==(const Version& other) const {
        return major == other.major && minor == other.minor && build == other.build;
    }
    
    bool operator!=(const Version& other) const {
        return !(*this == other);
    }
    
    bool operator<(const Version& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return build < other.build;
    }
    
    bool operator>(const Version& other) const {
        return other < *this;
    }
    
    bool operator<=(const Version& other) const {
        return !(other < *this);
    }
    
    bool operator>=(const Version& other) const {
        return !(*this < other);
    }
    
    // Conversão para string
    /**
 * toString
 * 
 * Implementa a funcionalidade toString conforme especificação original.
 * @return Retorna String
 */

    String toString() const{
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(build);
    }
};

// Coordenadas 2D e 3D com funções de utilidade
struct Position2D {
    INT x;
    INT y;
    
    Position2D() : x(0), y(0) {}
    Position2D(INT _x, INT _y) : x(_x), y(_y) {}
    
    // Distância entre duas posições
    /**
 * distance
 * 
 * Implementa a funcionalidade distance conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna INT
 */

    INT distance(const Position2D& other) const{
        INT dx = x - other.x;
        INT dy = y - other.y;
        return static_cast<INT>(sqrt(dx*dx + dy*dy));
    }
    
    // Operadores aritméticos
    Position2D operator+(const Position2D& other) const {
        /**
 * Position2D
 * 
 * Implementa a funcionalidade Position2D conforme especificação original.
 * @param other.x Parâmetro other.x
 * @param other.y Parâmetro other.y
 * @return Retorna return
 */

        return Position2D(x + other.x, y + other.y);
    }
    
    Position2D operator-(const Position2D& other) const {
        /**
 * Position2D
 * 
 * Implementa a funcionalidade Position2D conforme especificação original.
 * @param other.x Parâmetro other.x
 * @param other.y Parâmetro other.y
 * @return Retorna return
 */

        return Position2D(x - other.x, y - other.y);
    }
    
    bool operator==(const Position2D& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Position2D& other) const {
        return !(*this == other);
    }
    
    // Verifica se está dentro de um raio
    /**
 * isInRange
 * 
 * Implementa a funcionalidade isInRange conforme especificação original.
 * @param center Parâmetro center
 * @param radius Parâmetro radius
 * @return Retorna bool
 */

    bool isInRange(const Position2D& center, INT radius) const{
        return distance(center) <= radius;
    }
};

struct Position3D {
    INT x;
    INT y;
    INT z;
    
    Position3D() : x(0), y(0), z(0) {}
    Position3D(INT _x, INT _y, INT _z) : x(_x), y(_y), z(_z) {}
    
    // Distância entre duas posições
    /**
 * distance
 * 
 * Implementa a funcionalidade distance conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna INT
 */

    INT distance(const Position3D& other) const{
        INT dx = x - other.x;
        INT dy = y - other.y;
        INT dz = z - other.z;
        return static_cast<INT>(sqrt(dx*dx + dy*dy + dz*dz));
    }
    
    // Operadores aritméticos
    Position3D operator+(const Position3D& other) const {
        /**
 * Position3D
 * 
 * Implementa a funcionalidade Position3D conforme especificação original.
 * @param other.x Parâmetro other.x
 * @param other.y Parâmetro other.y
 * @param other.z Parâmetro other.z
 * @return Retorna return
 */

        return Position3D(x + other.x, y + other.y, z + other.z);
    }
    
    Position3D operator-(const Position3D& other) const {
        /**
 * Position3D
 * 
 * Implementa a funcionalidade Position3D conforme especificação original.
 * @param other.x Parâmetro other.x
 * @param other.y Parâmetro other.y
 * @param other.z Parâmetro other.z
 * @return Retorna return
 */

        return Position3D(x - other.x, y - other.y, z - other.z);
    }
    
    bool operator==(const Position3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    bool operator!=(const Position3D& other) const {
        return !(*this == other);
    }
    
    // Verifica se está dentro de um raio
    /**
 * isInRange
 * 
 * Implementa a funcionalidade isInRange conforme especificação original.
 * @param center Parâmetro center
 * @param radius Parâmetro radius
 * @return Retorna bool
 */

    bool isInRange(const Position3D& center, INT radius) const{
        return distance(center) <= radius;
    }
    
    // Converter para Position2D (ignorando z)
    /**
 * to2D
 * 
 * Implementa a funcionalidade to2D conforme especificação original.
 * @return Retorna Position2D
 */

    Position2D to2D() const{
        /**
 * Position2D
 * 
 * Implementa a funcionalidade Position2D conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna return
 */

        return Position2D(x, y);
    }
};

// Estrutura de retângulo para áreas
struct Rect {
    INT left;
    INT top;
    INT right;
    INT bottom;
    
    Rect() : left(0), top(0), right(0), bottom(0) {}
    Rect(INT l, INT t, INT r, INT b) : left(l), top(t), right(r), bottom(b) {}
    
    // Largura do retângulo
    /**
 * width
 * 
 * Implementa a funcionalidade width conforme especificação original.
 * @return Retorna INT
 */

    INT width() const{ return right - left; }
    
    // Altura do retângulo
    /**
 * height
 * 
 * Implementa a funcionalidade height conforme especificação original.
 * @return Retorna INT
 */

    INT height() const{ return bottom - top; }
    
    // Verifica se um ponto está dentro do retângulo
    /**
 * contains
 * 
 * Implementa a funcionalidade contains conforme especificação original.
 * @param pos Parâmetro pos
 * @return Retorna bool
 */

    bool contains(const Position2D& pos) const{
        return pos.x >= left && pos.x <= right && pos.y >= top && pos.y <= bottom;
    }
    
    // Verifica se dois retângulos se sobrepõem
    /**
 * intersects
 * 
 * Implementa a funcionalidade intersects conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna bool
 */

    bool intersects(const Rect& other) const{
        return !(left > other.right || right < other.left || top > other.bottom || bottom < other.top);
    }
};

// Status físico de um personagem ou NPC
enum class PhysicalState : BYTE {
    NORMAL = 0,
    SITTING = 1,  
    RIDING = 2,
    FLYING = 3,
    DEAD = 4,
    STANDING = 5,
    UNKNOWN = 0xFF
};

// Status mental (condição) de um personagem ou NPC
enum class MentalState : BYTE {
    NORMAL = 0,
    CALM = 1,
    ANGRY = 2,
    AFRAID = 3,
    CONFUSED = 4,
    CHARMED = 5,
    CONTROLLED = 6,
    UNKNOWN = 0xFF
};

// Estrutura para atributos básicos 
struct BaseStats {
    WORD strength;
    WORD dexterity; 
    WORD constitution;
    WORD intelligence;
    WORD wisdom;
    WORD charisma;
    
    BaseStats() : 
        strength(10), dexterity(10), constitution(10), 
        intelligence(10), wisdom(10), charisma(10) {}
    
    BaseStats(WORD str, WORD dex, WORD con, WORD inte, WORD wis, WORD cha) :
        strength(str), dexterity(dex), constitution(con),
        intelligence(inte), wisdom(wis), charisma(cha) {}
};

// Estrutura para atributos derivados
struct DerivedStats {
    DWORD maxHP;           // Pontos de vida máximos
    DWORD currentHP;       // Pontos de vida atuais
    DWORD maxMP;           // Pontos de mana máximos
    DWORD currentMP;       // Pontos de mana atuais
    WORD attackMin;        // Dano mínimo
    WORD attackMax;        // Dano máximo
    WORD defense;          // Defesa física
    WORD magicDefense;     // Defesa mágica
    WORD accuracy;         // Precisão
    WORD evasion;          // Evasão
    WORD criticalRate;     // Taxa de acerto crítico
    WORD criticalDamage;   // Dano crítico
    WORD attackSpeed;      // Velocidade de ataque
    WORD castingSpeed;     // Velocidade de conjuração
    WORD moveSpeed;        // Velocidade de movimento
    BYTE resistFire;       // Resistência ao fogo
    BYTE resistWater;      // Resistência à água
    BYTE resistEarth;      // Resistência à terra
    BYTE resistWind;       // Resistência ao vento
    BYTE resistHoly;       // Resistência ao sagrado
    BYTE resistDark;       // Resistência às trevas
    
    DerivedStats() :
        maxHP(100), currentHP(100), maxMP(100), currentMP(100),
        attackMin(1), attackMax(3), defense(0), magicDefense(0),
        accuracy(10), evasion(10), criticalRate(5), criticalDamage(150),
        attackSpeed(100), castingSpeed(100), moveSpeed(100),
        resistFire(0), resistWater(0), resistEarth(0),
        resistWind(0), resistHoly(0), resistDark(0) {}
};

// Estrutura para efeitos (buffs/debuffs)
struct Effect {
    EffectType type;       // Tipo de efeito
    WORD value;            // Valor do efeito
    DWORD duration;        // Duração em milissegundos
    DWORD startTime;       // Timestamp de início
    ObjectID sourceId;     // ID da fonte do efeito
    
    Effect() : 
        type(EffectType::NONE), value(0), duration(0), 
        startTime(0), sourceId(0) {}
    
    Effect(EffectType t, WORD v, DWORD d, DWORD st, ObjectID src) :
        type(t), value(v), duration(d), startTime(st), sourceId(src) {}
    
    // Verifica se o efeito expirou
    /**
 * hasExpired
 * 
 * Implementa a funcionalidade hasExpired conforme especificação original.
 * @param currentTime Parâmetro currentTime
 * @return Retorna bool
 */

    bool hasExpired(DWORD currentTime) const{
        return currentTime >= startTime + duration;
    }
    
    // Retorna o tempo restante
    /**
 * getRemainingTime
 * 
 * Implementa a funcionalidade getRemainingTime conforme especificação original.
 * @param currentTime Parâmetro currentTime
 * @return Retorna DWORD
 */

    DWORD getRemainingTime(DWORD currentTime) const{
        if (hasExpired(currentTime)) return 0;
        return (startTime + duration) - currentTime;
    }
};

// Estrutura que representa um item (com correções de bugs do original)
#pragma pack(push, 1) // Alinhamento de 1 byte para compatibilidade exata
struct Item {
    ItemID itemId;             // ID do item (16 bits)
    BYTE itemEffectId;         // Efeito visual do item
    BYTE itemRefine;           // Nível de refinamento (+0 a +15)
    
    union {
        struct {
            unsigned durability : 10;     // Durabilidade (10 bits)
            unsigned amount : 6;          // Quantidade para itens empilháveis (6 bits)
        };
        WORD durabilityAndAmount;         // Acesso direto aos 16 bits
    };
    
    union {
        struct {
            // Bit fields para atributos especiais, corrigidos do original
            unsigned hasPrefix : 1;       // Item tem prefixo (1 bit)
            unsigned hasSuffix : 1;       // Item tem sufixo (1 bit)
            unsigned isSocket : 1;        // Item é um socket (1 bit)
            unsigned isBlessed : 1;       // Item é abençoado (1 bit)
            unsigned isSealed : 1;        // Item está selado (1 bit)
            unsigned isEthereal : 1;      // Item é etéreo (1 bit)
            unsigned isAncient : 1;       // Item é antigo (1 bit)
            unsigned isPersonal : 1;      // Item é pessoal/não-comercializável (1 bit)
            unsigned prefixId : 8;        // ID do prefixo (8 bits)
            unsigned suffixId : 8;        // ID do sufixo (8 bits)
            unsigned sockets : 4;         // Número de sockets (4 bits)
            unsigned gemId1 : 4;          // ID da gema 1 (4 bits)
        };
        DWORD attributes;                 // Acesso direto aos 32 bits
    };
    
    // Próximos 32 bits
    union {
        struct {
            unsigned gemId2 : 4;          // ID da gema 2 (4 bits)
            unsigned gemId3 : 4;          // ID da gema 3 (4 bits)
            unsigned gemId4 : 4;          // ID da gema 4 (4 bits)
            unsigned gemId5 : 4;          // ID da gema 5 (4 bits)
            unsigned gemId6 : 4;          // ID da gema 6 (4 bits)
            unsigned serverRestrict : 4;  // Restrição de servidor (4 bits)
            unsigned isLocked : 1;        // Item está bloqueado (1 bit)
            unsigned isQuest : 1;         // Item de quest (1 bit)
            unsigned reserved : 2;        // Bits reservados (2 bits)
        };
        DWORD attributes2;                // Acesso direto aos 32 bits
    };
    
    Item() {
        // Inicialização segura de tudo como zero (corrige bug do jogo original)
        memset(this, 0, sizeof(Item));
    }
    
    // Funções para manipulação segura de atributos
    
    // Verifica se o item é vazio (slot vazio)
    /**
 * isEmpty
 * 
 * Implementa a funcionalidade isEmpty conforme especificação original.
 * @return Retorna bool
 */

    bool isEmpty() const{
        return itemId == 0;
    }
    
    // Limpa/zera o item (torna slot vazio)
    /**
 * clear
 * 
 * Implementa a funcionalidade clear conforme especificação original.
 */

    void clear(){
        memset(this, 0, sizeof(Item));
    }
    
    // Funções para manipulação de durabilidade
    /**
 * setDurability
 * 
 * Implementa a funcionalidade setDurability conforme especificação original.
 * @param value Parâmetro value
 */

    void setDurability(WORD value){
        durability = value > 1023 ? 1023 : value; // 10 bits = max 1023
    }
    
    /**
 * getDurability
 * 
 * Implementa a funcionalidade getDurability conforme especificação original.
 * @return Retorna WORD
 */

    
    WORD getDurability() const{
        return durability;
    }
    
    // Funções para manipulação de quantidade
    /**
 * setAmount
 * 
 * Implementa a funcionalidade setAmount conforme especificação original.
 * @param value Parâmetro value
 */

    void setAmount(BYTE value){
        amount = value > 63 ? 63 : value; // 6 bits = max 63
    }
    
    /**
 * getAmount
 * 
 * Implementa a funcionalidade getAmount conforme especificação original.
 * @return Retorna BYTE
 */

    
    BYTE getAmount() const{
        return amount;
    }
    
    // Funções para manipulação de gemas
    /**
 * setGem
 * 
 * Implementa a funcionalidade setGem conforme especificação original.
 * @param slot Parâmetro slot
 * @param gemId Parâmetro gemId
 */

    void setGem(BYTE slot, BYTE gemId){
        if (slot >= 1 && slot <= 6) {
            switch (slot) {
                case 1: gemId1 = gemId > 15 ? 15 : gemId; break; // 4 bits = max 15
                case 2: gemId2 = gemId > 15 ? 15 : gemId; break;
                case 3: gemId3 = gemId > 15 ? 15 : gemId; break;
                case 4: gemId4 = gemId > 15 ? 15 : gemId; break;
                case 5: gemId5 = gemId > 15 ? 15 : gemId; break;
                case 6: gemId6 = gemId > 15 ? 15 : gemId; break;
            }
        }
    }
    
    /**
 * getGem
 * 
 * Implementa a funcionalidade getGem conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna BYTE
 */

    
    BYTE getGem(BYTE slot) const{
        switch (slot) {
            case 1: return gemId1;
            case 2: return gemId2;
            case 3: return gemId3;
            case 4: return gemId4;
            case 5: return gemId5;
            case 6: return gemId6;
            default: return 0;
        }
    }
    
    // Função para serialização/desserialização para compatibilidade com armazenamento legado
    /**
 * fromBinary
 * 
 * Implementa a funcionalidade fromBinary conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna Item
 */

    static Item fromBinary(const void* data, size_t size){
        Item item;
        if (size >= sizeof(Item)) {
            memcpy(&item, data, sizeof(Item));
        }
        return item;
    }
    
    /**
 * toBinary
 * 
 * Implementa a funcionalidade toBinary conforme especificação original.
 * @param buffer Parâmetro buffer
 * @param size Parâmetro size
 */

    
    void toBinary(void* buffer, size_t size) const{
        if (size >= sizeof(Item)) {
            memcpy(buffer, this, sizeof(Item));
        }
    }
};
#pragma pack(pop)

// Verificação estática para garantir o tamanho correto da estrutura
static_assert(sizeof(Item) == 12, "Item structure size must be exactly 12 bytes for compatibility");

// Versão atualizada do jogo (exemplo: 3.81 build 2570)
/**
 * CURRENT_VERSION
 * 
 * Implementa a funcionalidade CURRENT_VERSION conforme especificação original.
 * @param 3 Parâmetro 3
 * @param 81 Parâmetro 81
 * @param 2570 Parâmetro 2570
 * @return Retorna Version
 */

static const Version CURRENT_VERSION(3, 81, 2570);

} // namespace wyd

#endif // WYD_TYPES_H

} // namespace wydbr
