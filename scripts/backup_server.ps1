# WYDBR2.0 Backup Script
# Requer execução como Administrador
#Requires -RunAsAdministrator

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    BackupRoot = "F:\Backups\WYDBR2.0"
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    DatabaseHost = "localhost"
    DatabasePort = 3306
    RetentionDays = 30
    CompressBackups = $true
    BackupSchedule = @{
        Full = "0 0 * * 0"  # Todo domingo à meia-noite
        Incremental = "0 0 * * 1-6"  # Segunda a sábado à meia-noite
        Differential = "0 12 * * *"  # Todo dia ao meio-dia
    }
    EmailNotification = "admin@wydbr2.com"
    DiscordWebhook = "https://discord.com/api/webhooks/your_webhook_url"
    NotificationsEnabled = $true
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
    
    # Criar diretório de logs se não existir
    $logDir = Join-Path $config.ProjectRoot "logs"
    if (-not (Test-Path $logDir)) {
        New-Item -ItemType Directory -Path $logDir -Force | Out-Null
    }
    
    Add-Content -Path "$logDir\backup.log" -Value $logMessage
}

# Verificar execução como administrador
Write-Log "Verificando privilégios de administrador..."
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Log "Este script precisa ser executado como Administrador!" "ERROR"
    exit 1
}

# Função para criar diretórios de backup
function Initialize-BackupDirectories {
    Write-Log "Inicializando diretórios de backup..."
    
    try {
        $directories = @(
            $config.BackupRoot,
            "$($config.BackupRoot)\full",
            "$($config.BackupRoot)\incremental",
            "$($config.BackupRoot)\differential",
            "$($config.BackupRoot)\logs"
        )
        
        foreach ($dir in $directories) {
            if (-not (Test-Path $dir)) {
                New-Item -ItemType Directory -Path $dir -Force | Out-Null
                Write-Log "Diretório criado: $dir"
            }
        }
        
        Write-Log "Diretórios de backup inicializados com sucesso"
        return $true
    }
    catch {
        Write-Log "Erro ao criar diretórios de backup: $_" "ERROR"
        return $false
    }
}

# Função para backup do banco de dados
function Backup-Database {
    param(
        [string]$BackupType = "full"
    )
    
    Write-Log "Iniciando backup $BackupType do banco de dados..."
    
    try {
        $timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
        $backupDir = "$($config.BackupRoot)\$BackupType"
        $backupFile = "$backupDir\db_backup_${BackupType}_$timestamp.sql"
        
        # Criar backup
        mysqldump --host=$($config.DatabaseHost) --port=$($config.DatabasePort) --user=$($config.DatabaseUser) --password=$($config.DatabasePassword) $($config.DatabaseName) > $backupFile
        
        if ($LASTEXITCODE -eq 0) {
            # Comprimir backup
            if ($config.CompressBackups) {
                Compress-Archive -Path $backupFile -DestinationPath "$backupFile.zip" -Force
                Remove-Item $backupFile -Force
                $backupFile = "$backupFile.zip"
            }
            
            Write-Log "Backup do banco de dados concluído: $backupFile"
            return $backupFile
        }
        else {
            Write-Log "Erro ao criar backup do banco de dados" "ERROR"
            return $null
        }
    }
    catch {
        Write-Log "Erro durante o backup do banco de dados: $_" "ERROR"
        return $null
    }
}

# Função para backup de arquivos
function Backup-Files {
    param(
        [string]$BackupType = "full"
    )
    
    Write-Log "Iniciando backup $BackupType dos arquivos..."
    
    try {
        $timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
        $backupDir = "$($config.BackupRoot)\$BackupType"
        $backupFile = "$backupDir\files_backup_${BackupType}_$timestamp.zip"
        
        # Definir arquivos para backup
        $filesToBackup = @(
            "$($config.ProjectRoot)\config",
            "$($config.ProjectRoot)\data",
            "$($config.ProjectRoot)\logs"
        )
        
        # Criar backup
        Compress-Archive -Path $filesToBackup -DestinationPath $backupFile -Force
        
        if (Test-Path $backupFile) {
            Write-Log "Backup de arquivos concluído: $backupFile"
            return $backupFile
        }
        else {
            Write-Log "Erro ao criar backup de arquivos" "ERROR"
            return $null
        }
    }
    catch {
        Write-Log "Erro durante o backup de arquivos: $_" "ERROR"
        return $null
    }
}

