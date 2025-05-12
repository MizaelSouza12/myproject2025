#ifndef WYDSTUDIO_MAIN_WINDOW_H
#define WYDSTUDIO_MAIN_WINDOW_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>
#include <chrono>
#include <nlohmann/json.hpp>
#include "../core/WYDStudioFacade.h"
#include "ItemStudio.h"
#include "MapDesigner.h"
#include "QuestCreator.h"

namespace wydstudio {
namespace ui {

using json = nlohmann::json;

/**
 * @brief Tipos de módulos na UI principal
 */
enum class ModuleType {
    DASHBOARD,          // Painel inicial
    SERVER_CONTROL,     // Controle de servidor
    ITEM_EDITOR,        // Editor de itens
    MAP_DESIGNER,       // Designer de mapas
    QUEST_CREATOR,      // Criador de quests
    NPC_EDITOR,         // Editor de NPCs
    MOB_EDITOR,         // Editor de monstros
    SKILL_EDITOR,       // Editor de habilidades
    ECONOMY_MANAGER,    // Gerenciador de economia
    PLAYER_MANAGER,     // Gerenciador de jogadores
    GUILD_MANAGER,      // Gerenciador de guildas
    EVENT_SCHEDULER,    // Agendador de eventos
    SECURITY_CENTER,    // Centro de segurança
    LOG_ANALYZER,       // Analisador de logs
    SETTINGS            // Configurações
};

/**
 * @brief Nível de permissão requerida para módulos
 */
enum class PermissionLevel {
    PLAYER,     // Jogador normal
    TRIAL_GM,   // GM em teste
    GM,         // Game Master
    ADMIN,      // Administrador
    DEV,        // Desenvolvedor
    OWNER       // Proprietário
};

/**
 * @brief Informações de status do servidor
 */
struct ServerStatusInfo {
    bool isRunning;
    std::chrono::system_clock::time_point startTime;
    int32_t onlinePlayers;
    int32_t maxPlayers;
    int32_t cpuUsage;
    int32_t memoryUsage;
    int32_t diskUsage;
    double networkIn;
    double networkOut;
    std::string version;
    std::map<std::string, std::string> additionalInfo;
};

/**
 * @brief Estatísticas de jogo para o dashboard
 */
struct GameStatistics {
    // Jogadores
    int32_t totalPlayers;
    int32_t activePlayersLast24h;
    int32_t activePlayersLast7d;
    int32_t newPlayersLast24h;
    
    // Economia
    int64_t goldInCirculation;
    int64_t goldGeneratedLast24h;
    int64_t goldRemovedLast24h;
    int32_t itemsInCirculation;
    int32_t itemsGeneratedLast24h;
    
    // Atividade
    int32_t questsCompletedLast24h;
    int32_t mobsKilledLast24h;
    int32_t pvpBattlesLast24h;
    int32_t tradesLast24h;
    
    // Agrupamentos
    std::map<std::string, int32_t> playersByClass;
    std::map<std::string, int32_t> playersByLevel;
    std::map<std::string, int32_t> activeMapsByPlayers;
};

/**
 * @brief Evento de servidor recente
 */
struct ServerEvent {
    enum class Type {
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
        SECURITY,
        PLAYER,
        SYSTEM
    };
    
