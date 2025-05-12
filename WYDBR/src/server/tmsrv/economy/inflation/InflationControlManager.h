/**
 * InflationControlManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/economy/inflation/InflationControlManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef INFLATIONCONTROLMANAGER_H
#define INFLATIONCONTROLMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <random>

#include "../../../../core/WYDTypes.h"
#include "../EconomyTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file InflationControlManager.h
 * @brief Gerenciador do sistema de controle de inflação
 * 
 * Este arquivo contém a definição do gerenciador de controle de inflação,
 * responsável por monitorar e ajustar a economia do jogo para manter a
 * estabilidade econômica. Corrige problemas do sistema original do WYD
 * que levavam à hiperinflação devido à falta de mecanismos de controle.
 */

namespace wyd {
namespace server {
namespace economy {
namespace inflation {

/**
 * @brief Período de análise econômica
 */
enum class EconomicPeriod : BYTE {
    DAILY = 0,             // Diário
    WEEKLY = 1,            // Semanal
    MONTHLY = 2,           // Mensal
    QUARTERLY = 3,         // Trimestral
    YEARLY = 4,            // Anual
    CUSTOM = 5,            // Personalizado
};

/**
 * @brief Tipo de sumidouro de ouro
 */
enum class GoldSinkType : BYTE {
    MARKET_TAX = 0,         // Imposto de mercado
    REPAIR_COST = 1,        // Custo de reparo
    SKILL_COST = 2,         // Custo de skill
    TELEPORT_COST = 3,      // Custo de teleporte
    STORAGE_COST = 4,       // Custo de armazenamento
    ENCHANT_COST = 5,       // Custo de encantamento
    REFINE_COST = 6,        // Custo de refinamento
    DEATH_PENALTY = 7,      // Penalidade por morte
    PURCHASE_COST = 8,      // Custo de compra (NPC)
    GUILD_COST = 9,         // Custo de guild
    PREMIUM_SERVICE = 10,   // Serviço premium
    CUSTOM = 11,            // Personalizado
};

/**
 * @brief Tipo de fonte de ouro
 */
enum class GoldFaucetType : BYTE {
    MOB_DROP = 0,           // Drop de monstro
    QUEST_REWARD = 1,       // Recompensa de quest
    SELL_TO_NPC = 2,        // Venda para NPC
    MARKET_SALE = 3,        // Venda no mercado
    EVENT_REWARD = 4,       // Recompensa de evento
    GUILD_REWARD = 5,       // Recompensa de guild
    DAILY_BONUS = 6,        // Bônus diário
    ACHIEVEMENT = 7,        // Conquista
    CUSTOM = 8,             // Personalizado
};

/**
 * @brief Nível de severidade de inflação
 */
enum class InflationSeverity : BYTE {
    NONE = 0,               // Nenhum
    VERY_LOW = 1,           // Muito baixo
    LOW = 2,                // Baixo
    MODERATE = 3,           // Moderado
    HIGH = 4,               // Alto
    VERY_HIGH = 5,          // Muito alto
    EXTREME = 6,            // Extremo
    HYPERINFLATION = 7,     // Hiperinflação
    CUSTOM = 8,             // Personalizado
};

/**
 * @brief Nível de severidade de deflação
 */
enum class DeflationSeverity : BYTE {
    NONE = 0,               // Nenhum
    VERY_LOW = 1,           // Muito baixo
    LOW = 2,                // Baixo
    MODERATE = 3,           // Moderado
    HIGH = 4,               // Alto
    VERY_HIGH = 5,          // Muito alto
    EXTREME = 6,            // Extremo
    CUSTOM = 7,             // Personalizado
};

/**
 * @brief Estratégia de controle de inflação
 */
enum class InflationControlStrategy : BYTE {
    DO_NOTHING = 0,         // Não fazer nada
    INCREASE_GOLD_SINKS = 1, // Aumentar sumidouros de ouro
    DECREASE_GOLD_FAUCETS = 2, // Diminuir fontes de ouro
    PRICE_CONTROLS = 3,     // Controles de preço
    INTRODUCE_NEW_ITEMS = 4, // Introduzir novos itens
    TAX_ADJUSTMENT = 5,     // Ajuste de impostos
    SPECIAL_EVENT = 6,      // Evento especial
    CURRENCY_ADJUSTMENT = 7, // Ajuste de moeda
    CUSTOM = 8,             // Personalizado
};

/**
 * @brief Estratégia de controle de deflação
 */
enum class DeflationControlStrategy : BYTE {
    DO_NOTHING = 0,         // Não fazer nada
    DECREASE_GOLD_SINKS = 1, // Diminuir sumidouros de ouro
    INCREASE_GOLD_FAUCETS = 2, // Aumentar fontes de ouro
    PRICE_SUBSIDIES = 3,    // Subsídios de preço
    ECONOMIC_STIMULUS = 4,  // Estímulo econômico
    TAX_REDUCTION = 5,      // Redução de impostos
    SPECIAL_EVENT = 6,      // Evento especial
    CURRENCY_INJECTION = 7, // Injeção de moeda
    CUSTOM = 8,             // Personalizado
};

/**
 * @brief Status de implementação de estratégia
 */
enum class StrategyStatus : BYTE {
    PROPOSED = 0,           // Proposto
    APPROVED = 1,           // Aprovado
    IMPLEMENTED = 2,        // Implementado
    COMPLETED = 3,          // Completo
    FAILED = 4,             // Falhou
    CANCELLED = 5,          // Cancelado
    CUSTOM = 6,             // Personalizado
};

/**
 * @brief Medição econômica
 */
struct EconomicMeasurement {
    DWORD measurementID;           // ID da medição
    EconomicPeriod period;         // Período
    time_t startTime;               // Hora de início
    time_t endTime;                 // Hora de término
    DWORD totalGoldInEconomy;       // Total de ouro na economia
    DWORD totalGoldPerPlayer;       // Total de ouro por jogador
    DWORD totalGoldSinks;           // Total de sumidouros de ouro
    DWORD totalGoldFaucets;         // Total de fontes de ouro
    float inflationRate;             // Taxa de inflação
    float averageItemPrice;          // Preço médio de item
    DWORD activePlayerCount;        // Contagem de jogadores ativos
    DWORD transactionCount;         // Contagem de transações
    float marketLiquidity;           // Liquidez de mercado
    float economicActivity;          // Atividade econômica
    std::string notes;               // Notas
    
