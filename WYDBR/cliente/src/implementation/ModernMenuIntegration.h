/**
 * @file ModernMenuIntegration.h
 * @brief Integração dos novos menus modernos com o sistema existente do WYD
 */

#ifndef WYD_MODERN_MENU_INTEGRATION_H
#define WYD_MODERN_MENU_INTEGRATION_H

#include <string>
#include <map>
#include <vector>
#include <functional>

namespace wydbr {
namespace ui {

/**
 * @class ModernMenuIntegration
 * @brief Gerencia a integração de novos menus com o sistema existente
 */
class ModernMenuIntegration {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência à instância única
     */
    static ModernMenuIntegration& getInstance();
    
    /**
     * @brief Inicializa a integração de menu
     */
    void initialize();
    
    /**
     * @brief Processa eventos de teclado para os novos menus
     * @param keyCode Código da tecla pressionada
     * @param altPressed Se a tecla ALT está pressionada
     * @param ctrlPressed Se a tecla CTRL está pressionada
     * @param shiftPressed Se a tecla SHIFT está pressionada
     * @return true se o evento foi processado, false caso contrário
     */
    bool processKeyEvent(int keyCode, bool altPressed, bool ctrlPressed, bool shiftPressed);
    
    /**
     * @brief Processa eventos de clique para os novos menus
     * @param x Coordenada X do clique
     * @param y Coordenada Y do clique
     * @param rightClick Se é um clique com o botão direito
     * @return true se o evento foi processado, false caso contrário
     */
    bool processClickEvent(int x, int y, bool rightClick);
    
    /**
     * @brief Abre o menu de ajustes avançados
     */
    void openAdvancedSettingsMenu();
    
    /**
     * @brief Abre o menu de compartilhamento de tela
     */
    void openScreenSharingMenu();
    
    /**
     * @brief Abre o menu para acessar tela remota
     */
    void openScreenAccessMenu();
    
    /**
     * @brief Abre o menu de configurações VOIP
     */
    void openVoipSettingsMenu();
    
    /**
     * @brief Abre o menu de chat de voz
     */
    void openVoiceChatMenu();
    
    /**
     * @brief Abre o menu de jogadores online
     */
    void openOnlinePlayersMenu();
    
    /**
     * @brief Abre o menu de amigos
     */
    void openFriendsMenu();
    
    /**
     * @brief Abre o menu de perfil
     */
    void openProfileMenu();
    
    /**
     * @brief Abre/fecha o menu de teleporte rápido
     */
    void toggleTeleportMenu();
    
    /**
     * @brief Ativa/desativa o VOIP
     */
    void toggleVoip();
    
    /**
     * @brief Mostra o overlay de compartilhamento ativo
     * @param remoteUserName Nome do usuário conectado remotamente
     */
    void showSharingOverlay(const std::string& remoteUserName);
    
    /**
     * @brief Esconde o overlay de compartilhamento
     */
    void hideSharingOverlay();
    
    /**
     * @brief Processa clique no nome do usuário remoto
     * @param x Coordenada X do clique
     * @param y Coordenada Y do clique
     * @param rightClick Se é um clique com o botão direito
     * @return true se o clique foi processado, false caso contrário
     */
    bool processRemoteUserClick(int x, int y, bool rightClick);
    
    /**
     * @brief Processa clique na barra de vida para abrir o perfil
     * @param x Coordenada X do clique
     * @param y Coordenada Y do clique
     * @return true se o clique foi processado, false caso contrário
     */
    bool processHealthBarClick(int x, int y);
    
    /**
     * @brief Atualiza os indicadores de desempenho
     * @param fps Taxa de quadros atual
     * @param ping Ping em ms
     * @param serverLatency Latência do servidor em ms
     * @param networkStatus Status da rede (0-3)
     */
    void updatePerformanceIndicators(int fps, int ping, int serverLatency, int networkStatus);
    
    /**
     * @brief Verifica se o cursor está sobre algum indicador de desempenho
     * @param x Coordenada X do cursor
     * @param y Coordenada Y do cursor
     * @return Índice do indicador ou -1 se não houver indicador sob o cursor
     */
    int getPerformanceIndicatorUnderCursor(int x, int y);
    
    /**
     * @brief Mostra tooltip para um indicador de desempenho
     * @param indicatorIndex Índice do indicador
     * @param x Coordenada X para exibir tooltip
     * @param y Coordenada Y para exibir tooltip
     */
    void showPerformanceTooltip(int indicatorIndex, int x, int y);

private:
    ModernMenuIntegration();
    ~ModernMenuIntegration();
    
    // Previne cópia e atribuição
    ModernMenuIntegration(const ModernMenuIntegration&) = delete;
    ModernMenuIntegration& operator=(const ModernMenuIntegration&) = delete;
    
    // Métodos privados
    void registerHotkeys();
    void loadMenuFiles();
    void createContextMenu();
    void saveUserSettings();
    void loadUserSettings();
    void teleportToCity(int cityIndex);
    bool checkTeleportCooldown();
    void updateVoipIcons();
    void generateSharingLink();
    
    // Estado interno
    bool m_initialized;
    bool m_voipEnabled;
    bool m_sharingEnabled;
    std::string m_remoteUser;
    std::map<std::string, std::string> m_menuFileMap;
    
    // Menu de teleporte
    bool m_teleportMenuVisible;
    int m_teleportCooldown;
    
    // Dados de indicadores de desempenho
    struct {
        int fps;
        int ping;
        int serverLatency;
        int networkStatus;
        bool visible;
        int position; // 0=sup-dir, 1=sup-esq, 2=inf-dir, 3=inf-esq
    } m_performanceData;
    
    // Configurações
    struct {
        int pushToTalkKey;
        bool useMouse;
        int mouseButton;
        bool voiceActivation;
        int inputVolume;
        int outputVolume;
    } m_voipConfig;
    
    struct {
        std::string password;
        std::string sharingLink;
        bool blockItemMovement;
        bool blockPasswordChange;
        bool blockCharDeletion;
    } m_sharingConfig;
    
    struct {
        int graphicsQuality;
        int viewDistance;
        int effectsQuality;
        std::map<std::string, int> keyBindings;
    } m_graphicsConfig;
    
    // Lista de amigos
    std::vector<std::string> m_friendsList;
    std::vector<std::string> m_blockedUsers;
};

}} // namespace wydbr::ui

#endif // WYD_MODERN_MENU_INTEGRATION_H
