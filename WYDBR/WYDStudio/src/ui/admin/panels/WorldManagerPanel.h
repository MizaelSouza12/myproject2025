#ifndef WYDSTUDIO_WORLD_MANAGER_PANEL_H
#define WYDSTUDIO_WORLD_MANAGER_PANEL_H

#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QScrollArea>
#include <QSplitter>
#include <QDateTimeEdit>
#include <QSlider>
#include <QStackedWidget>
#include <QCalendarWidget>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QButtonGroup>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QToolTip>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "../../../core/WYDStudioFacade.h"
#include "../../common/FileBrowserButton.h"

namespace wydstudio {
namespace ui {
namespace admin {
namespace panels {

/**
 * @brief Tipo de evento de mundo
 */
enum class WorldEventType {
    INVASION,            // Invasão de monstros
    BOSS_SPAWN,          // Aparecimento de chefe
    DROP_RATE_BOOST,     // Aumento da taxa de drop
    EXP_RATE_BOOST,      // Aumento da taxa de experiência
    GOLD_RATE_BOOST,     // Aumento da taxa de gold
    PVP_EVENT,           // Evento PVP
    GUILD_WAR,           // Guerra de guildas
    THUNDER_STORM,       // Tempestade de raios
    TREASURE_HUNT,       // Caça ao tesouro
    TIMED_QUEST,         // Missão por tempo limitado
    CUSTOM               // Evento personalizado
};

/**
 * @brief Status do evento de mundo
 */
enum class WorldEventStatus {
    SCHEDULED,           // Agendado
    RUNNING,             // Em execução
    COMPLETED,           // Concluído
    CANCELLED,           // Cancelado
    ERROR                // Erro
};

/**
 * @brief Evento de mundo
 */
struct WorldEvent {
    std::string id;
    std::string name;
    std::string description;
    WorldEventType type;
    std::string mapName;
    int xStart;
    int yStart;
    int xEnd;
    int yEnd;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    int duration;          // Duração em minutos
    bool isRecurring;
    std::string recurrencePattern;
    WorldEventStatus status;
    std::map<std::string, std::string> parameters;
    std::string createdBy;
    std::chrono::system_clock::time_point createdAt;
    std::string lastModifiedBy;
    std::chrono::system_clock::time_point lastModifiedAt;
    std::string scriptPath;
    std::string commandsToExecute;
    std::vector<std::string> mobsToSpawn;
    std::vector<std::string> itemsToDrop;
    std::vector<std::string> rewards;
    std::string notificationMessage;
    int notificationInterval;
    bool isVisible;
    int minLevel;
    int maxLevel;
    bool restrictToGuild;
    std::string guildName;
};

/**
 * @brief Coordenada no mundo
 */
struct WorldCoordinate {
    int x;
    int y;
    int mapId;
    std::string mapName;
};

/**
 * @brief Área no mundo
 */
struct WorldArea {
    int xStart;
    int yStart;
    int xEnd;
    int yEnd;
    int mapId;
    std::string mapName;
    std::string areaName;
    std::string areaType;
};

/**
 * @brief Painel de gerenciamento de mundo
 */
class WorldManagerPanel : public QWidget {
    Q_OBJECT
    
public:
    /**
     * @brief Construtor
     * @param parent Widget pai
     */
    WorldManagerPanel(QWidget* parent = nullptr);
    
    /**
     * @brief Destrutor
     */
    ~WorldManagerPanel();
    
    /**
     * @brief Inicializa o painel
     * @param facade Referência ao facade do WYDStudio
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Atualiza os dados do painel
     */
    void refresh();
    
    // ===== Eventos =====
    
    /**
     * @brief Obtém todos os eventos programados
     * @return Lista de eventos
     */
    std::vector<WorldEvent> getAllEvents();
    
    /**
     * @brief Obtém eventos por tipo
     * @param type Tipo de evento
     * @return Lista de eventos
     */
    std::vector<WorldEvent> getEventsByType(WorldEventType type);
    
    /**
     * @brief Obtém eventos por status
     * @param status Status dos eventos
     * @return Lista de eventos
     */
    std::vector<WorldEvent> getEventsByStatus(WorldEventStatus status);
    
    /**
     * @brief Obtém eventos agendados para uma data
     * @param date Data dos eventos
     * @return Lista de eventos
     */
    std::vector<WorldEvent> getEventsByDate(const QDate& date);
    
