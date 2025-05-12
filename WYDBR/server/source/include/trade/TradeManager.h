/**
 * TradeManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/trade/TradeManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _TRADE_MANAGER_H_
#define _TRADE_MANAGER_H_

/**
 * @file TradeManager.h
 * @brief Gerenciador de comércio do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de comércio do WYDBR,
 * responsável por controlar as trocas entre jogadores, o mercado e os leilões,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <chrono>

#include "../GlobalDef.h"
#include "../Basedef.h"

namespace wydbr {
namespace trade {

/**
 * @brief Estado da transação
 */
enum /**
 * Classe WYDTradeState
 * 
 * Esta classe implementa a funcionalidade WYDTradeState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TradeState {
    NONE = 0,                   // Nenhum
    INITIATED = 1,              // Iniciada
    WAITING_CONFIRMATION = 2,   // Aguardando confirmação
    CONFIRMED = 3,              // Confirmada
    COMPLETED = 4,              // Concluída
    CANCELED = 5,               // Cancelada
    FAILED = 6,                 // Falha
    EXPIRED = 7,                // Expirada
    LOCKED = 8,                 // Bloqueada
    VERIFIED = 9,               // Verificada
    PROCESSING = 10,            // Processando
    REFUNDED = 11,              // Reembolsada
    CUSTOM_1 = 12,              // Personalizado 1
    CUSTOM_2 = 13,              // Personalizado 2
    CUSTOM_3 = 14,              // Personalizado 3
    UNKNOWN = 15                // Desconhecido
};

/**
 * @brief Tipo de comércio
 */
enum /**
 * Classe WYDTradeType
 * 
 * Esta classe implementa a funcionalidade WYDTradeType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TradeType {
    NONE = 0,                   // Nenhum
    PLAYER_TO_PLAYER = 1,       // Jogador para jogador
    PLAYER_TO_NPC = 2,          // Jogador para NPC
    NPC_TO_PLAYER = 3,          // NPC para jogador
    MARKET_SELL = 4,            // Venda no mercado
    MARKET_BUY = 5,             // Compra no mercado
    AUCTION_BID = 6,            // Lance em leilão
    AUCTION_SELL = 7,           // Venda em leilão
    GUILD_STORE = 8,            // Armazém de guilda
    BANK_DEPOSIT = 9,           // Depósito bancário
    BANK_WITHDRAW = 10,         // Saque bancário
    MAIL_ATTACHMENT = 11,       // Anexo de correio
    SYSTEM_REWARD = 12,         // Recompensa do sistema
    QUEST_REWARD = 13,          // Recompensa de missão
    EVENT_REWARD = 14,          // Recompensa de evento
    EXCHANGE_TOKEN = 15,        // Troca de token
    CUSTOM_1 = 16,              // Personalizado 1
    CUSTOM_2 = 17,              // Personalizado 2
    CUSTOM_3 = 18,              // Personalizado 3
    UNKNOWN = 19                // Desconhecido
};

/**
 * @brief Tipo de leilão
 */
enum /**
 * Classe WYDAuctionType
 * 
 * Esta classe implementa a funcionalidade WYDAuctionType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AuctionType {
    NONE = 0,                   // Nenhum
    STANDARD = 1,               // Padrão
    DUTCH = 2,                  // Holandês
    SEALED = 3,                 // Lacrado
    RESERVE = 4,                // Reserva
    INSTANT = 5,                // Instantâneo
    SCHEDULED = 6,              // Agendado
    FEATURED = 7,               // Destaque
    LIMITED = 8,                // Limitado
    BLIND = 9,                  // Cego
    CUSTOM_1 = 10,              // Personalizado 1
    CUSTOM_2 = 11,              // Personalizado 2
    CUSTOM_3 = 12,              // Personalizado 3
    UNKNOWN = 13                // Desconhecido
};

/**
 * @brief Tipo de listagem de mercado
 */
enum /**
 * Classe WYDMarketListingType
 * 
 * Esta classe implementa a funcionalidade WYDMarketListingType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MarketListingType {
    NONE = 0,                   // Nenhum
    FIXED_PRICE = 1,            // Preço fixo
    BEST_OFFER = 2,             // Melhor oferta
    BULK_QUANTITY = 3,          // Quantidade em massa
    VARIABLE_PRICE = 4,         // Preço variável
    BUNDLE = 5,                 // Pacote
    LIMITED_TIME = 6,           // Tempo limitado
    SUBSCRIPTION = 7,           // Assinatura
    CUSTOM_1 = 8,               // Personalizado 1
    CUSTOM_2 = 9,               // Personalizado 2
    CUSTOM_3 = 10,              // Personalizado 3
    UNKNOWN = 11                // Desconhecido
};

/**
 * @brief Resultado da operação de comércio
 */
enum /**
 * Classe WYDTradeResult
 * 
 * Esta classe implementa a funcionalidade WYDTradeResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TradeResult {
    SUCCESS = 0,                // Sucesso
    INSUFFICIENT_FUNDS = 1,     // Fundos insuficientes
    INVENTORY_FULL = 2,         // Inventário cheio
    ITEM_NOT_FOUND = 3,         // Item não encontrado
    TRADE_CANCELED = 4,         // Comércio cancelado
    TRADE_EXPIRED = 5,          // Comércio expirado
    INVALID_OFFER = 6,          // Oferta inválida
    INVALID_BID = 7,            // Lance inválido
    OUTBID = 8,                 // Superado
    AUCTION_ENDED = 9,          // Leilão encerrado
    LISTING_NOT_FOUND = 10,     // Listagem não encontrada
    INSUFFICIENT_PERMISSION = 11, // Permissão insuficiente
    TRADE_LOCKED = 12,          // Comércio bloqueado
    TRADE_LIMIT_REACHED = 13,   // Limite de comércio atingido
    INVALID_OPERATION = 14,     // Operação inválida
    SYSTEM_ERROR = 15,          // Erro do sistema
    CUSTOM_1 = 16,              // Personalizado 1
    CUSTOM_2 = 17,              // Personalizado 2
    CUSTOM_3 = 18,              // Personalizado 3
    UNKNOWN = 19                // Desconhecido
};

/**
 * @brief Tipo de taxa de comércio
 */
