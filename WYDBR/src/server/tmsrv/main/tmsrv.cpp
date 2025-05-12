/**
 * @file tmsrv.cpp
 * @brief Arquivo principal do Trade Market Server (TMSrv)
 * 
 * Implementação principal do Trade Market Server (TMSrv) do WYD.
 * Este servidor é responsável pelo gerenciamento de trade, batalha, NPCs,
 * movimento de jogadores, e interações do mundo do jogo.
 * 
 * Esta implementação mantém a estrutura original do WYD, mas utiliza
 * técnicas modernas para corrigir bugs conhecidos e melhorar a performance.
 * 
 * @author WYDBRASIL
 * @date 2025-05-06
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <csignal>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#endif

#include "../../../core/WYDTypes.h"
#include "../../../network/WYDPackets.h"
#include "../../../network/NetworkManager.h"
#include "../../../network/SecurityManager.h"
#include "../player/Player.h"
#include "../npc/NPC.h"
#include "../world/World.h"
#include "../database/DatabaseManager.h"
#include "../handlers/PacketHandlers.h"
#include "../config/ServerConfig.h"
#include "../utils/Logger.h"

namespace wydbr {


using namespace wyd;
using namespace wyd::server;
using namespace wyd::net;

namespace wyd {
namespace server {
namespace tmsrv {

// Variáveis globais do servidor (mantidas para compatibilidade com código original)
std::atomic<bool> g_bRunning(false);        // Flag de servidor rodando
std::atomic<bool> g_bShutdown(false);       // Flag de shutdown em progresso
DWORD g_dwServerStartTime = 0;              // Timestamp de início do servidor
WORD g_wServerID = 0;                       // ID deste servidor
WORD g_wServerGroupID = 0;                  // ID do grupo de servidores
char g_szServerName[32] = {0};              // Nome do servidor
char g_szServerPublicIP[16] = {0};          // IP público do servidor
int g_iServerPort = 8281;                   // Porta do servidor

// Estatísticas do servidor
std::atomic<DWORD> g_dwTotalConnections(0);     // Total de conexões desde o início
std::atomic<DWORD> g_dwCurrentConnections(0);   // Conexões atuais
std::atomic<DWORD> g_dwTotalPacketsReceived(0); // Total de pacotes recebidos
std::atomic<DWORD> g_dwTotalPacketsSent(0);     // Total de pacotes enviados
std::atomic<DWORD> g_dwTotalBytesReceived(0);   // Total de bytes recebidos
std::atomic<DWORD> g_dwTotalBytesSent(0);       // Total de bytes enviados

// Objetos principais do servidor
std::unique_ptr<WorldManager> g_pWorldManager;              // Gerenciador de mundo
std::unique_ptr<NPCManager> g_pNPCManager;                  // Gerenciador de NPCs
std::unique_ptr<PlayerManager> g_pPlayerManager;            // Gerenciador de jogadores
std::unique_ptr<DatabaseManager> g_pDatabaseManager;        // Gerenciador de database
std::unique_ptr<PacketHandlerManager> g_pPacketHandlerManager; // Gerenciador de handlers

// Logs do servidor
std::unique_ptr<Logger> g_pLogger;                          // Sistema de log

/**
 * @brief Handler para sinais do sistema (SIGINT, SIGTERM)
 * @param signal O sinal recebido
 */
void SignalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        g_pLogger->Write(LogLevel::INFO, "Sinal %d recebido. Iniciando shutdown...", signal);
        g_bShutdown = true;
    }
}

/**
 * @brief Inicializa o servidor
 * @param configFile Caminho para o arquivo de configuração
 * @return true se a inicialização foi bem-sucedida, false caso contrário
 */
