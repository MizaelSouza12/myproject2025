#include "../include/WYDStudioApp.h"
#include "../include/AgentTrainer.h"
#include "../include/GameKnowledgeBase.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <functional>

using namespace WYDStudio;

// Callback para progresso do treinamento
void TrainingProgressCallback(const TrainingProgress& progress) {
    std::cout << "[TREINAMENTO] " 
              << progress.currentStage << ": " 
              << progress.currentDescription
              << " (" << (progress.overallProgress * 100.0f) << "%)" 
              << std::endl;
}

// Callback para log geral
void LogCallback(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
}

/**
 * @brief Ponto de entrada principal para o aplicativo WYDStudio
 * 
 * Este arquivo contém a função main() que inicializa o aplicativo
 * WYDStudio e gerencia o ciclo de vida principal do programa.
 */
int main(int argc, char* argv[]) {
    try {
        std::cout << "Iniciando WYDStudio..." << std::endl;
        
        // Caminho de configuração padrão
        std::string configPath = "";
        bool startTrainingImmediately = false;
        
        // Processar argumentos de linha de comando
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--config" && i + 1 < argc) {
                configPath = argv[++i];
            }
            else if (arg == "--train") {
                startTrainingImmediately = true;
            }
            else if (arg == "--help" || arg == "-h") {
                // Exibir ajuda
                std::cout << "WYDStudio - WYD Game Management and Administration Tool" << std::endl;
                std::cout << "Usage: WYDStudio [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --config <path>  Specify configuration file path" << std::endl;
                std::cout << "  --train          Immediately start agent training" << std::endl;
                std::cout << "  --help, -h       Display this help message" << std::endl;
                return 0;
            }
        }
        
        // Criar e inicializar a base de conhecimento
        auto knowledgeBase = std::make_shared<GameKnowledgeBase>(nullptr); // FileSystemConnector será definido depois
        knowledgeBase->SetLogCallback(LogCallback);
        
        // Criar instância do aplicativo
        auto app = new WYDStudioApp();
        
        // Inicializar o aplicativo
        if (!app->Initialize(configPath)) {
            std::cerr << "Erro: Falha ao inicializar WYDStudio." << std::endl;
            delete app;
            return 1;
        }
        
        // Agora podemos obter o FileSystemConnector e inicializar a base de conhecimento
        auto fsConnector = app->GetFileSystemConnector(); // Este método precisa ser adicionado
        knowledgeBase->Initialize("./WYD", "./src", "");
        
        // Criar e inicializar o treinador de agentes
        auto agentTrainer = std::make_shared<AgentTrainer>(knowledgeBase);
        agentTrainer->SetLogCallback(LogCallback);
        agentTrainer->Initialize("./WYD", "./src", "./models");
        
        // Registrar agentes para treinamento
        auto adminAgent = app->GetAgent(AgentType::ADMIN); // Este método precisa ser adicionado
        auto gmAgent = app->GetAgent(AgentType::GM);
        auto devAgent = app->GetAgent(AgentType::DEV);
        
        if (adminAgent) agentTrainer->RegisterAgent(adminAgent, AgentType::ADMIN);
        if (gmAgent) agentTrainer->RegisterAgent(gmAgent, AgentType::GM);
        if (devAgent) agentTrainer->RegisterAgent(devAgent, AgentType::DEV);
        
        // Se solicitado, iniciar treinamento imediatamente
        if (startTrainingImmediately) {
            std::cout << "Iniciando treinamento de agentes..." << std::endl;
            
            // Iniciar treinamento em thread separada
            auto futureResult = agentTrainer->StartTraining(TrainingMode::FULL, TrainingProgressCallback);
            
            // Exibir mensagem de aguardando
            std::cout << "Treinamento iniciado em segundo plano. A aplicação continuará normalmente." << std::endl;
            std::cout << "Você pode cancelar o treinamento a qualquer momento usando o menu." << std::endl;
        }
        
        // Executar o loop principal
        app->Run();
        
        // Finalizar o aplicativo
        app->Shutdown();
        delete app;
        
        std::cout << "WYDStudio encerrado com sucesso." << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Erro desconhecido." << std::endl;
        return 1;
    }
}