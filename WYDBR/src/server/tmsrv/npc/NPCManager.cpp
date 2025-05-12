/**
 * NPCManager.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/NPCManager.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/npc/NPCManager.h"
#include "src/server/tmsrv/npc/WYDMob.h"
#include "src/server/tmsrv/npc/WYDNPC.h"
#include "src/server/tmsrv/world/WYDMap.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>

namespace wydbr {


NPCManager::NPCManager(TMServer* server)
    : WYDSystemManager(server, "NPCManager")
    , spawnSystemActive_(false)
    , lastSpawnCheck_(0)
    , lastMobUpdate_(0)
    , nextUniqueId_(1000) // Iniciar IDs a partir de 1000
{
}

NPCManager::~NPCManager()
{
    shutdown();
}

bool NPCManager::initialize()
{
    Log()->info("Inicializando gerenciador de NPCs e monstros...");
    
    // Carregar dados
    bool success = true;
    success &= loadMobData("data/mobs/MobList.bin");
    success &= loadNPCData("data/npcs/NPCList.bin");
    success &= loadSpawnData("data/maps/SpawnList.bin");
    
    if (!success) {
        Log()->error("Falha ao carregar dados de NPCs e monstros");
        return false;
    }
    
    Log()->info("Gerenciador de NPCs e monstros inicializado com sucesso");
    Log()->info("Templates de mobs carregados: {}", mobTemplates_.size());
    Log()->info("Templates de NPCs carregados: {}", npcTemplates_.size());
    Log()->info("Pontos de spawn carregados: {}", spawnPoints_.size());
    
    return true;
}

void NPCManager::update(uint64_t timestamp)
{
    // Atualizar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1000 Parâmetro 1000
 * @return Retorna spawns
 */
 spawns
    if(spawnSystemActive_ && timestamp - lastSpawnCheck_ > 1000){ // 1s entre /**
 * updateSpawns
 * 
 * Implementa a funcionalidade updateSpawns conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @return Retorna checks
 */
 checks
        updateSpawns(timestamp);
        lastSpawnCheck_ = timestamp;
    }
    
    // Atualizar mobs (em trechos para não impactar performance)
    if (timestamp - lastMobUpdate_ > 100) { // 100ms entre atualizaçõ/**
 * updateMobs
 * 
 * Implementa a funcionalidade updateMobs conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @return Retorna es
 */
es
        updateMobs(timestamp);
        updateNPCs(timestamp);
        lastMobUpdate_ = timestamp;
    }
}

void NPCManager::shutdown()
{
    Log()->info("Desligando gerenciador de NPCs e monstros...");
    
    // Parar sistema de spawn
    spawnSystemActive_ = false;
    
    // Limpar todos os mobs e NPCs
    std::lock_guard<std::mutex> mobGuard(mobMutex_);
    std::lock_guard<std::mutex> npcGuard(npcMutex_);
    
    activeMobs_.clear();
    activeNPCs_.clear();
    mobsByMap_.clear();
    npcsByMap_.clear();
    mobsByType_.clear();
    
    Log()->info("Gerenciador de NPCs e monstros desligado");
}

