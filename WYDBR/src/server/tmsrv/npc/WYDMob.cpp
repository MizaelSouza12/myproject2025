/**
 * WYDMob.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/WYDMob.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/npc/WYDMob.h"
#include "src/server/tmsrv/npc/NPCManager.h"
#include "src/server/tmsrv/combat/CombatSystem.h"
#include <algorithm>
#include <ctime>
#include <cmath>
#include <sstream>
#include <random>

namespace wydbr {


WYDMob::WYDMob(uint32_t uniqueId, uint16_t mobId, const std::string& name)
    : WYDEntity(uniqueId, name, EntityType::MOB)
    , manager_(nullptr)
    , mobId_(mobId)
    , level_(1)
    , maxHP_(100)
    , hp_(100)
    , maxMP_(0)
    , mp_(0)
    , strength_(10)
    , dexterity_(10)
    , intelligence_(10)
    , constitution_(10)
    , attack_(10)
    , defense_(10)
    , behavior_(MobBehavior::PASSIVE)
    , aggroRange_(0)
    , spawnX_(0)
    , spawnY_(0)
    , moveSpeed_(0)
    , attackSpeed_(0)
    , attackType_(MobAttackType::MELEE)
    , currentPatrolPoint_(0)
    , lastSkillUse_(0)
    , experience_(0)
    , gold_(0)
    , inCombat_(false)
    , lastCombatAction_(0)
    , currentTargetId_(0)
    , lastMoveTime_(0)
    , lastAttackTime_(0)
    , lastAggroCheck_(0)
    , lastRoamingCheck_(0)
{
    // Definir flags padrã/**
 * setFlag
 * 
 * Implementa a funcionalidade setFlag conforme especificação original.
 * @param EntityFlag::ATTACKABLE Parâmetro EntityFlag::ATTACKABLE
 * @param true Parâmetro true
 * @return Retorna o
 */
o
    setFlag(EntityFlag::ATTACKABLE, true);
}

WYDMob::~WYDMob()
{
    // Limpar recursos
}

void WYDMob::initialize(NPCManager* manager)
{
    manager_ = manager;
    
    // Posição inicial é a de spawn
    spawnX_ = getPosX();
    spawnY_ = getPosY();
}

void WYDMob::update(uint64_t timestamp)
{
    std::lock_guard<std::mutex> guard(mobMutex_);
    
    // Não atualizar se estiver morto
    if (getState() == EntityState::DEAD) {
        return;
    }
    
    // Atualizações perió/**
 * updateCombat
 * 
 * Implementa a funcionalidade updateCombat conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @return Retorna dicas
 */
dicas
    updateCombat(timestamp);
    updateMovement(timestamp);
    updateAggro(timestamp);
    
    // Verificar se precisa voltar ao spawn (muito longe)
    if (!inCombat_ && timestamp - lastCombatAction_ > 10000) { // 10s após o /**
 * checkReturnToSpawn
 * 
 * Implementa a funcionalidade checkReturnToSpawn conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @return Retorna combate
 */
 combate
        checkReturnToSpawn(timestamp);
    }
    
    // Regeneração /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 5000 Parâmetro 5000
 * @return Retorna natural
 */
 natural
    if(timestamp - lastCombatAction_ > 5000){ // 5s após o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param maxHP_ Parâmetro maxHP_
 * @return Retorna combate
 */
 combate
        if(hp_ < maxHP_){
            hp_ = std::min(maxHP_, hp_ + maxHP_ / 100); // 1% de regen por tick
        }
        if (mp_ < maxMP_) {
            mp_ = std::min(maxMP_, mp_ + maxMP_ / 50);  // 2% de regen por tick
        }
    }
    
    // Atualização de roaming se não estiver em /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 3000 Parâmetro 3000
 * @return Retorna combate
 */
 combate
    if(!inCombat_ && timestamp - lastRoamingCheck_ > 3000){ // 3s entre checks de /**
 * updateRoaming
 * 
 * Implementa a funcionalidade updateRoaming conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @return Retorna roaming
 */
 roaming
        updateRoaming(timestamp);
        lastRoamingCheck_ = timestamp;
    }
}

std::string WYDMob::serialize() const
{
    // Na implementação real, isso serializaria o mob para o cliente
    // Aqui, vamos retornar uma string de exemplo para fins de estrutura
    std::stringstream ss;
    ss << "MOB:" << uniqueId_ << ":" << mobId_ << ":" << name_ << ":"
       << static_cast<int>(state_) << ":" << mapId_ << ":" << posX_ << ":" << posY_ << ":"
       << direction_ << ":" << hp_ << ":" << maxHP_ << ":" << level_;
    return ss.str();
}

