#ifndef WYDSTUDIO_FILE_BROWSER_H
#define WYDSTUDIO_FILE_BROWSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <QWidget>
#include <QFileDialog>
#include <QTreeView>
#include <QListView>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QTabWidget>
#include <QSplitter>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QPixmap>
#include <QIcon>
#include <QContextMenuEvent>
#include <QSortFilterProxyModel>
#include "../../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {
namespace common {

/**
 * @brief Tipo de arquivo reconhecido pelo WYD
 */
enum class WYDFileType {
    ITEM_LIST,          // Lista de itens (.bin/.json)
    ITEM_NAME,          // Nomes de itens (.bin/.json)
    ITEM_HELP,          // Descrições de itens (.bin/.json)
    ITEM_PRICE,         // Preços de itens (.bin/.json)
    MAP_DATA,           // Dados de mapa (.dat/.json)
    MAP_HEIGHT,         // Altura do terreno (.dat/.json)
    MAP_ATTRIBUTE,      // Atributos do mapa (.dat/.json)
    NPC_DATA,           // Dados de NPCs (.bin/.json)
    MOB_DATA,           // Dados de monstros (.bin/.json)
    SKILL_DATA,         // Dados de habilidades (.bin/.json)
    QUEST_DATA,         // Dados de quests (.bin/.json)
    EVENT_DATA,         // Dados de eventos (.bin/.json)
    UI_COMPONENT,       // Componentes de interface (.wyt/.json)
    UI_TEXTURE,         // Texturas de interface (.dds/.png)
    UI_CONFIG,          // Configuração de interface (.bin/.json)
    GAME_CONFIG,        // Configuração do jogo (.bin/.json)
    SERVER_CONFIG,      // Configuração do servidor (.bin/.json/.conf)
    TEXT_RESOURCE,      // Recursos de texto (.txt)
    GENERIC_BINARY,     // Binário genérico
    GENERIC_TEXT,       // Texto genérico
    GENERIC_IMAGE,      // Imagem genérica
    OTHER               // Outro tipo de arquivo
};

/**
 * @brief Mapeamento de extensões para tipos de arquivo WYD
 */
struct WYDFileMapping {
    std::string extension;
    std::string description;
    WYDFileType type;
    bool isEditable;
    bool needsDecompilation;
    std::string relatedEditor;
    std::string defaultLocation;
    std::vector<std::string> supportedFormats;
    std::map<std::string, std::string> additionalInfo;
};

/**
 * @brief Modo de exibição do navegador de arquivos
 */
enum class FileBrowserViewMode {
    LIST,       // Visualização em lista
    TREE,       // Visualização em árvore
    ICONS,      // Visualização com ícones grandes
    DETAILS     // Visualização detalhada
};

/**
 * @brief Item do navegador de arquivos com informações adicionais
 */
class FileBrowserItem : public QStandardItem {
public:
    FileBrowserItem(const QString& text, const QString& filePath, WYDFileType fileType);
    
    QString getFilePath() const;
    WYDFileType getFileType() const;
    bool isDirectory() const;
    bool isCompiled() const;
    bool isDecompiled() const;
    
    // Metadados adicionais
    void setMetadata(const QString& key, const QVariant& value);
    QVariant getMetadata(const QString& key) const;
    bool hasMetadata(const QString& key) const;
    
private:
    QString m_filePath;
    WYDFileType m_fileType;
    bool m_isDirectory;
    bool m_isCompiled;
    bool m_isDecompiled;
    QMap<QString, QVariant> m_metadata;
};

/**
 * @brief Delegate personalizado para exibição de itens
 */
class FileBrowserItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    FileBrowserItemDelegate(QObject* parent = nullptr);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, 
              const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, 
                  const QModelIndex& index) const override;
    
private:
    QMap<WYDFileType, QIcon> m_typeIcons;
    QIcon m_folderIcon;
    QIcon m_fileIcon;
    QIcon m_compiledIcon;
    QIcon m_decompiledIcon;
    
    void initIcons();
};

