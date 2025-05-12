/**
 * ConnectionManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/network/ConnectionManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _CONNECTION_MANAGER_H_
#define _CONNECTION_MANAGER_H_

/**
 * @file ConnectionManager.h
 * @brief Sistema de gerenciamento de conexões - Otimizado para WYD
 * 
 * Implementa o sistema de gerenciamento de conexões do servidor WYD,
 * mantendo compatibilidade com o protocolo original e implementando
 * otimizações específicas para o protocolo do WYD.
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <chrono>
#include <atomic>

#include "../GlobalDef.h"
#include "../TM_Protocol.h"

namespace wydbr {

/**
 * @brief Estados de conexão
 */
enum ConnectionState {
    CONN_DISCONNECTED = 0,    // Desconectado
    CONN_CONNECTING = 1,      // Conectando
    CONN_CONNECTED = 2,       // Conectado
    CONN_AUTHENTICATING = 3,  // Autenticando
    CONN_AUTHENTICATED = 4,   // Autenticado
    CONN_GAME = 5,            // Em jogo
    CONN_RECONNECTING = 6,    // Reconectando
    CONN_TRANSFERRING = 7,    // Transferindo
    CONN_CLOSING = 8,         // Fechando
    CONN_ERROR = 9            // Erro
};

/**
 * @brief Tipos de erro de conexão
 */
enum ConnectionErrorType {
    CONN_ERROR_NONE = 0,              // Sem erro
    CONN_ERROR_SOCKET = 1,            // Erro de socket
    CONN_ERROR_TIMEOUT = 2,           // Timeout
    CONN_ERROR_PROTOCOL = 3,          // Erro de protocolo
    CONN_ERROR_AUTH = 4,              // Erro de autenticação
    CONN_ERROR_VERSION = 5,           // Erro de versão
    CONN_ERROR_BANNED = 6,            // Banido
    CONN_ERROR_MAINTENANCE = 7,       // Manutenção
    CONN_ERROR_CAPACITY = 8,          // Capacidade excedida
    CONN_ERROR_DUPLICATE = 9,         // Conta duplicada
    CONN_ERROR_ALREADY_CONNECTED = 10, // Já conectado
    CONN_ERROR_ACCOUNT_LOCK = 11,     // Conta bloqueada
    CONN_ERROR_FLOOD = 12,            // Flood protection
    CONN_ERROR_INVALID_PACKET = 13,   // Pacote inválido
    CONN_ERROR_CHECKSUM = 14,         // Erro de checksum
    CONN_ERROR_SERVER = 15,           // Erro de servidor
    CONN_ERROR_CUSTOM = 16            // Erro personalizado
};

/**
 * @brief Estatísticas de conexão
 */
struct ConnectionStats {
    uint64_t bytesReceived;           // Bytes recebidos
    uint64_t bytesSent;               // Bytes enviados
    uint64_t packetsReceived;         // Pacotes recebidos
    uint64_t packetsSent;             // Pacotes enviados
    uint64_t errors;                  // Erros
    uint64_t invalidPackets;          // Pacotes inválidos
    uint64_t checksumErrors;          // Erros de checksum
    uint64_t overflowErrors;          // Erros de overflow
    uint64_t timeoutErrors;           // Erros de timeout
    uint64_t protocolErrors;          // Erros de protocolo
    uint64_t reconnections;           // Reconexões
    uint32_t pingTime;                // Tempo de ping (ms)
    uint32_t lastPingTime;            // Último tempo de ping
    uint32_t lastPacketTime;          // Último tempo de pacote
    uint32_t connectionTime;          // Tempo de conexão
    uint32_t authTime;                // Tempo de autenticação
    
    ConnectionStats() : bytesReceived(0), bytesSent(0), packetsReceived(0), 
                       packetsSent(0), errors(0), invalidPackets(0), 
                       checksumErrors(0), overflowErrors(0), timeoutErrors(0), 
                       protocolErrors(0), reconnections(0), pingTime(0), 
                       lastPingTime(0), lastPacketTime(0), connectionTime(0), 
                       authTime(0) {}
};

/**
 * @brief Informações de conexão
 */
