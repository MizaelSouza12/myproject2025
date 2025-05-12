/**
 * WydConfig - Configurador Gráfico para WYDBRASIL 2.0
 * 
 * Este é um simulador do executável WydConfig.exe usando Node.js.
 * Em um ambiente de produção, este seria um executável compilado em C#.
 * 
 * Copyright (c) 2025 WYDBRASIL Team
 */

const fs = require('fs');
const path = require('path');
const readline = require('readline');

// Constantes
const CONFIG_PATH = path.join(process.cwd(), 'config/graphics_config.json');
const SECURITY_PATH = path.join(process.cwd(), 'security/security_config.json');

// Cores para saída no console
const colors = {
  reset: '\x1b[0m',
  red: '\x1b[31m',
  green: '\x1b[32m',
  yellow: '\x1b[33m',
  blue: '\x1b[34m',
  magenta: '\x1b[35m',
  cyan: '\x1b[36m',
  white: '\x1b[37m'
};

// Argumentos da linha de comando
const args = process.argv.slice(2);
const advancedMode = args.includes('-advanced');

// Interface de leitura para interação com o usuário
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

// Configuração gráfica
let config = null;
let security = null;

/**
 * Função principal
 */
async function main() {
  try {
    initialize();
    await run();
  } catch (error) {
    console.error(`${colors.red}Erro crítico: ${error.message}${colors.reset}`);
    console.error(error.stack);
    console.log("\nPressione Enter para sair...");
    await getUserInput();
  } finally {
    rl.close();
  }
}

/**
 * Inicializa o configurador
 */
function initialize() {
  // Configurar console
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`          WYDBRASIL 2.0 - CONFIGURADOR GRÁFICO         `);
  console.log(`=======================================================${colors.reset}`);
  console.log();
  
  // Verificar diretórios de configuração
  ensureConfigDirectories();
  
  // Carregar configurações
  loadGraphicsConfig();
  loadSecurityConfig();
  
  if (advancedMode) {
    console.log(`${colors.yellow}Modo avançado ativado - Todas as opções disponíveis${colors.reset}`);
    console.log();
  }
}

/**
 * Executa o configurador
 */
async function run() {
  let running = true;
  
  while (running) {
    displayMainMenu();
    const option = await getUserInput();
    
    switch (option) {
      case '1':
        await configureRenderer();
        break;
      case '2':
        await configureResolution();
        break;
      case '3':
        await configureQuality();
        break;
      case '4':
        await configurePerformance();
        break;
      case '5':
        if (advancedMode) {
          await configureAdvanced();
        } else {
          await resetToDefaults();
        }
        break;
      case '6':
        if (advancedMode) {
          await resetToDefaults();
        } else {
          running = false;
        }
        break;
      case '7':
        if (advancedMode) {
          running = false;
        }
        break;
      case 'q':
      case 'Q':
        running = false;
        break;
    }
  }
  
  // Salvar antes de sair
  saveGraphicsConfig();
  console.log("\nConfiguração salva com sucesso!");
  console.log("Pressione Enter para sair...");
  await getUserInput();
}

/**
 * Exibe o menu principal
 */
function displayMainMenu() {
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`          WYDBRASIL 2.0 - CONFIGURADOR GRÁFICO         `);
  console.log(`=======================================================${colors.reset}`);
  
  // Mostrar resumo de configuração atual
  console.log("\nConfiguração atual:");
  console.log(`  Renderização: ${config.graphics.renderer.toUpperCase()}`);
  console.log(`  Resolução: ${config.graphics.resolution.width}x${config.graphics.resolution.height} ` +
              `(${config.graphics.resolution.fullscreen ? "Tela cheia" : "Janela"})`);
  console.log(`  Qualidade: ${config.graphics.quality.toUpperCase()}`);
  console.log(`  FPS alvo: ${config.graphics.performance.targetFPS}`);
  
  // Menu principal
  console.log("\nOpções disponíveis:");
  console.log("  1. Configurar renderização (DX11, Vulkan, Compatibilidade)");
  console.log("  2. Configurar resolução e modo de exibição");
  console.log("  3. Configurar qualidade gráfica");
  console.log("  4. Configurar desempenho");
  
  if (advancedMode) {
    console.log("  5. Configurações avançadas");
    console.log("  6. Restaurar padrões");
    console.log("  7. Sair");
  } else {
    console.log("  5. Restaurar padrões");
    console.log("  6. Sair");
  }
  
  console.log("\nSelecione uma opção (ou Q para sair):");
}

