/**
 * PacketTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/network/protocol/PacketTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PACKETTYPES_H
#define PACKETTYPES_H

#include <string>
#include <vector>
#include <array>
#include <cstring>

#include "../../../../core/WYDTypes.h"
#include "../../item/ItemTypes.h"
#include "../../battle/BattleTypes.h"

namespace wydbr {


/**
 * @file PacketTypes.h
 * @brief Definição dos tipos de pacotes do protocolo do WYD
 * 
 * Este arquivo contém as definições dos tipos de pacotes e estruturas
 * utilizadas pelo protocolo de rede do WYD original, com melhorias
 * para corrigir bugs e vulnerabilidades.
 */

namespace wyd {
namespace server {
namespace network {

// Tamanhos de buffer
#define MAX_PACKET_SIZE        8192 // Tamanho máximo de pacote
#define DEFAULT_BUFFER_SIZE    4096 // Tamanho padrão de buffer
#define MIN_PACKET_SIZE        12   // Tamanho mínimo de pacote (header)

// Tempos
#define PING_INTERVAL          10000 // Intervalo de ping em ms
#define PING_TIMEOUT           30000 // Timeout de ping em ms
#define CONNECTION_TIMEOUT     60000 // Timeout de conexão em ms
#define RECONNECT_INTERVAL     5000  // Intervalo de reconexão em ms
#define MAX_RECONNECT_ATTEMPTS 5     // Máximo de tentativas de reconexão

// Flags de pacote
#define PACKET_FLAG_ENCRYPTED  0x01  // Pacote encriptado
#define PACKET_FLAG_COMPRESSED 0x02  // Pacote comprimido
#define PACKET_FLAG_RELIABLE   0x04  // Pacote confiável
#define PACKET_FLAG_FRAGMENTED 0x08  // Pacote fragmentado
#define PACKET_FLAG_ENCRYPTED2 0x10  // Pacote com encriptação secundária

// Estados de conexão
enum class ConnectionState : BYTE {
    DISCONNECTED = 0,       // Desconectado
    CONNECTING = 1,         // Conectando
    CONNECTED = 2,          // Conectado
    AUTHENTICATING = 3,     // Autenticando
    AUTHENTICATED = 4,      // Autenticado
    ENTERING_WORLD = 5,     // Entrando no mundo
    IN_GAME = 6,            // No jogo
    DISCONNECTING = 7,      // Desconectando
    RECONNECTING = 8,       // Reconectando
};

// Tipos de pacotes do WYD Original
enum class PacketType : WORD {
    // Pacotes de sistema
    NONE = 0x0000,               // Nenhum pacote
    PING = 0x0001,               // Ping
    DISCONNECT = 0x0002,         // Desconexão
    ENCRYPT_KEY = 0x0003,        // Chave de encriptação
    SERVER_NOTICE = 0x0004,      // Aviso do servidor
    SERVER_TIME = 0x0005,        // Hora do servidor
    CHAT_MESSAGE = 0x0006,       // Mensagem de chat
    WORLD_MESSAGE = 0x0007,      // Mensagem global
    
    // Pacotes de autenticação
    LOGIN_REQUEST = 0x0100,      // Requisição de login
    LOGIN_RESPONSE = 0x0101,     // Resposta de login
    CHARACTER_LIST = 0x0102,     // Lista de personagens
    CREATE_CHARACTER = 0x0103,   // Criar personagem
    DELETE_CHARACTER = 0x0104,   // Deletar personagem
    SELECT_CHARACTER = 0x0105,   // Selecionar personagem
    LOGOUT = 0x0106,             // Logout
    
    // Pacotes de movimento e posição
    MOVEMENT = 0x0200,           // Movimento
    TELEPORT = 0x0201,           // Teleporte
    CLIENT_POSITION = 0x0202,    // Posição do cliente
    ENTITY_MOVEMENT = 0x0203,    // Movimento de entidade
    ENTITY_TELEPORT = 0x0204,    // Teleporte de entidade
    ENTITY_SPAWN = 0x0205,       // Spawn de entidade
    ENTITY_DESPAWN = 0x0206,     // Despawn de entidade
    REGION_CHANGE = 0x0207,      // Mudança de região
    
