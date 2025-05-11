#pragma once

// Inclui todos os schemas já criados
#include "Account.hpp"
#include "Character.hpp"
#include "Inventory.hpp"
#include "Item.hpp"
#include "NPC.hpp"
#include "Quest.hpp"
#include "Guild.hpp"
#include "ChatMessage.hpp"
#include "Event.hpp"

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace wyd {
namespace database {
namespace schema {

// Friend (Amizade)
namespace Friend_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct CharacterId { SQLPP_COLUMN(character_id, sqlpp::integer); };
    struct FriendId { SQLPP_COLUMN(friend_id, sqlpp::integer); };
    struct Status { SQLPP_COLUMN(status, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
    struct UpdatedAt { SQLPP_COLUMN(updated_at, sqlpp::timestamp); };
}
struct Friend : sqlpp::table_t<Friend,
    Friend_::Id, Friend_::CharacterId, Friend_::FriendId, Friend_::Status, Friend_::CreatedAt, Friend_::UpdatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "friends"; } };
    using _name = _name_t;
};

// Ranking
namespace Ranking_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct CharacterId { SQLPP_COLUMN(character_id, sqlpp::integer); };
    struct Rank { SQLPP_COLUMN(rank, sqlpp::integer); };
    struct Points { SQLPP_COLUMN(points, sqlpp::bigint); };
    struct Type { SQLPP_COLUMN(type, sqlpp::integer); };
    struct UpdatedAt { SQLPP_COLUMN(updated_at, sqlpp::timestamp); };
}
struct Ranking : sqlpp::table_t<Ranking,
    Ranking_::Id, Ranking_::CharacterId, Ranking_::Rank, Ranking_::Points, Ranking_::Type, Ranking_::UpdatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "rankings"; } };
    using _name = _name_t;
};

// Mail (Correio)
namespace Mail_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct SenderId { SQLPP_COLUMN(sender_id, sqlpp::integer); };
    struct ReceiverId { SQLPP_COLUMN(receiver_id, sqlpp::integer); };
    struct Subject { SQLPP_COLUMN(subject, sqlpp::varchar); };
    struct Message { SQLPP_COLUMN(message, sqlpp::varchar); };
    struct Status { SQLPP_COLUMN(status, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
    struct UpdatedAt { SQLPP_COLUMN(updated_at, sqlpp::timestamp); };
}
struct Mail : sqlpp::table_t<Mail,
    Mail_::Id, Mail_::SenderId, Mail_::ReceiverId, Mail_::Subject, Mail_::Message, Mail_::Status, Mail_::CreatedAt, Mail_::UpdatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "mails"; } };
    using _name = _name_t;
};

// Log
namespace Log_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct Type { SQLPP_COLUMN(type, sqlpp::integer); };
    struct Message { SQLPP_COLUMN(message, sqlpp::varchar); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Log : sqlpp::table_t<Log,
    Log_::Id, Log_::Type, Log_::Message, Log_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "logs"; } };
    using _name = _name_t;
};

// Config
namespace Config_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct Key { SQLPP_COLUMN(key, sqlpp::varchar); };
    struct Value { SQLPP_COLUMN(value, sqlpp::varchar); };
    struct UpdatedAt { SQLPP_COLUMN(updated_at, sqlpp::timestamp); };
}
struct Config : sqlpp::table_t<Config,
    Config_::Id, Config_::Key, Config_::Value, Config_::UpdatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "configs"; } };
    using _name = _name_t;
};

// Ban
namespace Ban_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct AccountId { SQLPP_COLUMN(account_id, sqlpp::integer); };
    struct CharacterId { SQLPP_COLUMN(character_id, sqlpp::integer); };
    struct Reason { SQLPP_COLUMN(reason, sqlpp::varchar); };
    struct ExpiresAt { SQLPP_COLUMN(expires_at, sqlpp::timestamp); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Ban : sqlpp::table_t<Ban,
    Ban_::Id, Ban_::AccountId, Ban_::CharacterId, Ban_::Reason, Ban_::ExpiresAt, Ban_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "bans"; } };
    using _name = _name_t;
};

