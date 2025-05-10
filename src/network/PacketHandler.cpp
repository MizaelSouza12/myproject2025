/**
 * @file PacketHandler.cpp
 * @brief Implementação do processador de pacotes de rede
 * 
 * Este arquivo implementa a lógica de processamento de pacotes de rede do WYDBR,
 * com segurança aprimorada contra ataques e exploits.
 */

#include "../../include/Network/NetworkManager.h"
#include "../../include/Network/CryptoManager.h"
#include "../../include/Security/SecurityManager.h"
#include "../../include/Network/PacketHandler.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <ctime>

namespace WYDBR {
namespace Network {

// Matriz de handlers para os diferentes tipos de pacotes
std::unordered_map<PacketType, PacketHandler::PacketHandlerFunc> PacketHandler::handlers_;

void PacketHandler::Initialize() {
    // Registrar handlers de pacotes
    RegisterHandlers();
}

void PacketHandler::RegisterHandlers() {
    // Pacotes de autenticação e login
    handlers_[PacketType::C_LOGIN] = &PacketHandler::HandleLoginRequest;
    
    // Pacotes de movimento e ação
    handlers_[PacketType::C_MOVE] = &PacketHandler::HandleMoveRequest;
    handlers_[PacketType::C_ATTACK] = &PacketHandler::HandleAttackRequest;
    handlers_[PacketType::C_USE_ITEM] = &PacketHandler::HandleUseItemRequest;
    handlers_[PacketType::C_DROP_ITEM] = &PacketHandler::HandleDropItemRequest;
    handlers_[PacketType::C_PICKUP_ITEM] = &PacketHandler::HandlePickupItemRequest;
    
    // Pacotes de chat
    handlers_[PacketType::C_CHAT] = &PacketHandler::HandleChatMessage;
    
    // Pacotes de comércio
    handlers_[PacketType::C_TRADE_REQUEST] = &PacketHandler::HandleTradeRequest;
    handlers_[PacketType::C_TRADE_CONFIRM] = &PacketHandler::HandleTradeConfirm;
    handlers_[PacketType::C_TRADE_CANCEL] = &PacketHandler::HandleTradeCancel;
    
    // Pacotes de grupo
    handlers_[PacketType::C_PARTY_REQUEST] = &PacketHandler::HandlePartyRequest;
    handlers_[PacketType::C_PARTY_CONFIRM] = &PacketHandler::HandlePartyConfirm;
    handlers_[PacketType::C_PARTY_CANCEL] = &PacketHandler::HandlePartyCancel;
    
    // Pacotes de habilidades
    handlers_[PacketType::C_SKILL_USE] = &PacketHandler::HandleSkillUse;
    
    // Pacotes de loja
    handlers_[PacketType::C_SHOP_BUY] = &PacketHandler::HandleShopBuy;
    handlers_[PacketType::C_SHOP_SELL] = &PacketHandler::HandleShopSell;
    
    // Pacotes de administração
    handlers_[PacketType::C_ADMIN_COMMAND] = &PacketHandler::HandleAdminCommand;
    
    // Outros pacotes...
    handlers_[PacketType::C_WARP] = &PacketHandler::HandleWarpRequest;
}

void PacketHandler::ProcessPacket(int conn_index, PacketType packet_type, const uint8_t* packet_data, uint16_t packet_size) {
    // Verificar se existe um handler registrado para este tipo de pacote
    auto it = handlers_.find(packet_type);
    if (it == handlers_.end()) {
        std::cerr << "Sem handler para pacote tipo: " << static_cast<int>(packet_type) << std::endl;
        
        // Registrar tentativa de pacote desconhecido (pode ser ataque)
        Security::SecurityManager::GetInstance().RegisterAttackAttempt(
            "UNKNOWN_PACKET", 2, conn_index, 
            "Unknown packet type: " + std::to_string(static_cast<int>(packet_type))
        );
        
        return;
    }
    
    // Executar o handler
    try {
        // Obter função de handler
        auto handlerFunc = it->second;
        
        // Executar handler
        (handlerFunc)(conn_index, packet_data, packet_size);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao processar pacote: " << e.what() << std::endl;
        
        // Registrar erro de processamento
        Security::SecurityManager::GetInstance().RegisterAttackAttempt(
            "PACKET_PROCESSING_ERROR", 3, conn_index, e.what()
        );
    }
}

// Implementação dos handlers de pacotes

void PacketHandler::HandleLoginRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Verificar tamanho do pacote
    if (packet_size < sizeof(PacketHeader) + sizeof(LoginPacket) - sizeof(PacketHeader)) {
        std::cerr << "Tamanho de pacote de login inválido" << std::endl;
        return;
    }
    
