/**
 * Sistema de Eventos de Mundo WYDBRASIL
 * 
 * Sistema para gerenciar eventos dinâmicos no mundo, 
 * incluindo invasões, eventos sazonais e mudanças de clima.
 */

const WorldEventManager = require('./events/WorldEventManager');
const WeatherSystem = require('./weather/WeatherSystem');
const TimeSystem = require('./time/TimeSystem');
const SpawnManager = require('./spawning/SpawnManager');

class WorldSystem {
    constructor() {
        this.events = new WorldEventManager();
        this.weather = new WeatherSystem();
        this.time = new TimeSystem();
        this.spawning = new SpawnManager();
        console.log('Sistema de Mundo WYDBRASIL inicializado');
    }
    
    /**
     * Inicia o sistema de mundo e eventos
     */
    start() {
        this.time.start();
        this.weather.start();
        this.events.start();
        this.spawning.start();
        console.log('Sistema de Mundo iniciado com sucesso');
    }
    
    /**
     * Agenda um novo evento para acontecer no mundo
     * @param {string} eventType Tipo de evento
     * @param {object} config Configuração do evento
     * @param {Date} scheduledTime Horário agendado (opcional, se null acontece imediatamente)
     * @returns {string} ID do evento agendado
     */
    scheduleEvent(eventType, config, scheduledTime = null) {
        return this.events.scheduleEvent(eventType, config, scheduledTime);
    }
    
    /**
     * Cancela um evento agendado
     * @param {string} eventId ID do evento
     * @returns {boolean} Sucesso da operação
     */
    cancelEvent(eventId) {
        return this.events.cancelEvent(eventId);
    }
    
    /**
     * Altera o clima em uma região do mundo
     * @param {string} region Região afetada
     * @param {string} weatherType Tipo de clima
     * @param {number} duration Duração em minutos
     * @returns {boolean} Sucesso da operação
     */
    changeWeather(region, weatherType, duration) {
        return this.weather.setWeather(region, weatherType, duration);
    }
    
    /**
     * Avança o tempo do mundo
     * @param {number} minutes Minutos para avançar
     */
    advanceTime(minutes) {
        this.time.advanceTime(minutes);
    }
    
    /**
     * Adiciona um novo ponto de spawn para criaturas
     * @param {string} region Região do mundo
     * @param {object} spawnConfig Configuração do spawn
     * @returns {string} ID do ponto de spawn
     */
    addSpawnPoint(region, spawnConfig) {
        return this.spawning.addSpawnPoint(region, spawnConfig);
    }
}

module.exports = WorldSystem;
