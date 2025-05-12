/**
 * WYDPacketHandler.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/network/WYDPacketHandler.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _WYD_PACKET_HANDLER_H_
#define _WYD_PACKET_HANDLER_H_

/**
 * @file WYDPacketHandler.h
 * @brief Gerenciador de pacotes de rede do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de pacotes de rede do WYDBR,
 * responsável por processar, validar e rotear pacotes entre cliente e servidor,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>

#include "../GlobalDef.h"
#include "../TM_Protocol.h"

namespace wydbr {
namespace network {

/**
 * @brief Tipos de resultado de processamento de pacotes
 */
enum /**
 * Classe WYDPacketProcessResult
 * 
 * Esta classe implementa a funcionalidade WYDPacketProcessResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PacketProcessResult {
    SUCCESS = 0,                      // Processado com sucesso
    INVALID_SIZE = 1,                 // Tamanho inválido
    INVALID_OPCODE = 2,               // Código de operação inválido
    INVALID_CHECKSUM = 3,             // Checksum inválido
    INVALID_CLIENT = 4,               // Cliente inválido
    NOT_AUTHENTICATED = 5,            // Não autenticado
    NOT_AUTHORIZED = 6,               // Não autorizado
    HANDLER_NOT_FOUND = 7,            // Manipulador não encontrado
    HANDLER_ERROR = 8,                // Erro do manipulador
    THROTTLED = 9,                    // Limitado por throttling
    ENCRYPTION_ERROR = 10,            // Erro de criptografia
    COMPRESSION_ERROR = 11,           // Erro de compressão
    SECURITY_ERROR = 12,              // Erro de segurança
    INTERNAL_ERROR = 13,              // Erro interno
    CLIENT_DISCONNECTED = 14,         // Cliente desconectado
    SERVER_SHUTDOWN = 15,             // Servidor em shutdown
    UNKNOWN_ERROR = 16                // Erro desconhecido
};

/**
 * @brief Tipos de contexto de pacotes
 */
enum /**
 * Classe WYDPacketContext
 * 
 * Esta classe implementa a funcionalidade WYDPacketContext conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PacketContext {
    UNKNOWN = 0,                      // Desconhecido
    CONNECTION = 1,                   // Conexão
    AUTHENTICATION = 2,               // Autenticação
    CHARACTER = 3,                    // Personagem
    MOVEMENT = 4,                     // Movimento
    COMBAT = 5,                       // Combate
    CHAT = 6,                         // Chat
    ITEM = 7,                         // Item
    SKILL = 8,                        // Habilidade
    TRADE = 9,                        // Comércio
    PARTY = 10,                       // Grupo
    GUILD = 11,                       // Guilda
    SHOP = 12,                        // Loja
    STORAGE = 13,                     // Armazenamento
    AUCTION = 14,                     // Leilão
    MAIL = 15,                        // Correio
    QUEST = 16,                       // Missão
    EVENT = 17,                       // Evento
    ADMIN = 18,                       // Administração
    SYSTEM = 19,                      // Sistema
    DEBUG = 20,                       // Depuração
    CUSTOM = 21                       // Personalizado
};

/**
 * @brief Níveis de segurança de pacotes
 */
enum /**
 * Classe WYDPacketSecurityLevel
 * 
 * Esta classe implementa a funcionalidade WYDPacketSecurityLevel conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PacketSecurityLevel {
    NONE = 0,                         // Nenhum
    LOW = 1,                          // Baixo
    MEDIUM = 2,                       // Médio
    HIGH = 3,                         // Alto
    VERY_HIGH = 4,                    // Muito alto
    MAXIMUM = 5                       // Máximo
};

/**
 * @brief Informações de validação de pacotes
 */
struct PacketValidationInfo {
    bool requiresAuthentication;      // Requer autenticação
    bool requiresEncryption;          // Requer criptografia
    bool requiresCompression;         // Requer compressão
    bool requiresChecksum;            // Requer checksum
    PacketSecurityLevel securityLevel; // Nível de segurança
    uint32_t minSize;                 // Tamanho mínimo
    uint32_t maxSize;                 // Tamanho máximo
    uint32_t rateLimit;               // Limite de taxa (ms)
    uint32_t burstLimit;              // Limite de burst
    
    PacketValidationInfo() : requiresAuthentication(false), requiresEncryption(false),
        requiresCompression(false), requiresChecksum(true),
        securityLevel(PacketSecurityLevel::MEDIUM), minSize(12), maxSize(8192),
        rateLimit(0), burstLimit(0) {}
};

/**
 * @brief Informações de rastreamento de pacotes
 */
