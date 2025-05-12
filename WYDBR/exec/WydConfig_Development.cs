/**
 * WydConfig - Configurador Gráfico para WYDBR
 * 
 * Este é o código-fonte para o executável WydConfig.exe, responsável por
 * configurar as opções gráficas e de desempenho do jogo de forma segura.
 * O sistema foi projetado para não depender de nenhum arquivo .bat ou script externo.
 * 
 * Copyright (c) 2025 WYDBR Team
 */

using System;
using System.IO;
using System.Diagnostics;
using System.Security.Cryptography;
using System.Text.Json;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Text.Json.Serialization;

namespace WYDBR
{
    /// <summary>
    /// Classe principal do configurador
    /// </summary>
    public class ConfigTool
    {
        private const string CONFIG_PATH = "config/graphics_config.json";
        private const string SECURITY_PATH = "security/security_config.json";
        
        private GraphicsConfig _config;
        private SecurityConfig _security;
        private bool _advancedMode = false;
        
        // Paleta de cores para a interface
        private static readonly ConsoleColor TITLE_COLOR = ConsoleColor.Cyan;
        private static readonly ConsoleColor HIGHLIGHT_COLOR = ConsoleColor.Yellow;
        private static readonly ConsoleColor SUCCESS_COLOR = ConsoleColor.Green;
        private static readonly ConsoleColor ERROR_COLOR = ConsoleColor.Red;
        private static readonly ConsoleColor INFO_COLOR = ConsoleColor.White;
        
        /// <summary>
        /// Ponto de entrada principal
        /// </summary>
        public static void Main(string[] args)
        {
            try
            {
                var tool = new ConfigTool();
                tool.Initialize(args);
                tool.Run();
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ERROR_COLOR;
                Console.WriteLine($"Erro crítico: {ex.Message}");
                Console.WriteLine(ex.StackTrace);
                Console.ResetColor();
                Console.WriteLine("\nPressione qualquer tecla para sair...");
                Console.ReadKey();
            }
        }
        
