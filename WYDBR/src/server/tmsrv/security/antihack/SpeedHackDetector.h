/**
 * SpeedHackDetector.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/security/antihack/SpeedHackDetector.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef SPEEDHACKDETECTOR_H
#define SPEEDHACKDETECTOR_H

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
#include <algorithm>
#include <numeric>
#include <cmath>
#include <deque>

#include "../../../../core/WYDTypes.h"
#include "../SecurityTypes.h"
#include "../../player/CPlayer.h"

namespace wydbr {


/**
 * @file SpeedHackDetector.h
 * @brief Sistema avançado de detecção de speedhack
 * 
 * Este arquivo contém a definição do sistema de detecção de speedhack,
 * que utiliza múltiplas técnicas para identificar e prevenir a manipulação
 * de tempo por parte dos clientes, um dos hacks mais comuns no WYD original.
 */

namespace wyd {
namespace server {
namespace security {
namespace antihack {

/**
 * @brief Configuração do detector de speedhack
 */
struct SpeedHackDetectorConfig {
    bool enabled;                         // Habilitado
    bool autoRespond;                     // Responder automaticamente
    bool logViolations;                   // Registrar violações
    bool useMultipleAlgorithms;           // Usar múltiplos algoritmos
    bool usePredictiveAnalysis;           // Usar análise preditiva
    bool adaptiveThresholds;              // Limites adaptativos
    bool learningMode;                    // Modo de aprendizado
    DWORD timeWindowMs;                  // Janela de tempo (ms)
    DWORD maxDeviation;                  // Desvio máximo (ms)
    DWORD checkInterval;                 // Intervalo de verificação (ms)
    DWORD heartbeatInterval;             // Intervalo de heartbeat (ms)
    DWORD maxViolationsBeforeAction;     // Máximo de violações antes de ação
    DWORD samplesBeforeTraining;         // Amostras antes de treinamento
    float anomalyThreshold;               // Limite de anomalia
    SecurityResponseType violationResponse; // Resposta a violação
    
    SpeedHackDetectorConfig()
        : enabled(true)
        , autoRespond(true)
        , logViolations(true)
        , useMultipleAlgorithms(true)
        , usePredictiveAnalysis(true)
        , adaptiveThresholds(true)
        , learningMode(true)
        , timeWindowMs(10000)
        , maxDeviation(1000)
        , checkInterval(2000)
        , heartbeatInterval(5000)
        , maxViolationsBeforeAction(3)
        , samplesBeforeTraining(1000)
        , anomalyThreshold(0.85f)
        , violationResponse(SecurityResponseType::DISCONNECT)
    {
    }
};

/**
 * @brief Tipo de análise de speedhack
 */
enum class SpeedHackAnalysisType : BYTE {
    DELTA_TIME = 0,           // Delta de tempo
    ACTION_FREQUENCY = 1,     // Frequência de ação
    MOVEMENT_SPEED = 2,       // Velocidade de movimento
    SERVER_CLIENT_DELTA = 3,  // Delta servidor-cliente
    PACKET_TIMING = 4,        // Tempo de pacote
    EVENT_CORRELATION = 5,    // Correlação de evento
    STATISTICAL = 6,          // Estatístico
    PREDICTIVE = 7,           // Preditivo
    COMBINED = 8,             // Combinado
    CUSTOM = 9,               // Personalizado
};

/**
 * @brief Registro de tempo
 */
struct TimeRecord {
    DWORD clientTime;         // Tempo do cliente
    DWORD serverTime;         // Tempo do servidor
    DWORD delta;              // Delta
    DWORD packetId;           // ID do pacote
    DWORD playerId;           // ID do jogador
    time_t timestamp;          // Timestamp
};

/**
 * @brief Resultado de análise de speedhack
 */
struct SpeedHackAnalysisResult {
    bool detected;             // Detectado
    float confidence;          // Confiança
    SpeedHackAnalysisType type; // Tipo
    DWORD expectedValue;      // Valor esperado
    DWORD actualValue;        // Valor atual
    DWORD deviation;          // Desvio
    std::string details;       // Detalhes
    
