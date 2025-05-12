/**
 * WYDMap.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/WYDMap.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/world/WYDMap.h"
#include "src/server/tmsrv/world/WorldManager.h"
#include "src/server/tmsrv/world/zones/WYDZone.h"
#include "src/server/tmsrv/world/zones/WYDTrigger.h"
#include "src/server/tmsrv/world/zones/WYDHeightmap.h"
#include "src/server/tmsrv/world/zones/WYDAtmosphere.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_set>

namespace wydbr {


// Construtor
WYDMap::WYDMap(WorldManager* worldManager, const std::string& mapId)
    : worldManager_(worldManager), 
      mapId_(mapId), 
      width_(0), 
      height_(0) {
}

// Destrutor
WYDMap::~WYDMap() {
    // Limpeza de recursos
}

// Carregamento do mapa a partir de arquivo
bool WYDMap::loadFromFile(const std::string& mapPath) {
    std::cout << "[WYDMap] Carregando mapa " << mapId_ << " de " << mapPath << std::endl;
    
    filePath_ = mapPath;
    
    try {
        // Abre o arquivo
        /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param mapPath Parâmetro mapPath
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

        std::ifstream file(mapPath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "[WYDMap] Erro ao abrir arquivo de mapa: " << mapPath << std::endl;
            return false;
        }
        
        // Lê cabeçalho do mapa
        char mapName[32] = {0};
        file.read(mapName, 32);
        name_ = mapName;
        
        // Lê dimensões
        uint16_t width, height;
        file.read(reinterpret_cast<char*>(&width), sizeof(width));
        file.read(reinterpret_cast<char*>(&height), sizeof(height));
        
        width_ = width;
        height_ = height;
        
        std::cout << "[WYDMap] Mapa " << name_ << " carregado: " << width_ << "x" << height_ << std::endl;
        
        // Inicializa células do /**
 * initializeCells
 * 
 * Implementa a funcionalidade initializeCells conforme especificação original.
 * @param width_ Parâmetro width_
 * @param height_ Parâmetro height_
 * @return Retorna mapa
 */
 mapa
        initializeCells(width_, height_);
        
        // Lê dados das cé/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param y++ Parâmetro y++
 * @return Retorna lulas
 */
lulas
        for(uint16_t y = 0; y < height_; y++){
            for (uint16_t x = 0; x < width_; x++) {
                uint8_t cellData;
                file.read(reinterpret_cast<char*>(&cellData), 1);
                updateCell(x, y, cellData);
            }
        }
        
        // Lê informações adicionais (zonas, triggers, heightmap, etc.)
        // O formato exato dependeria do arquivo real do WYD
        
        // Exemplo: Criar zonas padrã/**
 * createDefaultZones
 * 
 * Implementa a funcionalidade createDefaultZones conforme especificação original.
 * @return Retorna o
 */
o
        createDefaultZones();
        
        // Exemplo: Criar gatilhos de /**
 * createDefaultTriggers
 * 
 * Implementa a funcionalidade createDefaultTriggers conforme especificação original.
 * @return Retorna teleporte
 */
 teleporte
        createDefaultTriggers();
        
        return true;
    
    } catch (const std::exception& e) {
        std::cerr << "[WYDMap] Erro ao carregar mapa: " << e.what() << std::endl;
        return false;
    }
}

// Inicializa células do mapa
void WYDMap::initializeCells(uint16_t width, uint16_t height) {
    cells_.resize(width * height, 0);
    std::cout << "[WYDMap] Inicializadas " << cells_.size() << " células" << std::endl;
}

// Atualiza célula do mapa
void WYDMap::updateCell(uint16_t x, uint16_t y, uint8_t flags) {
    if (isInBounds(x, y)) {
        cells_[y * width_ + x] = flags;
    }
}

// Obtém flags de uma célula
uint8_t WYDMap::getCellFlags(uint16_t x, uint16_t y) const {
    if (isInBounds(x, y)) {
        return cells_[y * width_ + x];
    }
    return static_cast<uint8_t>(CellFlag::BLOCKED);
}

