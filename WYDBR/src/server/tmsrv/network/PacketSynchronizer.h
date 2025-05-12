/**
 * PacketSynchronizer.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/network/PacketSynchronizer.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PACKETSYNCHRONIZER_H
#define PACKETSYNCHRONIZER_H

#include <map>
#include <vector>
#include <queue>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>

#include "../../../core/WYDTypes.h"
#include "protocol/PacketTypes.h"

namespace wydbr {


/**
 * @file PacketSynchronizer.h
 * @brief Sistema de sincronização de pacotes
 * 
 * Este arquivo contém a definição do sistema de sincronização de pacotes,
 * que é responsável por garantir a sincronização correta de dados entre
 * cliente e servidor, lidando com pacotes perdidos, duplicados ou fora de ordem.
 * No WYD original, esta funcionalidade não existia, causando problemas.
 */

namespace wyd {
namespace server {
namespace network {

// Forward declarations
class CConnection;
class CPacket;

// Constantes
#define MAX_PACKET_SEQUENCE        65535   // Máximo da sequência de pacotes
#define MAX_PACKET_WINDOW          1024    // Tamanho máximo da janela de pacotes
#define MAX_PACKET_RETRIES         5       // Máximo de retentativas de envio
#define PACKET_RETRY_INTERVAL      500     // Intervalo entre retentativas (ms)
#define PACKET_ACK_TIMEOUT         5000    // Timeout para ACK (ms)
#define PACKET_SYNC_INTERVAL       100     // Intervalo de sincronização (ms)
#define OUT_OF_ORDER_THRESHOLD     100     // Limiar para pacotes fora de ordem

/**
 * @brief Status de sincronização de pacote
 */
enum class SyncStatus : BYTE {
    NONE = 0,           // Nenhum status
    PENDING = 1,        // Pendente de envio/recebimento
    SENT = 2,           // Enviado
    RECEIVED = 3,       // Recebido
    ACKNOWLEDGED = 4,   // Confirmado
    TIMED_OUT = 5,      // Timeout
    FAILED = 6,         // Falhou
};

/**
 * @brief Informações de sincronização de pacote
 */
struct PacketSyncInfo {
    WORD wSequence;           // Sequência do pacote
    DWORD dwTimestamp;        // Timestamp de envio/recebimento
    DWORD dwLastRetryTime;    // Último tempo de retry
    BYTE byRetryCount;        // Contador de retentativas
    SyncStatus status;        // Status de sincronização
    std::shared_ptr<CPacket> packet; // Ponteiro para o pacote
    
    PacketSyncInfo()
        : wSequence(0)
        , dwTimestamp(0)
        , dwLastRetryTime(0)
        , byRetryCount(0)
        , status(SyncStatus::NONE)
        , packet(nullptr)
    {
    }
};

/**
 * @brief Callback de sincronização de pacote
 */
using PacketSyncCallback = std::function<void(WORD, SyncStatus)>;

/**
 * @brief Sistema de sincronização de pacotes
 * 
 * Esta classe implementa um sistema de sincronização de pacotes,
 * garantindo a entrega confiável e ordenada.
 */
/**
 * Classe WYDPacketSynchronizer
 * 
 * Esta classe implementa a funcionalidade WYDPacketSynchronizer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CPacketSynchronizer {
public:
    /**
     * @brief Construtor
     * @param connection Conexão associada
     */
    CPacketSynchronizer(std::shared_ptr<CConnection> connection);
    
    /**
     * @brief Destrutor
     */
    ~CPacketSynchronizer();
    
    /**
     * @brief Inicializa o sincronizador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o sincronizador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o sincronizador
     * @param currentTime Tempo atual
     */
    void Update(DWORD currentTime);
    
    /**
     * @brief Rastreia um pacote para sincronização
     * @param packet Pacote a rastrear
     * @param reliable Se o pacote é confiável
     * @return Sequência do pacote
     */
    WORD TrackPacket(std::shared_ptr<CPacket> packet, bool reliable = true);
    
    /**
     * @brief Processa um ACK recebido
     * @param sequence Sequência do pacote
     * @return true se processado com sucesso
     */
    bool ProcessAck(WORD sequence);
    
    /**
     * @brief Processa um NACK recebido
     * @param sequence Sequência do pacote
     * @return true se processado com sucesso
     */
    bool ProcessNack(WORD sequence);
    
    /**
     * @brief Processa um pacote recebido
     * @param packet Pacote recebido
     * @return true se processado com sucesso
     */
    bool ProcessIncomingPacket(std::shared_ptr<CPacket> packet);
    
