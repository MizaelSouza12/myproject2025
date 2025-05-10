/**
 * @file DatabaseManager.h
 * @brief Gerenciador seguro de banco de dados para WYDBR 2.0
 * 
 * Implementa acesso seguro ao banco de dados com proteção contra SQL injection
 * e gerenciamento eficiente de conexões.
 */

#ifndef WYDBR_DATABASE_MANAGER_H
#define WYDBR_DATABASE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace WYDBR {
namespace Database {

/**
 * @class PreparedStatement
 * @brief Representa uma consulta SQL pré-compilada
 */
class PreparedStatement {
public:
    PreparedStatement(void* stmt_handle);
    ~PreparedStatement();
    
    void BindInt(int param_index, int value);
    void BindInt64(int param_index, int64_t value);
    void BindDouble(int param_index, double value);
    void BindString(int param_index, const std::string& value);
    void BindBlob(int param_index, const void* data, size_t size);
    void BindNull(int param_index);
    
    bool Execute();
    bool FetchNext();
    int GetInt(int column_index);
    int64_t GetInt64(int column_index);
    double GetDouble(int column_index);
    std::string GetString(int column_index);
    std::vector<uint8_t> GetBlob(int column_index);
    bool IsNull(int column_index);
    
private:
    void* stmt_handle_; // Ponteiro opaco para o statement nativo
    bool has_result_;
};

/**
 * @class DatabaseConnection
 * @brief Representa uma conexão com o banco de dados
 */
class DatabaseConnection {
public:
    DatabaseConnection(const std::string& connection_string);
    ~DatabaseConnection();
    
    bool Connect();
    void Disconnect();
    bool IsConnected() const;
    
    std::shared_ptr<PreparedStatement> PrepareStatement(const std::string& sql);
    bool ExecuteNonQuery(const std::string& sql);
    int64_t GetLastInsertId();
    
private:
    std::string connection_string_;
    void* db_handle_; // Ponteiro opaco para o handle de conexão
    bool is_connected_;
};

/**
 * @class DatabaseManager
 * @brief Gerencia conexões e acesso ao banco de dados
 */
class DatabaseManager {
public:
    static DatabaseManager& GetInstance();
    
    bool Initialize(const std::string& connection_string);
    std::shared_ptr<DatabaseConnection> GetConnection();
    void ReleaseConnection(std::shared_ptr<DatabaseConnection> connection);
    
    // Métodos auxiliares para operações comuns
    bool ExecuteNonQuery(const std::string& sql);
    std::vector<std::unordered_map<std::string, std::string>> ExecuteQuery(const std::string& sql);
    
private:
    DatabaseManager();
    ~DatabaseManager();
    
    // Proibir cópia e atribuição
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    std::string connection_string_;
    std::vector<std::shared_ptr<DatabaseConnection>> connection_pool_;
    std::mutex mutex_;
    bool is_initialized_;
    
    const size_t MAX_POOL_SIZE = 10;
};

} // namespace Database
} // namespace WYDBR

#endif // WYDBR_DATABASE_MANAGER_H