    EconomicMeasurement()
        : measurementID(0)
        , period(EconomicPeriod::DAILY)
        , startTime(0)
        , endTime(0)
        , totalGoldInEconomy(0)
        , totalGoldPerPlayer(0)
        , totalGoldSinks(0)
        , totalGoldFaucets(0)
        , inflationRate(0.0f)
        , averageItemPrice(0.0f)
        , activePlayerCount(0)
        , transactionCount(0)
        , marketLiquidity(0.0f)
        , economicActivity(0.0f)
        , notes("")
    {
    }
};

/**
 * @brief Sumidouro de ouro
 */
struct GoldSink {
    DWORD sinkID;                  // ID do sumidouro
    GoldSinkType type;             // Tipo
    std::string name;               // Nome
    std::string description;        // Descrição
    DWORD baseAmount;              // Valor base
    float adjustmentFactor;         // Fator de ajuste
    bool enabled;                   // Habilitado
    bool dynamic;                   // Dinâmico
    float inflationSensitivity;     // Sensibilidade à inflação
    
    GoldSink()
        : sinkID(0)
        , type(GoldSinkType::MARKET_TAX)
        , name("")
        , description("")
        , baseAmount(0)
        , adjustmentFactor(1.0f)
        , enabled(true)
        , dynamic(false)
        , inflationSensitivity(1.0f)
    {
    }
};

/**
 * @brief Fonte de ouro
 */
struct GoldFaucet {
    DWORD faucetID;                // ID da fonte
    GoldFaucetType type;           // Tipo
    std::string name;               // Nome
    std::string description;        // Descrição
    DWORD baseAmount;              // Valor base
    float adjustmentFactor;         // Fator de ajuste
    bool enabled;                   // Habilitado
    bool dynamic;                   // Dinâmico
    float inflationSensitivity;     // Sensibilidade à inflação
    
