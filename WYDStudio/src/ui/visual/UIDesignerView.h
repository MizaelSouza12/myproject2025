#ifndef WYDSTUDIO_UI_DESIGNER_VIEW_H
#define WYDSTUDIO_UI_DESIGNER_VIEW_H

#include <QWidget>
#include <QListView>
#include <QTreeView>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QTabWidget>
#include <QScrollArea>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QSplitter>
#include <QStandardItemModel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>
#include <QItemDelegate>
#include <QPainter>
#include <QDrag>
#include <QMimeData>
#include <QDropEvent>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QUndoStack>
#include <QUndoCommand>
#include <QColorDialog>
#include <QFontDialog>
#include <QDockWidget>
#include <QStatusBar>
#include <QTextEdit>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QMainWindow>
#include <memory>

#include "../UIDesigner.h"
#include "../../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {
namespace visual {

// Forward declarations
class ComponentListModel;
class ComponentListView;
class UICanvasScene;
class UICanvasView;
class ElementTreeModel;
class ElementTreeView;
class PropertyEditor;
class UIPreviewWidget;
class StyleEditor;
class ResourceBrowser;
class ThemeEditor;
class LayoutEditor;

/**
 * @brief Comando de inserção de elemento UI
 */
class AddUIElementCommand : public QUndoCommand {
public:
    AddUIElementCommand(const std::string& componentId, const UIElement& element, UIDesignerView* view);
    void undo() override;
    void redo() override;
    
private:
    std::string m_componentId;
    UIElement m_element;
    UIDesignerView* m_view;
};

/**
 * @brief Comando de remoção de elemento UI
 */
class RemoveUIElementCommand : public QUndoCommand {
public:
    RemoveUIElementCommand(const std::string& componentId, const std::string& elementId, UIDesignerView* view);
    void undo() override;
    void redo() override;
    
private:
    std::string m_componentId;
    std::string m_elementId;
    UIElement m_element;
    UIDesignerView* m_view;
};

/**
 * @brief Comando de modificação de elemento UI
 */
class ModifyUIElementCommand : public QUndoCommand {
public:
    ModifyUIElementCommand(const std::string& componentId, const UIElement& oldElement, 
                           const UIElement& newElement, UIDesignerView* view);
    void undo() override;
    void redo() override;
    
private:
    std::string m_componentId;
    UIElement m_oldElement;
    UIElement m_newElement;
    UIDesignerView* m_view;
};

/**
 * @brief Comando de movimentação de elemento UI
 */
class MoveUIElementCommand : public QUndoCommand {
public:
    MoveUIElementCommand(const std::string& componentId, const std::string& elementId, 
                         const std::string& oldParentId, const std::string& newParentId, 
                         int32_t oldIndex, int32_t newIndex, UIDesignerView* view);
    void undo() override;
    void redo() override;
    
private:
    std::string m_componentId;
    std::string m_elementId;
    std::string m_oldParentId;
    std::string m_newParentId;
    int32_t m_oldIndex;
    int32_t m_newIndex;
    UIDesignerView* m_view;
};

/**
 * @brief Item gráfico base para elementos UI
 */
class UIElementItem : public QGraphicsItem {
public:
    UIElementItem(const UIElement& element, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    const UIElement& getElement() const;
    void updateElement(const UIElement& element);
    
    void setSelected(bool selected) override;
    bool isSelected() const;
    
    void setHighlighted(bool highlighted);
    bool isHighlighted() const;
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    
private:
    UIElement m_element;
    bool m_selected;
    bool m_highlighted;
    bool m_dragging;
    QPointF m_dragStartPos;
    
    // Auxiliar para desenho
    void drawElementBackground(QPainter* painter, const QRectF& rect);
    void drawElementBorder(QPainter* painter, const QRectF& rect);
    void drawElementContent(QPainter* painter, const QRectF& rect);
    void drawSelectionHandles(QPainter* painter, const QRectF& rect);
    
    // Funções específicas por tipo
    void drawPanel(QPainter* painter, const QRectF& rect);
    void drawButton(QPainter* painter, const QRectF& rect);
    void drawIcon(QPainter* painter, const QRectF& rect);
    void drawText(QPainter* painter, const QRectF& rect);
    void drawProgressBar(QPainter* painter, const QRectF& rect);
    void drawSlider(QPainter* painter, const QRectF& rect);
    void drawCheckbox(QPainter* painter, const QRectF& rect);
    void drawRadioButton(QPainter* painter, const QRectF& rect);
    void drawTextField(QPainter* painter, const QRectF& rect);
    void drawDropdown(QPainter* painter, const QRectF& rect);
    void drawList(QPainter* painter, const QRectF& rect);
    void drawGrid(QPainter* painter, const QRectF& rect);
    void drawTab(QPainter* painter, const QRectF& rect);
    void drawScrollbar(QPainter* painter, const QRectF& rect);
    void drawImage(QPainter* painter, const QRectF& rect);
    void drawContainer(QPainter* painter, const QRectF& rect);
    void drawSeparator(QPainter* painter, const QRectF& rect);
    void drawTooltip(QPainter* painter, const QRectF& rect);
    void drawCustom(QPainter* painter, const QRectF& rect);
};

/**
 * @brief Cena para o canvas de edição
 */
class UICanvasScene : public QGraphicsScene {
    Q_OBJECT
    
public:
    UICanvasScene(QObject* parent = nullptr);
    
