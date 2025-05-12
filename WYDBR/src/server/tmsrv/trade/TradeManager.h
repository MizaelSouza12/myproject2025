/**
 * TradeManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/trade/TradeManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef TRADEMANAGER_H
#define TRADEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <set>
#include <variant>
#include <optional>
#include <queue>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <deque>

#include "../../../core/WYDTypes.h"
#include "./types/TradeTypes.h"
#include "../player/CPlayer.h"
#include "../kernel/persistence/PersistenceManager.h"

namespace wydbr {


/**
 * @file TradeManager.h
 * @brief Sistema avançado de gerenciamento de comércio e economia
 * 
 * Este arquivo contém a implementação do sistema de gerenciamento de comércio,
 * que suporta mercados dinâmicos, leilões, comércio entre jogadores, lojas NPC,
 * sistema monetário com múltiplas moedas, controle de inflação e economia dinâmica.
 */

namespace wyd {
namespace server {
namespace trade {

/**
 * @brief Configuração do gerenciador de comércio
 */
struct TradeManagerConfig {
    bool autosave;                  // Autosave de dados
    DWORD autosaveInterval;         // Intervalo de autosave (ms)
    bool asyncProcessing;           // Processamento assíncrono
    float marketTaxRate;            // Taxa de imposto do mercado
    float auctionTaxRate;           // Taxa de imposto do leilão
    bool enableAuctionHouse;        // Habilitar casa de leilão
    bool enableMarketplace;         // Habilitar mercado
    bool enableTradeRoutes;         // Habilitar rotas de comércio
    bool enablePriceHistory;        // Habilitar histórico de preços
    DWORD maxMarketListingDuration; // Duração máxima de listagem de mercado (horas)
    DWORD maxAuctionDuration;       // Duração máxima de leilão (horas)
    DWORD minAuctionBidIncrement;   // Incremento mínimo de lance de leilão (%)
    DWORD maxCurrencyPerTransaction;// Máximo de moeda por transação
    bool enableInflationControl;    // Habilitar controle de inflação
    float defaultInflationRate;     // Taxa de inflação padrão
    DWORD economicEventInterval;    // Intervalo de evento econômico (horas)
    DWORD shopRestockInterval;      // Intervalo de reabastecimento de loja (minutos)
    bool enableTradeLog;            // Habilitar registro de comércio
    bool enableTelemetry;           // Habilitar telemetria
    bool enableSeasonal;            // Habilitar eventos sazonais
    
    TradeManagerConfig()
        : autosave(true)
        , autosaveInterval(300000)
        , asyncProcessing(true)
        , marketTaxRate(0.05f)
        , auctionTaxRate(0.10f)
        , enableAuctionHouse(true)
        , enableMarketplace(true)
        , enableTradeRoutes(true)
        , enablePriceHistory(true)
        , maxMarketListingDuration(168)
        , maxAuctionDuration(72)
        , minAuctionBidIncrement(5)
        , maxCurrencyPerTransaction(1000000000)
        , enableInflationControl(true)
        , defaultInflationRate(0.02f)
        , economicEventInterval(24)
        , shopRestockInterval(60)
        , enableTradeLog(true)
        , enableTelemetry(true)
        , enableSeasonal(true)
    {
    }
};

/**
 * @brief Estatísticas do gerenciador de comércio
 */
struct TradeManagerStats {
    DWORD activeMarketListings;     // Listagens de mercado ativas
    DWORD activeAuctions;           // Leilões ativos
    DWORD dailyTransactionCount;    // Contagem de transações diárias
    DWORD dailyTransactionVolume;   // Volume de transações diárias
    float averageMarketPrice;       // Preço médio de mercado
    DWORD totalGoldTraded;          // Total de ouro comercializado
    DWORD totalItemsTraded;         // Total de itens comercializados
    DWORD activeTraders;            // Comerciantes ativos
    float inflationRate;            // Taxa de inflação
    DWORD economicEventsTriggered;  // Eventos econômicos desencadeados
    
    TradeManagerStats()
        : activeMarketListings(0)
        , activeAuctions(0)
        , dailyTransactionCount(0)
        , dailyTransactionVolume(0)
        , averageMarketPrice(0.0f)
        , totalGoldTraded(0)
        , totalItemsTraded(0)
        , activeTraders(0)
        , inflationRate(0.0f)
        , economicEventsTriggered(0)
    {
    }
};

/**
 * @brief Resultado de transação de mercado
 */
struct MarketTransactionResult {
    bool success;                   // Sucesso
    DWORD transactionID;            // ID da transação
    DWORD amount;                   // Valor
    DWORD tax;                      // Imposto
    std::string message;            // Mensagem
    TransactionStatus status;       // Status
    
