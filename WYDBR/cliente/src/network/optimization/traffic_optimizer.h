#ifndef WYDBR_TRAFFIC_OPTIMIZER_H
#define WYDBR_TRAFFIC_OPTIMIZER_H

#include <vector>
#include <map>
#include <deque>
#include <string>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>

namespace wydbr {
namespace network {

// Enumeração para modo de otimização de tráfego
enum class OptimizationMode {
    NONE,            // Sem otimização
    CONSERVATIVE,    // Otimização conservadora
    BALANCED,        // Equilíbrio entre dados e desempenho
    AGGRESSIVE,      // Otimização agressiva, sacrifica qualidade
    ULTRA            // Otimização máxima, sacrifica muito a qualidade
};

// Enumeração para tipo de otimização
enum class OptimizationType {
    DELTA_ENCODING,       // Codificação delta (apenas diferenças)
    PACKET_BATCHING,      // Agrupamento de pacotes
    PACKET_REORDERING,    // Reordenação por prioridade
    HEADER_COMPRESSION,   // Compressão de cabeçalhos
    FREQUENCY_CONTROL,    // Controle de frequência
    BANDWIDTH_ADAPTATION, // Adaptação de largura de banda
    PRIORITY_CONTROL,     // Controle de prioridade
    DATA_FILTERING,       // Filtragem de dados
    INTEREST_MANAGEMENT,  // Gerenciamento de interesse
    REDUNDANCY_CONTROL    // Controle de redundância
};

// Estrutura para configuração do otimizador
struct TrafficOptimizerConfig {
    OptimizationMode mode;
    bool enableDeltaEncoding;
    bool enablePacketBatching;
    bool enablePacketReordering;
    bool enableHeaderCompression;
    bool enableFrequencyControl;
    bool enableBandwidthAdaptation;
    bool enablePriorityControl;
    bool enableDataFiltering;
    bool enableInterestManagement;
    bool enableRedundancyControl;
    int batchSizeLimit;
    int maxBatchDelayMs;
    int priorityLevels;
    int updateFrequencyLimitMs;
    int bandwidthTargetKbps;
    int adaptationCheckIntervalMs;
    float qualityScaleFactor;
    
    // Construtor com valores padrão
    TrafficOptimizerConfig() :
        mode(OptimizationMode::BALANCED),
        enableDeltaEncoding(true),
        enablePacketBatching(true),
        enablePacketReordering(true),
        enableHeaderCompression(true),
        enableFrequencyControl(true),
        enableBandwidthAdaptation(true),
        enablePriorityControl(true),
        enableDataFiltering(true),
        enableInterestManagement(true),
        enableRedundancyControl(true),
        batchSizeLimit(1400),
        maxBatchDelayMs(50),
        priorityLevels(4),
        updateFrequencyLimitMs(100),
        bandwidthTargetKbps(0), // 0 = auto
        adaptationCheckIntervalMs(1000),
        qualityScaleFactor(1.0f)
    {}
};

// Estrutura para pacote a otimizar
struct PacketInfo {
    uint32_t id;
    uint8_t type;
    uint8_t priority;
    std::vector<uint8_t> data;
    uint64_t timestamp;
    bool reliable;
    bool important;
    std::string entity;
    float position[3];
    float radius;
    uint32_t channel;
};

// Estrutura para estatísticas de otimização
struct OptimizationStats {
    int64_t originalDataSize;
    int64_t optimizedDataSize;
    int packetsOptimized;
    int batchesCreated;
    int packetsDropped;
    int packetsReordered;
    float compressionRatio;
    float bandwidthSaved;
    int currentBandwidthUsage;
    float frequencyReductionRatio;
    int activePriorities;
    int entitiesFiltered;
    int redundancyAvoided;
    
