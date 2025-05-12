#ifndef WYDSTUDIO_NETWORK_MONITOR_H
#define WYDSTUDIO_NETWORK_MONITOR_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <optional>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <map>
#include <unordered_map>
#include <pcap.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "ProtocolAnalyzer.h"
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace network {

/**
 * @brief Tipo de filtro de captura
 */
enum class CaptureFilterType {
    ALL_TRAFFIC,
    GAME_TRAFFIC_ONLY,
    CUSTOM
};

/**
 * @brief Configuração do monitor de rede
 */
struct NetworkMonitorConfig {
    std::string interfaceName;
    CaptureFilterType filterType;
    std::string customFilter;
    bool promiscuousMode;
    uint32_t bufferSize;
    int snapLength;
    int readTimeout;
    bool captureLocalTraffic;
    std::vector<uint16_t> gamePortsToMonitor;
    std::string gameBinaryPath;
    
    NetworkMonitorConfig() : 
        filterType(CaptureFilterType::GAME_TRAFFIC_ONLY),
        promiscuousMode(true),
        bufferSize(1024 * 1024 * 16), // 16MB
        snapLength(65535),
        readTimeout(1000),
        captureLocalTraffic(true) {
        // Portas padrão do WYD
        gamePortsToMonitor = {8281, 8282, 8283, 8284, 8285, 8286, 8287, 8288, 8289, 8290};
    }
};

/**
 * @brief Informações sobre a captura de rede
 */
struct CaptureInfo {
    std::string deviceName;
    std::string filterExpression;
    std::chrono::system_clock::time_point startTime;
    uint64_t packetsTotal;
    uint64_t packetsGame;
    uint64_t bytesTotal;
    uint64_t bytesGame;
    uint32_t droppedPackets;
    
    CaptureInfo() : 
        packetsTotal(0),
        packetsGame(0),
        bytesTotal(0),
        bytesGame(0),
        droppedPackets(0) {}
};

/**
 * @brief Estatísticas da conexão monitorada
 */
struct ConnectionStats {
    std::string sourceAddress;
    uint16_t sourcePort;
    std::string destAddress;
    uint16_t destPort;
    uint64_t packetsIn;
    uint64_t packetsOut;
    uint64_t bytesIn;
    uint64_t bytesOut;
    std::chrono::system_clock::time_point firstSeen;
    std::chrono::system_clock::time_point lastSeen;
    std::optional<PacketDirection> direction;
    bool isGameTraffic;
    
    ConnectionStats() : 
        sourcePort(0),
        destPort(0),
        packetsIn(0),
        packetsOut(0),
        bytesIn(0),
        bytesOut(0),
        isGameTraffic(false) {}
};

/**
 * @brief Informações sobre uma sessão de jogo
 */
struct GameSession {
    std::string sessionId;
    std::string clientAddress;
    uint16_t clientPort;
    std::string serverAddress;
    uint16_t serverPort;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastActivity;
    std::string accountName;
    std::string characterName;
    uint32_t packetCount;
    
    // Informações derivadas da análise de pacotes
    struct {
        int32_t characterLevel;
        int32_t posX;
        int32_t posY;
        std::string currentMap;
        std::string characterClass;
        int32_t guildId;
        std::string partyMembers;
        std::string lastAction;
    } gameState;
    
    GameSession() : 
        clientPort(0),
        serverPort(0),
        packetCount(0) {
        gameState.characterLevel = 0;
        gameState.posX = 0;
        gameState.posY = 0;
        gameState.guildId = -1;
    }
};

/**
 * @brief Tipo de anomalia de rede detectada
 */
enum class NetworkAnomalyType {
    EXCESSIVE_PACKETS,
    SUSPICIOUS_TIMING,
    MODIFIED_PACKET_STRUCTURE,
    INVALID_GAME_STATE,
    UNEXPECTED_PACKET_SEQUENCE,
    DATA_MANIPULATION,
    PACKET_REPLAY,
    UNAUTHORIZED_TOOL,
    CUSTOM_DETECTION
};

/**
 * @brief Informações sobre uma anomalia detectada
 */
struct NetworkAnomaly {
    NetworkAnomalyType type;
    std::string sessionId;
    std::string description;
    float confidenceScore;
    std::chrono::system_clock::time_point detectionTime;
    std::vector<uint8_t> evidenceData;
    std::string sourceAddress;
    uint16_t sourcePort;
    bool requiresAction;
    std::string recommendedAction;
    
    NetworkAnomaly() : 
        type(NetworkAnomalyType::CUSTOM_DETECTION),
        confidenceScore(0.0f),
        sourcePort(0),
        requiresAction(false) {}
};

/**
 * @brief Monitor de rede avançado
 */
class NetworkMonitor {
public:
    /**
     * @brief Construtor
     */
    NetworkMonitor();
    
    /**
     * @brief Destrutor
     */
    ~NetworkMonitor();
    
    /**
     * @brief Inicializa o monitor com a configuração especificada
     * @param config Configuração do monitor
     * @return true se inicializado com sucesso
     */
    bool initialize(const NetworkMonitorConfig& config);
    
    /**
     * @brief Inicia a captura de pacotes em uma thread separada
     * @return true se iniciado com sucesso
     */
    bool startCapture();
    
    /**
     * @brief Para a captura de pacotes
     */
    void stopCapture();
    
    /**
     * @brief Verifica se a captura está ativa
     * @return true se a captura estiver ativa
     */
    bool isCapturing() const;
    
    /**
     * @brief Obtém informações sobre a captura atual
     * @return Informações da captura
     */
    CaptureInfo getCaptureInfo() const;
    
    /**
     * @brief Obtém estatísticas de todas as conexões monitoradas
     * @return Mapa de conexões com suas estatísticas
     */
    std::vector<ConnectionStats> getConnectionStats() const;
    
    /**
     * @brief Obtém informações sobre as sessões de jogo ativas
     * @return Lista de sessões de jogo
     */
    std::vector<GameSession> getGameSessions() const;
    
    /**
     * @brief Obtém anomalias de rede detectadas
     * @param onlyActive Se true, retorna apenas anomalias ativas
     * @return Lista de anomalias
     */
    std::vector<NetworkAnomaly> getDetectedAnomalies(bool onlyActive = true) const;
    
    /**
     * @brief Define o analisador de protocolo para processar pacotes
     * @param analyzer Ponteiro para o analisador
     */
    void setProtocolAnalyzer(std::shared_ptr<ProtocolAnalyzer> analyzer);
    
    /**
     * @brief Obtém o analisador de protocolo usado
     * @return Ponteiro para o analisador
     */
    std::shared_ptr<ProtocolAnalyzer> getProtocolAnalyzer() const;
    
    /**
     * @brief Registra um callback para notificações de pacotes
     * @param callback Função de callback
     * @return ID da inscrição
     */
    core::EventBus::SubscriptionID registerPacketCallback(
        std::function<void(const PacketCapture&)> callback);
    
    /**
     * @brief Registra um callback para notificações de anomalias
     * @param callback Função de callback
     * @return ID da inscrição
     */
    core::EventBus::SubscriptionID registerAnomalyCallback(
        std::function<void(const NetworkAnomaly&)> callback);
    
    /**
     * @brief Cancela o registro de um callback
     * @param subscriptionId ID da inscrição
     */
    void unregisterCallback(core::EventBus::SubscriptionID subscriptionId);
    
    /**
     * @brief Salva os pacotes capturados em um arquivo para análise posterior
     * @param filePath Caminho do arquivo
     * @param sessionId ID da sessão (opcional, se não especificado salva todos)
     * @return true se salvo com sucesso
     */
    bool saveCaptureToPcap(const std::string& filePath, const std::string& sessionId = "");
    
    /**
     * @brief Carrega pacotes de um arquivo pcap
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool loadCaptureFromPcap(const std::string& filePath);
    
    /**
     * @brief Adiciona um filtro personalizado para captura de pacotes
     * @param filterExpression Expressão de filtro no formato pcap
     */
    void setCustomFilter(const std::string& filterExpression);
    
    /**
     * @brief Lista interfaces de rede disponíveis
     * @return Lista de nomes de interfaces
     */
    static std::vector<std::string> listAvailableInterfaces();
    
    /**
     * @brief Limpa todas as estatísticas e reinicia contadores
     */
    void resetStatistics();
    
    /**
     * @brief Define limite para detecção de anomalias
     * @param anomalyType Tipo de anomalia
     * @param threshold Limite
     */
    void setAnomalyThreshold(NetworkAnomalyType anomalyType, float threshold);
    
    /**
     * @brief Obtém o limite atual para detecção de anomalias
     * @param anomalyType Tipo de anomalia
     * @return Limite atual
     */
    float getAnomalyThreshold(NetworkAnomalyType anomalyType) const;
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Métodos auxiliares para processamento de pacotes
    void processPacket(const struct pcap_pkthdr* header, const uint8_t* packet);
    void updateConnectionStats(const std::string& srcAddr, uint16_t srcPort,
                               const std::string& dstAddr, uint16_t dstPort,
                               size_t length, bool isIncoming);
    void detectAnomalies(const PacketCapture& packet);
    std::string generateSessionId(const std::string& clientAddr, uint16_t clientPort,
                                 const std::string& serverAddr, uint16_t serverPort);
    bool isGamePacket(uint16_t port) const;
    void updateGameSession(const std::string& sessionId, const DecodedPacket& packet);
    void buildPcapFilter();
};

/**
 * @brief Fábrica para criar instâncias do monitor de rede
 */
class NetworkMonitorFactory {
public:
    /**
     * @brief Cria um monitor de rede com configuração padrão
     * @return Ponteiro para o monitor
     */
    static std::shared_ptr<NetworkMonitor> createDefaultMonitor();
    
    /**
     * @brief Cria um monitor de rede com configuração personalizada
     * @param config Configuração do monitor
     * @return Ponteiro para o monitor
     */
    static std::shared_ptr<NetworkMonitor> createCustomMonitor(const NetworkMonitorConfig& config);
    
    /**
     * @brief Cria um monitor de rede integrado com o analisador de protocolo
     * @param analyzer Ponteiro para o analisador de protocolo
     * @return Ponteiro para o monitor
     */
    static std::shared_ptr<NetworkMonitor> createIntegratedMonitor(std::shared_ptr<ProtocolAnalyzer> analyzer);
    
    /**
     * @brief Cria um monitor de rede para uma interface específica
     * @param interfaceName Nome da interface
     * @return Ponteiro para o monitor
     */
    static std::shared_ptr<NetworkMonitor> createForInterface(const std::string& interfaceName);
};

} // namespace network
} // namespace wydstudio

#endif // WYDSTUDIO_NETWORK_MONITOR_H