    /**
     * @brief Cria um novo evento
     * @param event Dados do evento
     * @return ID do evento criado ou string vazia se falhar
     */
    std::string createEvent(const WorldEvent& event);
    
    /**
     * @brief Atualiza um evento existente
     * @param event Dados do evento
     * @return true se atualizado com sucesso
     */
    bool updateEvent(const WorldEvent& event);
    
    /**
     * @brief Remove um evento
     * @param eventId ID do evento
     * @return true se removido com sucesso
     */
    bool removeEvent(const std::string& eventId);
    
    /**
     * @brief Inicia um evento
     * @param eventId ID do evento
     * @return true se iniciado com sucesso
     */
    bool startEvent(const std::string& eventId);
    
    /**
     * @brief Para um evento
     * @param eventId ID do evento
     * @return true se parado com sucesso
     */
    bool stopEvent(const std::string& eventId);
    
    /**
     * @brief Inicia um evento de drop personalizado
     * @param itemName Nome do item
     * @param mobName Nome do mob (ou vazio para todos)
     * @param mapName Nome do mapa (ou vazio para todos)
     * @param dropRate Taxa de drop (0-100)
     * @param duration Duração em minutos
     * @return true se iniciado com sucesso
     */
    bool startDropEvent(const std::string& itemName, const std::string& mobName, 
                      const std::string& mapName, int dropRate, int duration);
    
    // ===== Mapa =====
    
    /**
     * @brief Obtém informações sobre um mapa
     * @param mapName Nome do mapa
     * @return Dados do mapa em formato JSON
     */
    QJsonObject getMapInfo(const std::string& mapName);
    
    /**
     * @brief Teleporta um jogador para uma localização
     * @param playerName Nome do jogador
     * @param mapName Nome do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return true se teleportado com sucesso
     */
    bool teleportPlayer(const std::string& playerName, const std::string& mapName, int x, int y);
    
    /**
     * @brief Teleporta todos os jogadores para uma localização
     * @param mapName Nome do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return true se teleportado com sucesso
     */
    bool teleportAllPlayers(const std::string& mapName, int x, int y);
    
    /**
     * @brief Define uma área restrita
     * @param area Dados da área
     * @param restricted Se verdadeiro, restringe a área
     * @return true se definido com sucesso
     */
    bool setRestrictedArea(const WorldArea& area, bool restricted);
    
    /**
     * @brief Define uma área de PVP
     * @param area Dados da área
     * @param isPvp Se verdadeiro, habilita PVP na área
     * @return true se definido com sucesso
     */
    bool setPvpArea(const WorldArea& area, bool isPvp);
    
    /**
     * @brief Faz aparecer um NPC temporário
     * @param npcId ID do NPC
     * @param mapName Nome do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param duration Duração em minutos (0 = permanente)
     * @return true se criado com sucesso
     */
    bool spawnTemporaryNpc(const std::string& npcId, const std::string& mapName, 
                         int x, int y, int duration = 0);
    
    /**
     * @brief Faz aparecer um monstro
     * @param mobId ID do monstro
     * @param mapName Nome do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param count Quantidade
     * @param respawnTime Tempo de respawn em segundos
     * @param duration Duração em minutos (0 = permanente)
     * @return true se criado com sucesso
     */
    bool spawnMob(const std::string& mobId, const std::string& mapName, 
                int x, int y, int count = 1, int respawnTime = 30, int duration = 0);
    
    /**
     * @brief Remove todos os monstros de um mapa
     * @param mapName Nome do mapa
     * @param mobId ID do monstro (ou vazio para todos)
     * @return true se removido com sucesso
     */
    bool removeAllMobs(const std::string& mapName, const std::string& mobId = "");
    
    /**
     * @brief Cria um item no chão
     * @param itemId ID do item
     * @param mapName Nome do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param count Quantidade (para itens empilháveis)
     * @param duration Duração em segundos (0 = padrão do servidor)
     * @return true se criado com sucesso
     */
    bool createGroundItem(const std::string& itemId, const std::string& mapName, 
                        int x, int y, int count = 1, int duration = 0);
    
