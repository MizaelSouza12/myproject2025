#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace wyd {
namespace database {
namespace schema {

namespace Account_ {
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

    struct Username {
        struct _column_t {
            using _value_type = sqlpp::varchar;
            struct _name_t {
                static constexpr const char* _get_name() { return "username"; }
                template<typename T>
                struct _member_t {
                    T username;
                    T& operator()() { return username; }
                    const T& operator()() const { return username; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::varchar;
        struct _name_t {
            static constexpr const char* _get_name() { return "username"; }
            template<typename T>
            struct _member_t {
                T username;
                T& operator()() { return username; }
                const T& operator()() const { return username; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Password {
        struct _column_t {
            using _value_type = sqlpp::varchar;
            struct _name_t {
                static constexpr const char* _get_name() { return "password"; }
                template<typename T>
                struct _member_t {
                    T password;
                    T& operator()() { return password; }
                    const T& operator()() const { return password; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::varchar;
        struct _name_t {
            static constexpr const char* _get_name() { return "password"; }
            template<typename T>
            struct _member_t {
                T password;
                T& operator()() { return password; }
                const T& operator()() const { return password; }
            };
        };
        using _name = _name_t;
        template<typename T>
        using _member_t = _name::_member_t<T>;
    };

    struct Email {
        struct _column_t {
            using _value_type = sqlpp::varchar;
            struct _name_t {
                static constexpr const char* _get_name() { return "email"; }
                template<typename T>
                struct _member_t {
                    T email;
                    T& operator()() { return email; }
                    const T& operator()() const { return email; }
                };
            };
            using _name = _name_t;
            template<typename T>
            using _member_t = _name::_member_t<T>;
        };
        using _value_type = sqlpp::varchar;
        struct _name_t {
            static constexpr const char* _get_name() { return "email"; }
            template<typename T>
            struct _member_t {
                T email;
                T& operator()() { return email; }
                const T& operator()() const { return email; }
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

struct Account : sqlpp::table_t<Account,
                               Account_::Id,
                               Account_::Username,
                               Account_::Password,
                               Account_::Email,
                               Account_::CreatedAt,
                               Account_::LastLogin,
                               Account_::Status> {
    struct _name_t {
        static constexpr const char* _get_name() { return "accounts"; }
    };
    using _name = _name_t;
    using _value_type = sqlpp::no_value_t;
    using _column_tuple_t = std::tuple<Account_::Id,
                                     Account_::Username,
                                     Account_::Password,
                                     Account_::Email,
                                     Account_::CreatedAt,
                                     Account_::LastLogin,
                                     Account_::Status>;
    using _column_names_t = sqlpp::char_sequence<'i', 'd', ',', 'u', 's', 'e', 'r', 'n', 'a', 'm', 'e', ',', 'p', 'a', 's', 's', 'w', 'o', 'r', 'd', ',', 'e', 'm', 'a', 'i', 'l', ',', 'c', 'r', 'e', 'a', 't', 'e', 'd', '_', 'a', 't', ',', 'l', 'a', 's', 't', '_', 'l', 'o', 'g', 'i', 'n', ',', 's', 't', 'a', 't', 'u', 's'>;
    using _column_names = _column_names_t;
    using _column_tuple = _column_tuple_t;
    using _value_tuple = sqlpp::no_value_t;
    using _name = _name_t;
    using _value_type = sqlpp::no_value_t;
    using _column_tuple_t = std::tuple<Account_::Id,
                                     Account_::Username,
                                     Account_::Password,
                                     Account_::Email,
                                     Account_::CreatedAt,
                                     Account_::LastLogin,
                                     Account_::Status>;
    using _column_names_t = sqlpp::char_sequence<'i', 'd', ',', 'u', 's', 'e', 'r', 'n', 'a', 'm', 'e', ',', 'p', 'a', 's', 's', 'w', 'o', 'r', 'd', ',', 'e', 'm', 'a', 'i', 'l', ',', 'c', 'r', 'e', 'a', 't', 'e', 'd', '_', 'a', 't', ',', 'l', 'a', 's', 't', '_', 'l', 'o', 'g', 'i', 'n', ',', 's', 't', 'a', 't', 'u', 's'>;
    using _column_names = _column_names_t;
    using _column_tuple = _column_tuple_t;
    using _value_tuple = sqlpp::no_value_t;
};

} // namespace schema
} // namespace database
} // namespace wyd 