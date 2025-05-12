/**
 * DBServer.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/dbsrv/DBServer.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "DBServer.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace wydbr {


/**
 * @file DBServer.cpp
 * @brief Implementação do servidor de banco de dados do WYD
 * 
 * Este arquivo contém a implementação do servidor de banco de dados (DBServer)
 * do WYD, seguindo a estrutura original, mas com melhorias para corrigir
 * problemas de concorrência, performance e segurança.
 */

namespace wyd {

} // namespace wydbr

namespace server {

CDBServer& CDBServer::GetInstance() {
    static CDBServer instance;
    return instance;
}

CDBServer::CDBServer()
    : isRunning_(false)
    , shuttingDown_(false)
    , serverSocket_(INVALID_SOCKET)
    , initialized_(false)
    , socketInitialized_(false)
{
    // Inicializa estatísticas
    stats_.dwStartTime = static_cast<DWORD>(time(nullptr));
}

CDBServer::~CDBServer() {
    Shutdown();
}

bool CDBServer::Initialize(const DBServerConfig& config) {
    if (initialized_) {
        LOG_WARNING("DBServer já inicializado");
        return true;
    }
    
    LOG_INFO("Inicializando DBServer...");
    
    // Armazena a configuração
    config_ = config;
    
    // Configura o logger
    wyd::core::Logger::SetLogLevel(static_cast<wyd::core::LogLevel>(config.logLevel));
    if (!config.logFile.empty()) {
        wyd::core::Logger::SetLogFile(config.logFile);
    }
    
    LOG_INFO("Inicializando componentes do DBServer...");
    
    // Inicializa o gerenciador de conexões
    if (!CDBConnectionManager::GetInstance().Initialize(
        config.dbConnectionString, config.dbMaxConnections, config.dbAutoReconnect)) {
        LOG_ERROR("Falha ao inicializar o gerenciador de conexões");
        return false;
    }
    
    // Inicializa o gerenciador de cache
    if (!CDBCacheManager::GetInstance().Initialize(config.cacheSyncInterval)) {
        LOG_ERROR("Falha ao inicializar o gerenciador de cache");
        return false;
    }
    
    // Define as políticas de cache padrã/**
 * SetDefaultCachePolicies
 * 
 * Implementa a funcionalidade SetDefaultCachePolicies conforme especificação original.
 * @return Retorna o
 */
o
    SetDefaultCachePolicies();
    
    // Inicializa o gerenciador de pacotes
    if (!CDBPacketManager::GetInstance().Initialize(config.numPacketWorkers)) {
        LOG_ERROR("Falha ao inicializar o gerenciador de pacotes");
        return false;
    }
    
    // Registra os handlers de pacotes padrã/**
 * RegisterDefaultPacketHandlers
 * 
 * Implementa a funcionalidade RegisterDefaultPacketHandlers conforme especificação original.
 * @return Retorna o
 */
o
    RegisterDefaultPacketHandlers();
    
    // Registra /**
 * RegisterCallbacks
 * 
 * Implementa a funcionalidade RegisterCallbacks conforme especificação original.
 * @return Retorna callbacks
 */
 callbacks
    RegisterCallbacks();
    
    // Inicializa o socket do servidor
    if (!InitializeSocket()) {
        LOG_ERROR("Falha ao inicializar o socket do servidor");
        return false;
    }
    
    initialized_ = true;
    LOG_INFO("DBServer inicializado com sucesso");
    
    return true;
}

void CDBServer::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    LOG_INFO("Desligando DBServer...");
    
    // Marca como desligando
    shuttingDown_ = true;
    
    // Para o /**
 * Stop
 * 
 * Implementa a funcionalidade Stop conforme especificação original.
 * @return Retorna servidor
 */
 servidor
    Stop();
    
    // Fecha o socket do /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param INVALID_SOCKET Parâmetro INVALID_SOCKET
 * @return Retorna servidor
 */
 servidor
    if(serverSocket_ != INVALID_SOCKET){
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
    }
    
    // Fecha os sockets de clientes
    {
        std::lock_guard<std::mutex> lock(socketsMutex_);
        for (auto& client : clientSockets_) {
            closesocket(client.first);
        }
        clientSockets_.clear();
    }
    
    // Desliga o gerenciador de pacotes
    CDBPacketManager::GetInstance().Shutdown();
    
    // Desliga o gerenciador de cache
    CDBCacheManager::GetInstance().Shutdown();
    
    // Desliga o gerenciador de conexões
    CDBConnectionManager::GetInstance().Shutdown();
    
    // Limpa os callbacks
    {
        std::lock_guard<std::mutex> lock(callbacksMutex_);
        eventCallbacks_.clear();
    }
    
    initialized_ = false;
    socketInitialized_ = false;
    
    LOG_INFO("DBServer desligado");
}

bool CDBServer::Start() {
    if (!initialized_) {
        LOG_ERROR("DBServer não inicializado");
        return false;
    }
    
    if (isRunning_) {
        LOG_WARNING("DBServer já está rodando");
        return true;
    }
    
    LOG_INFO("Iniciando DBServer...");
    
    // Marca como rodando
    isRunning_ = true;
    shuttingDown_ = false;
    
    // Inicia a thread principal do servidor
    serverThread_ = std::thread(&CDBServer::ServerThread, this);
    
    // Inicia a thread de manutenção
    maintenanceThread_ = std::thread(&CDBServer::MaintenanceThread, this);
    
    // Notifica evento de inicializaçã/**
 * NotifyServerEvent
 * 
 * Implementa a funcionalidade NotifyServerEvent conforme especificação original.
 * @param DB_EVENT_STARTUP Parâmetro DB_EVENT_STARTUP
 * @param 0 Parâmetro 0
 * @param iniciado" Parâmetro iniciado"
 * @return Retorna o
 */
o
    NotifyServerEvent(DB_EVENT_STARTUP, 0, "DBServer iniciado");
    
    LOG_INFO("DBServer iniciado na porta {}", config_.bindPort);
    
    return true;
}

void CDBServer::Stop() {
    if (!isRunning_) {
        return;
    }
    
    LOG_INFO("Parando DBServer...");
    
    // Marca como não rodando
    isRunning_ = false;
    
    // Espera pela thread principal do servidor
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
    
    // Espera pela thread de manutenção
    if (maintenanceThread_.joinable()) {
        maintenanceThread_.join();
    }
    
    // Notifica evento de /**
 * NotifyServerEvent
 * 
 * Implementa a funcionalidade NotifyServerEvent conforme especificação original.
 * @param DB_EVENT_SHUTDOWN Parâmetro DB_EVENT_SHUTDOWN
 * @param 0 Parâmetro 0
 * @param parado" Parâmetro parado"
 * @return Retorna parada
 */
 parada
    NotifyServerEvent(DB_EVENT_SHUTDOWN, 0, "DBServer parado");
    
    LOG_INFO("DBServer parado");
}

bool CDBServer::IsRunning() const {
    return isRunning_;
}