    /**
     * @brief Altera o clima de um mapa
     * @param mapName Nome do mapa
     * @param weather Tipo de clima (rain, snow, thunder, fog, clear)
     * @param intensity Intensidade (0-100)
     * @param duration Duração em minutos
     * @return true se alterado com sucesso
     */
    bool setMapWeather(const std::string& mapName, const std::string& weather, 
                     int intensity = 50, int duration = 30);
    
    /**
     * @brief Bloqueia/desbloqueia um mapa
     * @param mapName Nome do mapa
     * @param blocked Se verdadeiro, bloqueia o mapa
     * @param message Mensagem para os jogadores
     * @return true se bloqueado/desbloqueado com sucesso
     */
    bool setMapBlocked(const std::string& mapName, bool blocked, 
                     const std::string& message = "");
    
    // ===== Guerra =====
    
    /**
     * @brief Inicia uma guerra de guildas
     * @param mapName Nome do mapa
     * @param duration Duração em minutos
     * @param useScoreboard Se verdadeiro, usa placar
     * @return true se iniciado com sucesso
     */
    bool startGuildWar(const std::string& mapName, int duration, bool useScoreboard = true);
    
    /**
     * @brief Para uma guerra de guildas
     * @param mapName Nome do mapa
     * @return true se parado com sucesso
     */
    bool stopGuildWar(const std::string& mapName);
    
    /**
     * @brief Adiciona uma guilda a uma guerra
     * @param warId ID da guerra
     * @param guildName Nome da guilda
     * @return true se adicionado com sucesso
     */
    bool addGuildToWar(const std::string& warId, const std::string& guildName);
    
    /**
     * @brief Registra pontos para uma guilda em uma guerra
     * @param warId ID da guerra
     * @param guildName Nome da guilda
     * @param points Pontos a adicionar
     * @return true se registrado com sucesso
     */
    bool addWarPoints(const std::string& warId, const std::string& guildName, int points);
    
public slots:
    /**
     * @brief Slot para atualizar a visualização do calendário
     * @param date Data selecionada
     */
    void onCalendarDateSelected(const QDate& date);
    
    /**
     * @brief Slot para criar um novo evento
     */
    void onCreateEventClicked();
    
    /**
     * @brief Slot para editar um evento
     * @param eventId ID do evento
     */
    void onEditEventClicked(const std::string& eventId);
    
    /**
     * @brief Slot para remover um evento
     * @param eventId ID do evento
     */
    void onRemoveEventClicked(const std::string& eventId);
    
    /**
     * @brief Slot para iniciar um evento
     * @param eventId ID do evento
     */
    void onStartEventClicked(const std::string& eventId);
    
    /**
     * @brief Slot para parar um evento
     * @param eventId ID do evento
     */
    void onStopEventClicked(const std::string& eventId);
    
    /**
     * @brief Slot para criar um drop personalizado
     */
    void onQuickDropEventClicked();
    
    /**
     * @brief Slot quando o tipo de evento é alterado
     * @param index Índice do tipo
     */
    void onEventTypeChanged(int index);
    
    /**
     * @brief Slot para atualizar a lista de eventos
     */
    void onRefreshEventsClicked();
    
    /**
     * @brief Slot para atualizar o mapa
     * @param mapName Nome do mapa
     */
    void onMapSelected(const QString& mapName);
    
    /**
     * @brief Slot para teleportar um jogador
     */
    void onTeleportPlayerClicked();
    
    /**
     * @brief Slot para teleportar todos os jogadores
     */
    void onTeleportAllPlayersClicked();
    
    /**
     * @brief Slot para fazer aparecer um monstro
     */
    void onSpawnMobClicked();
    
    /**
     * @brief Slot para remover todos os monstros
     */
    void onRemoveAllMobsClicked();
    
    /**
     * @brief Slot para alterar o clima
     */
    void onSetWeatherClicked();
    
    /**
     * @brief Slot para bloquear/desbloquear um mapa
     * @param blocked Se verdadeiro, bloqueia o mapa
     */
    void onToggleMapBlockClicked(bool blocked);
    
    /**
     * @brief Slot para iniciar uma guerra de guildas
     */
    void onStartGuildWarClicked();
    
    /**
     * @brief Slot para parar uma guerra de guildas
     */
    void onStopGuildWarClicked();
    
signals:
    /**
     * @brief Sinal emitido quando um evento é criado
     * @param eventId ID do evento
     * @param eventName Nome do evento
     */
    void eventCreated(const std::string& eventId, const std::string& eventName);
    
