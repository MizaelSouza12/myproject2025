/**
 * PlayerManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/player/PlayerManager.h
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

#include "../../include/GlobalDef.h"
#include "player/WYDPlayer.h"

namespace wydbr {
namespace tmsrv {

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
    uint32_t currentActive;        // Jogadores ativos atualmente
    uint32_t peakConnected;        // Pico de jogadores conectados
    uint32_t peakActive;           // Pico de jogadores ativos
    
    PlayerManagerStats() : totalCreated(0), totalDestroyed(0), totalConnected(0),
        totalDisconnected(0), totalLoaded(0), totalSaved(0), currentConnected(0),
        currentActive(0), peakConnected(0), peakActive(0) {}
};

/**
 * @brief Tipo de callback de evento do gerenciador de jogadores
 */
using PlayerManagerEventCallback = std::function<void(const PlayerManagerEvent&)>;

/**
 * @brief Tipo de callback de erro do gerenciador de jogadores
 */
using PlayerManagerErrorCallback = std::function<void(const std::string&, PlayerId)>;

/**
 * @brief Gerenciador de jogadores
 * 
 * Esta classe é responsável por gerenciar os jogadores no sistema WYDBR,
 * incluindo criação, destruição, conexão, desconexão, carregamento e salvamento.
 */
