/**
 * ResourceManager.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/core/ResourceManager.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "ResourceManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <filesystem>
#include <algorithm>

namespace wydbr {


/**
 * @file ResourceManager.cpp
 * @brief Implementação do sistema de gerenciamento de recursos do WYDBRASIL
 * 
 * Gerencia o carregamento, utilização e descarregamento de recursos do jogo,
 * como itens, mobs, mapas, scripts e outros dados estáticos.
 */

// Implementação do singleton
ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

// Construtor
ResourceManager::ResourceManager() 
    : m_resourcePath("./data"),
      m_enableCaching(true),
      m_totalResourcesCreated(0),
      m_resourcesLoaded(0),
      m_cacheHits(0),
      m_cacheMisses(0),
      m_initialized(false) {
}

// Destrutor
ResourceManager::~ResourceManager() {
    shutdown();
}

// Inicializa o gerenciador de recursos
bool ResourceManager::initialize(const std::string& resourcePath, bool enableCaching, 
                               bool preloadCommonResources) {
    if (m_initialized) {
        return true;
    }
    
    m_resourcePath = resourcePath;
    m_enableCaching = enableCaching;
    
    m_totalResourcesCreated = 0;
    m_resourcesLoaded = 0;
    m_cacheHits = 0;
    m_cacheMisses = 0;
    
    // Verificar se o diretório existe
    if (!std::filesystem::exists(resourcePath)) {
        std::cerr << "Diretório de recursos não encontrado: " << resourcePath << std::endl;
        return false;
    }
    
    // Pré-carregar recursos comuns se /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param preloadCommonResources Parâmetro preloadCommonResources
 * @return Retorna solicitado
 */
 solicitado
    if(preloadCommonResources){
        // Carregar lista de /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna itens
 */
 itens
        for(int i = 0; i < MAX_ITEMLIST; i++){
            loadItem(i);
        }
        
        // Carregar lista de mobs /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna comuns
 */
 comuns
        for(int i = 0; i < 100; i++){ // Primeiros 100 /**
 * loadMob
 * 
 * Implementa a funcionalidade loadMob conforme especificação original.
 * @param i Parâmetro i
 * @return Retorna mobs
 */
 mobs
            loadMob(i);
        }
        
        // Carregar todas as /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna skills
 */
 skills
        for(int i = 0; i < MAX_SPELL_LIST; i++){
            loadSkill(i);
        }
        
        // Carregar primeiro /**
 * loadMap
 * 
 * Implementa a funcionalidade loadMap conforme especificação original.
 * @param "default" Parâmetro "default"
 * @return Retorna mapa
 */
 mapa
        loadMap("default");
    }
    
    m_initialized = true;
    return true;
}

// Finaliza o gerenciador de recursos
void ResourceManager::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    // Salvar recursos /**
 * saveAllModifiedResources
 * 
 * Implementa a funcionalidade saveAllModifiedResources conforme especificação original.
 * @return Retorna modificados
 */
 modificados
    saveAllModifiedResources();
    
    // Limpar recursos
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    m_resources.clear();
    
    m_resourcesLoaded = 0;
    m_initialized = false;
}

// Carrega item por índice
std::shared_ptr<STRUCT_ITEM> ResourceManager::loadItem(int index) {
    if (!m_initialized || index < 0 || index >= MAX_ITEMLIST) {
        return nullptr;
    }
    
    // Converter índice para string
    std::ostringstream oss;
    oss << "item_" << index;
    std::string id = oss.str();
    
    // Verificar se o item já está carregado
    auto item = getResource<STRUCT_ITEM>(id);
    if (item) {
        return item;
    }
    
    // Determinar caminho do arquivo (baseado na organização do WYD original)
    std::string filePath = m_resourcePath + "/itens/";
    
    // Determinar em qual arquivo está o item
    int fileIndex = index / 1000; // Agrupar em arquivos de 1000 itens
    filePath += "itemlist_" + std::to_string(fileIndex) + ".bin";
    
    // Verificar se o arquivo existe
    if (!std::filesystem::exists(filePath)) {
        return nullptr;
    }
    
    // Abrir arquivo binário
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return nullptr;
    }
    
    // Calcular offset no arquivo
    int itemSize = sizeof(STRUCT_ITEM);
    int offset = (index % 1000) * itemSize;
    
    // Posicionar no item correto
    file.seekg(offset);
    
    // Criar estrutura para o item
    auto itemData = std::make_shared<STRUCT_ITEM>();
    std::memset(itemData.get(), 0, itemSize);
    
    // Ler item do arquivo
    file.read(reinterpret_cast<char*>(itemData.get()), itemSize);
    
    // Verificar se a leitura foi bem-/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !file Parâmetro !file
 * @return Retorna sucedida
 */
