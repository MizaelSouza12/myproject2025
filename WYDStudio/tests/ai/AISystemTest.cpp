#include <gtest/gtest.h>
#include "ai/AIKnowledgeBase.h"
#include "ai/CommandTokenizer.h"
#include "ai/ContextAnalyzer.h"
#include <QDateTime>

using namespace wydbr::ai;

class AISystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        knowledgeBase = std::make_unique<AIKnowledgeBase>();
        tokenizer = std::make_unique<CommandTokenizer>();
        analyzer = std::make_unique<ContextAnalyzer>();
    }

    std::unique_ptr<AIKnowledgeBase> knowledgeBase;
    std::unique_ptr<CommandTokenizer> tokenizer;
    std::unique_ptr<ContextAnalyzer> analyzer;
};

// Testes do Tokenizador
TEST_F(AISystemTest, BasicTokenization) {
    QString command = "/give @player item_name +3";
    auto tokens = tokenizer->tokenize(command);

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, "command");
    EXPECT_EQ(tokens[0].value, "give");
    EXPECT_EQ(tokens[1].type, "mention");
    EXPECT_EQ(tokens[2].type, "text");
    EXPECT_EQ(tokens[3].type, "enhancement");
}

TEST_F(AISystemTest, WYDSpecificTokenization) {
    QString command = "set arc+7 celestial";
    auto tokens = tokenizer->tokenize(command);

    ASSERT_GE(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, "item");
    EXPECT_TRUE(tokens[1].type == "enhancement" || tokens[1].type == "special_item");
}

TEST_F(AISystemTest, ParameterExtraction) {
    QString command = "/ban @player \"reason for ban\" --force";
    auto tokens = tokenizer->tokenize(command);
    auto params = tokenizer->extractParameters(tokens);

    EXPECT_EQ(params["command"], "ban");
    EXPECT_TRUE(params.contains("target"));
    EXPECT_TRUE(params["string_params"].size() > 0);
    EXPECT_TRUE(params["flags"].size() > 0);
}

// Testes do Analisador de Contexto
TEST_F(AISystemTest, TimeAnalysis) {
    QDateTime peak(QDate::currentDate(), QTime(15, 0)); // 15:00
    QDateTime offPeak(QDate::currentDate(), QTime(3, 0)); // 03:00

    float peakScore = analyzer->analyzeTimeOfDay(peak);
    float offPeakScore = analyzer->analyzeTimeOfDay(offPeak);

    EXPECT_GT(peakScore, offPeakScore);
    EXPECT_GE(peakScore, 0.8f);
    EXPECT_LE(offPeakScore, 0.7f);
}

TEST_F(AISystemTest, ServerMetricsAnalysis) {
    nlohmann::json goodMetrics = {
        {"cpu_usage", 50.0f},
        {"memory_usage", 60.0f},
        {"players_online", 500},
        {"latency", 50.0f}
    };

    nlohmann::json badMetrics = {
        {"cpu_usage", 90.0f},
        {"memory_usage", 95.0f},
        {"players_online", 2000},
        {"latency", 200.0f}
    };

    float goodScore = analyzer->analyzeServerMetrics(goodMetrics);
    float badScore = analyzer->analyzeServerMetrics(badMetrics);

    EXPECT_GT(goodScore, badScore);
    EXPECT_GE(goodScore, 0.8f);
    EXPECT_LE(badScore, 0.5f);
}

TEST_F(AISystemTest, UserRoleAnalysis) {
    float ownerScore = analyzer->analyzeUserRole("owner");
    float adminScore = analyzer->analyzeUserRole("admin");
    float modScore = analyzer->analyzeUserRole("moderator");
    float helperScore = analyzer->analyzeUserRole("helper");

    EXPECT_GT(ownerScore, adminScore);
    EXPECT_GT(adminScore, modScore);
    EXPECT_GT(modScore, helperScore);
}

// Testes da Base de Conhecimento
TEST_F(AISystemTest, PatternMatching) {
    AIKnowledgeBase::CommandPattern pattern;
    pattern.input = "/give @player item";
    pattern.action = "give_item";
    pattern.confidence = 1.0f;
    pattern.successCount = 10;
    pattern.lastUsed = QDateTime::currentDateTime();

    knowledgeBase->addCommandPattern(pattern);

    auto matches = knowledgeBase->findSimilarPatterns("/give @user item", 0.7f);
    EXPECT_FALSE(matches.empty());
    EXPECT_GE(matches[0].confidence, 0.7f);
}

TEST_F(AISystemTest, SuccessPrediction) {
    // Configurar contexto de teste
    AIKnowledgeBase::CommandContext context;
    context.serverState = "normal";
    context.previousCommand = "/help";
    context.userRole = "admin";
    context.timeOfDay = QDateTime::currentDateTime();
    context.metrics = {
        {"cpu_usage", 50.0f},
        {"memory_usage", 60.0f}
    };

    // Testar predição para comando seguro
    float safeScore = knowledgeBase->predictSuccess("/help", context);
    EXPECT_GE(safeScore, 0.7f);

    // Testar predição para comando perigoso
    context.previousCommand = "/delete_all";
    float dangerScore = knowledgeBase->predictSuccess("/ban_all", context);
    EXPECT_LE(dangerScore, 0.5f);
}

TEST_F(AISystemTest, LearningSystem) {
    AIKnowledgeBase::CommandContext context;
    context.serverState = "normal";
    context.userRole = "admin";
    context.timeOfDay = QDateTime::currentDateTime();

    QString command = "/give @player rare_item";

    // Registrar sucesso e verificar aprendizado
    knowledgeBase->learnFromSuccess(command, context);

    auto patterns = knowledgeBase->findSimilarPatterns(command, 0.8f);
    EXPECT_FALSE(patterns.empty());
    EXPECT_GT(patterns[0].successCount, 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