    MarketTransactionResult()
        : success(false)
        , transactionID(0)
        , amount(0)
        , tax(0)
        , message("")
        , status(TransactionStatus::FAILED)
    {
    }
};

/**
 * @brief Resultado de lance de leilão
 */
struct AuctionBidResult {
    bool success;                   // Sucesso
    DWORD bidID;                    // ID do lance
    DWORD amount;                   // Valor
    std::string message;            // Mensagem
    DWORD outbidPlayerID;           // ID do jogador superado
    bool isBuyout;                  // Se é compra imediata
    
    AuctionBidResult()
        : success(false)
        , bidID(0)
        , amount(0)
        , message("")
        , outbidPlayerID(0)
        , isBuyout(false)
    {
    }
};

/**
 * @brief Resultado de troca entre jogadores
 */
struct PlayerTradeResult {
    bool success;                   // Sucesso
    DWORD tradeID;                  // ID da troca
    std::string message;            // Mensagem
    bool bothConfirmed;             // Ambos confirmaram
    std::vector<DWORD> itemsTraded; // Itens trocados
    
    PlayerTradeResult()
        : success(false)
        , tradeID(0)
        , message("")
        , bothConfirmed(false)
    {
    }
};

/**
 * @brief Evento econômico atual
 */
struct CurrentEconomicEvent {
    DWORD eventID;                  // ID do evento
    EconomicEventType type;         // Tipo
    std::string name;               // Nome
    time_t startTime;               // Hora de início
    time_t endTime;                 // Hora de término
    float impactFactor;             // Fator de impacto
    DWORD zoneID;                   // ID da zona
    bool global;                    // Global
    
    CurrentEconomicEvent()
        : eventID(0)
        , type(EconomicEventType::INFLATION)
        , name("")
        , startTime(0)
        , endTime(0)
        , impactFactor(0.0f)
        , zoneID(0)
        , global(false)
    {
    }
};

/**
 * @brief Gerenciador de comércio
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
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static TradeManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const TradeManagerConfig& config = TradeManagerConfig());
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Cria uma listagem de mercado
     * @param sellerID ID do vendedor
     * @param itemID ID do item
     * @param quantity Quantidade
     * @param unitPrice Preço unitário
     * @param type Tipo
     * @param currencyType Tipo de moeda
     * @param duration Duração (horas)
     * @param description Descrição
     * @param category Categoria
     * @param errorReason Razão do erro (saída)
     * @return ID da listagem, ou 0 se falhar
     */
    DWORD CreateMarketListing(DWORD sellerID, DWORD itemID, DWORD quantity, DWORD unitPrice, 
                            MarketListingType type, CurrencyType currencyType, DWORD duration, 
                            const std::string& description, TradeCategoryType category, 
                            std::string& errorReason);
    
    /**
     * @brief Obtém listagens de mercado
     * @param type Tipo (MarketListingType::SELL para todos)
     * @param category Categoria (TradeCategoryType::WEAPON para todos)
     * @param status Status (MarketListingStatus::ACTIVE para ativos)
     * @param sellerID ID do vendedor (0 para todos)
     * @return Lista de listagens
     */
    std::vector<MarketListing> GetMarketListings(MarketListingType type = MarketListingType::SELL, 
                                              TradeCategoryType category = TradeCategoryType::WEAPON, 
                                              MarketListingStatus status = MarketListingStatus::ACTIVE, 
                                              DWORD sellerID = 0) const;
    
    /**
     * @brief Compra um item do mercado
     * @param listingID ID da listagem
     * @param buyerID ID do comprador
     * @param quantity Quantidade
     * @param errorReason Razão do erro (saída)
     * @return Resultado da transação
     */
    MarketTransactionResult BuyMarketItem(DWORD listingID, DWORD buyerID, DWORD quantity, std::string& errorReason);
    
    /**
     * @brief Cancela uma listagem de mercado
     * @param listingID ID da listagem
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelMarketListing(DWORD listingID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Cria um leilão
     * @param sellerID ID do vendedor
     * @param itemID ID do item
     * @param quantity Quantidade
     * @param startingBid Lance inicial
     * @param buyoutPrice Preço de compra imediata
     * @param currencyType Tipo de moeda
     * @param duration Duração (horas)
     * @param description Descrição
     * @param category Categoria
     * @param type Tipo
     * @param errorReason Razão do erro (saída)
     * @return ID do leilão, ou 0 se falhar
     */
    DWORD CreateAuction(DWORD sellerID, DWORD itemID, DWORD quantity, DWORD startingBid, 
                       DWORD buyoutPrice, CurrencyType currencyType, DWORD duration, 
                       const std::string& description, TradeCategoryType category, 
                       AuctionType type, std::string& errorReason);
    
