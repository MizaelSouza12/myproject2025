/**
 * @file SystemIntegrationManager.h
 * @brief Gerenciador de integração do sistema WYDBR 2.0
 * 
 * Responsável por coordenar a comunicação entre todos os módulos
 * e garantir o funcionamento unificado do sistema.
 */

#ifndef WYDBR_SYSTEM_INTEGRATION_MANAGER_H
#define WYDBR_SYSTEM_INTEGRATION_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

// Inclusões de componentes do sistema
#include "WYDStudio/integration/ServerInterface.h"
#include "WYDStudio/integration/TMSrvIntegration.h"
#include "WYDStudio/integration/DBSrvIntegration.h"
#include "WYDStudio/integration/VoIPIntegration.h"
#include "WYDStudio/integration/ScreenShareIntegration.h"
#include "Security/SecurityManager.h"
#include "Network/NetworkManager.h"
#include "VoIP/VoIPManager.h"
#include "ScreenShare/ScreenShareManager.h"
#include "Storage/database/DatabaseSchema.h"

namespace WYDBR {
namespace Integration {

/**
 * Estado do sistema
 */
enum class SystemState {
    Uninitialized,     // Não inicializado
    Initializing,      // Inicializando
    Running,           // Em execução
    ShuttingDown,      // Desligando
    Stopped,           // Parado
    Error              // Erro
};

/**
 * Tipo de componente do sistema
 */
enum class ComponentType {
    WYDStudio,         // Interface WYDStudio
    TMSrv,             // Servidor TM
    DBSrv,             // Servidor DB
    VoIP,              // Sistema VoIP
    ScreenShare,       // Sistema de Compartilhamento de Tela
    Security,          // Sistema de Segurança
    Network,           // Sistema de Rede
    Storage,           // Sistema de Armazenamento
    GameLogic          // Lógica de Jogo
};

/**
 * Estado de componente
 */
enum class ComponentState {
    Uninitialized,     // Não inicializado
    Initializing,      // Inicializando
    Ready,             // Pronto
    Running,           // Em execução
    Paused,            // Pausado
    Error,             // Erro
    Stopped            // Parado
};

/**
 * Configuração do gerenciador de integração
 */
struct IntegrationConfig {
    std::string configFilePath;    // Caminho para arquivo de configuração
    bool autoStart;                // Iniciar componentes automaticamente
    bool enforceSecurityChecks;    // Aplicar verificações de segurança
    int startupTimeoutMs;          // Timeout de inicialização em ms
    int shutdownTimeoutMs;         // Timeout de desligamento em ms
    std::vector<ComponentType> autoStartComponents; // Componentes para iniciar automaticamente
};

/**
 * Informações de componente
 */
struct ComponentInfo {
    ComponentType type;            // Tipo do componente
    std::string name;              // Nome do componente
    std::string version;           // Versão do componente
    ComponentState state;          // Estado atual
    std::string errorMessage;      // Mensagem de erro (se houver)
    std::string statusDetails;     // Detalhes de status
    uint64_t startTime;            // Timestamp de início
    uint64_t lastUpdateTime;       // Timestamp da última atualização
    void* instance;                // Ponteiro para a instância
};

/**
 * Callback para eventos de componente
 */
using ComponentEventCallback = std::function<void(const ComponentInfo&)>;

/**
 * Gerenciador de integração do sistema
 */
class SystemIntegrationManager {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o gerenciador
     */
    static SystemIntegrationManager& GetInstance();
    
    /**
     * Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const IntegrationConfig& config);
    
    /**
     * Inicia o sistema
     * @return true se iniciado com sucesso
     */
    bool StartSystem();
    
    /**
     * Para o sistema
     * @param force Forçar parada imediata
     * @return true se parado com sucesso
     */
    bool StopSystem(bool force = false);
    
    /**
     * Registra um componente
     * @param type Tipo do componente
     * @param name Nome do componente
     * @param version Versão do componente
     * @param instance Ponteiro para a instância
     * @return true se registrado com sucesso
     */
    bool RegisterComponent(ComponentType type, const std::string& name, 
                         const std::string& version, void* instance);
    
    /**
     * Inicia um componente específico
     * @param type Tipo do componente
     * @return true se iniciado com sucesso
     */
    bool StartComponent(ComponentType type);
    
