# WYDBR2.0 Server Setup Script
# Autor: Claude
# Data: 2024

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    ServerPort = 8080
    MaxConnections = 1000
    LogLevel = "INFO"
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    DatabaseHost = "localhost"
    DatabasePort = 3306
    ServerName = "WYDBR2.0"
    ServerVersion = "2.0.0"
    ServerDescription = "Servidor WYDBR2.0 - With Your Destiny Brazil"
    ServerWebsite = "https://www.wydbr2.com"
    ServerDiscord = "https://discord.gg/wydbr2"
    ServerEmail = "admin@wydbr2.com"
    ServerTimezone = "America/Sao_Paulo"
    ServerLanguage = "pt_BR"
    ServerCurrency = "BRL"
    ServerExpRate = 1.0
    ServerDropRate = 1.0
    ServerGoldRate = 1.0
    ServerMaxLevel = 99
    ServerMaxStats = 255
    ServerMaxInventory = 50
    ServerMaxWarehouse = 100
    ServerMaxGuildMembers = 50
    ServerMaxPartyMembers = 8
    ServerMaxFriends = 50
    ServerMaxBlocked = 50
    ServerMaxMail = 50
    ServerMaxQuest = 50
    ServerMaxAchievement = 50
    ServerMaxPet = 5
    ServerMaxMount = 5
    ServerMaxSkill = 50
    ServerMaxItem = 1000
    ServerMaxNPC = 100
    ServerMaxMonster = 100
    ServerMaxMap = 100
    ServerMaxEvent = 10
    ServerMaxGuild = 100
    ServerMaxParty = 100
    ServerMaxTrade = 100
    ServerMaxAuction = 100
    ServerMaxTicket = 100
    ServerMaxLog = 1000
    ServerMaxBackup = 10
    ServerMaxLogDays = 7
    ServerMaxBackupDays = 30
    ServerMaxConnectionsPerIP = 3
    ServerMaxConnectionsPerAccount = 1
    ServerMaxConnectionsPerCharacter = 1
    ServerMaxConnectionsPerGuild = 50
    ServerMaxConnectionsPerParty = 8
    ServerMaxConnectionsPerTrade = 2
    ServerMaxConnectionsPerAuction = 100
    ServerMaxConnectionsPerMail = 50
    ServerMaxConnectionsPerTicket = 100
    ServerMaxConnectionsPerLog = 1000
    ServerMaxConnectionsPerBackup = 10
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
    Add-Content -Path "$($config.ProjectRoot)\logs\server_setup.log" -Value $logMessage
}

# Função para criar arquivo de configuração
function New-ServerConfig {
    Write-Log "Criando arquivo de configuração do servidor..."
    
    try {
        $serverConfig = @{
            server = @{
                name = $config.ServerName
                version = $config.ServerVersion
                description = $config.ServerDescription
                website = $config.ServerWebsite
                discord = $config.ServerDiscord
                email = $config.ServerEmail
                timezone = $config.ServerTimezone
                language = $config.ServerLanguage
                currency = $config.ServerCurrency
            }
            network = @{
                port = $config.ServerPort
                max_connections = $config.MaxConnections
                max_connections_per_ip = $config.ServerMaxConnectionsPerIP
                max_connections_per_account = $config.ServerMaxConnectionsPerAccount
                max_connections_per_character = $config.ServerMaxConnectionsPerCharacter
            }
            database = @{
                name = $config.DatabaseName
                user = $config.DatabaseUser
                password = $config.DatabasePassword
                host = $config.DatabaseHost
                port = $config.DatabasePort
            }
            game = @{
                exp_rate = $config.ServerExpRate
                drop_rate = $config.ServerDropRate
                gold_rate = $config.ServerGoldRate
                max_level = $config.ServerMaxLevel
                max_stats = $config.ServerMaxStats
            }
            limits = @{
                inventory = $config.ServerMaxInventory
                warehouse = $config.ServerMaxWarehouse
                guild_members = $config.ServerMaxGuildMembers
                party_members = $config.ServerMaxPartyMembers
                friends = $config.ServerMaxFriends
                blocked = $config.ServerMaxBlocked
                mail_system = $config.ServerMaxMail
                quest = $config.ServerMaxQuest
                achievement = $config.ServerMaxAchievement
                pet = $config.ServerMaxPet
                mount = $config.ServerMaxMount
                skill = $config.ServerMaxSkill
                item = $config.ServerMaxItem
                npc = $config.ServerMaxNPC
                monster = $config.ServerMaxMonster
                map = $config.ServerMaxMap
                event = $config.ServerMaxEvent
                guild = $config.ServerMaxGuild
                party = $config.ServerMaxParty
                trade = $config.ServerMaxTrade
                auction = $config.ServerMaxAuction
                ticket = $config.ServerMaxTicket
                log = $config.ServerMaxLog
                backup = $config.ServerMaxBackup
            }
            retention = @{
                log_days = $config.ServerMaxLogDays
                backup_days = $config.ServerMaxBackupDays
            }
            logging = @{
                level = $config.LogLevel
                file = "$($config.ProjectRoot)\logs\server.log"
                max_size = "100MB"
                max_files = 10
            }
        }
        
        $serverConfig | ConvertTo-Json -Depth 10 | Set-Content "$($config.ProjectRoot)\config\server.json"
        
        Write-Log "Arquivo de configuração criado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao criar arquivo de configuração: $_" "ERROR"
        return $false
    }
}

