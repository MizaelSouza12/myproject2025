/**
 * @file SecurityMonitor.cpp
 * @brief Implementação do sistema de monitoramento de segurança para VoIP
 */

#include "SecurityMonitor.h"
#include <algorithm>
#include <chrono>

namespace WYDBR {
namespace VoIP {

SecurityMonitor& SecurityMonitor::GetInstance()
{
    static SecurityMonitor instance;
    return instance;
}

SecurityMonitor::SecurityMonitor()
    : m_initialized(false)
    , m_monitoring(false)
    , m_maxEvents(1000)
    , m_maxFailedLogins(100)
    , m_maxBlockedIPs(100)
    , m_metricsUpdateInterval(std::chrono::seconds(60))
    , m_cleanupInterval(std::chrono::seconds(300))
{
    m_metrics = SecurityMetrics{};
}

SecurityMonitor::~SecurityMonitor()
{
    Shutdown();
}

bool SecurityMonitor::Initialize()
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

void SecurityMonitor::Shutdown()
{
    StopMonitoring();

    std::lock_guard<std::mutex> lock(m_monitorMutex);
    m_initialized = false;
    m_events.clear();
    m_failedLogins.clear();
    m_blockedIPs.clear();
}

void SecurityMonitor::StartMonitoring()
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    if (!m_initialized || m_monitoring) {
        return;
    }

    m_monitoring = true;
    m_monitorThread = std::thread(&SecurityMonitor::MonitorThread, this);
}

void SecurityMonitor::StopMonitoring()
{
    {
        std::lock_guard<std::mutex> lock(m_monitorMutex);
        if (!m_monitoring) {
            return;
        }

        m_monitoring = false;
    }

    if (m_monitorThread.joinable()) {
        m_monitorThread.join();
    }
}

void SecurityMonitor::AddEvent(const SecurityEvent& event)
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    if (!m_initialized) {
        return;
    }

    m_events.push_back(event);

    // Manter apenas os eventos mais recentes
    if (m_events.size() > m_maxEvents) {
        m_events.erase(m_events.begin(), m_events.begin() + (m_events.size() - m_maxEvents));
    }

    // Registrar evento no logger
    SecurityLogger::GetInstance().Log(event.severity, event.type, event.source, event.details);
}

void SecurityMonitor::AddFailedLogin(const std::string& username, const std::string& ip)
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    if (!m_initialized) {
        return;
    }

    std::string key = username + "@" + ip;
    m_failedLogins[key]++;

    // Criar evento de falha de login
    SecurityEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.type = "FailedLogin";
    event.source = username;
    event.details = "Failed login attempt from IP: " + ip;
    event.severity = SecurityLogLevel::WARNING;

    AddEvent(event);
}

void SecurityMonitor::AddBlockedIP(const std::string& ip)
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    if (!m_initialized) {
        return;
    }

    m_blockedIPs[ip]++;

    // Criar evento de IP bloqueado
    SecurityEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.type = "BlockedIP";
    event.source = ip;
    event.details = "IP address blocked due to suspicious activity";
    event.severity = SecurityLogLevel::WARNING;

    AddEvent(event);
}

void SecurityMonitor::UpdateConnectionCount(size_t count)
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);
    m_metrics.activeConnections = count;
}

void SecurityMonitor::UpdateChannelCount(size_t count)
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);
    m_metrics.activeChannels = count;
}

SecurityMetrics SecurityMonitor::GetMetrics() const
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);
    return m_metrics;
}

std::vector<SecurityEvent> SecurityMonitor::GetRecentEvents(size_t count) const
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    if (m_events.empty()) {
        return std::vector<SecurityEvent>();
    }

    size_t start = m_events.size() > count ? m_events.size() - count : 0;
    return std::vector<SecurityEvent>(m_events.begin() + start, m_events.end());
}

std::vector<std::string> SecurityMonitor::GetBlockedIPs() const
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    std::vector<std::string> ips;
    ips.reserve(m_blockedIPs.size());

    for (const auto& pair : m_blockedIPs) {
        ips.push_back(pair.first);
    }

    return ips;
}

std::vector<std::string> SecurityMonitor::GetFailedLogins() const
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    std::vector<std::string> logins;
    logins.reserve(m_failedLogins.size());

    for (const auto& pair : m_failedLogins) {
        logins.push_back(pair.first);
    }

    return logins;
}

