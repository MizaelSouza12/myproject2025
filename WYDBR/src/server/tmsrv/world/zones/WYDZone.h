/**
 * WYDZone.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/zones/WYDZone.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_ZONE_H
#define WYD_ZONE_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

#include "headers/wyd_core.h"

namespace wydbr {


class WYDMap;
class WYDZoneEvent;

/**
 * Tipos de zonas
 */
enum class ZoneType : uint8_t {
    NORMAL,    // Zona normal
    ARENA,     // Arena PvP
    GUILD,     // Área de guilda
    BOSS,      // Área de boss
    EVENT,     // Área de evento
    BATTLE,    // Campo de batalha
    CASTLE,    // Castelo
    DUNGEON,   // Dungeon
    SAFEZONE   // Zona segura
};

/**
 * Propriedades de zonas
 */
enum class ZoneProperty : uint8_t {
    ALLOW_PVP,        // Permite PvP
    ALLOW_TELEPORT,   // Permite teleporte
    ALLOW_TRADE,      // Permite comércio
    ALLOW_MOUNT,      // Permite montarias
    ALLOW_FLY,        // Permite vôo
    ALLOW_RESURRECTION, // Permite ressurreição
    SPECIAL_WEATHER,  // Clima especial
    SPECIAL_LIGHT,    // Iluminação especial
    DRAINS_HP,        // Drena HP
    DRAINS_MP,        // Drena MP
    RESTRICTED_ACCESS, // Acesso restrito
    GUILDS_ONLY,      // Apenas membros de guilda
    PARTY_ONLY,       // Apenas membros de grupo
    LEVEL_RESTRICTION, // Restrição de nível
    QUEST_RESTRICTION, // Restrição por quest
    SPAWN_POINT       // Ponto de spawn
};

/**
 * Zona de um mapa
 * 
 * Uma zona representa uma área dentro de um mapa com propriedades específicas,
 * como regras de PvP, eventos especiais, e outros comportamentos.
 */