    // Pacotes de atributos e status
    ENTITY_STATS = 0x0300,       // Atributos de entidade
    PLAYER_STATS = 0x0301,       // Atributos do jogador
    LEVEL_UP = 0x0302,           // Subiu de nível
    STAT_POINT_USE = 0x0303,     // Uso de ponto de atributo
    UPDATE_HP = 0x0304,          // Atualização de HP
    UPDATE_MP = 0x0305,          // Atualização de MP
    UPDATE_EXPERIENCE = 0x0306,  // Atualização de experiência
    EFFECT_APPLY = 0x0307,       // Aplicação de efeito
    EFFECT_REMOVE = 0x0308,      // Remoção de efeito
    UPDATE_GOLD = 0x0309,        // Atualização de gold
    
    // Pacotes de combate
    ATTACK = 0x0400,             // Ataque
    ATTACK_RESULT = 0x0401,      // Resultado de ataque
    SKILL_USE = 0x0402,          // Uso de skill
    SKILL_RESULT = 0x0403,       // Resultado de skill
    ENTITY_DEATH = 0x0404,       // Morte de entidade
    ENTITY_RESURRECT = 0x0405,   // Ressurreição de entidade
    COMBAT_MODE = 0x0406,        // Modo de combate
    PVP_REQUEST = 0x0407,        // Requisição de PVP
    PVP_RESPONSE = 0x0408,       // Resposta de PVP
    
    // Pacotes de itens e inventário
    ITEM_USE = 0x0500,           // Uso de item
    ITEM_DROP = 0x0501,          // Drop de item
    ITEM_PICKUP = 0x0502,        // Pickup de item
    ITEM_MOVE = 0x0503,          // Mover item
    INVENTORY_UPDATE = 0x0504,   // Atualização de inventário
    ITEM_EQUIP = 0x0505,         // Equipar item
    ITEM_UNEQUIP = 0x0506,       // Desequipar item
    ITEM_TRADE = 0x0507,         // Troca de item
    ITEM_STORAGE = 0x0508,       // Armazenamento de item
    ITEM_REPAIR = 0x0509,        // Reparo de item
    
    // Pacotes de interação social
    CHAT_PRIVATE = 0x0600,       // Chat privado
    CHAT_PARTY = 0x0601,         // Chat de grupo
    CHAT_GUILD = 0x0602,         // Chat de guilda
    CHAT_GLOBAL = 0x0603,        // Chat global
    FRIEND_ADD = 0x0604,         // Adicionar amigo
    FRIEND_REMOVE = 0x0605,      // Remover amigo
    FRIEND_LIST = 0x0606,        // Lista de amigos
    BLOCK_USER = 0x0607,         // Bloquear usuário
    UNBLOCK_USER = 0x0608,       // Desbloquear usuário
    
    // Pacotes de grupo
    PARTY_INVITE = 0x0700,       // Convidar para grupo
    PARTY_ACCEPT = 0x0701,       // Aceitar convite de grupo
    PARTY_DECLINE = 0x0702,      // Recusar convite de grupo
    PARTY_LEAVE = 0x0703,        // Sair do grupo
    PARTY_KICK = 0x0704,         // Expulsar do grupo
    PARTY_PROMOTE = 0x0705,      // Promover líder de grupo
    PARTY_UPDATE = 0x0706,       // Atualização de grupo
    
