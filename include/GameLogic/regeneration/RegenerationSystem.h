/**
 * @file RegenerationSystem.h
 * @brief Sistema de regeneração baseado em tempo absoluto
 */

#ifndef WYDBR_REGENERATION_SYSTEM_H
#define WYDBR_REGENERATION_SYSTEM_H

#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <functional>
#include <atomic>
#include <memory>

namespace WYDBR {
namespace GameLogic {

/**
 * Tipo de recurso
 */
enum class ResourceType {
    Health,             // Vida
    Mana,               // Mana
    Stamina,            // Stamina
    Energy,             // Energia
    Spirit,             // Espírito
    Shield,             // Escudo
    Custom              // Personalizado
};

/**
 * Estado de regeneração
 */
enum class RegenerationState {
    Active,             // Regenerando
    Paused,             // Pausado
    Disabled,           // Desativado
    Draining            // Drenando
};

/**
 * Configuração de regeneração
 */
struct RegenerationConfig {
    ResourceType type;                 // Tipo de recurso
    float baseRate;                    // Taxa base (unidades por segundo)
    float bonusRate;                   // Taxa bônus (unidades por segundo)
    float percentRate;                 // Taxa percentual (% do máximo por segundo)
    int32_t tickIntervalMs;            // Intervalo de ticks em ms
    int32_t updateIntervalMs;          // Intervalo de atualizações para clientes em ms
    bool continueWhenFull;             // Continuar quando cheio
    bool continueInCombat;             // Continuar em combate
    bool allowOverflow;                // Permitir passar do máximo
    bool allowUnderflow;               // Permitir passar do mínimo
    std::vector<std::string> effects;  // Efeitos que afetam a regeneração
};

/**
 * Callback para eventos de regeneração
 */
using RegenerationCallback = std::function<void(uint32_t entityId, ResourceType type, 
                                              float currentValue, float delta)>;

/**
 * Gerenciador de regeneração
 */
class RegenerationManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static RegenerationManager& GetInstance();
    
    /**
     * Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * Registra uma entidade para regeneração
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @param currentValue Valor atual
     * @param maxValue Valor máximo
     * @param config Configuração de regeneração
     * @return true se registrado com sucesso
     */
    bool RegisterEntity(uint32_t entityId, ResourceType type, float currentValue, 
                      float maxValue, const RegenerationConfig& config);
    
    /**
     * Remove uma entidade da regeneração
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @return true se removido com sucesso
     */
    bool UnregisterEntity(uint32_t entityId, ResourceType type);
    
    /**
     * Atualiza o valor atual de um recurso
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @param currentValue Valor atual
     * @return true se atualizado com sucesso
     */
    bool UpdateCurrentValue(uint32_t entityId, ResourceType type, float currentValue);
    
    /**
     * Atualiza o valor máximo de um recurso
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @param maxValue Valor máximo
     * @return true se atualizado com sucesso
     */
    bool UpdateMaxValue(uint32_t entityId, ResourceType type, float maxValue);
    
    /**
     * Atualiza a configuração de regeneração
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @param config Nova configuração
     * @return true se atualizado com sucesso
     */
    bool UpdateConfig(uint32_t entityId, ResourceType type, const RegenerationConfig& config);
    
    /**
     * Define o estado de regeneração
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @param state Novo estado
     * @return true se alterado com sucesso
     */
    bool SetState(uint32_t entityId, ResourceType type, RegenerationState state);
    
    /**
     * Obtém o estado de regeneração
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @return Estado atual
     */
    RegenerationState GetState(uint32_t entityId, ResourceType type) const;
    
    /**
     * Obtém o valor atual de um recurso
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @param outValue Ponteiro para receber o valor
     * @return true se obtido com sucesso
     */
    bool GetCurrentValue(uint32_t entityId, ResourceType type, float* outValue) const;
    
    /**
     * Obtém o valor máximo de um recurso
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @param outValue Ponteiro para receber o valor
     * @return true se obtido com sucesso
     */
    bool GetMaxValue(uint32_t entityId, ResourceType type, float* outValue) const;
    
