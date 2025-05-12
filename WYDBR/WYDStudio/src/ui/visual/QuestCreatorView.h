#ifndef WYDSTUDIO_QUEST_CREATOR_VIEW_H
#define WYDSTUDIO_QUEST_CREATOR_VIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QTabWidget>
#include <QDockWidget>
#include <QUndoStack>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QStatusBar>
#include <QScrollArea>
#include <QSplitter>
#include <QGridLayout>
#include <QTextEdit>
#include <QTreeWidget>
#include <QMenu>
#include <QAction>
#include <memory>
#include <vector>
#include <map>

#include "../QuestCreator.h"
#include "../../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {
namespace visual {

// Forward declarations
class QuestNodeItem;
class QuestGraphicsScene;
class QuestConnectionItem;

/**
 * @brief Tipos de nós para o fluxograma de quest
 */
enum class QuestNodeType {
    START,         // Nó de início
    END,           // Nó de finalização
    DIALOG,        // Diálogo com NPC
    OBJECTIVE,     // Objetivo de missão
    CONDITION,     // Condição
    REWARD,        // Recompensa
    BRANCH,        // Ramificação
    SCRIPT,        // Script personalizado
    EVENT,         // Evento
    TIMER,         // Temporizador
    GROUP          // Grupo de nós
};

/**
 * @brief Item gráfico representando um nó do fluxograma de quest
 */
class QuestNodeItem : public QGraphicsItem {
public:
    QuestNodeItem(QuestNode node, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    const QuestNode& getNode() const;
    void updateNode(const QuestNode& node);
    
    void addInputConnector(const QString& name);
    void addOutputConnector(const QString& name);
    
    QPointF getInputPosition(const QString& name) const;
    QPointF getOutputPosition(const QString& name) const;
    
    // Para seleção e edição
    void setSelected(bool selected);
    bool isSelected() const;
    
    // Para conexões
    void addConnection(QuestConnectionItem* connection, bool isOutput);
    void removeConnection(QuestConnectionItem* connection);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    
private:
    QuestNode m_node;
    bool m_selected;
    bool m_dragging;
    QPointF m_dragStartPos;
    bool m_hover;
    
    // Conectores de entrada e saída
    struct Connector {
        QString name;
        QRectF rect;
        bool highlighted;
    };
    
    std::vector<Connector> m_inputConnectors;
    std::vector<Connector> m_outputConnectors;
    
    // Conexões
    std::vector<QuestConnectionItem*> m_inputConnections;
    std::vector<QuestConnectionItem*> m_outputConnections;
    
    // Métodos auxiliares
    QColor getNodeColor() const;
    QIcon getNodeIcon() const;
    void updateConnectorPositions();
    int connectorAtPosition(const QPointF& pos, bool& isOutput);
};

/**
 * @brief Item gráfico representando uma conexão entre nós
 */
class QuestConnectionItem : public QGraphicsItem {
public:
    QuestConnectionItem(QuestNodeItem* sourceNode, const QString& sourceConnector,
                         QuestNodeItem* targetNode, const QString& targetConnector,
                         QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    // Acesso aos nós e conectores
    QuestNodeItem* getSourceNode() const;
    QuestNodeItem* getTargetNode() const;
    QString getSourceConnector() const;
    QString getTargetConnector() const;
    
    // Atualiza posição baseado nos nós
    void updatePosition();
    
    // Para seleção
    void setSelected(bool selected);
    bool isSelected() const;
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    
private:
    QuestNodeItem* m_sourceNode;
    QuestNodeItem* m_targetNode;
    QString m_sourceConnector;
    QString m_targetConnector;
    
    bool m_selected;
    bool m_hover;
    
    // Calculos de bezier para a linha
    QPainterPath createConnectionPath() const;
};

/**
 * @brief Cena gráfica para o editor de quests
 */
class QuestGraphicsScene : public QGraphicsScene {
    Q_OBJECT
    
public:
    QuestGraphicsScene(QObject* parent = nullptr);
    
    void setQuestData(const QuestData& questData);
    void updateQuestData(QuestData& questData);
    void clear();
    
    // Manipulação de nós
    QuestNodeItem* addNode(const QuestNode& node);
    void updateNode(const QuestNode& node);
    void removeNode(int nodeId);
    
    // Manipulação de conexões
    QuestConnectionItem* addConnection(int sourceNodeId, const QString& sourceConnector,
                                       int targetNodeId, const QString& targetConnector);
    void removeConnection(int sourceNodeId, const QString& sourceConnector,
                          int targetNodeId, const QString& targetConnector);
    
    // Seleção
    QuestNodeItem* getNodeItemById(int nodeId) const;
    QuestNodeItem* getSelectedNodeItem() const;
    QuestConnectionItem* getSelectedConnectionItem() const;
    
