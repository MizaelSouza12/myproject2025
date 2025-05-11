#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace wyd {
namespace database {
namespace schema {

namespace Character_ {
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

    struct AccountId {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "account_id"; }
                template<typename T>
                struct _member_t {
                    T account_id;
                    T& operator()() { return account_id; }
                    const T& operator()() const { return account_id; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "account_id"; }
            template<typename T>
            struct _member_t {
                T account_id;
                T& operator()() { return account_id; }
                const T& operator()() const { return account_id; }
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

    struct Class {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "class"; }
                template<typename T>
                struct _member_t {
                    T class_;
                    T& operator()() { return class_; }
                    const T& operator()() const { return class_; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "class"; }
            template<typename T>
            struct _member_t {
                T class_;
                T& operator()() { return class_; }
                const T& operator()() const { return class_; }
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

    struct Mana {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "mana"; }
                template<typename T>
                struct _member_t {
                    T mana;
                    T& operator()() { return mana; }
                    const T& operator()() const { return mana; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "mana"; }
            template<typename T>
            struct _member_t {
                T mana;
                T& operator()() { return mana; }
                const T& operator()() const { return mana; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct PositionX {
        struct _column_t {
            using _value_type = sqlpp::float_;
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
        using _value_type = sqlpp::float_;
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
            using _value_type = sqlpp::float_;
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
        using _value_type = sqlpp::float_;
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

    struct LastLogin {
        struct _column_t {
            using _value_type = sqlpp::timestamp;
            struct _name_t {
                static constexpr const char* _get_name() { return "last_login"; }
                template<typename T>
                struct _member_t {
                    T last_login;
                    T& operator()() { return last_login; }
                    const T& operator()() const { return last_login; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::timestamp;
        struct _name_t {
            static constexpr const char* _get_name() { return "last_login"; }
            template<typename T>
            struct _member_t {
                T last_login;
                T& operator()() { return last_login; }
                const T& operator()() const { return last_login; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Status {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "status"; }
                template<typename T>
                struct _member_t {
                    T status;
                    T& operator()() { return status; }
                    const T& operator()() const { return status; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "status"; }
            template<typename T>
            struct _member_t {
                T status;
                T& operator()() { return status; }
                const T& operator()() const { return status; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };
}

struct Character : sqlpp::table_t<Character,
                                Character_::Id,
                                Character_::AccountId,
                                Character_::Name,
                                Character_::Class,
                                Character_::Level,
                                Character_::Experience,
                                Character_::Strength,
                                Character_::Dexterity,
                                Character_::Intelligence,
                                Character_::Vitality,
                                Character_::Health,
                                Character_::Mana,
                                Character_::PositionX,
                                Character_::PositionY,
                                Character_::MapId,
                                Character_::CreatedAt,
                                Character_::LastLogin,
                                Character_::Status> {
    struct _name_t {
        static constexpr const char* _get_name() { return "characters"; }
    };
    using _name = _name_t;
    using _value_type = sqlpp::no_value_t;
    using _column_tuple_t = std::tuple<Character_::Id,
                                      Character_::AccountId,
                                      Character_::Name,
                                      Character_::Class,
                                      Character_::Level,
                                      Character_::Experience,
                                      Character_::Strength,
                                      Character_::Dexterity,
                                      Character_::Intelligence,
                                      Character_::Vitality,
                                      Character_::Health,
                                      Character_::Mana,
                                      Character_::PositionX,
                                      Character_::PositionY,
                                      Character_::MapId,
                                      Character_::CreatedAt,
                                      Character_::LastLogin,
                                      Character_::Status>;
    using _column_names_t = sqlpp::char_sequence<'i', 'd', ',', 'a', 'c', 'c', 'o', 'u', 'n', 't', '_', 'i', 'd', ',', 'n', 'a', 'm', 'e', ',', 'c', 'l', 'a', 's', 's', ',', 'l', 'e', 'v', 'e', 'l', ',', 'e', 'x', 'p', 'e', 'r', 'i', 'e', 'n', 'c', 'e', ',', 's', 't', 'r', 'e', 'n', 'g', 't', 'h', ',', 'd', 'e', 'x', 't', 'e', 'r', 'i', 't', 'y', ',', 'i', 'n', 't', 'e', 'l', 'l', 'i', 'g', 'e', 'n', 'c', 'e', ',', 'v', 'i', 't', 'a', 'l', 'i', 't', 'y', ',', 'h', 'e', 'a', 'l', 't', 'h', ',', 'm', 'a', 'n', 'a', ',', 'p', 'o', 's', 'i', 't', 'i', 'o', 'n', '_', 'x', ',', 'p', 'o', 's', 'i', 't', 'i', 'o', 'n', '_', 'y', ',', 'm', 'a', 'p', '_', 'i', 'd', ',', 'c', 'r', 'e', 'a', 't', 'e', 'd', '_', 'a', 't', ',', 'l', 'a', 's', 't', '_', 'l', 'o', 'g', 'i', 'n', ',', 's', 't', 'a', 't', 'u', 's'>;
    using _column_names = _column_names_t;
    using _column_tuple = _column_tuple_t;
    using _value_tuple = sqlpp::no_value_t;
};

} // namespace schema
} // namespace database
} // namespace wyd 