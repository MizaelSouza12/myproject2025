#include "ItemStudioView.h"
#include <QApplication>
#include <QStyle>
#include <QToolButton>
#include <QPalette>
#include <QPixmap>
#include <QImage>
#include <QMimeData>
#include <QDrag>
#include <QScrollBar>
#include <QDateTime>
#include <QDir>
#include <QScreen>
#include <algorithm>
#include <random>

namespace wydstudio {
namespace ui {
namespace visual {

//--------------------------------------------------------------------------------------------
// ItemGridModel Implementation
//--------------------------------------------------------------------------------------------

ItemGridModel::ItemGridModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

void ItemGridModel::setItems(const std::vector<ItemUIModel>& items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}

ItemUIModel ItemGridModel::getItem(const QModelIndex& index) const
{
    if (index.isValid() && index.row() < static_cast<int>(m_items.size())) {
        return m_items[index.row()];
    }
    return ItemUIModel{};
}

void ItemGridModel::updateItem(const ItemUIModel& item)
{
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == item.id) {
            m_items[i] = item;
            QModelIndex idx = index(static_cast<int>(i), 0);
            emit dataChanged(idx, idx);
            return;
        }
    }
}

void ItemGridModel::addItem(const ItemUIModel& item)
{
    beginInsertRows(QModelIndex(), static_cast<int>(m_items.size()), static_cast<int>(m_items.size()));
    m_items.push_back(item);
    endInsertRows();
}

void ItemGridModel::removeItem(uint32_t itemId)
{
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == itemId) {
            beginRemoveRows(QModelIndex(), static_cast<int>(i), static_cast<int>(i));
            m_items.erase(m_items.begin() + i);
            endRemoveRows();
            return;
        }
    }
}

int ItemGridModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_items.size());
}

QVariant ItemGridModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_items.size())) {
        return QVariant();
    }

    const ItemUIModel& item = m_items[index.row()];

    switch (role) {
        case Qt::DisplayRole:
            return QString::fromStdString(item.name);
        case Qt::UserRole:
            return QVariant::fromValue(item);
        case Qt::ToolTipRole: {
            QString tooltip = QString("<b>%1</b> (ID: %2)<br>")
                .arg(QString::fromStdString(item.name))
                .arg(item.id);
            
            if (!item.description.empty()) {
                tooltip += QString("<i>%1</i><br>").arg(QString::fromStdString(item.description));
            }
            
            tooltip += QString("<br>Level: %1 | Req Level: %2")
                .arg(item.level)
                .arg(item.requirements.level);
                
            if (item.stats.attack > 0) {
                tooltip += QString("<br>Attack: +%1").arg(item.stats.attack);
            }
            if (item.stats.defense > 0) {
                tooltip += QString("<br>Defense: +%1").arg(item.stats.defense);
            }
            
            return tooltip;
        }
        default:
            return QVariant();
    }
}

Qt::ItemFlags ItemGridModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

bool ItemGridModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_items.size())) {
        return false;
    }

    if (role == Qt::UserRole) {
        if (value.canConvert<ItemUIModel>()) {
            m_items[index.row()] = value.value<ItemUIModel>();
            emit dataChanged(index, index);
            return true;
        }
    }
    
    return false;
}

QModelIndex ItemGridModel::findItemById(uint32_t itemId) const
{
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == itemId) {
            return index(static_cast<int>(i), 0);
        }
    }
    return QModelIndex();
}

//--------------------------------------------------------------------------------------------
// ItemGridDelegate Implementation
//--------------------------------------------------------------------------------------------

ItemGridDelegate::ItemGridDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void ItemGridDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, 
                            const QModelIndex& index) const
{
    if (!index.isValid()) {
        return;
    }

    const ItemUIModel item = index.data(Qt::UserRole).value<ItemUIModel>();
    
    // Draw background and selection
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    
    const QWidget* widget = option.widget;
    QStyle* style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
    
    // Calculate item rect
    QRect contentRect = opt.rect.adjusted(4, 4, -4, -4);
    
    // Draw item icon
    QRect iconRect = contentRect;
    iconRect.setHeight(contentRect.width()); // Square for icon
    if (iconRect.height() > contentRect.height() - 20) {
        iconRect.setHeight(contentRect.height() - 20);
    }
    
    QPixmap icon = generateItemIcon(item);
    if (!icon.isNull()) {
        painter->drawPixmap(iconRect, icon);
    } else {
        // Draw placeholder for missing icon
        painter->save();
        painter->setPen(Qt::gray);
        painter->setBrush(Qt::lightGray);
        painter->drawRect(iconRect);
        painter->drawText(iconRect, Qt::AlignCenter, "No Icon");
        painter->restore();
    }
    
    // Draw item name
    QRect textRect = contentRect;
    textRect.setTop(iconRect.bottom() + 2);
    
    painter->save();
    
    // Determine text color based on item rarity
    QColor textColor;
    switch (item.rarity) {
        case ItemRarity::COMMON:
            textColor = QColor(255, 255, 255); // White
            break;
        case ItemRarity::UNCOMMON:
            textColor = QColor(30, 255, 0);    // Green
            break;
        case ItemRarity::RARE:
            textColor = QColor(0, 112, 221);   // Blue
            break;
        case ItemRarity::EPIC:
            textColor = QColor(163, 53, 238);  // Purple
            break;
        case ItemRarity::LEGENDARY:
            textColor = QColor(255, 128, 0);   // Orange
            break;
        case ItemRarity::MYTHIC:
            textColor = QColor(255, 0, 0);     // Red
            break;
        default:
            textColor = QColor(255, 255, 255); // White
    }
    
    // Draw item name with appropriate color
    painter->setPen(textColor);
    painter->setFont(QFont(painter->font().family(), 8, item.rarity > ItemRarity::COMMON ? QFont::Bold : QFont::Normal));
    
    QString displayName = QString::fromStdString(item.name);
    if (displayName.length() > 12) {
        displayName = displayName.left(10) + "...";
    }
    
    painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, displayName);
    
    // Draw item level if non-zero
    if (item.level > 0) {
        QRect levelRect = iconRect;
        levelRect.setLeft(levelRect.right() - 20);
        levelRect.setTop(levelRect.top());
        levelRect.setHeight(16);
        levelRect.setWidth(20);
        
        // Draw level background
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 0, 0, 180));
        painter->drawRect(levelRect);
        
        // Draw level text
        painter->setPen(Qt::white);
        painter->setFont(QFont(painter->font().family(), 7, QFont::Bold));
        painter->drawText(levelRect, Qt::AlignCenter, QString::number(item.level));
    }
    
    // Draw item attributes (bonus stats, sockets, etc.)
    drawItemAttributes(painter, iconRect, item);
    
    painter->restore();
}

QSize ItemGridDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(80, 100);
}

