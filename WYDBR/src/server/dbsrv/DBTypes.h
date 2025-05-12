/**
 * DBTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/dbsrv/DBTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef DBTYPES_H
#define DBTYPES_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstring>

#include "../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file DBTypes.h
 * @brief Definição dos tipos de dados do DBServer
 * 
 * Este arquivo contém as definições das estruturas de dados usadas pelo DBServer,
 * seguindo exatamente a estrutura do código original do WYD, mas com melhorias
 * de design, comentários e correções de bugs conhecidos.
 */

namespace wyd {
namespace server {

// Constantes de sistema do DB Server (exatamente como no WYD original)
#define MAX_CHARACTER         4           // Máximo de personagens por conta
#define MAX_GUILD             5000        // Máximo de guildas
#define MAX_GUILD_MEMBER      128         // Máximo de membros por guilda
#define MAX_GUILD_TAX         10          // Máximo de impostos por guilda
#define MAX_RANK              50          // Máximo de posições no ranking
#define MAX_GRIDPOP           10          // Máximo de população por célula do grid
#define MAX_LOG_LENGTH        1024        // Comprimento máximo de um log
#define MAX_USER_BLOCKLIST    20          // Máximo de bloqueios por usuário
#define MAX_EVENT             100         // Máximo de eventos por mapa

// Tempo de expiração de dados em cache (em milissegundos)
#define CACHE_ACCOUNT_EXPIRE  3600000     // 1 hora
#define CACHE_CHAR_EXPIRE     1800000     // 30 minutos
#define CACHE_GUILD_EXPIRE    7200000     // 2 horas
#define CACHE_ITEM_EXPIRE     600000      // 10 minutos

// Tipos de operações de query
enum DB_QUERY_TYPE {
    DB_QUERY_NONE = 0,            // Nenhuma query
    DB_QUERY_CREATE = 1,          // Criar
    DB_QUERY_READ = 2,            // Ler
    DB_QUERY_UPDATE = 3,          // Atualizar
    DB_QUERY_DELETE = 4,          // Remover
    DB_QUERY_LIST = 5,            // Listar
    DB_QUERY_SEARCH = 6,          // Buscar
    DB_QUERY_COUNT = 7,           // Contar
    DB_QUERY_CUSTOM = 8,          // Custom
};

// Tipos de operação em Lote
enum DB_BATCH_TYPE {
    DB_BATCH_NONE = 0,            // Nenhuma operação
    DB_BATCH_INSERT = 1,          // Inserção em lote
    DB_BATCH_UPDATE = 2,          // Atualização em lote
    DB_BATCH_DELETE = 3,          // Remoção em lote
};

// Tipos de entidades do banco
enum DB_ENTITY_TYPE {
    DB_ENTITY_NONE = 0,           // Nenhuma entidade
    DB_ENTITY_ACCOUNT = 1,        // Conta de usuário
    DB_ENTITY_CHARACTER = 2,      // Personagem
    DB_ENTITY_ITEM = 3,           // Item
    DB_ENTITY_INVENTORY = 4,      // Inventário
    DB_ENTITY_STORAGE = 5,        // Armazenamento (cargo)
    DB_ENTITY_GUILD = 6,          // Guilda
    DB_ENTITY_SKILL = 7,          // Habilidade
    DB_ENTITY_QUEST = 8,          // Missão
    DB_ENTITY_EVENT = 9,          // Evento
    DB_ENTITY_MARKET = 10,        // Mercado
    DB_ENTITY_BILLING = 11,       // Cobrança
    DB_ENTITY_LOG = 12,           // Log
    DB_ENTITY_MISC = 13,          // Diversos
    DB_ENTITY_RANKING = 14,       // Ranking
    DB_ENTITY_SYSTEM = 15,        // Sistema
};

// Estados da transação
enum DB_TRANSACTION_STATE {
    DB_TRANS_NONE = 0,            // Sem transação
    DB_TRANS_STARTED = 1,         // Transação iniciada
    DB_TRANS_COMMITTED = 2,       // Transação confirmada
    DB_TRANS_ROLLEDBACK = 3,      // Transação revertida
    DB_TRANS_FAILED = 4,          // Transação falhou
};

// Estados da conexão com o banco
enum DB_CONNECTION_STATE {
    DB_CONN_DISCONNECTED = 0,     // Desconectado
    DB_CONN_CONNECTING = 1,       // Conectando
    DB_CONN_CONNECTED = 2,        // Conectado
    DB_CONN_FAILED = 3,           // Falha na conexão
    DB_CONN_RECONNECTING = 4,     // Reconectando
};

// Tipos de logs
enum DB_LOG_TYPE {
    DB_LOG_NONE = 0,              // Nenhum log
    DB_LOG_LOGIN = 1,             // Login
    DB_LOG_ITEM = 2,              // Operação com item
    DB_LOG_GOLD = 3,              // Operação com gold
    DB_LOG_KILL = 4,              // Morte de personagem
    DB_LOG_TRADE = 5,             // Comércio
    DB_LOG_GUILD = 6,             // Operação de guilda
    DB_LOG_SKILL = 7,             // Uso de habilidade
    DB_LOG_QUEST = 8,             // Progresso de missão
    DB_LOG_SHOP = 9,              // Compra/venda em loja
    DB_LOG_SYSTEM = 10,           // Sistema
    DB_LOG_ADMIN = 11,            // Ação de administrador
    DB_LOG_ERROR = 12,            // Erro
    DB_LOG_HACK = 13,             // Suspeita de hack
    DB_LOG_GM = 14,               // Comando de GM
};

/**
 * @brief Estrutura de cabeçalho de query do DB (original: DB_PACKET)
 * 
 * Esta estrutura corresponde ao cabeçalho usado em todas as comunicações
 * entre o TMServer e o DBServer no WYD original.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_DB_PACKET {
    WORD wSize;                   // Tamanho total do pacote
    BYTE byType;                  // Tipo de pacote
    BYTE byResult;                // Resultado da operação
    DWORD dwClientTick;           // Timestamp do cliente
    DWORD dwServerTick;           // Timestamp do servidor
    
    STRUCT_DB_PACKET()
        : wSize(sizeof(STRUCT_DB_PACKET))
        , byType(0)
        , byResult(0)
        , dwClientTick(0)
        , dwServerTick(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de informações de conta (original: ACCOUNT_INFO)
 * 
 * Esta estrutura armazena informações sobre uma conta no WYD.
 * Corresponde aos dados armazenados na tabela de contas.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_ACCOUNTINFO {
    char   szLogin[32];           // Nome de login
    char   szPassword[40];        // Senha (hash)
    BYTE   byAccountStatus;       // Status da conta (0: normal, 1: bloqueada, etc.)
    BYTE   byGMLevel;             // Nível de GM (0: usuário comum)
    DWORD  dwLastLoginTime;       // Último login (timestamp)
    char   szLastIP[16];          // Último IP
    DWORD  dwBanTime;             // Tempo de ban (0: não banida)
    BYTE   byNumChars;            // Número de personagens
    BYTE   byIsOnline;            // Se está online
    WORD   wPremiumType;          // Tipo de conta premium
    DWORD  dwPremiumTime;         // Tempo restante de premium (em segundos)
    DWORD  dwCreateTime;          // Data de criação da conta
    char   szEmail[64];           // Email
    WORD   wCashPoints;           // Pontos de cash
    BYTE   bySecretQuestion;      // Pergunta secreta (índice)
    char   szSecretAnswer[32];    // Resposta à pergunta secreta (hash)
    
    STRUCT_ACCOUNTINFO() {
        memset(this, 0, sizeof(STRUCT_ACCOUNTINFO));
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de informações de personagem (original: CHAR_INFO)
 * 
 * Esta estrutura armazena informações básicas sobre um personagem no WYD.
 * Corresponde aos dados armazenados na tabela de personagens.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_CHARINFO {
    DWORD  dwCharID;              // ID do personagem
    char   szName[16];            // Nome do personagem
    BYTE   byClass;               // Classe
    BYTE   byFace;                // Rosto
    BYTE   byLevel;               // Nível
    DWORD  dwExperience;          // Experiência
    DWORD  dwHP;                  // HP atual
    DWORD  dwMP;                  // MP atual
    WORD   wStr;                  // Força
    WORD   wInt;                  // Inteligência
    WORD   wDex;                  // Destreza
    WORD   wCon;                  // Constituição
    WORD   wWis;                  // Sabedoria (não usado no WYD)
    WORD   wStat;                 // Pontos de atributos não distribuídos
    WORD   wSkillPoint;           // Pontos de habilidade não distribuídos
    DWORD  dwGold;                // Gold
    WORD   wMapID;                // ID do mapa
    WORD   wPosX;                 // Posição X
    WORD   wPosY;                 // Posição Y
    BYTE   byGuildLevel;          // Nível na guilda (0: membro, 1: sub-líder, etc.)
    DWORD  dwGuildID;             // ID da guilda
    char   szGuildName[12];       // Nome da guilda
    DWORD  dwLastLoginTime;       // Último login (timestamp)
    BYTE   byOnlineStatus;        // Status online (0: offline)
    BYTE   byIsDeleted;           // Se está marcado para deleçã/**
 * STRUCT_CHARINFO
 * 
 * Implementa a funcionalidade STRUCT_CHARINFO conforme especificação original.
 * @return Retorna o
 */
o
    
