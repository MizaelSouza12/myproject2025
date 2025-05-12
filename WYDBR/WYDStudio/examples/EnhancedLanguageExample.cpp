#include "../include/NaturalLanguageProcessor.h"
#include "../include/LanguageTermsLoader.h"
#include "../include/AdvancedAdminAgent.h"
#include "../include/WYDSpecificKnowledge.h"
#include "../include/GameKnowledgeBase.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace WYDStudio;

// Função para exibir progresso
void DisplayProgress(float progress, const std::string& status) {
    const int barWidth = 50;
    int pos = barWidth * progress;
    
    std::cout << "\r" << status << " [";
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

// Função para exibir logs
void LogMessage(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    
    std::cout << "[" << std::put_time(std::localtime(&nowTime), "%H:%M:%S") << "] " 
              << message << std::endl;
}

// Função para testar o reconhecimento dos novos termos e gírias
void TestNewLanguageTerms(std::shared_ptr<NaturalLanguageProcessor> nlProcessor) {
    std::cout << "\n===== TESTANDO NOVOS TERMOS E GÍRIAS =====\n" << std::endl;
    
    // Lista de exemplos para testar com os novos termos
    std::vector<std::string> examples = {
        "Quanto custa um set E4 de FM no servidor?",
        "Onde consigo FoF e FoW para evolução?",
        "Quero trocar 200 tokens por um set celestial",
        "Como configurar AutoBuff e AutoHP para tank?",
        "Quando será o próximo boss time?",
        "Preciso de ajuda para tankar solo o boss",
        "Alguém sabe se o evento está ON ou OFF?",
        "Esse jogador é muito zika do PvP",
        "Como funciona o sistema anti-KS desse servidor?",
        "Vou telar para a área VIP e fazer farm",
        "Estou travando muito nessa dungeon, PC lento",
        "Ele usou modo BRK no X1 e me destruiu",
        "Pode me ajudar a upar? Sou noob ainda",
        "Procurando guild para participar do BC",
        "Alguém aí tem macro para farm automático?",
        "Como faço para debuffar no PvP?",
        "Tem muito bot upando dormindo nesse spot"
    };
    
    for (const auto& example : examples) {
        std::cout << "\nEntrada: " << example << std::endl;
        
        // Normalizar o texto (identificar termos específicos)
        std::string normalized = nlProcessor->NormalizeText(example);
        std::cout << "Texto normalizado: " << normalized << std::endl;
        
        // Processar a entrada para identificar intenção
        RecognizedIntent intent = nlProcessor->ProcessText(example);
        
        std::cout << "Intenção: " << intent.intentName 
                  << " (confiança: " << intent.confidence << ")" << std::endl;
        
        // Mostrar termos reconhecidos
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
        
        // Mostrar parâmetros extraídos
        if (!intent.params.empty()) {
            std::cout << "Parâmetros: ";
            for (const auto& [key, value] : intent.params) {
                std::cout << key << "='" << value << "' ";
            }
            std::cout << std::endl;
        }
        
        std::cout << "---------------------------------------------" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

// Função para testar processamento de comandos com os novos termos
void TestCommandProcessing(std::shared_ptr<AdvancedAdminAgent> agent) {
    std::cout << "\n===== TESTANDO PROCESSAMENTO DE COMANDOS COM NOVOS TERMOS =====\n" << std::endl;
    
    // Lista de comandos para testar
    std::vector<std::string> commands = {
        "Cria um set E4 +10 para jogador TestUser",
        "Inicia evento ON de drop de FoF em área VIP por 2 horas",
        "Banir o bot que está upando dormindo no spot de trolls",
        "Teleporta o zika do PvP para a área de torneio",
        "Anuncia que o boss time será às 21h hoje",
        "Verifica se tem algum player usando macro no mapa donate",
        "Ajuda players noobs na dungeon iniciante",
        "Ativa anti-KS em todas as áreas PvP",
        "Aumenta a taxa de drop para 3x durante o BC de hoje"
    };
    
    for (const auto& command : commands) {
        std::cout << "\nComando: " << command << std::endl;
        
        // Processar o comando usando o agente
        std::string response = agent->ProcessQuery(command);
        
        std::cout << "Resposta: " << response << std::endl;
        std::cout << "---------------------------------------------" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    try {
        // Banner
        std::cout << R"(
===============================================================
    WYD MODERN 2025 STUDIO - Exemplo de Linguagem Aprimorada
===============================================================
Este exemplo demonstra o processamento avançado de gírias e termos
específicos dos jogadores de WYD, permitindo reconhecimento e 
interpretação de linguagem natural com vocabulário expandido.
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
            DisplayProgress(progress, status);
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
        
        // 7. Criar carregador de termos
        auto termsLoader = std::make_shared<LanguageTermsLoader>();
        
        // 8. Carregar termos expandidos
        std::cout << "Carregando termos expandidos de linguagem...\n" << std::endl;
        if (!termsLoader->LoadTermsToProcessor(
            nlProcessor, 
            "./data/expanded_language_terms.json", 
            DisplayProgress)) {
            std::cerr << "Aviso: Falha ao carregar alguns termos expandidos." << std::endl;
        }
        std::cout << std::endl;
        
        // 9. Criar agente administrador
        auto adminAgent = std::make_shared<AdvancedAdminAgent>(nlProcessor, wydKnowledge);
        adminAgent->SetLogCallback(LogMessage);
        
        // 10. Inicializar agente
        if (!adminAgent->Initialize("")) {
            std::cerr << "Erro ao inicializar agente administrador" << std::endl;
            return 1;
        }
        
        // 11. Registrar handlers de intenção básicos para demonstração
        nlProcessor->RegisterIntentHandler({
            "query",
            [&wydKnowledge](const RecognizedIntent& intent) -> std::string {
                // Simplesmente encaminhar para a base de conhecimento
                return wydKnowledge->AnswerGameQuestion(intent.originalText);
            },
            false, // Não requer parâmetros
            false  // Não é somente admin
        });
        
        // 12. Testar reconhecimento dos novos termos
        TestNewLanguageTerms(nlProcessor);
        
        // 13. Testar processamento de comandos
        TestCommandProcessing(adminAgent);
        
        std::cout << "\n===== Exemplo concluído com sucesso =====\n" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }
}