QPixmap ItemGridDelegate::generateItemIcon(const ItemUIModel& item) const
{
    // In a real implementation, we would load the actual item icon from resources
    // For this example, we'll generate a placeholder based on item attributes
    
    const int size = 64;
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Determine base color by item type
    QColor baseColor;
    switch (item.type) {
        case ItemType::WEAPON:
            baseColor = QColor(200, 30, 30);
            break;
        case ItemType::ARMOR:
            baseColor = QColor(30, 70, 200);
            break;
        case ItemType::ACCESSORY:
            baseColor = QColor(200, 180, 30);
            break;
        case ItemType::POTION:
            baseColor = QColor(200, 30, 200);
            break;
        case ItemType::MATERIAL:
            baseColor = QColor(100, 100, 100);
            break;
        case ItemType::QUEST:
            baseColor = QColor(30, 180, 130);
            break;
        default:
            baseColor = QColor(150, 150, 150);
    }
    
    // Apply rarity modifiers
    switch (item.rarity) {
        case ItemRarity::COMMON:
            // No adjustment for common
            break;
        case ItemRarity::UNCOMMON:
            baseColor = baseColor.lighter(120);
            break;
        case ItemRarity::RARE:
            baseColor = baseColor.lighter(135);
            break;
        case ItemRarity::EPIC:
            baseColor = baseColor.lighter(150);
            break;
        case ItemRarity::LEGENDARY:
            baseColor = baseColor.lighter(170);
            break;
        case ItemRarity::MYTHIC:
            baseColor = baseColor.lighter(190);
            break;
    }
    
    // Draw basic shape based on item type
    painter.setBrush(QBrush(baseColor));
    
    // Draw outline with higher quality
    painter.setPen(QPen(baseColor.darker(150), 2));
    
    switch (item.type) {
        case ItemType::WEAPON:
            // Draw a sword-like shape
            {
                QPolygonF sword;
                sword << QPointF(32, 8) << QPointF(40, 16) << QPointF(48, 24) 
                      << QPointF(48, 56) << QPointF(40, 56) << QPointF(32, 48)
                      << QPointF(24, 56) << QPointF(16, 56) << QPointF(16, 24)
                      << QPointF(24, 16);
                painter.drawPolygon(sword);
                
                // Draw handle
                painter.setBrush(QBrush(QColor(139, 69, 19)));
                painter.setPen(QPen(QColor(101, 67, 33), 1));
                painter.drawRect(28, 8, 8, 16);
                
                // Draw blade shine
                painter.setPen(QPen(baseColor.lighter(200), 1, Qt::SolidLine, Qt::RoundCap));
                painter.drawLine(24, 24, 40, 48);
            }
            break;
            
        case ItemType::ARMOR:
            // Draw armor-like shape
            {
                // Body of armor
                QPolygonF armor;
                armor << QPointF(16, 16) << QPointF(48, 16) << QPointF(52, 24) 
                      << QPointF(52, 48) << QPointF(40, 56) << QPointF(24, 56)
                      << QPointF(12, 48) << QPointF(12, 24);
                painter.drawPolygon(armor);
                
                // Collar area
                painter.setBrush(baseColor.darker(120));
                QPolygonF collar;
                collar << QPointF(24, 16) << QPointF(40, 16) << QPointF(36, 28) << QPointF(28, 28);
                painter.drawPolygon(collar);
                
                // Details
                painter.setPen(QPen(baseColor.lighter(150), 1));
                painter.drawLine(32, 28, 32, 48);
                painter.drawLine(24, 36, 40, 36);
            }
            break;
            
        case ItemType::ACCESSORY:
            // Draw a ring or amulet
            {
                // For a ring
                painter.drawEllipse(12, 12, 40, 40);
                painter.setBrush(QBrush(Qt::transparent));
                painter.setPen(QPen(baseColor.lighter(200), 8));
                painter.drawEllipse(20, 20, 24, 24);
                
                // Add a gem based on rarity
                QColor gemColor;
                switch (item.rarity) {
                    case ItemRarity::COMMON:
                        gemColor = QColor(200, 200, 200);
                        break;
                    case ItemRarity::UNCOMMON:
                        gemColor = QColor(30, 200, 30);
                        break;
                    case ItemRarity::RARE:
                        gemColor = QColor(30, 30, 200);
                        break;
                    case ItemRarity::EPIC:
                        gemColor = QColor(128, 0, 128);
                        break;
                    case ItemRarity::LEGENDARY:
                        gemColor = QColor(255, 140, 0);
                        break;
                    case ItemRarity::MYTHIC:
                        gemColor = QColor(255, 0, 0);
                        break;
                }
                
                if (item.rarity > ItemRarity::COMMON) {
                    painter.setBrush(QBrush(gemColor));
                    painter.setPen(QPen(gemColor.darker(150), 1));
                    painter.drawEllipse(28, 20, 8, 8);
                }
            }
            break;
            
        case ItemType::POTION:
            // Draw a potion bottle
            {
                // Bottle
                QPolygonF bottle;
                bottle << QPointF(24, 12) << QPointF(40, 12) << QPointF(40, 20) 
                       << QPointF(48, 28) << QPointF(48, 52) << QPointF(16, 52)
                       << QPointF(16, 28) << QPointF(24, 20);
                painter.drawPolygon(bottle);
                
                // Neck
                painter.drawRect(24, 8, 16, 4);
                
                // Cork
                painter.setBrush(QBrush(QColor(139, 69, 19)));
                painter.setPen(QPen(QColor(101, 67, 33), 1));
                painter.drawRect(26, 4, 12, 6);
                
                // Liquid
                QColor liquidColor = baseColor.lighter(130);
                liquidColor.setAlpha(180);
                painter.setBrush(QBrush(liquidColor));
                painter.setPen(Qt::NoPen);
                QPolygonF liquid;
                liquid << QPointF(17, 36) << QPointF(47, 36) << QPointF(47, 51) << QPointF(17, 51);
                painter.drawPolygon(liquid);
                
                // Shine
                painter.setPen(QPen(Qt::white, 1, Qt::SolidLine, Qt::RoundCap));
                painter.setOpacity(0.6);
                painter.drawLine(20, 28, 20, 44);
            }
            break;
            
        case ItemType::MATERIAL:
            // Draw crafting material
            {
                // Base shape
                painter.drawRect(12, 20, 40, 24);
                
                // Texture pattern
                painter.setPen(QPen(baseColor.darker(130), 1, Qt::SolidLine));
                for (int i = 16; i < 50; i += 8) {
                    painter.drawLine(i, 20, i, 44);
                }
                for (int i = 24; i < 45; i += 8) {
                    painter.drawLine(12, i, 52, i);
                }
            }
            break;
            
        case ItemType::QUEST:
            // Draw quest item (scroll or document)
            {
                // Parchment
                painter.setBrush(QBrush(QColor(255, 250, 220)));
                painter.setPen(QPen(QColor(200, 180, 150), 1));
                painter.drawRect(16, 12, 32, 40);
                
                // Text lines
                painter.setPen(QPen(QColor(100, 80, 60), 1, Qt::SolidLine));
                for (int i = 18; i < 50; i += 6) {
                    painter.drawLine(20, i, 44, i);
                }
                
                // Seal or marking
                painter.setBrush(QBrush(baseColor));
                painter.setPen(QPen(baseColor.darker(150), 1));
                painter.drawEllipse(36, 36, 8, 8);
            }
            break;
            
        default:
            // Generic item shape
            painter.drawRoundedRect(12, 12, 40, 40, 8, 8);
            
            // Some generic details
            painter.setPen(QPen(baseColor.lighter(130), 2));
            painter.drawLine(20, 20, 44, 20);
            painter.drawLine(20, 32, 44, 32);
            painter.drawLine(20, 44, 44, 44);
    }
    
    // Add glow effect for higher rarity items
    if (item.rarity >= ItemRarity::EPIC) {
        painter.setOpacity(0.3);
        painter.setPen(Qt::NoPen);
        
        QRadialGradient gradient(QPointF(32, 32), 40);
        QColor glowColor;
        
        switch (item.rarity) {
            case ItemRarity::EPIC:
                glowColor = QColor(163, 53, 238);
                break;
            case ItemRarity::LEGENDARY:
                glowColor = QColor(255, 128, 0);
                break;
            case ItemRarity::MYTHIC:
                glowColor = QColor(255, 0, 0);
                break;
            default:
                glowColor = QColor(255, 255, 255);
        }
        
        gradient.setColorAt(0, glowColor);
        gradient.setColorAt(1, Qt::transparent);
        painter.setBrush(gradient);
        painter.drawEllipse(0, 0, size, size);
    }
    
    return pixmap;
}

void ItemGridDelegate::drawItemAttributes(QPainter* painter, const QRect& rect, const ItemUIModel& item) const
{
    // Add visual indicators for special attributes

    // Socket indicator
    if (item.sockets > 0) {
        QRect socketRect = QRect(rect.right() - 12, rect.bottom() - 12, 10, 10);
        painter->save();
        painter->setPen(QPen(QColor(100, 100, 100), 1));
        painter->setBrush(QBrush(QColor(50, 50, 50, 200)));
        painter->drawEllipse(socketRect);
        if (item.sockets > 1) {
            painter->setPen(Qt::white);
            painter->setFont(QFont(painter->font().family(), 6, QFont::Bold));
            painter->drawText(socketRect, Qt::AlignCenter, QString::number(item.sockets));
        }
        painter->restore();
    }
    
    // Enhancement level indicator (+1, +2, etc.)
    if (item.enhancementLevel > 0) {
        QRect enhRect = QRect(rect.left() + 2, rect.top() + 2, 20, 15);
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(0, 0, 0, 180)));
        painter->drawRect(enhRect);
        
        painter->setPen(Qt::white);
        painter->setFont(QFont(painter->font().family(), 7, QFont::Bold));
        painter->drawText(enhRect, Qt::AlignCenter, QString("+%1").arg(item.enhancementLevel));
        painter->restore();
    }
    
    // Bound/untradeable indicator
    if (!item.flags.tradeable) {
        QRect boundRect = QRect(rect.left() + rect.width()/2 - 15, rect.top() + rect.height()/2 - 10, 30, 20);
        painter->save();
        painter->setPen(QPen(QColor(255, 0, 0, 200), 2));
        painter->setBrush(QBrush(QColor(0, 0, 0, 120)));
        painter->setOpacity(0.7);
        painter->drawRoundedRect(boundRect, 3, 3);
        
        painter->setPen(QColor(255, 0, 0, 220));
        painter->setFont(QFont(painter->font().family(), 7, QFont::Bold));
        painter->drawText(boundRect, Qt::AlignCenter, "BOUND");
        painter->restore();
    }
    
    // Quest item marker
    if (item.flags.questItem) {
        QRect questRect = QRect(rect.left() + 2, rect.bottom() - 12, 10, 10);
        painter->save();
        painter->setPen(QPen(QColor(255, 215, 0), 1));
        painter->setBrush(QBrush(QColor(255, 215, 0, 150)));
        painter->drawRect(questRect);
        painter->restore();
    }
}

//--------------------------------------------------------------------------------------------
// ItemGridView Implementation
//--------------------------------------------------------------------------------------------

ItemGridView::ItemGridView(QWidget* parent)
    : QListView(parent)
{
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Static);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setUniformItemSizes(true);
    setItemDelegate(new ItemGridDelegate(this));
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    
    setSpacing(10);
    setIconSize(QSize(64, 64));
    setGridSize(QSize(80, 100));
}

void ItemGridView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }
    QListView::mousePressEvent(event);
}

void ItemGridView::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }
    
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }
    
    QModelIndex index = indexAt(m_dragStartPosition);
    if (!index.isValid()) {
        return;
    }
    
    // Get the item data
    ItemUIModel item = index.data(Qt::UserRole).value<ItemUIModel>();
    
    // Create a drag object
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    
    // Store the item ID in the mime data
    mimeData->setText(QString::number(item.id));
    drag->setMimeData(mimeData);
    
    // Create a pixmap for drag visual
    ItemGridDelegate delegate;
    QStyleOptionViewItem option;
    option.rect = visualRect(index);
    
    QPixmap pixmap(option.rect.size());
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    delegate.paint(&painter, option, index);
    painter.end();
    
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    
    // Execute the drag operation
    drag->exec(Qt::CopyAction | Qt::MoveAction);
}

