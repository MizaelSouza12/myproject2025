/**
 * @file PlayerManager.cpp
 * @brief Implementação do sistema de gerenciamento de jogadores
 * 
 * Este arquivo implementa a classe de gerenciamento de jogadores do servidor WYD,
 * mantendo compatibilidade total com as estruturas originais e implementando
 * funcionalidades avançadas para gestão mais eficiente.
 */

#include "../../include/tmsrv/PlayerManager.h"
#include "../../include/database/DatabaseManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <random>
#include <cstring>
#include <functional>
#include <ctime>

namespace wydbr {
namespace tmsrv {

// Declaração de tipos e classes privadas

// Implementação dos métodos do PlayerManager
// A classe já está declarada no header

// Singleton
PlayerManager& PlayerManager::GetInstance() {
        static PlayerManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador de jogadores
     * @param maxPlayers Número máximo de jogadores
     * @param basePath Caminho base para arquivos de jogador
     * @param saveInterval Intervalo de salvamento automático (segundos)
     * @param autoSave Se deve ativar salvamento automático
     * @return true se inicializado com sucesso
     */
    bool PlayerManager::Initialize(uint32_t maxPlayers, const std::string& basePath, 
                   uint32_t saveInterval, bool autoSave) {
        std::lock_guard<std::mutex> lock(_statsMutex);
        
        if (_initialized) {
            std::cerr << "PlayerManager já inicializado" << std::endl;
            return false;
        }
        
        _basePath = basePath;
        _maxPlayers = maxPlayers;
        _autoSaveInterval = saveInterval;
        _autoSaveEnabled = autoSave;
        
        // Inicializar estatísticas
        _stats.totalCreated = 0;
        _stats.totalDestroyed = 0;
        _stats.totalConnected = 0;
        _stats.totalDisconnected = 0;
        _stats.totalLoaded = 0;
        _stats.totalSaved = 0;
        _stats.currentConnected = 0;
        
        // Inicializar timers
        _autoSaveTimer = static_cast<uint32_t>(time(nullptr));
        _inactivityCheckTimer = static_cast<uint32_t>(time(nullptr));
        _inactivityCheckInterval = 60; // 1 minuto
        _inactivityTime = 3600; // 1 hora
        
        // Inicializar próximo ID
        _nextPlayerId = 1;
        _nextCallbackId = 1;
        
        // Verificar diretório base
        if (!_ensureDirectory(_basePath)) {
            std::cerr << "Falha ao criar diretório base: " << _basePath << std::endl;
            return false;
        }
        
        _initialized = true;
        _debugMode = false;
        _loggingEnabled = true;
        
        LogInfo("PlayerManager inicializado com sucesso");
        return true;
    }
    
    /**
     * @brief Finaliza o gerenciador de jogadores
     */
    void PlayerManager::Shutdown() {
        LogInfo("Finalizando PlayerManager...");
        
        if (!_initialized) {
            std::cerr << "PlayerManager não inicializado" << std::endl;
            return;
        }
        
        // Salvar todos os jogadores
        PlayerManager::SaveAllPlayers();
        
        // Remover todos os jogadores
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            _players.clear();
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByClientMutex);
            _playersByClient.clear();
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByNameMutex);
            _playersByName.clear();
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByMapMutex);
            _playersByMap.clear();
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByPartyMutex);
            _playersByParty.clear();
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByGuildMutex);
            _playersByGuild.clear();
        }
        
        // Limpar callbacks
        {
            std::lock_guard<std::mutex> lock(_callbacksMutex);
            _eventCallbacks.clear();
            _errorCallbacks.clear();
        }
        
        _initialized = false;
        LogInfo("PlayerManager finalizado");
    }
    
    /**
     * @brief Processa um tick do gerenciador de jogadores
     */
    void PlayerManager::ProcessTick() {
        if (!_initialized) {
            return;
        }
        
        uint32_t currentTime = static_cast<uint32_t>(time(nullptr));
        
        // Verificar salvamento automá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param _autoSaveInterval Parâmetro _autoSaveInterval
 * @return Retorna tico
 */
tico
        if(_autoSaveEnabled && currentTime - _autoSaveTimer >= _autoSaveInterval){
            SaveAllPlayers();
            _autoSaveTimer = currentTime;
        }
        
        // Verificar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param _inactivityCheckInterval Parâmetro _inactivityCheckInterval
 * @return Retorna inatividade
 */
 inatividade
        if(currentTime - _inactivityCheckTimer >= _inactivityCheckInterval){
            CheckPlayerInactivity(currentTime);
            _inactivityCheckTimer = currentTime;
        }
    }
    
    /**
     * @brief Cria um jogador
     * @param mob Dados do personagem
     * @param accountId ID da conta
     * @param accountName Nome da conta
     * @param clientId ID do cliente (conexão)
     * @return ID do jogador criado (0 se falhar)
     */
    PlayerId PlayerManager::CreatePlayer(const STRUCT_MOB* mob, AccountId accountId, const std::string& accountName, ClientId clientId) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return 0;
        }
        
        if (mob == nullptr) {
            LogError("Dados do personagem inválidos");
            return 0;
        }
        
        // Verificar se já existe jogador com mesmo nome
        if (GetPlayerByName(mob->MobName) != nullptr) {
            LogError("Já existe um jogador com o nome: " + std::string(mob->MobName));
            return 0;
        }
        
        // Verificar o número máximo de jogadores
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            if (_players.size() >= _maxPlayers) {
                LogError("Número máximo de jogadores atingido");
                return 0;
            }
        }
        
        // Criar jogador
        PlayerId playerId = _nextPlayerId++;
        std::shared_ptr<WYDPlayer> player = std::make_shared<WYDPlayer>();
        
        // Inicializar dados
        player->playerId = playerId;
        player->accountId = accountId;
        player->accountName = accountName;
        player->clientId = clientId;
        player->isActive = true;
        player->isOnline = false;
        player->lastActivity = static_cast<uint32_t>(time(nullptr));
        player->creationTime = static_cast<uint32_t>(time(nullptr));
        player->saveCount = 0;
        
        // Copiar dados do MOB
        PlayerManager::memcpy(&player->mob, mob, sizeof(STRUCT_MOB));
        
        // Limpar listas e dados voláteis
        memset(&player->trade, 0, sizeof(player->trade));
        memset(&player->party, 0, sizeof(player->party));
        player->guildId = 0;
        
        // Adicionar às listas
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            _players[playerId] = player;
        }
        
        if (clientId > 0) {
            std::lock_guard<std::mutex> lock(_playersByClientMutex);
            _playersByClient[clientId] = playerId;
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByNameMutex);
            _playersByName[player->mob.MobName] = playerId;
        }
        
        // Atualizar estatísticas
        {
            std::lock_guard<std::mutex> lock(_statsMutex);
            _stats.totalCreated++;
        }
        
        // Disparar evento
        PlayerManager::_fireEvent(PlayerManagerEventType::PLAYER_CREATED, playerId, clientId, accountId);
        
        LogInfo("Jogador criado: " + std::string(player->mob.MobName) + " (ID: " + std::to_string(playerId) + ")");
        return playerId;
    }
    
    /**
     * @brief Carrega um jogador pelo nome
     * @param characterName Nome do personagem
     * @param accountId ID da conta
     * @param accountName Nome da conta
     * @param clientId ID do cliente (conexão)
     * @return ID do jogador carregado (0 se falhar)
     */
    PlayerId PlayerManager::LoadPlayer(const std::string& characterName, AccountId accountId, const std::string& accountName, ClientId clientId) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return 0;
        }
        
        // Verificar se o jogador já está carregado
        std::shared_ptr<WYDPlayer> existingPlayer = GetPlayerByName(characterName);
        if (existingPlayer != nullptr) {
            LogWarning("Jogador já carregado: " + characterName);
            
            // Se o jogador estiver online, verificar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param existingPlayer->isOnline Parâmetro existingPlayer->isOnline
 * @return Retorna conta
 */
 conta
            if(existingPlayer->isOnline){
                if (existingPlayer->accountId != accountId) {
                    LogError("Tentativa de acesso a personagem por conta diferente");
                    return 0;
                }
                
                LogWarning("Jogador já está online, forçando desconexão anterior");
                Disconnect(existingPlayer->playerId);
            }
            
            // Atualizar cliente
            existingPlayer->clientId = clientId;
            existingPlayer->isOnline = true;
            existingPlayer->lastActivity = static_cast<uint32_t>(time(nullptr));
            
            // Atualizar mapeamento de cliente
            {
                std::lock_guard<std::mutex> lock(_playersByClientMutex);
                _playersByClient[clientId] = existingPlayer->playerId;
            }
            
            // Atualizar estatísticas
            {
                std::lock_guard<std::mutex> lock(_statsMutex);
                _stats.totalConnected++;
                _stats.currentConnected++;
            }
            
            // Disparar evento
            PlayerManager::_fireEvent(PlayerManagerEventType::PLAYER_CONNECTED, existingPlayer->playerId, clientId, accountId);
            
            LogInfo("Jogador reconectado: " + characterName + " (ID: " + std::to_string(existingPlayer->playerId) + ")");
            return existingPlayer->playerId;
        }
        
        // Verificar o número máximo de jogadores
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            if (_players.size() >= _maxPlayers) {
                LogError("Número máximo de jogadores atingido");
                return 0;
            }
        }
        
        // Caminho do arquivo do personagem
        std::string filePath = _getCharacterFilePath(characterName);
        
        // Verificar se o arquivo existe
        /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            LogError("Arquivo do personagem não encontrado: " + filePath);
            return 0;
        }
        
        // Criar jogador
        PlayerId playerId = _nextPlayerId++;
        std::shared_ptr<WYDPlayer> player = std::make_shared<WYDPlayer>();
        
        // Inicializar dados
        player->playerId = playerId;
        player->accountId = accountId;
        player->accountName = accountName;
        player->clientId = clientId;
        player->isActive = true;
        player->isOnline = true;
        player->lastActivity = static_cast<uint32_t>(time(nullptr));
        player->loadTime = static_cast<uint32_t>(time(nullptr));
        player->saveCount = 0;
        
        // Ler dados do arquivo
        file.read(reinterpret_cast<char*>(&player->mob), sizeof(STRUCT_MOB));
        file.close();
        
        // Verificar nome
        if (strcmp(player->mob.MobName, characterName.c_str()) != 0) {
            LogError("Nome de personagem não corresponde ao arquivo: " + characterName);
            return 0;
        }
        
        // Limpar listas e dados voláteis
        memset(&player->trade, 0, sizeof(player->trade));
        memset(&player->party, 0, sizeof(player->party));
        player->guildId = 0;
        
        // Adicionar às listas
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            _players[playerId] = player;
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByClientMutex);
            _playersByClient[clientId] = playerId;
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByNameMutex);
            _playersByName[player->mob.MobName] = playerId;
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByMapMutex);
            _playersByMap[player->mob.TargetX << 16 | player->mob.TargetY].insert(playerId);
        }
        
        // Atualizar estatísticas
        {
            std::lock_guard<std::mutex> lock(_statsMutex);
            _stats.totalLoaded++;
            _stats.totalConnected++;
            _stats.currentConnected++;
        }
        
        // Disparar eventos
        PlayerManager::_fireEvent(PlayerManagerEventType::PLAYER_LOADED, playerId, clientId, accountId);
        _fireEvent(PlayerManagerEventType::PLAYER_CONNECTED, playerId, clientId, accountId);
        
        LogInfo("Jogador carregado: " + std::string(player->mob.MobName) + " (ID: " + std::to_string(playerId) + ")");
        return playerId;
    }
    
    /**
     * @brief Salva um jogador
     * @param playerId ID do jogador
     * @return true se salvo com sucesso
     */
    bool PlayerManager::SavePlayer(PlayerId playerId) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return false;
        }
        
        // Obter jogador
        std::shared_ptr<WYDPlayer> player = nullptr;
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            auto it = _players.find(playerId);
            if (it == _players.end()) {
                LogError("Jogador não encontrado: " + std::to_string(playerId));
                return false;
            }
            player = it->second;
        }
        
        // Caminho do arquivo do personagem
        std::string filePath = _getCharacterFilePath(player->mob.MobName);
        
        // Salvar dados para arquivo
        /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ofstream
 */

        std::ofstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            LogError("Falha ao abrir arquivo para salvar: " + filePath);
            return false;
        }
        
        // Escrever dados
        file.write(reinterpret_cast<const char*>(&player->mob), sizeof(STRUCT_MOB));
        file.close();
        
        // Atualizar contador de salvamento
        player->saveCount++;
        
        // Atualizar estatísticas
        {
            std::lock_guard<std::mutex> lock(_statsMutex);
            _stats.totalSaved++;
        }
        
        // Disparar evento
        PlayerManager::_fireEvent(PlayerManagerEventType::PLAYER_SAVED, playerId, player->clientId, player->accountId);
        
        LogInfo("Jogador salvo: " + std::string(player->mob.MobName) + " (ID: " + std::to_string(playerId) + ")");
        return true;
    }
    
    /**
     * @brief Salva todos os jogadores
     * @return Número de jogadores salvos
     */
    uint32_t PlayerManager::SaveAllPlayers() {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return 0;
        }
        
        LogInfo("Salvando todos os jogadores...");
        
        uint32_t savedCount = 0;
        std::vector<PlayerId> playerIds;
        
        // Obter lista de IDs
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            for (const auto& playerPair : _players) {
                if (playerPair.second->isActive) {
                    playerIds.push_back(playerPair.first);
                }
            }
        }
        
        // Salvar cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param playerIds Parâmetro playerIds
 * @return Retorna jogador
 */
 jogador
        for(PlayerId playerId : playerIds){
            if (SavePlayer(playerId)) {
                savedCount++;
            }
        }
        
        LogInfo("Jogadores salvos: " + std::to_string(savedCount));
        return savedCount;
    }
    
    /**
     * @brief Remove um jogador
     * @param playerId ID do jogador
     * @param save Se deve salvar antes de remover
     * @return true se removido com sucesso
     */
    bool PlayerManager::RemovePlayer(PlayerId playerId, bool save) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return false;
        }
        
        // Obter jogador
        std::shared_ptr<WYDPlayer> player = nullptr;
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            auto it = _players.find(playerId);
            if (it == _players.end()) {
                LogError("Jogador não encontrado: " + std::to_string(playerId));
                return false;
            }
            player = it->second;
        }
        
        // Verificar se deve /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param save Parâmetro save
 * @return Retorna salvar
 */
 salvar
        if(save){
            SavePlayer(playerId);
        }
        
        // Obter informações para log
        std::string playerName = player->mob.MobName;
        ClientId clientId = player->clientId;
        AccountId accountId = player->accountId;
        
        // Remover das listas
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            _players.erase(playerId);
        }
        
        if (clientId > 0) {
            std::lock_guard<std::mutex> lock(_playersByClientMutex);
            _playersByClient.erase(clientId);
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByNameMutex);
            _playersByName.erase(playerName);
        }
        
        {
            std::lock_guard<std::mutex> lock(_playersByMapMutex);
            auto mapKey = player->mob.TargetX << 16 | player->mob.TargetY;
            auto it = _playersByMap.find(mapKey);
            if (it != _playersByMap.end()) {
                it->second.erase(playerId);
                if (it->second.empty()) {
                    _playersByMap.erase(mapKey);
                }
            }
        }
        
        if (player->party.leaderId > 0) {
            std::lock_guard<std::mutex> lock(_playersByPartyMutex);
            auto it = _playersByParty.find(player->party.leaderId);
            if (it != _playersByParty.end()) {
                it->second.erase(playerId);
                if (it->second.empty()) {
                    _playersByParty.erase(player->party.leaderId);
                }
            }
        }
        
        if (player->guildId > 0) {
            std::lock_guard<std::mutex> lock(_playersByGuildMutex);
            auto it = _playersByGuild.find(player->guildId);
            if (it != _playersByGuild.end()) {
                it->second.erase(playerId);
                if (it->second.empty()) {
                    _playersByGuild.erase(player->guildId);
                }
            }
        }
        
        // Atualizar estatísticas
        {
            std::lock_guard<std::mutex> lock(_statsMutex);
            _stats.totalDestroyed++;
        }
        
        // Disparar evento
        PlayerManager::_fireEvent(PlayerManagerEventType::PLAYER_DESTROYED, playerId, clientId, accountId);
        
        LogInfo("Jogador removido: " + playerName + " (ID: " + std::to_string(playerId) + ")");
        return true;
    }
    
    /**
     * @brief Conecta um jogador
     * @param playerId ID do jogador
     * @param clientId ID do cliente
     * @return true se conectado com sucesso
     */
    bool PlayerManager::Connect(PlayerId playerId, ClientId clientId) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return false;
        }
        
        // Obter jogador
        std::shared_ptr<WYDPlayer> player = nullptr;
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            auto it = _players.find(playerId);
            if (it == _players.end()) {
                LogError("Jogador não encontrado: " + std::to_string(playerId));
                return false;
            }
            player = it->second;
        }
        
        // Verificar se já está /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param player->isOnline Parâmetro player->isOnline
 * @return Retorna online
 */
 online
        if(player->isOnline){
            // Se já está online com o mesmo cliente, não faz /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param clientId Parâmetro clientId
 * @return Retorna nada
 */
 nada
            if(player->clientId == clientId){
                return true;
            }
            
            // Desconectar cliente anterior
            PlayerManager::LogWarning("Jogador já está online, forçando desconexão anterior");
            Disconnect(playerId);
        }
        
        // Atualizar dados
        player->clientId = clientId;
        player->isOnline = true;
        player->lastActivity = static_cast<uint32_t>(time(nullptr));
        
        // Atualizar mapeamento de cliente
        {
            std::lock_guard<std::mutex> lock(_playersByClientMutex);
            _playersByClient[clientId] = playerId;
        }
        
        // Atualizar estatísticas
        {
            std::lock_guard<std::mutex> lock(_statsMutex);
            _stats.totalConnected++;
            _stats.currentConnected++;
        }
        
        // Disparar evento
        PlayerManager::_fireEvent(PlayerManagerEventType::PLAYER_CONNECTED, playerId, clientId, player->accountId);
        
        LogInfo("Jogador conectado: " + std::string(player->mob.MobName) + " (ID: " + std::to_string(playerId) + ")");
        return true;
    }
    
    /**
     * @brief Desconecta um jogador
     * @param playerId ID do jogador
     * @param save Se deve salvar antes de desconectar
     * @return true se desconectado com sucesso
     */
    bool PlayerManager::Disconnect(PlayerId playerId, bool save) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return false;
        }
        
        // Obter jogador
        std::shared_ptr<WYDPlayer> player = nullptr;
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            auto it = _players.find(playerId);
            if (it == _players.end()) {
                LogError("Jogador não encontrado: " + std::to_string(playerId));
                return false;
            }
            player = it->second;
        }
        
        // Verificar se está /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !player->isOnline Parâmetro !player->isOnline
 * @return Retorna online
 */
 online
        if(!player->isOnline){
            LogWarning("Jogador já está offline: " + std::string(player->mob.MobName));
            return true;
        }
        
        // Verificar se deve /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param save Parâmetro save
 * @return Retorna salvar
 */
 salvar
        if(save){
            SavePlayer(playerId);
        }
        
        // Obter informações para log
        ClientId clientId = player->clientId;
        
        // Atualizar dados
        player->isOnline = false;
        
        // Remover mapeamento de cliente
        {
            std::lock_guard<std::mutex> lock(_playersByClientMutex);
            _playersByClient.erase(clientId);
        }
        
        // Atualizar estatísticas
        {
            std::lock_guard<std::mutex> lock(_statsMutex);
            _stats.totalDisconnected++;
            _stats.currentConnected--;
        }
        
        // Disparar evento
        PlayerManager::_fireEvent(PlayerManagerEventType::PLAYER_DISCONNECTED, playerId, clientId, player->accountId);
        
        LogInfo("Jogador desconectado: " + std::string(player->mob.MobName) + " (ID: " + std::to_string(playerId) + ")");
        return true;
    }
    
    /**
     * @brief Atualiza a posição de um jogador
     * @param playerId ID do jogador
     * @param x Nova posição X
     * @param y Nova posição Y
     * @return true se atualizado com sucesso
     */
    bool PlayerManager::UpdatePosition(PlayerId playerId, uint16_t x, uint16_t y) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return false;
        }
        
        // Obter jogador
        std::shared_ptr<WYDPlayer> player = nullptr;
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            auto it = _players.find(playerId);
            if (it == _players.end()) {
                LogError("Jogador não encontrado: " + std::to_string(playerId));
                return false;
            }
            player = it->second;
        }
        
        // Obter posição atual
        uint16_t oldX = player->mob.TargetX;
        uint16_t oldY = player->mob.TargetY;
        
        // Se a posição não mudou, não faz /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param y Parâmetro y
 * @return Retorna nada
 */
 nada
        if(oldX == x && oldY == y){
            return true;
        }
        
        // Remover da posição antiga
        {
            std::lock_guard<std::mutex> lock(_playersByMapMutex);
            auto mapKey = oldX << 16 | oldY;
            auto it = _playersByMap.find(mapKey);
            if (it != _playersByMap.end()) {
                it->second.erase(playerId);
                if (it->second.empty()) {
                    _playersByMap.erase(mapKey);
                }
            }
        }
        
        // Atualizar posição
        player->mob.TargetX = x;
        player->mob.TargetY = y;
        player->lastActivity = static_cast<uint32_t>(time(nullptr));
        
        // Adicionar à nova posição
        {
            std::lock_guard<std::mutex> lock(_playersByMapMutex);
            auto mapKey = x << 16 | y;
            _playersByMap[mapKey].insert(playerId);
        }
        
        LogDebug("Jogador movido: " + std::string(player->mob.MobName) + 
                " de (" + std::to_string(oldX) + "," + std::to_string(oldY) + ") " +
                "para (" + std::to_string(x) + "," + std::to_string(y) + ")");
        return true;
    }
    
    /**
     * @brief Define o grupo de um jogador
     * @param playerId ID do jogador
     * @param partyLeaderId ID do líder do grupo
     * @return true se atualizado com sucesso
     */
    bool PlayerManager::SetParty(PlayerId playerId, PlayerId partyLeaderId) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return false;
        }
        
        // Obter jogador
        std::shared_ptr<WYDPlayer> player = nullptr;
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            auto it = _players.find(playerId);
            if (it == _players.end()) {
                LogError("Jogador não encontrado: " + std::to_string(playerId));
                return false;
            }
            player = it->second;
        }
        
        // Obter ID de grupo atual
        PlayerId oldPartyLeaderId = player->party.leaderId;
        
        // Se o grupo não mudou, não faz /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param partyLeaderId Parâmetro partyLeaderId
 * @return Retorna nada
 */
 nada
        if(oldPartyLeaderId == partyLeaderId){
            return true;
        }
        
        // Remover do grupo /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna anterior
 */
 anterior
        if(oldPartyLeaderId > 0){
            std::lock_guard<std::mutex> lock(_playersByPartyMutex);
            auto it = _playersByParty.find(oldPartyLeaderId);
            if (it != _playersByParty.end()) {
                it->second.erase(playerId);
                if (it->second.empty()) {
                    _playersByParty.erase(oldPartyLeaderId);
                }
            }
        }
        
        // Atualizar grupo
        player->party.leaderId = partyLeaderId;
        
        // Adicionar ao novo /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna grupo
 */
 grupo
        if(partyLeaderId > 0){
            std::lock_guard<std::mutex> lock(_playersByPartyMutex);
            _playersByParty[partyLeaderId].insert(playerId);
        }
        
        // Disparar evento
        PlayerManager::_fireEvent(PlayerManagerEventType::PLAYER_PARTY_CHANGED, playerId, player->clientId, player->accountId);
        
        LogInfo("Grupo do jogador atualizado: " + std::string(player->mob.MobName) + 
               " de " + std::to_string(oldPartyLeaderId) + " para " + std::to_string(partyLeaderId));
        return true;
    }
    
    /**
     * @brief Define a guilda de um jogador
     * @param playerId ID do jogador
     * @param guildId ID da guilda
     * @return true se atualizado com sucesso
     */
    bool PlayerManager::SetGuild(PlayerId playerId, uint32_t guildId) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return false;
        }
        
        // Obter jogador
        std::shared_ptr<WYDPlayer> player = nullptr;
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            auto it = _players.find(playerId);
            if (it == _players.end()) {
                LogError("Jogador não encontrado: " + std::to_string(playerId));
                return false;
            }
            player = it->second;
        }
        
        // Obter ID de guilda atual
        uint32_t oldGuildId = player->guildId;
        
        // Se a guilda não mudou, não faz /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param guildId Parâmetro guildId
 * @return Retorna nada
 */
 nada
        if(oldGuildId == guildId){
            return true;
        }
        
        // Remover da guilda /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna anterior
 */
 anterior
        if(oldGuildId > 0){
            std::lock_guard<std::mutex> lock(_playersByGuildMutex);
            auto it = _playersByGuild.find(oldGuildId);
            if (it != _playersByGuild.end()) {
                it->second.erase(playerId);
                if (it->second.empty()) {
                    _playersByGuild.erase(oldGuildId);
                }
            }
        }
        
        // Atualizar guilda
        player->guildId = guildId;
        
        // Adicionar à nova /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna guilda
 */
 guilda
        if(guildId > 0){
            std::lock_guard<std::mutex> lock(_playersByGuildMutex);
            _playersByGuild[guildId].insert(playerId);
        }
        
        // Disparar evento
        PlayerManager::_fireEvent(PlayerManagerEventType::PLAYER_GUILD_CHANGED, playerId, player->clientId, player->accountId);
        
        LogInfo("Guilda do jogador atualizada: " + std::string(player->mob.MobName) + 
               " de " + std::to_string(oldGuildId) + " para " + std::to_string(guildId));
        return true;
    }
    
    /**
     * @brief Obtém um jogador pelo ID
     * @param playerId ID do jogador
     * @return Ponteiro para o jogador (nullptr se não encontrado)
     */
    std::shared_ptr<WYDPlayer> PlayerManager::GetPlayer(PlayerId playerId) const {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return nullptr;
        }
        
        std::lock_guard<std::mutex> lock(_playersMutex);
        auto it = _players.find(playerId);
        if (it == _players.end()) {
            return nullptr;
        }
        
        return it->second;
    }
    
    /**
     * @brief Obtém um jogador pelo ID do cliente
     * @param clientId ID do cliente
     * @return Ponteiro para o jogador (nullptr se não encontrado)
     */
    std::shared_ptr<WYDPlayer> PlayerManager::GetPlayerByClient(ClientId clientId) const {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return nullptr;
        }
        
        PlayerId playerId = 0;
        {
            std::lock_guard<std::mutex> lock(_playersByClientMutex);
            auto it = _playersByClient.find(clientId);
            if (it == _playersByClient.end()) {
                return nullptr;
            }
            playerId = it->second;
        }
        
        return PlayerManager::GetPlayer(playerId);
    }
    
    /**
     * @brief Obtém um jogador pelo nome
     * @param name Nome do jogador
     * @return Ponteiro para o jogador (nullptr se não encontrado)
     */
    std::shared_ptr<WYDPlayer> PlayerManager::GetPlayerByName(const std::string& name) const {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return nullptr;
        }
        
        PlayerId playerId = 0;
        {
            std::lock_guard<std::mutex> lock(_playersByNameMutex);
            auto it = _playersByName.find(name);
            if (it == _playersByName.end()) {
                return nullptr;
            }
            playerId = it->second;
        }
        
        return PlayerManager::GetPlayer(playerId);
    }
    
    /**
     * @brief Obtém jogadores em uma área
     * @param x1 Coordenada X1
     * @param y1 Coordenada Y1
     * @param x2 Coordenada X2
     * @param y2 Coordenada Y2
     * @return Lista de IDs de jogadores na área
     */
    std::vector<PlayerId> PlayerManager::GetPlayersInArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const {
        std::vector<PlayerId> result;
        
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return result;
        }
        
        // Garantir que x1 <= x2 e y1 <= y2
        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);
        
        // Obter jogadores por posição
        std::lock_guard<std::mutex> lock(_playersByMapMutex);
        for (uint16_t x = x1; x <= x2; x++) {
            for (uint16_t y = y1; y <= y2; y++) {
                auto mapKey = x << 16 | y;
                auto it = _playersByMap.find(mapKey);
                if (it != _playersByMap.end()) {
                    const auto& playerSet = it->second;
                    result.insert(result.end(), playerSet.begin(), playerSet.end());
                }
            }
        }
        
        return result;
    }
    
    /**
     * @brief Obtém jogadores em um raio
     * @param x Coordenada X central
     * @param y Coordenada Y central
     * @param radius Raio
     * @return Lista de IDs de jogadores no raio
     */
    std::vector<PlayerId> PlayerManager::GetPlayersInRadius(uint16_t x, uint16_t y, uint16_t radius) const {
        return PlayerManager::GetPlayersInArea(
            x > radius ? x - radius : 0,
            y > radius ? y - radius : 0,
            x + radius,
            y + radius
        );
    }
    
    /**
     * @brief Obtém jogadores em um grupo
     * @param partyLeaderId ID do líder do grupo
     * @return Lista de IDs de jogadores no grupo
     */
    std::vector<PlayerId> PlayerManager::GetPlayersInParty(PlayerId partyLeaderId) const {
        std::vector<PlayerId> result;
        
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return result;
        }
        
        std::lock_guard<std::mutex> lock(_playersByPartyMutex);
        auto it = _playersByParty.find(partyLeaderId);
        if (it == _playersByParty.end()) {
            return result;
        }
        
        const auto& playerSet = it->second;
        result.insert(result.end(), playerSet.begin(), playerSet.end());
        
        return result;
    }
    
    /**
     * @brief Obtém jogadores em uma guilda
     * @param guildId ID da guilda
     * @return Lista de IDs de jogadores na guilda
     */
    std::vector<PlayerId> PlayerManager::GetPlayersInGuild(uint32_t guildId) const {
        std::vector<PlayerId> result;
        
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return result;
        }
        
        std::lock_guard<std::mutex> lock(_playersByGuildMutex);
        auto it = _playersByGuild.find(guildId);
        if (it == _playersByGuild.end()) {
            return result;
        }
        
        const auto& playerSet = it->second;
        result.insert(result.end(), playerSet.begin(), playerSet.end());
        
        return result;
    }
    
    /**
     * @brief Obtém todos os jogadores
     * @return Lista de IDs de jogadores
     */
    std::vector<PlayerId> PlayerManager::GetAllPlayers() const {
        std::vector<PlayerId> result;
        
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return result;
        }
        
        std::lock_guard<std::mutex> lock(_playersMutex);
        for (const auto& playerPair : _players) {
            result.push_back(playerPair.first);
        }
        
        return result;
    }
    
    /**
     * @brief Obtém todos os jogadores online
     * @return Lista de IDs de jogadores online
     */
    std::vector<PlayerId> PlayerManager::GetOnlinePlayers() const {
        std::vector<PlayerId> result;
        
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return result;
        }
        
        std::lock_guard<std::mutex> lock(_playersMutex);
        for (const auto& playerPair : _players) {
            if (playerPair.second->isOnline) {
                result.push_back(playerPair.first);
            }
        }
        
        return result;
    }
    
    /**
     * @brief Obtém estatísticas do gerenciador
     * @return Estatísticas
     */
    PlayerManagerStats PlayerManager::GetStats() const {
        std::lock_guard<std::mutex> lock(_statsMutex);
        return _stats;
    }
    
    /**
     * @brief Verifica inatividade de jogadores
     * @param currentTime Tempo atual
     * @return Número de jogadores desconectados por inatividade
     */
    uint32_t PlayerManager::CheckPlayerInactivity(uint32_t currentTime) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return 0;
        }
        
        LogDebug("Verificando inatividade de jogadores...");
        
        uint32_t disconnectedCount = 0;
        std::vector<PlayerId> inactivePlayers;
        
        // Obter jogadores inativos
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            for (const auto& playerPair : _players) {
                if (playerPair.second->isOnline) {
                    uint32_t inactiveTime = currentTime - playerPair.second->lastActivity;
                    if (inactiveTime >= _inactivityTime) {
                        inactivePlayers.push_back(playerPair.first);
                    }
                }
            }
        }
        
        // Desconectar jogadores /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param inactivePlayers Parâmetro inactivePlayers
 * @return Retorna inativos
 */
 inativos
        for(PlayerId playerId : inactivePlayers){
            std::shared_ptr<WYDPlayer> player = GetPlayer(playerId);
            if (player) {
                LogWarning("Desconectando jogador inativo: " + std::string(player->mob.MobName) + 
                          " (inativo por " + std::to_string((currentTime - player->lastActivity) / 60) + " minutos)");
                
                if (Disconnect(playerId)) {
                    disconnectedCount++;
                }
            }
        }
        
        if (disconnectedCount > 0) {
            LogInfo("Jogadores desconectados por inatividade: " + std::to_string(disconnectedCount));
        }
        
        return disconnectedCount;
    }
    
    /**
     * @brief Registra um callback para eventos
     * @param callback Função de callback
     * @return ID do callback
     */
    uint32_t PlayerManager::RegisterEventCallback(PlayerManagerEventCallback callback) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return 0;
        }
        
        if (!callback) {
            LogError("Callback inválido");
            return 0;
        }
        
        uint32_t callbackId = _nextCallbackId++;
        
        {
            std::lock_guard<std::mutex> lock(_callbacksMutex);
            _eventCallbacks[callbackId] = callback;
        }
        
        LogDebug("Callback de evento registrado: " + std::to_string(callbackId));
        return callbackId;
    }
    
    /**
     * @brief Remove um callback de eventos
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool PlayerManager::UnregisterEventCallback(uint32_t callbackId) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return false;
        }
        
        {
            std::lock_guard<std::mutex> lock(_callbacksMutex);
            auto it = _eventCallbacks.find(callbackId);
            if (it == _eventCallbacks.end()) {
                LogError("Callback não encontrado: " + std::to_string(callbackId));
                return false;
            }
            
            _eventCallbacks.erase(it);
        }
        
        LogDebug("Callback de evento removido: " + std::to_string(callbackId));
        return true;
    }
    
    /**
     * @brief Registra um callback para erros
     * @param callback Função de callback
     * @return ID do callback
     */
    uint32_t PlayerManager::RegisterErrorCallback(PlayerManagerErrorCallback callback) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return 0;
        }
        
        if (!callback) {
            LogError("Callback inválido");
            return 0;
        }
        
        uint32_t callbackId = _nextCallbackId++;
        
        {
            std::lock_guard<std::mutex> lock(_callbacksMutex);
            _errorCallbacks[callbackId] = callback;
        }
        
        LogDebug("Callback de erro registrado: " + std::to_string(callbackId));
        return callbackId;
    }
    
    /**
     * @brief Remove um callback de erros
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool PlayerManager::UnregisterErrorCallback(uint32_t callbackId) {
        if (!_initialized) {
            LogError("PlayerManager não inicializado");
            return false;
        }
        
        {
            std::lock_guard<std::mutex> lock(_callbacksMutex);
            auto it = _errorCallbacks.find(callbackId);
            if (it == _errorCallbacks.end()) {
                LogError("Callback não encontrado: " + std::to_string(callbackId));
                return false;
            }
            
            _errorCallbacks.erase(it);
        }
        
        LogDebug("Callback de erro removido: " + std::to_string(callbackId));
        return true;
    }
    
    /**
     * @brief Define o modo de debug
     * @param enabled Se o modo de debug está ativado
     */
    void PlayerManager::SetDebugMode(bool enabled) {
        _debugMode = enabled;
        LogInfo("Modo debug " + std::string(enabled ? "ativado" : "desativado"));
    }
    
    /**
     * @brief Define o modo de logging
     * @param enabled Se o logging está ativado
     */
    void PlayerManager::SetLogging(bool enabled) {
        _loggingEnabled = enabled;
        LogInfo("Logging " + std::string(enabled ? "ativado" : "desativado"));
    }
    
    /**
     * @brief Loga uma mensagem de informação
     * @param message Mensagem
     */
    void PlayerManager::LogInfo(const std::string& message) const {
        if (_loggingEnabled) {
            std::cout << "[INFO][PlayerManager] " << message << std::endl;
        }
    }
    
    /**
     * @brief Loga uma mensagem de aviso
     * @param message Mensagem
     */
    void PlayerManager::LogWarning(const std::string& message) const {
        if (_loggingEnabled) {
            std::cout << "[WARN][PlayerManager] " << message << std::endl;
        }
    }
    
    /**
     * @brief Loga uma mensagem de erro
     * @param message Mensagem
     */
    void PlayerManager::LogError(const std::string& message) const {
        if (_loggingEnabled) {
            std::cerr << "[ERROR][PlayerManager] " << message << std::endl;
        }
    }
    
    /**
     * @brief Loga uma mensagem de debug
     * @param message Mensagem
     */
    void PlayerManager::LogDebug(const std::string& message) const {
        if (_loggingEnabled && _debugMode) {
            std::cout << "[DEBUG][PlayerManager] " << message << std::endl;
        }
    }
    