// GuildMember
namespace GuildMember_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct GuildId { SQLPP_COLUMN(guild_id, sqlpp::integer); };
    struct CharacterId { SQLPP_COLUMN(character_id, sqlpp::integer); };
    struct Role { SQLPP_COLUMN(role, sqlpp::integer); };
    struct JoinedAt { SQLPP_COLUMN(joined_at, sqlpp::timestamp); };
}
struct GuildMember : sqlpp::table_t<GuildMember,
    GuildMember_::Id, GuildMember_::GuildId, GuildMember_::CharacterId, GuildMember_::Role, GuildMember_::JoinedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "guild_members"; } };
    using _name = _name_t;
};

// GuildWar
namespace GuildWar_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct GuildAId { SQLPP_COLUMN(guild_a_id, sqlpp::integer); };
    struct GuildBId { SQLPP_COLUMN(guild_b_id, sqlpp::integer); };
    struct Status { SQLPP_COLUMN(status, sqlpp::integer); };
    struct StartedAt { SQLPP_COLUMN(started_at, sqlpp::timestamp); };
    struct EndedAt { SQLPP_COLUMN(ended_at, sqlpp::timestamp); };
}
struct GuildWar : sqlpp::table_t<GuildWar,
    GuildWar_::Id, GuildWar_::GuildAId, GuildWar_::GuildBId, GuildWar_::Status, GuildWar_::StartedAt, GuildWar_::EndedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "guild_wars"; } };
    using _name = _name_t;
};

// Trade
namespace Trade_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct SenderId { SQLPP_COLUMN(sender_id, sqlpp::integer); };
    struct ReceiverId { SQLPP_COLUMN(receiver_id, sqlpp::integer); };
    struct Status { SQLPP_COLUMN(status, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Trade : sqlpp::table_t<Trade,
    Trade_::Id, Trade_::SenderId, Trade_::ReceiverId, Trade_::Status, Trade_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "trades"; } };
    using _name = _name_t;
};

// Auction
namespace Auction_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct SellerId { SQLPP_COLUMN(seller_id, sqlpp::integer); };
    struct ItemId { SQLPP_COLUMN(item_id, sqlpp::integer); };
    struct Price { SQLPP_COLUMN(price, sqlpp::bigint); };
    struct Status { SQLPP_COLUMN(status, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Auction : sqlpp::table_t<Auction,
    Auction_::Id, Auction_::SellerId, Auction_::ItemId, Auction_::Price, Auction_::Status, Auction_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "auctions"; } };
    using _name = _name_t;
};

// Party
namespace Party_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct LeaderId { SQLPP_COLUMN(leader_id, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Party : sqlpp::table_t<Party,
    Party_::Id, Party_::LeaderId, Party_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "parties"; } };
    using _name = _name_t;
};

// Skill
namespace Skill_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct Name { SQLPP_COLUMN(name, sqlpp::varchar); };
    struct Type { SQLPP_COLUMN(type, sqlpp::integer); };
    struct Power { SQLPP_COLUMN(power, sqlpp::integer); };
    struct Cooldown { SQLPP_COLUMN(cooldown, sqlpp::integer); };
    struct ManaCost { SQLPP_COLUMN(mana_cost, sqlpp::integer); };
    struct Description { SQLPP_COLUMN(description, sqlpp::varchar); };
}
struct Skill : sqlpp::table_t<Skill,
    Skill_::Id, Skill_::Name, Skill_::Type, Skill_::Power, Skill_::Cooldown, Skill_::ManaCost, Skill_::Description> {
    struct _name_t { static constexpr const char* _get_name() { return "skills"; } };
    using _name = _name_t;
};

// Achievement
namespace Achievement_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct Name { SQLPP_COLUMN(name, sqlpp::varchar); };
    struct Description { SQLPP_COLUMN(description, sqlpp::varchar); };
    struct Points { SQLPP_COLUMN(points, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Achievement : sqlpp::table_t<Achievement,
    Achievement_::Id, Achievement_::Name, Achievement_::Description, Achievement_::Points, Achievement_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "achievements"; } };
    using _name = _name_t;
};

// Storage
namespace Storage_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct CharacterId { SQLPP_COLUMN(character_id, sqlpp::integer); };
    struct ItemId { SQLPP_COLUMN(item_id, sqlpp::integer); };
    struct Slot { SQLPP_COLUMN(slot, sqlpp::integer); };
    struct Quantity { SQLPP_COLUMN(quantity, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Storage : sqlpp::table_t<Storage,
    Storage_::Id, Storage_::CharacterId, Storage_::ItemId, Storage_::Slot, Storage_::Quantity, Storage_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "storages"; } };
    using _name = _name_t;
};