// Verifica se uma posição é válida
bool WYDMap::isValidPosition(uint16_t x, uint16_t y) const {
    /**
 * isInBounds
 * 
 * Implementa a funcionalidade isInBounds conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna return
 */

    return isInBounds(x, y);
}

// Verifica se uma posição é caminhável
bool WYDMap::isWalkable(uint16_t x, uint16_t y) const {
    if (!isInBounds(x, y)) {
        return false;
    }
    
    uint8_t flags = getCellFlags(x, y);
    return (flags & static_cast<uint8_t>(CellFlag::BLOCKED)) == 0 &&
           (flags & static_cast<uint8_t>(CellFlag::WATER)) == 0;
}

// Verifica se uma célula tem uma flag específica
bool WYDMap::hasCellFlag(uint16_t x, uint16_t y, CellFlag flag) const {
    if (!isInBounds(x, y)) {
        return false;
    }
    
    uint8_t flags = getCellFlags(x, y);
    return (flags & static_cast<uint8_t>(flag)) != 0;
}

// Verifica se uma posição está em zona segura
bool WYDMap::isInSafeZone(uint16_t x, uint16_t y) const {
    /**
 * hasCellFlag
 * 
 * Implementa a funcionalidade hasCellFlag conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param CellFlag::SAFEZONE Parâmetro CellFlag::SAFEZONE
 * @return Retorna return
 */

    return hasCellFlag(x, y, CellFlag::SAFEZONE);
}

// Verifica se uma posição está em zona de castelo
bool WYDMap::isInCastleZone(uint16_t x, uint16_t y) const {
    /**
 * hasCellFlag
 * 
 * Implementa a funcionalidade hasCellFlag conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param CellFlag::CASTLE Parâmetro CellFlag::CASTLE
 * @return Retorna return
 */

    return hasCellFlag(x, y, CellFlag::CASTLE);
}

// Obtém altura do terreno
float WYDMap::getHeight(uint16_t x, uint16_t y) const {
    if (heightmap_ && isInBounds(x, y)) {
        return heightmap_->getHeight(x, y);
    }
    return 0.0f;
}

// Adiciona entidade ao mapa
bool WYDMap::addEntity(uint32_t entityId, uint16_t x, uint16_t y) {
    std::lock_guard<std::mutex> lock(entityMutex_);
    
    // Verifica se posição é válida
    if (!isValidPosition(x, y) || !isWalkable(x, y)) {
        return false;
    }
    
    // Verifica se entidade já está no mapa
    auto it = entityPositions_.find(entityId);
    if (it != entityPositions_.end()) {
        // Já está no mapa, atualiza posição
        auto oldPos = it->second;
        
        // Remove da antiga posição
        auto posIt = positionEntities_.find(oldPos);
        if (posIt != positionEntities_.end()) {
            posIt->second.erase(entityId);
            if (posIt->second.empty()) {
                positionEntities_.erase(posIt);
            }
        }
        
        // Atualiza para nova posição
        it->second = std::make_pair(x, y);
    } else {
        // Adiciona nova entidade
        entityPositions_[entityId] = std::make_pair(x, y);
    }
    
    // Adiciona à lista de entidades na posição
    positionEntities_[std::make_pair(x, y)].insert(entityId);
    
    return true;
}

// Remove entidade do mapa
bool WYDMap::removeEntity(uint32_t entityId) {
    std::lock_guard<std::mutex> lock(entityMutex_);
    
    // Verifica se entidade está no mapa
    auto it = entityPositions_.find(entityId);
    if (it == entityPositions_.end()) {
        return false;
    }
    
    // Remove da lista de posições
    auto pos = it->second;
    auto posIt = positionEntities_.find(pos);
    if (posIt != positionEntities_.end()) {
        posIt->second.erase(entityId);
        if (posIt->second.empty()) {
            positionEntities_.erase(posIt);
        }
    }
    
    // Remove da lista de entidades
    entityPositions_.erase(it);
    
    return true;
}

