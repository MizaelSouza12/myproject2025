#ifndef WYDSTUDIO_MACHINE_LEARNING_DETECTION_H
#define WYDSTUDIO_MACHINE_LEARNING_DETECTION_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <variant>
#include <functional>
#include <array>
#include <chrono>
#include <random>
#include <future>
#include <span>
#include <concepts>
#include <type_traits>
#include <filesystem>

#include "SecurityGuardian.h"
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace security {
namespace ml {

// Tipos e conceitos para ML
using FeatureVector = std::vector<float>;
using LabelType = std::variant<bool, int, float, std::string>;
using TimePoint = std::chrono::system_clock::time_point;
using ModelIdentifier = std::string;

// Feature descriptor
struct FeatureDescriptor {
    std::string name;
    std::string description;
    float minValue;
    float maxValue;
    float weight;
    bool isNormalized;
    
    FeatureDescriptor() : 
        minValue(0.0f), 
        maxValue(1.0f), 
        weight(1.0f), 
        isNormalized(true) {}
};

// Training sample
struct TrainingSample {
    FeatureVector features;
    LabelType label;
    float weight;
    TimePoint timestamp;
    
    TrainingSample() : 
        weight(1.0f), 
        timestamp(std::chrono::system_clock::now()) {}
};

// Prediction result
struct PredictionResult {
    LabelType predictedLabel;
    float confidence;
    std::unordered_map<std::string, float> classConfidences;
    std::vector<std::pair<std::string, float>> featureImportance;
    TimePoint timestamp;
    
    PredictionResult() : 
        confidence(0.0f), 
        timestamp(std::chrono::system_clock::now()) {}
    
    [[nodiscard]] bool asBool() const {
        if (std::holds_alternative<bool>(predictedLabel))
            return std::get<bool>(predictedLabel);
        if (std::holds_alternative<int>(predictedLabel))
            return std::get<int>(predictedLabel) > 0;
        if (std::holds_alternative<float>(predictedLabel))
            return std::get<float>(predictedLabel) > 0.5f;
        return false;
    }
    
    [[nodiscard]] float asFloat() const {
        if (std::holds_alternative<float>(predictedLabel))
            return std::get<float>(predictedLabel);
        if (std::holds_alternative<int>(predictedLabel))
            return static_cast<float>(std::get<int>(predictedLabel));
        if (std::holds_alternative<bool>(predictedLabel))
            return std::get<bool>(predictedLabel) ? 1.0f : 0.0f;
        return 0.0f;
    }
    
    [[nodiscard]] int asInt() const {
        if (std::holds_alternative<int>(predictedLabel))
            return std::get<int>(predictedLabel);
        if (std::holds_alternative<float>(predictedLabel))
            return static_cast<int>(std::get<float>(predictedLabel));
        if (std::holds_alternative<bool>(predictedLabel))
            return std::get<bool>(predictedLabel) ? 1 : 0;
        return 0;
    }
    
    [[nodiscard]] std::string asString() const {
        if (std::holds_alternative<std::string>(predictedLabel))
            return std::get<std::string>(predictedLabel);
        if (std::holds_alternative<bool>(predictedLabel))
            return std::get<bool>(predictedLabel) ? "true" : "false";
        if (std::holds_alternative<int>(predictedLabel))
            return std::to_string(std::get<int>(predictedLabel));
        if (std::holds_alternative<float>(predictedLabel))
            return std::to_string(std::get<float>(predictedLabel));
        return "";
    }
};

// Model performance metrics
struct ModelPerformanceMetrics {
    float accuracy;
    float precision;
    float recall;
    float f1Score;
    float auc;
    float falsePositiveRate;
    float falseNegativeRate;
    std::size_t sampleCount;
    TimePoint evaluationTime;
    
    ModelPerformanceMetrics() : 
        accuracy(0.0f), 
        precision(0.0f), 
        recall(0.0f), 
        f1Score(0.0f), 
        auc(0.0f), 
        falsePositiveRate(0.0f),
        falseNegativeRate(0.0f),
        sampleCount(0),
        evaluationTime(std::chrono::system_clock::now()) {}
    
    [[nodiscard]] float getBalancedAccuracy() const {
        return 0.5f * (recall + (1.0f - falsePositiveRate));
    }
};

// Training options
struct TrainingOptions {
    std::size_t epochs;
    float learningRate;
    float validationSplit;
    float regularizationStrength;
    std::size_t batchSize;
    bool shuffleData;
    bool earlyStoppingEnabled;
    std::size_t earlyStoppingPatience;
    std::string lossFunction;
    std::string optimizer;
    bool enableFeatureSelection;
    float featureSelectionThreshold;
    
    TrainingOptions() : 
        epochs(100),
        learningRate(0.01f),
        validationSplit(0.2f),
        regularizationStrength(0.001f),
        batchSize(32),
        shuffleData(true),
        earlyStoppingEnabled(true),
        earlyStoppingPatience(5),
        lossFunction("categorical_crossentropy"),
        optimizer("adam"),
        enableFeatureSelection(true),
        featureSelectionThreshold(0.01f) {}
};

// Interface para modelos de ML
class IMLModel {
public:
    virtual ~IMLModel() = default;
    
    // Inicialização e configuração
    virtual bool initialize() = 0;
    virtual void setFeatureDescriptors(const std::vector<FeatureDescriptor>& descriptors) = 0;
    
    // Treinamento
    virtual bool train(const std::vector<TrainingSample>& trainingSamples, 
                     const TrainingOptions& options) = 0;
    
    virtual bool trainIncrementally(const TrainingSample& sample) = 0;
    
    // Predição
    virtual PredictionResult predict(const FeatureVector& features) const = 0;
    
    virtual std::vector<PredictionResult> batchPredict(
        const std::vector<FeatureVector>& featuresBatch) const = 0;
    
    // Avaliação
    virtual ModelPerformanceMetrics evaluate(
        const std::vector<TrainingSample>& testSamples) const = 0;
    
    // Persistência
    virtual bool saveModel(const std::string& path) const = 0;
    virtual bool loadModel(const std::string& path) = 0;
    
    // Informações do modelo
    [[nodiscard]] virtual ModelIdentifier getModelIdentifier() const = 0;
    [[nodiscard]] virtual std::string getModelDescription() const = 0;
    [[nodiscard]] virtual std::vector<FeatureDescriptor> getFeatureDescriptors() const = 0;
    [[nodiscard]] virtual std::size_t getFeatureDimension() const = 0;
};

// Forward declaration de implementações de modelos
class RandomForestModel;
class NeuralNetworkModel;
class SupportVectorMachineModel;
class GradientBoostingModel;
class EnsembleModel;

/**
 * @brief Tipo de anomalia detectável
 */
enum class AnomalyType : std::uint8_t {
    UNKNOWN = 0,
    SPEED_HACK = 1,
    TELEPORT_HACK = 2, 
    COMBAT_HACK = 3,
    AUTO_CLICK = 4,
    RESOURCE_EXPLOITATION = 5,
    PACKET_FLOOD = 6,
    ABNORMAL_PATTERN = 7,
    BOT_BEHAVIOR = 8,
    MEMORY_TAMPERING = 9,
    HARDWARE_SPOOFING = 10
};

/**
 * @brief Contexto para detecção de comportamento
 */
struct BehaviorContext {
    std::uint32_t accountId;
    std::string playerName;
    std::string characterClass;
    std::uint32_t level;
    TimePoint lastActivity;
    std::unordered_map<std::string, float> playerAttributes;
    std::unordered_map<std::string, std::vector<float>> historicalData;
    
    // Geolocalização e hardware
    std::string ipAddress;
    std::string countryCode;
    std::optional<std::string> hardwareId;
    
    // Estado do jogo
    std::string currentMap;
    std::int32_t positionX;
    std::int32_t positionY;
    std::uint32_t currentHP;
    std::uint32_t maxHP;
    std::uint32_t currentMP;
    std::uint32_t maxMP;
    std::uint32_t gold;
    
    BehaviorContext() : 
        accountId(0),
        level(1),
        lastActivity(std::chrono::system_clock::now()),
        positionX(0),
        positionY(0),
        currentHP(0),
        maxHP(0),
        currentMP(0),
        maxMP(0),
        gold(0) {}
    
    [[nodiscard]] bool isValid() const {
        return accountId > 0 && !playerName.empty();
    }
};

/**
 * @brief Resultado de detecção de anomalia
 */
struct AnomalyDetectionResult {
    AnomalyType type;
    float anomalyScore; // 0.0 a 1.0
    float confidence; // 0.0 a 1.0
    std::string description;
    TimePoint detectionTime;
    std::unordered_map<std::string, float> contributingFactors;
    
    AnomalyDetectionResult() : 
        type(AnomalyType::UNKNOWN),
        anomalyScore(0.0f),
        confidence(0.0f),
        detectionTime(std::chrono::system_clock::now()) {}
    
    [[nodiscard]] security::ThreatType toThreatType() const {
        switch (type) {
            case AnomalyType::SPEED_HACK: 
                return security::ThreatType::SPEED_HACK;
            case AnomalyType::TELEPORT_HACK: 
                return security::ThreatType::ABNORMAL_BEHAVIOR;
            case AnomalyType::COMBAT_HACK: 
                return security::ThreatType::ABNORMAL_BEHAVIOR;
            case AnomalyType::AUTO_CLICK: 
                return security::ThreatType::BOT_AUTOMATION;
            case AnomalyType::BOT_BEHAVIOR: 
                return security::ThreatType::BOT_AUTOMATION;
            case AnomalyType::PACKET_FLOOD: 
                return security::ThreatType::PACKET_INJECTION;
            case AnomalyType::MEMORY_TAMPERING: 
                return security::ThreatType::MEMORY_MANIPULATION;
            case AnomalyType::HARDWARE_SPOOFING: 
                return security::ThreatType::VIRTUAL_MACHINE;
            default: 
                return security::ThreatType::ABNORMAL_BEHAVIOR;
        }
    }
    
    [[nodiscard]] security::ThreatSeverity getThreatSeverity() const {
        if (confidence < 0.3f)
            return security::ThreatSeverity::LOW;
        else if (confidence < 0.7f)
            return security::ThreatSeverity::MEDIUM;
        else if (confidence < 0.9f)
            return security::ThreatSeverity::HIGH;
        else
            return security::ThreatSeverity::CRITICAL;
    }
    
    [[nodiscard]] security::ThreatReport toThreatReport() const {
        security::ThreatReport report;
        report.type = toThreatType();
        report.severity = getThreatSeverity();
        report.description = description;
        report.confirmed = confidence > 0.85f;
        report.confidenceScore = confidence;
        
        // Adicionar evidências
        for (const auto& factor : contributingFactors) {
            report.evidence[factor.first] = std::to_string(factor.second);
        }
        
        return report;
    }
};

/**
 * @brief Extrator de características para detecção de anomalias
 */
class FeatureExtractor {
public:
    FeatureExtractor();
    ~FeatureExtractor();
    
    // Extração de características
    [[nodiscard]] FeatureVector extractMovementFeatures(
        const BehaviorContext& context,
        const std::vector<std::pair<TimePoint, std::pair<int, int>>>& movements) const;
    
    [[nodiscard]] FeatureVector extractCombatFeatures(
        const BehaviorContext& context,
        const std::vector<std::pair<TimePoint, std::uint32_t>>& attacks,
        const std::vector<std::pair<TimePoint, std::uint32_t>>& damages) const;
    
    [[nodiscard]] FeatureVector extractResourceGatheringFeatures(
        const BehaviorContext& context,
        const std::unordered_map<std::string, std::uint32_t>& resourcesGathered,
        const TimePoint& startTime,
        const TimePoint& endTime) const;
    
    [[nodiscard]] FeatureVector extractPacketPatternFeatures(
        const BehaviorContext& context,
        const std::vector<std::pair<TimePoint, std::size_t>>& packetSizes,
        const std::unordered_map<std::string, std::uint32_t>& packetTypes) const;
    
    [[nodiscard]] FeatureVector extractClickPatternFeatures(
        const BehaviorContext& context,
        const std::vector<TimePoint>& clicks,
        const std::vector<std::pair<int, int>>& clickPositions) const;
    
    [[nodiscard]] FeatureVector extractHardwareFeatures(
        const BehaviorContext& context,
        const std::string& hardwareFingerprint,
        const std::unordered_map<std::string, std::string>& systemInfo) const;
    
    // Normalização de características
    [[nodiscard]] FeatureVector normalizeFeatures(
        const FeatureVector& features,
        const std::vector<FeatureDescriptor>& descriptors) const;
    
    // Seleção de características
    [[nodiscard]] std::vector<std::size_t> selectMostImportantFeatures(
        const std::vector<FeatureVector>& allFeatures,
        const std::vector<LabelType>& labels,
        std::size_t maxFeatures = 10) const;
    
    // Descritores de características
    [[nodiscard]] std::vector<FeatureDescriptor> getMovementFeatureDescriptors() const;
    [[nodiscard]] std::vector<FeatureDescriptor> getCombatFeatureDescriptors() const;
    [[nodiscard]] std::vector<FeatureDescriptor> getResourceFeatureDescriptors() const;
    [[nodiscard]] std::vector<FeatureDescriptor> getPacketFeatureDescriptors() const;
    [[nodiscard]] std::vector<FeatureDescriptor> getClickFeatureDescriptors() const;
    [[nodiscard]] std::vector<FeatureDescriptor> getHardwareFeatureDescriptors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Cálculos internos
    [[nodiscard]] float calculateTimingRegularity(const std::vector<TimePoint>& timestamps) const;
    [[nodiscard]] float calculateSpatialRegularity(const std::vector<std::pair<int, int>>& positions) const;
    [[nodiscard]] float calculateSpeedAnomaly(const std::vector<std::pair<TimePoint, std::pair<int, int>>>& movements) const;
    [[nodiscard]] std::vector<float> calculateTimeIntervals(const std::vector<TimePoint>& timestamps) const;
};

/**
 * @brief Detector de comportamento anômalo baseado em ML
 */
class MLAnomalyDetector {
public:
    // Singleton
    static MLAnomalyDetector& getInstance();
    
    // Inicialização
    bool initialize();
    bool shutdown();
    [[nodiscard]] bool isInitialized() const noexcept { return m_initialized; }
    
    // Carregar/salvar modelos
    bool loadModels(const std::string& directoryPath);
    bool saveModels(const std::string& directoryPath) const;
    
    // Treinamento de modelos
    bool trainSpeedHackModel(const std::vector<TrainingSample>& samples);
    bool trainTeleportModel(const std::vector<TrainingSample>& samples);
    bool trainCombatModel(const std::vector<TrainingSample>& samples);
    bool trainBotBehaviorModel(const std::vector<TrainingSample>& samples);
    bool trainPacketModel(const std::vector<TrainingSample>& samples);
    bool trainHardwareSpoofingModel(const std::vector<TrainingSample>& samples);
    
    // Treinamento incremental em tempo real
    bool updateModelWithLegitimateData(AnomalyType type, const FeatureVector& features);
    bool updateModelWithAnomalousData(AnomalyType type, const FeatureVector& features);
    
    // Detecção de anomalias
    [[nodiscard]] AnomalyDetectionResult detectSpeedHack(
        const BehaviorContext& context,
        const std::vector<std::pair<TimePoint, std::pair<int, int>>>& movements) const;
    
    [[nodiscard]] AnomalyDetectionResult detectTeleportHack(
        const BehaviorContext& context,
        const std::pair<int, int>& oldPosition,
        const std::pair<int, int>& newPosition,
        const TimePoint& oldTime,
        const TimePoint& newTime) const;
    
    [[nodiscard]] AnomalyDetectionResult detectCombatHack(
        const BehaviorContext& context,
        const std::vector<std::pair<TimePoint, std::uint32_t>>& attacks,
        const std::vector<std::pair<TimePoint, std::uint32_t>>& damages) const;
    
    [[nodiscard]] AnomalyDetectionResult detectBotBehavior(
        const BehaviorContext& context,
        const std::vector<TimePoint>& actions,
        const std::vector<std::pair<int, int>>& movements) const;
    
    [[nodiscard]] AnomalyDetectionResult detectPacketFlooding(
        const BehaviorContext& context,
        const std::vector<std::pair<TimePoint, std::size_t>>& packets) const;
    
    [[nodiscard]] AnomalyDetectionResult detectHardwareSpoofing(
        const BehaviorContext& context,
        const std::string& hardwareFingerprint,
        const std::unordered_map<std::string, std::string>& systemInfo) const;
    
    // Avaliação de modelos
    [[nodiscard]] ModelPerformanceMetrics evaluateModel(
        AnomalyType type, 
        const std::vector<TrainingSample>& testSamples) const;
    
    // Configuração de sensibilidade
    void setDetectionThreshold(AnomalyType type, float threshold);
    void setConfidenceThreshold(AnomalyType type, float threshold);
    void configureAutoThresholdAdjustment(bool enable, float targetFalsePositiveRate = 0.01f);
    
    // Estatísticas e diagnóstico
    [[nodiscard]] std::unordered_map<AnomalyType, ModelPerformanceMetrics> getAllModelMetrics() const;
    [[nodiscard]] std::size_t getTotalDetectionCount() const;
    [[nodiscard]] std::size_t getFalsePositiveCount() const;
    [[nodiscard]] std::unordered_map<AnomalyType, std::size_t> getDetectionCountByType() const;
    
    // Acessos diretos para análise
    [[nodiscard]] FeatureExtractor& getFeatureExtractor() { return *m_featureExtractor; }
    [[nodiscard]] const FeatureExtractor& getFeatureExtractor() const { return *m_featureExtractor; }
    
private:
    MLAnomalyDetector();
    ~MLAnomalyDetector();
    
    // Prevenção de cópia
    MLAnomalyDetector(const MLAnomalyDetector&) = delete;
    MLAnomalyDetector& operator=(const MLAnomalyDetector&) = delete;
    
    // Implementação interna
    [[nodiscard]] std::shared_ptr<IMLModel> getModel(AnomalyType type) const;
    void adjustThresholds();
    void loadTrainingData();
    
    // Estado
    bool m_initialized;
    
    // Componentes
    std::unique_ptr<FeatureExtractor> m_featureExtractor;
    std::unordered_map<AnomalyType, std::shared_ptr<IMLModel>> m_models;
    
    // Configuração
    std::unordered_map<AnomalyType, float> m_detectionThresholds;
    std::unordered_map<AnomalyType, float> m_confidenceThresholds;
    bool m_autoAdjustThresholds;
    float m_targetFalsePositiveRate;
    
    // Estatísticas
    std::unordered_map<AnomalyType, std::size_t> m_detectionCounts;
    std::unordered_map<AnomalyType, std::size_t> m_falsePositiveCounts;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
};

/**
 * @brief Definição para o modelo de ensemble
 */
class EnsembleModel : public IMLModel {
public:
    EnsembleModel(const std::vector<std::shared_ptr<IMLModel>>& baseModels,
                 const std::vector<float>& weights);
    ~EnsembleModel() override;
    
    bool initialize() override;
    void setFeatureDescriptors(const std::vector<FeatureDescriptor>& descriptors) override;
    
    bool train(const std::vector<TrainingSample>& trainingSamples, 
             const TrainingOptions& options) override;
    
    bool trainIncrementally(const TrainingSample& sample) override;
    
    PredictionResult predict(const FeatureVector& features) const override;
    
    std::vector<PredictionResult> batchPredict(
        const std::vector<FeatureVector>& featuresBatch) const override;
    
    ModelPerformanceMetrics evaluate(
        const std::vector<TrainingSample>& testSamples) const override;
    
    bool saveModel(const std::string& path) const override;
    bool loadModel(const std::string& path) override;
    
    [[nodiscard]] ModelIdentifier getModelIdentifier() const override;
    [[nodiscard]] std::string getModelDescription() const override;
    [[nodiscard]] std::vector<FeatureDescriptor> getFeatureDescriptors() const override;
    [[nodiscard]] std::size_t getFeatureDimension() const override;
    
    // Métodos específicos de ensemble
    void addBaseModel(std::shared_ptr<IMLModel> model, float weight);
    void removeBaseModel(const ModelIdentifier& modelId);
    void updateModelWeight(const ModelIdentifier& modelId, float weight);
    [[nodiscard]] std::vector<std::pair<ModelIdentifier, float>> getModelWeights() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Implementação interna
    PredictionResult combineBasePredictions(
        const std::vector<PredictionResult>& basePredictions) const;
    
    void normalizeWeights();
};

/**
 * @brief Fábrica de modelos de ML
 */
class MLModelFactory {
public:
    static std::shared_ptr<RandomForestModel> createRandomForest(
        std::size_t numTrees = 100,
        std::size_t maxDepth = 10,
        float featureSamplingRatio = 0.7f);
    
    static std::shared_ptr<NeuralNetworkModel> createNeuralNetwork(
        const std::vector<std::size_t>& layerSizes,
        const std::string& activationFunction = "relu",
        float dropoutRate = 0.2f);
    
    static std::shared_ptr<SupportVectorMachineModel> createSVM(
        const std::string& kernelType = "rbf",
        float c = 1.0f,
        float gamma = 0.1f);
    
    static std::shared_ptr<GradientBoostingModel> createGradientBoosting(
        std::size_t numEstimators = 100,
        float learningRate = 0.1f,
        std::size_t maxDepth = 3);
    
    static std::shared_ptr<EnsembleModel> createEnsemble(
        const std::vector<std::shared_ptr<IMLModel>>& baseModels,
        const std::vector<float>& weights);
    
    static std::shared_ptr<IMLModel> createOptimalModelForAnomalyType(AnomalyType type);
};

} // namespace ml
} // namespace security
} // namespace wydstudio

#endif // WYDSTUDIO_MACHINE_LEARNING_DETECTION_H