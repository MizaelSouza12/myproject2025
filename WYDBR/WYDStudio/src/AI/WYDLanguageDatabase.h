#ifndef WYDSTUDIO_WYD_LANGUAGE_DATABASE_H
#define WYDSTUDIO_WYD_LANGUAGE_DATABASE_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <optional>
#include <functional>
#include <nlohmann/json.hpp>
#include "../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ai {

using json = nlohmann::json;

/**
 * @brief Categoria de termos de linguagem do WYD
 */
enum class TermCategory {
    ITEM,             // Itens, equipamentos
    CLASS,            // Classes de personagem
    SKILL,            // Habilidades
    LOCATION,         // Locais, mapas
    MOB,              // Monstros, NPCs
    QUEST,            // Missões
    ATTRIBUTE,        // Atributos de personagem/item
    ENHANCEMENT,      // Aprimoramentos, refinamentos
    EVENT,            // Eventos
    ECONOMY,          // Economia, comércio
    SOCIAL,           // Interação social, guildas
    COMMAND,          // Comandos do jogo
    PVP,              // PVP, duelos
    GENERAL_SLANG     // Gírias gerais do jogo
};

/**
 * @brief Termo da linguagem do WYD com suas variações
 */
struct LanguageTerm {
    std::string standardForm;       // Forma padrão/oficial do termo
    std::vector<std::string> synonyms; // Sinônimos e variações
    std::string description;        // Descrição do termo
    TermCategory category;          // Categoria do termo
    std::vector<std::string> relatedTerms; // Termos relacionados
    std::map<std::string, std::string> attributes; // Atributos específicos do termo
    std::string internalId;         // ID interno para referência (por exemplo, ID do item no jogo)
    std::vector<std::string> usageExamples; // Exemplos de uso em contextos reais de jogadores
};

/**
 * @brief Padrão de linguagem específico do WYD
 */
struct LanguagePattern {
    std::string pattern;            // Padrão de expressão (regex simplificado)
    std::string interpretation;     // Interpretação do padrão
    std::string intentMapping;      // Mapeamento para intenção/comando
    std::vector<std::string> examples; // Exemplos de frases que seguem o padrão
    std::map<std::string, std::string> parameterMappings; // Mapeamento de parâmetros
};

/**
 * @brief Intent reconhecido de uma entrada do usuário
 */
struct RecognizedIntent {
    std::string intent;            // Intenção principal
    float confidence;              // Confiança (0-1)
    std::map<std::string, std::string> parameters; // Parâmetros extraídos
    std::vector<std::string> alternativeIntents; // Intenções alternativas
    std::string originalInput;     // Entrada original do usuário
};

/**
 * @brief Banco de dados de linguagem do WYD
 */
class WYDLanguageDatabase {
public:
    /**
     * @brief Construtor
     */
    WYDLanguageDatabase();
    
    /**
     * @brief Destrutor
     */
    ~WYDLanguageDatabase();
    
    /**
     * @brief Inicializa o banco de dados
     * @param facade Referência ao facade do WYDStudio
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Carrega o banco de dados de um arquivo
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool loadFromFile(const std::string& filePath);
    
    /**
     * @brief Salva o banco de dados em um arquivo
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool saveToFile(const std::string& filePath);
    
    /**
     * @brief Adiciona um termo ao banco de dados
     * @param term Termo a ser adicionado
     * @return true se adicionado com sucesso
     */
    bool addTerm(const LanguageTerm& term);
    
    /**
     * @brief Atualiza um termo existente
     * @param standardForm Forma padrão do termo
     * @param term Novo termo
     * @return true se atualizado com sucesso
     */
    bool updateTerm(const std::string& standardForm, const LanguageTerm& term);
    
    /**
     * @brief Remove um termo
     * @param standardForm Forma padrão do termo
     * @return true se removido com sucesso
     */
    bool removeTerm(const std::string& standardForm);
    
    /**
     * @brief Obtém um termo pela forma padrão
     * @param standardForm Forma padrão do termo
     * @return Termo ou nulo se não encontrado
     */
    std::optional<LanguageTerm> getTermByStandardForm(const std::string& standardForm) const;
    
    /**
     * @brief Obtém um termo por uma de suas variações
     * @param variation Variação do termo
     * @return Termo ou nulo se não encontrado
     */
    std::optional<LanguageTerm> getTermBySynonym(const std::string& variation) const;
    
    /**
     * @brief Obtém termos por categoria
     * @param category Categoria dos termos
     * @return Lista de termos
     */
    std::vector<LanguageTerm> getTermsByCategory(TermCategory category) const;
    
    /**
     * @brief Adiciona um padrão de linguagem
     * @param pattern Padrão a ser adicionado
     * @return true se adicionado com sucesso
     */
    bool addPattern(const LanguagePattern& pattern);
    
    /**
     * @brief Atualiza um padrão existente
     * @param patternId ID do padrão
     * @param pattern Novo padrão
     * @return true se atualizado com sucesso
     */
    bool updatePattern(const std::string& patternId, const LanguagePattern& pattern);
    
