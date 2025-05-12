/**
 * CPSock.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/CPSock.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _CPSOCK_H_
#define _CPSOCK_H_

/**
 * @file CPSock.h
 * @brief Implementação compatível da classe de socket para o WYD
 * 
 * Esta implementação mantém a compatibilidade com o protocolo original
 * do WYD, enquanto adiciona funcionalidades modernas e corrige
 * vulnerabilidades de segurança conhecidas.
 */

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <cstring>
#include <memory>
#include <mutex>
#include <functional>
#include <atomic>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#endif

#include "Basedef.h"

namespace wydbr {


// Constantes originais do WYD (manter compatibilidade)
#define RECV_BUFFER_SIZE 131072                 // Tamanho original do buffer de recepção
#define SEND_BUFFER_SIZE 131072                 // Tamanho original do buffer de envio
#define MAX_KEYWORD_QUEUE 16                    // Tamanho máximo da fila de palavras-chave
#define INIT_CODE 521270033                     // Código de inicialização original

// Constantes adicionais para melhorias
#define PACKET_HEADER_SIZE 12                   // Tamanho do cabeçalho de pacote
#define MAX_PACKET_SIZE 8192                    // Tamanho máximo de pacote suportado
#define MAX_MESSAGE_SIZE 4096                   // Tamanho máximo de mensagem
#define MAX_CONNECTIONS 2048                    // Máximo de conexões simultâneas
#define CONNECT_TIMEOUT 5000                    // Timeout para tentativas de conexão (ms)
#define KEEPALIVE_TIME 30000                    // Tempo para testar conexões ativas (ms)
#define MAX_PENDING_CONNECTS 64                 // Máximo de conexões pendentes
#define IDLE_TIMEOUT 300000                     // Tempo para timeout por inatividade (5 min)
#define PING_INTERVAL 60000                     // Intervalo para envio de ping (1 min)
#define MAX_RECONNECT_ATTEMPTS 5                // Máximo de tentativas de reconexão

// Códigos de retorno
enum SocketError {
    SOCKET_ERROR_NONE = 0,                      // Sem erro
    SOCKET_ERROR_CONNECT = 1,                   // Erro de conexão
    SOCKET_ERROR_BIND = 2,                      // Erro de bind
    SOCKET_ERROR_LISTEN = 3,                    // Erro de listen
    SOCKET_ERROR_ACCEPT = 4,                    // Erro de accept
    SOCKET_ERROR_RECEIVE = 5,                   // Erro de recepção
    SOCKET_ERROR_SEND = 6,                      // Erro de envio
    SOCKET_ERROR_CREATION = 7,                  // Erro de criação de socket
    SOCKET_ERROR_INVALIDSTATE = 8,              // Estado inválido
    SOCKET_ERROR_PARAMETER = 9,                 // Parâmetro inválido
    SOCKET_ERROR_TIMEOUT = 10,                  // Timeout
    SOCKET_ERROR_UNSPECIFIED = 11,              // Erro não especificado
    SOCKET_ERROR_NOTINITIALIZED = 12,           // Socket não inicializado
    SOCKET_ERROR_ADDRNOTAVAIL = 13,             // Endereço não disponível
    SOCKET_ERROR_WOULDBLOCK = 14,               // Operação bloquearia
    SOCKET_ERROR_BUFFER_FULL = 15,              // Buffer cheio
    SOCKET_ERROR_PACKET_TOO_LARGE = 16,         // Pacote muito grande
    SOCKET_ERROR_CORRUPT_PACKET = 17,           // Pacote corrompido
    SOCKET_ERROR_INVALID_PACKET = 18,           // Pacote inválido
    SOCKET_ERROR_AUTHENTICATION = 19,           // Erro de autenticação
    SOCKET_ERROR_ENCRYPTION = 20,               // Erro de criptografia
    SOCKET_ERROR_CONNECTION_CLOSED = 21,        // Conexão fechada pelo peer
    SOCKET_ERROR_RECV_BUFFER_OVERFLOW = 22,     // Buffer de recepção cheio
    SOCKET_ERROR_SEND_BUFFER_OVERFLOW = 23      // Buffer de envio cheio
};

// Estado do socket
enum SocketState {
    SOCKET_STATE_CLOSED = 0,                    // Socket fechado
    SOCKET_STATE_CREATED = 1,                   // Socket criado
    SOCKET_STATE_CONNECTING = 2,                // Socket conectando
    SOCKET_STATE_CONNECTED = 3,                 // Socket conectado
    SOCKET_STATE_LISTENING = 4,                 // Socket escutando
    SOCKET_STATE_CLOSING = 5,                   // Socket fechando
    SOCKET_STATE_ERROR = 6                      // Socket em estado de erro
};

/**
 * @class CPSock
 * @brief Classe de socket compatível com o protocolo do WYD
 * 
 * Esta classe implementa a funcionalidade de socket necessária
 * para comunicação entre cliente e servidor, mantendo compatibilidade
 * com o protocolo original do WYD enquanto adiciona funcionalidades
 * modernas e corrige vulnerabilidades de segurança.
 */
/**
 * Classe WYDPSock
 * 
 * Esta classe implementa a funcionalidade WYDPSock conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CPSock 
{
public:
    /**
     * @brief Construtor padrão
     */
    CPSock();
    
