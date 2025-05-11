#ifndef WYDSTUDIO_WYD_ASSISTANT_INTEGRATION_H
#define WYDSTUDIO_WYD_ASSISTANT_INTEGRATION_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <future>
#include <atomic>
#include <mutex>
#include <optional>
#include <chrono>
#include <queue>
#include <set>
#include <nlohmann/json.hpp>
#include "WYDLanguageDatabase.h"
#include "../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ai {

using json = nlohmann::json;

/**
 * @brief Configuração do assistente IA
 */
struct WYDAssistantConfig {
    std::string modelName;
    std::string apiKey;
    float temperature;
    int maxTokens;
    int contextWindow;
    int maxHistory;
    bool enableOfflineMode;
    std::string offlineModelPath;
    bool prioritizeLocalRules;
    bool enableSuggestions;
    int suggestionThreshold;
    std::string systemPrompt;
    std::map<std::string, std::string> customSettings;
};

/**
 * @brief Tipo de assistente
 */
enum class AssistantType {
    PLAYER,
    GM,
    ADMIN,
    DEVELOPER
};

/**
 * @brief Tipo de resposta do assistente
 */
enum class ResponseType {
    TEXT,
    COMMAND,
    ACTION,
    ERROR,
    SUGGESTION,
    HYBRID
};

/**
 * @brief Resposta do assistente
 */
struct AssistantResponse {
    std::string message;
    ResponseType type;
    std::vector<std::string> commands;
    std::vector<std::string> suggestions;
    json metadata;
    bool requiresConfirmation;
    float confidence;
    bool isOfflineGenerated;
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @brief Mensagem de chat
 */
struct ChatMessage {
    std::string content;
    std::string role;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Comando do jogo reconhecido
 */
struct RecognizedCommand {
    std::string command;
    std::map<std::string, std::string> parameters;
    float confidence;
    std::vector<std::string> alternativeCommands;
    bool requiresElevatedPermissions;
    std::string scope;
    std::string originalInput;
};

/**
 * @brief Integração com IA para o WYD
 */
class WYDAssistantIntegration {
public:
    /**
     * @brief Construtor
     */
    WYDAssistantIntegration();
    
    /**
     * @brief Destrutor
     */
    ~WYDAssistantIntegration();
    
    /**
     * @brief Inicializa o assistente
     * @param facade Referência ao facade do WYDStudio
     * @param config Configuração do assistente
     * @param languageDb Banco de dados de linguagem do WYD
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::shared_ptr<core::WYDStudioFacade>& facade, 
                   const WYDAssistantConfig& config,
                   const std::shared_ptr<WYDLanguageDatabase>& languageDb);
    
    /**
     * @brief Define o tipo de assistente
     * @param type Tipo de assistente
     */
    void setAssistantType(AssistantType type);
    
    /**
     * @brief Obtém o tipo de assistente
     * @return Tipo de assistente
     */
    AssistantType getAssistantType() const;
    
    /**
     * @brief Processa uma entrada do usuário
     * @param input Entrada do usuário
     * @param context Contexto adicional (opcional)
     * @return Resposta do assistente
     */
    AssistantResponse processInput(const std::string& input, const json& context = {});
    
    /**
     * @brief Processa uma entrada do usuário de forma assíncrona
     * @param input Entrada do usuário
     * @param callback Função de callback para receber a resposta
     * @param context Contexto adicional (opcional)
     */
    void processInputAsync(const std::string& input, 
                          std::function<void(const AssistantResponse&)> callback,
                          const json& context = {});
    
    /**
     * @brief Cancela o processamento atual
     */
    void cancelProcessing();
    
    /**
     * @brief Gera sugestões baseadas no contexto atual
     * @param currentContext Contexto atual
     * @param maxSuggestions Número máximo de sugestões
     * @return Lista de sugestões
     */
    std::vector<std::string> generateSuggestions(const json& currentContext, int maxSuggestions = 3);
    
    /**
     * @brief Reconhece um comando a partir de uma entrada em linguagem natural
     * @param input Entrada do usuário
     * @return Comando reconhecido ou nulo se não reconhecido
     */
    std::optional<RecognizedCommand> recognizeCommand(const std::string& input);
    
    /**
     * @brief Executa um comando reconhecido
     * @param command Comando reconhecido
     * @param context Contexto adicional (opcional)
     * @return Resultado da execução
     */
    AssistantResponse executeCommand(const RecognizedCommand& command, const json& context = {});
    
    /**
     * @brief Limpa o histórico de chat
     */
    void clearChatHistory();
    
    /**
     * @brief Obtém o histórico de chat
     * @return Histórico de chat
     */
    std::vector<ChatMessage> getChatHistory() const;
    
    /**
     * @brief Adiciona uma mensagem ao histórico
     * @param message Mensagem a ser adicionada
     * @param role Papel da mensagem (user, assistant, system)
     */
    void addMessageToHistory(const std::string& message, const std::string& role);
    
    /**
     * @brief Verifica se o assistente está processando
     * @return true se estiver processando
     */
    bool isProcessing() const;
    
    /**
     * @brief Define o prompt do sistema
     * @param prompt Prompt do sistema
     */
    void setSystemPrompt(const std::string& prompt);
    
    /**
     * @brief Obtém o prompt do sistema
     * @return Prompt do sistema
     */
    std::string getSystemPrompt() const;
    
    /**
     * @brief Define a temperatura do modelo
     * @param temperature Temperatura (0-1)
     */
    void setTemperature(float temperature);
    
    /**
     * @brief Obtém a temperatura do modelo
     * @return Temperatura
     */
    float getTemperature() const;
    
