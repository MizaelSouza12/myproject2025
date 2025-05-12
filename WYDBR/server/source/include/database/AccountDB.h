/**
 * AccountDB.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/database/AccountDB.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _ACCOUNT_DB_H_
#define _ACCOUNT_DB_H_

/**
 * @file AccountDB.h
 * @brief Gerenciador de banco de dados de contas do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de banco de dados de contas do WYDBR,
 * responsável por armazenar e gerenciar informações de contas de jogadores,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>

#include "../GlobalDef.h"
#include "../Basedef.h"
#include "DatabaseManager.h"

namespace wydbr {
namespace database {

/**
 * @brief Estado da conta
 */
enum /**
 * Classe WYDAccountState
 * 
 * Esta classe implementa a funcionalidade WYDAccountState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AccountState {
    NONE = 0,               // Nenhum
    NORMAL = 1,             // Normal
    BANNED = 2,             // Banida
    SUSPENDED = 3,          // Suspensa
    INACTIVE = 4,           // Inativa
    LOCKED = 5,             // Bloqueada
    PENDING = 6,            // Pendente
    DELETED = 7,            // Excluída
    CUSTOM_1 = 8,           // Personalizado 1
    CUSTOM_2 = 9,           // Personalizado 2
    CUSTOM_3 = 10,          // Personalizado 3
    UNKNOWN = 11            // Desconhecido
};

/**
 * @brief Tipo de conta
 */
enum /**
 * Classe WYDAccountType
 * 
 * Esta classe implementa a funcionalidade WYDAccountType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AccountType {
    NONE = 0,               // Nenhum
    NORMAL = 1,             // Normal
    VIP = 2,                // VIP
    PREMIUM = 3,            // Premium
    ADMIN = 4,              // Administrador
    GM = 5,                 // Game Master
    MODERATOR = 6,          // Moderador
    TESTER = 7,             // Testador
    CUSTOM_1 = 8,           // Personalizado 1
    CUSTOM_2 = 9,           // Personalizado 2
    CUSTOM_3 = 10,          // Personalizado 3
    UNKNOWN = 11            // Desconhecido
};

/**
 * @brief Resultado da operação de conta
 */
enum /**
 * Classe WYDAccountResult
 * 
 * Esta classe implementa a funcionalidade WYDAccountResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AccountResult {
    SUCCESS = 0,                    // Sucesso
    FAILED = 1,                     // Falha
    ACCOUNT_EXISTS = 2,             // Conta existe
    ACCOUNT_NOT_FOUND = 3,          // Conta não encontrada
    INVALID_CREDENTIALS = 4,        // Credenciais inválidas
    ACCOUNT_BANNED = 5,             // Conta banida
    ACCOUNT_SUSPENDED = 6,          // Conta suspensa
    ACCOUNT_INACTIVE = 7,           // Conta inativa
    ACCOUNT_LOCKED = 8,             // Conta bloqueada
    PASSWORD_MISMATCH = 9,          // Senha não confere
    INVALID_EMAIL = 10,             // Email inválido
    INVALID_USERNAME = 11,          // Nome de usuário inválido
    INVALID_PASSWORD = 12,          // Senha inválida
    TOO_MANY_ATTEMPTS = 13,         // Muitas tentativas
    SESSION_EXPIRED = 14,           // Sessão expirada
    SESSION_INVALID = 15,           // Sessão inválida
    ALREADY_LOGGED_IN = 16,         // Já está logado
    CUSTOM_1 = 17,                  // Personalizado 1
    CUSTOM_2 = 18,                  // Personalizado 2
    CUSTOM_3 = 19,                  // Personalizado 3
    UNKNOWN = 20                    // Desconhecido
};

/**
 * @brief Tipo de log de conta
 */
enum /**
 * Classe WYDAccountLogType
 * 
 * Esta classe implementa a funcionalidade WYDAccountLogType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AccountLogType {
    NONE = 0,               // Nenhum
    LOGIN = 1,              // Login
    LOGOUT = 2,             // Logout
    CREATION = 3,           // Criação
    DELETION = 4,           // Exclusão
    PASSWORD_CHANGE = 5,    // Alteração de senha
    EMAIL_CHANGE = 6,       // Alteração de email
    BAN = 7,                // Ban
    UNBAN = 8,              // Unban
    SUSPENSION = 9,         // Suspensão
    LOCK = 10,              // Bloqueio
    UNLOCK = 11,            // Desbloqueio
    FAILED_LOGIN = 12,      // Login falho
    TYPE_CHANGE = 13,       // Alteração de tipo
    STATUS_CHANGE = 14,     // Alteração de status
    CUSTOM_1 = 15,          // Personalizado 1
    CUSTOM_2 = 16,          // Personalizado 2
    CUSTOM_3 = 17,          // Personalizado 3
    UNKNOWN = 18            // Desconhecido
};

/**
 * @brief Informações da conta
 */
