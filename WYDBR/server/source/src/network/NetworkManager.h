/**
 * NetworkManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/network/NetworkManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _NETWORK_MANAGER_H_
#define _NETWORK_MANAGER_H_

/**
 * @file NetworkManager.h
 * @brief Gerenciador de rede para o servidor WYDBRASIL
 * 
 * Implementação de gerenciamento de conexões de rede compatível com o WYD original,
 * mantendo compatibilidade total com o protocolo original, mas com melhorias de segurança
 * e escalabilidade. Gerencia múltiplas conexões usando uma arquitetura orientada a eventos.
 */

#include <memory>
#include <unordered_map>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <string>

#include "../../include/CPSock.h"
#include "../../include/Basedef.h"

namespace wydbr {


// Códigos de comando do protocolo WYD original
enum class PacketCommand : WORD {
    // Comandos de autenticação
    PKT_INIT = 0x0001,               // Inicialização de conexão
    PKT_KEEPALIVE = 0x0002,          // Keep-alive para manter conexão
    PKT_LOGOUT = 0x0003,             // Logout do servidor
    PKT_RELOADUSER = 0x0004,         // Recarregar dados do usuário
    
    // Comandos de chat
    PKT_CHAT = 0x0101,               // Mensagem de chat normal
    PKT_WHISPER = 0x0102,            // Mensagem privada (whisper)
    PKT_PARTY = 0x0103,              // Mensagem de grupo
    PKT_GUILD = 0x0104,              // Mensagem de guild
    
    // Comandos de movimentação
    PKT_MOVE = 0x0201,               // Movimento do personagem
    PKT_TELEPORT = 0x0202,           // Teleporte para outra posição
    PKT_TELEPORT_CONFIRM = 0x0203,   // Confirmação de teleporte
    
    // Comandos de combate
    PKT_ATTACK = 0x0301,             // Ataque físico
    PKT_DEFENSE = 0x0302,            // Ação de defesa
    PKT_DAMAGE = 0x0303,             // Dano recebido/causado
    PKT_SPELL = 0x0304,              // Uso de magia
    
    // Comandos de itens/inventário
    PKT_GETITEM = 0x0401,            // Pegar item do chão
    PKT_DROPITEM = 0x0402,           // Dropar item no chão
    PKT_USEITEM = 0x0403,            // Usar um item
    PKT_TRADE = 0x0404,              // Iniciar/continuar troca
    PKT_TRADE_CONFIRM = 0x0405,      // Confirmar troca
    PKT_TRADE_CANCEL = 0x0406,       // Cancelar troca
    PKT_SHOP = 0x0407,               // Interação com loja
    PKT_SKILLLEVEL = 0x0408,         // Atualizar nível de skill
    
    // Comandos de sistema
    PKT_SAVEPOSITION = 0x0501,       // Salvar posição do personagem
    PKT_REVIVAL = 0x0502,            // Reviver após morte
    PKT_ACCOUNT_LOGIN = 0x0503,      // Login com conta
    PKT_CHARACTER_LOGIN = 0x0504,    // Seleção de personagem
    PKT_CHARACTER_CREATE = 0x0505,   // Criação de personagem
    PKT_CHARACTER_DELETE = 0x0506,   // Exclusão de personagem
    
    // Comandos de guild
    PKT_GUILD_CREATE = 0x0601,       // Criar guild
    PKT_GUILD_DISBAND = 0x0602,      // Dissolver guild
    PKT_GUILD_INVITE = 0x0603,       // Convidar para guild
    PKT_GUILD_ACCEPT = 0x0604,       // Aceitar convite de guild
    PKT_GUILD_REJECT = 0x0605,       // Rejeitar convite de guild
    PKT_GUILD_KICK = 0x0606,         // Expulsar da guild
    PKT_GUILD_MEMBER = 0x0607,       // Informações de membro da guild
    
    // Comandos diversos
    PKT_UPDATEEQUIP = 0x0701,        // Atualizar equipamento
    PKT_QUEST_UPDATE = 0x0702,       // Atualização de quest
    PKT_QUEST_COMPLETE = 0x0703,     // Completar quest
    PKT_PARTY_INVITE = 0x0704,       // Convidar para grupo
    PKT_PARTY_ACCEPT = 0x0705,       // Aceitar convite de grupo
    PKT_PARTY_REJECT = 0x0706,       // Rejeitar convite de grupo
    PKT_PARTY_LEAVE = 0x0707,        // Sair do grupo
    PKT_PARTY_KICK = 0x0708,         // Expulsar do grupo
    