/**
 * @brief Modelo personalizado para exibição de arquivos
 */
class FileBrowserModel : public QStandardItemModel {
    Q_OBJECT
    
public:
    FileBrowserModel(QObject* parent = nullptr);
    
    void loadDirectory(const QString& path, bool recursive = false);
    void clear();
    
    FileBrowserItem* getItem(const QModelIndex& index) const;
    FileBrowserItem* findItemByPath(const QString& path) const;
    
    QList<FileBrowserItem*> getAllItems() const;
    QList<FileBrowserItem*> getItemsByType(WYDFileType type) const;
    
private:
    void addFile(const QString& filePath, QStandardItem* parent = nullptr);
    void addDirectory(const QString& dirPath, QStandardItem* parent = nullptr, bool recursive = false);
    
    WYDFileType detectFileType(const QString& filePath) const;
    QString getFileDescription(WYDFileType type) const;
    QIcon getFileIcon(WYDFileType type, bool isCompiled) const;
    
    QMap<QString, WYDFileMapping> m_fileMappings;
    
    void initFileMappings();
};

/**
 * @brief Filtro para o modelo de navegação de arquivos
 */
class FileBrowserFilter : public QSortFilterProxyModel {
    Q_OBJECT
    
public:
    FileBrowserFilter(QObject* parent = nullptr);
    
    void setFilterWYDType(const QSet<WYDFileType>& types);
    void setFilterExtensions(const QStringList& extensions);
    void setShowCompiledFiles(bool show);
    void setShowDecompiledFiles(bool show);
    void setFilterName(const QString& nameFilter);
    
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    
private:
    QSet<WYDFileType> m_filterTypes;
    QStringList m_filterExtensions;
    bool m_showCompiledFiles;
    bool m_showDecompiledFiles;
    QString m_nameFilter;
};

/**
 * @brief Configuração para o navegador de arquivos
 */
struct FileBrowserConfig {
    QString initialDirectory;
    QSet<WYDFileType> defaultFilterTypes;
    QStringList defaultFilterExtensions;
    bool showCompiledFiles;
    bool showDecompiledFiles;
    FileBrowserViewMode defaultViewMode;
    bool allowMultipleSelection;
    bool showHiddenFiles;
    bool showFilePreview;
    bool showDirectories;
    bool showFileInfo;
    bool allowFileOperations;
    QStringList favoriteLocations;
    std::map<std::string, std::string> customSettings;
};

/**
 * @brief Widget principal do navegador de arquivos
 */
class FileBrowser : public QWidget {
    Q_OBJECT
    
public:
    /**
     * @brief Construtor
     * @param parent Widget pai
     */
    FileBrowser(QWidget* parent = nullptr);
    
    /**
     * @brief Destrutor
     */
    ~FileBrowser();
    
    /**
     * @brief Inicializa o navegador de arquivos
     * @param facade Referência ao facade do WYDStudio
     * @param config Configuração do navegador
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::shared_ptr<core::WYDStudioFacade>& facade, 
                   const FileBrowserConfig& config);
    
    /**
     * @brief Define a função de callback para quando um arquivo é selecionado
     * @param callback Função a ser chamada quando um arquivo é selecionado
     */
    void setFileSelectedCallback(std::function<void(const std::string&, WYDFileType)> callback);
    
    /**
     * @brief Define a função de callback para quando um arquivo é aberto
     * @param callback Função a ser chamada quando um arquivo é aberto
     */
    void setFileOpenedCallback(std::function<void(const std::string&, WYDFileType)> callback);
    
    /**
     * @brief Define a função de callback para quando um arquivo é arrastado
     * @param callback Função a ser chamada quando um arquivo é arrastado
     */
    void setFileDraggedCallback(std::function<void(const std::string&, WYDFileType)> callback);
    
    /**
     * @brief Navega para um diretório específico
     * @param dirPath Caminho do diretório
     */
    void navigateToDirectory(const std::string& dirPath);
    
