/**
 * PacketHandler.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/network/PacketHandler.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "PacketHandler.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace wydbr {


/**
 * @file PacketHandler.cpp
 * @brief Implementação do manipulador de pacotes para o servidor WYDBRASIL
 * 
 * Esta classe gerencia o processamento e distribuição de pacotes recebidos,
 * implementando sistemas avançados de validação, segurança e controle de fluxo.
 */

// Implementação do singleton
PacketHandler& PacketHandler::getInstance() {
    static PacketHandler instance;
    return instance;
}

// Construtor
PacketHandler::PacketHandler() 
    : m_networkManager(nullptr), 
      m_initialized(false),
      m_strictMode(true),
      m_dropUnknownPackets(true),
      m_logUnknownPackets(true),
      m_globalRateLimit(3000),
      m_nextValidatorId(1),
      m_totalPackets(0),
      m_totalProcessingTime(0),
      m_droppedPackets(0),
      m_totalQueueSize(0),
      m_queueSizeSamples(0) {
}

// Destrutor
PacketHandler::~PacketHandler() {
    shutdown();
}

// Inicializa o manipulador de pacotes
bool PacketHandler::initialize(NetworkManager& networkManager) {
    if (m_initialized) {
        return true;
    }
    
    m_networkManager = &networkManager;
    
    // Registrar callback de processamento no NetworkManager
    m_networkManager->setPacketProcessor([this](int connectionId, PacketCommand cmd, const BYTE* data, int size) {
        return this->processPacket(connectionId, cmd, data, size);
    });
    
    // Inicializar estatísticas
    m_totalPackets = 0;
    m_totalProcessingTime = 0;
    m_droppedPackets = 0;
    m_totalQueueSize = 0;
    m_queueSizeSamples = 0;
    
    m_initialized = true;
    return true;
}

// Finaliza o manipulador de pacotes
void PacketHandler::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    // Limpar manipuladores
    std::lock_guard<std::mutex> lock(m_handlersMutex);
    m_handlers.clear();
    
    // Limpar validadores
    std::lock_guard<std::mutex> validatorLock(m_validatorsMutex);
    m_globalValidators.clear();
    
    // Limpar limitadores de taxa
    std::lock_guard<std::mutex> rateLock(m_rateLimitMutex);
    m_rateLimits.clear();
    
    m_initialized = false;
}

// Registra um manipulador para um tipo específico de pacote
bool PacketHandler::registerHandler(PacketCommand cmd, PacketCallback callback, bool requireAuth, bool adminOnly) {
    if (!m_initialized || !callback) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_handlersMutex);
    
    HandlerEntry entry;
    entry.callback = callback;
    entry.requireAuth = requireAuth;
    entry.adminOnly = adminOnly;
    
    // Se já existe, atualizar configurações
    auto it = m_handlers.find(cmd);
    if (it != m_handlers.end()) {
        entry.minSize = it->second.minSize;
        entry.maxSize = it->second.maxSize;
        entry.logging = it->second.logging;
        entry.logData = it->second.logData;
        entry.packetsPerMinute = it->second.packetsPerMinute;
        entry.burstSize = it->second.burstSize;
    }
    
    m_handlers[cmd] = entry;
    return true;
}

// Remove um manipulador registrado
bool PacketHandler::unregisterHandler(PacketCommand cmd) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_handlersMutex);
    
    auto it = m_handlers.find(cmd);
    if (it == m_handlers.end()) {
        return false;
    }
    
    m_handlers.erase(it);
    return true;
}

