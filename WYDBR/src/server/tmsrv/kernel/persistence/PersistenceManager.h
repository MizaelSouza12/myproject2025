/**
 * PersistenceManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/kernel/persistence/PersistenceManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PERSISTENCEMANAGER_H
#define PERSISTENCEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>

#include "../../../../core/WYDTypes.h"
#include "../KernelTypes.h"
#include "../resources/ResourceManager.h"

namespace wydbr {


/**
 * @file PersistenceManager.h
 * @brief Gerenciador de persistência robusto
 * 
 * Este arquivo contém a definição do gerenciador de persistência avançado,
 * que implementa mecanismos seguros para salvar e carregar dados do jogo,
 * com proteção contra corrupção, verificação de integridade e estratégias
 * de recuperação de falhas.
 */

namespace wyd {
namespace server {
namespace kernel {
namespace persistence {

/**
 * @brief Configuração do gerenciador de persistência
 */
struct PersistenceManagerConfig {
    bool useJournaling;                   // Usar journaling
    bool useTransactions;                 // Usar transações
    bool useCheckpoints;                  // Usar checkpoints
    bool useDifferentialSaving;           // Usar salvamento diferencial
    bool useCompression;                  // Usar compressão
    bool useEncryption;                   // Usar criptografia
    bool useBackups;                      // Usar backups
    bool validateIntegrity;               // Validar integridade
    bool useAsyncWriting;                 // Usar escrita assíncrona
    DWORD checkpointInterval;            // Intervalo de checkpoint (ms)
    DWORD backupInterval;                // Intervalo de backup (ms)
    DWORD journalFlushInterval;          // Intervalo de flush de journal (ms)
    DWORD asyncThreadPoolSize;           // Tamanho do pool de threads assíncronas
    std::string backupDirectory;          // Diretório de backup
    CompressionAlgorithm compression;    // Algoritmo de compressão
    EncryptionAlgorithm encryption;      // Algoritmo de criptografia
    HashingAlgorithm hashing;            // Algoritmo de hashing
    
    PersistenceManagerConfig()
        : useJournaling(true)
        , useTransactions(true)
        , useCheckpoints(true)
        , useDifferentialSaving(true)
        , useCompression(true)
        , useEncryption(true)
        , useBackups(true)
        , validateIntegrity(true)
        , useAsyncWriting(true)
        , checkpointInterval(300000) // 5 minutos
        , backupInterval(3600000) // 1 hora
        , journalFlushInterval(5000) // 5 segundos
        , asyncThreadPoolSize(4)
        , backupDirectory("./backups")
        , compression(CompressionAlgorithm::ZSTD)
        , encryption(EncryptionAlgorithm::AES)
        , hashing(HashingAlgorithm::SHA256)
    {
    }
};

/**
 * @brief Opções de operação de persistência
 */
struct PersistenceOperationOptions {
    PersistenceStrategy strategy;         // Estratégia
    PersistenceCriticality criticality;   // Criticidade
    bool async;                            // Assíncrono
    bool compress;                         // Comprimir
    bool encrypt;                          // Criptografar
    bool keepJournal;                      // Manter journal
    DWORD retryCount;                     // Contagem de tentativas
    DWORD retryInterval;                  // Intervalo de tentativas (ms)
    bool validateBeforeSave;               // Validar antes de salvar
    bool validateAfterLoad;                // Validar após carregar
    HashingAlgorithm hashAlgorithm;       // Algoritmo de hashing
    
    PersistenceOperationOptions()
        : strategy(PersistenceStrategy::IMMEDIATE)
        , criticality(PersistenceCriticality::MEDIUM)
        , async(true)
        , compress(true)
        , encrypt(true)
        , keepJournal(true)
        , retryCount(3)
        , retryInterval(1000)
        , validateBeforeSave(true)
        , validateAfterLoad(true)
        , hashAlgorithm(HashingAlgorithm::SHA256)
    {
    }
};

/**
 * @brief Resultado de operação de persistência
 */
struct PersistenceOperationResult {
    DWORD operationID;                   // ID da operação
    PersistenceStatus status;            // Status
    std::string error;                     // Erro
    DWORD operationTimeMs;               // Tempo de operação (ms)
    size_t dataSize;                      // Tamanho dos dados
    bool wasRecovered;                     // Se foi recuperado
    bool fromBackup;                       // Se veio de backup
    std::string hash;                      // Hash
    