struct ConnectionInfo {
    int socket;                       // Socket
    std::string ipAddress;            // Endereço IP
    uint16_t port;                    // Porta
    uint32_t accountId;               // ID da conta
    std::string accountName;          // Nome da conta
    ConnectionState state;            // Estado
    ConnectionErrorType errorType;    // Tipo de erro
    std::string errorMessage;         // Mensagem de erro
    uint32_t key;                     // Chave de criptografia
    uint8_t seed;                     // Seed de randomização
    uint8_t lastTimeStamp;            // Último timestamp
    uint16_t clientVersion;           // Versão do cliente
    uint16_t clientType;              // Tipo de cliente
    std::string macAddress;           // Endereço MAC
    uint16_t packetSequence;          // Sequência de pacotes
    bool useCompression;              // Usar compressão
    bool useEncryption;               // Usar criptografia
    int playerId;                     // ID do jogador
    uint32_t lastKeepAlive;           // Último keep-alive
    bool pendingClose;                // Fechamento pendente
    uint32_t closeTime;               // Tempo de fechamento
    ConnectionStats stats;            // Estatísticas
    
    ConnectionInfo() : socket(-1), port(0), accountId(0), state(CONN_DISCONNECTED), 
                      errorType(CONN_ERROR_NONE), key(0), seed(0), lastTimeStamp(0), 
                      clientVersion(0), clientType(0), packetSequence(0), 
                      useCompression(false), useEncryption(false), playerId(-1), 
                      lastKeepAlive(0), pendingClose(false), closeTime(0) {}
};

/**
 * @brief Classe de buffer de conexão
 * 
 * Implementa um buffer circular para gerenciamento eficiente de dados de rede
 */
/**
 * Classe WYDConnectionBuffer
 * 
 * Esta classe implementa a funcionalidade WYDConnectionBuffer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ConnectionBuffer {
public:
    /**
     * @brief Construtor
     * @param size Tamanho do buffer
     */
    ConnectionBuffer(size_t size = 8192);
    
    /**
     * @brief Destrutor
     */
    ~ConnectionBuffer();
    
    /**
     * @brief Reseta o buffer
     */
    void Reset();
    
    /**
     * @brief Escreve dados no buffer
     * @param data Dados
     * @param size Tamanho
     * @return Número de bytes escritos
     */
    size_t Write(const void* data, size_t size);
    
    /**
     * @brief Lê dados do buffer
     * @param data Buffer para receber dados
     * @param size Tamanho máximo
     * @return Número de bytes lidos
     */
    size_t Read(void* data, size_t size);
    
    /**
     * @brief Espia dados no buffer sem remover
     * @param data Buffer para receber dados
     * @param size Tamanho máximo
     * @return Número de bytes lidos
     */
    size_t Peek(void* data, size_t size) const;
    
    /**
     * @brief Descarta bytes do buffer
     * @param size Número de bytes
     * @return Número de bytes descartados
     */
    size_t Skip(size_t size);
    
    /**
     * @brief Obtém o espaço disponível
     * @return Espaço disponível
     */
    size_t GetSpace() const;
    
    /**
     * @brief Obtém o tamanho dos dados
     * @return Tamanho dos dados
     */
    size_t GetDataSize() const;
    
    /**
     * @brief Obtém o tamanho total do buffer
     * @return Tamanho total
     */
    size_t GetTotalSize() const;
    
    /**
     * @brief Verifica se o buffer está vazio
     * @return true se vazio
     */
    bool IsEmpty() const;
    
    /**
     * @brief Verifica se o buffer está cheio
     * @return true se cheio
     */
    bool IsFull() const;
    
    /**
     * @brief Obtém o ponteiro para escrita direta
     * @param availableSize Ponteiro para receber o tamanho disponível
     * @return Ponteiro para escrita
     */
    uint8_t* GetWritePointer(size_t* availableSize = nullptr);
    
    /**
     * @brief Atualiza o ponteiro de escrita
     * @param size Número de bytes escritos
     * @return true se atualizado com sucesso
     */
    bool AdvanceWritePointer(size_t size);
    
    /**
     * @brief Obtém o ponteiro para leitura direta
     * @param availableSize Ponteiro para receber o tamanho disponível
     * @return Ponteiro para leitura
     */
    const uint8_t* GetReadPointer(size_t* availableSize = nullptr) const;
    
    /**
     * @brief Atualiza o ponteiro de leitura
     * @param size Número de bytes lidos
     * @return true se atualizado com sucesso
     */
    bool AdvanceReadPointer(size_t size);
    
private:
    uint8_t* buffer_;                // Buffer
    size_t bufferSize_;              // Tamanho do buffer
    size_t readPos_;                 // Posição de leitura
    size_t writePos_;                // Posição de escrita
    size_t dataSize_;                // Tamanho dos dados
};

/**
 * @brief Classe de conexão
 * 
 * Representa uma conexão com um cliente
 */
