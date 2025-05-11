#ifndef WYDSTUDIO_UI_MANAGER_H
#define WYDSTUDIO_UI_MANAGER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <variant>
#include <chrono>
#include <any>
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace ui {

// Forward declarations
class Window;
class Panel;
class Control;
class Menu;
class Toolbar;
class StatusBar;
class MenuItem;
class ToolbarItem;
class DockPanel;
class Dialog;
class Theme;
class StyleSheet;

/**
 * @brief Evento de UI
 */
struct UIEvent {
    enum class Type {
        MOUSE_MOVE,
        MOUSE_ENTER,
        MOUSE_LEAVE,
        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_CLICK,
        MOUSE_DOUBLE_CLICK,
        KEY_DOWN,
        KEY_UP,
        KEY_PRESS,
        FOCUS_GAINED,
        FOCUS_LOST,
        VALUE_CHANGED,
        SELECTION_CHANGED,
        WINDOW_RESIZE,
        WINDOW_MOVE,
        WINDOW_CLOSE,
        WINDOW_OPEN,
        DRAG_START,
        DRAG_END,
        DRAG_ENTER,
        DRAG_LEAVE,
        DRAG_OVER,
        DROP,
        CUSTOM
    };
    
    Type type;
    std::string source;      // ID do controle de origem
    std::any data;           // Dados específicos do evento
    bool handled;            // Se o evento foi tratado
    std::chrono::system_clock::time_point timestamp;
    
    UIEvent(Type eventType) : type(eventType), handled(false),
                            timestamp(std::chrono::system_clock::now()) {}
};

/**
 * @brief Dados de evento de mouse
 */
struct MouseEventData {
    int x;
    int y;
    enum class Button {
        NONE,
        LEFT,
        RIGHT,
        MIDDLE,
        X1,
        X2
    } button;
    
    bool shiftKey;
    bool ctrlKey;
    bool altKey;
    bool metaKey;
    
    MouseEventData() : x(0), y(0), button(Button::NONE),
                     shiftKey(false), ctrlKey(false), altKey(false), metaKey(false) {}
};

/**
 * @brief Dados de evento de teclado
 */
struct KeyEventData {
    int keyCode;
    char keyChar;
    bool shiftKey;
    bool ctrlKey;
    bool altKey;
    bool metaKey;
    bool repeat;
    
    KeyEventData() : keyCode(0), keyChar(0),
                   shiftKey(false), ctrlKey(false), altKey(false), metaKey(false),
                   repeat(false) {}
};

/**
 * @brief Dados de evento de redimensionamento
 */
struct ResizeEventData {
    int width;
    int height;
    int oldWidth;
    int oldHeight;
    
    ResizeEventData() : width(0), height(0), oldWidth(0), oldHeight(0) {}
};

/**
 * @brief Dados de evento de arrastar e soltar
 */
struct DragEventData {
    int x;
    int y;
    std::string sourceId;
    std::any data;
    std::vector<std::string> allowedEffects;
    std::string effect;
    
    DragEventData() : x(0), y(0) {}
};

/**
 * @brief Tipos de controle de UI
 */
enum class ControlType {
    UNKNOWN,
    BUTTON,
    LABEL,
    TEXTBOX,
    CHECKBOX,
    RADIO,
    COMBOBOX,
    LISTBOX,
    GRID,
    TREE,
    TAB,
    PANEL,
    SCROLLBAR,
    PROGRESSBAR,
    SLIDER,
    IMAGE,
    MENU,
    MENUITEM,
    TOOLBAR,
    STATUSBAR,
    CUSTOM
};

/**
 * @brief Estilos de controle de UI
 */
struct ControlStyle {
    // Dimensões e posição
    int x;
    int y;
    int width;
    int height;
    int minWidth;
    int minHeight;
    int maxWidth;
    int maxHeight;
    
    // Cores
    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
        
        Color() : r(0), g(0), b(0), a(255) {}
        Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
            : r(red), g(green), b(blue), a(alpha) {}
        
        std::string toString() const;
        static Color fromString(const std::string& colorStr);
        static Color fromHex(const std::string& hexColor);
    };
    
    std::optional<Color> backgroundColor;
    std::optional<Color> foregroundColor;
    std::optional<Color> borderColor;
    
    // Borda
    int borderWidth;
    enum class BorderStyle {
        NONE,
        SOLID,
        DASHED,
        DOTTED,
        DOUBLE,
        GROOVE,
        RIDGE,
        INSET,
        OUTSET
    } borderStyle;
    int borderRadius;
    
    // Fonte
    std::string fontFamily;
    int fontSize;
    enum class FontWeight {
        NORMAL,
        BOLD,
        BOLDER,
        LIGHTER
    } fontWeight;
    enum class FontStyle {
        NORMAL,
        ITALIC,
        OBLIQUE
    } fontStyle;
    
    // Margens e preenchimento
    int marginTop;
    int marginRight;
    int marginBottom;
    int marginLeft;
    int paddingTop;
    int paddingRight;
    int paddingBottom;
    int paddingLeft;
    
    // Alinhamento
    enum class TextAlign {
        LEFT,
        CENTER,
        RIGHT,
        JUSTIFY
    } textAlign;
    
    enum class VerticalAlign {
        TOP,
        MIDDLE,
        BOTTOM
    } verticalAlign;
    
    // Visibilidade e habilitação
    bool visible;
    bool enabled;
    
    // Outros
    int zIndex;
    std::string cursor;
    
    // Extensões específicas por tipo
    std::unordered_map<std::string, std::string> extraStyles;
    
    // Construtores
    ControlStyle() : x(0), y(0), width(100), height(30),
                   minWidth(0), minHeight(0), maxWidth(0), maxHeight(0),
                   borderWidth(0), borderStyle(BorderStyle::NONE), borderRadius(0),
                   fontSize(12), fontWeight(FontWeight::NORMAL), fontStyle(FontStyle::NORMAL),
                   marginTop(0), marginRight(0), marginBottom(0), marginLeft(0),
                   paddingTop(0), paddingRight(0), paddingBottom(0), paddingLeft(0),
                   textAlign(TextAlign::LEFT), verticalAlign(VerticalAlign::MIDDLE),
                   visible(true), enabled(true), zIndex(0), cursor("default") {}
};

