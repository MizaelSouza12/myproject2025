#include "NLPProcessor.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>
#include <random>
#include <ctime>
#include <sstream>

namespace WYDStudio {

/**
 * @brief Classe de implementação privada para NLPProcessor
 */
class NLPProcessor::Impl {
public:
    Impl() : m_initialized(false) {
        // Seed para gerador de números aleatórios
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }
    
    ~Impl() = default;
    
    bool m_initialized;
    std::string m_modelsPath;
    
    // Mapeamento de intenções para exemplos de treinamento
    std::map<std::string, std::vector<std::string>> m_intentExamples;
    
    // Mapeamento de entidades para reconhecimento
    std::map<std::string, std::vector<std::string>> m_entityExamples;
    
    // Exemplos de respostas para diferentes tipos de consultas
    std::map<std::string, std::vector<std::string>> m_responseTemplates;
    
    // Função de log
    std::function<void(const std::string&)> m_logCallback;
    
    /**
     * @brief Registra uma mensagem no log
     * @param message Mensagem a ser registrada
     */
    void Log(const std::string& message) {
        if (m_logCallback) {
            m_logCallback(message);
        } else {
            std::cout << "[NLPProcessor] " << message << std::endl;
        }
    }
    
    /**
     * @brief Carrega exemplos de treinamento
     * @param modelsPath Caminho para os modelos
     * @return true se o carregamento foi bem-sucedido
     */
    bool LoadTrainingExamples(const std::string& modelsPath) {
        // Em uma implementação real, carregaríamos dados de arquivos
        // Nesta versão simplificada, usamos exemplos hardcoded
        
        // Exemplos para intenções relacionadas a itens
        m_intentExamples["adjust_item_price"] = {
            "alterar o preço da espada de aço para 1000",
            "mudar preço da poção de vida para 500",
            "ajustar o preço do pergaminho de teleporte para 750",
            "definir preço da armadura de couro em 1200",
            "mudar o valor da gema mágica para 2000"
        };
        
        // Exemplos para intenções relacionadas a guilds
        m_intentExamples["adjust_guild_tax"] = {
            "alterar a taxa de guild para 5%",
            "mudar imposto de guild para 10 por cento",
            "definir taxa de guild como 7%",
            "ajustar imposto de guild para 8 porcento",
            "configurar taxa de imposto das guildas para 3%"
        };
        
        // Exemplos para intenções de recarga de configurações
        m_intentExamples["reload_items"] = {
            "recarregar dados de itens",
            "fazer reload de itens",
            "atualizar dados de itens",
            "recarregar configuração de itens",
            "reload item database"
        };
        
        m_intentExamples["reload_guild_config"] = {
            "recarregar configuração de guilds",
            "fazer reload das configurações de guilda",
            "atualizar configurações de guild",
            "recarregar dados de guilds",
            "reload guild settings"
        };
        
        // Exemplos para entidades
        m_entityExamples["itemName"] = {
            "espada de aço", "poção de vida", "armadura de couro", 
            "gema mágica", "pergaminho de teleporte"
        };
        
        // Templates de respostas
        m_responseTemplates["adjust_item_price_success"] = {
            "O preço do item {itemName} foi alterado com sucesso para {newPrice} gold.",
            "Preço do {itemName} modificado para {newPrice} gold.",
            "O valor do item {itemName} agora é {newPrice} gold."
        };
        
        m_responseTemplates["adjust_guild_tax_success"] = {
            "A taxa de imposto das guilds foi alterada para {taxRate}%.",
            "O imposto de guild agora está definido como {taxRate}%.",
            "Taxa de guilds modificada com sucesso para {taxRate}%."
        };
        
        m_responseTemplates["reload_items_success"] = {
            "Dados de itens recarregados com sucesso.",
            "A base de dados de itens foi atualizada.",
            "Configurações de itens recarregadas com sucesso."
        };
        
        m_responseTemplates["reload_guild_config_success"] = {
            "Configurações de guild recarregadas com sucesso.",
            "Dados de guilds atualizados no servidor.",
            "Configurações de guilds foram recarregadas."
        };
        
        m_responseTemplates["error"] = {
            "Desculpe, ocorreu um erro ao processar sua solicitação: {error}",
            "Erro: {error}",
            "Não foi possível completar a ação: {error}"
        };
        
        m_responseTemplates["unknown_intent"] = {
            "Desculpe, não entendi o que você quer fazer.",
            "Não consegui identificar sua intenção. Pode reformular?",
            "Não compreendi o comando. Tente ser mais específico."
        };
        
        return true;
    }
    
