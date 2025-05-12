/**
 * Sistema de Tempo do WYDBRASIL
 * 
 * Gerencia a passagem do tempo no mundo do jogo, incluindo
 * ciclos de dia e noite, calendário e eventos programados.
 */

const EventEmitter = require('events');

class TimeSystem extends EventEmitter {
    constructor() {
        super();
        
        // Inicialização das variáveis de controle de tempo
        this.worldTime = 0; // Horas do mundo (0-24)
        this.gameDate = new Date(1000, 0, 1); // Data do mundo (ano 1000)
        this.timeScale = 72; // 72x mais rápido que o tempo real (20 min = 1 dia)
        this.dayLength = 24; // Duração do dia em horas do mundo
        this.isRunning = false;
        this.timeUpdateInterval = null;
        this.eventQueue = []; // Fila de eventos programados
        
        this.dayParts = [
            { name: 'Madrugada', start: 0, end: 6 },
            { name: 'Manhã', start: 6, end: 12 },
            { name: 'Tarde', start: 12, end: 18 },
            { name: 'Noite', start: 18, end: 24 }
        ];
        
        this.seasons = [
            { name: 'Primavera', start: { month: 2, day: 21 }, end: { month: 5, day: 20 } },
            { name: 'Verão', start: { month: 5, day: 21 }, end: { month: 8, day: 22 } },
            { name: 'Outono', start: { month: 8, day: 23 }, end: { month: 11, day: 20 } },
            { name: 'Inverno', start: { month: 11, day: 21 }, end: { month: 2, day: 20 } }
        ];
        
        console.log('[TimeSystem] Inicializado');
    }
    
    /**
     * Inicia o sistema de tempo
     */
    start() {
        if (this.isRunning) return;
        
        this.isRunning = true;
        
        // Atualizar o tempo do mundo a cada segundo
        this.timeUpdateInterval = setInterval(() => {
            this._updateWorldTime();
        }, 1000);
        
        console.log('[TimeSystem] Sistema de tempo iniciado');
        console.log(`[TimeSystem] Data inicial: ${this.getFormattedDate()}, ${this.getFormattedTime()}`);
    }
    
    /**
     * Para o sistema de tempo
     */
    stop() {
        if (!this.isRunning) return;
        
        this.isRunning = false;
        
        if (this.timeUpdateInterval) {
            clearInterval(this.timeUpdateInterval);
            this.timeUpdateInterval = null;
        }
        
        console.log('[TimeSystem] Sistema de tempo parado');
    }
    
    /**
     * Obtém o horário atual do mundo em formato legível
     * @returns {string} Horário formatado (HH:MM)
     */
    getFormattedTime() {
        const hours = Math.floor(this.worldTime);
        const minutes = Math.floor((this.worldTime - hours) * 60);
        return `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}`;
    }
    
    /**
     * Obtém a data atual do mundo em formato legível
     * @returns {string} Data formatada (DD/MM/AAAA)
     */
    getFormattedDate() {
        return `${this.gameDate.getDate().toString().padStart(2, '0')}/${(this.gameDate.getMonth() + 1).toString().padStart(2, '0')}/${this.gameDate.getFullYear()}`;
    }
    
    /**
     * Obtém a estação atual do mundo
     * @returns {object} Informações da estação atual
     */
    getCurrentSeason() {
        const month = this.gameDate.getMonth();
        const day = this.gameDate.getDate();
        
        for (const season of this.seasons) {
            const startMonth = season.start.month;
            const startDay = season.start.day;
            const endMonth = season.end.month;
            const endDay = season.end.day;
            
            if (
                (month > startMonth || (month === startMonth && day >= startDay)) &&
                (month < endMonth || (month === endMonth && day <= endDay))
            ) {
                return { name: season.name, ...season };
            }
        }
        
        return this.seasons[0]; // Retornar primavera como padrão
    }
    