bool InitServer(const std::string& configFile) {
    g_pLogger = std::make_unique<Logger>("logs/tmsrv.log", LogLevel::INFO);
    g_pLogger->Write(LogLevel::INFO, "Iniciando Trade Market Server (TMSrv)...");

    // Carrega configurações do servidor
    ServerConfig config;
    if (!config.Load(configFile)) {
        g_pLogger->Write(LogLevel::ERROR, "Falha ao carregar configurações de: %s", configFile.c_str());
        return false;
    }

    // Aplica configurações
    g_wServerID = config.GetServerID();
    g_wServerGroupID = config.GetServerGroupID();
    strncpy(g_szServerName, config.GetServerName().c_str(), sizeof(g_szServerName) - 1);
    strncpy(g_szServerPublicIP, config.GetServerPublicIP().c_str(), sizeof(g_szServerPublicIP) - 1);
    g_iServerPort = config.GetServerPort();

    g_pLogger->Write(LogLevel::INFO, "Configurações carregadas: Servidor [%d:%d] %s (%s:%d)",
        g_wServerID, g_wServerGroupID, g_szServerName, g_szServerPublicIP, g_iServerPort);

    // Inicializa gerenciadores
    g_pLogger->Write(LogLevel::INFO, "Inicializando Database Manager...");
    g_pDatabaseManager = std::make_unique<DatabaseManager>();
    if (!g_pDatabaseManager->Initialize(config.GetDatabaseConfig())) {
        g_pLogger->Write(LogLevel::ERROR, "Falha ao inicializar Database Manager");
        return false;
    }

    g_pLogger->Write(LogLevel::INFO, "Inicializando World Manager...");
    g_pWorldManager = std::make_unique<WorldManager>();
    if (!g_pWorldManager->Initialize(config.GetWorldConfig())) {
        g_pLogger->Write(LogLevel::ERROR, "Falha ao inicializar World Manager");
        return false;
    }

    g_pLogger->Write(LogLevel::INFO, "Inicializando NPC Manager...");
    g_pNPCManager = std::make_unique<NPCManager>();
    if (!g_pNPCManager->Initialize(config.GetNPCConfig())) {
        g_pLogger->Write(LogLevel::ERROR, "Falha ao inicializar NPC Manager");
        return false;
    }

    g_pLogger->Write(LogLevel::INFO, "Inicializando Player Manager...");
    g_pPlayerManager = std::make_unique<PlayerManager>();
    if (!g_pPlayerManager->Initialize(config.GetPlayerConfig())) {
        g_pLogger->Write(LogLevel::ERROR, "Falha ao inicializar Player Manager");
        return false;
    }

    g_pLogger->Write(LogLevel::INFO, "Inicializando Packet Handler Manager...");
    g_pPacketHandlerManager = std::make_unique<PacketHandlerManager>();
    if (!g_pPacketHandlerManager->Initialize()) {
        g_pLogger->Write(LogLevel::ERROR, "Falha ao inicializar Packet Handler Manager");
        return false;
    }

    // Inicializa NetworkManager
    g_pLogger->Write(LogLevel::INFO, "Inicializando Network Manager...");
    NetworkConfig netConfig;
    netConfig.address = config.GetServerBindIP();
    netConfig.port = config.GetServerPort();
    netConfig.maxConnections = config.GetMaxConnections();

    if (!NetworkManager::getInstance().initialize(netConfig)) {
        g_pLogger->Write(LogLevel::ERROR, "Falha ao inicializar Network Manager");
        return false;
    }

    // Registra handlers de /**
 * RegisterPacketHandlers
 * 
 * Implementa a funcionalidade RegisterPacketHandlers conforme especificação original.
 * @return Retorna pacotes
 */
 pacotes
    RegisterPacketHandlers();

    // Registra handlers de /**
 * signal
 * 
 * Implementa a funcionalidade signal conforme especificação original.
 * @param SIGINT Parâmetro SIGINT
 * @param SignalHandler Parâmetro SignalHandler
 * @return Retorna sinais
 */
 sinais
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    // Servidor inicializado com sucesso
    g_dwServerStartTime = static_cast<DWORD>(std::time(nullptr));
    g_bRunning = true;

    g_pLogger->Write(LogLevel::INFO, "Trade Market Server inicializado com sucesso");
    return true;
}

/**
 * @brief Atualiza o estado do servidor
 * @param deltaTime Tempo desde a última atualização em milissegundos
 */
