#include "../include/BaseAgent.h"
#include "../include/GameKnowledgeBase.h"
#include "../include/WYDSpecificKnowledge.h"
#include "../include/AgentTrainer.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>

using namespace WYDStudio;

// Agente com capacidade de responder perguntas sobre o jogo
class WYDKnowledgeAgent : public BaseAgent {
public:
    WYDKnowledgeAgent() {
        // Adicionar capacidade de conhecimento de jogo
        m_capabilities.push_back(AgentCapability::GAME_KNOWLEDGE);
    }
    
    bool Initialize(const std::string& modelPath) override {
        std::cout << "Inicializando WYDKnowledgeAgent..." << std::endl;
        
        // Em uma implementação real, aqui carregaria o modelo de IA
        // Como é um exemplo, vamos considerar que a inicialização foi bem-sucedida
        
        m_initialized = true;
        std::cout << "WYDKnowledgeAgent inicializado com sucesso" << std::endl;
        return true;
    }
    
    void SetGameKnowledge(std::shared_ptr<WYDSpecificKnowledge> gameKnowledge) {
        m_gameKnowledge = gameKnowledge;
    }
    
    std::string ProcessQuery(const std::string& query) override {
        if (!m_initialized) {
            return "Agente não inicializado";
        }
        
        if (!m_gameKnowledge) {
            return "Conhecimento de jogo não configurado";
        }
        
        // Usar o sistema de conhecimento para responder à pergunta
        return m_gameKnowledge->AnswerGameQuestion(query);
    }
    
    std::string ExecuteAction(const std::string& actionName, 
                           const std::map<std::string, std::string>& parameters) override {
        return "WYDKnowledgeAgent executou: " + actionName;
    }
    
    AgentType GetAgentType() const override {
        return AgentType::QUEST_HELPER;
    }
    
    std::vector<AgentCapability> GetCapabilities() const override {
        return m_capabilities;
    }
    
private:
    std::shared_ptr<WYDSpecificKnowledge> m_gameKnowledge;
};

// Função para exibir progresso de treinamento
void DisplayTrainingProgress(float progress, const std::string& status) {
    const int barWidth = 70;
    int pos = barWidth * progress;
    
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "% " << status << "\r";
    std::cout.flush();
    
    if (progress >= 1.0) {
        std::cout << std::endl;
    }
}

// Função para registrar mensagens de log
void LogTrainingMessage(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
}

// Função para testar o agente com uma série de perguntas
void TestAgentWithQuestions(std::shared_ptr<WYDKnowledgeAgent> agent) {
    std::vector<std::string> questions = {
        "Qual o preço de Potion of Vigor no NPC AKI?",
        "Onde encontro o NPC AKI?",
        "Quais itens o NPC AKI vende?",
        "Quanto custa Town Portal no AKI?",
        "Qual o preço de Small Healing Potion?",
        "Onde posso comprar Medium Mana Potion?",
        "Quais NPCs existem em Armia?",
        "O que é Potion of Wisdom?"
    };
    
    std::cout << "\n===== TESTANDO AGENTE COM PERGUNTAS SOBRE O JOGO =====\n" << std::endl;
    
    for (const auto& question : questions) {
        std::cout << "Pergunta: " << question << std::endl;
        std::string answer = agent->ProcessQuery(question);
        std::cout << "\nResposta:\n" << answer << "\n\n";
        std::cout << "---------------------------------------------\n" << std::endl;
        
        // Pequena pausa para facilitar leitura
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    std::cout << "=== Exemplo de Treinamento de Agente com Conhecimento do Jogo ===\n" << std::endl;
    
    try {
        // 1. Criar base de conhecimento
        auto knowledgeBase = std::make_shared<GameKnowledgeBase>();
        knowledgeBase->SetLogCallback(LogTrainingMessage);
        
        // 2. Criar conhecimento específico do jogo
        auto gameKnowledge = std::make_shared<WYDSpecificKnowledge>(knowledgeBase);
        gameKnowledge->SetLogCallback(LogTrainingMessage);
        
        // 3. Inicializar conhecimento específico com caminho para dados do jogo
        if (!gameKnowledge->Initialize("../../")) {
            std::cerr << "Erro ao inicializar conhecimento específico do jogo" << std::endl;
            return 1;
        }
        
        // 4. Carregar dados do jogo
        std::cout << "Carregando dados do jogo..." << std::endl;
        gameKnowledge->LoadGameData([](float progress, const std::string& status) {
            DisplayTrainingProgress(progress, status);
        });
        
        // 5. Criar agente de conhecimento
        auto agent = std::make_shared<WYDKnowledgeAgent>();
        agent->Initialize("");
        agent->SetGameKnowledge(gameKnowledge);
        
        // 6. Criar treinador de agentes
        auto trainer = std::make_shared<AgentTrainer>(knowledgeBase);
        trainer->SetLogCallback(LogTrainingMessage);
        trainer->SetProgressCallback(DisplayTrainingProgress);
        
        // 7. Inicializar treinador
        if (!trainer->Initialize("../../", "./models")) {
            std::cerr << "Erro ao inicializar treinador de agentes" << std::endl;
            return 1;
        }
        
        // 8. Adicionar agente ao treinador
        trainer->AddAgent(agent);
        
        // 9. Definir parâmetros de treinamento
        TrainingParameters params;
        params.epochCount = 3;       // Menos épocas para o exemplo
        params.batchSize = 8;        // Lotes menores
        params.learningRate = 0.001f; // Taxa de aprendizado maior
        trainer->SetTrainingParameters(params);
        
        // 10. Iniciar treinamento
        std::cout << "\nIniciando treinamento do agente...\n" << std::endl;
        if (!trainer->StartTraining()) {
            std::cerr << "Erro ao iniciar treinamento" << std::endl;
            return 1;
        }
        
        // 11. Aguardar conclusão do treinamento
        while (trainer->IsTraining()) {
            float progress = trainer->GetTrainingProgress();
            std::string status = trainer->GetTrainingStatus();
            DisplayTrainingProgress(progress, status);
            
            // Aguardar um pouco
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "\nTreinamento concluído!\n" << std::endl;
        
        // 12. Exportar dados de treinamento
        std::cout << "Exportando dados de treinamento..." << std::endl;
        trainer->ExportTrainingData("./training_data.csv");
        
        // 13. Testar o agente
        TestAgentWithQuestions(agent);
        
        std::cout << "\n=== Exemplo concluído com sucesso ===\n" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro durante a execução do exemplo: " << e.what() << std::endl;
        return 1;
    }
}