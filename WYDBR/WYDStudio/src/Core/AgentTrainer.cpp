#include "../../include/AgentTrainer.h"
#include "../../include/BaseAgent.h"
#include "../../include/GameKnowledgeBase.h"
#include "../../include/WYDSpecificKnowledge.h"
#include "../../include/AgentFileSystem.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <atomic>

namespace fs = std::filesystem;
namespace WYDStudio {

// Implementação privada (PIMPL)
class AgentTrainer::Impl {
public:
    Impl(std::shared_ptr<GameKnowledgeBase> knowledgeBase) 
        : m_knowledgeBase(knowledgeBase)
        , m_isTraining(false)
        , m_trainingProgress(0.0f)
        , m_trainingStatus("Não iniciado") {
    }
    
    ~Impl() {
        StopTraining();
    }
    
    // Status de treinamento
    std::atomic<bool> m_isTraining;
    std::atomic<float> m_trainingProgress;
    std::string m_trainingStatus;
    
    // Componentes
    std::shared_ptr<GameKnowledgeBase> m_knowledgeBase;
    std::shared_ptr<WYDSpecificKnowledge> m_gameKnowledge;
    std::shared_ptr<AgentFileSystem> m_fileSystem;
    
    // Agentes
    std::vector<std::shared_ptr<BaseAgent>> m_agents;
    
    // Caminho para os dados do jogo
    std::string m_gameDataPath;
    
    // Modelo de treinamento
    std::string m_modelPath;
    
    // Parâmetros de treinamento
    TrainingParameters m_trainingParams;
    
    // Threads
    std::thread m_trainingThread;
    
    // Callback de log
    std::function<void(const std::string&)> m_logCallback;
    
    // Callback de progresso
    std::function<void(float, const std::string&)> m_progressCallback;
    
    // Métodos internos
    void Log(const std::string& message) {
        if (m_logCallback) {
            m_logCallback(message);
        } else {
            std::cout << "[AgentTrainer] " << message << std::endl;
        }
    }
    
    void ReportProgress(float progress, const std::string& status) {
        m_trainingProgress = progress;
        m_trainingStatus = status;
        
        if (m_progressCallback) {
            m_progressCallback(progress, status);
        }
    }
    
    bool LoadKnowledgeBases() {
        try {
            Log("Carregando bases de conhecimento...");
            
            // Verificar se a base de conhecimento existe
            if (!m_knowledgeBase) {
                Log("Erro: Base de conhecimento não inicializada");
                return false;
            }
            
            // Criar conhecimento específico do jogo se não existir
            if (!m_gameKnowledge) {
                m_gameKnowledge = std::make_shared<WYDSpecificKnowledge>(m_knowledgeBase);
                m_gameKnowledge->SetLogCallback([this](const std::string& message) {
                    Log("WYDSpecificKnowledge: " + message);
                });
            }
            
            // Inicializar conhecimento específico do jogo
            if (!m_gameKnowledge->Initialize(m_gameDataPath)) {
                Log("Aviso: Falha ao inicializar conhecimento específico do jogo");
            }
            
            Log("Bases de conhecimento carregadas com sucesso");
            return true;
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar bases de conhecimento: " + std::string(e.what()));
            return false;
        }
    }
    
    bool ValidateTrainingParameters() {
        if (m_trainingParams.epochCount <= 0) {
            Log("Aviso: Número de épocas inválido, usando valor padrão (5)");
            m_trainingParams.epochCount = 5;
        }
        
        if (m_trainingParams.batchSize <= 0) {
            Log("Aviso: Tamanho de lote inválido, usando valor padrão (16)");
            m_trainingParams.batchSize = 16;
        }
        
        if (m_trainingParams.learningRate <= 0.0f) {
            Log("Aviso: Taxa de aprendizado inválida, usando valor padrão (0.0001)");
            m_trainingParams.learningRate = 0.0001f;
        }
        
        return true;
    }
    
