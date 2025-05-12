/**
 * DBServerConnector.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/database/DBServerConnector.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef DBSERVER_CONNECTOR_H
#define DBSERVER_CONNECTOR_H

#include <string>
#include <map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <functional>

#include "headers/wyd_core.h"

namespace wydbr {


// Forward declarations
class TMServer;
class DBPacket;
class DBServerConnection;

/**
 * Callback para respostas de operações de banco de dados
 */
using DBResponseCallback = std::function<void(const DBPacket& response, bool success)>;

/**
 * Tipos de operações de banco de dados
 */
enum class DBOperation : uint16_t {
    // Operações de conta
    ACCOUNT_AUTH = 0x101,
    ACCOUNT_CREATE = 0x102,
    ACCOUNT_UPDATE = 0x103,
    
    // Operações de personagem
    CHAR_LOAD = 0x201,
    CHAR_SAVE = 0x202,
    CHAR_CREATE = 0x203,
    CHAR_DELETE = 0x204,
    CHAR_LIST = 0x205,
    
    // Operações de item
    ITEM_LOAD = 0x301,
    ITEM_SAVE = 0x302,
    ITEM_CREATE = 0x303,
    ITEM_DELETE = 0x304,
    
    // Operações de guilda
    GUILD_LOAD = 0x401,
    GUILD_SAVE = 0x402,
    GUILD_CREATE = 0x403,
    GUILD_DELETE = 0x404,
    GUILD_MEMBER_ADD = 0x405,
    GUILD_MEMBER_REMOVE = 0x406,
    
    // Operações administrativas
    ADMIN_COMMAND = 0x901,
    ADMIN_BACKUP = 0x902,
    ADMIN_RESTORE = 0x903,
    
    // Operações de sistema
    SYS_SHUTDOWN = 0xF01,
    SYS_MAINTENANCE = 0xF02
};

/**
 * Resultados possíveis de operações de banco de dados
 */
enum class DBResult : uint8_t {
    SUCCESS = 0,
    ERROR_GENERAL = 1,
    ERROR_CONNECTION = 2,
    ERROR_TIMEOUT = 3,
    ERROR_NOT_FOUND = 4,
    ERROR_ALREADY_EXISTS = 5,
    ERROR_INVALID_DATA = 6,
    ERROR_PERMISSION = 7,
    ERROR_MAINTENANCE = 8
};

/**
 * Pacote de comunicação com o servidor de banco de dados
 */
/**
 * Classe WYDDBPacket
 * 
 * Esta classe implementa a funcionalidade WYDDBPacket conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DBPacket {
public:
    // /**
 * DBPacket
 * 
 * Implementa a funcionalidade DBPacket conforme especificação original.
 * @param operation Parâmetro operation
 * @return Retorna Constructores
 */
 Constructores
    DBPacket(DBOperation operation);
    DBPacket(DBOperation operation, const std::string& data);
    
    // Getters
    /**
 * getOperation
 * 
 * Implementa a funcionalidade getOperation conforme especificação original.
 * @return Retorna DBOperation
 */

    DBOperation getOperation() const{ return operation_; }
    /**
 * getSequence
 * 
 * Implementa a funcionalidade getSequence conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getSequence() const{ return sequence_; }
    const std::string& getData() const { return data_; }
    /**
 * getResult
 * 
 * Implementa a funcionalidade getResult conforme especificação original.
 * @return Retorna DBResult
 */

    DBResult getResult() const{ return result_; }
    
    // Setters
    /**
 * setSequence
 * 
 * Implementa a funcionalidade setSequence conforme especificação original.
 * @param sequence Parâmetro sequence
 */

    void setSequence(uint32_t sequence){ sequence_ = sequence; }
    /**
 * setData
 * 
 * Implementa a funcionalidade setData conforme especificação original.
 * @param data Parâmetro data
 */

    void setData(const std::string& data){ data_ = data; }
    /**
 * setResult
 * 
 * Implementa a funcionalidade setResult conforme especificação original.
 * @param result Parâmetro result
 */

    void setResult(DBResult result){ result_ = result; }
    
    // Serialização
    /**
 * serialize
 * 
 * Implementa a funcionalidade serialize conforme especificação original.
 * @return Retorna std::string
 */

    std::string serialize() const;
    /**
 * deserialize
 * 
 * Implementa a funcionalidade deserialize conforme especificação original.
 * @param data Parâmetro data
 * @return Retorna DBPacket
 */

    static DBPacket deserialize(const std::string& data);
    
