/**
 * PlayerManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/tmsrv/PlayerManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _PLAYER_MANAGER_H_
#define _PLAYER_MANAGER_H_

/**
 * @file PlayerManager.h
 * @brief Gerenciador de jogadores do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de jogadores do WYDBR,
 * responsável por criar, gerenciar e controlar os jogadores no servidor,
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
#include <functional>

#include "../GlobalDef.h"
#include "../TM_Mob.h"
#include "../TM_Item.h"

namespace wydbr {
namespace tmsrv {

// Typedef para IDs
typedef uint32_t PlayerId;
typedef uint32_t ClientId;
typedef uint32_t AccountId;

/**
 * @brief Constantes para o PlayerManager
 */
const uint32_t MAX_PLAYERS = 1000;
const uint32_t AUTO_SAVE_INTERVAL = 300; // 5 minutos
const uint32_t INACTIVITY_CHECK_INTERVAL = 60; // 1 minuto
const uint32_t INACTIVITY_TIME = 3600; // 1 hora

/**
 * @brief Tipos de evento de gerenciamento de jogadores
 */
enum /**
 * Classe WYDPlayerManagerEventType
 * 
 * Esta classe implementa a funcionalidade WYDPlayerManagerEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PlayerManagerEventType {
    PLAYER_CREATED = 0,            // Jogador criado
    PLAYER_DESTROYED = 1,          // Jogador destruído
    PLAYER_CONNECTED = 2,          // Jogador conectado
    PLAYER_DISCONNECTED = 3,       // Jogador desconectado
    PLAYER_LOADED = 4,             // Jogador carregado
    PLAYER_SAVED = 5,              // Jogador salvo
    PLAYER_STATE_CHANGED = 6,      // Estado do jogador alterado
    PLAYER_ATTRIBUTE_CHANGED = 7,  // Atributo do jogador alterado
    PLAYER_ITEM_CHANGED = 8,       // Item do jogador alterado
    PLAYER_SKILL_CHANGED = 9,      // Habilidade do jogador alterada
    PLAYER_QUEST_CHANGED = 10,     // Missão do jogador alterada
    PLAYER_ACHIEVEMENT_CHANGED = 11, // Conquista do jogador alterada
    PLAYER_TITLE_CHANGED = 12,     // Título do jogador alterado
    PLAYER_PARTY_CHANGED = 13,     // Grupo do jogador alterado
    PLAYER_GUILD_CHANGED = 14,     // Guilda do jogador alterado
    PLAYER_FRIENDSHIP_CHANGED = 15, // Amizade do jogador alterada
    PLAYER_INVENTORY_CHANGED = 16, // Inventário do jogador alterado
    PLAYER_STORAGE_CHANGED = 17,   // Armazenamento do jogador alterado
    PLAYER_MAIL_CHANGED = 18,      // Correio do jogador alterado
    PLAYER_AUCTION_CHANGED = 19,   // Leilão do jogador alterado
    PLAYER_TRADE_CHANGED = 20,     // Comércio do jogador alterado
    PLAYER_SHOP_CHANGED = 21,      // Loja do jogador alterada
    PLAYER_BANK_CHANGED = 22,      // Banco do jogador alterado
    PLAYER_CUSTOM_1 = 23,          // Personalizado 1
    PLAYER_CUSTOM_2 = 24,          // Personalizado 2
    PLAYER_CUSTOM_3 = 25,          // Personalizado 3
    PLAYER_UNKNOWN = 26            // Desconhecido
};

/**
 * @brief Informações de evento de gerenciamento de jogadores
 */
struct PlayerManagerEvent {
    PlayerManagerEventType type;   // Tipo do evento
    PlayerId playerId;             // ID do jogador
    ClientId clientId;             // ID do cliente
    AccountId accountId;           // ID da conta
    void* data;                    // Dados do evento
    uint32_t dataSize;             // Tamanho dos dados
    uint32_t timestamp;            // Timestamp
    
    PlayerManagerEvent() : type(PlayerManagerEventType::PLAYER_UNKNOWN), playerId(0),
        clientId(0), accountId(0), data(nullptr), dataSize(0), timestamp(0) {}
    
    PlayerManagerEvent(PlayerManagerEventType _type, PlayerId _playerId, ClientId _clientId = 0,
                     AccountId _accountId = 0, void* _data = nullptr, uint32_t _dataSize = 0)
        : type(_type), playerId(_playerId), clientId(_clientId), accountId(_accountId),
          data(_data), dataSize(_dataSize),
          timestamp(static_cast<uint32_t>(time(nullptr))) {}
};

/**
 * @brief Estatísticas do gerenciador de jogadores
 */
struct PlayerManagerStats {
    uint32_t totalCreated;         // Total de jogadores criados
    uint32_t totalDestroyed;       // Total de jogadores destruídos
    uint32_t totalConnected;       // Total de jogadores conectados
    uint32_t totalDisconnected;    // Total de jogadores desconectados
    uint32_t totalLoaded;          // Total de jogadores carregados
    uint32_t totalSaved;           // Total de jogadores salvos
    uint32_t currentConnected;     // Jogadores conectados atualmente
    
