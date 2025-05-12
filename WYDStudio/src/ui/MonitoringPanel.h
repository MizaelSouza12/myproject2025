#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QTimer>
#include "../monitoring/ServerMonitor.h"

QT_CHARTS_USE_NAMESPACE

namespace wydbr {
namespace ui {

class MonitoringPanel : public QWidget {
    Q_OBJECT

public:
    explicit MonitoringPanel(QWidget* parent = nullptr);
    ~MonitoringPanel();

    void setServerMonitor(std::shared_ptr<monitoring::ServerMonitor> monitor);

private slots:
    void updateMetrics(const monitoring::ServerMetrics& metrics);
    void showNotification(const QString& message, const QString& type);
    void updateCharts();

private:
    std::shared_ptr<monitoring::ServerMonitor> m_monitor;
    
    // UI Elements
    QVBoxLayout* m_mainLayout;
    
    // Current metrics display
    QProgressBar* m_cpuBar;
    QProgressBar* m_memoryBar;
    QProgressBar* m_diskBar;
    QLabel* m_networkLabel;
    
    // Charts
    QChart* m_cpuChart;
    QChart* m_memoryChart;
    QChart* m_diskChart;
    QChart* m_networkChart;
    
    QChartView* m_cpuChartView;
    QChartView* m_memoryChartView;
    QChartView* m_diskChartView;
    QChartView* m_networkChartView;
    
    QLineSeries* m_cpuSeries;
    QLineSeries* m_memorySeries;
    QLineSeries* m_diskSeries;
    QLineSeries* m_networkRxSeries;
    QLineSeries* m_networkTxSeries;
    
    QTimer* m_updateTimer;
    
    void setupUI();
    void setupCharts();
    void connectSignals();
};

} // namespace ui
} // namespace wydbr 