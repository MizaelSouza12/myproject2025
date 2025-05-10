/**
 * @file RateLimiter.h
 * @brief Sistema de limitação de taxa para proteção contra DoS
 */

#ifndef WYDBR_RATE_LIMITER_H
#define WYDBR_RATE_LIMITER_H

#include <unordered_map>
#include <string>
#include <cstdint>
#include <chrono>
#include <mutex>
#include <vector>
#include "NetworkProtocol.h"

namespace WYDBR {
namespace Network {

/**
 * Configuração de limite para um tipo de pacote
 */
struct RateLimitConfig {
    PacketType packetType;          // Tipo de pacote
    uint32_t maxPerSecond;          // Máximo por segundo
    uint32_t burstAllowance;        // Tolerância para burst
    uint32_t penaltyThreshold;      // Limite para aplicar penalidade
    uint32_t penaltyDurationMs;     // Duração da penalidade em ms
    bool blockIfExceeded;           // Bloquear se exceder
};

/**
 * Estado atual de limitação para um cliente
 */
struct RateLimitState {
    uint64_t clientId;              // ID do cliente
    PacketType packetType;          // Tipo de pacote
    uint32_t counter;               // Contador atual
    uint64_t windowStartTime;       // Início da janela atual
    uint32_t burstCounter;          // Contador de burst
    uint64_t penaltyEndTime;        // Fim da penalidade
    uint32_t totalViolations;       // Total de violações
};

/**
 * Sistema de limitação de taxa
 */
class RateLimiter {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o limitador
     */
    static RateLimiter& GetInstance();
    
    /**
     * Adiciona configuração de limite
     * @param config Configuração de limite
     */
    void AddLimitConfig(const RateLimitConfig& config);
    
    /**
     * Remove configuração de limite
     * @param packetType Tipo de pacote
     */
    void RemoveLimitConfig(PacketType packetType);
    
    /**
     * Verifica se um pacote está dentro do limite
     * @param clientId ID do cliente
     * @param packetType Tipo de pacote
     * @return true se o pacote pode ser processado
     */
    bool CheckLimit(uint64_t clientId, PacketType packetType);
    
    /**
     * Verifica se um cliente está sob penalidade
     * @param clientId ID do cliente
     * @param packetType Tipo de pacote
     * @return true se estiver sob penalidade
     */
    bool IsClientPenalized(uint64_t clientId, PacketType packetType);
    
    /**
     * Aplica penalidade a um cliente
     * @param clientId ID do cliente
     * @param packetType Tipo de pacote
     * @param durationMs Duração da penalidade em ms
     */
    void PenalizeClient(uint64_t clientId, PacketType packetType, uint32_t durationMs);
    
    /**
     * Remove penalidade de um cliente
     * @param clientId ID do cliente
     * @param packetType Tipo de pacote
     */
    void RemovePenalty(uint64_t clientId, PacketType packetType);
    
    /**
     * Limpa estados expirados
     * @return Número de estados limpos
     */
    int CleanupExpiredStates();
    
    /**
     * Obtém estatísticas de limitação
     * @return String formatada com estatísticas
     */
    std::string GetStatistics() const;
    
    /**
     * Reseta contadores para um cliente
     * @param clientId ID do cliente
     */
    void ResetCounters(uint64_t clientId);
    
private:
    // Construtor e destrutor privados (singleton)
    RateLimiter();
    ~RateLimiter();
    
    // Não permitir cópias
    RateLimiter(const RateLimiter&) = delete;
    RateLimiter& operator=(const RateLimiter&) = delete;
    
    // Mapa de configurações por tipo de pacote
    std::unordered_map<uint16_t, RateLimitConfig> m_configs;
    
    // Mapa de estados por cliente e tipo de pacote
    std::unordered_map<uint64_t, std::unordered_map<uint16_t, RateLimitState>> m_states;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Métodos privados
    RateLimitState* GetOrCreateState(uint64_t clientId, PacketType packetType);
    uint64_t GetKey(uint64_t clientId, uint16_t packetType) const;
    void UpdateWindow(RateLimitState& state);
};

} // namespace Network
} // namespace WYDBR

#endif // WYDBR_RATE_LIMITER_H
