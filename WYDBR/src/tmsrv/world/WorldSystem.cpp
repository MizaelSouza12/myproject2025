#include "WorldSystem.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "../../common/utils/Logger.h"

namespace wydbr::tmsrv::world {

WorldSystem& WorldSystem::getInstance() {
    static WorldSystem instance;
    return instance;
}

bool WorldSystem::initialize(const std::string& configPath) {
    if (m_initialized) {
        LOG_WARNING("WorldSystem já está inicializado");
        return true;
    }

    LOG_INFO("Inicializando WorldSystem com configuração: " + configPath);
    
    // Inicializa o gerenciador de timers
    m_timerManager = std::make_shared<common::utils::TimerManager>();
    
    // Carrega configurações do mundo
    try {
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            LOG_ERROR("Falha ao abrir arquivo de configuração do mundo: " + configPath);
            return false;
        }
        
        // Aqui seria implementada a leitura das configurações
        // Por exemplo, lendo o caminho para os arquivos de definição de mapas
        std::string mapDefinitionPath;
        // configFile >> mapDefinitionPath;
        
        // Temporariamente, usaremos um caminho padrão
        mapDefinitionPath = "data/MapList.bin";
        
        // Carrega definições de mapas
        if (!loadMapDefinitions(mapDefinitionPath)) {
            LOG_ERROR("Falha ao carregar definições de mapas");
            return false;
        }
        
        configFile.close();
    } catch (const std::exception& e) {
        LOG_ERROR("Exceção durante inicialização do WorldSystem: " + std::string(e.what()));
        return false;
    }
    
    m_initialized = true;
    LOG_INFO("WorldSystem inicializado com sucesso");
    return true;
}

void WorldSystem::update(uint32_t deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    // Atualiza o gerenciador de timers
    m_timerManager->update(deltaTime);
    
    // Atualiza todas as instâncias ativas
    for (auto& [instanceId, instance] : m_instances) {
        instance->update(deltaTime);
        
        // Remove instâncias marcadas para remoção
        if (instance->shouldBeRemoved()) {
            // Marca para remoção posterior para evitar problemas de iteração
            m_timerManager->scheduleTask([this, instanceId]() {
                this->removeInstance(instanceId);
            }, 0);
        }
    }
}

void WorldSystem::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    LOG_INFO("Finalizando WorldSystem");
    
    // Remove todas as instâncias
    m_instances.clear();
    
    // Limpa outros recursos
    m_mapDefinitions.clear();
    m_actorInstances.clear();
    
    // Finaliza o gerenciador de timers
    m_timerManager.reset();
    
    m_initialized = false;
    LOG_INFO("WorldSystem finalizado com sucesso");
}

std::shared_ptr<WorldInstance> WorldSystem::createInstance(uint32_t mapId, uint8_t instanceType) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de criar instância com WorldSystem não inicializado");
        return nullptr;
    }
    
    // Verifica se o mapa existe
    auto mapIter = m_mapDefinitions.find(mapId);
    if (mapIter == m_mapDefinitions.end()) {
        LOG_ERROR("Tentativa de criar instância para mapa inexistente: " + std::to_string(mapId));
        return nullptr;
    }
    
    // Gera um novo ID de instância
    uint32_t instanceId = m_nextInstanceId++;
    
    // Cria a nova instância
    auto instance = std::make_shared<WorldInstance>(instanceId, mapId, instanceType, mapIter->second);
    
    // Inicializa a instância
    if (!instance->initialize()) {
        LOG_ERROR("Falha ao inicializar instância de mapa: " + std::to_string(mapId));
        return nullptr;
    }
    
    // Adiciona à lista de instâncias
    m_instances[instanceId] = instance;
    
    LOG_INFO("Instância criada: ID=" + std::to_string(instanceId) + 
             ", Mapa=" + std::to_string(mapId) + 
             ", Tipo=" + std::to_string(instanceType));
    
    return instance;
}

bool WorldSystem::removeInstance(uint32_t instanceId) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de remover instância com WorldSystem não inicializado");
        return false;
    }
    
    // Verifica se a instância existe
    auto instanceIter = m_instances.find(instanceId);
    if (instanceIter == m_instances.end()) {
        LOG_WARNING("Tentativa de remover instância inexistente: " + std::to_string(instanceId));
        return false;
    }
    
    // Remove todos os atores desta instância
    for (auto it = m_actorInstances.begin(); it != m_actorInstances.end();) {
        if (it->second == instanceId) {
            it = m_actorInstances.erase(it);
        } else {
            ++it;
        }
    }
    
    // Finaliza a instância
    instanceIter->second->shutdown();
    
    // Remove da lista de instâncias
    m_instances.erase(instanceId);
    
    LOG_INFO("Instância removida: ID=" + std::to_string(instanceId));
    
    return true;
}