std::string CDBServer::GetStats() const {
    std::stringstream ss;
    
    // Captura as estatísticas
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    // Calcula o uptime
    DWORD now = static_cast<DWORD>(time(nullptr));
    DWORD uptime = now - stats_.dwStartTime;
    stats_.dwUptime = uptime;
    
    // Formata o uptime
    DWORD days = uptime / 86400;
    DWORD hours = (uptime % 86400) / 3600;
    DWORD minutes = (uptime % 3600) / 60;
    DWORD seconds = uptime % 60;
    
    // Atualiza clientes conectados
    stats_.dwConnectedClients = static_cast<DWORD>(clientSockets_.size());
    
    // Formata as estatísticas
    ss << "=== DBServer Statistics ===" << std::endl;
    ss << "Uptime: " << days << "d " << hours << "h " << minutes << "m " << seconds << "s" << std::endl;
    ss << "Connected Clients: " << stats_.dwConnectedClients << "/" << config_.maxClients << std::endl;
    ss << "Peak Connections: " << stats_.dwPeakConnections << std::endl;
    ss << "Total Connections: " << stats_.dwTotalConnections << std::endl;
    ss << "Total Disconnections: " << stats_.dwTotalDisconnections << std::endl;
    ss << "Total Packets Processed: " << stats_.dwTotalPacketsProcessed << std::endl;
    ss << "Packets Per Second: " << stats_.dwPacketsPerSecond << std::endl;
    ss << "Total Events: " << stats_.dwTotalEvents << std::endl;
    
    // Adiciona estatísticas do gerenciador de conexões
    ss << std::endl << CDBConnectionManager::GetInstance().GetConnectionStats();
    
    // Adiciona estatísticas do gerenciador de cache
    ss << std::endl << CDBCacheManager::GetInstance().GetCacheStats();
    
    // Adiciona estatísticas do gerenciador de pacotes
    ss << std::endl << CDBPacketManager::GetInstance().GetPacketStats();
    
    return ss.str();
}

void CDBServer::RegisterEventCallback(const DBServerEventCallback& callback) {
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    eventCallbacks_.push_back(callback);
}

const DBServerConfig& CDBServer::GetConfig() const {
    return config_;
}

