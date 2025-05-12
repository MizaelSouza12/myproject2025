/**
 * BattleManager.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/battle/BattleManager.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "BattleManager.h"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <fstream>
#include <iostream>

#include "../../../core/Logger.h"
#include "../../../core/Utils.h"

namespace wydbr {


/**
 * @file BattleManager.cpp
 * @brief Implementação do gerenciador de combate
 * 
 * Este arquivo contém a implementação do gerenciador de combate, que implementa
 * as fórmulas de dano, efeitos, e lógica de combate do WYD original.
 */

namespace wyd {

} // namespace wydbr

namespace server {

// Forward declarations
class CPlayer;
class CMob;
class CEntity;

CBattleManager& CBattleManager::GetInstance() {
    static CBattleManager instance;
    return instance;
}

CBattleManager::CBattleManager()
    : nextCallbackID_(1)
    , initialized_(false)
{
    // Inicializa o gerador de números aleatórios com uma semente baseada no tempo
    unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    rng_.seed(seed);
}

CBattleManager::~CBattleManager() {
    Shutdown();
}

bool CBattleManager::Initialize() {
    if (initialized_) {
        LOG_WARNING("BattleManager já inicializado");
        return true;
    }
    
    LOG_INFO("Inicializando BattleManager...");
    
    // No WYD original, as skills eram carregadas de arquivos .bin
    // Aqui, vamos simular isso carregando de um arquivo de texto para facilitar o desenvolvimento
    // Em produção, isso seria um parser de arquivo binário
    
    // Carrega as definições de /**
 * LoadSkillDefinitions
 * 
 * Implementa a funcionalidade LoadSkillDefinitions conforme especificação original.
 * @return Retorna skills
 */
 skills
    LoadSkillDefinitions();
    
    initialized_ = true;
    LOG_INFO("BattleManager inicializado com sucesso");
    
    return true;
}

void CBattleManager::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    LOG_INFO("Desligando BattleManager...");
    
    // Limpa as definições de skills
    skills_.clear();
    
    // Limpa os registros de dano
    {
        std::lock_guard<std::mutex> lock(damageRecordsMutex_);
        damageRecords_.clear();
    }
    
    // Limpa os callbacks
    {
        std::lock_guard<std::mutex> lock(callbacksMutex_);
        battleEventCallbacks_.clear();
    }
    
    initialized_ = false;
    LOG_INFO("BattleManager desligado");
}