struct AccountInfo {
    uint32_t AccountID;         // ID da conta
    std::string Username;       // Nome de usuário
    std::string PasswordHash;   // Hash da senha
    std::string Email;          // Email
    std::string FullName;       // Nome completo
    std::string SecurityQuestion; // Pergunta de segurança
    std::string SecurityAnswer; // Resposta de segurança
    AccountState State;         // Estado
    AccountType Type;           // Tipo
    uint32_t CreationDate;      // Data de criação
    uint32_t LastLoginDate;     // Data do último login
    uint32_t LastLogoutDate;    // Data do último logout
    std::string LastLoginIP;    // IP do último login
    uint32_t BanExpirationDate; // Data de expiração do ban
    std::string BanReason;      // Motivo do ban
    uint32_t FailedLoginAttempts; // Tentativas de login falhas
    uint32_t TotalLoginCount;   // Contagem total de logins
    uint32_t TotalPlayTime;     // Tempo total de jogo
    bool TwoFactorEnabled;      // 2FA habilitado
    uint32_t VIPExpirationDate; // Data de expiração do VIP
    uint32_t LastCharacterID;   // ID do último personagem
    
    AccountInfo() : AccountID(0), State(AccountState::NORMAL),
        Type(AccountType::NORMAL), CreationDate(0), LastLoginDate(0),
        LastLogoutDate(0), BanExpirationDate(0), FailedLoginAttempts(0),
        TotalLoginCount(0), TotalPlayTime(0), TwoFactorEnabled(false),
        VIPExpirationDate(0), LastCharacterID(0) {}
};

/**
 * @brief Sessão de conta
 */
struct AccountSession {
    uint32_t SessionID;         // ID da sessão
    uint32_t AccountID;         // ID da conta
    std::string SessionToken;   // Token da sessão
    uint32_t StartTime;         // Tempo de início
    uint32_t LastActivity;      // Última atividade
    uint32_t ExpirationTime;    // Tempo de expiração
    std::string IP;             // IP
    bool IsActive;              // Está ativo
    
    AccountSession() : SessionID(0), AccountID(0), StartTime(0),
        LastActivity(0), ExpirationTime(0), IsActive(false) {}
};

/**
 * @brief Log de conta
 */
struct AccountLog {
    uint32_t LogID;             // ID do log
    uint32_t AccountID;         // ID da conta
    AccountLogType Type;        // Tipo
    uint32_t Timestamp;         // Timestamp
    std::string IP;             // IP
    std::string Details;        // Detalhes
    
    AccountLog() : LogID(0), AccountID(0), Type(AccountLogType::NONE),
        Timestamp(0) {}
};

/**
 * @brief Estatísticas de conta
 */
struct AccountStats {
    uint32_t TotalAccounts;     // Total de contas
    uint32_t ActiveAccounts;    // Contas ativas
    uint32_t BannedAccounts;    // Contas banidas
    uint32_t SuspendedAccounts; // Contas suspensas
    uint32_t VIPAccounts;       // Contas VIP
    uint32_t PremiumAccounts;   // Contas premium
    uint32_t NewAccountsToday;  // Novas contas hoje
    uint32_t LoginsToday;       // Logins hoje
    
    AccountStats() : TotalAccounts(0), ActiveAccounts(0), BannedAccounts(0),
        SuspendedAccounts(0), VIPAccounts(0), PremiumAccounts(0),
        NewAccountsToday(0), LoginsToday(0) {}
};

/**
 * @brief Configuração de conta
 */
struct AccountConfig {
    uint32_t MaxLoginAttempts;      // Máximo de tentativas de login
    uint32_t LockDuration;          // Duração do bloqueio
    uint32_t SessionTimeout;        // Timeout da sessão
    bool RequireEmailVerification;  // Requer verificação de email
    bool AllowMultipleSessions;     // Permite múltiplas sessões
    uint8_t MinUsernameLength;      // Tamanho mínimo do nome de usuário
    uint8_t MaxUsernameLength;      // Tamanho máximo do nome de usuário
    uint8_t MinPasswordLength;      // Tamanho mínimo da senha
    uint8_t MaxPasswordLength;      // Tamanho máximo da senha
    bool PasswordRequiresLetter;    // Senha requer letra
    bool PasswordRequiresNumber;    // Senha requer número
    bool PasswordRequiresSpecial;   // Senha requer caractere especial
    bool UsePasswordHashing;        // Usar hash de senha
    