bool WYDMob::isBoss() const
{
    /**
 * hasFlag
 * 
 * Implementa a funcionalidade hasFlag conforme especificação original.
 * @param EntityFlag::BOSS Parâmetro EntityFlag::BOSS
 * @return Retorna return
 */

    return hasFlag(EntityFlag::BOSS);
}

void WYDMob::setAttributes(uint16_t level, uint32_t hp, uint32_t mp, 
                         uint16_t str, uint16_t dex, uint16_t intel, uint16_t con,
                         uint16_t attack, uint16_t defense)
{
    level_ = level;
    maxHP_ = hp;
    hp_ = hp;
    maxMP_ = mp;
    mp_ = mp;
    strength_ = str;
    dexterity_ = dex;
    intelligence_ = intel;
    constitution_ = con;
    attack_ = attack;
    defense_ = defense;
}

void WYDMob::setPatrolPoints(const std::vector<std::pair<uint16_t, uint16_t>>& points)
{
    patrolPoints_ = points;
    currentPatrolPoint_ = 0;
}

void WYDMob::setSkills(const std::vector<uint16_t>& skills)
{
    skills_ = skills;
}

void WYDMob::setDropItems(const std::vector<uint16_t>& items, const std::vector<uint8_t>& rates)
{
    // Validar que os vetores têm o mesmo tamanho
    if (items.size() != rates.size()) {
        return;
    }
    
    dropItems_ = items;
    dropRates_ = rates;
}

void WYDMob::setExperienceAndGold(uint32_t exp, uint32_t gold)
{
    experience_ = exp;
    gold_ = gold;
}

bool WYDMob::takeDamage(uint32_t damage, uint32_t attackerId)
{
    // Aplicar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param hp_ Parâmetro hp_
 * @return Retorna dano
 */
 dano
    if(damage >= hp_){
        hp_ = 0;
        setState(EntityState::DEAD);
        inCombat_ = false;
        
        // Limpar path de movimento
        movementPath_.clear();
        
        // Processar morte (loot, experiência, etc.)
        if (manager_) {
            // Na implementação completa, chamaria uma função do manager
            // para processar a morte do mob e gerar drops
        }
        
        return true; // Morreu
    }
    
    hp_ -= damage;
    
    // Adicionar atacante à lista de /**
 * addToAggroList
 * 
 * Implementa a funcionalidade addToAggroList conforme especificação original.
 * @param attackerId Parâmetro attackerId
 * @param damage Parâmetro damage
 * @return Retorna aggro
 */
 aggro
    addToAggroList(attackerId, damage);
    
    // Entrar em combate
    inCombat_ = true;
    lastCombatAction_ = static_cast<uint64_t>(std::time(nullptr)) * 1000;
    
    // Se for a primeira vez tomando dano e for passivo, mudar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna comportamento
 */
 comportamento
    if(behavior_ == MobBehavior::PASSIVE && currentTargetId_ == 0){
        currentTargetId_ = attackerId;
    }
    
    return false; // Continua vivo
}

bool WYDMob::useSkill(uint16_t skillId, uint32_t targetId)
{
    // Encontrar a skill nos skills disponíveis
    if (std::find(skills_.begin(), skills_.end(), skillId) == skills_.end()) {
        return false; // Skill não encontrada
    }
    
    // Verificar cooldown
    uint64_t now = static_cast<uint64_t>(std::time(nullptr)) * 1000;
    if (now - lastSkillUse_ < 3000) { // 3s de cooldown geral
        return false;
    }
    
    // Usar skill (na implementação real, aplicaria efeitos)
    lastSkillUse_ = now;
    
    // Atualizar alvo se /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna fornecido
 */
 fornecido
    if(targetId != 0){
        currentTargetId_ = targetId;
    }
    
    return true;
}

bool WYDMob::moveToPosition(uint16_t x, uint16_t y)
{
    // Limpar caminho atual
    movementPath_.clear();
    
    // Adicionar o destino como único ponto do caminho
    movementPath_.push_back(std::make_pair(x, y));
    
    // Atualizar /**
 * setState
 * 
 * Implementa a funcionalidade setState conforme especificação original.
 * @param EntityState::MOVING Parâmetro EntityState::MOVING
 * @return Retorna estado
 */
 estado
    setState(EntityState::MOVING);
    
    return true;
}