/**
 * Classe WYDConnection
 * 
 * Esta classe implementa a funcionalidade WYDConnection conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Connection {
public:
    /**
     * @brief Construtor
     * @param socket Socket
     * @param ipAddress Endereço IP
     * @param port Porta
     * @param id ID da conexão
     */
    Connection(int socket, const std::string& ipAddress, uint16_t port, int id);
    
    /**
     * @brief Destrutor
     */
    ~Connection();
    
    /**
     * @brief Inicializa a conexão
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza a conexão
     */
    void Finalize();
    
    /**
     * @brief Fecha a conexão
     * @param immediate Se deve fechar imediatamente
     * @param errorType Tipo de erro
     * @param errorMessage Mensagem de erro
     */
    void Close(bool immediate = false, ConnectionErrorType errorType = CONN_ERROR_NONE, const std::string& errorMessage = "");
    
    /**
     * @brief Processa dados recebidos
     * @param data Dados recebidos
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool ProcessReceive(const void* data, size_t size);
    
    /**
     * @brief Envia um pacote
     * @param data Dados
     * @param size Tamanho
     * @return true se enviado com sucesso
     */
    bool SendPacket(const void* data, size_t size);
    
    /**
     * @brief Envia um pacote com dados
     * @param opcode Opcode
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @return true se enviado com sucesso
     */
    bool SendPacketWithData(uint16_t opcode, const void* data, size_t dataSize);
    
    /**
     * @brief Envia um pacote simples
     * @param opcode Opcode
     * @return true se enviado com sucesso
     */
    bool SendSimplePacket(uint16_t opcode);
    
    /**
     * @brief Envia um heartbeat
     * @return true se enviado com sucesso
     */
    bool SendHeartbeat();
    
    /**
     * @brief Envia uma mensagem
     * @param message Mensagem
     * @param type Tipo de mensagem
     * @return true se enviado com sucesso
     */
    bool SendMessage(const std::string& message, uint16_t type = CHAT_SYSTEM);
    
    /**
     * @brief Processa um tick
     * @param currentTime Tempo atual
     * @return true se processado com sucesso
     */
    bool ProcessTick(uint32_t currentTime);
    
    /**
     * @brief Processa um pacote
     * @param packet Pacote
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessPacket(const void* packet, size_t size);
    
    /**
     * @brief Obtém os dados para envio
     * @param buffer Buffer para receber dados
     * @param bufferSize Tamanho máximo do buffer
     * @return Número de bytes copiados
     */
    size_t GetSendData(void* buffer, size_t bufferSize);
    
    /**
     * @brief Registra um callback para processamento de pacote
     * @param opcode Opcode
     * @param callback Função de callback
     */
    void RegisterPacketCallback(uint16_t opcode, std::function<bool(Connection*, const void*, size_t)> callback);
    
    /**
     * @brief Define o ID da conta
     * @param accountId ID da conta
     */
    void SetAccountId(uint32_t accountId);
    
    /**
     * @brief Define o nome da conta
     * @param accountName Nome da conta
     */
    void SetAccountName(const std::string& accountName);
    
    /**
     * @brief Define o ID do jogador
     * @param playerId ID do jogador
     */
    void SetPlayerId(int playerId);
    
    /**
     * @brief Define o estado
     * @param state Estado
     */
    void SetState(ConnectionState state);
    
    /**
     * @brief Define o erro
     * @param errorType Tipo de erro
     * @param errorMessage Mensagem de erro
     */
    void SetError(ConnectionErrorType errorType, const std::string& errorMessage);
    
    /**
     * @brief Define o último keep-alive
     * @param time Tempo
     */
    void SetLastKeepAlive(uint32_t time);
    
    /**
     * @brief Define a versão do cliente
     * @param version Versão
     */
    void SetClientVersion(uint16_t version);
    
    /**
     * @brief Define o tipo do cliente
     * @param type Tipo
     */
    void SetClientType(uint16_t type);
    
    /**
     * @brief Define o endereço MAC
     * @param macAddress Endereço MAC
     */
    void SetMacAddress(const std::string& macAddress);
    
    /**
     * @brief Define a chave de criptografia
     * @param key Chave
     */
    void SetKey(uint32_t key);
    
    /**
     * @brief Define o seed de randomização
     * @param seed Seed
     */
    void SetSeed(uint8_t seed);
    
    /**
     * @brief Define se usa compressão
     * @param useCompression Flag
     */
    void SetUseCompression(bool useCompression);
    
    /**
     * @brief Define se usa criptografia
     * @param useEncryption Flag
     */
    void SetUseEncryption(bool useEncryption);
    
    /**
     * @brief Obtém o ID
     * @return ID
     */
    int GetId() const;
    
    /**
     * @brief Obtém o socket
     * @return Socket
     */
    int GetSocket() const;
    
    /**
     * @brief Obtém o endereço IP
     * @return Endereço IP
     */
    const std::string& GetIpAddress() const;
    
    /**
     * @brief Obtém a porta
     * @return Porta
     */
    uint16_t GetPort() const;
    
    /**
     * @brief Obtém o ID da conta
     * @return ID da conta
     */
    uint32_t GetAccountId() const;
    
    /**
     * @brief Obtém o nome da conta
     * @return Nome da conta
     */
    const std::string& GetAccountName() const;
    
    /**
     * @brief Obtém o ID do jogador
     * @return ID do jogador
     */
    int GetPlayerId() const;
    
    /**
     * @brief Obtém o estado
     * @return Estado
     */
    ConnectionState GetState() const;
    
    /**
     * @brief Obtém o tipo de erro
     * @return Tipo de erro
     */
    ConnectionErrorType GetErrorType() const;
    
    /**
     * @brief Obtém a mensagem de erro
     * @return Mensagem de erro
     */
    const std::string& GetErrorMessage() const;
    
    /**
     * @brief Obtém o último keep-alive
     * @return Último keep-alive
     */
    uint32_t GetLastKeepAlive() const;
    
    /**
     * @brief Obtém a versão do cliente
     * @return Versão do cliente
     */
    uint16_t GetClientVersion() const;
    
    /**
     * @brief Obtém o tipo do cliente
     * @return Tipo do cliente
     */
    uint16_t GetClientType() const;
    
    /**
     * @brief Obtém o endereço MAC
     * @return Endereço MAC
     */
    const std::string& GetMacAddress() const;
    
    /**
     * @brief Obtém a chave de criptografia
     * @return Chave
     */
    uint32_t GetKey() const;
    
    /**
     * @brief Obtém o seed de randomização
     * @return Seed
     */
    uint8_t GetSeed() const;
    
    /**
     * @brief Verifica se usa compressão
     * @return true se usar
     */
    bool GetUseCompression() const;
    
    /**
     * @brief Verifica se usa criptografia
     * @return true se usar
     */
    bool GetUseEncryption() const;
    
    /**
     * @brief Obtém as estatísticas
     * @return Estatísticas
     */
    const ConnectionStats& GetStats() const;
    
    /**
     * @brief Verifica se está conectado
     * @return true se conectado
     */
    bool IsConnected() const;
    
    /**
     * @brief Verifica se está autenticado
     * @return true se autenticado
     */
    bool IsAuthenticated() const;
    
    /**
     * @brief Verifica se está em jogo
     * @return true se em jogo
     */
    bool IsInGame() const;
    
    /**
     * @brief Verifica se está fechando
     * @return true se fechando
     */
    bool IsClosing() const;
    
    /**
     * @brief Verifica se tem erro
     * @return true se tiver erro
     */
    bool HasError() const;
    