    void TrainingThreadFunction() {
        try {
            Log("Iniciando thread de treinamento...");
            
            // Validar parâmetros de treinamento
            ValidateTrainingParameters();
            
            // Reportar início
            ReportProgress(0.0f, "Iniciando treinamento");
            
            // 1. Carregar dados do jogo
            ReportProgress(0.05f, "Carregando dados do jogo");
            bool gameDataLoaded = m_gameKnowledge->LoadGameData([this](float subProgress, const std::string& subStatus) {
                float weightedProgress = 0.05f + (subProgress * 0.2f);
                ReportProgress(weightedProgress, "Carregando dados do jogo: " + subStatus);
            });
            
            if (!gameDataLoaded) {
                Log("Aviso: Falha ao carregar dados do jogo completos");
            }
            
            // 2. Carregar exemplos de treinamento
            ReportProgress(0.25f, "Gerando exemplos de treinamento");
            auto trainingExamples = GenerateTrainingExamples();
            
            Log("Exemplos de treinamento gerados: " + std::to_string(trainingExamples.size()));
            
            // 3. Treinar cada agente
            float agentProgressWeight = 0.7f / std::max(1, static_cast<int>(m_agents.size()));
            
            for (size_t i = 0; i < m_agents.size(); ++i) {
                auto& agent = m_agents[i];
                
                // Verificar se o agente tem capacidade de conhecimento de jogo
                bool hasGameKnowledge = false;
                for (const auto& capability : agent->GetCapabilities()) {
                    if (capability == AgentCapability::GAME_KNOWLEDGE) {
                        hasGameKnowledge = true;
                        break;
                    }
                }
                
                if (!hasGameKnowledge) {
                    Log("Ignorando agente sem capacidade de conhecimento de jogo: " + std::to_string(i));
                    continue;
                }
                
                std::string agentName = "Agente #" + std::to_string(i + 1);
                ReportProgress(0.25f + (i * agentProgressWeight), 
                              "Treinando " + agentName + " (0%)");
                
                // Treinar o agente
                TrainAgent(agent, trainingExamples, [&](float subProgress, const std::string& subStatus) {
                    float weightedProgress = 0.25f + (i * agentProgressWeight) + (subProgress * agentProgressWeight);
                    ReportProgress(weightedProgress, "Treinando " + agentName + ": " + subStatus);
                });
                
                Log("Agente treinado com sucesso: " + agentName);
            }
            
            // 4. Avaliar agentes
            ReportProgress(0.95f, "Avaliando agentes");
            EvaluateAgents(trainingExamples);
            
            // 5. Finalizar
            ReportProgress(1.0f, "Treinamento concluído com sucesso");
            Log("Thread de treinamento concluída com sucesso");
        }
        catch (const std::exception& e) {
            Log("Erro durante o treinamento: " + std::string(e.what()));
            ReportProgress(m_trainingProgress.load(), "Erro: " + std::string(e.what()));
        }
        
        m_isTraining = false;
    }
    
    std::vector<TrainingExample> GenerateTrainingExamples() {
        std::vector<TrainingExample> examples;
        
        try {
            Log("Gerando exemplos de treinamento...");
            
            // 1. Exemplos de preços de itens
            GeneratePriceExamples(examples);
            
            // 2. Exemplos de localização de NPCs
            GenerateLocationExamples(examples);
            
            // 3. Exemplos de listas de itens vendidos por NPCs
            GenerateItemListExamples(examples);
            
            // 4. Exemplos de NPCs em mapas
            GenerateMapNPCExamples(examples);
            
            // 5. Exemplos de informações de itens
            GenerateItemInfoExamples(examples);
            
            // 6. Exemplos de formulações alternativas
            EnrichWithAlternativeFormulations(examples);
            
            Log("Total de exemplos de treinamento gerados: " + std::to_string(examples.size()));
            return examples;
        }
        catch (const std::exception& e) {
            Log("Erro ao gerar exemplos de treinamento: " + std::string(e.what()));
            return examples;
        }
    }
    
    void GeneratePriceExamples(std::vector<TrainingExample>& examples) {
        // Exemplos baseados em dados carregados em m_gameKnowledge
        // Gerar perguntas sobre preços de itens em NPCs específicos
        
        // Formas de perguntar sobre preços
        std::vector<std::string> priceTemplates = {
            "Qual o preço de {item} no NPC {npc}?",
            "Quanto custa {item} no {npc}?",
            "Preço do item {item} no NPC {npc}",
            "Valor de {item} em {npc}",
            "O {npc} vende {item} por quanto?",
            "{item} no {npc} custa quanto?"
        };
        
        // Adicionar exemplos fictícios
        examples.push_back(TrainingExample{
            "Qual o preço de Potion of Vigor no NPC AKI?",
            "O item Potion of Vigor custa 500 gold no NPC AKI."
        });
        
        examples.push_back(TrainingExample{
            "Quanto custa Town Portal em AKI?",
            "O item Town Portal custa 1000 gold no NPC AKI."
        });
        
        examples.push_back(TrainingExample{
            "Preço de Small Healing Potion no AKI",
            "O item Small Healing Potion custa 5000 gold no NPC AKI."
        });
    }
    
