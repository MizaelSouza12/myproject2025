/**
 * MarketplaceManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/economy/marketplace/MarketplaceManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef MARKETPLACEMANAGER_H
#define MARKETPLACEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>

#include "../../../../core/WYDTypes.h"
#include "../EconomyTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file MarketplaceManager.h
 * @brief Gerenciador do sistema de marketplace
 * 
 * Este arquivo contém a definição do gerenciador do marketplace, responsável
 * por todas as listagens, transações, leilões e operações relacionadas ao
 * marketplace do jogo. O sistema corrige as limitações do sistema original do WYD,
 * oferecendo funcionalidades avançadas de mercado entre jogadores.
 */

namespace wyd {
namespace server {
namespace economy {
namespace marketplace {

/**
 * @brief Resultado de uma busca no marketplace
 */
struct MarketSearchResult {
    std::vector<MarketListing> directSales;  // Vendas diretas
    std::vector<Auction> auctions;           // Leilões
    size_t totalResults;                    // Total de resultados
    size_t pageCount;                       // Número de páginas
    bool hasMore;                           // Se tem mais resultados
};

/**
 * @brief Filtros de busca no marketplace
 */
struct MarketSearchFilters {
    std::string nameFilter;               // Filtro de nome
    std::vector<MarketCategory> categories; // Categorias
    WORD itemID;                          // ID do item
    BYTE minQuality;                      // Qualidade mínima
    BYTE maxQuality;                      // Qualidade máxima
    BYTE minEnchantLevel;                 // Nível de encantamento mínimo
    BYTE maxEnchantLevel;                 // Nível de encantamento máximo
    DWORD minPrice;                       // Preço mínimo
    DWORD maxPrice;                       // Preço máximo
    CurrencyType currencyType;            // Tipo de moeda
    std::vector<DWORD> attributeFilters;   // Filtros de atributos
    std::vector<WORD> socketFilters;       // Filtros de soquetes
    bool onlyBuyout;                      // Apenas compra direta
    bool onlyAuctions;                    // Apenas leilões
    bool excludeExpiring;                 // Excluir expirando
    DWORD sellerID;                       // ID do vendedor
    DWORD pageSize;                       // Tamanho da página
    DWORD pageNumber;                     // Número da página
    std::string sortBy;                    // Ordenar por
    bool ascending;                        // Ordem ascendente
    
    MarketSearchFilters()
        : nameFilter("")
        , itemID(0)
        , minQuality(0)
        , maxQuality(255)
        , minEnchantLevel(0)
        , maxEnchantLevel(255)
        , minPrice(0)
        , maxPrice(2000000000)
        , currencyType(CurrencyType::GOLD)
        , onlyBuyout(false)
        , onlyAuctions(false)
        , excludeExpiring(false)
        , sellerID(0)
        , pageSize(50)
        , pageNumber(1)
        , sortBy("price")
        , ascending(true)
    {
    }
};

/**
 * @brief Atualização de marketplace
 */
struct MarketplaceUpdate {
    DWORD updateID;                    // ID da atualização
    std::string updateType;             // Tipo de atualização
    DWORD listingID;                   // ID da listagem
    DWORD auctionID;                   // ID do leilão
    DWORD playerID;                    // ID do jogador
    std::string details;                // Detalhes
    time_t timestamp;                   // Timestamp
};

/**
 * @brief Gerenciador do marketplace
 */
/**
 * Classe WYDMarketplaceManager
 * 
 * Esta classe implementa a funcionalidade WYDMarketplaceManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MarketplaceManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static MarketplaceManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
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
     * @brief Carrega listagens
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadListings(const std::string& filePath);
    
    /**
     * @brief Salva listagens
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveListings(const std::string& filePath);
    
    /**
     * @brief Carrega leilões
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadAuctions(const std::string& filePath);
    
    /**
     * @brief Salva leilões
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveAuctions(const std::string& filePath);
    
    /**
     * @brief Carrega histórico de preços
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadPriceHistory(const std::string& filePath);
    
    /**
     * @brief Salva histórico de preços
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SavePriceHistory(const std::string& filePath);
    
    /**
     * @brief Carrega transações
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadTransactions(const std::string& filePath);
    
    /**
     * @brief Salva transações
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveTransactions(const std::string& filePath);
    
    /**
     * @brief Cria uma listagem de venda direta
     * @param seller Vendedor
     * @param itemSlot Slot do item
     * @param price Preço
     * @param quantity Quantidade
     * @param duration Duração em horas
     * @param currencyType Tipo de moeda
     * @param description Descrição
     * @param errorReason Razão do erro (saída)
     * @return ID da listagem, ou 0 se falhar
     */
    DWORD CreateDirectSaleListing(player::CPlayer* seller, BYTE itemSlot, DWORD price,
                                WORD quantity, BYTE duration, CurrencyType currencyType,
                                const std::string& description, std::string& errorReason);
    
