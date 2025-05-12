/**
 * WYDTrigger.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/zones/WYDTrigger.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_TRIGGER_H
#define WYD_TRIGGER_H

#include <string>
#include <functional>
#include <set>
#include <memory>

namespace wydbr {


/**
 * Tipos de gatilhos
 */
enum class TriggerType : uint8_t {
    TELEPORT,      // Teleporta para outro local
    SCRIPT,        // Executa um script
    SPAWN,         // Spawna monstros
    EFFECT,        // Efeito visual
    QUEST,         // Progresso de quest
    SHOP,          // Abre loja
    DIALOGUE,      // Inicia diálogo
    TRAP,          // Armadilha
    EVENT          // Evento especial
};

/**
 * Estado de ativação de gatilhos
 */
enum class TriggerState : uint8_t {
    INACTIVE,  // Inativo
    ACTIVE,    // Ativo
    COOLDOWN,  // Em cooldown
    TRIGGERED, // Foi acionado
    DISABLED   // Desativado permanentemente
};

// Forward declaration
class WYDMap;

/**
 * Callback para gatilhos
 */
using TriggerCallback = std::function<void(uint32_t entityId, void* data)>;

/**
 * Gatilho de mapa
 * 
 * Um gatilho é um ponto ou área no mapa que desencadeia uma ação quando
 * uma entidade interage com ele, como teleportes, spawns, armadilhas, etc.
 */
/**
 * Classe WYDTrigger
 * 
 * Esta classe implementa a funcionalidade WYDTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDTrigger
 * 
 * Esta classe implementa a funcionalidade WYDTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDTrigger {
public:
    // /**
 * WYDTrigger
 * 
 * Implementa a funcionalidade WYDTrigger conforme especificação original.
 * @param type Parâmetro type
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    WYDTrigger(TriggerType type, uint16_t x, uint16_t y, const std::string& name = "");
    
    // Destrutor
    virtual ~WYDTrigger();
    
    // Getters
    /**
 * getType
 * 
 * Implementa a funcionalidade getType conforme especificação original.
 * @return Retorna TriggerType
 */

    TriggerType getType() const{ return type_; }
    /**
 * getState
 * 
 * Implementa a funcionalidade getState conforme especificação original.
 * @return Retorna TriggerState
 */

    TriggerState getState() const{ return state_; }
    const std::string& getName() const { return name_; }
    /**
 * getX
 * 
 * Implementa a funcionalidade getX conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getX() const{ return x_; }
    /**
 * getY
 * 
 * Implementa a funcionalidade getY conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getY() const{ return y_; }
    /**
 * getLastTriggerTime
 * 
 * Implementa a funcionalidade getLastTriggerTime conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getLastTriggerTime() const{ return lastTriggerTime_; }
    
    // Setters
    /**
 * setEnabled
 * 
 * Implementa a funcionalidade setEnabled conforme especificação original.
 * @param enabled Parâmetro enabled
 */

    void setEnabled(bool enabled);
    /**
 * setCooldown
 * 
 * Implementa a funcionalidade setCooldown conforme especificação original.
 * @param cooldownMs Parâmetro cooldownMs
 */

    void setCooldown(uint32_t cooldownMs){ cooldownMs_ = cooldownMs; }
    /**
 * setCallback
 * 
 * Implementa a funcionalidade setCallback conforme especificação original.
 * @param callback Parâmetro callback
 */

    void setCallback(TriggerCallback callback){ callback_ = callback; }
    /**
 * setUserData
 * 
 * Implementa a funcionalidade setUserData conforme especificação original.
 * @param data Parâmetro data
 */

    void setUserData(void* data){ userData_ = data; }
    
    // Acionamento do gatilho
    /**
 * trigger
 * 
 * Implementa a funcionalidade trigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool trigger(uint32_t entityId);
    
    // Reset do gatilho
    /**
 * reset
 * 
 * Implementa a funcionalidade reset conforme especificação original.
 */

    void reset();
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    virtual void update(uint64_t timestamp);
    
