/**
 * PacketHandler.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/network/PacketHandler.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "PacketHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <cstring>

#include "../core/Logger.h"
#include "../core/Utils.h"
#include "SocketManager.h"
#include "PacketBuilder.h"
#include "../server/tmsrv/player/Player.h"
#include "../server/tmsrv/TMServer.h"

namespace wydbr {


/**
 * @file PacketHandler.cpp
 * @brief Implementação do sistema de tratamento de pacotes do WYD
 * 
 * Este arquivo contém a implementação do sistema de tratamento de pacotes.
 * Mantém a estrutura do WYD original enquanto incorpora melhorias significativas
 * para tratar bugs conhecidos e garantir maior segurança e desempenho.
 */

namespace wyd {

} // namespace wydbr

namespace network {

// Implementação de CPacketHandler

CPacketHandler::CPacketHandler()
    : socketManager_(nullptr)
    , totalProcessed_(0)
    , successCount_(0)
    , errorCount_(0)
    , unknownCount_(0)
{
}

CPacketHandler::~CPacketHandler() {
    ClearHandlers();
}

bool CPacketHandler::Initialize() {
    // Limpa handlers /**
 * ClearHandlers
 * 
 * Implementa a funcionalidade ClearHandlers conforme especificação original.
 * @return Retorna existentes
 */
 existentes
    ClearHandlers();
    
    // Inicializa os handlers padrã/**
 * InitializeDefaultHandlers
 * 
 * Implementa a funcionalidade InitializeDefaultHandlers conforme especificação original.
 * @return Retorna o
 */
o
    InitializeDefaultHandlers();
    
    LOG_INFO("Packet Handler inicializado com {} handlers", handlers_.size());
    
    return true;
}

bool CPacketHandler::RegisterHandler(WORD command, PacketHandlerFunction handler) {
    if (!handler) {
        LOG_ERROR("Tentativa de registrar handler nulo para comando 0x{:04X}", command);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(handlerMutex_);
    
    // Verifica se já existe um handler para este comando
    if (handlers_.find(command) != handlers_.end()) {
        LOG_WARNING("Substituindo handler existente para comando 0x{:04X}", command);
    }
    
    // Registra o handler
    handlers_[command] = handler;
    
    return true;
}

bool CPacketHandler::UnregisterHandler(WORD command) {
    std::lock_guard<std::mutex> lock(handlerMutex_);
    
    // Busca o handler
    auto it = handlers_.find(command);
    if (it == handlers_.end()) {
        LOG_WARNING("Tentativa de remover handler inexistente para comando 0x{:04X}", command);
        return false;
    }
    
    // Remove o handler
    handlers_.erase(it);
    
    return true;
}

bool CPacketHandler::ProcessPacket(SOCKET clientID, const Packet& packet) {
    // Incrementa contadores
    totalProcessed_++;
    
    // Obtém o comando do pacote
    WORD command = packet.GetCommand();
    
    // Valida o pacote
    if (!ValidatePacket(packet)) {
        LOG_ERROR("Pacote inválido recebido do cliente {}: comando 0x{:04X}, tamanho {}", 
            clientID, command, packet.GetSize());
        errorCount_++;
        return false;
    }
    
    // Busca o handler para este comando
    PacketHandlerFunction handler;
    {
        std::lock_guard<std::mutex> lock(handlerMutex_);
        auto it = handlers_.find(command);
        if (it == handlers_.end()) {
            LOG_WARNING("Comando desconhecido 0x{:04X} recebido do cliente {}", command, clientID);
            unknownCount_++;
            return false;
        }
        
        handler = it->second;
    }
    
    // Chama o handler
    try {
        if (handler(clientID, packet)) {
            successCount_++;
            return true;
        }
        else {
            LOG_ERROR("Falha ao processar pacote do cliente {}: comando 0x{:04X}", clientID, command);
            errorCount_++;
            return false;
        }
    }
    catch (const std::exception& ex) {
        LOG_ERROR("Exceção ao processar pacote do cliente {}: comando 0x{:04X}, erro: {}", 
            clientID, command, ex.what());
        errorCount_++;
        return false;
    }
}

bool CPacketHandler::ProcessBuffer(SOCKET clientID, const BYTE* buffer, WORD size) {
    if (!buffer || size < HEADER_SIZE) {
        LOG_ERROR("Buffer inválido recebido do cliente {}: tamanho {}", clientID, size);
        errorCount_++;
        return false;
    }
    
    // Converte o buffer para um pacote
    Packet packet;
    if (!packet.Deserialize(buffer, size)) {
        LOG_ERROR("Falha ao deserializar pacote do cliente {}: tamanho {}", clientID, size);
        errorCount_++;
        return false;
    }
    
    // Processa o pacote
    /**
 * ProcessPacket
 * 
 * Implementa a funcionalidade ProcessPacket conforme especificação original.
 * @param clientID Parâmetro clientID
 * @param packet Parâmetro packet
 * @return Retorna return
 */

    return ProcessPacket(clientID, packet);
}

void CPacketHandler::ClearHandlers() {
    std::lock_guard<std::mutex> lock(handlerMutex_);
    handlers_.clear();
}

bool CPacketHandler::ValidatePacket(const Packet& packet) const {
    // Verifica tamanho mínimo
    if (packet.GetSize() < HEADER_SIZE) {
        return false;
    }
    
    // Verifica tamanho máximo
    if (packet.GetSize() > MAX_PACKET_SIZE) {
        return false;
    }
    
    // Verifica comando válido (opcional, depende da implementação)
    WORD command = packet.GetCommand();
    if (command == 0) {
        return false;
    }
    
    // Verifica consistência do tamanho
    if (packet.GetHeader().wSize != packet.GetSize()) {
        return false;
    }
    
    // Não valida checksum aqui, pois depende da implementação específica
    // e algumas versões do WYD não usavam checksum de forma consistente
    
    return true;
}

void CPacketHandler::GetStats(DWORD& totalProcessed, DWORD& successCount, 
                             DWORD& errorCount, DWORD& unknownCount) const {
    totalProcessed = totalProcessed_;
    successCount = successCount_;
    errorCount = errorCount_;
    unknownCount = unknownCount_;
}

void CPacketHandler::ResetStats() {
    totalProcessed_ = 0;
    successCount_ = 0;
    errorCount_ = 0;
    unknownCount_ = 0;
}

void CPacketHandler::SetSocketManager(CSocketManager* socketManager) {
    socketManager_ = socketManager;
}

void CPacketHandler::InitializeDefaultHandlers() {
    // Registra os handlers para diferentes categorias de /**
 * RegisterAuthHandlers
 * 
 * Implementa a funcionalidade RegisterAuthHandlers conforme especificação original.
 * @return Retorna pacotes
 */
 pacotes
    RegisterAuthHandlers();
    RegisterGameHandlers();
    RegisterChatHandlers();
    RegisterAdminHandlers();
}

void CPacketHandler::RegisterAuthHandlers() {
    // Handlers de autenticaçã/**
 * RegisterHandler
 * 
 * Implementa a funcionalidade RegisterHandler conforme especificação original.
 * @param PKT_ACCOUNT_LOGIN Parâmetro PKT_ACCOUNT_LOGIN
 * @param clientID Parâmetro clientID
 * @param packet Parâmetro packet
 * @return Retorna o
 */
o
    RegisterHandler(PKT_ACCOUNT_LOGIN, 
        [this](SOCKET clientID, const Packet& packet){ 
            return this->HandleAccountLogin(clientID, packet); 
        });
        
    RegisterHandler(PKT_NUMERIC_PASSWORD, 
        [this](SOCKET clientID, const Packet& packet) { 
            return this->HandleNumericPassword(clientID, packet); 
        });
        
    RegisterHandler(PKT_CHAR_CREATE, 
        [this](SOCKET clientID, const Packet& packet) { 
            return this->HandleCharCreate(clientID, packet); 
        });
        
    RegisterHandler(PKT_CHAR_DELETE, 
        [this](SOCKET clientID, const Packet& packet) { 
            return this->HandleCharDelete(clientID, packet); 
        });
        
    RegisterHandler(PKT_CHAR_SELECT, 
        [this](SOCKET clientID, const Packet& packet) { 
            return this->HandleCharSelect(clientID, packet); 
        });
}

void CPacketHandler::RegisterGameHandlers() {
    // Handlers de /**
 * RegisterHandler
 * 
 * Implementa a funcionalidade RegisterHandler conforme especificação original.
 * @param PKT_WALK Parâmetro PKT_WALK
 * @param clientID Parâmetro clientID
 * @param packet Parâmetro packet
 * @return Retorna jogabilidade
 */
 jogabilidade
    RegisterHandler(PKT_WALK, 
        [this](SOCKET clientID, const Packet& packet){ 
            return this->HandleWalk(clientID, packet); 
        });
        
    RegisterHandler(PKT_ATTACK, 
        [this](SOCKET clientID, const Packet& packet) { 
            return this->HandleAttack(clientID, packet); 
        });
        
    RegisterHandler(PKT_CAST_SPELL, 
        [this](SOCKET clientID, const Packet& packet) { 
            return this->HandleCastSpell(clientID, packet); 
        });
        
    RegisterHandler(PKT_TELEPORT, 
        [this](SOCKET clientID, const Packet& packet) { 
            return this->HandleTeleport(clientID, packet); 
        });
        
    RegisterHandler(PKT_KEEPALIVE, 
        [this](SOCKET clientID, const Packet& packet) { 
            return this->HandleKeepAlive(clientID, packet); 
        });
}

void CPacketHandler::RegisterChatHandlers() {
    // Handlers de /**
 * RegisterHandler
 * 
 * Implementa a funcionalidade RegisterHandler conforme especificação original.
 * @param PKT_CHAT Parâmetro PKT_CHAT
 * @param clientID Parâmetro clientID
 * @param packet Parâmetro packet
 * @return Retorna chat
 */
 chat
    RegisterHandler(PKT_CHAT, 
        [this](SOCKET clientID, const Packet& packet){ 
            return this->HandleChat(clientID, packet); 
        });
        
    RegisterHandler(PKT_WHISPER, 
        [this](SOCKET clientID, const Packet& packet) { 
            return this->HandleWhisper(clientID, packet); 
        });
}

void CPacketHandler::RegisterAdminHandlers() {
    // Handlers /**
 * RegisterHandler
 * 
 * Implementa a funcionalidade RegisterHandler conforme especificação original.
 * @param PKT_GM_COMMAND Parâmetro PKT_GM_COMMAND
 * @param clientID Parâmetro clientID
 * @param packet Parâmetro packet
 * @return Retorna administrativos
 */
 administrativos
    RegisterHandler(PKT_GM_COMMAND, 
        [this](SOCKET clientID, const Packet& packet){ 
            return this->HandleGMCommand(clientID, packet); 
        });
}

// Implementações dos handlers

bool CPacketHandler::HandleAccountLogin(SOCKET clientID, const Packet& packet) {
    // Verifica tamanho do pacote
    if (packet.GetSize() < HEADER_SIZE + sizeof(STRUCT_PACKET_ACCOUNTLOGIN)) {
        LOG_ERROR("Pacote de login com tamanho inválido: {}", packet.GetSize());
        return false;
    }
    
    // Extrai dados do pacote
    STRUCT_PACKET_ACCOUNTLOGIN loginData;
    memcpy(&loginData, packet.GetData(), sizeof(loginData));
    
    // Log de tentativa de login (sem a senha!)
    LOG_INFO("Tentativa de login: Conta '{}', Versão '{}'", 
        loginData.szAccountName, loginData.szClientVersion);
    
    // No WYD original, aqui verificaríamos os dados com o DBServer
    // Para esta implementação, vamos simular uma resposta de login
    
    // Constrói pacote de resposta
    Packet responsePacket(PKT_ACCOUNT_LOGIN_RESULT, packet.GetClientKey());
    STRUCT_PACKET_ACCOUNTLOGINRESULT loginResult;
    
    // No WYD original, a versão do cliente era verificada aqui
    // Este é um bug conhecido: não havia validação robusta da versão
    
    // Cria uma verificação mais robusta que corrige o bug do original
    if (strcmp(loginData.szClientVersion, "1.414") != 0) { // Exemplo de versão
        loginResult.SetResult(LOGIN_INVALID_VERSION);
        loginResult.SetMessage("Versão incompatível. Por favor, atualize seu cliente.");
        
        LOG_WARNING("Versão incompatível: '{}'", loginData.szClientVersion);
    }
    else {
        // Exemplo de resposta bem-sucedida
        // No sistema real, verificaríamos com o banco de dados
        loginResult.SetResult(LOGIN_SUCCESS);
        loginResult.SetMessage("Login realizado com sucesso.");
        
        LOG_INFO("Login bem-sucedido: Conta '{}'", loginData.szAccountName);
    }
    
    // Adiciona os dados de resultado ao pacote
    responsePacket.SetData(&loginResult, sizeof(loginResult));
    
    // Envia a resposta para o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param socketManager_ Parâmetro socketManager_
 * @return Retorna cliente
 */
 cliente
    if(socketManager_){
        if (!socketManager_->SendPacket(clientID, responsePacket)) {
            LOG_ERROR("Falha ao enviar resposta de login para o cliente {}", clientID);
            return false;
        }
    }
    else {
        LOG_ERROR("Socket manager não inicializado");
        return false;
    }
    
    return true;
}

bool CPacketHandler::HandleNumericPassword(SOCKET clientID, const Packet& packet) {
    // Implementação simplificada - no sistema real, validaríamos o /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param {}" Parâmetro {}"
 * @param clientID Parâmetro clientID
 * @return Retorna PIN
 */
 PIN
    LOG_INFO("Recebido pacote de PIN para o cliente {}", clientID);
    
    // Exemplo de resposta bem-sucedida
    Packet responsePacket(PKT_NUMERIC_PASSWORD, packet.GetClientKey());
    
    // O WYD original enviava uma resposta muito simples para o PIN
    BYTE result = 1; // 1 = Sucesso, 0 = Falha
    responsePacket.AppendData(&result, sizeof(BYTE));
    
    // Envia a resposta para o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param socketManager_ Parâmetro socketManager_
 * @return Retorna cliente
 */
 cliente
    if(socketManager_){
        if (!socketManager_->SendPacket(clientID, responsePacket)) {
            LOG_ERROR("Falha ao enviar resposta de PIN para o cliente {}", clientID);
            return false;
        }
    }
    
    return true;
}

bool CPacketHandler::HandleCharCreate(SOCKET clientID, const Packet& packet) {
    // Implementação simplificada - no sistema real, criaríamos o personagem no banco de /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param {}" Parâmetro {}"
 * @param clientID Parâmetro clientID
 * @return Retorna dados
 */
 dados
    LOG_INFO("Recebido pacote de criação de personagem para o cliente {}", clientID);
    
    // Para o WYD original, a resposta seria enviada pelo DBServer
    // Aqui simulamos uma resposta bem-sucedida
    
    // Envia uma resposta positiva
    Packet responsePacket(PKT_CHAR_CREATE, packet.GetClientKey());
    
    // No WYD original, o formato da resposta era simples:
    // 1 byte para o resultado (1 = sucesso, 0 = falha)
    BYTE result = 1; // Sucesso
    responsePacket.AppendData(&result, sizeof(BYTE));
    
    // Envia a resposta para o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param socketManager_ Parâmetro socketManager_
 * @return Retorna cliente
 */
 cliente
    if(socketManager_){
        if (!socketManager_->SendPacket(clientID, responsePacket)) {
            LOG_ERROR("Falha ao enviar resposta de criação de personagem para o cliente {}", clientID);
            return false;
        }
    }
    
    return true;
}

bool CPacketHandler::HandleCharDelete(SOCKET clientID, const Packet& packet) {
    // Implementação simplificada - no sistema real, excluiríamos o personagem do banco de /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param {}" Parâmetro {}"
 * @param clientID Parâmetro clientID
 * @return Retorna dados
 */
 dados
    LOG_INFO("Recebido pacote de exclusão de personagem para o cliente {}", clientID);
    
    // Para o WYD original, a resposta seria enviada pelo DBServer
    // Aqui simulamos uma resposta bem-sucedida
    
    // Envia uma resposta positiva
    Packet responsePacket(PKT_CHAR_DELETE, packet.GetClientKey());
    
    // No WYD original, o formato da resposta era simples:
    // 1 byte para o resultado (1 = sucesso, 0 = falha)
    BYTE result = 1; // Sucesso
    responsePacket.AppendData(&result, sizeof(BYTE));
    
    // Envia a resposta para o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param socketManager_ Parâmetro socketManager_
 * @return Retorna cliente
 */
 cliente
    if(socketManager_){
        if (!socketManager_->SendPacket(clientID, responsePacket)) {
            LOG_ERROR("Falha ao enviar resposta de exclusão de personagem para o cliente {}", clientID);
            return false;
        }
    }
    
    return true;
}

bool CPacketHandler::HandleCharSelect(SOCKET clientID, const Packet& packet) {
    // Implementação simplificada - no sistema real, carregaríamos o personagem e colocaríamos no /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param {}" Parâmetro {}"
 * @param clientID Parâmetro clientID
 * @return Retorna mundo
 */
 mundo
    LOG_INFO("Recebido pacote de seleção de personagem para o cliente {}", clientID);
    
    // Para o WYD original, este era um processo complexo que envolvia:
    // 1. Carregar os dados do personagem do banco
    // 2. Inicializar o jogador no mundo
    // 3. Enviar dados iniciais (inventário, status, etc.)
    // 4. Notificar jogadores próximos
    
    // Aqui, simplificamos com uma resposta básica
    Packet responsePacket(PKT_TELEPORT_REPLY, packet.GetClientKey());
    
    // O pacote de resposta teria dados de teleporte inicial
    STRUCT_PACKET_TELEPORT teleportData;
    teleportData.wMapID = 1;          // ID do mapa inicial
    teleportData.wPosX = 2000;        // Posição X inicial
    teleportData.wPosY = 2000;        // Posição Y inicial
    teleportData.dwIsValid = 1;       // Flag de validade
    
    responsePacket.AppendData(&teleportData, sizeof(teleportData));
    
    // Envia a resposta para o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param socketManager_ Parâmetro socketManager_
 * @return Retorna cliente
 */
 cliente
    if(socketManager_){
        if (!socketManager_->SendPacket(clientID, responsePacket)) {
            LOG_ERROR("Falha ao enviar resposta de seleção de personagem para o cliente {}", clientID);
            return false;
        }
    }
    
    return true;
}

bool CPacketHandler::HandleWalk(SOCKET clientID, const Packet& packet) {
    // Verifica tamanho do pacote
    if (packet.GetSize() < HEADER_SIZE + sizeof(STRUCT_PACKET_WALK)) {
        LOG_ERROR("Pacote de movimento com tamanho inválido: {}", packet.GetSize());
        return false;
    }
    
    // Extrai dados do pacote
    STRUCT_PACKET_WALK walkData;
    memcpy(&walkData, packet.GetData(), sizeof(walkData));
    
    // No WYD original, aqui atualizaríamos a posição do jogador e notificaríamos os vizinhos
    // Para esta implementação, apenas logamos a informação
    LOG_DEBUG("Movimento para o cliente {}: Posição ({}, {}), Direção {}, Tipo {}",
        clientID, walkData.wPosX, walkData.wPosY, walkData.byDirection, walkData.byType);
    
    // No sistema real, enviaríamos a atualização para jogadores próximos
    
    // Um bug conhecido no WYD original era a falta de validação robusta da posição
    // Não havia verificação de colisão nem de velocidade máxima
    // Aqui poderíamos implementar essas validações
    
    return true;
}

bool CPacketHandler::HandleAttack(SOCKET clientID, const Packet& packet) {
    // Verifica tamanho do pacote
    if (packet.GetSize() < HEADER_SIZE + sizeof(STRUCT_PACKET_ATTACK)) {
        LOG_ERROR("Pacote de ataque com tamanho inválido: {}", packet.GetSize());
        return false;
    }
    
    // Extrai dados do pacote
    STRUCT_PACKET_ATTACK attackData;
    memcpy(&attackData, packet.GetData(), sizeof(attackData));
    
    // No WYD original, aqui processaríamos o ataque e notificaríamos os afetados
    LOG_DEBUG("Ataque do cliente {}: Posição ({}, {}), Direção {}, Tipo {}, Alvo {}, Skill {}",
        clientID, attackData.wPosX, attackData.wPosY, attackData.byDirection, 
        attackData.byType, attackData.dwTargetID, attackData.wSkillID);
    
    // O WYD original tinha bugs no sistema de combate, como:
    // - Não validava adequadamente a distância de ataque
    // - Permitia ataques através de paredes em alguns casos
    // - Tinha problemas de sincronização em áreas congestionadas
    
    // Essas melhorias seriam implementadas no sistema real
    
    return true;
}

bool CPacketHandler::HandleChat(SOCKET clientID, const Packet& packet) {
    // Verifica tamanho do pacote
    if (packet.GetSize() < HEADER_SIZE + sizeof(STRUCT_PACKET_CHAT)) {
        LOG_ERROR("Pacote de chat com tamanho inválido: {}", packet.GetSize());
        return false;
    }
    
    // Extrai dados do pacote
    STRUCT_PACKET_CHAT chatData;
    memcpy(&chatData, packet.GetData(), sizeof(chatData));
    
    // Garante que a mensagem termina com null
    chatData.szMessage[sizeof(chatData.szMessage) - 1] = '\0';
    
    // No WYD original, aqui processaríamos a mensagem e a enviaríamos para jogadores pró/**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param '{}'" Parâmetro '{}'"
 * @param clientID Parâmetro clientID
 * @param chatData.szMessage Parâmetro chatData.szMessage
 * @return Retorna ximos
 */
ximos
    LOG_INFO("Mensagem de chat do cliente {}: '{}'", clientID, chatData.szMessage);
    
    // Um bug conhecido no WYD original era a falta de validação robusta de mensagens
    // Não havia filtros adequados para conteúdo impróprio
    // Não havia limitação de frequência de mensagens (permitindo spam)
    
    // Essas melhorias seriam implementadas no sistema real
    
    return true;
}

bool CPacketHandler::HandleWhisper(SOCKET clientID, const Packet& packet) {
    // No WYD original, o pacote de whisper tinha uma estrutura não documentada
    // Aqui implementamos uma estrutura básica
    
    // Estrutura simples para mensagem de whisper
    struct WhisperData {
        char targetName[32];  // Nome do alvo
        char message[256];    // Mensagem
    };
    
    // Verifica tamanho do pacote
    if (packet.GetSize() < HEADER_SIZE + sizeof(WhisperData)) {
        LOG_ERROR("Pacote de whisper com tamanho inválido: {}", packet.GetSize());
        return false;
    }
    
    // Extrai dados do pacote
    WhisperData whisperData;
    memcpy(&whisperData, packet.GetData(), sizeof(whisperData));
    
    // Garante que as strings terminam com null
    whisperData.targetName[sizeof(whisperData.targetName) - 1] = '\0';
    whisperData.message[sizeof(whisperData.message) - 1] = '\0';
    
    // No WYD original, aqui enviaríamos a mensagem para o jogador /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param '{}'" Parâmetro '{}'"
 * @param clientID Parâmetro clientID
 * @param whisperData.targetName Parâmetro whisperData.targetName
 * @param whisperData.message Parâmetro whisperData.message
 * @return Retorna alvo
 */
 alvo
    LOG_INFO("Whisper do cliente {} para '{}': '{}'", 
        clientID, whisperData.targetName, whisperData.message);
    
    return true;
}

bool CPacketHandler::HandleCastSpell(SOCKET clientID, const Packet& packet) {
    // Verifica tamanho do pacote
    if (packet.GetSize() < HEADER_SIZE + sizeof(STRUCT_PACKET_SKILLCAST)) {
        LOG_ERROR("Pacote de cast de skill com tamanho inválido: {}", packet.GetSize());
        return false;
    }
    
    // Extrai dados do pacote
    STRUCT_PACKET_SKILLCAST skillData;
    memcpy(&skillData, packet.GetData(), sizeof(skillData));
    
    // No WYD original, aqui processaríamos o cast da skill
    LOG_DEBUG("Cast de skill do cliente {}: Skill {}, Posição ({}, {}), Alvo {}, Direção {}",
        clientID, skillData.wSkillID, skillData.wPosX, skillData.wPosY, 
        skillData.dwTargetID, skillData.byDirection);
    
    // Um bug conhecido no WYD original era a validação insuficiente de skills:
    // - Não verificava adequadamente o cooldown
    // - Permitia casts de skills não aprendidas em alguns casos
    // - Tinha problemas com skills de movimento em terrenos específicos
    
    return true;
}

bool CPacketHandler::HandleTeleport(SOCKET clientID, const Packet& packet) {
    // Verifica tamanho do pacote
    if (packet.GetSize() < HEADER_SIZE + sizeof(STRUCT_PACKET_TELEPORT)) {
        LOG_ERROR("Pacote de teleporte com tamanho inválido: {}", packet.GetSize());
        return false;
    }
    
    // Extrai dados do pacote
    STRUCT_PACKET_TELEPORT teleportData;
    memcpy(&teleportData, packet.GetData(), sizeof(teleportData));
    
    // No WYD original, aqui processaríamos o teleporte
    LOG_DEBUG("Teleporte do cliente {}: Mapa {}, Posição ({}, {})",
        clientID, teleportData.wMapID, teleportData.wPosX, teleportData.wPosY);
    
    // Um bug conhecido no WYD original era a validação insuficiente de teleportes:
    // - Não verificava adequadamente se o destino era válido
    // - Tinha problemas com teleportes entre mapas específicos
    // - Não tratava corretamente casos de teleportes durante estados específicos (stun, etc)
    
    // Simulamos uma resposta de teleporte bem-sucedida
    Packet responsePacket(PKT_TELEPORT_REPLY, packet.GetClientKey());
    
    // Usamos os mesmos dados recebidos para a resposta
    responsePacket.AppendData(&teleportData, sizeof(teleportData));
    
    // Envia a resposta para o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param socketManager_ Parâmetro socketManager_
 * @return Retorna cliente
 */
 cliente
    if(socketManager_){
        if (!socketManager_->SendPacket(clientID, responsePacket)) {
            LOG_ERROR("Falha ao enviar resposta de teleporte para o cliente {}", clientID);
            return false;
        }
    }
    
    return true;
}

bool CPacketHandler::HandleKeepAlive(SOCKET clientID, const Packet& packet) {
    // No WYD original, este pacote era usado para manter a conexão ativa
    // e verificar se o cliente ainda estava conectado.
    
    // Log em nível baixo para não poluir os /**
 * LOG_TRACE
 * 
 * Implementa a funcionalidade LOG_TRACE conforme especificação original.
 * @param {}" Parâmetro {}"
 * @param clientID Parâmetro clientID
 * @return Retorna logs
 */
 logs
    LOG_TRACE("Keep-alive do cliente {}", clientID);
    
    // Responde com o mesmo pacote (echo)
    if (socketManager_) {
        if (!socketManager_->SendPacket(clientID, packet)) {
            LOG_ERROR("Falha ao enviar resposta de keep-alive para o cliente {}", clientID);
            return false;
        }
    }
    
    return true;
}

bool CPacketHandler::HandleGMCommand(SOCKET clientID, const Packet& packet) {
    // Implementação básica para comandos de /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param {}" Parâmetro {}"
 * @param clientID Parâmetro clientID
 * @return Retorna GM
 */
 GM
    LOG_INFO("Comando de GM recebido do cliente {}", clientID);
    
    // Extrai dados do pacote
    // No WYD original, o formato exato variava dependendo do comando específico
    // Aqui usamos um formato genérico para demonstração
    
    // Tamanho mínimo para um comando (assumindo pelo menos 1 caractere)
    if (packet.GetDataSize() < 1) {
        LOG_ERROR("Pacote de comando GM vazio do cliente {}", clientID);
        return false;
    }
    
    // Extrai a string do comando
    std::string commandStr(reinterpret_cast<const char*>(packet.GetData()), packet.GetDataSize());
    
    // Garante que a string termina com null
    if (commandStr.back() != '\0') {
        commandStr.push_back('\0');
    }
    
    LOG_INFO("Comando GM do cliente {}: '{}'", clientID, commandStr.c_str());
    
    // No sistema real, processaríamos o comando com base no nível de acesso do GM
    // e executaríamos a ação correspondente
    
    return true;
}

// Implementação de CPacketLogger

CPacketLogger::CPacketLogger(const std::string& logFilename, bool enableHexDump)
    : logFilename_(logFilename)
    , enableLogging_(true)
    , enableHexDump_(enableHexDump)
    , logFile_(nullptr)
{
}

CPacketLogger::~CPacketLogger() {
    Shutdown();
}

bool CPacketLogger::Initialize() {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    // Fecha o arquivo se já estiver /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param logFile_ Parâmetro logFile_
 * @return Retorna aberto
 */
 aberto
    if(logFile_){
        close(logFile_);
        logFile_ = nullptr;
    }
    
    // Abre o arquivo de log
    logFile_ = std::fstream(logFilename_.c_str(), "a");
    if (!logFile_) {
        LOG_ERROR("Falha ao abrir arquivo de log de pacotes: {}", logFilename_);
        return false;
    }
    
    // Escreve cabeçalho
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::string header = "=== Início de sessão de log: " + 
                        std::string(std::strftime(&time)) + 
                        "===\n";
    
    fprintf(logFile_, "%s", header.c_str());
    fflush(logFile_);
    
    LOG_INFO("Logger de pacotes inicializado: {}", logFilename_);
    
    return true;
}

void CPacketLogger::Shutdown() {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    if (logFile_) {
        // Escreve rodapé
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::string footer = "=== Fim de sessão de log: " + 
                            std::string(std::strftime(&time)) + 
                            "===\n";
        
        fprintf(logFile_, "%s", footer.c_str());
        fflush(logFile_);
        
        close(logFile_);
        logFile_ = nullptr;
    }
}

void CPacketLogger::LogReceivedPacket(SOCKET clientID, const Packet& packet) {
    if (!enableLogging_) {
        return;
    }
    
    std::string message = "RECV [" + std::to_string(clientID) + "] - " + 
                         "Cmd: 0x" + utils::ToHexString(packet.GetCommand()) + 
                         ", Size: " + std::to_string(packet.GetSize()) + 
                         ", Key: 0x" + utils::ToHexString(packet.GetClientKey());
    
    if (enableHexDump_ && packet.GetDataSize() > 0) {
        message += "\n" + GenerateHexDump(packet.GetData(), packet.GetDataSize());
    }
    
    WriteToLog(message);
}

void CPacketLogger::LogSentPacket(SOCKET clientID, const Packet& packet) {
    if (!enableLogging_) {
        return;
    }
    
    std::string message = "SEND [" + std::to_string(clientID) + "] - " + 
                         "Cmd: 0x" + utils::ToHexString(packet.GetCommand()) + 
                         ", Size: " + std::to_string(packet.GetSize()) + 
                         ", Key: 0x" + utils::ToHexString(packet.GetClientKey());
    
    if (enableHexDump_ && packet.GetDataSize() > 0) {
        message += "\n" + GenerateHexDump(packet.GetData(), packet.GetDataSize());
    }
    
    WriteToLog(message);
}

void CPacketLogger::LogPacketError(SOCKET clientID, const Packet& packet, const std::string& errorMessage) {
    if (!enableLogging_) {
        return;
    }
    
    std::string message = "ERROR [" + std::to_string(clientID) + "] - " + 
                         "Cmd: 0x" + utils::ToHexString(packet.GetCommand()) + 
                         ", Size: " + std::to_string(packet.GetSize()) + 
                         ", Key: 0x" + utils::ToHexString(packet.GetClientKey()) + 
                         "\nError: " + errorMessage;
    
    if (enableHexDump_ && packet.GetDataSize() > 0) {
        message += "\n" + GenerateHexDump(packet.GetData(), packet.GetDataSize());
    }
    
    WriteToLog(message);
}

void CPacketLogger::EnableLogging(bool enable) {
    enableLogging_ = enable;
}

void CPacketLogger::EnableHexDump(bool enable) {
    enableHexDump_ = enable;
}

std::string CPacketLogger::GenerateHexDump(const BYTE* buffer, WORD size) const {
    if (!buffer || size == 0) {
        return "";
    }
    
    std::stringstream ss;
    
    // Cabeçalho
    ss << "Offset  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII\n";
    ss << "------  -----------------------------------------------  ----------------\n";
    
    // Número de linhas completas
    WORD fullLines = size / 16;
    
    // Para cada linha /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param line++ Parâmetro line++
 * @return Retorna completa
 */
 completa
    for(WORD line = 0; line < fullLines; line++){
        // Offset
        ss << std::setfill('0') << std::setw(6) << std::hex << (line * 16) << "  ";
        
        // Bytes em /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna hex
 */
 hex
        for(WORD i = 0; i < 16; i++){
            BYTE b = buffer[line * 16 + i];
            ss << std::setfill('0') << std::setw(2) << std::hex << (int)b << " ";
        }
        
        // ASCII
        ss << " ";
        for (WORD i = 0; i < 16; i++) {
            BYTE b = buffer[line * 16 + i];
            if (b >= 32 && b <= 126) {
                ss << (char)b;
            }
            else {
                ss << ".";
            }
        }
        
        ss << "\n";
    }
    
    // Bytes restantes
    WORD remaining = size % 16;
    if (remaining > 0) {
        // Offset
        ss << std::setfill('0') << std::setw(6) << std::hex << (fullLines * 16) << "  ";
        
        // Bytes em /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna hex
 */
 hex
        for(WORD i = 0; i < remaining; i++){
            BYTE b = buffer[fullLines * 16 + i];
            ss << std::setfill('0') << std::setw(2) << std::hex << (int)b << " ";
        }
        
        // /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna Padding
 */
 Padding
        for(WORD i = remaining; i < 16; i++){
            ss << "   ";
        }
        
        // ASCII
        ss << " ";
        for (WORD i = 0; i < remaining; i++) {
            BYTE b = buffer[fullLines * 16 + i];
            if (b >= 32 && b <= 126) {
                ss << (char)b;
            }
            else {
                ss << ".";
            }
        }
    }
    
    return ss.str();
}

void CPacketLogger::WriteToLog(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    if (!logFile_) {
        return;
    }
    
    // Obtém timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::tm* tm = std::localtime_r(&time);
    
    // Formata timestamp
    char timeBuffer[64];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm);
    
    // Escreve no arquivo
    fprintf(logFile_, "[%s] %s\n", timeBuffer, message.c_str());
    fflush(logFile_);
}

} // namespace network
} // namespace wyd