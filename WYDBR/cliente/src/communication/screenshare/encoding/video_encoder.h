#ifndef WYDBR_VIDEO_ENCODER_H
#define WYDBR_VIDEO_ENCODER_H

#include <vector>
#include <string>
#include <atomic>
#include <memory>
#include <functional>
#include <mutex>
#include <queue>
#include <cstdint>

#include "../screen_sharing_system.h"

namespace wydbr {
namespace communication {

// Enumeração para implementação de codificador
enum class EncoderImplementation {
    SOFTWARE_X264,      // Implementação software x264
    NVENC,              // NVIDIA NVENC
    AMF,                // AMD Advanced Media Framework
    QUICKSYNC,          // Intel QuickSync
    MEDIACODEC,         // Android MediaCodec
    VIDEOTOOLBOX,       // Apple VideoToolbox
    V4L2,               // Video4Linux2
    CUSTOM              // Implementação customizada
};

// Estrutura para configuração do codificador
struct VideoEncoderConfig {
    VideoCodec codec;
    CompressionLevel compression;
    EncoderImplementation preferredImplementation;
    int targetBitrate;
    int maxBitrate;
    int width;
    int height;
    int fps;
    int gop;             // Group of Pictures
    int bFrames;         // Número de B-frames
    int quality;         // 0-100, usado para codificação baseada em qualidade
    bool useCBR;         // Constant Bitrate
    bool useLowLatency;
    bool useHardwareAcceleration;
    int threads;
    int speedPreset;     // 0-10, 0=slowest/best quality, 10=fastest
    std::string profile; // baseline, main, high, etc.
    bool adaptiveBitrate;
    
    // Configurações específicas
    bool useSlices;      // Use multiple slices
    int sliceCount;
    bool intraRefresh;   // Use intra refresh instead of keyframes
    bool useFillerData;  // Use filler data for CBR
    
    // Construtor com valores padrão
    VideoEncoderConfig() :
        codec(VideoCodec::H264),
        compression(CompressionLevel::BALANCED),
        preferredImplementation(EncoderImplementation::SOFTWARE_X264),
        targetBitrate(5000),
        maxBitrate(10000),
        width(1920),
        height(1080),
        fps(30),
        gop(60),
        bFrames(0),
        quality(80),
        useCBR(true),
        useLowLatency(true),
        useHardwareAcceleration(true),
        threads(4),
        speedPreset(7),
        profile("baseline"),
        adaptiveBitrate(true),
        useSlices(true),
        sliceCount(4),
        intraRefresh(true),
        useFillerData(true)
    {}
};

// Estrutura para frame codificado
struct EncodedVideoFrame {
    std::vector<uint8_t> data;
    uint64_t timestamp;
    uint32_t sequenceNumber;
    int width;
    int height;
    bool isKeyFrame;
    VideoCodec codec;
    int flags;           // Flags específicas do codec
    uint32_t duration;
    float compressionRatio;
    float encodingTimeMs;
};

// Callback para frame codificado
using FrameEncodedCallback = std::function<void(const EncodedVideoFrame& frame)>;

/**
 * @class VideoEncoder
 * @brief Codificador de vídeo de alto desempenho
 * 
 * Codifica frames de vídeo usando diversos codecs e implementações,
 * com otimizações para baixa latência e qualidade adaptativa.
 */
class VideoEncoder {
public:
    VideoEncoder();
    ~VideoEncoder();
    
    /**
     * @brief Inicializa o codificador
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const VideoEncoderConfig& config = VideoEncoderConfig());
    
    /**
     * @brief Desliga o codificador e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Codifica um frame
     * @param frameData Dados do frame em formato de pixel (RGB ou YUV)
     * @param width Largura do frame
     * @param height Altura do frame
     * @param stride Stride do frame (bytes por linha)
     * @param format Formato do pixel (ex: RGB24, YUV420P)
     * @param timestamp Timestamp do frame
     * @return true se o frame foi enfileirado para codificação
     */
    bool encodeFrame(const std::vector<uint8_t>& frameData, int width, int height,
                    int stride, int format, uint64_t timestamp);
    
    /**
     * @brief Força a geração de um keyframe
     */
    void forceKeyFrame();
    
    /**
     * @brief Registra um callback para evento de frame codificado
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerFrameEncodedCallback(FrameEncodedCallback callback);
    
    /**
     * @brief Remove um callback de evento de frame codificado
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterFrameEncodedCallback(int callbackId);
    
    /**
     * @brief Verifica se o codificador está ativo
     * @return true se está ativo
     */
    bool isActive() const;
    
    /**
     * @brief Atualiza a taxa de bits alvo
     * @param targetBitrate Taxa de bits em kbps
     */
    void updateTargetBitrate(int targetBitrate);
    
    /**
     * @brief Atualiza a resolução
     * @param width Nova largura
     * @param height Nova altura
     * @return true se atualizada com sucesso
     */
    bool updateResolution(int width, int height);
    
    /**
     * @brief Atualiza a taxa de quadros
     * @param fps Nova taxa de quadros
     */
    void updateFrameRate(int fps);
    
    /**
     * @brief Define o codec de vídeo
     * @param codec Novo codec
     * @return true se alterado com sucesso
     */
    bool setCodec(VideoCodec codec);
    