    /**
     * @brief Remove um padrão
     * @param patternId ID do padrão
     * @return true se removido com sucesso
     */
    bool removePattern(const std::string& patternId);
    
    /**
     * @brief Obtém todos os padrões
     * @return Lista de padrões
     */
    std::vector<LanguagePattern> getAllPatterns() const;
    
    /**
     * @brief Reconhece a intenção de uma entrada do usuário
     * @param input Entrada do usuário
     * @return Intenção reconhecida
     */
    RecognizedIntent recognizeIntent(const std::string& input) const;
    
    /**
     * @brief Normaliza um termo para sua forma padrão
     * @param term Termo a ser normalizado
     * @return Forma padrão do termo ou o próprio termo se não reconhecido
     */
    std::string normalizeTerm(const std::string& term) const;
    
    /**
     * @brief Expande um termo para incluir suas variações
     * @param term Termo a ser expandido
     * @return Lista com o termo e suas variações
     */
    std::vector<std::string> expandTerm(const std::string& term) const;
    
    /**
     * @brief Verifica se um termo existe no banco de dados
     * @param term Termo a ser verificado
     * @return true se o termo existir
     */
    bool termExists(const std::string& term) const;
    
    /**
     * @brief Obtém uma descrição para um termo
     * @param term Termo
     * @return Descrição do termo ou string vazia se não encontrado
     */
    std::string getTermDescription(const std::string& term) const;
    
    /**
     * @brief Tokeniza uma entrada do usuário em termos reconhecíveis
     * @param input Entrada do usuário
     * @return Lista de tokens
     */
    std::vector<std::string> tokenize(const std::string& input) const;
    
    /**
     * @brief Treina o banco de dados com exemplos
     * @param examples Exemplos de entradas e suas intenções
     * @return true se treinado com sucesso
     */
    bool trainWithExamples(const std::map<std::string, std::string>& examples);
    
    /**
     * @brief Exporta o banco de dados para treinamento de IA
     * @param outputPath Caminho de saída
     * @return true se exportado com sucesso
     */
    bool exportForAITraining(const std::string& outputPath) const;
    
    /**
     * @brief Carrega termos padrão do WYD
     * @return true se carregado com sucesso
     */
    bool loadDefaultTerms();
    
    /**
     * @brief Carrega padrões padrão do WYD
     * @return true se carregado com sucesso
     */
    bool loadDefaultPatterns();
    
    /**
     * @brief Carrega dados do cliente do WYD
     * @param clientPath Caminho do cliente
     * @return true se carregado com sucesso
     */
    bool loadFromClientData(const std::string& clientPath);
    
    /**
     * @brief Procura termos no banco de dados
     * @param query Consulta de busca
     * @return Lista de termos encontrados
     */
    std::vector<LanguageTerm> searchTerms(const std::string& query) const;
    
    /**
     * @brief Verifica se uma frase contém termos do WYD
     * @param text Texto a ser verificado
     * @return Lista de termos encontrados com suas posições
     */
    std::vector<std::pair<std::string, size_t>> findWYDTerms(const std::string& text) const;
    
    /**
     * @brief Gera sugestões para uma entrada parcial
     * @param partialInput Entrada parcial
     * @param maxSuggestions Número máximo de sugestões
     * @return Lista de sugestões
     */
    std::vector<std::string> generateSuggestions(const std::string& partialInput, int maxSuggestions = 5) const;
    
    /**
     * @brief Gera variações de comandos para uma intenção
     * @param intent Intenção
     * @param parameters Parâmetros
     * @return Lista de comandos possíveis
     */
    std::vector<std::string> generateCommandVariations(const std::string& intent, 
                                                     const std::map<std::string, std::string>& parameters) const;

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Dados em memória
    std::map<std::string, LanguageTerm> m_termsByStandardForm;
    std::map<std::string, std::string> m_synonymToStandardMap;
    std::vector<LanguagePattern> m_patterns;
    
    // Índices para busca eficiente
    std::map<TermCategory, std::vector<std::string>> m_termsByCategory;
    std::set<std::string> m_allVariations;
    
    // Métodos auxiliares
    void buildIndices();
    bool parseTermFromJson(const json& jsonTerm, LanguageTerm& term);
    json termToJson(const LanguageTerm& term) const;
    bool parsePatternFromJson(const json& jsonPattern, LanguagePattern& pattern);
    json patternToJson(const LanguagePattern& pattern) const;
    TermCategory stringToCategory(const std::string& categoryStr) const;
    std::string categoryToString(TermCategory category) const;
    std::vector<std::string> splitInput(const std::string& input) const;
    std::vector<std::pair<std::string, float>> matchPatterns(const std::string& input) const;
    std::map<std::string, std::string> extractParameters(const std::string& input, const LanguagePattern& pattern) const;
};

}} // namespace wydstudio::ai

#endif // WYDSTUDIO_WYD_LANGUAGE_DATABASE_H