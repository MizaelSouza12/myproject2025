/**
 * WYDEntity.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/WYDEntity.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_ENTITY_H
#define WYD_ENTITY_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace wydbr {


/**
 * Tipos de Entidade
 */
enum class EntityType : uint8_t {
    PLAYER,     // Jogador
    MOB,        // Monstro
    NPC,        // NPC amigável
    ITEM,       // Item no chão
    PROJECTILE, // Projétil
    EFFECT      // Efeito visual
};

/**
 * Estados de Entidade
 */
enum class EntityState : uint8_t {
    IDLE,       // Parado
    MOVING,     // Movendo
    ATTACKING,  // Atacando
    DEFENDING,  // Defendendo
    CASTING,    // Conjurando
    STUNNED,    // Atordoado
    DEAD        // Morto
};

/**
 * Flags de Entidade
 */
enum class EntityFlag : uint32_t {
    ATTACKABLE   = 0x00000001, // Pode ser atacado
    INVISIBLE    = 0x00000002, // Invisível para jogadores
    INVULNERABLE = 0x00000004, // Não pode ser danificado
    NO_COLLISION = 0x00000008, // Não tem colisão
    FLYING       = 0x00000010, // Voando (ignora altura do terreno)
    GM_ENTITY    = 0x00000020, // Entidade de GM
    QUEST_ENTITY = 0x00000040, // Entidade de quest
    EVENT_ENTITY = 0x00000080, // Entidade de evento
    BOSS         = 0x00000100, // Chefe (boss)
    MOUNT        = 0x00000200, // Montaria
    PET          = 0x00000400, // Pet
    ELEMENTAL    = 0x00000800  // Elemental
};

/**
 * Classe base para todas as entidades do mundo
 * 
 * Representa qualquer objeto que pode existir no mundo do jogo:
 * jogadores, monstros, NPCs, itens, projéteis, etc.
 */
/**
 * Classe WYDEntity
 * 
 * Esta classe implementa a funcionalidade WYDEntity conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDEntity {
public:
    // /**
 * WYDEntity
 * 
 * Implementa a funcionalidade WYDEntity conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @param name Parâmetro name
 * @param type Parâmetro type
 * @return Retorna Construtor
 */
 Construtor
    WYDEntity(uint32_t uniqueId, std::string name, EntityType type);
    
    // Destrutor virtual
    virtual ~WYDEntity();
    
    // Identificação
    /**
 * getUniqueId
 * 
 * Implementa a funcionalidade getUniqueId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getUniqueId() const{ return uniqueId_; }
    const std::string& getName() const { return name_; }
    /**
 * getType
 * 
 * Implementa a funcionalidade getType conforme especificação original.
 * @return Retorna EntityType
 */

    EntityType getType() const{ return type_; }
    
    // Estado
    /**
 * getState
 * 
 * Implementa a funcionalidade getState conforme especificação original.
 * @return Retorna EntityState
 */

    EntityState getState() const{ return state_; }
    /**
 * setState
 * 
 * Implementa a funcionalidade setState conforme especificação original.
 * @param state Parâmetro state
 */

    void setState(EntityState state){ state_ = state; }
    /**
 * getLastStateChange
 * 
 * Implementa a funcionalidade getLastStateChange conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getLastStateChange() const{ return lastStateChange_; }
    
    // Posição e movimento
    /**
 * getMapId
 * 
 * Implementa a funcionalidade getMapId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getMapId() const{ return mapId_; }
    /**
 * getPosX
 * 
 * Implementa a funcionalidade getPosX conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getPosX() const{ return posX_; }
    /**
 * getPosY
 * 
 * Implementa a funcionalidade getPosY conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getPosY() const{ return posY_; }
    /**
 * getDirection
 * 
 * Implementa a funcionalidade getDirection conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getDirection() const{ return direction_; }
    
    /**
 * setMap
 * 
 * Implementa a funcionalidade setMap conforme especificação original.
 * @param mapId Parâmetro mapId
 */

    
    void setMap(uint16_t mapId){ mapId_ = mapId; }
    /**
 * setPosition
 * 
 * Implementa a funcionalidade setPosition conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool setPosition(uint16_t x, uint16_t y);
    /**
 * setDirection
 * 
 * Implementa a funcionalidade setDirection conforme especificação original.
 * @param direction Parâmetro direction
 */

    void setDirection(uint8_t direction){ direction_ = direction; }
    
    // Flags
    /**
 * hasFlag
 * 
 * Implementa a funcionalidade hasFlag conforme especificação original.
 * @param flag Parâmetro flag
 * @return Retorna bool
 */

    bool hasFlag(EntityFlag flag) const;
    /**
 * setFlag
 * 
 * Implementa a funcionalidade setFlag conforme especificação original.
 * @param flag Parâmetro flag
 * @param value Parâmetro value
 */

    void setFlag(EntityFlag flag, bool value);
    
    // Atualização
    virtual void update(uint64_t timestamp) = 0;
    
    // Serialização (para envio ao cliente)
    virtual std::string serialize() const = 0;
    
protected:
    // Identificação
    uint32_t uniqueId_;
    std::string name_;
    EntityType type_;
    
    // Estado
    EntityState state_;
    uint64_t lastStateChange_;
    
    // Posição
    uint16_t mapId_;
    uint16_t posX_;
    uint16_t posY_;
    uint8_t direction_;
    
    // Flags
    uint32_t flags_;
    
    // Método auxiliar para atualizações de estado
    /**
 * changeState
 * 
 * Implementa a funcionalidade changeState conforme especificação original.
 * @param newState Parâmetro newState
 * @param timestamp Parâmetro timestamp
 */

    void changeState(EntityState newState, uint64_t timestamp);
};

#endif // WYD_ENTITY_H

} // namespace wydbr
