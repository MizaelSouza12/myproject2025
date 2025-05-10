# Plano de Integração TMSrv/DBSrv no WYDStudio

## Visão Geral

Este documento detalha o plano para integrar completamente o TMSrv (servidor de jogo) e o DBSrv (servidor de banco de dados) no WYDStudio (ferramenta administrativa) para o WYDBR 2.0, seguindo a arquitetura original em C++ e aplicando melhorias de segurança e correções necessárias.

## Objetivos

1. **Integração Completa**: TMSrv e DBSrv devem ser componentes internos do WYDStudio, não serviços externos
2. **Controle Centralizado**: Interface única para gerenciamento, monitoramento e configuração
3. **Compartilhamento de Recursos**: Compartilhamento eficiente de memória e processamento
4. **Segurança Aprimorada**: Comunicação interna segura entre componentes
5. **Facilidade de Uso**: Simplificação do processo de administração

## Arquitetura de Integração

### Modelo de Componentes

```
+-------------------------------------------+
|                 WYDStudio                 |
|                                           |
|  +---------------+    +---------------+   |
|  |     TMSrv     |<-->|     DBSrv     |   |
|  +---------------+    +---------------+   |
|           ^                   ^           |
|           |                   |           |
|  +---------------+    +---------------+   |
|  |   Sistema     |    |    Sistema    |   |
|  |     VoIP      |    | Screen Share  |   |
|  +---------------+    +---------------+   |
|                                           |
+-------------------------------------------+
                    ^
                    |
                    v
+-------------------------------------------+
|               Cliente WYDBR               |
+-------------------------------------------+
```

### Fluxo de Dados

1. **Inicialização**:
   - WYDStudio inicializa componentes internos (TMSrv, DBSrv)
   - Carrega configurações centralizadas
   - Estabelece canais de comunicação entre componentes

2. **Operação Normal**:
   - Cliente conecta ao TMSrv através do WYDStudio
   - TMSrv processa lógica de jogo
   - TMSrv consulta DBSrv para operações de dados
   - Administrador interage com TMSrv/DBSrv através da interface WYDStudio

3. **Controle Administrativo**:
   - Interface administrativa do WYDStudio monitora TMSrv/DBSrv
   - Comandos administrativos são processados diretamente
   - Logs e estatísticas são coletados em tempo real

## Detalhes de Implementação

### 1. Ciclo de Vida Compartilhado

#### Inicialização Sincronizada
```cpp
// WYDStudio.cpp
bool WYDStudio::Initialize() {
    // Inicializar componentes internos
    m_tmSrv = std::make_unique<TMSrv>();
    if (!m_tmSrv->Initialize()) {
        return false;
    }
    
    m_dbSrv = std::make_unique<DBSrv>();
    if (!m_dbSrv->Initialize()) {
        return false;
    }
    
    // Estabelecer conexões internas
    m_tmSrv->SetDatabaseService(m_dbSrv.get());
    
    return true;
}
```

#### Encerramento Coordenado
```cpp
// WYDStudio.cpp
void WYDStudio::Shutdown() {
    // Encerrar em ordem específica
    if (m_tmSrv) {
        m_tmSrv->Shutdown();
    }
    
    if (m_dbSrv) {
        m_dbSrv->Shutdown();
    }
    
    // Limpar recursos
    m_tmSrv.reset();
    m_dbSrv.reset();
}
```

### 2. Comunicação Interna

#### Interface de Serviço de Banco de Dados
```cpp
// DBSrv.h
class DBService {
public:
    virtual ~DBService() = default;
    
    virtual bool LoadCharacter(uint32_t characterId, CharacterData& outData) = 0;
    virtual bool SaveCharacter(const CharacterData& data) = 0;
    virtual bool LoadInventory(uint32_t characterId, InventoryData& outData) = 0;
    virtual bool SaveInventory(uint32_t characterId, const InventoryData& data) = 0;
    // Outros métodos de acesso a dados...
};

// DBSrv deve implementar esta interface
class DBSrv : public DBService {
    // Implementação...
};
```

#### Utilização no TMSrv
```cpp
// TMSrv.h
class TMSrv {
public:
    // ...
    void SetDatabaseService(DBService* dbService);
    
private:
    DBService* m_dbService;
    // ...
};

// TMSrv.cpp
void TMSrv::HandlePlayerLogin(ClientID clientId, const LoginRequest& request) {
    // ...
    CharacterData charData;
    if (m_dbService->LoadCharacter(request.characterId, charData)) {
        // Processo de login...
    }
    // ...
}
```

