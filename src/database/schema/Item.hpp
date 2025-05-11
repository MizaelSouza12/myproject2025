#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace wyd {
namespace database {
namespace schema {

namespace Item_ {
    struct Id {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "id"; }
                template<typename T>
                struct _member_t {
                    T id;
                    T& operator()() { return id; }
                    const T& operator()() const { return id; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "id"; }
            template<typename T>
            struct _member_t {
                T id;
                T& operator()() { return id; }
                const T& operator()() const { return id; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Name {
        struct _column_t {
            using _value_type = sqlpp::varchar;
            struct _name_t {
                static constexpr const char* _get_name() { return "name"; }
                template<typename T>
                struct _member_t {
                    T name;
                    T& operator()() { return name; }
                    const T& operator()() const { return name; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::varchar;
        struct _name_t {
            static constexpr const char* _get_name() { return "name"; }
            template<typename T>
            struct _member_t {
                T name;
                T& operator()() { return name; }
                const T& operator()() const { return name; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Type {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "type"; }
                template<typename T>
                struct _member_t {
                    T type;
                    T& operator()() { return type; }
                    const T& operator()() const { return type; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "type"; }
            template<typename T>
            struct _member_t {
                T type;
                T& operator()() { return type; }
                const T& operator()() const { return type; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct SubType {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "sub_type"; }
                template<typename T>
                struct _member_t {
                    T sub_type;
                    T& operator()() { return sub_type; }
                    const T& operator()() const { return sub_type; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "sub_type"; }
            template<typename T>
            struct _member_t {
                T sub_type;
                T& operator()() { return sub_type; }
                const T& operator()() const { return sub_type; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Level {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "level"; }
                template<typename T>
                struct _member_t {
                    T level;
                    T& operator()() { return level; }
                    const T& operator()() const { return level; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "level"; }
            template<typename T>
            struct _member_t {
                T level;
                T& operator()() { return level; }
                const T& operator()() const { return level; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct RequiredLevel {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "required_level"; }
                template<typename T>
                struct _member_t {
                    T required_level;
                    T& operator()() { return required_level; }
                    const T& operator()() const { return required_level; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "required_level"; }
            template<typename T>
            struct _member_t {
                T required_level;
                T& operator()() { return required_level; }
                const T& operator()() const { return required_level; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct RequiredClass {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "required_class"; }
                template<typename T>
                struct _member_t {
                    T required_class;
                    T& operator()() { return required_class; }
                    const T& operator()() const { return required_class; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "required_class"; }
            template<typename T>
            struct _member_t {
                T required_class;
                T& operator()() { return required_class; }
                const T& operator()() const { return required_class; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Price {
        struct _column_t {
            using _value_type = sqlpp::bigint;
            struct _name_t {
                static constexpr const char* _get_name() { return "price"; }
                template<typename T>
                struct _member_t {
                    T price;
                    T& operator()() { return price; }
                    const T& operator()() const { return price; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::bigint;
        struct _name_t {
            static constexpr const char* _get_name() { return "price"; }
            template<typename T>
            struct _member_t {
                T price;
                T& operator()() { return price; }
                const T& operator()() const { return price; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Weight {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "weight"; }
                template<typename T>
                struct _member_t {
                    T weight;
                    T& operator()() { return weight; }
                    const T& operator()() const { return weight; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "weight"; }
            template<typename T>
            struct _member_t {
                T weight;
                T& operator()() { return weight; }
                const T& operator()() const { return weight; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Durability {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "durability"; }
                template<typename T>
                struct _member_t {
                    T durability;
                    T& operator()() { return durability; }
                    const T& operator()() const { return durability; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "durability"; }
            template<typename T>
            struct _member_t {
                T durability;
                T& operator()() { return durability; }
                const T& operator()() const { return durability; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct MaxStack {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "max_stack"; }
                template<typename T>
                struct _member_t {
                    T max_stack;
                    T& operator()() { return max_stack; }
                    const T& operator()() const { return max_stack; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "max_stack"; }
            template<typename T>
            struct _member_t {
                T max_stack;
                T& operator()() { return max_stack; }
                const T& operator()() const { return max_stack; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct MaxSocket {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "max_socket"; }
                template<typename T>
                struct _member_t {
                    T max_socket;
                    T& operator()() { return max_socket; }
                    const T& operator()() const { return max_socket; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "max_socket"; }
            template<typename T>
            struct _member_t {
                T max_socket;
                T& operator()() { return max_socket; }
                const T& operator()() const { return max_socket; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct MaxRefine {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "max_refine"; }
                template<typename T>
                struct _member_t {
                    T max_refine;
                    T& operator()() { return max_refine; }
                    const T& operator()() const { return max_refine; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "max_refine"; }
            template<typename T>
            struct _member_t {
                T max_refine;
                T& operator()() { return max_refine; }
                const T& operator()() const { return max_refine; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Strength {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "strength"; }
                template<typename T>
                struct _member_t {
                    T strength;
                    T& operator()() { return strength; }
                    const T& operator()() const { return strength; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "strength"; }
            template<typename T>
            struct _member_t {
                T strength;
                T& operator()() { return strength; }
                const T& operator()() const { return strength; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Dexterity {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "dexterity"; }
                template<typename T>
                struct _member_t {
                    T dexterity;
                    T& operator()() { return dexterity; }
                    const T& operator()() const { return dexterity; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "dexterity"; }
            template<typename T>
            struct _member_t {
                T dexterity;
                T& operator()() { return dexterity; }
                const T& operator()() const { return dexterity; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Intelligence {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "intelligence"; }
                template<typename T>
                struct _member_t {
                    T intelligence;
                    T& operator()() { return intelligence; }
                    const T& operator()() const { return intelligence; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "intelligence"; }
            template<typename T>
            struct _member_t {
                T intelligence;
                T& operator()() { return intelligence; }
                const T& operator()() const { return intelligence; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Vitality {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "vitality"; }
                template<typename T>
                struct _member_t {
                    T vitality;
                    T& operator()() { return vitality; }
                    const T& operator()() const { return vitality; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "vitality"; }
            template<typename T>
            struct _member_t {
                T vitality;
                T& operator()() { return vitality; }
                const T& operator()() const { return vitality; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct MinDamage {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "min_damage"; }
                template<typename T>
                struct _member_t {
                    T min_damage;
                    T& operator()() { return min_damage; }
                    const T& operator()() const { return min_damage; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "min_damage"; }
            template<typename T>
            struct _member_t {
                T min_damage;
                T& operator()() { return min_damage; }
                const T& operator()() const { return min_damage; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct MaxDamage {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "max_damage"; }
                template<typename T>
                struct _member_t {
                    T max_damage;
                    T& operator()() { return max_damage; }
                    const T& operator()() const { return max_damage; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "max_damage"; }
            template<typename T>
            struct _member_t {
                T max_damage;
                T& operator()() { return max_damage; }
                const T& operator()() const { return max_damage; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Defense {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "defense"; }
                template<typename T>
                struct _member_t {
                    T defense;
                    T& operator()() { return defense; }
                    const T& operator()() const { return defense; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "defense"; }
            template<typename T>
            struct _member_t {
                T defense;
                T& operator()() { return defense; }
                const T& operator()() const { return defense; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct MagicDefense {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "magic_defense"; }
                template<typename T>
                struct _member_t {
                    T magic_defense;
                    T& operator()() { return magic_defense; }
                    const T& operator()() const { return magic_defense; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "magic_defense"; }
            template<typename T>
            struct _member_t {
                T magic_defense;
                T& operator()() { return magic_defense; }
                const T& operator()() const { return magic_defense; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct CreatedAt {
        struct _column_t {
            using _value_type = sqlpp::timestamp;
            struct _name_t {
                static constexpr const char* _get_name() { return "created_at"; }
                template<typename T>
                struct _member_t {
                    T created_at;
                    T& operator()() { return created_at; }
                    const T& operator()() const { return created_at; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::timestamp;
        struct _name_t {
            static constexpr const char* _get_name() { return "created_at"; }
            template<typename T>
            struct _member_t {
                T created_at;
                T& operator()() { return created_at; }
                const T& operator()() const { return created_at; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct UpdatedAt {
        struct _column_t {
            using _value_type = sqlpp::timestamp;
            struct _name_t {
                static constexpr const char* _get_name() { return "updated_at"; }
                template<typename T>
                struct _member_t {
                    T updated_at;
                    T& operator()() { return updated_at; }
                    const T& operator()() const { return updated_at; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::timestamp;
        struct _name_t {
            static constexpr const char* _get_name() { return "updated_at"; }
            template<typename T>
            struct _member_t {
                T updated_at;
                T& operator()() { return updated_at; }
                const T& operator()() const { return updated_at; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };
}

struct Item : sqlpp::table_t<Item,
                           Item_::Id,
                           Item_::Name,
                           Item_::Type,
                           Item_::SubType,
                           Item_::Level,
                           Item_::RequiredLevel,
                           Item_::RequiredClass,
                           Item_::Price,
                           Item_::Weight,
                           Item_::Durability,
                           Item_::MaxStack,
                           Item_::MaxSocket,
                           Item_::MaxRefine,
                           Item_::Strength,
                           Item_::Dexterity,
                           Item_::Intelligence,
                           Item_::Vitality,
                           Item_::MinDamage,
                           Item_::MaxDamage,
                           Item_::Defense,
                           Item_::MagicDefense,
                           Item_::CreatedAt,
                           Item_::UpdatedAt> {
    struct _name_t {
        static constexpr const char* _get_name() { return "items"; }
    };
    using _name = _name_t;
    using _value_type = sqlpp::no_value_t;
    using _column_tuple_t = std::tuple<Item_::Id,
                                     Item_::Name,
                                     Item_::Type,
                                     Item_::SubType,
                                     Item_::Level,
                                     Item_::RequiredLevel,
                                     Item_::RequiredClass,
                                     Item_::Price,
                                     Item_::Weight,
                                     Item_::Durability,
                                     Item_::MaxStack,
                                     Item_::MaxSocket,
                                     Item_::MaxRefine,
                                     Item_::Strength,
                                     Item_::Dexterity,
                                     Item_::Intelligence,
                                     Item_::Vitality,
                                     Item_::MinDamage,
                                     Item_::MaxDamage,
                                     Item_::Defense,
                                     Item_::MagicDefense,
                                     Item_::CreatedAt,
                                     Item_::UpdatedAt>;
    using _column_names_t = sqlpp::char_sequence<'i', 'd', ',', 'n', 'a', 'm', 'e', ',', 't', 'y', 'p', 'e', ',', 's', 'u', 'b', '_', 't', 'y', 'p', 'e', ',', 'l', 'e', 'v', 'e', 'l', ',', 'r', 'e', 'q', 'u', 'i', 'r', 'e', 'd', '_', 'l', 'e', 'v', 'e', 'l', ',', 'r', 'e', 'q', 'u', 'i', 'r', 'e', 'd', '_', 'c', 'l', 'a', 's', 's', ',', 'p', 'r', 'i', 'c', 'e', ',', 'w', 'e', 'i', 'g', 'h', 't', ',', 'd', 'u', 'r', 'a', 'b', 'i', 'l', 'i', 't', 'y', ',', 'm', 'a', 'x', '_', 's', 't', 'a', 'c', 'k', ',', 'm', 'a', 'x', '_', 's', 'o', 'c', 'k', 'e', 't', ',', 'm', 'a', 'x', '_', 'r', 'e', 'f', 'i', 'n', 'e', ',', 's', 't', 'r', 'e', 'n', 'g', 't', 'h', ',', 'd', 'e', 'x', 't', 'e', 'r', 'i', 't', 'y', ',', 'i', 'n', 't', 'e', 'l', 'l', 'i', 'g', 'e', 'n', 'c', 'e', ',', 'v', 'i', 't', 'a', 'l', 'i', 't', 'y', ',', 'm', 'i', 'n', '_', 'd', 'a', 'm', 'a', 'g', 'e', ',', 'm', 'a', 'x', '_', 'd', 'a', 'm', 'a', 'g', 'e', ',', 'd', 'e', 'f', 'e', 'n', 's', 'e', ',', 'm', 'a', 'g', 'i', 'c', '_', 'd', 'e', 'f', 'e', 'n', 's', 'e', ',', 'c', 'r', 'e', 'a', 't', 'e', 'd', '_', 'a', 't', ',', 'u', 'p', 'd', 'a', 't', 'e', 'd', '_', 'a', 't'>;
    using _column_names = _column_names_t;
    using _column_tuple = _column_tuple_t;
    using _value_tuple = sqlpp::no_value_t;
};

} // namespace schema
} // namespace database
} // namespace wyd 