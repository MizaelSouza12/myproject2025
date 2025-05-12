/**
 * @file UserSettingsDB.h
 * @brief Definição da tabela de configurações do usuário
 */

#ifndef WYD_USER_SETTINGS_DB_H
#define WYD_USER_SETTINGS_DB_H

#include <string>
#include <vector>
#include <functional>

namespace wydbr {
namespace database {

/**
 * @class UserSettingsDB
 * @brief Gerencia a persistência das configurações do usuário
 */
class UserSettingsDB {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência à instância única
     */
    static UserSettingsDB& getInstance();
    
    /**
     * @brief Inicializa a conexão com o banco de dados
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool initialize();
    
    /**
     * @struct UserSettings
     * @brief Estrutura que armazena todas as configurações do usuário
     */
    struct UserSettings {
        // Configurações de gráficos
        int graphicsQuality;
        int viewDistance;
        int effectsQuality;
        
        // Configurações de VOIP
        int pushToTalkKey;
        bool useMouse;
        int mouseButton;
        bool voiceActivation;
        int inputVolume;
        int outputVolume;
        
        // Configurações de compartilhamento
        std::string sharingPassword;
        bool blockItemMovement;
        bool blockPasswordChange;
        bool blockCharDeletion;
        
        // Configurações de interface
        bool showPerformanceIndicators;
        int indicatorsPosition;
        
        // Teclas de atalho personalizadas
        std::vector<std::pair<std::string, int>> customHotkeys;
        
        // Amigos e bloqueados
        std::vector<std::string> friendsList;
        std::vector<std::string> blockedUsers;
    };
    
    /**
     * @brief Carrega as configurações de um usuário
     * @param userId ID do usuário
     * @param settings Estrutura onde as configurações serão armazenadas
     * @return true se carregado com sucesso, false caso contrário
     */
    bool loadUserSettings(int userId, UserSettings& settings);
    
    /**
     * @brief Salva as configurações de um usuário
     * @param userId ID do usuário
     * @param settings Estrutura com as configurações a serem salvas
     * @return true se salvo com sucesso, false caso contrário
     */
    bool saveUserSettings(int userId, const UserSettings& settings);
    
    /**
     * @brief Adiciona um amigo à lista de um usuário
     * @param userId ID do usuário
     * @param friendName Nome do amigo a adicionar
     * @return true se adicionado com sucesso, false caso contrário
     */
    bool addFriend(int userId, const std::string& friendName);
    
    /**
     * @brief Remove um amigo da lista de um usuário
     * @param userId ID do usuário
     * @param friendName Nome do amigo a remover
     * @return true se removido com sucesso, false caso contrário
     */
    bool removeFriend(int userId, const std::string& friendName);
    
    /**
     * @brief Bloqueia um usuário
     * @param userId ID do usuário
     * @param blockedName Nome do usuário a bloquear
     * @return true se bloqueado com sucesso, false caso contrário
     */
    bool blockUser(int userId, const std::string& blockedName);
    
    /**
     * @brief Desbloqueia um usuário
     * @param userId ID do usuário
     * @param blockedName Nome do usuário a desbloquear
     * @return true se desbloqueado com sucesso, false caso contrário
     */
    bool unblockUser(int userId, const std::string& blockedName);

private:
    UserSettingsDB();
    ~UserSettingsDB();
    
    // Previne cópia e atribuição
    UserSettingsDB(const UserSettingsDB&) = delete;
    UserSettingsDB& operator=(const UserSettingsDB&) = delete;
    
    // Métodos privados
    bool createTablesIfNotExist();
    void logError(const std::string& message);
    
    // Estado interno
    bool m_initialized;
    std::string m_connectionString;
};

}} // namespace wydbr::database

#endif // WYD_USER_SETTINGS_DB_H
