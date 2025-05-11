#include "NetworkMonitor.h"
#include "ProtocolAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <signal.h>

using namespace wydstudio;
using namespace wydstudio::network;

// Sinalizador para controle da execução
volatile sig_atomic_t running = 1;

// Handler para sinais
void signal_handler(int signal) {
    running = 0;
}

// Mutex e condição para sincronização
std::mutex mutex;
std::condition_variable cv;

// Callback para novos pacotes
void packet_callback(const PacketCapture& packet) {
    std::cout << "Pacote capturado - De: " << packet.source << " Para: " << packet.destination
              << " Direção: " << (packet.direction == PacketDirection::CLIENT_TO_SERVER ? "Cliente->Servidor" :
                                  packet.direction == PacketDirection::SERVER_TO_CLIENT ? "Servidor->Cliente" : "Desconhecida")
              << " Tamanho: " << packet.data.size() << " bytes" << std::endl;
    
    // Exibir parte dos dados em formato hexadecimal
    std::cout << "Dados (primeiros 32 bytes): ";
    for (size_t i = 0; i < std::min<size_t>(32, packet.data.size()); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(packet.data[i]) << " ";
    }
    std::cout << std::dec << std::endl << std::endl;
}

// Callback para anomalias detectadas
void anomaly_callback(const NetworkAnomaly& anomaly) {
    std::cout << "!!! ANOMALIA DETECTADA !!!" << std::endl;
    std::cout << "Tipo: ";
    
    switch (anomaly.type) {
        case NetworkAnomalyType::EXCESSIVE_PACKETS:
            std::cout << "Excesso de pacotes";
            break;
        case NetworkAnomalyType::SUSPICIOUS_TIMING:
            std::cout << "Timing suspeito";
            break;
        case NetworkAnomalyType::MODIFIED_PACKET_STRUCTURE:
            std::cout << "Estrutura de pacote modificada";
            break;
        case NetworkAnomalyType::INVALID_GAME_STATE:
            std::cout << "Estado de jogo inválido";
            break;
        case NetworkAnomalyType::UNEXPECTED_PACKET_SEQUENCE:
            std::cout << "Sequência de pacotes inesperada";
            break;
        case NetworkAnomalyType::DATA_MANIPULATION:
            std::cout << "Manipulação de dados";
            break;
        case NetworkAnomalyType::PACKET_REPLAY:
            std::cout << "Replay de pacotes";
            break;
        case NetworkAnomalyType::UNAUTHORIZED_TOOL:
            std::cout << "Ferramenta não autorizada";
            break;
        default:
            std::cout << "Outra";
            break;
    }
    
    std::cout << std::endl;
    std::cout << "Descrição: " << anomaly.description << std::endl;
    std::cout << "Origem: " << anomaly.sourceAddress << std::endl;
    std::cout << "Confiança: " << anomaly.confidenceScore * 100 << "%" << std::endl;
    std::cout << "Ação recomendada: " << anomaly.recommendedAction << std::endl << std::endl;
}

