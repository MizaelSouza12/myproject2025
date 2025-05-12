#ifndef WYDBR_RECONNECTION_SYSTEM_H
#define WYDBR_RECONNECTION_SYSTEM_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

namespace wydbr {
namespace network {

// Enumeração para estado de reconexão
enum class ReconnectionState {
    INACTIVE,
    WAITING,
    ATTEMPTING,
    SUCCEEDED,
    FAILED
};

// Estrutura para configuração de reconexão
struct ReconnectionConfig {
    bool enabled;
    int maxAttempts;
    int initialDelayMs;
    float backoffMultiplier;
    int maxDelayMs;
    bool useRandomization;
    float randomizationFactor;
    bool keepAlive;
    int keepAliveIntervalMs;
    int pingTimeoutMs;
    bool intelligentReconnect;
    
    // Construtor com valores padrão
    ReconnectionConfig() :
        enabled(true),
        maxAttempts(10),
        initialDelayMs(500),
        backoffMultiplier(1.5f),
        maxDelayMs(30000),
        useRandomization(true),
        randomizationFactor(0.2f),
        keepAlive(true),
        keepAliveIntervalMs(30000),
        pingTimeoutMs(5000),
        intelligentReconnect(true)
    {}
};

// Estrutura para sessão persistente
struct PersistentSession {
    std::string sessionId;
    std::string authToken;
    std::string serverAddress;
    int serverPort;
    std::vector<uint8_t> sessionData;
    std::vector<uint8_t> clientState;
    int64_t creationTime;
    int64_t lastUpdateTime;
    bool isValid;
};

// Estrutura para contexto de tentativa de reconexão
struct ReconnectionContext {
    int attempt;
    int maxAttempts;
    int delayMs;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point nextAttemptTime;
    ReconnectionState state;
};

// Callbacks
using ReconnectionAttemptCallback = std::function<void(int attempt, int maxAttempts, int delayMs)>;
using ReconnectionResultCallback = std::function<void(bool success, int attempts, const std::string& message)>;
using ConnectionQualityCallback = std::function<void(float quality, bool unstable)>;

/**
 * @class ReconnectionSystem
 * @brief Sistema de reconexão automática
 * 
 * Gerencia a persistência de sessão, detecção de problemas de conexão,
 * e reconexão automática ao servidor com backoff exponencial.
 */
class ReconnectionSystem {
public:
    ReconnectionSystem();
    ~ReconnectionSystem();
    
    /**
     * @brief Inicializa o sistema de reconexão
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const ReconnectionConfig& config = ReconnectionConfig());
    
    /**
     * @brief Desliga o sistema e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Configura uma nova sessão
     * @param sessionId ID da sessão
     * @param authToken Token de autenticação
     * @param serverAddress Endereço do servidor
     * @param serverPort Porta do servidor
     * @return true se a sessão foi configurada com sucesso
     */
    bool setupSession(const std::string& sessionId, 
                     const std::string& authToken,
                     const std::string& serverAddress,
                     int serverPort);
    
    /**
     * @brief Atualiza o estado da sessão
     * @param sessionData Dados da sessão
     * @param clientState Estado do cliente
     * @return true se atualizado com sucesso
     */
    bool updateSessionState(const std::vector<uint8_t>& sessionData,
                           const std::vector<uint8_t>& clientState);
    
    /**
     * @brief Inicia o monitoramento de conexão
     * @return true se iniciado com sucesso
     */
    bool startMonitoring();
    
    /**
     * @brief Para o monitoramento de conexão
     */
    void stopMonitoring();
    
    /**
     * @brief Verifica se o monitoramento está ativo
     * @return true se ativo
     */
    bool isMonitoring() const;
    
    /**
     * @brief Inicia manualmente o processo de reconexão
     * @return true se o processo foi iniciado com sucesso
     */
    bool startReconnection();
    
    /**
     * @brief Cancela o processo de reconexão atual
     */
    void cancelReconnection();
    
    /**
     * @brief Verifica se a reconexão está em andamento
     * @return true se em andamento
     */
    bool isReconnecting() const;
    
    /**
     * @brief Registra um callback para eventos de tentativa de reconexão
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerAttemptCallback(ReconnectionAttemptCallback callback);
    
    /**
     * @brief Remove um callback de eventos de tentativa de reconexão
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterAttemptCallback(int callbackId);
    
    /**
     * @brief Registra um callback para eventos de resultado de reconexão
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerResultCallback(ReconnectionResultCallback callback);
    
    /**
     * @brief Remove um callback de eventos de resultado de reconexão
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterResultCallback(int callbackId);
    
    /**
     * @brief Registra um callback para eventos de qualidade de conexão
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerQualityCallback(ConnectionQualityCallback callback);
    
    /**
     * @brief Remove um callback de eventos de qualidade de conexão
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterQualityCallback(int callbackId);
    
    /**
     * @brief Ativa ou desativa o sistema
     * @param enabled Se deve estar ativado
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Verifica se o sistema está ativado
     * @return true se ativado
     */
    bool isEnabled() const;
    
