/**
 * TMServer.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/tmsrv/TMServer.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "TMServer.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <ctime>

namespace wydbr {
namespace tmsrv {

TMServer::TMServer(const std::string& configPath)
    : m_running(false), m_shuttingDown(false), m_connectionCounter(0), 
      m_activeConnections(0), m_bytesReceived(0), m_bytesSent(0), m_packetsProcessed(0) {
    
    m_config = std::make_unique<TMConfig>(configPath);
}

TMServer::~TMServer() {
    shutdown();
}

bool TMServer::initialize() {
    if (m_running) {
        logEvent(1, "Tentativa de inicializar servidor já em execução");
        return false;
    }
    
    // Carrega configurações
    if (!loadConfig(m_config->getConfigPath())) {
        logEvent(3, "Falha ao carregar configurações do servidor: " + m_config->getConfigPath());
        return false;
    }
    
    logEvent(0, "Configurações carregadas com sucesso");
    
    // Inicializa subsistemas
    if (!initializeSubsystems()) {
        logEvent(3, "Falha ao inicializar subsistemas do servidor");
        return false;
    }
    
    logEvent(0, "Subsistemas inicializados com sucesso");
    
    // Registra o tempo de início
    m_startTime = std::chrono::system_clock::now();
    
    return true;
}

bool TMServer::start() {
    if (m_running) {
        logEvent(1, "Tentativa de iniciar servidor já em execução");
        return false;
    }
    
    m_running = true;
    m_shuttingDown = false;
    
    // Inicia a thread principal do servidor
    m_mainThread = std::make_unique<std::thread>(&TMServer::serverMainLoop, this);
    
    // Inicia a thread de rede
    m_networkThread = std::make_unique<std::thread>(&TMServer::processIncomingConnections, this);
    
    logEvent(0, "Servidor iniciado com sucesso");
    
    return true;
}

void TMServer::shutdown() {
    if (!m_running) {
        return;
    }
    
    logEvent(0, "Iniciando desligamento do servidor...");
    
    m_shuttingDown = true;
    m_running = false;
    
    // Espera as threads terminarem
    if (m_mainThread && m_mainThread->joinable()) {
        m_mainThread->join();
    }
    
    if (m_networkThread && m_networkThread->joinable()) {
        m_networkThread->join();
    }
    
    // Salva todos os /**
 * saveAllPlayers
 * 
 * Implementa a funcionalidade saveAllPlayers conforme especificação original.
 * @return Retorna jogadores
 */
 jogadores
    saveAllPlayers();
    
    // Limpa recursos
    m_players.clear();
    m_world.reset();
    m_databaseClient.reset();
    m_networkAdapter.reset();
    m_eventBus.reset();
    
    logEvent(0, "Servidor desligado com sucesso");
}

bool TMServer::isRunning() const {
    return m_running;
}

