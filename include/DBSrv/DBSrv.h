
/**
 * @file DBSrv.h
 * @brief Servidor de banco de dados DBSrv para WYDBR 2.0
 */

#ifndef WYDBR_DBSRV_H
#define WYDBR_DBSRV_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace WYDBR {

// Forward declarations
class DatabaseManager;
class NetworkManager;
class AccountManager;
class ItemStorageManager;
class CharacterStorageManager;
class LogStorageManager;

/**
 * Enum para status do banco de dados
 */
enum class DatabaseStatus {
    OFFLINE,
    CONNECTING,
    CONNECTED,
    ERROR
};

/**
 * Classe principal do DBSrv
 */
class DBSrv {
public:
    DBSrv();
    ~DBSrv();
    
    /**
     * Inicializa o servidor de banco de dados
     * @param configPath Caminho para o arquivo de configuração
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(const std::string& configPath = "");
    
    /**
     * Inicia o servidor de banco de dados
     * @param port Porta para escutar (0 = usar valor da configuração)
     * @return true se o início foi bem-sucedido, false caso contrário
     */
    bool Start(uint16_t port = 0);
    
    /**
     * Encerra o servidor de banco de dados
     */
    void Shutdown();
    
    /**
     * Processa a lógica do servidor de banco de dados
     * @param deltaTime Tempo desde o último update em milissegundos
     */
    void Update(float deltaTime);
    
    /**
     * Obtém o status atual do banco de dados
     * @return Status atual
     */
    DatabaseStatus GetStatus() const;
    
    // Getters para gerenciadores
    DatabaseManager* GetDatabaseManager() const;
    AccountManager* GetAccountManager() const;
    ItemStorageManager* GetItemStorageManager() const;
    CharacterStorageManager* GetCharacterStorageManager() const;
    LogStorageManager* GetLogStorageManager() const;
    
private:
    // Evitar cópias
    DBSrv(const DBSrv&) = delete;
    DBSrv& operator=(const DBSrv&) = delete;
    
    // Componentes internos
    std::unique_ptr<DatabaseManager> m_databaseManager;
    std::unique_ptr<NetworkManager> m_networkManager;
    std::unique_ptr<AccountManager> m_accountManager;
    std::unique_ptr<ItemStorageManager> m_itemStorageManager;
    std::unique_ptr<CharacterStorageManager> m_characterStorageManager;
    std::unique_ptr<LogStorageManager> m_logStorageManager;
    
    // Estado
    DatabaseStatus m_status;
    std::string m_configPath;
    uint16_t m_port;
    
    // Métodos internos
    bool LoadConfig();
    bool InitializeManagers();
    void ProcessDatabaseTasks();
};

} // namespace WYDBR

#endif // WYDBR_DBSRV_H
