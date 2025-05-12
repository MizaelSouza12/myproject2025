/**
 * WYDPackets.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/network/WYDPackets.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_PACKETS_H
#define WYD_PACKETS_H

#include "../core/WYDTypes.h"
#include <cstring>

namespace wydbr {


/**
 * @file WYDPackets.h
 * @brief Definições dos pacotes de rede do WYD
 * 
 * Este arquivo contém as definições dos pacotes de rede usados na comunicação 
 * cliente-servidor do WYD. Mantém compatibilidade com o protocolo original,
 * mas adiciona segurança e validações para evitar exploits conhecidos.
 */

namespace wyd {
namespace net {

// Definições de tamanhos para pacotes
constexpr WORD MAX_PACKET_SIZE = 4096;      // Tamanho máximo de pacote (valor absoluto)
constexpr WORD DEFAULT_PACKET_SIZE = 256;   // Tamanho padrão para pacotes comuns

// Códigos de tipos de pacotes (MSG_TYPE)
enum PacketType : WORD {
    // Pacotes de Conexão
    PKT_REQUESTHTTPGAMEDATA = 0x101,   // Requer dados do servidor via HTTP
    PKT_GAMEDATA           = 0x102,   // Dados do servidor para o cliente
    PKT_ENTERGAME          = 0x110,   // Entrar no jogo (cliente -> servidor)
    PKT_STARTGAME          = 0x111,   // Iniciar jogo (servidor -> cliente)
    PKT_REJECTGAME         = 0x112,   // Rejeitar entrada no jogo
    PKT_ENCRYPTACC         = 0x113,   // Criptografia de conta
    PKT_CREATEACCOUNTREPLY = 0x114,   // Resposta à criação de conta
    PKT_ACCOUNTREPLY       = 0x115,   // Resposta de login
    PKT_CHANGEPWRESULT     = 0x116,   // Resultado de alteração de senha
    PKT_AGREEMENT          = 0x117,   // Acordo de termos de uso
    PKT_ENTERSECURE        = 0x118,   // Modo de segurança

    // Pacotes de Chat
    PKT_CHAT               = 0x333,   // Mensagem de chat
    PKT_CHATNOTICE         = 0x334,   // Aviso do sistema em chat
    PKT_CHATEVENT          = 0x335,   // Evento de chat

    // Pacotes de Movimento
    PKT_INITIALMOVE        = 0x340,   // Movimento inicial
    PKT_MOVE               = 0x366,   // Movimento
    PKT_MOVINGOBJECT       = 0x367,   // Objeto em movimento
    PKT_REMOVEOBJECT       = 0x368,   // Remover objeto do mundo
    PKT_SETWAYPOINT        = 0x369,   // Definir waypoint
    PKT_TELEPORT           = 0x370,   // Teleportar
    PKT_MOUSEXYMOVE        = 0x371,   // Movimento por clique

    // Pacotes de Status
    PKT_LEVELUP            = 0x373,   // Level up
    PKT_LEVELUPBONUS       = 0x374,   // Bônus de level up
    PKT_GETCHARACTERSTATUS = 0x376,   // Obter status do personagem
    PKT_UPDATEWEAPONANIM   = 0x378,   // Atualizar animação de arma
    PKT_CHANGEBODYPART     = 0x379,   // Alterar parte do corpo
    PKT_MESSENGERREPLY     = 0x37A,   // Resposta de mensageiro
    PKT_GETEVENTTIME       = 0x37B,   // Obter tempo de evento
    PKT_WIZARDRANKUPDATE   = 0x37C,   // Atualização de rank de mago
    PKT_FISHINGREPLY       = 0x37D,   // Resposta de pesca
    PKT_AUCTIONREPLY       = 0x37E,   // Resposta de leilão
    PKT_SUMMONANIMUS       = 0x37F,   // Invocar animus

    // Pacotes de Combate
    PKT_ATTACK             = 0x3A0,   // Ataque
    PKT_DAMAGEMOB          = 0x3A1,   // Dano em mob
    PKT_ATTACKMAGICUSER    = 0x3A3,   // Ataque mágico de usuário
    PKT_ATTACKMAGICMOB     = 0x3A4,   // Ataque mágico de mob
    PKT_ATTACKCLIENT       = 0x3A5,   // Ataque de cliente
    PKT_ATTACKARROW        = 0x3A6,   // Ataque com flecha
    PKT_SKILLCAST          = 0x3AE,   // Conjuração de habilidade
    PKT_SKILLPARAMETER     = 0x3AF,   // Parâmetros de habilidade

