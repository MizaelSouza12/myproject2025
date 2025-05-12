/**
 * WYDAtmosphere.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/zones/WYDAtmosphere.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/world/zones/WYDAtmosphere.h"
#include "src/server/tmsrv/world/WYDMap.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <ctime>

namespace wydbr {


/**
 * Implementação de WYDAtmosphere
 */

// Constructor
WYDAtmosphere::WYDAtmosphere(WYDMap* map)
    : map_(map), 
      currentWeather_(WeatherType::CLEAR), 
      weatherIntensity_(0.0f),
      weatherStartTime_(0),
      weatherDurationMs_(0),
      hasScheduledWeather_(false),
      scheduledWeather_(WeatherType::CLEAR),
      scheduledWeatherIntensity_(0.0f),
      weatherTransitionTime_(0),
      currentTime_(0),
      currentDayPeriod_(DayPeriod::DAWN),
      dayProgress_(0.0f),
      dayDurationMs_(24 * 60 * 60 * 1000) // 24 horas por padrão
{
    // Inicializa configurações de luz para diferentes períodos do dia
    // Valores são simplificados; em uma implementação real seriam calibrados
    
    // Amanhecer (tons rosados/alaranjados)
    LightConfig dawnLight;
    dawnLight.ambientR = 100;
    dawnLight.ambientG = 70;
    dawnLight.ambientB = 70;
    dawnLight.directionalR = 255;
    dawnLight.directionalG = 180;
    dawnLight.directionalB = 150;
    dawnLight.intensity = 0.7f;
    dawnLight.dirX = 0.5f;
    dawnLight.dirY = 0.1f;
    dawnLight.dirZ = 0.8f;
    periodLights_[DayPeriod::DAWN] = dawnLight;
    
    // Manhã (luz amarelada)
    LightConfig morningLight;
    morningLight.ambientR = 110;
    morningLight.ambientG = 110;
    morningLight.ambientB = 90;
    morningLight.directionalR = 255;
    morningLight.directionalG = 240;
    morningLight.directionalB = 200;
    morningLight.intensity = 0.9f;
    morningLight.dirX = 0.3f;
    morningLight.dirY = 0.5f;
    morningLight.dirZ = 0.8f;
    periodLights_[DayPeriod::MORNING] = morningLight;
    
    // Meio-dia (luz branca intensa)
    LightConfig noonLight;
    noonLight.ambientR = 140;
    noonLight.ambientG = 140;
    noonLight.ambientB = 140;
    noonLight.directionalR = 255;
    noonLight.directionalG = 255;
    noonLight.directionalB = 255;
    noonLight.intensity = 1.0f;
    noonLight.dirX = 0.0f;
    noonLight.dirY = 1.0f;
    noonLight.dirZ = 0.0f;
    periodLights_[DayPeriod::NOON] = noonLight;
    
    // Tarde (luz amarelada suave)
    LightConfig afternoonLight;
    afternoonLight.ambientR = 130;
    afternoonLight.ambientG = 130;
    afternoonLight.ambientB = 110;
    afternoonLight.directionalR = 250;
    afternoonLight.directionalG = 240;
    afternoonLight.directionalB = 190;
    afternoonLight.intensity = 0.95f;
    afternoonLight.dirX = -0.3f;
    afternoonLight.dirY = 0.5f;
    afternoonLight.dirZ = 0.8f;
    periodLights_[DayPeriod::AFTERNOON] = afternoonLight;
    
    // Entardecer (tons alaranjados/avermelhados)
    LightConfig duskLight;
    duskLight.ambientR = 110;
    duskLight.ambientG = 80;
    duskLight.ambientB = 70;
    duskLight.directionalR = 255;
    duskLight.directionalG = 170;
    duskLight.directionalB = 100;
    duskLight.intensity = 0.7f;
    duskLight.dirX = -0.7f;
    duskLight.dirY = 0.1f;
    duskLight.dirZ = 0.7f;
    periodLights_[DayPeriod::DUSK] = duskLight;
    
    // Anoitecer (tons azulados)
    LightConfig eveningLight;
    eveningLight.ambientR = 50;
    eveningLight.ambientG = 50;
    eveningLight.ambientB = 80;
    eveningLight.directionalR = 80;
    eveningLight.directionalG = 80;
    eveningLight.directionalB = 150;
    eveningLight.intensity = 0.4f;
    eveningLight.dirX = -0.8f;
    eveningLight.dirY = -0.1f;
    eveningLight.dirZ = 0.6f;
    periodLights_[DayPeriod::EVENING] = eveningLight;
    
    // Noite (luz azul escura)
    LightConfig nightLight;
    nightLight.ambientR = 20;
    nightLight.ambientG = 20;
    nightLight.ambientB = 40;
    nightLight.directionalR = 40;
    nightLight.directionalG = 40;
    nightLight.directionalB = 80;
    nightLight.intensity = 0.2f;
    nightLight.dirX = 0.0f;
    nightLight.dirY = -0.8f;
    nightLight.dirZ = 0.6f;
    periodLights_[DayPeriod::NIGHT] = nightLight;
    
    // Meia-noite (luz azul muito escura)
    LightConfig midnightLight;
    midnightLight.ambientR = 10;
    midnightLight.ambientG = 10;
    midnightLight.ambientB = 30;
    midnightLight.directionalR = 20;
    midnightLight.directionalG = 20;
    midnightLight.directionalB = 60;
    midnightLight.intensity = 0.1f;
    midnightLight.dirX = 0.0f;
    midnightLight.dirY = -1.0f;
    midnightLight.dirZ = 0.0f;
    periodLights_[DayPeriod::MIDNIGHT] = midnightLight;
    
    // Inicia com tempo atual
    currentTime_ = static_cast<uint64_t>(std::time(nullptr)) * 1000;
    
    // Calcula período /**
 * calculateDayPeriod
 * 
 * Implementa a funcionalidade calculateDayPeriod conforme especificação original.
 * @return Retorna inicial
 */
 inicial
    calculateDayPeriod();
    
    // Configura luz /**
 * updateLighting
 * 
 * Implementa a funcionalidade updateLighting conforme especificação original.
 * @return Retorna inicial
 */
 inicial
    updateLighting();
    
    std::cout << "[WYDAtmosphere] Sistema de atmosfera inicializado. Período atual: " 
              << static_cast<int>(currentDayPeriod_) << std::endl;
}

