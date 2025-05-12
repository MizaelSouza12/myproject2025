/**
 * DBPacketManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/dbsrv/DBPacketManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef DBPACKETMANAGER_H
#define DBPACKETMANAGER_H

#include <vector>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>

#include "DBTypes.h"
#include "../../core/WYDTypes.h"
#include "../../core/Logger.h"
#include "../../network/PacketTypes.h"

namespace wydbr {


/**
 * @file DBPacketManager.h
 * @brief Gerenciador de pacotes de comunicação do DBServer
 * 
 * Este arquivo contém a definição do gerenciador de pacotes de comunicação
 * entre o DBServer e os TMServers. No WYD original, esta comunicação era
 * realizada através de um protocolo específico com pacotes binários.
 */

namespace wyd {
namespace server {

// Forward declarations
class CDBConnectionManager;
class CDBCacheManager;

// Tipos de pacotes do DB Server (correspondem exatamente aos usados no WYD original)
enum DB_PACKET_TYPE : BYTE {
    DB_PKT_NONE = 0,                // Nenhum pacote
    DB_PKT_PING = 1,                // Ping
    DB_PKT_CONNECT = 2,             // Conectar
    DB_PKT_DISCONNECT = 3,          // Desconectar
    
    DB_PKT_ACCOUNT_LOGIN = 10,      // Login de conta
    DB_PKT_ACCOUNT_CREATE = 11,     // Criar conta
    DB_PKT_ACCOUNT_UPDATE = 12,     // Atualizar conta
    DB_PKT_ACCOUNT_DELETE = 13,     // Deletar conta
    DB_PKT_ACCOUNT_BAN = 14,        // Banir conta
    DB_PKT_ACCOUNT_UNBAN = 15,      // Desbanir conta
    DB_PKT_ACCOUNT_PREMIUM = 16,    // Alteração de status premium
    
    DB_PKT_CHAR_LIST = 20,          // Listar personagens
    DB_PKT_CHAR_CREATE = 21,        // Criar personagem
    DB_PKT_CHAR_DELETE = 22,        // Deletar personagem
    DB_PKT_CHAR_LOAD = 23,          // Carregar personagem
    DB_PKT_CHAR_SAVE = 24,          // Salvar personagem
    DB_PKT_CHAR_POSITION = 25,      // Atualizar posição
    DB_PKT_CHAR_RENAME = 26,        // Renomear personagem
    DB_PKT_CHAR_LOGOUT = 27,        // Logout de personagem
    
    DB_PKT_ITEM_LOAD = 30,          // Carregar item
    DB_PKT_ITEM_SAVE = 31,          // Salvar item
    DB_PKT_ITEM_DELETE = 32,        // Deletar item
    DB_PKT_ITEM_UPDATE = 33,        // Atualizar item
    DB_PKT_INVENTORY_LOAD = 34,     // Carregar inventário
    DB_PKT_INVENTORY_SAVE = 35,     // Salvar inventário
    DB_PKT_STORAGE_LOAD = 36,       // Carregar armazenamento
    DB_PKT_STORAGE_SAVE = 37,       // Salvar armazenamento
    
    DB_PKT_GUILD_LIST = 40,         // Listar guildas
    DB_PKT_GUILD_CREATE = 41,       // Criar guilda
    DB_PKT_GUILD_DELETE = 42,       // Deletar guilda
    DB_PKT_GUILD_LOAD = 43,         // Carregar guilda
    DB_PKT_GUILD_SAVE = 44,         // Salvar guilda
    DB_PKT_GUILD_MEMBER_ADD = 45,   // Adicionar membro à guilda
    DB_PKT_GUILD_MEMBER_REMOVE = 46,// Remover membro da guilda
    DB_PKT_GUILD_MEMBER_UPDATE = 47,// Atualizar membro da guilda
    DB_PKT_GUILD_ALLY = 48,         // Aliança entre guildas
    DB_PKT_GUILD_WAR = 49,          // Guerra entre guildas
    
    DB_PKT_SKILL_LOAD = 50,         // Carregar habilidades
    DB_PKT_SKILL_SAVE = 51,         // Salvar habilidades
    
    DB_PKT_QUEST_LOAD = 60,         // Carregar missões
    DB_PKT_QUEST_SAVE = 61,         // Salvar missões
    DB_PKT_QUEST_UPDATE = 62,       // Atualizar missão
    
    DB_PKT_RANK_LIST = 70,          // Listar rankings
    DB_PKT_RANK_UPDATE = 71,        // Atualizar ranking
    
    DB_PKT_LOG_ADD = 80,            // Adicionar log
    DB_PKT_LOG_QUERY = 81,          // Consultar logs
    
    DB_PKT_BILLING_CHECK = 90,      // Verificar cobrança
    DB_PKT_BILLING_UPDATE = 91,     // Atualizar cobrança
    
