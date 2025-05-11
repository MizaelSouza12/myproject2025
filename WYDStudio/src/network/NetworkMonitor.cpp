#include "NetworkMonitor.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <future>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pcap/pcap.h>

namespace wydstudio {
namespace network {

//-----------------------------------------------------------------------------
// Implementação interna do NetworkMonitor
//-----------------------------------------------------------------------------

class NetworkMonitor::Impl {
public:
    Impl(NetworkMonitor* monitor)
        : m_monitor(monitor),
          m_isInitialized(false),
          m_isCapturing(false),
          m_pcapHandle(nullptr),
          m_eventBus(std::make_shared<core::EventBus>()),
          m_taskScheduler(std::make_shared<core::TaskScheduler>()) {
    }
    
    ~Impl() {
        stopCapture();
        
        if (m_pcapHandle) {
            pcap_close(m_pcapHandle);
            m_pcapHandle = nullptr;
        }
    }
    
    // Membros internos
    NetworkMonitor* m_monitor;
    NetworkMonitorConfig m_config;
    bool m_isInitialized;
    std::atomic<bool> m_isCapturing;
    pcap_t* m_pcapHandle;
    std::thread m_captureThread;
    std::shared_ptr<ProtocolAnalyzer> m_protocolAnalyzer;
    
    std::mutex m_captureMutex;
    CaptureInfo m_captureInfo;
    
    std::mutex m_connectionsMutex;
    std::map<std::string, ConnectionStats> m_connections;
    
    std::mutex m_sessionsMutex;
    std::map<std::string, GameSession> m_gameSessions;
    
    std::mutex m_anomaliesMutex;
    std::vector<NetworkAnomaly> m_anomalies;
    
    std::shared_ptr<core::EventBus> m_eventBus;
    std::shared_ptr<core::TaskScheduler> m_taskScheduler;
    
    std::map<NetworkAnomalyType, float> m_anomalyThresholds;
    
    // Métodos de implementação
    
    bool initialize(const NetworkMonitorConfig& config) {
        if (m_isCapturing) {
            return false;
        }
        
        m_config = config;
        
        // Abrir o dispositivo para captura
        char errbuf[PCAP_ERRBUF_SIZE];
        m_pcapHandle = pcap_open_live(
            config.interfaceName.c_str(),
            config.snapLength,
            config.promiscuousMode ? 1 : 0,
            config.readTimeout,
            errbuf
        );
        
        if (!m_pcapHandle) {
            std::cerr << "Error opening device: " << errbuf << std::endl;
            return false;
        }
        
        // Configurar filtro de captura
        buildPcapFilter();
        
        // Inicializar configurações de anomalia
        initializeAnomalyThresholds();
        
        // Inicializar informações de captura
        m_captureInfo = CaptureInfo();
        m_captureInfo.deviceName = config.interfaceName;
        
        m_isInitialized = true;
        return true;
    }
    