    /**
     * @brief Obtém o próximo pacote na ordem
     * @return Próximo pacote, ou nullptr se não houver
     */
    std::shared_ptr<CPacket> GetNextOrderedPacket();
    
    /**
     * @brief Verifica se há pacotes na ordem disponíveis
     * @return true se houver
     */
    bool HasOrderedPackets();
    
    /**
     * @brief Registra um callback para sincronização
     * @param callback Callback
     */
    void RegisterCallback(PacketSyncCallback callback);
    
    /**
     * @brief Limpa o callback
     */
    void ClearCallback();
    
    /**
     * @brief Ativa o modo de sincronização estrita
     * @param strict Se deve ser estrito
     */
    void SetStrictMode(bool strict);
    
    /**
     * @brief Obtém a sequência atual
     * @return Sequência atual
     */
    WORD GetCurrentSequence() const;
    
    /**
     * @brief Obtém a última sequência recebida
     * @return Última sequência recebida
     */
    WORD GetLastReceivedSequence() const;
    
    /**
     * @brief Obtém o status de um pacote
     * @param sequence Sequência do pacote
     * @return Status do pacote
     */
    SyncStatus GetPacketStatus(WORD sequence) const;
    
    /**
     * @brief Obtém as estatísticas de sincronização
     * @return Estatísticas de sincronização
     */
    struct SyncStats {
        DWORD dwPacketsSent;            // Pacotes enviados
        DWORD dwPacketsReceived;        // Pacotes recebidos
        DWORD dwPacketsAcknowledged;    // Pacotes confirmados
        DWORD dwPacketsTimedOut;        // Pacotes com timeout
        DWORD dwPacketsRetried;         // Pacotes reenvidos
        DWORD dwPacketsLost;            // Pacotes perdidos
        DWORD dwPacketsOutOfOrder;      // Pacotes fora de ordem
        DWORD dwPacketsDuplicated;      // Pacotes duplicados
        float fPacketLoss;              // Perda de pacotes (%)
        float fAverageLatency;          // Latência média (ms)
        
        SyncStats() {
            memset(this, 0, sizeof(SyncStats));
        }
    };
    
    /**
 * GetStats
 * 
 * Implementa a funcionalidade GetStats conforme especificação original.
 * @return Retorna SyncStats
 */

    
    SyncStats GetStats() const;
    
    /**
     * @brief Reseta as estatísticas
     */
    void ResetStats();
    
    /**
     * @brief Verifica se um pacote está dentro da janela
     * @param sequence Sequência do pacote
     * @param windowStart Início da janela
     * @return true se estiver dentro da janela
     */
    bool IsPacketInWindow(WORD sequence, WORD windowStart) const;

private:
    /**
     * @brief Envia um ACK
     * @param sequence Sequência do pacote
     * @return true se enviado com sucesso
     */
    bool SendAck(WORD sequence);
    
    /**
     * @brief Envia um NACK
     * @param sequence Sequência do pacote
     * @return true se enviado com sucesso
     */
    bool SendNack(WORD sequence);
    
    /**
     * @brief Processa timeouts de pacotes
     * @param currentTime Tempo atual
     */
    void ProcessTimeouts(DWORD currentTime);
    
    /**
     * @brief Tenta reenviar pacotes com timeout
     * @param currentTime Tempo atual
     */
    void RetryTimedOutPackets(DWORD currentTime);
    
    /**
     * @brief Notifica o callback
     * @param sequence Sequência do pacote
     * @param status Status do pacote
     */
    void NotifyCallback(WORD sequence, SyncStatus status);
    
    // Conexão associada
    std::weak_ptr<CConnection> connection_;
    
    // Pacotes saindo
    std::map<WORD, PacketSyncInfo> outgoingPackets_;
    std::mutex outgoingMutex_;
    
    // Pacotes chegando
    std::map<WORD, PacketSyncInfo> incomingPackets_;
    std::priority_queue<WORD, std::vector<WORD>, std::greater<WORD>> orderedQueue_;
    std::mutex incomingMutex_;
    
    // Sequências
    WORD nextOutgoingSequence_;
    WORD lastReceivedSequence_;
    
    // Callback
    PacketSyncCallback callback_;
    std::mutex callbackMutex_;
    
    // Configurações
    bool strictMode_;
    
    // Estatísticas
    SyncStats stats_;
    std::mutex statsMutex_;
    
    // Estado
    bool initialized_;
    bool shutdownRequested_;
};

} // namespace network
} // namespace server
} // namespace wyd

#endif // PACKETSYNCHRONIZER_H

} // namespace wydbr