private:
    PlayerManager() : _initialized(false), _debugMode(false), _loggingEnabled(true),
                     _maxPlayers(0), _nextPlayerId(1), _nextCallbackId(1),
                     _autoSaveTimer(0), _inactivityCheckTimer(0),
                     _autoSaveInterval(300), _inactivityCheckInterval(60),
                     _inactivityTime(3600), _autoSaveEnabled(true) {}
    
    ~PlayerManager() {
        Shutdown();
    }
    
    /**
     * @brief Dispara um evento
     * @param type Tipo do evento
     * @param playerId ID do jogador
     * @param clientId ID do cliente
     * @param accountId ID da conta
     * @param data Dados adicionais
     * @param dataSize Tamanho dos dados
     */
    void PlayerManager::_fireEvent(PlayerManagerEventType type, PlayerId playerId, ClientId clientId,
                  AccountId accountId, void* data = nullptr, uint32_t dataSize = 0) {
        if (!_initialized) {
            return;
        }
        
        // Criar evento
        PlayerManagerEvent PlayerManager::event(type, playerId, clientId, accountId, data, dataSize);
        
        // Copiar callbacks para evitar deadlock
        std::vector<PlayerManagerEventCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(_callbacksMutex);
            for (const auto& cbPair : _eventCallbacks) {
                callbacks.push_back(cbPair.second);
            }
        }
        
        // Chamar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param callbacks Parâmetro callbacks
 * @return Retorna callbacks
 */
 callbacks
        for(const auto& callback : callbacks){
            callback(event);
        }
    }
    
    /**
     * @brief Garante que um diretório existe
     * @param path Caminho
     * @return true se o diretório existe ou foi criado
     */
    bool PlayerManager::_ensureDirectory(const std::string& path) {
        struct stat info;
        if (stat(path.c_str(), &info) != 0) {
            // Diretório não existe, tentar criar
#ifdef _WIN32
            int result = mkdir(path.c_str());
#else
            int result = mkdir(path.c_str(), 0755);
#/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna endif
 */
endif
            if(result != 0){
                LogError("Falha ao criar diretório: " + path);
                return false;
            }
        } else if (!(info.st_mode & S_IFDIR)) {
            // Caminho existe, mas não é um diretó/**
 * LogError
 * 
 * Implementa a funcionalidade LogError conforme especificação original.
 * @param path Parâmetro path
 * @return Retorna rio
 */
rio
            LogError("Caminho não é um diretório: " + path);
            return false;
        }
        
        return true;
    }
    
    /**
     * @brief Obtém o caminho do arquivo de um personagem
     * @param characterName Nome do personagem
     * @return Caminho do arquivo
     */
    std::string _getCharacterFilePath(const std::string& characterName) const {
        return _basePath + "/" + characterName + ".dat";
    }
    
    // Atributos
    bool _initialized;
    bool _debugMode;
    std::atomic<bool> _loggingEnabled;
    std::string _basePath;
    uint32_t _maxPlayers;
    
    // Containers
    mutable std::mutex _playersMutex;
    std::unordered_map<PlayerId, std::shared_ptr<WYDPlayer>> _players;
    
    mutable std::mutex _playersByClientMutex;
    std::unordered_map<ClientId, PlayerId> _playersByClient;
    
    mutable std::mutex _playersByNameMutex;
    std::unordered_map<std::string, PlayerId> _playersByName;
    
    mutable std::mutex _playersByMapMutex;
    std::unordered_map<uint32_t, std::set<PlayerId>> _playersByMap;
    
    mutable std::mutex _playersByPartyMutex;
    std::unordered_map<PlayerId, std::set<PlayerId>> _playersByParty;
    
    mutable std::mutex _playersByGuildMutex;
    std::unordered_map<uint32_t, std::set<PlayerId>> _playersByGuild;
    
    // Estatísticas
    mutable std::mutex _statsMutex;
    PlayerManagerStats _stats;
    
    // Próximo ID de jogador
    std::atomic<PlayerId> _nextPlayerId;
    
    // Callbacks de evento e erro
    mutable std::mutex _callbacksMutex;
    std::unordered_map<uint32_t, PlayerManagerEventCallback> _eventCallbacks;
    std::unordered_map<uint32_t, PlayerManagerErrorCallback> _errorCallbacks;
    
    // Próximo ID de callback
    std::atomic<uint32_t> _nextCallbackId;
    
    // Timers
    uint32_t _autoSaveTimer;
    uint32_t _inactivityCheckTimer;
    uint32_t _autoSaveInterval;
    uint32_t _inactivityCheckInterval;
    uint32_t _inactivityTime;
    bool _autoSaveEnabled;
};

} // namespace tmsrv
} // namespace wydbr