private:
    DBOperation operation_;
    uint32_t sequence_;
    std::string data_;
    DBResult result_;
};

/**
 * Conector com o servidor de banco de dados (DBSrv)
 * 
 * Gerencia a comunicação entre o TMSrv e o DBSrv, lidando com
 * serialização, deserialização, e gerenciamento assíncrono de operações.
 */
/**
 * Classe WYDDBServerConnector
 * 
 * Esta classe implementa a funcionalidade WYDDBServerConnector conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DBServerConnector : public WYDSystemManager {
public:
    // Construtor
    /**
 * DBServerConnector
 * 
 * Implementa a funcionalidade DBServerConnector conforme especificação original.
 * @param server Parâmetro server
 * @return Retorna explicit
 */

    explicit DBServerConnector(TMServer* server);
    
    // Destrutor
    virtual ~DBServerConnector();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @return Retorna bool
 */

    bool initialize() override;
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp) override;
    
    // Desligamento
    /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 */

    void shutdown() override;
    
    // Conexão ao DBSrv
    /**
 * connect
 * 
 * Implementa a funcionalidade connect conforme especificação original.
 * @param host Parâmetro host
 * @param port Parâmetro port
 * @return Retorna bool
 */

    bool connect(const std::string& host, uint16_t port);
    /**
 * disconnect
 * 
 * Implementa a funcionalidade disconnect conforme especificação original.
 */

    void disconnect();
    /**
 * isConnected
 * 
 * Implementa a funcionalidade isConnected conforme especificação original.
 * @return Retorna bool
 */

    bool isConnected() const;
    
    // Execução de operações no banco de dados
    /**
 * executeOperation
 * 
 * Implementa a funcionalidade executeOperation conforme especificação original.
 * @param operation Parâmetro operation
 * @param data Parâmetro data
 * @param nullptr Parâmetro nullptr
 * @param padrão Parâmetro padrão
 * @return Retorna uint32_t
 */

    uint32_t executeOperation(
        DBOperation operation, 
        const std::string& data, 
        DBResponseCallback callback = nullptr,
        uint32_t timeoutMs = 30000 // 30 segundos por padrão
    );
    
    // Autenticação de conta (wrapper para operação comum)
    /**
 * authenticateAccount
 * 
 * Implementa a funcionalidade authenticateAccount conforme especificação original.
 * @param username Parâmetro username
 * @param password Parâmetro password
 * @param callback Parâmetro callback
 * @return Retorna bool
 */

    bool authenticateAccount(
        const std::string& username, 
        const std::string& password, 
        DBResponseCallback callback
    );
    
    // Carregamento de personagem (wrapper para operação comum)
    /**
 * loadCharacter
 * 
 * Implementa a funcionalidade loadCharacter conforme especificação original.
 * @param accountId Parâmetro accountId
 * @param characterName Parâmetro characterName
 * @param callback Parâmetro callback
 * @return Retorna bool
 */

    bool loadCharacter(
        uint32_t accountId, 
        const std::string& characterName, 
        DBResponseCallback callback
    );
    
    // Salvamento de personagem (wrapper para operação comum)
    /**
 * saveCharacter
 * 
 * Implementa a funcionalidade saveCharacter conforme especificação original.
 * @param accountId Parâmetro accountId
 * @param characterData Parâmetro characterData
 * @param callback Parâmetro callback
 * @return Retorna bool
 */

    bool saveCharacter(
        uint32_t accountId, 
        const std::string& characterData, 
        DBResponseCallback callback
    );
    
    // Lista de personagens (wrapper para operação comum)
    /**
 * getCharacterList
 * 
 * Implementa a funcionalidade getCharacterList conforme especificação original.
 * @param accountId Parâmetro accountId
 * @param callback Parâmetro callback
 * @return Retorna bool
 */

    bool getCharacterList(
        uint32_t accountId, 
        DBResponseCallback callback
    );
    
    // Criação de personagem (wrapper para operação comum)
    /**
 * createCharacter
 * 
 * Implementa a funcionalidade createCharacter conforme especificação original.
 * @param accountId Parâmetro accountId
 * @param characterData Parâmetro characterData
 * @param callback Parâmetro callback
 * @return Retorna bool
 */

    bool createCharacter(
        uint32_t accountId, 
        const std::string& characterData, 
        DBResponseCallback callback
    );
    
