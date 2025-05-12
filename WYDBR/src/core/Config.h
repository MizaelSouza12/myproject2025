2. Assets
Texturas de personagens - Falta implementação completa
Modelos 3D - Faltam modelos essenciais
Sons e músicas - Faltam arquivos de áudio
Implementações Faltantes de Sistemas Críticos
/**
 * @file Config.h
 * @brief Sistema de gerenciamento de configurações
 * @author WYDBR Team
 */

#ifndef WYDBR_CONFIG_H
#define WYDBR_CONFIG_H

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <variant>
#include <optional>
#include <algorithm>
#include <filesystem>
#include <mutex>

#include "Logger.h"
#include "Utils.h"

namespace wydbr {
namespace core {

/**
 * @brief Tipo de dado de configuração
 */
using ConfigValue = std::variant<
    std::string,
    int,
    double,
    bool,
    std::vector<std::string>,
    std::vector<int>,
    std::vector<double>,
    std::unordered_map<std::string, std::string>
>;

/**
 * @brief Formato do arquivo de configuração
 */
enum class ConfigFormat {
    INI,
    JSON,
    XML,
    YAML,
    AUTO // Determina o formato baseado na extensão
};

/**
 * @class ConfigManager
 * @brief Gerencia configurações do sistema
 */
class ConfigManager {
public:
    /**
     * @brief Obtém a instância singleton do ConfigManager
     * @return Referência para a instância
     */
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    /**
     * @brief Carrega configurações de um arquivo
     * @param filename Caminho do arquivo
     * @param format Formato do arquivo (AUTO para detectar por extensão)
     * @param section Seção a ser carregada (apenas para formatos que suportam seções)
     * @return true se sucesso, false em caso de erro
     */
    bool loadFromFile(const std::string& filename, 
                     ConfigFormat format = ConfigFormat::AUTO,
                     const std::string& section = "") {
        std::lock_guard<std::mutex> lock(m_mutex);

        ConfigFormat actualFormat = format;
        if (actualFormat == ConfigFormat::AUTO) {
            actualFormat = detectFormatFromExtension(filename);
        }

        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                LOG_ERROR("Não foi possível abrir o arquivo de configuração: " + filename);
                return false;
            }

            switch (actualFormat) {
                case ConfigFormat::INI:
                    return loadIniFile(file, section);
                case ConfigFormat::JSON:
                    return loadJsonFile(file);
                case ConfigFormat::XML:
                    return loadXmlFile(file);
                case ConfigFormat::YAML:
                    return loadYamlFile(file);
                default:
                    LOG_ERROR("Formato de configuração desconhecido ou não suportado");
                    return false;
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Erro ao carregar configurações: " + std::string(e.what()));
            return false;
        }
    }

    /**
     * @brief Salva configurações em um arquivo
     * @param filename Caminho do arquivo
     * @param format Formato do arquivo (AUTO para detectar por extensão)
     * @param section Seção a ser salva (apenas para formatos que suportam seções)
     * @return true se sucesso, false em caso de erro
     */
    bool saveToFile(const std::string& filename, 
                   ConfigFormat format = ConfigFormat::AUTO,
                   const std::string& section = "") {
        std::lock_guard<std::mutex> lock(m_mutex);

        ConfigFormat actualFormat = format;
        if (actualFormat == ConfigFormat::AUTO) {
            actualFormat = detectFormatFromExtension(filename);
        }

        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                LOG_ERROR("Não foi possível abrir o arquivo para salvar configuração: " + filename);
                return false;
            }

            switch (actualFormat) {
                case ConfigFormat::INI:
                    return saveIniFile(file, section);
                case ConfigFormat::JSON:
                    return saveJsonFile(file);
                case ConfigFormat::XML:
                    return saveXmlFile(file);
                case ConfigFormat::YAML:
                    return saveYamlFile(file);
                default:
                    LOG_ERROR("Formato de configuração desconhecido ou não suportado");
                    return false;
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Erro ao salvar configurações: " + std::string(e.what()));
            return false;
        }
    }

