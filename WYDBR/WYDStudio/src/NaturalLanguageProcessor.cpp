#include "../include/NaturalLanguageProcessor.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <random>
#include <ctime>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace WYDStudio {

// Implementação da classe NaturalLanguageProcessor

NaturalLanguageProcessor::NaturalLanguageProcessor() {
    m_statistics.totalProcessedTexts = 0;
    m_statistics.successfulMatches = 0;
    m_statistics.failedMatches = 0;
    
    // Inicializar recursos padrão
    InitializeDefaultFeatures();
}

NaturalLanguageProcessor::~NaturalLanguageProcessor() {
    // Salvar estatísticas antes de encerrar, se necessário
    try {
        ExportUsageStatistics("./nlp_statistics_final.json");
    } catch (...) {
        // Ignorar erros no destrutor
    }
}

bool NaturalLanguageProcessor::Initialize(const std::string& termsDatabasePath, 
                                        const std::string& patternsDatabasePath) {
    Log("Inicializando processador de linguagem natural...");
    
    bool termsLoaded = false;
    bool patternsLoaded = false;
    
    // Carregar banco de dados de termos
    if (!termsDatabasePath.empty()) {
        termsLoaded = LoadTermsDatabase(termsDatabasePath);
        if (!termsLoaded) {
            Log("Aviso: Falha ao carregar banco de dados de termos de " + termsDatabasePath);
        }
    }
    
    // Carregar banco de dados de padrões
    if (!patternsDatabasePath.empty()) {
        patternsLoaded = LoadPatternsDatabase(patternsDatabasePath);
        if (!patternsLoaded) {
            Log("Aviso: Falha ao carregar banco de dados de padrões de " + patternsDatabasePath);
        }
    }
    
    // Se os arquivos não existirem ou não puderem ser carregados, criar bancos padrão
    if (!termsLoaded) {
        Log("Criando banco de dados padrão de termos...");
        termsLoaded = CreateDefaultTermsDatabase();
    }
    
    if (!patternsLoaded) {
        Log("Criando banco de dados padrão de padrões...");
        patternsLoaded = CreateDefaultPatternsDatabase();
    }
    
    bool success = termsLoaded && patternsLoaded;
    Log(success ? "Processador de linguagem natural inicializado com sucesso." 
                : "Falha ao inicializar completamente o processador de linguagem natural.");
    
    return success;
}

RecognizedIntent NaturalLanguageProcessor::ProcessText(const std::string& text, 
                                                    const std::string& contextId) {
    std::lock_guard<std::mutex> statLock(m_statsMutex);
    m_statistics.totalProcessedTexts++;
    
    Log("Processando texto: '" + text + "'");
    
    // Normalizar o texto (converter termos para formas padrão)
    std::string normalizedText = NormalizeText(text);
    Log("Texto normalizado: '" + normalizedText + "'");
    
    // Analisar o contexto para melhorar o reconhecimento
    std::map<std::string, std::string> contextualInfo;
    if (!contextId.empty()) {
        contextualInfo = AnalyzeContext(contextId, normalizedText);
    }
    
    // Reconhecer intenções no texto normalizado
    std::vector<RecognizedIntent> intents = RecognizeIntents(normalizedText);
    
    // Se não encontrou nenhuma intenção, retornar intent vazia
    if (intents.empty()) {
        Log("Nenhuma intenção reconhecida.");
        RecognizedIntent emptyIntent;
        emptyIntent.intentName = "unknown";
        emptyIntent.confidence = 0.0f;
        emptyIntent.originalText = text;
        emptyIntent.normalizedText = normalizedText;
        
        m_statistics.failedMatches++;
        return emptyIntent;
    }
    
    // Usar a intenção com maior confiança
    RecognizedIntent bestIntent = intents[0];
    
    // Adicionar informações contextuais aos parâmetros da intenção
    for (auto& contextParam : contextualInfo) {
        if (bestIntent.params.find(contextParam.first) == bestIntent.params.end()) {
            bestIntent.params[contextParam.first] = contextParam.second;
        }
    }
    
    Log("Intenção reconhecida: " + bestIntent.intentName + 
        " (confiança: " + std::to_string(bestIntent.confidence) + ")");
    
    // Atualizar estatísticas
    UpdateStatistics(bestIntent);
    m_statistics.successfulMatches++;
    
    return bestIntent;
}