    // Extensões e comandos adicionais
    PKT_ADMIN_COMMAND = 0x0901,      // Comandos administrativos
    PKT_GM_COMMAND = 0x0902,         // Comandos de GM
    PKT_WEATHER_UPDATE = 0x0903,     // Atualização de clima
    PKT_WORLD_EVENT = 0x0904,        // Evento de mundo
    PKT_SYSTEM_MESSAGE = 0x0905      // Mensagem de sistema
};

// Eventos de rede
enum /**
 * Classe WYDNetEvent
 * 
 * Esta classe implementa a funcionalidade WYDNetEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class NetEvent {
    CONNECT,            // Conexão estabelecida
    DISCONNECT,         // Desconexão
    DATA_RECEIVED,      // Dados recebidos
    ERROR,              // Erro de rede
    TIMEOUT,            // Timeout de conexão
    SEND_COMPLETE,      // Envio completo
    ACCEPT_CONNECTION,  // Conexão aceita (servidor)
    CONNECTION_REFUSED  // Conexão recusada (cliente)
};

// Informações de conexão
struct ConnectionInfo {
    std::string remoteIP;    // IP remoto
    int remotePort;          // Porta remota
    std::string localIP;     // IP local
    int localPort;           // Porta local
    uint64_t connectionTime; // Timestamp da conexão
    uint64_t lastActivity;   // Timestamp da última atividade
    int connectionId;        // ID único de conexão
    bool authenticated;      // Conexão autenticada
    int accountId;           // ID da conta associada (se autenticado)
    int characterId;         // ID do personagem associado (se logado)
};

// Callback para eventos de rede
using NetworkEventCallback = std::function<void(NetEvent event, int connectionId, void* data)>;

// Callback para processamento de pacotes
using PacketProcessorCallback = std::function<bool(int connectionId, PacketCommand cmd, const BYTE* data, int size)>;

/**
 * @class NetworkManager
 * @brief Gerenciador central de conexões de rede
 * 
 * Esta classe implementa um gerenciador de conexões de rede para o servidor WYDBRASIL,
 * fornecendo uma interface simples e robusta para manipulação de conexões, envio e
 * recebimento de dados, compatível com o protocolo original do WYD.
 */
/**
 * Classe WYDNetworkManager
 * 
 * Esta classe implementa a funcionalidade WYDNetworkManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class NetworkManager {
public:
    /**
     * @brief Obtém a instância única do NetworkManager (Singleton)
     * @return Referência para a instância única
     */
    static NetworkManager& getInstance();
    
    /**
     * @brief Inicializa o gerenciador de rede
     * @param maxConnections Número máximo de conexões suportadas
     * @param receiveBufferSize Tamanho do buffer de recepção
     * @param sendBufferSize Tamanho do buffer de envio
     * @return true se inicializado com sucesso
     */
    bool initialize(int maxConnections = MAX_CONNECTIONS, 
                    int receiveBufferSize = RECV_BUFFER_SIZE, 
                    int sendBufferSize = SEND_BUFFER_SIZE);
    
    /**
     * @brief Inicia o servidor em uma porta específica
     * @param host Endereço para bind (0.0.0.0 para todas interfaces)
     * @param port Porta para escutar
     * @return true se servidor iniciado com sucesso
     */
    bool startServer(const std::string& host, int port);
    
    /**
     * @brief Conecta a um servidor remoto
     * @param host Endereço do servidor
     * @param port Porta do servidor
     * @param timeout Timeout de conexão (ms)
     * @return ID da conexão ou -1 se falhou
     */
    int connectToServer(const std::string& host, int port, int timeout = CONNECT_TIMEOUT);
    
    /**
     * @brief Fecha uma conexão específica
     * @param connectionId ID da conexão
     * @return true se conexão fechada com sucesso
     */
    bool closeConnection(int connectionId);
    
    /**
     * @brief Fecha todas as conexões e para o servidor
     */
    void shutdown();
    
    /**
     * @brief Envia dados para uma conexão específica
     * @param connectionId ID da conexão
     * @param cmd Comando do pacote
     * @param data Dados a serem enviados
     * @param size Tamanho dos dados
     * @return true se enviado com sucesso
     */
    bool sendPacket(int connectionId, PacketCommand cmd, const BYTE* data, int size);
    
    /**
     * @brief Envia dados para todas as conexões
     * @param cmd Comando do pacote
     * @param data Dados a serem enviados
     * @param size Tamanho dos dados
     * @return true se enviado com sucesso para todas
     */
    bool broadcastPacket(PacketCommand cmd, const BYTE* data, int size);
    
    /**
     * @brief Registra callback para eventos de rede
     * @param callback Função de callback
     */
    void setEventCallback(NetworkEventCallback callback);
    
    /**
     * @brief Registra callback para processamento de pacotes
     * @param callback Função de callback
     */
    void setPacketProcessor(PacketProcessorCallback callback);
    
    /**
     * @brief Obtém estatísticas de rede para uma conexão
     * @param connectionId ID da conexão
     * @param bytesSent Total de bytes enviados (saída)
     * @param bytesReceived Total de bytes recebidos (saída)
     * @param packetsSent Total de pacotes enviados (saída)
     * @param packetsReceived Total de pacotes recebidos (saída)
     * @return true se informações obtidas com sucesso
     */
    bool getConnectionStats(int connectionId, uint64_t& bytesSent, uint64_t& bytesReceived, 
                          uint32_t& packetsSent, uint32_t& packetsReceived);
    
    /**
     * @brief Obtém informações de uma conexão
     * @param connectionId ID da conexão
     * @return Informações da conexão ou nullptr se não existir
     */
    ConnectionInfo* getConnectionInfo(int connectionId);
    