    /**
     * @brief Obtém leilões
     * @param type Tipo (AuctionType::STANDARD para todos)
     * @param category Categoria (TradeCategoryType::WEAPON para todos)
     * @param status Status (AuctionStatus::ACTIVE para ativos)
     * @param sellerID ID do vendedor (0 para todos)
     * @return Lista de leilões
     */
    std::vector<AuctionItem> GetAuctions(AuctionType type = AuctionType::STANDARD, 
                                       TradeCategoryType category = TradeCategoryType::WEAPON, 
                                       AuctionStatus status = AuctionStatus::ACTIVE, 
                                       DWORD sellerID = 0) const;
    
    /**
     * @brief Coloca um lance em leilão
     * @param auctionID ID do leilão
     * @param bidderID ID do licitante
     * @param bidAmount Valor do lance
     * @param isBuyout Se é compra imediata
     * @param autoRebid Se pode dar lance automático
     * @param maxAutoBid Lance automático máximo
     * @param errorReason Razão do erro (saída)
     * @return Resultado do lance
     */
    AuctionBidResult PlaceAuctionBid(DWORD auctionID, DWORD bidderID, DWORD bidAmount, 
                                  bool isBuyout, bool autoRebid, DWORD maxAutoBid, 
                                  std::string& errorReason);
    
    /**
     * @brief Obtém lances de um leilão
     * @param auctionID ID do leilão
     * @return Lista de lances
     */
    std::vector<AuctionBid> GetAuctionBids(DWORD auctionID) const;
    
    /**
     * @brief Cancela um leilão
     * @param auctionID ID do leilão
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelAuction(DWORD auctionID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Inicia uma troca entre jogadores
     * @param player1ID ID do jogador 1
     * @param player2ID ID do jogador 2
     * @param errorReason Razão do erro (saída)
     * @return ID da troca, ou 0 se falhar
     */
    DWORD StartPlayerTrade(DWORD player1ID, DWORD player2ID, std::string& errorReason);
    
    /**
     * @brief Adiciona um item a uma troca
     * @param tradeID ID da troca
     * @param playerID ID do jogador
     * @param itemID ID do item
     * @param quantity Quantidade
     * @param slot Slot
     * @param errorReason Razão do erro (saída)
     * @return true se adicionado com sucesso
     */
    bool AddItemToTrade(DWORD tradeID, DWORD playerID, DWORD itemID, DWORD quantity, 
                       BYTE slot, std::string& errorReason);
    
