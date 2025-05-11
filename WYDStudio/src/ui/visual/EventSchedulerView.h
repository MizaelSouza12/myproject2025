#ifndef WYDSTUDIO_EVENT_SCHEDULER_VIEW_H
#define WYDSTUDIO_EVENT_SCHEDULER_VIEW_H

#include <QWidget>
#include <QCalendarWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QStatusBar>
#include <QScrollArea>
#include <QSplitter>
#include <QGridLayout>
#include <QTextEdit>
#include <QTabWidget>
#include <QTreeWidget>
#include <QMenu>
#include <QAction>
#include <QTimeEdit>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <memory>
#include <vector>
#include <map>

#include "../EventScheduler.h"
#include "../../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {
namespace visual {

/**
 * @brief Visualização do calendário de eventos
 */
class EventCalendarView : public QWidget {
    Q_OBJECT
    
public:
    EventCalendarView(QWidget* parent = nullptr);
    
    void setEvents(const std::vector<GameEventModel>& events);
    void selectDate(const QDate& date);
    QDate selectedDate() const;
    
signals:
    void dateSelected(const QDate& date);
    void eventSelected(int eventId);
    
private:
    QCalendarWidget* m_calendar;
    QListWidget* m_eventList;
    
    std::map<QDate, std::vector<GameEventModel>> m_eventsByDate;
    
    void createUI();
    void updateEventList(const QDate& date);
    void paintCell(QDate date, bool hasEvents, EventRecurrence recurrence);
};

/**
 * @brief Delegate personalizado para células do calendário
 */
class EventCalendarDelegate : public QCalendarWidget {
    Q_OBJECT
    
public:
    EventCalendarDelegate(QWidget* parent = nullptr);
    
    void setEventDates(const std::map<QDate, std::vector<GameEventModel>>& events);
    
protected:
    void paintCell(QPainter* painter, const QRect& rect, const QDate& date) const override;
    
private:
    std::map<QDate, std::vector<GameEventModel>> m_events;
};

/**
 * @brief Widget para edição de temporizador de evento
 */
class EventTimerWidget : public QWidget {
    Q_OBJECT
    
public:
    EventTimerWidget(QWidget* parent = nullptr);
    
    void setEventTimer(const EventTimer& timer);
    void updateEventTimer(EventTimer& timer);
    
signals:
    void timerChanged();
    
private:
    QDateTimeEdit* m_startDateTimeEdit;
    QDateTimeEdit* m_endDateTimeEdit;
    QSpinBox* m_durationMinutesSpinBox;
    QCheckBox* m_hasCountdownCheckBox;
    QSpinBox* m_countdownMinutesSpinBox;
    QCheckBox* m_autoStartCheckBox;
    QCheckBox* m_autoEndCheckBox;
    
    // Recorrência
    QGroupBox* m_recurrenceGroupBox;
    QComboBox* m_recurrenceTypeComboBox;
    QLineEdit* m_recurrencePatternEdit;
    QTimeEdit* m_recurrenceTimeEdit;
    QSpinBox* m_recurrenceIntervalSpinBox;
    QDateEdit* m_recurrenceEndDateEdit;
    
    // Ajustes de fuso horário
    QCheckBox* m_adjustForServerTimeCheckBox;
    QComboBox* m_timeZoneComboBox;
    
    void createUI();
    void updateRecurrenceUI();
    void loadTimeZones();
};

/**
 * @brief Widget para edição de notificação de evento
 */
class EventNotificationWidget : public QWidget {
    Q_OBJECT
    
public:
    EventNotificationWidget(QWidget* parent = nullptr);
    
    void setNotification(const EventNotification& notification);
    void updateNotification(EventNotification& notification);
    
signals:
    void notificationChanged();
    
private:
    // Tipo e conteúdo
    QComboBox* m_typeComboBox;
    QLineEdit* m_titleEdit;
    QTextEdit* m_messageEdit;
    