bool NPCManager::loadMobData(const std::string& filename)
{
    // Em um cenário real, carregariamos de um arquivo binário
    // Aqui vamos simular o carregamento para demonstração
    
    Log()->info("Carregando dados de mobs de: {}", filename);
    
    // Para demonstração, criar alguns templates de mob
    // Em um cenário real, isso seria carregado do arquivo
    
    // Adicionar alguns mobs de exemplo
    MobTemplate wolf;
    wolf.id = 101;
    wolf.name = "Lobo Selvagem";
    wolf.level = 10;
    wolf.hp = 500;
    wolf.mp = 0;
    wolf.strength = 20;
    wolf.dexterity = 15;
    wolf.intelligence = 5;
    wolf.constitution = 15;
    wolf.attack = 30;
    wolf.defense = 20;
    wolf.aggroRange = 10;
    wolf.moveSpeed = 5;
    wolf.attackSpeed = 7;
    wolf.isAggressive = true;
    wolf.isBoss = false;
    wolf.skills[0] = 0; // Sem skills
    wolf.dropItems[0] = 2001; // Item: Pele de Lobo
    wolf.dropRates[0] = 60;   // 60% de chance
    wolf.experience = 50;
    wolf.gold = 25;
    
    mobTemplates_[wolf.id] = wolf;
    
    MobTemplate skeleton;
    skeleton.id = 102;
    skeleton.name = "Esqueleto";
    skeleton.level = 15;
    skeleton.hp = 700;
    skeleton.mp = 50;
    skeleton.strength = 25;
    skeleton.dexterity = 12;
    skeleton.intelligence = 8;
    skeleton.constitution = 10;
    skeleton.attack = 40;
    skeleton.defense = 15;
    skeleton.aggroRange = 8;
    skeleton.moveSpeed = 4;
    skeleton.attackSpeed = 5;
    skeleton.isAggressive = true;
    skeleton.isBoss = false;
    skeleton.skills[0] = 1001; // Skill: Golpe Ósseo
    skeleton.dropItems[0] = 2002; // Item: Osso
    skeleton.dropRates[0] = 75;   // 75% de chance
    skeleton.dropItems[1] = 2003; // Item: Crânio
    skeleton.dropRates[1] = 25;   // 25% de chance
    skeleton.experience = 75;
    skeleton.gold = 35;
    
    mobTemplates_[skeleton.id] = skeleton;
    
    MobTemplate dragon;
    dragon.id = 201;
    dragon.name = "Dragão Antigo";
    dragon.level = 50;
    dragon.hp = 10000;
    dragon.mp = 2000;
    dragon.strength = 100;
    dragon.dexterity = 50;
    dragon.intelligence = 80;
    dragon.constitution = 120;
    dragon.attack = 200;
    dragon.defense = 150;
    dragon.aggroRange = 20;
    dragon.moveSpeed = 3;
    dragon.attackSpeed = 3;
    dragon.isAggressive = true;
    dragon.isBoss = true;
    dragon.skills[0] = 2001; // Skill: Baforada de Fogo
    dragon.skills[1] = 2002; // Skill: Golpe da Cauda
    dragon.skills[2] = 2003; // Skill: Rugido do Dragão
    dragon.dropItems[0] = 3001; // Item: Escama de Dragão
    dragon.dropRates[0] = 100;  // 100% de chance
    dragon.dropItems[1] = 3002; // Item: Coração de Dragão
    dragon.dropRates[1] = 10;   // 10% de chance
    dragon.experience = 5000;
    dragon.gold = 2000;
    
    mobTemplates_[dragon.id] = dragon;
    
    Log()->info("Carregados {} templates de mobs", mobTemplates_.size());
    
    return true;
}

bool NPCManager::loadNPCData(const std::string& filename)
{
    // Em um cenário real, carregariamos de um arquivo binário
    // Aqui vamos simular o carregamento para demonstração
    
    Log()->info("Carregando dados de NPCs de: {}", filename);
    
    // Para demonstração, criar alguns templates de NPC
    // Em um cenário real, isso seria carregado do arquivo
    
    // Adicionar alguns NPCs de exemplo
    MobTemplate merchant;
    merchant.id = 1001;
    merchant.name = "Mercador de Armas";
    merchant.level = 1;
    merchant.hp = 1000;
    merchant.mp = 0;
    merchant.aggroRange = 0;
    merchant.moveSpeed = 2;
    merchant.attackSpeed = 0;
    merchant.isAggressive = false;
    merchant.isBoss = false;
    
    npcTemplates_[merchant.id] = merchant;
    
    MobTemplate innkeeper;
    innkeeper.id = 1002;
    innkeeper.name = "Estalajadeiro";
    innkeeper.level = 1;
    innkeeper.hp = 1000;
    innkeeper.mp = 0;
    innkeeper.aggroRange = 0;
    innkeeper.moveSpeed = 2;
    innkeeper.attackSpeed = 0;
    innkeeper.isAggressive = false;
    innkeeper.isBoss = false;
    
    npcTemplates_[innkeeper.id] = innkeeper;
    
    MobTemplate questGiver;
    questGiver.id = 1003;
    questGiver.name = "Mestre de Aventureiros";
    questGiver.level = 50;
    questGiver.hp = 10000;
    questGiver.mp = 5000;
    questGiver.aggroRange = 0;
    questGiver.moveSpeed = 2;
    questGiver.attackSpeed = 0;
    questGiver.isAggressive = false;
    questGiver.isBoss = false;
    
    npcTemplates_[questGiver.id] = questGiver;
    
    Log()->info("Carregados {} templates de NPCs", npcTemplates_.size());
    
    return true;
}

