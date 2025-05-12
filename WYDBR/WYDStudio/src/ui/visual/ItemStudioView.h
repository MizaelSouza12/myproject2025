#ifndef WYDSTUDIO_ITEM_STUDIO_VIEW_H
#define WYDSTUDIO_ITEM_STUDIO_VIEW_H

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
#include <QSortFilterProxyModel>
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
#include <memory>

#include "../ItemStudio.h"
#include "../../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {
namespace visual {

/**
 * @brief Modelo para visualização de itens em grade
 */
class ItemGridModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    ItemGridModel(QObject* parent = nullptr);
    
    void setItems(const std::vector<ItemUIModel>& items);
    ItemUIModel getItem(const QModelIndex& index) const;
    void updateItem(const ItemUIModel& item);
    void addItem(const ItemUIModel& item);
    void removeItem(uint32_t itemId);
    
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    
    QModelIndex findItemById(uint32_t itemId) const;
    
private:
    std::vector<ItemUIModel> m_items;
};

/**
 * @brief Delegado para renderizar itens na visualização de grade
 */
class ItemGridDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    ItemGridDelegate(QObject* parent = nullptr);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, 
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, 
                   const QModelIndex& index) const override;
    
private:
    QPixmap generateItemIcon(const ItemUIModel& item) const;
    void drawItemAttributes(QPainter* painter, const QRect& rect, 
                          const ItemUIModel& item) const;
};

/**
 * @brief Visualização em grade para itens
 */
class ItemGridView : public QListView {
    Q_OBJECT
    
public:
    ItemGridView(QWidget* parent = nullptr);
    
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    
private:
    QPoint m_dragStartPosition;
};

/**
 * @brief Modelo para visualização hierárquica de itens
 */
class ItemTreeModel : public QStandardItemModel {
    Q_OBJECT
    
public:
    ItemTreeModel(QObject* parent = nullptr);
    
    void setItems(const std::vector<ItemUIModel>& items);
    ItemUIModel getItem(const QModelIndex& index) const;
    
private:
    void organizeItemsByCategory(const std::vector<ItemUIModel>& items);
    QStandardItem* createCategoryItem(const std::string& category);
    QStandardItem* createItemNode(const ItemUIModel& item);
    
    std::map<std::string, QStandardItem*> m_categoryItems;
    std::map<QStandardItem*, ItemUIModel> m_itemDataMap;
};

/**
 * @brief Widget para edição de aparência de item
 */
class ItemAppearanceEditor : public QWidget {
    Q_OBJECT
    
public:
    ItemAppearanceEditor(QWidget* parent = nullptr);
    
    void setItem(const ItemUIModel& item);
    void updateItemAppearance(ItemUIModel& item);
    
signals:
    void appearanceChanged();
    
private:
    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    
    QSpinBox* m_modelIdSpinBox;
    QSpinBox* m_textureIdSpinBox;
    QComboBox* m_colorComboBox;
    QCheckBox* m_glowEffectCheckBox;
    
    QSlider* m_rotationSlider;
    QSlider* m_zoomSlider;
    
    void createUI();
    void updatePreview();
    
    ItemUIModel m_currentItem;
};

/**
 * @brief Widget para edição de atributos de item
 */
class ItemAttributesEditor : public QWidget {
    Q_OBJECT
    
public:
    ItemAttributesEditor(QWidget* parent = nullptr);
    
    void setItem(const ItemUIModel& item);
    void updateItemAttributes(ItemUIModel& item);
    
signals:
    void attributesChanged();
    
private:
    QSpinBox* m_levelSpinBox;
    QSpinBox* m_reqLevelSpinBox;
    QSpinBox* m_reqStrSpinBox;
    QSpinBox* m_reqIntSpinBox;
    QSpinBox* m_reqDexSpinBox;
    QSpinBox* m_reqConSpinBox;
    
    QSpinBox* m_attackSpinBox;
    QSpinBox* m_defenseSpinBox;
    QSpinBox* m_magicAttackSpinBox;
    QSpinBox* m_magicDefenseSpinBox;
    QSpinBox* m_hitRateSpinBox;
    QSpinBox* m_dodgeRateSpinBox;
    QSpinBox* m_hpBonusSpinBox;
    QSpinBox* m_mpBonusSpinBox;
    
    QCheckBox* m_usableByTKCheckBox;
    QCheckBox* m_usableByFMCheckBox;
    QCheckBox* m_usableByBMCheckBox;
    QCheckBox* m_usableByHTCheckBox;
    
    QSpinBox* m_priceSpinBox;
    QComboBox* m_rarityComboBox;
    
    QCheckBox* m_tradeableCheckBox;
    QCheckBox* m_storableCheckBox;
    QCheckBox* m_sellableCheckBox;
    QCheckBox* m_droppableCheckBox;
    QCheckBox* m_questItemCheckBox;
    
    QSpinBox* m_maxSocketsSpinBox;
    QSpinBox* m_maxUpgradeLevelSpinBox;
    
    void createUI();
};

/**
 * @brief Widget principal para visualização de ItemStudio
 */
class ItemStudioView : public QWidget {
    Q_OBJECT
    
public:
    ItemStudioView(QWidget* parent = nullptr);
    ~ItemStudioView();
    
    void initialize(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
private slots:
    void onItemSelected(const QModelIndex& index);
    void onCreateNewItem();
    void onDuplicateItem();
    void onDeleteItem();
    void onSaveItem();
    void onRefreshItems();
    void onSearchTextChanged(const QString& text);
    void onFilterChanged(int index);
    void onViewModeChanged(int index);
    void onApplyToServer();
    void onImportItems();
    void onExportItems();
    void onBalanceItem();
    
private:
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    std::shared_ptr<ItemStudio> m_itemStudio;
    
    // Widgets
    QSplitter* m_mainSplitter;
    QTabWidget* m_viewTabs;
    
    // Item list view
    ItemGridView* m_gridView;
    QTreeView* m_treeView;
    
    // Models
    ItemGridModel* m_gridModel;
    ItemTreeModel* m_treeModel;
    QSortFilterProxyModel* m_proxyModel;
    
    // Toolbar and controls
    QToolBar* m_toolbar;
    QLineEdit* m_searchEdit;
    QComboBox* m_filterComboBox;
    QComboBox* m_viewModeComboBox;
    
    // Editor widgets
    QTabWidget* m_editorTabs;
    QWidget* m_generalTab;
    ItemAppearanceEditor* m_appearanceTab;
    ItemAttributesEditor* m_attributesTab;
    QWidget* m_specialEffectsTab;
    
    // General tab widgets
    QLineEdit* m_nameEdit;
    QComboBox* m_categoryComboBox;
    QComboBox* m_typeComboBox;
    QTextEdit* m_descriptionEdit;
    QSpinBox* m_idSpinBox;
    
    // Current item
    std::optional<ItemUIModel> m_currentItem;
    
    // UI setup methods
    void createUI();
    void setupToolbar();
    void setupItemViews();
    void setupEditorPanel();
    void setupGeneralTab();
    void setupSpecialEffectsTab();
    
    // Data methods
    void loadItems();
    void updateItemDetails();
    void clearItemDetails();
    void collectItemDataFromUI(ItemUIModel& item);
    
    // Helper methods
    void showMessage(const QString& title, const QString& message, QMessageBox::Icon icon = QMessageBox::Information);
    void updateItemInModel(const ItemUIModel& item);
    void selectItemById(uint32_t itemId);
};

}}} // namespace wydstudio::ui::visual

#endif // WYDSTUDIO_ITEM_STUDIO_VIEW_H