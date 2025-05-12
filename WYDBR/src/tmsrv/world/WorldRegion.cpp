#include "WorldRegion.h"
#include "../../common/utils/Logger.h"

namespace wydbr::tmsrv::world {

WorldRegion::WorldRegion(uint32_t regionId, float startX, float startY, float endX, float endY)
    : m_regionId(regionId)
    , m_startX(startX)
    , m_startY(startY)
    , m_endX(endX)
    , m_endY(endY)
    , m_width(endX - startX)
    , m_height(endY - startY)
    , m_initialized(false)
{
}

WorldRegion::~WorldRegion() {
    if (m_initialized) {
        shutdown();
    }
}

bool WorldRegion::initialize() {
    if (m_initialized) {
        LOG_WARNING("Tentativa de inicializar região já inicializada: ID=" + std::to_string(m_regionId));
        return true;
    }
    
    LOG_DEBUG("Inicializando região: ID=" + std::to_string(m_regionId) + 
             ", Área=(" + std::to_string(m_startX) + "," + std::to_string(m_startY) + 
             ")-(" + std::to_string(m_endX) + "," + std::to_string(m_endY) + ")");
    
    // Limpa coleções
    m_actors.clear();
    m_neighbors.clear();
    
    m_initialized = true;
    return true;
}

void WorldRegion::update(uint32_t deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    // Atualmente, a região não tem comportamento dinâmico que precise ser atualizado
    // Esta função pode ser expandida no futuro para incluir lógica específica da região
}

void WorldRegion::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    LOG_DEBUG("Finalizando região: ID=" + std::to_string(m_regionId));
    
    // Limpa coleções
    m_actors.clear();
    m_neighbors.clear();
    
    m_initialized = false;
}

bool WorldRegion::addActor(const common::types::ActorId& actorId) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de adicionar ator a uma região não inicializada: ID=" + std::to_string(m_regionId));
        return false;
    }
    
    // Verifica se o ator já existe na região
    if (m_actors.find(actorId) != m_actors.end()) {
        LOG_WARNING("Ator já existe na região: " + actorId.toString() + 
                   ", Região=" + std::to_string(m_regionId));
        return false;
    }
    
    // Adiciona o ator à região
    m_actors.insert(actorId);
    
    LOG_DEBUG("Ator adicionado à região: " + actorId.toString() + 
             ", Região=" + std::to_string(m_regionId));
    
    return true;
}

bool WorldRegion::removeActor(const common::types::ActorId& actorId) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de remover ator de uma região não inicializada: ID=" + std::to_string(m_regionId));
        return false;
    }
    
    // Verifica se o ator existe na região
    auto actorIter = m_actors.find(actorId);
    if (actorIter == m_actors.end()) {
        LOG_WARNING("Tentativa de remover ator inexistente da região: " + actorId.toString() + 
                   ", Região=" + std::to_string(m_regionId));
        return false;
    }
    
    // Remove o ator da região
    m_actors.erase(actorIter);
    
    LOG_DEBUG("Ator removido da região: " + actorId.toString() + 
             ", Região=" + std::to_string(m_regionId));
    
    return true;
}

bool WorldRegion::hasActor(const common::types::ActorId& actorId) const {
    if (!m_initialized) {
        return false;
    }
    
    return m_actors.find(actorId) != m_actors.end();
}

uint32_t WorldRegion::getRegionId() const {
    return m_regionId;
}

float WorldRegion::getStartX() const {
    return m_startX;
}

float WorldRegion::getStartY() const {
    return m_startY;
}

float WorldRegion::getEndX() const {
    return m_endX;
}

float WorldRegion::getEndY() const {
    return m_endY;
}

float WorldRegion::getWidth() const {
    return m_width;
}

float WorldRegion::getHeight() const {
    return m_height;
}

bool WorldRegion::containsPoint(float x, float y) const {
    return (x >= m_startX && x < m_endX && y >= m_startY && y < m_endY);
}

const std::unordered_set<common::types::ActorId>& WorldRegion::getActors() const {
    return m_actors;
}

size_t WorldRegion::getActorCount() const {
    return m_actors.size();
}

bool WorldRegion::isEmpty() const {
    return m_actors.empty();
}

const std::vector<WorldRegion*>& WorldRegion::getNeighbors() const {
    return m_neighbors;
}

void WorldRegion::addNeighbor(WorldRegion* neighbor) {
    if (!m_initialized) {
        LOG_ERROR("Tentativa de adicionar vizinho a uma região não inicializada: ID=" + std::to_string(m_regionId));
        return;
    }
    
    if (!neighbor) {
        LOG_ERROR("Tentativa de adicionar vizinho nulo à região: ID=" + std::to_string(m_regionId));
        return;
    }
    
    // Verifica se o vizinho já existe
    for (auto existingNeighbor : m_neighbors) {
        if (existingNeighbor == neighbor) {
            return;
        }
    }
    
    // Adiciona o vizinho
    m_neighbors.push_back(neighbor);
    
    LOG_DEBUG("Vizinho adicionado à região: Região=" + std::to_string(m_regionId) + 
             ", Vizinho=" + std::to_string(neighbor->getRegionId()));
}

} // namespace wydbr::tmsrv::world 