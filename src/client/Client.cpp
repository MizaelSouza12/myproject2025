/**
 * @file Client.cpp
 * @brief Implementação da classe principal do cliente WYDBR 2.0
 */

#include "../../include/Client/Client.h"
#include "game/GameEngine.h"
#include "network/ClientNetwork.h"
#include "ui/UserInterface.h"
#include "audio/AudioEngine.h"
#include "graphics/GraphicsEngine.h"

#ifdef WYDBR_ENABLE_VOIP
#include "../../include/VoIP/VoIPClient.h"
#endif

#ifdef WYDBR_ENABLE_SCREEN_SHARE
#include "../../include/ScreenShare/ScreenShareClient.h"
#endif

#include <iostream>
#include <chrono>
#include <thread>

namespace WYDBR {

// Forward declarations para componentes internos
class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();
    
private:
    // Implementação interna
};

class ClientNetwork {
public:
    ClientNetwork();
    ~ClientNetwork();
    
    bool Initialize();
    bool Connect(const std::string& address, uint16_t port);
    void Disconnect();
    void Update();
    bool IsConnected() const;
    
private:
    std::string m_serverAddress;
    uint16_t m_serverPort;
    bool m_connected;
    
    // Implementação interna
};

class UserInterface {
public:
    UserInterface();
    ~UserInterface();
    
    bool Initialize(int width, int height);
    void Render();
    void Update(float deltaTime);
    void Shutdown();
    
private:
    int m_width;
    int m_height;
    
    // Implementação interna
};

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();
    
    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();
    
private:
    // Implementação interna
};

class GraphicsEngine {
public:
    GraphicsEngine();
    ~GraphicsEngine();
    
    bool Initialize(int width, int height, bool fullscreen);
    void BeginFrame();
    void EndFrame();
    void Render();
    void Shutdown();
    
private:
    int m_width;
    int m_height;
    bool m_fullscreen;
    
    // Implementação interna
};

#ifdef WYDBR_ENABLE_VOIP
class VoIPClient {
public:
    VoIPClient();
    ~VoIPClient();
    
    bool Initialize();
    bool Connect(const std::string& address, uint16_t port);
    void Disconnect();
    void Update();
    
private:
    // Implementação interna
};
#endif

#ifdef WYDBR_ENABLE_SCREEN_SHARE
class ScreenShareClient {
public:
    ScreenShareClient();
    ~ScreenShareClient();
    
    bool Initialize();
    bool Connect(const std::string& address, uint16_t port);
    void Disconnect();
    void Update();
    
private:
    // Implementação interna
};
#endif

// Implementação da classe Client
Client::Client()
    : m_initialized(false), m_running(false), m_serverPort(0),
      m_windowWidth(1024), m_windowHeight(768), m_fullscreen(false) {
}

Client::~Client() {
    Shutdown();
}

bool Client::Initialize(const std::string& serverAddress, uint16_t serverPort, 
                        int windowWidth, int windowHeight, bool fullscreen) {
    if (m_initialized) {
        std::cerr << "Cliente já inicializado!" << std::endl;
        return false;
    }
    
    m_serverAddress = serverAddress;
    m_serverPort = serverPort;
    m_windowWidth = windowWidth;
    m_windowHeight = windowHeight;
    m_fullscreen = fullscreen;
    
    // Criar componentes
    m_gameEngine = std::make_unique<GameEngine>();
    m_network = std::make_unique<ClientNetwork>();
    m_ui = std::make_unique<UserInterface>();
    m_audio = std::make_unique<AudioEngine>();
    m_graphics = std::make_unique<GraphicsEngine>();
    
#ifdef WYDBR_ENABLE_VOIP
    m_voipClient = std::make_unique<VoIPClient>();
#endif

#ifdef WYDBR_ENABLE_SCREEN_SHARE
    m_screenShareClient = std::make_unique<ScreenShareClient>();
#endif
    
    // Inicializar subsistemas
    if (!InitializeSubsystems()) {
        std::cerr << "Falha ao inicializar subsistemas." << std::endl;
        return false;
    }
    
    // Conectar ao servidor
    if (!ConnectToServer()) {
        std::cerr << "Aviso: Não foi possível conectar ao servidor. Continuando em modo offline." << std::endl;
        // Continua a inicialização mesmo sem conexão
    }
    
    m_initialized = true;
    m_running = true;
    
    std::cout << "Cliente inicializado com sucesso." << std::endl;
    return true;
}

