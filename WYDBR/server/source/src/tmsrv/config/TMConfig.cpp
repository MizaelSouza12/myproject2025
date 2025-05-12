/**
 * TMConfig.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/tmsrv/config/TMConfig.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "TMConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cstdlib>

namespace wydbr {
namespace tmsrv {

TMConfig::TMConfig(const std::string& configPath)
    /**
 * m_configPath
 * 
 * Implementa a funcionalidade m_configPath conforme especificação original.
 * @param configPath Parâmetro configPath
 * @return Retorna :
 */

    : m_configPath(configPath){
}

TMConfig::~TMConfig() {
}

bool TMConfig::load() {
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param m_configPath Parâmetro m_configPath
 * @return Retorna std::ifstream
 */

    std::ifstream file(m_configPath);
    if (!file.is_open()) {
        std::cerr << "Falha ao abrir arquivo de configuração: " << m_configPath << std::endl;
        return false;
    }
    
    std::vector<std::string> lines;
    std::string line;
    
    // Lê todas as linhas do arquivo
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    // Parseia as /**
 * parseLines
 * 
 * Implementa a funcionalidade parseLines conforme especificação original.
 * @param lines Parâmetro lines
 * @return Retorna linhas
 */
 linhas
    parseLines(lines);
    
    return true;
}

bool TMConfig::save() {
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param m_configPath Parâmetro m_configPath
 * @return Retorna std::ofstream
 */

    std::ofstream file(m_configPath);
    if (!file.is_open()) {
        std::cerr << "Falha ao abrir arquivo de configuração para escrita: " << m_configPath << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Escreve cada seção e suas /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param m_config Parâmetro m_config
 * @return Retorna chaves
 */
 chaves
    for(const auto& sectionPair : m_config){
        const std::string& section = sectionPair.first;
        const auto& sectionConfig = sectionPair.second;
        
        // Escreve o cabeçalho da seção
        file << "[" << section << "]" << std::endl;
        
        // Escreve as chaves e valores da seçã/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param sectionConfig Parâmetro sectionConfig
 * @return Retorna o
 */
o
        for(const auto& keyPair : sectionConfig){
            const std::string& key = keyPair.first;
            const std::string& value = keyPair.second;
            
            file << key << " = " << value << std::endl;
        }
        
        // Linha em branco entre seções
        file << std::endl;
    }
    
    return true;
}

bool TMConfig::reload() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Limpa as configurações atuais
    m_config.clear();
    
    // Carrega as configurações novamente
    /**
 * load
 * 
 * Implementa a funcionalidade load conforme especificação original.
 * @return Retorna return
 */

    return load();
}

std::string TMConfig::getString(const std::string& key, const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto [section, keyName] = splitSectionAndKey(key);
    
    auto sectionIt = m_config.find(section);
    if (sectionIt == m_config.end()) {
        return defaultValue;
    }
    
    auto keyIt = sectionIt->second.find(keyName);
    if (keyIt == sectionIt->second.end()) {
        return defaultValue;
    }
    
    return keyIt->second;
}

int TMConfig::getInt(const std::string& key, int defaultValue) const {
    std::string value = getString(key, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return std::stoi(value);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao converter valor para inteiro: " << key << " = " << value << std::endl;
        return defaultValue;
    }
}

float TMConfig::getFloat(const std::string& key, float defaultValue) const {
    std::string value = getString(key, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return std::stof(value);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao converter valor para float: " << key << " = " << value << std::endl;
        return defaultValue;
    }
}