private:
    /**
     * @brief Processa um pacote de login
     * @param packet Pacote
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessLoginPacket(const void* packet, size_t size);
    
    /**
     * @brief Processa um pacote de heartbeat
     * @param packet Pacote
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessHeartbeatPacket(const void* packet, size_t size);
    
    /**
     * @brief Processa um pacote de versão
     * @param packet Pacote
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessVersionPacket(const void* packet, size_t size);
    
    /**
     * @brief Processa um pacote de logout
     * @param packet Pacote
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessLogoutPacket(const void* packet, size_t size);
    
    /**
     * @brief Processa um pacote de desconexão
     * @param packet Pacote
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessDisconnectPacket(const void* packet, size_t size);
    
    /**
     * @brief Processa um pacote de reconexão
     * @param packet Pacote
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessReconnectPacket(const void* packet, size_t size);
    
    /**
     * @brief Prepara um pacote para envio
     * @param data Dados
     * @param size Tamanho
     * @param preparedData Buffer para dados preparados
     * @param preparedSize Tamanho máximo do buffer
     * @return Tamanho dos dados preparados
     */
    size_t PreparePacketForSend(const void* data, size_t size, void* preparedData, size_t preparedSize);
    
    /**
     * @brief Prepara um pacote para processamento
     * @param data Dados
     * @param size Tamanho
     * @param preparedData Buffer para dados preparados
     * @param preparedSize Tamanho máximo do buffer
     * @return Tamanho dos dados preparados
     */
    size_t PreparePacketForProcess(const void* data, size_t size, void* preparedData, size_t preparedSize);
    
    /**
     * @brief Inicializa callbacks padrão
     */
    void InitializeDefaultCallbacks();
    
    /**
     * @brief Atualiza estatísticas
     * @param currentTime Tempo atual
     */
    void UpdateStats(uint32_t currentTime);
    
    int id_;                            // ID da conexão
    ConnectionInfo info_;               // Informações da conexão
    ConnectionBuffer receiveBuffer_;    // Buffer de recepção
    ConnectionBuffer sendBuffer_;       // Buffer de envio
    
    // Registro de callbacks de pacotes
    std::map<uint16_t, std::function<bool(Connection*, const void*, size_t)>> packetCallbacks_;
    
    // Mutex para proteção de dados
    mutable std::mutex mutex_;
};

