/**
 * WYDMob.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/WYDMob.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_MOB_H
#define WYD_MOB_H

#include "src/server/tmsrv/npc/WYDEntity.h"
#include <vector>
#include <deque>
#include <memory>
#include <mutex>

namespace wydbr {


// Forward declaration
class NPCManager;
class CombatSystem;

/**
 * Comportamentos de Mob
 */
enum class MobBehavior : uint8_t {
    PASSIVE,        // Só ataca se for atacado
    AGGRESSIVE,     // Ataca qualquer jogador em alcance
    DEFENSIVE,      // Permanece próximo ao spawn point
    COWARD,         // Foge quando está com pouca vida
    ASSIST,         // Ajuda outros mobs do mesmo tipo
    GUARD,          // Protege uma área específica
    PATROL,         // Patrulha entre pontos específicos
    BOSS            // Comportamento especial de chefe
};

/**
 * Tipos de Ataques de Mob
 */
enum class MobAttackType : uint8_t {
    MELEE,          // Ataque corpo a corpo
    RANGED,         // Ataque à distância
    MAGIC,          // Ataque mágico
    SPECIAL         // Ataque especial
};

/**
 * Classe de monstros (mobs)
 * 
 * Representa qualquer entidade hostil no mundo do jogo.
 * Possui comportamento controlado por IA, atributos de combate,
 * e capacidade de interagir com jogadores principalmente através de combate.
 */