    /**
     * @brief Cria uma listagem de leilão
     * @param seller Vendedor
     * @param itemSlot Slot do item
     * @param startingBid Lance inicial
     * @param buyoutPrice Preço de compra direta
     * @param minBidIncrement Incremento mínimo de lance
     * @param duration Duração em horas
     * @param currencyType Tipo de moeda
     * @param description Descrição
     * @param errorReason Razão do erro (saída)
     * @return ID do leilão, ou 0 se falhar
     */
    DWORD CreateAuctionListing(player::CPlayer* seller, BYTE itemSlot, DWORD startingBid,
                             DWORD buyoutPrice, DWORD minBidIncrement, BYTE duration,
                             CurrencyType currencyType, const std::string& description,
                             std::string& errorReason);
    
    /**
     * @brief Cancela uma listagem
     * @param listingID ID da listagem
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelListing(DWORD listingID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Cancela um leilão
     * @param auctionID ID do leilão
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se cancelado com sucesso
     */
    bool CancelAuction(DWORD auctionID, DWORD playerID, std::string& errorReason);
    
    /**
     * @brief Compra um item listado
     * @param buyer Comprador
     * @param listingID ID da listagem
     * @param quantity Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se comprado com sucesso
     */
    bool BuyListedItem(player::CPlayer* buyer, DWORD listingID, WORD quantity, std::string& errorReason);
    
    /**
     * @brief Dá um lance em um leilão
     * @param bidder Licitante
     * @param auctionID ID do leilão
     * @param bidAmount Valor do lance
     * @param errorReason Razão do erro (saída)
     * @return true se o lance foi dado com sucesso
     */
    bool PlaceBid(player::CPlayer* bidder, DWORD auctionID, DWORD bidAmount, std::string& errorReason);
    
    /**
     * @brief Compra diretamente um leilão
     * @param buyer Comprador
     * @param auctionID ID do leilão
     * @param errorReason Razão do erro (saída)
     * @return true se comprado com sucesso
     */
    bool BuyoutAuction(player::CPlayer* buyer, DWORD auctionID, std::string& errorReason);
    
    /**
     * @brief Processa leilões expirados
     */
    void ProcessExpiredAuctions();
    
    /**
     * @brief Processa listagens expiradas
     */
    void ProcessExpiredListings();
    
    /**
     * @brief Atualiza o histórico de preços
     */
    void UpdatePriceHistory();
    
    /**
     * @brief Busca no marketplace
     * @param filters Filtros
     * @return Resultados da busca
     */
    MarketSearchResult SearchMarketplace(const MarketSearchFilters& filters);
    
    /**
     * @brief Obtém uma listagem
     * @param listingID ID da listagem
     * @return Ponteiro para a listagem, ou nullptr se não existir
     */
    const MarketListing* GetListing(DWORD listingID) const;
    
    /**
     * @brief Obtém um leilão
     * @param auctionID ID do leilão
     * @return Ponteiro para o leilão, ou nullptr se não existir
     */
    const Auction* GetAuction(DWORD auctionID) const;
    
    /**
     * @brief Obtém listagens por vendedor
     * @param sellerID ID do vendedor
     * @return Lista de listagens
     */
    std::vector<const MarketListing*> GetListingsBySeller(DWORD sellerID) const;
    
    /**
     * @brief Obtém leilões por vendedor
     * @param sellerID ID do vendedor
     * @return Lista de leilões
     */
    std::vector<const Auction*> GetAuctionsBySeller(DWORD sellerID) const;
    
