/**
 * WYDAtmosphere.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/zones/WYDAtmosphere.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_ATMOSPHERE_H
#define WYD_ATMOSPHERE_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <memory>

namespace wydbr {


class WYDMap;

/**
 * Tipos de clima
 */
enum class WeatherType : uint8_t {
    CLEAR,         // Céu limpo
    RAIN,          // Chuva
    SNOW,          // Neve
    FOG,           // Neblina
    THUNDERSTORM,  // Tempestade com relâmpagos
    SANDSTORM,     // Tempestade de areia
    BLIZZARD       // Nevasca
};

/**
 * Período do dia
 */
enum class DayPeriod : uint8_t {
    DAWN,      // Amanhecer
    MORNING,   // Manhã
    NOON,      // Meio-dia
    AFTERNOON, // Tarde
    DUSK,      // Entardecer
    EVENING,   // Anoitecer
    NIGHT,     // Noite
    MIDNIGHT   // Meia-noite
};

/**
 * Configuração de luz
 */
struct LightConfig {
    uint8_t ambientR;     // Componente R da luz ambiente (0-255)
    uint8_t ambientG;     // Componente G da luz ambiente (0-255)
    uint8_t ambientB;     // Componente B da luz ambiente (0-255)
    uint8_t directionalR; // Componente R da luz direcional (0-255)
    uint8_t directionalG; // Componente G da luz direcional (0-255)
    uint8_t directionalB; // Componente B da luz direcional (0-255)
    float intensity;      // Intensidade da luz (0.0 - 1.0)
    float dirX;           // Direção da luz - X
    float dirY;           // Direção da luz - Y
    float dirZ;           // Direção da luz - Z
    
    // Construtor com valores padrão (luz neutra do meio-dia)
    LightConfig() 
        : ambientR(128), ambientG(128), ambientB(128),
          directionalR(255), directionalG(255), directionalB(255),
          intensity(1.0f), dirX(0.0f), dirY(-1.0f), dirZ(0.0f) {}
};

/**
 * Efeito atmosférico
 */
struct AtmosphericEffect {
    std::string name;      // Nome do efeito
    std::string particleType; // Tipo de partícula
    float density;         // Densidade (0.0 - 1.0)
    float speed;           // Velocidade
    uint8_t r, g, b, a;    // Cor e transparência (0-255)
    
    // Construtor com valores padrão
    AtmosphericEffect() 
        : name(""), particleType(""), density(0.0f), speed(0.0f),
          r(255), g(255), b(255), a(255) {}
};

/**
 * Sistema de atmosfera e clima
 * 
 * Gerencia as condições atmosféricas do mapa, incluindo clima,
 * ciclo dia/noite, iluminação, e efeitos visuais.
 */
