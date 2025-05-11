# WYDBR2.0 Server Restore Script
# Autor: Claude
# Data: 2024

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    DatabaseHost = "localhost"
    DatabasePort = 3306
    BackupPath = "F:\Backups\WYDBR2.0"
    RestoreEmail = "admin@wydbr2.com"
    RestoreDiscord = "https://discord.com/api/webhooks/your_webhook_url"
    RestoreEnabled = $true
    RestoreVerify = $true
    RestoreEncryption = $true
    RestorePassword = "your_backup_password"
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
    Add-Content -Path "$($config.ProjectRoot)\logs\restore.log" -Value $logMessage
}

# Função para listar backups disponíveis
function Get-AvailableBackups {
    Write-Log "Listando backups disponíveis..."
    
    try {
        $backups = Get-ChildItem $config.BackupPath -Directory | 
        Where-Object { $_.Name -match '^\d{4}-\d{2}-\d{2}$' } | 
        Sort-Object Name -Descending
        
        if ($backups.Count -eq 0) {
            Write-Log "Nenhum backup encontrado." "WARNING"
            return $null
        }
        
        Write-Log "Backups encontrados: $($backups.Count)"
        return $backups
    }
    catch {
        Write-Log "Erro ao listar backups: $_" "ERROR"
        return $null
    }
}

# Função para descriptografar backup
function Unprotect-Backup {
    param(
        [string]$BackupFile
    )
    
    if (-not $config.RestoreEncryption) {
        return $BackupFile
    }
    
    Write-Log "Descriptografando arquivo de backup..."
    
    try {
        $decryptedFile = $BackupFile -replace '\.enc$', ''
        
        # Descriptografar arquivo
        $securePassword = ConvertTo-SecureString $config.RestorePassword -AsPlainText -Force
        $encryptedContent = Get-Content $BackupFile -Raw
        $decryptedContent = ConvertTo-SecureString $encryptedContent -SecureKey $securePassword
        [System.Runtime.InteropServices.Marshal]::PtrToStringAuto([System.Runtime.InteropServices.Marshal]::SecureStringToBSTR($decryptedContent)) | Set-Content $decryptedFile
        
        Write-Log "Arquivo descriptografado com sucesso: $decryptedFile"
        return $decryptedFile
    }
    catch {
        Write-Log "Erro ao descriptografar arquivo: $_" "ERROR"
        return $BackupFile
    }
}