    // Tempo de notificação
    struct TimingWidget {
        QSpinBox* minutesSpinBox;
        QPushButton* removeButton;
    };
    std::vector<TimingWidget> m_timingWidgets;
    QWidget* m_timingContainer;
    QPushButton* m_addTimingButton;
    
    // Flags de notificação
    QCheckBox* m_notifyOnStartCheckBox;
    QCheckBox* m_notifyOnEndCheckBox;
    QCheckBox* m_notifyOnChangeCheckBox;
    
    // Recursos
    QLineEdit* m_iconPathEdit;
    QPushButton* m_browseIconButton;
    QLineEdit* m_soundPathEdit;
    QPushButton* m_browseSoundButton;
    
    // Alvos da notificação
    QGroupBox* m_targetGroupBox;
    QCheckBox* m_allPlayersCheckBox;
    QSpinBox* m_minLevelSpinBox;
    QSpinBox* m_maxLevelSpinBox;
    QListWidget* m_specificPlayersList;
    QPushButton* m_addPlayerButton;
    QPushButton* m_removePlayerButton;
    QListWidget* m_factionsList;
    QListWidget* m_guildsList;
    QListWidget* m_classesList;
    QLineEdit* m_customTargetLogicEdit;
    
    void createUI();
    void addTimingWidget(int minutes = 5);
    void updateTimingWidgets();
    void updateTargetsUI();
};

/**
 * @brief Widget para edição de estágio de evento
 */
class EventStageWidget : public QWidget {
    Q_OBJECT
    
public:
    EventStageWidget(QWidget* parent = nullptr);
    
    void setStage(const EventStage& stage);
    void updateStage(EventStage& stage);
    
signals:
    void stageChanged();
    
private:
    // Propriedades básicas
    QSpinBox* m_stageIdSpinBox;
    QLineEdit* m_nameEdit;
    QTextEdit* m_descriptionEdit;
    QSpinBox* m_durationMinutesSpinBox;
    QCheckBox* m_isOptionalCheckBox;
    
    // Condições para avançar
    QGroupBox* m_completionGroupBox;
    QComboBox* m_completionTypeComboBox;
    QLineEdit* m_completionTargetEdit;
    QSpinBox* m_completionQuantitySpinBox;
    QCheckBox* m_sharedProgressCheckBox;
    QTextEdit* m_completionLogicEdit;
    
    // Elementos do estágio
    QGroupBox* m_elementsGroupBox;
    
    // Mobs do estágio
    struct MobWidget {
        QSpinBox* mobIdSpinBox;
        QComboBox* mobComboBox;
        QPushButton* removeButton;
    };
    std::vector<MobWidget> m_mobWidgets;
    QWidget* m_mobsContainer;
    QPushButton* m_addMobButton;
    
    // NPCs do estágio
    struct NpcWidget {
        QSpinBox* npcIdSpinBox;
        QComboBox* npcComboBox;
        QPushButton* removeButton;
    };
    std::vector<NpcWidget> m_npcWidgets;
    QWidget* m_npcsContainer;
    QPushButton* m_addNpcButton;
    
    // Portais do estágio
    struct PortalWidget {
        QSpinBox* portalIdSpinBox;
        QComboBox* portalComboBox;
        QPushButton* removeButton;
    };
    std::vector<PortalWidget> m_portalWidgets;
    QWidget* m_portalsContainer;
    QPushButton* m_addPortalButton;
    
    // Mudanças no mundo
    struct WorldChangeWidget {
        QLineEdit* changeEdit;
        QPushButton* removeButton;
    };
    std::vector<WorldChangeWidget> m_worldChangeWidgets;
    QWidget* m_worldChangesContainer;
    QPushButton* m_addWorldChangeButton;
    
    // Actions com script
    struct ScriptedActionWidget {
        QLineEdit* nameEdit;
        QTextEdit* scriptEdit;
        QPushButton* removeButton;
    };
    std::vector<ScriptedActionWidget> m_scriptedActionWidgets;
    QWidget* m_scriptedActionsContainer;
    QPushButton* m_addScriptedActionButton;
    
