/**
 * @file modern_menu_integration.h
 * @brief Integração dos novos menus modernos com o sistema legado do WYD
 */

#ifndef MODERN_MENU_INTEGRATION_H
#define MODERN_MENU_INTEGRATION_H

#include <string>
#include <map>
#include <vector>
#include <functional>

namespace WYDBR {
namespace UI {

/**
 * @class ModernMenuIntegration
 * @brief Classe responsável por integrar os novos menus modernos ao WYDBR
 * 
 * Esta classe gerencia a exibição e interação com os novos menus adicionados
 * ao WYDBRASIL, incluindo ajustes avançados, compartilhamento de tela,
 * chat de voz, e teletransporte rápido.
 */
class ModernMenuIntegration {
public:
    /**
     * @brief Construtor padrão
     */
    ModernMenuIntegration();
    
    /**
     * @brief Destrutor
     */
    ~ModernMenuIntegration();
    
    /**
     * @brief Inicializa o sistema de menus modernos
     * @return true se a inicialização for bem-sucedida, false caso contrário
     */
    bool Initialize();
    
    /**
     * @brief Carrega os arquivos de menu da pasta UI
     * @param forcedReload Se true, recarrega os menus mesmo se já estiverem carregados
     * @return true se o carregamento for bem-sucedido, false caso contrário
     */
    bool LoadMenuFiles(bool forcedReload = false);
    
    /**
     * @brief Processa eventos de teclado para acionar os menus
     * @param key Código da tecla pressionada
     * @param modifiers Modificadores (ALT, CTRL, SHIFT)
     * @return true se o evento de teclado for processado, false caso contrário
     */
    bool ProcessKeyboardEvent(int key, int modifiers);
    
    /**
     * @brief Abre um menu específico
     * @param menuName Nome do menu a ser aberto
     * @return true se o menu for aberto com sucesso, false caso contrário
     */
    bool OpenMenu(const std::string& menuName);
    
    /**
     * @brief Fecha o menu atual
     * @return true se o menu for fechado com sucesso, false caso contrário
     */
    bool CloseCurrentMenu();
    
    /**
     * @brief Atualiza o estado dos menus
     * @param deltaTime Tempo desde a última atualização (em segundos)
     */
    void Update(float deltaTime);
    
    /**
     * @brief Renderiza o menu atual
     */
    void Render();
    
    /**
     * @brief Salva as configurações do usuário
     * @return true se as configurações forem salvas com sucesso, false caso contrário
     */
    bool SaveUserSettings();
    
    /**
     * @brief Carrega as configurações do usuário
     * @return true se as configurações forem carregadas com sucesso, false caso contrário
     */
    bool LoadUserSettings();
    
private:
    struct MenuData {
        std::string name;
        std::string filePath;
        std::vector<std::string> content;
        bool isLoaded;
        bool isVisible;
    };
    
    // Lista de menus disponíveis
    std::map<std::string, MenuData> menus_;
    
    // Menu atualmente aberto
    std::string currentMenu_;
    
    // Estado do sistema de menus
    bool isInitialized_;
    
    // Função auxiliar para carregar um arquivo de menu
    bool LoadMenuFile(const std::string& menuName, const std::string& filePath);
    
    // Registra as teclas de atalho
    void RegisterHotkeys();
};

} // namespace UI
} // namespace WYDBR

#endif // MODERN_MENU_INTEGRATION_H
