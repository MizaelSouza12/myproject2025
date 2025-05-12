/**
 * EconomyTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/economy/EconomyTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef ECONOMYTYPES_H
#define ECONOMYTYPES_H

#include <string>
#include <vector>
#include <map>
#include <array>
#include <bitset>
#include <chrono>
#include <ctime>
#include <functional>
#include <atomic>
#include <mutex>

#include "../../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file EconomyTypes.h
 * @brief Definições de tipos para o sistema econômico avançado
 * 
 * Este arquivo contém as definições de tipos para o sistema econômico avançado do WYD,
 * que corrige várias limitações e bugs do sistema original e implementa funcionalidades
 * avançadas para controle econômico e marketplace.
 */

namespace wyd {
namespace server {
namespace economy {

// Constantes de sistema
constexpr int MAX_MARKET_LISTINGS = 10000;          // Máximo de listagens no mercado
constexpr int MAX_MARKET_HISTORY_ENTRIES = 50000;   // Máximo de entradas no histórico do mercado
constexpr int MAX_AUCTION_LISTINGS = 5000;          // Máximo de listagens em leilão
constexpr int MAX_AUCTION_BIDS = 50;                // Máximo de lances por leilão
constexpr int MAX_MARKET_CATEGORIES = 50;           // Máximo de categorias de mercado
constexpr int MAX_PRICE_HISTORY_POINTS = 500;       // Máximo de pontos no histórico de preços
constexpr int MAX_TRANSACTION_LOG_ENTRIES = 100000; // Máximo de entradas no log de transações
constexpr int MAX_SEARCH_RESULTS = 200;             // Máximo de resultados de busca
constexpr int MAX_FEATURED_ITEMS = 10;              // Máximo de itens em destaque
constexpr int MAX_WATCHLIST_ITEMS = 50;             // Máximo de itens na lista de observação
constexpr int MAX_TAX_BRACKETS = 10;                // Máximo de faixas de imposto
constexpr int MAX_PRICE_CONTROL_RULES = 20;         // Máximo de regras de controle de preço
constexpr int MAX_ECONOMIC_INDICATORS = 30;         // Máximo de indicadores econômicos
constexpr int MAX_SERVER_CONNECTIONS = 100;         // Máximo de conexões entre servidores
constexpr int MAX_CURRENCY_TYPES = 10;              // Máximo de tipos de moeda

/**
 * @brief Tipos de listagem de mercado
 */
enum class MarketListingType : BYTE {
    DIRECT_SALE = 0,       // Venda direta
    AUCTION = 1,           // Leilão
    BID_REQUEST = 2,       // Requisição de compra
    BULK_SALE = 3,         // Venda em massa
    CONSIGNMENT = 4,       // Venda por consignação
    TRADE_OFFER = 5,       // Oferta de troca
    LIMITED_TIME = 6,      // Tempo limitado
    RESERVED = 7,          // Reservado para um jogador
    GLOBAL_LISTING = 8,    // Listagem global
    CUSTOM = 9,            // Personalizado
};

/**
 * @brief Status de listagem de mercado
 */
enum class MarketListingStatus : BYTE {
    ACTIVE = 0,            // Ativa
    SOLD = 1,              // Vendida
    EXPIRED = 2,           // Expirada
    CANCELLED = 3,         // Cancelada
    PENDING = 4,           // Pendente
    UNDER_REVIEW = 5,      // Em revisão
    RESERVED = 6,          // Reservada
    DISPUTED = 7,          // Disputada
    COMPLETED = 8,         // Completada
    FAILED = 9,            // Falhou
};

/**
 * @brief Categorias de mercado
 */
enum class MarketCategory : BYTE {
    WEAPONS = 0,           // Armas
    ARMOR = 1,             // Armaduras
    ACCESSORIES = 2,       // Acessórios
    CONSUMABLES = 3,       // Consumíveis
    MATERIALS = 4,         // Materiais
    QUEST_ITEMS = 5,       // Itens de quest
    RARE_ITEMS = 6,        // Itens raros
    EVENT_ITEMS = 7,       // Itens de evento
    SCROLLS = 8,           // Pergaminhos
    GEMS = 9,              // Gemas
    MOUNT_ITEMS = 10,      // Itens de montaria
    GUILD_ITEMS = 11,      // Itens de guild
    COSMETICS = 12,        // Cosméticos
    SPECIAL = 13,          // Especiais
    PREMIUM = 14,          // Premium
    OTHER = 15,            // Outros
};

/**
 * @brief Tipos de moeda
 */
enum class CurrencyType : BYTE {
    GOLD = 0,              // Ouro
    CREDIT = 1,            // Crédito
    HONOR_POINT = 2,       // Ponto de honra
    GUILD_POINT = 3,       // Ponto de guild
    EVENT_TOKEN = 4,       // Token de evento
    PREMIUM_POINT = 5,     // Ponto premium
    BATTLE_POINT = 6,      // Ponto de batalha
    DONATION_POINT = 7,    // Ponto de doação
    CUSTOM_1 = 8,          // Personalizado 1
    CUSTOM_2 = 9,          // Personalizado 2
};

/**
 * @brief Tipos de taxas de mercado
 */
enum class MarketTaxType : BYTE {
    LISTING_FEE = 0,       // Taxa de listagem
    SALE_TAX = 1,          // Imposto sobre vendas
    AUCTION_FEE = 2,       // Taxa de leilão
    PREMIUM_FEE = 3,       // Taxa premium
    TRANSFER_TAX = 4,      // Imposto de transferência
    CURRENCY_EXCHANGE = 5, // Taxa de câmbio
    BULK_TAX = 6,          // Imposto sobre massa
    SPECIAL_ITEM_TAX = 7,  // Imposto sobre item especial
    CUSTOM = 8,            // Personalizado
};

/**
 * @brief Tipos de eventos econômicos
 */
enum class EconomicEventType : BYTE {
    PRICE_CHANGE = 0,      // Mudança de preço
    MARKET_CRASH = 1,      // Quebra de mercado
    INFLATION = 2,         // Inflação
    DEFLATION = 3,         // Deflação
    CURRENCY_DEVALUATION = 4, // Desvalorização de moeda
    RESOURCE_SHORTAGE = 5, // Escassez de recurso
    OVERSUPPLY = 6,        // Excesso de oferta
    TAX_CHANGE = 7,        // Mudança de imposto
    TRADE_DISRUPTION = 8,  // Perturbação comercial
    SEASONAL_CHANGE = 9,   // Mudança sazonal
    EVENT_IMPACT = 10,     // Impacto de evento
    GUILD_WAR_IMPACT = 11, // Impacto de guerra de guild
    CUSTOM = 12,           // Personalizado
};

/**
 * @brief Tipos de indicadores econômicos
 */
enum class EconomicIndicatorType : BYTE {
    INFLATION_RATE = 0,    // Taxa de inflação
    TRADE_VOLUME = 1,      // Volume de comércio
    AVERAGE_PRICE = 2,     // Preço médio
    PRICE_VOLATILITY = 3,  // Volatilidade de preço
    GOLD_SINK_RATIO = 4,   // Relação de sumidouro de ouro
    GOLD_FAUCET_RATIO = 5, // Relação de fonte de ouro
    PLAYER_WEALTH = 6,     // Riqueza do jogador
    MARKET_LIQUIDITY = 7,  // Liquidez de mercado
    ITEM_RARITY = 8,       // Raridade de item
    DEMAND_PRESSURE = 9,   // Pressão de demanda
    SUPPLY_PRESSURE = 10,  // Pressão de oferta
    CUSTOM = 11,           // Personalizado
};

/**
 * @brief Status de sincronização entre servidores
 */
enum class CrossServerSyncStatus : BYTE {
    SYNCHRONIZED = 0,      // Sincronizado
    PENDING = 1,           // Pendente
    CONFLICT = 2,          // Conflito
    ERROR = 3,             // Erro
    OUTDATED = 4,          // Desatualizado
    PARTIAL = 5,           // Parcial
    DISABLED = 6,          // Desativado
    CUSTOM = 7,            // Personalizado
};

/**
 * @brief Tipos de intervenção econômica
 */
enum class EconomicInterventionType : BYTE {
    PRICE_FLOOR = 0,       // Piso de preço
    PRICE_CEILING = 1,     // Teto de preço
    ITEM_REMOVAL = 2,      // Remoção de item
    ITEM_INJECTION = 3,    // Injeção de item
    TAX_ADJUSTMENT = 4,    // Ajuste de imposto
    GOLD_SINK = 5,         // Sumidouro de ouro
    GOLD_INJECTION = 6,    // Injeção de ouro
    MARKET_RESTRICTION = 7, // Restrição de mercado
    INCENTIVE = 8,         // Incentivo
    CUSTOM = 9,            // Personalizado
};

/**
 * @brief Níveis de restrição de comércio
 */
enum class TradingRestrictionLevel : BYTE {
    NONE = 0,              // Nenhum
    LOW = 1,               // Baixo
    MEDIUM = 2,            // Médio
    HIGH = 3,              // Alto
    VERY_HIGH = 4,         // Muito alto
    COMPLETE = 5,          // Completo
    CUSTOM = 6,            // Personalizado
};

/**
 * @brief Status de transação
 */
enum class TransactionStatus : BYTE {
    PENDING = 0,           // Pendente
    COMPLETED = 1,         // Completada
    FAILED = 2,            // Falhou
    CANCELLED = 3,         // Cancelada
    DISPUTED = 4,          // Disputada
    REFUNDED = 5,          // Reembolsada
    PARTIAL = 6,           // Parcial
    CUSTOM = 7,            // Personalizado
};

/**
 * @brief Listagem de mercado
 */
struct MarketListing {
    DWORD listingID;                  // ID da listagem
    DWORD sellerID;                   // ID do vendedor
    std::string sellerName;            // Nome do vendedor
    MarketListingType type;           // Tipo
    MarketListingStatus status;       // Status
    MarketCategory category;          // Categoria
    WORD itemID;                      // ID do item
    WORD quantity;                    // Quantidade
    BYTE quality;                     // Qualidade
    BYTE enchantLevel;                // Nível de encantamento
    DWORD price;                      // Preço
    CurrencyType currencyType;        // Tipo de moeda
    time_t listingDate;                // Data de listagem
    time_t expiryDate;                 // Data de expiração
    DWORD buyerID;                    // ID do comprador
    std::string buyerName;             // Nome do comprador
    DWORD tags;                       // Tags
    std::string description;           // Descrição
    bool featured;                     // Em destaque
    BYTE attributeCount;              // Contagem de atributos
    std::array<std::pair<BYTE, WORD>, 8> attributes; // Atributos do item
    BYTE socketCount;                 // Contagem de soquetes
    std::array<WORD, 4> sockets;       // Soquetes
    