    /**
     * Obtém a parte do dia atual (manhã, tarde, noite, etc.)
     * @returns {object} Informações da parte do dia atual
     */
    getDayPart() {
        for (const dayPart of this.dayParts) {
            if (this.worldTime >= dayPart.start && this.worldTime < dayPart.end) {
                return { name: dayPart.name, ...dayPart };
            }
        }
        
        return this.dayParts[0]; // Retornar madrugada como padrão
    }
    
    /**
     * Obtém a fase atual da lua
     * @returns {string} Nome da fase da lua
     */
    getMoonPhase() {
        // Ciclo lunar de 28 dias
        const day = this.gameDate.getDate();
        const month = this.gameDate.getMonth();
        const year = this.gameDate.getFullYear();
        
        // Calcular dias desde o início do ano
        const daysInMonth = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
        let dayOfYear = day;
        for (let i = 0; i < month; i++) {
            dayOfYear += daysInMonth[i];
        }
        
        // Adicionar dia extra se for ano bissexto e após fevereiro
        if ((year % 4 === 0) && (month > 1)) {
            dayOfYear += 1;
        }
        
        // Calcular fase baseada em ciclo de 28 dias
        const moonAge = dayOfYear % 28;
        
        if (moonAge < 2) return 'Nova';
        if (moonAge < 7) return 'Crescente';
        if (moonAge < 9) return 'Quarto Crescente';
        if (moonAge < 14) return 'Gibosa Crescente';
        if (moonAge < 16) return 'Cheia';
        if (moonAge < 21) return 'Gibosa Minguante';
        if (moonAge < 23) return 'Quarto Minguante';
        return 'Minguante';
    }
    
    /**
     * Define o fator de escala do tempo
     * @param {number} scale Fator de escala (1 = tempo real, maior = mais rápido)
     */
    setTimeScale(scale) {
        this.timeScale = Math.max(1, scale);
        console.log(`[TimeSystem] Escala de tempo definida para ${this.timeScale}x`);
        this.emit('time_scale_changed', { scale: this.timeScale });
    }
    
    /**
     * Define a data e hora do mundo
     * @param {number} year Ano
     * @param {number} month Mês (0-11)
     * @param {number} day Dia
     * @param {number} hour Hora
     * @param {number} minute Minuto
     */
    setWorldDateTime(year, month, day, hour, minute) {
        const oldDate = new Date(this.gameDate);
        const oldTime = this.worldTime;
        
        this.gameDate = new Date(year, month, day);
        this.worldTime = hour + (minute / 60);
        
        console.log(`[TimeSystem] Data e hora do mundo definidas para ${this.getFormattedDate()} ${this.getFormattedTime()}`);
        
        this.emit('time_changed', {
            oldDate,
            newDate: new Date(this.gameDate),
            oldTime,
            newTime: this.worldTime
        });
    }
    
    /**
     * Agenda um evento para acontecer em uma data/hora específica
     * @param {function} callback Função a ser chamada quando o evento ocorrer
     * @param {object} dateTime Objeto com data e hora do evento
     * @param {object} data Dados adicionais para o evento
     * @returns {string} ID do evento
     */
    scheduleEvent(callback, dateTime, data = {}) {
        const eventId = `time-event-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`;
        
        // Converter para data do mundo
        const eventDate = new Date(
            dateTime.year || this.gameDate.getFullYear(),
            dateTime.month !== undefined ? dateTime.month : this.gameDate.getMonth(),
            dateTime.day || this.gameDate.getDate()
        );
        
        const eventTime = dateTime.hour + (dateTime.minute / 60) || this.worldTime;
        
        this.eventQueue.push({
            id: eventId,
            date: eventDate,
            time: eventTime,
            callback,
            data,
            executed: false
        });
        
        // Ordenar fila de eventos por data/hora
        this.eventQueue.sort((a, b) => {
            const dateA = a.date.getTime();
            const dateB = b.date.getTime();
            
            if (dateA !== dateB) return dateA - dateB;
            return a.time - b.time;
        });
        
        console.log(`[TimeSystem] Evento agendado para ${eventDate.toLocaleDateString()} ${Math.floor(eventTime)}:${Math.floor((eventTime % 1) * 60).toString().padStart(2, '0')}`);
        
        return eventId;
    }
    
