#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

class AntiCheatGenerator {
private:
    bool createAntiCheatManager(const std::string& basePath) {
        // Garantir que o diretório exista
        std::string dirPath = basePath + "/common/security";
        fs::create_directories(dirPath);
        
        // Criar o arquivo de cabeçalho AntiCheatManager.h
        std::string headerPath = dirPath + "/AntiCheatManager.h";
        std::ofstream headerFile(headerPath);
        
        if (!headerFile.is_open()) {
            std::cerr << "Falha ao criar arquivo: " << headerPath << std::endl;
            return false;
        }
        
        headerFile << R"(/**
 * @file AntiCheatManager.h
 * @brief Sistema avançado de detecção e prevenção de trapaças
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include "../network/PacketData.h"
#include "../entities/Character.h"
#include "../utils/Logger.h"

namespace wydbr {
namespace security {

/**
 * @enum AntiCheatDetectionLevel
 * @brief Níveis de severidade de detecção de trapaças
 */
enum class AntiCheatDetectionLevel {
    LEVEL_INFO,       // Apenas informacional, pode ser comportamento incomum
    LEVEL_SUSPICIOUS, // Comportamento suspeito, mas não conclusivo
    LEVEL_WARNING,    // Alta probabilidade de trapaça
    LEVEL_VIOLATION   // Trapaça confirmada
};

/**
 * @enum AntiCheatViolationType
 * @brief Tipos de violações que podem ser detectadas
 */
enum class AntiCheatViolationType {
    SPEED_HACK,             // Movimentação anormalmente rápida
    TELEPORT_HACK,          // Teletransporte não autorizado
    PACKET_MODIFICATION,    // Modificação de pacotes
    CLIENT_MODIFICATION,    // Modificação do cliente
    SKILL_COOLDOWN_BYPASS,  // Bypass de cooldowns de habilidades
    WALL_HACK,              // Travessia de paredes ou obstáculos
    DAMAGE_MODIFICATION,    // Modificação de valores de dano
    BOT_BEHAVIOR,           // Comportamento sugestivo de uso de bot
    PACKET_INJECTION,       // Injeção de pacotes fraudulentos
    MEMORY_MANIPULATION,    // Manipulação de memória no cliente
    UNUSUAL_BEHAVIOR        // Comportamento anormal não classificado
};

/**
 * @struct AntiCheatViolation
 * @brief Estrutura que representa uma violação detectada
 */
struct AntiCheatViolation {
    uint64_t timestamp;
    uint32_t accountId;
    uint32_t characterId;
    std::string playerName;
    std::string ipAddress;
    AntiCheatViolationType type;
    AntiCheatDetectionLevel level;
    std::string details;
    std::vector<uint8_t> evidenceData;
};

/**
 * @class AntiCheatManager
 * @brief Gerencia a detecção e prevenção de trapaças no sistema
 */
class AntiCheatManager {
public:
    /**
     * @brief Obtém a instância singleton do AntiCheatManager
     * @return Referência à instância singleton
     */
    static AntiCheatManager& getInstance();
    
    /**
     * @brief Inicializa o sistema anti-cheat
     * @param configPath Caminho para o arquivo de configuração
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool initialize(const std::string& configPath);
    
    /**
     * @brief Analisa pacote para detecção de manipulação
     * @param accountId ID da conta
     * @param packetData Dados do pacote
     * @param expectedSize Tamanho esperado do pacote
     * @return true se o pacote é válido, false se for detectada manipulação
     */
    bool validatePacket(uint32_t accountId, const PacketData& packetData, size_t expectedSize);
    
    /**
     * @brief Verifica movimentação do personagem para detectar speed hack
     * @param character Personagem a ser verificado
     * @param newX Nova coordenada X
     * @param newY Nova coordenada Y
     * @param timestamp Timestamp do movimento
     * @return true se o movimento é válido, false se for detectada anomalia
     */
    bool validateMovement(
        const std::shared_ptr<Character>& character,
        float newX, float newY,
        uint64_t timestamp
    );
    
    /**
     * @brief Verifica uso de skill para detectar bypass de cooldown
     * @param characterId ID do personagem
     * @param skillId ID da skill
     * @param timestamp Timestamp do uso da skill
     * @return true se o uso da skill é válido, false se for detectado bypass
     */
    bool validateSkillUsage(uint32_t characterId, uint32_t skillId, uint64_t timestamp);
    
    /**
     * @brief Verifica valores de dano para detectar modificações
     * @param attackerId ID do atacante
     * @param targetId ID do alvo
     * @param damageAmount Quantidade de dano
     * @param skillId ID da skill (0 para ataque normal)
     * @return true se o dano é válido, false se for detectada manipulação
     */
    bool validateDamage(
        uint32_t attackerId,
        uint32_t targetId,
        int damageAmount,
        uint32_t skillId = 0
    );
    
    /**
     * @brief Analisa comportamento para detectar uso de bot
     * @param characterId ID do personagem
     * @param actionType Tipo de ação realizada
     * @param timestamp Timestamp da ação
     * @return true se o comportamento parece ser humano, false se suspeito de bot
     */
    bool analyzePlayerBehavior(
        uint32_t characterId,
        uint32_t actionType,
        uint64_t timestamp
    );
    
    /**
     * @brief Verifica assinatura do cliente para detectar modificações
     * @param accountId ID da conta
     * @param clientSignature Assinatura enviada pelo cliente
     * @return true se a assinatura é válida, false se for detectada modificação
     */
    bool validateClientIntegrity(uint32_t accountId, const std::string& clientSignature);
    
    /**
     * @brief Registra uma violação detectada
     * @param violation Detalhes da violação
     */
    void reportViolation(const AntiCheatViolation& violation);
    
    /**
     * @brief Aplica penalidade automática com base no histórico de violações
     * @param accountId ID da conta
     * @param violationType Tipo de violação
     * @return true se uma penalidade foi aplicada, false caso contrário
     */
    bool applyPenalty(uint32_t accountId, AntiCheatViolationType violationType);
    
    /**
     * @brief Obtém histórico de violações de uma conta
     * @param accountId ID da conta
     * @return Lista de violações registradas
     */
    std::vector<AntiCheatViolation> getViolationHistory(uint32_t accountId);
    
    /**
     * @brief Finaliza o sistema anti-cheat e salva dados pendentes
     */
    void shutdown();
    
private:
    // Construtor privado para singleton
    AntiCheatManager();
    ~AntiCheatManager();
    
    // Previne cópia/atribuição
    AntiCheatManager(const AntiCheatManager&) = delete;
    AntiCheatManager& operator=(const AntiCheatManager&) = delete;
    
    // Estruturas de dados internas
    std::unordered_map<uint32_t, std::vector<AntiCheatViolation>> violationHistory;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint64_t>> skillCooldowns;
    std::unordered_map<uint32_t, std::tuple<float, float, uint64_t>> lastPositions;
    std::unordered_map<uint32_t, std::vector<std::tuple<uint32_t, uint64_t>>> actionHistory;
    
    // Proteção multithread
    std::shared_mutex dataMutex;
    
    // Configurações
    struct {
        float maxMovementSpeed;
        float maxTeleportDistance;
        int maxDamageMultiplier;
        int minActionIntervalMs;
        int suspiciousActionsThreshold;
        bool enableBotDetection;
        bool enableClientIntegrityCheck;
        bool autoApplyPenalties;
        std::string violationLogPath;
    } config;
    
    // Métodos auxiliares
    bool loadConfiguration(const std::string& configPath);
    void logViolation(const AntiCheatViolation& violation);
    float calculateDistanceSq(float x1, float y1, float x2, float y2);
    bool isPatternSuspicious(const std::vector<std::tuple<uint32_t, uint64_t>>& actions);
    void cleanupOldData();
};

} // namespace security
} // namespace wydbr
)";
        headerFile.close();
        
