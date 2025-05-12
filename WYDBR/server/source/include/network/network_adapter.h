/**
 * network_adapter.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/network/network_adapter.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYDBR_NETWORK_ADAPTER_H
#define WYDBR_NETWORK_ADAPTER_H

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <netinet/in.h>
#include <atomic>
#include <mutex>
#include <queue>
#include <chrono>

#include "../Basedef.h"
#include "../CPSock.h"
#include "../core/event_bus.h"

namespace wydbr {
namespace network {

// Forward declarations
class NetworkAdapterImpl;

// Definições para status de conexão
enum /**
 * Classe WYDConnectionStatus
 * 
 * Esta classe implementa a funcionalidade WYDConnectionStatus conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ConnectionStatus {
    DISCONNECTED = 0,       // Desconectado
    CONNECTING = 1,         // Tentando conectar
    CONNECTED = 2,          // Conectado
    AUTHENTICATING = 3,     // Autenticando
    AUTHENTICATED = 4,      // Autenticado
    ERROR = 5               // Erro de conexão
};

// Estrutura de estatísticas de rede
struct NetworkStats {
    uint64_t packetsSent;          // Total de pacotes enviados
    uint64_t packetsReceived;      // Total de pacotes recebidos
    uint64_t bytesSent;            // Total de bytes enviados
    uint64_t bytesReceived;        // Total de bytes recebidos
    uint64_t packetsDropped;       // Pacotes descartados
    uint64_t connectionAttempts;   // Tentativas de conexão
    uint64_t successfulConnections; // Conexões bem-sucedidas
    uint64_t failedConnections;    // Conexões falhas
    uint64_t avgPing;              // Ping médio (ms)
    uint64_t maxPing;              // Ping máximo registrado (ms)
    uint64_t minPing;              // Ping mínimo registrado (ms)
    uint64_t totalErrors;          // Total de erros de rede
    uint32_t currentConnections;   // Conexões atuais
    uint32_t peakConnections;      // Pico de conexões simultâneas
    
    // Construtor com valores iniciais
    NetworkStats() :
        packetsSent(0), packetsReceived(0),
        bytesSent(0), bytesReceived(0),
        packetsDropped(0), connectionAttempts(0),
        successfulConnections(0), failedConnections(0),
        avgPing(0), maxPing(0), minPing(9999),
        totalErrors(0), currentConnections(0),
        peakConnections(0) {}
};

// Estrutura para configuração do adaptador de rede
struct NetworkAdapterConfig {
    std::string bindAddress;        // Endereço para bind (servidor)
    uint16_t bindPort;              // Porta para bind (servidor)
    std::string connectAddress;     // Endereço para conectar (cliente)
    uint16_t connectPort;           // Porta para conectar (cliente)
    bool isServer;                  // True se for servidor, false se for cliente
    bool useEncryption;             // Usar criptografia?
    bool useCompression;            // Usar compressão?
    uint32_t recvBufferSize;        // Tamanho do buffer de recepção
    uint32_t sendBufferSize;        // Tamanho do buffer de envio
    uint32_t maxConnections;        // Máximo de conexões (servidor)
    uint32_t connectionTimeout;     // Timeout para conexões (ms)
    uint32_t reconnectInterval;     // Intervalo para reconexão (ms)
    uint32_t heartbeatInterval;     // Intervalo de heartbeat (ms)
    bool tcpNoDelay;                // Ativar TCP_NODELAY?
    bool reuseAddress;              // Ativar SO_REUSEADDR?
    uint32_t backlogSize;           // Tamanho do backlog para listen()
    
    // Construtor com valores padrão
    NetworkAdapterConfig() :
        bindAddress("0.0.0.0"), bindPort(0),
        connectAddress(""), connectPort(0),
        isServer(true), useEncryption(true),
        useCompression(true), recvBufferSize(8192),
        sendBufferSize(8192), maxConnections(1000),
        connectionTimeout(30000), reconnectInterval(5000),
        heartbeatInterval(15000), tcpNoDelay(true),
        reuseAddress(true), backlogSize(50) {}
};

// Callbacks para eventos de rede
using ConnectCallback = std::function<void(int connectionId, const std::string& address, uint16_t port)>;
using DisconnectCallback = std::function<void(int connectionId, const std::string& reason)>;
using DataReceivedCallback = std::function<void(int connectionId, const std::vector<BYTE>& data)>;
using ErrorCallback = std::function<void(int connectionId, const std::string& error, int errorCode)>;
using AcceptCallback = std::function<void(int connectionId, const std::string& address, uint16_t port)>;
using StatusCallback = std::function<void(int connectionId, ConnectionStatus status)>;

/**
 * @class NetworkAdapter
 * @brief Camada de abstração para comunicação de rede no WYDBR Server
 * 
 * Esta classe fornece uma interface unificada para comunicação de rede,
 * abstraindo os detalhes da implementação do socket e oferecendo recursos
 * avançados como reconexão automática, criptografia, compressão e gerenciamento
 * de sessões.
 */
