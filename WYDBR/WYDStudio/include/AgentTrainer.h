#pragma once

#include "BaseAgent.h"
#include "GameKnowledgeBase.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace WYDStudio {

class AgentFileSystem;

/**
 * @struct TrainingExample
 * @brief Exemplo de par pergunta-resposta para treinamento
 */
struct TrainingExample {
    std::string question; // Pergunta
    std::string answer;   // Resposta esperada
};

/**
 * @struct TrainingParameters
 * @brief Parâmetros para o treinamento de agentes
 */
struct TrainingParameters {
    int epochCount;        // Número de épocas de treinamento
    int batchSize;         // Tamanho do lote
    float learningRate;    // Taxa de aprendizado
    bool usePretrainedModel; // Usar modelo pré-treinado
    std::string outputModelPath; // Caminho para salvar o modelo treinado
    
    TrainingParameters()
        : epochCount(5)
        , batchSize(16)
        , learningRate(0.0001f)
        , usePretrainedModel(true)
        , outputModelPath("") {
    }
};

/**
 * @class AgentTrainer
 * @brief Classe responsável por treinar agentes com dados do jogo
 * 
 * Esta classe gerencia o treinamento de agentes com exemplos gerados
 * a partir dos dados do jogo, permitindo que eles respondam a perguntas
 * sobre itens, NPCs, preços, etc.
 */
class AgentTrainer {
public:
    /**
     * @brief Construtor
     * @param knowledgeBase Base de conhecimento do jogo
     */
    AgentTrainer(std::shared_ptr<GameKnowledgeBase> knowledgeBase);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~AgentTrainer();
    
    /**
     * @brief Inicializa o treinador de agentes
     * @param gameDataPath Caminho para os dados do jogo
     * @param modelPath Caminho para os modelos de IA
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& gameDataPath, const std::string& modelPath = "");
    
    /**
     * @brief Define o sistema de arquivos a ser usado
     * @param fileSystem Sistema de arquivos
     */
    void SetFileSystem(std::shared_ptr<AgentFileSystem> fileSystem);
    
    /**
     * @brief Adiciona um agente para treinamento
     * @param agent Agente a ser treinado
     */
    void AddAgent(std::shared_ptr<BaseAgent> agent);
    
    /**
     * @brief Define os parâmetros de treinamento
     * @param params Parâmetros de treinamento
     */
    void SetTrainingParameters(const TrainingParameters& params);
    
    /**
     * @brief Inicia o treinamento dos agentes
     * @return true se iniciado com sucesso
     */
    bool StartTraining();
    
    /**
     * @brief Para o treinamento em andamento
     * @return true se parado com sucesso
     */
    bool StopTraining();
    
    /**
     * @brief Verifica se o treinamento está em andamento
     * @return true se estiver treinando
     */
    bool IsTraining() const;
    
    /**
     * @brief Obtém o progresso do treinamento
     * @return Progresso de 0.0 a 1.0
     */
    float GetTrainingProgress() const;
    
    /**
     * @brief Obtém o status do treinamento
     * @return String com o status atual
     */
    std::string GetTrainingStatus() const;
    
    /**
     * @brief Define uma função de callback para log
     * @param logCallback Função de callback para log
     */
    void SetLogCallback(std::function<void(const std::string&)> logCallback);
    
    /**
     * @brief Define uma função de callback para progresso
     * @param progressCallback Função de callback para progresso
     */
    void SetProgressCallback(std::function<void(float, const std::string&)> progressCallback);
    
    /**
     * @brief Testa um agente com uma pergunta
     * @param agent Agente a ser testado
     * @param question Pergunta a ser feita
     * @param answer Resposta recebida (saída)
     * @return true se o teste foi bem-sucedido
     */
    bool TestAgent(std::shared_ptr<BaseAgent> agent, const std::string& question, std::string& answer);
    
    /**
     * @brief Exporta dados de treinamento para um arquivo
     * @param filePath Caminho do arquivo
     * @return true se exportado com sucesso
     */
    bool ExportTrainingData(const std::string& filePath);
    
    /**
     * @brief Importa dados de treinamento de um arquivo
     * @param filePath Caminho do arquivo
     * @return true se importado com sucesso
     */
    bool ImportTrainingData(const std::string& filePath);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
};

} // namespace WYDStudio