    PersistenceOperationResult()
        : operationID(0)
        , status(PersistenceStatus::COMPLETED)
        , error("")
        , operationTimeMs(0)
        , dataSize(0)
        , wasRecovered(false)
        , fromBackup(false)
        , hash("")
    {
    }
};

/**
 * @brief Callback de operação de persistência
 */
using PersistenceOperationCallback = std::function<void(const PersistenceOperationResult&)>;

/**
 * @brief Gerenciador de persistência
 */
/**
 * Classe WYDPersistenceManager
 * 
 * Esta classe implementa a funcionalidade WYDPersistenceManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PersistenceManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static PersistenceManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const PersistenceManagerConfig& config);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Salva dados
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param targetPath Caminho alvo
     * @param options Opções
     * @param callback Callback
     * @return ID da operação, ou 0 se falhar
     */
    DWORD SaveData(const void* data, size_t dataSize, const std::string& targetPath,
                  const PersistenceOperationOptions& options = PersistenceOperationOptions(),
                  PersistenceOperationCallback callback = nullptr);
    
    /**
     * @brief Carrega dados
     * @param targetPath Caminho alvo
     * @param options Opções
     * @param callback Callback
     * @return ID da operação, ou 0 se falhar
     */
    DWORD LoadData(const std::string& targetPath,
                  const PersistenceOperationOptions& options = PersistenceOperationOptions(),
                  PersistenceOperationCallback callback = nullptr);
    
    /**
     * @brief Carrega dados sincronamente
     * @param targetPath Caminho alvo
     * @param outData Dados (saída)
     * @param outDataSize Tamanho dos dados (saída)
     * @param options Opções
     * @return Resultado da operação
     */
    PersistenceOperationResult LoadDataSync(const std::string& targetPath, void** outData, size_t* outDataSize,
                                          const PersistenceOperationOptions& options = PersistenceOperationOptions());
    
    /**
     * @brief Obtém o status de uma operação
     * @param operationID ID da operação
     * @return Status
     */
    PersistenceStatus GetOperationStatus(DWORD operationID) const;
    
    /**
     * @brief Obtém o resultado de uma operação
     * @param operationID ID da operação
     * @return Resultado
     */
    PersistenceOperationResult GetOperationResult(DWORD operationID) const;
    
    /**
     * @brief Cancela uma operação
     * @param operationID ID da operação
     * @return true se cancelado com sucesso
     */
    bool CancelOperation(DWORD operationID);
    
    /**
     * @brief Cria um checkpoint
     * @param name Nome
     * @return true se criado com sucesso
     */
    bool CreateCheckpoint(const std::string& name);
    
    /**
     * @brief Restaura um checkpoint
     * @param name Nome
     * @return true se restaurado com sucesso
     */
    bool RestoreCheckpoint(const std::string& name);
    
    /**
     * @brief Obtém checkpoints disponíveis
     * @return Lista de nomes
     */
    std::vector<std::string> GetAvailableCheckpoints() const;
    
    /**
     * @brief Cria um backup
     * @param name Nome
     * @return true se criado com sucesso
     */
    bool CreateBackup(const std::string& name);
    
    /**
     * @brief Restaura um backup
     * @param name Nome
     * @return true se restaurado com sucesso
     */
    bool RestoreBackup(const std::string& name);
    
    /**
     * @brief Verifica a integridade de um arquivo
     * @param path Caminho
     * @return true se íntegro
     */
    bool VerifyFileIntegrity(const std::string& path);
    
    /**
     * @brief Obtém o hash de um arquivo
     * @param path Caminho
     * @param algorithm Algoritmo
     * @return Hash, ou string vazia se falhar
     */
    std::string GetFileHash(const std::string& path, HashingAlgorithm algorithm = HashingAlgorithm::SHA256);
    
    /**
     * @brief Registra um callback para eventos de persistência
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterEventCallback(PersistenceEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterEventCallback(int callbackID);
    
    /**
     * @brief Obtém estatísticas de persistência
     * @return Estatísticas
     */
    PersistenceStatistics GetStatistics() const;
    
    /**
     * @brief Inicia uma transação
     * @param transactionName Nome
     * @return ID da transação, ou 0 se falhar
     */
    DWORD BeginTransaction(const std::string& transactionName);
    
    /**
     * @brief Confirma uma transação
     * @param transactionID ID da transação
     * @return true se confirmado com sucesso
     */
    bool CommitTransaction(DWORD transactionID);
    
    /**
     * @brief Reverte uma transação
     * @param transactionID ID da transação
     * @return true se revertido com sucesso
     */
    bool RollbackTransaction(DWORD transactionID);
    
    /**
     * @brief Adiciona uma operação a uma transação
     * @param transactionID ID da transação
     * @param operationID ID da operação
     * @return true se adicionado com sucesso
     */
    bool AddOperationToTransaction(DWORD transactionID, DWORD operationID);
    