    GoldFaucet()
        : faucetID(0)
        , type(GoldFaucetType::MOB_DROP)
        , name("")
        , description("")
        , baseAmount(0)
        , adjustmentFactor(1.0f)
        , enabled(true)
        , dynamic(false)
        , inflationSensitivity(1.0f)
    {
    }
};

/**
 * @brief Estratégia de inflação
 */
struct InflationStrategy {
    DWORD strategyID;                     // ID da estratégia
    InflationControlStrategy type;        // Tipo
    std::string name;                      // Nome
    std::string description;               // Descrição
    InflationSeverity targetSeverity;     // Severidade alvo
    float expectedImpact;                  // Impacto esperado
    time_t implementationTime;             // Hora de implementação
    time_t expiryTime;                     // Hora de expiração
    StrategyStatus status;                // Status
    std::string parameters;                // Parâmetros (JSON)
    std::string results;                   // Resultados
    
    InflationStrategy()
        : strategyID(0)
        , type(InflationControlStrategy::DO_NOTHING)
        , name("")
        , description("")
        , targetSeverity(InflationSeverity::NONE)
        , expectedImpact(0.0f)
        , implementationTime(0)
        , expiryTime(0)
        , status(StrategyStatus::PROPOSED)
        , parameters("")
        , results("")
    {
    }
};

/**
 * @brief Estratégia de deflação
 */
struct DeflationStrategy {
    DWORD strategyID;                     // ID da estratégia
    DeflationControlStrategy type;        // Tipo
    std::string name;                      // Nome
    std::string description;               // Descrição
    DeflationSeverity targetSeverity;     // Severidade alvo
    float expectedImpact;                  // Impacto esperado
    time_t implementationTime;             // Hora de implementação
    time_t expiryTime;                     // Hora de expiração
    StrategyStatus status;                // Status
    std::string parameters;                // Parâmetros (JSON)
    std::string results;                   // Resultados
    
    DeflationStrategy()
        : strategyID(0)
        , type(DeflationControlStrategy::DO_NOTHING)
        , name("")
        , description("")
        , targetSeverity(DeflationSeverity::NONE)
        , expectedImpact(0.0f)
        , implementationTime(0)
        , expiryTime(0)
        , status(StrategyStatus::PROPOSED)
        , parameters("")
        , results("")
    {
    }
};

/**
 * @brief Configuração de alerta de inflação
 */
struct InflationAlertConfig {
    DWORD configID;                // ID da configuração
    InflationSeverity level;        // Nível
    float thresholdRate;            // Taxa limite
    std::string message;             // Mensagem
    std::string actions;             // Ações (JSON)
    bool autoImplement;             // Implementar automaticamente
    bool notifyAdmins;              // Notificar administradores
    InflationControlStrategy defaultStrategy; // Estratégia padrão
    
    InflationAlertConfig()
        : configID(0)
        , level(InflationSeverity::NONE)
        , thresholdRate(0.0f)
        , message("")
        , actions("")
        , autoImplement(false)
        , notifyAdmins(true)
        , defaultStrategy(InflationControlStrategy::DO_NOTHING)
    {
    }
};

/**
 * @brief Configuração de alerta de deflação
 */
struct DeflationAlertConfig {
    DWORD configID;                // ID da configuração
    DeflationSeverity level;        // Nível
    float thresholdRate;            // Taxa limite
    std::string message;             // Mensagem
    std::string actions;             // Ações (JSON)
    bool autoImplement;             // Implementar automaticamente
    bool notifyAdmins;              // Notificar administradores
    DeflationControlStrategy defaultStrategy; // Estratégia padrão
    
