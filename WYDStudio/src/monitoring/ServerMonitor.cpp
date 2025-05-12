#include "ServerMonitor.h"
#include <mysql/mysql.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace wydbr {
namespace monitoring {

ServerMonitor::ServerMonitor() : m_running(false) {}

ServerMonitor::~ServerMonitor() {
    stop();
}

void ServerMonitor::setDatabaseConfig(const std::string& host, int port,
                                    const std::string& user, const std::string& pass,
                                    const std::string& dbName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_dbHost = host;
    m_dbPort = port;
    m_dbUser = user;
    m_dbPass = pass;
    m_dbName = dbName;
}

void ServerMonitor::start() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_running) {
        m_running = true;
        m_monitorThread = std::thread(&ServerMonitor::monitoringLoop, this);
    }
}

void ServerMonitor::stop() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_running) return;
        m_running = false;
    }
    m_cv.notify_one();
    if (m_monitorThread.joinable()) {
        m_monitorThread.join();
    }
}

void ServerMonitor::monitoringLoop() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!m_running) break;
        }
        
        collectMetrics();
        
        // Espera 1 minuto antes da próxima coleta
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait_for(lock, std::chrono::minutes(1), [this]{ return !m_running; });
    }
}

void ServerMonitor::collectMetrics() {
    ServerMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::now();
    
    // CPU Usage
    std::ifstream stat("/proc/stat");
    std::string line;
    std::getline(stat, line);
    std::istringstream ss(line);
    std::string cpu;
    long user, nice, system, idle, iowait, irq, softirq;
    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
    
    long total = user + nice + system + idle + iowait + irq + softirq;
    long idle_total = idle + iowait;
    
    static long prev_total = 0;
    static long prev_idle = 0;
    
    long diff_total = total - prev_total;
    long diff_idle = idle_total - prev_idle;
    
    metrics.cpu_usage = (1.0 - (diff_idle / (double)diff_total)) * 100.0;
    
    prev_total = total;
    prev_idle = idle_total;
    
    // Memory Usage
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        unsigned long total_ram = si.totalram * si.mem_unit;
        unsigned long free_ram = si.freeram * si.mem_unit;
        metrics.memory_usage = ((total_ram - free_ram) / (double)total_ram) * 100.0;
    }
    
    // Disk Usage
    struct statvfs fs;
    if (statvfs("/", &fs) == 0) {
        unsigned long total = fs.f_blocks * fs.f_frsize;
        unsigned long free = fs.f_bfree * fs.f_frsize;
        metrics.disk_usage = ((total - free) / (double)total) * 100.0;
    }
    
    // Network (simplificado - você pode expandir isso)
    std::ifstream net("/proc/net/dev");
    std::string netline;
    while (std::getline(net, netline)) {
        if (netline.find("eth0") != std::string::npos) {
            std::istringstream netss(netline);
            std::string iface;
            netss >> iface >> metrics.network_rx >> metrics.network_tx;
            break;
        }
    }
    
    // Salva as métricas
    saveMetrics(metrics);
    
    // Verifica os limites
    checkThresholds(metrics);
    
    // Notifica a UI
    if (onMetricsUpdate) {
        onMetricsUpdate(metrics);
    }
}

void ServerMonitor::saveMetrics(const ServerMetrics& metrics) {
    MYSQL* conn = mysql_init(nullptr);
    if (mysql_real_connect(conn, m_dbHost.c_str(), m_dbUser.c_str(),
                          m_dbPass.c_str(), m_dbName.c_str(), m_dbPort,
                          nullptr, 0)) {
        std::stringstream query;
        query << "INSERT INTO server_metrics (cpu_usage, memory_usage, disk_usage, "
              << "network_rx, network_tx) VALUES ("
              << metrics.cpu_usage << ", "
              << metrics.memory_usage << ", "
              << metrics.disk_usage << ", "
              << metrics.network_rx << ", "
              << metrics.network_tx << ")";
        
        mysql_query(conn, query.str().c_str());
        mysql_close(conn);
    }
}

void ServerMonitor::checkThresholds(const ServerMetrics& metrics) {
    if (metrics.cpu_usage > 80.0) {
        notifyUI("Uso de CPU alto: " + std::to_string(metrics.cpu_usage) + "%", "warning");
    }
    if (metrics.memory_usage > 85.0) {
        notifyUI("Uso de memória alto: " + std::to_string(metrics.memory_usage) + "%", "warning");
    }
    if (metrics.disk_usage > 90.0) {
        notifyUI("Uso de disco alto: " + std::to_string(metrics.disk_usage) + "%", "warning");
    }
}

void ServerMonitor::notifyUI(const std::string& message, const std::string& type) {
    if (onNotification) {
        onNotification(message, type);
    }
}

std::vector<ServerMetrics> ServerMonitor::getMetricsHistory(int lastNMinutes) {
    std::vector<ServerMetrics> history;
    
    MYSQL* conn = mysql_init(nullptr);
    if (mysql_real_connect(conn, m_dbHost.c_str(), m_dbUser.c_str(),
                          m_dbPass.c_str(), m_dbName.c_str(), m_dbPort,
                          nullptr, 0)) {
        std::stringstream query;
        query << "SELECT cpu_usage, memory_usage, disk_usage, network_rx, network_tx, "
              << "created_at FROM server_metrics WHERE created_at >= "
              << "DATE_SUB(NOW(), INTERVAL " << lastNMinutes << " MINUTE) "
              << "ORDER BY created_at DESC";
        
        mysql_query(conn, query.str().c_str());
        MYSQL_RES* result = mysql_store_result(conn);
        
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                ServerMetrics metrics;
                metrics.cpu_usage = std::stod(row[0]);
                metrics.memory_usage = std::stod(row[1]);
                metrics.disk_usage = std::stod(row[2]);
                metrics.network_rx = std::stod(row[3]);
                metrics.network_tx = std::stod(row[4]);
                // Converte string timestamp para std::chrono
                std::tm tm = {};
                std::istringstream ss(row[5]);
                ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                metrics.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
                
                history.push_back(metrics);
            }
            mysql_free_result(result);
        }
        mysql_close(conn);
    }
    
    return history;
}

ServerMetrics ServerMonitor::getCurrentMetrics() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_metrics.empty()) {
        return m_metrics.back();
    }
    return ServerMetrics{};
}

} // namespace monitoring
} // namespace wydbr 