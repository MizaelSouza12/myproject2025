# WYDBR2.0 Server Monitoring Script
# Requer execução como Administrador
#Requires -RunAsAdministrator

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    ServerPort = 8080
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    DatabaseHost = "localhost"
    DatabasePort = 3306
    MaxCpuUsage = 90
    MaxMemoryUsage = 2GB
    MaxDiskUsage = 90
    MaxConnections = 1000
    MaxResponseTime = 1000
    AlertEmail = "admin@wydbr2.com"
    AlertDiscord = "https://discord.com/api/webhooks/your_webhook_url"
    LogRetentionDays = 7
    MetricsRetentionDays = 30
    MetricsInterval = 60
    MetricsEnabled = $true
    AlertsEnabled = $true
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
    Add-Content -Path "$($config.ProjectRoot)\logs\monitor.log" -Value $logMessage
}

# Verificar execução como administrador
Write-Log "Verificando privilégios de administrador..."
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Log "Este script precisa ser executado como Administrador!" "ERROR"
    exit 1
}

# Função para coletar métricas do sistema
function Get-SystemMetrics {
    Write-Log "Coletando métricas do sistema..."
    
    try {
        $metrics = @{
            timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
            cpu = (Get-Counter '\Processor(_Total)\% Processor Time').CounterSamples.CookedValue
            memory = (Get-Counter '\Memory\Available MBytes').CounterSamples.CookedValue
            disk = (Get-Counter '\LogicalDisk(_Total)\% Free Space').CounterSamples.CookedValue
            network = (Get-Counter '\Network Interface(*)\Bytes Total/sec').CounterSamples.CookedValue
            connections = (Get-NetTCPConnection -State Listen | Where-Object LocalPort -eq $config.ServerPort).Count
            process = Get-Process -Name "wydbr2_server" -ErrorAction SilentlyContinue
        }
        
        # Salvar métricas
        $metricsDir = "$($config.ProjectRoot)\data\metrics"
        if (-not (Test-Path $metricsDir)) {
            New-Item -ItemType Directory -Path $metricsDir -Force | Out-Null
        }
        
        $metrics | ConvertTo-Json | Add-Content -Path "$metricsDir\system_metrics.json"
        
        Write-Log "Métricas coletadas com sucesso"
        return $metrics
    }
    catch {
        Write-Log "Erro ao coletar métricas: $_" "ERROR"
        return $null
    }
}

# Função para coletar métricas do banco de dados
function Get-DatabaseMetrics {
    Write-Log "Coletando métricas do banco de dados..."
    
    try {
        $query = @"
        SELECT 
            COUNT(*) as total_connections,
            SUM(CASE WHEN command = 'Sleep' THEN 1 ELSE 0 END) as idle_connections,
            SUM(CASE WHEN command != 'Sleep' THEN 1 ELSE 0 END) as active_connections,
            MAX(time) as max_query_time,
            AVG(time) as avg_query_time
        FROM information_schema.processlist
        WHERE user = '$($config.DatabaseUser)'
"@
        
        $metrics = Invoke-MySqlQuery -Query $query -ConnectionString "Server=$($config.DatabaseHost);Port=$($config.DatabasePort);Database=$($config.DatabaseName);Uid=$($config.DatabaseUser);Pwd=$($config.DatabasePassword)"
        
        # Salvar métricas
        $metricsDir = "$($config.ProjectRoot)\data\metrics"
        if (-not (Test-Path $metricsDir)) {
            New-Item -ItemType Directory -Path $metricsDir -Force | Out-Null
        }
        
        $metrics | ConvertTo-Json | Add-Content -Path "$metricsDir\database_metrics.json"
        
        Write-Log "Métricas do banco de dados coletadas com sucesso"
        return $metrics
    }
    catch {
        Write-Log "Erro ao coletar métricas do banco de dados: $_" "ERROR"
        return $null
    }
}

