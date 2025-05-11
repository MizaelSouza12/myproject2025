# WYDBR2.0 Server Update Script
# Autor: Claude
# Data: 2024

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    GitRepository = "https://github.com/your-username/wydbr2.0.git"
    GitBranch = "main"
    BuildType = "Release"
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    DatabaseHost = "localhost"
    DatabasePort = 3306
    ServerPort = 8080
    UpdateEmail = "admin@wydbr2.com"
    UpdateDiscord = "https://discord.com/api/webhooks/your_webhook_url"
    UpdateEnabled = $true
    UpdateSchedule = "Weekly"
    UpdateDay = "Sunday"
    UpdateTime = "04:00"
    UpdateVerify = $true
    UpdateRollback = $true
    UpdateBackup = $true
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
    Add-Content -Path "$($config.ProjectRoot)\logs\update.log" -Value $logMessage
}

# Função para verificar atualizações
function Test-UpdateAvailable {
    Write-Log "Verificando atualizações disponíveis..."
    
    try {
        # Mudar para o diretório do projeto
        Set-Location $config.ProjectRoot
        
        # Atualizar repositório
        git fetch origin $config.GitBranch
        
        # Verificar se há atualizações
        $localCommit = git rev-parse HEAD
        $remoteCommit = git rev-parse "origin/$($config.GitBranch)"
        
        if ($localCommit -ne $remoteCommit) {
            Write-Log "Atualização disponível: $remoteCommit"
            return $true
        }
        
        Write-Log "Nenhuma atualização disponível."
        return $false
    }
    catch {
        Write-Log "Erro ao verificar atualizações: $_" "ERROR"
        return $false
    }
}

