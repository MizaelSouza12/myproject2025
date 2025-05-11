# WYDBR2.0 Dependencies Installation Script
# Autor: Claude
# Data: 2024

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    MySQLVersion = "8.0.33"
    CMakeVersion = "3.20.0"
    PowerShellVersion = "7.0.0"
    GitVersion = "2.40.0"
    MinGWVersion = "8.1.0"
}

# Funções de Log
function Write-Log {
    param(
        [string]$Message,
        [string]$Level = "INFO"
    )
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    Write-Host $logMessage
    Add-Content -Path "$($config.ProjectRoot)\logs\install.log" -Value $logMessage
}

# Função para verificar se um comando existe
function Test-Command {
    param(
        [string]$Command
    )
    return [bool](Get-Command -Name $Command -ErrorAction SilentlyContinue)
}

# Função para instalar Chocolatey
function Install-Chocolatey {
    Write-Log "Instalando Chocolatey..."
    
    try {
        if (-not (Test-Command choco)) {
            Set-ExecutionPolicy Bypass -Scope Process -Force
            [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
            Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
            Write-Log "Chocolatey instalado com sucesso."
        } else {
            Write-Log "Chocolatey já está instalado."
        }
        return $true
    }
    catch {
        Write-Log "Erro ao instalar Chocolatey: $_" "ERROR"
        return $false
    }
}

# Função para instalar MySQL
function Install-MySQL {
    Write-Log "Instalando MySQL..."
    
    try {
        if (-not (Test-Command mysql)) {
            choco install mysql --version $config.MySQLVersion -y
            Write-Log "MySQL instalado com sucesso."
        } else {
            Write-Log "MySQL já está instalado."
        }
        return $true
    }
    catch {
        Write-Log "Erro ao instalar MySQL: $_" "ERROR"
        return $false
    }
}

# Função para instalar CMake
function Install-CMake {
    Write-Log "Instalando CMake..."
    
    try {
        if (-not (Test-Command cmake)) {
            choco install cmake --version $config.CMakeVersion -y
            Write-Log "CMake instalado com sucesso."
        } else {
            Write-Log "CMake já está instalado."
        }
        return $true
    }
    catch {
        Write-Log "Erro ao instalar CMake: $_" "ERROR"
        return $false
    }
}

# Função para instalar PowerShell
function Install-PowerShell {
    Write-Log "Instalando PowerShell..."
    
    try {
        if ($PSVersionTable.PSVersion.Major -lt 7) {
            choco install powershell-core --version $config.PowerShellVersion -y
            Write-Log "PowerShell instalado com sucesso."
        } else {
            Write-Log "PowerShell já está instalado."
        }
        return $true
    }
    catch {
        Write-Log "Erro ao instalar PowerShell: $_" "ERROR"
        return $false
    }
}

# Função para instalar Git
function Install-Git {
    Write-Log "Instalando Git..."
    
    try {
        if (-not (Test-Command git)) {
            choco install git --version $config.GitVersion -y
            Write-Log "Git instalado com sucesso."
        } else {
            Write-Log "Git já está instalado."
        }
        return $true
    }
    catch {
        Write-Log "Erro ao instalar Git: $_" "ERROR"
        return $false
    }
}

# Função para instalar MinGW
function Install-MinGW {
    Write-Log "Instalando MinGW..."
    
    try {
        if (-not (Test-Command g++)) {
            choco install mingw --version $config.MinGWVersion -y
            Write-Log "MinGW instalado com sucesso."
        } else {
            Write-Log "MinGW já está instalado."
        }
        return $true
    }
    catch {
        Write-Log "Erro ao instalar MinGW: $_" "ERROR"
        return $false
    }
}

# Função para configurar variáveis de ambiente
function Set-EnvironmentVariables {
    Write-Log "Configurando variáveis de ambiente..."
    
    try {
        # Adicionar diretórios ao PATH
        $paths = @(
            "C:\Program Files\MySQL\MySQL Server $($config.MySQLVersion.Split('.')[0])\bin",
            "C:\Program Files\CMake\bin",
            "C:\Program Files\Git\cmd",
            "C:\MinGW\bin"
        )
        
        foreach ($path in $paths) {
            if (Test-Path $path) {
                $currentPath = [Environment]::GetEnvironmentVariable("Path", "Machine")
                if (-not $currentPath.Contains($path)) {
                    [Environment]::SetEnvironmentVariable("Path", "$currentPath;$path", "Machine")
                    Write-Log "Adicionado ao PATH: $path"
                }
            }
        }
        
        Write-Log "Variáveis de ambiente configuradas com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao configurar variáveis de ambiente: $_" "ERROR"
        return $false
    }
}

# Função para criar diretórios necessários
function New-RequiredDirectories {
    Write-Log "Criando diretórios necessários..."
    
    try {
        $directories = @(
            "$($config.ProjectRoot)\logs",
            "$($config.ProjectRoot)\config",
            "$($config.ProjectRoot)\database",
            "$($config.ProjectRoot)\build"
        )
        
        foreach ($dir in $directories) {
            if (-not (Test-Path $dir)) {
                New-Item -ItemType Directory -Path $dir
                Write-Log "Diretório criado: $dir"
            }
        }
        
        Write-Log "Diretórios criados com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao criar diretórios: $_" "ERROR"
        return $false
    }
}

# Função principal
function Start-Installation {
    Write-Log "Iniciando instalação de dependências..."
    
    # Instalar Chocolatey
    if (-not (Install-Chocolatey)) {
        Write-Log "Falha ao instalar Chocolatey. Abortando instalação." "ERROR"
        return $false
    }
    
    # Instalar dependências
    if (-not (Install-MySQL)) {
        Write-Log "Falha ao instalar MySQL." "ERROR"
    }
    
    if (-not (Install-CMake)) {
        Write-Log "Falha ao instalar CMake." "ERROR"
    }
    
    if (-not (Install-PowerShell)) {
        Write-Log "Falha ao instalar PowerShell." "ERROR"
    }
    
    if (-not (Install-Git)) {
        Write-Log "Falha ao instalar Git." "ERROR"
    }
    
    if (-not (Install-MinGW)) {
        Write-Log "Falha ao instalar MinGW." "ERROR"
    }
    
    # Configurar variáveis de ambiente
    if (-not (Set-EnvironmentVariables)) {
        Write-Log "Falha ao configurar variáveis de ambiente." "ERROR"
    }
    
    # Criar diretórios
    if (-not (New-RequiredDirectories)) {
        Write-Log "Falha ao criar diretórios." "ERROR"
    }
    
    Write-Log "Instalação concluída!"
    return $true
}

# Executar instalação
Start-Installation 