protected:
    // Tipo de gatilho
    TriggerType type_;
    
    // Estado atual
    TriggerState state_;
    
    // Nome do gatilho
    std::string name_;
    
    // Posição no mapa
    uint16_t x_, y_;
    
    // Cooldown entre acionamentos (em ms)
    uint32_t cooldownMs_;
    
    // Último tempo de acionamento
    uint64_t lastTriggerTime_;
    
    // Callback para acionamento
    TriggerCallback callback_;
    
    // Dados adicionais do usuário
    void* userData_;
    
    // Método para ação específica do tipo de gatilho
    virtual bool onTrigger(uint32_t entityId) = 0;
};

/**
 * Gatilho de Teleporte
 * 
 * Teleporta uma entidade para outro local, possivelmente em outro mapa.
 */
/**
 * Classe WYDTeleportTrigger
 * 
 * Esta classe implementa a funcionalidade WYDTeleportTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDTeleportTrigger
 * 
 * Esta classe implementa a funcionalidade WYDTeleportTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TeleportTrigger : public WYDTrigger {
public:
    // /**
 * TeleportTrigger
 * 
 * Implementa a funcionalidade TeleportTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    TeleportTrigger(uint16_t x, uint16_t y, const std::string& name = "");
    
    // Getters
    const std::string& getDestinationMap() const { return destMap_; }
    /**
 * getDestinationX
 * 
 * Implementa a funcionalidade getDestinationX conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getDestinationX() const{ return destX_; }
    /**
 * getDestinationY
 * 
 * Implementa a funcionalidade getDestinationY conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getDestinationY() const{ return destY_; }
    
    // Setters
    /**
 * setDestination
 * 
 * Implementa a funcionalidade setDestination conforme especificação original.
 * @param destMap Parâmetro destMap
 * @param destX Parâmetro destX
 * @param destY Parâmetro destY
 */

    void setDestination(const std::string& destMap, uint16_t destX, uint16_t destY);
    
protected:
    // Ação específica para teleporte
    /**
 * onTrigger
 * 
 * Implementa a funcionalidade onTrigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool onTrigger(uint32_t entityId) override;
    
private:
    // Destino do teleporte
    std::string destMap_;
    uint16_t destX_, destY_;
};

/**
 * Gatilho de Script
 * 
 * Executa um script quando acionado.
 */
/**
 * Classe WYDScriptTrigger
 * 
 * Esta classe implementa a funcionalidade WYDScriptTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDScriptTrigger
 * 
 * Esta classe implementa a funcionalidade WYDScriptTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ScriptTrigger : public WYDTrigger {
public:
    // /**
 * ScriptTrigger
 * 
 * Implementa a funcionalidade ScriptTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    ScriptTrigger(uint16_t x, uint16_t y, const std::string& name = "");
    
    // Getters
    const std::string& getScriptName() const { return scriptName_; }
    
    // Setters
    /**
 * setScriptName
 * 
 * Implementa a funcionalidade setScriptName conforme especificação original.
 * @param scriptName Parâmetro scriptName
 */

    void setScriptName(const std::string& scriptName){ scriptName_ = scriptName; }
    
protected:
    // Ação específica para script
    /**
 * onTrigger
 * 
 * Implementa a funcionalidade onTrigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool onTrigger(uint32_t entityId) override;
    
private:
    // Nome do script a executar
    std::string scriptName_;
};

/**
 * Gatilho de Spawn
 * 
 * Spawna monstros quando acionado.
 */
