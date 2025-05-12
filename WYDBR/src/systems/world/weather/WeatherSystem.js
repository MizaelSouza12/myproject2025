/**
 * Sistema de Clima do WYDBRASIL
 * 
 * Controla e simula mudanças climáticas dinâmicas no mundo do jogo,
 * afetando jogabilidade, eventos e aparência visual.
 */

const EventEmitter = require('events');

class WeatherSystem extends EventEmitter {
    constructor() {
        super();
        this.regions = new Map();
        this.transitioningRegions = new Set();
        this.weatherUpdateInterval = null;
        this.naturalCycleInterval = null;
        this.isRunning = false;
        
        // Tipos de clima disponíveis
        this.weatherTypes = {
            'clear': {
                name: 'Céu Limpo',
                effects: {
                    visibility: 1.0,
                    movementSpeed: 1.0,
                    combatModifier: 1.0,
                    regeneration: 1.1
                },
                visualEffects: {
                    skyColor: '#87CEEB',
                    lightIntensity: 1.0,
                    fogDensity: 0.0
                }
            },
            'cloudy': {
                name: 'Nublado',
                effects: {
                    visibility: 0.9,
                    movementSpeed: 1.0,
                    combatModifier: 1.0,
                    regeneration: 1.0
                },
                visualEffects: {
                    skyColor: '#708090',
                    lightIntensity: 0.8,
                    fogDensity: 0.1
                }
            },
            'rain': {
                name: 'Chuva',
                effects: {
                    visibility: 0.7,
                    movementSpeed: 0.9,
                    combatModifier: 0.95,
                    regeneration: 0.9
                },
                visualEffects: {
                    skyColor: '#4682B4',
                    lightIntensity: 0.6,
                    fogDensity: 0.3,
                    particles: 'rain'
                }
            },
            'heavy_rain': {
                name: 'Tempestade',
                effects: {
                    visibility: 0.5,
                    movementSpeed: 0.8,
                    combatModifier: 0.9,
                    regeneration: 0.8
                },
                visualEffects: {
                    skyColor: '#2F4F4F',
                    lightIntensity: 0.4,
                    fogDensity: 0.5,
                    particles: 'heavy_rain'
                }
            },
            'storm': {
                name: 'Tempestade Elétrica',
                effects: {
                    visibility: 0.4,
                    movementSpeed: 0.7,
                    combatModifier: 0.85,
                    regeneration: 0.7
                },
                visualEffects: {
                    skyColor: '#191970',
                    lightIntensity: 0.3,
                    fogDensity: 0.6,
                    particles: 'rain',
                    lightningFrequency: 0.2
                }
            },
            'snow': {
                name: 'Neve',
                effects: {
                    visibility: 0.6,
                    movementSpeed: 0.7,
                    combatModifier: 0.9,
                    regeneration: 0.8
                },
                visualEffects: {
                    skyColor: '#B0C4DE',
                    lightIntensity: 0.9,
                    fogDensity: 0.4,
                    particles: 'snow'
                }
            },
            'blizzard': {
                name: 'Nevasca',
                effects: {
                    visibility: 0.3,
                    movementSpeed: 0.5,
                    combatModifier: 0.8,
                    regeneration: 0.6
                },
                visualEffects: {
                    skyColor: '#F0F8FF',
                    lightIntensity: 0.5,
                    fogDensity: 0.8,
                    particles: 'heavy_snow'
                }
            },
            'fog': {
                name: 'Névoa',
                effects: {
                    visibility: 0.4,
                    movementSpeed: 0.9,
                    combatModifier: 0.9,
                    regeneration: 0.9
                },
                visualEffects: {
                    skyColor: '#D3D3D3',
                    lightIntensity: 0.7,
                    fogDensity: 0.7
                }
            },
            'sandstorm': {
                name: 'Tempestade de Areia',
                effects: {
                    visibility: 0.2,
                    movementSpeed: 0.6,
                    combatModifier: 0.8,
                    regeneration: 0.5
                },
                visualEffects: {
                    skyColor: '#DAA520',
                    lightIntensity: 0.6,
                    fogDensity: 0.8,
                    fogColor: '#B8860B',
                    particles: 'sand'
                }
            },
            'volcanic': {
                name: 'Erupção Vulcânica',
                effects: {
                    visibility: 0.3,
                    movementSpeed: 0.7,
                    combatModifier: 0.8,
                    regeneration: 0.3,
                    damageOverTime: 5
                },
                visualEffects: {
                    skyColor: '#8B0000',
                    lightIntensity: 0.4,
                    fogDensity: 0.7,
                    fogColor: '#CD5C5C',
                    particles: 'ash'
                }
            }
        };
        
        console.log('[WeatherSystem] Inicializado');
    }
    