/**
 * Classe WYDPlayerManager
 * 
 * Esta classe implementa a funcionalidade WYDPlayerManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PlayerManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static PlayerManager& getInstance() {
        static PlayerManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Cria um jogador
     * @param clientId ID do cliente
     * @param accountId ID da conta
     * @param name Nome do jogador
     * @return ID do jogador ou 0 em caso de erro
     */
    PlayerId createPlayer(ClientId clientId, AccountId accountId, const std::string& name);
    
    /**
     * @brief Destrói um jogador
     * @param playerId ID do jogador
     * @return true se destruído com sucesso
     */
    bool destroyPlayer(PlayerId playerId);
    
    /**
     * @brief Conecta um jogador
     * @param playerId ID do jogador
     * @param clientId ID do cliente
     * @return true se conectado com sucesso
     */
    bool connectPlayer(PlayerId playerId, ClientId clientId);
    
    /**
     * @brief Desconecta um jogador
     * @param playerId ID do jogador
     * @param reason Motivo da desconexão
     * @return true se desconectado com sucesso
     */
    bool disconnectPlayer(PlayerId playerId, const std::string& reason = "");
    
    /**
     * @brief Desconecta um jogador pelo ID do cliente
     * @param clientId ID do cliente
     * @param reason Motivo da desconexão
     * @return true se desconectado com sucesso
     */
    bool disconnectPlayerByClientId(ClientId clientId, const std::string& reason = "");
    
    /**
     * @brief Carrega um jogador
     * @param playerId ID do jogador
     * @return true se carregado com sucesso
     */
    bool loadPlayer(PlayerId playerId);
    
    /**
     * @brief Salva um jogador
     * @param playerId ID do jogador
     * @return true se salvo com sucesso
     */
    bool savePlayer(PlayerId playerId);
    
    /**
     * @brief Salva todos os jogadores
     * @return Número de jogadores salvos
     */
    uint32_t saveAllPlayers();
    
    /**
     * @brief Obtém um jogador pelo ID
     * @param playerId ID do jogador
     * @return Ponteiro para o jogador ou nullptr se não encontrado
     */
    player::WYDPlayer* getPlayer(PlayerId playerId);
    
    /**
     * @brief Obtém um jogador pelo ID do cliente
     * @param clientId ID do cliente
     * @return Ponteiro para o jogador ou nullptr se não encontrado
     */
    player::WYDPlayer* getPlayerByClientId(ClientId clientId);
    
    /**
     * @brief Obtém um jogador pelo nome
     * @param name Nome do jogador
     * @return Ponteiro para o jogador ou nullptr se não encontrado
     */
    player::WYDPlayer* getPlayerByName(const std::string& name);
    
    /**
     * @brief Verifica se um jogador está conectado
     * @param playerId ID do jogador
     * @return true se estiver conectado
     */
    bool isPlayerConnected(PlayerId playerId);
    
    /**
     * @brief Verifica se um jogador está online
     * @param playerId ID do jogador
     * @return true se estiver online
     */
    bool isPlayerOnline(PlayerId playerId);
    
    /**
     * @brief Obtém o número de jogadores conectados
     * @return Número de jogadores conectados
     */
    uint32_t getConnectedCount() const;
    
    /**
     * @brief Obtém o número de jogadores online
     * @return Número de jogadores online
     */
    uint32_t getOnlineCount() const;
    
    /**
     * @brief Obtém o número total de jogadores
     * @return Número total de jogadores
     */
    uint32_t getTotalCount() const;
    
    /**
     * @brief Obtém as estatísticas do gerenciador
     * @return Estatísticas
     */
    const PlayerManagerStats& getStats() const;
    
    /**
     * @brief Reseta as estatísticas do gerenciador
     */
    void resetStats();
    
    /**
     * @brief Registra um callback de evento
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerEventCallback(PlayerManagerEventCallback callback);
    
    /**
     * @brief Remove um callback de evento
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEventCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de erro
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerErrorCallback(PlayerManagerErrorCallback callback);
    
    /**
     * @brief Remove um callback de erro
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterErrorCallback(uint32_t callbackId);
    
    /**
     * @brief Dispara um evento
     * @param event Evento
     */
    void fireEvent(const PlayerManagerEvent& event);
    
    /**
     * @brief Reporta um erro
     * @param error Mensagem de erro
     * @param playerId ID do jogador
     */
    void reportError(const std::string& error, PlayerId playerId = 0);
    
    /**
     * @brief Envia um pacote para um jogador
     * @param playerId ID do jogador
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @return true se enviado com sucesso
     */
    bool sendPacket(PlayerId playerId, uint16_t opcode, const uint8_t* data, uint32_t size);
    
    /**
     * @brief Envia um pacote para todos os jogadores
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param exceptPlayerId ID do jogador a ser excluído
     * @return Número de jogadores que receberam o pacote
     */
    uint32_t sendPacketToAll(uint16_t opcode, const uint8_t* data, uint32_t size, PlayerId exceptPlayerId = 0);
    
    /**
     * @brief Envia um pacote para um grupo de jogadores
     * @param partyId ID do grupo
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param exceptPlayerId ID do jogador a ser excluído
     * @return Número de jogadores que receberam o pacote
     */
    uint32_t sendPacketToParty(PartyId partyId, uint16_t opcode, const uint8_t* data, uint32_t size, PlayerId exceptPlayerId = 0);
    
    /**
     * @brief Envia um pacote para uma guilda
     * @param guildId ID da guilda
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param exceptPlayerId ID do jogador a ser excluído
     * @return Número de jogadores que receberam o pacote
     */
    uint32_t sendPacketToGuild(GuildId guildId, uint16_t opcode, const uint8_t* data, uint32_t size, PlayerId exceptPlayerId = 0);
    
    /**
     * @brief Envia um pacote para um mapa
     * @param mapId ID do mapa
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param exceptPlayerId ID do jogador a ser excluído
     * @return Número de jogadores que receberam o pacote
     */
    uint32_t sendPacketToMap(MapId mapId, uint16_t opcode, const uint8_t* data, uint32_t size, PlayerId exceptPlayerId = 0);
    
    /**
     * @brief Envia um pacote para uma área
     * @param mapId ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param radius Raio
     * @param opcode Código de operação
     * @param data Dados do pacote
     * @param size Tamanho dos dados
     * @param exceptPlayerId ID do jogador a ser excluído
     * @return Número de jogadores que receberam o pacote
     */
    uint32_t sendPacketToArea(MapId mapId, int16_t x, int16_t y, uint16_t radius, uint16_t opcode, const uint8_t* data, uint32_t size, PlayerId exceptPlayerId = 0);
    
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
     * @brief Executa uma função para cada jogador
     * @param func Função a ser executada
     */
    void forEachPlayer(std::function<void(player::WYDPlayer*)> func);
    
    /**
     * @brief Executa uma função para cada jogador online
     * @param func Função a ser executada
     */
    void forEachOnlinePlayer(std::function<void(player::WYDPlayer*)> func);
    
    /**
     * @brief Executa uma função para cada jogador conectado
     * @param func Função a ser executada
     */
    void forEachConnectedPlayer(std::function<void(player::WYDPlayer*)> func);
    
    /**
     * @brief Cria uma instância do jogador do WYD
     * @param playerId ID do jogador
     * @param clientId ID do cliente
     * @param accountId ID da conta
     * @param name Nome do jogador
     * @return Ponteiro para o jogador ou nullptr em caso de erro
     */
    player::WYDPlayer* createPlayerInstance(PlayerId playerId, ClientId clientId, AccountId accountId, const std::string& name);
    