std::string NaturalLanguageProcessor::NormalizeText(const std::string& text) {
    if (text.empty()) return text;
    
    std::string result = text;
    
    // Converter para minúsculas
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    
    // Substituir termos específicos do WYD por suas formas padrão
    // Isso é para garantir consistência independente das gírias usadas
    auto terms = RecognizeTerms(result);
    
    // Ordenar por posição, de trás para frente para evitar problemas com índices
    std::vector<std::pair<size_t, std::shared_ptr<LanguageTerm>>> sortedTerms;
    for (const auto& pair : terms) {
        sortedTerms.push_back(pair);
    }
    
    std::sort(sortedTerms.begin(), sortedTerms.end(), 
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Substituir os termos de trás para frente
    for (const auto& [pos, term] : sortedTerms) {
        // Obter o texto original substituído
        std::string originalText;
        size_t i = pos;
        // Encontrar o fim do termo (uma palavra)
        while (i < result.size() && (std::isalnum(result[i]) || result[i] == '_')) {
            originalText += result[i];
            i++;
        }
        
        // Substituir pelo termo padrão
        if (!originalText.empty()) {
            result.replace(pos, originalText.size(), term->standardForm);
        }
    }
    
    // Normalizar espaços extras
    std::string normalizedSpaces;
    bool lastWasSpace = false;
    for (char c : result) {
        if (std::isspace(c)) {
            if (!lastWasSpace) {
                normalizedSpaces += ' ';
                lastWasSpace = true;
            }
        } else {
            normalizedSpaces += c;
            lastWasSpace = false;
        }
    }
    
    // Remover espaços no início e fim
    normalizedSpaces.erase(0, normalizedSpaces.find_first_not_of(' '));
    if (normalizedSpaces.find_last_not_of(' ') != std::string::npos)
        normalizedSpaces.erase(normalizedSpaces.find_last_not_of(' ') + 1);
    
    return normalizedSpaces;
}

bool NaturalLanguageProcessor::AddTerm(const LanguageTerm& term) {
    std::lock_guard<std::mutex> lock(m_termsMutex);
    
    // Verificar se o termo já existe
    for (const auto& existingTerm : m_terms) {
        if (existingTerm->standardForm == term.standardForm) {
            Log("Termo '" + term.standardForm + "' já existe no banco de dados.");
            return false;
        }
    }
    
    // Criar cópia do termo e adicionar
    auto newTerm = std::make_shared<LanguageTerm>(term);
    m_terms.push_back(newTerm);
    
    // Atualizar índices
    m_termLookup[newTerm->standardForm] = newTerm;
    for (const auto& synonym : newTerm->synonyms) {
        m_termLookup[synonym] = newTerm;
    }
    
    // Atualizar índice por categoria
    m_termsByCategory[newTerm->category].push_back(newTerm);
    
    Log("Termo '" + term.standardForm + "' adicionado ao banco de dados.");
    return true;
}

bool NaturalLanguageProcessor::AddPattern(const LanguagePattern& pattern) {
    std::lock_guard<std::mutex> lock(m_patternsMutex);
    
    // Verificar se o padrão já existe
    for (const auto& existingPattern : m_patterns) {
        if (existingPattern->id == pattern.id) {
            Log("Padrão com ID '" + pattern.id + "' já existe no banco de dados.");
            return false;
        }
    }
    
    // Verificar se o padrão regex é válido
    try {
        std::regex testRegex(pattern.regexPattern);
    } catch (const std::regex_error& e) {
        Log("Erro ao adicionar padrão: regex inválido - " + std::string(e.what()));
        return false;
    }
    
    // Criar cópia do padrão e adicionar
    auto newPattern = std::make_shared<LanguagePattern>(pattern);
    m_patterns.push_back(newPattern);
    
    // Atualizar índice
    m_patternLookup[newPattern->id] = newPattern;
    
    // Ordenar padrões por prioridade (maior primeiro)
    std::sort(m_patterns.begin(), m_patterns.end(),
        [](const auto& a, const auto& b) { return a->priority > b->priority; });
    
    Log("Padrão '" + pattern.id + "' adicionado ao banco de dados.");
    return true;
}

bool NaturalLanguageProcessor::RegisterIntentHandler(const IntentAction& intentAction) {
    if (intentAction.intentName.empty() || !intentAction.handler) {
        Log("Erro ao registrar handler: nome de intenção vazio ou handler inválido.");
        return false;
    }
    
    m_intentHandlers[intentAction.intentName] = intentAction;
    Log("Handler registrado para intenção '" + intentAction.intentName + "'");
    return true;
}

std::string NaturalLanguageProcessor::ProcessAndExecute(const std::string& text, 
                                                     const std::string& contextId) {
    // Processar o texto para obter a intenção
    RecognizedIntent intent = ProcessText(text, contextId);
    
    // Se não reconheceu uma intenção válida
    if (intent.intentName == "unknown" || intent.confidence < 0.4f) {
        return "Não entendi o que você quis dizer. Pode reformular?";
    }
    
    // Verificar se existe um handler para esta intenção
    auto it = m_intentHandlers.find(intent.intentName);
    if (it == m_intentHandlers.end()) {
        Log("Nenhum handler encontrado para intenção '" + intent.intentName + "'");
        return "Entendi o que você quer, mas não sei como executar esta ação.";
    }
    
    // Verificar se todos os parâmetros necessários estão presentes
    const IntentAction& action = it->second;
    if (action.requiresParameters) {
        bool missingParams = false;
        std::vector<std::string> missing;
        
        for (const auto& requiredParam : action.requiredParams) {
            if (!intent.HasParam(requiredParam)) {
                missing.push_back(requiredParam);
                missingParams = true;
            }
        }
        
        if (missingParams) {
            std::string response = "Preciso de mais informações para executar esta ação. ";
            response += "Faltam os seguintes detalhes: ";
            for (size_t i = 0; i < missing.size(); ++i) {
                response += missing[i];
                if (i < missing.size() - 1) response += ", ";
            }
            return response;
        }
    }
    
    // Executar a ação
    try {
        std::string result = action.handler(intent);
        
        // Salvar na memória do contexto se houver ID de contexto
        if (!contextId.empty()) {
            NLPMemoryItem memoryItem;
            memoryItem.userInput = text;
            memoryItem.recognizedIntent = intent;
            memoryItem.systemResponse = result;
            memoryItem.timestamp = std::chrono::system_clock::now();
            
            AddToContextMemory(contextId, memoryItem);
        }
        
        return result;
    } catch (const std::exception& e) {
        Log("Erro ao executar ação para intenção '" + intent.intentName + "': " + e.what());
        return "Ocorreu um erro ao executar esta ação: " + std::string(e.what());
    } catch (...) {
        Log("Erro desconhecido ao executar ação para intenção '" + intent.intentName + "'");
        return "Ocorreu um erro desconhecido ao executar esta ação.";
    }
}

void NaturalLanguageProcessor::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_logCallback = logCallback;
}

void NaturalLanguageProcessor::EnableFeature(NLPFeature feature, bool enable) {
    m_enabledFeatures[feature] = enable;
    Log(std::string(enable ? "Habilitando" : "Desabilitando") + 
        " recurso de NLP #" + std::to_string(static_cast<int>(feature)));
}

bool NaturalLanguageProcessor::IsFeatureEnabled(NLPFeature feature) const {
    auto it = m_enabledFeatures.find(feature);
    return (it != m_enabledFeatures.end()) ? it->second : false;
}

void NaturalLanguageProcessor::AddToContextMemory(const std::string& contextId, 
                                               const NLPMemoryItem& memoryItem) {
    if (contextId.empty()) return;
    
    std::lock_guard<std::mutex> lock(m_memoryMutex);
    m_contextMemory[contextId].push_back(memoryItem);
    
    // Limitar tamanho da memória (manter últimos 20 itens)
    const size_t maxMemorySize = 20;
    if (m_contextMemory[contextId].size() > maxMemorySize) {
        // Remover os itens mais antigos
        m_contextMemory[contextId].erase(
            m_contextMemory[contextId].begin(), 
            m_contextMemory[contextId].begin() + (m_contextMemory[contextId].size() - maxMemorySize)
        );
    }
}

