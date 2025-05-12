/**
 * ResourceManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/core/ResourceManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

/**
 * @file ResourceManager.h
 * @brief Sistema de gerenciamento de recursos para o WYDBRASIL
 * 
 * Esta classe implementa um sistema de gerenciamento de recursos do jogo,
 * permitindo o carregamento, controle de referências e acesso eficiente a
 * recursos como itens, mobs, NPCs, mapas e outros dados do jogo.
 */

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <functional>
#include <optional>
#include <atomic>
#include <future>
#include <type_traits>
#include <stdexcept>
#include <any>

#include "../../include/Basedef.h"

namespace wydbr {


/**
 * @class ResourceManager
 * @brief Sistema para gerenciamento de recursos do jogo
 * 
 * Gerencia o carregamento, utilização e descarregamento de recursos
 * do jogo, como itens, mobs, mapas, scripts e outros dados estáticos.
 */
/**
 * Classe WYDResourceManager
 * 
 * Esta classe implementa a funcionalidade WYDResourceManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ResourceManager {
public:
    /**
     * @brief Obtém a instância única do ResourceManager (Singleton)
     * @return Referência para a instância única
     */
    static ResourceManager& getInstance();
    
    /**
     * @brief Inicializa o gerenciador de recursos
     * @param resourcePath Caminho para os arquivos de recursos
     * @param enableCaching Habilitar cache de recursos
     * @param preloadCommonResources Pré-carregar recursos comuns
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::string& resourcePath, bool enableCaching = true, 
                   bool preloadCommonResources = true);
    
    /**
     * @brief Finaliza o gerenciador de recursos
     */
    void shutdown();
    
    /**
     * @brief Carrega um recurso específico
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @param async Carregar de forma assíncrona
     * @return Referência ao recurso ou nullptr se não encontrado
     */
    template<typename T>
    std::shared_ptr<T> loadResource(const std::string& id, bool async = false);
    
    /**
     * @brief Obtém um recurso já carregado
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @return Referência ao recurso ou nullptr se não carregado
     */
    template<typename T>
    std::shared_ptr<T> getResource(const std::string& id);
    
    /**
     * @brief Libera um recurso específico
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @return true se liberado com sucesso
     */
    template<typename T>
    /**
 * releaseResource
 * 
 * Implementa a funcionalidade releaseResource conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna bool
 */

    bool releaseResource(const std::string& id);
    
    /**
     * @brief Verifica se um recurso está carregado
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @return true se o recurso está carregado
     */
    template<typename T>
    /**
 * isResourceLoaded
 * 
 * Implementa a funcionalidade isResourceLoaded conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna bool
 */

    bool isResourceLoaded(const std::string& id);
    
    /**
     * @brief Recarrega um recurso específico
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @param async Recarregar de forma assíncrona
     * @return true se recarregado com sucesso
     */
    template<typename T>
    /**
 * reloadResource
 * 
 * Implementa a funcionalidade reloadResource conforme especificação original.
 * @param id Parâmetro id
 * @param false Parâmetro false
 * @return Retorna bool
 */

    bool reloadResource(const std::string& id, bool async = false);
    
    /**
     * @brief Carrega todos os recursos de um tipo
     * @tparam T Tipo dos recursos
     * @param async Carregar de forma assíncrona
     * @return Número de recursos carregados com sucesso
     */
    template<typename T>
    /**
 * loadAllResources
 * 
 * Implementa a funcionalidade loadAllResources conforme especificação original.
 * @param false Parâmetro false
 * @return Retorna int
 */

    int loadAllResources(bool async = false);
    
    /**
     * @brief Obtém todos os recursos de um tipo
     * @tparam T Tipo dos recursos
     * @return Vetor com todos os recursos carregados
     */
    template<typename T>
    std::vector<std::shared_ptr<T>> getAllResources();
    
    /**
     * @brief Libera todos os recursos de um tipo
     * @tparam T Tipo dos recursos
     * @return Número de recursos liberados
     */
    template<typename T>
    /**
 * releaseAllResources
 * 
 * Implementa a funcionalidade releaseAllResources conforme especificação original.
 * @return Retorna int
 */

    int releaseAllResources();
    
    /**
     * @brief Libera recursos não utilizados
     * @param minimumIdleTime Tempo mínimo ocioso para liberar (segundos)
     * @return Número de recursos liberados
     */
    int releaseUnusedResources(int minimumIdleTime = 300);
    