    // Pacotes de guilda
    GUILD_CREATE = 0x0800,       // Criar guilda
    GUILD_DISBAND = 0x0801,      // Desfazer guilda
    GUILD_INVITE = 0x0802,       // Convidar para guilda
    GUILD_ACCEPT = 0x0803,       // Aceitar convite de guilda
    GUILD_DECLINE = 0x0804,      // Recusar convite de guilda
    GUILD_LEAVE = 0x0805,        // Sair da guilda
    GUILD_KICK = 0x0806,         // Expulsar da guilda
    GUILD_PROMOTE = 0x0807,      // Promover membro de guilda
    GUILD_DEMOTE = 0x0808,       // Rebaixar membro de guilda
    GUILD_UPDATE = 0x0809,       // Atualização de guilda
    GUILD_WAR = 0x080A,          // Guerra de guilda
    GUILD_ALLIANCE = 0x080B,     // Aliança de guilda
    
    // Pacotes de missões
    QUEST_LIST = 0x0900,         // Lista de missões
    QUEST_ACCEPT = 0x0901,       // Aceitar missão
    QUEST_COMPLETE = 0x0902,     // Completar missão
    QUEST_ABANDON = 0x0903,      // Abandonar missão
    QUEST_UPDATE = 0x0904,       // Atualização de missão
    
    // Pacotes de comércio
    TRADE_REQUEST = 0x0A00,      // Requisição de comércio
    TRADE_ACCEPT = 0x0A01,       // Aceitar comércio
    TRADE_DECLINE = 0x0A02,      // Recusar comércio
    TRADE_CANCEL = 0x0A03,       // Cancelar comércio
    TRADE_ADD_ITEM = 0x0A04,     // Adicionar item ao comércio
    TRADE_REMOVE_ITEM = 0x0A05,  // Remover item do comércio
    TRADE_SET_GOLD = 0x0A06,     // Definir gold do comércio
    TRADE_CONFIRM = 0x0A07,      // Confirmar comércio
    TRADE_COMPLETE = 0x0A08,     // Completar comércio
    
    // Pacotes de loja
    SHOP_OPEN = 0x0B00,          // Abrir loja
    SHOP_CLOSE = 0x0B01,         // Fechar loja
    SHOP_BUY = 0x0B02,           // Comprar item
    SHOP_SELL = 0x0B03,          // Vender item
    SHOP_LIST = 0x0B04,          // Lista de itens da loja
    
    // Pacotes de administração
    ADMIN_COMMAND = 0x0C00,      // Comando de administração
    ADMIN_TELEPORT = 0x0C01,     // Teleporte de administração
    ADMIN_SPAWN = 0x0C02,        // Spawn de administração
    ADMIN_KILL = 0x0C03,         // Kill de administração
    ADMIN_BAN = 0x0C04,          // Ban de administração
    ADMIN_UNBAN = 0x0C05,        // Unban de administração
    ADMIN_KICK = 0x0C06,         // Kick de administração
    ADMIN_GIVE_ITEM = 0x0C07,    // Dar item de administração
    ADMIN_GIVE_GOLD = 0x0C08,    // Dar gold de administração
    
    // Pacotes de eventos
    EVENT_UPDATE = 0x0D00,       // Atualização de evento
    EVENT_START = 0x0D01,        // Início de evento
    EVENT_END = 0x0D02,          // Fim de evento
    EVENT_JOIN = 0x0D03,         // Entrar em evento
    EVENT_LEAVE = 0x0D04,        // Sair de evento
    EVENT_REWARD = 0x0D05,       // Recompensa de evento
    
    // Pacotes de sistema avançado
    CONFIG_UPDATE = 0x0E00,      // Atualização de configuração
    WEATHER_UPDATE = 0x0E01,     // Atualização de clima
    TIME_UPDATE = 0x0E02,        // Atualização de tempo
    SERVER_STATUS = 0x0E03,      // Status do servidor
    CLIENT_READY = 0x0E04,       // Cliente pronto
    
