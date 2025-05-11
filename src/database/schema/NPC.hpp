#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace wyd {
namespace database {
namespace schema {

namespace NPC_ {
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

    struct MapId {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "map_id"; }
                template<typename T>
                struct _member_t {
                    T map_id;
                    T& operator()() { return map_id; }
                    const T& operator()() const { return map_id; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "map_id"; }
            template<typename T>
            struct _member_t {
                T map_id;
                T& operator()() { return map_id; }
                const T& operator()() const { return map_id; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct PositionX {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "position_x"; }
                template<typename T>
                struct _member_t {
                    T position_x;
                    T& operator()() { return position_x; }
                    const T& operator()() const { return position_x; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "position_x"; }
            template<typename T>
            struct _member_t {
                T position_x;
                T& operator()() { return position_x; }
                const T& operator()() const { return position_x; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct PositionY {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "position_y"; }
                template<typename T>
                struct _member_t {
                    T position_y;
                    T& operator()() { return position_y; }
                    const T& operator()() const { return position_y; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "position_y"; }
            template<typename T>
            struct _member_t {
                T position_y;
                T& operator()() { return position_y; }
                const T& operator()() const { return position_y; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Direction {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "direction"; }
                template<typename T>
                struct _member_t {
                    T direction;
                    T& operator()() { return direction; }
                    const T& operator()() const { return direction; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "direction"; }
            template<typename T>
            struct _member_t {
                T direction;
                T& operator()() { return direction; }
                const T& operator()() const { return direction; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Health {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "health"; }
                template<typename T>
                struct _member_t {
                    T health;
                    T& operator()() { return health; }
                    const T& operator()() const { return health; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "health"; }
            template<typename T>
            struct _member_t {
                T health;
                T& operator()() { return health; }
                const T& operator()() const { return health; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct MaxHealth {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "max_health"; }
                template<typename T>
                struct _member_t {
                    T max_health;
                    T& operator()() { return max_health; }
                    const T& operator()() const { return max_health; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "max_health"; }
            template<typename T>
            struct _member_t {
                T max_health;
                T& operator()() { return max_health; }
                const T& operator()() const { return max_health; }
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

    struct Experience {
        struct _column_t {
            using _value_type = sqlpp::bigint;
            struct _name_t {
                static constexpr const char* _get_name() { return "experience"; }
                template<typename T>
                struct _member_t {
                    T experience;
                    T& operator()() { return experience; }
                    const T& operator()() const { return experience; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::bigint;
        struct _name_t {
            static constexpr const char* _get_name() { return "experience"; }
            template<typename T>
            struct _member_t {
                T experience;
                T& operator()() { return experience; }
                const T& operator()() const { return experience; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Gold {
        struct _column_t {
            using _value_type = sqlpp::bigint;
            struct _name_t {
                static constexpr const char* _get_name() { return "gold"; }
                template<typename T>
                struct _member_t {
                    T gold;
                    T& operator()() { return gold; }
                    const T& operator()() const { return gold; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::bigint;
        struct _name_t {
            static constexpr const char* _get_name() { return "gold"; }
            template<typename T>
            struct _member_t {
                T gold;
                T& operator()() { return gold; }
                const T& operator()() const { return gold; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct RespawnTime {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "respawn_time"; }
                template<typename T>
                struct _member_t {
                    T respawn_time;
                    T& operator()() { return respawn_time; }
                    const T& operator()() const { return respawn_time; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "respawn_time"; }
            template<typename T>
            struct _member_t {
                T respawn_time;
                T& operator()() { return respawn_time; }
                const T& operator()() const { return respawn_time; }
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

struct NPC : sqlpp::table_t<NPC,
                          NPC_::Id,
                          NPC_::Name,
                          NPC_::Type,
                          NPC_::MapId,
                          NPC_::PositionX,
                          NPC_::PositionY,
                          NPC_::Direction,
                          NPC_::Health,
                          NPC_::MaxHealth,
                          NPC_::Level,
                          NPC_::MinDamage,
                          NPC_::MaxDamage,
                          NPC_::Defense,
                          NPC_::MagicDefense,
                          NPC_::Experience,
                          NPC_::Gold,
                          NPC_::RespawnTime,
                          NPC_::CreatedAt,
                          NPC_::UpdatedAt> {
    struct _name_t {
        static constexpr const char* _get_name() { return "npcs"; }
    };
    using _name = _name_t;
    using _value_type = sqlpp::no_value_t;
    using _column_tuple_t = std::tuple<NPC_::Id,
                                     NPC_::Name,
                                     NPC_::Type,
                                     NPC_::MapId,
                                     NPC_::PositionX,
                                     NPC_::PositionY,
                                     NPC_::Direction,
                                     NPC_::Health,
                                     NPC_::MaxHealth,
                                     NPC_::Level,
                                     NPC_::MinDamage,
                                     NPC_::MaxDamage,
                                     NPC_::Defense,
                                     NPC_::MagicDefense,
                                     NPC_::Experience,
                                     NPC_::Gold,
                                     NPC_::RespawnTime,
                                     NPC_::CreatedAt,
                                     NPC_::UpdatedAt>;
    using _column_names_t = sqlpp::char_sequence<'i', 'd', ',', 'n', 'a', 'm', 'e', ',', 't', 'y', 'p', 'e', ',', 'm', 'a', 'p', '_', 'i', 'd', ',', 'p', 'o', 's', 'i', 't', 'i', 'o', 'n', '_', 'x', ',', 'p', 'o', 's', 'i', 't', 'i', 'o', 'n', '_', 'y', ',', 'd', 'i', 'r', 'e', 'c', 't', 'i', 'o', 'n', ',', 'h', 'e', 'a', 'l', 't', 'h', ',', 'm', 'a', 'x', '_', 'h', 'e', 'a', 'l', 't', 'h', ',', 'l', 'e', 'v', 'e', 'l', ',', 'm', 'i', 'n', '_', 'd', 'a', 'm', 'a', 'g', 'e', ',', 'm', 'a', 'x', '_', 'd', 'a', 'm', 'a', 'g', 'e', ',', 'd', 'e', 'f', 'e', 'n', 's', 'e', ',', 'm', 'a', 'g', 'i', 'c', '_', 'd', 'e', 'f', 'e', 'n', 's', 'e', ',', 'e', 'x', 'p', 'e', 'r', 'i', 'e', 'n', 'c', 'e', ',', 'g', 'o', 'l', 'd', ',', 'r', 'e', 's', 'p', 'a', 'w', 'n', '_', 't', 'i', 'm', 'e', ',', 'c', 'r', 'e', 'a', 't', 'e', 'd', '_', 'a', 't', ',', 'u', 'p', 'd', 'a', 't', 'e', 'd', '_', 'a', 't'>;
    using _column_names = _column_names_t;
    using _column_tuple = _column_tuple_t;
    using _value_tuple = sqlpp::no_value_t;
};

} // namespace schema
} // namespace database
} // namespace wyd 