    /**
     * @brief Seleciona um arquivo específico
     * @param filePath Caminho do arquivo
     */
    void selectFile(const std::string& filePath);
    
    /**
     * @brief Atualiza a exibição do navegador
     */
    void refresh();
    
    /**
     * @brief Define o modo de visualização
     * @param mode Modo de visualização
     */
    void setViewMode(FileBrowserViewMode mode);
    
    /**
     * @brief Define os tipos de arquivo a serem exibidos
     * @param types Conjunto de tipos de arquivo
     */
    void setFilterTypes(const QSet<WYDFileType>& types);
    
    /**
     * @brief Define as extensões de arquivo a serem exibidas
     * @param extensions Lista de extensões
     */
    void setFilterExtensions(const QStringList& extensions);
    
    /**
     * @brief Define se arquivos compilados devem ser exibidos
     * @param show Se verdadeiro, exibe arquivos compilados
     */
    void setShowCompiledFiles(bool show);
    
    /**
     * @brief Define se arquivos decompilados devem ser exibidos
     * @param show Se verdadeiro, exibe arquivos decompilados
     */
    void setShowDecompiledFiles(bool show);
    
    /**
     * @brief Define um filtro por nome de arquivo
     * @param nameFilter Texto para filtrar nomes de arquivo
     */
    void setFilterName(const QString& nameFilter);
    
    /**
     * @brief Obtém o diretório atual
     * @return Caminho do diretório atual
     */
    std::string getCurrentDirectory() const;
    
    /**
     * @brief Obtém o arquivo selecionado
     * @return Caminho do arquivo selecionado
     */
    std::string getSelectedFile() const;
    
    /**
     * @brief Obtém os arquivos selecionados
     * @return Lista de caminhos de arquivos selecionados
     */
    std::vector<std::string> getSelectedFiles() const;
    
    /**
     * @brief Verifica se um arquivo é do tipo especificado
     * @param filePath Caminho do arquivo
     * @param type Tipo de arquivo
     * @return true se o arquivo for do tipo especificado
     */
    bool isFileOfType(const std::string& filePath, WYDFileType type) const;
    
    /**
     * @brief Detecta o tipo de um arquivo
     * @param filePath Caminho do arquivo
     * @return Tipo do arquivo
     */
    WYDFileType detectFileType(const std::string& filePath) const;
    
    /**
     * @brief Verifica se um arquivo está compilado
     * @param filePath Caminho do arquivo
     * @return true se o arquivo estiver compilado
     */
    bool isFileCompiled(const std::string& filePath) const;
    
    /**
     * @brief Verifica se um arquivo está decompilado
     * @param filePath Caminho do arquivo
     * @return true se o arquivo estiver decompilado
     */
    bool isFileDecompiled(const std::string& filePath) const;
    
    /**
     * @brief Obtém o caminho do arquivo decompilado correspondente
     * @param compiledFilePath Caminho do arquivo compilado
     * @return Caminho do arquivo decompilado ou string vazia se não existir
     */
    std::string getDecompiledFilePath(const std::string& compiledFilePath) const;
    
    /**
     * @brief Obtém o caminho do arquivo compilado correspondente
     * @param decompiledFilePath Caminho do arquivo decompilado
     * @return Caminho do arquivo compilado ou string vazia se não existir
     */
    std::string getCompiledFilePath(const std::string& decompiledFilePath) const;
    
    /**
     * @brief Decompila um arquivo
     * @param filePath Caminho do arquivo
     * @return true se decompilado com sucesso
     */
    bool decompileFile(const std::string& filePath);
    
    /**
     * @brief Compila um arquivo
     * @param filePath Caminho do arquivo
     * @return true se compilado com sucesso
     */
    bool compileFile(const std::string& filePath);
    
    /**
     * @brief Obtém uma descrição para um tipo de arquivo
     * @param type Tipo de arquivo
     * @return Descrição do tipo de arquivo
     */
    std::string getFileTypeDescription(WYDFileType type) const;
    