        // Criar o arquivo de implementação AntiCheatManager.cpp
        std::string implPath = dirPath + "/AntiCheatManager.cpp";
        std::ofstream implFile(implPath);
        
        if (!implFile.is_open()) {
            std::cerr << "Falha ao criar arquivo: " << implPath << std::endl;
            return false;
        }
        
        implFile << R"(/**
 * @file AntiCheatManager.cpp
 * @brief Implementação do sistema avançado anti-cheat
 */

#include "AntiCheatManager.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <nlohmann/json.hpp>
#include "../utils/FileUtils.h"
#include "../utils/TimeUtils.h"
#include "../network/NetworkConstants.h"
#include "../utils/SecurityUtils.h"

namespace wydbr {
namespace security {

using json = nlohmann::json;

// Implementação do singleton
AntiCheatManager& AntiCheatManager::getInstance() {
    static AntiCheatManager instance;
    return instance;
}

AntiCheatManager::AntiCheatManager() {
    // Valores default para configuração
    config.maxMovementSpeed = 20.0f;
    config.maxTeleportDistance = 50.0f;
    config.maxDamageMultiplier = 3;
    config.minActionIntervalMs = 150;
    config.suspiciousActionsThreshold = 10;
    config.enableBotDetection = true;
    config.enableClientIntegrityCheck = true;
    config.autoApplyPenalties = true;
    config.violationLogPath = "logs/anticheat_violations.log";
}

AntiCheatManager::~AntiCheatManager() {
    shutdown();
}

bool AntiCheatManager::initialize(const std::string& configPath) {
    // Carregar configurações
    if (!loadConfiguration(configPath)) {
        Logger::error("AntiCheatManager", "Falha ao carregar configuração: {}", configPath);
        return false;
    }
    
    Logger::info("AntiCheatManager", "Sistema Anti-Cheat inicializado com sucesso");
    return true;
}

bool AntiCheatManager::loadConfiguration(const std::string& configPath) {
    try {
        if (!FileUtils::fileExists(configPath)) {
            Logger::warning("AntiCheatManager", "Arquivo de configuração não encontrado: {}. Usando valores padrão.", configPath);
            return false;
        }
        
        std::ifstream configFile(configPath);
        json configJson;
        configFile >> configJson;
        
        // Carrega valores da configuração
        config.maxMovementSpeed = configJson.value("maxMovementSpeed", 20.0f);
        config.maxTeleportDistance = configJson.value("maxTeleportDistance", 50.0f);
        config.maxDamageMultiplier = configJson.value("maxDamageMultiplier", 3);
        config.minActionIntervalMs = configJson.value("minActionIntervalMs", 150);
        config.suspiciousActionsThreshold = configJson.value("suspiciousActionsThreshold", 10);
        config.enableBotDetection = configJson.value("enableBotDetection", true);
        config.enableClientIntegrityCheck = configJson.value("enableClientIntegrityCheck", true);
        config.autoApplyPenalties = configJson.value("autoApplyPenalties", true);
        config.violationLogPath = configJson.value("violationLogPath", "logs/anticheat_violations.log");
        
        return true;
    } catch (const std::exception& e) {
        Logger::error("AntiCheatManager", "Erro ao carregar configuração: {}", e.what());
        return false;
    }
}

bool AntiCheatManager::validatePacket(uint32_t accountId, const PacketData& packetData, size_t expectedSize) {
    // Verifica tamanho do pacote
    if (packetData.size != expectedSize) {
        AntiCheatViolation violation;
        violation.timestamp = TimeUtils::getCurrentTimestamp();
        violation.accountId = accountId;
        violation.type = AntiCheatViolationType::PACKET_MODIFICATION;
        violation.level = AntiCheatDetectionLevel::LEVEL_VIOLATION;
        violation.details = "Tamanho de pacote inválido. Esperado: " + 
                           std::to_string(expectedSize) + ", Recebido: " + 
                           std::to_string(packetData.size);
        
        // Copia os dados do pacote como evidência
        violation.evidenceData.resize(std::min(packetData.size, size_t(100)));
        std::copy_n(packetData.data, violation.evidenceData.size(), violation.evidenceData.begin());
        
        reportViolation(violation);
        return false;
    }
    
    // Verifica checksum do pacote (simplificado)
    uint16_t calculatedChecksum = SecurityUtils::calculatePacketChecksum(
        packetData.data, packetData.size - sizeof(uint16_t));
    
    uint16_t receivedChecksum = *reinterpret_cast<const uint16_t*>(
        packetData.data + packetData.size - sizeof(uint16_t));
    
    if (calculatedChecksum != receivedChecksum) {
        AntiCheatViolation violation;
        violation.timestamp = TimeUtils::getCurrentTimestamp();
        violation.accountId = accountId;
        violation.type = AntiCheatViolationType::PACKET_MODIFICATION;
        violation.level = AntiCheatDetectionLevel::LEVEL_VIOLATION;
        violation.details = "Checksum de pacote inválido. Esperado: " + 
                           std::to_string(calculatedChecksum) + ", Recebido: " + 
                           std::to_string(receivedChecksum);
        
        reportViolation(violation);
        return false;
    }
    
    // Pacote validado com sucesso
    return true;
}

bool AntiCheatManager::validateMovement(
    const std::shared_ptr<Character>& character,
    float newX, float newY,
    uint64_t timestamp
) {
    if (!character) {
        return false;
    }
    
    uint32_t characterId = character->getId();
    
    // Bloqueia para leitura
    std::shared_lock<std::shared_mutex> lock(dataMutex);
    
    // Verifica se temos posição anterior registrada
    auto it = lastPositions.find(characterId);
    if (it == lastPositions.end()) {
        // Primeira posição registrada, aceitamos
        lastPositions[characterId] = std::make_tuple(newX, newY, timestamp);
        return true;
    }
    
    // Recupera última posição e tempo
    const auto& [lastX, lastY, lastTime] = it->second;
    
    // Calcula tempo decorrido em segundos
    double elapsedSeconds = (timestamp - lastTime) / 1000.0;
    
    // Evita divisão por zero ou valores muito pequenos
    if (elapsedSeconds < 0.01) {
        elapsedSeconds = 0.01;
    }
    
    // Calcula distância percorrida
    float distanceSq = calculateDistanceSq(lastX, lastY, newX, newY);
    float distance = std::sqrt(distanceSq);
    
    // Calcula velocidade (unidades por segundo)
    float speed = distance / elapsedSeconds;
    
    // Verifica se é um teletransporte não autorizado
    if (distance > config.maxTeleportDistance && !character->hasActiveSkill(TELEPORT_SKILL_ID)) {
        AntiCheatViolation violation;
        violation.timestamp = timestamp;
        violation.accountId = character->getAccountId();
        violation.characterId = characterId;
        violation.playerName = character->getName();
        violation.type = AntiCheatViolationType::TELEPORT_HACK;
        violation.level = AntiCheatDetectionLevel::LEVEL_VIOLATION;
        violation.details = "Teletransporte não autorizado. Distância: " + 
                           std::to_string(distance) + ", De: (" + 
                           std::to_string(lastX) + "," + std::to_string(lastY) + 
                           ") Para: (" + std::to_string(newX) + "," + 
                           std::to_string(newY) + ")";
        
        reportViolation(violation);
        return false;
    }
    
    // Verifica se a velocidade excede o limite
    float speedBonus = character->getSpeedBonus();
    float maxAllowedSpeed = config.maxMovementSpeed * (1.0f + speedBonus);
    
    if (speed > maxAllowedSpeed) {
        AntiCheatViolation violation;
        violation.timestamp = timestamp;
        violation.accountId = character->getAccountId();
        violation.characterId = characterId;
        violation.playerName = character->getName();
        violation.type = AntiCheatViolationType::SPEED_HACK;
        violation.level = AntiCheatDetectionLevel::LEVEL_WARNING;
        violation.details = "Movimentação rápida demais. Velocidade: " + 
                           std::to_string(speed) + ", Máximo permitido: " + 
                           std::to_string(maxAllowedSpeed);
        
        reportViolation(violation);
        return false;
    }
    
    // Atualiza posição
    lastPositions[characterId] = std::make_tuple(newX, newY, timestamp);
    
    return true;
}

bool AntiCheatManager::validateSkillUsage(uint32_t characterId, uint32_t skillId, uint64_t timestamp) {
    // Bloqueia para leitura/escrita
    std::unique_lock<std::shared_mutex> lock(dataMutex);
    
    // Verifica se a skill está em cooldown
    auto it = skillCooldowns.find(characterId);
    if (it != skillCooldowns.end()) {
        auto& characterCooldowns = it->second;
        auto cooldownIt = characterCooldowns.find(skillId);
        
        if (cooldownIt != characterCooldowns.end()) {
            uint64_t cooldownEnd = cooldownIt->second;
            
            // Se o timestamp é menor que o fim do cooldown, a skill ainda está em cooldown
            if (timestamp < cooldownEnd) {
                uint64_t remainingCooldown = cooldownEnd - timestamp;
                
                AntiCheatViolation violation;
                violation.timestamp = timestamp;
                violation.characterId = characterId;
                violation.type = AntiCheatViolationType::SKILL_COOLDOWN_BYPASS;
                violation.level = AntiCheatDetectionLevel::LEVEL_VIOLATION;
                violation.details = "Bypass de cooldown. Skill ID: " + 
                                   std::to_string(skillId) + ", Cooldown restante: " + 
                                   std::to_string(remainingCooldown) + "ms";
                
                reportViolation(violation);
                return false;
            }
        }
    }
    
    // Obtém o tempo de cooldown para a skill (em ms)
    uint64_t cooldownTime = 0;
    // TODO: Obter o cooldown real da skill do gerenciador de habilidades
    cooldownTime = 5000; // Placeholder: 5 segundos
    
    // Registra o novo cooldown
    if (cooldownTime > 0) {
        skillCooldowns[characterId][skillId] = timestamp + cooldownTime;
    }
    
    return true;
}

float AntiCheatManager::calculateDistanceSq(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;
}

bool AntiCheatManager::analyzePlayerBehavior(uint32_t characterId, uint32_t actionType, uint64_t timestamp) {
    if (!config.enableBotDetection) {
        return true;
    }
    
    // Bloqueia para leitura/escrita
    std::unique_lock<std::shared_mutex> lock(dataMutex);
    
    // Recupera ou cria o histórico de ações do jogador
    auto& actions = actionHistory[characterId];
    
    // Adiciona a nova ação ao histórico
    actions.emplace_back(actionType, timestamp);
    
    // Mantém apenas as últimas 100 ações
    if (actions.size() > 100) {
        actions.erase(actions.begin());
    }
    
    // Precisa de pelo menos algumas ações para análise
    if (actions.size() < 10) {
        return true;
    }
    
    // Verifica se há padrões regulares demais (indício de botting)
    if (isPatternSuspicious(actions)) {
        AntiCheatViolation violation;
        violation.timestamp = timestamp;
        violation.characterId = characterId;
        violation.type = AntiCheatViolationType::BOT_BEHAVIOR;
        violation.level = AntiCheatDetectionLevel::LEVEL_SUSPICIOUS;
        violation.details = "Comportamento suspeito de bot detectado.";
        
        reportViolation(violation);
        return false;
    }
    
    // Verifica se as ações estão muito próximas umas das outras (ação humana normal tem variação)
    int suspiciousIntervals = 0;
    for (size_t i = 1; i < actions.size(); i++) {
        uint64_t interval = std::get<1>(actions[i]) - std::get<1>(actions[i-1]);
        
        // Intervalo muito regular ou muito pequeno
        if (interval < config.minActionIntervalMs) {
            suspiciousIntervals++;
        }
    }
    
    if (suspiciousIntervals > config.suspiciousActionsThreshold) {
        AntiCheatViolation violation;
        violation.timestamp = timestamp;
        violation.characterId = characterId;
        violation.type = AntiCheatViolationType::BOT_BEHAVIOR;
        violation.level = AntiCheatDetectionLevel::LEVEL_SUSPICIOUS;
        violation.details = "Padrão de ações suspeito. Ações rápidas demais: " + 
                            std::to_string(suspiciousIntervals);
        
        reportViolation(violation);
        return false;
    }
    
    return true;
}

bool AntiCheatManager::isPatternSuspicious(const std::vector<std::tuple<uint32_t, uint64_t>>& actions) {
    // Analisa os intervalos entre ações
    std::vector<uint64_t> intervals;
    intervals.reserve(actions.size() - 1);
    
    for (size_t i = 1; i < actions.size(); i++) {
        intervals.push_back(std::get<1>(actions[i]) - std::get<1>(actions[i-1]));
    }
    
    // Conta ocorrências de cada intervalo (arredondado para os 10ms mais próximos)
    std::unordered_map<uint64_t, int> intervalCounts;
    for (uint64_t interval : intervals) {
        uint64_t roundedInterval = (interval / 10) * 10;
        intervalCounts[roundedInterval]++;
    }
    
    // Se um mesmo intervalo ocorre com frequência muito alta, é suspeito
    int totalIntervals = intervals.size();
    for (const auto& [interval, count] : intervalCounts) {
        float percentage = static_cast<float>(count) / totalIntervals;
        
        // Se mais de 70% dos intervalos são muito similares, é provavelmente um bot
        if (percentage > 0.7f) {
            return true;
        }
    }
    
    return false;
}

bool AntiCheatManager::validateClientIntegrity(uint32_t accountId, const std::string& clientSignature) {
    if (!config.enableClientIntegrityCheck) {
        return true;
    }
    
    // TODO: Implementar verificação real da assinatura do cliente
    // Esta é uma implementação simplificada
    
    // Hash esperado do cliente oficial
    static const std::string OFFICIAL_CLIENT_SIGNATURE = "b8d4f84c9aa84981e6c78bbbfa8ffc8a";
    
    if (clientSignature != OFFICIAL_CLIENT_SIGNATURE) {
        AntiCheatViolation violation;
        violation.timestamp = TimeUtils::getCurrentTimestamp();
        violation.accountId = accountId;
        violation.type = AntiCheatViolationType::CLIENT_MODIFICATION;
        violation.level = AntiCheatDetectionLevel::LEVEL_VIOLATION;
        violation.details = "Assinatura do cliente inválida. Esperada: " + 
                           OFFICIAL_CLIENT_SIGNATURE + ", Recebida: " + 
                           clientSignature;
        
        reportViolation(violation);
        return false;
    }
    
    return true;
}

void AntiCheatManager::reportViolation(const AntiCheatViolation& violation) {
    // Bloqueia para escrita
    std::unique_lock<std::shared_mutex> lock(dataMutex);
    
    // Adiciona ao histórico
    if (violation.accountId != 0) {
        violationHistory[violation.accountId].push_back(violation);
    }
    
    // Loga a violação
    logViolation(violation);
    
    // Aplica penalidade se configurado
    if (config.autoApplyPenalties && 
        violation.level >= AntiCheatDetectionLevel::LEVEL_WARNING) {
        applyPenalty(violation.accountId, violation.type);
    }
}

void AntiCheatManager::logViolation(const AntiCheatViolation& violation) {
    // Garante que o diretório de log existe
    size_t lastSlash = config.violationLogPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string logDir = config.violationLogPath.substr(0, lastSlash);
        if (!FileUtils::createDirectoryIfNotExists(logDir)) {
            Logger::error("AntiCheatManager", "Falha ao criar diretório de log: {}", logDir);
        }
    }
    