// Implementação do carregamento de definições de skills
void CBattleManager::LoadSkillDefinitions() {
    // Esta é uma implementação simplificada para desenvolvimento
    // No WYD original, isso carregaria um arquivo binário com as definições de todas as skills
    
    // Vamos simular o carregamento de algumas skills básicas
    
    // Skill 1: Ataque Poderoso (TK)
    STRUCT_SKILL_DATA powerAttack;
    powerAttack.wSkillIndex = 1;
    strncpy(powerAttack.szName, "Ataque Poderoso");
    strncpy(powerAttack.szDescription, "Um ataque físico poderoso contra um inimigo.");
    powerAttack.bySkillType = static_cast<BYTE>(SkillType::ATTACK);
    powerAttack.byTargetType = static_cast<BYTE>(TargetType::ONE_ENEMY);
    powerAttack.byVFX = static_cast<BYTE>(SkillVFX::NONE);
    powerAttack.byDamageType = static_cast<BYTE>(DamageType::PHYSICAL);
    powerAttack.byLevel = 1;
    powerAttack.byMaxLevel = 10;
    powerAttack.byLevelInterval = 6;
    powerAttack.byClass = static_cast<BYTE>(SkillClass::TK);
    powerAttack.wMPCost = 15;
    powerAttack.wHPCost = 0;
    powerAttack.wRange = 1;
    powerAttack.wArea = 0;
    powerAttack.wCastTime = 200;
    powerAttack.wCooldown = 3000;
    powerAttack.wDuration = 0;
    powerAttack.wAnimationDelay = 800;
    powerAttack.wDamageBase = 150;
    powerAttack.wDamagePerLevel = 20;
    powerAttack.wValueBase = 0;
    powerAttack.wValuePerLevel = 0;
    powerAttack.bySuccessRate = 100;
    powerAttack.bySuccessPerLevel = 0;
    powerAttack.byControlType = static_cast<BYTE>(ControlType::NONE);
    powerAttack.byControlValue = 0;
    skills_[1] = powerAttack;
    
    // Skill 2: Bola de Fogo (FM)
    STRUCT_SKILL_DATA fireball;
    fireball.wSkillIndex = 2;
    strncpy(fireball.szName, "Bola de Fogo");
    strncpy(fireball.szDescription, "Conjura uma bola de fogo que causa dano mágico.");
    fireball.bySkillType = static_cast<BYTE>(SkillType::ATTACK);
    fireball.byTargetType = static_cast<BYTE>(TargetType::ONE_ENEMY);
    fireball.byVFX = static_cast<BYTE>(SkillVFX::FIRE);
    fireball.byDamageType = static_cast<BYTE>(DamageType::FIRE);
    fireball.byLevel = 1;
    fireball.byMaxLevel = 10;
    fireball.byLevelInterval = 6;
    fireball.byClass = static_cast<BYTE>(SkillClass::FM);
    fireball.wMPCost = 20;
    fireball.wHPCost = 0;
    fireball.wRange = 7;
    fireball.wArea = 0;
    fireball.wCastTime = 600;
    fireball.wCooldown = 2000;
    fireball.wDuration = 0;
    fireball.wAnimationDelay = 1000;
    fireball.wDamageBase = 180;
    fireball.wDamagePerLevel = 25;
    fireball.wValueBase = 0;
    fireball.wValuePerLevel = 0;
    fireball.bySuccessRate = 100;
    fireball.bySuccessPerLevel = 0;
    fireball.byControlType = static_cast<BYTE>(ControlType::NONE);
    fireball.byControlValue = 0;
    skills_[2] = fireball;
    
    // Skill 3: Tempestade de Fogo (FM)
    STRUCT_SKILL_DATA firestorm;
    firestorm.wSkillIndex = 3;
    strncpy(firestorm.szName, "Tempestade de Fogo");
    strncpy(firestorm.szDescription, "Conjura uma tempestade de fogo que causa dano em área.");
    firestorm.bySkillType = static_cast<BYTE>(SkillType::ATTACK);
    firestorm.byTargetType = static_cast<BYTE>(TargetType::AREA_ENEMY);
    firestorm.byVFX = static_cast<BYTE>(SkillVFX::FIRE);
    firestorm.byDamageType = static_cast<BYTE>(DamageType::FIRE);
    firestorm.byLevel = 30;
    firestorm.byMaxLevel = 10;
    firestorm.byLevelInterval = 8;
    firestorm.byClass = static_cast<BYTE>(SkillClass::FM);
    firestorm.wMPCost = 50;
    firestorm.wHPCost = 0;
    firestorm.wRange = 7;
    firestorm.wArea = 3;
    firestorm.wCastTime = 1200;
    firestorm.wCooldown = 8000;
    firestorm.wDuration = 0;
    firestorm.wAnimationDelay = 1500;
    firestorm.wDamageBase = 300;
    firestorm.wDamagePerLevel = 40;
    firestorm.wValueBase = 0;
    firestorm.wValuePerLevel = 0;
    firestorm.bySuccessRate = 100;
    firestorm.bySuccessPerLevel = 0;
    firestorm.byControlType = static_cast<BYTE>(ControlType::NONE);
    firestorm.byControlValue = 0;
    skills_[3] = firestorm;
    
    // Skill 4: Golpe Brutal (BM)
    STRUCT_SKILL_DATA brutalStrike;
    brutalStrike.wSkillIndex = 4;
    strncpy(brutalStrike.szName, "Golpe Brutal");
    strncpy(brutalStrike.szDescription, "Um golpe brutal que causa dano físico e atordoa o alvo.");
    brutalStrike.bySkillType = static_cast<BYTE>(SkillType::ATTACK);
    brutalStrike.byTargetType = static_cast<BYTE>(TargetType::ONE_ENEMY);
    brutalStrike.byVFX = static_cast<BYTE>(SkillVFX::EARTH);
    brutalStrike.byDamageType = static_cast<BYTE>(DamageType::PHYSICAL);
    brutalStrike.byLevel = 1;
    brutalStrike.byMaxLevel = 10;
    brutalStrike.byLevelInterval = 6;
    brutalStrike.byClass = static_cast<BYTE>(SkillClass::BM);
    brutalStrike.wMPCost = 15;
    brutalStrike.wHPCost = 0;
    brutalStrike.wRange = 1;
    brutalStrike.wArea = 0;
    brutalStrike.wCastTime = 200;
    brutalStrike.wCooldown = 5000;
    brutalStrike.wDuration = 2000;
    brutalStrike.wAnimationDelay = 800;
    brutalStrike.wDamageBase = 160;
    brutalStrike.wDamagePerLevel = 20;
    brutalStrike.wValueBase = 0;
    brutalStrike.wValuePerLevel = 0;
    brutalStrike.bySuccessRate = 70;
    brutalStrike.bySuccessPerLevel = 3;
    brutalStrike.byControlType = static_cast<BYTE>(ControlType::STUN);
    brutalStrike.byControlValue = 1;
    skills_[4] = brutalStrike;
    
    // Skill 5: Flecha Perfurante (HT)
    STRUCT_SKILL_DATA piercingArrow;
    piercingArrow.wSkillIndex = 5;
    strncpy(piercingArrow.szName, "Flecha Perfurante");
    strncpy(piercingArrow.szDescription, "Uma flecha que perfura a defesa do inimigo.");
    piercingArrow.bySkillType = static_cast<BYTE>(SkillType::ATTACK);
    piercingArrow.byTargetType = static_cast<BYTE>(TargetType::ONE_ENEMY);
    piercingArrow.byVFX = static_cast<BYTE>(SkillVFX::WIND);
    piercingArrow.byDamageType = static_cast<BYTE>(DamageType::RANGED);
    piercingArrow.byLevel = 1;
    piercingArrow.byMaxLevel = 10;
    piercingArrow.byLevelInterval = 6;
    piercingArrow.byClass = static_cast<BYTE>(SkillClass::HT);
    piercingArrow.wMPCost = 15;
    piercingArrow.wHPCost = 0;
    piercingArrow.wRange = 9;
    piercingArrow.wArea = 0;
    piercingArrow.wCastTime = 400;
    piercingArrow.wCooldown = 3000;
    piercingArrow.wDuration = 0;
    piercingArrow.wAnimationDelay = 600;
    piercingArrow.wDamageBase = 170;
    piercingArrow.wDamagePerLevel = 20;
    piercingArrow.wValueBase = 0;
    piercingArrow.wValuePerLevel = 0;
    piercingArrow.bySuccessRate = 100;
    piercingArrow.bySuccessPerLevel = 0;
    piercingArrow.byControlType = static_cast<BYTE>(ControlType::NONE);
    piercingArrow.byControlValue = 0;
    skills_[5] = piercingArrow;
    
    // Skill 6: Cura (FM)
    STRUCT_SKILL_DATA heal;
    heal.wSkillIndex = 6;
    strncpy(heal.szName, "Cura");
    strncpy(heal.szDescription, "Cura o alvo.");
    heal.bySkillType = static_cast<BYTE>(SkillType::HEAL);
    heal.byTargetType = static_cast<BYTE>(TargetType::ONE_ALLY);
    heal.byVFX = static_cast<BYTE>(SkillVFX::HEAL);
    heal.byDamageType = static_cast<BYTE>(DamageType::HEAL);
    heal.byLevel = 1;
    heal.byMaxLevel = 10;
    heal.byLevelInterval = 6;
    heal.byClass = static_cast<BYTE>(SkillClass::FM);
    heal.wMPCost = 30;
    heal.wHPCost = 0;
    heal.wRange = 5;
    heal.wArea = 0;
    heal.wCastTime = 800;
    heal.wCooldown = 3000;
    heal.wDuration = 0;
    heal.wAnimationDelay = 1000;
    heal.wDamageBase = 200;
    heal.wDamagePerLevel = 30;
    heal.wValueBase = 0;
    heal.wValuePerLevel = 0;
    heal.bySuccessRate = 100;
    heal.bySuccessPerLevel = 0;
    heal.byControlType = static_cast<BYTE>(ControlType::NONE);
    heal.byControlValue = 0;
    skills_[6] = heal;
    
    // Skill 7: Proteção Divina (Todas as classes)
    STRUCT_SKILL_DATA divineProtection;
    divineProtection.wSkillIndex = 7;
    strncpy(divineProtection.szName, "Proteção Divina");
    strncpy(divineProtection.szDescription, "Concede proteção contra danos por um tempo.");
    divineProtection.bySkillType = static_cast<BYTE>(SkillType::BUFF);
    divineProtection.byTargetType = static_cast<BYTE>(TargetType::SELF);
    divineProtection.byVFX = static_cast<BYTE>(SkillVFX::HOLY);
    divineProtection.byDamageType = static_cast<BYTE>(DamageType::PHYSICAL);
    divineProtection.byLevel = 50;
    divineProtection.byMaxLevel = 5;
    divineProtection.byLevelInterval = 10;
    divineProtection.byClass = static_cast<BYTE>(SkillClass::ALL);
    divineProtection.wMPCost = 50;
    divineProtection.wHPCost = 0;
    divineProtection.wRange = 0;
    divineProtection.wArea = 0;
    divineProtection.wCastTime = 500;
    divineProtection.wCooldown = 30000;
    divineProtection.wDuration = 10000;
    divineProtection.wAnimationDelay = 1000;
    divineProtection.wDamageBase = 0;
    divineProtection.wDamagePerLevel = 0;
    divineProtection.wValueBase = 20;
    divineProtection.wValuePerLevel = 5;
    divineProtection.bySuccessRate = 100;
    divineProtection.bySuccessPerLevel = 0;
    divineProtection.byControlType = static_cast<BYTE>(ControlType::NONE);
    divineProtection.byControlValue = 0;
    skills_[7] = divineProtection;
    
    LOG_INFO("Carregadas {} definições de skills", skills_.size());
}