/**
 * Classe WYDNetworkAdapter
 * 
 * Esta classe implementa a funcionalidade WYDNetworkAdapter conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class NetworkAdapter {
public:
    /**
     * @brief Construtor
     * @param name Nome identificador para este adaptador
     * @param config Configuração do adaptador de rede
     */
    NetworkAdapter(const std::string& name, const NetworkAdapterConfig& config);
    
    /**
     * @brief Destrutor
     */
    ~NetworkAdapter();
    
    /**
     * @brief Inicializa o adaptador de rede
     * @return true se inicializado com sucesso
     */
    bool init();
    
    /**
     * @brief Para o adaptador de rede e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Inicia o servidor para aceitar conexões (modo servidor)
     * @return true se iniciado com sucesso
     */
    bool startServer();
    
    /**
     * @brief Conecta a um servidor remoto (modo cliente)
     * @param address Endereço do servidor
     * @param port Porta do servidor
     * @return ID da conexão se bem-sucedido, -1 em caso de erro
     */
    int connect(const std::string& address, uint16_t port);
    
    /**
     * @brief Desconecta uma conexão específica
     * @param connectionId ID da conexão
     * @param reason Motivo da desconexão
     */
    void disconnect(int connectionId, const std::string& reason = "Requested by application");
    
    /**
     * @brief Desconecta todas as conexões
     * @param reason Motivo da desconexão
     */
    void disconnectAll(const std::string& reason = "Requested by application");
    
    /**
     * @brief Envia dados para uma conexão específica
     * @param connectionId ID da conexão
     * @param data Dados a serem enviados
     * @param urgent Marcar como urgente (prioridade alta)
     * @return true se os dados foram enfileirados para envio com sucesso
     */
    bool send(int connectionId, const std::vector<BYTE>& data, bool urgent = false);
    
    /**
     * @brief Envia dados para uma conexão específica (versão com tipo de mensagem)
     * @param connectionId ID da conexão
     * @param messageType Tipo da mensagem (utilizado para routing)
     * @param data Dados a serem enviados
     * @param urgent Marcar como urgente (prioridade alta)
     * @return true se os dados foram enfileirados para envio com sucesso
     */
    bool sendTyped(int connectionId, uint16_t messageType, const std::vector<BYTE>& data, bool urgent = false);
    
    /**
     * @brief Envia dados para todas as conexões
     * @param data Dados a serem enviados
     * @param urgent Marcar como urgente (prioridade alta)
     * @return Número de conexões para as quais o envio foi bem-sucedido
     */
    int broadcast(const std::vector<BYTE>& data, bool urgent = false);
    
    /**
     * @brief Atualiza o adaptador de rede (processa eventos, gerencia conexões)
     * @param maxEvents Número máximo de eventos a processar (0 = todos)
     * @return Número de eventos processados
     */
    int update(int maxEvents = 0);
    
    /**
     * @brief Obtém estatísticas de rede
     * @return Estatísticas atuais
     */
    const NetworkStats& getStats() const;
    
    /**
     * @brief Obtém o status atual de uma conexão específica
     * @param connectionId ID da conexão
     * @return Status da conexão
     */
    ConnectionStatus getConnectionStatus(int connectionId) const;
    
    /**
     * @brief Obtém o último erro de uma conexão específica
     * @param connectionId ID da conexão
     * @return Mensagem de erro
     */
    std::string getLastError(int connectionId) const;
    
    /**
     * @brief Obtém o número de conexões ativas
     * @return Número de conexões
     */
    int getConnectionCount() const;
    
    /**
     * @brief Obtém a latência (ping) de uma conexão específica
     * @param connectionId ID da conexão
     * @return Latência em milissegundos (-1 se desconhecida)
     */
    int getLatency(int connectionId) const;
    
    /**
     * @brief Obtém o endereço remoto de uma conexão específica
     * @param connectionId ID da conexão
     * @return Endereço remoto
     */
    std::string getRemoteAddress(int connectionId) const;
    
    /**
     * @brief Obtém a porta remota de uma conexão específica
     * @param connectionId ID da conexão
     * @return Porta remota
     */
    uint16_t getRemotePort(int connectionId) const;
    
    /**
     * @brief Define a chave de criptografia para uma conexão específica
     * @param connectionId ID da conexão
     * @param key Chave de criptografia
     * @param keySize Tamanho da chave
     */
    void setEncryptionKey(int connectionId, const BYTE* key, int keySize);
    
    /**
     * @brief Força uma reconexão para uma conexão específica (modo cliente)
     * @param connectionId ID da conexão
     * @return true se a reconexão foi iniciada com sucesso
     */
    bool reconnect(int connectionId);
    
    /**
     * @brief Envia um heartbeat para uma conexão específica
     * @param connectionId ID da conexão
     * @return true se o heartbeat foi enviado com sucesso
     */
    bool sendHeartbeat(int connectionId);
    
    /**
     * @brief Reconfigura o adaptador de rede
     * @param config Nova configuração
     * @return true se a reconfiguração foi bem-sucedida
     */
    bool reconfigure(const NetworkAdapterConfig& config);
    
    /**
     * @brief Define callback para eventos de conexão
     * @param callback Função de callback
     */
    void setConnectCallback(const ConnectCallback& callback);
    
    /**
     * @brief Define callback para eventos de desconexão
     * @param callback Função de callback
     */
    void setDisconnectCallback(const DisconnectCallback& callback);
    
    /**
     * @brief Define callback para eventos de recepção de dados
     * @param callback Função de callback
     */
    void setDataReceivedCallback(const DataReceivedCallback& callback);
    
    /**
     * @brief Define callback para eventos de erro
     * @param callback Função de callback
     */
    void setErrorCallback(const ErrorCallback& callback);
    
    /**
     * @brief Define callback para eventos de aceitação de conexão (servidor)
     * @param callback Função de callback
     */
    void setAcceptCallback(const AcceptCallback& callback);
    
    /**
     * @brief Define callback para eventos de mudança de status
     * @param callback Função de callback
     */
    void setStatusCallback(const StatusCallback& callback);
    
    /**
     * @brief Obtém o nome deste adaptador
     * @return Nome do adaptador
     */
    const std::string& getName() const;
    
    /**
     * @brief Verifica se o adaptador está inicializado
     * @return true se inicializado
     */
    bool isInitialized() const;
    
    /**
     * @brief Verifica se o adaptador está em modo servidor
     * @return true se for servidor
     */
    bool isServer() const;
    
    /**
     * @brief Obtém o endereço de bind do servidor
     * @return Endereço de bind
     */
    const std::string& getBindAddress() const;
    
    /**
     * @brief Obtém a porta de bind do servidor
     * @return Porta de bind
     */
    uint16_t getBindPort() const;
    
    /**
     * @brief Verifica se o adaptador está tentando uma reconexão automática
     * @param connectionId ID da conexão
     * @return true se estiver tentando reconectar
     */
    bool isReconnecting(int connectionId) const;
    
    /**
     * @brief Obtém o número de tentativas de reconexão para uma conexão específica
     * @param connectionId ID da conexão
     * @return Número de tentativas de reconexão
     */
    int getReconnectAttempts(int connectionId) const;
    
    /**
     * @brief Obtém a configuração atual do adaptador
     * @return Configuração atual
     */
    const NetworkAdapterConfig& getConfig() const;
    
    /**
     * @brief Ativa o modo de debug (logs detalhados)
     * @param enable true para ativar, false para desativar
     */
    void setDebugMode(bool enable);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se o modo de debug estiver ativado
     */
    bool isDebugMode() const;
    
    /**
     * @brief Limpa as estatísticas de rede
     */
    void clearStats();

private:
    // Implementação (usando pimpl idiom)
    std::unique_ptr<NetworkAdapterImpl> impl_;
};

/**
 * @brief Converte um status de conexão para string
 * @param status Status de conexão
 * @return String representando o status
 */
std::string connectionStatusToString(ConnectionStatus status);

} // namespace network
} // namespace wydbr

#endif // WYDBR_NETWORK_ADAPTER_H