    // Recompensas do estágio
    QGroupBox* m_rewardsGroupBox;
    
    // Recompensas de item
    struct ItemRewardWidget {
        QSpinBox* itemIdSpinBox;
        QComboBox* itemComboBox;
        QSpinBox* quantitySpinBox;
        QPushButton* removeButton;
    };
    std::vector<ItemRewardWidget> m_itemRewardWidgets;
    QWidget* m_itemRewardsContainer;
    QPushButton* m_addItemRewardButton;
    
    // Outras recompensas
    QSpinBox* m_goldRewardSpinBox;
    QSpinBox* m_expRewardSpinBox;
    
    // Recompensas especiais
    struct SpecialRewardWidget {
        QLineEdit* rewardEdit;
        QPushButton* removeButton;
    };
    std::vector<SpecialRewardWidget> m_specialRewardWidgets;
    QWidget* m_specialRewardsContainer;
    QPushButton* m_addSpecialRewardButton;
    
    void createUI();
    void updateCompletionUI();
    
    // Helpers para widgets dinâmicos
    void addMobWidget(int mobId = 0);
    void updateMobWidgets();
    
    void addNpcWidget(int npcId = 0);
    void updateNpcWidgets();
    
    void addPortalWidget(int portalId = 0);
    void updatePortalWidgets();
    
    void addWorldChangeWidget(const QString& change = "");
    void updateWorldChangeWidgets();
    
    void addScriptedActionWidget(const QString& name = "", const QString& script = "");
    void updateScriptedActionWidgets();
    
    void addItemRewardWidget(int itemId = 0, int quantity = 1);
    void updateItemRewardWidgets();
    
    void addSpecialRewardWidget(const QString& reward = "");
    void updateSpecialRewardWidgets();
};

/**
 * @brief Widget para edição de recompensas de evento
 */
class EventRewardWidget : public QWidget {
    Q_OBJECT
    
public:
    EventRewardWidget(QWidget* parent = nullptr);
    
    void setReward(const EventReward& reward);
    void updateReward(EventReward& reward);
    
signals:
    void rewardChanged();
    
private:
    // Tipo e propriedades básicas
    QComboBox* m_typeComboBox;
    QSpinBox* m_resourceIdSpinBox;
    QComboBox* m_resourceComboBox;
    QLineEdit* m_resourceNameEdit;
    QSpinBox* m_quantitySpinBox;
    
    // Condições
    QGroupBox* m_conditionsGroupBox;
    QSpinBox* m_minParticipationPointsSpinBox;
    QCheckBox* m_requireCompletionCheckBox;
    QCheckBox* m_topPlayersOnlyCheckBox;
    QSpinBox* m_topPlayerCountSpinBox;
    QLineEdit* m_customConditionEdit;
    
    // Distribuição
    QGroupBox* m_distributionGroupBox;
    QCheckBox* m_scaleByContributionCheckBox;
    QCheckBox* m_equalDistributionCheckBox;
    QCheckBox* m_randomChanceCheckBox;
    QDoubleSpinBox* m_dropChanceSpinBox;
    QLineEdit* m_distributionLogicEdit;
    
    void createUI();
    void updateTypeUI();
};

/**
 * @brief Widget para exibição de estatísticas de evento
 */
class EventStatisticsWidget : public QWidget {
    Q_OBJECT
    
public:
    EventStatisticsWidget(QWidget* parent = nullptr);
    
    void setEventStatistics(const EventStatistics& stats);
    
private:
    // Estatísticas por tipo
    QTableWidget* m_typeStatsTable;
    
    // Gráficos de participação
    QGraphicsView* m_timeStatsView;
    QGraphicsScene* m_timeStatsScene;
    
    // Estatísticas de jogadores
    QTableWidget* m_playerStatsTable;
    
    // Estatísticas adicionais
    QTextEdit* m_additionalStatsEdit;
    