    // Pacotes de Item
    PKT_DROPITEM           = 0x3B0,   // Soltar item
    PKT_PICKUPITEM         = 0x3B1,   // Pegar item
    PKT_UPDATEITEM         = 0x3B2,   // Atualizar item
    PKT_USEITEM            = 0x3B3,   // Usar item
    PKT_GIVEITEM           = 0x3B4,   // Dar item
    PKT_ITEMLIST           = 0x3B5,   // Lista de itens
    PKT_EQUIPITEM          = 0x3B6,   // Equipar item
    PKT_GIVEITEMRESULT     = 0x3B7,   // Resultado de dar item
    PKT_UPDATEITEMEXTRA    = 0x3B8,   // Atualização extra de item
    PKT_UPDATEITEMDATE     = 0x3B9,   // Atualização de data de item
    PKT_ITEMREPAIRRESULT   = 0x3BA,   // Resultado de reparo de item
    PKT_REMOVEITEMRESULT   = 0x3BB,   // Resultado de remoção de item
    PKT_MISTICRESULT       = 0x3BC,   // Resultado de refinamento místico
    PKT_UPDATEEFFECTITEM   = 0x3BD,   // Atualizar efeito de item
    PKT_WITHDRAWCONFIRMED  = 0x3BE,   // Confirmação de saque
    PKT_WITHDRAWCOMPLETED  = 0x3BF,   // Saque completado

    // Pacotes de NPC
    PKT_NPCITEMLIST        = 0x3C0,   // Lista de itens de NPC
    PKT_NPCCHAT            = 0x3C1,   // Chat de NPC
    PKT_NPCACTIONREPLY     = 0x3C2,   // Resposta de ação de NPC
    PKT_NPCGETREPAIR       = 0x3C3,   // Obter reparo de NPC
    PKT_NPCCHATMOVE        = 0x3C4,   // Movimento de chat de NPC
    PKT_NPCDEAD            = 0x3C5,   // NPC morto
    PKT_CREATEANIMUS       = 0x3C6,   // Criar animus

    // Pacotes de Quest
    PKT_QUEST              = 0x3D0,   // Pacote de quest
    PKT_QUESTUPDATE        = 0x3D1,   // Atualização de quest
    PKT_QUESTCOMPLETE      = 0x3D2,   // Conclusão de quest
    PKT_QUESTDELETE        = 0x3D3,   // Exclusão de quest
    PKT_QUESTFLOATINGTEXT  = 0x3D4,   // Texto flutuante de quest
    PKT_QUESTREWARDLIST    = 0x3D5,   // Lista de recompensas de quest
    
    // Pacotes de Comércio
    PKT_TRADE              = 0x3E0,   // Comércio
    PKT_TRADEREPLY         = 0x3E1,   // Resposta de comércio
    PKT_TRADECANCEL        = 0x3E2,   // Cancelamento de comércio
    PKT_TRADECONFIRM       = 0x3E3,   // Confirmação de comércio
    PKT_TRADECOMPLETED     = 0x3E4,   // Comércio completado
    PKT_TRADEREQUEST       = 0x3E5,   // Solicitação de comércio
    PKT_TRADEADDITEM       = 0x3E6,   // Adicionar item ao comércio
    PKT_TRADEREMOVEITEM    = 0x3E7,   // Remover item do comércio
    PKT_TRADESTORAGE       = 0x3E8,   // Armazenamento de comércio
    PKT_TRADESTORAGECLOSE  = 0x3E9,   // Fechar armazenamento de comércio
    PKT_TRADEGOLD          = 0x3EA,   // Comércio de ouro
    
    // Pacotes de Party (Grupo)
    PKT_PARTY              = 0x3F0,   // Grupo
    PKT_PARTYREQUEST       = 0x3F1,   // Solicitação de grupo
    PKT_PARTYREPLY         = 0x3F2,   // Resposta de grupo
    PKT_PARTYNOTIFY        = 0x3F3,   // Notificação de grupo
    PKT_PARTYFORMATION     = 0x3F4,   // Formação de grupo
    PKT_PARTYSTATUS        = 0x3F5,   // Status de grupo
    PKT_PARTYINFORMATION   = 0x3F6,   // Informações de grupo
    PKT_PARTYMEMBERUPDATE  = 0x3F7,   // Atualização de membro de grupo
    
