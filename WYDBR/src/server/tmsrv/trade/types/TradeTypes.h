/**
 * TradeTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/trade/types/TradeTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef TRADETYPES_H
#define TRADETYPES_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <memory>
#include <set>
#include <bitset>
#include <array>
#include <variant>
#include <optional>
#include <ctime>

#include "../../../../core/WYDTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file TradeTypes.h
 * @brief Definições de tipos para o sistema avançado de comércio e economia
 * 
 * Este arquivo contém as definições de tipos para o sistema avançado de comércio,
 * incluindo mercados, leilões, comércio entre jogadores, vendedores NPC, sistema
 * monetário, controle de inflação, e economia dinâmica.
 */

namespace wyd {
namespace server {
namespace trade {

// Constantes do sistema de comércio
constexpr size_t MAX_MARKET_LISTINGS = 10000;     // Máximo de listagens no mercado
constexpr size_t MAX_AUCTION_ITEMS = 5000;        // Máximo de itens em leilão
constexpr size_t MAX_TRADE_ITEMS = 16;            // Máximo de itens por troca
constexpr size_t MAX_NPC_SHOP_ITEMS = 100;        // Máximo de itens por loja NPC
constexpr size_t MAX_CURRENCY_TYPES = 20;         // Máximo de tipos de moeda
constexpr size_t MAX_PRICE_HISTORY = 30;          // Máximo de histórico de preços
constexpr size_t MAX_TRADER_LEVELS = 10;          // Máximo de níveis de comerciante
constexpr size_t MAX_TRADE_CATEGORIES = 20;       // Máximo de categorias de comércio
constexpr size_t MAX_TRADE_ROUTES = 50;           // Máximo de rotas de comércio
constexpr size_t MAX_ECONOMIC_EVENTS = 30;        // Máximo de eventos econômicos
constexpr size_t MAX_BLACK_MARKET_ITEMS = 50;     // Máximo de itens no mercado negro
constexpr size_t MAX_TRANSACTION_TYPES = 25;      // Máximo de tipos de transação
constexpr size_t MAX_ECONOMY_FACTIONS = 10;       // Máximo de facções econômicas
constexpr size_t MAX_ECONOMY_POLICIES = 15;       // Máximo de políticas econômicas

/**
 * @brief Tipo de listagem de mercado
 */
enum class MarketListingType : BYTE {
    SELL = 0,                  // Venda
    BUY = 1,                   // Compra
    EXCHANGE = 2,              // Troca
    RENT = 3,                  // Aluguel
    SERVICE = 4,               // Serviço
    CRAFTING = 5,              // Criação
    CUSTOM = 6,                // Personalizado
};

/**
 * @brief Status de listagem de mercado
 */
enum class MarketListingStatus : BYTE {
    ACTIVE = 0,                // Ativa
    EXPIRED = 1,               // Expirada
    COMPLETED = 2,             // Completada
    CANCELLED = 3,             // Cancelada
    PENDING = 4,               // Pendente
    HIDDEN = 5,                // Oculta
    FEATURED = 6,              // Destaque
    CUSTOM = 7,                // Personalizado
};

/**
 * @brief Tipo de leilão
 */
enum class AuctionType : BYTE {
    STANDARD = 0,              // Padrão
    REVERSE = 1,               // Reverso
    SEALED = 2,                // Lacrado
    DUTCH = 3,                 // Holandês
    TIMED = 4,                 // Cronometrado
    GUILD_ONLY = 5,            // Somente guilda
    FACTION_ONLY = 6,          // Somente facção
    VIP_ONLY = 7,              // Somente VIP
    CUSTOM = 8,                // Personalizado
};

/**
 * @brief Status de leilão
 */
enum class AuctionStatus : BYTE {
    PENDING = 0,               // Pendente
    ACTIVE = 1,                // Ativo
    ENDED = 2,                 // Encerrado
    CANCELLED = 3,             // Cancelado
    PROCESSING = 4,            // Processando
    FAILED = 5,                // Falhou
    CUSTOM = 6,                // Personalizado
};

/**
 * @brief Tipo de moeda
 */
enum class CurrencyType : BYTE {
    GOLD = 0,                  // Ouro
    SILVER = 1,                // Prata
    COPPER = 2,                // Cobre
    PREMIUM = 3,               // Premium
    FACTION = 4,               // Facção
    GUILD = 5,                 // Guilda
    EVENT = 6,                 // Evento
    REPUTATION = 7,            // Reputação
    BARTER = 8,                // Escambo
    BLACK_MARKET = 9,          // Mercado negro
    CUSTOM = 10,               // Personalizado
};

/**
 * @brief Tipo de transação
 */
enum class TransactionType : BYTE {
    MARKET_PURCHASE = 0,       // Compra no mercado
    MARKET_SALE = 1,           // Venda no mercado
    PLAYER_TRADE = 2,          // Troca entre jogadores
    NPC_PURCHASE = 3,          // Compra de NPC
    NPC_SALE = 4,              // Venda para NPC
    AUCTION_BID = 5,           // Lance em leilão
    AUCTION_SALE = 6,          // Venda em leilão
    MAIL_TRANSFER = 7,         // Transferência por correio
    GUILD_BANK = 8,            // Banco de guilda
    REPAIR_COST = 9,           // Custo de reparo
    SKILL_COST = 10,           // Custo de habilidade
    TRANSPORT_COST = 11,       // Custo de transporte
    TAX_PAYMENT = 12,          // Pagamento de imposto
    QUEST_REWARD = 13,         // Recompensa de quest
    CUSTOM = 14,               // Personalizado
};

/**
 * @brief Status de transação
 */
enum class TransactionStatus : BYTE {
    PENDING = 0,               // Pendente
    COMPLETED = 1,             // Completada
    FAILED = 2,                // Falhou
    CANCELLED = 3,             // Cancelada
    REFUNDED = 4,              // Reembolsada
    DISPUTED = 5,              // Contestada
    PROCESSING = 6,            // Processando
    CUSTOM = 7,                // Personalizado
};

/**
 * @brief Categoria de item de troca
 */
enum class TradeCategoryType : BYTE {
    WEAPON = 0,                // Arma
    ARMOR = 1,                 // Armadura
    ACCESSORY = 2,             // Acessório
    POTION = 3,                // Poção
    MATERIAL = 4,              // Material
    QUEST = 5,                 // Quest
    RARE = 6,                  // Raro
    UNIQUE = 7,                // Único
    CONSUMABLE = 8,            // Consumível
    CRAFT_INGREDIENT = 9,      // Ingrediente de criação
    COLLECTIBLE = 10,          // Colecionável
    UTILITY = 11,              // Utilidade
    CURRENCY = 12,             // Moeda
    SERVICE = 13,              // Serviço
    CUSTOM = 14,               // Personalizado
};

/**
 * @brief Tipo de rota de comércio
 */
enum class TradeRouteType : BYTE {
    LAND = 0,                  // Terra
    SEA = 1,                   // Mar
    AIR = 2,                   // Ar
    PORTAL = 3,                // Portal
    UNDERGROUND = 4,           // Subterrânea
    MIXED = 5,                 // Mista
    CUSTOM = 6,                // Personalizada
};

/**
 * @brief Status de rota de comércio
 */
enum class TradeRouteStatus : BYTE {
    OPEN = 0,                  // Aberta
    CLOSED = 1,                // Fechada
    RESTRICTED = 2,            // Restrita
    DANGEROUS = 3,             // Perigosa
    CONTESTED = 4,             // Contestada
    TAXED = 5,                 // Taxada
    CUSTOM = 6,                // Personalizada
};

/**
 * @brief Tipo de evento econômico
 */
enum class EconomicEventType : BYTE {
    BOOM = 0,                  // Boom
    RECESSION = 1,             // Recessão
    INFLATION = 2,             // Inflação
    DEFLATION = 3,             // Deflação
    SCARCITY = 4,              // Escassez
    SURPLUS = 5,               // Excedente
    WAR_IMPACT = 6,            // Impacto de guerra
    NATURAL_DISASTER = 7,      // Desastre natural
    TAXATION_CHANGE = 8,       // Mudança de imposto
    TRADE_AGREEMENT = 9,       // Acordo comercial
    EMBARGO = 10,              // Embargo
    FESTIVAL = 11,             // Festival
    CUSTOM = 12,               // Personalizado
};

/**
 * @brief Tipo de regulação de mercado
 */
enum class MarketRegulationType : BYTE {
    PRICE_FLOOR = 0,           // Piso de preço
    PRICE_CEILING = 1,         // Teto de preço
    TAX_RATE = 2,              // Taxa de imposto
    LISTING_LIMIT = 3,         // Limite de listagem
    ITEM_RESTRICTION = 4,      // Restrição de item
    TRADE_QUOTA = 5,           // Cota de comércio
    TRANSACTION_FEE = 6,       // Taxa de transação
    MARKET_ACCESS = 7,         // Acesso ao mercado
    CUSTOM = 8,                // Personalizado
};

/**
 * @brief Nível de restrição comercial
 */
enum class TradingRestrictionLevel : BYTE {
    NONE = 0,                  // Nenhum
    LOW = 1,                   // Baixo
    MEDIUM = 2,                // Médio
    HIGH = 3,                  // Alto
    TOTAL = 4,                 // Total
    FACTION_BASED = 5,         // Baseado em facção
    REPUTATION_BASED = 6,      // Baseado em reputação
    CUSTOM = 7,                // Personalizado
};

/**
 * @brief Listagem de mercado
 */
struct MarketListing {
    DWORD listingID;            // ID da listagem
    DWORD sellerID;             // ID do vendedor
    std::string sellerName;      // Nome do vendedor
    DWORD itemID;               // ID do item
    BYTE itemQuality;           // Qualidade do item
    DWORD quantity;             // Quantidade
    DWORD unitPrice;            // Preço unitário
    CurrencyType currencyType;  // Tipo de moeda
    time_t listingTime;          // Hora da listagem
    time_t expiryTime;           // Hora de expiração
    MarketListingType type;      // Tipo
    MarketListingStatus status;  // Status
    TradeCategoryType category;  // Categoria
    std::string description;     // Descrição
    bool negotiable;             // Negociável
    DWORD guildID;              // ID da guilda
    DWORD factionID;            // ID da facção
    BYTE minimumLevel;          // Nível mínimo
    
