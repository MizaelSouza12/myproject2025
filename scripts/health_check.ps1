# WYDBR2.0 Health Check Script
# Autor: Claude
# Data: 2024

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    ServerPort = 8080
    MaxConnections = 1000
    MaxCpuUsage = 90
    MaxMemoryUsage = 2GB
    MaxDiskUsage = 90
    AutoRestart = $true
    AlertEmail = "admin@wydbr2.com"
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
    Add-Content -Path "$($config.ProjectRoot)\logs\health.log" -Value $logMessage
}

# Função para verificar processo do servidor
function Test-ServerProcess {
    Write-Log "Verificando processo do servidor..."
    
    try {
        $serverProcess = Get-Process wydbr2.0_server -ErrorAction SilentlyContinue
        if (-not $serverProcess) {
            Write-Log "Servidor não está rodando!" "ERROR"
            if ($config.AutoRestart) {
                Write-Log "Tentando reiniciar o servidor..."
                Start-Process -FilePath "$($config.ProjectRoot)\build\bin\wydbr2.0_server.exe" -NoNewWindow
            }
            return $false
        }
        
        Write-Log "Servidor está rodando (PID: $($serverProcess.Id))"
        return $true
    }
    catch {
        Write-Log "Erro ao verificar processo do servidor: $_" "ERROR"
        return $false
    }
}

# Função para verificar uso de recursos
function Test-ResourceUsage {
    Write-Log "Verificando uso de recursos..."
    
    try {
        $serverProcess = Get-Process wydbr2.0_server -ErrorAction SilentlyContinue
        if ($serverProcess) {
            # CPU
            $cpuUsage = (Get-Counter '\Process(wydbr2.0_server)\% Processor Time').CounterSamples.CookedValue
            if ($cpuUsage -gt $config.MaxCpuUsage) {
                Write-Log "Uso de CPU muito alto: $cpuUsage%" "WARNING"
            }
            
            # Memória
            $memoryUsage = $serverProcess.WorkingSet64
            if ($memoryUsage -gt $config.MaxMemoryUsage) {
                Write-Log "Uso de memória muito alto: $([math]::Round($memoryUsage/1GB, 2))GB" "WARNING"
            }
        }
        
        # Disco
        $drive = (Get-PSDrive -PSProvider FileSystem | Where-Object {$_.Root -eq (Split-Path $config.ProjectRoot -Qualifier)})
        $diskUsage = ($drive.Used / $drive.Used + $drive.Free) * 100
        if ($diskUsage -gt $config.MaxDiskUsage) {
            Write-Log "Uso de disco muito alto: $([math]::Round($diskUsage, 2))%" "WARNING"
        }
        
        Write-Log "Verificação de recursos concluída"
        return $true
    }
    catch {
        Write-Log "Erro ao verificar uso de recursos: $_" "ERROR"
        return $false
    }
}

# Função para verificar conexões com o banco de dados
function Test-DatabaseConnections {
    Write-Log "Verificando conexões com o banco de dados..."
    
    try {
        $dbConnections = mysql -u $config.DatabaseUser -p$config.DatabasePassword -e "SHOW STATUS LIKE 'Threads_connected';" $config.DatabaseName
        if ($dbConnections -gt $config.MaxConnections * 0.8) {
            Write-Log "Número alto de conexões com o banco de dados: $dbConnections" "WARNING"
        }
        
        Write-Log "Verificação de conexões concluída"
        return $true
    }
    catch {
        Write-Log "Erro ao verificar conexões com o banco de dados: $_" "ERROR"
        return $false
    }
}

# Função para verificar portas
function Test-Ports {
    Write-Log "Verificando portas..."
    
    try {
        $tcpConnections = Get-NetTCPConnection -LocalPort $config.ServerPort -ErrorAction SilentlyContinue
        if ($tcpConnections) {
            Write-Log "Porta $($config.ServerPort) está em uso"
        } else {
            Write-Log "Porta $($config.ServerPort) não está em uso" "WARNING"
        }
        
        Write-Log "Verificação de portas concluída"
        return $true
    }
    catch {
        Write-Log "Erro ao verificar portas: $_" "ERROR"
        return $false
    }
}

# Função para verificar logs
function Test-Logs {
    Write-Log "Verificando logs..."
    
    try {
        $logPath = "$($config.ProjectRoot)\logs"
        $logs = Get-ChildItem $logPath -Filter "*.log"
        
        foreach ($log in $logs) {
            $lastError = Get-Content $log.FullName -Tail 100 | Select-String -Pattern "ERROR|FATAL|CRITICAL"
            if ($lastError) {
                Write-Log "Erros encontrados em $($log.Name):" "WARNING"
                $lastError | ForEach-Object { Write-Log $_ }
            }
        }
        
        Write-Log "Verificação de logs concluída"
        return $true
    }
    catch {
        Write-Log "Erro ao verificar logs: $_" "ERROR"
        return $false
    }
}

# Função para enviar alerta por email
function Send-Alert {
    param(
        [string]$Subject,
        [string]$Body
    )
    
    try {
        $smtpServer = "smtp.wydbr2.com"
        $smtpPort = 587
        $smtpUser = "alerts@wydbr2.com"
        $smtpPass = "your_smtp_password"
        
        $message = New-Object System.Net.Mail.MailMessage
        $message.From = $smtpUser
        $message.To.Add($config.AlertEmail)
        $message.Subject = $Subject
        $message.Body = $Body
        
        $smtp = New-Object System.Net.Mail.SmtpClient($smtpServer, $smtpPort)
        $smtp.EnableSsl = $true
        $smtp.Credentials = New-Object System.Net.NetworkCredential($smtpUser, $smtpPass)
        
        $smtp.Send($message)
        Write-Log "Alerta enviado por email"
        return $true
    }
    catch {
        Write-Log "Erro ao enviar alerta por email: $_" "ERROR"
        return $false
    }
}

# Função principal
function Start-HealthCheck {
    Write-Log "Iniciando verificação de saúde do sistema..."
    
    $issues = @()
    
    # Verificar processo do servidor
    if (-not (Test-ServerProcess)) {
        $issues += "Servidor não está rodando"
    }
    
    # Verificar uso de recursos
    if (-not (Test-ResourceUsage)) {
        $issues += "Problemas com uso de recursos"
    }
    
    # Verificar conexões com o banco de dados
    if (-not (Test-DatabaseConnections)) {
        $issues += "Problemas com conexões do banco de dados"
    }
    
    # Verificar portas
    if (-not (Test-Ports)) {
        $issues += "Problemas com portas"
    }
    
    # Verificar logs
    if (-not (Test-Logs)) {
        $issues += "Problemas com logs"
    }
    
    # Se houver problemas, enviar alerta
    if ($issues.Count -gt 0) {
        $subject = "WYDBR2.0 - Alertas de Saúde do Sistema"
        $body = "Problemas detectados:`n" + ($issues -join "`n")
        Send-Alert -Subject $subject -Body $body
    }
    
    Write-Log "Verificação de saúde concluída"
    return $issues.Count -eq 0
}

# Executar verificação de saúde
Start-HealthCheck 