    // Pacotes de Guild
    PKT_GUILD              = 0x400,   // Guild
    PKT_GUILDREQUEST       = 0x401,   // Solicitação de guild
    PKT_GUILDREPLY         = 0x402,   // Resposta de guild
    PKT_GUILDNOTIFY        = 0x403,   // Notificação de guild
    PKT_GUILDMEMBERLIST    = 0x404,   // Lista de membros de guild
    PKT_GUILDMEMBER        = 0x405,   // Membro de guild
    PKT_GUILDDECLAREWAR    = 0x406,   // Declaração de guerra de guild
    PKT_GUILDALLIANCEINFO  = 0x407,   // Informações de aliança de guild
    PKT_GUILDUPDATEINFO    = 0x408,   // Atualização de informações de guild
    PKT_GUILDUPDATE        = 0x409,   // Atualização de guild
    PKT_GUILDWAR           = 0x40A,   // Guerra de guild
    
    // Pacotes de Sistema
    PKT_ADMIN_COMMAND      = 0x700,   // Comando de administrador
    PKT_COMMAND            = 0x701,   // Comando geral
    PKT_HACKING_TOOL       = 0x702,   // Detecção de ferramenta de hacking
    PKT_CHECKSERVER        = 0x703,   // Verificar servidor
    PKT_PONG               = 0x777,   // Resposta a ping
    PKT_PING               = 0x778,   // Ping
    PKT_DISCONNECT         = 0x999,   // Desconexão
    
    // Códigos de erro e status
    PKT_FAIL               = 0xFFF,   // Falha
    PKT_UNKNOWN            = 0xFFFF   // Desconhecido/não implementado
};

// Direção do pacote
enum /**
 * Classe WYDPacketDirection
 * 
 * Esta classe implementa a funcionalidade WYDPacketDirection conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PacketDirection {
    CLIENT_TO_SERVER,  // Cliente para servidor
    SERVER_TO_CLIENT,  // Servidor para cliente
    BIDIRECTIONAL      // Ambas direções
};

// Estado de criptografia do pacote
enum /**
 * Classe WYDPacketEncryption
 * 
 * Esta classe implementa a funcionalidade WYDPacketEncryption conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PacketEncryption {
    NONE,       // Sem criptografia
    BASIC,      // Criptografia básica (XOR)
    ADVANCED    // Criptografia avançada (AES)
};

// Estrutura base para todos os pacotes
#pragma pack(push, 1) // Alinhamento de 1 byte para compatibilidade com o cliente
struct PacketHeader {
    WORD size;      // Tamanho total do pacote em bytes
    WORD type;      // Tipo do pacote (valores de PacketType)
    DWORD handle;   // Handle do cliente ou identificador único
    
    PacketHeader() : size(0), type(PKT_UNKNOWN), handle(0) {}
    
    PacketHeader(WORD _size, WORD _type, DWORD _handle) 
        : size(_size), type(_type), handle(_handle) {}
    
    // Utilitário para validar o cabeçalho
    /**
 * isValid
 * 
 * Implementa a funcionalidade isValid conforme especificação original.
 * @return Retorna bool
 */

    bool isValid() const{
        // Verificações de segurança
        return size >= sizeof(PacketHeader) && 
               size <= MAX_PACKET_SIZE &&
               type != PKT_UNKNOWN;
    }
};

// Template para criação de pacotes com tamanho fixo
template<WORD TYPE, WORD SIZE = DEFAULT_PACKET_SIZE>
struct Packet {
    static_assert(SIZE >= sizeof(PacketHeader), "Packet size must be at least the size of PacketHeader");
    static_assert(SIZE <= MAX_PACKET_SIZE, "Packet size exceeds maximum allowed");
    
    union {
        struct {
            WORD size;      // Tamanho total do pacote em bytes
            WORD type;      // Tipo do pacote (TYPE)
            DWORD handle;   // Handle do cliente ou identificador único
            BYTE data[SIZE - sizeof(PacketHeader)];  // Dados específicos do pacote
        };
        BYTE buffer[SIZE];  // Buffer completo para acesso direto
    };
    
    // Construtor padrã/**
 * Packet
 * 
 * Implementa a funcionalidade Packet conforme especificação original.
 * @return Retorna o
 */
o
    Packet(){
        clear();
        size = SIZE;
        type = TYPE;
    }
    