    /**
     * @brief Obtém uma resposta para uma intenção e entidades
     * @param intent Intenção detectada
     * @param entities Entidades extraídas
     * @param success Indica se a ação foi bem-sucedida
     * @param errorMessage Mensagem de erro (se houver)
     * @return Resposta gerada
     */
    std::string GenerateResponseFromTemplate(const std::string& intent, 
                                           const std::map<std::string, std::string>& entities,
                                           bool success = true,
                                           const std::string& errorMessage = "") {
        // Obter o tipo de template a usar
        std::string templateType;
        
        if (!success) {
            templateType = "error";
        } else if (intent == "unknown") {
            templateType = "unknown_intent";
        } else {
            templateType = intent + "_success";
        }
        
        // Verificar se temos templates para este tipo
        if (m_responseTemplates.find(templateType) == m_responseTemplates.end() || 
            m_responseTemplates[templateType].empty()) {
            return "Não foi possível gerar uma resposta apropriada.";
        }
        
        // Escolher um template aleatório
        const auto& templates = m_responseTemplates[templateType];
        int index = std::rand() % templates.size();
        std::string response = templates[index];
        
        // Substituir placeholders por valores reais
        if (templateType == "error") {
            response = std::regex_replace(response, std::regex("\\{error\\}"), errorMessage);
        } else {
            // Substituir entidades
            for (const auto& entity : entities) {
                std::string placeholder = "\\{" + entity.first + "\\}";
                response = std::regex_replace(response, std::regex(placeholder), entity.second);
            }
        }
        
        return response;
    }
    
    /**
     * @brief Calcula a similaridade entre duas strings
     * @param str1 Primeira string
     * @param str2 Segunda string
     * @return Valor de similaridade (0.0 a 1.0)
     */
    float CalculateStringSimilarity(const std::string& str1, const std::string& str2) {
        // Implementação simplificada de similaridade baseada em palavras comuns
        std::istringstream iss1(str1);
        std::istringstream iss2(str2);
        
        std::vector<std::string> words1;
        std::vector<std::string> words2;
        std::string word;
        
        // Separar palavras da primeira string
        while (iss1 >> word) {
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            words1.push_back(word);
        }
        
        // Separar palavras da segunda string
        while (iss2 >> word) {
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            words2.push_back(word);
        }
        
        // Contar palavras comuns
        int common = 0;
        for (const auto& w1 : words1) {
            for (const auto& w2 : words2) {
                if (w1 == w2) {
                    common++;
                    break;
                }
            }
        }
        
        // Calcular similaridade como proporção de palavras comuns
        int totalWords = static_cast<int>(words1.size() + words2.size());
        if (totalWords == 0) return 0.0f;
        
        return static_cast<float>(2 * common) / static_cast<float>(totalWords);
    }
};

// Implementação da classe NLPProcessor

NLPProcessor::NLPProcessor() : m_pImpl(std::make_unique<Impl>()) {
}

NLPProcessor::~NLPProcessor() = default;

bool NLPProcessor::Initialize(const std::string& modelsPath) {
    try {
        if (m_pImpl->m_initialized) {
            return true; // Já inicializado
        }
        
        m_pImpl->Log("Inicializando processador de linguagem natural...");
        
        // Salvar caminho dos modelos
        m_pImpl->m_modelsPath = modelsPath;
        
        // Carregar exemplos de treinamento
        if (!m_pImpl->LoadTrainingExamples(modelsPath)) {
            m_pImpl->Log("Erro: Falha ao carregar exemplos de treinamento");
            return false;
        }
        
        m_pImpl->m_initialized = true;
        m_pImpl->Log("Processador de linguagem natural inicializado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao inicializar processador de linguagem natural: " + std::string(e.what()));
        return false;
    }
}

CommandIntent NLPProcessor::ParseCommand(const std::string& command) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Aviso: NLPProcessor não inicializado. Tentando inicializar...");
        if (!Initialize("models")) {
            CommandIntent result;
            result.intent = "unknown";
            result.confidence = 0.0f;
            return result;
        }
    }
    
    CommandIntent result;
    result.intent = "unknown";
    result.confidence = 0.0f;
    
    try {
        // Converter comando para minúsculas para comparação
        std::string lowerCommand = command;
        std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);
        
        // Detectar intenção pelo exemplo mais similar
        float bestSimilarity = 0.0f;
        std::string bestIntent = "unknown";
        
        for (const auto& intentExamples : m_pImpl->m_intentExamples) {
            const std::string& intent = intentExamples.first;
            const std::vector<std::string>& examples = intentExamples.second;
            
            for (const auto& example : examples) {
                float similarity = m_pImpl->CalculateStringSimilarity(lowerCommand, example);
                
                if (similarity > bestSimilarity) {
                    bestSimilarity = similarity;
                    bestIntent = intent;
                }
            }
        }
        
        // Se a similaridade for muito baixa, manter intent como "unknown"
        if (bestSimilarity < 0.3f) {
            result.intent = "unknown";
            result.confidence = bestSimilarity;
            return result;
        }
        
        // Definir intenção detectada
        result.intent = bestIntent;
        result.confidence = bestSimilarity;
        
        // Extrair entidades
        result.entities = ExtractEntities(command);
        
        // Extrair parâmetros
        result.parameters = ExtractParameters(command, bestIntent);
        
        return result;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao analisar comando: " + std::string(e.what()));
        return result;
    }
}