    /**
     * @brief Define um valor de configuração
     * @param key Chave da configuração
     * @param value Valor da configuração
     */
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_config[key] = value;
    }

    /**
     * @brief Define um vetor de valores de configuração
     * @param key Chave da configuração
     * @param values Valores da configuração
     */
    template<typename T>
    void setArray(const std::string& key, const std::vector<T>& values) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if constexpr (std::is_same_v<T, std::string>) {
            m_config[key] = values;
        } else if constexpr (std::is_same_v<T, int>) {
            m_config[key] = values;
        } else if constexpr (std::is_same_v<T, double>) {
            m_config[key] = values;
        } else {
            // Converte para strings se não for um tipo suportado diretamente
            std::vector<std::string> strValues;
            strValues.reserve(values.size());
            for (const auto& value : values) {
                strValues.push_back(utils::string::toString(value));
            }
            m_config[key] = strValues;
        }
    }

    /**
     * @brief Define um mapa de valores de configuração
     * @param key Chave da configuração
     * @param values Mapa de valores
     */
    void setMap(const std::string& key, const std::unordered_map<std::string, std::string>& values) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_config[key] = values;
    }

    /**
     * @brief Obtém um valor de configuração como string
     * @param key Chave da configuração
     * @param defaultValue Valor padrão se a chave não existir
     * @return Valor da configuração ou valor padrão
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_config.find(key);
        if (it == m_config.end()) {
            return defaultValue;
        }

        try {
            if (const auto value = std::get_if<std::string>(&it->second)) {
                return *value;
            }
            // Tenta converter outros tipos para string
            if (const auto value = std::get_if<int>(&it->second)) {
                return std::to_string(*value);
            }
            if (const auto value = std::get_if<double>(&it->second)) {
                return std::to_string(*value);
            }
            if (const auto value = std::get_if<bool>(&it->second)) {
                return *value ? "true" : "false";
            }
        } catch (const std::bad_variant_access&) {
            LOG_WARNING("Erro ao converter configuração para string: " + key);
            return defaultValue;
        }

        return defaultValue;
    }

    /**
     * @brief Obtém um valor de configuração como inteiro
     * @param key Chave da configuração
     * @param defaultValue Valor padrão se a chave não existir
     * @return Valor da configuração ou valor padrão
     */
    int getInt(const std::string& key, int defaultValue = 0) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_config.find(key);
        if (it == m_config.end()) {
            return defaultValue;
        }

        try {
            if (const auto value = std::get_if<int>(&it->second)) {
                return *value;
            }
            if (const auto value = std::get_if<double>(&it->second)) {
                return static_cast<int>(*value);
            }
            if (const auto value = std::get_if<std::string>(&it->second)) {
                return utils::string::fromString<int>(*value, defaultValue);
            }
            if (const auto value = std::get_if<bool>(&it->second)) {
                return *value ? 1 : 0;
            }
        } catch (const std::bad_variant_access&) {
            LOG_WARNING("Erro ao converter configuração para inteiro: " + key);
            return defaultValue;
        }

        return defaultValue;
    }

    /**
     * @brief Obtém um valor de configuração como double
     * @param key Chave da configuração
     * @param defaultValue Valor padrão se a chave não existir
     * @return Valor da configuração ou valor padrão
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_config.find(key);
        if (it == m_config.end()) {
            return defaultValue;
        }

        try {
            if (const auto value = std::get_if<double>(&it->second)) {
                return *value;
            }
            if (const auto value = std::get_if<int>(&it->second)) {
                return static_cast<double>(*value);
            }
            if (const auto value = std::get_if<std::string>(&it->second)) {
                return utils::string::fromString<double>(*value, defaultValue);
            }
            if (const auto value = std::get_if<bool>(&it->second)) {
                return *value ? 1.0 : 0.0;
            }
        } catch (const std::bad_variant_access&) {
            LOG_WARNING("Erro ao converter configuração para double: " + key);
            return defaultValue;
        }

        return defaultValue;
    }

    /**
     * @brief Obtém um valor de configuração como boolean
     * @param key Chave da configuração
     * @param defaultValue Valor padrão se a chave não existir
     * @return Valor da configuração ou valor padrão
     */
    bool getBool(const std::string& key, bool defaultValue = false) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_config.find(key);
        if (it == m_config.end()) {
            return defaultValue;
        }

        try {
            if (const auto value = std::get_if<bool>(&it->second)) {
                return *value;
            }
            if (const auto value = std::get_if<int>(&it->second)) {
                return *value != 0;
            }
            if (const auto value = std::get_if<double>(&it->second)) {
                return *value != 0.0;
            }
            if (const auto value = std::get_if<std::string>(&it->second)) {
                std::string valueStr = utils::string::toLower(*value);
                return valueStr == "true" || valueStr == "yes" || 
                       valueStr == "1" || valueStr == "on" || 
                       valueStr == "y" || valueStr == "t";
            }
        } catch (const std::bad_variant_access&) {
            LOG_WARNING("Erro ao converter configuração para boolean: " + key);
            return defaultValue;
        }

        return defaultValue;
    }

    /**
     * @brief Obtém um array de strings
     * @param key Chave da configuração
     * @return Vector de strings ou vetor vazio se não encontrado
     */
    std::vector<std::string> getStringArray(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_config.find(key);
        if (it == m_config.end()) {
            return {};
        }

        try {
            if (const auto value = std::get_if<std::vector<std::string>>(&it->second)) {
                return *value;
            }
            if (const auto value = std::get_if<std::string>(&it->second)) {
                // Tenta interpretar a string como um array delimitado por vírgulas
                return utils::string::split(*value, ',');
            }
        } catch (const std::bad_variant_access&) {
            LOG_WARNING("Erro ao obter array de strings: " + key);
            return {};
        }

        return {};
    }

    /**
     * @brief Obtém um array de inteiros
     * @param key Chave da configuração
     * @return Vector de inteiros ou vetor vazio se não encontrado
     */
    std::vector<int> getIntArray(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_config.find(key);
        if (it == m_config.end()) {
            return {};
        }

        try {
            if (const auto value = std::get_if<std::vector<int>>(&it->second)) {
                return *value;
            }
            if (const auto value = std::get_if<std::string>(&it->second)) {
                // Tenta interpretar a string como um array delimitado por vírgulas
                std::vector<int> result;
                auto strParts = utils::string::split(*value, ',');
                for (const auto& part : strParts) {
                    result.push_back(utils::string::fromString<int>(utils::string::trim(part), 0));
                }
                return result;
            }
            if (const auto value = std::get_if<std::vector<std::string>>(&it->second)) {
                std::vector<int> result;
                for (const auto& part : *value) {
                    result.push_back(utils::string::fromString<int>(part, 0));
                }
                return result;
            }
        } catch (const std::bad_variant_access&) {
            LOG_WARNING("Erro ao obter array de inteiros: " + key);
            return {};
        }

        return {};
    }

    /**
     * @brief Obtém um array de doubles
     * @param key Chave da configuração
     * @return Vector de doubles ou vetor vazio se não encontrado
     */
    std::vector<double> getDoubleArray(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_config.find(key);
        if (it == m_config.end()) {
            return {};
        }

        try {
            if (const auto value = std::get_if<std::vector<double>>(&it->second)) {
                return *value;
            }
            if (const auto value = std::get_if<std::string>(&it->second)) {
                // Tenta interpretar a string como um array delimitado por vírgulas
                std::vector<double> result;
                auto strParts = utils::string::split(*value, ',');
                for (const auto& part : strParts) {
                    result.push_back(utils::string::fromString<double>(utils::string::trim(part), 0.0));
                }
                return result;
            }
            if (const auto value = std::get_if<std::vector<std::string>>(&it->second)) {
                std::vector<double> result;
                for (const auto& part : *value) {
                    result.push_back(utils::string::fromString<double>(part, 0.0));
                }
                return result;
            }
        } catch (const std::bad_variant_access&) {
            LOG_WARNING("Erro ao obter array de doubles: " + key);
            return {};
        }

        return {};
    }

    /**
     * @brief Obtém um mapa de strings
     * @param key Chave da configuração
     * @return Mapa de string para string ou mapa vazio se não encontrado
     */
    std::unordered_map<std::string, std::string> getMap(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_config.find(key);
        if (it == m_config.end()) {
            return {};
        }

        try {
            if (const auto value = std::get_if<std::unordered_map<std::string, std::string>>(&it->second)) {
                return *value;
            }
            // Outros tipos não são facilmente convertíveis para map
        } catch (const std::bad_variant_access&) {
            LOG_WARNING("Erro ao obter mapa: " + key);
            return {};
        }

        return {};
    }

    /**
     * @brief Verifica se uma chave existe
     * @param key Chave a verificar
     * @return true se a chave existir, false caso contrário
     */
    bool hasKey(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_config.find(key) != m_config.end();
    }

    /**
     * @brief Remove uma configuração
     * @param key Chave a remover
     */
    void removeKey(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_config.erase(key);
    }

    /**
     * @brief Limpa todas as configurações
     */
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_config.clear();
    }

    /**
     * @brief Obtém todas as chaves de configuração
     * @return Vector com todas as chaves
     */
    std::vector<std::string> getAllKeys() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> keys;
        keys.reserve(m_config.size());
        
        for (const auto& [key, _] : m_config) {
            keys.push_back(key);
        }
        
        return keys;
    }

