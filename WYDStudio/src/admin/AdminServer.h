#ifndef WYDSTUDIO_ADMIN_SERVER_H
#define WYDSTUDIO_ADMIN_SERVER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <variant>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace admin {

// Forward declarations
class AdminUser;
class AdminSession;
class AdminCommand;
class AdminCommandProcessor;
class AdminPlugin;
class AdminAuditLog;
class AdminPermissionManager;

/**
 * @brief Níveis de permissão para usuários admin
 */
enum class PermissionLevel {
    NONE = 0,
    VIEWER = 10,      // Somente visualização
    OPERATOR = 20,    // Operações básicas
    ADMIN = 30,       // Administração completa
    SUPER_ADMIN = 40  // Administração de sistema
};

/**
 * @brief Tipo de recurso para controle de acesso
 */
enum class ResourceType {
    SERVER,         // Servidor
    PLAYER,         // Jogador
    ITEM,           // Item
    MOB,            // Monstro/NPC
    MAP,            // Mapa
    QUEST,          // Quest
    GUILD,          // Guild
    EVENT,          // Evento
    ECONOMY,        // Economia
    ACCOUNT,        // Conta
    PUNISHMENT,     // Punição
    LOG,            // Logs
    SYSTEM,         // Sistema
    CONFIGURATION,  // Configuração
    PLUGIN,         // Plugin
    CUSTOM          // Personalizado
};

/**
 * @brief Operação para controle de acesso
 */
enum class Operation {
    VIEW,      // Ver/Ler
    CREATE,    // Criar/Adicionar
    MODIFY,    // Modificar/Editar
    DELETE,    // Excluir/Remover
    EXECUTE,   // Executar
    MANAGE,    // Gerenciar
    ALL        // Todas as operações
};

/**
 * @brief Entrada de log de auditoria
 */
struct AuditLogEntry {
    uint64_t id;
    std::chrono::system_clock::time_point timestamp;
    std::string userId;
    std::string username;
    std::string ipAddress;
    std::string action;
    ResourceType resourceType;
    std::string resourceId;
    std::string details;
    bool success;
    std::string errorMessage;
    
    AuditLogEntry() : id(0), success(true) {}
};

/**
 * @brief Regra de controle de acesso
 */
struct AccessControlRule {
    ResourceType resourceType;
    std::string resourceId;     // "" = todos
    Operation operation;
    bool allow;
    std::string conditions;     // Condições adicionais em formato de expressão
    
    AccessControlRule() : resourceType(ResourceType::SERVER), 
                        operation(Operation::VIEW), allow(false) {}
    
    // Regras de conveniência
    static AccessControlRule allowView(ResourceType type, const std::string& id = "") {
        AccessControlRule rule;
        rule.resourceType = type;
        rule.resourceId = id;
        rule.operation = Operation::VIEW;
        rule.allow = true;
        return rule;
    }
    
    static AccessControlRule allowAll(ResourceType type, const std::string& id = "") {
        AccessControlRule rule;
        rule.resourceType = type;
        rule.resourceId = id;
        rule.operation = Operation::ALL;
        rule.allow = true;
        return rule;
    }
    
    static AccessControlRule deny(ResourceType type, Operation op, const std::string& id = "") {
        AccessControlRule rule;
        rule.resourceType = type;
        rule.resourceId = id;
        rule.operation = op;
        rule.allow = false;
        return rule;
    }
};

/**
 * @brief Usuário administrador
 */
class AdminUser {
public:
    AdminUser(const std::string& id, const std::string& username);
    ~AdminUser() = default;
    
    // Identificação
    std::string getId() const { return m_id; }
    std::string getUsername() const { return m_username; }
    
    // Autenticação
    void setPasswordHash(const std::string& passwordHash);
    std::string getPasswordHash() const { return m_passwordHash; }
    
    bool verifyPassword(const std::string& password) const;
    std::string generatePasswordHash(const std::string& password) const;
    
    // Permissões
    void setPermissionLevel(PermissionLevel level);
    PermissionLevel getPermissionLevel() const { return m_permissionLevel; }
    
    void addAccessRule(const AccessControlRule& rule);
    void removeAccessRule(ResourceType type, Operation op, const std::string& resourceId = "");
    void clearAccessRules();
    
    const std::vector<AccessControlRule>& getAccessRules() const { return m_accessRules; }
    