    void setComponent(const UIComponent& component);
    const UIComponent& getComponent() const;
    void clear();
    
    UIElementItem* addElement(const UIElement& element);
    void updateElement(const UIElement& element);
    void removeElement(const std::string& elementId);
    
    UIElementItem* getElementItemById(const std::string& elementId) const;
    UIElementItem* getSelectedElementItem() const;
    
    void clearSelection();
    
    void setGridVisible(bool visible);
    void setSnapToGrid(bool snap);
    void setGridSize(int size);
    
    void updateHierarchy();
    
signals:
    void elementSelected(const UIElement& element);
    void elementMoved(const std::string& elementId, const QPointF& newPos);
    void elementResized(const std::string& elementId, const QRectF& newRect);
    void sceneClicked();
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    
private:
    UIComponent m_component;
    std::map<std::string, UIElementItem*> m_elementItems;
    
    bool m_gridVisible;
    bool m_snapToGrid;
    int m_gridSize;
    
    enum class ResizeHandle {
        NONE,
        TOP_LEFT,
        TOP,
        TOP_RIGHT,
        RIGHT,
        BOTTOM_RIGHT,
        BOTTOM,
        BOTTOM_LEFT,
        LEFT
    };
    
    bool m_isResizing;
    ResizeHandle m_activeHandle;
    QPointF m_resizeStartPos;
    QRectF m_resizeStartRect;
    
    UIElementItem* m_selectedItem;
    
    void updateItemsZOrder();
    void updateParentChildRelationships();
    QPointF snapToGrid(const QPointF& point) const;
    
    ResizeHandle getResizeHandleAt(UIElementItem* item, const QPointF& pos) const;
    QCursor getResizeHandleCursor(ResizeHandle handle) const;
    QRectF calculateResizedRect(const QRectF& startRect, ResizeHandle handle, const QPointF& startPos, const QPointF& currentPos) const;
};

/**
 * @brief Vista para o canvas de edição
 */
class UICanvasView : public QGraphicsView {
    Q_OBJECT
    
public:
    UICanvasView(QWidget* parent = nullptr);
    
    void setScene(UICanvasScene* scene);
    
    void zoomIn();
    void zoomOut();
    void resetZoom();
    
protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    
private:
    qreal m_zoom;
    bool m_isPanning;
    QPoint m_lastPanPos;
};

/**
 * @brief Modelo para a lista de componentes
 */
class ComponentListModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    ComponentListModel(QObject* parent = nullptr);
    
