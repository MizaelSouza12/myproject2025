#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace WYD {
    // Tipos base
    using BYTE = uint8_t;
    using WORD = uint16_t;
    using DWORD = uint32_t;
    using QWORD = uint64_t;
    using CHAR = char;
    using WCHAR = wchar_t;
    using BOOL = bool;
    using FLOAT = float;
    using DOUBLE = double;

    // Estruturas de dados do jogo
    struct Position {
        FLOAT x;
        FLOAT y;
        FLOAT z;
    };

    struct Quaternion {
        FLOAT x;
        FLOAT y;
        FLOAT z;
        FLOAT w;
    };

    struct Transform {
        Position position;
        Quaternion rotation;
        FLOAT scale;
    };

    // Enums do jogo
    enum class ClassType : BYTE {
        NONE = 0,
        WARRIOR = 1,
        MAGE = 2,
        ARCHER = 3,
        PRIEST = 4,
        ASSASSIN = 5,
        KNIGHT = 6,
        BERSERKER = 7,
        MECHANIC = 8,
        NECROMANCER = 9,
        MAX_CLASS
    };

    enum class ItemType : BYTE {
        NONE = 0,
        WEAPON = 1,
        ARMOR = 2,
        ACCESSORY = 3,
        CONSUMABLE = 4,
        MATERIAL = 5,
        QUEST = 6,
        MAX_ITEM_TYPE
    };

    enum class ItemGrade : BYTE {
        NORMAL = 0,
        MAGIC = 1,
        RARE = 2,
        UNIQUE = 3,
        LEGENDARY = 4,
        MYTHIC = 5,
        MAX_GRADE
    };

    // Estruturas de dados avançadas
    template<typename T>
    class WYDVector {
    private:
        std::vector<T> data;
        size_t capacity;
        size_t size;

    public:
        WYDVector(size_t initialCapacity = 0) : capacity(initialCapacity), size(0) {
            if (initialCapacity > 0) {
                data.reserve(initialCapacity);
            }
        }

        void push_back(const T& value) {
            if (size >= capacity) {
                capacity = capacity == 0 ? 1 : capacity * 2;
                data.reserve(capacity);
            }
            data.push_back(value);
            size++;
        }

        T& operator[](size_t index) {
            return data[index];
        }

        const T& operator[](size_t index) const {
            return data[index];
        }

        size_t getSize() const { return size; }
        size_t getCapacity() const { return capacity; }
    };

    // Smart pointers customizados
    template<typename T>
    using WYDUniquePtr = std::unique_ptr<T>;

    template<typename T>
    using WYDSharedPtr = std::shared_ptr<T>;

    template<typename T>
    using WYDWeakPtr = std::weak_ptr<T>;

    // Funções utilitárias
    template<typename T>
    inline T clamp(T value, T min, T max) {
        return value < min ? min : (value > max ? max : value);
    }

    // Estruturas de dados para networking
    struct PacketHeader {
        WORD size;
        WORD type;
        DWORD timestamp;
    };

    // Estruturas de dados para o sistema de combate
    struct CombatStats {
        DWORD hp;
        DWORD mp;
        DWORD stamina;
        DWORD attack;
        DWORD defense;
        DWORD magicAttack;
        DWORD magicDefense;
        DWORD criticalRate;
        DWORD criticalDamage;
        DWORD dodgeRate;
        DWORD blockRate;
    };

    // Estruturas de dados para o sistema de habilidades
    struct SkillData {
        DWORD id;
        std::string name;
        BYTE level;
        DWORD cooldown;
        DWORD manaCost;
        DWORD damage;
        DWORD range;
        DWORD targetType;
    };

    // Estruturas de dados para o sistema de itens
    struct ItemData {
        DWORD id;
        std::string name;
        ItemType type;
        ItemGrade grade;
        DWORD level;
        DWORD durability;
        DWORD maxDurability;
        CombatStats stats;
    };

    // Estruturas de dados para o sistema de personagem
    struct CharacterData {
        DWORD id;
        std::string name;
        ClassType classType;
        BYTE level;
        DWORD experience;
        DWORD gold;
        Position position;
        CombatStats stats;
        WYDVector<SkillData> skills;
        WYDVector<ItemData> inventory;
    };
} 