    DeflationAlertConfig()
        : configID(0)
        , level(DeflationSeverity::NONE)
        , thresholdRate(0.0f)
        , message("")
        , actions("")
        , autoImplement(false)
        , notifyAdmins(true)
        , defaultStrategy(DeflationControlStrategy::DO_NOTHING)
    {
    }
};

/**
 * @brief Resultado de análise econômica
 */
struct EconomicAnalysisResult {
    float currentInflationRate;           // Taxa de inflação atual
    InflationSeverity inflationSeverity;  // Severidade da inflação
    DeflationSeverity deflationSeverity;  // Severidade da deflação
    float goldSinkRatio;                   // Relação de sumidouro de ouro
    float goldFaucetRatio;                 // Relação de fonte de ouro
    float economicHealthIndex;             // Índice de saúde econômica
    std::string recommendations;            // Recomendações
    std::vector<InflationStrategy> suggestedInflationStrategies; // Estratégias de inflação sugeridas
    std::vector<DeflationStrategy> suggestedDeflationStrategies; // Estratégias de deflação sugeridas
    bool needsIntervention;                // Necessita intervenção
    std::string analysisDetails;            // Detalhes da análise
};

/**
 * @brief Gerenciador de controle de inflação
 */
/**
 * Classe WYDInflationControlManager
 * 
 * Esta classe implementa a funcionalidade WYDInflationControlManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InflationControlManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static InflationControlManager& GetInstance();
    
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
     * @brief Carrega dados econômicos
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadEconomicData(const std::string& filePath);
    
    /**
     * @brief Salva dados econômicos
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveEconomicData(const std::string& filePath);
    
    /**
     * @brief Carrega configurações
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadConfigurations(const std::string& filePath);
    
    /**
     * @brief Salva configurações
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveConfigurations(const std::string& filePath);
    
    /**
     * @brief Realiza uma medição econômica
     * @param period Período
     * @param notes Notas
     * @return ID da medição, ou 0 se falhar
     */
    DWORD PerformEconomicMeasurement(EconomicPeriod period, const std::string& notes = "");
    
    /**
     * @brief Obtém a última medição econômica
     * @return Medição econômica
     */
    const EconomicMeasurement& GetLatestMeasurement() const;
    
    /**
     * @brief Obtém medições econômicas por período
     * @param period Período
     * @param count Número de medições
     * @return Lista de medições
     */
    std::vector<const EconomicMeasurement*> GetMeasurementsByPeriod(EconomicPeriod period, size_t count = 10) const;
    
    /**
     * @brief Realiza uma análise econômica
     * @param detailedAnalysis Se deve realizar análise detalhada
     * @return Resultado da análise
     */
    EconomicAnalysisResult PerformEconomicAnalysis(bool detailedAnalysis = false);
    
    /**
     * @brief Adiciona um sumidouro de ouro
     * @param type Tipo
     * @param name Nome
     * @param description Descrição
     * @param baseAmount Valor base
     * @param adjustmentFactor Fator de ajuste
     * @param dynamic Se é dinâmico
     * @param inflationSensitivity Sensibilidade à inflação
     * @return ID do sumidouro, ou 0 se falhar
     */
    DWORD AddGoldSink(GoldSinkType type, const std::string& name, const std::string& description,
                    DWORD baseAmount, float adjustmentFactor, bool dynamic, float inflationSensitivity);
    
    /**
     * @brief Adiciona uma fonte de ouro
     * @param type Tipo
     * @param name Nome
     * @param description Descrição
     * @param baseAmount Valor base
     * @param adjustmentFactor Fator de ajuste
     * @param dynamic Se é dinâmico
     * @param inflationSensitivity Sensibilidade à inflação
     * @return ID da fonte, ou 0 se falhar
     */
    DWORD AddGoldFaucet(GoldFaucetType type, const std::string& name, const std::string& description,
                      DWORD baseAmount, float adjustmentFactor, bool dynamic, float inflationSensitivity);
    
    /**
     * @brief Remove um sumidouro de ouro
     * @param sinkID ID do sumidouro
     * @return true se removido com sucesso
     */
    bool RemoveGoldSink(DWORD sinkID);
    
    /**
     * @brief Remove uma fonte de ouro
     * @param faucetID ID da fonte
     * @return true se removido com sucesso
     */
    bool RemoveGoldFaucet(DWORD faucetID);
    
    /**
     * @brief Obtém um sumidouro de ouro
     * @param sinkID ID do sumidouro
     * @return Ponteiro para o sumidouro, ou nullptr se não existir
     */
    const GoldSink* GetGoldSink(DWORD sinkID) const;
    
    /**
     * @brief Obtém uma fonte de ouro
     * @param faucetID ID da fonte
     * @return Ponteiro para a fonte, ou nullptr se não existir
     */
    const GoldFaucet* GetGoldFaucet(DWORD faucetID) const;
    
