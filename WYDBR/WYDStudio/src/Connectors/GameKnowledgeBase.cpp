#include "../../include/GameKnowledgeBase.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <thread>
#include <chrono>
#include <sstream>
#include <filesystem>
#include <json/json.hpp> // Usando nlohmann/json para C++

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace WYDStudio {

class GameKnowledgeBase::Impl {
public:
    Impl(std::shared_ptr<FileSystemConnector> fsConnector) 
        : m_fsConnector(fsConnector)
        , m_initialized(false)
        , m_totalFragments(0)
        , m_analyzeRunning(false) {
    }
    
    ~Impl() = default;
    
    // Caminhos para diferentes partes do jogo
    std::string m_gamePath;
    std::string m_sourcesPath;
    std::string m_databasePath;
    
    // Repositório de conhecimento
    std::vector<KnowledgeFragment> m_knowledgeFragments;
    std::map<KnowledgeDomain, std::vector<size_t>> m_domainIndices;
    std::map<std::string, std::vector<size_t>> m_textSearchIndex;
    
    // Status e estatísticas
    bool m_initialized;
    std::atomic<size_t> m_totalFragments;
    std::atomic<bool> m_analyzeRunning;
    
    // Sincronização
    std::mutex m_fragmentsMutex;
    std::mutex m_indexMutex;
    
    // Compartilhado
    std::shared_ptr<FileSystemConnector> m_fsConnector;
    
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
            std::cout << "[GameKnowledgeBase] " << message << std::endl;
        }
    }
    
    /**
     * @brief Analisa uma árvore de diretórios em busca de arquivos relevantes
     * @param rootPath Diretório raiz para análise
     * @param progressCallback Callback para reportar progresso
     * @param totalItems Total estimado de itens a serem processados
     * @param processedItems Contador de itens processados
     */
    void AnalyzeDirectory(const std::string& rootPath, 
                         std::function<void(float, const std::string&)> progressCallback,
                         size_t totalItems,
                         std::atomic<size_t>& processedItems) {
        try {
            if (!fs::exists(rootPath)) {
                Log("Aviso: Diretório não existe: " + rootPath);
                return;
            }
            
            for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
                if (!m_analyzeRunning) break; // Verificar se análise foi cancelada
                
                if (entry.is_regular_file()) {
                    // Obter caminho e extensão do arquivo
                    std::string filePath = entry.path().string();
                    std::string extension = entry.path().extension().string();
                    std::string fileName = entry.path().filename().string();
                    
                    // Atualizar progresso
                    processedItems++;
                    float progress = static_cast<float>(processedItems) / static_cast<float>(totalItems);
                    progressCallback(progress, "Analisando: " + fileName);
                    
                    // Processar com base na extensão do arquivo
                    if (extension == ".bin" || extension == ".dat") {
                        AnalyzeBinaryFile(filePath);
                    }
                    else if (extension == ".json") {
                        AnalyzeJsonFile(filePath);
                    }
                    else if (extension == ".cpp" || extension == ".h") {
                        AnalyzeSourceFile(filePath);
                    }
                    else if (extension == ".txt" || extension == ".md") {
                        AnalyzeTextFile(filePath);
                    }
                    else if (extension == ".csv") {
                        AnalyzeCsvFile(filePath);
                    }
                    // Adicionar outros tipos de arquivo conforme necessário
                }
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao analisar diretório " + rootPath + ": " + e.what());
        }
    }
    
    /**
     * @brief Analisa um arquivo binário do jogo
     * @param filePath Caminho para o arquivo
     */
    void AnalyzeBinaryFile(const std::string& filePath) {
        // Extrai informações básicas do nome do arquivo
        fs::path path(filePath);
        std::string fileName = path.filename().string();
        std::string extension = path.extension().string();
        
        // Determinar domínio com base no nome do arquivo
        KnowledgeDomain domain = DetermineFileDomain(fileName);
        
        // Criar fragmento com metadados básicos
        KnowledgeFragment fragment;
        fragment.id = "file:" + fileName;
        fragment.content = "Arquivo binário: " + fileName;
        fragment.source = filePath;
        fragment.domain = domain;
        fragment.confidence = 1.0f;
        fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        
        // Adicionar metadados
        fragment.metadata["tipo"] = "arquivo_binario";
        fragment.metadata["tamanho"] = std::to_string(fs::file_size(filePath));
        fragment.metadata["extensao"] = extension;
        
        // Tenta determinar o tipo específico de arquivo com base no nome
        if (fileName.find("item") != std::string::npos) {
            fragment.metadata["categoria"] = "itens";
            fragment.content += "\nContém dados de itens do jogo.";
        }
        else if (fileName.find("mob") != std::string::npos) {
            fragment.metadata["categoria"] = "mobs";
            fragment.content += "\nContém dados de monstros do jogo.";
        }
        else if (fileName.find("npc") != std::string::npos) {
            fragment.metadata["categoria"] = "npcs";
            fragment.content += "\nContém dados de NPCs do jogo.";
        }
        else if (fileName.find("map") != std::string::npos) {
            fragment.metadata["categoria"] = "mapas";
            fragment.content += "\nContém dados de mapas do jogo.";
        }
        else if (fileName.find("skill") != std::string::npos) {
            fragment.metadata["categoria"] = "habilidades";
            fragment.content += "\nContém dados de habilidades do jogo.";
        }
        
        // Em uma implementação real, aqui analisaríamos o conteúdo do arquivo binário
        // Por exemplo, se for um arquivo de item, extrairíamos informações sobre os itens
        
        // Adiciona o fragmento à base de conhecimento
        AddFragmentInternal(fragment);
    }
    
    /**
     * @brief Analisa um arquivo JSON do jogo
     * @param filePath Caminho para o arquivo
     */
    void AnalyzeJsonFile(const std::string& filePath) {
        try {
            // Ler o arquivo JSON
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo JSON: " + filePath);
                return;
            }
            
            // Extrair informações básicas do nome do arquivo
            fs::path path(filePath);
            std::string fileName = path.filename().string();
            
            // Determinar domínio com base no nome do arquivo
            KnowledgeDomain domain = DetermineFileDomain(fileName);
            
            // Ler o JSON
            json data;
            file >> data;
            file.close();
            
            // Processar o JSON com base no tipo de arquivo
            if (fileName.find("ItemList") != std::string::npos) {
                ProcessItemListJson(data, filePath);
            }
            else if (fileName.find("MobList") != std::string::npos) {
                ProcessMobListJson(data, filePath);
            }
            else if (fileName.find("NpcList") != std::string::npos) {
                ProcessNpcListJson(data, filePath);
            }
            else if (fileName.find("MapList") != std::string::npos) {
                ProcessMapListJson(data, filePath);
            }
            else if (fileName.find("SkillList") != std::string::npos) {
                ProcessSkillListJson(data, filePath);
            }
            else {
                // JSON genérico, criar um fragmento básico
                KnowledgeFragment fragment;
                fragment.id = "file:" + fileName;
                fragment.content = "Arquivo JSON: " + fileName;
                fragment.source = filePath;
                fragment.domain = domain;
                fragment.confidence = 1.0f;
                fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                // Adicionar metadados
                fragment.metadata["tipo"] = "arquivo_json";
                fragment.metadata["tamanho"] = std::to_string(fs::file_size(filePath));
                
                // Adicionar o fragmento
                AddFragmentInternal(fragment);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao analisar arquivo JSON " + filePath + ": " + e.what());
        }
    }
    
    /**
     * @brief Processa um arquivo JSON de lista de itens
     * @param data Dados JSON
     * @param filePath Caminho do arquivo
     */
    void ProcessItemListJson(const json& data, const std::string& filePath) {
        try {
            // Extrair cada item e criar um fragmento de conhecimento
            for (const auto& item : data.items()) {
                // Verificar se possui uma estrutura válida de item
                if (!item.value().is_object()) continue;
                
                // Extrair dados do item
                int itemId = -1;
                std::string itemName = "Item desconhecido";
                
                // Tentar extrair ID e nome
                if (item.value().contains("id")) {
                    itemId = item.value()["id"].get<int>();
                }
                else if (item.value().contains("Index")) {
                    itemId = item.value()["Index"].get<int>();
                }
                else if (item.value().contains("index")) {
                    itemId = item.value()["index"].get<int>();
                }
                
                if (item.value().contains("name")) {
                    itemName = item.value()["name"].get<std::string>();
                }
                else if (item.value().contains("Name")) {
                    itemName = item.value()["Name"].get<std::string>();
                }
                
                // Criar o fragmento
                KnowledgeFragment fragment;
                fragment.id = "item:" + std::to_string(itemId);
                fragment.content = "Item: " + itemName + " (ID: " + std::to_string(itemId) + ")";
                fragment.source = filePath;
                fragment.domain = KnowledgeDomain::ITEMS;
                fragment.confidence = 1.0f;
                fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                // Adicionar metadados
                fragment.metadata["tipo"] = "item";
                fragment.metadata["id"] = std::to_string(itemId);
                fragment.metadata["nome"] = itemName;
                
                // Adicionar atributos do item se disponíveis
                for (const auto& attr : item.value().items()) {
                    if (attr.key() != "id" && attr.key() != "name" && 
                        attr.key() != "Index" && attr.key() != "Name") {
                        
                        // Converter o valor para string
                        std::string valueStr;
                        if (attr.value().is_string()) {
                            valueStr = attr.value().get<std::string>();
                        }
                        else if (attr.value().is_number()) {
                            valueStr = std::to_string(attr.value().get<int>());
                        }
                        else if (attr.value().is_boolean()) {
                            valueStr = attr.value().get<bool>() ? "true" : "false";
                        }
                        else {
                            valueStr = "complexo";
                        }
                        
                        // Adicionar ao conteúdo e metadados
                        fragment.content += "\n" + attr.key() + ": " + valueStr;
                        fragment.metadata[attr.key()] = valueStr;
                    }
                }
                
                // Adicionar o fragmento
                AddFragmentInternal(fragment);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao processar lista de itens JSON: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Processa um arquivo JSON de lista de monstros
     * @param data Dados JSON
     * @param filePath Caminho do arquivo
     */
    void ProcessMobListJson(const json& data, const std::string& filePath) {
        try {
            // Extrair cada mob e criar um fragmento de conhecimento
            for (const auto& mob : data.items()) {
                // Verificar se possui uma estrutura válida de mob
                if (!mob.value().is_object()) continue;
                
                // Extrair dados do mob
                int mobId = -1;
                std::string mobName = "Monstro desconhecido";
                
                // Tentar extrair ID e nome
                if (mob.value().contains("id")) {
                    mobId = mob.value()["id"].get<int>();
                }
                else if (mob.value().contains("Index")) {
                    mobId = mob.value()["Index"].get<int>();
                }
                else if (mob.value().contains("index")) {
                    mobId = mob.value()["index"].get<int>();
                }
                
                if (mob.value().contains("name")) {
                    mobName = mob.value()["name"].get<std::string>();
                }
                else if (mob.value().contains("Name")) {
                    mobName = mob.value()["Name"].get<std::string>();
                }
                
                // Criar o fragmento
                KnowledgeFragment fragment;
                fragment.id = "mob:" + std::to_string(mobId);
                fragment.content = "Monstro: " + mobName + " (ID: " + std::to_string(mobId) + ")";
                fragment.source = filePath;
                fragment.domain = KnowledgeDomain::MOBS;
                fragment.confidence = 1.0f;
                fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                // Adicionar metadados
                fragment.metadata["tipo"] = "monstro";
                fragment.metadata["id"] = std::to_string(mobId);
                fragment.metadata["nome"] = mobName;
                
                // Adicionar atributos do monstro se disponíveis
                for (const auto& attr : mob.value().items()) {
                    if (attr.key() != "id" && attr.key() != "name" && 
                        attr.key() != "Index" && attr.key() != "Name") {
                        
                        // Converter o valor para string
                        std::string valueStr;
                        if (attr.value().is_string()) {
                            valueStr = attr.value().get<std::string>();
                        }
                        else if (attr.value().is_number()) {
                            valueStr = std::to_string(attr.value().get<int>());
                        }
                        else if (attr.value().is_boolean()) {
                            valueStr = attr.value().get<bool>() ? "true" : "false";
                        }
                        else {
                            valueStr = "complexo";
                        }
                        
                        // Adicionar ao conteúdo e metadados
                        fragment.content += "\n" + attr.key() + ": " + valueStr;
                        fragment.metadata[attr.key()] = valueStr;
                    }
                }
                
                // Adicionar o fragmento
                AddFragmentInternal(fragment);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao processar lista de monstros JSON: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Processa um arquivo JSON de lista de NPCs
     * @param data Dados JSON
     * @param filePath Caminho do arquivo
     */
    void ProcessNpcListJson(const json& data, const std::string& filePath) {
        // Implementação similar ao ProcessMobListJson, adaptada para NPCs
        try {
            // Extrair cada NPC e criar um fragmento de conhecimento
            for (const auto& npc : data.items()) {
                // Verificar se possui uma estrutura válida de NPC
                if (!npc.value().is_object()) continue;
                
                // Extrair dados do NPC
                int npcId = -1;
                std::string npcName = "NPC desconhecido";
                
                // Tentar extrair ID e nome
                if (npc.value().contains("id")) {
                    npcId = npc.value()["id"].get<int>();
                }
                else if (npc.value().contains("Index")) {
                    npcId = npc.value()["Index"].get<int>();
                }
                else if (npc.value().contains("index")) {
                    npcId = npc.value()["index"].get<int>();
                }
                
                if (npc.value().contains("name")) {
                    npcName = npc.value()["name"].get<std::string>();
                }
                else if (npc.value().contains("Name")) {
                    npcName = npc.value()["Name"].get<std::string>();
                }
                
                // Criar o fragmento
                KnowledgeFragment fragment;
                fragment.id = "npc:" + std::to_string(npcId);
                fragment.content = "NPC: " + npcName + " (ID: " + std::to_string(npcId) + ")";
                fragment.source = filePath;
                fragment.domain = KnowledgeDomain::NPCS;
                fragment.confidence = 1.0f;
                fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                // Adicionar metadados
                fragment.metadata["tipo"] = "npc";
                fragment.metadata["id"] = std::to_string(npcId);
                fragment.metadata["nome"] = npcName;
                
                // Adicionar atributos do NPC se disponíveis
                for (const auto& attr : npc.value().items()) {
                    if (attr.key() != "id" && attr.key() != "name" && 
                        attr.key() != "Index" && attr.key() != "Name") {
                        
                        // Converter o valor para string
                        std::string valueStr;
                        if (attr.value().is_string()) {
                            valueStr = attr.value().get<std::string>();
                        }
                        else if (attr.value().is_number()) {
                            valueStr = std::to_string(attr.value().get<int>());
                        }
                        else if (attr.value().is_boolean()) {
                            valueStr = attr.value().get<bool>() ? "true" : "false";
                        }
                        else {
                            valueStr = "complexo";
                        }
                        
                        // Adicionar ao conteúdo e metadados
                        fragment.content += "\n" + attr.key() + ": " + valueStr;
                        fragment.metadata[attr.key()] = valueStr;
                    }
                }
                
                // Adicionar o fragmento
                AddFragmentInternal(fragment);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao processar lista de NPCs JSON: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Processa um arquivo JSON de lista de mapas
     * @param data Dados JSON
     * @param filePath Caminho do arquivo
     */
    void ProcessMapListJson(const json& data, const std::string& filePath) {
        // Implementação similar ao ProcessMobListJson, adaptada para mapas
        try {
            // Extrair cada mapa e criar um fragmento de conhecimento
            for (const auto& map : data.items()) {
                // Verificar se possui uma estrutura válida de mapa
                if (!map.value().is_object()) continue;
                
                // Extrair dados do mapa
                int mapId = -1;
                std::string mapName = "Mapa desconhecido";
                
                // Tentar extrair ID e nome
                if (map.value().contains("id")) {
                    mapId = map.value()["id"].get<int>();
                }
                else if (map.value().contains("Index")) {
                    mapId = map.value()["Index"].get<int>();
                }
                else if (map.value().contains("index")) {
                    mapId = map.value()["index"].get<int>();
                }
                
                if (map.value().contains("name")) {
                    mapName = map.value()["name"].get<std::string>();
                }
                else if (map.value().contains("Name")) {
                    mapName = map.value()["Name"].get<std::string>();
                }
                
                // Criar o fragmento
                KnowledgeFragment fragment;
                fragment.id = "map:" + std::to_string(mapId);
                fragment.content = "Mapa: " + mapName + " (ID: " + std::to_string(mapId) + ")";
                fragment.source = filePath;
                fragment.domain = KnowledgeDomain::MAPS;
                fragment.confidence = 1.0f;
                fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                // Adicionar metadados
                fragment.metadata["tipo"] = "mapa";
                fragment.metadata["id"] = std::to_string(mapId);
                fragment.metadata["nome"] = mapName;
                
                // Adicionar atributos do mapa se disponíveis
                for (const auto& attr : map.value().items()) {
                    if (attr.key() != "id" && attr.key() != "name" && 
                        attr.key() != "Index" && attr.key() != "Name") {
                        
                        // Converter o valor para string
                        std::string valueStr;
                        if (attr.value().is_string()) {
                            valueStr = attr.value().get<std::string>();
                        }
                        else if (attr.value().is_number()) {
                            valueStr = std::to_string(attr.value().get<int>());
                        }
                        else if (attr.value().is_boolean()) {
                            valueStr = attr.value().get<bool>() ? "true" : "false";
                        }
                        else {
                            valueStr = "complexo";
                        }
                        
                        // Adicionar ao conteúdo e metadados
                        fragment.content += "\n" + attr.key() + ": " + valueStr;
                        fragment.metadata[attr.key()] = valueStr;
                    }
                }
                
                // Adicionar o fragmento
                AddFragmentInternal(fragment);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao processar lista de mapas JSON: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Processa um arquivo JSON de lista de habilidades
     * @param data Dados JSON
     * @param filePath Caminho do arquivo
     */
    void ProcessSkillListJson(const json& data, const std::string& filePath) {
        // Implementação similar ao ProcessMobListJson, adaptada para habilidades
        try {
            // Extrair cada habilidade e criar um fragmento de conhecimento
            for (const auto& skill : data.items()) {
                // Verificar se possui uma estrutura válida de habilidade
                if (!skill.value().is_object()) continue;
                
                // Extrair dados da habilidade
                int skillId = -1;
                std::string skillName = "Habilidade desconhecida";
                
                // Tentar extrair ID e nome
                if (skill.value().contains("id")) {
                    skillId = skill.value()["id"].get<int>();
                }
                else if (skill.value().contains("Index")) {
                    skillId = skill.value()["Index"].get<int>();
                }
                else if (skill.value().contains("index")) {
                    skillId = skill.value()["index"].get<int>();
                }
                
                if (skill.value().contains("name")) {
                    skillName = skill.value()["name"].get<std::string>();
                }
                else if (skill.value().contains("Name")) {
                    skillName = skill.value()["Name"].get<std::string>();
                }
                
                // Criar o fragmento
                KnowledgeFragment fragment;
                fragment.id = "skill:" + std::to_string(skillId);
                fragment.content = "Habilidade: " + skillName + " (ID: " + std::to_string(skillId) + ")";
                fragment.source = filePath;
                fragment.domain = KnowledgeDomain::SKILLS;
                fragment.confidence = 1.0f;
                fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                // Adicionar metadados
                fragment.metadata["tipo"] = "habilidade";
                fragment.metadata["id"] = std::to_string(skillId);
                fragment.metadata["nome"] = skillName;
                
                // Adicionar atributos da habilidade se disponíveis
                for (const auto& attr : skill.value().items()) {
                    if (attr.key() != "id" && attr.key() != "name" && 
                        attr.key() != "Index" && attr.key() != "Name") {
                        
                        // Converter o valor para string
                        std::string valueStr;
                        if (attr.value().is_string()) {
                            valueStr = attr.value().get<std::string>();
                        }
                        else if (attr.value().is_number()) {
                            valueStr = std::to_string(attr.value().get<int>());
                        }
                        else if (attr.value().is_boolean()) {
                            valueStr = attr.value().get<bool>() ? "true" : "false";
                        }
                        else {
                            valueStr = "complexo";
                        }
                        
                        // Adicionar ao conteúdo e metadados
                        fragment.content += "\n" + attr.key() + ": " + valueStr;
                        fragment.metadata[attr.key()] = valueStr;
                    }
                }
                
                // Adicionar o fragmento
                AddFragmentInternal(fragment);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao processar lista de habilidades JSON: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Analisa um arquivo de código-fonte
     * @param filePath Caminho para o arquivo
     */
    void AnalyzeSourceFile(const std::string& filePath) {
        try {
            // Abrir o arquivo de código-fonte
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo de código: " + filePath);
                return;
            }
            
            // Extrair informações básicas do nome do arquivo
            fs::path path(filePath);
            std::string fileName = path.filename().string();
            std::string extension = path.extension().string();
            
            // Ler o conteúdo do arquivo
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string code = buffer.str();
            file.close();
            
            // Extrair estruturas e funções do código
            std::vector<std::string> classes = ExtractClasses(code);
            std::vector<std::string> functions = ExtractFunctions(code);
            std::vector<std::string> structs = ExtractStructs(code);
            
            // Criar fragmento para o arquivo em si
            KnowledgeFragment fileFragment;
            fileFragment.id = "sourcefile:" + fileName;
            fileFragment.content = "Arquivo de código-fonte: " + fileName;
            fileFragment.source = filePath;
            fileFragment.domain = KnowledgeDomain::CODE_STRUCTURE;
            fileFragment.confidence = 1.0f;
            fileFragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            
            // Adicionar metadados
            fileFragment.metadata["tipo"] = "codigo_fonte";
            fileFragment.metadata["linguagem"] = (extension == ".cpp" || extension == ".h") ? "C++" : "Desconhecida";
            fileFragment.metadata["classes"] = std::to_string(classes.size());
            fileFragment.metadata["funcoes"] = std::to_string(functions.size());
            fileFragment.metadata["estruturas"] = std::to_string(structs.size());
            
            // Adicionar resumo do conteúdo
            fileFragment.content += "\nClasses: " + std::to_string(classes.size());
            fileFragment.content += "\nFunções: " + std::to_string(functions.size());
            fileFragment.content += "\nEstruturas: " + std::to_string(structs.size());
            
            // Adicionar o fragmento do arquivo
            AddFragmentInternal(fileFragment);
            
            // Criar fragmentos para cada classe
            for (const std::string& className : classes) {
                KnowledgeFragment classFragment;
                classFragment.id = "class:" + className;
                classFragment.content = "Classe: " + className;
                classFragment.source = filePath;
                classFragment.domain = KnowledgeDomain::CODE_STRUCTURE;
                classFragment.confidence = 0.9f;
                classFragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                classFragment.metadata["tipo"] = "classe";
                classFragment.metadata["arquivo"] = fileName;
                
                // Relacionar com o arquivo
                classFragment.relationships.push_back(fileFragment.id);
                
                AddFragmentInternal(classFragment);
            }
            
            // Criar fragmentos para cada função
            for (const std::string& functionName : functions) {
                KnowledgeFragment funcFragment;
                funcFragment.id = "function:" + functionName;
                funcFragment.content = "Função: " + functionName;
                funcFragment.source = filePath;
                funcFragment.domain = KnowledgeDomain::CODE_STRUCTURE;
                funcFragment.confidence = 0.9f;
                funcFragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                funcFragment.metadata["tipo"] = "funcao";
                funcFragment.metadata["arquivo"] = fileName;
                
                // Relacionar com o arquivo
                funcFragment.relationships.push_back(fileFragment.id);
                
                AddFragmentInternal(funcFragment);
            }
            
            // Criar fragmentos para cada estrutura
            for (const std::string& structName : structs) {
                KnowledgeFragment structFragment;
                structFragment.id = "struct:" + structName;
                structFragment.content = "Estrutura: " + structName;
                structFragment.source = filePath;
                structFragment.domain = KnowledgeDomain::CODE_STRUCTURE;
                structFragment.confidence = 0.9f;
                structFragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                structFragment.metadata["tipo"] = "estrutura";
                structFragment.metadata["arquivo"] = fileName;
                
                // Relacionar com o arquivo
                structFragment.relationships.push_back(fileFragment.id);
                
                AddFragmentInternal(structFragment);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao analisar arquivo de código " + filePath + ": " + e.what());
        }
    }
    
    /**
     * @brief Extrai definições de classes de um código-fonte
     * @param code Código-fonte
     * @return Vetor de nomes de classes
     */
    std::vector<std::string> ExtractClasses(const std::string& code) {
        std::vector<std::string> classes;
        std::regex classRegex(R"(class\s+(\w+))");
        
        auto classBegin = std::sregex_iterator(code.begin(), code.end(), classRegex);
        auto classEnd = std::sregex_iterator();
        
        for (std::sregex_iterator i = classBegin; i != classEnd; ++i) {
            std::smatch match = *i;
            classes.push_back(match[1]);
        }
        
        return classes;
    }
    
    /**
     * @brief Extrai definições de funções de um código-fonte
     * @param code Código-fonte
     * @return Vetor de nomes de funções
     */
    std::vector<std::string> ExtractFunctions(const std::string& code) {
        std::vector<std::string> functions;
        std::regex funcRegex(R"((?:void|int|float|double|char|bool|std::string|\w+)\s+(\w+)\s*\([^)]*\)\s*(?:\{|;))");
        
        auto funcBegin = std::sregex_iterator(code.begin(), code.end(), funcRegex);
        auto funcEnd = std::sregex_iterator();
        
        for (std::sregex_iterator i = funcBegin; i != funcEnd; ++i) {
            std::smatch match = *i;
            functions.push_back(match[1]);
        }
        
        return functions;
    }
    
    /**
     * @brief Extrai definições de estruturas de um código-fonte
     * @param code Código-fonte
     * @return Vetor de nomes de estruturas
     */
    std::vector<std::string> ExtractStructs(const std::string& code) {
        std::vector<std::string> structs;
        std::regex structRegex(R"(struct\s+(\w+))");
        
        auto structBegin = std::sregex_iterator(code.begin(), code.end(), structRegex);
        auto structEnd = std::sregex_iterator();
        
        for (std::sregex_iterator i = structBegin; i != structEnd; ++i) {
            std::smatch match = *i;
            structs.push_back(match[1]);
        }
        
        return structs;
    }
    
    /**
     * @brief Analisa um arquivo de texto
     * @param filePath Caminho para o arquivo
     */
    void AnalyzeTextFile(const std::string& filePath) {
        try {
            // Abrir o arquivo de texto
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo de texto: " + filePath);
                return;
            }
            
            // Extrair informações básicas do nome do arquivo
            fs::path path(filePath);
            std::string fileName = path.filename().string();
            std::string extension = path.extension().string();
            
            // Determinar domínio com base no nome do arquivo
            KnowledgeDomain domain = DetermineFileDomain(fileName);
            
            // Ler o conteúdo do arquivo
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string text = buffer.str();
            file.close();
            
            // Criar fragmento para o arquivo
            KnowledgeFragment fragment;
            fragment.id = "textfile:" + fileName;
            fragment.content = "Arquivo de texto: " + fileName;
            fragment.source = filePath;
            fragment.domain = domain;
            fragment.confidence = 1.0f;
            fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            
            // Adicionar metadados
            fragment.metadata["tipo"] = "arquivo_texto";
            fragment.metadata["extensao"] = extension;
            
            // Adicionar conteúdo resumido (limitado a 1000 caracteres)
            if (text.length() > 1000) {
                fragment.content += "\n\nConteúdo (primeiros 1000 caracteres):\n" + text.substr(0, 1000) + "...";
            }
            else {
                fragment.content += "\n\nConteúdo:\n" + text;
            }
            
            // Adicionar o fragmento
            AddFragmentInternal(fragment);
        }
        catch (const std::exception& e) {
            Log("Erro ao analisar arquivo de texto " + filePath + ": " + e.what());
        }
    }
    
    /**
     * @brief Analisa um arquivo CSV
     * @param filePath Caminho para o arquivo
     */
    void AnalyzeCsvFile(const std::string& filePath) {
        try {
            // Abrir o arquivo CSV
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo CSV: " + filePath);
                return;
            }
            
            // Extrair informações básicas do nome do arquivo
            fs::path path(filePath);
            std::string fileName = path.filename().string();
            
            // Determinar domínio com base no nome do arquivo
            KnowledgeDomain domain = DetermineFileDomain(fileName);
            
            // Ler cabeçalho
            std::string header;
            std::getline(file, header);
            
            // Dividir cabeçalho em colunas
            std::vector<std::string> columns = SplitCSV(header);
            
            // Contar linhas
            int lineCount = 0;
            std::string line;
            while (std::getline(file, line)) {
                lineCount++;
            }
            
            // Voltar ao início do arquivo e pular cabeçalho
            file.clear();
            file.seekg(0, std::ios::beg);
            std::getline(file, header); // Pular cabeçalho
            
            // Criar fragmento para o arquivo
            KnowledgeFragment fragment;
            fragment.id = "csvfile:" + fileName;
            fragment.content = "Arquivo CSV: " + fileName;
            fragment.source = filePath;
            fragment.domain = domain;
            fragment.confidence = 1.0f;
            fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            
            // Adicionar metadados
            fragment.metadata["tipo"] = "arquivo_csv";
            fragment.metadata["colunas"] = std::to_string(columns.size());
            fragment.metadata["linhas"] = std::to_string(lineCount);
            
            // Adicionar colunas ao conteúdo
            fragment.content += "\n\nColunas: ";
            for (size_t i = 0; i < columns.size(); ++i) {
                if (i > 0) fragment.content += ", ";
                fragment.content += columns[i];
            }
            
            fragment.content += "\nTotal de registros: " + std::to_string(lineCount);
            
            // Ler algumas linhas de exemplo (limitado a 5)
            fragment.content += "\n\nExemplos:";
            int exampleCount = 0;
            while (std::getline(file, line) && exampleCount < 5) {
                fragment.content += "\n" + line;
                exampleCount++;
            }
            
            // Se houver mais linhas, indicar
            if (lineCount > 5) {
                fragment.content += "\n...e mais " + std::to_string(lineCount - 5) + " registros";
            }
            
            // Adicionar o fragmento
            AddFragmentInternal(fragment);
            
            // Para arquivos específicos como ItemList.csv, processar registros individualmente
            if (fileName == "ItemList.csv" || fileName.find("ItemList") != std::string::npos) {
                ProcessItemListCSV(filePath, columns);
            }
            else if (fileName == "MobList.csv" || fileName.find("MobList") != std::string::npos) {
                ProcessMobListCSV(filePath, columns);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao analisar arquivo CSV " + filePath + ": " + e.what());
        }
    }
    
    /**
     * @brief Divide uma linha CSV em colunas
     * @param line Linha CSV
     * @return Vetor de colunas
     */
    std::vector<std::string> SplitCSV(const std::string& line) {
        std::vector<std::string> columns;
        std::stringstream ss(line);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            // Remover espaços em branco extras
            item.erase(0, item.find_first_not_of(" \t\r\n"));
            item.erase(item.find_last_not_of(" \t\r\n") + 1);
            
            columns.push_back(item);
        }
        
        return columns;
    }
    
    /**
     * @brief Processa um arquivo CSV de lista de itens
     * @param filePath Caminho para o arquivo
     * @param columns Colunas do CSV
     */
    void ProcessItemListCSV(const std::string& filePath, const std::vector<std::string>& columns) {
        try {
            // Abrir o arquivo CSV
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo CSV de itens: " + filePath);
                return;
            }
            
            // Pular cabeçalho
            std::string header;
            std::getline(file, header);
            
            // Ler cada linha
            std::string line;
            while (std::getline(file, line)) {
                // Dividir em colunas
                std::vector<std::string> values = SplitCSV(line);
                
                // Verificar se tem colunas suficientes
                if (values.size() < 2) continue;
                
                // Tentar extrair ID e nome
                int itemId = -1;
                std::string itemName = "Item desconhecido";
                
                // Assumir que a primeira coluna é o ID e a segunda é o nome
                // (Adaptar conforme a estrutura real do arquivo)
                try {
                    itemId = std::stoi(values[0]);
                    itemName = values.size() > 1 ? values[1] : "Item desconhecido";
                }
                catch (...) {
                    continue; // Pular se não conseguir converter o ID
                }
                
                // Criar fragmento para o item
                KnowledgeFragment fragment;
                fragment.id = "item:" + std::to_string(itemId);
                fragment.content = "Item: " + itemName + " (ID: " + std::to_string(itemId) + ")";
                fragment.source = filePath;
                fragment.domain = KnowledgeDomain::ITEMS;
                fragment.confidence = 1.0f;
                fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                // Adicionar metadados
                fragment.metadata["tipo"] = "item";
                fragment.metadata["id"] = std::to_string(itemId);
                fragment.metadata["nome"] = itemName;
                
                // Adicionar outros atributos disponíveis
                for (size_t i = 2; i < values.size() && i < columns.size(); ++i) {
                    fragment.content += "\n" + columns[i] + ": " + values[i];
                    fragment.metadata[columns[i]] = values[i];
                }
                
                // Adicionar o fragmento
                AddFragmentInternal(fragment);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao processar lista de itens CSV: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Processa um arquivo CSV de lista de monstros
     * @param filePath Caminho para o arquivo
     * @param columns Colunas do CSV
     */
    void ProcessMobListCSV(const std::string& filePath, const std::vector<std::string>& columns) {
        // Implementação similar ao ProcessItemListCSV, adaptada para monstros
        try {
            // Abrir o arquivo CSV
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo CSV de monstros: " + filePath);
                return;
            }
            
            // Pular cabeçalho
            std::string header;
            std::getline(file, header);
            
            // Ler cada linha
            std::string line;
            while (std::getline(file, line)) {
                // Dividir em colunas
                std::vector<std::string> values = SplitCSV(line);
                
                // Verificar se tem colunas suficientes
                if (values.size() < 2) continue;
                
                // Tentar extrair ID e nome
                int mobId = -1;
                std::string mobName = "Monstro desconhecido";
                
                // Assumir que a primeira coluna é o ID e a segunda é o nome
                // (Adaptar conforme a estrutura real do arquivo)
                try {
                    mobId = std::stoi(values[0]);
                    mobName = values.size() > 1 ? values[1] : "Monstro desconhecido";
                }
                catch (...) {
                    continue; // Pular se não conseguir converter o ID
                }
                
                // Criar fragmento para o monstro
                KnowledgeFragment fragment;
                fragment.id = "mob:" + std::to_string(mobId);
                fragment.content = "Monstro: " + mobName + " (ID: " + std::to_string(mobId) + ")";
                fragment.source = filePath;
                fragment.domain = KnowledgeDomain::MOBS;
                fragment.confidence = 1.0f;
                fragment.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                
                // Adicionar metadados
                fragment.metadata["tipo"] = "monstro";
                fragment.metadata["id"] = std::to_string(mobId);
                fragment.metadata["nome"] = mobName;
                
                // Adicionar outros atributos disponíveis
                for (size_t i = 2; i < values.size() && i < columns.size(); ++i) {
                    fragment.content += "\n" + columns[i] + ": " + values[i];
                    fragment.metadata[columns[i]] = values[i];
                }
                
                // Adicionar o fragmento
                AddFragmentInternal(fragment);
            }
        }
        catch (const std::exception& e) {
            Log("Erro ao processar lista de monstros CSV: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Determina o domínio de conhecimento de um arquivo com base no nome
     * @param fileName Nome do arquivo
     * @return Domínio determinado
     */
    KnowledgeDomain DetermineFileDomain(const std::string& fileName) {
        std::string lowerName = fileName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName.find("item") != std::string::npos) {
            return KnowledgeDomain::ITEMS;
        }
        else if (lowerName.find("mob") != std::string::npos) {
            return KnowledgeDomain::MOBS;
        }
        else if (lowerName.find("npc") != std::string::npos) {
            return KnowledgeDomain::NPCS;
        }
        else if (lowerName.find("map") != std::string::npos) {
            return KnowledgeDomain::MAPS;
        }
        else if (lowerName.find("skill") != std::string::npos) {
            return KnowledgeDomain::SKILLS;
        }
        else if (lowerName.find("quest") != std::string::npos) {
            return KnowledgeDomain::QUESTS;
        }
        else if (lowerName.find("price") != std::string::npos || 
                 lowerName.find("shop") != std::string::npos || 
                 lowerName.find("store") != std::string::npos) {
            return KnowledgeDomain::ECONOMY;
        }
        else if (lowerName.find("server") != std::string::npos || 
                 lowerName.find("config") != std::string::npos || 
                 lowerName.find("setting") != std::string::npos) {
            return KnowledgeDomain::SERVER_CONFIG;
        }
        else if (lowerName.find(".cpp") != std::string::npos || 
                 lowerName.find(".h") != std::string::npos || 
                 lowerName.find(".c") != std::string::npos) {
            return KnowledgeDomain::CODE_STRUCTURE;
        }
        else if (lowerName.find(".sql") != std::string::npos || 
                 lowerName.find("database") != std::string::npos || 
                 lowerName.find("db") != std::string::npos) {
            return KnowledgeDomain::DATABASE_SCHEMA;
        }
        else {
            return KnowledgeDomain::GAME_MECHANICS;
        }
    }
    
    /**
     * @brief Adiciona um fragmento de conhecimento à base de dados
     * @param fragment Fragmento a ser adicionado
     */
    void AddFragmentInternal(const KnowledgeFragment& fragment) {
        std::lock_guard<std::mutex> lock(m_fragmentsMutex);
        
        // Verificar se já existe um fragmento com o mesmo ID
        for (size_t i = 0; i < m_knowledgeFragments.size(); ++i) {
            if (m_knowledgeFragments[i].id == fragment.id) {
                // Substituir o fragmento existente
                m_knowledgeFragments[i] = fragment;
                return;
            }
        }
        
        // Adicionar o novo fragmento
        m_knowledgeFragments.push_back(fragment);
        
        // Atualizar índice por domínio
        m_domainIndices[fragment.domain].push_back(m_knowledgeFragments.size() - 1);
        
        // Atualizar índice textual
        std::vector<std::string> terms = ExtractSearchTerms(fragment.content);
        for (const std::string& term : terms) {
            m_textSearchIndex[term].push_back(m_knowledgeFragments.size() - 1);
        }
        
        // Incrementar contador
        m_totalFragments++;
    }
    
    /**
     * @brief Extrai termos de busca de um texto
     * @param text Texto a ser analisado
     * @return Vetor de termos únicos
     */
    std::vector<std::string> ExtractSearchTerms(const std::string& text) {
        std::vector<std::string> terms;
        std::string lowerText = text;
        std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
        
        // Dividir em palavras
        std::regex wordRegex(R"(\w+)");
        auto wordBegin = std::sregex_iterator(lowerText.begin(), lowerText.end(), wordRegex);
        auto wordEnd = std::sregex_iterator();
        
        std::set<std::string> uniqueTerms;
        for (std::sregex_iterator i = wordBegin; i != wordEnd; ++i) {
            std::smatch match = *i;
            std::string term = match.str();
            
            // Ignorar termos muito curtos
            if (term.length() < 3) continue;
            
            // Ignorar stopwords comuns
            static const std::set<std::string> stopwords = {
                "the", "and", "for", "with", "that", "this", "its", "are", "not"
            };
            
            if (stopwords.find(term) == stopwords.end()) {
                uniqueTerms.insert(term);
            }
        }
        
        terms.insert(terms.end(), uniqueTerms.begin(), uniqueTerms.end());
        return terms;
    }
    
    /**
     * @brief Calcula a similaridade entre um texto e termos de busca
     * @param text Texto a ser comparado
     * @param searchTerms Termos de busca
     * @return Valor de similaridade (0.0 a 1.0)
     */
    float CalculateTextSimilarity(const std::string& text, const std::vector<std::string>& searchTerms) {
        if (searchTerms.empty()) return 0.0f;
        
        std::string lowerText = text;
        std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
        
        int matchCount = 0;
        for (const std::string& term : searchTerms) {
            if (lowerText.find(term) != std::string::npos) {
                matchCount++;
            }
        }
        
        return static_cast<float>(matchCount) / static_cast<float>(searchTerms.size());
    }
};

// Implementação da classe GameKnowledgeBase

GameKnowledgeBase::GameKnowledgeBase(std::shared_ptr<FileSystemConnector> fsConnector)
    : m_pImpl(std::make_unique<Impl>(fsConnector))
    , m_fsConnector(fsConnector) {
}

GameKnowledgeBase::~GameKnowledgeBase() = default;

bool GameKnowledgeBase::Initialize(const std::string& gamePath, const std::string& sourcesPath, const std::string& databasePath) {
    try {
        m_pImpl->Log("Inicializando base de conhecimento...");
        
        // Validar caminhos
        if (!fs::exists(gamePath)) {
            m_pImpl->Log("Erro: Caminho do jogo não existe: " + gamePath);
            return false;
        }
        
        // Armazenar caminhos
        m_pImpl->m_gamePath = gamePath;
        m_pImpl->m_sourcesPath = sourcesPath;
        m_pImpl->m_databasePath = databasePath;
        
        // Limpar dados existentes
        m_pImpl->m_knowledgeFragments.clear();
        m_pImpl->m_domainIndices.clear();
        m_pImpl->m_textSearchIndex.clear();
        m_pImpl->m_totalFragments = 0;
        
        m_pImpl->m_initialized = true;
        m_pImpl->Log("Base de conhecimento inicializada com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao inicializar base de conhecimento: " + std::string(e.what()));
        return false;
    }
}

std::future<bool> GameKnowledgeBase::StartDeepAnalysis(std::function<void(float, const std::string&)> progressCallback) {
    // Retornar um std::future que executa a análise em uma thread separada
    return std::async(std::launch::async, [this, progressCallback]() {
        if (!m_pImpl->m_initialized) {
            m_pImpl->Log("Erro: Base de conhecimento não inicializada");
            if (progressCallback) {
                progressCallback(1.0f, "Erro: Base de conhecimento não inicializada");
            }
            return false;
        }
        
        if (m_pImpl->m_analyzeRunning) {
            m_pImpl->Log("Aviso: Análise já em andamento");
            if (progressCallback) {
                progressCallback(1.0f, "Aviso: Análise já em andamento");
            }
            return false;
        }
        
        m_pImpl->m_analyzeRunning = true;
        m_pImpl->Log("Iniciando análise profunda...");
        
        try {
            // Contagem aproximada de arquivos para progresso
            size_t totalItems = 1000; // Valor inicial estimado
            
            // Tentar contar arquivos para progresso mais preciso
            if (fs::exists(m_pImpl->m_gamePath)) {
                size_t fileCount = 0;
                for (const auto& entry : fs::recursive_directory_iterator(m_pImpl->m_gamePath)) {
                    if (entry.is_regular_file()) {
                        fileCount++;
                    }
                }
                totalItems = fileCount;
            }
            
            // Contador de itens processados
            std::atomic<size_t> processedItems(0);
            
            // Processar diretórios em threads separadas
            std::vector<std::thread> threads;
            
            // Processar diretório de arquivos do jogo
            if (!m_pImpl->m_gamePath.empty() && fs::exists(m_pImpl->m_gamePath)) {
                threads.push_back(std::thread([this, &progressCallback, totalItems, &processedItems]() {
                    m_pImpl->AnalyzeDirectory(m_pImpl->m_gamePath, progressCallback, totalItems, processedItems);
                }));
            }
            
            // Processar diretório de código-fonte
            if (!m_pImpl->m_sourcesPath.empty() && fs::exists(m_pImpl->m_sourcesPath)) {
                threads.push_back(std::thread([this, &progressCallback, totalItems, &processedItems]() {
                    m_pImpl->AnalyzeDirectory(m_pImpl->m_sourcesPath, progressCallback, totalItems, processedItems);
                }));
            }
            
            // Aguardar todas as threads terminarem
            for (auto& thread : threads) {
                thread.join();
            }
            
            // Construir índices de busca
            BuildSearchIndex();
            
            m_pImpl->m_analyzeRunning = false;
            m_pImpl->Log("Análise profunda concluída. Total de fragmentos: " + std::to_string(m_pImpl->m_totalFragments));
            
            if (progressCallback) {
                progressCallback(1.0f, "Análise concluída. Total de fragmentos: " + std::to_string(m_pImpl->m_totalFragments));
            }
            
            return true;
        }
        catch (const std::exception& e) {
            m_pImpl->m_analyzeRunning = false;
            m_pImpl->Log("Erro durante análise profunda: " + std::string(e.what()));
            
            if (progressCallback) {
                progressCallback(1.0f, "Erro: " + std::string(e.what()));
            }
            
            return false;
        }
    });
}

KnowledgeResult GameKnowledgeBase::Query(const KnowledgeQuery& query) {
    KnowledgeResult result;
    result.relevanceScore = 0.0f;
    
    if (!m_pImpl->m_initialized) {
        result.explanation = "Erro: Base de conhecimento não inicializada";
        return result;
    }
    
    try {
        // Extrair termos de busca
        std::vector<std::string> searchTerms = m_pImpl->ExtractSearchTerms(query.query);
        
        // Estrutura para armazenar fragmentos com pontuação
        std::vector<std::pair<size_t, float>> scoredIndices;
        
        // Buscar por domínio
        for (KnowledgeDomain domain : query.domains) {
            auto it = m_pImpl->m_domainIndices.find(domain);
            if (it != m_pImpl->m_domainIndices.end()) {
                for (size_t index : it->second) {
                    // Calcular similaridade
                    float similarity = m_pImpl->CalculateTextSimilarity(
                        m_pImpl->m_knowledgeFragments[index].content, searchTerms);
                    
                    // Adicionar se relevante o suficiente
                    if (similarity >= query.minConfidence) {
                        scoredIndices.push_back(std::make_pair(index, similarity));
                    }
                }
            }
        }
        
        // Ordenar por relevância
        std::sort(scoredIndices.begin(), scoredIndices.end(), 
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Limitar ao número máximo de resultados
        int resultCount = std::min(static_cast<int>(scoredIndices.size()), query.maxResults);
        
        // Pontuação de relevância média
        float totalScore = 0.0f;
        
        // Adicionar fragmentos ao resultado
        for (int i = 0; i < resultCount; ++i) {
            size_t index = scoredIndices[i].first;
            KnowledgeFragment fragment = m_pImpl->m_knowledgeFragments[index];
            
            // Limpar metadados e relacionamentos se não solicitados
            if (!query.includeMetadata) {
                fragment.metadata.clear();
            }
            
            if (!query.includeRelationships) {
                fragment.relationships.clear();
            }
            
            result.fragments.push_back(fragment);
            totalScore += scoredIndices[i].second;
        }
        
        // Calcular pontuação média
        if (resultCount > 0) {
            result.relevanceScore = totalScore / static_cast<float>(resultCount);
        }
        
        // Adicionar explicação
        result.explanation = "Encontrados " + std::to_string(resultCount) + 
                            " fragmentos relevantes com pontuação média de " + 
                            std::to_string(result.relevanceScore);
        
        return result;
    }
    catch (const std::exception& e) {
        result.explanation = "Erro ao processar consulta: " + std::string(e.what());
        return result;
    }
}

KnowledgeResult GameKnowledgeBase::NaturalLanguageQuery(const std::string& naturalQuery) {
    // Analisar a consulta em linguagem natural para determinar os domínios relevantes
    std::string lowerQuery = naturalQuery;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    // Construir uma consulta estruturada
    KnowledgeQuery query;
    query.query = naturalQuery;
    query.minConfidence = 0.3f;
    query.maxResults = 10;
    query.includeMetadata = true;
    query.includeRelationships = true;
    
    // Determinar domínios relevantes com base em palavras-chave
    if (lowerQuery.find("item") != std::string::npos || 
        lowerQuery.find("equip") != std::string::npos || 
        lowerQuery.find("arma") != std::string::npos || 
        lowerQuery.find("armadura") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::ITEMS);
    }
    
    if (lowerQuery.find("mob") != std::string::npos || 
        lowerQuery.find("monstro") != std::string::npos || 
        lowerQuery.find("criatura") != std::string::npos || 
        lowerQuery.find("inimigo") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::MOBS);
    }
    
    if (lowerQuery.find("npc") != std::string::npos || 
        lowerQuery.find("personagem") != std::string::npos || 
        lowerQuery.find("mercador") != std::string::npos || 
        lowerQuery.find("vendedor") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::NPCS);
    }
    
    if (lowerQuery.find("mapa") != std::string::npos || 
        lowerQuery.find("área") != std::string::npos || 
        lowerQuery.find("região") != std::string::npos || 
        lowerQuery.find("localização") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::MAPS);
    }
    
    if (lowerQuery.find("skill") != std::string::npos || 
        lowerQuery.find("habilidade") != std::string::npos || 
        lowerQuery.find("magia") != std::string::npos || 
        lowerQuery.find("talento") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::SKILLS);
    }
    
    if (lowerQuery.find("quest") != std::string::npos || 
        lowerQuery.find("missão") != std::string::npos || 
        lowerQuery.find("tarefa") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::QUESTS);
    }
    
    if (lowerQuery.find("preço") != std::string::npos || 
        lowerQuery.find("economia") != std::string::npos || 
        lowerQuery.find("loja") != std::string::npos || 
        lowerQuery.find("comércio") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::ECONOMY);
    }
    
    if (lowerQuery.find("config") != std::string::npos || 
        lowerQuery.find("servidor") != std::string::npos || 
        lowerQuery.find("configuração") != std::string::npos || 
        lowerQuery.find("settings") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::SERVER_CONFIG);
    }
    
    if (lowerQuery.find("mecânica") != std::string::npos || 
        lowerQuery.find("sistema") != std::string::npos || 
        lowerQuery.find("como funciona") != std::string::npos || 
        lowerQuery.find("regra") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::GAME_MECHANICS);
    }
    
    if (lowerQuery.find("código") != std::string::npos || 
        lowerQuery.find("classe") != std::string::npos || 
        lowerQuery.find("estrutura") != std::string::npos || 
        lowerQuery.find("função") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::CODE_STRUCTURE);
    }
    
    if (lowerQuery.find("banco de dados") != std::string::npos || 
        lowerQuery.find("database") != std::string::npos || 
        lowerQuery.find("tabela") != std::string::npos || 
        lowerQuery.find("sql") != std::string::npos) {
        query.domains.push_back(KnowledgeDomain::DATABASE_SCHEMA);
    }
    
    // Se nenhum domínio específico foi identificado, buscar em todos
    if (query.domains.empty()) {
        query.domains = {
            KnowledgeDomain::ITEMS,
            KnowledgeDomain::MOBS,
            KnowledgeDomain::NPCS,
            KnowledgeDomain::MAPS,
            KnowledgeDomain::SKILLS,
            KnowledgeDomain::QUESTS,
            KnowledgeDomain::ECONOMY,
            KnowledgeDomain::SERVER_CONFIG,
            KnowledgeDomain::GAME_MECHANICS,
            KnowledgeDomain::CODE_STRUCTURE,
            KnowledgeDomain::DATABASE_SCHEMA
        };
    }
    
    // Executar a consulta
    return Query(query);
}

bool GameKnowledgeBase::AddKnowledgeFragment(const KnowledgeFragment& fragment) {
    try {
        m_pImpl->AddFragmentInternal(fragment);
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao adicionar fragmento de conhecimento: " + std::string(e.what()));
        return false;
    }
}

int GameKnowledgeBase::AddKnowledgeFragments(const std::vector<KnowledgeFragment>& fragments) {
    int successCount = 0;
    
    for (const auto& fragment : fragments) {
        if (AddKnowledgeFragment(fragment)) {
            successCount++;
        }
    }
    
    return successCount;
}

size_t GameKnowledgeBase::GetFragmentCount() const {
    return m_pImpl->m_totalFragments;
}

std::map<KnowledgeDomain, size_t> GameKnowledgeBase::GetFragmentCountByDomain() const {
    std::map<KnowledgeDomain, size_t> counts;
    
    for (const auto& pair : m_pImpl->m_domainIndices) {
        counts[pair.first] = pair.second.size();
    }
    
    return counts;
}

bool GameKnowledgeBase::ExportToFile(const std::string& filePath) {
    try {
        m_pImpl->Log("Exportando base de conhecimento para " + filePath);
        
        // Criar array JSON com todos os fragmentos
        json jsonData = json::array();
        
        // Converter cada fragmento para JSON
        for (const auto& fragment : m_pImpl->m_knowledgeFragments) {
            json fragmentJson;
            fragmentJson["id"] = fragment.id;
            fragmentJson["content"] = fragment.content;
            fragmentJson["source"] = fragment.source;
            fragmentJson["metadata"] = fragment.metadata;
            fragmentJson["relationships"] = fragment.relationships;
            fragmentJson["domain"] = static_cast<int>(fragment.domain);
            fragmentJson["confidence"] = fragment.confidence;
            fragmentJson["timestamp"] = fragment.timestamp;
            
            jsonData.push_back(fragmentJson);
        }
        
        // Escrever no arquivo
        std::ofstream file(filePath);
        if (!file.is_open()) {
            m_pImpl->Log("Erro: Não foi possível abrir arquivo para escrita: " + filePath);
            return false;
        }
        
        file << jsonData.dump(4); // Pretty print com 4 espaços
        file.close();
        
        m_pImpl->Log("Base de conhecimento exportada com sucesso: " + std::to_string(m_pImpl->m_totalFragments) + " fragmentos");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao exportar base de conhecimento: " + std::string(e.what()));
        return false;
    }
}

bool GameKnowledgeBase::ImportFromFile(const std::string& filePath, bool merge) {
    try {
        m_pImpl->Log("Importando base de conhecimento de " + filePath);
        
        // Abrir arquivo
        std::ifstream file(filePath);
        if (!file.is_open()) {
            m_pImpl->Log("Erro: Não foi possível abrir arquivo para leitura: " + filePath);
            return false;
        }
        
        // Ler JSON
        json jsonData;
        file >> jsonData;
        file.close();
        
        // Verificar se é um array
        if (!jsonData.is_array()) {
            m_pImpl->Log("Erro: Formato de arquivo inválido (esperado array JSON)");
            return false;
        }
        
        // Se não for para mesclar, limpar dados existentes
        if (!merge) {
            m_pImpl->m_knowledgeFragments.clear();
            m_pImpl->m_domainIndices.clear();
            m_pImpl->m_textSearchIndex.clear();
            m_pImpl->m_totalFragments = 0;
        }
        
        // Converter cada item JSON para um fragmento
        for (const auto& fragmentJson : jsonData) {
            KnowledgeFragment fragment;
            
            fragment.id = fragmentJson["id"].get<std::string>();
            fragment.content = fragmentJson["content"].get<std::string>();
            fragment.source = fragmentJson["source"].get<std::string>();
            fragment.metadata = fragmentJson["metadata"].get<std::map<std::string, std::string>>();
            fragment.relationships = fragmentJson["relationships"].get<std::vector<std::string>>();
            fragment.domain = static_cast<KnowledgeDomain>(fragmentJson["domain"].get<int>());
            fragment.confidence = fragmentJson["confidence"].get<float>();
            fragment.timestamp = fragmentJson["timestamp"].get<int64_t>();
            
            m_pImpl->AddFragmentInternal(fragment);
        }
        
        // Reconstruir índices
        BuildSearchIndex();
        
        m_pImpl->Log("Base de conhecimento importada com sucesso: " + std::to_string(m_pImpl->m_totalFragments) + " fragmentos");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao importar base de conhecimento: " + std::string(e.what()));
        return false;
    }
}

std::string GameKnowledgeBase::GenerateSummary(const std::string& topic, int maxLength) {
    try {
        // Buscar fragmentos relacionados ao tópico
        KnowledgeQuery query;
        query.query = topic;
        query.domains = {
            KnowledgeDomain::ITEMS,
            KnowledgeDomain::MOBS,
            KnowledgeDomain::NPCS,
            KnowledgeDomain::MAPS,
            KnowledgeDomain::SKILLS,
            KnowledgeDomain::QUESTS,
            KnowledgeDomain::ECONOMY,
            KnowledgeDomain::GAME_MECHANICS
        };
        query.minConfidence = 0.4f;
        query.maxResults = 20;
        query.includeMetadata = false;
        query.includeRelationships = false;
        
        KnowledgeResult result = Query(query);
        
        if (result.fragments.empty()) {
            return "Não foram encontradas informações sobre " + topic + ".";
        }
        
        // Gerar resumo a partir dos fragmentos
        std::stringstream summary;
        summary << "Resumo sobre " << topic << ":\n\n";
        
        // Adicionar informações de cada fragmento
        for (const auto& fragment : result.fragments) {
            summary << fragment.content << "\n\n";
            
            // Verificar se o resumo já está muito longo
            if (summary.str().length() > static_cast<size_t>(maxLength)) {
                break;
            }
        }
        
        // Truncar se necessário
        std::string summaryStr = summary.str();
        if (summaryStr.length() > static_cast<size_t>(maxLength)) {
            summaryStr = summaryStr.substr(0, maxLength) + "...";
        }
        
        return summaryStr;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao gerar resumo: " + std::string(e.what()));
        return "Erro ao gerar resumo: " + std::string(e.what());
    }
}

void GameKnowledgeBase::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_pImpl->m_logCallback = logCallback;
}

void GameKnowledgeBase::AnalyzeItemDatabase() {
    // Este método seria utilizado para análise específica de itens
    // Implementação estaria no arquivo de implementação
}

void GameKnowledgeBase::AnalyzeMobDatabase() {
    // Este método seria utilizado para análise específica de monstros
    // Implementação estaria no arquivo de implementação
}

void GameKnowledgeBase::AnalyzeNpcDatabase() {
    // Este método seria utilizado para análise específica de NPCs
    // Implementação estaria no arquivo de implementação
}

void GameKnowledgeBase::AnalyzeMapFiles() {
    // Este método seria utilizado para análise específica de mapas
    // Implementação estaria no arquivo de implementação
}

void GameKnowledgeBase::AnalyzeSkillDatabase() {
    // Este método seria utilizado para análise específica de habilidades
    // Implementação estaria no arquivo de implementação
}

void GameKnowledgeBase::AnalyzeServerConfiguration() {
    // Este método seria utilizado para análise de configurações do servidor
    // Implementação estaria no arquivo de implementação
}

void GameKnowledgeBase::AnalyzeSourceCode() {
    // Este método seria utilizado para análise de código-fonte
    // Implementação estaria no arquivo de implementação
}

void GameKnowledgeBase::AnalyzeGameDatabase() {
    // Este método seria utilizado para análise do banco de dados do jogo
    // Implementação estaria no arquivo de implementação
}

void GameKnowledgeBase::BuildSearchIndex() {
    std::lock_guard<std::mutex> lock(m_pImpl->m_indexMutex);
    
    m_pImpl->Log("Construindo índices de busca...");
    
    // Limpar índices existentes
    m_pImpl->m_domainIndices.clear();
    m_pImpl->m_textSearchIndex.clear();
    
    // Reconstruir índice por domínio
    for (size_t i = 0; i < m_pImpl->m_knowledgeFragments.size(); ++i) {
        const auto& fragment = m_pImpl->m_knowledgeFragments[i];
        m_pImpl->m_domainIndices[fragment.domain].push_back(i);
        
        // Reconstruir índice textual
        std::vector<std::string> terms = m_pImpl->ExtractSearchTerms(fragment.content);
        for (const std::string& term : terms) {
            m_pImpl->m_textSearchIndex[term].push_back(i);
        }
    }
    
    m_pImpl->Log("Índices de busca construídos");
}

std::vector<KnowledgeFragment> GameKnowledgeBase::SearchByText(const std::string& searchText, float minRelevance) {
    std::vector<KnowledgeFragment> results;
    
    // Extrair termos de busca
    std::vector<std::string> searchTerms = m_pImpl->ExtractSearchTerms(searchText);
    
    // Conjunto de índices únicos de fragmentos encontrados
    std::set<size_t> matchedIndices;
    
    // Para cada termo, procurar no índice textual
    for (const auto& term : searchTerms) {
        auto it = m_pImpl->m_textSearchIndex.find(term);
        if (it != m_pImpl->m_textSearchIndex.end()) {
            for (size_t index : it->second) {
                matchedIndices.insert(index);
            }
        }
    }
    
    // Calcular relevância para cada fragmento encontrado
    std::vector<std::pair<size_t, float>> scoredMatches;
    for (size_t index : matchedIndices) {
        float similarity = m_pImpl->CalculateTextSimilarity(
            m_pImpl->m_knowledgeFragments[index].content, searchTerms);
        
        if (similarity >= minRelevance) {
            scoredMatches.push_back(std::make_pair(index, similarity));
        }
    }
    
    // Ordenar por relevância
    std::sort(scoredMatches.begin(), scoredMatches.end(), 
             [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Adicionar fragmentos aos resultados
    for (const auto& pair : scoredMatches) {
        results.push_back(m_pImpl->m_knowledgeFragments[pair.first]);
    }
    
    return results;
}

} // namespace WYDStudio