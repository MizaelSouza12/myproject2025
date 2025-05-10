/**
 * @file GameLogicIntegration.h
 * @brief Camada de integração da lógica de jogo
 * 
 * Integra os componentes de lógica de jogo com o resto do sistema.
 */

#ifndef WYDBR_GAME_LOGIC_INTEGRATION_H
#define WYDBR_GAME_LOGIC_INTEGRATION_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <unordered_map>

#include "GameLogic/balance/DamageCalculator.h"
#include "GameLogic/pathfinding/Pathfinder.h"
#include "GameLogic/regeneration/RegenerationSystem.h"
#include "Storage/database/DatabaseSchema.h"

namespace WYDBR {
namespace Integration {

/**
 * Configuração da integração de lógica de jogo
 */
struct GameLogicIntegrationConfig {
    bool enableAdvancedDamageCalculation;  // Habilitar cálculo avançado de dano
    bool enablePathfindingCache;           // Habilitar cache de pathfinding
    bool enablePeriodicalDataSync;         // Habilitar sincronização periódica de dados
    int simulationTickRateHz;              // Taxa de tick da simulação em Hz
    std::string gameLogicConfigFile;       // Arquivo de configuração da lógica de jogo
};

/**
 * Eventos da lógica de jogo
 */
enum class GameLogicEvent {
    EntityDamaged,             // Entidade sofreu dano
    EntityHealed,              // Entidade foi curada
    EntityDied,                // Entidade morreu
    EntityRespawned,           // Entidade ressurgiu
    EntityMoved,               // Entidade se moveu
    PathFound,                 // Caminho encontrado
    PathNotFound,              // Caminho não encontrado
    ResourceChanged,           // Recurso alterado
    CombatStarted,             // Combate iniciado
    CombatEnded                // Combate terminado
};

/**
 * Callback para eventos da lógica de jogo
 */
using GameLogicEventCallback = std::function<void(GameLogicEvent, uint32_t, const void*, size_t)>;

/**
 * Integração da lógica de jogo
 */
class GameLogicIntegration {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o integrador
     */
    static GameLogicIntegration& GetInstance();
    
    /**
     * Inicializa a integração
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const GameLogicIntegrationConfig& config);
    
    /**
     * Integra lógica de jogo com armazenamento
     * @return true se integrado com sucesso
     */
    bool IntegrateWithStorage();
    
    /**
     * Integra lógica de jogo com rede
     * @return true se integrado com sucesso
     */
    bool IntegrateWithNetwork();
    
    /**
     * Registra um callback para eventos da lógica de jogo
     * @param event Tipo de evento
     * @param callback Função a ser chamada quando o evento ocorrer
     * @return true se registrado com sucesso
     */
    bool RegisterEventHandler(GameLogicEvent event, GameLogicEventCallback callback);
    
    /**
     * Calcula dano entre entidades
     * @param attackerId ID do atacante
     * @param targetId ID do alvo
     * @param skillId ID da habilidade (0 para ataque normal)
     * @param damageType Tipo de dano
     * @param outDamageResult Ponteiro para receber resultado do dano
     * @return true se calculado com sucesso
     */
    bool CalculateDamage(uint32_t attackerId, uint32_t targetId, uint32_t skillId,
                       GameLogic::DamageType damageType,
                       GameLogic::DamageResult* outDamageResult);
    
    /**
     * Encontra caminho entre pontos
     * @param entityId ID da entidade
     * @param startPos Posição inicial
     * @param goalPos Posição final
     * @param outPath Ponteiro para receber caminho
     * @return true se encontrado com sucesso
     */
    bool FindPath(uint32_t entityId, const GameLogic::Position& startPos,
                const GameLogic::Position& goalPos,
                std::vector<GameLogic::Position>* outPath);
    
    /**
     * Atualiza regeneração de recurso
     * @param entityId ID da entidade
     * @param resourceType Tipo de recurso
     * @param currentValue Valor atual
     * @param maxValue Valor máximo
     * @return true se atualizado com sucesso
     */
    bool UpdateResourceRegeneration(uint32_t entityId, GameLogic::ResourceType resourceType,
                                  float currentValue, float maxValue);
    
    /**
     * Define entidade em combate
     * @param entityId ID da entidade
     * @param inCombat Estado de combate
     * @return true se alterado com sucesso
     */
    bool SetEntityInCombat(uint32_t entityId, bool inCombat);
    
    /**
     * Atualiza a lógica de jogo
     * @param deltaTimeMs Tempo decorrido em ms
     */
    void Update(int32_t deltaTimeMs);
    
    /**
     * Verifica integridade da lógica de jogo
     * @return true se a lógica estiver íntegra
     */
    bool VerifyGameLogicIntegrity();
    
    /**
     * Obtém estatísticas da lógica de jogo
     * @return String formatada com estatísticas
     */
    std::string GetGameLogicStatistics() const;
    
private:
    // Construtor e destrutor privados (singleton)
    GameLogicIntegration();
    ~GameLogicIntegration();
    
    // Não permitir cópias
    GameLogicIntegration(const GameLogicIntegration&) = delete;
    GameLogicIntegration& operator=(const GameLogicIntegration&) = delete;
    
    // Configuração
    GameLogicIntegrationConfig m_config;
    
    // Componentes de lógica de jogo
    GameLogic::DamageCalculator* m_damageCalculator;
    GameLogic::PathfindingManager* m_pathfindingManager;
    GameLogic::RegenerationManager* m_regenerationManager;
    
    // Handlers de eventos
    std::unordered_map<GameLogicEvent, std::vector<GameLogicEventCallback>> m_eventHandlers;
    
    // Estado
    bool m_initialized;
    bool m_storageIntegrated;
    bool m_networkIntegrated;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    mutable std::mutex m_handlersMutex;
    
    // Métodos privados
    void LoadGameLogicConfig();
    bool VerifyComponentIntegrity();
    void NotifyEvent(GameLogicEvent event, uint32_t entityId, const void* data, size_t size);
    GameLogic::CombatContext BuildCombatContext(uint32_t attackerId, uint32_t targetId, uint32_t skillId, GameLogic::DamageType damageType);
    void SyncGameLogicWithStorage();
};

} // namespace Integration
} // namespace WYDBR

#endif // WYDBR_GAME_LOGIC_INTEGRATION_H
