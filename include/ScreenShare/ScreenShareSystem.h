
/**
 * @file ScreenShareSystem.h
 * @brief Sistema de compartilhamento de tela para WYDBR 2.0
 */

#ifndef WYDBR_SCREEN_SHARE_SYSTEM_H
#define WYDBR_SCREEN_SHARE_SYSTEM_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace WYDBR {
namespace ScreenShare {

// Forward declarations
class CaptureManager;
class CodecManager;
class ScreenShareNetworkManager;
class ShareSessionManager;

// Tipos
using ScreenShareID = uint32_t;
using ClientID = uint32_t;

// Enum para codec de vídeo
enum class VideoCodec {
    H264,
    H265,
    VP8,
    VP9,
    MJPEG
};

// Estrutura de configuração de compartilhamento
struct ScreenShareConfig {
    uint16_t width;
    uint16_t height;
    uint8_t frameRate;
    VideoCodec codec;
    uint8_t quality;  // 0-100
    bool captureArea;  // true = área específica, false = tela inteira
    uint16_t captureX;
    uint16_t captureY;
    uint16_t captureWidth;
    uint16_t captureHeight;
};

// Estrutura de frame
struct FrameData {
    uint32_t timestamp;
    uint16_t width;
    uint16_t height;
    std::vector<uint8_t> data;
};

// Estrutura de frame codificado
struct EncodedFrame {
    uint32_t timestamp;
    VideoCodec codec;
    uint16_t width;
    uint16_t height;
    std::vector<uint8_t> data;
};

/**
 * Classe principal do sistema de compartilhamento de tela
 */
class ScreenShareSystem {
public:
    ScreenShareSystem();
    ~ScreenShareSystem();
    
    /**
     * Inicializa o sistema de compartilhamento de tela
     * @param configPath Caminho para o arquivo de configuração
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(const std::string& configPath = "");
    
    /**
     * Inicia o sistema de compartilhamento de tela
     * @param port Porta para escutar (0 = usar valor da configuração)
     * @return true se o início foi bem-sucedido, false caso contrário
     */
    bool Start(uint16_t port = 0);
    
    /**
     * Encerra o sistema de compartilhamento de tela
     */
    void Shutdown();
    
    /**
     * Processa a lógica do sistema de compartilhamento de tela
     * @param deltaTime Tempo desde o último update em milissegundos
     */
    void Update(float deltaTime);
    
    // Gerenciamento de sessões
    ScreenShareID StartSharing(ClientID clientId, const ScreenShareConfig& config);
    bool StopSharing(ScreenShareID shareId);
    bool ViewScreenShare(ScreenShareID shareId, ClientID viewerClientId);
    bool StopViewingScreenShare(ScreenShareID shareId, ClientID viewerClientId);
    
    // Gerenciamento de configuração
    bool UpdateShareConfig(ScreenShareID shareId, const ScreenShareConfig& config);
    
private:
    // Evitar cópias
    ScreenShareSystem(const ScreenShareSystem&) = delete;
    ScreenShareSystem& operator=(const ScreenShareSystem&) = delete;
    
    // Componentes internos
    std::unique_ptr<CaptureManager> m_captureManager;
    std::unique_ptr<CodecManager> m_codecManager;
    std::unique_ptr<ScreenShareNetworkManager> m_networkManager;
    std::unique_ptr<ShareSessionManager> m_sessionManager;
    
    // Estado
    bool m_initialized;
    bool m_running;
    uint16_t m_port;
    std::string m_configPath;
    
    // Métodos internos
    bool LoadConfig();
    bool InitializeManagers();
    void ProcessFrames();
};

} // namespace ScreenShare
} // namespace WYDBR

#endif // WYDBR_SCREEN_SHARE_SYSTEM_H