bool CDBServer::LoadConfig(const std::string& filePath) {
    LOG_INFO("Carregando configuração do arquivo: {}", filePath);
    
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @return Retorna std::ifstream
 */

    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        LOG_ERROR("Falha ao abrir arquivo de configuração: {}", filePath);
        return false;
    }
    
    DBServerConfig newConfig;
    std::string line;
    
    // Lê o arquivo linha por linha
    while (std::getline(file, line)) {
        // Ignora linhas em branco e comentários
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Separa a chave e o valor
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Remove espaços
        key.erase(remove_if(key.begin(), key.end(), isspace), key.end());
        value.erase(remove_if(value.begin(), value.end(), isspace), value.end());
        
        // Processa a /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "dbConnectionString" Parâmetro "dbConnectionString"
 * @return Retorna chave
 */
 chave
        if(key == "dbConnectionString"){
            newConfig.dbConnectionString = value;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "dbMaxConnections" Parâmetro "dbMaxConnections"
 * @return Retorna else
 */
 else if(key == "dbMaxConnections"){
            newConfig.dbMaxConnections = std::stoi(value);
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "dbAutoReconnect" Parâmetro "dbAutoReconnect"
 * @return Retorna else
 */
 else if(key == "dbAutoReconnect"){
            newConfig.dbAutoReconnect = (value == "1" || value == "true");
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "cacheSize" Parâmetro "cacheSize"
 * @return Retorna else
 */
 else if(key == "cacheSize"){
            newConfig.cacheSize = std::stoi(value);
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "cacheSyncInterval" Parâmetro "cacheSyncInterval"
 * @return Retorna else
 */
 else if(key == "cacheSyncInterval"){
            newConfig.cacheSyncInterval = std::stoul(value);
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "bindIp" Parâmetro "bindIp"
 * @return Retorna else
 */
 else if(key == "bindIp"){
            newConfig.bindIp = value;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "bindPort" Parâmetro "bindPort"
 * @return Retorna else
 */
 else if(key == "bindPort"){
            newConfig.bindPort = std::stoi(value);
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "maxClients" Parâmetro "maxClients"
 * @return Retorna else
 */
 else if(key == "maxClients"){
            newConfig.maxClients = std::stoi(value);
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "numPacketWorkers" Parâmetro "numPacketWorkers"
 * @return Retorna else
 */
 else if(key == "numPacketWorkers"){
            newConfig.numPacketWorkers = std::stoi(value);
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "logLevel" Parâmetro "logLevel"
 * @return Retorna else
 */
 else if(key == "logLevel"){
            newConfig.logLevel = std::stoi(value);
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "logFile" Parâmetro "logFile"
 * @return Retorna else
 */
 else if(key == "logFile"){
            newConfig.logFile = value;
        }
    }
    
    file.close();
    
    // Atualiza a configuração
    config_ = newConfig;
    
    // Notifica evento de carregamento de configuraçã/**
 * NotifyServerEvent
 * 
 * Implementa a funcionalidade NotifyServerEvent conforme especificação original.
 * @param DB_EVENT_CONFIG_LOADED Parâmetro DB_EVENT_CONFIG_LOADED
 * @param 0 Parâmetro 0
 * @param carregada" Parâmetro carregada"
 * @return Retorna o
 */
o
    NotifyServerEvent(DB_EVENT_CONFIG_LOADED, 0, "Configuração carregada");
    
    LOG_INFO("Configuração carregada com sucesso");
    
    return true;
}

bool CDBServer::SaveConfig(const std::string& filePath) {
    LOG_INFO("Salvando configuração no arquivo: {}", filePath);
    
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @return Retorna std::ofstream
 */

    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        LOG_ERROR("Falha ao abrir arquivo de configuração para escrita: {}", filePath);
        return false;
    }
    
    // Escreve um cabeçalho
    file << "# DBServer Configuration" << std::endl;
    file << "# Generated on " << std::put_time(std::localtime_r(&stats_.dwStartTime), "%Y-%m-%d %H:%M:%S") << std::endl;
    file << std::endl;
    
    // Escreve a configuração
    file << "# Database Configuration" << std::endl;
    file << "dbConnectionString=" << config_.dbConnectionString << std::endl;
    file << "dbMaxConnections=" << config_.dbMaxConnections << std::endl;
    file << "dbAutoReconnect=" << (config_.dbAutoReconnect ? "1" : "0") << std::endl;
    file << std::endl;
    
    file << "# Cache Configuration" << std::endl;
    file << "cacheSize=" << config_.cacheSize << std::endl;
    file << "cacheSyncInterval=" << config_.cacheSyncInterval << std::endl;
    file << std::endl;
    
    file << "# Network Configuration" << std::endl;
    file << "bindIp=" << config_.bindIp << std::endl;
    file << "bindPort=" << config_.bindPort << std::endl;
    file << "maxClients=" << config_.maxClients << std::endl;
    file << std::endl;
    
    file << "# Processing Configuration" << std::endl;
    file << "numPacketWorkers=" << config_.numPacketWorkers << std::endl;
    file << std::endl;
    
    file << "# Logging Configuration" << std::endl;
    file << "logLevel=" << config_.logLevel << std::endl;
    file << "logFile=" << config_.logFile << std::endl;
    
    file.close();
    
    // Notifica evento de salvamento de configuraçã/**
 * NotifyServerEvent
 * 
 * Implementa a funcionalidade NotifyServerEvent conforme especificação original.
 * @param DB_EVENT_CONFIG_SAVED Parâmetro DB_EVENT_CONFIG_SAVED
 * @param 0 Parâmetro 0
 * @param salva" Parâmetro salva"
 * @return Retorna o
 */
o
    NotifyServerEvent(DB_EVENT_CONFIG_SAVED, 0, "Configuração salva");
    
    LOG_INFO("Configuração salva com sucesso");
    
    return true;
}

bool CDBServer::PerformMaintenance(bool fullMaintenance) {
    LOG_INFO("Executando manutenção no banco de dados (full: {})", fullMaintenance ? "sim" : "não");
    
    // Sincroniza o cache com o banco
    int syncedEntities = CDBCacheManager::GetInstance().SyncDirtyEntities();
    LOG_INFO("Sincronizadas {} entidades com o banco", syncedEntities);
    
    // Remove entidades expiradas do cache
    int evictedEntities = CDBCacheManager::GetInstance().EvictExpiredEntities();
    LOG_INFO("Removidas {} entidades expiradas do cache", evictedEntities);
    
    // Manutenção completa?
    if (fullMaintenance) {
        LOG_INFO("Executando manutenção completa");
        
        // Executa manutenção mais pesada (análise de tabelas, otimização, etc.)
        STRUCT_DBQUERY query;
        query.queryType = DB_QUERY_CUSTOM;
        query.entityType = DB_ENTITY_SYSTEM;
        query.queryString = "OPTIMIZE TABLE accounts, characters, guilds, guild_members, items, logs";
        query.dwTimeout = 120000; // 2 minutos
        
        STRUCT_DBRESULT result = CDBConnectionManager::GetInstance().ExecuteQuery(query);
        if (!result.success) {
            LOG_ERROR("Falha ao executar manutenção completa: {}", result.errorMessage);
            return false;
        }
        
        LOG_INFO("Manutenção completa executada com sucesso");
    }
    
    // Atualiza estatísticas
    std::lock_guard<std::mutex> lock(statsMutex_);
    stats_.dwLastMaintenanceTime = static_cast<DWORD>(time(nullptr));
    
    // Notifica evento de manutençã/**
 * NotifyServerEvent
 * 
 * Implementa a funcionalidade NotifyServerEvent conforme especificação original.
 * @param DB_EVENT_NONE Parâmetro DB_EVENT_NONE
 * @param 0 Parâmetro 0
 * @param executada" Parâmetro executada"
 * @return Retorna o
 */
o
    NotifyServerEvent(DB_EVENT_NONE, 0, "Manutenção executada");
    
    LOG_INFO("Manutenção concluída com sucesso");
    
    return true;
}

bool CDBServer::InitializeSocket() {
    LOG_INFO("Inicializando socket do servidor");
    
    // Se já estiver inicializado, desliga /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param socketInitialized_ Parâmetro socketInitialized_
 * @return Retorna primeiro
 */
 primeiro
    if(socketInitialized_){
        if (serverSocket_ != INVALID_SOCKET) {
            closesocket(serverSocket_);
            serverSocket_ = INVALID_SOCKET;
        }
    }
    
    // Inicializa a biblioteca de sockets se necessário
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        LOG_ERROR("Falha ao inicializar Winsock");
        return false;
    }
    #endif
    
    // Cria o socket do servidor
    serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket_ == INVALID_SOCKET) {
        LOG_ERROR("Falha ao criar socket do servidor");
        return false;
    }
    
    // Configura o socket para reutilizar endereço
    int reuse = 1;
    if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
        LOG_ERROR("Falha ao configurar reutilização de endereço");
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
        return false;
    }
    
    // Configura o socket para não bloquear
    #ifdef _WIN32
    unsigned long mode = 1;
    if (ioctlsocket(serverSocket_, FIONBIO, &mode) != 0) {
        LOG_ERROR("Falha ao configurar socket não bloqueante");
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
        return false;
    }
    #else
    if (fcntl(serverSocket_, F_SETFL, fcntl(serverSocket_, F_GETFL, 0) | O_NONBLOCK) == -1) {
        LOG_ERROR("Falha ao configurar socket não bloqueante");
        close(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
        return false;
    }
    #endif
    
    // Configura o endereço do servidor
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(config_.bindPort);
    
    // Converte o IP de string para formato biná/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "0.0.0.0" Parâmetro "0.0.0.0"
 * @return Retorna rio
 */
rio
    if(config_.bindIp == "0.0.0.0"){
        serverAddr.sin_addr.s_addr = INADDR_ANY;
    } else {
        serverAddr.sin_addr.s_addr = inet_addr(config_.bindIp.c_str());
    }
    
    // Faz o bind do socket
    if (bind(serverSocket_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        LOG_ERROR("Falha ao fazer bind do socket: {}", errno);
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
        return false;
    }
    
    // Inicia a escuta de conexões
    if (listen(serverSocket_, SOMAXCONN) < 0) {
        LOG_ERROR("Falha ao iniciar escuta do socket");
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
        return false;
    }
    
    socketInitialized_ = true;
    LOG_INFO("Socket do servidor inicializado na porta {}", config_.bindPort);
    
    return true;
}

void CDBServer::ServerThread() {
    LOG_INFO("Thread principal do servidor iniciada");
    
    // Define o conjunto de descritores
    fd_set readSet;
    struct timeval timeout;
    
    // Loop principal do /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param isRunning_ Parâmetro isRunning_
 * @return Retorna servidor
 */
 servidor
    while(isRunning_){
        // Limpa o conjunto de /**
 * FD_ZERO
 * 
 * Implementa a funcionalidade FD_ZERO conforme especificação original.
 * @param readSet Parâmetro readSet
 * @return Retorna descritores
 */
 descritores
        FD_ZERO(&readSet);
        
        // Adiciona o socket do servidor ao /**
 * FD_SET
 * 
 * Implementa a funcionalidade FD_SET conforme especificação original.
 * @param serverSocket_ Parâmetro serverSocket_
 * @param readSet Parâmetro readSet
 * @return Retorna conjunto
 */
 conjunto
        FD_SET(serverSocket_, &readSet);
        
        // Adiciona os sockets dos clientes ao conjunto
        std::unordered_map<SOCKET, DWORD> clientsCopy;
        {
            std::lock_guard<std::mutex> lock(socketsMutex_);
            clientsCopy = clientSockets_;
        }
        
        SOCKET maxSocket = serverSocket_;
        for (const auto& client : clientsCopy) {
            FD_SET(client.first, &readSet);
            if (client.first > maxSocket) {
                maxSocket = client.first;
            }
        }
        
        // Define o timeout (100 ms)
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
        
        // Espera por eventos
        int activity = select(maxSocket + 1, &readSet, NULL, NULL, &timeout);
        
        // Erro?
        if (activity < 0) {
            // Ignora erros de interrupção
            #ifdef _WIN32
            if (WSAGetLastError() != WSAEINTR) {
                LOG_ERROR("Erro em select(): {}", WSAGetLastError());
            }
            #/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param EINTR Parâmetro EINTR
 * @return Retorna else
 */
else
            if(errno != EINTR){
                LOG_ERROR("Erro em select(): {}", errno);
            }
            #endif
            continue;
        }
        
        // Timeout?
        if (activity == 0) {
            continue;
        }
        
        // Nova conexão?
        if (FD_ISSET(serverSocket_, &readSet)) {
            // Aceita a conexão
            struct sockaddr_in clientAddr;
            socklen_t addrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &addrLen);
            
            if (clientSocket == INVALID_SOCKET) {
                LOG_ERROR("Falha ao aceitar conexão");
            } else {
                // Obtém o IP do cliente
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
                
                // Configura o socket para não bloquear
                #ifdef _WIN32
                unsigned long mode = 1;
                if (ioctlsocket(clientSocket, FIONBIO, &mode) != 0) {
                    LOG_ERROR("Falha ao configurar socket de cliente não bloqueante");
                    closesocket(clientSocket);
                    continue;
                }
                #else
                if (fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK) == -1) {
                    LOG_ERROR("Falha ao configurar socket de cliente não bloqueante");
                    close(clientSocket);
                    continue;
                }
                #endif
                
                // Verifica se já atingiu o limite de clientes
                {
                    std::lock_guard<std::mutex> lock(socketsMutex_);
                    if (clientSockets_.size() >= config_.maxClients) {
                        LOG_WARNING("Limite de clientes atingido, rejeitando conexão de {}", clientIP);
                        closesocket(clientSocket);
                        continue;
                    }
                    
                    // Adiciona o cliente à lista
                    DWORD clientId = static_cast<DWORD>(clientSockets_.size() + 1);
                    clientSockets_[clientSocket] = clientId;
                    
                    // Atualiza estatísticas
                    stats_.dwTotalConnections++;
                    if (clientSockets_.size() > stats_.dwPeakConnections) {
                        stats_.dwPeakConnections = static_cast<DWORD>(clientSockets_.size());
                    }
                }
                
                LOG_INFO("Nova conexão aceita de {} (socket: {}, id: {})", 
                          clientIP, clientSocket, clientSockets_[clientSocket]);
                
                // Processa a conexão em thread separada
                std::thread(&CDBServer::HandleClientConnection, this, clientSocket).detach();
            }
        }
        
        // Verifica os sockets dos /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param clientsCopy Parâmetro clientsCopy
 * @return Retorna clientes
 */
 clientes
        for(const auto& client : clientsCopy){
            if (FD_ISSET(client.first, &readSet)) {
                // O cliente tem dados para ler
                // Isso é tratado na thread dedicada, não precisamos fazer nada aqui
            }
        }
    }
    
    LOG_INFO("Thread principal do servidor finalizada");
}

void CDBServer::MaintenanceThread() {
    LOG_INFO("Thread de manutenção iniciada");
    
    // Define o intervalo de manutenção
    const DWORD maintenanceInterval = 300; // 5 minutos
    const DWORD fullMaintenanceInterval = 3600; // 1 hora
    
    DWORD lastMaintenanceTime = 0;
    DWORD lastFullMaintenanceTime = 0;
    
    // Loop de manutençã/**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param isRunning_ Parâmetro isRunning_
 * @return Retorna o
 */
o
    while(isRunning_){
        // Dorme um pouco
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Obtém o tempo atual
        DWORD now = static_cast<DWORD>(time(nullptr));
        
        // Verifica se é hora de fazer manutençã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param maintenanceInterval Parâmetro maintenanceInterval
 * @return Retorna o
 */
o
        if(now - lastMaintenanceTime >= maintenanceInterval){
            PerformMaintenance(false);
            lastMaintenanceTime = now;
        }
        
        // Verifica se é hora de fazer manutenção /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param fullMaintenanceInterval Parâmetro fullMaintenanceInterval
 * @return Retorna completa
 */
 completa
        if(now - lastFullMaintenanceTime >= fullMaintenanceInterval){
            PerformMaintenance(true);
            lastFullMaintenanceTime = now;
        }
    }
    
    LOG_INFO("Thread de manutenção finalizada");
}

void CDBServer::HandleClientConnection(SOCKET clientSocket) {
    LOG_DEBUG("Iniciando thread de processamento para cliente (socket: {})", clientSocket);
    
    DWORD clientId;
    {
        std::lock_guard<std::mutex> lock(socketsMutex_);
        auto it = clientSockets_.find(clientSocket);
        if (it == clientSockets_.end()) {
            LOG_ERROR("Cliente não encontrado na lista (socket: {})", clientSocket);
            closesocket(clientSocket);
            return;
        }
        clientId = it->second;
    }
    
    // Buffer para receber dados
    const int bufferSize = 8192;
    BYTE buffer[bufferSize];
    
    // Espera pela mensagem de autenticação
    DWORD authTimeoutSeconds = 10;
    DWORD authStartTime = static_cast<DWORD>(time(nullptr));
    bool authenticated = false;
    
    // Loop de /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param !shuttingDown_ Parâmetro !shuttingDown_
 * @return Retorna processamento
 */
 processamento
    while(isRunning_ && !shuttingDown_){
        // Verifica timeout de autenticaçã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !authenticated Parâmetro !authenticated
 * @return Retorna o
 */
o
        if(!authenticated){
            DWORD now = static_cast<DWORD>(time(nullptr));
            if (now - authStartTime > authTimeoutSeconds) {
                LOG_WARNING("Timeout de autenticação para cliente (id: {}, socket: {})", clientId, clientSocket);
                break;
            }
        }
        
        // Recebe dados
        int bytesRead = recv(clientSocket, reinterpret_cast<char*>(buffer), bufferSize, 0);
        
        // Erro ou desconexão?
        if (bytesRead <= 0) {
            #ifdef _WIN32
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                if (error == WSAECONNRESET) {
                    LOG_INFO("Cliente desconectado (id: {}, socket: {})", clientId, clientSocket);
                } else {
                    LOG_ERROR("Erro ao receber dados do cliente (id: {}, socket: {}): {}", 
                               clientId, clientSocket, error);
                }
                break;
            }
            #/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param EAGAIN Parâmetro EAGAIN
 * @return Retorna else
 */
else
            if(errno != EWOULDBLOCK && errno != EAGAIN){
                if (errno == ECONNRESET) {
                    LOG_INFO("Cliente desconectado (id: {}, socket: {})", clientId, clientSocket);
                } else {
                    LOG_ERROR("Erro ao receber dados do cliente (id: {}, socket: {}): {}", 
                               clientId, clientSocket, errno);
                }
                break;
            }
            #endif
            
            // Não há dados para ler, espera um pouco
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Dados recebidos, processa o pacote
        if (bytesRead >= sizeof(STRUCT_DB_PACKET)) {
            STRUCT_DB_PACKET* packet = reinterpret_cast<STRUCT_DB_PACKET*>(buffer);
            
            // Verifica se o tamanho do pacote é válido
            if (packet->wSize <= bufferSize && packet->wSize >= sizeof(STRUCT_DB_PACKET)) {
                // Dados do pacote
                const BYTE* packetData = buffer + sizeof(STRUCT_DB_PACKET);
                int packetDataSize = bytesRead - sizeof(STRUCT_DB_PACKET);
                
                // Pacote de autenticação?
                if (!authenticated && packet->byType == DB_PKT_CONNECT) {
                    // Processa a autenticação (no WYD original, isso era um pacote específico)
                    // [wSize][byType][byResult][dwClientTick][dwServerTick][szUsername][szPassword]
                    
                    // No WYD original, isso era feito comparando com valores hardcoded
                    // ou em um arquivo de configuração, vamos simular isso /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 64 Parâmetro 64
 * @return Retorna aqui
 */
 aqui
                    if(packetDataSize >= 64){ // 32 bytes para usuário, 32 para senha
                        const char* username = reinterpret_cast<const char*>(packetData);
                        const char* password = reinterpret_cast<const char*>(packetData + 32);
                        
                        // No WYD original, isso era uma comparação simples
                        if (strcmp(username, "WYDDBServer") == 0 && strcmp(password, "WYDDBPassword") == 0) {
                            authenticated = true;
                            LOG_INFO("Cliente autenticado (id: {}, socket: {})", clientId, clientSocket);
                            
                            // Envia resposta de autenticação
                            STRUCT_DB_PACKET response;
                            response.wSize = sizeof(STRUCT_DB_PACKET);
                            response.byType = DB_PKT_CONNECT;
                            response.byResult = 1; // Sucesso
                            response.dwClientTick = packet->dwClientTick;
                            response.dwServerTick = static_cast<DWORD>(time(nullptr));
                            
                            send(clientSocket, reinterpret_cast<const char*>(&response), sizeof(response), 0);
                        } else {
                            LOG_WARNING("Falha na autenticação do cliente (id: {}, socket: {})", clientId, clientSocket);
                            
                            // Envia resposta de autenticação falha
                            STRUCT_DB_PACKET response;
                            response.wSize = sizeof(STRUCT_DB_PACKET);
                            response.byType = DB_PKT_CONNECT;
                            response.byResult = 0; // Falha
                            response.dwClientTick = packet->dwClientTick;
                            response.dwServerTick = static_cast<DWORD>(time(nullptr));
                            
                            send(clientSocket, reinterpret_cast<const char*>(&response), sizeof(response), 0);
                            
                            // Encerra a conexão
                            break;
                        }
                    } else {
                        LOG_WARNING("Pacote de autenticação inválido (id: {}, socket: {})", clientId, clientSocket);
                        break;
                    }
                }
                // Outros pacotes só são processados se o cliente estiver autenticado
                /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param authenticated Parâmetro authenticated
 * @return Retorna else
 */

                else if(authenticated){
                    // Pacote de disconnect?
                    if (packet->byType == DB_PKT_DISCONNECT) {
                        LOG_INFO("Cliente solicitou desconexão (id: {}, socket: {})", clientId, clientSocket);
                        break;
                    }
                    
                    // Pacote de ping?
                    if (packet->byType == DB_PKT_PING) {
                        // Responde com um pacote de ping
                        STRUCT_DB_PACKET response;
                        response.wSize = sizeof(STRUCT_DB_PACKET);
                        response.byType = DB_PKT_PING;
                        response.byResult = 1; // Sucesso
                        response.dwClientTick = packet->dwClientTick;
                        response.dwServerTick = static_cast<DWORD>(time(nullptr));
                        
                        send(clientSocket, reinterpret_cast<const char*>(&response), sizeof(response), 0);
                    }
                    // Outros pacotes
                    else {
                        // Processa o pacote
                        BYTE responseBuffer[bufferSize];
                        int responseSize = 0;
                        
                        // Cria uma cópia do pacote para processamento
                        STRUCT_DB_PACKET packetCopy = *packet;
                        
                        // Processa o pacote
                        DB_PACKET_RESULT result = CDBPacketManager::GetInstance().ProcessPacket(
                            &packetCopy, packetData, packetDataSize, responseBuffer, responseSize);
                        
                        // Atualiza estatísticas
                        {
                            std::lock_guard<std::mutex> lock(statsMutex_);
                            stats_.dwTotalPacketsProcessed++;
                        }
                        
                        // Envia a /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna resposta
 */
 resposta
                        if(responseSize > 0){
                            send(clientSocket, reinterpret_cast<const char*>(responseBuffer), responseSize, 0);
                        }
                    }
                }
                // Cliente não autenticado
                else {
                    LOG_WARNING("Cliente não autenticado tentou enviar pacote (id: {}, socket: {})", clientId, clientSocket);
                    break;
                }
            }
            // Pacote inválido
            else {
                LOG_WARNING("Pacote inválido recebido do cliente (id: {}, socket: {})", clientId, clientSocket);
                break;
            }
        }
        // Dados incompletos
        else {
            LOG_WARNING("Dados incompletos recebidos do cliente (id: {}, socket: {})", clientId, clientSocket);
            break;
        }
    }
    
    // Remove o cliente da lista
    {
        std::lock_guard<std::mutex> lock(socketsMutex_);
        clientSockets_.erase(clientSocket);
        
        // Atualiza estatísticas
        stats_.dwTotalDisconnections++;
    }
    
    // Fecha o /**
 * closesocket
 * 
 * Implementa a funcionalidade closesocket conforme especificação original.
 * @param clientSocket Parâmetro clientSocket
 * @return Retorna socket
 */
 socket
    closesocket(clientSocket);
    
    LOG_INFO("Thread de processamento para cliente finalizada (id: {}, socket: {})", clientId, clientSocket);
}

void CDBServer::OnDatabaseConnectionEvent(bool connected) {
    // Notifica o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param connected Parâmetro connected
 * @return Retorna evento
 */
 evento
    if(connected){
        NotifyServerEvent(DB_EVENT_CONNECTED, 0, "Conectado ao banco de dados");
    } else {
        NotifyServerEvent(DB_EVENT_DISCONNECTED, 0, "Desconectado do banco de dados");
    }
}

void CDBServer::OnCacheEvent(DB_ENTITY_TYPE entityType, DWORD entityId, bool isDirty) {
    // Converte o tipo de entidade para string
    const char* entityTypeStr = "Unknown";
    switch (entityType) {
        case DB_ENTITY_ACCOUNT: entityTypeStr = "Account"; break;
        case DB_ENTITY_CHARACTER: entityTypeStr = "Character"; break;
        case DB_ENTITY_ITEM: entityTypeStr = "Item"; break;
        case DB_ENTITY_INVENTORY: entityTypeStr = "Inventory"; break;
        case DB_ENTITY_STORAGE: entityTypeStr = "Storage"; break;
        case DB_ENTITY_GUILD: entityTypeStr = "Guild"; break;
        case DB_ENTITY_SKILL: entityTypeStr = "Skill"; break;
        case DB_ENTITY_QUEST: entityTypeStr = "Quest"; break;
        case DB_ENTITY_EVENT: entityTypeStr = "Event"; break;
        case DB_ENTITY_MARKET: entityTypeStr = "Market"; break;
        case DB_ENTITY_BILLING: entityTypeStr = "Billing"; break;
        case DB_ENTITY_LOG: entityTypeStr = "Log"; break;
        case DB_ENTITY_MISC: entityTypeStr = "Misc"; break;
        case DB_ENTITY_RANKING: entityTypeStr = "Ranking"; break;
        case DB_ENTITY_SYSTEM: entityTypeStr = "System"; break;
        default: break;
    }
    
    // Notifica o evento
    char message[256];
    if (isDirty) {
        snprintf(message, sizeof(message), "Cache atualizado: %s (ID: %u)", entityTypeStr, entityId);
        NotifyServerEvent(DB_EVENT_CACHE_SYNCED, entityId, message);
    } else {
        snprintf(message, sizeof(message), "Cache removido: %s (ID: %u)", entityTypeStr, entityId);
        NotifyServerEvent(DB_EVENT_CACHE_EVICTED, entityId, message);
    }
}

void CDBServer::OnPacketEvent(DB_PACKET_TYPE packetType, DB_PACKET_RESULT result, 
                             DWORD clientId, DWORD processTime) {
    // Converte o tipo de pacote para string
    const char* packetTypeStr = "Unknown";
    switch (packetType) {
        case DB_PKT_PING: packetTypeStr = "Ping"; break;
        case DB_PKT_CONNECT: packetTypeStr = "Connect"; break;
        case DB_PKT_DISCONNECT: packetTypeStr = "Disconnect"; break;
        case DB_PKT_ACCOUNT_LOGIN: packetTypeStr = "AccountLogin"; break;
        case DB_PKT_ACCOUNT_CREATE: packetTypeStr = "AccountCreate"; break;
        case DB_PKT_ACCOUNT_UPDATE: packetTypeStr = "AccountUpdate"; break;
        case DB_PKT_ACCOUNT_DELETE: packetTypeStr = "AccountDelete"; break;
        case DB_PKT_ACCOUNT_BAN: packetTypeStr = "AccountBan"; break;
        case DB_PKT_ACCOUNT_UNBAN: packetTypeStr = "AccountUnban"; break;
        case DB_PKT_ACCOUNT_PREMIUM: packetTypeStr = "AccountPremium"; break;
        case DB_PKT_CHAR_LIST: packetTypeStr = "CharList"; break;
        case DB_PKT_CHAR_CREATE: packetTypeStr = "CharCreate"; break;
        case DB_PKT_CHAR_DELETE: packetTypeStr = "CharDelete"; break;
        case DB_PKT_CHAR_LOAD: packetTypeStr = "CharLoad"; break;
        case DB_PKT_CHAR_SAVE: packetTypeStr = "CharSave"; break;
        case DB_PKT_CHAR_POSITION: packetTypeStr = "CharPosition"; break;
        case DB_PKT_CHAR_RENAME: packetTypeStr = "CharRename"; break;
        case DB_PKT_CHAR_LOGOUT: packetTypeStr = "CharLogout"; break;
        case DB_PKT_ITEM_LOAD: packetTypeStr = "ItemLoad"; break;
        case DB_PKT_ITEM_SAVE: packetTypeStr = "ItemSave"; break;
        case DB_PKT_ITEM_DELETE: packetTypeStr = "ItemDelete"; break;
        case DB_PKT_ITEM_UPDATE: packetTypeStr = "ItemUpdate"; break;
        case DB_PKT_INVENTORY_LOAD: packetTypeStr = "InventoryLoad"; break;
        case DB_PKT_INVENTORY_SAVE: packetTypeStr = "InventorySave"; break;
        case DB_PKT_STORAGE_LOAD: packetTypeStr = "StorageLoad"; break;
        case DB_PKT_STORAGE_SAVE: packetTypeStr = "StorageSave"; break;
        case DB_PKT_GUILD_LIST: packetTypeStr = "GuildList"; break;
        case DB_PKT_GUILD_CREATE: packetTypeStr = "GuildCreate"; break;
        case DB_PKT_GUILD_DELETE: packetTypeStr = "GuildDelete"; break;
        case DB_PKT_GUILD_LOAD: packetTypeStr = "GuildLoad"; break;
        case DB_PKT_GUILD_SAVE: packetTypeStr = "GuildSave"; break;
        case DB_PKT_GUILD_MEMBER_ADD: packetTypeStr = "GuildMemberAdd"; break;
        case DB_PKT_GUILD_MEMBER_REMOVE: packetTypeStr = "GuildMemberRemove"; break;
        case DB_PKT_GUILD_MEMBER_UPDATE: packetTypeStr = "GuildMemberUpdate"; break;
        case DB_PKT_GUILD_ALLY: packetTypeStr = "GuildAlly"; break;
        case DB_PKT_GUILD_WAR: packetTypeStr = "GuildWar"; break;
        case DB_PKT_SKILL_LOAD: packetTypeStr = "SkillLoad"; break;
        case DB_PKT_SKILL_SAVE: packetTypeStr = "SkillSave"; break;
        case DB_PKT_QUEST_LOAD: packetTypeStr = "QuestLoad"; break;
        case DB_PKT_QUEST_SAVE: packetTypeStr = "QuestSave"; break;
        case DB_PKT_QUEST_UPDATE: packetTypeStr = "QuestUpdate"; break;
        case DB_PKT_RANK_LIST: packetTypeStr = "RankList"; break;
        case DB_PKT_RANK_UPDATE: packetTypeStr = "RankUpdate"; break;
        case DB_PKT_LOG_ADD: packetTypeStr = "LogAdd"; break;
        case DB_PKT_LOG_QUERY: packetTypeStr = "LogQuery"; break;
        case DB_PKT_BILLING_CHECK: packetTypeStr = "BillingCheck"; break;
        case DB_PKT_BILLING_UPDATE: packetTypeStr = "BillingUpdate"; break;
        case DB_PKT_ADMIN_COMMAND: packetTypeStr = "AdminCommand"; break;
        case DB_PKT_ADMIN_BROADCAST: packetTypeStr = "AdminBroadcast"; break;
        case DB_PKT_SHUTDOWN: packetTypeStr = "Shutdown"; break;
        case DB_PKT_CONFIG: packetTypeStr = "Config"; break;
        default: break;
    }
    
    // Converte o resultado para string
    const char* resultStr = "Unknown";
    switch (result) {
        case PACKET_SUCCESS: resultStr = "Success"; break;
        case PACKET_ERROR_INVALID: resultStr = "Invalid"; break;
        case PACKET_ERROR_AUTH: resultStr = "Auth"; break;
        case PACKET_ERROR_DB: resultStr = "DB"; break;
        case PACKET_ERROR_PARAMS: resultStr = "Params"; break;
        case PACKET_ERROR_NOTFOUND: resultStr = "NotFound"; break;
        case PACKET_ERROR_INTERNAL: resultStr = "Internal"; break;
        case PACKET_ERROR_OVERLOAD: resultStr = "Overload"; break;
        case PACKET_ERROR_TIMEOUT: resultStr = "Timeout"; break;
        case PACKET_ERROR_INVALID_STATE: resultStr = "InvalidState"; break;
        default: break;
    }
    
    // Notifica o evento
    char message[256];
    if (result == PACKET_SUCCESS) {
        snprintf(message, sizeof(message), "Pacote processado: %s (Cliente: %u, Tempo: %u ms)", 
                 packetTypeStr, clientId, processTime);
        NotifyServerEvent(DB_EVENT_QUERY_COMPLETED, clientId, message);
    } else {
        snprintf(message, sizeof(message), "Erro no pacote: %s (Cliente: %u, Erro: %s, Tempo: %u ms)", 
                 packetTypeStr, clientId, resultStr, processTime);
        NotifyServerEvent(DB_EVENT_QUERY_ERROR, clientId, message);
    }
}

void CDBServer::NotifyServerEvent(DB_EVENT eventType, DWORD param, const char* message) {
    // Loga o evento
    LOG_DEBUG("Evento: {} - {} (Param: {})", eventType, message, param);
    
    // Notifica os callbacks
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    for (const auto& callback : eventCallbacks_) {
        callback(eventType, param, message);
    }
    
    // Atualiza estatísticas
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        stats_.dwTotalEvents++;
    }
}

void CDBServer::SetDefaultCachePolicies() {
    LOG_INFO("Definindo políticas de cache padrão");
    
    CachePolicy accountPolicy;
    accountPolicy.dwExpirationTime = CACHE_ACCOUNT_EXPIRE; // 1 hora
    accountPolicy.dwMaxEntries = 1000;
    accountPolicy.syncOnEviction = true;
    accountPolicy.preloadEnabled = false;
    CDBCacheManager::GetInstance().SetCachePolicy(DB_ENTITY_ACCOUNT, accountPolicy);
    
    CachePolicy charPolicy;
    charPolicy.dwExpirationTime = CACHE_CHAR_EXPIRE; // 30 minutos
    charPolicy.dwMaxEntries = 2000;
    charPolicy.syncOnEviction = true;
    charPolicy.preloadEnabled = false;
    CDBCacheManager::GetInstance().SetCachePolicy(DB_ENTITY_CHARACTER, charPolicy);
    
    CachePolicy guildPolicy;
    guildPolicy.dwExpirationTime = CACHE_GUILD_EXPIRE; // 2 horas
    guildPolicy.dwMaxEntries = 500;
    guildPolicy.syncOnEviction = true;
    guildPolicy.preloadEnabled = true;
    CDBCacheManager::GetInstance().SetCachePolicy(DB_ENTITY_GUILD, guildPolicy);
    
    CachePolicy itemPolicy;
    itemPolicy.dwExpirationTime = CACHE_ITEM_EXPIRE; // 10 minutos
    itemPolicy.dwMaxEntries = 5000;
    itemPolicy.syncOnEviction = true;
    itemPolicy.preloadEnabled = false;
    CDBCacheManager::GetInstance().SetCachePolicy(DB_ENTITY_ITEM, itemPolicy);
    
    CachePolicy inventoryPolicy;
    inventoryPolicy.dwExpirationTime = CACHE_CHAR_EXPIRE; // 30 minutos
    inventoryPolicy.dwMaxEntries = 2000;
    inventoryPolicy.syncOnEviction = true;
    inventoryPolicy.preloadEnabled = false;
    CDBCacheManager::GetInstance().SetCachePolicy(DB_ENTITY_INVENTORY, inventoryPolicy);
    
    CachePolicy storagePolicy;
    storagePolicy.dwExpirationTime = CACHE_CHAR_EXPIRE; // 30 minutos
    storagePolicy.dwMaxEntries = 2000;
    storagePolicy.syncOnEviction = true;
    storagePolicy.preloadEnabled = false;
    CDBCacheManager::GetInstance().SetCachePolicy(DB_ENTITY_STORAGE, storagePolicy);
    
    CachePolicy skillPolicy;
    skillPolicy.dwExpirationTime = CACHE_CHAR_EXPIRE; // 30 minutos
    skillPolicy.dwMaxEntries = 2000;
    skillPolicy.syncOnEviction = true;
    skillPolicy.preloadEnabled = false;
    CDBCacheManager::GetInstance().SetCachePolicy(DB_ENTITY_SKILL, skillPolicy);
    
    CachePolicy questPolicy;
    questPolicy.dwExpirationTime = CACHE_CHAR_EXPIRE; // 30 minutos
    questPolicy.dwMaxEntries = 2000;
    questPolicy.syncOnEviction = true;
    questPolicy.preloadEnabled = false;
    CDBCacheManager::GetInstance().SetCachePolicy(DB_ENTITY_QUEST, questPolicy);
    
    CachePolicy rankPolicy;
    rankPolicy.dwExpirationTime = 3600000; // 1 hora
    rankPolicy.dwMaxEntries = 100;
    rankPolicy.syncOnEviction = false;
    rankPolicy.preloadEnabled = true;
    CDBCacheManager::GetInstance().SetCachePolicy(DB_ENTITY_RANKING, rankPolicy);
    
    LOG_INFO("Políticas de cache padrão definidas");
}

void CDBServer::RegisterDefaultPacketHandlers() {
    LOG_INFO("Registrando handlers de pacotes padrão");
    
    // No WYD original, isso era feito em funções específicas para cada tipo de pacote
    // Por exemplo, ProcAccountLogin, ProcCharCreate, etc.
    
    // Aqui, vamos registrar handlers para os principais tipos de pacotes
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_ACCOUNT_LOGIN,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleAccountLogin(packet, data, dataSize, responseData, responseSize);
        });
    
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_ACCOUNT_CREATE,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleAccountCreate(packet, data, dataSize, responseData, responseSize);
        });
    
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_CHAR_CREATE,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleCharCreate(packet, data, dataSize, responseData, responseSize);
        });
    
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_CHAR_LOAD,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleCharLoad(packet, data, dataSize, responseData, responseSize);
        });
    
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_CHAR_SAVE,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleCharSave(packet, data, dataSize, responseData, responseSize);
        });
    
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_INVENTORY_LOAD,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleInventoryLoad(packet, data, dataSize, responseData, responseSize);
        });
    
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_INVENTORY_SAVE,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleInventorySave(packet, data, dataSize, responseData, responseSize);
        });
    
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_GUILD_LOAD,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleGuildLoad(packet, data, dataSize, responseData, responseSize);
        });
    
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_GUILD_SAVE,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleGuildSave(packet, data, dataSize, responseData, responseSize);
        });
    
    CDBPacketManager::GetInstance().RegisterPacketHandler(
        DB_PKT_ADMIN_COMMAND,
        [this](STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
            HandleAdminCommand(packet, data, dataSize, responseData, responseSize);
        });
    
    LOG_INFO("Handlers de pacotes padrão registrados");
}

