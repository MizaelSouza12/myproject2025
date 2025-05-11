/**
 * @file CodecManager.h
 * @brief Gerenciamento de codecs de áudio para VoIP
 */

#ifndef WYDBR_CODEC_MANAGER_H
#define WYDBR_CODEC_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <opus/opus.h>
#include <opus/opus_defines.h>

namespace WYDBR {
namespace VoIP {

class CodecManager {
public:
    CodecManager();
    ~CodecManager();

    bool Initialize();
    bool InitializeOpus(int sampleRate, int channels, int bitrate);
    
    std::vector<uint8_t> EncodeAudio(const std::vector<int16_t>& pcmData);
    std::vector<int16_t> DecodeAudio(const std::vector<uint8_t>& encodedData);
    
    bool SetBitrate(int bitrate);
    bool SetComplexity(int complexity);
    bool SetPacketLossPerc(int percentage);
    bool SetDTX(bool enabled);
    bool SetVBR(bool enabled);
    
    int GetBitrate() const;
    int GetComplexity() const;
    int GetPacketLossPerc() const;
    bool GetDTX() const;
    bool GetVBR() const;

private:
    OpusEncoder* m_encoder;
    OpusDecoder* m_decoder;
    
    int m_sampleRate;
    int m_channels;
    int m_bitrate;
    int m_complexity;
    int m_packetLossPerc;
    bool m_dtx;
    bool m_vbr;
    
    bool m_initialized;
    
    void Cleanup();
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_CODEC_MANAGER_H 