    DB_PKT_ADMIN_COMMAND = 100,     // Comando administrativo
    DB_PKT_ADMIN_BROADCAST = 101,   // Broadcast administrativo
    
    DB_PKT_SHUTDOWN = 110,          // Desligar servidor
    DB_PKT_CONFIG = 111,            // Configuração
    
    DB_PKT_MAX = 255                // Máximo (para verificações)
};

/**
 * @brief Resultado do processamento de pacote
 */
enum DB_PACKET_RESULT {
    PACKET_SUCCESS = 0,           // Sucesso
    PACKET_ERROR_INVALID = 1,     // Pacote inválido
    PACKET_ERROR_AUTH = 2,        // Erro de autenticação
    PACKET_ERROR_DB = 3,          // Erro de banco de dados
    PACKET_ERROR_PARAMS = 4,      // Parâmetros inválidos
    PACKET_ERROR_NOTFOUND = 5,    // Não encontrado
    PACKET_ERROR_INTERNAL = 6,    // Erro interno
    PACKET_ERROR_OVERLOAD = 7,    // Servidor sobrecarregado
    PACKET_ERROR_TIMEOUT = 8,     // Timeout
    PACKET_ERROR_INVALID_STATE = 9// Estado inválido
};

/**
 * @brief Callback para processamento de pacotes
 */
typedef std::function<void(STRUCT_DB_PACKET*, const BYTE*, int, BYTE*, int&)> DBPacketHandler;

/**
 * @brief Callback para eventos de pacote
 */
typedef std::function<void(DB_PACKET_TYPE, DB_PACKET_RESULT, DWORD, DWORD)> DBPacketEventCallback;

/**
 * @brief Gerenciador de pacotes do DBServer
 * 
 * Esta classe gerencia o processamento de pacotes de comunicação entre
 * o DBServer e os TMServers. No WYD original, este processamento era
 * feito em funções espalhadas por vários arquivos.
 */
/**
 * Classe WYDDBPacketManager
 * 
 * Esta classe implementa a funcionalidade WYDDBPacketManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CDBPacketManager {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static CDBPacketManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador de pacotes
     * @param numWorkers Número de workers para processamento
     * @return true se inicializado com sucesso
     */
    bool Initialize(int numWorkers = 4);
    
    /**
     * @brief Finaliza o gerenciador de pacotes
     */
    void Shutdown();
    
    /**
     * @brief Processa um pacote recebido
     * @param packet Cabeçalho do pacote
     * @param data Dados do pacote
     * @param dataSize Tamanho dos dados
     * @param responseData Buffer para resposta
     * @param responseSize Tamanho da resposta
     * @return Resultado do processamento
     */
    DB_PACKET_RESULT ProcessPacket(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, 
                                  BYTE* responseData, int& responseSize);
    
    /**
     * @brief Enfileira um pacote para processamento assíncrono
     * @param packet Cabeçalho do pacote
     * @param data Dados do pacote
     * @param dataSize Tamanho dos dados
     * @param clientId ID do cliente (TMServer)
     * @return true se enfileirado com sucesso
     */
    bool EnqueuePacket(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, DWORD clientId);
    
    /**
     * @brief Registra um handler para um tipo de pacote
     * @param packetType Tipo de pacote
     * @param handler Função de handler
     */
    void RegisterPacketHandler(DB_PACKET_TYPE packetType, const DBPacketHandler& handler);
    
    /**
     * @brief Registra um callback para eventos de pacote
     * @param callback Função de callback
     */
    void RegisterPacketEventCallback(const DBPacketEventCallback& callback);
    
    /**
     * @brief Envia um pacote para um cliente
     * @param clientId ID do cliente (TMServer)
     * @param packetType Tipo de pacote
     * @param data Dados do pacote
     * @param dataSize Tamanho dos dados
     * @return true se enviado com sucesso
     */
    bool SendPacket(DWORD clientId, DB_PACKET_TYPE packetType, const BYTE* data, int dataSize);
    
    /**
     * @brief Cria um pacote de resposta
     * @param originalPacket Pacote original
     * @param result Resultado do processamento
     * @param data Dados da resposta
     * @param dataSize Tamanho dos dados
     * @return true se criado com sucesso
     */
    bool CreateResponsePacket(STRUCT_DB_PACKET* originalPacket, DB_PACKET_RESULT result, 
                              BYTE* data, int& dataSize);
    
    /**
     * @brief Cria um pacote de erro
     * @param originalPacket Pacote original
     * @param errorCode Código de erro
     * @param errorMessage Mensagem de erro
     * @param data Buffer para o pacote
     * @param dataSize Tamanho do pacote
     * @return true se criado com sucesso
     */
    bool CreateErrorPacket(STRUCT_DB_PACKET* originalPacket, DWORD errorCode, 
                          const char* errorMessage, BYTE* data, int& dataSize);
    
