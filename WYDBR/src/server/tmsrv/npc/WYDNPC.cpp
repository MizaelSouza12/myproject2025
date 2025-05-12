/**
 * WYDNPC.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/WYDNPC.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/npc/WYDNPC.h"
#include "src/server/tmsrv/npc/NPCManager.h"
#include <algorithm>
#include <ctime>
#include <cmath>
#include <sstream>
#include <random>

namespace wydbr {


WYDNPC::WYDNPC(uint32_t uniqueId, uint16_t npcId, const std::string& name)
    : WYDEntity(uniqueId, name, EntityType::NPC)
    , manager_(nullptr)
    , npcId_(npcId)
    , type_(NPCType::SERVICE)
    , isWandering_(false)
    , moveSpeed_(0)
    , wanderCenterX_(0)
    , wanderCenterY_(0)
    , wanderRadius_(0)
    , lastMoveTime_(0)
    , lastWanderCheck_(0)
{
    // Desabilitar flags de ataque por padrão para /**
 * setFlag
 * 
 * Implementa a funcionalidade setFlag conforme especificação original.
 * @param EntityFlag::ATTACKABLE Parâmetro EntityFlag::ATTACKABLE
 * @param false Parâmetro false
 * @return Retorna NPCs
 */
 NPCs
    setFlag(EntityFlag::ATTACKABLE, false);
    setFlag(EntityFlag::NO_COLLISION, true); // NPCs geralmente não têm colisão
}

WYDNPC::~WYDNPC()
{
    // Limpar recursos
}

void WYDNPC::initialize(NPCManager* manager)
{
    manager_ = manager;
    
    // Centro da área de perambulação é a posição inicial
    wanderCenterX_ = getPosX();
    wanderCenterY_ = getPosY();
}

void WYDNPC::update(uint64_t timestamp)
{
    std::lock_guard<std::mutex> guard(npcMutex_);
    
    // Atualização de movimento (se estiver perambulando)
    if (isWandering_) {
        updateWandering(timestamp);
    }
    
    // Atualização de interaçõ/**
 * updateInteractions
 * 
 * Implementa a funcionalidade updateInteractions conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @return Retorna es
 */
es
    updateInteractions(timestamp);
    
    // Limpeza de interações /**
 * cleanupStaleInteractions
 * 
 * Implementa a funcionalidade cleanupStaleInteractions conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @return Retorna expiradas
 */
 expiradas
    cleanupStaleInteractions(timestamp);
}

std::string WYDNPC::serialize() const
{
    // Na implementação real, isso serializaria o NPC para o cliente
    // Aqui, vamos retornar uma string de exemplo para fins de estrutura
    std::stringstream ss;
    ss << "NPC:" << uniqueId_ << ":" << npcId_ << ":" << name_ << ":"
       << static_cast<int>(type_) << ":" << mapId_ << ":" << posX_ << ":" << posY_ << ":"
       << direction_;
    return ss.str();
}

void WYDNPC::setWanderArea(uint16_t centerX, uint16_t centerY, uint16_t radius)
{
    wanderCenterX_ = centerX;
    wanderCenterY_ = centerY;
    wanderRadius_ = radius;
}

void WYDNPC::setDialogues(const std::map<uint16_t, std::string>& dialogues)
{
    dialogues_ = dialogues;
}

void WYDNPC::setResponses(const std::map<uint16_t, std::vector<uint16_t>>& responses)
{
    responses_ = responses;
}

std::string WYDNPC::getDialogue(uint16_t dialogueId) const
{
    auto it = dialogues_.find(dialogueId);
    if (it != dialogues_.end()) {
        return it->second;
    }
    return "..."; // Diálogo padrão se não encontrado
}

std::vector<uint16_t> WYDNPC::getResponseOptions(uint16_t dialogueId) const
{
    auto it = responses_.find(dialogueId);
    if (it != responses_.end()) {
        return it->second;
    }
    return {}; // Lista vazia se não encontrado
}

