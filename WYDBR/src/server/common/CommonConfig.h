/**
 * @file CommonConfig.h
 * @brief Configurações comuns para servidores
 * @author WYDBR Team
 */

#ifndef WYDBR_COMMON_CONFIG_H
#define WYDBR_COMMON_CONFIG_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include "../../core/Logger.h"
#include "../../core/Config.h"
#include "../../core/Utils.h"

namespace wydbr {
namespace server {

/**
 * @class CommonServerConfig
 * @brief Configurações comuns para todos os servidores
 */
class CommonServerConfig {
public:
    /**
     * @brief Construtor padrão
     */
    CommonServerConfig() 
        : m_name("Unnamed"),
          m_ip("127.0.0.1"),
          m_port(0),
          m_threadCount(4),
          m_maxConnections(1000),
          m_maxConnectionsPerIP(5),
          m_connectionTimeout(300),
          m_maxPacketSize(8192),
          m_tickRateMs(100),
          m_maxEvents(1000),
          m_logLevel("INFO"),
          m_enableConsoleLogging(true),
          m_enableFileLogging(true),
          m_logRotationSize(100),
          m_maintenanceMode(false) {
    }

    /**
     * @brief Carrega configurações de um arquivo
     * @param filename Caminho do arquivo
     * @return true se sucesso, false caso contrário
     */
    bool load(const std::string& filename) {
        wydbr::core::ConfigManager& configManager = wydbr::core::ConfigManager::getInstance();
        
        if (!configManager.loadFromFile(filename)) {
            LOG_ERROR("Falha ao carregar arquivo de configuração: " + filename);
            return false;
        }
        
        // Carrega configurações básicas
        m_name = configManager.getString("server.name", m_name);
        m_ip = configManager.getString("server.ip", m_ip);
        m_port = configManager.getInt("server.port", m_port);
        m_threadCount = configManager.getInt("server.threadCount", m_threadCount);
        m_maxConnections = configManager.getInt("server.maxConnections", m_maxConnections);
        m_maxConnectionsPerIP = configManager.getInt("server.maxConnectionsPerIP", m_maxConnectionsPerIP);
        m_connectionTimeout = configManager.getInt("server.connectionTimeout", m_connectionTimeout);
        m_maxPacketSize = configManager.getInt("server.maxPacketSize", m_maxPacketSize);
        m_tickRateMs = configManager.getInt("server.tickRateMs", m_tickRateMs);
        m_maxEvents = configManager.getInt("server.maxEventHistory", m_maxEvents);
        
        // Configurações de log
        m_logLevel = configManager.getString("logging.level", m_logLevel);
        m_enableConsoleLogging = configManager.getBool("logging.console", m_enableConsoleLogging);
        m_enableFileLogging = configManager.getBool("logging.file", m_enableFileLogging);
        m_logRotationSize = configManager.getInt("logging.rotationSizeMB", m_logRotationSize);
        
        // Configurações de conexão
        m_maintenanceMode = configManager.getBool("server.maintenanceMode", m_maintenanceMode);
        
        // Configurações de rede
        m_allowedIPs = configManager.getStringArray("network.allowedIPs");
        m_bannedIPs = configManager.getStringArray("network.bannedIPs");
        
        // Configurações de segurança
        m_enableAuthentication = configManager.getBool("security.enableAuthentication", true);
        m_enableEncryption = configManager.getBool("security.enableEncryption", true);
        m_encryptionKey = configManager.getString("security.encryptionKey", "default_key");
        
        // Carrega conexões a outros servidores
        loadServerConnections(configManager);
        
        // Carrega configurações específicas do servidor
        loadServerSpecificConfig(configManager);
        
        // Configura logger com base nas configurações
        configureLogger();
        
        return true;
    }

