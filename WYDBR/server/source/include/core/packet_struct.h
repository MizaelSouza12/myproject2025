/**
 * packet_struct.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/core/packet_struct.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PACKET_STRUCT_H
#define PACKET_STRUCT_H

/**
 * Definição das estruturas de pacotes de rede do WYD
 * 
 * Estas estruturas são baseadas na análise do protocolo de rede do WYD
 * e serão refinadas conforme a engenharia reversa progride.
 */

#include <stdint.h>
#include "item_struct.h"

#pragma pack(push, 1)  // Importante: garante que não há padding entre os campos

namespace wydbr {


/**
 * Cabeçalho padrão para todos os pacotes
 */
typedef struct _PACKET_HEADER {
    uint16_t Size;       // Tamanho total do pacote
    uint16_t Type;       // Tipo/comando do pacote
    uint32_t Checksum;   // Checksum para validação
    uint32_t ClientId;   // ID do cliente/sessão
} PACKET_HEADER;

/**
 * Pacote básico com apenas cabeçalho 
 */
typedef struct _PACKET_DEFAULT {
    PACKET_HEADER Header;
} PACKET_DEFAULT;

/**
 * Pacote de login
 */
typedef struct _PACKET_LOGIN {
    PACKET_HEADER Header;
    char Username[16];   // Nome de usuário
    char Password[32];   // Senha (possivelmente cifrada)
    uint32_t Version;    // Versão do cliente
    uint8_t  MacAddress[6]; // Endereço MAC do cliente
    uint16_t Padding;    // Alinhamento
} PACKET_LOGIN;

/**
 * Pacote de resposta de login
 */
typedef struct _PACKET_LOGIN_RESPONSE {
    PACKET_HEADER Header;
    uint16_t Result;     // Resultado (0 = sucesso, outros = códigos de erro)
    uint32_t AccountId;  // ID da conta
    uint8_t  AccessLevel; // Nível de acesso (jogador comum, GM, admin)
    uint8_t  Reserved[11]; // Bytes reservados para uso futuro
} PACKET_LOGIN_RESPONSE;

/**
 * Pacote de movimento
 */
typedef struct _PACKET_MOVEMENT {
    PACKET_HEADER Header;
    uint16_t PosX;       // Posição X
    uint16_t PosY;       // Posição Y
    uint8_t  Direction;  // Direção (0-7 para 8 direções)
    uint8_t  Reserved[7]; // Bytes reservados
} PACKET_MOVEMENT;

/**
 * Pacote de chat
 */
typedef struct _PACKET_CHAT {
    PACKET_HEADER Header;
    uint8_t  ChatType;   // Tipo de chat (geral, grupo, privado)
    char     ToName[16]; // Nome do destinatário (para mensagens privadas)
    char     Message[100]; // Conteúdo da mensagem
} PACKET_CHAT;

/**
 * Pacote de atualização de atributos
 */
typedef struct _PACKET_STATS_UPDATE {
    PACKET_HEADER Header;
    uint16_t Level;      // Nível do personagem
    uint32_t Experience; // Experiência atual
    uint16_t Strength;   // Força
    uint16_t Dexterity;  // Destreza
    uint16_t Intelligence; // Inteligência
    uint16_t Constitution; // Constituição
    uint32_t HP;         // Pontos de vida atuais
    uint32_t MaxHP;      // Pontos de vida máximos
    uint32_t MP;         // Pontos de mana atuais
    uint32_t MaxMP;      // Pontos de mana máximos
    uint16_t Attack;     // Ataque
    uint16_t Defense;    // Defesa
    uint16_t Resistance; // Resistência
    uint16_t StatPoints; // Pontos de atributo disponíveis
    uint16_t SkillPoints; // Pontos de habilidade disponíveis
} PACKET_STATS_UPDATE;

/**
 * Pacote de transação de item
 */
typedef struct _PACKET_ITEM_TRANSACTION {
    PACKET_HEADER Header;
    uint8_t  TransactionType; // Tipo (drop, pegar, trocar, comprar, vender)
    uint16_t SourceSlot;     // Slot de origem
    uint16_t DestinationSlot; // Slot de destino
    uint16_t Quantity;       // Quantidade (para itens empilháveis)
    STRUCT_ITEM Item;        // Dados do item
} PACKET_ITEM_TRANSACTION;

// Definição de constantes para tipos de pacotes
enum PACKET_TYPE {
    PACKET_TYPE_LOGIN            = 0x101,   // Login
    PACKET_TYPE_LOGIN_RESPONSE   = 0x102,   // Resposta de login
    PACKET_TYPE_CREATE_CHARACTER = 0x110,   // Criação de personagem
    PACKET_TYPE_DELETE_CHARACTER = 0x111,   // Exclusão de personagem
    PACKET_TYPE_ENTER_WORLD      = 0x120,   // Entrar no mundo
    PACKET_TYPE_MOVEMENT         = 0x210,   // Movimento
    PACKET_TYPE_CHAT             = 0x301,   // Chat
    PACKET_TYPE_STATS_UPDATE     = 0x401,   // Atualização de atributos
    PACKET_TYPE_ITEM_TRANSACTION = 0x501    // Transação de item
};

#pragma pack(pop)  // Restaura o alinhamento original

#endif // PACKET_STRUCT_H

} // namespace wydbr
