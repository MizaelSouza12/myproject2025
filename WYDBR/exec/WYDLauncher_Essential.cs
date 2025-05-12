/**
 * WYDLauncher - Unified Game Launcher for WYDBR 2.0
 * 
 * Este é o código-fonte para o executável WYDLauncher.exe, responsável por
 * verificar requisitos essenciais do sistema e iniciar o jogo.
 * 
 * Copyright (c) 2025 WYDBR Team
 */

using System;
using System.IO;
using System.Diagnostics;
using System.Text.Json;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Net;
using Microsoft.Win32;

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
        
        // Requisitos mínimos
        private const int MIN_DISK_SPACE_GB = 5;
        
        /// <summary>
        /// Ponto de entrada principal do launcher
        /// </summary>
        public static void Main(string[] args)
        {
            try
            {
                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.WriteLine(@"
 __        ______ ______  ______  ______  ______  ______  _____ _____      _____  _____
 \ \      / / __ \|  _ \ \  _ \  \  __  \/ __ \ / ___ ||  __  ||  _ \
  \ \ /\ / /| |  | | | | || | | || |__| /| |__| |\___ \| |__| || |_| |
   \ V  V / | |__| | |_| || |_| || |  | || |___ |___) |  __  ||    /
    \_/\_/   \____/|____/ |____/ |_|  |_| \____/|____/|_|  |_||_|\_\
");
                Console.ResetColor();
                Console.WriteLine("                   Inicializando WYDBR 2.0\n");

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
            bool skipChecks = args.Any(arg => arg.ToLower() == "-skip-checks");
            
            // Verificar ambiente e diretórios
            Console.WriteLine("===================================");
            if (_secureMode)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("WYDBR 2.0 - INICIALIZAÇÃO SEGURA");
                Console.ResetColor();
            }
            else
            {
                Console.WriteLine("WYDBR 2.0 - Inicializando...");
            }
            Console.WriteLine("===================================\n");
            
            // Verificar requisitos do sistema
            if (!skipChecks)
            {
                Console.WriteLine("Verificando requisitos do sistema...");
                if (!CheckSystemRequirements())
                {
                    throw new SystemException("O programa será encerrado devido a requisitos não atendidos.");
                }
            }
            
            // Verificar arquivos críticos e integridade
            VerifyFileIntegrity();
            
            // Carregar configurações
            LoadGraphicsConfig();
            LoadSecurityConfig();
            
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
            Console.WriteLine("Iniciando WYDBR 2.0...");
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
        /// Verifica os requisitos do sistema
        /// </summary>
        private bool CheckSystemRequirements()
        {
            bool allRequirementsMet = true;
            var missingComponents = new List<(string Name, string Message, string DownloadUrl)>();
            
            // 1. .NET Framework/Runtime
            Console.Write("Verificando .NET Framework/Runtime... ");
            var dotNetResult = CheckDotNet();
            PrintResult(dotNetResult.Success);
            if (!dotNetResult.Success)
            {
                Console.WriteLine($"  → {dotNetResult.Message}");
                allRequirementsMet = false;
                missingComponents.Add((".NET Framework/Runtime", dotNetResult.Message, dotNetResult.DownloadUrl));
            }
            
            // 2. DirectX
            Console.Write("Verificando DirectX... ");
            var directXResult = CheckDirectX();
            PrintResult(directXResult.Success);
            if (!directXResult.Success)
            {
                Console.WriteLine($"  → {directXResult.Message}");
                allRequirementsMet = false;
                missingComponents.Add(("DirectX", directXResult.Message, directXResult.DownloadUrl));
            }
            
            // 3. Visual C++ Redistributable
            Console.Write("Verificando Visual C++ Redistributable... ");
            var vcRedistResult = CheckVCRedist();
            PrintResult(vcRedistResult.Success);
            if (!vcRedistResult.Success)
            {
                Console.WriteLine($"  → {vcRedistResult.Message}");
                allRequirementsMet = false;
                missingComponents.Add(("Visual C++ Redistributable", vcRedistResult.Message, vcRedistResult.DownloadUrl));
            }
            
            // 4. Drivers de GPU
            Console.Write("Verificando drivers de GPU... ");
            var gpuDriversResult = CheckGPUDrivers();
            PrintResult(gpuDriversResult.Success);
            if (!gpuDriversResult.Success)
            {
                Console.WriteLine($"  → {gpuDriversResult.Message}");
                allRequirementsMet = false;
                missingComponents.Add(("Drivers de GPU", gpuDriversResult.Message, gpuDriversResult.DownloadUrl));
            }
            
            // 5. Espaço em disco
            Console.Write("Verificando espaço em disco... ");
            var diskSpaceResult = CheckDiskSpace();
            PrintResult(diskSpaceResult.Success);
            if (!diskSpaceResult.Success)
            {
                Console.WriteLine($"  → {diskSpaceResult.Message}");
                allRequirementsMet = false;
                missingComponents.Add(("Espaço em disco", diskSpaceResult.Message, diskSpaceResult.DownloadUrl));
            }
            
            // Se houver componentes ausentes, oferecer instalação
            if (missingComponents.Count > 0)
            {
                Console.WriteLine("\n== Requisitos em falta ==");
                foreach (var comp in missingComponents)
                {
                    Console.WriteLine($"- {comp.Name}: {comp.Message}");
                }
                
                // Filtrar componentes que podem ser instalados
                var installableItems = missingComponents.Where(c => !string.IsNullOrEmpty(c.DownloadUrl)).ToList();
                if (installableItems.Count > 0)
                {
                    if (PromptInstallComponents())
                    {
                        InstallMissingComponents(installableItems);
                        
                        // Verificar novamente
                        Console.WriteLine("\nVerificando novamente os requisitos após instalação...");
                        return CheckSystemRequirements();
                    }
                }
                
                // Perguntar se deseja continuar mesmo com requisitos faltando
                if (PromptContinueAnyway())
                {
                    Console.ForegroundColor = ConsoleColor.Yellow;
                    Console.WriteLine("Continuando mesmo com requisitos em falta. O jogo pode não funcionar corretamente.");
                    Console.ResetColor();
                    return true;
                }
            }
            
            return allRequirementsMet;
        }
        
        /// <summary>
        /// Formata o resultado da verificação
        /// </summary>
        private void PrintResult(bool success)
        {
            if (success)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("OK");
                Console.ResetColor();
            }
            else
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("FALHA");
                Console.ResetColor();
            }
        }
        
        /// <summary>
        /// Verifica o .NET Framework/Runtime
        /// </summary>
        private (bool Success, string Message, string DownloadUrl) CheckDotNet()
        {
            try
            {
                // Verificar se estamos rodando em .NET Framework
                string frameworkDescription = RuntimeInformation.FrameworkDescription;
                
                if (frameworkDescription.Contains(".NET Framework"))
                {
                    // Extrair a versão do .NET Framework
                    string versionStr = frameworkDescription.Replace(".NET Framework ", "");
                    if (Version.TryParse(versionStr, out Version version))
                    {
                        if (version.Major >= 4 && version.Minor >= 8)
                        {
                            return (true, "", null);
                        }
                        else
                        {
                            return (false, $"Encontrado .NET Framework {version}, mas é necessário 4.8 ou superior", 
                                  "https://dotnet.microsoft.com/download/dotnet-framework/net48");
                        }
                    }
                }
                else if (frameworkDescription.Contains(".NET"))
                {
                    // Extrair a versão do .NET Core/.NET
                    string versionStr = frameworkDescription.Replace(".NET ", "");
                    if (Version.TryParse(versionStr, out Version version))
                    {
                        if (version.Major >= 6)
                        {
                            return (true, "", null);
                        }
                        else
                        {
                            return (false, $"Encontrado .NET {version}, mas é necessário 6.0 ou superior", 
                                  "https://dotnet.microsoft.com/download/dotnet/6.0");
                        }
                    }
                }
                
                // Verificar no registro em Windows
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    try
                    {
                        using (RegistryKey ndpKey = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full\"))
                        {
                            if (ndpKey != null)
                            {
                                int releaseKey = Convert.ToInt32(ndpKey.GetValue("Release"));
                                if (releaseKey >= 528040) // .NET Framework 4.8
                                {
                                    return (true, "", null);
                                }
                            }
                        }
                    }
                    catch
                    {
                        // Ignorar erros de registro
                    }
                }
                
                return (false, ".NET Framework 4.8 ou .NET 6.0+ não encontrado", 
                       "https://dotnet.microsoft.com/download/dotnet/6.0");
            }
            catch
            {
                return (false, "Erro ao verificar .NET Framework/Runtime", 
                       "https://dotnet.microsoft.com/download/dotnet/6.0");
            }
        }
        
        /// <summary>
        /// Verifica o DirectX
        /// </summary>
        private (bool Success, string Message, string DownloadUrl) CheckDirectX()
        {
            try
            {
                // Verificar a presença de DLLs essenciais do DirectX
                string systemDir = Environment.GetFolderPath(Environment.SpecialFolder.System);
                string[] dxFiles = { "d3d11.dll", "dxgi.dll" };
                
                List<string> missingFiles = new List<string>();
                foreach (string file in dxFiles)
                {
                    if (!File.Exists(Path.Combine(systemDir, file)))
                    {
                        missingFiles.Add(file);
                    }
                }
                
                if (missingFiles.Count == 0)
                {
                    return (true, "", null);
                }
                else
                {
                    return (false, $"Arquivos DirectX ausentes: {string.Join(", ", missingFiles)}",
                           "https://www.microsoft.com/download/details.aspx?id=35");
                }
            }
            catch
            {
                return (false, "Erro ao verificar DirectX",
                       "https://www.microsoft.com/download/details.aspx?id=35");
            }
        }
        
        /// <summary>
        /// Verifica o Visual C++ Redistributable
        /// </summary>
        private (bool Success, string Message, string DownloadUrl) CheckVCRedist()
        {
            try
            {
                // Verificar a presença de DLLs do Visual C++ Redistributable
                string systemDir = Environment.GetFolderPath(Environment.SpecialFolder.System);
                if (File.Exists(Path.Combine(systemDir, "vcruntime140.dll")))
                {
                    return (true, "", null);
                }
                
                // Verificar no registro
                string[] registryKeys = {
                    @"SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64",
                    @"SOFTWARE\WOW6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x64"
                };
                
                foreach (string registryKey in registryKeys)
                {
                    try
                    {
                        using (RegistryKey key = Registry.LocalMachine.OpenSubKey(registryKey))
                        {
                            if (key != null && key.GetValue("Installed") != null)
                            {
                                return (true, "", null);
                            }
                        }
                    }
                    catch
                    {
                        // Ignorar erros de registro
                    }
                }
                
                return (false, "Visual C++ Redistributable 2015-2022 não encontrado",
                       "https://aka.ms/vs/17/release/vc_redist.x64.exe");
            }
            catch
            {
                return (false, "Erro ao verificar Visual C++ Redistributable",
                       "https://aka.ms/vs/17/release/vc_redist.x64.exe");
            }
        }
        
        /// <summary>
        /// Verifica os drivers de GPU
        /// </summary>
        private (bool Success, string Message, string DownloadUrl) CheckGPUDrivers()
        {
            try
            {
                // Em um ambiente real, verificaríamos a data do driver
                // e compararíamos com um mínimo aceitável
                
                // Simplificação: verificar se há uma GPU reconhecida
                string gpuName = "GPU não identificada";
                string gpuDriverVersion = "Desconhecido";
                DateTime driverDate = DateTime.MinValue;
                
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    try
                    {
                        // Usar WMI para obter informações da GPU
                        var searcher = new System.Management.ManagementObjectSearcher("SELECT * FROM Win32_VideoController");
                        foreach (System.Management.ManagementObject obj in searcher.Get())
                        {
                            gpuName = obj["Name"]?.ToString() ?? "GPU não identificada";
                            gpuDriverVersion = obj["DriverVersion"]?.ToString() ?? "Desconhecido";
                            
                            // Verificar data do driver se disponível
                            if (obj["DriverDate"] != null)
                            {
                                string driverDateStr = obj["DriverDate"].ToString();
                                // Parsing do formato WMI para DateTime
                                if (driverDateStr.Length > 8)
                                {
                                    int year = int.Parse(driverDateStr.Substring(0, 4));
                                    int month = int.Parse(driverDateStr.Substring(4, 2));
                                    int day = int.Parse(driverDateStr.Substring(6, 2));
                                    driverDate = new DateTime(year, month, day);
                                }
                            }
                            
                            break; // Considerar apenas a primeira GPU
                        }
                    }
                    catch
                    {
                        // Fallback se WMI falhar
                    }
                }
                
                // Verificar se temos informações válidas
                if (gpuName != "GPU não identificada")
                {
                    // Verificar idade do driver - assumimos que drivers com mais
                    // de 3 anos de idade podem estar desatualizados
                    if (driverDate != DateTime.MinValue && (DateTime.Now - driverDate).TotalDays > 365 * 3)
                    {
                        return (false, $"Driver da GPU ({gpuName}) está desatualizado ({driverDate.ToShortDateString()})",
                               "https://www.nvidia.com/Download/index.aspx"); // URL genérica para Nvidia
                    }
                    
                    return (true, "", null);
                }
                else
                {
                    // Não conseguimos identificar a GPU, o que é um pouco preocupante
                    return (false, "Não foi possível identificar a GPU",
                           "https://www.microsoft.com/en-us/software-download/windows10"); // Link para Windows Update
                }
            }
            catch
            {
                return (false, "Erro ao verificar drivers de GPU", null);
            }
        }
        
        /// <summary>
        /// Verifica o espaço em disco
        /// </summary>
        private (bool Success, string Message, string DownloadUrl) CheckDiskSpace()
        {
            try
            {
                // Obter o diretório atual
                string currentDir = Directory.GetCurrentDirectory();
                string driveLetter = Path.GetPathRoot(currentDir);
                
                // Obter informações do disco
                DriveInfo drive = new DriveInfo(driveLetter);
                double freeSpaceGB = drive.AvailableFreeSpace / (1024.0 * 1024.0 * 1024.0);
                
                if (freeSpaceGB >= MIN_DISK_SPACE_GB)
                {
                    return (true, "", null);
                }
                else
                {
                    return (false, $"Apenas {freeSpaceGB:F2} GB livres. Mínimo necessário: {MIN_DISK_SPACE_GB} GB", null);
                }
            }
            catch
            {
                return (false, "Erro ao verificar espaço em disco", null);
            }
        }
        
        /// <summary>
        /// Pergunta ao usuário se deseja instalar os componentes em falta
        /// </summary>
        private bool PromptInstallComponents()
        {
            Console.WriteLine("\nDeseja baixar e instalar os componentes ausentes?");
            Console.WriteLine("1. Sim, baixar e instalar automaticamente");
            Console.WriteLine("2. Não, eu os instalarei manualmente mais tarde");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            var key = Console.ReadKey(false).KeyChar;
            Console.WriteLine();
            
            return key == '1';
        }
        
        /// <summary>
        /// Pergunta ao usuário se deseja continuar mesmo com requisitos em falta
        /// </summary>
        private bool PromptContinueAnyway()
        {
            Console.WriteLine("\nAlguns requisitos não foram atendidos. Continuar mesmo assim?");
            Console.WriteLine("1. Sim, continuar mesmo assim (pode não funcionar corretamente)");
            Console.WriteLine("2. Não, sair e resolver os problemas");
            
            Console.Write("\nEscolha uma opção (1-2): ");
            var key = Console.ReadKey(false).KeyChar;
            Console.WriteLine();
            
            return key == '1';
        }
        
        /// <summary>
        /// Instala os componentes em falta
        /// </summary>
        private void InstallMissingComponents(List<(string Name, string Message, string DownloadUrl)> components)
        {
            foreach (var component in components)
            {
                Console.WriteLine($"\nPreparando instalação de: {component.Name}");
                DownloadAndInstallComponent(component.Name, component.DownloadUrl);
            }
            
            Console.WriteLine("\nInstalações concluídas. Verificando requisitos novamente...");
        }
        
        /// <summary>
        /// Faz o download e instala um componente
        /// </summary>
        private void DownloadAndInstallComponent(string componentName, string downloadUrl)
        {
            try
            {
                // Nome do arquivo temporário para o instalador
                string fileName = Path.GetFileName(downloadUrl) ?? $"{componentName.Replace(" ", "")}_setup.exe";
                string installerPath = Path.Combine(Path.GetTempPath(), fileName);
                
                Console.WriteLine($"Baixando de: {downloadUrl}");
                Console.WriteLine($"Salvando em: {installerPath}");
                
                // Fazer o download
                using (WebClient client = new WebClient())
                {
                    // Mostrar progresso
                    client.DownloadProgressChanged += (sender, e) =>
                    {
                        Console.SetCursorPosition(0, Console.CursorTop);
                        Console.Write($"Progresso: {e.ProgressPercentage}% ({e.BytesReceived / 1024 / 1024} MB / {e.TotalBytesToReceive / 1024 / 1024} MB)    ");
                    };
                    
                    // Quando concluído, executar o instalador
                    client.DownloadFileCompleted += (sender, e) =>
                    {
                        if (e.Error != null)
                        {
                            Console.ForegroundColor = ConsoleColor.Red;
                            Console.WriteLine($"\nErro ao baixar: {e.Error.Message}");
                            Console.ResetColor();
                        }
                        else if (e.Cancelled)
                        {
                            Console.WriteLine("\nDownload cancelado.");
                        }
                        else
                        {
                            Console.WriteLine("\nDownload concluído. Iniciando instalação...");
                            
                            try
                            {
                                // Iniciar o instalador
                                var process = new Process
                                {
                                    StartInfo = new ProcessStartInfo
                                    {
                                        FileName = installerPath,
                                        UseShellExecute = true,
                                        Arguments = "/passive /norestart"
                                    }
                                };
                                
                                process.Start();
                                Console.WriteLine($"Instalador de {componentName} iniciado...");
                                
                                // Aguardar concluir
                                process.WaitForExit();
                                int exitCode = process.ExitCode;
                                
                                if (exitCode == 0 || exitCode == 3010) // 3010 = success, restart required
                                {
                                    Console.ForegroundColor = ConsoleColor.Green;
                                    Console.WriteLine($"Instalação de {componentName} concluída com sucesso.");
                                    Console.ResetColor();
                                }
                                else
                                {
                                    Console.ForegroundColor = ConsoleColor.Yellow;
                                    Console.WriteLine($"Instalação de {componentName} concluída com código: {exitCode}");
                                    Console.ResetColor();
                                }
                            }
                            catch (Exception ex)
                            {
                                Console.ForegroundColor = ConsoleColor.Red;
                                Console.WriteLine($"Erro ao iniciar instalador: {ex.Message}");
                                Console.ResetColor();
                            }
                        }
                    };
                    
                    // Iniciar o download
                    client.DownloadFileAsync(new Uri(downloadUrl), installerPath);
                    
                    // Aguardar concluir
                    while (client.IsBusy)
                    {
                        System.Threading.Thread.Sleep(100);
                    }
                }
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine($"Erro no processo de download/instalação: {ex.Message}");
                Console.ResetColor();
                
                Console.WriteLine($"\nPor favor, visite manualmente: {downloadUrl}");
                Console.WriteLine("Instale o componente e tente novamente.");
            }
        }
        
        /// <summary>
        /// Verifica a integridade dos arquivos críticos
        /// </summary>
        private void VerifyFileIntegrity()
        {
            Console.WriteLine("Verificando integridade dos arquivos...");
            
            // Verificar arquivos críticos
            string[] criticalFiles = new[] { CLIENT_EXE, CONFIG_EXE };
            foreach (var file in criticalFiles)
            {
                if (!File.Exists(file))
                {
                    throw new FileNotFoundException($"Arquivo crítico não encontrado: {file}");
                }
            }
            
            Console.WriteLine("Verificação de arquivos concluída.");
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
                }
                else
                {
                    CreateDefaultConfig();
                }
            }
            catch
            {
                CreateDefaultConfig();
            }
        }
        
        /// <summary>
        /// Cria configurações padrão
        /// </summary>
        private void CreateDefaultConfig()
        {
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
            
            SaveGraphicsConfig();
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
                    CreateDefaultSecurity();
                }
            }
            catch
            {
                CreateDefaultSecurity();
            }
        }
        
        /// <summary>
        /// Cria configurações de segurança padrão
        /// </summary>
        private void CreateDefaultSecurity()
        {
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
            
            SaveSecurityConfig();
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