    void buildPcapFilter() {
        if (!m_pcapHandle) {
            return;
        }
        
        std::string filterExpression;
        
        switch (m_config.filterType) {
            case CaptureFilterType::ALL_TRAFFIC:
                filterExpression = "ip";  // Capturar todo tráfego IP
                break;
                
            case CaptureFilterType::GAME_TRAFFIC_ONLY: {
                std::stringstream ss;
                ss << "tcp and (";
                
                // Adicionar todas as portas do jogo ao filtro
                for (size_t i = 0; i < m_config.gamePortsToMonitor.size(); ++i) {
                    if (i > 0) {
                        ss << " or ";
                    }
                    ss << "port " << m_config.gamePortsToMonitor[i];
                }
                
                ss << ")";
                filterExpression = ss.str();
                break;
            }
                
            case CaptureFilterType::CUSTOM:
                filterExpression = m_config.customFilter;
                break;
        }
        
        if (!m_config.captureLocalTraffic) {
            filterExpression += " and not host localhost and not host 127.0.0.1";
        }
        
        // Compilar e aplicar o filtro
        struct bpf_program fp;
        if (pcap_compile(m_pcapHandle, &fp, filterExpression.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
            std::cerr << "Error compiling filter: " << pcap_geterr(m_pcapHandle) << std::endl;
            return;
        }
        
        if (pcap_setfilter(m_pcapHandle, &fp) == -1) {
            std::cerr << "Error setting filter: " << pcap_geterr(m_pcapHandle) << std::endl;
            pcap_freecode(&fp);
            return;
        }
        
        pcap_freecode(&fp);
        
        // Atualizar informações de captura
        m_captureInfo.filterExpression = filterExpression;
    }
    
    void initializeAnomalyThresholds() {
        // Valores padrão para detecção de anomalias
        m_anomalyThresholds[NetworkAnomalyType::EXCESSIVE_PACKETS] = 1000.0f;     // pacotes/segundo
        m_anomalyThresholds[NetworkAnomalyType::SUSPICIOUS_TIMING] = 50.0f;       // ms de tolerância
        m_anomalyThresholds[NetworkAnomalyType::MODIFIED_PACKET_STRUCTURE] = 0.8f; // nível de confiança
        m_anomalyThresholds[NetworkAnomalyType::INVALID_GAME_STATE] = 0.9f;       // nível de confiança
        m_anomalyThresholds[NetworkAnomalyType::UNEXPECTED_PACKET_SEQUENCE] = 0.7f; // nível de confiança
        m_anomalyThresholds[NetworkAnomalyType::DATA_MANIPULATION] = 0.85f;       // nível de confiança
        m_anomalyThresholds[NetworkAnomalyType::PACKET_REPLAY] = 0.95f;           // nível de confiança
        m_anomalyThresholds[NetworkAnomalyType::UNAUTHORIZED_TOOL] = 0.75f;       // nível de confiança
        m_anomalyThresholds[NetworkAnomalyType::CUSTOM_DETECTION] = 0.6f;         // nível de confiança
    }
    
    bool startCapture() {
        if (!m_isInitialized || m_isCapturing) {
            return false;
        }
        
        // Reset capture info
        std::lock_guard<std::mutex> lock(m_captureMutex);
        m_captureInfo.startTime = std::chrono::system_clock::now();
        m_captureInfo.packetsTotal = 0;
        m_captureInfo.packetsGame = 0;
        m_captureInfo.bytesTotal = 0;
        m_captureInfo.bytesGame = 0;
        m_captureInfo.droppedPackets = 0;
        
        m_isCapturing = true;
        
        // Iniciar thread de captura
        m_captureThread = std::thread([this]() {
            pcap_loop(m_pcapHandle, 0, 
                      [](u_char* user, const struct pcap_pkthdr* header, const u_char* packet) {
                          auto* implPtr = reinterpret_cast<Impl*>(user);
                          implPtr->m_monitor->processPacket(header, packet);
                      }, 
                      reinterpret_cast<u_char*>(this));
        });
        
        return true;
    }
    
    void stopCapture() {
        if (m_isCapturing && m_pcapHandle) {
            m_isCapturing = false;
            pcap_breakloop(m_pcapHandle);
            
            if (m_captureThread.joinable()) {
                m_captureThread.join();
            }
        }
    }
    
    void processPacket(const pcap_pkthdr* header, const uint8_t* packet) {
        // Estruturas básicas para parsing de pacotes
        const struct ether_header* ethernetHeader;
        const struct ip* ipHeader;
        const struct tcphdr* tcpHeader;
        const struct udphdr* udpHeader;
        
        // Obter cabeçalho Ethernet
        ethernetHeader = reinterpret_cast<const struct ether_header*>(packet);
        
        // Verificar se é um pacote IP
        if (ntohs(ethernetHeader->ether_type) != ETHERTYPE_IP) {
            return;
        }
        
        // Obter cabeçalho IP
        ipHeader = reinterpret_cast<const struct ip*>(packet + sizeof(struct ether_header));
        
        // Obter endereços IP de origem e destino
        char srcIp[INET_ADDRSTRLEN];
        char dstIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ipHeader->ip_src), srcIp, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ipHeader->ip_dst), dstIp, INET_ADDRSTRLEN);
        
        // Determinar o protocolo de transporte
        uint16_t srcPort = 0;
        uint16_t dstPort = 0;
        const uint8_t* payload = nullptr;
        size_t payloadLength = 0;
        
