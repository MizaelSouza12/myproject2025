# WYDBR2.0 Deployment Script
# Autor: Claude
# Data: 2024

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

# Função para verificar dependências
function Test-Dependencies {
    Write-Log "Verificando dependências do sistema..."
    
    # Verificar CMake
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        Write-Log "CMake não encontrado. Por favor, instale o CMake." "ERROR"
        return $false
    }
    
    # Verificar compilador C++
    if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
        Write-Log "Compilador C++ não encontrado. Por favor, instale o MinGW ou MSVC." "ERROR"
        return $false
    }
    
    # Verificar MySQL
    if (-not (Get-Service -Name "MySQL*" -ErrorAction SilentlyContinue)) {
        Write-Log "Serviço MySQL não encontrado. Por favor, instale o MySQL." "ERROR"
        return $false
    }
    
    Write-Log "Todas as dependências verificadas com sucesso."
    return $true
}

# Função para build do projeto
function Start-Build {
    Write-Log "Iniciando build do projeto..."
    
    try {
        # Criar diretório de build se não existir
        if (-not (Test-Path "$($config.ProjectRoot)\build")) {
            New-Item -ItemType Directory -Path "$($config.ProjectRoot)\build"
        }
        
        # Configurar CMake
        Set-Location "$($config.ProjectRoot)\build"
        cmake .. -DCMAKE_BUILD_TYPE=$config.BuildType
        
        # Compilar
        cmake --build . --config $config.BuildType
        
        if ($LASTEXITCODE -eq 0) {
            Write-Log "Build concluído com sucesso."
            return $true
        } else {
            Write-Log "Erro durante o build." "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro durante o build: $_" "ERROR"
        return $false
    }
}

# Função para migração do banco de dados
function Start-DatabaseMigration {
    Write-Log "Iniciando migração do banco de dados..."
    
    try {
        # Executar scripts de migração
        $migrationScripts = Get-ChildItem "$($config.ProjectRoot)\database\migrations" -Filter "*.sql"
        foreach ($script in $migrationScripts) {
            Write-Log "Executando script: $($script.Name)"
            mysql -u $config.DatabaseUser -p$config.DatabasePassword $config.DatabaseName < $script.FullName
        }
        
        Write-Log "Migração do banco de dados concluída com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro durante a migração do banco de dados: $_" "ERROR"
        return $false
    }
}

# Função para backup do banco de dados
function Start-DatabaseBackup {
    Write-Log "Iniciando backup do banco de dados..."
    
    try {
        $timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
        $backupFile = "$($config.BackupPath)\backup_$timestamp.sql"
        
        # Criar diretório de backup se não existir
        if (-not (Test-Path $config.BackupPath)) {
            New-Item -ItemType Directory -Path $config.BackupPath
        }
        
        # Executar backup
        mysqldump -u $config.DatabaseUser -p$config.DatabasePassword $config.DatabaseName > $backupFile
        
        Write-Log "Backup concluído: $backupFile"
        return $true
    }
    catch {
        Write-Log "Erro durante o backup: $_" "ERROR"
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
            Write-Log "Testes concluídos com sucesso."
            return $true
        } else {
            Write-Log "Testes falharam." "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro durante os testes: $_" "ERROR"
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
        }
        
        $serverConfig | ConvertTo-Json | Set-Content "$($config.ProjectRoot)\config\server.json"
        
        # Iniciar servidor
        Start-Process -FilePath "$($config.ProjectRoot)\build\bin\wydbr2.0_server.exe" -NoNewWindow
        
        Write-Log "Servidor iniciado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao iniciar o servidor: $_" "ERROR"
        return $false
    }
}

# Função para monitoramento
function Start-Monitoring {
    if (-not $config.MonitoringEnabled) {
        return $true
    }
    
    Write-Log "Iniciando monitoramento..."
    
    try {
        # Iniciar agente de monitoramento
        Start-Process -FilePath "$($config.ProjectRoot)\tools\monitoring_agent.exe" -NoNewWindow
        
        Write-Log "Monitoramento iniciado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao iniciar monitoramento: $_" "ERROR"
        return $false
    }
}

# Função principal de deploy
function Start-Deploy {
    Write-Log "Iniciando processo de deploy do WYDBR2.0..."
    
    # Verificar dependências
    if (-not (Test-Dependencies)) {
        Write-Log "Falha na verificação de dependências. Abortando deploy." "ERROR"
        return $false
    }
    
    # Backup do banco de dados
    if (-not (Start-DatabaseBackup)) {
        Write-Log "Falha no backup do banco de dados. Abortando deploy." "ERROR"
        return $false
    }
    
    # Build do projeto
    if (-not (Start-Build)) {
        Write-Log "Falha no build do projeto. Abortando deploy." "ERROR"
        return $false
    }
    
    # Executar testes
    if (-not (Start-Tests)) {
        Write-Log "Falha nos testes. Abortando deploy." "ERROR"
        return $false
    }
    
    # Migração do banco de dados
    if (-not (Start-DatabaseMigration)) {
        Write-Log "Falha na migração do banco de dados. Abortando deploy." "ERROR"
        return $false
    }
    
    # Iniciar servidor
    if (-not (Start-Server)) {
        Write-Log "Falha ao iniciar o servidor. Abortando deploy." "ERROR"
        return $false
    }
    
    # Iniciar monitoramento
    if (-not (Start-Monitoring)) {
        Write-Log "Falha ao iniciar monitoramento. Abortando deploy." "ERROR"
        return $false
    }
    
    Write-Log "Deploy concluído com sucesso!"
    return $true
}