/**
 * @brief Gerenciador de conexões
 * 
 * Classe singleton responsável por gerenciar todas as conexões do servidor
 */
/**
 * Classe WYDConnectionManager
 * 
 * Esta classe implementa a funcionalidade WYDConnectionManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ConnectionManager {
public:
    /**
     * @brief Obtém a instância única
     * @return Referência para a instância
     */
    static ConnectionManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param port Porta
     * @param maxConnections Número máximo de conexões
     * @return true se inicializado com sucesso
     */
    bool Initialize(uint16_t port = GAME_PORT, int maxConnections = MAX_PLAYER);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Finalize();
    
    /**
     * @brief Inicia o servidor
     * @return true se iniciado com sucesso
     */
    bool Start();
    
    /**
     * @brief Para o servidor
     */
    void Stop();
    
    /**
     * @brief Processa conexões
     * @param timeout Timeout (ms)
     * @return Número de conexões processadas
     */
    int ProcessConnections(int timeout = 10);
    
    /**
     * @brief Fecha todas as conexões
     * @param immediate Se deve fechar imediatamente
     * @param errorType Tipo de erro
     * @param errorMessage Mensagem de erro
     */
    void CloseAllConnections(bool immediate = false, ConnectionErrorType errorType = CONN_ERROR_NONE, const std::string& errorMessage = "");
    
    /**
     * @brief Obtém uma conexão pelo ID
     * @param connectionId ID da conexão
     * @return Ponteiro para a conexão (nullptr se não existir)
     */
    Connection* GetConnection(int connectionId);
    
    /**
     * @brief Obtém uma conexão pelo ID da conta
     * @param accountId ID da conta
     * @return Ponteiro para a conexão (nullptr se não existir)
     */
    Connection* GetConnectionByAccountId(uint32_t accountId);
    
    /**
     * @brief Obtém uma conexão pelo nome da conta
     * @param accountName Nome da conta
     * @return Ponteiro para a conexão (nullptr se não existir)
     */
    Connection* GetConnectionByAccountName(const std::string& accountName);
    
    /**
     * @brief Obtém uma conexão pelo ID do jogador
     * @param playerId ID do jogador
     * @return Ponteiro para a conexão (nullptr se não existir)
     */
    Connection* GetConnectionByPlayerId(int playerId);
    
    /**
     * @brief Fecha uma conexão
     * @param connectionId ID da conexão
     * @param immediate Se deve fechar imediatamente
     * @param errorType Tipo de erro
     * @param errorMessage Mensagem de erro
     * @return true se fechada com sucesso
     */
    bool CloseConnection(int connectionId, bool immediate = false, ConnectionErrorType errorType = CONN_ERROR_NONE, const std::string& errorMessage = "");
    
    /**
     * @brief Envia um pacote para uma conexão
     * @param connectionId ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @return true se enviado com sucesso
     */
    bool SendPacket(int connectionId, const void* data, size_t size);
    
    /**
     * @brief Envia um pacote para todas as conexões
     * @param data Dados
     * @param size Tamanho
     * @param filter Função de filtro (opcional)
     * @return Número de conexões que receberam o pacote
     */
    int BroadcastPacket(const void* data, size_t size, std::function<bool(Connection*)> filter = nullptr);
    
    /**
     * @brief Envia uma mensagem para uma conexão
     * @param connectionId ID da conexão
     * @param message Mensagem
     * @param type Tipo de mensagem
     * @return true se enviada com sucesso
     */
    bool SendMessage(int connectionId, const std::string& message, uint16_t type = CHAT_SYSTEM);
    
    /**
     * @brief Envia uma mensagem para todas as conexões
     * @param message Mensagem
     * @param type Tipo de mensagem
     * @param filter Função de filtro (opcional)
     * @return Número de conexões que receberam a mensagem
     */
    int BroadcastMessage(const std::string& message, uint16_t type = CHAT_SYSTEM, std::function<bool(Connection*)> filter = nullptr);
    
    /**
     * @brief Registra um callback para conexão nova
     * @param callback Função de callback
     */
    void RegisterNewConnectionCallback(std::function<void(Connection*)> callback);
    
    /**
     * @brief Registra um callback para conexão fechada
     * @param callback Função de callback
     */
    void RegisterConnectionClosedCallback(std::function<void(Connection*)> callback);
    
    /**
     * @brief Registra um callback para autenticação
     * @param callback Função de callback
     */
    void RegisterAuthCallback(std::function<bool(Connection*, const std::string&, const std::string&)> callback);
    
    /**
     * @brief Registra um callback para verificação de conta
     * @param callback Função de callback
     */
    void RegisterAccountCheckCallback(std::function<bool(const std::string&)> callback);
    
    /**
     * @brief Registra um callback para verificação de IP
     * @param callback Função de callback
     */
    void RegisterIpCheckCallback(std::function<bool(const std::string&)> callback);
    
    /**
     * @brief Registra um callback para processamento de pacote
     * @param opcode Opcode
     * @param callback Função de callback
     */
    void RegisterPacketCallback(uint16_t opcode, std::function<bool(Connection*, const void*, size_t)> callback);
    
    /**
     * @brief Atualiza uma conexão
     * @param connectionId ID da conexão
     * @param currentTime Tempo atual
     * @return true se atualizada com sucesso
     */
    bool UpdateConnection(int connectionId, uint32_t currentTime);
    
    /**
     * @brief Atualiza todas as conexões
     * @param currentTime Tempo atual
     * @return Número de conexões atualizadas
     */
    int UpdateAllConnections(uint32_t currentTime);
    
    /**
     * @brief Desconecta conexões inativas
     * @param timeout Timeout (ms)
     * @return Número de conexões desconectadas
     */
    int DisconnectInactiveConnections(uint32_t timeout = MAX_PING_TOLERANCE);
    
    /**
     * @brief Obtém o número de conexões
     * @return Número de conexões
     */
    int GetConnectionCount() const;
    
    /**
     * @brief Obtém o número de conexões autenticadas
     * @return Número de conexões autenticadas
     */
    int GetAuthenticatedConnectionCount() const;
    
    /**
     * @brief Obtém o número de conexões em jogo
     * @return Número de conexões em jogo
     */
    int GetInGameConnectionCount() const;
    
    /**
     * @brief Obtém as estatísticas de todas as conexões
     * @return Estatísticas
     */
    ConnectionStats GetTotalStats() const;
    
    /**
     * @brief Verifica se o servidor está rodando
     * @return true se rodando
     */
    bool IsRunning() const;
    
    /**
     * @brief Imprime estatísticas
     * @param detailed Se deve imprimir detalhes
     */
    void PrintStats(bool detailed = false);
    
    /**
     * @brief Retorna a porta ativa
     * @return Porta
     */
    uint16_t GetPort() const { return port_; }
    
    /**
     * @brief Define a capacidade máxima
     * @param maxConnections Número máximo de conexões
     */
    void SetMaxConnections(int maxConnections) { maxConnections_ = maxConnections; }
    
    /**
     * @brief Obtém a capacidade máxima
     * @return Número máximo de conexões
     */
    int GetMaxConnections() const { return maxConnections_; }
    
    /**
     * @brief Define o timeout de conexão
     * @param timeout Timeout (ms)
     */
    void SetConnectionTimeout(uint32_t timeout) { connectionTimeout_ = timeout; }
    
    /**
     * @brief Obtém o timeout de conexão
     * @return Timeout (ms)
     */
    uint32_t GetConnectionTimeout() const { return connectionTimeout_; }
    
    /**
     * @brief Define o intervalo de heartbeat
     * @param interval Intervalo (ms)
     */
    void SetHeartbeatInterval(uint32_t interval) { heartbeatInterval_ = interval; }
    
    /**
     * @brief Obtém o intervalo de heartbeat
     * @return Intervalo (ms)
     */
    uint32_t GetHeartbeatInterval() const { return heartbeatInterval_; }
    
    /**
     * @brief Define o modo de manutenção
     * @param maintenance Flag
     */
    void SetMaintenanceMode(bool maintenance) { maintenanceMode_ = maintenance; }
    
    /**
     * @brief Verifica se está em modo de manutenção
     * @return true se estiver
     */
    bool IsMaintenanceMode() const { return maintenanceMode_; }
    
    /**
     * @brief Define a mensagem de manutenção
     * @param message Mensagem
     */
    void SetMaintenanceMessage(const std::string& message) { maintenanceMessage_ = message; }
    
    /**
     * @brief Obtém a mensagem de manutenção
     * @return Mensagem
     */
    const std::string& GetMaintenanceMessage() const { return maintenanceMessage_; }
    
    /**
     * @brief Define o modo de depuração
     * @param debug Flag
     */
    void SetDebugMode(bool debug) { debugMode_ = debug; }
    
    /**
     * @brief Verifica se está em modo de depuração
     * @return true se estiver
     */
    bool IsDebugMode() const { return debugMode_; }
    
