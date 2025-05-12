/**
 * TM_Protocol.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/TM_Protocol.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _TM_PROTOCOL_H_
#define _TM_PROTOCOL_H_

/**
 * @file TM_Protocol.h
 * @brief Protocolo de comunicação do WYD
 * 
 * Este arquivo contém as estruturas de pacotes de comunicação utilizadas no WYD,
 * mantendo compatibilidade binária exata com o cliente original.
 */

#include "GlobalDef.h"
#include "TM_Item.h"
#include "TM_Mob.h"

namespace wydbr {

// Estruturas de protocolo para compatibilidade binária
#pragma pack(push, 1)

/**
 * @brief Estrutura para pacote P_Initial (inicialização da conexão)
 */
struct MSG_INITIAL
{
    STRUCT_PHEADER Header;
    uint16_t Version;
    uint16_t Unk1;
    uint8_t  Unk2;
    uint8_t  Unk3;
};

/**
 * @brief Estrutura para pacote de login na conta
 */
struct MSG_ACCOUNT_LOGIN
{
    STRUCT_PHEADER Header;
    char   AccountName[ACCOUNTNAME_LENGTH];
    char   Password[ACCOUNTNAME_LENGTH]; // Senha
    uint8_t  Unk1;
    uint8_t  Unk2;
    uint16_t  ClientVersion;
    uint16_t  Unk3;
    char   Unk4;
};

/**
 * @brief Estrutura para resposta de login na conta
 */
struct MSG_ACCOUNT_LOGIN_RESPONSE
{
    STRUCT_PHEADER Header;
    int16_t Result;     // 0 = Sucesso, outros valores indicam erro
    uint8_t Unk1;
    uint8_t Unk2;
    char   Unk3[44];
};

/**
 * @brief Estrutura para pacote de logout da conta
 */
struct MSG_ACCOUNT_LOGOUT
{
    STRUCT_PHEADER Header;
    int32_t Unk1;
};

/**
 * @brief Estrutura para pacote de criação de personagem
 */
struct MSG_CHARACTER_CREATE
{
    STRUCT_PHEADER Header;
    char   MobName[NAME_LENGTH];
    uint16_t Type; // 0 a 3 - Classe
    char   Unk1;
    char   Unk2;
};

/**
 * @brief Estrutura para pacote de resposta de criação de personagem
 */
struct MSG_CHARACTER_CREATE_RESPONSE
{
    STRUCT_PHEADER Header;
    int16_t Result; // 0 = Sucesso, outros valores indicam erro
};

/**
 * @brief Estrutura para pacote de deleção de personagem
 */
struct MSG_CHARACTER_DELETE
{
    STRUCT_PHEADER Header;
    char   MobName[NAME_LENGTH];
    char   Password[ACCOUNTNAME_LENGTH];
};

/**
 * @brief Estrutura para pacote de resposta de deleção de personagem
 */
struct MSG_CHARACTER_DELETE_RESPONSE
{
    STRUCT_PHEADER Header;
    int16_t Result; // 0 = Sucesso, outros valores indicam erro
};

/**
 * @brief Estrutura para pacote de login no personagem
 */
struct MSG_CHARACTER_LOGIN
{
    STRUCT_PHEADER Header;
    char   MobName[NAME_LENGTH];
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de login no personagem
 */
struct MSG_CHARACTER_LOGIN_RESPONSE
{
    STRUCT_PHEADER Header;
    int16_t Result;         // 0 = Sucesso, outros valores indicam erro
    uint16_t PosX;          // Posição X do personagem
    uint16_t PosY;          // Posição Y do personagem
    uint32_t ClientTime;    // Timestamp
    uint16_t Unk1;
    uint8_t  Unk2;
    STRUCT_MOB Mob;         // Dados do personagem
    char   Unk3[196];
};

/**
 * @brief Estrutura para pacote de chat
 */
struct MSG_CHAT
{
    STRUCT_PHEADER Header;
    uint16_t Type;              // Tipo de mensagem
    char   MobName[NAME_LENGTH];
    char   Message[128];
};

/**
 * @brief Estrutura para pacote de movimento
 */
struct MSG_MOVE
{
    STRUCT_PHEADER Header;
    uint16_t PosX;          // Posição X
    uint16_t PosY;          // Posição Y
    uint16_t DestX;         // Destino X
    uint16_t DestY;         // Destino Y
    uint16_t Speed;         // Velocidade
    uint16_t MoveType;      // Tipo de movimento
};

/**
 * @brief Estrutura para pacote de ataque
 */
struct MSG_ATTACK
{
    STRUCT_PHEADER Header;
    uint16_t MobID;         // ID do alvo
    uint16_t SkillID;       // ID da habilidade (0 = ataque normal)
    uint16_t Unk1;
    uint16_t Unk2;
};

/**
 * @brief Estrutura para pacote de resposta de ataque
 */
struct MSG_ATTACK_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t AttackerID;    // ID do atacante
    uint16_t DefenderID;    // ID do defensor
    uint16_t Damage;        // Dano causado
    uint16_t Unk1;
    uint16_t Unk2;
    uint16_t Unk3;
    uint16_t Unk4;
    uint16_t Unk5;
};

/**
 * @brief Estrutura para pacote de uso de item
 */
struct MSG_ITEM_USE
{
    STRUCT_PHEADER Header;
    uint16_t SrcSlot;       // Slot de origem
    uint16_t DstSlot;       // Slot de destino (para combinar itens)
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de uso de item
 */
struct MSG_ITEM_USE_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de equipar item
 */
struct MSG_ITEM_EQUIP
{
    STRUCT_PHEADER Header;
    uint16_t SrcSlot;       // Slot de origem (inventário)
    uint16_t DstSlot;       // Slot de destino (equipamento)
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de equipar item
 */
struct MSG_ITEM_EQUIP_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de remover item
 */
struct MSG_ITEM_REMOVE
{
    STRUCT_PHEADER Header;
    uint16_t SrcSlot;       // Slot de origem (equipamento)
    uint16_t DstSlot;       // Slot de destino (inventário)
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de remover item
 */
struct MSG_ITEM_REMOVE_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de drop de item
 */
struct MSG_ITEM_DROP
{
    STRUCT_PHEADER Header;
    uint16_t SrcSlot;       // Slot de origem
    uint16_t Unk1;
    uint16_t PosX;          // Posição X
    uint16_t PosY;          // Posição Y
    uint16_t Unk2;
};

/**
 * @brief Estrutura para pacote de resposta de drop de item
 */
struct MSG_ITEM_DROP_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de pegar item do chão
 */
struct MSG_ITEM_GET
{
    STRUCT_PHEADER Header;
    uint16_t ItemID;        // ID do item no chão
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de pegar item do chão
 */
struct MSG_ITEM_GET_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t ItemSlot;      // Slot onde o item foi colocado
    STRUCT_ITEM Item;       // Item obtido
};

/**
 * @brief Estrutura para pacote de abrir loja de NPC
 */
struct MSG_SHOP_OPEN
{
    STRUCT_PHEADER Header;
    uint16_t MobID;         // ID do NPC
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de abrir loja de NPC
 */
struct MSG_SHOP_OPEN_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t ShopType;      // Tipo de loja
    char   ShopName[24];    // Nome da loja
    STRUCT_ITEM Items[24];  // Itens da loja
};

/**
 * @brief Estrutura para pacote de fechar loja de NPC
 */
struct MSG_SHOP_CLOSE
{
    STRUCT_PHEADER Header;
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de comprar item de loja de NPC
 */
struct MSG_SHOP_BUY
{
    STRUCT_PHEADER Header;
    uint16_t ShopSlot;      // Slot da loja
    uint16_t Unk1;
    uint16_t Quantity;      // Quantidade
    uint16_t Unk2;
};

/**
 * @brief Estrutura para pacote de resposta de comprar item de loja de NPC
 */
struct MSG_SHOP_BUY_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de vender item para loja de NPC
 */
struct MSG_SHOP_SELL
{
    STRUCT_PHEADER Header;
    uint16_t InvenSlot;     // Slot do inventário
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de vender item para loja de NPC
 */
struct MSG_SHOP_SELL_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de abrir banco
 */
struct MSG_STORAGE_OPEN
{
    STRUCT_PHEADER Header;
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de abrir banco
 */
struct MSG_STORAGE_OPEN_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    STRUCT_STORAGE Storage; // Dados do banco
};

/**
 * @brief Estrutura para pacote de colocar item no banco
 */
struct MSG_STORAGE_PUT
{
    STRUCT_PHEADER Header;
    uint16_t InvenSlot;     // Slot do inventário
    uint16_t StorageSlot;   // Slot do banco
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de colocar item no banco
 */
struct MSG_STORAGE_PUT_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de pegar item do banco
 */
struct MSG_STORAGE_GET
{
    STRUCT_PHEADER Header;
    uint16_t StorageSlot;   // Slot do banco
    uint16_t InvenSlot;     // Slot do inventário
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de pegar item do banco
 */
struct MSG_STORAGE_GET_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de fechar banco
 */
struct MSG_STORAGE_CLOSE
{
    STRUCT_PHEADER Header;
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de usar habilidade
 */
struct MSG_SKILL_USE
{
    STRUCT_PHEADER Header;
    uint16_t SkillID;       // ID da habilidade
    uint16_t SkillParam;    // Parâmetro da habilidade
    uint16_t TargetX;       // Posição X do alvo
    uint16_t TargetY;       // Posição Y do alvo
    uint16_t TargetID;      // ID do alvo (0 = sem alvo)
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de usar habilidade
 */
struct MSG_SKILL_USE_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de iniciar comércio
 */
struct MSG_TRADE_START
{
    STRUCT_PHEADER Header;
    uint16_t TargetID;      // ID do alvo
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de iniciar comércio
 */
struct MSG_TRADE_START_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de definir item no comércio
 */
struct MSG_TRADE_SET_ITEM
{
    STRUCT_PHEADER Header;
    uint16_t MyItemSlot;    // Slot do item no meu inventário
    uint16_t TradeSlot;     // Slot do comércio (0-14)
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de definir item no comércio
 */
struct MSG_TRADE_SET_ITEM_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t MyTradeSlot;   // Slot do comércio onde meu item foi colocado
    uint16_t OpTradeSlot;   // Slot do comércio onde o item do outro foi colocado
    STRUCT_ITEM MyItem;     // Item que eu coloquei
    STRUCT_ITEM OpItem;     // Item que o outro colocou
};

/**
 * @brief Estrutura para pacote de definir gold no comércio
 */
struct MSG_TRADE_SET_GOLD
{
    STRUCT_PHEADER Header;
    uint32_t Gold;          // Quantidade de gold
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de definir gold no comércio
 */
struct MSG_TRADE_SET_GOLD_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint32_t MyGold;        // Meu gold
    uint32_t OpGold;        // Gold do outro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de aceitar comércio
 */
struct MSG_TRADE_ACCEPT
{
    STRUCT_PHEADER Header;
    uint16_t Accept;        // 1 = Aceitar, 0 = Cancelar
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de aceitar comércio
 */
struct MSG_TRADE_ACCEPT_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Aceito, outros valores indicam recusa ou erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de cancelar comércio
 */
struct MSG_TRADE_CANCEL
{
    STRUCT_PHEADER Header;
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de cancelar comércio
 */
struct MSG_TRADE_CANCEL_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de convidar para grupo
 */
struct MSG_PARTY_INVITE
{
    STRUCT_PHEADER Header;
    uint16_t TargetID;      // ID do alvo
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de convidar para grupo
 */
struct MSG_PARTY_INVITE_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Aceito, outros valores indicam recusa ou erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de aceitar convite de grupo
 */
struct MSG_PARTY_ACCEPT
{
    STRUCT_PHEADER Header;
    uint16_t LeaderID;      // ID do líder
    uint16_t Accept;        // 1 = Aceitar, 0 = Recusar
};

/**
 * @brief Estrutura para pacote de resposta de aceitar convite de grupo
 */
struct MSG_PARTY_ACCEPT_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de expulsar do grupo
 */
struct MSG_PARTY_KICK
{
    STRUCT_PHEADER Header;
    uint16_t TargetID;      // ID do alvo
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de resposta de expulsar do grupo
 */
struct MSG_PARTY_KICK_RESPONSE
{
    STRUCT_PHEADER Header;
    uint16_t Result;        // 0 = Sucesso, outros valores indicam erro
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de informações do grupo
 */
struct MSG_PARTY_INFO
{
    STRUCT_PHEADER Header;
    uint16_t PartyCount;    // Número de membros
    uint16_t LeaderID;      // ID do líder
    uint16_t MemberID[12];  // IDs dos membros
    char   MemberName[12][NAME_LENGTH]; // Nomes dos membros
    uint16_t MemberLevel[12]; // Níveis dos membros
    uint8_t  MemberClass[12]; // Classes dos membros
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de informações da guilda
 */
struct MSG_GUILD_INFO
{
    STRUCT_PHEADER Header;
    uint16_t GuildID;       // ID da guilda
    char   GuildName[16];   // Nome da guilda
    uint16_t MemberCount;   // Número de membros
    uint16_t GuildLevel;    // Nível da guilda
    uint32_t GuildExp;      // Experiência da guilda
    uint8_t  GuildLogo[16]; // Logo da guilda
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de membros da guilda
 */
struct MSG_GUILD_MEMBERS
{
    STRUCT_PHEADER Header;
    uint16_t GuildID;       // ID da guilda
    uint16_t MemberCount;   // Número de membros
    struct {
        char  MemberName[NAME_LENGTH];
        uint8_t MemberLevel;
        uint8_t MemberClass;
        uint8_t MemberRank;
        uint8_t Unk1;
    } Members[50];
    uint16_t Unk2;
};

/**
 * @brief Estrutura para pacote de manter conexão (keep alive)
 */
struct MSG_KEEP_ALIVE
{
    STRUCT_PHEADER Header;
    uint32_t ClientTime;    // Timestamp do cliente
    uint16_t Unk1;
};

/**
 * @brief Estrutura para pacote de comando do GM
 */
struct MSG_GM_COMMAND
{
    STRUCT_PHEADER Header;
    char   Command[128];    // Comando
};

#pragma pack(pop)

/**
 * @brief Calcula o checksum para um pacote
 * @param packet Ponteiro para o pacote
 * @param size Tamanho do pacote
 * @return Checksum calculado
 */
inline /**
 * CalculateChecksum
 * 
 * Implementa a funcionalidade CalculateChecksum conforme especificação original.
 * @param packet Parâmetro packet
 * @param size Parâmetro size
 * @return Retorna uint16_t
 */
 uint16_t CalculateChecksum(void* packet, int size){
    if (size < 12) return 0;
    
    uint16_t* p = (uint16_t*)packet;
    uint32_t checksum = 0;
    
    for (int i = 6; i < size / 2; i++) {
        checksum += p[i];
    }
    
    return (uint16_t)(checksum & 0xFFFF);
}

/**
 * @brief Prepara um pacote para envio
 * @param packet Ponteiro para o pacote
 * @param packetType Tipo do pacote
 * @param clientId ID do cliente
 * @param packetSize Tamanho do pacote
 */
inline /**
 * PreparePacket
 * 
 * Implementa a funcionalidade PreparePacket conforme especificação original.
 * @param packet Parâmetro packet
 * @param packetType Parâmetro packetType
 * @param clientId Parâmetro clientId
 * @param packetSize Parâmetro packetSize
 */
 void PreparePacket(void* packet, uint16_t packetType, uint16_t clientId, uint16_t packetSize){
    STRUCT_PHEADER* pHeader = (STRUCT_PHEADER*)packet;
    
    pHeader->Size = packetSize;
    pHeader->Type = packetType;
    pHeader->ClientId = clientId;
    pHeader->CheckSum = 0;
    
    pHeader->CheckSum = CalculateChecksum(packet, packetSize);
}

/**
 * @brief Verifica a validade de um pacote
 * @param packet Ponteiro para o pacote
 * @param packetSize Tamanho do pacote
 * @return true se o pacote é válido, false caso contrário
 */
inline /**
 * ValidatePacket
 * 
 * Implementa a funcionalidade ValidatePacket conforme especificação original.
 * @param packet Parâmetro packet
 * @param packetSize Parâmetro packetSize
 * @return Retorna bool
 */
 bool ValidatePacket(void* packet, int packetSize){
    if (packetSize < 12) return false;
    
    STRUCT_PHEADER* pHeader = (STRUCT_PHEADER*)packet;
    
    if (pHeader->Size != packetSize) return false;
    
    uint16_t checksum = pHeader->CheckSum;
    pHeader->CheckSum = 0;
    
    uint16_t calculatedChecksum = CalculateChecksum(packet, packetSize);
    pHeader->CheckSum = checksum;
    
    return checksum == calculatedChecksum;
}

/**
 * @brief Verifica se o tipo de pacote é válido
 * @param packetType Tipo do pacote
 * @return true se o tipo de pacote é válido, false caso contrário
 */
inline /**
 * IsValidPacketType
 * 
 * Implementa a funcionalidade IsValidPacketType conforme especificação original.
 * @param packetType Parâmetro packetType
 * @return Retorna bool
 */
 bool IsValidPacketType(uint16_t packetType){
    // Lista de tipos de pacotes válidos
    static const uint16_t validPacketTypes[] = {
        PKT_KEEP_ALIVE,
        PKT_INITIAL,
        PKT_ACCOUNT_LOGIN,
        PKT_CHARACTER_LOGOUT,
        PKT_CHARACTER_CREATE,
        PKT_ACCOUNT_LOGOUT,
        PKT_SERVER_CHANGE,
        PKT_CHAT,
        PKT_MOVE,
        PKT_ATTACK,
        PKT_ITEM_DROP,
        PKT_MOVEMENT_ON_CMD,
        PKT_MAP_TELEPORT_CONFIRM,
        PKT_MAP_TELEPORT,
        PKT_QUEST_HISTORY,
        PKT_QUEST_UPDATE,
        PKT_GAME_OPT,
        PKT_CHARACTER_LOGIN,
        PKT_NUMERIC_CHAR_SELECT,
        PKT_CHARACTER_DELETE,
        PKT_PARTY_TARGET,
        PKT_PARTY_MEMBER,
        PKT_PARTY_LEADER,
        PKT_PARTY_KICK,
        PKT_PARTY_INFO,
        PKT_QUEST,
        PKT_ITEM_GET,
        PKT_TRADE_START,
        PKT_TRADE_CLS,
        PKT_TRADE_ACCEPT,
        PKT_TRADE_SET_ITEM,
        PKT_TRADE_SET_GOLD,
        PKT_SHOP_LIST,
        PKT_SHOP_OPEN,
        PKT_SHOP_CLOSE,
        PKT_SHOP_BUY,
        PKT_SHOP_ITEM,
        PKT_ITEM_REMOVE,
        PKT_ITEM_EQUIP,
        PKT_COMMAND,
        PKT_SKILL_USE,
        PKT_SKILLIN,
        PKT_SKILLINPOINT,
        PKT_EFFECT,
        PKT_ITEM_USE,
        PKT_STORAGE_GET,
        PKT_STORAGE_OPEN,
        PKT_STORAGE_CLOSE,
        PKT_STORAGE_ITEM,
        PKT_STORAGE_GOLD,
        PKT_STORAGE_PUT,
        PKT_GUILD_MEMBER_LIST,
        PKT_GUILD_WAR_LIST,
        PKT_GUILD_LIST,
        PKT_GUILD_GET_LAST_NOTICE,
        PKT_GUILD_SET_LAST_NOTICE,
        PKT_GUILD_CREATE,
        PKT_GUILD_DISBAND,
        PKT_GUILD_NOTICE,
        PKT_GUILD_INFO_UPDATE,
        PKT_GUILD_INVITE,
        PKT_GUILD_KICK,
        PKT_GUILD_JOIN,
        PKT_TMSRV_CONNECT,
        PKT_TMSRV_DISCONNECT,
        PKT_TMSRV_CHARACTER_INFO,
        PKT_TMSRV_CHARACTER_LIST,
        PKT_TMSRV_CHARACTER_EQUIP
    };
    
    int numPacketTypes = sizeof(validPacketTypes) / sizeof(validPacketTypes[0]);
    
    for (int i = 0; i < numPacketTypes; i++) {
        if (packetType == validPacketTypes[i]) return true;
    }
    
    return false;
}

/**
 * @brief Obtém o tamanho esperado para um tipo de pacote
 * @param packetType Tipo do pacote
 * @return Tamanho esperado do pacote (0 se desconhecido)
 */
inline /**
 * GetExpectedPacketSize
 * 
 * Implementa a funcionalidade GetExpectedPacketSize conforme especificação original.
 * @param packetType Parâmetro packetType
 * @return Retorna uint16_t
 */
 uint16_t GetExpectedPacketSize(uint16_t packetType){
    switch (packetType) {
        case PKT_KEEP_ALIVE:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_KEEP_ALIVE Parâmetro MSG_KEEP_ALIVE
 * @return Retorna return
 */

            return sizeof(MSG_KEEP_ALIVE);
        case PKT_INITIAL:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_INITIAL Parâmetro MSG_INITIAL
 * @return Retorna return
 */

            return sizeof(MSG_INITIAL);
        case PKT_ACCOUNT_LOGIN:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_ACCOUNT_LOGIN Parâmetro MSG_ACCOUNT_LOGIN
 * @return Retorna return
 */

            return sizeof(MSG_ACCOUNT_LOGIN);
        case PKT_ACCOUNT_LOGOUT:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_ACCOUNT_LOGOUT Parâmetro MSG_ACCOUNT_LOGOUT
 * @return Retorna return
 */

            return sizeof(MSG_ACCOUNT_LOGOUT);
        case PKT_CHARACTER_CREATE:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_CHARACTER_CREATE Parâmetro MSG_CHARACTER_CREATE
 * @return Retorna return
 */

            return sizeof(MSG_CHARACTER_CREATE);
        case PKT_CHARACTER_DELETE:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_CHARACTER_DELETE Parâmetro MSG_CHARACTER_DELETE
 * @return Retorna return
 */

            return sizeof(MSG_CHARACTER_DELETE);
        case PKT_CHARACTER_LOGIN:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_CHARACTER_LOGIN Parâmetro MSG_CHARACTER_LOGIN
 * @return Retorna return
 */

            return sizeof(MSG_CHARACTER_LOGIN);
        case PKT_CHAT:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_CHAT Parâmetro MSG_CHAT
 * @return Retorna return
 */

            return sizeof(MSG_CHAT);
        case PKT_MOVE:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_MOVE Parâmetro MSG_MOVE
 * @return Retorna return
 */

            return sizeof(MSG_MOVE);
        case PKT_ATTACK:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_ATTACK Parâmetro MSG_ATTACK
 * @return Retorna return
 */

            return sizeof(MSG_ATTACK);
        case PKT_ITEM_USE:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_ITEM_USE Parâmetro MSG_ITEM_USE
 * @return Retorna return
 */

            return sizeof(MSG_ITEM_USE);
        case PKT_ITEM_EQUIP:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_ITEM_EQUIP Parâmetro MSG_ITEM_EQUIP
 * @return Retorna return
 */

            return sizeof(MSG_ITEM_EQUIP);
        case PKT_ITEM_REMOVE:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_ITEM_REMOVE Parâmetro MSG_ITEM_REMOVE
 * @return Retorna return
 */

            return sizeof(MSG_ITEM_REMOVE);
        case PKT_ITEM_DROP:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_ITEM_DROP Parâmetro MSG_ITEM_DROP
 * @return Retorna return
 */

            return sizeof(MSG_ITEM_DROP);
        case PKT_ITEM_GET:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_ITEM_GET Parâmetro MSG_ITEM_GET
 * @return Retorna return
 */

            return sizeof(MSG_ITEM_GET);
        case PKT_SHOP_OPEN:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_SHOP_OPEN Parâmetro MSG_SHOP_OPEN
 * @return Retorna return
 */

            return sizeof(MSG_SHOP_OPEN);
        case PKT_SHOP_CLOSE:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_SHOP_CLOSE Parâmetro MSG_SHOP_CLOSE
 * @return Retorna return
 */

            return sizeof(MSG_SHOP_CLOSE);
        case PKT_SHOP_BUY:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_SHOP_BUY Parâmetro MSG_SHOP_BUY
 * @return Retorna return
 */

            return sizeof(MSG_SHOP_BUY);
        case PKT_STORAGE_OPEN:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_STORAGE_OPEN Parâmetro MSG_STORAGE_OPEN
 * @return Retorna return
 */

            return sizeof(MSG_STORAGE_OPEN);
        case PKT_STORAGE_CLOSE:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_STORAGE_CLOSE Parâmetro MSG_STORAGE_CLOSE
 * @return Retorna return
 */

            return sizeof(MSG_STORAGE_CLOSE);
        case PKT_STORAGE_PUT:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_STORAGE_PUT Parâmetro MSG_STORAGE_PUT
 * @return Retorna return
 */

            return sizeof(MSG_STORAGE_PUT);
        case PKT_STORAGE_GET:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_STORAGE_GET Parâmetro MSG_STORAGE_GET
 * @return Retorna return
 */

            return sizeof(MSG_STORAGE_GET);
        case PKT_SKILL_USE:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_SKILL_USE Parâmetro MSG_SKILL_USE
 * @return Retorna return
 */

            return sizeof(MSG_SKILL_USE);
        case PKT_TRADE_START:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_TRADE_START Parâmetro MSG_TRADE_START
 * @return Retorna return
 */

            return sizeof(MSG_TRADE_START);
        case PKT_TRADE_SET_ITEM:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_TRADE_SET_ITEM Parâmetro MSG_TRADE_SET_ITEM
 * @return Retorna return
 */

            return sizeof(MSG_TRADE_SET_ITEM);
        case PKT_TRADE_SET_GOLD:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_TRADE_SET_GOLD Parâmetro MSG_TRADE_SET_GOLD
 * @return Retorna return
 */

            return sizeof(MSG_TRADE_SET_GOLD);
        case PKT_TRADE_ACCEPT:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_TRADE_ACCEPT Parâmetro MSG_TRADE_ACCEPT
 * @return Retorna return
 */

            return sizeof(MSG_TRADE_ACCEPT);
        case PKT_TRADE_CLS:
            /**
 * sizeof
 * 
 * Implementa a funcionalidade sizeof conforme especificação original.
 * @param MSG_TRADE_CANCEL Parâmetro MSG_TRADE_CANCEL
 * @return Retorna return
 */

            return sizeof(MSG_TRADE_CANCEL);
        // Adicione outros pacotes conforme necessário
        default:
            return 0; // Tamanho desconhecido
    }
}

} // namespace wydbr

#endif // _TM_PROTOCOL_H_