    MarketListing()
        : listingID(0)
        , sellerID(0)
        , sellerName("")
        , itemID(0)
        , itemQuality(0)
        , quantity(0)
        , unitPrice(0)
        , currencyType(CurrencyType::GOLD)
        , listingTime(0)
        , expiryTime(0)
        , type(MarketListingType::SELL)
        , status(MarketListingStatus::ACTIVE)
        , category(TradeCategoryType::MATERIAL)
        , description("")
        , negotiable(false)
        , guildID(0)
        , factionID(0)
        , minimumLevel(0)
    {
    }
};

/**
 * @brief Item de leilão
 */
struct AuctionItem {
    DWORD auctionID;            // ID do leilão
    DWORD sellerID;             // ID do vendedor
    std::string sellerName;      // Nome do vendedor
    DWORD itemID;               // ID do item
    BYTE itemQuality;           // Qualidade do item
    DWORD quantity;             // Quantidade
    DWORD startingBid;          // Lance inicial
    DWORD currentBid;           // Lance atual
    DWORD buyoutPrice;          // Preço de compra
    DWORD highestBidderID;      // ID do maior licitante
    std::string highestBidderName; // Nome do maior licitante
    time_t startTime;            // Hora de início
    time_t endTime;              // Hora de término
    DWORD minimumBidIncrement;  // Incremento mínimo de lance
    AuctionType type;            // Tipo
    AuctionStatus status;        // Status
    TradeCategoryType category;  // Categoria
    DWORD bidCount;             // Contagem de lances
    CurrencyType currencyType;  // Tipo de moeda
    std::string description;     // Descrição
    