    void setComponents(const std::vector<UIComponent>& components);
    const UIComponent& getComponent(const QModelIndex& index) const;
    
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    
    QModelIndex findComponentById(const std::string& componentId) const;
    void updateComponent(const UIComponent& component);
    
private:
    std::vector<UIComponent> m_components;
};

/**
 * @brief Delegate para itens da lista de componentes
 */
class ComponentListDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    ComponentListDelegate(QObject* parent = nullptr);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, 
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, 
                  const QModelIndex& index) const override;
};

/**
 * @brief Vista para a lista de componentes
 */
class ComponentListView : public QListView {
    Q_OBJECT
    
public:
    ComponentListView(QWidget* parent = nullptr);
    
    void setFilterText(const QString& text);
    
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    
private:
    QString m_filterText;
    QPoint m_dragStartPosition;
};

/**
 * @brief Modelo para a árvore de elementos
 */
class ElementTreeModel : public QStandardItemModel {
    Q_OBJECT
    
public:
    ElementTreeModel(QObject* parent = nullptr);
    
    void setElements(const std::vector<UIElement>& elements);
    const UIElement& getElement(const QModelIndex& index) const;
    
    QModelIndex findElementById(const std::string& elementId) const;
    void updateElement(const UIElement& element);
    void removeElement(const std::string& elementId);
    
private:
    std::map<std::string, QStandardItem*> m_elementItems;
    std::map<QStandardItem*, UIElement> m_elements;
    
    void buildTree(const std::vector<UIElement>& elements);
    QStandardItem* createElementItem(const UIElement& element);
    void addChildrenRecursive(QStandardItem* parentItem, const std::vector<UIElement>& elements, const std::string& parentId);
};

/**
 * @brief Vista para a árvore de elementos
 */
class ElementTreeView : public QTreeView {
    Q_OBJECT
    
public:
    ElementTreeView(QWidget* parent = nullptr);
    
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

/**
 * @brief Editor de propriedades para elementos UI
 */
class PropertyEditor : public QWidget {
    Q_OBJECT
    
public:
    PropertyEditor(QWidget* parent = nullptr);
    
    void setElement(const UIElement& element);
    void clearElement();
    UIElement getUpdatedElement() const;
    
signals:
    void propertyChanged();
    
private:
    UIElement m_element;
    
    // Widgets
    QTabWidget* m_tabWidget;
    
    // Aba geral
    QWidget* m_generalTab;
    QLineEdit* m_idEdit;
    QLineEdit* m_nameEdit;
    QComboBox* m_typeComboBox;
    QTextEdit* m_descriptionEdit;
    
    // Aba de layout
    QWidget* m_layoutTab;
    QSpinBox* m_xSpinBox;
    QSpinBox* m_ySpinBox;
    QSpinBox* m_widthSpinBox;
    QSpinBox* m_heightSpinBox;
    QComboBox* m_hAlignComboBox;
    QComboBox* m_vAlignComboBox;
    QComboBox* m_parentAnchorComboBox;
    QComboBox* m_childAnchorComboBox;
    QSpinBox* m_marginLeftSpinBox;
    QSpinBox* m_marginTopSpinBox;
    QSpinBox* m_marginRightSpinBox;
    QSpinBox* m_marginBottomSpinBox;
    QCheckBox* m_resizableCheckBox;
    QSpinBox* m_minWidthSpinBox;
    QSpinBox* m_minHeightSpinBox;
    QSpinBox* m_maxWidthSpinBox;
    QSpinBox* m_maxHeightSpinBox;
    QDoubleSpinBox* m_scaleXSpinBox;
    QDoubleSpinBox* m_scaleYSpinBox;
    