BattleResult CBattleManager::ProcessAttack(CEntity* attacker, CEntity* target, WORD skillID) {
    if (!initialized_) {
        LOG_ERROR("BattleManager não inicializado");
        /**
 * BattleResult
 * 
 * Implementa a funcionalidade BattleResult conforme especificação original.
 * @return Retorna return
 */

        return BattleResult();
    }
    
    if (!attacker || !target) {
        LOG_ERROR("Atacante ou alvo inválido");
        /**
 * BattleResult
 * 
 * Implementa a funcionalidade BattleResult conforme especificação original.
 * @return Retorna return
 */

        return BattleResult();
    }
    
    // Verifica se pode atacar
    if (!CanAttack(attacker, target)) {
        BattleResult result;
        result.SetFlag(BattleFlag::IMMUNE);
        result.AttackerID = attacker->GetID();
        result.TargetID = target->GetID();
        result.Timestamp = static_cast<DWORD>(GetTickCount64());
        return result;
    }
    
    // Obtém os atributos de batalha
    BattleAttributes attAttributes = GetBattleAttributes(attacker);
    BattleAttributes defAttributes = GetBattleAttributes(target);
    
    // Verifica se acerta
    bool hits = CheckHit(attacker, target, skillID);
    if (!hits) {
        BattleResult result;
        result.SetFlag(BattleFlag::MISSED);
        result.AttackerID = attacker->GetID();
        result.TargetID = target->GetID();
        result.Timestamp = static_cast<DWORD>(GetTickCount64());
        return result;
    }
    
    // Determina o tipo de dano
    DamageType damageType = DamageType::PHYSICAL;
    if (skillID > 0) {
        const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
        if (skillData) {
            damageType = static_cast<DamageType>(skillData->byDamageType);
        }
    }
    
    // Calcula o dano
    BattleResult result = CalculateDamage(attacker, target, damageType, skillID);
    
    // Registra o dano para distribuição de experiência e /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna drops
 */
 drops
    if(result.Damage > 0){
        AddDamageRecord(target->GetID(), attacker->GetID(), result.Damage);
    }
    
    // Notifica eventos de /**
 * NotifyBattleEvent
 * 
 * Implementa a funcionalidade NotifyBattleEvent conforme especificação original.
 * @param result Parâmetro result
 * @return Retorna batalha
 */
 batalha
    NotifyBattleEvent(result);
    
    return result;
}