    /**
     * @brief Obtém todos os sumidouros de ouro
     * @return Lista de sumidouros
     */
    std::vector<const GoldSink*> GetAllGoldSinks() const;
    
    /**
     * @brief Obtém todas as fontes de ouro
     * @return Lista de fontes
     */
    std::vector<const GoldFaucet*> GetAllGoldFaucets() const;
    
    /**
     * @brief Ajusta um sumidouro de ouro
     * @param sinkID ID do sumidouro
     * @param adjustmentFactor Novo fator de ajuste
     * @param enabled Se está habilitado
     * @return true se ajustado com sucesso
     */
    bool AdjustGoldSink(DWORD sinkID, float adjustmentFactor, bool enabled);
    
    /**
     * @brief Ajusta uma fonte de ouro
     * @param faucetID ID da fonte
     * @param adjustmentFactor Novo fator de ajuste
     * @param enabled Se está habilitado
     * @return true se ajustado com sucesso
     */
    bool AdjustGoldFaucet(DWORD faucetID, float adjustmentFactor, bool enabled);
    
    /**
     * @brief Registra um gasto de ouro
     * @param playerID ID do jogador
     * @param amount Valor
     * @param sinkType Tipo de sumidouro
     * @param notes Notas
     * @return true se registrado com sucesso
     */
    bool RegisterGoldSpent(DWORD playerID, DWORD amount, GoldSinkType sinkType, const std::string& notes = "");
    
    /**
     * @brief Registra um ganho de ouro
     * @param playerID ID do jogador
     * @param amount Valor
     * @param faucetType Tipo de fonte
     * @param notes Notas
     * @return true se registrado com sucesso
     */
    bool RegisterGoldGained(DWORD playerID, DWORD amount, GoldFaucetType faucetType, const std::string& notes = "");
    
    /**
     * @brief Implementa uma estratégia de controle de inflação
     * @param strategy Estratégia
     * @return true se implementado com sucesso
     */
    bool ImplementInflationStrategy(const InflationStrategy& strategy);
    
    /**
     * @brief Implementa uma estratégia de controle de deflação
     * @param strategy Estratégia
     * @return true se implementado com sucesso
     */
    bool ImplementDeflationStrategy(const DeflationStrategy& strategy);
    
    /**
     * @brief Cancela uma estratégia de controle de inflação
     * @param strategyID ID da estratégia
     * @return true se cancelado com sucesso
     */
    bool CancelInflationStrategy(DWORD strategyID);
    
    /**
     * @brief Cancela uma estratégia de controle de deflação
     * @param strategyID ID da estratégia
     * @return true se cancelado com sucesso
     */
    bool CancelDeflationStrategy(DWORD strategyID);
    
    /**
     * @brief Obtém estatísticas de controle de inflação
     * @return String formatada com estatísticas
     */
    std::string GetInflationControlStats();
    
    /**
     * @brief Obtém o multiplicador de sumidouro de ouro
     * @param sinkType Tipo de sumidouro
     * @return Multiplicador
     */
    float GetGoldSinkMultiplier(GoldSinkType sinkType);
    
    /**
     * @brief Obtém o multiplicador de fonte de ouro
     * @param faucetType Tipo de fonte
     * @return Multiplicador
     */
    float GetGoldFaucetMultiplier(GoldFaucetType faucetType);
    
    /**
     * @brief Adiciona uma configuração de alerta de inflação
     * @param level Nível
     * @param thresholdRate Taxa limite
     * @param message Mensagem
     * @param actions Ações (JSON)
     * @param autoImplement Se deve implementar automaticamente
     * @param notifyAdmins Se deve notificar administradores
     * @param defaultStrategy Estratégia padrão
     * @return ID da configuração, ou 0 se falhar
     */
    DWORD AddInflationAlertConfig(InflationSeverity level, float thresholdRate, const std::string& message,
                               const std::string& actions, bool autoImplement, bool notifyAdmins,
                               InflationControlStrategy defaultStrategy);
    
