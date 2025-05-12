#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>
#include <set>

namespace WYDStudio {

/**
 * @enum TermCategory
 * @brief Categorias para termos de linguagem
 */
enum class TermCategory {
    UNKNOWN,
    ITEM,           // Equipamentos, itens consumíveis, etc.
    CLASS,          // Classes de personagens
    SKILL,          // Habilidades
    LOCATION,       // Locais, mapas
    MOB,            // Monstros e criaturas
    QUEST,          // Missões
    ATTRIBUTE,      // Atributos de personagem e itens
    ENHANCEMENT,    // Refinamentos, melhorias
    EVENT,          // Eventos especiais
    ECONOMY,        // Economia, comércio
    SOCIAL,         // Interação social, guildas
    COMMAND,        // Comandos administrativos
    PVP,            // Combate entre jogadores
    GENERAL_SLANG   // Gírias gerais do jogo
};

/**
 * @struct LanguageTerm
 * @brief Definição de um termo específico do jogo
 */
struct LanguageTerm {
    std::string standardForm;          // Forma oficial
    std::vector<std::string> synonyms; // Sinônimos e gírias
    std::string description;           // Descrição
    TermCategory category;             // Categoria
    std::vector<std::string> usageExamples; // Exemplos de uso
    std::map<std::string, std::string> metaData; // Metadados adicionais
    bool isActive;                     // Status de ativação
};

/**
 * @struct RecognizedIntent
 * @brief Resultado do processamento de linguagem natural
 */
struct RecognizedIntent {
    std::string intentName;            // Nome da intenção identificada
    float confidence;                  // Confiança no reconhecimento (0.0-1.0)
    std::map<std::string, std::string> params; // Parâmetros extraídos
    std::vector<std::string> recognizedTerms; // Termos reconhecidos
    std::string originalText;          // Texto original
    std::string normalizedText;        // Texto normalizado
    std::string contextId;             // ID do contexto relacionado
    
    // Métodos de conveniência
    bool HasParam(const std::string& key) const {
        return params.find(key) != params.end();
    }
    
    std::string GetParam(const std::string& key, const std::string& defaultValue = "") const {
        auto it = params.find(key);
        return it != params.end() ? it->second : defaultValue;
    }
};

/**
 * @struct IntentHandlerInfo
 * @brief Informações do manipulador de intenção
 */
struct IntentHandlerInfo {
    std::string intentName;
    std::function<std::string(const RecognizedIntent&)> handler;
    bool requiresParameters;
    bool adminOnly;
};

/**
 * @typedef LogCallback
 * @brief Tipo para callback de log
 */
using LogCallback = std::function<void(const std::string&)>;

/**
 * @class NaturalLanguageProcessor
 * @brief Processador de linguagem natural específico para WYD
 */
class NaturalLanguageProcessor {
public:
    NaturalLanguageProcessor();
    ~NaturalLanguageProcessor();
    
    /**
     * @brief Inicializa o processador
     * @return True se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Define callback para logs
     * @param callback Função de callback
     */
    void SetLogCallback(LogCallback callback);
    
    /**
     * @brief Normaliza texto substituindo gírias por formas padrão
     * @param text Texto de entrada
     * @return Texto normalizado
     */
    std::string NormalizeText(const std::string& text);
    
    /**
     * @brief Processa texto para identificar intenção
     * @param text Texto de entrada
     * @param contextId ID de contexto opcional para processamento contextual
     * @return Intenção reconhecida
     */
    RecognizedIntent ProcessText(const std::string& text, const std::string& contextId = "");
    
    /**
     * @brief Adiciona um termo ao processador
     * @param term Termo a ser adicionado
     * @return True se adicionado com sucesso
     */
    bool AddTerm(const LanguageTerm& term);
    
    /**
     * @brief Remove um termo do processador
     * @param standardForm Forma padrão do termo a remover
     * @return True se removido com sucesso
     */
    bool RemoveTerm(const std::string& standardForm);
    
    /**
     * @brief Obtém um termo pelo seu formulário padrão
     * @param standardForm Forma padrão do termo
     * @return Ponteiro para o termo ou nullptr se não encontrado
     */
    const LanguageTerm* GetTerm(const std::string& standardForm) const;
    
    /**
     * @brief Registra um manipulador de intenção
     * @param handler Informações do manipulador
     * @return True se registrado com sucesso
     */
    bool RegisterIntentHandler(const IntentHandlerInfo& handler);
    
    /**
     * @brief Remove um manipulador de intenção
     * @param intentName Nome da intenção
     * @return True se removido com sucesso
     */
    bool UnregisterIntentHandler(const std::string& intentName);
    
    /**
     * @brief Exporta estatísticas de uso
     * @param filePath Caminho do arquivo para salvar
     * @return True se exportado com sucesso
     */
    bool ExportUsageStatistics(const std::string& filePath) const;
    
    /**
     * @brief Verifica se um texto contém termos específicos de uma categoria
     * @param text Texto a verificar
     * @param category Categoria a verificar
     * @return True se contém termos da categoria
     */
    bool ContainsCategoryTerms(const std::string& text, TermCategory category) const;
    
    /**
     * @brief Extrai termos específicos de um texto
     * @param text Texto a processar
     * @return Lista de termos reconhecidos
     */
    std::vector<std::string> ExtractTerms(const std::string& text) const;
    
    /**
     * @brief Gera sugestões contextuais com base na entrada atual
     * @param contextId ID de contexto
     * @param currentInput Entrada atual
     * @param maxSuggestions Número máximo de sugestões
     * @return Lista de sugestões
     */
    std::vector<std::string> GenerateContextualSuggestions(
        const std::string& contextId,
        const std::string& currentInput,
        int maxSuggestions = 5);
    
    /**
     * @brief Limpa o contexto de conversa para um ID específico
     * @param contextId ID do contexto a limpar
     */
    void ClearContext(const std::string& contextId);
    
private:
    // Tipos internos
    struct ConversationContext;
    struct PatternMatcher;
    struct IntentPattern;
    struct UsageStatistics;
    
    // Armazenamento de termos
    std::unordered_map<std::string, LanguageTerm> m_terms;
    std::unordered_map<std::string, std::string> m_synonymToStandard;
    std::unordered_map<TermCategory, std::set<std::string>> m_termsByCategory;
    
    // Manipuladores de intenções
    std::unordered_map<std::string, IntentHandlerInfo> m_intentHandlers;
    
    // Contextos de conversa
    std::unordered_map<std::string, std::shared_ptr<ConversationContext>> m_contexts;
    
    // Padrões de reconhecimento
    std::vector<std::shared_ptr<IntentPattern>> m_intentPatterns;
    
    // Estatísticas de uso
    std::shared_ptr<UsageStatistics> m_statistics;
    
    // Callbacks
    LogCallback m_logCallback;
    
    // Implementações privadas
    void Log(const std::string& message);
    RecognizedIntent MatchIntentPattern(const std::string& normalizedText, const std::string& originalText, const std::string& contextId);
    void UpdateContext(const std::string& contextId, const RecognizedIntent& intent);
    void InitializeDefaultPatterns();
    std::string ExtractEntityValue(const std::string& text, const std::string& entityType) const;
    bool MatchPattern(const std::string& text, const std::string& pattern) const;
    void TrackUsage(const RecognizedIntent& intent);
};

} // namespace WYDStudio