enum /**
 * Classe WYDTradeFeeType
 * 
 * Esta classe implementa a funcionalidade WYDTradeFeeType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TradeFeeType {
    NONE = 0,                   // Nenhum
    LISTING_FEE = 1,            // Taxa de listagem
    TRANSACTION_FEE = 2,        // Taxa de transação
    MARKET_FEE = 3,             // Taxa de mercado
    AUCTION_FEE = 4,            // Taxa de leilão
    CURRENCY_EXCHANGE_FEE = 5,  // Taxa de câmbio
    BANK_FEE = 6,               // Taxa bancária
    RUSH_FEE = 7,               // Taxa de urgência
    ESCROW_FEE = 8,             // Taxa de custódia
    REFUND_FEE = 9,             // Taxa de reembolso
    CUSTOM_1 = 10,              // Personalizado 1
    CUSTOM_2 = 11,              // Personalizado 2
    CUSTOM_3 = 12,              // Personalizado 3
    UNKNOWN = 13                // Desconhecido
};

/**
 * @brief Categoria do mercado
 */
enum /**
 * Classe WYDMarketCategory
 * 
 * Esta classe implementa a funcionalidade WYDMarketCategory conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MarketCategory {
    NONE = 0,                   // Nenhum
    WEAPON = 1,                 // Arma
    ARMOR = 2,                  // Armadura
    ACCESSORY = 3,              // Acessório
    CONSUMABLE = 4,             // Consumível
    MATERIAL = 5,               // Material
    QUEST_ITEM = 6,             // Item de missão
    MOUNT = 7,                  // Montaria
    PET = 8,                    // Mascote
    DECORATION = 9,             // Decoração
    CURRENCY = 10,              // Moeda
    RECIPE = 11,                // Receita
    BLUEPRINT = 12,             // Projeto
    GEM = 13,                   // Gema
    ENHANCEMENT = 14,           // Melhoria
    SERVICE = 15,               // Serviço
    CUSTOM_1 = 16,              // Personalizado 1
    CUSTOM_2 = 17,              // Personalizado 2
    CUSTOM_3 = 18,              // Personalizado 3
    UNKNOWN = 19                // Desconhecido
};

/**
 * @brief Filtragem de mercado
 */
enum /**
 * Classe WYDMarketSort
 * 
 * Esta classe implementa a funcionalidade WYDMarketSort conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MarketSort {
    NONE = 0,                   // Nenhum
    PRICE_LOW_TO_HIGH = 1,      // Preço baixo para alto
    PRICE_HIGH_TO_LOW = 2,      // Preço alto para baixo
    NAME_A_TO_Z = 3,            // Nome A a Z
    NAME_Z_TO_A = 4,            // Nome Z a A
    NEWEST_FIRST = 5,           // Mais novo primeiro
    OLDEST_FIRST = 6,           // Mais antigo primeiro
    END_TIME_SOONEST = 7,       // Tempo de término mais próximo
    RELEVANCE = 8,              // Relevância
    QUANTITY_LOW_TO_HIGH = 9,   // Quantidade baixa para alta
    QUANTITY_HIGH_TO_LOW = 10,  // Quantidade alta para baixa
    RATING = 11,                // Avaliação
    POPULARITY = 12,            // Popularidade
    CUSTOM_1 = 13,              // Personalizado 1
    CUSTOM_2 = 14,              // Personalizado 2
    CUSTOM_3 = 15,              // Personalizado 3
    UNKNOWN = 16                // Desconhecido
};

/**
 * @brief Item de comércio
 */
struct TradeItem {
    uint32_t TradeItemID;       // ID do item de comércio
    uint32_t CharacterID;       // ID do personagem
    STRUCT_ITEM Item;           // Item
    uint32_t Quantity;          // Quantidade
    uint32_t Price;             // Preço
    bool Locked;                // Bloqueado
    bool Confirmed;             // Confirmado
    
    TradeItem() : TradeItemID(0), CharacterID(0), Quantity(0), Price(0),
        Locked(false), Confirmed(false) {
        memset(&Item, 0, sizeof(STRUCT_ITEM));
    }
};

/**
 * @brief Transação de comércio
 */
struct TradeTransaction {
    uint32_t TransactionID;     // ID da transação
    TradeType Type;             // Tipo
    TradeState State;           // Estado
    uint32_t InitiatorID;       // ID do iniciador
    uint32_t TargetID;          // ID do alvo
    uint32_t StartTime;         // Tempo de início
    uint32_t EndTime;           // Tempo de fim
    uint32_t TotalValue;        // Valor total
    uint32_t TaxAmount;         // Valor do imposto
    std::vector<TradeItem> InitiatorItems; // Itens do iniciador
    std::vector<TradeItem> TargetItems; // Itens do alvo
    uint32_t InitiatorGold;     // Ouro do iniciador
    uint32_t TargetGold;        // Ouro do alvo
    std::string Note;           // Observação
    bool InitiatorConfirmed;    // Iniciador confirmou
    bool TargetConfirmed;       // Alvo confirmou
    uint32_t LastUpdateTime;    // Último tempo de atualização
    