    /**
     * Inicia o sistema de clima
     */
    start() {
        if (this.isRunning) return;
        
        this.isRunning = true;
        
        // Atualizar efeitos climáticos a cada 5 segundos
        this.weatherUpdateInterval = setInterval(() => {
            this._updateWeatherEffects();
        }, 5000);
        
        // Mudanças climáticas naturais a cada 30 minutos
        this.naturalCycleInterval = setInterval(() => {
            this._applyNaturalWeatherChanges();
        }, 30 * 60 * 1000);
        
        console.log('[WeatherSystem] Sistema de clima iniciado');
    }
    
    /**
     * Para o sistema de clima
     */
    stop() {
        if (!this.isRunning) return;
        
        this.isRunning = false;
        
        if (this.weatherUpdateInterval) {
            clearInterval(this.weatherUpdateInterval);
            this.weatherUpdateInterval = null;
        }
        
        if (this.naturalCycleInterval) {
            clearInterval(this.naturalCycleInterval);
            this.naturalCycleInterval = null;
        }
        
        console.log('[WeatherSystem] Sistema de clima parado');
    }
    
    /**
     * Define o clima para uma região específica
     * @param {string} region Nome da região
     * @param {string} weatherType Tipo de clima
     * @param {number} duration Duração em minutos (0 para permanente)
     * @param {boolean} immediate Se verdadeiro, aplica mudança sem transição
     * @returns {boolean} Sucesso da operação
     */
    setWeather(region, weatherType, duration = 0, immediate = false) {
        // Validar tipo de clima
        if (!this.weatherTypes[weatherType]) {
            console.error(`[WeatherSystem] Tipo de clima desconhecido: ${weatherType}`);
            return false;
        }
        
        // Obter informações da região (ou criar se não existir)
        if (!this.regions.has(region)) {
            this.regions.set(region, {
                name: region,
                weather: 'clear',
                effects: { ...this.weatherTypes['clear'].effects },
                visualEffects: { ...this.weatherTypes['clear'].visualEffects },
                transitionProgress: 0,
                expiresAt: null
            });
        }
        
        const regionInfo = this.regions.get(region);
        const oldWeather = regionInfo.weather;
        
        // Aplicar novo clima
        regionInfo.weather = weatherType;
        regionInfo.expiresAt = duration > 0 ? new Date(Date.now() + duration * 60 * 1000) : null;
        
        if (immediate) {
            // Aplicar efeitos imediatamente
            regionInfo.effects = { ...this.weatherTypes[weatherType].effects };
            regionInfo.visualEffects = { ...this.weatherTypes[weatherType].visualEffects };
            regionInfo.transitionProgress = 1;
            this.transitioningRegions.delete(region);
        } else {
            // Iniciar transição gradual
            regionInfo.transitionProgress = 0;
            regionInfo.targetEffects = { ...this.weatherTypes[weatherType].effects };
            regionInfo.targetVisualEffects = { ...this.weatherTypes[weatherType].visualEffects };
            regionInfo.sourceEffects = { ...regionInfo.effects };
            regionInfo.sourceVisualEffects = { ...regionInfo.visualEffects };
            this.transitioningRegions.add(region);
        }
        
        console.log(`[WeatherSystem] Clima da região ${region} alterado de ${oldWeather} para ${weatherType} (Duração: ${duration > 0 ? duration + ' minutos' : 'permanente'})`);
        
        // Emitir evento de mudança climática
        this.emit('weather_changed', {
            region,
            oldWeather,
            newWeather: weatherType,
            duration,
            immediate
        });
        
        return true;
    }
    
