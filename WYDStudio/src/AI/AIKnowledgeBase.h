#pragma once

#include <QString>
#include <QDateTime>
#include <QSqlDatabase>
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace wydbr {
namespace ai {

class AIKnowledgeBase {
public:
    explicit AIKnowledgeBase();
    ~AIKnowledgeBase();

    // Estruturas de dados
    struct CommandPattern {
        QString input;           // Comando original
        QString action;          // Ação executada
        float confidence;        // Confiança da correspondência
        nlohmann::json params;   // Parâmetros extraídos
        int successCount;        // Número de execuções bem-sucedidas
        QDateTime lastUsed;      // Última vez que foi usado
    };

    struct ServerState {
        nlohmann::json metrics;  // Métricas do servidor
        QString status;          // Status geral
        QDateTime timestamp;     // Momento da coleta
    };

    struct AdminAction {
        QString command;         // Comando executado
        QString result;          // Resultado da ação
        bool success;           // Se foi bem-sucedido
        QDateTime timestamp;     // Momento da execução
        QString adminName;       // Quem executou
    };

    // Nova estrutura para análise de contexto
    struct CommandContext {
        QString serverState;     // Estado do servidor no momento
        QString previousCommand; // Comando anterior
        QString userRole;        // Papel do usuário
        QDateTime timeOfDay;     // Hora do dia
        nlohmann::json metrics;  // Métricas relevantes
    };

    // Nova estrutura para tokens de comando
    struct CommandToken {
        QString value;          // Valor do token
        QString type;           // Tipo (ação, alvo, parâmetro, etc)
        float confidence;       // Confiança na classificação
        nlohmann::json meta;    // Metadados adicionais
    };

    // Gerenciamento de padrões
    void addCommandPattern(const CommandPattern& pattern);
    std::vector<CommandPattern> findSimilarPatterns(const QString& input, float minConfidence = 0.5f);
    void updatePatternSuccess(const QString& input, bool success);

    // Análise de estado
    void addServerState(const ServerState& state);
    std::vector<ServerState> getRecentStates(int count = 10);
    ServerState getAverageState(const QDateTime& start, const QDateTime& end);

    // Ações administrativas
    void logAdminAction(const AdminAction& action);
    std::vector<AdminAction> getAdminHistory(const QString& adminName = "", int limit = 100);
    float getActionSuccessRate(const QString& command);

    // Aprendizado
    void trainFromLogs(const QString& logPath);
    void trainFromAdminActions(const std::vector<AdminAction>& actions);
    void optimizePatterns();

    // Novos métodos para análise avançada
    std::vector<CommandToken> tokenizeCommand(const QString& command);
    nlohmann::json extractParameters(const QString& command, const CommandContext& context);
    float predictSuccess(const QString& command, const CommandContext& context);
    std::vector<QString> suggestCompletions(const QString& partialCommand);

    // Métodos para aprendizado avançado
    void learnFromSuccess(const QString& command, const CommandContext& context);
    void learnFromFailure(const QString& command, const QString& errorMessage);
    void analyzePatterns(const QDateTime& start, const QDateTime& end);

    // Métodos para otimização
    void mergeSimilarPatterns();
    void removeOutdatedPatterns();
    void rankPatternsByEffectiveness();

private:
    // Helpers
    float calculateSimilarity(const QString& str1, const QString& str2);
    nlohmann::json extractParameters(const QString& input);
    void cleanupOldData();

    // Novos helpers
    struct PatternAnalysis {
        int totalUses;
        int successCount;
        float avgConfidence;
        QDateTime firstUse;
        QDateTime lastUse;
        std::map<QString, int> commonErrors;
        std::map<QString, float> parameterStats;
    };

    // Análise de padrões
    PatternAnalysis analyzePattern(const CommandPattern& pattern);
    float calculateContextSimilarity(const CommandContext& ctx1, const CommandContext& ctx2);
    std::vector<QString> extractKeywords(const QString& command);
    void updatePatternStats(const CommandPattern& pattern, bool success);

    // Base de dados
    QSqlDatabase m_db;

    // Cache em memória
    struct Cache {
        std::vector<CommandPattern> recentPatterns;
        std::vector<ServerState> recentStates;
        std::vector<AdminAction> recentActions;
    } m_cache;

    // Cache de análise
    struct AnalysisCache {
        std::map<QString, PatternAnalysis> patternStats;
        std::map<QString, std::set<QString>> keywordMap;
        std::map<QString, float> successRates;
        std::vector<CommandToken> recentTokens;
    } m_analysisCache;

    // Configurações
    struct Config {
        int maxPatternAge;        // Dias para manter padrões
        int maxCacheSize;         // Número máximo de itens em cache
        float minConfidence;      // Confiança mínima para correspondência
        int cleanupInterval;      // Intervalo de limpeza em dias
    } m_config;

    // Configurações avançadas
    struct AdvancedConfig {
        float keywordWeight;     // Peso para correspondência de palavras-chave
        float contextWeight;     // Peso para similaridade de contexto
        float timeWeight;        // Peso para proximidade temporal
        float successWeight;     // Peso para taxa de sucesso
        int minPatternUses;     // Usos mínimos para considerar padrão válido
        float mergeThreshold;    // Limiar para mesclar padrões similares
    } m_advancedConfig;
};

} // namespace ai
} // namespace wydbr
