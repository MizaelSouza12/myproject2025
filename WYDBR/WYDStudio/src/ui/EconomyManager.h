#ifndef WYDSTUDIO_ECONOMY_MANAGER_H
#define WYDSTUDIO_ECONOMY_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>
#include <chrono>
#include <variant>
#include <nlohmann/json.hpp>
#include "../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {

using json = nlohmann::json;

/**
 * @brief Tipo de recurso econômico
 */
enum class EconomicResourceType {
    GOLD,               // Dinheiro do jogo
    ITEM,               // Item específico
    PREMIUM_CURRENCY,   // Moeda premium
    XP,                 // Experiência
    SKILL_POINT,        // Ponto de habilidade
    SPECIAL_RESOURCE,   // Recurso especial
    CRAFTING_MATERIAL,  // Material de crafting
    TOKEN,              // Token especial
    REPUTATION,         // Reputação com facção
    CUSTOM              // Recurso personalizado
};

/**
 * @brief Tipo de transação econômica
 */
enum class TransactionType {
    PLAYER_TO_PLAYER,   // Transação entre jogadores
    PLAYER_TO_NPC,      // Jogador compra de NPC
    NPC_TO_PLAYER,      // Jogador vende para NPC
    PLAYER_TO_SYSTEM,   // Jogador gasta no sistema
    SYSTEM_TO_PLAYER,   // Sistema dá ao jogador
    QUEST_REWARD,       // Recompensa de quest
    MONSTER_DROP,       // Drop de monstro
    CRAFTING,           // Resultado de crafting
    ENCHANT,            // Resultado de enchant
    AUCTION_HOUSE,      // Casa de leilões
    MAIL,               // Correio
    BANK,               // Banco
    STORAGE_FEE,        // Taxa de armazenamento
    REPAIR_COST,        // Custo de reparo
    DEATH_PENALTY,      // Penalidade por morte
    CUSTOM              // Tipo personalizado
};

/**
 * @brief Tipo de fluxo econômico
 */
enum class EconomicFlowType {
    INFLOW,             // Entrada na economia
    OUTFLOW,            // Saída da economia
    TRANSFER,           // Transferência dentro da economia
    CONVERSION          // Conversão entre recursos
};

/**
 * @brief Tipo de intervenção econômica
 */
enum class EconomicInterventionType {
    PRICE_ADJUSTMENT,       // Ajuste de preço
    DROP_RATE_ADJUSTMENT,   // Ajuste de taxa de drop
    ITEM_SINK,              // Remoção de itens
    GOLD_SINK,              // Remoção de dinheiro
    ITEM_SOURCE,            // Adição de itens
    GOLD_SOURCE,            // Adição de dinheiro
    TRADE_LIMITATION,       // Limitação de comércio
    TAX_ADJUSTMENT,         // Ajuste de impostos
    EVENT_PROMOTION,        // Promoção de evento
    CUSTOM                  // Intervenção personalizada
};

/**
 * @brief Dados detalhados de um item na economia
 */
struct EconomicItemData {
    int32_t itemId;
    std::string name;
    std::string category;
    int32_t basePrice;                  // Preço base do item
    int32_t currentAveragePrice;        // Preço médio atual
    int32_t totalQuantityInGame;        // Quantidade total no jogo
    int32_t activelyTraded;             // Quantidade negociada ativamente
    int32_t dailyInflowAverage;         // Entrada média diária
    int32_t dailyOutflowAverage;        // Saída média diária
    std::vector<int32_t> priceHistory;  // Histórico de preços (médias diárias)
    std::vector<int32_t> volumeHistory; // Histórico de volume (negociações diárias)
    std::map<std::string, std::variant<int32_t, float, std::string>> additionalData;
};

/**
 * @brief Status de saúde econômica
 */
enum class EconomicHealthStatus {
    HEALTHY,        // Economia saudável
    INFLATION,      // Inflação detectada
    DEFLATION,      // Deflação detectada
    STAGNATION,     // Estagnação econômica
    HYPERINFLATION, // Hiperinflação
    MONOPOLY,       // Monopólio detectado
    MARKET_CRASH,   // Crash de mercado
    IMBALANCE       // Desequilíbrio geral
};

/**
 * @brief Indicador econômico geral
 */
struct EconomicIndicator {
    std::string name;                  // Nome do indicador
    float currentValue;                // Valor atual
    float previousValue;               // Valor anterior
    float changePercentage;            // Percentual de mudança
    std::string trend;                 // Tendência (up, down, stable)
    std::vector<float> historicalData; // Dados históricos
    std::map<std::string, std::variant<int32_t, float, std::string>> metadata;
};