    /**
     * @brief Define o tamanho máximo de tokens
     * @param maxTokens Tamanho máximo
     */
    void setMaxTokens(int maxTokens);
    
    /**
     * @brief Obtém o tamanho máximo de tokens
     * @return Tamanho máximo
     */
    int getMaxTokens() const;
    
    /**
     * @brief Registra uma função de callback para comandos reconhecidos
     * @param commandType Tipo de comando
     * @param callback Função de callback
     * @return ID do registro
     */
    int registerCommandCallback(const std::string& commandType, 
                              std::function<bool(const RecognizedCommand&)> callback);
    
    /**
     * @brief Remove um registro de callback
     * @param registrationId ID do registro
     */
    void unregisterCommandCallback(int registrationId);
    
    /**
     * @brief Define se o assistente deve operar no modo offline
     * @param enable Se verdadeiro, habilita o modo offline
     */
    void setOfflineMode(bool enable);
    
    /**
     * @brief Verifica se o assistente está no modo offline
     * @return true se estiver no modo offline
     */
    bool isOfflineMode() const;
    
    /**
     * @brief Treina o assistente com exemplos específicos do WYD
     * @param examples Exemplos de treinamento (pares de entrada/saída)
     * @return true se treinado com sucesso
     */
    bool trainWithExamples(const std::map<std::string, std::string>& examples);
    
    /**
     * @brief Treina o assistente com dados do jogo
     * @param gameDataPath Caminho para os dados do jogo
     * @return true se treinado com sucesso
     */
    bool trainWithGameData(const std::string& gameDataPath);
    
    /**
     * @brief Exporta os dados de treinamento
     * @param outputPath Caminho de saída
     * @return true se exportado com sucesso
     */
    bool exportTrainingData(const std::string& outputPath) const;
    
    /**
     * @brief Extrai comandos específicos do WYD a partir da conversa atual
     * @return Lista de comandos extraídos
     */
    std::vector<std::string> extractWYDCommands() const;
    
    /**
     * @brief Gera estatísticas sobre comandos e termos utilizados
     * @return Estatísticas em formato JSON
     */
    json generateUsageStatistics() const;
    
    /**
     * @brief Atualiza os termos no banco de dados
     * @return true se atualizado com sucesso
     */
    bool updateTermsDatabase();
    
    /**
     * @brief Converte uma entrada em linguagem natural para comando do jogo
     * @param input Entrada em linguagem natural
     * @return Comando do jogo ou string vazia se não reconhecido
     */
    std::string naturalLanguageToCommand(const std::string& input);
    
    /**
     * @brief Converte um comando do jogo para linguagem natural
     * @param command Comando do jogo
     * @return Descrição em linguagem natural
     */
    std::string commandToNaturalLanguage(const std::string& command);
    
    /**
     * @brief Carrega a base de conhecimento do WYD
     * @param knowledgeBasePath Caminho para a base de conhecimento
     * @return true se carregado com sucesso
     */
    bool loadWYDKnowledgeBase(const std::string& knowledgeBasePath);
    
    /**
     * @brief Responde a uma pergunta sobre o WYD usando a base de conhecimento
     * @param question Pergunta
     * @return Resposta baseada na base de conhecimento
     */
    std::string answerWYDQuestion(const std::string& question);

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referências externas
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    std::shared_ptr<WYDLanguageDatabase> m_languageDb;
    
    // Configuração
    WYDAssistantConfig m_config;
    
    // Estado atual
    AssistantType m_assistantType;
    std::atomic<bool> m_isProcessing;
    std::string m_systemPrompt;
    float m_temperature;
    int m_maxTokens;
    bool m_offlineMode;
    
    // Histórico
    std::vector<ChatMessage> m_chatHistory;
    std::mutex m_historyMutex;
    
    // Callbacks registrados
    std::map<int, std::pair<std::string, std::function<bool(const RecognizedCommand&)>>> m_commandCallbacks;
    std::mutex m_callbacksMutex;
    int m_nextCallbackId;
    
    // Processamento assíncrono
    std::atomic<bool> m_cancelRequested;
    std::queue<std::function<void()>> m_taskQueue;
    std::mutex m_taskMutex;
    std::condition_variable m_taskCondition;
    std::thread m_processingThread;
    
    // Estatísticas
    struct UsageStats {
        std::map<std::string, int> commandUsage;
        std::map<TermCategory, int> termCategoryUsage;
        std::map<std::string, int> commonTerms;
        int totalQueries;
        int successfulCommandRecognition;
        int failedCommandRecognition;
        std::chrono::system_clock::time_point lastReset;
    };
    
    UsageStats m_usageStats;
    std::mutex m_statsMutex;
    
    // Métodos auxiliares
    std::vector<ChatMessage> prepareContext(const std::string& input, const json& contextData);
    AssistantResponse callLLM(const std::vector<ChatMessage>& context);
    AssistantResponse generateOfflineResponse(const std::vector<ChatMessage>& context);
    AssistantResponse parseResponse(const std::string& response);
    void processTask();
    void startProcessingThread();
    void stopProcessingThread();
    void updateUsageStats(const std::string& input, const AssistantResponse& response);
    std::string getSystemPromptForType(AssistantType type);
    bool executeAdminCommand(const std::string& command, const std::map<std::string, std::string>& parameters, std::string& result);
    bool executeGMCommand(const std::string& command, const std::map<std::string, std::string>& parameters, std::string& result);
    bool executePlayerCommand(const std::string& command, const std::map<std::string, std::string>& parameters, std::string& result);
};

}} // namespace wydstudio::ai

#endif // WYDSTUDIO_WYD_ASSISTANT_INTEGRATION_H