sucedida
    if(!file){
        return nullptr;
    }
    
    // Criar metadados para o recurso
    auto metadata = getOrCreateMetadata<STRUCT_ITEM>(id);
    metadata->resource = itemData;
    metadata->loaded = true;
    metadata->referenceCount = 1;
    metadata->lastAccess = std::chrono::steady_clock::now();
    metadata->modified = false;
    metadata->filePath = filePath;
    
    m_resourcesLoaded++;
    
    return itemData;
}

// Carrega mob por índice
std::shared_ptr<STRUCT_MOB> ResourceManager::loadMob(int index) {
    if (!m_initialized || index < 0 || index >= MAX_MOB) {
        return nullptr;
    }
    
    // Converter índice para string
    std::ostringstream oss;
    oss << "mob_" << index;
    std::string id = oss.str();
    
    // Verificar se o mob já está carregado
    auto mob = getResource<STRUCT_MOB>(id);
    if (mob) {
        return mob;
    }
    
    // Determinar caminho do arquivo (baseado na organização do WYD original)
    std::string filePath = m_resourcePath + "/mobs/";
    
    // Determinar em qual arquivo está o mob
    int fileIndex = index / 500; // Agrupar em arquivos de 500 mobs
    filePath += "moblist_" + std::to_string(fileIndex) + ".bin";
    
    // Verificar se o arquivo existe
    if (!std::filesystem::exists(filePath)) {
        return nullptr;
    }
    
    // Abrir arquivo binário
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return nullptr;
    }
    
    // Calcular offset no arquivo
    int mobSize = sizeof(STRUCT_MOB);
    int offset = (index % 500) * mobSize;
    
    // Posicionar no mob correto
    file.seekg(offset);
    
    // Criar estrutura para o mob
    auto mobData = std::make_shared<STRUCT_MOB>();
    std::memset(mobData.get(), 0, mobSize);
    
    // Ler mob do arquivo
    file.read(reinterpret_cast<char*>(mobData.get()), mobSize);
    
    // Verificar se a leitura foi bem-/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !file Parâmetro !file
 * @return Retorna sucedida
 */
sucedida
    if(!file){
        return nullptr;
    }
    
    // Criar metadados para o recurso
    auto metadata = getOrCreateMetadata<STRUCT_MOB>(id);
    metadata->resource = mobData;
    metadata->loaded = true;
    metadata->referenceCount = 1;
    metadata->lastAccess = std::chrono::steady_clock::now();
    metadata->modified = false;
    metadata->filePath = filePath;
    
    m_resourcesLoaded++;
    
    return mobData;
}

// Carrega skill por índice
std::shared_ptr<STRUCT_SKILL> ResourceManager::loadSkill(int index) {
    if (!m_initialized || index < 0 || index >= MAX_SPELL_LIST) {
        return nullptr;
    }
    
    // Converter índice para string
    std::ostringstream oss;
    oss << "skill_" << index;
    std::string id = oss.str();
    
    // Verificar se a skill já está carregada
    auto skill = getResource<STRUCT_SKILL>(id);
    if (skill) {
        return skill;
    }
    
    // Determinar caminho do arquivo (baseado na organização do WYD original)
    std::string filePath = m_resourcePath + "/skills/skilldata.bin";
    
    // Verificar se o arquivo existe
    if (!std::filesystem::exists(filePath)) {
        return nullptr;
    }
    
    // Abrir arquivo binário
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return nullptr;
    }
    
    // Calcular offset no arquivo
    int skillSize = sizeof(STRUCT_SKILL);
    int offset = index * skillSize;
    
    // Posicionar na skill correta
    file.seekg(offset);
    
    // Criar estrutura para a skill
    auto skillData = std::make_shared<STRUCT_SKILL>();
    std::memset(skillData.get(), 0, skillSize);
    
    // Ler skill do arquivo
    file.read(reinterpret_cast<char*>(skillData.get()), skillSize);
    
    // Verificar se a leitura foi bem-/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !file Parâmetro !file
 * @return Retorna sucedida
 */