    /**
     * @brief Remove um item de uma troca
     * @param tradeID ID da troca
     * @param playerID ID do jogador
     * @param slot Slot
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveItemFromTrade(DWORD tradeID, DWORD playerID, BYTE slot, std::string& errorReason);
    
    /**
     * @brief Define ouro para uma troca
     * @param tradeID ID da troca
     * @param playerID ID do jogador
     * @param amount Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetTradeGold(DWORD tradeID, DWORD playerID, DWORD amount, std::string& errorReason);
    
    /**
     * @brief Confirma uma oferta de troca
     * @param tradeID ID da troca
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return Resultado da troca
     */
    PlayerTradeResult ConfirmTradeOffer(DWORD tradeID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Cancela uma troca
     * @param tradeID ID da troca
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelTrade(DWORD tradeID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Obtém itens de uma loja NPC
     * @param shopID ID da loja
     * @return Lista de itens
     */
    std::vector<NPCShopItem> GetNPCShopItems(DWORD shopID) const;
    
    /**
     * @brief Compra um item de uma loja NPC
     * @param shopID ID da loja
     * @param shopItemID ID do item da loja
     * @param buyerID ID do comprador
     * @param quantity Quantidade
     * @param errorReason Razão do erro (saída)
     * @return Resultado da transação
     */
    MarketTransactionResult BuyFromNPCShop(DWORD shopID, DWORD shopItemID, DWORD buyerID, 
                                        DWORD quantity, std::string& errorReason);
    
    /**
     * @brief Vende um item para uma loja NPC
     * @param shopID ID da loja
     * @param sellerID ID do vendedor
     * @param itemID ID do item
     * @param quantity Quantidade
     * @param errorReason Razão do erro (saída)
     * @return Resultado da transação
     */
    MarketTransactionResult SellToNPCShop(DWORD shopID, DWORD sellerID, DWORD itemID, 
                                       DWORD quantity, std::string& errorReason);
    
    /**
     * @brief Obtém histórico de preço de um item
     * @param itemID ID do item
     * @param quality Qualidade
     * @param daysBack Dias para trás
     * @return Lista de históricos
     */
    std::vector<ItemPriceHistory> GetItemPriceHistory(DWORD itemID, BYTE quality, DWORD daysBack = 30) const;
    
    /**
     * @brief Obtém transações recentes
     * @param playerID ID do jogador (0 para todos)
     * @param type Tipo (TransactionType::MARKET_PURCHASE para todos)
     * @param count Contagem
     * @return Lista de transações
     */
    std::vector<Transaction> GetRecentTransactions(DWORD playerID = 0, 
                                                TransactionType type = TransactionType::MARKET_PURCHASE, 
                                                size_t count = 100) const;
    
    /**
     * @brief Obtém rotas de comércio
     * @param startZoneID ID da zona inicial (0 para todos)
     * @param endZoneID ID da zona final (0 para todos)
     * @param status Status (TradeRouteStatus::OPEN para abertas)
     * @return Lista de rotas
     */
    std::vector<TradeRoute> GetTradeRoutes(DWORD startZoneID = 0, DWORD endZoneID = 0, 
                                        TradeRouteStatus status = TradeRouteStatus::OPEN) const;
    
    /**
     * @brief Cria uma rota de comércio
     * @param playerID ID do jogador
     * @param startZoneID ID da zona inicial
     * @param endZoneID ID da zona final
     * @param type Tipo
     * @param name Nome
     * @param errorReason Razão do erro (saída)
     * @return ID da rota, ou 0 se falhar
     */
    DWORD CreateTradeRoute(DWORD playerID, DWORD startZoneID, DWORD endZoneID, 
                         TradeRouteType type, const std::string& name, std::string& errorReason);
    
    /**
     * @brief Define o status de uma rota de comércio
     * @param routeID ID da rota
     * @param status Status
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se definido com sucesso
     */
    bool SetTradeRouteStatus(DWORD routeID, TradeRouteStatus status, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Obtém eventos econômicos ativos
     * @param zoneID ID da zona (0 para todos)
     * @return Lista de eventos
     */
    std::vector<EconomicEvent> GetActiveEconomicEvents(DWORD zoneID = 0) const;
    
    /**
     * @brief Cria um evento econômico
     * @param type Tipo
     * @param name Nome
     * @param description Descrição
     * @param duration Duração (horas)
     * @param impactFactor Fator de impacto
     * @param zoneID ID da zona
     * @param global Se é global
     * @param categoryImpacts Impactos por categoria
     * @param itemImpacts Impactos por item
     * @param errorReason Razão do erro (saída)
     * @return ID do evento, ou 0 se falhar
     */
    DWORD CreateEconomicEvent(EconomicEventType type, const std::string& name, 
                            const std::string& description, DWORD duration, 
                            float impactFactor, DWORD zoneID, bool global, 
                            const std::vector<std::pair<TradeCategoryType, float>>& categoryImpacts, 
                            const std::vector<std::pair<DWORD, float>>& itemImpacts, 
                            std::string& errorReason);
    
    /**
     * @brief Encerra um evento econômico
     * @param eventID ID do evento
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se encerrado com sucesso
     */
    bool EndEconomicEvent(DWORD eventID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Cria uma regulação de mercado
     * @param type Tipo
     * @param itemID ID do item
     * @param category Categoria
     * @param value Valor
     * @param duration Duração (horas)
     * @param factionID ID da facção
     * @param zoneID ID da zona
     * @param description Descrição
     * @param errorReason Razão do erro (saída)
     * @return ID da regulação, ou 0 se falhar
     */
    DWORD CreateMarketRegulation(MarketRegulationType type, DWORD itemID, 
                               TradeCategoryType category, float value, 
                               DWORD duration, DWORD factionID, DWORD zoneID, 
                               const std::string& description, std::string& errorReason);
    
    /**
     * @brief Obtém regulações de mercado
     * @param type Tipo (MarketRegulationType::TAX_RATE para todos)
     * @param itemID ID do item (0 para todos)
     * @param zoneID ID da zona (0 para todos)
     * @return Lista de regulações
     */
    std::vector<MarketRegulation> GetMarketRegulations(MarketRegulationType type = MarketRegulationType::TAX_RATE, 
                                                    DWORD itemID = 0, DWORD zoneID = 0) const;
    
    /**
     * @brief Obtém o sistema econômico
     * @return Sistema econômico
     */
    EconomicSystem GetEconomicSystem() const;
    
    /**
     * @brief Verifica se uma transação é válida
     * @param senderID ID do remetente
     * @param receiverID ID do destinatário
     * @param currencyType Tipo de moeda
     * @param amount Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se válido
     */
    bool IsTransactionValid(DWORD senderID, DWORD receiverID, CurrencyType currencyType, 
                          DWORD amount, std::string& errorReason) const;
    
    /**
     * @brief Executa uma transferência de moeda
     * @param senderID ID do remetente
     * @param receiverID ID do destinatário
     * @param currencyType Tipo de moeda
     * @param amount Quantidade
     * @param type Tipo
     * @param referenceID ID de referência
     * @param notes Notas
     * @param errorReason Razão do erro (saída)
     * @return Resultado da transação
     */
    MarketTransactionResult ExecuteCurrencyTransfer(DWORD senderID, DWORD receiverID, 
                                                 CurrencyType currencyType, DWORD amount, 
                                                 TransactionType type, DWORD referenceID, 
                                                 const std::string& notes, std::string& errorReason);
    
    /**
     * @brief Obtém as estatísticas do gerenciador
     * @return Estatísticas
     */
    TradeManagerStats GetStatistics() const;
    
    /**
     * @brief Registra um callback para eventos de transação
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterTransactionEventCallback(TransactionEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterTransactionEventCallback(int callbackID);
    
    /**
     * @brief Registra um callback para eventos de mercado
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterMarketEventCallback(MarketEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterMarketEventCallback(int callbackID);
    
    /**
     * @brief Registra um callback para eventos de leilão
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterAuctionEventCallback(AuctionEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterAuctionEventCallback(int callbackID);
    
    /**
     * @brief Registra um callback para eventos econômicos
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterEconomicEventCallback(EconomicEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterEconomicEventCallback(int callbackID);
    
    /**
     * @brief Salva dados de comércio
     * @return true se salvo com sucesso
     */
    bool SaveTradeData();
    
    /**
     * @brief Carrega dados de comércio
     * @return true se carregado com sucesso
     */
    bool LoadTradeData();
    
    /**
     * @brief Verifica e processa listagens de mercado expiradas
     */
    void ProcessExpiredMarketListings();
    
    /**
     * @brief Verifica e processa leilões expirados
     */
    void ProcessExpiredAuctions();
    
    /**
     * @brief Verifica e processa trocas inativas
     */
    void ProcessInactiveTrades();
    
    /**
     * @brief Atualiza inventário de lojas NPC
     */
    void UpdateNPCShopInventory();
    
    /**
     * @brief Gera histórico de preços
     */
    void GeneratePriceHistory();
    
    /**
     * @brief Verifica e processa eventos econômicos
     */
    void ProcessEconomicEvents();
    
    /**
     * @brief Verifica e processa regulações de mercado
     */
    void ProcessMarketRegulations();
    
    /**
     * @brief Calcula e atualiza a inflação
     */
    void CalculateAndUpdateInflation();
    
    /**
     * @brief Obtém o preço atual de um item
     * @param itemID ID do item
     * @param quality Qualidade
     * @param currencyType Tipo de moeda
     * @return Preço, ou 0 se não disponível
     */
    DWORD GetCurrentItemPrice(DWORD itemID, BYTE quality, CurrencyType currencyType) const;
    
    /**
     * @brief Aplica modificadores de preço
     * @param basePrice Preço base
     * @param itemID ID do item
     * @param category Categoria
     * @param zoneID ID da zona
     * @return Preço modificado
     */
    DWORD ApplyPriceModifiers(DWORD basePrice, DWORD itemID, TradeCategoryType category, DWORD zoneID) const;
    
    /**
     * @brief Obtém vendedores de um item
     * @param itemID ID do item
     * @param quality Qualidade
     * @return Lista de listagens
     */
    std::vector<MarketListing> GetItemSellers(DWORD itemID, BYTE quality) const;
    
    /**
     * @brief Obtém compradores de um item
     * @param itemID ID do item
     * @param quality Qualidade
     * @return Lista de listagens
     */
    std::vector<MarketListing> GetItemBuyers(DWORD itemID, BYTE quality) const;
    
    /**
     * @brief Renova uma listagem de mercado
     * @param listingID ID da listagem
     * @param playerID ID do jogador
     * @param duration Duração (horas)
     * @param errorReason Razão do erro (saída)
     * @return true se renovado com sucesso
     */
    bool RenewMarketListing(DWORD listingID, DWORD playerID, DWORD duration, std::string& errorReason);
    
    /**
     * @brief Obtém facções de comércio
     * @return Lista de facções
     */
    std::vector<TradeFaction> GetTradeFactions() const;
    
    /**
     * @brief Inicia uma transação segura
     * @param playerID1 ID do jogador 1
     * @param playerID2 ID do jogador 2
     * @param errorReason Razão do erro (saída)
     * @return ID da transação, ou 0 se falhar
     */
    DWORD StartSecureTransaction(DWORD playerID1, DWORD playerID2, std::string& errorReason);
    
    /**
     * @brief Verifica trocas em progresso para um jogador
     * @param playerID ID do jogador
     * @return true se estiver em uma troca
     */
    bool IsPlayerInTrade(DWORD playerID) const;
    
    /**
     * @brief Obtém evento econômico atual
     * @return Evento econômico atual
     */
    CurrentEconomicEvent GetCurrentEconomicEvent() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    TradeManager();
    
    /**
     * @brief Destrutor
     */
    ~TradeManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    TradeManager(const TradeManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    TradeManager& operator=(const TradeManager&) = delete;
    
    /**
     * @brief Thread de autosave
     */
    void AutosaveThread();
    
    /**
     * @brief Thread de economia
     */
    void EconomyThread();
    
    /**
     * @brief Thread de limpeza
     */
    void CleanupThread();
    
    /**
     * @brief Notifica callbacks de evento de transação
     * @param transactionID ID da transação
     * @param type Tipo
     * @param senderID ID do remetente
     * @param receiverID ID do destinatário
     * @param status Status
     */
    void NotifyTransactionCallbacks(DWORD transactionID, TransactionType type, DWORD senderID, DWORD receiverID, TransactionStatus status);
    
    /**
     * @brief Notifica callbacks de evento de mercado
     * @param listingID ID da listagem
     * @param type Tipo
     * @param sellerID ID do vendedor
     * @param buyerID ID do comprador
     * @param status Status
     */
    void NotifyMarketCallbacks(DWORD listingID, MarketListingType type, DWORD sellerID, DWORD buyerID, MarketListingStatus status);
    
    /**
     * @brief Notifica callbacks de evento de leilão
     * @param auctionID ID do leilão
     * @param type Tipo
     * @param sellerID ID do vendedor
     * @param bidderID ID do licitante
     * @param status Status
     */
    void NotifyAuctionCallbacks(DWORD auctionID, AuctionType type, DWORD sellerID, DWORD bidderID, AuctionStatus status);
    
    /**
     * @brief Notifica callbacks de evento econômico
     * @param eventID ID do evento
     * @param type Tipo
     * @param startTime Hora de início
     * @param endTime Hora de término
     * @param impactFactor Fator de impacto
     */
    void NotifyEconomicEventCallbacks(DWORD eventID, EconomicEventType type, time_t startTime, time_t endTime, float impactFactor);
    
    /**
     * @brief Gera o preço de mercado para um item
     * @param itemID ID do item
     * @param quality Qualidade
     * @param currencyType Tipo de moeda
     * @return Preço gerado
     */
    DWORD GenerateMarketPrice(DWORD itemID, BYTE quality, CurrencyType currencyType) const;
    
    /**
     * @brief Obtém o nome de um jogador
     * @param playerID ID do jogador
     * @return Nome do jogador
     */
    std::string GetPlayerName(DWORD playerID) const;
    
    /**
     * @brief Carrega listagens de mercado do banco de dados
     * @param type Tipo
     * @param category Categoria
     * @param status Status
     * @param sellerID ID do vendedor
     * @return Lista de listagens
     */
    std::vector<MarketListing> LoadMarketListingsFromDB(MarketListingType type, TradeCategoryType category, MarketListingStatus status, DWORD sellerID) const;
    
    /**
     * @brief Carrega leilões do banco de dados
     * @param type Tipo
     * @param category Categoria
     * @param status Status
     * @param sellerID ID do vendedor
     * @return Lista de leilões
     */
    std::vector<AuctionItem> LoadAuctionsFromDB(AuctionType type, TradeCategoryType category, AuctionStatus status, DWORD sellerID) const;
    
    /**
     * @brief Carrega lances de leilão do banco de dados
     * @param auctionID ID do leilão
     * @return Lista de lances
     */
    std::vector<AuctionBid> LoadAuctionBidsFromDB(DWORD auctionID) const;
    
    /**
     * @brief Carrega itens de loja NPC do banco de dados
     * @param shopID ID da loja
     * @return Lista de itens
     */
    std::vector<NPCShopItem> LoadNPCShopItemsFromDB(DWORD shopID) const;
    
    /**
     * @brief Carrega histórico de preço do banco de dados
     * @param itemID ID do item
     * @param quality Qualidade
     * @param daysBack Dias para trás
     * @return Lista de históricos
     */
    std::vector<ItemPriceHistory> LoadItemPriceHistoryFromDB(DWORD itemID, BYTE quality, DWORD daysBack) const;
    
    /**
     * @brief Carrega transações do banco de dados
     * @param playerID ID do jogador
     * @param type Tipo
     * @param count Contagem
     * @return Lista de transações
     */
    std::vector<Transaction> LoadTransactionsFromDB(DWORD playerID, TransactionType type, size_t count) const;
    
    /**
     * @brief Carrega rotas de comércio do banco de dados
     * @param startZoneID ID da zona inicial
     * @param endZoneID ID da zona final
     * @param status Status
     * @return Lista de rotas
     */
    std::vector<TradeRoute> LoadTradeRoutesFromDB(DWORD startZoneID, DWORD endZoneID, TradeRouteStatus status) const;
    
    /**
     * @brief Carrega eventos econômicos do banco de dados
     * @param zoneID ID da zona
     * @return Lista de eventos
     */
    std::vector<EconomicEvent> LoadEconomicEventsFromDB(DWORD zoneID) const;
    
    /**
     * @brief Carrega regulações de mercado do banco de dados
     * @param type Tipo
     * @param itemID ID do item
     * @param zoneID ID da zona
     * @return Lista de regulações
     */
    std::vector<MarketRegulation> LoadMarketRegulationsFromDB(MarketRegulationType type, DWORD itemID, DWORD zoneID) const;
    
    /**
     * @brief Carrega sistema econômico do banco de dados
     * @return Sistema econômico
     */
    EconomicSystem LoadEconomicSystemFromDB() const;
    
    /**
     * @brief Carrega facções de comércio do banco de dados
     * @return Lista de facções
     */
    std::vector<TradeFaction> LoadTradeFactionsFromDB() const;
    
    /**
     * @brief Registro de transação
     * @param type Tipo
     * @param buyerID ID do comprador
     * @param sellerID ID do vendedor
     * @param itemID ID do item
     * @param quantity Quantidade
     * @param amount Valor
     * @param currencyType Tipo de moeda
     * @param referenceID ID de referência
     * @param notes Notas
     * @param feeAmount Valor da taxa
     * @return ID da transação, ou 0 se falhar
     */
    DWORD LogTransaction(TransactionType type, DWORD buyerID, DWORD sellerID, DWORD itemID, 
                     DWORD quantity, DWORD amount, CurrencyType currencyType, DWORD referenceID, 
                     const std::string& notes, DWORD feeAmount);
    
    /**
     * @brief Adiciona uma tarefa assíncrona à fila
     * @param task Tarefa
     */
    void AddAsyncTask(std::function<void()> task);
    
    /**
     * @brief Atualiza estatísticas
     */
    void UpdateStatistics();
    
    /**
     * @brief Calcula taxa para uma transação
     * @param amount Valor
     * @param type Tipo
     * @return Valor da taxa
     */
    DWORD CalculateTransactionFee(DWORD amount, TransactionType type) const;
    
    /**
     * @brief Encerra uma troca entre jogadores
     * @param tradeID ID da troca
     * @param status Status
     * @param reason Razão
     */
    void EndPlayerTrade(DWORD tradeID, TransactionStatus status, const std::string& reason);
    
    /**
     * @brief Processa troca entre jogadores
     * @param tradeID ID da troca
     * @return Resultado da troca
     */
    PlayerTradeResult ProcessPlayerTrade(DWORD tradeID);
    
    // Configuração
    TradeManagerConfig config_;
    
    // Listagens de mercado
    std::unordered_map<DWORD, MarketListing> marketListings_;
    mutable std::shared_mutex marketListingsMutex_;
    
    // Leilões
    std::unordered_map<DWORD, AuctionItem> auctions_;
    mutable std::shared_mutex auctionsMutex_;
    
    // Lances de leilão
    std::unordered_map<DWORD, std::vector<AuctionBid>> auctionBids_;
    mutable std::shared_mutex auctionBidsMutex_;
    
    // Trocas entre jogadores
    std::unordered_map<DWORD, PlayerTrade> playerTrades_;
    mutable std::shared_mutex playerTradesMutex_;
    
    // Itens de troca
    std::unordered_map<DWORD, std::vector<PlayerTradeItem>> tradeItems_;
    mutable std::shared_mutex tradeItemsMutex_;
    
    // Lojas NPC
    std::unordered_map<DWORD, NPCShop> npcShops_;
    mutable std::shared_mutex npcShopsMutex_;
    
    // Itens de loja NPC
    std::unordered_map<DWORD, std::vector<NPCShopItem>> npcShopItems_;
    mutable std::shared_mutex npcShopItemsMutex_;
    
    // Histórico de preço
    std::unordered_map<DWORD, std::unordered_map<BYTE, std::deque<ItemPriceHistory>>> priceHistory_;
    mutable std::shared_mutex priceHistoryMutex_;
    
    // Transações
    std::vector<Transaction> transactions_;
    mutable std::shared_mutex transactionsMutex_;
    
    // Rotas de comércio
    std::unordered_map<DWORD, TradeRoute> tradeRoutes_;
    mutable std::shared_mutex tradeRoutesMutex_;
    
    // Eventos econômicos
    std::unordered_map<DWORD, EconomicEvent> economicEvents_;
    mutable std::shared_mutex economicEventsMutex_;
    
    // Regulações de mercado
    std::unordered_map<DWORD, MarketRegulation> marketRegulations_;
    mutable std::shared_mutex marketRegulationsMutex_;
    
    // Sistema econômico
    EconomicSystem economicSystem_;
    mutable std::shared_mutex economicSystemMutex_;
    
    // Facções de comércio
    std::unordered_map<DWORD, TradeFaction> tradeFactions_;
    mutable std::shared_mutex tradeFactionsMutex_;
    
    // Índices
    std::unordered_map<DWORD, std::vector<DWORD>> sellerListings_;
    std::unordered_map<DWORD, std::vector<DWORD>> sellerAuctions_;
    std::unordered_map<DWORD, std::vector<DWORD>> itemListings_;
    std::unordered_map<DWORD, std::vector<DWORD>> itemAuctions_;
    std::unordered_map<DWORD, DWORD> playerActiveTrades_;
    mutable std::shared_mutex indicesMutex_;
    
    // Callbacks
    std::map<int, TransactionEventCallback> transactionCallbacks_;
    std::mutex transactionCallbacksMutex_;
    int nextTransactionCallbackID_;
    
    std::map<int, MarketEventCallback> marketCallbacks_;
    std::mutex marketCallbacksMutex_;
    int nextMarketCallbackID_;
    
    std::map<int, AuctionEventCallback> auctionCallbacks_;
    std::mutex auctionCallbacksMutex_;
    int nextAuctionCallbackID_;
    
    std::map<int, EconomicEventCallback> economicEventCallbacks_;
    std::mutex economicEventCallbacksMutex_;
    int nextEconomicEventCallbackID_;
    
    // Threads
    std::thread autosaveThread_;
    std::thread economyThread_;
    std::thread cleanupThread_;
    std::atomic<bool> running_;
    
    // Tarefas assíncronas
    std::queue<std::function<void()>> asyncTasks_;
    std::mutex asyncTasksMutex_;
    std::condition_variable asyncTasksCV_;
    
    // Estatísticas
    TradeManagerStats stats_;
    mutable std::mutex statsMutex_;
    
    // Estado
    bool initialized_;
    time_t lastAutosaveTime_;
    time_t lastExpiredListingsCheckTime_;
    time_t lastExpiredAuctionsCheckTime_;
    time_t lastInactiveTradesCheckTime_;
    time_t lastNPCShopUpdateTime_;
    time_t lastPriceHistoryGenerationTime_;
    time_t lastEconomicEventCheckTime_;
    time_t lastMarketRegulationCheckTime_;
    time_t lastInflationUpdateTime_;
    
    // Evento econômico atual
    CurrentEconomicEvent currentEconomicEvent_;
    
    // IDs únicos
    std::atomic<DWORD> nextListingID_;
    std::atomic<DWORD> nextAuctionID_;
    std::atomic<DWORD> nextBidID_;
    std::atomic<DWORD> nextTradeID_;
    std::atomic<DWORD> nextTransactionID_;
    std::atomic<DWORD> nextRouteID_;
    std::atomic<DWORD> nextEventID_;
    std::atomic<DWORD> nextRegulationID_;
};

// Acesso global
#define g_TradeManager TradeManager::GetInstance()

} // namespace trade
} // namespace server
} // namespace wyd

#endif // TRADEMANAGER_H

} // namespace wydbr
