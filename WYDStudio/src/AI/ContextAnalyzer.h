#pragma once

#include "AIKnowledgeBase.h"
#include <QMap>
#include <QString>
#include <nlohmann/json.hpp>

namespace wydbr {
namespace ai {

class ContextAnalyzer {
public:
    ContextAnalyzer();
    ~ContextAnalyzer() = default;

    // Estrutura de pesos para análise
    struct AnalysisWeights {
        float timeOfDay;         // Peso para análise temporal
        float serverLoad;        // Peso para carga do servidor
        float userHistory;       // Peso para histórico do usuário
        float commandComplexity; // Peso para complexidade do comando
    };

    // Análise principal de contexto
    float analyzeContext(const AIKnowledgeBase::CommandContext& context);

    // Análises específicas
    float analyzeTimeOfDay(const QDateTime& time);
    float analyzeServerMetrics(const nlohmann::json& metrics);
    float analyzeUserRole(const QString& role);
    float analyzeCommandSequence(const QString& previousCommand);

    // Configuração de pesos
    void setWeights(const AnalysisWeights& weights) { m_weights = weights; }
    const AnalysisWeights& getWeights() const { return m_weights; }

private:
    AnalysisWeights m_weights;
};

} // namespace ai
} // namespace wydbr