    MarketListing()
        : listingID(0)
        , sellerID(0)
        , sellerName("")
        , type(MarketListingType::DIRECT_SALE)
        , status(MarketListingStatus::ACTIVE)
        , category(MarketCategory::WEAPONS)
        , itemID(0)
        , quantity(1)
        , quality(0)
        , enchantLevel(0)
        , price(0)
        , currencyType(CurrencyType::GOLD)
        , listingDate(0)
        , expiryDate(0)
        , buyerID(0)
        , buyerName("")
        , tags(0)
        , description("")
        , featured(false)
        , attributeCount(0)
        , socketCount(0)
    {
        attributes.fill({0, 0});
        sockets.fill(0);
    }
};

/**
 * @brief Lance de leilão
 */
struct AuctionBid {
    DWORD bidID;                      // ID do lance
    DWORD auctionID;                  // ID do leilão
    DWORD bidderID;                   // ID do licitante
    std::string bidderName;            // Nome do licitante
    DWORD amount;                     // Valor
    time_t bidTime;                    // Hora do lance
    bool outbid;                       // Superado
    bool cancelled;                    // Cancelado
    
    AuctionBid()
        : bidID(0)
        , auctionID(0)
        , bidderID(0)
        , bidderName("")
        , amount(0)
        , bidTime(0)
        , outbid(false)
        , cancelled(false)
    {
    }
};

/**
 * @brief Leilão
 */
struct Auction {
    DWORD auctionID;                  // ID do leilão
    DWORD sellerID;                   // ID do vendedor
    std::string sellerName;            // Nome do vendedor
    WORD itemID;                      // ID do item
    WORD quantity;                    // Quantidade
    BYTE quality;                     // Qualidade
    BYTE enchantLevel;                // Nível de encantamento
    DWORD startingBid;                // Lance inicial
    DWORD currentBid;                 // Lance atual
    DWORD buyoutPrice;                // Preço de compra direta
    DWORD minBidIncrement;            // Incremento mínimo de lance
    DWORD currentHighBidderID;        // ID do maior licitante atual
    std::string currentHighBidderName; // Nome do maior licitante atual
    time_t startTime;                  // Hora de início
    time_t endTime;                    // Hora de término
    MarketListingStatus status;       // Status
    BYTE attributeCount;              // Contagem de atributos
    std::array<std::pair<BYTE, WORD>, 8> attributes; // Atributos do item
    BYTE socketCount;                 // Contagem de soquetes
    std::array<WORD, 4> sockets;       // Soquetes
    CurrencyType currencyType;        // Tipo de moeda
    std::vector<AuctionBid> bids;      // Lances
    
