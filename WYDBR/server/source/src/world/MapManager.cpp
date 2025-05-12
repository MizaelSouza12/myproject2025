/**
 * @file MapManager.cpp
 * @brief Implementação do sistema de gerenciamento de mapas
 * 
 * Este arquivo implementa a classe de gerenciamento de mapas do servidor WYD,
 * mantendo compatibilidade total com o formato .wys original do cliente,
 * enquanto adiciona funcionalidades avançadas para gestão mais eficiente.
 */

#include "../../include/world/MapManager.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <stack>
#include <iomanip>
#include <sys/stat.h>
#include <sstream>

namespace wydbr {

/**
 * @brief Implementação da classe MapInfo
 */

MapInfo::MapInfo(uint16_t mapId, const std::string& name)
    : mapId_(mapId), name_(name), width_(4096), height_(4096),
      attribute_(MAP_ATTR_NORMAL), weatherType_(WEATHER_CLEAR),
      weatherIntensity_(0) {
}

MapInfo::~MapInfo() {
}

bool MapInfo::LoadFromWysFile(const std::string& filePath) {
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo WYS: " << filePath << std::endl;
        return false;
    }
    
    // Obter tamanho do arquivo
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Verificar tamanho mínimo
    if (fileSize < sizeof(WysHeader)) {
        std::cerr << "Arquivo WYS inválido (tamanho menor que o cabeçalho): " << filePath << std::endl;
        return false;
    }
    
    // Ler todo o arquivo para um buffer
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();
    
    // Verificar assinatura
    const WysHeader* header = reinterpret_cast<const WysHeader*>(buffer.data());
    if (std::strncmp(header->signature, "WYS ", 4) != 0) {
        std::cerr << "Arquivo WYS inválido (assinatura incorreta): " << filePath << std::endl;
        return false;
    }
    
    // Atualizar informações
    width_ = header->width;
    height_ = header->height;
    attribute_ = header->attribute;
    weatherType_ = header->weatherType;
    weatherIntensity_ = header->weatherIntensity;
    
    // Inicializar células
    if (!InitializeCells(width_, height_, CELL_ATTR_EMPTY)) {
        std::cerr << "Falha ao inicializar células do mapa: " << filePath << std::endl;
        return false;
    }
    
    // Ler estruturas do arquivo
    if (!ReadWysStructures(buffer.data(), fileSize)) {
        std::cerr << "Falha ao ler estruturas do mapa: " << filePath << std::endl;
        return false;
    }
    
    // Inicializar /**
 * InitializeSegments
 * 
 * Implementa a funcionalidade InitializeSegments conforme especificação original.
 * @return Retorna segmentos
 */
 segmentos
    InitializeSegments();
    
    std::cout << "Mapa carregado com sucesso: " << name_ << " (" << mapId_ << ")" << std::endl;
    return true;
}

