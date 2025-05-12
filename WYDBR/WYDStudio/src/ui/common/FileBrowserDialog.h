#ifndef WYDSTUDIO_FILE_BROWSER_DIALOG_H
#define WYDSTUDIO_FILE_BROWSER_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFileInfo>
#include <QMessageBox>
#include <string>
#include <memory>
#include <vector>
#include "FileBrowser.h"
#include "../../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {
namespace common {

/**
 * @brief Configuração do diálogo de navegação de arquivos
 */
struct FileBrowserDialogConfig {
    QString title;
    QString initialDirectory;
    QStringList fileFilters;
    QSet<WYDFileType> acceptedTypes;
    bool allowDirectorySelection;
    bool allowMultipleSelection;
    bool showHiddenFiles;
    bool showFilePreview;
    FileBrowserViewMode defaultViewMode;
    std::function<bool(const std::string&)> validationCallback;
    bool showDecompileButton;
    bool showCompileButton;
    bool showOpenButton;
    std::vector<std::string> recentFiles;
    std::string relatedEditor;
};

/**
 * @brief Diálogo de navegação de arquivos personalizado
 */
class FileBrowserDialog : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Construtor
     * @param parent Widget pai
     */
    FileBrowserDialog(QWidget* parent = nullptr);
    
    /**
     * @brief Destrutor
     */
    ~FileBrowserDialog();
    
    /**
     * @brief Inicializa o diálogo
     * @param facade Referência ao facade do WYDStudio
     * @param config Configuração do diálogo
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::shared_ptr<core::WYDStudioFacade>& facade, 
                   const FileBrowserDialogConfig& config);
    
    /**
     * @brief Obtém o arquivo selecionado
     * @return Caminho do arquivo selecionado
     */
    QString getSelectedFile() const;
    
    /**
     * @brief Obtém os arquivos selecionados
     * @return Lista de caminhos de arquivos selecionados
     */
    QStringList getSelectedFiles() const;
    
    /**
     * @brief Exibe o diálogo
     * @return Código de resultado do diálogo
     */
    int exec() override;
    
    /**
     * @brief Define um caminho de arquivo inicial
     * @param filePath Caminho do arquivo
     */
    void setSelectedFile(const QString& filePath);
    
    /**
     * @brief Define uma lista de arquivos inicialmente selecionados
     * @param filePaths Lista de caminhos de arquivos
     */
    void setSelectedFiles(const QStringList& filePaths);
    
    /**
     * @brief Define o diretório inicial
     * @param directory Caminho do diretório
     */
    void setDirectory(const QString& directory);
    
    /**
     * @brief Obtém o navegador de arquivos interno
     * @return Referência ao navegador de arquivos
     */
    FileBrowser* getFileBrowser();
    
    /**
     * @brief Adiciona um arquivo à lista de arquivos recentes
     * @param filePath Caminho do arquivo
     */
    void addRecentFile(const QString& filePath);
    
    /**
     * @brief Obtém a lista de arquivos recentes
     * @return Lista de caminhos de arquivos recentes
     */
    QStringList getRecentFiles() const;
    
    /**
     * @brief Limpa a lista de arquivos recentes
     */
    void clearRecentFiles();
    
    /**
     * @brief Cria e exibe um diálogo de seleção de arquivo
     * @param parent Widget pai
     * @param facade Referência ao facade do WYDStudio
     * @param title Título do diálogo
     * @param initialDirectory Diretório inicial
     * @param fileFilters Filtros de arquivo
     * @param acceptedTypes Tipos de arquivo aceitos
     * @param allowDirSelection Se permitir seleção de diretórios
     * @return Caminho do arquivo selecionado ou string vazia se cancelado
     */
    static QString getOpenFileName(QWidget* parent, 
                               const std::shared_ptr<core::WYDStudioFacade>& facade,
                               const QString& title,
                               const QString& initialDirectory,
                               const QStringList& fileFilters,
                               const QSet<WYDFileType>& acceptedTypes = {},
                               bool allowDirSelection = false);
    
    /**
     * @brief Cria e exibe um diálogo de seleção de arquivos múltiplos
     * @param parent Widget pai
     * @param facade Referência ao facade do WYDStudio
     * @param title Título do diálogo
     * @param initialDirectory Diretório inicial
     * @param fileFilters Filtros de arquivo
     * @param acceptedTypes Tipos de arquivo aceitos
     * @return Lista de caminhos de arquivos selecionados ou lista vazia se cancelado
     */
    static QStringList getOpenFileNames(QWidget* parent, 
                                   const std::shared_ptr<core::WYDStudioFacade>& facade,
                                   const QString& title,
                                   const QString& initialDirectory,
                                   const QStringList& fileFilters,
                                   const QSet<WYDFileType>& acceptedTypes = {});
    
    /**
     * @brief Cria e exibe um diálogo de seleção de diretório
     * @param parent Widget pai
     * @param facade Referência ao facade do WYDStudio
     * @param title Título do diálogo
     * @param initialDirectory Diretório inicial
     * @return Caminho do diretório selecionado ou string vazia se cancelado
     */
    static QString getExistingDirectory(QWidget* parent, 
                                   const std::shared_ptr<core::WYDStudioFacade>& facade,
                                   const QString& title,
                                   const QString& initialDirectory);
    
private slots:
    /**
     * @brief Slot chamado quando um arquivo é selecionado no navegador
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo
     */
    void onFileSelected(const std::string& filePath, WYDFileType fileType);
    
    /**
     * @brief Slot chamado quando um arquivo é aberto no navegador
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo
     */
    void onFileOpened(const std::string& filePath, WYDFileType fileType);
    
    /**
     * @brief Slot chamado quando o botão de decompilação é clicado
     */
    void onDecompileClicked();
    
    /**
     * @brief Slot chamado quando o botão de compilação é clicado
     */
    void onCompileClicked();
    
    /**
     * @brief Slot chamado quando o botão de abrir no editor é clicado
     */
    void onOpenInEditorClicked();
    
    /**
     * @brief Slot chamado quando um arquivo recente é selecionado
     */
    void onRecentFileSelected();
    
    /**
     * @brief Slot chamado quando é solicitado limpar arquivos recentes
     */
    void onClearRecentFiles();
    
    /**
     * @brief Slot chamado quando é solicitado criar um novo diretório
     */
    void onCreateDirectory();
    
private:
    // Interface do usuário
    QVBoxLayout* m_mainLayout;
    FileBrowser* m_fileBrowser;
    QDialogButtonBox* m_buttonBox;
    QPushButton* m_selectButton;
    QPushButton* m_cancelButton;
    QPushButton* m_decompileButton;
    QPushButton* m_compileButton;
    QPushButton* m_openButton;
    QPushButton* m_recentButton;
    QMenu* m_recentFilesMenu;
    
    // Referência ao facade
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Configuração
    FileBrowserDialogConfig m_config;
    
    // Estado atual
    QStringList m_selectedFiles;
    QStringList m_recentFiles;
    
    // Inicialização da UI
    void createUI();
    
    // Atualização de UI
    void updateButtons();
    void updateRecentFilesMenu();
    
    // Validação de seleção
    bool validateSelection();
    
    // Utilitários
    void acceptSelection();
    void saveRecentFiles();
    void loadRecentFiles();
};

}}} // namespace wydstudio::ui::common

#endif // WYDSTUDIO_FILE_BROWSER_DIALOG_H