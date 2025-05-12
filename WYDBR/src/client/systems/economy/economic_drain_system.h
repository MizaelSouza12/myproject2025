/**
 * @file economic_drain_system.h
 * @brief Sistema multi-camada de drenos econômicos
 * 
 * Este arquivo define o sistema de drenos econômicos para o WYDBR,
 * implementando mecanismos de balanceamento da economia in-game
 * com adaptação dinâmica baseada em métricas econômicas.
 * 
 * @author WYDBR Team
 * @version 1.0.0
 * @date 2025-05-06
 */

#ifndef WYDBR_ECONOMIC_DRAIN_SYSTEM_H
#define WYDBR_ECONOMIC_DRAIN_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <queue>
#include <deque>
#include <random>
#include <chrono>

namespace wydbr {
namespace systems {
namespace economy {

/**
 * @enum DrainType
 * @brief Tipos de drenos econômicos
 */
enum /**
 * Classe WYDDrainType
 * 
 * Esta classe implementa a funcionalidade WYDDrainType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DrainType {
    TRANSACTION_FEE,      ///< Taxa de transação
    AUCTION_FEE,          ///< Taxa de leilão
    REPAIR_COST,          ///< Custo de reparo
    ENCHANTMENT_COST,     ///< Custo de encantamento
    TELEPORT_COST,        ///< Custo de teleporte
    RESPAWN_COST,         ///< Custo de ressurreição
    MAINTENANCE_COST,     ///< Custo de manutenção
    GUILD_TAX,            ///< Taxa de guilda
    VENDOR_MARKUP,        ///< Markup de vendedor
    VENDOR_DISCOUNT,      ///< Desconto de vendedor
    STORAGE_COST,         ///< Custo de armazenamento
    LUXURY_ITEM,          ///< Item de luxo
    COSMETIC_ITEM,        ///< Item cosmético
    UPGRADE_COST,         ///< Custo de upgrade
    GUILD_INVESTMENT,     ///< Investimento em guilda
    MARKET_TAX,           ///< Taxa de mercado
    DEATH_PENALTY,        ///< Penalidade por morte
    DURABILITY_LOSS,      ///< Perda de durabilidade
    SERVICE_FEE,          ///< Taxa de serviço
    CONTENT_ACCESS,       ///< Acesso a conteúdo
    CUSTOM                ///< Personalizado
};

/**
 * @enum DrainPriority
 * @brief Prioridade de aplicação de drenos
 */
enum /**
 * Classe WYDDrainPriority
 * 
 * Esta classe implementa a funcionalidade WYDDrainPriority conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DrainPriority {
    CRITICAL = 0,     ///< Crítica (sempre aplicada)
    HIGH = 1,         ///< Alta
    MEDIUM = 2,       ///< Média
    LOW = 3,          ///< Baixa
    OPTIONAL = 4      ///< Opcional (pode ser ignorada)
};

/**
 * @enum DrainCalculationMethod
 * @brief Método de cálculo do valor do dreno
 */
enum /**
 * Classe WYDDrainCalculationMethod
 * 
 * Esta classe implementa a funcionalidade WYDDrainCalculationMethod conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DrainCalculationMethod {
    FIXED,          ///< Valor fixo
    PERCENTAGE,     ///< Porcentagem do valor base
    TIERED,         ///< Baseado em níveis
    PROGRESSIVE,    ///< Progressivo com base em valor
    FORMULA,        ///< Fórmula customizada
    TIME_BASED      ///< Baseado em tempo
};

/**
 * @enum DrainCondition
 * @brief Condição de aplicação do dreno
 */
enum /**
 * Classe WYDDrainCondition
 * 
 * Esta classe implementa a funcionalidade WYDDrainCondition conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DrainCondition {
    ALWAYS,             ///< Sempre aplica
    PLAYER_LEVEL,       ///< Baseado no nível do jogador
    PLAYER_REPUTATION,  ///< Baseado na reputação do jogador
    PLAYER_WEALTH,      ///< Baseado na riqueza do jogador
    GUILD_SIZE,         ///< Baseado no tamanho da guilda
    GUILD_LEVEL,        ///< Baseado no nível da guilda
    ITEM_TIER,          ///< Baseado no tier do item
    ITEM_LEVEL,         ///< Baseado no nível do item
    ITEM_RARITY,        ///< Baseado na raridade do item
    TRANSACTION_SIZE,   ///< Baseado no tamanho da transação
    LOCATION,           ///< Baseado na localização
    TIME_OF_DAY,        ///< Baseado na hora do dia
    DAY_OF_WEEK,        ///< Baseado no dia da semana
    CUSTOM              ///< Personalizada
};

/**
 * @typedef GoldAmount
 * @brief Tipo para quantidades de ouro
 */
using GoldAmount = int64_t;

/**
 * @typedef DrainId
 * @brief Tipo para identificadores de dreno
 */
using DrainId = std::string;

/**
 * @typedef EntityId
 * @brief Tipo para identificadores de entidade
 */
using EntityId = std::string;

/**
 * @typedef FormulaFunction
 * @brief Tipo para funções de fórmula personalizada
 */
using FormulaFunction = std::function<GoldAmount(GoldAmount, const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>&)>;

/**
 * @struct DrainTier
 * @brief Define um nível em um dreno baseado em tiers
 */
struct DrainTier {
    GoldAmount threshold;       ///< Limite inferior do tier
    std::string type;           ///< Tipo de cálculo (fixed, percentage)
    double value;               ///< Valor do tier
};

/**
 * @struct DrainConfig
 * @brief Configuração de um dreno econômico
 */
/**
 * Classe WYDDrainConfig
 * 
 * Esta classe implementa a funcionalidade WYDDrainConfig conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DrainConfig {
public:
    /**
     * @brief Construtor
     * @param id ID do dreno
     * @param options Opções de configuração
     */
    DrainConfig(const std::string& id, const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool, std::vector<DrainTier>, FormulaFunction>>& options);
    