    // Cast para o tipo de pacote apropriado
    const LoginPacket* login = reinterpret_cast<const LoginPacket*>(packet_data);
    
    // Verificar credenciais (exemplo simplificado)
    std::string username(login->username);
    std::string password(login->password);
    
    std::cout << "Pedido de login recebido: " << username << std::endl;
    
    // Sanitizar strings para evitar ataques
    username = Security::SecurityManager::GetInstance().SanitizeSqlInput(username);
    
    // Verificação de versão do cliente
    bool validVersion = VerifyClientVersion(login->clientVersion);
    if (!validVersion) {
        std::cout << "Versão de cliente inválida!" << std::endl;
        SendLoginResponse(conn_index, ResultCode::INVALID_CHARACTER, 0);
        return;
    }
    
    // Autenticação (em um sistema real seria verificado no banco de dados)
    bool auth_success = false;
    uint32_t account_id = 0;
    
    // Exemplo de autenticação
    if (username == "admin" && password == "password") {
        auth_success = true;
        account_id = 1;
    } else if (username == "player" && password == "password") {
        auth_success = true;
        account_id = 2;
    }
    
    // Enviar resposta de login
    if (auth_success) {
        SendLoginResponse(conn_index, ResultCode::SUCCESS, account_id);
    } else {
        SendLoginResponse(conn_index, ResultCode::WRONG_PASSWORD, 0);
    }
}

void PacketHandler::HandleMoveRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Verificar tamanho do pacote
    if (packet_size < sizeof(PacketHeader) + sizeof(MovePacket) - sizeof(PacketHeader)) {
        std::cerr << "Tamanho de pacote de movimento inválido" << std::endl;
        return;
    }
    
    // Cast para o tipo de pacote apropriado
    const MovePacket* move = reinterpret_cast<const MovePacket*>(packet_data);
    
    // Verificar se o movimento é válido
    if (!move->IsValidMove()) {
        std::cerr << "Movimento inválido detectado!" << std::endl;
        
        // Registrar possível tentativa de speed hack
        Security::SecurityManager::GetInstance().RegisterAttackAttempt(
            "INVALID_MOVE", 2, conn_index, "Possible speed hack"
        );
        
        return;
    }
    
    // Processar movimento (em um sistema real seria atualizado no game state)
    std::cout << "Movimento de entidade " << move->entityId 
              << " para (" << move->targetPos.x << ", " << move->targetPos.y << ")" << std::endl;
    
    // Enviar movimento para outros jogadores (broadcast)
    BroadcastEntityMove(move->entityId, move->targetPos, move->moveSpeed, move->runMode == 1);
}