    AuctionItem()
        : auctionID(0)
        , sellerID(0)
        , sellerName("")
        , itemID(0)
        , itemQuality(0)
        , quantity(0)
        , startingBid(0)
        , currentBid(0)
        , buyoutPrice(0)
        , highestBidderID(0)
        , highestBidderName("")
        , startTime(0)
        , endTime(0)
        , minimumBidIncrement(0)
        , type(AuctionType::STANDARD)
        , status(AuctionStatus::PENDING)
        , category(TradeCategoryType::MATERIAL)
        , bidCount(0)
        , currencyType(CurrencyType::GOLD)
        , description("")
    {
    }
};

/**
 * @brief Lance em leilão
 */
struct AuctionBid {
    DWORD bidID;                // ID do lance
    DWORD auctionID;            // ID do leilão
    DWORD bidderID;             // ID do licitante
    std::string bidderName;      // Nome do licitante
    DWORD bidAmount;            // Valor do lance
    time_t bidTime;              // Hora do lance
    bool outbid;                 // Superado
    bool autoRebid;              // Auto-rebid
    DWORD maxAutoBid;           // Lance automático máximo
    
    AuctionBid()
        : bidID(0)
        , auctionID(0)
        , bidderID(0)
        , bidderName("")
        , bidAmount(0)
        , bidTime(0)
        , outbid(false)
        , autoRebid(false)
        , maxAutoBid(0)
    {
    }
};

/**
 * @brief Item de troca entre jogadores
 */
struct PlayerTradeItem {
    DWORD tradeID;              // ID da troca
    DWORD playerID;             // ID do jogador
    DWORD itemID;               // ID do item
    BYTE itemQuality;           // Qualidade do item
    DWORD quantity;             // Quantidade
    BYTE slot;                  // Slot
    bool confirmed;              // Confirmado
    