// Processa um pacote recebido
bool PacketHandler::processPacket(int connectionId, PacketCommand cmd, const BYTE* data, int size) {
    if (!m_initialized || !m_networkManager) {
        return false;
    }
    
    // Incrementar contador de pacotes
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        m_totalPackets++;
    }
    
    // Obter informações da conexão
    ConnectionInfo* connInfo = m_networkManager->getConnectionInfo(connectionId);
    if (!connInfo) {
        // Conexão inválida
        std::lock_guard<std::mutex> lock(m_statsMutex);
        m_droppedPackets++;
        return false;
    }
    
    // Verificar validade do comando
    {
        std::lock_guard<std::mutex> lock(m_handlersMutex);
        
        auto it = m_handlers.find(cmd);
        if (it == m_handlers.end()) {
            // Comando não /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_logUnknownPackets Parâmetro m_logUnknownPackets
 * @return Retorna registrado
 */
 registrado
            if(m_logUnknownPackets){
                std::ostringstream oss;
                oss << "Pacote desconhecido recebido: Comando=" << static_cast<int>(cmd)
                    << ", Tamanho=" << size
                    << ", IP=" << connInfo->remoteIP;
                std::cout << "[PacketHandler] " << oss.str() << std::endl;
            }
            
            std::lock_guard<std::mutex> statsLock(m_statsMutex);
            m_droppedPackets++;
            
            return !m_dropUnknownPackets;
        }
        
        // Verificar autenticaçã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !connInfo->authenticated Parâmetro !connInfo->authenticated
 * @return Retorna o
 */
o
        if(it->second.requireAuth && !connInfo->authenticated){
            // Requer autenticação, mas conexão não está autenticada
            std::lock_guard<std::mutex> statsLock(m_statsMutex);
            m_droppedPackets++;
            return false;
        }
        
        // Verificar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1 Parâmetro 1
 * @return Retorna administrador
 */
 administrador
        if(it->second.adminOnly && connInfo->accountId != 1){ // Assumindo que accountId 1 é admin
            // Somente admin pode processar, mas não é admin
            std::lock_guard<std::mutex> statsLock(m_statsMutex);
            m_droppedPackets++;
            return false;
        }
        
        // Validar pacote
        if (!validatePacket(connectionId, cmd, data, size, it->second)) {
            // Falha na validação
            std::lock_guard<std::mutex> statsLock(m_statsMutex);
            m_droppedPackets++;
            return false;
        }
        
        // Verificar limite de taxa
        if (!checkRateLimit(connectionId, cmd, it->second)) {
            // Limite de taxa excedido
            std::lock_guard<std::mutex> statsLock(m_statsMutex);
            m_droppedPackets++;
            return false;
        }
        
        // Logar pacote se necessá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param it->second.logging Parâmetro it->second.logging
 * @return Retorna rio
 */
rio
        if(it->second.logging){
            logPacket(connectionId, cmd, data, size, true);
        }
        
        // Iniciar medição de tempo
        auto startTime = std::chrono::steady_clock::now();
        
        // Processar pacote
        bool result = it->second.callback(connectionId, data, size);
        
        // Calcular tempo de processamento
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        
        // Atualizar estatísticas
        {
            std::lock_guard<std::mutex> statsLock(m_statsMutex);
            m_totalProcessingTime += duration;
            
            it->second.packetCount++;
            it->second.totalSize += size;
            it->second.totalProcessingTime += duration;
            
            if (!result) {
                it->second.failureCount++;
            }
        }
        
        return result;
    }
}

