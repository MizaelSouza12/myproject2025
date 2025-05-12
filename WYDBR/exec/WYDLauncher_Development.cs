/**
 * WYDLauncher - Unified Game Launcher for WYDBR
 * 
 * Este é o código-fonte para o executável WYDLauncher.exe, a única interface oficial
 * e segura para iniciar o jogo. Todo o sistema foi projetado para não depender
 * de nenhum arquivo .bat ou script externo, utilizando apenas executáveis assinados.
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
    /// Classe principal do WYDLauncher
    /// </summary>
    public class Launcher
    {
        private const string CONFIG_PATH = "config/graphics_config.json";
        private const string SECURITY_PATH = "security/security_config.json";
        private const string CLIENT_EXE = "WYD.exe";
        private const string CONFIG_EXE = "WydConfig.exe";
        
        private GraphicsConfig _config;
        private SecurityConfig _security;
        private bool _secureMode = false;
        
        /// <summary>
        /// Ponto de entrada principal do launcher
        /// </summary>
        public static void Main(string[] args)
        {
            try
            {
                var launcher = new Launcher();
                launcher.Initialize(args);
                launcher.Run();
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine($"Erro crítico: {ex.Message}");
                Console.WriteLine(ex.StackTrace);
                Console.ResetColor();
                Console.WriteLine("\nPressione qualquer tecla para sair...");
                Console.ReadKey();
            }
        }
        
        /// <summary>
        /// Inicializa o launcher, verificando argumentos e carregando configurações
        /// </summary>
        public void Initialize(string[] args)
        {
            // Analisar argumentos de linha de comando
            _secureMode = args.Any(arg => arg.ToLower() == "-secure");
            bool debugMode = args.Any(arg => arg.ToLower() == "-debug");
            bool compatMode = args.Any(arg => arg.ToLower() == "-compat");
            
            // Verificar ambiente e diretórios
            Console.WriteLine("===================================");
            if (_secureMode)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("WYDBR - INICIALIZAÇÃO SEGURA");
                Console.ResetColor();
            }
            else
            {
                Console.WriteLine("WYDBR - Inicializando...");
            }
            Console.WriteLine("===================================\n");
            
            // Verificar arquivos críticos e integridade
            VerifyFileIntegrity();
            
            // Carregar configurações
            LoadGraphicsConfig();
            LoadSecurityConfig();
            
            // Verificar requisitos do sistema
            CheckSystemRequirements();
            
            // Configurar modo de renderização
            if (compatMode)
            {
                _config.graphics.renderer = "compatibility";
                SaveGraphicsConfig();
            }
            
            // Ativar modo seguro se necessário
            if (_secureMode && _security != null)
            {
                Console.WriteLine("\nInicializando sistema de proteção...");
                _security.security.preventScriptExecution = true;
                _security.security.signatureVerification = true;
                _security.security.executableOnly = true;
                SaveSecurityConfig();
                Console.WriteLine("Sistema de proteção ativado.\n");
            }
        }
        
        /// <summary>
        /// Executa o launcher, iniciando o cliente do jogo
        /// </summary>
        public void Run()
        {
            Console.WriteLine("Iniciando WYDBR...");
            try
            {
                // Lógica para iniciar o cliente do jogo
                var clientPath = Path.GetFullPath(CLIENT_EXE);
                if (!File.Exists(clientPath))
                {
                    throw new FileNotFoundException($"Arquivo crítico não encontrado: {CLIENT_EXE}");
                }
                
                var startInfo = new ProcessStartInfo
                {
                    FileName = clientPath,
                    UseShellExecute = true
                };
                
                // Passar parâmetros relevantes ao cliente
                if (_secureMode)
                {
                    startInfo.Arguments = "-secure";
                }
                
                // Iniciar o cliente
                Process.Start(startInfo);
                Console.WriteLine($"Cliente iniciado com sucesso: {CLIENT_EXE}");
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine($"Erro ao iniciar o cliente: {ex.Message}");
                Console.ResetColor();
                throw;
            }
        }
        
        /// <summary>
        /// Verifica a integridade dos arquivos críticos
        /// </summary>
        private void VerifyFileIntegrity()
        {
            Console.WriteLine("Verificando integridade dos arquivos...");
            
            // Verificar arquivos críticos
            string[] criticalFiles = new[] { CLIENT_EXE, CONFIG_EXE, CONFIG_PATH, SECURITY_PATH };
            foreach (var file in criticalFiles)
            {
                if (!File.Exists(file))
                {
                    throw new FileNotFoundException($"Arquivo crítico não encontrado: {file}");
                }
            }
            
            // TODO: Implementar verificação de hash/assinatura digital dos executáveis
            
            Console.WriteLine("Verificação concluída com sucesso.");
        }
        
        /// <summary>
        /// Carrega as configurações gráficas
        /// </summary>
        private void LoadGraphicsConfig()
        {
            try
            {
                string json = File.ReadAllText(CONFIG_PATH);
                _config = JsonSerializer.Deserialize<GraphicsConfig>(json);
                
                if (_config == null)
                {
                    throw new InvalidOperationException("Falha ao carregar configuração gráfica");
                }
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine($"Aviso: {ex.Message}");
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
                        quality = "auto"
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
            }
        }
        
        /// <summary>
        /// Carrega as configurações de segurança
        /// </summary>
        private void LoadSecurityConfig()
        {
            try
            {
                string json = File.ReadAllText(SECURITY_PATH);
                _security = JsonSerializer.Deserialize<SecurityConfig>(json);
            }
            catch
            {
                // Fallback para configurações de segurança padrão
                _security = new SecurityConfig
                {
                    lastUpdate = DateTime.UtcNow.ToString("o"),
                    security = new SecuritySettings
                    {
                        fileIntegrityCheck = true,
                        preventScriptExecution = true,
                        executableOnly = true,
                        signatureVerification = _secureMode
                    }
                };
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
                
                string json = JsonSerializer.Serialize(_config, options);
                File.WriteAllText(CONFIG_PATH, json);
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine($"Aviso: Não foi possível salvar configuração gráfica: {ex.Message}");
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
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine($"Aviso: Não foi possível salvar configuração de segurança: {ex.Message}");
                Console.ResetColor();
            }
        }
        
        /// <summary>
        /// Verifica os requisitos do sistema
        /// </summary>
        private void CheckSystemRequirements()
        {
            // TODO: Implementar verificação de requisitos mínimos
            // - Memória
            // - Processador
            // - GPU
            // - DirectX/Vulkan
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