/**
 * @file packet_struct.h
 * @brief Definição das estruturas de pacotes de rede do WYD
 * 
 * Este arquivo contém as declarações completas para pacotes de rede
 * no sistema WYDBR, com correções de segurança e melhorias.
 */

#ifndef WYDBR_PACKET_STRUCT_H
#define WYDBR_PACKET_STRUCT_H

#include <cstdint>
#include <cstring>
#include "wyd_core.h"

namespace WYDBR {

// Definição de opcodes (correção de segurança - antes vulnerável a injeção de pacotes)
enum class PacketType : uint16_t {
    // Cliente para Servidor
    C_LOGIN = 0x100,
    C_MOVE = 0x101,
    C_CHAT = 0x102,
    C_ATTACK = 0x103,
    C_USE_ITEM = 0x104,
    C_DROP_ITEM = 0x105,
    C_TRADE_REQUEST = 0x106,
    C_TRADE_CONFIRM = 0x107,
    C_TRADE_CANCEL = 0x108,
    C_PARTY_REQUEST = 0x109,
    C_PARTY_CONFIRM = 0x10A,
    C_PARTY_CANCEL = 0x10B,
    C_WARP = 0x10C,
    C_SKILL_USE = 0x10D,
    C_PICKUP_ITEM = 0x10E,
    C_SHOP_BUY = 0x10F,
    C_SHOP_SELL = 0x110,
    C_ADMIN_COMMAND = 0x111,
    
    // Servidor para Cliente
    S_LOGIN_RESULT = 0x200,
    S_ENTER_GAME = 0x201,
    S_LEAVE_GAME = 0x202,
    S_CHAT = 0x203,
    S_MOVE_ENTITY = 0x204,
    S_CREATE_ENTITY = 0x205,
    S_REMOVE_ENTITY = 0x206,
    S_DAMAGE = 0x207,
    S_ITEM_DROP = 0x208,
    S_ITEM_PICKUP = 0x209,
    S_STAT_CHANGE = 0x20A,
    S_TRADE_START = 0x20B,
    S_TRADE_ITEM = 0x20C,
    S_TRADE_CANCEL = 0x20D,
    S_TRADE_COMPLETE = 0x20E,
    S_PARTY_INVITE = 0x20F,
    S_PARTY_JOIN = 0x210,
    S_PARTY_LEAVE = 0x211,
    S_SKILL_CAST = 0x212,
    S_SKILL_EFFECT = 0x213,
    S_WARP = 0x214,
    S_SYSTEM_MESSAGE = 0x215,
    S_HP_CHANGE = 0x216,
    S_MP_CHANGE = 0x217,
    S_EXP_CHANGE = 0x218,
    S_LEVEL_UP = 0x219,
    S_SHOP_OPEN = 0x21A,
    S_SHOP_CLOSE = 0x21B
};

// Cabeçalho de pacote seguro
struct PacketHeader {
    uint16_t size = 0;         // Tamanho total do pacote
    uint16_t opcode = 0;       // Código de operação
    uint32_t timestamp = 0;    // Timestamp para prevenir replay attacks
    uint32_t checksum = 0;     // Checksum para verificar integridade
    
    PacketHeader() = default;
    
    PacketHeader(uint16_t _size, PacketType _opcode, uint32_t _timestamp = 0) {
        size = _size;
        opcode = static_cast<uint16_t>(_opcode);
        timestamp = _timestamp;
        checksum = 0; // Será calculado depois
    }
    
    // Método para calcular o checksum
    void CalculateChecksum(const void* packetData, uint16_t dataSize) {
        // Salvar o checksum atual
        uint32_t savedChecksum = checksum;
        
        // Zerar o checksum para o cálculo
        checksum = 0;
        
        // Calcular o checksum do cabeçalho
        uint32_t headerChecksum = Utils::CalculateChecksum(this, sizeof(PacketHeader));
        
        // Calcular o checksum dos dados
        uint32_t dataChecksum = Utils::CalculateChecksum(packetData, dataSize);
        
        // Combinar os checksums
        checksum = headerChecksum ^ dataChecksum;
        
        // Aplicar uma transformação adicional para maior segurança
        checksum = (checksum << 16) | (checksum >> 16);
        checksum ^= 0x5A3C2E1D; // Valor mágico para dificultar falsificação
    }
    
    // Método para verificar o checksum
    bool VerifyChecksum(const void* packetData, uint16_t dataSize) const {
        // Criar uma cópia temporária do cabeçalho
        PacketHeader tempHeader = *this;
        
        // Salvar o checksum original
        uint32_t originalChecksum = checksum;
        
        // Calcular o checksum da cópia
        tempHeader.CalculateChecksum(packetData, dataSize);
        
        // Comparar com o checksum original
        return tempHeader.checksum == originalChecksum;
    }
};

// Tipos básicos de respostas do servidor
enum class ResultCode : uint8_t {
    SUCCESS = 0,
    FAILED = 1,
    INVALID_ACCOUNT = 2,
    WRONG_PASSWORD = 3,
    ACCOUNT_BANNED = 4,
    ALREADY_LOGGED = 5,
    SERVER_FULL = 6,
    NO_PERMISSION = 7,
    INVALID_LEVEL = 8,
    INVALID_CHARACTER = 9,
    INVALID_POSITION = 10,
    INVALID_ITEM = 11,
    NOT_ENOUGH_MONEY = 12,
    INVENTORY_FULL = 13,
    TRADE_ALREADY_ACTIVE = 14,
    PARTY_ALREADY_ACTIVE = 15,
    TARGET_NOT_FOUND = 16,
    SKILL_NOT_LEARNED = 17,
    SKILL_IN_COOLDOWN = 18,
    NOT_ENOUGH_MP = 19,
    WARP_NOT_ALLOWED = 20,
    UNKNOWN_ERROR = 255
};

// Pacote de login
struct LoginPacket {
    PacketHeader header;
    char username[32] = {0};
    char password[32] = {0};
    uint8_t clientVersion[4] = {0}; // versão do cliente
    
