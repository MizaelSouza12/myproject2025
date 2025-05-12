#ifndef WYDBR_VOIP_GROUP_MANAGER_H
#define WYDBR_VOIP_GROUP_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <functional>
#include <memory>

namespace wydbr {
namespace communication {

// Enumeração para tipo de grupo
enum class VoIPGroupType {
    PARTY,          // Grupo de jogadores em um grupo de jogo
    GUILD,          // Grupo de guild
    RAID,           // Grupo de raid
    CUSTOM,         // Grupo personalizado
    AREA,           // Grupo baseado em área geográfica
    EVENT           // Grupo temporário para evento
};

// Enumeração para permissões de grupo
enum class VoIPGroupPermission {
    TALK,           // Permissão para falar no grupo
    LISTEN,         // Permissão para ouvir o grupo
    INVITE,         // Permissão para convidar outros jogadores
    REMOVE,         // Permissão para remover jogadores
    MANAGE,         // Permissão para gerenciar configurações do grupo
    ADMINISTRATE    // Permissão para administração completa do grupo
};

// Callback para evento de alteração de grupo
using GroupChangeCallback = std::function<void(const std::string& groupId, 
                                             const std::string& userId,
                                             bool joined)>;

// Callback para evento de alteração de permissão
using PermissionChangeCallback = std::function<void(const std::string& groupId, 
                                                  const std::string& userId,
                                                  VoIPGroupPermission permission,
                                                  bool granted)>;

/**
 * @class VoIPGroupManager
 * @brief Gerenciador de grupos para o sistema VOIP
 * 
 * Gerencia grupos de voz para diferentes contextos (party, guild, área, etc.),
 * permitindo comunicação separada e configuração de permissões.
 */
class VoIPGroupManager {
public:
    VoIPGroupManager();
    ~VoIPGroupManager();
    
    /**
     * @brief Inicializa o gerenciador de grupos
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Desliga o gerenciador e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Cria um novo grupo
     * @param groupName Nome do grupo
     * @param groupType Tipo do grupo
     * @param persistent Se o grupo deve persistir após o criador sair
     * @return ID do grupo criado ou string vazia se falhar
     */
    std::string createGroup(const std::string& groupName, 
                           VoIPGroupType groupType = VoIPGroupType::CUSTOM,
                           bool persistent = false);
    
    /**
     * @brief Remove um grupo
     * @param groupId ID do grupo
     * @return true se removido com sucesso
     */
    bool removeGroup(const std::string& groupId);
    
    /**
     * @brief Obtém informações sobre um grupo
     * @param groupId ID do grupo
     * @param outName Nome do grupo (saída)
     * @param outType Tipo do grupo (saída)
     * @param outPersistent Se é persistente (saída)
     * @param outMemberCount Número de membros (saída)
     * @return true se o grupo existe
     */
    bool getGroupInfo(const std::string& groupId, 
                     std::string& outName,
                     VoIPGroupType& outType,
                     bool& outPersistent,
                     int& outMemberCount) const;
    
    /**
     * @brief Entra em um grupo
     * @param groupId ID do grupo
     * @param userId ID do usuário
     * @return true se entrou com sucesso
     */
    bool joinGroup(const std::string& groupId, const std::string& userId);
    
    /**
     * @brief Sai de um grupo
     * @param groupId ID do grupo
     * @param userId ID do usuário
     * @return true se saiu com sucesso
     */
    bool leaveGroup(const std::string& groupId, const std::string& userId);
    
    /**
     * @brief Convida um usuário para um grupo
     * @param groupId ID do grupo
     * @param targetUserId ID do usuário convidado
     * @param inviterUserId ID do usuário que convida
     * @return true se o convite foi enviado com sucesso
     */
    bool inviteToGroup(const std::string& groupId, 
                      const std::string& targetUserId,
                      const std::string& inviterUserId);
    
    /**
     * @brief Obtém a lista de membros de um grupo
     * @param groupId ID do grupo
     * @return Lista de IDs de usuários no grupo
     */
    std::vector<std::string> getGroupMembers(const std::string& groupId) const;
    
    /**
     * @brief Obtém a lista de grupos que um usuário participa
     * @param userId ID do usuário
     * @return Lista de IDs de grupos
     */
    std::vector<std::string> getUserGroups(const std::string& userId) const;
    
    /**
     * @brief Verifica se um usuário pertence a um grupo
     * @param groupId ID do grupo
     * @param userId ID do usuário
     * @return true se o usuário está no grupo
     */
    bool isUserInGroup(const std::string& groupId, const std::string& userId) const;
    
    /**
     * @brief Define o nome de um grupo
     * @param groupId ID do grupo
     * @param newName Novo nome
     * @return true se o nome foi alterado com sucesso
     */
    bool setGroupName(const std::string& groupId, const std::string& newName);
    
    /**
     * @brief Obtém o nome de um grupo
     * @param groupId ID do grupo
     * @return Nome do grupo ou string vazia se não existir
     */
    std::string getGroupName(const std::string& groupId) const;
    
    /**
     * @brief Define o tipo de um grupo
     * @param groupId ID do grupo
     * @param groupType Novo tipo
     * @return true se o tipo foi alterado com sucesso
     */
    bool setGroupType(const std::string& groupId, VoIPGroupType groupType);
    
    /**
     * @brief Obtém o tipo de um grupo
     * @param groupId ID do grupo
     * @return Tipo do grupo
     */
    VoIPGroupType getGroupType(const std::string& groupId) const;
    