    PlayerTradeItem()
        : tradeID(0)
        , playerID(0)
        , itemID(0)
        , itemQuality(0)
        , quantity(0)
        , slot(0)
        , confirmed(false)
    {
    }
};

/**
 * @brief Troca entre jogadores
 */
struct PlayerTrade {
    DWORD tradeID;              // ID da troca
    DWORD player1ID;            // ID do jogador 1
    DWORD player2ID;            // ID do jogador 2
    DWORD player1Gold;          // Ouro do jogador 1
    DWORD player2Gold;          // Ouro do jogador 2
    bool player1Confirmed;       // Jogador 1 confirmou
    bool player2Confirmed;       // Jogador 2 confirmou
    bool player1Offered;         // Jogador 1 ofereceu
    bool player2Offered;         // Jogador 2 ofereceu
    time_t startTime;            // Hora de início
    time_t completionTime;       // Hora de conclusão
    TransactionStatus status;    // Status
    std::string cancelReason;    // Razão de cancelamento
    
    PlayerTrade()
        : tradeID(0)
        , player1ID(0)
        , player2ID(0)
        , player1Gold(0)
        , player2Gold(0)
        , player1Confirmed(false)
        , player2Confirmed(false)
        , player1Offered(false)
        , player2Offered(false)
        , startTime(0)
        , completionTime(0)
        , status(TransactionStatus::PENDING)
        , cancelReason("")
    {
    }
};

/**
 * @brief Item de loja NPC
 */
struct NPCShopItem {
    DWORD shopItemID;           // ID do item da loja
    DWORD shopID;               // ID da loja
    DWORD itemID;               // ID do item
    BYTE itemQuality;           // Qualidade do item
    DWORD baseBuyPrice;         // Preço base de compra
    DWORD baseSellPrice;        // Preço base de venda
    DWORD stock;                // Estoque
    DWORD maxStock;             // Estoque máximo
    time_t restockTime;          // Hora de reabastecimento
    bool unlimited;              // Ilimitado
    TradeCategoryType category;  // Categoria
    DWORD requiredFactionID;    // ID da facção necessária
    BYTE requiredRank;          // Rank necessário
    DWORD requiredItemID;       // ID do item necessário
    bool seasonal;               // Sazonal
    
    NPCShopItem()
        : shopItemID(0)
        , shopID(0)
        , itemID(0)
        , itemQuality(0)
        , baseBuyPrice(0)
        , baseSellPrice(0)
        , stock(0)
        , maxStock(0)
        , restockTime(0)
        , unlimited(false)
        , category(TradeCategoryType::MATERIAL)
        , requiredFactionID(0)
        , requiredRank(0)
        , requiredItemID(0)
        , seasonal(false)
    {
    }
};

/**
 * @brief Loja NPC
 */
struct NPCShop {
    DWORD shopID;               // ID da loja
    DWORD npcID;                // ID do NPC
    std::string name;            // Nome
    TradingRestrictionLevel restrictions; // Restrições
    float buyMultiplier;         // Multiplicador de compra
    float sellMultiplier;        // Multiplicador de venda
    DWORD factionID;            // ID da facção
    bool blackMarket;            // Mercado negro
    time_t openTime;             // Hora de abertura
    time_t closeTime;            // Hora de fechamento
    DWORD zoneID;               // ID da zona
    bool mobile;                 // Móvel
    