        if (ipHeader->ip_p == IPPROTO_TCP) {
            // Pacote TCP
            size_t ipHeaderLength = ipHeader->ip_hl * 4;
            tcpHeader = reinterpret_cast<const struct tcphdr*>(
                reinterpret_cast<const uint8_t*>(ipHeader) + ipHeaderLength);
            
            srcPort = ntohs(tcpHeader->th_sport);
            dstPort = ntohs(tcpHeader->th_dport);
            
            // Obter payload
            size_t tcpHeaderLength = tcpHeader->th_off * 4;
            payload = reinterpret_cast<const uint8_t*>(tcpHeader) + tcpHeaderLength;
            
            // Calcular tamanho do payload
            payloadLength = ntohs(ipHeader->ip_len) - ipHeaderLength - tcpHeaderLength;
        }
        else if (ipHeader->ip_p == IPPROTO_UDP) {
            // Pacote UDP
            size_t ipHeaderLength = ipHeader->ip_hl * 4;
            udpHeader = reinterpret_cast<const struct udphdr*>(
                reinterpret_cast<const uint8_t*>(ipHeader) + ipHeaderLength);
            
            srcPort = ntohs(udpHeader->uh_sport);
            dstPort = ntohs(udpHeader->uh_dport);
            
            // Obter payload
            payload = reinterpret_cast<const uint8_t*>(udpHeader) + sizeof(struct udphdr);
            
            // Calcular tamanho do payload
            payloadLength = ntohs(udpHeader->uh_ulen) - sizeof(struct udphdr);
        }
        else {
            // Outro protocolo, ignorar
            return;
        }
        
        // Atualizar estatísticas de captura
        {
            std::lock_guard<std::mutex> lock(m_captureMutex);
            m_captureInfo.packetsTotal++;
            m_captureInfo.bytesTotal += header->len;
            
            struct pcap_stat stats;
            if (pcap_stats(m_pcapHandle, &stats) == 0) {
                m_captureInfo.droppedPackets = stats.ps_drop;
            }
        }
        
        // Verificar se é tráfego de jogo
        bool isGameTraffic = isGamePacket(srcPort) || isGamePacket(dstPort);
        
        if (isGameTraffic) {
            // Atualizar estatísticas de tráfego de jogo
            std::lock_guard<std::mutex> lock(m_captureMutex);
            m_captureInfo.packetsGame++;
            m_captureInfo.bytesGame += header->len;
            
            // Determinar direção do pacote
            PacketDirection direction = PacketDirection::UNKNOWN;
            
            // Verificar portas para determinar a direção
            bool srcIsGamePort = isGamePacket(srcPort);
            bool dstIsGamePort = isGamePacket(dstPort);
            
            if (srcIsGamePort && !dstIsGamePort) {
                direction = PacketDirection::SERVER_TO_CLIENT;
            }
            else if (!srcIsGamePort && dstIsGamePort) {
                direction = PacketDirection::CLIENT_TO_SERVER;
            }
            
            // Criar ID de sessão
            std::string clientAddr, serverAddr;
            uint16_t clientPort, serverPort;
            
            if (direction == PacketDirection::CLIENT_TO_SERVER) {
                clientAddr = srcIp;
                clientPort = srcPort;
                serverAddr = dstIp;
                serverPort = dstPort;
            }
            else if (direction == PacketDirection::SERVER_TO_CLIENT) {
                clientAddr = dstIp;
                clientPort = dstPort;
                serverAddr = srcIp;
                serverPort = srcPort;
            }
            else {
                // Não é possível determinar com certeza, usar ordem lexicográfica
                if (std::string(srcIp) < std::string(dstIp)) {
                    clientAddr = srcIp;
                    clientPort = srcPort;
                    serverAddr = dstIp;
                    serverPort = dstPort;
                }
                else {
                    clientAddr = dstIp;
                    clientPort = dstPort;
                    serverAddr = srcIp;
                    serverPort = srcPort;
                }
            }
            
            std::string sessionId = generateSessionId(clientAddr, clientPort, 
                                                     serverAddr, serverPort);
            
            // Criar captura de pacote para analisar
            PacketCapture capture;
            capture.source = srcIp;
            capture.destination = dstIp;
            capture.timestamp = std::chrono::system_clock::now();
            
            // Copiar apenas o payload
            if (payload && payloadLength > 0) {
                capture.data.assign(payload, payload + payloadLength);
            }
            
            capture.direction = direction;
            capture.captureId = m_captureInfo.packetsGame;
            capture.sessionId = sessionId;
            
            // Enviar pacote para análise
            if (m_protocolAnalyzer && !capture.data.empty()) {
                auto decodedPacket = m_protocolAnalyzer->decodePacket(capture.data, direction);
                
                // Atualizar informações da sessão de jogo
                updateGameSession(sessionId, decodedPacket);
                
                // Detectar anomalias
                detectAnomalies(capture, decodedPacket);
            }
            
            // Atualizar estatísticas de conexão
            updateConnectionStats(srcIp, srcPort, dstIp, dstPort, header->len, 
                                 direction == PacketDirection::SERVER_TO_CLIENT);
            
            // Notificar observadores
            m_eventBus->publish("packet.captured", capture);
        }
    }
    
