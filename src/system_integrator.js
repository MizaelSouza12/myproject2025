/**
 * WYDBR 2.0 - System Integrator
 * 
 * Este arquivo é responsável por integrar todos os sistemas gerados
 * ao motor principal do WYDBR 2.0.
 */

const path = require('path');
const fs = require('fs');

// Context do Engine para integração de sistemas
const engineContext = {
  systems: {},
  
  registerSystem: function(systemInfo) {
    this.systems[systemInfo.name] = {
      info: systemInfo,
      instance: null
    };
    console.log(`Sistema registrado: ${systemInfo.name}`);
  },
  
  initializeAllSystems: function() {
    // Inicializa sistemas em ordem baseada em dependências
    const initialized = new Set();
    const systems = Object.keys(this.systems);
    
    // Continue tentando inicializar sistemas até que todos sejam inicializados
    // ou nenhum progresso adicional seja possível
    let progress = true;
    while (progress) {
      progress = false;
      
      for (const systemName of systems) {
        if (initialized.has(systemName)) continue;
        
        const system = this.systems[systemName];
        const dependencies = system.info.dependencies || [];
        
        // Verifica se todas as dependências estão inicializadas
        const allDepsInitialized = dependencies.every(dep => 
          !this.systems[dep] || initialized.has(dep)
        );
        
        if (allDepsInitialized) {
          try {
            console.log(`Inicializando sistema: ${systemName}`);
            
            // Determinando o diretório correto baseado no nome do sistema
            let moduleDir;
            if (systemName.includes('clustering') || systemName.includes('eventos_distribuidos') || 
                systemName.includes('cache') || systemName.includes('banco_dados')) {
                moduleDir = 'core_systems';
            } else if (systemName.includes('guerra') || systemName.includes('crafting')) {
                moduleDir = 'gameplay_systems';
            } else if (systemName.includes('anti_ddos') || systemName.includes('anomalias') || 
                      systemName.includes('auditing') || systemName.includes('exploits')) {
                moduleDir = 'security_systems';
            } else if (systemName.includes('clima')) {
                moduleDir = 'environment_systems';
            } else if (systemName.includes('guildas')) {
                moduleDir = 'guild_systems';
            } else if (systemName.includes('economico')) {
                moduleDir = 'economy_systems';
            } else if (systemName.includes('eventos_mundiais')) {
                moduleDir = 'events_systems';
            }
            
            // Importando do diretório específico
            system.instance = require(`./${moduleDir}/${systemName}`).setupSystem(this);
            const result = system.instance.initialize();
            if (result) {
              initialized.add(systemName);
              progress = true;
              console.log(`Sistema inicializado com sucesso: ${systemName}`);
            } else {
              console.error(`Falha ao inicializar sistema: ${systemName}`);
            }
          } catch (error) {
            console.error(`Erro ao inicializar sistema ${systemName}: ${error.message}`);
          }
        }
      }
    }
    
    // Verifica se todos os sistemas foram inicializados
    const uninitializedSystems = systems.filter(s => !initialized.has(s));
    if (uninitializedSystems.length > 0) {
      console.error(`Os seguintes sistemas não puderam ser inicializados: ${uninitializedSystems.join(', ')}`);
      console.error('Possível problema de dependência circular ou dependência não satisfeita');
    }
    
    return initialized.size === systems.length;
  },
  
  shutdownAllSystems: function() {
    // Desliga os sistemas na ordem inversa de inicialização
    const systemNames = Object.keys(this.systems).reverse();
    for (const systemName of systemNames) {
      const system = this.systems[systemName];
      if (system.instance) {
        try {
          system.instance.shutdown();
          console.log(`Sistema desligado: ${systemName}`);
        } catch (error) {
          console.error(`Erro ao desligar sistema ${systemName}: ${error.message}`);
        }
      }
    }
  }
};

// Função principal para iniciar o mecanismo
function startEngine() {
  console.log('Iniciando WYDBR 2.0 Engine...');
  
  // Registrar todos os sistemas disponíveis
  const systemsDir = path.join(__dirname);
  registerAllSystems(systemsDir);
  
  // Inicializar todos os sistemas
  const success = engineContext.initializeAllSystems();
  if (success) {
    console.log('Todos os sistemas inicializados com sucesso!');
  } else {
    console.error('Alguns sistemas não puderam ser inicializados. Verifique os logs acima.');
  }
  
  return success;
}

// Registra todos os sistemas encontrados nos diretórios
function registerAllSystems(baseDir) {
  // Lista de diretórios de sistemas
  const systemsDirs = [
    'core_systems',
    'gameplay_systems',
    'security_systems',
    'environment_systems',
    'guild_systems',
    'economy_systems',
    'events_systems'
  ];
  
  // Registra sistemas em cada diretório
  for (const dirName of systemsDirs) {
    const dirPath = path.join(baseDir, dirName);
    if (fs.existsSync(dirPath)) {
      const files = fs.readdirSync(dirPath).filter(f => f.endsWith('.js'));
      for (const file of files) {
        try {
          require(path.join(dirPath, file)).setupSystem(engineContext);
        } catch (error) {
          console.error(`Erro ao registrar sistema ${file}: ${error.message}`);
        }
      }
    }
  }
}

// Exporta as funções e contexto
module.exports = {
  startEngine,
  engineContext,
  shutdownEngine: () => engineContext.shutdownAllSystems()
};
