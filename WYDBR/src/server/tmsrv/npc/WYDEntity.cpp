/**
 * WYDEntity.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/WYDEntity.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/npc/WYDEntity.h"
#include <ctime>

namespace wydbr {


WYDEntity::WYDEntity(uint32_t uniqueId, std::string name, EntityType type)
    : uniqueId_(uniqueId)
    , name_(name)
    , type_(type)
    , state_(EntityState::IDLE)
    , lastStateChange_(static_cast<uint64_t>(std::time(nullptr)) * 1000)
    , mapId_(0)
    , posX_(0)
    , posY_(0)
    , direction_(0)
    , flags_(0)
{
}

WYDEntity::~WYDEntity() 
{
    // Cleanup if needed
}

bool WYDEntity::setPosition(uint16_t x, uint16_t y) 
{
    // A implementação real verificaria colisões e outras restrições
    // Aqui estamos simplificando para focar na estrutura
    posX_ = x;
    posY_ = y;
    return true;
}

bool WYDEntity::hasFlag(EntityFlag flag) const 
{
    return (flags_ & static_cast<uint32_t>(flag)) != 0;
}

void WYDEntity::setFlag(EntityFlag flag, bool value) 
{
    if (value) {
        flags_ |= static_cast<uint32_t>(flag);
    } else {
        flags_ &= ~static_cast<uint32_t>(flag);
    }
}

void WYDEntity::changeState(EntityState newState, uint64_t timestamp) 
{

} // namespace wydbr

    if (state_ != newState) {
        state_ = newState;
        lastStateChange_ = timestamp;
    }
}