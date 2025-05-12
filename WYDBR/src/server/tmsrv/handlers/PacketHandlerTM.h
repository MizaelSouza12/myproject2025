/**
 * PacketHandlerTM.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/handlers/PacketHandlerTM.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PACKET_HANDLER_TM_H
#define PACKET_HANDLER_TM_H

#include <memory>
#include <string>
#include "headers/wyd_core.h"

namespace wydbr {


class TMServer;

/**
 * Manipulador de pacotes específico para o TMSrv
 */
/**
 * Classe WYDPacketHandlerTM
 * 
 * Esta classe implementa a funcionalidade WYDPacketHandlerTM conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PacketHandlerTM {
public:
    // Construtor
    /**
 * PacketHandlerTM
 * 
 * Implementa a funcionalidade PacketHandlerTM conforme especificação original.
 * @param server Parâmetro server
 * @return Retorna explicit
 */

    explicit PacketHandlerTM(TMServer* server);
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @return Retorna bool
 */

    bool initialize();
    
    // Shutdown
    /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 */

    void shutdown();
    
    // Manipuladores específicos
    /**
 * handleLogin
 * 
 * Implementa a funcionalidade handleLogin conforme especificação original.
 * @param clientId Parâmetro clientId
 * @param packet Parâmetro packet
 */

    void handleLogin(uint32_t clientId, const WYDPacket& packet);
    /**
 * handleMovement
 * 
 * Implementa a funcionalidade handleMovement conforme especificação original.
 * @param clientId Parâmetro clientId
 * @param packet Parâmetro packet
 */

    void handleMovement(uint32_t clientId, const WYDPacket& packet);
    /**
 * handleChat
 * 
 * Implementa a funcionalidade handleChat conforme especificação original.
 * @param clientId Parâmetro clientId
 * @param packet Parâmetro packet
 */

    void handleChat(uint32_t clientId, const WYDPacket& packet);
    /**
 * handleAttack
 * 
 * Implementa a funcionalidade handleAttack conforme especificação original.
 * @param clientId Parâmetro clientId
 * @param packet Parâmetro packet
 */

    void handleAttack(uint32_t clientId, const WYDPacket& packet);
    /**
 * handleItemUse
 * 
 * Implementa a funcionalidade handleItemUse conforme especificação original.
 * @param clientId Parâmetro clientId
 * @param packet Parâmetro packet
 */

    void handleItemUse(uint32_t clientId, const WYDPacket& packet);
    /**
 * handlePing
 * 
 * Implementa a funcionalidade handlePing conforme especificação original.
 * @param clientId Parâmetro clientId
 * @param packet Parâmetro packet
 */

    void handlePing(uint32_t clientId, const WYDPacket& packet);
    
private:
    // Referência para o servidor
    TMServer* server_;
    
    // Métodos de utilidade
    /**
 * sendErrorMessage
 * 
 * Implementa a funcionalidade sendErrorMessage conforme especificação original.
 * @param clientId Parâmetro clientId
 * @param message Parâmetro message
 */

    void sendErrorMessage(uint32_t clientId, const std::string& message);
    /**
 * sendSystemMessage
 * 
 * Implementa a funcionalidade sendSystemMessage conforme especificação original.
 * @param clientId Parâmetro clientId
 * @param message Parâmetro message
 */

    void sendSystemMessage(uint32_t clientId, const std::string& message);
    /**
 * broadcastMessage
 * 
 * Implementa a funcionalidade broadcastMessage conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param radius Parâmetro radius
 * @param message Parâmetro message
 */

    void broadcastMessage(uint16_t mapId, uint16_t x, uint16_t y, uint16_t radius, const std::string& message);
};

#endif // PACKET_HANDLER_TM_H

} // namespace wydbr