    /**
     * @brief Recupera um arquivo corrompido
     * @param path Caminho
     * @return true se recuperado com sucesso
     */
    bool RecoverCorruptedFile(const std::string& path);
    
    /**
     * @brief Cria uma fila de persistência
     * @param queueName Nome
     * @param flushInterval Intervalo de flush (ms)
     * @param strategy Estratégia
     * @return ID da fila, ou 0 se falhar
     */
    DWORD CreatePersistenceQueue(const std::string& queueName, DWORD flushInterval,
                               PersistenceStrategy strategy = PersistenceStrategy::BATCHED);
    
    /**
     * @brief Destrói uma fila de persistência
     * @param queueID ID da fila
     * @return true se destruído com sucesso
     */
    bool DestroyPersistenceQueue(DWORD queueID);
    
    /**
     * @brief Adiciona uma operação a uma fila
     * @param queueID ID da fila
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @param targetPath Caminho alvo
     * @param options Opções
     * @return ID da operação, ou 0 se falhar
     */
    DWORD QueuePersistenceOperation(DWORD queueID, const void* data, size_t dataSize,
                                  const std::string& targetPath,
                                  const PersistenceOperationOptions& options = PersistenceOperationOptions());
    
    /**
     * @brief Força o flush de uma fila
     * @param queueID ID da fila
     * @return true se flushed com sucesso
     */
    bool FlushPersistenceQueue(DWORD queueID);
    
    /**
     * @brief Obtém o tamanho de uma fila
     * @param queueID ID da fila
     * @return Tamanho
     */
    size_t GetPersistenceQueueSize(DWORD queueID) const;
    
    /**
     * @brief Define o caminho base de dados
     * @param path Caminho
     */
    void SetDataBasePath(const std::string& path);
    
    /**
     * @brief Obtém o caminho base de dados
     * @return Caminho
     */
    std::string GetDataBasePath() const;
    
    /**
     * @brief Cria um journal
     * @param journalName Nome
     * @param autoFlush Se deve fazer flush automático
     * @param flushInterval Intervalo de flush (ms)
     * @return ID do journal, ou 0 se falhar
     */
    DWORD CreateJournal(const std::string& journalName, bool autoFlush = true, DWORD flushInterval = 5000);
    
    /**
     * @brief Destrói um journal
     * @param journalID ID do journal
     * @return true se destruído com sucesso
     */
    bool DestroyJournal(DWORD journalID);
    
    /**
     * @brief Adiciona uma entrada a um journal
     * @param journalID ID do journal
     * @param entryType Tipo de entrada
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @return true se adicionado com sucesso
     */
    bool AddJournalEntry(DWORD journalID, const std::string& entryType, const void* data, size_t dataSize);
    
    /**
     * @brief Força o flush de um journal
     * @param journalID ID do journal
     * @return true se flushed com sucesso
     */
    bool FlushJournal(DWORD journalID);
    
    /**
     * @brief Lista todos os arquivos em um diretório
     * @param directory Diretório
     * @param pattern Padrão
     * @param recursive Se deve ser recursivo
     * @return Lista de caminhos
     */
    std::vector<std::string> ListFiles(const std::string& directory, const std::string& pattern = "*", bool recursive = false);
    
    /**
     * @brief Cria um diretório
     * @param directoryPath Caminho
     * @return true se criado com sucesso
     */
    bool CreateDirectory(const std::string& directoryPath);
    
    /**
     * @brief Remove um arquivo
     * @param filePath Caminho
     * @return true se removido com sucesso
     */
    bool RemoveFile(const std::string& filePath);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    PersistenceManager();
    
