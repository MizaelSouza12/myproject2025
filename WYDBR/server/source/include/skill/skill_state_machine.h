/**
 * @file skill_state_machine.h
 * @brief Sistema de máquina de estados para habilidades
 * 
 * Este arquivo implementa uma máquina de estados para gerenciar o ciclo de vida
 * e execução das habilidades, incluindo fases de preparação, execução, cooldown, etc.
 */

#ifndef _SKILL_STATE_MACHINE_H_
#define _SKILL_STATE_MACHINE_H_

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace wydbr {
namespace skill {

// Estados da máquina de estados de habilidades
#define SKILL_STATE_IDLE          0
#define SKILL_STATE_PREPARATION   1
#define SKILL_STATE_CASTING       2
#define SKILL_STATE_CHANNELING    3
#define SKILL_STATE_RECOVERY      4
#define SKILL_STATE_COOLDOWN      5
#define SKILL_STATE_ERROR         6

// Tamanho do token de segurança
#define SKILL_TOKEN_LENGTH        16

/**
 * Contexto de execução de uma habilidade
 */
struct SKILL_EXECUTION_CONTEXT {
    int entityId;                          // ID da entidade executora
    int skillId;                           // ID da habilidade
    unsigned int startTime;                // Timestamp de início
    unsigned int lastTransition;           // Último timestamp de transição
    char token[SKILL_TOKEN_LENGTH + 1];    // Token de segurança
    int currentState;                      // Estado atual
    int previousState;                     // Estado anterior
    bool interruptible;                    // Se pode ser interrompida
    int charges;                           // Cargas disponíveis
    int maxCharges;                        // Máximo de cargas
    unsigned int cooldownEndTime;          // Timestamp de fim do cooldown
};

/**
 * Máquina de estados para habilidades
 */
class SkillStateMachine {
public:
    /**
     * Construtor
     * @param entityId ID da entidade (jogador, mob)
     * @param skillId ID da habilidade
     */
    SkillStateMachine(int entityId, int skillId);
    
    /**
     * Destrutor
     */
    ~SkillStateMachine();
    
    /**
     * Inicia a execução da habilidade
     * @return true se iniciou com sucesso, false caso contrário
     */
    bool StartExecution();
    
    /**
     * Interrompe a execução da habilidade
     * @param reason Motivo da interrupção
     * @return true se interrompeu com sucesso, false caso contrário
     */
    bool InterruptExecution(const char* reason);
    
    /**
     * Verifica se a habilidade pode ser executada
     * @return true se pode executar, false caso contrário
     */
    bool CanExecute() const;
    
    /**
     * Verifica se a habilidade está em cooldown
     * @return true se em cooldown, false caso contrário
     */
    bool IsInCooldown() const;
    
    /**
     * Gera o token de segurança
     * @param outToken Buffer para o token (deve ter tamanho >= SKILL_TOKEN_LENGTH+1)
     */
    void GenerateSecurityToken(char* outToken);
    
    /**
     * Valida um token recebido
     * @param token Token a validar
     * @return true se válido, false caso contrário
     */
    bool ValidateToken(const char* token);
    
    /**
     * Processa a transição para um novo estado
     * @param newState Novo estado
     * @return true se a transição foi válida, false caso contrário
     */
    bool TransitionTo(int newState);
    
    /**
     * Registra um uso da habilidade para monitorar abusos
     * @param success Se a execução foi bem-sucedida
     */
    void RegisterExecution(bool success);
    
    /**
     * Obtém o contexto atual
     * @return Referência para o contexto de execução
     */
    const SKILL_EXECUTION_CONTEXT& GetContext() const { return m_context; }
    
private:
    // Contexto de execução
    SKILL_EXECUTION_CONTEXT m_context;
    
    // Histórico de execuções para detecção de anomalias
    std::vector<std::pair<unsigned int, bool>> m_executionHistory;
    
    // Contador de execuções anômalas
    int m_anomalousExecutions;
    
    // Tempos de duração de cada fase (em ms)
    std::unordered_map<int, int> m_timings;
    
    /**
     * Atualiza os timings da habilidade
     * @param skillId ID da habilidade
     */
    void _UpdateTimings(int skillId);
    
    /**
     * Verifica se uma transição entre estados é válida
     * @param fromState Estado de origem
     * @param toState Estado de destino
     * @return true se válida, false caso contrário
     */
    bool _IsValidTransition(int fromState, int toState);
    
    /**
     * Gera um token seguro
     * @param outToken Buffer para o token
     */
    void _GenerateToken(char* outToken);
    
    /**
     * Detecta padrões anômalos de uso
     * @return true se padrões suspeitos foram detectados
     */
    bool _DetectAnomalies();
};

} // namespace skill
} // namespace wydbr

#endif // _SKILL_STATE_MACHINE_H_