    /**
     * Para um componente específico
     * @param type Tipo do componente
     * @param force Forçar parada imediata
     * @return true se parado com sucesso
     */
    bool StopComponent(ComponentType type, bool force = false);
    
    /**
     * Pausa um componente
     * @param type Tipo do componente
     * @return true se pausado com sucesso
     */
    bool PauseComponent(ComponentType type);
    
    /**
     * Retoma um componente
     * @param type Tipo do componente
     * @return true se retomado com sucesso
     */
    bool ResumeComponent(ComponentType type);
    
    /**
     * Obtém informações de um componente
     * @param type Tipo do componente
     * @return Informações do componente
     */
    ComponentInfo GetComponentInfo(ComponentType type) const;
    
    /**
     * Obtém o estado atual do sistema
     * @return Estado do sistema
     */
    SystemState GetSystemState() const;
    
    /**
     * Obtém informações de todos os componentes
     * @return Vetor de informações de componentes
     */
    std::vector<ComponentInfo> GetAllComponentsInfo() const;
    
    /**
     * Registra um callback para eventos de componente
     * @param callback Função a ser chamada quando eventos ocorrerem
     */
    void RegisterComponentEventCallback(ComponentEventCallback callback);
    
    /**
     * Obtém estatísticas do sistema
     * @return String formatada com estatísticas
     */
    std::string GetSystemStatistics() const;
    
    /**
     * Executa verificação de integridade do sistema
     * @return true se todos os componentes estiverem íntegros
     */
    bool PerformSystemIntegrityCheck();
    
    /**
     * Obtém o WYDStudio integrado
     * @return Ponteiro para a interface do WYDStudio
     */
    WYDStudio::ServerInterface* GetWYDStudio();
    
    /**
     * Obtém o TMSrv integrado
     * @return Ponteiro para o TMSrv
     */
    WYDStudio::TMSrvIntegration* GetTMSrv();
    
    /**
     * Obtém o DBSrv integrado
     * @return Ponteiro para o DBSrv
     */
    WYDStudio::DBSrvIntegration* GetDBSrv();
    
    /**
     * Obtém o gerenciador VoIP
     * @return Ponteiro para o gerenciador VoIP
     */
    VoIP::VoIPManager* GetVoIPManager();
    
    /**
     * Obtém o gerenciador de compartilhamento de tela
     * @return Ponteiro para o gerenciador de compartilhamento de tela
     */
    ScreenShare::ScreenShareManager* GetScreenShareManager();
    
    /**
     * Obtém o gerenciador de segurança
     * @return Ponteiro para o gerenciador de segurança
     */
    Security::SecurityManager* GetSecurityManager();
    
    /**
     * Obtém o gerenciador de rede
     * @return Ponteiro para o gerenciador de rede
     */
    Network::NetworkManager* GetNetworkManager();
    
private:
    // Construtor e destrutor privados (singleton)
    SystemIntegrationManager();
    ~SystemIntegrationManager();
    
    // Não permitir cópias
    SystemIntegrationManager(const SystemIntegrationManager&) = delete;
    SystemIntegrationManager& operator=(const SystemIntegrationManager&) = delete;
    
    // Estado do sistema
    std::atomic<SystemState> m_systemState;
    
    // Componentes registrados
    std::unordered_map<ComponentType, ComponentInfo> m_components;
    
    // Configuração
    IntegrationConfig m_config;
    
    // Threads de monitoramento
    std::thread m_monitorThread;
    std::atomic<bool> m_monitorRunning;
    
    // Mutex para thread-safety
    mutable std::mutex m_componentsMutex;
    mutable std::mutex m_callbacksMutex;
    
    // Callbacks
    std::vector<ComponentEventCallback> m_callbacks;
    
    // Métodos privados
    void MonitorThreadFunc();
    void NotifyComponentEvent(const ComponentInfo& info);
    bool InternalStopComponent(ComponentType type, bool force);
    bool ValidateComponentDependencies(ComponentType type);
    ComponentInfo* GetComponentInfoPtr(ComponentType type);
    void UpdateComponentState(ComponentType type, ComponentState state, const std::string& details = "");
    uint64_t GetCurrentTimeMs() const;
};

} // namespace Integration
} // namespace WYDBR

#endif // WYDBR_SYSTEM_INTEGRATION_MANAGER_H
