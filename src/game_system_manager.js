const EventEmitter = require('events');

class GameSystemManager extends EventEmitter {
  constructor() {
    super();
    this.systems = new Map();
    this.states = new Map();
    this.priorities = new Map();
  }

  // Estados possíveis dos sistemas
  static STATES = {
    INACTIVE: 'inactive',
    LOADING: 'loading',
    ACTIVE: 'active',
    ERROR: 'error',
    MAINTENANCE: 'maintenance',
    ROLLED_BACK: 'rolled_back'
  };

  // Prioridades de inicialização
  static PRIORITIES = {
    CRITICAL: 0,    // Banco de dados, rede
    HIGH: 1,        // Combate, inventário
    MEDIUM: 2,      // Missões, guildas
    LOW: 3          // Chat, eventos
  };

  registerSystem(name, system, priority = GameSystemManager.PRIORITIES.MEDIUM) {
    this.systems.set(name, {
      instance: system,
      state: GameSystemManager.STATES.INACTIVE,
      priority,
      dependencies: system.dependencies || []
    });
    this.priorities.set(name, priority);
    this.emit('systemRegistered', name);
  }

  async initializeSystem(name) {
    const system = this.systems.get(name);
    if (!system) throw new Error(`Sistema ${name} não encontrado`);

    // Verifica dependências
    for (const dep of system.dependencies) {
      const depSystem = this.systems.get(dep);
      if (!depSystem || depSystem.state !== GameSystemManager.STATES.ACTIVE) {
        return false; // Retorna false em vez de lançar erro
      }
    }

    try {
      system.state = GameSystemManager.STATES.LOADING;
      this.emit('systemLoading', name);

      await system.instance.initialize();
      
      system.state = GameSystemManager.STATES.ACTIVE;
      this.emit('systemActivated', name);
      return true;
    } catch (error) {
      system.state = GameSystemManager.STATES.ERROR;
      this.emit('systemError', { name, error });
      await this.rollbackDependents(name);
      return false;
    }
  }

  async initializeAll() {
    // Ordena sistemas por prioridade
    const orderedSystems = Array.from(this.systems.entries())
      .sort((a, b) => this.priorities.get(a[0]) - this.priorities.get(b[0]));

    for (const [name, system] of orderedSystems) {
      if (system.state === GameSystemManager.STATES.INACTIVE) {
        const success = await this.initializeSystem(name);
        if (!success) {
          // Se a inicialização falhou, para o processo
          break;
        }
      }
    }
  }

  async rollbackDependents(failedSystem) {
    // Primeiro, encontra todos os sistemas que dependem do sistema que falhou
    const dependents = Array.from(this.systems.entries())
      .filter(([_, system]) => system.dependencies.includes(failedSystem));

    // Depois, reverte cada dependente
    for (const [name, system] of dependents) {
      if (system.state === GameSystemManager.STATES.ACTIVE) {
        try {
          await system.instance.shutdown();
          system.state = GameSystemManager.STATES.ROLLED_BACK;
          this.emit('systemRolledBack', name);
        } catch (error) {
          this.emit('rollbackError', { name, error });
        }
      }
    }
  }

  getSystemStatus(name) {
    const system = this.systems.get(name);
    return system ? system.state : null;
  }

  getAllSystemsStatus() {
    const status = {};
    for (const [name, system] of this.systems) {
      status[name] = {
        state: system.state,
        priority: this.priorities.get(name),
        dependencies: system.dependencies
      };
    }
    return status;
  }

  async shutdownSystem(name) {
    const system = this.systems.get(name);
    if (!system) return;

    try {
      await system.instance.shutdown();
      system.state = GameSystemManager.STATES.INACTIVE;
      this.emit('systemShutdown', name);
    } catch (error) {
      this.emit('shutdownError', { name, error });
    }
  }

  async shutdownAll() {
    // Desliga na ordem inversa da inicialização
    const orderedSystems = Array.from(this.systems.entries())
      .sort((a, b) => this.priorities.get(b[0]) - this.priorities.get(a[0]));

    for (const [name] of orderedSystems) {
      await this.shutdownSystem(name);
    }
  }
}

module.exports = GameSystemManager; 