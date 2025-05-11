/**
 * @file SecurityMonitor.h
 * @brief Sistema de monitoramento de segurança para VoIP
 */

#ifndef WYDBR_VOIP_SECURITY_MONITOR_H
#define WYDBR_VOIP_SECURITY_MONITOR_H

#include "SecurityLogger.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>

namespace WYDBR {
namespace VoIP {

using ClientID = uint32_t;
using ChannelID = uint32_t;

struct SecurityEvent {
    std::chrono::system_clock::time_point timestamp;
    std::string type;
    std::string source;
    std::string details;
    SecurityLogLevel severity;
};

struct SecurityMetrics {
    size_t totalEvents;
    size_t criticalEvents;
    size_t errorEvents;
    size_t warningEvents;
    size_t failedLogins;
    size_t blockedIPs;
    size_t activeConnections;
    size_t activeChannels;
    std::chrono::system_clock::time_point lastUpdate;
};

class SecurityMonitor {
public:
    static SecurityMonitor& GetInstance();

    bool Initialize();
    void Shutdown();

    void StartMonitoring();
    void StopMonitoring();

    void AddEvent(const SecurityEvent& event);
    void AddFailedLogin(const std::string& username, const std::string& ip);
    void AddBlockedIP(const std::string& ip);
    void UpdateConnectionCount(size_t count);
    void UpdateChannelCount(size_t count);

    SecurityMetrics GetMetrics() const;
    std::vector<SecurityEvent> GetRecentEvents(size_t count = 100) const;
    std::vector<std::string> GetBlockedIPs() const;
    std::vector<std::string> GetFailedLogins() const;

    void SetAlertCallback(std::function<void(const SecurityEvent&)> callback);
    void SetMetricsCallback(std::function<void(const SecurityMetrics&)> callback);

    bool IsMonitoring() const;
    bool IsInitialized() const;

private:
    SecurityMonitor();
    ~SecurityMonitor();

    SecurityMonitor(const SecurityMonitor&) = delete;
    SecurityMonitor& operator=(const SecurityMonitor&) = delete;

    void MonitorThread();
    void ProcessEvents();
    void UpdateMetrics();
    void CheckThresholds();
    void Alert(const SecurityEvent& event);

    bool m_initialized;
    std::atomic<bool> m_monitoring;
    std::thread m_monitorThread;
    std::mutex m_monitorMutex;
    std::vector<SecurityEvent> m_events;
    std::unordered_map<std::string, size_t> m_failedLogins;
    std::unordered_map<std::string, size_t> m_blockedIPs;
    SecurityMetrics m_metrics;
    std::function<void(const SecurityEvent&)> m_alertCallback;
    std::function<void(const SecurityMetrics&)> m_metricsCallback;
    std::chrono::system_clock::time_point m_lastMetricsUpdate;
    size_t m_maxEvents;
    size_t m_maxFailedLogins;
    size_t m_maxBlockedIPs;
    std::chrono::seconds m_metricsUpdateInterval;
    std::chrono::seconds m_cleanupInterval;
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_SECURITY_MONITOR_H 