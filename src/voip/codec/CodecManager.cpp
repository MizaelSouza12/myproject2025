/**
 * @file CodecManager.cpp
 * @brief Implementação do gerenciamento de codecs de áudio
 */

#include "CodecManager.h"
#include <iostream>

namespace WYDBR {
namespace VoIP {

CodecManager::CodecManager()
    : m_encoder(nullptr)
    , m_decoder(nullptr)
    , m_sampleRate(48000)
    , m_channels(2)
    , m_bitrate(64000)
    , m_complexity(5)
    , m_packetLossPerc(0)
    , m_dtx(false)
    , m_vbr(true)
    , m_initialized(false) {
}

CodecManager::~CodecManager() {
    Cleanup();
}

bool CodecManager::Initialize() {
    return InitializeOpus(m_sampleRate, m_channels, m_bitrate);
}

bool CodecManager::InitializeOpus(int sampleRate, int channels, int bitrate) {
    if (m_initialized) {
        Cleanup();
    }

    m_sampleRate = sampleRate;
    m_channels = channels;
    m_bitrate = bitrate;

    int error = 0;

    // Criar encoder
    m_encoder = opus_encoder_create(sampleRate, channels, OPUS_APPLICATION_VOIP, &error);
    if (error != OPUS_OK || !m_encoder) {
        std::cerr << "Falha ao criar encoder Opus: " << opus_strerror(error) << std::endl;
        return false;
    }

    // Criar decoder
    m_decoder = opus_decoder_create(sampleRate, channels, &error);
    if (error != OPUS_OK || !m_decoder) {
        std::cerr << "Falha ao criar decoder Opus: " << opus_strerror(error) << std::endl;
        opus_encoder_destroy(m_encoder);
        m_encoder = nullptr;
        return false;
    }

    // Configurar encoder
    opus_encoder_ctl(m_encoder, OPUS_SET_BITRATE(bitrate));
    opus_encoder_ctl(m_encoder, OPUS_SET_COMPLEXITY(m_complexity));
    opus_encoder_ctl(m_encoder, OPUS_SET_PACKET_LOSS_PERC(m_packetLossPerc));
    opus_encoder_ctl(m_encoder, OPUS_SET_DTX(m_dtx ? 1 : 0));
    opus_encoder_ctl(m_encoder, OPUS_SET_VBR(m_vbr ? 1 : 0));

    m_initialized = true;
    return true;
}

std::vector<uint8_t> CodecManager::EncodeAudio(const std::vector<int16_t>& pcmData) {
    if (!m_initialized || !m_encoder) {
        return std::vector<uint8_t>();
    }

    const int frameSize = pcmData.size() / m_channels;
    const int maxPacketSize = 1275; // Tamanho máximo do pacote Opus
    std::vector<uint8_t> encodedData(maxPacketSize);

    int encodedBytes = opus_encode(
        m_encoder,
        pcmData.data(),
        frameSize,
        encodedData.data(),
        maxPacketSize
    );

    if (encodedBytes < 0) {
        std::cerr << "Falha ao codificar áudio: " << opus_strerror(encodedBytes) << std::endl;
        return std::vector<uint8_t>();
    }

    encodedData.resize(encodedBytes);
    return encodedData;
}

std::vector<int16_t> CodecManager::DecodeAudio(const std::vector<uint8_t>& encodedData) {
    if (!m_initialized || !m_decoder) {
        return std::vector<int16_t>();
    }

    const int maxFrameSize = 5760; // Tamanho máximo do frame Opus
    std::vector<int16_t> decodedData(maxFrameSize * m_channels);

    int decodedSamples = opus_decode(
        m_decoder,
        encodedData.data(),
        encodedData.size(),
        decodedData.data(),
        maxFrameSize,
        0 // Sem FEC
    );

    if (decodedSamples < 0) {
        std::cerr << "Falha ao decodificar áudio: " << opus_strerror(decodedSamples) << std::endl;
        return std::vector<int16_t>();
    }

    decodedData.resize(decodedSamples * m_channels);
    return decodedData;
}

bool CodecManager::SetBitrate(int bitrate) {
    if (!m_initialized || !m_encoder) return false;

    int error = opus_encoder_ctl(m_encoder, OPUS_SET_BITRATE(bitrate));
    if (error != OPUS_OK) {
        std::cerr << "Falha ao definir bitrate: " << opus_strerror(error) << std::endl;
        return false;
    }

    m_bitrate = bitrate;
    return true;
}

bool CodecManager::SetComplexity(int complexity) {
    if (!m_initialized || !m_encoder) return false;

    int error = opus_encoder_ctl(m_encoder, OPUS_SET_COMPLEXITY(complexity));
    if (error != OPUS_OK) {
        std::cerr << "Falha ao definir complexidade: " << opus_strerror(error) << std::endl;
        return false;
    }

    m_complexity = complexity;
    return true;
}

bool CodecManager::SetPacketLossPerc(int percentage) {
    if (!m_initialized || !m_encoder) return false;

    int error = opus_encoder_ctl(m_encoder, OPUS_SET_PACKET_LOSS_PERC(percentage));
    if (error != OPUS_OK) {
        std::cerr << "Falha ao definir porcentagem de perda de pacotes: " << opus_strerror(error) << std::endl;
        return false;
    }

    m_packetLossPerc = percentage;
    return true;
}

bool CodecManager::SetDTX(bool enabled) {
    if (!m_initialized || !m_encoder) return false;

    int error = opus_encoder_ctl(m_encoder, OPUS_SET_DTX(enabled ? 1 : 0));
    if (error != OPUS_OK) {
        std::cerr << "Falha ao definir DTX: " << opus_strerror(error) << std::endl;
        return false;
    }

    m_dtx = enabled;
    return true;
}

bool CodecManager::SetVBR(bool enabled) {
    if (!m_initialized || !m_encoder) return false;

    int error = opus_encoder_ctl(m_encoder, OPUS_SET_VBR(enabled ? 1 : 0));
    if (error != OPUS_OK) {
        std::cerr << "Falha ao definir VBR: " << opus_strerror(error) << std::endl;
        return false;
    }

    m_vbr = enabled;
    return true;
}

int CodecManager::GetBitrate() const {
    return m_bitrate;
}

int CodecManager::GetComplexity() const {
    return m_complexity;
}

int CodecManager::GetPacketLossPerc() const {
    return m_packetLossPerc;
}

bool CodecManager::GetDTX() const {
    return m_dtx;
}

bool CodecManager::GetVBR() const {
    return m_vbr;
}

void CodecManager::Cleanup() {
    if (m_encoder) {
        opus_encoder_destroy(m_encoder);
        m_encoder = nullptr;
    }

    if (m_decoder) {
        opus_decoder_destroy(m_decoder);
        m_decoder = nullptr;
    }

    m_initialized = false;
}

} // namespace VoIP
} // namespace WYDBR 