    TradeTransaction() : TransactionID(0), Type(TradeType::NONE),
        State(TradeState::NONE), InitiatorID(0), TargetID(0),
        StartTime(0), EndTime(0), TotalValue(0), TaxAmount(0),
        InitiatorGold(0), TargetGold(0), InitiatorConfirmed(false),
        TargetConfirmed(false), LastUpdateTime(0) {}
};

/**
 * @brief Listagem de mercado
 */
struct MarketListing {
    uint32_t ListingID;         // ID da listagem
    uint32_t SellerID;          // ID do vendedor
    STRUCT_ITEM Item;           // Item
    uint32_t Quantity;          // Quantidade
    uint32_t Price;             // Preço
    uint32_t MinBid;            // Lance mínimo
    uint32_t BuyoutPrice;       // Preço de compra imediata
    uint32_t TimePosted;        // Tempo postado
    uint32_t ExpirationTime;    // Tempo de expiração
    MarketListingType Type;     // Tipo
    MarketCategory Category;    // Categoria
    bool Featured;              // Destaque
    bool Sold;                  // Vendido
    bool Expired;               // Expirado
    bool Canceled;              // Cancelado
    uint32_t ViewCount;         // Contagem de visualizações
    
    MarketListing() : ListingID(0), SellerID(0), Quantity(0), Price(0),
        MinBid(0), BuyoutPrice(0), TimePosted(0), ExpirationTime(0),
        Type(MarketListingType::NONE), Category(MarketCategory::NONE),
        Featured(false), Sold(false), Expired(false), Canceled(false),
        ViewCount(0) {
        memset(&Item, 0, sizeof(STRUCT_ITEM));
    }
};

/**
 * @brief Leilão
 */
struct Auction {
    uint32_t AuctionID;         // ID do leilão
    uint32_t SellerID;          // ID do vendedor
    STRUCT_ITEM Item;           // Item
    uint32_t Quantity;          // Quantidade
    uint32_t StartingBid;       // Lance inicial
    uint32_t CurrentBid;        // Lance atual
    uint32_t BuyoutPrice;       // Preço de compra imediata
    uint32_t ReservePrice;      // Preço de reserva
    uint32_t TimePosted;        // Tempo postado
    uint32_t EndTime;           // Tempo de fim
    uint32_t BidIncrement;      // Incremento de lance
    AuctionType Type;           // Tipo
    MarketCategory Category;    // Categoria
    uint32_t HighestBidderID;   // ID do maior licitante
    uint32_t BidCount;          // Contagem de lances
    bool Featured;              // Destaque
    bool Ended;                 // Encerrado
    bool Canceled;              // Cancelado
    bool ReserveMet;            // Reserva atendida
    uint32_t ViewCount;         // Contagem de visualizações
    
    Auction() : AuctionID(0), SellerID(0), Quantity(0), StartingBid(0),
        CurrentBid(0), BuyoutPrice(0), ReservePrice(0), TimePosted(0),
        EndTime(0), BidIncrement(0), Type(AuctionType::NONE),
        Category(MarketCategory::NONE), HighestBidderID(0), BidCount(0),
        Featured(false), Ended(false), Canceled(false), ReserveMet(false),
        ViewCount(0) {
        memset(&Item, 0, sizeof(STRUCT_ITEM));
    }
};

/**
 * @brief Lance de leilão
 */
struct AuctionBid {
    uint32_t BidID;             // ID do lance
    uint32_t AuctionID;         // ID do leilão
    uint32_t BidderID;          // ID do licitante
    uint32_t BidAmount;         // Valor do lance
    uint32_t BidTime;           // Tempo do lance
    bool AutoBid;               // Lance automático
    uint32_t MaxAutoBid;        // Lance automático máximo
    bool IsWinning;             // Está ganhando
    bool Refunded;              // Reembolsado
    
    AuctionBid() : BidID(0), AuctionID(0), BidderID(0), BidAmount(0),
        BidTime(0), AutoBid(false), MaxAutoBid(0), IsWinning(false),
        Refunded(false) {}
};

/**
 * @brief Oferta de mercado
 */
struct MarketOffer {
    uint32_t OfferID;           // ID da oferta
    uint32_t ListingID;         // ID da listagem
    uint32_t BuyerID;           // ID do comprador
    uint32_t OfferAmount;       // Valor da oferta
    uint32_t OfferTime;         // Tempo da oferta
    bool Accepted;              // Aceita
    bool Rejected;              // Rejeitada
    bool Expired;               // Expirada
    bool Countered;             // Contraproposta
    uint32_t CounterAmount;     // Valor da contraproposta
    
    MarketOffer() : OfferID(0), ListingID(0), BuyerID(0), OfferAmount(0),
        OfferTime(0), Accepted(false), Rejected(false), Expired(false),
        Countered(false), CounterAmount(0) {}
};

/**
 * @brief Histórico de comércio
 */
struct TradeHistory {
    uint32_t HistoryID;         // ID do histórico
    uint32_t TransactionID;     // ID da transação
    uint32_t ListingID;         // ID da listagem
    uint32_t AuctionID;         // ID do leilão
    TradeType Type;             // Tipo
    uint32_t SellerID;          // ID do vendedor
    uint32_t BuyerID;           // ID do comprador
    STRUCT_ITEM Item;           // Item
    uint32_t Quantity;          // Quantidade
    uint32_t Price;             // Preço
    uint32_t FeeAmount;         // Valor da taxa
    uint32_t TransactionTime;   // Tempo da transação
    TradeResult Result;         // Resultado
    std::string Note;           // Observação
    
    TradeHistory() : HistoryID(0), TransactionID(0), ListingID(0),
        AuctionID(0), Type(TradeType::NONE), SellerID(0), BuyerID(0),
        Quantity(0), Price(0), FeeAmount(0), TransactionTime(0),
        Result(TradeResult::SUCCESS) {
        memset(&Item, 0, sizeof(STRUCT_ITEM));
    }
};