/**
 * ConnectionManager
 * 
 * Implementa a funcionalidade ConnectionManager conforme especificação original.
 * @return Retorna private:
 */

    
private:
    ConnectionManager();  // Construtor privado (singleton)
    ~ConnectionManager(); // Destrutor privado (singleton)
    
    /**
     * @brief Thread de aceitação
     */
    void AcceptThread();
    
    /**
     * @brief Thread de recepção
     */
    void ReceiveThread();
    
    /**
     * @brief Thread de envio
     */
    void SendThread();
    
    /**
     * @brief Aceita uma nova conexão
     * @return true se aceita com sucesso
     */
    bool AcceptConnection();
    
    /**
     * @brief Processa uma nova conexão
     * @param socket Socket
     * @param ipAddress Endereço IP
     * @param port Porta
     * @return ID da conexão (-1 se falhar)
     */
    int ProcessNewConnection(int socket, const std::string& ipAddress, uint16_t port);
    
    /**
     * @brief Processa dados recebidos
     * @param connectionId ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessReceivedData(int connectionId, const void* data, size_t size);
    
    /**
     * @brief Remove uma conexão
     * @param connectionId ID da conexão
     */
    void RemoveConnection(int connectionId);
    
    /**
     * @brief Envia dados
     * @param connectionId ID da conexão
     * @return true se enviado com sucesso
     */
    bool SendData(int connectionId);
    
    /**
     * @brief Verifica se um IP está banido
     * @param ipAddress Endereço IP
     * @return true se estiver banido
     */
    bool IsIpBanned(const std::string& ipAddress);
    
    /**
     * @brief Verifica se um MAC está banido
     * @param macAddress Endereço MAC
     * @return true se estiver banido
     */
    bool IsMacBanned(const std::string& macAddress);
    
    /**
     * @brief Verifica se uma conta está banida
     * @param accountName Nome da conta
     * @return true se estiver banida
     */
    bool IsAccountBanned(const std::string& accountName);
    
    /**
     * @brief Obtém o próximo ID de conexão
     * @return Próximo ID
     */
    int GetNextConnectionId();
    
    /**
     * @brief Registra callbacks padrão
     */
    void RegisterDefaultCallbacks();
    
    static ConnectionManager* instance_;          // Instância única (singleton)
    
    // Configurações
    uint16_t port_;                               // Porta
    int maxConnections_;                          // Número máximo de conexões
    uint32_t connectionTimeout_;                  // Timeout de conexão (ms)
    uint32_t heartbeatInterval_;                  // Intervalo de heartbeat (ms)
    bool maintenanceMode_;                        // Modo de manutenção
    std::string maintenanceMessage_;              // Mensagem de manutenção
    bool debugMode_;                              // Modo de depuração
    
    // Estado
    bool initialized_;                            // Inicializado
    bool running_;                                // Rodando
    int acceptSocket_;                            // Socket de aceitação
    int receiveSocket_;                           // Socket de recepção
    int nextConnectionId_;                        // Próximo ID de conexão
    
    // Conexões
    std::map<int, std::unique_ptr<Connection>> connections_;           // Conexões por ID
    std::map<uint32_t, int> connectionsByAccountId_;                   // Conexões por ID de conta
    std::map<std::string, int> connectionsByAccountName_;              // Conexões por nome de conta
    std::map<int, int> connectionsByPlayerId_;                         // Conexões por ID de jogador
    
    // Listas de banimento
    std::set<std::string> bannedIps_;                                   // IPs banidos
    std::set<std::string> bannedMacs_;                                  // MACs banidos
    std::set<std::string> bannedAccounts_;                              // Contas banidas
    
    // Threads
    std::thread acceptThread_;                                          // Thread de aceitação
    std::thread receiveThread_;                                         // Thread de recepção
    std::thread sendThread_;                                            // Thread de envio
    
    // Flags de controle
    std::atomic<bool> acceptThreadRunning_;                             // Thread de aceitação rodando
    std::atomic<bool> receiveThreadRunning_;                            // Thread de recepção rodando
    std::atomic<bool> sendThreadRunning_;                               // Thread de envio rodando
    
    // Callbacks
    std::function<void(Connection*)> newConnectionCallback_;            // Callback de conexão nova
    std::function<void(Connection*)> connectionClosedCallback_;         // Callback de conexão fechada
    std::function<bool(Connection*, const std::string&, const std::string&)> authCallback_; // Callback de autenticação
    std::function<bool(const std::string&)> accountCheckCallback_;      // Callback de verificação de conta
    std::function<bool(const std::string&)> ipCheckCallback_;           // Callback de verificação de IP
    std::map<uint16_t, std::function<bool(Connection*, const void*, size_t)>> packetCallbacks_; // Callbacks de pacotes
    
    // Estatísticas
    ConnectionStats totalStats_;                                        // Estatísticas totais
    
    // Mutexes
    mutable std::mutex connectionsMutex_;                               // Mutex para conexões
    mutable std::mutex banMutex_;                                       // Mutex para listas de banimento
    
    // Desabilita cópias (singleton)
    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;
};

