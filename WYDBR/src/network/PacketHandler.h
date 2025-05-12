/**
 * PacketHandler.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/network/PacketHandler.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <map>
#include <functional>
#include <vector>
#include <string>
#include <mutex>
#include <memory>

#include "../core/WYDTypes.h"
#include "PacketTypes.h"

namespace wydbr {


/**
 * @file PacketHandler.h
 * @brief Sistema de tratamento de pacotes do WYD
 * 
 * Este arquivo contém a definição do sistema de tratamento de pacotes,
 * responsável por processar os pacotes recebidos do cliente e enviar respostas.
 * Segue a estrutura original do WYD, mas com melhorias significativas para
 * evitar bugs comuns e vazamentos de memória.
 */

namespace wyd {
namespace network {

// Forward declarations
class CSocketManager;

// Definição do tipo de função callback para tratamento de pacotes
using PacketHandlerFunction = std::function<bool(SOCKET, const Packet&)>;

/**
 * @brief Classe para tratamento de pacotes (original: CPacketHandler)
 * 
 * Esta classe é responsável por gerenciar o tratamento de pacotes recebidos.
 * No WYD original, esta funcionalidade era implementada com uma abordagem mais
 * monolítica e propensa a erros. Esta versão utiliza callbacks modernos e é thread-safe.
 */
/**
 * Classe WYDPacketHandler
 * 
 * Esta classe implementa a funcionalidade WYDPacketHandler conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CPacketHandler {
public:
    /**
     * @brief Construtor
     */
    CPacketHandler();
    
    /**
     * @brief Destrutor
     */
    ~CPacketHandler();
    
    /**
     * @brief Inicializa o handler de pacotes
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool Initialize();
    
    /**
     * @brief Registra um handler para um tipo de pacote específico
     * @param command Código do comando (tipo de pacote)
     * @param handler Função de callback para tratamento
     * @return true se registrado com sucesso, false caso contrário
     */
    bool RegisterHandler(WORD command, PacketHandlerFunction handler);
    
    /**
     * @brief Remove o handler para um tipo de pacote específico
     * @param command Código do comando (tipo de pacote)
     * @return true se removido com sucesso, false caso contrário
     */
    bool UnregisterHandler(WORD command);
    
    /**
     * @brief Processa um pacote recebido
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool ProcessPacket(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Processa um pacote recebido a partir de um buffer de bytes
     * @param clientID ID do cliente (socket)
     * @param buffer Buffer contendo os dados do pacote
     * @param size Tamanho do buffer
     * @return true se processado com sucesso, false caso contrário
     */
    bool ProcessBuffer(SOCKET clientID, const BYTE* buffer, WORD size);
    
    /**
     * @brief Limpa todos os handlers registrados
     */
    void ClearHandlers();
    
    /**
     * @brief Verifica se um pacote é válido
     * @param packet Pacote a verificar
     * @return true se válido, false caso contrário
     */
    bool ValidatePacket(const Packet& packet) const;
    
    /**
     * @brief Obtém estatísticas do processamento de pacotes
     * @param totalProcessed Total de pacotes processados
     * @param successCount Total de pacotes processados com sucesso
     * @param errorCount Total de pacotes com erro de processamento
     * @param unknownCount Total de pacotes com tipo desconhecido
     */
    void GetStats(DWORD& totalProcessed, DWORD& successCount, DWORD& errorCount, DWORD& unknownCount) const;
    
    /**
     * @brief Reseta as estatísticas do processamento de pacotes
     */
    void ResetStats();
    
    /**
     * @brief Define o socket manager para envio de respostas
     * @param socketManager Ponteiro para o socket manager
     */
    void SetSocketManager(CSocketManager* socketManager);
    
private:
    /**
     * @brief Inicializa os handlers padrão
     */
    void InitializeDefaultHandlers();
    
    /**
     * @brief Registra handlers para pacotes de autenticação
     */
    void RegisterAuthHandlers();
    
    /**
     * @brief Registra handlers para pacotes de jogo
     */
    void RegisterGameHandlers();
    
    /**
     * @brief Registra handlers para pacotes de chat
     */
    void RegisterChatHandlers();
    