    Auction()
        : auctionID(0)
        , sellerID(0)
        , sellerName("")
        , itemID(0)
        , quantity(1)
        , quality(0)
        , enchantLevel(0)
        , startingBid(0)
        , currentBid(0)
        , buyoutPrice(0)
        , minBidIncrement(0)
        , currentHighBidderID(0)
        , currentHighBidderName("")
        , startTime(0)
        , endTime(0)
        , status(MarketListingStatus::ACTIVE)
        , attributeCount(0)
        , socketCount(0)
        , currencyType(CurrencyType::GOLD)
    {
        attributes.fill({0, 0});
        sockets.fill(0);
    }
};

/**
 * @brief Entrada no histórico de preços
 */
struct PriceHistoryEntry {
    DWORD entryID;                    // ID da entrada
    WORD itemID;                      // ID do item
    BYTE quality;                     // Qualidade
    BYTE enchantLevel;                // Nível de encantamento
    DWORD averagePrice;               // Preço médio
    DWORD lowestPrice;                // Preço mais baixo
    DWORD highestPrice;               // Preço mais alto
    DWORD volumeTraded;               // Volume negociado
    time_t timestamp;                  // Timestamp
    
    PriceHistoryEntry()
        : entryID(0)
        , itemID(0)
        , quality(0)
        , enchantLevel(0)
        , averagePrice(0)
        , lowestPrice(0)
        , highestPrice(0)
        , volumeTraded(0)
        , timestamp(0)
    {
    }
};

/**
 * @brief Transação de mercado
 */
struct MarketTransaction {
    DWORD transactionID;              // ID da transação
    DWORD listingID;                  // ID da listagem
    DWORD sellerID;                   // ID do vendedor
    std::string sellerName;            // Nome do vendedor
    DWORD buyerID;                    // ID do comprador
    std::string buyerName;             // Nome do comprador
    WORD itemID;                      // ID do item
    WORD quantity;                    // Quantidade
    BYTE quality;                     // Qualidade
    BYTE enchantLevel;                // Nível de encantamento
    DWORD price;                      // Preço
    DWORD tax;                        // Imposto
    time_t timestamp;                  // Timestamp
    TransactionStatus status;         // Status
    CurrencyType currencyType;        // Tipo de moeda
    
