#ifndef WYDSTUDIO_MAP_DESIGNER_VIEW_H
#define WYDSTUDIO_MAP_DESIGNER_VIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include <QListWidget>
#include <QTabWidget>
#include <QDockWidget>
#include <QColorDialog>
#include <QUndoStack>
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QStatusBar>
#include <QScrollArea>
#include <QSplitter>
#include <QGridLayout>
#include <memory>
#include <vector>
#include <map>

#include "../MapDesigner.h"
#include "../../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {
namespace visual {

/**
 * @brief Modo de edição do mapa
 */
enum class MapEditMode {
    TERRAIN,       // Edição de terreno (altura, texturas)
    OBJECTS,       // Colocação e edição de objetos
    ENTITIES,      // Colocação e edição de NPCs e mobs
    WARPS,         // Definição de portais e warps
    RESPAWNS,      // Definição de pontos de respawn
    AREAS,         // Definição de áreas especiais
    COLLISION,     // Edição de colisão
    CLIMATE,       // Configuração de clima
    LIGHTING       // Configuração de iluminação
};

/**
 * @brief Item gráfico para terreno do mapa
 */
class MapTerrainItem : public QGraphicsItem {
public:
    MapTerrainItem(const MapData& mapData, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    void updateMapData(const MapData& mapData);
    void setTerrainTextureVisible(bool visible);
    void setHeightmapVisible(bool visible);
    void setAttributesVisible(bool visible);
    void setGridVisible(bool visible);
    
    // Métodos para edição direta
    void setHeightAtPosition(int x, int y, int height);
    void setTextureAtPosition(int x, int y, int textureId);
    void setAttributeAtPosition(int x, int y, MapAttribute attribute);
    
private:
    MapData m_mapData;
    bool m_showTerrainTexture;
    bool m_showHeightmap;
    bool m_showAttributes;
    bool m_showGrid;
    
    std::map<int, QPixmap> m_textureCache;
    
    void loadTextureCache();
    QColor getHeightColor(int height) const;
    QColor getAttributeColor(MapAttribute attribute) const;
};

/**
 * @brief Item gráfico para um objeto do mapa
 */
class MapObjectItem : public QGraphicsItem {
public:
    MapObjectItem(const MapObject& object, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    void updateObject(const MapObject& object);
    const MapObject& getObject() const;
    
    // Para seleção e edição
    void setSelected(bool selected);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    
private:
    MapObject m_object;
    bool m_selected;
    bool m_dragging;
    QPointF m_dragStartPos;
    
    QPixmap m_objectPixmap;
    void loadObjectPixmap();
};

/**
 * @brief Item gráfico para uma entidade (NPC, mob) do mapa
 */
class MapEntityItem : public QGraphicsItem {
public:
    MapEntityItem(const MapEntity& entity, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    void updateEntity(const MapEntity& entity);
    const MapEntity& getEntity() const;
    
    // Para seleção e edição
    void setSelected(bool selected);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    
private:
    MapEntity m_entity;
    bool m_selected;
    bool m_dragging;
    QPointF m_dragStartPos;
    
    QPixmap m_entityPixmap;
    void loadEntityPixmap();
    QColor getEntityColor() const;
};

/**
 * @brief Item gráfico para um warp/portal do mapa
 */
class MapWarpItem : public QGraphicsItem {
public:
    MapWarpItem(const MapWarp& warp, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    void updateWarp(const MapWarp& warp);
    const MapWarp& getWarp() const;
    
    // Para seleção e edição
    void setSelected(bool selected);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    
private:
    MapWarp m_warp;
    bool m_selected;
    bool m_dragging;
    QPointF m_dragStartPos;
};

/**
 * @brief Item gráfico para uma área especial do mapa
 */
class MapAreaItem : public QGraphicsItem {
public:
    MapAreaItem(const MapArea& area, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    void updateArea(const MapArea& area);
    const MapArea& getArea() const;
    
    // Para seleção e edição
    void setSelected(bool selected);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    
private:
    MapArea m_area;
    bool m_selected;
    bool m_dragging;
    QPointF m_dragStartPos;
    
    QColor getAreaColor() const;
};

/**
 * @brief Comando para desfazer/refazer alterações do mapa
 */
class MapEditCommand : public QUndoCommand {
public:
    MapEditCommand(const QString& text, MapDesignerView* view);
    
