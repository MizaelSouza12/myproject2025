#include "WorldInstance.h"
#include <algorithm>
#include <cmath>
#include "../../common/utils/Logger.h"

namespace wydbr::tmsrv::world {

WorldInstance::WorldInstance(uint32_t instanceId, uint32_t mapId, uint8_t instanceType, const MapInfo& mapInfo)
    : m_instanceId(instanceId)
    , m_mapId(mapId)
    , m_instanceType(instanceType)
    , m_mapInfo(mapInfo)
    , m_initialized(false)
    , m_markedForRemoval(false)
    , m_lifetimeMs(0)
    , m_remainingLifetimeMs(0)
    , m_timeSinceLastUpdate(0)
{
}

WorldInstance::~WorldInstance() {
    if (m_initialized) {
        shutdown();
    }
}

bool WorldInstance::initialize() {
    if (m_initialized) {
        LOG_WARNING("Tentativa de inicializar instância já inicializada: ID=" + std::to_string(m_instanceId));
        return true;
    }
    
    LOG_INFO("Inicializando instância: ID=" + std::to_string(m_instanceId) + 
             ", Mapa=" + std::to_string(m_mapId) + 
             ", Tipo=" + std::to_string(m_instanceType));
    
    // Inicializa as regiões do mundo
    if (!initializeRegions()) {
        LOG_ERROR("Falha ao inicializar regiões da instância: ID=" + std::to_string(m_instanceId));
        return false;
    }
    
    m_initialized = true;
    LOG_INFO("Instância inicializada com sucesso: ID=" + std::to_string(m_instanceId));
    return true;
}

void WorldInstance::update(uint32_t deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    // Atualiza o tempo desde a última atualização
    m_timeSinceLastUpdate += deltaTime;
    
    // Atualiza o tempo de vida restante, se aplicável
    if (m_lifetimeMs > 0) {
        if (m_remainingLifetimeMs <= deltaTime) {
            m_remainingLifetimeMs = 0;
            markForRemoval();
        } else {
            m_remainingLifetimeMs -= deltaTime;
        }
    }
    
    // Atualiza todas as regiões
    for (auto& region : m_regions) {
        region->update(deltaTime);
    }
    
    // Se a instância estiver vazia e não for uma instância permanente, marca para remoção
    if (m_instanceType != static_cast<uint8_t>(InstanceType::PERMANENT) && isEmpty() && m_timeSinceLastUpdate >= 60000) {
        LOG_INFO("Instância vazia por 60 segundos, marcando para remoção: ID=" + std::to_string(m_instanceId));
        markForRemoval();
    }
    
    // Reinicia o contador de tempo desde a última atualização
    if (m_timeSinceLastUpdate >= 60000) {
        m_timeSinceLastUpdate = 0;
    }
}

void WorldInstance::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    LOG_INFO("Finalizando instância: ID=" + std::to_string(m_instanceId));
    
    // Limpa as posições dos atores
    m_actorPositions.clear();
    
    // Limpa as regiões
    m_regions.clear();
    
    m_initialized = false;
    LOG_INFO("Instância finalizada com sucesso: ID=" + std::to_string(m_instanceId));
}

bool WorldInstance::shouldBeRemoved() const {
    return m_markedForRemoval;
}

void WorldInstance::markForRemoval() {
    if (!m_markedForRemoval) {
        m_markedForRemoval = true;
        LOG_INFO("Instância marcada para remoção: ID=" + std::to_string(m_instanceId));
    }
}