void ItemGridView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void ItemGridView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void ItemGridView::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasText()) {
        QString idStr = event->mimeData()->text();
        bool ok;
        uint32_t itemId = idStr.toUInt(&ok);
        
        if (ok) {
            QModelIndex dropIndex = indexAt(event->pos());
            
            // This is where we'd implement logic to reorder items
            // For now, we'll just accept the action
            event->acceptProposedAction();
        }
    }
}

//--------------------------------------------------------------------------------------------
// ItemTreeModel Implementation
//--------------------------------------------------------------------------------------------

ItemTreeModel::ItemTreeModel(QObject* parent)
    : QStandardItemModel(parent)
{
    // Setup header
    setHorizontalHeaderLabels(QStringList() << "Name" << "ID" << "Level" << "Type");
}

void ItemTreeModel::setItems(const std::vector<ItemUIModel>& items)
{
    clear();
    setHorizontalHeaderLabels(QStringList() << "Name" << "ID" << "Level" << "Type");
    
    m_categoryItems.clear();
    m_itemDataMap.clear();
    
    organizeItemsByCategory(items);
}

ItemUIModel ItemTreeModel::getItem(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return ItemUIModel{};
    }
    
    QStandardItem* item = itemFromIndex(index);
    if (!item) {
        return ItemUIModel{};
    }
    
    auto it = m_itemDataMap.find(item);
    if (it != m_itemDataMap.end()) {
        return it->second;
    }
    
    return ItemUIModel{};
}

void ItemTreeModel::organizeItemsByCategory(const std::vector<ItemUIModel>& items)
{
    // Group items by category
    std::map<std::string, std::vector<ItemUIModel>> categories;
    
    for (const auto& item : items) {
        categories[item.category].push_back(item);
    }
    
    // Create category items
    for (const auto& category : categories) {
        QStandardItem* categoryItem = createCategoryItem(category.first);
        
        // Add all items in this category
        for (const auto& item : category.second) {
            QStandardItem* itemNode = createItemNode(item);
            categoryItem->appendRow(itemNode);
        }
    }
}

QStandardItem* ItemTreeModel::createCategoryItem(const std::string& category)
{
    QString categoryName = QString::fromStdString(category.empty() ? "Uncategorized" : category);
    
    QStandardItem* categoryItem = new QStandardItem(categoryName);
    categoryItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    categoryItem->setData(QVariant::fromValue(categoryName), Qt::DisplayRole);
    
    appendRow(categoryItem);
    m_categoryItems[category] = categoryItem;
    
    return categoryItem;
}

QStandardItem* ItemTreeModel::createItemNode(const ItemUIModel& item)
{
    QStandardItem* nameItem = new QStandardItem(QString::fromStdString(item.name));
    QStandardItem* idItem = new QStandardItem(QString::number(item.id));
    QStandardItem* levelItem = new QStandardItem(QString::number(item.level));
    QStandardItem* typeItem = new QStandardItem(QString::fromStdString(itemTypeToString(item.type)));
    
    // Store the full item data
    m_itemDataMap[nameItem] = item;
    
    // Set display icon
    ItemGridDelegate delegate;
    QPixmap icon = delegate.generateItemIcon(item);
    
    if (!icon.isNull()) {
        nameItem->setData(icon, Qt::DecorationRole);
    }
    
    // Setup flags
    nameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    idItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    levelItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    typeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    
    QList<QStandardItem*> rowItems;
    rowItems << nameItem << idItem << levelItem << typeItem;
    
    return nameItem;
}

//--------------------------------------------------------------------------------------------
// ItemAppearanceEditor Implementation
//--------------------------------------------------------------------------------------------

ItemAppearanceEditor::ItemAppearanceEditor(QWidget* parent)
    : QWidget(parent), m_scene(nullptr), m_view(nullptr)
{
    createUI();
}

void ItemAppearanceEditor::setItem(const ItemUIModel& item)
{
    m_currentItem = item;
    
    // Update UI controls with item appearance data
    m_modelIdSpinBox->setValue(item.appearance.modelId);
    m_textureIdSpinBox->setValue(item.appearance.textureId);
    
    // Update color dropdown
    int colorIndex = 0;
    if (!item.appearance.colorPalette.empty()) {
        QString colorName = QString::fromStdString(item.appearance.colorPalette);
        colorIndex = m_colorComboBox->findText(colorName);
        if (colorIndex < 0) colorIndex = 0;
    }
    m_colorComboBox->setCurrentIndex(colorIndex);
    
    // Update glow effect
    m_glowEffectCheckBox->setChecked(item.appearance.hasGlowEffect);
    
    // Reset view transformations
    m_rotationSlider->setValue(0);
    m_zoomSlider->setValue(50);
    
    // Update 3D preview
    updatePreview();
}

void ItemAppearanceEditor::updateItemAppearance(ItemUIModel& item)
{
    // Update the item with the current appearance settings
    item.appearance.modelId = m_modelIdSpinBox->value();
    item.appearance.textureId = m_textureIdSpinBox->value();
    item.appearance.colorPalette = m_colorComboBox->currentText().toStdString();
    item.appearance.hasGlowEffect = m_glowEffectCheckBox->isChecked();
}

void ItemAppearanceEditor::createUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create 3D preview
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);
    m_view->setMinimumSize(300, 300);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform);
    m_view->setBackgroundBrush(QBrush(QColor(40, 40, 40)));
    
    mainLayout->addWidget(m_view);
    
    // Create control panel
    QGridLayout* controlLayout = new QGridLayout();
    
    // Model ID
    controlLayout->addWidget(new QLabel("Model ID:"), 0, 0);
    m_modelIdSpinBox = new QSpinBox();
    m_modelIdSpinBox->setRange(0, 9999);
    m_modelIdSpinBox->setSingleStep(1);
    connect(m_modelIdSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &ItemAppearanceEditor::updatePreview);
    controlLayout->addWidget(m_modelIdSpinBox, 0, 1);
    
    // Texture ID
    controlLayout->addWidget(new QLabel("Texture ID:"), 1, 0);
    m_textureIdSpinBox = new QSpinBox();
    m_textureIdSpinBox->setRange(0, 9999);
    m_textureIdSpinBox->setSingleStep(1);
    connect(m_textureIdSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &ItemAppearanceEditor::updatePreview);
    controlLayout->addWidget(m_textureIdSpinBox, 1, 1);
    
    // Color palette
    controlLayout->addWidget(new QLabel("Color:"), 2, 0);
    m_colorComboBox = new QComboBox();
    m_colorComboBox->addItems(QStringList() << "Default" << "Red" << "Blue" << "Green" << "Yellow" << "Purple" << "Black" << "White" << "Gold" << "Silver");
    connect(m_colorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ItemAppearanceEditor::updatePreview);
    controlLayout->addWidget(m_colorComboBox, 2, 1);
    
    // Glow effect
    controlLayout->addWidget(new QLabel("Glow Effect:"), 3, 0);
    m_glowEffectCheckBox = new QCheckBox();
    connect(m_glowEffectCheckBox, &QCheckBox::toggled, 
            this, &ItemAppearanceEditor::updatePreview);
    controlLayout->addWidget(m_glowEffectCheckBox, 3, 1);
    
    // View controls
    QHBoxLayout* viewControlLayout = new QHBoxLayout();
    
    // Rotation
    QVBoxLayout* rotationLayout = new QVBoxLayout();
    rotationLayout->addWidget(new QLabel("Rotation:"));
    m_rotationSlider = new QSlider(Qt::Horizontal);
    m_rotationSlider->setRange(0, 360);
    m_rotationSlider->setValue(0);
    connect(m_rotationSlider, &QSlider::valueChanged, 
            this, &ItemAppearanceEditor::updatePreview);
    rotationLayout->addWidget(m_rotationSlider);
    viewControlLayout->addLayout(rotationLayout);
    
    // Zoom
    QVBoxLayout* zoomLayout = new QVBoxLayout();
    zoomLayout->addWidget(new QLabel("Zoom:"));
    m_zoomSlider = new QSlider(Qt::Horizontal);
    m_zoomSlider->setRange(10, 100);
    m_zoomSlider->setValue(50);
    connect(m_zoomSlider, &QSlider::valueChanged, 
            this, &ItemAppearanceEditor::updatePreview);
    zoomLayout->addWidget(m_zoomSlider);
    viewControlLayout->addLayout(zoomLayout);
    
    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(viewControlLayout);
    
    setLayout(mainLayout);
}