    /**
     * @brief Sinal emitido quando um evento é iniciado
     * @param eventId ID do evento
     * @param eventName Nome do evento
     */
    void eventStarted(const std::string& eventId, const std::string& eventName);
    
    /**
     * @brief Sinal emitido quando um evento é parado
     * @param eventId ID do evento
     * @param eventName Nome do evento
     */
    void eventStopped(const std::string& eventId, const std::string& eventName);
    
    /**
     * @brief Sinal emitido quando um jogador é teleportado
     * @param playerName Nome do jogador
     * @param mapName Nome do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     */
    void playerTeleported(const std::string& playerName, const std::string& mapName, int x, int y);
    
    /**
     * @brief Sinal emitido quando uma guerra de guildas é iniciada
     * @param warId ID da guerra
     * @param mapName Nome do mapa
     */
    void guildWarStarted(const std::string& warId, const std::string& mapName);
    
private:
    // Interface do usuário
    QTabWidget* m_tabWidget;
    
    // Aba de eventos
    QWidget* m_eventsTab;
    QSplitter* m_eventsSplitter;
    QCalendarWidget* m_eventsCalendar;
    QListWidget* m_eventsList;
    QWidget* m_eventsDetailsWidget;
    QStackedWidget* m_eventsStack;
    
    // Detalhes do evento
    QWidget* m_eventDetailsWidget;
    QLineEdit* m_eventNameEdit;
    QTextEdit* m_eventDescriptionEdit;
    QComboBox* m_eventTypeComboBox;
    QComboBox* m_eventMapComboBox;
    QSpinBox* m_eventXStartSpinBox;
    QSpinBox* m_eventYStartSpinBox;
    QSpinBox* m_eventXEndSpinBox;
    QSpinBox* m_eventYEndSpinBox;
    QDateTimeEdit* m_eventStartTimeEdit;
    QDateTimeEdit* m_eventEndTimeEdit;
    QSpinBox* m_eventDurationSpinBox;
    QCheckBox* m_eventRecurringCheckBox;
    QComboBox* m_eventRecurrencePatternComboBox;
    QListWidget* m_eventMobsList;
    QListWidget* m_eventItemsList;
    QTextEdit* m_eventCommandsEdit;
    QLineEdit* m_eventScriptPathEdit;
    common::FileBrowserButton* m_eventScriptBrowseButton;
    QLineEdit* m_eventNotificationEdit;
    QSpinBox* m_eventNotificationIntervalSpinBox;
    QCheckBox* m_eventVisibleCheckBox;
    QSpinBox* m_eventMinLevelSpinBox;
    QSpinBox* m_eventMaxLevelSpinBox;
    QCheckBox* m_eventGuildRestrictCheckBox;
    QComboBox* m_eventGuildComboBox;
    QPushButton* m_eventSaveButton;
    QPushButton* m_eventCancelButton;
    QPushButton* m_eventStartButton;
    QPushButton* m_eventStopButton;
    QPushButton* m_eventDeleteButton;
    
    // Widget de drop rápido
    QWidget* m_quickDropWidget;
    QComboBox* m_quickDropItemComboBox;
    QComboBox* m_quickDropMobComboBox;
    QComboBox* m_quickDropMapComboBox;
    QSpinBox* m_quickDropRateSpinBox;
    QSpinBox* m_quickDropDurationSpinBox;
    QPushButton* m_quickDropStartButton;
    
    // Aba de mapa
    QWidget* m_mapTab;
    QSplitter* m_mapSplitter;
    QComboBox* m_mapSelectComboBox;
    QGraphicsView* m_mapView;
    QGraphicsScene* m_mapScene;
    QWidget* m_mapControlsWidget;
    QTabWidget* m_mapControlsTabWidget;
    
    // Controles do mapa - Teleporte
    QWidget* m_teleportTab;
    QLineEdit* m_teleportPlayerEdit;
    QSpinBox* m_teleportXSpinBox;
    QSpinBox* m_teleportYSpinBox;
    QPushButton* m_teleportPlayerButton;
    QPushButton* m_teleportAllButton;
    