/**
 * Obter entrada do usuário
 */
function getUserInput() {
  return new Promise(resolve => {
    rl.question('', answer => {
      resolve(answer);
    });
  });
}

/**
 * Configuração de renderização
 */
async function configureRenderer() {
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`                CONFIGURAR RENDERIZAÇÃO                `);
  console.log(`=======================================================${colors.reset}`);
  
  console.log("\nSelecione o motor de renderização:");
  console.log("  1. Automático (recomendado)");
  console.log("  2. DirectX 11");
  console.log("  3. Vulkan");
  console.log("  4. Modo de compatibilidade");
  
  console.log(`\nModo atual: ${config.graphics.renderer.toUpperCase()}`);
  console.log("\nEscolha uma opção (1-4): ");
  
  const option = await getUserInput();
  
  switch (option) {
    case '1':
      config.graphics.renderer = "auto";
      console.log("Modo Automático selecionado");
      break;
    case '2':
      config.graphics.renderer = "dx11";
      console.log("DirectX 11 selecionado");
      break;
    case '3':
      config.graphics.renderer = "vulkan";
      console.log("Vulkan selecionado");
      break;
    case '4':
      config.graphics.renderer = "compatibility";
      console.log("Modo de Compatibilidade selecionado");
      break;
    default:
      return;
  }
  
  saveGraphicsConfig();
  
  console.log(`\n${colors.green}Configuração salva com sucesso!${colors.reset}`);
  console.log("\nPressione Enter para continuar...");
  await getUserInput();
}

/**
 * Configuração de resolução
 */
async function configureResolution() {
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`               CONFIGURAR RESOLUÇÃO                    `);
  console.log(`=======================================================${colors.reset}`);
  
  console.log("\nSelecione a resolução:");
  console.log("  1. 1920x1080 (Full HD)");
  console.log("  2. 2560x1440 (2K)");
  console.log("  3. 3840x2160 (4K)");
  console.log("  4. 1280x720 (HD)");
  console.log("  5. 1366x768 (HD+)");
  console.log("  6. Personalizada");
  
  console.log(`\nResolução atual: ${config.graphics.resolution.width}x${config.graphics.resolution.height}`);
  console.log("\nEscolha uma opção (1-6): ");
  
  const option = await getUserInput();
  
  switch (option) {
    case '1':
      config.graphics.resolution.width = 1920;
      config.graphics.resolution.height = 1080;
      break;
    case '2':
      config.graphics.resolution.width = 2560;
      config.graphics.resolution.height = 1440;
      break;
    case '3':
      config.graphics.resolution.width = 3840;
      config.graphics.resolution.height = 2160;
      break;
    case '4':
      config.graphics.resolution.width = 1280;
      config.graphics.resolution.height = 720;
      break;
    case '5':
      config.graphics.resolution.width = 1366;
      config.graphics.resolution.height = 768;
      break;
    case '6':
      console.log("\nInforme a largura (pixels): ");
      const widthStr = await getUserInput();
      const width = parseInt(widthStr);
      if (!isNaN(width) && width > 0) {
        config.graphics.resolution.width = width;
      }
      
      console.log("Informe a altura (pixels): ");
      const heightStr = await getUserInput();
      const height = parseInt(heightStr);
      if (!isNaN(height) && height > 0) {
        config.graphics.resolution.height = height;
      }
      break;
    default:
      return;
  }
  
  // Configurar modo de tela
  console.log("\nSelecione o modo de exibição:");
  console.log("  1. Tela cheia");
  console.log("  2. Janela");
  console.log("  3. Janela sem bordas");
  
  console.log("\nEscolha uma opção (1-3): ");
  const displayMode = await getUserInput();
  
  switch (displayMode) {
    case '1':
      config.graphics.resolution.fullscreen = true;
      console.log("Modo Tela cheia selecionado");
      break;
    case '2':
      config.graphics.resolution.fullscreen = false;
      console.log("Modo Janela selecionado");
      break;
    case '3':
      config.graphics.resolution.fullscreen = false;
      // TODO: Implementar flag para janela sem bordas
      console.log("Modo Janela sem bordas selecionado");
      break;
    default:
      break;
  }
  
  saveGraphicsConfig();
  
  console.log(`\n${colors.green}Resolução configurada para ${config.graphics.resolution.width}x${config.graphics.resolution.height}`);
  console.log("Configuração salva com sucesso!${colors.reset}");
  console.log("\nPressione Enter para continuar...");
  await getUserInput();
}

