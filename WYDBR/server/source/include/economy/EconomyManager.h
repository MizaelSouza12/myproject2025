/**
 * EconomyManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/economy/EconomyManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _ECONOMY_MANAGER_H_
#define _ECONOMY_MANAGER_H_

/**
 * @file EconomyManager.h
 * @brief Sistema de economia do servidor WYD - Baseado no código original
 * 
 * Implementa o sistema econômico do jogo, incluindo preços de itens,
 * moedas, comércio, taxas e sistemas para controle da economia.
 * Mantém compatibilidade com o sistema original do WYD.
 */

#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <functional>

#include "../GlobalDef.h"
#include "../TM_Item.h"

namespace wydbr {

class WYDPlayer;
class ItemManager;

/**
 * @brief Categorias de itens de loja
 */
enum ShopCategory {
    SHOP_CATEGORY_WEAPONS = 0,       // Armas
    SHOP_CATEGORY_ARMOR = 1,         // Armaduras
    SHOP_CATEGORY_SHIELDS = 2,       // Escudos
    SHOP_CATEGORY_HELMS = 3,         // Elmos/Capacetes
    SHOP_CATEGORY_ACCESSORIES = 4,   // Acessórios
    SHOP_CATEGORY_POTIONS = 5,       // Poções
    SHOP_CATEGORY_MATERIALS = 6,     // Materiais
    SHOP_CATEGORY_BOOKS = 7,         // Livros
    SHOP_CATEGORY_QUEST = 8,         // Itens de quest
    SHOP_CATEGORY_SPECIAL = 9        // Itens especiais
};

/**
 * @brief Tipo de moeda virtual
 */
enum CurrencyType {
    CURRENCY_GOLD = 0,              // Ouro (moeda básica)
    CURRENCY_ZEN = 1,               // Zen (moeda premium)
    CURRENCY_DONA = 2,              // Ponto de doação
    CURRENCY_BOSS = 3,              // Ponto de boss
    CURRENCY_GUILD = 4,             // Ponto de guilda
    CURRENCY_HONOR = 5,             // Ponto de honra
    CURRENCY_ARENA = 6,             // Ponto de arena
    CURRENCY_EVENT = 7,             // Ponto de evento
    CURRENCY_CUSTOM1 = 8,           // Moeda customizada 1
    CURRENCY_CUSTOM2 = 9            // Moeda customizada 2
};

/**
 * @brief Tipo de leilão
 */
enum AuctionType {
    AUCTION_NORMAL = 0,             // Leilão normal
    AUCTION_QUICK = 1,              // Leilão rápido
    AUCTION_SILENT = 2,             // Leilão silencioso
    AUCTION_REVERSE = 3,            // Leilão reverso
    AUCTION_FIXED = 4,              // Preço fixo (compra imediata)
    AUCTION_EVENT = 5,              // Leilão de evento
    AUCTION_GUILD = 6               // Leilão de guilda
};

/**
 * @brief Tipo de transação
 */
enum TransactionType {
    TRANSACTION_SHOP_BUY = 0,        // Compra em loja
    TRANSACTION_SHOP_SELL = 1,       // Venda em loja
    TRANSACTION_PLAYER_TRADE = 2,    // Troca entre jogadores
    TRANSACTION_AUCTION_BID = 3,     // Lance em leilão
    TRANSACTION_AUCTION_BUY = 4,     // Compra direta em leilão
    TRANSACTION_AUCTION_SELL = 5,    // Venda em leilão
    TRANSACTION_BANK_DEPOSIT = 6,    // Depósito no banco
    TRANSACTION_BANK_WITHDRAW = 7,   // Saque do banco
    TRANSACTION_MAIL_SEND = 8,       // Envio por correio
    TRANSACTION_MAIL_RECEIVE = 9,    // Recebimento por correio
    TRANSACTION_QUEST_REWARD = 10,   // Recompensa de quest
    TRANSACTION_ADMIN_ADD = 11,      // Adicionado por admin
    TRANSACTION_ADMIN_REMOVE = 12,   // Removido por admin
    TRANSACTION_SYSTEM_ADD = 13,     // Adicionado pelo sistema
    TRANSACTION_SYSTEM_REMOVE = 14,  // Removido pelo sistema
    TRANSACTION_STORAGE_COST = 15,   // Custo de armazenamento
    TRANSACTION_TAX = 16,            // Taxa
    TRANSACTION_FEE = 17,            // Taxa de serviço
    TRANSACTION_REFUND = 18,         // Reembolso
    TRANSACTION_GUILD = 19,          // Transação de guilda
    TRANSACTION_REPAIR = 20          // Reparo de item
};

/**
 * @brief Item de loja
 */
struct ShopItem {
    int itemId;                      // ID do item
    int price;                       // Preço base do item
    int quantity;                    // Quantidade disponível (-1 = infinito)
    CurrencyType currency;           // Tipo de moeda
    int level;                       // Nível do item
    ShopCategory category;           // Categoria do item
    bool limited;                    // Item limitado
    uint32_t restockTime;            // Tempo de reabastecimento
    