bool WorldInstance::addActor(const common::types::ActorId& actorId, float x, float y) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de adicionar ator a uma instância não inicializada: ID=" + std::to_string(m_instanceId));
        return false;
    }
    
    // Verifica se o ator já existe na instância
    if (m_actorPositions.find(actorId) != m_actorPositions.end()) {
        LOG_WARNING("Ator já existe na instância: " + actorId.toString() + 
                   ", Instância=" + std::to_string(m_instanceId));
        return false;
    }
    
    // Verifica se as coordenadas estão dentro dos limites do mapa
    if (x < 0 || x >= m_mapInfo.width || y < 0 || y >= m_mapInfo.height) {
        LOG_WARNING("Coordenadas fora dos limites do mapa: (" + std::to_string(x) + ", " + std::to_string(y) + 
                   "), Limites=(" + std::to_string(m_mapInfo.width) + ", " + std::to_string(m_mapInfo.height) + 
                   "), Instância=" + std::to_string(m_instanceId));
        return false;
    }
    
    // Cria a posição do ator
    ActorPosition position;
    position.x = x;
    position.y = y;
    position.regionId = 0; // Valor padrão, será atualizado abaixo
    
    // Atualiza a região do ator
    WorldRegion* region = getRegionAt(x, y);
    if (!region) {
        LOG_ERROR("Não foi possível encontrar região para as coordenadas: (" + 
                 std::to_string(x) + ", " + std::to_string(y) + 
                 "), Instância=" + std::to_string(m_instanceId));
        return false;
    }
    
    // Adiciona o ator à região
    if (!region->addActor(actorId)) {
        LOG_ERROR("Falha ao adicionar ator à região: " + actorId.toString());
        return false;
    }
    
    // Atualiza o ID da região na posição do ator
    position.regionId = region->getRegionId();
    
    // Adiciona a posição do ator ao mapa
    m_actorPositions[actorId] = position;
    
    LOG_INFO("Ator adicionado à instância: " + actorId.toString() + 
             ", Instância=" + std::to_string(m_instanceId) + 
             ", Posição=(" + std::to_string(x) + ", " + std::to_string(y) + 
             "), Região=" + std::to_string(position.regionId));
    
    return true;
}

bool WorldInstance::removeActor(const common::types::ActorId& actorId) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de remover ator de uma instância não inicializada: ID=" + std::to_string(m_instanceId));
        return false;
    }
    
    // Verifica se o ator existe na instância
    auto actorIter = m_actorPositions.find(actorId);
    if (actorIter == m_actorPositions.end()) {
        LOG_WARNING("Tentativa de remover ator inexistente da instância: " + actorId.toString() + 
                   ", Instância=" + std::to_string(m_instanceId));
        return false;
    }
    
    // Obtém a região do ator
    uint32_t regionId = actorIter->second.regionId;
    WorldRegion* region = nullptr;
    
    // Encontra a região pelo ID
    for (auto& r : m_regions) {
        if (r->getRegionId() == regionId) {
            region = r.get();
            break;
        }
    }
    
    // Remove o ator da região, se encontrada
    if (region) {
        if (!region->removeActor(actorId)) {
            LOG_WARNING("Falha ao remover ator da região: " + actorId.toString() + 
                       ", Região=" + std::to_string(regionId));
        }
    } else {
        LOG_WARNING("Região não encontrada para ator: " + actorId.toString() + 
                   ", Região=" + std::to_string(regionId));
    }
    
    // Remove a posição do ator do mapa
    m_actorPositions.erase(actorId);
    
    LOG_INFO("Ator removido da instância: " + actorId.toString() + 
             ", Instância=" + std::to_string(m_instanceId));
    
    return true;
}

bool WorldInstance::moveActor(const common::types::ActorId& actorId, float x, float y) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de mover ator em uma instância não inicializada: ID=" + std::to_string(m_instanceId));
        return false;
    }
    
    // Verifica se o ator existe na instância
    auto actorIter = m_actorPositions.find(actorId);
    if (actorIter == m_actorPositions.end()) {
        LOG_WARNING("Tentativa de mover ator inexistente na instância: " + actorId.toString() + 
                   ", Instância=" + std::to_string(m_instanceId));
        return false;
    }
    
    // Verifica se as coordenadas estão dentro dos limites do mapa
    if (x < 0 || x >= m_mapInfo.width || y < 0 || y >= m_mapInfo.height) {
        LOG_WARNING("Coordenadas de movimento fora dos limites do mapa: (" + std::to_string(x) + ", " + std::to_string(y) + 
                   "), Limites=(" + std::to_string(m_mapInfo.width) + ", " + std::to_string(m_mapInfo.height) + 
                   "), Instância=" + std::to_string(m_instanceId));
        return false;
    }
    
    // Obtém a posição atual do ator
    ActorPosition& position = actorIter->second;
    
    // Se a posição não mudou, não faz nada
    if (std::abs(position.x - x) < 0.001f && std::abs(position.y - y) < 0.001f) {
        return true;
    }
    
    // Atualiza a região do ator, se necessário
    if (!updateActorRegion(actorId, x, y)) {
        LOG_ERROR("Falha ao atualizar região do ator durante movimento: " + actorId.toString());
        return false;
    }
    
    // Atualiza a posição do ator
    position.x = x;
    position.y = y;
    
    return true;
}