std::vector<NLPMemoryItem> NaturalLanguageProcessor::GetContextMemory(
    const std::string& contextId, int maxItems) {
    
    std::lock_guard<std::mutex> lock(m_memoryMutex);
    
    // Se o contextId não existe, retorna vetor vazio
    auto it = m_contextMemory.find(contextId);
    if (it == m_contextMemory.end()) {
        return {};
    }
    
    // Se maxItems é 0 ou maior que o tamanho disponível, retorna todos os itens
    if (maxItems <= 0 || maxItems >= static_cast<int>(it->second.size())) {
        return it->second;
    }
    
    // Caso contrário, retorna os últimos 'maxItems' itens
    return std::vector<NLPMemoryItem>(
        it->second.end() - maxItems,
        it->second.end()
    );
}

void NaturalLanguageProcessor::ClearContextMemory(const std::string& contextId) {
    std::lock_guard<std::mutex> lock(m_memoryMutex);
    
    if (contextId.empty()) {
        // Limpar toda a memória de contexto
        m_contextMemory.clear();
        Log("Toda a memória de contexto foi limpa.");
    } else {
        // Limpar apenas o contexto específico
        auto it = m_contextMemory.find(contextId);
        if (it != m_contextMemory.end()) {
            m_contextMemory.erase(it);
            Log("Memória do contexto '" + contextId + "' foi limpa.");
        }
    }
}

std::shared_ptr<LanguageTerm> NaturalLanguageProcessor::GetTerm(const std::string& termText) {
    std::lock_guard<std::mutex> lock(m_termsMutex);
    
    // Tentar encontrar o termo exato
    auto it = m_termLookup.find(termText);
    if (it != m_termLookup.end()) {
        return it->second;
    }
    
    // Se o recurso de correspondência aproximada estiver habilitado
    if (IsFeatureEnabled(NLPFeature::FUZZY_MATCHING)) {
        float bestScore = 0.0f;
        std::shared_ptr<LanguageTerm> bestMatch;
        
        // Procurar o melhor match aproximado
        for (const auto& term : m_terms) {
            // Verificar a forma padrão
            float score = FuzzyMatch(termText, term->standardForm);
            if (score > bestScore && score > 0.8f) {  // Threshold de 80% de similaridade
                bestScore = score;
                bestMatch = term;
            }
            
            // Verificar sinônimos
            for (const auto& synonym : term->synonyms) {
                score = FuzzyMatch(termText, synonym);
                if (score > bestScore && score > 0.8f) {
                    bestScore = score;
                    bestMatch = term;
                }
            }
        }
        
        return bestMatch;  // Pode ser nullptr se nenhum match for bom o suficiente
    }
    
    // Nenhum termo encontrado
    return nullptr;
}

std::vector<std::shared_ptr<LanguageTerm>> NaturalLanguageProcessor::GetTermsByCategory(
    TermCategory category) {
    
    std::lock_guard<std::mutex> lock(m_termsMutex);
    
    auto it = m_termsByCategory.find(category);
    if (it != m_termsByCategory.end()) {
        return it->second;
    }
    
    return {};  // Retorna vetor vazio se categoria não encontrada
}

std::shared_ptr<LanguagePattern> NaturalLanguageProcessor::GetPattern(const std::string& patternId) {
    std::lock_guard<std::mutex> lock(m_patternsMutex);
    
    auto it = m_patternLookup.find(patternId);
    if (it != m_patternLookup.end()) {
        return it->second;
    }
    
    return nullptr;  // Retorna nullptr se padrão não encontrado
}

std::vector<std::string> NaturalLanguageProcessor::GenerateContextualSuggestions(
    const std::string& contextId,
    const std::string& currentInput,
    int maxSuggestions) {
    
    std::vector<std::string> suggestions;
    
    // Se entrada atual estiver vazia, retornar sugestões genéricas
    if (currentInput.empty()) {
        // Sugestões baseadas em comandos comuns
        suggestions = {
            "Como criar um item para um jogador?",
            "Iniciar um evento de drop em trolls",
            "Preciso teleportar um jogador",
            "Mostrar status do servidor",
            "Quanto custa uma poção de vigor?"
        };
        
        return suggestions;
    }
    
    // Se temos um contexto, usar histórico para informar sugestões
    if (!contextId.empty()) {
        auto memoryItems = GetContextMemory(contextId, 5);  // Últimos 5 itens
        
        // Análise simplificada de contexto para sugestões
        // Na implementação real, isso seria mais sofisticado
        std::set<std::string> recentIntents;
        for (const auto& item : memoryItems) {
            recentIntents.insert(item.recognizedIntent.intentName);
        }
        
        // Sugerir com base em intenções recentes
        if (recentIntents.find("create_item") != recentIntents.end()) {
            suggestions.push_back("Criar um set D +11 para FM");
            suggestions.push_back("Gerar uma arma +10 para BM");
        }
        
        if (recentIntents.find("drop_event") != recentIntents.end()) {
            suggestions.push_back("Aumentar taxa de drop de joias em Noatun");
            suggestions.push_back("Terminar evento de drop atual");
        }
    }
    
    // Sugestões baseadas no input atual
    std::string lowerInput = currentInput;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(),
        [](unsigned char c) { return std::tolower(c); });
    
    if (lowerInput.find("item") != std::string::npos || 
        lowerInput.find("criar") != std::string::npos) {
        suggestions.push_back("Criar um set A +7 para guerreiro");
        suggestions.push_back("Gerar colar celestial +9");
    }
    
    if (lowerInput.find("teleport") != std::string::npos || 
        lowerInput.find("tp") != std::string::npos) {
        suggestions.push_back("Teleportar jogador para Armia");
        suggestions.push_back("TP para RockyVale");
    }
    
    if (lowerInput.find("ban") != std::string::npos || 
        lowerInput.find("banir") != std::string::npos) {
        suggestions.push_back("Banir jogador temporariamente");
        suggestions.push_back("Ban permanente no jogador");
    }
    
    // Limitar ao número máximo de sugestões
    if (static_cast<int>(suggestions.size()) > maxSuggestions) {
        suggestions.resize(maxSuggestions);
    }
    
    return suggestions;
}

