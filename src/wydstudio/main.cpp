
/**
 * @file main.cpp
 * @brief Ponto de entrada para o WYDStudio com TMSrv/DBSrv integrados
 */

#include "../../include/WYDStudio/WYDStudio.h"
#include "../../include/Common/Version.h"
#include <iostream>
#include <string>
#include <csignal>

// Instância global do WYDStudio
std::unique_ptr<WYDBR::WYDStudio> g_wydstudio;

// Variável para controle de saída limpa
bool g_running = true;

// Handler de sinal
void signal_handler(int signal) {
    std::cout << "Sinal recebido: " << signal << ", preparando para sair..." << std::endl;
    g_running = false;
    
    // Encerrar WYDStudio de forma limpa
    if (g_wydstudio) {
        g_wydstudio->Shutdown();
    }
}

// Função para exibir ajuda
void print_help() {
    std::cout << "Uso: WYDStudio [opções]" << std::endl;
    std::cout << "Opções:" << std::endl;
    std::cout << "  --config CAMINHO  Especifica o caminho para o arquivo de configuração" << std::endl;
    std::cout << "  --help            Exibe esta ajuda" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "==============================================================" << std::endl;
    std::cout << "                   WYDBR 2.0 - WYDStudio                      " << std::endl;
    std::cout << "         Versão: " << WYDBR::WYDBR_VERSION_MAJOR << "." 
              << WYDBR::WYDBR_VERSION_MINOR << "." << WYDBR::WYDBR_VERSION_PATCH << std::endl;
    std::cout << "==============================================================" << std::endl;
    
    // Configurar handler de sinal
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // Processar argumentos de linha de comando
    std::string configPath = "";
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--config" && i + 1 < argc) {
            configPath = argv[++i];
        }
        else if (arg == "--help") {
            print_help();
            return 0;
        }
    }
    
    try {
        // Criar e inicializar WYDStudio
        g_wydstudio = std::make_unique<WYDBR::WYDStudio>();
        
        if (!g_wydstudio->Initialize(configPath)) {
            std::cerr << "Falha ao inicializar WYDStudio." << std::endl;
            return 1;
        }
        
        // Iniciar WYDStudio
        if (!g_wydstudio->Start()) {
            std::cerr << "Falha ao iniciar WYDStudio." << std::endl;
            return 1;
        }
        
        // Executar WYDStudio (não retorna até que o WYDStudio seja encerrado)
        g_wydstudio->Run();
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Erro fatal desconhecido." << std::endl;
        return 1;
    }
}
