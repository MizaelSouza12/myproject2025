/**
 * main.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/main.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include <iostream>
#include <string>
#include <csignal>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "../../core/Logger.h"
#include "../../core/Utils.h"
#include "../../core/WYDTypes.h"
#include "TMServer.h"
#include "TMConfig.h"

namespace wydbr {


/**
 * @file main.cpp
 * @brief Ponto de entrada para o TMServer (Trade & Merchant Server)
 * 
 * Este arquivo contém a função main() que inicializa o servidor TM.
 * Ele segue a estrutura original do WYD, mas com melhorias significativas
 * para facilitar a manutenção e adicionar novos recursos.
 */

using namespace wyd;
using namespace wyd::server;

// Flag de controle para término do programa
static bool g_Running = true;

/**
 * @brief Handler para sinais do sistema (SIGINT, SIGTERM)
 * @param signal Sinal recebido
 */
void SignalHandler(int signal) {
    LOG_INFO("Sinal recebido: {}", signal);
    
    switch (signal) {
        case SIGINT:
        case SIGTERM:
            g_Running = false;
            break;
    }
}

/**
 * @brief Inicializa os manipuladores de sinal
 */
void InitializeSignalHandlers() {
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
}

/**
 * @brief Exibe informações sobre o servidor
 */
void PrintServerInfo() {
    std::cout << "====================================" << std::endl;
    std::cout << "    WYDBRASIL - TMServer (WYD 2.0)    " << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Versão: 1.0.0" << std::endl;
    
    char timeBuffer[64];
    time_t now = time(nullptr);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localtime_r(&now));
    
    std::cout << "Data de Início: " << timeBuffer << std::endl;
    std::cout << "Servidor: " << g_Config.szServerName << std::endl;
    std::cout << "Modo: " << g_Config.GetServerModeString() << std::endl;
    std::cout << "Porta: " << g_Config.wTMServerPort << std::endl;
    std::cout << "====================================" << std::endl;
}

/**
 * @brief Exibe informações de uso (help)
 * @param executableName Nome do executável
 */
void PrintUsage(const char* executableName) {
    std::cout << "Uso: " << executableName << " [opções]" << std::endl;
    std::cout << "Opções:" << std::endl;
    std::cout << "  -c, --config <arquivo>   Especifica o arquivo de configuração" << std::endl;
    std::cout << "  -p, --port <porta>       Especifica a porta do servidor" << std::endl;
    std::cout << "  -n, --name <nome>        Especifica o nome do servidor" << std::endl;
    std::cout << "  -m, --mode <modo>        Especifica o modo do servidor (0-4)" << std::endl;
    std::cout << "  -d, --debug              Habilita modo de debug" << std::endl;
    std::cout << "  -h, --help               Exibe esta ajuda" << std::endl;
}

/**
 * @brief Processa argumentos de linha de comando
 * @param argc Número de argumentos
 * @param argv Vetor de argumentos
 * @param configFile Caminho para o arquivo de configuração
 * @return true se os argumentos foram processados com sucesso, false para sair
 */
bool ProcessCommandLineArgs(int argc, char* argv[], std::string& configFile) {
    configFile = "TMServer.conf"; // Valor padrã/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna o
 */
o
    
    for(int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
            else {
                std::cerr << "Erro: Arquivo de configuração não especificado" << std::endl;
                PrintUsage(argv[0]);
                return false;
            }
        }
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            if (i + 1 < argc) {
                g_Config.wTMServerPort = static_cast<WORD>(std::strtol(argv[++i]));
            }
            else {
                std::cerr << "Erro: Porta não especificada" << std::endl;
                PrintUsage(argv[0]);
                return false;
            }
        }
        else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0) {
            if (i + 1 < argc) {
                strncpy(g_Config.szServerName, argv[++i], sizeof(g_Config.szServerName) - 1);
                g_Config.szServerName[sizeof(g_Config.szServerName) - 1] = '\0';
            }
            else {
                std::cerr << "Erro: Nome do servidor não especificado" << std::endl;
                PrintUsage(argv[0]);
                return false;
            }
        }
        else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mode") == 0) {
            if (i + 1 < argc) {
                int mode = std::strtol(argv[++i]);
                if (mode >= 0 && mode <= 4) {
                    g_Config.byServerMode = static_cast<BYTE>(mode);
                }
                else {
                    std::cerr << "Erro: Modo inválido (deve ser 0-4)" << std::endl;
                    PrintUsage(argv[0]);
                    return false;
                }
            }
            else {
                std::cerr << "Erro: Modo não especificado" << std::endl;
                PrintUsage(argv[0]);
                return false;
            }
        }
        else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            g_Config.byServerMode = SERVER_MODE_DEBUG;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            PrintUsage(argv[0]);
            return false;
        }
        else {
            std::cerr << "Erro: Opção desconhecida: " << argv[i] << std::endl;
            PrintUsage(argv[0]);
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Função principal (ponto de entrada do programa)
 * @param argc Número de argumentos
 * @param argv Vetor de argumentos
 * @return Código de retorno (0 para sucesso)
 */
int main(int argc, char* argv[]) {

} // namespace wydbr

    // Inicializa /**
 * InitializeLogger
 * 
 * Implementa a funcionalidade InitializeLogger conforme especificação original.
 * @param "TMServer.log" Parâmetro "TMServer.log"
 * @return Retorna logger
 */
 logger
    InitializeLogger("TMServer.log");
    LOG_INFO("TMServer iniciando...");
    
    // Inicializa tratadores de /**
 * InitializeSignalHandlers
 * 
 * Implementa a funcionalidade InitializeSignalHandlers conforme especificação original.
 * @return Retorna sinal
 */
 sinal
    InitializeSignalHandlers();
    
    // Processa argumentos de linha de comando
    std::string configFile;
    if (!ProcessCommandLineArgs(argc, argv, configFile)) {
        return 1;
    }
    
    // Inicializa o servidor
    CTMServer& server = CTMServer::GetInstance();
    if (!server.Initialize(configFile.c_str())) {
        LOG_ERROR("Falha ao inicializar o servidor");
        return 1;
    }
    
    // Exibe informações do /**
 * PrintServerInfo
 * 
 * Implementa a funcionalidade PrintServerInfo conforme especificação original.
 * @return Retorna servidor
 */
 servidor
    PrintServerInfo();
    
    // Inicia o servidor
    if (!server.Start()) {
        LOG_ERROR("Falha ao iniciar o servidor");
        return 1;
    }
    
    LOG_INFO("TMServer iniciado com sucesso");
    LOG_INFO("Pressione Ctrl+C para sair");
    
    // Loop /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param g_Running Parâmetro g_Running
 * @return Retorna principal
 */
 principal
    while(g_Running){
        // Dorme por curto período
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Verifica se o servidor ainda está rodando
        if (!server.IsRunning()) {
            LOG_WARNING("Servidor parou inesperadamente");
            g_Running = false;
        }
    }
    
    // Para o /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param TMServer..." Parâmetro TMServer..."
 * @return Retorna servidor
 */
 servidor
    LOG_INFO("Parando TMServer...");
    server.Stop();
    
    LOG_INFO("TMServer finalizado");
    return 0;
}