/**
 * PacketHandler.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/network/PacketHandler.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_

/**
 * @file PacketHandler.h
 * @brief Sistema de processamento de pacotes do WYDBRASIL
 * 
 * Esta classe implementa o sistema de processamento de pacotes recebidos pelo servidor,
 * mantendo total compatibilidade com o protocolo original do WYD, enquanto adiciona
 * recursos de segurança e validação avançados para prevenir exploits e hacks.
 */

#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>
#include <mutex>
#include <chrono>
#include <atomic>
#include <queue>

#include "../../include/Basedef.h"
#include "NetworkManager.h"

namespace wydbr {


/**
 * @class PacketHandler
 * @brief Manipulador de pacotes recebidos pelo servidor
 * 
 * Esta classe gerencia o processamento de pacotes recebidos, encaminhando-os
 * para os manipuladores específicos registrados e aplicando validações de segurança.
 */
/**
 * Classe WYDPacketHandler
 * 
 * Esta classe implementa a funcionalidade WYDPacketHandler conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PacketHandler {
public:
    /**
     * @brief Obtém a instância única do PacketHandler (Singleton)
     * @return Referência para a instância única
     */
    static PacketHandler& getInstance();
    
    /**
     * @brief Inicializa o manipulador de pacotes
     * @param networkManager Referência para o gerenciador de rede
     * @return true se inicializado com sucesso
     */
    bool initialize(NetworkManager& networkManager);
    
    /**
     * @brief Finaliza o manipulador de pacotes
     */
    void shutdown();
    
    /**
     * @typedef PacketCallback
     * @brief Tipo de função de callback para processamento de pacotes
     * @param connectionId ID da conexão
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se o pacote foi processado com sucesso
     */
    using PacketCallback = std::function<bool(int connectionId, const BYTE* data, int size)>;
    
    /**
     * @brief Registra um manipulador para um tipo específico de pacote
     * @param cmd Comando do pacote
     * @param callback Função de callback
     * @param requireAuth Requer autenticação para processar
     * @param adminOnly Somente administrador pode processar
     * @return true se registrado com sucesso
     */
    bool registerHandler(PacketCommand cmd, PacketCallback callback, bool requireAuth = true, bool adminOnly = false);
    
    /**
     * @brief Remove um manipulador registrado
     * @param cmd Comando do pacote
     * @return true se removido com sucesso
     */
    bool unregisterHandler(PacketCommand cmd);
    
    /**
     * @brief Processa um pacote recebido (chamado pelo NetworkManager)
     * @param connectionId ID da conexão
     * @param cmd Comando do pacote
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processPacket(int connectionId, PacketCommand cmd, const BYTE* data, int size);
    
    /**
     * @brief Define limites de taxa para um comando específico
     * @param cmd Comando do pacote
     * @param packetsPerMinute Máximo de pacotes por minuto
     * @param burstSize Tamanho máximo do burst
     * @return true se configurado com sucesso
     */
    bool setRateLimit(PacketCommand cmd, int packetsPerMinute, int burstSize = 10);
    
    /**
     * @brief Define validação de tamanho para um comando específico
     * @param cmd Comando do pacote
     * @param minSize Tamanho mínimo do pacote
     * @param maxSize Tamanho máximo do pacote
     * @return true se configurado com sucesso
     */
    bool setSizeValidation(PacketCommand cmd, int minSize, int maxSize);
    
    /**
     * @brief Ativa ou desativa logging para um tipo de pacote
     * @param cmd Comando do pacote
     * @param enable Ativar (true) ou desativar (false)
     * @param logData Logar dados do pacote
     * @return true se configurado com sucesso
     */
    bool setPacketLogging(PacketCommand cmd, bool enable, bool logData = false);
    
    /**
     * @brief Obtém estatísticas para um tipo de pacote
     * @param cmd Comando do pacote
     * @param count Total de pacotes processados (saída)
     * @param avgProcessingTime Tempo médio de processamento em ms (saída)
     * @param failureRate Taxa de falha (saída)
     * @param avgSize Tamanho médio do pacote (saída)
     * @return true se estatísticas obtidas com sucesso
     */
    bool getPacketStats(PacketCommand cmd, uint64_t& count, float& avgProcessingTime, 
                          float& failureRate, float& avgSize);
    
    /**
     * @brief Verifica se um tipo de pacote está registrado
     * @param cmd Comando do pacote
     * @return true se registrado
     */
    bool isHandlerRegistered(PacketCommand cmd);
    
    /**
     * @brief Registra uma função de validação global para todos os pacotes
     * @param validator Função de validação
     * @return ID do validador registrado
     */
    int registerGlobalValidator(std::function<bool(int, PacketCommand, const BYTE*, int)> validator);
    
    /**
     * @brief Remove um validador global
     * @param validatorId ID do validador
     * @return true se removido com sucesso
     */
    bool unregisterGlobalValidator(int validatorId);
    
    /**
     * @brief Define comportamento para pacotes não reconhecidos
     * @param dropPacket Descartar pacote (true) ou permitir (false)
     * @param logUnknown Logar pacotes desconhecidos
     */
    void setUnknownPacketPolicy(bool dropPacket, bool logUnknown = true);
    
    /**
     * @brief Define modo de segurança
     * @param strictMode Modo estrito (validações mais rigorosas)
     */
    void setSecurityMode(bool strictMode);
    
    /**
     * @brief Define limite global de pacotes por conexão
     * @param packetsPerMinute Máximo de pacotes por minuto por conexão
     */
    void setGlobalRateLimit(int packetsPerMinute);
    