// Move entidade no mapa
bool WYDMap::moveEntity(uint32_t entityId, uint16_t newX, uint16_t newY) {
    std::lock_guard<std::mutex> lock(entityMutex_);
    
    // Verifica se posição é válida
    if (!isValidPosition(newX, newY) || !isWalkable(newX, newY)) {
        return false;
    }
    
    // Verifica se entidade está no mapa
    auto it = entityPositions_.find(entityId);
    if (it == entityPositions_.end()) {
        return false;
    }
    
    // Remove da antiga posição
    auto oldPos = it->second;
    auto posIt = positionEntities_.find(oldPos);
    if (posIt != positionEntities_.end()) {
        posIt->second.erase(entityId);
        if (posIt->second.empty()) {
            positionEntities_.erase(posIt);
        }
    }
    
    // Atualiza para nova posição
    it->second = std::make_pair(newX, newY);
    
    // Adiciona à lista de entidades na nova posição
    positionEntities_[std::make_pair(newX, newY)].insert(entityId);
    
    return true;
}

// Obtém posição de uma entidade
bool WYDMap::getEntityPosition(uint32_t entityId, uint16_t& outX, uint16_t& outY) const {
    std::lock_guard<std::mutex> lock(entityMutex_);
    
    // Verifica se entidade está no mapa
    auto it = entityPositions_.find(entityId);
    if (it == entityPositions_.end()) {
        return false;
    }
    
    // Retorna posição
    outX = it->second.first;
    outY = it->second.second;
    
    return true;
}

// Obtém entidades em uma área
std::vector<uint32_t> WYDMap::getEntitiesInArea(uint16_t centerX, uint16_t centerY, uint16_t radius) const {
    std::lock_guard<std::mutex> lock(entityMutex_);
    
    std::vector<uint32_t> entities;
    
    // Calcula limites da área
    uint16_t minX = std::max(0, static_cast<int>(centerX) - radius);
    uint16_t maxX = std::min(static_cast<int>(width_ - 1), static_cast<int>(centerX) + radius);
    uint16_t minY = std::max(0, static_cast<int>(centerY) - radius);
    uint16_t maxY = std::min(static_cast<int>(height_ - 1), static_cast<int>(centerY) + radius);
    
    // Percorre todas as posições na á/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param y++ Parâmetro y++
 * @return Retorna rea
 */
rea
    for(uint16_t y = minY; y <= maxY; y++){
        for (uint16_t x = minX; x <= maxX; x++) {
            // Verifica se há entidades nesta posição
            auto posIt = positionEntities_.find(std::make_pair(x, y));
            if (posIt != positionEntities_.end()) {
                // Adiciona todas as entidades desta posição
                entities.insert(entities.end(), posIt->second.begin(), posIt->second.end());
            }
        }
    }
    
    return entities;
}