std::vector<BattleResult> CBattleManager::ProcessSkill(CEntity* caster, const std::vector<CEntity*>& targets, 
                                                     WORD skillID, BYTE skillLevel) {
    if (!initialized_) {
        LOG_ERROR("BattleManager não inicializado");
        return std::vector<BattleResult>();
    }
    
    if (!caster) {
        LOG_ERROR("Conjurador inválido");
        return std::vector<BattleResult>();
    }
    
    // Verifica se pode usar a skill
    if (!CanUseSkill(caster, skillID, skillLevel)) {
        LOG_ERROR("Não pode usar a skill {}", skillID);
        return std::vector<BattleResult>();
    }
    
    // Obtém os dados da skill
    const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
    if (!skillData) {
        LOG_ERROR("Skill {} não encontrada", skillID);
        return std::vector<BattleResult>();
    }
    
    // Resultados para cada alvo
    std::vector<BattleResult> results;
    
    // Determina o tipo de dano/efeito
    DamageType damageType = static_cast<DamageType>(skillData->byDamageType);
    SkillType skillType = static_cast<SkillType>(skillData->bySkillType);
    
    // Para cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param targets Parâmetro targets
 * @return Retorna alvo
 */
 alvo
    for(CEntity* target : targets){
        if (!target) {
            continue;
        }
        
        // Verifica se pode usar a skill neste alvo
        if (!CanTargetWithSkill(caster, target, skillID)) {
            BattleResult result;
            result.SetFlag(BattleFlag::IMMUNE);
            result.AttackerID = caster->GetID();
            result.TargetID = target->GetID();
            result.Timestamp = static_cast<DWORD>(GetTickCount64());
            result.SkillID = skillID;
            result.IsSkill = true;
            results.push_back(result);
            continue;
        }
        
        // Processa o efeito da skill com base no tipo
        BattleResult result;
        result.AttackerID = caster->GetID();
        result.TargetID = target->GetID();
        result.Timestamp = static_cast<DWORD>(GetTickCount64());
        result.SkillID = skillID;
        result.IsSkill = true;
        
        switch (skillType) {
            case SkillType::ATTACK: {
                // Skills de ataque: calcula o dano
                result = CalculateDamage(caster, target, damageType, skillID, skillLevel);
                
                // Aplica efeitos de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna controle
 */
 controle
                if(skillData->byControlType != 0){
                    ControlType controlType = static_cast<ControlType>(skillData->byControlType);
                    int successRate = skillData->bySuccessRate + (skillData->bySuccessPerLevel * skillLevel);
                    
                    // Lança um dado para ver se o efeito de controle é aplicado
                    std::uniform_int_distribution<> dis(1, 100);
                    int roll = dis(rng_);
                    
                    if (roll <= successRate) {
                        result.SetControlEffect(controlType, skillData->byControlValue);
                    }
                }
                break;
            }
            case SkillType::HEAL: {
                // Skills de cura: calcula a cura
                BattleAttributes attAttributes = GetBattleAttributes(caster);
                BattleAttributes defAttributes = GetBattleAttributes(target);
                
                result = CalculateHealing(attAttributes, defAttributes, skillData, skillLevel);
                break;
            }
            case SkillType::BUFF: {
                // Skills de buff: aplica o efeito positivo
                int value = skillData->wValueBase + (skillData->wValuePerLevel * skillLevel);
                int duration = skillData->wDuration;
                
                // Determina o tipo de buff com base no ID da skill
                AffectType affectType = AffectType::NONE;
                switch (skillID) {
                    case 7: // Proteção Divina
                        affectType = AffectType::DIVINE_PROTECTION;
                        break;
                    // Outros buffs
                    default:
                        affectType = AffectType::NONE;
                        break;
                }
                
                if (affectType != AffectType::NONE) {
                    result.AddEffect(affectType, static_cast<BYTE>(value));
                    result.Damage = 0; // Buffs não causam dano
                }
                break;
            }
            case SkillType::DEBUFF: {
                // Skills de debuff: aplica o efeito negativo
                int value = skillData->wValueBase + (skillData->wValuePerLevel * skillLevel);
                int duration = skillData->wDuration;
                
                // Determina o tipo de debuff com base no ID da skill
                AffectType affectType = AffectType::NONE;
                switch (skillID) {
                    case 4: // Golpe Brutal (stun)
                        affectType = AffectType::STUN;
                        break;
                    // Outros debuffs
                    default:
                        affectType = AffectType::NONE;
                        break;
                }
                
                if (affectType != AffectType::NONE) {
                    result.AddEffect(affectType, static_cast<BYTE>(value));
                }
                break;
            }
            default:
                // Outros tipos de skills
                break;
        }
        
        // Registra o dano para distribuição de experiência e /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna drops
 */
 drops
        if(result.Damage > 0){
            AddDamageRecord(target->GetID(), caster->GetID(), result.Damage);
        }
        
        // Notifica eventos de /**
 * NotifyBattleEvent
 * 
 * Implementa a funcionalidade NotifyBattleEvent conforme especificação original.
 * @param result Parâmetro result
 * @return Retorna batalha
 */
 batalha
        NotifyBattleEvent(result);
        
        results.push_back(result);
    }
    
    return results;
}

BattleResult CBattleManager::CalculateDamage(CEntity* attacker, CEntity* target, DamageType damageType, 
                                           WORD skillID, BYTE skillLevel) {
    if (!initialized_ || !attacker || !target) {
        /**
 * BattleResult
 * 
 * Implementa a funcionalidade BattleResult conforme especificação original.
 * @return Retorna return
 */

        return BattleResult();
    }
    
    // Obtém os atributos de batalha
    BattleAttributes attAttributes = GetBattleAttributes(attacker);
    BattleAttributes defAttributes = GetBattleAttributes(target);
    
    // Obtém dados da skill (se aplicável)
    const STRUCT_SKILL_DATA* skillData = nullptr;
    if (skillID > 0) {
        skillData = GetSkillData(skillID);
    }
    
    // Escolhe a fórmula de dano com base no tipo
    BattleResult result;
    switch (damageType) {
        case DamageType::PHYSICAL:
        case DamageType::RANGED:
            result = CalculatePhysicalDamage(attAttributes, defAttributes, skillData, skillLevel);
            break;
            
        case DamageType::MAGIC:
        case DamageType::FIRE:
        case DamageType::ICE:
        case DamageType::LIGHTNING:
        case DamageType::EARTH:
        case DamageType::WIND:
        case DamageType::HOLY:
        case DamageType::DARK:
        case DamageType::POISON:
            result = CalculateMagicDamage(attAttributes, defAttributes, skillData, skillLevel);
            break;
            
        case DamageType::PERCENTAGE:
            result = CalculatePercentageDamage(attAttributes, defAttributes, skillData, skillLevel);
            break;
            
        case DamageType::TRUE:
            // Dano verdadeiro ignora defesa
            result.Damage = attAttributes.PhysicalDamageMax;
            if (skillData) {
                result.Damage = skillData->wDamageBase + (skillData->wDamagePerLevel * skillLevel);
            }
            result.Type = DamageType::TRUE;
            break;
            
        case DamageType::HEAL:
            result = CalculateHealing(attAttributes, defAttributes, skillData, skillLevel);
            break;
            
        default:
            // Tipo de dano não suportado
            /**
 * BattleResult
 * 
 * Implementa a funcionalidade BattleResult conforme especificação original.
 * @return Retorna return
 */

            return BattleResult();
    }
    
    // Define IDs e timestamp
    result.AttackerID = attacker->GetID();
    result.TargetID = target->GetID();
    result.Timestamp = static_cast<DWORD>(GetTickCount64());
    result.SkillID = skillID;
    result.IsSkill = (skillID > 0);
    
    // Verifica se deve aplicar algum /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna efeito
 */
 efeito
    if(skillData && skillData->byControlType != 0){
        result.SetControlEffect(static_cast<ControlType>(skillData->byControlType), skillData->byControlValue);
    }
    
    return result;
}

bool CBattleManager::ApplyEffect(CEntity* target, AffectType affectType, BYTE value, DWORD duration) {
    if (!initialized_ || !target) {
        return false;
    }
    
    // Aplica o efeito à entidade
    // Esta é uma implementação simplificada, a real teria que lidar com
    // todos os detalhes de aplicação de efeitos no WYD
    
    // No WYD original, isso seria algo como:
    // STRUCT_AFFECT affect;
    // affect.byType = static_cast<BYTE>(affectType);
    // affect.byValue = value;
    // affect.dwTime = duration;
    // 
    // for (int i = 0; i < MAX_AFFECT; i++) {
    //     if (target->Affects[i].byType == 0) {
    //         target->Affects[i] = affect;
    //         return true;
    //     }
    // }
    
    // Aqui, como não temos acesso direto à estrutura do personagem,
    // vamos apenas retornar true para simular que o efeito foi aplicado
    return true;
}