bool WYDMob::attackTarget(uint32_t targetId)
{
    // Verificar cooldown de ataque
    uint64_t now = static_cast<uint64_t>(std::time(nullptr)) * 1000;
    if (now - lastAttackTime_ < (1000 / std::max(1, static_cast<int>(attackSpeed_)))) {
        return false;
    }
    
    // Atacar (na implementação real, calcularia dano, etc.)
    lastAttackTime_ = now;
    currentTargetId_ = targetId;
    inCombat_ = true;
    lastCombatAction_ = now;
    
    // Mudar /**
 * setState
 * 
 * Implementa a funcionalidade setState conforme especificação original.
 * @param EntityState::ATTACKING Parâmetro EntityState::ATTACKING
 * @return Retorna estado
 */
 estado
    setState(EntityState::ATTACKING);
    
    return true;
}

void WYDMob::addToAggroList(uint32_t entityId, uint32_t aggroAmount)
{
    if (entityId == 0) {
        return;
    }
    
    aggroList_[entityId] += aggroAmount;
    
    // Atualizar alvo com maior aggro
    uint32_t highestAggro = 0;
    uint32_t highestAggroTarget = 0;
    
    for (const auto& entry : aggroList_) {
        if (entry.second > highestAggro) {
            highestAggro = entry.second;
            highestAggroTarget = entry.first;
        }
    }
    
    if (highestAggroTarget != 0) {
        currentTargetId_ = highestAggroTarget;
    }
}

void WYDMob::clearAggroList()
{
    aggroList_.clear();
    currentTargetId_ = 0;
    inCombat_ = false;
}

void WYDMob::setHP(uint32_t hp)
{
    hp_ = std::min(hp, maxHP_);
    
    if (hp_ == 0) {
        setState(EntityState::DEAD);
        inCombat_ = false;
    }
}

void WYDMob::setMP(uint32_t mp)
{
    mp_ = std::min(mp, maxMP_);
}

void WYDMob::modifyHP(int32_t amount)
{
    if (amount < 0) {
        // Dano
        if (static_cast<uint32_t>(-amount) >= hp_) {
            hp_ = 0;
            setState(EntityState::DEAD);
            inCombat_ = false;
        } else {
            hp_ -= static_cast<uint32_t>(-amount);
        }
    } else {
        // Cura
        hp_ = std::min(maxHP_, hp_ + static_cast<uint32_t>(amount));
    }
}

void WYDMob::modifyMP(int32_t amount)
{
    if (amount < 0) {
        // Consumo
        if (static_cast<uint32_t>(-amount) >= mp_) {
            mp_ = 0;
        } else {
            mp_ -= static_cast<uint32_t>(-amount);
        }
    } else {
        // Recuperação
        mp_ = std::min(maxMP_, mp_ + static_cast<uint32_t>(amount));
    }
}

std::vector<uint32_t> WYDMob::generateDrops()
{
    std::vector<uint32_t> drops;
    
    // Gerar drops com base nas taxas
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    
    for (size_t i = 0; i < dropItems_.size(); ++i) {
        if (dis(gen) <= dropRates_[i]) {
            // Na implementação real, criaria o item e retornaria seu ID único
            drops.push_back(dropItems_[i]);
        }
    }
    
    return drops;
}

void WYDMob::updateCombat(uint64_t timestamp)
{
    if (!inCombat_ || currentTargetId_ == 0) {
        return;
    }
    
    // Verificar timeout de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 15000 Parâmetro 15000
 * @return Retorna combate
 */
 combate
    if(timestamp - lastCombatAction_ > 15000){ // 15s sem ação
        inCombat_ = false;
        currentTargetId_ = 0;
        return;
    }
    
    // Verificar se alvo existe e está ao alcance
    // Na implementação real, verificaria com o manager
    
    // Verificar cooldown de ataque
    if (timestamp - lastAttackTime_ >= (1000 / std::max(1, static_cast<int>(attackSpeed_)))) {
        // Tentar usar skill
        uint16_t skillId = 0;
        if (shouldUseSkill(timestamp, skillId)) {
            useSkill(skillId, currentTargetId_);
        } else {
            // Ataque /**
 * attackTarget
 * 
 * Implementa a funcionalidade attackTarget conforme especificação original.
 * @param currentTargetId_ Parâmetro currentTargetId_
 * @return Retorna normal
 */
 normal
            attackTarget(currentTargetId_);
        }
    }
}