    // Pacotes extras/especiais (não existiam no WYD original, mas são melhorias)
    ANTI_HACK_CHALLENGE = 0x0F00, // Desafio anti-hack
    ANTI_HACK_RESPONSE = 0x0F01,  // Resposta anti-hack
    BANDWIDTH_CONTROL = 0x0F02,   // Controle de banda
    RECONNECT_TOKEN = 0x0F03,     // Token de reconexão
    HEARTBEAT = 0x0F04,           // Heartbeat
    PACKET_ACK = 0x0F05,          // Confirmação de pacote
    PACKET_NACK = 0x0F06,         // Não-confirmação de pacote
    
    // Máximo
    MAX_PACKET_TYPE = 0xFFFF,     // Máximo
};

/**
 * @brief Cabeçalho de pacote (original: STRUCT_PKTHEADER)
 * 
 * Estrutura do cabeçalho de pacote do WYD original.
 * Melhorada para incluir mais flags e controle.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com o protocolo original
struct STRUCT_PACKET_HEADER {
    WORD wSize;                // Tamanho do pacote
    WORD wType;                // Tipo do pacote (PacketType)
    BYTE byKeyID;              // ID da chave de encriptação
    BYTE byFlags;              // Flags do pacote
    DWORD dwClientTick;        // Tick do cliente
    
    STRUCT_PACKET_HEADER()
        : wSize(sizeof(STRUCT_PACKET_HEADER))
        , wType(0)
        , byKeyID(0)
        , byFlags(0)
        , dwClientTick(0)
    {
    }
    
    // Helpers para manipulação de flags
    /**
 * IsEncrypted
 * 
 * Implementa a funcionalidade IsEncrypted conforme especificação original.
 * @return Retorna bool
 */

    bool IsEncrypted() const{
        return (byFlags & PACKET_FLAG_ENCRYPTED) != 0;
    }
    
    /**
 * IsCompressed
 * 
 * Implementa a funcionalidade IsCompressed conforme especificação original.
 * @return Retorna bool
 */

    
    bool IsCompressed() const{
        return (byFlags & PACKET_FLAG_COMPRESSED) != 0;
    }
    
    /**
 * IsReliable
 * 
 * Implementa a funcionalidade IsReliable conforme especificação original.
 * @return Retorna bool
 */

    
    bool IsReliable() const{
        return (byFlags & PACKET_FLAG_RELIABLE) != 0;
    }
    
    /**
 * IsFragmented
 * 
 * Implementa a funcionalidade IsFragmented conforme especificação original.
 * @return Retorna bool
 */

    
    bool IsFragmented() const{
        return (byFlags & PACKET_FLAG_FRAGMENTED) != 0;
    }
    
    /**
 * HasSecondaryEncryption
 * 
 * Implementa a funcionalidade HasSecondaryEncryption conforme especificação original.
 * @return Retorna bool
 */

    
    bool HasSecondaryEncryption() const{
        return (byFlags & PACKET_FLAG_ENCRYPTED2) != 0;
    }
    
    /**
 * SetEncrypted
 * 
 * Implementa a funcionalidade SetEncrypted conforme especificação original.
 * @param true Parâmetro true
 */

    
    void SetEncrypted(bool value = true){
        if (value) {
            byFlags |= PACKET_FLAG_ENCRYPTED;
        } else {
            byFlags &= ~PACKET_FLAG_ENCRYPTED;
        }
    }
    
    /**
 * SetCompressed
 * 
 * Implementa a funcionalidade SetCompressed conforme especificação original.
 * @param true Parâmetro true
 */

    
    void SetCompressed(bool value = true){
        if (value) {
            byFlags |= PACKET_FLAG_COMPRESSED;
        } else {
            byFlags &= ~PACKET_FLAG_COMPRESSED;
        }
    }
    
    /**
 * SetReliable
 * 
 * Implementa a funcionalidade SetReliable conforme especificação original.
 * @param true Parâmetro true
 */

    
    void SetReliable(bool value = true){
        if (value) {
            byFlags |= PACKET_FLAG_RELIABLE;
        } else {
            byFlags &= ~PACKET_FLAG_RELIABLE;
        }
    }
    