/**
 * @brief Leiaute para organização de controles
 */
enum class LayoutType {
    NONE,
    HORIZONTAL,
    VERTICAL,
    GRID,
    STACK,
    DOCK,
    FLOW,
    ABSOLUTE
};

/**
 * @brief Configuração de leiaute
 */
struct LayoutConfig {
    LayoutType type;
    int spacing;
    int margin;
    bool expandHorizontal;
    bool expandVertical;
    int rows;
    int columns;
    
    LayoutConfig() : type(LayoutType::NONE), spacing(5), margin(5),
                   expandHorizontal(false), expandVertical(false),
                   rows(0), columns(0) {}
};

/**
 * @brief Base para controles de UI
 */
class Control : public std::enable_shared_from_this<Control> {
public:
    Control(const std::string& id, ControlType type = ControlType::UNKNOWN);
    virtual ~Control() = default;
    
    // Identificação
    std::string getId() const;
    ControlType getType() const;
    void setName(const std::string& name);
    std::string getName() const;
    
    // Hierarquia
    void setParent(std::shared_ptr<Control> parent);
    std::shared_ptr<Control> getParent() const;
    void addChild(std::shared_ptr<Control> child);
    bool removeChild(const std::string& id);
    void clearChildren();
    
    std::shared_ptr<Control> findChild(const std::string& id) const;
    std::vector<std::shared_ptr<Control>> getChildren() const;
    
    // Estilo
    void setStyle(const ControlStyle& style);
    ControlStyle& getStyle();
    const ControlStyle& getStyle() const;
    
    // Dados
    void setData(const std::any& data);
    std::any getData() const;
    
    // Estado
    void setVisible(bool visible);
    bool isVisible() const;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    void setFocused(bool focused);
    bool isFocused() const;
    
    // Layout
    void setLayout(const LayoutConfig& layout);
    LayoutConfig& getLayout();
    const LayoutConfig& getLayout() const;
    
    // Eventos
    using EventHandler = std::function<void(const UIEvent&)>;
    void addEventListener(UIEvent::Type eventType, EventHandler handler);
    void removeEventListener(UIEvent::Type eventType);
    bool dispatchEvent(const UIEvent& event);
    
    // Renderização
    virtual void render() = 0;
    
    // Utilitários
    virtual std::string toString() const;
    
protected:
    std::string m_id;
    std::string m_name;
    ControlType m_type;
    ControlStyle m_style;
    LayoutConfig m_layout;
    std::any m_data;
    
    bool m_visible;
    bool m_enabled;
    bool m_focused;
    
    std::weak_ptr<Control> m_parent;
    std::vector<std::shared_ptr<Control>> m_children;
    
