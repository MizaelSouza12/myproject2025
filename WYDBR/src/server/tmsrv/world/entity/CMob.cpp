/**
 * CMob.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/entity/CMob.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "CMob.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>

#include "../../../../core/Logger.h"
#include "../../../../core/Utils.h"
#include "../WorldManager.h"
#include "../generator/MobTemplate.h"
#include "../spawner/MobGroup.h"
#include "MobAI.h"

namespace wydbr {


/**
 * @file CMob.cpp
 * @brief Implementação da classe base para mobs
 * 
 * Este arquivo contém a implementação da classe base para mobs, que
 * representa todos os NPCs e monstros controlados pelo sistema.
 */

namespace wyd {

} // namespace wydbr

namespace server {
namespace world {
namespace entity {

CMob::CMob(WORD mobID, const std::string& name, BYTE level)
    : CEntity(name, BASE_MOB_ID + mobID)
    , mobID_(mobID)
    , level_(level)
    , mapID_(0)
    , posX_(0)
    , posY_(0)
    , alive_(false)
    , currentHP_(100)
    , maxHP_(100)
    , currentMP_(0)
    , maxMP_(0)
    , str_(10)
    , dex_(10)
    , int_(10)
    , con_(10)
    , moveSpeed_(5)
    , attackSpeed_(10)
    , physicalDamageMin_(5)
    , physicalDamageMax_(10)
    , magicDamageMin_(0)
    , magicDamageMax_(0)
    , physicalDefense_(0)
    , magicDefense_(0)
    , criticalRate_(5)
    , evasionRate_(0)
    , attackRange_(2)
    , sightRange_(DEFAULT_SIGHT_RANGE)
    , size_(1)
    , subtitle_("")
    , race_(MobRace::NONE)
    , class_(MobClass::NONE)
    , movementType_(MovementType::RANDOM)
    , aggroType_(AggroType::PASSIVE)
    , lootType_(LootType::INDIVIDUAL)
    , modelID_(0)
    , dropTableID_(0)
    , experience_(10)
    , gold_(0)
    , flags_(0)
    , spawnRadius_(0)
    , respawnTime_(0)
    , deathTime_(0)
    , spawnStatus_(SpawnedEntityStatus::NONE)
    , worldEventID_(0)
    , lastMoveTime_(0)
    , lastAttackTime_(0)
    , lastSkillTime_(0)
    , animState_(battle::AnimState::IDLE)
    , moveType_(battle::MoveType::WALK)
    , template_(nullptr)
    , group_(nullptr)
    , target_(nullptr)
    , nextCallbackID_(1)
{
    // Zera as defesas /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna elementais
 */
 elementais
    for(int i = 0; i < 5; i++){
        elementalDefense_[i] = 0;
    }
    
    // Zera as /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param skills_ Parâmetro skills_
 * @return Retorna skills
 */
 skills
    for(auto& skill : skills_){
        skill = 0;
    }
}

CMob::~CMob() {
    // Limpa referências
    target_ = nullptr;
    template_ = nullptr;
    group_ = nullptr;
    
    // Limpa tabela de ameaças
    {
        std::lock_guard<std::mutex> lock(threatMutex_);
        threatTable_.clear();
    }
    
    // Limpa efeitos
    {
        std::lock_guard<std::mutex> lock(effectsMutex_);
        effects_.clear();
    }
    
    // Limpa callbacks
    {
        std::lock_guard<std::mutex> lock(callbacksMutex_);
        eventCallbacks_.clear();
    }
    
    // Notifica que foi destruí/**
 * NotifyEventCallbacks
 * 
 * Implementa a funcionalidade NotifyEventCallbacks conforme especificação original.
 * @param EntityEvent::DESPAWN Parâmetro EntityEvent::DESPAWN
 * @param nullptr Parâmetro nullptr
 * @return Retorna do
 */
do
    NotifyEventCallbacks(EntityEvent::DESPAWN, nullptr);
}

BYTE CMob::GetEntityType() const {
    // No WYD original, isso era determinado por flags na struct de MOB
    if (IsBoss()) {
        return static_cast<BYTE>(EntityType::BOSS);
    } else if (IsNPC()) {
        return static_cast<BYTE>(EntityType::NPC);
    } else {
        return static_cast<BYTE>(EntityType::MOB);
    }
}

bool CMob::IsAlive() const {
    return alive_ && currentHP_ > 0;
}

BYTE CMob::GetLevel() const {
    return level_;
}

WORD CMob::GetPosX() const {
    return posX_;
}

WORD CMob::GetPosY() const {
    return posY_;
}

WORD CMob::GetMapID() const {
    return mapID_;
}

bool CMob::ApplyDamage(int damage, CEntity* attacker, WORD skillID) {
    if (!alive_ || currentHP_ <= 0) {
        return false;
    }
    
    // Aplica o dano
    currentHP_ -= damage;
    
    // Verifica se morreu
    bool died = (currentHP_ <= 0);
    
    if (died) {
        currentHP_ = 0;
        alive_ = false;
        
        // Processa a /**
 * ProcessDeath
 * 
 * Implementa a funcionalidade ProcessDeath conforme especificação original.
 * @param attacker Parâmetro attacker
 * @return Retorna morte
 */
 morte
        ProcessDeath(attacker);
    }
    
    // Notifica o dano
    struct DamageInfo {
        int damage;
        DWORD attackerID;
        WORD skillID;
    } damageInfo = {
        damage,
        attacker ? attacker->GetID() : 0,
        skillID
    };
    
    NotifyEventCallbacks(EntityEvent::DAMAGE, &damageInfo);
    
    // Se o mob não estava em combate, ativa o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param AggroType::NEUTRAL Parâmetro AggroType::NEUTRAL
 * @return Retorna AI
 */
 AI
    if(attacker && aggroType_ != AggroType::PASSIVE && aggroType_ != AggroType::NEUTRAL){
        // Adiciona ameaça ao /**
 * AddThreat
 * 
 * Implementa a funcionalidade AddThreat conforme especificação original.
 * @param attacker Parâmetro attacker
 * @param damage Parâmetro damage
 * @return Retorna atacante
 */
 atacante
        AddThreat(attacker, damage);
        
        // Se não tem alvo, define o atacante como /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !target_ Parâmetro !target_
 * @return Retorna alvo
 */
 alvo
        if(!target_){
            SetTarget(attacker);
        }
        
        // Notifica evento de agressã/**
 * NotifyEventCallbacks
 * 
 * Implementa a funcionalidade NotifyEventCallbacks conforme especificação original.
 * @param EntityEvent::AGGRO Parâmetro EntityEvent::AGGRO
 * @param attacker Parâmetro attacker
 * @return Retorna o
 */
o
        NotifyEventCallbacks(EntityEvent::AGGRO, attacker);
    }
    
    return died;
}

int CMob::ApplyHealing(int amount, CEntity* healer, WORD skillID) {
    if (!alive_) {
        return 0;
    }
    
    // Calcula a quantidade efetiva de cura
    int effectiveHeal = std::min(amount, maxHP_ - currentHP_);
    
    // Aplica a cura
    currentHP_ += effectiveHeal;
    
    // Garante que não exceda o máximo
    currentHP_ = std::min(currentHP_, maxHP_);
    
    // Notifica a cura
    struct HealInfo {
        int amount;
        DWORD healerID;
        WORD skillID;
    } healInfo = {
        effectiveHeal,
        healer ? healer->GetID() : 0,
        skillID
    };
    
    NotifyEventCallbacks(EntityEvent::HEAL, &healInfo);
    
    return effectiveHeal;
}

bool CMob::ApplyEffect(battle::AffectType affectType, BYTE value, DWORD duration, CEntity* applier) {
    // Verifica se já tem o efeito
    {
        std::lock_guard<std::mutex> lock(effectsMutex_);
        
        // Procura o efeito
        auto it = std::find_if(effects_.begin(), effects_.end(), [affectType](const Effect& effect) {
            return effect.type == affectType;
        });
        
        if (it != effects_.end()) {
            // Atualiza o efeito existente (só atualiza se o novo for mais forte)
            if (value > it->value) {
                it->value = value;
                it->timestamp = static_cast<DWORD>(GetTickCount64());
                it->duration = duration;
                it->entityID = applier ? applier->GetID() : 0;
            }
        } else {
            // Adiciona novo efeito
            Effect effect;
            effect.type = affectType;
            effect.value = value;
            effect.timestamp = static_cast<DWORD>(GetTickCount64());
            effect.duration = duration;
            effect.entityID = applier ? applier->GetID() : 0;
            
            effects_.push_back(effect);
        }
    }
    
    // Notifica o efeito
    struct EffectInfo {
        battle::AffectType type;
        BYTE value;
        DWORD duration;
        DWORD applierID;
    } effectInfo = {
        affectType,
        value,
        duration,
        applier ? applier->GetID() : 0
    };
    
    NotifyEventCallbacks(EntityEvent::DEBUFF, &effectInfo);
    
    return true;
}

bool CMob::RemoveEffect(battle::AffectType affectType) {
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    // Procura o efeito
    auto it = std::find_if(effects_.begin(), effects_.end(), [affectType](const Effect& effect) {
        return effect.type == affectType;
    });
    
    if (it != effects_.end()) {
        // Remove o efeito
        effects_.erase(it);
        return true;
    }
    
    return false;
}

bool CMob::HasEffect(battle::AffectType affectType) const {
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    // Procura o efeito
    auto it = std::find_if(effects_.begin(), effects_.end(), [affectType](const Effect& effect) {
        return effect.type == affectType;
    });
    
    return (it != effects_.end());
}

BYTE CMob::GetEffectValue(battle::AffectType affectType) const {
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    // Procura o efeito
    auto it = std::find_if(effects_.begin(), effects_.end(), [affectType](const Effect& effect) {
        return effect.type == affectType;
    });
    
    if (it != effects_.end()) {
        return it->value;
    }
    
    return 0;
}

DWORD CMob::GetEffectTimeRemaining(battle::AffectType affectType) const {
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    // Procura o efeito
    auto it = std::find_if(effects_.begin(), effects_.end(), [affectType](const Effect& effect) {
        return effect.type == affectType;
    });
    
    if (it != effects_.end()) {
        DWORD currentTime = static_cast<DWORD>(GetTickCount64());
        DWORD elapsedTime = currentTime - it->timestamp;
        
        // Se a duração for 0, é um efeito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna permanente
 */
 permanente
        if(it->duration == 0){
            return 0xFFFFFFFF;
        }
        
        // Se já passou da duração, retorna /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param it->duration Parâmetro it->duration
 * @return Retorna 0
 */
 0
        if(elapsedTime >= it->duration){
            return 0;
        }
        
        // Retorna o tempo restante
        return it->duration - elapsedTime;
    }
    
    return 0;
}

void CMob::UpdateEffects(DWORD elapsed) {
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    DWORD currentTime = static_cast<DWORD>(GetTickCount64());
    
    // Atualiza cada efeito
    auto it = effects_.begin();
    while (it != effects_.end()) {
        // Verifica se o efeito expirou (duração 0 é permanente)
        if (it->duration > 0 && currentTime - it->timestamp >= it->duration) {
            // Remove o efeito
            it = effects_.erase(it);
        } else {
            // Avança para o próximo
            ++it;
        }
    }
}

battle::BattleAttributes CMob::GetBattleAttributes() const {
    battle::BattleAttributes attributes;
    
    // No WYD original, isso não existia como uma função, mas era calculado
    // diretamente onde necessário. Aqui centralizamos para melhor manutenção.
    
    // Atributos básicos
    attributes.Level = level_;
    attributes.Str = str_;
    attributes.Dex = dex_;
    attributes.Int = int_;
    attributes.Con = con_;
    
    // Atributos de ataque
    attributes.AttackRating = dex_ * 2 + level_ * 2;
    attributes.PhysicalDamageMin = physicalDamageMin_;
    attributes.PhysicalDamageMax = physicalDamageMax_;
    attributes.MagicDamageMin = magicDamageMin_;
    attributes.MagicDamageMax = magicDamageMax_;
    
    // Atributos de defesa
    attributes.Evasion = dex_ + level_;
    attributes.PhysicalDefense = physicalDefense_;
    attributes.MagicDefense = magicDefense_;
    
    // Resistências /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna elementais
 */
 elementais
    for(int i = 0; i < 5; i++){
        attributes.ElementalResist[i] = elementalDefense_[i];
    }
    
    // Outros atributos
    attributes.CriticalChance = criticalRate_;
    attributes.BlockChance = 0;
    attributes.MoveSpeed = moveSpeed_;
    attributes.AttackSpeed = attackSpeed_;
    attributes.CooldownReduction = 0;
    attributes.DamageReduction = 0;
    attributes.Reflect = 0;
    attributes.AbsorbHP = 0;
    
    // Aplica modificadores de efeitos
    std::lock_guard<std::mutex> lock(effectsMutex_);
    for (const auto& effect : effects_) {
        // Aplica o efeito aos atributos de acordo com o /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param effect.type Parâmetro effect.type
 * @return Retorna tipo
 */
 tipo
        switch(effect.type){
            case battle::AffectType::STUN:
                // Stun impede movimento e ataques, mas não afeta atributos
                break;
                
            case battle::AffectType::SLOW:
                // Slow reduz velocidade de movimento
                attributes.MoveSpeed = static_cast<int>(attributes.MoveSpeed * (1.0f - effect.value / 100.0f));
                break;
                
            case battle::AffectType::ROOT:
                // Root impede movimento
                attributes.MoveSpeed = 0;
                break;
                
            case battle::AffectType::SILENCE:
                // Silence impede uso de skills, mas não afeta atributos
                break;
                
            case battle::AffectType::BLIND:
                // Blind reduz precisão
                attributes.AttackRating = static_cast<int>(attributes.AttackRating * (1.0f - effect.value / 100.0f));
                break;
                
            case battle::AffectType::WEAKEN:
                // Weaken reduz dano físico
                attributes.PhysicalDamageMin = static_cast<int>(attributes.PhysicalDamageMin * (1.0f - effect.value / 100.0f));
                attributes.PhysicalDamageMax = static_cast<int>(attributes.PhysicalDamageMax * (1.0f - effect.value / 100.0f));
                break;
                
            case battle::AffectType::CURSE:
                // Curse reduz defesa
                attributes.PhysicalDefense = static_cast<int>(attributes.PhysicalDefense * (1.0f - effect.value / 100.0f));
                attributes.MagicDefense = static_cast<int>(attributes.MagicDefense * (1.0f - effect.value / 100.0f));
                break;
                
            case battle::AffectType::POISON:
                // Poison causa dano periódico, mas não afeta atributos
                break;
                
            case battle::AffectType::BLEED:
                // Bleed causa dano periódico, mas não afeta atributos
                break;
                
            case battle::AffectType::BURN:
                // Burn causa dano periódico, mas não afeta atributos
                break;
                
            case battle::AffectType::FREEZE:
                // Freeze impede movimento e ataques
                attributes.MoveSpeed = 0;
                attributes.AttackSpeed = 0;
                break;
                
            case battle::AffectType::SHOCK:
                // Shock causa periodicamente stun, mas não afeta atributos diretamente
                break;
                
            case battle::AffectType::DIVINE_PROTECTION:
                // Divine Protection aumenta defesa
                attributes.PhysicalDefense += effect.value;
                attributes.MagicDefense += effect.value;
                break;
                
            default:
                // Outros efeitos não afetam atributos diretamente
                break;
        }
    }
    
    return attributes;
}

int CMob::GetHP() const {
    return currentHP_;
}

int CMob::GetMaxHP() const {
    return maxHP_;
}

int CMob::GetMP() const {
    return currentMP_;
}

int CMob::GetMaxMP() const {
    return maxMP_;
}

int CMob::GetStr() const {
    return str_;
}

int CMob::GetInt() const {
    return int_;
}

int CMob::GetDex() const {
    return dex_;
}

int CMob::GetCon() const {
    return con_;
}

int CMob::GetMoveSpeed() const {
    return moveSpeed_;
}

int CMob::GetAttackSpeed() const {
    return attackSpeed_;
}

int CMob::GetPhysicalDamageMin() const {
    return physicalDamageMin_;
}

int CMob::GetPhysicalDamageMax() const {
    return physicalDamageMax_;
}

int CMob::GetMagicDamageMin() const {
    return magicDamageMin_;
}

int CMob::GetMagicDamageMax() const {
    return magicDamageMax_;
}

int CMob::GetPhysicalDefense() const {
    return physicalDefense_;
}

int CMob::GetMagicDefense() const {
    return magicDefense_;
}

int CMob::GetCriticalRate() const {
    return criticalRate_;
}

int CMob::GetEvasionRate() const {
    return evasionRate_;
}

int CMob::GetElementalDefense(int element) const {
    if (element >= 0 && element < 5) {
        return elementalDefense_[element];
    }
    return 0;
}

int CMob::GetAttackRange() const {
    return attackRange_;
}

void CMob::SetHP(int hp) {
    currentHP_ = std::max(0, std::min(hp, maxHP_));
    
    // Atualiza o status de vida
    alive_ = (currentHP_ > 0);
}

void CMob::SetMP(int mp) {
    currentMP_ = std::max(0, std::min(mp, maxMP_));
}

void CMob::SetPosition(WORD x, WORD y, WORD mapID) {
    posX_ = x;
    posY_ = y;
    
    if (mapID > 0) {
        mapID_ = mapID;
    }
}

battle::AnimState CMob::GetAnimState() const {
    return animState_;
}

void CMob::SetAnimState(battle::AnimState state) {
    animState_ = state;
}

battle::MoveType CMob::GetMoveType() const {
    return moveType_;
}

void CMob::SetMoveType(battle::MoveType type) {
    moveType_ = type;
}

void CMob::ProcessDeath(CEntity* killer) {
    if (!alive_) {
        return;
    }
    
    // Marca como morto
    alive_ = false;
    currentHP_ = 0;
    
    // Salva o tempo de morte
    deathTime_ = static_cast<DWORD>(GetTickCount64());
    
    // Atualiza o status de spawn
    spawnStatus_ = SpawnedEntityStatus::DEAD;
    
    // Notifica o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param group_ Parâmetro group_
 * @return Retorna grupo
 */
 grupo
    if(group_){
        group_->NotifyEntityDeath(this);
    }
    
    // Notifica o evento de morte
    struct DeathInfo {
        DWORD killerID;
    } deathInfo = {
        killer ? killer->GetID() : 0
    };
    
    NotifyEventCallbacks(EntityEvent::DEATH, &deathInfo);
    
    // Limpa o alvo e a tabela de ameaç/**
 * ClearTarget
 * 
 * Implementa a funcionalidade ClearTarget conforme especificação original.
 * @return Retorna as
 */
as
    ClearTarget();
    ClearThreat();
    
    // Invoca ao morrer, se tiver a flag
    if (SummonsOnDeath() && template_ && template_->wSummonID > 0) {
        // TODO: Implementar invocação
    }
}

bool CMob::ProcessResurrection(CEntity* resurrecter, int percentHP, int percentMP) {
    if (alive_) {
        return false;
    }
    
    // Calcula o HP e MP
    int newHP = static_cast<int>(maxHP_ * (percentHP / 100.0f));
    int newMP = static_cast<int>(maxMP_ * (percentMP / 100.0f));
    
    // Define os novos valores
    currentHP_ = std::max(1, newHP);
    currentMP_ = std::max(0, newMP);
    
    // Marca como vivo
    alive_ = true;
    
    // Atualiza o status de spawn
    spawnStatus_ = SpawnedEntityStatus::ALIVE;
    
    // Notifica o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param group_ Parâmetro group_
 * @return Retorna grupo
 */
 grupo
    if(group_){
        group_->NotifyEntityRespawn(this);
    }
    
    // Notifica o evento de ressurreição
    struct ResurrectInfo {
        DWORD resurrecter_id;
        int percentHP;
        int percentMP;
    } resurrectInfo = {
        resurrecter ? resurrecter->GetID() : 0,
        percentHP,
        percentMP
    };
    
    NotifyEventCallbacks(EntityEvent::RESPAWN, &resurrectInfo);
    
    return true;
}

void CMob::Update(DWORD elapsed) {
    if (!alive_) {
        // Verifica se é hora de respawn
        DWORD currentTime = static_cast<DWORD>(GetTickCount64());
        if (respawnTime_ > 0 && IsTimeToRespawn(currentTime)) {
            // Ressuscita o /**
 * ProcessResurrection
 * 
 * Implementa a funcionalidade ProcessResurrection conforme especificação original.
 * @param nullptr Parâmetro nullptr
 * @param 100 Parâmetro 100
 * @param 100 Parâmetro 100
 * @return Retorna mob
 */
 mob
            ProcessResurrection(nullptr, 100, 100);
        }
        return;
    }
    
    // Atualiza /**
 * UpdateEffects
 * 
 * Implementa a funcionalidade UpdateEffects conforme especificação original.
 * @param elapsed Parâmetro elapsed
 * @return Retorna efeitos
 */
 efeitos
    UpdateEffects(elapsed);
    
    // Atualiza /**
 * UpdateMovement
 * 
 * Implementa a funcionalidade UpdateMovement conforme especificação original.
 * @param elapsed Parâmetro elapsed
 * @return Retorna movimento
 */
 movimento
    UpdateMovement(elapsed);
    
    // Atualiza /**
 * UpdateCombat
 * 
 * Implementa a funcionalidade UpdateCombat conforme especificação original.
 * @param elapsed Parâmetro elapsed
 * @return Retorna combate
 */
 combate
    UpdateCombat(elapsed);
    
    // Atualiza /**
 * UpdateAI
 * 
 * Implementa a funcionalidade UpdateAI conforme especificação original.
 * @param elapsed Parâmetro elapsed
 * @return Retorna AI
 */
 AI
    UpdateAI(elapsed);
}

WORD CMob::GetMobID() const {
    return mobID_;
}

const MobTemplate* CMob::GetTemplate() const {
    return template_;
}

void CMob::SetTemplate(const MobTemplate* templ) {
    template_ = templ;
    
    if (template_) {
        // Carrega dados do /**
 * LoadFromTemplate
 * 
 * Implementa a funcionalidade LoadFromTemplate conforme especificação original.
 * @param template_ Parâmetro template_
 * @return Retorna template
 */
 template
        LoadFromTemplate(template_);
    }
}

bool CMob::IsNPC() const {
    // No WYD original, isso era determinado por uma flag na struct MOB
    // Aqui usamos o tipo de classe para /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param AggroType::NEUTRAL Parâmetro AggroType::NEUTRAL
 * @return Retorna determinar
 */
 determinar
    return(class_ == MobClass::NONE && aggroType_ == AggroType::NEUTRAL);
}

bool CMob::IsBoss() const {
    // No WYD original, isso era determinado por uma flag na struct MOB
    return (byIsBoss == 1) || (class_ == MobClass::BOSS);
}

bool CMob::IsElite() const {
    // No WYD original, isso era determinado por uma flag na struct MOB
    return (byIsElite == 1) || (class_ == MobClass::ELITE);
}

bool CMob::IsAggressive() const {
    // No WYD original, isso era determinado por uma flag na struct MOB
    return (byIsAggressive == 1) || 
           (aggroType_ == AggroType::AGGRESSIVE) || 
           (aggroType_ == AggroType::FRENZIED);
}

MovementType CMob::GetMovementType() const {
    return movementType_;
}

void CMob::SetMovementType(MovementType type) {
    movementType_ = type;
}

AggroType CMob::GetAggroType() const {
    return aggroType_;
}

void CMob::SetAggroType(AggroType type) {
    aggroType_ = type;
}

WORD CMob::GetSightRange() const {
    return sightRange_;
}

void CMob::SetSightRange(WORD range) {
    sightRange_ = range;
}

MobRace CMob::GetRace() const {
    return race_;
}

MobClass CMob::GetClass() const {
    return class_;
}

bool CMob::CanBeMounted() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna MOB
 */
 MOB
    return(byNoBarding == 0);
}

bool CMob::AcceptsReflect() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna MOB
 */
 MOB
    return(byNoReflect == 0);
}

bool CMob::CanBeTamed() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna MOB
 */
 MOB
    return(byNoTaming == 0);
}