    // Construtor
    OptimizationStats() :
        originalDataSize(0),
        optimizedDataSize(0),
        packetsOptimized(0),
        batchesCreated(0),
        packetsDropped(0),
        packetsReordered(0),
        compressionRatio(0.0f),
        bandwidthSaved(0.0f),
        currentBandwidthUsage(0),
        frequencyReductionRatio(0.0f),
        activePriorities(0),
        entitiesFiltered(0),
        redundancyAvoided(0)
    {}
};

// Estrutura para lote de pacotes
struct PacketBatch {
    std::vector<PacketInfo> packets;
    bool hasHighPriority;
    uint64_t creationTimestamp;
    uint32_t batchId;
    uint32_t channel;
    std::vector<uint8_t> optimizedData;
};

// Callbacks
using OptimizedPacketCallback = std::function<void(const std::vector<uint8_t>& data, bool isOptimized)>;
using BatchReadyCallback = std::function<void(const PacketBatch& batch)>;
using StatisticsCallback = std::function<void(const OptimizationStats& stats)>;

/**
 * @class TrafficOptimizer
 * @brief Otimizador de tráfego de rede
 * 
 * Sistema que otimiza o tráfego de rede usando técnicas como codificação delta,
 * agrupamento de pacotes, e adaptação de largura de banda.
 */
class TrafficOptimizer {
public:
    TrafficOptimizer();
    ~TrafficOptimizer();
    
    /**
     * @brief Inicializa o otimizador
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const TrafficOptimizerConfig& config = TrafficOptimizerConfig());
    
    /**
     * @brief Desliga o otimizador e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Adiciona um pacote para otimização
     * @param packet Informações do pacote
     * @return true se adicionado com sucesso
     */
    bool addPacket(const PacketInfo& packet);
    
    /**
     * @brief Processa pacotes em fila e cria lotes
     * @return Número de pacotes processados
     */
    int processQueue();
    
    /**
     * @brief Define o modo de otimização
     * @param mode Novo modo
     */
    void setOptimizationMode(OptimizationMode mode);
    
    /**
     * @brief Obtém o modo de otimização atual
     * @return Modo atual
     */
    OptimizationMode getOptimizationMode() const;
    
    /**
     * @brief Ativa ou desativa um tipo de otimização
     * @param type Tipo de otimização
     * @param enable Se deve ativar
     */
    void enableOptimization(OptimizationType type, bool enable);
    
    /**
     * @brief Verifica se um tipo de otimização está ativo
     * @param type Tipo de otimização
     * @return true se ativo
     */
    bool isOptimizationEnabled(OptimizationType type) const;
    
    /**
     * @brief Define o limite de tamanho de lote
     * @param sizeBytes Tamanho em bytes
     */
    void setBatchSizeLimit(int sizeBytes);
    
    /**
     * @brief Obtém o limite de tamanho de lote
     * @return Tamanho em bytes
     */
    int getBatchSizeLimit() const;
    
    /**
     * @brief Define o atraso máximo de lote
     * @param delayMs Atraso em milissegundos
     */
    void setMaxBatchDelay(int delayMs);
    
    /**
     * @brief Obtém o atraso máximo de lote
     * @return Atraso em milissegundos
     */
    int getMaxBatchDelay() const;
    
    /**
     * @brief Define a largura de banda alvo
     * @param kbps Taxa de bits em kbps (0 = auto)
     */
    void setBandwidthTarget(int kbps);
    
    /**
     * @brief Obtém a largura de banda alvo
     * @return Taxa de bits em kbps
     */
    int getBandwidthTarget() const;
    
    /**
     * @brief Define o fator de escala de qualidade
     * @param factor Fator de escala (0.1-2.0)
     */
    void setQualityScaleFactor(float factor);
    
    /**
     * @brief Obtém o fator de escala de qualidade
     * @return Fator de escala
     */
    float getQualityScaleFactor() const;
    
    /**
     * @brief Registra um callback para pacotes otimizados
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerPacketCallback(OptimizedPacketCallback callback);
    
    /**
     * @brief Remove um callback de pacotes otimizados
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterPacketCallback(int callbackId);
    
    /**
     * @brief Registra um callback para lotes prontos
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerBatchCallback(BatchReadyCallback callback);
    
    /**
     * @brief Remove um callback de lotes prontos
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterBatchCallback(int callbackId);
    
    /**
     * @brief Registra um callback para estatísticas
     * @param callback Função a ser chamada
     * @param intervalMs Intervalo de atualização em milissegundos
     * @return ID do callback (para remoção)
     */
    int registerStatsCallback(StatisticsCallback callback, int intervalMs = 1000);
    