        /// <summary>
        /// Inicializa o configurador
        /// </summary>
        public void Initialize(string[] args)
        {
            // Analisar argumentos de linha de comando
            _advancedMode = args.Any(arg => arg.ToLower() == "-advanced");
            
            // Configurar console
            Console.Title = "WYDBR - Configurador Gráfico";
            Console.Clear();
            
            // Exibir cabeçalho
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("          WYDBR - CONFIGURADOR GRÁFICO         ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            Console.WriteLine();
            
            // Verificar arquivos de configuração
            EnsureConfigDirectories();
            
            // Carregar configurações
            LoadGraphicsConfig();
            LoadSecurityConfig();
            
            if (_advancedMode)
            {
                Console.ForegroundColor = HIGHLIGHT_COLOR;
                Console.WriteLine("Modo avançado ativado - Todas as opções disponíveis");
                Console.ResetColor();
                Console.WriteLine();
            }
        }
        
        /// <summary>
        /// Executa o configurador, exibindo o menu principal
        /// </summary>
        public void Run()
        {
            bool running = true;
            
            while (running)
            {
                DisplayMainMenu();
                var key = Console.ReadKey(true);
                
                switch (key.KeyChar)
                {
                    case '1':
                        ConfigureRenderer();
                        break;
                    case '2':
                        ConfigureResolution();
                        break;
                    case '3':
                        ConfigureQuality();
                        break;
                    case '4':
                        ConfigurePerformance();
                        break;
                    case '5':
                        if (_advancedMode)
                            ConfigureAdvanced();
                        else
                            ResetToDefaults();
                        break;
                    case '6':
                        if (_advancedMode)
                            ResetToDefaults();
                        else
                            running = false;
                        break;
                    case '7':
                        if (_advancedMode)
                            running = false;
                        break;
                    case 'q':
                    case 'Q':
                        running = false;
                        break;
                }
            }
            
            // Salvar antes de sair
            SaveGraphicsConfig();
            Console.WriteLine("\nConfiguração salva com sucesso!");
            Console.WriteLine("Pressione qualquer tecla para sair...");
            Console.ReadKey();
        }
        
        /// <summary>
        /// Exibe o menu principal
        /// </summary>
        private void DisplayMainMenu()
        {
            Console.Clear();
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("          WYDBR - CONFIGURADOR GRÁFICO         ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            
            // Mostrar resumo de configuração atual
            Console.WriteLine("\nConfiguração atual:");
            Console.ForegroundColor = INFO_COLOR;
            Console.WriteLine($"  Renderização: {_config.graphics.renderer.ToUpper()}");
            Console.WriteLine($"  Resolução: {_config.graphics.resolution.width}x{_config.graphics.resolution.height} " +
                              $"({(_config.graphics.resolution.fullscreen ? "Tela cheia" : "Janela")})");
            Console.WriteLine($"  Qualidade: {_config.graphics.quality.ToUpper()}");
            Console.WriteLine($"  FPS alvo: {_config.graphics.performance.targetFPS}");
            Console.ResetColor();
            
            // Menu principal
            Console.WriteLine("\nOpções disponíveis:");
            Console.WriteLine("  1. Configurar renderização (DX11, Vulkan, Compatibilidade)");
            Console.WriteLine("  2. Configurar resolução e modo de exibição");
            Console.WriteLine("  3. Configurar qualidade gráfica");
            Console.WriteLine("  4. Configurar desempenho");
            
            if (_advancedMode)
            {
                Console.WriteLine("  5. Configurações avançadas");
                Console.WriteLine("  6. Restaurar padrões");
                Console.WriteLine("  7. Sair");
            }
            else
            {
                Console.WriteLine("  5. Restaurar padrões");
                Console.WriteLine("  6. Sair");
            }
            
            Console.WriteLine("\nSelecione uma opção (ou Q para sair):");
        }
        
        /// <summary>
        /// Configuração de renderização
        /// </summary>
        private void ConfigureRenderer()
        {
            Console.Clear();
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("                CONFIGURAR RENDERIZAÇÃO                ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            
            Console.WriteLine("\nSelecione o motor de renderização:");
            Console.WriteLine("  1. Automático (recomendado)");
            Console.WriteLine("  2. DirectX 11");
            Console.WriteLine("  3. Vulkan");
            Console.WriteLine("  4. Modo de compatibilidade");
            
            Console.WriteLine("\nModo atual: " + _config.graphics.renderer.ToUpper());
            Console.Write("\nEscolha uma opção (1-4): ");
            
            var key = Console.ReadKey(true);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.renderer = "auto";
                    Console.WriteLine("Modo Automático selecionado");
                    break;
                case '2':
                    _config.graphics.renderer = "dx11";
                    Console.WriteLine("DirectX 11 selecionado");
                    break;
                case '3':
                    _config.graphics.renderer = "vulkan";
                    Console.WriteLine("Vulkan selecionado");
                    break;
                case '4':
                    _config.graphics.renderer = "compatibility";
                    Console.WriteLine("Modo de Compatibilidade selecionado");
                    break;
                default:
                    return;
            }
            
            SaveGraphicsConfig();
            
            Console.ForegroundColor = SUCCESS_COLOR;
            Console.WriteLine("\nConfiguração salva com sucesso!");
            Console.ResetColor();
            Console.WriteLine("\nPressione qualquer tecla para continuar...");
            Console.ReadKey();
        }
        
        /// <summary>
        /// Configuração de resolução
        /// </summary>
        private void ConfigureResolution()
        {
            Console.Clear();
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("               CONFIGURAR RESOLUÇÃO                    ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            
            Console.WriteLine("\nSelecione a resolução:");
            Console.WriteLine("  1. 1920x1080 (Full HD)");
            Console.WriteLine("  2. 2560x1440 (2K)");
            Console.WriteLine("  3. 3840x2160 (4K)");
            Console.WriteLine("  4. 1280x720 (HD)");
            Console.WriteLine("  5. 1366x768 (HD+)");
            Console.WriteLine("  6. Personalizada");
            
            Console.WriteLine($"\nResolução atual: {_config.graphics.resolution.width}x{_config.graphics.resolution.height}");
            Console.Write("\nEscolha uma opção (1-6): ");
            
            var key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.resolution.width = 1920;
                    _config.graphics.resolution.height = 1080;
                    break;
                case '2':
                    _config.graphics.resolution.width = 2560;
                    _config.graphics.resolution.height = 1440;
                    break;
                case '3':
                    _config.graphics.resolution.width = 3840;
                    _config.graphics.resolution.height = 2160;
                    break;
                case '4':
                    _config.graphics.resolution.width = 1280;
                    _config.graphics.resolution.height = 720;
                    break;
                case '5':
                    _config.graphics.resolution.width = 1366;
                    _config.graphics.resolution.height = 768;
                    break;
                case '6':
                    Console.Write("\nInforme a largura (pixels): ");
                    if (int.TryParse(Console.ReadLine(), out int width) && width > 0)
                    {
                        _config.graphics.resolution.width = width;
                    }
                    
                    Console.Write("Informe a altura (pixels): ");
                    if (int.TryParse(Console.ReadLine(), out int height) && height > 0)
                    {
                        _config.graphics.resolution.height = height;
                    }
                    break;
                default:
                    return;
            }
            
            // Configurar modo de tela
            Console.WriteLine("\nSelecione o modo de exibição:");
            Console.WriteLine("  1. Tela cheia");
            Console.WriteLine("  2. Janela");
            Console.WriteLine("  3. Janela sem bordas");
            
            Console.Write("\nEscolha uma opção (1-3): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.resolution.fullscreen = true;
                    Console.WriteLine("Modo Tela cheia selecionado");
                    break;
                case '2':
                    _config.graphics.resolution.fullscreen = false;
                    Console.WriteLine("Modo Janela selecionado");
                    break;
                case '3':
                    _config.graphics.resolution.fullscreen = false;
                    // TODO: Implementar flag para janela sem bordas
                    Console.WriteLine("Modo Janela sem bordas selecionado");
                    break;
                default:
                    break;
            }
            
            SaveGraphicsConfig();
            
            Console.ForegroundColor = SUCCESS_COLOR;
            Console.WriteLine($"\nResolução configurada para {_config.graphics.resolution.width}x{_config.graphics.resolution.height}");
            Console.WriteLine("Configuração salva com sucesso!");
            Console.ResetColor();
            Console.WriteLine("\nPressione qualquer tecla para continuar...");
            Console.ReadKey();
        }
        
        /// <summary>
        /// Configuração de qualidade gráfica
        /// </summary>
        private void ConfigureQuality()
        {
            Console.Clear();
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("             CONFIGURAR QUALIDADE GRÁFICA              ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            
            Console.WriteLine("\nSelecione o nível de qualidade gráfica:");
            Console.WriteLine("  1. Automático (baseado no hardware)");
            Console.WriteLine("  2. Ultra (máxima qualidade)");
            Console.WriteLine("  3. Alta");
            Console.WriteLine("  4. Média");
            Console.WriteLine("  5. Baixa");
            Console.WriteLine("  6. Personalizada");
            
            Console.WriteLine($"\nQualidade atual: {_config.graphics.quality.ToUpper()}");
            Console.Write("\nEscolha uma opção (1-6): ");
            
            var key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.quality = "auto";
                    ApplyPresetQuality("auto");
                    break;
                case '2':
                    _config.graphics.quality = "ultra";
                    ApplyPresetQuality("ultra");
                    break;
                case '3':
                    _config.graphics.quality = "high";
                    ApplyPresetQuality("high");
                    break;
                case '4':
                    _config.graphics.quality = "medium";
                    ApplyPresetQuality("medium");
                    break;
                case '5':
                    _config.graphics.quality = "low";
                    ApplyPresetQuality("low");
                    break;
                case '6':
                    _config.graphics.quality = "custom";
                    ConfigureCustomQuality();
                    break;
                default:
                    return;
            }
            
            SaveGraphicsConfig();
            
            Console.ForegroundColor = SUCCESS_COLOR;
            Console.WriteLine($"\nQualidade configurada para {_config.graphics.quality.ToUpper()}");
            Console.WriteLine("Configuração salva com sucesso!");
            Console.ResetColor();
            Console.WriteLine("\nPressione qualquer tecla para continuar...");
            Console.ReadKey();
        }
        
        /// <summary>
        /// Configuração de desempenho
        /// </summary>
        private void ConfigurePerformance()
        {
            Console.Clear();
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("               CONFIGURAR DESEMPENHO                   ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            
            Console.WriteLine("\nSelecione o FPS alvo:");
            Console.WriteLine("  1. 30 FPS");
            Console.WriteLine("  2. 60 FPS (recomendado)");
            Console.WriteLine("  3. 120 FPS");
            Console.WriteLine("  4. 144 FPS");
            Console.WriteLine("  5. Sem limite");
            Console.WriteLine("  6. Personalizado");
            
            Console.WriteLine($"\nFPS alvo atual: {_config.graphics.performance.targetFPS}");
            Console.Write("\nEscolha uma opção (1-6): ");
            
            var key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.performance.targetFPS = 30;
                    break;
                case '2':
                    _config.graphics.performance.targetFPS = 60;
                    break;
                case '3':
                    _config.graphics.performance.targetFPS = 120;
                    break;
                case '4':
                    _config.graphics.performance.targetFPS = 144;
                    break;
                case '5':
                    _config.graphics.performance.targetFPS = 0; // 0 = sem limite
                    break;
                case '6':
                    Console.Write("\nInforme o FPS alvo (0 para sem limite): ");
                    if (int.TryParse(Console.ReadLine(), out int fps) && fps >= 0)
                    {
                        _config.graphics.performance.targetFPS = fps;
                    }
                    break;
                default:
                    return;
            }
            
            // Ativar VSync
            Console.WriteLine("\nHabilitar sincronização vertical (VSync)?");
            Console.WriteLine("  1. Sim (elimina tearing, pode reduzir FPS)");
            Console.WriteLine("  2. Não (potencialmente mais FPS, pode causar tearing)");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.resolution.vsync = true;
                    Console.WriteLine("VSync ativado");
                    break;
                case '2':
                    _config.graphics.resolution.vsync = false;
                    Console.WriteLine("VSync desativado");
                    break;
                default:
                    break;
            }
            
            // Modo de baixa especificação
            Console.WriteLine("\nAtivar modo de baixa especificação? (recomendado para hardware antigo)");
            Console.WriteLine("  1. Sim (melhor desempenho em hardware antigo)");
            Console.WriteLine("  2. Não (recomendado para hardware moderno)");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.performance.lowSpecMode = true;
                    Console.WriteLine("Modo de baixa especificação ativado");
                    break;
                case '2':
                    _config.graphics.performance.lowSpecMode = false;
                    Console.WriteLine("Modo de baixa especificação desativado");
                    break;
                default:
                    break;
            }
            
            // Multithreading
            Console.WriteLine("\nAtivar processamento multi-thread? (recomendado para CPUs multi-core)");
            Console.WriteLine("  1. Sim (melhor desempenho em CPUs multi-core)");
            Console.WriteLine("  2. Não (melhor para CPUs antigas ou com poucos núcleos)");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.performance.multiThreaded = true;
                    Console.WriteLine("Processamento multi-thread ativado");
                    break;
                case '2':
                    _config.graphics.performance.multiThreaded = false;
                    Console.WriteLine("Processamento multi-thread desativado");
                    break;
                default:
                    break;
            }
            