sucedida
    if(!file){
        return nullptr;
    }
    
    // Criar metadados para o recurso
    auto metadata = getOrCreateMetadata<STRUCT_SKILL>(id);
    metadata->resource = skillData;
    metadata->loaded = true;
    metadata->referenceCount = 1;
    metadata->lastAccess = std::chrono::steady_clock::now();
    metadata->modified = false;
    metadata->filePath = filePath;
    
    m_resourcesLoaded++;
    
    return skillData;
}

// Carrega mapa por nome
std::shared_ptr<void> ResourceManager::loadMap(const std::string& mapName) {
    if (!m_initialized || mapName.empty()) {
        return nullptr;
    }
    
    // Verificar se o mapa já está carregado
    auto map = getResource<void>(mapName);
    if (map) {
        return map;
    }
    
    // Determinar caminho do arquivo (baseado na organização do WYD original)
    std::string filePath = m_resourcePath + "/maps/" + mapName + ".wys";
    
    // Verificar se o arquivo existe
    if (!std::filesystem::exists(filePath)) {
        return nullptr;
    }
    
    // Abrir arquivo binário
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param std::ios::ate Parâmetro std::ios::ate
 * @return Retorna std::ifstream
 */

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) {
        return nullptr;
    }
    
    // Obter tamanho do arquivo
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Criar buffer para o mapa
    auto mapData = std::make_shared<char[]>(size);
    
    // Ler mapa do arquivo
    if (!file.read(mapData.get(), size)) {
        return nullptr;
    }
    
    // Criar metadados para o recurso
    auto metadata = getOrCreateMetadata<void>(mapName);
    metadata->resource = std::shared_ptr<void>(mapData, static_cast<void*>(mapData.get()));
    metadata->loaded = true;
    metadata->referenceCount = 1;
    metadata->lastAccess = std::chrono::steady_clock::now();
    metadata->modified = false;
    metadata->filePath = filePath;
    
    m_resourcesLoaded++;
    
    return std::shared_ptr<void>(mapData, static_cast<void*>(mapData.get()));
}

// Libera recursos não utilizados
int ResourceManager::releaseUnusedResources(int minimumIdleTime) {
    if (!m_initialized) {
        return 0;
    }
    
    int releasedCount = 0;
    auto now = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    
    for (auto& typePair : m_resources) {
        for (auto it = typePair.second.begin(); it != typePair.second.end();) {
            auto& metadata = it->second;
            
            if (metadata->loaded && metadata->referenceCount <= 0) {
                auto idleTime = std::chrono::duration_cast<std::chrono::seconds>(
                    now - metadata->lastAccess).count();
                
                if (idleTime >= minimumIdleTime) {
                    // Verificar se o recurso foi modificado e precisa ser /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param metadata->modified Parâmetro metadata->modified
 * @return Retorna salvo
 */
 salvo
                    if(metadata->modified){
                        // Não podemos chamar saveResource aqui por causa do lock,
                        // então sinalizamos para salvar depois
                        // TODO: Implementar fila de salvamento para recursos modificados
                    }
                    
                    // Descarregar recurso
                    metadata->resource = std::any();
                    metadata->loaded = false;
                    
                    releasedCount++;
                    m_resourcesLoaded--;
                }
            }
            
            ++it;
        }
    }
    
    return releasedCount;
}