bool CMob::AppearInDungeons() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna MOB
 */
 MOB
    return(byNoDungeon == 0);
}

bool CMob::CanBeKnockedBack() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna MOB
 */
 MOB
    return(byNoKnockback == 0);
}

bool CMob::RunsOnLowHealth() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 1 Parâmetro 1
 * @return Retorna MOB
 */
 MOB
    return(byRunOnLowHealth == 1);
}

bool CMob::CallsForHelp() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 1 Parâmetro 1
 * @return Retorna MOB
 */
 MOB
    return(byCallForHelp == 1);
}

bool CMob::SummonsOnDeath() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 1 Parâmetro 1
 * @return Retorna MOB
 */
 MOB
    return(bySummonOnDeath == 1);
}

bool CMob::GivesExperience() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna MOB
 */
 MOB
    return(byNoExp == 0);
}

bool CMob::DropsItems() const {
    // No WYD original, isso era determinado por uma flag na struct /**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna MOB
 */
 MOB
    return(byNoItem == 0);
}

WORD CMob::GetDropTableID() const {
    return dropTableID_;
}

BYTE CMob::GetSize() const {
    return size_;
}

void CMob::SetGroup(CMobGroup* group) {
    group_ = group;
}

CMobGroup* CMob::GetGroup() const {
    return group_;
}