    AccountConfig() : MaxLoginAttempts(5), LockDuration(3600),
        SessionTimeout(3600), RequireEmailVerification(false),
        AllowMultipleSessions(false), MinUsernameLength(4),
        MaxUsernameLength(16), MinPasswordLength(6), MaxPasswordLength(32),
        PasswordRequiresLetter(true), PasswordRequiresNumber(true),
        PasswordRequiresSpecial(false), UsePasswordHashing(true) {}
};

/**
 * @brief Tipo de callback de conta
 */
using AccountCallback = std::function<void(const AccountInfo&, AccountState)>;

/**
 * @brief Gerenciador de banco de dados de contas
 * 
 * Esta classe é responsável por gerenciar o banco de dados de contas dos jogadores.
 */
/**
 * Classe WYDAccountDB
 * 
 * Esta classe implementa a funcionalidade WYDAccountDB conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class AccountDB {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static AccountDB& getInstance() {
        static AccountDB instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @param dbManager Gerenciador de banco de dados
     * @return true se inicializado com sucesso
     */
    bool initialize(DatabaseManager* dbManager);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração de contas
     * @return true se carregado com sucesso
     */
    bool loadAccountConfig();
    
    /**
     * @brief Cria uma conta
     * @param username Nome de usuário
     * @param password Senha
     * @param email Email
     * @param fullName Nome completo
     * @param type Tipo
     * @return ID da conta ou 0 em caso de erro
     */
    uint32_t createAccount(const std::string& username, const std::string& password,
                         const std::string& email, const std::string& fullName = "",
                         AccountType type = AccountType::NORMAL);
    
    /**
     * @brief Exclui uma conta
     * @param accountID ID da conta
     * @return Resultado da operação
     */
    AccountResult deleteAccount(uint32_t accountID);
    
    /**
     * @brief Autentica uma conta
     * @param username Nome de usuário
     * @param password Senha
     * @param ip IP
     * @return ID da conta ou 0 em caso de erro
     */
    uint32_t authenticateAccount(const std::string& username, const std::string& password, const std::string& ip);
    
    /**
     * @brief Cria uma sessão
     * @param accountID ID da conta
     * @param ip IP
     * @return Token da sessão ou string vazia em caso de erro
     */
    std::string createSession(uint32_t accountID, const std::string& ip);
    
    /**
     * @brief Valida uma sessão
     * @param sessionToken Token da sessão
     * @return ID da conta ou 0 em caso de erro
     */
    uint32_t validateSession(const std::string& sessionToken);
    
    /**
     * @brief Encerra uma sessão
     * @param sessionToken Token da sessão
     * @return Resultado da operação
     */
    AccountResult endSession(const std::string& sessionToken);
    
    /**
     * @brief Altera a senha de uma conta
     * @param accountID ID da conta
     * @param oldPassword Senha antiga
     * @param newPassword Nova senha
     * @return Resultado da operação
     */
    AccountResult changePassword(uint32_t accountID, const std::string& oldPassword, const std::string& newPassword);
    
    /**
     * @brief Altera o email de uma conta
     * @param accountID ID da conta
     * @param password Senha
     * @param newEmail Novo email
     * @return Resultado da operação
     */
    AccountResult changeEmail(uint32_t accountID, const std::string& password, const std::string& newEmail);
    
    /**
     * @brief Define a pergunta e resposta de segurança
     * @param accountID ID da conta
     * @param password Senha
     * @param question Pergunta
     * @param answer Resposta
     * @return Resultado da operação
     */
    AccountResult setSecurityQuestion(uint32_t accountID, const std::string& password, const std::string& question, const std::string& answer);
    
    /**
     * @brief Redefine a senha de uma conta
     * @param username Nome de usuário
     * @param email Email
     * @param securityAnswer Resposta de segurança
     * @param newPassword Nova senha
     * @return Resultado da operação
     */
    AccountResult resetPassword(const std::string& username, const std::string& email, const std::string& securityAnswer, const std::string& newPassword);
    
    /**
     * @brief Bane uma conta
     * @param accountID ID da conta
     * @param reason Motivo
     * @param duration Duração (0 para permanente)
     * @return Resultado da operação
     */
    AccountResult banAccount(uint32_t accountID, const std::string& reason, uint32_t duration = 0);
    
    /**
     * @brief Remove o ban de uma conta
     * @param accountID ID da conta
     * @return Resultado da operação
     */
    AccountResult unbanAccount(uint32_t accountID);
    
    /**
     * @brief Suspende uma conta
     * @param accountID ID da conta
     * @param reason Motivo
     * @param duration Duração (0 para permanente)
     * @return Resultado da operação
     */
    AccountResult suspendAccount(uint32_t accountID, const std::string& reason, uint32_t duration = 0);
    
    /**
     * @brief Remove a suspensão de uma conta
     * @param accountID ID da conta
     * @return Resultado da operação
     */
    AccountResult unsuspendAccount(uint32_t accountID);
    
    /**
     * @brief Bloqueia uma conta
     * @param accountID ID da conta
     * @param reason Motivo
     * @return Resultado da operação
     */
    AccountResult lockAccount(uint32_t accountID, const std::string& reason);
    
    /**
     * @brief Desbloqueia uma conta
     * @param accountID ID da conta
     * @return Resultado da operação
     */
    AccountResult unlockAccount(uint32_t accountID);
    
    /**
     * @brief Define o tipo de uma conta
     * @param accountID ID da conta
     * @param type Tipo
     * @return Resultado da operação
     */
    AccountResult setAccountType(uint32_t accountID, AccountType type);
    
    /**
     * @brief Registra um login
     * @param accountID ID da conta
     * @param ip IP
     * @return Resultado da operação
     */
    AccountResult registerLogin(uint32_t accountID, const std::string& ip);
    
    /**
     * @brief Registra um logout
     * @param accountID ID da conta
     * @return Resultado da operação
     */
    AccountResult registerLogout(uint32_t accountID);
    
    /**
     * @brief Registra um login falho
     * @param username Nome de usuário
     * @param ip IP
     * @return Número de tentativas falhas
     */
    uint32_t registerFailedLogin(const std::string& username, const std::string& ip);
    
    /**
     * @brief Reseta as tentativas de login falhas
     * @param username Nome de usuário
     * @return Resultado da operação
     */
    AccountResult resetFailedLoginAttempts(const std::string& username);
    
    /**
     * @brief Adiciona tempo de jogo
     * @param accountID ID da conta
     * @param seconds Segundos
     * @return Resultado da operação
     */
    AccountResult addPlayTime(uint32_t accountID, uint32_t seconds);
    
    /**
     * @brief Habilita ou desabilita 2FA
     * @param accountID ID da conta
     * @param enable Habilitar
     * @return Resultado da operação
     */
    AccountResult setTwoFactorEnabled(uint32_t accountID, bool enable);
    
    /**
     * @brief Define a data de expiração do VIP
     * @param accountID ID da conta
     * @param expirationDate Data de expiração
     * @return Resultado da operação
     */
    AccountResult setVIPExpirationDate(uint32_t accountID, uint32_t expirationDate);
    
    /**
     * @brief Define o último personagem
     * @param accountID ID da conta
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    AccountResult setLastCharacter(uint32_t accountID, uint32_t characterID);
    
    /**
     * @brief Obtém uma conta por ID
     * @param accountID ID da conta
     * @return Informações da conta ou nullptr se não encontrada
     */
    const AccountInfo* getAccount(uint32_t accountID) const;
    
    /**
     * @brief Obtém uma conta por nome de usuário
     * @param username Nome de usuário
     * @return ID da conta ou 0 se não encontrada
     */
    uint32_t getAccountByUsername(const std::string& username) const;
    
    /**
     * @brief Obtém uma conta por email
     * @param email Email
     * @return ID da conta ou 0 se não encontrada
     */
    uint32_t getAccountByEmail(const std::string& email) const;
    
    /**
     * @brief Obtém sessões de uma conta
     * @param accountID ID da conta
     * @return Lista de sessões
     */
    std::vector<AccountSession> getAccountSessions(uint32_t accountID) const;
    
    /**
     * @brief Obtém logs de uma conta
     * @param accountID ID da conta
     * @param limit Limite (0 para todos)
     * @param offset Deslocamento
     * @return Lista de logs
     */
    std::vector<AccountLog> getAccountLogs(uint32_t accountID, uint32_t limit = 0, uint32_t offset = 0) const;
    
    /**
     * @brief Verifica o estado de uma conta
     * @param accountID ID da conta
     * @return Estado
     */
    AccountState getAccountState(uint32_t accountID) const;
    
    /**
     * @brief Verifica o tipo de uma conta
     * @param accountID ID da conta
     * @return Tipo
     */
    AccountType getAccountType(uint32_t accountID) const;
    
    /**
     * @brief Verifica se uma sessão é válida
     * @param sessionToken Token da sessão
     * @return true se for válida
     */
    bool isSessionValid(const std::string& sessionToken) const;
    
    /**
     * @brief Verifica se uma conta está online
     * @param accountID ID da conta
     * @return true se estiver online
     */
    bool isAccountOnline(uint32_t accountID) const;
    
    /**
     * @brief Registra um callback de conta
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerAccountCallback(AccountCallback callback);
    
    /**
     * @brief Remove um callback de conta
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterAccountCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém estatísticas de contas
     * @return Estatísticas
     */
    AccountStats getAccountStats() const;
    
    /**
     * @brief Obtém a configuração de contas
     * @return Configuração
     */
    AccountConfig getAccountConfig() const;
    
    /**
     * @brief Define a configuração de contas
     * @param config Configuração
     */
    void setAccountConfig(const AccountConfig& config);
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
private:
    /**
     * @brief Construtor privado
     */
    AccountDB();
    
    /**
     * @brief Destrutor privado
     */
    ~AccountDB();
    
    /**
     * @brief Construtor de cópia privado
     */
    AccountDB(const AccountDB&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    AccountDB& operator=(const AccountDB&) = delete;
    
    // Gerenciador de banco de dados
    DatabaseManager* _dbManager;
    
    // Cache de contas
    std::unordered_map<uint32_t, AccountInfo> _accountCache;
    std::unordered_map<std::string, uint32_t> _usernameToID;
    std::unordered_map<std::string, uint32_t> _emailToID;
    std::atomic<uint32_t> _nextAccountID;
    
    // Sessões
    std::unordered_map<std::string, AccountSession> _sessions;
    std::unordered_map<uint32_t, std::vector<std::string>> _accountSessions;
    std::atomic<uint32_t> _nextSessionID;
    
    // Logs
    std::unordered_map<uint32_t, std::vector<AccountLog>> _accountLogs;
    std::atomic<uint32_t> _nextLogID;
    
    // Tentativas de login
    std::unordered_map<std::string, uint32_t> _failedLoginAttempts;
    
    // Estatísticas
    AccountStats _accountStats;
    
    // Configuração
    AccountConfig _accountConfig;
    
    // Callbacks
    std::vector<AccountCallback> _accountCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _accountMutex;
    mutable std::mutex _sessionMutex;
    mutable std::mutex _logMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Timers
    uint32_t _sessionExpirationTimer;
    uint32_t _vipExpirationTimer;
    
    /**
     * @brief Verifica sessões expiradas
     * @param currentTime Tempo atual
     */
    void checkExpiredSessions(uint32_t currentTime);
    
    /**
     * @brief Verifica contas VIP expiradas
     * @param currentTime Tempo atual
     */
    void checkExpiredVIP(uint32_t currentTime);
    
    /**
     * @brief Atualiza as estatísticas
     */
    void updateAccountStats();
    
    /**
     * @brief Adiciona um log
     * @param accountID ID da conta
     * @param type Tipo
     * @param ip IP
     * @param details Detalhes
     * @return ID do log
     */
    uint32_t addLog(uint32_t accountID, AccountLogType type, const std::string& ip = "", const std::string& details = "");
    
    /**
     * @brief Executa callbacks de conta
     * @param account Conta
     * @param oldState Estado antigo
     */
    void executeAccountCallbacks(const AccountInfo& account, AccountState oldState);
    
    /**
     * @brief Gera um hash de senha
     * @param password Senha
     * @return Hash
     */
    std::string hashPassword(const std::string& password);
    
    /**
     * @brief Verifica uma senha
     * @param password Senha
     * @param hash Hash
     * @return true se corresponder
     */
    bool verifyPassword(const std::string& password, const std::string& hash);
    
    /**
     * @brief Gera um token de sessão
     * @return Token
     */
    std::string generateSessionToken();
    
    /**
     * @brief Valida um email
     * @param email Email
     * @return true se for válido
     */
    bool isValidEmail(const std::string& email);
    
    /**
     * @brief Valida um nome de usuário
     * @param username Nome de usuário
     * @return true se for válido
     */
    bool isValidUsername(const std::string& username);
    
    /**
     * @brief Valida uma senha
     * @param password Senha
     * @return true se for válida
     */
    bool isValidPassword(const std::string& password);
    
    /**
     * @brief Carrega uma conta do banco de dados
     * @param accountID ID da conta
     * @return Informações da conta ou nullptr se não encontrada
     */
    const AccountInfo* loadAccountFromDB(uint32_t accountID);
    
    /**
     * @brief Salva uma conta no banco de dados
     * @param account Conta
     * @return true se salvo com sucesso
     */
    bool saveAccountToDB(const AccountInfo& account);
};

} // namespace database
} // namespace wydbr

#endif // _ACCOUNT_DB_H_