    /**
     * @brief Adiciona uma configuração de alerta de deflação
     * @param level Nível
     * @param thresholdRate Taxa limite
     * @param message Mensagem
     * @param actions Ações (JSON)
     * @param autoImplement Se deve implementar automaticamente
     * @param notifyAdmins Se deve notificar administradores
     * @param defaultStrategy Estratégia padrão
     * @return ID da configuração, ou 0 se falhar
     */
    DWORD AddDeflationAlertConfig(DeflationSeverity level, float thresholdRate, const std::string& message,
                               const std::string& actions, bool autoImplement, bool notifyAdmins,
                               DeflationControlStrategy defaultStrategy);
    
    /**
     * @brief Remove uma configuração de alerta de inflação
     * @param configID ID da configuração
     * @return true se removido com sucesso
     */
    bool RemoveInflationAlertConfig(DWORD configID);
    
    /**
     * @brief Remove uma configuração de alerta de deflação
     * @param configID ID da configuração
     * @return true se removido com sucesso
     */
    bool RemoveDeflationAlertConfig(DWORD configID);
    
    /**
     * @brief Calcula a taxa de inflação
     * @param previousAvgPrice Preço médio anterior
     * @param currentAvgPrice Preço médio atual
     * @return Taxa de inflação
     */
    float CalculateInflationRate(float previousAvgPrice, float currentAvgPrice);
    
    /**
     * @brief Determina a severidade da inflação
     * @param inflationRate Taxa de inflação
     * @return Severidade
     */
    InflationSeverity DetermineInflationSeverity(float inflationRate);
    
    /**
     * @brief Determina a severidade da deflação
     * @param inflationRate Taxa de inflação
     * @return Severidade
     */
    DeflationSeverity DetermineDeflationSeverity(float inflationRate);
    
    /**
     * @brief Estima o impacto de uma estratégia de inflação
     * @param strategy Estratégia
     * @return Impacto estimado
     */
    float EstimateInflationStrategyImpact(const InflationStrategy& strategy);
    
    /**
     * @brief Estima o impacto de uma estratégia de deflação
     * @param strategy Estratégia
     * @return Impacto estimado
     */
    float EstimateDeflationStrategyImpact(const DeflationStrategy& strategy);
    
    /**
     * @brief Sugere estratégias de controle para uma taxa de inflação
     * @param inflationRate Taxa de inflação
     * @param detailedAnalysis Se deve realizar análise detalhada
     * @return Lista de estratégias
     */
    std::vector<InflationStrategy> SuggestInflationControlStrategies(float inflationRate, bool detailedAnalysis = false);
    
    /**
     * @brief Sugere estratégias de controle para uma taxa de deflação
     * @param inflationRate Taxa de inflação
     * @param detailedAnalysis Se deve realizar análise detalhada
     * @return Lista de estratégias
     */
    std::vector<DeflationStrategy> SuggestDeflationControlStrategies(float inflationRate, bool detailedAnalysis = false);
    
    /**
     * @brief Força a inflação para teste
     * @param rate Taxa de inflação alvo
     * @return true se aplicado com sucesso
     */
    bool ForceInflationForTesting(float rate);
    
    /**
     * @brief Força a deflação para teste
     * @param rate Taxa de deflação alvo
     * @return true se aplicado com sucesso
     */
    bool ForceDeflationForTesting(float rate);
    
    /**
     * @brief Define o nível de estabilidade econômica alvo
     * @param stabilityLevel Nível de estabilidade (0.0 a 1.0)
     * @return true se definido com sucesso
     */
    bool SetTargetEconomicStabilityLevel(float stabilityLevel);
    
    /**
     * @brief Obtém o nível de estabilidade econômica atual
     * @return Nível de estabilidade (0.0 a 1.0)
     */
    float GetCurrentEconomicStabilityLevel();
    
    /**
     * @brief Obtém o status do sistema de controle de inflação
     * @return String formatada com status
     */
    std::string GetInflationControlSystemStatus();
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    InflationControlManager();
    