    /**
     * Adiciona um efeito de regeneração
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @param effectName Nome do efeito
     * @param effectValue Valor do efeito
     * @param durationMs Duração em ms (0 = permanente)
     * @return true se adicionado com sucesso
     */
    bool AddEffect(uint32_t entityId, ResourceType type, const std::string& effectName, 
                 float effectValue, int64_t durationMs = 0);
    
    /**
     * Remove um efeito de regeneração
     * @param entityId ID da entidade
     * @param type Tipo de recurso
     * @param effectName Nome do efeito
     * @return true se removido com sucesso
     */
    bool RemoveEffect(uint32_t entityId, ResourceType type, const std::string& effectName);
    
    /**
     * Registra um callback para eventos de regeneração
     * @param callback Função a ser chamada quando ocorrer regeneração
     */
    void RegisterCallback(RegenerationCallback callback);
    
    /**
     * Atualiza o sistema de regeneração
     * @param deltaTimeMs Tempo decorrido em ms
     */
    void Update(int32_t deltaTimeMs);
    
    /**
     * Define se uma entidade está em combate
     * @param entityId ID da entidade
     * @param inCombat Estado de combate
     */
    void SetEntityInCombat(uint32_t entityId, bool inCombat);
    
    /**
     * Verifica se uma entidade está em combate
     * @param entityId ID da entidade
     * @return true se estiver em combate
     */
    bool IsEntityInCombat(uint32_t entityId) const;
    
private:
    // Construtor e destrutor privados (singleton)
    RegenerationManager();
    ~RegenerationManager();
    
    // Não permitir cópias
    RegenerationManager(const RegenerationManager&) = delete;
    RegenerationManager& operator=(const RegenerationManager&) = delete;
    
    // Efeito de regeneração
    struct RegenerationEffect {
        std::string name;           // Nome do efeito
        float value;                // Valor do efeito
        int64_t startTimeMs;        // Tempo de início
        int64_t durationMs;         // Duração em ms (0 = permanente)
    };
    
    // Informações de recurso
    struct ResourceInfo {
        float currentValue;         // Valor atual
        float maxValue;             // Valor máximo
        RegenerationConfig config;  // Configuração
        RegenerationState state;    // Estado
        int64_t lastTickTimeMs;     // Tempo do último tick
        int64_t lastUpdateTimeMs;   // Tempo da última atualização para clientes
        std::unordered_map<std::string, RegenerationEffect> effects; // Efeitos
    };
    
    // Mapa de recursos por entidade e tipo
    std::unordered_map<uint32_t, std::unordered_map<ResourceType, ResourceInfo>> m_resources;
    
    // Mapa de estado de combate por entidade
    std::unordered_map<uint32_t, bool> m_combatState;
    
    // Callbacks
    std::vector<RegenerationCallback> m_callbacks;
    
    // Mutex para thread-safety
    mutable std::mutex m_resourcesMutex;
    mutable std::mutex m_combatMutex;
    std::mutex m_callbacksMutex;
    
    // Estado
    std::atomic<bool> m_initialized;
    
    // Relógio global
    std::chrono::steady_clock::time_point m_startTime;
    
    // Métodos privados
    bool GetResourceInfo(uint32_t entityId, ResourceType type, ResourceInfo** outInfo);
    bool GetResourceInfo(uint32_t entityId, ResourceType type, const ResourceInfo** outInfo) const;
    void ProcessTick(uint32_t entityId, ResourceType type, ResourceInfo& info, int64_t currentTimeMs);
    void NotifyCallbacks(uint32_t entityId, ResourceType type, float currentValue, float delta);
    float CalculateRegenerationRate(const ResourceInfo& info);
    int64_t GetCurrentTimeMs() const;
};

} // namespace GameLogic
} // namespace WYDBR

#endif // WYDBR_REGENERATION_SYSTEM_H
