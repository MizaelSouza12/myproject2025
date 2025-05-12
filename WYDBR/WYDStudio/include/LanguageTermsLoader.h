#pragma once

#include "NaturalLanguageProcessor.h"
#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace WYDStudio {

/**
 * @typedef ProgressCallback
 * @brief Tipo para callback de progresso
 */
using ProgressCallback = std::function<void(float progress, const std::string& status)>;

/**
 * @class LanguageTermsLoader
 * @brief Carregador de termos de linguagem específicos do WYD
 * 
 * Esta classe é responsável por carregar termos específicos do jogo WYD
 * de arquivos externos (JSON) e adicioná-los ao processador de linguagem natural.
 */
class LanguageTermsLoader {
public:
    LanguageTermsLoader();
    ~LanguageTermsLoader();
    
    /**
     * @brief Carrega termos de linguagem de um arquivo JSON
     * @param processor Processador de linguagem natural
     * @param jsonPath Caminho para o arquivo JSON
     * @param progressCallback Callback opcional para progresso
     * @return True se carregado com sucesso
     */
    bool LoadTermsToProcessor(
        std::shared_ptr<NaturalLanguageProcessor> processor,
        const std::string& jsonPath,
        ProgressCallback progressCallback = nullptr);
    
    /**
     * @brief Exporta termos do processador para um arquivo JSON
     * @param processor Processador de linguagem natural
     * @param jsonPath Caminho para o arquivo JSON
     * @return True se exportado com sucesso
     */
    bool ExportTermsFromProcessor(
        std::shared_ptr<NaturalLanguageProcessor> processor,
        const std::string& jsonPath);
    
    /**
     * @brief Obtém o caminho padrão para o arquivo de termos de linguagem
     * @return Caminho padrão para o arquivo de termos
     */
    static std::string GetDefaultTermsPath();

private:
    /**
     * @brief Converte string de categoria para enum TermCategory
     * @param categoryStr String de categoria
     * @return Enum de categoria
     */
    TermCategory StringToCategory(const std::string& categoryStr);
    
    /**
     * @brief Converte enum TermCategory para string
     * @param category Enum de categoria
     * @return String representando a categoria
     */
    std::string CategoryToString(TermCategory category);
    
    /**
     * @brief Verifica se o arquivo existe
     * @param filePath Caminho do arquivo
     * @return True se o arquivo existe
     */
    bool FileExists(const std::string& filePath);
};

} // namespace WYDStudio