    // Controles do mapa - Monstros
    QWidget* m_mobsTab;
    QComboBox* m_spawnMobComboBox;
    QSpinBox* m_spawnMobXSpinBox;
    QSpinBox* m_spawnMobYSpinBox;
    QSpinBox* m_spawnMobCountSpinBox;
    QSpinBox* m_spawnMobRespawnSpinBox;
    QSpinBox* m_spawnMobDurationSpinBox;
    QPushButton* m_spawnMobButton;
    QPushButton* m_removeAllMobsButton;
    
    // Controles do mapa - Clima
    QWidget* m_weatherTab;
    QComboBox* m_weatherTypeComboBox;
    QSlider* m_weatherIntensitySlider;
    QSpinBox* m_weatherDurationSpinBox;
    QPushButton* m_setWeatherButton;
    
    // Controles do mapa - Áreas
    QWidget* m_areasTab;
    QComboBox* m_areaTypeComboBox;
    QSpinBox* m_areaXStartSpinBox;
    QSpinBox* m_areaYStartSpinBox;
    QSpinBox* m_areaXEndSpinBox;
    QSpinBox* m_areaYEndSpinBox;
    QLineEdit* m_areaNameEdit;
    QPushButton* m_setAreaButton;
    QPushButton* m_clearAreaButton;
    QCheckBox* m_showAreasCheckBox;
    
    // Controles do mapa - Itens
    QWidget* m_itemsTab;
    QComboBox* m_groundItemComboBox;
    QSpinBox* m_groundItemXSpinBox;
    QSpinBox* m_groundItemYSpinBox;
    QSpinBox* m_groundItemCountSpinBox;
    QSpinBox* m_groundItemDurationSpinBox;
    QPushButton* m_createGroundItemButton;
    
    // Controles do mapa - Geral
    QWidget* m_mapGeneralTab;
    QPushButton* m_blockMapButton;
    QLineEdit* m_blockMapMessageEdit;
    QCheckBox* m_showPlayersCheckBox;
    QCheckBox* m_showMobsCheckBox;
    QCheckBox* m_showNpcsCheckBox;
    QCheckBox* m_showItemsCheckBox;
    
    // Aba de guerra
    QWidget* m_warTab;
    QSplitter* m_warSplitter;
    QComboBox* m_warMapComboBox;
    QSpinBox* m_warDurationSpinBox;
    QCheckBox* m_warScoreboardCheckBox;
    QTableWidget* m_warGuildsTable;
    QPushButton* m_startWarButton;
    QPushButton* m_stopWarButton;
    QPushButton* m_addGuildButton;
    QPushButton* m_removeGuildButton;
    QSpinBox* m_addPointsSpinBox;
    QPushButton* m_addPointsButton;
    
    // Referência ao facade
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Estado atual
    std::vector<WorldEvent> m_currentEvents;
    std::string m_currentMapName;
    std::string m_currentEventId;
    bool m_isEditingEvent;
    
    // Dados carregados
    std::vector<std::string> m_mapNames;
    std::vector<std::string> m_mobNames;
    std::vector<std::string> m_itemNames;
    std::vector<std::string> m_guildNames;
    std::map<std::string, QPixmap> m_mapImages;
    
    // Inicialização da UI
    void createUI();
    void createEventsTab();
    void createEventDetailsWidget();
    void createQuickDropWidget();
    void createMapTab();
    void createWarTab();
    void connectSignals();
    
    // Atualização de UI
    void updateEventsCalendar();
    void updateEventsList(const QDate& date);
    void updateEventDetails(const std::string& eventId);
    void updateMapsComboBoxes();
    void updateMobsComboBoxes();
    void updateItemsComboBoxes();
    void updateGuildsComboBoxes();
    void updateMapView(const std::string& mapName);
    void updateWarTable();
    
    // Carregamento de dados
    void loadMapNames();
    void loadMobNames();
    void loadItemNames();
    void loadGuildNames();
    void loadMapImages();
    
    // Utilitários
    std::string eventTypeToString(WorldEventType type) const;
    WorldEventType stringToEventType(const std::string& typeStr) const;
    std::string eventStatusToString(WorldEventStatus status) const;
    WorldEventStatus stringToEventStatus(const std::string& statusStr) const;
    WorldEvent getEventFromUI() const;
    void setEventToUI(const WorldEvent& event);
    bool validateEventData() const;
    std::string getGuildWarId(const std::string& mapName) const;
};

}}}} // namespace wydstudio::ui::admin::panels

#endif // WYDSTUDIO_WORLD_MANAGER_PANEL_H