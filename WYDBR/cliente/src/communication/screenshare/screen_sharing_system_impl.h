#ifndef WYDBR_SCREEN_SHARING_SYSTEM_IMPL_H
#define WYDBR_SCREEN_SHARING_SYSTEM_IMPL_H

#include "screen_sharing_system.h"
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
 * @brief Implementação interna do sistema de compartilhamento de tela
 */
class ScreenSharingSystem::Impl {
public:
    Impl();
    ~Impl();
    
    bool initialize(const ScreenSharingConfig& config);
    void shutdown();
    
    std::string startSharing(const std::string& sessionName, bool isPrivate);
    void stopSharing();
    bool isSharing() const;
    
    bool joinSession(const std::string& sessionId);
    void leaveSession();
    std::vector<SessionInfo> getAvailableSessions();
    
    bool updateConfig(const ScreenSharingConfig& config);
    ScreenSharingConfig getCurrentConfig() const;
    
    bool inviteUser(const std::string& userId);
    bool respondToInvite(const std::string& sessionId, bool accept);
    bool removeViewer(const std::string& userId);
    
    bool grantPermission(const std::string& userId, SharingPermission permission);
    bool revokePermission(const std::string& userId, SharingPermission permission);
    bool hasPermission(const std::string& userId, SharingPermission permission) const;
    
    bool sendChatMessage(const std::string& message);
    bool setCustomCaptureRegion(int x, int y, int width, int height);
    
    void getNetworkStats(int& outBitrateKbps, int& outFps, int& outLatency,
                         float& outPacketLoss, float& outJitter) const;
    
    void enableAudio(bool enabled);
    bool isAudioEnabled() const;
    
    void enableMouseIndicator(bool enabled);
    bool isMouseIndicatorEnabled() const;
    
    void setQuality(StreamQuality quality);
    StreamQuality getQuality() const;
    
    void setFrameRate(FrameRate frameRate);
    FrameRate getFrameRate() const;
    
    void setCodec(VideoCodec codec);
    VideoCodec getCodec() const;
    
    int registerFrameReceivedCallback(FrameReceivedCallback callback);
    bool unregisterFrameReceivedCallback(int callbackId);
    
    int registerPermissionChangeCallback(PermissionChangeCallback callback);
    bool unregisterPermissionChangeCallback(int callbackId);
    
    int registerErrorCallback(ErrorCallback callback);
    bool unregisterErrorCallback(int callbackId);
    
    int registerSessionStateCallback(SessionStateCallback callback);
    bool unregisterSessionStateCallback(int callbackId);
    
    int registerStatisticsCallback(StatisticsCallback callback);
    bool unregisterStatisticsCallback(int callbackId);
    
    bool startRecording(const std::string& filePath);
    void stopRecording();
    bool isRecording() const;
    
    void enableLowLatencyMode(bool enabled);
    bool isLowLatencyModeEnabled() const;
    
    void enableQualityScaling(bool enabled);
    bool isQualityScalingEnabled() const;
    
private:
    // Métodos internos
    void captureThread();
    void encodeThread();
    void networkThread();
    void decodeThread();
    void renderThread();
    
    void startCaptureThread();
    void startEncodeThread();
    void startNetworkThread();
    void startDecodeThread();
    void startRenderThread();
    
    void stopCaptureThread();
    void stopEncodeThread();
    void stopNetworkThread();
    void stopDecodeThread();
    void stopRenderThread();
    
    void captureFrm();
    void encodeFrame(const std::vector<uint8_t>& rawFrameData, int width, int height, int stride);
    void sendEncodedFrame(const std::vector<uint8_t>& encodedData, const FrameInfo& info);
    void receiveFrame();
    void decodeFrame(const std::vector<uint8_t>& encodedData, const FrameInfo& info);
    void notifyFrameReceived(const std::vector<uint8_t>& frameData, const FrameInfo& info);
    
    void handlePermissionChange(const std::string& userId, SharingPermission permission, bool granted);
    void handleError(int errorCode, const std::string& errorMessage);
    void handleSessionState(const std::string& sessionId, bool isActive);
    void updateStatistics();
    void notifyStatistics();
    
    void autoAdjustQuality();
    void detectOptimalSettings();
    int calculateOptimalBitrate();
    int estimateAvailableBandwidth();
    
    // Estrutura interna para controle de permissões
    struct ViewerPermissions {
        std::set<SharingPermission> permissions;
        std::string userId;
        bool isActive;
    };
    
    // Estrutura para controle de sessão
    struct SessionData {
        std::string sessionId;
        std::string hostId;
        std::string name;
        bool isPrivate;
        bool isActive;
        int64_t startTime;
        int64_t lastUpdateTime;
        std::map<std::string, ViewerPermissions> viewerPermissions;
    };
    
    // Estrutura para buffer de frame
    struct FrameBuffer {
        std::vector<uint8_t> data;
        int width;
        int height;
        int stride;
        uint64_t timestamp;
    };
    
    // Configurações e estado
    bool m_initialized;
    ScreenSharingConfig m_config;
    std::atomic<bool> m_isSharing;
    std::atomic<bool> m_isViewing;
    std::atomic<bool> m_isRecording;
    std::string m_currentSessionId;
    std::string m_recordingPath;
    
    // Threads
    std::unique_ptr<std::thread> m_captureThread;
    std::unique_ptr<std::thread> m_encodeThread;
    std::unique_ptr<std::thread> m_networkThread;
    std::unique_ptr<std::thread> m_decodeThread;
    std::unique_ptr<std::thread> m_renderThread;
    std::atomic<bool> m_threadsShouldRun;
    
    // Dados de sessão
    SessionData m_sessionData;
    mutable std::mutex m_sessionMutex;
    
    // Buffers e queues
    std::queue<FrameBuffer> m_captureQueue;
    std::queue<std::pair<std::vector<uint8_t>, FrameInfo>> m_encodeQueue;
    std::queue<std::pair<std::vector<uint8_t>, FrameInfo>> m_decodeQueue;
    std::queue<FrameBuffer> m_renderQueue;
    
    mutable std::mutex m_captureQueueMutex;
    mutable std::mutex m_encodeQueueMutex;
    mutable std::mutex m_decodeQueueMutex;
    mutable std::mutex m_renderQueueMutex;
    
    // Estatísticas de rede
    struct NetworkStats {
        std::atomic<int> bitrateKbps;
        std::atomic<int> fps;
        std::atomic<int> latencyMs;
        std::atomic<float> packetLossPercent;
        std::atomic<float> jitterMs;
    } m_stats;
    
    // Callbacks
    struct Callbacks {
        std::vector<std::pair<int, FrameReceivedCallback>> frameReceived;
        std::vector<std::pair<int, PermissionChangeCallback>> permissionChange;
        std::vector<std::pair<int, ErrorCallback>> error;
        std::vector<std::pair<int, SessionStateCallback>> sessionState;
        std::vector<std::pair<int, StatisticsCallback>> statistics;
        int nextCallbackId;
        mutable std::mutex mutex;
    } m_callbacks;
    
    // Componentes especializados
    std::unique_ptr<class ScreenCapturer> m_capturer;
    std::unique_ptr<class VideoEncoder> m_encoder;
    std::unique_ptr<class VideoDecoder> m_decoder;
    std::unique_ptr<class NetworkProtocol> m_protocol;
    std::unique_ptr<class SecurityManager> m_security;
    std::unique_ptr<class ScreenRenderer> m_renderer;
    std::unique_ptr<class RecordingManager> m_recorder;
};

} // namespace communication
} // namespace wydbr

#endif // WYDBR_SCREEN_SHARING_SYSTEM_IMPL_H