// Função para exibir estatísticas periodicamente
void display_stats(std::shared_ptr<NetworkMonitor> monitor) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        auto captureInfo = monitor->getCaptureInfo();
        auto connStats = monitor->getConnectionStats();
        auto sessions = monitor->getGameSessions();
        
        std::cout << "=== Estatísticas de Captura ===" << std::endl;
        std::cout << "Interface: " << captureInfo.deviceName << std::endl;
        std::cout << "Filtro: " << captureInfo.filterExpression << std::endl;
        std::cout << "Pacotes totais: " << captureInfo.packetsTotal << std::endl;
        std::cout << "Pacotes de jogo: " << captureInfo.packetsGame << std::endl;
        std::cout << "Bytes totais: " << captureInfo.bytesTotal << std::endl;
        std::cout << "Bytes de jogo: " << captureInfo.bytesGame << std::endl;
        std::cout << "Pacotes descartados: " << captureInfo.droppedPackets << std::endl;
        
        std::cout << "\n=== Conexões Ativas: " << connStats.size() << " ===" << std::endl;
        for (size_t i = 0; i < std::min<size_t>(5, connStats.size()); ++i) {
            const auto& conn = connStats[i];
            std::cout << conn.sourceAddress << ":" << conn.sourcePort << " -> "
                      << conn.destAddress << ":" << conn.destPort
                      << " (Pacotes: " << conn.packetsIn + conn.packetsOut
                      << ", Bytes: " << conn.bytesIn + conn.bytesOut << ")" << std::endl;
        }
        
        std::cout << "\n=== Sessões de Jogo: " << sessions.size() << " ===" << std::endl;
        for (const auto& session : sessions) {
            std::cout << "Sessão: " << session.sessionId << std::endl;
            std::cout << "  Cliente: " << session.clientAddress << ":" << session.clientPort << std::endl;
            std::cout << "  Servidor: " << session.serverAddress << ":" << session.serverPort << std::endl;
            
            if (!session.accountName.empty()) {
                std::cout << "  Conta: " << session.accountName << std::endl;
            }
            
            if (!session.characterName.empty()) {
                std::cout << "  Personagem: " << session.characterName;
                
                if (!session.gameState.characterClass.empty()) {
                    std::cout << " (" << session.gameState.characterClass << ")";
                }
                
                if (session.gameState.characterLevel > 0) {
                    std::cout << " Nível " << session.gameState.characterLevel;
                }
                
                std::cout << std::endl;
            }
            
            if (!session.gameState.currentMap.empty()) {
                std::cout << "  Mapa: " << session.gameState.currentMap
                          << " (" << session.gameState.posX << ", " << session.gameState.posY << ")" << std::endl;
            }
            
            if (session.gameState.guildId >= 0) {
                std::cout << "  Guilda: " << session.gameState.guildId << std::endl;
            }
            
            std::cout << "  Pacotes: " << session.packetCount << std::endl;
            std::cout << std::endl;
        }
        
        std::cout << "==========================================" << std::endl << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Configurar handler para sinais
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    std::cout << "WYD Network Monitor Example" << std::endl;
    std::cout << "============================" << std::endl;
    
    // Criar analisador de protocolo
    auto analyzer = std::make_shared<ProtocolAnalyzer>();
    
    // Carregar definições de pacotes (supondo que existe um arquivo com definições)
    std::cout << "Carregando definições de pacotes..." << std::endl;
    bool definitionsLoaded = false;
    
    try {
        // Tentar carregar de um arquivo de definições padrão
        definitionsLoaded = analyzer->loadPacketDefinitions("packet_definitions.json");
    }
    catch (const std::exception& e) {
        std::cerr << "Erro ao carregar definições: " << e.what() << std::endl;
    }
    
    if (!definitionsLoaded) {
        std::cout << "Definições não encontradas. Usando detecção automática." << std::endl;
    }
    
    // Listar interfaces disponíveis
    std::cout << "Interfaces de rede disponíveis:" << std::endl;
    auto interfaces = NetworkMonitor::listAvailableInterfaces();
    
    for (size_t i = 0; i < interfaces.size(); ++i) {
        std::cout << i + 1 << ": " << interfaces[i] << std::endl;
    }
    
    // Selecionar interface
    std::string interfaceName;
    
    if (interfaces.empty()) {
        std::cout << "Nenhuma interface disponível!" << std::endl;
        return 1;
    }
    else if (interfaces.size() == 1) {
        interfaceName = interfaces[0];
        std::cout << "Usando única interface disponível: " << interfaceName << std::endl;
    }
    else {
        if (argc > 1) {
            // Interface especificada como argumento
            interfaceName = argv[1];
        }
        else {
            // Usar primeira interface como padrão
            interfaceName = interfaces[0];
        }
        
        std::cout << "Usando interface: " << interfaceName << std::endl;
    }
    
    // Configurar e inicializar o monitor
    NetworkMonitorConfig config;
    config.interfaceName = interfaceName;
    config.filterType = CaptureFilterType::GAME_TRAFFIC_ONLY;
    config.promiscuousMode = true;
    
    // Adicionar portas de jogo do WYD
    config.gamePortsToMonitor = {8281, 8282, 8283, 8284, 8285, 8286, 8287, 8288, 8289, 8290};
    
    std::cout << "Inicializando monitor de rede..." << std::endl;
    auto monitor = NetworkMonitorFactory::createCustomMonitor(config);
    monitor->setProtocolAnalyzer(analyzer);
    
    // Registrar callbacks
    auto packetSubscription = monitor->registerPacketCallback(packet_callback);
    auto anomalySubscription = monitor->registerAnomalyCallback(anomaly_callback);
    
    // Iniciar thread para exibir estatísticas
    std::thread statsThread(display_stats, monitor);
    
    // Iniciar captura
    std::cout << "Iniciando captura de pacotes..." << std::endl;
    if (!monitor->startCapture()) {
        std::cerr << "Erro ao iniciar captura!" << std::endl;
        running = 0;
        statsThread.join();
        return 1;
    }
    
    std::cout << "Captura iniciada. Pressione Ctrl+C para sair." << std::endl;
    
    // Aguardar sinal de término
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, []{ return running == 0; });
    
    // Finalizar
    std::cout << "Finalizando..." << std::endl;
    
    monitor->stopCapture();
    
    // Aguardar thread de estatísticas
    if (statsThread.joinable()) {
        statsThread.join();
    }
    
    // Cancelar callbacks
    monitor->unregisterCallback(packetSubscription);
    monitor->unregisterCallback(anomalySubscription);
    
    // Exibir estatísticas finais
    auto captureInfo = monitor->getCaptureInfo();
    std::cout << "Estatísticas Finais:" << std::endl;
    std::cout << "Pacotes totais: " << captureInfo.packetsTotal << std::endl;
    std::cout << "Pacotes de jogo: " << captureInfo.packetsGame << std::endl;
    std::cout << "Bytes totais: " << captureInfo.bytesTotal << std::endl;
    std::cout << "Bytes de jogo: " << captureInfo.bytesGame << std::endl;
    
    return 0;
}