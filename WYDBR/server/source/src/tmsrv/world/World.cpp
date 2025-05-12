/**
 * World.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/tmsrv/world/World.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "World.h"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>

namespace wydbr {
namespace tmsrv {

World::World(const std::string& name)
    : m_name(name), m_nextEntityID(1000), m_nextItemID(1), 
      m_initialized(false), m_worldTick(0) {
}

World::~World() {
    // Limpa recursos
    m_maps.clear();
    m_players.clear();
    m_mobs.clear();
    m_droppedItems.clear();
    m_spawnPoints.clear();
}

bool World::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Inicializa estruturas internas
    m_maps.clear();
    m_players.clear();
    m_mobs.clear();
    m_droppedItems.clear();
    m_spawnPoints.clear();
    
    m_initialized = true;
    m_worldTick = 0;
    
    return true;
}

void World::update(float deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    // Incrementa o tick do mundo
    m_worldTick++;
    
    // Atualiza /**
 * updateMobs
 * 
 * Implementa a funcionalidade updateMobs conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 * @return Retorna mobs
 */
 mobs
    updateMobs(deltaTime);
    
    // Atualiza itens no chã/**
 * updateDroppedItems
 * 
 * Implementa a funcionalidade updateDroppedItems conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 * @return Retorna o
 */
o
    updateDroppedItems(deltaTime);
    
    // Processa respawns de /**
 * processRespawns
 * 
 * Implementa a funcionalidade processRespawns conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 * @return Retorna mobs
 */
 mobs
    processRespawns(deltaTime);
    
    // Atualiza eventos do /**
 * updateWorldEvents
 * 
 * Implementa a funcionalidade updateWorldEvents conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 * @return Retorna mundo
 */
 mundo
    updateWorldEvents(deltaTime);
}

bool World::loadMap(int mapID, const std::string& fileName) {
    // Verifica se o mapa já existe
    if (m_maps.find(mapID) != m_maps.end()) {
        // O mapa já existe, vamos mantê-lo e apenas recarregar os dados
        std::cout << "Atualizando mapa existente: " << mapID << " (" << fileName << ")" << std::endl;
    }
    
    // Carrega o arquivo do mapa
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param fileName Parâmetro fileName
 * @return Retorna std::ifstream
 */

    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Falha ao abrir arquivo de mapa: " << fileName << std::endl;
        return false;
    }
    
    // Cria uma nova estrutura de mapa
    Map map;
    map.mapID = mapID;
    map.name = fileName;
    
    // Lê os dados do cabeçalho
    // [Implementação específica para o formato de arquivo de mapa do WYD]
    
    // Valores padrão para teste
    map.width = 512;
    map.height = 512;
    map.attributes = 0;
    
    // Inicializa as células do mapa
    map.cells.resize(map.width * map.height);
    for (auto& cell : map.cells) {
        cell.type = CellType::NORMAL;
        cell.allowMovement = true;
        cell.allowAttack = true;
        cell.entityIDs.clear();
    }
    
    // Lê os dados das células do mapa
    // [Implementação específica para o formato de arquivo de mapa do WYD]
    
    // Adiciona o mapa à lista
    m_maps[mapID] = map;
    
    std::cout << "Mapa carregado com sucesso: " << mapID << " (" << fileName << ")" << std::endl;
    return true;
}

bool World::registerPlayer(std::shared_ptr<WYDPlayer> player) {
    if (!player) {
        return false;
    }
    
    uint32_t playerID = player->getID();
    
    std::lock_guard<std::mutex> lock(m_playersMutex);
    
    // Verifica se o jogador já está registrado
    if (m_players.find(playerID) != m_players.end()) {
        return false;
    }
    
    // Registra o jogador
    m_players[playerID] = player;
    
    // Atualiza a célula do mapa com a posição do jogador
    STRUCT_MOB* mob = player->getMob();
    if (mob) {
        int mapID = mob->Area;
        int x = mob->PosX;
        int y = mob->PosY;
        
        // Verifica se a posição é válida
        if (isValidPosition(mapID, x, y)) {
            updateEntityCell(playerID, mapID, 0, 0, x, y);
        }
    }
    
    return true;
}