void ItemAppearanceEditor::updatePreview()
{
    m_scene->clear();
    
    // For a real implementation, we would load the 3D model and render it
    // For this example, we'll create a simple preview
    
    // Create a placeholder for the item preview
    QPixmap preview(300, 300);
    preview.fill(Qt::transparent);
    
    QPainter painter(&preview);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Get the current settings
    int modelId = m_modelIdSpinBox->value();
    int textureId = m_textureIdSpinBox->value();
    QString colorName = m_colorComboBox->currentText();
    bool hasGlow = m_glowEffectCheckBox->isChecked();
    
    // Determine color
    QColor itemColor;
    if (colorName == "Default") {
        itemColor = QColor(200, 200, 200);
    } else if (colorName == "Red") {
        itemColor = QColor(220, 50, 50);
    } else if (colorName == "Blue") {
        itemColor = QColor(50, 50, 220);
    } else if (colorName == "Green") {
        itemColor = QColor(50, 220, 50);
    } else if (colorName == "Yellow") {
        itemColor = QColor(220, 220, 50);
    } else if (colorName == "Purple") {
        itemColor = QColor(180, 50, 220);
    } else if (colorName == "Black") {
        itemColor = QColor(40, 40, 40);
    } else if (colorName == "White") {
        itemColor = QColor(240, 240, 240);
    } else if (colorName == "Gold") {
        itemColor = QColor(255, 215, 0);
    } else if (colorName == "Silver") {
        itemColor = QColor(192, 192, 192);
    }
    
    // Draw based on model ID (just a placeholder visualization)
    painter.translate(150, 150);
    
    // Apply rotation
    painter.rotate(m_rotationSlider->value());
    
    // Apply zoom
    float zoom = m_zoomSlider->value() / 50.0f;
    painter.scale(zoom, zoom);
    
    // Draw based on the item type
    switch (m_currentItem.type) {
        case ItemType::WEAPON:
            // Draw a sword shape
            {
                QPolygonF sword;
                sword << QPointF(0, -100) << QPointF(20, -80) << QPointF(20, 60) 
                      << QPointF(0, 100) << QPointF(-20, 60) << QPointF(-20, -80);
                
                // Draw blade
                painter.setBrush(QBrush(itemColor));
                painter.setPen(QPen(itemColor.darker(150), 2));
                painter.drawPolygon(sword);
                
                // Draw handle
                painter.setBrush(QBrush(QColor(139, 69, 19)));
                painter.setPen(QPen(QColor(101, 67, 33), 1));
                painter.drawRect(QRect(-15, -130, 30, 40));
                
                // Draw guard
                painter.setBrush(QBrush(QColor(192, 192, 192)));
                painter.setPen(QPen(QColor(128, 128, 128), 1));
                painter.drawRect(QRect(-40, -95, 80, 15));
                
                // Add texture variation based on texture ID
                if (textureId > 0) {
                    painter.setPen(QPen(itemColor.lighter(), 1, Qt::SolidLine, Qt::RoundCap));
                    int lines = textureId % 5 + 1;
                    for (int i = 0; i < lines; i++) {
                        painter.drawLine(-18, -70 + i*30, 18, -70 + i*30);
                    }
                }
            }
            break;
            
        case ItemType::ARMOR:
            // Draw armor shape
            {
                // Body
                QPolygonF armor;
                armor << QPointF(-60, -90) << QPointF(60, -90) << QPointF(80, -40) 
                      << QPointF(80, 60) << QPointF(40, 100) << QPointF(-40, 100)
                      << QPointF(-80, 60) << QPointF(-80, -40);
                
                painter.setBrush(QBrush(itemColor));
                painter.setPen(QPen(itemColor.darker(150), 3));
                painter.drawPolygon(armor);
                
                // Collar area
                painter.setBrush(QBrush(itemColor.darker(120)));
                QPolygonF collar;
                collar << QPointF(-40, -90) << QPointF(40, -90) << QPointF(30, -50) << QPointF(-30, -50);
                painter.drawPolygon(collar);
                
                // Details based on texture ID
                painter.setPen(QPen(itemColor.lighter(130), 2));
                painter.drawLine(0, -50, 0, 60);
                
                if (textureId % 2 == 0) {
                    // Horizontal lines
                    painter.drawLine(-60, 0, 60, 0);
                    painter.drawLine(-40, 50, 40, 50);
                } else {
                    // Diagonal pattern
                    painter.drawLine(-60, -20, 60, 20);
                    painter.drawLine(-60, 20, 60, -20);
                }
                
                // Emblem based on model ID
                QColor emblemColor = itemColor.lighter(150);
                painter.setBrush(QBrush(emblemColor));
                painter.setPen(QPen(emblemColor.darker(120), 1));
                
                switch (modelId % 5) {
                    case 0:
                        painter.drawEllipse(QPoint(0, 20), 20, 20);
                        break;
                    case 1:
                        {
                            QPolygonF star;
                            for (int i = 0; i < 5; i++) {
                                double angle = i * 4 * M_PI / 5;
                                star << QPointF(cos(angle) * 20, sin(angle) * 20 + 20);
                                angle += 2 * M_PI / 5;
                                star << QPointF(cos(angle) * 10, sin(angle) * 10 + 20);
                            }
                            painter.drawPolygon(star);
                        }
                        break;
                    case 2:
                        painter.drawRect(-15, 5, 30, 30);
                        break;
                    case 3:
                        {
                            QPolygonF diamond;
                            diamond << QPointF(0, 0) << QPointF(20, 20) << QPointF(0, 40) << QPointF(-20, 20);
                            painter.drawPolygon(diamond);
                        }
                        break;
                    case 4:
                        {
                            QPolygonF shield;
                            shield << QPointF(-15, 0) << QPointF(15, 0) << QPointF(20, 20) 
                                  << QPointF(0, 40) << QPointF(-20, 20);
                            painter.drawPolygon(shield);
                        }
                        break;
                }
            }
            break;
            
        case ItemType::ACCESSORY:
            // Draw accessories
            {
                if (modelId % 3 == 0) {
                    // Ring
                    painter.setBrush(QBrush(itemColor));
                    painter.setPen(QPen(itemColor.darker(150), 3));
                    painter.drawEllipse(QPoint(0, 0), 80, 80);
                    painter.setBrush(QBrush(Qt::transparent));
                    painter.setPen(QPen(itemColor.lighter(120), 25));
                    painter.drawEllipse(QPoint(0, 0), 50, 50);
                    
                    // Add a gem based on texture ID
                    if (textureId > 0) {
                        QColor gemColor;
                        switch (textureId % 7) {
                            case 0: gemColor = QColor(255, 0, 0); break;    // Ruby
                            case 1: gemColor = QColor(0, 0, 255); break;    // Sapphire
                            case 2: gemColor = QColor(0, 255, 0); break;    // Emerald
                            case 3: gemColor = QColor(255, 255, 0); break;  // Topaz
                            case 4: gemColor = QColor(255, 0, 255); break;  // Amethyst
                            case 5: gemColor = QColor(0, 255, 255); break;  // Aquamarine
                            case 6: gemColor = QColor(255, 255, 255); break;// Diamond
                        }
                        
                        painter.setBrush(QBrush(gemColor));
                        painter.setPen(QPen(gemColor.darker(150), 1));
                        painter.drawEllipse(QPoint(0, -80), 20, 20);
                    }
                } else if (modelId % 3 == 1) {
                    // Necklace/Amulet
                    painter.setBrush(QBrush(itemColor));
                    painter.setPen(QPen(itemColor.darker(150), 3));
                    
                    // Chain
                    painter.drawArc(-60, -100, 120, 120, 30 * 16, 120 * 16);
                    
                    // Connect to pendant
                    painter.drawLine(-30, -45, 0, 0);
                    painter.drawLine(30, -45, 0, 0);
                    
                    // Pendant shape based on texture ID
                    QBrush pendantBrush;
                    if (textureId % 4 == 0) {
                        // Solid color
                        pendantBrush = QBrush(itemColor);
                    } else if (textureId % 4 == 1) {
                        // Gradient
                        QLinearGradient gradient(0, 0, 0, 80);
                        gradient.setColorAt(0, itemColor.lighter(130));
                        gradient.setColorAt(1, itemColor.darker(150));
                        pendantBrush = QBrush(gradient);
                    } else if (textureId % 4 == 2) {
                        // Radial gradient
                        QRadialGradient gradient(QPointF(0, 40), 60);
                        gradient.setColorAt(0, itemColor.lighter(130));
                        gradient.setColorAt(1, itemColor.darker(130));
                        pendantBrush = QBrush(gradient);
                    } else {
                        // Texture pattern
                        pendantBrush = QBrush(itemColor, Qt::DiagCrossPattern);
                    }
                    
                    painter.setBrush(pendantBrush);
                    
                    if (textureId % 3 == 0) {
                        // Circle pendant
                        painter.drawEllipse(QPoint(0, 40), 40, 40);
                    } else if (textureId % 3 == 1) {
                        // Diamond pendant
                        QPolygonF diamond;
                        diamond << QPointF(0, 0) << QPointF(40, 40) << QPointF(0, 80) << QPointF(-40, 40);
                        painter.drawPolygon(diamond);
                    } else {
                        // Heart pendant
                        QPainterPath path;
                        path.moveTo(0, 80);
                        path.cubicTo(-40, 40, -40, 0, 0, 20);
                        path.cubicTo(40, 0, 40, 40, 0, 80);
                        painter.drawPath(path);
                    }
                } else {
                    // Bracelet
                    painter.setBrush(QBrush(itemColor));
                    painter.setPen(QPen(itemColor.darker(150), 5));
                    painter.drawEllipse(QPoint(0, 0), 60, 30);
                    
                    // Add details based on texture ID
                    painter.setPen(QPen(itemColor.lighter(130), 2));
                    int gems = textureId % 8 + 1;
                    double angleStep = 2 * M_PI / gems;
                    
                    for (int i = 0; i < gems; i++) {
                        double angle = i * angleStep;
                        int gemX = static_cast<int>(cos(angle) * 60);
                        int gemY = static_cast<int>(sin(angle) * 30);
                        
                        QColor gemColor = QColor::fromHsv((i * 255 / gems) % 360, 255, 255);
                        painter.setBrush(QBrush(gemColor));
                        painter.setPen(QPen(gemColor.darker(150), 1));
                        painter.drawEllipse(QPoint(gemX, gemY), 10, 10);
                    }
                }
            }
            break;
            
        case ItemType::POTION:
            // Draw potion
            {
                // Bottle shape
                QPainterPath bottlePath;
                bottlePath.moveTo(-30, -100);
                bottlePath.lineTo(30, -100);
                bottlePath.lineTo(30, -70);
                bottlePath.lineTo(40, -60);
                bottlePath.lineTo(40, 80);
                bottlePath.lineTo(-40, 80);
                bottlePath.lineTo(-40, -60);
                bottlePath.lineTo(-30, -70);
                bottlePath.lineTo(-30, -100);
                
                // Bottle color based on texture
                QColor bottleColor = QColor(200, 200, 220, 180); // Glass color
                painter.setBrush(QBrush(bottleColor));
                painter.setPen(QPen(bottleColor.darker(150), 2));
                painter.drawPath(bottlePath);
                
                // Liquid color based on color selection
                QColor liquidColor = itemColor;
                liquidColor.setAlpha(200);
                
                // Liquid level based on model ID
                int fillLevel = 20 + (modelId % 5) * 15;
                
                // Liquid
                QPainterPath liquidPath;
                liquidPath.moveTo(-39, 80 - fillLevel);
                liquidPath.lineTo(39, 80 - fillLevel);
                liquidPath.lineTo(39, 79);
                liquidPath.lineTo(-39, 79);
                liquidPath.lineTo(-39, 80 - fillLevel);
                
                painter.setBrush(QBrush(liquidColor));
                painter.setPen(Qt::NoPen);
                painter.drawPath(liquidPath);
                
                // Cork
                painter.setBrush(QBrush(QColor(139, 69, 19)));
                painter.setPen(QPen(QColor(101, 67, 33), 1));
                painter.drawRect(-25, -120, 50, 20);
                
                // Neck ring
                painter.setBrush(QBrush(QColor(192, 192, 192)));
                painter.setPen(QPen(QColor(128, 128, 128), 1));
                painter.drawRect(-32, -102, 64, 4);
                
                // Bubbles based on texture ID
                if (textureId > 0) {
                    painter.setPen(QPen(Qt::white, 1));
                    painter.setBrush(QBrush(QColor(255, 255, 255, 100)));
                    
                    // Random bubbles
                    std::mt19937 gen(textureId); // Use texture ID as seed
                    std::uniform_int_distribution<> xDist(-35, 35);
                    std::uniform_int_distribution<> yDist(80 - fillLevel, 75);
                    std::uniform_int_distribution<> sizeDist(3, 10);
                    
                    int bubbleCount = textureId % 8 + 2;
                    for (int i = 0; i < bubbleCount; i++) {
                        int x = xDist(gen);
                        int y = yDist(gen);
                        int size = sizeDist(gen);
                        painter.drawEllipse(QPoint(x, y), size, size);
                    }
                }
                
                // Shine effect
                painter.setPen(QPen(Qt::white, 1));
                painter.setOpacity(0.4);
                painter.drawLine(-35, -50, -35, 50);
            }
            break;
            
        default:
            // Default shape
            painter.setBrush(QBrush(itemColor));
            painter.setPen(QPen(itemColor.darker(150), 3));
            painter.drawRoundedRect(-80, -80, 160, 160, 20, 20);
            
            // Simple details
            painter.setPen(QPen(itemColor.lighter(130), 2));
            painter.drawLine(-60, -40, 60, -40);
            painter.drawLine(-60, 0, 60, 0);
            painter.drawLine(-60, 40, 60, 40);
            
            // Texture-based details
            if (textureId > 0) {
                painter.setPen(QPen(itemColor.lighter(150), 1));
                
                switch (textureId % 4) {
                    case 0:
                        // Grid pattern
                        for (int i = -60; i <= 60; i += 30) {
                            painter.drawLine(i, -60, i, 60);
                            painter.drawLine(-60, i, 60, i);
                        }
                        break;
                    case 1:
                        // X pattern
                        painter.drawLine(-60, -60, 60, 60);
                        painter.drawLine(-60, 60, 60, -60);
                        break;
                    case 2:
                        // Concentric circles
                        for (int r = 20; r <= 80; r += 20) {
                            painter.drawEllipse(QPoint(0, 0), r, r);
                        }
                        break;
                    case 3:
                        // Spiral pattern
                        QPainterPath spiralPath;
                        spiralPath.moveTo(0, 0);
                        for (int i = 0; i < 720; i += 5) {
                            double angle = i * M_PI / 180.0;
                            double r = i / 720.0 * 80.0;
                            double x = cos(angle) * r;
                            double y = sin(angle) * r;
                            spiralPath.lineTo(x, y);
                        }
                        painter.drawPath(spiralPath);
                        break;
                }
            }
    }
    
    // Add glow effect if enabled
    if (hasGlow) {
        painter.setOpacity(0.4);
        painter.setPen(Qt::NoPen);
        
        QRadialGradient gradient(QPointF(0, 0), 120);
        
        QColor glowColor = itemColor.lighter(150);
        gradient.setColorAt(0, glowColor);
        gradient.setColorAt(1, Qt::transparent);
        
        painter.setBrush(gradient);
        painter.drawEllipse(-120, -120, 240, 240);
    }
    
    // Create a graphics item from the pixmap
    QGraphicsPixmapItem* item = m_scene->addPixmap(preview);
    item->setOffset(-150, -150);
    
    // Emit signal that appearance has changed
    emit appearanceChanged();
}