void PacketHandler::HandleAttackRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Pedido de ataque recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleUseItemRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Pedido de uso de item recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleDropItemRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Pedido de descarte de item recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandlePickupItemRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Pedido de coleta de item recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleChatMessage(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Verificar tamanho do pacote
    if (packet_size < sizeof(PacketHeader) + sizeof(ChatPacket) - sizeof(PacketHeader)) {
        std::cerr << "Tamanho de pacote de chat inválido" << std::endl;
        return;
    }
    
    // Cast para o tipo de pacote apropriado
    const ChatPacket* chat = reinterpret_cast<const ChatPacket*>(packet_data);
    
    // Criar cópia modificável para filtragem
    ChatPacket filtered_chat = *chat;
    
    // Aplicar filtro de mensagem para remover conteúdo ofensivo
    filtered_chat.FilterMessage();
    
    // Processar mensagem de chat com base no tipo
    switch (chat->chatType) {
        case 0: // Chat normal
            std::cout << "Chat normal do cliente " << conn_index << ": " << filtered_chat.message << std::endl;
            // Broadcast para jogadores próximos
            BroadcastChatMessage(conn_index, filtered_chat.message, 0);
            break;
            
        case 1: // Sussurro (whisper)
            std::cout << "Sussurro do cliente " << conn_index << " para " 
                      << filtered_chat.targetName << ": " << filtered_chat.message << std::endl;
            // Enviar para jogador específico
            SendWhisperMessage(conn_index, filtered_chat.targetName, filtered_chat.message);
            break;
            
        case 2: // Chat de guilda
            std::cout << "Chat de guilda do cliente " << conn_index << ": " << filtered_chat.message << std::endl;
            // Broadcast para membros da guilda
            BroadcastGuildMessage(conn_index, filtered_chat.message);
            break;
            
        case 3: // Chat de grupo
            std::cout << "Chat de grupo do cliente " << conn_index << ": " << filtered_chat.message << std::endl;
            // Broadcast para membros do grupo
            BroadcastPartyMessage(conn_index, filtered_chat.message);
            break;
            
        case 4: // Chat global
            std::cout << "Chat global do cliente " << conn_index << ": " << filtered_chat.message << std::endl;
            // Verificar permissão para chat global
            if (HasGlobalChatPermission(conn_index)) {
                BroadcastGlobalMessage(conn_index, filtered_chat.message);
            } else {
                // Enviar mensagem de permissão negada
                SendSystemMessage(conn_index, "Você não tem permissão para usar o chat global.");
            }
            break;
            
        default:
            std::cerr << "Tipo de chat inválido: " << chat->chatType << std::endl;
            // Registrar possível manipulação
            Security::SecurityManager::GetInstance().RegisterAttackAttempt(
                "INVALID_CHAT_TYPE", 1, conn_index, 
                "Invalid chat type: " + std::to_string(chat->chatType)
            );
            break;
    }
}

