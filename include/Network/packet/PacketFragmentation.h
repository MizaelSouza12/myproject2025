/**
 * @file PacketFragmentation.h
 * @brief Sistema de fragmentação e remontagem de pacotes
 */

#ifndef WYDBR_PACKET_FRAGMENTATION_H
#define WYDBR_PACKET_FRAGMENTATION_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <memory>
#include "NetworkProtocol.h"

namespace WYDBR {
namespace Network {

/**
 * Configuração de fragmentação
 */
struct FragmentationConfig {
    uint16_t maxFragmentSize;          // Tamanho máximo de cada fragmento
    uint16_t maxFragmentsPerPacket;    // Número máximo de fragmentos por pacote
    uint32_t fragmentTimeout;          // Timeout para fragmentos em ms
    uint32_t maxPendingFragments;      // Máximo de fragmentos pendentes
    bool compressLargePackets;         // Comprimir pacotes grandes
};

/**
 * Fragmentador de pacotes
 */
class PacketFragmenter {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o fragmentador
     */
    static PacketFragmenter& GetInstance();
    
    /**
     * Inicializa o fragmentador
     * @param config Configuração de fragmentação
     * @return true se inicializado com sucesso
     */
    bool Initialize(const FragmentationConfig& config);
    
    /**
     * Fragmenta um pacote se necessário
     * @param packet Pacote para fragmentar
     * @param packetSize Tamanho do pacote
     * @return Vetor de fragmentos (1 elemento se não precisar fragmentar)
     */
    std::vector<std::vector<uint8_t>> FragmentPacket(const uint8_t* packet, size_t packetSize);
    
    /**
     * Verifica se um pacote é um fragmento
     * @param packet Pacote a verificar
     * @param packetSize Tamanho do pacote
     * @return true se for um fragmento
     */
    bool IsFragment(const uint8_t* packet, size_t packetSize);
    
    /**
     * Processa um fragmento
     * @param fragment Pacote fragmento
     * @param fragmentSize Tamanho do fragmento
     * @param reconstructedPacket Ponteiro para receber pacote reconstruído (se completo)
     * @return true se o pacote foi remontado com sucesso
     */
    bool ProcessFragment(const uint8_t* fragment, size_t fragmentSize, 
                        std::vector<uint8_t>* reconstructedPacket);
    
    /**
     * Limpa fragmentos expirados
     * @return Número de conjuntos de fragmentos removidos
     */
    int CleanupExpiredFragments();
    
    /**
     * Obtém estatísticas de fragmentação
     * @return String formatada com estatísticas
     */
    std::string GetStatistics() const;
    
    /**
     * Verifica se um pacote precisa ser fragmentado
     * @param packetSize Tamanho do pacote
     * @return true se precisar fragmentar
     */
    bool NeedsFragmentation(size_t packetSize) const;
    
private:
    // Construtor e destrutor privados (singleton)
    PacketFragmenter();
    ~PacketFragmenter();
    
    // Não permitir cópias
    PacketFragmenter(const PacketFragmenter&) = delete;
    PacketFragmenter& operator=(const PacketFragmenter&) = delete;
    
    // Estrutura para conjunto de fragmentos
    struct FragmentSet {
        std::vector<std::vector<uint8_t>> fragments;    // Fragmentos recebidos
        uint16_t fragmentCount;                        // Total de fragmentos
        uint16_t receivedCount;                        // Número recebido
        uint64_t firstFragmentTime;                    // Timestamp do primeiro fragmento
        bool complete;                                 // Conjunto completo
    };
    
    // Mapa de conjuntos de fragmentos por ID
    std::unordered_map<uint32_t, FragmentSet> m_fragmentSets;
    
    // Configuração
    FragmentationConfig m_config;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
    
    // Contador para gerar IDs de fragmentação
    std::atomic<uint32_t> m_nextFragmentId;
    
    // Estatísticas
    uint32_t m_totalFragmented;         // Total de pacotes fragmentados
    uint32_t m_totalReassembled;        // Total de pacotes remontados
    uint32_t m_totalExpired;            // Total de conjuntos expirados
    uint32_t m_totalFragments;          // Total de fragmentos processados
    
    // Métodos privados
    std::vector<std::vector<uint8_t>> SplitPacket(const uint8_t* packet, size_t packetSize, 
                                                uint32_t fragmentId);
    std::vector<uint8_t> ReassemblePacket(const FragmentSet& fragmentSet);
    bool TryReassemblePacket(FragmentSet& fragmentSet, std::vector<uint8_t>* reconstructedPacket);
    uint32_t GenerateFragmentId();
};

} // namespace Network
} // namespace WYDBR

#endif // WYDBR_PACKET_FRAGMENTATION_H
