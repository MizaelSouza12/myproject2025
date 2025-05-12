/**
 * SocketManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/network/SocketManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <unordered_map>
#include <vector>
#include <mutex>
#include <atomic>
#include <memory>
#include <string>
#include <functional>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #define SOCKET int
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define closesocket close
#endif

#include "../core/WYDTypes.h"
#include "PacketTypes.h"
#include "CryptoManager.h"

namespace wydbr {


/**
 * @file SocketManager.h
 * @brief Gerenciador de sockets para comunicação TCP
 * 
 * Este arquivo contém a definição do gerenciador de sockets TCP usado
 * para comunicação cliente-servidor no WYD. Segue a estrutura original
 * do WYD, mas com melhorias significativas para evitar bugs comuns.
 */

namespace wyd {
namespace network {

// Forward declarations
class CPacketHandler;

/**
 * @brief Informações de um cliente conectado (original: CLIENT_INFO)
 * 
 * Esta estrutura armazena informações sobre um cliente conectado,
 * como seu socket, endereço IP, chave de criptografia, etc.
 */
struct STRUCT_CLIENT_INFO {
    SOCKET socket;                // Socket do cliente
    std::string ipAddress;        // Endereço IP
    WORD port;                    // Porta
    DWORD clientKey;              // Chave do cliente (ID de sessão)
    bool encrypted;               // Se a comunicação é criptografada
    BYTE encryptionKey[32];       // Chave de criptografia
    std::chrono::steady_clock::time_point lastActivity; // Última atividade
    DWORD totalBytesReceived;     // Total de bytes recebidos
    DWORD totalBytesSent;         // Total de bytes enviados
    DWORD packetCount;            // Contador de pacotes processados
    bool authenticated;           // Se o cliente está autenticado
    
    // Buffer de recebimento para mensagens parciais
    std::vector<BYTE> recvBuffer;
    WORD bytesInRecvBuffer;
    
    STRUCT_CLIENT_INFO()
        : socket(INVALID_SOCKET)
        , port(0)
        , clientKey(0)
        , encrypted(false)
        , totalBytesReceived(0)
        , totalBytesSent(0)
        , packetCount(0)
        , authenticated(false)
        , bytesInRecvBuffer(0)
    {
        memset(encryptionKey, 0, sizeof(encryptionKey));
        recvBuffer.resize(8192); // Buffer inicial
    }
};

/**
 * @brief Gerenciador de sockets (original: CNetworkManager)
 * 
 * Esta classe gerencia sockets TCP para comunicação cliente-servidor.
 * No WYD original, existia uma classe similar mas com limitações e bugs.
 */
/**
 * Classe WYDSocketManager
 * 
 * Esta classe implementa a funcionalidade WYDSocketManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CSocketManager {
public:
    /**
     * @brief Construtor
     */
    CSocketManager();
    
    /**
     * @brief Destrutor
     */
    ~CSocketManager();
    
    /**
     * @brief Inicializa o gerenciador de sockets
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador de sockets
     */
    void Shutdown();
    
    /**
     * @brief Inicia a escuta em uma porta específica
     * @param port Porta para escutar
     * @param backlog Tamanho da fila de conexões pendentes
     * @return true se iniciou com sucesso, false caso contrário
     */
    bool StartListen(WORD port, int backlog = 5);
    
    /**
     * @brief Para a escuta
     */
    void StopListen();
    
    /**
     * @brief Aceita uma nova conexão
     * @return Socket do cliente aceito, ou INVALID_SOCKET se nenhum
     */
    SOCKET Accept();
    
    /**
     * @brief Fecha um socket específico
     * @param socket Socket a fechar
     */
    void CloseSocket(SOCKET socket);
    
    /**
     * @brief Fecha todos os sockets
     */
    void CloseAllSockets();
    
    /**
     * @brief Envia dados para um socket
     * @param socket Socket para enviar
     * @param data Dados a enviar
     * @param size Tamanho dos dados
     * @return true se enviou com sucesso, false caso contrário
     */
    bool SendData(SOCKET socket, const BYTE* data, WORD size);
    