# Função para agendar tarefas
function Set-ScheduledTasks {
    Write-Log "Configurando tarefas agendadas..."
    
    try {
        # Agendar backup diário
        $action = New-ScheduledTaskAction -Execute "powershell.exe" -Argument "-File `"$($config.ProjectRoot)\scripts\backup.ps1`""
        $trigger = New-ScheduledTaskTrigger -Daily -At 3AM
        $settings = New-ScheduledTaskSettingsSet -StartWhenAvailable -DontStopOnIdleEnd
        
        Register-ScheduledTask -TaskName "WYDBR2.0_Backup" -Action $action -Trigger $trigger -Settings $settings -Force
        
        # Agendar verificação de integridade
        $action = New-ScheduledTaskAction -Execute "powershell.exe" -Argument "-File `"$($config.ProjectRoot)\scripts\health_check.ps1`""
        $trigger = New-ScheduledTaskTrigger -Once -At (Get-Date).AddMinutes(5) -RepetitionInterval (New-TimeSpan -Minutes 30)
        
        Register-ScheduledTask -TaskName "WYDBR2.0_HealthCheck" -Action $action -Trigger $trigger -Settings $settings -Force
        
        Write-Log "Tarefas agendadas configuradas com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao configurar tarefas agendadas: $_" "ERROR"
        return $false
    }
}

# Função para verificar saúde do sistema
function Test-SystemHealth {
    Write-Log "Verificando saúde do sistema..."
    
    try {
        # Verificar se o servidor está rodando
        $serverProcess = Get-Process wydbr2.0_server -ErrorAction SilentlyContinue
        if (-not $serverProcess) {
            Write-Log "Servidor não está rodando!" "ERROR"
            if ($config.AutoRestart) {
                Write-Log "Tentando reiniciar o servidor..."
                Start-Server
            }
            return $false
        }
        
        # Verificar uso de recursos
        $cpuUsage = (Get-Counter '\Process(wydbr2.0_server)\% Processor Time').CounterSamples.CookedValue
        $memoryUsage = (Get-Counter '\Process(wydbr2.0_server)\Working Set').CounterSamples.CookedValue
        
        if ($cpuUsage -gt 90) {
            Write-Log "Uso de CPU muito alto: $cpuUsage%" "WARNING"
        }
        
        if ($memoryUsage -gt 2GB) {
            Write-Log "Uso de memória muito alto: $($memoryUsage/1GB)GB" "WARNING"
        }
        
        # Verificar conexões com o banco de dados
        $dbConnections = mysql -u $config.DatabaseUser -p$config.DatabasePassword -e "SHOW STATUS LIKE 'Threads_connected';" $config.DatabaseName
        if ($dbConnections -gt $config.MaxConnections * 0.8) {
            Write-Log "Número alto de conexões com o banco de dados: $dbConnections" "WARNING"
        }
        
        Write-Log "Verificação de saúde concluída."
        return $true
    }
    catch {
        Write-Log "Erro ao verificar saúde do sistema: $_" "ERROR"
        return $false
    }
}

# Função para limpeza de logs
function Clear-OldLogs {
    Write-Log "Limpando logs antigos..."
    
    try {
        $logPath = "$($config.ProjectRoot)\logs"
        $logs = Get-ChildItem $logPath -Filter "*.log"
        
        foreach ($log in $logs) {
            if ($log.LastWriteTime -lt (Get-Date).AddDays(-7)) {
                Remove-Item $log.FullName
                Write-Log "Log removido: $($log.Name)"
            }
        }
        
        Write-Log "Limpeza de logs concluída."
        return $true
    }
    catch {
        Write-Log "Erro ao limpar logs: $_" "ERROR"
        return $false
    }
}

# Função para atualização do sistema
function Update-System {
    Write-Log "Iniciando atualização do sistema..."
    
    try {
        # Parar o servidor
        Stop-Process -Name "wydbr2.0_server" -Force -ErrorAction SilentlyContinue
        
        # Backup do banco de dados
        Start-DatabaseBackup
        
        # Atualizar código fonte (assumindo uso de git)
        Set-Location $config.ProjectRoot
        git pull
        
        # Rebuild do projeto
        Start-Build
        
        # Migração do banco de dados
        Start-DatabaseMigration
        
        # Reiniciar servidor
        Start-Server
        
        Write-Log "Atualização concluída com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro durante a atualização: $_" "ERROR"
        return $false
    }
}

# Menu principal
function Show-Menu {
    Clear-Host
    Write-Host "=== WYDBR2.0 Deployment Menu ==="
    Write-Host "1. Deploy Completo"
    Write-Host "2. Backup do Banco de Dados"
    Write-Host "3. Verificar Saúde do Sistema"
    Write-Host "4. Atualizar Sistema"
    Write-Host "5. Limpar Logs Antigos"
    Write-Host "6. Configurar Tarefas Agendadas"
    Write-Host "7. Sair"
    
    $choice = Read-Host "Escolha uma opção"
    
    switch ($choice) {
        "1" { Start-Deploy }
        "2" { Start-DatabaseBackup }
        "3" { Test-SystemHealth }
        "4" { Update-System }
        "5" { Clear-OldLogs }
        "6" { Set-ScheduledTasks }
        "7" { exit }
        default { Write-Host "Opção inválida" }
    }
}

# Iniciar menu
Show-Menu 