/**
 * Configuração de qualidade gráfica
 */
async function configureQuality() {
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`             CONFIGURAR QUALIDADE GRÁFICA              `);
  console.log(`=======================================================${colors.reset}`);
  
  console.log("\nSelecione o nível de qualidade gráfica:");
  console.log("  1. Automático (baseado no hardware)");
  console.log("  2. Ultra (máxima qualidade)");
  console.log("  3. Alta");
  console.log("  4. Média");
  console.log("  5. Baixa");
  console.log("  6. Personalizada");
  
  console.log(`\nQualidade atual: ${config.graphics.quality.toUpperCase()}`);
  console.log("\nEscolha uma opção (1-6): ");
  
  const option = await getUserInput();
  
  switch (option) {
    case '1':
      config.graphics.quality = "auto";
      applyPresetQuality("auto");
      break;
    case '2':
      config.graphics.quality = "ultra";
      applyPresetQuality("ultra");
      break;
    case '3':
      config.graphics.quality = "high";
      applyPresetQuality("high");
      break;
    case '4':
      config.graphics.quality = "medium";
      applyPresetQuality("medium");
      break;
    case '5':
      config.graphics.quality = "low";
      applyPresetQuality("low");
      break;
    case '6':
      config.graphics.quality = "custom";
      await configureCustomQuality();
      break;
    default:
      return;
  }
  
  saveGraphicsConfig();
  
  console.log(`\n${colors.green}Qualidade configurada para ${config.graphics.quality.toUpperCase()}`);
  console.log(`Configuração salva com sucesso!${colors.reset}`);
  console.log("\nPressione Enter para continuar...");
  await getUserInput();
}

/**
 * Configuração de desempenho
 */
async function configurePerformance() {
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`               CONFIGURAR DESEMPENHO                   `);
  console.log(`=======================================================${colors.reset}`);
  
  console.log("\nSelecione o FPS alvo:");
  console.log("  1. 30 FPS");
  console.log("  2. 60 FPS (recomendado)");
  console.log("  3. 120 FPS");
  console.log("  4. 144 FPS");
  console.log("  5. Sem limite");
  console.log("  6. Personalizado");
  
  console.log(`\nFPS alvo atual: ${config.graphics.performance.targetFPS}`);
  console.log("\nEscolha uma opção (1-6): ");
  
  const option = await getUserInput();
  
  switch (option) {
    case '1':
      config.graphics.performance.targetFPS = 30;
      break;
    case '2':
      config.graphics.performance.targetFPS = 60;
      break;
    case '3':
      config.graphics.performance.targetFPS = 120;
      break;
    case '4':
      config.graphics.performance.targetFPS = 144;
      break;
    case '5':
      config.graphics.performance.targetFPS = 0; // 0 = sem limite
      break;
    case '6':
      console.log("\nInforme o FPS alvo (0 para sem limite): ");
      const fpsStr = await getUserInput();
      const fps = parseInt(fpsStr);
      if (!isNaN(fps) && fps >= 0) {
        config.graphics.performance.targetFPS = fps;
      }
      break;
    default:
      return;
  }
  
  // Ativar VSync
  console.log("\nHabilitar sincronização vertical (VSync)?");
  console.log("  1. Sim (elimina tearing, pode reduzir FPS)");
  console.log("  2. Não (potencialmente mais FPS, pode causar tearing)");
  
  console.log("\nEscolha uma opção (1-2): ");
  const vsyncOption = await getUserInput();
  
  switch (vsyncOption) {
    case '1':
      config.graphics.resolution.vsync = true;
      console.log("VSync ativado");
      break;
    case '2':
      config.graphics.resolution.vsync = false;
      console.log("VSync desativado");
      break;
    default:
      break;
  }
  
  // Modo de baixa especificação
  console.log("\nAtivar modo de baixa especificação? (recomendado para hardware antigo)");
  console.log("  1. Sim (melhor desempenho em hardware antigo)");
  console.log("  2. Não (recomendado para hardware moderno)");
  
  console.log("\nEscolha uma opção (1-2): ");
  const lowSpecOption = await getUserInput();
  
  switch (lowSpecOption) {
    case '1':
      config.graphics.performance.lowSpecMode = true;
      console.log("Modo de baixa especificação ativado");
      break;
    case '2':
      config.graphics.performance.lowSpecMode = false;
      console.log("Modo de baixa especificação desativado");
      break;
    default:
      break;
  }
  
  // Multithreading
  console.log("\nAtivar processamento multi-thread? (recomendado para CPUs multi-core)");
  console.log("  1. Sim (melhor desempenho em CPUs multi-core)");
  console.log("  2. Não (melhor para CPUs antigas ou com poucos núcleos)");
  
  console.log("\nEscolha uma opção (1-2): ");
  const multiThreadOption = await getUserInput();
  
  switch (multiThreadOption) {
    case '1':
      config.graphics.performance.multiThreaded = true;
      console.log("Processamento multi-thread ativado");
      break;
    case '2':
      config.graphics.performance.multiThreaded = false;
      console.log("Processamento multi-thread desativado");
      break;
    default:
      break;
  }
  
  saveGraphicsConfig();
  
  console.log(`\n${colors.green}Configurações de desempenho salvas com sucesso!${colors.reset}`);
  console.log("\nPressione Enter para continuar...");
  await getUserInput();
}