    /**
     * @brief Obtém transações por comprador
     * @param buyerID ID do comprador
     * @param count Número de transações
     * @param offset Deslocamento
     * @return Lista de transações
     */
    std::vector<const MarketTransaction*> GetTransactionsByBuyer(DWORD buyerID, size_t count, size_t offset = 0) const;
    
    /**
     * @brief Obtém transações por vendedor
     * @param sellerID ID do vendedor
     * @param count Número de transações
     * @param offset Deslocamento
     * @return Lista de transações
     */
    std::vector<const MarketTransaction*> GetTransactionsBySeller(DWORD sellerID, size_t count, size_t offset = 0) const;
    
    /**
     * @brief Obtém o histórico de preços de um item
     * @param itemID ID do item
     * @param quality Qualidade
     * @param enchantLevel Nível de encantamento
     * @param timeRange Intervalo de tempo em dias (0 para todos)
     * @return Lista de entradas no histórico
     */
    std::vector<const PriceHistoryEntry*> GetItemPriceHistory(WORD itemID, BYTE quality,
                                                          BYTE enchantLevel, BYTE timeRange = 0) const;
    
    /**
     * @brief Obtém o preço médio atual de um item
     * @param itemID ID do item
     * @param quality Qualidade
     * @param enchantLevel Nível de encantamento
     * @return Preço médio, ou 0 se não houver dados
     */
    DWORD GetItemAveragePrice(WORD itemID, BYTE quality, BYTE enchantLevel) const;
    
    /**
     * @brief Adiciona um item à lista de observação
     * @param playerID ID do jogador
     * @param itemID ID do item
     * @param quality Qualidade
     * @param enchantLevel Nível de encantamento
     * @param maxPrice Preço máximo
     * @param notifyOnListing Notificar ao listar
     * @param notifyOnPriceChange Notificar em mudança de preço
     * @param errorReason Razão do erro (saída)
     * @return ID de observação, ou 0 se falhar
     */
    DWORD AddToWatchlist(DWORD playerID, WORD itemID, BYTE quality, BYTE enchantLevel,
                       DWORD maxPrice, bool notifyOnListing, bool notifyOnPriceChange,
                       std::string& errorReason);
    
    /**
     * @brief Remove um item da lista de observação
     * @param playerID ID do jogador
     * @param watchID ID de observação
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveFromWatchlist(DWORD playerID, DWORD watchID, std::string& errorReason);
    
    /**
     * @brief Obtém a lista de observação de um jogador
     * @param playerID ID do jogador
     * @return Lista de itens observados
     */
    std::vector<const WatchlistItem*> GetPlayerWatchlist(DWORD playerID) const;
    
    /**
     * @brief Processa notificações de lista de observação
     */
    void ProcessWatchlistNotifications();
    
    /**
     * @brief Destaca um item
     * @param listingID ID da listagem
     * @param adminID ID do administrador
     * @param duration Duração em horas
     * @param errorReason Razão do erro (saída)
     * @return true se destacado com sucesso
     */
    bool FeatureListing(DWORD listingID, DWORD adminID, BYTE duration, std::string& errorReason);
    
    /**
     * @brief Remove o destaque de um item
     * @param listingID ID da listagem
     * @param adminID ID do administrador
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool UnfeatureListing(DWORD listingID, DWORD adminID, std::string& errorReason);
    
    /**
     * @brief Obtém itens em destaque
     * @return Lista de listagens em destaque
     */
    std::vector<const MarketListing*> GetFeaturedListings() const;
    
    /**
     * @brief Calcula o imposto para uma listagem
     * @param price Preço
     * @param listingType Tipo de listagem
     * @return Valor do imposto
     */
    DWORD CalculateTax(DWORD price, MarketListingType listingType);
    
    /**
     * @brief Obtém estatísticas do marketplace
     * @return Estatísticas
     */
    MarketStatistics GetMarketStatistics() const;
    
    /**
     * @brief Registra um callback para transações
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterTransactionCallback(MarketTransactionCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterTransactionCallback(int callbackID);
    
    /**
     * @brief Adiciona uma faixa de imposto
     * @param minPrice Preço mínimo
     * @param maxPrice Preço máximo
     * @param taxRate Taxa de imposto
     * @param description Descrição
     * @param errorReason Razão do erro (saída)
     * @return ID da faixa, ou 0 se falhar
     */
    DWORD AddTaxBracket(DWORD minPrice, DWORD maxPrice, float taxRate,
                      const std::string& description, std::string& errorReason);
    