private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // Detecta o formato baseado na extensão do arquivo
    ConfigFormat detectFormatFromExtension(const std::string& filename) const {
        std::filesystem::path path(filename);
        std::string extension = path.extension().string();
        
        if (extension == ".ini" || extension == ".cfg" || extension == ".conf") {
            return ConfigFormat::INI;
        } else if (extension == ".json") {
            return ConfigFormat::JSON;
        } else if (extension == ".xml") {
            return ConfigFormat::XML;
        } else if (extension == ".yaml" || extension == ".yml") {
            return ConfigFormat::YAML;
        }
        
        // Padrão para INI se não reconhecer a extensão
        return ConfigFormat::INI;
    }

    // Funções para cada formato de arquivo
    bool loadIniFile(std::ifstream& file, const std::string& section) {
        std::string currentSection;
        std::string line;
        
        while (std::getline(file, line)) {
            // Remove espaços em branco e comentários
            line = utils::string::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // Verifica se é um cabeçalho de seção
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                currentSection = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Se estamos em uma seção específica e não é a que queremos, ignore
            if (!section.empty() && !currentSection.empty() && currentSection != section) {
                continue;
            }
            
            // Processa pares chave=valor
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = utils::string::trim(line.substr(0, equalsPos));
                std::string value = utils::string::trim(line.substr(equalsPos + 1));
                
                // Adiciona prefixo de seção se estiver em uma seção e não foi solicitado filtro
                if (!currentSection.empty() && section.empty()) {
                    key = currentSection + "." + key;
                }
                
                // Tenta converter para tipos primitivos
                if (value == "true" || value == "yes" || value == "y" || value == "on") {
                    m_config[key] = true;
                } else if (value == "false" || value == "no" || value == "n" || value == "off") {
                    m_config[key] = false;
                } else if (value.find('.') != std::string::npos) {
                    // Tenta converter para double
                    try {
                        m_config[key] = std::stod(value);
                    } catch (...) {
                        m_config[key] = value;
                    }
                } else {
                    // Tenta converter para int
                    try {
                        m_config[key] = std::stoi(value);
                    } catch (...) {
                        m_config[key] = value;
                    }
                }
            }
        }
        
        return true;
    }

    bool saveIniFile(std::ofstream& file, const std::string& section) const {
        // Agrupa as chaves por seção
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections;
        
        for (const auto& [key, value] : m_config) {
            size_t dotPos = key.find('.');
            std::string sectionName;
            std::string sectionKey;
            
            if (dotPos != std::string::npos) {
                sectionName = key.substr(0, dotPos);
                sectionKey = key.substr(dotPos + 1);
            } else {
                // Usa "global" para chaves sem seção
                sectionName = "global";
                sectionKey = key;
            }
            
            // Se uma seção específica foi solicitada, filtra
            if (!section.empty() && sectionName != section) {
                continue;
            }
            
            // Converte o valor para string
            std::string valueStr;
            std::visit([&valueStr](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    valueStr = arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    valueStr = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    valueStr = std::to_string(arg);
                    // Remove zeros à direita
                    valueStr.erase(valueStr.find_last_not_of('0') + 1, std::string::npos);
                    if (valueStr.back() == '.') {
                        valueStr.push_back('0');
                    }
                } else if constexpr (std::is_same_v<T, bool>) {
                    valueStr = arg ? "true" : "false";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    for (const auto& item : arg) {
                        if (!valueStr.empty()) valueStr += ",";
                        valueStr += item;
                    }
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    for (const auto& item : arg) {
                        if (!valueStr.empty()) valueStr += ",";
                        valueStr += std::to_string(item);
                    }
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    for (const auto& item : arg) {
                        if (!valueStr.empty()) valueStr += ",";
                        valueStr += std::to_string(item);
                    }
                } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, std::string>>) {
                    // Mapas são mais complexos, usamos JSON-like para ini
                    valueStr = "{";
                    bool first = true;
                    for (const auto& [k, v] : arg) {
                        if (!first) valueStr += ",";
                        first = false;
                        valueStr += k + ":" + v;
                    }
                    valueStr += "}";
                }
            }, value);
            
            sections[sectionName][sectionKey] = valueStr;
        }
        
        // Escreve a seção global primeiro (sem cabeçalho)
        if (sections.count("global") > 0 && (section.empty() || section == "global")) {
            for (const auto& [key, value] : sections["global"]) {
                file << key << " = " << value << std::endl;
            }
            file << std::endl;
            sections.erase("global");
        }
        
        // Escreve as demais seções
        for (const auto& [sectionName, sectionData] : sections) {
            file << "[" << sectionName << "]" << std::endl;
            for (const auto& [key, value] : sectionData) {
                file << key << " = " << value << std::endl;
            }
            file << std::endl;
        }
        
        return true;
    }

    // Implementações simplificadas para outros formatos
    // Em um sistema real, você usaria bibliotecas especializadas para JSON, XML e YAML
    
    bool loadJsonFile(std::ifstream& file) {
        // Implementação simplificada - em um sistema real, use uma biblioteca como nlohmann::json
        LOG_WARNING("Carregamento de JSON não está completamente implementado");
        return false;
    }
    
    bool saveJsonFile(std::ofstream& file) const {
        // Implementação simplificada
        LOG_WARNING("Salvamento em JSON não está completamente implementado");
        return false;
    }
    
    bool loadXmlFile(std::ifstream& file) {
        // Implementação simplificada - em um sistema real, use uma biblioteca como tinyxml2
        LOG_WARNING("Carregamento de XML não está completamente implementado");
        return false;
    }
    
    bool saveXmlFile(std::ofstream& file) const {
        // Implementação simplificada
        LOG_WARNING("Salvamento em XML não está completamente implementado");
        return false;
    }
    
    bool loadYamlFile(std::ifstream& file) {
        // Implementação simplificada - em um sistema real, use uma biblioteca como yaml-cpp
        LOG_WARNING("Carregamento de YAML não está completamente implementado");
        return false;
    }
    
    bool saveYamlFile(std::ofstream& file) const {
        // Implementação simplificada
        LOG_WARNING("Salvamento em YAML não está completamente implementado");
        return false;
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_map<std::string, ConfigValue> m_config;
};

} // namespace core
} // namespace wydbr

#endif // WYDBR_CONFIG_H 