#ifndef WYDSTUDIO_FILE_BROWSER_BUTTON_H
#define WYDSTUDIO_FILE_BROWSER_BUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <functional>
#include <string>
#include <memory>
#include "FileBrowser.h"
#include "../../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {
namespace common {

/**
 * @brief Configuração do botão do navegador de arquivos
 */
struct FileBrowserButtonConfig {
    QString initialDirectory;
    QString dialogTitle;
    QStringList fileFilters;
    QSet<WYDFileType> acceptedTypes;
    QString buttonText;
    QString placeholderText;
    bool showTextField;
    bool showFileInfo;
    bool allowDirectorySelection;
    bool allowMultipleSelection;
    bool showHiddenFiles;
    bool clearTextOnCancel;
    bool openInEditor;
    std::string relatedEditor;
    FileBrowserViewMode defaultViewMode;
    std::function<bool(const std::string&)> validationCallback;
};

/**
 * @brief Botão para abrir o navegador de arquivos
 * 
 * Este widget combina um campo de texto e um botão para selecionar
 * arquivos usando o navegador de arquivos personalizado do WYDStudio.
 */
class FileBrowserButton : public QWidget {
    Q_OBJECT
    
public:
    /**
     * @brief Construtor
     * @param parent Widget pai
     */
    FileBrowserButton(QWidget* parent = nullptr);
    
    /**
     * @brief Destrutor
     */
    ~FileBrowserButton();
    
    /**
     * @brief Inicializa o botão do navegador
     * @param facade Referência ao facade do WYDStudio
     * @param config Configuração do botão
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::shared_ptr<core::WYDStudioFacade>& facade, 
                   const FileBrowserButtonConfig& config);
    
    /**
     * @brief Define o caminho do arquivo
     * @param filePath Caminho do arquivo
     */
    void setFilePath(const QString& filePath);
    
    /**
     * @brief Obtém o caminho do arquivo
     * @return Caminho do arquivo
     */
    QString getFilePath() const;
    
    /**
     * @brief Define o texto do botão
     * @param text Texto do botão
     */
    void setButtonText(const QString& text);
    
    /**
     * @brief Define o texto do placeholder
     * @param text Texto do placeholder
     */
    void setPlaceholderText(const QString& text);
    
    /**
     * @brief Define se o campo de texto deve ser exibido
     * @param show Se verdadeiro, exibe o campo de texto
     */
    void setShowTextField(bool show);
    
    /**
     * @brief Define se as informações do arquivo devem ser exibidas
     * @param show Se verdadeiro, exibe as informações do arquivo
     */
    void setShowFileInfo(bool show);
    
    /**
     * @brief Define o diretório inicial
     * @param dirPath Caminho do diretório
     */
    void setInitialDirectory(const QString& dirPath);
    
    /**
     * @brief Define os filtros de arquivo
     * @param filters Lista de filtros
     */
    void setFileFilters(const QStringList& filters);
    
    /**
     * @brief Define os tipos aceitos
     * @param types Conjunto de tipos aceitos
     */
    void setAcceptedTypes(const QSet<WYDFileType>& types);
    
    /**
     * @brief Define o modo de visualização padrão
     * @param mode Modo de visualização
     */
    void setDefaultViewMode(FileBrowserViewMode mode);
    
    /**
     * @brief Define se a seleção de diretórios é permitida
     * @param allow Se verdadeiro, permite seleção de diretórios
     */
    void setAllowDirectorySelection(bool allow);
    
    /**
     * @brief Define se a seleção múltipla é permitida
     * @param allow Se verdadeiro, permite seleção múltipla
     */
    void setAllowMultipleSelection(bool allow);
    
    /**
     * @brief Define se o campo deve ser limpo ao cancelar
     * @param clear Se verdadeiro, limpa o campo ao cancelar
     */
    void setClearTextOnCancel(bool clear);
    
    /**
     * @brief Define o callback de validação
     * @param callback Função de validação
     */
    void setValidationCallback(std::function<bool(const std::string&)> callback);
    
    /**
     * @brief Limpa o campo de texto
     */
    void clear();
    
    /**
     * @brief Habilita ou desabilita o botão
     * @param enable Se verdadeiro, habilita o botão
     */
    void setEnabled(bool enable);
    
signals:
    /**
     * @brief Sinal emitido quando um arquivo é selecionado
     * @param filePath Caminho do arquivo
     */
    void fileSelected(const QString& filePath);
    
    /**
     * @brief Sinal emitido quando o caminho do arquivo é alterado
     * @param filePath Caminho do arquivo
     */
    void filePathChanged(const QString& filePath);
    
    /**
     * @brief Sinal emitido quando vários arquivos são selecionados
     * @param filePaths Lista de caminhos de arquivos
     */
    void filesSelected(const QStringList& filePaths);
    
    /**
     * @brief Sinal emitido quando um arquivo é decompilado
     * @param originalPath Caminho do arquivo original
     * @param decompiledPath Caminho do arquivo decompilado
     */
    void fileDecompiled(const QString& originalPath, const QString& decompiledPath);
    
    /**
     * @brief Sinal emitido quando um arquivo é compilado
     * @param originalPath Caminho do arquivo original
     * @param compiledPath Caminho do arquivo compilado
     */
    void fileCompiled(const QString& originalPath, const QString& compiledPath);
    
private slots:
    /**
     * @brief Slot chamado quando o botão de navegação é clicado
     */
    void onBrowseButtonClicked();
    
    /**
     * @brief Slot chamado quando o campo de texto é editado
     * @param text Novo texto
     */
    void onTextChanged(const QString& text);
    
    /**
     * @brief Slot chamado quando o botão de limpar é clicado
     */
    void onClearButtonClicked();
    
    /**
     * @brief Slot chamado quando um arquivo é selecionado no navegador
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo
     */
    void onFileBrowserFileSelected(const std::string& filePath, WYDFileType fileType);
    
    /**
     * @brief Slot chamado quando o botão de decompilação é clicado
     */
    void onDecompileButtonClicked();
    
    /**
     * @brief Slot chamado quando o botão de compilação é clicado
     */
    void onCompileButtonClicked();
    
    /**
     * @brief Slot chamado quando o botão de edição é clicado
     */
    void onEditButtonClicked();
    
private:
    // Interface do usuário
    QHBoxLayout* m_mainLayout;
    QLineEdit* m_pathLineEdit;
    QPushButton* m_browseButton;
    QToolButton* m_clearButton;
    QToolButton* m_decompileButton;
    QToolButton* m_compileButton;
    QToolButton* m_editButton;
    QLabel* m_fileInfoLabel;
    
    // Referência ao facade
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Configuração
    FileBrowserButtonConfig m_config;
    
    // Navegador de arquivos
    FileBrowser* m_fileBrowser;
    
    // Estado atual
    QString m_currentFilePath;
    bool m_initialized;
    
    // Inicialização da UI
    void createUI();
    
    // Atualização de UI
    void updateFileInfo();
    void updateButtons();
    
    // Validação de caminho
    bool validatePath(const QString& path);
    
    // Utilitários
    QString getFileTypeString(const QString& filePath) const;
    WYDFileType detectFileType(const QString& filePath) const;
    bool isFileCompiled(const QString& filePath) const;
    bool isFileDecompiled(const QString& filePath) const;
    
    // Operações com arquivos
    bool decompileFile(const QString& filePath);
    bool compileFile(const QString& filePath);
    void openInEditor(const QString& filePath);
};

}}} // namespace wydstudio::ui::common

#endif // WYDSTUDIO_FILE_BROWSER_BUTTON_H