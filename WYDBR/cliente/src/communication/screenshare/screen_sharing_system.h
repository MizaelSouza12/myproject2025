#ifndef WYDBR_SCREEN_SHARING_SYSTEM_H
#define WYDBR_SCREEN_SHARING_SYSTEM_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>

namespace wydbr {
namespace communication {

// Enumeração para nível de qualidade
enum class StreamQuality {
    LOW,            // Baixa qualidade (máxima economia de banda)
    MEDIUM,         // Qualidade média (equilíbrio)
    HIGH,           // Alta qualidade
    VERY_HIGH       // Qualidade máxima
};

// Enumeração para taxa de quadros
enum class FrameRate {
    FPS_15,         // 15 quadros por segundo
    FPS_30,         // 30 quadros por segundo
    FPS_60          // 60 quadros por segundo
};

// Enumeração para área de captura
enum class CaptureArea {
    FULL_SCREEN,    // Tela inteira
    GAME_WINDOW,    // Apenas janela do jogo
    CUSTOM_REGION   // Região personalizada
};

// Enumeração para codec de vídeo
enum class VideoCodec {
    H264,           // H.264/AVC
    H265,           // H.265/HEVC
    VP9,            // VP9
    AV1,            // AV1
    FAST_CODEC      // Codec proprietário ultra rápido
};

// Enumeração para permissão de compartilhamento
enum class SharingPermission {
    VIEW,           // Permissão para visualizar
    INTERACT,       // Permissão para interagir (mouse/teclado)
    CONTROL,        // Permissão para controle total
    RECORD          // Permissão para gravar
};

// Enumeração para nivel de compressão
enum class CompressionLevel {
    NONE,           // Sem compressão
    FAST,           // Compressão rápida
    BALANCED,       // Equilíbrio entre velocidade e qualidade
    HIGH            // Alta compressão
};

// Estrutura para configuração do compartilhamento
struct ScreenSharingConfig {
    StreamQuality quality;
    FrameRate frameRate;
    CaptureArea captureArea;
    VideoCodec codec;
    CompressionLevel compression;
    bool enableAudio;
    bool enableMouseIndicator;
    bool enableSecurityFeatures;
    bool enableLowLatencyMode;
    bool enableQualityScaling;
    int maxBitrateKbps;
    int customWidth;
    int customHeight;
    int customX;
    int customY;
    int bufferingSizeMs;
    
    // Construtor com valores padrão
    ScreenSharingConfig() :
        quality(StreamQuality::MEDIUM),
        frameRate(FrameRate::FPS_30),
        captureArea(CaptureArea::GAME_WINDOW),
        codec(VideoCodec::H264),
        compression(CompressionLevel::BALANCED),
        enableAudio(true),
        enableMouseIndicator(true),
        enableSecurityFeatures(true),
        enableLowLatencyMode(true),
        enableQualityScaling(true),
        maxBitrateKbps(5000),
        customWidth(0),
        customHeight(0),
        customX(0),
        customY(0),
        bufferingSizeMs(100)
    {}
};

// Estrutura para informações de sessão
struct SessionInfo {
    std::string sessionId;
    std::string hostId;
    std::vector<std::string> viewerIds;
    ScreenSharingConfig config;
    bool isActive;
    bool isPrivate;
    std::string name;
    int64_t startTime;
    int64_t lastUpdateTime;
};

// Informações sobre quadro
struct FrameInfo {
    int width;
    int height;
    int stride;
    uint64_t timestamp;
    uint32_t sequenceNumber;
    VideoCodec codec;
    uint32_t encodedSizeBytes;
    float compressionRatio;
    float encodingTimeMs;
    bool isKeyFrame;
};

// Callback para evento de quadro recebido
using FrameReceivedCallback = std::function<void(const std::vector<uint8_t>& frameData, const FrameInfo& info)>;

// Callback para evento de permissão
using PermissionChangeCallback = std::function<void(const std::string& userId, 
                                                  SharingPermission permission,
                                                  bool granted)>;

// Callback para evento de erro
using ErrorCallback = std::function<void(int errorCode, const std::string& errorMessage)>;

// Callback para evento de estado de sessão
using SessionStateCallback = std::function<void(const std::string& sessionId, bool isActive)>;

// Callback para evento de estatísticas
using StatisticsCallback = std::function<void(int bitrateKbps, int fps, int latencyMs, 
                                            float packetLossPercent, float jitterMs)>;

/**
 * @class ScreenSharingSystem
 * @brief Sistema de compartilhamento de tela seguro
 * 
 * Permite o compartilhamento de tela entre jogadores com controles
 * avançados de permissão, codificação eficiente, e baixa latência.
 */
class ScreenSharingSystem {
public:
    /**
     * @brief Obtém a instância única do sistema (singleton)
     * @return Referência para a instância
     */
    static ScreenSharingSystem& getInstance();
    
    /**
     * @brief Inicializa o sistema de compartilhamento de tela
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const ScreenSharingConfig& config = ScreenSharingConfig());
    
    /**
     * @brief Desliga o sistema e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Inicia uma sessão de compartilhamento (como host)
     * @param sessionName Nome da sessão
     * @param isPrivate Se a sessão é privada (apenas por convite)
     * @return ID da sessão criada ou string vazia se falhar
     */
    std::string startSharing(const std::string& sessionName = Screen