    void updateConnectionStats(const std::string& srcAddr, uint16_t srcPort,
                              const std::string& dstAddr, uint16_t dstPort,
                              size_t length, bool isIncoming) {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        
        // Criar um ID único para essa conexão
        std::string connectionId = srcAddr + ":" + std::to_string(srcPort) + "-" +
                                  dstAddr + ":" + std::to_string(dstPort);
        
        // Adicionar ou atualizar estatísticas
        auto it = m_connections.find(connectionId);
        if (it == m_connections.end()) {
            // Nova conexão
            ConnectionStats stats;
            stats.sourceAddress = srcAddr;
            stats.sourcePort = srcPort;
            stats.destAddress = dstAddr;
            stats.destPort = dstPort;
            stats.firstSeen = std::chrono::system_clock::now();
            stats.lastSeen = stats.firstSeen;
            stats.isGameTraffic = isGamePacket(srcPort) || isGamePacket(dstPort);
            
            if (isIncoming) {
                stats.packetsIn = 1;
                stats.bytesIn = length;
            }
            else {
                stats.packetsOut = 1;
                stats.bytesOut = length;
            }
            
            // Determinar direção se for tráfego de jogo
            if (stats.isGameTraffic) {
                if (isGamePacket(srcPort)) {
                    stats.direction = PacketDirection::SERVER_TO_CLIENT;
                }
                else if (isGamePacket(dstPort)) {
                    stats.direction = PacketDirection::CLIENT_TO_SERVER;
                }
            }
            
            m_connections[connectionId] = stats;
        }
        else {
            // Conexão existente
            ConnectionStats& stats = it->second;
            stats.lastSeen = std::chrono::system_clock::now();
            
            if (isIncoming) {
                stats.packetsIn++;
                stats.bytesIn += length;
            }
            else {
                stats.packetsOut++;
                stats.bytesOut += length;
            }
        }
    }
    
    void updateGameSession(const std::string& sessionId, const DecodedPacket& packet) {
        std::lock_guard<std::mutex> lock(m_sessionsMutex);
        
        // Adicionar ou atualizar sessão
        auto it = m_gameSessions.find(sessionId);
        if (it == m_gameSessions.end()) {
            // Nova sessão
            GameSession session;
            session.sessionId = sessionId;
            session.startTime = std::chrono::system_clock::now();
            session.lastActivity = session.startTime;
            session.packetCount = 1;
            
            // Extrair informações da sessão de strings no ID
            // Formato: clientAddr:clientPort-serverAddr:serverPort
            std::string clientInfo = sessionId.substr(0, sessionId.find('-'));
            std::string serverInfo = sessionId.substr(sessionId.find('-') + 1);
            
            session.clientAddress = clientInfo.substr(0, clientInfo.find(':'));
            session.clientPort = std::stoi(clientInfo.substr(clientInfo.find(':') + 1));
            session.serverAddress = serverInfo.substr(0, serverInfo.find(':'));
            session.serverPort = std::stoi(serverInfo.substr(serverInfo.find(':') + 1));
            
            m_gameSessions[sessionId] = session;
        }
        else {
            // Sessão existente
            GameSession& session = it->second;
            session.lastActivity = std::chrono::system_clock::now();
            session.packetCount++;
            
            // Tentar extrair informações do estado do jogo do pacote decodificado
            updateGameStateFromPacket(session, packet);
        }
    }
    