void UpdateServer(DWORD deltaTime) {
    // Atualiza gerenciadores
    g_pWorldManager->Update(deltaTime);
    g_pNPCManager->Update(deltaTime);
    g_pPlayerManager->Update(deltaTime);

    // Processa pacotes pendentes
    NetworkManager::getInstance().processPackets();

    // A cada 60 segundos, registra estatísticas no log
    static DWORD statsTimer = 0;
    statsTimer += deltaTime;
    if (statsTimer >= 60000) {
        statsTimer = 0;
        
        g_pLogger->Write(LogLevel::INFO, "Estatísticas: Conexões=%d, Jogadores=%d, NPCs=%d, Pacotes RX/TX=%d/%d",
            g_dwCurrentConnections.load(), g_pPlayerManager->GetPlayerCount(),
            g_pNPCManager->GetNPCCount(), g_dwTotalPacketsReceived.load(), g_dwTotalPacketsSent.load());
    }
}

/**
 * @brief Finaliza o servidor
 */
void ShutdownServer() {
    g_pLogger->Write(LogLevel::INFO, "Finalizando Trade Market Server...");
    g_bRunning = false;

    // Finaliza gerenciadores em ordem inversa à inicialização
    g_pLogger->Write(LogLevel::INFO, "Finalizando Network Manager...");
    NetworkManager::getInstance().shutdown();

    g_pLogger->Write(LogLevel::INFO, "Finalizando Packet Handler Manager...");
    g_pPacketHandlerManager->Shutdown();

    g_pLogger->Write(LogLevel::INFO, "Finalizando Player Manager...");
    g_pPlayerManager->Shutdown();

    g_pLogger->Write(LogLevel::INFO, "Finalizando NPC Manager...");
    g_pNPCManager->Shutdown();

    g_pLogger->Write(LogLevel::INFO, "Finalizando World Manager...");
    g_pWorldManager->Shutdown();

    g_pLogger->Write(LogLevel::INFO, "Finalizando Database Manager...");
    g_pDatabaseManager->Shutdown();

    g_pLogger->Write(LogLevel::INFO, "Trade Market Server finalizado");
}

/**
 * @brief Loop principal do servidor
 */
void ServerLoop() {
    g_pLogger->Write(LogLevel::INFO, "Iniciando loop principal do servidor...");

    const DWORD TARGET_FRAME_TIME = 50; // 20 FPS (50ms por frame)
    DWORD lastTime = GetTickCount();
    DWORD currentTime = 0;
    DWORD deltaTime = 0;

    // Loop /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param !g_bShutdown Parâmetro !g_bShutdown
 * @return Retorna principal
 */
 principal
    while(g_bRunning && !g_bShutdown){
        currentTime = GetTickCount();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Atualiza o /**
 * UpdateServer
 * 
 * Implementa a funcionalidade UpdateServer conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 * @return Retorna servidor
 */
 servidor
        UpdateServer(deltaTime);

        // Controle de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param TARGET_FRAME_TIME Parâmetro TARGET_FRAME_TIME
 * @return Retorna FPS
 */
 FPS
        if(deltaTime < TARGET_FRAME_TIME){
            std::this_thread::sleep_for(std::chrono::milliseconds(TARGET_FRAME_TIME - deltaTime));
        }
    }

    // Finaliza o /**
 * ShutdownServer
 * 
 * Implementa a funcionalidade ShutdownServer conforme especificação original.
 * @return Retorna servidor
 */
 servidor
    ShutdownServer();
}

/**
 * @brief Função principal
 * @param argc Número de argumentos da linha de comando
 * @param argv Argumentos da linha de comando
 * @return Código de saída
 */
int main(int argc, char* argv[]) {
    std::string configFile = "config/tmsrv.ini";

    // Processa argumentos de linha de /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna comando
 */
 comando
    for(int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            configFile = argv[i + 1];
            i++;
        }
    }

    // Inicializa o servidor
    if (!InitServer(configFile)) {
        std::cerr << "Falha ao inicializar o servidor. Verifique os logs para mais detalhes." << std::endl;
        return 1;
    }

    // Inicia o loop /**
 * ServerLoop
 * 
 * Implementa a funcionalidade ServerLoop conforme especificação original.
 * @return Retorna principal
 */
 principal
    ServerLoop();

    return 0;
}

} // namespace tmsrv
} // namespace server
} // namespace wyd

/**
 * @brief Ponto de entrada do programa
 */
int main(int argc, char* argv[]) {

} // namespace wydbr

    return wyd::server::tmsrv::main(argc, argv);
}