    // Aba de conteúdo
    QWidget* m_contentTab;
    QLineEdit* m_textEdit;
    QLineEdit* m_textKeyEdit;
    QLineEdit* m_textBindingPathEdit;
    QLineEdit* m_imagePathEdit;
    QPushButton* m_browseImageButton;
    QLineEdit* m_iconNameEdit;
    QPushButton* m_browseIconButton;
    QDoubleSpinBox* m_currentValueSpinBox;
    QDoubleSpinBox* m_minValueSpinBox;
    QDoubleSpinBox* m_maxValueSpinBox;
    QLineEdit* m_valueSuffixEdit;
    QLineEdit* m_valueBindingPathEdit;
    QLineEdit* m_maxValueBindingPathEdit;
    QComboBox* m_numberFormatComboBox;
    QCheckBox* m_useThousandsSeparatorCheckBox;
    QLineEdit* m_itemsSourcePathEdit;
    QLineEdit* m_itemTemplateEdit;
    QSpinBox* m_tooltipDelaySpinBox;
    QComboBox* m_tooltipPositionComboBox;
    
    // Aba de comportamento
    QWidget* m_behaviorTab;
    QCheckBox* m_isInteractiveCheckBox;
    QCheckBox* m_isDraggableCheckBox;
    QCheckBox* m_isDropTargetCheckBox;
    QLineEdit* m_visibilityConditionEdit;
    QListWidget* m_showInContextsList;
    QListWidget* m_hideInContextsList;
    QCheckBox* m_canReceiveFocusCheckBox;
    QSpinBox* m_tabIndexSpinBox;
    QCheckBox* m_autoHideCheckBox;
    QSpinBox* m_autoHideDelaySpinBox;
    QDoubleSpinBox* m_autoHideOpacitySpinBox;
    QCheckBox* m_useClickSoundCheckBox;
    QLineEdit* m_clickSoundNameEdit;
    QLineEdit* m_clickActionEdit;
    QLineEdit* m_onClickFunctionEdit;
    QLineEdit* m_shortcutKeyEdit;
    QCheckBox* m_useShortcutModifierCheckBox;
    QCheckBox* m_shortcutCtrlCheckBox;
    QCheckBox* m_shortcutAltCheckBox;
    QCheckBox* m_shortcutShiftCheckBox;
    QLineEdit* m_dragTypeEdit;
    QLineEdit* m_dropAcceptTypesEdit;
    QLineEdit* m_onDragFunctionEdit;
    QLineEdit* m_onDropFunctionEdit;
    QComboBox* m_entryAnimationComboBox;
    QComboBox* m_exitAnimationComboBox;
    QSpinBox* m_animationDurationSpinBox;
    QCheckBox* m_showTooltipCheckBox;
    QTextEdit* m_tooltipTextEdit;
    QLineEdit* m_tooltipBindingPathEdit;
    QLineEdit* m_onMouseEnterFunctionEdit;
    QLineEdit* m_onMouseLeaveFunctionEdit;
    QLineEdit* m_onFocusFunctionEdit;
    QLineEdit* m_onBlurFunctionEdit;
    QLineEdit* m_onValueChangeFunctionEdit;
    
    // Aba de propriedades personalizadas
    QWidget* m_customPropertiesTab;
    QTableWidget* m_customPropertiesTable;
    QPushButton* m_addPropertyButton;
    QPushButton* m_removePropertyButton;
    
    void createUI();
    void updateUI();
    void connectSignals();
    
private slots:
    void onPropertyValueChanged();
    void onBrowseImageClicked();
    void onBrowseIconClicked();
    void onAddPropertyClicked();
    void onRemovePropertyClicked();
    void onAddShowContextClicked();
    void onRemoveShowContextClicked();
    void onAddHideContextClicked();
    void onRemoveHideContextClicked();
    void onElementTypeChanged(int index);
};

/**
 * @brief Widget para previsualização de UI
 */
class UIPreviewWidget : public QWidget {
    Q_OBJECT
    
public:
    UIPreviewWidget(QWidget* parent = nullptr);
    
    void setComponent(const UIComponent& component);
    void setLayout(const UILayout& layout);
    void clear();
    
    void setTheme(const UITheme& theme);
    void setDesignResolution(int width, int height);
    
    QImage generatePreviewImage() const;
    
private:
    enum class PreviewType {
        NONE,
        COMPONENT,
        LAYOUT
    };
    
    PreviewType m_previewType;
    UIComponent m_component;
    UILayout m_layout;
    UITheme m_theme;
    int m_designWidth;
    int m_designHeight;
    
    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    
    void createUI();
    void updatePreview();
    void renderComponent(const UIComponent& component);
    void renderLayout(const UILayout& layout);
    