    STRUCT_CHARINFO(){
        memset(this, 0, sizeof(STRUCT_CHARINFO));
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de informações de guilda (original: GUILD_INFO)
 * 
 * Esta estrutura armazena informações sobre uma guilda no WYD.
 * Corresponde aos dados armazenados na tabela de guildas.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_GUILDINFO {
    DWORD  dwGuildID;             // ID da guilda
    char   szGuildName[12];       // Nome da guilda
    char   szGuildMark[16];       // Marca da guilda (dados da imagem)
    char   szGuildNotice[256];    // Aviso da guilda
    DWORD  dwGuildMoney;          // Dinheiro da guilda
    DWORD  dwAllianceID;          // ID da aliança
    BYTE   byLevel;               // Nível da guilda
    DWORD  dwFame;                // Fama da guilda
    DWORD  dwCreationTime;        // Data de criação da guilda
    DWORD  dwLeaderID;            // ID do líder
    char   szLeaderName[16];      // Nome do líder
    WORD   wMembers;              // Número de membros
    BYTE   byStatus;              // Status da guilda (0: normal, 1: em guerra, etc.)
    DWORD  dwGuildScore;          // Pontuação da /**
 * STRUCT_GUILDINFO
 * 
 * Implementa a funcionalidade STRUCT_GUILDINFO conforme especificação original.
 * @return Retorna guilda
 */
 guilda
    
    STRUCT_GUILDINFO(){
        memset(this, 0, sizeof(STRUCT_GUILDINFO));
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de membro de guilda (original: GUILD_MEMBER)
 * 
 * Esta estrutura armazena informações sobre um membro de guilda no WYD.
 * Corresponde aos dados armazenados na tabela de membros de guildas.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_GUILDMEMBER {
    DWORD  dwGuildID;             // ID da guilda
    DWORD  dwCharID;              // ID do personagem
    char   szCharName[16];        // Nome do personagem
    BYTE   byClass;               // Classe
    BYTE   byLevel;               // Nível
    BYTE   byGuildLevel;          // Nível na guilda (0: membro)
    DWORD  dwContribution;        // Contribuição para a guilda
    DWORD  dwLastLoginTime;       // Último login (timestamp)
    BYTE   byIsOnline;            // Se está /**
 * STRUCT_GUILDMEMBER
 * 
 * Implementa a funcionalidade STRUCT_GUILDMEMBER conforme especificação original.
 * @return Retorna online
 */
 online
    
    STRUCT_GUILDMEMBER(){
        memset(this, 0, sizeof(STRUCT_GUILDMEMBER));
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de ranking (original: RANK_DATA)
 * 
 * Esta estrutura armazena informações de ranking no WYD.
 * Corresponde aos dados armazenados nas tabelas de ranking.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_RANKDATA {
    DWORD  dwCharID;              // ID do personagem/guilda
    char   szName[16];            // Nome do personagem/guilda
    BYTE   byType;                // Tipo de ranking (0: nível, 1: fama, etc.)
    DWORD  dwValue;               // Valor do ranking
    WORD   wRank;                 // Posição no ranking
    BYTE   byClass;               // Classe (para personagens)
    BYTE   byLevel;               // Nível (para guildas)
    DWORD  dwGuildID;             // ID da guilda (para personagens)
    
    STRUCT_RANKDATA() {
        memset(this, 0, sizeof(STRUCT_RANKDATA));
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de log (original: LOG_DATA)
 * 
 * Esta estrutura armazena informações de log no WYD.
 * Corresponde aos dados armazenados nas tabelas de log.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_LOGDATA {
    DWORD  dwLogID;               // ID do log
    DWORD  dwAccountID;           // ID da conta
    DWORD  dwCharID;              // ID do personagem
    BYTE   byLogType;             // Tipo de log
    DWORD  dwDateTime;            // Data/hora do log
    char   szIPAddress[16];       // Endereço IP
    char   szLog[MAX_LOG_LENGTH]; // Dados do /**
 * STRUCT_LOGDATA
 * 
 * Implementa a funcionalidade STRUCT_LOGDATA conforme especificação original.
 * @return Retorna log
 */
 log
    
    STRUCT_LOGDATA(){
        memset(this, 0, sizeof(STRUCT_LOGDATA));
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de query (original: DB_QUERY)
 * 
 * Esta estrutura representa uma query enviada ao DBServer no WYD.
 * Usada internamente para gerenciar operações de banco de dados.
 */
struct STRUCT_DBQUERY {
    DB_QUERY_TYPE queryType;      // Tipo de query
    DB_ENTITY_TYPE entityType;    // Tipo de entidade
    std::string queryString;      // String da query SQL
    DWORD dwTransactionID;        // ID da transação (0: sem transação)
    DWORD dwTimeout;              // Timeout da query (em ms)
    bool isAsync;                 // Se é assíncrona
    
    STRUCT_DBQUERY()
        : queryType(DB_QUERY_NONE)
        , entityType(DB_ENTITY_NONE)
        , dwTransactionID(0)
        , dwTimeout(5000)         // 5 segundos de timeout padrão
        , isAsync(false)
    {
    }
};

/**
 * @brief Estrutura de resultado de query (original: DB_RESULT)
 * 
 * Esta estrutura representa o resultado de uma query no WYD.
 * Usada internamente para gerenciar resultados de operações de banco de dados.
 */
struct STRUCT_DBRESULT {
    bool success;                 // Se a query foi bem sucedida
    DWORD dwErrorCode;            // Código de erro (0: sem erro)
    std::string errorMessage;     // Mensagem de erro
    DWORD dwAffectedRows;         // Número de linhas afetadas
    DWORD dwLastInsertID;         // ID da última inserção
    DWORD dwExecutionTime;        // Tempo de execução da query (em ms)
    
    // Resultados da query (para SELECT)
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> columns;
    
    STRUCT_DBRESULT()
        : success(false)
        , dwErrorCode(0)
        , dwAffectedRows(0)
        , dwLastInsertID(0)
        , dwExecutionTime(0)
    {
    }
};

/**
 * @brief Estrutura de transação (original: DB_TRANSACTION)
 * 
 * Esta estrutura representa uma transação no WYD.
 * Usada internamente para gerenciar transações no banco de dados.
 */
struct STRUCT_DBTRANSACTION {
    DWORD dwTransactionID;        // ID da transação
    DB_TRANSACTION_STATE state;   // Estado da transação
    DWORD dwStartTime;            // Hora de início (timestamp)
    DWORD dwEndTime;              // Hora de término (timestamp)
    DWORD dwTimeout;              // Timeout da transação (em ms)
    
    STRUCT_DBTRANSACTION()
        : dwTransactionID(0)
        , state(DB_TRANS_NONE)
        , dwStartTime(0)
        , dwEndTime(0)
        , dwTimeout(30000)        // 30 segundos de timeout padrão
    {
    }
};

/**
 * @brief Estrutura de cache (original: DB_CACHE_ENTRY)
 * 
 * Esta estrutura representa uma entrada de cache no WYD.
 * Usada internamente para gerenciar o cache de dados.
 */
struct STRUCT_DBCACHEENTRY {
    DB_ENTITY_TYPE entityType;    // Tipo de entidade
    DWORD dwEntityID;             // ID da entidade
    DWORD dwLastAccessTime;       // Último acesso (timestamp)
    DWORD dwExpirationTime;       // Tempo de expiração (timestamp)
    bool isDirty;                 // Se foi modificado desde a última sincronização
    DWORD dwLockCount;            // Contador de locks (para acesso concorrente)
    DWORD dwLockOwner;            // ID do owner do lock (thread/cliente)
    
    // Dados em cache
    void* pData;                  // Ponteiro para os dados
    size_t dataSize;              // Tamanho dos dados
    
    STRUCT_DBCACHEENTRY()
        : entityType(DB_ENTITY_NONE)
        , dwEntityID(0)
        , dwLastAccessTime(0)
        , dwExpirationTime(0)
        , isDirty(false)
        , dwLockCount(0)
        , dwLockOwner(0)
        , pData(nullptr)
        , dataSize(0)
    {
    }
};

/**
 * @brief Eventos do DBServer
 * 
 * Enumeração que lista os eventos que podem ser disparados pelo DBServer.
 * No WYD original, estes eram definidos como constantes em várias partes do código.
 */
enum DB_EVENT {
    DB_EVENT_NONE = 0,              // Nenhum evento
    DB_EVENT_CONNECTED = 1,         // Conectado ao banco
    DB_EVENT_DISCONNECTED = 2,      // Desconectado do banco
    DB_EVENT_QUERY_COMPLETED = 3,   // Query concluída
    DB_EVENT_QUERY_ERROR = 4,       // Erro na query
    DB_EVENT_TRANSACTION_COMPLETED = 5, // Transação concluída
    DB_EVENT_TRANSACTION_ERROR = 6, // Erro na transação
    DB_EVENT_CACHE_EVICTED = 7,     // Cache removido
    DB_EVENT_CACHE_SYNCED = 8,      // Cache sincronizado
    DB_EVENT_ACCOUNT_CREATED = 9,   // Conta criada
    DB_EVENT_ACCOUNT_UPDATED = 10,  // Conta atualizada
    DB_EVENT_ACCOUNT_DELETED = 11,  // Conta removida
    DB_EVENT_CHAR_CREATED = 12,     // Personagem criado
    DB_EVENT_CHAR_UPDATED = 13,     // Personagem atualizado
    DB_EVENT_CHAR_DELETED = 14,     // Personagem removido
    DB_EVENT_GUILD_CREATED = 15,    // Guilda criada
    DB_EVENT_GUILD_UPDATED = 16,    // Guilda atualizada
    DB_EVENT_GUILD_DELETED = 17,    // Guilda removida
    DB_EVENT_GUILD_MEMBER_ADDED = 18, // Membro adicionado à guilda
    DB_EVENT_GUILD_MEMBER_REMOVED = 19, // Membro removido da guilda
    DB_EVENT_RANK_UPDATED = 20,     // Ranking atualizado
    DB_EVENT_LOG_ADDED = 21,        // Log adicionado
    DB_EVENT_BILLING_UPDATED = 22,  // Cobrança atualizada
    DB_EVENT_CONFIG_LOADED = 23,    // Configuração carregada
    DB_EVENT_CONFIG_SAVED = 24,     // Configuração salva
    DB_EVENT_SHUTDOWN = 25,         // Servidor em desligamento
    DB_EVENT_STARTUP = 26,          // Servidor iniciando
};

} // namespace server
} // namespace wyd

#endif // DBTYPES_H

} // namespace wydbr
