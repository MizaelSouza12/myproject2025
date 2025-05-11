/**
 * @file WebRTCConnection.cpp
 * @brief Implementação do gerenciamento de conexões WebRTC
 */

#include "WebRTCConnection.h"
#include "../../../include/Common/Config.h"
#include <iostream>

namespace WYDBR {
namespace VoIP {

class PeerConnectionObserver : public webrtc::PeerConnectionObserver {
public:
    PeerConnectionObserver(WebRTCConnection* connection)
        : m_connection(connection) {}

    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override {
        std::string sdp;
        if (candidate->ToString(&sdp)) {
            if (m_connection->m_onIceCandidate) {
                m_connection->m_onIceCandidate(sdp);
            }
        }
    }

    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {
        if (m_connection->m_onIceConnectionStateChange) {
            m_connection->m_onIceConnectionStateChange(new_state);
        }
    }

    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {
        auto audioTracks = stream->GetAudioTracks();
        if (!audioTracks.empty() && m_connection->m_onAudioTrack) {
            m_connection->m_onAudioTrack(audioTracks[0]);
        }
    }

private:
    WebRTCConnection* m_connection;
};

WebRTCConnection::WebRTCConnection()
    : m_onIceCandidate(nullptr)
    , m_onIceConnectionStateChange(nullptr)
    , m_onAudioTrack(nullptr) {
}

WebRTCConnection::~WebRTCConnection() {
    m_peerConnection = nullptr;
    m_peerConnectionFactory = nullptr;
    m_localStream = nullptr;
    m_audioTrack = nullptr;
}

bool WebRTCConnection::Initialize() {
    if (!InitializePeerConnectionFactory()) {
        std::cerr << "Falha ao inicializar PeerConnectionFactory" << std::endl;
        return false;
    }

    if (!InitializePeerConnection()) {
        std::cerr << "Falha ao inicializar PeerConnection" << std::endl;
        return false;
    }

    ConfigureIceServers();
    return true;
}

bool WebRTCConnection::InitializePeerConnectionFactory() {
    rtc::InitializeSSL();
    
    m_peerConnectionFactory = webrtc::CreatePeerConnectionFactory();
    if (!m_peerConnectionFactory) {
        std::cerr << "Falha ao criar PeerConnectionFactory" << std::endl;
        return false;
    }

    return true;
}

bool WebRTCConnection::InitializePeerConnection() {
    webrtc::PeerConnectionInterface::RTCConfiguration config;
    config.servers = m_iceServers;
    config.type = webrtc::PeerConnectionInterface::kRelay;

    webrtc::PeerConnectionInterface::IceServer stunServer;
    stunServer.uri = "stun:stun.l.google.com:19302";
    config.servers.push_back(stunServer);

    auto observer = new PeerConnectionObserver(this);
    m_peerConnection = m_peerConnectionFactory->CreatePeerConnection(
        config, nullptr, nullptr, observer);

    if (!m_peerConnection) {
        std::cerr << "Falha ao criar PeerConnection" << std::endl;
        return false;
    }

    return true;
}

void WebRTCConnection::ConfigureIceServers() {
    // Configurar servidores STUN/TURN
    webrtc::PeerConnectionInterface::IceServer stunServer;
    stunServer.uri = "stun:stun.l.google.com:19302";
    m_iceServers.push_back(stunServer);

    // Adicionar servidores TURN se configurados
    // TODO: Carregar servidores TURN do arquivo de configuração
}

bool WebRTCConnection::CreateOffer() {
    if (!m_peerConnection) return false;

    m_peerConnection->CreateOffer(
        [this](webrtc::SessionDescriptionInterface* desc) {
            m_peerConnection->SetLocalDescription(
                new webrtc::SetSessionDescriptionObserver(),
                desc);
        },
        nullptr);

    return true;
}

bool WebRTCConnection::CreateAnswer() {
    if (!m_peerConnection) return false;

    m_peerConnection->CreateAnswer(
        [this](webrtc::SessionDescriptionInterface* desc) {
            m_peerConnection->SetLocalDescription(
                new webrtc::SetSessionDescriptionObserver(),
                desc);
        },
        nullptr);

    return true;
}

bool WebRTCConnection::SetRemoteDescription(const std::string& sdp) {
    if (!m_peerConnection) return false;

    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface* sessionDescription(
        webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdp, &error));

    if (!sessionDescription) {
        std::cerr << "Falha ao criar descrição de sessão: " << error.description << std::endl;
        return false;
    }

    m_peerConnection->SetRemoteDescription(
        new webrtc::SetSessionDescriptionObserver(),
        sessionDescription);

    return true;
}

bool WebRTCConnection::AddIceCandidate(const std::string& candidate) {
    if (!m_peerConnection) return false;

    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> iceCandidate(
        webrtc::CreateIceCandidate("", 0, candidate, &error));

    if (!iceCandidate) {
        std::cerr << "Falha ao criar ICE candidate: " << error.description << std::endl;
        return false;
    }

    m_peerConnection->AddIceCandidate(iceCandidate.get());
    return true;
}

bool WebRTCConnection::AddAudioTrack() {
    if (!m_peerConnection || !m_peerConnectionFactory) return false;

    if (!CreateAudioTrack()) {
        return false;
    }

    m_localStream = m_peerConnectionFactory->CreateLocalMediaStream("audio_stream");
    m_localStream->AddTrack(m_audioTrack);
    m_peerConnection->AddStream(m_localStream);

    return true;
}

bool WebRTCConnection::CreateAudioTrack() {
    cricket::AudioOptions options;
    options.echo_cancellation = true;
    options.auto_gain_control = true;
    options.noise_suppression = true;

    m_audioTrack = m_peerConnectionFactory->CreateAudioTrack(
        "audio_track",
        m_peerConnectionFactory->CreateAudioSource(options));

    return m_audioTrack != nullptr;
}

bool WebRTCConnection::RemoveAudioTrack() {
    if (!m_peerConnection || !m_localStream) return false;

    m_localStream->RemoveTrack(m_audioTrack);
    m_peerConnection->RemoveStream(m_localStream);
    m_audioTrack = nullptr;
    m_localStream = nullptr;

    return true;
}

bool WebRTCConnection::SetAudioEnabled(bool enabled) {
    if (!m_audioTrack) return false;

    m_audioTrack->set_enabled(enabled);
    return true;
}

bool WebRTCConnection::SetAudioVolume(float volume) {
    if (!m_audioTrack) return false;

    // Implementar controle de volume
    // TODO: Implementar controle de volume usando WebRTC AudioProcessing
    return true;
}

void WebRTCConnection::SetOnIceCandidate(OnIceCandidateCallback callback) {
    m_onIceCandidate = callback;
}

void WebRTCConnection::SetOnIceConnectionStateChange(OnIceConnectionStateChangeCallback callback) {
    m_onIceConnectionStateChange = callback;
}

void WebRTCConnection::SetOnAudioTrack(OnAudioTrackCallback callback) {
    m_onAudioTrack = callback;
}

} // namespace VoIP
} // namespace WYDBR 