    /**
     * @brief Destrutor
     */
    ~CPSock();

    /**
     * @brief Inicializa o sistema de sockets (WSA no Windows)
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool WSAInitialize();

    /**
     * @brief Inicia um servidor de escuta
     * 
     * Mantida para compatibilidade com a API original do WYD
     * 
     * @param hWnd Handle da janela (usado apenas no Windows, ignorado em outros sistemas)
     * @param ip Endereço IP a utilizar
     * @param port Porta a utilizar
     * @param WSA Utilizar WSA (somente Windows)
     * @return Descritor do socket ou 0 em caso de erro
     */
    unsigned /**
 * StartListen
 * 
 * Implementa a funcionalidade StartListen conforme especificação original.
 * @param hWnd Parâmetro hWnd
 * @param ip Parâmetro ip
 * @param port Parâmetro port
 * @param WSA Parâmetro WSA
 * @return Retorna int
 */
 int StartListen(void* hWnd, int ip, int port, int WSA);

    /**
     * @brief Conecta a um servidor
     * 
     * Mantida para compatibilidade com a API original do WYD
     * 
     * @param HostAddr Endereço do host
     * @param Port Porta a ser utilizada
     * @param ip IP a ser utilizado (para bind do socket local)
     * @param WSA Utilizar WSA (somente Windows)
     * @return Descritor do socket ou 0 em caso de erro
     */
    unsigned /**
 * ConnectServer
 * 
 * Implementa a funcionalidade ConnectServer conforme especificação original.
 * @param HostAddr Parâmetro HostAddr
 * @param Port Parâmetro Port
 * @param ip Parâmetro ip
 * @param WSA Parâmetro WSA
 * @return Retorna int
 */
 int ConnectServer(char* HostAddr, int Port, int ip, int WSA);

    /**
     * @brief Estabelece uma conexão única 
     * 
     * Mantida para compatibilidade com a API original do WYD
     * 
     * @param HostAddr Endereço do host
     * @param Port Porta a ser utilizada
     * @param ip IP a ser utilizado (para bind do socket local)
     * @param WSA Utilizar WSA (somente Windows)
     * @return Descritor do socket ou 0 em caso de erro
     */
    unsigned /**
 * SingleConnect
 * 
 * Implementa a funcionalidade SingleConnect conforme especificação original.
 * @param HostAddr Parâmetro HostAddr
 * @param Port Parâmetro Port
 * @param ip Parâmetro ip
 * @param WSA Parâmetro WSA
 * @return Retorna int
 */
 int SingleConnect(char* HostAddr, int Port, int ip, int WSA);

