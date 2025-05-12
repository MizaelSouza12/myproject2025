/**
 * @file skill_state_machine.h
 * @brief Sistema avançado de máquina de estados para habilidades
 * 
 * Este arquivo define a máquina de estados para gerenciamento de habilidades
 * no WYDBR, permitindo execução, interrupção, combos e transições de estado
 * controladas com validação de segurança.
 * 
 * @author WYDBR Team
 * @version 1.0.0
 * @date 2025-05-06
 */

#ifndef WYDBR_SKILL_STATE_MACHINE_H
#define WYDBR_SKILL_STATE_MACHINE_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <optional>
#include <queue>
#include <deque>
#include <random>

namespace wydbr {
namespace gameplay {
namespace skills {

/**
 * @enum SkillState
 * @brief Estados possíveis para uma habilidade
 */
enum /**
 * Classe WYDSkillState
 * 
 * Esta classe implementa a funcionalidade WYDSkillState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillState {
    IDLE,           ///< Estado de repouso
    PREPARATION,     ///< Preparação para uso
    CASTING,         ///< Executando a habilidade
    RECOVERY,        ///< Recuperação após uso
    COOLDOWN,        ///< Em tempo de recarga
    CHANNELING,      ///< Canalizando efeito contínuo
    INTERRUPTED,     ///< Habilidade interrompida
    FAILED           ///< Falha na execução
};

/**
 * @enum SkillCategory
 * @brief Categorias de habilidades
 */
enum /**
 * Classe WYDSkillCategory
 * 
 * Esta classe implementa a funcionalidade WYDSkillCategory conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillCategory {
    MAGIC,           ///< Magia
    MELEE,           ///< Corpo a corpo
    RANGED,          ///< À distância
    MOVEMENT,        ///< Movimento
    BUFF,            ///< Melhoria
    DEBUFF,          ///< Penalidade
    HEALING,         ///< Cura
    SUMMON,          ///< Invocação
    UTILITY          ///< Utilidade
};

/**
 * @enum SkillEvent
 * @brief Eventos que podem ocorrer com habilidades
 */
enum /**
 * Classe WYDSkillEvent
 * 
 * Esta classe implementa a funcionalidade WYDSkillEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillEvent {
    EXECUTE,         ///< Execução iniciada
    PROGRESS,        ///< Progresso na execução
    FINISH_PREP,     ///< Preparação concluída
    FINISH_CAST,     ///< Cast concluído
    FINISH_RECOVERY, ///< Recuperação concluída
    INTERRUPT,       ///< Interrupção
    CANCEL,          ///< Cancelamento
    COOLDOWN_END,    ///< Fim do cooldown
    CHARGE_GAINED,   ///< Carga recuperada
    CHAINING,        ///< Encadeamento com outra habilidade
    ERROR            ///< Erro na execução
};

/**
 * @enum ExecutionResult
 * @brief Resultado da tentativa de execução de habilidade
 */
enum /**
 * Classe WYDExecutionResult
 * 
 * Esta classe implementa a funcionalidade WYDExecutionResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ExecutionResult {
    SUCCESS,                ///< Executada com sucesso
    FAILED_COOLDOWN,        ///< Falhou por estar em cooldown
    FAILED_NO_CHARGES,      ///< Falhou por não ter cargas disponíveis
    FAILED_REQUIREMENTS,    ///< Falhou por não cumprir requisitos
    FAILED_STATE,           ///< Falhou por estar em estado incompatível
    FAILED_INTERRUPTED,     ///< Falhou por ser interrompida
    FAILED_PERMISSIONS,     ///< Falhou por falta de permissões
    FAILED_TARGET,          ///< Falhou por problemas com alvo
    FAILED_COMBO,           ///< Falhou por problemas no combo
    FAILED_SECURITY,        ///< Falhou por violação de segurança
    BUFFERED                ///< Colocada em buffer para execução futura
};

/**
 * @enum InterruptResult
 * @brief Resultado da tentativa de interrupção de habilidade
 */
enum /**
 * Classe WYDInterruptResult
 * 
 * Esta classe implementa a funcionalidade WYDInterruptResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InterruptResult {
    SUCCESS,               ///< Interrompida com sucesso
    FAILED_UNINTERRUPTIBLE,///< Falhou por ser não-interruptível
    FAILED_STATE,          ///< Falhou por estar em estado que não permite interrupção
    FAILED_PERMISSIONS,    ///< Falhou por falta de permissões
    FAILED_PRIORITY,       ///< Falhou por prioridade insuficiente
    NONE                   ///< Nenhuma ação necessária
};

/**
 * @enum ViolationType
 * @brief Tipos de violação de segurança
 */
enum /**
 * Classe WYDViolationType
 * 
 * Esta classe implementa a funcionalidade WYDViolationType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ViolationType {
    NONE,                  ///< Nenhuma violação
    COOLDOWN_MANIPULATION, ///< Manipulação de cooldown
    CHARGE_MANIPULATION,   ///< Manipulação de cargas
    STATE_MANIPULATION,    ///< Manipulação de estado
    TOKEN_MISMATCH,        ///< Token de execução inválido
    TIMING_MANIPULATION,   ///< Manipulação de timing
    COMBO_EXPLOITATION,    ///< Exploração de combos
    RAPID_EXECUTION,       ///< Execução rápida demais
    PARAMETER_TAMPERING,   ///< Adulteração de parâmetros
    PERMISSION_BYPASS      ///< Bypass de permissões
};

/**
 * @struct SkillTiming
 * @brief Configuração de timing para habilidades
 */
struct SkillTiming {
    uint32_t preparation = 0;   ///< Tempo de preparação em ms
    uint32_t casting = 0;       ///< Tempo de casting em ms
    uint32_t recovery = 0;      ///< Tempo de recuperação em ms
    uint32_t cooldown = 0;      ///< Tempo de cooldown em ms
    uint32_t channelDuration = 0; ///< Duração de canalização em ms
    float variability = 0.0f;   ///< Variabilidade de timing (0.0-1.0)
};

/**
 * @struct SkillRequirements
 * @brief Requisitos para uso de habilidade
 */
struct SkillRequirements {
    uint32_t mana = 0;         ///< Mana necessária
    uint32_t stamina = 0;      ///< Stamina necessária
    uint32_t health = 0;       ///< Vida necessária
    uint32_t level = 0;        ///< Nível necessário
    std::vector<std::string> items; ///< Itens necessários
    std::vector<std::string> buffs; ///< Buffs necessários
    std::vector<std::string> states; ///< Estados necessários
};

/**
 * @struct SkillExecutionContext
 * @brief Contexto de execução de habilidade
 */
struct SkillExecutionContext {
    std::string executionId;    ///< ID única da execução
    std::string targetId;       ///< ID do alvo (se houver)
    std::string securityToken;  ///< Token de segurança
    std::unordered_map<std::string, std::string> parameters; ///< Parâmetros adicionais
    uint64_t clientTimestamp;   ///< Timestamp do cliente
    uint64_t serverTimestamp;   ///< Timestamp do servidor
    std::unordered_map<std::string, float> multipliers; ///< Multiplicadores de efeito
    bool isServerAuthoritative; ///< Se o servidor é autoritativo
};

/**
 * @struct SkillComboStep
 * @brief Etapa em uma sequência de combo
 */
struct SkillComboStep {
    std::string skillId;        ///< ID da habilidade
    uint32_t windowStart;       ///< Início da janela de combo em ms
    uint32_t windowDuration;    ///< Duração da janela em ms
    float damageMultiplier;     ///< Multiplicador de dano
    bool requiresSuccess;       ///< Se requer sucesso na etapa anterior
};

/**
 * @struct SkillExecution
 * @brief Representa uma execução específica de habilidade
 */
struct SkillExecution {
    std::string executionId;    ///< ID única da execução
    SkillState state;           ///< Estado atual
    uint64_t startTime;         ///< Timestamp de início
    uint64_t stateStartTime;    ///< Timestamp de início do estado atual
    bool interrupted;           ///< Se foi interrompida
    uint32_t progressPercent;   ///< Progresso em porcentagem
    SkillExecutionContext context; ///< Contexto de execução
    std::unordered_map<std::string, std::string> results; ///< Resultados da execução
    bool completed;             ///< Se foi completada
    uint32_t retryCount;        ///< Número de tentativas
};

/**
 * @class SkillStateMachine
 * @brief Implementação da máquina de estados para habilidades
 */
/**
 * Classe WYDSkillStateMachine
 * 
 * Esta classe implementa a funcionalidade WYDSkillStateMachine conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillStateMachine {
public:
    /**
     * @typedef EventCallback
     * @brief Tipo para callback de eventos de habilidade
     */
    using EventCallback = std::function<void(const std::string&, SkillEvent, const std::unordered_map<std::string, std::string>&)>;
    
