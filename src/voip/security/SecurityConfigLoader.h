/**
 * @file SecurityConfigLoader.h
 * @brief Carregador de configuração de segurança para VoIP
 */

#ifndef WYDBR_VOIP_SECURITY_CONFIG_LOADER_H
#define WYDBR_VOIP_SECURITY_CONFIG_LOADER_H

#include "VoIPSecurityManager.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace WYDBR {
namespace VoIP {

class SecurityConfigLoader {
public:
    SecurityConfigLoader();
    ~SecurityConfigLoader();

    bool LoadConfig(const std::string& configPath);
    SecurityConfig GetConfig() const;
    bool ValidateConfig() const;
    bool UpdateConfig(const SecurityConfig& config);
    bool SaveConfig(const std::string& configPath);

private:
    SecurityConfig m_config;
    std::string m_configPath;
    bool m_initialized;

    bool ParseConfigFile();
    bool ParseGeneralSection(const std::unordered_map<std::string, std::string>& section);
    bool ParseEncryptionSection(const std::unordered_map<std::string, std::string>& section);
    bool ParseAuthenticationSection(const std::unordered_map<std::string, std::string>& section);
    bool ParseChannelSecuritySection(const std::unordered_map<std::string, std::string>& section);
    bool ParseUserSecuritySection(const std::unordered_map<std::string, std::string>& section);
    bool ParseIPFilteringSection(const std::unordered_map<std::string, std::string>& section);
    bool ParseMonitoringSection(const std::unordered_map<std::string, std::string>& section);
    bool ParseAdvancedSection(const std::unordered_map<std::string, std::string>& section);
    bool ParseDebugSection(const std::unordered_map<std::string, std::string>& section);

    std::unordered_map<std::string, std::string> ParseSection(const std::string& sectionName);
    bool WriteSection(std::ofstream& file, const std::string& sectionName, 
                     const std::unordered_map<std::string, std::string>& values);
};

} // namespace VoIP
} // namespace WYDBR

#endif // WYDBR_VOIP_SECURITY_CONFIG_LOADER_H 