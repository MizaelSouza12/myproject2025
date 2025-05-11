const fs = require('fs').promises;
const path = require('path');
const Logger = require('./utils/logger');
const SystemEventEmitter = require('./utils/event_emitter');

class SystemOrchestrator {
  constructor() {
    this.logger = new Logger();
    this.events = new SystemEventEmitter();
    this.systems = new Map();
    this.systemStates = new Map();
    this.config = null;
    this.hotReloadInterval = null;
  }

  static STATES = {
    INACTIVE: 'inactive',
    LOADING: 'loading',
    ACTIVE: 'active',
    ERROR: 'error',
    RELOADING: 'reloading'
  };

  async loadConfig() {
    try {
      const configPath = path.join(__dirname, 'config', 'systems.json');
      const configData = await fs.readFile(configPath, 'utf8');
      this.config = JSON.parse(configData);
      this.logger.info('Configuração carregada com sucesso');
    } catch (error) {
      this.logger.error(`Erro ao carregar configuração: ${error.message}`);
      throw error;
    }
  }

  async loadSystems() {
    const systemsDir = path.join(__dirname, 'systems');
    const files = await fs.readdir(systemsDir);
    
    for (const file of files) {
      if (file.endsWith('.js')) {
        const systemName = path.basename(file, '.js');
        const systemConfig = this.config.systems[systemName];
        
        if (systemConfig && systemConfig.enabled) {
          const system = require(path.join(systemsDir, file));
          this.systems.set(systemName, {
            instance: system,
            config: systemConfig.config,
            state: SystemOrchestrator.STATES.INACTIVE
          });
          this.logger.debug(`Sistema ${systemName} carregado`);
        }
      }
    }
  }

  async initializeSystem(systemName) {
    const system = this.systems.get(systemName);
    if (!system) {
      throw new Error(`Sistema ${systemName} não encontrado`);
    }

    try {
      this.logger.info(`Inicializando sistema ${systemName}`);
      this.events.emitSystemStateChange(systemName, system.state, SystemOrchestrator.STATES.LOADING);
      system.state = SystemOrchestrator.STATES.LOADING;

      await system.instance.initialize(system.config);
      
      system.state = SystemOrchestrator.STATES.ACTIVE;
      this.events.emitSystemInitialized(systemName);
      this.events.emitSystemStateChange(systemName, SystemOrchestrator.STATES.LOADING, SystemOrchestrator.STATES.ACTIVE);
      
      return true;
    } catch (error) {
      system.state = SystemOrchestrator.STATES.ERROR;
      this.events.emitSystemError(systemName, error);
      this.events.emitSystemStateChange(systemName, SystemOrchestrator.STATES.LOADING, SystemOrchestrator.STATES.ERROR);
      this.logger.error(`Erro ao inicializar sistema ${systemName}: ${error.message}`);
      return false;
    }
  }

  async initializeAll() {
    await this.loadConfig();
    await this.loadSystems();

    const orderedSystems = Array.from(this.systems.entries())
      .sort((a, b) => this.config.systems[a[0]].priority - this.config.systems[b[0]].priority);

    for (const [systemName, system] of orderedSystems) {
      if (system.state === SystemOrchestrator.STATES.INACTIVE) {
        const success = await this.initializeSystem(systemName);
        if (!success) {
          await this.shutdownAll();
          throw new Error(`Falha ao inicializar sistema ${systemName}`);
        }
      }
    }

    if (this.config.hotReload.enabled) {
      this.startHotReload();
    }
  }

  async shutdownSystem(systemName) {
    const system = this.systems.get(systemName);
    if (!system) return;

    try {
      this.logger.info(`Desligando sistema ${systemName}`);
      await system.instance.shutdown();
      system.state = SystemOrchestrator.STATES.INACTIVE;
      this.events.emitSystemShutdown(systemName);
      this.events.emitSystemStateChange(systemName, SystemOrchestrator.STATES.ACTIVE, SystemOrchestrator.STATES.INACTIVE);
    } catch (error) {
      this.logger.error(`Erro ao desligar sistema ${systemName}: ${error.message}`);
      this.events.emitSystemError(systemName, error);
    }
  }

  async shutdownAll() {
    if (this.hotReloadInterval) {
      clearInterval(this.hotReloadInterval);
    }

    const orderedSystems = Array.from(this.systems.entries())
      .sort((a, b) => this.config.systems[b[0]].priority - this.config.systems[a[0]].priority);

    for (const [systemName] of orderedSystems) {
      await this.shutdownSystem(systemName);
    }
  }

  async reloadSystem(systemName) {
    const system = this.systems.get(systemName);
    if (!system) return;

    try {
      this.logger.info(`Recarregando sistema ${systemName}`);
      this.events.emitSystemStateChange(systemName, system.state, SystemOrchestrator.STATES.RELOADING);
      
      await this.shutdownSystem(systemName);
      delete require.cache[require.resolve(`./systems/${systemName}`)];
      
      const newSystem = require(`./systems/${systemName}`);
      this.systems.set(systemName, {
        ...system,
        instance: newSystem
      });

      await this.initializeSystem(systemName);
      this.events.emitSystemReload(systemName);
    } catch (error) {
      this.logger.error(`Erro ao recarregar sistema ${systemName}: ${error.message}`);
      this.events.emitSystemError(systemName, error);
    }
  }

  startHotReload() {
    this.hotReloadInterval = setInterval(async () => {
      const systemsDir = path.join(__dirname, 'systems');
      const files = await fs.readdir(systemsDir);
      
      for (const file of files) {
        if (file.endsWith('.js')) {
          const systemName = path.basename(file, '.js');
          const systemPath = path.join(systemsDir, file);
          const stats = await fs.stat(systemPath);
          
          const system = this.systems.get(systemName);
          if (system && stats.mtime > system.lastModified) {
            await this.reloadSystem(systemName);
            system.lastModified = stats.mtime;
          }
        }
      }
    }, this.config.hotReload.watchInterval);
  }

  getSystemStatus(systemName) {
    const system = this.systems.get(systemName);
    return system ? system.state : null;
  }

  getAllSystemsStatus() {
    const status = {};
    for (const [name, system] of this.systems) {
      status[name] = {
        state: system.state,
        config: this.config.systems[name]
      };
    }
    return status;
  }
}

module.exports = new SystemOrchestrator(); 