    void createUI();
    void updateTypeStatsTable(const EventStatistics& stats);
    void updateTimeStatsGraph(const EventStatistics& stats);
    void updatePlayerStatsTable(const EventStatistics& stats);
    void updateAdditionalStats(const EventStatistics& stats);
};

/**
 * @brief Widget para visualização de participação em evento
 */
class EventParticipationWidget : public QWidget {
    Q_OBJECT
    
public:
    EventParticipationWidget(QWidget* parent = nullptr);
    
    void setParticipationStats(const EventParticipationStats& stats);
    
private:
    // Resumo geral
    QLabel* m_totalParticipantsLabel;
    QLabel* m_activeParticipantsLabel;
    QLabel* m_completedParticipantsLabel;
    
    // Distribuição de jogadores
    QGraphicsView* m_playerDistributionView;
    QGraphicsScene* m_playerDistributionScene;
    
    // Melhores jogadores
    QTableWidget* m_topPlayersTable;
    
    // Progresso de objetivos
    QTableWidget* m_objectivesProgressTable;
    
    void createUI();
    void updatePlayerDistributionGraph(const EventParticipationStats& stats);
    void updateTopPlayersTable(const EventParticipationStats& stats);
    void updateObjectivesProgressTable(const EventParticipationStats& stats);
};

/**
 * @brief Widget para agendamento de eventos recorrentes
 */
class RecurringEventSchedulerWidget : public QWidget {
    Q_OBJECT
    
public:
    RecurringEventSchedulerWidget(QWidget* parent = nullptr);
    
    void setEvents(const std::vector<GameEventModel>& events);
    
signals:
    void eventRecurrenceChanged(int eventId, EventRecurrence recurrence, const std::string& pattern);
    void eventScheduleChanged(int eventId, std::chrono::system_clock::time_point startTime);
    
private:
    QTableWidget* m_recurringEventsTable;
    
    void createUI();
    void updateEventsTable(const std::vector<GameEventModel>& events);
};

/**
 * @brief Widget principal para o agendador de eventos
 */
class EventSchedulerView : public QWidget {
    Q_OBJECT
    
public:
    EventSchedulerView(QWidget* parent = nullptr);
    ~EventSchedulerView();
    