    /**
     * @brief Recebe dados do socket
     * @return Número de bytes recebidos ou -1 em caso de erro
     */
    int Receive();

    /**
     * @brief Lê uma mensagem do buffer de recepção
     * @param ErrorCode Código de erro (saída)
     * @param ErrorType Tipo de erro (saída)
     * @return Ponteiro para a mensagem ou nullptr em caso de erro
     */
    char* ReadMessage(int* ErrorCode, int* ErrorType);

    /**
     * @brief Fecha o socket
     * @return 0 se fechado com sucesso, código de erro caso contrário
     */
    int CloseSocket();

    /**
     * @brief Adiciona uma mensagem à fila de envio
     * @param pMsg Ponteiro para a mensagem
     * @param Size Tamanho da mensagem
     * @return Número de bytes adicionados ou -1 em caso de erro
     */
    int AddMessage(char* pMsg, int Size);

    /**
     * @brief Adiciona uma mensagem à fila de envio com palavra-chave fixa
     * @param pMsg Ponteiro para a mensagem
     * @param Size Tamanho da mensagem
     * @param FixedKeyWord Palavra-chave fixa para criptografia
     * @return Número de bytes adicionados ou -1 em caso de erro
     */
    int AddMessage(char* pMsg, int Size, int FixedKeyWord);

    /**
     * @brief Envia todas as mensagens na fila de envio
     * @return true se todas as mensagens foram enviadas, false caso contrário
     */
    bool SendMessageA();

    /**
     * @brief Envia uma única mensagem diretamente
     * @param Msg Mensagem a ser enviada
     * @param Size Tamanho da mensagem
     * @return Número de bytes enviados ou -1 em caso de erro
     */
    int SendOneMessage(char* Msg, int Size);

    /**
     * @brief Envia uma única mensagem com palavra-chave
     * @param Msg Mensagem a ser enviada
     * @param Size Tamanho da mensagem
     * @param Keyword Palavra-chave para criptografia
     * @return Número de bytes enviados ou -1 em caso de erro
     */
    int SendOneMessageKeyword(char* Msg, int Size, int Keyword);

    /**
     * @brief Adiciona uma mensagem à fila de envio alternativa
     * @param pMsg Ponteiro para a mensagem
     * @param Size Tamanho da mensagem
     * @return Número de bytes adicionados ou -1 em caso de erro
     */
    int AddMessage2(char* pMsg, int Size);

    /**
     * @brief Lê uma mensagem da fila de recepção alternativa
     * @param ErrorCode Código de erro (saída)
     * @param ErrorType Tipo de erro (saída)
     * @return Ponteiro para a mensagem ou nullptr em caso de erro
     */
    char* ReadMessage2(int* ErrorCode, int* ErrorType);

    /**
     * @brief Limpa o buffer de recepção
     */
    void RefreshRecvBuffer();

    /**
     * @brief Limpa o buffer de envio
     */
    void RefreshSendBuffer();

    /**
     * @brief Inicializa o socket com opções de alto nível
     * @param receiveBufferSize Tamanho do buffer de recepção
     * @param sendBufferSize Tamanho do buffer de envio
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool Initialize(int receiveBufferSize = RECV_BUFFER_SIZE, int sendBufferSize = SEND_BUFFER_SIZE);

    /**
     * @brief Inicia um servidor de escuta em formato C++ moderno
     * @param host Endereço do host (IP ou nome)
     * @param port Porta a ser utilizada
     * @param backlog Tamanho da fila de conexões pendentes
     * @return Descritor do socket ou -1 em caso de erro
     */
    int Listen(const std::string& host, int port, int backlog = MAX_PENDING_CONNECTS);