    /**
     * @brief Concede uma permissão a um usuário em um grupo
     * @param groupId ID do grupo
     * @param userId ID do usuário
     * @param permission Permissão a conceder
     * @return true se a permissão foi concedida com sucesso
     */
    bool grantPermission(const std::string& groupId, 
                        const std::string& userId,
                        VoIPGroupPermission permission);
    
    /**
     * @brief Revoga uma permissão de um usuário em um grupo
     * @param groupId ID do grupo
     * @param userId ID do usuário
     * @param permission Permissão a revogar
     * @return true se a permissão foi revogada com sucesso
     */
    bool revokePermission(const std::string& groupId, 
                         const std::string& userId,
                         VoIPGroupPermission permission);
    
    /**
     * @brief Verifica se um usuário tem uma permissão em um grupo
     * @param groupId ID do grupo
     * @param userId ID do usuário
     * @param permission Permissão a verificar
     * @return true se o usuário tem a permissão
     */
    bool hasPermission(const std::string& groupId, 
                      const std::string& userId,
                      VoIPGroupPermission permission) const;
    
    /**
     * @brief Gerencia permissões para um novo membro
     * @param groupId ID do grupo
     * @param permissions Lista de permissões iniciais para novos membros
     * @return true se as permissões foram definidas com sucesso
     */
    bool setDefaultPermissions(const std::string& groupId, 
                              const std::vector<VoIPGroupPermission>& permissions);
    
    /**
     * @brief Obtém as permissões padrão para novos membros
     * @param groupId ID do grupo
     * @return Lista de permissões padrão
     */
    std::vector<VoIPGroupPermission> getDefaultPermissions(const std::string& groupId) const;
    
    /**
     * @brief Registra um callback para evento de alteração de grupo
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerGroupChangeCallback(GroupChangeCallback callback);
    
    /**
     * @brief Remove um callback de evento de alteração de grupo
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterGroupChangeCallback(int callbackId);
    
    /**
     * @brief Registra um callback para evento de alteração de permissão
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerPermissionChangeCallback(PermissionChangeCallback callback);
    
    /**
     * @brief Remove um callback de evento de alteração de permissão
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterPermissionChangeCallback(int callbackId);
    
    /**
     * @brief Define o limite máximo de usuários para um grupo
     * @param groupId ID do grupo
     * @param maxUsers Número máximo de usuários
     * @return true se o limite foi definido com sucesso
     */
    bool setGroupUserLimit(const std::string& groupId, int maxUsers);
    
    /**
     * @brief Obtém o limite atual de usuários para um grupo
     * @param groupId ID do grupo
     * @return Número máximo de usuários ou -1 se não existir
     */
    int getGroupUserLimit(const std::string& groupId) const;
    
    /**
     * @brief Define um grupo como privado (apenas por convite)
     * @param groupId ID do grupo
     * @param isPrivate Se deve ser privado
     * @return true se a privacidade foi alterada com sucesso
     */
    bool setGroupPrivate(const std::string& groupId, bool isPrivate);
    
    /**
     * @brief Verifica se um grupo é privado
     * @param groupId ID do grupo
     * @return true se o grupo é privado
     */
    bool isGroupPrivate(const std::string& groupId) const;
    
    /**
     * @brief Obtém a lista de grupos disponíveis (públicos)
     * @return Lista de IDs de grupos públicos
     */
    std::vector<std::string> getAvailableGroups() const;
    
    /**
     * @brief Define um proprietário de grupo
     * @param groupId ID do grupo
     * @param userId ID do usuário proprietário
     * @return true se o proprietário foi definido com sucesso
     */
    bool setGroupOwner(const std::string& groupId, const std::string& userId);
    
    /**
     * @brief Obtém o proprietário de um grupo
     * @param groupId ID do grupo
     * @return ID do usuário proprietário ou string vazia se não existir
     */
    std::string getGroupOwner(const std::string& groupId) const;

private:
    // Estrutura para armazenar informações de um grupo
    struct GroupInfo {
        std::string name;
        VoIPGroupType type;
        bool persistent;
        std::string owner;
        std::set<std::string> members;
        std::map<std::string, std::set<VoIPGroupPermission>> permissions;
        std::set<VoIPGroupPermission> defaultPermissions;
        int userLimit;
        bool isPrivate;
    };
    
    // Método para notificar alterações de grupo
    void notifyGroupChange(const std::string& groupId, 
                          const std::string& userId, 
                          bool joined);
    
    // Método para notificar alterações de permissão
    void notifyPermissionChange(const std::string& groupId, 
                               const std::string& userId,
                               VoIPGroupPermission permission, 
                               bool granted);
    
    // Dados de estado
    bool m_initialized;
    std::map<std::string, GroupInfo> m_groups;
    mutable std::mutex m_groupsMutex;
    
    // Mapeamento inverso para consulta rápida de usuários -> grupos
    std::map<std::string, std::set<std::string>> m_userGroups;
    mutable std::mutex m_userGroupsMutex;
    
    // Callbacks
    struct Callbacks {
        std::vector<std::pair<int, GroupChangeCallback>> groupChange;
        std::vector<std::pair<int, PermissionChangeCallback>> permissionChange;
        int nextCallbackId;
        mutable std::mutex mutex;
    } m_callbacks;
};

} // namespace communication
} // namespace wydbr

#endif // WYDBR_VOIP_GROUP_MANAGER_H