    /**
     * @brief Destrutor
     */
    ~DrainConfig() = default;
    
    /**
     * @brief Calcula o valor do dreno
     * @param baseAmount Valor base para cálculo
     * @param context Contexto da operação
     * @return Valor calculado do dreno
     */
    GoldAmount calculate(GoldAmount baseAmount, const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& context) const;
    
    /**
     * @brief Verifica se o dreno deve ser aplicado
     * @param context Contexto da operação
     * @return true se deve ser aplicado, false caso contrário
     */
    bool shouldApply(const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& context) const;
    
    /**
     * @brief Obtém o ID do dreno
     * @return ID do dreno
     */
    const std::string& getId() const { return id_; }
    
    /**
     * @brief Obtém o nome do dreno
     * @return Nome do dreno
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Obtém a descrição do dreno
     * @return Descrição do dreno
     */
    const std::string& getDescription() const { return description_; }
    
    /**
     * @brief Obtém o tipo do dreno
     * @return Tipo do dreno
     */
    DrainType getType() const { return type_; }
    
    /**
     * @brief Obtém a prioridade do dreno
     * @return Prioridade do dreno
     */
    DrainPriority getPriority() const { return priority_; }
    
    /**
     * @brief Obtém o método de cálculo do dreno
     * @return Método de cálculo
     */
    DrainCalculationMethod getCalculationMethod() const { return calculationMethod_; }
    
    /**
     * @brief Obtém a condição de aplicação do dreno
     * @return Condição de aplicação
     */
    DrainCondition getCondition() const { return condition_; }
    
    /**
     * @brief Obtém o valor base do dreno
     * @return Valor base
     */
    double getBaseValue() const { return baseValue_; }
    
    /**
     * @brief Define o valor base do dreno
     * @param value Novo valor base
     */
    void setBaseValue(double value) { baseValue_ = value; }
    
    /**
     * @brief Obtém o valor mínimo do dreno
     * @return Valor mínimo
     */
    GoldAmount getMinValue() const { return minValue_; }
    
