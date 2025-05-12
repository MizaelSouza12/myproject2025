#include "../include/BaseAgent.h"
#include "../include/GameKnowledgeBase.h"
#include "../include/WYDSpecificKnowledge.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>

using namespace WYDStudio;

// Classe fictícia para simular um agente de conhecimento do jogo
class GameKnowledgeAgent : public BaseAgent {
public:
    GameKnowledgeAgent() {
        // Adicionar capacidade de responder perguntas sobre o jogo
        m_capabilities.push_back(AgentCapability::GAME_KNOWLEDGE);
    }
    
    bool Initialize(const std::string& modelPath) override {
        std::cout << "GameKnowledgeAgent inicializado" << std::endl;
        m_initialized = true;
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
            return "Base de conhecimento do jogo não configurada";
        }
        
        std::cout << "Processando pergunta: " << query << std::endl;
        
        // Usar a base de conhecimento do jogo para responder à pergunta
        return m_gameKnowledge->AnswerGameQuestion(query);
    }
    
    std::string ExecuteAction(const std::string& actionName, 
                           const std::map<std::string, std::string>& parameters) override {
        return "GameKnowledgeAgent executou: " + actionName;
    }
    
    AgentType GetAgentType() const override {
        return AgentType::GM;
    }
    
    std::vector<AgentCapability> GetCapabilities() const override {
        return m_capabilities;
    }
    
private:
    bool m_initialized = false;
    std::shared_ptr<WYDSpecificKnowledge> m_gameKnowledge;
};

// Função para logar mensagens
void LogMessage(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
}

// Função auxiliar para responder uma pergunta
void AnswerQuestion(std::shared_ptr<GameKnowledgeAgent> agent, const std::string& question) {
    std::cout << "\n===== PERGUNTA =====\n" << question << "\n" << std::endl;
    std::string answer = agent->ProcessQuery(question);
    std::cout << "===== RESPOSTA =====\n" << answer << "\n" << std::endl;
}

int main() {
    std::cout << "=== Exemplo de Sistema de Conhecimento do Jogo WYD ===\n" << std::endl;
    
    try {
        // 1. Criar a base de conhecimento geral
        auto gameKnowledgeBase = std::make_shared<GameKnowledgeBase>();
        gameKnowledgeBase->SetLogCallback(LogMessage);
        
        // 2. Criar o conhecimento específico do WYD
        auto wydKnowledge = std::make_shared<WYDSpecificKnowledge>(gameKnowledgeBase);
        wydKnowledge->SetLogCallback(LogMessage);
        
        // 3. Inicializar com o caminho para os dados do jogo
        if (!wydKnowledge->Initialize("../../")) {
            std::cerr << "Erro ao inicializar conhecimento do WYD" << std::endl;
            return 1;
        }
        
        // 4. Carregar dados do jogo
        wydKnowledge->LoadGameData([](float progress, const std::string& message) {
            std::cout << "[PROGRESS] " << message << " - " << (progress * 100) << "%" << std::endl;
        });
        
        // 5. Criar um agente com conhecimento do jogo
        auto agent = std::make_shared<GameKnowledgeAgent>();
        agent->Initialize("");
        agent->SetGameKnowledge(wydKnowledge);
        
        // 6. Testar perguntas sobre o jogo
        
        // Exemplo 1: Preço de item em NPC específico
        AnswerQuestion(agent, "Qual é o preço da Potion of Vigor no NPC AKI?");
        
        // Exemplo 2: Localização de NPC
        AnswerQuestion(agent, "Onde encontrar o NPC AKI?");
        
        // Exemplo 3: Itens vendidos por NPC
        AnswerQuestion(agent, "Quais itens o NPC AKI vende?");
        
        // Exemplo 4: NPCs em um mapa
        AnswerQuestion(agent, "Quais NPCs estão no mapa Armia?");
        
        // Exemplo 5: Detalhes de um item
        AnswerQuestion(agent, "O que é Potion of Wisdom?");
        
        // Exemplo 6: Perguntas em formato livre
        AnswerQuestion(agent, "Onde comprar Small Healing Potion?");
        AnswerQuestion(agent, "Quanto custa Town Portal no Noa?");
        AnswerQuestion(agent, "Preço Medium Mana Potion?");
        
        std::cout << "\n=== Exemplo concluído com sucesso ===\n" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro durante a execução do exemplo: " << e.what() << std::endl;
        return 1;
    }
}