    /**
     * @brief Define o número máximo de tentativas
     * @param maxAttempts Número máximo
     */
    void setMaxAttempts(int maxAttempts);
    
    /**
     * @brief Obtém o número máximo de tentativas
     * @return Número máximo
     */
    int getMaxAttempts() const;
    
    /**
     * @brief Define o atraso inicial
     * @param initialDelayMs Atraso em milissegundos
     */
    void setInitialDelay(int initialDelayMs);
    
    /**
     * @brief Obtém o atraso inicial
     * @return Atraso em milissegundos
     */
    int getInitialDelay() const;
    
    /**
     * @brief Limpa a sessão atual
     */
    void clearSession();
    
    /**
     * @brief Obtém o estado atual da reconexão
     * @return Estado da reconexão
     */
    ReconnectionState getState() const;
    
    /**
     * @brief Obtém informações sobre a tentativa atual
     * @param outAttempt Tentativa atual (saída)
     * @param outMaxAttempts Número máximo de tentativas (saída)
     * @param outTimeUntilNextMs Tempo até a próxima tentativa em ms (saída)
     * @return true se uma reconexão está em andamento
     */
    bool getCurrentAttemptInfo(int& outAttempt, int& outMaxAttempts, int& outTimeUntilNextMs) const;
    
    /**
     * @brief Obtém a sessão atual
     * @return Sessão atual
     */
    PersistentSession getSession() const;
    
    /**
     * @brief Verifica se existe uma sessão válida
     * @return true se existe
     */
    bool hasValidSession() const;
    
    /**
     * @brief Reporta uma falha de conexão
     * @param errorCode Código de erro
     * @param errorMessage Mensagem de erro
     * @return true se o relatório foi processado
     */
    bool reportConnectionFailure(int errorCode, const std::string& errorMessage);
    
    /**
     * @brief Reporta uma reconexão bem-sucedida
     * @return true se o relatório foi processado
     */
    bool reportReconnectSuccess();
    
    /**
     * @brief Salva o estado da sessão atual em um arquivo
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool saveSessionToFile(const std::string& filePath) const;
    
    /**
     * @brief Carrega o estado da sessão de um arquivo
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool loadSessionFromFile(const std::string& filePath);

private:
    // Métodos internos
    void monitoringThread();
    void reconnectionThread();
    
    int calculateNextDelay(int attempt);
    void notifyAttempt(int attempt, int maxAttempts, int delayMs);
    void notifyResult(bool success, int attempts, const std::string& message);
    void notifyQuality(float quality, bool unstable);
    
    bool detectNetworkCondition();
    bool isConnectionStable() const;
    float estimateConnectionQuality() const;
    void updateNetworkMetrics(bool connected, float latency);
    
    // Dados de estado
    bool m_initialized;
    ReconnectionConfig m_config;
    std::atomic<bool> m_enabled;
    std::atomic<bool> m_isMonitoring;
    std::atomic<bool> m_isReconnecting;
    ReconnectionState m_state;
    PersistentSession m_session;
    ReconnectionContext m_reconnContext;
    
    // Métricas de rede
    struct NetworkMetrics {
        std::vector<float> latencyHistory;
        std::vector<int64_t> disconnectTimes;
        float avgLatency;
        float jitter;
        float packetLoss;
        int failureCount;
        int successCount;
        bool hasInternetConnection;
        bool hasServerConnection;
        int consecutiveFailures;
        
        NetworkMetrics() :
            avgLatency(0.0f),
            jitter(0.0f),
            packetLoss(0.0f),
            failureCount(0),
            successCount(0),
            hasInternetConnection(true),
            hasServerConnection(true),
            consecutiveFailures(0)
        {}
    } m_metrics;
    
    // Threads
    std::unique_ptr<std::thread> m_monitoringThread;
    std::unique_ptr<std::thread> m_reconnectionThread;
    std::atomic<bool> m_monitoringThreadShouldRun;
    std::atomic<bool> m_reconnectionThreadShouldRun;
    
    // Callbacks
    std::vector<std::pair<int, ReconnectionAttemptCallback>> m_attemptCallbacks;
    std::vector<std::pair<int, ReconnectionResultCallback>> m_resultCallbacks;
    std::vector<std::pair<int, ConnectionQualityCallback>> m_qualityCallbacks;
    
    int m_nextCallbackId;
    
    // Mutex
    mutable std::mutex m_stateMutex;
    mutable std::mutex m_metricsMutex;
    mutable std::mutex m_callbackMutex;
    mutable std::mutex m_sessionMutex;
};

} // namespace network
} // namespace wydbr

#endif // WYDBR_RECONNECTION_SYSTEM_H