### 3. Configuração Centralizada

#### Sistema de Configuração Unificado
```cpp
// ConfigManager.h
class ConfigManager {
public:
    bool LoadConfig(const std::string& path);
    
    // Getters para TMSrv
    uint16_t GetTMSrvPort() const;
    int GetMaxPlayers() const;
    int GetDropRate() const;
    
    // Getters para DBSrv
    std::string GetDatabaseHost() const;
    std::string GetDatabaseName() const;
    std::string GetDatabaseUser() const;
    std::string GetDatabasePassword() const;
    
private:
    // Dados de configuração
    uint16_t m_tmSrvPort;
    int m_maxPlayers;
    int m_dropRate;
    
    std::string m_dbHost;
    std::string m_dbName;
    std::string m_dbUser;
    std::string m_dbPassword;
};
```

#### Uso no WYDStudio
```cpp
// WYDStudio.cpp
bool WYDStudio::Initialize(const std::string& configPath) {
    // Carregar configuração centralizada
    m_configManager = std::make_unique<ConfigManager>();
    if (!m_configManager->LoadConfig(configPath)) {
        return false;
    }
    
    // Inicializar TMSrv com configuração relevante
    m_tmSrv = std::make_unique<TMSrv>();
    TMSrvConfig tmConfig;
    tmConfig.port = m_configManager->GetTMSrvPort();
    tmConfig.maxPlayers = m_configManager->GetMaxPlayers();
    tmConfig.dropRate = m_configManager->GetDropRate();
    
    if (!m_tmSrv->Initialize(tmConfig)) {
        return false;
    }
    
    // Inicializar DBSrv com configuração relevante
    m_dbSrv = std::make_unique<DBSrv>();
    DBSrvConfig dbConfig;
    dbConfig.host = m_configManager->GetDatabaseHost();
    dbConfig.dbName = m_configManager->GetDatabaseName();
    dbConfig.user = m_configManager->GetDatabaseUser();
    dbConfig.password = m_configManager->GetDatabasePassword();
    
    if (!m_dbSrv->Initialize(dbConfig)) {
        return false;
    }
    
    // Continuar inicialização...
}
```

### 4. Interface Administrativa Unificada

#### Componentes da UI
```cpp
// AdminUI.h
class AdminUI {
public:
    AdminUI(TMSrv* tmSrv, DBSrv* dbSrv);
    
    void Initialize();
    void Show();
    void Update();
    
private:
    TMSrv* m_tmSrv;
    DBSrv* m_dbSrv;
    
    // Componentes da UI
    std::unique_ptr<ServerStatusPanel> m_statusPanel;
    std::unique_ptr<PlayerManagerPanel> m_playerPanel;
    std::unique_ptr<LogViewerPanel> m_logPanel;
    std::unique_ptr<ConfigEditorPanel> m_configPanel;
    std::unique_ptr<DatabaseBrowserPanel> m_dbBrowserPanel;
    
    // Métodos para criar componentes da UI
    void CreateStatusPanel();
    void CreatePlayerManagerPanel();
    void CreateLogViewerPanel();
    void CreateConfigEditorPanel();
    void CreateDatabaseBrowserPanel();
};
```

#### Integração com TMSrv/DBSrv
```cpp
// AdminUI.cpp
void AdminUI::Update() {
    // Atualizar status do servidor
    ServerStatus status = m_tmSrv->GetStatus();
    m_statusPanel->UpdateStatus(status);
    
    // Atualizar lista de jogadores
    const auto& players = m_tmSrv->GetConnectedPlayers();
    m_playerPanel->UpdatePlayerList(players);
    
    // Atualizar logs
    const auto& logs = m_tmSrv->GetRecentLogs();
    m_logPanel->AddLogs(logs);
    
    const auto& dbLogs = m_dbSrv->GetRecentLogs();
    m_logPanel->AddLogs(dbLogs);
    
    // Atualizar outros painéis...
}
```

### 5. Gerenciamento de Memória Compartilhada

#### Pool de Objetos Compartilhado
```cpp
// SharedObjectPool.h
template<typename T>
class SharedObjectPool {
public:
    SharedObjectPool(size_t initialSize);
    
    T* Acquire();
    void Release(T* object);
    
private:
    std::vector<std::unique_ptr<T>> m_pool;
    std::vector<T*> m_available;
    std::mutex m_mutex;
};

// Exemplo de uso
using PacketPool = SharedObjectPool<NetworkPacket>;
```

