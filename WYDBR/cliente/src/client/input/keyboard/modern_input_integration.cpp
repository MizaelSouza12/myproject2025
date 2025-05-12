/**
 * @file modern_input_integration.cpp
 * @brief Integração do sistema de input moderno com o WYD original
 */

#include "../../implementation/ModernMenuIntegration.h"
#include <iostream>

// Código para integrar com o sistema de entrada do WYD
extern "C" {

/**
 * @brief Função chamada pelo WYD quando uma tecla é pressionada
 * @param keyCode Código da tecla pressionada
 * @param altPressed Se a tecla ALT está pressionada
 * @param ctrlPressed Se a tecla CTRL está pressionada
 * @param shiftPressed Se a tecla SHIFT está pressionada
 * @return 1 se o evento foi processado, 0 caso contrário
 */
int wydbr_process_modern_key_event(int keyCode, int altPressed, int ctrlPressed, int shiftPressed) {
    // Delegar para o sistema moderno de menus
    bool processed = wydbr::ui::ModernMenuIntegration::getInstance().processKeyEvent(
        keyCode, altPressed != 0, ctrlPressed != 0, shiftPressed != 0);
    
    return processed ? 1 : 0;
}

/**
 * @brief Função chamada pelo WYD quando ocorre um clique do mouse
 * @param x Coordenada X do clique
 * @param y Coordenada Y do clique
 * @param rightClick Se é um clique com o botão direito
 * @return 1 se o evento foi processado, 0 caso contrário
 */
int wydbr_process_modern_click_event(int x, int y, int rightClick) {
    // Delegar para o sistema moderno de menus
    bool processed = wydbr::ui::ModernMenuIntegration::getInstance().processClickEvent(
        x, y, rightClick != 0);
    
    return processed ? 1 : 0;
}

/**
 * @brief Função chamada pelo WYD para inicializar o sistema moderno de menus
 */
void wydbr_initialize_modern_menu() {
    wydbr::ui::ModernMenuIntegration::getInstance().initialize();
}

/**
 * @brief Função chamada pelo WYD quando há atividade de compartilhamento
 * @param remoteUser Nome do usuário remoto conectado
 */
void wydbr_set_remote_user(const char* remoteUser) {
    if (remoteUser && *remoteUser) {
        wydbr::ui::ModernMenuIntegration::getInstance().showSharingOverlay(remoteUser);
    } else {
        wydbr::ui::ModernMenuIntegration::getInstance().hideSharingOverlay();
    }
}

/**
 * @brief Função chamada pelo WYD quando há um clique no nome do usuário remoto
 * @param x Coordenada X do clique
 * @param y Coordenada Y do clique
 * @param rightClick Se é um clique com o botão direito
 * @return 1 se o clique foi processado, 0 caso contrário
 */
int wydbr_handle_remote_user_click(int x, int y, int rightClick) {
    bool processed = wydbr::ui::ModernMenuIntegration::getInstance().processRemoteUserClick(
        x, y, rightClick != 0);
    
    return processed ? 1 : 0;
}

/**
 * @brief Função chamada pelo WYD para atualizar os indicadores de desempenho
 * @param fps Taxa de quadros atual
 * @param ping Ping em ms
 * @param serverLatency Latência do servidor em ms
 * @param networkStatus Status da rede (0-3)
 */
void wydbr_update_performance_indicators(int fps, int ping, int serverLatency, int networkStatus) {
    wydbr::ui::ModernMenuIntegration::getInstance().updatePerformanceIndicators(
        fps, ping, serverLatency, networkStatus);
}

} // extern "C"