    MarketTransaction()
        : transactionID(0)
        , listingID(0)
        , sellerID(0)
        , sellerName("")
        , buyerID(0)
        , buyerName("")
        , itemID(0)
        , quantity(0)
        , quality(0)
        , enchantLevel(0)
        , price(0)
        , tax(0)
        , timestamp(0)
        , status(TransactionStatus::COMPLETED)
        , currencyType(CurrencyType::GOLD)
    {
    }
};

/**
 * @brief Faixa de imposto
 */
struct TaxBracket {
    DWORD bracketID;                  // ID da faixa
    DWORD minPrice;                   // Preço mínimo
    DWORD maxPrice;                   // Preço máximo
    float taxRate;                     // Taxa de imposto
    std::string description;           // Descrição
    
    TaxBracket()
        : bracketID(0)
        , minPrice(0)
        , maxPrice(0)
        , taxRate(0.0f)
        , description("")
    {
    }
};

/**
 * @brief Indicador econômico
 */
struct EconomicIndicator {
    DWORD indicatorID;                // ID do indicador
    EconomicIndicatorType type;       // Tipo
    float currentValue;                // Valor atual
    float previousValue;               // Valor anterior
    float changeRate;                  // Taxa de mudança
    time_t lastUpdateTime;             // Hora da última atualização
    std::string description;           // Descrição
    