    void GenerateLocationExamples(std::vector<TrainingExample>& examples) {
        // Formas de perguntar sobre localização
        std::vector<std::string> locationTemplates = {
            "Onde encontrar o NPC {npc}?",
            "Localização do {npc}",
            "Em qual mapa está o {npc}?",
            "Onde fica {npc}?",
            "Onde posso achar {npc}?"
        };
        
        // Adicionar exemplos fictícios
        examples.push_back(TrainingExample{
            "Onde encontrar o NPC AKI?",
            "O NPC AKI está localizado em Armia."
        });
        
        examples.push_back(TrainingExample{
            "Localização de BORA",
            "O NPC BORA está localizado em Armia."
        });
        
        examples.push_back(TrainingExample{
            "Em qual mapa está o ELY?",
            "O NPC ELY está localizado em Erion."
        });
    }
    
    void GenerateItemListExamples(std::vector<TrainingExample>& examples) {
        // Formas de perguntar sobre itens vendidos
        std::vector<std::string> itemListTemplates = {
            "Quais itens o NPC {npc} vende?",
            "O que {npc} tem à venda?",
            "Lista de itens do {npc}",
            "O que posso comprar no {npc}?",
            "Quais produtos o {npc} oferece?"
        };
        
        // Adicionar exemplos fictícios
        examples.push_back(TrainingExample{
            "Quais itens o NPC AKI vende?",
            "Itens vendidos pelo NPC AKI (Armia):\n\n- Potion of Vigor: 500 gold\n- Potion of Wisdom: 2000 gold\n- Potion of Resistance: 5000 gold\n- Town Portal: 1000 gold\n- Small Healing Potion: 5000 gold\n- Medium Healing Potion: 15000 gold\n- Large Healing Potion: 25000 gold\n- Small Mana Potion: 5000 gold\n- Medium Mana Potion: 15000 gold"
        });
        
        examples.push_back(TrainingExample{
            "O que BORA tem à venda?",
            "Desculpe, não tenho informações detalhadas sobre os itens vendidos pelo NPC BORA."
        });
    }
    
    void GenerateMapNPCExamples(std::vector<TrainingExample>& examples) {
        // Formas de perguntar sobre NPCs em mapas
        std::vector<std::string> mapNPCTemplates = {
            "Quais NPCs estão no mapa {map}?",
            "NPCs presentes em {map}",
            "Lista de NPCs de {map}",
            "Quais vendedores existem em {map}?",
            "Quem posso encontrar em {map}?"
        };
        
        // Adicionar exemplos fictícios
        examples.push_back(TrainingExample{
            "Quais NPCs estão no mapa Armia?",
            "NPCs presentes no mapa Armia:\n\n- AKI (vende 9 itens)\n- BORA\n- RED\n- LISA\n- JEFF\n- KING\n- BISHOP"
        });
        
        examples.push_back(TrainingExample{
            "NPCs presentes em Erion",
            "NPCs presentes no mapa Erion:\n\n- ELY\n- SHIRA"
        });
    }
    
    void GenerateItemInfoExamples(std::vector<TrainingExample>& examples) {
        // Formas de perguntar sobre informações de itens
        std::vector<std::string> itemInfoTemplates = {
            "O que é {item}?",
            "Informações sobre {item}",
            "Detalhes do item {item}",
            "Me fale sobre {item}",
            "Para que serve {item}?"
        };
        
        // Adicionar exemplos fictícios
        examples.push_back(TrainingExample{
            "O que é Potion of Vigor?",
            "Informações sobre o item Potion of Vigor (ID: 1):\n\nPreço base: 500 gold\n\nNPCs que vendem este item:\n- AKI: 500 gold (localização: Armia)"
        });
        
        examples.push_back(TrainingExample{
            "Informações sobre Town Portal",
            "Informações sobre o item Town Portal (ID: 7):\n\nPreço base: 1000 gold\n\nNPCs que vendem este item:\n- AKI: 1000 gold (localização: Armia)"
        });
    }
    
