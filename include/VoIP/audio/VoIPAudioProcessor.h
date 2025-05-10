/**
 * @file VoIPAudioProcessor.h
 * @brief Processamento avançado de áudio para VoIP
 */

#ifndef WYDBR_VOIP_AUDIO_PROCESSOR_H
#define WYDBR_VOIP_AUDIO_PROCESSOR_H

#include <memory>
#include <vector>
#include <cstdint>

namespace WYDBR {
namespace VoIP {

/**
 * Configuração para processamento de áudio
 */
struct AudioProcessingConfig {
    bool enableNoiseSuppression;    // Habilitar supressão de ruído
    int noiseSuppressionLevel;      // Nível de supressão (0-3)
    bool enableEchoCancellation;    // Habilitar cancelamento de eco
    bool enableGainControl;         // Habilitar controle de ganho
    int gainControlLevel;           // Nível de ganho (0-3)
    bool enableVoiceDetection;      // Habilitar detecção de voz
    float voiceDetectionThreshold;  // Limiar para detecção (0.0-1.0)
    bool enableHighPassFilter;      // Remover frequências baixas
    bool enableTransientSuppression; // Suprimir transientes
};

/**
 * Interface para processamento de áudio
 */
class IAudioProcessor {
public:
    virtual ~IAudioProcessor() = default;
    
    /**
     * Inicializa o processador de áudio
     * @param config Configuração de processamento
     * @param sampleRate Taxa de amostragem
     * @param channels Número de canais
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const AudioProcessingConfig& config, int sampleRate, int channels) = 0;
    
    /**
     * Processa um frame de áudio
     * @param input Dados de entrada
     * @param inputSize Tamanho dos dados de entrada
     * @param output Buffer para saída (deve ser pré-alocado)
     * @param outputSize Tamanho do buffer de saída
     * @return Tamanho dos dados processados ou -1 em caso de erro
     */
    virtual int ProcessFrame(const int16_t* input, size_t inputSize, int16_t* output, size_t outputSize) = 0;
    
    /**
     * Verifica se áudio contém voz
     * @param audioData Dados de áudio
     * @param sampleCount Número de amostras
     * @return Probabilidade (0.0-1.0) de conter voz
     */
    virtual float DetectVoiceActivity(const int16_t* audioData, size_t sampleCount) = 0;
    
    /**
     * Atualiza a configuração de processamento
     * @param config Nova configuração
     * @return true se atualizada com sucesso
     */
    virtual bool UpdateConfig(const AudioProcessingConfig& config) = 0;
};

/**
 * Implementação de processador de áudio
 */
class AudioProcessor : public IAudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor() override;
    
    bool Initialize(const AudioProcessingConfig& config, int sampleRate, int channels) override;
    int ProcessFrame(const int16_t* input, size_t inputSize, int16_t* output, size_t outputSize) override;
    float DetectVoiceActivity(const int16_t* audioData, size_t sampleCount) override;
    bool UpdateConfig(const AudioProcessingConfig& config) override;
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_AUDIO_PROCESSOR_H
