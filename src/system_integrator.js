/**
 * WYDBR 2.0 - System Integrator
 * 
 * Este arquivo é responsável por integrar todos os sistemas gerados
 * ao motor principal do WYDBR 2.0.
 */

const path = require('path');
const fs = require('fs');

// WYDBR 2.0 - Novo System Integrator Simples

class SystemIntegrator {
  constructor() {
    this.systems = {};
    this.initialized = new Set();
    this.failed = new Set();
    this.rolledBack = new Set();
  }

  registerSystem({ name, dependencies = [], mock }) {
    this.systems[name] = {
      name,
      dependencies,
      mock,
      status: 'registered',
      instance: null
    };
  }

  initializeAll() {
    // Inicializa todos os sistemas respeitando dependências
    let progress = true;
    while (progress) {
      progress = false;
      for (const name in this.systems) {
        const sys = this.systems[name];
        if (sys.status !== 'registered') continue;
        if (!sys.dependencies.every(dep => this.systems[dep] && this.systems[dep].status === 'initialized')) continue;
        try {
          sys.instance = sys.mock ? sys.mock() : { initialize: () => true, shutdown: () => true };
          if (sys.instance.initialize() === true) {
            sys.status = 'initialized';
            this.initialized.add(name);
            progress = true;
          } else {
            throw new Error('Falha de inicialização');
          }
        } catch (e) {
          sys.status = 'failed';
          this.failed.add(name);
          this.rollbackDependents(name);
          return false;
        }
      }
    }
    return Object.values(this.systems).every(s => s.status === 'initialized');
  }

  rollbackDependents(failedName) {
    for (const name in this.systems) {
      const sys = this.systems[name];
      if (sys.dependencies.includes(failedName) && sys.status === 'initialized') {
        if (sys.instance && typeof sys.instance.shutdown === 'function') sys.instance.shutdown();
        sys.status = 'rolled_back';
        this.rolledBack.add(name);
        this.initialized.delete(name);
      }
    }
  }

  getStatus() {
    const status = {};
    for (const name in this.systems) {
      status[name] = this.systems[name].status;
    }
    return status;
  }
}

module.exports = SystemIntegrator;