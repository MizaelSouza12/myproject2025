# WYDBR2.0 Deployment Script
# Requer execução como Administrador
#Requires -RunAsAdministrator

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    BuildType = "Release"
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    ServerPort = 8080
    MaxConnections = 1000
    LogLevel = "INFO"
    BackupPath = "F:\Backups\WYDBR2.0"
    MonitoringEnabled = $true
    AutoRestart = $true
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
    Add-Content -Path "$($config.ProjectRoot)\logs\deploy.log" -Value $logMessage
}

# Verificar execução como administrador
Write-Log "Verificando privilégios de administrador..."
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Log "Este script precisa ser executado como Administrador!" "ERROR"
    exit 1
}

# Função para verificar dependências
function Test-Dependencies {
    Write-Log "Verificando dependências do sistema..."
    
    $dependencies = @{
        "cmake" = "CMake"
        "mysql" = "MySQL"
        "git" = "Git"
        "msbuild" = "Visual Studio Build Tools"
    }
    
    $missingDeps = @()
    
    foreach ($dep in $dependencies.Keys) {
        if (-not (Get-Command $dep -ErrorAction SilentlyContinue)) {
            $missingDeps += $dependencies[$dep]
        }
    }
    
    if ($missingDeps.Count -gt 0) {
        Write-Log "Dependências faltando: $($missingDeps -join ', ')" "ERROR"
        Write-Log "Por favor, execute o script install_dependencies.ps1 primeiro." "ERROR"
        return $false
    }
    
    Write-Log "Todas as dependências verificadas com sucesso."
    return $true
}

# Função para backup do banco de dados
function Start-DatabaseBackup {
    Write-Log "Iniciando backup do banco de dados..."
    
    try {
        $timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
        $backupFile = "$($config.BackupPath)\backup_$timestamp.sql"
        
        # Criar diretório de backup se não existir
        if (-not (Test-Path $config.BackupPath)) {
            New-Item -ItemType Directory -Path $config.BackupPath -Force | Out-Null
        }
        
        # Executar backup
        mysqldump -u $config.DatabaseUser -p$config.DatabasePassword $config.DatabaseName > $backupFile
        
        if ($LASTEXITCODE -eq 0) {
            Write-Log "Backup concluído: $backupFile"
            return $true
        }
        else {
            Write-Log "Erro ao criar backup" "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro durante o backup: $_" "ERROR"
        return $false
    }
}

# Função para build do projeto
function Start-Build {
    Write-Log "Iniciando build do projeto..."
    
    try {
        # Criar diretório de build se não existir
        $buildDir = "$($config.ProjectRoot)\build"
        if (-not (Test-Path $buildDir)) {
            New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
        }
        
        # Configurar CMake
        Set-Location $buildDir
        cmake .. -DCMAKE_BUILD_TYPE=$($config.BuildType)
        
        if ($LASTEXITCODE -ne 0) {
            Write-Log "Erro na configuração do CMake" "ERROR"
            return $false
        }
        
        # Compilar
        cmake --build . --config $($config.BuildType)
        
        if ($LASTEXITCODE -eq 0) {
            Write-Log "Build concluído com sucesso"
            return $true
        }
        else {
            Write-Log "Erro durante o build" "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro durante o build: $_" "ERROR"
        return $false
    }
}

# Função para executar testes
function Start-Tests {
    Write-Log "Iniciando testes..."
    
    try {
        Set-Location "$($config.ProjectRoot)\build"
        ctest --output-on-failure
        
        if ($LASTEXITCODE -eq 0) {
            Write-Log "Testes concluídos com sucesso"
            return $true
        }
        else {
            Write-Log "Falha nos testes" "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro durante os testes: $_" "ERROR"
        return $false
    }
}

# Função para migração do banco de dados
function Start-DatabaseMigration {
    Write-Log "Iniciando migração do banco de dados..."
    
    try {
        $migrationsDir = "$($config.ProjectRoot)\database\migrations"
        if (-not (Test-Path $migrationsDir)) {
            Write-Log "Diretório de migrações não encontrado" "ERROR"
            return $false
        }
        
        # Executar migrações em ordem
        Get-ChildItem $migrationsDir -Filter "*.sql" | Sort-Object Name | ForEach-Object {
            Write-Log "Executando migração: $($_.Name)"
            mysql -u $config.DatabaseUser -p$config.DatabasePassword $config.DatabaseName < $_.FullName
            
            if ($LASTEXITCODE -ne 0) {
                Write-Log "Erro ao executar migração: $($_.Name)" "ERROR"
                return $false
            }
        }
        
        Write-Log "Migrações concluídas com sucesso"
        return $true
    }
    catch {
        Write-Log "Erro durante as migrações: $_" "ERROR"
        return $false
    }
}

# Função para iniciar o servidor
function Start-Server {
    Write-Log "Iniciando servidor WYDBR2.0..."
    
    try {
        # Criar arquivo de configuração
        $serverConfig = @{
            port = $config.ServerPort
            max_connections = $config.MaxConnections
            log_level = $config.LogLevel
            database = @{
                name = $config.DatabaseName
                user = $config.DatabaseUser
                password = $config.DatabasePassword
            }
            monitoring = @{
                enabled = $config.MonitoringEnabled
                auto_restart = $config.AutoRestart
            }
        }
        
        $configDir = "$($config.ProjectRoot)\config"
        if (-not (Test-Path $configDir)) {
            New-Item -ItemType Directory -Path $configDir -Force | Out-Null
        }
        
        $serverConfig | ConvertTo-Json | Set-Content "$configDir\server.json"
        
        # Iniciar servidor
        $serverExe = "$($config.ProjectRoot)\build\bin\$($config.BuildType)\wydbr2_server.exe"
        if (Test-Path $serverExe) {
            Start-Process $serverExe -NoNewWindow
            Write-Log "Servidor iniciado com sucesso"
            return $true
        }
        else {
            Write-Log "Executável do servidor não encontrado: $serverExe" "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro ao iniciar o servidor: $_" "ERROR"
        return $false
    }
}

# Função principal de deploy
function Start-Deploy {
    Write-Log "Iniciando processo de deploy..."
    
    # Verificar dependências
    if (-not (Test-Dependencies)) {
        return $false
    }
    
    # Fazer backup
    if (-not (Start-DatabaseBackup)) {
        Write-Log "Falha no backup. Continuando com o deploy..." "WARNING"
    }
    
    # Build
    if (-not (Start-Build)) {
        return $false
    }
    
    # Testes
    if (-not (Start-Tests)) {
        Write-Log "Falha nos testes. Deseja continuar? (S/N)"
        $response = Read-Host
        if ($response -ne "S") {
            return $false
        }
    }
    
    # Migração
    if (-not (Start-DatabaseMigration)) {
        return $false
    }
    
    # Iniciar servidor
    if (-not (Start-Server)) {
        return $false
    }
    
    Write-Log "Deploy concluído com sucesso!"
    return $true
}

# Executar deploy
Start-Deploy 