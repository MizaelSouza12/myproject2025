/**
 * @file PacketHandler.h
 * @brief Processador de pacotes de rede para WYDBR 2.0
 * 
 * Define a classe PacketHandler, responsável por processar
 * pacotes de rede e implementar a lógica de negócio correspondente.
 */

#ifndef WYDBR_PACKET_HANDLER_H
#define WYDBR_PACKET_HANDLER_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>
#include "../../headers/packet_struct.h"
#include "../../headers/wyd_core.h"

namespace WYDBR {
namespace Network {

/**
 * @class PacketHandler
 * @brief Gerencia o processamento de pacotes de rede
 */
class PacketHandler {
public:
    // Tipo de função handler para pacotes
    typedef void (*PacketHandlerFunc)(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    
    /**
     * @brief Inicializa o sistema de handlers de pacotes
     */
    static void Initialize();
    
    /**
     * @brief Processa um pacote de rede
     * @param conn_index Índice da conexão
     * @param packet_type Tipo de pacote
     * @param packet_data Dados do pacote
     * @param packet_size Tamanho do pacote
     */
    static void ProcessPacket(int conn_index, PacketType packet_type, const uint8_t* packet_data, uint16_t packet_size);
    
private:
    // Registro de handlers
    static void RegisterHandlers();
    
    // Mapa de tipos de pacotes para funções de handler
    static std::unordered_map<PacketType, PacketHandlerFunc> handlers_;
    
    // Handlers para diferentes tipos de pacotes
    static void HandleLoginRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleMoveRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleAttackRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleUseItemRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleDropItemRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandlePickupItemRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleChatMessage(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleTradeRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleTradeConfirm(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleTradeCancel(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandlePartyRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandlePartyConfirm(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandlePartyCancel(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleSkillUse(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleShopBuy(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleShopSell(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleAdminCommand(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    static void HandleWarpRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size);
    
    // Métodos auxiliares
    static void SendLoginResponse(int conn_index, ResultCode result, uint32_t account_id);
    static void BroadcastEntityMove(uint32_t entity_id, const Position& position, uint8_t speed, bool running);
    static void BroadcastChatMessage(int sender_index, const char* message, uint8_t chat_type);
    static void SendWhisperMessage(int sender_index, const char* target_name, const char* message);
    static void BroadcastGuildMessage(int sender_index, const char* message);
    static void BroadcastPartyMessage(int sender_index, const char* message);
    static void BroadcastGlobalMessage(int sender_index, const char* message);
    static void SendSystemMessage(int conn_index, const char* message);
    static bool HasGlobalChatPermission(int conn_index);
    static bool IsAdminUser(int conn_index);
    static bool VerifyClientVersion(const uint8_t* version);
};

} // namespace Network
} // namespace WYDBR

#endif // WYDBR_PACKET_HANDLER_H