    void undo() override;
    void redo() override;
    
protected:
    MapDesignerView* m_view;
    MapData m_oldMapData;
    MapData m_newMapData;
};

/**
 * @brief Cena gráfica para o editor de mapas
 */
class MapGraphicsScene : public QGraphicsScene {
    Q_OBJECT
    
public:
    MapGraphicsScene(QObject* parent = nullptr);
    
    void setMapData(const MapData& mapData);
    void updateMapData(const MapData& mapData);
    
    void setEditMode(MapEditMode mode);
    MapEditMode getEditMode() const;
    
    // Métodos para controle de visualização
    void setTerrainTextureVisible(bool visible);
    void setHeightmapVisible(bool visible);
    void setAttributesVisible(bool visible);
    void setGridVisible(bool visible);
    void setObjectsVisible(bool visible);
    void setEntitiesVisible(bool visible);
    void setWarpsVisible(bool visible);
    void setAreasVisible(bool visible);
    
    // Métodos para edição
    void addObject(const MapObject& object);
    void updateObject(const MapObject& object);
    void removeObject(int objectId);
    
    void addEntity(const MapEntity& entity);
    void updateEntity(const MapEntity& entity);
    void removeEntity(int entityId);
    
    void addWarp(const MapWarp& warp);
    void updateWarp(const MapWarp& warp);
    void removeWarp(int warpId);
    
    void addArea(const MapArea& area);
    void updateArea(const MapArea& area);
    void removeArea(int areaId);
    
    // Seleção
    std::optional<MapObject> getSelectedObject() const;
    std::optional<MapEntity> getSelectedEntity() const;
    std::optional<MapWarp> getSelectedWarp() const;
    std::optional<MapArea> getSelectedArea() const;
    
    void clearSelection();
    
signals:
    void objectSelected(const MapObject& object);
    void entitySelected(const MapEntity& entity);
    void warpSelected(const MapWarp& warp);
    void areaSelected(const MapArea& area);
    void terrainEdited(int x, int y, int value);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    
private:
    MapData m_mapData;
    MapEditMode m_editMode;
    
    MapTerrainItem* m_terrainItem;
    std::map<int, MapObjectItem*> m_objectItems;
    std::map<int, MapEntityItem*> m_entityItems;
    std::map<int, MapWarpItem*> m_warpItems;
    std::map<int, MapAreaItem*> m_areaItems;
    
    // Estado de edição
    bool m_isEditing;
    QPointF m_lastEditPos;
    int m_brushSize;
    int m_brushValue;
    
    // Estado de seleção
    QGraphicsItem* m_selectedItem;
    
    // Métodos auxiliares
    void handleTerrainEdit(const QPointF& pos);
    void handleObjectPlacement(const QPointF& pos);
    void handleEntityPlacement(const QPointF& pos);
    void handleWarpPlacement(const QPointF& pos);
    void handleAreaPlacement(const QPointF& pos);
    void handleSelection(const QPointF& pos);
    
    // Aplicação de ferramentas
    void applyHeightTool(int x, int y, int brushSize, int value);
    void applyTextureTool(int x, int y, int brushSize, int textureId);
    void applyAttributeTool(int x, int y, int brushSize, MapAttribute attribute);
    
    // Conversões de coordenadas
    QPoint sceneToMap(const QPointF& scenePos) const;
    QPointF mapToScene(const QPoint& mapPos) const;
};

/**
 * @brief Vista gráfica para o editor de mapas
 */
class MapGraphicsView : public QGraphicsView {
    Q_OBJECT
    
public:
    MapGraphicsView(QWidget* parent = nullptr);
    
    void setScene(MapGraphicsScene* scene);
    
    // Controles de zoom e navegação
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void centerOn(int x, int y);
    
protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    
private:
    qreal m_zoom;
    bool m_isPanning;
    QPoint m_lastPanPos;
};

/**
 * @brief Widget para edição de propriedades gerais do mapa
 */
class MapPropertiesWidget : public QWidget {
    Q_OBJECT
    
public:
    MapPropertiesWidget(QWidget* parent = nullptr);
    