bool CMob::SetTarget(CEntity* entity, int initialThreat) {
    if (!entity) {
        ClearTarget();
        return false;
    }
    
    // Define o alvo
    target_ = entity;
    
    // Adiciona ameaç/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna a
 */
a
    if(initialThreat > 0){
        AddThreat(entity, initialThreat);
    }
    
    // Notifica o evento de agressã/**
 * NotifyEventCallbacks
 * 
 * Implementa a funcionalidade NotifyEventCallbacks conforme especificação original.
 * @param EntityEvent::AGGRO Parâmetro EntityEvent::AGGRO
 * @param entity Parâmetro entity
 * @return Retorna o
 */
o
    NotifyEventCallbacks(EntityEvent::AGGRO, entity);
    
    return true;
}

CEntity* CMob::GetTarget() const {
    return target_;
}

void CMob::ClearTarget() {
    target_ = nullptr;
}

bool CMob::AddThreat(CEntity* entity, int amount) {
    if (!entity) {
        return false;
    }
    
    DWORD entityID = entity->GetID();
    
    std::lock_guard<std::mutex> lock(threatMutex_);
    
    // Adiciona ou atualiza a ameaça
    threatTable_[entityID] += amount;
    
    return true;
}

bool CMob::RemoveThreat(CEntity* entity, int amount) {
    if (!entity) {
        return false;
    }
    
    DWORD entityID = entity->GetID();
    
    std::lock_guard<std::mutex> lock(threatMutex_);
    
    // Procura a entidade na tabela
    auto it = threatTable_.find(entityID);
    if (it != threatTable_.end()) {
        // Remove a ameaça
        it->second -= amount;
        
        // Remove da tabela se a ameaça for <= /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna 0
 */
 0
        if(it->second <= 0){
            threatTable_.erase(it);
        }
        
        return true;
    }
    
    return false;
}