    /**
     * @brief Remove uma faixa de imposto
     * @param bracketID ID da faixa
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemoveTaxBracket(DWORD bracketID, std::string& errorReason);
    
    /**
     * @brief Obtém todas as faixas de imposto
     * @return Lista de faixas de imposto
     */
    std::vector<const TaxBracket*> GetAllTaxBrackets() const;
    
    /**
     * @brief Adiciona uma regra de controle de preço
     * @param itemID ID do item
     * @param quality Qualidade
     * @param minPrice Preço mínimo
     * @param maxPrice Preço máximo
     * @param adjustmentFactor Fator de ajuste
     * @param reason Razão
     * @param errorReason Razão do erro (saída)
     * @return ID da regra, ou 0 se falhar
     */
    DWORD AddPriceControlRule(WORD itemID, BYTE quality, DWORD minPrice, DWORD maxPrice,
                           float adjustmentFactor, const std::string& reason,
                           std::string& errorReason);
    
    /**
     * @brief Remove uma regra de controle de preço
     * @param ruleID ID da regra
     * @param errorReason Razão do erro (saída)
     * @return true se removido com sucesso
     */
    bool RemovePriceControlRule(DWORD ruleID, std::string& errorReason);
    
    /**
     * @brief Obtém todas as regras de controle de preço
     * @return Lista de regras
     */
    std::vector<const PriceControlRule*> GetAllPriceControlRules() const;
    
    /**
     * @brief Ativa ou desativa uma regra de controle de preço
     * @param ruleID ID da regra
     * @param enforced Se deve ser aplicada
     * @param errorReason Razão do erro (saída)
     * @return true se atualizado com sucesso
     */
    bool SetPriceControlRuleEnforcement(DWORD ruleID, bool enforced, std::string& errorReason);
    
    /**
     * @brief Verifica se um preço está dentro das regras de controle
     * @param itemID ID do item
     * @param quality Qualidade
     * @param price Preço
     * @param errorReason Razão do erro (saída)
     * @return true se estiver dentro das regras
     */
    bool IsPriceWithinControlRules(WORD itemID, BYTE quality, DWORD price, std::string& errorReason);
    
    /**
     * @brief Obtém o preço sugerido para um item
     * @param itemID ID do item
     * @param quality Qualidade
     * @param enchantLevel Nível de encantamento
     * @return Preço sugerido, ou 0 se não houver dados
     */
    DWORD GetSuggestedPrice(WORD itemID, BYTE quality, BYTE enchantLevel);
    
    /**
     * @brief Obtém um resumo do mercado para um item
     * @param itemID ID do item
     * @param quality Qualidade
     * @param enchantLevel Nível de encantamento
     * @return String formatada com o resumo
     */
    std::string GetMarketSummaryForItem(WORD itemID, BYTE quality, BYTE enchantLevel);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    MarketplaceManager();
    