// Encontra caminho entre dois pontos (algoritmo A*)
std::vector<std::pair<uint16_t, uint16_t>> WYDMap::findPath(
    uint16_t startX, uint16_t startY, 
    uint16_t endX, uint16_t endY, 
    uint16_t maxDistance) const {
    
    // Verifica se posições são válidas
    if (!isValidPosition(startX, startY) || !isValidPosition(endX, endY)) {
        return {};
    }
    
    // Verifica se destino é caminhável
    if (!isWalkable(endX, endY)) {
        return {};
    }
    
    // Estruturas para o algoritmo A*
    struct Node {
        uint16_t x, y;
        float g; // Custo do início até este nó
        float h; // Heurística (estimativa do custo deste nó até o destino)
        float f; // Custo total (g + h)
        std::pair<uint16_t, uint16_t> parent;
        
        Node(uint16_t _x, uint16_t _y, float _g, float _h, std::pair<uint16_t, uint16_t> _parent)
            : x(_x), y(_y), g(_g), h(_h), f(_g + _h), parent(_parent) {}
        
        bool operator<(const Node& other) const {
            if (f == other.f) {
                // Desempate por h (heurística)
                return h > other.h;
            }
            return f > other.f; // Ordenação por menor f (prioridade)
        }
    };
    
    // Função para calcular heurística (distância de Manhattan)
    auto heuristic = [](uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) -> float {
        return std::abs(static_cast<int>(x1) - static_cast<int>(x2)) +
               std::abs(static_cast<int>(y1) - static_cast<int>(y2));
    };
    
    // Fila de prioridade para nós abertos
    std::priority_queue<Node> openSet;
    
    // Conjunto de nós fechados
    std::unordered_set<size_t> closedSet;
    
    // Mapa de nós já processados para reconstrução de caminho
    std::map<std::pair<uint16_t, uint16_t>, std::pair<uint16_t, uint16_t>> cameFrom;
    
    // Adiciona nó inicial
    openSet.push(Node(startX, startY, 0, heuristic(startX, startY, endX, endY), std::make_pair(startX, startY)));
    
    // Função para gerar chave única para conjunto fechado
    auto getKey = [this](uint16_t x, uint16_t y) -> size_t {
        return static_cast<size_t>(y) * width_ + x;
    };
    
    // Direções possíveis de movimento (4 direções cardinais)
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};
    
    // Loop principal do A*
    while (!openSet.empty()) {
        // Obtém nó com menor f
        Node current = openSet.top();
        openSet.pop();
        
        // Chegou ao destino?
        if (current.x == endX && current.y == endY) {
            // Reconstrói caminho
            std::vector<std::pair<uint16_t, uint16_t>> path;
            std::pair<uint16_t, uint16_t> current_pos = std::make_pair(endX, endY);
            
            while (current_pos.first != startX || current_pos.second != startY) {
                path.push_back(current_pos);
                current_pos = cameFrom[current_pos];
            }
            
            // Inverte caminho (do início para o fim)
            std::reverse(path.begin(), path.end());
            
            return path;
        }
        
        // Adiciona ao conjunto fechado
        size_t currentKey = getKey(current.x, current.y);
        if (closedSet.find(currentKey) != closedSet.end()) {
            continue; // Já processado
        }
        closedSet.insert(currentKey);
        
        // Verifica se atingiu distância má/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param maxDistance Parâmetro maxDistance
 * @return Retorna xima
 */
xima
        if(current.g >= maxDistance){
            continue;
        }
        
        // Explora /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna vizinhos
 */
 vizinhos
        for(int i = 0; i < 4; i++){
            uint16_t nx = current.x + dx[i];
            uint16_t ny = current.y + dy[i];
            
            // Verifica se posição é válida e caminhável
            if (!isValidPosition(nx, ny) || !isWalkable(nx, ny)) {
                continue;
            }
            
            // Verifica se já está no conjunto fechado
            size_t neighborKey = getKey(nx, ny);
            if (closedSet.find(neighborKey) != closedSet.end()) {
                continue;
            }
            
            // Calcula custo g para este vizinho
            float g = current.g + 1.0f; // Custo de movimento = 1
            
            // Calcula heurística
            float h = heuristic(nx, ny, endX, endY);
            
            // Adiciona ao conjunto aberto
            openSet.push(Node(nx, ny, g, h, std::make_pair(current.x, current.y)));
            
            // Registra de onde veio
            cameFrom[std::make_pair(nx, ny)] = std::make_pair(current.x, current.y);
        }
    }
    
    // Não encontrou caminho
    return {};
}

