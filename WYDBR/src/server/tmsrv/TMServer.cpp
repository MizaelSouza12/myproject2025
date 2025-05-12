/**
 * TMServer.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/TMServer.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "TMServer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <cstring>
#include <algorithm>
#include <iomanip>

#include "../../core/Logger.h"
#include "../../core/Utils.h"
#include "../network/NetworkManager.h"
#include "../network/PacketBuilder.h"
#include "player/Player.h"
#include "world/World.h"
#include "world/Zone.h"
#include "TMConfig.h"

namespace wydbr {


/**
 * @file TMServer.cpp
 * @brief Implementação da classe principal do TM Server
 * 
 * Este arquivo contém a implementação da classe CTMServer,
 * seguindo a estrutura original do WYD, mas com melhorias
 * e correções de bugs conhecidos.
 */

namespace wyd {

} // namespace wydbr

namespace server {

// Definição dos membros estáticos
CTMServer& CTMServer::GetInstance() {
    static CTMServer instance;
    return instance;
}

CTMServer::CTMServer()
    : isRunning_(false)
    , isInitialized_(false)
    , tickCount_(0)
    , packetCount_(0)
    , activeConnectionCount_(0)
    , totalPacketsProcessed_(0)
{
}

CTMServer::~CTMServer() {
    if (isRunning_.load()) {
        Stop();
    }
}

bool CTMServer::Initialize(const char* configFile) {
    // Já inicializado?
    if (isInitialized_.load()) {
        LOG_WARNING("TMServer já inicializado");
        return true;
    }
    
    // Carrega configuração
    if (!CTMConfig::GetInstance().Load(configFile)) {
        LOG_ERROR("Falha ao carregar configuração: {}", configFile);
        return false;
    }
    
    LOG_INFO("Inicializando TMServer (versão WYD 2.0)");
    LOG_INFO("Nome do servidor: {}", g_Config.szServerName);
    LOG_INFO("Modo do servidor: {}", g_Config.GetServerModeString());
    LOG_INFO("Porta: {}", g_Config.wTMServerPort);
    LOG_INFO("Capacidade máxima: {} jogadores", g_Config.dwMaxUsers);
    
    try {
        // Inicializa gerenciador de sockets
        if (!sockManager_.Initialize()) {
            LOG_ERROR("Falha ao inicializar gerenciador de sockets");
            return false;
        }
        
        // Inicializa cliente DB
        pDBClient_ = std::make_unique<CDBClient>();
        if (!pDBClient_->Initialize(g_Config.szDBServerIP, g_Config.wDBServerPort)) {
            LOG_ERROR("Falha ao inicializar cliente DB");
            return false;
        }
        
        // Inicializa mundo
        pWorld_ = std::make_unique<CWorld>();
        if (!pWorld_->Initialize()) {
            LOG_ERROR("Falha ao inicializar mundo");
            return false;
        }
        
        // Inicializa handler de /**
 * InitializePacketHandler
 * 
 * Implementa a funcionalidade InitializePacketHandler conforme especificação original.
 * @return Retorna pacotes
 */
 pacotes
        InitializePacketHandler();
        
        // Carrega tabelas de referência
        // TODO: Implementar carregamento de tabelas de experiência, pontos de atributos, etc.
        
        isInitialized_ = true;
        LOG_INFO("TMServer inicializado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção durante inicialização: {}", e.what());
        return false;
    }
}

bool CTMServer::Start() {
    // Já rodando?
    if (isRunning_.load()) {
        LOG_WARNING("TMServer já está rodando");
        return true;
    }
    
    // Inicializado?
    if (!isInitialized_.load()) {
        LOG_ERROR("TMServer não inicializado");
        return false;
    }
    
    LOG_INFO("Iniciando TMServer...");
    
    try {
        // Inicia escuta na porta configurada
        if (!sockManager_.StartListen(g_Config.wTMServerPort)) {
            LOG_ERROR("Falha ao iniciar escuta na porta {}", g_Config.wTMServerPort);
            return false;
        }
        
        // Conecta ao servidor DB
        if (!ConnectToDBServer()) {
            LOG_ERROR("Falha ao conectar ao servidor DB");
            return false;
        }
        
        // Inicia o mundo
        if (!pWorld_->Start()) {
            LOG_ERROR("Falha ao iniciar o mundo");
            return false;
        }
        
        // Marca início da execução
        startTime_ = std::chrono::steady_clock::now();
        isRunning_ = true;
        
        // Inicia threads
        acceptThread_ = std::thread(&CTMServer::AcceptThread, this);
        updateThread_ = std::thread(&CTMServer::UpdateThread, this);
        saveThread_ = std::thread(&CTMServer::SaveThread, this);
        
        LOG_INFO("TMServer iniciado com sucesso");
        LOG_INFO("Ouvindo na porta: {}", g_Config.wTMServerPort);
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção durante inicialização: {}", e.what());
        isRunning_ = false;
        return false;
    }
}

bool CTMServer::Stop() {
    // Não está rodando?
    if (!isRunning_.load()) {
        return true;
    }
    
    LOG_INFO("Parando TMServer...");
    
    // Marca flag de parada
    isRunning_ = false;
    
    // Espera threads terminarem
    if (acceptThread_.joinable()) {
        acceptThread_.join();
    }
    
    if (updateThread_.joinable()) {
        updateThread_.join();
    }
    
    if (saveThread_.joinable()) {
        saveThread_.join();
    }
    
    // Salva todos os /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param jogadores..." Parâmetro jogadores..."
 * @return Retorna jogadores
 */
 jogadores
    LOG_INFO("Salvando todos os jogadores...");
    int savedCount = SaveAllPlayers(true);
    LOG_INFO("{} jogadores salvos", savedCount);
    
    // Para o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param pWorld_ Parâmetro pWorld_
 * @return Retorna mundo
 */
 mundo
    if(pWorld_){
        pWorld_->Stop();
    }
    
    // Desconecta do /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param pDBClient_ Parâmetro pDBClient_
 * @return Retorna DB
 */
 DB
    if(pDBClient_){
        pDBClient_->Disconnect();
    }
    
    // Para o socket
    sockManager_.StopListen();
    
    LOG_INFO("TMServer parado com sucesso");
    return true;
}

void CTMServer::Update(DWORD deltaTime) {
    // Não está rodando?
    if (!isRunning_.load()) {
        return;
    }
    
    // Incrementa contador de ticks
    tickCount_++;
    
    // Atualiza /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param pWorld_ Parâmetro pWorld_
 * @return Retorna mundo
 */
 mundo
    if(pWorld_){
        pWorld_->Update(deltaTime);
    }
    
    // Atualiza jogadores
    {
        std::lock_guard<std::mutex> lock(playerMutex_);
        for (auto& pair : players_) {
            if (pair.second) {
                pair.second->Update(deltaTime);
            }
        }
    }
    
    // Verifica conexões /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna inativas
 */
 inativas
    if(tickCount_ % 200 == 0){ // A cada 10 segundos (200 * 50ms)
        CheckInactiveConnections();
    }
    
    // Registra estatí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna sticas
 */
sticas
    if(tickCount_ % 1200 == 0){ // A cada minuto (1200 * 50ms)
        LOG_INFO("Estatísticas: {} jogadores online, {} pacotes processados",
            GetOnlinePlayerCount(), totalPacketsProcessed_.load());
    }
}

DWORD CTMServer::GetUptime() const {
    if (!isRunning_.load()) {
        return 0;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
    return static_cast<DWORD>(uptime);
}

WORD CTMServer::GetOnlinePlayerCount() const {
    std::lock_guard<std::mutex> lock(playerMutex_);
    return static_cast<WORD>(players_.size());
}

int CTMServer::BroadcastPacket(const Packet& packet, CPlayer* excludePlayer) {
    int count = 0;
    std::lock_guard<std::mutex> lock(playerMutex_);
    
    for (auto& pair : players_) {
        auto player = pair.second;
        if (player && player.get() != excludePlayer) {
            if (sockManager_.SendPacket(pair.first, packet)) {
                count++;
            }
        }
    }
    
    return count;
}

int CTMServer::BroadcastPacketArea(const Packet& packet, WORD mapID, WORD centerX, WORD centerY, WORD radius, CPlayer* excludePlayer) {
    int count = 0;
    std::lock_guard<std::mutex> lock(playerMutex_);
    
    for (auto& pair : players_) {
        auto player = pair.second;
        if (!player || player.get() == excludePlayer) {
            continue;
        }
        
        // Verifica se está no mesmo mapa
        if (player->GetMapID() != mapID) {
            continue;
        }
        
        // Verifica se está dentro do raio
        WORD playerX = player->GetPosX();
        WORD playerY = player->GetPosY();
        
        int dx = static_cast<int>(playerX) - static_cast<int>(centerX);
        int dy = static_cast<int>(playerY) - static_cast<int>(centerY);
        int distanceSquared = dx * dx + dy * dy;
        
        if (distanceSquared <= radius * radius) {
            if (sockManager_.SendPacket(pair.first, packet)) {
                count++;
            }
        }
    }
    
    return count;
}

int CTMServer::BroadcastPacketMap(const Packet& packet, WORD mapID, CPlayer* excludePlayer) {
    int count = 0;
    std::lock_guard<std::mutex> lock(playerMutex_);
    
    for (auto& pair : players_) {
        auto player = pair.second;
        if (!player || player.get() == excludePlayer) {
            continue;
        }
        
        // Verifica se está no mesmo mapa
        if (player->GetMapID() == mapID) {
            if (sockManager_.SendPacket(pair.first, packet)) {
                count++;
            }
        }
    }
    
    return count;
}

bool CTMServer::AddPlayer(std::shared_ptr<CPlayer> player) {
    if (!player) {
        return false;
    }
    
    SOCKET clientID = player->GetSocket();
    std::string playerName = player->GetName();
    DWORD playerID = player->GetID();
    
    std::lock_guard<std::mutex> lock(playerMutex_);
    
    // Já existe outro jogador com este socket?
    auto it = players_.find(clientID);
    if (it != players_.end()) {
        LOG_WARNING("Socket {} já está em uso por outro jogador", clientID);
        return false;
    }
    
    // Verifica limite de jogadores
    if (players_.size() >= g_Config.dwMaxUsers) {
        LOG_WARNING("Limite de jogadores atingido: {}", g_Config.dwMaxUsers);
        return false;
    }
    
    // Adiciona ao mapa de jogadores
    players_[clientID] = player;
    
    // Adiciona ao mapa de nomes
    if (!playerName.empty()) {
        playerNameMap_[playerName] = playerID;
    }
    
    // Adiciona ao /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param pWorld_ Parâmetro pWorld_
 * @return Retorna mundo
 */
 mundo
    if(pWorld_){
        pWorld_->AddPlayer(player.get());
    }
    
    LOG_INFO("Jogador adicionado: {} (ID: {}, Socket: {})", 
        player->GetName(), player->GetID(), clientID);
    
    return true;
}

bool CTMServer::RemovePlayer(CPlayer* player) {
    if (!player) {
        return false;
    }
    
    SOCKET clientID = player->GetSocket();
    return RemovePlayerByID(player->GetID());
}

bool CTMServer::RemovePlayerByID(DWORD playerID) {
    std::lock_guard<std::mutex> lock(playerMutex_);
    
    // Procura o jogador pelo ID
    auto it = std::find_if(players_.begin(), players_.end(),
        [playerID](const auto& pair) {
            return pair.second && pair.second->GetID() == playerID;
        });
    
    if (it == players_.end()) {
        return false;
    }
    
    auto player = it->second;
    SOCKET clientID = it->first;
    
    // Remove do /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param player Parâmetro player
 * @return Retorna mundo
 */
 mundo
    if(pWorld_ && player){
        pWorld_->RemovePlayer(player.get());
    }
    
    // Remove do mapa de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param player Parâmetro player
 * @return Retorna nomes
 */
 nomes
    if(player){
        std::string playerName = player->GetName();
        if (!playerName.empty()) {
            playerNameMap_.erase(playerName);
        }
    }
    
    // Remove do mapa de jogadores
    players_.erase(it);
    
    // Fecha a conexão do socket
    sockManager_.CloseSocket(clientID);
    
    LOG_INFO("Jogador removido: ID {} (Socket: {})", playerID, clientID);
    
    return true;
}

std::shared_ptr<CPlayer> CTMServer::GetPlayerByID(DWORD playerID) {
    std::lock_guard<std::mutex> lock(playerMutex_);
    
    auto it = std::find_if(players_.begin(), players_.end(),
        [playerID](const auto& pair) {
            return pair.second && pair.second->GetID() == playerID;
        });
    
    if (it != players_.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::shared_ptr<CPlayer> CTMServer::GetPlayerByName(const char* playerName) {
    if (!playerName || playerName[0] == '\0') {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(playerMutex_);
    
    // Busca pelo mapa de nomes (mais eficiente)
    auto it = playerNameMap_.find(playerName);
    if (it != playerNameMap_.end()) {
        DWORD playerID = it->second;
        /**
 * GetPlayerByID
 * 
 * Implementa a funcionalidade GetPlayerByID conforme especificação original.
 * @param playerID Parâmetro playerID
 * @return Retorna return
 */

        return GetPlayerByID(playerID);
    }
    
    return nullptr;
}

void CTMServer::ProcessPacket(SOCKET clientID, const Packet& packet) {
    if (!isRunning_.load()) {
        return;
    }
    
    // Incrementa contador de pacotes
    packetCount_++;
    totalPacketsProcessed_++;
    
    try {
        // Processa o pacote usando o handler
        packetHandler_.ProcessPacket(clientID, packet);
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao processar pacote: {}", e.what());
        // Fecha a conexão em caso de /**
 * CloseConnection
 * 
 * Implementa a funcionalidade CloseConnection conforme especificação original.
 * @param clientID Parâmetro clientID
 * @return Retorna erro
 */
 erro
        CloseConnection(clientID);
    }
}

bool CTMServer::ConnectToDBServer() {
    if (!pDBClient_) {
        LOG_ERROR("Cliente DB não inicializado");
        return false;
    }
    
    // Conecta ao servidor DB
    if (!pDBClient_->Connect()) {
        LOG_ERROR("Falha ao conectar ao servidor DB: {}:{}", 
            g_Config.szDBServerIP, g_Config.wDBServerPort);
        return false;
    }
    
    LOG_INFO("Conectado ao servidor DB: {}:{}", 
        g_Config.szDBServerIP, g_Config.wDBServerPort);
        
    return true;
}

int CTMServer::SaveAllPlayers(bool force) {
    std::lock_guard<std::mutex> lock(playerMutex_);
    
    int count = 0;
    for (auto& pair : players_) {
        auto player = pair.second;
        if (player) {
            // Salva o jogador se forçado ou se necessário
            if (force || player->NeedsSave()) {
                if (player->Save()) {
                    count++;
                }
            }
        }
    }
    
    return count;
}

bool CTMServer::ProcessGMCommand(CPlayer* player, const char* command, const char* params) {
    if (!player || !command || !params) {
        return false;
    }
    
    // Apenas GMs podem usar comandos
    if (!player->IsGM()) {
        return false;
    }
    
    LOG_INFO("Comando GM: {} por {}", command, player->GetName());
    
    // Processa o comando
    if (strcmp(command, "teleport") == 0) {
        // Formato: teleport <mapID> <x> <y>
        WORD mapID = 0, x = 0, y = 0;
        if (sscanf(params, "%hu %hu %hu", &mapID, &x, &y) == 3) {
            player->Teleport(mapID, x, y);
            return true;
        }
    }
    else if (strcmp(command, "broadcast") == 0) {
        // Formato: broadcast <message>
        if (params[0] != '\0') {
            SendGlobalMessage(params);
            return true;
        }
    }
    // TODO: Implementar mais comandos
    
    return false;
}

int CTMServer::SendGlobalMessage(const char* message, BYTE type) {
    if (!message || message[0] == '\0') {
        return 0;
    }
    
    // Cria um pacote de chat
    Packet packet;
    // TODO: Implementar construção de pacote de chat
    
    // Envia para todos os jogadores
    /**
 * BroadcastPacket
 * 
 * Implementa a funcionalidade BroadcastPacket conforme especificação original.
 * @param packet Parâmetro packet
 * @return Retorna return
 */

    return BroadcastPacket(packet);
}

bool CTMServer::LogServerStats(const char* fileName) {
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param fileName Parâmetro fileName
 * @param std::ios::app Parâmetro std::ios::app
 * @return Retorna std::ofstream
 */

    std::ofstream file(fileName, std::ios::app);
    if (!file.is_open()) {
        LOG_ERROR("Falha ao abrir arquivo de log: {}", fileName);
        return false;
    }
    
    // Obtém data e hora atual
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime_r(&now_c), "%Y-%m-%d %H:%M:%S");
    
    // Escreve estatísticas
    file << "[" << ss.str() << "] ";
    file << "Players: " << GetOnlinePlayerCount() << "/" << GetMaxPlayerCount() << ", ";
    file << "Uptime: " << GetUptime() << "s, ";
    file << "Packets: " << totalPacketsProcessed_.load() << std::endl;
    
    return true;
}

void CTMServer::InitializePacketHandler() {
    LOG_INFO("Inicializando handler de pacotes");
    
    // TODO: Registrar handlers para cada tipo de /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param inicializado" Parâmetro inicializado"
 * @return Retorna pacote
 */
 pacote
    
    LOG_INFO("Handler de pacotes inicializado");
}

void CTMServer::AcceptThread() {
    LOG_INFO("Thread de aceitação iniciada");
    
    while (isRunning_.load()) {
        // Aceita novas conexões
        SOCKET clientSocket = sockManager_.Accept();
        if (clientSocket != INVALID_SOCKET) {
            LOG_INFO("Nova conexão: Socket {}", clientSocket);
            
            // TODO: Inicializar estruturas para o novo cliente
            // Por enquanto, apenas aceita a conexão
        }
        
        // Dorme por um curto período para não consumir 100% da CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    LOG_INFO("Thread de aceitação finalizada");
}

void CTMServer::UpdateThread() {
    LOG_INFO("Thread de atualização iniciada");
    
    /**
 * tickDuration
 * 
 * Implementa a funcionalidade tickDuration conforme especificação original.
 * @param TICK_RATE Parâmetro TICK_RATE
 * @return Retorna std::chrono::milliseconds
 */

    
    const std::chrono::milliseconds tickDuration(TICK_RATE);
    auto lastTick = std::chrono::steady_clock::now();
    
    while (isRunning_.load()) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);
        
        // Atualiza o servidor
        Update(static_cast<DWORD>(elapsed.count()));
        
        // Calcula tempo restante para o próximo tick
        auto endTick = std::chrono::steady_clock::now();
        auto tickTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTick - lastTick);
        auto sleepTime = tickDuration - tickTime;
        
        // Se ainda há tempo, dorme
        if (sleepTime.count() > 0) {
            std::this_thread::sleep_for(sleepTime);
        }
        
        // Atualiza timestamp do último tick
        lastTick = std::chrono::steady_clock::now();
    }
    
    LOG_INFO("Thread de atualização finalizada");
}

void CTMServer::SaveThread() {
    LOG_INFO("Thread de salvamento iniciada");
    
    while (isRunning_.load()) {
        // Dorme por 5 /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param isRunning_.load( Parâmetro isRunning_.load(
 * @return Retorna minutos
 */
 minutos
        for(int i = 0; i < 300 && isRunning_.load(); i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        // Salva todos os jogadores
        if (isRunning_.load()) {
            int count = SaveAllPlayers();
            if (count > 0) {
                LOG_INFO("{} jogadores salvos", count);
            }
        }
    }
    
    LOG_INFO("Thread de salvamento finalizada");
}

void CTMServer::CheckInactiveConnections() {
    std::lock_guard<std::mutex> lock(playerMutex_);
    
    auto now = std::chrono::steady_clock::now();
    std::vector<SOCKET> socketsToClose;
    
    for (auto& pair : players_) {
        SOCKET clientID = pair.first;
        auto player = pair.second;
        
        if (!player) {
            // Jogador inválido, fecha a conexão
            socketsToClose.push_back(clientID);
            continue;
        }
        
        // Verifica timeout
        auto lastActivity = player->GetLastActivityTime();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastActivity).count();
        
        if (elapsed > g_Config.dwUserDisconnectTimeout) {
            LOG_INFO("Timeout para jogador {}: {} ms (limite: {})", 
                player->GetName(), elapsed, g_Config.dwUserDisconnectTimeout);
            socketsToClose.push_back(clientID);
        }
    }
    
    // Fecha as conexões /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param socketsToClose Parâmetro socketsToClose
 * @return Retorna inativas
 */
 inativas
    for(SOCKET clientID : socketsToClose){
        CloseConnection(clientID);
    }
    
    if (!socketsToClose.empty()) {
        LOG_INFO("{} conexões inativas fechadas", socketsToClose.size());
    }
}

void CTMServer::CloseConnection(SOCKET clientID) {
    // Busca o jogador
    std::shared_ptr<CPlayer> player;
    {
        std::lock_guard<std::mutex> lock(playerMutex_);
        auto it = players_.find(clientID);
        if (it != players_.end()) {
            player = it->second;
        }
    }
    
    // Remove o jogador (se existir)
    if (player) {
        LOG_INFO("Fechando conexão para jogador {}", player->GetName());
        RemovePlayer(player.get());
    }
    else {
        // Fecha a conexão /**
 * LOG_INFO
 * 
 * Implementa a funcionalidade LOG_INFO conforme especificação original.
 * @param {}" Parâmetro {}"
 * @param clientID Parâmetro clientID
 * @return Retorna diretamente
 */
 diretamente
        LOG_INFO("Fechando conexão para socket {}", clientID);
        sockManager_.CloseSocket(clientID);
    }
}

} // namespace server
} // namespace wyd