    /**
     * @brief Notifica um evento de pacote
     * @param packetType Tipo de pacote
     * @param result Resultado do processamento
     * @param clientId ID do cliente
     * @param processTime Tempo de processamento (ms)
     */
    void NotifyPacketEvent(DB_PACKET_TYPE packetType, DB_PACKET_RESULT result, 
                          DWORD clientId, DWORD processTime);
    
    /**
     * @brief Obtém estatísticas de pacotes
     * @return String com estatísticas
     */
    std::string GetPacketStats() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CDBPacketManager();
    
    /**
     * @brief Destrutor
     */
    ~CDBPacketManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CDBPacketManager(const CDBPacketManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CDBPacketManager& operator=(const CDBPacketManager&) = delete;
    
    /**
     * @brief Thread de worker para processamento de pacotes
     */
    void WorkerThread();
    
    /**
     * @brief Estrutura de tarefa de processamento de pacote
     */
    struct PacketTask {
        STRUCT_DB_PACKET packet;    // Cabeçalho do pacote
        std::vector<BYTE> data;     // Dados do pacote
        DWORD clientId;             // ID do cliente
        DWORD enqueueTime;          // Hora de entrada na fila
        
        PacketTask()
            : clientId(0)
            , enqueueTime(0)
        {
        }
    };
    
    // Queue de pacotes
    std::queue<PacketTask> packetQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    
    // Handlers de pacotes
    std::unordered_map<DB_PACKET_TYPE, DBPacketHandler> packetHandlers_;
    std::mutex handlersMutex_;
    
    // Callbacks para eventos
    std::vector<DBPacketEventCallback> eventCallbacks_;
    std::mutex callbackMutex_;
    
    // Threads de worker
    std::vector<std::thread> workerThreads_;
    std::atomic<bool> shutdownFlag_;
    
    // Estatísticas
    struct PacketStats {
        DWORD dwTotalPackets;                // Total de pacotes processados
        DWORD dwSuccessfulPackets;           // Pacotes processados com sucesso
        DWORD dwFailedPackets;               // Pacotes com erro
        DWORD dwTotalProcessingTime;         // Tempo total de processamento (ms)
        DWORD dwMaxProcessingTime;           // Tempo máximo de processamento (ms)
        DWORD dwMinProcessingTime;           // Tempo mínimo de processamento (ms)
        std::unordered_map<DB_PACKET_TYPE, DWORD> packetCounts; // Contagem por tipo
        
        PacketStats()
            : dwTotalPackets(0)
            , dwSuccessfulPackets(0)
            , dwFailedPackets(0)
            , dwTotalProcessingTime(0)
            , dwMaxProcessingTime(0)
            , dwMinProcessingTime(0xFFFFFFFF)
        {
        }
    };
    
    mutable std::mutex statsMutex_;
    PacketStats stats_;
    
    bool initialized_;
    
    // Handlers de pacotes específicos
    /**
 * InitializeDefaultHandlers
 * 
 * Implementa a funcionalidade InitializeDefaultHandlers conforme especificação original.
 */

    void InitializeDefaultHandlers();
    /**
 * HandleAccountLogin
 * 
 * Implementa a funcionalidade HandleAccountLogin conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleAccountLogin(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
    /**
 * HandleAccountCreate
 * 
 * Implementa a funcionalidade HandleAccountCreate conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleAccountCreate(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
    /**
 * HandleCharCreate
 * 
 * Implementa a funcionalidade HandleCharCreate conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleCharCreate(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
    /**
 * HandleCharLoad
 * 
 * Implementa a funcionalidade HandleCharLoad conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleCharLoad(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
    /**
 * HandleCharSave
 * 
 * Implementa a funcionalidade HandleCharSave conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleCharSave(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
    /**
 * HandleInventoryLoad
 * 
 * Implementa a funcionalidade HandleInventoryLoad conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleInventoryLoad(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
    /**
 * HandleInventorySave
 * 
 * Implementa a funcionalidade HandleInventorySave conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleInventorySave(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
    /**
 * HandleGuildLoad
 * 
 * Implementa a funcionalidade HandleGuildLoad conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleGuildLoad(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
    /**
 * HandleGuildSave
 * 
 * Implementa a funcionalidade HandleGuildSave conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleGuildSave(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
    /**
 * HandleAdminCommand
 * 
 * Implementa a funcionalidade HandleAdminCommand conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 * @param dataSize Parâmetro dataSize
 * @param responseData Parâmetro responseData
 * @param responseSize Parâmetro responseSize
 */

    void HandleAdminCommand(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize);
};

// Definição global para facilitar o acesso
#define g_DBPacket CDBPacketManager::GetInstance()

} // namespace server
} // namespace wyd

#endif // DBPACKETMANAGER_H

} // namespace wydbr
