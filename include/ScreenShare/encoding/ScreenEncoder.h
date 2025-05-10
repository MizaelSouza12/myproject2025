/**
 * @file ScreenEncoder.h
 * @brief Sistema de codificação de vídeo para compartilhamento de tela
 */

#ifndef WYDBR_SCREEN_ENCODER_H
#define WYDBR_SCREEN_ENCODER_H

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "../capture/ScreenCaptureManager.h"

namespace WYDBR {
namespace ScreenShare {

/**
 * Tipo de codec de vídeo
 */
enum class VideoCodecType {
    H264,       // H.264/AVC - compatível e eficiente
    H265,       // H.265/HEVC - mais eficiente, menos compatível
    VP8,        // VP8 - codec aberto
    VP9,        // VP9 - mais eficiente que VP8
    AV1         // AV1 - codec aberto de nova geração
};

/**
 * Perfil de codificação
 */
enum class EncodingProfile {
    Baseline,   // Perfil básico - compatibilidade máxima
    Main,       // Perfil principal - balanceado
    High,       // Perfil alto - melhor qualidade
    ScreenShare // Perfil otimizado para compartilhamento de tela
};

/**
 * Uso de hardware para codificação
 */
enum class HardwareAcceleration {
    None,       // Sem aceleração de hardware
    Intel,      // Aceleração Intel QuickSync
    NVIDIA,     // Aceleração NVIDIA NVENC
    AMD,        // Aceleração AMD VCE/VCN
    Auto        // Detecção automática
};

/**
 * Configuração de codificação
 */
struct EncoderConfig {
    VideoCodecType codecType;          // Tipo de codec
    EncodingProfile profile;           // Perfil de codificação
    HardwareAcceleration acceleration; // Aceleração de hardware
    int width;                         // Largura do vídeo
    int height;                        // Altura do vídeo
    int bitrate;                       // Taxa de bits em kbps
    int frameRate;                     // Taxa de frames alvo
    int keyframeInterval;              // Intervalo de keyframes
    int complexity;                    // Complexidade (0-10)
    bool enableLowLatency;             // Otimizar para baixa latência
    int threadCount;                   // Número de threads (0 = auto)
};

/**
 * Estrutura para um frame codificado
 */
struct EncodedFrame {
    uint8_t* data;           // Dados codificados
    size_t dataSize;         // Tamanho dos dados em bytes
    int64_t timestamp;       // Timestamp do frame
    bool keyFrame;           // É um keyframe
    int width;               // Largura do frame
    int height;              // Altura do frame
    VideoCodecType codecType; // Tipo de codec usado
};

/**
 * Callback para receber frames codificados
 */
using EncodedFrameCallback = std::function<void(const EncodedFrame& frame)>;

/**
 * Interface para codificador de vídeo
 */
class IVideoEncoder {
public:
    virtual ~IVideoEncoder() = default;
    
    /**
     * Inicializa o codificador
     * @param config Configuração do codificador
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const EncoderConfig& config) = 0;
    
    /**
     * Codifica um frame
     * @param frame Frame a ser codificado
     * @param forceKeyframe Força geração de keyframe
     * @return Frame codificado ou nullptr se falhar
     */
    virtual std::unique_ptr<EncodedFrame> EncodeFrame(const CapturedFrame& frame, bool forceKeyframe = false) = 0;
    
    /**
     * Finaliza a codificação (libera recursos)
     */
    virtual void Finalize() = 0;
    
    /**
     * Verifica se o codificador está inicializado
     * @return true se estiver inicializado
     */
    virtual bool IsInitialized() const = 0;
    
    /**
     * Obtém o tipo de codec
     * @return Tipo do codec
     */
    virtual VideoCodecType GetCodecType() const = 0;
    
    /**
     * Obtém nome do codec
     * @return Nome do codec
     */
    virtual std::string GetCodecName() const = 0;
    