bool NaturalLanguageProcessor::SaveTermsDatabase(const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(m_termsMutex);
    
    try {
        json termsJson = json::array();
        
        for (const auto& term : m_terms) {
            json termJson;
            termJson["standardForm"] = term->standardForm;
            termJson["synonyms"] = term->synonyms;
            termJson["description"] = term->description;
            termJson["category"] = static_cast<int>(term->category);
            termJson["usageExamples"] = term->usageExamples;
            termJson["metaData"] = term->metaData;
            termJson["isActive"] = term->isActive;
            
            termsJson.push_back(termJson);
        }
        
        std::ofstream outFile(outputPath);
        if (!outFile.is_open()) {
            Log("Erro ao abrir arquivo para salvar banco de dados de termos: " + outputPath);
            return false;
        }
        
        outFile << termsJson.dump(4);  // Pretty print com 4 espaços
        outFile.close();
        
        Log("Banco de dados de termos salvo em: " + outputPath);
        return true;
    } catch (const std::exception& e) {
        Log("Erro ao salvar banco de dados de termos: " + std::string(e.what()));
        return false;
    }
}

bool NaturalLanguageProcessor::SavePatternsDatabase(const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(m_patternsMutex);
    
    try {
        json patternsJson = json::array();
        
        for (const auto& pattern : m_patterns) {
            json patternJson;
            patternJson["id"] = pattern->id;
            patternJson["regexPattern"] = pattern->regexPattern;
            patternJson["intentName"] = pattern->intentName;
            patternJson["examples"] = pattern->examples;
            
            // Converter mapa de grupos de captura para array de objetos
            json captureGroups = json::array();
            for (const auto& [groupId, paramName] : pattern->captureGroups) {
                json groupJson;
                groupJson["groupId"] = groupId;
                groupJson["paramName"] = paramName;
                captureGroups.push_back(groupJson);
            }
            patternJson["captureGroups"] = captureGroups;
            
            patternJson["priority"] = pattern->priority;
            patternJson["isActive"] = pattern->isActive;
            
            patternsJson.push_back(patternJson);
        }
        
        std::ofstream outFile(outputPath);
        if (!outFile.is_open()) {
            Log("Erro ao abrir arquivo para salvar banco de dados de padrões: " + outputPath);
            return false;
        }
        
        outFile << patternsJson.dump(4);  // Pretty print com 4 espaços
        outFile.close();
        
        Log("Banco de dados de padrões salvo em: " + outputPath);
        return true;
    } catch (const std::exception& e) {
        Log("Erro ao salvar banco de dados de padrões: " + std::string(e.what()));
        return false;
    }
}

bool NaturalLanguageProcessor::ExportUsageStatistics(const std::string& outputPath) {
    std::lock_guard<std::mutex> statLock(m_statsMutex);
    
    try {
        json statsJson;
        statsJson["totalProcessedTexts"] = m_statistics.totalProcessedTexts;
        statsJson["successfulMatches"] = m_statistics.successfulMatches;
        statsJson["failedMatches"] = m_statistics.failedMatches;
        
        // Converter estatísticas de uso de termos
        json termStats = json::object();
        for (const auto& [term, count] : m_statistics.termUsageCount) {
            termStats[term] = count;
        }
        statsJson["termUsage"] = termStats;
        
        // Converter estatísticas de uso de padrões
        json patternStats = json::object();
        for (const auto& [pattern, count] : m_statistics.patternMatchCount) {
            patternStats[pattern] = count;
        }
        statsJson["patternUsage"] = patternStats;
        
        // Converter estatísticas de execução de intenções
        json intentStats = json::object();
        for (const auto& [intent, count] : m_statistics.intentExecutionCount) {
            intentStats[intent] = count;
        }
        statsJson["intentExecution"] = intentStats;
        
        // Adicionar timestamp de exportação
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);
        char timeBuffer[100];
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&nowTime));
        statsJson["exportTimestamp"] = timeBuffer;
        
        // Calcular métricas de sucesso
        double successRate = (m_statistics.totalProcessedTexts > 0) 
            ? (double)m_statistics.successfulMatches / m_statistics.totalProcessedTexts 
            : 0.0;
        statsJson["successRate"] = successRate;
        
        std::ofstream outFile(outputPath);
        if (!outFile.is_open()) {
            Log("Erro ao abrir arquivo para exportar estatísticas: " + outputPath);
            return false;
        }
        
        outFile << statsJson.dump(4);  // Pretty print com 4 espaços
        outFile.close();
        
        Log("Estatísticas de uso exportadas para: " + outputPath);
        return true;
    } catch (const std::exception& e) {
        Log("Erro ao exportar estatísticas: " + std::string(e.what()));
        return false;
    }
}