    /**
     * @brief Salva configurações em um arquivo
     * @param filename Caminho do arquivo
     * @return true se sucesso, false caso contrário
     */
    bool save(const std::string& filename) const {
        wydbr::core::ConfigManager& configManager = wydbr::core::ConfigManager::getInstance();
        
        // Configurações básicas
        configManager.setValue("server.name", m_name);
        configManager.setValue("server.ip", m_ip);
        configManager.setValue("server.port", m_port);
        configManager.setValue("server.threadCount", m_threadCount);
        configManager.setValue("server.maxConnections", m_maxConnections);
        configManager.setValue("server.maxConnectionsPerIP", m_maxConnectionsPerIP);
        configManager.setValue("server.connectionTimeout", m_connectionTimeout);
        configManager.setValue("server.maxPacketSize", m_maxPacketSize);
        configManager.setValue("server.tickRateMs", m_tickRateMs);
        configManager.setValue("server.maxEventHistory", m_maxEvents);
        configManager.setValue("server.maintenanceMode", m_maintenanceMode);
        
        // Configurações de log
        configManager.setValue("logging.level", m_logLevel);
        configManager.setValue("logging.console", m_enableConsoleLogging);
        configManager.setValue("logging.file", m_enableFileLogging);
        configManager.setValue("logging.rotationSizeMB", m_logRotationSize);
        
        // Configurações de rede
        configManager.setArray("network.allowedIPs", m_allowedIPs);
        configManager.setArray("network.bannedIPs", m_bannedIPs);
        
        // Configurações de segurança
        configManager.setValue("security.enableAuthentication", m_enableAuthentication);
        configManager.setValue("security.enableEncryption", m_enableEncryption);
        configManager.setValue("security.encryptionKey", m_encryptionKey);
        
        // Salva conexões a outros servidores
        saveServerConnections(configManager);
        
        // Salva configurações específicas do servidor
        saveServerSpecificConfig(configManager);
        
        return configManager.saveToFile(filename);
    }

    /**
     * @brief Obtém o nome do servidor
     * @return Nome do servidor
     */
    const std::string& getName() const {
        return m_name;
    }

    /**
     * @brief Define o nome do servidor
     * @param name Novo nome
     */
    void setName(const std::string& name) {
        m_name = name;
    }

    /**
     * @brief Obtém o IP do servidor
     * @return IP do servidor
     */
    const std::string& getIP() const {
        return m_ip;
    }

    /**
     * @brief Define o IP do servidor
     * @param ip Novo IP
     */
    void setIP(const std::string& ip) {
        m_ip = ip;
    }

    /**
     * @brief Obtém a porta do servidor
     * @return Porta do servidor
     */
    uint16_t getPort() const {
        return m_port;
    }

    /**
     * @brief Define a porta do servidor
     * @param port Nova porta
     */
    void setPort(uint16_t port) {
        m_port = port;
    }

    /**
     * @brief Obtém o número de threads do servidor
     * @return Número de threads
     */
    uint16_t getThreadCount() const {
        return m_threadCount;
    }

    /**
     * @brief Define o número de threads do servidor
     * @param count Novo número de threads
     */
    void setThreadCount(uint16_t count) {
        m_threadCount = count;
    }

    /**
     * @brief Obtém o número máximo de conexões
     * @return Número máximo de conexões
     */
    uint32_t getMaxConnections() const {
        return m_maxConnections;
    }

    /**
     * @brief Define o número máximo de conexões
     * @param max Novo número máximo de conexões
     */
    void setMaxConnections(uint32_t max) {
        m_maxConnections = max;
    }

    /**
     * @brief Obtém o número máximo de conexões por IP
     * @return Número máximo de conexões por IP
     */
    uint16_t getMaxConnectionsPerIP() const {
        return m_maxConnectionsPerIP;
    }

    /**
     * @brief Define o número máximo de conexões por IP
     * @param max Novo número máximo de conexões por IP
     */
    void setMaxConnectionsPerIP(uint16_t max) {
        m_maxConnectionsPerIP = max;
    }

    /**
     * @brief Obtém o timeout de conexão em segundos
     * @return Timeout de conexão
     */
    uint32_t getConnectionTimeout() const {
        return m_connectionTimeout;
    }

    /**
     * @brief Define o timeout de conexão em segundos
     * @param timeout Novo timeout de conexão
     */
    void setConnectionTimeout(uint32_t timeout) {
        m_connectionTimeout = timeout;
    }

    /**
     * @brief Obtém o tamanho máximo de pacotes
     * @return Tamanho máximo de pacotes
     */
    uint32_t getMaxPacketSize() const {
        return m_maxPacketSize;
    }

    /**
     * @brief Define o tamanho máximo de pacotes
     * @param size Novo tamanho máximo de pacotes
     */
    void setMaxPacketSize(uint32_t size) {
        m_maxPacketSize = size;
    }

    /**
     * @brief Obtém a taxa de ticks em milissegundos
     * @return Taxa de ticks
     */
    uint16_t getTickRateMs() const {
        return m_tickRateMs;
    }

