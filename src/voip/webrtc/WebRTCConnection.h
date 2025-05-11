/**
 * @file WebRTCConnection.h
 * @brief Gerenciamento de conexões WebRTC para VoIP
 */

#ifndef WYDBR_WEBRTC_CONNECTION_H
#define WYDBR_WEBRTC_CONNECTION_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/api/mediastreaminterface.h>
#include <webrtc/api/audiotrackinterface.h>
#include <webrtc/base/ssladapter.h>

namespace WYDBR {
namespace VoIP {

class WebRTCConnection {
public:
    WebRTCConnection();
    ~WebRTCConnection();

    bool Initialize();
    bool CreateOffer();
    bool CreateAnswer();
    bool SetRemoteDescription(const std::string& sdp);
    bool AddIceCandidate(const std::string& candidate);
    bool AddAudioTrack();
    bool RemoveAudioTrack();
    bool SetAudioEnabled(bool enabled);
    bool SetAudioVolume(float volume);

    // Callbacks
    using OnIceCandidateCallback = std::function<void(const std::string&)>;
    using OnIceConnectionStateChangeCallback = std::function<void(webrtc::PeerConnectionInterface::IceConnectionState)>;
    using OnAudioTrackCallback = std::function<void(rtc::scoped_refptr<webrtc::AudioTrackInterface>)>;

    void SetOnIceCandidate(OnIceCandidateCallback callback);
    void SetOnIceConnectionStateChange(OnIceConnectionStateChangeCallback callback);
    void SetOnAudioTrack(OnAudioTrackCallback callback);

private:
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_peerConnection;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_peerConnectionFactory;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> m_localStream;
    rtc::scoped_refptr<webrtc::AudioTrackInterface> m_audioTrack;

    // Callbacks
    OnIceCandidateCallback m_onIceCandidate;
    OnIceConnectionStateChangeCallback m_onIceConnectionStateChange;
    OnAudioTrackCallback m_onAudioTrack;

    // Configuração
    webrtc::PeerConnectionInterface::RTCConfiguration m_config;
    std::vector<webrtc::PeerConnectionInterface::IceServer> m_iceServers;

    bool InitializePeerConnectionFactory();
    bool InitializePeerConnection();
    bool CreateAudioTrack();
    void ConfigureIceServers();
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_WEBRTC_CONNECTION_H 