/**
 * WYDBR 2.0 - Ponto de Entrada Principal
 * 
 * Este arquivo é o ponto de entrada principal para o sistema WYDBR 2.0.
 * Ele inicializa o integrador de sistemas e gerencia o ciclo de vida da aplicação.
 */

const systemIntegrator = require('./system_integrator');

// Função para iniciar o sistema
function startWYDBR() {
  console.log('Iniciando WYDBR 2.0...');
  
  // Inicializa todos os sistemas através do integrador
  const success = systemIntegrator.startEngine();
  
  if (success) {
    console.log('WYDBR 2.0 inicializado com sucesso!');
    setupShutdownHandlers();
  } else {
    console.error('Falha na inicialização do WYDBR 2.0!');
    process.exit(1);
  }
}

// Configura handlers para desligar o sistema corretamente
function setupShutdownHandlers() {
  // Captura Ctrl+C
  process.on('SIGINT', () => {
    console.log('Sinal de interrupção recebido. Desligando WYDBR 2.0...');
    systemIntegrator.shutdownEngine();
    process.exit(0);
  });
  
  // Captura término do processo
  process.on('SIGTERM', () => {
    console.log('Sinal de término recebido. Desligando WYDBR 2.0...');
    systemIntegrator.shutdownEngine();
    process.exit(0);
  });
  
  // Captura exceções não tratadas
  process.on('uncaughtException', (error) => {
    console.error('Exceção não tratada:', error);
    systemIntegrator.shutdownEngine();
    process.exit(1);
  });
}

// Inicia o sistema
startWYDBR();