    /**
     * @brief Registra handlers para pacotes de administração
     */
    void RegisterAdminHandlers();
    
    // Handlers padrão
    
    /**
     * @brief Handler para pacote de login
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleAccountLogin(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de senha numérica (PIN)
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleNumericPassword(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de criação de personagem
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleCharCreate(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de deleção de personagem
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleCharDelete(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de seleção de personagem
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleCharSelect(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de movimentação
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleWalk(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de ataque
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleAttack(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de chat
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleChat(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de susurro (whisper)
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleWhisper(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de uso de habilidade
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleCastSpell(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de teleporte
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleTeleport(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de keep-alive
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleKeepAlive(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Handler para pacote de comando de GM
     * @param clientID ID do cliente (socket)
     * @param packet Pacote recebido
     * @return true se processado com sucesso, false caso contrário
     */
    bool HandleGMCommand(SOCKET clientID, const Packet& packet);
    
    std::map<WORD, PacketHandlerFunction> handlers_;    // Mapeamento de comandos para handlers
    std::mutex handlerMutex_;                          // Mutex para thread safety
    CSocketManager* socketManager_;                    // Socket manager para envio de respostas
    
    // Estatísticas
    DWORD totalProcessed_;    // Total de pacotes processados
    DWORD successCount_;      // Total de pacotes processados com sucesso
    DWORD errorCount_;        // Total de pacotes com erro de processamento
    DWORD unknownCount_;      // Total de pacotes com tipo desconhecido
};

/**
 * @brief Sistema de log especializado para pacotes
 * 
 * Esta classe fornece funcionalidades para logging detalhado de pacotes,
 * útil para debugging e análise de problemas de comunicação.
 */
/**
 * Classe WYDPacketLogger
 * 
 * Esta classe implementa a funcionalidade WYDPacketLogger conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CPacketLogger {
public:
    /**
     * @brief Construtor
     * @param logFilename Nome do arquivo de log
     * @param enableHexDump Habilita dump hexadecimal dos pacotes
     */
    CPacketLogger(const std::string& logFilename = "packets.log", bool enableHexDump = false);
    
    /**
     * @brief Destrutor
     */
    ~CPacketLogger();
    
    /**
     * @brief Inicializa o logger
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o logger
     */
    void Shutdown();
    
    /**
     * @brief Registra um pacote recebido
     * @param clientID ID do cliente
     * @param packet Pacote recebido
     */
    void LogReceivedPacket(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Registra um pacote enviado
     * @param clientID ID do cliente
     * @param packet Pacote enviado
     */
    void LogSentPacket(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Registra um erro de processamento de pacote
     * @param clientID ID do cliente
     * @param packet Pacote com erro
     * @param errorMessage Mensagem de erro
     */
    void LogPacketError(SOCKET clientID, const Packet& packet, const std::string& errorMessage);
    
    /**
     * @brief Habilita ou desabilita o log
     * @param enable true para habilitar, false para desabilitar
     */
    void EnableLogging(bool enable);
    
    /**
     * @brief Habilita ou desabilita o dump hexadecimal
     * @param enable true para habilitar, false para desabilitar
     */
    void EnableHexDump(bool enable);
    
private:
    /**
     * @brief Gera um dump hexadecimal de um buffer
     * @param buffer Buffer a ser "dumpado"
     * @param size Tamanho do buffer
     * @return String contendo o dump hexadecimal
     */
    std::string GenerateHexDump(const BYTE* buffer, WORD size) const;
    
    /**
     * @brief Escreve no arquivo de log
     * @param message Mensagem a ser escrita
     */
    void WriteToLog(const std::string& message);
    
    std::string logFilename_;     // Nome do arquivo de log
    bool enableLogging_;          // Flag para habilitar/desabilitar o log
    bool enableHexDump_;          // Flag para habilitar/desabilitar o dump hexadecimal
    std::mutex logMutex_;         // Mutex para thread safety
    FILE* logFile_;               // Arquivo de log
};

} // namespace network
} // namespace wyd

#endif // PACKETHANDLER_H

} // namespace wydbr
