/**
 * @file Client.h
 * @brief Classe principal do cliente WYDBR 2.0
 */

#ifndef WYDBR_CLIENT_H
#define WYDBR_CLIENT_H

#include <string>
#include <memory>
#include <cstdint>

namespace WYDBR {

// Forward declarations
class GameEngine;
class ClientNetwork;
class UserInterface;
class AudioEngine;
class GraphicsEngine;
class VoIPClient;
class ScreenShareClient;

/**
 * Classe principal do cliente WYDBR
 */
class Client {
public:
    Client();
    ~Client();
    
    /**
     * Inicializa o cliente
     * @param serverAddress Endereço do servidor
     * @param serverPort Porta do servidor
     * @param windowWidth Largura da janela
     * @param windowHeight Altura da janela
     * @param fullscreen Modo tela cheia
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(const std::string& serverAddress, uint16_t serverPort, 
                    int windowWidth = 1024, int windowHeight = 768, bool fullscreen = false);
    
    /**
     * Atualiza a lógica do cliente
     */
    void Update();
    
    /**
     * Renderiza o cliente
     */
    void Render();
    
    /**
     * Encerra o cliente
     */
    void Shutdown();
    
    /**
     * Verifica se o cliente está em execução
     * @return true se estiver em execução, false caso contrário
     */
    bool IsRunning() const;
    
private:
    // Componentes principais
    std::unique_ptr<GameEngine> m_gameEngine;
    std::unique_ptr<ClientNetwork> m_network;
    std::unique_ptr<UserInterface> m_ui;
    std::unique_ptr<AudioEngine> m_audio;
    std::unique_ptr<GraphicsEngine> m_graphics;
    
    // Componentes de comunicação
    std::unique_ptr<VoIPClient> m_voipClient;
    std::unique_ptr<ScreenShareClient> m_screenShareClient;
    
    // Estado
    bool m_initialized;
    bool m_running;
    std::string m_serverAddress;
    uint16_t m_serverPort;
    
    // Configurações
    int m_windowWidth;
    int m_windowHeight;
    bool m_fullscreen;
    
    // Métodos internos
    bool InitializeSubsystems();
    bool ConnectToServer();
};

} // namespace WYDBR

#endif // WYDBR_CLIENT_H