    /**
     * @brief Define função customizada para carregamento de recursos
     * @tparam T Tipo do recurso
     * @param loader Função de carregamento
     */
    template<typename T>
    void setResourceLoader(std::function<std::shared_ptr<T>(const std::string&)> loader);
    
    /**
     * @brief Define função customizada para salvamento de recursos
     * @tparam T Tipo do recurso
     * @param saver Função de salvamento
     */
    template<typename T>
    void setResourceSaver(std::function<bool(const std::string&, const std::shared_ptr<T>&)> saver);
    
    /**
     * @brief Salva um recurso modificado
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @param markAsSaved Marca o recurso como salvo (não modificado)
     * @return true se salvo com sucesso
     */
    template<typename T>
    /**
 * saveResource
 * 
 * Implementa a funcionalidade saveResource conforme especificação original.
 * @param id Parâmetro id
 * @param true Parâmetro true
 * @return Retorna bool
 */

    bool saveResource(const std::string& id, bool markAsSaved = true);
    
    /**
     * @brief Salva todos os recursos modificados
     * @return Número de recursos salvos
     */
    int saveAllModifiedResources();
    
    /**
     * @brief Marca um recurso como modificado
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @return true se marcado com sucesso
     */
    template<typename T>
    /**
 * markResourceAsModified
 * 
 * Implementa a funcionalidade markResourceAsModified conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna bool
 */

    bool markResourceAsModified(const std::string& id);
    
    /**
     * @brief Verifica se um recurso foi modificado
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @return true se o recurso foi modificado
     */
    template<typename T>
    /**
 * isResourceModified
 * 
 * Implementa a funcionalidade isResourceModified conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna bool
 */

    bool isResourceModified(const std::string& id);
    
    /**
     * @brief Obtém o caminho para um recurso
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @return Caminho para o recurso ou string vazia se não encontrado
     */
    template<typename T>
    /**
 * getResourcePath
 * 
 * Implementa a funcionalidade getResourcePath conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna std::string
 */

    std::string getResourcePath(const std::string& id);
    
    /**
     * @brief Verifica existência de um recurso
     * @tparam T Tipo do recurso
     * @param id ID ou nome do recurso
     * @return true se o recurso existe
     */
    template<typename T>
    /**
 * resourceExists
 * 
 * Implementa a funcionalidade resourceExists conforme especificação original.
 * @param id Parâmetro id
 * @return Retorna bool
 */

    bool resourceExists(const std::string& id);
    
    /**
     * @brief Carrega item por índice
     * @param index Índice do item
     * @return Referência ao item ou nullptr se não encontrado
     */
    std::shared_ptr<STRUCT_ITEM> loadItem(int index);
    
    /**
     * @brief Carrega mob por índice
     * @param index Índice do mob
     * @return Referência ao mob ou nullptr se não encontrado
     */
    std::shared_ptr<STRUCT_MOB> loadMob(int index);
    
    /**
     * @brief Carrega skill por índice
     * @param index Índice da skill
     * @return Referência à skill ou nullptr se não encontrada
     */
    std::shared_ptr<STRUCT_SKILL> loadSkill(int index);
    
    /**
     * @brief Carrega mapa por nome
     * @param mapName Nome do mapa
     * @return Referência ao mapa ou nullptr se não encontrado
     */
    std::shared_ptr<void> loadMap(const std::string& mapName);
    
    /**
     * @brief Obtém estatísticas do gerenciador de recursos
     * @param totalResources Total de recursos gerenciados (saída)
     * @param loadedResources Recursos atualmente carregados (saída)
     * @param memoryUsage Memória utilizada (aproximada, em bytes) (saída)
     * @param cacheHitRate Taxa de acertos do cache (%) (saída)
     */
    void getStats(int& totalResources, int& loadedResources, size_t& memoryUsage, float& cacheHitRate);
    
private:
    // Construtor privado (singleton)
    ResourceManager();
    
    // Destrutor
    ~ResourceManager();
    