/**
 * Classe WYDMob
 * 
 * Esta classe implementa a funcionalidade WYDMob conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDMob
 * 
 * Esta classe implementa a funcionalidade WYDMob conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDMob : public WYDEntity {
public:
    // /**
 * WYDMob
 * 
 * Implementa a funcionalidade WYDMob conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @param mobId Parâmetro mobId
 * @param name Parâmetro name
 * @return Retorna Construtor
 */
 Construtor
    WYDMob(uint32_t uniqueId, uint16_t mobId, const std::string& name);
    
    // Destrutor
    virtual ~WYDMob();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @param manager Parâmetro manager
 */

    void initialize(NPCManager* manager);
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp) override;
    
    // Serialização
    std::string serialize() const override;
    
    // Identificação específica
    /**
 * getMobId
 * 
 * Implementa a funcionalidade getMobId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getMobId() const{ return mobId_; }
    /**
 * getLevel
 * 
 * Implementa a funcionalidade getLevel conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getLevel() const{ return level_; }
    /**
 * isBoss
 * 
 * Implementa a funcionalidade isBoss conforme especificação original.
 * @return Retorna bool
 */

    bool isBoss() const;
    
    // Atributos
    /**
 * getMaxHP
 * 
 * Implementa a funcionalidade getMaxHP conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getMaxHP() const{ return maxHP_; }
    /**
 * getHP
 * 
 * Implementa a funcionalidade getHP conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getHP() const{ return hp_; }
    /**
 * getMaxMP
 * 
 * Implementa a funcionalidade getMaxMP conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getMaxMP() const{ return maxMP_; }
    /**
 * getMP
 * 
 * Implementa a funcionalidade getMP conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getMP() const{ return mp_; }
    
    /**
 * getStrength
 * 
 * Implementa a funcionalidade getStrength conforme especificação original.
 * @return Retorna uint16_t
 */

    
    uint16_t getStrength() const{ return strength_; }
    /**
 * getDexterity
 * 
 * Implementa a funcionalidade getDexterity conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getDexterity() const{ return dexterity_; }
    /**
 * getIntelligence
 * 
 * Implementa a funcionalidade getIntelligence conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getIntelligence() const{ return intelligence_; }
    /**
 * getConstitution
 * 
 * Implementa a funcionalidade getConstitution conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getConstitution() const{ return constitution_; }
    
    /**
 * getAttack
 * 
 * Implementa a funcionalidade getAttack conforme especificação original.
 * @return Retorna uint16_t
 */

    
    uint16_t getAttack() const{ return attack_; }
    /**
 * getDefense
 * 
 * Implementa a funcionalidade getDefense conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getDefense() const{ return defense_; }
    
    // Configurações
    /**
 * setAttributes
 * 
 * Implementa a funcionalidade setAttributes conforme especificação original.
 * @param level Parâmetro level
 * @param hp Parâmetro hp
 * @param mp Parâmetro mp
 * @param str Parâmetro str
 * @param dex Parâmetro dex
 * @param intel Parâmetro intel
 * @param con Parâmetro con
 * @param attack Parâmetro attack
 * @param defense Parâmetro defense
 */

    void setAttributes(uint16_t level, uint32_t hp, uint32_t mp, 
                     uint16_t str, uint16_t dex, uint16_t intel, uint16_t con,
                     uint16_t attack, uint16_t defense);
    
    /**
 * setBehavior
 * 
 * Implementa a funcionalidade setBehavior conforme especificação original.
 * @param behavior Parâmetro behavior
 */

    
    void setBehavior(MobBehavior behavior){ behavior_ = behavior; }
    /**
 * setAggroRange
 * 
 * Implementa a funcionalidade setAggroRange conforme especificação original.
 * @param range Parâmetro range
 */

    void setAggroRange(uint16_t range){ aggroRange_ = range; }
    /**
 * setSpawnPoint
 * 
 * Implementa a funcionalidade setSpawnPoint conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 */

    void setSpawnPoint(uint16_t x, uint16_t y){ spawnX_ = x; spawnY_ = y; }
    /**
 * setPatrolPoints
 * 
 * Implementa a funcionalidade setPatrolPoints conforme especificação original.
 * @param std::vector<std::pair<uint16_t Parâmetro std::vector<std::pair<uint16_t
 * @param points Parâmetro points
 */

    void setPatrolPoints(const std::vector<std::pair<uint16_t, uint16_t>>& points);
    /**
 * setMoveSpeed
 * 
 * Implementa a funcionalidade setMoveSpeed conforme especificação original.
 * @param speed Parâmetro speed
 */

    void setMoveSpeed(uint16_t speed){ moveSpeed_ = speed; }
    /**
 * setAttackSpeed
 * 
 * Implementa a funcionalidade setAttackSpeed conforme especificação original.
 * @param speed Parâmetro speed
 */

    void setAttackSpeed(uint16_t speed){ attackSpeed_ = speed; }
    /**
 * setAttackType
 * 
 * Implementa a funcionalidade setAttackType conforme especificação original.
 * @param type Parâmetro type
 */

    void setAttackType(MobAttackType type){ attackType_ = type; }
    
    // Configuração de habilidades e drops
    /**
 * setSkills
 * 
 * Implementa a funcionalidade setSkills conforme especificação original.
 * @param skills Parâmetro skills
 */

    void setSkills(const std::vector<uint16_t>& skills);
    /**
 * setDropItems
 * 
 * Implementa a funcionalidade setDropItems conforme especificação original.
 * @param items Parâmetro items
 * @param rates Parâmetro rates
 */

    void setDropItems(const std::vector<uint16_t>& items, const std::vector<uint8_t>& rates);
    /**
 * setExperienceAndGold
 * 
 * Implementa a funcionalidade setExperienceAndGold conforme especificação original.
 * @param exp Parâmetro exp
 * @param gold Parâmetro gold
 */

    void setExperienceAndGold(uint32_t exp, uint32_t gold);
    
    // Ações
    /**
 * takeDamage
 * 
 * Implementa a funcionalidade takeDamage conforme especificação original.
 * @param damage Parâmetro damage
 * @param attackerId Parâmetro attackerId
 * @return Retorna bool
 */

    bool takeDamage(uint32_t damage, uint32_t attackerId);
    /**
 * useSkill
 * 
 * Implementa a funcionalidade useSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @param 0 Parâmetro 0
 * @return Retorna bool
 */

    bool useSkill(uint16_t skillId, uint32_t targetId = 0);
    /**
 * moveToPosition
 * 
 * Implementa a funcionalidade moveToPosition conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool moveToPosition(uint16_t x, uint16_t y);
    /**
 * attackTarget
 * 
 * Implementa a funcionalidade attackTarget conforme especificação original.
 * @param targetId Parâmetro targetId
 * @return Retorna bool
 */

    bool attackTarget(uint32_t targetId);
    
    // Estado de combate
    /**
 * isInCombat
 * 
 * Implementa a funcionalidade isInCombat conforme especificação original.
 * @return Retorna bool
 */

    bool isInCombat() const{ return inCombat_; }
    /**
 * getCurrentTargetId
 * 
 * Implementa a funcionalidade getCurrentTargetId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getCurrentTargetId() const{ return currentTargetId_; }
    /**
 * addToAggroList
 * 
 * Implementa a funcionalidade addToAggroList conforme especificação original.
 * @param entityId Parâmetro entityId
 * @param aggroAmount Parâmetro aggroAmount
 */

    void addToAggroList(uint32_t entityId, uint32_t aggroAmount);
    /**
 * clearAggroList
 * 
 * Implementa a funcionalidade clearAggroList conforme especificação original.
 */

    void clearAggroList();
    
    // Manipulação direta de HP/MP (para efeitos/skills)
    /**
 * setHP
 * 
 * Implementa a funcionalidade setHP conforme especificação original.
 * @param hp Parâmetro hp
 */

    void setHP(uint32_t hp);
    /**
 * setMP
 * 
 * Implementa a funcionalidade setMP conforme especificação original.
 * @param mp Parâmetro mp
 */

    void setMP(uint32_t mp);
    /**
 * modifyHP
 * 
 * Implementa a funcionalidade modifyHP conforme especificação original.
 * @param amount Parâmetro amount
 */

    void modifyHP(int32_t amount);
    /**
 * modifyMP
 * 
 * Implementa a funcionalidade modifyMP conforme especificação original.
 * @param amount Parâmetro amount
 */

    void modifyMP(int32_t amount);
    
    // Geração de loot e recompensas
    std::vector<uint32_t> generateDrops();
    /**
 * getExperienceReward
 * 
 * Implementa a funcionalidade getExperienceReward conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getExperienceReward() const{ return experience_; }
    /**
 * getGoldReward
 * 
 * Implementa a funcionalidade getGoldReward conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getGoldReward() const{ return gold_; }
    
private:
    // Referência ao gerenciador
    NPCManager* manager_;
    
    // Características específicas
    uint16_t mobId_;           // ID do template do mob
    uint16_t level_;           // Nível do mob
    
    // Atributos
    uint32_t maxHP_;           // HP máximo
    uint32_t hp_;              // HP atual
    uint32_t maxMP_;           // MP máximo
    uint32_t mp_;              // MP atual
    
    uint16_t strength_;        // Força
    uint16_t dexterity_;       // Destreza
    uint16_t intelligence_;    // Inteligência
    uint16_t constitution_;    // Constituição
    
    uint16_t attack_;          // Ataque
    uint16_t defense_;         // Defesa
    
    // Comportamento
    MobBehavior behavior_;     // Comportamento da IA
    uint16_t aggroRange_;      // Raio de detecção
    uint16_t spawnX_;          // Posição X do spawn
    uint16_t spawnY_;          // Posição Y do spawn
    uint16_t moveSpeed_;       // Velocidade de movimento
    uint16_t attackSpeed_;     // Velocidade de ataque
    MobAttackType attackType_; // Tipo de ataque
    
    // Patrulha
    std::vector<std::pair<uint16_t, uint16_t>> patrolPoints_;
    size_t currentPatrolPoint_;
    
    // Habilidades
    std::vector<uint16_t> skills_;
    uint64_t lastSkillUse_;
    
    // Drops
    std::vector<uint16_t> dropItems_;
    std::vector<uint8_t> dropRates_;
    uint32_t experience_;
    uint32_t gold_;
    
    // Estado de combate
    bool inCombat_;
    uint64_t lastCombatAction_;
    uint32_t currentTargetId_;
    std::map<uint32_t, uint32_t> aggroList_;
    
    // Rota de movimento
    std::deque<std::pair<uint16_t, uint16_t>> movementPath_;
    uint64_t lastMoveTime_;
    
    // Mutex para proteção
    mutable std::mutex mobMutex_;
    
    // Timers e cooldowns
    uint64_t lastAttackTime_;
    uint64_t lastAggroCheck_;
    uint64_t lastRoamingCheck_;
    
    // Métodos auxiliares
    /**
 * updateCombat
 * 
 * Implementa a funcionalidade updateCombat conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateCombat(uint64_t timestamp);
    /**
 * updateMovement
 * 
 * Implementa a funcionalidade updateMovement conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateMovement(uint64_t timestamp);
    /**
 * updateAggro
 * 
 * Implementa a funcionalidade updateAggro conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateAggro(uint64_t timestamp);
    /**
 * updateRoaming
 * 
 * Implementa a funcionalidade updateRoaming conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateRoaming(uint64_t timestamp);
    /**
 * checkReturnToSpawn
 * 
 * Implementa a funcionalidade checkReturnToSpawn conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void checkReturnToSpawn(uint64_t timestamp);
    
    /**
 * getTopAggroTarget
 * 
 * Implementa a funcionalidade getTopAggroTarget conforme especificação original.
 * @return Retorna uint32_t
 */

    
    uint32_t getTopAggroTarget();
    /**
 * shouldUseSkill
 * 
 * Implementa a funcionalidade shouldUseSkill conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @param outSkillId Parâmetro outSkillId
 * @return Retorna bool
 */

    bool shouldUseSkill(uint64_t timestamp, uint16_t& outSkillId);
    /**
 * getDistanceToSpawn
 * 
 * Implementa a funcionalidade getDistanceToSpawn conforme especificação original.
 * @return Retorna float
 */

    float getDistanceToSpawn() const;
    
    // Comportamentos específicos
    /**
 * behavePassive
 * 
 * Implementa a funcionalidade behavePassive conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void behavePassive(uint64_t timestamp);
    /**
 * behaveAggressive
 * 
 * Implementa a funcionalidade behaveAggressive conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void behaveAggressive(uint64_t timestamp);
    /**
 * behaveDefensive
 * 
 * Implementa a funcionalidade behaveDefensive conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void behaveDefensive(uint64_t timestamp);
    /**
 * behaveCoward
 * 
 * Implementa a funcionalidade behaveCoward conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void behaveCoward(uint64_t timestamp);
    /**
 * behaveAssist
 * 
 * Implementa a funcionalidade behaveAssist conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void behaveAssist(uint64_t timestamp);
    /**
 * behaveGuard
 * 
 * Implementa a funcionalidade behaveGuard conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void behaveGuard(uint64_t timestamp);
    /**
 * behavePatrol
 * 
 * Implementa a funcionalidade behavePatrol conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void behavePatrol(uint64_t timestamp);
    /**
 * behaveBoss
 * 
 * Implementa a funcionalidade behaveBoss conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void behaveBoss(uint64_t timestamp);
};

#endif // WYD_MOB_H

} // namespace wydbr