std::string NLPProcessor::GenerateResponse(const std::string& query, const std::string& context) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Aviso: NLPProcessor não inicializado. Tentando inicializar...");
        if (!Initialize("models")) {
            return "Desculpe, não posso gerar uma resposta porque o processador de linguagem natural não está inicializado.";
        }
    }
    
    try {
        // Analisar a consulta para determinar intenção e entidades
        CommandIntent intent = ParseCommand(query);
        
        // Verificar se a intenção foi reconhecida
        if (intent.intent == "unknown" || intent.confidence < 0.3f) {
            // Resposta para intenção desconhecida
            return m_pImpl->GenerateResponseFromTemplate("unknown", {});
        }
        
        // Converter entidades para formato de string
        std::map<std::string, std::string> entityStrings;
        for (const auto& entity : intent.entities) {
            entityStrings[entity.first] = entity.second;
        }
        
        // Gerar resposta baseada na intenção e entidades
        return m_pImpl->GenerateResponseFromTemplate(intent.intent, entityStrings, true);
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao gerar resposta: " + std::string(e.what()));
        return "Desculpe, ocorreu um erro ao gerar a resposta: " + std::string(e.what());
    }
}

std::map<std::string, std::string> NLPProcessor::ExtractEntities(const std::string& text) {
    std::map<std::string, std::string> entities;
    
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Aviso: NLPProcessor não inicializado");
        return entities;
    }
    
    try {
        std::string lowerText = text;
        std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
        
        // Extrair entidades por tipo
        
        // Extrair itemName
        for (const auto& item : m_pImpl->m_entityExamples["itemName"]) {
            if (lowerText.find(item) != std::string::npos) {
                entities["itemName"] = item;
                break;
            }
        }
        
        // Extrair números (preço, taxa, etc.)
        std::regex numberRegex("\\b(\\d+)\\b");
        std::smatch numberMatch;
        if (std::regex_search(lowerText, numberMatch, numberRegex)) {
            // Determinar o contexto do número
            std::string numberStr = numberMatch[1];
            int number = std::stoi(numberStr);
            
            if (lowerText.find("preço") != std::string::npos || 
                lowerText.find("valor") != std::string::npos ||
                lowerText.find("gold") != std::string::npos) {
                
                entities["newPrice"] = numberStr;
            }
            else if (lowerText.find("taxa") != std::string::npos || 
                     lowerText.find("imposto") != std::string::npos ||
                     lowerText.find("porcento") != std::string::npos ||
                     lowerText.find("por cento") != std::string::npos ||
                     lowerText.find("%") != std::string::npos) {
                
                entities["taxRate"] = numberStr;
            }
            // Outros tipos de números podem ser adicionados aqui
        }
        
        return entities;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao extrair entidades: " + std::string(e.what()));
        return entities;
    }
}

float NLPProcessor::AnalyzeSentiment(const std::string& text) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Aviso: NLPProcessor não inicializado");
        return 0.0f;
    }
    
    try {
        // Implementação simplificada de análise de sentimento
        // Em uma implementação real, usaríamos um modelo de ML
        
        std::string lowerText = text;
        std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
        
        // Palavras positivas
        std::vector<std::string> positiveWords = {
            "bom", "ótimo", "excelente", "perfeito", "incrível", "sensacional",
            "maravilhoso", "fantástico", "legal", "gosto", "melhor", "sucesso"
        };
        
        // Palavras negativas
        std::vector<std::string> negativeWords = {
            "ruim", "péssimo", "terrível", "horrível", "detesto", "odeio",
            "pior", "falha", "problema", "erro", "quebrado", "bug"
        };
        
        // Contar ocorrências
        int positiveCount = 0;
        int negativeCount = 0;
        
        std::istringstream iss(lowerText);
        std::string word;
        
        while (iss >> word) {
            // Verificar se é uma palavra positiva
            for (const auto& positiveWord : positiveWords) {
                if (word.find(positiveWord) != std::string::npos) {
                    positiveCount++;
                    break;
                }
            }
            
            // Verificar se é uma palavra negativa
            for (const auto& negativeWord : negativeWords) {
                if (word.find(negativeWord) != std::string::npos) {
                    negativeCount++;
                    break;
                }
            }
        }
        
        // Calcular score de sentimento (-1.0 a 1.0)
        int total = positiveCount + negativeCount;
        if (total == 0) return 0.0f; // Neutro
        
        return static_cast<float>(positiveCount - negativeCount) / static_cast<float>(total);
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao analisar sentimento: " + std::string(e.what()));
        return 0.0f;
    }
}

