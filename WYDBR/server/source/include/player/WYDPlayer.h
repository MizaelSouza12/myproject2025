/**
 * WYDPlayer.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/player/WYDPlayer.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _WYD_PLAYER_H_
#define _WYD_PLAYER_H_

/**
 * @file WYDPlayer.h
 * @brief Definição da classe de jogador do WYD
 * 
 * Este arquivo contém a definição da classe de jogador do WYDBR,
 * responsável por armazenar e gerenciar os dados de um jogador no servidor,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <deque>

#include "../../../include/GlobalDef.h"
#include "../../../include/TM_Item.h"
#include "../../../include/TM_Skill.h"
#include "../../../include/TM_Mob.h"

namespace wydbr {
namespace tmsrv {
namespace player {

/**
 * @brief Estado do jogador
 */
enum /**
 * Classe WYDPlayerState
 * 
 * Esta classe implementa a funcionalidade WYDPlayerState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PlayerState {
    DISCONNECTED = 0,       // Desconectado
    CONNECTED = 1,          // Conectado
    LOADING = 2,            // Carregando
    CHARACTER_SELECT = 3,   // Seleção de personagem
    ENTERING_WORLD = 4,     // Entrando no mundo
    IN_WORLD = 5,           // No mundo
    DEAD = 6,               // Morto
    GHOST = 7,              // Fantasma
    TELEPORTING = 8,        // Teleportando
    TRADING = 9,            // Comerciando
    DUELING = 10,           // Duelando
    CRAFTING = 11,          // Fabricando
    ENCHANTING = 12,        // Encantando
    REFINING = 13,          // Refinando
    SOCKETING = 14,         // Inserindo socket
    SHOPPING = 15,          // Comprando
    BANKING = 16,           // Usando banco
    MAILING = 17,           // Usando correio
    AUCTIONING = 18,        // Usando leilão
    BATTLEFIELD = 19,       // Em campo de batalha
    EVENT = 20,             // Em evento
    RESTING = 21,           // Descansando
    AFK = 22,               // Ausente
    GM_MODE = 23,           // Modo GM
    INVISIBLE = 24,         // Invisível
    CUTSCENE = 25,          // Em cena de corte
    STUNNED = 26,           // Atordoado
    SILENCED = 27,          // Silenciado
    FROZEN = 28,            // Congelado
    FEARED = 29,            // Atemorizado
    CHARMED = 30,           // Enfeitiçado
    CONFUSED = 31,          // Confuso
    CUSTOM_1 = 32,          // Personalizado 1
    CUSTOM_2 = 33,          // Personalizado 2
    CUSTOM_3 = 34,          // Personalizado 3
    UNKNOWN = 35            // Desconhecido
};

/**
 * @brief Tipo de notificação de jogador
 */
enum /**
 * Classe WYDPlayerNotificationType
 * 
 * Esta classe implementa a funcionalidade WYDPlayerNotificationType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PlayerNotificationType {
    NONE = 0,               // Nenhuma
    INFO = 1,               // Informação
    WARNING = 2,            // Aviso
    ERROR = 3,              // Erro
    CRITICAL = 4,           // Crítico
    ACHIEVEMENT = 5,        // Conquista
    QUEST = 6,              // Missão
    ITEM = 7,               // Item
    GOLD = 8,               // Ouro
    LEVEL_UP = 9,           // Subir de nível
    SKILL = 10,             // Habilidade
    PVP = 11,               // PvP
    GUILD = 12,             // Guilda
    PARTY = 13,             // Grupo
    FRIEND = 14,            // Amigo
    MAIL = 15,              // Correio
    AUCTION = 16,           // Leilão
    TRADE = 17,             // Comércio
    EVENT = 18,             // Evento
    SYSTEM = 19,            // Sistema
    ADMIN = 20,             // Administração
    CUSTOM_1 = 21,          // Personalizado 1
    CUSTOM_2 = 22,          // Personalizado 2
    CUSTOM_3 = 23,          // Personalizado 3
    UNKNOWN = 24            // Desconhecido
};

/**
 * @brief Tipo de log de jogador
 */