    /**
     * @brief Define status de autenticação de uma conexão
     * @param connectionId ID da conexão
     * @param authenticated Status de autenticação
     * @param accountId ID da conta (se autenticado)
     * @return true se atualizado com sucesso
     */
    bool setAuthenticated(int connectionId, bool authenticated, int accountId = -1);
    
    /**
     * @brief Define ID do personagem para uma conexão
     * @param connectionId ID da conexão
     * @param characterId ID do personagem
     * @return true se atualizado com sucesso
     */
    bool setCharacterId(int connectionId, int characterId);
    
    /**
     * @brief Obtém número de conexões ativas
     * @return Total de conexões ativas
     */
    int getActiveConnectionCount() const;
    
    /**
     * @brief Verifica se uma conexão está ativa
     * @param connectionId ID da conexão
     * @return true se conexão ativa
     */
    bool isConnectionActive(int connectionId) const;

    /**
     * @brief Obtém o endereço IP de uma conexão
     * @param connectionId ID da conexão
     * @return Endereço IP ou string vazia se conexão inválida
     */
    std::string getConnectionIP(int connectionId) const;

    /**
     * @brief Desconecta todas as conexões de um determinado IP
     * @param ip Endereço IP
     * @return Número de conexões fechadas
     */
    int disconnectIP(const std::string& ip);

    /**
     * @brief Obtém o tempo desde a última atividade da conexão
     * @param connectionId ID da conexão
     * @return Tempo em milissegundos desde a última atividade ou -1 se inválido
     */
    int64_t getTimeSinceLastActivity(int connectionId) const;

private:
    // Construtor privado (singleton)
    NetworkManager();
    
    // Destrutor
    ~NetworkManager();
    
    // Impedir cópia e atribuição
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    
    // Thread de aceitação de novas conexões
    /**
 * acceptThread
 * 
 * Implementa a funcionalidade acceptThread conforme especificação original.
 */

    void acceptThread();
    
    // Thread de recebimento de dados
    /**
 * receiveThread
 * 
 * Implementa a funcionalidade receiveThread conforme especificação original.
 */

    void receiveThread();
    
    // Thread de envio de dados
    /**
 * sendThread
 * 
 * Implementa a funcionalidade sendThread conforme especificação original.
 */

    void sendThread();
    
    // Thread de manutenção (timeouts, keepalives)
    /**
 * maintenanceThread
 * 
 * Implementa a funcionalidade maintenanceThread conforme especificação original.
 */

    void maintenanceThread();
    
    // Processa pacotes recebidos
    /**
 * processReceivedData
 * 
 * Implementa a funcionalidade processReceivedData conforme especificação original.
 * @param connectionId Parâmetro connectionId
 * @param data Parâmetro data
 * @param size Parâmetro size
 */

    void processReceivedData(int connectionId, const BYTE* data, int size);
    
    // Gera ID único para conexão
    /**
 * generateConnectionId
 * 
 * Implementa a funcionalidade generateConnectionId conforme especificação original.
 * @return Retorna int
 */

    int generateConnectionId();
    
    // Estrutura para uma conexão
    struct Connection {
        std::shared_ptr<CPSock> socket;      // Socket
        ConnectionInfo info;                 // Informações da conexão
        std::mutex socketMutex;              // Mutex para acesso ao socket
        std::queue<std::vector<BYTE>> sendQueue; // Fila de pacotes a enviar
        std::mutex sendQueueMutex;           // Mutex para acesso à fila de envio
        std::condition_variable sendCondition; // Condição para sinalizar novos dados
        uint64_t bytesSent;                  // Total de bytes enviados
        uint64_t bytesReceived;              // Total de bytes recebidos
        uint32_t packetsSent;                // Total de pacotes enviados
        uint32_t packetsReceived;            // Total de pacotes recebidos
        bool active;                         // Conexão ativa
    };
    
    // Servidor de escuta
    std::shared_ptr<CPSock> m_listenSocket;
    
    // Lista de conexões
    std::unordered_map<int, std::shared_ptr<Connection>> m_connections;
    
    // Mutex para acesso à lista de conexões
    std::mutex m_connectionsMutex;
    
    // Flag de servidor ativo
    std::atomic<bool> m_serverActive;
    
    // Thread de aceitação
    std::thread m_acceptThread;
    
    // Thread de recebimento
    std::thread m_receiveThread;
    
    // Thread de envio
    std::thread m_sendThread;
    
    // Thread de manutenção
    std::thread m_maintenanceThread;
    
    // Contadores
    std::atomic<int> m_nextConnectionId;
    std::atomic<int> m_activeConnections;
    
    // Callbacks
    NetworkEventCallback m_eventCallback;
    PacketProcessorCallback m_packetProcessor;
    
    // Configurações
    int m_maxConnections;
    int m_receiveBufferSize;
    int m_sendBufferSize;
    
    // Inicializado
    bool m_initialized;
};

#endif // _NETWORK_MANAGER_H_

} // namespace wydbr
