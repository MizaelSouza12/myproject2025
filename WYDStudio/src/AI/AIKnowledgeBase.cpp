#include "AIKnowledgeBase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace wydbr {
namespace ai {

AIKnowledgeBase::AIKnowledgeBase() {
    // Inicialização do banco de dados SQLite
    m_db = QSqlDatabase::addDatabase("QSQLITE", "ai_knowledge");
    m_db.setDatabaseName("ai_knowledge.db");

    if (!m_db.open()) {
        spdlog::error("Erro ao abrir banco de dados: {}",
            m_db.lastError().text().toStdString());
        return;
    }

    // Configurações padrão
    m_config = {
        .maxPatternAge = 90,      // 90 dias
        .maxCacheSize = 1000,     // 1000 itens
        .minConfidence = 0.6f,    // 60% confiança mínima
        .cleanupInterval = 7      // Limpeza semanal
    };

    m_advancedConfig = {
        .keywordWeight = 0.4f,
        .contextWeight = 0.3f,
        .timeWeight = 0.1f,
        .successWeight = 0.2f,
        .minPatternUses = 5,
        .mergeThreshold = 0.85f
    };

    // Criação das tabelas
    QSqlQuery query(m_db);

    // Tabela de padrões de comando
    query.exec(
        "CREATE TABLE IF NOT EXISTS command_patterns ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "input TEXT NOT NULL,"
        "action TEXT NOT NULL,"
        "confidence REAL,"
        "params JSON,"
        "success_count INTEGER DEFAULT 0,"
        "last_used DATETIME,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)"
    );

    // Tabela de estados do servidor
    query.exec(
        "CREATE TABLE IF NOT EXISTS server_states ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "metrics JSON,"
        "status TEXT,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)"
    );

    // Tabela de ações administrativas
    query.exec(
        "CREATE TABLE IF NOT EXISTS admin_actions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "command TEXT NOT NULL,"
        "result TEXT,"
        "success BOOLEAN,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "admin_name TEXT)"
    );

    // Índices para otimização
    query.exec("CREATE INDEX IF NOT EXISTS idx_patterns_input ON command_patterns(input)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_patterns_confidence ON command_patterns(confidence)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_states_timestamp ON server_states(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_actions_admin ON admin_actions(admin_name, timestamp)");

    // Carregar cache inicial
    loadCache();
}

AIKnowledgeBase::~AIKnowledgeBase() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

void AIKnowledgeBase::addCommandPattern(const CommandPattern& pattern) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO command_patterns (input, action, confidence, params, success_count, last_used) "
        "VALUES (:input, :action, :confidence, :params, :success_count, :last_used)"
    );

    query.bindValue(":input", pattern.input);
    query.bindValue(":action", pattern.action);
    query.bindValue(":confidence", pattern.confidence);
    query.bindValue(":params", QString::fromStdString(pattern.params.dump()));
    query.bindValue(":success_count", pattern.successCount);
    query.bindValue(":last_used", pattern.lastUsed);

    if (!query.exec()) {
        spdlog::error("Erro ao adicionar padrão: {}",
            query.lastError().text().toStdString());
        return;
    }

    // Atualiza cache
    m_cache.recentPatterns.push_back(pattern);
    if (m_cache.recentPatterns.size() > m_config.maxCacheSize) {
        m_cache.recentPatterns.erase(m_cache.recentPatterns.begin());
    }
}