    /**
     * @brief Remove um callback de estatísticas
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterStatsCallback(int callbackId);
    
    /**
     * @brief Obtém estatísticas de otimização
     * @return Estatísticas atuais
     */
    OptimizationStats getStatistics() const;
    
    /**
     * @brief Define uma região de interesse
     * @param centerX Coordenada X do centro
     * @param centerY Coordenada Y do centro
     * @param centerZ Coordenada Z do centro
     * @param radius Raio de interesse
     */
    void setInterestArea(float centerX, float centerY, float centerZ, float radius);
    
    /**
     * @brief Define a prioridade de uma entidade
     * @param entityId ID da entidade
     * @param priority Prioridade (0-255, maior = mais importante)
     */
    void setEntityPriority(const std::string& entityId, uint8_t priority);
    
    /**
     * @brief Define a frequência de atualização para uma entidade
     * @param entityId ID da entidade
     * @param updateFrequencyMs Frequência em milissegundos
     */
    void setEntityUpdateFrequency(const std::string& entityId, int updateFrequencyMs);
    
    /**
     * @brief Força o fluxo de todos os pacotes em fila agora
     * @param immediate Se deve forçar imediatamente, ignorando otimizações
     * @return Número de pacotes processados
     */
    int flushQueue(bool immediate = false);
    
private:
    // Métodos de otimização
    std::vector<uint8_t> applyDeltaEncoding(const PacketInfo& packet);
    std::vector<uint8_t> createBatchPacket(const std::vector<PacketInfo>& packets);
    std::vector<PacketInfo> reorderPacketsByPriority(const std::vector<PacketInfo>& packets);
    std::vector<uint8_t> compressHeader(const PacketInfo& packet);
    bool shouldFilterPacket(const PacketInfo& packet);
    bool shouldReduceFrequency(const PacketInfo& packet);
    bool isInInterestArea(const PacketInfo& packet) const;
    bool isRedundant(const PacketInfo& packet) const;
    int estimatePacketPriority(const PacketInfo& packet) const;
    
    // Métodos de adaptação
    void adaptBandwidthUsage();
    void adjustUpdateFrequencies();
    void updateDataFilters();
    
    // Métodos de notificação
    void notifyPacketOptimized(const std::vector<uint8_t>& data, bool isOptimized);
    void notifyBatchReady(const PacketBatch& batch);
    void notifyStatistics();
    
    // Thread de processamento
    void processingThread();
    void statsThread();
    
    // Estado interno
    bool m_initialized;
    TrafficOptimizerConfig m_config;
    OptimizationStats m_stats;
    
    // Filas e lotes
    std::deque<PacketInfo> m_packetQueue;
    std::vector<PacketBatch> m_batchQueue;
    
    // Estado de interesse
    struct InterestArea {
        float centerX, centerY, centerZ;
        float radius;
    } m_interestArea;
    
    // Estado de entidades
    struct EntityState {
        uint8_t priority;
        int updateFrequencyMs;
        uint64_t lastUpdateTime;
        std::vector<uint8_t> lastData;
        float position[3];
    };
    std::map<std::string, EntityState> m_entityStates;
    
    // Threads
    std::unique_ptr<std::thread> m_processingThread;
    std::unique_ptr<std::thread> m_statsThread;
    std::atomic<bool> m_threadsShouldRun;
    
    // Callbacks
    struct StatsCallbackInfo {
        StatisticsCallback callback;
        int intervalMs;
        uint64_t lastCallTime;
    };
    
    std::vector<std::pair<int, OptimizedPacketCallback>> m_packetCallbacks;
    std::vector<std::pair<int, BatchReadyCallback>> m_batchCallbacks;
    std::vector<std::pair<int, StatsCallbackInfo>> m_statsCallbacks;
    
    int m_nextCallbackId;
    
    // Thread safety
    mutable std::mutex m_queueMutex;
    mutable std::mutex m_batchMutex;
    mutable std::mutex m_statsMutex;
    mutable std::mutex m_callbackMutex;
    mutable std::mutex m_entityMutex;
};

} // namespace network
} // namespace wydbr

#endif // WYDBR_TRAFFIC_OPTIMIZER_H