std::shared_ptr<WorldInstance> WorldSystem::getInstance(uint32_t instanceId) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de obter instância com WorldSystem não inicializado");
        return nullptr;
    }
    
    auto instanceIter = m_instances.find(instanceId);
    if (instanceIter == m_instances.end()) {
        return nullptr;
    }
    
    return instanceIter->second;
}

bool WorldSystem::addActor(const common::types::ActorId& actorId, uint32_t instanceId, float x, float y) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de adicionar ator com WorldSystem não inicializado");
        return false;
    }
    
    // Verifica se o ator já existe em alguma instância
    if (m_actorInstances.find(actorId) != m_actorInstances.end()) {
        LOG_WARNING("Ator já existe em uma instância: " + actorId.toString());
        return false;
    }
    
    // Verifica se a instância existe
    auto instanceIter = m_instances.find(instanceId);
    if (instanceIter == m_instances.end()) {
        LOG_ERROR("Tentativa de adicionar ator a uma instância inexistente: " + std::to_string(instanceId));
        return false;
    }
    
    // Adiciona o ator à instância
    if (!instanceIter->second->addActor(actorId, x, y)) {
        LOG_ERROR("Falha ao adicionar ator à instância");
        return false;
    }
    
    // Registra o ator no mapeamento de instâncias
    m_actorInstances[actorId] = instanceId;
    
    LOG_INFO("Ator adicionado: " + actorId.toString() + 
             " à instância " + std::to_string(instanceId) + 
             " na posição (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    
    return true;
}

bool WorldSystem::removeActor(const common::types::ActorId& actorId) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de remover ator com WorldSystem não inicializado");
        return false;
    }
    
    // Verifica se o ator existe em alguma instância
    auto actorInstanceIter = m_actorInstances.find(actorId);
    if (actorInstanceIter == m_actorInstances.end()) {
        LOG_WARNING("Tentativa de remover ator inexistente: " + actorId.toString());
        return false;
    }
    
    // Obtém a instância do ator
    uint32_t instanceId = actorInstanceIter->second;
    auto instanceIter = m_instances.find(instanceId);
    if (instanceIter == m_instances.end()) {
        // Situação inconsistente: ator registrado em uma instância inexistente
        LOG_ERROR("Inconsistência: ator " + actorId.toString() + 
                  " registrado na instância inexistente " + std::to_string(instanceId));
        m_actorInstances.erase(actorId);
        return false;
    }
    
    // Remove o ator da instância
    if (!instanceIter->second->removeActor(actorId)) {
        LOG_ERROR("Falha ao remover ator da instância");
        return false;
    }
    
    // Remove o ator do mapeamento de instâncias
    m_actorInstances.erase(actorId);
    
    LOG_INFO("Ator removido: " + actorId.toString() + 
             " da instância " + std::to_string(instanceId));
    
    return true;
}

bool WorldSystem::moveActor(const common::types::ActorId& actorId, float x, float y) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de mover ator com WorldSystem não inicializado");
        return false;
    }
    
    // Verifica se o ator existe em alguma instância
    auto actorInstanceIter = m_actorInstances.find(actorId);
    if (actorInstanceIter == m_actorInstances.end()) {
        LOG_WARNING("Tentativa de mover ator inexistente: " + actorId.toString());
        return false;
    }
    
    // Obtém a instância do ator
    uint32_t instanceId = actorInstanceIter->second;
    auto instanceIter = m_instances.find(instanceId);
    if (instanceIter == m_instances.end()) {
        // Situação inconsistente: ator registrado em uma instância inexistente
        LOG_ERROR("Inconsistência: ator " + actorId.toString() + 
                  " registrado na instância inexistente " + std::to_string(instanceId));
        return false;
    }
    
    // Move o ator na instância
    if (!instanceIter->second->moveActor(actorId, x, y)) {
        LOG_ERROR("Falha ao mover ator na instância");
        return false;
    }
    
    return true;
}