    void clearSelection();
    
signals:
    void nodeSelected(const QuestNode& node);
    void connectionSelected(int sourceNodeId, const QString& sourceConnector,
                            int targetNodeId, const QString& targetConnector);
    void nodePositionChanged(int nodeId, QPointF position);
    void sceneClicked();
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    
private:
    QuestData m_questData;
    
    std::map<int, QuestNodeItem*> m_nodeItems;
    std::vector<QuestConnectionItem*> m_connectionItems;
    
    // Estado de edição
    bool m_isCreatingConnection;
    QuestNodeItem* m_connectionSourceNode;
    QString m_connectionSourceConnector;
    QGraphicsPathItem* m_tempConnectionItem;
    
    // Estado de seleção
    QGraphicsItem* m_selectedItem;
    
    // Métodos auxiliares
    void handleConnectionCreation(const QPointF& pos);
    void startConnectionCreation(QuestNodeItem* sourceNode, const QString& connectorName);
    void finishConnectionCreation(QuestNodeItem* targetNode, const QString& connectorName);
    void cancelConnectionCreation();
    void updateTempConnection(const QPointF& pos);
};

/**
 * @brief View gráfica para o editor de quests
 */
class QuestGraphicsView : public QGraphicsView {
    Q_OBJECT
    
public:
    QuestGraphicsView(QWidget* parent = nullptr);
    
    void setScene(QuestGraphicsScene* scene);
    
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
    void keyPressEvent(QKeyEvent* event) override;
    
private:
    qreal m_zoom;
    bool m_isPanning;
    QPoint m_lastPanPos;
};

/**
 * @brief Widget para edição de propriedades gerais da quest
 */
class QuestPropertiesWidget : public QWidget {
    Q_OBJECT
    
public:
    QuestPropertiesWidget(QWidget* parent = nullptr);
    
    void setQuestData(const QuestData& questData);
    void updateQuestData(QuestData& questData);
    
signals:
    void propertiesChanged();
    
private:
    // Campos de edição
    QLineEdit* m_idEdit;
    QLineEdit* m_nameEdit;
    QComboBox* m_typeComboBox;
    QSpinBox* m_levelSpinBox;
    QSpinBox* m_minLevelSpinBox;
    QSpinBox* m_maxLevelSpinBox;
    QTextEdit* m_descriptionEdit;
    QCheckBox* m_repeatablecheckBox;
    QSpinBox* m_cooldownSpinBox;
    QComboBox* m_factionComboBox;
    QSpinBox* m_factionPointsSpinBox;
    
    void createUI();
};

/**
 * @brief Widget para edição de nós de diálogo
 */
class DialogNodeEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    DialogNodeEditorWidget(QWidget* parent = nullptr);
    
    void setNode(const QuestNode& node);
    void updateNode(QuestNode& node);
    
signals:
    void nodeChanged();
    
private:
    QSpinBox* m_npcIdSpinBox;
    QComboBox* m_npcComboBox;
    QTextEdit* m_dialogTextEdit;
    
    // Opções de resposta
    struct ResponseWidget {
        QLineEdit* textEdit;
        QLineEdit* outputConnectorEdit;
        QPushButton* removeButton;
    };
    
    std::vector<ResponseWidget> m_responseWidgets;
    QWidget* m_responsesContainer;
    QPushButton* m_addResponseButton;
    
    bool m_updatingFields;
    
    void createUI();
    void addResponseWidget(const QString& text = "", const QString& connector = "");
    void updateResponseWidgets();
};

/**
 * @brief Widget para edição de nós de objetivo
 */
class ObjectiveNodeEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    ObjectiveNodeEditorWidget(QWidget* parent = nullptr);
    
    void setNode(const QuestNode& node);
    void updateNode(QuestNode& node);
    
signals:
    void nodeChanged();
    
private:
    QComboBox* m_objectiveTypeComboBox;
    QSpinBox* m_targetIdSpinBox;
    QComboBox* m_targetComboBox;
    QSpinBox* m_quantitySpinBox;
    QCheckBox* m_trackableCheckBox;
    QTextEdit* m_descriptionEdit;
    QLineEdit* m_locationEdit;
    QCheckBox* m_timeConstraintCheckBox;
    QSpinBox* m_timeMinutesSpinBox;
    
    void createUI();
    void updateTargetCombo(QuestObjectiveType type);
};

/**
 * @brief Widget para edição de nós de condição
 */
class ConditionNodeEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    ConditionNodeEditorWidget(QWidget* parent = nullptr);
    