# Função para criar diretórios
function New-ServerDirectories {
    Write-Log "Criando diretórios do servidor..."
    
    try {
        $directories = @(
            "$($config.ProjectRoot)\logs",
            "$($config.ProjectRoot)\config",
            "$($config.ProjectRoot)\data",
            "$($config.ProjectRoot)\data\maps",
            "$($config.ProjectRoot)\data\items",
            "$($config.ProjectRoot)\data\npcs",
            "$($config.ProjectRoot)\data\monsters",
            "$($config.ProjectRoot)\data\skills",
            "$($config.ProjectRoot)\data\quests",
            "$($config.ProjectRoot)\data\events",
            "$($config.ProjectRoot)\data\achievements",
            "$($config.ProjectRoot)\data\pets",
            "$($config.ProjectRoot)\data\mounts",
            "$($config.ProjectRoot)\data\guilds",
            "$($config.ProjectRoot)\data\parties",
            "$($config.ProjectRoot)\data\trades",
            "$($config.ProjectRoot)\data\auctions",
            "$($config.ProjectRoot)\data\mails",
            "$($config.ProjectRoot)\data\tickets",
            "$($config.ProjectRoot)\data\logs",
            "$($config.ProjectRoot)\data\backups"
        )
        
        foreach ($dir in $directories) {
            if (-not (Test-Path $dir)) {
                New-Item -ItemType Directory -Path $dir
                Write-Log "Diretório criado: $dir"
            }
        }
        
        Write-Log "Diretórios criados com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao criar diretórios: $_" "ERROR"
        return $false
    }
}

# Função para configurar firewall
function Set-FirewallRules {
    Write-Log "Configurando regras do firewall..."
    
    try {
        # Regra para o servidor
        New-NetFirewallRule -DisplayName "WYDBR2.0 Server" -Direction Inbound -Protocol TCP -LocalPort $config.ServerPort -Action Allow
        
        Write-Log "Regras do firewall configuradas com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao configurar regras do firewall: $_" "ERROR"
        return $false
    }
}

# Função para configurar serviço
function Set-ServerService {
    Write-Log "Configurando serviço do servidor..."
    
    try {
        $serviceName = "WYDBR2.0"
        $servicePath = "$($config.ProjectRoot)\build\bin\wydbr2.0_server.exe"
        $serviceDescription = $config.ServerDescription
        
        # Remover serviço se existir
        if (Get-Service -Name $serviceName -ErrorAction SilentlyContinue) {
            Stop-Service -Name $serviceName
            Remove-Service -Name $serviceName
        }
        
        # Criar serviço
        New-Service -Name $serviceName -BinaryPathName $servicePath -DisplayName $serviceName -Description $serviceDescription -StartupType Automatic
        
        Write-Log "Serviço configurado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao configurar serviço: $_" "ERROR"
        return $false
    }
}

# Função para configurar tarefas agendadas
function Set-ScheduledTasks {
    Write-Log "Configurando tarefas agendadas..."
    
    try {
        # Backup diário
        $action = New-ScheduledTaskAction -Execute "powershell.exe" -Argument "-File `"$($config.ProjectRoot)\scripts\backup.ps1`""
        $trigger = New-ScheduledTaskTrigger -Daily -At 3AM
        $settings = New-ScheduledTaskSettingsSet -StartWhenAvailable -DontStopOnIdleEnd
        
        Register-ScheduledTask -TaskName "WYDBR2.0_Backup" -Action $action -Trigger $trigger -Settings $settings -Force
        
        # Verificação de saúde
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

# Função principal
function Start-ServerSetup {
    Write-Log "Iniciando configuração do servidor..."
    
    # Criar arquivo de configuração
    if (-not (New-ServerConfig)) {
        Write-Log "Falha ao criar arquivo de configuração. Abortando configuração." "ERROR"
        return $false
    }
    
    # Criar diretórios
    if (-not (New-ServerDirectories)) {
        Write-Log "Falha ao criar diretórios. Abortando configuração." "ERROR"
        return $false
    }
    
    # Configurar firewall
    if (-not (Set-FirewallRules)) {
        Write-Log "Falha ao configurar firewall." "WARNING"
    }
    
    # Configurar serviço
    if (-not (Set-ServerService)) {
        Write-Log "Falha ao configurar serviço." "WARNING"
    }
    
    # Configurar tarefas agendadas
    if (-not (Set-ScheduledTasks)) {
        Write-Log "Falha ao configurar tarefas agendadas." "WARNING"
    }
    
    Write-Log "Configuração do servidor concluída!"
    return $true
}

# Executar configuração
Start-ServerSetup 