void World::removePlayer(uint32_t playerID) {
    std::lock_guard<std::mutex> lock(m_playersMutex);
    
    // Verifica se o jogador está registrado
    auto it = m_players.find(playerID);
    if (it == m_players.end()) {
        return;
    }
    
    // Obtém informações de posição
    STRUCT_MOB* mob = it->second->getMob();
    if (mob) {
        int mapID = mob->Area;
        int x = mob->PosX;
        int y = mob->PosY;
        
        // Remove o jogador da célula do mapa
        if (isValidPosition(mapID, x, y)) {
            auto& mapIt = m_maps.find(mapID);
            if (mapIt != m_maps.end()) {
                int cellIndex = getCellIndex(x, y, mapIt->second.width);
                auto& cell = mapIt->second.cells[cellIndex];
                
                // Remove o ID do jogador da célula
                cell.entityIDs.erase(
                    std::remove(cell.entityIDs.begin(), cell.entityIDs.end(), playerID),
                    cell.entityIDs.end()
                );
            }
        }
    }
    
    // Remove o jogador da lista
    m_players.erase(it);
}

std::shared_ptr<WYDPlayer> World::getPlayer(uint32_t playerID) {
    std::lock_guard<std::mutex> lock(m_playersMutex);
    
    auto it = m_players.find(playerID);
    if (it != m_players.end()) {
        return it->second;
    }
    
    return nullptr;
}

CellType World::getCellType(int mapID, int x, int y) {
    if (!isValidPosition(mapID, x, y)) {
        return CellType::OBSTACLE;
    }
    
    auto mapIt = m_maps.find(mapID);
    if (mapIt == m_maps.end()) {
        return CellType::OBSTACLE;
    }
    
    int cellIndex = getCellIndex(x, y, mapIt->second.width);
    return mapIt->second.cells[cellIndex].type;
}

bool World::isValidPosition(int mapID, int x, int y) {
    auto mapIt = m_maps.find(mapID);
    if (mapIt == m_maps.end()) {
        return false;
    }
    
    if (x < 0 || x >= mapIt->second.width || y < 0 || y >= mapIt->second.height) {
        return false;
    }
    
    return true;
}

bool World::isValidMovement(int mapID, int fromX, int fromY, int toX, int toY) {
    // Verifica se as posições são válidas
    if (!isValidPosition(mapID, fromX, fromY) || !isValidPosition(mapID, toX, toY)) {
        return false;
    }
    
    auto mapIt = m_maps.find(mapID);
    if (mapIt == m_maps.end()) {
        return false;
    }
    
    // Verifica se a célula de destino permite movimento
    int toCellIndex = getCellIndex(toX, toY, mapIt->second.width);
    if (!mapIt->second.cells[toCellIndex].allowMovement) {
        return false;
    }
    
    // Verifica colisão com outras entidades
    // [Implementação específica]
    
    return true;
}

std::vector<uint32_t> World::getNearbyPlayers(int mapID, int x, int y, int radius) {
    std::vector<uint32_t> result;
    
    if (!isValidPosition(mapID, x, y)) {
        return result;
    }
    
    std::lock_guard<std::mutex> playersLock(m_playersMutex);
    
    for (const auto& pair : m_players) {
        STRUCT_MOB* mob = pair.second->getMob();
        if (mob && mob->Area == mapID) {
            int distance = getDistance(x, y, mob->PosX, mob->PosY);
            if (distance <= radius) {
                result.push_back(pair.first);
            }
        }
    }
    
    return result;
}

std::vector<uint32_t> World::getNearbyMobs(int mapID, int x, int y, int radius) {
    std::vector<uint32_t> result;
    
    if (!isValidPosition(mapID, x, y)) {
        return result;
    }
    
    std::lock_guard<std::mutex> mobsLock(m_mobsMutex);
    
    for (const auto& pair : m_mobs) {
        const Mob& mob = pair.second;
        if (mob.mapID == mapID) {
            int distance = getDistance(x, y, mob.x, mob.y);
            if (distance <= radius) {
                result.push_back(pair.first);
            }
        }
    }
    
    return result;
}

std::vector<uint32_t> World::getNearbyEntities(int mapID, int x, int y, int radius) {
    std::vector<uint32_t> result;
    
    // Obtém jogadores próximos
    auto players = getNearbyPlayers(mapID, x, y, radius);
    result.insert(result.end(), players.begin(), players.end());
    
    // Obtém mobs próximos
    auto mobs = getNearbyMobs(mapID, x, y, radius);
    result.insert(result.end(), mobs.begin(), mobs.end());
    
    return result;
}