    // Limpa o pacote (zera todos os bytes)
    /**
 * clear
 * 
 * Implementa a funcionalidade clear conforme especificação original.
 */

    void clear(){
        memset(buffer, 0, SIZE);
    }
    
    // Retorna o tamanho em bytes dos dados (sem o cabeçalho)
    constexpr /**
 * dataSize
 * 
 * Implementa a funcionalidade dataSize conforme especificação original.
 * @return Retorna WORD
 */
 WORD dataSize() const{
        return SIZE - sizeof(PacketHeader);
    }
    
    // Retorna o tamanho total do pacote
    constexpr /**
 * totalSize
 * 
 * Implementa a funcionalidade totalSize conforme especificação original.
 * @return Retorna WORD
 */
 WORD totalSize() const{
        return SIZE;
    }
    
    // Verifica se o pacote é válido
    /**
 * isValid
 * 
 * Implementa a funcionalidade isValid conforme especificação original.
 * @return Retorna bool
 */

    bool isValid() const{
        return size == SIZE && type == TYPE && size <= MAX_PACKET_SIZE;
    }
};

// Pacote genérico para uso quando o tipo não é conhecido antecipadamente
using GenericPacket = Packet<PKT_UNKNOWN, DEFAULT_PACKET_SIZE>;

// Pacote de login
struct PKT_ACCOUNTREPLY_STRUCT : public Packet<PKT_ACCOUNTREPLY> {
    enum ResultCode : BYTE {
        RESULT_SUCCESS = 0,
        RESULT_INVALID_ACCOUNT = 1,
        RESULT_INVALID_PASSWORD = 2,
        RESULT_ACCOUNT_BANNED = 3,
        RESULT_ACCOUNT_IN_USE = 4,
        RESULT_MAINTENANCE = 5,
        RESULT_VERSION_MISMATCH = 6,
        RESULT_SERVER_FULL = 7,
        RESULT_UNKNOWN_ERROR = 8
    };
    
    struct {
        // Os 8 primeiros bytes são o cabeçalho (já definido em Packet)
        ResultCode status;  // Resultado do login
        BYTE secureKey[16]; // Chave de segurança para criptografia
        BYTE reservedA[3];  // Padding para alinhamento
        DWORD accountId;    // ID da conta
        DWORD pktId;        // ID do pacote (anti-replay)
        BYTE reservedB[32]; // Dados reservados para uso futuro
    };
    
    PKT_ACCOUNTREPLY_STRUCT() {
        clear();
        size = sizeof(PKT_ACCOUNTREPLY_STRUCT);
        type = PKT_ACCOUNTREPLY;
        status = RESULT_UNKNOWN_ERROR;
        memset(secureKey, 0, sizeof(secureKey));
        memset(reservedA, 0, sizeof(reservedA));
        accountId = 0;
        pktId = 0;
        memset(reservedB, 0, sizeof(reservedB));
    }
};

// Pacote de movimento
struct PKT_MOVE_STRUCT : public Packet<PKT_MOVE> {
    struct {
        // Os 8 primeiros bytes são o cabeçalho (já definido em Packet)
        WORD posX;          // Posição X
        WORD posY;          // Posição Y
        BYTE direction;     // Direção do movimento (0-7)
        BYTE charState;     // Estado do personagem
        WORD clientTick;    // Tick do cliente (para sincronização)
        DWORD timestamp;    // Timestamp do movimento
        BYTE moveType;      // Tipo de movimento
        BYTE destX;         // Destino X (para pathfinding)
        BYTE destY;         // Destino Y (para pathfinding)
        BYTE reservedA[1];  // Padding para alinhamento
    };
    
    PKT_MOVE_STRUCT() {
        clear();
        size = sizeof(PKT_MOVE_STRUCT);
        type = PKT_MOVE;
        posX = 0;
        posY = 0;
        direction = 0;
        charState = 0;
        clientTick = 0;
        timestamp = 0;
        moveType = 0;
        destX = 0;
        destY = 0;
        memset(reservedA, 0, sizeof(reservedA));
    }
    
    // Validação específica para este pacote
    /**
 * validatePosition
 * 
 * Implementa a funcionalidade validatePosition conforme especificação original.
 * @return Retorna bool
 */