    std::unordered_map<UIEvent::Type, EventHandler> m_eventHandlers;
    
    // Métodos auxiliares para subclasses
    virtual void onParentChanged();
    virtual void onStyleChanged();
    virtual void onVisibilityChanged();
    virtual void onEnableChanged();
    virtual void onFocusChanged();
};

/**
 * @brief Painel para agrupar controles
 */
class Panel : public Control {
public:
    Panel(const std::string& id);
    ~Panel() override = default;
    
    // Renderização
    void render() override;
};

/**
 * @brief Botão para ações
 */
class Button : public Control {
public:
    Button(const std::string& id);
    ~Button() override = default;
    
    // Propriedades específicas
    void setText(const std::string& text);
    std::string getText() const;
    
    void setIcon(const std::string& iconPath);
    std::string getIcon() const;
    
    // Renderização
    void render() override;
    
private:
    std::string m_text;
    std::string m_iconPath;
};

/**
 * @brief Rótulo para texto
 */
class Label : public Control {
public:
    Label(const std::string& id);
    ~Label() override = default;
    
    // Propriedades específicas
    void setText(const std::string& text);
    std::string getText() const;
    
    void setHtmlContent(const std::string& html);
    std::string getHtmlContent() const;
    
    // Renderização
    void render() override;
    
private:
    std::string m_text;
    std::string m_htmlContent;
    bool m_isHtml;
};

/**
 * @brief Caixa de texto para entrada de dados
 */
class TextBox : public Control {
public:
    TextBox(const std::string& id);
    ~TextBox() override = default;
    
    // Propriedades específicas
    void setText(const std::string& text);
    std::string getText() const;
    
    void setPlaceholder(const std::string& placeholder);
    std::string getPlaceholder() const;
    
    void setPassword(bool isPassword);
    bool isPassword() const;
    
    void setMultiline(bool multiline);
    bool isMultiline() const;
    
    void setMaxLength(int maxLength);
    int getMaxLength() const;
    
    // Renderização
    void render() override;
    
private:
    std::string m_text;
    std::string m_placeholder;
    bool m_isPassword;
    bool m_multiline;
    int m_maxLength;
};

/**
 * @brief Caixa de seleção
 */
class CheckBox : public Control {
public:
    CheckBox(const std::string& id);
    ~CheckBox() override = default;
    
    // Propriedades específicas
    void setText(const std::string& text);
    std::string getText() const;
    
    void setChecked(bool checked);
    bool isChecked() const;
    
    // Renderização
    void render() override;
    
private:
    std::string m_text;
    bool m_checked;
};

/**
 * @brief Caixa de combinação
 */
class ComboBox : public Control {
public:
    struct Item {
        std::string text;
        std::string value;
        std::any data;
        
        Item(const std::string& itemText = "", const std::string& itemValue = "")
            : text(itemText), value(itemValue) {}
    };
    
    ComboBox(const std::string& id);
    ~ComboBox() override = default;
    
    // Propriedades específicas
    void addItem(const Item& item);
    void addItem(const std::string& text, const std::string& value = "");
    void removeItem(int index);
    void clearItems();
    
    void setSelectedIndex(int index);
    int getSelectedIndex() const;
    
    void setSelectedValue(const std::string& value);
    std::string getSelectedValue() const;
    
    Item getSelectedItem() const;
    std::vector<Item> getItems() const;
    
    // Renderização
    void render() override;
    
private:
    std::vector<Item> m_items;
    int m_selectedIndex;
};

/**
 * @brief Grade de dados
 */
class Grid : public Control {
public:
    struct Column {
        std::string id;
        std::string header;
        int width;
        bool resizable;
        bool sortable;
        bool visible;
        std::string alignment;
        
        Column(const std::string& columnId, const std::string& columnHeader)
            : id(columnId), header(columnHeader), width(100),
              resizable(true), sortable(true), visible(true),
              alignment("left") {}
    };
    
    using RowData = std::unordered_map<std::string, std::any>;
    
    Grid(const std::string& id);
    ~Grid() override = default;
    
    // Configuração de colunas
    void addColumn(const Column& column);
    void removeColumn(const std::string& columnId);
    void clearColumns();
    std::vector<Column> getColumns() const;
    
    // Dados
    void addRow(const RowData& rowData);
    void updateRow(int rowIndex, const RowData& rowData);
    void removeRow(int rowIndex);
    void clearRows();
    
    void setDataSource(const std::vector<RowData>& dataSource);
    std::vector<RowData> getDataSource() const;
    