    // Impedir cópia e atribuição
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    // Classe base para metadados de recursos
    /**
 * Classe WYDResourceMetadata
 * 
 * Esta classe implementa a funcionalidade WYDResourceMetadata conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ResourceMetadata {
    public:
        virtual ~ResourceMetadata() = default;
        
        std::string id;                  // ID do recurso
        std::string filePath;            // Caminho do arquivo
        std::any resource;               // Recurso armazenado como any
        std::chrono::steady_clock::time_point lastAccess; // Último acesso
        std::atomic<int> referenceCount; // Contador de referências
        bool modified;                   // Modificado desde último salvamento
        bool loaded;                     // Recurso está carregado
        
        ResourceMetadata() : referenceCount(0), modified(false), loaded(false) {}
    };
    
    // Tipo para mapa de metadados por tipo
    using TypeMap = std::unordered_map<std::string, std::shared_ptr<ResourceMetadata>>;
    
    // Determina o nome do tipo em runtime
    template<typename T>
    /**
 * getTypeName
 * 
 * Implementa a funcionalidade getTypeName conforme especificação original.
 * @return Retorna std::string
 */

    std::string getTypeName();
    
    // Verifica se um tipo é suportado
    template<typename T>
    /**
 * isSupportedType
 * 
 * Implementa a funcionalidade isSupportedType conforme especificação original.
 * @return Retorna bool
 */

    bool isSupportedType();
    
    // Loader genérico para recursos (implementação padrão)
    template<typename T>
    std::shared_ptr<T> defaultResourceLoader(const std::string& id);
    
    // Saver genérico para recursos (implementação padrão)
    template<typename T>
    /**
 * defaultResourceSaver
 * 
 * Implementa a funcionalidade defaultResourceSaver conforme especificação original.
 * @param id Parâmetro id
 * @param resource Parâmetro resource
 * @return Retorna bool
 */

    bool defaultResourceSaver(const std::string& id, const std::shared_ptr<T>& resource);
    
    // Obtém ou cria metadados para um recurso
    template<typename T>
    std::shared_ptr<ResourceMetadata> getOrCreateMetadata(const std::string& id);
    
    // Caminho base para recursos
    std::string m_resourcePath;
    
    // Flag de cache habilitado
    bool m_enableCaching;
    
    // Mapa de recursos por tipo
    std::unordered_map<std::string, TypeMap> m_resources;
    
    // Mutex para acesso ao mapa de recursos
    std::mutex m_resourcesMutex;
    
    // Mapa de funções de carregamento customizadas
    std::unordered_map<std::string, std::any> m_customLoaders;
    
    // Mapa de funções de salvamento customizadas
    std::unordered_map<std::string, std::any> m_customSavers;
    
    // Mutex para acesso aos mapas de funções
    std::mutex m_functionsMutex;
    
    // Estatísticas
    std::atomic<int> m_totalResourcesCreated;
    std::atomic<int> m_resourcesLoaded;
    std::atomic<uint64_t> m_cacheHits;
    std::atomic<uint64_t> m_cacheMisses;
    
    // Inicializado
    bool m_initialized;
};

// Implementação dos templates

template<typename T>
std::shared_ptr<T> ResourceManager::loadResource(const std::string& id, bool async) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return nullptr;
    }
    
    // Se for carregamento assíncrono, iniciar em thread /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param async Parâmetro async
 * @return Retorna separada
 */
 separada
    if(async){
        std::thread([this, id]() {
            loadResource<T>(id, false);
        }).detach();
        
        return nullptr;
    }
    
    // Verificar se o recurso já está carregado
    auto existingResource = getResource<T>(id);
    if (existingResource) {
        m_cacheHits++;
        return existingResource;
    }
    
    m_cacheMisses++;
    
    // Obter ou criar metadados
    auto metadata = getOrCreateMetadata<T>(id);
    
    // Se não estiver carregado, carregar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !metadata->loaded Parâmetro !metadata->loaded
 * @return Retorna agora
 */
 agora
    if(!metadata->loaded){
        // Determinar a função de carregamento
        std::function<std::shared_ptr<T>(const std::string&)> loader;
        
        {
            std::lock_guard<std::mutex> lock(m_functionsMutex);
            
            std::string typeName = getTypeName<T>();
            auto loaderIt = m_customLoaders.find(typeName);
            
            if (loaderIt != m_customLoaders.end()) {
                try {
                    loader = std::any_cast<std::function<std::shared_ptr<T>(const std::string&)>>(loaderIt->second);
                }
                catch (const std::bad_any_cast&) {
                    // Tipo incompatível, usar loader padrão
                    loader = std::bind(&ResourceManager::defaultResourceLoader<T>, this, std::placeholders::_1);
                }
            }
            else {
                // Não há loader customizado, usar padrão
                loader = std::bind(&ResourceManager::defaultResourceLoader<T>, this, std::placeholders::_1);
            }
        }
        
        // Carregar o recurso
        auto resource = loader(id);
        
        if (resource) {
            std::lock_guard<std::mutex> lock(m_resourcesMutex);
            
            metadata->resource = resource;
            metadata->loaded = true;
            metadata->referenceCount = 1;
            metadata->lastAccess = std::chrono::steady_clock::now();
            metadata->modified = false;
            
            m_resourcesLoaded++;
            
            return resource;
        }
        
        return nullptr;
    }
    
    // Recurso já carregado, extrair do any
    try {
        auto resource = std::any_cast<std::shared_ptr<T>>(metadata->resource);
        
        // Atualizar metadados
        metadata->referenceCount++;
        metadata->lastAccess = std::chrono::steady_clock::now();
        
        return resource;
    }
    catch (const std::bad_any_cast&) {
        // Tipo incompatível
        return nullptr;
    }
}