/**
 * @brief Configuração de taxa
 */
struct FeeConfig {
    TradeFeeType Type;          // Tipo
    float BaseRate;             // Taxa base
    float MinimumFee;           // Taxa mínima
    float MaximumFee;           // Taxa máxima
    float DiscountRate;         // Taxa de desconto
    bool Enabled;               // Habilitada
    
    FeeConfig() : Type(TradeFeeType::NONE), BaseRate(0.0f),
        MinimumFee(0.0f), MaximumFee(0.0f), DiscountRate(0.0f),
        Enabled(true) {}
};

/**
 * @brief Estatísticas de mercado
 */
struct MarketStats {
    uint32_t TotalListings;     // Total de listagens
    uint32_t ActiveListings;    // Listagens ativas
    uint32_t SoldItems;         // Itens vendidos
    uint32_t ExpiredItems;      // Itens expirados
    uint32_t TotalSales;        // Total de vendas
    uint32_t TotalVolume;       // Volume total
    uint32_t HighestSale;       // Maior venda
    uint32_t LowestSale;        // Menor venda
    float AverageSale;          // Venda média
    uint32_t FeeCollected;      // Taxa coletada
    std::map<MarketCategory, uint32_t> ListingsByCategory; // Listagens por categoria
    std::map<MarketCategory, uint32_t> SalesByCategory; // Vendas por categoria
    
    MarketStats() : TotalListings(0), ActiveListings(0), SoldItems(0),
        ExpiredItems(0), TotalSales(0), TotalVolume(0), HighestSale(0),
        LowestSale(0), AverageSale(0.0f), FeeCollected(0) {}
};

/**
 * @brief Estatísticas de leilão
 */
struct AuctionStats {
    uint32_t TotalAuctions;     // Total de leilões
    uint32_t ActiveAuctions;    // Leilões ativos
    uint32_t CompletedAuctions; // Leilões completos
    uint32_t CanceledAuctions;  // Leilões cancelados
    uint32_t TotalBids;         // Total de lances
    uint32_t TotalVolume;       // Volume total
    uint32_t HighestBid;        // Maior lance
    uint32_t LowestBid;         // Menor lance
    float AverageBid;           // Lance médio
    uint32_t FeeCollected;      // Taxa coletada
    std::map<AuctionType, uint32_t> AuctionsByType; // Leilões por tipo
    std::map<MarketCategory, uint32_t> AuctionsByCategory; // Leilões por categoria
    
    AuctionStats() : TotalAuctions(0), ActiveAuctions(0), CompletedAuctions(0),
        CanceledAuctions(0), TotalBids(0), TotalVolume(0), HighestBid(0),
        LowestBid(0), AverageBid(0.0f), FeeCollected(0) {}
};

/**
 * @brief Estatísticas de jogador
 */
struct PlayerTradeStats {
    uint32_t CharacterID;       // ID do personagem
    uint32_t ItemsSold;         // Itens vendidos
    uint32_t ItemsBought;       // Itens comprados
    uint32_t TotalSales;        // Total de vendas
    uint32_t TotalPurchases;    // Total de compras
    uint32_t AuctionsWon;       // Leilões ganhos
    uint32_t AuctionsLost;      // Leilões perdidos
    uint32_t TotalFeesPaid;     // Total de taxas pagas
    uint32_t HighestSale;       // Maior venda
    uint32_t HighestPurchase;   // Maior compra
    uint32_t TradeCount;        // Contagem de comércio
    uint8_t TradeRating;        // Avaliação de comércio
    bool IsTradeBanned;         // Está com comércio banido
    
    PlayerTradeStats() : CharacterID(0), ItemsSold(0), ItemsBought(0),
        TotalSales(0), TotalPurchases(0), AuctionsWon(0), AuctionsLost(0),
        TotalFeesPaid(0), HighestSale(0), HighestPurchase(0), TradeCount(0),
        TradeRating(0), IsTradeBanned(false) {}
};

/**
 * @brief Estado do mercado
 */
struct MarketState {
    bool IsOpen;                // Está aberto
    uint32_t OpenTime;          // Tempo de abertura
    uint32_t CloseTime;         // Tempo de fechamento
    uint32_t MaintenanceTime;   // Tempo de manutenção
    bool IsInMaintenance;       // Está em manutenção
    uint32_t TotalTransactions; // Total de transações
    uint32_t TodayTransactions; // Transações de hoje
    uint32_t ListingLimit;      // Limite de listagem
    uint32_t ListingDuration;   // Duração da listagem
    
    MarketState() : IsOpen(true), OpenTime(0), CloseTime(0),
        MaintenanceTime(0), IsInMaintenance(false), TotalTransactions(0),
        TodayTransactions(0), ListingLimit(50), ListingDuration(86400) {}
};

/**
 * @brief Filtro de mercado
 */
struct MarketFilter {
    std::string SearchText;     // Texto de pesquisa
    MarketCategory Category;    // Categoria
    MarketListingType Type;     // Tipo
    uint32_t MinPrice;          // Preço mínimo
    uint32_t MaxPrice;          // Preço máximo
    uint32_t MinLevel;          // Nível mínimo
    uint32_t MaxLevel;          // Nível máximo
    uint8_t MinQuality;         // Qualidade mínima
    uint8_t MaxQuality;         // Qualidade máxima
    bool ShowSold;              // Mostrar vendidos
    bool ShowExpired;           // Mostrar expirados
    bool ShowCanceled;          // Mostrar cancelados
    uint32_t SellerID;          // ID do vendedor
    MarketSort SortBy;          // Ordenar por
    uint32_t Page;              // Página
    uint32_t PageSize;          // Tamanho da página
    
