#include "../include/LanguageTermsLoader.h"
#include "../include/NaturalLanguageProcessor.h"
#include "../include/WYDLanguageFactory.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace WYDStudio {

LanguageTermsLoader::LanguageTermsLoader() {}

LanguageTermsLoader::~LanguageTermsLoader() {}

bool LanguageTermsLoader::LoadTermsToProcessor(
    std::shared_ptr<NaturalLanguageProcessor> processor,
    const std::string& filePath,
    TermsLoadingProgressCallback progressCallback) {
    
    if (!processor) {
        std::cerr << "Erro: Processador nulo fornecido para carregar termos." << std::endl;
        return false;
    }
    
    // Verificar se o arquivo existe
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Erro: Não foi possível abrir o arquivo de termos: " << filePath << std::endl;
        return false;
    }
    
    try {
        // Carregar o arquivo JSON
        json termsData;
        file >> termsData;
        file.close();
        
        // Verificar estrutura básica
        if (!termsData.contains("terms") || !termsData["terms"].is_array()) {
            std::cerr << "Erro: Formato de arquivo de termos inválido." << std::endl;
            return false;
        }
        
        // Total de categorias para cálculo de progresso
        int totalCategories = termsData["terms"].size();
        int processedCategories = 0;
        
        // Processar cada categoria
        for (const auto& category : termsData["terms"]) {
            // Verificar campos necessários
            if (!category.contains("category") || !category.contains("terms") || 
                !category["terms"].is_array()) {
                std::cerr << "Aviso: Categoria mal formatada, pulando..." << std::endl;
                continue;
            }
            
            std::string categoryName = category["category"];
            TermCategory termCategory = StringToTermCategory(categoryName);
            
            // Total de termos na categoria atual
            int totalTermsInCategory = category["terms"].size();
            int processedTerms = 0;
            
            // Processar cada termo
            for (const auto& term : category["terms"]) {
                if (LoadTermToProcessor(processor, term, termCategory)) {
                    processedTerms++;
                }
                
                // Atualizar progresso se callback fornecido
                if (progressCallback) {
                    float categoryProgress = static_cast<float>(processedTerms) / totalTermsInCategory;
                    float overallProgress = (static_cast<float>(processedCategories) / totalCategories) +
                                           (categoryProgress / totalCategories);
                    
                    std::stringstream progressMessage;
                    progressMessage << "Carregando termos da categoria " << categoryName 
                                   << " (" << processedTerms << "/" << totalTermsInCategory << ")";
                    
                    progressCallback(overallProgress, progressMessage.str());
                }
            }
            
            processedCategories++;
        }
        
        // Progresso final
        if (progressCallback) {
            progressCallback(1.0f, "Carregamento de termos concluído");
        }
        
        return true;
    }
    catch (const json::exception& e) {
        std::cerr << "Erro ao processar JSON: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro ao carregar termos: " << e.what() << std::endl;
        return false;
    }
}

bool LanguageTermsLoader::LoadTermToProcessor(
    std::shared_ptr<NaturalLanguageProcessor> processor,
    const json& termData,
    TermCategory category) {
    
    try {
        // Verificar campos obrigatórios
        if (!termData.contains("standardForm") || !termData.contains("synonyms") || 
            !termData.contains("description")) {
            return false;
        }
        
        // Extrair dados básicos
        std::string standardForm = termData["standardForm"];
        std::vector<std::string> synonyms = termData["synonyms"].get<std::vector<std::string>>();
        std::string description = termData["description"];
        
        // Criar estrutura de termo
        LanguageTerm term;
        term.standardForm = standardForm;
        term.synonyms = synonyms;
        term.description = description;
        term.category = category;
        
        // Adicionar exemplos de uso, se disponíveis
        if (termData.contains("usageExamples") && termData["usageExamples"].is_array()) {
            term.usageExamples = termData["usageExamples"].get<std::vector<std::string>>();
        }
        
        // Adicionar metadados, se disponíveis
        if (termData.contains("metaData") && termData["metaData"].is_object()) {
            for (auto& [key, value] : termData["metaData"].items()) {
                if (value.is_string()) {
                    term.metaData[key] = value.get<std::string>();
                }
            }
        }
        
        // Adicionar ao processador
        return processor->AddTerm(term);
    }
    catch (const json::exception& e) {
        std::cerr << "Erro ao processar termo: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro ao carregar termo: " << e.what() << std::endl;
        return false;
    }
}

TermCategory LanguageTermsLoader::StringToTermCategory(const std::string& categoryStr) {
    static const std::unordered_map<std::string, TermCategory> categoryMap = {
        {"ITEM", TermCategory::ITEM},
        {"CLASS", TermCategory::CLASS},
        {"SKILL", TermCategory::SKILL},
        {"LOCATION", TermCategory::LOCATION},
        {"MOB", TermCategory::MOB},
        {"QUEST", TermCategory::QUEST},
        {"ATTRIBUTE", TermCategory::ATTRIBUTE},
        {"ENHANCEMENT", TermCategory::ENHANCEMENT},
        {"EVENT", TermCategory::EVENT},
        {"ECONOMY", TermCategory::ECONOMY},
        {"SOCIAL", TermCategory::SOCIAL},
        {"COMMAND", TermCategory::COMMAND},
        {"PVP", TermCategory::PVP},
        {"GENERAL_SLANG", TermCategory::GENERAL_SLANG}
    };
    
    auto it = categoryMap.find(categoryStr);
    if (it != categoryMap.end()) {
        return it->second;
    }
    
    // Categoria padrão se não encontrar correspondência
    return TermCategory::GENERAL_SLANG;
}

} // namespace WYDStudio