    PlayerManagerStats() : totalCreated(0), totalDestroyed(0), totalConnected(0),
                         totalDisconnected(0), totalLoaded(0), totalSaved(0),
                         currentConnected(0) {}
};

/**
 * @brief Informações de comércio
 */
struct TradeInfo {
    PlayerId partnerId;            // ID do parceiro de comércio
    bool isConfirmed;              // Se o comércio está confirmado
    bool isPartnerConfirmed;       // Se o parceiro confirmou
    uint32_t gold;                 // Quantidade de gold no comércio
    STRUCT_ITEM items[15];         // Itens no comércio
    
    TradeInfo() : partnerId(0), isConfirmed(false), isPartnerConfirmed(false), gold(0) {
        memset(items, 0, sizeof(items));
    }
};

/**
 * @brief Informações de grupo
 */
struct PartyInfo {
    PlayerId leaderId;             // ID do líder do grupo
    bool isInvited;                // Se foi convidado
    uint32_t joinTime;             // Hora que entrou no grupo
    
    PartyInfo() : leaderId(0), isInvited(false), joinTime(0) {}
};

/**
 * @brief Classe para armazenar dados de jogador
 */
struct WYDPlayer {
    PlayerId playerId;             // ID do jogador
    AccountId accountId;           // ID da conta
    std::string accountName;       // Nome da conta
    ClientId clientId;             // ID do cliente
    bool isActive;                 // Se o jogador está ativo
    bool isOnline;                 // Se o jogador está online
    uint32_t creationTime;         // Hora de criação
    uint32_t loadTime;             // Hora de carregamento
    uint32_t lastActivity;         // Última atividade
    uint32_t saveCount;            // Contador de salvamentos
    
    STRUCT_MOB mob;                // Dados do personagem
    TradeInfo trade;               // Informações de comércio
    PartyInfo party;               // Informações de grupo
    uint32_t guildId;              // ID da guilda
    
    WYDPlayer() : playerId(0), accountId(0), clientId(0), 
                 isActive(false), isOnline(false),
                 creationTime(0), loadTime(0), lastActivity(0),
                 saveCount(0), guildId(0) {
        memset(&mob, 0, sizeof(STRUCT_MOB));
    }
};

// Tipos de callbacks
typedef std::function<void(const PlayerManagerEvent&)> PlayerManagerEventCallback;
typedef std::function<void(const std::string&, PlayerId, int)> PlayerManagerErrorCallback;

/**
 * @brief Gerenciador de jogadores do servidor WYD
 */