/**
 * Classe WYDZone
 * 
 * Esta classe implementa a funcionalidade WYDZone conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDZone
 * 
 * Esta classe implementa a funcionalidade WYDZone conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDZone {
public:
    // /**
 * WYDZone
 * 
 * Implementa a funcionalidade WYDZone conforme especificação original.
 * @param map Parâmetro map
 * @param type Parâmetro type
 * @param name Parâmetro name
 * @return Retorna Construtor
 */
 Construtor
    WYDZone(WYDMap* map, ZoneType type, const std::string& name);
    
    // Destrutor
    virtual ~WYDZone();
    
    // Propriedades básicas
    /**
 * getType
 * 
 * Implementa a funcionalidade getType conforme especificação original.
 * @return Retorna ZoneType
 */

    ZoneType getType() const{ return type_; }
    const std::string& getName() const { return name_; }
    
    // Área da zona
    /**
 * setBounds
 * 
 * Implementa a funcionalidade setBounds conforme especificação original.
 * @param x1 Parâmetro x1
 * @param y1 Parâmetro y1
 * @param x2 Parâmetro x2
 * @param y2 Parâmetro y2
 */

    void setBounds(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    /**
 * containsPoint
 * 
 * Implementa a funcionalidade containsPoint conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool containsPoint(uint16_t x, uint16_t y) const;
    
    // Propriedades da zona
    /**
 * setProperty
 * 
 * Implementa a funcionalidade setProperty conforme especificação original.
 * @param property Parâmetro property
 * @param value Parâmetro value
 */

    void setProperty(ZoneProperty property, bool value);
    /**
 * hasProperty
 * 
 * Implementa a funcionalidade hasProperty conforme especificação original.
 * @param property Parâmetro property
 * @return Retorna bool
 */

    bool hasProperty(ZoneProperty property) const;
    
    // Restrições de nível
    /**
 * setLevelRange
 * 
 * Implementa a funcionalidade setLevelRange conforme especificação original.
 * @param minLevel Parâmetro minLevel
 * @param maxLevel Parâmetro maxLevel
 */

    void setLevelRange(uint16_t minLevel, uint16_t maxLevel);
    /**
 * isLevelAllowed
 * 
 * Implementa a funcionalidade isLevelAllowed conforme especificação original.
 * @param level Parâmetro level
 * @return Retorna bool
 */

    bool isLevelAllowed(uint16_t level) const;
    
    // Restrições de guilda
    /**
 * setAllowedGuild
 * 
 * Implementa a funcionalidade setAllowedGuild conforme especificação original.
 * @param guildId Parâmetro guildId
 */

    void setAllowedGuild(uint32_t guildId);
    /**
 * clearAllowedGuild
 * 
 * Implementa a funcionalidade clearAllowedGuild conforme especificação original.
 */

    void clearAllowedGuild();
    /**
 * isGuildAllowed
 * 
 * Implementa a funcionalidade isGuildAllowed conforme especificação original.
 * @param guildId Parâmetro guildId
 * @return Retorna bool
 */

    bool isGuildAllowed(uint32_t guildId) const;
    
    // Entidades na zona
    /**
 * addEntity
 * 
 * Implementa a funcionalidade addEntity conforme especificação original.
 * @param entityId Parâmetro entityId
 */

    void addEntity(uint32_t entityId);
    /**
 * removeEntity
 * 
 * Implementa a funcionalidade removeEntity conforme especificação original.
 * @param entityId Parâmetro entityId
 */

    void removeEntity(uint32_t entityId);
    /**
 * containsEntity
 * 
 * Implementa a funcionalidade containsEntity conforme especificação original.
 * @param entityId Parâmetro entityId
 * @return Retorna bool
 */

    bool containsEntity(uint32_t entityId) const;
    const std::set<uint32_t>& getEntities() const { return entities_; }
    
    // Eventos da zona
    /**
 * addEvent
 * 
 * Implementa a funcionalidade addEvent conforme especificação original.
 * @param event Parâmetro event
 */

    void addEvent(std::unique_ptr<WYDZoneEvent> event);
    /**
 * removeEvent
 * 
 * Implementa a funcionalidade removeEvent conforme especificação original.
 * @param eventId Parâmetro eventId
 */

    void removeEvent(const std::string& eventId);
    /**
 * getEvent
 * 
 * Implementa a funcionalidade getEvent conforme especificação original.
 * @param eventId Parâmetro eventId
 * @return Retorna WYDZoneEvent
 */

    WYDZoneEvent* getEvent(const std::string& eventId);
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    virtual void update(uint64_t timestamp);
    
    // Método trigger para quando entidade entra na zona
    /**
 * onEntityEnter
 * 
 * Implementa a funcionalidade onEntityEnter conforme especificação original.
 * @param entityId Parâmetro entityId
 */

    virtual void onEntityEnter(uint32_t entityId);
    
    // Método trigger para quando entidade sai da zona
    /**
 * onEntityLeave
 * 
 * Implementa a funcionalidade onEntityLeave conforme especificação original.
 * @param entityId Parâmetro entityId
 */

    virtual void onEntityLeave(uint32_t entityId);
    
protected:
    // Referência ao mapa
    WYDMap* map_;
    
    // Propriedades básicas
    ZoneType type_;
    std::string name_;
    
    // Área da zona
    uint16_t x1_, y1_, x2_, y2_;
    
    // Conjunto de propriedades
    std::set<ZoneProperty> properties_;
    
    // Restrições
    uint16_t minLevel_, maxLevel_;
    uint32_t allowedGuildId_;
    
    // Entidades na zona
    std::set<uint32_t> entities_;
    
    // Eventos da zona
    std::map<std::string, std::unique_ptr<WYDZoneEvent>> events_;
};

/**
 * Evento de zona
 * 
 * Um evento representa algo que ocorre em uma zona, como spawn de monstros,
 * clima especial, efeitos periódicos, etc.
 */