private:
    /**
     * @brief Construtor privado
     */
    PlayerManager();
    
    /**
     * @brief Destrutor privado
     */
    ~PlayerManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    PlayerManager(const PlayerManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    PlayerManager& operator=(const PlayerManager&) = delete;
    
    /**
     * @brief Gera um ID de jogador único
     * @return ID do jogador
     */
    PlayerId generatePlayerId();
    
    /**
     * @brief Loga uma mensagem
     * @param message Mensagem
     * @param playerId ID do jogador
     */
    void logMessage(const std::string& message, PlayerId playerId = 0);
    
    /**
     * @brief Atualiza automaticamente os jogadores
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void updatePlayers(uint32_t deltaTime);
    
    /**
     * @brief Salva automaticamente os jogadores
     */
    void autoSavePlayers();
    
    /**
     * @brief Verifica os jogadores inativos
     */
    void checkInactivePlayers();
    
    /**
     * @brief Atualiza as estatísticas
     */
    void updateStats();
    
    // Mapa de jogadores
    std::unordered_map<PlayerId, std::unique_ptr<player::WYDPlayer>> _players;
    
    // Mapa de jogadores por cliente
    std::unordered_map<ClientId, PlayerId> _playersByClient;
    
    // Mapa de jogadores por nome
    std::unordered_map<std::string, PlayerId> _playersByName;
    
    // Mapa de jogadores por mapa
    std::map<MapId, std::vector<PlayerId>> _playersByMap;
    
    // Mapa de jogadores por grupo
    std::map<PartyId, std::vector<PlayerId>> _playersByParty;
    
    // Mapa de jogadores por guilda
    std::map<GuildId, std::vector<PlayerId>> _playersByGuild;
    
    // Estatísticas
    PlayerManagerStats _stats;
    
    // Próximo ID de jogador
    std::atomic<PlayerId> _nextPlayerId;
    
    // Callbacks de evento
    std::unordered_map<uint32_t, PlayerManagerEventCallback> _eventCallbacks;
    
    // Callbacks de erro
    std::unordered_map<uint32_t, PlayerManagerErrorCallback> _errorCallbacks;
    
    // Próximo ID de callback
    std::atomic<uint32_t> _nextCallbackId;
    
    // Temporizador de salvamento automático
    uint32_t _autoSaveTimer;
    
    // Temporizador de verificação de inatividade
    uint32_t _inactivityCheckTimer;
    
    // Intervalo de salvamento automático
    uint32_t _autoSaveInterval;
    
    // Intervalo de verificação de inatividade
    uint32_t _inactivityCheckInterval;
    
    // Tempo de inatividade
    uint32_t _inactivityTime;
    
    // Estado do gerenciador
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    // Thread safety
    mutable std::mutex _playersMutex;
    mutable std::mutex _playersByClientMutex;
    mutable std::mutex _playersByNameMutex;
    mutable std::mutex _playersByMapMutex;
    mutable std::mutex _playersByPartyMutex;
    mutable std::mutex _playersByGuildMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _callbacksMutex;
};

} // namespace tmsrv
} // namespace wydbr

#endif // _PLAYER_MANAGER_H_