//--------------------------------------------------------------------------------------------
// ItemAttributesEditor Implementation
//--------------------------------------------------------------------------------------------

ItemAttributesEditor::ItemAttributesEditor(QWidget* parent)
    : QWidget(parent)
{
    createUI();
}

void ItemAttributesEditor::setItem(const ItemUIModel& item)
{
    // Update level requirements
    m_levelSpinBox->setValue(item.level);
    m_reqLevelSpinBox->setValue(item.requirements.level);
    m_reqStrSpinBox->setValue(item.requirements.strength);
    m_reqIntSpinBox->setValue(item.requirements.intelligence);
    m_reqDexSpinBox->setValue(item.requirements.dexterity);
    m_reqConSpinBox->setValue(item.requirements.constitution);
    
    // Update stats
    m_attackSpinBox->setValue(item.stats.attack);
    m_defenseSpinBox->setValue(item.stats.defense);
    m_magicAttackSpinBox->setValue(item.stats.magicAttack);
    m_magicDefenseSpinBox->setValue(item.stats.magicDefense);
    m_hitRateSpinBox->setValue(item.stats.hitRate);
    m_dodgeRateSpinBox->setValue(item.stats.dodgeRate);
    m_hpBonusSpinBox->setValue(item.stats.hpBonus);
    m_mpBonusSpinBox->setValue(item.stats.mpBonus);
    
    // Update class restrictions
    m_usableByTKCheckBox->setChecked(item.requirements.usableByTK);
    m_usableByFMCheckBox->setChecked(item.requirements.usableByFM);
    m_usableByBMCheckBox->setChecked(item.requirements.usableByBM);
    m_usableByHTCheckBox->setChecked(item.requirements.usableByHT);
    
    // Update trade settings
    m_priceSpinBox->setValue(item.price);
    
    // Update rarity
    int rarityIndex = static_cast<int>(item.rarity);
    if (rarityIndex >= 0 && rarityIndex < m_rarityComboBox->count()) {
        m_rarityComboBox->setCurrentIndex(rarityIndex);
    }
    
    // Update flags
    m_tradeableCheckBox->setChecked(item.flags.tradeable);
    m_storableCheckBox->setChecked(item.flags.storable);
    m_sellableCheckBox->setChecked(item.flags.sellable);
    m_droppableCheckBox->setChecked(item.flags.droppable);
    m_questItemCheckBox->setChecked(item.flags.questItem);
    
    // Update upgrade info
    m_maxSocketsSpinBox->setValue(item.maxSockets);
    m_maxUpgradeLevelSpinBox->setValue(item.maxUpgradeLevel);
}

void ItemAttributesEditor::updateItemAttributes(ItemUIModel& item)
{
    // Update level and requirements
    item.level = m_levelSpinBox->value();
    item.requirements.level = m_reqLevelSpinBox->value();
    item.requirements.strength = m_reqStrSpinBox->value();
    item.requirements.intelligence = m_reqIntSpinBox->value();
    item.requirements.dexterity = m_reqDexSpinBox->value();
    item.requirements.constitution = m_reqConSpinBox->value();
    
    // Update stats
    item.stats.attack = m_attackSpinBox->value();
    item.stats.defense = m_defenseSpinBox->value();
    item.stats.magicAttack = m_magicAttackSpinBox->value();
    item.stats.magicDefense = m_magicDefenseSpinBox->value();
    item.stats.hitRate = m_hitRateSpinBox->value();
    item.stats.dodgeRate = m_dodgeRateSpinBox->value();
    item.stats.hpBonus = m_hpBonusSpinBox->value();
    item.stats.mpBonus = m_mpBonusSpinBox->value();
    
    // Update class restrictions
    item.requirements.usableByTK = m_usableByTKCheckBox->isChecked();
    item.requirements.usableByFM = m_usableByFMCheckBox->isChecked();
    item.requirements.usableByBM = m_usableByBMCheckBox->isChecked();
    item.requirements.usableByHT = m_usableByHTCheckBox->isChecked();
    
    // Update trade settings
    item.price = m_priceSpinBox->value();
    item.rarity = static_cast<ItemRarity>(m_rarityComboBox->currentIndex());
    
    // Update flags
    item.flags.tradeable = m_tradeableCheckBox->isChecked();
    item.flags.storable = m_storableCheckBox->isChecked();
    item.flags.sellable = m_sellableCheckBox->isChecked();
    item.flags.droppable = m_droppableCheckBox->isChecked();
    item.flags.questItem = m_questItemCheckBox->isChecked();
    
    // Update upgrade info
    item.maxSockets = m_maxSocketsSpinBox->value();
    item.maxUpgradeLevel = m_maxUpgradeLevelSpinBox->value();
}