    bool validatePosition() const{
        // Verificar limites de mapa (exemplo)
        const WORD MAX_MAP_SIZE = 4096;
        return posX < MAX_MAP_SIZE && posY < MAX_MAP_SIZE;
    }
};

// Pacote de chat
struct PKT_CHAT_STRUCT : public Packet<PKT_CHAT, 128> {  // Tamanho maior para acomodar mensagens
    enum ChatType : BYTE {
        CHAT_NORMAL = 0,    // Chat normal (local)
        CHAT_WHISPER = 1,   // Sussurro (privado)
        CHAT_PARTY = 2,     // Chat de grupo
        CHAT_GUILD = 3,     // Chat de guild
        CHAT_GLOBAL = 4,    // Chat global
        CHAT_CLAN = 5,      // Chat de clan
        CHAT_ALLIANCE = 6,  // Chat de aliança
        CHAT_GM = 7,        // Chat de GM (game master)
        CHAT_SYSTEM = 8     // Mensagem do sistema
    };
    
    struct {
        // Os 8 primeiros bytes são o cabeçalho (já definido em Packet)
        ChatType chatType;                      // Tipo de chat
        BYTE reservedA[3];                      // Padding para alinhamento
        char nickname[16];                      // Nome do personagem (origem)
        char targetName[16];                    // Nome do alvo (para sussurros)
        char message[128 - 8 - 4 - 16 - 16];    // Mensagem (tamanho variável)
    };
    
    PKT_CHAT_STRUCT() {
        clear();
        size = sizeof(PKT_CHAT_STRUCT);
        type = PKT_CHAT;
        chatType = CHAT_NORMAL;
        memset(reservedA, 0, sizeof(reservedA));
        memset(nickname, 0, sizeof(nickname));
        memset(targetName, 0, sizeof(targetName));
        memset(message, 0, sizeof(message));
    }
    
    // Sanitiza a mensagem para remover caracteres inválidos ou perigosos
    /**
 * sanitizeMessage
 * 
 * Implementa a funcionalidade sanitizeMessage conforme especificação original.
 */

    void sanitizeMessage(){
        // Remove caracteres de /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param sizeof(message Parâmetro sizeof(message
 * @return Retorna controle
 */
 controle
        for(size_t i = 0; i < sizeof(message); i++) {
            if (message[i] == 0) break;
            if (message[i] < 32 && message[i] != '\t') {
                message[i] = ' ';
            }
        }
        
        // Garante que a string termine com null
        message[sizeof(message) - 1] = 0;
    }
};

// Pacote de ataque
struct PKT_ATTACK_STRUCT : public Packet<PKT_ATTACK> {
    enum AttackType : BYTE {
        ATTACK_NORMAL = 0,      // Ataque normal
        ATTACK_CRITICAL = 1,    // Acerto crítico
        ATTACK_MISS = 2,        // Erro
        ATTACK_BLOCK = 3,       // Bloqueado
        ATTACK_PERFECT = 4,     // Acerto perfeito
        ATTACK_COUNTER = 5,     // Contra-ataque
        ATTACK_DEADLY = 6,      // Ataque mortal
        ATTACK_SPECIAL = 7      // Ataque especial
    };
    
    struct {
        // Os 8 primeiros bytes são o cabeçalho (já definido em Packet)
        DWORD attackerId;       // ID do atacante
        DWORD targetId;         // ID do alvo
        WORD damage;            // Dano causado
        AttackType attackType;  // Tipo de ataque
        BYTE weaponType;        // Tipo de arma
        BYTE skillId;           // ID da habilidade (se aplicável)
        BYTE reservedA[2];      // Padding para alinhamento
    };
    
    PKT_ATTACK_STRUCT() {
        clear();
        size = sizeof(PKT_ATTACK_STRUCT);
        type = PKT_ATTACK;
        attackerId = 0;
        targetId = 0;
        damage = 0;
        attackType = ATTACK_NORMAL;
        weaponType = 0;
        skillId = 0;
        memset(reservedA, 0, sizeof(reservedA));
    }
};

// Pacote de uso de habilidade
struct PKT_SKILLCAST_STRUCT : public Packet<PKT_SKILLCAST> {
    enum SkillResult : BYTE {
        SKILL_SUCCESS = 0,      // Habilidade executada com sucesso
        SKILL_FAIL_MANA = 1,    // Falha por falta de mana
        SKILL_FAIL_LEVEL = 2,   // Falha por nível insuficiente
        SKILL_FAIL_COOLDOWN = 3,// Falha por cooldown
        SKILL_FAIL_TARGET = 4,  // Falha por alvo inválido
        SKILL_FAIL_RANGE = 5,   // Falha por estar fora de alcance
        SKILL_FAIL_STATE = 6,   // Falha por estado inválido
        SKILL_FAIL_ITEM = 7,    // Falha por falta de item requerido
        SKILL_FAIL_UNKNOWN = 8  // Falha desconhecida
    };
    