void CMob::ClearThreat() {
    std::lock_guard<std::mutex> lock(threatMutex_);
    threatTable_.clear();
}

CEntity* CMob::GetHighestThreatEntity() const {
    std::lock_guard<std::mutex> lock(threatMutex_);
    
    // Procura a entidade com maior ameaça
    DWORD maxThreatID = 0;
    int maxThreat = 0;
    
    for (const auto& entry : threatTable_) {
        if (entry.second > maxThreat) {
            maxThreat = entry.second;
            maxThreatID = entry.first;
        }
    }
    
    // Procura a entidade com ID /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna correspondente
 */
 correspondente
    if(maxThreatID > 0){
        // TODO: Implementar busca de entidade por ID
        // return g_WorldManager.GetEntity(maxThreatID);
    }
    
    return nullptr;
}

int CMob::GetThreatLevel(CEntity* entity) const {
    if (!entity) {
        return 0;
    }
    
    DWORD entityID = entity->GetID();
    
    std::lock_guard<std::mutex> lock(threatMutex_);
    
    // Procura a entidade na tabela
    auto it = threatTable_.find(entityID);
    if (it != threatTable_.end()) {
        return it->second;
    }
    
    return 0;
}

bool CMob::IsTimeToRespawn(DWORD currentTime) const {
    if (alive_ || respawnTime_ == 0 || deathTime_ == 0) {
        return false;
    }
    
    return (currentTime - deathTime_ >= respawnTime_);
}