    NPCShop()
        : shopID(0)
        , npcID(0)
        , name("")
        , restrictions(TradingRestrictionLevel::NONE)
        , buyMultiplier(1.0f)
        , sellMultiplier(0.5f)
        , factionID(0)
        , blackMarket(false)
        , openTime(0)
        , closeTime(0)
        , zoneID(0)
        , mobile(false)
    {
    }
};

/**
 * @brief Transação
 */
struct Transaction {
    DWORD transactionID;        // ID da transação
    TransactionType type;        // Tipo
    DWORD buyerID;              // ID do comprador
    DWORD sellerID;             // ID do vendedor
    DWORD itemID;               // ID do item
    BYTE itemQuality;           // Qualidade do item
    DWORD quantity;             // Quantidade
    DWORD amount;               // Valor
    CurrencyType currencyType;  // Tipo de moeda
    time_t timestamp;            // Timestamp
    TransactionStatus status;    // Status
    DWORD referenceID;          // ID de referência
    std::string notes;           // Notas
    DWORD feeAmount;            // Valor da taxa
    
    Transaction()
        : transactionID(0)
        , type(TransactionType::PLAYER_TRADE)
        , buyerID(0)
        , sellerID(0)
        , itemID(0)
        , itemQuality(0)
        , quantity(0)
        , amount(0)
        , currencyType(CurrencyType::GOLD)
        , timestamp(0)
        , status(TransactionStatus::PENDING)
        , referenceID(0)
        , notes("")
        , feeAmount(0)
    {
    }
};

/**
 * @brief Histórico de preço de item
 */
struct ItemPriceHistory {
    DWORD historyID;            // ID do histórico
    DWORD itemID;               // ID do item
    BYTE itemQuality;           // Qualidade do item
    DWORD averagePrice;         // Preço médio
    DWORD lowestPrice;          // Preço mais baixo
    DWORD highestPrice;         // Preço mais alto
    DWORD volume;               // Volume
    DWORD weekday;              // Dia da semana
    DWORD hour;                 // Hora
    CurrencyType currencyType;  // Tipo de moeda
    
    ItemPriceHistory()
        : historyID(0)
        , itemID(0)
        , itemQuality(0)
        , averagePrice(0)
        , lowestPrice(0)
        , highestPrice(0)
        , volume(0)
        , weekday(0)
        , hour(0)
        , currencyType(CurrencyType::GOLD)
    {
    }
};

/**
 * @brief Rota de comércio
 */
struct TradeRoute {
    DWORD routeID;              // ID da rota
    TradeRouteType type;         // Tipo
    DWORD startZoneID;          // ID da zona inicial
    DWORD endZoneID;            // ID da zona final
    DWORD length;               // Comprimento
    DWORD travelTime;           // Tempo de viagem
    TradeRouteStatus status;     // Status
    float riskFactor;            // Fator de risco
    float taxRate;               // Taxa de imposto
    DWORD controllingFactionID; // ID da facção controladora
    std::string name;            // Nome
    
    TradeRoute()
        : routeID(0)
        , type(TradeRouteType::LAND)
        , startZoneID(0)
        , endZoneID(0)
        , length(0)
        , travelTime(0)
        , status(TradeRouteStatus::OPEN)
        , riskFactor(0.0f)
        , taxRate(0.0f)
        , controllingFactionID(0)
        , name("")
    {
    }
};

/**
 * @brief Evento econômico
 */
struct EconomicEvent {
    DWORD eventID;              // ID do evento
    EconomicEventType type;      // Tipo
    std::string name;            // Nome
    std::string description;     // Descrição
    time_t startTime;            // Hora de início
    time_t endTime;              // Hora de término
    float impactFactor;          // Fator de impacto
    std::vector<std::pair<TradeCategoryType, float>> categoryImpacts; // Impactos por categoria
    std::vector<std::pair<DWORD, float>> itemImpacts; // Impactos por item
    DWORD zoneID;               // ID da zona
    DWORD factionID;            // ID da facção
    bool global;                 // Global
    bool active;                 // Ativo
    