/**
 * Classe WYDSpawnTrigger
 * 
 * Esta classe implementa a funcionalidade WYDSpawnTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDSpawnTrigger
 * 
 * Esta classe implementa a funcionalidade WYDSpawnTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SpawnTrigger : public WYDTrigger {
public:
    // /**
 * SpawnTrigger
 * 
 * Implementa a funcionalidade SpawnTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    SpawnTrigger(uint16_t x, uint16_t y, const std::string& name = "");
    
    // Getters
    /**
 * getMonsterId
 * 
 * Implementa a funcionalidade getMonsterId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getMonsterId() const{ return monsterId_; }
    /**
 * getCount
 * 
 * Implementa a funcionalidade getCount conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getCount() const{ return count_; }
    /**
 * getRadius
 * 
 * Implementa a funcionalidade getRadius conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getRadius() const{ return radius_; }
    
    // Setters
    /**
 * setMonsterId
 * 
 * Implementa a funcionalidade setMonsterId conforme especificação original.
 * @param monsterId Parâmetro monsterId
 */

    void setMonsterId(uint32_t monsterId){ monsterId_ = monsterId; }
    /**
 * setCount
 * 
 * Implementa a funcionalidade setCount conforme especificação original.
 * @param count Parâmetro count
 */

    void setCount(uint16_t count){ count_ = count; }
    /**
 * setRadius
 * 
 * Implementa a funcionalidade setRadius conforme especificação original.
 * @param radius Parâmetro radius
 */

    void setRadius(uint16_t radius){ radius_ = radius; }
    
protected:
    // Ação específica para spawn
    /**
 * onTrigger
 * 
 * Implementa a funcionalidade onTrigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool onTrigger(uint32_t entityId) override;
    
private:
    // ID do monstro a spawnar
    uint32_t monsterId_;
    
    // Quantidade de monstros
    uint16_t count_;
    
    // Raio de spawn
    uint16_t radius_;
    
    // IDs dos monstros spawados
    std::set<uint32_t> spawnedMonsters_;
};

/**
 * Gatilho de Efeito
 * 
 * Exibe um efeito visual quando acionado.
 */
/**
 * Classe WYDEffectTrigger
 * 
 * Esta classe implementa a funcionalidade WYDEffectTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDEffectTrigger
 * 
 * Esta classe implementa a funcionalidade WYDEffectTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EffectTrigger : public WYDTrigger {
public:
    // /**
 * EffectTrigger
 * 
 * Implementa a funcionalidade EffectTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    EffectTrigger(uint16_t x, uint16_t y, const std::string& name = "");
    
    // Getters
    /**
 * getEffectId
 * 
 * Implementa a funcionalidade getEffectId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getEffectId() const{ return effectId_; }
    /**
 * getDuration
 * 
 * Implementa a funcionalidade getDuration conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getDuration() const{ return durationMs_; }
    
    // Setters
    /**
 * setEffectId
 * 
 * Implementa a funcionalidade setEffectId conforme especificação original.
 * @param effectId Parâmetro effectId
 */

    void setEffectId(uint16_t effectId){ effectId_ = effectId; }
    /**
 * setDuration
 * 
 * Implementa a funcionalidade setDuration conforme especificação original.
 * @param durationMs Parâmetro durationMs
 */

    void setDuration(uint16_t durationMs){ durationMs_ = durationMs; }
    
protected:
    // Ação específica para efeito
    /**
 * onTrigger
 * 
 * Implementa a funcionalidade onTrigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool onTrigger(uint32_t entityId) override;
    
private:
    // ID do efeito
    uint16_t effectId_;
    
    // Duração do efeito em ms
    uint16_t durationMs_;
};

/**
 * Gatilho de Quest
 * 
 * Atualiza o progresso de uma quest quando acionado.
 */