    struct {
        // Os 8 primeiros bytes são o cabeçalho (já definido em Packet)
        DWORD casterId;         // ID do conjurador
        BYTE skillId;           // ID da habilidade
        BYTE skillLevel;        // Nível da habilidade
        SkillResult result;     // Resultado da conjuração
        BYTE castTime;          // Tempo de conjuração (em décimos de segundo)
        WORD targetX;           // Posição X do alvo
        WORD targetY;           // Posição Y do alvo
        DWORD targetId;         // ID do alvo (0 para posição)
        BYTE effectId;          // ID do efeito visual
        BYTE reservedA[3];      // Padding para alinhamento
    };
    
    PKT_SKILLCAST_STRUCT() {
        clear();
        size = sizeof(PKT_SKILLCAST_STRUCT);
        type = PKT_SKILLCAST;
        casterId = 0;
        skillId = 0;
        skillLevel = 0;
        result = SKILL_FAIL_UNKNOWN;
        castTime = 0;
        targetX = 0;
        targetY = 0;
        targetId = 0;
        effectId = 0;
        memset(reservedA, 0, sizeof(reservedA));
    }
};

// Pacote de soltar/pegar item
struct PKT_ITEMDROP_STRUCT : public Packet<PKT_DROPITEM> {
    struct {
        // Os 8 primeiros bytes são o cabeçalho (já definido em Packet)
        DWORD itemId;           // ID único do item no mundo
        WORD posX;              // Posição X
        WORD posY;              // Posição Y
        WORD itemIndex;         // Índice do item (tipo)
        BYTE itemEffectId;      // ID do efeito do item
        BYTE itemRefine;        // Nível de refinamento
        WORD itemAmount;        // Quantidade
        DWORD dropTime;         // Timestamp do drop
        BYTE visibleFlags;      // Flags de visibilidade (para quem é visível)
        BYTE reservedA[3];      // Padding para alinhamento
    };
    
    PKT_ITEMDROP_STRUCT() {
        clear();
        size = sizeof(PKT_ITEMDROP_STRUCT);
        type = PKT_DROPITEM;
        itemId = 0;
        posX = 0;
        posY = 0;
        itemIndex = 0;
        itemEffectId = 0;
        itemRefine = 0;
        itemAmount = 0;
        dropTime = 0;
        visibleFlags = 0;
        memset(reservedA, 0, sizeof(reservedA));
    }
};

// Pacote de entrada no jogo
struct PKT_ENTERGAME_STRUCT : public Packet<PKT_ENTERGAME> {
    struct {
        // Os 8 primeiros bytes são o cabeçalho (já definido em Packet)
        DWORD accountId;                // ID da conta
        DWORD checksum;                 // Checksum anti-hack
        char username[16];              // Nome de usuário
        char password[16];              // Senha criptografada
        BYTE version[4];                // Versão do cliente [major,minor,patch,build]
        BYTE macAddress[6];             // Endereço MAC
        BYTE secureKey[16];             // Chave de segurança
        BYTE reservedA[2];              // Padding para alinhamento
    };
    
    PKT_ENTERGAME_STRUCT() {
        clear();
        size = sizeof(PKT_ENTERGAME_STRUCT);
        type = PKT_ENTERGAME;
        accountId = 0;
        checksum = 0;
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));
        memset(version, 0, sizeof(version));
        memset(macAddress, 0, sizeof(macAddress));
        memset(secureKey, 0, sizeof(secureKey));
        memset(reservedA, 0, sizeof(reservedA));
    }
};

// Pacote de quest
struct PKT_QUEST_STRUCT : public Packet<PKT_QUEST, 256> {  // Tamanho maior para acomodar dados de quest
    enum QuestAction : BYTE {
        QUEST_ACTION_INFO = 0,          // Informações da quest
        QUEST_ACTION_START = 1,         // Iniciar quest
        QUEST_ACTION_PROGRESS = 2,      // Progresso da quest
        QUEST_ACTION_COMPLETE = 3,      // Completar quest
        QUEST_ACTION_ABANDON = 4,       // Abandonar quest
        QUEST_ACTION_LIST = 5,          // Listar quests
        QUEST_ACTION_AVAILABLE = 6,     // Quests disponíveis
        QUEST_ACTION_REWARD = 7,        // Recompensa de quest
        QUEST_ACTION_TRACK = 8,         // Rastrear quest
        QUEST_ACTION_UNTRACK = 9        // Parar de rastrear quest
    };
    