// Destructor
WYDAtmosphere::~WYDAtmosphere() {
    // Cleanup, se necessário
}

// Carrega configuração de arquivo
bool WYDAtmosphere::loadConfig(const std::string& filePath) {
    try {
        /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @return Retorna std::ifstream
 */

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "[WYDAtmosphere] Erro ao abrir arquivo de configuração: " << filePath << std::endl;
            return false;
        }
        
        std::string line;
        std::string section;
        
        while (std::getline(file, line)) {
            // Remove espaços em branco
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            
            // Pula linhas vazias e comentários
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // Verifica se é uma seção
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                section = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Processa entrada
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Processa configuraçã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "DayNight" Parâmetro "DayNight"
 * @return Retorna o
 */
o
                if(section == "DayNight"){
                    if (key == "DayDuration") {
                        dayDurationMs_ = std::stoul(value);
                    }
                } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "Weather" Parâmetro "Weather"
 * @return Retorna else
 */
 else if(section == "Weather"){
                    if (key == "DefaultWeather") {
                        currentWeather_ = static_cast<WeatherType>(std::stoi(value));
                    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "DefaultIntensity" Parâmetro "DefaultIntensity"
 * @return Retorna else
 */
 else if(key == "DefaultIntensity"){
                        weatherIntensity_ = std::stof(value);
                    }
                }
                // Mais configurações podem ser adicionadas aqui
            }
        }
        
        std::cout << "[WYDAtmosphere] Configuração carregada de " << filePath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[WYDAtmosphere] Erro ao carregar configuração: " << e.what() << std::endl;
        return false;
    }
}

// Atualização principal
void WYDAtmosphere::update(uint64_t timestamp) {
    // Atualiza tempo atual
    currentTime_ = timestamp;
    
    // Atualiza ciclo dia//**
 * updateDayNightCycle
 * 
 * Implementa a funcionalidade updateDayNightCycle conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @return Retorna noite
 */
noite
    updateDayNightCycle(timestamp);
    
    // Atualiza /**
 * updateWeather
 * 
 * Implementa a funcionalidade updateWeather conforme especificação original.
 * @param timestamp Parâmetro timestamp
 * @return Retorna clima
 */
 clima
    updateWeather(timestamp);
}