/**
 * Classe WYDQuestTrigger
 * 
 * Esta classe implementa a funcionalidade WYDQuestTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDQuestTrigger
 * 
 * Esta classe implementa a funcionalidade WYDQuestTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestTrigger : public WYDTrigger {
public:
    // /**
 * QuestTrigger
 * 
 * Implementa a funcionalidade QuestTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    QuestTrigger(uint16_t x, uint16_t y, const std::string& name = "");
    
    // Getters
    /**
 * getQuestId
 * 
 * Implementa a funcionalidade getQuestId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getQuestId() const{ return questId_; }
    /**
 * getStepId
 * 
 * Implementa a funcionalidade getStepId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getStepId() const{ return stepId_; }
    
    // Setters
    /**
 * setQuestId
 * 
 * Implementa a funcionalidade setQuestId conforme especificação original.
 * @param questId Parâmetro questId
 */

    void setQuestId(uint32_t questId){ questId_ = questId; }
    /**
 * setStepId
 * 
 * Implementa a funcionalidade setStepId conforme especificação original.
 * @param stepId Parâmetro stepId
 */

    void setStepId(uint16_t stepId){ stepId_ = stepId; }
    
protected:
    // Ação específica para quest
    /**
 * onTrigger
 * 
 * Implementa a funcionalidade onTrigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool onTrigger(uint32_t entityId) override;
    
private:
    // ID da quest
    uint32_t questId_;
    
    // ID do passo da quest
    uint16_t stepId_;
};

/**
 * Gatilho de Loja
 * 
 * Abre uma loja quando acionado.
 */
/**
 * Classe WYDShopTrigger
 * 
 * Esta classe implementa a funcionalidade WYDShopTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDShopTrigger
 * 
 * Esta classe implementa a funcionalidade WYDShopTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ShopTrigger : public WYDTrigger {
public:
    // /**
 * ShopTrigger
 * 
 * Implementa a funcionalidade ShopTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    ShopTrigger(uint16_t x, uint16_t y, const std::string& name = "");
    
    // Getters
    /**
 * getShopId
 * 
 * Implementa a funcionalidade getShopId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getShopId() const{ return shopId_; }
    
    // Setters
    /**
 * setShopId
 * 
 * Implementa a funcionalidade setShopId conforme especificação original.
 * @param shopId Parâmetro shopId
 */

    void setShopId(uint32_t shopId){ shopId_ = shopId; }
    
protected:
    // Ação específica para loja
    /**
 * onTrigger
 * 
 * Implementa a funcionalidade onTrigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool onTrigger(uint32_t entityId) override;
    
private:
    // ID da loja
    uint32_t shopId_;
};

/**
 * Gatilho de Diálogo
 * 
 * Inicia um diálogo quando acionado.
 */
/**
 * Classe WYDDialogueTrigger
 * 
 * Esta classe implementa a funcionalidade WYDDialogueTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDDialogueTrigger
 * 
 * Esta classe implementa a funcionalidade WYDDialogueTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DialogueTrigger : public WYDTrigger {
public:
    // /**
 * DialogueTrigger
 * 
 * Implementa a funcionalidade DialogueTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    DialogueTrigger(uint16_t x, uint16_t y, const std::string& name = "");
    
    // Getters
    /**
 * getNpcId
 * 
 * Implementa a funcionalidade getNpcId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getNpcId() const{ return npcId_; }
    /**
 * getDialogueId
 * 
 * Implementa a funcionalidade getDialogueId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getDialogueId() const{ return dialogueId_; }
    
    // Setters
    /**
 * setNpcId
 * 
 * Implementa a funcionalidade setNpcId conforme especificação original.
 * @param npcId Parâmetro npcId
 */

    void setNpcId(uint32_t npcId){ npcId_ = npcId; }
    /**
 * setDialogueId
 * 
 * Implementa a funcionalidade setDialogueId conforme especificação original.
 * @param dialogueId Parâmetro dialogueId
 */

    void setDialogueId(uint32_t dialogueId){ dialogueId_ = dialogueId; }
    
protected:
    // Ação específica para diálogo
    /**
 * onTrigger
 * 
 * Implementa a funcionalidade onTrigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool onTrigger(uint32_t entityId) override;
    
private:
    // ID do NPC
    uint32_t npcId_;
    
    // ID do diálogo
    uint32_t dialogueId_;
};

/**
 * Gatilho de Armadilha
 * 
 * Causa dano ou efeito negativo quando acionado.
 */