    /**
     * @brief Adiciona um local favorito
     * @param path Caminho do diretório
     * @param name Nome do favorito (opcional)
     */
    void addFavoriteLocation(const std::string& path, const std::string& name = "");
    
    /**
     * @brief Remove um local favorito
     * @param path Caminho do diretório
     */
    void removeFavoriteLocation(const std::string& path);
    
    /**
     * @brief Obtém os locais favoritos
     * @return Lista de caminhos de locais favoritos
     */
    std::vector<std::string> getFavoriteLocations() const;
    
signals:
    /**
     * @brief Sinal emitido quando um arquivo é selecionado
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo
     */
    void fileSelected(const std::string& filePath, WYDFileType fileType);
    
    /**
     * @brief Sinal emitido quando um arquivo é aberto
     * @param filePath Caminho do arquivo
     * @param fileType Tipo do arquivo
     */
    void fileOpened(const std::string& filePath, WYDFileType fileType);
    
    /**
     * @brief Sinal emitido quando um diretório é alterado
     * @param dirPath Caminho do diretório
     */
    void directoryChanged(const std::string& dirPath);
    
    /**
     * @brief Sinal emitido quando um arquivo é decompilado
     * @param originalPath Caminho do arquivo original
     * @param decompiledPath Caminho do arquivo decompilado
     */
    void fileDecompiled(const std::string& originalPath, const std::string& decompiledPath);
    
    /**
     * @brief Sinal emitido quando um arquivo é compilado
     * @param originalPath Caminho do arquivo original
     * @param compiledPath Caminho do arquivo compilado
     */
    void fileCompiled(const std::string& originalPath, const std::string& compiledPath);
    
protected:
    /**
     * @brief Manipula eventos de arrastar e soltar
     * @param event Evento de arrastar e soltar
     */
    void dragEnterEvent(QDragEnterEvent* event) override;
    
    /**
     * @brief Manipula eventos de movimento de arrastar
     * @param event Evento de movimento de arrastar
     */
    void dragMoveEvent(QDragMoveEvent* event) override;
    
    /**
     * @brief Manipula eventos de soltar
     * @param event Evento de soltar
     */
    void dropEvent(QDropEvent* event) override;
    
    /**
     * @brief Manipula eventos de contexto de menu
     * @param event Evento de contexto de menu
     */
    void contextMenuEvent(QContextMenuEvent* event) override;
    
private slots:
    /**
     * @brief Slot chamado quando um arquivo é clicado
     * @param index Índice do modelo
     */
    void onFileClicked(const QModelIndex& index);
    
    /**
     * @brief Slot chamado quando um arquivo é clicado duas vezes
     * @param index Índice do modelo
     */
    void onFileDoubleClicked(const QModelIndex& index);
    
    /**
     * @brief Slot chamado quando o botão de navegação para cima é clicado
     */
    void onNavigateUpClicked();
    
    /**
     * @brief Slot chamado quando o botão de atualização é clicado
     */
    void onRefreshClicked();
    
    /**
     * @brief Slot chamado quando o botão de favoritos é clicado
     */
    void onFavoritesClicked();
    
    /**
     * @brief Slot chamado quando o botão de localização inicial é clicado
     */
    void onHomeClicked();
    
    /**
     * @brief Slot chamado quando o botão de histórico de navegação para trás é clicado
     */
    void onNavigateBackClicked();
    
    /**
     * @brief Slot chamado quando o botão de histórico de navegação para frente é clicado
     */
    void onNavigateForwardClicked();
    
    /**
     * @brief Slot chamado quando o modo de visualização é alterado
     * @param index Índice do modo de visualização
     */
    void onViewModeChanged(int index);
    
    /**
     * @brief Slot chamado quando o filtro de tipo de arquivo é alterado
     * @param index Índice do filtro de tipo
     */
    void onFilterTypeChanged(int index);
    