#### Utilização em TMSrv e DBSrv
```cpp
// WYDStudio.cpp
bool WYDStudio::Initialize() {
    // Criar pools compartilhados
    m_packetPool = std::make_unique<PacketPool>(1000);
    m_characterPool = std::make_unique<SharedObjectPool<CharacterData>>(500);
    
    // Passar para TMSrv e DBSrv
    TMSrvConfig tmConfig;
    tmConfig.packetPool = m_packetPool.get();
    tmConfig.characterPool = m_characterPool.get();
    
    DBSrvConfig dbConfig;
    dbConfig.characterPool = m_characterPool.get();
    
    // Inicializar com as configurações
    m_tmSrv->Initialize(tmConfig);
    m_dbSrv->Initialize(dbConfig);
    
    // ...
}
```

## Segurança e Validação de Dados

### 1. Validação Consistente

Implementar verificações de validação consistentes entre TMSrv e DBSrv para garantir integridade de dados.

```cpp
// ValidationUtils.h
namespace Validation {
    bool IsValidCharacterName(const std::string& name);
    bool IsValidItemID(uint32_t itemId);
    bool IsValidPosition(const Position& pos);
    // Outras funções de validação...
}

// Uso em TMSrv
bool TMSrv::HandleCreateCharacter(ClientID clientId, const CreateCharacterRequest& request) {
    // Validar nome do personagem
    if (!Validation::IsValidCharacterName(request.name)) {
        SendErrorResponse(clientId, "Nome de personagem inválido");
        return false;
    }
    
    // Continuar processamento...
}

// Uso em DBSrv
bool DBSrv::SaveCharacter(const CharacterData& data) {
    // Mesma validação
    if (!Validation::IsValidCharacterName(data.name)) {
        LogError("Tentativa de salvar personagem com nome inválido");
        return false;
    }
    
    // Continuar salvamento...
}
```

### 2. Gerenciamento de Permissões

Implementar sistema unificado de permissões para ações administrativas.

```cpp
// Permission.h
enum class Permission {
    VIEW_PLAYERS,
    KICK_PLAYERS,
    BAN_PLAYERS,
    MODIFY_ITEMS,
    MODIFY_CONFIG,
    VIEW_LOGS,
    FULL_ADMIN
};

class PermissionManager {
public:
    bool HasPermission(uint32_t adminId, Permission permission);
    void GrantPermission(uint32_t adminId, Permission permission);
    void RevokePermission(uint32_t adminId, Permission permission);
    
private:
    std::unordered_map<uint32_t, std::set<Permission>> m_permissions;
};

// Uso na interface administrativa
void AdminUI::OnKickPlayerButton(ClientID clientId) {
    if (!m_permissionManager->HasPermission(m_currentAdminId, Permission::KICK_PLAYERS)) {
        ShowError("Permissão negada");
        return;
    }
    
    m_tmSrv->KickPlayer(clientId);
}
```

## Monitoramento e Logging

### 1. Sistema de Logging Centralizado

```cpp
// LogManager.h
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct LogEntry {
    LogLevel level;
    std::string source;
    std::string message;
    int64_t timestamp;
};

class LogManager {
public:
    void Log(LogLevel level, const std::string& source, const std::string& message);
    std::vector<LogEntry> GetRecentLogs(int count = 100);
    void SetLogFile(const std::string& path);
    
private:
    std::vector<LogEntry> m_recentLogs;
    std::string m_logFilePath;
    std::mutex m_logMutex;
};

// Uso em TMSrv e DBSrv
void TMSrv::ProcessClientRequest(ClientID clientId, const Packet& packet) {
    m_logManager->Log(LogLevel::INFO, "TMSrv", "Processando pacote tipo " + std::to_string(static_cast<int>(packet.GetType())));
    // Processar pacote...
}
```

### 2. Métricas de Performance