    void setNode(const QuestNode& node);
    void updateNode(QuestNode& node);
    
signals:
    void nodeChanged();
    
private:
    QComboBox* m_conditionTypeComboBox;
    QSpinBox* m_conditionValueSpinBox;
    QComboBox* m_conditionComparison;
    QLineEdit* m_trueOutputEdit;
    QLineEdit* m_falseOutputEdit;
    QTextEdit* m_descriptionEdit;
    
    void createUI();
};

/**
 * @brief Widget para edição de nós de recompensa
 */
class RewardNodeEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    RewardNodeEditorWidget(QWidget* parent = nullptr);
    
    void setNode(const QuestNode& node);
    void updateNode(QuestNode& node);
    
signals:
    void nodeChanged();
    
private:
    QCheckBox* m_giveExpCheckBox;
    QSpinBox* m_expAmountSpinBox;
    
    QCheckBox* m_giveGoldCheckBox;
    QSpinBox* m_goldAmountSpinBox;
    
    QCheckBox* m_giveFactionCheckBox;
    QComboBox* m_factionComboBox;
    QSpinBox* m_factionPointsSpinBox;
    
    // Recompensas de item
    struct ItemRewardWidget {
        QSpinBox* itemIdSpinBox;
        QComboBox* itemComboBox;
        QSpinBox* quantitySpinBox;
        QCheckBox* boundCheckBox;
        QPushButton* removeButton;
    };
    
    std::vector<ItemRewardWidget> m_itemRewardWidgets;
    QWidget* m_itemRewardsContainer;
    QPushButton* m_addItemRewardButton;
    
    void createUI();
    void addItemRewardWidget(int itemId = 0, int quantity = 1, bool bound = false);
    void updateItemRewardWidgets();
};

/**
 * @brief Widget para edição de nós de script
 */
class ScriptNodeEditorWidget : public QWidget {
    Q_OBJECT
    
public:
    ScriptNodeEditorWidget(QWidget* parent = nullptr);
    
    void setNode(const QuestNode& node);
    void updateNode(QuestNode& node);
    
signals:
    void nodeChanged();
    void testScript(const QString& script);
    
private:
    QTextEdit* m_scriptEdit;
    QLineEdit* m_outputConnectorEdit;
    QPushButton* m_testButton;
    
    void createUI();
};

/**
 * @brief Widget para administração de variáveis de quest
 */
class QuestVariablesWidget : public QWidget {
    Q_OBJECT
    
public:
    QuestVariablesWidget(QWidget* parent = nullptr);
    
    void setQuestData(const QuestData& questData);
    void updateQuestData(QuestData& questData);
    
signals:
    void variablesChanged();
    
private:
    struct VariableWidget {
        QLineEdit* nameEdit;
        QComboBox* typeComboBox;
        QLineEdit* defaultValueEdit;
        QCheckBox* persistentCheckBox;
        QPushButton* removeButton;
    };
    
    std::vector<VariableWidget> m_variableWidgets;
    QWidget* m_variablesContainer;
    QPushButton* m_addVariableButton;
    
    void createUI();
    void addVariableWidget(const QString& name = "", QuestVariableType type = QuestVariableType::NUMBER, 
                            const QString& defaultValue = "", bool persistent = false);
    void updateVariableWidgets();
};

/**
 * @brief Widget para administração de NPCs relacionados à quest
 */
class QuestNPCsWidget : public QWidget {
    Q_OBJECT
    
public:
    QuestNPCsWidget(QWidget* parent = nullptr);
    
    void setQuestData(const QuestData& questData);
    void updateQuestData(QuestData& questData);
    
signals:
    void npcsChanged();
    
private:
    QListWidget* m_npcList;
    QPushButton* m_addNpcButton;
    QPushButton* m_removeNpcButton;
    
    // Campos de detalhe do NPC
    QWidget* m_detailsWidget;
    QSpinBox* m_npcIdSpinBox;
    QLineEdit* m_npcNameEdit;
    QComboBox* m_roleComboBox;
    QCheckBox* m_temporaryCheckBox;
    QCheckBox* m_visibleWithQuestCheckBox;
    
    void createUI();
    void loadNPCs();
    void updateNpcDetails(int index);
};

/**
 * @brief Widget para configuração de eventos de quest
 */
class QuestEventsWidget : public QWidget {
    Q_OBJECT
    
public:
    QuestEventsWidget(QWidget* parent = nullptr);
    
    void setQuestData(const QuestData& questData);
    void updateQuestData(QuestData& questData);
    
signals:
    void eventsChanged();
    
private:
    struct EventWidget {
        QComboBox* typeComboBox;
        QLineEdit* targetEdit;
        QTextEdit* scriptEdit;
        QPushButton* removeButton;
    };
    
