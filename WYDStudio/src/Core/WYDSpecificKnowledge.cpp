#include "../../include/WYDSpecificKnowledge.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>
#include <chrono>
#include <json/json.hpp> // Usando nlohmann/json para C++

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace WYDStudio {

class WYDSpecificKnowledge::Impl {
public:
    Impl(std::shared_ptr<GameKnowledgeBase> knowledgeBase)
        : m_knowledgeBase(knowledgeBase)
        , m_initialized(false) {
    }
    
    ~Impl() = default;
    
    // Base de conhecimento compartilhada
    std::shared_ptr<GameKnowledgeBase> m_knowledgeBase;
    
    // Caminho para os dados do jogo
    std::string m_gameDataPath;
    
    // Status
    bool m_initialized;
    
    // Dados do jogo
    std::map<int, std::shared_ptr<ItemData>> m_itemsById;
    std::map<std::string, std::shared_ptr<ItemData>> m_itemsByName;
    std::map<int, std::shared_ptr<NPCData>> m_npcsById;
    std::map<std::string, std::shared_ptr<NPCData>> m_npcsByName;
    std::map<int, std::shared_ptr<MapData>> m_mapsById;
    std::map<std::string, std::shared_ptr<MapData>> m_mapsByName;
    
    // Função de callback para log
    std::function<void(const std::string&)> m_logCallback;
    
    /**
     * @brief Registra uma mensagem no log
     * @param message Mensagem a ser registrada
     */
    void Log(const std::string& message) {
        if (m_logCallback) {
            m_logCallback(message);
        } else {
            std::cout << "[WYDSpecificKnowledge] " << message << std::endl;
        }
    }
    
    /**
     * @brief Normaliza um nome para búsca (remove acentos, converte para minúsculas)
     * @param name Nome a ser normalizado
     * @return Nome normalizado
     */
    std::string NormalizeName(const std::string& name) {
        std::string result = name;
        
        // Converter para minúsculas
        std::transform(result.begin(), result.end(), result.begin(), 
                     [](unsigned char c) { return std::tolower(c); });
        
        // Remover espaços extras
        result.erase(0, result.find_first_not_of(" \t\r\n"));
        result.erase(result.find_last_not_of(" \t\r\n") + 1);
        
        // Substituir acentos comuns (implementação básica)
        std::map<std::string, std::string> accentMap = {
            {"á", "a"}, {"é", "e"}, {"í", "i"}, {"ó", "o"}, {"ú", "u"},
            {"à", "a"}, {"è", "e"}, {"ì", "i"}, {"ò", "o"}, {"ù", "u"},
            {"ã", "a"}, {"õ", "o"}, {"ñ", "n"}, {"ç", "c"}
        };
        
        for (const auto& pair : accentMap) {
            size_t pos = 0;
            while ((pos = result.find(pair.first, pos)) != std::string::npos) {
                result.replace(pos, pair.first.length(), pair.second);
                pos += pair.second.length();
            }
        }
        
        return result;
    }
    
    /**
     * @brief Carrega arquivos JSON extraídos dos arquivos binários do jogo
     * @param gameDataPath Caminho para os dados do jogo
     */
    bool LoadJsonFiles(const std::string& gameDataPath) {
        try {
            Log("Carregando arquivos JSON do jogo...");
            
            // Tentar carregar ItemList.json
            if (fs::exists(gameDataPath + "/ItemList.json")) {
                if (!LoadItemsFromJson(gameDataPath + "/ItemList.json")) {
                    Log("Aviso: Falha ao carregar itens de ItemList.json");
                }
            } else {
                Log("Arquivo ItemList.json não encontrado em: " + gameDataPath);
            }
            
            // Tentar carregar Itemname.json
            if (fs::exists(gameDataPath + "/Itemname.json")) {
                if (!LoadItemNamesFromJson(gameDataPath + "/Itemname.json")) {
                    Log("Aviso: Falha ao carregar nomes de itens de Itemname.json");
                }
            } else {
                Log("Arquivo Itemname.json não encontrado em: " + gameDataPath);
            }
            
            // Tentar carregar ItemPrice.json (se existir)
            if (fs::exists(gameDataPath + "/ItemPrice.json")) {
                if (!LoadPricesFromJson(gameDataPath + "/ItemPrice.json")) {
                    Log("Aviso: Falha ao carregar preços de ItemPrice.json");
                }
            } else {
                Log("Arquivo ItemPrice.json não encontrado em: " + gameDataPath);
            }
            
            // Tentar carregar NPCs de arquivos relevantes
            LoadNPCsFromAvailableFiles(gameDataPath);
            
            // Tentar carregar mapas de arquivos relevantes
            LoadMapsFromAvailableFiles(gameDataPath);
            
            Log("Carregamento de arquivos JSON concluído.");
            return true;
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar arquivos JSON: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Carrega itens do arquivo ItemList.json
     * @param filePath Caminho para o arquivo
     * @return true se carregado com sucesso
     */
    bool LoadItemsFromJson(const std::string& filePath) {
        try {
            Log("Carregando itens de: " + filePath);
            
            // Abrir o arquivo
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo: " + filePath);
                return false;
            }
            
            // Ler o JSON
            json itemsJson;
            file >> itemsJson;
            file.close();
            
            // Processar cada item
            int loadedCount = 0;
            
            for (const auto& itemJson : itemsJson.items()) {
                try {
                    // Criar um novo objeto de item
                    auto item = std::make_shared<ItemData>();
                    
                    // Obter ID do item (chave ou campo "Index")
                    if (itemJson.value().contains("Index")) {
                        item->id = itemJson.value()["Index"].get<int>();
                    }
                    else {
                        // Tentar usar a chave como ID
                        item->id = std::stoi(itemJson.key());
                    }
                    
                    // Obter nome do item (pode ser preenchido depois por ItemName.json)
                    if (itemJson.value().contains("Name")) {
                        item->name = itemJson.value()["Name"].get<std::string>();
                    }
                    else if (itemJson.value().contains("name")) {
                        item->name = itemJson.value()["name"].get<std::string>();
                    }
                    else {
                        item->name = "Item " + std::to_string(item->id);
                    }
                    
                    // Obter preço base do item
                    if (itemJson.value().contains("Price")) {
                        item->price = itemJson.value()["Price"].get<int>();
                    }
                    else if (itemJson.value().contains("price")) {
                        item->price = itemJson.value()["price"].get<int>();
                    }
                    else {
                        item->price = 0; // Preço desconhecido
                    }
                    
                    // Obter tipo do item
                    if (itemJson.value().contains("Type")) {
                        item->type = itemJson.value()["Type"].get<std::string>();
                    }
                    else if (itemJson.value().contains("type")) {
                        item->type = itemJson.value()["type"].get<std::string>();
                    }
                    
                    // Obter descrição do item
                    if (itemJson.value().contains("Description")) {
                        item->description = itemJson.value()["Description"].get<std::string>();
                    }
                    else if (itemJson.value().contains("description")) {
                        item->description = itemJson.value()["description"].get<std::string>();
                    }
                    
                    // Obter requisitos do item
                    if (itemJson.value().contains("Requirements")) {
                        item->requirements = itemJson.value()["Requirements"].get<std::string>();
                    }
                    else if (itemJson.value().contains("requirements")) {
                        item->requirements = itemJson.value()["requirements"].get<std::string>();
                    }
                    
                    // Obter efeitos do item
                    if (itemJson.value().contains("Effects")) {
                        item->effects = itemJson.value()["Effects"].get<std::string>();
                    }
                    else if (itemJson.value().contains("effects")) {
                        item->effects = itemJson.value()["effects"].get<std::string>();
                    }
                    
                    // Obter outros atributos do item
                    for (const auto& attr : itemJson.value().items()) {
                        // Ignorar atributos já processados
                        if (attr.key() == "Index" || attr.key() == "Name" || attr.key() == "name" ||
                            attr.key() == "Price" || attr.key() == "price" ||
                            attr.key() == "Type" || attr.key() == "type" ||
                            attr.key() == "Description" || attr.key() == "description" ||
                            attr.key() == "Requirements" || attr.key() == "requirements" ||
                            attr.key() == "Effects" || attr.key() == "effects") {
                            continue;
                        }
                        
                        // Adicionar atributo
                        item->attributes[attr.key()] = attr.value().is_string() ? 
                            attr.value().get<std::string>() : attr.value().dump();
                    }
                    
                    // Adicionar o item aos mapas
                    m_itemsById[item->id] = item;
                    
                    std::string normalizedName = NormalizeName(item->name);
                    if (!normalizedName.empty()) {
                        m_itemsByName[normalizedName] = item;
                    }
                    
                    loadedCount++;
                }
                catch (const std::exception& e) {
                    Log("Aviso: Erro ao processar item: " + std::string(e.what()));
                }
            }
            
            Log("Itens carregados com sucesso: " + std::to_string(loadedCount));
            return loadedCount > 0;
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar itens de JSON: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Carrega nomes de itens do arquivo Itemname.json
     * @param filePath Caminho para o arquivo
     * @return true se carregado com sucesso
     */
    bool LoadItemNamesFromJson(const std::string& filePath) {
        try {
            Log("Carregando nomes de itens de: " + filePath);
            
            // Abrir o arquivo
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo: " + filePath);
                return false;
            }
            
            // Ler o JSON
            json namesJson;
            file >> namesJson;
            file.close();
            
            // Processar cada nome
            int updatedCount = 0;
            
            for (const auto& nameJson : namesJson.items()) {
                try {
                    // Obter ID do item e seu nome
                    int itemId = -1;
                    std::string itemName;
                    
                    if (nameJson.value().is_object()) {
                        // Formato objeto com id e name
                        if (nameJson.value().contains("id") && nameJson.value().contains("name")) {
                            itemId = nameJson.value()["id"].get<int>();
                            itemName = nameJson.value()["name"].get<std::string>();
                        }
                        else if (nameJson.value().contains("Index") && nameJson.value().contains("Name")) {
                            itemId = nameJson.value()["Index"].get<int>();
                            itemName = nameJson.value()["Name"].get<std::string>();
                        }
                    }
                    else if (nameJson.value().is_string()) {
                        // Formato de array com chave como id
                        itemId = std::stoi(nameJson.key());
                        itemName = nameJson.value().get<std::string>();
                    }
                    
                    if (itemId >= 0 && !itemName.empty()) {
                        // Verificar se o item já existe
                        auto it = m_itemsById.find(itemId);
                        if (it != m_itemsById.end()) {
                            // Atualizar nome do item
                            it->second->name = itemName;
                            
                            // Atualizar nome normalizado
                            std::string normalizedName = NormalizeName(itemName);
                            if (!normalizedName.empty()) {
                                m_itemsByName[normalizedName] = it->second;
                            }
                            
                            updatedCount++;
                        }
                        else {
                            // Criar um novo item com informações mínimas
                            auto item = std::make_shared<ItemData>();
                            item->id = itemId;
                            item->name = itemName;
                            item->price = 0; // Preço desconhecido
                            
                            // Adicionar o item aos mapas
                            m_itemsById[item->id] = item;
                            
                            std::string normalizedName = NormalizeName(item->name);
                            if (!normalizedName.empty()) {
                                m_itemsByName[normalizedName] = item;
                            }
                            
                            updatedCount++;
                        }
                    }
                }
                catch (const std::exception& e) {
                    Log("Aviso: Erro ao processar nome de item: " + std::string(e.what()));
                }
            }
            
            Log("Nomes de itens atualizados com sucesso: " + std::to_string(updatedCount));
            return updatedCount > 0;
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar nomes de itens de JSON: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Carrega preços de itens do arquivo ItemPrice.json
     * @param filePath Caminho para o arquivo
     * @return true se carregado com sucesso
     */
    bool LoadPricesFromJson(const std::string& filePath) {
        try {
            Log("Carregando preços de itens de: " + filePath);
            
            // Abrir o arquivo
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo: " + filePath);
                return false;
            }
            
            // Ler o JSON
            json pricesJson;
            file >> pricesJson;
            file.close();
            
            // Processar cada preço
            int updatedCount = 0;
            
            for (const auto& priceJson : pricesJson.items()) {
                try {
                    // Obter ID do item e seu preço
                    int itemId = -1;
                    int price = 0;
                    
                    // Tentar diferentes formatos possíveis
                    if (priceJson.value().is_object()) {
                        // Formato objeto com id e price
                        if (priceJson.value().contains("id") && priceJson.value().contains("price")) {
                            itemId = priceJson.value()["id"].get<int>();
                            price = priceJson.value()["price"].get<int>();
                        }
                        else if (priceJson.value().contains("Index") && priceJson.value().contains("Price")) {
                            itemId = priceJson.value()["Index"].get<int>();
                            price = priceJson.value()["Price"].get<int>();
                        }
                        else if (priceJson.value().contains("item_id") && priceJson.value().contains("price")) {
                            itemId = priceJson.value()["item_id"].get<int>();
                            price = priceJson.value()["price"].get<int>();
                        }
                    }
                    else if (priceJson.value().is_number()) {
                        // Formato de array com chave como id
                        itemId = std::stoi(priceJson.key());
                        price = priceJson.value().get<int>();
                    }
                    
                    if (itemId >= 0 && price > 0) {
                        // Verificar se o item já existe
                        auto it = m_itemsById.find(itemId);
                        if (it != m_itemsById.end()) {
                            // Atualizar preço do item
                            it->second->price = price;
                            updatedCount++;
                        }
                        else {
                            // Criar um novo item com informações mínimas
                            auto item = std::make_shared<ItemData>();
                            item->id = itemId;
                            item->name = "Item " + std::to_string(itemId);
                            item->price = price;
                            
                            // Adicionar o item aos mapas
                            m_itemsById[item->id] = item;
                            updatedCount++;
                        }
                    }
                }
                catch (const std::exception& e) {
                    Log("Aviso: Erro ao processar preço de item: " + std::string(e.what()));
                }
            }
            
            Log("Preços de itens atualizados com sucesso: " + std::to_string(updatedCount));
            return updatedCount > 0;
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar preços de itens de JSON: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Carrega NPCs de arquivos disponíveis
     * @param gameDataPath Caminho para os dados do jogo
     * @return true se pelo menos um NPC for carregado
     */
    bool LoadNPCsFromAvailableFiles(const std::string& gameDataPath) {
        try {
            Log("Procurando arquivos de NPCs...");
            
            int loadedCount = 0;
            
            // Lista de possíveis arquivos de NPCs
            std::vector<std::string> npcFiles = {
                "/NPC.json", 
                "/NPCList.json", 
                "/NPCData.json", 
                "/npc.json", 
                "/npclist.json",
                "/Merchant.json",
                "/MerchantList.json",
                "/ShopData.json",
                "/Shop.json"
            };
            
            // Tentar carregar cada arquivo
            for (const auto& npcFile : npcFiles) {
                std::string filePath = gameDataPath + npcFile;
                if (fs::exists(filePath)) {
                    Log("Encontrado arquivo de NPCs: " + filePath);
                    
                    if (LoadNPCsFromJson(filePath)) {
                        loadedCount++;
                    }
                }
            }
            
            // Verificar txt ou outro formato se necessário
            std::vector<std::string> textFiles = {
                "/NPC.txt", 
                "/Merchant.txt",
                "/ShopData.txt"
            };
            
            for (const auto& textFile : textFiles) {
                std::string filePath = gameDataPath + textFile;
                if (fs::exists(filePath)) {
                    Log("Encontrado arquivo de texto de NPCs: " + filePath);
                    
                    // Implementar parser de texto se necessário
                    // ...
                }
            }
            
            // Se não encontrou arquivos específicos, procurar em cdata.json
            if (loadedCount == 0 && fs::exists(gameDataPath + "/cdata.json")) {
                Log("Tentando extrair NPCs de cdata.json...");
                
                if (ExtractNPCsFromCData(gameDataPath + "/cdata.json")) {
                    loadedCount++;
                }
            }
            
            // Em último caso, adicionar alguns NPCs conhecidos do jogo WYD
            if (m_npcsById.empty()) {
                Log("Adicionando NPCs conhecidos do jogo WYD...");
                AddKnownNPCs();
                loadedCount++;
            }
            
            Log("Total de NPCs carregados: " + std::to_string(m_npcsById.size()));
            return !m_npcsById.empty();
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar NPCs: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Carrega NPCs de um arquivo JSON
     * @param filePath Caminho para o arquivo
     * @return true se carregado com sucesso
     */
    bool LoadNPCsFromJson(const std::string& filePath) {
        try {
            Log("Carregando NPCs de: " + filePath);
            
            // Abrir o arquivo
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo: " + filePath);
                return false;
            }
            
            // Ler o JSON
            json npcsJson;
            file >> npcsJson;
            file.close();
            
            // Processar cada NPC
            int loadedCount = 0;
            
            // Verificar se é um array ou objeto
            if (npcsJson.is_array()) {
                for (const auto& npcJson : npcsJson) {
                    if (ProcessNPCJson(npcJson)) {
                        loadedCount++;
                    }
                }
            }
            else if (npcsJson.is_object()) {
                // Verificar se o objeto tem npc como chaves
                for (const auto& npcEntry : npcsJson.items()) {
                    if (ProcessNPCJson(npcEntry.value(), npcEntry.key())) {
                        loadedCount++;
                    }
                }
            }
            
            Log("NPCs carregados com sucesso: " + std::to_string(loadedCount));
            return loadedCount > 0;
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar NPCs de JSON: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Processa dados JSON de um NPC
     * @param npcJson Dados JSON do NPC
     * @param keyName Nome da chave (opcional)
     * @return true se processado com sucesso
     */
    bool ProcessNPCJson(const json& npcJson, const std::string& keyName = "") {
        try {
            // Criar um novo objeto de NPC
            auto npc = std::make_shared<NPCData>();
            
            // Obter ID do NPC
            if (npcJson.contains("id")) {
                npc->id = npcJson["id"].get<int>();
            }
            else if (npcJson.contains("Index")) {
                npc->id = npcJson["Index"].get<int>();
            }
            else if (npcJson.contains("npc_id")) {
                npc->id = npcJson["npc_id"].get<int>();
            }
            else if (!keyName.empty()) {
                // Tentar usar a chave como ID
                try {
                    npc->id = std::stoi(keyName);
                }
                catch (...) {
                    // Se não for um número, gerar um ID baseado no hash do nome
                    npc->id = std::hash<std::string>{}(keyName) % 10000;
                }
            }
            else {
                // Gerar um ID único
                static int nextNpcId = 10000;
                npc->id = nextNpcId++;
            }
            
            // Obter nome do NPC
            if (npcJson.contains("name")) {
                npc->name = npcJson["name"].get<std::string>();
            }
            else if (npcJson.contains("Name")) {
                npc->name = npcJson["Name"].get<std::string>();
            }
            else if (!keyName.empty() && !std::all_of(keyName.begin(), keyName.end(), ::isdigit)) {
                npc->name = keyName;
            }
            else {
                npc->name = "NPC " + std::to_string(npc->id);
            }
            
            // Obter localização do NPC
            if (npcJson.contains("location")) {
                npc->location = npcJson["location"].get<std::string>();
            }
            else if (npcJson.contains("Location")) {
                npc->location = npcJson["Location"].get<std::string>();
            }
            else if (npcJson.contains("map")) {
                npc->location = npcJson["map"].get<std::string>();
            }
            else if (npcJson.contains("Map")) {
                npc->location = npcJson["Map"].get<std::string>();
            }
            
            // Obter tipo do NPC
            if (npcJson.contains("type")) {
                npc->type = npcJson["type"].get<std::string>();
            }
            else if (npcJson.contains("Type")) {
                npc->type = npcJson["Type"].get<std::string>();
            }
            
            // Obter itens à venda
            if (npcJson.contains("items") && npcJson["items"].is_array()) {
                for (const auto& item : npcJson["items"]) {
                    if (item.is_number()) {
                        npc->itemsForSale.push_back(item.get<int>());
                    }
                    else if (item.is_object() && item.contains("id")) {
                        int itemId = item["id"].get<int>();
                        npc->itemsForSale.push_back(itemId);
                        
                        // Verificar se tem preço específico
                        if (item.contains("price")) {
                            npc->itemPrices[itemId] = item["price"].get<int>();
                        }
                    }
                }
            }
            else if (npcJson.contains("Items") && npcJson["Items"].is_array()) {
                for (const auto& item : npcJson["Items"]) {
                    if (item.is_number()) {
                        npc->itemsForSale.push_back(item.get<int>());
                    }
                    else if (item.is_object() && item.contains("Index")) {
                        int itemId = item["Index"].get<int>();
                        npc->itemsForSale.push_back(itemId);
                        
                        // Verificar se tem preço específico
                        if (item.contains("Price")) {
                            npc->itemPrices[itemId] = item["Price"].get<int>();
                        }
                    }
                }
            }
            else if (npcJson.contains("itemsForSale") && npcJson["itemsForSale"].is_array()) {
                for (const auto& item : npcJson["itemsForSale"]) {
                    if (item.is_number()) {
                        npc->itemsForSale.push_back(item.get<int>());
                    }
                }
            }
            else if (npcJson.contains("shop") && npcJson["shop"].is_object()) {
                // Formato alternativo
                for (const auto& shopItem : npcJson["shop"].items()) {
                    try {
                        int itemId = std::stoi(shopItem.key());
                        npc->itemsForSale.push_back(itemId);
                        
                        // Verificar se tem preço específico
                        if (shopItem.value().is_number()) {
                            npc->itemPrices[itemId] = shopItem.value().get<int>();
                        }
                        else if (shopItem.value().is_object() && shopItem.value().contains("price")) {
                            npc->itemPrices[itemId] = shopItem.value()["price"].get<int>();
                        }
                    }
                    catch (...) {
                        // Ignorar se a chave não for um número
                    }
                }
            }
            
            // Obter preços específicos de itens
            if (npcJson.contains("prices") && npcJson["prices"].is_object()) {
                for (const auto& price : npcJson["prices"].items()) {
                    try {
                        int itemId = std::stoi(price.key());
                        if (price.value().is_number()) {
                            npc->itemPrices[itemId] = price.value().get<int>();
                        }
                    }
                    catch (...) {
                        // Ignorar se a chave não for um número
                    }
                }
            }
            else if (npcJson.contains("Prices") && npcJson["Prices"].is_object()) {
                for (const auto& price : npcJson["Prices"].items()) {
                    try {
                        int itemId = std::stoi(price.key());
                        if (price.value().is_number()) {
                            npc->itemPrices[itemId] = price.value().get<int>();
                        }
                    }
                    catch (...) {
                        // Ignorar se a chave não for um número
                    }
                }
            }
            
            // Obter outros atributos do NPC
            for (const auto& attr : npcJson.items()) {
                // Ignorar atributos já processados
                if (attr.key() == "id" || attr.key() == "Index" || attr.key() == "npc_id" ||
                    attr.key() == "name" || attr.key() == "Name" ||
                    attr.key() == "location" || attr.key() == "Location" ||
                    attr.key() == "map" || attr.key() == "Map" ||
                    attr.key() == "type" || attr.key() == "Type" ||
                    attr.key() == "items" || attr.key() == "Items" ||
                    attr.key() == "itemsForSale" || attr.key() == "shop" ||
                    attr.key() == "prices" || attr.key() == "Prices") {
                    continue;
                }
                
                // Adicionar atributo
                npc->attributes[attr.key()] = attr.value().is_string() ? 
                    attr.value().get<std::string>() : attr.value().dump();
            }
            
            // Adicionar o NPC aos mapas
            m_npcsById[npc->id] = npc;
            
            std::string normalizedName = NormalizeName(npc->name);
            if (!normalizedName.empty()) {
                m_npcsByName[normalizedName] = npc;
            }
            
            return true;
        }
        catch (const std::exception& e) {
            Log("Aviso: Erro ao processar NPC: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Extrai informações de NPCs do arquivo cdata.json
     * @param filePath Caminho para o arquivo
     * @return true se extraído com sucesso
     */
    bool ExtractNPCsFromCData(const std::string& filePath) {
        try {
            Log("Extraindo NPCs de: " + filePath);
            
            // Abrir o arquivo
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo: " + filePath);
                return false;
            }
            
            // Ler o JSON
            json cdataJson;
            file >> cdataJson;
            file.close();
            
            // Processar cada NPC
            int loadedCount = 0;
            
            // Procurar estruturas que possam conter NPCs
            if (cdataJson.contains("npcs") && cdataJson["npcs"].is_array()) {
                for (const auto& npcJson : cdataJson["npcs"]) {
                    if (ProcessNPCJson(npcJson)) {
                        loadedCount++;
                    }
                }
            }
            else if (cdataJson.contains("Npcs") && cdataJson["Npcs"].is_array()) {
                for (const auto& npcJson : cdataJson["Npcs"]) {
                    if (ProcessNPCJson(npcJson)) {
                        loadedCount++;
                    }
                }
            }
            else if (cdataJson.contains("merchants") && cdataJson["merchants"].is_array()) {
                for (const auto& npcJson : cdataJson["merchants"]) {
                    if (ProcessNPCJson(npcJson)) {
                        loadedCount++;
                    }
                }
            }
            else if (cdataJson.contains("Merchants") && cdataJson["Merchants"].is_array()) {
                for (const auto& npcJson : cdataJson["Merchants"]) {
                    if (ProcessNPCJson(npcJson)) {
                        loadedCount++;
                    }
                }
            }
            else {
                // Tentar encontrar NPCs em outras estruturas
                for (const auto& entry : cdataJson.items()) {
                    if (entry.value().is_object() && 
                        (entry.value().contains("name") || entry.value().contains("Name")) &&
                        (entry.value().contains("type") || entry.value().contains("Type") ||
                         entry.value().contains("items") || entry.value().contains("Items"))) {
                        
                        if (ProcessNPCJson(entry.value(), entry.key())) {
                            loadedCount++;
                        }
                    }
                }
            }
            
            Log("NPCs extraídos de cdata.json: " + std::to_string(loadedCount));
            return loadedCount > 0;
        }
        catch (const std::exception& e) {
            Log("Erro ao extrair NPCs de cdata.json: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Adiciona NPCs conhecidos do jogo WYD
     * @return true se adicionado com sucesso
     */
    bool AddKnownNPCs() {
        try {
            Log("Adicionando NPCs conhecidos do jogo WYD...");
            
            // Lista de NPCs conhecidos do jogo WYD
            std::vector<std::tuple<int, std::string, std::string>> knownNPCs = {
                // id, nome, localização
                std::make_tuple(1, "AKI", "Armia"),
                std::make_tuple(2, "BORA", "Armia"),
                std::make_tuple(3, "RED", "Armia"),
                std::make_tuple(4, "LISA", "Armia"),
                std::make_tuple(5, "JEFF", "Armia"),
                std::make_tuple(6, "ELY", "Erion"),
                std::make_tuple(7, "SHIRA", "Erion"),
                std::make_tuple(8, "NIAMY", "Azran"),
                std::make_tuple(9, "KORA", "Noatun"),
                std::make_tuple(10, "ORUM", "Noatun"),
                std::make_tuple(11, "JADE", "Noatun"),
                std::make_tuple(12, "LANIS", "Noatun"),
                std::make_tuple(13, "KING", "Armia"),
                std::make_tuple(14, "BISHOP", "Armia")
            };
            
            // Adicionar cada NPC
            for (const auto& npcInfo : knownNPCs) {
                auto npc = std::make_shared<NPCData>();
                npc->id = std::get<0>(npcInfo);
                npc->name = std::get<1>(npcInfo);
                npc->location = std::get<2>(npcInfo);
                npc->type = "Merchant";
                
                // Adicionar o NPC aos mapas
                m_npcsById[npc->id] = npc;
                
                std::string normalizedName = NormalizeName(npc->name);
                if (!normalizedName.empty()) {
                    m_npcsByName[normalizedName] = npc;
                }
            }
            
            // Definir itens e preços para o NPC AKI como exemplo
            if (m_npcsByName.find("aki") != m_npcsByName.end()) {
                auto aki = m_npcsByName["aki"];
                
                // Itens comuns vendidos pelo AKI
                std::vector<std::pair<int, int>> akiItems = {
                    {1, 500},    // Potion of Vigor - 500 gold
                    {2, 2000},   // Potion of Wisdom - 2000 gold
                    {3, 5000},   // Potion of Resistance - 5000 gold
                    {7, 1000},   // Town Portal - 1000 gold
                    {10, 5000},  // Small Healing Potion - 5000 gold
                    {11, 15000}, // Medium Healing Potion - 15000 gold
                    {12, 25000}, // Large Healing Potion - 25000 gold
                    {19, 5000},  // Small Mana Potion - 5000 gold
                    {20, 15000}  // Medium Mana Potion - 15000 gold
                };
                
                for (const auto& item : akiItems) {
                    aki->itemsForSale.push_back(item.first);
                    aki->itemPrices[item.first] = item.second;
                }
            }
            
            Log("NPCs conhecidos adicionados com sucesso: " + std::to_string(knownNPCs.size()));
            return true;
        }
        catch (const std::exception& e) {
            Log("Erro ao adicionar NPCs conhecidos: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Carrega mapas de arquivos disponíveis
     * @param gameDataPath Caminho para os dados do jogo
     * @return true se pelo menos um mapa for carregado
     */
    bool LoadMapsFromAvailableFiles(const std::string& gameDataPath) {
        try {
            Log("Procurando arquivos de mapas...");
            
            int loadedCount = 0;
            
            // Lista de possíveis arquivos de mapas
            std::vector<std::string> mapFiles = {
                "/MapList.json", 
                "/Map.json", 
                "/MapData.json", 
                "/maplist.json", 
                "/map.json"
            };
            
            // Tentar carregar cada arquivo
            for (const auto& mapFile : mapFiles) {
                std::string filePath = gameDataPath + mapFile;
                if (fs::exists(filePath)) {
                    Log("Encontrado arquivo de mapas: " + filePath);
                    
                    if (LoadMapsFromJson(filePath)) {
                        loadedCount++;
                    }
                }
            }
            
            // Adicionar mapas conhecidos se nenhum foi carregado
            if (m_mapsById.empty()) {
                Log("Adicionando mapas conhecidos do jogo WYD...");
                AddKnownMaps();
                loadedCount++;
            }
            
            Log("Total de mapas carregados: " + std::to_string(m_mapsById.size()));
            return !m_mapsById.empty();
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar mapas: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Carrega mapas de um arquivo JSON
     * @param filePath Caminho para o arquivo
     * @return true se carregado com sucesso
     */
    bool LoadMapsFromJson(const std::string& filePath) {
        try {
            Log("Carregando mapas de: " + filePath);
            
            // Abrir o arquivo
            std::ifstream file(filePath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo: " + filePath);
                return false;
            }
            
            // Ler o JSON
            json mapsJson;
            file >> mapsJson;
            file.close();
            
            // Processar cada mapa
            int loadedCount = 0;
            
            // Verificar se é um array ou objeto
            if (mapsJson.is_array()) {
                for (const auto& mapJson : mapsJson) {
                    if (ProcessMapJson(mapJson)) {
                        loadedCount++;
                    }
                }
            }
            else if (mapsJson.is_object()) {
                // Verificar se o objeto tem mapas como chaves
                for (const auto& mapEntry : mapsJson.items()) {
                    if (ProcessMapJson(mapEntry.value(), mapEntry.key())) {
                        loadedCount++;
                    }
                }
            }
            
            Log("Mapas carregados com sucesso: " + std::to_string(loadedCount));
            return loadedCount > 0;
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar mapas de JSON: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Processa dados JSON de um mapa
     * @param mapJson Dados JSON do mapa
     * @param keyName Nome da chave (opcional)
     * @return true se processado com sucesso
     */
    bool ProcessMapJson(const json& mapJson, const std::string& keyName = "") {
        try {
            // Criar um novo objeto de mapa
            auto map = std::make_shared<MapData>();
            
            // Obter ID do mapa
            if (mapJson.contains("id")) {
                map->id = mapJson["id"].get<int>();
            }
            else if (mapJson.contains("Index")) {
                map->id = mapJson["Index"].get<int>();
            }
            else if (mapJson.contains("map_id")) {
                map->id = mapJson["map_id"].get<int>();
            }
            else if (!keyName.empty()) {
                // Tentar usar a chave como ID
                try {
                    map->id = std::stoi(keyName);
                }
                catch (...) {
                    // Se não for um número, gerar um ID baseado no hash do nome
                    map->id = std::hash<std::string>{}(keyName) % 10000;
                }
            }
            else {
                // Gerar um ID único
                static int nextMapId = 1000;
                map->id = nextMapId++;
            }
            
            // Obter nome do mapa
            if (mapJson.contains("name")) {
                map->name = mapJson["name"].get<std::string>();
            }
            else if (mapJson.contains("Name")) {
                map->name = mapJson["Name"].get<std::string>();
            }
            else if (!keyName.empty() && !std::all_of(keyName.begin(), keyName.end(), ::isdigit)) {
                map->name = keyName;
            }
            else {
                map->name = "Map " + std::to_string(map->id);
            }
            
            // Obter NPCs no mapa
            if (mapJson.contains("npcs") && mapJson["npcs"].is_array()) {
                for (const auto& npc : mapJson["npcs"]) {
                    if (npc.is_number()) {
                        map->npcs.push_back(npc.get<int>());
                    }
                    else if (npc.is_object() && npc.contains("id")) {
                        map->npcs.push_back(npc["id"].get<int>());
                    }
                }
            }
            else if (mapJson.contains("Npcs") && mapJson["Npcs"].is_array()) {
                for (const auto& npc : mapJson["Npcs"]) {
                    if (npc.is_number()) {
                        map->npcs.push_back(npc.get<int>());
                    }
                    else if (npc.is_object() && npc.contains("Index")) {
                        map->npcs.push_back(npc["Index"].get<int>());
                    }
                }
            }
            
            // Obter mobs no mapa
            if (mapJson.contains("mobs") && mapJson["mobs"].is_array()) {
                for (const auto& mob : mapJson["mobs"]) {
                    if (mob.is_number()) {
                        map->mobs.push_back(mob.get<int>());
                    }
                    else if (mob.is_object() && mob.contains("id")) {
                        map->mobs.push_back(mob["id"].get<int>());
                    }
                }
            }
            else if (mapJson.contains("Mobs") && mapJson["Mobs"].is_array()) {
                for (const auto& mob : mapJson["Mobs"]) {
                    if (mob.is_number()) {
                        map->mobs.push_back(mob.get<int>());
                    }
                    else if (mob.is_object() && mob.contains("Index")) {
                        map->mobs.push_back(mob["Index"].get<int>());
                    }
                }
            }
            
            // Obter outros atributos do mapa
            for (const auto& attr : mapJson.items()) {
                // Ignorar atributos já processados
                if (attr.key() == "id" || attr.key() == "Index" || attr.key() == "map_id" ||
                    attr.key() == "name" || attr.key() == "Name" ||
                    attr.key() == "npcs" || attr.key() == "Npcs" ||
                    attr.key() == "mobs" || attr.key() == "Mobs") {
                    continue;
                }
                
                // Adicionar atributo
                map->attributes[attr.key()] = attr.value().is_string() ? 
                    attr.value().get<std::string>() : attr.value().dump();
            }
            
            // Adicionar o mapa aos mapas
            m_mapsById[map->id] = map;
            
            std::string normalizedName = NormalizeName(map->name);
            if (!normalizedName.empty()) {
                m_mapsByName[normalizedName] = map;
            }
            
            // Para cada NPC do mapa, associá-lo ao mapa se existir
            for (int npcId : map->npcs) {
                auto it = m_npcsById.find(npcId);
                if (it != m_npcsById.end() && it->second->location.empty()) {
                    it->second->location = map->name;
                }
            }
            
            return true;
        }
        catch (const std::exception& e) {
            Log("Aviso: Erro ao processar mapa: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Adiciona mapas conhecidos do jogo WYD
     * @return true se adicionado com sucesso
     */
    bool AddKnownMaps() {
        try {
            Log("Adicionando mapas conhecidos do jogo WYD...");
            
            // Lista de mapas conhecidos do jogo WYD
            std::vector<std::tuple<int, std::string>> knownMaps = {
                // id, nome
                std::make_tuple(0, "Armia"),
                std::make_tuple(1, "Erion"),
                std::make_tuple(2, "Azran"),
                std::make_tuple(3, "Noatun"),
                std::make_tuple(4, "Nippleheim"),
                std::make_tuple(5, "Dungeon"),
                std::make_tuple(6, "Ruins"),
                std::make_tuple(7, "Maze"),
                std::make_tuple(8, "Oasis")
            };
            
            // Adicionar cada mapa
            for (const auto& mapInfo : knownMaps) {
                auto map = std::make_shared<MapData>();
                map->id = std::get<0>(mapInfo);
                map->name = std::get<1>(mapInfo);
                
                // Adicionar o mapa aos mapas
                m_mapsById[map->id] = map;
                
                std::string normalizedName = NormalizeName(map->name);
                if (!normalizedName.empty()) {
                    m_mapsByName[normalizedName] = map;
                }
            }
            
            // Associar NPCs aos mapas
            for (auto& npcPair : m_npcsById) {
                auto& npc = npcPair.second;
                if (!npc->location.empty()) {
                    std::string normalizedLocation = NormalizeName(npc->location);
                    if (m_mapsByName.find(normalizedLocation) != m_mapsByName.end()) {
                        int mapId = m_mapsByName[normalizedLocation]->id;
                        if (std::find(m_mapsById[mapId]->npcs.begin(), 
                                     m_mapsById[mapId]->npcs.end(), 
                                     npc->id) == m_mapsById[mapId]->npcs.end()) {
                            m_mapsById[mapId]->npcs.push_back(npc->id);
                        }
                    }
                }
            }
            
            Log("Mapas conhecidos adicionados com sucesso: " + std::to_string(knownMaps.size()));
            return true;
        }
        catch (const std::exception& e) {
            Log("Erro ao adicionar mapas conhecidos: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Extrai informações úteis de uma pergunta
     * @param question Pergunta em linguagem natural
     * @return Par contendo entidade (item/npc) e contexto (preço/localização)
     */
    std::pair<std::string, std::string> ExtractQueryInfo(const std::string& question) {
        std::string entity;
        std::string context;
        
        // Converter para minúsculas para facilitar
        std::string lowerQuestion = question;
        std::transform(lowerQuestion.begin(), lowerQuestion.end(), lowerQuestion.begin(), ::tolower);
        
        // Identificar contexto (preço, local, etc)
        if (lowerQuestion.find("preco") != std::string::npos || 
            lowerQuestion.find("preço") != std::string::npos ||
            lowerQuestion.find("custa") != std::string::npos ||
            lowerQuestion.find("valor") != std::string::npos) {
            context = "price";
        }
        else if (lowerQuestion.find("onde") != std::string::npos || 
                lowerQuestion.find("local") != std::string::npos ||
                lowerQuestion.find("mapa") != std::string::npos) {
            context = "location";
        }
        else if (lowerQuestion.find("vend") != std::string::npos) {
            context = "sells";
        }
        else if (lowerQuestion.find("compr") != std::string::npos) {
            context = "buys";
        }
        
        // Identificar entidade (item ou NPC)
        
        // Padrão: "item X do NPC Y"
        std::regex itemNpcPattern(R"((?:item|poção|potion|arma|weapon|armor|armadura|equipamento|equipment)\s+(\w+(?:\s+\w+)*)\s+(?:do|da|de|no|na)\s+(?:npc|vendedor|merchant|comerciante|loja|shop)?\s*(\w+))", std::regex_constants::icase);
        std::smatch itemNpcMatch;
        
        // Padrão: "NPC X vende item Y"
        std::regex npcItemPattern(R"((?:npc|vendedor|merchant|comerciante|loja|shop)?\s*(\w+)\s+(?:vende|compra|oferece|oferece|tem|possui|serve)\s+(?:item|poção|potion|arma|weapon|armor|armadura|equipamento|equipment)?\s*(\w+(?:\s+\w+)*))", std::regex_constants::icase);
        std::smatch npcItemMatch;
        
        // Padrão: "preço de X"
        std::regex priceOfPattern(R"((?:preço|preco|valor|custo|custa)\s+(?:do|da|de)?\s+(\w+(?:\s+\w+)*))", std::regex_constants::icase);
        std::smatch priceOfMatch;
        
        // Padrão: "onde encontrar X"
        std::regex wherePattern(R"((?:onde|local|localização|localizacao|mapa)\s+(?:encontr\w+|achar|está|esta|fica)\s+(?:o|a|os|as)?\s+(\w+(?:\s+\w+)*))", std::regex_constants::icase);
        std::smatch whereMatch;
        
        if (std::regex_search(lowerQuestion, itemNpcMatch, itemNpcPattern)) {
            // Formato: "item X do NPC Y"
            entity = itemNpcMatch[1].str() + "|" + itemNpcMatch[2].str();
            if (context.empty()) context = "price";
        }
        else if (std::regex_search(lowerQuestion, npcItemMatch, npcItemPattern)) {
            // Formato: "NPC X vende item Y"
            entity = npcItemMatch[2].str() + "|" + npcItemMatch[1].str();
            if (context.empty()) context = "price";
        }
        else if (std::regex_search(lowerQuestion, priceOfMatch, priceOfPattern)) {
            // Formato: "preço de X"
            entity = priceOfMatch[1].str();
            if (context.empty()) context = "price";
        }
        else if (std::regex_search(lowerQuestion, whereMatch, wherePattern)) {
            // Formato: "onde encontrar X"
            entity = whereMatch[1].str();
            if (context.empty()) context = "location";
        }
        else {
            // Tenta extrair qualquer nome próprio ou palavra-chave
            std::regex entityPattern(R"((?:o|a|os|as|um|uma|uns|umas)?\s+(\w+))", std::regex_constants::icase);
            std::smatch entityMatch;
            
            if (std::regex_search(lowerQuestion, entityMatch, entityPattern)) {
                entity = entityMatch[1].str();
            }
        }
        
        return std::make_pair(entity, context);
    }
};

// Implementação da classe WYDSpecificKnowledge

WYDSpecificKnowledge::WYDSpecificKnowledge(std::shared_ptr<GameKnowledgeBase> knowledgeBase)
    : m_pImpl(std::make_unique<Impl>(knowledgeBase))
    , m_knowledgeBase(knowledgeBase) {
}

WYDSpecificKnowledge::~WYDSpecificKnowledge() = default;

bool WYDSpecificKnowledge::Initialize(const std::string& gameDataPath) {
    try {
        Log("Inicializando conhecimento específico do WYD...");
        
        // Armazenar o caminho para os dados do jogo
        m_pImpl->m_gameDataPath = gameDataPath;
        
        // Tentar carregar dados básicos
        bool loadedSome = m_pImpl->LoadJsonFiles(gameDataPath);
        
        if (!loadedSome) {
            Log("Aviso: Nenhum dado foi carregado durante a inicialização");
        }
        
        m_pImpl->m_initialized = true;
        Log("Conhecimento específico do WYD inicializado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao inicializar conhecimento específico do WYD: " + std::string(e.what()));
        return false;
    }
}

bool WYDSpecificKnowledge::LoadGameData(std::function<void(float, const std::string&)> progressCallback) {
    try {
        if (!m_pImpl->m_initialized) {
            Log("Erro: Conhecimento específico do WYD não inicializado");
            return false;
        }
        
        Log("Carregando dados do jogo WYD...");
        
        // Callback padrão se não fornecido
        auto callback = progressCallback ? progressCallback : 
            [this](float progress, const std::string& message) {
                Log(message + " (" + std::to_string(static_cast<int>(progress * 100)) + "%)");
            };
        
        // Carregar dados de itens
        callback(0.1f, "Carregando dados de itens");
        bool itemsLoaded = m_pImpl->LoadItemsFromJson(m_pImpl->m_gameDataPath + "/ItemList.json");
        bool namesLoaded = m_pImpl->LoadItemNamesFromJson(m_pImpl->m_gameDataPath + "/Itemname.json");
        
        if (!itemsLoaded && !namesLoaded) {
            Log("Aviso: Não foi possível carregar dados de itens");
        }
        
        // Carregar dados de NPCs
        callback(0.4f, "Carregando dados de NPCs");
        bool npcsLoaded = m_pImpl->LoadNPCsFromAvailableFiles(m_pImpl->m_gameDataPath);
        
        if (!npcsLoaded) {
            Log("Aviso: Não foi possível carregar dados de NPCs");
        }
        
        // Carregar dados de mapas
        callback(0.7f, "Carregando dados de mapas");
        bool mapsLoaded = m_pImpl->LoadMapsFromAvailableFiles(m_pImpl->m_gameDataPath);
        
        if (!mapsLoaded) {
            Log("Aviso: Não foi possível carregar dados de mapas");
        }
        
        // Carregar dados de preços
        callback(0.9f, "Carregando dados de preços");
        bool pricesLoaded = m_pImpl->LoadPricesFromJson(m_pImpl->m_gameDataPath + "/ItemPrice.json");
        
        callback(1.0f, "Carregamento concluído");
        
        Log("Dados do jogo WYD carregados com sucesso");
        Log("Itens: " + std::to_string(m_pImpl->m_itemsById.size()));
        Log("NPCs: " + std::to_string(m_pImpl->m_npcsById.size()));
        Log("Mapas: " + std::to_string(m_pImpl->m_mapsById.size()));
        
        return itemsLoaded || namesLoaded || npcsLoaded || mapsLoaded || pricesLoaded;
    }
    catch (const std::exception& e) {
        Log("Erro ao carregar dados do jogo WYD: " + std::string(e.what()));
        return false;
    }
}

std::shared_ptr<ItemData> WYDSpecificKnowledge::GetItemInfo(const std::string& itemNameOrId) {
    try {
        if (!m_pImpl->m_initialized) {
            Log("Erro: Conhecimento específico do WYD não inicializado");
            return nullptr;
        }
        
        // Verificar se é um ID numérico
        if (std::all_of(itemNameOrId.begin(), itemNameOrId.end(), ::isdigit)) {
            int itemId = std::stoi(itemNameOrId);
            auto it = m_pImpl->m_itemsById.find(itemId);
            if (it != m_pImpl->m_itemsById.end()) {
                return it->second;
            }
        }
        
        // Verificar pelo nome normalizado
        std::string normalizedName = m_pImpl->NormalizeName(itemNameOrId);
        if (!normalizedName.empty()) {
            auto it = m_pImpl->m_itemsByName.find(normalizedName);
            if (it != m_pImpl->m_itemsByName.end()) {
                return it->second;
            }
        }
        
        // Buscar por nome parcial
        for (const auto& item : m_pImpl->m_itemsByName) {
            if (item.first.find(normalizedName) != std::string::npos) {
                return item.second;
            }
        }
        
        return nullptr;
    }
    catch (const std::exception& e) {
        Log("Erro ao buscar informações de item: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<NPCData> WYDSpecificKnowledge::GetNPCInfo(const std::string& npcNameOrId) {
    try {
        if (!m_pImpl->m_initialized) {
            Log("Erro: Conhecimento específico do WYD não inicializado");
            return nullptr;
        }
        
        // Verificar se é um ID numérico
        if (std::all_of(npcNameOrId.begin(), npcNameOrId.end(), ::isdigit)) {
            int npcId = std::stoi(npcNameOrId);
            auto it = m_pImpl->m_npcsById.find(npcId);
            if (it != m_pImpl->m_npcsById.end()) {
                return it->second;
            }
        }
        
        // Verificar pelo nome normalizado
        std::string normalizedName = m_pImpl->NormalizeName(npcNameOrId);
        if (!normalizedName.empty()) {
            auto it = m_pImpl->m_npcsByName.find(normalizedName);
            if (it != m_pImpl->m_npcsByName.end()) {
                return it->second;
            }
        }
        
        // Buscar por nome parcial
        for (const auto& npc : m_pImpl->m_npcsByName) {
            if (npc.first.find(normalizedName) != std::string::npos) {
                return npc.second;
            }
        }
        
        return nullptr;
    }
    catch (const std::exception& e) {
        Log("Erro ao buscar informações de NPC: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<MapData> WYDSpecificKnowledge::GetMapInfo(const std::string& mapNameOrId) {
    try {
        if (!m_pImpl->m_initialized) {
            Log("Erro: Conhecimento específico do WYD não inicializado");
            return nullptr;
        }
        
        // Verificar se é um ID numérico
        if (std::all_of(mapNameOrId.begin(), mapNameOrId.end(), ::isdigit)) {
            int mapId = std::stoi(mapNameOrId);
            auto it = m_pImpl->m_mapsById.find(mapId);
            if (it != m_pImpl->m_mapsById.end()) {
                return it->second;
            }
        }
        
        // Verificar pelo nome normalizado
        std::string normalizedName = m_pImpl->NormalizeName(mapNameOrId);
        if (!normalizedName.empty()) {
            auto it = m_pImpl->m_mapsByName.find(normalizedName);
            if (it != m_pImpl->m_mapsByName.end()) {
                return it->second;
            }
        }
        
        // Buscar por nome parcial
        for (const auto& map : m_pImpl->m_mapsByName) {
            if (map.first.find(normalizedName) != std::string::npos) {
                return map.second;
            }
        }
        
        return nullptr;
    }
    catch (const std::exception& e) {
        Log("Erro ao buscar informações de mapa: " + std::string(e.what()));
        return nullptr;
    }
}

int WYDSpecificKnowledge::GetItemPriceAtNPC(const std::string& itemNameOrId, const std::string& npcNameOrId) {
    try {
        if (!m_pImpl->m_initialized) {
            Log("Erro: Conhecimento específico do WYD não inicializado");
            return -1;
        }
        
        // Obter informações do item
        auto item = GetItemInfo(itemNameOrId);
        if (!item) {
            Log("Item não encontrado: " + itemNameOrId);
            return -1;
        }
        
        // Obter informações do NPC
        auto npc = GetNPCInfo(npcNameOrId);
        if (!npc) {
            Log("NPC não encontrado: " + npcNameOrId);
            return -1;
        }
        
        // Verificar se o NPC vende este item
        if (std::find(npc->itemsForSale.begin(), npc->itemsForSale.end(), item->id) == npc->itemsForSale.end()) {
            Log("O NPC " + npc->name + " não vende o item " + item->name);
            return -1;
        }
        
        // Verificar se o NPC tem um preço específico para este item
        auto priceIt = npc->itemPrices.find(item->id);
        if (priceIt != npc->itemPrices.end()) {
            return priceIt->second;
        }
        
        // Retornar o preço base do item
        return item->price;
    }
    catch (const std::exception& e) {
        Log("Erro ao buscar preço de item: " + std::string(e.what()));
        return -1;
    }
}

std::vector<std::shared_ptr<NPCData>> WYDSpecificKnowledge::GetNPCsSellingItem(const std::string& itemNameOrId) {
    std::vector<std::shared_ptr<NPCData>> result;
    
    try {
        if (!m_pImpl->m_initialized) {
            Log("Erro: Conhecimento específico do WYD não inicializado");
            return result;
        }
        
        // Obter informações do item
        auto item = GetItemInfo(itemNameOrId);
        if (!item) {
            Log("Item não encontrado: " + itemNameOrId);
            return result;
        }
        
        // Buscar todos os NPCs que vendem este item
        for (const auto& npcPair : m_pImpl->m_npcsById) {
            auto npc = npcPair.second;
            if (std::find(npc->itemsForSale.begin(), npc->itemsForSale.end(), item->id) != npc->itemsForSale.end()) {
                result.push_back(npc);
            }
        }
        
        return result;
    }
    catch (const std::exception& e) {
        Log("Erro ao buscar NPCs que vendem item: " + std::string(e.what()));
        return result;
    }
}

std::vector<std::shared_ptr<ItemData>> WYDSpecificKnowledge::GetItemsSoldByNPC(const std::string& npcNameOrId) {
    std::vector<std::shared_ptr<ItemData>> result;
    
    try {
        if (!m_pImpl->m_initialized) {
            Log("Erro: Conhecimento específico do WYD não inicializado");
            return result;
        }
        
        // Obter informações do NPC
        auto npc = GetNPCInfo(npcNameOrId);
        if (!npc) {
            Log("NPC não encontrado: " + npcNameOrId);
            return result;
        }
        
        // Buscar todos os itens vendidos por este NPC
        for (int itemId : npc->itemsForSale) {
            auto it = m_pImpl->m_itemsById.find(itemId);
            if (it != m_pImpl->m_itemsById.end()) {
                result.push_back(it->second);
            }
        }
        
        return result;
    }
    catch (const std::exception& e) {
        Log("Erro ao buscar itens vendidos por NPC: " + std::string(e.what()));
        return result;
    }
}

std::vector<std::shared_ptr<NPCData>> WYDSpecificKnowledge::GetNPCsInMap(const std::string& mapNameOrId) {
    std::vector<std::shared_ptr<NPCData>> result;
    
    try {
        if (!m_pImpl->m_initialized) {
            Log("Erro: Conhecimento específico do WYD não inicializado");
            return result;
        }
        
        // Obter informações do mapa
        auto map = GetMapInfo(mapNameOrId);
        if (!map) {
            Log("Mapa não encontrado: " + mapNameOrId);
            return result;
        }
        
        // Buscar todos os NPCs neste mapa
        for (int npcId : map->npcs) {
            auto it = m_pImpl->m_npcsById.find(npcId);
            if (it != m_pImpl->m_npcsById.end()) {
                result.push_back(it->second);
            }
        }
        
        // Verificar também NPCs que têm localização neste mapa
        for (const auto& npcPair : m_pImpl->m_npcsById) {
            auto npc = npcPair.second;
            if (m_pImpl->NormalizeName(npc->location) == m_pImpl->NormalizeName(map->name) &&
                std::find(result.begin(), result.end(), npc) == result.end()) {
                result.push_back(npc);
            }
        }
        
        return result;
    }
    catch (const std::exception& e) {
        Log("Erro ao buscar NPCs em mapa: " + std::string(e.what()));
        return result;
    }
}

std::string WYDSpecificKnowledge::AnswerGameQuestion(const std::string& question) {
    try {
        if (!m_pImpl->m_initialized) {
            return "Desculpe, minha base de conhecimento do jogo WYD ainda não foi inicializada.";
        }
        
        Log("Processando pergunta: " + question);
        
        // Extrair informações da pergunta
        auto queryInfo = m_pImpl->ExtractQueryInfo(question);
        std::string entity = queryInfo.first;
        std::string context = queryInfo.second;
        
        Log("Entidade detectada: " + entity);
        Log("Contexto detectado: " + context);
        
        // Verificar se há entidade
        if (entity.empty()) {
            return "Desculpe, não consegui identificar sobre o que você está perguntando. Poderia reformular sua pergunta mencionando o nome do item ou NPC específico?";
        }
        
        // Verificar se é uma consulta sobre item e NPC específicos (formato "item|npc")
        auto pos = entity.find("|");
        if (pos != std::string::npos) {
            std::string itemName = entity.substr(0, pos);
            std::string npcName = entity.substr(pos + 1);
            
            // Obter informações do item e NPC
            auto item = GetItemInfo(itemName);
            auto npc = GetNPCInfo(npcName);
            
            if (!item) {
                return "Desculpe, não encontrei informações sobre o item '" + itemName + "'.";
            }
            
            if (!npc) {
                return "Desculpe, não encontrei informações sobre o NPC '" + npcName + "'.";
            }
            
            // Verificar preço
            int price = GetItemPriceAtNPC(itemName, npcName);
            if (price < 0) {
                return "O NPC " + npc->name + " não vende o item " + item->name + ".";
            }
            
            return "O item " + item->name + " custa " + std::to_string(price) + " gold no NPC " + npc->name + ".";
        }
        
        // Consulta sobre preço de item
        if (context == "price") {
            auto item = GetItemInfo(entity);
            if (item) {
                // Listar NPCs que vendem este item
                auto npcs = GetNPCsSellingItem(entity);
                if (npcs.empty()) {
                    return "O item " + item->name + " tem preço base de " + std::to_string(item->price) + " gold, mas não encontrei NPCs que o vendam.";
                }
                
                std::string response = "O item " + item->name + " (ID: " + std::to_string(item->id) + ") tem preço base de " + std::to_string(item->price) + " gold.\n\n";
                response += "NPCs que vendem este item:\n";
                
                for (const auto& npc : npcs) {
                    int npcPrice = npc->itemPrices.count(item->id) > 0 ? npc->itemPrices[item->id] : item->price;
                    response += "- " + npc->name + ": " + std::to_string(npcPrice) + " gold";
                    if (!npc->location.empty()) {
                        response += " (localização: " + npc->location + ")";
                    }
                    response += "\n";
                }
                
                return response;
            }
            
            // Verificar se é um NPC
            auto npc = GetNPCInfo(entity);
            if (npc) {
                auto items = GetItemsSoldByNPC(entity);
                if (items.empty()) {
                    return "O NPC " + npc->name + " não vende itens ou não tenho informações sobre os itens vendidos.";
                }
                
                std::string response = "Itens vendidos pelo NPC " + npc->name;
                if (!npc->location.empty()) {
                    response += " (" + npc->location + ")";
                }
                response += ":\n\n";
                
                for (const auto& item : items) {
                    int price = npc->itemPrices.count(item->id) > 0 ? npc->itemPrices[item->id] : item->price;
                    response += "- " + item->name + ": " + std::to_string(price) + " gold\n";
                }
                
                return response;
            }
            
            return "Desculpe, não encontrei informações sobre '" + entity + "'.";
        }
        
        // Consulta sobre localização
        if (context == "location") {
            // Verificar se é um NPC
            auto npc = GetNPCInfo(entity);
            if (npc) {
                if (npc->location.empty()) {
                    return "Desculpe, não tenho informações sobre a localização do NPC " + npc->name + ".";
                }
                
                return "O NPC " + npc->name + " está localizado em " + npc->location + ".";
            }
            
            // Verificar se é um item
            auto item = GetItemInfo(entity);
            if (item) {
                auto npcs = GetNPCsSellingItem(entity);
                if (npcs.empty()) {
                    return "Desculpe, não encontrei informações sobre onde conseguir o item " + item->name + ".";
                }
                
                std::string response = "O item " + item->name + " pode ser encontrado nos seguintes NPCs:\n\n";
                
                for (const auto& npc : npcs) {
                    response += "- " + npc->name;
                    if (!npc->location.empty()) {
                        response += " (localização: " + npc->location + ")";
                    }
                    response += "\n";
                }
                
                return response;
            }
            
            // Verificar se é um mapa
            auto map = GetMapInfo(entity);
            if (map) {
                auto npcs = GetNPCsInMap(entity);
                if (npcs.empty()) {
                    return "Não tenho informações sobre NPCs no mapa " + map->name + ".";
                }
                
                std::string response = "NPCs presentes no mapa " + map->name + ":\n\n";
                
                for (const auto& npc : npcs) {
                    response += "- " + npc->name;
                    
                    // Adicionar informações sobre itens vendidos
                    auto items = GetItemsSoldByNPC(npc->name);
                    if (!items.empty()) {
                        response += " (vende " + std::to_string(items.size()) + " itens)";
                    }
                    
                    response += "\n";
                }
                
                return response;
            }
            
            return "Desculpe, não encontrei informações sobre a localização de '" + entity + "'.";
        }
        
        // Consulta sobre vendas
        if (context == "sells") {
            auto npc = GetNPCInfo(entity);
            if (npc) {
                auto items = GetItemsSoldByNPC(entity);
                if (items.empty()) {
                    return "O NPC " + npc->name + " não vende itens ou não tenho informações sobre os itens vendidos.";
                }
                
                std::string response = "Itens vendidos pelo NPC " + npc->name;
                if (!npc->location.empty()) {
                    response += " (" + npc->location + ")";
                }
                response += ":\n\n";
                
                for (const auto& item : items) {
                    int price = npc->itemPrices.count(item->id) > 0 ? npc->itemPrices[item->id] : item->price;
                    response += "- " + item->name + ": " + std::to_string(price) + " gold\n";
                }
                
                return response;
            }
            
            return "Desculpe, não encontrei informações sobre o NPC '" + entity + "'.";
        }
        
        // Se chegou aqui, tentar buscar qualquer informação sobre a entidade
        auto item = GetItemInfo(entity);
        if (item) {
            std::string response = "Informações sobre o item " + item->name + " (ID: " + std::to_string(item->id) + "):\n\n";
            response += "Preço base: " + std::to_string(item->price) + " gold\n";
            
            if (!item->type.empty()) {
                response += "Tipo: " + item->type + "\n";
            }
            
            if (!item->description.empty()) {
                response += "Descrição: " + item->description + "\n";
            }
            
            if (!item->requirements.empty()) {
                response += "Requisitos: " + item->requirements + "\n";
            }
            
            if (!item->effects.empty()) {
                response += "Efeitos: " + item->effects + "\n";
            }
            
            // Listar NPCs que vendem
            auto npcs = GetNPCsSellingItem(entity);
            if (!npcs.empty()) {
                response += "\nNPCs que vendem este item:\n";
                for (const auto& npc : npcs) {
                    int npcPrice = npc->itemPrices.count(item->id) > 0 ? npc->itemPrices[item->id] : item->price;
                    response += "- " + npc->name + ": " + std::to_string(npcPrice) + " gold";
                    if (!npc->location.empty()) {
                        response += " (localização: " + npc->location + ")";
                    }
                    response += "\n";
                }
            }
            
            return response;
        }
        
        auto npc = GetNPCInfo(entity);
        if (npc) {
            std::string response = "Informações sobre o NPC " + npc->name + " (ID: " + std::to_string(npc->id) + "):\n\n";
            
            if (!npc->location.empty()) {
                response += "Localização: " + npc->location + "\n";
            }
            
            if (!npc->type.empty()) {
                response += "Tipo: " + npc->type + "\n";
            }
            
            // Listar itens vendidos
            auto items = GetItemsSoldByNPC(entity);
            if (!items.empty()) {
                response += "\nItens vendidos por este NPC:\n";
                for (const auto& item : items) {
                    int price = npc->itemPrices.count(item->id) > 0 ? npc->itemPrices[item->id] : item->price;
                    response += "- " + item->name + ": " + std::to_string(price) + " gold\n";
                }
            }
            
            return response;
        }
        
        auto map = GetMapInfo(entity);
        if (map) {
            std::string response = "Informações sobre o mapa " + map->name + " (ID: " + std::to_string(map->id) + "):\n\n";
            
            // Listar NPCs no mapa
            auto npcs = GetNPCsInMap(entity);
            if (!npcs.empty()) {
                response += "NPCs presentes neste mapa:\n";
                for (const auto& npc : npcs) {
                    response += "- " + npc->name + "\n";
                }
            }
            
            // Listar mobs no mapa
            if (!map->mobs.empty()) {
                response += "\nMonstros presentes neste mapa: " + std::to_string(map->mobs.size()) + "\n";
            }
            
            return response;
        }
        
        return "Desculpe, não encontrei informações específicas sobre '" + entity + "' na minha base de conhecimento do jogo WYD.";
    }
    catch (const std::exception& e) {
        Log("Erro ao responder pergunta: " + std::string(e.what()));
        return "Desculpe, ocorreu um erro ao processar sua pergunta: " + std::string(e.what());
    }
}

void WYDSpecificKnowledge::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_logCallback = logCallback;
    m_pImpl->m_logCallback = logCallback;
}

void WYDSpecificKnowledge::Log(const std::string& message) {
    if (m_logCallback) {
        m_logCallback(message);
    } else {
        std::cout << "[WYDSpecificKnowledge] " << message << std::endl;
    }
}

// Implementação dos métodos privados

bool WYDSpecificKnowledge::LoadItemsData(const std::string& filePath) {
    return m_pImpl->LoadItemsFromJson(filePath);
}

bool WYDSpecificKnowledge::LoadNPCsData(const std::string& filePath) {
    return m_pImpl->LoadNPCsFromJson(filePath);
}

bool WYDSpecificKnowledge::LoadMapsData(const std::string& filePath) {
    return m_pImpl->LoadMapsFromJson(filePath);
}

bool WYDSpecificKnowledge::LoadPricesData(const std::string& filePath) {
    return m_pImpl->LoadPricesFromJson(filePath);
}

} // namespace WYDStudio