    /**
     * @brief Conecta a um servidor remoto em formato C++ moderno
     * @param host Endereço do host (IP ou nome)
     * @param port Porta a ser utilizada
     * @param timeout Timeout para conexão (em milissegundos)
     * @return true se conectado com sucesso, false caso contrário
     */
    bool Connect(const std::string& host, int port, int timeout = CONNECT_TIMEOUT);

    /**
     * @brief Aceita uma conexão entrante
     * @param clientIP Endereço IP do cliente (saída)
     * @param clientPort Porta do cliente (saída)
     * @return Novo objeto CPSock para a conexão aceita ou nullptr em caso de erro
     */
    std::shared_ptr<CPSock> Accept(std::string& clientIP, int& clientPort);

    /**
     * @brief Configura a função de processamento de pacotes
     * @param callback Função de callback
     */
    void SetPacketProcessor(std::function<bool(const char*, int)> callback);

    /**
     * @brief Configura a função de eventos de conexão
     * @param callback Função de callback
     */
    void SetConnectionHandler(std::function<void(bool, const std::string&, int)> callback);

    /**
     * @brief Configura o socket como não bloqueante
     * @param nonBlocking true para não bloqueante, false para bloqueante
     * @return true se configurado com sucesso, false caso contrário
     */
    bool SetNonBlocking(bool nonBlocking);

    /**
     * @brief Configura o intervalo de envio de ping
     * @param interval Intervalo em milissegundos
     */
    void SetPingInterval(int interval);

    /**
     * @brief Configura o timeout por inatividade
     * @param timeout Timeout em milissegundos
     */
    void SetIdleTimeout(int timeout);

    /**
     * @brief Verifica se o socket está conectado
     * @return true se conectado, false caso contrário
     */
    bool IsConnected() const;

    /**
     * @brief Configura o algoritmo de Nagle
     * @param enable true para habilitar, false para desabilitar
     * @return true se configuração bem-sucedida
     */
    bool SetTcpNoDelay(bool enable);

    /**
     * @brief Configura o tamanho do buffer de recepção
     * @param size Tamanho do buffer
     * @return true se configuração bem-sucedida
     */
    bool SetReceiveBufferSize(int size);

    /**
     * @brief Configura o tamanho do buffer de envio
     * @param size Tamanho do buffer
     * @return true se configuração bem-sucedida
     */
    bool SetSendBufferSize(int size);

    /**
     * @brief Configura o timeout de recepção
     * @param timeout Timeout em milissegundos
     * @return true se configuração bem-sucedida
     */
    bool SetReceiveTimeout(int timeout);

    /**
     * @brief Configura o timeout de envio
     * @param timeout Timeout em milissegundos
     * @return true se configuração bem-sucedida
     */
    bool SetSendTimeout(int timeout);

    /**
     * @brief Habilita o keepalive para o socket
     * @param enable true para habilitar, false para desabilitar
     * @param time Tempo para início do keepalive (segundos)
     * @param interval Intervalo entre tentativas (segundos)
     * @return true se configuração bem-sucedida
     */
    bool SetKeepAlive(bool enable, int time = 60, int interval = 5);

    /**
     * @brief Habilita o reuso de endereços
     * @param enable true para habilitar, false para desabilitar
     * @return true se configuração bem-sucedida
     */
    bool SetReuseAddress(bool enable);

    /**
     * @brief Obtém o endereço IP remoto
     * @param ipBuffer Buffer para o endereço IP
     * @param bufferSize Tamanho do buffer
     * @return true se bem-sucedido
     */
    bool GetPeerAddress(char* ipBuffer, int bufferSize);

    /**
     * @brief Obtém a porta remota
     * @return Número da porta ou -1 se erro
     */
    int GetPeerPort();

    /**
     * @brief Obtém o estado atual do socket
     * @return Estado atual do socket (SocketState)
     */
    SocketState GetState() const;

    /**
     * @brief Obtém o último erro ocorrido
     * @return Código do último erro (SocketError)
     */
    SocketError GetLastError() const;

