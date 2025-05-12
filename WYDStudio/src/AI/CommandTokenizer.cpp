#include "CommandTokenizer.h"
#include <QRegExp>
#include <QStringList>
#include <algorithm>

namespace wydbr {
namespace ai {

CommandTokenizer::CommandTokenizer() {
    // Inicialização dos padrões de tokenização
    initializePatterns();
}

void CommandTokenizer::initializePatterns() {
    // Padrões de comandos administrativos
    m_commandPatterns = {
        {R"(/(\w+))", "command"},
        {R"(--(\w+))", "flag"},
        {R"(-(\w))", "shortflag"},
        {R"((\d+))", "number"},
        {R"("([^"]+)")", "quoted_string"},
        {R"('([^']+)')", "quoted_string"},
        {R"(\[([^\]]+)\])", "option"},
        {R"(\{([^}]+)\})", "block"},
        {R"(@(\w+))", "mention"},
        {R"(#(\w+))", "channel"}
    };

    // Padrões específicos do WYD
    m_wydPatterns = {
        // Items e equipamentos
        {R"((set|item|equip)\s*(\w+))", "item"},
        {R"(\+(\d+))", "enhancement"},
        {R"((anc|arc|cel|div)\s*(\w+))", "special_item"},

        // Ações de administração
        {R"((ban|kick|mute)\s*(\w+))", "admin_action"},
        {R"((give|take)\s*(\w+))", "item_action"},
        {R"((tp|teleport|warp)\s*(\w+))", "movement"},

        // Eventos e configurações
        {R"((event|drop|rate)\s*(\w+))", "event"},
        {R"((config|set)\s*(\w+))", "setting"},
        {R"((start|stop|pause)\s*(\w+))", "control"},

        // Alvos e parâmetros
        {R"(to\s*(\w+))", "target"},
        {R"(for\s*(\d+))", "duration"},
        {R"(at\s*(\d+))", "location"}
    };
}

std::vector<CommandToken> CommandTokenizer::tokenize(const QString& command) {
    std::vector<CommandToken> tokens;
    QString remaining = command;

    // Primeira passada: tokens básicos
    processBasicTokens(remaining, tokens);

    // Segunda passada: padrões específicos do WYD
    processWYDPatterns(remaining, tokens);

    // Terceira passada: análise de contexto
    enhanceTokens(tokens);

    return tokens;
}

void CommandTokenizer::processBasicTokens(QString& text, std::vector<CommandToken>& tokens) {
    for (const auto& pattern : m_commandPatterns) {
        QRegExp rx(pattern.first);
        int pos = 0;

        while ((pos = rx.indexIn(text, pos)) != -1) {
            CommandToken token;
            token.value = rx.cap(1);
            token.type = pattern.second;
            token.confidence = 1.0f;
            token.position = pos;

            // Metadados específicos
            token.meta = {
                {"raw_match", rx.cap(0).toStdString()},
                {"length", rx.matchedLength()},
                {"context", "basic"}
            };

            tokens.push_back(token);

            // Remove o token do texto
            text.replace(pos, rx.matchedLength(), QString(rx.matchedLength(), ' '));
            pos += rx.matchedLength();
        }
    }
}

void CommandTokenizer::processWYDPatterns(QString& text, std::vector<CommandToken>& tokens) {
    for (const auto& pattern : m_wydPatterns) {
        QRegExp rx(pattern.first, Qt::CaseInsensitive);
        int pos = 0;

        while ((pos = rx.indexIn(text, pos)) != -1) {
            CommandToken token;
            token.value = rx.cap(0);
            token.type = pattern.second;
            token.confidence = 0.9f;
            token.position = pos;

            // Metadados específicos do WYD
            token.meta = {
                {"command_group", pattern.second},
                {"sub_value", rx.cap(2).toStdString()},
                {"context", "wyd"}
            };

            tokens.push_back(token);

            // Remove o token do texto
            text.replace(pos, rx.matchedLength(), QString(rx.matchedLength(), ' '));
            pos += rx.matchedLength();
        }
    }

    // Processa palavras restantes como texto
    processRemainingText(text, tokens);
}

void CommandTokenizer::processRemainingText(const QString& text, std::vector<CommandToken>& tokens) {
    QStringList words = text.split(QRegExp("\\s+"), Qt::SkipEmptyParts);

    for (const QString& word : words) {
        if (word.trimmed().isEmpty()) continue;

        CommandToken token;
        token.value = word;
        token.type = "text";
        token.confidence = 0.5f;
        token.position = text.indexOf(word);

        token.meta = {
            {"context", "unknown"},
            {"length", word.length()}
        };

        tokens.push_back(token);
    }
}

void CommandTokenizer::enhanceTokens(std::vector<CommandToken>& tokens) {
    if (tokens.empty()) return;

    // Análise de sequência
    for (size_t i = 1; i < tokens.size(); i++) {
        auto& current = tokens[i];
        const auto& previous = tokens[i-1];

        // Melhora a confiança baseado no contexto
        if (previous.type == "command" && current.type == "text") {
            current.confidence += 0.2f;
            current.meta["context"] = "command_param";
        }

        if (previous.type == "item" && current.type == "number") {
            current.type = "item_quantity";
            current.confidence += 0.3f;
        }

        if (previous.type == "admin_action" && current.type == "text") {
            current.type = "target_player";
            current.confidence += 0.3f;
        }
    }

    // Normaliza as confianças
    for (auto& token : tokens) {
        token.confidence = std::min(1.0f, token.confidence);
    }
}

nlohmann::json CommandTokenizer::extractParameters(const std::vector<CommandToken>& tokens) {
    nlohmann::json params;

    if (tokens.empty()) return params;

    // Extrai o comando principal
    if (tokens[0].type == "command") {
        params["command"] = tokens[0].value.toStdString();
    }

    // Processa parâmetros
    for (size_t i = 1; i < tokens.size(); i++) {
        const auto& token = tokens[i];

        if (token.type == "flag" || token.type == "shortflag") {
            params["flags"].push_back(token.value.toStdString());
        }
        else if (token.type == "number") {
            params["numeric_params"].push_back(token.value.toInt());
        }
        else if (token.type == "quoted_string") {
            params["string_params"].push_back(token.value.toStdString());
        }
        else if (token.type == "target_player") {
            params["target"] = token.value.toStdString();
        }
        else if (token.type == "item" || token.type == "special_item") {
            params["items"].push_back({
                {"type", token.type.toStdString()},
                {"value", token.value.toStdString()},
                {"meta", token.meta}
            });
        }
    }

    return params;
}

} // namespace ai
} // namespace wydbr
