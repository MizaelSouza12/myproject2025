#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <memory>
#include "AIAssistant.h"

namespace wydbr {
namespace ai {

class AIAssistantWidget : public QWidget {
    Q_OBJECT

public:
    explicit AIAssistantWidget(QWidget* parent = nullptr);
    ~AIAssistantWidget();

private slots:
    void onSendCommand();
    void onCommandProcessed(const QString& result);
    void onSuggestionAvailable(const QString& suggestion);
    void onAlertGenerated(const QString& alert, int severity);
    void onActionRequired(const QString& action, const QString& reason);

private:
    void setupUI();
    void appendMessage(const QString& message, const QString& style = "");
    QString getSeverityStyle(int severity);

    QLineEdit* m_commandInput;
    QTextEdit* m_chatOutput;
    QPushButton* m_sendButton;
    QPushButton* m_clearButton;

    std::unique_ptr<AIAssistant> m_assistant;
};

} // namespace ai
} // namespace wydbr