    std::vector<EventWidget> m_eventWidgets;
    QWidget* m_eventsContainer;
    QPushButton* m_addEventButton;
    
    void createUI();
    void addEventWidget(QuestEventType type = QuestEventType::QUEST_START, 
                          const QString& target = "", const QString& script = "");
    void updateEventWidgets();
};

/**
 * @brief Paleta de nós para adicionar ao fluxograma
 */
class QuestNodePalette : public QWidget {
    Q_OBJECT
    
public:
    QuestNodePalette(QWidget* parent = nullptr);
    
signals:
    void nodeTypeSelected(QuestNodeType type);
    
private:
    QListWidget* m_nodeList;
    
    void createUI();
    void setupNodeList();
};

/**
 * @brief Widget principal para o editor de quests
 */
class QuestCreatorView : public QWidget {
    Q_OBJECT
    
public:
    QuestCreatorView(QWidget* parent = nullptr);
    ~QuestCreatorView();
    
    void initialize(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
public slots:
    // Gerais
    void onNewQuest();
    void onOpenQuest();
    void onSaveQuest();
    void onSaveQuestAs();
    void onCloseQuest();
    
    // Edição
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    
    // Visualização
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void onToggleGrid();
    void onToggleMinimap();
    void onToggleProperties();
    
    // Nós
    void onAddNode(QuestNodeType type);
    void onNodeSelected(const QuestNode& node);
    void onNodePositionChanged(int nodeId, QPointF position);
    void onNodeChanged();
    
    // Conexões
    void onConnectionSelected(int sourceNodeId, const QString& sourceConnector,
                             int targetNodeId, const QString& targetConnector);
    
    // Dados da quest
    void onPropertiesChanged();
    void onVariablesChanged();
    void onNpcsChanged();
    void onEventsChanged();
    void onSceneClicked();
    
    // Ferramentas
    void onValidateQuest();
    void onSimulateQuest();
    void onGenerateDocumentation();
    void onExportQuestScript();
    
    // Aplicar ao servidor
    void onApplyToServer();
    
private:
    // Core
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    std::shared_ptr<QuestCreator> m_questCreator;
    
    // Dados
    QuestData m_questData;
    std::string m_currentFilePath;
    bool m_modified;
    QuestNode m_selectedNode;
    
    // UI Components
    QToolBar* m_toolbar;
    QStatusBar* m_statusBar;
    QSplitter* m_mainSplitter;
    
    // Vista principal do fluxograma
    QuestGraphicsView* m_graphView;
    QuestGraphicsScene* m_graphScene;
    
    // Painéis de edição
    QTabWidget* m_propertiesTabs;
    QuestPropertiesWidget* m_propertiesWidget;
    QuestVariablesWidget* m_variablesWidget;
    QuestNPCsWidget* m_npcsWidget;
    QuestEventsWidget* m_eventsWidget;
    
    // Painel de nó selecionado
    QDockWidget* m_nodeEditorDock;
    QStackedWidget* m_nodeEditorStack;
    DialogNodeEditorWidget* m_dialogNodeEditor;
    ObjectiveNodeEditorWidget* m_objectiveNodeEditor;
    ConditionNodeEditorWidget* m_conditionNodeEditor;
    RewardNodeEditorWidget* m_rewardNodeEditor;
    ScriptNodeEditorWidget* m_scriptNodeEditor;
    QLabel* m_noNodeSelectedLabel;
    
    // Paleta de nós
    QDockWidget* m_nodePaletteDock;
    QuestNodePalette* m_nodePalette;
    
    // Lista de quests
    QDockWidget* m_questListDock;
    QTreeWidget* m_questList;
    
    // Miniatura e navegação
    QDockWidget* m_minimapDock;
    QGraphicsView* m_minimapView;
    
    // Histórico de desfazer/refazer
    QUndoStack* m_undoStack;
    
    // Inicialização da interface
    void createUI();
    void setupToolbar();
    void setupGraphView();
    void setupPropertyPanels();
    void setupNodeEditors();
    void setupDockWidgets();
    
    // Gerenciamento de quests
    void loadQuest(int questId);
    bool saveQuest();
    bool saveQuestAs(const std::string& filePath);
    void updateUI();
    void updateNodeEditor();
    
    // Utilidades
    void showMessage(const QString& message);
    void showError(const QString& title, const QString& message);
    bool confirmDiscardChanges();
    void updateWindowTitle();
    void setModified(bool modified);
    
    // Geração de novos nós
    int generateUniqueNodeId();
    QuestNode createDefaultNode(QuestNodeType type);
};

}}} // namespace wydstudio::ui::visual

#endif // WYDSTUDIO_QUEST_CREATOR_VIEW_H