    /**
 * SetFragmented
 * 
 * Implementa a funcionalidade SetFragmented conforme especificação original.
 * @param true Parâmetro true
 */

    
    void SetFragmented(bool value = true){
        if (value) {
            byFlags |= PACKET_FLAG_FRAGMENTED;
        } else {
            byFlags &= ~PACKET_FLAG_FRAGMENTED;
        }
    }
    
    /**
 * SetSecondaryEncryption
 * 
 * Implementa a funcionalidade SetSecondaryEncryption conforme especificação original.
 * @param true Parâmetro true
 */

    
    void SetSecondaryEncryption(bool value = true){
        if (value) {
            byFlags |= PACKET_FLAG_ENCRYPTED2;
        } else {
            byFlags &= ~PACKET_FLAG_ENCRYPTED2;
        }
    }
};
#pragma pack(pop)

/**
 * @brief Extensão de cabeçalho para pacotes fragmentados
 * 
 * Esta estrutura não existia no WYD original, mas foi adicionada
 * para permitir fragmentação de pacotes grandes.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_FRAGMENT {
    WORD wFragmentID;          // ID do fragmento
    WORD wTotalFragments;      // Total de fragmentos
    WORD wFragmentIndex;       // Índice do fragmento (0-based)
    WORD wOriginalSize;        // Tamanho original do pacote completo
    
    STRUCT_PACKET_FRAGMENT()
        : wFragmentID(0)
        , wTotalFragments(1)
        , wFragmentIndex(0)
        , wOriginalSize(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Ping (original: STRUCT_PKTPING)
 * 
 * Estrutura do pacote de ping do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_PING {
    DWORD dwPingTime;          // Tempo de ping
    DWORD dwClientTime;        // Tempo do cliente
    DWORD dwServerTime;        // Tempo do servidor
    
    STRUCT_PACKET_PING()
        : dwPingTime(0)
        , dwClientTime(0)
        , dwServerTime(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Login (original: STRUCT_PKTLOGIN)
 * 
 * Estrutura do pacote de login do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_LOGIN {
    char szLogin[32];          // Login
    char szPassword[40];       // Senha (hash)
    BYTE byClientVersion[8];   // Versão do cliente
    BYTE byReserved[20];       // /**
 * STRUCT_PACKET_LOGIN
 * 
 * Implementa a funcionalidade STRUCT_PACKET_LOGIN conforme especificação original.
 * @return Retorna Reservado
 */
 Reservado
    