std::vector<AIKnowledgeBase::CommandPattern> AIKnowledgeBase::findSimilarPatterns(
    const QString& input, float minConfidence) {

    std::vector<CommandPattern> results;

    // Primeiro, busca no cache
    for (const auto& pattern : m_cache.recentPatterns) {
        float similarity = calculateSimilarity(input, pattern.input);
        if (similarity >= minConfidence) {
            results.push_back(pattern);
        }
    }

    // Se não encontrou suficiente no cache, busca no banco
    if (results.size() < 5) {
        QSqlQuery query(m_db);
        query.prepare(
            "SELECT input, action, confidence, params, success_count, last_used "
            "FROM command_patterns "
            "WHERE confidence >= :min_confidence "
            "ORDER BY success_count DESC, last_used DESC "
            "LIMIT 10"
        );
        query.bindValue(":min_confidence", minConfidence);

        if (query.exec()) {
            while (query.next()) {
                CommandPattern pattern;
                pattern.input = query.value("input").toString();
                pattern.action = query.value("action").toString();
                pattern.confidence = query.value("confidence").toFloat();
                pattern.params = nlohmann::json::parse(query.value("params").toString().toStdString());
                pattern.successCount = query.value("success_count").toInt();
                pattern.lastUsed = query.value("last_used").toDateTime();

                float similarity = calculateSimilarity(input, pattern.input);
                if (similarity >= minConfidence) {
                    results.push_back(pattern);
                }
            }
        }
    }

    // Ordena por similaridade
    std::sort(results.begin(), results.end(),
        [&input, this](const CommandPattern& a, const CommandPattern& b) {
            return calculateSimilarity(input, a.input) > calculateSimilarity(input, b.input);
        });

    return results;
}

float AIKnowledgeBase::calculateSimilarity(const QString& str1, const QString& str2) {
    // Implementação do algoritmo de Levenshtein normalizado
    const int len1 = str1.length();
    const int len2 = str2.length();

    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

    for (int i = 0; i <= len1; i++) dp[i][0] = i;
    for (int j = 0; j <= len2; j++) dp[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (str1[i-1] == str2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            }
        }
    }

    // Normaliza a distância para um valor entre 0 e 1
    float maxLen = std::max(len1, len2);
    if (maxLen == 0) return 1.0f;

    return 1.0f - (dp[len1][len2] / maxLen);
}

void AIKnowledgeBase::updatePatternSuccess(const QString& input, bool success) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE command_patterns
        SET success_count = success_count + ?,
            last_used = CURRENT_TIMESTAMP
        WHERE input = ?
    )");

    query.addBindValue(success ? 1 : -1);
    query.addBindValue(input);

    if (!query.exec()) {
        spdlog::error("Erro ao atualizar padrão: {}",
            query.lastError().text().toStdString());
        return;
    }

    // Atualizar cache
    for (auto& pattern : m_cache.recentPatterns) {
        if (pattern.input == input) {
            pattern.successCount += success ? 1 : -1;
            pattern.lastUsed = QDateTime::currentDateTime();
            break;
        }
    }
}

void AIKnowledgeBase::trainFromLogs(const QString& logPath) {
    QFile file(logPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        spdlog::error("Erro ao abrir arquivo de log: {}", logPath.toStdString());
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();

        // TODO: Implementar parser de logs mais robusto
        if (line.contains("comando:", Qt::CaseInsensitive)) {
            AdminAction action;
            action.command = line.mid(line.indexOf("comando:") + 8).trimmed();
            action.success = line.contains("sucesso", Qt::CaseInsensitive);
            action.timestamp = QDateTime::currentDateTime();
            action.adminName = "system";

            logAdminAction(action);

            if (action.success) {
                CommandPattern pattern;
                pattern.input = action.command;
                pattern.action = translateToAction(action.command);
                pattern.confidence = 1.0f;
                pattern.successCount = 1;
                pattern.lastUsed = action.timestamp;

                addCommandPattern(pattern);
            }
        }
    }
}

QString AIKnowledgeBase::translateToAction(const QString& command) {
    // TODO: Implementar tradução mais sofisticada
    return command.toLower().replace(" ", "_");
}

