# WYDBR2.0 Server Management Script
# Autor: Claude
# Data: 2024

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    ServerName = "WYDBR2.0"
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
    LogRetentionDays = 7
    MetricsRetentionDays = 30
    MetricsInterval = 60
    MetricsEnabled = $true
    AlertsEnabled = $true
    AutoRestart = $true
    AlertEmail = "admin@wydbr2.com"
    AlertDiscord = "https://discord.com/api/webhooks/your_webhook_url"
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
    Add-Content -Path "$($config.ProjectRoot)\logs\manage.log" -Value $logMessage
}

# Função para verificar status do servidor
function Get-ServerStatus {
    Write-Log "Verificando status do servidor..."
    
    try {
        $status = @{
            server = @{
                name = $config.ServerName
                port = $config.ServerPort
                process = Get-Process -Name "wydbr2.0_server" -ErrorAction SilentlyContinue
                service = Get-Service -Name $config.ServerName -ErrorAction SilentlyContinue
                connections = (Get-NetTCPConnection -State Listen | Where-Object LocalPort -eq $config.ServerPort).Count
            }
            system = @{
                cpu = (Get-Counter '\Processor(_Total)\% Processor Time').CounterSamples.CookedValue
                memory = (Get-Counter '\Memory\Available MBytes').CounterSamples.CookedValue
                disk = (Get-Counter '\LogicalDisk(_Total)\% Free Space').CounterSamples.CookedValue
            }
            database = @{
                name = $config.DatabaseName
                host = $config.DatabaseHost
                port = $config.DatabasePort
                connections = (Invoke-MySQLQuery -Query "SELECT COUNT(*) FROM information_schema.processlist WHERE user = '$($config.DatabaseUser)'")
            }
        }
        
        Write-Log "Status do servidor verificado com sucesso."
        return $status
    }
    catch {
        Write-Log "Erro ao verificar status do servidor: $_" "ERROR"
        return $null
    }
}