    MarketFilter() : Category(MarketCategory::NONE), Type(MarketListingType::NONE),
        MinPrice(0), MaxPrice(0), MinLevel(0), MaxLevel(0), MinQuality(0),
        MaxQuality(0), ShowSold(false), ShowExpired(false), ShowCanceled(false),
        SellerID(0), SortBy(MarketSort::NONE), Page(0), PageSize(20) {}
};

/**
 * @brief Tipo de callback de comércio
 */
using TradeCallback = std::function<TradeResult(const TradeTransaction&)>;

/**
 * @brief Tipo de callback de mercado
 */
using MarketCallback = std::function<bool(const MarketListing&)>;

/**
 * @brief Tipo de callback de leilão
 */
using AuctionCallback = std::function<bool(const Auction&)>;

/**
 * @brief Gerenciador de comércio
 * 
 * Esta classe é responsável por gerenciar as trocas entre jogadores, o mercado e os leilões.
 */
/**
 * Classe WYDTradeManager
 * 
 * Esta classe implementa a funcionalidade WYDTradeManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TradeManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static TradeManager& getInstance() {
        static TradeManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração de comércio
     * @return true se carregado com sucesso
     */
    bool loadTradeConfig();
    
    /**
     * @brief Inicia uma transação de comércio
     * @param initiatorID ID do iniciador
     * @param targetID ID do alvo
     * @param type Tipo
     * @return ID da transação ou 0 em caso de erro
     */
    uint32_t startTrade(uint32_t initiatorID, uint32_t targetID, TradeType type = TradeType::PLAYER_TO_PLAYER);
    
    /**
     * @brief Cancela uma transação de comércio
     * @param transactionID ID da transação
     * @param characterID ID do personagem que cancela
     * @return Resultado da operação
     */
    TradeResult cancelTrade(uint32_t transactionID, uint32_t characterID);
    
    /**
     * @brief Adiciona um item a uma transação de comércio
     * @param transactionID ID da transação
     * @param characterID ID do personagem
     * @param item Item
     * @param quantity Quantidade
     * @param price Preço
     * @return Resultado da operação
     */
    TradeResult addItemToTrade(uint32_t transactionID, uint32_t characterID, const STRUCT_ITEM& item, uint32_t quantity = 1, uint32_t price = 0);
    
    /**
     * @brief Remove um item de uma transação de comércio
     * @param transactionID ID da transação
     * @param characterID ID do personagem
     * @param tradeItemID ID do item de comércio
     * @return Resultado da operação
     */
    TradeResult removeItemFromTrade(uint32_t transactionID, uint32_t characterID, uint32_t tradeItemID);
    
    /**
     * @brief Adiciona ouro a uma transação de comércio
     * @param transactionID ID da transação
     * @param characterID ID do personagem
     * @param amount Quantidade
     * @return Resultado da operação
     */
    TradeResult addGoldToTrade(uint32_t transactionID, uint32_t characterID, uint32_t amount);
    
    /**
     * @brief Altera a quantidade de ouro em uma transação de comércio
     * @param transactionID ID da transação
     * @param characterID ID do personagem
     * @param amount Quantidade
     * @return Resultado da operação
     */
    TradeResult updateGoldInTrade(uint32_t transactionID, uint32_t characterID, uint32_t amount);
    
    /**
     * @brief Bloqueia os itens em uma transação de comércio
     * @param transactionID ID da transação
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    TradeResult lockTrade(uint32_t transactionID, uint32_t characterID);
    
    /**
     * @brief Confirma uma transação de comércio
     * @param transactionID ID da transação
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    TradeResult confirmTrade(uint32_t transactionID, uint32_t characterID);
    
    /**
     * @brief Completa uma transação de comércio
     * @param transactionID ID da transação
     * @return Resultado da operação
     */
    TradeResult completeTrade(uint32_t transactionID);
    
    /**
     * @brief Obtém uma transação de comércio
     * @param transactionID ID da transação
     * @return Transação ou nullptr se não encontrada
     */
    const TradeTransaction* getTrade(uint32_t transactionID) const;
    
    /**
     * @brief Obtém as transações de comércio ativas de um personagem
     * @param characterID ID do personagem
     * @return Lista de IDs de transações
     */
    std::vector<uint32_t> getActiveTradesForCharacter(uint32_t characterID) const;
    
    /**
     * @brief Verifica se um personagem está em comércio
     * @param characterID ID do personagem
     * @return true se estiver em comércio
     */
    bool isCharacterTrading(uint32_t characterID) const;
    
    /**
     * @brief Cria uma listagem de mercado
     * @param sellerID ID do vendedor
     * @param item Item
     * @param quantity Quantidade
     * @param price Preço
     * @param listingType Tipo de listagem
     * @param category Categoria
     * @param expirationTime Tempo de expiração (0 para padrão)
     * @param minBid Lance mínimo (para ofertas)
     * @param buyoutPrice Preço de compra imediata
     * @return ID da listagem ou 0 em caso de erro
     */
    uint32_t createMarketListing(uint32_t sellerID, const STRUCT_ITEM& item, uint32_t quantity, uint32_t price,
                                MarketListingType listingType = MarketListingType::FIXED_PRICE,
                                MarketCategory category = MarketCategory::NONE,
                                uint32_t expirationTime = 0, uint32_t minBid = 0, uint32_t buyoutPrice = 0);
    
    /**
     * @brief Cancela uma listagem de mercado
     * @param listingID ID da listagem
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    TradeResult cancelMarketListing(uint32_t listingID, uint32_t characterID);
    
    /**
     * @brief Compra um item do mercado
     * @param listingID ID da listagem
     * @param buyerID ID do comprador
     * @param quantity Quantidade (0 para toda a quantidade)
     * @return Resultado da operação
     */
    TradeResult buyMarketItem(uint32_t listingID, uint32_t buyerID, uint32_t quantity = 0);
    
    /**
     * @brief Faz uma oferta em uma listagem de mercado
     * @param listingID ID da listagem
     * @param buyerID ID do comprador
     * @param offerAmount Valor da oferta
     * @return ID da oferta ou 0 em caso de erro
     */
    uint32_t makeMarketOffer(uint32_t listingID, uint32_t buyerID, uint32_t offerAmount);
    
    /**
     * @brief Aceita uma oferta de mercado
     * @param offerID ID da oferta
     * @param sellerID ID do vendedor
     * @return Resultado da operação
     */
    TradeResult acceptMarketOffer(uint32_t offerID, uint32_t sellerID);
    
    /**
     * @brief Rejeita uma oferta de mercado
     * @param offerID ID da oferta
     * @param sellerID ID do vendedor
     * @return Resultado da operação
     */
    TradeResult rejectMarketOffer(uint32_t offerID, uint32_t sellerID);
    
    /**
     * @brief Faz uma contraproposta para uma oferta de mercado
     * @param offerID ID da oferta
     * @param sellerID ID do vendedor
     * @param counterAmount Valor da contraproposta
     * @return Resultado da operação
     */
    TradeResult counterMarketOffer(uint32_t offerID, uint32_t sellerID, uint32_t counterAmount);
    
    /**
     * @brief Obtém uma listagem de mercado
     * @param listingID ID da listagem
     * @return Listagem ou nullptr se não encontrada
     */
    const MarketListing* getMarketListing(uint32_t listingID) const;
    
    /**
     * @brief Pesquisa listagens de mercado
     * @param filter Filtro
     * @return Lista de IDs de listagens
     */
    std::vector<uint32_t> searchMarketListings(const MarketFilter& filter);
    
    /**
     * @brief Obtém listagens de mercado por categoria
     * @param category Categoria
     * @param activeOnly Apenas ativas
     * @return Lista de IDs de listagens
     */
    std::vector<uint32_t> getMarketListingsByCategory(MarketCategory category, bool activeOnly = true);
    
    /**
     * @brief Obtém listagens de mercado por vendedor
     * @param sellerID ID do vendedor
     * @param activeOnly Apenas ativas
     * @return Lista de IDs de listagens
     */
    std::vector<uint32_t> getMarketListingsBySeller(uint32_t sellerID, bool activeOnly = true);
    
    /**
     * @brief Cria um leilão
     * @param sellerID ID do vendedor
     * @param item Item
     * @param quantity Quantidade
     * @param startingBid Lance inicial
     * @param buyoutPrice Preço de compra imediata
     * @param auctionType Tipo de leilão
     * @param category Categoria
     * @param duration Duração em segundos (0 para padrão)
     * @param reservePrice Preço de reserva
     * @param bidIncrement Incremento de lance
     * @return ID do leilão ou 0 em caso de erro
     */
    uint32_t createAuction(uint32_t sellerID, const STRUCT_ITEM& item, uint32_t quantity, uint32_t startingBid,
                          uint32_t buyoutPrice = 0, AuctionType auctionType = AuctionType::STANDARD,
                          MarketCategory category = MarketCategory::NONE, uint32_t duration = 0,
                          uint32_t reservePrice = 0, uint32_t bidIncrement = 0);
    
    /**
     * @brief Cancela um leilão
     * @param auctionID ID do leilão
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    TradeResult cancelAuction(uint32_t auctionID, uint32_t characterID);
    
    /**
     * @brief Faz um lance em um leilão
     * @param auctionID ID do leilão
     * @param bidderID ID do licitante
     * @param bidAmount Valor do lance
     * @param autoBid Lance automático
     * @param maxAutoBid Lance automático máximo
     * @return Resultado da operação
     */
    TradeResult placeBid(uint32_t auctionID, uint32_t bidderID, uint32_t bidAmount, bool autoBid = false, uint32_t maxAutoBid = 0);
    
    /**
     * @brief Compra imediatamente um item de leilão
     * @param auctionID ID do leilão
     * @param buyerID ID do comprador
     * @return Resultado da operação
     */
    TradeResult buyoutAuction(uint32_t auctionID, uint32_t buyerID);
    
    /**
     * @brief Finaliza um leilão
     * @param auctionID ID do leilão
     * @return Resultado da operação
     */
    TradeResult finalizeAuction(uint32_t auctionID);
    
    /**
     * @brief Obtém um leilão
     * @param auctionID ID do leilão
     * @return Leilão ou nullptr se não encontrado
     */
    const Auction* getAuction(uint32_t auctionID) const;
    
    /**
     * @brief Obtém lances de um leilão
     * @param auctionID ID do leilão
     * @return Lista de lances
     */
    std::vector<AuctionBid> getAuctionBids(uint32_t auctionID) const;
    
    /**
     * @brief Obtém lances de um licitante
     * @param bidderID ID do licitante
     * @param activeOnly Apenas ativos
     * @return Lista de lances
     */
    std::vector<AuctionBid> getBidsByBidder(uint32_t bidderID, bool activeOnly = true) const;
    
    /**
     * @brief Pesquisa leilões
     * @param filter Filtro
     * @return Lista de IDs de leilões
     */
    std::vector<uint32_t> searchAuctions(const MarketFilter& filter);
    
    /**
     * @brief Obtém leilões por categoria
     * @param category Categoria
     * @param activeOnly Apenas ativos
     * @return Lista de IDs de leilões
     */
    std::vector<uint32_t> getAuctionsByCategory(MarketCategory category, bool activeOnly = true);
    
    /**
     * @brief Obtém leilões por vendedor
     * @param sellerID ID do vendedor
     * @param activeOnly Apenas ativos
     * @return Lista de IDs de leilões
     */
    std::vector<uint32_t> getAuctionsBySeller(uint32_t sellerID, bool activeOnly = true);
    
    /**
     * @brief Obtém leilões por licitante
     * @param bidderID ID do licitante
     * @param activeOnly Apenas ativos
     * @return Lista de IDs de leilões
     */
    std::vector<uint32_t> getAuctionsByBidder(uint32_t bidderID, bool activeOnly = true);
    
    /**
     * @brief Calcula a taxa para uma transação
     * @param type Tipo de taxa
     * @param amount Valor
     * @param characterID ID do personagem (para descontos)
     * @return Valor da taxa
     */
    uint32_t calculateFee(TradeFeeType type, uint32_t amount, uint32_t characterID = 0);
    
    /**
     * @brief Obtém a configuração de taxa
     * @param type Tipo de taxa
     * @return Configuração de taxa
     */
    FeeConfig getFeeConfig(TradeFeeType type) const;
    
    /**
     * @brief Define a configuração de taxa
     * @param config Configuração
     * @return true se definido com sucesso
     */
    bool setFeeConfig(const FeeConfig& config);
    
    /**
     * @brief Obtém o histórico de comércio
     * @param characterID ID do personagem
     * @param limit Limite (0 para todos)
     * @param offset Deslocamento
     * @return Lista de históricos
     */
    std::vector<TradeHistory> getTradeHistory(uint32_t characterID, uint32_t limit = 0, uint32_t offset = 0) const;
    
    /**
     * @brief Obtém estatísticas do mercado
     * @return Estatísticas
     */
    MarketStats getMarketStats() const;
    
    /**
     * @brief Obtém estatísticas de leilão
     * @return Estatísticas
     */
    AuctionStats getAuctionStats() const;
    
    /**
     * @brief Obtém estatísticas de comércio de um jogador
     * @param characterID ID do personagem
     * @return Estatísticas
     */
    PlayerTradeStats getPlayerTradeStats(uint32_t characterID) const;
    
    /**
     * @brief Obtém o estado do mercado
     * @return Estado
     */
    MarketState getMarketState() const;
    
    /**
     * @brief Define se o mercado está aberto
     * @param isOpen Está aberto
     */
    void setMarketOpen(bool isOpen);
    
    /**
     * @brief Define se o mercado está em manutenção
     * @param inMaintenance Está em manutenção
     */
    void setMarketInMaintenance(bool inMaintenance);
    
    /**
     * @brief Define os tempos de abertura e fechamento do mercado
     * @param openTime Tempo de abertura
     * @param closeTime Tempo de fechamento
     */
    void setMarketTimes(uint32_t openTime, uint32_t closeTime);
    
    /**
     * @brief Bane um jogador do comércio
     * @param characterID ID do personagem
     * @param banned Banido
     * @return true se alterado com sucesso
     */
    bool setTradeBanned(uint32_t characterID, bool banned);
    
    /**
     * @brief Verifica se um jogador está banido do comércio
     * @param characterID ID do personagem
     * @return true se estiver banido
     */
    bool isTradeBanned(uint32_t characterID) const;
    
    /**
     * @brief Define o limite de listagem por jogador
     * @param limit Limite
     */
    void setListingLimit(uint32_t limit);
    
    /**
     * @brief Define a duração padrão de listagem
     * @param duration Duração em segundos
     */
    void setListingDuration(uint32_t duration);
    
    /**
     * @brief Registra um callback de comércio
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerTradeCallback(TradeCallback callback);
    
    /**
     * @brief Remove um callback de comércio
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterTradeCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de mercado
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerMarketCallback(MarketCallback callback);
    
    /**
     * @brief Remove um callback de mercado
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterMarketCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de leilão
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerAuctionCallback(AuctionCallback callback);
    
    /**
     * @brief Remove um callback de leilão
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterAuctionCallback(uint32_t callbackId);
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
    /**
     * @brief Define o modo de log
     * @param logging Modo de log
     */
    void setLoggingEnabled(bool logging);
    
    /**
     * @brief Verifica se o modo de log está ativado
     * @return true se estiver ativado
     */
    bool isLoggingEnabled() const;
    