    SpeedHackAnalysisResult()
        : detected(false)
        , confidence(0.0f)
        , type(SpeedHackAnalysisType::DELTA_TIME)
        , expectedValue(0)
        , actualValue(0)
        , deviation(0)
        , details("")
    {
    }
};

/**
 * @brief Perfil de jogador para detecção de speedhack
 */
struct PlayerTimingProfile {
    DWORD playerID;               // ID do jogador
    std::vector<TimeRecord> timeRecords; // Registros de tempo
    std::deque<DWORD> packetIntervals; // Intervalos de pacote
    std::deque<DWORD> movementIntervals; // Intervalos de movimento
    DWORD lastClientTime;         // Último tempo do cliente
    DWORD lastServerTime;         // Último tempo do servidor
    DWORD lastMovementTime;       // Último tempo de movimento
    DWORD lastActionTime;         // Último tempo de ação
    DWORD violations;             // Violações
    DWORD totalChecks;            // Total de verificações
    bool profileLearned;           // Perfil aprendido
    float baselineDeviation;       // Desvio de linha de base
    float latencyAverage;          // Média de latência
    float latencyStdDev;           // Desvio padrão de latência
    bool flagged;                  // Marcado
    DWORD lastChallengeTime;      // Último tempo de desafio
    DWORD lastHeartbeatTime;      // Último tempo de heartbeat
    
    PlayerTimingProfile()
        : playerID(0)
        , lastClientTime(0)
        , lastServerTime(0)
        , lastMovementTime(0)
        , lastActionTime(0)
        , violations(0)
        , totalChecks(0)
        , profileLearned(false)
        , baselineDeviation(0.0f)
        , latencyAverage(0.0f)
        , latencyStdDev(0.0f)
        , flagged(false)
        , lastChallengeTime(0)
        , lastHeartbeatTime(0)
    {
    }
};

/**
 * @brief Verificação de tempo
 */
struct TimeCheck {
    DWORD checkID;                // ID da verificação
    DWORD playerID;              // ID do jogador
    DWORD sentServerTime;        // Tempo do servidor enviado
    DWORD expectedResponseTime;  // Tempo de resposta esperado
    DWORD timeout;               // Timeout
    time_t sentTime;              // Hora de envio
    bool responded;               // Respondido
    DWORD clientResponse;        // Resposta do cliente
    
    TimeCheck()
        : checkID(0)
        , playerID(0)
        , sentServerTime(0)
        , expectedResponseTime(0)
        , timeout(0)
        , sentTime(0)
        , responded(false)
        , clientResponse(0)
    {
    }
};

/**
 * @brief Desafio de tempo
 */
struct TimeChallenge {
    DWORD challengeID;            // ID do desafio
    DWORD playerID;              // ID do jogador
    DWORD token;                 // Token
    DWORD timeSent;              // Tempo enviado
    DWORD expectedResponse;      // Resposta esperada
    time_t sentTime;              // Hora de envio
    bool completed;               // Completado
    bool passed;                  // Aprovado
    
    TimeChallenge()
        : challengeID(0)
        , playerID(0)
        , token(0)
        , timeSent(0)
        , expectedResponse(0)
        , sentTime(0)
        , completed(false)
        , passed(false)
    {
    }
};

/**
 * @brief Módulo de detecção de speedhack
 */
/**
 * Classe WYDSpeedHackDetector
 * 
 * Esta classe implementa a funcionalidade WYDSpeedHackDetector conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SpeedHackDetector {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static SpeedHackDetector& GetInstance();
    
    /**
     * @brief Inicializa o detector
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const SpeedHackDetectorConfig& config);
    
    /**
     * @brief Finaliza o detector
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o detector
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Registra um tempo de cliente
     * @param playerID ID do jogador
     * @param clientTime Tempo do cliente
     * @param packetID ID do pacote
     * @return true se registrado com sucesso
     */
    bool RegisterClientTime(DWORD playerID, DWORD clientTime, DWORD packetID);
    
    /**
     * @brief Registra uma ação de jogador
     * @param playerID ID do jogador
     * @param actionType Tipo de ação
     * @param timestamp Timestamp
     * @return true se registrado com sucesso
     */
    bool RegisterPlayerAction(DWORD playerID, BYTE actionType, DWORD timestamp);
    
    /**
     * @brief Registra um movimento de jogador
     * @param playerID ID do jogador
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param timestamp Timestamp
     * @return true se registrado com sucesso
     */
    bool RegisterPlayerMovement(DWORD playerID, WORD x, WORD y, DWORD timestamp);
    
    /**
     * @brief Verifica um jogador
     * @param playerID ID do jogador
     * @return Resultado da análise
     */
    SpeedHackAnalysisResult CheckPlayer(DWORD playerID);
    
    /**
     * @brief Envia um desafio de tempo
     * @param playerID ID do jogador
     * @return ID do desafio, ou 0 se falhar
     */
    DWORD SendTimeChallenge(DWORD playerID);
    
    /**
     * @brief Processa uma resposta de desafio
     * @param playerID ID do jogador
     * @param challengeID ID do desafio
     * @param response Resposta
     * @return true se processado com sucesso
     */
    bool ProcessChallengeResponse(DWORD playerID, DWORD challengeID, DWORD response);
    
