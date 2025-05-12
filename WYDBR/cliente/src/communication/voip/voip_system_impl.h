#ifndef WYDBR_VOIP_SYSTEM_IMPL_H
#define WYDBR_VOIP_SYSTEM_IMPL_H

#include "voip_system.h"
#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <memory>
#include <thread>
#include <queue>
#include <functional>

namespace wydbr {
namespace communication {

/**
 * @brief Implementação interna do sistema VoIP
 */
class VoIPSystem::Impl {
public:
    Impl();
    ~Impl();
    
    bool initialize(const VoIPConfig& config);
    void shutdown();
    
    bool connect(const std::string& serverAddress, int port, 
                 const std::string& username, const std::string& authToken);
    void disconnect();
    bool isConnected() const;
    
    bool joinChannel(const std::string& channelId);
    bool leaveChannel(const std::string& channelId);
    std::vector<std::string> getAvailableChannels() const;
    std::string getCurrentChannel() const;
    std::vector<VoIPUser> getUsersInCurrentChannel() const;
    
    bool enableMicrophone();
    void disableMicrophone();
    bool isMicrophoneEnabled() const;
    MicrophoneStatus getMicrophoneStatus() const;
    
    void setMicActivationMode(MicActivationMode mode);
    MicActivationMode getMicActivationMode() const;
    
    void setMasterVolume(float volume);
    float getMasterVolume() const;
    void setMicrophoneGain(float gain);
    float getMicrophoneGain() const;
    
    bool muteUser(const std::string& userId, bool muted);
    void muteAll(bool muted);
    bool isUserMuted(const std::string& userId) const;
    
    bool setUserVolume(const std::string& userId, float volume);
    float getUserVolume(const std::string& userId) const;
    
    void enableSpatialAudio(bool enabled);
    bool isSpatialAudioEnabled() const;
    
    void updateListenerPosition(float posX, float posY, float posZ,
                                float dirX, float dirY, float dirZ);
    bool updateUserPosition(const std::string& userId,
                           float posX, float posY, float posZ,
                           float dirX, float dirY, float dirZ);
    
    void setSpatialMaxDistance(float distance);
    float getSpatialMaxDistance() const;
    
    void setAudioQuality(AudioQuality quality);
    AudioQuality getAudioQuality() const;
    
    int registerSpeakingStateCallback(SpeakingStateCallback callback);
    bool unregisterSpeakingStateCallback(int callbackId);
    int registerVolumeChangeCallback(VolumeChangeCallback callback);
    bool unregisterVolumeChangeCallback(int callbackId);
    int registerErrorCallback(ErrorCallback callback);
    bool unregisterErrorCallback(int callbackId);
    int registerConnectivityCallback(ConnectivityCallback callback);
    bool unregisterConnectivityCallback(int callbackId);
    
    int getVoiceLatency() const;
    float getPacketLossRate() const;
    int getBitrate() const;
    
private:
    // Atualizar estado de comunicação
    void processCommunication();
    void processAudio();
    void processSpatialAudio();
    void detectVoiceActivity();
    void applyAudioEffects();
    void notifySpeakingState(const std::string& userId, bool isSpeaking);
    void handleConnectionStateChange(bool isConnected);
    void handleError(int errorCode, const std::string& errorMessage);
    void captureAudio();
    void playbackAudio();
    void encodeAudio(const std::vector<float>& samples);
    std::vector<float> decodeAudio(const std::vector<uint8_t>& encodedData);
    float calculateVoiceLevel(const std::vector<float>& samples);
    int calculateOptimalBitrate();
    void applyEchoCancellation(std::vector<float>& samples);
    void applyNoiseSuppression(std::vector<float>& samples);
    void applyAGC(std::vector<float>& samples);
    
    // Utilitários internos
    float clamp(float value, float min, float max);
    float dbToLinear(float db);
    float linearToDb(float linear);
    float calculateDistance(float x1, float y1, float z1, float x2, float y2, float z2);
    float calculateAttenuation(float distance);
    
    // Gerenciamento de threads
    void startAudioCaptureThread();
    void stopAudioCaptureThread();
    void startAudioPlaybackThread();
    void stopAudioPlaybackThread();
    void startNetworkThread();
    void stopNetworkThread();
    
    // Configurações
    VoIPConfig m_config;
    
    // Estado do sistema
    bool m_initialized;
    std::atomic<bool> m_connected;
    std::atomic<bool> m_microphoneEnabled;
    MicrophoneStatus m_microphoneStatus;
    std::string m_currentChannel;
    std::atomic<float> m_masterVolume;
    std::atomic<float> m_microphoneGain;
    std::atomic<bool> m_spatialAudioEnabled;
    std::atomic<float> m_spatialMaxDistance;
    std::atomic<AudioQuality> m_audioQuality;
    std::atomic<MicActivationMode> m_micActivationMode;
    
    // Threads
    std::unique_ptr<std::thread> m_audioCaptureThread;
    std::unique_ptr<std::thread> m_audioPlaybackThread;
    std::unique_ptr<std::thread> m_networkThread;
    std::atomic<bool> m_threadsShouldRun;
    
    // Estado do usuário local
    struct ListenerState {
        float posX, posY, posZ;
        float dirX, dirY, dirZ;
        mutable std::mutex mutex;
    } m_listenerState;
    
    // Estado dos usuários remotos
    struct UserState {
        VoIPUser user;
        float volume;
        bool muted;
        std::vector<uint8_t> lastAudioPacket;
        std::chrono::steady_clock::time_point lastPacketTime;
        std::vector<float> lastDecodedSamples;
    };
    std::map<std::string, UserState> m_userStates;
    mutable std::mutex m_userStatesMutex;
    
    // Buffers de áudio
    struct AudioBuffer {
        std::vector<float> samples;
        size_t sampleCount;
        int channels;
        int sampleRate;
    };
    AudioBuffer m_captureBuffer;
    std::map<std::string, AudioBuffer> m_playbackBuffers;
    mutable std::mutex m_audioBufferMutex;
    
    // Filas de pacotes de áudio
    struct AudioPacket {
        std::string userId;
        std::vector<uint8_t> data;
        uint32_t timestamp;
        uint32_t sequenceNumber;
    };
    std::queue<AudioPacket> m_incomingPackets;
    std::queue<AudioPacket> m_outgoingPackets;
    mutable std::mutex m_packetQueueMutex;
    
    // Estatísticas
    struct Stats {
        std::atomic<int> latency;
        std::atomic<float> packetLossRate;
        std::atomic<int> bitrate;
        std::chrono::steady_clock::time_point lastStatsUpdate;
    } m_stats;
    
    // Callbacks
    struct Callbacks {
        std::vector<std::pair<int, SpeakingStateCallback>> speakingState;
        std::vector<std::pair<int, VolumeChangeCallback>> volumeChange;
        std::vector<std::pair<int, ErrorCallback>> error;
        std::vector<std::pair<int, ConnectivityCallback>> connectivity;
        int nextCallbackId;
        mutable std::mutex mutex;
    } m_callbacks;
    
    // Implementações específicas de codec/protocolo
    std::unique_ptr<class OpusCodec> m_opusCodec;
    std::unique_ptr<class VoIPProtocol> m_protocol;
    std::unique_ptr<class SpatialAudioProcessor> m_spatialProcessor;
    std::unique_ptr<class AudioDeviceManager> m_audioDeviceManager;
};

} // namespace communication
} // namespace wydbr

#endif // WYDBR_VOIP_SYSTEM_IMPL_H
