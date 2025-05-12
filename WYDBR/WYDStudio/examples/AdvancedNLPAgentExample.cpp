#include "../include/NaturalLanguageProcessor.h"
#include "../include/AdvancedAdminAgent.h"
#include "../include/WYDSpecificKnowledge.h"
#include "../include/GameKnowledgeBase.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace WYDStudio;

// Função para exibir progresso
void DisplayProgress(const std::string& operation, float progress) {
    const int barWidth = 50;
    int pos = barWidth * progress;
    
    std::cout << "\r" << operation << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "% ";
    std::cout.flush();
    
    if (progress >= 1.0) {
        std::cout << std::endl;
    }
}

// Função para processar logs
void LogMessage(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream logStream;
    logStream << "[" << std::put_time(std::localtime(&nowTime), "%H:%M:%S") << "] " 
              << message;
    
    std::cout << logStream.str() << std::endl;
    
    // Opcional: salvar em arquivo de log
    std::ofstream logFile("nlp_agent_example.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << logStream.str() << std::endl;
        logFile.close();
    }
}

// Função para realizar um teste interativo com o agente
void RunInteractiveTest(
    std::shared_ptr<AdvancedAdminAgent> agent,
    std::shared_ptr<NaturalLanguageProcessor> nlProcessor) {
    
    std::cout << "\n===== INICIANDO TESTE INTERATIVO =====\n" << std::endl;
    std::cout << "Digite comandos em linguagem natural para interagir com o agente." << std::endl;
    std::cout << "Digite 'sair' para encerrar o teste." << std::endl;
    std::cout << "Digite 'ajuda' para obter informações sobre comandos disponíveis." << std::endl;
    std::cout << std::endl;
    
    std::string input;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        
        if (input == "sair" || input == "exit" || input == "quit") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        // Processar o comando
        std::string response = agent->ProcessQuery(input);
        
        // Mostrar a resposta
        std::cout << "\nResposta do Agente:\n" << response << std::endl;
        
        // Mostrar sugestões contextuais
        std::vector<std::string> suggestions = 
            nlProcessor->GenerateContextualSuggestions("interactive_test", input, 3);
        
        if (!suggestions.empty()) {
            std::cout << "\nSugestões:\n";
            for (const auto& suggestion : suggestions) {
                std::cout << "- " << suggestion << std::endl;
            }
        }
    }
    
    std::cout << "\n===== TESTE INTERATIVO ENCERRADO =====\n" << std::endl;
}