    ShopItem() : itemId(0), price(0), quantity(-1), currency(CURRENCY_GOLD),
                level(0), category(SHOP_CATEGORY_WEAPONS), limited(false),
                restockTime(0) {}
};

/**
 * @brief Loja
 */
struct Shop {
    int shopId;                      // ID da loja
    std::string name;                // Nome da loja
    int npcId;                       // ID do NPC associado
    int mapId;                       // ID do mapa
    int x;                           // Coordenada X
    int y;                           // Coordenada Y
    std::vector<ShopItem> items;     // Itens da loja
    bool premium;                    // Loja premium
    
    Shop() : shopId(0), npcId(0), mapId(0), x(0), y(0), premium(false) {}
};

/**
 * @brief Item em leilão
 */
struct AuctionItem {
    int auctionId;                   // ID do leilão
    int sellerId;                    // ID do vendedor
    std::string sellerName;          // Nome do vendedor
    int highestBidderId;             // ID do maior licitante
    std::string highestBidderName;   // Nome do maior licitante
    STRUCT_ITEM item;                // Item em leilão
    int startPrice;                  // Preço inicial
    int currentPrice;                // Preço atual
    int buyoutPrice;                 // Preço de compra imediata
    int minBidIncrement;             // Incremento mínimo de lance
    uint32_t startTime;              // Timestamp de início
    uint32_t endTime;                // Timestamp de término
    AuctionType type;                // Tipo de leilão
    CurrencyType currency;           // Tipo de moeda
    bool ended;                      // Leilão finalizado
    bool claimed;                    // Item/dinheiro reclamado
    
    AuctionItem() : auctionId(0), sellerId(0), highestBidderId(0), 
                   startPrice(0), currentPrice(0), buyoutPrice(0),
                   minBidIncrement(0), startTime(0), endTime(0),
                   type(AUCTION_NORMAL), currency(CURRENCY_GOLD),
                   ended(false), claimed(false) {}
};

/**
 * @brief Transação econômica
 */
struct Transaction {
    int transactionId;               // ID da transação
    int playerId;                    // ID do jogador
    std::string playerName;          // Nome do jogador
    int targetId;                    // ID do alvo (jogador, NPC, etc)
    std::string targetName;          // Nome do alvo
    TransactionType type;            // Tipo de transação
    int amount;                      // Montante da transação
    CurrencyType currency;           // Tipo de moeda
    int itemId;                      // ID do item (se aplicável)
    STRUCT_ITEM item;                // Item transacionado (se aplicável)
    uint32_t timestamp;              // Timestamp da transação
    std::string description;         // Descrição da transação
    
    Transaction() : transactionId(0), playerId(0), targetId(0),
                   type(TRANSACTION_SHOP_BUY), amount(0), 
                   currency(CURRENCY_GOLD), itemId(0),
                   timestamp(0) {}
};

/**
 * @brief Balança comercial (monitoramento da economia)
 */
struct TradeBalance {
    int goldInCirculation;          // Quantidade de ouro em circulação
    int goldGeneratedToday;         // Ouro gerado hoje
    int goldRemovedToday;           // Ouro removido hoje
    int itemsInCirculation;         // Quantidade de itens em circulação
    int itemsGeneratedToday;        // Itens gerados hoje
    int itemsRemovedToday;          // Itens removidos hoje
    int activeShops;                // Lojas ativas
    int activeAuctions;             // Leilões ativos
    int activePlayers;              // Jogadores ativos
    float inflation;                // Taxa de inflação
    float averagePrice;             // Preço médio dos itens
    uint32_t timestamp;             // Timestamp da atualização
    