# Função para limpar backups antigos
function Clear-OldBackups {
    Write-Log "Limpando backups antigos..."
    
    try {
        $cutoffDate = (Get-Date).AddDays(-$config.RetentionDays)
        
        $backupTypes = @("full", "incremental", "differential")
        foreach ($type in $backupTypes) {
            $backupDir = "$($config.BackupRoot)\$type"
            
            Get-ChildItem $backupDir -File | Where-Object {
                $_.LastWriteTime -lt $cutoffDate
            } | ForEach-Object {
                Remove-Item $_.FullName -Force
                Write-Log "Backup removido: $($_.Name)"
            }
        }
        
        Write-Log "Limpeza de backups concluída"
    }
    catch {
        Write-Log "Erro ao limpar backups antigos: $_" "ERROR"
    }
}

# Função para enviar notificações
function Send-BackupNotification {
    param(
        [string]$BackupType,
        [string[]]$BackupFiles,
        [bool]$Success
    )
    
    if (-not $config.NotificationsEnabled) {
        return
    }
    
    $status = if ($Success) { "Sucesso" } else { "Falha" }
    $message = "Status do Backup WYDBR2.0 ($BackupType)`n`n"
    $message += "Status: $status`n"
    $message += "Data: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')`n"
    
    if ($Success) {
        $message += "`nArquivos:`n"
        foreach ($file in $BackupFiles) {
            $message += "- $file`n"
        }
    }
    
    try {
        # Enviar email
        if ($config.EmailNotification) {
            Send-MailMessage -To $config.EmailNotification -Subject "Backup WYDBR2.0 - $status" -Body $message
        }
        
        # Enviar Discord
        if ($config.DiscordWebhook) {
            $discordBody = @{
                content = $message
            } | ConvertTo-Json
            
            Invoke-RestMethod -Uri $config.DiscordWebhook -Method Post -Body $discordBody -ContentType "application/json"
        }
        
        Write-Log "Notificações enviadas com sucesso"
    }
    catch {
        Write-Log "Erro ao enviar notificações: $_" "ERROR"
    }
}

# Função principal de backup
function Start-Backup {
    param(
        [string]$BackupType = "full"
    )
    
    Write-Log "Iniciando processo de backup $BackupType..."
    
    # Inicializar diretórios
    if (-not (Initialize-BackupDirectories)) {
        return
    }
    
    $backupFiles = @()
    $success = $true
    
    # Backup do banco de dados
    $dbBackup = Backup-Database -BackupType $BackupType
    if ($dbBackup) {
        $backupFiles += $dbBackup
    }
    else {
        $success = $false
    }
    
    # Backup de arquivos
    $filesBackup = Backup-Files -BackupType $BackupType
    if ($filesBackup) {
        $backupFiles += $filesBackup
    }
    else {
        $success = $false
    }
    
    # Limpar backups antigos
    Clear-OldBackups
    
    # Enviar notificações
    Send-BackupNotification -BackupType $BackupType -BackupFiles $backupFiles -Success $success
    
    if ($success) {
        Write-Log "Processo de backup $BackupType concluído com sucesso"
    }
    else {
        Write-Log "Processo de backup $BackupType concluído com erros" "ERROR"
    }
}

# Determinar tipo de backup baseado no dia
$today = Get-Date
$backupType = switch ($today.DayOfWeek) {
    "Sunday" { "full" }
    { $_ -in "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" } {
        if ($today.Hour -eq 12) { "differential" } else { "incremental" }
    }
}

# Executar backup
Start-Backup -BackupType $backupType 