template<typename T>
std::shared_ptr<T> ResourceManager::getResource(const std::string& id) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return nullptr;
    }
    
    std::string typeName = getTypeName<T>();
    
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    
    auto typeIt = m_resources.find(typeName);
    if (typeIt == m_resources.end()) {
        return nullptr;
    }
    
    auto& typeMap = typeIt->second;
    auto resourceIt = typeMap.find(id);
    
    if (resourceIt == typeMap.end() || !resourceIt->second->loaded) {
        return nullptr;
    }
    
    try {
        auto resource = std::any_cast<std::shared_ptr<T>>(resourceIt->second->resource);
        
        // Atualizar metadados
        resourceIt->second->referenceCount++;
        resourceIt->second->lastAccess = std::chrono::steady_clock::now();
        
        return resource;
    }
    catch (const std::bad_any_cast&) {
        // Tipo incompatível
        return nullptr;
    }
}

template<typename T>
bool ResourceManager::releaseResource(const std::string& id) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return false;
    }
    
    std::string typeName = getTypeName<T>();
    
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    
    auto typeIt = m_resources.find(typeName);
    if (typeIt == m_resources.end()) {
        return false;
    }
    
    auto& typeMap = typeIt->second;
    auto resourceIt = typeMap.find(id);
    
    if (resourceIt == typeMap.end() || !resourceIt->second->loaded) {
        return false;
    }
    
    // Decrementar referência
    resourceIt->second->referenceCount--;
    
    // Se não há mais referências e cache está desabilitado, /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !m_enableCaching Parâmetro !m_enableCaching
 * @return Retorna descarregar
 */
 descarregar
    if(resourceIt->second->referenceCount <= 0 && !m_enableCaching){
        // Verificar se o recurso foi modificado e precisa ser /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param resourceIt->second->modified Parâmetro resourceIt->second->modified
 * @return Retorna salvo
 */
 salvo
        if(resourceIt->second->modified){
            // Salvar o recurso
            saveResource<T>(id, true);
        }
        
        // Descarregar recurso
        resourceIt->second->resource = std::any();
        resourceIt->second->loaded = false;
        
        m_resourcesLoaded--;
    }
    
    return true;
}

template<typename T>
bool ResourceManager::isResourceLoaded(const std::string& id) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return false;
    }
    
    std::string typeName = getTypeName<T>();
    
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    
    auto typeIt = m_resources.find(typeName);
    if (typeIt == m_resources.end()) {
        return false;
    }
    
    auto& typeMap = typeIt->second;
    auto resourceIt = typeMap.find(id);
    
    return (resourceIt != typeMap.end() && resourceIt->second->loaded);
}

