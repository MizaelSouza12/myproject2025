#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace wydbr {
namespace monitoring {

struct ServerMetrics {
    double cpu_usage;
    double memory_usage;
    double disk_usage;
    double network_rx;
    double network_tx;
    std::chrono::system_clock::time_point timestamp;
};

class ServerMonitor {
private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_running;
    std::thread m_monitorThread;
    std::vector<ServerMetrics> m_metrics;
    
    // Configurações do banco de dados
    std::string m_dbHost;
    int m_dbPort;
    std::string m_dbUser;
    std::string m_dbPass;
    std::string m_dbName;
    
    void monitoringLoop();
    void collectMetrics();
    void saveMetrics(const ServerMetrics& metrics);
    void checkThresholds(const ServerMetrics& metrics);
    void notifyUI(const std::string& message, const std::string& type);

public:
    ServerMonitor();
    ~ServerMonitor();
    
    void start();
    void stop();
    void setDatabaseConfig(const std::string& host, int port, 
                          const std::string& user, const std::string& pass,
                          const std::string& dbName);
    
    std::vector<ServerMetrics> getMetricsHistory(int lastNMinutes = 60);
    ServerMetrics getCurrentMetrics();
    
    // Callbacks para a UI
    std::function<void(const std::string&, const std::string&)> onNotification;
    std::function<void(const ServerMetrics&)> onMetricsUpdate;
};

} // namespace monitoring
} // namespace wydbr 