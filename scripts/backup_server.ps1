# WYDBR2.0 Server Backup Script
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
    BackupRetentionDays = 30
    BackupCompression = $true
    BackupEncryption = $true
    BackupPassword = "your_backup_password"
    BackupEmail = "admin@wydbr2.com"
    BackupDiscord = "https://discord.com/api/webhooks/your_webhook_url"
    BackupEnabled = $true
    BackupSchedule = "Daily"
    BackupTime = "03:00"
    BackupType = "Full"
    BackupVerify = $true
    BackupCleanup = $true
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
    Add-Content -Path "$($config.ProjectRoot)\logs\backup.log" -Value $logMessage
}

# Função para criar diretório de backup
function New-BackupDirectory {
    Write-Log "Criando diretório de backup..."
    
    try {
        $backupDir = "$($config.BackupPath)\$(Get-Date -Format 'yyyy-MM-dd')"
        
        if (-not (Test-Path $backupDir)) {
            New-Item -ItemType Directory -Path $backupDir -Force
            Write-Log "Diretório de backup criado: $backupDir"
        }
        
        return $backupDir
    }
    catch {
        Write-Log "Erro ao criar diretório de backup: $_" "ERROR"
        return $null
    }
}

# Função para backup do banco de dados
function Backup-Database {
    param(
        [string]$BackupDir
    )
    
    Write-Log "Iniciando backup do banco de dados..."
    
    try {
        $timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
        $backupFile = "$BackupDir\database_$timestamp.sql"
        
        # Comando mysqldump
        $mysqldump = "mysqldump"
        $mysqldumpArgs = @(
            "--host=$($config.DatabaseHost)",
            "--port=$($config.DatabasePort)",
            "--user=$($config.DatabaseUser)",
            "--password=$($config.DatabasePassword)",
            "--databases",
            $config.DatabaseName,
            "--result-file=$backupFile",
            "--single-transaction",
            "--quick",
            "--lock-tables=false"
        )
        
        # Executar backup
        & $mysqldump $mysqldumpArgs
        
        if ($LASTEXITCODE -eq 0) {
            Write-Log "Backup do banco de dados concluído: $backupFile"
            
            # Verificar backup
            if ($config.BackupVerify) {
                $verifyQuery = "SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '$($config.DatabaseName)'"
                $tableCount = Invoke-MySQLQuery -Query $verifyQuery
                
                if ($tableCount -gt 0) {
                    Write-Log "Backup verificado com sucesso: $tableCount tabelas encontradas"
                }
                else {
                    Write-Log "Erro na verificação do backup: nenhuma tabela encontrada" "ERROR"
                    return $false
                }
            }
            
            return $backupFile
        }
        else {
            Write-Log "Erro ao executar backup do banco de dados" "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro ao fazer backup do banco de dados: $_" "ERROR"
        return $false
    }
}

# Função para backup dos arquivos
function Backup-Files {
    param(
        [string]$BackupDir
    )
    
    Write-Log "Iniciando backup dos arquivos..."
    
    try {
        $timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
        $backupFile = "$BackupDir\files_$timestamp.zip"
        
        # Diretórios para backup
        $directories = @(
            "$($config.ProjectRoot)\config",
            "$($config.ProjectRoot)\data",
            "$($config.ProjectRoot)\logs",
            "$($config.ProjectRoot)\reports"
        )
        
        # Criar arquivo ZIP
        Compress-Archive -Path $directories -DestinationPath $backupFile -Force
        
        if (Test-Path $backupFile) {
            Write-Log "Backup dos arquivos concluído: $backupFile"
            return $backupFile
        }
        else {
            Write-Log "Erro ao criar arquivo de backup" "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro ao fazer backup dos arquivos: $_" "ERROR"
        return $false
    }
}

# Função para criptografar backup
function Protect-Backup {
    param(
        [string]$BackupFile
    )
    
    if (-not $config.BackupEncryption) {
        return $BackupFile
    }
    
    Write-Log "Criptografando arquivo de backup..."
    
    try {
        $encryptedFile = "$BackupFile.enc"
        
        # Criptografar arquivo
        $securePassword = ConvertTo-SecureString $config.BackupPassword -AsPlainText -Force
        $encryptedContent = Get-Content $BackupFile -Raw | ConvertTo-SecureString -AsPlainText -Force
        $encryptedContent | ConvertFrom-SecureString -SecureKey $securePassword | Set-Content $encryptedFile
        
        # Remover arquivo original
        Remove-Item $BackupFile -Force
        
        Write-Log "Arquivo criptografado com sucesso: $encryptedFile"
        return $encryptedFile
    }
    catch {
        Write-Log "Erro ao criptografar arquivo: $_" "ERROR"
        return $BackupFile
    }
}

# Função para limpar backups antigos
function Clear-OldBackups {
    Write-Log "Limpando backups antigos..."
    
    try {
        $cutoffDate = (Get-Date).AddDays(-$config.BackupRetentionDays)
        
        Get-ChildItem $config.BackupPath -Directory | 
        Where-Object { $_.LastWriteTime -lt $cutoffDate } | 
        Remove-Item -Recurse -Force
        
        Write-Log "Backups antigos removidos com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao limpar backups antigos: $_" "ERROR"
        return $false
    }
}

# Função para enviar notificação
function Send-BackupNotification {
    param(
        [string]$BackupDir,
        [string]$DatabaseBackup,
        [string]$FilesBackup
    )
    
    Write-Log "Enviando notificação de backup..."
    
    try {
        $message = @"
Backup WYDBR2.0 concluído com sucesso!

Data: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
Diretório: $BackupDir
Banco de Dados: $DatabaseBackup
Arquivos: $FilesBackup

Tamanho total: $((Get-ChildItem $BackupDir -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB) MB
"@
        
        # Enviar email
        if ($config.BackupEmail) {
            Send-MailMessage -To $config.BackupEmail -Subject "Backup WYDBR2.0" -Body $message
        }
        
        # Enviar Discord
        if ($config.BackupDiscord) {
            $discordBody = @{
                content = $message
            } | ConvertTo-Json
            
            Invoke-RestMethod -Uri $config.BackupDiscord -Method Post -Body $discordBody -ContentType "application/json"
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
function Start-Backup {
    Write-Log "Iniciando processo de backup..."
    
    # Criar diretório de backup
    $backupDir = New-BackupDirectory
    if (-not $backupDir) {
        Write-Log "Falha ao criar diretório de backup. Abortando." "ERROR"
        return $false
    }
    
    # Backup do banco de dados
    $databaseBackup = Backup-Database -BackupDir $backupDir
    if (-not $databaseBackup) {
        Write-Log "Falha no backup do banco de dados. Abortando." "ERROR"
        return $false
    }
    
    # Backup dos arquivos
    $filesBackup = Backup-Files -BackupDir $backupDir
    if (-not $filesBackup) {
        Write-Log "Falha no backup dos arquivos. Abortando." "ERROR"
        return $false
    }
    
    # Criptografar backups
    if ($config.BackupEncryption) {
        $databaseBackup = Protect-Backup -BackupFile $databaseBackup
        $filesBackup = Protect-Backup -BackupFile $filesBackup
    }
    
    # Limpar backups antigos
    if ($config.BackupCleanup) {
        Clear-OldBackups
    }
    
    # Enviar notificação
    Send-BackupNotification -BackupDir $backupDir -DatabaseBackup $databaseBackup -FilesBackup $filesBackup
    
    Write-Log "Processo de backup concluído com sucesso!"
    return $true
}

# Executar backup
Start-Backup 