    // Informações pessoais
    void setDisplayName(const std::string& name);
    std::string getDisplayName() const { return m_displayName; }
    
    void setEmail(const std::string& email);
    std::string getEmail() const { return m_email; }
    
    // Estado
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    
    void setLastLogin(std::chrono::system_clock::time_point time);
    std::chrono::system_clock::time_point getLastLogin() const { return m_lastLogin; }
    
    void setLastIp(const std::string& ip);
    std::string getLastIp() const { return m_lastIp; }
    
    // Serialização
    std::string toJson() const;
    static AdminUser fromJson(const std::string& json);
    
private:
    std::string m_id;
    std::string m_username;
    std::string m_passwordHash;
    PermissionLevel m_permissionLevel;
    std::vector<AccessControlRule> m_accessRules;
    
    std::string m_displayName;
    std::string m_email;
    bool m_enabled;
    std::chrono::system_clock::time_point m_lastLogin;
    std::string m_lastIp;
};

/**
 * @brief Sessão de administrador
 */
class AdminSession {
public:
    AdminSession(const std::string& id, std::shared_ptr<AdminUser> user);
    ~AdminSession() = default;
    
    // Identificação
    std::string getId() const { return m_id; }
    std::shared_ptr<AdminUser> getUser() const { return m_user; }
    
    // Estado da sessão
    std::chrono::system_clock::time_point getCreationTime() const { return m_creationTime; }
    std::chrono::system_clock::time_point getLastActivityTime() const { return m_lastActivityTime; }
    std::string getIpAddress() const { return m_ipAddress; }
    
    void setIpAddress(const std::string& ip);
    void updateLastActivity();
    
    bool isExpired(std::chrono::seconds timeout) const;
    
    // Token de sessão
    std::string getToken() const { return m_token; }
    void regenerateToken();
    
    // Dados da sessão
    void setData(const std::string& key, const std::string& value);
    std::string getData(const std::string& key) const;
    bool hasData(const std::string& key) const;
    void removeData(const std::string& key);
    
private:
    std::string m_id;
    std::shared_ptr<AdminUser> m_user;
    std::chrono::system_clock::time_point m_creationTime;
    std::chrono::system_clock::time_point m_lastActivityTime;
    std::string m_ipAddress;
    std::string m_token;
    std::unordered_map<std::string, std::string> m_sessionData;
};

/**
 * @brief Resultado de execução de comando
 */
struct CommandResult {
    enum class Status {
        SUCCESS,
        ERROR,
        ACCESS_DENIED,
        INVALID_SYNTAX,
        NOT_FOUND,
        TIMEOUT,
        CANCELED
    };
    
    Status status;
    std::string message;
    std::variant<
        std::monostate,
        std::string,
        int,
        double,
        bool,
        std::vector<std::string>,
        std::unordered_map<std::string, std::string>
    > data;
    
    std::chrono::milliseconds executionTime;
    
    CommandResult() : status(Status::SUCCESS), executionTime(0) {}
    
    static CommandResult createSuccess(const std::string& msg = "Command executed successfully") {
        CommandResult result;
        result.status = Status::SUCCESS;
        result.message = msg;
        return result;
    }
    
    static CommandResult createError(const std::string& errorMsg) {
        CommandResult result;
        result.status = Status::ERROR;
        result.message = errorMsg;
        return result;
    }
    
    static CommandResult createAccessDenied() {
        CommandResult result;
        result.status = Status::ACCESS_DENIED;
        result.message = "Access denied";
        return result;
    }
    
    // Métodos auxiliares para definir dados
    void setStringData(const std::string& value) { data = value; }
    void setIntData(int value) { data = value; }
    void setDoubleData(double value) { data = value; }
    void setBoolData(bool value) { data = value; }
    void setStringListData(const std::vector<std::string>& value) { data = value; }
    void setMapData(const std::unordered_map<std::string, std::string>& value) { data = value; }
    