    TradeBalance() : goldInCirculation(0), goldGeneratedToday(0),
                   goldRemovedToday(0), itemsInCirculation(0),
                   itemsGeneratedToday(0), itemsRemovedToday(0),
                   activeShops(0), activeAuctions(0), activePlayers(0),
                   inflation(0.0f), averagePrice(0.0f), timestamp(0) {}
};

/**
 * @brief Taxa de mercado
 */
struct MarketFee {
    std::string name;               // Nome da taxa
    TransactionType type;           // Tipo de transação associada
    float basePercentage;           // Porcentagem base
    int baseFlatAmount;             // Valor fixo base
    float increasePerLevel;         // Aumento por nível
    int maxFeeAmount;               // Valor máximo da taxa
    
    MarketFee() : type(TRANSACTION_SHOP_BUY), basePercentage(0.0f),
                baseFlatAmount(0), increasePerLevel(0.0f),
                maxFeeAmount(0) {}
};

/**
 * @brief Gerenciador de economia global
 * 
 * Classe singleton responsável pelo gerenciamento de toda a
 * economia do servidor WYD, incluindo preços, lojas, leilões e taxas.
 */
/**
 * Classe WYDEconomyManager
 * 
 * Esta classe implementa a funcionalidade WYDEconomyManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EconomyManager {
public:
    /**
     * @brief Obtém a instância única do EconomyManager
     * @return Referência para a instância
     */
    static EconomyManager& GetInstance();

    /**
     * @brief Inicializa o gerenciador de economia
     * @param configPath Caminho para arquivo de configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& configPath = "");

    /**
     * @brief Finaliza o gerenciador de economia
     */
    void Finalize();
    
    /**
     * @brief Processa um tick de atualização para a economia
     * @param currentTick Tick atual
     */
    void ProcessTick(uint32_t currentTick);
    
    /**
     * @brief Carrega lojas de um arquivo
     * @param filePath Caminho para o arquivo
     * @return true se carregado com sucesso
     */
    bool LoadShops(const std::string& filePath);
    
    /**
     * @brief Salva lojas em um arquivo
     * @param filePath Caminho para o arquivo
     * @return true se salvo com sucesso
     */
    bool SaveShops(const std::string& filePath);
    
    /**
     * @brief Adiciona uma loja
     * @param shop Loja a adicionar
     * @return true se adicionada com sucesso
     */
    bool AddShop(const Shop& shop);
    
    /**
     * @brief Remove uma loja
     * @param shopId ID da loja
     * @return true se removida com sucesso
     */
    bool RemoveShop(int shopId);
    
    /**
     * @brief Obtém uma loja pelo ID
     * @param shopId ID da loja
     * @return Ponteiro para a loja, nullptr se não existir
     */
    Shop* GetShop(int shopId);
    
    /**
     * @brief Obtém uma lista de lojas
     * @return Vector de lojas
     */
    std::vector<Shop> GetShops();
    
    /**
     * @brief Adiciona um item a uma loja
     * @param shopId ID da loja
     * @param item Item a adicionar
     * @return true se adicionado com sucesso
     */
    bool AddShopItem(int shopId, const ShopItem& item);
    
    /**
     * @brief Remove um item de uma loja
     * @param shopId ID da loja
     * @param index Índice do item
     * @return true se removido com sucesso
     */
    bool RemoveShopItem(int shopId, int index);
    
    /**
     * @brief Processa uma compra em loja
     * @param playerId ID do jogador
     * @param shopId ID da loja
     * @param itemIndex Índice do item
     * @param quantity Quantidade a comprar
     * @return Código de resultado
     */
    ResultCode ProcessShopBuy(int playerId, int shopId, int itemIndex, int quantity);
    
    /**
     * @brief Processa uma venda em loja
     * @param playerId ID do jogador
     * @param shopId ID da loja
     * @param inventoryIndex Índice do item no inventário
     * @param quantity Quantidade a vender
     * @return Código de resultado
     */
    ResultCode ProcessShopSell(int playerId, int shopId, int inventoryIndex, int quantity);
    
