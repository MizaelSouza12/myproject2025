/**
 * protocol.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/network/protocol.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYDBR_NETWORK_PROTOCOL_H
#define WYDBR_NETWORK_PROTOCOL_H

#include <stdint.h>
#include "../Basedef.h"

namespace wydbr {
namespace network {
namespace protocol {

// Valores de tamanho constantes
constexpr int MAX_PACKET_SIZE = 4096;           // Tamanho máximo de pacote
constexpr int MIN_PACKET_SIZE = 12;             // Tamanho mínimo de pacote
constexpr int MAX_CHAT_LENGTH = 128;            // Tamanho máximo de mensagem de chat
constexpr int MAX_PASSWORD_LENGTH = 36;         // Tamanho máximo de senha
constexpr int MAX_ITEM_TRANSACTION = 12;        // Máximo de itens em uma transação
constexpr int MAX_PARTY_MEMBERS = 12;           // Máximo de membros em um grupo
constexpr int MAX_SKILL_DATA_LENGTH = 256;      // Tamanho máximo de dados de habilidade
constexpr int MAX_TRADE_ITEMS = 15;             // Máximo de itens em uma troca
constexpr int MAX_DROP_ITEMS = 20;              // Máximo de itens em um drop

// Versões do protocolo
constexpr uint16_t PROTOCOL_VERSION = 603;      // Versão atual do protocolo
constexpr uint16_t PROTOCOL_VERSION_MIN = 602;  // Versão mínima suportada

// Prefixos de pacotes para identificação
constexpr uint16_t PACKET_PREFIX_CLIENT = 0xC100;  // Pacotes do cliente
constexpr uint16_t PACKET_PREFIX_SERVER = 0xS100;  // Pacotes do servidor

// Cabeçalho padrão de pacote
#pragma pack(push, 1)
struct PacketHeader {
    uint16_t size;         // Tamanho total do pacote
    uint16_t type;         // Tipo do pacote
    uint16_t id;           // ID da conexão / sessão
    uint16_t clientTick;   // Tick do cliente para sincronização
};
#pragma pack(pop)

// Tipos de pacotes - Cliente para Servidor
enum ClientPacketType : uint16_t {
    // Gerais
    CPT_HEARTBEAT = 0xF,                  // Heartbeat para manter conexão
    CPT_TIMESTAMP = 0x111,                // Timestamp para sincronização
    CPT_VERSION_CHECK = 0x100,            // Verificação de versão
    
    // Login e autenticação
    CPT_LOGIN_REQUEST = 0x20C,            // Solicitação de login
    CPT_CREATE_CHARACTER = 0x20D,         // Criar personagem
    CPT_DELETE_CHARACTER = 0x20E,         // Excluir personagem
    CPT_CHARACTER_SELECT = 0x20F,         // Selecionar personagem
    CPT_LOGOUT = 0x215,                   // Logout do jogo
    
    // Movimentação
    CPT_MOVEMENT = 0x27A,                 // Movimentação do personagem
    CPT_JUMP = 0x27B,                     // Pulo do personagem
    CPT_TELEPORT_REQUEST = 0x2F7,         // Solicitação de teleporte
    
    // Ações
    CPT_ATTACK = 0x368,                   // Ataque
    CPT_SKILL_USE = 0x36A,                // Uso de habilidade
    CPT_ITEM_USE = 0x373,                 // Uso de item
    CPT_ITEM_DROP = 0x374,                // Largar item no chão
    CPT_ITEM_GET = 0x375,                 // Pegar item do chão
    CPT_ITEM_MOVE = 0x376,                // Mover item no inventário
    CPT_ITEM_SPLIT = 0x37A,               // Dividir item em pilha
    
    // Comércio
    CPT_TRADE_REQUEST = 0x37B,            // Solicitação de troca
    CPT_TRADE_ACCEPT = 0x37C,             // Aceitar troca
    CPT_TRADE_CANCEL = 0x37D,             // Cancelar troca
    CPT_TRADE_ADD_ITEM = 0x37E,           // Adicionar item à troca
    CPT_TRADE_CONFIRM = 0x37F,            // Confirmar troca
    CPT_TRADE_COMPLETE = 0x380,           // Finalizar troca
    
    // Shop e NPC
    CPT_SHOP_OPEN = 0x334,                // Abrir loja
    CPT_SHOP_BUY = 0x335,                 // Comprar item na loja
    CPT_SHOP_SELL = 0x336,                // Vender item na loja
    CPT_NPC_TALK = 0x230,                 // Falar com NPC
    CPT_NPC_RESPONSE = 0x231,             // Resposta ao NPC
    
    // Chat e social
    CPT_CHAT_NORMAL = 0x31,               // Chat normal
    CPT_CHAT_WHISPER = 0x33,              // Chat sussurro
    CPT_CHAT_PARTY = 0x34,                // Chat de grupo
    CPT_CHAT_GUILD = 0x35,                // Chat de guild
    CPT_CHAT_SHOUT = 0x36,                // Chat global
    CPT_EMOTE = 0x37,                     // Emote / animação
    
    // Grupo e Guild
    CPT_PARTY_REQUEST = 0x387,            // Solicitação de grupo
    CPT_PARTY_RESPONSE = 0x388,           // Resposta à solicitação de grupo
    CPT_PARTY_LEAVE = 0x389,              // Sair do grupo
    CPT_GUILD_CREATE = 0x3C0,             // Criar guild
    CPT_GUILD_INVITE = 0x3C1,             // Convidar para guild
    CPT_GUILD_RESPONSE = 0x3C2,           // Resposta a convite de guild
    CPT_GUILD_LEAVE = 0x3C3,              // Sair da guild
    CPT_GUILD_KICK = 0x3C4,               // Expulsar da guild
    
    // Habilidades e atributos
    CPT_SKILL_LEARN = 0x360,              // Aprender habilidade
    CPT_SKILL_UPGRADE = 0x361,            // Melhorar habilidade
    CPT_ATTRIBUTE_UP = 0x362,             // Aumentar atributo
    
    // Armazenamento
    CPT_STORAGE_OPEN = 0x390,             // Abrir armazenamento
    CPT_STORAGE_ITEM_PUT = 0x391,         // Colocar item no armazenamento
    CPT_STORAGE_ITEM_GET = 0x392,         // Pegar item do armazenamento
    
    // Sistemas avançados
    CPT_REFINE_REQUEST = 0x3A0,           // Solicitação de refinamento
    CPT_COMBINE_ITEMS = 0x3A1,            // Combinar itens
    CPT_QUEST_ACCEPT = 0x3B0,             // Aceitar missão
    CPT_QUEST_COMPLETE = 0x3B1,           // Completar missão
    CPT_QUEST_ABANDON = 0x3B2,            // Abandonar missão
    
    // Sistema de configurações
    CPT_CONFIG_UPDATE = 0x400,            // Atualização de configurações
    CPT_MACRO_UPDATE = 0x401,             // Atualização de macros
    CPT_SHORTCUT_UPDATE = 0x402,          // Atualização de atalhos
};

// Tipos de pacotes - Servidor para Cliente
enum ServerPacketType : uint16_t {
    // Gerais
    SPT_HEARTBEAT_ACK = 0xF,              // Confirmação de heartbeat
    SPT_TIMESTAMP_ACK = 0x112,            // Resposta de timestamp
    SPT_VERSION_CHECK_RESULT = 0x101,     // Resultado de verificação de versão
    
    // Login e autenticação
    SPT_LOGIN_RESPONSE = 0x210,           // Resposta ao login
    SPT_CHARACTER_LIST = 0x211,           // Lista de personagens
    SPT_CHARACTER_CREATE_RESULT = 0x212,  // Resultado da criação de personagem
    SPT_CHARACTER_DELETE_RESULT = 0x213,  // Resultado da exclusão de personagem
    SPT_ENTER_WORLD = 0x214,              // Entrar no mundo
    SPT_LOGOUT_ACK = 0x216,               // Confirmação de logout
    
    // Mundo e ambiente
    SPT_WEATHER_CHANGE = 0x50,            // Mudança de clima
    SPT_TIME_UPDATE = 0x51,               // Atualização de hora/dia
    SPT_ZONE_CHANGE = 0x52,               // Mudança de zona
    
    // Movimentação e entidades
    SPT_ENTITY_SPAWN = 0x280,             // Spawn de entidade
    SPT_ENTITY_DESPAWN = 0x281,           // Despawn de entidade
    SPT_ENTITY_MOVE = 0x282,              // Movimento de entidade
    SPT_ENTITY_TELEPORT = 0x283,          // Teleporte de entidade
    SPT_TELEPORT_RESPONSE = 0x2F8,        // Resposta à solicitação de teleporte
    
    // Ações
    SPT_ATTACK_RESULT = 0x369,            // Resultado de ataque
    SPT_SKILL_CAST_START = 0x36B,         // Início de cast de habilidade
    SPT_SKILL_CAST_RESULT = 0x36C,        // Resultado de cast de habilidade
    SPT_SKILL_EFFECT = 0x36D,             // Efeito visual de habilidade
    SPT_ITEM_USE_RESULT = 0x377,          // Resultado de uso de item
    SPT_ITEM_DROP_RESULT = 0x378,         // Resultado de largada de item
    SPT_ITEM_GET_RESULT = 0x379,          // Resultado de coleta de item
    SPT_ITEM_UPDATE = 0x37A,              // Atualização de item no inventário
    
    // Comércio
    SPT_TRADE_REQUEST_RESPONSE = 0x381,   // Resposta à solicitação de troca
    SPT_TRADE_STATUS = 0x382,             // Status da troca
    SPT_TRADE_ITEM_ADD = 0x383,           // Item adicionado à troca
    SPT_TRADE_CONFIRM = 0x384,            // Confirmação da troca
    SPT_TRADE_RESULT = 0x385,             // Resultado da troca
    
    // Shop e NPC
    SPT_SHOP_OPEN_RESPONSE = 0x337,       // Resposta à abertura de loja
    SPT_SHOP_ITEM_LIST = 0x338,           // Lista de itens da loja
    SPT_SHOP_BUY_RESULT = 0x339,          // Resultado da compra
    SPT_SHOP_SELL_RESULT = 0x33A,         // Resultado da venda
    SPT_NPC_TALK_RESPONSE = 0x232,        // Resposta ao diálogo com NPC
    SPT_NPC_OPTIONS = 0x233,              // Opções de diálogo com NPC
    
    // Chat e social
    SPT_CHAT_MESSAGE = 0x38,              // Mensagem de chat
    SPT_EMOTE_RESPONSE = 0x39,            // Resposta a emote
    SPT_ANNOUNCEMENT = 0x3A,              // Anúncio global
    SPT_SYSTEM_MESSAGE = 0x3B,            // Mensagem do sistema
    
    // Status e atributos
    SPT_STATUS_UPDATE = 0x300,            // Atualização de status
    SPT_STAT_UPDATE = 0x301,              // Atualização de atributos
    SPT_EXP_UPDATE = 0x302,               // Atualização de experiência
    SPT_LEVEL_UP = 0x303,                 // Subida de nível
    SPT_HP_MP_UPDATE = 0x304,             // Atualização de HP/MP
    SPT_SKILL_LEARN_RESULT = 0x363,       // Resultado de aprendizado de habilidade
    SPT_SKILL_UPDATE = 0x364,             // Atualização de habilidade
    SPT_SKILL_COOLDOWN = 0x365,           // Cooldown de habilidade
    
    // Grupo e Guild
    SPT_PARTY_UPDATE = 0x38A,             // Atualização de grupo
    SPT_PARTY_INVITE = 0x38B,             // Convite para grupo
    SPT_PARTY_LEAVE_RESULT = 0x38C,       // Resultado de saída do grupo
    SPT_GUILD_UPDATE = 0x3C5,             // Atualização de guild
    SPT_GUILD_INVITE_RESPONSE = 0x3C6,    // Resposta a convite de guild
    SPT_GUILD_MEMBER_UPDATE = 0x3C7,      // Atualização de membro da guild
    
    // Armazenamento
    SPT_STORAGE_OPEN_RESULT = 0x393,      // Resultado de abertura de armazenamento
    SPT_STORAGE_ITEM_LIST = 0x394,        // Lista de itens do armazenamento
    SPT_STORAGE_RESULT = 0x395,           // Resultado de operação no armazenamento
    
    // Sistemas avançados
    SPT_REFINE_RESULT = 0x3A2,            // Resultado de refinamento
    SPT_COMBINE_RESULT = 0x3A3,           // Resultado de combinação
    SPT_QUEST_LIST = 0x3B3,               // Lista de missões
    SPT_QUEST_UPDATE = 0x3B4,             // Atualização de missão
    SPT_QUEST_RESULT = 0x3B5,             // Resultado de missão
    
    // Eventos e notificações
    SPT_EVENT_NOTIFICATION = 0x500,       // Notificação de evento
    SPT_MAIL_NOTIFICATION = 0x501,        // Notificação de correio
    SPT_FRIEND_STATUS = 0x502,            // Status de amigo
    SPT_GUILD_WAR_NOTIFICATION = 0x503,   // Notificação de guerra de guild
    
    // Erros e falhas
    SPT_ERROR_NOTIFICATION = 0xFFE,       // Notificação de erro
    SPT_DISCONNECT_REASON = 0xFFF,        // Motivo de desconexão
};

// Códigos de resultado
enum ResultCode : uint8_t {
    RESULT_SUCCESS = 0,                     // Sucesso
    RESULT_FAIL = 1,                        // Falha geral
    RESULT_INVALID_PARAMETERS = 2,          // Parâmetros inválidos
    RESULT_NOT_ENOUGH_GOLD = 3,             // Ouro insuficiente
    RESULT_INVENTORY_FULL = 4,              // Inventário cheio
    RESULT_ITEM_NOT_FOUND = 5,              // Item não encontrado
    RESULT_LEVEL_TOO_LOW = 6,               // Nível muito baixo
    RESULT_STAT_TOO_LOW = 7,                // Atributo muito baixo
    RESULT_INCORRECT_POSITION = 8,          // Posição incorreta
    RESULT_TARGET_NOT_FOUND = 9,            // Alvo não encontrado
    RESULT_OUT_OF_RANGE = 10,               // Fora de alcance
    RESULT_COOLDOWN = 11,                   // Em cooldown
    RESULT_NOT_ENOUGH_MP = 12,              // MP insuficiente
    RESULT_INVALID_STATE = 13,              // Estado inválido
    RESULT_CANNOT_USE_NOW = 14,             // Não pode usar agora
    RESULT_TRADE_CANCEL = 15,               // Troca cancelada
    RESULT_GUILD_EXISTS = 16,               // Guild já existe
    RESULT_ALREADY_IN_GUILD = 17,           // Já está em uma guild
    RESULT_NEED_PERMISSION = 18,            // Precisa de permissão
    RESULT_TARGET_BUSY = 19,                // Alvo ocupado
    RESULT_ALREADY_TRADING = 20,            // Já está em uma troca
    RESULT_PARTY_FULL = 21,                 // Grupo cheio
    RESULT_ALREADY_IN_PARTY = 22,           // Já está em um grupo
    RESULT_REFINE_FAILED = 23,              // Refinamento falhou
    RESULT_ITEM_BREAK = 24,                 // Item quebrou
    RESULT_INCORRECT_PASSWORD = 25,         // Senha incorreta
    RESULT_ACCOUNT_BANNED = 26,             // Conta banida
    RESULT_SERVER_BUSY = 27,                // Servidor ocupado
    RESULT_VERSION_MISMATCH = 28,           // Versão incompatível
    RESULT_DATABASE_ERROR = 29,             // Erro de banco de dados
    RESULT_TOO_MANY_FAILURES = 30,          // Muitas falhas
};

// Estruturas de pacotes específicos
#pragma pack(push, 1)

// Pacote de login
struct PKT_Login {
    PacketHeader header;
    char username[16];
    char password[MAX_PASSWORD_LENGTH];
    uint32_t clientVersion;
};

// Pacote de resposta de login
struct PKT_LoginResponse {
    PacketHeader header;
    uint8_t result;
    uint32_t accountId;
    uint8_t accessLevel;
    uint8_t numChars;
    uint32_t lastLoginTime;
    uint32_t premiumExpireTime;
};

// Pacote de movimento
struct PKT_Movement {
    PacketHeader header;
    uint16_t posX;
    uint16_t posY;
    uint16_t destX;
    uint16_t destY;
    uint8_t moveType;
    uint8_t direction;
    uint16_t timestamp;
};

// Pacote de chat
struct PKT_Chat {
    PacketHeader header;
    uint8_t chatType;
    char sender[16];
    char recipient[16];
    uint16_t messageLength;
    char message[MAX_CHAT_LENGTH];
};

// Pacote de ataque
struct PKT_Attack {
    PacketHeader header;
    uint16_t attackerId;
    uint16_t targetId;
    uint8_t attackType;
    uint8_t skillId;
    uint16_t posX;
    uint16_t posY;
};

// Pacote de resultado de ataque
struct PKT_AttackResult {
    PacketHeader header;
    uint16_t attackerId;
    uint16_t targetId;
    uint16_t damage;
    uint8_t hitType;  // 0=miss, 1=normal, 2=crit, 3=block
    uint16_t remainingHP;
    uint16_t remainingMP;
    uint8_t effect;
};

// Pacote de uso de habilidade
struct PKT_SkillUse {
    PacketHeader header;
    uint8_t skillId;
    uint8_t skillLevel;
    uint16_t targetId;
    uint16_t posX;
    uint16_t posY;
};

// Pacote de resultado de uso de habilidade
struct PKT_SkillResult {
    PacketHeader header;
    uint8_t skillId;
    uint8_t result;
    uint16_t casterId;
    uint16_t numTargets;
    struct {
        uint16_t targetId;
        uint16_t damage;
        uint8_t hitType;
        uint16_t remainingHP;
        uint16_t remainingMP;
        uint8_t effect;
    } targets[8];  // Máximo de 8 alvos
};

// Pacote de item (formato genérico)
struct PKT_Item {
    PacketHeader header;
    uint8_t action;  // 0=add, 1=remove, 2=update
    uint8_t slot;
    STRUCT_ITEM item;
    uint8_t result;
};

// Pacote de troca
struct PKT_Trade {
    PacketHeader header;
    uint8_t action;  // 0=request, 1=accept, 2=cancel, 3=confirm, 4=complete
    uint16_t traderId;
    uint16_t partnerId;
    uint32_t gold;
    uint8_t itemCount;
    struct {
        uint8_t slot;
        STRUCT_ITEM item;
    } items[MAX_TRADE_ITEMS];
};

// Pacote de atualização de status
struct PKT_StatusUpdate {
    PacketHeader header;
    uint16_t entityId;
    uint16_t hp;
    uint16_t maxHp;
    uint16_t mp;
    uint16_t maxMp;
    uint32_t exp;
    uint32_t maxExp;
    uint8_t level;
    uint8_t str;
    uint8_t dex;
    uint8_t int_;
    uint8_t con;
    uint16_t statPoints;
    uint16_t skillPoints;
    uint8_t pkStatus;
    uint32_t guildId;
};

// Pacote de spawn de entidade
struct PKT_EntitySpawn {
    PacketHeader header;
    uint16_t entityId;
    uint8_t entityType;  // 0=player, 1=npc, 2=monster, 3=item
    uint16_t posX;
    uint16_t posY;
    uint8_t direction;
    uint8_t moveSpeed;
    char name[16];
    uint8_t appearance;
    STRUCT_ITEM equipment[MAX_EQUIP];
    uint8_t level;
    uint8_t guild[12];
    uint16_t guildId;
    uint8_t pkStatus;
    uint8_t effects;
};

// Pacote de atualização de grupo
struct PKT_PartyUpdate {
    PacketHeader header;
    uint8_t action;  // 0=create, 1=join, 2=leave, 3=update
    uint16_t leaderId;
    uint8_t memberCount;
    struct {
        uint16_t memberId;
        char name[16];
        uint8_t level;
        uint8_t classType;
        uint16_t hp;
        uint16_t maxHp;
        uint16_t mp;
        uint16_t maxMp;
        uint16_t posX;
        uint16_t posY;
    } members[MAX_PARTY_MEMBERS];
};

// Pacote de notificação de erro
struct PKT_Error {
    PacketHeader header;
    uint8_t errorCode;
    uint16_t messageLength;
    char message[256];
};

// Pacote de heartbeat
struct PKT_Heartbeat {
    PacketHeader header;
    uint32_t timestamp;
};

#pragma pack(pop)

// Funções utilitárias
/**
 * @brief Valida o tamanho de um pacote com base no tipo
 * @param type Tipo do pacote
 * @param size Tamanho do pacote
 * @return true se o tamanho for válido para o tipo
 */
bool validatePacketSize(uint16_t type, uint16_t size);

/**
 * @brief Converte um tipo de pacote para string
 * @param type Tipo do pacote
 * @return String representando o tipo do pacote
 */
const char* packetTypeToString(uint16_t type);

/**
 * @brief Cria um cabeçalho de pacote
 * @param type Tipo do pacote
 * @param size Tamanho do pacote
 * @param id ID da conexão
 * @param clientTick Tick do cliente
 * @return Cabeçalho preenchido
 */
PacketHeader createHeader(uint16_t type, uint16_t size, uint16_t id, uint16_t clientTick);

} // namespace protocol
} // namespace network
} // namespace wydbr

#endif // WYDBR_NETWORK_PROTOCOL_H