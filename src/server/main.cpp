/**
 * @file main.cpp
 * @brief Ponto de entrada principal para servidor WYDBR 2.0
 * 
 * Este arquivo implementa o ponto de entrada principal (função main) 
 * para o servidor WYDBR 2.0, inicializando todos os subsistemas necessários.
 */

#include "../../include/Network/NetworkManager.h"
#include "../../include/Network/PacketHandler.h"
#include "../../include/Network/CryptoManager.h"
#include "../../include/Security/SecurityManager.h"
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>

// Controle de sinal para encerramento adequado
bool running = true;

// Handler de sinal para encerramento limpo
void signal_handler(int signal) {
    std::cout << "Sinal recebido: " << signal << std::endl;
    running = false;
}

int main(int argc, char* argv[]) {
    std::cout << "==============================================================" << std::endl;
    std::cout << "                   WYDBR 2.0 SERVER                           " << std::endl;
    std::cout << "         Versão: " << WYDBR::WYDBR_VERSION_MAJOR << "." 
              << WYDBR::WYDBR_VERSION_MINOR << "." << WYDBR::WYDBR_VERSION_PATCH << std::endl;
    std::cout << "==============================================================" << std::endl;
    
    // Configurar handler de sinal
    signal(SIGINT, signal_handler);  // Ctrl+C
    signal(SIGTERM, signal_handler); // kill
    
    try {
        // Inicializar gerenciador de segurança
        if (!WYDBR::Security::SecurityManager::GetInstance().Initialize("security_config.json")) {
            std::cerr << "Falha ao inicializar o sistema de segurança." << std::endl;
            return 1;
        }
        std::cout << "Sistema de segurança inicializado com sucesso." << std::endl;
        
        // Inicializar handler de pacotes
        WYDBR::Network::PacketHandler::Initialize();
        std::cout << "Handler de pacotes inicializado." << std::endl;
        
        // Inicializar gerenciador de rede
        uint16_t port = 8281; // Porta padrão
        
        // Verificar se a porta foi especificada via linha de comando
        if (argc > 1) {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        }
        
        if (!WYDBR::Network::NetworkManager::GetInstance().Initialize(port)) {
            std::cerr << "Falha ao inicializar o sistema de rede." << std::endl;
            return 1;
        }
        std::cout << "Sistema de rede inicializado na porta " << port << "." << std::endl;
        
        // Iniciar serviço de rede
        WYDBR::Network::NetworkManager::GetInstance().Start();
        std::cout << "Servidor iniciado e aceitando conexões." << std::endl;
        
        // Loop principal
        std::cout << "Pressione Ctrl+C para encerrar o servidor." << std::endl;
        while (running) {
            // Executar tarefas periódicas, se necessário
            
            // Permitir que outros threads executem
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Encerrar serviço de rede
        std::cout << "Encerrando servidor..." << std::endl;
        WYDBR::Network::NetworkManager::GetInstance().Shutdown();
        
        std::cout << "Servidor encerrado com sucesso." << std::endl;
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