    /**
     * @brief Obtém a descrição do último erro
     * @return String com a descrição do erro
     */
    std::string GetLastErrorString() const;

    /**
     * @brief Retorna estatísticas de tráfego
     * @param bytesSent Total de bytes enviados (saída)
     * @param bytesReceived Total de bytes recebidos (saída)
     * @param packetsSent Total de pacotes enviados (saída)
     * @param packetsReceived Total de pacotes recebidos (saída)
     * @param packetsDropped Total de pacotes descartados (saída)
     * @param errorCount Total de erros (saída)
     */
    void GetStatistics(uint64_t& bytesSent, uint64_t& bytesReceived, 
                        uint32_t& packetsSent, uint32_t& packetsReceived,
                        uint32_t& packetsDropped, uint32_t& errorCount);

    /**
     * @brief Reduz o uso de memória liberando buffers não utilizados
     */
    void Compact();

    /**
     * @brief Cria um pacote
     * @param type Tipo do pacote
     * @param data Dados do pacote
     * @param dataSize Tamanho dos dados
     * @param output Buffer para o pacote completo
     * @param outputSize Tamanho do buffer de saída
     * @return Tamanho do pacote criado ou -1 se erro
     */
    int CreatePacket(WORD type, const BYTE* data, int dataSize, BYTE* output, int outputSize);

    /**
     * @brief Analisa um pacote
     * @param packet Dados do pacote
     * @param packetSize Tamanho do pacote
     * @param outType Tipo do pacote (saída)
     * @param outData Dados do pacote (saída)
     * @param outDataSize Tamanho dos dados (saída)
     * @return true se análise bem-sucedida
     */
    bool ParsePacket(const BYTE* packet, int packetSize, WORD* outType, BYTE** outData, int* outDataSize);

    // Atributos públicos (mantidos para compatibilidade com código original)
    unsigned int Sock;              // Descritor do socket
    char* pSendBuffer;              // Buffer de envio
    char* pRecvBuffer;              // Buffer de recepção
    int nSendPosition;              // Posição atual no buffer de envio
    int nRecvPosition;              // Posição atual no buffer de recepção
    int nProcPosition;              // Posição de processamento no buffer de recepção
    int nSentPosition;              // Posição já enviada no buffer de envio
    int Init;                       // Código de inicialização

    char SendQueue[MAX_KEYWORD_QUEUE]; // Fila de palavras-chave para envio
    char RecvQueue[MAX_KEYWORD_QUEUE]; // Fila de palavras-chave para recepção

    int SendCount;                  // Contador de pacotes enviados
    int RecvCount;                  // Contador de pacotes recebidos
    int ErrCount;                   // Contador de erros

private:
    SocketState m_state;            // Estado atual do socket
    SocketError m_lastError;        // Último erro ocorrido

    std::vector<char> m_sendBufferInternal;    // Buffer de envio interno
    std::vector<char> m_recvBufferInternal;    // Buffer de recepção interno

    std::mutex m_sendMutex;         // Mutex para acesso ao buffer de envio
    std::mutex m_recvMutex;         // Mutex para acesso ao buffer de recepção

    bool m_nonBlocking;             // Flag para modo não bloqueante
    int m_pingInterval;             // Intervalo de ping (ms)
    int m_idleTimeout;              // Timeout por inatividade (ms)
    uint64_t m_lastActivity;        // Timestamp da última atividade

    std::string m_localIP;          // IP local
    int m_localPort;                // Porta local
    std::string m_remoteIP;         // IP remoto
    int m_remotePort;               // Porta remota

    // Estatísticas de rede
    std::atomic<uint64_t> m_bytesSent;      // Total de bytes enviados
    std::atomic<uint64_t> m_bytesReceived;  // Total de bytes recebidos
    std::atomic<uint32_t> m_packetsSent;    // Total de pacotes enviados
    std::atomic<uint32_t> m_packetsReceived;// Total de pacotes recebidos
    std::atomic<uint32_t> m_packetsDropped; // Total de pacotes descartados
    std::atomic<uint32_t> m_errorCount;     // Total de erros

