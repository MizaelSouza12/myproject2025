#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace wyd {
namespace database {
namespace schema {

namespace Event_ {
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
    struct Description {
        struct _column_t {
            using _value_type = sqlpp::varchar;
            struct _name_t {
                static constexpr const char* _get_name() { return "description"; }
                template<typename T>
                struct _member_t {
                    T description;
                    T& operator()() { return description; }
                    const T& operator()() const { return description; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::varchar;
        struct _name_t {
            static constexpr const char* _get_name() { return "description"; }
            template<typename T>
            struct _member_t {
                T description;
                T& operator()() { return description; }
                const T& operator()() const { return description; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };
    struct StartAt {
        struct _column_t {
            using _value_type = sqlpp::timestamp;
            struct _name_t {
                static constexpr const char* _get_name() { return "start_at"; }
                template<typename T>
                struct _member_t {
                    T start_at;
                    T& operator()() { return start_at; }
                    const T& operator()() const { return start_at; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::timestamp;
        struct _name_t {
            static constexpr const char* _get_name() { return "start_at"; }
            template<typename T>
            struct _member_t {
                T start_at;
                T& operator()() { return start_at; }
                const T& operator()() const { return start_at; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };
    struct EndAt {
        struct _column_t {
            using _value_type = sqlpp::timestamp;
            struct _name_t {
                static constexpr const char* _get_name() { return "end_at"; }
                template<typename T>
                struct _member_t {
                    T end_at;
                    T& operator()() { return end_at; }
                    const T& operator()() const { return end_at; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::timestamp;
        struct _name_t {
            static constexpr const char* _get_name() { return "end_at"; }
            template<typename T>
            struct _member_t {
                T end_at;
                T& operator()() { return end_at; }
                const T& operator()() const { return end_at; }
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

struct Event : sqlpp::table_t<Event,
                           Event_::Id,
                           Event_::Name,
                           Event_::Type,
                           Event_::Description,
                           Event_::StartAt,
                           Event_::EndAt,
                           Event_::Status,
                           Event_::CreatedAt,
                           Event_::UpdatedAt> {
    struct _name_t {
        static constexpr const char* _get_name() { return "events"; }
    };
    using _name = _name_t;
    using _value_type = sqlpp::no_value_t;
    using _column_tuple_t = std::tuple<Event_::Id,
                                     Event_::Name,
                                     Event_::Type,
                                     Event_::Description,
                                     Event_::StartAt,
                                     Event_::EndAt,
                                     Event_::Status,
                                     Event_::CreatedAt,
                                     Event_::UpdatedAt>;
    using _column_names_t = sqlpp::char_sequence<'i', 'd', ',', 'n', 'a', 'm', 'e', ',', 't', 'y', 'p', 'e', ',', 'd', 'e', 's', 'c', 'r', 'i', 'p', 't', 'i', 'o', 'n', ',', 's', 't', 'a', 'r', 't', '_', 'a', 't', ',', 'e', 'n', 'd', '_', 'a', 't', ',', 's', 't', 'a', 't', 'u', 's', ',', 'c', 'r', 'e', 'a', 't', 'e', 'd', '_', 'a', 't', ',', 'u', 'p', 'd', 'a', 't', 'e', 'd', '_', 'a', 't'>;
    using _column_names = _column_names_t;
    using _column_tuple = _column_tuple_t;
    using _value_tuple = sqlpp::no_value_t;
};

} // namespace schema
} // namespace database
} // namespace wyd 