/**
 * @brief Dados de uma transação econômica
 */
struct EconomicTransaction {
    int64_t id;
    TransactionType type;
    EconomicFlowType flowType;
    
    // Participantes
    struct {
        std::string sourceId;
        std::string sourceType;  // player, npc, system, etc.
        std::string sourceName;
        
        std::string targetId;
        std::string targetType;
        std::string targetName;
    } participants;
    
    // Recursos trocados
    struct ResourceAmount {
        EconomicResourceType type;
        int32_t resourceId;      // Para itens, materiais, etc.
        std::string resourceName;
        int64_t amount;
        int32_t unitValue;
    };
    
    std::vector<ResourceAmount> resourcesTraded;
    
    // Detalhes da transação
    std::chrono::system_clock::time_point timestamp;
    std::string location;           // Mapa/coordenadas
    std::string transactionContext; // Contexto (quest, trade, etc.)
    bool successful;
    std::string failReason;
    int64_t totalValue;             // Valor total da transação
    
    // Metadados
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Definição de uma intervenção econômica
 */
struct EconomicIntervention {
    int32_t id;
    std::string name;
    std::string description;
    EconomicInterventionType type;
    
    // Alvo da intervenção
    struct {
        std::string targetType;   // item, category, all, etc.
        int32_t targetId;
        std::string targetName;
    } target;
    
    // Parâmetros
    struct {
        std::map<std::string, std::variant<int32_t, float, std::string>> settings;
    } parameters;
    
    // Programação
    struct {
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        bool isActive;
        bool isPermanent;
        std::string schedule;  // Formato cron para intervenções recorrentes
    } schedule;
    
    // Efeitos e resultados
    struct {
        bool hasBeenApplied;
        std::string lastApplicationResult;
        std::chrono::system_clock::time_point lastApplied;
        std::map<std::string, std::variant<int32_t, float, std::string>> measuredEffects;
    } results;
    
    // Metadados
    struct {
        std::chrono::system_clock::time_point createdAt;
        std::string createdBy;
        std::chrono::system_clock::time_point modifiedAt;
        std::string modifiedBy;
        int32_t version;
    } metadata;
};

/**
 * @brief Relatório de saúde econômica
 */
struct EconomicHealthReport {
    // Status geral
    EconomicHealthStatus overallStatus;
    std::string statusDescription;
    float healthScore;  // 0.0 a 1.0
    
    // Indicadores econômicos
    struct {
        float inflationRate;
        float tradeVolume;
        float wealthDistribution;  // Coeficiente Gini
        float economicActivity;    // 0.0 a 1.0
        float currencyVelocity;    // Taxa de circulação
        float marketLiquidity;     // 0.0 a 1.0
    } indicators;
    
    // Estatísticas gerais
    struct {
        int64_t totalGoldInGame;
        int64_t activelyTradedGold;
        int32_t totalUniqueItemsInGame;
        int32_t averageDailyTransactions;
        float averageDailyGoldTraded;
        float topPlayerWealthPercentage;  // % da riqueza nos top 1%
    } statistics;
    
    // Problemas detectados
    struct EconomicIssue {
        std::string name;
        std::string description;
        float severity;  // 0.0 a 1.0
        std::string recommendedAction;
    };
    
    std::vector<EconomicIssue> detectedIssues;
    
    // Recomendações
    std::vector<std::string> recommendations;
    
