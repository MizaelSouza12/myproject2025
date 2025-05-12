#include "ContextAnalyzer.h"
#include <QDateTime>
#include <QRegExp>
#include <algorithm>
#include <cmath>

namespace wydbr {
namespace ai {

ContextAnalyzer::ContextAnalyzer() {
    // Inicialização dos pesos de análise
    m_weights = {
        .timeOfDay = 0.2f,
        .serverLoad = 0.3f,
        .userHistory = 0.3f,
        .commandComplexity = 0.2f
    };
}

float ContextAnalyzer::analyzeContext(const AIKnowledgeBase::CommandContext& context) {
    float score = 0.0f;

    // Análise temporal
    score += analyzeTimeOfDay(context.timeOfDay) * m_weights.timeOfDay;

    // Análise de estado do servidor
    if (!context.metrics.empty()) {
        score += analyzeServerMetrics(context.metrics) * m_weights.serverLoad;
    }

    // Análise de histórico do usuário
    if (!context.userRole.isEmpty()) {
        score += analyzeUserRole(context.userRole) * m_weights.userHistory;
    }

    // Análise de comando anterior
    if (!context.previousCommand.isEmpty()) {
        score += analyzeCommandSequence(context.previousCommand) * m_weights.commandComplexity;
    }

    return std::min(1.0f, std::max(0.0f, score));
}

float ContextAnalyzer::analyzeTimeOfDay(const QDateTime& time) {
    // Análise do horário do dia
    int hour = time.time().hour();

    // Períodos de maior atividade (8h-22h) recebem pontuação mais alta
    if (hour >= 8 && hour <= 22) {
        return 0.8f + (0.2f * (1.0f - std::abs(15 - hour) / 7.0f));
    }

    // Madrugada (22h-8h) recebe pontuação mais baixa
    return 0.5f + (0.3f * (1.0f - std::abs(3 - hour) / 5.0f));
}

float ContextAnalyzer::analyzeServerMetrics(const nlohmann::json& metrics) {
    float score = 0.0f;
    int numMetrics = 0;

    // Análise de CPU
    if (metrics.contains("cpu_usage")) {
        float cpuUsage = metrics["cpu_usage"].get<float>();
        score += (cpuUsage < 80.0f) ? 1.0f : (100.0f - cpuUsage) / 20.0f;
        numMetrics++;
    }

    // Análise de memória
    if (metrics.contains("memory_usage")) {
        float memUsage = metrics["memory_usage"].get<float>();
        score += (memUsage < 85.0f) ? 1.0f : (100.0f - memUsage) / 15.0f;
        numMetrics++;
    }

    // Análise de jogadores online
    if (metrics.contains("players_online")) {
        int playersOnline = metrics["players_online"].get<int>();
        score += (playersOnline < 1000) ? 1.0f : 1000.0f / playersOnline;
        numMetrics++;
    }

    // Análise de latência
    if (metrics.contains("latency")) {
        float latency = metrics["latency"].get<float>();
        score += (latency < 100.0f) ? 1.0f : 100.0f / latency;
        numMetrics++;
    }

    return numMetrics > 0 ? score / numMetrics : 0.5f;
}

float ContextAnalyzer::analyzeUserRole(const QString& role) {
    // Análise baseada no papel do usuário
    static const QMap<QString, float> roleWeights = {
        {"owner", 1.0f},
        {"admin", 0.9f},
        {"moderator", 0.7f},
        {"helper", 0.5f},
        {"tester", 0.3f}
    };

    return roleWeights.value(role.toLower(), 0.1f);
}

float ContextAnalyzer::analyzeCommandSequence(const QString& previousCommand) {
    // Análise de sequência de comandos
    float complexity = 0.0f;

    // Verifica comandos perigosos
    static const QStringList dangerousPatterns = {
        "delete", "drop", "reset", "clear", "remove",
        "ban", "kick", "mute", "punish", "block"
    };

    // Verifica comandos administrativos
    static const QStringList adminPatterns = {
        "create", "add", "set", "config", "modify",
        "update", "change", "enable", "disable"
    };

    // Análise de padrões perigosos
    for (const QString& pattern : dangerousPatterns) {
        if (previousCommand.contains(pattern, Qt::CaseInsensitive)) {
            complexity += 0.3f;
        }
    }

    // Análise de padrões administrativos
    for (const QString& pattern : adminPatterns) {
        if (previousCommand.contains(pattern, Qt::CaseInsensitive)) {
            complexity += 0.2f;
        }
    }

    // Análise de parâmetros
    QRegExp numericPattern("\\d+");
    int numParams = previousCommand.count(numericPattern);
    complexity += std::min(0.3f, numParams * 0.1f);

    return std::min(1.0f, complexity);
}

} // namespace ai
} // namespace wydbr