bool NaturalLanguageProcessor::LearnFromCorrection(
    const std::string& originalText,
    const std::string& correctionText,
    bool learnAsPattern) {
    
    Log("Aprendendo com correção: original='" + originalText + 
        "', correção='" + correctionText + "'");
    
    // Verificar se já temos uma intenção reconhecida para o texto original
    RecognizedIntent originalIntent = ProcessText(originalText);
    RecognizedIntent correctionIntent = ProcessText(correctionText);
    
    // Se o texto corrigido foi reconhecido com sucesso mas o original não
    if (correctionIntent.intentName != "unknown" && 
        (originalIntent.intentName == "unknown" || 
         originalIntent.confidence < correctionIntent.confidence)) {
        
        if (learnAsPattern) {
            // Criar um novo padrão baseado na correção
            LanguagePattern newPattern;
            newPattern.id = "learned_pattern_" + std::to_string(m_patterns.size() + 1);
            
            // Usar uma versão simplificada do texto original como regex
            // Na implementação real, usaríamos algoritmos sofisticados para gerar o regex
            std::string escapedOriginal = originalText;
            // Escapar caracteres especiais do regex
            const std::string specialChars = "[](){}.*+?^$\\|";
            for (char c : specialChars) {
                auto pos = escapedOriginal.find(c);
                while (pos != std::string::npos) {
                    escapedOriginal.insert(pos, "\\");
                    pos = escapedOriginal.find(c, pos + 2);
                }
            }
            
            // Criar um padrão mais flexível substituindo palavras por \w+
            std::stringstream regexBuilder;
            std::istringstream iss(escapedOriginal);
            std::string word;
            bool first = true;
            
            while (iss >> word) {
                if (!first) regexBuilder << "\\s+";
                first = false;
                
                // Checar se é uma palavra crucial ou uma variável
                bool isCrucial = false;
                for (const auto& param : correctionIntent.params) {
                    if (word.find(param.second) != std::string::npos) {
                        regexBuilder << "(\\w+)";  // Grupo de captura para parâmetro
                        isCrucial = true;
                        break;
                    }
                }
                
                if (!isCrucial) {
                    regexBuilder << word;
                }
            }
            
            newPattern.regexPattern = regexBuilder.str();
            newPattern.intentName = correctionIntent.intentName;
            newPattern.examples.push_back(originalText);
            newPattern.priority = 0.5f;  // Prioridade média para padrões aprendidos
            
            // Adicionar o novo padrão
            if (AddPattern(newPattern)) {
                Log("Novo padrão aprendido: id=" + newPattern.id + 
                    ", regex=" + newPattern.regexPattern);
                return true;
            }
        } else {
            // Se não estamos aprendendo como padrão, podemos adicionar
            // o termo original como sinônimo se for relevante
            
            // Extrair termos de ambos os textos
            auto originalTerms = RecognizeTerms(originalText);
            auto correctionTerms = RecognizeTerms(correctionText);
            
            // Se encontramos termos na correção mas não no original
            if (originalTerms.empty() && !correctionTerms.empty()) {
                for (const auto& [pos, term] : correctionTerms) {
                    // Extrair o termo do texto original que corresponde à posição
                    // Na implementação real, usaríamos análise mais sofisticada
                    std::string potentialSynonym = originalText;
                    
                    // Verificar se este termo já existe
                    if (GetTerm(potentialSynonym) == nullptr) {
                        // Adicionar como sinônimo ao termo existente
                        auto updatedTerm = std::make_shared<LanguageTerm>(*term);
                        updatedTerm->synonyms.push_back(potentialSynonym);
                        
                        // Substituir o termo existente pelo atualizado
                        std::lock_guard<std::mutex> lock(m_termsMutex);
                        for (auto& existingTerm : m_terms) {
                            if (existingTerm->standardForm == term->standardForm) {
                                existingTerm = updatedTerm;
                                
                                // Atualizar índices
                                m_termLookup[potentialSynonym] = existingTerm;
                                
                                Log("Aprendido novo sinônimo '" + potentialSynonym + 
                                    "' para o termo '" + term->standardForm + "'");
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    
    Log("Não foi possível aprender com esta correção.");
    return false;
}

void NaturalLanguageProcessor::Log(const std::string& message) {
    if (m_logCallback) {
        m_logCallback(message);
    }
}

std::vector<RecognizedIntent> NaturalLanguageProcessor::RecognizeIntents(
    const std::string& normalizedText) {
    
    std::vector<RecognizedIntent> recognizedIntents;
    
    std::lock_guard<std::mutex> lock(m_patternsMutex);
    
    // Testar cada padrão contra o texto normalizado
    for (const auto& pattern : m_patterns) {
        // Pular padrões inativos
        if (!pattern->isActive) continue;
        
        try {
            std::regex re(pattern->regexPattern, 
                          std::regex_constants::ECMAScript | 
                          std::regex_constants::icase);
            
            std::smatch match;
            if (std::regex_search(normalizedText, match, re)) {
                // Processar o match para extrair parâmetros
                RecognizedIntent intent = ProcessRegexMatch(
                    match, *pattern, normalizedText, normalizedText);
                
                // Adicionar à lista de intenções reconhecidas
                recognizedIntents.push_back(intent);
            }
        } catch (const std::regex_error& e) {
            Log("Erro ao avaliar padrão '" + pattern->id + "': " + e.what());
        }
    }
    
    // Ordenar intenções por confiança (maior primeiro)
    std::sort(recognizedIntents.begin(), recognizedIntents.end(),
        [](const auto& a, const auto& b) { return a.confidence > b.confidence; });
    
    return recognizedIntents;
}

std::map<size_t, std::shared_ptr<LanguageTerm>> NaturalLanguageProcessor::RecognizeTerms(
    const std::string& text) {
    
    std::map<size_t, std::shared_ptr<LanguageTerm>> recognizedTerms;
    std::lock_guard<std::mutex> lock(m_termsMutex);
    
    // Para cada termo no banco de dados
    for (const auto& term : m_terms) {
        // Pular termos inativos
        if (!term->isActive) continue;
        
        // Verificar a forma padrão
        auto pos = text.find(term->standardForm);
        while (pos != std::string::npos) {
            recognizedTerms[pos] = term;
            pos = text.find(term->standardForm, pos + 1);
        }
        
        // Verificar sinônimos
        for (const auto& synonym : term->synonyms) {
            pos = text.find(synonym);
            while (pos != std::string::npos) {
                recognizedTerms[pos] = term;
                pos = text.find(synonym, pos + 1);
            }
        }
    }
    
    return recognizedTerms;
}

RecognizedIntent NaturalLanguageProcessor::ProcessRegexMatch(
    const std::smatch& match,
    const LanguagePattern& pattern,
    const std::string& originalText,
    const std::string& normalizedText) {
    
    RecognizedIntent intent;
    intent.intentName = pattern.intentName;
    intent.confidence = 0.8f;  // Valor base de confiança para matches exatos
    intent.originalText = originalText;
    intent.normalizedText = normalizedText;
    intent.patternId = pattern.id;
    
    // Extrair parâmetros dos grupos de captura
    for (const auto& [groupId, paramName] : pattern.captureGroups) {
        // Verificar se o grupo está dentro dos limites
        if (groupId >= 0 && groupId < static_cast<int>(match.size())) {
            intent.params[paramName] = match[groupId].str();
        }
    }
    
    // Reconhecer termos específicos no texto
    auto terms = RecognizeTerms(normalizedText);
    for (const auto& [pos, term] : terms) {
        intent.recognizedTerms.push_back(term->standardForm);
    }
    
    // Ajustar confiança com base na qualidade do match
    if (match[0].str().length() < normalizedText.length() * 0.5) {
        // Se o match cobre menos da metade do texto, reduzir confiança
        intent.confidence *= 0.8f;
    }
    
    if (intent.recognizedTerms.size() > 2) {
        // Aumentar confiança se reconhecemos vários termos específicos
        intent.confidence = std::min(1.0f, intent.confidence * 1.2f);
    }
    
    return intent;
}

bool NaturalLanguageProcessor::LoadTermsDatabase(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_termsMutex);
    
    try {
        std::ifstream inFile(filePath);
        if (!inFile.is_open()) {
            Log("Não foi possível abrir o arquivo de banco de dados de termos: " + filePath);
            return false;
        }
        
        json termsJson;
        inFile >> termsJson;
        inFile.close();
        
        if (!termsJson.is_array()) {
            Log("Formato inválido no banco de dados de termos: não é um array");
            return false;
        }
        
        // Limpar dados existentes
        m_terms.clear();
        m_termLookup.clear();
        m_termsByCategory.clear();
        
        // Carregar termos do JSON
        for (const auto& termJson : termsJson) {
            std::shared_ptr<LanguageTerm> term = std::make_shared<LanguageTerm>();
            
            term->standardForm = termJson["standardForm"];
            term->synonyms = termJson["synonyms"].get<std::vector<std::string>>();
            term->description = termJson["description"];
            term->category = static_cast<TermCategory>(termJson["category"].get<int>());
            term->usageExamples = termJson["usageExamples"].get<std::vector<std::string>>();
            
            if (termJson.contains("metaData")) {
                term->metaData = termJson["metaData"].get<std::map<std::string, std::string>>();
            }
            
            if (termJson.contains("isActive")) {
                term->isActive = termJson["isActive"];
            } else {
                term->isActive = true;
            }
            
            // Adicionar termo à lista
            m_terms.push_back(term);
            
            // Atualizar índices
            m_termLookup[term->standardForm] = term;
            for (const auto& synonym : term->synonyms) {
                m_termLookup[synonym] = term;
            }
            
            // Atualizar índice por categoria
            m_termsByCategory[term->category].push_back(term);
        }
        
        Log("Carregados " + std::to_string(m_terms.size()) + " termos do banco de dados");
        return true;
    } catch (const std::exception& e) {
        Log("Erro ao carregar banco de dados de termos: " + std::string(e.what()));
        return false;
    }
}

bool NaturalLanguageProcessor::LoadPatternsDatabase(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_patternsMutex);
    
    try {
        std::ifstream inFile(filePath);
        if (!inFile.is_open()) {
            Log("Não foi possível abrir o arquivo de banco de dados de padrões: " + filePath);
            return false;
        }
        
        json patternsJson;
        inFile >> patternsJson;
        inFile.close();
        
        if (!patternsJson.is_array()) {
            Log("Formato inválido no banco de dados de padrões: não é um array");
            return false;
        }
        
        // Limpar dados existentes
        m_patterns.clear();
        m_patternLookup.clear();
        
        // Carregar padrões do JSON
        for (const auto& patternJson : patternsJson) {
            std::shared_ptr<LanguagePattern> pattern = std::make_shared<LanguagePattern>();
            
            pattern->id = patternJson["id"];
            pattern->regexPattern = patternJson["regexPattern"];
            pattern->intentName = patternJson["intentName"];
            pattern->examples = patternJson["examples"].get<std::vector<std::string>>();
            
            // Carregar grupos de captura
            if (patternJson.contains("captureGroups") && patternJson["captureGroups"].is_array()) {
                for (const auto& groupJson : patternJson["captureGroups"]) {
                    int groupId = groupJson["groupId"];
                    std::string paramName = groupJson["paramName"];
                    pattern->captureGroups[groupId] = paramName;
                }
            }
            
            pattern->priority = patternJson["priority"];
            
            if (patternJson.contains("isActive")) {
                pattern->isActive = patternJson["isActive"];
            } else {
                pattern->isActive = true;
            }
            
            // Testar se o regex é válido
            try {
                std::regex testRegex(pattern->regexPattern);
            } catch (const std::regex_error& e) {
                Log("Aviso: Padrão '" + pattern->id + "' tem regex inválido: " + e.what());
                pattern->isActive = false;  // Desativar padrão inválido
            }
            
            // Adicionar padrão à lista
            m_patterns.push_back(pattern);
            
            // Atualizar índice
            m_patternLookup[pattern->id] = pattern;
        }
        
        // Ordenar padrões por prioridade (maior primeiro)
        std::sort(m_patterns.begin(), m_patterns.end(),
            [](const auto& a, const auto& b) { return a->priority > b->priority; });
        
        Log("Carregados " + std::to_string(m_patterns.size()) + " padrões do banco de dados");
        return true;
    } catch (const std::exception& e) {
        Log("Erro ao carregar banco de dados de padrões: " + std::string(e.what()));
        return false;
    }
}

float NaturalLanguageProcessor::FuzzyMatch(const std::string& str1, const std::string& str2) {
    // Implementação simplificada da distância de Levenshtein normalizada
    // Na implementação real, usaríamos algoritmos mais sofisticados
    
    const size_t len1 = str1.size();
    const size_t len2 = str2.size();
    
    // Caso especial: strings vazias
    if (len1 == 0) return len2 == 0 ? 1.0f : 0.0f;
    if (len2 == 0) return 0.0f;
    
    // Matriz para distância de Levenshtein
    std::vector<std::vector<size_t>> distance(
        len1 + 1, std::vector<size_t>(len2 + 1, 0));
    
    // Inicializar primeira linha e coluna
    for (size_t i = 0; i <= len1; ++i) distance[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) distance[0][j] = j;
    
    // Preencher matriz
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            size_t cost = (str1[i-1] == str2[j-1]) ? 0 : 1;
            distance[i][j] = std::min(
                std::min(
                    distance[i-1][j] + 1,     // deleção
                    distance[i][j-1] + 1),    // inserção
                distance[i-1][j-1] + cost);   // substituição
        }
    }
    
    // Distância máxima possível
    size_t maxDistance = std::max(len1, len2);
    
    // Calcular similaridade como 1 - (distância normalizada)
    float similarity = 1.0f - static_cast<float>(distance[len1][len2]) / maxDistance;
    
    return similarity;
}

void NaturalLanguageProcessor::UpdateStatistics(const RecognizedIntent& intent) {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    // Atualizar contagem de uso de termos
    for (const auto& term : intent.recognizedTerms) {
        m_statistics.termUsageCount[term]++;
    }
    
    // Atualizar contagem de uso de padrões
    if (!intent.patternId.empty()) {
        m_statistics.patternMatchCount[intent.patternId]++;
    }
    
    // Atualizar contagem de execução de intenções
    if (!intent.intentName.empty() && intent.intentName != "unknown") {
        m_statistics.intentExecutionCount[intent.intentName]++;
    }
}

void NaturalLanguageProcessor::InitializeDefaultFeatures() {
    m_enabledFeatures[NLPFeature::TERM_RECOGNITION] = true;
    m_enabledFeatures[NLPFeature::INTENT_RECOGNITION] = true;
    m_enabledFeatures[NLPFeature::CONTEXTUAL_MEMORY] = true;
    m_enabledFeatures[NLPFeature::FUZZY_MATCHING] = true;
    m_enabledFeatures[NLPFeature::SPELL_CORRECTION] = false;  // Desativado por padrão
    m_enabledFeatures[NLPFeature::SEMANTIC_ANALYSIS] = false; // Desativado por padrão
    m_enabledFeatures[NLPFeature::SENTIMENT_ANALYSIS] = false; // Desativado por padrão
}

bool NaturalLanguageProcessor::CreateDefaultTermsDatabase() {
    // Criar banco de dados padrão baseado no documento TERMOS_LINGUAGEM_WYD.md
    // Na implementação real, faríamos parsing do arquivo
    // Aqui, vamos criar alguns termos manualmente para exemplo
    
    // Sets de armadura
    {
        LanguageTerm term;
        term.standardForm = "Armadura Sephira";
        term.synonyms = {"Set A", "Sephira", "A", "Armadura A", "Set Sephira"};
        term.description = "Set de armadura level 200 classe Sephira";
        term.category = TermCategory::ITEM;
        term.usageExamples = {
            "Preciso upar para conseguir usar set A",
            "Vendo set A +6 de huntress",
            "Quanto custa set A sem refinar?"
        };
        AddTerm(term);
    }
    
    {
        LanguageTerm term;
        term.standardForm = "Armadura Sapphira";
        term.synonyms = {"Set B", "Sapphira", "B", "Armadura B", "Set Sapphira"};
        term.description = "Set de armadura level 220-250 classe Sapphira";
        term.category = TermCategory::ITEM;
        term.usageExamples = {
            "Dropei uma calça set B ontem",
            "Vendo set B completo +10",
            "Quem tem set B para vender?"
        };
        AddTerm(term);
    }
    
    // Classes
    {
        LanguageTerm term;
        term.standardForm = "ForeMaster";
        term.synonyms = {"FM", "Fore", "Foema"};
        term.description = "Evolução avançada de guerreiro";
        term.category = TermCategory::CLASS;
        term.usageExamples = {
            "Procurando guild que aceite FM",
            "Meu foema está no level 400",
            "Qual a melhor build para FM?"
        };
        AddTerm(term);
    }
    
    {
        LanguageTerm term;
        term.standardForm = "BloodMaster";
        term.synonyms = {"BM", "Blood"};
        term.description = "Evolução avançada de mago";
        term.category = TermCategory::CLASS;
        term.usageExamples = {
            "Procurando guild que aceite BM",
            "Meu blood está no level 400",
            "Qual a melhor build para BM?"
        };
        AddTerm(term);
    }
    
    // Localizações
    {
        LanguageTerm term;
        term.standardForm = "Armia";
        term.synonyms = {"Armia City", "Cidade principal", "Arm"};
        term.description = "Cidade principal do jogo";
        term.category = TermCategory::LOCATION;
        term.usageExamples = {
            "Estou vendendo na armia leste",
            "Me encontra em armia",
            "Quem tá em arm agora?"
        };
        AddTerm(term);
    }
    
    // Comandos
    {
        LanguageTerm term;
        term.standardForm = "Teleportar";
        term.synonyms = {"TP", "Teleporte", "Teleportação", "Mover", "Teletransportar"};
        term.description = "Mover-se instantaneamente para outro local";
        term.category = TermCategory::COMMAND;
        term.usageExamples = {
            "Admin, pode me dar tp para armia?",
            "Preciso de teleporte para dungeon",
            "Como faço para teleportar para noatun?"
        };
        AddTerm(term);
    }
    
    Log("Banco de dados padrão de termos criado com " + std::to_string(m_terms.size()) + " termos");
    
    // Salvar para arquivo
    SaveTermsDatabase("./terms_database.json");
    
    return !m_terms.empty();
}

bool NaturalLanguageProcessor::CreateDefaultPatternsDatabase() {
    // Criar banco de dados padrão de padrões baseado no documento
    
    // Padrão para criar item
    {
        LanguagePattern pattern;
        pattern.id = "create_item_pattern";
        pattern.regexPattern = "(cria|criar|gera|gerar|faz|fazer|quero|queria|preciso|adiciona) (um|uma|umas|uns)? (set|armadura|arma|equip|) ([A-Da-d]|cl|anc|celestial|ancient|foema|divine|arc|hunt) \\+?(\\d+)? (de|para|da|do)? ?([a-z]+)?";
        pattern.intentName = "create_item";
        pattern.examples = {
            "cria um set D +11 de FM",
            "quero uma arma celestial +10 para BM",
            "gera um set A +9 de huntress"
        };
        pattern.captureGroups = {
            {1, "action"},     // Ação (criar, gerar, etc.)
            {4, "itemType"},   // Tipo de item (A, B, C, D, celestial, etc.)
            {5, "refineLevel"}, // Nível de refinamento
            {7, "playerClass"}  // Classe (FM, BM, etc.)
        };
        pattern.priority = 1.0f;
        AddPattern(pattern);
    }
    
    // Padrão para evento de drop
    {
        LanguagePattern pattern;
        pattern.id = "drop_event_pattern";
        pattern.regexPattern = "(inicia|iniciar|começa|começar|faz|fazer|criar|ativa|ativar) (um|uma)? (evento|drop|droprate|) (de|para|com)? ?(drop|chance|taxa|)? (de)? ?([a-z\\s]+) (em|nos|nas|para|no)? ?([a-z\\s]+)?";
        pattern.intentName = "drop_event";
        pattern.examples = {
            "inicia um evento de drop de poeira de lac em trolls",
            "começa um drop rate de armadura D em noatun",
            "criar um evento para pedras em masmorras"
        };
        pattern.captureGroups = {
            {1, "action"},      // Ação (iniciar, começar, etc.)
            {7, "itemName"},    // Nome do item
            {9, "targetLocation"} // Local alvo
        };
        pattern.priority = 0.9f;
        AddPattern(pattern);
    }
    
    // Padrão para gerenciamento de jogador
    {
        LanguagePattern pattern;
        pattern.id = "player_management_pattern";
        pattern.regexPattern = "(bane|ban|bani|banir|silencia|cala|silenciar|tp|teleporta|move|mover|teleportar|kick|expulsa|expulsar|kika|kikar) (o jogador|a conta|o char|o personagem|o nick|o)? ?([a-zA-Z0-9]+)";
        pattern.intentName = "player_management";
        pattern.examples = {
            "bane o jogador TestUser por 3 dias",
            "teleporta TestPlayer para RockyVale",
            "silencia o nick BadMouth por spam"
        };
        pattern.captureGroups = {
            {1, "action"},     // Ação (banir, teleportar, etc.)
            {3, "playerName"}  // Nome do jogador
        };
        pattern.priority = 0.8f;
        AddPattern(pattern);
    }
    
    // Padrão para broadcast
    {
        LanguagePattern pattern;
        pattern.id = "broadcast_pattern";
        pattern.regexPattern = "(anuncia|broadcast|avisa|notifica|manda mensagem|envia mensagem) (que|sobre|para todos|global|no server)? ?(.+)";
        pattern.intentName = "broadcast";
        pattern.examples = {
            "anuncia que haverá evento às 20h",
            "broadcast vamos ter invasão de trolls",
            "manda mensagem para todos sobre o novo update"
        };
        pattern.captureGroups = {
            {1, "action"},   // Ação (anunciar, broadcast, etc.)
            {3, "message"}   // Mensagem
        };
        pattern.priority = 0.7f;
        AddPattern(pattern);
    }
    
    Log("Banco de dados padrão de padrões criado com " + std::to_string(m_patterns.size()) + " padrões");
    
    // Salvar para arquivo
    SavePatternsDatabase("./patterns_database.json");
    
    return !m_patterns.empty();
}

std::map<std::string, std::string> NaturalLanguageProcessor::AnalyzeContext(
    const std::string& contextId,
    const std::string& normalizedText) {
    
    std::map<std::string, std::string> contextualInfo;
    
    if (!IsFeatureEnabled(NLPFeature::CONTEXTUAL_MEMORY) || contextId.empty()) {
        return contextualInfo;
    }
    
    // Obter histórico de memória para este contexto
    auto memoryItems = GetContextMemory(contextId, 5);  // Últimos 5 itens
    
    if (memoryItems.empty()) {
        return contextualInfo;
    }
    
    // Analisar itens de memória para extrair contexto relevante
    for (const auto& item : memoryItems) {
        // Se estamos falando sobre o mesmo assunto que antes
        if (normalizedText.find("ele") != std::string::npos || 
            normalizedText.find("ela") != std::string::npos || 
            normalizedText.find("isso") != std::string::npos) {
            
            // Verificar se há parâmetros relevantes na intenção anterior
            for (const auto& [paramName, paramValue] : item.recognizedIntent.params) {
                // Parâmetros que poderiam ser referenciados por pronomes
                if (paramName == "itemType" || 
                    paramName == "playerName" || 
                    paramName == "targetLocation") {
                    
                    // Adicionar ao contexto atual
                    contextualInfo[paramName] = paramValue;
                }
            }
        }
        
        // Se a intenção atual parece ser continuação da anterior
        if (item.recognizedIntent.intentName == "create_item" && 
            normalizedText.find("refina") != std::string::npos) {
            
            // Continuando a falar sobre o mesmo item
            if (item.recognizedIntent.HasParam("itemType")) {
                contextualInfo["itemType"] = item.recognizedIntent.GetParam("itemType");
            }
        }
    }
    
    return contextualInfo;
}

std::map<std::string, std::vector<std::string>> NaturalLanguageProcessor::ExtractNamedEntities(
    const std::string& text) {
    
    std::map<std::string, std::vector<std::string>> entities;
    
    // Extrair entidades por categoria usando termos reconhecidos
    std::map<size_t, std::shared_ptr<LanguageTerm>> terms = RecognizeTerms(text);
    
    // Agrupar por categoria
    for (const auto& [pos, term] : terms) {
        std::string categoryName;
        
        switch (term->category) {
            case TermCategory::ITEM:
                categoryName = "ITEM";
                break;
            case TermCategory::CLASS:
                categoryName = "CLASS";
                break;
            case TermCategory::LOCATION:
                categoryName = "LOCATION";
                break;
            case TermCategory::MOB:
                categoryName = "MOB";
                break;
            default:
                categoryName = "OTHER";
                break;
        }
        
        entities[categoryName].push_back(term->standardForm);
    }
    
    return entities;
}

// Implementação da classe WYDLanguageFactory

LanguageTerm WYDLanguageFactory::CreateItemTerm(
    const std::string& standardForm,
    const std::vector<std::string>& synonyms,
    const std::string& description) {
    
    LanguageTerm term;
    term.standardForm = standardForm;
    term.synonyms = synonyms;
    term.description = description;
    term.category = TermCategory::ITEM;
    
    return term;
}

LanguageTerm WYDLanguageFactory::CreateClassTerm(
    const std::string& standardForm,
    const std::vector<std::string>& synonyms,
    const std::string& description) {
    
    LanguageTerm term;
    term.standardForm = standardForm;
    term.synonyms = synonyms;
    term.description = description;
    term.category = TermCategory::CLASS;
    
    return term;
}

LanguagePattern WYDLanguageFactory::CreateCommandPattern(
    const std::string& id,
    const std::string& regexPattern,
    const std::string& intentName,
    const std::map<int, std::string>& captureGroups,
    float priority) {
    
    LanguagePattern pattern;
    pattern.id = id;
    pattern.regexPattern = regexPattern;
    pattern.intentName = intentName;
    pattern.captureGroups = captureGroups;
    pattern.priority = priority;
    
    return pattern;
}

} // namespace WYDStudio