    // Metadados do relatório
    struct {
        std::chrono::system_clock::time_point generatedAt;
        std::string dataTimeRange;
        int32_t dataPointsAnalyzed;
    } metadata;
};

/**
 * @brief Resultado de operações econômicas
 */
struct EconomyOperationResult {
    bool success;
    std::string message;
    std::variant<
        std::monostate,
        EconomicItemData,
        EconomicTransaction,
        EconomicIntervention,
        EconomicHealthReport
    > data;
};

/**
 * @brief Gerenciador de Economia do WYDStudio
 */
class EconomyManager {
public:
    /**
     * @brief Construtor
     * @param facade Referência ao facade principal do WYDStudio
     */
    EconomyManager(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~EconomyManager();
    
    /**
     * @brief Inicializa o gerenciador de economia
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Obtém dados econômicos de um item
     * @param itemId ID do item
     * @return Dados econômicos do item
     */
    std::optional<EconomicItemData> getItemEconomicData(int32_t itemId);
    
    /**
     * @brief Busca itens com alta atividade econômica
     * @param category Categoria opcional para filtrar
     * @param limit Limite de resultados
     * @return Lista de dados econômicos de itens
     */
    std::vector<EconomicItemData> getHighActivityItems(
        const std::string& category = "",
        int32_t limit = 20);
    
    /**
     * @brief Busca itens com maior variação de preço
     * @param timeFrame Período de tempo em dias
     * @param limit Limite de resultados
     * @return Lista de dados econômicos de itens
     */
    std::vector<EconomicItemData> getHighestPriceVolatilityItems(
        int32_t timeFrame = 7,
        int32_t limit = 20);
    
    /**
     * @brief Obtém histórico de preços detalhado de um item
     * @param itemId ID do item
     * @param timeFrame Período de tempo em dias
     * @param resolution Resolução dos dados ("hourly", "daily", "weekly")
     * @return Séries temporais de preços
     */
    std::map<std::string, std::vector<std::pair<std::string, float>>> getItemPriceHistory(
        int32_t itemId,
        int32_t timeFrame = 30,
        const std::string& resolution = "daily");
    
    /**
     * @brief Obtém transações recentes
     * @param type Tipo de transação opcional para filtrar
     * @param limit Limite de resultados
     * @return Lista de transações
     */
    std::vector<EconomicTransaction> getRecentTransactions(
        std::optional<TransactionType> type = std::nullopt,
        int32_t limit = 100);
    
    /**
     * @brief Obtém transações de um jogador
     * @param playerId ID do jogador
     * @param timeFrame Período de tempo em dias
     * @param limit Limite de resultados
     * @return Lista de transações
     */
    std::vector<EconomicTransaction> getPlayerTransactions(
        const std::string& playerId,
        int32_t timeFrame = 30,
        int32_t limit = 100);
    
    /**
     * @brief Obtém transações envolvendo um item específico
     * @param itemId ID do item
     * @param timeFrame Período de tempo em dias
     * @param limit Limite de resultados
     * @return Lista de transações
     */
    std::vector<EconomicTransaction> getItemTransactions(
        int32_t itemId,
        int32_t timeFrame = 30,
        int32_t limit = 100);
    
    /**
     * @brief Obtém relatório geral de saúde econômica
     * @return Relatório de saúde econômica
     */
    EconomicHealthReport getEconomicHealthReport();
    
    /**
     * @brief Obtém indicadores econômicos do jogo
     * @return Lista de indicadores
     */
    std::vector<EconomicIndicator> getEconomicIndicators();
    
    /**
     * @brief Obtém dados de distribuição de riqueza
     * @param segmentCount Número de segmentos para dividir os jogadores
     * @return Dados de distribuição (percentis -> percentual de riqueza)
     */
    std::map<std::string, float> getWealthDistribution(int32_t segmentCount = 10);
    
    /**
     * @brief Cria uma nova intervenção econômica
     * @param intervention Dados da intervenção
     * @return Resultado da operação
     */
    EconomyOperationResult createIntervention(const EconomicIntervention& intervention);
    
    /**
     * @brief Atualiza uma intervenção existente
     * @param intervention Dados atualizados da intervenção
     * @return Resultado da operação
     */
    EconomyOperationResult updateIntervention(const EconomicIntervention& intervention);
    
    /**
     * @brief Remove uma intervenção
     * @param interventionId ID da intervenção a remover
     * @return Resultado da operação
     */
    EconomyOperationResult removeIntervention(int32_t interventionId);
    
    /**
     * @brief Obtém uma intervenção pelo ID
     * @param interventionId ID da intervenção
     * @return Intervenção ou nulo se não encontrada
     */
    std::optional<EconomicIntervention> getInterventionById(int32_t interventionId);
    
    /**
     * @brief Obtém todas as intervenções
     * @param activeOnly Se deve retornar apenas intervenções ativas
     * @return Lista de intervenções
     */
    std::vector<EconomicIntervention> getAllInterventions(bool activeOnly = false);
    
    /**
     * @brief Ativa ou desativa uma intervenção
     * @param interventionId ID da intervenção
     * @param active Estado desejado
     * @return Resultado da operação
     */
    EconomyOperationResult setInterventionActive(int32_t interventionId, bool active);
    
    /**
     * @brief Aplica imediatamente uma intervenção
     * @param interventionId ID da intervenção
     * @return Resultado da operação
     */
    EconomyOperationResult applyInterventionNow(int32_t interventionId);
    
    /**
     * @brief Prevê o impacto de uma intervenção
     * @param intervention Intervenção a simular
     * @return Relatório de impacto previsto
     */
    std::map<std::string, std::variant<int32_t, float, std::string>> predictInterventionImpact(
        const EconomicIntervention& intervention);
    
    /**
     * @brief Ajusta preços de NPCs para equilibrar a economia
     * @param categoryId ID da categoria de itens (0 = todos)
     * @param adjustmentFactor Fator de ajuste (1.0 = sem mudança)
     * @return Resultado da operação
     */
    EconomyOperationResult adjustNpcPrices(
        int32_t categoryId = 0,
        float adjustmentFactor = 1.0f);
    
    /**
     * @brief Ajusta taxas de drop para equilibrar a economia
     * @param itemTypeId ID do tipo de item (0 = todos)
     * @param adjustmentFactor Fator de ajuste (1.0 = sem mudança)
     * @return Resultado da operação
     */
    EconomyOperationResult adjustDropRates(
        int32_t itemTypeId = 0,
        float adjustmentFactor = 1.0f);
    
    /**
     * @brief Define limites de comércio entre jogadores
     * @param maxValuePerTrade Valor máximo por transação
     * @param maxDailyValue Valor máximo diário
     * @return Resultado da operação
     */
    EconomyOperationResult setTradeLimits(
        int64_t maxValuePerTrade,
        int64_t maxDailyValue);
    
    /**
     * @brief Define taxas de transação
     * @param transactionType Tipo de transação
     * @param percentage Percentual da taxa
     * @return Resultado da operação
     */
    EconomyOperationResult setTransactionTax(
        TransactionType transactionType,
        float percentage);
    
    /**
     * @brief Introduz ou remove dinheiro da economia
     * @param amount Quantidade (positiva para introduzir, negativa para remover)
     * @param method Método de distribuição ("all-players", "active-players", "by-level", etc.)
     * @param reason Motivo da alteração
     * @return Resultado da operação
     */
    EconomyOperationResult adjustGoldSupply(
        int64_t amount,
        const std::string& method,
        const std::string& reason);
    
    /**
     * @brief Gera um relatório econômico detalhado
     * @param timeFrame Período de tempo em dias
     * @param format Formato do relatório ("pdf", "html", "json", "txt")
     * @return Dados do relatório gerado
     */
    std::vector<uint8_t> generateEconomicReport(
        int32_t timeFrame,
        const std::string& format);
    
    /**
     * @brief Obtém estatísticas de atividade econômica
     * @param timeFrame Período de tempo em dias
     * @return Mapa de estatísticas
     */
    std::map<std::string, std::variant<int32_t, int64_t, float, std::string>> getEconomicActivityStats(
        int32_t timeFrame = 30);
    
    /**
     * @brief Define o valor base de um item
     * @param itemId ID do item
     * @param baseValue Valor base
     * @return Resultado da operação
     */
    EconomyOperationResult setItemBaseValue(
        int32_t itemId,
        int32_t baseValue);
    
    /**
     * @brief Registra um callback para notificações econômicas
     * @param eventType Tipo de evento ("price-change", "high-volume", "intervention", etc.)
     * @param callback Função de callback
     * @return ID de registro (para cancelamento posterior)
     */
    uint32_t registerEconomyCallback(
        const std::string& eventType,
        std::function<void(const json&)> callback);
    
    /**
     * @brief Cancela registro de callback
     * @param callbackId ID do callback a cancelar
     */
    void unregisterCallback(uint32_t callbackId);

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade principal
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Cache e dados internos
    std::map<int32_t, EconomicItemData> m_itemDataCache;
    std::map<int32_t, EconomicIntervention> m_interventionsCache;
    std::vector<EconomicIndicator> m_indicatorsCache;
    
    // Métodos de acesso a dados
    void loadItemEconomicData();
    void loadInterventions();
    void loadEconomicIndicators();
    
    void saveItemEconomicData(const EconomicItemData& itemData);
    void saveIntervention(const EconomicIntervention& intervention);
    
    // Métodos de análise
    EconomicHealthStatus analyzeEconomicHealth();
    std::vector<std::string> generateEconomicRecommendations();
    
    // Métodos de validação
    bool validateIntervention(const EconomicIntervention& intervention, std::string& errorMessage);
    
    // Métodos auxiliares
    int32_t generateUniqueId(const std::string& entityType);
    std::string serializeInterventionToJson(const EconomicIntervention& intervention);
    EconomicIntervention deserializeInterventionFromJson(const std::string& json);
};

}} // namespace wydstudio::ui

#endif // WYDSTUDIO_ECONOMY_MANAGER_H