    void updateGameStateFromPacket(GameSession& session, const DecodedPacket& packet) {
        // Verificar campos específicos baseados no tipo de pacote
        if (packet.name.find("Login") != std::string::npos || 
            packet.name.find("Auth") != std::string::npos) {
            // Pacote de login/autenticação
            auto accountField = packet.getFieldByName("AccountName");
            if (accountField) {
                try {
                    session.accountName = std::get<std::string>(accountField->value);
                }
                catch (const std::bad_variant_access&) {}
            }
        }
        else if (packet.name.find("Character") != std::string::npos) {
            // Pacote relacionado a personagem
            auto nameField = packet.getFieldByName("CharacterName");
            if (nameField) {
                try {
                    session.characterName = std::get<std::string>(nameField->value);
                }
                catch (const std::bad_variant_access&) {}
            }
            
            auto levelField = packet.getFieldByName("Level");
            if (levelField) {
                try {
                    session.gameState.characterLevel = std::get<int32_t>(levelField->value);
                }
                catch (const std::bad_variant_access&) {}
            }
            
            auto classField = packet.getFieldByName("Class");
            if (classField) {
                try {
                    auto classId = std::get<int32_t>(classField->value);
                    switch (classId) {
                        case 0: session.gameState.characterClass = "Transknight"; break;
                        case 1: session.gameState.characterClass = "Foema"; break;
                        case 2: session.gameState.characterClass = "BeastMaster"; break;
                        case 3: session.gameState.characterClass = "Huntress"; break;
                        default: session.gameState.characterClass = "Unknown";
                    }
                }
                catch (const std::bad_variant_access&) {}
            }
        }
        else if (packet.name.find("Move") != std::string::npos || 
                packet.name.find("Position") != std::string::npos) {
            // Pacote de movimento ou posição
            auto posXField = packet.getFieldByName("PosX");
            auto posYField = packet.getFieldByName("PosY");
            
            if (posXField && posYField) {
                try {
                    session.gameState.posX = std::get<int32_t>(posXField->value);
                    session.gameState.posY = std::get<int32_t>(posYField->value);
                }
                catch (const std::bad_variant_access&) {}
            }
            
            auto mapField = packet.getFieldByName("MapID");
            if (mapField) {
                try {
                    int32_t mapId = std::get<int32_t>(mapField->value);
                    session.gameState.currentMap = "Map_" + std::to_string(mapId);
                }
                catch (const std::bad_variant_access&) {}
            }
        }
        else if (packet.name.find("Guild") != std::string::npos) {
            // Pacote relacionado a guilda
            auto guildIdField = packet.getFieldByName("GuildID");
            if (guildIdField) {
                try {
                    session.gameState.guildId = std::get<int32_t>(guildIdField->value);
                }
                catch (const std::bad_variant_access&) {}
            }
        }
        else if (packet.name.find("Party") != std::string::npos) {
            // Pacote relacionado a grupo/party
            auto partyListField = packet.getFieldByName("PartyMembers");
            if (partyListField) {
                try {
                    session.gameState.partyMembers = std::get<std::string>(partyListField->value);
                }
                catch (const std::bad_variant_access&) {}
            }
        }
        else if (packet.name.find("Action") != std::string::npos) {
            // Pacote de ação do jogador
            auto actionField = packet.getFieldByName("ActionType");
            if (actionField) {
                try {
                    int32_t actionType = std::get<int32_t>(actionField->value);
                    switch (actionType) {
                        case 0: session.gameState.lastAction = "Attack"; break;
                        case 1: session.gameState.lastAction = "Skill"; break;
                        case 2: session.gameState.lastAction = "UseItem"; break;
                        default: session.gameState.lastAction = "Unknown";
                    }
                }
                catch (const std::bad_variant_access&) {}
            }
        }
    }
    