    /**
     * Obtém as informações climáticas de uma região
     * @param {string} region Nome da região
     * @returns {object|null} Informações do clima ou null se região não existir
     */
    getWeather(region) {
        return this.regions.has(region) ? { ...this.regions.get(region) } : null;
    }
    
    /**
     * Obtém todas as regiões e seus climas atuais
     * @returns {Array} Lista de regiões e climas
     */
    getAllRegionWeather() {
        return Array.from(this.regions.entries()).map(([name, info]) => ({
            region: name,
            weather: info.weather,
            effects: { ...info.effects },
            visualEffects: { ...info.visualEffects },
            expiresAt: info.expiresAt
        }));
    }
    
    /**
     * Atualiza os efeitos climáticos para regiões em transição
     * @private
     */
    _updateWeatherEffects() {
        const now = new Date();
        const transitionStep = 0.05; // 5% de progresso por atualização
        
        // Verificar regiões com clima expirado
        for (const [regionName, regionInfo] of this.regions.entries()) {
            if (regionInfo.expiresAt && regionInfo.expiresAt <= now) {
                // Clima expirou, retornar para "clear"
                this.setWeather(regionName, 'clear', 0, false);
            }
        }
        
        // Processar transições climáticas
        for (const region of this.transitioningRegions) {
            if (!this.regions.has(region)) {
                this.transitioningRegions.delete(region);
                continue;
            }
            
            const regionInfo = this.regions.get(region);
            regionInfo.transitionProgress += transitionStep;
            
            if (regionInfo.transitionProgress >= 1) {
                // Transição completa
                regionInfo.transitionProgress = 1;
                regionInfo.effects = { ...regionInfo.targetEffects };
                regionInfo.visualEffects = { ...regionInfo.targetVisualEffects };
                this.transitioningRegions.delete(region);
                
                this.emit('weather_transition_complete', {
                    region,
                    weather: regionInfo.weather
                });
            } else {
                // Interpolar efeitos durante a transição
                for (const key in regionInfo.targetEffects) {
                    regionInfo.effects[key] = this._interpolate(
                        regionInfo.sourceEffects[key] || 0,
                        regionInfo.targetEffects[key] || 0,
                        regionInfo.transitionProgress
                    );
                }
                
                for (const key in regionInfo.targetVisualEffects) {
                    if (key === 'skyColor' || key === 'fogColor') {
                        regionInfo.visualEffects[key] = this._interpolateColor(
                            regionInfo.sourceVisualEffects[key] || '#FFFFFF',
                            regionInfo.targetVisualEffects[key] || '#FFFFFF',
                            regionInfo.transitionProgress
                        );
                    } else {
                        regionInfo.visualEffects[key] = this._interpolate(
                            regionInfo.sourceVisualEffects[key] || 0,
                            regionInfo.targetVisualEffects[key] || 0,
                            regionInfo.transitionProgress
                        );
                    }
                }
                
                this.emit('weather_transition_update', {
                    region,
                    weather: regionInfo.weather,
                    progress: regionInfo.transitionProgress
                });
            }
        }
    }
    