// Define clima atual
void WYDAtmosphere::setWeather(WeatherType weather, float intensity, uint32_t durationMs) {
    // Se já tem um clima programado, /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param hasScheduledWeather_ Parâmetro hasScheduledWeather_
 * @return Retorna cancela
 */
 cancela
    if(hasScheduledWeather_){
        std::cout << "[WYDAtmosphere] Cancelando transição de clima programada" << std::endl;
    }
    
    // Configura novo clima
    currentWeather_ = weather;
    weatherIntensity_ = std::clamp(intensity, 0.0f, 1.0f);
    weatherStartTime_ = currentTime_;
    weatherDurationMs_ = durationMs;
    
    // Reset de flags
    hasScheduledWeather_ = false;
    
    std::cout << "[WYDAtmosphere] Clima configurado para " 
              << static_cast<int>(weather) << " com intensidade " 
              << weatherIntensity_ << " por " << durationMs << "ms" << std::endl;
}

// Define duração do dia
void WYDAtmosphere::setDayDuration(uint32_t dayDurationMs) {
    dayDurationMs_ = dayDurationMs;
    
    std::cout << "[WYDAtmosphere] Duração do dia configurada para " 
              << dayDurationMs_ << "ms" << std::endl;
}

// Define tempo atual
void WYDAtmosphere::setCurrentTime(uint64_t time) {
    currentTime_ = time;
    
    // Atualiza ciclo dia//**
 * calculateDayPeriod
 * 
 * Implementa a funcionalidade calculateDayPeriod conforme especificação original.
 * @return Retorna noite
 */
noite
    calculateDayPeriod();
    updateLighting();
    
    std::cout << "[WYDAtmosphere] Tempo configurado para " << currentTime_ << std::endl;
}

// Adiciona efeito atmosférico
void WYDAtmosphere::addEffect(const AtmosphericEffect& effect) {
    // Remove efeito anterior com mesmo nome, se /**
 * removeEffect
 * 
 * Implementa a funcionalidade removeEffect conforme especificação original.
 * @param effect.name Parâmetro effect.name
 * @return Retorna existir
 */
 existir
    removeEffect(effect.name);
    
    // Adiciona novo efeito
    activeEffects_.push_back(effect);
    
    std::cout << "[WYDAtmosphere] Efeito '" << effect.name << "' adicionado com densidade " 
              << effect.density << std::endl;
}

// Remove efeito atmosférico
void WYDAtmosphere::removeEffect(const std::string& effectName) {
    auto it = std::find_if(activeEffects_.begin(), activeEffects_.end(), 
                          [&effectName](const AtmosphericEffect& effect) {
                              return effect.name == effectName;
                          });
    
    if (it != activeEffects_.end()) {
        activeEffects_.erase(it);
        std::cout << "[WYDAtmosphere] Efeito '" << effectName << "' removido" << std::endl;
    }
}

// Verifica se tem um efeito
bool WYDAtmosphere::hasEffect(const std::string& effectName) const {
    return std::find_if(activeEffects_.begin(), activeEffects_.end(), 
                       [&effectName](const AtmosphericEffect& effect) {
                           return effect.name == effectName;
                       }) != activeEffects_.end();
}