bool WorldSystem::teleportActor(const common::types::ActorId& actorId, uint32_t targetInstanceId, float x, float y) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de teleportar ator com WorldSystem não inicializado");
        return false;
    }
    
    // Verifica se o ator existe em alguma instância
    auto actorInstanceIter = m_actorInstances.find(actorId);
    if (actorInstanceIter == m_actorInstances.end()) {
        LOG_WARNING("Tentativa de teleportar ator inexistente: " + actorId.toString());
        return false;
    }
    
    // Verifica se a instância de destino existe
    auto targetInstanceIter = m_instances.find(targetInstanceId);
    if (targetInstanceIter == m_instances.end()) {
        LOG_ERROR("Tentativa de teleportar para instância inexistente: " + std::to_string(targetInstanceId));
        return false;
    }
    
    // Se o ator estiver na mesma instância, é apenas um movimento
    if (actorInstanceIter->second == targetInstanceId) {
        return moveActor(actorId, x, y);
    }
    
    // Obtém a instância atual do ator
    uint32_t currentInstanceId = actorInstanceIter->second;
    auto currentInstanceIter = m_instances.find(currentInstanceId);
    if (currentInstanceIter == m_instances.end()) {
        // Situação inconsistente: ator registrado em uma instância inexistente
        LOG_ERROR("Inconsistência: ator " + actorId.toString() + 
                  " registrado na instância inexistente " + std::to_string(currentInstanceId));
        return false;
    }
    
    // Remove o ator da instância atual
    if (!currentInstanceIter->second->removeActor(actorId)) {
        LOG_ERROR("Falha ao remover ator da instância atual durante teleporte");
        return false;
    }
    
    // Adiciona o ator à instância de destino
    if (!targetInstanceIter->second->addActor(actorId, x, y)) {
        LOG_ERROR("Falha ao adicionar ator à instância de destino durante teleporte");
        // Tenta restaurar o ator na instância original
        currentInstanceIter->second->addActor(actorId, 0, 0); // Posição padrão como fallback
        return false;
    }
    
    // Atualiza o mapeamento de instâncias
    m_actorInstances[actorId] = targetInstanceId;
    
    LOG_INFO("Ator teleportado: " + actorId.toString() + 
             " da instância " + std::to_string(currentInstanceId) + 
             " para instância " + std::to_string(targetInstanceId) + 
             " na posição (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    
    return true;
}

bool WorldSystem::loadMapDefinitions(const std::string& mapFilePath) {
    LOG_INFO("Carregando definições de mapas de: " + mapFilePath);
    
    try {
        std::ifstream mapFile(mapFilePath, std::ios::binary);
        if (!mapFile.is_open()) {
            LOG_ERROR("Falha ao abrir arquivo de definições de mapas: " + mapFilePath);
            return false;
        }
        
        // Limpa definições existentes
        m_mapDefinitions.clear();
        
        // Lê o cabeçalho do arquivo (número de mapas)
        uint32_t mapCount = 0;
        mapFile.read(reinterpret_cast<char*>(&mapCount), sizeof(mapCount));
        
        LOG_INFO("Carregando " + std::to_string(mapCount) + " definições de mapas");
        
        // Lê cada definição de mapa
        for (uint32_t i = 0; i < mapCount; ++i) {
            MapInfo mapInfo;
            
            // Lê o ID do mapa
            uint32_t mapId = 0;
            mapFile.read(reinterpret_cast<char*>(&mapId), sizeof(mapId));
            
            // Lê os dados do mapa
            mapFile.read(reinterpret_cast<char*>(&mapInfo), sizeof(MapInfo));
            
            // Adiciona ao mapa de definições
            m_mapDefinitions[mapId] = mapInfo;
            
            LOG_DEBUG("Mapa carregado: ID=" + std::to_string(mapId) + 
                     ", Nome=" + mapInfo.name);
        }
        
        mapFile.close();
        LOG_INFO("Definições de mapas carregadas com sucesso: " + 
                std::to_string(m_mapDefinitions.size()) + " mapas");
        
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Exceção durante carregamento de definições de mapas: " + std::string(e.what()));
        return false;
    }
}

const MapInfo* WorldSystem::getMapInfo(uint32_t mapId) const {
    auto mapIter = m_mapDefinitions.find(mapId);
    if (mapIter == m_mapDefinitions.end()) {
        return nullptr;
    }
    
    return &(mapIter->second);
}

WorldRegion* WorldSystem::getRegionAt(uint32_t instanceId, float x, float y) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de obter região com WorldSystem não inicializado");
        return nullptr;
    }
    
    // Verifica se a instância existe
    auto instanceIter = m_instances.find(instanceId);
    if (instanceIter == m_instances.end()) {
        LOG_ERROR("Tentativa de obter região em instância inexistente: " + std::to_string(instanceId));
        return nullptr;
    }
    
    // Obtém a região na instância
    return instanceIter->second->getRegionAt(x, y);
}

} // namespace wydbr::tmsrv::world 