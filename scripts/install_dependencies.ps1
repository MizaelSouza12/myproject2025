# WYDBR2.0 Installation Script
# Requer execução como Administrador
#Requires -RunAsAdministrator

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    RequiredModules = @(
        "SqlServer",
        "PSWindowsUpdate",
        "7Zip4PowerShell"
    )
    Dependencies = @{
        CMake = "3.20.0"
        MySQL = "8.0"
        PowerShell = "7.0"
        Git = "2.30.0"
    }
}

# Função de Log
function Write-Log {
    param(
        [string]$Message,
        [string]$Level = "INFO"
    )
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    Write-Host $logMessage
    
    # Criar diretório de logs se não existir
    $logDir = Join-Path $config.ProjectRoot "logs"
    if (-not (Test-Path $logDir)) {
        New-Item -ItemType Directory -Path $logDir -Force | Out-Null
    }
    
    Add-Content -Path "$logDir\installation.log" -Value $logMessage
}

# Verificar execução como administrador
Write-Log "Verificando privilégios de administrador..."
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Log "Este script precisa ser executado como Administrador!" "ERROR"
    exit 1
}

# Criar estrutura de diretórios
Write-Log "Criando estrutura de diretórios..."
$directories = @(
    "logs",
    "data",
    "config",
    "backup",
    "temp",
    "scripts"
)

foreach ($dir in $directories) {
    $path = Join-Path $config.ProjectRoot $dir
    if (-not (Test-Path $path)) {
        New-Item -ItemType Directory -Path $path -Force | Out-Null
        Write-Log "Diretório criado: $path"
    }
}

# Instalar módulos PowerShell necessários
Write-Log "Instalando módulos PowerShell necessários..."
foreach ($module in $config.RequiredModules) {
    if (-not (Get-Module -ListAvailable -Name $module)) {
        try {
            Install-Module -Name $module -Force -AllowClobber -Scope AllUsers
            Write-Log "Módulo instalado: $module"
        }
        catch {
            Write-Log "Erro ao instalar módulo $module : $_" "ERROR"
        }
    }
    else {
        Write-Log "Módulo já instalado: $module"
    }
}

# Verificar/Instalar Chocolatey
Write-Log "Verificando instalação do Chocolatey..."
if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
    try {
        Set-ExecutionPolicy Bypass -Scope Process -Force
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
        Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
        Write-Log "Chocolatey instalado com sucesso"
    }
    catch {
        Write-Log "Erro ao instalar Chocolatey: $_" "ERROR"
        exit 1
    }
}

# Instalar dependências via Chocolatey
Write-Log "Instalando dependências via Chocolatey..."
$dependencies = @(
    "cmake",
    "mysql",
    "git",
    "visualstudio2019buildtools",
    "python3"
)

foreach ($dep in $dependencies) {
    try {
        choco install $dep -y
        Write-Log "Dependência instalada: $dep"
    }
    catch {
        Write-Log "Erro ao instalar $dep : $_" "ERROR"
    }
}

# Configurar variáveis de ambiente
Write-Log "Configurando variáveis de ambiente..."
$envPaths = @(
    "C:\Program Files\CMake\bin",
    "C:\Program Files\MySQL\MySQL Server 8.0\bin",
    "C:\Program Files\Git\bin"
)

foreach ($path in $envPaths) {
    if (Test-Path $path) {
        $currentPath = [Environment]::GetEnvironmentVariable("Path", "Machine")
        if ($currentPath -notlike "*$path*") {
            [Environment]::SetEnvironmentVariable("Path", "$currentPath;$path", "Machine")
            Write-Log "Adicionado ao PATH: $path"
        }
    }
}

# Verificar instalações
Write-Log "Verificando instalações..."
$checkCommands = @{
    "cmake --version" = "CMake"
    "mysql --version" = "MySQL"
    "git --version" = "Git"
    "python --version" = "Python"
}

foreach ($cmd in $checkCommands.Keys) {
    try {
        $result = Invoke-Expression $cmd
        Write-Log "$($checkCommands[$cmd]) instalado: $result"
    }
    catch {
        Write-Log "Erro ao verificar $($checkCommands[$cmd]): $_" "ERROR"
    }
}

Write-Log "Instalação de dependências concluída!" 