            SaveGraphicsConfig();
            
            Console.ForegroundColor = SUCCESS_COLOR;
            Console.WriteLine("\nConfigurações de desempenho salvas com sucesso!");
            Console.ResetColor();
            Console.WriteLine("\nPressione qualquer tecla para continuar...");
            Console.ReadKey();
        }
        
        /// <summary>
        /// Configurações avançadas (apenas no modo avançado)
        /// </summary>
        private void ConfigureAdvanced()
        {
            if (!_advancedMode) return;
            
            Console.Clear();
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("              CONFIGURAÇÕES AVANÇADAS                  ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            
            Console.WriteLine("\nATENÇÃO: Estas configurações podem afetar a estabilidade do jogo.");
            Console.WriteLine("Proceda com cautela e apenas se souber o que está fazendo.");
            
            Console.WriteLine("\nSelecione uma opção:");
            Console.WriteLine("  1. Habilitar modo de depuração");
            Console.WriteLine("  2. Configurar detalhes de renderização");
            Console.WriteLine("  3. Configurações de cache e memória");
            Console.WriteLine("  4. Voltar");
            
            Console.Write("\nEscolha uma opção (1-4): ");
            
            var key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    ConfigureDebugOptions();
                    break;
                case '2':
                    ConfigureRenderingDetails();
                    break;
                case '3':
                    ConfigureCacheOptions();
                    break;
                default:
                    return;
            }
            
            SaveGraphicsConfig();
            
            Console.ForegroundColor = SUCCESS_COLOR;
            Console.WriteLine("\nConfigurações avançadas salvas com sucesso!");
            Console.ResetColor();
            Console.WriteLine("\nPressione qualquer tecla para continuar...");
            Console.ReadKey();
        }
        