/**
 * @brief Funções utilitárias de rede
 */
namespace NetworkUtils {
    /**
     * @brief Obtém o endereço IP de uma interface
     * @param interfaceName Nome da interface
     * @return Endereço IP
     */
    std::string GetLocalIpAddress(const std::string& interfaceName = "");
    
    /**
     * @brief Verifica se um endereço IP é válido
     * @param ipAddress Endereço IP
     * @return true se válido
     */
    bool IsValidIpAddress(const std::string& ipAddress);
    
    /**
     * @brief Verifica se um endereço MAC é válido
     * @param macAddress Endereço MAC
     * @return true se válido
     */
    bool IsValidMacAddress(const std::string& macAddress);
    
    /**
     * @brief Obtém o IP de um hostname
     * @param hostname Nome do host
     * @return Endereço IP
     */
    std::string GetIpFromHostname(const std::string& hostname);
    
    /**
     * @brief Obtém o hostname de um IP
     * @param ipAddress Endereço IP
     * @return Nome do host
     */
    std::string GetHostnameFromIp(const std::string& ipAddress);
    
    /**
     * @brief Converte um endereço IP para um número
     * @param ipAddress Endereço IP
     * @return Número
     */
    uint32_t IpToNumber(const std::string& ipAddress);
    
    /**
     * @brief Converte um número para um endereço IP
     * @param ipNumber Número
     * @return Endereço IP
     */
    std::string NumberToIp(uint32_t ipNumber);
    