    /**
     * @brief Inicia um leilão
     * @param playerId ID do jogador vendedor
     * @param inventoryIndex Índice do item no inventário
     * @param startPrice Preço inicial
     * @param buyoutPrice Preço de compra imediata (0 = sem compra imediata)
     * @param duration Duração em segundos
     * @param type Tipo de leilão
     * @param currency Tipo de moeda
     * @return ID do leilão, 0 se falhar
     */
    int StartAuction(int playerId, int inventoryIndex, int startPrice, 
                    int buyoutPrice, uint32_t duration, 
                    AuctionType type = AUCTION_NORMAL, 
                    CurrencyType currency = CURRENCY_GOLD);
    
    /**
     * @brief Processa um lance em leilão
     * @param playerId ID do jogador licitante
     * @param auctionId ID do leilão
     * @param bidAmount Valor do lance
     * @return Código de resultado
     */
    ResultCode ProcessAuctionBid(int playerId, int auctionId, int bidAmount);
    
    /**
     * @brief Processa uma compra imediata em leilão
     * @param playerId ID do jogador
     * @param auctionId ID do leilão
     * @return Código de resultado
     */
    ResultCode ProcessAuctionBuyout(int playerId, int auctionId);
    
    /**
     * @brief Cancela um leilão
     * @param playerId ID do jogador vendedor
     * @param auctionId ID do leilão
     * @return Código de resultado
     */
    ResultCode CancelAuction(int playerId, int auctionId);
    
    /**
     * @brief Reclama item/dinheiro de um leilão finalizado
     * @param playerId ID do jogador
     * @param auctionId ID do leilão
     * @return Código de resultado
     */
    ResultCode ClaimAuction(int playerId, int auctionId);
    
    /**
     * @brief Obtém lista de leilões ativos
     * @param category Categoria de item (-1 = todas)
     * @param maxLevel Nível máximo do item (-1 = sem limite)
     * @param type Tipo de leilão (-1 = todos)
     * @param playerName Nome do jogador vendedor ("" = todos)
     * @param maxResults Número máximo de resultados
     * @return Lista de leilões ativos
     */
    std::vector<AuctionItem> GetActiveAuctions(int category = -1, int maxLevel = -1,
                                           int type = -1, const std::string& playerName = "",
                                           int maxResults = 100);
    
    /**
     * @brief Obtém informações de um leilão
     * @param auctionId ID do leilão
     * @return Informações do leilão, nullptr se não existir
     */
    AuctionItem* GetAuction(int auctionId);
    
    /**
     * @brief Abre uma transação entre jogadores
     * @param initiatorId ID do jogador iniciador
     * @param targetId ID do jogador alvo
     * @return ID da transação, 0 se falhar
     */
    int StartPlayerTrade(int initiatorId, int targetId);
    
    /**
     * @brief Adiciona um item a uma transação
     * @param tradeId ID da transação
     * @param playerId ID do jogador
     * @param inventoryIndex Índice do item no inventário
     * @param quantity Quantidade a adicionar
     * @return Código de resultado
     */
    ResultCode AddTradeItem(int tradeId, int playerId, int inventoryIndex, int quantity);
    
    /**
     * @brief Remove um item de uma transação
     * @param tradeId ID da transação
     * @param playerId ID do jogador
     * @param tradeIndex Índice do item na transação
     * @return Código de resultado
     */
    ResultCode RemoveTradeItem(int tradeId, int playerId, int tradeIndex);
    
    /**
     * @brief Define o ouro para uma transação
     * @param tradeId ID da transação
     * @param playerId ID do jogador
     * @param amount Quantidade de ouro
     * @return Código de resultado
     */
    ResultCode SetTradeGold(int tradeId, int playerId, int amount);
    
    /**
     * @brief Confirma uma transação
     * @param tradeId ID da transação
     * @param playerId ID do jogador
     * @return Código de resultado
     */
    ResultCode ConfirmTrade(int tradeId, int playerId);
    