/**
 * Classe WYDAtmosphere
 * 
 * Esta classe implementa a funcionalidade WYDAtmosphere conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDAtmosphere {
public:
    // Construtor
    /**
 * WYDAtmosphere
 * 
 * Implementa a funcionalidade WYDAtmosphere conforme especificação original.
 * @param map Parâmetro map
 * @return Retorna explicit
 */

    explicit WYDAtmosphere(WYDMap* map);
    
    // Destrutor
    ~WYDAtmosphere();
    
    // Carregamento de configuração
    /**
 * loadConfig
 * 
 * Implementa a funcionalidade loadConfig conforme especificação original.
 * @param filePath Parâmetro filePath
 * @return Retorna bool
 */

    bool loadConfig(const std::string& filePath);
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp);
    
    // Clima atual
    /**
 * getCurrentWeather
 * 
 * Implementa a funcionalidade getCurrentWeather conforme especificação original.
 * @return Retorna WeatherType
 */

    WeatherType getCurrentWeather() const{ return currentWeather_; }
    /**
 * getWeatherIntensity
 * 
 * Implementa a funcionalidade getWeatherIntensity conforme especificação original.
 * @return Retorna float
 */

    float getWeatherIntensity() const{ return weatherIntensity_; }
    /**
 * setWeather
 * 
 * Implementa a funcionalidade setWeather conforme especificação original.
 * @param weather Parâmetro weather
 * @param 1.0f Parâmetro 1.0f
 * @param 300000 Parâmetro 300000
 */

    void setWeather(WeatherType weather, float intensity = 1.0f, uint32_t durationMs = 300000);
    
    // Período do dia
    /**
 * getCurrentDayPeriod
 * 
 * Implementa a funcionalidade getCurrentDayPeriod conforme especificação original.
 * @return Retorna DayPeriod
 */

    DayPeriod getCurrentDayPeriod() const{ return currentDayPeriod_; }
    /**
 * getDayProgress
 * 
 * Implementa a funcionalidade getDayProgress conforme especificação original.
 * @return Retorna float
 */

    float getDayProgress() const{ return dayProgress_; }
    
    // Duração do dia (em ms)
    /**
 * setDayDuration
 * 
 * Implementa a funcionalidade setDayDuration conforme especificação original.
 * @param dayDurationMs Parâmetro dayDurationMs
 */

    void setDayDuration(uint32_t dayDurationMs);
    /**
 * getDayDuration
 * 
 * Implementa a funcionalidade getDayDuration conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getDayDuration() const{ return dayDurationMs_; }
    
    // Tempo atual
    /**
 * getCurrentTime
 * 
 * Implementa a funcionalidade getCurrentTime conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getCurrentTime() const{ return currentTime_; }
    /**
 * setCurrentTime
 * 
 * Implementa a funcionalidade setCurrentTime conforme especificação original.
 * @param time Parâmetro time
 */

    void setCurrentTime(uint64_t time);
    
    // Configuração de luz
    const LightConfig& getCurrentLight() const { return currentLight_; }
    
    // Efeitos atmosféricos
    /**
 * addEffect
 * 
 * Implementa a funcionalidade addEffect conforme especificação original.
 * @param effect Parâmetro effect
 */

    void addEffect(const AtmosphericEffect& effect);
    /**
 * removeEffect
 * 
 * Implementa a funcionalidade removeEffect conforme especificação original.
 * @param effectName Parâmetro effectName
 */

    void removeEffect(const std::string& effectName);
    /**
 * hasEffect
 * 
 * Implementa a funcionalidade hasEffect conforme especificação original.
 * @param effectName Parâmetro effectName
 * @return Retorna bool
 */

    bool hasEffect(const std::string& effectName) const;
    
    // Serialização para pacote de rede
    /**
 * serializeToPacket
 * 
 * Implementa a funcionalidade serializeToPacket conforme especificação original.
 * @return Retorna std::string
 */

    std::string serializeToPacket() const;
    
private:
    // Referência ao mapa
    WYDMap* map_;
    
    // Clima atual
    WeatherType currentWeather_;
    float weatherIntensity_;
    uint64_t weatherStartTime_;
    uint32_t weatherDurationMs_;
    
    // Clima programado (para transitions)
    bool hasScheduledWeather_;
    WeatherType scheduledWeather_;
    float scheduledWeatherIntensity_;
    uint64_t weatherTransitionTime_;
    
    // Ciclo dia/noite
    uint64_t currentTime_;
    DayPeriod currentDayPeriod_;
    float dayProgress_; // 0.0 = meia-noite, 0.5 = meio-dia, 1.0 = meia-noite
    uint32_t dayDurationMs_; // Duração do ciclo completo
    
    // Configuração de luz
    LightConfig currentLight_;
    
    // Configurações de luz pré-definidas para diferentes períodos do dia
    std::map<DayPeriod, LightConfig> periodLights_;
    
    // Efeitos atmosféricos ativos
    std::vector<AtmosphericEffect> activeEffects_;
    
    // Métodos auxiliares
    /**
 * updateDayNightCycle
 * 
 * Implementa a funcionalidade updateDayNightCycle conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateDayNightCycle(uint64_t timestamp);
    /**
 * updateWeather
 * 
 * Implementa a funcionalidade updateWeather conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateWeather(uint64_t timestamp);
    /**
 * updateLighting
 * 
 * Implementa a funcionalidade updateLighting conforme especificação original.
 */

    void updateLighting();
    /**
 * calculateDayPeriod
 * 
 * Implementa a funcionalidade calculateDayPeriod conforme especificação original.
 */

    void calculateDayPeriod();
    /**
 * interpolateLight
 * 
 * Implementa a funcionalidade interpolateLight conforme especificação original.
 * @param from Parâmetro from
 * @param to Parâmetro to
 * @param factor Parâmetro factor
 */

    void interpolateLight(const LightConfig& from, const LightConfig& to, float factor);
};

#endif // WYD_ATMOSPHERE_H

} // namespace wydbr