    /**
     * Aplica mudanças climáticas naturais baseadas em probabilidade
     * @private
     */
    _applyNaturalWeatherChanges() {
        // Lista de possíveis progressões climáticas naturais
        const naturalProgressions = {
            'clear': [
                { next: 'cloudy', probability: 0.4 },
                { next: 'fog', probability: 0.1 }
            ],
            'cloudy': [
                { next: 'clear', probability: 0.3 },
                { next: 'rain', probability: 0.4 },
                { next: 'fog', probability: 0.2 }
            ],
            'rain': [
                { next: 'cloudy', probability: 0.3 },
                { next: 'heavy_rain', probability: 0.3 },
                { next: 'clear', probability: 0.1 }
            ],
            'heavy_rain': [
                { next: 'rain', probability: 0.5 },
                { next: 'storm', probability: 0.3 }
            ],
            'storm': [
                { next: 'heavy_rain', probability: 0.6 },
                { next: 'rain', probability: 0.2 }
            ],
            'fog': [
                { next: 'clear', probability: 0.4 },
                { next: 'cloudy', probability: 0.4 }
            ],
            'snow': [
                { next: 'cloudy', probability: 0.4 },
                { next: 'blizzard', probability: 0.2 },
                { next: 'clear', probability: 0.2 }
            ],
            'blizzard': [
                { next: 'snow', probability: 0.7 }
            ],
            'sandstorm': [
                { next: 'clear', probability: 0.6 }
            ],
            'volcanic': [
                { next: 'clear', probability: 0.3 },
                { next: 'fog', probability: 0.3 }
            ]
        };
        
        // Aplicar mudanças a cada região
        for (const [regionName, regionInfo] of this.regions.entries()) {
            // Pular regiões com clima definido por eventos
            if (regionInfo.expiresAt !== null) continue;
            
            const currentWeather = regionInfo.weather;
            
            if (!naturalProgressions[currentWeather]) continue;
            
            // Calcular se o clima vai mudar
            const progressOptions = naturalProgressions[currentWeather];
            const random = Math.random();
            let cumulativeProbability = 0;
            let weatherChanged = false;
            
            for (const option of progressOptions) {
                cumulativeProbability += option.probability;
                
                if (random <= cumulativeProbability) {
                    // Mudar para o próximo clima na progressão natural
                    this.setWeather(regionName, option.next, Math.floor(Math.random() * 60) + 30, false);
                    weatherChanged = true;
                    break;
                }
            }
            
            // Registrar evento de ciclo natural
            if (weatherChanged) {
                console.log(`[WeatherSystem] Mudança climática natural em ${regionName}: ${currentWeather} -> ${regionInfo.weather}`);
            }
        }
    }
    
    /**
     * Interpola linearmente entre dois valores
     * @param {number} start Valor inicial
     * @param {number} end Valor final
     * @param {number} progress Progresso (0-1)
     * @returns {number} Valor interpolado
     * @private
     */
    _interpolate(start, end, progress) {
        return start + (end - start) * progress;
    }
    
    /**
     * Interpola entre duas cores em formato hexadecimal
     * @param {string} startColor Cor inicial (#RRGGBB)
     * @param {string} endColor Cor final (#RRGGBB)
     * @param {number} progress Progresso (0-1)
     * @returns {string} Cor interpolada (#RRGGBB)
     * @private
     */
    _interpolateColor(startColor, endColor, progress) {
        // Converter cores hexadecimais para RGB
        const startRGB = this._hexToRgb(startColor);
        const endRGB = this._hexToRgb(endColor);
        
        // Interpolar cada componente
        const r = Math.round(this._interpolate(startRGB.r, endRGB.r, progress));
        const g = Math.round(this._interpolate(startRGB.g, endRGB.g, progress));
        const b = Math.round(this._interpolate(startRGB.b, endRGB.b, progress));
        
        // Converter de volta para hexadecimal
        return `#${r.toString(16).padStart(2, '0')}${g.toString(16).padStart(2, '0')}${b.toString(16).padStart(2, '0')}`;
    }
    
    /**
     * Converte cor hexadecimal para componentes RGB
     * @param {string} hex Cor em formato hexadecimal (#RRGGBB)
     * @returns {object} Objeto com componentes r, g, b
     * @private
     */
    _hexToRgb(hex) {
        // Remover # se presente
        hex = hex.replace('#', '');
        
        // Converter para decimal
        const r = parseInt(hex.substring(0, 2), 16);
        const g = parseInt(hex.substring(2, 4), 16);
        const b = parseInt(hex.substring(4, 6), 16);
        
        return { r, g, b };
    }
}

module.exports = WeatherSystem;