    /**
     * @brief Destrutor
     */
    ~MarketplaceManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    MarketplaceManager(const MarketplaceManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    MarketplaceManager& operator=(const MarketplaceManager&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Registra uma transação
     * @param transaction Transação
     */
    void RegisterTransaction(const MarketTransaction& transaction);
    
    /**
     * @brief Notifica callbacks de transação
     * @param transaction Transação
     */
    void NotifyTransactionCallbacks(const MarketTransaction& transaction);
    
    /**
     * @brief Atualiza índices internos
     */
    void UpdateIndices();
    
    /**
     * @brief Valida um item para venda
     * @param player Jogador
     * @param itemSlot Slot do item
     * @param quantity Quantidade
     * @param errorReason Razão do erro (saída)
     * @return true se válido
     */
    bool ValidateItemForSale(player::CPlayer* player, BYTE itemSlot, WORD quantity, std::string& errorReason);
    
    /**
     * @brief Valida um preço
     * @param price Preço
     * @param itemID ID do item
     * @param quality Qualidade
     * @param errorReason Razão do erro (saída)
     * @return true se válido
     */
    bool ValidatePrice(DWORD price, WORD itemID, BYTE quality, std::string& errorReason);
    
    /**
     * @brief Finaliza um leilão
     * @param auctionID ID do leilão
     * @param winner ID do vencedor
     * @param winningBid Lance vencedor
     * @return true se finalizado com sucesso
     */
    bool FinalizeAuction(DWORD auctionID, DWORD winner, DWORD winningBid);
    
    /**
     * @brief Marca uma listagem como atualizada
     * @param listingID ID da listagem
     */
    void MarkListingAsUpdated(DWORD listingID);
    
    /**
     * @brief Marca um leilão como atualizado
     * @param auctionID ID do leilão
     */
    void MarkAuctionAsUpdated(DWORD auctionID);
    
    /**
     * @brief Determina a categoria para um item
     * @param itemID ID do item
     * @return Categoria
     */
    MarketCategory DetermineItemCategory(WORD itemID);
    
    /**
     * @brief Verifica restrições de mercado para um jogador
     * @param playerID ID do jogador
     * @param errorReason Razão do erro (saída)
     * @return true se não houver restrições
     */
    bool CheckMarketRestrictions(DWORD playerID, std::string& errorReason);
    
    // Listagens
    std::map<DWORD, MarketListing> listings_;
    mutable std::mutex listingsMutex_;
    
    // Leilões
    std::map<DWORD, Auction> auctions_;
    mutable std::mutex auctionsMutex_;
    
    // Histórico de preços
    std::vector<PriceHistoryEntry> priceHistory_;
    mutable std::mutex priceHistoryMutex_;
    
    // Transações
    std::vector<MarketTransaction> transactions_;
    mutable std::mutex transactionsMutex_;
    
    // Lista de observação
    std::map<DWORD, std::vector<WatchlistItem>> watchlists_;
    mutable std::mutex watchlistsMutex_;
    
    // Faixas de imposto
    std::vector<TaxBracket> taxBrackets_;
    mutable std::mutex taxBracketsMutex_;
    
    // Regras de controle de preço
    std::vector<PriceControlRule> priceControlRules_;
    mutable std::mutex priceControlRulesMutex_;
    
    // Índices para busca rápida
    std::map<DWORD, std::vector<DWORD>> listingsBySeller_;
    std::map<DWORD, std::vector<DWORD>> auctionsBySeller_;
    std::map<WORD, std::vector<DWORD>> listingsByItem_;
    std::map<WORD, std::vector<DWORD>> auctionsByItem_;
    std::map<MarketCategory, std::vector<DWORD>> listingsByCategory_;
    std::map<MarketCategory, std::vector<DWORD>> auctionsByCategory_;
    std::vector<DWORD> featuredListings_;
    
    // Cache de preços médios
    std::map<std::tuple<WORD, BYTE, BYTE>, DWORD> averagePriceCache_;
    mutable std::mutex averagePriceCacheMutex_;
    
    // Callbacks para transações
    std::map<int, MarketTransactionCallback> transactionCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contadores para novos IDs
    std::atomic<DWORD> nextListingID_;
    std::atomic<DWORD> nextAuctionID_;
    std::atomic<DWORD> nextTransactionID_;
    std::atomic<DWORD> nextHistoryEntryID_;
    std::atomic<DWORD> nextWatchID_;
    std::atomic<DWORD> nextTaxBracketID_;
    std::atomic<DWORD> nextPriceRuleID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Listagens e leilões atualizados recentemente (necessitam persistência)
    std::unordered_set<DWORD> updatedListings_;
    std::mutex updatedListingsMutex_;
    std::unordered_set<DWORD> updatedAuctions_;
    std::mutex updatedAuctionsMutex_;
    
    // Controle de tempos
    time_t lastHistoryUpdate_;
    time_t lastExpiryCheck_;
    time_t lastWatchlistCheck_;
    
    // Estatísticas
    MarketStatistics statistics_;
    mutable std::mutex statisticsMutex_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_MarketplaceManager wyd::server::economy::marketplace::MarketplaceManager::GetInstance()

} // namespace marketplace
} // namespace economy
} // namespace server
} // namespace wyd

#endif // MARKETPLACEMANAGER_H

} // namespace wydbr
