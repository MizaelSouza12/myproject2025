#ifndef WYDBR_SPATIAL_AUDIO_PROCESSOR_H
#define WYDBR_SPATIAL_AUDIO_PROCESSOR_H

#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <memory>

namespace wydbr {
namespace communication {

/**
 * @class SpatialAudioProcessor
 * @brief Processa áudio para criar efeitos espaciais 3D
 * 
 * Esta classe implementa o processamento de áudio para simular a percepção
 * espacial de fontes sonoras, incluindo atenuação com base na distância,
 * filtragem direcional (HRTF) e efeitos Doppler.
 */
class SpatialAudioProcessor {
public:
    SpatialAudioProcessor();
    ~SpatialAudioProcessor();
    
    /**
     * @brief Inicializa o processador de áudio espacial
     * @param sampleRate Taxa de amostragem do áudio
     * @param channels Número de canais de saída (1=mono, 2=stereo)
     * @param hrtfQuality Qualidade do HRTF (0-100)
     * @return true se inicializado com sucesso
     */
    bool initialize(int sampleRate, int channels, int hrtfQuality = 70);
    
    /**
     * @brief Desliga o processador e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Define a posição e orientação do ouvinte
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param dirX Direção X para onde está olhando
     * @param dirY Direção Y para onde está olhando
     * @param dirZ Direção Z para onde está olhando
     * @param upX Componente X do vetor up
     * @param upY Componente Y do vetor up
     * @param upZ Componente Z do vetor up
     */
    void setListenerPosition(float posX, float posY, float posZ,
                             float dirX, float dirY, float dirZ,
                             float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f);
    
    /**
     * @brief Define a velocidade do ouvinte (para efeito Doppler)
     * @param velX Velocidade X
     * @param velY Velocidade Y
     * @param velZ Velocidade Z
     */
    void setListenerVelocity(float velX, float velY, float velZ);
    
    /**
     * @brief Registra ou atualiza uma fonte sonora
     * @param sourceId ID único da fonte
     * @param posX Posição X
     * @param posY Posição Y
     * @param posZ Posição Z
     * @param dirX Direção X (opcional)
     * @param dirY Direção Y (opcional)
     * @param dirZ Direção Z (opcional)
     * @return true se registrada com sucesso
     */
    bool registerSource(const std::string& sourceId,
                        float posX, float posY, float posZ,
                        float dirX = 0.0f, float dirY = 0.0f, float dirZ = 1.0f);
    
    /**
     * @brief Define a velocidade de uma fonte (para efeito Doppler)
     * @param sourceId ID da fonte
     * @param velX Velocidade X
     * @param velY Velocidade Y
     * @param velZ Velocidade Z
     * @return true se sucesso
     */
    bool setSourceVelocity(const std::string& sourceId,
                           float velX, float velY, float velZ);
    
    /**
     * @brief Define propriedades de uma fonte sonora
     * @param sourceId ID da fonte
     * @param innerRadius Raio interno (volume máximo)
     * @param outerRadius Raio externo (início da atenuação)
     * @param maxRadius Raio máximo (silêncio completo)
     * @param directionalConeInnerAngle Ângulo interno do cone direcional (graus)
     * @param directionalConeOuterAngle Ângulo externo do cone direcional (graus)
     * @param directionalConeOuterGain Ganho fora do cone direcional (0.0-1.0)
     * @return true se sucesso
     */
    bool setSourceProperties(const std::string& sourceId,
                             float innerRadius, float outerRadius, float maxRadius,
                             float directionalConeInnerAngle = 360.0f,
                             float directionalConeOuterAngle = 360.0f,
                             float directionalConeOuterGain = 1.0f);
    
    /**
     * @brief Remove uma fonte sonora
     * @param sourceId ID da fonte
     * @return true se removida com sucesso
     */
    bool removeSource(const std::string& sourceId);
    
    /**
     * @brief Processa áudio mono para gerar áudio espacial
     * @param sourceId ID da fonte
     * @param inputSamples Amostras de entrada (mono)
     * @param outputSamples Amostras de saída (stereo para HRTF)
     * @return true se processado com sucesso
     */
    bool processSamples(const std::string& sourceId,
                        const std::vector<float>& inputSamples,
                        std::vector<float>& outputSamples);
    
