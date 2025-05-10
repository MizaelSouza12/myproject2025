/**
 * @file main.cpp
 * @brief Ponto de entrada principal para cliente WYDBR 2.0
 * 
 * Este arquivo implementa o ponto de entrada principal para o cliente WYDBR 2.0,
 * inicializando os subsistemas necessários e a interface gráfica.
 */

#include "../../headers/wyd_core.h"
#include "../../headers/packet_struct.h"
#include "Client.h"
#include "Graphics/GraphicsEngine.h"
#include "Network/ClientNetwork.h"
#include "UI/UserInterface.h"
#include "Audio/AudioEngine.h"
#include "Input/InputHandler.h"
#include <iostream>
#include <string>
#include <csignal>

// Variável para controle de saída limpa
bool g_running = true;

// Handler de sinal
void signal_handler(int signal) {
    std::cout << "Sinal recebido: " << signal << ", preparando para sair..." << std::endl;
    g_running = false;
}

int main(int argc, char* argv[]) {
    std::cout << "==============================================================" << std::endl;
    std::cout << "                   WYDBR 2.0 CLIENT                           " << std::endl;
    std::cout << "         Versão: " << WYDBR::WYDBR_VERSION_MAJOR << "." 
              << WYDBR::WYDBR_VERSION_MINOR << "." << WYDBR::WYDBR_VERSION_PATCH << std::endl;
    std::cout << "==============================================================" << std::endl;
    
    // Configurar handler de sinal
    std::signal(SIGINT, signal_handler);
    
    try {
        // Parâmetros para a inicialização
        std::string server_address = "127.0.0.1";
        uint16_t server_port = 8281;
        bool fullscreen = false;
        int width = 1024;
        int height = 768;
        
        // Processar argumentos de linha de comando
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "--server" && i + 1 < argc) {
                server_address = argv[++i];
            }
            else if (arg == "--port" && i + 1 < argc) {
                server_port = static_cast<uint16_t>(std::stoi(argv[++i]));
            }
            else if (arg == "--fullscreen") {
                fullscreen = true;
            }
            else if (arg == "--width" && i + 1 < argc) {
                width = std::stoi(argv[++i]);
            }
            else if (arg == "--height" && i + 1 < argc) {
                height = std::stoi(argv[++i]);
            }
            else if (arg == "--help") {
                std::cout << "Uso: WYDClient [opções]" << std::endl;
                std::cout << "Opções:" << std::endl;
                std::cout << "  --server ENDEREÇO   Especifica o endereço do servidor (padrão: 127.0.0.1)" << std::endl;
                std::cout << "  --port PORTA        Especifica a porta do servidor (padrão: 8281)" << std::endl;
                std::cout << "  --fullscreen        Inicia em modo tela cheia" << std::endl;
                std::cout << "  --width LARGURA     Define a largura da janela (padrão: 1024)" << std::endl;
                std::cout << "  --height ALTURA     Define a altura da janela (padrão: 768)" << std::endl;
                std::cout << "  --help              Exibe esta ajuda" << std::endl;
                return 0;
            }
        }
        
        // Criar e inicializar o cliente
        WYDBR::Client client;
        
        if (!client.Initialize(server_address, server_port, width, height, fullscreen)) {
            std::cerr << "Falha ao inicializar o cliente." << std::endl;
            return 1;
        }
        
        std::cout << "Cliente iniciado com sucesso." << std::endl;
        std::cout << "Conectando ao servidor: " << server_address << ":" << server_port << std::endl;
        
        // Loop principal
        while (g_running && client.IsRunning()) {
            client.Update();
            client.Render();
        }
        
        // Cleanup
        client.Shutdown();
        
        std::cout << "Cliente encerrado com sucesso." << std::endl;
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