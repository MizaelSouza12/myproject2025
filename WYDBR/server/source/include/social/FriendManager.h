/**
 * FriendManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/social/FriendManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _FRIEND_MANAGER_H_
#define _FRIEND_MANAGER_H_

/**
 * @file FriendManager.h
 * @brief Gerenciador de amigos do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de amigos do WYDBR,
 * permitindo que jogadores adicionem outros jogadores como amigos,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>

#include "../GlobalDef.h"
#include "../Basedef.h"

namespace wydbr {
namespace social {

/**
 * @brief Estado de amizade
 */
enum /**
 * Classe WYDFriendshipState
 * 
 * Esta classe implementa a funcionalidade WYDFriendshipState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class FriendshipState {
    NONE = 0,               // Nenhum
    PENDING = 1,            // Pendente
    ACCEPTED = 2,           // Aceito
    REJECTED = 3,           // Rejeitado
    BLOCKED = 4,            // Bloqueado
    REMOVED = 5,            // Removido
    CUSTOM_1 = 6,           // Personalizado 1
    CUSTOM_2 = 7,           // Personalizado 2
    CUSTOM_3 = 8,           // Personalizado 3
    UNKNOWN = 9             // Desconhecido
};

/**
 * @brief Resultado da operação de amizade
 */
enum /**
 * Classe WYDFriendResult
 * 
 * Esta classe implementa a funcionalidade WYDFriendResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class FriendResult {
    SUCCESS = 0,                    // Sucesso
    FAILED = 1,                     // Falha
    ALREADY_FRIENDS = 2,            // Já são amigos
    ALREADY_REQUESTED = 3,          // Já solicitado
    TARGET_NOT_FOUND = 4,           // Alvo não encontrado
    SELF_FRIENDSHIP = 5,            // Amizade consigo mesmo
    FRIENDSHIP_LIMIT_REACHED = 6,   // Limite de amizades atingido
    TARGET_BLOCKED = 7,             // Alvo bloqueado
    TARGET_OFFLINE = 8,             // Alvo offline
    ALREADY_BLOCKED = 9,            // Já bloqueado
    NOT_FRIENDS = 10,               // Não são amigos
    NOT_BLOCKED = 11,               // Não está bloqueado
    CUSTOM_1 = 12,                  // Personalizado 1
    CUSTOM_2 = 13,                  // Personalizado 2
    CUSTOM_3 = 14,                  // Personalizado 3
    UNKNOWN = 15                    // Desconhecido
};

/**
 * @brief Tipo de notificação de amizade
 */
enum /**
 * Classe WYDFriendNotificationType
 * 
 * Esta classe implementa a funcionalidade WYDFriendNotificationType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class FriendNotificationType {
    NONE = 0,               // Nenhum
    REQUEST = 1,            // Solicitação
    ACCEPT = 2,             // Aceitação
    REJECT = 3,             // Rejeição
    REMOVE = 4,             // Remoção
    BLOCK = 5,              // Bloqueio
    UNBLOCK = 6,            // Desbloqueio
    LOGIN = 7,              // Login
    LOGOUT = 8,             // Logout
    CUSTOM_1 = 9,           // Personalizado 1
    CUSTOM_2 = 10,          // Personalizado 2
    CUSTOM_3 = 11,          // Personalizado 3
    UNKNOWN = 12            // Desconhecido
};

/**
 * @brief Tipo de grupo de amigos
 */
enum /**
 * Classe WYDFriendGroupType
 * 
 * Esta classe implementa a funcionalidade WYDFriendGroupType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class FriendGroupType {
    NONE = 0,               // Nenhum
    GENERAL = 1,            // Geral
    FAMILY = 2,             // Família
    GUILD = 3,              // Guilda
    CUSTOM_1 = 4,           // Personalizado 1
    CUSTOM_2 = 5,           // Personalizado 2
    CUSTOM_3 = 6,           // Personalizado 3
    CUSTOM_4 = 7,           // Personalizado 4
    CUSTOM_5 = 8,           // Personalizado 5
    UNKNOWN = 9             // Desconhecido
};

/**
 * @brief Entrada de amigo
 */
struct FriendEntry {
    uint32_t OwnerID;           // ID do dono
    uint32_t FriendID;          // ID do amigo
    std::string Note;           // Nota
    FriendshipState State;      // Estado
    uint32_t FriendshipDate;    // Data de amizade
    uint32_t LastInteraction;   // Última interação
    FriendGroupType GroupType;  // Tipo de grupo
    std::string CustomGroup;    // Grupo personalizado
    