/**
 * Classe WYDZoneEvent
 * 
 * Esta classe implementa a funcionalidade WYDZoneEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDZoneEvent
 * 
 * Esta classe implementa a funcionalidade WYDZoneEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDZoneEvent {
public:
    // /**
 * WYDZoneEvent
 * 
 * Implementa a funcionalidade WYDZoneEvent conforme especificação original.
 * @param id Parâmetro id
 * @param zone Parâmetro zone
 * @return Retorna Construtor
 */
 Construtor
    WYDZoneEvent(const std::string& id, WYDZone* zone);
    
    // Destrutor
    virtual ~WYDZoneEvent();
    
    // Propriedades básicas
    const std::string& getId() const { return id_; }
    /**
 * isActive
 * 
 * Implementa a funcionalidade isActive conforme especificação original.
 * @return Retorna bool
 */

    bool isActive() const{ return active_; }
    
    // Ativação/desativação
    /**
 * activate
 * 
 * Implementa a funcionalidade activate conforme especificação original.
 */

    virtual void activate();
    /**
 * deactivate
 * 
 * Implementa a funcionalidade deactivate conforme especificação original.
 */

    virtual void deactivate();
    
    // Atualização
    virtual void update(uint64_t timestamp) = 0;
    
protected:
    // Identificação
    std::string id_;
    
    // Estado
    bool active_;
    
    // Referência à zona
    WYDZone* zone_;
};

/**
 * Evento de spawn de monstros
 */
/**
 * Classe WYDMonsterSpawnEvent
 * 
 * Esta classe implementa a funcionalidade WYDMonsterSpawnEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDMonsterSpawnEvent
 * 
 * Esta classe implementa a funcionalidade WYDMonsterSpawnEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MonsterSpawnEvent : public WYDZoneEvent {
public:
    // /**
 * MonsterSpawnEvent
 * 
 * Implementa a funcionalidade MonsterSpawnEvent conforme especificação original.
 * @param id Parâmetro id
 * @param zone Parâmetro zone
 * @return Retorna Construtor
 */
 Construtor
    MonsterSpawnEvent(const std::string& id, WYDZone* zone);
    
    // Configuração
    /**
 * setMonsterId
 * 
 * Implementa a funcionalidade setMonsterId conforme especificação original.
 * @param monsterId Parâmetro monsterId
 */

    void setMonsterId(uint32_t monsterId){ monsterId_ = monsterId; }
    /**
 * setSpawnCount
 * 
 * Implementa a funcionalidade setSpawnCount conforme especificação original.
 * @param count Parâmetro count
 */

    void setSpawnCount(uint16_t count){ spawnCount_ = count; }
    /**
 * setSpawnInterval
 * 
 * Implementa a funcionalidade setSpawnInterval conforme especificação original.
 * @param intervalMs Parâmetro intervalMs
 */

    void setSpawnInterval(uint32_t intervalMs){ spawnIntervalMs_ = intervalMs; }
    
    // Ativação específica
    /**
 * activate
 * 
 * Implementa a funcionalidade activate conforme especificação original.
 */

    void activate() override;
    
    // Desativação específica
    /**
 * deactivate
 * 
 * Implementa a funcionalidade deactivate conforme especificação original.
 */

    void deactivate() override;
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp) override;
    
private:
    // Propriedades específicas
    uint32_t monsterId_;
    uint16_t spawnCount_;
    uint32_t spawnIntervalMs_;
    
    // Estado de spawn
    uint64_t lastSpawnTime_;
    std::vector<uint32_t> spawnedMonsters_;
    
    // Método para spawn
    /**
 * spawnMonsters
 * 
 * Implementa a funcionalidade spawnMonsters conforme especificação original.
 */

    void spawnMonsters();
};

/**
 * Evento de clima especial
 */