    /**
     * @brief Adiciona um jogador
     * @param playerID ID do jogador
     * @return true se adicionado com sucesso
     */
    bool AddPlayer(DWORD playerID);
    
    /**
     * @brief Remove um jogador
     * @param playerID ID do jogador
     * @return true se removido com sucesso
     */
    bool RemovePlayer(DWORD playerID);
    
    /**
     * @brief Obtém o perfil de tempo de um jogador
     * @param playerID ID do jogador
     * @return Ponteiro para o perfil, ou nullptr se não existir
     */
    const PlayerTimingProfile* GetPlayerProfile(DWORD playerID) const;
    
    /**
     * @brief Obtém a média de latência de um jogador
     * @param playerID ID do jogador
     * @return Média de latência
     */
    float GetPlayerLatencyAverage(DWORD playerID) const;
    
    /**
     * @brief Envia um heartbeat
     * @param playerID ID do jogador
     * @return true se enviado com sucesso
     */
    bool SendHeartbeat(DWORD playerID);
    
    /**
     * @brief Processa uma resposta de heartbeat
     * @param playerID ID do jogador
     * @param clientTime Tempo do cliente
     * @param serverTimeEcho Eco do tempo do servidor
     * @return true se processado com sucesso
     */
    bool ProcessHeartbeatResponse(DWORD playerID, DWORD clientTime, DWORD serverTimeEcho);
    
    /**
     * @brief Define o tempo limite para jogadores sem resposta
     * @param timeoutMs Timeout (ms)
     */
    void SetNoResponseTimeout(DWORD timeoutMs);
    
    /**
     * @brief Obtém o tempo limite para jogadores sem resposta
     * @return Timeout (ms)
     */
    DWORD GetNoResponseTimeout() const;
    
    /**
     * @brief Obtém o número de violações de um jogador
     * @param playerID ID do jogador
     * @return Número de violações
     */
    DWORD GetPlayerViolationCount(DWORD playerID) const;
    
    /**
     * @brief Reseta o contador de violações de um jogador
     * @param playerID ID do jogador
     * @return true se resetado com sucesso
     */
    bool ResetPlayerViolations(DWORD playerID);
    
    /**
     * @brief Marca um jogador como suspeito
     * @param playerID ID do jogador
     * @param reason Razão
     * @return true se marcado com sucesso
     */
    bool FlagPlayerAsSuspect(DWORD playerID, const std::string& reason);
    
    /**
     * @brief Verifica se um jogador está marcado como suspeito
     * @param playerID ID do jogador
     * @return true se estiver marcado
     */
    bool IsPlayerFlagged(DWORD playerID) const;
    
    /**
     * @brief Define o tempo limite para desafios
     * @param timeoutMs Timeout (ms)
     */
    void SetChallengeTimeout(DWORD timeoutMs);
    
    /**
     * @brief Obtém o tempo limite para desafios
     * @return Timeout (ms)
     */
    DWORD GetChallengeTimeout() const;
    
    /**
     * @brief Registra um callback para detecção
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterDetectionCallback(std::function<void(DWORD, const SpeedHackAnalysisResult&)> callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterDetectionCallback(int callbackID);
    
    /**
     * @brief Aplica uma ação a um jogador
     * @param playerID ID do jogador
     * @param response Resposta
     * @param reason Razão
     * @return true se aplicado com sucesso
     */
    bool ApplyActionToPlayer(DWORD playerID, SecurityResponseType response, const std::string& reason);
    
    /**
     * @brief Gera um relatório de detecção
     * @param playerID ID do jogador
     * @return Relatório formatado
     */
    std::string GenerateDetectionReport(DWORD playerID);
    
    /**
     * @brief Obtém estatísticas de detecção
     * @return String formatada com estatísticas
     */
    std::string GetDetectionStatistics();
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    SpeedHackDetector();
    
