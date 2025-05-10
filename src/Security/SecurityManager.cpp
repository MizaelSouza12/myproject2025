/**
 * @file SecurityManager.cpp
 * @brief Implementação do gerenciador de segurança
 */

#include "Security/SecurityManager.h"
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <random>
#include <iomanip>
#include <ctime>

namespace WYDBR {
namespace Security {

SecurityManager& SecurityManager::GetInstance() {
    static SecurityManager instance;
    return instance;
}

SecurityManager::SecurityManager() {
    // Construtor deixado vazio intencionalmente
}

SecurityManager::~SecurityManager() {
    // Limpeza de recursos, se necessário
}

bool SecurityManager::Initialize(const std::string& config_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (is_initialized_) {
        return true;
    }
    
    bool success = true;
    
    // Carregar assinaturas de ataque do arquivo de configuração
    try {
        std::ifstream config_file(config_path);
        if (config_file.is_open()) {
            std::string line;
            while (std::getline(config_file, line)) {
                // Ignorar linhas de comentário e vazias
                if (line.empty() || line[0] == '#' || line[0] == ';') continue;
                
                // Processar linha
                attack_signatures_.push_back(line);
            }
        } else {
            // Arquivo não pode ser aberto, usar valores padrão
            attack_signatures_ = {
                "SELECT.*FROM", "INSERT.*INTO", "UPDATE.*SET", "DELETE.*FROM",
                "UNION.*SELECT", "DROP.*TABLE", "CREATE.*TABLE", "ALTER.*TABLE",
                "EXEC.*SP", "EXEC.*XP", "/*", "*/", "@@", "char\(", "--",
                "eval\(", "exec\(", "system\(", "passthru\("
            };
        }
    } catch (const std::exception& e) {
        // Erro ao processar arquivo, usar valores padrão
        attack_signatures_ = {
            "SELECT.*FROM", "INSERT.*INTO", "UPDATE.*SET", "DELETE.*FROM",
            "UNION.*SELECT", "DROP.*TABLE", "CREATE.*TABLE", "ALTER.*TABLE",
            "EXEC.*SP", "EXEC.*XP", "/*", "*/", "@@", "char\(", "--",
            "eval\(", "exec\(", "system\(", "passthru\("
        };
        success = false;
    }
    
    // Inicializar componentes de segurança
    success = success && InitializeAntiCheat();
    success = success && InitializePacketValidation();
    success = success && InitializeInputSanitization();
    
    if (success) {
        is_initialized_ = true;
    }
    
    return success;
}

bool SecurityManager::ValidatePacket(const uint8_t* data, size_t size) {
    if (!data || size <= 0 || size > 4096) {
        return false; // Dados inválidos ou tamanho suspeito
    }
    
    // Validação básica de cabeçalho de pacote
    if (size < 4) {
        return false; // Pacote muito pequeno para ter cabeçalho válido
    }
    
    // Verificação de tamanho consistente
    uint16_t packet_size = *reinterpret_cast<const uint16_t*>(data);
    if (packet_size != size) {
        return false; // Tamanho no cabeçalho não corresponde ao tamanho real
    }
    
    // Verificação de opcode válido
    uint16_t opcode = *reinterpret_cast<const uint16_t*>(data + 2);
    if (opcode == 0 || opcode > 2000) {
        return false; // Opcode inválido
    }
    
    // Mais verificações específicas podem ser adicionadas aqui
    
    return true;
}

std::string SecurityManager::SanitizeSqlInput(const std::string& input) {
    std::string sanitized = input;
    
    // Escapar aspas simples (a maneira mais comum de SQL injection)
    size_t pos = 0;
    while ((pos = sanitized.find("'", pos)) != std::string::npos) {
        sanitized.replace(pos, 1, "''");
        pos += 2;
    }
    
    // Remover comentários SQL
    sanitized = std::regex_replace(sanitized, std::regex("--.*"), "");
    sanitized = std::regex_replace(sanitized, std::regex("/\*.*?\*/"), "");
    
    // Remover caracteres de controle e não-imprimíveis
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(), 
        [](unsigned char c) { return c < 32 || c == 127; }), sanitized.end());
    
    return sanitized;
}

bool SecurityManager::ValidateCommand(const std::string& command) {
    // Lista de comandos permitidos (exemplo)
    static const std::vector<std::string> allowed_commands = {
        "status", "help", "list", "info", "restart", "shutdown", "reload"
    };
    
    // Verificar se o comando está na lista de permitidos
    return std::find(allowed_commands.begin(), allowed_commands.end(), command) 
           != allowed_commands.end();
}

void SecurityManager::RegisterAttackAttempt(
    const std::string& attack_type,
    int severity,
    int player_id,
    const std::string& details) 
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Implementação real registraria em um log ou banco de dados
    // Este é um exemplo simplificado
    std::time_t now = std::time(nullptr);
    std::ostringstream log_entry;
    
    log_entry << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S")
              << " | Severity: " << severity
              << " | Type: " << attack_type;
              
    if (player_id >= 0) {
        log_entry << " | Player ID: " << player_id;
    }
    
    if (!details.empty()) {
        log_entry << " | Details: " << details;
    }
    
    // Em uma implementação real, isto iria para um arquivo de log seguro
    // ou sistema de monitoramento
    std::cout << "SECURITY ALERT: " << log_entry.str() << std::endl;
}

void SecurityManager::BanIP(const std::string& ip, int duration_minutes, const std::string& reason) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto expiry = now + (duration_minutes * 60);
    
    banned_ips_[ip] = {expiry, reason};
    
    // Limpar entradas expiradas periodicamente
    CleanupExpiredBans();
}

bool SecurityManager::IsIPBanned(const std::string& ip) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    auto it = banned_ips_.find(ip);
    if (it != banned_ips_.end()) {
        if (it->second.expiry_time > now) {
            return true; // IP está banido e o banimento não expirou
        } else {
            banned_ips_.erase(it); // Limpar banimento expirado
        }
    }
    
    return false;
}

std::string SecurityManager::GenerateSecureRandomKey(size_t length) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    std::vector<uint8_t> random_bytes(length);
    for (size_t i = 0; i < length; ++i) {
        random_bytes[i] = static_cast<uint8_t>(dis(gen));
    }
    
    std::stringstream ss;
    for (auto byte : random_bytes) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    
    return ss.str();
}

bool SecurityManager::InitializeAntiCheat() {
    // Inicializar sistema anti-cheat
    return true;
}

bool SecurityManager::InitializePacketValidation() {
    // Inicializar validação de pacotes
    return true;
}

bool SecurityManager::InitializeInputSanitization() {
    // Inicializar sanitização de entrada
    return true;
}

void SecurityManager::CleanupExpiredBans() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    for (auto it = banned_ips_.begin(); it != banned_ips_.end(); ) {
        if (it->second.expiry_time <= now) {
            it = banned_ips_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace Security
} // namespace WYDBR