    LoginPacket() {
        header = PacketHeader(sizeof(LoginPacket), PacketType::C_LOGIN);
    }
    
    // Método para inicializar com dados seguros
    void Initialize(const char* user, const char* pass) {
        header = PacketHeader(sizeof(LoginPacket), PacketType::C_LOGIN);
        
        // Copiar com segurança (prevenir buffer overflow)
        strncpy(username, user, sizeof(username) - 1);
        strncpy(password, pass, sizeof(password) - 1);
        
        // Garantir terminador nulo
        username[sizeof(username) - 1] = '\0';
        password[sizeof(password) - 1] = '\0';
    }
};

// Resposta de login
struct LoginResultPacket {
    PacketHeader header;
    ResultCode result = ResultCode::FAILED;
    uint32_t accountId = 0;
    uint8_t adminLevel = 0;
    uint32_t banExpiryTime = 0;
    char characters[4][16] = {0}; // Nomes dos personagens disponíveis
    uint8_t characterLevels[4] = {0}; // Níveis dos personagens
    uint8_t characterClasses[4] = {0}; // Classes dos personagens
    
    LoginResultPacket() {
        header = PacketHeader(sizeof(LoginResultPacket), PacketType::S_LOGIN_RESULT);
    }
    
    // Método para inicializar com dados seguros
    void Initialize(ResultCode resultCode, uint32_t accId = 0) {
        header = PacketHeader(sizeof(LoginResultPacket), PacketType::S_LOGIN_RESULT);
        result = resultCode;
        accountId = accId;
        
        // Calcular checksum
        header.CalculateChecksum(this + sizeof(PacketHeader), 
                                sizeof(LoginResultPacket) - sizeof(PacketHeader));
    }
};

// Pacote de movimento
struct MovePacket {
    PacketHeader header;
    uint32_t entityId = 0;
    Position currentPos;
    Position targetPos;
    uint8_t moveSpeed = 0;
    uint8_t runMode = 0;
    
    MovePacket() {
        header = PacketHeader(sizeof(MovePacket), PacketType::C_MOVE);
    }
    
    // Método para inicializar com dados seguros
    void Initialize(uint32_t id, const Position& current, const Position& target, 
                   uint8_t speed, bool running) {
        header = PacketHeader(sizeof(MovePacket), PacketType::C_MOVE);
        entityId = id;
        currentPos = current;
        targetPos = target;
        moveSpeed = speed;
        runMode = running ? 1 : 0;
        
        // Calcular checksum
        header.CalculateChecksum(this + sizeof(PacketHeader), 
                                sizeof(MovePacket) - sizeof(PacketHeader));
    }
    
    // Verificação de segurança
    bool IsValidMove() const {
        // Limite de movimento por tick
        const int MAX_MOVE_DISTANCE = 20;
        
        // Verificar distância
        return currentPos.Distance(targetPos) <= MAX_MOVE_DISTANCE;
    }
};

// Pacote de chat
struct ChatPacket {
    PacketHeader header;
    uint8_t chatType = 0; // 0: normal, 1: whisper, 2: guild, 3: party, 4: global
    char targetName[16] = {0}; // Para whisper
    char message[128] = {0};
    
    ChatPacket() {
        header = PacketHeader(sizeof(ChatPacket), PacketType::C_CHAT);
    }
    
    // Método para inicializar com dados seguros
    void Initialize(uint8_t type, const char* target, const char* msg) {
        header = PacketHeader(sizeof(ChatPacket), PacketType::C_CHAT);
        chatType = type;
        
        // Copiar com segurança (prevenir buffer overflow)
        if (target) {
            strncpy(targetName, target, sizeof(targetName) - 1);
            targetName[sizeof(targetName) - 1] = '\0';
        }
        
        if (msg) {
            strncpy(message, msg, sizeof(message) - 1);
            message[sizeof(message) - 1] = '\0';
        }
        
        // Calcular checksum
        header.CalculateChecksum(this + sizeof(PacketHeader), 
                                sizeof(ChatPacket) - sizeof(PacketHeader));
    }
    
    // Filtrar mensagem para segurança
    void FilterMessage() {
        // Lista de palavras proibidas
        const char* badWords[] = {"hack", "cheat", "exploit", "admin", "gamemaster"};
        const int numBadWords = sizeof(badWords) / sizeof(badWords[0]);
        
        // Converter para minúsculas para comparação
        char lowerMsg[sizeof(message)];
        for (size_t i = 0; i < sizeof(message); ++i) {
            lowerMsg[i] = tolower(message[i]);
        }
        
        // Verificar cada palavra proibida
        for (int i = 0; i < numBadWords; ++i) {
            const char* badWord = badWords[i];
            size_t badWordLen = strlen(badWord);
            
            // Buscar a palavra na mensagem
            char* found = strstr(lowerMsg, badWord);
            while (found) {
                // Substituir por asteriscos
                for (size_t j = 0; j < badWordLen; ++j) {
                    size_t index = found - lowerMsg + j;
                    if (index < sizeof(message)) {
                        message[index] = '*';
                    }
                }
                
                // Procurar próxima ocorrência
                found = strstr(found + badWordLen, badWord);
            }
        }
    }
};

} // namespace WYDBR

#endif // WYDBR_PACKET_STRUCT_H