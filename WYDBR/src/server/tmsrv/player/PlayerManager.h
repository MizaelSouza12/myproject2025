/**
 * PlayerManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/player/PlayerManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <memory>
#include <unordered_map>

#include "headers/wyd_core.h"
#include "src/server/tmsrv/player/WYDPlayer.h"

namespace wydbr {


class TMServer;

/**
 * Gerenciador de jogadores
 * 
 * Responsável por controlar todos os jogadores conectados, incluindo
 * autenticação, logout, e processamento de comandos relacionados a jogadores.
 */
/**
 * Classe WYDPlayerManager
 * 
 * Esta classe implementa a funcionalidade WYDPlayerManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PlayerManager : public WYDSystemManager {
public:
    // Construtor
    /**
 * PlayerManager
 * 
 * Implementa a funcionalidade PlayerManager conforme especificação original.
 * @param server Parâmetro server
 * @return Retorna explicit
 */

    explicit PlayerManager(TMServer* server);
    
    // Destrutor
    virtual ~PlayerManager();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @return Retorna bool
 */

    bool initialize() override;
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp) override;
    
    // Desligamento
    /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 */

    void shutdown() override;
    
    // Autenticação/conexão de jogador
    /**
 * createSession
 * 
 * Implementa a funcionalidade createSession conforme especificação original.
 * @param clientId Parâmetro clientId
 * @param account Parâmetro account
 * @return Retorna uint32_t
 */

    uint32_t createSession(uint32_t clientId, const std::string& account);
    /**
 * loginPlayer
 * 
 * Implementa a funcionalidade loginPlayer conforme especificação original.
 * @param sessionId Parâmetro sessionId
 * @param characterName Parâmetro characterName
 * @return Retorna bool
 */

    bool loginPlayer(uint32_t sessionId, const std::string& characterName);
    /**
 * disconnectPlayer
 * 
 * Implementa a funcionalidade disconnectPlayer conforme especificação original.
 * @param sessionId Parâmetro sessionId
 * @return Retorna bool
 */

    bool disconnectPlayer(uint32_t sessionId);
    /**
 * disconnectClient
 * 
 * Implementa a funcionalidade disconnectClient conforme especificação original.
 * @param clientId Parâmetro clientId
 * @return Retorna bool
 */

    bool disconnectClient(uint32_t clientId);
    
    // Acesso a sessions/players
    /**
 * getSession
 * 
 * Implementa a funcionalidade getSession conforme especificação original.
 * @param sessionId Parâmetro sessionId
 * @return Retorna WYDSession
 */

    WYDSession* getSession(uint32_t sessionId);
    /**
 * getPlayer
 * 
 * Implementa a funcionalidade getPlayer conforme especificação original.
 * @param sessionId Parâmetro sessionId
 * @return Retorna WYDPlayer
 */

    WYDPlayer* getPlayer(uint32_t sessionId);
    /**
 * getPlayerByName
 * 
 * Implementa a funcionalidade getPlayerByName conforme especificação original.
 * @param name Parâmetro name
 * @return Retorna WYDPlayer
 */

    WYDPlayer* getPlayerByName(const std::string& name);
    
    // Gerenciamento de personagens
    /**
 * createCharacter
 * 
 * Implementa a funcionalidade createCharacter conforme especificação original.
 * @param sessionId Parâmetro sessionId
 * @param name Parâmetro name
 * @param charClass Parâmetro charClass
 * @return Retorna bool
 */

    bool createCharacter(uint32_t sessionId, const std::string& name, wyd::CharacterClass charClass);
    /**
 * deleteCharacter
 * 
 * Implementa a funcionalidade deleteCharacter conforme especificação original.
 * @param sessionId Parâmetro sessionId
 * @param name Parâmetro name
 * @return Retorna bool
 */

    bool deleteCharacter(uint32_t sessionId, const std::string& name);
    std::vector<CharacterInfo> getCharacterList(uint32_t sessionId);
    
    // Salvamento de dados
    /**
 * savePlayerData
 * 
 * Implementa a funcionalidade savePlayerData conforme especificação original.
 * @param sessionId Parâmetro sessionId
 * @return Retorna bool
 */

    bool savePlayerData(uint32_t sessionId);
    /**
 * saveAllPlayers
 * 
 * Implementa a funcionalidade saveAllPlayers conforme especificação original.
 * @return Retorna bool
 */

    bool saveAllPlayers();
    
    // Broadcast
    /**
 * broadcastToAll
 * 
 * Implementa a funcionalidade broadcastToAll conforme especificação original.
 * @param packet Parâmetro packet
 * @return Retorna bool
 */

    bool broadcastToAll(const WYDPacket& packet);
    /**
 * broadcastToMap
 * 
 * Implementa a funcionalidade broadcastToMap conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param packet Parâmetro packet
 * @return Retorna bool
 */

    bool broadcastToMap(const std::string& mapId, const WYDPacket& packet);
    /**
 * broadcastInRange
 * 
 * Implementa a funcionalidade broadcastInRange conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param posX Parâmetro posX
 * @param posY Parâmetro posY
 * @param radius Parâmetro radius
 * @param packet Parâmetro packet
 * @return Retorna bool
 */

    bool broadcastInRange(const std::string& mapId, uint16_t posX, uint16_t posY, uint16_t radius, const WYDPacket& packet);
    
    // Funções de utilidade
    /**
 * getOnlinePlayerCount
 * 
 * Implementa a funcionalidade getOnlinePlayerCount conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getOnlinePlayerCount() const;
    std::vector<WYDPlayer*> getPlayersInMap(const std::string& mapId);
    
private:
    // Sessões e jogadores
    std::unordered_map<uint32_t, std::unique_ptr<WYDSession>> sessions_;
    std::unordered_map<std::string, uint32_t> nameLookup_; // Mapeamento nome -> sessionId
    
    // Mutex para acesso seguro
    mutable std::mutex mutex_;
    
    // Dados do último auto-save
    uint64_t lastAutoSaveTime_;
    
    // Método para salvar um jogador
    /**
 * savePlayerToDatabase
 * 
 * Implementa a funcionalidade savePlayerToDatabase conforme especificação original.
 * @param player Parâmetro player
 * @return Retorna bool
 */

    bool savePlayerToDatabase(WYDPlayer* player);
    
    // Gera um novo ID de sessão
    /**
 * generateSessionId
 * 
 * Implementa a funcionalidade generateSessionId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t generateSessionId();
};

/**
 * Informações resumidas de um personagem para seleção
 */