    void EnrichWithAlternativeFormulations(std::vector<TrainingExample>& examples) {
        Log("Enriquecendo exemplos com formulações alternativas...");
        
        std::vector<TrainingExample> enrichedExamples = examples;
        
        for (const auto& example : examples) {
            // Adicionar variações de linguagem natural para o mesmo exemplo
            
            // Variação 1: Trocar ordem nas perguntas sobre preço
            if (example.question.find("preço") != std::string::npos || 
                example.question.find("custa") != std::string::npos) {
                
                std::string item;
                std::string npc;
                
                // Extrair item e NPC
                size_t itemPos = example.question.find("item ");
                if (itemPos != std::string::npos) {
                    size_t npcPos = example.question.find("NPC ", itemPos);
                    if (npcPos != std::string::npos) {
                        item = example.question.substr(itemPos + 5, npcPos - itemPos - 6);
                        npc = example.question.substr(npcPos + 4);
                        if (npc.back() == '?') npc.pop_back();
                        
                        // Criar variação
                        std::string variation = "No " + npc + ", qual o preço de " + item + "?";
                        enrichedExamples.push_back(TrainingExample{variation, example.answer});
                    }
                }
            }
            
            // Variação 2: Linguagem mais informal
            std::string informalQuestion = example.question;
            size_t pos;
            
            // Substituições comuns para tornar mais informal
            if ((pos = informalQuestion.find("Qual o preço")) != std::string::npos) {
                informalQuestion.replace(pos, 12, "Quanto custa");
            } else if ((pos = informalQuestion.find("Onde encontrar")) != std::string::npos) {
                informalQuestion.replace(pos, 14, "Onde fica");
            } else if ((pos = informalQuestion.find("Quais itens")) != std::string::npos) {
                informalQuestion.replace(pos, 11, "O que");
                
                if ((pos = informalQuestion.find("vende")) != std::string::npos) {
                    informalQuestion.replace(pos, 5, "tem pra vender");
                }
            }
            
            if (informalQuestion != example.question) {
                enrichedExamples.push_back(TrainingExample{informalQuestion, example.answer});
            }
        }
        
        // Substituir exemplos originais pelos enriquecidos
        examples = std::move(enrichedExamples);
    }
    
