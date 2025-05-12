#pragma once

#include <QString>
#include <QMap>
#include <vector>
#include <utility>
#include <nlohmann/json.hpp>

namespace wydbr {
namespace ai {

// Estrutura para representar um token de comando
struct CommandToken {
    QString value;           // Valor do token
    QString type;           // Tipo do token (comando, parâmetro, etc)
    float confidence;       // Confiança na classificação
    int position;          // Posição no comando original
    nlohmann::json meta;    // Metadados adicionais
};

class CommandTokenizer {
public:
    CommandTokenizer();
    ~CommandTokenizer() = default;

    // Tokenização principal
    std::vector<CommandToken> tokenize(const QString& command);

    // Extração de parâmetros
    nlohmann::json extractParameters(const std::vector<CommandToken>& tokens);

private:
    // Inicialização
    void initializePatterns();

    // Processamento de tokens
    void processBasicTokens(QString& text, std::vector<CommandToken>& tokens);
    void processWYDPatterns(QString& text, std::vector<CommandToken>& tokens);
    void processRemainingText(const QString& text, std::vector<CommandToken>& tokens);
    void enhanceTokens(std::vector<CommandToken>& tokens);

    // Padrões de reconhecimento
    using PatternPair = std::pair<QString, QString>;
    std::vector<PatternPair> m_commandPatterns;
    std::vector<PatternPair> m_wydPatterns;
};

} // namespace ai
} // namespace wydbr