    enum QuestResult : BYTE {
        QUEST_RESULT_SUCCESS = 0,       // Operação bem-sucedida
        QUEST_RESULT_FAIL_LEVEL = 1,    // Falha por nível insuficiente
        QUEST_RESULT_FAIL_ITEM = 2,     // Falha por falta de item
        QUEST_RESULT_FAIL_STATE = 3,    // Falha por estado inválido
        QUEST_RESULT_FAIL_PREREQ = 4,   // Falha por pré-requisito
        QUEST_RESULT_FAIL_FULL = 5,     // Falha por limite de quests
        QUEST_RESULT_FAIL_UNKNOWN = 6   // Falha desconhecida
    };
    
    struct {
        // Os 8 primeiros bytes são o cabeçalho (já definido em Packet)
        WORD questId;                   // ID da quest
        QuestAction action;             // Ação a ser realizada
        QuestResult result;             // Resultado da ação
        WORD npcId;                     // ID do NPC relacionado
        BYTE objectiveIndex;            // Índice do objetivo
        BYTE objectiveCount;            // Número de objetivos
        DWORD objectiveProgress[8];     // Progresso de cada objetivo
        BYTE questFlags;                // Flags da quest
        BYTE reservedA[3];              // Padding para alinhamento
        BYTE questData[192];            // Dados específicos da quest (tamanho variável)
    };
    
    PKT_QUEST_STRUCT() {
        clear();
        size = sizeof(PKT_QUEST_STRUCT);
        type = PKT_QUEST;
        questId = 0;
        action = QUEST_ACTION_INFO;
        result = QUEST_RESULT_SUCCESS;
        npcId = 0;
        objectiveIndex = 0;
        objectiveCount = 0;
        memset(objectiveProgress, 0, sizeof(objectiveProgress));
        questFlags = 0;
        memset(reservedA, 0, sizeof(reservedA));
        memset(questData, 0, sizeof(questData));
    }
};

// Funções utilitárias para manipulação de pacotes

// Criar um pacote genérico com base no tipo
template<typename T>
/**
 * createPacket
 * 
 * Implementa a funcionalidade createPacket conforme especificação original.
 * @param type Parâmetro type
 * @param 0 Parâmetro 0
 * @return Retorna T
 */

T* createPacket(WORD type, DWORD handle = 0){
    T* packet = /**
 * T
 * 
 * Implementa a funcionalidade T conforme especificação original.
 * @return Retorna new
 */
 new T();
    packet->type = type;
    packet->handle = handle;
    packet->size = sizeof(T);
    return packet;
}

// Validar um pacote genérico
/**
 * validatePacket
 * 
 * Implementa a funcionalidade validatePacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

bool validatePacket(const void* data, size_t size){
    if (!data || size < sizeof(PacketHeader)) {
        return false;
    }
    
    const PacketHeader* header = static_cast<const PacketHeader*>(data);
    
    // Verificações básicas de segurança
    if (header->size < sizeof(PacketHeader) || header->size > MAX_PACKET_SIZE) {
        return false;
    }
    
    if (header->size > size) {
        return false; // O pacote está incompleto
    }
    
    return true;
}

// Calcular checksum de um pacote (proteção contra modificação)
/**
 * calculatePacketChecksum
 * 
 * Implementa a funcionalidade calculatePacketChecksum conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna DWORD
 */

DWORD calculatePacketChecksum(const void* data, size_t size){
    if (!data || size < sizeof(PacketHeader)) {
        return 0;
    }
    
    const BYTE* bytes = static_cast<const BYTE*>(data);
    DWORD checksum = 0;
    
    // Algoritmo simples de checksum (pode ser mais complexo em produção)
    for (size_t i = 0; i < size; i++) {
        checksum = ((checksum << 5) | (checksum >> 27)) ^ bytes[i];
    }
    
    return checksum;
}

} // namespace net
} // namespace wyd

#endif // WYD_PACKETS_H

} // namespace wydbr