    void setMapData(const MapData& mapData);
    void updateMapData(MapData& mapData);
    
signals:
    void propertiesChanged();
    
private:
    // Campos de edição
    QLineEdit* m_nameEdit;
    QSpinBox* m_widthSpinBox;
    QSpinBox* m_heightSpinBox;
    QComboBox* m_typeComboBox;
    QSpinBox* m_minLevelSpinBox;
    QSpinBox* m_maxLevelSpinBox;
    QCheckBox* m_pvpEnabledCheckBox;
    QComboBox* m_climateComboBox;
    QCheckBox* m_indoorCheckBox;
    QCheckBox* m_weatherEnabledCheckBox;
    QCheckBox* m_dayNightCycleCheckBox;
    
    void createUI();
};

/**
 * @brief Widget para edição de terreno
 */
class TerrainEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    TerrainEditorWidget(QWidget* parent = nullptr);
    
    int getBrushSize() const;
    int getBrushValue() const;
    MapTerrainTool getCurrentTool() const;
    int getSelectedTextureId() const;
    MapAttribute getSelectedAttribute() const;
    
signals:
    void terrainToolChanged(MapTerrainTool tool);
    void brushSizeChanged(int size);
    void brushValueChanged(int value);
    void textureSelected(int textureId);
    void attributeSelected(MapAttribute attribute);
    void importHeightmap();
    void exportHeightmap();
    
private:
    // Ferramentas de edição
    QComboBox* m_toolComboBox;
    QSlider* m_brushSizeSlider;
    QSlider* m_brushValueSlider;
    
    // Seleção de texturas
    QListWidget* m_textureList;
    
    // Seleção de atributos
    QComboBox* m_attributeComboBox;
    
    // Botões de importação/exportação
    QPushButton* m_importButton;
    QPushButton* m_exportButton;
    
    MapTerrainTool m_currentTool;
    int m_brushSize;
    int m_brushValue;
    int m_selectedTextureId;
    MapAttribute m_selectedAttribute;
    
    void createUI();
    void loadTextures();
};

/**
 * @brief Widget para edição de objetos
 */
class ObjectEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    ObjectEditorWidget(QWidget* parent = nullptr);
    
    void setMapObject(const MapObject& object);
    void updateMapObject(MapObject& object);
    
    int getSelectedObjectModelId() const;
    
signals:
    void objectChanged();
    void createObject(const MapObject& object);
    void updateObject(const MapObject& object);
    void removeObject(int objectId);
    void selectObject(int objectId);
    
private:
    // Lista de modelos de objetos
    QListWidget* m_objectList;
    
    // Propriedades do objeto
    QSpinBox* m_objectIdSpinBox;
    QLineEdit* m_nameEdit;
    QSpinBox* m_modelIdSpinBox;
    QSpinBox* m_posXSpinBox;
    QSpinBox* m_posYSpinBox;
    QSpinBox* m_posZSpinBox;
    QSpinBox* m_rotationSpinBox;
    QSpinBox* m_scaleSpinBox;
    QCheckBox* m_collidableCheckBox;
    QCheckBox* m_interactableCheckBox;
    
    // Botões de ação
    QPushButton* m_newButton;
    QPushButton* m_updateButton;
    QPushButton* m_removeButton;
    
    bool m_updatingFields;
    int m_selectedModelId;
    std::optional<MapObject> m_currentObject;
    
    void createUI();
    void loadObjectModels();
};

/**
 * @brief Widget para edição de entidades (NPCs e mobs)
 */
class EntityEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    EntityEditorWidget(QWidget* parent = nullptr);
    
    void setMapEntity(const MapEntity& entity);
    void updateMapEntity(MapEntity& entity);
    
    int getSelectedEntityId() const;
    
signals:
    void entityChanged();
    void createEntity(const MapEntity& entity);
    void updateEntity(const MapEntity& entity);
    void removeEntity(int entityId);
    void selectEntity(int entityId);
    
private:
    // Lista de entidades disponíveis
    QTabWidget* m_entityTabWidget;
    QListWidget* m_npcList;
    QListWidget* m_mobList;
    