    // Seleção
    void setSelectedRow(int rowIndex);
    int getSelectedRow() const;
    RowData getSelectedRowData() const;
    
    void setMultiSelect(bool multiSelect);
    bool isMultiSelect() const;
    
    std::vector<int> getSelectedRows() const;
    
    // Ordenação e filtro
    void sortByColumn(const std::string& columnId, bool ascending = true);
    void setFilter(const std::string& filterExpression);
    std::string getFilter() const;
    
    // Renderização
    void render() override;
    
private:
    std::vector<Column> m_columns;
    std::vector<RowData> m_dataSource;
    int m_selectedRow;
    std::vector<int> m_selectedRows;
    bool m_multiSelect;
    std::string m_sortColumn;
    bool m_sortAscending;
    std::string m_filterExpression;
};

/**
 * @brief Janela principal
 */
class Window : public Control {
public:
    Window(const std::string& id);
    ~Window() override = default;
    
    // Propriedades específicas
    void setTitle(const std::string& title);
    std::string getTitle() const;
    
    void setIcon(const std::string& iconPath);
    std::string getIcon() const;
    
    void setResizable(bool resizable);
    bool isResizable() const;
    
    void setMaximizable(bool maximizable);
    bool isMaximizable() const;
    
    void setMinimizable(bool minimizable);
    bool isMinimizable() const;
    
    void setFullScreen(bool fullScreen);
    bool isFullScreen() const;
    
    // Estado da janela
    void show();
    void hide();
    void close();
    void maximize();
    void minimize();
    void restore();
    
    bool isMaximized() const;
    bool isMinimized() const;
    
    // Componentes da janela
    void setMenu(std::shared_ptr<Menu> menu);
    std::shared_ptr<Menu> getMenu() const;
    
    void setToolbar(std::shared_ptr<Toolbar> toolbar);
    std::shared_ptr<Toolbar> getToolbar() const;
    
    void setStatusBar(std::shared_ptr<StatusBar> statusBar);
    std::shared_ptr<StatusBar> getStatusBar() const;
    
    void setContent(std::shared_ptr<Control> content);
    std::shared_ptr<Control> getContent() const;
    
    // Renderização
    void render() override;
    
private:
    std::string m_title;
    std::string m_iconPath;
    bool m_resizable;
    bool m_maximizable;
    bool m_minimizable;
    bool m_fullScreen;
    
    bool m_maximized;
    bool m_minimized;
    
    std::shared_ptr<Menu> m_menu;
    std::shared_ptr<Toolbar> m_toolbar;
    std::shared_ptr<StatusBar> m_statusBar;
    std::shared_ptr<Control> m_content;
};

/**
 * @brief Barra de menu
 */
class Menu : public Control {
public:
    Menu(const std::string& id);
    ~Menu() override = default;
    
    // Gerenciamento de itens
    void addItem(std::shared_ptr<MenuItem> item);
    void removeItem(const std::string& id);
    void clearItems();
    
    std::vector<std::shared_ptr<MenuItem>> getItems() const;
    
    // Renderização
    void render() override;
    
private:
    std::vector<std::shared_ptr<MenuItem>> m_items;
};

/**
 * @brief Item de menu
 */
class MenuItem : public Control {
public:
    MenuItem(const std::string& id);
    ~MenuItem() override = default;
    
    // Propriedades específicas
    void setText(const std::string& text);
    std::string getText() const;
    
    void setIcon(const std::string& iconPath);
    std::string getIcon() const;
    
    void setShortcut(const std::string& shortcut);
    std::string getShortcut() const;
    
    void setChecked(bool checked);
    bool isChecked() const;
    
    void setCheckable(bool checkable);
    bool isCheckable() const;
    
    // Gerenciamento de subitens
    void addSubItem(std::shared_ptr<MenuItem> item);
    void removeSubItem(const std::string& id);
    void clearSubItems();
    
    std::vector<std::shared_ptr<MenuItem>> getSubItems() const;
    bool hasSubItems() const;
    
    // Ações
    void setAction(std::function<void()> action);
    
    // Renderização
    void render() override;
    
private:
    std::string m_text;
    std::string m_iconPath;
    std::string m_shortcut;
    bool m_checked;
    bool m_checkable;
    
    std::vector<std::shared_ptr<MenuItem>> m_subItems;
    std::function<void()> m_action;
};

/**
 * @brief Barra de ferramentas
 */
class Toolbar : public Control {
public:
    Toolbar(const std::string& id);
    ~Toolbar() override = default;
    