/**
 * Configurações avançadas
 */
async function configureAdvanced() {
  if (!advancedMode) return;
  
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`              CONFIGURAÇÕES AVANÇADAS                  `);
  console.log(`=======================================================${colors.reset}`);
  
  console.log("\nATENÇÃO: Estas configurações podem afetar a estabilidade do jogo.");
  console.log("Proceda com cautela e apenas se souber o que está fazendo.");
  
  console.log("\nSelecione uma opção:");
  console.log("  1. Habilitar modo de depuração");
  console.log("  2. Configurar detalhes de renderização");
  console.log("  3. Configurações de cache e memória");
  console.log("  4. Voltar");
  
  console.log("\nEscolha uma opção (1-4): ");
  
  const option = await getUserInput();
  
  switch (option) {
    case '1':
      await configureDebugOptions();
      break;
    case '2':
      await configureRenderingDetails();
      break;
    case '3':
      await configureCacheOptions();
      break;
    default:
      return;
  }
  
  saveGraphicsConfig();
  
  console.log(`\n${colors.green}Configurações avançadas salvas com sucesso!${colors.reset}`);
  console.log("\nPressione Enter para continuar...");
  await getUserInput();
}

/**
 * Configuração detalhada de qualidade personalizada
 */
async function configureCustomQuality() {
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`            CONFIGURAÇÃO PERSONALIZADA                 `);
  console.log(`=======================================================${colors.reset}`);
  
  // Configuração de sombras
  console.log("\nHabilitar sombras?");
  console.log("  1. Sim");
  console.log("  2. Não");
  
  console.log("\nEscolha uma opção (1-2): ");
  const shadowsOption = await getUserInput();
  
  config.graphics.effects.shadows = shadowsOption === '1';
  
  // Configuração de partículas
  console.log("\nHabilitar efeitos de partículas?");
  console.log("  1. Sim");
  console.log("  2. Não");
  
  console.log("\nEscolha uma opção (1-2): ");
  const particlesOption = await getUserInput();
  
  config.graphics.effects.particles = particlesOption === '1';
  
  // Configuração de pós-processamento
  console.log("\nHabilitar efeitos de pós-processamento?");
  console.log("  1. Sim");
  console.log("  2. Não");
  
  console.log("\nEscolha uma opção (1-2): ");
  const postProcessingOption = await getUserInput();
  
  config.graphics.effects.postProcessing = postProcessingOption === '1';
  
  // Configuração de bloom
  console.log("\nHabilitar efeito de bloom?");
  console.log("  1. Sim");
  console.log("  2. Não");
  
  console.log("\nEscolha uma opção (1-2): ");
  const bloomOption = await getUserInput();
  
  config.graphics.effects.bloom = bloomOption === '1';
  
  // Configuração de anti-aliasing
  console.log("\nSelecione o tipo de anti-aliasing:");
  console.log("  1. Nenhum");
  console.log("  2. FXAA (rápido)");
  console.log("  3. TAA (melhor qualidade)");
  console.log("  4. SMAA (balanceado)");
  
  console.log("\nEscolha uma opção (1-4): ");
  const antiAliasingOption = await getUserInput();
  
  switch (antiAliasingOption) {
    case '1':
      config.graphics.effects.antiAliasing = "none";
      break;
    case '2':
      config.graphics.effects.antiAliasing = "FXAA";
      break;
    case '3':
      config.graphics.effects.antiAliasing = "TAA";
      break;
    case '4':
      config.graphics.effects.antiAliasing = "SMAA";
      break;
  }
  
  // Qualidade de textura
  console.log("\nSelecione a qualidade das texturas:");
  console.log("  1. Muito Alta");
  console.log("  2. Alta");
  console.log("  3. Média");
  console.log("  4. Baixa");
  
  console.log("\nEscolha uma opção (1-4): ");
  const textureQualityOption = await getUserInput();
  
  switch (textureQualityOption) {
    case '1':
      config.graphics.effects.textureQuality = "ultra";
      break;
    case '2':
      config.graphics.effects.textureQuality = "high";
      break;
    case '3':
      config.graphics.effects.textureQuality = "medium";
      break;
    case '4':
      config.graphics.effects.textureQuality = "low";
      break;
  }
  
  // Distância de renderização
  console.log("\nSelecione a distância de renderização:");
  console.log("  1. Muito Longa");
  console.log("  2. Longa");
  console.log("  3. Média");
  console.log("  4. Curta");
  
  console.log("\nEscolha uma opção (1-4): ");
  const drawDistanceOption = await getUserInput();
  
  switch (drawDistanceOption) {
    case '1':
      config.graphics.effects.drawDistance = 2000;
      break;
    case '2':
      config.graphics.effects.drawDistance = 1000;
      break;
    case '3':
      config.graphics.effects.drawDistance = 500;
      break;
    case '4':
      config.graphics.effects.drawDistance = 250;
      break;
  }
}