    EconomicIndicator()
        : indicatorID(0)
        , type(EconomicIndicatorType::INFLATION_RATE)
        , currentValue(0.0f)
        , previousValue(0.0f)
        , changeRate(0.0f)
        , lastUpdateTime(0)
        , description("")
    {
    }
};

/**
 * @brief Evento econômico
 */
struct EconomicEvent {
    DWORD eventID;                    // ID do evento
    EconomicEventType type;           // Tipo
    time_t startTime;                  // Hora de início
    time_t endTime;                    // Hora de término
    float impactFactor;                // Fator de impacto
    std::vector<WORD> affectedItems;   // Itens afetados
    std::vector<MarketCategory> affectedCategories; // Categorias afetadas
    std::string description;           // Descrição
    bool active;                       // Ativo
    
    EconomicEvent()
        : eventID(0)
        , type(EconomicEventType::PRICE_CHANGE)
        , startTime(0)
        , endTime(0)
        , impactFactor(0.0f)
        , description("")
        , active(false)
    {
    }
};

/**
 * @brief Intervenção econômica
 */
struct EconomicIntervention {
    DWORD interventionID;             // ID da intervenção
    EconomicInterventionType type;    // Tipo
    time_t startTime;                  // Hora de início
    time_t endTime;                    // Hora de término
    float magnitude;                   // Magnitude
    std::vector<WORD> targetItems;     // Itens alvo
    std::vector<MarketCategory> targetCategories; // Categorias alvo
    DWORD targetValue;                // Valor alvo
    std::string reason;                // Razão
    bool active;                       // Ativo
    
    EconomicIntervention()
        : interventionID(0)
        , type(EconomicInterventionType::PRICE_FLOOR)
        , startTime(0)
        , endTime(0)
        , magnitude(0.0f)
        , targetValue(0)
        , reason("")
        , active(false)
    {
    }
};

/**
 * @brief Configuração de troca entre servidores
 */
struct CrossServerConfig {
    DWORD configID;                   // ID da configuração
    DWORD serverID;                   // ID do servidor
    std::string serverName;            // Nome do servidor
    bool tradingEnabled;               // Comércio habilitado
    bool inventorySyncEnabled;         // Sincronização de inventário habilitada
    bool currencySyncEnabled;          // Sincronização de moeda habilitada
    bool marketSyncEnabled;            // Sincronização de mercado habilitada
    TradingRestrictionLevel restrictionLevel; // Nível de restrição
    DWORD syncInterval;               // Intervalo de sincronização (ms)
    time_t lastSyncTime;               // Hora da última sincronização
    CrossServerSyncStatus syncStatus;  // Status de sincronização
    