    QGraphicsItem* createElementItem(const UIElement& element);
};

/**
 * @brief Editor de estilo para elementos UI
 */
class StyleEditor : public QWidget {
    Q_OBJECT
    
public:
    StyleEditor(QWidget* parent = nullptr);
    
    void setStyle(const UIStyle& style);
    UIStyle getUpdatedStyle() const;
    
signals:
    void styleChanged();
    
private:
    UIStyle m_style;
    
    // Widgets
    QTabWidget* m_tabWidget;
    
    // Aba de cores
    QWidget* m_colorsTab;
    QPushButton* m_backgroundColorButton;
    QPushButton* m_foregroundColorButton;
    QPushButton* m_borderColorButton;
    QPushButton* m_textColorButton;
    QPushButton* m_highlightColorButton;
    QDoubleSpinBox* m_opacitySpinBox;
    QDoubleSpinBox* m_backgroundOpacitySpinBox;
    
    // Aba de fonte
    QWidget* m_fontTab;
    QComboBox* m_fontFamilyComboBox;
    QSpinBox* m_fontSizeSpinBox;
    QCheckBox* m_fontBoldCheckBox;
    QCheckBox* m_fontItalicCheckBox;
    QSpinBox* m_fontSpacingSpinBox;
    QPushButton* m_fontDialogButton;
    
    // Aba de bordas
    QWidget* m_borderTab;
    QSpinBox* m_borderSizeSpinBox;
    QComboBox* m_borderStyleComboBox;
    QSpinBox* m_cornerRadiusSpinBox;
    
    // Aba de textura
    QWidget* m_textureTab;
    QLineEdit* m_backgroundTextureEdit;
    QPushButton* m_browseTextureButton;
    QLineEdit* m_backgroundImageSliceEdit;
    QComboBox* m_backgroundRepeatComboBox;
    QComboBox* m_backgroundPositionComboBox;
    QSpinBox* m_backgroundOffsetXSpinBox;
    QSpinBox* m_backgroundOffsetYSpinBox;
    
    // Aba de efeitos
    QWidget* m_effectsTab;
    QCheckBox* m_useShadowCheckBox;
    QSpinBox* m_shadowOffsetXSpinBox;
    QSpinBox* m_shadowOffsetYSpinBox;
    QSpinBox* m_shadowBlurSpinBox;
    QPushButton* m_shadowColorButton;
    QCheckBox* m_useGlowCheckBox;
    QSpinBox* m_glowRadiusSpinBox;
    QPushButton* m_glowColorButton;
    
    // Aba de estados
    QWidget* m_statesTab;
    QTabWidget* m_statesTabWidget;
    
    // Sub-aba hover
    QWidget* m_hoverStateTab;
    QPushButton* m_hoverBackgroundColorButton;
    QPushButton* m_hoverForegroundColorButton;
    QPushButton* m_hoverBorderColorButton;
    QPushButton* m_hoverTextColorButton;
    QDoubleSpinBox* m_hoverOpacitySpinBox;
    QLineEdit* m_hoverBackgroundTextureEdit;
    
    // Sub-aba active
    QWidget* m_activeStateTab;
    QPushButton* m_activeBackgroundColorButton;
    QPushButton* m_activeForegroundColorButton;
    QPushButton* m_activeBorderColorButton;
    QPushButton* m_activeTextColorButton;
    QDoubleSpinBox* m_activeOpacitySpinBox;
    QLineEdit* m_activeBackgroundTextureEdit;
    
    // Sub-aba disabled
    QWidget* m_disabledStateTab;
    QPushButton* m_disabledBackgroundColorButton;
    QPushButton* m_disabledForegroundColorButton;
    QPushButton* m_disabledBorderColorButton;
    QPushButton* m_disabledTextColorButton;
    QDoubleSpinBox* m_disabledOpacitySpinBox;
    QLineEdit* m_disabledBackgroundTextureEdit;
    