/**
 * Configuração de opções de depuração
 */
async function configureDebugOptions() {
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`            CONFIGURAÇÕES DE DEPURAÇÃO                 `);
  console.log(`=======================================================${colors.reset}`);
  
  // Habilitar modo de depuração
  console.log("\nHabilitar modo de depuração?");
  console.log("  1. Sim");
  console.log("  2. Não");
  
  console.log("\nEscolha uma opção (1-2): ");
  const debugOption = await getUserInput();
  
  config.graphics.debug.enabled = debugOption === '1';
  
  // Mostrar FPS
  console.log("\nMostrar contador de FPS?");
  console.log("  1. Sim");
  console.log("  2. Não");
  
  console.log("\nEscolha uma opção (1-2): ");
  const fpsOption = await getUserInput();
  
  config.graphics.debug.showFPS = fpsOption === '1';
  
  // Mostrar estatísticas
  console.log("\nMostrar estatísticas de renderização?");
  console.log("  1. Sim");
  console.log("  2. Não");
  
  console.log("\nEscolha uma opção (1-2): ");
  const statsOption = await getUserInput();
  
  config.graphics.debug.showStats = statsOption === '1';
  
  // Configurar nível de log
  console.log("\nSelecione o nível de log:");
  console.log("  1. Erro (apenas erros críticos)");
  console.log("  2. Aviso (erros e avisos)");
  console.log("  3. Info (informações gerais)");
  console.log("  4. Debug (informações detalhadas)");
  console.log("  5. Trace (informações extremamente detalhadas)");
  
  console.log("\nEscolha uma opção (1-5): ");
  const logLevelOption = await getUserInput();
  
  switch (logLevelOption) {
    case '1':
      config.graphics.debug.logLevel = "error";
      break;
    case '2':
      config.graphics.debug.logLevel = "warning";
      break;
    case '3':
      config.graphics.debug.logLevel = "info";
      break;
    case '4':
      config.graphics.debug.logLevel = "debug";
      break;
    case '5':
      config.graphics.debug.logLevel = "trace";
      break;
  }
}

/**
 * Configuração de detalhes de renderização
 */
async function configureRenderingDetails() {
  // Implementação futura - renderização detalhada
  console.log("\nEsta funcionalidade ainda está em desenvolvimento.");
  await getUserInput();
}

/**
 * Configuração de opções de cache
 */
async function configureCacheOptions() {
  // Implementação futura - opções de cache
  console.log("\nEsta funcionalidade ainda está em desenvolvimento.");
  await getUserInput();
}

/**
 * Restaurar configurações padrão
 */