bool NPCManager::loadSpawnData(const std::string& filename)
{
    // Em um cenário real, carregariamos de um arquivo binário
    // Aqui vamos simular o carregamento para demonstração
    
    Log()->info("Carregando dados de spawn de: {}", filename);
    
    // Para demonstração, criar alguns pontos de spawn
    // Em um cenário real, isso seria carregado do arquivo
    
    // Adicionar alguns pontos de spawn de exemplo
    SpawnPoint wolfSpawn;
    wolfSpawn.mobId = 101; // Lobo Selvagem
    wolfSpawn.mapId = 1;   // Floresta
    wolfSpawn.x = 500;
    wolfSpawn.y = 500;
    wolfSpawn.range = 150;
    wolfSpawn.amount = 10;
    wolfSpawn.respawnTime = 60000; // 1 minuto
    wolfSpawn.lastSpawnTime = 0;
    
    spawnPoints_.push_back(wolfSpawn);
    
    SpawnPoint skeletonSpawn;
    skeletonSpawn.mobId = 102; // Esqueleto
    skeletonSpawn.mapId = 2;   // Caverna
    skeletonSpawn.x = 300;
    skeletonSpawn.y = 300;
    skeletonSpawn.range = 100;
    skeletonSpawn.amount = 15;
    skeletonSpawn.respawnTime = 90000; // 1.5 minutos
    skeletonSpawn.lastSpawnTime = 0;
    
    spawnPoints_.push_back(skeletonSpawn);
    
    SpawnPoint dragonSpawn;
    dragonSpawn.mobId = 201; // Dragão Antigo
    dragonSpawn.mapId = 3;   // Montanha
    dragonSpawn.x = 800;
    dragonSpawn.y = 800;
    dragonSpawn.range = 10;
    dragonSpawn.amount = 1;
    dragonSpawn.respawnTime = 3600000; // 1 hora
    dragonSpawn.lastSpawnTime = 0;
    
    spawnPoints_.push_back(dragonSpawn);
    
    Log()->info("Carregados {} pontos de spawn", spawnPoints_.size());
    
    return true;
}

uint32_t NPCManager::createMob(uint16_t mobId, uint16_t mapId, uint16_t x, uint16_t y, uint8_t direction)
{
    std::lock_guard<std::mutex> guard(mobMutex_);
    
    // Verificar se o template existe
    auto tmplIt = mobTemplates_.find(mobId);
    if (tmplIt == mobTemplates_.end()) {
        Log()->error("Tentativa de criar mob inexistente: {}", mobId);
        return 0;
    }
    
    // Verificar se a posição é válida
    if (!isValidSpawnLocation(mapId, x, y)) {
        Log()->error("Tentativa de criar mob em posição inválida: Map={}, X={}, Y={}", mapId, x, y);
        return 0;
    }
    
    // Criar instância do mob
    WYDMob* mob = createMobInstance(tmplIt->second, mapId, x, y, direction);
    if (!mob) {
        Log()->error("Falha ao criar instância de mob: {}", mobId);
        return 0;
    }
    
    uint32_t uniqueId = mob->getUniqueId();
    
    // Adicionar aos índices
    mobsByMap_[mapId].push_back(uniqueId);
    mobsByType_[mobId].push_back(uniqueId);
    
    Log()->debug("Mob criado: ID={}, Tipo={}, Nome={}, Mapa={}, X={}, Y={}", 
                uniqueId, mobId, mob->getName(), mapId, x, y);
    
    return uniqueId;
}

uint32_t NPCManager::createNPC(uint16_t npcId, uint16_t mapId, uint16_t x, uint16_t y, uint8_t direction)
{
    std::lock_guard<std::mutex> guard(npcMutex_);
    
    // Verificar se o template existe
    auto tmplIt = npcTemplates_.find(npcId);
    if (tmplIt == npcTemplates_.end()) {
        Log()->error("Tentativa de criar NPC inexistente: {}", npcId);
        return 0;
    }
    
    // Verificar se a posição é válida
    if (!isValidSpawnLocation(mapId, x, y)) {
        Log()->error("Tentativa de criar NPC em posição inválida: Map={}, X={}, Y={}", mapId, x, y);
        return 0;
    }
    
    // Criar instância do NPC
    WYDNPC* npc = createNPCInstance(tmplIt->second, mapId, x, y, direction);
    if (!npc) {
        Log()->error("Falha ao criar instância de NPC: {}", npcId);
        return 0;
    }
    
    uint32_t uniqueId = npc->getUniqueId();
    
    // Adicionar aos índices
    npcsByMap_[mapId].push_back(uniqueId);
    
    Log()->debug("NPC criado: ID={}, Tipo={}, Nome={}, Mapa={}, X={}, Y={}", 
                uniqueId, npcId, npc->getName(), mapId, x, y);
    
    return uniqueId;
}

