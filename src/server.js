const orchestrator = require('./orchestrator');

async function startServer() {
  try {
    // Inicializa todos os sistemas
    await orchestrator.initializeAll();
    
    // Configura listeners de eventos
    orchestrator.events.on('system:initialized', ({ system }) => {
      console.log(`Sistema ${system} inicializado com sucesso!`);
    });

    orchestrator.events.on('system:error', ({ system, error }) => {
      console.error(`Erro no sistema ${system}:`, error);
    });

    orchestrator.events.on('system:reload', ({ system }) => {
      console.log(`Sistema ${system} recarregado com sucesso!`);
    });

    // Inicia o loop principal do servidor
    console.log('Servidor WYDStudio rodando!');
    
    // Exemplo de como acessar status dos sistemas
    setInterval(() => {
      const status = orchestrator.getAllSystemsStatus();
      console.log('Status dos sistemas:', status);
    }, 30000); // Log a cada 30 segundos

  } catch (err) {
    console.error('Erro fatal ao iniciar servidor:', err);
    await orchestrator.shutdownAll();
    process.exit(1);
  }
}

// Tratamento de sinais do sistema
process.on('SIGINT', async () => {
  console.log('\nDesligando servidor...');
  await orchestrator.shutdownAll();
  process.exit(0);
});

process.on('SIGTERM', async () => {
  console.log('\nDesligando servidor...');
  await orchestrator.shutdownAll();
  process.exit(0);
});

startServer(); 