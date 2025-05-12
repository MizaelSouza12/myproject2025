/**
 * main.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/dbsrv/main.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include <iostream>
#include <string>
#include <csignal>

#include "DBServer.h"
#include "../../core/Logger.h"

namespace wydbr {


/**
 * @file main.cpp
 * @brief Ponto de entrada principal para o DBServer
 * 
 * Este arquivo contém a função main que inicia o DBServer e gerencia
 * os sinais de sistema.
 */

// Flag global para controle de shutdown
bool g_shutdownRequested = false;

// Handler de sinais
/**
 * signalHandler
 * 
 * Implementa a funcionalidade signalHandler conforme especificação original.
 * @param signal Parâmetro signal
 */

void signalHandler(int signal){
    wyd::core::Logger::Info("Sinal {} recebido, iniciando shutdown...", signal);
    g_shutdownRequested = true;
}

// Função para imprimir informações de uso
/**
 * printUsage
 * 
 * Implementa a funcionalidade printUsage conforme especificação original.
 * @param programName Parâmetro programName
 */

void printUsage(const char* programName){
    std::cout << "Uso: " << programName << " [opções]" << std::endl;
    std::cout << "Opções:" << std::endl;
    std::cout << "  -h, --help               Mostra esta ajuda" << std::endl;
    std::cout << "  -c, --config <arquivo>   Carrega configuração de arquivo" << std::endl;
    std::cout << "  -p, --port <porta>       Define a porta do servidor (padrão: 8895)" << std::endl;
    std::cout << "  -l, --log <nível>        Define o nível de log (0-5, padrão: 2)" << std::endl;
    std::cout << "  -f, --log-file <arquivo> Define o arquivo de log" << std::endl;
    std::cout << "  -v, --version            Mostra a versão" << std::endl;
}

// Função para imprimir informações de versão
/**
 * printVersion
 * 
 * Implementa a funcionalidade printVersion conforme especificação original.
 */

void printVersion(){
    std::cout << "WYD DBServer v1.0.0" << std::endl;
    std::cout << "Copyright (C) 2025 WYDBRASIL" << std::endl;
}

// Função para imprimir o banner
/**
 * printBanner
 * 
 * Implementa a funcionalidade printBanner conforme especificação original.
 */

void printBanner(){
    std::cout << "=========================================" << std::endl;
    std::cout << "  WYD DBServer - Servidor de Banco de Dados" << std::endl;
    std::cout << "  Versão: 1.0.0" << std::endl;
    std::cout << "  Copyright (C) 2025 WYDBRASIL" << std::endl;
    std::cout << "=========================================" << std::endl;
}

/**
 * main
 * 
 * Implementa a funcionalidade main conforme especificação original.
 * @param argc Parâmetro argc
 * @param argv Parâmetro argv
 * @return Retorna int
 */


int main(int argc, char** argv){

} // namespace wydbr

    // Processa argumentos de linha de comando
    std::string configFile;
    int port = 8895;
    int logLevel = 2;
    std::string logFile;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "--version" Parâmetro "--version"
 * @return Retorna else
 */
 else if(arg == "-v" || arg == "--version"){
            printVersion();
            return 0;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "--config" Parâmetro "--config"
 * @return Retorna else
 */
 else if(arg == "-c" || arg == "--config"){
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Erro: Opção " << arg << " requer um argumento." << std::endl;
                return 1;
            }
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "--port" Parâmetro "--port"
 * @return Retorna else
 */
 else if(arg == "-p" || arg == "--port"){
            if (i + 1 < argc) {
                port = std::stoi(argv[++i]);
            } else {
                std::cerr << "Erro: Opção " << arg << " requer um argumento." << std::endl;
                return 1;
            }
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "--log" Parâmetro "--log"
 * @return Retorna else
 */
 else if(arg == "-l" || arg == "--log"){
            if (i + 1 < argc) {
                logLevel = std::stoi(argv[++i]);
            } else {
                std::cerr << "Erro: Opção " << arg << " requer um argumento." << std::endl;
                return 1;
            }
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "--log-file" Parâmetro "--log-file"
 * @return Retorna else
 */
 else if(arg == "-f" || arg == "--log-file"){
            if (i + 1 < argc) {
                logFile = argv[++i];
            } else {
                std::cerr << "Erro: Opção " << arg << " requer um argumento." << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Erro: Opção desconhecida: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Registra os handlers de sinais
    std::signal(SIGINT, signalHandler);  // Ctrl+C
    std::signal(SIGTERM, signalHandler); // kill
    
    // Imprime o /**
 * printBanner
 * 
 * Implementa a funcionalidade printBanner conforme especificação original.
 * @return Retorna banner
 */
 banner
    printBanner();
    
    // Configura o logger
    wyd::core::Logger::SetLogLevel(static_cast<wyd::core::LogLevel>(logLevel));
    if (!logFile.empty()) {
        wyd::core::Logger::SetLogFile(logFile);
    }
    
    // Cria a configuração do servidor
    wyd::server::DBServerConfig config;
    config.bindPort = port;
    config.logLevel = logLevel;
    if (!logFile.empty()) {
        config.logFile = logFile;
    }
    
    // Carrega a configuração de arquivo, se especificado
    if (!configFile.empty()) {
        if (!wyd::server::CDBServer::GetInstance().LoadConfig(configFile)) {
            std::cerr << "Erro: Falha ao carregar configuração de " << configFile << std::endl;
            return 1;
        }
    } else {
        // Configuração padrão
        config.dbConnectionString = "mysql://user:password@localhost:3306/wyd";
        config.dbMaxConnections = 10;
        config.dbAutoReconnect = true;
        config.cacheSize = 128;
        config.cacheSyncInterval = 60000;
        config.bindIp = "0.0.0.0";
        config.maxClients = 100;
        config.numPacketWorkers = 4;
    }
    
    // Inicializa o servidor
    if (!wyd::server::CDBServer::GetInstance().Initialize(config)) {
        std::cerr << "Erro: Falha ao inicializar o servidor" << std::endl;
        return 1;
    }
    
    // Registra callback para eventos
    wyd::server::CDBServer::GetInstance().RegisterEventCallback(
        [](wyd::server::DB_EVENT eventType, DWORD param, const char* message) {
            std::cout << "Evento: " << eventType << " - " << message << " (Param: " << param << ")" << std::endl;
        }
    );
    
    // Inicia o servidor
    if (!wyd::server::CDBServer::GetInstance().Start()) {
        std::cerr << "Erro: Falha ao iniciar o servidor" << std::endl;
        return 1;
    }
    
    std::cout << "Servidor iniciado na porta " << port << std::endl;
    
    // Loop /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param !g_shutdownRequested Parâmetro !g_shutdownRequested
 * @return Retorna principal
 */
 principal
    while(!g_shutdownRequested){
        // Dorme um pouco
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    std::cout << "Desligando servidor..." << std::endl;
    
    // Para o servidor
    wyd::server::CDBServer::GetInstance().Stop();
    
    // Finaliza o servidor
    wyd::server::CDBServer::GetInstance().Shutdown();
    
    std::cout << "Servidor desligado" << std::endl;
    
    return 0;
}