/**
 * Classe WYDWeatherEvent
 * 
 * Esta classe implementa a funcionalidade WYDWeatherEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDWeatherEvent
 * 
 * Esta classe implementa a funcionalidade WYDWeatherEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WeatherEvent : public WYDZoneEvent {
public:
    // Tipos de clima
    enum /**
 * Classe WYDWeatherType
 * 
 * Esta classe implementa a funcionalidade WYDWeatherType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDWeatherType
 * 
 * Esta classe implementa a funcionalidade WYDWeatherType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WeatherType {
        CLEAR,
        RAIN,
        SNOW,
        FOG,
        THUNDERSTORM,
        SANDSTORM,
        BLIZZARD
    };
    
    // /**
 * WeatherEvent
 * 
 * Implementa a funcionalidade WeatherEvent conforme especificação original.
 * @param id Parâmetro id
 * @param zone Parâmetro zone
 * @return Retorna Construtor
 */
 Construtor
    WeatherEvent(const std::string& id, WYDZone* zone);
    
    // Configuração
    /**
 * setWeatherType
 * 
 * Implementa a funcionalidade setWeatherType conforme especificação original.
 * @param type Parâmetro type
 */

    void setWeatherType(WeatherType type){ weatherType_ = type; }
    /**
 * setDuration
 * 
 * Implementa a funcionalidade setDuration conforme especificação original.
 * @param durationMs Parâmetro durationMs
 */

    void setDuration(uint32_t durationMs){ durationMs_ = durationMs; }
    /**
 * setIntensity
 * 
 * Implementa a funcionalidade setIntensity conforme especificação original.
 * @param intensity Parâmetro intensity
 */

    void setIntensity(float intensity){ intensity_ = intensity; }
    
    // Ativação específica
    /**
 * activate
 * 
 * Implementa a funcionalidade activate conforme especificação original.
 */

    void activate() override;
    
    // Desativação específica
    /**
 * deactivate
 * 
 * Implementa a funcionalidade deactivate conforme especificação original.
 */

    void deactivate() override;
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp) override;
    
private:
    // Propriedades específicas
    WeatherType weatherType_;
    uint32_t durationMs_;
    float intensity_;
    
    // Estado do clima
    uint64_t startTime_;
    bool fading_;
};

/**
 * Evento de efeito de zona
 */
/**
 * Classe WYDZoneEffectEvent
 * 
 * Esta classe implementa a funcionalidade WYDZoneEffectEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDZoneEffectEvent
 * 
 * Esta classe implementa a funcionalidade WYDZoneEffectEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ZoneEffectEvent : public WYDZoneEvent {
public:
    // /**
 * ZoneEffectEvent
 * 
 * Implementa a funcionalidade ZoneEffectEvent conforme especificação original.
 * @param id Parâmetro id
 * @param zone Parâmetro zone
 * @return Retorna Construtor
 */
 Construtor
    ZoneEffectEvent(const std::string& id, WYDZone* zone);
    
    // Configuração
    /**
 * setEffectType
 * 
 * Implementa a funcionalidade setEffectType conforme especificação original.
 * @param type Parâmetro type
 */

    void setEffectType(const std::string& type){ effectType_ = type; }
    /**
 * setTickInterval
 * 
 * Implementa a funcionalidade setTickInterval conforme especificação original.
 * @param intervalMs Parâmetro intervalMs
 */

    void setTickInterval(uint32_t intervalMs){ tickIntervalMs_ = intervalMs; }
    /**
 * setEffectMagnitude
 * 
 * Implementa a funcionalidade setEffectMagnitude conforme especificação original.
 * @param magnitude Parâmetro magnitude
 */

    void setEffectMagnitude(int32_t magnitude){ effectMagnitude_ = magnitude; }
    
    // Ativação específica
    /**
 * activate
 * 
 * Implementa a funcionalidade activate conforme especificação original.
 */

    void activate() override;
    
    // Desativação específica
    /**
 * deactivate
 * 
 * Implementa a funcionalidade deactivate conforme especificação original.
 */

    void deactivate() override;
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp) override;
    
private:
    // Propriedades específicas
    std::string effectType_;
    uint32_t tickIntervalMs_;
    int32_t effectMagnitude_;
    
    // Estado do efeito
    uint64_t lastTickTime_;
    
    // Aplicação do efeito
    /**
 * applyEffect
 * 
 * Implementa a funcionalidade applyEffect conforme especificação original.
 */

    void applyEffect();
};

#endif // WYD_ZONE_H

} // namespace wydbr