// Salva todos os recursos modificados
int ResourceManager::saveAllModifiedResources() {
    if (!m_initialized) {
        return 0;
    }
    
    int savedCount = 0;
    std::vector<std::pair<std::string, std::string>> resourcesToSave;
    
    // Coletar /**
 * modificados
 * 
 * Implementa a funcionalidade modificados conforme especificação original.
 * @param lock Parâmetro lock
 * @return Retorna recursos
 */
 recursos modificados(evitando deadlock ao chamar saveResource durante o lock){
        std::lock_guard<std::mutex> lock(m_resourcesMutex);
        
        for (const auto& typePair : m_resources) {
            for (const auto& resourcePair : typePair.second) {
                if (resourcePair.second->loaded && resourcePair.second->modified) {
                    resourcesToSave.emplace_back(typePair.first, resourcePair.first);
                }
            }
        }
    }
    
    // Salvar recursos /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param resourcesToSave Parâmetro resourcesToSave
 * @return Retorna coletados
 */
 coletados
    for(const auto& pair : resourcesToSave){
        const std::string& typeName = pair.first;
        const std::string& id = pair.second;
        
        // Verificar tipo e chamar função específica
        if (typeName == typeid(STRUCT_ITEM).name()) {
            if (saveResource<STRUCT_ITEM>(id)) {
                savedCount++;
            }
        }
        else if (typeName == typeid(STRUCT_MOB).name()) {
            if (saveResource<STRUCT_MOB>(id)) {
                savedCount++;
            }
        }
        else if (typeName == typeid(STRUCT_SKILL).name()) {
            if (saveResource<STRUCT_SKILL>(id)) {
                savedCount++;
            }
        }
        // Adicionar outros tipos conforme necessário
    }
    
    return savedCount;
}

// Obtém estatísticas do gerenciador de recursos
void ResourceManager::getStats(int& totalResources, int& loadedResources, size_t& memoryUsage, float& cacheHitRate) {
    if (!m_initialized) {
        totalResources = 0;
        loadedResources = 0;
        memoryUsage = 0;
        cacheHitRate = 0.0f;
        return;
    }
    
    // Calcular estatísticas
    totalResources = m_totalResourcesCreated;
    loadedResources = m_resourcesLoaded;
    
    // Calcular uso aproximado de memória (estimativa simplificada)
    memoryUsage = 0;
    
    {
        std::lock_guard<std::mutex> lock(m_resourcesMutex);
        
        for (const auto& typePair : m_resources) {
            for (const auto& resourcePair : typePair.second) {
                if (resourcePair.second->loaded) {
                    // Estimativa baseada no tipo
                    if (typePair.first == typeid(STRUCT_ITEM).name()) {
                        memoryUsage += sizeof(STRUCT_ITEM);
                    }
                    else if (typePair.first == typeid(STRUCT_MOB).name()) {
                        memoryUsage += sizeof(STRUCT_MOB);
                    }
                    else if (typePair.first == typeid(STRUCT_SKILL).name()) {
                        memoryUsage += sizeof(STRUCT_SKILL);
                    }
                    // Outros tipos...
                }
            }
        }
    }
    
    // Calcular taxa de acertos do cache
    uint64_t totalRequests = m_cacheHits + m_cacheMisses;
    if (totalRequests > 0) {
        cacheHitRate = static_cast<float>(m_cacheHits) / totalRequests * 100.0f;
    }
    else {
        cacheHitRate = 0.0f;
    }
}

// Função auxiliar - determinar caminho do arquivo
/**
 * determineFilePath
 * 
 * Implementa a funcionalidade determineFilePath conforme especificação original.
 * @param typeName Parâmetro typeName
 * @param id Parâmetro id
 * @return Retorna std::string
 */

