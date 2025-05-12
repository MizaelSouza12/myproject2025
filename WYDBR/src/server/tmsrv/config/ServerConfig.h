/**
 * ServerConfig.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/config/ServerConfig.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../utils/Logger.h"
#include "../database/DatabaseTypes.h"
#include "../world/WorldTypes.h"
#include "../npc/NPCTypes.h"
#include "../player/PlayerTypes.h"

namespace wydbr {


namespace wyd {
namespace server {

/**
 * @brief Classe para gerenciamento de configurações do servidor
 * 
 * Esta classe carrega e gerencia as configurações do servidor a partir
 * de um arquivo INI, fornecendo métodos para acessar essas configurações.
 */
/**
 * Classe WYDServerConfig
 * 
 * Esta classe implementa a funcionalidade WYDServerConfig conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ServerConfig {
public:
    /**
     * @brief Construtor padrão
     */
    ServerConfig() /**
 * loaded_
 * 
 * Implementa a funcionalidade loaded_ conforme especificação original.
 * @param false Parâmetro false
 * @return Retorna :
 */
 : loaded_(false){}
    
    /**
     * @brief Destrutor
     */
    ~ServerConfig() {}
    
    /**
     * @brief Carrega configurações de um arquivo
     * @param filename Caminho do arquivo de configuração
     * @return true se as configurações foram carregadas com sucesso, false caso contrário
     */
    bool Load(const std::string& filename) {
        /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filename Parâmetro filename
 * @return Retorna std::ifstream
 */

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erro ao abrir arquivo de configuração: " << filename << std::endl;
            return false;
        }
        
        std::string line;
        std::string section;
        
        while (std::getline(file, line)) {
            // Remove espaços em branco no início e fim
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            // Ignora linhas vazias e comentários
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue;
            }
            
            // Nova seção
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                section = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Chave/valor
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Remove espaços em branco
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // Armazena no mapa de configurações
                config_[section + "." + key] = value;
            }
        }
        
        file.close();
        loaded_ = true;
        
        // Carrega configurações especí/**
 * LoadSpecificConfigs
 * 
 * Implementa a funcionalidade LoadSpecificConfigs conforme especificação original.
 * @return Retorna ficas
 */
