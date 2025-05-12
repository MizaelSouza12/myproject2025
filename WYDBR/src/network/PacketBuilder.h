/**
 * PacketBuilder.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/network/PacketBuilder.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PACKETBUILDER_H
#define PACKETBUILDER_H

#include <string>
#include <vector>
#include <cstring>

#include "../core/WYDTypes.h"
#include "PacketTypes.h"

namespace wydbr {


/**
 * @file PacketBuilder.h
 * @brief Utilitário para construção de pacotes do WYD
 * 
 * Este arquivo contém utilitários para facilitar a construção de pacotes
 * complexos do WYD, mantendo a compatibilidade com o protocolo original
 * enquanto adiciona melhorias de segurança e facilidade de uso.
 */

namespace wyd {
namespace network {

/**
 * @brief Classe utilitária para construção de pacotes (original: não existia no WYD)
 * 
 * Esta classe facilita a construção de pacotes complexos, especialmente
 * os que envolvem múltiplos campos ou estruturas aninhadas.
 * No WYD original, pacotes eram construídos manualmente, o que era propenso a erros.
 */
/**
 * Classe WYDPacketBuilder
 * 
 * Esta classe implementa a funcionalidade WYDPacketBuilder conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CPacketBuilder {
public:
    /**
     * @brief Construtor
     * @param command Comando (tipo de pacote)
     * @param clientKey Chave do cliente (opcional)
     */
    CPacketBuilder(WORD command, DWORD clientKey = 0)
        /**
 * packet_
 * 
 * Implementa a funcionalidade packet_ conforme especificação original.
 * @param command Parâmetro command
 * @param clientKey Parâmetro clientKey
 * @return Retorna :
 */

        : packet_(command, clientKey){
    }
    
    /**
     * @brief Construtor de cópia
     * @param other Outro builder
     */
    CPacketBuilder(const CPacketBuilder& other)
        /**
 * packet_
 * 
 * Implementa a funcionalidade packet_ conforme especificação original.
 * @param other.packet_ Parâmetro other.packet_
 * @return Retorna :
 */

        : packet_(other.packet_){
    }
    
    /**
     * @brief Operador de atribuição
     * @param other Outro builder
     * @return Referência para este builder
     */
    CPacketBuilder& operator=(const CPacketBuilder& other) {
        if (this != &other) {
            packet_ = other.packet_;
        }
        return *this;
    }
    
    /**
     * @brief Adiciona um byte
     * @param value Valor
     * @return Referência para este builder
     */
    CPacketBuilder& AddByte(BYTE value) {
        packet_.AppendData(&value, sizeof(BYTE));
        return *this;
    }
    
    /**
     * @brief Adiciona um word (2 bytes)
     * @param value Valor
     * @return Referência para este builder
     */
    CPacketBuilder& AddWord(WORD value) {
        packet_.AppendData(&value, sizeof(WORD));
        return *this;
    }
    
    /**
     * @brief Adiciona um dword (4 bytes)
     * @param value Valor
     * @return Referência para este builder
     */
    CPacketBuilder& AddDword(DWORD value) {
        packet_.AppendData(&value, sizeof(DWORD));
        return *this;
    }
    
    /**
     * @brief Adiciona um qword (8 bytes)
     * @param value Valor
     * @return Referência para este builder
     */
    CPacketBuilder& AddQword(QWORD value) {
        packet_.AppendData(&value, sizeof(QWORD));
        return *this;
    }
    
    /**
     * @brief Adiciona uma string com tamanho fixo
     * @param str String a adicionar
     * @param fixedSize Tamanho fixo (se a string for menor, será preenchida com zeros)
     * @return Referência para este builder
     */
    CPacketBuilder& AddString(const std::string& str, size_t fixedSize) {
        // Aloca buffer temporário
        std::vector<char> buffer(fixedSize, 0);
        
        // Copia a string para o buffer
        if (!str.empty()) {
            size_t copySize = std::min(str.size(), fixedSize - 1);  // -1 para garantir null no final
            memcpy(buffer.data(), str.c_str(), copySize);
        }
        
        // Adiciona o buffer ao pacote
        packet_.AppendData(buffer.data(), static_cast<WORD>(fixedSize));
        
        return *this;
    }
    