void PacketHandler::HandleTradeRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Pedido de comércio recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleTradeConfirm(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Confirmação de comércio recebida do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleTradeCancel(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Cancelamento de comércio recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandlePartyRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Pedido de grupo recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandlePartyConfirm(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Confirmação de grupo recebida do cliente " << conn_index << std::endl;
}

void PacketHandler::HandlePartyCancel(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Cancelamento de grupo recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleSkillUse(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Uso de habilidade recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleShopBuy(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Compra em loja recebida do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleShopSell(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Venda em loja recebida do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleAdminCommand(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Verificar se o cliente tem privilégios de administrador
    if (!IsAdminUser(conn_index)) {
        std::cerr << "Tentativa não autorizada de comando de administrador do cliente " << conn_index << std::endl;
        
        // Registrar possível tentativa de abuso de privilégios
        Security::SecurityManager::GetInstance().RegisterAttackAttempt(
            "UNAUTHORIZED_ADMIN_COMMAND", 4, conn_index, "Attempt to use admin command without privileges"
        );
        
        SendSystemMessage(conn_index, "Você não tem permissão para executar comandos de administrador.");
        return;
    }
    
    // Implementação básica para demonstração
    std::cout << "Comando de administrador recebido do cliente " << conn_index << std::endl;
}

void PacketHandler::HandleWarpRequest(int conn_index, const uint8_t* packet_data, uint16_t packet_size) {
    // Implementação básica para demonstração
    std::cout << "Pedido de teletransporte recebido do cliente " << conn_index << std::endl;
}

// Funções auxiliares

void PacketHandler::SendLoginResponse(int conn_index, ResultCode result, uint32_t account_id) {
    // Criar pacote de resposta
    LoginResultPacket response;
    response.Initialize(result, account_id);
    
    // Exemplo de preenchimento de personagens (em sistema real seria do banco de dados)
    if (result == ResultCode::SUCCESS) {
        // Exemplos de personagens para a conta
        strncpy(response.characters[0], "Warrior", sizeof(response.characters[0])-1);
        response.characterLevels[0] = 50;
        response.characterClasses[0] = static_cast<uint8_t>(CharacterClass::TK);
        
        strncpy(response.characters[1], "Mage", sizeof(response.characters[1])-1);
        response.characterLevels[1] = 45;
        response.characterClasses[1] = static_cast<uint8_t>(CharacterClass::FM);
        
        // Garantir terminador nulo
        response.characters[0][sizeof(response.characters[0])-1] = '\0';
        response.characters[1][sizeof(response.characters[1])-1] = '\0';
    }
    
    // Enviar resposta
    NetworkManager::GetInstance().SendPacket(conn_index, &response, sizeof(response));
}

void PacketHandler::BroadcastEntityMove(uint32_t entity_id, const Position& position, uint8_t speed, bool running) {
    // Em um sistema real, enviaria para todos os jogadores próximos
    std::cout << "Broadcast de movimento de entidade " << entity_id << std::endl;
}

void PacketHandler::BroadcastChatMessage(int sender_index, const char* message, uint8_t chat_type) {
    // Em um sistema real, enviaria para todos os jogadores próximos
    std::cout << "Broadcast de mensagem de chat" << std::endl;
}

void PacketHandler::SendWhisperMessage(int sender_index, const char* target_name, const char* message) {
    // Em um sistema real, encontraria o jogador alvo e enviaria apenas para ele
    std::cout << "Enviando sussurro para " << target_name << std::endl;
}

void PacketHandler::BroadcastGuildMessage(int sender_index, const char* message) {
    // Em um sistema real, enviaria para todos os membros da mesma guilda
    std::cout << "Broadcast de mensagem de guilda" << std::endl;
}

void PacketHandler::BroadcastPartyMessage(int sender_index, const char* message) {
    // Em um sistema real, enviaria para todos os membros do mesmo grupo
    std::cout << "Broadcast de mensagem de grupo" << std::endl;
}

void PacketHandler::BroadcastGlobalMessage(int sender_index, const char* message) {
    // Em um sistema real, enviaria para todos os jogadores do servidor
    std::cout << "Broadcast de mensagem global" << std::endl;
}

void PacketHandler::SendSystemMessage(int conn_index, const char* message) {
    // Em um sistema real, enviaria uma mensagem de sistema para o jogador
    std::cout << "Enviando mensagem de sistema para cliente " << conn_index << ": " << message << std::endl;
}

bool PacketHandler::HasGlobalChatPermission(int conn_index) {
    // Em um sistema real, verificaria permissões específicas do jogador
    // Por enquanto, retornar true para fins de teste
    return true;
}

bool PacketHandler::IsAdminUser(int conn_index) {
    // Em um sistema real, verificaria se o usuário é admin no banco de dados
    // Por enquanto, verificação básica para teste (apenas conexão índice 0 é admin)
    return conn_index == 0;
}

bool PacketHandler::VerifyClientVersion(const uint8_t* version) {
    // Versão mínima aceita (exemplo: 1.2.3.4)
    const uint8_t MIN_VERSION[4] = {1, 0, 0, 0};
    
    // Comparar cada componente da versão
    for (int i = 0; i < 4; i++) {
        if (version[i] < MIN_VERSION[i]) {
            return false; // Versão muito antiga
        } else if (version[i] > MIN_VERSION[i]) {
            return true; // Versão mais nova
        }
    }
    
    return true; // Versão exata
}

} // namespace Network
} // namespace WYDBR