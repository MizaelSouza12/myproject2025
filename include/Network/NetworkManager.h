/**
 * @file NetworkManager.h
 * @brief Gerenciador de rede seguro para WYDBR 2.0
 * 
 * Este arquivo define a classe NetworkManager, responsável por gerenciar
 * conexões de rede com segurança aprimorada e detecção de ataques.
 */

#ifndef WYDBR_NETWORK_MANAGER_H
#define WYDBR_NETWORK_MANAGER_H

#include <cstdint>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include "../../headers/packet_struct.h"

// Definições para cross-platform
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET socket_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    #include <fcntl.h>
    typedef int socket_t;
#endif

namespace WYDBR {
namespace Network {

// Estado de conexão
enum class ConnectionState : uint8_t {
    Disconnected = 0,
    Connected = 1,
    Authenticating = 2,
    Authenticated = 3
};

// Tamanho máximo de pacote
constexpr int MAX_PACKET_SIZE = 8192;

// Informações de conexão
struct ConnectionInfo {
    socket_t socket = -1;                  // Socket da conexão
    ConnectionState state = ConnectionState::Disconnected;  // Estado atual
    std::chrono::steady_clock::time_point last_activity;   // Último timestamp de atividade
    char ip_address[INET_ADDRSTRLEN] = {0}; // Endereço IP
    
    // Buffer de recepção
    uint8_t recv_buffer[MAX_PACKET_SIZE] = {0};
    int recv_buffer_size = 0;
    uint16_t packet_size = 0;
    
    // Criptografia
    bool encryption_enabled = false;
    std::string encryption_key;
    
    // Identificação do jogador (após autenticação)
    uint32_t player_id = 0;
    bool is_admin = false;
};

/**
 * @class NetworkManager
 * @brief Gerencia todas as conexões de rede e processa pacotes
 */
class NetworkManager {
public:
    // Método Singleton
    static NetworkManager& GetInstance();
    
    // Destrutor
    ~NetworkManager();
    
    /**
     * @brief Inicializa o sistema de rede
     * @param port Porta para escutar conexões
     * @param max_connections Número máximo de conexões simultâneas
     * @return true se a inicialização foi bem-sucedida
     */
    bool Initialize(uint16_t port = 8281, int max_connections = 500);
    
    /**
     * @brief Inicia o servidor
     */
    void Start();
    
    /**
     * @brief Encerra o servidor
     */
    void Shutdown();
    
    /**
     * @brief Envia um pacote para um cliente
     * @param conn_index Índice da conexão
     * @param packet_data Dados do pacote
     * @param packet_size Tamanho do pacote
     * @return true se o pacote foi enviado com sucesso
     */
    bool SendPacket(int conn_index, const void* packet_data, uint16_t packet_size);
    
    /**
     * @brief Habilita/desabilita criptografia para uma conexão
     * @param conn_index Índice da conexão
     * @param enable true para habilitar, false para desabilitar
     */
    void EnableEncryption(int conn_index, bool enable);
    
    /**
     * @brief Verifica se uma conexão está ativa
     * @param conn_index Índice da conexão
     * @return true se a conexão está ativa
     */
    bool IsConnectionActive(int conn_index) const;
    
    /**
     * @brief Obtém o endereço IP de um cliente
     * @param conn_index Índice da conexão
     * @return Endereço IP como string
     */
    const std::string GetClientIP(int conn_index) const;
    
private:
    // Construtor privado (singleton)
    NetworkManager();
    
    // Métodos de thread
    void AcceptConnectionsThread();
    void ProcessConnectionsThread();
    
    // Processamento de pacotes
    void ProcessIncomingData(int conn_index);
    void ProcessPacket(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    
    // Gerenciamento de conexão
    void CloseConnection(ConnectionInfo& conn);
    
    // Utilidades
    static void CloseSocket(int socket);
    static bool IsValidOpcode(PacketType opcode);
    
    // Estado interno
    bool is_initialized_;
    socket_t server_socket_;
    int max_connections_;
    uint16_t port_;
    int backlog_;
    bool running_;
    
    // Lista de conexões
    std::vector<ConnectionInfo> connections_;
    
    // Threads
    std::thread accept_thread_;
    std::thread process_thread_;
    
    // Mutex para thread-safety
    mutable std::mutex mutex_;
};

} // namespace Network
} // namespace WYDBR

#endif // WYDBR_NETWORK_MANAGER_H