bool CBattleManager::RemoveEffect(CEntity* target, AffectType affectType) {
    if (!initialized_ || !target) {
        return false;
    }
    
    // Remove o efeito da entidade
    // Esta é uma implementação simplificada, a real teria que lidar com
    // todos os detalhes de remoção de efeitos no WYD
    
    // No WYD original, isso seria algo como:
    // for (int i = 0; i < MAX_AFFECT; i++) {
    //     if (target->Affects[i].byType == static_cast<BYTE>(affectType)) {
    //         target->Affects[i].byType = 0;
    //         target->Affects[i].byValue = 0;
    //         target->Affects[i].dwTime = 0;
    //         return true;
    //     }
    // }
    
    // Aqui, como não temos acesso direto à estrutura do personagem,
    // vamos apenas retornar true para simular que o efeito foi removido
    return true;
}

bool CBattleManager::CheckHit(CEntity* attacker, CEntity* target, WORD skillID) {
    if (!initialized_ || !attacker || !target) {
        return false;
    }
    
    // Obtém os atributos de batalha
    BattleAttributes attAttributes = GetBattleAttributes(attacker);
    BattleAttributes defAttributes = GetBattleAttributes(target);
    
    // No WYD original, a fórmula de acerto era algo como:
    // Base de acerto do atacante - Base de evasão do defensor + modificadores
    int hitChance = attAttributes.AttackRating - defAttributes.Evasion;
    
    // Ajusta com base na /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna skill
 */
 skill
    if(skillID > 0){
        const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
        if (skillData) {
            // Skills têm chance de acerto própria
            hitChance = skillData->bySuccessRate;
        }
    }
    
    // Garante que a chance está no intervalo [5, 95] (no WYD sempre tinha uma chance de erro/acerto)
    hitChance = std::max(5, std::min(hitChance, 95));
    
    // Lança um dado de 1 a 100
    std::uniform_int_distribution<> dis(1, 100);
    int roll = dis(rng_);
    
    // Acerta se o número for menor ou igual à chance de acerto
    return roll <= hitChance;
}

bool CBattleManager::CheckCritical(CEntity* attacker, CEntity* target, WORD skillID) {
    if (!initialized_ || !attacker || !target) {
        return false;
    }
    
    // Obtém os atributos de batalha
    BattleAttributes attAttributes = GetBattleAttributes(attacker);
    
    // Chance de crítico base do atacante
    int critChance = attAttributes.CriticalChance;
    
    // Ajusta com base na /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna skill
 */
 skill
    if(skillID > 0){
        // Algumas skills podem aumentar a chance de crítico
        // Isso seria implementado aqui
    }
    
    // Limita a chance de crítico máxima
    critChance = std::min(critChance, MAX_CRITICAL_CHANCE);
    
    // Lança um dado de 1 a 100
    std::uniform_int_distribution<> dis(1, 100);
    int roll = dis(rng_);
    
    // Crítico se o número for menor ou igual à chance de crítico
    return roll <= critChance;
}

BattleAttributes CBattleManager::GetBattleAttributes(CEntity* entity) {
    // Esta é uma implementação simplificada, a real teria que lidar com
    // todos os atributos de um personagem/mob no WYD
    
    BattleAttributes attributes;
    
    // No WYD original, isso extrairia os atributos da entidade e seus itens
    // Por exemplo:
    // attributes.Str = entity->Str + GetEquipmentBonusStr(entity);
    // attributes.PhysicalDamageMin = CalculatePhysicalDamageMin(entity);
    // Etc.
    
    // Aqui, como não temos acesso direto à estrutura do personagem,
    // vamos apenas retornar atributos padrão
    return attributes;
}

bool CBattleManager::CanAttack(CEntity* attacker, CEntity* target) {
    if (!initialized_ || !attacker || !target) {
        return false;
    }
    
    // No WYD original, isso verificaria várias condições:
    // - Se o alvo está vivo
    // - Se o atacante pode atacar (não está em stun, etc.)
    // - Se o alvo não é imune
    // - Se o atacante está em range
    // - Se é PvP, verifica as condições de PvP
    // - Etc.
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos apenas retornar true para simular que pode atacar
    return true;
}

bool CBattleManager::CanUseSkill(CEntity* caster, WORD skillID, BYTE skillLevel) {
    if (!initialized_ || !caster) {
        return false;
    }
    
    // Obtém dados da skill
    const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
    if (!skillData) {
        return false;
    }
    
    // No WYD original, isso verificaria várias condições:
    // - Se o personagem conhece a skill
    // - Se o nível da skill está correto
    // - Se o personagem tem MP suficiente
    // - Se o personagem tem HP suficiente (para skills que consomem HP)
    // - Se a skill não está em cooldown
    // - Se o personagem não está impedido de usar skills (stun, etc.)
    // - Etc.
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos apenas retornar true para simular que pode usar
    return true;
}

bool CBattleManager::CanTargetWithSkill(CEntity* caster, CEntity* target, WORD skillID) {
    if (!initialized_ || !caster || !target) {
        return false;
    }
    
    // Obtém dados da skill
    const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
    if (!skillData) {
        return false;
    }
    
    // Obtém o tipo de alvo
    TargetType targetType = static_cast<TargetType>(skillData->byTargetType);
    
    // No WYD original, isso verificaria várias condições com base no tipo de alvo:
    // - Para alvos aliados, verifica se o alvo é aliado
    // - Para alvos inimigos, verifica se o alvo é inimigo
    // - Para alvos de área, verifica se o alvo está na área
    // - Etc.
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos apenas retornar true para simular que pode usar
    return true;
}

const STRUCT_SKILL_DATA* CBattleManager::GetSkillData(WORD skillID) {
    if (!initialized_) {
        return nullptr;
    }
    
    auto it = skills_.find(skillID);
    if (it == skills_.end()) {
        return nullptr;
    }
    
    return &it->second;
}

int CBattleManager::RegisterBattleEventCallback(const BattleEventCallback& callback) {
    if (!initialized_) {
        LOG_ERROR("BattleManager não inicializado");
        return -1;
    }
    
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    int callbackID = nextCallbackID_++;
    battleEventCallbacks_[callbackID] = callback;
    
    return callbackID;
}

