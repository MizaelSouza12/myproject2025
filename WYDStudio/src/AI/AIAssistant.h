#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

namespace wydbr {
namespace ai {

class AIAssistant : public QObject {
    Q_OBJECT

public:
    explicit AIAssistant(QObject* parent = nullptr);
    ~AIAssistant();

    // Interface principal para comandos em linguagem natural
    void processCommand(const QString& command);

    // Sugestões automáticas
    void suggestActions();

    // Monitoramento inteligente
    void startMonitoring();
    void stopMonitoring();

signals:
    // Sinais para notificar a interface
    void commandProcessed(const QString& result);
    void suggestionAvailable(const QString& suggestion);
    void alertGenerated(const QString& alert, int severity);
    void actionRequired(const QString& action, const QString& reason);

private slots:
    void onMonitoringTick();
    void onAIModelResponse(const QString& response);

private:
    // Análise de comandos
    struct CommandIntent {
        QString action;
        nlohmann::json parameters;
        float confidence;
    };

    CommandIntent analyzeCommand(const QString& command);
    void executeCommand(const CommandIntent& intent);

    // Monitoramento e análise
    void analyzeServerState();
    void detectAnomalies();
    void generateSuggestions();

    // Helpers
    QString translateCommandToAction(const QString& command);
    bool validateAction(const QString& action, const nlohmann::json& params);
    void logAction(const QString& action, const nlohmann::json& params);

    // Estado interno
    bool m_isMonitoring;
    QTimer m_monitorTimer;
    std::vector<QString> m_recentCommands;
    nlohmann::json m_serverState;

    // Configurações
    struct {
        int monitoringInterval;
        float confidenceThreshold;
        bool autoExecute;
        QString modelEndpoint;
    } m_config;

    // Cache de modelos AI
    struct ModelCache {
        QString name;
        QString version;
        QDateTime lastUpdate;
        nlohmann::json parameters;
    };
    std::vector<ModelCache> m_modelCache;
};

} // namespace ai
} // namespace wydbr