void ItemAttributesEditor::createUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create tab widget for organizing attributes
    QTabWidget* attributeTabs = new QTabWidget(this);
    
    // Requirements tab
    QWidget* requirementsTab = new QWidget();
    QGridLayout* reqLayout = new QGridLayout(requirementsTab);
    
    // Item level
    reqLayout->addWidget(new QLabel("Item Level:"), 0, 0);
    m_levelSpinBox = new QSpinBox();
    m_levelSpinBox->setRange(0, 999);
    reqLayout->addWidget(m_levelSpinBox, 0, 1);
    
    // Required level
    reqLayout->addWidget(new QLabel("Required Level:"), 1, 0);
    m_reqLevelSpinBox = new QSpinBox();
    m_reqLevelSpinBox->setRange(0, 999);
    reqLayout->addWidget(m_reqLevelSpinBox, 1, 1);
    
    // Required stats
    reqLayout->addWidget(new QLabel("Required STR:"), 2, 0);
    m_reqStrSpinBox = new QSpinBox();
    m_reqStrSpinBox->setRange(0, 999);
    reqLayout->addWidget(m_reqStrSpinBox, 2, 1);
    
    reqLayout->addWidget(new QLabel("Required INT:"), 3, 0);
    m_reqIntSpinBox = new QSpinBox();
    m_reqIntSpinBox->setRange(0, 999);
    reqLayout->addWidget(m_reqIntSpinBox, 3, 1);
    
    reqLayout->addWidget(new QLabel("Required DEX:"), 4, 0);
    m_reqDexSpinBox = new QSpinBox();
    m_reqDexSpinBox->setRange(0, 999);
    reqLayout->addWidget(m_reqDexSpinBox, 4, 1);
    
    reqLayout->addWidget(new QLabel("Required CON:"), 5, 0);
    m_reqConSpinBox = new QSpinBox();
    m_reqConSpinBox->setRange(0, 999);
    reqLayout->addWidget(m_reqConSpinBox, 5, 1);
    
    // Class restrictions
    QGroupBox* classGroup = new QGroupBox("Usable by Classes:");
    QVBoxLayout* classLayout = new QVBoxLayout(classGroup);
    
    m_usableByTKCheckBox = new QCheckBox("Transcendental Knight (TK)");
    m_usableByFMCheckBox = new QCheckBox("Force Master (FM)");
    m_usableByBMCheckBox = new QCheckBox("Blade Master (BM)");
    m_usableByHTCheckBox = new QCheckBox("Holy Tide (HT)");
    
    classLayout->addWidget(m_usableByTKCheckBox);
    classLayout->addWidget(m_usableByFMCheckBox);
    classLayout->addWidget(m_usableByBMCheckBox);
    classLayout->addWidget(m_usableByHTCheckBox);
    
    reqLayout->addWidget(classGroup, 6, 0, 1, 2);
    
    // Add stretch to fill the remaining space
    reqLayout->setRowStretch(7, 1);
    
    // Stats tab
    QWidget* statsTab = new QWidget();
    QGridLayout* statsLayout = new QGridLayout(statsTab);
    
    // Combat stats
    statsLayout->addWidget(new QLabel("Attack:"), 0, 0);
    m_attackSpinBox = new QSpinBox();
    m_attackSpinBox->setRange(0, 9999);
    statsLayout->addWidget(m_attackSpinBox, 0, 1);
    
    statsLayout->addWidget(new QLabel("Defense:"), 1, 0);
    m_defenseSpinBox = new QSpinBox();
    m_defenseSpinBox->setRange(0, 9999);
    statsLayout->addWidget(m_defenseSpinBox, 1, 1);
    
    statsLayout->addWidget(new QLabel("Magic Attack:"), 2, 0);
    m_magicAttackSpinBox = new QSpinBox();
    m_magicAttackSpinBox->setRange(0, 9999);
    statsLayout->addWidget(m_magicAttackSpinBox, 2, 1);
    
    statsLayout->addWidget(new QLabel("Magic Defense:"), 3, 0);
    m_magicDefenseSpinBox = new QSpinBox();
    m_magicDefenseSpinBox->setRange(0, 9999);
    statsLayout->addWidget(m_magicDefenseSpinBox, 3, 1);
    
    statsLayout->addWidget(new QLabel("Hit Rate:"), 4, 0);
    m_hitRateSpinBox = new QSpinBox();
    m_hitRateSpinBox->setRange(0, 999);
    statsLayout->addWidget(m_hitRateSpinBox, 4, 1);
    
    statsLayout->addWidget(new QLabel("Dodge Rate:"), 5, 0);
    m_dodgeRateSpinBox = new QSpinBox();
    m_dodgeRateSpinBox->setRange(0, 999);
    statsLayout->addWidget(m_dodgeRateSpinBox, 5, 1);
    
    statsLayout->addWidget(new QLabel("HP Bonus:"), 6, 0);
    m_hpBonusSpinBox = new QSpinBox();
    m_hpBonusSpinBox->setRange(0, 9999);
    statsLayout->addWidget(m_hpBonusSpinBox, 6, 1);
    
    statsLayout->addWidget(new QLabel("MP Bonus:"), 7, 0);
    m_mpBonusSpinBox = new QSpinBox();
    m_mpBonusSpinBox->setRange(0, 9999);
    statsLayout->addWidget(m_mpBonusSpinBox, 7, 1);
    
    // Add stretch to fill the remaining space
    statsLayout->setRowStretch(8, 1);
    
    // Trading tab
    QWidget* tradingTab = new QWidget();
    QGridLayout* tradingLayout = new QGridLayout(tradingTab);
    
    tradingLayout->addWidget(new QLabel("Price:"), 0, 0);
    m_priceSpinBox = new QSpinBox();
    m_priceSpinBox->setRange(0, 999999999);
    m_priceSpinBox->setSingleStep(100);
    tradingLayout->addWidget(m_priceSpinBox, 0, 1);
    
    tradingLayout->addWidget(new QLabel("Rarity:"), 1, 0);
    m_rarityComboBox = new QComboBox();
    m_rarityComboBox->addItems(QStringList() 
        << "Common" << "Uncommon" << "Rare" << "Epic" << "Legendary" << "Mythic");
    tradingLayout->addWidget(m_rarityComboBox, 1, 1);
    
    // Item flags
    QGroupBox* flagsGroup = new QGroupBox("Flags:");
    QVBoxLayout* flagsLayout = new QVBoxLayout(flagsGroup);
    
    m_tradeableCheckBox = new QCheckBox("Tradeable");
    m_storableCheckBox = new QCheckBox("Storable");
    m_sellableCheckBox = new QCheckBox("Sellable");
    m_droppableCheckBox = new QCheckBox("Droppable");
    m_questItemCheckBox = new QCheckBox("Quest Item");
    
    flagsLayout->addWidget(m_tradeableCheckBox);
    flagsLayout->addWidget(m_storableCheckBox);
    flagsLayout->addWidget(m_sellableCheckBox);
    flagsLayout->addWidget(m_droppableCheckBox);
    flagsLayout->addWidget(m_questItemCheckBox);
    
    tradingLayout->addWidget(flagsGroup, 2, 0, 1, 2);
    
    // Add stretch to fill the remaining space
    tradingLayout->setRowStretch(3, 1);
    
    // Upgrade tab
    QWidget* upgradeTab = new QWidget();
    QGridLayout* upgradeLayout = new QGridLayout(upgradeTab);
    
    upgradeLayout->addWidget(new QLabel("Max Sockets:"), 0, 0);
    m_maxSocketsSpinBox = new QSpinBox();
    m_maxSocketsSpinBox->setRange(0, 9);
    upgradeLayout->addWidget(m_maxSocketsSpinBox, 0, 1);
    
    upgradeLayout->addWidget(new QLabel("Max Upgrade Level:"), 1, 0);
    m_maxUpgradeLevelSpinBox = new QSpinBox();
    m_maxUpgradeLevelSpinBox->setRange(0, 20);
    upgradeLayout->addWidget(m_maxUpgradeLevelSpinBox, 1, 1);
    
    // Add stretch to fill the remaining space
    upgradeLayout->setRowStretch(2, 1);
    
    // Add tabs to tab widget
    attributeTabs->addTab(requirementsTab, "Requirements");
    attributeTabs->addTab(statsTab, "Stats");
    attributeTabs->addTab(tradingTab, "Trading");
    attributeTabs->addTab(upgradeTab, "Upgrade");
    
    mainLayout->addWidget(attributeTabs);
    
    setLayout(mainLayout);
}

//--------------------------------------------------------------------------------------------
// ItemStudioView Implementation
//--------------------------------------------------------------------------------------------

ItemStudioView::ItemStudioView(QWidget* parent)
    : QWidget(parent)
{
    createUI();
}

ItemStudioView::~ItemStudioView()
{
}

void ItemStudioView::initialize(const std::shared_ptr<core::WYDStudioFacade>& facade)
{
    m_facade = facade;
    m_itemStudio = std::make_shared<ItemStudio>(facade);
    
    if (m_itemStudio->initialize()) {
        loadItems();
    } else {
        showMessage("Error", "Failed to initialize Item Studio. Please check the logs for more information.", QMessageBox::Critical);
    }
}

void ItemStudioView::createUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Setup toolbar
    setupToolbar();
    mainLayout->addWidget(m_toolbar);
    
    // Main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setHandleWidth(1);
    m_mainSplitter->setChildrenCollapsible(false);
    
    // Setup item views
    setupItemViews();
    
    // Setup editor panel
    setupEditorPanel();
    
    // Add to splitter
    m_mainSplitter->addWidget(m_viewTabs);
    m_mainSplitter->addWidget(m_editorTabs);
    
    // Set initial sizes (34% to 66%)
    QList<int> sizes;
    sizes << 300 << 500;
    m_mainSplitter->setSizes(sizes);
    
    mainLayout->addWidget(m_mainSplitter, 1);
    
    setLayout(mainLayout);
}