void WYDMob::updateMovement(uint64_t timestamp)
{
    if (getState() == EntityState::DEAD) {
        return;
    }
    
    // Se não há caminho a seguir ou está atacando, não move
    if (movementPath_.empty() || getState() == EntityState::ATTACKING) {
        if (getState() == EntityState::MOVING) {
            setState(EntityState::IDLE);
        }
        return;
    }
    
    // Verificar cooldown de movimento
    if (timestamp - lastMoveTime_ < (1000 / std::max(1, static_cast<int>(moveSpeed_)))) {
        return;
    }
    
    // Mover para o próximo ponto
    const auto& nextPoint = movementPath_.front();
    
    // Atualizar posiçã/**
 * setPosition
 * 
 * Implementa a funcionalidade setPosition conforme especificação original.
 * @param nextPoint.first Parâmetro nextPoint.first
 * @param nextPoint.second Parâmetro nextPoint.second
 * @return Retorna o
 */
o
    setPosition(nextPoint.first, nextPoint.second);
    
    // Atualizar direção (simplificado)
    // Na implementação real, calcularia a direção com base no movimento
    
    // Remover ponto do caminho
    movementPath_.pop_front();
    
    // Atualizar timestamp
    lastMoveTime_ = timestamp;
    
    // Atualizar estado
    if (movementPath_.empty()) {
        setState(EntityState::IDLE);
    } else {
        setState(EntityState::MOVING);
    }
}

void WYDMob::updateAggro(uint64_t timestamp)
{
    if (!inCombat_ || timestamp - lastAggroCheck_ < 2000) { // 2s entre checks
        return;
    }
    
    lastAggroCheck_ = timestamp;
    
    // Em uma implementação real, verificaria entidades próximas
    // e as adicionaria à lista de aggro se necessário
    
    // Também atualizaria o alvo atual com base na maior ameaça
    if (!aggroList_.empty()) {
        currentTargetId_ = getTopAggroTarget();
    }
}

void WYDMob::updateRoaming(uint64_t timestamp)
{
    // Se estiver em combate ou já movendo, não iniciar novo movimento
    if (inCombat_ || getState() == EntityState::MOVING) {
        return;
    }
    
    // Chance de movimento aleatório
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disMoveChance(1, 100);
    
    if (disMoveChance(gen) <= 20) { // 20% de chance de mover
        // Decidir para onde mover com base no comportamento
        if (behavior_ == MobBehavior::PATROL && !patrolPoints_.empty()) {
            // Usar pontos de patrulha
            const auto& point = patrolPoints_[currentPatrolPoint_];
            moveToPosition(point.first, point.second);
            
            // Avançar para o próximo ponto
            currentPatrolPoint_ = (currentPatrolPoint_ + 1) % patrolPoints_.size();
        } else {
            // Movimento aleatório próximo ao spawn
            std::uniform_int_distribution<> disX(spawnX_ - wanderRange_, spawnX_ + wanderRange_);
            std::uniform_int_distribution<> disY(spawnY_ - wanderRange_, spawnY_ + wanderRange_);
            
            uint16_t newX = disX(gen);
            uint16_t newY = disY(gen);
            
            // Verificar se posição é válida (simplificado)
            // Em uma implementação real, verificaria colisões, etc.
            moveToPosition(newX, newY);
        }
    }
}

void WYDMob::checkReturnToSpawn(uint64_t timestamp)
{
    // Calcular distância ao spawn
    float distToSpawn = getDistanceToSpawn();
    
    // Se muito longe, retornar ao /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param wanderRange_ Parâmetro wanderRange_
 * @return Retorna spawn
 */
 spawn
    if(distToSpawn > 2 * wanderRange_){
        moveToPosition(spawnX_, spawnY_);
    }
}

uint32_t WYDMob::getTopAggroTarget()
{
    uint32_t highestAggro = 0;
    uint32_t highestAggroTarget = 0;
    
    for (const auto& entry : aggroList_) {
        if (entry.second > highestAggro) {
            highestAggro = entry.second;
            highestAggroTarget = entry.first;
        }
    }
    
    return highestAggroTarget;
}

bool WYDMob::shouldUseSkill(uint64_t timestamp, uint16_t& outSkillId)
{
    // Se não tem skills, não pode usar
    if (skills_.empty()) {
        return false;
    }
    
    // Verificar cooldown global de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 3000 Parâmetro 3000
 * @return Retorna skills
 */
 skills
    if(timestamp - lastSkillUse_ < 3000){ // 3s
        return false;
    }
    
    // Chance de usar skill
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disSkillChance(1, 100);
    
    if (disSkillChance(gen) <= 30) { // 30% de chance
        // Escolher skill aleatória
        std::uniform_int_distribution<> disSkill(0, skills_.size() - 1);
        outSkillId = skills_[disSkill(gen)];
        return true;
    }
    
    return false;
}

float WYDMob::getDistanceToSpawn() const
{

} // namespace wydbr

    int dx = static_cast<int>(posX_) - static_cast<int>(spawnX_);
    int dy = static_cast<int>(posY_) - static_cast<int>(spawnY_);
    return std::sqrt(dx * dx + dy * dy);
}