    /**
     * @brief Cancela uma transação
     * @param tradeId ID da transação
     * @param playerId ID do jogador que cancelou
     * @return Código de resultado
     */
    ResultCode CancelTrade(int tradeId, int playerId);
    
    /**
     * @brief Registra uma transação econômica
     * @param transaction Transação a registrar
     * @return ID da transação, 0 se falhar
     */
    int LogTransaction(const Transaction& transaction);
    
    /**
     * @brief Calcula uma taxa de mercado
     * @param type Tipo de transação
     * @param amount Valor da transação
     * @param playerLevel Nível do jogador
     * @return Valor da taxa
     */
    int CalculateFee(TransactionType type, int amount, int playerLevel);
    
    /**
     * @brief Adiciona uma taxa de mercado
     * @param fee Taxa a adicionar
     * @return true se adicionada com sucesso
     */
    bool AddMarketFee(const MarketFee& fee);
    
    /**
     * @brief Remove uma taxa de mercado
     * @param type Tipo de transação
     * @return true se removida com sucesso
     */
    bool RemoveMarketFee(TransactionType type);
    
    /**
     * @brief Calcula o preço de compra de um item
     * @param itemId ID do item
     * @param level Nível do item
     * @param shopId ID da loja (0 = preço base)
     * @return Preço de compra
     */
    int CalculateBuyPrice(int itemId, int level, int shopId = 0);
    
    /**
     * @brief Calcula o preço de venda de um item
     * @param itemId ID do item
     * @param level Nível do item
     * @param durability Durabilidade atual
     * @param maxDurability Durabilidade máxima
     * @param shopId ID da loja (0 = preço base)
     * @return Preço de venda
     */
    int CalculateSellPrice(int itemId, int level, int durability, int maxDurability, int shopId = 0);
    
    /**
     * @brief Calcula o preço de reparo de um item
     * @param itemId ID do item
     * @param level Nível do item
     * @param durability Durabilidade atual
     * @param maxDurability Durabilidade máxima
     * @return Preço de reparo
     */
    int CalculateRepairPrice(int itemId, int level, int durability, int maxDurability);
    
    /**
     * @brief Atualiza os preços de mercado com base na economia
     */
    void UpdateMarketPrices();
    
    /**
     * @brief Atualiza estatísticas da economia
     */
    void UpdateEconomyStats();
    
    /**
     * @brief Obtém estatísticas da economia
     * @return Estatísticas da economia
     */
    TradeBalance GetEconomyStats();
    
    /**
     * @brief Adiciona moeda virtual a um jogador
     * @param playerId ID do jogador
     * @param type Tipo de moeda
     * @param amount Quantidade a adicionar
     * @param source Fonte da moeda (para log)
     * @return true se adicionada com sucesso
     */
    bool AddCurrency(int playerId, CurrencyType type, int amount, const std::string& source);
    
    /**
     * @brief Remove moeda virtual de um jogador
     * @param playerId ID do jogador
     * @param type Tipo de moeda
     * @param amount Quantidade a remover
     * @param reason Motivo da remoção (para log)
     * @return true se removida com sucesso
     */
    bool RemoveCurrency(int playerId, CurrencyType type, int amount, const std::string& reason);
    
    /**
     * @brief Obtém a quantidade de moeda virtual de um jogador
     * @param playerId ID do jogador
     * @param type Tipo de moeda
     * @return Quantidade de moeda
     */
    int GetCurrency(int playerId, CurrencyType type);
    
    /**
     * @brief Transfere moeda virtual entre jogadores
     * @param senderId ID do jogador remetente
     * @param receiverId ID do jogador destinatário
     * @param type Tipo de moeda
     * @param amount Quantidade a transferir
     * @param reason Motivo da transferência (para log)
     * @return true se transferida com sucesso
     */
    bool TransferCurrency(int senderId, int receiverId, CurrencyType type, int amount, const std::string& reason);

/**
 * EconomyManager
 * 
 * Implementa a funcionalidade EconomyManager conforme especificação original.
 * @return Retorna private:
 */


private:
    EconomyManager();  // Construtor privado (singleton)
    ~EconomyManager(); // Destrutor privado (singleton)
    
    // Métodos privados
    
    /**
     * @brief Processa leilões expirados
     */
    void ProcessExpiredAuctions();
    