    // Propriedades da entidade
    QSpinBox* m_entityIdSpinBox;
    QComboBox* m_entityTypeComboBox;
    QSpinBox* m_entityRefIdSpinBox;
    QLineEdit* m_nameEdit;
    QSpinBox* m_posXSpinBox;
    QSpinBox* m_posYSpinBox;
    QSpinBox* m_directionSpinBox;
    QSpinBox* m_spawnRadiusSpinBox;
    QSpinBox* m_respawnTimeSpinBox;
    QSpinBox* m_maxInstancesSpinBox;
    
    // Botões de ação
    QPushButton* m_newButton;
    QPushButton* m_updateButton;
    QPushButton* m_removeButton;
    
    bool m_updatingFields;
    int m_selectedEntityId;
    std::optional<MapEntity> m_currentEntity;
    
    void createUI();
    void loadEntityLists();
};

/**
 * @brief Widget para edição de warps/portais
 */
class WarpEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    WarpEditorWidget(QWidget* parent = nullptr);
    
    void setMapWarp(const MapWarp& warp);
    void updateMapWarp(MapWarp& warp);
    
signals:
    void warpChanged();
    void createWarp(const MapWarp& warp);
    void updateWarp(const MapWarp& warp);
    void removeWarp(int warpId);
    void selectWarp(int warpId);
    
private:
    // Propriedades do warp
    QSpinBox* m_warpIdSpinBox;
    QLineEdit* m_nameEdit;
    QSpinBox* m_posXSpinBox;
    QSpinBox* m_posYSpinBox;
    QSpinBox* m_destMapIdSpinBox;
    QComboBox* m_destMapComboBox;
    QSpinBox* m_destXSpinBox;
    QSpinBox* m_destYSpinBox;
    QSpinBox* m_radiusSpinBox;
    QCheckBox* m_requireKeyItemCheckBox;
    QSpinBox* m_keyItemIdSpinBox;
    QCheckBox* m_requireLevelCheckBox;
    QSpinBox* m_requiredLevelSpinBox;
    QCheckBox* m_enabledCheckBox;
    
    // Botões de ação
    QPushButton* m_newButton;
    QPushButton* m_updateButton;
    QPushButton* m_removeButton;
    
    bool m_updatingFields;
    std::optional<MapWarp> m_currentWarp;
    
    void createUI();
    void loadMapList();
};

/**
 * @brief Widget para edição de áreas especiais
 */
class AreaEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    AreaEditorWidget(QWidget* parent = nullptr);
    
    void setMapArea(const MapArea& area);
    void updateMapArea(MapArea& area);
    
signals:
    void areaChanged();
    void createArea(const MapArea& area);
    void updateArea(const MapArea& area);
    void removeArea(int areaId);
    void selectArea(int areaId);
    
private:
    // Propriedades da área
    QSpinBox* m_areaIdSpinBox;
    QLineEdit* m_nameEdit;
    QComboBox* m_typeComboBox;
    QSpinBox* m_posXSpinBox;
    QSpinBox* m_posYSpinBox;
    QSpinBox* m_widthSpinBox;
    QSpinBox* m_heightSpinBox;
    QTextEdit* m_propertiesEdit;
    
    // Botões de ação
    QPushButton* m_newButton;
    QPushButton* m_updateButton;
    QPushButton* m_removeButton;
    
    bool m_updatingFields;
    std::optional<MapArea> m_currentArea;
    
    void createUI();
};

/**
 * @brief Widget principal para o editor de mapas
 */
class MapDesignerView : public QWidget {
    Q_OBJECT
    
public:
    MapDesignerView(QWidget* parent = nullptr);
    ~MapDesignerView();
    