    /**
     * @brief Obtém o valor máximo do dreno
     * @return Valor máximo
     */
    GoldAmount getMaxValue() const { return maxValue_; }
    
    /**
     * @brief Obtém o impacto econômico alvo do dreno
     * @return Impacto econômico alvo (0.0-1.0)
     */
    float getTargetEconomyImpact() const { return targetEconomyImpact_; }
    
    /**
     * @brief Obtém os parâmetros da condição
     * @return Parâmetros da condição
     */
    const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& getConditionParams() const { return conditionParams_; }
    
    /**
     * @brief Obtém os fatores adaptativos
     * @return Fatores adaptativos
     */
    const std::unordered_map<std::string, float>& getAdaptiveFactors() const { return adaptiveFactors_; }
    
    /**
     * @brief Obtém os tiers do dreno
     * @return Tiers do dreno
     */
    const std::vector<DrainTier>& getTiers() const { return tiers_; }
    
private:
    std::string id_;                    ///< ID único do dreno
    std::string name_;                  ///< Nome do dreno
    std::string description_;           ///< Descrição do dreno
    DrainType type_;                    ///< Tipo de dreno
    DrainPriority priority_;            ///< Prioridade de aplicação
    DrainCalculationMethod calculationMethod_; ///< Método de cálculo
    DrainCondition condition_;          ///< Condição de aplicação
    double baseValue_;                  ///< Valor base para cálculo
    GoldAmount minValue_;               ///< Valor mínimo
    GoldAmount maxValue_;               ///< Valor máximo
    float targetEconomyImpact_;         ///< Impacto econômico alvo (0.0-1.0)
    
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> conditionParams_; ///< Parâmetros da condição
    std::unordered_map<std::string, float> adaptiveFactors_; ///< Fatores adaptativos
    std::vector<DrainTier> tiers_;      ///< Tiers para cálculo (se aplicável)
    std::optional<FormulaFunction> formula_; ///< Fórmula personalizada (se aplicável)
    
    /**
     * @brief Calcula com método fixo
     * @param baseAmount Valor base
     * @return Valor calculado
     */
    GoldAmount calculateFixed(GoldAmount baseAmount) const;
    
    /**
     * @brief Calcula com método percentual
     * @param baseAmount Valor base
     * @return Valor calculado
     */
    GoldAmount calculatePercentage(GoldAmount baseAmount) const;
    
    /**
     * @brief Calcula com método em tiers
     * @param baseAmount Valor base
     * @return Valor calculado
     */
    GoldAmount calculateTiered(GoldAmount baseAmount) const;
    
    /**
     * @brief Calcula com método progressivo
     * @param baseAmount Valor base
     * @param context Contexto da operação
     * @return Valor calculado
     */
    GoldAmount calculateProgressive(GoldAmount baseAmount, const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& context) const;
    
    /**
     * @brief Calcula com método baseado em tempo
     * @param baseAmount Valor base
     * @param context Contexto da operação
     * @return Valor calculado
     */
    GoldAmount calculateTimeBased(GoldAmount baseAmount, const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& context) const;
    
    /**
     * @brief Calcula com método de fórmula
     * @param baseAmount Valor base
     * @param context Contexto da operação
     * @return Valor calculado
     */
    GoldAmount calculateFormula(GoldAmount baseAmount, const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& context) const;
};

/**
 * @struct DrainDetail
 * @brief Detalhes da aplicação de um dreno
 */
struct DrainDetail {
    DrainId drainId;         ///< ID do dreno
    std::string name;        ///< Nome do dreno
    GoldAmount amount;       ///< Valor aplicado
    DrainType type;          ///< Tipo de dreno
    DrainPriority priority;  ///< Prioridade do dreno
};

/**
 * @struct DrainResult
 * @brief Resultado do cálculo de drenos
 */
struct DrainResult {
    GoldAmount total;                ///< Total de drenos aplicados
    std::vector<DrainDetail> details; ///< Detalhes por dreno
};

/**
 * @struct AppliedDrain
 * @brief Registro de um dreno aplicado
 */
struct AppliedDrain {
    std::string drainId;        ///< ID do dreno
    std::string entityId;       ///< ID da entidade afetada
    std::string entityType;     ///< Tipo da entidade
    std::string transactionId;  ///< ID da transação
    std::string transactionType;///< Tipo de transação
    GoldAmount amount;          ///< Valor drenado
    GoldAmount baseAmount;      ///< Valor base da transação
    std::chrono::system_clock::time_point timestamp; ///< Timestamp da aplicação
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> metadata; ///< Metadados adicionais
};

/**
 * @class IEconomyTracker
 * @brief Interface para rastreamento de métricas econômicas
 */
/**
 * Classe WYDIEconomyTracker
 * 
 * Esta classe implementa a funcionalidade WYDIEconomyTracker conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class IEconomyTracker {
public:
    /**
     * @brief Destrutor virtual
     */
    virtual ~IEconomyTracker() = default;
    