    /**
     * Verifica se aceleração de hardware está sendo usada
     * @return true se aceleração de hardware estiver ativa
     */
    virtual bool IsUsingHardwareAcceleration() const = 0;
    
    /**
     * Obtém estatísticas de codificação
     * @return String formatada com estatísticas
     */
    virtual std::string GetStatistics() const = 0;
    
    /**
     * Altera bitrate durante a codificação
     * @param bitrateKbps Novo bitrate em kbps
     * @return true se alterado com sucesso
     */
    virtual bool ChangeBitrate(int bitrateKbps) = 0;
};

/**
 * Implementação de codificador H.264
 */
class H264Encoder : public IVideoEncoder {
public:
    H264Encoder();
    ~H264Encoder() override;
    
    bool Initialize(const EncoderConfig& config) override;
    std::unique_ptr<EncodedFrame> EncodeFrame(const CapturedFrame& frame, bool forceKeyframe = false) override;
    void Finalize() override;
    bool IsInitialized() const override;
    VideoCodecType GetCodecType() const override;
    std::string GetCodecName() const override;
    bool IsUsingHardwareAcceleration() const override;
    std::string GetStatistics() const override;
    bool ChangeBitrate(int bitrateKbps) override;
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * Factory para criar codificadores de vídeo
 */
class VideoEncoderFactory {
public:
    /**
     * Cria codificador baseado no tipo
     * @param codecType Tipo de codec
     * @return Ponteiro para codificador
     */
    static std::unique_ptr<IVideoEncoder> Create(VideoCodecType codecType);
    
    /**
     * Obtém lista de codecs disponíveis
     * @return Lista de tipos de codec
     */
    static std::vector<VideoCodecType> GetAvailableCodecs();
    
    /**
     * Obtém lista de acelerações de hardware disponíveis
     * @return Lista de tipos de aceleração
     */
    static std::vector<HardwareAcceleration> GetAvailableAccelerations();
    
    /**
     * Determina o melhor codec para as condições atuais
     * @return Tipo de codec recomendado
     */
    static VideoCodecType GetRecommendedCodec();
    
    /**
     * Determina a melhor aceleração de hardware para as condições atuais
     * @return Tipo de aceleração recomendada
     */
    static HardwareAcceleration GetRecommendedAcceleration();
};

/**
 * Gerenciador de codificação de vídeo
 */
class ScreenEncoder {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static ScreenEncoder& GetInstance();
    
    /**
     * Inicializa o codificador
     * @param config Configuração de codificação
     * @return true se inicializado com sucesso
     */
    bool Initialize(const EncoderConfig& config);
    
    /**
     * Inicia a codificação
     * @param callback Função para receber frames codificados
     * @return true se iniciado com sucesso
     */
    bool Start(EncodedFrameCallback callback);
    
    /**
     * Para a codificação
     */
    void Stop();
    
    /**
     * Processa um frame capturado
     * @param frame Frame capturado
     */
    void ProcessCapturedFrame(const CapturedFrame& frame);
    
    /**
     * Força a geração de um keyframe
     */
    void ForceKeyframe();
    
    /**
     * Altera bitrate durante a codificação
     * @param bitrateKbps Novo bitrate em kbps
     * @return true se alterado com sucesso
     */
    bool ChangeBitrate(int bitrateKbps);
    
    /**
     * Obtém estatísticas de codificação
     * @return String formatada com estatísticas
     */
    std::string GetStatistics() const;
    
private:
    // Implementação privada do singleton
    ScreenEncoder();
    ~ScreenEncoder();
    
    // Membros privados
    std::unique_ptr<IVideoEncoder> m_encoder;
    EncoderConfig m_config;
    EncodedFrameCallback m_callback;
    mutable std::mutex m_mutex;
    std::atomic<bool> m_isInitialized;
    std::atomic<bool> m_isEncoding;
};

} // namespace ScreenShare
} // namespace WYDBR

#endif // WYDBR_SCREEN_ENCODER_H
