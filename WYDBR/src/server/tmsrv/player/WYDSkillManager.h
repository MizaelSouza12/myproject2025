/**
 * WYDSkillManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/player/WYDSkillManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_SKILL_MANAGER_H
#define WYD_SKILL_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <memory>

#include "headers/wyd_core.h"

namespace wydbr {


class WYDPlayer;
class WYDSkill;

/**
 * Estados possíveis de uma habilidade
 */
enum class SkillState : uint8_t {
    READY = 0,        // Pronta para uso
    CASTING = 1,      // Em processo de cast
    ACTIVE = 2,       // Efeito ativo (para skills persistentes)
    COOLDOWN = 3,     // Em cooldown após uso
    DISABLED = 4      // Desabilitada (por algum efeito)
};

/**
 * Tipos de habilidades
 */
enum class SkillType : uint8_t {
    PASSIVE = 0,      // Habilidade passiva (sempre ativa)
    ACTIVE = 1,       // Habilidade ativada pelo jogador
    TOGGLE = 2,       // Habilidade que pode ser ativada/desativada
    CHAIN = 3,        // Parte de uma sequência de habilidades em cadeia
    BUFF = 4,         // Benefício temporário
    DEBUFF = 5,       // Penalidade temporária
    SUMMON = 6,       // Invoca uma entidade
    TRANSFORM = 7     // Transforma o jogador
};

/**
 * Tipos de alvos para habilidades
 */
enum class SkillTargetType : uint8_t {
    SELF = 0,         // Apenas o próprio jogador
    FRIENDLY = 1,     // Jogador aliado (sem PK)
    ENEMY = 2,        // Jogador inimigo (com PK) ou monstro
    ALL = 3,          // Qualquer entidade
    AREA = 4,         // Área ao redor do jogador
    GROUND = 5,       // Posição no chão (alvo não-entidade)
    PARTY = 6,        // Todos os membros do grupo
    GUILD = 7         // Todos os membros da guilda
};

/**
 * Gerenciador de habilidades para um jogador
 */
/**
 * Classe WYDSkillManager
 * 
 * Esta classe implementa a funcionalidade WYDSkillManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDSkillManager
 * 
 * Esta classe implementa a funcionalidade WYDSkillManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDSkillManager {
public:
    // Construtor
    /**
 * WYDSkillManager
 * 
 * Implementa a funcionalidade WYDSkillManager conforme especificação original.
 * @param player Parâmetro player
 * @return Retorna explicit
 */

    explicit WYDSkillManager(WYDPlayer* player);
    
    // Destrutor
    ~WYDSkillManager();
    
    // Inicialização baseada na classe do jogador
    /**
 * initializeSkills
 * 
 * Implementa a funcionalidade initializeSkills conforme especificação original.
 */

    void initializeSkills();
    
    // Carrega habilidades do banco de dados
    /**
 * loadFromDatabase
 * 
 * Implementa a funcionalidade loadFromDatabase conforme especificação original.
 * @return Retorna bool
 */

    bool loadFromDatabase();
    
    // Salva habilidades no banco de dados
    /**
 * saveToDatabase
 * 
 * Implementa a funcionalidade saveToDatabase conforme especificação original.
 * @return Retorna bool
 */

    bool saveToDatabase();
    
    // Acesso a habilidades
    /**
 * getSkill
 * 
 * Implementa a funcionalidade getSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna WYDSkill
 */

    WYDSkill* getSkill(uint16_t skillId);
    /**
 * getSkill
 * 
 * Implementa a funcionalidade getSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna WYDSkill
 */

    const WYDSkill* getSkill(uint16_t skillId) const;
    std::vector<WYDSkill*> getAllSkills();
    std::vector<WYDSkill*> getSkillsByType(SkillType type);
    
    // Manipulação de habilidades
    /**
 * learnSkill
 * 
 * Implementa a funcionalidade learnSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @param 1 Parâmetro 1
 * @return Retorna bool
 */

    bool learnSkill(uint16_t skillId, uint8_t level = 1);
    /**
 * forgetSkill
 * 
 * Implementa a funcionalidade forgetSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool forgetSkill(uint16_t skillId);
    /**
 * upgradeSkill
 * 
 * Implementa a funcionalidade upgradeSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool upgradeSkill(uint16_t skillId);
    /**
 * hasSkill
 * 
 * Implementa a funcionalidade hasSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool hasSkill(uint16_t skillId) const;
    /**
 * getSkillLevel
 * 
 * Implementa a funcionalidade getSkillLevel conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna uint8_t
 */

    uint8_t getSkillLevel(uint16_t skillId) const;
    
    // Uso de habilidades
    /**
 * beginCastSkill
 * 
 * Implementa a funcionalidade beginCastSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @param 0 Parâmetro 0
 * @param 0 Parâmetro 0
 * @param 0 Parâmetro 0
 * @return Retorna bool
 */

    bool beginCastSkill(uint16_t skillId, uint32_t targetId = 0, uint16_t targetX = 0, uint16_t targetY = 0);
    /**
 * cancelCastSkill
 * 
 * Implementa a funcionalidade cancelCastSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool cancelCastSkill(uint16_t skillId);
    /**
 * finishCastSkill
 * 
 * Implementa a funcionalidade finishCastSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool finishCastSkill(uint16_t skillId);
    /**
 * activateSkill
 * 
 * Implementa a funcionalidade activateSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool activateSkill(uint16_t skillId);
    /**
 * deactivateSkill
 * 
 * Implementa a funcionalidade deactivateSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool deactivateSkill(uint16_t skillId);
    
    // Estado de habilidades
    /**
 * getSkillState
 * 
 * Implementa a funcionalidade getSkillState conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna SkillState
 */

    SkillState getSkillState(uint16_t skillId) const;
    /**
 * getSkillCooldownRemaining
 * 
 * Implementa a funcionalidade getSkillCooldownRemaining conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna float
 */

    float getSkillCooldownRemaining(uint16_t skillId) const;
    /**
 * getSkillCastTimeRemaining
 * 
 * Implementa a funcionalidade getSkillCastTimeRemaining conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna float
 */

    float getSkillCastTimeRemaining(uint16_t skillId) const;
    
    // Efeitos e buffs
    /**
 * hasEffect
 * 
 * Implementa a funcionalidade hasEffect conforme especificação original.
 * @param effectId Parâmetro effectId
 * @return Retorna bool
 */

    bool hasEffect(const std::string& effectId) const;
    /**
 * addEffect
 * 
 * Implementa a funcionalidade addEffect conforme especificação original.
 * @param effectId Parâmetro effectId
 * @param duration Parâmetro duration
 * @param strength Parâmetro strength
 */

    void addEffect(const std::string& effectId, float duration, int32_t strength);
    /**
 * removeEffect
 * 
 * Implementa a funcionalidade removeEffect conforme especificação original.
 * @param effectId Parâmetro effectId
 */

    void removeEffect(const std::string& effectId);
    
    // Serialização/desserialização
    std::vector<uint8_t> serialize() const;
    /**
 * deserialize
 * 
 * Implementa a funcionalidade deserialize conforme especificação original.
 * @param data Parâmetro data
 */

    void deserialize(const std::vector<uint8_t>& data);
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp);
    