void AIKnowledgeBase::loadCache() {
    // Carregar padrões mais recentes
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT input, action, confidence, params, success_count, last_used
        FROM command_patterns
        ORDER BY last_used DESC
        LIMIT ?
    )");

    query.addBindValue(m_config.maxCacheSize);

    if (query.exec()) {
        while (query.next()) {
            CommandPattern pattern;
            pattern.input = query.value(0).toString();
            pattern.action = query.value(1).toString();
            pattern.confidence = query.value(2).toFloat();
            pattern.params = nlohmann::json::parse(
                query.value(3).toString().toStdString()
            );
            pattern.successCount = query.value(4).toInt();
            pattern.lastUsed = query.value(5).toDateTime();

            m_cache.recentPatterns.push_back(pattern);
        }
    }
}

void AIKnowledgeBase::cleanupOldData() {
    QDateTime cutoff = QDateTime::currentDateTime()
        .addDays(-m_config.maxPatternAge);

    QSqlQuery query(m_db);
    query.prepare(R"(
        DELETE FROM command_patterns
        WHERE last_used < ? AND success_count <= 0
    )");

    query.addBindValue(cutoff);

    if (!query.exec()) {
        spdlog::error("Erro ao limpar dados antigos: {}",
            query.lastError().text().toStdString());
    }
}

std::vector<AIKnowledgeBase::CommandToken> AIKnowledgeBase::tokenizeCommand(const QString& command) {
    std::vector<CommandToken> tokens;
    QStringList words = command.split(" ", Qt::SkipEmptyParts);

    for (const QString& word : words) {
        CommandToken token;
        token.value = word;

        // Análise básica do tipo de token
        if (word.startsWith("/")) {
            token.type = "command";
            token.confidence = 1.0f;
        } else if (word.contains(QRegExp("\\d+"))) {
            token.type = "parameter";
            token.confidence = 0.9f;
        } else {
            token.type = "text";
            token.confidence = 0.8f;
        }

        tokens.push_back(token);
    }

    return tokens;
}

float AIKnowledgeBase::predictSuccess(const QString& command, const CommandContext& context) {
    float prediction = 0.0f;

    // Análise de padrões similares
    auto patterns = findSimilarPatterns(command, m_config.minConfidence);
    if (patterns.empty()) return 0.5f; // Valor neutro se não houver padrões

    float totalWeight = 0.0f;
    float weightedSuccess = 0.0f;

    for (const auto& pattern : patterns) {
        float contextSimilarity = calculateContextSimilarity(context, CommandContext{});
        float weight = pattern.confidence * contextSimilarity;

        weightedSuccess += weight * (pattern.successCount > 0 ? 1.0f : 0.0f);
        totalWeight += weight;
    }

    if (totalWeight > 0) {
        prediction = weightedSuccess / totalWeight;
    }

    return prediction;
}

void AIKnowledgeBase::learnFromSuccess(const QString& command, const CommandContext& context) {
    // Atualiza padrões existentes
    auto patterns = findSimilarPatterns(command, m_config.minConfidence);
    bool patternExists = false;

    for (auto& pattern : patterns) {
        if (calculateSimilarity(command, pattern.input) > m_advancedConfig.mergeThreshold) {
            pattern.successCount++;
            pattern.lastUsed = QDateTime::currentDateTime();
            updatePatternStats(pattern, true);
            patternExists = true;
            break;
        }
    }

    // Cria novo padrão se necessário
    if (!patternExists) {
        CommandPattern newPattern;
        newPattern.input = command;
        newPattern.confidence = 1.0f;
        newPattern.successCount = 1;
        newPattern.lastUsed = QDateTime::currentDateTime();
        addCommandPattern(newPattern);
    }
}

void AIKnowledgeBase::updatePatternStats(const CommandPattern& pattern, bool success) {
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE command_patterns "
        "SET success_count = success_count + :success, "
        "last_used = :last_used "
        "WHERE input = :input"
    );

    query.bindValue(":success", success ? 1 : 0);
    query.bindValue(":last_used", QDateTime::currentDateTime());
    query.bindValue(":input", pattern.input);

    if (!query.exec()) {
        spdlog::error("Erro ao atualizar estatísticas: {}",
            query.lastError().text().toStdString());
    }
}

} // namespace ai
} // namespace wydbr