    /**
     * @brief Obtém estatísticas gerais de processamento
     * @param totalPackets Total de pacotes processados (saída)
     * @param avgProcessingTime Tempo médio de processamento em ms (saída)
     * @param droppedPackets Total de pacotes descartados (saída)
     * @param avgQueueSize Tamanho médio da fila de processamento (saída)
     */
    void getGlobalStats(uint64_t& totalPackets, float& avgProcessingTime, 
                        uint64_t& droppedPackets, float& avgQueueSize);
    
    /**
     * @brief Limpa estatísticas acumuladas
     */
    void clearStats();
    
private:
    // Construtor privado (singleton)
    PacketHandler();
    
    // Destrutor
    ~PacketHandler();
    
    // Impedir cópia e atribuição
    PacketHandler(const PacketHandler&) = delete;
    PacketHandler& operator=(const PacketHandler&) = delete;
    
    // Estrutura para uma entrada na tabela de manipuladores
    struct HandlerEntry {
        PacketCallback callback;     // Função de callback
        bool requireAuth;            // Requer autenticação
        bool adminOnly;              // Somente administrador
        int minSize;                 // Tamanho mínimo do pacote
        int maxSize;                 // Tamanho máximo do pacote
        bool logging;                // Logging ativado
        bool logData;                // Logar dados do pacote
        
        // Limite de taxa
        int packetsPerMinute;        // Máximo de pacotes por minuto
        int burstSize;               // Tamanho máximo do burst
        
        // Estatísticas
        uint64_t packetCount;        // Total de pacotes processados
        uint64_t totalSize;          // Soma dos tamanhos dos pacotes
        uint64_t totalProcessingTime; // Tempo total de processamento
        uint64_t failureCount;       // Total de falhas no processamento
        
        // Construtor
        HandlerEntry() : requireAuth(true), adminOnly(false), 
                       minSize(0), maxSize(MAX_PACKET_SIZE), 
                       logging(false), logData(false),
                       packetsPerMinute(0), burstSize(10),
                       packetCount(0), totalSize(0), 
                       totalProcessingTime(0), failureCount(0) {}
    };
    
    // Estrutura para controle de limite de taxa por conexão
    struct RateLimitInfo {
        std::chrono::steady_clock::time_point lastReset;
        std::unordered_map<PacketCommand, int> packetCounts;
        int totalPackets;
        
        RateLimitInfo() : lastReset(std::chrono::steady_clock::now()), totalPackets(0) {}
    };
    
    // Verifica limites de taxa para um pacote
    /**
 * checkRateLimit
 * 
 * Implementa a funcionalidade checkRateLimit conforme especificação original.
 * @param connectionId Parâmetro connectionId
 * @param cmd Parâmetro cmd
 * @param entry Parâmetro entry
 * @return Retorna bool
 */

    bool checkRateLimit(int connectionId, PacketCommand cmd, const HandlerEntry& entry);
    
    // Valida um pacote
    /**
 * validatePacket
 * 
 * Implementa a funcionalidade validatePacket conforme especificação original.
 * @param connectionId Parâmetro connectionId
 * @param cmd Parâmetro cmd
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param entry Parâmetro entry
 * @return Retorna bool
 */

    bool validatePacket(int connectionId, PacketCommand cmd, const BYTE* data, int size, const HandlerEntry& entry);
    
    // Logs de pacotes
    /**
 * logPacket
 * 
 * Implementa a funcionalidade logPacket conforme especificação original.
 * @param connectionId Parâmetro connectionId
 * @param cmd Parâmetro cmd
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param success Parâmetro success
 */

    void logPacket(int connectionId, PacketCommand cmd, const BYTE* data, int size, bool success);
    
    // Mapeia comandos de pacote para manipuladores
    std::unordered_map<PacketCommand, HandlerEntry> m_handlers;
    
    // Mutex para acesso à tabela de manipuladores
    std::mutex m_handlersMutex;
    
    // Validadores globais
    std::vector<std::function<bool(int, PacketCommand, const BYTE*, int)>> m_globalValidators;
    
    // Mutex para validadores globais
    std::mutex m_validatorsMutex;
    
    // Próximo ID de validador
    std::atomic<int> m_nextValidatorId;
    
    // Limites de taxa por conexão
    std::unordered_map<int, RateLimitInfo> m_rateLimits;
    
    // Mutex para limites de taxa
    std::mutex m_rateLimitMutex;
    
    // Estatísticas globais
    uint64_t m_totalPackets;         // Total de pacotes processados
    uint64_t m_totalProcessingTime;  // Tempo total de processamento
    uint64_t m_droppedPackets;       // Total de pacotes descartados
    uint64_t m_totalQueueSize;       // Soma dos tamanhos da fila
    uint64_t m_queueSizeSamples;     // Número de amostras do tamanho da fila
    
    // Mutex para estatísticas
    std::mutex m_statsMutex;
    
    // Configurações
    bool m_strictMode;               // Modo de segurança estrito
    bool m_dropUnknownPackets;       // Descartar pacotes desconhecidos
    bool m_logUnknownPackets;        // Logar pacotes desconhecidos
    int m_globalRateLimit;           // Limite global de pacotes por minuto
    
    // Referência para o gerenciador de rede
    NetworkManager* m_networkManager;
    
    // Inicializado
    bool m_initialized;
};

#endif // _PACKET_HANDLER_H_

} // namespace wydbr