    /**
     * @brief Obtém métricas econômicas atuais
     * @return Mapa com métricas
     */
    virtual std::unordered_map<std::string, double> getEconomyMetrics() = 0;
};

/**
 * @class EconomicDrainSystem
 * @brief Sistema completo de drenos econômicos
 */
/**
 * Classe WYDEconomicDrainSystem
 * 
 * Esta classe implementa a funcionalidade WYDEconomicDrainSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EconomicDrainSystem {
public:
    /**
     * @typedef DrainCallback
     * @brief Tipo para callback de eventos de dreno
     */
    using DrainCallback = std::function<void(const std::unordered_map<std::string, std::variant<std::string, int64_t, double, bool>>&)>;
    
    /**
     * @brief Construtor
     * @param options Opções de configuração
     */
    explicit EconomicDrainSystem(const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool, std::shared_ptr<IEconomyTracker>>>& options = {});
    
    /**
     * @brief Destrutor
     */
    ~EconomicDrainSystem();
    
    /**
     * @brief Inicializa o sistema
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool initialize();
    
    /**
     * @brief Finaliza o sistema
     */
    void shutdown();
    
    /**
     * @brief Registra configuração de dreno
     * @param drainId ID do dreno
     * @param config Configuração do dreno
     * @return true se registrado com sucesso, false caso contrário
     */
    bool registerDrainConfig(const std::string& drainId, const DrainConfig& config);
    
    /**
     * @brief Remove configuração de dreno
     * @param drainId ID do dreno
     * @return true se removido com sucesso, false caso contrário
     */
    bool unregisterDrainConfig(const std::string& drainId);
    
    /**
     * @brief Calcula drenos para uma transação
     * @param amount Valor da transação
     * @param drainType Tipo de dreno a aplicar
     * @param context Contexto da transação
     * @return Resultado do cálculo de drenos
     */
    DrainResult calculateTotalDrain(GoldAmount amount, DrainType drainType, 
                                   const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& context = {});
    
    /**
     * @brief Registra drenos aplicados
     * @param baseAmount Valor base da transação
     * @param drainResult Resultado do cálculo de drenos
     * @param context Contexto da aplicação
     * @return true se registrado com sucesso, false caso contrário
     */
    bool recordAppliedDrains(GoldAmount baseAmount, const DrainResult& drainResult, 
                           const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& context);
    
    /**
     * @brief Adapta drenos baseado em métricas econômicas
     * @return true se adaptado com sucesso, false caso contrário
     */
    bool adaptDrainsBasedOnMetrics();
    
    /**
     * @brief Obtém todas as configurações de dreno
     * @return Vetor com configurações
     */
    std::vector<DrainConfig> getAllDrainConfigs() const;
    
    /**
     * @brief Obtém uma configuração de dreno específica
     * @param drainId ID do dreno
     * @return Configuração do dreno ou std::nullopt
     */
    std::optional<DrainConfig> getDrainConfig(const std::string& drainId) const;
    
    /**
     * @brief Obtém estatísticas do sistema
     * @return Mapa com estatísticas
     */
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> getStatistics() const;
    
