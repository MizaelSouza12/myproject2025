#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace wyd {
namespace database {
namespace schema {

namespace Inventory_ {
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

    struct CharacterId {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "character_id"; }
                template<typename T>
                struct _member_t {
                    T character_id;
                    T& operator()() { return character_id; }
                    const T& operator()() const { return character_id; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "character_id"; }
            template<typename T>
            struct _member_t {
                T character_id;
                T& operator()() { return character_id; }
                const T& operator()() const { return character_id; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct ItemId {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "item_id"; }
                template<typename T>
                struct _member_t {
                    T item_id;
                    T& operator()() { return item_id; }
                    const T& operator()() const { return item_id; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "item_id"; }
            template<typename T>
            struct _member_t {
                T item_id;
                T& operator()() { return item_id; }
                const T& operator()() const { return item_id; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Slot {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "slot"; }
                template<typename T>
                struct _member_t {
                    T slot;
                    T& operator()() { return slot; }
                    const T& operator()() const { return slot; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "slot"; }
            template<typename T>
            struct _member_t {
                T slot;
                T& operator()() { return slot; }
                const T& operator()() const { return slot; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Quantity {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "quantity"; }
                template<typename T>
                struct _member_t {
                    T quantity;
                    T& operator()() { return quantity; }
                    const T& operator()() const { return quantity; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "quantity"; }
            template<typename T>
            struct _member_t {
                T quantity;
                T& operator()() { return quantity; }
                const T& operator()() const { return quantity; }
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

    struct MaxDurability {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "max_durability"; }
                template<typename T>
                struct _member_t {
                    T max_durability;
                    T& operator()() { return max_durability; }
                    const T& operator()() const { return max_durability; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "max_durability"; }
            template<typename T>
            struct _member_t {
                T max_durability;
                T& operator()() { return max_durability; }
                const T& operator()() const { return max_durability; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct RefineLevel {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "refine_level"; }
                template<typename T>
                struct _member_t {
                    T refine_level;
                    T& operator()() { return refine_level; }
                    const T& operator()() const { return refine_level; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "refine_level"; }
            template<typename T>
            struct _member_t {
                T refine_level;
                T& operator()() { return refine_level; }
                const T& operator()() const { return refine_level; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct SocketCount {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "socket_count"; }
                template<typename T>
                struct _member_t {
                    T socket_count;
                    T& operator()() { return socket_count; }
                    const T& operator()() const { return socket_count; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "socket_count"; }
            template<typename T>
            struct _member_t {
                T socket_count;
                T& operator()() { return socket_count; }
                const T& operator()() const { return socket_count; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Socket1 {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "socket1"; }
                template<typename T>
                struct _member_t {
                    T socket1;
                    T& operator()() { return socket1; }
                    const T& operator()() const { return socket1; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "socket1"; }
            template<typename T>
            struct _member_t {
                T socket1;
                T& operator()() { return socket1; }
                const T& operator()() const { return socket1; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Socket2 {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "socket2"; }
                template<typename T>
                struct _member_t {
                    T socket2;
                    T& operator()() { return socket2; }
                    const T& operator()() const { return socket2; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "socket2"; }
            template<typename T>
            struct _member_t {
                T socket2;
                T& operator()() { return socket2; }
                const T& operator()() const { return socket2; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Socket3 {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "socket3"; }
                template<typename T>
                struct _member_t {
                    T socket3;
                    T& operator()() { return socket3; }
                    const T& operator()() const { return socket3; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "socket3"; }
            template<typename T>
            struct _member_t {
                T socket3;
                T& operator()() { return socket3; }
                const T& operator()() const { return socket3; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Socket4 {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "socket4"; }
                template<typename T>
                struct _member_t {
                    T socket4;
                    T& operator()() { return socket4; }
                    const T& operator()() const { return socket4; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "socket4"; }
            template<typename T>
            struct _member_t {
                T socket4;
                T& operator()() { return socket4; }
                const T& operator()() const { return socket4; }
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

struct Inventory : sqlpp::table_t<Inventory,
                                Inventory_::Id,
                                Inventory_::CharacterId,
                                Inventory_::ItemId,
                                Inventory_::Slot,
                                Inventory_::Quantity,
                                Inventory_::Durability,
                                Inventory_::MaxDurability,
                                Inventory_::RefineLevel,
                                Inventory_::SocketCount,
                                Inventory_::Socket1,
                                Inventory_::Socket2,
                                Inventory_::Socket3,
                                Inventory_::Socket4,
                                Inventory_::CreatedAt,
                                Inventory_::UpdatedAt> {
    struct _name_t {
        static constexpr const char* _get_name() { return "inventory"; }
    };
    using _name = _name_t;
    using _value_type = sqlpp::no_value_t;
    using _column_tuple_t = std::tuple<Inventory_::Id,
                                     Inventory_::CharacterId,
                                     Inventory_::ItemId,
                                     Inventory_::Slot,
                                     Inventory_::Quantity,
                                     Inventory_::Durability,
                                     Inventory_::MaxDurability,
                                     Inventory_::RefineLevel,
                                     Inventory_::SocketCount,
                                     Inventory_::Socket1,
                                     Inventory_::Socket2,
                                     Inventory_::Socket3,
                                     Inventory_::Socket4,
                                     Inventory_::CreatedAt,
                                     Inventory_::UpdatedAt>;
    using _column_names_t = sqlpp::char_sequence<'i', 'd', ',', 'c', 'h', 'a', 'r', 'a', 'c', 't', 'e', 'r', '_', 'i', 'd', ',', 'i', 't', 'e', 'm', '_', 'i', 'd', ',', 's', 'l', 'o', 't', ',', 'q', 'u', 'a', 'n', 't', 'i', 't', 'y', ',', 'd', 'u', 'r', 'a', 'b', 'i', 'l', 'i', 't', 'y', ',', 'm', 'a', 'x', '_', 'd', 'u', 'r', 'a', 'b', 'i', 'l', 'i', 't', 'y', ',', 'r', 'e', 'f', 'i', 'n', 'e', '_', 'l', 'e', 'v', 'e', 'l', ',', 's', 'o', 'c', 'k', 'e', 't', '_', 'c', 'o', 'u', 'n', 't', ',', 's', 'o', 'c', 'k', 'e', 't', '1', ',', 's', 'o', 'c', 'k', 'e', 't', '2', ',', 's', 'o', 'c', 'k', 'e', 't', '3', ',', 's', 'o', 'c', 'k', 'e', 't', '4', ',', 'c', 'r', 'e', 'a', 't', 'e', 'd', '_', 'a', 't', ',', 'u', 'p', 'd', 'a', 't', 'e', 'd', '_', 'a', 't'>;
    using _column_names = _column_names_t;
    using _column_tuple = _column_tuple_t;
    using _value_tuple = sqlpp::no_value_t;
};

} // namespace schema
} // namespace database
} // namespace wyd 