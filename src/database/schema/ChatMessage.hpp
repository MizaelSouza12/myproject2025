#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace wyd {
namespace database {
namespace schema {

namespace ChatMessage_ {
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
    struct SenderId {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "sender_id"; }
                template<typename T>
                struct _member_t {
                    T sender_id;
                    T& operator()() { return sender_id; }
                    const T& operator()() const { return sender_id; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "sender_id"; }
            template<typename T>
            struct _member_t {
                T sender_id;
                T& operator()() { return sender_id; }
                const T& operator()() const { return sender_id; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };
    struct ReceiverId {
        struct _column_t {
            using _value_type = sqlpp::integer;
            struct _name_t {
                static constexpr const char* _get_name() { return "receiver_id"; }
                template<typename T>
                struct _member_t {
                    T receiver_id;
                    T& operator()() { return receiver_id; }
                    const T& operator()() const { return receiver_id; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::integer;
        struct _name_t {
            static constexpr const char* _get_name() { return "receiver_id"; }
            template<typename T>
            struct _member_t {
                T receiver_id;
                T& operator()() { return receiver_id; }
                const T& operator()() const { return receiver_id; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };
    struct Channel {
        struct _column_t {
            using _value_type = sqlpp::varchar;
            struct _name_t {
                static constexpr const char* _get_name() { return "channel"; }
                template<typename T>
                struct _member_t {
                    T channel;
                    T& operator()() { return channel; }
                    const T& operator()() const { return channel; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::varchar;
        struct _name_t {
            static constexpr const char* _get_name() { return "channel"; }
            template<typename T>
            struct _member_t {
                T channel;
                T& operator()() { return channel; }
                const T& operator()() const { return channel; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };
    struct Message {
        struct _column_t {
            using _value_type = sqlpp::varchar;
            struct _name_t {
                static constexpr const char* _get_name() { return "message"; }
                template<typename T>
                struct _member_t {
                    T message;
                    T& operator()() { return message; }
                    const T& operator()() const { return message; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::varchar;
        struct _name_t {
            static constexpr const char* _get_name() { return "message"; }
            template<typename T>
            struct _member_t {
                T message;
                T& operator()() { return message; }
                const T& operator()() const { return message; }
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

struct ChatMessage : sqlpp::table_t<ChatMessage,
                                 ChatMessage_::Id,
                                 ChatMessage_::SenderId,
                                 ChatMessage_::ReceiverId,
                                 ChatMessage_::Channel,
                                 ChatMessage_::Message,
                                 ChatMessage_::Type,
                                 ChatMessage_::CreatedAt,
                                 ChatMessage_::UpdatedAt> {
    struct _name_t {
        static constexpr const char* _get_name() { return "chat_messages"; }
    };
    using _name = _name_t;
    using _value_type = sqlpp::no_value_t;
    using _column_tuple_t = std::tuple<ChatMessage_::Id,
                                     ChatMessage_::SenderId,
                                     ChatMessage_::ReceiverId,
                                     ChatMessage_::Channel,
                                     ChatMessage_::Message,
                                     ChatMessage_::Type,
                                     ChatMessage_::CreatedAt,
                                     ChatMessage_::UpdatedAt>;
    using _column_names_t = sqlpp::char_sequence<'i', 'd', ',', 's', 'e', 'n', 'd', 'e', 'r', '_', 'i', 'd', ',', 'r', 'e', 'c', 'e', 'i', 'v', 'e', 'r', '_', 'i', 'd', ',', 'c', 'h', 'a', 'n', 'n', 'e', 'l', ',', 'm', 'e', 's', 's', 'a', 'g', 'e', ',', 't', 'y', 'p', 'e', ',', 'c', 'r', 'e', 'a', 't', 'e', 'd', '_', 'a', 't', ',', 'u', 'p', 'd', 'a', 't', 'e', 'd', '_', 'a', 't'>;
    using _column_names = _column_names_t;
    using _column_tuple = _column_tuple_t;
    using _value_tuple = sqlpp::no_value_t;
};

} // namespace schema
} // namespace database
} // namespace wyd 