    // Propriedades específicas
    void setFloatable(bool floatable);
    bool isFloatable() const;
    
    // Gerenciamento de itens
    void addItem(std::shared_ptr<ToolbarItem> item);
    void removeItem(const std::string& id);
    void clearItems();
    
    std::vector<std::shared_ptr<ToolbarItem>> getItems() const;
    
    // Separador
    void addSeparator();
    
    // Renderização
    void render() override;
    
private:
    bool m_floatable;
    std::vector<std::shared_ptr<ToolbarItem>> m_items;
};

/**
 * @brief Item de barra de ferramentas
 */
class ToolbarItem : public Control {
public:
    ToolbarItem(const std::string& id);
    ~ToolbarItem() override = default;
    
    // Propriedades específicas
    void setText(const std::string& text);
    std::string getText() const;
    
    void setIcon(const std::string& iconPath);
    std::string getIcon() const;
    
    void setTooltip(const std::string& tooltip);
    std::string getTooltip() const;
    
    // Ações
    void setAction(std::function<void()> action);
    
    // Renderização
    void render() override;
    
private:
    std::string m_text;
    std::string m_iconPath;
    std::string m_tooltip;
    std::function<void()> m_action;
};

/**
 * @brief Barra de status
 */
class StatusBar : public Control {
public:
    StatusBar(const std::string& id);
    ~StatusBar() override = default;
    
    // Gerenciamento de painéis
    void addPanel(const std::string& id, const std::string& text, int width = 0);
    void setPanelText(const std::string& id, const std::string& text);
    std::string getPanelText(const std::string& id) const;
    void removePanel(const std::string& id);
    void clearPanels();
    
    // Renderização
    void render() override;
    
private:
    struct Panel {
        std::string id;
        std::string text;
        int width;
        
        Panel(const std::string& panelId, const std::string& panelText, int panelWidth = 0)
            : id(panelId), text(panelText), width(panelWidth) {}
    };
    
    std::vector<Panel> m_panels;
};

/**
 * @brief Diálogo modal
 */
class Dialog : public Window {
public:
    enum class DialogType {
        STANDARD,
        INFORMATION,
        WARNING,
        ERROR,
        QUESTION,
        CUSTOM
    };
    
    enum class DialogResult {
        NONE,
        OK,
        CANCEL,
        YES,
        NO,
        RETRY,
        ABORT,
        IGNORE
    };
    
    Dialog(const std::string& id, DialogType type = DialogType::STANDARD);
    ~Dialog() override = default;
    
    // Propriedades específicas
    void setType(DialogType type);
    DialogType getType() const;
    
    void setMessage(const std::string& message);
    std::string getMessage() const;
    
    void setButtons(const std::vector<DialogResult>& buttons);
    std::vector<DialogResult> getButtons() const;
    
    // Resultado
    void setResult(DialogResult result);
    DialogResult getResult() const;
    
    // Exibição
    DialogResult showModal();
    
    // Eventos
    using ResultHandler = std::function<void(DialogResult)>;
    void setResultHandler(ResultHandler handler);
    
    // Renderização
    void render() override;
    
private:
    DialogType m_type;
    std::string m_message;
    std::vector<DialogResult> m_buttons;
    DialogResult m_result;
    ResultHandler m_resultHandler;
};

/**
 * @brief Gerenciador de temas de UI
 */
class ThemeManager {
public:
    // Singleton
    static ThemeManager& getInstance();
    
    // Gerenciamento de temas
    bool loadTheme(const std::string& name, const std::string& filePath);
    bool setCurrentTheme(const std::string& name);
    std::string getCurrentThemeName() const;
    std::shared_ptr<Theme> getCurrentTheme() const;
    std::shared_ptr<Theme> getTheme(const std::string& name) const;
    std::vector<std::string> getAvailableThemes() const;
    
    // CSS
    std::string getStyleForControl(ControlType type) const;
    std::string getStyleForId(const std::string& id) const;
    std::string getStyleForClass(const std::string& className) const;
    
private:
    ThemeManager();
    ~ThemeManager();
    
    // Prevenção de cópia
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
    
    std::unordered_map<std::string, std::shared_ptr<Theme>> m_themes;
    std::string m_currentThemeName;
};

/**
 * @brief Tema da interface
 */
class Theme {
public:
    Theme(const std::string& name);
    ~Theme() = default;
    
    // Propriedades básicas
    std::string getName() const;
    
