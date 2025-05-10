/**
 * @file DBSrvIntegration.h
 * @brief Integração do DBSrv no WYDStudio
 */

#ifndef WYDBR_DBSRV_INTEGRATION_H
#define WYDBR_DBSRV_INTEGRATION_H

#include "ServerInterface.h"
#include "../../DBSrv/DBSrv.h"
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <functional>

namespace WYDBR {

/**
 * Implementação da interface de servidor para DBSrv
 */
class DBSrvIntegration : public ServerInterface {
public:
    /**
     * Construtor
     */
    DBSrvIntegration();
    
    /**
     * Destrutor
     */
    ~DBSrvIntegration() override;
    
    /**
     * Inicializa o DBSrv
     * @return true se inicializado com sucesso
     */
    bool Initialize() override;
    
    /**
     * Inicia o DBSrv
     * @return true se iniciado com sucesso
     */
    bool Start() override;
    
    /**
     * Para o DBSrv
     */
    void Stop() override;
    
    /**
     * Reinicia o DBSrv
     * @return true se reiniciado com sucesso
     */
    bool Restart() override;
    
    /**
     * Obtém o status atual do DBSrv
     * @return Status do servidor
     */
    ServerStatus GetStatus() const override;
    
    /**
     * Obtém o tipo do servidor
     * @return Tipo do servidor (sempre DBSRV)
     */
    ServerType GetType() const override;
    
    /**
     * Obtém o nome do servidor
     * @return Nome do servidor
     */
    std::string GetName() const override;
    
    /**
     * Obtém estatísticas do DBSrv
     * @return JSON com estatísticas
     */
    std::string GetStats() const override;
    
    /**
     * Executa um comando administrativo no DBSrv
     * @param command Comando a ser executado
     * @return Resultado do comando
     */
    std::string ExecuteCommand(const std::string& command) override;
    
    /**
     * Registra um callback para quando o status do DBSrv mudar
     * @param callback Função a ser chamada quando o status mudar
     */
    void RegisterStatusCallback(std::function<void(ServerStatus)> callback) override;
    
    /**
     * Obtém uma lista de clientes conectados
     * @return Lista vazia (DBSrv não tem clientes diretos)
     */
    std::vector<ClientInfo> GetConnectedClients() const override;
    
    /**
     * Obtém o objeto DBSrv interno
     * @return Ponteiro para o DBSrv
     */
    DBSrv* GetDBSrv() const;
    
    /**
     * Executa uma consulta SQL direta (uso administrativo apenas)
     * @param query Consulta SQL a ser executada
     * @param params Parâmetros para a consulta (prevenção de SQL injection)
     * @return Resultado da consulta em formato JSON
     */
    std::string ExecuteQuery(const std::string& query, const std::vector<std::string>& params = {});
    
private:
    // Instância do DBSrv
    std::unique_ptr<DBSrv> m_dbsrv;
    
    // Status atual
    mutable std::mutex m_statusMutex;
    ServerStatus m_status;
    
    // Thread para monitorar o DBSrv
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

#endif // WYDBR_DBSRV_INTEGRATION_H