// Verifica linha de visão entre dois pontos
bool WYDMap::hasLineOfSight(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY) const {
    // Verifica se posições são válidas
    if (!isValidPosition(startX, startY) || !isValidPosition(endX, endY)) {
        return false;
    }
    
    // Algoritmo de Bresenham para traçar linha
    int dx = std::abs(static_cast<int>(endX) - static_cast<int>(startX));
    int dy = std::abs(static_cast<int>(endY) - static_cast<int>(startY));
    int sx = startX < endX ? 1 : -1;
    int sy = startY < endY ? 1 : -1;
    int err = dx - dy;
    
    int x = startX;
    int y = startY;
    
    while (x != endX || y != endY) {
        // Verifica se célula atual é bloqueada
        if (!isWalkable(x, y)) {
            return false;
        }
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
    
    return true;
}

// Obtém zona do mapa para uma posição
WYDZone* WYDMap::getZone(uint16_t x, uint16_t y) {
    // Implementação simplificada, seria expandida com zonas reais
    // Por agora, retorna nullptr
    return nullptr;
}

// Verifica se existe gatilho de teleporte
bool WYDMap::hasTeleportTrigger(uint16_t x, uint16_t y, std::string& outDestMap, uint16_t& outDestX, uint16_t& outDestY) const {
    // Implementação simplificada, seria expandida com triggers reais
    
    // Verifica se é célula de teleporte
    if (!hasCellFlag(x, y, CellFlag::TELEPORT)) {
        return false;
    }
    
    // Procura gatilho nesta posiçã/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param triggers_ Parâmetro triggers_
 * @return Retorna o
 */
o
    for(const auto& trigger : triggers_){
        if (trigger->getType() == TriggerType::TELEPORT && 
            trigger->getX() == x && 
            trigger->getY() == y) {
            
            // Obtém informações de destino
            auto teleportTrigger = static_cast<TeleportTrigger*>(trigger.get());
            outDestMap = teleportTrigger->getDestinationMap();
            outDestX = teleportTrigger->getDestinationX();
            outDestY = teleportTrigger->getDestinationY();
            
            return true;
        }
    }
    
    return false;
}

// Atualiza o mapa
void WYDMap::update(uint64_t timestamp) {
    // Atualiza atmosfera, se /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param atmosphere_ Parâmetro atmosphere_
 * @return Retorna existir
 */
 existir
    if(atmosphere_){
        atmosphere_->update(timestamp);
    }
    
    // Atualiza /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param zones_ Parâmetro zones_
 * @return Retorna zonas
 */
 zonas
    for(auto& zone : zones_){
        zone->update(timestamp);
    }
    
    // Atualiza /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param triggers_ Parâmetro triggers_
 * @return Retorna gatilhos
 */
 gatilhos
    for(auto& trigger : triggers_){
        trigger->update(timestamp);
    }
}

// Métodos auxiliares

// Verifica se coordenadas estão dentro dos limites do mapa
bool WYDMap::isInBounds(uint16_t x, uint16_t y) const {
    return x < width_ && y < height_;
}

// Calcula distância entre dois pontos
float WYDMap::calculateDistance(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const {
    float dx = static_cast<float>(x2) - static_cast<float>(x1);
    float dy = static_cast<float>(y2) - static_cast<float>(y1);
    return std::sqrt(dx * dx + dy * dy);
}

// Obtém índices de zona para uma posição
std::pair<uint16_t, uint16_t> WYDMap::getZoneIndices(uint16_t x, uint16_t y) const {
    // Implementação simplificada, seria expandida com zonas reais
    // Por agora, retorna (0, 0)
    return std::make_pair(0, 0);
}

// Cria zonas padrão para o mapa
void WYDMap::createDefaultZones() {
    // Implementação simplificada, seria expandida com zonas reais
    std::cout << "[WYDMap] Criando zonas padrão para o mapa" << std::endl;
    
    // No futuro, analisaria o mapa e criaria zonas para regiões específicas
}

// Cria gatilhos de teleporte padrão
void WYDMap::createDefaultTriggers() {

} // namespace wydbr

    // Implementação simplificada, seria expandida com triggers reais
    std::cout << "[WYDMap] Criando gatilhos padrão para o mapa" << std::endl;
    
    // No futuro, analisaria o mapa e criaria gatilhos para células com flag de teleporte
}