uint32_t NPCManager::spawnQuestMob(uint16_t mobId, uint16_t mapId, uint16_t x, uint16_t y, uint32_t questId, uint32_t playerId)
{
    uint32_t mobUniqueId = createMob(mobId, mapId, x, y, 0);
    if (mobUniqueId == 0) {
        return 0;
    }
    
    WYDMob* mob = getMob(mobUniqueId);
    if (!mob) {
        return 0;
    }
    
    // Definir como entidade de quest
    mob->setFlag(EntityFlag::QUEST_ENTITY, true);
    
    // Em uma implementação real, registraria o mob no sistema de quests
    // e associaria ao jogador específico se necessário
    
    return mobUniqueId;
}

uint32_t NPCManager::spawnEventMob(uint16_t mobId, uint16_t mapId, uint16_t x, uint16_t y, uint32_t eventId)
{
    uint32_t mobUniqueId = createMob(mobId, mapId, x, y, 0);
    if (mobUniqueId == 0) {
        return 0;
    }
    
    WYDMob* mob = getMob(mobUniqueId);
    if (!mob) {
        return 0;
    }
    
    // Definir como entidade de evento
    mob->setFlag(EntityFlag::EVENT_ENTITY, true);
    
    // Em uma implementação real, registraria o mob no sistema de eventos
    
    return mobUniqueId;
}