enum /**
 * Classe WYDPlayerLogType
 * 
 * Esta classe implementa a funcionalidade WYDPlayerLogType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PlayerLogType {
    NONE = 0,               // Nenhum
    LOGIN = 1,              // Login
    LOGOUT = 2,             // Logout
    MOVE = 3,               // Movimento
    TELEPORT = 4,           // Teleporte
    ATTACK = 5,             // Ataque
    DAMAGE = 6,             // Dano
    HEAL = 7,               // Cura
    DEATH = 8,              // Morte
    RESURRECT = 9,          // Ressurreição
    LEVEL_UP = 10,          // Subir de nível
    CHAT = 11,              // Chat
    WHISPER = 12,           // Sussurro
    ITEM_ACQUIRE = 13,      // Adquirir item
    ITEM_LOSE = 14,         // Perder item
    ITEM_USE = 15,          // Usar item
    ITEM_EQUIP = 16,        // Equipar item
    ITEM_UNEQUIP = 17,      // Desequipar item
    ITEM_DROP = 18,         // Descartar item
    ITEM_PICKUP = 19,       // Pegar item
    ITEM_DESTROY = 20,      // Destruir item
    ITEM_TRADE = 21,        // Comerciar item
    ITEM_SELL = 22,         // Vender item
    ITEM_BUY = 23,          // Comprar item
    ITEM_CRAFT = 24,        // Fabricar item
    ITEM_ENCHANT = 25,      // Encantar item
    ITEM_REFINE = 26,       // Refinar item
    ITEM_SOCKET = 27,       // Socket de item
    SKILL_LEARN = 28,       // Aprender habilidade
    SKILL_USE = 29,         // Usar habilidade
    SKILL_UPGRADE = 30,     // Melhorar habilidade
    QUEST_ACCEPT = 31,      // Aceitar missão
    QUEST_COMPLETE = 32,    // Completar missão
    QUEST_ABANDON = 33,     // Abandonar missão
    QUEST_PROGRESS = 34,    // Progresso de missão
    ACHIEVEMENT_COMPLETE = 35, // Completar conquista
    ACHIEVEMENT_PROGRESS = 36, // Progresso de conquista
    TITLE_UNLOCK = 37,      // Desbloquear título
    TITLE_EQUIP = 38,       // Equipar título
    TITLE_UNEQUIP = 39,     // Desequipar título
    GOLD_GAIN = 40,         // Ganhar ouro
    GOLD_LOSE = 41,         // Perder ouro
    EXP_GAIN = 42,          // Ganhar experiência
    REPUTATION_GAIN = 43,   // Ganhar reputação
    REPUTATION_LOSE = 44,   // Perder reputação
    PARTY_JOIN = 45,        // Entrar em grupo
    PARTY_LEAVE = 46,       // Sair de grupo
    PARTY_INVITE = 47,      // Convidar para grupo
    PARTY_KICK = 48,        // Expulsar de grupo
    GUILD_JOIN = 49,        // Entrar em guilda
    GUILD_LEAVE = 50,       // Sair de guilda
    GUILD_INVITE = 51,      // Convidar para guilda
    GUILD_KICK = 52,        // Expulsar de guilda
    GUILD_PROMOTE = 53,     // Promover em guilda
    GUILD_DEMOTE = 54,      // Rebaixar em guilda
    FRIEND_ADD = 55,        // Adicionar amigo
    FRIEND_REMOVE = 56,     // Remover amigo
    TRADE_START = 57,       // Iniciar comércio
    TRADE_COMPLETE = 58,    // Completar comércio
    TRADE_CANCEL = 59,      // Cancelar comércio
    MAIL_SEND = 60,         // Enviar correio
    MAIL_RECEIVE = 61,      // Receber correio
    MAIL_READ = 62,         // Ler correio
    MAIL_DELETE = 63,       // Excluir correio
    AUCTION_CREATE = 64,    // Criar leilão
    AUCTION_BID = 65,       // Dar lance em leilão
    AUCTION_CANCEL = 66,    // Cancelar leilão
    AUCTION_COMPLETE = 67,  // Completar leilão
    PVP_KILL = 68,          // Matar em PvP
    PVP_DEATH = 69,         // Morrer em PvP
    BATTLEFIELD_JOIN = 70,  // Entrar em campo de batalha
    BATTLEFIELD_LEAVE = 71, // Sair de campo de batalha
    BATTLEFIELD_SCORE = 72, // Pontuar em campo de batalha
    EVENT_JOIN = 73,        // Entrar em evento
    EVENT_LEAVE = 74,       // Sair de evento
    EVENT_SCORE = 75,       // Pontuar em evento
    GUILD_WAR_DECLARE = 76, // Declarar guerra de guilda
    GUILD_WAR_SURRENDER = 77, // Render em guerra de guilda
    GUILD_WAR_VICTORY = 78, // Vencer guerra de guilda
    GUILD_WAR_DEFEAT = 79,  // Perder guerra de guilda
    COMMAND_USE = 80,       // Usar comando
    COMMAND_GM = 81,        // Usar comando de GM
    COMMAND_ADMIN = 82,     // Usar comando de administrador
    DEBUG = 83,             // Depuração
    CUSTOM_1 = 84,          // Personalizado 1
    CUSTOM_2 = 85,          // Personalizado 2
    CUSTOM_3 = 86,          // Personalizado 3
    UNKNOWN = 87            // Desconhecido
};

/**
 * @brief Tipo de local de inventário
 */
enum /**
 * Classe WYDInventoryLocationType
 * 
 * Esta classe implementa a funcionalidade WYDInventoryLocationType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InventoryLocationType {
    NONE = 0,               // Nenhum
    INVENTORY = 1,          // Inventário
    EQUIPMENT = 2,          // Equipamento
    STORAGE = 3,            // Armazenamento
    TRADE = 4,              // Comércio
    SHOP = 5,               // Loja
    MAIL = 6,               // Correio
    AUCTION = 7,            // Leilão
    GUILD = 8,              // Guilda
    CRAFTING = 9,           // Fabricação
    ENCHANTING = 10,        // Encantamento
    REFINING = 11,          // Refinamento
    SOCKETING = 12,         // Socket
    GROUND = 13,            // Chão
    NPC = 14,               // NPC
    QUEST = 15,             // Missão
    CUSTOM_1 = 16,          // Personalizado 1
    CUSTOM_2 = 17,          // Personalizado 2
    CUSTOM_3 = 18,          // Personalizado 3
    UNKNOWN = 19            // Desconhecido
};

/**
 * @brief Tipo de transação de item
 */
enum /**
 * Classe WYDItemTransactionType
 * 
 * Esta classe implementa a funcionalidade WYDItemTransactionType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemTransactionType {
    NONE = 0,               // Nenhum
    ACQUIRE = 1,            // Adquirir
    LOSE = 2,               // Perder
    USE = 3,                // Usar
    EQUIP = 4,              // Equipar
    UNEQUIP = 5,            // Desequipar
    DROP = 6,               // Descartar
    PICKUP = 7,             // Pegar
    DESTROY = 8,            // Destruir
    TRADE = 9,              // Comerciar
    SELL = 10,              // Vender
    BUY = 11,               // Comprar
    CRAFT = 12,             // Fabricar
    ENCHANT = 13,           // Encantar
    REFINE = 14,            // Refinar
    SOCKET = 15,            // Socket
    MOVE = 16,              // Mover
    SPLIT = 17,             // Dividir
    MERGE = 18,             // Juntar
    MAIL = 19,              // Correio
    AUCTION = 20,           // Leilão
    QUEST = 21,             // Missão
    STORAGE_DEPOSIT = 22,   // Depositar em armazenamento
    STORAGE_WITHDRAW = 23,  // Retirar de armazenamento
    GUILD = 24,             // Guilda
    GM = 25,                // GM
    CUSTOM_1 = 26,          // Personalizado 1
    CUSTOM_2 = 27,          // Personalizado 2
    CUSTOM_3 = 28,          // Personalizado 3
    UNKNOWN = 29            // Desconhecido
};

/**
 * @brief Tipo de slot de equipamento
 */