void CMob::SetRespawnTime(DWORD respawnTime) {
    respawnTime_ = respawnTime;
}

DWORD CMob::GetRespawnTime() const {
    return respawnTime_;
}

std::string CMob::GetSubtitle() const {
    return subtitle_;
}

void CMob::SetSubtitle(const std::string& subtitle) {
    subtitle_ = subtitle;
}

WORD CMob::GetModelID() const {
    return modelID_;
}

void CMob::SetModelID(WORD modelID) {
    modelID_ = modelID;
}

SpawnedEntityStatus CMob::GetSpawnStatus() const {
    return spawnStatus_;
}

void CMob::SetSpawnStatus(SpawnedEntityStatus status) {
    spawnStatus_ = status;
}

DWORD CMob::GetExperience() const {
    return experience_;
}

DWORD CMob::GetGold() const {
    return gold_;
}

std::array<WORD, 4> CMob::GetSkills() const {
    return skills_;
}

bool CMob::HasSkill(WORD skillID) const {
    return (skills_[0] == skillID || skills_[1] == skillID || 
            skills_[2] == skillID || skills_[3] == skillID);
}

bool CMob::CanUseSkill(WORD skillID) const {
    // Verifica se tem a skill
    if (!HasSkill(skillID)) {
        return false;
    }
    
    // Verifica se está /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !alive_ Parâmetro !alive_
 * @return Retorna vivo
 */
 vivo
    if(!alive_){
        return false;
    }
    
    // Verifica se está silenciado
    if (HasEffect(battle::AffectType::SILENCE)) {
        return false;
    }
    
    // Verifica tempo de recarga
    DWORD currentTime = static_cast<DWORD>(GetTickCount64());
    if (currentTime - lastSkillTime_ < 3000) { // 3 segundos de recarga
        return false;
    }
    
    return true;
}

bool CMob::UseSkill(WORD skillID, CEntity* target) {
    // Verifica se pode usar a skill
    if (!CanUseSkill(skillID)) {
        return false;
    }
    
    // Verifica se tem /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !target Parâmetro !target
 * @return Retorna alvo
 */
 alvo
    if(!target){
        return false;
    }
    
    // TODO: Implementar uso de skill
    
    // Atualiza o timestamp
    lastSkillTime_ = static_cast<DWORD>(GetTickCount64());
    
    // Notifica o evento
    struct SkillInfo {
        WORD skillID;
        DWORD targetID;
    } skillInfo = {
        skillID,
        target->GetID()
    };
    
    NotifyEventCallbacks(EntityEvent::SKILL, &skillInfo);
    
    return true;
}

void CMob::SetAI(std::shared_ptr<CMobAI> ai) {
    ai_ = ai;
}

std::shared_ptr<CMobAI> CMob::GetAI() const {
    return ai_;
}