bool MapInfo::SaveToWysFile(const std::string& filePath) {
    // Calcular tamanho necessário para o arquivo
    size_t fileSize = sizeof(WysHeader);
    fileSize += cells_.size() * sizeof(MapCell);
    fileSize += terrains_.size() * sizeof(MapTerrain);
    
    // Adicionar tamanho para /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param routes_ Parâmetro routes_
 * @return Retorna rotas
 */
 rotas
    for(const auto& route : routes_){
        fileSize += sizeof(uint16_t) * 2 + sizeof(uint8_t) * 2; // routeId, numPoints, loopType, padding
        fileSize += route.points.size() * sizeof(MapRoutePoint);
    }
    
    fileSize += teleports_.size() * sizeof(MapTeleport);
    fileSize += npcs_.size() * sizeof(MapNPC);
    
    // Adicionar tamanho para /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param events_ Parâmetro events_
 * @return Retorna eventos
 */
 eventos
    for(const auto& event : events_){
        fileSize += sizeof(MapEvent) - sizeof(std::string); // Tamanho fixo da estrutura
        fileSize += event.script.size() + 1; // Tamanho do script + terminador nulo
    }
    
    fileSize += areas_.size() * sizeof(MapArea);
    
    // Criar buffer para o arquivo
    std::vector<uint8_t> buffer(fileSize, 0);
    
    // Preencher cabeçalho
    WysHeader* header = reinterpret_cast<WysHeader*>(buffer.data());
    std::strncpy(header->signature, "WYS ", 4);
    header->version = 0x100;
    header->width = width_;
    header->height = height_;
    header->attribute = attribute_;
    header->numTerrains = static_cast<uint16_t>(terrains_.size());
    header->numRoutes = static_cast<uint16_t>(routes_.size());
    header->numTeleports = static_cast<uint16_t>(teleports_.size());
    header->numNPCs = static_cast<uint16_t>(npcs_.size());
    header->numEvents = static_cast<uint16_t>(events_.size());
    header->numAreas = static_cast<uint16_t>(areas_.size());
    header->weatherType = weatherType_;
    header->weatherIntensity = weatherIntensity_;
    std::memset(header->reserved, 0, sizeof(header->reserved));
    
    // Escrever estruturas no buffer
    size_t offset = sizeof(WysHeader);
    
    // Células
    std::memcpy(buffer.data() + offset, cells_.data(), cells_.size() * sizeof(MapCell));
    offset += cells_.size() * sizeof(MapCell);
    
    // Terrenos
    std::memcpy(buffer.data() + offset, terrains_.data(), terrains_.size() * sizeof(MapTerrain));
    offset += terrains_.size() * sizeof(MapTerrain);
    
    // /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param routes_ Parâmetro routes_
 * @return Retorna Rotas
 */
 Rotas
    for(const auto& route : routes_){
        // Escrever ID da rota
        uint16_t routeId = route.routeId;
        std::memcpy(buffer.data() + offset, &routeId, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Tipo de loop
        uint8_t loopType = route.loopType;
        std::memcpy(buffer.data() + offset, &loopType, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        
        // Número de pontos
        uint8_t numPoints = static_cast<uint8_t>(route.points.size());
        std::memcpy(buffer.data() + offset, &numPoints, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        
        // Padding
        uint8_t padding = 0;
        std::memcpy(buffer.data() + offset, &padding, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        
        // Pontos da rota
        std::memcpy(buffer.data() + offset, route.points.data(), route.points.size() * sizeof(MapRoutePoint));
        offset += route.points.size() * sizeof(MapRoutePoint);
    }
    
    // Teleportes
    std::memcpy(buffer.data() + offset, teleports_.data(), teleports_.size() * sizeof(MapTeleport));
    offset += teleports_.size() * sizeof(MapTeleport);
    
    // NPCs
    std::memcpy(buffer.data() + offset, npcs_.data(), npcs_.size() * sizeof(MapNPC));
    offset += npcs_.size() * sizeof(MapNPC);
    
    // /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param events_ Parâmetro events_
 * @return Retorna Eventos
 */
 Eventos
    for(const auto& event : events_){
        // Escrever ID do evento
        uint16_t eventId = event.eventId;
        std::memcpy(buffer.data() + offset, &eventId, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Tipo de evento
        uint16_t eventType = event.type;
        std::memcpy(buffer.data() + offset, &eventType, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Posição X
        uint16_t x = event.x;
        std::memcpy(buffer.data() + offset, &x, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Posição Y
        uint16_t y = event.y;
        std::memcpy(buffer.data() + offset, &y, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Raio
        uint16_t radius = event.radius;
        std::memcpy(buffer.data() + offset, &radius, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Tempo de gatilho
        uint32_t triggerTime = event.triggerTime;
        std::memcpy(buffer.data() + offset, &triggerTime, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Tempo de permanência
        uint32_t keepTime = event.keepTime;
        std::memcpy(buffer.data() + offset, &keepTime, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Parâmetros
        uint32_t parameter1 = event.parameter1;
        std::memcpy(buffer.data() + offset, &parameter1, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        uint32_t parameter2 = event.parameter2;
        std::memcpy(buffer.data() + offset, &parameter2, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        uint32_t parameter3 = event.parameter3;
        std::memcpy(buffer.data() + offset, &parameter3, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Script
        std::memcpy(buffer.data() + offset, event.script.c_str(), event.script.size() + 1);
        offset += event.script.size() + 1;
    }
    
    // Áreas
    std::memcpy(buffer.data() + offset, areas_.data(), areas_.size() * sizeof(MapArea));
    offset += areas_.size() * sizeof(MapArea);
    
    // Escrever arquivo
    /**
 * outFile
 * 
 * Implementa a funcionalidade outFile conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ofstream
 */

    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Erro ao criar arquivo WYS: " << filePath << std::endl;
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(buffer.data()), fileSize);
    outFile.close();
    
    std::cout << "Mapa salvo com sucesso: " << name_ << " (" << mapId_ << ")" << std::endl;
    return true;
}

bool MapInfo::InitializeCells(uint16_t width, uint16_t height, uint8_t defaultAttribute) {
    // Verificar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 8192 Parâmetro 8192
 * @return Retorna limites
 */
 limites
    if(width == 0 || height == 0 || width > 8192 || height > 8192){
        std::cerr << "Dimensões de mapa inválidas: " << width << "x" << height << std::endl;
        return false;
    }
    
    // Redimensionar vetor de células
    cells_.resize(width * height);
    
    // Inicializar células com atributo padrã/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param cells_ Parâmetro cells_
 * @return Retorna o
 */
o
    for(auto& cell : cells_){
        cell.attribute = defaultAttribute;
        cell.height = 0;
        cell.eventId = 0;
        cell.oceanAttribute = 0;
    }
    
    // Atualizar dimensões
    width_ = width;
    height_ = height;
    
    return true;
}

uint8_t MapInfo::GetCellAttribute(uint16_t x, uint16_t y) const {
    if (!IsValidPosition(x, y)) {
        return CELL_ATTR_BLOCK;
    }
    
    size_t index = y * width_ + x;
    return cells_[index].attribute;
}

void MapInfo::SetCellAttribute(uint16_t x, uint16_t y, uint8_t attribute) {
    if (!IsValidPosition(x, y)) {
        return;
    }
    
    size_t index = y * width_ + x;
    cells_[index].attribute = attribute;
}

uint8_t MapInfo::GetCellHeight(uint16_t x, uint16_t y) const {
    if (!IsValidPosition(x, y)) {
        return 0;
    }
    
    // Verificar cache /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param heightCache_ Parâmetro heightCache_
 * @return Retorna primeiro
 */
 primeiro
    for(const auto& cache : heightCache_){
        if (cache.x == x && cache.y == y) {
            return cache.height;
        }
    }
    
    size_t index = y * width_ + x;
    uint8_t height = cells_[index].height;
    
    // Atualizar cache
    if (heightCache_.size() >= 100) {
        heightCache_.erase(heightCache_.begin());
    }
    
    HeightCache cache;
    cache.x = x;
    cache.y = y;
    cache.height = height;
    heightCache_.push_back(cache);
    
    return height;
}

void MapInfo::SetCellHeight(uint16_t x, uint16_t y, uint8_t height) {
    if (!IsValidPosition(x, y)) {
        return;
    }
    
    size_t index = y * width_ + x;
    cells_[index].height = height;
    
    // Atualizar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param heightCache_ Parâmetro heightCache_
 * @return Retorna cache
 */
 cache
    for(auto& cache : heightCache_){
        if (cache.x == x && cache.y == y) {
            cache.height = height;
            return;
        }
    }
    
    if (heightCache_.size() >= 100) {
        heightCache_.erase(heightCache_.begin());
    }
    
    HeightCache cache;
    cache.x = x;
    cache.y = y;
    cache.height = height;
    heightCache_.push_back(cache);
}

bool MapInfo::IsValidPosition(uint16_t x, uint16_t y) const {
    return x < width_ && y < height_;
}

bool MapInfo::CanMoveToPosition(uint16_t x, uint16_t y, bool isFlying) const {
    if (!IsValidPosition(x, y)) {
        return false;
    }
    
    uint8_t attribute = GetCellAttribute(x, y);
    
    // Se estiver voando, ignora bloqueios de movimento /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param isFlying Parâmetro isFlying
 * @return Retorna terrestre
 */
 terrestre
    if(isFlying){
        return !(attribute & CELL_ATTR_NOTFLY);
    }
    
    return !(attribute & (CELL_ATTR_BLOCK | CELL_ATTR_NOTMOVE));
}

bool MapInfo::AddTerrain(const MapTerrain& terrain) {
    // Validar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna terreno
 */
 terreno
    if(terrain.width == 0 || terrain.length == 0){
        return false;
    }
    
    // Adicionar terreno
    terrains_.push_back(terrain);
    
    return true;
}

bool MapInfo::AddRoute(const MapRoute& route) {
    // Validar rota
    if (route.points.empty()) {
        return false;
    }
    
    // Verificar se já existe rota com mesmo /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param routes_ Parâmetro routes_
 * @return Retorna ID
 */
 ID
    for(const auto& existingRoute : routes_){
        if (existingRoute.routeId == route.routeId) {
            return false;
        }
    }
    
    // Adicionar rota
    routes_.push_back(route);
    
    return true;
}

bool MapInfo::AddTeleport(const MapTeleport& teleport) {
    // Validar teleporte
    if (!IsValidPosition(teleport.srcX, teleport.srcY)) {
        return false;
    }
    
    // Adicionar teleporte
    teleports_.push_back(teleport);
    
    return true;
}

bool MapInfo::AddNPC(const MapNPC& npc) {
    // Validar NPC
    if (!IsValidPosition(npc.x, npc.y)) {
        return false;
    }
    
    // Adicionar NPC
    npcs_.push_back(npc);
    
    return true;
}

bool MapInfo::AddEvent(const MapEvent& event) {
    // Validar evento
    if (!IsValidPosition(event.x, event.y)) {
        return false;
    }
    
    // Verificar se já existe evento com mesmo /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param events_ Parâmetro events_
 * @return Retorna ID
 */
 ID
    for(const auto& existingEvent : events_){
        if (existingEvent.eventId == event.eventId) {
            return false;
        }
    }
    
    // Adicionar evento
    events_.push_back(event);
    
    return true;
}

bool MapInfo::AddArea(const MapArea& area) {
    // Validar área
    if (!IsValidPosition(area.x1, area.y1) || !IsValidPosition(area.x2, area.y2)) {
        return false;
    }
    
    // Verificar se já existe área com mesmo /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param areas_ Parâmetro areas_
 * @return Retorna ID
 */
 ID
    for(const auto& existingArea : areas_){
        if (existingArea.areaId == area.areaId) {
            return false;
        }
    }
    
    // Adicionar área
    areas_.push_back(area);
    
    return true;
}

std::vector<int> MapInfo::FindTerrainsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const {
    std::vector<int> result;
    
    // Garantir que x1 <= x2 e y1 <= y2
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    // Encontrar segmentos que contêm a área
    auto segments = GetSegmentsInArea(x1, y1, x2, y2);
    
    // Conjunto para evitar duplicatas
    std::set<int> uniqueIndices;
    
    // Adicionar terrenos de cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param segments Parâmetro segments
 * @return Retorna segmento
 */
 segmento
    for(const auto& segIdx : segments){
        int segX = segIdx.first;
        int segY = segIdx.second;
        
        if (segX >= 0 && segX < static_cast<int>(segments_.size()) &&
            segY >= 0 && segY < static_cast<int>(segments_[segX].size())) {
            
            const auto& terrainIndices = segments_[segX][segY].terrains;
            uniqueIndices.insert(terrainIndices.begin(), terrainIndices.end());
        }
    }
    
    // Verificar cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param uniqueIndices Parâmetro uniqueIndices
 * @return Retorna terreno
 */
 terreno
    for(int index : uniqueIndices){
        if (index < 0 || index >= static_cast<int>(terrains_.size())) {
            continue;
        }
        
        const auto& terrain = terrains_[index];
        
        // Verificar se o terreno está na área
        uint16_t terrainX1 = terrain.baseX;
        uint16_t terrainY1 = terrain.baseY;
        uint16_t terrainX2 = terrainX1 + terrain.width;
        uint16_t terrainY2 = terrainY1 + terrain.length;
        
        if (terrainX2 >= x1 && terrainX1 <= x2 && terrainY2 >= y1 && terrainY1 <= y2) {
            result.push_back(index);
        }
    }
    
    return result;
}

std::vector<int> MapInfo::FindTeleportsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const {
    std::vector<int> result;
    
    // Garantir que x1 <= x2 e y1 <= y2
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    // Encontrar segmentos que contêm a área
    auto segments = GetSegmentsInArea(x1, y1, x2, y2);
    
    // Conjunto para evitar duplicatas
    std::set<int> uniqueIndices;
    
    // Adicionar teleportes de cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param segments Parâmetro segments
 * @return Retorna segmento
 */
 segmento
    for(const auto& segIdx : segments){
        int segX = segIdx.first;
        int segY = segIdx.second;
        
        if (segX >= 0 && segX < static_cast<int>(segments_.size()) &&
            segY >= 0 && segY < static_cast<int>(segments_[segX].size())) {
            
            const auto& teleportIndices = segments_[segX][segY].teleports;
            uniqueIndices.insert(teleportIndices.begin(), teleportIndices.end());
        }
    }
    
    // Verificar cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param uniqueIndices Parâmetro uniqueIndices
 * @return Retorna teleporte
 */
 teleporte
    for(int index : uniqueIndices){
        if (index < 0 || index >= static_cast<int>(teleports_.size())) {
            continue;
        }
        
        const auto& teleport = teleports_[index];
        
        // Verificar se o teleporte está na área
        uint16_t teleportX = teleport.srcX;
        uint16_t teleportY = teleport.srcY;
        uint8_t range = teleport.srcRange;
        
        if (teleportX + range >= x1 && teleportX - range <= x2 && 
            teleportY + range >= y1 && teleportY - range <= y2) {
            result.push_back(index);
        }
    }
    
    return result;
}

std::vector<int> MapInfo::FindNPCsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const {
    std::vector<int> result;
    
    // Garantir que x1 <= x2 e y1 <= y2
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    // Encontrar segmentos que contêm a área
    auto segments = GetSegmentsInArea(x1, y1, x2, y2);
    
    // Conjunto para evitar duplicatas
    std::set<int> uniqueIndices;
    
    // Adicionar NPCs de cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param segments Parâmetro segments
 * @return Retorna segmento
 */
 segmento
    for(const auto& segIdx : segments){
        int segX = segIdx.first;
        int segY = segIdx.second;
        
        if (segX >= 0 && segX < static_cast<int>(segments_.size()) &&
            segY >= 0 && segY < static_cast<int>(segments_[segX].size())) {
            
            const auto& npcIndices = segments_[segX][segY].npcs;
            uniqueIndices.insert(npcIndices.begin(), npcIndices.end());
        }
    }
    
    // Verificar cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param uniqueIndices Parâmetro uniqueIndices
 * @return Retorna NPC
 */
 NPC
    for(int index : uniqueIndices){
        if (index < 0 || index >= static_cast<int>(npcs_.size())) {
            continue;
        }
        
        const auto& npc = npcs_[index];
        
        // Verificar se o NPC está na á/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param y2 Parâmetro y2
 * @return Retorna rea
 */
rea
        if(npc.x >= x1 && npc.x <= x2 && npc.y >= y1 && npc.y <= y2){
            result.push_back(index);
        }
    }
    
    return result;
}

std::vector<int> MapInfo::FindEventsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const {
    std::vector<int> result;
    
    // Garantir que x1 <= x2 e y1 <= y2
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    // Encontrar segmentos que contêm a área
    auto segments = GetSegmentsInArea(x1, y1, x2, y2);
    
    // Conjunto para evitar duplicatas
    std::set<int> uniqueIndices;
    
    // Adicionar eventos de cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param segments Parâmetro segments
 * @return Retorna segmento
 */
 segmento
    for(const auto& segIdx : segments){
        int segX = segIdx.first;
        int segY = segIdx.second;
        
        if (segX >= 0 && segX < static_cast<int>(segments_.size()) &&
            segY >= 0 && segY < static_cast<int>(segments_[segX].size())) {
            
            const auto& eventIndices = segments_[segX][segY].events;
            uniqueIndices.insert(eventIndices.begin(), eventIndices.end());
        }
    }
    
    // Verificar cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param uniqueIndices Parâmetro uniqueIndices
 * @return Retorna evento
 */
 evento
    for(int index : uniqueIndices){
        if (index < 0 || index >= static_cast<int>(events_.size())) {
            continue;
        }
        
        const auto& event = events_[index];
        
        // Verificar se o evento está na área
        uint16_t eventX = event.x;
        uint16_t eventY = event.y;
        uint16_t radius = event.radius;
        
        if (eventX + radius >= x1 && eventX - radius <= x2 && 
            eventY + radius >= y1 && eventY - radius <= y2) {
            result.push_back(index);
        }
    }
    
    return result;
}

std::vector<int> MapInfo::FindAreasInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const {
    std::vector<int> result;
    
    // Garantir que x1 <= x2 e y1 <= y2
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    // Encontrar segmentos que contêm a área
    auto segments = GetSegmentsInArea(x1, y1, x2, y2);
    
    // Conjunto para evitar duplicatas
    std::set<int> uniqueIndices;
    
    // Adicionar áreas de cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param segments Parâmetro segments
 * @return Retorna segmento
 */
 segmento
    for(const auto& segIdx : segments){
        int segX = segIdx.first;
        int segY = segIdx.second;
        
        if (segX >= 0 && segX < static_cast<int>(segments_.size()) &&
            segY >= 0 && segY < static_cast<int>(segments_[segX].size())) {
            
            const auto& areaIndices = segments_[segX][segY].areas;
            uniqueIndices.insert(areaIndices.begin(), areaIndices.end());
        }
    }
    
    // Verificar cada á/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param uniqueIndices Parâmetro uniqueIndices
 * @return Retorna rea
 */
rea
    for(int index : uniqueIndices){
        if (index < 0 || index >= static_cast<int>(areas_.size())) {
            continue;
        }
        
        const auto& area = areas_[index];
        
        // Verificar se a área se sobrepõe à área de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param y2 Parâmetro y2
 * @return Retorna pesquisa
 */
 pesquisa
        if(area.x2 >= x1 && area.x1 <= x2 && area.y2 >= y1 && area.y1 <= y2){
            result.push_back(index);
        }
    }
    
    return result;
}

const MapRoute* MapInfo::FindRouteById(uint16_t routeId) const {
    for (const auto& route : routes_) {
        if (route.routeId == routeId) {
            return &route;
        }
    }
    
    return nullptr;
}

int MapInfo::FindTeleportByPosition(uint16_t x, uint16_t y) const {
    for (size_t i = 0; i < teleports_.size(); ++i) {
        const auto& teleport = teleports_[i];
        
        // Verificar se a posição está dentro do alcance do teleporte
        uint16_t dx = std::abs(static_cast<int>(teleport.srcX) - static_cast<int>(x));
        uint16_t dy = std::abs(static_cast<int>(teleport.srcY) - static_cast<int>(y));
        
        if (dx <= teleport.srcRange && dy <= teleport.srcRange) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

void MapInfo::UpdateWeather(uint8_t type, uint8_t intensity) {
    weatherType_ = type;
    weatherIntensity_ = intensity;
}

std::vector<std::pair<uint16_t, uint16_t>> MapInfo::CalculatePath(
    uint16_t startX, uint16_t startY, 
    uint16_t endX, uint16_t endY, 
    uint16_t maxDistance, 
    bool isFlying) const {
    
    std::vector<std::pair<uint16_t, uint16_t>> path;
    
    // Verificar posições
    if (!IsValidPosition(startX, startY) || !IsValidPosition(endX, endY)) {
        return path;
    }
    
    // Se a posição de destino for bloqueada, retornar caminho vazio
    if (!CanMoveToPosition(endX, endY, isFlying)) {
        return path;
    }
    
    // Se a distância em linha reta exceder o máximo, retornar caminho vazio
    uint16_t distance = static_cast<uint16_t>(
        std::sqrt(std::pow(static_cast<int>(endX) - static_cast<int>(startX), 2) + 
                 std::pow(static_cast<int>(endY) - static_cast<int>(startY), 2)));
    
    if (maxDistance > 0 && distance > maxDistance) {
        return path;
    }
    
    // Para voo, retornar apenas os pontos inicial e /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param isFlying Parâmetro isFlying
 * @return Retorna final
 */
 final
    if(isFlying){
        path.emplace_back(startX, startY);
        path.emplace_back(endX, endY);
        return path;
    }
    
    // Implementação do algoritmo A* para encontrar caminho
    struct Node {
        uint16_t x, y;
        int g; // Custo do início até este nó
        int h; // Heurística (distância estimada até o destino)
        int f; // Função de avaliação f = g + h
        Node* parent;
        
        Node(uint16_t _x, uint16_t _y, int _g, int _h, Node* _parent)
            : x(_x), y(_y), g(_g), h(_h), f(_g + _h), parent(_parent) {}
        
        // Operador de comparação para a fila de prioridade
        bool operator<(const Node& other) const {
            return f > other.f; // Inversão para obter min-heap
        }
    };
    
    // Direções de movimento (8 direções)
    const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    
    // Conjuntos para o algoritmo A*
    std::priority_queue<Node> openSet;
    std::set<std::pair<uint16_t, uint16_t>> closedSet;
    std::map<std::pair<uint16_t, uint16_t>, Node*> allNodes;
    
    // Nó inicial
    Node* startNode = new Node(startX, startY, 0,
        std::abs(static_cast<int>(endX) - static_cast<int>(startX)) + 
        std::abs(static_cast<int>(endY) - static_cast<int>(startY)), nullptr);
    
    openSet.push(*startNode);
    allNodes[{startX, startY}] = startNode;
    
    bool foundPath = false;
    Node* currentNode = nullptr;
    
    // Loop principal do A*
    while (!openSet.empty()) {
        // Obter nó com menor valor de f
        Node current = openSet.top();
        openSet.pop();
        
        uint16_t x = current.x;
        uint16_t y = current.y;
        
        // Se já processamos esta posição, continuar
        if (closedSet.find({x, y}) != closedSet.end()) {
            continue;
        }
        
        // Obter o nó atual do mapa de todos os nós
        currentNode = allNodes[{x, y}];
        
        // Adicionar ao conjunto fechado
        closedSet.insert({x, y});
        
        // Verificar se chegamos ao /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param endY Parâmetro endY
 * @return Retorna destino
 */
 destino
        if(x == endX && y == endY){
            foundPath = true;
            break;
        }
        
        // Explorar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna vizinhos
 */
 vizinhos
        for(int i = 0; i < 8; ++i){
            uint16_t newX = static_cast<uint16_t>(x + dx[i]);
            uint16_t newY = static_cast<uint16_t>(y + dy[i]);
            
            // Verificar se a posição é válida e pode se mover para lá
            if (!IsValidPosition(newX, newY) || !CanMoveToPosition(newX, newY, false)) {
                continue;
            }
            
            // Verificar se já está no conjunto fechado
            if (closedSet.find({newX, newY}) != closedSet.end()) {
                continue;
            }
            
            // Custo para o vizinho (diagonais custam mais)
            int moveCost = (i % 2 == 0) ? 14 : 10; // 14 para diagonais, 10 para cardeais
            int g = currentNode->g + moveCost;
            
            // Verificar se é uma posição nova ou se encontramos um caminho melhor
            auto nodeIt = allNodes.find({newX, newY});
            if (nodeIt == allNodes.end() || g < nodeIt->second->g) {
                // Heurística de Manhattan
                int h = std::abs(static_cast<int>(endX) - static_cast<int>(newX)) + 
                         std::abs(static_cast<int>(endY) - static_cast<int>(newY));
                
                Node* newNode = /**
 * Node
 * 
 * Implementa a funcionalidade Node conforme especificação original.
 * @param newX Parâmetro newX
 * @param newY Parâmetro newY
 * @param g Parâmetro g
 * @param h Parâmetro h
 * @param currentNode Parâmetro currentNode
 * @return Retorna new
 */
 new Node(newX, newY, g, h, currentNode);
                
                if (nodeIt != allNodes.end()) {
                    delete nodeIt->second;
                }
                
                allNodes[{newX, newY}] = newNode;
                openSet.push(*newNode);
            }
        }
        
        // Limitar o número de iterações para evitar caminhos muito longos
        if (closedSet.size() > 1000) {
            break;
        }
    }
    
    // Reconstruir caminho se /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param currentNode Parâmetro currentNode
 * @return Retorna encontrado
 */
 encontrado
    if(foundPath && currentNode){
        // Traçar caminho do destino para o início
        std::stack<std::pair<uint16_t, uint16_t>> tempPath;
        
        while (currentNode) {
            tempPath.push({currentNode->x, currentNode->y});
            currentNode = currentNode->parent;
        }
        
        // Inverter caminho para ter do início para o destino
        while (!tempPath.empty()) {
            path.push_back(tempPath.top());
            tempPath.pop();
        }
    }
    
    // Limpar memó/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param allNodes Parâmetro allNodes
 * @return Retorna ria
 */
ria
    for(auto& node : allNodes){
        delete node.second;
    }
    
    return path;
}

void MapInfo::PrintInfo(bool detailed) const {
    std::cout << "====== Informações do Mapa ======" << std::endl;
    std::cout << "ID: " << mapId_ << std::endl;
    std::cout << "Nome: " << name_ << std::endl;
    std::cout << "Dimensões: " << width_ << "x" << height_ << std::endl;
    std::cout << "Atributos: 0x" << std::hex << attribute_ << std::dec << std::endl;
    std::cout << "Clima: " << static_cast<int>(weatherType_) << " (intensidade: " << static_cast<int>(weatherIntensity_) << ")" << std::endl;
    std::cout << "Terrenos: " << terrains_.size() << std::endl;
    std::cout << "Rotas: " << routes_.size() << std::endl;
    std::cout << "Teleportes: " << teleports_.size() << std::endl;
    std::cout << "NPCs: " << npcs_.size() << std::endl;
    std::cout << "Eventos: " << events_.size() << std::endl;
    std::cout << "Áreas: " << areas_.size() << std::endl;
    
    if (detailed) {
        // Exibir detalhes dos terrenos
        if (!terrains_.empty()) {
            std::cout << "\n--- Terrenos ---" << std::endl;
            for (size_t i = 0; i < terrains_.size(); ++i) {
                const auto& terrain = terrains_[i];
                std::cout << "Terreno " << i << ": " 
                          << "Pos(" << terrain.baseX << "," << terrain.baseY << ") "
                          << "Dim(" << terrain.width << "x" << terrain.length << ") "
                          << "Tipo=" << static_cast<int>(terrain.type) << " "
                          << "Altura=" << static_cast<int>(terrain.height) << std::endl;
            }
        }
        
        // Exibir detalhes das rotas
        if (!routes_.empty()) {
            std::cout << "\n--- Rotas ---" << std::endl;
            for (size_t i = 0; i < routes_.size(); ++i) {
                const auto& route = routes_[i];
                std::cout << "Rota " << route.routeId << ": "
                          << "Loop=" << static_cast<int>(route.loopType) << " "
                          << "Pontos=" << route.points.size() << std::endl;
                
                if (route.points.size() <= 5 || detailed) {
                    for (size_t j = 0; j < route.points.size(); ++j) {
                        const auto& point = route.points[j];
                        std::cout << "  Ponto " << j << ": "
                                  << "Pos(" << point.x << "," << point.y << ") "
                                  << "Vel=" << static_cast<int>(point.velocity) << " "
                                  << "Esp=" << point.wait << " "
                                  << "Ação=" << static_cast<int>(point.action) << std::endl;
                    }
                }
            }
        }
        
        // Exibir detalhes dos teleportes
        if (!teleports_.empty()) {
            std::cout << "\n--- Teleportes ---" << std::endl;
            for (size_t i = 0; i < teleports_.size(); ++i) {
                const auto& teleport = teleports_[i];
                std::cout << "Teleporte " << i << ": "
                          << "Origem(" << teleport.srcX << "," << teleport.srcY << ") "
                          << "Destino(" << teleport.dstX << "," << teleport.dstY << ") "
                          << "Mapa=" << teleport.dstMap << " "
                          << "Alcance=" << static_cast<int>(teleport.srcRange) << " "
                          << "Condição=" << static_cast<int>(teleport.condition) << " "
                          << "Nível=" << static_cast<int>(teleport.level) << " "
                          << "Custo=" << teleport.cost << std::endl;
            }
        }
        
        // Exibir detalhes dos NPCs
        if (!npcs_.empty()) {
            std::cout << "\n--- NPCs ---" << std::endl;
            for (size_t i = 0; i < npcs_.size(); ++i) {
                const auto& npc = npcs_[i];
                std::cout << "NPC " << i << ": "
                          << "ID=" << npc.npcId << " "
                          << "Pos(" << npc.x << "," << npc.y << ") "
                          << "Rot=" << static_cast<int>(npc.rotation) << " "
                          << "Evento=" << static_cast<int>(npc.eventId) << " "
                          << "Instância=" << npc.instanceId << std::endl;
            }
        }
        
        // Exibir detalhes dos eventos
        if (!events_.empty()) {
            std::cout << "\n--- Eventos ---" << std::endl;
            for (size_t i = 0; i < events_.size(); ++i) {
                const auto& event = events_[i];
                std::cout << "Evento " << event.eventId << ": "
                          << "Tipo=" << event.type << " "
                          << "Pos(" << event.x << "," << event.y << ") "
                          << "Raio=" << event.radius << " "
                          << "Gatilho=" << event.triggerTime << " "
                          << "Duração=" << event.keepTime << std::endl;
                
                if (!event.script.empty()) {
                    std::cout << "  Script: " << (event.script.size() > 50 ? event.script.substr(0, 50) + "..." : event.script) << std::endl;
                }
            }
        }
        
        // Exibir detalhes das áreas
        if (!areas_.empty()) {
            std::cout << "\n--- Áreas ---" << std::endl;
            for (size_t i = 0; i < areas_.size(); ++i) {
                const auto& area = areas_[i];
                std::cout << "Área " << area.areaId << ": "
                          << "Tipo=" << area.type << " "
                          << "Pos(" << area.x1 << "," << area.y1 << " - " << area.x2 << "," << area.y2 << ") "
                          << "Params=" << area.parameter1 << "," << area.parameter2 << std::endl;
            }
        }
    }
    
    std::cout << "=================================" << std::endl;
}

void MapInfo::InitializeSegments() {
    const int SEGMENT_SIZE = 256; // Tamanho de cada segmento em células
    
    // Calcular número de segmentos
    int numSegmentsX = (width_ + SEGMENT_SIZE - 1) / SEGMENT_SIZE;
    int numSegmentsY = (height_ + SEGMENT_SIZE - 1) / SEGMENT_SIZE;
    
    // Inicializar matriz de segmentos
    segments_.resize(numSegmentsX);
    for (int i = 0; i < numSegmentsX; ++i) {
        segments_[i].resize(numSegmentsY);
    }
    
    // Adicionar terrenos aos /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param terrains_.size( Parâmetro terrains_.size(
 * @return Retorna segmentos
 */
 segmentos
    for(size_t i = 0; i < terrains_.size(); ++i) {
        const auto& terrain = terrains_[i];
        
        int startSegX = terrain.baseX / SEGMENT_SIZE;
        int startSegY = terrain.baseY / SEGMENT_SIZE;
        int endSegX = (terrain.baseX + terrain.width) / SEGMENT_SIZE;
        int endSegY = (terrain.baseY + terrain.length) / SEGMENT_SIZE;
        
        for (int segX = startSegX; segX <= endSegX && segX < numSegmentsX; ++segX) {
            for (int segY = startSegY; segY <= endSegY && segY < numSegmentsY; ++segY) {
                if (segX >= 0 && segY >= 0) {
                    segments_[segX][segY].terrains.insert(static_cast<int>(i));
                }
            }
        }
    }
    
    // Adicionar teleportes aos /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param teleports_.size( Parâmetro teleports_.size(
 * @return Retorna segmentos
 */
 segmentos
    for(size_t i = 0; i < teleports_.size(); ++i) {
        const auto& teleport = teleports_[i];
        
        int segX = teleport.srcX / SEGMENT_SIZE;
        int segY = teleport.srcY / SEGMENT_SIZE;
        
        if (segX >= 0 && segX < numSegmentsX && segY >= 0 && segY < numSegmentsY) {
            segments_[segX][segY].teleports.insert(static_cast<int>(i));
        }
    }
    
    // Adicionar NPCs aos /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param npcs_.size( Parâmetro npcs_.size(
 * @return Retorna segmentos
 */
 segmentos
    for(size_t i = 0; i < npcs_.size(); ++i) {
        const auto& npc = npcs_[i];
        
        int segX = npc.x / SEGMENT_SIZE;
        int segY = npc.y / SEGMENT_SIZE;
        
        if (segX >= 0 && segX < numSegmentsX && segY >= 0 && segY < numSegmentsY) {
            segments_[segX][segY].npcs.insert(static_cast<int>(i));
        }
    }
    
    // Adicionar eventos aos /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param events_.size( Parâmetro events_.size(
 * @return Retorna segmentos
 */
 segmentos
    for(size_t i = 0; i < events_.size(); ++i) {
        const auto& event = events_[i];
        
        int segX = event.x / SEGMENT_SIZE;
        int segY = event.y / SEGMENT_SIZE;
        
        if (segX >= 0 && segX < numSegmentsX && segY >= 0 && segY < numSegmentsY) {
            segments_[segX][segY].events.insert(static_cast<int>(i));
        }
    }
    
    // Adicionar áreas aos /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param areas_.size( Parâmetro areas_.size(
 * @return Retorna segmentos
 */
 segmentos
    for(size_t i = 0; i < areas_.size(); ++i) {
        const auto& area = areas_[i];
        
        int startSegX = area.x1 / SEGMENT_SIZE;
        int startSegY = area.y1 / SEGMENT_SIZE;
        int endSegX = area.x2 / SEGMENT_SIZE;
        int endSegY = area.y2 / SEGMENT_SIZE;
        
        for (int segX = startSegX; segX <= endSegX && segX < numSegmentsX; ++segX) {
            for (int segY = startSegY; segY <= endSegY && segY < numSegmentsY; ++segY) {
                if (segX >= 0 && segY >= 0) {
                    segments_[segX][segY].areas.insert(static_cast<int>(i));
                }
            }
        }
    }
}

std::vector<std::pair<int, int>> MapInfo::GetSegmentsInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const {
    std::vector<std::pair<int, int>> result;
    
    if (segments_.empty() || segments_[0].empty()) {
        return result;
    }
    
    const int SEGMENT_SIZE = 256; // Tamanho de cada segmento em células
    
    // Garantir que x1 <= x2 e y1 <= y2
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    // Calcular índices de segmentos
    int startSegX = x1 / SEGMENT_SIZE;
    int startSegY = y1 / SEGMENT_SIZE;
    int endSegX = x2 / SEGMENT_SIZE;
    int endSegY = y2 / SEGMENT_SIZE;
    
    // Limitar a faixa de segmentos
    startSegX = std::max(0, startSegX);
    startSegY = std::max(0, startSegY);
    endSegX = std::min(endSegX, static_cast<int>(segments_.size()) - 1);
    endSegY = std::min(endSegY, static_cast<int>(segments_[0].size()) - 1);
    
    // Adicionar todos os segmentos na á/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++segX Parâmetro ++segX
 * @return Retorna rea
 */
rea
    for(int segX = startSegX; segX <= endSegX; ++segX){
        for (int segY = startSegY; segY <= endSegY; ++segY) {
            result.emplace_back(segX, segY);
        }
    }
    
    return result;
}

bool MapInfo::ReadWysStructures(const uint8_t* buffer, size_t size) {
    if (buffer == nullptr || size < sizeof(WysHeader)) {
        return false;
    }
    
    const WysHeader* header = reinterpret_cast<const WysHeader*>(buffer);
    
    // Verificar assinatura
    if (std::strncmp(header->signature, "WYS ", 4) != 0) {
        return false;
    }
    
    // Atualizar informações
    width_ = header->width;
    height_ = header->height;
    attribute_ = header->attribute;
    weatherType_ = header->weatherType;
    weatherIntensity_ = header->weatherIntensity;
    
    // Avançar para além do cabeçalho
    size_t offset = sizeof(WysHeader);
    
    // Ler células
    size_t cellsSize = width_ * /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MapCell Parâmetro MapCell
 * @return Retorna height_
 */
 height_ * sizeof(MapCell);
    if (offset + cellsSize > size) {
        return false;
    }
    
    cells_.resize(width_ * height_);
    std::memcpy(cells_.data(), buffer + offset, cellsSize);
    offset += cellsSize;
    
    // Ler terrenos
    size_t terrainsSize = header->/**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MapTerrain Parâmetro MapTerrain
 * @return Retorna numTerrains
 */
numTerrains * sizeof(MapTerrain);
    if (offset + terrainsSize > size) {
        return false;
    }
    
    terrains_.resize(header->numTerrains);
    std::memcpy(terrains_.data(), buffer + offset, terrainsSize);
    offset += terrainsSize;
    
    // Ler rotas
    routes_.clear();
    for (uint16_t i = 0; i < header->numRoutes; ++i) {
        // Verificar espaço para cabeçalho da /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param size Parâmetro size
 * @return Retorna rota
 */
 rota
        if(offset + 4 > size){
            return false;
        }
        
        // Ler ID da rota
        uint16_t routeId;
        std::memcpy(&routeId, buffer + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Ler tipo de loop
        uint8_t loopType;
        std::memcpy(&loopType, buffer + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        
        // Ler número de pontos
        uint8_t numPoints;
        std::memcpy(&numPoints, buffer + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        
        // Pular padding
        offset += sizeof(uint8_t);
        
        // Verificar espaço para pontos
        if (offset + numPoints * sizeof(MapRoutePoint) > size) {
            return false;
        }
        
        // Criar rota
        MapRoute route;
        route.routeId = routeId;
        route.loopType = loopType;
        route.numPoints = numPoints;
        
        // Ler pontos
        route.points.resize(numPoints);
        std::memcpy(route.points.data(), buffer + offset, numPoints * sizeof(MapRoutePoint));
        offset += /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MapRoutePoint Parâmetro MapRoutePoint
 * @return Retorna numPoints
 */
 numPoints * sizeof(MapRoutePoint);
        
        // Adicionar rota
        routes_.push_back(route);
    }
    
    // Ler teleportes
    size_t teleportsSize = header->/**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MapTeleport Parâmetro MapTeleport
 * @return Retorna numTeleports
 */
numTeleports * sizeof(MapTeleport);
    if (offset + teleportsSize > size) {
        return false;
    }
    
    teleports_.resize(header->numTeleports);
    std::memcpy(teleports_.data(), buffer + offset, teleportsSize);
    offset += teleportsSize;
    
    // Ler NPCs
    size_t npcsSize = header->/**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MapNPC Parâmetro MapNPC
 * @return Retorna numNPCs
 */
numNPCs * sizeof(MapNPC);
    if (offset + npcsSize > size) {
        return false;
    }
    
    npcs_.resize(header->numNPCs);
    std::memcpy(npcs_.data(), buffer + offset, npcsSize);
    offset += npcsSize;
    
    // Ler eventos
    events_.clear();
    for (uint16_t i = 0; i < header->numEvents; ++i) {
        // Verificar espaço para dados básicos do evento
        if (offset + sizeof(MapEvent) - sizeof(std::string) > size) {
            return false;
        }
        
        // Criar evento
        MapEvent event;
        
        // Ler ID do evento
        std::memcpy(&event.eventId, buffer + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Ler tipo de evento
        std::memcpy(&event.type, buffer + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Ler posição X
        std::memcpy(&event.x, buffer + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Ler posição Y
        std::memcpy(&event.y, buffer + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Ler raio
        std::memcpy(&event.radius, buffer + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        // Ler tempo de gatilho
        std::memcpy(&event.triggerTime, buffer + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Ler tempo de permanência
        std::memcpy(&event.keepTime, buffer + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Ler parâmetros
        std::memcpy(&event.parameter1, buffer + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        std::memcpy(&event.parameter2, buffer + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        std::memcpy(&event.parameter3, buffer + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Ler script
        const char* scriptPtr = reinterpret_cast<const char*>(buffer + offset);
        size_t scriptLen = 0;
        
        // Encontrar o terminador /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param '\0' Parâmetro '\0'
 * @return Retorna nulo
 */
 nulo
        while(offset + scriptLen < size && scriptPtr[scriptLen] != '\0'){
            scriptLen++;
        }
        
        // Verificar se encontramos o terminador /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param size Parâmetro size
 * @return Retorna nulo
 */
 nulo
        if(offset + scriptLen >= size){
            return false;
        }
        
        // Copiar script
        event.script = std::string(scriptPtr, scriptLen);
        offset += scriptLen + 1; // +1 para o terminador nulo
        
        // Adicionar evento
        events_.push_back(event);
    }
    
    // Ler áreas
    size_t areasSize = header->/**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MapArea Parâmetro MapArea
 * @return Retorna numAreas
 */
numAreas * sizeof(MapArea);
    if (offset + areasSize > size) {
        return false;
    }
    
    areas_.resize(header->numAreas);
    std::memcpy(areas_.data(), buffer + offset, areasSize);
    offset += areasSize;
    
    return true;
}

/**
 * @brief Implementação da classe MapManager
 */

/**
 * Classe WYDMapManager
 * 
 * Esta classe implementa a funcionalidade WYDMapManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MapManager {
public:
    // Singleton
    static MapManager& GetInstance() {
        static MapManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador de mapas
     * @param mapsDirectory Diretório contendo os arquivos de mapa
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& mapsDirectory) {
        // Armazenar diretório
        mapsDirectory_ = mapsDirectory;
        
        // Verificar se o diretório existe
        struct stat info;
        if (stat(mapsDirectory.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
            std::cerr << "Diretório de mapas inválido: " << mapsDirectory << std::endl;
            return false;
        }
        
        // Inicializar variáveis
        maps_.clear();
        mapIdByName_.clear();
        initialized_ = true;
        
        std::cout << "MapManager inicializado com sucesso: " << mapsDirectory << std::endl;
        return true;
    }
    
    /**
     * @brief Finaliza o gerenciador de mapas
     */
    void Shutdown() {
        // Limpar mapas
        maps_.clear();
        mapIdByName_.clear();
        initialized_ = false;
        
        std::cout << "MapManager finalizado" << std::endl;
    }
    
    /**
     * @brief Carrega um mapa
     * @param mapId ID do mapa
     * @param mapName Nome do mapa
     * @return true se carregado com sucesso
     */
    bool LoadMap(uint16_t mapId, const std::string& mapName) {
        if (!initialized_) {
            std::cerr << "MapManager não inicializado" << std::endl;
            return false;
        }
        
        // Verificar se o mapa já está carregado
        if (maps_.find(mapId) != maps_.end()) {
            std::cout << "Mapa já carregado: " << mapName << " (" << mapId << ")" << std::endl;
            return true;
        }
        
        // Construir caminho para o arquivo
        std::string filePath = mapsDirectory_ + "/" + mapName + ".wys";
        
        // Criar mapa
        std::shared_ptr<MapInfo> map = std::make_shared<MapInfo>(mapId, mapName);
        
        // Carregar mapa
        if (!map->LoadFromWysFile(filePath)) {
            std::cerr << "Falha ao carregar mapa: " << filePath << std::endl;
            return false;
        }
        
        // Adicionar mapa
        maps_[mapId] = map;
        mapIdByName_[mapName] = mapId;
        
        std::cout << "Mapa carregado: " << mapName << " (" << mapId << ")" << std::endl;
        return true;
    }
    
    /**
     * @brief Descarrega um mapa
     * @param mapId ID do mapa
     * @return true se descarregado com sucesso
     */
    bool UnloadMap(uint16_t mapId) {
        if (!initialized_) {
            std::cerr << "MapManager não inicializado" << std::endl;
            return false;
        }
        
        // Verificar se o mapa está carregado
        auto it = maps_.find(mapId);
        if (it == maps_.end()) {
            std::cerr << "Mapa não carregado: " << mapId << std::endl;
            return false;
        }
        
        // Remover da associação por nome
        std::string mapName = it->second->GetName();
        mapIdByName_.erase(mapName);
        
        // Remover mapa
        maps_.erase(it);
        
        std::cout << "Mapa descarregado: " << mapName << " (" << mapId << ")" << std::endl;
        return true;
    }
    
    /**
     * @brief Obtém um mapa pelo ID
     * @param mapId ID do mapa
     * @return Ponteiro para o mapa (nullptr se não encontrado)
     */
    std::shared_ptr<MapInfo> GetMap(uint16_t mapId) const {
        if (!initialized_) {
            std::cerr << "MapManager não inicializado" << std::endl;
            return nullptr;
        }
        
        // Buscar mapa
        auto it = maps_.find(mapId);
        if (it == maps_.end()) {
            return nullptr;
        }
        
        return it->second;
    }
    
    /**
     * @brief Obtém um mapa pelo nome
     * @param mapName Nome do mapa
     * @return Ponteiro para o mapa (nullptr se não encontrado)
     */
    std::shared_ptr<MapInfo> GetMapByName(const std::string& mapName) const {
        if (!initialized_) {
            std::cerr << "MapManager não inicializado" << std::endl;
            return nullptr;
        }
        
        // Buscar ID pelo nome
        auto it = mapIdByName_.find(mapName);
        if (it == mapIdByName_.end()) {
            return nullptr;
        }
        
        // Buscar mapa pelo ID
        /**
 * GetMap
 * 
 * Implementa a funcionalidade GetMap conforme especificação original.
 * @param it->second Parâmetro it->second
 * @return Retorna return
 */

        return GetMap(it->second);
    }
    
    /**
     * @brief Verifica se um mapa está carregado
     * @param mapId ID do mapa
     * @return true se o mapa estiver carregado
     */
    bool IsMapLoaded(uint16_t mapId) const {
        if (!initialized_) {
            return false;
        }
        
        return maps_.find(mapId) != maps_.end();
    }
    
    /**
     * @brief Verifica se um mapa está carregado pelo nome
     * @param mapName Nome do mapa
     * @return true se o mapa estiver carregado
     */
    bool IsMapLoadedByName(const std::string& mapName) const {
        if (!initialized_) {
            return false;
        }
        
        return mapIdByName_.find(mapName) != mapIdByName_.end();
    }
    
    /**
     * @brief Obtém a lista de mapas carregados
     * @return Lista de IDs de mapas
     */
    std::vector<uint16_t> GetLoadedMapIds() const {
        std::vector<uint16_t> result;
        
        if (!initialized_) {
            return result;
        }
        
        for (const auto& map : maps_) {
            result.push_back(map.first);
        }
        
        return result;
    }
    
    /**
     * @brief Obtém a lista de nomes de mapas carregados
     * @return Lista de nomes de mapas
     */
    std::vector<std::string> GetLoadedMapNames() const {
        std::vector<std::string> result;
        
        if (!initialized_) {
            return result;
        }
        
        for (const auto& map : mapIdByName_) {
            result.push_back(map.first);
        }
        
        return result;
    }
    
    /**
     * @brief Cria um mapa novo
     * @param mapId ID do mapa
     * @param mapName Nome do mapa
     * @param width Largura
     * @param height Altura
     * @param defaultAttribute Atributo padrão das células
     * @return true se criado com sucesso
     */
    bool CreateMap(uint16_t mapId, const std::string& mapName, uint16_t width, uint16_t height, uint8_t defaultAttribute = CELL_ATTR_EMPTY) {
        if (!initialized_) {
            std::cerr << "MapManager não inicializado" << std::endl;
            return false;
        }
        
        // Verificar se o mapa já existe
        if (maps_.find(mapId) != maps_.end()) {
            std::cerr << "Mapa já existe: " << mapName << " (" << mapId << ")" << std::endl;
            return false;
        }
        
        // Criar mapa
        std::shared_ptr<MapInfo> map = std::make_shared<MapInfo>(mapId, mapName);
        
        // Inicializar células
        if (!map->InitializeCells(width, height, defaultAttribute)) {
            std::cerr << "Falha ao inicializar células do mapa: " << mapName << std::endl;
            return false;
        }
        
        // Adicionar mapa
        maps_[mapId] = map;
        mapIdByName_[mapName] = mapId;
        
        std::cout << "Mapa criado: " << mapName << " (" << mapId << ")" << std::endl;
        return true;
    }
    
    /**
     * @brief Salva um mapa
     * @param mapId ID do mapa
     * @return true se salvo com sucesso
     */
    bool SaveMap(uint16_t mapId) {
        if (!initialized_) {
            std::cerr << "MapManager não inicializado" << std::endl;
            return false;
        }
        
        // Verificar se o mapa está carregado
        auto it = maps_.find(mapId);
        if (it == maps_.end()) {
            std::cerr << "Mapa não carregado: " << mapId << std::endl;
            return false;
        }
        
        // Construir caminho para o arquivo
        std::string mapName = it->second->GetName();
        std::string filePath = mapsDirectory_ + "/" + mapName + ".wys";
        
        // Salvar mapa
        if (!it->second->SaveToWysFile(filePath)) {
            std::cerr << "Falha ao salvar mapa: " << filePath << std::endl;
            return false;
        }
        
        std::cout << "Mapa salvo: " << mapName << " (" << mapId << ")" << std::endl;
        return true;
    }
    
    /**
     * @brief Determina o tipo de célula pela combinação de atributos
     * @param x Posição X
     * @param y Posição Y
     * @param mapId ID do mapa
     * @return Tipo de célula (ver enum CellType)
     */
    uint8_t GetCellType(uint16_t x, uint16_t y, uint16_t mapId) const {
        // Obter mapa
        std::shared_ptr<MapInfo> map = GetMap(mapId);
        if (!map) {
            return CELL_TYPE_INVALID;
        }
        
        // Verificar posição
        if (!map->IsValidPosition(x, y)) {
            return CELL_TYPE_INVALID;
        }
        
        // Obter atributo
        uint8_t attribute = map->GetCellAttribute(x, y);
        
        // Determinar tipo de cé/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CELL_ATTR_BLOCK Parâmetro CELL_ATTR_BLOCK
 * @return Retorna lula
 */
lula
        if(attribute & CELL_ATTR_BLOCK){
            return CELL_TYPE_BLOCKED;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CELL_ATTR_NOTMOVE Parâmetro CELL_ATTR_NOTMOVE
 * @return Retorna else
 */
 else if(attribute & CELL_ATTR_NOTMOVE){
            return CELL_TYPE_NOMOVE;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CELL_ATTR_WATER Parâmetro CELL_ATTR_WATER
 * @return Retorna else
 */
 else if(attribute & CELL_ATTR_WATER){
            return CELL_TYPE_WATER;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CELL_ATTR_FOUNTAIN Parâmetro CELL_ATTR_FOUNTAIN
 * @return Retorna else
 */
 else if(attribute & CELL_ATTR_FOUNTAIN){
            return CELL_TYPE_FOUNTAIN;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CELL_ATTR_NOATTACK Parâmetro CELL_ATTR_NOATTACK
 * @return Retorna else
 */
 else if(attribute & CELL_ATTR_NOATTACK){
            return CELL_TYPE_NOATTACK;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CELL_ATTR_GUILD_ZONE Parâmetro CELL_ATTR_GUILD_ZONE
 * @return Retorna else
 */
 else if(attribute & CELL_ATTR_GUILD_ZONE){
            return CELL_TYPE_GUILD;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CELL_ATTR_CASTLE_ZONE Parâmetro CELL_ATTR_CASTLE_ZONE
 * @return Retorna else
 */
 else if(attribute & CELL_ATTR_CASTLE_ZONE){
            return CELL_TYPE_CASTLE;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CELL_ATTR_EVENT_ZONE Parâmetro CELL_ATTR_EVENT_ZONE
 * @return Retorna else
 */
 else if(attribute & CELL_ATTR_EVENT_ZONE){
            return CELL_TYPE_EVENT;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param CELL_ATTR_TELEPORT Parâmetro CELL_ATTR_TELEPORT
 * @return Retorna else
 */
 else if(attribute & CELL_ATTR_TELEPORT){
            return CELL_TYPE_TELEPORT;
        }
        
        return CELL_TYPE_NORMAL;
    }
    
    /**
     * @brief Verifica se uma posição é válida para movimento
     * @param x Posição X
     * @param y Posição Y
     * @param mapId ID do mapa
     * @param isFlying Se a entidade está voando
     * @return true se pode mover para a posição
     */
    bool CanMoveTo(uint16_t x, uint16_t y, uint16_t mapId, bool isFlying = false) const {
        // Obter mapa
        std::shared_ptr<MapInfo> map = GetMap(mapId);
        if (!map) {
            return false;
        }
        
        return map->CanMoveToPosition(x, y, isFlying);
    }
    
    /**
     * @brief Calcula um caminho entre dois pontos
     * @param startX Posição X inicial
     * @param startY Posição Y inicial
     * @param endX Posição X final
     * @param endY Posição Y final
     * @param mapId ID do mapa
     * @param maxDistance Distância máxima
     * @param isFlying Se a entidade está voando
     * @return Lista de pontos do caminho
     */
    std::vector<std::pair<uint16_t, uint16_t>> CalculatePath(
        uint16_t startX, uint16_t startY, 
        uint16_t endX, uint16_t endY, 
        uint16_t mapId, 
        uint16_t maxDistance = 0xFFFF, 
        bool isFlying = false) const {
        
        // Obter mapa
        std::shared_ptr<MapInfo> map = GetMap(mapId);
        if (!map) {
            return {};
        }
        
        return map->CalculatePath(startX, startY, endX, endY, maxDistance, isFlying);
    }
    
    /**
     * @brief Verifica se uma posição está dentro de uma área específica
     * @param x Posição X
     * @param y Posição Y
     * @param areaId ID da área
     * @param mapId ID do mapa
     * @return true se a posição estiver dentro da área
     */
    bool IsPositionInArea(uint16_t x, uint16_t y, uint16_t areaId, uint16_t mapId) const {
        // Obter mapa
        std::shared_ptr<MapInfo> map = GetMap(mapId);
        if (!map) {
            return false;
        }
        
        // Buscar área
        for (const auto& area : map->GetAreas()) {
            if (area.areaId == areaId) {
                return x >= area.x1 && x <= area.x2 && y >= area.y1 && y <= area.y2;
            }
        }
        
        return false;
    }
    
    /**
     * @brief Processa um teleporte
     * @param x Posição X
     * @param y Posição Y
     * @param mapId ID do mapa
     * @param level Nível do personagem
     * @param hasTeleportPermission Se o personagem tem permissão especial de teleporte
     * @return Informações do teleporte (dstMap, dstX, dstY, condition, cost)
     */
    std::tuple<uint16_t, uint16_t, uint16_t, uint8_t, uint32_t> ProcessTeleport(
        uint16_t x, uint16_t y, 
        uint16_t mapId, 
        uint8_t level, 
        bool hasTeleportPermission = false) const {
        
        // Obter mapa
        std::shared_ptr<MapInfo> map = GetMap(mapId);
        if (!map) {
            return {0, 0, 0, 0, 0};
        }
        
        // Buscar teleporte
        int teleportIndex = map->FindTeleportByPosition(x, y);
        if (teleportIndex < 0) {
            return {0, 0, 0, 0, 0};
        }
        
        const auto& teleport = map->GetTeleports()[teleportIndex];
        
        // Verificar condiçã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !hasTeleportPermission Parâmetro !hasTeleportPermission
 * @return Retorna o
 */
o
        if(teleport.condition == TELEPORT_CONDITION_LEVEL && !hasTeleportPermission){
            if (level < teleport.level) {
                return {0, 0, 0, TELEPORT_CONDITION_LEVEL, teleport.cost};
            }
        }
        
        return {teleport.dstMap, teleport.dstX, teleport.dstY, teleport.condition, teleport.cost};
    }
    
    /**
     * @brief Atualiza o clima de um mapa
     * @param mapId ID do mapa
     * @param weatherType Tipo de clima
     * @param weatherIntensity Intensidade
     * @return true se atualizado com sucesso
     */
    bool UpdateWeather(uint16_t mapId, uint8_t weatherType, uint8_t weatherIntensity) {
        // Obter mapa
        std::shared_ptr<MapInfo> map = GetMap(mapId);
        if (!map) {
            return false;
        }
        
        // Atualizar clima
        map->UpdateWeather(weatherType, weatherIntensity);
        return true;
    }
    
    /**
     * @brief Lista informações de todos os mapas carregados
     * @param detailed Se deve exibir detalhes
     */
    void ListMaps(bool detailed = false) const {
        if (!initialized_) {
            std::cout << "MapManager não inicializado" << std::endl;
            return;
        }
        
        std::cout << "=== Mapas Carregados ===" << std::endl;
        for (const auto& map : maps_) {
            std::cout << "ID: " << map.first << " | Nome: " << map.second->GetName()
                      << " | Dimensões: " << map.second->GetWidth() << "x" << map.second->GetHeight()
                      << std::endl;
            
            if (detailed) {
                map.second->PrintInfo(false);
            }
        }
        std::cout << "======================" << std::endl;
    }
    
private:
    MapManager() /**
 * initialized_
 * 
 * Implementa a funcionalidade initialized_ conforme especificação original.
 * @param false Parâmetro false
 * @return Retorna :
 */
 : initialized_(false){}
    ~MapManager() {
        Shutdown();
    }
    
    // Atributos
    bool initialized_;
    std::string mapsDirectory_;
    std::unordered_map<uint16_t, std::shared_ptr<MapInfo>> maps_;
    std::unordered_map<std::string, uint16_t> mapIdByName_;
};

} // namespace wydbr