/**
 * @file VoIPAudioCapture.h
 * @brief Sistema de captura de áudio para VoIP
 */

#ifndef WYDBR_VOIP_AUDIO_CAPTURE_H
#define WYDBR_VOIP_AUDIO_CAPTURE_H

#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace WYDBR {
namespace VoIP {

/**
 * Configuração para dispositivo de captura de áudio
 */
struct AudioCaptureConfig {
    int sampleRate;             // Taxa de amostragem (8000, 16000, 44100, 48000)
    int channels;               // Número de canais (1 = mono, 2 = estéreo)
    int bitsPerSample;          // Bits por amostra (16, 24, 32)
    int bufferSize;             // Tamanho do buffer em amostras
    int deviceId;               // ID do dispositivo de captura
    bool automaticGainControl;  // Controle automático de ganho
    bool noiseSuppression;      // Supressão de ruído
    bool echoCancellation;      // Cancelamento de eco
};

/**
 * Callback para receber dados de áudio capturados
 */
using AudioCaptureCallback = std::function<void(const int16_t* audioData, size_t frameCount, int channels)>;

/**
 * Classe base para captura de áudio
 */
class IAudioCapture {
public:
    virtual ~IAudioCapture() = default;
    
    /**
     * Inicializa o sistema de captura
     * @param config Configuração para captura
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const AudioCaptureConfig& config) = 0;
    
    /**
     * Inicia a captura de áudio
     * @param callback Função a ser chamada com dados de áudio
     * @return true se iniciado com sucesso
     */
    virtual bool Start(AudioCaptureCallback callback) = 0;
    
    /**
     * Para a captura de áudio
     */
    virtual void Stop() = 0;
    
    /**
     * Verifica se está capturando
     * @return true se está capturando
     */
    virtual bool IsCapturing() const = 0;
    
    /**
     * Obtém lista de dispositivos de captura disponíveis
     * @return Lista de nomes de dispositivos
     */
    virtual std::vector<std::string> GetAvailableDevices() = 0;
    
    /**
     * Define o dispositivo de captura
     * @param deviceId ID do dispositivo
     * @return true se alterado com sucesso
     */
    virtual bool SetDevice(int deviceId) = 0;
    
    /**
     * Ajusta o nível de volume de captura
     * @param volume Volume de 0.0 a 1.0
     */
    virtual void SetVolume(float volume) = 0;
    
    /**
     * Obtém o nível de volume atual
     * @return Volume de 0.0 a 1.0
     */
    virtual float GetVolume() const = 0;
    
    /**
     * Ativa/desativa o mudo
     * @param muted Estado de mudo
     */
    virtual void SetMuted(bool muted) = 0;
    
    /**
     * Verifica se está mudo
     * @return true se estiver mudo
     */
    virtual bool IsMuted() const = 0;
};

/**
 * Implementação de captura de áudio para Windows
 */
class WindowsAudioCapture : public IAudioCapture {
public:
    WindowsAudioCapture();
    ~WindowsAudioCapture() override;
    
    bool Initialize(const AudioCaptureConfig& config) override;
    bool Start(AudioCaptureCallback callback) override;
    void Stop() override;
    bool IsCapturing() const override;
    std::vector<std::string> GetAvailableDevices() override;
    bool SetDevice(int deviceId) override;
    void SetVolume(float volume) override;
    float GetVolume() const override;
    void SetMuted(bool muted) override;
    bool IsMuted() const override;
    
private:
    // Implementação usando Windows Core Audio API
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * Factory para criar sistema de captura de áudio apropriado
 */
class AudioCaptureFactory {
public:
    /**
     * Cria instância de captura de áudio adequada para a plataforma
     * @return Ponteiro para sistema de captura
     */
    static std::unique_ptr<IAudioCapture> Create();
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_AUDIO_CAPTURE_H
