/**
 * @file system_bridge.h
 * @brief Ponte de integração entre os sistemas avançados e o código existente
 */

#ifndef WYDBR_INTEGRATION_SYSTEM_BRIDGE_H
#define WYDBR_INTEGRATION_SYSTEM_BRIDGE_H

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <functional>

// Incluir os cabeçalhos dos sistemas avançados
#include "refine/refinement-system.h"
#include "sync/predictive-sync.h"
#include "queue/priority_queue.h"
#include "streaming/resource-streamer.h"
#include "memory/memory-allocator.h"
#include "compression/dynamic-compression.h"
#include "validation/validation-pipeline.h"
#include "sharding/shard-manager.h"

namespace wydbr {
namespace integration {

/**
 * @class SystemBridge
 * @brief Ponte para integração dos sistemas avançados com o código existente
 * 
 * Esta classe fornece uma API unificada para interagir com todos os sistemas
 * avançados, facilitando a integração com o código existente do WYDBR.
 */
class SystemBridge {
public:
    /**
     * @brief Obter a instância singleton
     * @return Referência para a instância singleton
     */
    static SystemBridge& getInstance() {
        static SystemBridge instance;
        return instance;
    }
    
    /**
     * @brief Inicializar todos os sistemas
     * @param config Configuração para todos os sistemas
     * @return Verdadeiro se todos os sistemas foram inicializados com sucesso
     */
    bool initializeAllSystems(const std::map<std::string, std::string>& config = {}) {
        bool success = true;
        
        // Inicializar sistema de Refinamento
        m_refineSystem = std::make_unique<refine::Refinementsystem>();
        success &= m_refineSystem->Initialize(config);
        
        // Inicializar sistema de Sincronização Preditiva
        m_syncSystem = std::make_unique<sync::Predictivesync>();
        success &= m_syncSystem->Initialize(config);
        
        // Inicializar sistema de Filas Prioritárias
        m_queueSystem = std::make_unique<queue::Priorityqueue>();
        success &= m_queueSystem->Initialize();
        
        // Inicializar sistema de Streaming
        m_streamingSystem = std::make_unique<streaming::Resourcestreamer>();
        success &= m_streamingSystem->Initialize(config);
        
        // Inicializar sistema de Memória
        m_memorySystem = std::make_unique<memory::Memoryallocator>();
        success &= m_memorySystem->Initialize(config);
        
        // Inicializar sistema de Compressão
        m_compressionSystem = std::make_unique<compression::Dynamiccompression>();
        success &= m_compressionSystem->Initialize(config);
        
        // Inicializar sistema de Validação
        m_validationSystem = std::make_unique<validation::Validationpipeline>();
        success &= m_validationSystem->Initialize(config);
        
        // Inicializar sistema de Sharding
        m_shardingSystem = std::make_unique<sharding::Shardmanager>();
        success &= m_shardingSystem->Initialize(config);
        
        // Registrar callbacks para o código legado
        registerLegacyCallbacks();
        
        return success;
    }
    
    /**
     * @brief Finalizar todos os sistemas
     */
    void shutdownAllSystems() {
        // Finalizar todos os sistemas na ordem inversa da inicialização
        if (m_shardingSystem) m_shardingSystem->Shutdown();
        if (m_validationSystem) m_validationSystem->Shutdown();
        if (m_compressionSystem) m_compressionSystem->Shutdown();
        if (m_memorySystem) m_memorySystem->Shutdown();
        if (m_streamingSystem) m_streamingSystem->Shutdown();
        if (m_queueSystem) m_queueSystem->Shutdown();
        if (m_syncSystem) m_syncSystem->Shutdown();
        if (m_refineSystem) m_refineSystem->Shutdown();
    }
    
    // Métodos para acessar sistemas individuais
    refine::Refinementsystem* getRefineSystem() { return m_refineSystem.get(); }
    sync::Predictivesync* getSyncSystem() { return m_syncSystem.get(); }
    queue::Priorityqueue* getQueueSystem() { return m_queueSystem.get(); }
    streaming::Resourcestreamer* getStreamingSystem() { return m_streamingSystem.get(); }
    memory::Memoryallocator* getMemorySystem() { return m_memorySystem.get(); }
    compression::Dynamiccompression* getCompressionSystem() { return m_compressionSystem.get(); }
    validation::Validationpipeline* getValidationSystem() { return m_validationSystem.get(); }
    sharding::Shardmanager* getShardingSystem() { return m_shardingSystem.get(); }
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    SystemBridge() {}
    
    /**
     * @brief Registra callbacks para integração com código legado
     */
    void registerLegacyCallbacks() {
        // TODO: Implementar os callbacks para cada sistema
    }
    
    // Instâncias dos sistemas
    std::unique_ptr<refine::Refinementsystem> m_refineSystem;
    std::unique_ptr<sync::Predictivesync> m_syncSystem;
    std::unique_ptr<queue::Priorityqueue> m_queueSystem;
    std::unique_ptr<streaming::Resourcestreamer> m_streamingSystem;
    std::unique_ptr<memory::Memoryallocator> m_memorySystem;
    std::unique_ptr<compression::Dynamiccompression> m_compressionSystem;
    std::unique_ptr<validation::Validationpipeline> m_validationSystem;
    std::unique_ptr<sharding::Shardmanager> m_shardingSystem;
};

// Funções de conveniência para inicialização/finalização global
inline bool initializeAllSystems(const std::map<std::string, std::string>& config = {}) {
    return SystemBridge::getInstance().initializeAllSystems(config);
}

inline void shutdownAllSystems() {
    SystemBridge::getInstance().shutdownAllSystems();
}

} // namespace integration
} // namespace wydbr

#endif // WYDBR_INTEGRATION_SYSTEM_BRIDGE_H