    /**
     * @typedef SecurityCallback
     * @brief Tipo para callback de violações de segurança
     */
    using SecurityCallback = std::function<void(ViolationType, const std::unordered_map<std::string, std::string>&)>;
    
    /**
     * @brief Construtor
     * @param skillId ID da habilidade
     * @param skillName Nome da habilidade
     * @param options Opções de configuração
     */
    SkillStateMachine(const std::string& skillId, const std::string& skillName, 
                     const std::unordered_map<std::string, std::string>& options = {});
    
    /**
     * @brief Destrutor
     */
    ~SkillStateMachine();
    
    /**
     * @brief Inicializa a máquina de estados
     * @param timing Configuração de timing
     * @param requirements Requisitos de uso
     * @param category Categoria da habilidade
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool initialize(const SkillTiming& timing, const SkillRequirements& requirements, 
                   SkillCategory category);
    
    /**
     * @brief Tenta executar a habilidade
     * @param context Contexto de execução
     * @return Resultado da tentativa
     */
    ExecutionResult execute(const SkillExecutionContext& context);
    
    /**
     * @brief Tenta interromper a execução atual
     * @param executionId ID da execução a interromper
     * @param reason Motivo da interrupção
     * @param priority Prioridade da interrupção
     * @return Resultado da tentativa
     */
    InterruptResult interrupt(const std::string& executionId, const std::string& reason, uint32_t priority = 0);
    