/**
 * Classe WYDPlayerManager
 * 
 * Esta classe implementa a funcionalidade WYDPlayerManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PlayerManager {
public:
    // Singleton
    static PlayerManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador de jogadores
     * @param maxPlayers Número máximo de jogadores
     * @param basePath Caminho base para arquivos de jogador
     * @param saveInterval Intervalo de salvamento automático (segundos)
     * @param autoSave Se deve ativar salvamento automático
     * @return true se inicializado com sucesso
     */
    bool Initialize(uint32_t maxPlayers, const std::string& basePath, 
                  uint32_t saveInterval = 300, bool autoSave = true);
    
    /**
     * @brief Finaliza o gerenciador de jogadores
     */
    void Shutdown();
    
    /**
     * @brief Processa um tick do gerenciador de jogadores
     */
    void ProcessTick();
    
    /**
     * @brief Cria um jogador
     * @param mob Dados do personagem
     * @param accountId ID da conta
     * @param accountName Nome da conta
     * @param clientId ID do cliente (conexão)
     * @return ID do jogador criado (0 se falhar)
     */
    PlayerId CreatePlayer(const STRUCT_MOB* mob, AccountId accountId, const std::string& accountName, ClientId clientId);
    
    /**
     * @brief Carrega um jogador pelo nome
     * @param characterName Nome do personagem
     * @param accountId ID da conta
     * @param accountName Nome da conta
     * @param clientId ID do cliente (conexão)
     * @return ID do jogador carregado (0 se falhar)
     */
    PlayerId LoadPlayer(const std::string& characterName, AccountId accountId, const std::string& accountName, ClientId clientId);
    
    /**
     * @brief Salva um jogador
     * @param playerId ID do jogador
     * @return true se salvo com sucesso
     */
    bool SavePlayer(PlayerId playerId);
    
    /**
     * @brief Salva todos os jogadores
     * @return Número de jogadores salvos
     */
    uint32_t SaveAllPlayers();
    
    /**
     * @brief Remove um jogador
     * @param playerId ID do jogador
     * @param save Se deve salvar antes de remover
     * @return true se removido com sucesso
     */
    bool RemovePlayer(PlayerId playerId, bool save = true);
    
    /**
     * @brief Conecta um jogador
     * @param playerId ID do jogador
     * @param clientId ID do cliente
     * @return true se conectado com sucesso
     */
    bool Connect(PlayerId playerId, ClientId clientId);
    
    /**
     * @brief Desconecta um jogador
     * @param playerId ID do jogador
     * @param save Se deve salvar antes de desconectar
     * @return true se desconectado com sucesso
     */
    bool Disconnect(PlayerId playerId, bool save = true);
    
    /**
     * @brief Atualiza a posição de um jogador
     * @param playerId ID do jogador
     * @param x Nova posição X
     * @param y Nova posição Y
     * @return true se atualizado com sucesso
     */
    bool UpdatePosition(PlayerId playerId, uint16_t x, uint16_t y);
    
    /**
     * @brief Define o grupo de um jogador
     * @param playerId ID do jogador
     * @param partyLeaderId ID do líder do grupo
     * @return true se atualizado com sucesso
     */
    bool SetParty(PlayerId playerId, PlayerId partyLeaderId);
    
    /**
     * @brief Define a guilda de um jogador
     * @param playerId ID do jogador
     * @param guildId ID da guilda
     * @return true se atualizado com sucesso
     */
    bool SetGuild(PlayerId playerId, uint32_t guildId);
    
    /**
     * @brief Obtém um jogador pelo ID
     * @param playerId ID do jogador
     * @return Ponteiro para o jogador (nullptr se não encontrado)
     */
    std::shared_ptr<WYDPlayer> GetPlayer(PlayerId playerId) const;
    
    /**
     * @brief Obtém um jogador pelo ID do cliente
     * @param clientId ID do cliente
     * @return Ponteiro para o jogador (nullptr se não encontrado)
     */
    std::shared_ptr<WYDPlayer> GetPlayerByClient(ClientId clientId) const;
    
    /**
     * @brief Obtém um jogador pelo nome
     * @param name Nome do jogador
     * @return Ponteiro para o jogador (nullptr se não encontrado)
     */
    std::shared_ptr<WYDPlayer> GetPlayerByName(const std::string& name) const;
    
    /**
     * @brief Obtém jogadores em uma área
     * @param x1 Coordenada X1
     * @param y1 Coordenada Y1
     * @param x2 Coordenada X2
     * @param y2 Coordenada Y2
     * @return Lista de IDs de jogadores na área
     */
    std::vector<PlayerId> GetPlayersInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const;
    
    /**
     * @brief Obtém jogadores em um raio
     * @param x Coordenada X central
     * @param y Coordenada Y central
     * @param radius Raio
     * @return Lista de IDs de jogadores no raio
     */
    std::vector<PlayerId> GetPlayersInRadius(uint16_t x, uint16_t y, uint16_t radius) const;
    
    /**
     * @brief Obtém jogadores em um grupo
     * @param partyLeaderId ID do líder do grupo
     * @return Lista de IDs de jogadores no grupo
     */
    std::vector<PlayerId> GetPlayersInParty(PlayerId partyLeaderId) const;
    
    /**
     * @brief Obtém jogadores em uma guilda
     * @param guildId ID da guilda
     * @return Lista de IDs de jogadores na guilda
     */
    std::vector<PlayerId> GetPlayersInGuild(uint32_t guildId) const;
    
    /**
     * @brief Obtém todos os jogadores
     * @return Lista de IDs de jogadores
     */
    std::vector<PlayerId> GetAllPlayers() const;
    
    /**
     * @brief Obtém todos os jogadores online
     * @return Lista de IDs de jogadores online
     */
    std::vector<PlayerId> GetOnlinePlayers() const;
    
    /**
     * @brief Obtém estatísticas do gerenciador
     * @return Estatísticas
     */
    PlayerManagerStats GetStats() const;
    
    /**
     * @brief Verifica inatividade de jogadores
     * @param currentTime Tempo atual
     * @return Número de jogadores desconectados por inatividade
     */
    uint32_t CheckPlayerInactivity(uint32_t currentTime);
    
    /**
     * @brief Registra um callback para eventos
     * @param callback Função de callback
     * @return ID do callback
     */
    uint32_t RegisterEventCallback(PlayerManagerEventCallback callback);
    
    /**
     * @brief Remove um callback de eventos
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterEventCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback para erros
     * @param callback Função de callback
     * @return ID do callback
     */
    uint32_t RegisterErrorCallback(PlayerManagerErrorCallback callback);
    
    /**
     * @brief Remove um callback de erros
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterErrorCallback(uint32_t callbackId);
    
    /**
     * @brief Define o modo de debug
     * @param enabled Se o modo de debug está ativado
     */
    void SetDebugMode(bool enabled);
    
    /**
     * @brief Define o modo de logging
     * @param enabled Se o logging está ativado
     */
    void SetLogging(bool enabled);
    
    /**
     * @brief Loga uma mensagem de informação
     * @param message Mensagem
     */
    void LogInfo(const std::string& message) const;
    
    /**
     * @brief Loga uma mensagem de aviso
     * @param message Mensagem
     */
    void LogWarning(const std::string& message) const;
    
    /**
     * @brief Loga uma mensagem de erro
     * @param message Mensagem
     */
    void LogError(const std::string& message) const;
    
    /**
     * @brief Loga uma mensagem de debug
     * @param message Mensagem
     */
    void LogDebug(const std::string& message) const;
};

} // namespace tmsrv
} // namespace wydbr

#endif // _PLAYER_MANAGER_H_