#ifndef WYDBR_SECURITY_MANAGER_H
#define WYDBR_SECURITY_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <functional>
#include <memory>

#include "../screen_sharing_system.h"

namespace wydbr {
namespace communication {

// Estrutura para configuração de segurança
struct SecurityConfig {
    bool enableEncryption;
    bool verifyIntegrity;
    bool requireAuthentication;
    bool logSensitiveActions;
    bool enablePermissionSystem;
    bool blockScreenshot;
    bool blockRecording;
    bool createWatermark;
    std::string encryptionAlgorithm;
    int encryptionKeySize;
    int sessionTimeout;
    
    // Construtor com valores padrão
    SecurityConfig() :
        enableEncryption(true),
        verifyIntegrity(true),
        requireAuthentication(true),
        logSensitiveActions(true),
        enablePermissionSystem(true),
        blockScreenshot(false),
        blockRecording(false),
        createWatermark(false),
        encryptionAlgorithm("AES-GCM"),
        encryptionKeySize(256),
        sessionTimeout(3600)
    {}
};

// Estrutura para contexto de segurança
struct SecurityContext {
    std::string sessionId;
    std::string userId;
    std::string authToken;
    int64_t creationTime;
    int64_t lastAccessTime;
    bool isAuthenticated;
    std::map<std::string, std::set<SharingPermission>> permissions;
};

// Estrutura para evento de segurança
struct SecurityEvent {
    enum class Type {
        PERMISSION_CHANGE,
        AUTHENTICATION,
        ENCRYPTION_CHANGE,
        INTEGRITY_VIOLATION,
        SUSPICIOUS_ACTIVITY,
        RATE_LIMIT_EXCEEDED
    };
    
    Type type;
    std::string userId;
    std::string sessionId;
    std::string description;
    int severity;
    int64_t timestamp;
};

// Callback para evento de segurança
using SecurityEventCallback = std::function<void(const SecurityEvent& event)>;

/**
 * @class SecurityManager
 * @brief Gerenciador de segurança para compartilhamento de tela
 * 
 * Gerencia permissões, criptografia, verificação de integridade
 * e medidas anti-gravação para o sistema de compartilhamento.
 */
class SecurityManager {
public:
    SecurityManager();
    ~SecurityManager();
    
    /**
     * @brief Inicializa o gerenciador de segurança
     * @param config Configuração inicial
     * @return true se inicializado com sucesso
     */
    bool initialize(const SecurityConfig& config = SecurityConfig());
    
    /**
     * @brief Desliga o gerenciador e libera recursos
     */
    void shutdown();
    
    /**
     * @brief Cria um novo contexto de segurança
     * @param sessionId ID da sessão
     * @param userId ID do usuário (host)
     * @param authToken Token de autenticação
     * @return true se criado com sucesso
     */
    bool createContext(const std::string& sessionId, 
                      const std::string& userId,
                      const std::string& authToken);
    
    /**
     * @brief Destrói um contexto de segurança
     * @param sessionId ID da sessão
     * @return true se destruído com sucesso
     */
    bool destroyContext(const std::string& sessionId);
    
    /**
     * @brief Autentica um usuário em um contexto
     * @param sessionId ID da sessão
     * @param userId ID do usuário
     * @param authToken Token de autenticação
     * @return true se autenticado com sucesso
     */
    bool authenticateUser(const std::string& sessionId,
                         const std::string& userId,
                         const std::string& authToken);
    
    /**
     * @brief Concede permissão a um usuário
     * @param sessionId ID da sessão
     * @param userId ID do usuário
     * @param permission Permissão a conceder
     * @return true se a permissão foi concedida com sucesso
     */
    bool grantPermission(const std::string& sessionId,
                        const std::string& userId,
                        SharingPermission permission);
    
    /**
     * @brief Revoga permissão de um usuário
     * @param sessionId ID da sessão
     * @param userId ID do usuário
     * @param permission Permissão a revogar
     * @return true se a permissão foi revogada com sucesso
     */
    bool revokePermission(const std::string& sessionId,
                         const std::string& userId,
                         SharingPermission permission);
    
    /**
     * @brief Verifica se um usuário tem uma permissão
     * @param sessionId ID da sessão
     * @param userId ID do usuário
     * @param permission Permissão a verificar
     * @return true se o usuário tem a permissão
     */
    bool hasPermission(const std::string& sessionId,
                      const std::string& userId,
                      SharingPermission permission) const;
    
    /**
     * @brief Define permissões para um usuário
     * @param sessionId ID da sessão
     * @param userId ID do usuário
     * @param permissions Conjunto de permissões
     * @return true se as permissões foram definidas com sucesso
     */
    bool setPermissions(const std::string& sessionId,
                       const std::string& userId,
                       const std::set<SharingPermission>& permissions);
    