    EconomicEvent()
        : eventID(0)
        , type(EconomicEventType::INFLATION)
        , name("")
        , description("")
        , startTime(0)
        , endTime(0)
        , impactFactor(0.0f)
        , zoneID(0)
        , factionID(0)
        , global(false)
        , active(false)
    {
    }
};

/**
 * @brief Regulação de mercado
 */
struct MarketRegulation {
    DWORD regulationID;         // ID da regulação
    MarketRegulationType type;   // Tipo
    DWORD itemID;               // ID do item
    TradeCategoryType category;  // Categoria
    float value;                 // Valor
    time_t startTime;            // Hora de início
    time_t endTime;              // Hora de término
    DWORD factionID;            // ID da facção
    DWORD zoneID;               // ID da zona
    std::string description;     // Descrição
    bool active;                 // Ativa
    
    MarketRegulation()
        : regulationID(0)
        , type(MarketRegulationType::TAX_RATE)
        , itemID(0)
        , category(TradeCategoryType::MATERIAL)
        , value(0.0f)
        , startTime(0)
        , endTime(0)
        , factionID(0)
        , zoneID(0)
        , description("")
        , active(true)
    {
    }
};

/**
 * @brief Sistema econômico
 */
struct EconomicSystem {
    DWORD economyID;            // ID da economia
    std::string name;            // Nome
    float inflationRate;         // Taxa de inflação
    float economicHealth;        // Saúde econômica
    DWORD totalGoldInCirculation; // Total de ouro em circulação
    DWORD dailyGoldCreated;     // Ouro criado diariamente
    DWORD dailyGoldDestroyed;   // Ouro destruído diariamente
    float averagePlayerWealth;   // Riqueza média do jogador
    float giniCoefficient;       // Coeficiente de Gini
    DWORD activeTransactions;   // Transações ativas
    float transactionVolume;     // Volume de transações
    float taxRevenue;            // Receita de impostos
    
    EconomicSystem()
        : economyID(0)
        , name("")
        , inflationRate(0.0f)
        , economicHealth(0.0f)
        , totalGoldInCirculation(0)
        , dailyGoldCreated(0)
        , dailyGoldDestroyed(0)
        , averagePlayerWealth(0.0f)
        , giniCoefficient(0.0f)
        , activeTransactions(0)
        , transactionVolume(0.0f)
        , taxRevenue(0.0f)
    {
    }
};

/**
 * @brief Facção de comércio
 */
struct TradeFaction {
    DWORD factionID;            // ID da facção
    std::string name;            // Nome
    float taxRate;               // Taxa de imposto
    TradingRestrictionLevel externalTradeRestrictions; // Restrições de comércio externo
    std::vector<std::pair<DWORD, TradingRestrictionLevel>> relationRestrictions; // Restrições de relação
    std::vector<std::pair<TradeCategoryType, float>> specializations; // Especializações
    float marketShare;           // Participação no mercado
    DWORD reputation;           // Reputação
    bool playerManaged;          // Gerenciada por jogador
    
    TradeFaction()
        : factionID(0)
        , name("")
        , taxRate(0.0f)
        , externalTradeRestrictions(TradingRestrictionLevel::NONE)
        , marketShare(0.0f)
        , reputation(0)
        , playerManaged(false)
    {
    }
};

/**
 * @brief Tipos de dados serializáveis para o sistema de comércio
 */
using TradeDataVariant = std::variant<
    MarketListing,
    AuctionItem,
    AuctionBid,
    PlayerTrade,
    PlayerTradeItem,
    NPCShop,
    NPCShopItem,
    Transaction,
    ItemPriceHistory,
    TradeRoute,
    EconomicEvent,
    MarketRegulation,
    EconomicSystem,
    TradeFaction
>;

/**
 * @brief Callback de evento de transação
 */
using TransactionEventCallback = std::function<void(DWORD, TransactionType, DWORD, DWORD, TransactionStatus)>;

/**
 * @brief Callback de evento de mercado
 */
using MarketEventCallback = std::function<void(DWORD, MarketListingType, DWORD, DWORD, MarketListingStatus)>;

/**
 * @brief Callback de evento de leilão
 */
using AuctionEventCallback = std::function<void(DWORD, AuctionType, DWORD, DWORD, AuctionStatus)>;

/**
 * @brief Callback de evento econômico
 */
using EconomicEventCallback = std::function<void(DWORD, EconomicEventType, time_t, time_t, float)>;

} // namespace trade
} // namespace server
} // namespace wyd

#endif // TRADETYPES_H

} // namespace wydbr