    Type type;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::string source;
    std::string details;
    int32_t importance; // 0-100, com 100 sendo o mais importante
};

/**
 * @brief Ação de usuário disponível
 */
struct UserAction {
    std::string id;
    std::string name;
    std::string description;
    std::string iconPath;
    ModuleType targetModule;
    std::function<void()> action;
};

/**
 * @brief Janela principal do WYDStudio
 */
class WYDStudioMainWindow {
public:
    /**
     * @brief Construtor
     * @param facade Referência ao facade principal do WYDStudio
     */
    WYDStudioMainWindow(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~WYDStudioMainWindow();
    
    /**
     * @brief Inicializa a interface principal
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Executa a interface principal (bloqueante)
     * @return Código de saída
     */
    int run();
    
    /**
     * @brief Carrega as configurações de UI
     * @param configPath Caminho opcional do arquivo de configuração
     * @return true se carregado com sucesso
     */
    bool loadConfiguration(const std::string& configPath = "");
    
    /**
     * @brief Salva as configurações de UI
     * @param configPath Caminho opcional do arquivo de configuração
     * @return true se salvo com sucesso
     */
    bool saveConfiguration(const std::string& configPath = "");
    
    /**
     * @brief Abre um módulo específico
     * @param type Tipo de módulo a abrir
     * @param params Parâmetros opcionais para o módulo
     * @return true se aberto com sucesso
     */
    bool openModule(ModuleType type, const std::map<std::string, std::string>& params = {});
    
    /**
     * @brief Obtém informações de status do servidor
     * @return Informações de status
     */
    ServerStatusInfo getServerStatus();
    
    /**
     * @brief Obtém estatísticas atuais do jogo
     * @return Estatísticas do jogo
     */
    GameStatistics getGameStatistics();
    
    /**
     * @brief Obtém eventos recentes do servidor
     * @param maxEvents Número máximo de eventos a retornar
     * @param minImportance Importância mínima dos eventos (0-100)
     * @return Lista de eventos
     */
    std::vector<ServerEvent> getRecentEvents(int32_t maxEvents = 50, int32_t minImportance = 0);
    
    /**
     * @brief Obtém ações rápidas disponíveis para o usuário
     * @return Lista de ações
     */
    std::vector<UserAction> getQuickActions();
    
    /**
     * @brief Define o tema da interface
     * @param themeName Nome do tema
     * @return true se aplicado com sucesso
     */
    bool setTheme(const std::string& themeName);
    
    /**
     * @brief Obtém o tema atual
     * @return Nome do tema
     */
    std::string getCurrentTheme() const;
    
    /**
     * @brief Lista todos os temas disponíveis
     * @return Lista de nomes de temas
     */
    std::vector<std::string> getAvailableThemes();
    
    /**
     * @brief Define o idioma da interface
     * @param languageCode Código do idioma (e.g., "en", "pt-br")
     * @return true se aplicado com sucesso
     */
    bool setLanguage(const std::string& languageCode);
    
    /**
     * @brief Obtém o idioma atual
     * @return Código do idioma
     */
    std::string getCurrentLanguage() const;
    
    /**
     * @brief Lista todos os idiomas disponíveis
     * @return Lista de códigos de idioma
     */
    std::vector<std::string> getAvailableLanguages();
    
    /**
     * @brief Exibe uma mensagem de notificação na interface
     * @param title Título da notificação
     * @param message Corpo da mensagem
     * @param type Tipo de notificação (info, warning, error, etc.)
     */
    void showNotification(const std::string& title, const std::string& message, const std::string& type);
    
    /**
     * @brief Exibe uma caixa de diálogo modal
     * @param title Título do diálogo
     * @param message Corpo da mensagem
     * @param buttons Botões a exibir (e.g., "Ok", "Cancel")
     * @return Texto do botão selecionado
     */
    std::string showDialog(const std::string& title, const std::string& message, const std::vector<std::string>& buttons);
    
    /**
     * @brief Registra um callback para eventos do sistema
     * @param eventType Tipo de evento
     * @param callback Função de callback
     * @return ID de registro (para cancelamento posterior)
     */
    uint32_t registerEventCallback(const std::string& eventType, std::function<void(const json&)> callback);
    
    /**
     * @brief Cancela registro de callback
     * @param callbackId ID do callback a cancelar
     */
    void unregisterEventCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém o módulo ItemStudio
     * @return Referência ao módulo ItemStudio
     */
    const ItemStudio& getItemStudio() const;
    
    /**
     * @brief Obtém o módulo MapDesigner
     * @return Referência ao módulo MapDesigner
     */
    const MapDesigner& getMapDesigner() const;
    
    /**
     * @brief Obtém o módulo QuestCreator
     * @return Referência ao módulo QuestCreator
     */
    const QuestCreator& getQuestCreator() const;
    
    /**
     * @brief Verifica se o usuário tem permissão para um módulo
     * @param moduleType Tipo de módulo
     * @return true se o usuário tem permissão
     */
    bool hasPermissionForModule(ModuleType moduleType) const;
    
    /**
     * @brief Obtém o nível de permissão do usuário atual
     * @return Nível de permissão
     */
    PermissionLevel getCurrentUserPermissionLevel() const;
    
    /**
     * @brief Obtém nível de permissão requerido para um módulo
     * @param moduleType Tipo de módulo
     * @return Nível de permissão requerido
     */
    PermissionLevel getRequiredPermissionForModule(ModuleType moduleType) const;

    /**
     * @brief Inicia o servidor
     * @return true se iniciado com sucesso
     */
    bool startServer();
    
    /**
     * @brief Para o servidor
     * @return true se parado com sucesso
     */
    bool stopServer();
    
    /**
     * @brief Reinicia o servidor
     * @return true se reiniciado com sucesso
     */
    bool restartServer();

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade principal
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Módulos da interface
    std::shared_ptr<ItemStudio> m_itemStudio;
    std::shared_ptr<MapDesigner> m_mapDesigner;
    std::shared_ptr<QuestCreator> m_questCreator;
    
    // Configurações
    std::string m_currentTheme;
    std::string m_currentLanguage;
    
    // Estado da interface
    ModuleType m_currentModule;
    std::map<ModuleType, std::shared_ptr<void>> m_moduleInstances;
    
    // Utilitários da interface
    void setupSystemTray();
    void setupThemes();
    void setupModules();
    void loadTranslations(const std::string& languageCode);
    void connectSignalsAndSlots();
    
    // Dashboard widgets
    void updateDashboardWidgets();
    void setupServerStatusWidget();
    void setupPlayerStatisticsWidget();
    void setupEconomyStatisticsWidget();
    void setupRecentEventsWidget();
    void setupQuickActionsWidget();
    
    // Gerenciamento de permissões
    std::map<ModuleType, PermissionLevel> m_modulePermissions;
    void initializePermissions();
};

}} // namespace wydstudio::ui

#endif // WYDSTUDIO_MAIN_WINDOW_H