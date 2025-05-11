# WYDBR2.0 Server Monitoring Script
# Autor: Claude
# Data: 2024

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
            process = Get-Process -Name "wydbr2.0_server" -ErrorAction SilentlyContinue
        }
        
        # Salvar métricas
        $metrics | ConvertTo-Json | Add-Content -Path "$($config.ProjectRoot)\data\metrics\system_metrics.json"
        
        Write-Log "Métricas coletadas com sucesso."
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
        
        $metrics = Invoke-MySQLQuery -Query $query
        
        # Salvar métricas
        $metrics | ConvertTo-Json | Add-Content -Path "$($config.ProjectRoot)\data\metrics\database_metrics.json"
        
        Write-Log "Métricas do banco de dados coletadas com sucesso."
        return $metrics
    }
    catch {
        Write-Log "Erro ao coletar métricas do banco de dados: $_" "ERROR"
        return $null
    }
}

# Função para coletar métricas do jogo
function Get-GameMetrics {
    Write-Log "Coletando métricas do jogo..."
    
    try {
        $query = @"
        SELECT 
            (SELECT COUNT(*) FROM accounts) as total_accounts,
            (SELECT COUNT(*) FROM characters) as total_characters,
            (SELECT COUNT(*) FROM guilds) as total_guilds,
            (SELECT COUNT(*) FROM parties) as total_parties,
            (SELECT COUNT(*) FROM trades) as total_trades,
            (SELECT COUNT(*) FROM auctions) as total_auctions,
            (SELECT COUNT(*) FROM mails) as total_mails,
            (SELECT COUNT(*) FROM tickets) as total_tickets
"@
        
        $metrics = Invoke-MySQLQuery -Query $query
        
        # Salvar métricas
        $metrics | ConvertTo-Json | Add-Content -Path "$($config.ProjectRoot)\data\metrics\game_metrics.json"
        
        Write-Log "Métricas do jogo coletadas com sucesso."
        return $metrics
    }
    catch {
        Write-Log "Erro ao coletar métricas do jogo: $_" "ERROR"
        return $null
    }
}

# Função para verificar alertas
function Test-Alerts {
    param(
        [hashtable]$SystemMetrics,
        [hashtable]$DatabaseMetrics,
        [hashtable]$GameMetrics
    )
    
    Write-Log "Verificando alertas..."
    
    try {
        $alerts = @()
        
        # Verificar CPU
        if ($SystemMetrics.cpu -gt $config.MaxCpuUsage) {
            $alerts += "CPU acima do limite: $($SystemMetrics.cpu)%"
        }
        
        # Verificar memória
        if ($SystemMetrics.memory -lt 1024) { # Menos de 1GB disponível
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
        
        # Verificar tempo de resposta do banco
        if ($DatabaseMetrics.max_query_time -gt $config.MaxResponseTime) {
            $alerts += "Tempo de resposta do banco alto: $($DatabaseMetrics.max_query_time)ms"
        }
        
        # Enviar alertas
        if ($alerts.Count -gt 0) {
            $alertMessage = "ALERTAS DO SERVIDOR WYDBR2.0:`n`n" + ($alerts -join "`n")
            
            # Enviar email
            if ($config.AlertsEnabled) {
                Send-MailMessage -To $config.AlertEmail -Subject "Alertas WYDBR2.0" -Body $alertMessage
            }
            
            # Enviar Discord
            if ($config.AlertDiscord) {
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
        $cutoffDate = (Get-Date).AddDays(-$config.MetricsRetentionDays)
        
        Get-ChildItem "$($config.ProjectRoot)\data\metrics\*.json" | 
        Where-Object { $_.LastWriteTime -lt $cutoffDate } | 
        Remove-Item -Force
        
        Write-Log "Métricas antigas removidas com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao limpar métricas antigas: $_" "ERROR"
        return $false
    }
}

# Função para gerar relatório
function New-MetricsReport {
    Write-Log "Gerando relatório de métricas..."
    
    try {
        $report = @{
            timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
            system = Get-Content "$($config.ProjectRoot)\data\metrics\system_metrics.json" | ConvertFrom-Json
            database = Get-Content "$($config.ProjectRoot)\data\metrics\database_metrics.json" | ConvertFrom-Json
            game = Get-Content "$($config.ProjectRoot)\data\metrics\game_metrics.json" | ConvertFrom-Json
        }
        
        $report | ConvertTo-Json -Depth 10 | Set-Content "$($config.ProjectRoot)\reports\metrics_report.json"
        
        Write-Log "Relatório gerado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao gerar relatório: $_" "ERROR"
        return $false
    }
}

# Função principal
function Start-Monitoring {
    Write-Log "Iniciando monitoramento do servidor..."
    
    while ($true) {
        # Coletar métricas
        $systemMetrics = Get-SystemMetrics
        $databaseMetrics = Get-DatabaseMetrics
        $gameMetrics = Get-GameMetrics
        
        # Verificar alertas
        if ($systemMetrics -and $databaseMetrics -and $gameMetrics) {
            Test-Alerts -SystemMetrics $systemMetrics -DatabaseMetrics $databaseMetrics -GameMetrics $gameMetrics
        }
        
        # Limpar métricas antigas
        Clear-OldMetrics
        
        # Gerar relatório
        New-MetricsReport
        
        # Aguardar próximo intervalo
        Start-Sleep -Seconds $config.MetricsInterval
    }
}

# Executar monitoramento
Start-Monitoring 