WorldRegion* WorldInstance::getRegionAt(float x, float y) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de obter região em uma instância não inicializada: ID=" + std::to_string(m_instanceId));
        return nullptr;
    }
    
    // Verifica se as coordenadas estão dentro dos limites do mapa
    if (x < 0 || x >= m_mapInfo.width || y < 0 || y >= m_mapInfo.height) {
        LOG_WARNING("Coordenadas fora dos limites do mapa: (" + std::to_string(x) + ", " + std::to_string(y) + 
                   "), Limites=(" + std::to_string(m_mapInfo.width) + ", " + std::to_string(m_mapInfo.height) + 
                   "), Instância=" + std::to_string(m_instanceId));
        return nullptr;
    }
    
    // Calcula o índice da região com base nas coordenadas
    uint32_t regionX = static_cast<uint32_t>(x / m_mapInfo.regionSize);
    uint32_t regionY = static_cast<uint32_t>(y / m_mapInfo.regionSize);
    uint32_t regionIndex = regionY * m_mapInfo.regionsX + regionX;
    
    // Verifica se o índice da região é válido
    if (regionIndex >= m_regions.size()) {
        LOG_ERROR("Índice de região inválido: " + std::to_string(regionIndex) + 
                 ", Total de regiões=" + std::to_string(m_regions.size()) + 
                 ", Coordenadas=(" + std::to_string(x) + ", " + std::to_string(y) + ")");
        return nullptr;
    }
    
    return m_regions[regionIndex].get();
}

uint32_t WorldInstance::getInstanceId() const {
    return m_instanceId;
}

uint32_t WorldInstance::getMapId() const {
    return m_mapId;
}

uint8_t WorldInstance::getInstanceType() const {
    return m_instanceType;
}

const MapInfo& WorldInstance::getMapInfo() const {
    return m_mapInfo;
}

bool WorldInstance::hasActor(const common::types::ActorId& actorId) const {
    return m_actorPositions.find(actorId) != m_actorPositions.end();
}

bool WorldInstance::getActorPosition(const common::types::ActorId& actorId, float& outX, float& outY) const {
    auto actorIter = m_actorPositions.find(actorId);
    if (actorIter == m_actorPositions.end()) {
        return false;
    }
    
    outX = actorIter->second.x;
    outY = actorIter->second.y;
    return true;
}

size_t WorldInstance::getActorCount() const {
    return m_actorPositions.size();
}

bool WorldInstance::isEmpty() const {
    return m_actorPositions.empty();
}

void WorldInstance::setLifetime(uint32_t lifetimeMs) {
    m_lifetimeMs = lifetimeMs;
    m_remainingLifetimeMs = lifetimeMs;
    
    if (lifetimeMs > 0) {
        LOG_INFO("Tempo de vida definido para instância: ID=" + std::to_string(m_instanceId) + 
                ", Tempo=" + std::to_string(lifetimeMs) + "ms");
    } else {
        LOG_INFO("Tempo de vida infinito definido para instância: ID=" + std::to_string(m_instanceId));
    }
}

uint32_t WorldInstance::getRemainingLifetime() const {
    return m_remainingLifetimeMs;
}