template<typename T>
bool ResourceManager::reloadResource(const std::string& id, bool async) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return false;
    }
    
    // Se for recarregamento assíncrono, iniciar em thread /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param async Parâmetro async
 * @return Retorna separada
 */
 separada
    if(async){
        std::thread([this, id]() {
            reloadResource<T>(id, false);
        }).detach();
        
        return true;
    }
    
    // Verificar se o recurso existe
    if (!resourceExists<T>(id)) {
        return false;
    }
    
    // Se o recurso foi modificado, salvá-lo primeiro
    if (isResourceModified<T>(id)) {
        saveResource<T>(id, true);
    }
    
    // Liberar referência atual
    releaseResource<T>(id);
    
    // Obter metadados
    auto metadata = getOrCreateMetadata<T>(id);
    
    // Marcar como não carregado
    metadata->resource = std::any();
    metadata->loaded = false;
    metadata->referenceCount = 0;
    
    // Carregar novamente
    auto resource = loadResource<T>(id, false);
    
    return (resource != nullptr);
}

template<typename T>
int ResourceManager::loadAllResources(bool async) {
    if (!m_initialized || !isSupportedType<T>()) {
        return 0;
    }
    
    // Implementação específica por tipo
    // Esta é uma simplificação - em um sistema real, seria necessário
    // percorrer arquivos no disco ou fazer consultas no banco de dados
    return 0;
}

template<typename T>
std::vector<std::shared_ptr<T>> ResourceManager::getAllResources() {
    std::vector<std::shared_ptr<T>> result;
    
    if (!m_initialized || !isSupportedType<T>()) {
        return result;
    }
    
    std::string typeName = getTypeName<T>();
    
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    
    auto typeIt = m_resources.find(typeName);
    if (typeIt == m_resources.end()) {
        return result;
    }
    
    auto& typeMap = typeIt->second;
    
    for (const auto& pair : typeMap) {
        if (pair.second->loaded) {
            try {
                auto resource = std::any_cast<std::shared_ptr<T>>(pair.second->resource);
                result.push_back(resource);
                
                // Atualizar metadados
                pair.second->referenceCount++;
                pair.second->lastAccess = std::chrono::steady_clock::now();
            }
            catch (const std::bad_any_cast&) {
                // Tipo incompatível, ignorar
            }
        }
    }
    
    return result;
}

template<typename T>
int ResourceManager::releaseAllResources() {
    if (!m_initialized || !isSupportedType<T>()) {
        return 0;
    }
    
    std::string typeName = getTypeName<T>();
    int releasedCount = 0;
    
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    
    auto typeIt = m_resources.find(typeName);
    if (typeIt == m_resources.end()) {
        return 0;
    }
    
    auto& typeMap = typeIt->second;
    
    for (auto& pair : typeMap) {
        if (pair.second->loaded) {
            // Verificar se o recurso foi modificado e precisa ser /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param pair.second->modified Parâmetro pair.second->modified
 * @return Retorna salvo
 */
 salvo
            if(pair.second->modified){
                // Salvar o recurso
                saveResource<T>(pair.first, true);
            }
            
            // Descarregar recurso
            pair.second->resource = std::any();
            pair.second->loaded = false;
            pair.second->referenceCount = 0;
            
            releasedCount++;
            m_resourcesLoaded--;
        }
    }
    
    return releasedCount;
}

template<typename T>
void ResourceManager::setResourceLoader(std::function<std::shared_ptr<T>(const std::string&)> loader) {
    if (!m_initialized || !loader || !isSupportedType<T>()) {
        return;
    }
    
    std::string typeName = getTypeName<T>();
    
    std::lock_guard<std::mutex> lock(m_functionsMutex);
    m_customLoaders[typeName] = loader;
}

template<typename T>
void ResourceManager::setResourceSaver(std::function<bool(const std::string&, const std::shared_ptr<T>&)> saver) {
    if (!m_initialized || !saver || !isSupportedType<T>()) {
        return;
    }
    
    std::string typeName = getTypeName<T>();
    
    std::lock_guard<std::mutex> lock(m_functionsMutex);
    m_customSavers[typeName] = saver;
}