    /**
     * @brief Atualiza a máquina de estados
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void update(float deltaTime);
    
    /**
     * @brief Verifica se a habilidade está em cooldown
     * @return true se estiver em cooldown, false caso contrário
     */
    bool isOnCooldown() const;
    
    /**
     * @brief Obtém o tempo restante de cooldown
     * @return Tempo restante em milissegundos
     */
    uint32_t getRemainingCooldown() const;
    
    /**
     * @brief Obtém o número de cargas disponíveis
     * @return Número de cargas
     */
    uint32_t getCharges() const;
    
    /**
     * @brief Obtém o tempo para próxima carga
     * @return Tempo em milissegundos
     */
    uint32_t getTimeToNextCharge() const;
    
    /**
     * @brief Verifica se a habilidade pode ser executada
     * @param checkRequirements Se deve verificar requisitos
     * @return true se pode ser executada, false caso contrário
     */
    bool canExecute(bool checkRequirements = true) const;
    
    /**
     * @brief Obtém o estado atual da habilidade
     * @return Estado atual
     */
    SkillState getCurrentState() const;
    
    /**
     * @brief Obtém a execução atual
     * @return Execução atual ou std::nullopt
     */
    std::optional<SkillExecution> getCurrentExecution() const;
    
    /**
     * @brief Adiciona uma etapa de combo
     * @param previousSkillId ID da habilidade anterior
     * @param comboStep Configuração da etapa
     * @return true se adicionado com sucesso, false caso contrário
     */
    bool addComboStep(const std::string& previousSkillId, const SkillComboStep& comboStep);
    
    /**
     * @brief Verifica se a habilidade atual pode ser parte de um combo
     * @param previousSkillId ID da habilidade anterior
     * @return true se pode formar combo, false caso contrário
     */
    bool canComboAfter(const std::string& previousSkillId) const;
    
    /**
     * @brief Registra um callback para eventos
     * @param callback Função de callback
     * @return ID do registro
     */
    uint64_t registerEventCallback(EventCallback callback);
    
    /**
     * @brief Registra um callback para segurança
     * @param callback Função de callback
     * @return ID do registro
     */
    uint64_t registerSecurityCallback(SecurityCallback callback);
    
    /**
     * @brief Remove um callback de eventos
     * @param callbackId ID do registro
     * @return true se removido com sucesso, false caso contrário
     */
    bool unregisterEventCallback(uint64_t callbackId);
    
    /**
     * @brief Remove um callback de segurança
     * @param callbackId ID do registro
     * @return true se removido com sucesso, false caso contrário
     */
    bool unregisterSecurityCallback(uint64_t callbackId);
    
    /**
     * @brief Obtém o ID da habilidade
     * @return ID da habilidade
     */
    const std::string& getSkillId() const { return skillId_; }
    
    /**
     * @brief Obtém o nome da habilidade
     * @return Nome da habilidade
     */
    const std::string& getSkillName() const { return skillName_; }
    