struct PacketTrackingInfo {
    uint32_t totalReceived;           // Total recebido
    uint32_t totalSent;               // Total enviado
    uint32_t bytesReceived;           // Bytes recebidos
    uint32_t bytesSent;               // Bytes enviados
    uint32_t invalidCount;            // Contagem inválida
    uint32_t throttledCount;          // Contagem limitada
    uint32_t lastPacketTime;          // Timestamp do último pacote
    uint32_t packetsPerSecond;        // Pacotes por segundo
    uint32_t bytesPerSecond;          // Bytes por segundo
    
    PacketTrackingInfo() : totalReceived(0), totalSent(0), bytesReceived(0),
        bytesSent(0), invalidCount(0), throttledCount(0), lastPacketTime(0),
        packetsPerSecond(0), bytesPerSecond(0) {}
};

/**
 * @brief Estatísticas de pacotes
 */
struct PacketStats {
    uint32_t totalProcessed;          // Total processado
    uint32_t totalSucceeded;          // Total com sucesso
    uint32_t totalFailed;             // Total com falha
    uint32_t totalInvalidSize;        // Total com tamanho inválido
    uint32_t totalInvalidOpcode;      // Total com opcode inválido
    uint32_t totalInvalidChecksum;    // Total com checksum inválido
    uint32_t totalNotAuthenticated;   // Total não autenticado
    uint32_t totalNotAuthorized;      // Total não autorizado
    uint32_t totalHandlerNotFound;    // Total com manipulador não encontrado
    uint32_t totalHandlerError;       // Total com erro no manipulador
    uint32_t totalThrottled;          // Total limitado
    uint32_t totalEncryptionError;    // Total com erro de criptografia
    uint32_t totalCompressionError;   // Total com erro de compressão
    uint32_t totalSecurityError;      // Total com erro de segurança
    uint32_t totalInternalError;      // Total com erro interno
    uint32_t totalUnknownError;       // Total com erro desconhecido
    
    PacketStats() : totalProcessed(0), totalSucceeded(0), totalFailed(0),
        totalInvalidSize(0), totalInvalidOpcode(0), totalInvalidChecksum(0),
        totalNotAuthenticated(0), totalNotAuthorized(0), totalHandlerNotFound(0),
        totalHandlerError(0), totalThrottled(0), totalEncryptionError(0),
        totalCompressionError(0), totalSecurityError(0), totalInternalError(0),
        totalUnknownError(0) {}
};

/**
 * @brief Informações do manipulador de pacotes
 */
struct PacketHandlerInfo {
    uint16_t opcode;                  // Código de operação
    std::string name;                 // Nome do manipulador
    PacketContext context;            // Contexto do pacote
    PacketValidationInfo validation;  // Informações de validação
    std::function<PacketProcessResult(ClientId, const uint8_t*, uint32_t)> handler; // Função de manipulação
    
    PacketHandlerInfo() : opcode(0), context(PacketContext::UNKNOWN) {}
    
    PacketHandlerInfo(uint16_t _opcode, const std::string& _name, PacketContext _context,
                    const PacketValidationInfo& _validation,
                    std::function<PacketProcessResult(ClientId, const uint8_t*, uint32_t)> _handler)
        : opcode(_opcode), name(_name), context(_context), validation(_validation), handler(_handler) {}
};

/**
 * @brief Gerenciador de pacotes de rede
 * 
 * Esta classe é responsável por processar, validar e rotear pacotes entre cliente e servidor,
 * incluindo segurança, criptografia, compressão e limitação de taxa.
 */