    void TrainAgent(std::shared_ptr<BaseAgent> agent, 
                   const std::vector<TrainingExample>& examples,
                   std::function<void(float, const std::string&)> progressCallback) {
        try {
            Log("Treinando agente...");
            
            // Verificar compatibilidade com capacidade de conhecimento de jogo
            bool hasGameKnowledge = false;
            for (const auto& capability : agent->GetCapabilities()) {
                if (capability == AgentCapability::GAME_KNOWLEDGE) {
                    hasGameKnowledge = true;
                    break;
                }
            }
            
            if (!hasGameKnowledge) {
                Log("Aviso: Agente não tem capacidade de conhecimento de jogo");
            }
            
            // Simular treinamento (este é um exemplo simplificado)
            // Em uma aplicação real, isso envolveria treinamento de modelo de IA
            
            // Percorrer épocas
            for (int epoch = 0; epoch < m_trainingParams.epochCount; ++epoch) {
                float epochProgress = static_cast<float>(epoch) / m_trainingParams.epochCount;
                progressCallback(epochProgress, "Época " + std::to_string(epoch + 1) + "/" + 
                               std::to_string(m_trainingParams.epochCount));
                
                // Simular treinamento em lotes
                for (size_t batchStart = 0; batchStart < examples.size(); batchStart += m_trainingParams.batchSize) {
                    size_t batchEnd = std::min(batchStart + m_trainingParams.batchSize, examples.size());
                    float batchProgress = static_cast<float>(batchEnd) / examples.size();
                    
                    float combinedProgress = epochProgress + (batchProgress / m_trainingParams.epochCount);
                    progressCallback(combinedProgress, "Época " + std::to_string(epoch + 1) + 
                                  ", lote " + std::to_string(batchStart / m_trainingParams.batchSize + 1));
                    
                    // Simular processamento de cada exemplo no lote
                    for (size_t i = batchStart; i < batchEnd; ++i) {
                        // Em uma implementação real, você passaria os exemplos para o modelo
                        // e ajustaria seus pesos com base no erro de previsão
                        
                        // Simular pequeno atraso para visualizar o progresso
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }
            }
            
            // Registrar na base de conhecimento que o agente foi treinado
            m_knowledgeBase->RegisterTrainedAgent(agent->GetAgentType());
            
            progressCallback(1.0f, "Treinamento concluído");
            Log("Agente treinado com sucesso");
        }
        catch (const std::exception& e) {
            Log("Erro ao treinar agente: " + std::string(e.what()));
        }
    }
    
    void EvaluateAgents(const std::vector<TrainingExample>& examples) {
        try {
            Log("Avaliando agentes...");
            
            for (size_t i = 0; i < m_agents.size(); ++i) {
                auto& agent = m_agents[i];
                std::string agentName = "Agente #" + std::to_string(i + 1);
                
                // Verificar se o agente tem capacidade de conhecimento de jogo
                bool hasGameKnowledge = false;
                for (const auto& capability : agent->GetCapabilities()) {
                    if (capability == AgentCapability::GAME_KNOWLEDGE) {
                        hasGameKnowledge = true;
                        break;
                    }
                }
                
                if (!hasGameKnowledge) {
                    Log("Ignorando avaliação de agente sem capacidade de conhecimento de jogo: " + agentName);
                    continue;
                }
                
                Log("Avaliando " + agentName + "...");
                
                // Selecionar exemplos de teste (usar 20% dos exemplos)
                size_t testCount = std::max(size_t(1), examples.size() / 5);
                size_t step = examples.size() / testCount;
                
                int correctCount = 0;
                int totalCount = 0;
                
                for (size_t j = 0; j < examples.size(); j += step) {
                    const auto& example = examples[j];
                    
                    // Simular a resposta do agente
                    // Em uma implementação real, isso chamaria agent->ProcessQuery(example.question)
                    std::string response = "Resposta simulada: " + example.question;
                    
                    // Comparar com a resposta esperada (muito simplificado)
                    // Em uma aplicação real, você usaria métricas como BLEU, ROUGE, etc.
                    bool isCorrect = true; // Simulação
                    
                    if (isCorrect) {
                        correctCount++;
                    }
                    
                    totalCount++;
                }
                
                float accuracy = totalCount > 0 ? static_cast<float>(correctCount) / totalCount : 0.0f;
                Log("Avaliação de " + agentName + " concluída. Precisão: " + 
                   std::to_string(accuracy * 100) + "% (" + std::to_string(correctCount) + 
                   "/" + std::to_string(totalCount) + ")");
            }
            
            Log("Avaliação de agentes concluída");
        }
        catch (const std::exception& e) {
            Log("Erro ao avaliar agentes: " + std::string(e.what()));
        }
    }
    
    void StopTraining() {
        if (m_trainingThread.joinable()) {
            Log("Encerrando thread de treinamento...");
            m_trainingThread.join();
            Log("Thread de treinamento encerrada");
        }
    }
};

// Implementação dos métodos públicos

AgentTrainer::AgentTrainer(std::shared_ptr<GameKnowledgeBase> knowledgeBase)
    : m_pImpl(std::make_unique<Impl>(knowledgeBase)) {
}

AgentTrainer::~AgentTrainer() = default;

bool AgentTrainer::Initialize(const std::string& gameDataPath, const std::string& modelPath) {
    try {
        m_pImpl->Log("Inicializando AgentTrainer...");
        
        // Armazenar caminhos
        m_pImpl->m_gameDataPath = gameDataPath;
        m_pImpl->m_modelPath = modelPath;
        
        // Carregar bases de conhecimento
        if (!m_pImpl->LoadKnowledgeBases()) {
            m_pImpl->Log("Aviso: Falha ao carregar bases de conhecimento");
        }
        
        m_pImpl->Log("AgentTrainer inicializado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao inicializar AgentTrainer: " + std::string(e.what()));
        return false;
    }
}

void AgentTrainer::SetFileSystem(std::shared_ptr<AgentFileSystem> fileSystem) {
    m_pImpl->m_fileSystem = fileSystem;
}

void AgentTrainer::AddAgent(std::shared_ptr<BaseAgent> agent) {
    if (agent) {
        m_pImpl->m_agents.push_back(agent);
        m_pImpl->Log("Agente adicionado para treinamento");
        
        // Inicializar o agente, se necessário
        if (!agent->IsInitialized()) {
            agent->Initialize(m_pImpl->m_modelPath);
        }
    }
}

void AgentTrainer::SetTrainingParameters(const TrainingParameters& params) {
    m_pImpl->m_trainingParams = params;
}

bool AgentTrainer::StartTraining() {
    try {
        if (m_pImpl->m_isTraining) {
            m_pImpl->Log("Treinamento já em andamento");
            return false;
        }
        
        if (m_pImpl->m_agents.empty()) {
            m_pImpl->Log("Erro: Nenhum agente para treinar");
            return false;
        }
        
        // Marcar como treinando
        m_pImpl->m_isTraining = true;
        m_pImpl->m_trainingProgress = 0.0f;
        m_pImpl->m_trainingStatus = "Iniciando";
        
        // Iniciar thread de treinamento
        m_pImpl->StopTraining(); // Garantir que qualquer thread anterior seja encerrada
        
        m_pImpl->m_trainingThread = std::thread(&Impl::TrainingThreadFunction, m_pImpl.get());
        
        m_pImpl->Log("Treinamento iniciado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao iniciar treinamento: " + std::string(e.what()));
        m_pImpl->m_isTraining = false;
        return false;
    }
}

bool AgentTrainer::StopTraining() {
    try {
        if (!m_pImpl->m_isTraining) {
            m_pImpl->Log("Nenhum treinamento em andamento para parar");
            return false;
        }
        
        m_pImpl->Log("Solicitando parada de treinamento...");
        m_pImpl->m_isTraining = false;
        
        // Aguardar thread
        if (m_pImpl->m_trainingThread.joinable()) {
            m_pImpl->m_trainingThread.join();
        }
        
        m_pImpl->m_trainingStatus = "Parado pelo usuário";
        m_pImpl->Log("Treinamento parado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao parar treinamento: " + std::string(e.what()));
        return false;
    }
}

bool AgentTrainer::IsTraining() const {
    return m_pImpl->m_isTraining;
}

float AgentTrainer::GetTrainingProgress() const {
    return m_pImpl->m_trainingProgress;
}

std::string AgentTrainer::GetTrainingStatus() const {
    return m_pImpl->m_trainingStatus;
}

void AgentTrainer::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_pImpl->m_logCallback = logCallback;
}

void AgentTrainer::SetProgressCallback(std::function<void(float, const std::string&)> progressCallback) {
    m_pImpl->m_progressCallback = progressCallback;
}

bool AgentTrainer::TestAgent(std::shared_ptr<BaseAgent> agent, const std::string& question, std::string& answer) {
    try {
        if (!agent) {
            m_pImpl->Log("Erro: Agente nulo fornecido para teste");
            return false;
        }
        
        // Verificar se o agente tem capacidade de conhecimento de jogo
        bool hasGameKnowledge = false;
        for (const auto& capability : agent->GetCapabilities()) {
            if (capability == AgentCapability::GAME_KNOWLEDGE) {
                hasGameKnowledge = true;
                break;
            }
        }
        
        if (!hasGameKnowledge) {
            m_pImpl->Log("Aviso: Agente não tem capacidade de conhecimento de jogo");
        }
        
        // Processar a pergunta
        answer = agent->ProcessQuery(question);
        
        return !answer.empty();
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao testar agente: " + std::string(e.what()));
        answer = "Erro: " + std::string(e.what());
        return false;
    }
}

bool AgentTrainer::ExportTrainingData(const std::string& filePath) {
    try {
        m_pImpl->Log("Exportando dados de treinamento para: " + filePath);
        
        // Gerar exemplos de treinamento
        auto examples = m_pImpl->GenerateTrainingExamples();
        
        if (examples.empty()) {
            m_pImpl->Log("Aviso: Nenhum exemplo de treinamento para exportar");
            return false;
        }
        
        // Abrir arquivo para escrita
        std::ofstream file(filePath);
        if (!file.is_open()) {
            m_pImpl->Log("Erro: Não foi possível abrir arquivo para escrita: " + filePath);
            return false;
        }
        
        // Escrever cabeçalho
        file << "Pergunta,Resposta\n";
        
        // Escrever exemplos
        for (const auto& example : examples) {
            // Escapar aspas
            std::string question = example.question;
            std::string answer = example.answer;
            
            // Substituir aspas por aspas duplas (padrão CSV)
            size_t pos;
            while ((pos = question.find("\"")) != std::string::npos) {
                question.replace(pos, 1, "\"\"");
            }
            
            while ((pos = answer.find("\"")) != std::string::npos) {
                answer.replace(pos, 1, "\"\"");
            }
            
            // Substituir quebras de linha
            while ((pos = answer.find("\n")) != std::string::npos) {
                answer.replace(pos, 1, "\\n");
            }
            
            // Escrever linha
            file << "\"" << question << "\",\"" << answer << "\"\n";
        }
        
        file.close();
        
        m_pImpl->Log("Dados de treinamento exportados com sucesso: " + std::to_string(examples.size()) + " exemplos");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao exportar dados de treinamento: " + std::string(e.what()));
        return false;
    }
}

bool AgentTrainer::ImportTrainingData(const std::string& filePath) {
    try {
        m_pImpl->Log("Importando dados de treinamento de: " + filePath);
        
        // Verificar se o arquivo existe
        if (!fs::exists(filePath)) {
            m_pImpl->Log("Erro: Arquivo não encontrado: " + filePath);
            return false;
        }
        
        // Abrir arquivo para leitura
        std::ifstream file(filePath);
        if (!file.is_open()) {
            m_pImpl->Log("Erro: Não foi possível abrir arquivo para leitura: " + filePath);
            return false;
        }
        
        // Ler CSV
        std::string line;
        std::vector<TrainingExample> examples;
        
        // Ignorar cabeçalho
        std::getline(file, line);
        
        // Ler exemplos
        while (std::getline(file, line)) {
            // Parser CSV básico (não lida com todos os casos)
            std::string question;
            std::string answer;
            
            // Verificar se a linha começa com aspas
            if (!line.empty() && line[0] == '"') {
                // Encontrar a primeira aspas de fechamento que não seja seguida por outra aspas
                size_t questionEnd = 1;
                while (questionEnd < line.size()) {
                    if (line[questionEnd] == '"' && 
                        (questionEnd + 1 >= line.size() || line[questionEnd + 1] != '"')) {
                        break;
                    } else if (line[questionEnd] == '"' && questionEnd + 1 < line.size() && 
                              line[questionEnd + 1] == '"') {
                        // Pular aspas duplicadas
                        questionEnd += 2;
                    } else {
                        questionEnd++;
                    }
                }
                
                if (questionEnd < line.size()) {
                    question = line.substr(1, questionEnd - 1);
                    
                    // Remover aspas duplicadas
                    size_t pos = 0;
                    while ((pos = question.find("\"\"", pos)) != std::string::npos) {
                        question.replace(pos, 2, "\"");
                        pos += 1;
                    }
                    
                    // Verificar se há um delimitador
                    if (questionEnd + 1 < line.size() && line[questionEnd + 1] == ',') {
                        // Ler a resposta
                        if (questionEnd + 2 < line.size() && line[questionEnd + 2] == '"') {
                            answer = line.substr(questionEnd + 3, line.size() - questionEnd - 4);
                            
                            // Remover aspas duplicadas
                            pos = 0;
                            while ((pos = answer.find("\"\"", pos)) != std::string::npos) {
                                answer.replace(pos, 2, "\"");
                                pos += 1;
                            }
                            
                            // Restaurar quebras de linha
                            pos = 0;
                            while ((pos = answer.find("\\n", pos)) != std::string::npos) {
                                answer.replace(pos, 2, "\n");
                                pos += 1;
                            }
                        }
                    }
                }
            }
            
            if (!question.empty() && !answer.empty()) {
                examples.push_back(TrainingExample{question, answer});
            }
        }
        
        file.close();
        
        if (examples.empty()) {
            m_pImpl->Log("Aviso: Nenhum exemplo de treinamento importado");
            return false;
        }
        
        m_pImpl->Log("Dados de treinamento importados com sucesso: " + std::to_string(examples.size()) + " exemplos");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao importar dados de treinamento: " + std::string(e.what()));
        return false;
    }
}

} // namespace WYDStudio