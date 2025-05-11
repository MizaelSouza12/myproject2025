const EventEmitter = require('events');

class SystemEventEmitter extends EventEmitter {
  constructor() {
    super();
    this.setMaxListeners(0); // Permite múltiplos listeners
  }

  // Eventos do sistema
  static EVENTS = {
    SYSTEM_INITIALIZED: 'system:initialized',
    SYSTEM_SHUTDOWN: 'system:shutdown',
    SYSTEM_ERROR: 'system:error',
    SYSTEM_RELOAD: 'system:reload',
    SYSTEM_STATE_CHANGE: 'system:state_change'
  };

  // Métodos auxiliares para emitir eventos com dados estruturados
  emitSystemInitialized(systemName) {
    this.emit(SystemEventEmitter.EVENTS.SYSTEM_INITIALIZED, {
      system: systemName,
      timestamp: new Date(),
      type: 'initialized'
    });
  }

  emitSystemShutdown(systemName) {
    this.emit(SystemEventEmitter.EVENTS.SYSTEM_SHUTDOWN, {
      system: systemName,
      timestamp: new Date(),
      type: 'shutdown'
    });
  }

  emitSystemError(systemName, error) {
    this.emit(SystemEventEmitter.EVENTS.SYSTEM_ERROR, {
      system: systemName,
      error,
      timestamp: new Date(),
      type: 'error'
    });
  }

  emitSystemReload(systemName) {
    this.emit(SystemEventEmitter.EVENTS.SYSTEM_RELOAD, {
      system: systemName,
      timestamp: new Date(),
      type: 'reload'
    });
  }

  emitSystemStateChange(systemName, oldState, newState) {
    this.emit(SystemEventEmitter.EVENTS.SYSTEM_STATE_CHANGE, {
      system: systemName,
      oldState,
      newState,
      timestamp: new Date(),
      type: 'state_change'
    });
  }
}

module.exports = SystemEventEmitter; 