/**
 * Classe WYDTrapTrigger
 * 
 * Esta classe implementa a funcionalidade WYDTrapTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDTrapTrigger
 * 
 * Esta classe implementa a funcionalidade WYDTrapTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TrapTrigger : public WYDTrigger {
public:
    // /**
 * TrapTrigger
 * 
 * Implementa a funcionalidade TrapTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    TrapTrigger(uint16_t x, uint16_t y, const std::string& name = "");
    
    // Getters
    /**
 * getDamage
 * 
 * Implementa a funcionalidade getDamage conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getDamage() const{ return damage_; }
    const std::string& getEffectType() const { return effectType_; }
    /**
 * getEffectDuration
 * 
 * Implementa a funcionalidade getEffectDuration conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getEffectDuration() const{ return effectDurationMs_; }
    
    // Setters
    /**
 * setDamage
 * 
 * Implementa a funcionalidade setDamage conforme especificação original.
 * @param damage Parâmetro damage
 */

    void setDamage(uint32_t damage){ damage_ = damage; }
    /**
 * setEffect
 * 
 * Implementa a funcionalidade setEffect conforme especificação original.
 * @param effectType Parâmetro effectType
 * @param durationMs Parâmetro durationMs
 */

    void setEffect(const std::string& effectType, uint16_t durationMs){ 
        effectType_ = effectType; 
        effectDurationMs_ = durationMs;
    }
    
protected:
    // Ação específica para armadilha
    /**
 * onTrigger
 * 
 * Implementa a funcionalidade onTrigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool onTrigger(uint32_t entityId) override;
    
private:
    // Dano causado
    uint32_t damage_;
    
    // Tipo de efeito negativo
    std::string effectType_;
    
    // Duração do efeito em ms
    uint16_t effectDurationMs_;
};

/**
 * Gatilho de Evento
 * 
 * Inicia um evento especial quando acionado.
 */
/**
 * Classe WYDEventTrigger
 * 
 * Esta classe implementa a funcionalidade WYDEventTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDEventTrigger
 * 
 * Esta classe implementa a funcionalidade WYDEventTrigger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventTrigger : public WYDTrigger {
public:
    // /**
 * EventTrigger
 * 
 * Implementa a funcionalidade EventTrigger conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param "" Parâmetro ""
 * @return Retorna Construtor
 */
 Construtor
    EventTrigger(uint16_t x, uint16_t y, const std::string& name = "");
    
    // Getters
    /**
 * getEventId
 * 
 * Implementa a funcionalidade getEventId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getEventId() const{ return eventId_; }
    
    // Setters
    /**
 * setEventId
 * 
 * Implementa a funcionalidade setEventId conforme especificação original.
 * @param eventId Parâmetro eventId
 */

    void setEventId(uint32_t eventId){ eventId_ = eventId; }
    
protected:
    // Ação específica para evento
    /**
 * onTrigger
 * 
 * Implementa a funcionalidade onTrigger conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool onTrigger(uint32_t entityId) override;
    
private:
    // ID do evento
    uint32_t eventId_;
};

/**
 * Fábrica de gatilhos
 * 
 * Cria gatilhos de diferentes tipos.
 */
/**
 * Classe WYDTriggerFactory
 * 
 * Esta classe implementa a funcionalidade WYDTriggerFactory conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDTriggerFactory
 * 
 * Esta classe implementa a funcionalidade WYDTriggerFactory conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TriggerFactory {
public:
    // Criação de gatilhos
    static std::unique_ptr<WYDTrigger> createTrigger(
        TriggerType type, 
        uint16_t x, 
        uint16_t y, 
        const std::string& name = ""
    );
};

#endif // WYD_TRIGGER_H

} // namespace wydbr