    /**
     * @brief Obtém o fator de atenuação para uma fonte
     * @param sourceId ID da fonte
     * @return Fator de atenuação (0.0-1.0) ou -1.0 se fonte não existir
     */
    float getAttenuationFactor(const std::string& sourceId) const;
    
    /**
     * @brief Obtém a distância atual até uma fonte
     * @param sourceId ID da fonte
     * @return Distância em unidades do mundo ou -1.0 se fonte não existir
     */
    float getDistance(const std::string& sourceId) const;
    
    /**
     * @brief Define a distância máxima para processamento de áudio
     * @param maxDistance Distância máxima
     */
    void setMaxDistance(float maxDistance);
    
    /**
     * @brief Obtém a distância máxima atual
     * @return Distância máxima
     */
    float getMaxDistance() const;
    
    /**
     * @brief Define a curva de atenuação
     * @param model Tipo de modelo (0=linear, 1=exponencial, 2=inverso, 3=inverso com limite)
     * @param rolloffFactor Fator de rolloff (afeta a rapidez da atenuação)
     */
    void setAttenuationModel(int model, float rolloffFactor);
    
    /**
     * @brief Ativa ou desativa efeito Doppler
     * @param enabled Se deve ativar (true) ou não (false)
     * @param scaleFactor Fator de escala para o efeito (1.0 = realista)
     */
    void enableDopplerEffect(bool enabled, float scaleFactor = 1.0f);
    
    /**
     * @brief Ativa ou desativa HRTF (Head-Related Transfer Function)
     * @param enabled Se deve ativar (true) ou não (false)
     */
    void enableHRTF(bool enabled);
    
    /**
     * @brief Verifica se HRTF está ativo
     * @return true se ativo
     */
    bool isHRTFEnabled() const;
    
    /**
     * @brief Define o tipo de ambiente acústico para reverberação
     * @param environmentType Tipo de ambiente (0=nenhum, 1=pequena sala, 2=sala média, 3=sala grande, 4=arena, 5=exterior, etc.)
     * @param mixLevel Nível de mistura da reverberação (0.0-1.0)
     */
    void setReverberation(int environmentType, float mixLevel = 0.3f);

private:
    // Estrutura para armazenar dados de fontes sonoras
    struct AudioSource {
        float posX, posY, posZ;
        float dirX, dirY, dirZ;
        float velX, velY, velZ;
        float innerRadius;
        float outerRadius;
        float maxRadius;
        float directionalConeInnerAngle;
        float directionalConeOuterAngle;
        float directionalConeOuterGain;
        float lastDistance;
        float lastAngleHorizontal;
        float lastAngleVertical;
        std::vector<float> lastInputSamples;
        bool active;
    };
    
    // Estrutura para armazenar dados do ouvinte
    struct Listener {
        float posX, posY, posZ;
        float dirX, dirY, dirZ;
        float upX, upY, upZ;
        float velX, velY, velZ;
    };
    
    // Dados de estado
    bool m_initialized;
    Listener m_listener;
    std::map<std::string, AudioSource> m_sources;
    mutable std::mutex m_sourcesMutex;
    
    // Configurações
    int m_sampleRate;
    int m_channels;
    int m_hrtfQuality;
    float m_maxDistance;
    int m_attenuationModel;
    float m_rolloffFactor;
    bool m_dopplerEnabled;
    float m_dopplerFactor;
    bool m_hrtfEnabled;
    int m_reverbType;
    float m_reverbMix;
    
    // Utilitários
    float calculateDirectionalGain(const AudioSource& source, const Listener& listener);
    float calculateDistanceAttenuation(float distance, const AudioSource& source);
    void calculateHorizontalVerticalAngles(const AudioSource& source, 
                                          const Listener& listener,
                                          float& horizontalAngle, 
                                          float& verticalAngle);
    float calculateDopplerShift(const AudioSource& source, const Listener& listener);
    void applyHRTF(const std::vector<float>& mono, std::vector<float>& stereo,
                  float horizontalAngle, float verticalAngle);
    void applyDistanceFilter(std::vector<float>& samples, float distance);
    
    // Implementação específica de HRTF
    std::unique_ptr<class HRTFFilter> m_hrtfFilter;
    
    // Implementação de reverberação
    std::unique_ptr<class ReverbProcessor> m_reverbProcessor;
};

} // namespace communication
} // namespace wydbr

#endif // WYDBR_SPATIAL_AUDIO_PROCESSOR_H