// Warehouse
namespace Warehouse_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct AccountId { SQLPP_COLUMN(account_id, sqlpp::integer); };
    struct ItemId { SQLPP_COLUMN(item_id, sqlpp::integer); };
    struct Slot { SQLPP_COLUMN(slot, sqlpp::integer); };
    struct Quantity { SQLPP_COLUMN(quantity, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Warehouse : sqlpp::table_t<Warehouse,
    Warehouse_::Id, Warehouse_::AccountId, Warehouse_::ItemId, Warehouse_::Slot, Warehouse_::Quantity, Warehouse_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "warehouses"; } };
    using _name = _name_t;
};

// Pet
namespace Pet_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct CharacterId { SQLPP_COLUMN(character_id, sqlpp::integer); };
    struct Name { SQLPP_COLUMN(name, sqlpp::varchar); };
    struct Type { SQLPP_COLUMN(type, sqlpp::integer); };
    struct Level { SQLPP_COLUMN(level, sqlpp::integer); };
    struct Experience { SQLPP_COLUMN(experience, sqlpp::bigint); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Pet : sqlpp::table_t<Pet,
    Pet_::Id, Pet_::CharacterId, Pet_::Name, Pet_::Type, Pet_::Level, Pet_::Experience, Pet_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "pets"; } };
    using _name = _name_t;
};

// Mount
namespace Mount_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct CharacterId { SQLPP_COLUMN(character_id, sqlpp::integer); };
    struct Type { SQLPP_COLUMN(type, sqlpp::integer); };
    struct Level { SQLPP_COLUMN(level, sqlpp::integer); };
    struct Speed { SQLPP_COLUMN(speed, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Mount : sqlpp::table_t<Mount,
    Mount_::Id, Mount_::CharacterId, Mount_::Type, Mount_::Level, Mount_::Speed, Mount_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "mounts"; } };
    using _name = _name_t;
};

// Shop
namespace Shop_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct Name { SQLPP_COLUMN(name, sqlpp::varchar); };
    struct Type { SQLPP_COLUMN(type, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Shop : sqlpp::table_t<Shop,
    Shop_::Id, Shop_::Name, Shop_::Type, Shop_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "shops"; } };
    using _name = _name_t;
};

// Coupon
namespace Coupon_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct Code { SQLPP_COLUMN(code, sqlpp::varchar); };
    struct Reward { SQLPP_COLUMN(reward, sqlpp::varchar); };
    struct Status { SQLPP_COLUMN(status, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Coupon : sqlpp::table_t<Coupon,
    Coupon_::Id, Coupon_::Code, Coupon_::Reward, Coupon_::Status, Coupon_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "coupons"; } };
    using _name = _name_t;
};

// Ticket
namespace Ticket_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct AccountId { SQLPP_COLUMN(account_id, sqlpp::integer); };
    struct Subject { SQLPP_COLUMN(subject, sqlpp::varchar); };
    struct Message { SQLPP_COLUMN(message, sqlpp::varchar); };
    struct Status { SQLPP_COLUMN(status, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct Ticket : sqlpp::table_t<Ticket,
    Ticket_::Id, Ticket_::AccountId, Ticket_::Subject, Ticket_::Message, Ticket_::Status, Ticket_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "tickets"; } };
    using _name = _name_t;
};

// BlockList
namespace BlockList_ {
    struct Id { SQLPP_COLUMN(id, sqlpp::integer); };
    struct CharacterId { SQLPP_COLUMN(character_id, sqlpp::integer); };
    struct BlockedId { SQLPP_COLUMN(blocked_id, sqlpp::integer); };
    struct CreatedAt { SQLPP_COLUMN(created_at, sqlpp::timestamp); };
}
struct BlockList : sqlpp::table_t<BlockList,
    BlockList_::Id, BlockList_::CharacterId, BlockList_::BlockedId, BlockList_::CreatedAt> {
    struct _name_t { static constexpr const char* _get_name() { return "block_lists"; } };
    using _name = _name_t;
};

// Fim dos schemas

} // namespace schema
} // namespace database
} // namespace wyd 