    void initialize(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    // Estado do mapa
    void setMapData(const MapData& mapData);
    MapData getMapData() const;
    
public slots:
    // Gerais
    void onNewMap();
    void onOpenMap();
    void onSaveMap();
    void onSaveMapAs();
    void onCloseMap();
    
    // Edição
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    
    // Modo de edição
    void onTerrainMode();
    void onObjectsMode();
    void onEntitiesMode();
    void onWarpsMode();
    void onAreasMode();
    void onCollisionMode();
    void onClimateMode();
    void onLightingMode();
    
    // Visualização
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void onToggleGrid();
    void onToggleTextures();
    void onToggleHeightmap();
    void onToggleAttributes();
    void onToggleObjects();
    void onToggleEntities();
    void onToggleWarps();
    void onToggleAreas();
    
    // Ferramentas de terreno
    void onTerrainToolChanged(MapTerrainTool tool);
    void onBrushSizeChanged(int size);
    void onBrushValueChanged(int value);
    void onTextureSelected(int textureId);
    void onAttributeSelected(MapAttribute attribute);
    void onImportHeightmap();
    void onExportHeightmap();
    
    // Objetos
    void onObjectSelected(const MapObject& object);
    void onCreateObject(const MapObject& object);
    void onUpdateObject(const MapObject& object);
    void onRemoveObject(int objectId);
    
    // Entidades
    void onEntitySelected(const MapEntity& entity);
    void onCreateEntity(const MapEntity& entity);
    void onUpdateEntity(const MapEntity& entity);
    void onRemoveEntity(int entityId);
    
    // Warps
    void onWarpSelected(const MapWarp& warp);
    void onCreateWarp(const MapWarp& warp);
    void onUpdateWarp(const MapWarp& warp);
    void onRemoveWarp(int warpId);
    
    // Áreas
    void onAreaSelected(const MapArea& area);
    void onCreateArea(const MapArea& area);
    void onUpdateArea(const MapArea& area);
    void onRemoveArea(int areaId);
    
    // Propriedades do mapa
    void onPropertiesChanged();
    
    // Aplicações
    void onTestMap();
    void onApplyToServer();
    void onImportMap();
    void onExportMap();
    
private:
    // Core
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    std::shared_ptr<MapDesigner> m_mapDesigner;
    
    // Dados
    MapData m_mapData;
    std::string m_currentFilePath;
    bool m_modified;
    
    // UI Components
    QToolBar* m_toolbar;
    QStatusBar* m_statusBar;
    QSplitter* m_mainSplitter;
    
    // Vista principal do mapa
    MapGraphicsView* m_mapView;
    MapGraphicsScene* m_mapScene;
    
    // Painéis de edição
    QDockWidget* m_editorDock;
    QTabWidget* m_editorTabs;
    
    MapPropertiesWidget* m_propertiesWidget;
    TerrainEditorWidget* m_terrainWidget;
    ObjectEditorWidget* m_objectWidget;
    EntityEditorWidget* m_entityWidget;
    WarpEditorWidget* m_warpWidget;
    AreaEditorWidget* m_areaWidget;
    QWidget* m_collisionWidget;
    QWidget* m_climateWidget;
    QWidget* m_lightingWidget;
    
    // Lista de mapas e objetos
    QDockWidget* m_mapListDock;
    QTreeWidget* m_mapList;
    
    // Janela de visualização da miniatura
    QDockWidget* m_minimapDock;
    QLabel* m_minimapLabel;
    
    // Histórico de desfazer/refazer
    QUndoStack* m_undoStack;
    
    // Inicialização da interface
    void createUI();
    void setupToolbar();
    void setupMapView();
    void setupEditorPanels();
    void setupDockWidgets();
    
    // Gerenciamento de mapas
    void loadMap(int mapId);
    bool saveMap();
    bool saveMapAs(const std::string& filePath);
    void updateMapViews();
    void generateMinimap();
    
    // Gestão de edição
    void setEditMode(MapEditMode mode);
    void showEditorTab(int tabIndex);
    
    // Utilidades
    void showMessage(const QString& message);
    void showError(const QString& title, const QString& message);
    bool confirmDiscardChanges();
    void updateWindowTitle();
    void setModified(bool modified);
};

}}} // namespace wydstudio::ui::visual

#endif // WYDSTUDIO_MAP_DESIGNER_VIEW_H