# Função para restaurar banco de dados
function Restore-Database {
    param(
        [string]$BackupDir
    )
    
    Write-Log "Restaurando banco de dados..."
    
    try {
        # Encontrar arquivo de backup do banco
        $databaseBackup = Get-ChildItem $BackupDir -Filter "database_*.sql*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        
        if (-not $databaseBackup) {
            Write-Log "Arquivo de backup do banco não encontrado." "ERROR"
            return $false
        }
        
        # Descriptografar backup se necessário
        $databaseBackup = Unprotect-Backup -BackupFile $databaseBackup.FullName
        
        # Restaurar banco
        $mysql = "mysql"
        $mysqlArgs = @(
            "--host=$($config.DatabaseHost)",
            "--port=$($config.DatabasePort)",
            "--user=$($config.DatabaseUser)",
            "--password=$($config.DatabasePassword)",
            $config.DatabaseName
        )
        
        Get-Content $databaseBackup | & $mysql $mysqlArgs
        
        if ($LASTEXITCODE -eq 0) {
            Write-Log "Banco de dados restaurado com sucesso."
            return $true
        }
        else {
            Write-Log "Erro ao restaurar banco de dados." "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro ao restaurar banco de dados: $_" "ERROR"
        return $false
    }
}

# Função para restaurar arquivos
function Restore-Files {
    param(
        [string]$BackupDir
    )
    
    Write-Log "Restaurando arquivos..."
    
    try {
        # Encontrar arquivo de backup dos arquivos
        $filesBackup = Get-ChildItem $BackupDir -Filter "files_*.zip*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        
        if (-not $filesBackup) {
            Write-Log "Arquivo de backup dos arquivos não encontrado." "ERROR"
            return $false
        }
        
        # Descriptografar backup se necessário
        $filesBackup = Unprotect-Backup -BackupFile $filesBackup.FullName
        
        # Extrair arquivos
        Expand-Archive -Path $filesBackup -DestinationPath $config.ProjectRoot -Force
        
        Write-Log "Arquivos restaurados com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao restaurar arquivos: $_" "ERROR"
        return $false
    }
}

# Função para verificar restauração
function Test-Restore {
    Write-Log "Verificando restauração..."
    
    try {
        # Verificar banco de dados
        $query = "SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '$($config.DatabaseName)'"
        $tableCount = Invoke-MySQLQuery -Query $query
        
        if ($tableCount -eq 0) {
            Write-Log "Erro na verificação do banco de dados: nenhuma tabela encontrada" "ERROR"
            return $false
        }
        
        # Verificar arquivos
        $requiredDirs = @(
            "$($config.ProjectRoot)\config",
            "$($config.ProjectRoot)\data",
            "$($config.ProjectRoot)\logs",
            "$($config.ProjectRoot)\reports"
        )
        
        foreach ($dir in $requiredDirs) {
            if (-not (Test-Path $dir)) {
                Write-Log "Erro na verificação dos arquivos: diretório não encontrado: $dir" "ERROR"
                return $false
            }
        }
        
        Write-Log "Restauração verificada com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao verificar restauração: $_" "ERROR"
        return $false
    }
}

# Função para enviar notificação
function Send-RestoreNotification {
    param(
        [bool]$Success,
        [string]$Message,
        [string]$BackupDir
    )
    
    Write-Log "Enviando notificação de restauração..."
    
    try {
        $status = if ($Success) { "SUCESSO" } else { "FALHA" }
        $notification = @"
Restauração WYDBR2.0: $status

Data: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
Diretório: $BackupDir
Mensagem: $Message

Detalhes:
- Banco de Dados: $($config.DatabaseName)
- Host: $($config.DatabaseHost)
- Porta: $($config.DatabasePort)
"@
        
        # Enviar email
        if ($config.RestoreEmail) {
            Send-MailMessage -To $config.RestoreEmail -Subject "Restauração WYDBR2.0: $status" -Body $notification
        }
        
        # Enviar Discord
        if ($config.RestoreDiscord) {
            $discordBody = @{
                content = $notification
            } | ConvertTo-Json
            
            Invoke-RestMethod -Uri $config.RestoreDiscord -Method Post -Body $discordBody -ContentType "application/json"
        }
        
        Write-Log "Notificação enviada com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao enviar notificação: $_" "ERROR"
        return $false
    }
}

# Função principal
function Start-Restore {
    param(
        [string]$BackupDate
    )
    
    Write-Log "Iniciando processo de restauração..."
    
    # Listar backups disponíveis
    $backups = Get-AvailableBackups
    if (-not $backups) {
        Write-Log "Nenhum backup disponível para restauração. Abortando." "ERROR"
        return $false
    }
    
    # Selecionar backup
    $backupDir = if ($BackupDate) {
        $backups | Where-Object { $_.Name -eq $BackupDate } | Select-Object -First 1
    }
    else {
        $backups | Select-Object -First 1
    }
    
    if (-not $backupDir) {
        Write-Log "Backup não encontrado. Abortando." "ERROR"
        return $false
    }
    
    Write-Log "Selecionado backup: $($backupDir.Name)"
    
    # Parar servidor
    Stop-Service -Name "WYDBR2.0" -Force
    
    # Restaurar banco de dados
    if (-not (Restore-Database -BackupDir $backupDir.FullName)) {
        Write-Log "Falha ao restaurar banco de dados. Abortando." "ERROR"
        return $false
    }
    
    # Restaurar arquivos
    if (-not (Restore-Files -BackupDir $backupDir.FullName)) {
        Write-Log "Falha ao restaurar arquivos. Abortando." "ERROR"
        return $false
    }
    
    # Verificar restauração
    if ($config.RestoreVerify) {
        if (-not (Test-Restore)) {
            Write-Log "Falha na verificação da restauração." "ERROR"
            return $false
        }
    }
    
    # Iniciar servidor
    Start-Service -Name "WYDBR2.0"
    
    # Enviar notificação
    Send-RestoreNotification -Success $true -Message "Restauração concluída com sucesso" -BackupDir $backupDir.FullName
    
    Write-Log "Processo de restauração concluído com sucesso!"
    return $true
}

# Executar restauração
Start-Restore 