    // Aba de animações
    QWidget* m_animationsTab;
    QCheckBox* m_useAnimationsCheckBox;
    QSpinBox* m_transitionDurationSpinBox;
    QComboBox* m_transitionEasingComboBox;
    
    void createUI();
    void updateUI();
    void connectSignals();
    
    QColor getColorFromHex(const std::string& hexColor) const;
    std::string getHexFromColor(const QColor& color) const;
    
private slots:
    void onStylePropertyChanged();
    void onBackgroundColorClicked();
    void onForegroundColorClicked();
    void onBorderColorClicked();
    void onTextColorClicked();
    void onHighlightColorClicked();
    void onShadowColorClicked();
    void onGlowColorClicked();
    void onHoverBackgroundColorClicked();
    void onHoverForegroundColorClicked();
    void onHoverBorderColorClicked();
    void onHoverTextColorClicked();
    void onActiveBackgroundColorClicked();
    void onActiveForegroundColorClicked();
    void onActiveBorderColorClicked();
    void onActiveTextColorClicked();
    void onDisabledBackgroundColorClicked();
    void onDisabledForegroundColorClicked();
    void onDisabledBorderColorClicked();
    void onDisabledTextColorClicked();
    void onBrowseTextureClicked();
    void onFontDialogClicked();
};

/**
 * @brief Navegador de recursos visuais
 */
class ResourceBrowser : public QWidget {
    Q_OBJECT
    
public:
    ResourceBrowser(QWidget* parent = nullptr);
    
    void initialize(const std::shared_ptr<UIDesigner>& designer);
    
signals:
    void resourceSelected(const QString& resourcePath, const QString& resourceType);
    
private:
    std::shared_ptr<UIDesigner> m_designer;
    
    QTabWidget* m_tabWidget;
    
    // Aba de texturas
    QWidget* m_texturesTab;
    QListWidget* m_texturesList;
    QLineEdit* m_texturesFilterEdit;
    QPushButton* m_importTextureButton;
    
    // Aba de ícones
    QWidget* m_iconsTab;
    QListWidget* m_iconsList;
    QLineEdit* m_iconsFilterEdit;
    QPushButton* m_importIconButton;
    
    // Aba de fundos
    QWidget* m_backgroundsTab;
    QListWidget* m_backgroundsList;
    QLineEdit* m_backgroundsFilterEdit;
    QPushButton* m_importBackgroundButton;
    
    void createUI();
    void loadResources();
    
private slots:
    void onTextureSelected();
    void onIconSelected();
    void onBackgroundSelected();
    void onImportTextureClicked();
    void onImportIconClicked();
    void onImportBackgroundClicked();
    void onTexturesFilterChanged(const QString& text);
    void onIconsFilterChanged(const QString& text);
    void onBackgroundsFilterChanged(const QString& text);
};

/**
 * @brief Editor de tema
 */
class ThemeEditor : public QWidget {
    Q_OBJECT
    
public:
    ThemeEditor(QWidget* parent = nullptr);
    
    void setTheme(const UITheme& theme);
    UITheme getUpdatedTheme() const;
    void clear();
    
signals:
    void themeChanged();
    
private:
    UITheme m_theme;
    
    // Widgets
    QLineEdit* m_idEdit;
    QLineEdit* m_nameEdit;
    
    // Cores principais
    QPushButton* m_primaryColorButton;
    QPushButton* m_secondaryColorButton;
    QPushButton* m_accentColorButton;
    QPushButton* m_backgroundColorButton;
    QPushButton* m_textColorButton;
    
    // Fonte principal
    QComboBox* m_fontFamilyComboBox;
    QSpinBox* m_defaultFontSizeSpinBox;
    QPushButton* m_fontDialogButton;
    
    // Textura de fundo
    QLineEdit* m_backgroundTextureEdit;
    QPushButton* m_browseTextureButton;
    
    // Estilos de componentes
    QTreeWidget* m_componentStylesTree;
    QPushButton* m_editComponentStyleButton;
    