bool WYDNPC::interact(uint32_t playerId, uint16_t option)
{
    // Iniciar nova interação ou continuar existente
    auto it = activeInteractions_.find(playerId);
    
    if (it == activeInteractions_.end()) {
        // Nova interação
        if (!startInteraction(playerId)) {
            return false;
        }
        
        it = activeInteractions_.find(playerId);
        if (it == activeInteractions_.end()) {
            return false;
        }
    }
    
    // Atualizar timestamp de interação
    it->second.lastInteraction = static_cast<uint64_t>(std::time(nullptr)) * 1000;
    
    // Processar opção /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna selecionada
 */
 selecionada
    if(option != 0){
        // Na implementação real, atualizaria o diálogo atual com base na opção
        // e processaria quaisquer ações (comércio, quest, etc.)
        
        // Exemplo: Avançar para o próximo diálogo
        it->second.currentDialogue = option;
        
        // Se a opção levar a uma ação especial, processá-la
        bool actionProcessed = false;
        
        switch (type_) {
            case NPCType::MERCHANT:
                // Abrir janela de comércio
                actionProcessed = true;
                break;
                
            case NPCType::QUEST_GIVER:
                // Iniciar ou completar quest
                actionProcessed = true;
                break;
                
            case NPCType::TELEPORTER:
                // Teletransportar jogador
                actionProcessed = true;
                break;
                
            default:
                // Apenas continuar diálogo
                break;
        }
        
        return actionProcessed;
    }
    
    // Retornar sucesso para interação básica
    return true;
}

bool WYDNPC::buy(uint32_t playerId, uint16_t itemId, uint16_t quantity)
{
    // Verificar se é um /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param NPCType::MERCHANT Parâmetro NPCType::MERCHANT
 * @return Retorna comerciante
 */
 comerciante
    if(type_ != NPCType::MERCHANT){
        return false;
    }
    
    // Verificar se tem o item
    if (!hasItem(itemId)) {
        return false;
    }
    
    // Na implementação real, processaria a compra, verificando dinheiro, etc.
    
    return true; // Compra bem-sucedida
}

bool WYDNPC::sell(uint32_t playerId, uint32_t itemInstanceId)
{
    // Verificar se é um /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param NPCType::MERCHANT Parâmetro NPCType::MERCHANT
 * @return Retorna comerciante
 */
 comerciante
    if(type_ != NPCType::MERCHANT){
        return false;
    }
    
    // Na implementação real, processaria a venda
    
    return true; // Venda bem-sucedida
}

bool WYDNPC::startQuest(uint32_t playerId, uint16_t questId)
{
    // Verificar se é um NPC de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param NPCType::QUEST_GIVER Parâmetro NPCType::QUEST_GIVER
 * @return Retorna quest
 */
 quest
    if(type_ != NPCType::QUEST_GIVER){
        return false;
    }
    
    // Na implementação real, verificaria requisitos e iniciaria a quest
    
    return true; // Quest iniciada com sucesso
}

bool WYDNPC::completeQuest(uint32_t playerId, uint16_t questId)
{
    // Verificar se é um NPC de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param NPCType::QUEST_GIVER Parâmetro NPCType::QUEST_GIVER
 * @return Retorna quest
 */
 quest
    if(type_ != NPCType::QUEST_GIVER){
        return false;
    }
    
    // Na implementação real, verificaria progresso e completaria a quest
    
    return true; // Quest completada com sucesso
}

bool WYDNPC::teleport(uint32_t playerId, uint16_t destMapId, uint16_t destX, uint16_t destY)
{
    // Verificar se é um /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param NPCType::TELEPORTER Parâmetro NPCType::TELEPORTER
 * @return Retorna teleportador
 */
 teleportador
    if(type_ != NPCType::TELEPORTER){
        return false;
    }
    
    // Na implementação real, teletransportaria o jogador
    
    return true; // Teleporte bem-sucedido
}

bool WYDNPC::useService(uint32_t playerId, uint16_t serviceId)
{
    // Verificar se tem o serviço
    if (!hasService(serviceId)) {
        return false;
    }
    
    // Na implementação real, processaria o serviço específico
    
    return true; // Serviço usado com sucesso
}

std::string WYDNPC::getParameter(const std::string& key) const
{
    auto it = parameters_.find(key);
    if (it != parameters_.end()) {
        return it->second;
    }
    return ""; // Valor vazio se não encontrado
}

bool WYDNPC::hasService(uint16_t serviceId) const
{
    return std::find(services_.begin(), services_.end(), serviceId) != services_.end();
}

bool WYDNPC::hasItem(uint16_t itemId) const
{
    return std::find(items_.begin(), items_.end(), itemId) != items_.end();
}

void WYDNPC::moveToPosition(uint16_t x, uint16_t y)
{
    // Atualizar posiçã/**
 * setPosition
 * 
 * Implementa a funcionalidade setPosition conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna o
 */
o
    setPosition(x, y);
    
    // Atualizar /**
 * setState
 * 
 * Implementa a funcionalidade setState conforme especificação original.
 * @param EntityState::MOVING Parâmetro EntityState::MOVING
 * @return Retorna estado
 */
 estado
    setState(EntityState::MOVING);
}

