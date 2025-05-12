#include "AIAssistantWidget.h"
#include <QScrollBar>
#include <QDateTime>

namespace wydbr {
namespace ai {

AIAssistantWidget::AIAssistantWidget(QWidget* parent)
    : QWidget(parent)
    , m_assistant(std::make_unique<AIAssistant>())
{
    setupUI();

    // Conectar sinais do assistente
    connect(m_assistant.get(), &AIAssistant::commandProcessed,
            this, &AIAssistantWidget::onCommandProcessed);
    connect(m_assistant.get(), &AIAssistant::suggestionAvailable,
            this, &AIAssistantWidget::onSuggestionAvailable);
    connect(m_assistant.get(), &AIAssistant::alertGenerated,
            this, &AIAssistantWidget::onAlertGenerated);
    connect(m_assistant.get(), &AIAssistant::actionRequired,
            this, &AIAssistantWidget::onActionRequired);

    // Iniciar monitoramento
    m_assistant->startMonitoring();

    // Mensagem de boas-vindas
    appendMessage(
        "Olá! Eu sou o assistente do WYDStudio. Como posso ajudar?\n"
        "Você pode me pedir para:\n"
        "- Banir jogadores suspeitos\n"
        "- Reiniciar o servidor\n"
        "- Verificar status do servidor\n"
        "- Gerenciar eventos\n"
        "E muito mais! Basta descrever o que precisa em linguagem natural.",
        "color: #4CAF50;"
    );
}

AIAssistantWidget::~AIAssistantWidget() {
    m_assistant->stopMonitoring();
}

void AIAssistantWidget::setupUI() {
    auto layout = new QVBoxLayout(this);

    // Área de chat
    m_chatOutput = new QTextEdit(this);
    m_chatOutput->setReadOnly(true);
    m_chatOutput->setStyleSheet(R"(
        QTextEdit {
            background-color: #1A1A1A;
            color: #D0D0D0;
            border: 1px solid #3A3A3A;
            border-radius: 4px;
            padding: 8px;
        }
    )");
    layout->addWidget(m_chatOutput);

    // Área de input
    auto inputLayout = new QHBoxLayout();

    m_commandInput = new QLineEdit(this);
    m_commandInput->setPlaceholderText("Digite seu comando aqui...");
    m_commandInput->setStyleSheet(R"(
        QLineEdit {
            background-color: #2A2A2A;
            color: #D0D0D0;
            border: 1px solid #3A3A3A;
            border-radius: 4px;
            padding: 8px;
        }
        QLineEdit:focus {
            border: 1px solid #4A4A4A;
        }
    )");
    connect(m_commandInput, &QLineEdit::returnPressed,
            this, &AIAssistantWidget::onSendCommand);
    inputLayout->addWidget(m_commandInput);

    m_sendButton = new QPushButton("Enviar", this);
    m_sendButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
    )");
    connect(m_sendButton, &QPushButton::clicked,
            this, &AIAssistantWidget::onSendCommand);
    inputLayout->addWidget(m_sendButton);

    m_clearButton = new QPushButton("Limpar", this);
    m_clearButton->setStyleSheet(R"(
        QPushButton {
            background-color: #f44336;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #da190b;
        }
        QPushButton:pressed {
            background-color: #d32f2f;
        }
    )");
    connect(m_clearButton, &QPushButton::clicked,
            m_chatOutput, &QTextEdit::clear);
    inputLayout->addWidget(m_clearButton);

    layout->addLayout(inputLayout);
}

void AIAssistantWidget::onSendCommand() {
    QString command = m_commandInput->text().trimmed();
    if (command.isEmpty()) return;

    // Adicionar comando ao chat
    appendMessage(
        QString("[%1] Você: %2")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
            .arg(command),
        "color: #42A5F5;"
    );

    // Limpar input
    m_commandInput->clear();

    // Processar comando
    m_assistant->processCommand(command);
}

void AIAssistantWidget::onCommandProcessed(const QString& result) {
    appendMessage(
        QString("[%1] Assistente: %2")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
            .arg(result),
        "color: #4CAF50;"
    );
}

void AIAssistantWidget::onSuggestionAvailable(const QString& suggestion) {
    appendMessage(
        QString("[%1] Sugestão: %2")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
            .arg(suggestion),
        "color: #FFA726;"
    );
}

void AIAssistantWidget::onAlertGenerated(const QString& alert, int severity) {
    appendMessage(
        QString("[%1] Alerta: %2")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
            .arg(alert),
        getSeverityStyle(severity)
    );
}

void AIAssistantWidget::onActionRequired(const QString& action, const QString& reason) {
    appendMessage(
        QString("[%1] Ação Requerida: %2\nMotivo: %3")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
            .arg(action)
            .arg(reason),
        "color: #AB47BC;"
    );
}

void AIAssistantWidget::appendMessage(const QString& message, const QString& style) {
    // Adicionar mensagem ao chat
    m_chatOutput->append(QString("<div style='%1'>%2</div>")
        .arg(style)
        .arg(message.toHtmlEscaped().replace("\n", "<br>"))
    );

    // Rolar para o final
    auto scrollbar = m_chatOutput->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

QString AIAssistantWidget::getSeverityStyle(int severity) {
    switch (severity) {
        case 1: // Baixa
            return "color: #FFA726;"; // Laranja
        case 2: // Média
            return "color: #FF7043;"; // Laranja escuro
        case 3: // Alta
            return "color: #f44336;"; // Vermelho
        default:
            return "color: #D0D0D0;"; // Padrão
    }
}

} // namespace ai
} // namespace wydbr