    /**
     * @brief Adiciona uma string terminada em null
     * @param str String a adicionar
     * @return Referência para este builder
     */
    CPacketBuilder& AddString(const std::string& str) {
        // Adiciona a string + null terminator
        if (!str.empty()) {
            packet_.AppendData(str.c_str(), static_cast<WORD>(str.size() + 1));
        }
        else {
            // String vazia, adiciona apenas null
            BYTE null = 0;
            packet_.AppendData(&null, sizeof(BYTE));
        }
        
        return *this;
    }
    
    /**
     * @brief Adiciona um buffer de bytes
     * @param buffer Buffer
     * @param size Tamanho do buffer
     * @return Referência para este builder
     */
    CPacketBuilder& AddBuffer(const void* buffer, WORD size) {
        if (buffer && size > 0) {
            packet_.AppendData(buffer, size);
        }
        
        return *this;
    }
    
    /**
     * @brief Adiciona dados estruturados
     * @param data Ponteiro para os dados
     * @param size Tamanho dos dados
     * @return Referência para este builder
     */
    template<typename T>
    CPacketBuilder& AddStruct(const T& data) {
        packet_.AppendData(&data, sizeof(T));
        return *this;
    }
    
    /**
     * @brief Finaliza a construção e retorna o pacote
     * @return Pacote construído
     */
    Packet Build() {
        // Calcula o checksum antes de retornar
        packet_.CalculateChecksum();
        
        return packet_;
    }
    
    /**
     * @brief Define o comando do pacote
     * @param command Comando
     * @return Referência para este builder
     */
    CPacketBuilder& SetCommand(WORD command) {
        packet_.SetCommand(command);
        return *this;
    }
    
    /**
     * @brief Define a chave do cliente
     * @param clientKey Chave do cliente
     * @return Referência para este builder
     */
    CPacketBuilder& SetClientKey(DWORD clientKey) {
        packet_.SetClientKey(clientKey);
        return *this;
    }
    
