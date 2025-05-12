/**
 * @file skill_security_manager.h
 * @brief Sistema de segurança para o sistema de habilidades
 * 
 * Este arquivo implementa um sistema de segurança que detecta e previne
 * tentativas de explorar o sistema de habilidades através de hacking,
 * modificação de pacotes ou outras técnicas.
 */

#ifndef _SKILL_SECURITY_MANAGER_H_
#define _SKILL_SECURITY_MANAGER_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>

namespace wydbr {
namespace security {

/**
 * Tipos de violações de segurança
 */
enum SkillSecurityViolationType {
    VIOLATION_NONE,
    VIOLATION_COOLDOWN_HACK,
    VIOLATION_TOKEN_MISMATCH,
    VIOLATION_IMPOSSIBLE_TIMING,
    VIOLATION_RAPID_USAGE,
    VIOLATION_PARAMETER_TAMPERING
};

/**
 * Registro de violação
 */
struct SkillViolationRecord {
    SkillSecurityViolationType type;
    int entityId;
    int skillId;
    std::string details;
    unsigned long long timestamp;
    int severity;  // 1-10
};

/**
 * Gerenciador de segurança de habilidades
 */
class SkillSecurityManager {
public:
    static SkillSecurityManager& GetInstance();
    
    /**
     * Gera token de segurança para uso de habilidade
     * @param skillId ID da habilidade
     * @param entityId ID da entidade
     * @return Token de segurança
     */
    std::string GenerateSecurityToken(int skillId, int entityId);
    
    /**
     * Valida token de segurança
     * @param skillId ID da habilidade
     * @param entityId ID da entidade
     * @param token Token a validar
     * @return true se válido, false caso contrário
     */
    bool ValidateToken(int skillId, int entityId, const std::string& token);
    
    /**
     * Verifica se uso de habilidade é válido temporalmente
     * @param skillId ID da habilidade
     * @param entityId ID da entidade
     * @return true se válido, false caso contrário
     */
    bool ValidateUsageTiming(int skillId, int entityId);
    
    /**
     * Registra violação de segurança
     * @param type Tipo de violação
     * @param entityId ID da entidade
     * @param skillId ID da habilidade
     * @param details Detalhes da violação
     * @param severity Severidade (1-10)
     */
    void RegisterViolation(SkillSecurityViolationType type, int entityId, 
                        int skillId, const std::string& details, int severity);
    
    /**
     * Obtém histórico de violações
     * @param entityId ID da entidade
     * @return Lista de violações
     */
    std::vector<SkillViolationRecord> GetViolationHistory(int entityId);
    
    /**
     * Limpa histórico de violações
     * @param entityId ID da entidade
     */
    void ClearViolationHistory(int entityId);
    
    /**
     * Define callback para ações em caso de violação
     * @param callback Função a chamar quando ocorrer violação
     */
    void SetViolationCallback(std::function<void(const SkillViolationRecord&)> callback);
    
private:
    SkillSecurityManager();
    ~SkillSecurityManager();
    
    // Tokens ativos
    std::unordered_map<std::string, std::string> m_activeTokens;
    
    // Histórico de violações
    std::unordered_map<int, std::vector<SkillViolationRecord>> m_violationHistory;
    
    // Timestamps de último uso
    std::unordered_map<int, std::unordered_map<int, unsigned long long>> m_lastUsageTime;
    
    // Callback para violações
    std::function<void(const SkillViolationRecord&)> m_violationCallback;
    
    // Mutex para acesso thread-safe
    std::mutex m_mutex;
    
    // Gera hash seguro
    std::string _GenerateHash(const std::string& input);
    
    // Verifica padrões de uso suspeitos
    bool _DetectAnomalousPattern(int entityId, int skillId);
};

} // namespace security
} // namespace wydbr

#endif // _SKILL_SECURITY_MANAGER_H_