bool CMob::ProcessEvent(EntityEvent event, void* param) {
    NotifyEventCallbacks(event, param);
    
    // Processa o evento de acordo com o /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param event Parâmetro event
 * @return Retorna tipo
 */
 tipo
    switch(event){
        case EntityEvent::SPAWN:
            // Mob spawnou
            alive_ = true;
            spawnStatus_ = SpawnedEntityStatus::ALIVE;
            currentHP_ = maxHP_;
            currentMP_ = maxMP_;
            break;
            
        case EntityEvent::DEATH:
            // Mob /**
 * ProcessDeath
 * 
 * Implementa a funcionalidade ProcessDeath conforme especificação original.
 * @param nullptr Parâmetro nullptr
 * @return Retorna morreu
 */
 morreu
            ProcessDeath(nullptr);
            break;
            
        case EntityEvent::RESPAWN:
            // Mob /**
 * ProcessResurrection
 * 
 * Implementa a funcionalidade ProcessResurrection conforme especificação original.
 * @param nullptr Parâmetro nullptr
 * @param 100 Parâmetro 100
 * @param 100 Parâmetro 100
 * @return Retorna respawnou
 */
 respawnou
            ProcessResurrection(nullptr, 100, 100);
            break;
            
        case EntityEvent::DESPAWN:
            // Mob despawnou
            alive_ = false;
            spawnStatus_ = SpawnedEntityStatus::DESPAWNING;
            break;
            
        case EntityEvent::AGGRO:
            // Mob adquiriu /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param param Parâmetro param
 * @return Retorna aggro
 */
 aggro
            if(param){
                CEntity* entity = static_cast<CEntity*>(param);
                SetTarget(entity);
            }
            break;
            
        default:
            // Outros eventos
            break;
    }
    
    return true;
}

int CMob::RegisterEventCallback(EntityEventCallback callback) {
    if (!callback) {
        return -1;
    }
    
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    
    int callbackID = nextCallbackID_++;
    eventCallbacks_[callbackID] = callback;
    
    return callbackID;
}

bool CMob::UnregisterEventCallback(int callbackID) {
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    
    auto it = eventCallbacks_.find(callbackID);
    if (it != eventCallbacks_.end()) {
        eventCallbacks_.erase(it);
        return true;
    }
    
    return false;
}

void CMob::SetSpawnArea(const SpawnPoint& spawnPoint, WORD radius) {
    spawnPoint_ = spawnPoint;
    spawnRadius_ = radius;
}

const SpawnPoint& CMob::GetSpawnPoint() const {
    return spawnPoint_;
}

void CMob::SetWorldEvent(WORD eventID) {
    worldEventID_ = eventID;
}

WORD CMob::GetWorldEvent() const {
    return worldEventID_;
}

bool CMob::IsInWorldEvent() const {
    return (worldEventID_ > 0);
}

void CMob::LoadFromTemplate(const MobTemplate* templ) {
    if (!templ) {
        return;
    }
    
    // Carrega atributos básicos
    level_ = templ->byLevel;
    subtitle_ = templ->szSubtitle;
    race_ = templ->eRace;
    class_ = templ->eClass;
    size_ = templ->bySize;
    
    // Carrega atributos de batalha
    str_ = templ->byStr;
    dex_ = templ->byDex;
    int_ = templ->byInt;
    con_ = templ->byCon;
    maxHP_ = templ->dwMaxHP;
    maxMP_ = templ->dwMaxMP;
    currentHP_ = maxHP_;
    currentMP_ = maxMP_;
    moveSpeed_ = templ->wMoveSpeed;
    attackSpeed_ = templ->wAttackSpeed;
    sightRange_ = templ->wSightRange;
    attackRange_ = templ->wAttackRange;
    
    // Carrega atributos de dano
    physicalDamageMin_ = templ->wAttackPower / 2;
    physicalDamageMax_ = templ->wAttackPower;
    magicDamageMin_ = templ->wMagicPower / 2;
    magicDamageMax_ = templ->wMagicPower;
    
    // Carrega atributos de defesa
    physicalDefense_ = templ->wDefense;
    magicDefense_ = templ->wResistance;
    elementalDefense_[0] = templ->byResistFire;
    elementalDefense_[1] = templ->byResistIce;
    elementalDefense_[2] = templ->byResistLightning;
    elementalDefense_[3] = templ->byResistHoly;
    elementalDefense_[4] = templ->byResistDark;
    
    // Carrega informações de comportamento
    movementType_ = templ->eMoveType;
    aggroType_ = templ->eAggroType;
    lootType_ = templ->eLootType;
    modelID_ = templ->wModelID;
    
    // Carrega skills
    skills_[0] = templ->wSkillID1;
    skills_[1] = templ->wSkillID2;
    skills_[2] = templ->wSkillID3;
    skills_[3] = templ->wSkillID4;
    
    // Carrega tabela de drops
    dropTableID_ = templ->wDropTableID;
    experience_ = templ->dwExperience;
    gold_ = templ->dwGold;
    
    // Carrega flags
    byIsElite = templ->byIsElite;
    byIsBoss = templ->byIsBoss;
    byIsAggressive = templ->byIsAggressive;
    byNoBarding = templ->byNoBarding;
    byNoReflect = templ->byNoReflect;
    byNoTaming = templ->byNoTaming;
    byNoDungeon = templ->byNoDungeon;
    byNoKnockback = templ->byNoKnockback;
    byRunOnLowHealth = templ->byRunOnLowHealth;
    byCallForHelp = templ->byCallForHelp;
    bySummonOnDeath = templ->bySummonOnDeath;
    byNoExp = templ->byNoExp;
    byNoItem = templ->byNoItem;
    byRespawnGroup = templ->byRespawnGroup;
    byMoveArea = templ->byMoveArea;
    
    // Calcula atributos /**
 * CalculateDerivedStats
 * 
 * Implementa a funcionalidade CalculateDerivedStats conforme especificação original.
 * @return Retorna derivados
 */
 derivados
    CalculateDerivedStats();
}

