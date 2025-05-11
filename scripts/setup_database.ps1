# WYDBR2.0 Database Setup Script
# Autor: Claude
# Data: 2024

# Configurações
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    DatabaseRootPassword = "your_root_password"
    DatabaseHost = "localhost"
    DatabasePort = 3306
    CharacterSet = "utf8mb4"
    Collation = "utf8mb4_unicode_ci"
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
    Add-Content -Path "$($config.ProjectRoot)\logs\database_setup.log" -Value $logMessage
}

# Função para executar comando MySQL
function Invoke-MySQLCommand {
    param(
        [string]$Command,
        [string]$User = $config.DatabaseUser,
        [string]$Password = $config.DatabasePassword
    )
    
    try {
        $result = mysql -u $User -p$Password -e $Command
        return $result
    }
    catch {
        Write-Log "Erro ao executar comando MySQL: $_" "ERROR"
        return $null
    }
}

# Função para criar banco de dados
function New-Database {
    Write-Log "Criando banco de dados..."
    
    try {
        $command = "CREATE DATABASE IF NOT EXISTS $($config.DatabaseName) CHARACTER SET $($config.CharacterSet) COLLATE $($config.Collation);"
        Invoke-MySQLCommand -Command $command -User "root" -Password $config.DatabaseRootPassword
        
        Write-Log "Banco de dados criado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao criar banco de dados: $_" "ERROR"
        return $false
    }
}

# Função para criar usuário
function New-DatabaseUser {
    Write-Log "Criando usuário do banco de dados..."
    
    try {
        $commands = @(
            "CREATE USER IF NOT EXISTS '$($config.DatabaseUser)'@'$($config.DatabaseHost)' IDENTIFIED BY '$($config.DatabasePassword)';",
            "GRANT ALL PRIVILEGES ON $($config.DatabaseName).* TO '$($config.DatabaseUser)'@'$($config.DatabaseHost)';",
            "FLUSH PRIVILEGES;"
        )
        
        foreach ($command in $commands) {
            Invoke-MySQLCommand -Command $command -User "root" -Password $config.DatabaseRootPassword
        }
        
        Write-Log "Usuário criado com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao criar usuário: $_" "ERROR"
        return $false
    }
}

# Função para executar scripts SQL
function Invoke-SQLScripts {
    Write-Log "Executando scripts SQL..."
    
    try {
        $scripts = Get-ChildItem "$($config.ProjectRoot)\database\migrations" -Filter "*.sql" | Sort-Object Name
        
        foreach ($script in $scripts) {
            Write-Log "Executando script: $($script.Name)"
            mysql -u $config.DatabaseUser -p$config.DatabasePassword $config.DatabaseName < $script.FullName
        }
        
        Write-Log "Scripts SQL executados com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao executar scripts SQL: $_" "ERROR"
        return $false
    }
}

# Função para inserir dados iniciais
function Add-InitialData {
    Write-Log "Inserindo dados iniciais..."
    
    try {
        $commands = @(
            # Configurações iniciais
            "INSERT INTO configs (key, value, updated_at) VALUES ('server_name', 'WYDBR2.0', NOW());",
            "INSERT INTO configs (key, value, updated_at) VALUES ('max_players', '1000', NOW());",
            "INSERT INTO configs (key, value, updated_at) VALUES ('exp_rate', '1.0', NOW());",
            "INSERT INTO configs (key, value, updated_at) VALUES ('drop_rate', '1.0', NOW());",
            
            # Itens iniciais
            "INSERT INTO items (name, type, level, price, created_at) VALUES ('Espada Inicial', 1, 1, 100, NOW());",
            "INSERT INTO items (name, type, level, price, created_at) VALUES ('Armadura Inicial', 2, 1, 100, NOW());",
            
            # NPCs iniciais
            "INSERT INTO npcs (name, type, level, created_at) VALUES ('Mestre de Armas', 1, 1, NOW());",
            "INSERT INTO npcs (name, type, level, created_at) VALUES ('Mercador', 2, 1, NOW());",
            
            # Missões iniciais
            "INSERT INTO quests (name, description, level, created_at) VALUES ('Tutorial', 'Aprenda os básicos do jogo', 1, NOW());",
            "INSERT INTO quests (name, description, level, created_at) VALUES ('Primeira Missão', 'Derrote 10 monstros', 1, NOW());"
        )
        
        foreach ($command in $commands) {
            Invoke-MySQLCommand -Command $command
        }
        
        Write-Log "Dados iniciais inseridos com sucesso."
        return $true
    }
    catch {
        Write-Log "Erro ao inserir dados iniciais: $_" "ERROR"
        return $false
    }
}

# Função para verificar integridade
function Test-DatabaseIntegrity {
    Write-Log "Verificando integridade do banco de dados..."
    
    try {
        $tables = @(
            "accounts",
            "characters",
            "inventories",
            "items",
            "npcs",
            "quests",
            "guilds",
            "chat_messages",
            "events"
        )
        
        foreach ($table in $tables) {
            $command = "SELECT COUNT(*) FROM $table;"
            $count = Invoke-MySQLCommand -Command $command
            Write-Log "Tabela $table: $count registros"
        }
        
        Write-Log "Verificação de integridade concluída."
        return $true
    }
    catch {
        Write-Log "Erro ao verificar integridade: $_" "ERROR"
        return $false
    }
}

# Função para otimizar banco de dados
function Optimize-Database {
    Write-Log "Otimizando banco de dados..."
    
    try {
        $command = "OPTIMIZE TABLE accounts, characters, inventories, items, npcs, quests, guilds, chat_messages, events;"
        Invoke-MySQLCommand -Command $command
        
        Write-Log "Otimização concluída."
        return $true
    }
    catch {
        Write-Log "Erro ao otimizar banco de dados: $_" "ERROR"
        return $false
    }
}

# Função principal
function Start-DatabaseSetup {
    Write-Log "Iniciando configuração do banco de dados..."
    
    # Criar banco de dados
    if (-not (New-Database)) {
        Write-Log "Falha ao criar banco de dados. Abortando configuração." "ERROR"
        return $false
    }
    
    # Criar usuário
    if (-not (New-DatabaseUser)) {
        Write-Log "Falha ao criar usuário. Abortando configuração." "ERROR"
        return $false
    }
    
    # Executar scripts SQL
    if (-not (Invoke-SQLScripts)) {
        Write-Log "Falha ao executar scripts SQL. Abortando configuração." "ERROR"
        return $false
    }
    
    # Inserir dados iniciais
    if (-not (Add-InitialData)) {
        Write-Log "Falha ao inserir dados iniciais." "WARNING"
    }
    
    # Verificar integridade
    if (-not (Test-DatabaseIntegrity)) {
        Write-Log "Falha na verificação de integridade." "WARNING"
    }
    
    # Otimizar banco de dados
    if (-not (Optimize-Database)) {
        Write-Log "Falha ao otimizar banco de dados." "WARNING"
    }
    
    Write-Log "Configuração do banco de dados concluída!"
    return $true
}

# Executar configuração
Start-DatabaseSetup 