// Verifica limites de taxa para um pacote
bool PacketHandler::checkRateLimit(int connectionId, PacketCommand cmd, const HandlerEntry& entry) {
    if (entry.packetsPerMinute <= 0) {
        // Sem limite de taxa para este comando
        return true;
    }
    
    std::lock_guard<std::mutex> lock(m_rateLimitMutex);
    
    // Obter ou criar informações de limite de taxa para esta conexão
    auto& rateInfo = m_rateLimits[connectionId];
    
    // Verificar se é hora de resetar contadores
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - rateInfo.lastReset).count();
    
    if (elapsed >= 60000) {
        // Passou 1 minuto, resetar contadores
        rateInfo.lastReset = now;
        rateInfo.packetCounts.clear();
        rateInfo.totalPackets = 0;
    }
    
    // Verificar limite /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_globalRateLimit Parâmetro m_globalRateLimit
 * @return Retorna global
 */
 global
    if(m_globalRateLimit > 0 && rateInfo.totalPackets >= m_globalRateLimit){
        return false;
    }
    
    // Verificar limite para este comando
    auto& packetCount = rateInfo.packetCounts[cmd];
    
    if (packetCount >= entry.packetsPerMinute) {
        // Verificar se permite /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param entry.burstSize Parâmetro entry.burstSize
 * @return Retorna burst
 */
 burst
        if(entry.burstSize > 0 && packetCount < entry.packetsPerMinute + entry.burstSize){
            // Permitir burst
            packetCount++;
            rateInfo.totalPackets++;
            return true;
        }
        
        return false;
    }
    
    // Incrementar contadores
    packetCount++;
    rateInfo.totalPackets++;
    
    return true;
}

// Valida um pacote
bool PacketHandler::validatePacket(int connectionId, PacketCommand cmd, const BYTE* data, int size, const HandlerEntry& entry) {
    // Verificar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param entry.maxSize Parâmetro entry.maxSize
 * @return Retorna tamanho
 */
 tamanho
    if(size < entry.minSize || size > entry.maxSize){
        return false;
    }
    
    // Executar validadores globais
    {
        std::lock_guard<std::mutex> lock(m_validatorsMutex);
        
        for (const auto& validator : m_globalValidators) {
            if (!validator(connectionId, cmd, data, size)) {
                return false;
            }
        }
    }
    
    return true;
}

// Logs de pacotes
void PacketHandler::logPacket(int connectionId, PacketCommand cmd, const BYTE* data, int size, bool success) {
    if (!m_initialized) {
        return;
    }
    
    ConnectionInfo* connInfo = m_networkManager->getConnectionInfo(connectionId);
    if (!connInfo) {
        return;
    }
    
    std::ostringstream oss;
    oss << "Pacote: Cmd=" << static_cast<int>(cmd)
        << ", Conn=" << connectionId
        << ", IP=" << connInfo->remoteIP
        << ", Size=" << size
        << ", Auth=" << (connInfo->authenticated ? "Sim" : "Não")
        << ", Status=" << (success ? "Sucesso" : "Falha");
    
    std::cout << "[PacketHandler] " << oss.str() << std::endl;
    
    // Log detalhado dos dados se necessário
    std::lock_guard<std::mutex> lock(m_handlersMutex);
    auto it = m_handlers.find(cmd);
    
    if (it != m_handlers.end() && it->second.logData) {
        // Formatar primeiros bytes para log
        std::ostringstream hexData;
        hexData << "Dados: ";
        
        const int maxBytesToLog = std::min(32, size);
        for (int i = 0; i < maxBytesToLog; i++) {
            hexData << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
        }
        
        if (size > maxBytesToLog) {
            hexData << "...";
        }
        
        std::cout << "[PacketHandler] " << hexData.str() << std::endl;
    }
}

// Define limites de taxa para um comando específico
bool PacketHandler::setRateLimit(PacketCommand cmd, int packetsPerMinute, int burstSize) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_handlersMutex);
    
    auto it = m_handlers.find(cmd);
    if (it == m_handlers.end()) {
        return false;
    }
    
    it->second.packetsPerMinute = packetsPerMinute;
    it->second.burstSize = burstSize;
    
    return true;
}

// Define validação de tamanho para um comando específico
bool PacketHandler::setSizeValidation(PacketCommand cmd, int minSize, int maxSize) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_handlersMutex);
    
    auto it = m_handlers.find(cmd);
    if (it == m_handlers.end()) {
        return false;
    }
    
    it->second.minSize = minSize;
    it->second.maxSize = maxSize;
    
    return true;
}

// Ativa ou desativa logging para um tipo de pacote
bool PacketHandler::setPacketLogging(PacketCommand cmd, bool enable, bool logData) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_handlersMutex);
    
    auto it = m_handlers.find(cmd);
    if (it == m_handlers.end()) {
        return false;
    }
    
    it->second.logging = enable;
    it->second.logData = logData;
    
    return true;
}