void ItemStudioView::setupToolbar()
{
    m_toolbar = new QToolBar(this);
    m_toolbar->setIconSize(QSize(24, 24));
    
    // New Item
    QAction* newAction = m_toolbar->addAction("New Item");
    connect(newAction, &QAction::triggered, this, &ItemStudioView::onCreateNewItem);
    
    // Duplicate
    QAction* duplicateAction = m_toolbar->addAction("Duplicate");
    connect(duplicateAction, &QAction::triggered, this, &ItemStudioView::onDuplicateItem);
    
    // Delete
    QAction* deleteAction = m_toolbar->addAction("Delete");
    connect(deleteAction, &QAction::triggered, this, &ItemStudioView::onDeleteItem);
    
    m_toolbar->addSeparator();
    
    // Save
    QAction* saveAction = m_toolbar->addAction("Save");
    connect(saveAction, &QAction::triggered, this, &ItemStudioView::onSaveItem);
    
    // Refresh
    QAction* refreshAction = m_toolbar->addAction("Refresh");
    connect(refreshAction, &QAction::triggered, this, &ItemStudioView::onRefreshItems);
    
    m_toolbar->addSeparator();
    
    // Search box
    m_toolbar->addWidget(new QLabel("Search:"));
    m_searchEdit = new QLineEdit();
    m_searchEdit->setMinimumWidth(150);
    m_toolbar->addWidget(m_searchEdit);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ItemStudioView::onSearchTextChanged);
    
    // Filter combobox
    m_toolbar->addWidget(new QLabel("Filter:"));
    m_filterComboBox = new QComboBox();
    m_filterComboBox->addItem("All Items");
    m_filterComboBox->addItem("Weapons");
    m_filterComboBox->addItem("Armor");
    m_filterComboBox->addItem("Accessories");
    m_filterComboBox->addItem("Potions");
    m_filterComboBox->addItem("Materials");
    m_filterComboBox->addItem("Quest Items");
    m_toolbar->addWidget(m_filterComboBox);
    connect(m_filterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ItemStudioView::onFilterChanged);
    
    // View mode combobox
    m_toolbar->addWidget(new QLabel("View:"));
    m_viewModeComboBox = new QComboBox();
    m_viewModeComboBox->addItem("Grid");
    m_viewModeComboBox->addItem("List");
    m_toolbar->addWidget(m_viewModeComboBox);
    connect(m_viewModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ItemStudioView::onViewModeChanged);
    
    m_toolbar->addSeparator();
    
    // Apply to server
    QAction* applyAction = m_toolbar->addAction("Apply to Server");
    connect(applyAction, &QAction::triggered, this, &ItemStudioView::onApplyToServer);
    
    // Import/Export
    QAction* importAction = m_toolbar->addAction("Import");
    connect(importAction, &QAction::triggered, this, &ItemStudioView::onImportItems);
    
    QAction* exportAction = m_toolbar->addAction("Export");
    connect(exportAction, &QAction::triggered, this, &ItemStudioView::onExportItems);
    
    // Additional tools
    m_toolbar->addSeparator();
    
    QAction* balanceAction = m_toolbar->addAction("Balance Item");
    connect(balanceAction, &QAction::triggered, this, &ItemStudioView::onBalanceItem);
}

void ItemStudioView::setupItemViews()
{
    m_viewTabs = new QTabWidget();
    
    // Create grid view
    m_gridView = new ItemGridView();
    m_gridModel = new ItemGridModel(this);
    m_gridView->setModel(m_gridModel);
    
    connect(m_gridView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ItemStudioView::onItemSelected);
    
    // Create tree view
    m_treeView = new QTreeView();
    m_treeModel = new ItemTreeModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_treeModel);
    m_treeView->setModel(m_proxyModel);
    
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setRootIsDecorated(true);
    m_treeView->setUniformRowHeights(true);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ItemStudioView::onItemSelected);
    
    // Add views to tabs
    m_viewTabs->addTab(m_gridView, "Grid View");
    m_viewTabs->addTab(m_treeView, "Tree View");
    
    // Set grid view as default
    m_viewTabs->setCurrentIndex(0);
}

void ItemStudioView::setupEditorPanel()
{
    m_editorTabs = new QTabWidget();
    
    // General tab
    m_generalTab = new QWidget();
    setupGeneralTab();
    
    // Appearance tab
    m_appearanceTab = new ItemAppearanceEditor();
    connect(m_appearanceTab, &ItemAppearanceEditor::appearanceChanged, 
            [this]() {
                if (m_currentItem) {
                    m_appearanceTab->updateItemAppearance(*m_currentItem);
                }
            });
    
    // Attributes tab
    m_attributesTab = new ItemAttributesEditor();
    connect(m_attributesTab, &ItemAttributesEditor::attributesChanged,
            [this]() {
                if (m_currentItem) {
                    m_attributesTab->updateItemAttributes(*m_currentItem);
                }
            });
    
    // Special effects tab
    m_specialEffectsTab = new QWidget();
    setupSpecialEffectsTab();
    
    // Add tabs to editor
    m_editorTabs->addTab(m_generalTab, "General");
    m_editorTabs->addTab(m_appearanceTab, "Appearance");
    m_editorTabs->addTab(m_attributesTab, "Attributes");
    m_editorTabs->addTab(m_specialEffectsTab, "Special Effects");
}

void ItemStudioView::setupGeneralTab()
{
    QVBoxLayout* layout = new QVBoxLayout(m_generalTab);
    
    QGridLayout* formLayout = new QGridLayout();
    
    // Item ID (read-only)
    formLayout->addWidget(new QLabel("Item ID:"), 0, 0);
    m_idSpinBox = new QSpinBox();
    m_idSpinBox->setRange(0, 999999);
    m_idSpinBox->setReadOnly(true);
    m_idSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_idSpinBox->setStyleSheet("QSpinBox { background-color: #f0f0f0; }");
    formLayout->addWidget(m_idSpinBox, 0, 1);
    
    // Item name
    formLayout->addWidget(new QLabel("Name:"), 1, 0);
    m_nameEdit = new QLineEdit();
    formLayout->addWidget(m_nameEdit, 1, 1);
    
    // Category
    formLayout->addWidget(new QLabel("Category:"), 2, 0);
    m_categoryComboBox = new QComboBox();
    m_categoryComboBox->setEditable(true);
    m_categoryComboBox->addItems(QStringList() 
        << "General" << "Weapon" << "Armor" << "Accessory" << "Consumable" 
        << "Material" << "Quest" << "Special");
    formLayout->addWidget(m_categoryComboBox, 2, 1);
    
    // Type
    formLayout->addWidget(new QLabel("Type:"), 3, 0);
    m_typeComboBox = new QComboBox();
    m_typeComboBox->addItems(QStringList()
        << "Weapon" << "Armor" << "Accessory" << "Potion" << "Material" << "Quest" << "Other");
    formLayout->addWidget(m_typeComboBox, 3, 1);
    
    // Description
    formLayout->addWidget(new QLabel("Description:"), 4, 0, 1, 2);
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setMaximumHeight(100);
    formLayout->addWidget(m_descriptionEdit, 5, 0, 1, 2);
    
    layout->addLayout(formLayout);
    
    // Add stretch to push everything to the top
    layout->addStretch(1);
}

void ItemStudioView::setupSpecialEffectsTab()
{
    QVBoxLayout* layout = new QVBoxLayout(m_specialEffectsTab);
    
    // This is a placeholder for the special effects editor
    // In a real implementation, this would be replaced with actual controls
    
    QLabel* placeholderLabel = new QLabel("Special Effects Editor Coming Soon");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(placeholderLabel);
    
    layout->addStretch(1);
}

void ItemStudioView::loadItems()
{
    std::vector<ItemUIModel> items = m_itemStudio->getAllItems();
    
    m_gridModel->setItems(items);
    m_treeModel->setItems(items);
    
    // Auto-select the first item if available
    if (!items.empty()) {
        QModelIndex firstIndex = m_gridModel->index(0, 0);
        m_gridView->setCurrentIndex(firstIndex);
        onItemSelected(firstIndex);
    } else {
        clearItemDetails();
    }
}

void ItemStudioView::onItemSelected(const QModelIndex& index)
{
    if (!index.isValid()) {
        clearItemDetails();
        return;
    }
    
    ItemUIModel selectedItem;
    
    // Determine which view the selection came from
    if (m_viewTabs->currentWidget() == m_gridView) {
        selectedItem = m_gridModel->getItem(index);
    } else {
        // If from tree view, need to get from proxy model
        QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
        selectedItem = m_treeModel->getItem(sourceIndex);
    }
    
    // Only process if we have a valid item
    if (selectedItem.id > 0) {
        m_currentItem = selectedItem;
        updateItemDetails();
    } else {
        clearItemDetails();
    }
}

void ItemStudioView::updateItemDetails()
{
    if (!m_currentItem) {
        clearItemDetails();
        return;
    }
    
    // Update general tab
    m_idSpinBox->setValue(m_currentItem->id);
    m_nameEdit->setText(QString::fromStdString(m_currentItem->name));
    m_categoryComboBox->setCurrentText(QString::fromStdString(m_currentItem->category));
    
    int typeIndex = static_cast<int>(m_currentItem->type);
    if (typeIndex >= 0 && typeIndex < m_typeComboBox->count()) {
        m_typeComboBox->setCurrentIndex(typeIndex);
    }
    
    m_descriptionEdit->setText(QString::fromStdString(m_currentItem->description));
    
    // Update appearance tab
    m_appearanceTab->setItem(*m_currentItem);
    
    // Update attributes tab
    m_attributesTab->setItem(*m_currentItem);
    
    // Update special effects tab
    // This would be implemented in a real system
}