    /**
     * @brief Obtém a categoria da habilidade
     * @return Categoria da habilidade
     */
    SkillCategory getCategory() const { return category_; }
    
    /**
     * @brief Verifica se a habilidade está inicializada
     * @return true se inicializada, false caso contrário
     */
    bool isInitialized() const { return initialized_; }
    
    /**
     * @brief Obtém estatísticas da habilidade
     * @return Mapa com estatísticas
     */
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> getStats() const;
    
    /**
     * @brief Verifica se a habilidade pode ser interrompida
     * @return true se pode ser interrompida, false caso contrário
     */
    bool isInterruptible() const { return interruptible_; }
    
    /**
     * @brief Define se a habilidade pode ser interrompida
     * @param interruptible Se pode ser interrompida
     */
    void setInterruptible(bool interruptible) { interruptible_ = interruptible; }
    
    /**
     * @brief Obtém o timing da habilidade
     * @return Referência para o timing
     */
    const SkillTiming& getTiming() const { return timing_; }
    
    /**
     * @brief Obtém os requisitos da habilidade
     * @return Referência para os requisitos
     */
    const SkillRequirements& getRequirements() const { return requirements_; }
    
private:
    std::string skillId_;                   ///< ID da habilidade
    std::string skillName_;                 ///< Nome da habilidade
    bool initialized_ = false;              ///< Se foi inicializada
    bool enabled_ = true;                   ///< Se está habilitada
    bool interruptible_ = true;             ///< Se pode ser interrompida
    SkillCategory category_;                ///< Categoria da habilidade
    SkillTiming timing_;                    ///< Configuração de timing
    SkillRequirements requirements_;        ///< Requisitos de uso
    
    SkillState currentState_ = SkillState::IDLE; ///< Estado atual
    uint64_t stateStartTime_ = 0;           ///< Timestamp de início do estado atual
    uint64_t cooldownEndTime_ = 0;          ///< Timestamp de fim do cooldown
    uint32_t charges_ = 1;                  ///< Cargas disponíveis
    uint32_t maxCharges_ = 1;               ///< Máximo de cargas
    uint64_t nextChargeTime_ = 0;           ///< Timestamp da próxima carga
    uint32_t chargeRestoreTime_ = 0;        ///< Tempo para restaurar uma carga
    
    std::optional<SkillExecution> currentExecution_; ///< Execução atual
    std::deque<SkillExecutionContext> executionBuffer_; ///< Buffer de execuções pendentes
    uint32_t bufferSize_ = 5;               ///< Tamanho máximo do buffer
    
    // Combos
    std::unordered_map<std::string, std::vector<SkillComboStep>> comboSteps_; ///< Etapas de combo
    std::optional<std::string> lastComboSkillId_; ///< Última habilidade no combo
    uint64_t lastComboEndTime_ = 0;         ///< Timestamp do fim do último combo
    
    // Callbacks
    struct CallbackEntry {
        uint64_t id;
        EventCallback callback;
    };
    std::vector<CallbackEntry> eventCallbacks_; ///< Callbacks de eventos
    std::vector<CallbackEntry> securityCallbacks_; ///< Callbacks de segurança
    uint64_t nextCallbackId_ = 1;            ///< Próximo ID de callback
    
    // Segurança
    std::unordered_map<std::string, std::string> securityTokens_; ///< Tokens de segurança
    std::unordered_map<std::string, uint64_t> executionTimestamps_; ///< Timestamps de execução
    float anomalyThreshold_ = 0.85f;          ///< Limite para detecção de anomalias
    
    // Estatísticas
    struct Statistics {
        uint64_t totalExecutions = 0;
        uint64_t successfulExecutions = 0;
        uint64_t failedExecutions = 0;
        uint64_t interruptions = 0;
        uint64_t securityViolations = 0;
        uint64_t bufferOverflows = 0;
        uint64_t combosInitiated = 0;
        uint64_t combosCompleted = 0;
        float averageExecutionTime = 0.0f;
        uint32_t consecutiveFailures = 0;
    } stats_;
    
    // Gerador de IDs e tokens
    std::mt19937 rng_;
    
    // Mutex para proteção de acesso concorrente
    mutable std::mutex mutex_;
    
    /**
     * @brief Verifica se os requisitos são atendidos
     * @param context Contexto de execução
     * @return true se atendidos, false caso contrário
     */
    bool checkRequirements(const SkillExecutionContext& context) const;
    