    /**
     * @brief Destrutor
     */
    ~PersistenceManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    PersistenceManager(const PersistenceManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    PersistenceManager& operator=(const PersistenceManager&) = delete;
    
    /**
     * @brief Thread de processamento
     */
    void ProcessingThread();
    
    /**
     * @brief Thread de update
     */
    void UpdateThread();
    
    /**
     * @brief Executa uma operação de salvamento
     * @param operationID ID da operação
     */
    void ExecuteSaveOperation(DWORD operationID);
    
    /**
     * @brief Executa uma operação de carregamento
     * @param operationID ID da operação
     */
    void ExecuteLoadOperation(DWORD operationID);
    
    /**
     * @brief Notifica callbacks de evento
     * @param eventType Tipo de evento
     * @param data Dados
     */
    void NotifyEventCallbacks(PersistenceEventType eventType, const void* data);
    
    /**
     * @brief Processa filas de persistência
     */
    void ProcessPersistenceQueues();
    
    /**
     * @brief Processa journals
     */
    void ProcessJournals();
    
    /**
     * @brief Cria um checkpoint periódico
     */
    void CreatePeriodicCheckpoint();
    
    /**
     * @brief Cria um backup periódico
     */
    void CreatePeriodicBackup();
    
    /**
     * @brief Cria o diretório de checkpoints
     * @return true se criado com sucesso
     */
    bool CreateCheckpointDirectory();
    
    /**
     * @brief Cria o diretório de backups
     * @return true se criado com sucesso
     */
    bool CreateBackupDirectory();
    
    /**
     * @brief Escreve cabeçalho de arquivo
     * @param file Arquivo
     * @param dataHash Hash dos dados
     * @param dataSize Tamanho dos dados
     * @param compressedSize Tamanho comprimido
     * @param compressed Se está comprimido
     * @param encrypted Se está criptografado
     * @return true se escrito com sucesso
     */
    bool WriteFileHeader(FILE* file, const std::string& dataHash, size_t dataSize,
                       size_t compressedSize, bool compressed, bool encrypted);
    
    /**
     * @brief Lê cabeçalho de arquivo
     * @param file Arquivo
     * @param dataHash Hash dos dados (saída)
     * @param dataSize Tamanho dos dados (saída)
     * @param compressedSize Tamanho comprimido (saída)
     * @param compressed Se está comprimido (saída)
     * @param encrypted Se está criptografado (saída)
     * @return true se lido com sucesso
     */
    bool ReadFileHeader(FILE* file, std::string& dataHash, size_t& dataSize,
                      size_t& compressedSize, bool& compressed, bool& encrypted);
    
    /**
     * @brief Recupera do journal
     * @param path Caminho
     * @return true se recuperado com sucesso
     */
    bool RecoverFromJournal(const std::string& path);
    
    /**
     * @brief Recupera de backup
     * @param path Caminho
     * @return true se recuperado com sucesso
     */
    bool RecoverFromBackup(const std::string& path);
    
    // Configuração
    PersistenceManagerConfig config_;
    
    // Operações
    struct PersistenceOperationData {
        DWORD operationID;
        PersistenceOperation operation;
        PersistenceOperationCallback callback;
        void* data;
        size_t dataSize;
        PersistenceOperationResult result;
    };
    
    std::unordered_map<DWORD, PersistenceOperationData> operations_;
    mutable std::mutex operationsMutex_;
    
    // Filas de operações
    std::queue<DWORD> saveOperationQueue_;
    std::queue<DWORD> loadOperationQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    
    // Filas de persistência
    struct PersistenceQueue {
        DWORD queueID;
        std::string name;
        DWORD flushInterval;
        PersistenceStrategy strategy;
        std::queue<DWORD> operations;
        time_t lastFlushTime;
    };
    
    std::unordered_map<DWORD, PersistenceQueue> persistenceQueues_;
    mutable std::mutex persistenceQueuesMutex_;
    
    // Journals
    struct Journal {
        DWORD journalID;
        std::string name;
        bool autoFlush;
        DWORD flushInterval;
        std::vector<std::pair<std::string, std::vector<BYTE>>> entries;
        time_t lastFlushTime;
        FILE* file;
    };
    
    std::unordered_map<DWORD, Journal> journals_;
    mutable std::mutex journalsMutex_;
    
    // Transações
    struct Transaction {
        DWORD transactionID;
        std::string name;
        std::vector<DWORD> operations;
        time_t startTime;
    };
    
    std::unordered_map<DWORD, Transaction> transactions_;
    mutable std::mutex transactionsMutex_;
    
    // Estatísticas
    PersistenceStatistics statistics_;
    mutable std::mutex statisticsMutex_;
    
    // Caminhos
    std::string dataBasePath_;
    std::string checkpointDirectory_;
    std::string backupDirectory_;
    std::mutex pathsMutex_;
    
    // Callbacks para eventos
    std::map<int, PersistenceEventCallback> eventCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Contadores para novos IDs
    std::atomic<DWORD> nextOperationID_;
    std::atomic<DWORD> nextQueueID_;
    std::atomic<DWORD> nextJournalID_;
    std::atomic<DWORD> nextTransactionID_;
    
    // Threads e sincronização
    std::vector<std::thread> processingThreads_;
    std::thread updateThread_;
    std::atomic<bool> running_;
    std::mutex updateMutex_;
    
    // Tempos
    time_t lastCheckpointTime_;
    time_t lastBackupTime_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_PersistenceManager PersistenceManager::GetInstance()

} // namespace persistence
} // namespace kernel
} // namespace server
} // namespace wyd

#endif // PERSISTENCEMANAGER_H

} // namespace wydbr