    /**
     * @brief Envia um pacote para um socket
     * @param socket Socket para enviar
     * @param packet Pacote a enviar
     * @return true se enviou com sucesso, false caso contrário
     */
    bool SendPacket(SOCKET socket, const Packet& packet);
    
    /**
     * @brief Envia um pacote para vários sockets
     * @param sockets Lista de sockets para enviar
     * @param packet Pacote a enviar
     * @return Número de sockets que receberam o pacote com sucesso
     */
    int SendPacketToMultiple(const std::vector<SOCKET>& sockets, const Packet& packet);
    
    /**
     * @brief Processa dados recebidos de um socket
     * @param socket Socket que recebeu dados
     * @param data Dados recebidos
     * @param size Tamanho dos dados
     * @return true se processou com sucesso, false caso contrário
     */
    bool ProcessReceivedData(SOCKET socket, const BYTE* data, WORD size);
    
    /**
     * @brief Processa um pacote recebido
     * @param socket Socket que recebeu o pacote
     * @param packet Pacote recebido
     * @return true se processou com sucesso, false caso contrário
     */
    bool ProcessPacket(SOCKET socket, const Packet& packet);
    
    /**
     * @brief Verifica se um socket é válido
     * @param socket Socket a verificar
     * @return true se válido, false caso contrário
     */
    bool IsSocketValid(SOCKET socket) const;
    
    /**
     * @brief Obtém informações de um cliente
     * @param socket Socket do cliente
     * @return Ponteiro para as informações do cliente, ou nullptr se não encontrado
     */
    STRUCT_CLIENT_INFO* GetClientInfo(SOCKET socket);
    
    /**
     * @brief Obtém o endereço IP de um socket
     * @param socket Socket
     * @return Endereço IP, ou string vazia se não encontrado
     */
    std::string GetIPAddress(SOCKET socket) const;
    
    /**
     * @brief Define a chave do cliente
     * @param socket Socket do cliente
     * @param clientKey Chave do cliente
     * @return true se definida com sucesso, false caso contrário
     */
    bool SetClientKey(SOCKET socket, DWORD clientKey);
    
    /**
     * @brief Obtém a chave do cliente
     * @param socket Socket do cliente
     * @return Chave do cliente, ou 0 se não encontrado
     */
    DWORD GetClientKey(SOCKET socket) const;
    
    /**
     * @brief Define a chave de criptografia
     * @param socket Socket do cliente
     * @param key Chave de criptografia
     * @param keySize Tamanho da chave
     * @return true se definida com sucesso, false caso contrário
     */
    bool SetEncryptionKey(SOCKET socket, const BYTE* key, BYTE keySize);
    
    /**
     * @brief Habilita ou desabilita criptografia para um socket
     * @param socket Socket do cliente
     * @param enable true para habilitar, false para desabilitar
     * @return true se alterada com sucesso, false caso contrário
     */
    bool EnableEncryption(SOCKET socket, bool enable);
    
    /**
     * @brief Verifica se um socket está usando criptografia
     * @param socket Socket do cliente
     * @return true se usando criptografia, false caso contrário
     */
    bool IsEncrypted(SOCKET socket) const;
    
    /**
     * @brief Define o handler de pacotes
     * @param packetHandler Ponteiro para o handler de pacotes
     */
    void SetPacketHandler(CPacketHandler* packetHandler);
    
    /**
     * @brief Atualiza o timestamp de última atividade para um socket
     * @param socket Socket do cliente
     */
    void UpdateLastActivity(SOCKET socket);
    
    /**
     * @brief Verifica timeouts para todos os sockets conectados
     * @param timeout Tempo limite em milissegundos
     * @return Número de sockets fechados por timeout
     */
    int CheckTimeouts(DWORD timeout);
    
    /**
     * @brief Obtém o número de clientes conectados
     * @return Número de clientes
     */
    size_t GetClientCount() const;
    
