/**
 * @file TMSrvIntegration.h
 * @brief Integração do TMSrv no WYDStudio
 */

#ifndef WYDBR_TMSRV_INTEGRATION_H
#define WYDBR_TMSRV_INTEGRATION_H

#include "ServerInterface.h"
#include "../../TMSrv/TMSrv.h"
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <functional>

namespace WYDBR {

/**
 * Implementação da interface de servidor para TMSrv
 */
class TMSrvIntegration : public ServerInterface {
public:
    /**
     * Construtor
     */
    TMSrvIntegration();
    
    /**
     * Destrutor
     */
    ~TMSrvIntegration() override;
    
    /**
     * Inicializa o TMSrv
     * @return true se inicializado com sucesso
     */
    bool Initialize() override;
    
    /**
     * Inicia o TMSrv
     * @return true se iniciado com sucesso
     */
    bool Start() override;
    
    /**
     * Para o TMSrv
     */
    void Stop() override;
    
    /**
     * Reinicia o TMSrv
     * @return true se reiniciado com sucesso
     */
    bool Restart() override;
    
    /**
     * Obtém o status atual do TMSrv
     * @return Status do servidor
     */
    ServerStatus GetStatus() const override;
    
    /**
     * Obtém o tipo do servidor
     * @return Tipo do servidor (sempre TMSRV)
     */
    ServerType GetType() const override;
    
    /**
     * Obtém o nome do servidor
     * @return Nome do servidor
     */
    std::string GetName() const override;
    
    /**
     * Obtém estatísticas do TMSrv
     * @return JSON com estatísticas
     */
    std::string GetStats() const override;
    
    /**
     * Executa um comando administrativo no TMSrv
     * @param command Comando a ser executado
     * @return Resultado do comando
     */
    std::string ExecuteCommand(const std::string& command) override;
    
    /**
     * Registra um callback para quando o status do TMSrv mudar
     * @param callback Função a ser chamada quando o status mudar
     */
    void RegisterStatusCallback(std::function<void(ServerStatus)> callback) override;
    
    /**
     * Obtém uma lista de clientes conectados ao TMSrv
     * @return Lista de informações de clientes
     */
    std::vector<ClientInfo> GetConnectedClients() const override;
    
    /**
     * Obtém o objeto TMSrv interno
     * @return Ponteiro para o TMSrv
     */
    TMSrv* GetTMSrv() const;
    
private:
    // Instância do TMSrv
    std::unique_ptr<TMSrv> m_tmsrv;
    
    // Status atual
    mutable std::mutex m_statusMutex;
    ServerStatus m_status;
    
    // Thread para monitorar o TMSrv
    std::thread m_monitorThread;
    std::atomic<bool> m_stopMonitoring;
    
    // Callbacks de status
    mutable std::mutex m_callbacksMutex;
    std::vector<std::function<void(ServerStatus)>> m_statusCallbacks;
    
    // Atualiza o status e notifica callbacks
    void UpdateStatus(ServerStatus newStatus);
    
    // Thread de monitoramento
    void MonitorThread();
};

} // namespace WYDBR

#endif // WYDBR_TMSRV_INTEGRATION_H