    /**
     * @brief Obtém o codec atual
     * @return Codec atual
     */
    VideoCodec getCodec() const;
    
    /**
     * @brief Define o nível de compressão
     * @param level Novo nível
     */
    void setCompressionLevel(CompressionLevel level);
    
    /**
     * @brief Obtém o nível de compressão atual
     * @return Nível atual
     */
    CompressionLevel getCompressionLevel() const;
    
    /**
     * @brief Ativa ou desativa o modo de baixa latência
     * @param enable Se deve ativar
     */
    void enableLowLatencyMode(bool enable);
    
    /**
     * @brief Verifica se o modo de baixa latência está ativo
     * @return true se ativo
     */
    bool isLowLatencyModeEnabled() const;
    
    /**
     * @brief Ativa ou desativa a aceleração de hardware
     * @param enable Se deve ativar
     * @return true se a mudança foi bem-sucedida
     */
    bool enableHardwareAcceleration(bool enable);
    
    /**
     * @brief Verifica se a aceleração de hardware está ativa
     * @return true se ativa
     */
    bool isHardwareAccelerationEnabled() const;
    
    /**
     * @brief Obtém estatísticas de codificação
     * @param outBitrate Taxa de bits atual em kbps (saída)
     * @param outFps Taxa de quadros atual (saída)
     * @param outAvgEncodeTimeMs Tempo médio de codificação em ms (saída)
     * @param outCompressionRatio Taxa de compressão média (saída)
     * @param outQueueSize Tamanho atual da fila (saída)
     */
    void getStats(int& outBitrate, float& outFps, float& outAvgEncodeTimeMs,
                 float& outCompressionRatio, int& outQueueSize) const;
    
    /**
     * @brief Detecta as implementações de hardware disponíveis
     * @return Lista de implementações disponíveis
     */
    std::vector<EncoderImplementation> detectAvailableHardwareEncoders();
    
    /**
     * @brief Obtém a implementação ativa
     * @return Implementação ativa
     */
    EncoderImplementation getActiveImplementation() const;
    
private:
    // Estrutura interna para frame a codificar
    struct FrameToEncode {
        std::vector<uint8_t> data;
        int width;
        int height;
        int stride;
        int format;
        uint64_t timestamp;
    };
    
    // Thread de codificação
    void encodeThread();
    
    // Métodos de implementação específicos
    bool initializeEncoder();
    bool initializeX264();
    bool initializeNVENC();
    bool initializeAMF();
    bool initializeQuickSync();
    bool initializeMediaCodec();
    bool initializeVideoToolbox();
    bool initializeV4L2();
    bool initializeCustom();
    
    // Métodos específicos de codificação
    bool encodeWithX264(const FrameToEncode& frame, EncodedVideoFrame& encodedFrame);
    bool encodeWithNVENC(const FrameToEncode& frame, EncodedVideoFrame& encodedFrame);
    bool encodeWithAMF(const FrameToEncode& frame, EncodedVideoFrame& encodedFrame);
    bool encodeWithQuickSync(const FrameToEncode& frame, EncodedVideoFrame& encodedFrame);
    bool encodeWithMediaCodec(const FrameToEncode& frame, EncodedVideoFrame& encodedFrame);
    bool encodeWithVideoToolbox(const FrameToEncode& frame, EncodedVideoFrame& encodedFrame);
    bool encodeWithV4L2(const FrameToEncode& frame, EncodedVideoFrame& encodedFrame);
    bool encodeWithCustom(const FrameToEncode& frame, EncodedVideoFrame& encodedFrame);
    
    // Métodos utilitários
    void notifyFrameEncoded(const EncodedVideoFrame& frame);
    EncoderImplementation selectBestImplementation();
    bool reinitializeWithNewSettings();
    
    // Estado interno
    bool m_initialized;
    VideoEncoderConfig m_config;
    std::atomic<bool> m_isActive;
    EncoderImplementation m_activeImplementation;
    std::atomic<uint32_t> m_nextSequenceNumber;
    std::atomic<bool> m_forceNextKeyFrame;
    
    // Fila de codificação
    std::queue<FrameToEncode> m_frameQueue;
    std::mutex m_queueMutex;
    
    // Thread
    std::unique_ptr<std::thread> m_encodeThread;
    std::atomic<bool> m_threadShouldRun;
    
    // Callbacks
    std::vector<std::pair<int, FrameEncodedCallback>> m_callbacks;
    int m_nextCallbackId;
    std::mutex m_callbackMutex;
    
    // Estatísticas
    struct Stats {
        std::atomic<int> bitrate;
        std::atomic<float> fps;
        std::atomic<float> avgEncodeTimeMs;
        std::atomic<float> compressionRatio;
        std::chrono::steady_clock::time_point lastFrameTime;
        std::vector<float> encodeTimeSamples;
        std::vector<float> compressionRatioSamples;
        mutable std::mutex samplesMutex;
    } m_stats;
    
    // Estado de codec
    void* m_codec;
    void* m_context;
};

} // namespace communication
} // namespace wydbr

#endif // WYDBR_VIDEO_ENCODER_H