# Função para iniciar servidor
function Start-Server {
    Write-Log "Iniciando servidor..."
    
    try {
        # Verificar se o servidor já está rodando
        $serverProcess = Get-Process -Name "wydbr2.0_server" -ErrorAction SilentlyContinue
        if ($serverProcess) {
            Write-Log "Servidor já está rodando." "WARNING"
            return $true
        }
        
        # Iniciar serviço
        Start-Service -Name $config.ServerName
        
        # Aguardar inicialização
        Start-Sleep -Seconds 10
        
        # Verificar se iniciou
        $serverProcess = Get-Process -Name "wydbr2.0_server" -ErrorAction SilentlyContinue
        if ($serverProcess) {
            Write-Log "Servidor iniciado com sucesso."
            return $true
        }
        else {
            Write-Log "Erro ao iniciar servidor." "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro ao iniciar servidor: $_" "ERROR"
        return $false
    }
}

# Função para parar servidor
function Stop-Server {
    Write-Log "Parando servidor..."
    
    try {
        # Verificar se o servidor está rodando
        $serverProcess = Get-Process -Name "wydbr2.0_server" -ErrorAction SilentlyContinue
        if (-not $serverProcess) {
            Write-Log "Servidor não está rodando." "WARNING"
            return $true
        }
        
        # Parar serviço
        Stop-Service -Name $config.ServerName -Force
        
        # Aguardar parada
        Start-Sleep -Seconds 10
        
        # Verificar se parou
        $serverProcess = Get-Process -Name "wydbr2.0_server" -ErrorAction SilentlyContinue
        if (-not $serverProcess) {
            Write-Log "Servidor parado com sucesso."
            return $true
        }
        else {
            Write-Log "Erro ao parar servidor." "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro ao parar servidor: $_" "ERROR"
        return $false
    }
}

# Função para reiniciar servidor
function Restart-Server {
    Write-Log "Reiniciando servidor..."
    
    try {
        # Parar servidor
        if (-not (Stop-Server)) {
            Write-Log "Falha ao parar servidor. Abortando reinicialização." "ERROR"
            return $false
        }
        
        # Aguardar
        Start-Sleep -Seconds 5
        
        # Iniciar servidor
        if (-not (Start-Server)) {
            Write-Log "Falha ao iniciar servidor. Abortando reinicialização." "ERROR"
            return $false
        }
        
        Write-Log "Servidor reiniciado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao reiniciar servidor: $_" "ERROR"
        return $false
    }
}

# Função para limpar logs
function Clear-Logs {
    Write-Log "Limpando logs..."
    
    try {
        $cutoffDate = (Get-Date).AddDays(-$config.LogRetentionDays)
        
        Get-ChildItem "$($config.ProjectRoot)\logs\*.log" | 
        Where-Object { $_.LastWriteTime -lt $cutoffDate } | 
        Remove-Item -Force
        
        Write-Log "Logs limpos com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao limpar logs: $_" "ERROR"
        return $false
    }
}

# Função para limpar métricas
function Clear-Metrics {
    Write-Log "Limpando métricas..."
    
    try {
        $cutoffDate = (Get-Date).AddDays(-$config.MetricsRetentionDays)
        
        Get-ChildItem "$($config.ProjectRoot)\data\metrics\*.json" | 
        Where-Object { $_.LastWriteTime -lt $cutoffDate } | 
        Remove-Item -Force
        
        Write-Log "Métricas limpas com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao limpar métricas: $_" "ERROR"
        return $false
    }
}

# Função para enviar notificação
function Send-Notification {
    param(
        [string]$Subject,
        [string]$Message
    )
    
    Write-Log "Enviando notificação..."
    
    try {
        # Enviar email
        if ($config.AlertEmail) {
            Send-MailMessage -To $config.AlertEmail -Subject $Subject -Body $Message
        }
        
        # Enviar Discord
        if ($config.AlertDiscord) {
            $discordBody = @{
                content = $Message
            } | ConvertTo-Json
            
            Invoke-RestMethod -Uri $config.AlertDiscord -Method Post -Body $discordBody -ContentType "application/json"
        }
        
        Write-Log "Notificação enviada com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao enviar notificação: $_" "ERROR"
        return $false
    }
}

# Função para mostrar menu
function Show-Menu {
    Write-Host "`nWYDBR2.0 Server Management"
    Write-Host "------------------------"
    Write-Host "1. Verificar Status"
    Write-Host "2. Iniciar Servidor"
    Write-Host "3. Parar Servidor"
    Write-Host "4. Reiniciar Servidor"
    Write-Host "5. Limpar Logs"
    Write-Host "6. Limpar Métricas"
    Write-Host "7. Sair"
    Write-Host "------------------------"
}

# Função principal
function Start-Management {
    while ($true) {
        Show-Menu
        
        $choice = Read-Host "`nEscolha uma opção"
        
        switch ($choice) {
            "1" {
                $status = Get-ServerStatus
                if ($status) {
                    Write-Host "`nStatus do Servidor:"
                    Write-Host "-----------------"
                    Write-Host "Processo: $($status.server.process ? 'Rodando' : 'Parado')"
                    Write-Host "Serviço: $($status.server.service.Status)"
                    Write-Host "Conexões: $($status.server.connections)"
                    Write-Host "CPU: $($status.system.cpu)%"
                    Write-Host "Memória: $($status.system.memory)MB"
                    Write-Host "Disco: $($status.system.disk)%"
                    Write-Host "Banco de Dados: $($status.database.connections) conexões"
                }
            }
            "2" {
                if (Start-Server) {
                    Send-Notification -Subject "Servidor Iniciado" -Message "O servidor WYDBR2.0 foi iniciado com sucesso."
                }
            }
            "3" {
                if (Stop-Server) {
                    Send-Notification -Subject "Servidor Parado" -Message "O servidor WYDBR2.0 foi parado com sucesso."
                }
            }
            "4" {
                if (Restart-Server) {
                    Send-Notification -Subject "Servidor Reiniciado" -Message "O servidor WYDBR2.0 foi reiniciado com sucesso."
                }
            }
            "5" {
                Clear-Logs
            }
            "6" {
                Clear-Metrics
            }
            "7" {
                Write-Host "`nSaindo..."
                return
            }
            default {
                Write-Host "`nOpção inválida!"
            }
        }
        
        Write-Host "`nPressione qualquer tecla para continuar..."
        $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    }
}

# Executar gerenciamento
Start-Management 