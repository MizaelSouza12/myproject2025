/**
 * @file VoIPCodecs.h
 * @brief Sistema de codecs para compressão de áudio VoIP
 */

#ifndef WYDBR_VOIP_CODECS_H
#define WYDBR_VOIP_CODECS_H

#include <memory>
#include <vector>
#include <string>
#include <cstdint>

namespace WYDBR {
namespace VoIP {

/**
 * Tipo de codec de áudio
 */
enum class CodecType {
    OPUS,     // Codec Opus - alta qualidade, baixa latência
    SPEEX,    // Codec Speex - otimizado para voz
    G711,     // G.711 - codec tradicional de telefonia
    G722,     // G.722 - qualidade melhor que G.711
    CUSTOM    // Codec personalizado
};

/**
 * Configuração para codecs
 */
struct CodecConfig {
    CodecType type;              // Tipo de codec
    int bitrate;                 // Taxa de bits (bits por segundo)
    int complexity;              // Complexidade de codificação (0-10)
    int packetLossPercentage;    // Perda de pacotes estimada (0-100)
    bool useFEC;                 // Usar Forward Error Correction
    bool useDTX;                 // Usar Discontinuous Transmission
    int frameSize;               // Tamanho do frame em ms (10, 20, 40, 60)
};

/**
 * Interface para codecs de áudio
 */
class IAudioCodec {
public:
    virtual ~IAudioCodec() = default;
    
    /**
     * Inicializa o codec
     * @param config Configuração do codec
     * @param sampleRate Taxa de amostragem
     * @param channels Número de canais
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const CodecConfig& config, int sampleRate, int channels) = 0;
    
    /**
     * Codifica dados de áudio
     * @param input Dados de entrada (PCM)
     * @param inputSize Tamanho dos dados de entrada em bytes
     * @param output Buffer para dados codificados
     * @param outputSize Tamanho do buffer de saída
     * @return Tamanho dos dados codificados ou -1 em caso de erro
     */
    virtual int Encode(const int16_t* input, size_t inputSize, uint8_t* output, size_t outputSize) = 0;
    
    /**
     * Decodifica dados de áudio
     * @param input Dados codificados
     * @param inputSize Tamanho dos dados codificados
     * @param output Buffer para dados decodificados (PCM)
     * @param outputSize Tamanho do buffer de saída
     * @return Tamanho dos dados decodificados ou -1 em caso de erro
     */
    virtual int Decode(const uint8_t* input, size_t inputSize, int16_t* output, size_t outputSize) = 0;
    
    /**
     * Obtém o tipo de codec
     * @return Tipo do codec
     */
    virtual CodecType GetType() const = 0;
    
    /**
     * Obtém o nome do codec
     * @return Nome do codec
     */
    virtual std::string GetName() const = 0;
    
    /**
     * Obtém o bitrate atual
     * @return Bitrate em bits por segundo
     */
    virtual int GetBitrate() const = 0;
    
    /**
     * Define o bitrate
     * @param bitrate Novo bitrate em bits por segundo
     * @return true se alterado com sucesso
     */
    virtual bool SetBitrate(int bitrate) = 0;
    
    /**
     * Obtém o tamanho típico de um frame codificado
     * @return Tamanho típico em bytes
     */
    virtual int GetTypicalFrameSize() const = 0;
    
    /**
     * Verifica se o codec suporta FEC (Forward Error Correction)
     * @return true se suportar
     */
    virtual bool SupportsFEC() const = 0;
    
    /**
     * Ativa ou desativa FEC
     * @param enabled Estado de ativação
     * @return true se alterado com sucesso
     */
    virtual bool SetFEC(bool enabled) = 0;
};

/**
 * Implementação do codec Opus
 */
class OpusCodec : public IAudioCodec {
public:
    OpusCodec();
    ~OpusCodec() override;
    
    bool Initialize(const CodecConfig& config, int sampleRate, int channels) override;
    int Encode(const int16_t* input, size_t inputSize, uint8_t* output, size_t outputSize) override;
    int Decode(const uint8_t* input, size_t inputSize, int16_t* output, size_t outputSize) override;
    CodecType GetType() const override;
    std::string GetName() const override;
    int GetBitrate() const override;
    bool SetBitrate(int bitrate) override;
    int GetTypicalFrameSize() const override;
    bool SupportsFEC() const override;
    bool SetFEC(bool enabled) override;
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * Factory para criar códecs de áudio
 */
class AudioCodecFactory {
public:
    /**
     * Cria uma instância de codec baseado no tipo
     * @param type Tipo de codec
     * @return Ponteiro para o codec
     */
    static std::unique_ptr<IAudioCodec> Create(CodecType type);
    
    /**
     * Obtém lista de codecs disponíveis
     * @return Lista de tipos de codec
     */
    static std::vector<CodecType> GetAvailableCodecs();
    
    /**
     * Obtém nome legível de um tipo de codec
     * @param type Tipo de codec
     * @return Nome do codec
     */
    static std::string GetCodecName(CodecType type);
    
    /**
     * Obtém o bitrate recomendado para um codec
     * @param type Tipo de codec
     * @param sampleRate Taxa de amostragem
     * @param channels Número de canais
     * @return Bitrate recomendado em bits por segundo
     */
    static int GetRecommendedBitrate(CodecType type, int sampleRate, int channels);
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_CODECS_H