    /**
     * @brief Obtém o endereço IP de um socket
     * @param socket Socket
     * @return Endereço IP
     */
    std::string GetSocketIpAddress(int socket);
    
    /**
     * @brief Obtém a porta de um socket
     * @param socket Socket
     * @return Porta
     */
    uint16_t GetSocketPort(int socket);
    
    /**
     * @brief Verifica se um socket é válido
     * @param socket Socket
     * @return true se válido
     */
    bool IsSocketValid(int socket);
    
    /**
     * @brief Fecha um socket
     * @param socket Socket
     */
    void CloseSocket(int socket);
    
    /**
     * @brief Inicializa a biblioteca de sockets
     * @return true se inicializado com sucesso
     */
    bool InitializeSocketLibrary();
    
    /**
     * @brief Finaliza a biblioteca de sockets
     */
    void FinalizeSocketLibrary();
}

/**
 * @brief Funções utilitárias de criptografia para o protocolo WYD
 */
namespace WYDCrypto {
    /**
     * @brief Gera uma chave de criptografia
     * @return Chave
     */
    uint32_t GenerateKey();
    
    /**
     * @brief Gera um seed de randomização
     * @return Seed
     */
    uint8_t GenerateSeed();
    
    /**
     * @brief Criptografa um pacote
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     */
    void EncryptPacket(void* data, size_t size, uint32_t key);
    
    /**
     * @brief Descriptografa um pacote
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     */
    void DecryptPacket(void* data, size_t size, uint32_t key);
    
    /**
     * @brief Randomiza um pacote
     * @param data Dados
     * @param size Tamanho
     * @param seed Seed
     */
    void RandomizePacket(void* data, size_t size, uint8_t seed);
    
    /**
     * @brief Desrandomiza um pacote
     * @param data Dados
     * @param size Tamanho
     * @param seed Seed
     */
    void DeRandomizePacket(void* data, size_t size, uint8_t seed);
    
    /**
     * @brief Calcula o checksum de um pacote
     * @param data Dados
     * @param size Tamanho
     * @return Checksum
     */
    uint16_t CalculateChecksum(const void* data, size_t size);
    
    /**
     * @brief Verifica o checksum de um pacote
     * @param data Dados
     * @param size Tamanho
     * @return true se válido
     */
    bool VerifyChecksum(const void* data, size_t size);
    
    /**
     * @brief Prepara um pacote para envio
     * @param data Dados
     * @param size Tamanho
     * @param key Chave
     * @param seed Seed
     * @param timestamp Timestamp
     * @param useEncryption Se deve usar criptografia
     * @param useRandomize Se deve usar randomização
     * @return Checksum calculado
     */
    uint16_t PreparePacket(void* data, size_t size, uint32_t key, uint8_t seed, uint8_t timestamp, bool useEncryption, bool useRandomize);
}

} // namespace wydbr

#endif // _CONNECTION_MANAGER_H_