```cpp
// MetricsManager.h
struct ServerMetrics {
    float cpuUsage;
    size_t memoryUsage;
    int connectedPlayers;
    int activeThreads;
    float networkBandwidth;
    int queriesPerSecond;
    int64_t timestamp;
};

class MetricsManager {
public:
    void Update();
    const ServerMetrics& GetCurrentMetrics() const;
    std::vector<ServerMetrics> GetMetricsHistory(int minutes) const;
    
private:
    ServerMetrics m_currentMetrics;
    std::vector<ServerMetrics> m_metricsHistory;
    std::mutex m_metricsMutex;
};

// Coleta automática de métricas
void WYDStudio::Run() {
    // Configurar timer para coleta de métricas
    m_metricsTimer = std::make_unique<Timer>(5000, [this]() {
        // Coletar métricas de TMSrv
        ServerMetrics tmMetrics;
        tmMetrics.connectedPlayers = m_tmSrv->GetConnectedPlayerCount();
        tmMetrics.cpuUsage = m_tmSrv->GetCPUUsage();
        tmMetrics.memoryUsage = m_tmSrv->GetMemoryUsage();
        
        // Coletar métricas de DBSrv
        tmMetrics.queriesPerSecond = m_dbSrv->GetQueriesPerSecond();
        
        // Atualizar métricas
        m_metricsManager->UpdateMetrics(tmMetrics);
    });
    
    // Loop principal...
}
```

## Extensibilidade

### 1. Sistema de Plugins

```cpp
// Plugin.h
class Plugin {
public:
    virtual ~Plugin() = default;
    
    virtual bool Initialize(WYDStudio* studio) = 0;
    virtual void Shutdown() = 0;
    virtual const std::string& GetName() const = 0;
    virtual const std::string& GetVersion() const = 0;
};

// PluginManager.h
class PluginManager {
public:
    bool LoadPlugin(const std::string& path);
    void UnloadPlugin(const std::string& name);
    Plugin* GetPlugin(const std::string& name);
    std::vector<std::string> GetLoadedPluginNames() const;
    
private:
    std::unordered_map<std::string, std::unique_ptr<Plugin>> m_plugins;
};
```

### 2. Sistema de Eventos

```cpp
// EventSystem.h
enum class EventType {
    PLAYER_CONNECT,
    PLAYER_DISCONNECT,
    PLAYER_LEVEL_UP,
    ITEM_DROPPED,
    MONSTER_KILLED,
    SERVER_START,
    SERVER_STOP,
    CONFIG_CHANGED
};

struct Event {
    EventType type;
    std::unordered_map<std::string, std::any> data;
};

class EventListener {
public:
    virtual ~EventListener() = default;
    virtual void OnEvent(const Event& event) = 0;
};

class EventManager {
public:
    void RegisterListener(EventType type, EventListener* listener);
    void UnregisterListener(EventType type, EventListener* listener);
    void FireEvent(const Event& event);
    
private:
    std::unordered_map<EventType, std::vector<EventListener*>> m_listeners;
};

// Uso em TMSrv
void TMSrv::OnPlayerConnect(ClientID clientId) {
    // Lógica interna...
    
    // Notificar evento
    Event event;
    event.type = EventType::PLAYER_CONNECT;
    event.data["clientId"] = clientId;
    event.data["timestamp"] = GetCurrentTimestamp();
    m_eventManager->FireEvent(event);
}
```

## Plano de Implementação

### Fase 1: Infraestrutura Básica

1. Implementar classes de comunicação interna
2. Implementar sistema de configuração centralizado
3. Implementar sistema de logging compartilhado

### Fase 2: Integração Funcional

1. Implementar ciclo de vida compartilhado TMSrv/DBSrv
2. Desenvolver interfaces entre componentes
3. Implementar gerenciamento de memória compartilhada

### Fase 3: Interface Administrativa

1. Implementar painel de status do servidor
2. Implementar gerenciamento de jogadores
3. Implementar visualizador de logs
4. Implementar editor de configuração
5. Implementar navegador de banco de dados

### Fase 4: Sistemas Avançados

1. Implementar sistema de permissões
2. Implementar coletor de métricas
3. Implementar sistema de plugins/extensões
4. Implementar sistema de eventos

### Fase 5: Testes e Otimização

1. Testar comunicação entre componentes
2. Verificar gerenciamento de memória
3. Testar cargas de estresse
4. Otimizar pontos críticos de performance

## Conclusão

A integração do TMSrv e DBSrv no WYDStudio proporcionará uma arquitetura mais coesa, simplificará o gerenciamento do servidor e melhorará a experiência administrativa. Seguindo os princípios de design apresentados neste documento, a implementação preservará a arquitetura original em C++ enquanto aplica melhorias significativas em termos de segurança, usabilidade e performance.