void CDBServer::RegisterCallbacks() {
    LOG_INFO("Registrando callbacks");
    
    // Registra callback para eventos de conexão com o banco
    CDBConnectionManager::GetInstance().RegisterConnectionCallback(
        [this](bool connected) {
            OnDatabaseConnectionEvent(connected);
        }
    );
    
    // Registra callback para eventos de cache
    CDBCacheManager::GetInstance().RegisterCacheEventCallback(
        [this](DB_ENTITY_TYPE entityType, DWORD entityId, bool isDirty) {
            OnCacheEvent(entityType, entityId, isDirty);
        }
    );
    
    // Registra callback para eventos de pacote
    CDBPacketManager::GetInstance().RegisterPacketEventCallback(
        [this](DB_PACKET_TYPE packetType, DB_PACKET_RESULT result, DWORD clientId, DWORD processTime) {
            OnPacketEvent(packetType, result, clientId, processTime);
        }
    );
    
    LOG_INFO("Callbacks registrados");
}

// Implementações dos handlers de pacotes
// Estas são versões simplificadas das funções originais do WYD

void CDBServer::HandleAccountLogin(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // No WYD original, essa função processava um pacote de login de conta
    // [wSize][byType][byResult][dwClientTick][dwServerTick][szUsername][szPassword]
    
    // Verificamos se o tamanho dos dados é /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 64 Parâmetro 64
 * @return Retorna suficiente
 */
 suficiente
    if(dataSize < 64){ // 32 bytes para usuário, 32 para senha
        // Cria um pacote de erro
        CDBPacketManager::GetInstance().CreateErrorPacket(
            packet, PACKET_ERROR_PARAMS, "Invalid login packet size", responseData, responseSize);
        return;
    }
    
    // Extrai usuário e senha
    const char* username = reinterpret_cast<const char*>(data);
    const char* password = reinterpret_cast<const char*>(data + 32);
    
    // Cria uma query para buscar a conta
    STRUCT_DBQUERY query;
    query.queryType = DB_QUERY_READ;
    query.entityType = DB_ENTITY_ACCOUNT;
    query.queryString = "SELECT * FROM accounts WHERE username = '" + std::string(username) + "' LIMIT 1";
    
    // Executa a query
    STRUCT_DBRESULT result = CDBConnectionManager::GetInstance().ExecuteQuery(query);
    
    // Verifica o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !result.success Parâmetro !result.success
 * @return Retorna resultado
 */
 resultado
    if(!result.success){
        // Erro na query
        CDBPacketManager::GetInstance().CreateErrorPacket(
            packet, PACKET_ERROR_DB, "Database error", responseData, responseSize);
        return;
    }
    
    // Verifica se encontrou a conta
    if (result.rows.empty()) {
        // Conta não encontrada
        CDBPacketManager::GetInstance().CreateErrorPacket(
            packet, PACKET_ERROR_NOTFOUND, "Account not found", responseData, responseSize);
        return;
    }
    
    // Verifica a senha (no WYD original, isso era feito com MD5)
    // Simplificando aqui para fins de exemplo
    const std::string& dbPassword = result.rows[0][1]; // Assumindo que a senha é a segunda /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param password Parâmetro password
 * @return Retorna coluna
 */
 coluna
    if(dbPassword != password){
        // Senha incorreta
        CDBPacketManager::GetInstance().CreateErrorPacket(
            packet, PACKET_ERROR_AUTH, "Invalid password", responseData, responseSize);
        return;
    }
    
    // Verifica se a conta está banida
    const std::string& accountStatus = result.rows[0][2]; // Assumindo que o status é a terceira /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "1" Parâmetro "1"
 * @return Retorna coluna
 */
 coluna
    if(accountStatus == "1"){ // 1 = banida
        // Conta banida
        CDBPacketManager::GetInstance().CreateErrorPacket(
            packet, PACKET_ERROR_AUTH, "Account is banned", responseData, responseSize);
        return;
    }
    
    // Cria uma estrutura de conta para a resposta
    STRUCT_ACCOUNTINFO accountInfo;
    strncpy(accountInfo.szLogin, username, 32);
    accountInfo.byAccountStatus = 0; // 0 = normal
    accountInfo.byGMLevel = 0; // 0 = usuário comum
    accountInfo.dwLastLoginTime = static_cast<DWORD>(time(nullptr));
    accountInfo.byNumChars = 0; // Será preenchido depois
    accountInfo.byIsOnline = 1; // 1 = online
    
    // Busca o número de personagens
    query.queryString = "SELECT COUNT(*) FROM characters WHERE accountId = " + std::to_string(std::stoi(result.rows[0][0]));
    result = CDBConnectionManager::GetInstance().ExecuteQuery(query);
    
    if (result.success && !result.rows.empty()) {
        accountInfo.byNumChars = static_cast<BYTE>(std::stoi(result.rows[0][0]));
    }
    
    // Cria a resposta
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    // Copia a estrutura de conta para a resposta
    memcpy(responseData + sizeof(STRUCT_DB_PACKET), &accountInfo, sizeof(accountInfo));
    
    // Define o tamanho da resposta
    responseSize = sizeof(STRUCT_DB_PACKET) + sizeof(STRUCT_ACCOUNTINFO);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

// Implementações dos outros handlers (simplificadas para este exemplo)

void CDBServer::HandleAccountCreate(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // Esta função implementaria a criação de conta
    // Por simplicidade, apenas criamos uma resposta de sucesso
    
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    responseSize = sizeof(STRUCT_DB_PACKET);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

void CDBServer::HandleCharCreate(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // Esta função implementaria a criação de personagem
    // Por simplicidade, apenas criamos uma resposta de sucesso
    
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    responseSize = sizeof(STRUCT_DB_PACKET);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

void CDBServer::HandleCharLoad(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // Esta função implementaria o carregamento de personagem
    // Por simplicidade, apenas criamos uma resposta de sucesso com dados simulados
    
    STRUCT_CHARINFO charInfo;
    charInfo.dwCharID = 1;
    strncpy(charInfo.szName, "TestChar", 16);
    charInfo.byClass = 1; // TransKnight
    charInfo.byLevel = 1;
    
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    // Copia a estrutura de personagem para a resposta
    memcpy(responseData + sizeof(STRUCT_DB_PACKET), &charInfo, sizeof(charInfo));
    
    // Define o tamanho da resposta
    responseSize = sizeof(STRUCT_DB_PACKET) + sizeof(STRUCT_CHARINFO);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

void CDBServer::HandleCharSave(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // Esta função implementaria o salvamento de personagem
    // Por simplicidade, apenas criamos uma resposta de sucesso
    
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    responseSize = sizeof(STRUCT_DB_PACKET);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

void CDBServer::HandleInventoryLoad(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // Esta função implementaria o carregamento de inventário
    // Por simplicidade, apenas criamos uma resposta de sucesso com dados simulados
    
    STRUCT_INVENTORY inventory;
    
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    // Copia a estrutura de inventário para a resposta
    memcpy(responseData + sizeof(STRUCT_DB_PACKET), &inventory, sizeof(inventory));
    
    // Define o tamanho da resposta
    responseSize = sizeof(STRUCT_DB_PACKET) + sizeof(STRUCT_INVENTORY);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

void CDBServer::HandleInventorySave(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // Esta função implementaria o salvamento de inventário
    // Por simplicidade, apenas criamos uma resposta de sucesso
    
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    responseSize = sizeof(STRUCT_DB_PACKET);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

void CDBServer::HandleGuildLoad(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // Esta função implementaria o carregamento de guilda
    // Por simplicidade, apenas criamos uma resposta de sucesso com dados simulados
    
    STRUCT_GUILDINFO guildInfo;
    guildInfo.dwGuildID = 1;
    strncpy(guildInfo.szGuildName, "TestGuild", 12);
    guildInfo.byLevel = 1;
    
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    // Copia a estrutura de guilda para a resposta
    memcpy(responseData + sizeof(STRUCT_DB_PACKET), &guildInfo, sizeof(guildInfo));
    
    // Define o tamanho da resposta
    responseSize = sizeof(STRUCT_DB_PACKET) + sizeof(STRUCT_GUILDINFO);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

void CDBServer::HandleGuildSave(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // Esta função implementaria o salvamento de guilda
    // Por simplicidade, apenas criamos uma resposta de sucesso
    
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    responseSize = sizeof(STRUCT_DB_PACKET);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

void CDBServer::HandleAdminCommand(STRUCT_DB_PACKET* packet, const BYTE* data, int dataSize, BYTE* responseData, int& responseSize) {
    // Esta função implementaria o processamento de comando administrativo
    // Por simplicidade, apenas criamos uma resposta de sucesso
    
    STRUCT_DB_PACKET* responsePacket = reinterpret_cast<STRUCT_DB_PACKET*>(responseData);
    *responsePacket = *packet;
    responsePacket->byResult = PACKET_SUCCESS;
    responsePacket->dwServerTick = static_cast<DWORD>(time(nullptr));
    
    responseSize = sizeof(STRUCT_DB_PACKET);
    responsePacket->wSize = static_cast<WORD>(responseSize);
}

} // namespace server
} // namespace wyd