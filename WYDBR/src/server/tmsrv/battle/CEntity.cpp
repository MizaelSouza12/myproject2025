/**
 * CEntity.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/battle/CEntity.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "CEntity.h"
#include "BattleManager.h"

namespace wydbr {


/**
 * @file CEntity.cpp
 * @brief Implementação da classe base para entidades de combate
 * 
 * Este arquivo contém a implementação da classe base para entidades de combate,
 * que é usada como interface para jogadores, mobs, NPCs, etc.
 */

namespace wyd {

} // namespace wydbr

namespace server {

CEntity::CEntity(const std::string& name, DWORD entityID)
    : name_(name)
    , entityID_(entityID)
    , battleManager_(CBattleManager::GetInstance())
{
}

CEntity::~CEntity() {
}

DWORD CEntity::GetID() const {
    return entityID_;
}

std::string CEntity::GetName() const {
    return name_;
}

void CEntity::SetName(const std::string& name) {
    name_ = name;
}

std::vector<BattleResult> CEntity::UseSkill(WORD skillID, BYTE skillLevel, const std::vector<CEntity*>& targets) {
    // Delega para o BattleManager
    return battleManager_.ProcessSkill(this, targets, skillID, skillLevel);
}

BattleResult CEntity::Attack(CEntity* target) {
    // Delega para o BattleManager
    return battleManager_.ProcessAttack(this, target);
}

bool CEntity::CanAttack(CEntity* target) const {
    // Delega para o BattleManager
    return battleManager_.CanAttack(const_cast<CEntity*>(this), target);
}

bool CEntity::CanUseSkill(WORD skillID, BYTE skillLevel) const {
    // Delega para o BattleManager
    return battleManager_.CanUseSkill(const_cast<CEntity*>(this), skillID, skillLevel);
}

bool CEntity::CanTargetWithSkill(WORD skillID, CEntity* target) const {
    // Delega para o BattleManager
    return battleManager_.CanTargetWithSkill(const_cast<CEntity*>(this), target, skillID);
}

} // namespace server
} // namespace wyd