    // Cores
    void setColor(const std::string& name, const ControlStyle::Color& color);
    ControlStyle::Color getColor(const std::string& name) const;
    
    // Fontes
    void setFontFamily(const std::string& fontFamily);
    std::string getFontFamily() const;
    
    void setFontSize(int size);
    int getFontSize() const;
    
    // Estilos por tipo de controle
    void setStyleForType(ControlType type, const std::string& css);
    std::string getStyleForType(ControlType type) const;
    
    // Estilos por ID
    void setStyleForId(const std::string& id, const std::string& css);
    std::string getStyleForId(const std::string& id) const;
    
    // Estilos por classe
    void setStyleForClass(const std::string& className, const std::string& css);
    std::string getStyleForClass(const std::string& className) const;
    
    // Stylesheet completo
    std::string getStyleSheet() const;
    
private:
    std::string m_name;
    std::unordered_map<std::string, ControlStyle::Color> m_colors;
    std::string m_fontFamily;
    int m_fontSize;
    
    std::unordered_map<ControlType, std::string> m_typeStyles;
    std::unordered_map<std::string, std::string> m_idStyles;
    std::unordered_map<std::string, std::string> m_classStyles;
};

/**
 * @brief Gerenciador da interface de usuário
 */
class UIManager {
public:
    // Singleton
    static UIManager& getInstance();
    
    // Inicialização
    bool initialize();
    bool shutdown();
    bool isInitialized() const { return m_initialized; }
    
    // Gerenciamento de janelas
    std::shared_ptr<Window> createWindow(const std::string& id, const std::string& title = "");
    std::shared_ptr<Window> getWindow(const std::string& id) const;
    std::vector<std::shared_ptr<Window>> getAllWindows() const;
    bool closeWindow(const std::string& id);
    void closeAllWindows();
    
    // Gerenciamento de diálogos
    std::shared_ptr<Dialog> createDialog(const std::string& id, Dialog::DialogType type = Dialog::DialogType::STANDARD);
    Dialog::DialogResult showDialog(const std::string& id, const std::string& message, Dialog::DialogType type = Dialog::DialogType::INFORMATION);
    Dialog::DialogResult showMessageBox(const std::string& title, const std::string& message, Dialog::DialogType type = Dialog::DialogType::INFORMATION);
    
    // Criação de controles
    std::shared_ptr<Button> createButton(const std::string& id, const std::string& text = "");
    std::shared_ptr<Label> createLabel(const std::string& id, const std::string& text = "");
    std::shared_ptr<TextBox> createTextBox(const std::string& id, const std::string& text = "");
    std::shared_ptr<CheckBox> createCheckBox(const std::string& id, const std::string& text = "");
    std::shared_ptr<ComboBox> createComboBox(const std::string& id);
    std::shared_ptr<Grid> createGrid(const std::string& id);
    std::shared_ptr<Panel> createPanel(const std::string& id);
    std::shared_ptr<Menu> createMenu(const std::string& id);
    std::shared_ptr<MenuItem> createMenuItem(const std::string& id, const std::string& text = "");
    std::shared_ptr<Toolbar> createToolbar(const std::string& id);
    std::shared_ptr<ToolbarItem> createToolbarItem(const std::string& id, const std::string& text = "");
    std::shared_ptr<StatusBar> createStatusBar(const std::string& id);
    
    // Evento global
    void addEventListener(UIEvent::Type eventType, Control::EventHandler handler);
    void removeEventListener(UIEvent::Type eventType);
    
    // Ciclo de eventos
    void processEvents();
    
    // Tema
    ThemeManager& getThemeManager() { return ThemeManager::getInstance(); }
    
    // Loop de UI
    void runMainLoop();
    void stopMainLoop();
    bool isRunning() const;
    
    // Utilitários
    std::shared_ptr<Control> findControlById(const std::string& id) const;
    
private:
    UIManager();
    ~UIManager();
    
    // Prevenção de cópia
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    
    bool m_initialized;
    bool m_running;
    
    std::unordered_map<std::string, std::shared_ptr<Window>> m_windows;
    std::unordered_map<std::string, std::shared_ptr<Control>> m_controlsById;
    std::unordered_map<UIEvent::Type, Control::EventHandler> m_globalEventHandlers;
    
    // Ponto de entrada de renderização
    void renderAll();
    
    // Controle de eventos
    void dispatchEvent(const UIEvent& event);
};

} // namespace ui
} // namespace wydstudio

#endif // WYDSTUDIO_UI_MANAGER_H