    /**
     * Cancela um evento agendado
     * @param {string} eventId ID do evento
     * @returns {boolean} Sucesso da operação
     */
    cancelEvent(eventId) {
        const index = this.eventQueue.findIndex(event => event.id === eventId);
        
        if (index === -1) {
            console.error(`[TimeSystem] Evento ${eventId} não encontrado para cancelar`);
            return false;
        }
        
        this.eventQueue.splice(index, 1);
        console.log(`[TimeSystem] Evento ${eventId} cancelado`);
        
        return true;
    }
    
    /**
     * Avança o tempo do mundo por um número específico de minutos
     * @param {number} minutes Minutos a avançar
     */
    advanceTime(minutes) {
        const hoursToAdvance = minutes / 60;
        const oldTime = this.worldTime;
        const oldDate = new Date(this.gameDate);
        
        // Calcular novo tempo
        this.worldTime += hoursToAdvance;
        
        // Verificar se passou para o próximo dia
        while (this.worldTime >= this.dayLength) {
            this.worldTime -= this.dayLength;
            
            // Avançar para o próximo dia
            this.gameDate.setDate(this.gameDate.getDate() + 1);
        }
        
        console.log(`[TimeSystem] Tempo avançado em ${minutes} minutos. Nova data/hora: ${this.getFormattedDate()} ${this.getFormattedTime()}`);
        
        // Verificar eventos que devem ser executados
        this._checkScheduledEvents();
        
        // Emitir eventos de mudança de tempo
        this._emitTimeChangeEvents(oldTime, oldDate);
    }
    
    /**
     * Atualiza o tempo do mundo
     * @private
     */
    _updateWorldTime() {
        const oldTime = this.worldTime;
        const oldDate = new Date(this.gameDate);
        
        // Calcular passagem de tempo baseada no tempo real
        const timePassage = (1 / 3600) * (this.timeScale / 10); // Horas que passam por atualização (a cada 100ms)
        this.worldTime += timePassage;
        
        // Verificar se passou para o próximo dia
        if (this.worldTime >= this.dayLength) {
            this.worldTime -= this.dayLength;
            
            // Avançar para o próximo dia
            this.gameDate.setDate(this.gameDate.getDate() + 1);
        }
        
        // Verificar eventos programados
        this._checkScheduledEvents();
        
        // Emitir eventos de mudança de tempo, se relevantes
        this._emitTimeChangeEvents(oldTime, oldDate);
    }
    
    /**
     * Verifica e executa eventos agendados
     * @private
     */
    _checkScheduledEvents() {
        const currentDate = this.gameDate;
        const currentTime = this.worldTime;
        
        // Verificar cada evento na fila
        for (let i = 0; i < this.eventQueue.length; i++) {
            const event = this.eventQueue[i];
            
            // Pular eventos já executados
            if (event.executed) continue;
            
            // Verificar se o evento deve ocorrer
            if (
                event.date.getFullYear() < currentDate.getFullYear() ||
                (
                    event.date.getFullYear() === currentDate.getFullYear() &&
                    event.date.getMonth() < currentDate.getMonth()
                ) ||
                (
                    event.date.getFullYear() === currentDate.getFullYear() &&
                    event.date.getMonth() === currentDate.getMonth() &&
                    event.date.getDate() < currentDate.getDate()
                ) ||
                (
                    event.date.getFullYear() === currentDate.getFullYear() &&
                    event.date.getMonth() === currentDate.getMonth() &&
                    event.date.getDate() === currentDate.getDate() &&
                    event.time <= currentTime
                )
            ) {
                // Executar evento
                try {
                    event.callback(event.data);
                    event.executed = true;
                    console.log(`[TimeSystem] Evento ${event.id} executado`);
                } catch (error) {
                    console.error(`[TimeSystem] Erro ao executar evento ${event.id}:`, error);
                }
            }
        }
        
        // Remover eventos executados da fila
        this.eventQueue = this.eventQueue.filter(event => !event.executed);
    }
    