    void detectAnomalies(const PacketCapture& packet, const DecodedPacket& decodedPacket) {
        // Esta é uma versão simplificada da detecção de anomalias
        // Uma implementação real seria muito mais complexa
        
        // Verificar se o pacote é válido
        if (decodedPacket.validationState != ValidationState::VALID) {
            NetworkAnomaly anomaly;
            anomaly.type = NetworkAnomalyType::MODIFIED_PACKET_STRUCTURE;
            anomaly.sessionId = packet.sessionId;
            anomaly.description = "Invalid packet structure: " + decodedPacket.validationMessage;
            anomaly.confidenceScore = 0.9f;
            anomaly.detectionTime = std::chrono::system_clock::now();
            anomaly.sourceAddress = packet.source;
            anomaly.requiresAction = true;
            anomaly.recommendedAction = "Validate client integrity";
            
            // Registrar anomalia
            std::lock_guard<std::mutex> lock(m_anomaliesMutex);
            m_anomalies.push_back(anomaly);
            
            // Notificar observadores
            m_eventBus->publish("anomaly.detected", anomaly);
        }
        
        // DEMO: Detecção de alteração na estrutura do pacote
        // Uma implementação real usaria algoritmos mais avançados
        
        // Verificar se o pacote foi modificado comparando com a definição conhecida
        if (m_protocolAnalyzer && m_protocolAnalyzer->hasPacketDefinition(decodedPacket.id)) {
            auto definition = m_protocolAnalyzer->getPacketDefinition(decodedPacket.id);
            
            // Verificar se todos os campos obrigatórios estão presentes
            for (const auto& fieldDef : definition.fields) {
                if (fieldDef.isRequired && !decodedPacket.hasField(fieldDef.name)) {
                    NetworkAnomaly anomaly;
                    anomaly.type = NetworkAnomalyType::MODIFIED_PACKET_STRUCTURE;
                    anomaly.sessionId = packet.sessionId;
                    anomaly.description = "Missing required field: " + fieldDef.name;
                    anomaly.confidenceScore = 0.95f;
                    anomaly.detectionTime = std::chrono::system_clock::now();
                    anomaly.sourceAddress = packet.source;
                    anomaly.requiresAction = true;
                    anomaly.recommendedAction = "Validate client integrity";
                    
                    // Registrar anomalia
                    std::lock_guard<std::mutex> lock(m_anomaliesMutex);
                    m_anomalies.push_back(anomaly);
                    
                    // Notificar observadores
                    m_eventBus->publish("anomaly.detected", anomaly);
                }
            }
        }
        
        // Detecção baseada em sessão
        std::lock_guard<std::mutex> lock(m_sessionsMutex);
        auto it = m_gameSessions.find(packet.sessionId);
        if (it != m_gameSessions.end()) {
            GameSession& session = it->second;
            
            // Verificar taxa de pacotes
            auto sessionDuration = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - session.startTime).count();
            
            if (sessionDuration > 0) {
                double packetsPerSecond = static_cast<double>(session.packetCount) / sessionDuration;
                
                // Verificar se excede o limite
                if (packetsPerSecond > m_anomalyThresholds[NetworkAnomalyType::EXCESSIVE_PACKETS]) {
                    NetworkAnomaly anomaly;
                    anomaly.type = NetworkAnomalyType::EXCESSIVE_PACKETS;
                    anomaly.sessionId = packet.sessionId;
                    anomaly.description = "Excessive packet rate: " + 
                                        std::to_string(packetsPerSecond) + " packets/second";
                    anomaly.confidenceScore = 0.85f;
                    anomaly.detectionTime = std::chrono::system_clock::now();
                    anomaly.sourceAddress = packet.source;
                    anomaly.requiresAction = false;
                    anomaly.recommendedAction = "Monitor";
                    
                    // Registrar anomalia
                    std::lock_guard<std::mutex> anomalyLock(m_anomaliesMutex);
                    m_anomalies.push_back(anomaly);
                    
                    // Notificar observadores
                    m_eventBus->publish("anomaly.detected", anomaly);
                }
            }
        }
    }
    
    bool isGamePacket(uint16_t port) const {
        // Verificar se a porta está na lista de portas de jogo monitoradas
        for (const auto& gamePort : m_config.gamePortsToMonitor) {
            if (port == gamePort) {
                return true;
            }
        }
        return false;
    }
    
    std::string generateSessionId(const std::string& clientAddr, uint16_t clientPort,
                                 const std::string& serverAddr, uint16_t serverPort) {
        // Formato: clientAddr:clientPort-serverAddr:serverPort
        return clientAddr + ":" + std::to_string(clientPort) + "-" +
               serverAddr + ":" + std::to_string(serverPort);
    }
    
    void setAnomalyThreshold(NetworkAnomalyType anomalyType, float threshold) {
        m_anomalyThresholds[anomalyType] = threshold;
    }
    
    float getAnomalyThreshold(NetworkAnomalyType anomalyType) const {
        auto it = m_anomalyThresholds.find(anomalyType);
        if (it != m_anomalyThresholds.end()) {
            return it->second;
        }
        return 0.0f;
    }
};

//-----------------------------------------------------------------------------
// Implementação da interface pública NetworkMonitor
//-----------------------------------------------------------------------------

NetworkMonitor::NetworkMonitor()
    : pImpl(std::make_unique<Impl>(this)) {
}

NetworkMonitor::~NetworkMonitor() = default;

bool NetworkMonitor::initialize(const NetworkMonitorConfig& config) {
    return pImpl->initialize(config);
}

bool NetworkMonitor::startCapture() {
    return pImpl->startCapture();
}

void NetworkMonitor::stopCapture() {
    pImpl->stopCapture();
}

