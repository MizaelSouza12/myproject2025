/**
 * TMConfig.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/tmsrv/config/TMConfig.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYDBR_TMCONFIG_H
#define WYDBR_TMCONFIG_H

#include <string>
#include <map>
#include <vector>
#include <mutex>

namespace wydbr {
namespace tmsrv {

/**
 * @class TMConfig
 * @brief Classe para gerenciar as configurações do servidor TMSrv
 * 
 * Carrega, armazena e fornece acesso às configurações do servidor
 * a partir de um arquivo de configuração.
 */
/**
 * Classe TMConfig
 * 
 * Esta classe implementa a funcionalidade TMConfig conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TMConfig {
public:
    /**
     * @brief Constrói um novo gerenciador de configurações
     * @param configPath Caminho para o arquivo de configuração
     */
    explicit TMConfig(const std::string& configPath);
    
    /**
     * @brief Destrutor
     */
    ~TMConfig();
    
    /**
     * @brief Carrega as configurações do arquivo
     * @return true se as configurações foram carregadas com sucesso
     */
    bool load();
    
    /**
     * @brief Salva as configurações no arquivo
     * @return true se as configurações foram salvas com sucesso
     */
    bool save();
    
    /**
     * @brief Recarrega as configurações do arquivo
     * @return true se as configurações foram recarregadas com sucesso
     */
    bool reload();
    
    /**
     * @brief Obtém uma configuração de string
     * @param key Chave da configuração
     * @param defaultValue Valor padrão caso a configuração não exista
     * @return Valor da configuração
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * @brief Obtém uma configuração de inteiro
     * @param key Chave da configuração
     * @param defaultValue Valor padrão caso a configuração não exista
     * @return Valor da configuração
     */
    int getInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * @brief Obtém uma configuração de float
     * @param key Chave da configuração
     * @param defaultValue Valor padrão caso a configuração não exista
     * @return Valor da configuração
     */
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    
    /**
     * @brief Obtém uma configuração de boolean
     * @param key Chave da configuração
     * @param defaultValue Valor padrão caso a configuração não exista
     * @return Valor da configuração
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    /**
     * @brief Obtém uma lista de strings
     * @param key Chave da configuração
     * @param delimiter Delimitador para separar os valores
     * @return Lista de strings
     */
    std::vector<std::string> getStringList(const std::string& key, const std::string& delimiter = ",") const;
    
    /**
     * @brief Obtém uma lista de inteiros
     * @param key Chave da configuração
     * @param delimiter Delimitador para separar os valores
     * @return Lista de inteiros
     */
    std::vector<int> getIntList(const std::string& key, const std::string& delimiter = ",") const;
    
    /**
     * @brief Define uma configuração de string
     * @param key Chave da configuração
     * @param value Valor da configuração
     */
    void setString(const std::string& key, const std::string& value);
    
    /**
     * @brief Define uma configuração de inteiro
     * @param key Chave da configuração
     * @param value Valor da configuração
     */
    void setInt(const std::string& key, int value);
    
    /**
     * @brief Define uma configuração de float
     * @param key Chave da configuração
     * @param value Valor da configuração
     */
    void setFloat(const std::string& key, float value);
    
    /**
     * @brief Define uma configuração de boolean
     * @param key Chave da configuração
     * @param value Valor da configuração
     */
    void setBool(const std::string& key, bool value);
    
    /**
     * @brief Define uma lista de strings
     * @param key Chave da configuração
     * @param values Lista de valores
     * @param delimiter Delimitador para separar os valores
     */
    void setStringList(const std::string& key, const std::vector<std::string>& values, const std::string& delimiter = ",");
    
    /**
     * @brief Define uma lista de inteiros
     * @param key Chave da configuração
     * @param values Lista de valores
     * @param delimiter Delimitador para separar os valores
     */
    void setIntList(const std::string& key, const std::vector<int>& values, const std::string& delimiter = ",");
    
    /**
     * @brief Verifica se uma configuração existe
     * @param key Chave da configuração
     * @return true se a configuração existir
     */
    bool hasKey(const std::string& key) const;
    
    /**
     * @brief Remove uma configuração
     * @param key Chave da configuração
     * @return true se a configuração foi removida
     */
    bool removeKey(const std::string& key);
    
    /**
     * @brief Obtém o caminho do arquivo de configuração
     * @return Caminho do arquivo
     */
    std::string getConfigPath() const;
    
    /**
     * @brief Obtém todas as configurações
     * @return Mapa com todas as configurações
     */
    std::map<std::string, std::string> getAllConfig() const;
    
    /**
     * @brief Obtém configurações com um prefixo específico
     * @param prefix Prefixo das chaves
     * @return Mapa com as configurações que têm o prefixo
     */
    std::map<std::string, std::string> getConfigByPrefix(const std::string& prefix) const;

private:
    /**
     * @brief Separa seção e chave
     * @param fullKey Chave completa no formato "seção.chave"
     * @return Par com seção e chave
     */
    std::pair<std::string, std::string> splitSectionAndKey(const std::string& fullKey) const;
    
    /**
     * @brief Parseia linhas de configuração
     * @param lines Linhas do arquivo
     */
    void parseLines(const std::vector<std::string>& lines);

private:
    std::string m_configPath;                              // Caminho do arquivo de configuração
    std::map<std::string, std::map<std::string, std::string>> m_config; // Configurações por seção e chave
    mutable std::mutex m_mutex;                            // Mutex para acesso thread-safe
};

} // namespace tmsrv
} // namespace wydbr

#endif // WYDBR_TMCONFIG_H