    /**
     * @brief Obtém estatísticas de rede
     * @param totalBytesSent Total de bytes enviados (saída)
     * @param totalBytesReceived Total de bytes recebidos (saída)
     * @param totalPacketsSent Total de pacotes enviados (saída)
     * @param totalPacketsReceived Total de pacotes recebidos (saída)
     */
    void GetStats(DWORD& totalBytesSent, DWORD& totalBytesReceived,
                 DWORD& totalPacketsSent, DWORD& totalPacketsReceived) const;
    
private:
    /**
     * @brief Inicializa a biblioteca de sockets (apenas Windows)
     * @return true se inicializada com sucesso, false caso contrário
     */
    bool InitSocketLib();
    
    /**
     * @brief Finaliza a biblioteca de sockets (apenas Windows)
     */
    void CleanupSocketLib();
    
    /**
     * @brief Configura um socket TCP
     * @param socket Socket a configurar
     * @return true se configurado com sucesso, false caso contrário
     */
    bool ConfigureSocket(SOCKET socket);
    
    /**
     * @brief Registra um novo cliente
     * @param socket Socket do cliente
     * @param address Endereço do cliente
     * @return true se registrado com sucesso, false caso contrário
     */
    bool RegisterClient(SOCKET socket, const sockaddr_in& address);
    
    /**
     * @brief Remove um cliente
     * @param socket Socket do cliente
     * @return true se removido com sucesso, false caso contrário
     */
    bool UnregisterClient(SOCKET socket);
    
    /**
     * @brief Criptografa dados para envio
     * @param clientInfo Informações do cliente
     * @param data Dados a criptografar
     * @param size Tamanho dos dados
     * @param outBuffer Buffer de saída
     * @param outSize Tamanho de saída (entrada/saída)
     * @return true se criptografado com sucesso, false caso contrário
     */
    bool EncryptData(const STRUCT_CLIENT_INFO* clientInfo, const BYTE* data, 
                    WORD size, BYTE* outBuffer, WORD& outSize);
    
    /**
     * @brief Descriptografa dados recebidos
     * @param clientInfo Informações do cliente
     * @param data Dados a descriptografar
     * @param size Tamanho dos dados
     * @param outBuffer Buffer de saída
     * @param outSize Tamanho de saída (entrada/saída)
     * @return true se descriptografado com sucesso, false caso contrário
     */
    bool DecryptData(const STRUCT_CLIENT_INFO* clientInfo, const BYTE* data, 
                    WORD size, BYTE* outBuffer, WORD& outSize);
    
    /**
     * @brief Processa dados recebidos parciais
     * @param clientInfo Informações do cliente
     * @return true se processou com sucesso, false caso contrário
     */
    bool ProcessPartialData(STRUCT_CLIENT_INFO* clientInfo);
    
    SOCKET listenSocket_;                    // Socket de escuta
    WORD listenPort_;                        // Porta de escuta
    std::atomic<bool> listening_;            // Flag de escuta ativa
    std::atomic<bool> initialized_;          // Flag de inicialização
    
    std::unordered_map<SOCKET, std::unique_ptr<STRUCT_CLIENT_INFO>> clients_; // Clientes conectados
    mutable std::mutex clientsMutex_;        // Mutex para acesso seguro a clientes_
    
    CPacketHandler* packetHandler_;          // Handler de pacotes
    
    // Estatísticas
    std::atomic<DWORD> totalBytesSent_;      // Total de bytes enviados
    std::atomic<DWORD> totalBytesReceived_;  // Total de bytes recebidos
    std::atomic<DWORD> totalPacketsSent_;    // Total de pacotes enviados
    std::atomic<DWORD> totalPacketsReceived_; // Total de pacotes recebidos
    
    // Buffers temporários para processamento
    std::vector<BYTE> encryptBuffer_;        // Buffer para criptografia
    std::vector<BYTE> decryptBuffer_;        // Buffer para descriptografia
    std::vector<BYTE> packetBuffer_;         // Buffer para pacotes serializados
};

} // namespace network
} // namespace wyd

#endif // SOCKETMANAGER_H

} // namespace wydbr