bool WorldInstance::initializeRegions() {
    // Limpa regiões existentes
    m_regions.clear();
    
    // Calcula o número de regiões com base no tamanho do mapa e no tamanho da região
    uint32_t regionsX = m_mapInfo.regionsX;
    uint32_t regionsY = m_mapInfo.regionsY;
    uint32_t totalRegions = regionsX * regionsY;
    
    LOG_INFO("Inicializando " + std::to_string(totalRegions) + " regiões (" + 
             std::to_string(regionsX) + "x" + std::to_string(regionsY) + 
             ") para instância: ID=" + std::to_string(m_instanceId));
    
    // Cria as regiões
    m_regions.reserve(totalRegions);
    for (uint32_t y = 0; y < regionsY; ++y) {
        for (uint32_t x = 0; x < regionsX; ++x) {
            uint32_t regionId = y * regionsX + x;
            float startX = x * m_mapInfo.regionSize;
            float startY = y * m_mapInfo.regionSize;
            float endX = std::min(startX + m_mapInfo.regionSize, static_cast<float>(m_mapInfo.width));
            float endY = std::min(startY + m_mapInfo.regionSize, static_cast<float>(m_mapInfo.height));
            
            auto region = std::make_unique<WorldRegion>(regionId, startX, startY, endX, endY);
            
            // Inicializa a região
            if (!region->initialize()) {
                LOG_ERROR("Falha ao inicializar região: ID=" + std::to_string(regionId) + 
                         ", Instância=" + std::to_string(m_instanceId));
                return false;
            }
            
            m_regions.push_back(std::move(region));
        }
    }
    
    LOG_INFO("Regiões inicializadas com sucesso para instância: ID=" + std::to_string(m_instanceId));
    return true;
}

bool WorldInstance::updateActorRegion(const common::types::ActorId& actorId, float x, float y) {
    // Verifica se o ator existe na instância
    auto actorIter = m_actorPositions.find(actorId);
    if (actorIter == m_actorPositions.end()) {
        LOG_ERROR("Tentativa de atualizar região de ator inexistente: " + actorId.toString());
        return false;
    }
    
    // Obtém a região atual do ator
    uint32_t currentRegionId = actorIter->second.regionId;
    
    // Obtém a nova região com base nas coordenadas
    WorldRegion* newRegion = getRegionAt(x, y);
    if (!newRegion) {
        LOG_ERROR("Não foi possível encontrar região para as coordenadas: (" + 
                 std::to_string(x) + ", " + std::to_string(y) + ")");
        return false;
    }
    
    uint32_t newRegionId = newRegion->getRegionId();
    
    // Se a região não mudou, não faz nada
    if (currentRegionId == newRegionId) {
        return true;
    }
    
    // Encontra a região atual pelo ID
    WorldRegion* currentRegion = nullptr;
    for (auto& r : m_regions) {
        if (r->getRegionId() == currentRegionId) {
            currentRegion = r.get();
            break;
        }
    }
    
    // Remove o ator da região atual, se encontrada
    if (currentRegion) {
        if (!currentRegion->removeActor(actorId)) {
            LOG_WARNING("Falha ao remover ator da região atual: " + actorId.toString() + 
                       ", Região=" + std::to_string(currentRegionId));
        }
    } else {
        LOG_WARNING("Região atual não encontrada para ator: " + actorId.toString() + 
                   ", Região=" + std::to_string(currentRegionId));
    }
    
    // Adiciona o ator à nova região
    if (!newRegion->addActor(actorId)) {
        LOG_ERROR("Falha ao adicionar ator à nova região: " + actorId.toString() + 
                 ", Região=" + std::to_string(newRegionId));
        
        // Tenta restaurar o ator na região original
        if (currentRegion) {
            currentRegion->addActor(actorId);
        }
        
        return false;
    }
    
    // Atualiza o ID da região na posição do ator
    actorIter->second.regionId = newRegionId;
    
    LOG_DEBUG("Região do ator atualizada: " + actorId.toString() + 
             ", De=" + std::to_string(currentRegionId) + 
             ", Para=" + std::to_string(newRegionId));
    
    return true;
}

} // namespace wydbr::tmsrv::world 