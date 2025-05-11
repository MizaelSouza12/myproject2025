# WYDBR2.0 Backup Script
# Autor: Claude
# Data: 2024

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    BackupPath = "F:\Backups\WYDBR2.0"
    RetentionDays = 30
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
        
        # Comprimir backup
        Compress-Archive -Path $backupFile -DestinationPath "$backupFile.zip"
        Remove-Item $backupFile
        
        Write-Log "Backup concluído: $backupFile.zip"
        return $true
    }
    catch {
        Write-Log "Erro durante o backup: $_" "ERROR"
        return $false
    }
}

# Função para limpar backups antigos
function Clear-OldBackups {
    Write-Log "Limpando backups antigos..."
    
    try {
        $backups = Get-ChildItem $config.BackupPath -Filter "backup_*.zip"
        $cutoffDate = (Get-Date).AddDays(-$config.RetentionDays)
        
        foreach ($backup in $backups) {
            if ($backup.LastWriteTime -lt $cutoffDate) {
                Remove-Item $backup.FullName
                Write-Log "Backup antigo removido: $($backup.Name)"
            }
        }
        
        Write-Log "Limpeza de backups concluída."
        return $true
    }
    catch {
        Write-Log "Erro ao limpar backups antigos: $_" "ERROR"
        return $false
    }
}

# Função para verificar espaço em disco
function Test-DiskSpace {
    Write-Log "Verificando espaço em disco..."
    
    try {
        $drive = (Get-PSDrive -PSProvider FileSystem | Where-Object {$_.Root -eq (Split-Path $config.BackupPath -Qualifier)})
        $freeSpaceGB = [math]::Round($drive.Free / 1GB, 2)
        
        if ($freeSpaceGB -lt 10) {
            Write-Log "Espaço em disco crítico: $freeSpaceGB GB livre" "WARNING"
            return $false
        }
        
        Write-Log "Espaço em disco OK: $freeSpaceGB GB livre"
        return $true
    }
    catch {
        Write-Log "Erro ao verificar espaço em disco: $_" "ERROR"
        return $false
    }
}

# Função principal
function Start-Backup {
    Write-Log "Iniciando processo de backup..."
    
    # Verificar espaço em disco
    if (-not (Test-DiskSpace)) {
        Write-Log "Espaço em disco insuficiente. Abortando backup." "ERROR"
        return $false
    }
    
    # Realizar backup
    if (-not (Start-DatabaseBackup)) {
        Write-Log "Falha no backup do banco de dados." "ERROR"
        return $false
    }
    
    # Limpar backups antigos
    if (-not (Clear-OldBackups)) {
        Write-Log "Falha ao limpar backups antigos." "WARNING"
    }
    
    Write-Log "Processo de backup concluído com sucesso!"
    return $true
}

# Executar backup
Start-Backup 