    // Estilos de elementos
    QTreeWidget* m_elementStylesTree;
    QPushButton* m_editElementStyleButton;
    
    // Metadados
    QLineEdit* m_authorEdit;
    QTextEdit* m_descriptionEdit;
    
    void createUI();
    void updateUI();
    
private slots:
    void onThemePropertyChanged();
    void onPrimaryColorClicked();
    void onSecondaryColorClicked();
    void onAccentColorClicked();
    void onBackgroundColorClicked();
    void onTextColorClicked();
    void onFontDialogClicked();
    void onBrowseTextureClicked();
    void onEditComponentStyleClicked();
    void onEditElementStyleClicked();
};

/**
 * @brief Editor de layout
 */
class LayoutEditor : public QWidget {
    Q_OBJECT
    
public:
    LayoutEditor(QWidget* parent = nullptr);
    
    void setLayout(const UILayout& layout);
    UILayout getUpdatedLayout() const;
    void clear();
    
    void initialize(const std::shared_ptr<UIDesigner>& designer);
    
signals:
    void layoutChanged();
    
private:
    UILayout m_layout;
    std::shared_ptr<UIDesigner> m_designer;
    
    // Widgets
    QLineEdit* m_idEdit;
    QLineEdit* m_nameEdit;
    
    // Tema
    QComboBox* m_themeComboBox;
    
    // Resolução
    QSpinBox* m_designWidthSpinBox;
    QSpinBox* m_designHeightSpinBox;
    QComboBox* m_commonResolutionsComboBox;
    
    // Escalabilidade
    QCheckBox* m_autoScaleCheckBox;
    QComboBox* m_scaleModeComboBox;
    
    // Componentes
    QListWidget* m_componentsList;
    QPushButton* m_addComponentButton;
    QPushButton* m_removeComponentButton;
    QPushButton* m_moveUpButton;
    QPushButton* m_moveDownButton;
    
    // Metadados
    QLineEdit* m_authorEdit;
    QTextEdit* m_descriptionEdit;
    QSpinBox* m_versionSpinBox;
    
    void createUI();
    void updateUI();
    void loadThemes();
    void loadComponents();
    
private slots:
    void onLayoutPropertyChanged();
    void onAddComponentClicked();
    void onRemoveComponentClicked();
    void onMoveUpClicked();
    void onMoveDownClicked();
    void onCommonResolutionSelected(int index);
};

/**
 * @brief Janela principal para o designer de UI
 */
class UIDesignerView : public QMainWindow {
    Q_OBJECT
    
public:
    UIDesignerView(QWidget* parent = nullptr);
    ~UIDesignerView();
    