    void initialize(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
public slots:
    // Gerais
    void onNewEvent();
    void onOpenEvent();
    void onSaveEvent();
    void onSaveEventAs();
    void onCloseEvent();
    
    // Eventos
    void onStartEvent();
    void onStopEvent();
    void onPauseEvent();
    void onResumeEvent();
    void onRestartEvent();
    
    // Agendamento
    void onScheduleEvent();
    void onSetRecurrence();
    
    // Calendário
    void onDateSelected(const QDate& date);
    void onCalendarEventSelected(int eventId);
    
    // Edição do evento
    void onEventSelected(int eventId);
    void onEventNameChanged(const QString& name);
    void onEventTypeChanged(int index);
    void onEventPropertiesChanged();
    
    // Estágios
    void onAddStage();
    void onRemoveStage(int stageId);
    void onStageSelected(int stageId);
    void onStageChanged();
    void onStageOrderChanged();
    
    // Recompensas
    void onAddReward();
    void onRemoveReward(int rewardIndex);
    void onRewardSelected(int rewardIndex);
    void onRewardChanged();
    
    // Notificações
    void onAddNotification();
    void onRemoveNotification(int notificationIndex);
    void onNotificationSelected(int notificationIndex);
    void onNotificationChanged();
    
    // Temporizador
    void onTimerChanged();
    
    // Estatísticas
    void onRefreshStatistics();
    
    // Aplicação ao servidor
    void onApplyToServer();
    void onGenerateCalendar();
    void onExportEvents();
    void onImportEvents();
    
private:
    // Core
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    std::shared_ptr<EventScheduler> m_eventScheduler;
    
    // Dados
    GameEventModel m_currentEvent;
    int m_currentEventId;
    std::string m_currentFilePath;
    bool m_modified;
    
    // UI Components
    QToolBar* m_toolbar;
    QStatusBar* m_statusBar;
    QSplitter* m_mainSplitter;
    
    // Painel esquerdo - Calendário e lista
    QWidget* m_leftPanel;
    EventCalendarView* m_calendarView;
    QListWidget* m_upcomingEventsList;
    QListWidget* m_activeEventsList;
    QComboBox* m_eventTypeFilterComboBox;
    
    // Painel central - Editor de evento
    QTabWidget* m_eventEditorTabs;
    
    // Aba de informações gerais
    QWidget* m_generalTab;
    QLineEdit* m_nameEdit;
    QComboBox* m_typeComboBox;
    QLineEdit* m_displayNameEdit;
    QTextEdit* m_descriptionEdit;
    QTextEdit* m_shortDescriptionEdit;
    QSpinBox* m_minLevelSpinBox;
    QSpinBox* m_recommendedLevelSpinBox;
    QSpinBox* m_maxParticipantsSpinBox;
    QCheckBox* m_isGuildEventCheckBox;
    QCheckBox* m_isPvPEnabledCheckBox;
    QComboBox* m_categoryComboBox;
    QComboBox* m_difficultyComboBox;
    
    // Aba de temporizador
    EventTimerWidget* m_timerWidget;
    
    // Aba de localização
    QWidget* m_locationTab;
    QSpinBox* m_mapIdSpinBox;
    QComboBox* m_mapComboBox;
    QSpinBox* m_centerXSpinBox;
    QSpinBox* m_centerYSpinBox;
    QSpinBox* m_radiusSpinBox;
    QCheckBox* m_instancedCheckBox;
    QSpinBox* m_maxInstancesSpinBox;
    
    // Lista de áreas adicionais
    struct AreaWidget {
        QSpinBox* xSpinBox;
        QSpinBox* ySpinBox;
        QPushButton* removeButton;
    };
    std::vector<AreaWidget> m_areaWidgets;
    QWidget* m_areasContainer;
    QPushButton* m_addAreaButton;
    
    // Lista de mapas relacionados
    QListWidget* m_relatedMapsList;
    QPushButton* m_addRelatedMapButton;
    QPushButton* m_removeRelatedMapButton;
    
    // Aba de estágios
    QWidget* m_stagesTab;
    QListWidget* m_stagesList;
    QPushButton* m_addStageButton;
    QPushButton* m_removeStageButton;
    QPushButton* m_moveStageUpButton;
    QPushButton* m_moveStageDownButton;
    EventStageWidget* m_stageEditorWidget;
    
    // Aba de recompensas
    QWidget* m_rewardsTab;
    QListWidget* m_rewardsList;
    QPushButton* m_addRewardButton;
    QPushButton* m_removeRewardButton;
    EventRewardWidget* m_rewardEditorWidget;
    
    // Aba de entidades
    QWidget* m_entitiesTab;
    
    // NPCs
    QListWidget* m_npcsList;
    QPushButton* m_addNpcButton;
    QPushButton* m_removeNpcButton;
    
    // Mobs
    QListWidget* m_mobsList;
    QPushButton* m_addMobButton;
    QPushButton* m_removeMobButton;
    
    // Localizações de spawn
    QTableWidget* m_spawnLocationsTable;
    QPushButton* m_addSpawnButton;
    QPushButton* m_removeSpawnButton;
    
    // Aba de requisitos
    QWidget* m_requirementsTab;
    QCheckBox* m_hasRequirementsCheckBox;
    QSpinBox* m_requiredLevelSpinBox;
    
    // Items requeridos
    QListWidget* m_requiredItemsList;
    QPushButton* m_addRequiredItemButton;
    QPushButton* m_removeRequiredItemButton;
    
    // Quests requeridas
    QListWidget* m_requiredQuestsList;
    QPushButton* m_addRequiredQuestButton;
    QPushButton* m_removeRequiredQuestButton;
    
    // Outros requisitos
    QSpinBox* m_requiredGoldSpinBox;
    QSpinBox* m_requiredReputationSpinBox;
    
    // Classes permitidas
    QListWidget* m_allowedClassesList;
    
    // Requisitos de guild
    QCheckBox* m_guildRequiredCheckBox;
    QSpinBox* m_minGuildMembersSpinBox;
    QTextEdit* m_customRequirementsEdit;
    
    // Aba de pontuação
    QWidget* m_scoringTab;
    QCheckBox* m_hasScoringCheckBox;
    
    // Pontos por ação
    QTableWidget* m_actionPointsTable;
    QPushButton* m_addActionPointsButton;
    QPushButton* m_removeActionPointsButton;
    
    // Outras configurações de pontuação
    QCheckBox* m_displayLeaderboardCheckBox;
    QCheckBox* m_saveHighScoresCheckBox;
    QTextEdit* m_scoringLogicEdit;
    
    // Aba de regras
    QWidget* m_rulesTab;
    QCheckBox* m_allowLateJoinCheckBox;
    QCheckBox* m_allowReconnectCheckBox;
    QCheckBox* m_saveProgressOnDisconnectCheckBox;
    QCheckBox* m_resetOnFailCheckBox;
    QSpinBox* m_failCooldownMinutesSpinBox;
    QCheckBox* m_allowSoloAttemptCheckBox;
    
    // Regras personalizadas
    QTableWidget* m_customRulesTable;
    QPushButton* m_addCustomRuleButton;
    QPushButton* m_removeCustomRuleButton;
    
    // Aba de efeitos
    QWidget* m_effectsTab;
    QComboBox* m_weatherEffectComboBox;
    QComboBox* m_skyboxEffectComboBox;
    QComboBox* m_musicThemeComboBox;
    QComboBox* m_ambientSoundComboBox;
    
    // Efeitos mundiais
    QListWidget* m_worldEffectsList;
    QPushButton* m_addWorldEffectButton;
    QPushButton* m_removeWorldEffectButton;
    
    // Efeitos personalizados
    QTableWidget* m_customEffectsTable;
    QPushButton* m_addCustomEffectButton;
    QPushButton* m_removeCustomEffectButton;
    
    // Aba de notificações
    QWidget* m_notificationsTab;
    QListWidget* m_notificationsList;
    QPushButton* m_addNotificationButton;
    QPushButton* m_removeNotificationButton;
    EventNotificationWidget* m_notificationEditorWidget;
    
    // Painel direito - Estatísticas e histórico
    QTabWidget* m_rightPanelTabs;
    
    // Aba de próximos eventos
    RecurringEventSchedulerWidget* m_recurringSchedulerWidget;
    
    // Aba de estatísticas
    EventStatisticsWidget* m_statisticsWidget;
    
    // Aba de participação
    EventParticipationWidget* m_participationWidget;
    
    // Aba de histórico
    QWidget* m_historyTab;
    QTableWidget* m_runHistoryTable;
    
    // Inicialização da interface
    void createUI();
    void setupToolbar();
    void setupEventEditor();
    void setupLeftPanel();
    void setupRightPanel();
    
    // Gerenciamento de eventos
    void loadEvent(int eventId);
    bool saveEvent();
    bool saveEventAs(const std::string& filePath);
    void updateUI();
    void loadCurrentEventToUI();
    void saveCurrentEventFromUI();
    
    // Utilidades
    void showMessage(const QString& message);
    void showError(const QString& title, const QString& message);
    bool confirmDiscardChanges();
    void updateWindowTitle();
    void setModified(bool modified);
    void loadEventLists();
    
    // Helpers para widgets dinâmicos
    void addAreaWidget(int x = 0, int y = 0);
    void updateAreaWidgets();
    
    // Geração da UI baseada nos dados do evento
    void generateStagesUI();
    void generateRewardsUI();
    void generateNotificationsUI();
    void generateEntitiesUI();
};

}}} // namespace wydstudio::ui::visual

#endif // WYDSTUDIO_EVENT_SCHEDULER_VIEW_H