void SecurityMonitor::SetAlertCallback(std::function<void(const SecurityEvent&)> callback)
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);
    m_alertCallback = callback;
}

void SecurityMonitor::SetMetricsCallback(std::function<void(const SecurityMetrics&)> callback)
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);
    m_metricsCallback = callback;
}

bool SecurityMonitor::IsMonitoring() const
{
    return m_monitoring;
}

bool SecurityMonitor::IsInitialized() const
{
    return m_initialized;
}

void SecurityMonitor::MonitorThread()
{
    while (m_monitoring) {
        ProcessEvents();
        UpdateMetrics();
        CheckThresholds();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void SecurityMonitor::ProcessEvents()
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    // Limpar eventos antigos
    auto now = std::chrono::system_clock::now();
    auto it = std::remove_if(m_events.begin(), m_events.end(),
        [&](const SecurityEvent& event) {
            return (now - event.timestamp) > m_cleanupInterval;
        });

    if (it != m_events.end()) {
        m_events.erase(it, m_events.end());
    }

    // Limpar falhas de login antigas
    for (auto it = m_failedLogins.begin(); it != m_failedLogins.end();) {
        if (it->second > m_maxFailedLogins) {
            it = m_failedLogins.erase(it);
        } else {
            ++it;
        }
    }

    // Limpar IPs bloqueados antigos
    for (auto it = m_blockedIPs.begin(); it != m_blockedIPs.end();) {
        if (it->second > m_maxBlockedIPs) {
            it = m_blockedIPs.erase(it);
        } else {
            ++it;
        }
    }
}

void SecurityMonitor::UpdateMetrics()
{
    auto now = std::chrono::system_clock::now();
    if (now - m_lastMetricsUpdate < m_metricsUpdateInterval) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_monitorMutex);

    m_metrics.totalEvents = m_events.size();
    m_metrics.failedLogins = m_failedLogins.size();
    m_metrics.blockedIPs = m_blockedIPs.size();

    // Contar eventos por severidade
    m_metrics.criticalEvents = std::count_if(m_events.begin(), m_events.end(),
        [](const SecurityEvent& event) { return event.severity == SecurityLogLevel::CRITICAL; });
    m_metrics.errorEvents = std::count_if(m_events.begin(), m_events.end(),
        [](const SecurityEvent& event) { return event.severity == SecurityLogLevel::ERROR; });
    m_metrics.warningEvents = std::count_if(m_events.begin(), m_events.end(),
        [](const SecurityEvent& event) { return event.severity == SecurityLogLevel::WARNING; });

    m_metrics.lastUpdate = now;

    if (m_metricsCallback) {
        m_metricsCallback(m_metrics);
    }
}

void SecurityMonitor::CheckThresholds()
{
    std::lock_guard<std::mutex> lock(m_monitorMutex);

    // Verificar número de falhas de login
    for (const auto& pair : m_failedLogins) {
        if (pair.second >= 5) { // Limite de 5 tentativas
            SecurityEvent event;
            event.timestamp = std::chrono::system_clock::now();
            event.type = "LoginThresholdExceeded";
            event.source = pair.first;
            event.details = "Multiple failed login attempts detected";
            event.severity = SecurityLogLevel::WARNING;

            Alert(event);
        }
    }

    // Verificar número de IPs bloqueados
    if (m_blockedIPs.size() >= 10) { // Limite de 10 IPs bloqueados
        SecurityEvent event;
        event.timestamp = std::chrono::system_clock::now();
        event.type = "BlockedIPThresholdExceeded";
        event.source = "SecurityMonitor";
        event.details = "High number of blocked IPs detected";
        event.severity = SecurityLogLevel::WARNING;

        Alert(event);
    }

    // Verificar número de eventos críticos
    if (m_metrics.criticalEvents >= 5) { // Limite de 5 eventos críticos
        SecurityEvent event;
        event.timestamp = std::chrono::system_clock::now();
        event.type = "CriticalEventThresholdExceeded";
        event.source = "SecurityMonitor";
        event.details = "High number of critical security events detected";
        event.severity = SecurityLogLevel::CRITICAL;

        Alert(event);
    }
}

void SecurityMonitor::Alert(const SecurityEvent& event)
{
    if (m_alertCallback) {
        m_alertCallback(event);
    }

    AddEvent(event);
}

} // namespace VoIP
} // namespace WYDBR 