bool World::isSafeZone(int mapID, int x, int y) {
    // Verifica se a posição é válida
    if (!isValidPosition(mapID, x, y)) {
        return false;
    }
    
    auto mapIt = m_maps.find(mapID);
    if (mapIt == m_maps.end()) {
        return false;
    }
    
    int cellIndex = getCellIndex(x, y, mapIt->second.width);
    return mapIt->second.cells[cellIndex].type == CellType::SAFEZONE;
}

bool World::mapExists(int mapID) const {
    return m_maps.find(mapID) != m_maps.end();
}

void World::broadcastMessage(const std::string& message, int mapID) {
    // Criar pacote de mensagem
    // [Implementação específica]
    
    std::lock_guard<std::mutex> lock(m_playersMutex);
    
    for (const auto& pair : m_players) {
        auto player = pair.second;
        
        // Verifica se o jogador está no mapa especificado (ou todos os mapas se mapID for 0)
        if (player && player->isConnected() && 
            (mapID == 0 || player->getMob()->Area == mapID)) {
            
            // Envia a mensagem
            // [Implementação específica]
        }
    }
}

void World::broadcastPacket(const MSG_STANDARD* packet, int mapID) {
    if (!packet) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_playersMutex);
    
    for (const auto& pair : m_players) {
        auto player = pair.second;
        
        // Verifica se o jogador está no mapa especificado (ou todos os mapas se mapID for 0)
        if (player && player->isConnected() && 
            (mapID == 0 || player->getMob()->Area == mapID)) {
            
            // Envia o pacote
            player->sendPacket(packet);
        }
    }
}

uint32_t World::createNPC(int mobID, int mapID, int x, int y) {
    // Verifica se a posição é válida
    if (!isValidPosition(mapID, x, y)) {
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(m_mobsMutex);
    
    // Gera um novo ID para a entidade
    uint32_t entityID = m_nextEntityID++;
    
    // Cria o mob
    Mob mob;
    mob.entityID = entityID;
    mob.mobID = mobID;
    mob.mapID = mapID;
    mob.x = x;
    mob.y = y;
    mob.direction = 0;
    
    // [Implementação específica para configurar atributos do NPC]
    
    // Adiciona o mob à lista
    m_mobs[entityID] = mob;
    
    // Atualiza a célula do /**
 * updateEntityCell
 * 
 * Implementa a funcionalidade updateEntityCell conforme especificação original.
 * @param entityID Parâmetro entityID
 * @param mapID Parâmetro mapID
 * @param 0 Parâmetro 0
 * @param 0 Parâmetro 0
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna mapa
 */
 mapa
    updateEntityCell(entityID, mapID, 0, 0, x, y);
    
    return entityID;
}

std::vector<uint32_t> World::createMobs(int mobID, int mapID, int x, int y, int count) {
    std::vector<uint32_t> result;
    
    for (int i = 0; i < count; i++) {
        // Gera uma posição próxima ao ponto especificado
        int offsetX = (i % 5) - 2; // -2, -1, 0, 1, 2
        int offsetY = (i / 5) - 2; // -2, -1, 0, 1, 2
        
        int newX = x + offsetX;
        int newY = y + offsetY;
        
        // Cria o mob
        uint32_t entityID = createNPC(mobID, mapID, newX, newY);
        if (entityID > 0) {
            result.push_back(entityID);
        }
    }
    
    return result;
}

uint32_t World::dropItem(const STRUCT_ITEM& item, int mapID, int x, int y, uint32_t ownerID, int duration) {
    // Verifica se a posição é válida
    if (!isValidPosition(mapID, x, y)) {
        return 0;
    }
    
    // Verifica se o item é vá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna lido
 */
lido
    if(item.sIndex <= 0){
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(m_itemsMutex);
    
    // Gera um novo ID para o item
    uint32_t itemID = m_nextItemID++;
    
    // Cria o item
    DroppedItem droppedItem;
    droppedItem.itemID = itemID;
    droppedItem.item = item;
    droppedItem.mapID = mapID;
    droppedItem.x = x;
    droppedItem.y = y;
    droppedItem.ownerID = ownerID;
    
    // Calcula o tempo de expiração
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    droppedItem.dropTime = now;
    
    // Se a duração for 0, usa o valor padrão (2 minutos)
    if (duration <= 0) {
        duration = DROP_DECAY_TIME;
    }
    
    droppedItem.expireTime = now + duration;
    
    // Adiciona o item à lista
    m_droppedItems[itemID] = droppedItem;
    
    return itemID;
}

bool World::removeDroppedItem(uint32_t itemID) {
    std::lock_guard<std::mutex> lock(m_itemsMutex);
    
    auto it = m_droppedItems.find(itemID);
    if (it == m_droppedItems.end()) {
        return false;
    }
    
    m_droppedItems.erase(it);
    return true;
}

std::string World::getName() const {
    return m_name;
}

int World::getPlayerCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_playersMutex));
    return m_players.size();
}