async function resetToDefaults() {
  process.stdout.write('\x1Bc'); // Limpar a tela
  console.log(`${colors.cyan}=======================================================`);
  console.log(`            RESTAURAR CONFIGURAÇÕES PADRÃO              `);
  console.log(`=======================================================${colors.reset}`);
  
  console.log("\nVocê tem certeza que deseja restaurar todas as configurações para o padrão?");
  console.log("Esta ação não pode ser desfeita.");
  console.log("\n  1. Sim, restaurar padrões");
  console.log("  2. Não, cancelar");
  
  console.log("\nEscolha uma opção (1-2): ");
  const option = await getUserInput();
  
  if (option === '1') {
    // Criar configuração padrão
    config = createDefaultConfig();
    saveGraphicsConfig();
    
    console.log(`\n${colors.green}Configuração restaurada para os valores padrão!${colors.reset}`);
    console.log("\nPressione Enter para continuar...");
    await getUserInput();
  }
}

/**
 * Aplica configurações de qualidade predefinidas
 */
function applyPresetQuality(preset) {
  switch (preset.toLowerCase()) {
    case "ultra":
      config.graphics.effects.shadows = true;
      config.graphics.effects.particles = true;
      config.graphics.effects.postProcessing = true;
      config.graphics.effects.bloom = true;
      config.graphics.effects.ambientOcclusion = true;
      config.graphics.effects.antiAliasing = "TAA";
      config.graphics.effects.textureQuality = "ultra";
      config.graphics.effects.modelDetail = "ultra";
      config.graphics.effects.drawDistance = 2000;
      config.graphics.effects.weatherEffects = true;
      config.graphics.effects.reflection = true;
      config.graphics.performance.gpuOptimization = true;
      config.graphics.performance.useGPUParticles = true;
      break;
      
    case "high":
      config.graphics.effects.shadows = true;
      config.graphics.effects.particles = true;
      config.graphics.effects.postProcessing = true;
      config.graphics.effects.bloom = true;
      config.graphics.effects.ambientOcclusion = true;
      config.graphics.effects.antiAliasing = "TAA";
      config.graphics.effects.textureQuality = "high";
      config.graphics.effects.modelDetail = "high";
      config.graphics.effects.drawDistance = 1000;
      config.graphics.effects.weatherEffects = true;
      config.graphics.effects.reflection = true;
      config.graphics.performance.gpuOptimization = true;
      config.graphics.performance.useGPUParticles = true;
      break;
      
    case "medium":
      config.graphics.effects.shadows = true;
      config.graphics.effects.particles = true;
      config.graphics.effects.postProcessing = true;
      config.graphics.effects.bloom = false;
      config.graphics.effects.ambientOcclusion = false;
      config.graphics.effects.antiAliasing = "FXAA";
      config.graphics.effects.textureQuality = "medium";
      config.graphics.effects.modelDetail = "medium";
      config.graphics.effects.drawDistance = 500;
      config.graphics.effects.weatherEffects = true;
      config.graphics.effects.reflection = false;
      config.graphics.performance.gpuOptimization = true;
      config.graphics.performance.useGPUParticles = false;
      break;
      
    case "low":
      config.graphics.effects.shadows = false;
      config.graphics.effects.particles = false;
      config.graphics.effects.postProcessing = false;
      config.graphics.effects.bloom = false;
      config.graphics.effects.ambientOcclusion = false;
      config.graphics.effects.antiAliasing = "none";
      config.graphics.effects.textureQuality = "low";
      config.graphics.effects.modelDetail = "low";
      config.graphics.effects.drawDistance = 250;
      config.graphics.effects.weatherEffects = false;
      config.graphics.effects.reflection = false;
      config.graphics.performance.gpuOptimization = true;
      config.graphics.performance.useGPUParticles = false;
      config.graphics.performance.lowSpecMode = true;
      break;
      
    case "auto":
      // Detectar hardware e aplicar configurações adequadas
      detectHardwareAndApplySettings();
      break;
  }
}

/**
 * Detecta hardware e aplica configurações adequadas
 */
function detectHardwareAndApplySettings() {
  // Em ambiente real, verificaríamos o hardware
  // Em ambiente de simulação, aplicamos configurações de alta qualidade como fallback
  applyPresetQuality("high");
}

/**
 * Garante que os diretórios de configuração existam
 */