    FriendEntry() : OwnerID(0), FriendID(0), State(FriendshipState::NONE),
        FriendshipDate(0), LastInteraction(0), GroupType(FriendGroupType::GENERAL) {}
};

/**
 * @brief Solicitação de amizade
 */
struct FriendRequest {
    uint32_t RequestID;         // ID da solicitação
    uint32_t SenderID;          // ID do remetente
    uint32_t ReceiverID;        // ID do destinatário
    uint32_t RequestTime;       // Tempo da solicitação
    uint32_t ResponseTime;      // Tempo da resposta
    FriendshipState State;      // Estado
    std::string Message;        // Mensagem
    
    FriendRequest() : RequestID(0), SenderID(0), ReceiverID(0),
        RequestTime(0), ResponseTime(0), State(FriendshipState::PENDING) {}
};

/**
 * @brief Notificação de amizade
 */
struct FriendNotification {
    uint32_t NotificationID;    // ID da notificação
    uint32_t SenderID;          // ID do remetente
    uint32_t ReceiverID;        // ID do destinatário
    FriendNotificationType Type;// Tipo
    uint32_t Timestamp;         // Timestamp
    bool Read;                  // Lido
    uint32_t RelatedID;         // ID relacionado
    
    FriendNotification() : NotificationID(0), SenderID(0), ReceiverID(0),
        Type(FriendNotificationType::NONE), Timestamp(0), Read(false), RelatedID(0) {}
};

/**
 * @brief Estatísticas de amizade
 */
struct FriendStats {
    uint32_t CharacterID;       // ID do personagem
    uint32_t FriendCount;       // Contagem de amigos
    uint32_t BlockedCount;      // Contagem de bloqueados
    uint32_t RequestsSent;      // Solicitações enviadas
    uint32_t RequestsReceived;  // Solicitações recebidas
    uint32_t AcceptedCount;     // Contagem de aceitos
    uint32_t RejectedCount;     // Contagem de rejeitados
    uint32_t RemovedCount;      // Contagem de removidos
    
    FriendStats() : CharacterID(0), FriendCount(0), BlockedCount(0),
        RequestsSent(0), RequestsReceived(0), AcceptedCount(0),
        RejectedCount(0), RemovedCount(0) {}
};

/**
 * @brief Configuração de amigos
 */
struct FriendConfig {
    uint32_t MaxFriends;        // Máximo de amigos
    uint32_t MaxBlocked;        // Máximo de bloqueados
    uint32_t RequestTimeout;    // Timeout de solicitação
    bool AutoRejectIfBlocked;   // Auto rejeitar se bloqueado
    bool NotifyOnLogin;         // Notificar ao fazer login
    bool NotifyOnLogout;        // Notificar ao fazer logout
    bool AllowOfflineRequests;  // Permitir solicitações offline
    
    FriendConfig() : MaxFriends(100), MaxBlocked(50), RequestTimeout(0),
        AutoRejectIfBlocked(true), NotifyOnLogin(true), NotifyOnLogout(true),
        AllowOfflineRequests(true) {}
};

/**
 * @brief Estado global de amizades
 */
struct GlobalFriendState {
    uint32_t TotalFriendships;  // Total de amizades
    uint32_t TotalRequests;     // Total de solicitações
    uint32_t TotalBlocked;      // Total de bloqueados
    uint32_t TodayRequests;     // Solicitações hoje
    uint32_t TodayAccepted;     // Aceitos hoje
    uint32_t TodayRejected;     // Rejeitados hoje
    
    GlobalFriendState() : TotalFriendships(0), TotalRequests(0),
        TotalBlocked(0), TodayRequests(0), TodayAccepted(0),
        TodayRejected(0) {}
};

/**
 * @brief Tipo de callback de amizade
 */
using FriendCallback = std::function<void(const FriendEntry&, FriendshipState)>;

/**
 * @brief Tipo de callback de notificação
 */
using NotificationCallback = std::function<void(const FriendNotification&)>;

/**
 * @brief Gerenciador de amigos
 * 
 * Esta classe é responsável por gerenciar a lista de amigos dos jogadores.
 */