/**
 * Classe WYDPacketHandler
 * 
 * Esta classe implementa a funcionalidade WYDPacketHandler conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDPacketHandler {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static WYDPacketHandler& getInstance() {
        static WYDPacketHandler instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Processa um pacote recebido
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult processPacket(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Envia um pacote para um cliente
     * @param clientId ID do cliente
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se enviado com sucesso
     */
    bool sendPacket(ClientId clientId, uint16_t opcode, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Envia um pacote para todos os clientes
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param exceptClientId ID do cliente a ser excluído
     * @return Número de clientes que receberam o pacote
     */
    uint32_t sendPacketToAll(uint16_t opcode, const uint8_t* data, uint32_t size, ClientId exceptClientId = 0);
    
    /**
     * @brief Registra um manipulador de pacotes
     * @param opcode Código de operação
     * @param name Nome do manipulador
     * @param context Contexto do pacote
     * @param handler Função de manipulação
     * @param validation Informações de validação
     * @return true se registrado com sucesso
     */
    bool registerHandler(uint16_t opcode, const std::string& name, PacketContext context,
                        std::function<PacketProcessResult(ClientId, const uint8_t*, uint32_t)> handler,
                        const PacketValidationInfo& validation = PacketValidationInfo());
    
    /**
     * @brief Remove um manipulador de pacotes
     * @param opcode Código de operação
     * @return true se removido com sucesso
     */
    bool unregisterHandler(uint16_t opcode);
    
    /**
     * @brief Verifica se um manipulador está registrado
     * @param opcode Código de operação
     * @return true se estiver registrado
     */
    bool isHandlerRegistered(uint16_t opcode) const;
    
    /**
     * @brief Obtém as informações de um manipulador
     * @param opcode Código de operação
     * @return Informações do manipulador ou nullptr se não encontrado
     */
    const PacketHandlerInfo* getHandlerInfo(uint16_t opcode) const;
    
    /**
     * @brief Obtém as estatísticas de pacotes
     * @return Estatísticas de pacotes
     */
    const PacketStats& getStats() const;
    
    /**
     * @brief Reseta as estatísticas de pacotes
     */
    void resetStats();
    
    /**
     * @brief Obtém as informações de rastreamento de um cliente
     * @param clientId ID do cliente
     * @return Informações de rastreamento ou nullptr se não encontrado
     */
    const PacketTrackingInfo* getClientTrackingInfo(ClientId clientId) const;
    
    /**
     * @brief Define a chave de criptografia de um cliente
     * @param clientId ID do cliente
     * @param key Chave de criptografia
     * @return true se definido com sucesso
     */
    bool setClientEncryptionKey(ClientId clientId, uint32_t key);
    
    /**
     * @brief Habilita a criptografia para um cliente
     * @param clientId ID do cliente
     * @param enabled Habilitar ou desabilitar
     * @return true se alterado com sucesso
     */
    bool setClientEncryption(ClientId clientId, bool enabled);
    
    /**
     * @brief Habilita a compressão para um cliente
     * @param clientId ID do cliente
     * @param enabled Habilitar ou desabilitar
     * @return true se alterado com sucesso
     */
    bool setClientCompression(ClientId clientId, bool enabled);
    
    /**
     * @brief Define a autenticação de um cliente
     * @param clientId ID do cliente
     * @param authenticated Autenticado ou não
     * @return true se alterado com sucesso
     */
    bool setClientAuthenticated(ClientId clientId, bool authenticated);
    
    /**
     * @brief Define o nível de segurança global
     * @param level Nível de segurança
     */
    void setGlobalSecurityLevel(PacketSecurityLevel level);
    
    /**
     * @brief Obtém o nível de segurança global
     * @return Nível de segurança
     */
    PacketSecurityLevel getGlobalSecurityLevel() const;
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
    /**
     * @brief Define o modo de log
     * @param logging Modo de log
     */
    void setLoggingEnabled(bool logging);
    
    /**
     * @brief Verifica se o modo de log está ativado
     * @return true se estiver ativado
     */
    bool isLoggingEnabled() const;
    
    /**
     * @brief Registra os manipuladores padrão
     * @return true se registrado com sucesso
     */
    bool registerDefaultHandlers();
    
    /**
     * @brief Cria um pacote
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param dataSize Tamanho dos dados
     * @param packet Buffer para o pacote
     * @param packetSize Tamanho do buffer
     * @return Tamanho do pacote criado
     */
    uint32_t createPacket(uint16_t opcode, const void* data, uint32_t dataSize, 
                         void* packet, uint32_t packetSize);
    
    /**
     * @brief Verifica se um pacote é válido
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se o pacote for válido
     */
    bool isValidPacket(const void* data, uint32_t size);
    
    /**
     * @brief Verifica a autenticação de um cliente
     * @param clientId ID do cliente
     * @return true se estiver autenticado
     */
    bool isClientAuthenticated(ClientId clientId) const;
    
    /**
     * @brief Verifica a criptografia de um cliente
     * @param clientId ID do cliente
     * @return true se estiver com criptografia ativada
     */
    bool isClientEncryptionEnabled(ClientId clientId) const;
    
    /**
     * @brief Verifica a compressão de um cliente
     * @param clientId ID do cliente
     * @return true se estiver com compressão ativada
     */
    bool isClientCompressionEnabled(ClientId clientId) const;
    
    /**
     * @brief Obtém a chave de criptografia de um cliente
     * @param clientId ID do cliente
     * @return Chave de criptografia
     */
    uint32_t getClientEncryptionKey(ClientId clientId) const;
    
private:
    /**
     * @brief Construtor privado
     */
    WYDPacketHandler();
    
    /**
     * @brief Destrutor privado
     */
    ~WYDPacketHandler();
    
    /**
     * @brief Construtor de cópia privado
     */
    WYDPacketHandler(const WYDPacketHandler&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    WYDPacketHandler& operator=(const WYDPacketHandler&) = delete;
    
    /**
     * @brief Valida um pacote
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param handlerInfo Informações do manipulador
     * @return Resultado da validação
     */
    PacketProcessResult validatePacket(ClientId clientId, const uint8_t* data, uint32_t size,
                                      const PacketHandlerInfo* handlerInfo);
    
    /**
     * @brief Verifica a limitação de taxa de um cliente
     * @param clientId ID do cliente
     * @param opcode Código de operação
     * @param rateLimit Limite de taxa
     * @param burstLimit Limite de burst
     * @return true se não estiver limitado
     */
    bool checkRateLimit(ClientId clientId, uint16_t opcode, uint32_t rateLimit, uint32_t burstLimit);
    
    /**
     * @brief Atualiza as estatísticas de pacotes
     * @param result Resultado do processamento
     */
    void updateStats(PacketProcessResult result);
    
    /**
     * @brief Atualiza as informações de rastreamento de um cliente
     * @param clientId ID do cliente
     * @param received Recebido ou enviado
     * @param size Tamanho do pacote
     */
    void updateClientTracking(ClientId clientId, bool received, uint32_t size);
    
    /**
     * @brief Descriptografa um pacote
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se descriptografado com sucesso
     */
    bool decryptPacket(ClientId clientId, uint8_t* data, uint32_t size);
    
    /**
     * @brief Criptografa um pacote
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se criptografado com sucesso
     */
    bool encryptPacket(ClientId clientId, uint8_t* data, uint32_t size);
    
    /**
     * @brief Descompacta um pacote
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param uncompressedData Dados descompactados
     * @param uncompressedSize Tamanho dos dados descompactados
     * @return true se descompactado com sucesso
     */
    bool decompressPacket(ClientId clientId, const uint8_t* data, uint32_t size,
                         uint8_t* uncompressedData, uint32_t& uncompressedSize);
    
    /**
     * @brief Compacta um pacote
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param compressedData Dados compactados
     * @param compressedSize Tamanho dos dados compactados
     * @return true se compactado com sucesso
     */
    bool compressPacket(ClientId clientId, const uint8_t* data, uint32_t size,
                       uint8_t* compressedData, uint32_t& compressedSize);
    
    /**
     * @brief Verifica o checksum de um pacote
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se o checksum for válido
     */
    bool verifyChecksum(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Atualiza o checksum de um pacote
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     */
    void updateChecksum(uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de conexão
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleConnectRequest(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de login
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleLoginRequest(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de desconexão
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleDisconnect(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de lista de personagens
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleCharacterListRequest(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de criação de personagem
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleCharacterCreateRequest(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de exclusão de personagem
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleCharacterDeleteRequest(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de entrada no mundo
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleEnterWorldRequest(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de saída do mundo
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleExitWorld(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de movimento
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleMove(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de chat
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleChatMessage(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de sussurro
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleWhisperMessage(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de ataque
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleAttack(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de uso de habilidade
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleSkillUse(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de cancelamento de habilidade
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleSkillCancel(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de uso de item
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleItemUse(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de movimento de item
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleItemMove(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de equipamento de item
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleItemEquip(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de desequipamento de item
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleItemUnequip(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de descarte de item
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleItemDrop(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de coleta de item
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleItemPickup(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de comando
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handleCommand(ClientId clientId, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Manipulador padrão para pacotes de ping
     * @param clientId ID do cliente
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return Resultado do processamento
     */
    PacketProcessResult handlePing(ClientId clientId, const uint8_t* data, uint32_t size);
    
    // Mapa de manipuladores
    std::unordered_map<uint16_t, PacketHandlerInfo> _handlers;
    
    // Mapa de clientes
    std::unordered_map<ClientId, ClientConnection> _clients;
    
    // Mapa de rastreamento de clientes
    std::unordered_map<ClientId, PacketTrackingInfo> _clientTracking;
    
    // Mapa de limitação de taxa
    std::unordered_map<ClientId, std::unordered_map<uint16_t, std::vector<uint32_t>>> _rateLimits;
    
    // Estatísticas de pacotes
    PacketStats _stats;
    
    // Nível de segurança global
    PacketSecurityLevel _globalSecurityLevel;
    
    // Buffer para descompressão
    std::vector<uint8_t> _decompressBuffer;
    
    // Buffer para compressão
    std::vector<uint8_t> _compressBuffer;
    
    // Estado do gerenciador
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    // Thread safety
    mutable std::mutex _handlersMutex;
    mutable std::mutex _clientsMutex;
    mutable std::mutex _trackingMutex;
    mutable std::mutex _rateLimitMutex;
    mutable std::mutex _statsMutex;
};

} // namespace network
} // namespace wydbr

#endif // _WYD_PACKET_HANDLER_H_