    // Métodos auxiliares para obter dados
    std::string getStringData(const std::string& defaultValue = "") const {
        try {
            return std::get<std::string>(data);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    
    int getIntData(int defaultValue = 0) const {
        try {
            return std::get<int>(data);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    
    double getDoubleData(double defaultValue = 0.0) const {
        try {
            return std::get<double>(data);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    
    bool getBoolData(bool defaultValue = false) const {
        try {
            return std::get<bool>(data);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    
    std::vector<std::string> getStringListData() const {
        try {
            return std::get<std::vector<std::string>>(data);
        } catch (const std::bad_variant_access&) {
            return {};
        }
    }
    
    std::unordered_map<std::string, std::string> getMapData() const {
        try {
            return std::get<std::unordered_map<std::string, std::string>>(data);
        } catch (const std::bad_variant_access&) {
            return {};
        }
    }
};

/**
 * @brief Comando administrativo
 */
class AdminCommand {
public:
    AdminCommand(const std::string& name, const std::string& description = "");
    virtual ~AdminCommand() = default;
    
    // Metadados
    std::string getName() const { return m_name; }
    std::string getDescription() const { return m_description; }
    
    // Sintaxe e ajuda
    void setSyntax(const std::string& syntax);
    std::string getSyntax() const { return m_syntax; }
    
    void setHelp(const std::string& help);
    std::string getHelp() const { return m_help; }
    
    // Exigências
    void setRequiredPermissionLevel(PermissionLevel level);
    PermissionLevel getRequiredPermissionLevel() const { return m_requiredPermissionLevel; }
    
    void setRequiredAccessRule(ResourceType type, Operation op);
    std::pair<ResourceType, Operation> getRequiredAccessRule() const {
        return {m_requiredResourceType, m_requiredOperation};
    }
    
    // Execução
    virtual CommandResult execute(const std::vector<std::string>& args, 
                               std::shared_ptr<AdminSession> session) = 0;
    
    // Validação básica
    bool validateArgCount(const std::vector<std::string>& args, 
                         size_t minCount,
                         size_t maxCount = std::numeric_limits<size_t>::max()) const;
    
protected:
    std::string m_name;
    std::string m_description;
    std::string m_syntax;
    std::string m_help;
    PermissionLevel m_requiredPermissionLevel;
    ResourceType m_requiredResourceType;
    Operation m_requiredOperation;
};

/**
 * @brief Processador de comandos admin
 */
class AdminCommandProcessor {
public:
    AdminCommandProcessor();
    ~AdminCommandProcessor() = default;
    
    // Registro de comandos
    void registerCommand(std::shared_ptr<AdminCommand> command);
    void unregisterCommand(const std::string& name);
    
    // Acesso a comandos
    std::shared_ptr<AdminCommand> getCommand(const std::string& name) const;
    std::vector<std::shared_ptr<AdminCommand>> getAllCommands() const;
    
    // Processamento
    CommandResult processCommand(const std::string& commandLine, 
                               std::shared_ptr<AdminSession> session);
    
    // Ferramentas
    std::vector<std::string> parseCommandLine(const std::string& commandLine) const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<AdminCommand>> m_commands;
};

/**
 * @brief Gerenciador de permissões
 */
class AdminPermissionManager {
public:
    AdminPermissionManager();
    ~AdminPermissionManager() = default;
    
    // Verificação de acesso
    bool hasPermission(std::shared_ptr<AdminUser> user, 
                     PermissionLevel requiredLevel) const;
    
    bool hasAccess(std::shared_ptr<AdminUser> user,
                  ResourceType resourceType,
                  Operation operation,
                  const std::string& resourceId = "") const;
    
    // Gerenciamento de papéis
    void defineRole(const std::string& roleName,
                   PermissionLevel level,
                   const std::vector<AccessControlRule>& rules);
    
    void assignRoleToUser(std::shared_ptr<AdminUser> user,
                         const std::string& roleName);
    
    void revokeRoleFromUser(std::shared_ptr<AdminUser> user,
                           const std::string& roleName);
    
    // Acesso a papéis
    std::vector<std::string> getRoleNames() const;
    PermissionLevel getRoleLevel(const std::string& roleName) const;
    std::vector<AccessControlRule> getRoleRules(const std::string& roleName) const;
    
private:
    struct Role {
        std::string name;
        PermissionLevel level;
        std::vector<AccessControlRule> rules;
    };
    
    std::unordered_map<std::string, Role> m_roles;
    std::unordered_map<std::string, std::vector<std::string>> m_userRoles;
};

/**
 * @brief Sistema de log de auditoria
 */
class AdminAuditLog {
public:
    AdminAuditLog();
    ~AdminAuditLog();
    
    // Adicionar entradas
    void logAction(const std::string& userId,
                  const std::string& username,
                  const std::string& ipAddress,
                  const std::string& action,
                  ResourceType resourceType,
                  const std::string& resourceId,
                  const std::string& details,
                  bool success,
                  const std::string& errorMessage = "");
    
    // Consulta de logs
    std::vector<AuditLogEntry> queryLogs(
        std::optional<std::chrono::system_clock::time_point> startTime = std::nullopt,
        std::optional<std::chrono::system_clock::time_point> endTime = std::nullopt,
        std::optional<std::string> userId = std::nullopt,
        std::optional<std::string> action = std::nullopt,
        std::optional<ResourceType> resourceType = std::nullopt,
        std::optional<std::string> resourceId = std::nullopt,
        std::optional<bool> success = std::nullopt,
        size_t maxResults = 100);
    
    // Exportação
    bool exportLogs(const std::string& filePath,
                  std::optional<std::chrono::system_clock::time_point> startTime = std::nullopt,
                  std::optional<std::chrono::system_clock::time_point> endTime = std::nullopt);
    
    // Limpeza
    bool clearLogsOlderThan(std::chrono::system_clock::time_point cutoffTime);
    
private:
    std::vector<AuditLogEntry> m_logs;
    std::mutex m_logMutex;
    uint64_t m_nextLogId;
    
    // Persistência
    bool loadLogs();
    bool saveLogs();
    std::string m_logFilePath;
};

/**
 * @brief Interface para plugins de admin
 */
class AdminPlugin {
public:
    AdminPlugin(const std::string& name, const std::string& version);
    virtual ~AdminPlugin() = default;
    
    // Identificação
    std::string getName() const { return m_name; }
    std::string getVersion() const { return m_version; }
    std::string getDescription() const { return m_description; }
    std::string getAuthor() const { return m_author; }
    
    // Metadados
    void setDescription(const std::string& description);
    void setAuthor(const std::string& author);
    
    // Ciclo de vida
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Comandos
    virtual std::vector<std::shared_ptr<AdminCommand>> getCommands() const = 0;
    
protected:
    std::string m_name;
    std::string m_version;
    std::string m_description;
    std::string m_author;
};

/**
 * @brief Configuração do servidor admin
 */
struct AdminServerConfig {
    uint16_t httpPort;
    uint16_t webSocketPort;
    std::string bindAddress;
    std::string certificatePath;
    std::string keyPath;
    bool useHttps;
    
    std::chrono::seconds sessionTimeout;
    std::chrono::seconds tokenExpirationTime;
    
    std::string logDirectory;
    size_t maxLogEntries;
    
    std::string usersFilePath;
    std::string pluginsDirectory;
    
    AdminServerConfig() 
        : httpPort(8080),
          webSocketPort(8081),
          bindAddress("0.0.0.0"),
          useHttps(false),
          sessionTimeout(3600),
          tokenExpirationTime(86400),
          maxLogEntries(10000) {}
};

/**
 * @brief Servidor de administração
 */
class AdminServer {
public:
    // Singleton
    static AdminServer& getInstance();
    
    // Inicialização
    bool initialize(const AdminServerConfig& config);
    bool shutdown();
    bool isInitialized() const { return m_initialized; }
    
    // Estado do servidor
    bool start();
    bool stop();
    bool isRunning() const { return m_running; }
    
    // Gerenciamento de usuários
    bool addUser(std::shared_ptr<AdminUser> user);
    bool updateUser(std::shared_ptr<AdminUser> user);
    bool removeUser(const std::string& userId);
    std::shared_ptr<AdminUser> getUser(const std::string& userId) const;
    std::shared_ptr<AdminUser> getUserByUsername(const std::string& username) const;
    std::vector<std::shared_ptr<AdminUser>> getAllUsers() const;
    
    // Autenticação
    std::shared_ptr<AdminSession> authenticateUser(const std::string& username, 
                                               const std::string& password,
                                               const std::string& ipAddress);
    
    std::shared_ptr<AdminSession> getSession(const std::string& sessionId) const;
    std::shared_ptr<AdminSession> getSessionByToken(const std::string& token) const;
    bool invalidateSession(const std::string& sessionId);
    bool invalidateAllUserSessions(const std::string& userId);
    
    // Comandos
    AdminCommandProcessor& getCommandProcessor() { return *m_commandProcessor; }
    CommandResult executeCommand(const std::string& command, 
                              std::shared_ptr<AdminSession> session);
    
    // Permissões
    AdminPermissionManager& getPermissionManager() { return *m_permissionManager; }
    
    // Logs
    AdminAuditLog& getAuditLog() { return *m_auditLog; }
    
    // Plugins
    bool loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& name);
    std::shared_ptr<AdminPlugin> getPlugin(const std::string& name) const;
    std::vector<std::shared_ptr<AdminPlugin>> getAllPlugins() const;
    
    // Configuração
    AdminServerConfig& getConfig() { return m_config; }
    const AdminServerConfig& getConfig() const { return m_config; }
    
    // Monitoramento
    struct ServerStatistics {
        uint32_t activeUsers;
        uint32_t activeSessions;
        uint32_t commandsExecuted;
        uint32_t failedCommands;
        uint32_t failedLogins;
        std::chrono::system_clock::time_point startTime;
        std::chrono::seconds uptime;
    };
    
    ServerStatistics getStatistics() const;
    
private:
    AdminServer();
    ~AdminServer();
    
    // Prevenção de cópia
    AdminServer(const AdminServer&) = delete;
    AdminServer& operator=(const AdminServer&) = delete;
    
    bool m_initialized;
    bool m_running;
    AdminServerConfig m_config;
    
    // Componentes internos
    std::unique_ptr<AdminCommandProcessor> m_commandProcessor;
    std::unique_ptr<AdminPermissionManager> m_permissionManager;
    std::unique_ptr<AdminAuditLog> m_auditLog;
    
    // Servidores HTTP e WebSocket
    void* m_httpServer;
    void* m_wsServer;
    
    // Usuários e sessões
    std::unordered_map<std::string, std::shared_ptr<AdminUser>> m_users;
    std::unordered_map<std::string, std::shared_ptr<AdminSession>> m_sessions;
    std::unordered_map<std::string, std::string> m_tokenToSessionMap;
    
    // Plugins
    std::unordered_map<std::string, std::shared_ptr<AdminPlugin>> m_plugins;
    
    // Tarefas em segundo plano
    void startWorkerTasks();
    void sessionCleanupTask();
    
    // Carregamento e salvamento de dados
    bool loadUsers();
    bool saveUsers();
    
    // Estatísticas
    ServerStatistics m_statistics;
    std::chrono::system_clock::time_point m_startTime;
    std::atomic<uint32_t> m_commandsExecuted;
    std::atomic<uint32_t> m_failedCommands;
    std::atomic<uint32_t> m_failedLogins;
    
    // Task scheduler
    std::shared_ptr<core::TaskScheduler> m_taskScheduler;
};

/**
 * @brief Comandos padrão
 */
namespace standard_commands {

// Comando Help
class HelpCommand : public AdminCommand {
public:
    HelpCommand();
    
    CommandResult execute(const std::vector<std::string>& args, 
                        std::shared_ptr<AdminSession> session) override;
};

// Comando de usuários
class UserCommand : public AdminCommand {
public:
    UserCommand();
    
    CommandResult execute(const std::vector<std::string>& args, 
                        std::shared_ptr<AdminSession> session) override;
    
private:
    CommandResult listUsers();
    CommandResult showUser(const std::string& username);
    CommandResult addUser(const std::vector<std::string>& args);
    CommandResult removeUser(const std::string& username);
    CommandResult setPassword(const std::string& username, const std::string& password);
    CommandResult setPermission(const std::string& username, const std::string& permLevel);
};

// Comando de servidor
class ServerCommand : public AdminCommand {
public:
    ServerCommand();
    
    CommandResult execute(const std::vector<std::string>& args, 
                        std::shared_ptr<AdminSession> session) override;
};

// Comando de logs
class LogCommand : public AdminCommand {
public:
    LogCommand();
    
    CommandResult execute(const std::vector<std::string>& args, 
                        std::shared_ptr<AdminSession> session) override;
};

// Comando de plugin
class PluginCommand : public AdminCommand {
public:
    PluginCommand();
    
    CommandResult execute(const std::vector<std::string>& args, 
                        std::shared_ptr<AdminSession> session) override;
};

} // namespace standard_commands

} // namespace admin
} // namespace wydstudio

#endif // WYDSTUDIO_ADMIN_SERVER_H