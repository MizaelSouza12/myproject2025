#ifndef WYDSTUDIO_AI_ASSISTANT_WIDGET_H
#define WYDSTUDIO_AI_ASSISTANT_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QListWidget>
#include <QSplitter>
#include <QComboBox>
#include <QCheckBox>
#include <QToolButton>
#include <QMenu>
#include <QTimer>
#include <QThread>
#include <QPixmap>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include "../../../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {
namespace admin {
namespace widgets {

/**
 * @brief Configuração do assistente de IA
 */
struct AIAssistantConfig {
    std::string modelName;
    bool enableSuggestions;
    bool enableAutoCommands;
    bool enableContinuousConversation;
    int suggestionDelay;
    int maxHistory;
    float temperature;
    std::vector<std::string> presetPrompts;
    std::map<std::string, std::string> systemInstructions;
    std::string defaultPersonality;
    bool keepContext;
    std::vector<std::string> favoriteCommands;
    std::map<std::string, std::string> customSettings;
};

/**
 * @brief Tipo de mensagem no chat
 */
enum class MessageType {
    USER,           // Mensagem do usuário
    ASSISTANT,      // Resposta do assistente
    SYSTEM,         // Mensagem do sistema
    ERROR,          // Mensagem de erro
    ACTION,         // Ação executada
    SUGGESTION      // Sugestão do assistente
};

/**
 * @brief Mensagem no histórico de chat
 */
struct ChatMessage {
    std::string content;
    MessageType type;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> suggestedActions;
    std::string metadata;
};

/**
 * @brief Widget para interagir com o assistente de IA
 */
class AIAssistantWidget : public QWidget {
    Q_OBJECT
    
public:
    /**
     * @brief Construtor
     * @param parent Widget pai
     */
    AIAssistantWidget(QWidget* parent = nullptr);
    
    /**
     * @brief Destrutor
     */
    ~AIAssistantWidget();
    
    /**
     * @brief Inicializa o assistente
     * @param facade Referência ao facade do WYDStudio
     * @param config Configuração do assistente
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::shared_ptr<core::WYDStudioFacade>& facade, 
                   const AIAssistantConfig& config);
    
    /**
     * @brief Envia uma mensagem para o assistente
     * @param message Mensagem a ser enviada
     */
    void sendMessage(const std::string& message);
    
    /**
     * @brief Processa um comando
     * @param command Comando a ser processado
     * @return Resultado do processamento
     */
    std::string processCommand(const std::string& command);
    
    /**
     * @brief Adiciona uma mensagem do sistema ao chat
     * @param message Mensagem a ser adicionada
     */
    void addSystemMessage(const std::string& message);
    
    /**
     * @brief Adiciona uma mensagem de erro ao chat
     * @param message Mensagem a ser adicionada
     */
    void addErrorMessage(const std::string& message);
    
    /**
     * @brief Adiciona uma mensagem de ação ao chat
     * @param action Descrição da ação
     * @param result Resultado da ação
     * @param success Se verdadeiro, a ação foi bem-sucedida
     */
    void addActionMessage(const std::string& action, const std::string& result, bool success);
    
    /**
     * @brief Limpa o histórico de chat
     */
    void clearChat();
    
    /**
     * @brief Define a personalidade do assistente
     * @param personality Nome da personalidade
     */
    void setPersonality(const std::string& personality);
    
    /**
     * @brief Obtém a personalidade atual
     * @return Nome da personalidade
     */
    std::string getPersonality() const;
    
    /**
     * @brief Define se o contexto deve ser mantido entre mensagens
     * @param keep Se verdadeiro, mantém o contexto
     */
    void setKeepContext(bool keep);
    
    /**
     * @brief Verifica se o assistente está mantendo contexto
     * @return true se estiver mantendo contexto
     */
    bool isKeepingContext() const;
    
    /**
     * @brief Define se sugestões devem ser exibidas
     * @param enable Se verdadeiro, habilita sugestões
     */
    void setEnableSuggestions(bool enable);
    
    /**
     * @brief Define se comandos automáticos devem ser executados
     * @param enable Se verdadeiro, habilita comandos automáticos
     */
    void setEnableAutoCommands(bool enable);
    
    /**
     * @brief Adiciona um comando favorito
     * @param command Comando a ser adicionado
     */
    void addFavoriteCommand(const std::string& command);
    
    /**
     * @brief Remove um comando favorito
     * @param command Comando a ser removido
     */
    void removeFavoriteCommand(const std::string& command);
    
    /**
     * @brief Verifica se o assistente está processando
     * @return true se estiver processando
     */
    bool isProcessing() const;
    
    /**
     * @brief Cancela o processamento atual
     */
    void cancelProcessing();
    
    /**
     * @brief Obtém o histórico de mensagens
     * @return Vetor com o histórico de mensagens
     */
    std::vector<ChatMessage> getChatHistory() const;
    
    /**
     * @brief Exporta o histórico de chat para um arquivo
     * @param filePath Caminho do arquivo
     * @return true se exportado com sucesso
     */
    bool exportChatHistory(const std::string& filePath) const;
    
public slots:
    /**
     * @brief Slot para processar a entrada do usuário
     */
    void onSendButtonClicked();
    