# Função para fazer backup antes da atualização
function Backup-BeforeUpdate {
    Write-Log "Fazendo backup antes da atualização..."
    
    try {
        # Executar script de backup
        & "$($config.ProjectRoot)\scripts\backup_server.ps1"
        
        if ($LASTEXITCODE -eq 0) {
            Write-Log "Backup concluído com sucesso."
            return $true
        }
        else {
            Write-Log "Erro ao fazer backup." "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro ao fazer backup: $_" "ERROR"
        return $false
    }
}

# Função para atualizar código
function Update-Code {
    Write-Log "Atualizando código..."
    
    try {
        # Mudar para o diretório do projeto
        Set-Location $config.ProjectRoot
        
        # Fazer stash de alterações locais
        git stash
        
        # Atualizar código
        git pull origin $config.GitBranch
        
        # Aplicar stash se houver
        git stash pop
        
        Write-Log "Código atualizado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao atualizar código: $_" "ERROR"
        return $false
    }
}

# Função para compilar projeto
function Build-Project {
    Write-Log "Compilando projeto..."
    
    try {
        # Criar diretório de build se não existir
        if (-not (Test-Path "$($config.ProjectRoot)\build")) {
            New-Item -ItemType Directory -Path "$($config.ProjectRoot)\build"
        }
        
        # Mudar para o diretório de build
        Set-Location "$($config.ProjectRoot)\build"
        
        # Configurar CMake
        cmake -DCMAKE_BUILD_TYPE=$config.BuildType ..
        
        # Compilar
        cmake --build . --config $config.BuildType
        
        if ($LASTEXITCODE -eq 0) {
            Write-Log "Projeto compilado com sucesso."
            return $true
        }
        else {
            Write-Log "Erro ao compilar projeto." "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro ao compilar projeto: $_" "ERROR"
        return $false
    }
}

# Função para atualizar banco de dados
function Update-Database {
    Write-Log "Atualizando banco de dados..."
    
    try {
        # Executar scripts de migração
        $migrationScripts = Get-ChildItem "$($config.ProjectRoot)\database\migrations" -Filter "*.sql" | Sort-Object Name
        
        foreach ($script in $migrationScripts) {
            Write-Log "Executando script: $($script.Name)"
            
            $query = Get-Content $script.FullName -Raw
            Invoke-MySQLQuery -Query $query
            
            if ($LASTEXITCODE -eq 0) {
                Write-Log "Script executado com sucesso: $($script.Name)"
            }
            else {
                Write-Log "Erro ao executar script: $($script.Name)" "ERROR"
                return $false
            }
        }
        
        Write-Log "Banco de dados atualizado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao atualizar banco de dados: $_" "ERROR"
        return $false
    }
}

# Função para verificar atualização
function Test-Update {
    Write-Log "Verificando atualização..."
    
    try {
        # Verificar se o servidor está rodando
        $serverProcess = Get-Process -Name "wydbr2.0_server" -ErrorAction SilentlyContinue
        
        if ($serverProcess) {
            Write-Log "Servidor está rodando."
            return $true
        }
        else {
            Write-Log "Servidor não está rodando." "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Erro ao verificar atualização: $_" "ERROR"
        return $false
    }
}

# Função para fazer rollback
function Start-Rollback {
    Write-Log "Iniciando rollback..."
    
    try {
        # Parar servidor
        Stop-Service -Name "WYDBR2.0" -Force
        
        # Restaurar backup
        & "$($config.ProjectRoot)\scripts\restore_backup.ps1"
        
        # Iniciar servidor
        Start-Service -Name "WYDBR2.0"
        
        Write-Log "Rollback concluído com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao fazer rollback: $_" "ERROR"
        return $false
    }
}

# Função para enviar notificação
function Send-UpdateNotification {
    param(
        [bool]$Success,
        [string]$Message
    )
    
    Write-Log "Enviando notificação de atualização..."
    
    try {
        $status = if ($Success) { "SUCESSO" } else { "FALHA" }
        $notification = @"
Atualização WYDBR2.0: $status

Data: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
Mensagem: $Message

Detalhes:
- Código: $(git rev-parse HEAD)
- Branch: $($config.GitBranch)
- Build: $($config.BuildType)
"@
        
        # Enviar email
        if ($config.UpdateEmail) {
            Send-MailMessage -To $config.UpdateEmail -Subject "Atualização WYDBR2.0: $status" -Body $notification
        }
        
        # Enviar Discord
        if ($config.UpdateDiscord) {
            $discordBody = @{
                content = $notification
            } | ConvertTo-Json
            
            Invoke-RestMethod -Uri $config.UpdateDiscord -Method Post -Body $discordBody -ContentType "application/json"
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
function Start-Update {
    Write-Log "Iniciando processo de atualização..."
    
    # Verificar atualizações
    if (-not (Test-UpdateAvailable)) {
        Write-Log "Nenhuma atualização disponível. Abortando."
        return $false
    }
    
    # Fazer backup
    if ($config.UpdateBackup) {
        if (-not (Backup-BeforeUpdate)) {
            Write-Log "Falha ao fazer backup. Abortando." "ERROR"
            return $false
        }
    }
    
    # Atualizar código
    if (-not (Update-Code)) {
        Write-Log "Falha ao atualizar código. Abortando." "ERROR"
        return $false
    }
    
    # Compilar projeto
    if (-not (Build-Project)) {
        Write-Log "Falha ao compilar projeto. Abortando." "ERROR"
        return $false
    }
    
    # Atualizar banco de dados
    if (-not (Update-Database)) {
        Write-Log "Falha ao atualizar banco de dados. Abortando." "ERROR"
        return $false
    }
    
    # Verificar atualização
    if ($config.UpdateVerify) {
        if (-not (Test-Update)) {
            Write-Log "Falha na verificação da atualização."
            
            # Fazer rollback
            if ($config.UpdateRollback) {
                if (-not (Start-Rollback)) {
                    Write-Log "Falha ao fazer rollback." "ERROR"
                }
            }
            
            Send-UpdateNotification -Success $false -Message "Falha na verificação da atualização"
            return $false
        }
    }
    
    # Enviar notificação
    Send-UpdateNotification -Success $true -Message "Atualização concluída com sucesso"
    
    Write-Log "Processo de atualização concluído com sucesso!"
    return $true
}

# Executar atualização
Start-Update 