    /**
     * Emite eventos relacionados a mudanças significativas de tempo
     * @param {number} oldTime Tempo anterior
     * @param {Date} oldDate Data anterior
     * @private
     */
    _emitTimeChangeEvents(oldTime, oldDate) {
        const oldHour = Math.floor(oldTime);
        const newHour = Math.floor(this.worldTime);
        
        // Evento de mudança de hora
        if (oldHour !== newHour) {
            this.emit('hour_changed', {
                hour: newHour,
                formattedTime: this.getFormattedTime()
            });
            
            // Verificar mudança de parte do dia
            const oldDayPart = this._getDayPartByHour(oldHour);
            const newDayPart = this._getDayPartByHour(newHour);
            
            if (oldDayPart !== newDayPart) {
                this.emit('day_part_changed', {
                    from: oldDayPart,
                    to: newDayPart
                });
            }
            
            // Verificar nascer/pôr do sol
            if (oldHour === 5 && newHour === 6) {
                this.emit('sunrise');
            } else if (oldHour === 17 && newHour === 18) {
                this.emit('sunset');
            }
        }
        
        // Evento de mudança de dia
        if (oldDate.getDate() !== this.gameDate.getDate() ||
            oldDate.getMonth() !== this.gameDate.getMonth() ||
            oldDate.getFullYear() !== this.gameDate.getFullYear()) {
            
            this.emit('day_changed', {
                date: new Date(this.gameDate),
                formattedDate: this.getFormattedDate()
            });
            
            // Verificar mudança de mês
            if (oldDate.getMonth() !== this.gameDate.getMonth() ||
                oldDate.getFullYear() !== this.gameDate.getFullYear()) {
                
                this.emit('month_changed', {
                    month: this.gameDate.getMonth(),
                    monthName: this._getMonthName(this.gameDate.getMonth())
                });
                
                // Verificar mudança de estação
                const oldSeason = this._getSeasonByDate(oldDate);
                const newSeason = this._getSeasonByDate(this.gameDate);
                
                if (oldSeason !== newSeason) {
                    this.emit('season_changed', {
                        from: oldSeason,
                        to: newSeason
                    });
                }
            }
            
            // Verificar mudança de ano
            if (oldDate.getFullYear() !== this.gameDate.getFullYear()) {
                this.emit('year_changed', {
                    year: this.gameDate.getFullYear()
                });
            }
        }
    }
    
    /**
     * Obtém o nome da parte do dia para uma hora específica
     * @param {number} hour Hora
     * @returns {string} Nome da parte do dia
     * @private
     */
    _getDayPartByHour(hour) {
        for (const dayPart of this.dayParts) {
            if (hour >= dayPart.start && hour < dayPart.end) {
                return dayPart.name;
            }
        }
        return 'Madrugada';
    }
    
    /**
     * Obtém o nome da estação para uma data específica
     * @param {Date} date Data
     * @returns {string} Nome da estação
     * @private
     */
    _getSeasonByDate(date) {
        const month = date.getMonth();
        const day = date.getDate();
        
        for (const season of this.seasons) {
            const startMonth = season.start.month;
            const startDay = season.start.day;
            const endMonth = season.end.month;
            const endDay = season.end.day;
            
            if (
                (month > startMonth || (month === startMonth && day >= startDay)) &&
                (month < endMonth || (month === endMonth && day <= endDay))
            ) {
                return season.name;
            }
        }
        
        return 'Primavera';
    }
    
    /**
     * Obtém o nome do mês pelo seu número
     * @param {number} month Número do mês (0-11)
     * @returns {string} Nome do mês
     * @private
     */
    _getMonthName(month) {
        const monthNames = [
            'Janeiro', 'Fevereiro', 'Março', 'Abril', 'Maio', 'Junho',
            'Julho', 'Agosto', 'Setembro', 'Outubro', 'Novembro', 'Dezembro'
        ];
        
        return monthNames[month];
    }
}

module.exports = TimeSystem;