std::string determineFilePath(const std::string& typeName, const std::string& id){
    // Implementação básica - ajustar conforme necessário
    if (typeName == typeid(STRUCT_ITEM).name()) {
        // Extrair índice do ID (por exemplo, de "item_123" extrai 123)
        size_t pos = id.find_last_of('_');
        if (pos != std::string::npos) {
            std::string indexStr = id.substr(pos + 1);
            int index = std::stoi(indexStr);
            int fileIndex = index / 1000;
            return "itens/itemlist_" + std::to_string(fileIndex) + ".bin";
        }
    }
    else if (typeName == typeid(STRUCT_MOB).name()) {
        size_t pos = id.find_last_of('_');
        if (pos != std::string::npos) {
            std::string indexStr = id.substr(pos + 1);
            int index = std::stoi(indexStr);
            int fileIndex = index / 500;
            return "mobs/moblist_" + std::to_string(fileIndex) + ".bin";
        }
    }
    else if (typeName == typeid(STRUCT_SKILL).name()) {
        return "skills/skilldata.bin";
    }
    else if (typeName == typeid(void).name()) {
        // Assumir que ID é o nome do mapa
        return "maps/" + id + ".wys";
    }
    
    // Padrão genérico
    return typeName + "/" + id;
}

// Template especializado para loadResource<STRUCT_ITEM>
template<>
std::shared_ptr<STRUCT_ITEM> ResourceManager::defaultResourceLoader(const std::string& id) {
    // Extrair índice do ID (por exemplo, de "item_123" extrai 123)
    size_t pos = id.find_last_of('_');
    if (pos != std::string::npos) {
        std::string indexStr = id.substr(pos + 1);
        try {
            int index = std::stoi(indexStr);
            /**
 * loadItem
 * 
 * Implementa a funcionalidade loadItem conforme especificação original.
 * @param index Parâmetro index
 * @return Retorna return
 */

            return loadItem(index);
        }
        catch (const std::exception&) {
            // Erro ao converter para inteiro
            return nullptr;
        }
    }
    
    return nullptr;
}

// Template especializado para loadResource<STRUCT_MOB>
template<>
std::shared_ptr<STRUCT_MOB> ResourceManager::defaultResourceLoader(const std::string& id) {
    // Extrair índice do ID (por exemplo, de "mob_123" extrai 123)
    size_t pos = id.find_last_of('_');
    if (pos != std::string::npos) {
        std::string indexStr = id.substr(pos + 1);
        try {
            int index = std::stoi(indexStr);
            /**
 * loadMob
 * 
 * Implementa a funcionalidade loadMob conforme especificação original.
 * @param index Parâmetro index
 * @return Retorna return
 */

            return loadMob(index);
        }
        catch (const std::exception&) {
            // Erro ao converter para inteiro
            return nullptr;
        }
    }
    
    return nullptr;
}

// Template especializado para loadResource<STRUCT_SKILL>
template<>
std::shared_ptr<STRUCT_SKILL> ResourceManager::defaultResourceLoader(const std::string& id) {
    // Extrair índice do ID (por exemplo, de "skill_123" extrai 123)
    size_t pos = id.find_last_of('_');
    if (pos != std::string::npos) {
        std::string indexStr = id.substr(pos + 1);
        try {
            int index = std::stoi(indexStr);
            /**
 * loadSkill
 * 
 * Implementa a funcionalidade loadSkill conforme especificação original.
 * @param index Parâmetro index
 * @return Retorna return
 */

            return loadSkill(index);
        }
        catch (const std::exception&) {
            // Erro ao converter para inteiro
            return nullptr;
        }
    }
    
    return nullptr;
}

// Template especializado para loadResource<void>
template<>
std::shared_ptr<void> ResourceManager::defaultResourceLoader(const std::string& id) {
    // Assumir que ID é o nome do mapa
    /**
 * loadMap
 * 
 * Implementa a funcionalidade loadMap conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna return
 */

    return loadMap(id);
}

