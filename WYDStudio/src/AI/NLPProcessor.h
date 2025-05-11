#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace WYDStudio {

/**
 * @struct CommandIntent
 * @brief Estrutura que representa a intenção extraída de um comando
 */
struct CommandIntent {
    std::string intent;                           // Intenção principal
    std::map<std::string, std::string> entities;  // Entidades extraídas
    float confidence;                             // Confiança na classificação
    std::vector<std::string> parameters;          // Parâmetros extraídos
};

/**
 * @class NLPProcessor
 * @brief Processador de linguagem natural para interpretar comandos
 * 
 * Esta classe é responsável por analisar textos em linguagem natural,
 * extrair intenções e entidades, e gerar respostas contextuais.
 */
class NLPProcessor {
public:
    /**
     * @brief Construtor padrão
     */
    NLPProcessor();
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~NLPProcessor();
    
    /**
     * @brief Inicializa o processador com modelos de IA
     * @param modelsPath Caminho para os modelos de IA
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& modelsPath);
    
    /**
     * @brief Analisa um comando para extrair sua intenção
     * @param command Comando em linguagem natural
     * @return Intenção extraída do comando
     */
    CommandIntent ParseCommand(const std::string& command);
    
    /**
     * @brief Gera uma resposta para uma consulta
     * @param query Consulta em linguagem natural
     * @param context Contexto atual (opcional)
     * @return Resposta gerada
     */
    std::string GenerateResponse(const std::string& query, 
                                const std::string& context = "");
    
    /**
     * @brief Extrai entidades de um texto
     * @param text Texto a ser analisado
     * @return Mapa de entidades extraídas (tipo -> valor)
     */
    std::map<std::string, std::string> ExtractEntities(const std::string& text);
    
    /**
     * @brief Classifica o sentimento de um texto
     * @param text Texto a ser analisado
     * @return Valor de sentimento (-1.0 a 1.0, negativo a positivo)
     */
    float AnalyzeSentiment(const std::string& text);
    
    /**
     * @brief Extrai parâmetros de um comando baseado em um template
     * @param command Comando em linguagem natural
     * @param intentTemplate Template da intenção
     * @return Vetor de parâmetros extraídos
     */
    std::vector<std::string> ExtractParameters(const std::string& command, 
                                             const std::string& intentTemplate);
    
    /**
     * @brief Adiciona um exemplo para treinamento contínuo
     * @param text Texto de exemplo
     * @param intent Intenção correspondente
     * @param entities Entidades presentes no exemplo
     * @return true se o exemplo foi adicionado com sucesso
     */
    bool AddTrainingExample(const std::string& text, 
                          const std::string& intent,
                          const std::map<std::string, std::string>& entities);
    
    /**
     * @brief Realiza um treinamento incremental com novos exemplos
     * @return true se o treinamento foi bem-sucedido
     */
    bool PerformIncrementalTraining();
    
    /**
     * @brief Traduz texto entre idiomas
     * @param text Texto a ser traduzido
     * @param sourceLanguage Idioma de origem
     * @param targetLanguage Idioma de destino
     * @return Texto traduzido
     */
    std::string TranslateText(const std::string& text,
                             const std::string& sourceLanguage,
                             const std::string& targetLanguage);
    
    /**
     * @brief Registra um callback para feedback de aprendizado
     * @param callback Função a ser chamada para feedback
     */
    void RegisterFeedbackCallback(std::function<void(const std::string&, bool)> callback);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
};

} // namespace WYDStudio