void WYDNPC::updateWandering(uint64_t timestamp)
{
    // Verificar cooldown de perambulaçã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 3000 Parâmetro 3000
 * @return Retorna o
 */
o
    if(timestamp - lastWanderCheck_ < 3000){ // 3s entre checks
        return;
    }
    
    lastWanderCheck_ = timestamp;
    
    // Verificar cooldown de movimento
    if (timestamp - lastMoveTime_ < (1000 / std::max(1, static_cast<int>(moveSpeed_)))) {
        return;
    }
    
    // Chance de movimento aleatório
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disMoveChance(1, 100);
    
    if (disMoveChance(gen) <= 10) { // 10% de chance de mover
        // Gerar posição aleatória
        auto newPos = getRandomWanderPosition();
        
        // Mover para nova posiçã/**
 * moveToPosition
 * 
 * Implementa a funcionalidade moveToPosition conforme especificação original.
 * @param newPos.first Parâmetro newPos.first
 * @param newPos.second Parâmetro newPos.second
 * @return Retorna o
 */
o
        moveToPosition(newPos.first, newPos.second);
        
        // Atualizar timestamp
        lastMoveTime_ = timestamp;
    } else if (getState() == EntityState::MOVING) {
        // Parar de /**
 * setState
 * 
 * Implementa a funcionalidade setState conforme especificação original.
 * @param EntityState::IDLE Parâmetro EntityState::IDLE
 * @return Retorna mover
 */
 mover
        setState(EntityState::IDLE);
    }
}

void WYDNPC::updateInteractions(uint64_t timestamp)
{
    // Atualizar estado de interações ativas
    // Na implementação real, processaria eventos baseados no tempo para interações
}

void WYDNPC::cleanupStaleInteractions(uint64_t timestamp)
{
    // Identificar interações inativas
    std::vector<uint32_t> toRemove;
    
    for (const auto& entry : activeInteractions_) {
        if (timestamp - entry.second.lastInteraction > 60000) { // 60s sem interação
            toRemove.push_back(entry.first);
        }
    }
    
    // Remover interações /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param toRemove Parâmetro toRemove
 * @return Retorna inativas
 */
 inativas
    for(uint32_t playerId : toRemove){
        endInteraction(playerId);
    }
}

std::pair<uint16_t, uint16_t> WYDNPC::getRandomWanderPosition()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(
        static_cast<int>(wanderCenterX_) - wanderRadius_,
        static_cast<int>(wanderCenterX_) + wanderRadius_
    );
    std::uniform_int_distribution<> disY(
        static_cast<int>(wanderCenterY_) - wanderRadius_,
        static_cast<int>(wanderCenterY_) + wanderRadius_
    );
    
    uint16_t newX, newY;
    int attempts = 0;
    
    do {
        newX = static_cast<uint16_t>(disX(gen));
        newY = static_cast<uint16_t>(disY(gen));
        attempts++;
    } while (!isValidPosition(newX, newY) && attempts < 10);
    
    if (attempts >= 10) {
        // Falha em encontrar posição válida, retornar posição atual
        return std::make_pair(getPosX(), getPosY());
    }
    
    return std::make_pair(newX, newY);
}

bool WYDNPC::isValidPosition(uint16_t x, uint16_t y) const
{
    // Simplificado: verificar se está dentro dos limites de perambulação
    float dx = static_cast<float>(x) - wanderCenterX_;
    float dy = static_cast<float>(y) - wanderCenterY_;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    return dist <= wanderRadius_;
    
    // Na implementação real, também verificaria colisões, terreno, etc.
}

bool WYDNPC::startInteraction(uint32_t playerId)
{
    if (playerId == 0) {
        return false;
    }
    
    // Verificar se já existe interação
    if (activeInteractions_.find(playerId) != activeInteractions_.end()) {
        return true; // Já existe, considerado sucesso
    }
    
    // Iniciar nova interação
    PlayerInteraction interaction;
    interaction.playerId = playerId;
    interaction.currentDialogue = 1; // Diálogo inicial
    interaction.interactionStart = static_cast<uint64_t>(std::time(nullptr)) * 1000;
    interaction.lastInteraction = interaction.interactionStart;
    
    activeInteractions_[playerId] = interaction;
    
    return true;
}

void WYDNPC::endInteraction(uint32_t playerId)
{

} // namespace wydbr

    activeInteractions_.erase(playerId);
}