    /**
     * @brief Obtém as permissões de um usuário
     * @param sessionId ID da sessão
     * @param userId ID do usuário
     * @return Conjunto de permissões
     */
    std::set<SharingPermission> getPermissions(const std::string& sessionId,
                                              const std::string& userId) const;
    
    /**
     * @brief Criptografa dados
     * @param sessionId ID da sessão
     * @param data Dados a criptografar
     * @param encrypted Dados criptografados (saída)
     * @return true se criptografado com sucesso
     */
    bool encryptData(const std::string& sessionId,
                    const std::vector<uint8_t>& data,
                    std::vector<uint8_t>& encrypted);
    
    /**
     * @brief Descriptografa dados
     * @param sessionId ID da sessão
     * @param encrypted Dados criptografados
     * @param data Dados descriptografados (saída)
     * @return true se descriptografado com sucesso
     */
    bool decryptData(const std::string& sessionId,
                    const std::vector<uint8_t>& encrypted,
                    std::vector<uint8_t>& data);
    
    /**
     * @brief Verifica a integridade de dados
     * @param sessionId ID da sessão
     * @param data Dados a verificar
     * @param signature Assinatura dos dados
     * @return true se a integridade é válida
     */
    bool verifyIntegrity(const std::string& sessionId,
                        const std::vector<uint8_t>& data,
                        const std::vector<uint8_t>& signature);
    
    /**
     * @brief Gera uma assinatura para dados
     * @param sessionId ID da sessão
     * @param data Dados a assinar
     * @param signature Assinatura gerada (saída)
     * @return true se a assinatura foi gerada com sucesso
     */
    bool signData(const std::string& sessionId,
                 const std::vector<uint8_t>& data,
                 std::vector<uint8_t>& signature);
    
    /**
     * @brief Registra um evento de segurança
     * @param event Evento a registrar
     */
    void logSecurityEvent(const SecurityEvent& event);
    
    /**
     * @brief Ativa ou desativa a criptografia
     * @param enable Se deve ativar
     */
    void enableEncryption(bool enable);
    
    /**
     * @brief Verifica se a criptografia está ativa
     * @return true se ativa
     */
    bool isEncryptionEnabled() const;
    
    /**
     * @brief Ativa ou desativa a verificação de integridade
     * @param enable Se deve ativar
     */
    void enableIntegrityVerification(bool enable);
    
    /**
     * @brief Verifica se a verificação de integridade está ativa
     * @return true se ativa
     */
    bool isIntegrityVerificationEnabled() const;
    
    /**
     * @brief Aplica marca d'água aos quadros
     * @param data Dados do quadro (modificados no local)
     * @param width Largura do quadro
     * @param height Altura do quadro
     * @param stride Stride do quadro
     * @param format Formato do quadro
     * @param userId ID do usuário
     */
    void applyWatermark(std::vector<uint8_t>& data,
                       int width, int height, int stride, int format,
                       const std::string& userId);
    
    /**
     * @brief Registra um callback para eventos de segurança
     * @param callback Função a ser chamada
     * @return ID do callback (para remoção)
     */
    int registerSecurityEventCallback(SecurityEventCallback callback);
    
    /**
     * @brief Remove um callback de eventos de segurança
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterSecurityEventCallback(int callbackId);
    
    /**
     * @brief Verifica se um contexto ainda é válido
     * @param sessionId ID da sessão
     * @return true se o contexto é válido
     */
    bool isContextValid(const std::string& sessionId) const;
    
    /**
     * @brief Atualiza o tempo de último acesso de um contexto
     * @param sessionId ID da sessão
     * @return true se atualizado com sucesso
     */
    bool updateLastAccessTime(const std::string& sessionId);
    
private:
    // Métodos internos
    void notifySecurityEvent(const SecurityEvent& event);
    bool generateSessionKeys(const std::string& sessionId);
    void cleanupExpiredContexts();
    
    // Estado interno
    bool m_initialized;
    SecurityConfig m_config;
    
    // Contextos de segurança
    std::map<std::string, SecurityContext> m_contexts;
    mutable std::mutex m_contextsMutex;
    
    // Callbacks
    std::vector<std::pair<int, SecurityEventCallback>> m_callbacks;
    int m_nextCallbackId;
    std::mutex m_callbackMutex;
    
    // Material criptográfico
    std::map<std::string, std::vector<uint8_t>> m_encryptionKeys;
    std::map<std::string, std::vector<uint8_t>> m_hmacKeys;
    std::mutex m_cryptoMutex;
    
    // Log de eventos de segurança
    std::vector<SecurityEvent> m_eventLog;
    std::mutex m_eventLogMutex;
    
    // Implementação criptográfica
    void* m_cryptoImpl;
};

} // namespace communication
} // namespace wydbr

#endif // WYDBR_SECURITY_MANAGER_H