// Obtém estatísticas para um tipo de pacote
bool PacketHandler::getPacketStats(PacketCommand cmd, uint64_t& count, float& avgProcessingTime, 
                                   float& failureRate, float& avgSize) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_handlersMutex);
    
    auto it = m_handlers.find(cmd);
    if (it == m_handlers.end()) {
        return false;
    }
    
    count = it->second.packetCount;
    
    if (count > 0) {
        avgProcessingTime = static_cast<float>(it->second.totalProcessingTime) / count / 1000.0f; // ms
        failureRate = static_cast<float>(it->second.failureCount) / count * 100.0f; // %
        avgSize = static_cast<float>(it->second.totalSize) / count;
    }
    else {
        avgProcessingTime = 0.0f;
        failureRate = 0.0f;
        avgSize = 0.0f;
    }
    
    return true;
}

// Verifica se um tipo de pacote está registrado
bool PacketHandler::isHandlerRegistered(PacketCommand cmd) {
    if (!m_initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_handlersMutex);
    return m_handlers.find(cmd) != m_handlers.end();
}

// Registra uma função de validação global para todos os pacotes
int PacketHandler::registerGlobalValidator(std::function<bool(int, PacketCommand, const BYTE*, int)> validator) {
    if (!m_initialized || !validator) {
        return -1;
    }
    
    std::lock_guard<std::mutex> lock(m_validatorsMutex);
    
    int validatorId = m_nextValidatorId++;
    m_globalValidators.push_back(validator);
    
    return validatorId;
}

// Remove um validador global
bool PacketHandler::unregisterGlobalValidator(int validatorId) {
    if (!m_initialized || validatorId <= 0 || validatorId >= m_nextValidatorId) {
        return false;
    }
    
    // Este método é simplificado, na prática seria necessário associar
    // IDs com validadores específicos para remoção
    return false;
}

// Define comportamento para pacotes não reconhecidos
void PacketHandler::setUnknownPacketPolicy(bool dropPacket, bool logUnknown) {
    m_dropUnknownPackets = dropPacket;
    m_logUnknownPackets = logUnknown;
}

// Define modo de segurança
void PacketHandler::setSecurityMode(bool strictMode) {
    m_strictMode = strictMode;
}

// Define limite global de pacotes por conexão
void PacketHandler::setGlobalRateLimit(int packetsPerMinute) {
    m_globalRateLimit = packetsPerMinute;
}

// Obtém estatísticas gerais de processamento
void PacketHandler::getGlobalStats(uint64_t& totalPackets, float& avgProcessingTime, 
                                   uint64_t& droppedPackets, float& avgQueueSize) {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    totalPackets = m_totalPackets;
    droppedPackets = m_droppedPackets;
    
    if (m_totalPackets > 0) {
        avgProcessingTime = static_cast<float>(m_totalProcessingTime) / m_totalPackets / 1000.0f; // ms
    }
    else {
        avgProcessingTime = 0.0f;
    }
    
    if (m_queueSizeSamples > 0) {
        avgQueueSize = static_cast<float>(m_totalQueueSize) / m_queueSizeSamples;
    }
    else {
        avgQueueSize = 0.0f;
    }
}

// Limpa estatísticas acumuladas
void PacketHandler::clearStats() {

} // namespace wydbr

    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    m_totalPackets = 0;
    m_totalProcessingTime = 0;
    m_droppedPackets = 0;
    m_totalQueueSize = 0;
    m_queueSizeSamples = 0;
    
    std::lock_guard<std::mutex> handlersLock(m_handlersMutex);
    
    for (auto& pair : m_handlers) {
        pair.second.packetCount = 0;
        pair.second.totalSize = 0;
        pair.second.totalProcessingTime = 0;
        pair.second.failureCount = 0;
    }
}