private:
    // Referência ao jogador
    WYDPlayer* player_;
    
    // Habilidades disponíveis
    std::map<uint16_t, std::unique_ptr<WYDSkill>> skills_;
    
    // Efeitos ativos
    struct SkillEffect {
        std::string effectId;
        float remainingTime;
        int32_t strength;
        uint16_t sourceSkillId;
        uint32_t sourceEntityId;
    };
    std::vector<SkillEffect> activeEffects_;
    
    // Último timestamp de atualização
    uint64_t lastUpdateTime_;
    
    // Mutex para acesso seguro
    mutable std::mutex mutex_;
    
    // Métodos auxiliares
    /**
 * isSkillReady
 * 
 * Implementa a funcionalidade isSkillReady conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool isSkillReady(uint16_t skillId) const;
    /**
 * hasRequiredResources
 * 
 * Implementa a funcionalidade hasRequiredResources conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool hasRequiredResources(uint16_t skillId) const;
    /**
 * applySkillCost
 * 
 * Implementa a funcionalidade applySkillCost conforme especificação original.
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool applySkillCost(uint16_t skillId);
    /**
 * updateCooldowns
 * 
 * Implementa a funcionalidade updateCooldowns conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void updateCooldowns(float deltaTime);
    /**
 * updateEffects
 * 
 * Implementa a funcionalidade updateEffects conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void updateEffects(float deltaTime);
};

/**
 * Classe que representa uma habilidade
 */