bool CBattleManager::UnregisterBattleEventCallback(int callbackID) {
    if (!initialized_) {
        LOG_ERROR("BattleManager não inicializado");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    auto it = battleEventCallbacks_.find(callbackID);
    if (it == battleEventCallbacks_.end()) {
        return false;
    }
    
    battleEventCallbacks_.erase(it);
    return true;
}

void CBattleManager::AddDamageRecord(DWORD targetID, DWORD attackerID, int damage) {
    if (!initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(damageRecordsMutex_);
    
    // Cria um novo registro de dano
    DamageRecord record(attackerID, damage, static_cast<DWORD>(GetTickCount64()));
    
    // Adiciona à lista de dano do alvo
    damageRecords_[targetID].push_back(record);
    
    // Limita o tamanho da lista para evitar uso excessivo de memória
    if (damageRecords_[targetID].size() > MAX_DAMAGE_COUNT) {
        damageRecords_[targetID].pop_front();
    }
}

std::vector<DamageRecord> CBattleManager::GetDamageRecords(DWORD targetID) {
    if (!initialized_) {
        return std::vector<DamageRecord>();
    }
    
    std::lock_guard<std::mutex> lock(damageRecordsMutex_);
    
    auto it = damageRecords_.find(targetID);
    if (it == damageRecords_.end()) {
        return std::vector<DamageRecord>();
    }
    
    return std::vector<DamageRecord>(it->second.begin(), it->second.end());
}

void CBattleManager::CleanupDamageRecords(DWORD currentTime, DWORD maxAge) {
    if (!initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(damageRecordsMutex_);
    
    // Para cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param damageRecords_ Parâmetro damageRecords_
 * @return Retorna entidade
 */
 entidade
    for(auto& entry : damageRecords_){
        auto& records = entry.second;
        
        // Remove registros antigos
        while (!records.empty() && (currentTime - records.front().Timestamp) > maxAge) {
            records.pop_front();
        }
    }
}

std::unordered_map<DWORD, float> CBattleManager::GetDamageContributions(DWORD targetID) {
    if (!initialized_) {
        return std::unordered_map<DWORD, float>();
    }
    
    std::lock_guard<std::mutex> lock(damageRecordsMutex_);
    
    auto it = damageRecords_.find(targetID);
    if (it == damageRecords_.end()) {
        return std::unordered_map<DWORD, float>();
    }
    
    const auto& records = it->second;
    
    // Calcula o dano total
    int totalDamage = 0;
    for (const auto& record : records) {
        totalDamage += record.Damage;
    }
    
    // Se não houver dano, retorna /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna vazio
 */
 vazio
    if(totalDamage <= 0){
        return std::unordered_map<DWORD, float>();
    }
    
    // Calcula a contribuição de cada entidade
    std::unordered_map<DWORD, int> damageByEntity;
    for (const auto& record : records) {
        damageByEntity[record.EntityID] += record.Damage;
    }
    
    // Converte para porcentagem
    std::unordered_map<DWORD, float> contributions;
    for (const auto& entry : damageByEntity) {
        contributions[entry.first] = static_cast<float>(entry.second) / totalDamage;
    }
    
    return contributions;
}

DWORD CBattleManager::GetExperienceReceiver(DWORD targetID) {
    if (!initialized_) {
        return 0;
    }
    
    // Obtém as contribuições de dano
    auto contributions = GetDamageContributions(targetID);
    
    // Se não houver contribuições, retorna 0
    if (contributions.empty()) {
        return 0;
    }
    
    // Encontra a entidade com maior contribuição
    DWORD topContributorID = 0;
    float topContribution = 0.0f;
    
    for (const auto& entry : contributions) {
        if (entry.second > topContribution) {
            topContribution = entry.second;
            topContributorID = entry.first;
        }
    }
    
    return topContributorID;
}

void CBattleManager::ProcessDeath(CEntity* deadEntity, CEntity* killer) {
    if (!initialized_ || !deadEntity) {
        return;
    }
    
    DWORD deadEntityID = deadEntity->GetID();
    DWORD killerID = killer ? killer->GetID() : 0;
    
    // Se não há um killer explícito, determina pelo /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna dano
 */
 dano
    if(killerID == 0){
        killerID = GetExperienceReceiver(deadEntityID);
    }
    
    // No WYD original, isso iria:
    // 1. Calcular a experiência a ser dada
    // 2. Gerar os drops
    // 3. Distribuir tudo conforme as contribuições
    // 4. Remover o morto do mundo
    // 5. Controlar respawn se necessário
    // Etc.
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos apenas registrar o /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param {}" Parâmetro {}"
 * @param deadEntityID Parâmetro deadEntityID
 * @param killerID Parâmetro killerID
 * @return Retorna evento
 */
 evento
    LOG_INFO("Entidade {} morta por {}", deadEntityID, killerID);
    
    // Limpa os registros de dano do morto
    {
        std::lock_guard<std::mutex> lock(damageRecordsMutex_);
        damageRecords_.erase(deadEntityID);
    }
}

DWORD CBattleManager::CalculateAttackDelay(int attackSpeed) {
    if (!initialized_) {
        return BATTLE_DELAY_NORMAL;
    }
    
    // No WYD original, a fórmula era algo como:
    // delay = BASE_DELAY * (100 / attackSpeed)
    // Com limites mínimo e máximo
    
    // Limita a velocidade de ataque
    attackSpeed = std::max(1, std::min(attackSpeed, MAX_ATTACK_SPEED));
    
    // Calcula o delay
    DWORD delay = static_cast<DWORD>(BATTLE_DELAY_NORMAL * (100.0f / attackSpeed));
    
    // Limita o delay
    delay = std::max(BATTLE_DELAY_FAST, std::min(delay, BATTLE_DELAY_SLOW));
    
    return delay;
}

DWORD CBattleManager::CalculateSkillCooldown(WORD skillID, BYTE skillLevel, CEntity* attacker) {
    if (!initialized_) {
        return 0;
    }
    
    // Obtém dados da skill
    const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
    if (!skillData) {
        return 0;
    }
    
    // No WYD original, o cooldown base era definido pela skill
    // E podia ser modificado por itens, buffs, etc.
    DWORD cooldown = skillData->wCooldown;
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos apenas retornar o cooldown base
    return cooldown;
}

DWORD CBattleManager::CalculateEffectDuration(AffectType affectType, BYTE value, CEntity* caster, CEntity* target) {
    if (!initialized_) {
        return 0;
    }
    
    // No WYD original, a duração de efeitos era determinada por várias coisas:
    // - Tipo do efeito
    // - Valor do efeito
    // - Atributos do conjurador
    // - Resistências do alvo
    // - Etc.
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos retornar durações padrão por /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param affectType Parâmetro affectType
 * @return Retorna tipo
 */
 tipo
    switch(affectType){
        case AffectType::STUN:
            return 2000 + (value * 500); // 2-7 segundos
            
        case AffectType::POISON:
            return 10000 + (value * 1000); // 10-20 segundos
            
        case AffectType::SLOW:
            return 5000 + (value * 1000); // 5-15 segundos
            
        case AffectType::DIVINE_PROTECTION:
            return 10000 + (value * 2000); // 10-30 segundos
            
        default:
            return 10000; // 10 segundos padrão
    }
}

int CBattleManager::CalculateSkillRange(WORD skillID, BYTE skillLevel, CEntity* caster) {
    if (!initialized_) {
        return 0;
    }
    
    // Obtém dados da skill
    const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
    if (!skillData) {
        return 0;
    }
    
    // No WYD original, o alcance base era definido pela skill
    // E podia ser modificado por itens, buffs, etc.
    int range = skillData->wRange;
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos apenas retornar o alcance base
    return range;
}

int CBattleManager::CalculateSkillArea(WORD skillID, BYTE skillLevel, CEntity* caster) {
    if (!initialized_) {
        return 0;
    }
    
    // Obtém dados da skill
    const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
    if (!skillData) {
        return 0;
    }
    
    // No WYD original, a área base era definida pela skill
    // E podia ser modificada por itens, buffs, etc.
    int area = skillData->wArea;
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos apenas retornar a área base
    return area;
}

int CBattleManager::CalculateSkillMPCost(WORD skillID, BYTE skillLevel, CEntity* caster) {
    if (!initialized_) {
        return 0;
    }
    
    // Obtém dados da skill
    const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
    if (!skillData) {
        return 0;
    }
    
    // No WYD original, o custo de MP base era definido pela skill
    // E podia ser modificado por itens, buffs, etc.
    int mpCost = skillData->wMPCost;
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos apenas retornar o custo base
    return mpCost;
}

int CBattleManager::CalculateSkillHPCost(WORD skillID, BYTE skillLevel, CEntity* caster) {
    if (!initialized_) {
        return 0;
    }
    
    // Obtém dados da skill
    const STRUCT_SKILL_DATA* skillData = GetSkillData(skillID);
    if (!skillData) {
        return 0;
    }
    
    // No WYD original, o custo de HP base era definido pela skill
    // E podia ser modificado por itens, buffs, etc.
    int hpCost = skillData->wHPCost;
    
    // Aqui, como não temos acesso direto às estruturas,
    // vamos apenas retornar o custo base
    return hpCost;
}

void CBattleManager::NotifyBattleEvent(const BattleResult& result) {
    if (!initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    
    // Notifica todos os callbacks /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param battleEventCallbacks_ Parâmetro battleEventCallbacks_
 * @return Retorna registrados
 */
 registrados
    for(const auto& entry : battleEventCallbacks_){
        entry.second(result);
    }
}

STRUCT_SKILL_DATA* CBattleManager::FindSkill(WORD skillID) {
    if (!initialized_) {
        return nullptr;
    }
    
    auto it = skills_.find(skillID);
    if (it == skills_.end()) {
        return nullptr;
    }
    
    return &it->second;
}

int CBattleManager::DamageTypeToResistIndex(DamageType damageType) {
    switch (damageType) {
        case DamageType::FIRE:
            return 0; // Índice para FireResist
            
        case DamageType::ICE:
            return 1; // Índice para IceResist
            
        case DamageType::LIGHTNING:
            return 2; // Índice para LightningResist
            
        case DamageType::HOLY:
            return 3; // Índice para HolyResist
            
        case DamageType::DARK:
            return 4; // Índice para DarkResist
            
        default:
            return -1; // Sem resistência específica
    }
}

BattleResult CBattleManager::CalculatePhysicalDamage(const BattleAttributes& attAttributes, 
                                                  const BattleAttributes& defAttributes,
                                                  const STRUCT_SKILL_DATA* skillData,
                                                  BYTE skillLevel) {
    BattleResult result;
    result.Type = DamageType::PHYSICAL;
    
    // Determina dano base - entre mínimo e máximo
    std::uniform_int_distribution<> disDamage(attAttributes.PhysicalDamageMin, attAttributes.PhysicalDamageMax);
    int baseDamage = disDamage(rng_);
    
    // Se for uma skill, usa o dano da /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param skillData Parâmetro skillData
 * @return Retorna skill
 */
 skill
    if(skillData){
        baseDamage = skillData->wDamageBase + (skillData->wDamagePerLevel * skillLevel);
        result.Type = static_cast<DamageType>(skillData->byDamageType);
    }
    
    // No WYD original, a fórmula era algo como:
    // damage = (baseDamage * STR_FACTOR) - (defesa * DEF_FACTOR)
    // Com fatores dependendo do tipo de ataque, classe, etc.
    
    // Simplificando para esta implementação
    float strFactor = 1.0f + (attAttributes.Str * 0.01f);
    int damage = static_cast<int>(baseDamage * strFactor);
    
    // Reduz pela defesa
    float defFactor = 50.0f / (50.0f + defAttributes.PhysicalDefense);
    damage = static_cast<int>(damage * defFactor);
    
    // Verifica se é crítico
    std::uniform_int_distribution<> disCrit(1, 100);
    bool isCritical = disCrit(rng_) <= attAttributes.CriticalChance;
    
    if (isCritical) {
        // Aplica dano crítico (no WYD, críticos geralmente eram 1.5x ou 2x)
        damage = static_cast<int>(damage * (attAttributes.CriticalDamage / 100.0f));
        result.SetFlag(BattleFlag::CRITICAL);
    }
    
    // Aplica redução de dano /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna percentual
 */
 percentual
    if(defAttributes.DamageReduction > 0){
        float reduction = std::min(defAttributes.DamageReduction, 80) / 100.0f; // Máximo de 80%
        damage = static_cast<int>(damage * (1.0f - reduction));
    }
    
    // Aplica absorção se houver
    int absorbAmount = 0;
    if (defAttributes.AbsorbHP > 0) {
        float absorbRate = std::min(defAttributes.AbsorbHP, MAX_ABSORB) / 100.0f;
        absorbAmount = static_cast<int>(damage * absorbRate);
        damage -= absorbAmount;
    }
    
    // Garante que o dano é pelo menos 1 (no WYD, ataques bem-sucedidos sempre causavam pelo menos 1 de dano)
    damage = std::max(1, damage);
    
    // Reflete dano se /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna houver
 */
 houver
    if(defAttributes.Reflect > 0){
        float reflectRate = defAttributes.Reflect / 100.0f;
        int reflectDamage = static_cast<int>(damage * reflectRate);
        
        if (reflectDamage > 0) {
            result.SetFlag(BattleFlag::REFLECT);
            // No WYD real, isso aplicaria o dano refletido ao atacante
        }
    }
    
    // Armazena o resultado
    result.Damage = damage;
    result.AbsorbedHP = absorbAmount;
    
    return result;
}

BattleResult CBattleManager::CalculateMagicDamage(const BattleAttributes& attAttributes, 
                                               const BattleAttributes& defAttributes,
                                               const STRUCT_SKILL_DATA* skillData,
                                               BYTE skillLevel) {
    BattleResult result;
    result.Type = DamageType::MAGIC;
    
    // Skills mágicas sempre vêm de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !skillData Parâmetro !skillData
 * @return Retorna skills
 */
 skills
    if(!skillData){
        return result;
    }
    
    // Define o tipo de dano
    result.Type = static_cast<DamageType>(skillData->byDamageType);
    
    // Dano base da skill
    int baseDamage = skillData->wDamageBase + (skillData->wDamagePerLevel * skillLevel);
    
    // No WYD original, a fórmula era algo como:
    // damage = (baseDamage * INT_FACTOR) - (defesa_magica * MDEF_FACTOR)
    // Com fatores dependendo do tipo de magia, classe, etc.
    
    // Simplificando para esta implementação
    float intFactor = 1.0f + (attAttributes.Int * 0.01f);
    int damage = static_cast<int>(baseDamage * intFactor);
    
    // Verifica resistência ao tipo de dano
    int resistIndex = DamageTypeToResistIndex(result.Type);
    int resistance = 0;
    
    if (resistIndex >= 0) {
        // No WYD original, isso acessaria o array de resistências
        // resistance = defAttributes.Resist[resistIndex]
        
        // Como não temos acesso a isso aqui, vamos usar um valor padrão
        resistance = 0;
    }
    
    // Aplica resistência mágica geral
    float mdefFactor = 50.0f / (50.0f + defAttributes.MagicDefense);
    damage = static_cast<int>(damage * mdefFactor);
    
    // Aplica resistência especí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna fica
 */
fica
    if(resistance > 0){
        float resistFactor = 1.0f - (std::min(resistance, MAX_MAGIC_RESIST) / 100.0f);
        damage = static_cast<int>(damage * resistFactor);
        result.SetFlag(BattleFlag::RESISTED);
    }
    
    // Verifica se é crítico (magias também podiam ser críticas no WYD)
    std::uniform_int_distribution<> disCrit(1, 100);
    bool isCritical = disCrit(rng_) <= attAttributes.CriticalChance;
    
    if (isCritical) {
        // Aplica dano crítico
        damage = static_cast<int>(damage * (attAttributes.CriticalDamage / 100.0f));
        result.SetFlag(BattleFlag::CRITICAL);
    }
    
    // Aplica redução de dano /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna percentual
 */
 percentual
    if(defAttributes.DamageReduction > 0){
        float reduction = std::min(defAttributes.DamageReduction, 80) / 100.0f; // Máximo de 80%
        damage = static_cast<int>(damage * (1.0f - reduction));
    }
    
    // Garante que o dano é pelo menos 1
    damage = std::max(1, damage);
    
    // Armazena o resultado
    result.Damage = damage;
    
    return result;
}

BattleResult CBattleManager::CalculateHealing(const BattleAttributes& attAttributes, 
                                           const BattleAttributes& defAttributes,
                                           const STRUCT_SKILL_DATA* skillData,
                                           BYTE skillLevel) {
    BattleResult result;
    result.Type = DamageType::HEAL;
    
    // Curas sempre vêm de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !skillData Parâmetro !skillData
 * @return Retorna skills
 */
 skills
    if(!skillData){
        return result;
    }
    
    // Valor base da cura
    int baseHeal = skillData->wDamageBase + (skillData->wDamagePerLevel * skillLevel);
    
    // No WYD original, a fórmula para cura tinha fatores baseados em INT, sabedoria, etc.
    // Simplificando para esta implementação
    float intFactor = 1.0f + (attAttributes.Int * 0.01f);
    int healAmount = static_cast<int>(baseHeal * intFactor);
    
    // Curas não podem ter críticos no WYD original
    // Curas não são reduzidas por defesa
    // Curas não são absorvidas ou refletidas
    
    // No WYD, curas negativas não existiam (não causavam dano)
    healAmount = std::max(0, healAmount);
    
    // Armazena o resultado (como cura, o "Damage" é negativo)
    result.Damage = -healAmount;
    
    return result;
}

BattleResult CBattleManager::CalculatePercentageDamage(const BattleAttributes& attAttributes, 
                                                    const BattleAttributes& defAttributes,
                                                    const STRUCT_SKILL_DATA* skillData,
                                                    BYTE skillLevel) {
    BattleResult result;
    result.Type = DamageType::PERCENTAGE;
    
    // Dano percentual sempre vem de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !skillData Parâmetro !skillData
 * @return Retorna skills
 */
 skills
    if(!skillData){
        return result;
    }
    
    // Porcentagem base da skill
    int basePercent = skillData->wValueBase + (skillData->wValuePerLevel * skillLevel);
    
    // No WYD original, danos percentuais geralmente eram baseados no HP máximo do alvo
    // Como não temos acesso direto a isso, vamos assumir um valor
    int targetMaxHP = 1000; // Valor fictício
    
    // Calcula o dano
    int damage = static_cast<int>(targetMaxHP * (basePercent / 100.0f));
    
    // Garante que o dano é pelo menos 1
    damage = std::max(1, damage);
    
    // Armazena o resultado
    result.Damage = damage;
    
    return result;
}

} // namespace server
} // namespace wyd