enum /**
 * Classe WYDEquipmentSlotType
 * 
 * Esta classe implementa a funcionalidade WYDEquipmentSlotType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EquipmentSlotType {
    NONE = -1,              // Nenhum
    HEAD = 0,               // Cabeça
    NECK = 1,               // Pescoço
    SHOULDERS = 2,          // Ombros
    CHEST = 3,              // Peito
    WAIST = 4,              // Cintura
    LEGS = 5,               // Pernas
    FEET = 6,               // Pés
    WRISTS = 7,             // Pulsos
    HANDS = 8,              // Mãos
    FINGER_1 = 9,           // Dedo 1
    FINGER_2 = 10,          // Dedo 2
    TRINKET_1 = 11,         // Berloque 1
    TRINKET_2 = 12,         // Berloque 2
    BACK = 13,              // Costas
    MAIN_HAND = 14,         // Mão principal
    OFF_HAND = 15,          // Mão secundária
    RANGED = 16,            // Longo alcance
    AMMO = 17,              // Munição
    TABARD = 18,            // Tabardo
    MOUNT = 19,             // Montaria
    MOUNT_ARMOR = 20,       // Armadura de montaria
    MOUNT_SADDLE = 21,      // Sela de montaria
    MOUNT_ACCESSORY = 22,   // Acessório de montaria
    ARTIFACT = 23,          // Artefato
    COSMETIC_HEAD = 24,     // Cabeça cosmética
    COSMETIC_SHOULDERS = 25, // Ombros cosméticos
    COSMETIC_CHEST = 26,    // Peito cosmético
    COSMETIC_WAIST = 27,    // Cintura cosmética
    COSMETIC_LEGS = 28,     // Pernas cosméticas
    COSMETIC_FEET = 29,     // Pés cosméticos
    COSMETIC_WRISTS = 30,   // Pulsos cosméticos
    COSMETIC_HANDS = 31,    // Mãos cosméticas
    COSMETIC_BACK = 32,     // Costas cosméticas
    CUSTOM_1 = 33,          // Personalizado 1
    CUSTOM_2 = 34,          // Personalizado 2
    CUSTOM_3 = 35,          // Personalizado 3
    MAX_SLOTS = 36          // Número máximo de slots
};

/**
 * @brief Informações de log de um jogador
 */
struct PlayerLogEntry {
    uint32_t timestamp;            // Timestamp
    PlayerLogType type;            // Tipo de log
    std::string message;           // Mensagem
    void* data;                    // Dados adicionais
    uint32_t dataSize;             // Tamanho dos dados
    
    PlayerLogEntry() : timestamp(0), type(PlayerLogType::NONE), data(nullptr), dataSize(0) {}
    
    PlayerLogEntry(PlayerLogType _type, const std::string& _message, void* _data = nullptr, uint32_t _dataSize = 0)
        : timestamp(static_cast<uint32_t>(time(nullptr))), type(_type), message(_message),
          data(_data), dataSize(_dataSize) {}
};

/**
 * @brief Informações de transação de item de um jogador
 */
struct ItemTransactionEntry {
    uint32_t timestamp;            // Timestamp
    ItemTransactionType type;      // Tipo de transação
    uint16_t itemId;               // ID do item
    uint32_t count;                // Quantidade
    InventoryLocationType sourceLocation; // Local de origem
    uint8_t sourceSlot;            // Slot de origem
    InventoryLocationType targetLocation; // Local de destino
    uint8_t targetSlot;            // Slot de destino
    std::string note;              // Observação
    
    ItemTransactionEntry() : timestamp(0), type(ItemTransactionType::NONE), itemId(0),
        count(0), sourceLocation(InventoryLocationType::NONE), sourceSlot(0),
        targetLocation(InventoryLocationType::NONE), targetSlot(0) {}
    
    ItemTransactionEntry(ItemTransactionType _type, uint16_t _itemId, uint32_t _count,
                      InventoryLocationType _sourceLocation = InventoryLocationType::NONE,
                      uint8_t _sourceSlot = 0,
                      InventoryLocationType _targetLocation = InventoryLocationType::NONE,
                      uint8_t _targetSlot = 0,
                      const std::string& _note = "")
        : timestamp(static_cast<uint32_t>(time(nullptr))), type(_type), itemId(_itemId),
          count(_count), sourceLocation(_sourceLocation), sourceSlot(_sourceSlot),
          targetLocation(_targetLocation), targetSlot(_targetSlot), note(_note) {}
};

/**
 * @brief Notificação de um jogador
 */
struct PlayerNotification {
    uint32_t id;                    // ID da notificação
    uint32_t timestamp;             // Timestamp
    PlayerNotificationType type;    // Tipo de notificação
    std::string title;              // Título
    std::string message;            // Mensagem
    uint32_t duration;              // Duração (ms)
    bool read;                      // Se foi lida
    bool autoHide;                  // Se esconde automaticamente
    
    PlayerNotification() : id(0), timestamp(0), type(PlayerNotificationType::NONE),
        duration(0), read(false), autoHide(true) {}
    
    PlayerNotification(uint32_t _id, PlayerNotificationType _type, const std::string& _title,
                     const std::string& _message, uint32_t _duration = 5000,
                     bool _autoHide = true)
        : id(_id), timestamp(static_cast<uint32_t>(time(nullptr))), type(_type),
          title(_title), message(_message), duration(_duration),
          read(false), autoHide(_autoHide) {}
};