        /// <summary>
        /// Configuração detalhada de qualidade personalizada
        /// </summary>
        private void ConfigureCustomQuality()
        {
            Console.Clear();
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("            CONFIGURAÇÃO PERSONALIZADA                 ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            
            // Configuração de sombras
            Console.WriteLine("\nHabilitar sombras?");
            Console.WriteLine("  1. Sim");
            Console.WriteLine("  2. Não");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            var key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            _config.graphics.effects.shadows = key.KeyChar == '1';
            
            // Configuração de partículas
            Console.WriteLine("\nHabilitar efeitos de partículas?");
            Console.WriteLine("  1. Sim");
            Console.WriteLine("  2. Não");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            _config.graphics.effects.particles = key.KeyChar == '1';
            
            // Configuração de pós-processamento
            Console.WriteLine("\nHabilitar efeitos de pós-processamento?");
            Console.WriteLine("  1. Sim");
            Console.WriteLine("  2. Não");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            _config.graphics.effects.postProcessing = key.KeyChar == '1';
            
            // Configuração de bloom
            Console.WriteLine("\nHabilitar efeito de bloom?");
            Console.WriteLine("  1. Sim");
            Console.WriteLine("  2. Não");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            _config.graphics.effects.bloom = key.KeyChar == '1';
            
            // Configuração de anti-aliasing
            Console.WriteLine("\nSelecione o tipo de anti-aliasing:");
            Console.WriteLine("  1. Nenhum");
            Console.WriteLine("  2. FXAA (rápido)");
            Console.WriteLine("  3. TAA (melhor qualidade)");
            Console.WriteLine("  4. SMAA (balanceado)");
            
            Console.Write("\nEscolha uma opção (1-4): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.effects.antiAliasing = "none";
                    break;
                case '2':
                    _config.graphics.effects.antiAliasing = "FXAA";
                    break;
                case '3':
                    _config.graphics.effects.antiAliasing = "TAA";
                    break;
                case '4':
                    _config.graphics.effects.antiAliasing = "SMAA";
                    break;
            }
            
            // Qualidade de textura
            Console.WriteLine("\nSelecione a qualidade das texturas:");
            Console.WriteLine("  1. Muito Alta");
            Console.WriteLine("  2. Alta");
            Console.WriteLine("  3. Média");
            Console.WriteLine("  4. Baixa");
            
            Console.Write("\nEscolha uma opção (1-4): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.effects.textureQuality = "ultra";
                    break;
                case '2':
                    _config.graphics.effects.textureQuality = "high";
                    break;
                case '3':
                    _config.graphics.effects.textureQuality = "medium";
                    break;
                case '4':
                    _config.graphics.effects.textureQuality = "low";
                    break;
            }
            
            // Distância de renderização
            Console.WriteLine("\nSelecione a distância de renderização:");
            Console.WriteLine("  1. Muito Longa");
            Console.WriteLine("  2. Longa");
            Console.WriteLine("  3. Média");
            Console.WriteLine("  4. Curta");
            
            Console.Write("\nEscolha uma opção (1-4): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.effects.drawDistance = 2000;
                    break;
                case '2':
                    _config.graphics.effects.drawDistance = 1000;
                    break;
                case '3':
                    _config.graphics.effects.drawDistance = 500;
                    break;
                case '4':
                    _config.graphics.effects.drawDistance = 250;
                    break;
            }
        }
        
        /// <summary>
        /// Configuração de opções de depuração
        /// </summary>
        private void ConfigureDebugOptions()
        {
            Console.Clear();
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("            CONFIGURAÇÕES DE DEPURAÇÃO                 ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            
            // Habilitar modo de depuração
            Console.WriteLine("\nHabilitar modo de depuração?");
            Console.WriteLine("  1. Sim");
            Console.WriteLine("  2. Não");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            var key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            _config.graphics.debug.enabled = key.KeyChar == '1';
            
            // Mostrar FPS
            Console.WriteLine("\nMostrar contador de FPS?");
            Console.WriteLine("  1. Sim");
            Console.WriteLine("  2. Não");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            _config.graphics.debug.showFPS = key.KeyChar == '1';
            
            // Mostrar estatísticas
            Console.WriteLine("\nMostrar estatísticas de renderização?");
            Console.WriteLine("  1. Sim");
            Console.WriteLine("  2. Não");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            _config.graphics.debug.showStats = key.KeyChar == '1';
            
            // Configurar nível de log
            Console.WriteLine("\nSelecione o nível de log:");
            Console.WriteLine("  1. Erro (apenas erros críticos)");
            Console.WriteLine("  2. Aviso (erros e avisos)");
            Console.WriteLine("  3. Info (informações gerais)");
            Console.WriteLine("  4. Debug (informações detalhadas)");
            Console.WriteLine("  5. Trace (informações extremamente detalhadas)");
            
            Console.Write("\nEscolha uma opção (1-5): ");
            key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            switch (key.KeyChar)
            {
                case '1':
                    _config.graphics.debug.logLevel = "error";
                    break;
                case '2':
                    _config.graphics.debug.logLevel = "warning";
                    break;
                case '3':
                    _config.graphics.debug.logLevel = "info";
                    break;
                case '4':
                    _config.graphics.debug.logLevel = "debug";
                    break;
                case '5':
                    _config.graphics.debug.logLevel = "trace";
                    break;
            }
        }
        
        /// <summary>
        /// Configuração de detalhes de renderização
        /// </summary>
        private void ConfigureRenderingDetails()
        {
            // Implementação futura - renderização detalhada
            Console.WriteLine("\nEsta funcionalidade ainda está em desenvolvimento.");
        }
        
        /// <summary>
        /// Configuração de opções de cache
        /// </summary>
        private void ConfigureCacheOptions()
        {
            // Implementação futura - opções de cache
            Console.WriteLine("\nEsta funcionalidade ainda está em desenvolvimento.");
        }
        
        /// <summary>
        /// Restaurar configurações padrão
        /// </summary>
        private void ResetToDefaults()
        {
            Console.Clear();
            Console.ForegroundColor = TITLE_COLOR;
            Console.WriteLine("=======================================================");
            Console.WriteLine("            RESTAURAR CONFIGURAÇÕES PADRÃO              ");
            Console.WriteLine("=======================================================");
            Console.ResetColor();
            
            Console.WriteLine("\nVocê tem certeza que deseja restaurar todas as configurações para o padrão?");
            Console.WriteLine("Esta ação não pode ser desfeita.");
            Console.WriteLine("\n  1. Sim, restaurar padrões");
            Console.WriteLine("  2. Não, cancelar");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            var key = Console.ReadKey(true);
            Console.WriteLine(key.KeyChar);
            
            if (key.KeyChar == '1')
            {
                // Criar configuração padrão
                _config = new GraphicsConfig
                {
                    version = "2.0.0",
                    lastUpdate = DateTime.UtcNow.ToString("o"),
                    signature = "",
                    graphics = new Graphics
                    {
                        renderer = "auto",
                        preferredRenderers = new[] { "dx11", "vulkan", "compatibility" },
                        quality = "high",
                        resolution = new Resolution
                        {
                            width = 1920,
                            height = 1080,
                            fullscreen = true,
                            vsync = true
                        },
                        effects = new Effects
                        {
                            shadows = true,
                            particles = true,
                            postProcessing = true,
                            bloom = true,
                            ambientOcclusion = true,
                            antiAliasing = "TAA",
                            textureQuality = "high",
                            modelDetail = "high",
                            drawDistance = 1000,
                            weatherEffects = true,
                            reflection = true
                        },
                        performance = new Performance
                        {
                            targetFPS = 60,
                            lowSpecMode = false,
                            gpuOptimization = true,
                            multiThreaded = true,
                            useGPUParticles = true
                        },
                        debug = new Debug
                        {
                            enabled = false,
                            showFPS = false,
                            showStats = false,
                            logLevel = "info",
                            validateShaders = false
                        }
                    },
                    security = new Security
                    {
                        fileIntegrityCheck = true,
                        configVersion = 2,
                        configChecksum = "",
                        executableOnly = true,
                        disableBatchExecution = true,
                        signatureVerification = true,
                        preventScriptExecution = true
                    }
                };
                
                SaveGraphicsConfig();
                
                Console.ForegroundColor = SUCCESS_COLOR;
                Console.WriteLine("\nConfiguração restaurada para os valores padrão!");
                Console.ResetColor();
                Console.WriteLine("\nPressione qualquer tecla para continuar...");
                Console.ReadKey();
            }
        }
        
        /// <summary>
        /// Aplica configurações de qualidade predefinidas
        /// </summary>
        private void ApplyPresetQuality(string preset)
        {
            switch (preset.ToLower())
            {
                case "ultra":
                    _config.graphics.effects.shadows = true;
                    _config.graphics.effects.particles = true;
                    _config.graphics.effects.postProcessing = true;
                    _config.graphics.effects.bloom = true;
                    _config.graphics.effects.ambientOcclusion = true;
                    _config.graphics.effects.antiAliasing = "TAA";
                    _config.graphics.effects.textureQuality = "ultra";
                    _config.graphics.effects.modelDetail = "ultra";
                    _config.graphics.effects.drawDistance = 2000;
                    _config.graphics.effects.weatherEffects = true;
                    _config.graphics.effects.reflection = true;
                    _config.graphics.performance.gpuOptimization = true;
                    _config.graphics.performance.useGPUParticles = true;
                    break;
                    
                case "high":
                    _config.graphics.effects.shadows = true;
                    _config.graphics.effects.particles = true;
                    _config.graphics.effects.postProcessing = true;
                    _config.graphics.effects.bloom = true;
                    _config.graphics.effects.ambientOcclusion = true;
                    _config.graphics.effects.antiAliasing = "TAA";
                    _config.graphics.effects.textureQuality = "high";
                    _config.graphics.effects.modelDetail = "high";
                    _config.graphics.effects.drawDistance = 1000;
                    _config.graphics.effects.weatherEffects = true;
                    _config.graphics.effects.reflection = true;
                    _config.graphics.performance.gpuOptimization = true;
                    _config.graphics.performance.useGPUParticles = true;
                    break;
                    
                case "medium":
                    _config.graphics.effects.shadows = true;
                    _config.graphics.effects.particles = true;
                    _config.graphics.effects.postProcessing = true;
                    _config.graphics.effects.bloom = false;
                    _config.graphics.effects.ambientOcclusion = false;
                    _config.graphics.effects.antiAliasing = "FXAA";
                    _config.graphics.effects.textureQuality = "medium";
                    _config.graphics.effects.modelDetail = "medium";
                    _config.graphics.effects.drawDistance = 500;
                    _config.graphics.effects.weatherEffects = true;
                    _config.graphics.effects.reflection = false;
                    _config.graphics.performance.gpuOptimization = true;
                    _config.graphics.performance.useGPUParticles = false;
                    break;
                    
                case "low":
                    _config.graphics.effects.shadows = false;
                    _config.graphics.effects.particles = false;
                    _config.graphics.effects.postProcessing = false;
                    _config.graphics.effects.bloom = false;
                    _config.graphics.effects.ambientOcclusion = false;
                    _config.graphics.effects.antiAliasing = "none";
                    _config.graphics.effects.textureQuality = "low";
                    _config.graphics.effects.modelDetail = "low";
                    _config.graphics.effects.drawDistance = 250;
                    _config.graphics.effects.weatherEffects = false;
                    _config.graphics.effects.reflection = false;
                    _config.graphics.performance.gpuOptimization = true;
                    _config.graphics.performance.useGPUParticles = false;
                    _config.graphics.performance.lowSpecMode = true;
                    break;
                    
                case "auto":
                    // Detectar hardware e aplicar configurações adequadas
                    DetectHardwareAndApplySettings();
                    break;
            }
        }
        
        /// <summary>
        /// Detecta hardware e aplica configurações adequadas
        /// </summary>
        private void DetectHardwareAndApplySettings()
        {
            // TODO: Implementar detecção de hardware
            // Por enquanto, aplicando configurações de alta qualidade como fallback
            ApplyPresetQuality("high");
        }
        
        /// <summary>
        /// Garante que os diretórios de configuração existam
        /// </summary>
        private void EnsureConfigDirectories()
        {
            Directory.CreateDirectory(Path.GetDirectoryName(CONFIG_PATH));
            Directory.CreateDirectory(Path.GetDirectoryName(SECURITY_PATH));
        }
        
        /// <summary>
        /// Carrega as configurações gráficas
        /// </summary>
        private void LoadGraphicsConfig()
        {
            try
            {
                if (File.Exists(CONFIG_PATH))
                {
                    string json = File.ReadAllText(CONFIG_PATH);
                    _config = JsonSerializer.Deserialize<GraphicsConfig>(json);
                    
                    if (_config == null)
                    {
                        throw new InvalidOperationException("Falha ao carregar configuração gráfica");
                    }
                }
                else
                {
                    throw new FileNotFoundException($"Arquivo de configuração não encontrado: {CONFIG_PATH}");
                }
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ERROR_COLOR;
                Console.WriteLine($"Erro ao carregar configurações: {ex.Message}");
                Console.WriteLine("Utilizando configuração padrão");
                Console.ResetColor();
                
                // Criar configuração padrão
                _config = new GraphicsConfig
                {
                    version = "2.0.0",
                    lastUpdate = DateTime.UtcNow.ToString("o"),
                    signature = "",
                    graphics = new Graphics
                    {
                        renderer = "auto",
                        preferredRenderers = new[] { "dx11", "vulkan", "compatibility" },
                        quality = "high",
                        resolution = new Resolution(),
                        effects = new Effects(),
                        performance = new Performance(),
                        debug = new Debug()
                    },
                    security = new Security
                    {
                        fileIntegrityCheck = true,
                        configVersion = 2,
                        configChecksum = "",
                        executableOnly = true,
                        disableBatchExecution = true,
                        signatureVerification = true,
                        preventScriptExecution = true
                    },
                    executables = new Executables()
                };
                
                SaveGraphicsConfig();
            }
        }
        
        /// <summary>
        /// Carrega as configurações de segurança
        /// </summary>
        private void LoadSecurityConfig()
        {
            try
            {
                if (File.Exists(SECURITY_PATH))
                {
                    string json = File.ReadAllText(SECURITY_PATH);
                    _security = JsonSerializer.Deserialize<SecurityConfig>(json);
                }
                else
                {
                    throw new FileNotFoundException($"Arquivo de configuração de segurança não encontrado: {SECURITY_PATH}");
                }
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ERROR_COLOR;
                Console.WriteLine($"Erro ao carregar configurações de segurança: {ex.Message}");
                Console.WriteLine("Utilizando configuração de segurança padrão");
                Console.ResetColor();
                
                // Fallback para configurações de segurança padrão
                _security = new SecurityConfig
                {
                    lastUpdate = DateTime.UtcNow.ToString("o"),
                    security = new SecuritySettings
                    {
                        fileIntegrityCheck = true,
                        preventScriptExecution = true,
                        executableOnly = true,
                        signatureVerification = false
                    }
                };
                
                SaveSecurityConfig();
            }
        }
        
        /// <summary>
        /// Salva as configurações gráficas
        /// </summary>
        private void SaveGraphicsConfig()
        {
            try
            {
                var options = new JsonSerializerOptions
                {
                    WriteIndented = true
                };
                
                _config.lastUpdate = DateTime.UtcNow.ToString("o");
                string json = JsonSerializer.Serialize(_config, options);
                File.WriteAllText(CONFIG_PATH, json);
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ERROR_COLOR;
                Console.WriteLine($"Erro ao salvar configurações: {ex.Message}");
                Console.ResetColor();
            }
        }
        
        /// <summary>
        /// Salva as configurações de segurança
        /// </summary>
        private void SaveSecurityConfig()
        {
            try
            {
                var options = new JsonSerializerOptions
                {
                    WriteIndented = true
                };
                
                _security.lastUpdate = DateTime.UtcNow.ToString("o");
                string json = JsonSerializer.Serialize(_security, options);
                File.WriteAllText(SECURITY_PATH, json);
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ERROR_COLOR;
                Console.WriteLine($"Erro ao salvar configurações de segurança: {ex.Message}");
                Console.ResetColor();
            }
        }
    }
    
    /// <summary>
    /// Estrutura de dados para configuração gráfica
    /// </summary>
    public class GraphicsConfig
    {
        public string version { get; set; }
        public string lastUpdate { get; set; }
        public string signature { get; set; }
        public Graphics graphics { get; set; }
        public Security security { get; set; }
        public Executables executables { get; set; }
    }
    
    /// <summary>
    /// Estrutura de dados para configurações de segurança
    /// </summary>
    public class SecurityConfig
    {
        public string lastUpdate { get; set; }
        public SecuritySettings security { get; set; }
    }
    
    /// <summary>
    /// Configurações gráficas detalhadas
    /// </summary>
    public class Graphics
    {
        public string renderer { get; set; }
        public string[] preferredRenderers { get; set; }
        public string quality { get; set; }
        public Resolution resolution { get; set; } = new Resolution();
        public Effects effects { get; set; } = new Effects();
        public Performance performance { get; set; } = new Performance();
        public Debug debug { get; set; } = new Debug();
    }
    
    /// <summary>
    /// Resolução e configurações de tela
    /// </summary>
    public class Resolution
    {
        public int width { get; set; } = 1920;
        public int height { get; set; } = 1080;
        public bool fullscreen { get; set; } = true;
        public bool vsync { get; set; } = true;
    }
    
    /// <summary>
    /// Configurações de efeitos visuais
    /// </summary>
    public class Effects
    {
        public bool shadows { get; set; } = true;
        public bool particles { get; set; } = true;
        public bool postProcessing { get; set; } = true;
        public bool bloom { get; set; } = true;
        public bool ambientOcclusion { get; set; } = true;
        public string antiAliasing { get; set; } = "TAA";
        public string textureQuality { get; set; } = "high";
        public string modelDetail { get; set; } = "high";
        public int drawDistance { get; set; } = 1000;
        public bool weatherEffects { get; set; } = true;
        public bool reflection { get; set; } = true;
    }
    
    /// <summary>
    /// Configurações de desempenho
    /// </summary>
    public class Performance
    {
        public int targetFPS { get; set; } = 60;
        public bool lowSpecMode { get; set; } = false;
        public bool gpuOptimization { get; set; } = true;
        public bool multiThreaded { get; set; } = true;
        public bool useGPUParticles { get; set; } = true;
    }
    
    /// <summary>
    /// Configurações de depuração
    /// </summary>
    public class Debug
    {
        public bool enabled { get; set; } = false;
        public bool showFPS { get; set; } = false;
        public bool showStats { get; set; } = false;
        public string logLevel { get; set; } = "info";
        public bool validateShaders { get; set; } = false;
    }
    
    /// <summary>
    /// Configurações de segurança
    /// </summary>
    public class Security
    {
        public bool fileIntegrityCheck { get; set; } = true;
        public int configVersion { get; set; } = 1;
        public string configChecksum { get; set; } = "";
        public bool executableOnly { get; set; } = false;
        public bool disableBatchExecution { get; set; } = false;
        public bool signatureVerification { get; set; } = false;
        public bool preventScriptExecution { get; set; } = false;
    }
    
    /// <summary>
    /// Configurações específicas de segurança
    /// </summary>
    public class SecuritySettings
    {
        public bool fileIntegrityCheck { get; set; } = true;
        public bool executableOnly { get; set; } = true;
        public bool signatureVerification { get; set; } = false;
        public bool preventScriptExecution { get; set; } = true;
    }
    
    /// <summary>
    /// Configurações de executáveis
    /// </summary>
    public class Executables
    {
        public ExecutableConfig launcher { get; set; } = new ExecutableConfig();
        public ExecutableConfig client { get; set; } = new ExecutableConfig();
        public ExecutableConfig config { get; set; } = new ExecutableConfig();
    }
    
    /// <summary>
    /// Configuração para um executável específico
    /// </summary>
    public class ExecutableConfig
    {
        public string path { get; set; } = "";
        public Dictionary<string, string> parameters { get; set; } = new Dictionary<string, string>();
        public bool requiredSignature { get; set; } = true;
    }
}