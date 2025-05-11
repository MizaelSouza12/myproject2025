#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>

namespace wyd {
namespace database {

class DatabaseManager {
public:
    static DatabaseManager& getInstance();

    // Configuração
    void initialize(const std::string& host, 
                   const std::string& user, 
                   const std::string& password, 
                   const std::string& database,
                   uint16_t port = 3306,
                   size_t poolSize = 10);

    // Gerenciamento de Conexões
    sqlpp::mysql::connection getConnection();
    void releaseConnection(sqlpp::mysql::connection& conn);

    // Transações
    template<typename Func>
    auto executeTransaction(Func&& func) -> decltype(func(std::declval<sqlpp::mysql::connection&>())) {
        auto conn = getConnection();
        try {
            conn.start_transaction();
            auto result = func(conn);
            conn.commit_transaction();
            return result;
        } catch (const std::exception& e) {
            conn.rollback_transaction();
            throw;
        } finally {
            releaseConnection(conn);
        }
    }

    // Cache
    template<typename T>
    void cacheData(const std::string& key, const T& data, std::chrono::seconds ttl) {
        std::lock_guard<std::mutex> lock(cacheMutex_);
        cache_[key] = CacheEntry{data, std::chrono::system_clock::now() + ttl};
    }

    template<typename T>
    std::optional<T> getCachedData(const std::string& key) {
        std::lock_guard<std::mutex> lock(cacheMutex_);
        auto it = cache_.find(key);
        if (it != cache_.end() && it->second.expiry > std::chrono::system_clock::now()) {
            return std::any_cast<T>(it->second.data);
        }
        return std::nullopt;
    }

    // Monitoramento
    struct DatabaseStats {
        size_t activeConnections;
        size_t queuedQueries;
        size_t totalQueries;
        size_t failedQueries;
        double averageQueryTime;
    };
    DatabaseStats getStats() const;

    // Backup
    void scheduleBackup(const std::string& path, 
                       std::chrono::system_clock::time_point time,
                       bool incremental = false);
    void restoreFromBackup(const std::string& backupPath);

private:
    DatabaseManager() = default;
    ~DatabaseManager();

    // Singleton
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Pool de Conexões
    struct ConnectionPool {
        std::queue<sqlpp::mysql::connection> connections;
        std::mutex mutex;
        std::condition_variable cv;
    };
    std::unique_ptr<ConnectionPool> pool_;

    // Cache
    struct CacheEntry {
        std::any data;
        std::chrono::system_clock::time_point expiry;
    };
    std::unordered_map<std::string, CacheEntry> cache_;
    mutable std::mutex cacheMutex_;

    // Monitoramento
    struct QueryMetrics {
        size_t totalQueries{0};
        size_t failedQueries{0};
        double totalQueryTime{0.0};
    };
    QueryMetrics metrics_;
    mutable std::mutex metricsMutex_;

    // Backup
    struct BackupJob {
        std::string path;
        std::chrono::system_clock::time_point scheduledTime;
        bool incremental;
    };
    std::vector<BackupJob> scheduledBackups_;
    std::mutex backupMutex_;

    // Thread de Manutenção
    std::thread maintenanceThread_;
    bool running_{true};
    void maintenanceLoop();
};

} // namespace database
} // namespace wyd 