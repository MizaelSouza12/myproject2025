#include "MonitoringPanel.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDateTime>
#include <QValueAxis>

namespace wydbr {
namespace ui {

MonitoringPanel::MonitoringPanel(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_cpuBar(new QProgressBar(this))
    , m_memoryBar(new QProgressBar(this))
    , m_diskBar(new QProgressBar(this))
    , m_networkLabel(new QLabel(this))
    , m_updateTimer(new QTimer(this))
{
    setupUI();
    setupCharts();
    connectSignals();
}

MonitoringPanel::~MonitoringPanel() {
    if (m_monitor) {
        m_monitor->stop();
    }
}

void MonitoringPanel::setupUI() {
    // Configuração dos indicadores de uso atual
    auto* currentMetricsGroup = new QGroupBox("Métricas Atuais", this);
    auto* currentMetricsLayout = new QGridLayout;
    
    m_cpuBar->setRange(0, 100);
    m_memoryBar->setRange(0, 100);
    m_diskBar->setRange(0, 100);
    
    currentMetricsLayout->addWidget(new QLabel("CPU:"), 0, 0);
    currentMetricsLayout->addWidget(m_cpuBar, 0, 1);
    
    currentMetricsLayout->addWidget(new QLabel("Memória:"), 1, 0);
    currentMetricsLayout->addWidget(m_memoryBar, 1, 1);
    
    currentMetricsLayout->addWidget(new QLabel("Disco:"), 2, 0);
    currentMetricsLayout->addWidget(m_diskBar, 2, 1);
    
    currentMetricsLayout->addWidget(new QLabel("Rede:"), 3, 0);
    currentMetricsLayout->addWidget(m_networkLabel, 3, 1);
    
    currentMetricsGroup->setLayout(currentMetricsLayout);
    m_mainLayout->addWidget(currentMetricsGroup);
    
    // Configuração dos gráficos
    auto* chartsGroup = new QGroupBox("Histórico", this);
    auto* chartsLayout = new QGridLayout;
    
    m_cpuChartView = new QChartView(m_cpuChart);
    m_memoryChartView = new QChartView(m_memoryChart);
    m_diskChartView = new QChartView(m_diskChart);
    m_networkChartView = new QChartView(m_networkChart);
    
    chartsLayout->addWidget(m_cpuChartView, 0, 0);
    chartsLayout->addWidget(m_memoryChartView, 0, 1);
    chartsLayout->addWidget(m_diskChartView, 1, 0);
    chartsLayout->addWidget(m_networkChartView, 1, 1);
    
    chartsGroup->setLayout(chartsLayout);
    m_mainLayout->addWidget(chartsGroup);
    
    setLayout(m_mainLayout);
}

void MonitoringPanel::setupCharts() {
    // CPU Chart
    m_cpuChart = new QChart();
    m_cpuSeries = new QLineSeries();
    m_cpuChart->addSeries(m_cpuSeries);
    m_cpuChart->setTitle("Uso de CPU");
    
    // Memory Chart
    m_memoryChart = new QChart();
    m_memorySeries = new QLineSeries();
    m_memoryChart->addSeries(m_memorySeries);
    m_memoryChart->setTitle("Uso de Memória");
    
    // Disk Chart
    m_diskChart = new QChart();
    m_diskSeries = new QLineSeries();
    m_diskChart->addSeries(m_diskSeries);
    m_diskChart->setTitle("Uso de Disco");
    
    // Network Chart
    m_networkChart = new QChart();
    m_networkRxSeries = new QLineSeries();
    m_networkTxSeries = new QLineSeries();
    m_networkChart->addSeries(m_networkRxSeries);
    m_networkChart->addSeries(m_networkTxSeries);
    m_networkChart->setTitle("Tráfego de Rede");
    
    // Configuração dos eixos
    for (auto* chart : {m_cpuChart, m_memoryChart, m_diskChart}) {
        auto* axisX = new QValueAxis;
        auto* axisY = new QValueAxis;
        
        axisX->setRange(0, 60); // 60 minutos
        axisY->setRange(0, 100); // Porcentagem
        
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        
        chart->legend()->hide();
    }
    
    // Configuração específica para o gráfico de rede
    auto* networkAxisX = new QValueAxis;
    auto* networkAxisY = new QValueAxis;
    
    networkAxisX->setRange(0, 60);
    networkAxisY->setRange(0, 1000); // MB/s
    
    m_networkChart->addAxis(networkAxisX, Qt::AlignBottom);
    m_networkChart->addAxis(networkAxisY, Qt::AlignLeft);
    
    m_networkRxSeries->setName("Download");
    m_networkTxSeries->setName("Upload");
}

void MonitoringPanel::connectSignals() {
    m_updateTimer->setInterval(1000); // Atualiza a cada segundo
    connect(m_updateTimer, &QTimer::timeout, this, &MonitoringPanel::updateCharts);
    m_updateTimer->start();
}

void MonitoringPanel::setServerMonitor(std::shared_ptr<monitoring::ServerMonitor> monitor) {
    m_monitor = monitor;
    
    // Conecta os callbacks do monitor
    m_monitor->onMetricsUpdate = [this](const monitoring::ServerMetrics& metrics) {
        QMetaObject::invokeMethod(this, "updateMetrics",
                                Q_ARG(monitoring::ServerMetrics, metrics));
    };
    
    m_monitor->onNotification = [this](const std::string& message, const std::string& type) {
        QMetaObject::invokeMethod(this, "showNotification",
                                Q_ARG(QString, QString::fromStdString(message)),
                                Q_ARG(QString, QString::fromStdString(type)));
    };
}

void MonitoringPanel::updateMetrics(const monitoring::ServerMetrics& metrics) {
    // Atualiza os indicadores
    m_cpuBar->setValue(static_cast<int>(metrics.cpu_usage));
    m_memoryBar->setValue(static_cast<int>(metrics.memory_usage));
    m_diskBar->setValue(static_cast<int>(metrics.disk_usage));
    
    // Atualiza o label de rede
    QString networkText = QString("RX: %1 MB/s | TX: %2 MB/s")
        .arg(metrics.network_rx / 1024.0 / 1024.0, 0, 'f', 2)
        .arg(metrics.network_tx / 1024.0 / 1024.0, 0, 'f', 2);
    m_networkLabel->setText(networkText);
    
    // Define cores baseadas nos limites
    auto setBarColor = [](QProgressBar* bar, double value) {
        QString style;
        if (value > 90) {
            style = "QProgressBar::chunk { background-color: red; }";
        } else if (value > 75) {
            style = "QProgressBar::chunk { background-color: orange; }";
        } else {
            style = "QProgressBar::chunk { background-color: green; }";
        }
        bar->setStyleSheet(style);
    };
    
    setBarColor(m_cpuBar, metrics.cpu_usage);
    setBarColor(m_memoryBar, metrics.memory_usage);
    setBarColor(m_diskBar, metrics.disk_usage);
}

void MonitoringPanel::showNotification(const QString& message, const QString& type) {
    QMessageBox::Icon icon = QMessageBox::Information;
    if (type == "warning") {
        icon = QMessageBox::Warning;
    } else if (type == "error") {
        icon = QMessageBox::Critical;
    }
    
    QMessageBox msgBox(icon, "Monitoramento do Servidor", message, QMessageBox::Ok, this);
    msgBox.exec();
}

void MonitoringPanel::updateCharts() {
    if (!m_monitor) return;
    
    auto metrics = m_monitor->getMetricsHistory(60); // Últimos 60 minutos
    
    // Limpa as séries
    m_cpuSeries->clear();
    m_memorySeries->clear();
    m_diskSeries->clear();
    m_networkRxSeries->clear();
    m_networkTxSeries->clear();
    
    // Adiciona os novos pontos
    for (const auto& metric : metrics) {
        auto timestamp = std::chrono::duration_cast<std::chrono::minutes>(
            metric.timestamp.time_since_epoch()).count() % 60;
            
        m_cpuSeries->append(timestamp, metric.cpu_usage);
        m_memorySeries->append(timestamp, metric.memory_usage);
        m_diskSeries->append(timestamp, metric.disk_usage);
        m_networkRxSeries->append(timestamp, metric.network_rx / 1024.0 / 1024.0);
        m_networkTxSeries->append(timestamp, metric.network_tx / 1024.0 / 1024.0);
    }
}

} // namespace ui
} // namespace wydbr 