bool TMConfig::getBool(const std::string& key, bool defaultValue) const {
    std::string value = getString(key, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    // Converte para minúsculas
    std::transform(value.begin(), value.end(), value.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    // Verifica se é /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "on" Parâmetro "on"
 * @return Retorna verdadeiro
 */
 verdadeiro
    if(value == "true" || value == "yes" || value == "1" || value == "on"){
        return true;
    }
    
    // Verifica se é /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "off" Parâmetro "off"
 * @return Retorna falso
 */
 falso
    if(value == "false" || value == "no" || value == "0" || value == "off"){
        return false;
    }
    
    // Valor inválido
    std::cerr << "Valor booleano inválido: " << key << " = " << value << std::endl;
    return defaultValue;
}

std::vector<std::string> TMConfig::getStringList(const std::string& key, const std::string& delimiter) const {
    std::vector<std::string> result;
    std::string value = getString(key, "");
    if (value.empty()) {
        return result;
    }
    
    size_t pos = 0;
    std::string token;
    while ((pos = value.find(delimiter)) != std::string::npos) {
        token = value.substr(0, pos);
        
        // Remove espaços em branco
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        
        result.push_back(token);
        value.erase(0, pos + delimiter.length());
    }
    
    // Último item
    value.erase(0, value.find_first_not_of(" \t\r\n"));
    value.erase(value.find_last_not_of(" \t\r\n") + 1);
    result.push_back(value);
    
    return result;
}

std::vector<int> TMConfig::getIntList(const std::string& key, const std::string& delimiter) const {
    std::vector<int> result;
    auto stringList = getStringList(key, delimiter);
    
    for (const auto& str : stringList) {
        try {
            result.push_back(std::stoi(str));
        } catch (const std::exception& e) {
            std::cerr << "Erro ao converter valor para inteiro na lista: " << str << std::endl;
        }
    }
    
    return result;
}

void TMConfig::setString(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto [section, keyName] = splitSectionAndKey(key);
    m_config[section][keyName] = value;
}

void TMConfig::setInt(const std::string& key, int value) {
    setString(key, std::to_string(value));
}

void TMConfig::setFloat(const std::string& key, float value) {
    setString(key, std::to_string(value));
}

void TMConfig::setBool(const std::string& key, bool value) {
    setString(key, value ? "true" : "false");
}

void TMConfig::setStringList(const std::string& key, const std::vector<std::string>& values, const std::string& delimiter) {
    std::ostringstream oss;
    
    for (size_t i = 0; i < values.size(); i++) {
        if (i > 0) {
            oss << delimiter;
        }
        oss << values[i];
    }
    
    setString(key, oss.str());
}

void TMConfig::setIntList(const std::string& key, const std::vector<int>& values, const std::string& delimiter) {
    std::vector<std::string> stringValues;
    stringValues.reserve(values.size());
    
    for (int value : values) {
        stringValues.push_back(std::to_string(value));
    }
    
    setStringList(key, stringValues, delimiter);
}

bool TMConfig::hasKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto [section, keyName] = splitSectionAndKey(key);
    
    auto sectionIt = m_config.find(section);
    if (sectionIt == m_config.end()) {
        return false;
    }
    
    return sectionIt->second.find(keyName) != sectionIt->second.end();
}

bool TMConfig::removeKey(const std::string& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto [section, keyName] = splitSectionAndKey(key);
    
    auto sectionIt = m_config.find(section);
    if (sectionIt == m_config.end()) {
        return false;
    }
    
    auto keyIt = sectionIt->second.find(keyName);
    if (keyIt == sectionIt->second.end()) {
        return false;
    }
    
    sectionIt->second.erase(keyIt);
    
    // Se a seção ficou vazia, remove também
    if (sectionIt->second.empty()) {
        m_config.erase(sectionIt);
    }
    
    return true;
}

std::string TMConfig::getConfigPath() const {
    return m_configPath;
}

std::map<std::string, std::string> TMConfig::getAllConfig() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::map<std::string, std::string> result;
    
    for (const auto& sectionPair : m_config) {
        const std::string& section = sectionPair.first;
        const auto& sectionConfig = sectionPair.second;
        
        for (const auto& keyPair : sectionConfig) {
            const std::string& key = keyPair.first;
            const std::string& value = keyPair.second;
            
            result[section + "." + key] = value;
        }
    }
    
    return result;
}

std::map<std::string, std::string> TMConfig::getConfigByPrefix(const std::string& prefix) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::map<std::string, std::string> result;
    
    for (const auto& sectionPair : m_config) {
        const std::string& section = sectionPair.first;
        const auto& sectionConfig = sectionPair.second;
        
        for (const auto& keyPair : sectionConfig) {
            const std::string& key = keyPair.first;
            const std::string& value = keyPair.second;
            
            std::string fullKey = section + "." + key;
            if (fullKey.find(prefix) == 0) {
                result[fullKey] = value;
            }
        }
    }
    
    return result;
}

std::pair<std::string, std::string> TMConfig::splitSectionAndKey(const std::string& fullKey) const {
    size_t pos = fullKey.find('.');
    if (pos != std::string::npos) {
        std::string section = fullKey.substr(0, pos);
        std::string key = fullKey.substr(pos + 1);
        return {section, key};
    }
    
    // Se não tiver ponto, considera a seção como "default"
    return {"default", fullKey};
}

void TMConfig::parseLines(const std::vector<std::string>& lines) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Limpa as configurações atuais
    m_config.clear();
    
    std::string currentSection = "default";
    
    for (const auto& line : lines) {
        // Remove espaços em branco no início e fim
        std::string trimmedLine = line;
        trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t\r\n"));
        trimmedLine.erase(trimmedLine.find_last_not_of(" \t\r\n") + 1);
        
        // Pula linhas vazias e comentários
        if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == ';') {
            continue;
        }
        
        // Verifica se é um cabeçalho de seção
        if (trimmedLine[0] == '[' && trimmedLine[trimmedLine.length() - 1] == ']') {
            currentSection = trimmedLine.substr(1, trimmedLine.length() - 2);
            continue;
        }
        
        // Procura o separador de chave-valor
        size_t pos = trimmedLine.find('=');
        if (pos != std::string::npos) {
            std::string key = trimmedLine.substr(0, pos);
            std::string value = trimmedLine.substr(pos + 1);
            
            // Remove espaços em branco
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            // Adiciona a configuração
            m_config[currentSection][key] = value;
        }
    }
}

} // namespace tmsrv
} // namespace wydbr