    /**
     * @brief Limpa os dados do pacote
     * @return Referência para este builder
     */
    CPacketBuilder& Clear() {
        packet_.Clear();
        return *this;
    }
    
private:
    Packet packet_;  // Pacote sendo construído
};

/**
 * @brief Cria um pacote de login de conta
 * @param accountName Nome da conta
 * @param password Senha
 * @param clientVersion Versão do cliente
 * @param uniqueID ID único do computador
 * @return Pacote construído
 */
inline /**
 * CreateAccountLoginPacket
 * 
 * Implementa a funcionalidade CreateAccountLoginPacket conforme especificação original.
 * @param accountName Parâmetro accountName
 * @param password Parâmetro password
 * @param clientVersion Parâmetro clientVersion
 * @param nullptr Parâmetro nullptr
 * @return Retorna Packet
 */
 Packet CreateAccountLoginPacket(const std::string& accountName, 
                                      const std::string& password, 
                                      const std::string& clientVersion,
                                      const BYTE* uniqueID = nullptr){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param PKT_ACCOUNT_LOGIN Parâmetro PKT_ACCOUNT_LOGIN
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(PKT_ACCOUNT_LOGIN);
    
    // Criar estrutura de login
    STRUCT_PACKET_ACCOUNTLOGIN loginData;
    loginData.SetAccountName(accountName.c_str());
    loginData.SetPassword(password.c_str());
    loginData.SetClientVersion(clientVersion.c_str());
    
    // Define ID único, se /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param uniqueID Parâmetro uniqueID
 * @return Retorna fornecido
 */
 fornecido
    if(uniqueID){
        memcpy(loginData.byUnique, uniqueID, sizeof(loginData.byUnique));
    }
    
    // Adiciona a estrutura e constrói o pacote
    return builder.AddStruct(loginData).Build();
}

/**
 * @brief Cria um pacote de resposta de login
 * @param result Resultado do login
 * @param message Mensagem (opcional)
 * @param clientKey Chave do cliente
 * @return Pacote construído
 */
inline /**
 * CreateLoginResultPacket
 * 
 * Implementa a funcionalidade CreateLoginResultPacket conforme especificação original.
 * @param result Parâmetro result
 * @param message Parâmetro message
 * @param clientKey Parâmetro clientKey
 * @return Retorna Packet
 */
 Packet CreateLoginResultPacket(LOGIN_RESULT result, const std::string& message, DWORD clientKey){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param PKT_ACCOUNT_LOGIN_RESULT Parâmetro PKT_ACCOUNT_LOGIN_RESULT
 * @param clientKey Parâmetro clientKey
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(PKT_ACCOUNT_LOGIN_RESULT, clientKey);
    
    // Criar estrutura de resultado
    STRUCT_PACKET_ACCOUNTLOGINRESULT resultData;
    resultData.SetResult(result);
    
    if (!message.empty()) {
        resultData.SetMessage(message.c_str());
    }
    
    // Adiciona a estrutura e constrói o pacote
    return builder.AddStruct(resultData).Build();
}

/**
 * @brief Cria um pacote de chat
 * @param message Mensagem de chat
 * @param clientKey Chave do cliente
 * @return Pacote construído
 */
inline /**
 * CreateChatPacket
 * 
 * Implementa a funcionalidade CreateChatPacket conforme especificação original.
 * @param message Parâmetro message
 * @param clientKey Parâmetro clientKey
 * @return Retorna Packet
 */
 Packet CreateChatPacket(const std::string& message, DWORD clientKey){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param PKT_CHAT Parâmetro PKT_CHAT
 * @param clientKey Parâmetro clientKey
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(PKT_CHAT, clientKey);
    
    // Criar estrutura de chat
    STRUCT_PACKET_CHAT chatData;
    chatData.SetMessage(message.c_str());
    
    // Adiciona a estrutura e constrói o pacote
    return builder.AddStruct(chatData).Build();
}

/**
 * @brief Cria um pacote de teleporte
 * @param mapID ID do mapa
 * @param posX Posição X
 * @param posY Posição Y
 * @param isValid Flag de validação
 * @param clientKey Chave do cliente
 * @return Pacote construído
 */
inline /**
 * CreateTeleportPacket
 * 
 * Implementa a funcionalidade CreateTeleportPacket conforme especificação original.
 * @param mapID Parâmetro mapID
 * @param posX Parâmetro posX
 * @param posY Parâmetro posY
 * @param isValid Parâmetro isValid
 * @param clientKey Parâmetro clientKey
 * @return Retorna Packet
 */
 Packet CreateTeleportPacket(WORD mapID, WORD posX, WORD posY, DWORD isValid, DWORD clientKey){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param PKT_TELEPORT Parâmetro PKT_TELEPORT
 * @param clientKey Parâmetro clientKey
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(PKT_TELEPORT, clientKey);
    
    // Criar estrutura de teleporte
    STRUCT_PACKET_TELEPORT teleportData;
    teleportData.wMapID = mapID;
    teleportData.wPosX = posX;
    teleportData.wPosY = posY;
    teleportData.dwIsValid = isValid;
    
    // Adiciona a estrutura e constrói o pacote
    return builder.AddStruct(teleportData).Build();
}

/**
 * @brief Cria um pacote de resposta de teleporte
 * @param mapID ID do mapa
 * @param posX Posição X
 * @param posY Posição Y
 * @param isValid Flag de validação
 * @param clientKey Chave do cliente
 * @return Pacote construído
 */
inline /**
 * CreateTeleportReplyPacket
 * 
 * Implementa a funcionalidade CreateTeleportReplyPacket conforme especificação original.
 * @param mapID Parâmetro mapID
 * @param posX Parâmetro posX
 * @param posY Parâmetro posY
 * @param isValid Parâmetro isValid
 * @param clientKey Parâmetro clientKey
 * @return Retorna Packet
 */
 Packet CreateTeleportReplyPacket(WORD mapID, WORD posX, WORD posY, DWORD isValid, DWORD clientKey){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param PKT_TELEPORT_REPLY Parâmetro PKT_TELEPORT_REPLY
 * @param clientKey Parâmetro clientKey
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(PKT_TELEPORT_REPLY, clientKey);
    
    // Criar estrutura de teleporte
    STRUCT_PACKET_TELEPORT teleportData;
    teleportData.wMapID = mapID;
    teleportData.wPosX = posX;
    teleportData.wPosY = posY;
    teleportData.dwIsValid = isValid;
    
    // Adiciona a estrutura e constrói o pacote
    return builder.AddStruct(teleportData).Build();
}

/**
 * @brief Cria um pacote de atualização de posição/movimento
 * @param posX Posição X
 * @param posY Posição Y
 * @param direction Direção (0-7)
 * @param moveType Tipo de movimento (0=andar, 1=correr)
 * @param clientKey Chave do cliente
 * @return Pacote construído
 */
inline /**
 * CreateWalkPacket
 * 
 * Implementa a funcionalidade CreateWalkPacket conforme especificação original.
 * @param posX Parâmetro posX
 * @param posY Parâmetro posY
 * @param direction Parâmetro direction
 * @param moveType Parâmetro moveType
 * @param clientKey Parâmetro clientKey
 * @return Retorna Packet
 */
 Packet CreateWalkPacket(WORD posX, WORD posY, BYTE direction, BYTE moveType, DWORD clientKey){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param PKT_WALK Parâmetro PKT_WALK
 * @param clientKey Parâmetro clientKey
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(PKT_WALK, clientKey);
    
    // Criar estrutura de movimento
    STRUCT_PACKET_WALK walkData;
    walkData.wPosX = posX;
    walkData.wPosY = posY;
    walkData.byDirection = direction;
    walkData.byType = moveType;
    
    // Adiciona a estrutura e constrói o pacote
    return builder.AddStruct(walkData).Build();
}

/**
 * @brief Cria um pacote de erro genérico
 * @param errorCode Código de erro
 * @param message Mensagem de erro
 * @param clientKey Chave do cliente
 * @return Pacote construído
 */
inline /**
 * CreateErrorPacket
 * 
 * Implementa a funcionalidade CreateErrorPacket conforme especificação original.
 * @param errorCode Parâmetro errorCode
 * @param message Parâmetro message
 * @param clientKey Parâmetro clientKey
 * @return Retorna Packet
 */
 Packet CreateErrorPacket(WORD errorCode, const std::string& message, DWORD clientKey){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param errorCode Parâmetro errorCode
 * @param clientKey Parâmetro clientKey
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(errorCode, clientKey);
    
    // Adiciona a mensagem de erro
    builder.AddString(message);
    
    // Constrói o pacote
    return builder.Build();
}

/**
 * @brief Cria um pacote de keep-alive (heartbeat)
 * @param clientKey Chave do cliente
 * @return Pacote construído
 */
inline /**
 * CreateKeepAlivePacket
 * 
 * Implementa a funcionalidade CreateKeepAlivePacket conforme especificação original.
 * @param clientKey Parâmetro clientKey
 * @return Retorna Packet
 */
 Packet CreateKeepAlivePacket(DWORD clientKey){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param PKT_KEEPALIVE Parâmetro PKT_KEEPALIVE
 * @param clientKey Parâmetro clientKey
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(PKT_KEEPALIVE, clientKey);
    
    // No WYD original, este pacote era apenas o cabeçalho
    
    // Constrói o pacote
    return builder.Build();
}

/**
 * @brief Cria um pacote para desconexão
 * @param reason Motivo da desconexão (0=normal, 1=kickado, 2=banido, etc)
 * @param clientKey Chave do cliente
 * @return Pacote construído
 */
inline /**
 * CreateDisconnectPacket
 * 
 * Implementa a funcionalidade CreateDisconnectPacket conforme especificação original.
 * @param reason Parâmetro reason
 * @param clientKey Parâmetro clientKey
 * @return Retorna Packet
 */
 Packet CreateDisconnectPacket(BYTE reason, DWORD clientKey){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param PKT_DISCONNECT Parâmetro PKT_DISCONNECT
 * @param clientKey Parâmetro clientKey
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(PKT_DISCONNECT, clientKey);
    
    // Adiciona o motivo da desconexão
    builder.AddByte(reason);
    
    // Constrói o pacote
    return builder.Build();
}

/**
 * @brief Cria um pacote com dados do mundo/mapa
 * @param weatherType Tipo de clima
 * @param timeOfDay Hora do dia (0-23)
 * @param clientKey Chave do cliente
 * @return Pacote construído
 */
inline /**
 * CreateWeatherPacket
 * 
 * Implementa a funcionalidade CreateWeatherPacket conforme especificação original.
 * @param weatherType Parâmetro weatherType
 * @param timeOfDay Parâmetro timeOfDay
 * @param clientKey Parâmetro clientKey
 * @return Retorna Packet
 */
 Packet CreateWeatherPacket(BYTE weatherType, BYTE timeOfDay, DWORD clientKey){
    /**
 * builder
 * 
 * Implementa a funcionalidade builder conforme especificação original.
 * @param PKT_WEATHER Parâmetro PKT_WEATHER
 * @param clientKey Parâmetro clientKey
 * @return Retorna CPacketBuilder
 */

    CPacketBuilder builder(PKT_WEATHER, clientKey);
    
    // Adiciona informações de clima
    builder.AddByte(weatherType);
    builder.AddByte(timeOfDay);
    
    // Constrói o pacote
    return builder.Build();
}

} // namespace network
} // namespace wyd

#endif // PACKETBUILDER_H

} // namespace wydbr