// Serializa para pacote de rede
std::string WYDAtmosphere::serializeToPacket() const {
    // Em uma implementação real, aqui seria criado um pacote binário
    // seguindo o protocolo específico do jogo.
    // Por agora, retornamos uma string JSON simplificada.
    
    std::ostringstream oss;
    oss << "{"
        << "\"weather\":" << static_cast<int>(currentWeather_) << ","
        << "\"intensity\":" << weatherIntensity_ << ","
        << "\"dayProgress\":" << dayProgress_ << ","
        << "\"period\":" << static_cast<int>(currentDayPeriod_) << ","
        << "\"light\":{"
        << "\"ambientR\":" << static_cast<int>(currentLight_.ambientR) << ","
        << "\"ambientG\":" << static_cast<int>(currentLight_.ambientG) << ","
        << "\"ambientB\":" << static_cast<int>(currentLight_.ambientB) << ","
        << "\"directionalR\":" << static_cast<int>(currentLight_.directionalR) << ","
        << "\"directionalG\":" << static_cast<int>(currentLight_.directionalG) << ","
        << "\"directionalB\":" << static_cast<int>(currentLight_.directionalB) << ","
        << "\"intensity\":" << currentLight_.intensity
        << "},"
        << "\"effects\":[";
    
    // Adiciona /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param activeEffects_.size( Parâmetro activeEffects_.size(
 * @return Retorna efeitos
 */
 efeitos
    for(size_t i = 0; i < activeEffects_.size(); i++) {
        const auto& effect = activeEffects_[i];
        oss << "{"
            << "\"name\":\"" << effect.name << "\","
            << "\"type\":\"" << effect.particleType << "\","
            << "\"density\":" << effect.density << ","
            << "\"speed\":" << effect.speed << ","
            << "\"r\":" << static_cast<int>(effect.r) << ","
            << "\"g\":" << static_cast<int>(effect.g) << ","
            << "\"b\":" << static_cast<int>(effect.b) << ","
            << "\"a\":" << static_cast<int>(effect.a)
            << "}";
        
        if (i < activeEffects_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "]}";
    
    return oss.str();
}

// Atualiza ciclo dia/noite
void WYDAtmosphere::updateDayNightCycle(uint64_t timestamp) {
    // Calcula progresso do dia (0.0 - 1.0)
    uint64_t timeOfDay = timestamp % dayDurationMs_;
    dayProgress_ = static_cast<float>(timeOfDay) / static_cast<float>(dayDurationMs_);
    
    // Calcula período do dia
    DayPeriod oldPeriod = currentDayPeriod_;
    calculateDayPeriod();
    
    // Se o período mudou, atualiza iluminaçã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param currentDayPeriod_ Parâmetro currentDayPeriod_
 * @return Retorna o
 */
o
    if(oldPeriod != currentDayPeriod_){
        updateLighting();
        
        std::cout << "[WYDAtmosphere] Período do dia mudou para " 
                  << static_cast<int>(currentDayPeriod_) << std::endl;
    } else {
        // Atualiza apenas a direção da luz com base no progresso do dia
        float sunAngle = dayProgress_ * 2.0f * 3.14159f; // 0 a 2π
        
        // Direção do sol (simplificada)
        currentLight_.dirX = std::sin(sunAngle);
        currentLight_.dirY = std::cos(sunAngle);
        currentLight_.dirZ = 0.4f; // Ligeira inclinação para cima
        
        // Normaliza o vetor
        float len = std::sqrt(currentLight_.dirX * currentLight_.dirX + 
                             currentLight_.dirY * currentLight_.dirY + 
                             currentLight_.dirZ * currentLight_.dirZ);
        
        if (len > 0.0001f) {
            currentLight_.dirX /= len;
            currentLight_.dirY /= len;
            currentLight_.dirZ /= len;
        }
    }
}

// Atualiza clima
void WYDAtmosphere::updateWeather(uint64_t timestamp) {
    // Verifica se o clima atual /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !hasScheduledWeather_ Parâmetro !hasScheduledWeather_
 * @return Retorna expirou
 */
 expirou
    if(weatherDurationMs_ > 0 && 
        timestamp - weatherStartTime_ >= weatherDurationMs_ &&
        !hasScheduledWeather_){
        
        // Agenda transição para clima padrão (claro)
        hasScheduledWeather_ = true;
        scheduledWeather_ = WeatherType::CLEAR;
        scheduledWeatherIntensity_ = 0.0f;
        weatherTransitionTime_ = timestamp;
        
        std::cout << "[WYDAtmosphere] Clima expirando, iniciando transição para clima padrão" << std::endl;
    }
    
    // Processa transição de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param hasScheduledWeather_ Parâmetro hasScheduledWeather_
 * @return Retorna clima
 */
 clima
    if(hasScheduledWeather_){
        // Duração da transição (5 segundos)
        const uint64_t transitionDuration = 5000;
        
        // Calcula progresso da transição
        uint64_t elapsed = timestamp - weatherTransitionTime_;
        float progress = std::min(1.0f, static_cast<float>(elapsed) / static_cast<float>(transitionDuration));
        
        // Interpola intensidade
        weatherIntensity_ = weatherIntensity_ * (1.0f - progress) + scheduledWeatherIntensity_ * progress;
        
        // Se a transição completou, /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1.0f Parâmetro 1.0f
 * @return Retorna finaliza
 */
 finaliza
        if(progress >= 1.0f){
            currentWeather_ = scheduledWeather_;
            weatherIntensity_ = scheduledWeatherIntensity_;
            hasScheduledWeather_ = false;
            
            std::cout << "[WYDAtmosphere] Transição de clima completa para " 
                      << static_cast<int>(currentWeather_) << " com intensidade " 
                      << weatherIntensity_ << std::endl;
        }
    }
}

// Calcula período do dia
void WYDAtmosphere::calculateDayPeriod() {
    // Tabela de períodos do dia (aproximada)
    // DAWN: 5:00 - 7:00 (0.21 - 0.29)
    // MORNING: 7:00 - 11:00 (0.29 - 0.46)
    // NOON: 11:00 - 13:00 (0.46 - 0.54)
    // AFTERNOON: 13:00 - 17:00 (0.54 - 0.71)
    // DUSK: 17:00 - 19:00 (0.71 - 0.79)
    // EVENING: 19:00 - 21:00 (0.79 - 0.88)
    // NIGHT: 21:00 - 23:00 (0.88 - 0.96)
    // MIDNIGHT: 23:00 - 5:00 (0.96 - 0.21)
    
    if (dayProgress_ >= 0.96f || dayProgress_ < 0.21f) {
        currentDayPeriod_ = DayPeriod::MIDNIGHT;
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0.29f Parâmetro 0.29f
 * @return Retorna else
 */
 else if(dayProgress_ >= 0.21f && dayProgress_ < 0.29f){
        currentDayPeriod_ = DayPeriod::DAWN;
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0.46f Parâmetro 0.46f
 * @return Retorna else
 */
 else if(dayProgress_ >= 0.29f && dayProgress_ < 0.46f){
        currentDayPeriod_ = DayPeriod::MORNING;
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0.54f Parâmetro 0.54f
 * @return Retorna else
 */
 else if(dayProgress_ >= 0.46f && dayProgress_ < 0.54f){
        currentDayPeriod_ = DayPeriod::NOON;
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0.71f Parâmetro 0.71f
 * @return Retorna else
 */
 else if(dayProgress_ >= 0.54f && dayProgress_ < 0.71f){
        currentDayPeriod_ = DayPeriod::AFTERNOON;
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0.79f Parâmetro 0.79f
 * @return Retorna else
 */
 else if(dayProgress_ >= 0.71f && dayProgress_ < 0.79f){
        currentDayPeriod_ = DayPeriod::DUSK;
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0.88f Parâmetro 0.88f
 * @return Retorna else
 */
 else if(dayProgress_ >= 0.79f && dayProgress_ < 0.88f){
        currentDayPeriod_ = DayPeriod::EVENING;
    } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0.96f Parâmetro 0.96f
 * @return Retorna else
 */
 else if(dayProgress_ >= 0.88f && dayProgress_ < 0.96f){
        currentDayPeriod_ = DayPeriod::NIGHT;
    }
}

// Atualiza iluminação com base no período do dia
void WYDAtmosphere::updateLighting() {
    // Obtém configuração de luz para o período atual
    auto it = periodLights_.find(currentDayPeriod_);
    if (it != periodLights_.end()) {
        currentLight_ = it->second;
    } else {
        // Configuração padrão se não encontrar
        currentLight_ = LightConfig();
    }
    
    // Verifica se precisa interpolar entre períodos
    DayPeriod nextPeriod;
    float blendFactor = 0.0f;
    
    // Calcula próximo período e fator de /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param currentDayPeriod_ Parâmetro currentDayPeriod_
 * @return Retorna blend
 */
 blend
    switch(currentDayPeriod_){
        case DayPeriod::MIDNIGHT:
            // Midnight (0.96 - 0.21) -> /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0.96f Parâmetro 0.96f
 * @return Retorna Dawn
 */
 Dawn
            if(dayProgress_ >= 0.96f){
                nextPeriod = DayPeriod::DAWN;
                blendFactor = (dayProgress_ - 0.96f) / 0.25f;
            } else {
                nextPeriod = DayPeriod::DAWN;
                blendFactor = (dayProgress_ + 0.04f) / 0.25f;
            }
            break;
            
        case DayPeriod::DAWN:
            // Dawn (0.21 - 0.29) -> Morning
            nextPeriod = DayPeriod::MORNING;
            blendFactor = (dayProgress_ - 0.21f) / 0.08f;
            break;
            
        case DayPeriod::MORNING:
            // Morning (0.29 - 0.46) -> Noon
            nextPeriod = DayPeriod::NOON;
            blendFactor = (dayProgress_ - 0.29f) / 0.17f;
            break;
            
        case DayPeriod::NOON:
            // Noon (0.46 - 0.54) -> Afternoon
            nextPeriod = DayPeriod::AFTERNOON;
            blendFactor = (dayProgress_ - 0.46f) / 0.08f;
            break;
            
        case DayPeriod::AFTERNOON:
            // Afternoon (0.54 - 0.71) -> Dusk
            nextPeriod = DayPeriod::DUSK;
            blendFactor = (dayProgress_ - 0.54f) / 0.17f;
            break;
            
        case DayPeriod::DUSK:
            // Dusk (0.71 - 0.79) -> Evening
            nextPeriod = DayPeriod::EVENING;
            blendFactor = (dayProgress_ - 0.71f) / 0.08f;
            break;
            
        case DayPeriod::EVENING:
            // Evening (0.79 - 0.88) -> Night
            nextPeriod = DayPeriod::NIGHT;
            blendFactor = (dayProgress_ - 0.79f) / 0.09f;
            break;
            
        case DayPeriod::NIGHT:
            // Night (0.88 - 0.96) -> Midnight
            nextPeriod = DayPeriod::MIDNIGHT;
            blendFactor = (dayProgress_ - 0.88f) / 0.08f;
            break;
    }
    
    // Interpola entre configurações de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1.0f Parâmetro 1.0f
 * @return Retorna luz
 */
 luz
    if(blendFactor > 0.0f && blendFactor < 1.0f){
        auto nextIt = periodLights_.find(nextPeriod);
        if (nextIt != periodLights_.end()) {
            interpolateLight(currentLight_, nextIt->second, blendFactor);
        }
    }
}

// Interpola entre duas configurações de luz
void WYDAtmosphere::interpolateLight(const LightConfig& from, const LightConfig& to, float factor) {

} // namespace wydbr

    // Garante que o fator está entre 0 e 1
    factor = std::clamp(factor, 0.0f, 1.0f);
    
    // Interpola componentes da luz
    currentLight_.ambientR = static_cast<uint8_t>(from.ambientR * (1.0f - factor) + to.ambientR * factor);
    currentLight_.ambientG = static_cast<uint8_t>(from.ambientG * (1.0f - factor) + to.ambientG * factor);
    currentLight_.ambientB = static_cast<uint8_t>(from.ambientB * (1.0f - factor) + to.ambientB * factor);
    
    currentLight_.directionalR = static_cast<uint8_t>(from.directionalR * (1.0f - factor) + to.directionalR * factor);
    currentLight_.directionalG = static_cast<uint8_t>(from.directionalG * (1.0f - factor) + to.directionalG * factor);
    currentLight_.directionalB = static_cast<uint8_t>(from.directionalB * (1.0f - factor) + to.directionalB * factor);
    
    currentLight_.intensity = from.intensity * (1.0f - factor) + to.intensity * factor;
    
    // Interpola direção (interpolação linear simples)
    currentLight_.dirX = from.dirX * (1.0f - factor) + to.dirX * factor;
    currentLight_.dirY = from.dirY * (1.0f - factor) + to.dirY * factor;
    currentLight_.dirZ = from.dirZ * (1.0f - factor) + to.dirZ * factor;
    
    // Normaliza o vetor de direção
    float len = std::sqrt(currentLight_.dirX * currentLight_.dirX + 
                         currentLight_.dirY * currentLight_.dirY + 
                         currentLight_.dirZ * currentLight_.dirZ);
    
    if (len > 0.0001f) {
        currentLight_.dirX /= len;
        currentLight_.dirY /= len;
        currentLight_.dirZ /= len;
    }
}