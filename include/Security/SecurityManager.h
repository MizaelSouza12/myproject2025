/**
 * @file SecurityManager.h
 * @brief Gerenciador central de segurança para WYDBR 2.0
 * 
 * Implementa proteções contra exploits conhecidos e ataques,
 * mantendo compatibilidade total com o código C++ original.
 */

#ifndef WYDBR_SECURITY_MANAGER_H
#define WYDBR_SECURITY_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace WYDBR {
namespace Security {

/**
 * @class SecurityManager
 * @brief Gerencia todos os aspectos de segurança do servidor
 * 
 * Esta classe implementa proteções contra diversos vetores de ataque,
 * incluindo buffer overflows, SQL injection, validação de pacotes,
 * e proteção contra tentativas de exploração de memória.
 */
class SecurityManager {
public:
    /**
     * @brief Obtém a instância singleton do gerenciador
     * @return Referência ao gerenciador de segurança
     */
    static SecurityManager& GetInstance();

    /**
     * @brief Inicializa o sistema de segurança
     * @param config_path Caminho para o arquivo de configuração
     * @return true se a inicialização foi bem-sucedida
     */
    bool Initialize(const std::string& config_path);
    
    /**
     * @brief Verifica se um pacote de rede é válido e seguro
     * @param data Ponteiro para os dados do pacote
     * @param size Tamanho do pacote em bytes
     * @return true se o pacote é válido e não contém exploits conhecidos
     */
    bool ValidatePacket(const uint8_t* data, size_t size);
    
    /**
     * @brief Sanitiza uma string SQL para prevenir injeção
     * @param input String de entrada potencialmente não-confiável
     * @return String sanitizada segura para uso em consultas SQL
     */
    std::string SanitizeSqlInput(const std::string& input);
    
    /**
     * @brief Sanitiza um comando para prevenir injeção de comandos
     * @param command Comando a ser sanitizado
     * @return true se o comando está na lista de comandos permitidos
     */
    bool ValidateCommand(const std::string& command);
    
    /**
     * @brief Registra uma tentativa de ataque ou comportamento suspeito
     * @param attack_type Tipo de ataque detectado
     * @param severity Nível de severidade (1-5)
     * @param player_id ID do jogador (opcional)
     * @param details Detalhes adicionais sobre o ataque
     */
    void RegisterAttackAttempt(
        const std::string& attack_type,
        int severity,
        int player_id = -1,
        const std::string& details = ""
    );
    
    /**
     * @brief Bloqueia temporariamente um endereço IP suspeito
     * @param ip Endereço IP a ser bloqueado
     * @param duration_minutes Duração do bloqueio em minutos
     * @param reason Motivo do bloqueio
     */
    void BanIP(const std::string& ip, int duration_minutes, const std::string& reason);
    
    /**
     * @brief Verifica se um endereço IP está banido
     * @param ip Endereço IP a verificar
     * @return true se o IP está banido
     */
    bool IsIPBanned(const std::string& ip);
    
    /**
     * @brief Gera uma nova chave de criptografia criptograficamente segura
     * @param length Tamanho da chave em bytes
     * @return Chave gerada como string hexadecimal
     */
    std::string GenerateSecureRandomKey(size_t length = 32);
    
private:
    // Construtor privado (singleton)
    SecurityManager();
    
    // Destrutor
    ~SecurityManager();
    
    // Proibir cópia e atribuição
    SecurityManager(const SecurityManager&) = delete;
    SecurityManager& operator=(const SecurityManager&) = delete;
    
    // Métodos internos
    bool InitializeAntiCheat();
    bool InitializePacketValidation();
    bool InitializeInputSanitization();
    void CleanupExpiredBans();
    
    // Estado interno
    bool is_initialized_ = false;
    std::vector<std::string> attack_signatures_;
    
    // Mutex para threadsafety
    std::mutex mutex_;
    
    // Registro de endereços IP banidos
    struct BanEntry {
        std::time_t expiry_time;
        std::string reason;
    };
    std::unordered_map<std::string, BanEntry> banned_ips_;
};

} // namespace Security
} // namespace WYDBR

#endif // WYDBR_SECURITY_MANAGER_H