void CMob::CalculateDerivedStats() {
    // No WYD original, alguns atributos eram calculados a partir de outros
    
    // Crítico baseado em destreza
    criticalRate_ = static_cast<int>(dex_ * 0.25f);
    
    // Evasão baseada em destreza e nível
    evasionRate_ = static_cast<int>(dex_ * 0.5f + level_ * 0.5f);
    
    // Ajustes baseados em /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param class_ Parâmetro class_
 * @return Retorna classe
 */
 classe
    switch(class_){
        case MobClass::MELEE:
            // Mobs corpo a corpo: mais dano físico, menos dano mágico
            physicalDamageMin_ = static_cast<int>(physicalDamageMin_ * 1.2f);
            physicalDamageMax_ = static_cast<int>(physicalDamageMax_ * 1.2f);
            magicDamageMin_ = static_cast<int>(magicDamageMin_ * 0.8f);
            magicDamageMax_ = static_cast<int>(magicDamageMax_ * 0.8f);
            break;
            
        case MobClass::RANGED:
            // Mobs à distância: mais precisão, menos defesa
            dex_ = static_cast<int>(dex_ * 1.2f);
            physicalDefense_ = static_cast<int>(physicalDefense_ * 0.9f);
            break;
            
        case MobClass::MAGIC:
            // Mobs mágicos: mais dano mágico, menos dano físico
            magicDamageMin_ = static_cast<int>(magicDamageMin_ * 1.3f);
            magicDamageMax_ = static_cast<int>(magicDamageMax_ * 1.3f);
            physicalDamageMin_ = static_cast<int>(physicalDamageMin_ * 0.7f);
            physicalDamageMax_ = static_cast<int>(physicalDamageMax_ * 0.7f);
            break;
            
        case MobClass::TANK:
            // Mobs tanque: mais defesa, menos dano
            physicalDefense_ = static_cast<int>(physicalDefense_ * 1.3f);
            magicDefense_ = static_cast<int>(magicDefense_ * 1.3f);
            physicalDamageMin_ = static_cast<int>(physicalDamageMin_ * 0.8f);
            physicalDamageMax_ = static_cast<int>(physicalDamageMax_ * 0.8f);
            break;
            
        case MobClass::ELITE:
            // Mobs elite: tudo melhorado
            physicalDamageMin_ = static_cast<int>(physicalDamageMin_ * 1.2f);
            physicalDamageMax_ = static_cast<int>(physicalDamageMax_ * 1.2f);
            magicDamageMin_ = static_cast<int>(magicDamageMin_ * 1.2f);
            magicDamageMax_ = static_cast<int>(magicDamageMax_ * 1.2f);
            physicalDefense_ = static_cast<int>(physicalDefense_ * 1.2f);
            magicDefense_ = static_cast<int>(magicDefense_ * 1.2f);
            break;
            
        case MobClass::BOSS:
            // Chefes: tudo muito melhorado
            physicalDamageMin_ = static_cast<int>(physicalDamageMin_ * 1.5f);
            physicalDamageMax_ = static_cast<int>(physicalDamageMax_ * 1.5f);
            magicDamageMin_ = static_cast<int>(magicDamageMin_ * 1.5f);
            magicDamageMax_ = static_cast<int>(magicDamageMax_ * 1.5f);
            physicalDefense_ = static_cast<int>(physicalDefense_ * 1.5f);
            magicDefense_ = static_cast<int>(magicDefense_ * 1.5f);
            criticalRate_ = static_cast<int>(criticalRate_ * 1.5f);
            maxHP_ = static_cast<int>(maxHP_ * 2.0f);
            maxMP_ = static_cast<int>(maxMP_ * 2.0f);
            break;
            
        default:
            // Outros tipos não têm ajustes específicos
            break;
    }
    
    // Ajustes baseados em raç/**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param race_ Parâmetro race_
 * @return Retorna a
 */
a
    switch(race_){
        case MobRace::BEAST:
            // Bestas: mais ataque, menos defesa
            physicalDamageMin_ = static_cast<int>(physicalDamageMin_ * 1.1f);
            physicalDamageMax_ = static_cast<int>(physicalDamageMax_ * 1.1f);
            physicalDefense_ = static_cast<int>(physicalDefense_ * 0.9f);
            break;
            
        case MobRace::UNDEAD:
            // Mortos-vivos: mais resistência, menos velocidade
            elementalDefense_[3] = static_cast<int>(elementalDefense_[3] * 0.7f); // Fraqueza ao sagrado
            moveSpeed_ = static_cast<int>(moveSpeed_ * 0.9f);
            break;
            
        case MobRace::DEMON:
            // Demônios: mais dano mágico, fraqueza ao sagrado
            magicDamageMin_ = static_cast<int>(magicDamageMin_ * 1.1f);
            magicDamageMax_ = static_cast<int>(magicDamageMax_ * 1.1f);
            elementalDefense_[3] = static_cast<int>(elementalDefense_[3] * 0.7f); // Fraqueza ao sagrado
            break;
            
        case MobRace::HUMANOID:
            // Humanóides: mais balanceados
            break;
            
        case MobRace::ELEMENTAL:
            // Elementais: mais resistências elementais, menos defesa fí/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna sica
 */
sica
            for(int i = 0; i < 5; i++){
                elementalDefense_[i] = static_cast<int>(elementalDefense_[i] * 1.2f);
            }
            physicalDefense_ = static_cast<int>(physicalDefense_ * 0.9f);
            break;
            
        default:
            // Outras raças não têm ajustes específicos
            break;
    }
    
    // Cuida de flags /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1 Parâmetro 1
 * @return Retorna especiais
 */
 especiais
    if(byIsElite == 1){
        maxHP_ = static_cast<int>(maxHP_ * 1.3f);
        physicalDamageMin_ = static_cast<int>(physicalDamageMin_ * 1.2f);
        physicalDamageMax_ = static_cast<int>(physicalDamageMax_ * 1.2f);
        physicalDefense_ = static_cast<int>(physicalDefense_ * 1.1f);
        magicDefense_ = static_cast<int>(magicDefense_ * 1.1f);
    }
    
    if (byIsBoss == 1) {
        maxHP_ = static_cast<int>(maxHP_ * 2.0f);
        physicalDamageMin_ = static_cast<int>(physicalDamageMin_ * 1.5f);
        physicalDamageMax_ = static_cast<int>(physicalDamageMax_ * 1.5f);
        physicalDefense_ = static_cast<int>(physicalDefense_ * 1.3f);
        magicDefense_ = static_cast<int>(magicDefense_ * 1.3f);
    }
    
    // Atualiza HP e MP atuais
    currentHP_ = maxHP_;
    currentMP_ = maxMP_;
}