void Client::Update() {
    if (!m_running) return;
    
    // Calcular delta time (tempo desde último frame)
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;
    
    // Atualizar rede
    m_network->Update();
    
    // Atualizar motor de jogo
    m_gameEngine->Update(deltaTime);
    
    // Atualizar áudio
    m_audio->Update(deltaTime);
    
    // Atualizar UI
    m_ui->Update(deltaTime);
    
#ifdef WYDBR_ENABLE_VOIP
    // Atualizar VoIP
    if (m_voipClient) {
        m_voipClient->Update();
    }
#endif

#ifdef WYDBR_ENABLE_SCREEN_SHARE
    // Atualizar compartilhamento de tela
    if (m_screenShareClient) {
        m_screenShareClient->Update();
    }
#endif
}

void Client::Render() {
    if (!m_running) return;
    
    // Iniciar frame
    m_graphics->BeginFrame();
    
    // Renderizar o jogo
    m_graphics->Render();
    
    // Renderizar UI
    m_ui->Render();
    
    // Finalizar frame
    m_graphics->EndFrame();
}

void Client::Shutdown() {
    if (!m_initialized) return;
    
    m_running = false;
    
    std::cout << "Encerrando cliente..." << std::endl;
    
    // Desconectar
    m_network->Disconnect();
    
#ifdef WYDBR_ENABLE_VOIP
    // Encerrar VoIP
    if (m_voipClient) {
        m_voipClient->Disconnect();
    }
#endif

#ifdef WYDBR_ENABLE_SCREEN_SHARE
    // Encerrar compartilhamento de tela
    if (m_screenShareClient) {
        m_screenShareClient->Disconnect();
    }
#endif
    
    // Encerrar subsistemas na ordem inversa
    if (m_graphics) {
        m_graphics->Shutdown();
    }
    
    if (m_ui) {
        m_ui->Shutdown();
    }
    
    if (m_audio) {
        m_audio->Shutdown();
    }
    
    if (m_gameEngine) {
        m_gameEngine->Shutdown();
    }
    
    // Liberar recursos
    m_screenShareClient.reset();
    m_voipClient.reset();
    m_graphics.reset();
    m_audio.reset();
    m_ui.reset();
    m_network.reset();
    m_gameEngine.reset();
    
    m_initialized = false;
    
    std::cout << "Cliente encerrado." << std::endl;
}

bool Client::IsRunning() const {
    return m_running;
}

bool Client::InitializeSubsystems() {
    // Inicializar motor gráfico
    if (!m_graphics->Initialize(m_windowWidth, m_windowHeight, m_fullscreen)) {
        std::cerr << "Falha ao inicializar motor gráfico." << std::endl;
        return false;
    }
    
    // Inicializar motor de áudio
    if (!m_audio->Initialize()) {
        std::cerr << "Falha ao inicializar motor de áudio." << std::endl;
        return false;
    }
    
    // Inicializar interface de usuário
    if (!m_ui->Initialize(m_windowWidth, m_windowHeight)) {
        std::cerr << "Falha ao inicializar interface de usuário." << std::endl;
        return false;
    }
    
    // Inicializar motor de jogo
    if (!m_gameEngine->Initialize()) {
        std::cerr << "Falha ao inicializar motor de jogo." << std::endl;
        return false;
    }
    
    // Inicializar rede
    if (!m_network->Initialize()) {
        std::cerr << "Falha ao inicializar sistema de rede." << std::endl;
        return false;
    }
    
#ifdef WYDBR_ENABLE_VOIP
    // Inicializar VoIP
    if (m_voipClient && !m_voipClient->Initialize()) {
        std::cerr << "Aviso: Falha ao inicializar sistema VoIP." << std::endl;
        // Continua mesmo com falha no VoIP
    }
#endif

#ifdef WYDBR_ENABLE_SCREEN_SHARE
    // Inicializar compartilhamento de tela
    if (m_screenShareClient && !m_screenShareClient->Initialize()) {
        std::cerr << "Aviso: Falha ao inicializar sistema de compartilhamento de tela." << std::endl;
        // Continua mesmo com falha no compartilhamento
    }
#endif
    
    return true;
}

bool Client::ConnectToServer() {
    if (!m_network->Connect(m_serverAddress, m_serverPort)) {
        return false;
    }
    
#ifdef WYDBR_ENABLE_VOIP
    // Conectar ao servidor VoIP (usar mesma porta + 1)
    if (m_voipClient) {
        m_voipClient->Connect(m_serverAddress, m_serverPort + 1);
    }
#endif

#ifdef WYDBR_ENABLE_SCREEN_SHARE
    // Conectar ao servidor de compartilhamento (usar mesma porta + 2)
    if (m_screenShareClient) {
        m_screenShareClient->Connect(m_serverAddress, m_serverPort + 2);
    }
#endif
    
    return true;
}

} // namespace WYDBR