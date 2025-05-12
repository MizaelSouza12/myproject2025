#include "screen_sharing_system.h"
#include "screen_sharing_system_impl.h"
#include <iostream>

namespace wydbr {
namespace communication {

// Implementação do singleton
ScreenSharingSystem& ScreenSharingSystem::getInstance() {
    static ScreenSharingSystem instance;
    return instance;
}

// Construtor
ScreenSharingSystem::ScreenSharingSystem() : pImpl(std::make_unique<Impl>()) {
    // Construtor privado - usar getInstance()
}

// Destrutor
ScreenSharingSystem::~ScreenSharingSystem() {
    // Destrutor definido aqui para permitir tipo incompleto no header
}

// Inicialização
bool ScreenSharingSystem::initialize(const ScreenSharingConfig& config) {
    return pImpl->initialize(config);
}

// Shutdown
void ScreenSharingSystem::shutdown() {
    pImpl->shutdown();
}

// Compartilhamento
std::string ScreenSharingSystem::startSharing(const std::string& sessionName, bool isPrivate) {
    return pImpl->startSharing(sessionName, isPrivate);
}

void ScreenSharingSystem::stopSharing() {
    pImpl->stopSharing();
}

bool ScreenSharingSystem::isSharing() const {
    return pImpl->isSharing();
}

// Sessões
bool ScreenSharingSystem::joinSession(const std::string& sessionId) {
    return pImpl->joinSession(sessionId);
}

void ScreenSharingSystem::leaveSession() {
    pImpl->leaveSession();
}

std::vector<SessionInfo> ScreenSharingSystem::getAvailableSessions() {
    return pImpl->getAvailableSessions();
}

// Configuração
bool ScreenSharingSystem::updateConfig(const ScreenSharingConfig& config) {
    return pImpl->updateConfig(config);
}

ScreenSharingConfig ScreenSharingSystem::getCurrentConfig() const {
    return pImpl->getCurrentConfig();
}

// Gerenciamento de usuários
bool ScreenSharingSystem::inviteUser(const std::string& userId) {
    return pImpl->inviteUser(userId);
}

bool ScreenSharingSystem::respondToInvite(const std::string& sessionId, bool accept) {
    return pImpl->respondToInvite(sessionId, accept);
}

bool ScreenSharingSystem::removeViewer(const std::string& userId) {
    return pImpl->removeViewer(userId);
}

// Permissões
bool ScreenSharingSystem::grantPermission(const std::string& userId, SharingPermission permission) {
    return pImpl->grantPermission(userId, permission);
}

bool ScreenSharingSystem::revokePermission(const std::string& userId, SharingPermission permission) {
    return pImpl->revokePermission(userId, permission);
}

bool ScreenSharingSystem::hasPermission(const std::string& userId, SharingPermission permission) const {
    return pImpl->hasPermission(userId, permission);
}

// Chat
bool ScreenSharingSystem::sendChatMessage(const std::string& message) {
    return pImpl->sendChatMessage(message);
}

// Região de captura
bool ScreenSharingSystem::setCustomCaptureRegion(int x, int y, int width, int height) {
    return pImpl->setCustomCaptureRegion(x, y, width, height);
}

// Estatísticas
void ScreenSharingSystem::getNetworkStats(int& outBitrateKbps, int& outFps, int& outLatency,
                                         float& outPacketLoss, float& outJitter) const {
    pImpl->getNetworkStats(outBitrateKbps, outFps, outLatency, outPacketLoss, outJitter);
}

// Áudio
void ScreenSharingSystem::enableAudio(bool enabled) {
    pImpl->enableAudio(enabled);
}

bool ScreenSharingSystem::isAudioEnabled() const {
    return pImpl->isAudioEnabled();
}

// Indicador de mouse
void ScreenSharingSystem::enableMouseIndicator(bool enabled) {
    pImpl->enableMouseIndicator(enabled);
}

bool ScreenSharingSystem::isMouseIndicatorEnabled() const {
    return pImpl->isMouseIndicatorEnabled();
}

// Qualidade
void ScreenSharingSystem::setQuality(StreamQuality quality) {
    pImpl->setQuality(quality);
}

StreamQuality ScreenSharingSystem::getQuality() const {
    return pImpl->getQuality();
}

// Taxa de quadros
void ScreenSharingSystem::setFrameRate(FrameRate frameRate) {
    pImpl->setFrameRate(frameRate);
}

FrameRate ScreenSharingSystem::getFrameRate() const {
    return pImpl->getFrameRate();
}

// Codec
void ScreenSharingSystem::setCodec(VideoCodec codec) {
    pImpl->setCodec(codec);
}

VideoCodec ScreenSharingSystem::getCodec() const {
    return pImpl->getCodec();
}

// Callbacks
int ScreenSharingSystem::registerFrameReceivedCallback(FrameReceivedCallback callback) {
    return pImpl->registerFrameReceivedCallback(callback);
}

bool ScreenSharingSystem::unregisterFrameReceivedCallback(int callbackId) {
    return pImpl->unregisterFrameReceivedCallback(callbackId);
}

int ScreenSharingSystem::registerPermissionChangeCallback(PermissionChangeCallback callback) {
    return pImpl->registerPermissionChangeCallback(callback);
}

bool ScreenSharingSystem::unregisterPermissionChangeCallback(int callbackId) {
    return pImpl->unregisterPermissionChangeCallback(callbackId);
}

int ScreenSharingSystem::registerErrorCallback(ErrorCallback callback) {
    return pImpl->registerErrorCallback(callback);
}

bool ScreenSharingSystem::unregisterErrorCallback(int callbackId) {
    return pImpl->unregisterErrorCallback(callbackId);
}

int ScreenSharingSystem::registerSessionStateCallback(SessionStateCallback callback) {
    return pImpl->registerSessionStateCallback(callback);
}

bool ScreenSharingSystem::unregisterSessionStateCallback(int callbackId) {
    return pImpl->unregisterSessionStateCallback(callbackId);
}

int ScreenSharingSystem::registerStatisticsCallback(StatisticsCallback callback) {
    return pImpl->registerStatisticsCallback(callback);
}

bool ScreenSharingSystem::unregisterStatisticsCallback(int callbackId) {
    return pImpl->unregisterStatisticsCallback(callbackId);
}

// Gravação
bool ScreenSharingSystem::startRecording(const std::string& filePath) {
    return pImpl->startRecording(filePath);
}

void ScreenSharingSystem::stopRecording() {
    pImpl->stopRecording();
}

bool ScreenSharingSystem::isRecording() const {
    return pImpl->isRecording();
}

// Modo de baixa latência
void ScreenSharingSystem::enableLowLatencyMode(bool enabled) {
    pImpl->enableLowLatencyMode(enabled);
}

bool ScreenSharingSystem::isLowLatencyModeEnabled() const {
    return pImpl->isLowLatencyModeEnabled();
}

// Escalonamento de qualidade
void ScreenSharingSystem::enableQualityScaling(bool enabled) {
    pImpl->enableQualityScaling(enabled);
}

bool ScreenSharingSystem::isQualityScalingEnabled() const {
    return pImpl->isQualityScalingEnabled();
}

} // namespace communication
} // namespace wydbr