    /**
     * @brief Gera ID única para execução
     * @return ID gerada
     */
    std::string generateExecutionId();
    
    /**
     * @brief Gera token de segurança
     * @return Token gerado
     */
    std::string generateSecurityToken();
    
    /**
     * @brief Transiciona para um novo estado
     * @param newState Novo estado
     * @param executionId ID da execução
     */
    void transitionToState(SkillState newState, const std::string& executionId);
    
    /**
     * @brief Dispara um evento
     * @param executionId ID da execução
     * @param event Evento
     * @param data Dados adicionais
     */
    void fireEvent(const std::string& executionId, SkillEvent event, 
                  const std::unordered_map<std::string, std::string>& data = {});
    
    /**
     * @brief Reporta violação de segurança
     * @param type Tipo de violação
     * @param context Contexto da violação
     */
    void reportSecurityViolation(ViolationType type, 
                               const std::unordered_map<std::string, std::string>& context);
    
    /**
     * @brief Processa uma execução completa
     * @param execution Execução a ser processada
     */
    void processCompletedExecution(const SkillExecution& execution);
    
    /**
     * @brief Consume uma carga da habilidade
     * @return true se consumida, false caso contrário
     */
    bool consumeCharge();
    
    /**
     * @brief Restaura uma carga da habilidade
     */
    void restoreCharge();
    
    /**
     * @brief Processa o buffer de execuções
     */
    void processExecutionBuffer();
    
    /**
     * @brief Aplica efeitos da habilidade
     * @param execution Execução da habilidade
     * @param effectType Tipo de efeito
     * @return true se aplicado com sucesso, false caso contrário
     */
    bool applySkillEffects(const SkillExecution& execution, const std::string& effectType);
    
    /**
     * @brief Atualiza cooldowns e cargas
     * @param currentTime Timestamp atual
     */
    void updateCooldownsAndCharges(uint64_t currentTime);
    
    /**
     * @brief Verifica timing anômalo
     * @param context Contexto de execução
     * @return true se anômalo, false caso contrário
     */
    bool checkAnomalousExecutionTiming(const SkillExecutionContext& context);
    
    /**
     * @brief Valida token de segurança
     * @param executionId ID da execução
     * @param token Token a validar
     * @return true se válido, false caso contrário
     */
    bool validateSecurityToken(const std::string& executionId, const std::string& token);
};

/**
 * @class SkillManager
 * @brief Gerenciador de habilidades
 */
/**
 * Classe WYDSkillManager
 * 
 * Esta classe implementa a funcionalidade WYDSkillManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SkillManager {
public:
    /**
     * @brief Construtor
     * @param entityId ID da entidade proprietária
     */
    explicit SkillManager(const std::string& entityId);
    
    /**
     * @brief Destrutor
     */
    ~SkillManager();
    
    /**
     * @brief Registra uma habilidade
     * @param skillId ID da habilidade
     * @param name Nome da habilidade
     * @param timing Configuração de timing
     * @param requirements Requisitos de uso
     * @param category Categoria da habilidade
     * @param options Opções adicionais
     * @return true se registrada com sucesso, false caso contrário
     */
    bool registerSkill(const std::string& skillId, const std::string& name,
                      const SkillTiming& timing, const SkillRequirements& requirements,
                      SkillCategory category,
                      const std::unordered_map<std::string, std::string>& options = {});
    
    /**
     * @brief Remove uma habilidade registrada
     * @param skillId ID da habilidade
     * @return true se removida com sucesso, false caso contrário
     */
    bool unregisterSkill(const std::string& skillId);
    
    /**
     * @brief Registra uma categoria de habilidade
     * @param categoryId ID da categoria
     * @param name Nome da categoria
     * @param cooldownSharing Se compartilha cooldown entre habilidades
     * @return true se registrada com sucesso, false caso contrário
     */
    bool registerCategory(const std::string& categoryId, const std::string& name, bool cooldownSharing = false);
    
    /**
     * @brief Obtém uma habilidade
     * @param skillId ID da habilidade
     * @return Ponteiro para a habilidade ou nullptr
     */
    std::shared_ptr<SkillStateMachine> getSkill(const std::string& skillId);
    
    /**
     * @brief Executa uma habilidade
     * @param skillId ID da habilidade
     * @param context Contexto de execução
     * @return Resultado da execução
     */
    ExecutionResult executeSkill(const std::string& skillId, const SkillExecutionContext& context);
    