    /**
     * @brief Define a taxa de ticks em milissegundos
     * @param tickRateMs Nova taxa de ticks
     */
    void setTickRateMs(uint16_t tickRateMs) {
        m_tickRateMs = tickRateMs;
    }

    /**
     * @brief Obtém o número máximo de eventos no histórico
     * @return Número máximo de eventos
     */
    size_t getMaxEventHistory() const {
        return m_maxEvents;
    }

    /**
     * @brief Define o número máximo de eventos no histórico
     * @param max Novo número máximo de eventos
     */
    void setMaxEventHistory(size_t max) {
        m_maxEvents = max;
    }

    /**
     * @brief Verifica se o modo de manutenção está ativo
     * @return true se em manutenção, false caso contrário
     */
    bool isInMaintenanceMode() const {
        return m_maintenanceMode;
    }

    /**
     * @brief Define o modo de manutenção
     * @param enabled Estado do modo de manutenção
     */
    void setMaintenanceMode(bool enabled) {
        m_maintenanceMode = enabled;
    }

    /**
     * @brief Obtém o nível de log
     * @return Nível de log
     */
    const std::string& getLogLevel() const {
        return m_logLevel;
    }

    /**
     * @brief Define o nível de log
     * @param level Novo nível de log
     */
    void setLogLevel(const std::string& level) {
        m_logLevel = level;
        configureLogger();
    }

    /**
     * @brief Verifica se o log em console está ativo
     * @return true se ativo, false caso contrário
     */
    bool isConsoleLoggingEnabled() const {
        return m_enableConsoleLogging;
    }

    /**
     * @brief Define o estado do log em console
     * @param enabled Novo estado
     */
    void setConsoleLoggingEnabled(bool enabled) {
        m_enableConsoleLogging = enabled;
        configureLogger();
    }

    /**
     * @brief Verifica se o log em arquivo está ativo
     * @return true se ativo, false caso contrário
     */
    bool isFileLoggingEnabled() const {
        return m_enableFileLogging;
    }

    /**
     * @brief Define o estado do log em arquivo
     * @param enabled Novo estado
     */
    void setFileLoggingEnabled(bool enabled) {
        m_enableFileLogging = enabled;
        configureLogger();
    }

    /**
     * @brief Obtém o tamanho de rotação de logs em MB
     * @return Tamanho de rotação
     */
    uint32_t getLogRotationSize() const {
        return m_logRotationSize;
    }

    /**
     * @brief Define o tamanho de rotação de logs em MB
     * @param sizeMB Novo tamanho de rotação
     */
    void setLogRotationSize(uint32_t sizeMB) {
        m_logRotationSize = sizeMB;
        configureLogger();
    }

    /**
     * @brief Obtém a lista de IPs permitidos
     * @return Lista de IPs permitidos
     */
    const std::vector<std::string>& getAllowedIPs() const {
        return m_allowedIPs;
    }

    /**
     * @brief Define a lista de IPs permitidos
     * @param ips Nova lista de IPs
     */
    void setAllowedIPs(const std::vector<std::string>& ips) {
        m_allowedIPs = ips;
    }

    /**
     * @brief Adiciona um IP à lista de permitidos
     * @param ip IP a adicionar
     */
    void addAllowedIP(const std::string& ip) {
        if (std::find(m_allowedIPs.begin(), m_allowedIPs.end(), ip) == m_allowedIPs.end()) {
            m_allowedIPs.push_back(ip);
        }
    }

    /**
     * @brief Remove um IP da lista de permitidos
     * @param ip IP a remover
     * @return true se removido, false se não encontrado
     */
    bool removeAllowedIP(const std::string& ip) {
        auto it = std::find(m_allowedIPs.begin(), m_allowedIPs.end(), ip);
        if (it != m_allowedIPs.end()) {
            m_allowedIPs.erase(it);
            return true;
        }
        return false;
    }

    /**
     * @brief Obtém a lista de IPs banidos
     * @return Lista de IPs banidos
     */
    const std::vector<std::string>& getBannedIPs() const {
        return m_bannedIPs;
    }

    /**
     * @brief Define a lista de IPs banidos
     * @param ips Nova lista de IPs
     */
    void setBannedIPs(const std::vector<std::string>& ips) {
        m_bannedIPs = ips;
    }

    /**
     * @brief Adiciona um IP à lista de banidos
     * @param ip IP a adicionar
     */
    void addBannedIP(const std::string& ip) {
        if (std::find(m_bannedIPs.begin(), m_bannedIPs.end(), ip) == m_bannedIPs.end()) {
            m_bannedIPs.push_back(ip);
        }
    }