bool NetworkMonitor::isCapturing() const {
    return pImpl->m_isCapturing;
}

CaptureInfo NetworkMonitor::getCaptureInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->m_captureMutex);
    return pImpl->m_captureInfo;
}

std::vector<ConnectionStats> NetworkMonitor::getConnectionStats() const {
    std::lock_guard<std::mutex> lock(pImpl->m_connectionsMutex);
    
    std::vector<ConnectionStats> result;
    result.reserve(pImpl->m_connections.size());
    
    for (const auto& [id, stats] : pImpl->m_connections) {
        result.push_back(stats);
    }
    
    return result;
}

std::vector<GameSession> NetworkMonitor::getGameSessions() const {
    std::lock_guard<std::mutex> lock(pImpl->m_sessionsMutex);
    
    std::vector<GameSession> result;
    result.reserve(pImpl->m_gameSessions.size());
    
    for (const auto& [id, session] : pImpl->m_gameSessions) {
        result.push_back(session);
    }
    
    return result;
}

std::vector<NetworkAnomaly> NetworkMonitor::getDetectedAnomalies(bool onlyActive) const {
    std::lock_guard<std::mutex> lock(pImpl->m_anomaliesMutex);
    
    if (!onlyActive) {
        return pImpl->m_anomalies;
    }
    
    // Filtrar apenas anomalias recentes (últimas 24 horas)
    std::vector<NetworkAnomaly> result;
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24);
    
    for (const auto& anomaly : pImpl->m_anomalies) {
        if (anomaly.detectionTime >= cutoff) {
            result.push_back(anomaly);
        }
    }
    
    return result;
}

void NetworkMonitor::setProtocolAnalyzer(std::shared_ptr<ProtocolAnalyzer> analyzer) {
    pImpl->m_protocolAnalyzer = analyzer;
}

std::shared_ptr<ProtocolAnalyzer> NetworkMonitor::getProtocolAnalyzer() const {
    return pImpl->m_protocolAnalyzer;
}

core::EventBus::SubscriptionID NetworkMonitor::registerPacketCallback(
    std::function<void(const PacketCapture&)> callback) {
    return pImpl->m_eventBus->subscribe("packet.captured", callback);
}

core::EventBus::SubscriptionID NetworkMonitor::registerAnomalyCallback(
    std::function<void(const NetworkAnomaly&)> callback) {
    return pImpl->m_eventBus->subscribe("anomaly.detected", callback);
}

void NetworkMonitor::unregisterCallback(core::EventBus::SubscriptionID subscriptionId) {
    pImpl->m_eventBus->unsubscribe(subscriptionId);
}

bool NetworkMonitor::saveCaptureToPcap(const std::string& filePath, const std::string& sessionId) {
    // Implementação simplificada, em produção seria mais completa
    pcap_dumper_t* dumper = nullptr;
    
    // Abrir arquivo para escrita
    pcap_t* pcap = pcap_open_dead(DLT_EN10MB, 65535);
    if (!pcap) {
        return false;
    }
    
    dumper = pcap_dump_open(pcap, filePath.c_str());
    if (!dumper) {
        pcap_close(pcap);
        return false;
    }
    
    // TODO: Escrever pacotes capturados no arquivo
    // Esta implementação é um stub
    
    pcap_dump_close(dumper);
    pcap_close(pcap);
    
    return true;
}

bool NetworkMonitor::loadCaptureFromPcap(const std::string& filePath) {
    // Implementação simplificada, em produção seria mais completa
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = pcap_open_offline(filePath.c_str(), errbuf);
    
    if (!pcap) {
        return false;
    }
    
    // Processar pacotes do arquivo
    pcap_loop(pcap, 0, 
              [](u_char* user, const struct pcap_pkthdr* header, const u_char* packet) {
                  auto* monitor = reinterpret_cast<NetworkMonitor*>(user);
                  monitor->processPacket(header, packet);
              }, 
              reinterpret_cast<u_char*>(this));
    
    pcap_close(pcap);
    return true;
}

void NetworkMonitor::setCustomFilter(const std::string& filterExpression) {
    pImpl->m_config.filterType = CaptureFilterType::CUSTOM;
    pImpl->m_config.customFilter = filterExpression;
    pImpl->buildPcapFilter();
}