    // Formata o log
    std::string logMessage = TimeUtils::timestampToString(violation.timestamp) + 
                             " | Account: " + std::to_string(violation.accountId) +
                             " | Character: " + std::to_string(violation.characterId) +
                             " | Name: " + violation.playerName +
                             " | Type: " + std::to_string(static_cast<int>(violation.type)) +
                             " | Level: " + std::to_string(static_cast<int>(violation.level)) +
                             " | Details: " + violation.details;
    
    // Escreve no arquivo de log
    try {
        std::ofstream logFile(config.violationLogPath, std::ios::app);
        if (logFile.is_open()) {
            logFile << logMessage << std::endl;
            logFile.close();
        } else {
            Logger::error("AntiCheatManager", "Falha ao abrir arquivo de log: {}", config.violationLogPath);
        }
    } catch (const std::exception& e) {
        Logger::error("AntiCheatManager", "Erro ao escrever no log: {}", e.what());
    }
    
    // Também loga no sistema de log geral
    Logger::warning("AntiCheat", "{}", logMessage);
}

bool AntiCheatManager::applyPenalty(uint32_t accountId, AntiCheatViolationType violationType) {
    // Bloqueia para leitura
    std::shared_lock<std::shared_mutex> lock(dataMutex);
    
    // Verifica se temos histórico de violações para esta conta
    auto it = violationHistory.find(accountId);
    if (it == violationHistory.end()) {
        return false;
    }
    
    const auto& violations = it->second;
    
    // Conta o número de violações por tipo e nível
    int infoCount = 0;
    int suspiciousCount = 0;
    int warningCount = 0;
    int violationCount = 0;
    
    for (const auto& v : violations) {
        if (v.type == violationType) {
            switch (v.level) {
                case AntiCheatDetectionLevel::LEVEL_INFO:
                    infoCount++;
                    break;
                case AntiCheatDetectionLevel::LEVEL_SUSPICIOUS:
                    suspiciousCount++;
                    break;
                case AntiCheatDetectionLevel::LEVEL_WARNING:
                    warningCount++;
                    break;
                case AntiCheatDetectionLevel::LEVEL_VIOLATION:
                    violationCount++;
                    break;
            }
        }
    }
    
    // TODO: Implementar sistema real de penalidades
    // Esta é uma implementação simplificada
    
    // Toma ação com base no histórico
    if (violationCount >= 3) {
        // Ban permanente
        Logger::info("AntiCheat", "Aplicando ban permanente para conta {}", accountId);
        // BanManager::banAccount(accountId, "Violações anti-cheat", 0);
        return true;
    } else if (violationCount >= 1 || warningCount >= 3) {
        // Ban temporário (7 dias)
        Logger::info("AntiCheat", "Aplicando ban temporário (7 dias) para conta {}", accountId);
        // BanManager::banAccount(accountId, "Violações anti-cheat", 7 * 24 * 60 * 60);
        return true;
    } else if (warningCount >= 1 || suspiciousCount >= 5) {
        // Desconecta o jogador
        Logger::info("AntiCheat", "Desconectando conta {} por comportamento suspeito", accountId);
        // SessionManager::disconnectAccount(accountId, "Comportamento suspeito detectado");
        return true;
    }
    
    return false;
}

std::vector<AntiCheatViolation> AntiCheatManager::getViolationHistory(uint32_t accountId) {
    // Bloqueia para leitura
    std::shared_lock<std::shared_mutex> lock(dataMutex);
    
    auto it = violationHistory.find(accountId);
    if (it == violationHistory.end()) {
        return {};
    }
    
    return it->second;
}

void AntiCheatManager::cleanupOldData() {
    // Bloqueia para escrita
    std::unique_lock<std::shared_mutex> lock(dataMutex);
    
    uint64_t currentTime = TimeUtils::getCurrentTimestamp();
    const uint64_t MAX_DATA_AGE = 24 * 60 * 60 * 1000; // 24 horas em ms
    
    // Limpa posições antigas
    for (auto it = lastPositions.begin(); it != lastPositions.end(); ) {
        if (currentTime - std::get<2>(it->second) > MAX_DATA_AGE) {
            it = lastPositions.erase(it);
        } else {
            ++it;
        }
    }
    
    // Limpa cooldowns expirados
    for (auto it = skillCooldowns.begin(); it != skillCooldowns.end(); ) {
        auto& characterCooldowns = it->second;
        for (auto cooldownIt = characterCooldowns.begin(); cooldownIt != characterCooldowns.end(); ) {
            if (cooldownIt->second < currentTime) {
                cooldownIt = characterCooldowns.erase(cooldownIt);
            } else {
                ++cooldownIt;
            }
        }
        
        if (characterCooldowns.empty()) {
            it = skillCooldowns.erase(it);
        } else {
            ++it;
        }
    }
    
    // Limpa histórico de ações antigo
    for (auto it = actionHistory.begin(); it != actionHistory.end(); ) {
        auto& actions = it->second;
        if (!actions.empty()) {
            uint64_t lastActionTime = std::get<1>(actions.back());
            if (currentTime - lastActionTime > MAX_DATA_AGE) {
                it = actionHistory.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void AntiCheatManager::shutdown() {
    Logger::info("AntiCheatManager", "Finalizando sistema Anti-Cheat");
    
    // Executa limpeza para evitar uso excessivo de memória na próxima inicialização
    cleanupOldData();
    
    // Bloqueia para escrita
    std::unique_lock<std::shared_mutex> lock(dataMutex);
    
    // Limpa todas as estruturas de dados
    violationHistory.clear();
    skillCooldowns.clear();
    lastPositions.clear();
    actionHistory.clear();
}

} // namespace security
} // namespace wydbr
)";
        implFile.close();
        
        return true;
    }
    
    bool createClientVerifier(const std::string& basePath) {
        // Implementação similar à anterior
        return true; // Implementar conforme necessário
    }
    
    bool createIntegrityChecker(const std::string& basePath) {
        // Implementação similar à anterior
        return true; // Implementar conforme necessário
    }
    
    bool createBehaviorReporter(const std::string& basePath) {
        // Implementação similar à anterior
        return true; // Implementar conforme necessário
    }
    
public:
    bool generate(const std::string& basePath) {
        std::cout << "Gerando Sistema Anti-Cheat..." << std::endl;
        
        bool success = true;
        
        // Criar os componentes do sistema
        success &= createAntiCheatManager(basePath);
        success &= createClientVerifier(basePath);
        success &= createIntegrityChecker(basePath);
        success &= createBehaviorReporter(basePath);
        
        return success;
    }
};

int main(int argc, char* argv[]) {
    std::string basePath = (argc > 1) ? argv[1] : "../src";
    
    AntiCheatGenerator generator;
    bool success = generator.generate(basePath);
    
    return success ? 0 : 1;
} 