    /**
     * @brief Remove um IP da lista de banidos
     * @param ip IP a remover
     * @return true se removido, false se não encontrado
     */
    bool removeBannedIP(const std::string& ip) {
        auto it = std::find(m_bannedIPs.begin(), m_bannedIPs.end(), ip);
        if (it != m_bannedIPs.end()) {
            m_bannedIPs.erase(it);
            return true;
        }
        return false;
    }

    /**
     * @brief Verifica se a autenticação está habilitada
     * @return true se habilitada, false caso contrário
     */
    bool isAuthenticationEnabled() const {
        return m_enableAuthentication;
    }

    /**
     * @brief Define o estado da autenticação
     * @param enabled Novo estado
     */
    void setAuthenticationEnabled(bool enabled) {
        m_enableAuthentication = enabled;
    }

    /**
     * @brief Verifica se a criptografia está habilitada
     * @return true se habilitada, false caso contrário
     */
    bool isEncryptionEnabled() const {
        return m_enableEncryption;
    }

    /**
     * @brief Define o estado da criptografia
     * @param enabled Novo estado
     */
    void setEncryptionEnabled(bool enabled) {
        m_enableEncryption = enabled;
    }

    /**
     * @brief Obtém a chave de criptografia
     * @return Chave de criptografia
     */
    const std::string& getEncryptionKey() const {
        return m_encryptionKey;
    }

    /**
     * @brief Define a chave de criptografia
     * @param key Nova chave
     */
    void setEncryptionKey(const std::string& key) {
        m_encryptionKey = key;
    }

    /**
     * @brief Obtém informações de conexão a um servidor
     * @param serverType Tipo do servidor
     * @return Par com IP e porta, ou valores vazios se não encontrado
     */
    std::pair<std::string, uint16_t> getServerConnection(const std::string& serverType) const {
        auto it = m_serverConnections.find(serverType);
        if (it != m_serverConnections.end()) {
            return it->second;
        }
        return std::make_pair("", 0);
    }

    /**
     * @brief Define informações de conexão a um servidor
     * @param serverType Tipo do servidor
     * @param ip IP do servidor
     * @param port Porta do servidor
     */
    void setServerConnection(const std::string& serverType, const std::string& ip, uint16_t port) {
        m_serverConnections[serverType] = std::make_pair(ip, port);
    }

    /**
     * @brief Remove informações de conexão a um servidor
     * @param serverType Tipo do servidor
     * @return true se removido, false se não encontrado
     */
    bool removeServerConnection(const std::string& serverType) {
        return m_serverConnections.erase(serverType) > 0;
    }

    /**
     * @brief Obtém todas as conexões de servidores
     * @return Mapa com todas as conexões
     */
    const std::unordered_map<std::string, std::pair<std::string, uint16_t>>& getServerConnections() const {
        return m_serverConnections;
    }

protected:
    /**
     * @brief Carrega configurações específicas do servidor
     * @param configManager Gerenciador de configuração
     * 
     * Função a ser sobrescrita por classes derivadas para carregar configurações específicas
     */
    virtual void loadServerSpecificConfig(const wydbr::core::ConfigManager& configManager) {
        // Implementação vazia na classe base
    }
    
    /**
     * @brief Salva configurações específicas do servidor
     * @param configManager Gerenciador de configuração
     * 
     * Função a ser sobrescrita por classes derivadas para salvar configurações específicas
     */
    virtual void saveServerSpecificConfig(wydbr::core::ConfigManager& configManager) const {
        // Implementação vazia na classe base
    }

private:
    /**
     * @brief Configura o logger com base nas configurações atuais
     */
    void configureLogger() {
        wydbr::core::LogConfig logConfig;
        
        // Define o diretório de logs
        logConfig.logDirectory = "logs";
        
        // Define o prefixo dos arquivos de log
        logConfig.logFilePrefix = m_name;
        
        // Converte string de nível para enum
        wydbr::core::LogLevel level = wydbr::core::LogLevel::INFO;
        std::string levelStr = wydbr::core::utils::string::toUpper(m_logLevel);
        
        if (levelStr == "TRACE") level = wydbr::core::LogLevel::TRACE;
        else if (levelStr == "DEBUG") level = wydbr::core::LogLevel::DEBUG;
        else if (levelStr == "INFO") level = wydbr::core::LogLevel::INFO;
        else if (levelStr == "WARNING" || levelStr == "WARN") level = wydbr::core::LogLevel::WARNING;
        else if (levelStr == "ERROR") level = wydbr::core::LogLevel::ERROR;
        else if (levelStr == "CRITICAL") level = wydbr::core::LogLevel::CRITICAL;
        
        // Configura níveis de log
        logConfig.consoleLevel = level;
        logConfig.fileLevel = level;
        
        // Configura opções de logging
        logConfig.enableConsoleLogging = m_enableConsoleLogging;
        logConfig.enableFileLogging = m_enableFileLogging;
        logConfig.enableColoredOutput = true;
        logConfig.enableTimestamps = true;
        
        // Configura rotação de logs
        logConfig.maxFileSizeMB = m_logRotationSize;
        logConfig.maxLogFiles = 10;
        
        // Aplica configurações ao logger
        wydbr::core::Logger::getInstance().configure(logConfig);
    }
    
