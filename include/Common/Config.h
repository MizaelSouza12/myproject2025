
/**
 * @file Config.h
 * @brief Define configurações globais para o WYDBR 2.0
 */

#ifndef WYDBR_CONFIG_H
#define WYDBR_CONFIG_H

#include <string>
#include <cstdint>

namespace WYDBR {

// Configurações de rede
constexpr uint16_t DEFAULT_TMSRV_PORT = 8281;
constexpr uint16_t DEFAULT_DBSRV_PORT = 8282;
constexpr uint16_t DEFAULT_VOIP_PORT = 8283;
constexpr uint16_t DEFAULT_SCREEN_SHARE_PORT = 8284;

// Configurações de tamanho
constexpr int MAX_PACKET_SIZE = 8192;
constexpr int MAX_CLIENTS = 2000;
constexpr int MAX_VOIP_CHANNELS = 100;
constexpr int MAX_SCREEN_SHARES = 50;

// Configuração de caminhos
#ifdef _WIN32
constexpr const char* DEFAULT_CONFIG_PATH = "./config/";
#else
constexpr const char* DEFAULT_CONFIG_PATH = "./config/";
#endif

/**
 * Classe de configuração global
 */
class GlobalConfig {
public:
    static GlobalConfig& GetInstance();
    
    bool Initialize(const std::string& configPath = DEFAULT_CONFIG_PATH);
    
    // Getters para várias configurações
    const std::string& GetDataPath() const;
    const std::string& GetLogPath() const;
    uint16_t GetTMSrvPort() const;
    uint16_t GetDBSrvPort() const;
    uint16_t GetVoIPPort() const;
    uint16_t GetScreenSharePort() const;
    
private:
    GlobalConfig() = default;
    ~GlobalConfig() = default;
    
    // Evitar cópias
    GlobalConfig(const GlobalConfig&) = delete;
    GlobalConfig& operator=(const GlobalConfig&) = delete;
    
    // Dados internos
    std::string m_dataPath;
    std::string m_logPath;
    uint16_t m_tmSrvPort = DEFAULT_TMSRV_PORT;
    uint16_t m_dbSrvPort = DEFAULT_DBSRV_PORT;
    uint16_t m_voipPort = DEFAULT_VOIP_PORT;
    uint16_t m_screenSharePort = DEFAULT_SCREEN_SHARE_PORT;
};

} // namespace WYDBR

#endif // WYDBR_CONFIG_H