    /**
     * @brief Interrompe execução de habilidade
     * @param skillId ID da habilidade
     * @param executionId ID da execução
     * @param reason Motivo da interrupção
     * @return Resultado da interrupção
     */
    InterruptResult interruptSkill(const std::string& skillId, const std::string& executionId, const std::string& reason);
    
    /**
     * @brief Registra uma sequência de combo
     * @param comboId ID do combo
     * @param skillSequence Sequência de IDs de habilidades
     * @param options Opções do combo
     * @return true se registrado com sucesso, false caso contrário
     */
    bool registerCombo(const std::string& comboId, const std::vector<std::string>& skillSequence,
                      const std::unordered_map<std::string, std::string>& options = {});
    
    /**
     * @brief Atualiza todas as habilidades
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void update(float deltaTime);
    
    /**
     * @brief Obtém as estatísticas do manager
     * @return Mapa com estatísticas
     */
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> getStats() const;
    
    /**
     * @brief Obtém o ID da entidade
     * @return ID da entidade
     */
    const std::string& getEntityId() const { return entityId_; }
    
private:
    std::string entityId_;  ///< ID da entidade proprietária
    
    // Habilidades registradas
    std::unordered_map<std::string, std::shared_ptr<SkillStateMachine>> skills_;
    
    // Categorias
    struct CategoryInfo {
        std::string name;
        bool cooldownSharing;
        uint64_t globalCooldownEnd;
        std::vector<std::string> skillIds;
    };
    std::unordered_map<std::string, CategoryInfo> categories_;
    
    // Combos
    struct ComboInfo {
        std::string id;
        std::vector<std::string> skillSequence;
        uint32_t windowDuration;
        float damageMultiplier;
        bool requiresPerfectExecution;
    };
    std::unordered_map<std::string, ComboInfo> combos_;
    
    // Tracking de combos ativos
    struct ActiveCombo {
        std::string comboId;
        std::vector<std::string> completedSteps;
        uint64_t lastStepTime;
        uint32_t currentStep;
    };
    std::optional<ActiveCombo> activeCombo_;
    
    // Estatísticas
    struct Statistics {
        uint64_t totalExecutions = 0;
        uint64_t successfulExecutions = 0;
        uint64_t failedExecutions = 0;
        uint64_t combosAttempted = 0;
        uint64_t combosCompleted = 0;
        uint64_t securityViolations = 0;
        uint32_t activeSkills = 0;
    } stats_;
    
    // Mutex para proteção de acesso concorrente
    mutable std::mutex mutex_;
    
    /**
     * @brief Processa categoria após execução de habilidade
     * @param categoryId ID da categoria
     * @param cooldown Cooldown da habilidade
     */
    void processCategoryAfterExecution(const std::string& categoryId, uint32_t cooldown);
    
    /**
     * @brief Verifica global cooldown da categoria
     * @param categoryId ID da categoria
     * @return true se em cooldown, false caso contrário
     */
    bool isCategoryOnGlobalCooldown(const std::string& categoryId) const;
    
    /**
     * @brief Processa progresso de combo
     * @param skillId ID da habilidade executada
     * @param success Se a execução foi bem-sucedida
     */
    void processComboProgress(const std::string& skillId, bool success);
    
    /**
     * @brief Verifica se uma habilidade pode iniciar um combo
     * @param skillId ID da habilidade
     * @return ID do combo possível ou string vazia
     */
    std::string canStartCombo(const std::string& skillId) const;
    
    /**
     * @brief Verifica se uma habilidade continua um combo ativo
     * @param skillId ID da habilidade
     * @return true se continua, false caso contrário
     */
    bool canContinueCombo(const std::string& skillId) const;
    
    /**
     * @brief Processa callback de evento de habilidade
     * @param skillId ID da habilidade
     * @param event Evento ocorrido
     * @param data Dados adicionais
     */
    void onSkillEvent(const std::string& skillId, SkillEvent event, const std::unordered_map<std::string, std::string>& data);
    
    /**
     * @brief Processa callback de violação de segurança
     * @param type Tipo de violação
     * @param context Contexto da violação
     */
    void onSecurityViolation(ViolationType type, const std::unordered_map<std::string, std::string>& context);
};

} // namespace skills
} // namespace gameplay
} // namespace wydbr

#endif // WYDBR_SKILL_STATE_MACHINE_H