/**
 * @brief Jogador do WYD
 * 
 * Esta classe é responsável por armazenar e gerenciar os dados de um jogador no sistema WYDBR,
 * incluindo informações de personagem, inventário, habilidades, entre outros.
 */
/**
 * Classe WYDPlayer
 * 
 * Esta classe implementa a funcionalidade WYDPlayer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDPlayer {
public:
    /**
     * @brief Construtor
     * @param playerId ID do jogador
     * @param clientId ID do cliente
     * @param accountId ID da conta
     * @param name Nome do jogador
     */
    WYDPlayer(PlayerId playerId, ClientId clientId, AccountId accountId, const std::string& name);
    
    /**
     * @brief Destrutor
     */
    ~WYDPlayer();
    
    /**
     * @brief Inicializa o jogador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o jogador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o jogador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega os dados do jogador
     * @return true se carregado com sucesso
     */
    bool load();
    
    /**
     * @brief Salva os dados do jogador
     * @return true se salvo com sucesso
     */
    bool save();
    
    /**
     * @brief Conecta o jogador
     * @param clientId ID do cliente
     * @return true se conectado com sucesso
     */
    bool connect(ClientId clientId);
    
    /**
     * @brief Desconecta o jogador
     * @param reason Motivo da desconexão
     * @return true se desconectado com sucesso
     */
    bool disconnect(const std::string& reason = "");
    
    /**
     * @brief Inicializa o jogador
     * @param mob Dados do mob/personagem
     * @return true se inicializado com sucesso
     */
    bool init(const STRUCT_MOB& mob);
    
    /**
     * @brief Envia um pacote para o jogador
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se enviado com sucesso
     */
    bool sendPacket(uint16_t opcode, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Envia uma mensagem para o jogador
     * @param message Mensagem
     * @param type Tipo de mensagem
     * @return true se enviada com sucesso
     */
    bool sendMessage(const std::string& message, uint8_t type = 0);
    
    /**
     * @brief Envia uma notificação para o jogador
     * @param type Tipo de notificação
     * @param title Título
     * @param message Mensagem
     * @param duration Duração
     * @param autoHide Se esconde automaticamente
     * @return ID da notificação ou 0 em caso de erro
     */
    uint32_t sendNotification(PlayerNotificationType type, const std::string& title,
                           const std::string& message, uint32_t duration = 5000,
                           bool autoHide = true);
    
    /**
     * @brief Define o estado do jogador
     * @param state Estado
     */
    void setState(PlayerState state);
    
    /**
     * @brief Obtém o estado do jogador
     * @return Estado
     */
    PlayerState getState() const;
    
    /**
     * @brief Verifica se o jogador está conectado
     * @return true se estiver conectado
     */
    bool isConnected() const;
    
    /**
     * @brief Verifica se o jogador está online
     * @return true se estiver online
     */
    bool isOnline() const;
    
    /**
     * @brief Verifica se o jogador está no mundo
     * @return true se estiver no mundo
     */
    bool isInWorld() const;
    
    /**
     * @brief Verifica se o jogador está vivo
     * @return true se estiver vivo
     */
    bool isAlive() const;
    
    /**
     * @brief Verifica se o jogador está morto
     * @return true se estiver morto
     */
    bool isDead() const;
    
    /**
     * @brief Verifica se o jogador está em combate
     * @return true se estiver em combate
     */
    bool isInCombat() const;
    
    /**
     * @brief Verifica se o jogador está em um grupo
     * @return true se estiver em um grupo
     */
    bool isInParty() const;
    
    /**
     * @brief Verifica se o jogador está em uma guilda
     * @return true se estiver em uma guilda
     */
    bool isInGuild() const;
    
    /**
     * @brief Verifica se o jogador está comerciando
     * @return true se estiver comerciando
     */
    bool isTrading() const;
    
    /**
     * @brief Verifica se o jogador está duelando
     * @return true se estiver duelando
     */
    bool isDueling() const;
    
    /**
     * @brief Verifica se o jogador está em um campo de batalha
     * @return true se estiver em um campo de batalha
     */
    bool isInBattlefield() const;
    
    /**
     * @brief Verifica se o jogador está em um evento
     * @return true se estiver em um evento
     */
    bool isInEvent() const;
    
    /**
     * @brief Verifica se o jogador está ausente
     * @return true se estiver ausente
     */
    bool isAFK() const;
    
    /**
     * @brief Verifica se o jogador é um GM
     * @return true se for um GM
     */
    bool isGM() const;
    
    /**
     * @brief Verifica se o jogador é invisível
     * @return true se for invisível
     */
    bool isInvisible() const;
    
    /**
     * @brief Define o ID do jogador
     * @param playerId ID do jogador
     */
    void setPlayerId(PlayerId playerId);
    
    /**
     * @brief Obtém o ID do jogador
     * @return ID do jogador
     */
    PlayerId getPlayerId() const;
    
    /**
     * @brief Define o ID do cliente
     * @param clientId ID do cliente
     */
    void setClientId(ClientId clientId);
    
    /**
     * @brief Obtém o ID do cliente
     * @return ID do cliente
     */
    ClientId getClientId() const;
    
    /**
     * @brief Define o ID da conta
     * @param accountId ID da conta
     */
    void setAccountId(AccountId accountId);
    
    /**
     * @brief Obtém o ID da conta
     * @return ID da conta
     */
    AccountId getAccountId() const;
    
    /**
     * @brief Define o nome do jogador
     * @param name Nome do jogador
     * @return true se definido com sucesso
     */
    bool setName(const std::string& name);
    
    /**
     * @brief Obtém o nome do jogador
     * @return Nome do jogador
     */
    const std::string& getName() const;
    
    /**
     * @brief Define o apelido do jogador
     * @param nickname Apelido do jogador
     * @return true se definido com sucesso
     */
    bool setNickname(const std::string& nickname);
    
    /**
     * @brief Obtém o apelido do jogador
     * @return Apelido do jogador
     */
    const std::string& getNickname() const;
    
    /**
     * @brief Define a classe do jogador
     * @param classId ID da classe
     */
    void setClass(uint8_t classId);
    
    /**
     * @brief Obtém a classe do jogador
     * @return ID da classe
     */
    uint8_t getClass() const;
    
    /**
     * @brief Define o nível do jogador
     * @param level Nível
     */
    void setLevel(uint8_t level);
    
    /**
     * @brief Obtém o nível do jogador
     * @return Nível
     */
    uint8_t getLevel() const;
    
    /**
     * @brief Define a experiência do jogador
     * @param exp Experiência
     */
    void setExp(uint32_t exp);
    
    /**
     * @brief Obtém a experiência do jogador
     * @return Experiência
     */
    uint32_t getExp() const;
    
    /**
     * @brief Define o ouro do jogador
     * @param gold Ouro
     */
    void setGold(int32_t gold);
    
    /**
     * @brief Obtém o ouro do jogador
     * @return Ouro
     */
    int32_t getGold() const;
    
    /**
     * @brief Adiciona ouro ao jogador
     * @param amount Quantidade
     * @param reason Motivo
     * @return true se adicionado com sucesso
     */
    bool addGold(int32_t amount, const std::string& reason = "");
    
    /**
     * @brief Remove ouro do jogador
     * @param amount Quantidade
     * @param reason Motivo
     * @return true se removido com sucesso
     */
    bool removeGold(int32_t amount, const std::string& reason = "");
    
    /**
     * @brief Define o mapa do jogador
     * @param mapId ID do mapa
     */
    void setMap(MapId mapId);
    
    /**
     * @brief Obtém o mapa do jogador
     * @return ID do mapa
     */
    MapId getMap() const;
    
    /**
     * @brief Define a posição X do jogador
     * @param x Posição X
     */
    void setX(int16_t x);
    
    /**
     * @brief Obtém a posição X do jogador
     * @return Posição X
     */
    int16_t getX() const;
    
    /**
     * @brief Define a posição Y do jogador
     * @param y Posição Y
     */
    void setY(int16_t y);
    
    /**
     * @brief Obtém a posição Y do jogador
     * @return Posição Y
     */
    int16_t getY() const;
    
    /**
     * @brief Define a direção do jogador
     * @param direction Direção
     */
    void setDirection(uint8_t direction);
    
    /**
     * @brief Obtém a direção do jogador
     * @return Direção
     */
    uint8_t getDirection() const;
    
    /**
     * @brief Define os pontos de vida do jogador
     * @param hp Pontos de vida
     */
    void setHP(int32_t hp);
    
    /**
     * @brief Obtém os pontos de vida do jogador
     * @return Pontos de vida
     */
    int32_t getHP() const;
    
    /**
     * @brief Define os pontos de vida máximos do jogador
     * @param maxHP Pontos de vida máximos
     */
    void setMaxHP(int32_t maxHP);
    
    /**
     * @brief Obtém os pontos de vida máximos do jogador
     * @return Pontos de vida máximos
     */
    int32_t getMaxHP() const;
    
    /**
     * @brief Define os pontos de mana do jogador
     * @param mp Pontos de mana
     */
    void setMP(int32_t mp);
    
    /**
     * @brief Obtém os pontos de mana do jogador
     * @return Pontos de mana
     */
    int32_t getMP() const;
    
    /**
     * @brief Define os pontos de mana máximos do jogador
     * @param maxMP Pontos de mana máximos
     */
    void setMaxMP(int32_t maxMP);
    
    /**
     * @brief Obtém os pontos de mana máximos do jogador
     * @return Pontos de mana máximos
     */
    int32_t getMaxMP() const;
    
    /**
     * @brief Define a força do jogador
     * @param str Força
     */
    void setSTR(int16_t str);
    
    /**
     * @brief Obtém a força do jogador
     * @return Força
     */
    int16_t getSTR() const;
    
    /**
     * @brief Define a inteligência do jogador
     * @param int_ Inteligência
     */
    void setINT(int16_t int_);
    
    /**
     * @brief Obtém a inteligência do jogador
     * @return Inteligência
     */
    int16_t getINT() const;
    
    /**
     * @brief Define a destreza do jogador
     * @param dex Destreza
     */
    void setDEX(int16_t dex);
    
    /**
     * @brief Obtém a destreza do jogador
     * @return Destreza
     */
    int16_t getDEX() const;
    
    /**
     * @brief Define a constituição do jogador
     * @param con Constituição
     */
    void setCON(int16_t con);
    
    /**
     * @brief Obtém a constituição do jogador
     * @return Constituição
     */
    int16_t getCON() const;
    
    /**
     * @brief Define a defesa do jogador
     * @param ac Defesa
     */
    void setAC(int16_t ac);
    
    /**
     * @brief Obtém a defesa do jogador
     * @return Defesa
     */
    int16_t getAC() const;
    
    /**
     * @brief Define o dano do jogador
     * @param damage Dano
     */
    void setDamage(int16_t damage);
    
    /**
     * @brief Obtém o dano do jogador
     * @return Dano
     */
    int16_t getDamage() const;
    
    /**
     * @brief Define o ID do grupo do jogador
     * @param partyId ID do grupo
     */
    void setPartyId(PartyId partyId);
    
    /**
     * @brief Obtém o ID do grupo do jogador
     * @return ID do grupo
     */
    PartyId getPartyId() const;
    
    /**
     * @brief Define o ID da guilda do jogador
     * @param guildId ID da guilda
     */
    void setGuildId(GuildId guildId);
    
    /**
     * @brief Obtém o ID da guilda do jogador
     * @return ID da guilda
     */
    GuildId getGuildId() const;
    
    /**
     * @brief Define o tempo da última atualização do jogador
     * @param lastUpdateTime Tempo da última atualização
     */
    void setLastUpdateTime(uint32_t lastUpdateTime);
    
    /**
     * @brief Obtém o tempo da última atualização do jogador
     * @return Tempo da última atualização
     */
    uint32_t getLastUpdateTime() const;
    
    /**
     * @brief Define o tempo do último login do jogador
     * @param lastLoginTime Tempo do último login
     */
    void setLastLoginTime(uint32_t lastLoginTime);
    
    /**
     * @brief Obtém o tempo do último login do jogador
     * @return Tempo do último login
     */
    uint32_t getLastLoginTime() const;
    
    /**
     * @brief Define o tempo do último logout do jogador
     * @param lastLogoutTime Tempo do último logout
     */
    void setLastLogoutTime(uint32_t lastLogoutTime);
    
    /**
     * @brief Obtém o tempo do último logout do jogador
     * @return Tempo do último logout
     */
    uint32_t getLastLogoutTime() const;
    
    /**
     * @brief Define o tempo de jogo do jogador
     * @param playTime Tempo de jogo
     */
    void setPlayTime(uint32_t playTime);
    
    /**
     * @brief Obtém o tempo de jogo do jogador
     * @return Tempo de jogo
     */
    uint32_t getPlayTime() const;
    
    /**
     * @brief Define o tempo da última ação do jogador
     * @param lastActionTime Tempo da última ação
     */
    void setLastActionTime(uint32_t lastActionTime);
    
    /**
     * @brief Obtém o tempo da última ação do jogador
     * @return Tempo da última ação
     */
    uint32_t getLastActionTime() const;
    
    /**
     * @brief Define o último comandon utilizado pelo jogador
     * @param lastCommand Último comando
     */
    void setLastCommand(const std::string& lastCommand);
    
    /**
     * @brief Obtém o último comando utilizado pelo jogador
     * @return Último comando
     */
    const std::string& getLastCommand() const;
    
    /**
     * @brief Define o tempo da última mensagem do jogador
     * @param lastChatTime Tempo da última mensagem
     */
    void setLastChatTime(uint32_t lastChatTime);
    
    /**
     * @brief Obtém o tempo da última mensagem do jogador
     * @return Tempo da última mensagem
     */
    uint32_t getLastChatTime() const;
    
    /**
     * @brief Define se o jogador está silenciado
     * @param muted Silenciado
     */
    void setMuted(bool muted);
    
    /**
     * @brief Verifica se o jogador está silenciado
     * @return true se estiver silenciado
     */
    bool isMuted() const;
    
    /**
     * @brief Define se o jogador está banido
     * @param banned Banido
     * @param reason Motivo
     * @param duration Duração
     */
    void setBanned(bool banned, const std::string& reason = "", uint32_t duration = 0);
    
    /**
     * @brief Verifica se o jogador está banido
     * @return true se estiver banido
     */
    bool isBanned() const;
    
    /**
     * @brief Define um item no inventário
     * @param slot Slot
     * @param item Item
     * @return true se definido com sucesso
     */
    bool setInventoryItem(uint8_t slot, const STRUCT_ITEM& item);
    
    /**
     * @brief Obtém um item do inventário
     * @param slot Slot
     * @return Item
     */
    const STRUCT_ITEM& getInventoryItem(uint8_t slot) const;
    
    /**
     * @brief Define um item no equipamento
     * @param slot Slot
     * @param item Item
     * @return true se definido com sucesso
     */
    bool setEquipmentItem(EquipmentSlotType slot, const STRUCT_ITEM& item);
    
    /**
     * @brief Obtém um item do equipamento
     * @param slot Slot
     * @return Item
     */
    const STRUCT_ITEM& getEquipmentItem(EquipmentSlotType slot) const;
    
    /**
     * @brief Adiciona um item ao inventário
     * @param item Item
     * @param count Quantidade
     * @param reason Motivo
     * @return Slot ou -1 em caso de erro
     */
    int8_t addItem(const STRUCT_ITEM& item, uint32_t count = 1, const std::string& reason = "");
    
    /**
     * @brief Remove um item do inventário
     * @param slot Slot
     * @param count Quantidade
     * @param reason Motivo
     * @return true se removido com sucesso
     */
    bool removeItem(uint8_t slot, uint32_t count = 1, const std::string& reason = "");
    
    /**
     * @brief Remove um item pelo ID
     * @param itemId ID do item
     * @param count Quantidade
     * @param reason Motivo
     * @return Quantidade removida
     */
    uint32_t removeItemById(uint16_t itemId, uint32_t count = 1, const std::string& reason = "");
    
    /**
     * @brief Usa um item
     * @param slot Slot
     * @return true se usado com sucesso
     */
    bool useItem(uint8_t slot);
    
    /**
     * @brief Equipa um item
     * @param slot Slot
     * @return true se equipado com sucesso
     */
    bool equipItem(uint8_t slot);
    
    /**
     * @brief Desequipa um item
     * @param slot Slot
     * @return true se desequipado com sucesso
     */
    bool unequipItem(EquipmentSlotType slot);
    
    /**
     * @brief Move um item
     * @param sourceSlot Slot de origem
     * @param targetSlot Slot de destino
     * @return true se movido com sucesso
     */
    bool moveItem(uint8_t sourceSlot, uint8_t targetSlot);
    
    /**
     * @brief Divide um item
     * @param sourceSlot Slot de origem
     * @param targetSlot Slot de destino
     * @param count Quantidade
     * @return true se dividido com sucesso
     */
    bool splitItem(uint8_t sourceSlot, uint8_t targetSlot, uint32_t count);
    
    /**
     * @brief Junta itens
     * @param sourceSlot Slot de origem
     * @param targetSlot Slot de destino
     * @return true se juntado com sucesso
     */
    bool mergeItem(uint8_t sourceSlot, uint8_t targetSlot);
    
    /**
     * @brief Define uma habilidade
     * @param slot Slot
     * @param skillId ID da habilidade
     * @param level Nível
     * @return true se definido com sucesso
     */
    bool setSkill(uint8_t slot, uint16_t skillId, uint8_t level);
    
    /**
     * @brief Obtém uma habilidade
     * @param slot Slot
     * @param skillId ID da habilidade (saída)
     * @param level Nível (saída)
     * @return true se obtido com sucesso
     */
    bool getSkill(uint8_t slot, uint16_t& skillId, uint8_t& level) const;
    
    /**
     * @brief Aprende uma habilidade
     * @param skillId ID da habilidade
     * @param level Nível
     * @return true se aprendido com sucesso
     */
    bool learnSkill(uint16_t skillId, uint8_t level = 1);
    
    /**
     * @brief Melhora uma habilidade
     * @param slot Slot
     * @param levels Níveis
     * @return true se melhorado com sucesso
     */
    bool upgradeSkill(uint8_t slot, uint8_t levels = 1);
    
    /**
     * @brief Usa uma habilidade
     * @param slot Slot
     * @param targetId ID do alvo
     * @param x Posição X
     * @param y Posição Y
     * @return true se usado com sucesso
     */
    bool useSkill(uint8_t slot, EntityId targetId = 0, int16_t x = -1, int16_t y = -1);
    
    /**
     * @brief Obtém a estrutura MOB do jogador
     * @return Estrutura MOB
     */
    const STRUCT_MOB& getMob() const;
    
    /**
     * @brief Define a estrutura MOB do jogador
     * @param mob Estrutura MOB
     */
    void setMob(const STRUCT_MOB& mob);
    
    /**
     * @brief Registra um log
     * @param type Tipo de log
     * @param message Mensagem
     * @param data Dados adicionais
     * @param dataSize Tamanho dos dados
     */
    void logEntry(PlayerLogType type, const std::string& message, void* data = nullptr, uint32_t dataSize = 0);
    
    /**
     * @brief Registra uma transação de item
     * @param type Tipo de transação
     * @param itemId ID do item
     * @param count Quantidade
     * @param sourceLocation Local de origem
     * @param sourceSlot Slot de origem
     * @param targetLocation Local de destino
     * @param targetSlot Slot de destino
     * @param note Observação
     */
    void logItemTransaction(ItemTransactionType type, uint16_t itemId, uint32_t count,
                          InventoryLocationType sourceLocation = InventoryLocationType::NONE,
                          uint8_t sourceSlot = 0,
                          InventoryLocationType targetLocation = InventoryLocationType::NONE,
                          uint8_t targetSlot = 0,
                          const std::string& note = "");
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
    /**
     * @brief Define o modo de log
     * @param logging Modo de log
     */
    void setLoggingEnabled(bool logging);
    
    /**
     * @brief Verifica se o modo de log está ativado
     * @return true se estiver ativado
     */
    bool isLoggingEnabled() const;
    
    /**
     * @brief Processa um pacote recebido
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processPacket(uint16_t opcode, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Método de serialização para salvar o jogador
     * @param data Buffer para os dados
     * @param size Tamanho do buffer
     * @return Tamanho dos dados serializados
     */
    uint32_t serialize(uint8_t* data, uint32_t size) const;
    
    /**
     * @brief Método de desserialização para carregar o jogador
     * @param data Dados serializados
     * @param size Tamanho dos dados
     * @return true se desserializado com sucesso
     */
    bool deserialize(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Teleporta o jogador
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @return true se teleportado com sucesso
     */
    bool teleport(MapId mapId, int16_t x, int16_t y);
    
    /**
     * @brief Causa dano ao jogador
     * @param damage Dano
     * @param attackerId ID do atacante
     * @param damageType Tipo de dano
     * @return true se o dano foi aplicado
     */
    bool takeDamage(int32_t damage, EntityId attackerId = 0, uint8_t damageType = 0);
    
    /**
     * @brief Cura o jogador
     * @param healing Cura
     * @param healerId ID do curador
     * @param healType Tipo de cura
     * @return true se a cura foi aplicada
     */
    bool heal(int32_t healing, EntityId healerId = 0, uint8_t healType = 0);
    
    /**
     * @brief Mata o jogador
     * @param killerId ID do assassino
     * @param damageType Tipo de dano
     * @return true se o jogador foi morto
     */
    bool kill(EntityId killerId = 0, uint8_t damageType = 0);
    
    /**
     * @brief Ressuscita o jogador
     * @param resurrectorId ID do ressuscitador
     * @param resurrectType Tipo de ressurreição
     * @return true se o jogador foi ressuscitado
     */
    bool resurrect(EntityId resurrectorId = 0, uint8_t resurrectType = 0);
    
    /**
     * @brief Adiciona experiência ao jogador
     * @param exp Experiência
     * @param reason Motivo
     * @return true se a experiência foi adicionada
     */
    bool addExp(uint32_t exp, const std::string& reason = "");
    
    /**
     * @brief Sobe de nível
     * @param levels Níveis
     * @return true se o jogador subiu de nível
     */
    bool levelUp(uint8_t levels = 1);
    
private:
    // Dados básicos
    PlayerId _playerId;            // ID do jogador
    ClientId _clientId;            // ID do cliente
    AccountId _accountId;          // ID da conta
    std::string _name;             // Nome do jogador
    std::string _nickname;         // Apelido do jogador
    
    // Estado do jogador
    std::atomic<PlayerState> _state; // Estado
    
    // Dados do personagem
    STRUCT_MOB _mob;               // Dados do mob/personagem
    
    // Inventário e equipamentos
    STRUCT_ITEM _inventory[MAX_INVENTORY_SLOTS]; // Inventário
    STRUCT_ITEM _equipment[static_cast<size_t>(EquipmentSlotType::MAX_SLOTS)]; // Equipamento
    
    // Habilidades
    struct {
        uint16_t id;               // ID da habilidade
        uint8_t level;             // Nível
    } _skills[MAX_SKILL_SLOTS];    // Habilidades
    
    // Logs e notificações
    std::deque<PlayerLogEntry> _logs; // Logs
    std::deque<ItemTransactionEntry> _itemTransactions; // Transações de item
    std::unordered_map<uint32_t, PlayerNotification> _notifications; // Notificações
    uint32_t _nextNotificationId;  // Próximo ID de notificação
    
    // Timers e contadores
    uint32_t _updateTimer;         // Timer de atualização
    uint32_t _saveTimer;           // Timer de salvamento
    uint32_t _autoSaveTimer;       // Timer de salvamento automático
    uint32_t _playTime;            // Tempo de jogo
    uint32_t _lastUpdateTime;      // Tempo da última atualização
    uint32_t _lastLoginTime;       // Tempo do último login
    uint32_t _lastLogoutTime;      // Tempo do último logout
    uint32_t _lastActionTime;      // Tempo da última ação
    uint32_t _lastChatTime;        // Tempo da última mensagem
    uint32_t _combatTimer;         // Timer de combate
    
    // Outros dados
    uint8_t _direction;            // Direção
    PartyId _partyId;              // ID do grupo
    GuildId _guildId;              // ID da guilda
    std::string _lastCommand;      // Último comando
    uint32_t _nextAttackTime;      // Tempo do próximo ataque
    
    // Flags
    std::atomic<bool> _initialized; // Inicializado
    std::atomic<bool> _loaded;     // Carregado
    std::atomic<bool> _connected;  // Conectado
    std::atomic<bool> _inWorld;    // No mundo
    std::atomic<bool> _alive;      // Vivo
    std::atomic<bool> _inCombat;   // Em combate
    std::atomic<bool> _muted;      // Silenciado
    std::atomic<bool> _banned;     // Banido
    std::atomic<bool> _debugMode;  // Modo de debug
    std::atomic<bool> _loggingEnabled; // Modo de log
    
    // Informações de banimento
    std::string _banReason;        // Motivo do banimento
    uint32_t _banTime;             // Tempo do banimento
    uint32_t _banDuration;         // Duração do banimento
    
    // Thread safety
    mutable std::mutex _mutex;     // Mutex
    
    // Métodos internos
    /**
     * @brief Atualiza os atributos do jogador
     */
    void updateAttributes();
    
    /**
     * @brief Envia os dados do jogador para o cliente
     * @return true se enviado com sucesso
     */
    bool sendPlayerData();
    
    /**
     * @brief Envia o inventário para o cliente
     * @return true se enviado com sucesso
     */
    bool sendInventory();
    
    /**
     * @brief Envia o equipamento para o cliente
     * @return true se enviado com sucesso
     */
    bool sendEquipment();
    
    /**
     * @brief Envia as habilidades para o cliente
     * @return true se enviado com sucesso
     */
    bool sendSkills();
    
    /**
     * @brief Verifica se um item pode ser equipado
     * @param item Item
     * @param slot Slot
     * @return true se puder ser equipado
     */
    bool canEquipItem(const STRUCT_ITEM& item, EquipmentSlotType slot) const;
    
    /**
     * @brief Obtém o slot de equipamento para um item
     * @param item Item
     * @return Slot de equipamento
     */
    EquipmentSlotType getEquipmentSlotForItem(const STRUCT_ITEM& item) const;
    
    /**
     * @brief Atualiza o timer de combate
     */
    void updateCombatTimer();
    
    /**
     * @brief Limpa os logs antigos
     */
    void cleanOldLogs();
    
    /**
     * @brief Prepara um pacote para envio
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param packet Buffer para o pacote
     * @param packetSize Tamanho do buffer
     * @return Tamanho do pacote preparado
     */
    uint32_t preparePacket(uint16_t opcode, const uint8_t* data, uint32_t size,
                         uint8_t* packet, uint32_t packetSize) const;
    
    /**
     * @brief Envia os dados para o cliente
     * @param data Dados
     * @param size Tamanho dos dados
     * @return true se enviado com sucesso
     */
    bool sendToClient(const uint8_t* data, uint32_t size) const;
    
    /**
     * @brief Processa um pacote de movimento
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processMovePacket(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Processa um pacote de chat
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processChatPacket(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Processa um pacote de ataque
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processAttackPacket(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Processa um pacote de uso de habilidade
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processSkillUsePacket(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Processa um pacote de uso de item
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processItemUsePacket(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Processa um pacote de movimento de item
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processItemMovePacket(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Processa um pacote de equipamento de item
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processItemEquipPacket(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Processa um pacote de desequipamento de item
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se processado com sucesso
     */
    bool processItemUnequipPacket(const uint8_t* data, uint32_t size);
    
    /**
     * @brief Verifica se o jogador pode se mover
     * @return true se puder se mover
     */
    bool canMove() const;
    
    /**
     * @brief Verifica se o jogador pode atacar
     * @return true se puder atacar
     */
    bool canAttack() const;
    
    /**
     * @brief Verifica se o jogador pode usar habilidades
     * @return true se puder usar habilidades
     */
    bool canUseSkill() const;
    
    /**
     * @brief Verifica se o jogador pode usar itens
     * @return true se puder usar itens
     */
    bool canUseItem() const;
    
    /**
     * @brief Verifica se o jogador pode conversar
     * @return true se puder conversar
     */
    bool canChat() const;
};

} // namespace player
} // namespace tmsrv
} // namespace wydbr

#endif // _WYD_PLAYER_H_