ficas
        LoadSpecificConfigs();
        
        return true;
    }
    
    /**
     * @brief Obtém uma configuração como string
     * @param section Seção da configuração
     * @param key Chave da configuração
     * @param defaultValue Valor padrão caso a configuração não exista
     * @return Valor da configuração ou o valor padrão
     */
    std::string GetString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const {
        std::string fullKey = section + "." + key;
        auto it = config_.find(fullKey);
        if (it != config_.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    /**
     * @brief Obtém uma configuração como inteiro
     * @param section Seção da configuração
     * @param key Chave da configuração
     * @param defaultValue Valor padrão caso a configuração não exista
     * @return Valor da configuração como inteiro ou o valor padrão
     */
    int GetInt(const std::string& section, const std::string& key, int defaultValue = 0) const {
        std::string value = GetString(section, key, "");
        if (!value.empty()) {
            try {
                return std::stoi(value);
            } catch (const std::exception& e) {
                std::cerr << "Erro ao converter valor para inteiro: " << value << std::endl;
            }
        }
        return defaultValue;
    }
    
    /**
     * @brief Obtém uma configuração como float
     * @param section Seção da configuração
     * @param key Chave da configuração
     * @param defaultValue Valor padrão caso a configuração não exista
     * @return Valor da configuração como float ou o valor padrão
     */
    float GetFloat(const std::string& section, const std::string& key, float defaultValue = 0.0f) const {
        std::string value = GetString(section, key, "");
        if (!value.empty()) {
            try {
                return std::stof(value);
            } catch (const std::exception& e) {
                std::cerr << "Erro ao converter valor para float: " << value << std::endl;
            }
        }
        return defaultValue;
    }
    
    /**
     * @brief Obtém uma configuração como booleano
     * @param section Seção da configuração
     * @param key Chave da configuração
     * @param defaultValue Valor padrão caso a configuração não exista
     * @return Valor da configuração como booleano ou o valor padrão
     */
    bool GetBool(const std::string& section, const std::string& key, bool defaultValue = false) const {
        std::string value = GetString(section, key, "");
        if (!value.empty()) {
            return (value == "1" || value == "true" || value == "yes" || value == "on");
        }
        return defaultValue;
    }
    
    /**
     * @brief Obtém uma configuração como array
     * @param section Seção da configuração
     * @param key Chave da configuração
     * @param delimiter Delimitador entre os elementos
     * @return Valor da configuração como array de strings
     */
    std::vector<std::string> GetArray(const std::string& section, const std::string& key, char delimiter = ',') const {
        std::vector<std::string> result;
        std::string value = GetString(section, key, "");
        
        if (!value.empty()) {
            /**
 * ss
 * 
 * Implementa a funcionalidade ss conforme especificação original.
 * @param value Parâmetro value
 * @return Retorna std::stringstream
 */

            std::stringstream ss(value);
            std::string item;
            
            while (std::getline(ss, item, delimiter)) {
                // Remove espaços em branco
                item.erase(0, item.find_first_not_of(" \t"));
                item.erase(item.find_last_not_of(" \t") + 1);
                
                if (!item.empty()) {
                    result.push_back(item);
                }
            }
        }
        
        return result;
    }
    
    // Métodos específicos para configurações do servidor
    
    /**
     * @brief Obtém o ID do servidor
     * @return ID do servidor
     */
    WORD GetServerID() const {
        return static_cast<WORD>(GetInt("Server", "ServerID", 1));
    }
    
    /**
     * @brief Obtém o ID do grupo de servidores
     * @return ID do grupo de servidores
     */
    WORD GetServerGroupID() const {
        return static_cast<WORD>(GetInt("Server", "ServerGroupID", 1));
    }
    
    /**
     * @brief Obtém o nome do servidor
     * @return Nome do servidor
     */
    std::string GetServerName() const {
        /**
 * GetString
 * 
 * Implementa a funcionalidade GetString conforme especificação original.
 * @param "Server" Parâmetro "Server"
 * @param "ServerName" Parâmetro "ServerName"
 * @param "TMSrv" Parâmetro "TMSrv"
 * @return Retorna return
 */

        return GetString("Server", "ServerName", "TMSrv");
    }
    
    /**
     * @brief Obtém o IP público do servidor
     * @return IP público do servidor
     */
    std::string GetServerPublicIP() const {
        /**
 * GetString
 * 
 * Implementa a funcionalidade GetString conforme especificação original.
 * @param "Server" Parâmetro "Server"
 * @param "PublicIP" Parâmetro "PublicIP"
 * @param "127.0.0.1" Parâmetro "127.0.0.1"
 * @return Retorna return
 */

        return GetString("Server", "PublicIP", "127.0.0.1");
    }
    
    /**
     * @brief Obtém o IP de bind do servidor
     * @return IP de bind do servidor
     */
    std::string GetServerBindIP() const {
        /**
 * GetString
 * 
 * Implementa a funcionalidade GetString conforme especificação original.
 * @param "Server" Parâmetro "Server"
 * @param "BindIP" Parâmetro "BindIP"
 * @param "0.0.0.0" Parâmetro "0.0.0.0"
 * @return Retorna return
 */

        return GetString("Server", "BindIP", "0.0.0.0");
    }
    
    /**
     * @brief Obtém a porta do servidor
     * @return Porta do servidor
     */
    int GetServerPort() const {
        /**
 * GetInt
 * 
 * Implementa a funcionalidade GetInt conforme especificação original.
 * @param "Server" Parâmetro "Server"
 * @param "Port" Parâmetro "Port"
 * @param 8281 Parâmetro 8281
 * @return Retorna return
 */

        return GetInt("Server", "Port", 8281);
    }
    
    /**
     * @brief Obtém o número máximo de conexões
     * @return Número máximo de conexões
     */
    int GetMaxConnections() const {
        /**
 * GetInt
 * 
 * Implementa a funcionalidade GetInt conforme especificação original.
 * @param "Server" Parâmetro "Server"
 * @param "MaxConnections" Parâmetro "MaxConnections"
 * @param 1000 Parâmetro 1000
 * @return Retorna return
 */

        return GetInt("Server", "MaxConnections", 1000);
    }
    
    /**
     * @brief Obtém o número máximo de jogadores
     * @return Número máximo de jogadores
     */
    int GetMaxPlayers() const {
        /**
 * GetInt
 * 
 * Implementa a funcionalidade GetInt conforme especificação original.
 * @param "Server" Parâmetro "Server"
 * @param "MaxPlayers" Parâmetro "MaxPlayers"
 * @param 1000 Parâmetro 1000
 * @return Retorna return
 */

        return GetInt("Server", "MaxPlayers", 1000);
    }
    
    /**
     * @brief Obtém o número máximo de NPCs
     * @return Número máximo de NPCs
     */
    int GetMaxNPCs() const {
        /**
 * GetInt
 * 
 * Implementa a funcionalidade GetInt conforme especificação original.
 * @param "Server" Parâmetro "Server"
 * @param "MaxNPCs" Parâmetro "MaxNPCs"
 * @param 10000 Parâmetro 10000
 * @return Retorna return
 */

        return GetInt("Server", "MaxNPCs", 10000);
    }
    
    /**
     * @brief Obtém a configuração do banco de dados
     * @return Configuração do banco de dados
     */
    DatabaseConfig GetDatabaseConfig() const {
        DatabaseConfig config;
        config.host = GetString("Database", "Host", "localhost");
        config.port = GetInt("Database", "Port", 3306);
        config.username = GetString("Database", "Username", "root");
        config.password = GetString("Database", "Password", "");
        config.database = GetString("Database", "Database", "wyd");
        config.poolSize = GetInt("Database", "PoolSize", 10);
        config.connectionTimeout = GetInt("Database", "ConnectionTimeout", 5000);
        config.commandTimeout = GetInt("Database", "CommandTimeout", 30000);
        return config;
    }
    
    /**
     * @brief Obtém a configuração do mundo
     * @return Configuração do mundo
     */
    WorldConfig GetWorldConfig() const {
        WorldConfig config;
        config.mapDir = GetString("World", "MapDir", "data/maps");
        config.maxMaps = GetInt("World", "MaxMaps", 100);
        config.gridSize = GetInt("World", "GridSize", 16);
        config.viewDistance = GetInt("World", "ViewDistance", 20);
        config.dayLengthMinutes = GetInt("World", "DayLengthMinutes", 120);
        return config;
    }
    
    /**
     * @brief Obtém a configuração de NPCs
     * @return Configuração de NPCs
     */
    NPCConfig GetNPCConfig() const {
        NPCConfig config;
        config.npcDir = GetString("NPC", "NPCDir", "data/npcs");
        config.maxPerZone = GetInt("NPC", "MaxPerZone", 500);
        config.spawnInterval = GetInt("NPC", "SpawnInterval", 60000);
        config.despawnDistance = GetInt("NPC", "DespawnDistance", 100);
        config.updateInterval = GetInt("NPC", "UpdateInterval", 1000);
        return config;
    }
    
    /**
     * @brief Obtém a configuração de jogadores
     * @return Configuração de jogadores
     */
    PlayerConfig GetPlayerConfig() const {
        PlayerConfig config;
        config.startMap = GetInt("Player", "StartMap", 1);
        config.startX = GetInt("Player", "StartX", 2100);
        config.startY = GetInt("Player", "StartY", 2100);
        config.maxLevel = GetInt("Player", "MaxLevel", 400);
        config.expRate = GetFloat("Player", "ExpRate", 1.0f);
        config.dropRate = GetFloat("Player", "DropRate", 1.0f);
        config.goldRate = GetFloat("Player", "GoldRate", 1.0f);
        config.maxInventorySize = GetInt("Player", "MaxInventorySize", 64);
        config.maxStorageSize = GetInt("Player", "MaxStorageSize", 120);
        return config;
    }
    
private:
    /**
     * @brief Carrega configurações específicas
     */
    void LoadSpecificConfigs() {
        // Pode ser implementado se necessário para carregar configurações específicas
    }
    
    bool loaded_;                               // Flag de configuração carregada
    std::map<std::string, std::string> config_; // Mapa de configurações
};

} // namespace server
} // namespace wyd

#endif // SERVER_CONFIG_H

} // namespace wydbr