    /**
     * @brief Carrega conexões a outros servidores
     * @param configManager Gerenciador de configuração
     */
    void loadServerConnections(const wydbr::core::ConfigManager& configManager) {
        m_serverConnections.clear();
        
        // Obtém todas as chaves que começam com "connection."
        std::vector<std::string> keys = configManager.getAllKeys();
        
        for (const auto& key : keys) {
            if (key.find("connection.") == 0) {
                // Extrai o tipo de servidor
                std::string serverType = key.substr(11); // Length of "connection."
                
                // Obtém IP e porta
                std::string connectionString = configManager.getString(key);
                auto parts = wydbr::core::utils::string::split(connectionString, ':');
                
                if (parts.size() == 2) {
                    std::string ip = parts[0];
                    uint16_t port = wydbr::core::utils::string::fromString<uint16_t>(parts[1], 0);
                    
                    if (!ip.empty() && port > 0) {
                        m_serverConnections[serverType] = std::make_pair(ip, port);
                    }
                }
            }
        }
    }
    
    /**
     * @brief Salva conexões a outros servidores
     * @param configManager Gerenciador de configuração
     */
    void saveServerConnections(wydbr::core::ConfigManager& configManager) const {
        // Remove todas as configurações de conexão anteriores
        std::vector<std::string> keys = configManager.getAllKeys();
        for (const auto& key : keys) {
            if (key.find("connection.") == 0) {
                configManager.removeKey(key);
            }
        }
        
        // Adiciona as configurações atuais
        for (const auto& [serverType, connection] : m_serverConnections) {
            const std::string& ip = connection.first;
            uint16_t port = connection.second;
            
            std::string connectionString = ip + ":" + std::to_string(port);
            configManager.setValue("connection." + serverType, connectionString);
        }
    }

private:
    std::string m_name;             ///< Nome do servidor
    std::string m_ip;               ///< IP do servidor
    uint16_t m_port;                ///< Porta do servidor
    uint16_t m_threadCount;         ///< Número de threads
    uint32_t m_maxConnections;      ///< Número máximo de conexões
    uint16_t m_maxConnectionsPerIP; ///< Número máximo de conexões por IP
    uint32_t m_connectionTimeout;   ///< Timeout de conexão em segundos
    uint32_t m_maxPacketSize;       ///< Tamanho máximo de pacotes
    uint16_t m_tickRateMs;          ///< Taxa de ticks em milissegundos
    size_t m_maxEvents;             ///< Número máximo de eventos no histórico
    
    // Log
    std::string m_logLevel;         ///< Nível de log
    bool m_enableConsoleLogging;    ///< Flag para habilitar log em console
    bool m_enableFileLogging;       ///< Flag para habilitar log em arquivo
    uint32_t m_logRotationSize;     ///< Tamanho de rotação de logs em MB
    
    // Rede
    std::vector<std::string> m_allowedIPs;  ///< Lista de IPs permitidos
    std::vector<std::string> m_bannedIPs;   ///< Lista de IPs banidos
    bool m_maintenanceMode;                 ///< Flag de modo de manutenção
    
    // Segurança
    bool m_enableAuthentication;    ///< Flag para habilitar autenticação
    bool m_enableEncryption;        ///< Flag para habilitar criptografia
    std::string m_encryptionKey;    ///< Chave de criptografia
    
    // Conexões a outros servidores
    std::unordered_map<std::string, std::pair<std::string, uint16_t>> m_serverConnections;
};

} // namespace server
} // namespace wydbr

#endif // WYDBR_COMMON_CONFIG_H 