WYDMob* NPCManager::getMob(uint32_t uniqueId)
{
    std::lock_guard<std::mutex> guard(mobMutex_);
    
    auto it = activeMobs_.find(uniqueId);
    if (it != activeMobs_.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

WYDNPC* NPCManager::getNPC(uint32_t uniqueId)
{
    std::lock_guard<std::mutex> guard(npcMutex_);
    
    auto it = activeNPCs_.find(uniqueId);
    if (it != activeNPCs_.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

WYDEntity* NPCManager::getEntity(uint32_t uniqueId)
{
    // Verificar se é um mob
    WYDMob* mob = getMob(uniqueId);
    if (mob) {
        return mob;
    }
    
    // Verificar se é um NPC
    WYDNPC* npc = getNPC(uniqueId);
    if (npc) {
        return npc;
    }
    
    return nullptr;
}

bool NPCManager::removeMob(uint32_t uniqueId)
{
    std::lock_guard<std::mutex> guard(mobMutex_);
    
    auto it = activeMobs_.find(uniqueId);
    if (it == activeMobs_.end()) {
        return false;
    }
    
    WYDMob* mob = it->second.get();
    uint16_t mapId = mob->getMapId();
    uint16_t mobId = mob->getMobId();
    
    // Remover dos índices
    auto& mapList = mobsByMap_[mapId];
    mapList.erase(std::remove(mapList.begin(), mapList.end(), uniqueId), mapList.end());
    
    auto& typeList = mobsByType_[mobId];
    typeList.erase(std::remove(typeList.begin(), typeList.end(), uniqueId), typeList.end());
    
    // Remover dos pontos de /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param spawnPoints_ Parâmetro spawnPoints_
 * @return Retorna spawn
 */
 spawn
    for(auto& point : spawnPoints_){
        auto& spawnedList = point.spawnedMobs;
        spawnedList.erase(std::remove(spawnedList.begin(), spawnedList.end(), uniqueId), spawnedList.end());
    }
    
    // Remover da lista de mobs ativos
    activeMobs_.erase(uniqueId);
    
    Log()->debug("Mob removido: ID={}", uniqueId);
    
    return true;
}

bool NPCManager::removeNPC(uint32_t uniqueId)
{
    std::lock_guard<std::mutex> guard(npcMutex_);
    
    auto it = activeNPCs_.find(uniqueId);
    if (it == activeNPCs_.end()) {
        return false;
    }
    
    WYDNPC* npc = it->second.get();
    uint16_t mapId = npc->getMapId();
    
    // Remover dos índices
    auto& mapList = npcsByMap_[mapId];
    mapList.erase(std::remove(mapList.begin(), mapList.end(), uniqueId), mapList.end());
    
    // Remover da lista de NPCs ativos
    activeNPCs_.erase(uniqueId);
    
    Log()->debug("NPC removido: ID={}", uniqueId);
    
    return true;
}

std::vector<uint32_t> NPCManager::getMobsInRange(uint16_t mapId, uint16_t centerX, uint16_t centerY, uint16_t radius)
{
    std::lock_guard<std::mutex> guard(mobMutex_);
    
    std::vector<uint32_t> result;
    
    // Verificar se o mapa tem mobs
    auto mapIt = mobsByMap_.find(mapId);
    if (mapIt == mobsByMap_.end()) {
        return result;
    }
    
    // Verificar cada mob no /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param mapIt->second Parâmetro mapIt->second
 * @return Retorna mapa
 */
 mapa
    for(uint32_t mobUniqueId : mapIt->second){
        auto mobIt = activeMobs_.find(mobUniqueId);
        if (mobIt == activeMobs_.end()) {
            continue;
        }
        
        WYDMob* mob = mobIt->second.get();
        
        // Calcular distância
        int dx = static_cast<int>(mob->getPosX()) - static_cast<int>(centerX);
        int dy = static_cast<int>(mob->getPosY()) - static_cast<int>(centerY);
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance <= radius) {
            result.push_back(mobUniqueId);
        }
    }
    
    return result;
}

std::vector<uint32_t> NPCManager::getNPCsInRange(uint16_t mapId, uint16_t centerX, uint16_t centerY, uint16_t radius)
{
    std::lock_guard<std::mutex> guard(npcMutex_);
    
    std::vector<uint32_t> result;
    
    // Verificar se o mapa tem NPCs
    auto mapIt = npcsByMap_.find(mapId);
    if (mapIt == npcsByMap_.end()) {
        return result;
    }
    
    // Verificar cada NPC no /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param mapIt->second Parâmetro mapIt->second
 * @return Retorna mapa
 */
 mapa
    for(uint32_t npcUniqueId : mapIt->second){
        auto npcIt = activeNPCs_.find(npcUniqueId);
        if (npcIt == activeNPCs_.end()) {
            continue;
        }
        
        WYDNPC* npc = npcIt->second.get();
        
        // Calcular distância
        int dx = static_cast<int>(npc->getPosX()) - static_cast<int>(centerX);
        int dy = static_cast<int>(npc->getPosY()) - static_cast<int>(centerY);
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance <= radius) {
            result.push_back(npcUniqueId);
        }
    }
    
    return result;
}

std::vector<uint32_t> NPCManager::getMobsByType(uint16_t mobId, uint16_t mapId)
{
    std::lock_guard<std::mutex> guard(mobMutex_);
    
    std::vector<uint32_t> result;
    
    // Verificar se existe este tipo de mob
    auto typeIt = mobsByType_.find(mobId);
    if (typeIt == mobsByType_.end()) {
        return result;
    }
    
    if (mapId == 0) {
        // Retornar todos deste tipo
        return typeIt->second;
    }
    
    // Filtrar por /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param typeIt->second Parâmetro typeIt->second
 * @return Retorna mapa
 */
 mapa
    for(uint32_t mobUniqueId : typeIt->second){
        auto mobIt = activeMobs_.find(mobUniqueId);
        if (mobIt == activeMobs_.end()) {
            continue;
        }
        
        WYDMob* mob = mobIt->second.get();
        
        if (mob->getMapId() == mapId) {
            result.push_back(mobUniqueId);
        }
    }
    
    return result;
}

bool NPCManager::moveMob(uint32_t uniqueId, uint16_t targetX, uint16_t targetY)
{
    WYDMob* mob = getMob(uniqueId);
    if (!mob) {
        return false;
    }
    
    return mob->moveToPosition(targetX, targetY);
}

bool NPCManager::attackMob(uint32_t uniqueId, uint32_t targetId, uint16_t skillId)
{
    WYDMob* mob = getMob(uniqueId);
    if (!mob) {
        return false;
    }
    
    if (skillId != 0) {
        return mob->useSkill(skillId, targetId);
    } else {
        return mob->attackTarget(targetId);
    }
}

bool NPCManager::talkToNPC(uint32_t npcId, uint32_t playerId, uint16_t dialogueOption)
{
    WYDNPC* npc = getNPC(npcId);
    if (!npc) {
        return false;
    }
    
    return npc->interact(playerId, dialogueOption);
}

void NPCManager::startWorldSpawns()
{
    Log()->info("Iniciando sistema de spawn global...");
    
    spawnSystemActive_ = true;
    lastSpawnCheck_ = static_cast<uint64_t>(std::time(nullptr)) * 1000;
    
    // Iniciar spawns /**
 * updateSpawns
 * 
 * Implementa a funcionalidade updateSpawns conforme especificação original.
 * @param lastSpawnCheck_ Parâmetro lastSpawnCheck_
 * @return Retorna iniciais
 */
 iniciais
    updateSpawns(lastSpawnCheck_);
    
    Log()->info("Sistema de spawn iniciado com sucesso");
}

void NPCManager::stopWorldSpawns()
{
    Log()->info("Parando sistema de spawn global...");
    
    spawnSystemActive_ = false;
    
    Log()->info("Sistema de spawn parado");
}

void NPCManager::resetMapSpawns(uint16_t mapId)
{
    std::lock_guard<std::mutex> mobGuard(mobMutex_);
    std::lock_guard<std::mutex> spawnGuard(spawnMutex_);
    
    Log()->info("Reiniciando spawns do mapa: {}", mapId);
    
    // Remover todos os mobs do mapa
    auto mapIt = mobsByMap_.find(mapId);
    if (mapIt != mobsByMap_.end()) {
        std::vector<uint32_t> mobsToRemove = mapIt->second;
        
        for (uint32_t mobUniqueId : mobsToRemove) {
            WYDMob* mob = getMob(mobUniqueId);
            if (mob && !mob->hasFlag(EntityFlag::QUEST_ENTITY) && !mob->hasFlag(EntityFlag::EVENT_ENTITY)) {
                removeMob(mobUniqueId);
            }
        }
    }
    
    // Resetar timers de spawn para este /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param spawnPoints_ Parâmetro spawnPoints_
 * @return Retorna mapa
 */
 mapa
    for(auto& point : spawnPoints_){
        if (point.mapId == mapId) {
            point.lastSpawnTime = 0;
            point.spawnedMobs.clear();
        }
    }
    
    // Realizar spawn imediato
    updateSpawns(static_cast<uint64_t>(std::time(nullptr)) * 1000);
    
    Log()->info("Spawns do mapa {} reiniciados", mapId);
}

size_t NPCManager::getMobCount(uint16_t mapId)
{
    std::lock_guard<std::mutex> guard(mobMutex_);
    
    if (mapId == 0) {
        return activeMobs_.size();
    }
    
    auto it = mobsByMap_.find(mapId);
    if (it != mobsByMap_.end()) {
        return it->second.size();
    }
    
    return 0;
}

size_t NPCManager::getNPCCount(uint16_t mapId)
{
    std::lock_guard<std::mutex> guard(npcMutex_);
    
    if (mapId == 0) {
        return activeNPCs_.size();
    }
    
    auto it = npcsByMap_.find(mapId);
    if (it != npcsByMap_.end()) {
        return it->second.size();
    }
    
    return 0;
}

std::map<uint16_t, uint32_t> NPCManager::getMobDistribution()
{
    std::lock_guard<std::mutex> guard(mobMutex_);
    
    std::map<uint16_t, uint32_t> result;
    
    // Contar mobs por /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param mobsByType_ Parâmetro mobsByType_
 * @return Retorna tipo
 */
 tipo
    for(const auto& entry : mobsByType_){
        result[entry.first] = entry.second.size();
    }
    
    return result;
}

void NPCManager::updateSpawns(uint64_t timestamp)
{
    std::lock_guard<std::mutex> guard(spawnMutex_);
    
    for (auto& point : spawnPoints_) {
        // Verificar se já passou o tempo de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param point.respawnTime Parâmetro point.respawnTime
 * @return Retorna respawn
 */
 respawn
        if(timestamp - point.lastSpawnTime < point.respawnTime){
            continue;
        }
        
        // Limpar mobs mortos da lista
        std::vector<uint32_t> validMobs;
        for (uint32_t mobId : point.spawnedMobs) {
            WYDMob* mob = getMob(mobId);
            if (mob && mob->getState() != EntityState::DEAD) {
                validMobs.push_back(mobId);
            }
        }
        
        point.spawnedMobs = validMobs;
        
        // Verificar se precisa spawnar mais mobs
        int toSpawn = point.amount - point.spawnedMobs.size();
        if (toSpawn <= 0) {
            continue;
        }
        
        // Spawnar novos /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna mobs
 */
 mobs
        for(int i = 0; i < toSpawn; ++i){
            // Gerar posição aleatória dentro do raio de spawn
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(-point.range, point.range);
            
            uint16_t x = point.x + dis(gen);
            uint16_t y = point.y + dis(gen);
            
            // Verificar se a posição é válida
            if (!isValidSpawnLocation(point.mapId, x, y)) {
                continue;
            }
            
            // Criar mob
            uint32_t mobUniqueId = createMob(point.mobId, point.mapId, x, y, 0);
            if (mobUniqueId != 0) {
                point.spawnedMobs.push_back(mobUniqueId);
            }
        }
        
        // Atualizar timestamp do último spawn
        point.lastSpawnTime = timestamp;
    }
}

void NPCManager::updateMobs(uint64_t timestamp)
{
    std::vector<uint32_t> mobsToUpdate;
    
    {
        std::lock_guard<std::mutex> guard(mobMutex_);
        
        // Coletar IDs de mobs para atualizar (para evitar problemas de concorrência)
        mobsToUpdate.reserve(activeMobs_.size());
        for (const auto& entry : activeMobs_) {
            mobsToUpdate.push_back(entry.first);
        }
    }
    
    // Atualizar cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param mobsToUpdate Parâmetro mobsToUpdate
 * @return Retorna mob
 */
 mob
    for(uint32_t mobUniqueId : mobsToUpdate){
        WYDMob* mob = getMob(mobUniqueId);
        if (mob) {
            mob->update(timestamp);
            
            // Se o mob morreu, processar morte
            if (mob->getState() == EntityState::DEAD) {
                processMobDeath(mob, mob->getCurrentTargetId());
            }
        }
    }
}

void NPCManager::updateNPCs(uint64_t timestamp)
{
    std::vector<uint32_t> npcsToUpdate;
    
    {
        std::lock_guard<std::mutex> guard(npcMutex_);
        
        // Coletar IDs de NPCs para atualizar (para evitar problemas de concorrência)
        npcsToUpdate.reserve(activeNPCs_.size());
        for (const auto& entry : activeNPCs_) {
            npcsToUpdate.push_back(entry.first);
        }
    }
    
    // Atualizar cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param npcsToUpdate Parâmetro npcsToUpdate
 * @return Retorna NPC
 */
 NPC
    for(uint32_t npcUniqueId : npcsToUpdate){
        WYDNPC* npc = getNPC(npcUniqueId);
        if (npc) {
            npc->update(timestamp);
        }
    }
}

WYDMob* NPCManager::createMobInstance(const MobTemplate& tmpl, uint16_t mapId, uint16_t x, uint16_t y, uint8_t direction)
{
    // Gerar ID único
    uint32_t uniqueId = nextUniqueId_++;
    
    // Criar instância
    auto mobPtr = std::make_unique<WYDMob>(uniqueId, tmpl.id, tmpl.name);
    WYDMob* mob = mobPtr.get();
    
    // Configurar posição
    mob->setMap(mapId);
    mob->setPosition(x, y);
    mob->setDirection(direction);
    
    // Configurar atributos
    mob->setAttributes(tmpl.level, tmpl.hp, tmpl.mp, 
                      tmpl.strength, tmpl.dexterity, tmpl.intelligence, tmpl.constitution,
                      tmpl.attack, tmpl.defense);
    
    // Configurar comportamento
    mob->setBehavior(tmpl.isAggressive ? MobBehavior::AGGRESSIVE : MobBehavior::PASSIVE);
    mob->setAggroRange(tmpl.aggroRange);
    mob->setMoveSpeed(tmpl.moveSpeed);
    mob->setAttackSpeed(tmpl.attackSpeed);
    mob->setSpawnPoint(x, y);
    
    // Configurar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param tmpl.isBoss Parâmetro tmpl.isBoss
 * @return Retorna flags
 */
 flags
    if(tmpl.isBoss){
        mob->setFlag(EntityFlag::BOSS, true);
    }
    
    // Configurar skills
    std::vector<uint16_t> skills;
    for (int i = 0; i < 8; ++i) {
        if (tmpl.skills[i] != 0) {
            skills.push_back(tmpl.skills[i]);
        }
    }
    mob->setSkills(skills);
    
    // Configurar drops
    std::vector<uint16_t> dropItems;
    std::vector<uint8_t> dropRates;
    for (int i = 0; i < 16; ++i) {
        if (tmpl.dropItems[i] != 0) {
            dropItems.push_back(tmpl.dropItems[i]);
            dropRates.push_back(tmpl.dropRates[i]);
        }
    }
    mob->setDropItems(dropItems, dropRates);
    
    // Configurar experiência e ouro
    mob->setExperienceAndGold(tmpl.experience, tmpl.gold);
    
    // Inicializar
    mob->initialize(this);
    
    // Adicionar à lista de mobs ativos
    activeMobs_[uniqueId] = std::move(mobPtr);
    
    return mob;
}

WYDNPC* NPCManager::createNPCInstance(const MobTemplate& tmpl, uint16_t mapId, uint16_t x, uint16_t y, uint8_t direction)
{
    // Gerar ID único
    uint32_t uniqueId = nextUniqueId_++;
    
    // Criar instância
    auto npcPtr = std::make_unique<WYDNPC>(uniqueId, tmpl.id, tmpl.name);
    WYDNPC* npc = npcPtr.get();
    
    // Configurar posição
    npc->setMap(mapId);
    npc->setPosition(x, y);
    npc->setDirection(direction);
    
    // Configurar tipo (baseado no ID, por exemplo)
    if (tmpl.id >= 1000 && tmpl.id < 1100) {
        npc->setType(NPCType::MERCHANT);
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1200 Parâmetro 1200
 * @return Retorna else
 */
 else if(tmpl.id >= 1100 && tmpl.id < 1200){
        npc->setType(NPCType::QUEST_GIVER);
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1300 Parâmetro 1300
 * @return Retorna else
 */
 else if(tmpl.id >= 1200 && tmpl.id < 1300){
        npc->setType(NPCType::TELEPORTER);
    } else {
        npc->setType(NPCType::SERVICE);
    }
    
    // Configurar movimento
    npc->setMoveSpeed(tmpl.moveSpeed);
    npc->setWanderArea(x, y, 5); // Pequena área de perambulação
    
    // Por padrão, NPCs não perambulam
    npc->setWandering(false);
    
    // Inicializar
    npc->initialize(this);
    
    // Adicionar à lista de NPCs ativos
    activeNPCs_[uniqueId] = std::move(npcPtr);
    
    return npc;
}

void NPCManager::checkMobsInRange(WYDMob* mob, uint16_t radius)
{
    if (!mob) {
        return;
    }
    
    uint16_t mapId = mob->getMapId();
    uint16_t x = mob->getPosX();
    uint16_t y = mob->getPosY();
    
    // Verificar outros mobs próximos (para comportamentos de assist, por exemplo)
    std::vector<uint32_t> nearbyMobs = getMobsInRange(mapId, x, y, radius);
    
    // Implementar lógica específica aqui
}

void NPCManager::processMobDeath(WYDMob* mob, uint32_t killerId)
{
    if (!mob) {
        return;
    }
    
    // Gerar drops (na implementação real, os drops seriam criados no mundo)
    std::vector<uint32_t> drops = mob->generateDrops();
    
    // Atribuir experiência e ouro (na implementação real, isso seria feito no PlayerManager)
    
    Log()->debug("Mob {} (ID: {}) morto por entidade {}", mob->getName(), mob->getUniqueId(), killerId);
    
    // Em uma implementação real, iniciaria um timer para remover o corpo e eventualmente respawnar
}

bool NPCManager::isValidSpawnLocation(uint16_t mapId, uint16_t x, uint16_t y)
{

} // namespace wydbr

    // Em uma implementação real, verificaria colisões, água, etc.
    // Para simplificar, vamos apenas verificar limites bá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna sicos
 */
sicos
    
    if(mapId == 0){
        return false;
    }
    
    // Assumindo limites de mapa padrã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 3990 Parâmetro 3990
 * @return Retorna o
 */
o
    if(x < 10 || x > 3990 || y < 10 || y > 3990){
        return false;
    }
    
    // Verificar mapa específico
    // WYDMap* map = GetMapManager()->GetMap(mapId);
    // if (!map) {
    //     return false;
    // }
    // return !map->IsBlocked(x, y);
    
    return true;
}