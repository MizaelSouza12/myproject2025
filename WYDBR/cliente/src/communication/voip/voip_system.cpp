#include "voip_system.h"
#include "voip_system_impl.h"

namespace wydbr {
namespace communication {

// Implementação do singleton
VoIPSystem& VoIPSystem::getInstance() {
    static VoIPSystem instance;
    return instance;
}

// Construtor
VoIPSystem::VoIPSystem() : pImpl(std::make_unique<Impl>()) {
    // Construtor privado - usar getInstance()
}

// Destrutor
VoIPSystem::~VoIPSystem() {
    // Destrutor definido aqui para permitir tipo incompleto no header
}

// Inicialização
bool VoIPSystem::initialize(const VoIPConfig& config) {
    return pImpl->initialize(config);
}

// Shutdown
void VoIPSystem::shutdown() {
    pImpl->shutdown();
}

// Conexão
bool VoIPSystem::connect(const std::string& serverAddress, int port, 
                         const std::string& username, const std::string& authToken) {
    return pImpl->connect(serverAddress, port, username, authToken);
}

// Desconexão
void VoIPSystem::disconnect() {
    pImpl->disconnect();
}

// Verificar conexão
bool VoIPSystem::isConnected() const {
    return pImpl->isConnected();
}

// Gerenciamento de canais
bool VoIPSystem::joinChannel(const std::string& channelId) {
    return pImpl->joinChannel(channelId);
}

bool VoIPSystem::leaveChannel(const std::string& channelId) {
    return pImpl->leaveChannel(channelId);
}

std::vector<std::string> VoIPSystem::getAvailableChannels() const {
    return pImpl->getAvailableChannels();
}

std::string VoIPSystem::getCurrentChannel() const {
    return pImpl->getCurrentChannel();
}

std::vector<VoIPUser> VoIPSystem::getUsersInCurrentChannel() const {
    return pImpl->getUsersInCurrentChannel();
}

// Gerenciamento de microfone
bool VoIPSystem::enableMicrophone() {
    return pImpl->enableMicrophone();
}

void VoIPSystem::disableMicrophone() {
    pImpl->disableMicrophone();
}

bool VoIPSystem::isMicrophoneEnabled() const {
    return pImpl->isMicrophoneEnabled();
}

MicrophoneStatus VoIPSystem::getMicrophoneStatus() const {
    return pImpl->getMicrophoneStatus();
}

void VoIPSystem::setMicActivationMode(MicActivationMode mode) {
    pImpl->setMicActivationMode(mode);
}

MicActivationMode VoIPSystem::getMicActivationMode() const {
    return pImpl->getMicActivationMode();
}

// Controle de volume
void VoIPSystem::setMasterVolume(float volume) {
    pImpl->setMasterVolume(volume);
}

float VoIPSystem::getMasterVolume() const {
    return pImpl->getMasterVolume();
}

void VoIPSystem::setMicrophoneGain(float gain) {
    pImpl->setMicrophoneGain(gain);
}

float VoIPSystem::getMicrophoneGain() const {
    return pImpl->getMicrophoneGain();
}

// Gerenciamento de usuários
bool VoIPSystem::muteUser(const std::string& userId, bool muted) {
    return pImpl->muteUser(userId, muted);
}

void VoIPSystem::muteAll(bool muted) {
    pImpl->muteAll(muted);
}

bool VoIPSystem::isUserMuted(const std::string& userId) const {
    return pImpl->isUserMuted(userId);
}

bool VoIPSystem::setUserVolume(const std::string& userId, float volume) {
    return pImpl->setUserVolume(userId, volume);
}

float VoIPSystem::getUserVolume(const std::string& userId) const {
    return pImpl->getUserVolume(userId);
}

// Áudio espacial
void VoIPSystem::enableSpatialAudio(bool enabled) {
    pImpl->enableSpatialAudio(enabled);
}

bool VoIPSystem::isSpatialAudioEnabled() const {
    return pImpl->isSpatialAudioEnabled();
}

void VoIPSystem::updateListenerPosition(float posX, float posY, float posZ,
                                       float dirX, float dirY, float dirZ) {
    pImpl->updateListenerPosition(posX, posY, posZ, dirX, dirY, dirZ);
}

bool VoIPSystem::updateUserPosition(const std::string& userId,
                                   float posX, float posY, float posZ,
                                   float dirX, float dirY, float dirZ) {
    return pImpl->updateUserPosition(userId, posX, posY, posZ, dirX, dirY, dirZ);
}

void VoIPSystem::setSpatialMaxDistance(float distance) {
    pImpl->setSpatialMaxDistance(distance);
}

float VoIPSystem::getSpatialMaxDistance() const {
    return pImpl->getSpatialMaxDistance();
}

// Qualidade de áudio
void VoIPSystem::setAudioQuality(AudioQuality quality) {
    pImpl->setAudioQuality(quality);
}

AudioQuality VoIPSystem::getAudioQuality() const {
    return pImpl->getAudioQuality();
}

// Callbacks
int VoIPSystem::registerSpeakingStateCallback(SpeakingStateCallback callback) {
    return pImpl->registerSpeakingStateCallback(callback);
}

bool VoIPSystem::unregisterSpeakingStateCallback(int callbackId) {
    return pImpl->unregisterSpeakingStateCallback(callbackId);
}

int VoIPSystem::registerVolumeChangeCallback(VolumeChangeCallback callback) {
    return pImpl->registerVolumeChangeCallback(callback);
}

bool VoIPSystem::unregisterVolumeChangeCallback(int callbackId) {
    return pImpl->unregisterVolumeChangeCallback(callbackId);
}

int VoIPSystem::registerErrorCallback(ErrorCallback callback) {
    return pImpl->registerErrorCallback(callback);
}

bool VoIPSystem::unregisterErrorCallback(int callbackId) {
    return pImpl->unregisterErrorCallback(callbackId);
}

int VoIPSystem::registerConnectivityCallback(ConnectivityCallback callback) {
    return pImpl->registerConnectivityCallback(callback);
}

bool VoIPSystem::unregisterConnectivityCallback(int callbackId) {
    return pImpl->unregisterConnectivityCallback(callbackId);
}

// Estatísticas de rede
int VoIPSystem::getVoiceLatency() const {
    return pImpl->getVoiceLatency();
}

float VoIPSystem::getPacketLossRate() const {
    return pImpl->getPacketLossRate();
}

int VoIPSystem::getBitrate() const {
    return pImpl->getBitrate();
}

} // namespace communication
} // namespace wydbr
