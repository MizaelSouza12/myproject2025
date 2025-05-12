#include "spatial_audio_processor.h"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace wydbr {
namespace communication {

SpatialAudioProcessor::SpatialAudioProcessor() :
    m_initialized(false),
    m_maxDistance(100.0f),
    m_attenuationModel(2),
    m_rolloffFactor(1.0f),
    m_dopplerEnabled(true),
    m_dopplerFactor(1.0f),
    m_hrtfEnabled(true),
    m_reverbType(0),
    m_reverbMix(0.3f)
{
    // Inicializar posição padrão do ouvinte
    m_listener.posX = 0.0f;
    m_listener.posY = 0.0f;
    m_listener.posZ = 0.0f;
    m_listener.dirX = 0.0f;
    m_listener.dirY = 0.0f;
    m_listener.dirZ = 1.0f;
    m_listener.upX = 0.0f;
    m_listener.upY = 1.0f;
    m_listener.upZ = 0.0f;
    m_listener.velX = 0.0f;
    m_listener.velY = 0.0f;
    m_listener.velZ = 0.0f;
}

SpatialAudioProcessor::~SpatialAudioProcessor() {
    shutdown();
}

bool SpatialAudioProcessor::initialize(int sampleRate, int channels, int hrtfQuality) {
    if (m_initialized) {
        std::cerr << "SpatialAudioProcessor já inicializado" << std::endl;
        return false;
    }
    
    m_sampleRate = sampleRate;
    m_channels = channels;
    m_hrtfQuality = hrtfQuality;
    
    // Inicializar filtro HRTF
    // TODO: Implementar quando tivermos a classe HRTFFilter
    //m_hrtfFilter = std::make_unique<HRTFFilter>();
    //m_hrtfFilter->initialize(sampleRate, hrtfQuality);
    
    // Inicializar processador de reverberação
    // TODO: Implementar quando tivermos a classe ReverbProcessor
    //m_reverbProcessor = std::make_unique<ReverbProcessor>();
    //m_reverbProcessor->initialize(sampleRate, channels);
    
    m_initialized = true;
    return true;
}

void SpatialAudioProcessor::shutdown() {
    if (!m_initialized) return;
    
    // Liberar recursos
    m_hrtfFilter.reset();
    m_reverbProcessor.reset();
    
    // Limpar fontes
    {
        std::lock_guard<std::mutex> lock(m_sourcesMutex);
        m_sources.clear();
    }
    
    m_initialized = false;
}

void SpatialAudioProcessor::setListenerPosition(float posX, float posY, float posZ,
                                               float dirX, float dirY, float dirZ,
                                               float upX, float upY, float upZ) {
    m_listener.posX = posX;
    m_listener.posY = posY;
    m_listener.posZ = posZ;
    m_listener.dirX = dirX;
    m_listener.dirY = dirY;
    m_listener.dirZ = dirZ;
    m_listener.upX = upX;
    m_listener.upY = upY;
    m_listener.upZ = upZ;
}

void SpatialAudioProcessor::setListenerVelocity(float velX, float velY, float velZ) {
    m_listener.velX = velX;
    m_listener.velY = velY;
    m_listener.velZ = velZ;
}

bool SpatialAudioProcessor::registerSource(const std::string& sourceId,
                                          float posX, float posY, float posZ,
                                          float dirX, float dirY, float dirZ) {
    // Verificar inicialização
    if (!m_initialized) {
        std::cerr << "SpatialAudioProcessor não inicializado" << std::endl;
        return false;
    }
    
    // Configurar fonte
    AudioSource source;
    source.posX = posX;
    source.posY = posY;
    source.posZ = posZ;
    source.dirX = dirX;
    source.dirY = dirY;
    source.dirZ = dirZ;
    source.velX = 0.0f;
    source.velY = 0.0f;
    source.velZ = 0.0f;
    source.innerRadius = 1.0f;
    source.outerRadius = 10.0f;
    source.maxRadius = 50.0f;
    source.directionalConeInnerAngle = 360.0f;
    source.directionalConeOuterAngle = 360.0f;
    source.directionalConeOuterGain = 1.0f;
    source.lastDistance = 0.0f;
    source.lastAngleHorizontal = 0.0f;
    source.lastAngleVertical = 0.0f;
    source.active = true;
    
    // Registrar fonte
    {
        std::lock_guard<std::mutex> lock(m_sourcesMutex);
        m_sources[sourceId] = source;
    }
    
    return true;
}

bool SpatialAudioProcessor::setSourceVelocity(const std::string& sourceId,
                                            float velX, float velY, float velZ) {
    // Verificar inicialização
    if (!m_initialized) {
        std::cerr << "SpatialAudioProcessor não inicializado" << std::endl;
        return false;
    }
    
    // Atualizar velocidade da fonte
    {
        std::lock_guard<std::mutex> lock(m_sourcesMutex);
        
        auto it = m_sources.find(sourceId);
        if (it == m_sources.end()) {
            std::cerr << "Fonte não encontrada: " << sourceId << std::endl;
            return false;
        }
        
        it->second.velX = velX;
        it->second.velY = velY;
        it->second.velZ = velZ;
    }
    
    return true;
}

bool SpatialAudioProcessor::setSourceProperties(const std::string& sourceId,
                                              float innerRadius, float outerRadius, float maxRadius,
                                              float directionalConeInnerAngle,
                                              float directionalConeOuterAngle,
                                              float directionalConeOuterGain) {
    // Verificar inicialização
    if (!m_initialized) {
        std::cerr << "SpatialAudioProcessor não inicializado" << std::endl;
        return false;
    }
    
    // Atualizar propriedades da fonte
    {
        std::lock_guard<std::mutex> lock(m_sourcesMutex);
        
        auto it = m_sources.find(sourceId);
        if (it == m_sources.end()) {
            std::cerr << "Fonte não encontrada: " << sourceId << std::endl;
            return false;
        }
        
        it->second.innerRadius = innerRadius;
        it->second.outerRadius = outerRadius;
        it->second.maxRadius = maxRadius;
        it->second.directionalConeInnerAngle = directionalConeInnerAngle;
        it->second.directionalConeOuterAngle = directionalConeOuterAngle;
        it->second.directionalConeOuterGain = directionalConeOuterGain;
    }
    
    return true;
}

bool SpatialAudioProcessor::removeSource(const std::string& sourceId) {
    // Verificar inicialização
    if (!m_initialized) {
        std::cerr << "SpatialAudioProcessor não inicializado" << std::endl;
        return false;
    }
    
    // Remover fonte
    {
        std::lock_guard<std::mutex> lock(m_sourcesMutex);
        
        auto it = m_sources.find(sourceId);
        if (it == m_sources.end()) {
            std::cerr << "Fonte não encontrada: " << sourceId << std::endl;
            return false;
        }
        
        m_sources.erase(it);
    }
    
    return true;
}

bool SpatialAudioProcessor::processSamples(const std::string& sourceId,
                                          const std::vector<float>& inputSamples,
                                          std::vector<float>& outputSamples) {
    // Verificar inicialização
    if (!m_initialized) {
        std::cerr << "SpatialAudioProcessor não inicializado" << std::endl;
        return false;
    }
    
    // Verificar se a fonte existe
    AudioSource source;
    bool sourceFound = false;
    
    {
        std::lock_guard<std::mutex> lock(m_sourcesMutex);
        
        auto it = m_sources.find(sourceId);
        if (it == m_sources.end()) {
            std::cerr << "Fonte não encontrada: " << sourceId << std::endl;
            return false;
        }
        
        source = it->second;
        sourceFound = true;
    }
    
    if (!sourceFound) {
        return false;
    }
    
    // Calcular distância e ângulos
    float distance = std::sqrt(
        std::pow(source.posX - m_listener.posX, 2) +
        std::pow(source.posY - m_listener.posY, 2) +
        std::pow(source.posZ - m_listener.posZ, 2)
    );
    
    float horizontalAngle, verticalAngle;
    calculateHorizontalVerticalAngles(source, m_listener, horizontalAngle, verticalAngle);
    
    // Verificar se está além da distância máxima
    if (distance > m_maxDistance) {
        // Apenas silêncio
        outputSamples.resize(inputSamples.size() * m_channels);
        std::fill(outputSamples.begin(), outputSamples.end(), 0.0f);
        return true;
    }
    
    // Calcular atenuação por distância
    float attenuation = calculateDistanceAttenuation(distance, source);
    
    // Calcular ganho direcional
    float directionalGain = calculateDirectionalGain(source, m_listener);
    
    // Calcular deslocamento Doppler se habilitado
    float dopplerShift = 1.0f;
    if (m_dopplerEnabled) {
        dopplerShift = calculateDopplerShift(source, m_listener);
    }
    
    // Aplicar ganho total
    float totalGain = attenuation * directionalGain;
    
    // Processar a amostra mono para stereo (ou multicanal)
    if (m_hrtfEnabled && m_channels >= 2) {
        // Aplicar HRTF para simular percepção 3D
        // TODO: Implementar quando tivermos a classe HRTFFilter
        //applyHRTF(inputSamples, outputSamples, horizontalAngle, verticalAngle);
        
        // Por enquanto, apenas uma simulação simples de stereo
        outputSamples.resize(inputSamples.size() * 2); // Stereo
        
        float leftGain = 0.5f + (horizontalAngle / 180.0f) * 0.5f;
        float rightGain = 0.5f - (horizontalAngle / 180.0f) * 0.5f;
        
        for (size_t i = 0; i < inputSamples.size(); i++) {
            float sample = inputSamples[i] * totalGain;
            outputSamples[i * 2] = sample * leftGain;
            outputSamples[i * 2 + 1] = sample * rightGain;
        }
    } else {
        // Sem HRTF, apenas aplicar ganho
        outputSamples.resize(inputSamples.size() * m_channels);
        
        for (size_t i = 0; i < inputSamples.size(); i++) {
            float sample = inputSamples[i] * totalGain;
            
            for (int c = 0; c < m_channels; c++) {
                outputSamples[i * m_channels + c] = sample;
            }
        }
    }
    
    // Aplicar reverberação se necessário
    if (m_reverbType > 0 && m_reverbMix > 0.0f) {
        // TODO: Implementar quando tivermos a classe ReverbProcessor
        //m_reverbProcessor->process(outputSamples, distance, m_reverbMix);
    }
    
    // Atualizar dados da fonte
    {
        std::lock_guard<std::mutex> lock(m_sourcesMutex);
        
        auto it = m_sources.find(sourceId);
        if (it != m_sources.end()) {
            it->second.lastDistance = distance;
            it->second.lastAngleHorizontal = horizontalAngle;
            it->second.lastAngleVertical = verticalAngle;
            it->second.lastInputSamples = inputSamples;
        }
    }
    
    return true;
}

float SpatialAudioProcessor::getAttenuationFactor(const std::string& sourceId) const {
    // Verificar inicialização
    if (!m_initialized) {
        std::cerr << "SpatialAudioProcessor não inicializado" << std::endl;
        return -1.0f;
    }
    
    // Obter atenuação da fonte
    std::lock_guard<std::mutex> lock(m_sourcesMutex);
    
    auto it = m_sources.find(sourceId);
    if (it == m_sources.end()) {
        std::cerr << "Fonte não encontrada: " << sourceId << std::endl;
        return -1.0f;
    }
    
    const auto& source = it->second;
    float distance = std::sqrt(
        std::pow(source.posX - m_listener.posX, 2) +
        std::pow(source.posY - m_listener.posY, 2) +
        std::pow(source.posZ - m_listener.posZ, 2)
    );
    
    return calculateDistanceAttenuation(distance, source);
}

float SpatialAudioProcessor::getDistance(const std::string& sourceId) const {
    // Verificar inicialização
    if (!m_initialized) {
        std::cerr << "SpatialAudioProcessor não inicializado" << std::endl;
        return -1.0f;
    }
    
    // Obter distância da fonte
    std::lock_guard<std::mutex> lock(m_sourcesMutex);
    
    auto it = m_sources.find(sourceId);
    if (it == m_sources.end()) {
        std::cerr << "Fonte não encontrada: " << sourceId << std::endl;
        return -1.0f;
    }
    
    const auto& source = it->second;
    float distance = std::sqrt(
        std::pow(source.posX - m_listener.posX, 2) +
        std::pow(source.posY - m_listener.posY, 2) +
        std::pow(source.posZ - m_listener.posZ, 2)
    );
    
    return distance;
}

void SpatialAudioProcessor::setMaxDistance(float maxDistance) {
    m_maxDistance = maxDistance;
}

float SpatialAudioProcessor::getMaxDistance() const {
    return m_maxDistance;
}

void SpatialAudioProcessor::setAttenuationModel(int model, float rolloffFactor) {
    m_attenuationModel = model;
    m_rolloffFactor = rolloffFactor;
}

void SpatialAudioProcessor::enableDopplerEffect(bool enabled, float scaleFactor) {
    m_dopplerEnabled = enabled;
    m_dopplerFactor = scaleFactor;
}

void SpatialAudioProcessor::enableHRTF(bool enabled) {
    m_hrtfEnabled = enabled;
}

bool SpatialAudioProcessor::isHRTFEnabled() const {
    return m_hrtfEnabled;
}

void SpatialAudioProcessor::setReverberation(int environmentType, float mixLevel) {
    m_reverbType = environmentType;
    m_reverbMix = mixLevel;
    
    // Atualizar processador de reverberação
    // TODO: Implementar quando tivermos a classe ReverbProcessor
    //if (m_reverbProcessor) {
    //    m_reverbProcessor->setEnvironment(environmentType, mixLevel);
    //}
}

// Métodos utilitários

float SpatialAudioProcessor::calculateDirectionalGain(const AudioSource& source, const Listener& listener) {
    // Se não é direcional, ganho direto = 1.0
    if (source.directionalConeInnerAngle >= 360.0f) {
        return 1.0f;
    }
    
    // Calcular ângulo entre direção da fonte e direção para o ouvinte
    float sourceToListenerX = listener.posX - source.posX;
    float sourceToListenerY = listener.posY - source.posY;
    float sourceToListenerZ = listener.posZ - source.posZ;
    
    float dirLength = std::sqrt(source.dirX * source.dirX + 
                               source.dirY * source.dirY + 
                               source.dirZ * source.dirZ);
    
    float sourceToListenerLength = std::sqrt(sourceToListenerX * sourceToListenerX + 
                                            sourceToListenerY * sourceToListenerY + 
                                            sourceToListenerZ * sourceToListenerZ);
    
    if (dirLength <= 0.0f || sourceToListenerLength <= 0.0f) {
        return 1.0f;
    }
    
    // Normalizar
    float normDirX = source.dirX / dirLength;
    float normDirY = source.dirY / dirLength;
    float normDirZ = source.dirZ / dirLength;
    
    float normToListenerX = sourceToListenerX / sourceToListenerLength;
    float normToListenerY = sourceToListenerY / sourceToListenerLength;
    float normToListenerZ = sourceToListenerZ / sourceToListenerLength;
    
    // Produto escalar (dot product)
    float dotProduct = normDirX * normToListenerX + 
                     normDirY * normToListenerY + 
                     normDirZ * normToListenerZ;
    
    // Converter para ângulo em graus
    float angleInRadians = std::acos(std::max(-1.0f, std::min(1.0f, dotProduct)));
    float angleInDegrees = angleInRadians * 180.0f / M_PI;
    
    // Aplicar modelo de cone
    if (angleInDegrees <= source.directionalConeInnerAngle * 0.5f) {
        // Dentro do cone interno, ganho máximo
        return 1.0f;
    } else if (angleInDegrees <= source.directionalConeOuterAngle * 0.5f) {
        // Entre cones interno e externo, interpolação linear
        float t = (angleInDegrees - source.directionalConeInnerAngle * 0.5f) / 
                 (source.directionalConeOuterAngle * 0.5f - source.directionalConeInnerAngle * 0.5f);
        return 1.0f * (1.0f - t) + source.directionalConeOuterGain * t;
    } else {
        // Fora do cone externo, ganho mínimo
        return source.directionalConeOuterGain;
    }
}

float SpatialAudioProcessor::calculateDistanceAttenuation(float distance, const AudioSource& source) {
    // Se dentro do raio interno, ganho máximo
    if (distance <= source.innerRadius) {
        return 1.0f;
    }
    
    // Se além do raio máximo, ganho zero
    if (distance >= source.maxRadius) {
        return 0.0f;
    }
    
    // Normalizar a distância para o intervalo [0,1] do modelo de atenuação
    float normalizedDistance = 0.0f;
    
    if (distance <= source.outerRadius) {
        // Entre raio interno e raio externo
        normalizedDistance = (distance - source.innerRadius) / 
                            (source.outerRadius - source.innerRadius);
    } else {
        // Entre raio externo e raio máximo
        normalizedDistance = 0.5f + 0.5f * (distance - source.outerRadius) / 
                            (source.maxRadius - source.outerRadius);
    }
    
    // Aplicar modelo de atenuação
    float attenuation = 0.0f;
    
    switch (m_attenuationModel) {
        case 0: // Linear
            attenuation = 1.0f - normalizedDistance;
            break;
            
        case 1: // Exponencial
            attenuation = std::pow(1.0f - normalizedDistance, m_rolloffFactor);
            break;
            
        case 2: // Inverso
            attenuation = 1.0f / (1.0f + m_rolloffFactor * normalizedDistance);
            break;
            
        case 3: // Inverso com limite
            attenuation = 1.0f / (1.0f + m_rolloffFactor * normalizedDistance * normalizedDistance);
            break;
            
        default:
            attenuation = 1.0f - normalizedDistance;
            break;
    }
    
    return std::max(0.0f, std::min(1.0f, attenuation));
}

void SpatialAudioProcessor::calculateHorizontalVerticalAngles(
    const AudioSource& source, const Listener& listener,
    float& horizontalAngle, float& verticalAngle)
{
    // Calcular vetor da fonte para o ouvinte
    float sourceToListenerX = listener.posX - source.posX;
    float sourceToListenerY = listener.posY - source.posY;
    float sourceToListenerZ = listener.posZ - source.posZ;
    
    // Base dos vetores do ouvinte
    float forwardX = listener.dirX;
    float forwardY = listener.dirY;
    float forwardZ = listener.dirZ;
    
    float upX = listener.upX;
    float upY = listener.upY;
    float upZ = listener.upZ;
    
    // Calcular vetor direito (cross product de forward e up)
    float rightX = forwardY * upZ - forwardZ * upY;
    float rightY = forwardZ * upX - forwardX * upZ;
    float rightZ = forwardX * upY - forwardY * upX;
    
    // Normalizar
    float forwardLength = std::sqrt(forwardX * forwardX + forwardY * forwardY + forwardZ * forwardZ);
    float rightLength = std::sqrt(rightX * rightX + rightY * rightY + rightZ * rightZ);
    float upLength = std::sqrt(upX * upX + upY * upY + upZ * upZ);
    
    if (forwardLength > 0.0f) {
        forwardX /= forwardLength;
        forwardY /= forwardLength;
        forwardZ /= forwardLength;
    }
    
    if (rightLength > 0.0f) {
        rightX /= rightLength;
        rightY /= rightLength;
        rightZ /= rightLength;
    }
    
    if (upLength > 0.0f) {
        upX /= upLength;
        upY /= upLength;
        upZ /= upLength;
    }
    
    // Calcular projeções
    float sourceToListenerLength = std::sqrt(
        sourceToListenerX * sourceToListenerX +
        sourceToListenerY * sourceToListenerY +
        sourceToListenerZ * sourceToListenerZ
    );
    
    if (sourceToListenerLength > 0.0f) {
        sourceToListenerX /= sourceToListenerLength;
        sourceToListenerY /= sourceToListenerLength;
        sourceToListenerZ /= sourceToListenerLength;
    }
    
    // Projeções na base do ouvinte
    float projForward = sourceToListenerX * forwardX + 
                      sourceToListenerY * forwardY + 
                      sourceToListenerZ * forwardZ;
    
    float projRight = sourceToListenerX * rightX + 
                    sourceToListenerY * rightY + 
                    sourceToListenerZ * rightZ;
    
    float projUp = sourceToListenerX * upX + 
                 sourceToListenerY * upY + 
                 sourceToListenerZ * upZ;
    
    // Calcular ângulos
    // Ângulo horizontal (azimuth) [-180, 180], 0 = frente, positivo = direita
    horizontalAngle = std::atan2(projRight, projForward) * 180.0f / M_PI;
    
    // Ângulo vertical (elevation) [-90, 90], 0 = frente, positivo = acima
    verticalAngle = std::asin(std::max(-1.0f, std::min(1.0f, projUp))) * 180.0f / M_PI;
}

float SpatialAudioProcessor::calculateDopplerShift(const AudioSource& source, const Listener& listener) {
    // Vetor da posição da fonte para o ouvinte
    float sourceToListenerX = listener.posX - source.posX;
    float sourceToListenerY = listener.posY - source.posY;
    float sourceToListenerZ = listener.posZ - source.posZ;
    
    float distance = std::sqrt(
        sourceToListenerX * sourceToListenerX +
        sourceToListenerY * sourceToListenerY +
        sourceToListenerZ * sourceToListenerZ
    );
    
    if (distance < 0.01f) {
        return 1.0f; // Evitar divisão por zero
    }
    
    // Normalizar o vetor direção
    sourceToListenerX /= distance;
    sourceToListenerY /= distance;
    sourceToListenerZ /= distance;
    
    // Calcular a velocidade relativa na direção da linha entre fonte e ouvinte
    float sourceVelocityProj = source.velX * sourceToListenerX +
                              source.velY * sourceToListenerY +
                              source.velZ * sourceToListenerZ;
    
    float listenerVelocityProj = listener.velX * sourceToListenerX +
                                listener.velY * sourceToListenerY +
                                listener.velZ * sourceToListenerZ;
    
    // Velocidade do som (343 m/s no ar à temperatura ambiente)
    const float speedOfSound = 343.0f;
    
    // Aplicar fator de escala Doppler
    float scaledSpeedOfSound = speedOfSound / m_dopplerFactor;
    
    // Calcular deslocamento Doppler 
    // Fórmula: frequência ouvida = frequência emitida * (velocidade do som - velocidade do ouvinte) / (velocidade do som - velocidade da fonte)
    float dopplerShift = (scaledSpeedOfSound - listenerVelocityProj) / 
                        (scaledSpeedOfSound - sourceVelocityProj);
    
    // Limitar o deslocamento para evitar valores irreais
    dopplerShift = std::max(0.5f, std::min(2.0f, dopplerShift));
    
    return dopplerShift;
}

} // namespace communication
} // namespace wydbr