private:
    /**
     * @brief Construtor privado
     */
    TradeManager();
    
    /**
     * @brief Destrutor privado
     */
    ~TradeManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    TradeManager(const TradeManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    TradeManager& operator=(const TradeManager&) = delete;
    
    // Transações de comércio
    std::unordered_map<uint32_t, TradeTransaction> _tradeTransactions;
    std::unordered_map<uint32_t, uint32_t> _characterToTrade;
    std::atomic<uint32_t> _nextTransactionID;
    std::atomic<uint32_t> _nextTradeItemID;
    
    // Listagens de mercado
    std::unordered_map<uint32_t, MarketListing> _marketListings;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _sellerListings;
    std::unordered_map<MarketCategory, std::vector<uint32_t>> _categoryListings;
    std::atomic<uint32_t> _nextListingID;
    
    // Ofertas de mercado
    std::unordered_map<uint32_t, MarketOffer> _marketOffers;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _listingOffers;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _buyerOffers;
    std::atomic<uint32_t> _nextOfferID;
    
    // Leilões
    std::unordered_map<uint32_t, Auction> _auctions;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _sellerAuctions;
    std::unordered_map<MarketCategory, std::vector<uint32_t>> _categoryAuctions;
    std::atomic<uint32_t> _nextAuctionID;
    
    // Lances de leilão
    std::unordered_map<uint32_t, std::vector<AuctionBid>> _auctionBids;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _bidderBids;
    std::atomic<uint32_t> _nextBidID;
    
    // Histórico de comércio
    std::unordered_map<uint32_t, std::vector<TradeHistory>> _tradeHistories;
    std::vector<TradeHistory> _globalHistory;
    std::atomic<uint32_t> _nextHistoryID;
    
    // Estatísticas
    MarketStats _marketStats;
    AuctionStats _auctionStats;
    std::unordered_map<uint32_t, PlayerTradeStats> _playerStats;
    
    // Configurações
    std::unordered_map<TradeFeeType, FeeConfig> _feeConfigs;
    MarketState _marketState;
    
    // Callbacks
    std::unordered_map<uint32_t, TradeCallback> _tradeCallbacks;
    std::unordered_map<uint32_t, MarketCallback> _marketCallbacks;
    std::unordered_map<uint32_t, AuctionCallback> _auctionCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _tradeMutex;
    mutable std::mutex _marketMutex;
    mutable std::mutex _offerMutex;
    mutable std::mutex _auctionMutex;
    mutable std::mutex _bidMutex;
    mutable std::mutex _historyMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    // Timers
    uint32_t _listingExpirationTimer;
    uint32_t _auctionExpirationTimer;
    uint32_t _offerExpirationTimer;
    uint32_t _marketOpenTimer;
    
    /**
     * @brief Verifica listagens expiradas
     * @param currentTime Tempo atual
     */
    void checkExpiredListings(uint32_t currentTime);
    
    /**
     * @brief Verifica leilões expirados
     * @param currentTime Tempo atual
     */
    void checkExpiredAuctions(uint32_t currentTime);
    
    /**
     * @brief Verifica ofertas expiradas
     * @param currentTime Tempo atual
     */
    void checkExpiredOffers(uint32_t currentTime);
    
    /**
     * @brief Verifica tempos de abertura e fechamento do mercado
     * @param currentTime Tempo atual
     */
    void checkMarketOpenTimes(uint32_t currentTime);
    
    /**
     * @brief Processa uma transação de comércio
     * @param transaction Transação
     * @return Resultado da operação
     */
    TradeResult processTrade(const TradeTransaction& transaction);
    
    /**
     * @brief Processa uma compra no mercado
     * @param listing Listagem
     * @param buyerID ID do comprador
     * @param quantity Quantidade
     * @return Resultado da operação
     */
    TradeResult processMarketPurchase(const MarketListing& listing, uint32_t buyerID, uint32_t quantity);
    
    /**
     * @brief Processa uma oferta aceita
     * @param offer Oferta
     * @return Resultado da operação
     */
    TradeResult processAcceptedOffer(const MarketOffer& offer);
    
    /**
     * @brief Processa um leilão finalizado
     * @param auction Leilão
     * @return Resultado da operação
     */
    TradeResult processAuctionFinalization(const Auction& auction);
    
    /**
     * @brief Atualiza estatísticas de mercado
     * @param listing Listagem
     * @param sold Vendido
     */
    void updateMarketStats(const MarketListing& listing, bool sold);
    
    /**
     * @brief Atualiza estatísticas de leilão
     * @param auction Leilão
     * @param completed Completo
     */
    void updateAuctionStats(const Auction& auction, bool completed);
    
    /**
     * @brief Atualiza estatísticas de jogador
     * @param characterID ID do personagem
     * @param isSeller É vendedor
     * @param amount Valor
     * @param feeAmount Valor da taxa
     * @param isAuction É leilão
     * @param isWin É vitória
     */
    void updatePlayerStats(uint32_t characterID, bool isSeller, uint32_t amount, uint32_t feeAmount, bool isAuction, bool isWin);
    
    /**
     * @brief Adiciona histórico de comércio
     * @param history Histórico
     * @return ID do histórico
     */
    uint32_t addTradeHistory(const TradeHistory& history);
    
    /**
     * @brief Executa callbacks de comércio
     * @param transaction Transação
     * @return Resultado da operação
     */
    TradeResult executeTradeCallbacks(const TradeTransaction& transaction);
    
    /**
     * @brief Executa callbacks de mercado
     * @param listing Listagem
     * @return true se processado com sucesso
     */
    bool executeMarketCallbacks(const MarketListing& listing);
    
    /**
     * @brief Executa callbacks de leilão
     * @param auction Leilão
     * @return true se processado com sucesso
     */
    bool executeAuctionCallbacks(const Auction& auction);
    
    /**
     * @brief Valida uma transação de comércio
     * @param transaction Transação
     * @return Resultado da operação
     */
    TradeResult validateTrade(const TradeTransaction& transaction);
    
    /**
     * @brief Valida uma listagem de mercado
     * @param listing Listagem
     * @param sellerID ID do vendedor
     * @return true se válida
     */
    bool validateMarketListing(const MarketListing& listing, uint32_t sellerID);
    
    /**
     * @brief Valida um leilão
     * @param auction Leilão
     * @param sellerID ID do vendedor
     * @return true se válido
     */
    bool validateAuction(const Auction& auction, uint32_t sellerID);
    
    /**
     * @brief Valida um lance
     * @param auction Leilão
     * @param bidderID ID do licitante
     * @param bidAmount Valor do lance
     * @return true se válido
     */
    bool validateBid(const Auction& auction, uint32_t bidderID, uint32_t bidAmount);
    
    /**
     * @brief Carrega dados de comércio do banco de dados
     * @return true se carregado com sucesso
     */
    bool loadTradeFromDatabase();
    
    /**
     * @brief Salva dados de comércio no banco de dados
     * @return true se salvo com sucesso
     */
    bool saveTradeToDatabase();
};

} // namespace trade
} // namespace wydbr

#endif // _TRADE_MANAGER_H_