/**
 * Classe WYDFriendManager
 * 
 * Esta classe implementa a funcionalidade WYDFriendManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class FriendManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static FriendManager& getInstance() {
        static FriendManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração de amigos
     * @return true se carregado com sucesso
     */
    bool loadFriendConfig();
    
    /**
     * @brief Carrega a lista de amigos de um jogador
     * @param characterID ID do personagem
     * @return true se carregado com sucesso
     */
    bool loadFriendList(uint32_t characterID);
    
    /**
     * @brief Envia uma solicitação de amizade
     * @param senderID ID do remetente
     * @param receiverID ID do destinatário
     * @param message Mensagem
     * @return ID da solicitação ou 0 em caso de erro
     */
    uint32_t sendFriendRequest(uint32_t senderID, uint32_t receiverID, const std::string& message = "");
    
    /**
     * @brief Aceita uma solicitação de amizade
     * @param requestID ID da solicitação
     * @return Resultado da operação
     */
    FriendResult acceptFriendRequest(uint32_t requestID);
    
    /**
     * @brief Rejeita uma solicitação de amizade
     * @param requestID ID da solicitação
     * @return Resultado da operação
     */
    FriendResult rejectFriendRequest(uint32_t requestID);
    
    /**
     * @brief Remove um amigo
     * @param ownerID ID do dono
     * @param friendID ID do amigo
     * @return Resultado da operação
     */
    FriendResult removeFriend(uint32_t ownerID, uint32_t friendID);
    
    /**
     * @brief Bloqueia um jogador
     * @param ownerID ID do dono
     * @param targetID ID do alvo
     * @return Resultado da operação
     */
    FriendResult blockPlayer(uint32_t ownerID, uint32_t targetID);
    
    /**
     * @brief Desbloqueia um jogador
     * @param ownerID ID do dono
     * @param targetID ID do alvo
     * @return Resultado da operação
     */
    FriendResult unblockPlayer(uint32_t ownerID, uint32_t targetID);
    
    /**
     * @brief Adiciona uma nota a um amigo
     * @param ownerID ID do dono
     * @param friendID ID do amigo
     * @param note Nota
     * @return Resultado da operação
     */
    FriendResult addFriendNote(uint32_t ownerID, uint32_t friendID, const std::string& note);
    
    /**
     * @brief Define o grupo de um amigo
     * @param ownerID ID do dono
     * @param friendID ID do amigo
     * @param groupType Tipo de grupo
     * @param customGroup Grupo personalizado
     * @return Resultado da operação
     */
    FriendResult setFriendGroup(uint32_t ownerID, uint32_t friendID, FriendGroupType groupType, const std::string& customGroup = "");
    
    /**
     * @brief Notifica o status online
     * @param characterID ID do personagem
     * @param online Online
     * @return true se notificado com sucesso
     */
    bool notifyOnlineStatus(uint32_t characterID, bool online);
    
    /**
     * @brief Verifica se dois jogadores são amigos
     * @param characterID1 ID do personagem 1
     * @param characterID2 ID do personagem 2
     * @return true se forem amigos
     */
    bool areFriends(uint32_t characterID1, uint32_t characterID2) const;
    
    /**
     * @brief Verifica se um jogador está bloqueado
     * @param ownerID ID do dono
     * @param targetID ID do alvo
     * @return true se estiver bloqueado
     */
    bool isBlocked(uint32_t ownerID, uint32_t targetID) const;
    
    /**
     * @brief Obtém os amigos de um jogador
     * @param characterID ID do personagem
     * @return Lista de amigos
     */
    std::vector<FriendEntry> getFriendList(uint32_t characterID) const;
    
    /**
     * @brief Obtém os amigos de um jogador por grupo
     * @param characterID ID do personagem
     * @param groupType Tipo de grupo
     * @return Lista de amigos
     */
    std::vector<FriendEntry> getFriendsByGroup(uint32_t characterID, FriendGroupType groupType) const;
    
    /**
     * @brief Obtém os amigos de um jogador por grupo personalizado
     * @param characterID ID do personagem
     * @param customGroup Grupo personalizado
     * @return Lista de amigos
     */
    std::vector<FriendEntry> getFriendsByCustomGroup(uint32_t characterID, const std::string& customGroup) const;
    
    /**
     * @brief Obtém os jogadores bloqueados por um jogador
     * @param characterID ID do personagem
     * @return Lista de bloqueados
     */
    std::vector<uint32_t> getBlockedPlayers(uint32_t characterID) const;
    
    /**
     * @brief Obtém as solicitações de amizade pendentes
     * @param characterID ID do personagem
     * @param asSender Como remetente
     * @return Lista de solicitações
     */
    std::vector<FriendRequest> getPendingRequests(uint32_t characterID, bool asSender = false) const;
    
    /**
     * @brief Obtém as notificações de amizade
     * @param characterID ID do personagem
     * @param unreadOnly Apenas não lidas
     * @return Lista de notificações
     */
    std::vector<FriendNotification> getFriendNotifications(uint32_t characterID, bool unreadOnly = false) const;
    
    /**
     * @brief Marca notificações como lidas
     * @param characterID ID do personagem
     * @param notificationID ID da notificação (0 para todas)
     * @return Número de notificações marcadas
     */
    uint32_t markNotificationsAsRead(uint32_t characterID, uint32_t notificationID = 0);
    
    /**
     * @brief Obtém as estatísticas de amizade de um jogador
     * @param characterID ID do personagem
     * @return Estatísticas
     */
    FriendStats getFriendStats(uint32_t characterID) const;
    
    /**
     * @brief Registra um callback de amizade
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerFriendCallback(FriendCallback callback);
    
    /**
     * @brief Remove um callback de amizade
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterFriendCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de notificação
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerNotificationCallback(NotificationCallback callback);
    
    /**
     * @brief Remove um callback de notificação
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterNotificationCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém a configuração de amigos
     * @return Configuração
     */
    FriendConfig getFriendConfig() const;
    
    /**
     * @brief Define a configuração de amigos
     * @param config Configuração
     */
    void setFriendConfig(const FriendConfig& config);
    
    /**
     * @brief Obtém o estado global de amizades
     * @return Estado global
     */
    GlobalFriendState getGlobalState() const;
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
private:
    /**
     * @brief Construtor privado
     */
    FriendManager();
    
    /**
     * @brief Destrutor privado
     */
    ~FriendManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    FriendManager(const FriendManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    FriendManager& operator=(const FriendManager&) = delete;
    
    // Amizades
    std::unordered_map<uint32_t, std::vector<FriendEntry>> _friendLists;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _blockedLists;
    
    // Solicitações
    std::unordered_map<uint32_t, FriendRequest> _friendRequests;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _sentRequests;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _receivedRequests;
    std::atomic<uint32_t> _nextRequestID;
    
    // Notificações
    std::unordered_map<uint32_t, std::vector<FriendNotification>> _notifications;
    std::atomic<uint32_t> _nextNotificationID;
    
    // Estatísticas
    std::unordered_map<uint32_t, FriendStats> _friendStats;
    GlobalFriendState _globalState;
    
    // Configuração
    FriendConfig _friendConfig;
    
    // Callbacks
    std::vector<FriendCallback> _friendCallbacks;
    std::vector<NotificationCallback> _notificationCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _friendMutex;
    mutable std::mutex _requestMutex;
    mutable std::mutex _notificationMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Timers
    uint32_t _requestTimeoutTimer;
    
    /**
     * @brief Verifica solicitações expiradas
     * @param currentTime Tempo atual
     */
    void checkExpiredRequests(uint32_t currentTime);
    
    /**
     * @brief Atualiza o estado global
     */
    void updateGlobalState();
    
    /**
     * @brief Adiciona uma notificação
     * @param senderID ID do remetente
     * @param receiverID ID do destinatário
     * @param type Tipo
     * @param relatedID ID relacionado
     * @return ID da notificação
     */
    uint32_t addNotification(uint32_t senderID, uint32_t receiverID, FriendNotificationType type, uint32_t relatedID = 0);
    
    /**
     * @brief Atualiza as estatísticas de amizade
     * @param characterID ID do personagem
     */
    void updateFriendStats(uint32_t characterID);
    
    /**
     * @brief Executa callbacks de amizade
     * @param entry Entrada
     * @param oldState Estado antigo
     */
    void executeFriendCallbacks(const FriendEntry& entry, FriendshipState oldState);
    
    /**
     * @brief Executa callbacks de notificação
     * @param notification Notificação
     */
    void executeNotificationCallbacks(const FriendNotification& notification);
    
    /**
     * @brief Carrega dados de amizade do banco de dados
     * @return true se carregado com sucesso
     */
    bool loadFriendsFromDatabase();
    
    /**
     * @brief Salva dados de amizade no banco de dados
     * @return true se salvo com sucesso
     */
    bool saveFriendsToDatabase();
};

} // namespace social
} // namespace wydbr

#endif // _FRIEND_MANAGER_H_