    // Callbacks
    std::function<bool(const char*, int)> m_packetProcessor;
    std::function<void(bool, const std::string&, int)> m_connectionHandler;

    // Sistemas de segurança
    bool m_useEncryption;           // Usar criptografia?
    std::vector<BYTE> m_encryptionKey; // Chave de criptografia
    int m_compressionLevel;         // Nível de compressão
    bool m_useCompression;          // Usar compressão?

    /**
     * @brief Inicializa o sistema de sockets específico da plataforma
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool InitializePlatform();

    /**
     * @brief Obtém o endereço IP a partir de um nome de host
     * @param hostname Nome do host
     * @return Endereço IP ou vazio em caso de erro
     */
    std::string ResolveHostname(const std::string& hostname);

    /**
     * @brief Configura as opções do socket
     * @return true se configurado com sucesso, false caso contrário
     */
    bool ConfigureSocketOptions();

    /**
     * @brief Atualiza as informações locais do socket
     * @return true se atualizado com sucesso, false caso contrário
     */
    bool UpdateLocalInfo();

    /**
     * @brief Atualiza as informações remotas do socket
     * @return true se atualizado com sucesso, false caso contrário
     */
    bool UpdateRemoteInfo();

    /**
     * @brief Processa erros específicos da plataforma
     * @return Código de erro padronizado
     */
    SocketError TranslateError();

    /**
     * @brief Criptografa um pacote antes do envio
     * @param data Buffer contendo o pacote
     * @param size Tamanho do pacote
     * @param key Chave de criptografia
     */
    void EncryptPacket(char* data, int size, int key);

    /**
     * @brief Descriptografa um pacote após recepção
     * @param data Buffer contendo o pacote
     * @param size Tamanho do pacote
     * @param key Chave de criptografia
     */
    void DecryptPacket(char* data, int size, int key);

    /**
     * @brief Verifica a validade de um pacote recebido
     * @param data Buffer contendo o pacote
     * @param size Tamanho do pacote
     * @return true se o pacote é válido, false caso contrário
     */
    bool ValidatePacket(const char* data, int size);

    /**
     * @brief Calcula o checksum de um pacote
     * @param data Buffer contendo o pacote
     * @param size Tamanho do pacote
     * @return Valor do checksum
     */
    uint8_t CalculateChecksum(const char* data, int size);

    /**
     * @brief Envia um pacote de ping
     * @return true se enviado com sucesso, false caso contrário
     */
    bool SendPing();

    /**
     * @brief Atualiza o timestamp de última atividade
     */
    void UpdateLastActivity();

    /**
     * @brief Verifica se houve timeout por inatividade
     * @return true se houve timeout, false caso contrário
     */
    bool CheckIdleTimeout();

    /**
     * @brief Obtém o timestamp atual em milissegundos
     * @return Timestamp atual
     */
    uint64_t GetCurrentTimeMs();

    /**
     * @brief Codifica um valor de 32 bits para o formato de rede
     * @param value Valor a codificar
     * @return Valor codificado
     */
    uint32_t HostToNetwork32(uint32_t value);

    /**
     * @brief Decodifica um valor de 32 bits do formato de rede
     * @param value Valor a decodificar
     * @return Valor decodificado
     */
    uint32_t NetworkToHost32(uint32_t value);

    /**
     * @brief Codifica um valor de 16 bits para o formato de rede
     * @param value Valor a codificar
     * @return Valor codificado
     */
    uint16_t HostToNetwork16(uint16_t value);

    /**
     * @brief Decodifica um valor de 16 bits do formato de rede
     * @param value Valor a decodificar
     * @return Valor decodificado
     */
    uint16_t NetworkToHost16(uint16_t value);
};

#endif // _CPSOCK_H_

} // namespace wydbr