function ensureConfigDirectories() {
  try {
    const configDir = path.dirname(CONFIG_PATH);
    const securityDir = path.dirname(SECURITY_PATH);
    
    if (!fs.existsSync(configDir)) {
      fs.mkdirSync(configDir, { recursive: true });
    }
    
    if (!fs.existsSync(securityDir)) {
      fs.mkdirSync(securityDir, { recursive: true });
    }
  } catch (error) {
    console.error(`Erro ao criar diretórios: ${error.message}`);
  }
}

/**
 * Carrega as configurações gráficas
 */
function loadGraphicsConfig() {
  try {
    if (fs.existsSync(CONFIG_PATH)) {
      const data = fs.readFileSync(CONFIG_PATH, 'utf8');
      config = JSON.parse(data);
    } else {
      config = createDefaultConfig();
      saveGraphicsConfig();
    }
  } catch (error) {
    console.error(`${colors.red}Erro ao carregar configurações: ${error.message}${colors.reset}`);
    console.log("Utilizando configuração padrão");
    
    config = createDefaultConfig();
    saveGraphicsConfig();
  }
}

/**
 * Carrega as configurações de segurança
 */
function loadSecurityConfig() {
  try {
    if (fs.existsSync(SECURITY_PATH)) {
      const data = fs.readFileSync(SECURITY_PATH, 'utf8');
      security = JSON.parse(data);
    } else {
      security = createDefaultSecurity();
      saveSecurityConfig();
    }
  } catch (error) {
    console.error(`${colors.red}Erro ao carregar configurações de segurança: ${error.message}${colors.reset}`);
    console.log("Utilizando configuração de segurança padrão");
    
    security = createDefaultSecurity();
    saveSecurityConfig();
  }
}

/**
 * Cria configurações padrão
 */
function createDefaultConfig() {
  return {
    version: "2.0.0",
    lastUpdate: new Date().toISOString(),
    signature: "",
    graphics: {
      renderer: "auto",
      preferredRenderers: ["dx11", "vulkan", "compatibility"],
      quality: "high",
      resolution: {
        width: 1920,
        height: 1080,
        fullscreen: true,
        vsync: true
      },
      effects: {
        shadows: true,
        particles: true,
        postProcessing: true,
        bloom: true,
        ambientOcclusion: true,
        antiAliasing: "TAA",
        textureQuality: "high",
        modelDetail: "high",
        drawDistance: 1000,
        weatherEffects: true,
        reflection: true
      },
      performance: {
        targetFPS: 60,
        lowSpecMode: false,
        gpuOptimization: true,
        multiThreaded: true,
        useGPUParticles: true
      },
      debug: {
        enabled: false,
        showFPS: false,
        showStats: false,
        logLevel: "info",
        validateShaders: false
      }
    },
    security: {
      fileIntegrityCheck: true,
      configVersion: 2,
      configChecksum: "",
      executableOnly: true,
      disableBatchExecution: true,
      signatureVerification: true,
      preventScriptExecution: true
    },
    executables: {
      launcher: {
        path: "WYDLauncher.exe",
        parameters: {},
        requiredSignature: true
      },
      client: {
        path: "WYD.exe",
        parameters: {},
        requiredSignature: true
      },
      config: {
        path: "WydConfig.exe",
        parameters: {},
        requiredSignature: true
      }
    }
  };
}

/**
 * Cria configurações de segurança padrão
 */
function createDefaultSecurity() {
  return {
    lastUpdate: new Date().toISOString(),
    security: {
      fileIntegrityCheck: true,
      preventScriptExecution: true,
      executableOnly: true,
      signatureVerification: false
    }
  };
}

/**
 * Salva as configurações gráficas
 */
function saveGraphicsConfig() {
  try {
    config.lastUpdate = new Date().toISOString();
    const data = JSON.stringify(config, null, 2);
    fs.writeFileSync(CONFIG_PATH, data);
  } catch (error) {
    console.error(`${colors.red}Erro ao salvar configurações: ${error.message}${colors.reset}`);
  }
}

/**
 * Salva as configurações de segurança
 */
function saveSecurityConfig() {
  try {
    security.lastUpdate = new Date().toISOString();
    const data = JSON.stringify(security, null, 2);
    fs.writeFileSync(SECURITY_PATH, data);
  } catch (error) {
    console.error(`${colors.red}Erro ao salvar configurações de segurança: ${error.message}${colors.reset}`);
  }
}

// Iniciar o programa
main();