std::vector<std::string> NLPProcessor::ExtractParameters(const std::string& command, const std::string& intentTemplate) {
    std::vector<std::string> parameters;
    
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Aviso: NLPProcessor não inicializado");
        return parameters;
    }
    
    try {
        // Extrair entidades
        std::map<std::string, std::string> entities = ExtractEntities(command);
        
        // Converter entidades para parâmetros ordenados com base no template de intenção
        if (intentTemplate == "adjust_item_price") {
            // Para adjust_item_price, esperamos itemName e newPrice
            if (entities.find("itemName") != entities.end()) {
                parameters.push_back(entities["itemName"]);
            } else {
                parameters.push_back("");
            }
            
            if (entities.find("newPrice") != entities.end()) {
                parameters.push_back(entities["newPrice"]);
            } else {
                parameters.push_back("");
            }
        }
        else if (intentTemplate == "adjust_guild_tax") {
            // Para adjust_guild_tax, esperamos taxRate
            if (entities.find("taxRate") != entities.end()) {
                parameters.push_back(entities["taxRate"]);
            } else {
                parameters.push_back("");
            }
        }
        // Adicionar mais templates conforme necessário
        
        return parameters;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao extrair parâmetros: " + std::string(e.what()));
        return parameters;
    }
}

bool NLPProcessor::AddTrainingExample(const std::string& text, const std::string& intent, const std::map<std::string, std::string>& entities) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Aviso: NLPProcessor não inicializado. Tentando inicializar...");
        if (!Initialize("models")) {
            return false;
        }
    }
    
    try {
        // Adicionar exemplo à lista de exemplos para a intenção
        m_pImpl->m_intentExamples[intent].push_back(text);
        
        // Adicionar entidades relevantes
        for (const auto& entity : entities) {
            // Se a entidade não existir no mapa, criar uma lista vazia
            if (m_pImpl->m_entityExamples.find(entity.first) == m_pImpl->m_entityExamples.end()) {
                m_pImpl->m_entityExamples[entity.first] = std::vector<std::string>();
            }
            
            // Verificar se a entidade já existe na lista de exemplos
            auto& entityList = m_pImpl->m_entityExamples[entity.first];
            if (std::find(entityList.begin(), entityList.end(), entity.second) == entityList.end()) {
                entityList.push_back(entity.second);
            }
        }
        
        m_pImpl->Log("Exemplo de treinamento adicionado para intenção: " + intent);
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao adicionar exemplo de treinamento: " + std::string(e.what()));
        return false;
    }
}

bool NLPProcessor::PerformIncrementalTraining() {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Aviso: NLPProcessor não inicializado. Tentando inicializar...");
        if (!Initialize("models")) {
            return false;
        }
    }
    
    try {
        // Em uma implementação real, treinaríamos o modelo com os novos exemplos
        // Nesta versão simplificada, apenas logamos que o treinamento foi realizado
        
        m_pImpl->Log("Treinamento incremental realizado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao realizar treinamento incremental: " + std::string(e.what()));
        return false;
    }
}

std::string NLPProcessor::TranslateText(const std::string& text, const std::string& sourceLanguage, const std::string& targetLanguage) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Aviso: NLPProcessor não inicializado");
        return "Error: NLP Processor not initialized";
    }
    
    try {
        // Em uma implementação real, usaríamos um serviço de tradução
        // Nesta versão simplificada, retornamos o texto original com indicação de idiomas
        
        std::string result = "[" + sourceLanguage + " to " + targetLanguage + "] " + text;
        m_pImpl->Log("Tradução simulada de " + sourceLanguage + " para " + targetLanguage);
        return result;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao traduzir texto: " + std::string(e.what()));
        return "Error: " + std::string(e.what());
    }
}

void NLPProcessor::RegisterFeedbackCallback(std::function<void(const std::string&, bool)> callback) {
    // Este método registraria um callback para feedback de aprendizado
    // Por simplicidade, apenas logamos que o callback foi registrado
    m_pImpl->Log("Callback de feedback registrado");
}

} // namespace WYDStudio