// Template especializado para saveResource<STRUCT_ITEM>
template<>
bool ResourceManager::defaultResourceSaver(const std::string& id, const std::shared_ptr<STRUCT_ITEM>& resource) {
    if (!resource) {
        return false;
    }
    
    // Obter metadados do recurso
    auto metadata = getOrCreateMetadata<STRUCT_ITEM>(id);
    if (!metadata->loaded || metadata->filePath.empty()) {
        return false;
    }
    
    // Extrair índice do ID (por exemplo, de "item_123" extrai 123)
    size_t pos = id.find_last_of('_');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string indexStr = id.substr(pos + 1);
    int index;
    
    try {
        index = std::stoi(indexStr);
    }
    catch (const std::exception&) {
        return false;
    }
    
    // Abrir arquivo para escrita
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param metadata->filePath Parâmetro metadata->filePath
 * @param std::ios::out Parâmetro std::ios::out
 * @return Retorna std::fstream
 */

    std::fstream file(metadata->filePath, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        return false;
    }
    
    // Calcular offset no arquivo
    int itemSize = sizeof(STRUCT_ITEM);
    int fileIndex = index / 1000;
    int offset = (index % 1000) * itemSize;
    
    // Posicionar no item correto
    file.seekp(offset);
    
    // Escrever item no arquivo
    file.write(reinterpret_cast<const char*>(resource.get()), itemSize);
    
    // Verificar se a escrita foi bem-/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !file Parâmetro !file
 * @return Retorna sucedida
 */
sucedida
    if(!file){
        return false;
    }
    
    return true;
}

// Template especializado para saveResource<STRUCT_MOB>
template<>
bool ResourceManager::defaultResourceSaver(const std::string& id, const std::shared_ptr<STRUCT_MOB>& resource) {
    if (!resource) {
        return false;
    }
    
    // Obter metadados do recurso
    auto metadata = getOrCreateMetadata<STRUCT_MOB>(id);
    if (!metadata->loaded || metadata->filePath.empty()) {
        return false;
    }
    
    // Extrair índice do ID (por exemplo, de "mob_123" extrai 123)
    size_t pos = id.find_last_of('_');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string indexStr = id.substr(pos + 1);
    int index;
    
    try {
        index = std::stoi(indexStr);
    }
    catch (const std::exception&) {
        return false;
    }
    
    // Abrir arquivo para escrita
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param metadata->filePath Parâmetro metadata->filePath
 * @param std::ios::out Parâmetro std::ios::out
 * @return Retorna std::fstream
 */

    std::fstream file(metadata->filePath, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        return false;
    }
    
    // Calcular offset no arquivo
    int mobSize = sizeof(STRUCT_MOB);
    int fileIndex = index / 500;
    int offset = (index % 500) * mobSize;
    
    // Posicionar no mob correto
    file.seekp(offset);
    
    // Escrever mob no arquivo
    file.write(reinterpret_cast<const char*>(resource.get()), mobSize);
    
    // Verificar se a escrita foi bem-/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !file Parâmetro !file
 * @return Retorna sucedida
 */
sucedida
    if(!file){
        return false;
    }
    
    return true;
}

// Template especializado para saveResource<STRUCT_SKILL>
template<>
bool ResourceManager::defaultResourceSaver(const std::string& id, const std::shared_ptr<STRUCT_SKILL>& resource) {

} // namespace wydbr

    if (!resource) {
        return false;
    }
    
    // Obter metadados do recurso
    auto metadata = getOrCreateMetadata<STRUCT_SKILL>(id);
    if (!metadata->loaded || metadata->filePath.empty()) {
        return false;
    }
    
    // Extrair índice do ID (por exemplo, de "skill_123" extrai 123)
    size_t pos = id.find_last_of('_');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string indexStr = id.substr(pos + 1);
    int index;
    
    try {
        index = std::stoi(indexStr);
    }
    catch (const std::exception&) {
        return false;
    }
    
    // Abrir arquivo para escrita
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param metadata->filePath Parâmetro metadata->filePath
 * @param std::ios::out Parâmetro std::ios::out
 * @return Retorna std::fstream
 */

    std::fstream file(metadata->filePath, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        return false;
    }
    
    // Calcular offset no arquivo
    int skillSize = sizeof(STRUCT_SKILL);
    int offset = index * skillSize;
    
    // Posicionar na skill correta
    file.seekp(offset);
    
    // Escrever skill no arquivo
    file.write(reinterpret_cast<const char*>(resource.get()), skillSize);
    
    // Verificar se a escrita foi bem-/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !file Parâmetro !file
 * @return Retorna sucedida
 */
sucedida
    if(!file){
        return false;
    }
    
    return true;
}