struct CharacterInfo {
    std::string name;
    wyd::CharacterClass charClass;
    uint16_t level;
    std::string guild;
    bool isDeleting;
    uint32_t deleteTime;
};

/**
 * Sessão de jogador
 */
/**
 * Classe WYDSession
 * 
 * Esta classe implementa a funcionalidade WYDSession conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDSession {
public:
    // /**
 * WYDSession
 * 
 * Implementa a funcionalidade WYDSession conforme especificação original.
 * @param sessionId Parâmetro sessionId
 * @param clientId Parâmetro clientId
 * @param account Parâmetro account
 * @return Retorna Construtor
 */
 Construtor
    WYDSession(uint32_t sessionId, uint32_t clientId, const std::string& account);
    
    // Destrutor
    ~WYDSession();
    
    // Getters
    /**
 * getSessionId
 * 
 * Implementa a funcionalidade getSessionId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getSessionId() const{ return sessionId_; }
    /**
 * getClientId
 * 
 * Implementa a funcionalidade getClientId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getClientId() const{ return clientId_; }
    const std::string& getAccount() const { return account_; }
    /**
 * getPlayer
 * 
 * Implementa a funcionalidade getPlayer conforme especificação original.
 * @return Retorna WYDPlayer
 */

    WYDPlayer* getPlayer(){ return player_.get(); }
    /**
 * getPlayer
 * 
 * Implementa a funcionalidade getPlayer conforme especificação original.
 * @return Retorna WYDPlayer
 */

    const WYDPlayer* getPlayer() const{ return player_.get(); }
    /**
 * getState
 * 
 * Implementa a funcionalidade getState conforme especificação original.
 * @return Retorna wyd::ConnectionState
 */

    wyd::ConnectionState getState() const{ return state_; }
    
    // Setters
    /**
 * setState
 * 
 * Implementa a funcionalidade setState conforme especificação original.
 * @param state Parâmetro state
 */

    void setState(wyd::ConnectionState state){ state_ = state; }
    
    // Ações
    /**
 * createPlayer
 * 
 * Implementa a funcionalidade createPlayer conforme especificação original.
 * @param name Parâmetro name
 * @param charClass Parâmetro charClass
 * @return Retorna bool
 */

    bool createPlayer(const std::string& name, wyd::CharacterClass charClass);
    
private:
    uint32_t sessionId_;
    uint32_t clientId_;
    std::string account_;
    std::unique_ptr<WYDPlayer> player_;
    wyd::ConnectionState state_;
};

#endif // PLAYER_MANAGER_H

} // namespace wydbr