std::map<std::string, std::string> TMServer::getStats() const {
    std::map<std::string, std::string> stats;
    
    // Informações de tempo
    auto now = std::chrono::system_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count();
    
    auto uptimeStr = [uptime]() -> std::string {
        std::ostringstream oss;
        int days = uptime / 86400;
        int hours = (uptime % 86400) / 3600;
        int minutes = (uptime % 3600) / 60;
        int seconds = uptime % 60;
        
        oss << days << "d " << std::setfill('0') << std::setw(2) << hours << "h "
            << std::setfill('0') << std::setw(2) << minutes << "m "
            << std::setfill('0') << std::setw(2) << seconds << "s";
        return oss.str();
    };
    
    // Timestamp atual
    auto currentTimeStr = [now]() -> std::string {
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime_r(&time_t_now), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    };
    
    stats["version"] = SERVER_VERSION;
    stats["uptime"] = uptimeStr();
    stats["current_time"] = currentTimeStr();
    stats["start_time"] = [this]() -> std::string {
        auto time_t_start = std::chrono::system_clock::to_time_t(m_startTime);
        std::ostringstream oss;
        oss << std::put_time(std::localtime_r(&time_t_start), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }();
    
    // Informações de conexão
    stats["connections_total"] = std::to_string(m_connectionCounter);
    stats["connections_active"] = std::to_string(m_activeConnections);
    stats["bytes_received"] = std::to_string(m_bytesReceived);
    stats["bytes_sent"] = std::to_string(m_bytesSent);
    stats["packets_processed"] = std::to_string(m_packetsProcessed);
    
    // Informações do /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_world Parâmetro m_world
 * @return Retorna mundo
 */
 mundo
    if(m_world){
        stats["world_name"] = m_world->getName();
        stats["players_online"] = std::to_string(m_world->getPlayerCount());
        stats["mobs_active"] = std::to_string(m_world->getMobCount());
        stats["items_dropped"] = std::to_string(m_world->getDroppedItemCount());
    }
    
    return stats;
}

void TMServer::performMaintenance(bool fullMaintenance) {
    logEvent(0, "Iniciando manutenção do servidor" + std::string(fullMaintenance ? " (completa)" : ""));
    
    // Salva todos os /**
 * saveAllPlayers
 * 
 * Implementa a funcionalidade saveAllPlayers conforme especificação original.
 * @return Retorna jogadores
 */
 jogadores
    saveAllPlayers();
    
    if (fullMaintenance) {
        // Executa manutenção completa (limpeza de logs, otimização de banco de dados, etc.)
        if (m_databaseClient) {
            m_databaseClient->optimizeTables();
        }
        
        // Limpa arquivos de log antigos
        // [Implementação específica]
    }
    
    logEvent(0, "Manutenção concluída com sucesso");
}

bool TMServer::loadConfig(const std::string& configPath) {
    if (!m_config->load()) {
        logEvent(3, "Falha ao carregar arquivo de configuração: " + configPath);
        return false;
    }
    
    return true;
}

bool TMServer::initializeSubsystems() {
    try {
        // 1. Inicializa o EventBus
        m_eventBus = std::make_unique<core::EventBus>();
        if (!m_eventBus) {
            logEvent(3, "Falha ao criar EventBus");
            return false;
        }
        
        // 2. Inicializa o NetworkAdapter
        int port = m_config->getInt("server_port", TM_CONNECTION_PORT);
        m_networkAdapter = std::make_unique<network::NetworkAdapter>(port);
        if (!m_networkAdapter || !m_networkAdapter->initialize()) {
            logEvent(3, "Falha ao inicializar NetworkAdapter na porta " + std::to_string(port));
            return false;
        }
        
        // 3. Inicializa o DatabaseClient
        std::string dbHost = m_config->getString("db_host", "localhost");
        int dbPort = m_config->getInt("db_port", 3306);
        std::string dbName = m_config->getString("db_name", "wydbrasil");
        std::string dbUser = m_config->getString("db_user", "root");
        std::string dbPassword = m_config->getString("db_password", "");
        
        m_databaseClient = std::make_unique<database::DatabaseClient>(
            dbHost, dbPort, dbName, dbUser, dbPassword);
        if (!m_databaseClient || !m_databaseClient->connect()) {
            logEvent(3, "Falha ao conectar ao banco de dados");
            return false;
        }
        
        // 4. Inicializa o World
        std::string worldName = m_config->getString("world_name", "WYDBRASIL");
        m_world = std::make_unique<World>(worldName);
        if (!m_world || !m_world->initialize()) {
            logEvent(3, "Falha ao inicializar o mundo: " + worldName);
            return false;
        }
        
        // Carrega mapas
        auto mapList = m_config->getStringList("map_files");
        for (const auto& mapFile : mapList) {
            size_t separatorPos = mapFile.find(':');
            if (separatorPos != std::string::npos) {
                std::string mapIdStr = mapFile.substr(0, separatorPos);
                std::string fileName = mapFile.substr(separatorPos + 1);
                
                int mapId = std::stoi(mapIdStr);
                if (!m_world->loadMap(mapId, fileName)) {
                    logEvent(2, "Falha ao carregar mapa: " + fileName);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        logEvent(3, "Exceção durante inicialização de subsistemas: " + std::string(e.what()));
        return false;
    }
}

void TMServer::serverMainLoop() {
    logEvent(0, "Thread principal do servidor iniciada");
    
    auto lastUpdateTime = std::chrono::high_resolution_clock::now();
    
    while (m_running) {
        // Calcula o delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastUpdateTime).count();
        lastUpdateTime = currentTime;
        
        // Atualiza sistemas do /**
 * updateGameSystems
 * 
 * Implementa a funcionalidade updateGameSystems conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 * @return Retorna jogo
 */
 jogo
        updateGameSystems(deltaTime);
        
        // Pequena pausa para não sobrecarregar o CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    logEvent(0, "Thread principal do servidor finalizada");
}

void TMServer::processIncomingConnections() {
    logEvent(0, "Thread de processamento de conexões iniciada");
    
    while (m_running) {
        // Verifica novas conexões
        auto newConnections = m_networkAdapter->acceptNewConnections();
        
        for (auto& connection : newConnections) {
            m_connectionCounter++;
            m_activeConnections++;
            
            // Cria um novo jogador para cada conexão
            uint32_t playerID = static_cast<uint32_t>(m_connectionCounter);
            auto player = std::make_shared<WYDPlayer>(playerID, connection);
            
            if (player->initialize()) {
                std::lock_guard<std::mutex> lock(m_playersMutex);
                m_players[playerID] = player;
                
                logEvent(0, "Nova conexão aceita: " + player->getIPAddress() + " (ID: " + std::to_string(playerID) + ")");
            } else {
                logEvent(1, "Falha ao inicializar jogador para conexão: " + 
                    std::to_string(playerID) + " de " + connection->getIP());
                m_activeConnections--;
            }
        }
        
        // Processa conexões existentes
        std::vector<uint32_t> disconnectedPlayers;
        
        {
            std::lock_guard<std::mutex> lock(m_playersMutex);
            for (auto& pair : m_players) {
                auto& player = pair.second;
                
                if (!player->isConnected()) {
                    disconnectedPlayers.push_back(pair.first);
                    continue;
                }
                
                // Processa pacotes de entrada
                try {
                    player->processIncomingPackets();
                } catch (const std::exception& e) {
                    logEvent(2, "Erro ao processar pacotes do jogador " + 
                        std::to_string(pair.first) + ": " + e.what());
                    disconnectedPlayers.push_back(pair.first);
                }
            }
        }
        
        // Processa desconexõ/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param disconnectedPlayers Parâmetro disconnectedPlayers
 * @return Retorna es
 */
es
        for(auto playerID : disconnectedPlayers){
            handlePlayerDisconnect(playerID);
        }
        
        // Pequena pausa para não sobrecarregar o CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    logEvent(0, "Thread de processamento de conexões finalizada");
}

void TMServer::updateGameSystems(float deltaTime) {
    // Atualiza o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_world Parâmetro m_world
 * @return Retorna mundo
 */
 mundo
    if(m_world){
        m_world->update(deltaTime);
    }
    
    // Atualiza jogadores
    std::lock_guard<std::mutex> lock(m_playersMutex);
    for (auto& pair : m_players) {
        auto& player = pair.second;
        
        if (player->isConnected()) {
            try {
                player->update(deltaTime);
            } catch (const std::exception& e) {
                logEvent(2, "Erro ao atualizar jogador " + 
                    std::to_string(pair.first) + ": " + e.what());
                player->disconnect("Erro interno do servidor");
            }
        }
    }
}

bool TMServer::authenticatePlayer(uint32_t playerID, const std::string& authToken) {
    auto player = getPlayer(playerID);
    if (!player) {
        logEvent(1, "Tentativa de autenticar jogador inexistente: " + std::to_string(playerID));
        return false;
    }
    
    // Consulta o banco de dados para verificar a autenticaçã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_databaseClient Parâmetro m_databaseClient
 * @return Retorna o
 */
o
    if(m_databaseClient){
        // Exemplo de consulta de autenticação
        // [Implementação específica]
        
        // Simples validação de exemplo
        if (authToken.length() >= 8) {
            player->setConnectionState(WYDPlayer::ConnectionState::LOADING_CHARACTER);
            logEvent(0, "Jogador " + std::to_string(playerID) + " autenticado com sucesso");
            return true;
        }
    }
    
    logEvent(1, "Falha na autenticação do jogador " + std::to_string(playerID));
    return false;
}

void TMServer::handlePlayerDisconnect(uint32_t playerID) {
    std::shared_ptr<WYDPlayer> player;
    
    {
        std::lock_guard<std::mutex> lock(m_playersMutex);
        auto it = m_players.find(playerID);
        if (it != m_players.end()) {
            player = it->second;
            m_players.erase(it);
        }
    }
    
    if (player) {
        // Salva dados do jogador
        try {
            player->saveToDatabase();
        } catch (const std::exception& e) {
            logEvent(2, "Erro ao salvar dados do jogador " + 
                std::to_string(playerID) + ": " + e.what());
        }
        
        // Remove do /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_world Parâmetro m_world
 * @return Retorna mundo
 */
 mundo
        if(m_world){
            m_world->removePlayer(playerID);
        }
        
        logEvent(0, "Jogador desconectado: " + std::to_string(playerID));
        m_activeConnections--;
    }
}

void TMServer::saveAllPlayers() {
    logEvent(0, "Salvando dados de todos os jogadores...");
    
    int savedCount = 0;
    int failCount = 0;
    
    std::lock_guard<std::mutex> lock(m_playersMutex);
    for (auto& pair : m_players) {
        try {
            if (pair.second->saveToDatabase()) {
                savedCount++;
            } else {
                failCount++;
            }
        } catch (const std::exception& e) {
            logEvent(2, "Erro ao salvar jogador " + std::to_string(pair.first) + ": " + e.what());
            failCount++;
        }
    }
    
    logEvent(0, "Salvamento concluído: " + std::to_string(savedCount) + 
        " jogadores salvos, " + std::to_string(failCount) + " falhas");
}

void TMServer::logEvent(int severity, const std::string& message) {
    // Obtém timestamp atual
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime_r(&now_time), "%Y-%m-%d %H:%M:%S");
    
    // Converte severidade para string
    std::string severityStr;
    switch (severity) {
        case 0: severityStr = "INFO"; break;
        case 1: severityStr = "WARN"; break;
        case 2: severityStr = "ERROR"; break;
        case 3: severityStr = "FATAL"; break;
        default: severityStr = "DEBUG"; break;
    }
    
    // Formata a mensagem de log
    std::ostringstream logMessage;
    logMessage << "[" << timestamp.str() << "] [" << severityStr << "] " << message;
    
    // Exibe no console
    std::cout << logMessage.str() << std::endl;
    
    // Salva no arquivo de log
    try {
        std::string logFileName = "logs/server_" + 
            std::string(std::put_time(std::localtime_r(&now_time), "%Y%m%d")) + ".log";
        
        /**
 * logFile
 * 
 * Implementa a funcionalidade logFile conforme especificação original.
 * @param logFileName Parâmetro logFileName
 * @param std::ios::app Parâmetro std::ios::app
 * @return Retorna std::ofstream
 */

        
        std::ofstream logFile(logFileName, std::ios::app);
        if (logFile.is_open()) {
            logFile << logMessage.str() << std::endl;
            logFile.close();
        }
    } catch (...) {
        // Ignora erros de escrita no arquivo
    }
    
    // Registra no /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_eventBus Parâmetro m_eventBus
 * @return Retorna EventBus
 */
 EventBus
    if(m_eventBus){
        m_eventBus->publish("log.event", {
            {"severity", std::to_string(severity)},
            {"message", message},
            {"timestamp", timestamp.str()}
        });
    }
}

} // namespace tmsrv
} // namespace wydbr