std::vector<std::string> NetworkMonitor::listAvailableInterfaces() {
    std::vector<std::string> interfaces;
    
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t* alldevs;
    
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        return interfaces;
    }
    
    for (pcap_if_t* dev = alldevs; dev; dev = dev->next) {
        interfaces.push_back(dev->name);
    }
    
    pcap_freealldevs(alldevs);
    return interfaces;
}

void NetworkMonitor::resetStatistics() {
    // Resetar estatísticas de captura
    std::lock_guard<std::mutex> captureLock(pImpl->m_captureMutex);
    pImpl->m_captureInfo.packetsTotal = 0;
    pImpl->m_captureInfo.packetsGame = 0;
    pImpl->m_captureInfo.bytesTotal = 0;
    pImpl->m_captureInfo.bytesGame = 0;
    pImpl->m_captureInfo.droppedPackets = 0;
    pImpl->m_captureInfo.startTime = std::chrono::system_clock::now();
    
    // Resetar estatísticas de conexão
    std::lock_guard<std::mutex> connLock(pImpl->m_connectionsMutex);
    pImpl->m_connections.clear();
    
    // Não resetar sessões - elas podem continuar ativas
}

void NetworkMonitor::setAnomalyThreshold(NetworkAnomalyType anomalyType, float threshold) {
    pImpl->setAnomalyThreshold(anomalyType, threshold);
}

float NetworkMonitor::getAnomalyThreshold(NetworkAnomalyType anomalyType) const {
    return pImpl->getAnomalyThreshold(anomalyType);
}

void NetworkMonitor::processPacket(const pcap_pkthdr* header, const uint8_t* packet) {
    pImpl->processPacket(header, packet);
}

void NetworkMonitor::updateConnectionStats(const std::string& srcAddr, uint16_t srcPort,
                                         const std::string& dstAddr, uint16_t dstPort,
                                         size_t length, bool isIncoming) {
    pImpl->updateConnectionStats(srcAddr, srcPort, dstAddr, dstPort, length, isIncoming);
}

void NetworkMonitor::detectAnomalies(const PacketCapture& packet) {
    if (pImpl->m_protocolAnalyzer) {
        auto decodedPacket = pImpl->m_protocolAnalyzer->decodePacket(packet.data, packet.direction);
        pImpl->detectAnomalies(packet, decodedPacket);
    }
}

std::string NetworkMonitor::generateSessionId(const std::string& clientAddr, uint16_t clientPort,
                                           const std::string& serverAddr, uint16_t serverPort) {
    return pImpl->generateSessionId(clientAddr, clientPort, serverAddr, serverPort);
}

bool NetworkMonitor::isGamePacket(uint16_t port) const {
    return pImpl->isGamePacket(port);
}

void NetworkMonitor::updateGameSession(const std::string& sessionId, const DecodedPacket& packet) {
    pImpl->updateGameSession(sessionId, packet);
}

void NetworkMonitor::buildPcapFilter() {
    pImpl->buildPcapFilter();
}

//-----------------------------------------------------------------------------
// Implementação do NetworkMonitorFactory
//-----------------------------------------------------------------------------

std::shared_ptr<NetworkMonitor> NetworkMonitorFactory::createDefaultMonitor() {
    auto monitor = std::make_shared<NetworkMonitor>();
    NetworkMonitorConfig config;
    
    // Detectar interface padrão
    std::string defaultInterface = "eth0";  // Fallback
    std::vector<std::string> interfaces = NetworkMonitor::listAvailableInterfaces();
    if (!interfaces.empty()) {
        defaultInterface = interfaces[0];
    }
    
    config.interfaceName = defaultInterface;
    monitor->initialize(config);
    
    return monitor;
}

std::shared_ptr<NetworkMonitor> NetworkMonitorFactory::createCustomMonitor(const NetworkMonitorConfig& config) {
    auto monitor = std::make_shared<NetworkMonitor>();
    monitor->initialize(config);
    return monitor;
}

std::shared_ptr<NetworkMonitor> NetworkMonitorFactory::createIntegratedMonitor(std::shared_ptr<ProtocolAnalyzer> analyzer) {
    auto monitor = createDefaultMonitor();
    monitor->setProtocolAnalyzer(analyzer);
    return monitor;
}

std::shared_ptr<NetworkMonitor> NetworkMonitorFactory::createForInterface(const std::string& interfaceName) {
    auto monitor = std::make_shared<NetworkMonitor>();
    NetworkMonitorConfig config;
    config.interfaceName = interfaceName;
    monitor->initialize(config);
    return monitor;
}

} // namespace network
} // namespace wydstudio