int World::getMobCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mobsMutex));
    return m_mobs.size();
}

int World::getDroppedItemCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_itemsMutex));
    return m_droppedItems.size();
}

int World::getDistance(int x1, int y1, int x2, int y2) {
    // Calcula a distância euclidiana
    double dx = x2 - x1;
    double dy = y2 - y1;
    return static_cast<int>(std::sqrt(dx*dx + dy*dy));
}

void World::updateMobs(float deltaTime) {
    std::lock_guard<std::mutex> lock(m_mobsMutex);
    
    for (auto& pair : m_mobs) {
        Mob& mob = pair.second;
        
        // Atualiza os mobs conforme a lógica de cada um
        // [Implementação específica]
    }
}

void World::updateDroppedItems(float deltaTime) {
    std::lock_guard<std::mutex> lock(m_itemsMutex);
    
    // Obtém o timestamp atual
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Lista de itens a remover
    std::vector<uint32_t> itemsToRemove;
    
    for (const auto& pair : m_droppedItems) {
        const DroppedItem& item = pair.second;
        
        // Verifica se o item /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param now Parâmetro now
 * @return Retorna expirou
 */
 expirou
        if(item.expireTime <= now){
            itemsToRemove.push_back(item.itemID);
        }
    }
    
    // Remove os itens /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param itemsToRemove Parâmetro itemsToRemove
 * @return Retorna expirados
 */
 expirados
    for(uint32_t itemID : itemsToRemove){
        m_droppedItems.erase(itemID);
    }
}

bool World::checkCollision(int mapID, int x1, int y1, int x2, int y2) {
    // [Implementação específica para verificar colisões]
    
    return false;
}

void World::processRespawns(float deltaTime) {
    // Obtém o timestamp atual
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    for (auto& spawn : m_spawnPoints) {
        // Verifica se é hora de fazer /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1000 Parâmetro 1000
 * @return Retorna respawn
 */
 respawn
        if(now - spawn.lastSpawnTime >= spawn.respawnTime * 1000){
            // Conta quantos mobs deste tipo já existem nesta área
            int existingCount = 0;
            
            {
                std::lock_guard<std::mutex> lock(m_mobsMutex);
                
                for (const auto& pair : m_mobs) {
                    const Mob& mob = pair.second;
                    if (mob.mobID == spawn.mobID && mob.mapID == spawn.mapID) {
                        existingCount++;
                    }
                }
            }
            
            // Se há menos mobs do que o limite, cria novos
            int toCreate = spawn.count - existingCount;
            if (toCreate > 0) {
                createMobs(spawn.mobID, spawn.mapID, spawn.x, spawn.y, toCreate);
            }
            
            // Atualiza o timestamp do último spawn
            spawn.lastSpawnTime = now;
        }
    }
}

void World::updateWorldEvents(float deltaTime) {
    // [Implementação específica para eventos do mundo]
}

int World::getCellIndex(int x, int y, int mapWidth) const {
    return y * mapWidth + x;
}

void World::updateEntityCell(uint32_t entityID, int mapID, int oldX, int oldY, int newX, int newY) {
    auto mapIt = m_maps.find(mapID);
    if (mapIt == m_maps.end()) {
        return;
    }
    
    // Remove a entidade da célula antiga
    if (isValidPosition(mapID, oldX, oldY)) {
        int oldCellIndex = getCellIndex(oldX, oldY, mapIt->second.width);
        auto& oldCell = mapIt->second.cells[oldCellIndex];
        
        oldCell.entityIDs.erase(
            std::remove(oldCell.entityIDs.begin(), oldCell.entityIDs.end(), entityID),
            oldCell.entityIDs.end()
        );
    }
    
    // Adiciona a entidade à nova célula
    if (isValidPosition(mapID, newX, newY)) {
        int newCellIndex = getCellIndex(newX, newY, mapIt->second.width);
        auto& newCell = mapIt->second.cells[newCellIndex];
        
        // Verifica se a entidade já está na célula
        if (std::find(newCell.entityIDs.begin(), newCell.entityIDs.end(), entityID) == newCell.entityIDs.end()) {
            newCell.entityIDs.push_back(entityID);
        }
    }
}

} // namespace tmsrv
} // namespace wydbr