void ItemStudioView::clearItemDetails()
{
    m_currentItem = std::nullopt;
    
    // Clear general tab
    m_idSpinBox->setValue(0);
    m_nameEdit->clear();
    m_categoryComboBox->setCurrentIndex(0);
    m_typeComboBox->setCurrentIndex(0);
    m_descriptionEdit->clear();
    
    // Clear other tabs as needed
    // This is simplified for the example
}

void ItemStudioView::collectItemDataFromUI(ItemUIModel& item)
{
    // Collect data from general tab
    item.name = m_nameEdit->text().toStdString();
    item.category = m_categoryComboBox->currentText().toStdString();
    item.type = static_cast<ItemType>(m_typeComboBox->currentIndex());
    item.description = m_descriptionEdit->toPlainText().toStdString();
    
    // Collect from appearance tab
    m_appearanceTab->updateItemAppearance(item);
    
    // Collect from attributes tab
    m_attributesTab->updateItemAttributes(item);
    
    // Special effects tab would be added in a real implementation
}

void ItemStudioView::onCreateNewItem()
{
    // Create a new empty item with default values
    ItemUIModel newItem = m_itemStudio->getNewItemTemplate();
    
    // Add to models
    m_gridModel->addItem(newItem);
    m_treeModel->setItems(m_itemStudio->getAllItems());
    
    // Select the new item
    selectItemById(newItem.id);
    
    // Show message
    showMessage("Item Created", "New item has been created successfully.");
}

void ItemStudioView::onDuplicateItem()
{
    if (!m_currentItem) {
        showMessage("Error", "No item selected to duplicate.", QMessageBox::Warning);
        return;
    }
    
    // Call the duplicate function
    ItemOperationResult result = m_itemStudio->duplicateItem(m_currentItem->id);
    
    if (result.success && result.item) {
        // Add to models
        m_gridModel->addItem(*result.item);
        m_treeModel->setItems(m_itemStudio->getAllItems());
        
        // Select the new item
        selectItemById(result.item->id);
        
        // Show message
        showMessage("Item Duplicated", "Item has been duplicated successfully.");
    } else {
        showMessage("Error", "Failed to duplicate item: " + QString::fromStdString(result.message), QMessageBox::Warning);
    }
}

void ItemStudioView::onDeleteItem()
{
    if (!m_currentItem) {
        showMessage("Error", "No item selected to delete.", QMessageBox::Warning);
        return;
    }
    
    // Ask for confirmation
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to delete this item?\nThis action cannot be undone.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        uint32_t itemId = m_currentItem->id;
        
        // Call the delete function
        ItemOperationResult result = m_itemStudio->removeItem(itemId);
        
        if (result.success) {
            // Remove from models
            m_gridModel->removeItem(itemId);
            m_treeModel->setItems(m_itemStudio->getAllItems());
            
            // Clear current item
            m_currentItem = std::nullopt;
            clearItemDetails();
            
            // Show message
            showMessage("Item Deleted", "Item has been deleted successfully.");
        } else {
            showMessage("Error", "Failed to delete item: " + QString::fromStdString(result.message), QMessageBox::Warning);
        }
    }
}

void ItemStudioView::onSaveItem()
{
    if (!m_currentItem) {
        showMessage("Error", "No item selected to save.", QMessageBox::Warning);
        return;
    }
    
    // Update the item from UI
    collectItemDataFromUI(*m_currentItem);
    
    // Call the save function
    ItemOperationResult result = m_itemStudio->saveItem(*m_currentItem);
    
    if (result.success && result.item) {
        // Update the item in models
        updateItemInModel(*result.item);
        
        // Show message
        showMessage("Item Saved", "Item has been saved successfully.");
    } else {
        showMessage("Error", "Failed to save item: " + QString::fromStdString(result.message), QMessageBox::Warning);
    }
}

void ItemStudioView::onRefreshItems()
{
    // Get the ID of the current item if any
    std::optional<uint32_t> currentId;
    if (m_currentItem) {
        currentId = m_currentItem->id;
    }
    
    // Reload all items
    loadItems();
    
    // Reselect the previously selected item if any
    if (currentId) {
        selectItemById(*currentId);
    }
    
    // Show message
    showMessage("Refreshed", "Item list has been refreshed.");
}

void ItemStudioView::onSearchTextChanged(const QString& text)
{
    // Implement search functionality
    // This is a simplified implementation - a real system would have more advanced search
    
    if (m_viewTabs->currentWidget() == m_gridView) {
        // For grid view, we would implement a custom filter
        // For simplicity, we'll just reload and filter
        std::vector<ItemUIModel> items = m_itemStudio->getAllItems();
        
        if (!text.isEmpty()) {
            // Filter items by name containing the search text
            std::vector<ItemUIModel> filteredItems;
            
            for (const auto& item : items) {
                if (QString::fromStdString(item.name).contains(text, Qt::CaseInsensitive)) {
                    filteredItems.push_back(item);
                }
            }
            
            m_gridModel->setItems(filteredItems);
        } else {
            m_gridModel->setItems(items);
        }
    } else {
        // For tree view, we can use the proxy model
        m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        m_proxyModel->setFilterKeyColumn(0); // Filter by name column
        m_proxyModel->setFilterFixedString(text);
    }
}

void ItemStudioView::onFilterChanged(int index)
{
    // Filter based on selected category
    std::vector<ItemUIModel> items = m_itemStudio->getAllItems();
    std::vector<ItemUIModel> filteredItems;
    
    if (index == 0) {
        // "All Items" - no filtering
        filteredItems = items;
    } else {
        // Filter by type based on index
        ItemType filterType = static_cast<ItemType>(index - 1);
        
        for (const auto& item : items) {
            if (item.type == filterType) {
                filteredItems.push_back(item);
            }
        }
    }
    
    // Update both models
    m_gridModel->setItems(filteredItems);
    m_treeModel->setItems(filteredItems);
}

void ItemStudioView::onViewModeChanged(int index)
{
    // Switch tab view based on selected view mode
    m_viewTabs->setCurrentIndex(index);
}

void ItemStudioView::onApplyToServer()
{
    // This function would push changes to the live server
    // In a real implementation, this might involve exporting to binary formats
    // and reloading server data
    
    bool success = m_itemStudio->applyChangesToServer();
    
    if (success) {
        showMessage("Changes Applied", "Changes have been successfully applied to the server.");
    } else {
        showMessage("Error", "Failed to apply changes to the server. Check the logs for details.", QMessageBox::Warning);
    }
}

void ItemStudioView::onImportItems()
{
    // Open file dialog to select import file
    QString filePath = QFileDialog::getOpenFileName(this, "Import Items", QString(), "Item Data (*.json *.xml *.csv);;All Files (*)");
    
    if (filePath.isEmpty()) {
        return;
    }
    
    // Call import function
    ItemOperationResult result = m_itemStudio->importItems(filePath.toStdString());
    
    if (result.success) {
        // Refresh item lists
        loadItems();
        
        // Show message
        showMessage("Import Successful", "Items have been imported successfully.");
    } else {
        showMessage("Import Failed", "Failed to import items: " + QString::fromStdString(result.message), QMessageBox::Warning);
    }
}

void ItemStudioView::onExportItems()
{
    // Open file dialog to select export file
    QString filePath = QFileDialog::getSaveFileName(this, "Export Items", QString(), "JSON (*.json);;XML (*.xml);;CSV (*.csv);;All Files (*)");
    
    if (filePath.isEmpty()) {
        return;
    }
    
    // Determine export format based on file extension
    std::string format = "json"; // Default
    if (filePath.endsWith(".xml", Qt::CaseInsensitive)) {
        format = "xml";
    } else if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        format = "csv";
    }
    
    // Call export function
    ItemOperationResult result = m_itemStudio->exportItems(filePath.toStdString(), format);
    
    if (result.success) {
        showMessage("Export Successful", "Items have been exported successfully.");
    } else {
        showMessage("Export Failed", "Failed to export items: " + QString::fromStdString(result.message), QMessageBox::Warning);
    }
}

void ItemStudioView::onBalanceItem()
{
    if (!m_currentItem) {
        showMessage("Error", "No item selected to balance.", QMessageBox::Warning);
        return;
    }
    
    // Call the balance function
    ItemOperationResult result = m_itemStudio->balanceItem(m_currentItem->id);
    
    if (result.success && result.item) {
        // Update the item
        m_currentItem = *result.item;
        updateItemDetails();
        updateItemInModel(*result.item);
        
        // Show message
        showMessage("Item Balanced", "Item attributes have been balanced successfully.");
    } else {
        showMessage("Error", "Failed to balance item: " + QString::fromStdString(result.message), QMessageBox::Warning);
    }
}

void ItemStudioView::showMessage(const QString& title, const QString& message, QMessageBox::Icon icon)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(icon);
    msgBox.exec();
}

void ItemStudioView::updateItemInModel(const ItemUIModel& item)
{
    // Update the item in both models
    m_gridModel->updateItem(item);
    m_treeModel->setItems(m_itemStudio->getAllItems());
}

void ItemStudioView::selectItemById(uint32_t itemId)
{
    // Find and select the item in the current view
    if (m_viewTabs->currentWidget() == m_gridView) {
        QModelIndex index = m_gridModel->findItemById(itemId);
        if (index.isValid()) {
            m_gridView->setCurrentIndex(index);
            m_gridView->scrollTo(index);
        }
    } else {
        // For tree view, we need to find the item in the tree
        // This is simplified - a real implementation would be more robust
        m_treeModel->setItems(m_itemStudio->getAllItems());
        
        // Reset current item and reload details
        m_currentItem = m_itemStudio->getItemById(itemId);
        if (m_currentItem) {
            updateItemDetails();
        }
    }
}

}}} // namespace wydstudio::ui::visual