    CrossServerConfig()
        : configID(0)
        , serverID(0)
        , serverName("")
        , tradingEnabled(false)
        , inventorySyncEnabled(false)
        , currencySyncEnabled(false)
        , marketSyncEnabled(false)
        , restrictionLevel(TradingRestrictionLevel::NONE)
        , syncInterval(0)
        , lastSyncTime(0)
        , syncStatus(CrossServerSyncStatus::SYNCHRONIZED)
    {
    }
};

/**
 * @brief Status de uma moeda
 */
struct CurrencyStatus {
    CurrencyType type;                // Tipo
    std::string name;                  // Nome
    std::string description;           // Descrição
    DWORD totalSupply;                // Oferta total
    DWORD circulatingSupply;          // Oferta circulante
    float inflationRate;               // Taxa de inflação
    float exchangeRate;                // Taxa de câmbio (para ouro)
    bool tradable;                     // Negociável
    bool limited;                      // Limitado
    
    CurrencyStatus()
        : type(CurrencyType::GOLD)
        , name("")
        , description("")
        , totalSupply(0)
        , circulatingSupply(0)
        , inflationRate(0.0f)
        , exchangeRate(1.0f)
        , tradable(true)
        , limited(false)
    {
    }
};

/**
 * @brief Item em lista de observação
 */
struct WatchlistItem {
    DWORD watchID;                    // ID de observação
    DWORD playerID;                   // ID do jogador
    WORD itemID;                      // ID do item
    BYTE quality;                     // Qualidade
    BYTE enchantLevel;                // Nível de encantamento
    DWORD maxPrice;                   // Preço máximo
    bool notifyOnListing;              // Notificar ao listar
    bool notifyOnPriceChange;          // Notificar em mudança de preço
    time_t addedTime;                  // Hora de adição
    
    WatchlistItem()
        : watchID(0)
        , playerID(0)
        , itemID(0)
        , quality(0)
        , enchantLevel(0)
        , maxPrice(0)
        , notifyOnListing(false)
        , notifyOnPriceChange(false)
        , addedTime(0)
    {
    }
};

/**
 * @brief Estatísticas de mercado
 */
struct MarketStatistics {
    DWORD activeListings;              // Listagens ativas
    DWORD completedTransactions;       // Transações completadas
    DWORD totalGoldTraded;             // Total de ouro negociado
    DWORD averageTransactionValue;     // Valor médio de transação
    DWORD uniqueBuyers;                // Compradores únicos
    DWORD uniqueSellers;               // Vendedores únicos
    DWORD highestValueTransaction;     // Transação de maior valor
    DWORD mostTradedItemID;            // ID do item mais negociado
    float taxCollected;                 // Imposto coletado
    float marketLiquidity;              // Liquidez de mercado
    
    MarketStatistics()
        : activeListings(0)
        , completedTransactions(0)
        , totalGoldTraded(0)
        , averageTransactionValue(0)
        , uniqueBuyers(0)
        , uniqueSellers(0)
        , highestValueTransaction(0)
        , mostTradedItemID(0)
        , taxCollected(0.0f)
        , marketLiquidity(0.0f)
    {
    }
};

/**
 * @brief Rule for market price controls
 */
struct PriceControlRule {
    DWORD ruleID;                      // ID da regra
    WORD itemID;                       // ID do item
    BYTE quality;                      // Qualidade
    DWORD minPrice;                    // Preço mínimo
    DWORD maxPrice;                    // Preço máximo
    float adjustmentFactor;             // Fator de ajuste
    bool enforced;                      // Aplicada
    std::string reason;                 // Razão
    
    PriceControlRule()
        : ruleID(0)
        , itemID(0)
        , quality(0)
        , minPrice(0)
        , maxPrice(0)
        , adjustmentFactor(1.0f)
        , enforced(false)
        , reason("")
    {
    }
};

/**
 * @brief Callback para eventos econômicos
 */
using EconomicEventCallback = std::function<void(const EconomicEvent& event)>;

/**
 * @brief Callback para transações de mercado
 */
using MarketTransactionCallback = std::function<void(const MarketTransaction& transaction)>;

/**
 * @brief Callback para intervenções econômicas
 */
using EconomicInterventionCallback = std::function<void(const EconomicIntervention& intervention)>;

// Final do namespace
}
}
}

#endif // ECONOMYTYPES_H

} // namespace wydbr