    /**
     * @brief Destrutor
     */
    ~InflationControlManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    InflationControlManager(const InflationControlManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    InflationControlManager& operator=(const InflationControlManager&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Processa alertas de inflação
     * @param inflationRate Taxa de inflação
     * @param severity Severidade
     */
    void ProcessInflationAlerts(float inflationRate, InflationSeverity severity);
    
    /**
     * @brief Processa alertas de deflação
     * @param inflationRate Taxa de inflação
     * @param severity Severidade
     */
    void ProcessDeflationAlerts(float inflationRate, DeflationSeverity severity);
    
    /**
     * @brief Atualiza ajustes dinâmicos
     */
    void UpdateDynamicAdjustments();
    
    /**
     * @brief Implementa ajustes baseados em taxa de inflação
     * @param inflationRate Taxa de inflação
     * @param severity Severidade
     */
    void ImplementRateBasedAdjustments(float inflationRate, InflationSeverity severity);
    
    /**
     * @brief Gera um relatório econômico
     * @param detailedReport Se deve gerar relatório detalhado
     * @return String formatada com relatório
     */
    std::string GenerateEconomicReport(bool detailedReport = false);
    
    /**
     * @brief Verifica a eficácia das estratégias implementadas
     */
    void EvaluateStrategyEffectiveness();
    
    /**
     * @brief Calcula o índice de pressão econômica
     * @return Índice (0.0 a 1.0)
     */
    float CalculateEconomicPressureIndex();
    
    /**
     * @brief Estima o total de ouro na economia
     * @return Total estimado
     */
    DWORD EstimateTotalGoldInEconomy();
    
    /**
     * @brief Estima o total de ouro por jogador
     * @return Total por jogador
     */
    DWORD EstimateGoldPerPlayer();
    
    /**
     * @brief Obtém histórico de preços
     * @param itemID ID do item (0 para média geral)
     * @param timeRange Intervalo de tempo em dias
     * @return Lista de preços
     */
    std::vector<float> GetPriceHistory(WORD itemID = 0, BYTE timeRange = 30);
    
    // Medições econômicas
    std::vector<EconomicMeasurement> measurements_;
    mutable std::mutex measurementsMutex_;
    
    // Sumidouros de ouro
    std::map<DWORD, GoldSink> goldSinks_;
    mutable std::mutex goldSinksMutex_;
    
    // Fontes de ouro
    std::map<DWORD, GoldFaucet> goldFaucets_;
    mutable std::mutex goldFaucetsMutex_;
    
    // Estratégias de inflação
    std::map<DWORD, InflationStrategy> inflationStrategies_;
    mutable std::mutex inflationStrategiesMutex_;
    
    // Estratégias de deflação
    std::map<DWORD, DeflationStrategy> deflationStrategies_;
    mutable std::mutex deflationStrategiesMutex_;
    
    // Configurações de alerta de inflação
    std::vector<InflationAlertConfig> inflationAlertConfigs_;
    mutable std::mutex inflationAlertConfigsMutex_;
    
    // Configurações de alerta de deflação
    std::vector<DeflationAlertConfig> deflationAlertConfigs_;
    mutable std::mutex deflationAlertConfigsMutex_;
    
    // Índices para busca rápida
    std::map<GoldSinkType, std::vector<DWORD>> sinksByType_;
    std::map<GoldFaucetType, std::vector<DWORD>> faucetsByType_;
    std::map<InflationControlStrategy, std::vector<DWORD>> strategiesByType_;
    std::map<DeflationControlStrategy, std::vector<DWORD>> deflationStrategiesByType_;
    
    // Contadores para novos IDs
    std::atomic<DWORD> nextMeasurementID_;
    std::atomic<DWORD> nextSinkID_;
    std::atomic<DWORD> nextFaucetID_;
    std::atomic<DWORD> nextStrategyID_;
    std::atomic<DWORD> nextAlertConfigID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Configurações
    float targetStabilityLevel_;
    float maxAllowedInflationRate_;
    float minAllowedInflationRate_;
    bool autoAdjustEnabled_;
    DWORD measurementInterval_;
    float economicResponseFactor_;
    
    // Cache e estado
    float currentInflationRate_;
    float averageItemPrice_;
    time_t lastMeasurementTime_;
    time_t lastAdjustmentTime_;
    bool systemUnderStress_;
    
    // Gerador de números aleatórios
    std::mt19937 rng_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_InflationControlManager wyd::server::economy::inflation::InflationControlManager::GetInstance()

} // namespace inflation
} // namespace economy
} // namespace server
} // namespace wyd

#endif // INFLATIONCONTROLMANAGER_H

} // namespace wydbr
