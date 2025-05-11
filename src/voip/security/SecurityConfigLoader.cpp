/**
 * @file SecurityConfigLoader.cpp
 * @brief Implementação do carregador de configuração de segurança para VoIP
 */

#include "SecurityConfigLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace WYDBR {
namespace VoIP {

SecurityConfigLoader::SecurityConfigLoader()
    : m_initialized(false)
{
}

SecurityConfigLoader::~SecurityConfigLoader()
{
}

bool SecurityConfigLoader::LoadConfig(const std::string& configPath)
{
    m_configPath = configPath;
    m_initialized = ParseConfigFile();
    return m_initialized;
}

SecurityConfig SecurityConfigLoader::GetConfig() const
{
    return m_config;
}

bool SecurityConfigLoader::ValidateConfig() const
{
    if (!m_initialized) {
        return false;
    }

    // Validar configurações gerais
    if (m_config.enableEncryption && m_config.encryptionType.empty()) {
        return false;
    }

    if (m_config.enableIPFiltering && m_config.allowedIPs.empty()) {
        return false;
    }

    return true;
}

bool SecurityConfigLoader::UpdateConfig(const SecurityConfig& config)
{
    m_config = config;
    return ValidateConfig();
}

bool SecurityConfigLoader::SaveConfig(const std::string& configPath)
{
    if (!m_initialized) {
        return false;
    }

    try {
        std::ofstream file(configPath);
        if (!file.is_open()) {
            return false;
        }

        // Escrever seção General
        std::unordered_map<std::string, std::string> generalValues = {
            {"Version", "1.0"},
            {"EnableEncryption", m_config.enableEncryption ? "true" : "false"},
            {"EnableAuthentication", m_config.enableAuthentication ? "true" : "false"},
            {"EnableChannelEncryption", m_config.enableChannelEncryption ? "true" : "false"},
            {"EnableUserAuthentication", m_config.enableUserAuthentication ? "true" : "false"},
            {"EnableChannelAuthentication", m_config.enableChannelAuthentication ? "true" : "false"},
            {"EnableIPFiltering", m_config.enableIPFiltering ? "true" : "false"}
        };
        WriteSection(file, "General", generalValues);

        // Escrever seção Encryption
        std::unordered_map<std::string, std::string> encryptionValues = {
            {"Type", m_config.encryptionType}
        };
        WriteSection(file, "Encryption", encryptionValues);

        // Escrever seção IPFiltering
        std::string allowedIPs;
        for (const auto& ip : m_config.allowedIPs) {
            if (!allowedIPs.empty()) {
                allowedIPs += ", ";
            }
            allowedIPs += ip;
        }
        std::unordered_map<std::string, std::string> ipFilteringValues = {
            {"AllowedIPs", allowedIPs}
        };
        WriteSection(file, "IPFiltering", ipFilteringValues);

        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool SecurityConfigLoader::ParseConfigFile()
{
    try {
        std::ifstream file(m_configPath);
        if (!file.is_open()) {
            return false;
        }

        // Parse General section
        auto generalSection = ParseSection("General");
        if (!ParseGeneralSection(generalSection)) {
            return false;
        }

        // Parse Encryption section
        auto encryptionSection = ParseSection("Encryption");
        if (!ParseEncryptionSection(encryptionSection)) {
            return false;
        }

        // Parse Authentication section
        auto authenticationSection = ParseSection("Authentication");
        if (!ParseAuthenticationSection(authenticationSection)) {
            return false;
        }

        // Parse ChannelSecurity section
        auto channelSecuritySection = ParseSection("ChannelSecurity");
        if (!ParseChannelSecuritySection(channelSecuritySection)) {
            return false;
        }

        // Parse UserSecurity section
        auto userSecuritySection = ParseSection("UserSecurity");
        if (!ParseUserSecuritySection(userSecuritySection)) {
            return false;
        }

        // Parse IPFiltering section
        auto ipFilteringSection = ParseSection("IPFiltering");
        if (!ParseIPFilteringSection(ipFilteringSection)) {
            return false;
        }

        // Parse Monitoring section
        auto monitoringSection = ParseSection("Monitoring");
        if (!ParseMonitoringSection(monitoringSection)) {
            return false;
        }

        // Parse Advanced section
        auto advancedSection = ParseSection("Advanced");
        if (!ParseAdvancedSection(advancedSection)) {
            return false;
        }

        // Parse Debug section
        auto debugSection = ParseSection("Debug");
        if (!ParseDebugSection(debugSection)) {
            return false;
        }

        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool SecurityConfigLoader::ParseGeneralSection(const std::unordered_map<std::string, std::string>& section)
{
    try {
        m_config.enableEncryption = section.at("EnableEncryption") == "true";
        m_config.enableAuthentication = section.at("EnableAuthentication") == "true";
        m_config.enableChannelEncryption = section.at("EnableChannelEncryption") == "true";
        m_config.enableUserAuthentication = section.at("EnableUserAuthentication") == "true";
        m_config.enableChannelAuthentication = section.at("EnableChannelAuthentication") == "true";
        m_config.enableIPFiltering = section.at("EnableIPFiltering") == "true";
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool SecurityConfigLoader::ParseEncryptionSection(const std::unordered_map<std::string, std::string>& section)
{
    try {
        m_config.encryptionType = section.at("Type");
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool SecurityConfigLoader::ParseAuthenticationSection(const std::unordered_map<std::string, std::string>& section)
{
    // Implementar parsing da seção de autenticação
    return true;
}

bool SecurityConfigLoader::ParseChannelSecuritySection(const std::unordered_map<std::string, std::string>& section)
{
    // Implementar parsing da seção de segurança de canais
    return true;
}

bool SecurityConfigLoader::ParseUserSecuritySection(const std::unordered_map<std::string, std::string>& section)
{
    // Implementar parsing da seção de segurança de usuários
    return true;
}

bool SecurityConfigLoader::ParseIPFilteringSection(const std::unordered_map<std::string, std::string>& section)
{
    try {
        std::string allowedIPs = section.at("AllowedIPs");
        std::istringstream iss(allowedIPs);
        std::string ip;
        m_config.allowedIPs.clear();
        
        while (std::getline(iss, ip, ',')) {
            // Remover espaços em branco
            ip.erase(std::remove_if(ip.begin(), ip.end(), ::isspace), ip.end());
            if (!ip.empty()) {
                m_config.allowedIPs.push_back(ip);
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool SecurityConfigLoader::ParseMonitoringSection(const std::unordered_map<std::string, std::string>& section)
{
    // Implementar parsing da seção de monitoramento
    return true;
}

bool SecurityConfigLoader::ParseAdvancedSection(const std::unordered_map<std::string, std::string>& section)
{
    // Implementar parsing da seção avançada
    return true;
}

bool SecurityConfigLoader::ParseDebugSection(const std::unordered_map<std::string, std::string>& section)
{
    // Implementar parsing da seção de debug
    return true;
}

std::unordered_map<std::string, std::string> SecurityConfigLoader::ParseSection(const std::string& sectionName)
{
    std::unordered_map<std::string, std::string> section;
    std::ifstream file(m_configPath);
    std::string line;
    bool inSection = false;

    while (std::getline(file, line)) {
        // Remover espaços em branco no início e fim
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }

        if (line[0] == '[' && line[line.length() - 1] == ']') {
            std::string currentSection = line.substr(1, line.length() - 2);
            inSection = (currentSection == sectionName);
            continue;
        }

        if (inSection) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);

                // Remover espaços em branco
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                section[key] = value;
            }
        }
    }

    return section;
}

bool SecurityConfigLoader::WriteSection(std::ofstream& file, const std::string& sectionName,
                                      const std::unordered_map<std::string, std::string>& values)
{
    file << "[" << sectionName << "]" << std::endl;
    
    for (const auto& pair : values) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    file << std::endl;
    return true;
}

} // namespace VoIP
} // namespace WYDBR 