    /**
     * @brief Atualiza lojas (restock e preços)
     */
    void UpdateShops();
    
    /**
     * @brief Aplica uma transação econômica
     * @param transaction Transação a aplicar
     * @return true se aplicada com sucesso
     */
    bool ApplyTransaction(const Transaction& transaction);
    
    /**
     * @brief Carrega dados da economia
     * @param filePath Caminho para o arquivo
     * @return true se carregado com sucesso
     */
    bool LoadEconomyData(const std::string& filePath);
    
    /**
     * @brief Salva dados da economia
     * @param filePath Caminho para o arquivo
     * @return true se salvo com sucesso
     */
    bool SaveEconomyData(const std::string& filePath);
    
    // Atributos privados
    static EconomyManager* instance_;              // Instância única (singleton)
    bool initialized_;                             // Flag de inicialização
    
    // Lojas e Leilões
    std::map<int, Shop> shops_;                    // Mapa de lojas por ID
    std::map<int, AuctionItem> auctions_;          // Mapa de leilões por ID
    int nextAuctionId_;                            // Próximo ID de leilão
    
    // Transações entre jogadores
    struct Trade {
        int tradeId;                              // ID da transação
        int initiatorId;                          // ID do jogador iniciador
        int targetId;                             // ID do jogador alvo
        std::vector<std::pair<int, int>> initiatorItems; // Itens do iniciador (índice, quantidade)
        std::vector<std::pair<int, int>> targetItems;   // Itens do alvo (índice, quantidade)
        int initiatorGold;                        // Ouro do iniciador
        int targetGold;                           // Ouro do alvo
        bool initiatorConfirmed;                  // Iniciador confirmou
        bool targetConfirmed;                     // Alvo confirmou
        uint32_t startTime;                       // Timestamp de início
        
        Trade() : tradeId(0), initiatorId(0), targetId(0), 
                 initiatorGold(0), targetGold(0), 
                 initiatorConfirmed(false), targetConfirmed(false),
                 startTime(0) {}
    };
    
    std::map<int, Trade> trades_;                  // Mapa de transações por ID
    int nextTradeId_;                              // Próximo ID de transação
    
    // Transações e histórico econômico
    std::vector<Transaction> transactions_;        // Histórico de transações
    int nextTransactionId_;                        // Próximo ID de transação
    TradeBalance economyStats_;                    // Estatísticas da economia
    
    // Taxas de mercado
    std::map<TransactionType, MarketFee> fees_;    // Taxas de mercado por tipo
    float inflationFactor_;                        // Fator de inflação para preços
    
    // Moedas virtuais
    std::map<int, std::map<CurrencyType, int>> playerCurrencies_; // Moedas por jogador
    
    // Configurações
    std::string configPath_;                      // Caminho da configuração
    bool dynamicPricing_;                         // Preços dinâmicos
    bool economyMonitoring_;                      // Monitoramento de economia
    bool logTransactions_;                        // Log de transações
    int economyUpdateInterval_;                   // Intervalo de atualização da economia
    uint32_t lastEconomyUpdate_;                  // Último tick de atualização da economia
    uint32_t lastAuctionCheck_;                   // Último tick de verificação de leilões
    uint32_t lastShopUpdate_;                     // Último tick de atualização de lojas
    
    // Callbacks para eventos econômicos
    std::function<void(const AuctionItem&)> auctionStartCallback_;
    std::function<void(const AuctionItem&)> auctionEndCallback_;
    std::function<void(const AuctionItem&)> auctionBidCallback_;
    std::function<void(const Shop&, const ShopItem&, int)> shopBuyCallback_;
    std::function<void(const Shop&, int, int)> shopSellCallback_;
    std::function<void(const Trade&)> tradeCompleteCallback_;
    std::function<void(const TradeBalance&)> economyUpdateCallback_;
    
    // Mutex para acesso seguro
    std::mutex economyMutex_;                    // Mutex para acesso à economia
    
    // Desabilita cópias (singleton)
    EconomyManager(const EconomyManager&) = delete;
    EconomyManager& operator=(const EconomyManager&) = delete;
};

} // namespace wydbr

#endif // _ECONOMY_MANAGER_H_