    /**
     * @brief Destrutor
     */
    ~SpeedHackDetector();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    SpeedHackDetector(const SpeedHackDetector&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    SpeedHackDetector& operator=(const SpeedHackDetector&) = delete;
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Análise delta de tempo
     * @param playerID ID do jogador
     * @return Resultado
     */
    SpeedHackAnalysisResult AnalyzeTimeDelta(DWORD playerID);
    
    /**
     * @brief Análise frequência de ação
     * @param playerID ID do jogador
     * @return Resultado
     */
    SpeedHackAnalysisResult AnalyzeActionFrequency(DWORD playerID);
    
    /**
     * @brief Análise velocidade de movimento
     * @param playerID ID do jogador
     * @return Resultado
     */
    SpeedHackAnalysisResult AnalyzeMovementSpeed(DWORD playerID);
    
    /**
     * @brief Análise delta servidor-cliente
     * @param playerID ID do jogador
     * @return Resultado
     */
    SpeedHackAnalysisResult AnalyzeServerClientDelta(DWORD playerID);
    
    /**
     * @brief Análise estatística
     * @param playerID ID do jogador
     * @return Resultado
     */
    SpeedHackAnalysisResult AnalyzeStatistical(DWORD playerID);
    
    /**
     * @brief Análise preditiva
     * @param playerID ID do jogador
     * @return Resultado
     */
    SpeedHackAnalysisResult AnalyzePredictive(DWORD playerID);
    
    /**
     * @brief Análise combinada
     * @param playerID ID do jogador
     * @return Resultado
     */
    SpeedHackAnalysisResult AnalyzeCombined(DWORD playerID);
    
    /**
     * @brief Atualiza o perfil de tempo de um jogador
     * @param playerID ID do jogador
     */
    void UpdatePlayerProfile(DWORD playerID);
    
    /**
     * @brief Processa verificações de tempo expiradas
     */
    void ProcessExpiredTimeChecks();
    
    /**
     * @brief Processa desafios de tempo expirados
     */
    void ProcessExpiredChallenges();
    
    /**
     * @brief Processa heartbeats
     */
    void ProcessHeartbeats();
    
    /**
     * @brief Notifica callbacks de detecção
     * @param playerID ID do jogador
     * @param result Resultado
     */
    void NotifyDetectionCallbacks(DWORD playerID, const SpeedHackAnalysisResult& result);
    
    /**
     * @brief Calcula a média de diferenças de tempo
     * @param differences Diferenças
     * @return Média
     */
    float CalculateTimeDifferenceAverage(const std::vector<DWORD>& differences);
    
    /**
     * @brief Calcula o desvio padrão de diferenças de tempo
     * @param differences Diferenças
     * @param mean Média
     * @return Desvio padrão
     */
    float CalculateTimeDifferenceStdDev(const std::vector<DWORD>& differences, float mean);
    
    /**
     * @brief Detecta anomalias em diferenças de tempo
     * @param differences Diferenças
     * @param mean Média
     * @param stdDev Desvio padrão
     * @param threshold Limite
     * @return Anomalias
     */
    std::vector<DWORD> DetectTimeDifferenceAnomalies(const std::vector<DWORD>& differences,
                                                  float mean, float stdDev, float threshold);
    
    /**
     * @brief Gera um token para um desafio
     * @return Token
     */
    DWORD GenerateChallengeToken();
    
    /**
     * @brief Calcula a resposta esperada para um desafio
     * @param token Token
     * @param timeSent Tempo enviado
     * @return Resposta esperada
     */
    DWORD CalculateExpectedChallengeResponse(DWORD token, DWORD timeSent);
    
    // Configuração
    SpeedHackDetectorConfig config_;
    
    // Perfis de jogador
    std::unordered_map<DWORD, PlayerTimingProfile> playerProfiles_;
    mutable std::mutex profilesMutex_;
    
    // Verificações de tempo
    std::map<DWORD, TimeCheck> timeChecks_;
    mutable std::mutex timeChecksMutex_;
    
    // Desafios de tempo
    std::map<DWORD, TimeChallenge> timeChallenges_;
    mutable std::mutex challengesMutex_;
    
    // Jogadores marcados
    std::unordered_set<DWORD> flaggedPlayers_;
    std::unordered_map<DWORD, std::string> flagReasons_;
    mutable std::mutex flaggedPlayersMutex_;
    
    // Callbacks para detecção
    std::map<int, std::function<void(DWORD, const SpeedHackAnalysisResult&)>> detectionCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contadores para novos IDs
    std::atomic<DWORD> nextCheckID_;
    std::atomic<DWORD> nextChallengeID_;
    
    // Threads e sincronização
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Estatísticas
    DWORD totalChecks_;
    DWORD totalDetections_;
    DWORD falsePositives_;
    std::unordered_map<SpeedHackAnalysisType, DWORD> detectionsByType_;
    mutable std::mutex statisticsMutex_;
    
    // Tempos
    time_t lastCheckTime_;
    time_t lastHeartbeatTime_;
    
    // Limites
    DWORD noResponseTimeout_;
    DWORD challengeTimeout_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_SpeedHackDetector SpeedHackDetector::GetInstance()

} // namespace antihack
} // namespace security
} // namespace server
} // namespace wyd

#endif // SPEEDHACKDETECTOR_H

} // namespace wydbr
