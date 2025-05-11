#include "DatabaseManager.hpp"
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace wyd {
namespace database {

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

void DatabaseManager::initialize(const std::string& host,
                               const std::string& user,
                               const std::string& password,
                               const std::string& database,
                               uint16_t port,
                               size_t poolSize) {
    pool_ = std::make_unique<ConnectionPool>();
    
    // Configuração do MySQL
    sqlpp::mysql::connection_config config;
    config.host = host;
    config.user = user;
    config.password = password;
    config.database = database;
    config.port = port;
    config.auto_reconnect = true;
    config.allow_multi_queries = true;
    config.charset = "utf8mb4";
    config.collation = "utf8mb4_unicode_ci";

    // Inicializa o pool de conexões
    for (size_t i = 0; i < poolSize; ++i) {
        pool_->connections.push(sqlpp::mysql::connection(config));
    }

    // Inicia thread de manutenção
    maintenanceThread_ = std::thread(&DatabaseManager::maintenanceLoop, this);
}

DatabaseManager::~DatabaseManager() {
    running_ = false;
    if (maintenanceThread_.joinable()) {
        maintenanceThread_.join();
    }
}

sqlpp::mysql::connection DatabaseManager::getConnection() {
    std::unique_lock<std::mutex> lock(pool_->mutex);
    while (pool_->connections.empty()) {
        pool_->cv.wait(lock);
    }
    
    auto conn = std::move(pool_->connections.front());
    pool_->connections.pop();
    return conn;
}

void DatabaseManager::releaseConnection(sqlpp::mysql::connection& conn) {
    std::lock_guard<std::mutex> lock(pool_->mutex);
    pool_->connections.push(std::move(conn));
    pool_->cv.notify_one();
}

DatabaseManager::DatabaseStats DatabaseManager::getStats() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return DatabaseStats{
        pool_->connections.size(),
        0, // TODO: Implementar contagem de queries na fila
        metrics_.totalQueries,
        metrics_.failedQueries,
        metrics_.totalQueries > 0 ? metrics_.totalQueryTime / metrics_.totalQueries : 0.0
    };
}

void DatabaseManager::scheduleBackup(const std::string& path,
                                   std::chrono::system_clock::time_point time,
                                   bool incremental) {
    std::lock_guard<std::mutex> lock(backupMutex_);
    scheduledBackups_.push_back({path, time, incremental});
}

void DatabaseManager::restoreFromBackup(const std::string& backupPath) {
    if (!std::filesystem::exists(backupPath)) {
        throw std::runtime_error("Backup file not found: " + backupPath);
    }

    auto conn = getConnection();
    try {
        // Desativa chaves estrangeiras temporariamente
        conn.execute("SET FOREIGN_KEY_CHECKS = 0");

        // Lê e executa o arquivo de backup
        std::ifstream backupFile(backupPath);
        std::string line;
        while (std::getline(backupFile, line)) {
            if (!line.empty() && line[0] != '-' && line[0] != '/') {
                conn.execute(line);
            }
        }

        // Reativa chaves estrangeiras
        conn.execute("SET FOREIGN_KEY_CHECKS = 1");
    } catch (const std::exception& e) {
        conn.execute("SET FOREIGN_KEY_CHECKS = 1");
        throw;
    }
    releaseConnection(conn);
}

void DatabaseManager::maintenanceLoop() {
    while (running_) {
        try {
            // Limpa cache expirado
            {
                std::lock_guard<std::mutex> lock(cacheMutex_);
                auto now = std::chrono::system_clock::now();
                for (auto it = cache_.begin(); it != cache_.end();) {
                    if (it->second.expiry <= now) {
                        it = cache_.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            // Verifica backups agendados
            {
                std::lock_guard<std::mutex> lock(backupMutex_);
                auto now = std::chrono::system_clock::now();
                for (auto it = scheduledBackups_.begin(); it != scheduledBackups_.end();) {
                    if (it->scheduledTime <= now) {
                        // Executa backup
                        std::stringstream filename;
                        filename << it->path << "/backup_"
                                << std::put_time(std::localtime(&std::time_t(now.time_since_epoch().count())), "%Y%m%d_%H%M%S")
                                << (it->incremental ? "_inc" : "_full")
                                << ".sql";

                        auto conn = getConnection();
                        try {
                            if (it->incremental) {
                                // Backup incremental (apenas alterações desde último backup)
                                // TODO: Implementar lógica de backup incremental
                            } else {
                                // Backup completo
                                conn.execute("FLUSH TABLES WITH READ LOCK");
                                // TODO: Implementar mysqldump ou similar
                                conn.execute("UNLOCK TABLES");
                            }
                        } catch (const std::exception& e) {
                            // Log error
                        }
                        releaseConnection(conn);

                        it = scheduledBackups_.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            // Verifica conexões do pool
            {
                std::lock_guard<std::mutex> lock(pool_->mutex);
                for (auto& conn : pool_->connections) {
                    try {
                        conn.execute("SELECT 1");
                    } catch (const std::exception& e) {
                        // Reconecta se necessário
                        // TODO: Implementar reconexão
                    }
                }
            }

        } catch (const std::exception& e) {
            // Log error
        }

        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

} // namespace database
} // namespace wyd 