    /**
     * @brief Slot para executar uma ação sugerida
     * @param action Ação a ser executada
     */
    void onSuggestedActionClicked(const QString& action);
    
    /**
     * @brief Slot para selecionar um prompt predefinido
     * @param index Índice do prompt
     */
    void onPresetPromptSelected(int index);
    
    /**
     * @brief Slot para selecionar um comando favorito
     * @param index Índice do comando
     */
    void onFavoriteCommandSelected(int index);
    
    /**
     * @brief Slot para alterar a personalidade
     * @param index Índice da personalidade
     */
    void onPersonalityChanged(int index);
    
    /**
     * @brief Slot para alternar o modo de manter contexto
     * @param keep Se verdadeiro, mantém o contexto
     */
    void onKeepContextToggled(bool keep);
    
    /**
     * @brief Slot para limpar o histórico
     */
    void onClearHistoryClicked();
    
    /**
     * @brief Slot para exportar o histórico
     */
    void onExportHistoryClicked();
    
    /**
     * @brief Slot para copiar a última mensagem
     */
    void onCopyLastMessageClicked();
    
    /**
     * @brief Slot para cancelar o processamento
     */
    void onCancelProcessingClicked();
    
signals:
    /**
     * @brief Sinal emitido quando uma resposta é recebida
     * @param message Mensagem original
     * @param response Resposta do assistente
     * @param suggestedActions Ações sugeridas
     */
    void responseReceived(const std::string& message, const std::string& response, 
                         const std::vector<std::string>& suggestedActions);
    
    /**
     * @brief Sinal emitido quando uma ação é sugerida
     * @param action Ação sugerida
     */
    void actionSuggested(const std::string& action);
    
    /**
     * @brief Sinal emitido quando um comando é extraído
     * @param command Comando extraído
     * @param params Parâmetros do comando
     */
    void commandExtracted(const std::string& command, 
                         const std::map<std::string, std::string>& params);
    
    /**
     * @brief Sinal emitido quando o processamento é iniciado
     */
    void processingStarted();
    
    /**
     * @brief Sinal emitido quando o processamento é concluído
     */
    void processingFinished();
    
private:
    // Interface do usuário
    QVBoxLayout* m_mainLayout;
    QSplitter* m_mainSplitter;
    
    // Área de chat
    QScrollArea* m_chatScrollArea;
    QWidget* m_chatWidget;
    QVBoxLayout* m_chatLayout;
    
    // Entrada de usuário
    QHBoxLayout* m_inputLayout;
    QLineEdit* m_inputLineEdit;
    QPushButton* m_sendButton;
    QToolButton* m_optionsButton;
    QMenu* m_optionsMenu;
    
    // Ações sugeridas
    QListWidget* m_suggestionsListWidget;
    
    // Barra lateral
    QWidget* m_sidebarWidget;
    QVBoxLayout* m_sidebarLayout;
    QComboBox* m_personalityComboBox;
    QComboBox* m_presetPromptsComboBox;
    QListWidget* m_favoriteCommandsListWidget;
    QCheckBox* m_keepContextCheckBox;
    QPushButton* m_clearHistoryButton;
    QPushButton* m_exportHistoryButton;
    
    // Status do processamento
    QLabel* m_statusLabel;
    QPushButton* m_cancelButton;
    
    // Referência ao facade
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Configuração
    AIAssistantConfig m_config;
    
    // Estado atual
    std::vector<ChatMessage> m_chatHistory;
    std::string m_currentPersonality;
    bool m_keepContext;
    bool m_isProcessing;
    
    // Mutex para thread-safety
    mutable std::mutex m_historyMutex;
    
    // Thread de processamento
    QThread* m_processingThread;
    
    // Timer para sugestões
    QTimer* m_suggestionTimer;
    
    // Inicialização da UI
    void createUI();
    void setupOptions();
    void connectSignals();
    
    // Atualização de UI
    void updateChatDisplay();
    void scrollToBottom();
    void updateSuggestions(const std::vector<std::string>& suggestions);
    void updateStatusDisplay();
    
    // Processamento de mensagens
    void processMessageInBackground(const std::string& message);
    std::string processMessageSync(const std::string& message);
    
    // Geração de widget de mensagem
    QWidget* createMessageWidget(const ChatMessage& message);
    QWidget* createUserMessageWidget(const ChatMessage& message);
    QWidget* createAssistantMessageWidget(const ChatMessage& message);
    QWidget* createSystemMessageWidget(const ChatMessage& message);
    QWidget* createErrorMessageWidget(const ChatMessage& message);
    QWidget* createActionMessageWidget(const ChatMessage& message);
    QWidget* createSuggestionMessageWidget(const ChatMessage& message);
    
    // Manipulação de histórico
    void addMessageToHistory(const ChatMessage& message);
    void trimHistory();
    
    // Utilitários
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::vector<std::string> extractSuggestedActions(const std::string& response);
    bool extractCommand(const std::string& response, std::string& command, 
                       std::map<std::string, std::string>& params);
    std::string sanitizeMessage(const std::string& message) const;
    
    // Conversão para interfaces gráficas
    QString toQString(const std::string& str) const;
    std::string fromQString(const QString& str) const;
};

}}}} // namespace wydstudio::ui::admin::widgets

#endif // WYDSTUDIO_AI_ASSISTANT_WIDGET_H