    STRUCT_PACKET_LOGIN(){
        memset(this, 0, sizeof(STRUCT_PACKET_LOGIN));
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Resposta de Login (original: STRUCT_PKTLOGINRESP)
 * 
 * Estrutura do pacote de resposta de login do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_LOGIN_RESPONSE {
    BYTE byResult;             // Resultado (0: falha, 1: sucesso)
    BYTE byReserved[3];        // Reservado
    DWORD dwServerTime;        // Tempo do servidor
    char szServerName[16];     // Nome do servidor
    BYTE byCharCount;          // Quantidade de personagens
    BYTE byPremiumType;        // Tipo de conta premium
    DWORD dwPremiumTime;       // Tempo de premium restante
    BYTE byServerList[50];     // Lista de /**
 * STRUCT_PACKET_LOGIN_RESPONSE
 * 
 * Implementa a funcionalidade STRUCT_PACKET_LOGIN_RESPONSE conforme especificação original.
 * @return Retorna servidores
 */
 servidores
    
    STRUCT_PACKET_LOGIN_RESPONSE(){
        memset(this, 0, sizeof(STRUCT_PACKET_LOGIN_RESPONSE));
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Criação de Personagem (original: STRUCT_PKTCREATCHAR)
 * 
 * Estrutura do pacote de criação de personagem do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_CREATE_CHARACTER {
    char szCharName[16];       // Nome do personagem
    BYTE byClass;              // Classe
    BYTE byFace;               // /**
 * STRUCT_PACKET_CREATE_CHARACTER
 * 
 * Implementa a funcionalidade STRUCT_PACKET_CREATE_CHARACTER conforme especificação original.
 * @return Retorna Rosto
 */
 Rosto
    
    STRUCT_PACKET_CREATE_CHARACTER(){
        memset(this, 0, sizeof(STRUCT_PACKET_CREATE_CHARACTER));
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Movimento (original: STRUCT_PKTMOVEMENT)
 * 
 * Estrutura do pacote de movimento do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_MOVEMENT {
    WORD wSrcX;                // Posição X de origem
    WORD wSrcY;                // Posição Y de origem
    WORD wDstX;                // Posição X de destino
    WORD wDstY;                // Posição Y de destino
    BYTE byMoveType;           // Tipo de movimento
    BYTE bySpeed;              // Velocidade
    DWORD dwEntityID;          // ID da entidade
    
    STRUCT_PACKET_MOVEMENT()
        : wSrcX(0)
        , wSrcY(0)
        , wDstX(0)
        , wDstY(0)
        , byMoveType(0)
        , bySpeed(0)
        , dwEntityID(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Ataque (original: STRUCT_PKTATTACK)
 * 
 * Estrutura do pacote de ataque do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_ATTACK {
    DWORD dwAttackerID;        // ID do atacante
    DWORD dwTargetID;          // ID do alvo
    WORD wSkillID;             // ID da skill (0 para ataque normal)
    BYTE bySkillLevel;         // Nível da skill
    BYTE byAnimationDelay;     // Delay da animação
    
    STRUCT_PACKET_ATTACK()
        : dwAttackerID(0)
        , dwTargetID(0)
        , wSkillID(0)
        , bySkillLevel(0)
        , byAnimationDelay(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Resultado de Ataque (original: STRUCT_PKTATTACKRESP)
 * 
 * Estrutura do pacote de resultado de ataque do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_ATTACK_RESULT {
    DWORD dwAttackerID;        // ID do atacante
    DWORD dwTargetID;          // ID do alvo
    WORD wSkillID;             // ID da skill (0 para ataque normal)
    WORD wDamage;              // Dano causado
    BYTE byResult;             // Resultado (0: errou, 1: acertou, 2: crítico, etc.)
    BYTE byFlag;               // Flag
    WORD wHP;                  // HP restante do alvo
    WORD wMP;                  // MP restante do alvo
    
    STRUCT_PACKET_ATTACK_RESULT()
        : dwAttackerID(0)
        , dwTargetID(0)
        , wSkillID(0)
        , wDamage(0)
        , byResult(0)
        , byFlag(0)
        , wHP(0)
        , wMP(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Uso de Skill (original: STRUCT_PKTSKILLUSE)
 * 
 * Estrutura do pacote de uso de skill do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_SKILL_USE {
    DWORD dwCasterID;          // ID do conjurador
    DWORD dwTargetID;          // ID do alvo
    WORD wSkillID;             // ID da skill
    BYTE bySkillLevel;         // Nível da skill
    BYTE byTargetType;         // Tipo de alvo
    WORD wPosX;                // Posição X do alvo (para skills de área)
    WORD wPosY;                // Posição Y do alvo (para skills de área)
    
    STRUCT_PACKET_SKILL_USE()
        : dwCasterID(0)
        , dwTargetID(0)
        , wSkillID(0)
        , bySkillLevel(0)
        , byTargetType(0)
        , wPosX(0)
        , wPosY(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Resultado de Skill (original: STRUCT_PKTSKILLRESP)
 * 
 * Estrutura do pacote de resultado de skill do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_SKILL_RESULT {
    DWORD dwCasterID;          // ID do conjurador
    DWORD dwTargetID;          // ID do alvo
    WORD wSkillID;             // ID da skill
    WORD wDamage;              // Dano/Cura causado
    BYTE byResult;             // Resultado (0: falha, 1: sucesso, etc.)
    BYTE byFlag;               // Flag
    WORD wHP;                  // HP restante do alvo
    WORD wMP;                  // MP restante do alvo
    WORD wCasterMP;            // MP restante do conjurador
    
    STRUCT_PACKET_SKILL_RESULT()
        : dwCasterID(0)
        , dwTargetID(0)
        , wSkillID(0)
        , wDamage(0)
        , byResult(0)
        , byFlag(0)
        , wHP(0)
        , wMP(0)
        , wCasterMP(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Mensagem de Chat (original: STRUCT_PKTCHATMSG)
 * 
 * Estrutura do pacote de mensagem de chat do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_CHAT_MESSAGE {
    BYTE byType;               // Tipo de chat
    DWORD dwSenderID;          // ID do remetente
    char szSender[16];         // Nome do remetente
    char szMessage[100];       // /**
 * STRUCT_PACKET_CHAT_MESSAGE
 * 
 * Implementa a funcionalidade STRUCT_PACKET_CHAT_MESSAGE conforme especificação original.
 * @return Retorna Mensagem
 */
 Mensagem
    
    STRUCT_PACKET_CHAT_MESSAGE(){
        memset(this, 0, sizeof(STRUCT_PACKET_CHAT_MESSAGE));
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Uso de Item (original: STRUCT_PKTITEMUSE)
 * 
 * Estrutura do pacote de uso de item do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_ITEM_USE {
    WORD wItemID;              // ID do item
    WORD wSlot;                // Slot do item
    DWORD dwTargetID;          // ID do alvo (0 para uso próprio)
    
    STRUCT_PACKET_ITEM_USE()
        : wItemID(0)
        , wSlot(0)
        , dwTargetID(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Atualização de Inventário (original: STRUCT_PKTINVENTORY)
 * 
 * Estrutura do pacote de atualização de inventário do WYD original.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_INVENTORY_UPDATE {
    WORD wSlot;                // Slot atualizado (-1 para todo o inventário)
    STRUCT_ITEM stItem;        // Item atualizado
    DWORD dwGold;              // Gold atual
    
    STRUCT_PACKET_INVENTORY_UPDATE()
        : wSlot(0)
        , dwGold(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Anti-Hack (não existia no WYD original)
 * 
 * Estrutura de pacote para verificação anti-hack.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_ANTI_HACK {
    DWORD dwChallengeCode;     // Código de desafio
    BYTE byType;               // Tipo de verificação
    BYTE byRegion[3];          // Região de memória a verificar
    DWORD dwChecksum;          // Checksum esperado
    
    STRUCT_PACKET_ANTI_HACK()
        : dwChallengeCode(0)
        , byType(0)
        , dwChecksum(0)
    {
        memset(byRegion, 0, sizeof(byRegion));
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Heartbeat (não existia no WYD original)
 * 
 * Estrutura de pacote para heartbeat e detecção de lag.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_HEARTBEAT {
    DWORD dwSequence;          // Sequência do heartbeat
    DWORD dwTimestamp;         // Timestamp
    BYTE byNetworkQuality;     // Qualidade da rede (0-100)
    BYTE byReserved[3];        // Reservado
    
    STRUCT_PACKET_HEARTBEAT()
        : dwSequence(0)
        , dwTimestamp(0)
        , byNetworkQuality(100)
    {
        memset(byReserved, 0, sizeof(byReserved));
    }
};
#pragma pack(pop)

/**
 * @brief Pacote de Confirmação (não existia no WYD original)
 * 
 * Estrutura de pacote para confirmação de pacotes confiáveis.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_ACK {
    DWORD dwSequence;          // Sequência do pacote confirmado
    WORD wType;                // Tipo do pacote confirmado
    WORD wReserved;            // Reservado
    
    STRUCT_PACKET_ACK()
        : dwSequence(0)
        , wType(0)
        , wReserved(0)
    {
    }
};
#pragma pack(pop)

} // namespace network
} // namespace server
} // namespace wyd

#endif // PACKETTYPES_H

} // namespace wydbr
