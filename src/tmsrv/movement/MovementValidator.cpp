/**
 * @file MovementValidator.cpp
 * @brief Implementação do sistema de validação de movimento para WYDBR 2.0
 */

#include "movement/MovementValidator.h"
#include "../world/Position.h"
#include "../world/Grid.h"
#include "../entity/Entity.h"
#include "../Security/SecurityManager.h"
#include "../Network/CryptoManager.h"
#include <cmath>
#include <sstream>
#include <algorithm>

namespace WYDBR {
namespace Movement {

// Singleton instance
std::unique_ptr<MovementValidator> MovementValidator::s_instance;
std::once_flag MovementValidator::s_onceFlag;

MovementValidator& MovementValidator::getInstance() {
    std::call_once(s_onceFlag, []() {
        s_instance.reset(new MovementValidator());
    });
    return *s_instance;
}

MovementValidator::MovementValidator() 
    : m_maxMovementDistance(10.0f)
    , m_maxMovesPerSecond(5.0f)
    , m_rateLimitBurstSize(10)
    , m_rateWindowDuration(std::chrono::seconds(5))
    , m_timestampTolerance(std::chrono::milliseconds(5000))
    , m_lastCleanupTime(std::chrono::system_clock::now()) {
    
    // Inicializar referências a outros subsistemas
    m_grid = std::make_shared<Grid>(Grid::getInstance());
    m_securityManager = std::make_shared<Security::SecurityManager>(Security::SecurityManager::getInstance());
    m_cryptoManager = std::make_shared<CryptoManager>(CryptoManager::getInstance());
}

MovementValidator::~MovementValidator() {
    // Limpeza
}

MovementValidationResult MovementValidator::validateMovement(
    const Entity* entity, 
    const Position& currentPos,
    const Position& newPos) {
    
    // Verificar limites do mundo
    if (!isWithinWorldBounds(newPos)) {
        return MovementValidationResult::INVALID_POSITION;
    }
    
    // Verificar se o terreno é caminhável
    if (!isTerrainWalkable(newPos)) {
        return MovementValidationResult::TERRAIN_BLOCKED;
    }
    
    // Verificar colisões com outras entidades
    if (hasEntityCollision(entity, newPos)) {
        return MovementValidationResult::COLLISION;
    }
    
    // Verificar se a entidade tem capacidade para este movimento
    if (entity) {
        // Verificar se o terreno é compatível com as capacidades da entidade
        TerrainType terrain = m_grid->getTerrainType(newPos);
        if (!entity->canTraverse(terrain)) {
            return MovementValidationResult::MOVEMENT_CAPACITY;
        }
        
        // Verificar distância máxima de movimento
        if (!isWithinMovementRange(entity, currentPos, newPos)) {
            return MovementValidationResult::DISTANCE_EXCEEDED;
        }
        
        // Verificar linha de visão para movimento diagonal
        if (isDiagonalMovement(currentPos, newPos) && 
            !hasLineOfSight(currentPos, newPos)) {
            return MovementValidationResult::TERRAIN_BLOCKED;
        }
    }
    
    return MovementValidationResult::VALID;
}

bool MovementValidator::authenticateMovementPacket(
    const MovementAuthenticationData& authData,
    const Position& currentPos,
    const Position& newPos) {
    
    // Verificar timestamp para evitar replay attacks
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::milliseconds(authData.timestamp);
    auto packetTime = std::chrono::system_clock::time_point(timestamp);
    
    if (now - packetTime > m_timestampTolerance || 
        packetTime - now > m_timestampTolerance) {
        return false;  // Timestamp fora da tolerância
    }
    
    // Verificar rate limit
    if (!checkRateLimit(authData.entityId)) {
        return false;  // Rate limit excedido
    }
    
    // Calcular token esperado
    std::string expectedToken = calculateMovementToken(
        authData.entityId,
        authData.sessionId,
        authData.timestamp,
        authData.moveCounter,
        currentPos,
        newPos
    );
    
    // Verificar token com comparação constante-time
    return m_cryptoManager->constantTimeCompare(authData.token, expectedToken);
}

MovementValidationResult MovementValidator::validateAndAuthenticateMovement(
    const Entity* entity,
    const Position& currentPos,
    const Position& newPos,
    const MovementAuthenticationData& authData) {
    
    // Primeiramente validar a autenticação do pacote
    // IMPORTANTE: Esta separação corrige o bug crítico da análise de engenharia reversa
    if (!authenticateMovementPacket(authData, currentPos, newPos)) {
        return MovementValidationResult::AUTHENTICATION_FAILED;
    }
    
    // Se a autenticação passar, então validar o movimento em si
    return validateMovement(entity, currentPos, newPos);
}

void MovementValidator::setRateLimit(float maxMovesPerSecond, int burstSize) {
    std::lock_guard<std::mutex> lock(m_rateLimitMutex);
    
    m_maxMovesPerSecond = maxMovesPerSecond > 0.0f ? maxMovesPerSecond : 1.0f;
    m_rateLimitBurstSize = burstSize > 0 ? burstSize : 1;
}

void MovementValidator::setMaxMovementDistance(float maxDistance) {
    m_maxMovementDistance = maxDistance > 0.0f ? maxDistance : 1.0f;
}

bool MovementValidator::isWithinWorldBounds(const Position& pos) const {
    return m_grid->isWithinBounds(pos);
}

bool MovementValidator::isTerrainWalkable(const Position& pos) const {
    return m_grid->isWalkable(pos);
}

bool MovementValidator::hasLineOfSight(const Position& start, const Position& end) const {
    // Implementar algoritmo de linha de visão (Bresenham ou similar)
    // Para simplicidade, apenas verificamos pontos intermediários para movimentos diagonais
    if (isDiagonalMovement(start, end)) {
        Position midPoint1(end.x, start.y, start.z);
        Position midPoint2(start.x, end.y, start.z);
        
        return isTerrainWalkable(midPoint1) && isTerrainWalkable(midPoint2);
    }
    
    return true;
}

bool MovementValidator::isWithinMovementRange(
    const Entity* entity, 
    const Position& currentPos, 
    const Position& newPos) const {
    
    // Calcular distância euclidiana
    float dx = static_cast<float>(newPos.x - currentPos.x);
    float dy = static_cast<float>(newPos.y - currentPos.y);
    float dz = static_cast<float>(newPos.z - currentPos.z);
    
    float distance = std::sqrt(dx*dx + dy*dy + dz*dz);
    
    // Verificar contra o limite configurado ou específico da entidade
    float maxDistance = m_maxMovementDistance;
    if (entity && entity->getMaxMovementDistance() > 0.0f) {
        maxDistance = entity->getMaxMovementDistance();
    }
    
    return distance <= maxDistance;
}

bool MovementValidator::hasEntityCollision(const Entity* entity, const Position& pos) const {
    // Verificar se há outra entidade na posição
    if (!entity || !m_grid->hasEntityAt(pos)) {
        return false;
    }
    
    // Se a entidade pode atravessar outras, não há colisão
    if (entity->canPassThrough()) {
        return false;
    }
    
    // Verifica se a entidade na posição é a própria entidade atual
    Entity* entityAtPos = m_grid->getEntityAt(pos);
    if (entityAtPos && entityAtPos->getEntityId() == entity->getEntityId()) {
        return false;
    }
    
    return true;
}

bool MovementValidator::checkRateLimit(const std::string& entityId) {
    std::lock_guard<std::mutex> lock(m_rateLimitMutex);
    
    auto now = std::chrono::system_clock::now();
    
    // Executar limpeza periódica de entradas antigas
    if (now - m_lastCleanupTime > std::chrono::minutes(5)) {
        cleanupOldRateLimitEntries();
        m_lastCleanupTime = now;
    }
    
    // Obter ou criar entrada para esta entidade
    auto& entry = m_rateLimitMap[entityId];
    
    // Atualizar contador e histórico de movimentos
    entry.counter++;
    entry.recentMoves.push_back(now);
    
    // Remover movimentos antigos da janela de tempo
    auto windowStart = now - m_rateWindowDuration;
    entry.recentMoves.erase(
        std::remove_if(entry.recentMoves.begin(), entry.recentMoves.end(),
                      [&windowStart](const auto& time) {
                          return time < windowStart;
                      }),
        entry.recentMoves.end()
    );
    
    // Verificar limite de burst
    if (entry.recentMoves.size() > static_cast<size_t>(m_rateLimitBurstSize)) {
        return false;
    }
    
    // Verificar taxa média
    if (entry.recentMoves.size() > 1) {
        float durationSeconds = std::chrono::duration<float>(
            entry.recentMoves.back() - entry.recentMoves.front()).count();
        
        if (durationSeconds > 0) {
            float rate = static_cast<float>(entry.recentMoves.size() - 1) / durationSeconds;
            if (rate > m_maxMovesPerSecond) {
                return false;
            }
        }
    }
    
    // Atualizar tempo do último movimento
    entry.lastMoveTime = now;
    
    return true;
}

void MovementValidator::cleanupOldRateLimitEntries() {
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - m_rateWindowDuration;
    
    for (auto it = m_rateLimitMap.begin(); it != m_rateLimitMap.end();) {
        if (it->second.lastMoveTime < cutoff) {
            it = m_rateLimitMap.erase(it);
        } else {
            ++it;
        }
    }
}

std::string MovementValidator::calculateMovementToken(
    const std::string& entityId,
    const std::string& sessionId,
    uint64_t timestamp,
    uint32_t moveCounter,
    const Position& currentPos,
    const Position& newPos) const {
    
    // Criar string com todos os dados do movimento
    std::stringstream dataStream;
    dataStream << entityId << "|"
               << sessionId << "|"
               << timestamp << "|"
               << moveCounter << "|"
               << currentPos.x << "," << currentPos.y << "," << currentPos.z << "|"
               << newPos.x << "," << newPos.y << "," << newPos.z;
    
    // Gerar token usando HMAC
    std::string movementSecret = m_securityManager->getMovementSecret();
    return m_cryptoManager->generateHMAC(dataStream.str(), movementSecret);
}

bool MovementValidator::isDiagonalMovement(const Position& start, const Position& end) const {
    // Um movimento é diagonal se ambos dx e dy são não-zero
    int dx = end.x - start.x;
    int dy = end.y - start.y;
    
    return dx != 0 && dy != 0;
}

} // namespace Movement
} // namespace WYDBR