private:
    // Conexão com o DBSrv
    std::unique_ptr<DBServerConnection> connection_;
    
    // Thread para processamento assíncrono
    std::thread processingThread_;
    std::atomic<bool> threadRunning_;
    
    // Contador de sequência para operações
    std::atomic<uint32_t> sequenceCounter_;
    
    // Registro de operações pendentes
    struct PendingOperation {
        DBOperation operation;
        uint32_t sequence;
        std::chrono::steady_clock::time_point startTime;
        uint32_t timeoutMs;
        DBResponseCallback callback;
    };
    
    // Queue de operações pendentes
    std::map<uint32_t, PendingOperation> pendingOperations_;
    std::mutex operationMutex_;
    
    // Queue de envio
    std::queue<DBPacket> sendQueue_;
    std::mutex sendMutex_;
    std::condition_variable sendCondition_;
    
    // Métodos internos
    /**
 * processingLoop
 * 
 * Implementa a funcionalidade processingLoop conforme especificação original.
 */

    void processingLoop();
    /**
 * handleResponse
 * 
 * Implementa a funcionalidade handleResponse conforme especificação original.
 * @param response Parâmetro response
 */

    void handleResponse(const DBPacket& response);
    /**
 * cleanupTimedOutOperations
 * 
 * Implementa a funcionalidade cleanupTimedOutOperations conforme especificação original.
 */

    void cleanupTimedOutOperations();
    
    // Verificação de conexão periódica
    /**
 * checkConnection
 * 
 * Implementa a funcionalidade checkConnection conforme especificação original.
 */

    void checkConnection();
    std::chrono::steady_clock::time_point lastConnectionCheck_;
};

/**
 * Conexão com o servidor de banco de dados
 */
/**
 * Classe WYDDBServerConnection
 * 
 * Esta classe implementa a funcionalidade WYDDBServerConnection conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DBServerConnection {
public:
    // /**
 * DBServerConnection
 * 
 * Implementa a funcionalidade DBServerConnection conforme especificação original.
 * @return Retorna Construtor
 */
 Construtor
    DBServerConnection();
    
    // Destrutor
    ~DBServerConnection();
    
    // Conecta ao servidor
    /**
 * connect
 * 
 * Implementa a funcionalidade connect conforme especificação original.
 * @param host Parâmetro host
 * @param port Parâmetro port
 * @return Retorna bool
 */

    bool connect(const std::string& host, uint16_t port);
    
    // Desconecta do servidor
    /**
 * disconnect
 * 
 * Implementa a funcionalidade disconnect conforme especificação original.
 */

    void disconnect();
    
    // Verifica se está conectado
    /**
 * isConnected
 * 
 * Implementa a funcionalidade isConnected conforme especificação original.
 * @return Retorna bool
 */

    bool isConnected() const;
    
    // Envia um pacote
    /**
 * send
 * 
 * Implementa a funcionalidade send conforme especificação original.
 * @param packet Parâmetro packet
 * @return Retorna bool
 */

    bool send(const DBPacket& packet);
    
    // Recebe um pacote (com timeout)
    // Retorna true se um pacote foi recebido, false se timeout ou erro
    /**
 * receive
 * 
 * Implementa a funcionalidade receive conforme especificação original.
 * @param outPacket Parâmetro outPacket
 * @param 5000 Parâmetro 5000
 * @return Retorna bool
 */

    bool receive(DBPacket& outPacket, uint32_t timeoutMs = 5000);
    
private:
    // Socket TCP
    int socket_;
    
    // Endereço do servidor
    std::string host_;
    uint16_t port_;
    
    // Estado de conexão
    bool connected_;
    
    // Buffer de recepção
    std::string receiveBuffer_;
    
    // Mutex para acesso seguro
    mutable std::mutex mutex_;
};

#endif // DBSERVER_CONNECTOR_H

} // namespace wydbr