void CMob::UpdateMovement(DWORD elapsed) {
    // No WYD original, o movimento dos mobs era gerenciado pelo servidor
    // de forma muito rudimentar. Aqui implementamos um sistema mais robusto.
    
    // Verifica se pode se mover
    if (HasEffect(battle::AffectType::STUN) || 
        HasEffect(battle::AffectType::ROOT) || 
        HasEffect(battle::AffectType::FREEZE)) {
        return;
    }
    
    // TODO: Implementar lógica de movimento de acordo com o tipo
    
    DWORD currentTime = static_cast<DWORD>(GetTickCount64());
    
    // Apenas move se passou tempo suficiente desde o último /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1000 Parâmetro 1000
 * @return Retorna movimento
 */
 movimento
    if(currentTime - lastMoveTime_ < 1000){ // 1 segundo
        return;
    }
    
    // Atualiza o timestamp
    lastMoveTime_ = currentTime;
    
    // Se tem AI, deixa o AI controlar o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ai_ Parâmetro ai_
 * @return Retorna movimento
 */
 movimento
    if(ai_){
        // A AI controlará o movimento
        return;
    }
    
    // Lógica básica de movimento de acordo com o /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param movementType_ Parâmetro movementType_
 * @return Retorna tipo
 */
 tipo
    switch(movementType_){
        case MovementType::NONE:
        case MovementType::STATIC:
            // Não se move
            break;
            
        case MovementType::RANDOM: {
            // Movimento aleatório dentro da área de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna spawn
 */
 spawn
            if(spawnRadius_ > 0){
                // Se está fora da área de spawn ou se há uma chance aleatória, move para um ponto na área
                std::uniform_int_distribution<int> moveChance(0, 100);
                if (moveChance(rng_) < 20) { // 20% de chance de se mover
                    // Escolhe um ponto aleatório na área de spawn
                    std::uniform_int_distribution<int> offsetDist(-static_cast<int>(spawnRadius_), static_cast<int>(spawnRadius_));
                    WORD newX = std::max(0, std::min(static_cast<int>(spawnPoint_.wPosX + offsetDist(rng_)), MAX_WORLD_RANGE));
                    WORD newY = std::max(0, std::min(static_cast<int>(spawnPoint_.wPosY + offsetDist(rng_)), MAX_WORLD_RANGE));
                    
                    // Move para o novo /**
 * SetPosition
 * 
 * Implementa a funcionalidade SetPosition conforme especificação original.
 * @param newX Parâmetro newX
 * @param newY Parâmetro newY
 * @return Retorna ponto
 */
 ponto
                    SetPosition(newX, newY);
                    
                    // Notifica o /**
 * NotifyEventCallbacks
 * 
 * Implementa a funcionalidade NotifyEventCallbacks conforme especificação original.
 * @param EntityEvent::MOVE Parâmetro EntityEvent::MOVE
 * @param nullptr Parâmetro nullptr
 * @return Retorna movimento
 */
 movimento
                    NotifyEventCallbacks(EntityEvent::MOVE, nullptr);
                }
            }
            break;
        }
            
        case MovementType::CHASE: {
            // Persegue o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param AggroType::NEUTRAL Parâmetro AggroType::NEUTRAL
 * @return Retorna alvo
 */
 alvo
            if(target_ && aggroType_ != AggroType::PASSIVE && aggroType_ != AggroType::NEUTRAL){
                // Calcula a direção para o alvo
                WORD targetX = target_->GetPosX();
                WORD targetY = target_->GetPosY();
                
                // Verifica se está no alcance de ataque
                int dx = static_cast<int>(targetX) - static_cast<int>(posX_);
                int dy = static_cast<int>(targetY) - static_cast<int>(posY_);
                int distSq = dx * dx + dy * dy;
                
                if (distSq > attackRange_ * attackRange_) {
                    // Calcula a nova posição (move em direção ao alvo)
                    int distance = static_cast<int>(sqrt(distSq));
                    float moveStepX = dx * moveSpeed_ / distance;
                    float moveStepY = dy * moveSpeed_ / distance;
                    
                    WORD newX = static_cast<WORD>(posX_ + moveStepX);
                    WORD newY = static_cast<WORD>(posY_ + moveStepY);
                    
                    // Move para a nova posiçã/**
 * SetPosition
 * 
 * Implementa a funcionalidade SetPosition conforme especificação original.
 * @param newX Parâmetro newX
 * @param newY Parâmetro newY
 * @return Retorna o
 */
o
                    SetPosition(newX, newY);
                    
                    // Notifica o /**
 * NotifyEventCallbacks
 * 
 * Implementa a funcionalidade NotifyEventCallbacks conforme especificação original.
 * @param EntityEvent::MOVE Parâmetro EntityEvent::MOVE
 * @param nullptr Parâmetro nullptr
 * @return Retorna movimento
 */
 movimento
                    NotifyEventCallbacks(EntityEvent::MOVE, nullptr);
                }
            }
            break;
        }
            
        default:
            // Outros tipos de movimento seriam implementados aqui
            break;
    }
}

void CMob::UpdateCombat(DWORD elapsed) {
    // No WYD original, o combate dos mobs era gerenciado pelo servidor
    // de forma muito rudimentar. Aqui implementamos um sistema mais robusto.
    
    // Verifica se pode atacar
    if (!alive_ || HasEffect(battle::AffectType::STUN) || HasEffect(battle::AffectType::FREEZE)) {
        return;
    }
    
    // Verifica se tem alvo
    if (!target_ || !target_->IsAlive()) {
        ClearTarget();
        return;
    }
    
    // Verifica se o alvo está no mesmo mapa
    if (target_->GetMapID() != mapID_) {
        ClearTarget();
        return;
    }
    
    // Verifica se o alvo está no alcance de visão
    WORD targetX = target_->GetPosX();
    WORD targetY = target_->GetPosY();
    
    int dx = static_cast<int>(targetX) - static_cast<int>(posX_);
    int dy = static_cast<int>(targetY) - static_cast<int>(posY_);
    int distSq = dx * dx + dy * dy;
    
    if (distSq > sightRange_ * sightRange_) {
        ClearTarget();
        return;
    }
    
    // Verifica se pode atacar (alcance e tempo)
    DWORD currentTime = static_cast<DWORD>(GetTickCount64());
    
    if (distSq <= attackRange_ * attackRange_ && currentTime - lastAttackTime_ >= 3000) { // 3 segundos
        // Atualiza o timestamp
        lastAttackTime_ = currentTime;
        
        // Ataca o /**
 * Attack
 * 
 * Implementa a funcionalidade Attack conforme especificação original.
 * @param target_ Parâmetro target_
 * @return Retorna alvo
 */
 alvo
        Attack(target_);
        
        // Notifica o ataque
        struct AttackInfo {
            DWORD targetID;
        } attackInfo = {
            target_->GetID()
        };
        
        NotifyEventCallbacks(EntityEvent::ATTACK, &attackInfo);
    }
}

void CMob::UpdateAI(DWORD elapsed) {
    // Se tem AI, deixa o AI /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ai_ Parâmetro ai_
 * @return Retorna atualizar
 */
 atualizar
    if(ai_){
        ai_->Update(this, elapsed);
    }
}

void CMob::NotifyEventCallbacks(EntityEvent event, const void* param) {
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    
    for (const auto& entry : eventCallbacks_) {
        entry.second(event, GetID(), param);
    }
}

} // namespace entity
} // namespace world
} // namespace server
} // namespace wyd