    void initialize(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
public slots:
    // Menu File
    void onNewComponent();
    void onOpenComponent();
    void onSaveComponent();
    void onSaveComponentAs();
    void onCloseComponent();
    void onImportFromWYT();
    void onExportToWYT();
    
    // Menu Edit
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onSelectAll();
    
    // Menu View
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void onShowGrid(bool show);
    void onSnapToGrid(bool snap);
    void onGridSize();
    
    // Menu Element
    void onAddElement();
    void onGroupElements();
    void onUngroupElements();
    void onBringToFront();
    void onSendToBack();
    void onBringForward();
    void onSendBackward();
    void onAlignLeft();
    void onAlignCenter();
    void onAlignRight();
    void onAlignTop();
    void onAlignMiddle();
    void onAlignBottom();
    void onDistributeHorizontally();
    void onDistributeVertically();
    
    // Menu Theme
    void onNewTheme();
    void onOpenTheme();
    void onSaveTheme();
    void onApplyTheme();
    
    // Menu Layout
    void onNewLayout();
    void onOpenLayout();
    void onSaveLayout();
    void onApplyLayout();
    
    // Menu Tools
    void onGeneratePreview();
    void onTestInGame();
    void onApplyToServer();
    void onCheckCompatibility();
    
    // UI Events
    void onComponentSelected(const QModelIndex& index);
    void onElementTreeSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onElementSelected(const UIElement& element);
    void onPropertyChanged();
    void onStyleChanged();
    void onResourceSelected(const QString& resourcePath, const QString& resourceType);
    void onThemeChanged();
    void onLayoutChanged();
    
    // Canvas Events
    void onElementMoved(const std::string& elementId, const QPointF& newPos);
    void onElementResized(const std::string& elementId, const QRectF& newRect);
    void onSceneClicked();
    
    // Element Creation
    void onCreateElement(UIElementType type);
    
private:
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    std::shared_ptr<UIDesigner> m_designer;
    
    // Dados atuais
    UIComponent m_currentComponent;
    std::string m_currentComponentPath;
    UITheme m_currentTheme;
    std::string m_currentThemePath;
    UILayout m_currentLayout;
    std::string m_currentLayoutPath;
    UIElement m_selectedElement;
    
    // Histórico de operações
    QUndoStack* m_undoStack;
    
    // UI Elements
    QToolBar* m_mainToolBar;
    QToolBar* m_elementToolBar;
    QStatusBar* m_statusBar;
    
    QSplitter* m_mainSplitter;
    QSplitter* m_leftSplitter;
    QSplitter* m_rightSplitter;
    
    // Painel esquerdo
    QTabWidget* m_leftTabWidget;
    ComponentListView* m_componentListView;
    ComponentListModel* m_componentListModel;
    QLineEdit* m_componentFilterEdit;
    
    // Centro
    QTabWidget* m_centerTabWidget;
    UICanvasView* m_canvasView;
    UICanvasScene* m_canvasScene;
    ElementTreeView* m_elementTreeView;
    ElementTreeModel* m_elementTreeModel;
    
    // Painel direito
    QTabWidget* m_rightTabWidget;
    PropertyEditor* m_propertyEditor;
    StyleEditor* m_styleEditor;
    
    // Painéis de encaixe
    QDockWidget* m_previewDock;
    UIPreviewWidget* m_previewWidget;
    
    QDockWidget* m_resourceBrowserDock;
    ResourceBrowser* m_resourceBrowser;
    
    QDockWidget* m_themeEditorDock;
    ThemeEditor* m_themeEditor;
    
    QDockWidget* m_layoutEditorDock;
    LayoutEditor* m_layoutEditor;
    
    // Inicialização da UI
    void createUI();
    void createActions();
    void createMenus();
    void createToolbars();
    void createDockWidgets();
    void connectSignals();
    
    // Utilitários
    void updateTitle();
    void updateStatusBar();
    void showMessage(const QString& message, int timeout = 3000);
    void showError(const QString& title, const QString& message);
    bool confirmDiscardChanges();
    
    // Gerenciamento de componentes
    void loadComponent(const std::string& componentId);
    bool saveComponent();
    bool saveComponentAs(const std::string& path);
    void clearComponent();
    void updateComponentDisplay();
    
    // Gerenciamento de elementos
    void selectElement(const std::string& elementId);
    void updateElementInScene(const UIElement& element);
    
    // Manipulação de elementos
    void addElement(const UIElement& element);
    void removeElement(const std::string& elementId);
    void updateElement(const UIElement& element);
    void moveElement(const std::string& elementId, const std::string& newParentId, int32_t index);
    
    // Gerenciamento de temas
    void loadTheme(const std::string& themeId);
    bool saveTheme();
    bool saveThemeAs(const std::string& path);
    void clearTheme();
    
    // Gerenciamento de layouts
    void loadLayout(const std::string& layoutId);
    bool saveLayout();
    bool saveLayoutAs(const std::string& path);
    void clearLayout();
    
    // Operações de alinhamento
    void alignElements(const std::vector<std::string>& elementIds, const std::string& alignment);
    void distributeElements(const std::vector<std::string>& elementIds, bool horizontally);
    
    // Auxiliares
    std::vector<std::string> getSelectedElementIds() const;
    UIElement createDefaultElement(UIElementType type, const std::string& name, const QPointF& position);
};

}}} // namespace wydstudio::ui::visual

#endif // WYDSTUDIO_UI_DESIGNER_VIEW_H