/**
 * Classe WYDSkill
 * 
 * Esta classe implementa a funcionalidade WYDSkill conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDSkill
 * 
 * Esta classe implementa a funcionalidade WYDSkill conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDSkill {
public:
    // /**
 * WYDSkill
 * 
 * Implementa a funcionalidade WYDSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @param name Parâmetro name
 * @return Retorna Construtor
 */
 Construtor
    WYDSkill(uint16_t skillId, const std::string& name);
    
    // Getters
    /**
 * getSkillId
 * 
 * Implementa a funcionalidade getSkillId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getSkillId() const{ return skillId_; }
    const std::string& getName() const { return name_; }
    /**
 * getLevel
 * 
 * Implementa a funcionalidade getLevel conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getLevel() const{ return level_; }
    /**
 * getType
 * 
 * Implementa a funcionalidade getType conforme especificação original.
 * @return Retorna SkillType
 */

    SkillType getType() const{ return type_; }
    /**
 * getTargetType
 * 
 * Implementa a funcionalidade getTargetType conforme especificação original.
 * @return Retorna SkillTargetType
 */

    SkillTargetType getTargetType() const{ return targetType_; }
    /**
 * getState
 * 
 * Implementa a funcionalidade getState conforme especificação original.
 * @return Retorna SkillState
 */

    SkillState getState() const{ return state_; }
    
    // Propriedades da habilidade
    /**
 * getCooldown
 * 
 * Implementa a funcionalidade getCooldown conforme especificação original.
 * @return Retorna float
 */

    float getCooldown() const{ return cooldown_; }
    /**
 * getCastTime
 * 
 * Implementa a funcionalidade getCastTime conforme especificação original.
 * @return Retorna float
 */

    float getCastTime() const{ return castTime_; }
    /**
 * getDuration
 * 
 * Implementa a funcionalidade getDuration conforme especificação original.
 * @return Retorna float
 */

    float getDuration() const{ return duration_; }
    /**
 * getArea
 * 
 * Implementa a funcionalidade getArea conforme especificação original.
 * @return Retorna float
 */

    float getArea() const{ return area_; }
    /**
 * getRange
 * 
 * Implementa a funcionalidade getRange conforme especificação original.
 * @return Retorna float
 */

    float getRange() const{ return range_; }
    
    // Propriedades temporais
    /**
 * getCooldownRemaining
 * 
 * Implementa a funcionalidade getCooldownRemaining conforme especificação original.
 * @return Retorna float
 */

    float getCooldownRemaining() const{ return cooldownRemaining_; }
    /**
 * getCastTimeRemaining
 * 
 * Implementa a funcionalidade getCastTimeRemaining conforme especificação original.
 * @return Retorna float
 */

    float getCastTimeRemaining() const{ return castTimeRemaining_; }
    /**
 * getDurationRemaining
 * 
 * Implementa a funcionalidade getDurationRemaining conforme especificação original.
 * @return Retorna float
 */

    float getDurationRemaining() const{ return durationRemaining_; }
    
    // Setters
    /**
 * setLevel
 * 
 * Implementa a funcionalidade setLevel conforme especificação original.
 * @param level Parâmetro level
 */

    void setLevel(uint8_t level){ level_ = level; }
    /**
 * setState
 * 
 * Implementa a funcionalidade setState conforme especificação original.
 * @param state Parâmetro state
 */

    void setState(SkillState state){ state_ = state; }
    /**
 * setCooldownRemaining
 * 
 * Implementa a funcionalidade setCooldownRemaining conforme especificação original.
 * @param time Parâmetro time
 */

    void setCooldownRemaining(float time){ cooldownRemaining_ = time; }
    /**
 * setCastTimeRemaining
 * 
 * Implementa a funcionalidade setCastTimeRemaining conforme especificação original.
 * @param time Parâmetro time
 */

    void setCastTimeRemaining(float time){ castTimeRemaining_ = time; }
    /**
 * setDurationRemaining
 * 
 * Implementa a funcionalidade setDurationRemaining conforme especificação original.
 * @param time Parâmetro time
 */

    void setDurationRemaining(float time){ durationRemaining_ = time; }
    
    // Efeitos da habilidade
    /**
 * getDamage
 * 
 * Implementa a funcionalidade getDamage conforme especificação original.
 * @return Retorna int32_t
 */

    int32_t getDamage() const;
    /**
 * getHealing
 * 
 * Implementa a funcionalidade getHealing conforme especificação original.
 * @return Retorna int32_t
 */

    int32_t getHealing() const;
    
    // Funções de atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void update(float deltaTime);
    
    // Serialização/desserialização
    std::vector<uint8_t> serialize() const;
    /**
 * deserialize
 * 
 * Implementa a funcionalidade deserialize conforme especificação original.
 * @param data Parâmetro data
 */

    void deserialize(const std::vector<uint8_t>& data);
    
private:
    // Identificação
    uint16_t skillId_;
    std::string name_;
    
    // Propriedades básicas
    uint8_t level_;
    SkillType type_;
    SkillTargetType targetType_;
    SkillState state_;
    
    // Tempos
    float cooldown_;
    float castTime_;
    float duration_;
    
    // Propriedades espaciais
    float area_;
    float range_;
    
    // Estado atual
    float cooldownRemaining_;
    float castTimeRemaining_;
    float durationRemaining_;
    
    // Método auxiliar
    /**
 * updateTimers
 * 
 * Implementa a funcionalidade updateTimers conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void updateTimers(float deltaTime);
};

#endif // WYD_SKILL_MANAGER_H

} // namespace wydbr