// Função para demonstrar reconhecimento de termos e padrões
void DemonstrateNLPFeatures(std::shared_ptr<NaturalLanguageProcessor> nlProcessor) {
    std::cout << "\n===== DEMONSTRAÇÃO DE RECURSOS DE NLP =====\n" << std::endl;
    
    // Exemplos de termos específicos
    std::vector<std::string> termExamples = {
        "Quero comprar um set A para meu FM",
        "Onde posso encontrar o NPC AKI?",
        "Quanto custa uma Armadura Celestial +10?",
        "Como faço para teleportar para Noatun?",
        "Qual a melhor build para BloodMaster?"
    };
    
    // Exemplos de comandos administrativos
    std::vector<std::string> commandExamples = {
        "Cria um set D +11 de FM para o jogador TestUser",
        "Inicia um evento de drop de poeira de lac em trolls",
        "Bane o jogador Hacker123 por uso de programa ilegal",
        "Anuncia que teremos manutenção às 18h hoje",
        "Verifica o status do servidor"
    };
    
    std::cout << "==== Processamento de Termos Específicos ====\n" << std::endl;
    
    for (const auto& example : termExamples) {
        std::cout << "Input: " << example << std::endl;
        
        // Normalizar texto
        std::string normalized = nlProcessor->NormalizeText(example);
        std::cout << "Normalizado: " << normalized << std::endl;
        
        // Processar texto
        RecognizedIntent intent = nlProcessor->ProcessText(example);
        
        std::cout << "Intenção: " << intent.intentName 
                  << " (confiança: " << intent.confidence << ")" << std::endl;
        
        if (!intent.recognizedTerms.empty()) {
            std::cout << "Termos reconhecidos: ";
            for (size_t i = 0; i < intent.recognizedTerms.size(); ++i) {
                std::cout << intent.recognizedTerms[i];
                if (i < intent.recognizedTerms.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << std::endl;
        }
        
        if (!intent.params.empty()) {
            std::cout << "Parâmetros extraídos:";
            for (const auto& [key, value] : intent.params) {
                std::cout << " " << key << "='" << value << "'";
            }
            std::cout << std::endl;
        }
        
        std::cout << "---------------------------------------------" << std::endl;
    }
    
    std::cout << "\n==== Processamento de Comandos Administrativos ====\n" << std::endl;
    
    for (const auto& example : commandExamples) {
        std::cout << "Input: " << example << std::endl;
        
        // Processar texto
        RecognizedIntent intent = nlProcessor->ProcessText(example);
        
        std::cout << "Intenção: " << intent.intentName 
                  << " (confiança: " << intent.confidence << ")" << std::endl;
        
        if (!intent.params.empty()) {
            std::cout << "Parâmetros extraídos:";
            for (const auto& [key, value] : intent.params) {
                std::cout << " " << key << "='" << value << "'";
            }
            std::cout << std::endl;
        }
        
        std::cout << "---------------------------------------------" << std::endl;
    }
}

// Função para demonstrar memória contextual
void DemonstrateContextualMemory(
    std::shared_ptr<NaturalLanguageProcessor> nlProcessor, 
    std::shared_ptr<AdvancedAdminAgent> agent) {
    
    std::cout << "\n===== DEMONSTRAÇÃO DE MEMÓRIA CONTEXTUAL =====\n" << std::endl;
    
    // Exemplo de conversa com referências contextuais
    const std::string contextId = "demo_context";
    std::vector<std::string> conversation = {
        "Cria um set D +11 de FM para o jogador TestUser",
        "Agora aumenta o refinamento dele para +12",
        "Teleporta o TestUser para Armia",
        "Cria uma arma celestial +10 para ele",
        "Inicia um evento de drop em trolls",
        "Aumenta a taxa de drop para 300%"
    };
    
    for (const auto& input : conversation) {
        std::cout << "\nInput: " << input << std::endl;
        
        // Processar com o agente (que usa NLP internamente)
        std::string response = agent->ProcessQuery(input);
        
        std::cout << "Resposta: " << response << std::endl;
        
        // Demonstrar contexto
        std::vector<NLPMemoryItem> memoryItems = 
            nlProcessor->GetContextMemory(contextId, 1);  // Último item apenas
        
        if (!memoryItems.empty()) {
            const auto& lastMemory = memoryItems[0];
            
            std::cout << "Memória Contextual:" << std::endl;
            std::cout << "- Intenção: " << lastMemory.recognizedIntent.intentName << std::endl;
            
            if (!lastMemory.recognizedIntent.params.empty()) {
                std::cout << "- Parâmetros: ";
                for (const auto& [key, value] : lastMemory.recognizedIntent.params) {
                    std::cout << key << "='" << value << "' ";
                }
                std::cout << std::endl;
            }
        }
        
        std::cout << "---------------------------------------------" << std::endl;
    }
}

// Função principal
int main() {
    try {
        // Banner
        std::cout << R"(
===============================================================
    WYD MODERN 2025 STUDIO - Avançado NLP Agent Exemplo
===============================================================
Este exemplo demonstra o uso do Processador de Linguagem Natural
e do Agente de Administração Avançado do WYD Modern 2025 Studio.
)";

        // Inicializar componentes
        std::cout << "\nInicializando componentes...\n" << std::endl;
        
        // 1. Criar base de conhecimento compartilhada
        auto knowledgeBase = std::make_shared<GameKnowledgeBase>();
        knowledgeBase->SetLogCallback(LogMessage);
        
        // 2. Criar conhecimento específico WYD
        auto wydKnowledge = std::make_shared<WYDSpecificKnowledge>(knowledgeBase);
        wydKnowledge->SetLogCallback(LogMessage);
        
        // 3. Inicializar base de conhecimento
        if (!wydKnowledge->Initialize("../../")) {
            std::cerr << "Erro ao inicializar conhecimento WYD" << std::endl;
            return 1;
        }
        
        // 4. Carregar dados do jogo
        std::cout << "Carregando dados do jogo...\n" << std::endl;
        wydKnowledge->LoadGameData([](float progress, const std::string& status) {
            DisplayProgress("Carregando dados", progress);
        });
        std::cout << std::endl;
        
        // 5. Criar processador de linguagem natural
        auto nlProcessor = std::make_shared<NaturalLanguageProcessor>();
        nlProcessor->SetLogCallback(LogMessage);
        
        // 6. Inicializar NLP
        if (!nlProcessor->Initialize()) {
            std::cerr << "Erro ao inicializar processador de linguagem natural" << std::endl;
            return 1;
        }
        
        // 7. Criar agente administrador
        auto adminAgent = std::make_shared<AdvancedAdminAgent>(nlProcessor, wydKnowledge);
        adminAgent->SetLogCallback(LogMessage);
        adminAgent->SetProgressCallback(DisplayProgress);
        
        // 8. Inicializar agente
        if (!adminAgent->Initialize("")) {
            std::cerr << "Erro ao inicializar agente administrador" << std::endl;
            return 1;
        }
        
        // 9. Registrar handlers de intenção
        nlProcessor->RegisterIntentHandler({
            "create_item",
            [&adminAgent](const RecognizedIntent& intent) -> std::string {
                // Mapear parâmetros da intenção para formato esperado pelo agente
                std::map<std::string, std::string> params;
                params["itemType"] = intent.GetParam("itemType");
                params["refineLevel"] = intent.GetParam("refineLevel");
                params["playerClass"] = intent.GetParam("playerClass");
                
                // Se não tiver nome do jogador, usar um padrão para o exemplo
                params["playerName"] = intent.GetParam("playerName", "TestUser");
                
                // Executar operação através do agente
                return adminAgent->ExecuteOperation("create_game_item", params);
            },
            true,  // Requer parâmetros
            false, // Não é somente admin
            {"itemType"}  // Parâmetros necessários
        });
        
        nlProcessor->RegisterIntentHandler({
            "drop_event",
            [&adminAgent](const RecognizedIntent& intent) -> std::string {
                // Mapear parâmetros da intenção para formato esperado pelo agente
                std::map<std::string, std::string> params;
                params["itemName"] = intent.GetParam("itemName", "item genérico");
                params["targetLocation"] = intent.GetParam("targetLocation", "mundo");
                
                // Parâmetros opcionais
                if (intent.HasParam("duration")) {
                    params["duration"] = intent.GetParam("duration");
                }
                
                if (intent.HasParam("dropRate")) {
                    params["dropRate"] = intent.GetParam("dropRate");
                }
                
                // Executar operação através do agente
                return adminAgent->ExecuteOperation("start_drop_event", params);
            },
            true,  // Requer parâmetros
            false, // Não é somente admin
            {"itemName"}  // Parâmetros necessários
        });
        
        nlProcessor->RegisterIntentHandler({
            "player_management",
            [&adminAgent](const RecognizedIntent& intent) -> std::string {
                // Mapear parâmetros da intenção para formato esperado pelo agente
                std::map<std::string, std::string> params;
                params["action"] = intent.GetParam("action");
                params["playerName"] = intent.GetParam("playerName", "UnknownPlayer");
                
                // Parâmetros opcionais
                if (intent.HasParam("duration")) {
                    params["duration"] = intent.GetParam("duration");
                }
                
                if (intent.HasParam("reason")) {
                    params["reason"] = intent.GetParam("reason");
                }
                
                if (intent.HasParam("targetLocation")) {
                    params["targetLocation"] = intent.GetParam("targetLocation");
                }
                
                // Executar operação através do agente
                return adminAgent->ExecuteOperation("manage_player", params);
            },
            true,  // Requer parâmetros
            false, // Não é somente admin
            {"action", "playerName"}  // Parâmetros necessários
        });
        
        nlProcessor->RegisterIntentHandler({
            "broadcast",
            [&adminAgent](const RecognizedIntent& intent) -> std::string {
                // Mapear parâmetros da intenção para formato esperado pelo agente
                std::map<std::string, std::string> params;
                params["message"] = intent.GetParam("message", "Mensagem do sistema");
                
                // Parâmetros opcionais
                if (intent.HasParam("color")) {
                    params["color"] = intent.GetParam("color");
                }
                
                if (intent.HasParam("repeat")) {
                    params["repeat"] = intent.GetParam("repeat");
                }
                
                // Executar operação através do agente
                return adminAgent->ExecuteOperation("send_broadcast", params);
            },
            true,  // Requer parâmetros
            false, // Não é somente admin
            {"message"}  // Parâmetros necessários
        });
        
        // 10. Demonstrar recursos
        std::cout << "\nComponentes inicializados com sucesso.\n" << std::endl;
        
        // Demonstrar recursos NLP
        DemonstrateNLPFeatures(nlProcessor);
        
        // Demonstrar memória contextual
        DemonstrateContextualMemory(nlProcessor, adminAgent);
        
        // Executar teste interativo
        RunInteractiveTest(adminAgent, nlProcessor);
        
        // Exportar dados de aprendizado
        std::cout << "Exportando dados de aprendizado..." << std::endl;
        adminAgent->ExportLearningData("./admin_agent_learning_data.json");
        
        std::cout << "\n===== Exemplo concluído com sucesso =====\n" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Erro desconhecido" << std::endl;
        return 1;
    }
}