    /**
     * @brief Slot chamado quando o filtro de nome é alterado
     * @param text Texto do filtro
     */
    void onFilterNameChanged(const QString& text);
    
    /**
     * @brief Slot chamado quando a opção de mostrar arquivos compilados é alterada
     * @param checked Estado da opção
     */
    void onShowCompiledChanged(bool checked);
    
    /**
     * @brief Slot chamado quando a opção de mostrar arquivos decompilados é alterada
     * @param checked Estado da opção
     */
    void onShowDecompiledChanged(bool checked);
    
    /**
     * @brief Slot chamado quando um local favorito é selecionado
     * @param action Ação selecionada
     */
    void onFavoriteLocationSelected(QAction* action);
    
    /**
     * @brief Slot chamado quando a opção de adicionar local favorito é selecionada
     */
    void onAddFavoriteLocation();
    
    /**
     * @brief Slot chamado quando o botão de decompilação é clicado
     */
    void onDecompileClicked();
    
    /**
     * @brief Slot chamado quando o botão de compilação é clicado
     */
    void onCompileClicked();
    
    /**
     * @brief Slot chamado quando a opção de abertura com editor padrão é selecionada
     */
    void onOpenWithDefaultEditor();
    
    /**
     * @brief Slot chamado quando a opção de exibição de propriedades é selecionada
     */
    void onShowProperties();
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Interface do usuário
    QGridLayout* m_mainLayout;
    QToolBar* m_toolbar;
    QComboBox* m_locationComboBox;
    QComboBox* m_filterTypeComboBox;
    QComboBox* m_viewModeComboBox;
    QLineEdit* m_filterLineEdit;
    QCheckBox* m_showCompiledCheckBox;
    QCheckBox* m_showDecompiledCheckBox;
    QPushButton* m_decompileButton;
    QPushButton* m_compileButton;
    
    // Visualização de arquivos
    QWidget* m_viewContainer;
    QTreeView* m_treeView;
    QListView* m_listView;
    
    // Modelos e filtros
    FileBrowserModel* m_model;
    FileBrowserFilter* m_proxyModel;
    FileBrowserItemDelegate* m_itemDelegate;
    
    // Visualização de informações/preview
    QWidget* m_infoPanel;
    QLabel* m_fileInfoLabel;
    QLabel* m_previewLabel;
    
    // Referência ao facade
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Configuração
    FileBrowserConfig m_config;
    
    // Estado atual
    QString m_currentDirectory;
    QStringList m_navigationHistory;
    int m_historyPosition;
    
    // Callbacks
    std::function<void(const std::string&, WYDFileType)> m_fileSelectedCallback;
    std::function<void(const std::string&, WYDFileType)> m_fileOpenedCallback;
    std::function<void(const std::string&, WYDFileType)> m_fileDraggedCallback;
    
    // Inicialização da UI
    void createUI();
    
    // Atualização de UI
    void updateButtons();
    void updateLocationComboBox();
    void updateFileInfo(const QString& filePath);
    void updatePreview(const QString& filePath);
    
    // Navegação
    void navigateToHistory(int index);
    void addToHistory(const QString& path);
    
    // Utilitários
    QIcon getIconForFileType(WYDFileType type, bool isCompiled) const;
    QString getFileExtension(const QString& filePath) const;
    void buildFileTypesMapping();
    
    // Operações com arquivos
    void openFile(const QString& filePath);
    bool deleteFile(const QString& filePath);
    bool renameFile(const QString& oldPath, const QString& newPath);
    bool copyFile(const QString& sourcePath, const QString& destPath);
    
    // Gerenciamento de favoritos
    void loadFavoriteLocations();
    void saveFavoriteLocations();
    
    // Menus contextuais
    QMenu* createFileContextMenu(const QString& filePath);
    QMenu* createDirectoryContextMenu(const QString& dirPath);
    QMenu* createBackgroundContextMenu();
};

}}} // namespace wydstudio::ui::common

#endif // WYDSTUDIO_FILE_BROWSER_H