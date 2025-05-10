/**
 * @file ServerInterface.h
 * @brief Interface para comunicação entre WYDStudio e servidores
 */

#ifndef WYDBR_SERVER_INTERFACE_H
#define WYDBR_SERVER_INTERFACE_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace WYDBR {

/**
 * Enum para tipo de servidor
 */
enum class ServerType {
    TMSRV,
    DBSRV
};

/**
 * Enum para status do servidor
 */
enum class ServerStatus {
    OFFLINE,
    STARTING,
    RUNNING,
    STOPPING,
    ERROR
};

/**
 * Estrutura para informações de cliente conectado
 */
struct ClientInfo {
    uint32_t id;
    std::string accountName;
    std::string characterName;
    std::string ipAddress;
    uint64_t connectTime;
    uint16_t level;
    std::string location;
    
    // Outros campos relevantes
};

/**
 * Interface base para servidores
 */
class ServerInterface {
public:
    virtual ~ServerInterface() = default;
    
    /**
     * Inicializa o servidor
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize() = 0;
    
    /**
     * Inicia o servidor
     * @return true se iniciado com sucesso
     */
    virtual bool Start() = 0;
    
    /**
     * Para o servidor
     */
    virtual void Stop() = 0;
    
    /**
     * Reinicia o servidor
     * @return true se reiniciado com sucesso
     */
    virtual bool Restart() = 0;
    
    /**
     * Obtém o status atual do servidor
     * @return Status do servidor
     */
    virtual ServerStatus GetStatus() const = 0;
    
    /**
     * Obtém o tipo do servidor
     * @return Tipo do servidor
     */
    virtual ServerType GetType() const = 0;
    
    /**
     * Obtém o nome do servidor
     * @return Nome do servidor
     */
    virtual std::string GetName() const = 0;
    
    /**
     * Obtém estatísticas do servidor
     * @return JSON com estatísticas
     */
    virtual std::string GetStats() const = 0;
    
    /**
     * Executa um comando administrativo
     * @param command Comando a ser executado
     * @return Resultado do comando
     */
    virtual std::string ExecuteCommand(const std::string& command) = 0;
    
    /**
     * Registra um callback para quando o status do servidor mudar
     * @param callback Função a ser chamada quando o status mudar
     */
    virtual void RegisterStatusCallback(std::function<void(ServerStatus)> callback) = 0;
    
    /**
     * Obtém uma lista de clientes conectados (apenas para TMSrv)
     * @return Lista de informações de clientes
     */
    virtual std::vector<ClientInfo> GetConnectedClients() const = 0;
};

} // namespace WYDBR

#endif // WYDBR_SERVER_INTERFACE_H