    /**
     * @brief Registra callback para evento
     * @param eventType Tipo de evento
     * @param callback Função de callback
     * @return ID do registro
     */
    uint64_t addEventListener(const std::string& eventType, DrainCallback callback);
    
    /**
     * @brief Remove callback registrado
     * @param eventType Tipo de evento
     * @param callbackId ID do registro
     * @return true se removido com sucesso, false caso contrário
     */
    bool removeEventListener(const std::string& eventType, uint64_t callbackId);
    
    /**
     * @brief Simula impacto econômico do sistema
     * @param days Número de dias a simular
     * @param params Parâmetros da simulação
     * @return Resultado da simulação
     */
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool, std::vector<std::unordered_map<std::string, double>>>> 
    simulateEconomicImpact(int days, const std::unordered_map<std::string, double>& params);
    
private:
    bool initialized_ = false;          ///< Se foi inicializado
    bool adaptationEnabled_ = true;     ///< Se adaptação automática está habilitada
    bool debugMode_ = false;            ///< Se modo de debug está ativado
    float targetInflationRate_ = 2.0f;  ///< Taxa de inflação alvo
    uint64_t adaptationInterval_ = 86400000; ///< Intervalo de adaptação (ms)
    uint64_t lastAdaptationTime_ = 0;   ///< Timestamp da última adaptação
    
    std::unordered_map<std::string, DrainConfig> drainConfigs_; ///< Configurações de dreno
    std::deque<AppliedDrain> recentDrains_; ///< Drenos recentemente aplicados
    uint32_t maxRecentDrains_ = 1000;   ///< Máximo de drenos recentes a armazenar
    
    std::shared_ptr<IEconomyTracker> economyTracker_; ///< Rastreador de métricas econômicas
    
    // Estatísticas
    struct Statistics {
        GoldAmount totalDrained = 0;
        uint64_t totalTransactions = 0;
        std::unordered_map<DrainType, std::pair<GoldAmount, uint64_t>> drainsByType;
        std::unordered_map<std::string, std::pair<GoldAmount, uint64_t>> drainsById;
        uint64_t adaptationsPerformed = 0;
        double averageDrainPercentage = 0.0;
        uint64_t highestSingleDrain = 0;
    } stats_;
    
    // Callbacks
    struct CallbackEntry {
        uint64_t id;
        DrainCallback callback;
    };
    std::unordered_map<std::string, std::vector<CallbackEntry>> callbacks_;
    uint64_t nextCallbackId_ = 1;
    
    // Mutex para proteção de acesso concorrente
    mutable std::mutex mutex_;
    
    // Timer para adaptação periódica
    std::unique_ptr<std::thread> adaptationThread_;
    bool adaptationThreadRunning_ = false;
    
    /**
     * @brief Thread de adaptação periódica
     */
    void adaptationThreadFunction();
    
    /**
     * @brief Calcula fator de ajuste baseado em métricas
     * @param metrics Métricas econômicas
     * @return Fator de ajuste
     */
    float calculateAdjustmentFactor(const std::unordered_map<std::string, double>& metrics);
    
    /**
     * @brief Dispara evento
     * @param eventType Tipo de evento
     * @param data Dados do evento
     */
    void fireEvent(const std::string& eventType, const std::unordered_map<std::string, std::variant<std::string, int64_t, double, bool>>& data);
    
    /**
     * @brief Filtra drenos por tipo
     * @param drainType Tipo de dreno
     * @return Vetor com configurações
     */
    std::vector<DrainConfig> filterDrainsByType(DrainType drainType) const;
    
    /**
     * @brief Registra valor drenado nas estatísticas
     * @param drainId ID do dreno
     * @param drainType Tipo de dreno
     * @param amount Valor drenado
     */
    void updateStatistics(const std::string& drainId, DrainType drainType, GoldAmount amount);
    
    /**
     * @brief Log de debug
     * @param message Mensagem a logar
     */
    void debug(const std::string& message);
};

} // namespace economy
} // namespace systems
} // namespace wydbr

#endif // WYDBR_ECONOMIC_DRAIN_SYSTEM_H