# Função para verificar alertas
function Test-Alerts {
    param(
        [hashtable]$SystemMetrics,
        [hashtable]$DatabaseMetrics
    )
    
    Write-Log "Verificando alertas..."
    
    try {
        $alerts = @()
        
        # Verificar CPU
        if ($SystemMetrics.cpu -gt $config.MaxCpuUsage) {
            $alerts += "CPU acima do limite: $($SystemMetrics.cpu)%"
        }
        
        # Verificar memória
        if ($SystemMetrics.memory -lt 1024) {
            $alerts += "Memória baixa: $($SystemMetrics.memory)MB disponível"
        }
        
        # Verificar disco
        if ($SystemMetrics.disk -lt (100 - $config.MaxDiskUsage)) {
            $alerts += "Espaço em disco baixo: $($SystemMetrics.disk)% livre"
        }
        
        # Verificar conexões
        if ($SystemMetrics.connections -gt $config.MaxConnections) {
            $alerts += "Número de conexões acima do limite: $($SystemMetrics.connections)"
        }
        
        # Verificar processo do servidor
        if (-not $SystemMetrics.process) {
            $alerts += "Processo do servidor não encontrado!"
            
            if ($config.AutoRestart) {
                Write-Log "Tentando reiniciar o servidor..."
                Start-Process "$($config.ProjectRoot)\build\bin\wydbr2_server.exe" -NoNewWindow
            }
        }
        
        # Verificar tempo de resposta do banco
        if ($DatabaseMetrics.max_query_time -gt $config.MaxResponseTime) {
            $alerts += "Tempo de resposta do banco alto: $($DatabaseMetrics.max_query_time)ms"
        }
        
        # Enviar alertas
        if ($alerts.Count -gt 0) {
            $alertMessage = "ALERTAS DO SERVIDOR WYDBR2.0:`n`n" + ($alerts -join "`n")
            
            # Enviar email
            if ($config.AlertsEnabled -and $config.AlertEmail) {
                Send-MailMessage -To $config.AlertEmail -Subject "Alertas WYDBR2.0" -Body $alertMessage
            }
            
            # Enviar Discord
            if ($config.AlertsEnabled -and $config.AlertDiscord) {
                $discordBody = @{
                    content = $alertMessage
                } | ConvertTo-Json
                
                Invoke-RestMethod -Uri $config.AlertDiscord -Method Post -Body $discordBody -ContentType "application/json"
            }
            
            Write-Log "Alertas enviados: $($alerts.Count)"
        }
        
        return $alerts
    }
    catch {
        Write-Log "Erro ao verificar alertas: $_" "ERROR"
        return $null
    }
}

# Função para limpar métricas antigas
function Clear-OldMetrics {
    Write-Log "Limpando métricas antigas..."
    
    try {
        $metricsDir = "$($config.ProjectRoot)\data\metrics"
        $cutoffDate = (Get-Date).AddDays(-$config.MetricsRetentionDays)
        
        Get-ChildItem $metricsDir -File | Where-Object {
            $_.LastWriteTime -lt $cutoffDate
        } | Remove-Item -Force
        
        Write-Log "Limpeza de métricas concluída"
    }
    catch {
        Write-Log "Erro ao limpar métricas antigas: $_" "ERROR"
    }
}

# Função principal de monitoramento
function Start-Monitoring {
    Write-Log "Iniciando monitoramento do servidor..."
    
    while ($true) {
        # Coletar métricas
        $systemMetrics = Get-SystemMetrics
        $databaseMetrics = Get-DatabaseMetrics
        
        if ($systemMetrics -and $databaseMetrics) {
            # Verificar alertas
            Test-Alerts -SystemMetrics $systemMetrics -DatabaseMetrics $databaseMetrics
        }
        
        # Limpar métricas antigas periodicamente
        if ((Get-Random -Minimum 1 -Maximum 100) -eq 1) {
            Clear-OldMetrics
        }
        
        # Aguardar intervalo
        Start-Sleep -Seconds $config.MetricsInterval
    }
}

# Iniciar monitoramento
Start-Monitoring 