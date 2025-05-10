/**
 * @file MovementValidator.h
 * @brief Sistema de validação de movimento seguro para WYDBR 2.0
 */

#ifndef WYDBR_MOVEMENT_VALIDATOR_H
#define WYDBR_MOVEMENT_VALIDATOR_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

namespace WYDBR {

// Forward declarations
class Entity;
class Position;
class Grid;
class CryptoManager;

namespace Security {
class SecurityManager;
}

namespace Movement {

/**
 * @enum MovementValidationResult
 * @brief Resultado da validação de movimento
 */
enum class MovementValidationResult {
    VALID,                      // Movimento válido e autenticado
    INVALID_POSITION,           // Posição inválida (fora dos limites do mundo)
    TERRAIN_BLOCKED,            // Terreno bloqueado (não caminhável)
    COLLISION,                  // Colisão com outra entidade
    MOVEMENT_CAPACITY,          // Entidade não pode se mover desta forma
    DISTANCE_EXCEEDED,          // Distância de movimento excedida
    SPEED_EXCEEDED,             // Velocidade de movimento excedida
    AUTHENTICATION_FAILED,      // Falha na autenticação do pacote
    RATE_LIMIT_EXCEEDED,        // Limite de taxa de movimentos excedido
    SERVER_ERROR                // Erro interno do servidor
};

/**
 * @struct MovementAuthenticationData
 * @brief Dados para autenticação de movimentos
 */
struct MovementAuthenticationData {
    std::string entityId;
    std::string sessionId;
    uint64_t timestamp;
    uint32_t moveCounter;
    std::string token;
};

/**
 * @struct RateLimitEntry
 * @brief Estrutura para controle de rate limit
 */
struct RateLimitEntry {
    std::chrono::system_clock::time_point lastMoveTime;
    std::vector<std::chrono::system_clock::time_point> recentMoves;
    uint32_t counter = 0;
};

/**
 * @class MovementValidator
 * @brief Valida e autentica movimentos de entidades
 * 
 * Esta classe separa claramente as responsabilidades de validação física de movimento
 * (verificação de possibilidade) da autenticação dos pacotes de movimento.
 */
class MovementValidator {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância única
     */
    static MovementValidator& getInstance();
    
    /**
     * @brief Verifica se um movimento é fisicamente possível
     * @param entity Entidade que se move
     * @param currentPos Posição atual da entidade
     * @param newPos Nova posição pretendida
     * @return Resultado da validação
     */
    MovementValidationResult validateMovement(const Entity* entity, 
                                           const Position& currentPos,
                                           const Position& newPos);
    
    /**
     * @brief Autentica um pacote de movimento
     * @param authData Dados de autenticação do pacote
     * @param currentPos Posição atual da entidade
     * @param newPos Nova posição pretendida
     * @return true se o pacote é autêntico
     */
    bool authenticateMovementPacket(const MovementAuthenticationData& authData,
                                   const Position& currentPos,
                                   const Position& newPos);
    
    /**
     * @brief Processa um movimento completo (validação + autenticação)
     * @param entity Entidade que se move
     * @param currentPos Posição atual da entidade
     * @param newPos Nova posição pretendida
     * @param authData Dados de autenticação
     * @return Resultado da validação
     */
    MovementValidationResult validateAndAuthenticateMovement(
        const Entity* entity,
        const Position& currentPos,
        const Position& newPos,
        const MovementAuthenticationData& authData);
    
    /**
     * @brief Define limite da taxa de movimentos
     * @param maxMovesPerSecond Número máximo de movimentos por segundo
     * @param burstSize Tamanho do burst permitido
     */
    void setRateLimit(float maxMovesPerSecond, int burstSize);
    
    /**
     * @brief Define distância máxima de movimento por pacote
     * @param maxDistance Distância máxima permitida
     */
    void setMaxMovementDistance(float maxDistance);

private:
    // Singleton
    static std::unique_ptr<MovementValidator> s_instance;
    static std::once_flag s_onceFlag;
    
    MovementValidator();
    ~MovementValidator();
    
    // Métodos auxiliares
    bool isWithinWorldBounds(const Position& pos) const;
    bool isTerrainWalkable(const Position& pos) const;
    bool hasLineOfSight(const Position& start, const Position& end) const;
    bool isWithinMovementRange(const Entity* entity, const Position& currentPos, const Position& newPos) const;
    bool hasEntityCollision(const Entity* entity, const Position& pos) const;
    
    // Validação de rate limit
    bool checkRateLimit(const std::string& entityId);
    void cleanupOldRateLimitEntries();
    
    // Cálculo de token de movimento
    std::string calculateMovementToken(const std::string& entityId,
                                     const std::string& sessionId,
                                     uint64_t timestamp,
                                     uint32_t moveCounter,
                                     const Position& currentPos,
                                     const Position& newPos) const;
    
    // Membros
    std::shared_ptr<Grid> m_grid;
    std::shared_ptr<Security::SecurityManager> m_securityManager;
    std::shared_ptr<CryptoManager> m_cryptoManager;
    
    // Configurações
    float m_maxMovementDistance;
    float m_maxMovesPerSecond;
    int m_rateLimitBurstSize;
    std::chrono::milliseconds m_rateWindowDuration;
    std::chrono::milliseconds m_timestampTolerance;
    
    // Controle de rate limit
    std::unordered_map<std::string, RateLimitEntry> m_rateLimitMap;
    std::mutex m_rateLimitMutex;
    std::chrono::system_clock::time_point m_lastCleanupTime;
};

} // namespace Movement
} // namespace WYDBR

#endif // WYDBR_MOVEMENT_VALIDATOR_H