template<typename T>
bool ResourceManager::saveResource(const std::string& id, bool markAsSaved) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return false;
    }
    
    // Verificar se o recurso está carregado
    auto resource = getResource<T>(id);
    if (!resource) {
        return false;
    }
    
    // Determinar a função de salvamento
    std::function<bool(const std::string&, const std::shared_ptr<T>&)> saver;
    
    {
        std::lock_guard<std::mutex> lock(m_functionsMutex);
        
        std::string typeName = getTypeName<T>();
        auto saverIt = m_customSavers.find(typeName);
        
        if (saverIt != m_customSavers.end()) {
            try {
                saver = std::any_cast<std::function<bool(const std::string&, const std::shared_ptr<T>&)>>(saverIt->second);
            }
            catch (const std::bad_any_cast&) {
                // Tipo incompatível, usar saver padrão
                saver = std::bind(&ResourceManager::defaultResourceSaver<T>, this, 
                                std::placeholders::_1, std::placeholders::_2);
            }
        }
        else {
            // Não há saver customizado, usar padrão
            saver = std::bind(&ResourceManager::defaultResourceSaver<T>, this, 
                            std::placeholders::_1, std::placeholders::_2);
        }
    }
    
    // Salvar o recurso
    bool success = saver(id, resource);
    
    if (success && markAsSaved) {
        // Marcar como não modificado
        std::lock_guard<std::mutex> lock(m_resourcesMutex);
        
        std::string typeName = getTypeName<T>();
        auto& typeMap = m_resources[typeName];
        auto resourceIt = typeMap.find(id);
        
        if (resourceIt != typeMap.end()) {
            resourceIt->second->modified = false;
        }
    }
    
    return success;
}

template<typename T>
bool ResourceManager::markResourceAsModified(const std::string& id) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return false;
    }
    
    std::string typeName = getTypeName<T>();
    
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    
    auto typeIt = m_resources.find(typeName);
    if (typeIt == m_resources.end()) {
        return false;
    }
    
    auto& typeMap = typeIt->second;
    auto resourceIt = typeMap.find(id);
    
    if (resourceIt == typeMap.end() || !resourceIt->second->loaded) {
        return false;
    }
    
    resourceIt->second->modified = true;
    return true;
}

template<typename T>
bool ResourceManager::isResourceModified(const std::string& id) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return false;
    }
    
    std::string typeName = getTypeName<T>();
    
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    
    auto typeIt = m_resources.find(typeName);
    if (typeIt == m_resources.end()) {
        return false;
    }
    
    auto& typeMap = typeIt->second;
    auto resourceIt = typeMap.find(id);
    
    if (resourceIt == typeMap.end() || !resourceIt->second->loaded) {
        return false;
    }
    
    return resourceIt->second->modified;
}

template<typename T>
std::string ResourceManager::getResourcePath(const std::string& id) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return "";
    }
    
    // Obter metadados
    auto metadata = getOrCreateMetadata<T>(id);
    
    return metadata->filePath;
}

template<typename T>
bool ResourceManager::resourceExists(const std::string& id) {
    if (!m_initialized || id.empty() || !isSupportedType<T>()) {
        return false;
    }
    
    // Verificar se o recurso já está carregado
    if (isResourceLoaded<T>(id)) {
        return true;
    }
    
    // Implementação específica por tipo
    // Esta é uma simplificação - em um sistema real, seria necessário
    // verificar a existência do arquivo no disco ou no banco de dados
    return false;
}

template<typename T>
std::string ResourceManager::getTypeName() {
    return typeid(T).name();
}

template<typename T>
bool ResourceManager::isSupportedType() {
    // Verificar se o tipo é suportado pelo sistema
    // Implementação depende dos tipos específicos do jogo
    return true;
}

template<typename T>
std::shared_ptr<T> ResourceManager::defaultResourceLoader(const std::string& id) {
    // Implementação padrão para carregamento de recursos
    // Deve ser especializada para cada tipo específico
    return nullptr;
}

template<typename T>
bool ResourceManager::defaultResourceSaver(const std::string& id, const std::shared_ptr<T>& resource) {
    // Implementação padrão para salvamento de recursos
    // Deve ser especializada para cada tipo específico
    return false;
}

template<typename T>
std::shared_ptr<ResourceManager::ResourceMetadata> ResourceManager::getOrCreateMetadata(const std::string& id) {
    std::string typeName = getTypeName<T>();
    
    std::lock_guard<std::mutex> lock(m_resourcesMutex);
    
    auto& typeMap = m_resources[typeName];
    auto it = typeMap.find(id);
    
    if (it == typeMap.end()) {
        // Criar novo metadado
        auto metadata = std::make_shared<ResourceMetadata>();
        metadata->id = id;
        metadata->filePath = m_resourcePath + "/" + determineFilePath(typeName, id);
        
        typeMap[id] = metadata;
        m_totalResourcesCreated++;
        
        return metadata;
    }
    
    return it->second;
}

#endif // _RESOURCE_MANAGER_H_

} // namespace wydbr
